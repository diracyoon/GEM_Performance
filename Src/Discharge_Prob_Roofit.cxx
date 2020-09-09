#include "Discharge_Prob_Roofit.h"

ClassImp(Discharge_Prob_Roofit);

//////////

Discharge_Prob_Roofit::Discharge_Prob_Roofit(const TString& a_gain_file) : n_gain_bin(1500), gain_range(150000), gain_file(a_gain_file), gain("gain", "gain", 0, gain_range), a("a", "a", 1.55e-4, 2e-5, 3e-4), b("b", "b", 1.46e-10, 1e-11, 1e-7), alpha("alpha", "alpha", 0, 2e9), discharge("discharge", "discharge", 0, 150), point(alpha, discharge, "point"), roo_data("data", "data", RooArgSet(gain, point)), max_iteration(2)  
{

  TString path_gain_file = "./Data/" + gain_file;
  fin_gain = new TFile(path_gain_file);

  fit_gain = *(TF1*)fin_gain->Get("Fit_HV_Current_Gain");

  //root file for output
  fout = new TFile("Discharge_Prob_Roofit.root", "RECREATE");

  workspace = new RooWorkspace("w");

  Read_Poisson_CL();
  Read_Count_Data();
  Get_Gain();
  Iteration();
  Contour();
  Variation();
  Visualization();
}//Discharge_Prob_Roofit::Discharge_Prob_Roofit(const TString& a_gain_file)

//////////

Discharge_Prob_Roofit::~Discharge_Prob_Roofit()
{
  fin_gain->Close();
  delete fin_gain;
  
  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data& data = vec_data[i];
      fout->cd();
      data.h_tp->Write();
      data.h_gain->Write();
      //data.h_iteration[0]->Write();
    }

  fout->cd();
  workspace->Write();
  
  fout->Close();
  delete fout;
}//Discharge_Prob_Roofit::~Discharge_Prob_Roofit()

//////////

void Discharge_Prob_Roofit::Contour()
{
  RooNLLVar nll("nll", "nll", *pdf[max_iteration-1], roo_data, false);
  RooMinimizer m(nll);
  m.migrad();

  RooPlot* cont = m.contour(a, b, 1, 2);
  gr_contour = (TGraph*)cont->findObject("contour_nll_n1.000000");
    
  TCanvas* can_contour = new TCanvas("Can_Contour", "Can_Countour", 800, 500);
  can_contour->Draw();

  can_contour->cd();
  cont->Draw();

  fout->cd();
  can_contour->Write();
  
  return;
}//void Discharge_Prob::Contour()

//////////

void Discharge_Prob_Roofit::Draw()
{
  //canvas = new TCanvas("canvas", "canvas", 800, 500);

  
  return;
}//void Discharge_Prob_Roofit::Draw()

//////////

void Discharge_Prob_Roofit::Fill_RooData(const Int_t& index)
{
  //reset roo_data
  roo_data.reset();

  //init
  if(index==0)
    {
      Int_t n_point = vec_data.size();
      for(Int_t i=0; i<n_point; i++)
	{
	  Data& data = vec_data[i];

	  gain = data.gain;
	  alpha = data.net_alpha_count;
	  discharge = data.discharge_count;

	  roo_data.add(RooArgSet(gain,  point));
	}
    }//init
  else
    {
      //construct gain*discharge prob histogram
      Int_t n_point = vec_data.size();
      for(Int_t i=0; i<n_point; i++)
	{
	  Data& data = vec_data[i];
	  
	  TH1D* histo = data.h_gain;
	  Int_t n_bin = histo->GetNbinsX();
	  
	  string h_name_iteration = "histo_iteration_" + to_string(i) + "_" + to_string((Int_t)round(data.current));
	  TH1D* histo_iteration = new TH1D(h_name_iteration.c_str(), h_name_iteration.c_str(), n_gain_bin, 0, gain_range);
	  
	  for(Int_t i=0; i<n_bin; i++)
	    {
	      //Double_t gain = histo->GetBinCenter(i+1);
	      Int_t bin_content = histo->GetBinContent(i+1);
	      if(bin_content!=0)
		{
		  gain = histo->GetBinCenter(i+1);
		  
		  Double_t bin_content_iteration = bin_content*fit_discharge_prob[index-1]->getValV();
		  
		  histo_iteration->SetBinContent(i+1, bin_content_iteration);
		  
		  //cout << "Gain = " <<  histo->GetBinCenter(i+1) << ", bin content = " << bin_content << ", func = " << fit_discharge_prob[index-1]->getValV() << ", bin = " << bin_content_iteration <<  endl;
		}
	    }
	  
	  //save
	  //data.h_iteration.push_back(histo_iteration);
	  subdir->cd();
	  histo_iteration->Write();

	  //discharge count rebalance
	  RooDataHist datahist("datahist", "datahist", gain, histo_iteration);
	  RooHistPdf pdf_iteration("pdf", "pdf", gain, datahist, 2);
	  
	  gain.setBins(n_gain_bin);
	  RooDataHist* discharge_gen = pdf_iteration.generateBinned(gain, data.discharge_count);
	  
	  //fill roodata
	  cout << "Data file " << data.current << endl; 
	  Int_t net_alpha_count = data.net_alpha_count;
	  Int_t entries = histo->GetEntries();
	  for(Int_t i=0; i<n_gain_bin; i++)
	    {
	      gain = histo->GetBinCenter(i+1);
	      
	      Int_t bin_content = histo->GetBinContent(i+1);
	      Int_t alpha_count_bin = (Double_t)bin_content/entries*net_alpha_count;
	      
	      Int_t discharge_bin = 0;
	      if(data.discharge_count!=0)
		{
		  discharge_gen->get(i);
		  discharge_bin = discharge_gen->weight();
		}
	      
	      if(alpha_count_bin!=0)
		{
		  //cout << histo->GetBinCenter(i+1) << " " << alpha_count_bin << " " << discharge_bin << endl;
		  
		  gain = histo->GetBinCenter(i+1);
		  alpha = alpha_count_bin;
		  discharge = discharge_bin;
		  
		  roo_data.add(RooArgSet(gain, point));
		}
	    }
	}//for loop over data 
    }//iteration loop
  
  return;
}//void Discharge_Prob_Roofit::Fill_RooData(const Int_t&)

//////////

void Discharge_Prob_Roofit::Find_Poisson_CL(const Int_t& count, Double_t* cl)
{
  Bool_t chk = false;
  for(Int_t i=0; i<vec_cl_data.size(); i++)
    {
      CL_Data cl_data = vec_cl_data[i];

      if(count==cl_data.count)
	{
	  cl[0] = cl_data.lower_cl;
	  cl[1] = cl_data.upper_cl;
	  
	  break;
	}
    }
  
  return;
}//void Discharge_Prob_Roofit::Find_Poisson_CL(const Int_t& count, Double_t* cl)

//////////

void Discharge_Prob_Roofit::Fit(const Int_t& index)
{
  cout << "Fit index = " << index << endl;
  
  string fit_name = "fit_discharge_prob_" + to_string(index);
  fit_discharge_prob[index] = new RooFormulaVar(fit_name.c_str(), "b*(exp(a*gain)-1)", RooArgList(a, b, gain));
   
  //fit
  string pdf_name = "pdf_" + to_string(index);
  pdf[index] = new PDF_Poisson(pdf_name.c_str(), pdf_name.c_str(), *fit_discharge_prob[index], point);
  pdf[index]->fitTo(roo_data, ConditionalObservables(gain));

  workspace->import(*fit_discharge_prob[index]);

  return;
}//void Discharge_Prob_Roofit::Fit(const Int_t& index)

//////////

void Discharge_Prob_Roofit::Get_Gain()
{
  TTree* tree = (TTree*)fin_gain->Get("T");

  Double_t temperature_ref;
  Double_t pressure_ref;

  tree->SetBranchAddress("temperature", &temperature_ref);
  tree->SetBranchAddress("pressure", &pressure_ref);

  tree->GetEntry(0);

  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data& data = vec_data[i];

      Float_t current = data.current;

      string h_name_tp = "histo_tp_" + to_string(i) + "_" + to_string((Int_t)round(current));
      data.h_tp = new TH1D(h_name_tp.c_str(), h_name_tp.c_str(), 1000, 0, 1);

      string h_name_gain = "histo_gain_" + to_string(i) + "_" + to_string((Int_t)round(current));
      data.h_gain = new TH1D(h_name_gain.c_str(), h_name_gain.c_str(), n_gain_bin, 0, gain_range);

      for(Int_t j=0; j<data.env_data.size(); j++)
	{
	  Float_t temperature = data.env_data[j].temperature;
	  Float_t pressure =  data.env_data[j].pressure;

	  data.h_tp->Fill(temperature/pressure);

	  Float_t correction_factor = (pressure_ref/temperature_ref)*(temperature/pressure);
	  Float_t current_corr = correction_factor*current;

	  Float_t gain = fit_gain.Eval(current_corr);

	  data.h_gain->Fill(gain);

	  //cout << temperature << " " << pressure << " " << correction_factor << " " << current_corr << " " << gain <<  endl;
	}

      data.gain = data.h_gain->GetMean();
      data.gain_error = data.h_gain->GetRMS();
    }

  return;
}//void Discharge_Prob_Roofit::Get_Gain()

//////////

void Discharge_Prob_Roofit::Read_Count_Data()
{
  ifstream fin;
  fin.open("./Data/Data.csv");

  string buf;
  getline(fin, buf);
  getline(fin, buf);
  while(!fin.eof())
    {
      getline(fin, buf);
      if(buf.compare("")==0) break;

      if(buf.substr(0, 1).compare("#")==0) continue;

      stringstream ss;
      ss.str(buf);

      Data data;

      getline(ss, buf, ',');
      data.voltage = stof(buf, nullptr);

      getline(ss, buf, ',');
      data.current = stof(buf, nullptr);

      getline(ss, buf, ',');
      data.alpha_count = stoi(buf, nullptr);

      getline(ss, buf, ',');
      data.discharge_count = stoi(buf, nullptr);

      getline(ss, buf, ',');
      data.start_time =  Parsing_Time(buf);

      getline(ss, buf, ',');
      data.end_time = Parsing_Time(buf);

      data.duration = difftime(data.end_time, data.start_time);

      vector<TString> dates;
      Return_Dates(data.start_time, data.end_time, dates);

      cout << "Start: " << data.start_time << ", End: " << data.end_time << endl;
      for(Int_t i=0; i<dates.size(); i++)
	{
	  TString env_data_path = "./Data/Env_Data/";
	  env_data_path += dates[i];
	  env_data_path += ".dat";

	  cout << env_data_path << endl;
	  Read_Env_Data env_data(env_data_path, data.env_data);
	}

      //chopping out unnessary env data
      vector<namespace_Env_Data::Data>::iterator start_iter;
      vector<namespace_Env_Data::Data>::iterator end_iter;
      for(auto i=data.env_data.begin(); i<data.env_data.end(); ++i)
	{
	  struct tm time = {0};

	  time.tm_year = i->year - 1900;
	  time.tm_mon = i->month - 1;
	  time.tm_mday = i->day;
	  time.tm_hour = i->hour;
	  time.tm_min = i->min;
	  time.tm_sec = i->sec;

	  time_t current_time = mktime(&time);

	  if(current_time<data.start_time) start_iter = i;
	  if(data.end_time<current_time)
	    {
	      end_iter = i;
	      break;
	    }
	}

      data.env_data.erase(end_iter, data.env_data.end());
      data.env_data.erase(data.env_data.begin(), start_iter);

      getline(ss, buf, ',');
      data.background_count = stoi(buf, nullptr);

      Double_t scale = data.duration/600.;
      data.net_alpha_count = data.alpha_count - scale*data.background_count;
      
      vec_data.push_back(data);
    }

  return;
}//void Discharge_Prob_Roofit::Read_Count_Data()

//////////

void Discharge_Prob_Roofit::Read_Poisson_CL()
{
  ifstream fin;
  fin.open("./Data/CL.dat");

  if(!fin.is_open())
    {
      cerr << "Can not find CL.dat" << endl;
      exit(1);
    }

  string buf;
  while(!fin.eof())
    {
      getline(fin, buf);
      if(buf.compare("")==0) break;

      cout << buf << endl;

      stringstream ss;
      ss.str(buf);

      CL_Data cl_data;

      getline(ss, buf, ' ');
      cl_data.count = stoi(buf, nullptr);

      getline(ss, buf, ' ');
      cl_data.lower_cl = stof(buf, nullptr);

      getline(ss, buf);
      cl_data.upper_cl = stof(buf, nullptr);

      vec_cl_data.push_back(cl_data);

      cout << cl_data.count << " " << cl_data.lower_cl << " " << cl_data.upper_cl << endl;
    }
  
  return;
}//void Discharge_Prob_Roofit::Read_Poisson_CL()

//////////

void Discharge_Prob_Roofit::Iteration()
{
  for(Int_t i=0; i<max_iteration; i++)
    {
      cout << "Start iteration " << i << endl;
      
      string dir_name = "Iteration_" + to_string(i);
      subdir = fout->mkdir(dir_name.c_str(), dir_name.c_str());
      
      Fill_RooData(i);
      Fit(i);
      Visualization(i);

      cout << "End iteration " << i << endl;
    }
  
  return;
}//void Discharge_Prob_Roofit::Iteration()

//////////

time_t Discharge_Prob_Roofit::Parsing_Time(const string& str)
{
  struct tm time = {0};
  time.tm_year = stoi(str.substr(0, 4), nullptr) - 1900;
  time.tm_mon = stoi(str.substr(4, 2), nullptr) - 1;
  time.tm_mday = stoi(str.substr(6, 2), nullptr);
  time.tm_hour = stoi(str.substr(9, 2), nullptr);
  time.tm_min = stoi(str.substr(12, 2), nullptr);

  return mktime(&time);
}//time_t Discharge_Prob_Roofit::Parsing_Time(const string& str)

//////////

void Discharge_Prob_Roofit::Return_Dates(const time_t& start_time, const time_t& end_time, vector<TString>& dates)
{
  struct tm* tm_start = localtime(&start_time);

  Int_t start_year = tm_start->tm_year+1900;
  Int_t start_mon = tm_start->tm_mon + 1;
  Int_t start_day = tm_start->tm_mday;

  struct tm* tm_end = localtime(&end_time);

  Int_t end_year = tm_end->tm_year+1900;
  Int_t end_mon = tm_end->tm_mon+1;
  Int_t end_day = tm_end->tm_mday;

  Int_t year = start_year;
  Int_t mon = start_mon;
  Int_t day = start_day;

  while(1)
    {
      Int_t compare_date = year*10000 + mon*100 + day;
      Int_t compare_end_date = end_year*10000 + end_mon*100 + end_day;

      if(compare_date>compare_end_date) break;

      string date = to_string(year) + "_";
      date += string(2 - to_string(mon).length(), '0') + to_string(mon);
      date += "_";
      date += string(2 - to_string(day).length(), '0') + to_string(day);

      dates.push_back(date);

      day++;

      Int_t month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      if( (year%400)==0 || ((year%100)!=0 && (year%4)==0) ) month_days[1] = 29;

      if(day>month_days[mon-1])
	{
	  day -= month_days[mon-1];
	  mon++;

	  if(mon>12)
	    {
	      mon -= 12;
	      year++;
	    }
	}
    }

  return;
}//void Discahrge_Prob::Return_Dates(const time_t& start_time, const time_t& end_time, vector<TString>& dates)

//////////

void Discharge_Prob_Roofit::Variation()
{
  cout<< "Variation around the contour" << endl;

  gr_conf = new TGraphAsymmErrors();
  gr_conf->SetTitle("Gr_Conf");
  gr_conf->SetName("Gr_Conf");
  
  Double_t* x = gr_contour->GetX();
  Double_t* y = gr_contour->GetY();

  RooFormulaVar variation_discharge_prob("Variation", "b*(exp(a*gain)-1)", RooArgList(a, b, gain)); 

  Int_t n_step = 500;
  Double_t step_size = gain_range/n_step;
  for(Int_t i=0; i<n_step; i++)
    {
      cout << "gain = " << step_size*(i+1) << endl;
      gain = step_size*(i+1);

      Double_t central = fit_discharge_prob[max_iteration-1]->getValV();
      
      Double_t cl[2] = {999, -999};
     
      for(Int_t j=0; j<gr_contour->GetN()-1; j++)
	{
	  a = x[j];
	  b = y[j];
	  
	  Double_t value = variation_discharge_prob.getValV();

	  cout << j+1 << " " << x[j] << " " << y[j] << " " << value << endl;

	  //lower bound
	  if(value < cl[0]) cl[0] = value;
	   
	  //upper bound
	  if(cl[1] < value) cl[1] = value;
	}//loop over contour
      cout << central << " " << cl[0] << " " << cl[1] << endl;

      gr_conf->SetPoint(i, step_size*(i+1), central);
      gr_conf->SetPointError(i, 0, 0, central-cl[0], cl[1]-central);
    }//loop over gain

  fout->cd();
  gr_conf->Write();

  return;
}//void Discharge_Prob_Roofit::Variation()

//////////

void Discharge_Prob_Roofit::Visualization()
{
  can_conf = new TCanvas("Can_Conf", "Can_Conf", 800, 500);
  can_conf->Draw();

  RooPlot* frame = gain.frame();
  fit_discharge_prob[max_iteration-1]->plotOn(frame);

  can_conf->cd();
  frame->Draw();

  frame->GetXaxis()->SetRangeUser(0.5e4, 1e5);
  frame->GetYaxis()->SetRangeUser(1e-9, 1e-4);

  gr_discharge_prob[0]->Draw("samep");
  //gr_discharge_prob[max_iteration-1]->Draw("samep");
  
  gr_conf->SetFillColor(6);
  gr_conf->SetFillStyle(3005);
  gr_conf->Draw("same3");
  
  can_conf->SetLogy();
  
  fout->cd();
  can_conf->Write();
  
  return;
}//void Discharge_Prob_Roofit::Visualization()

//////////

void Discharge_Prob_Roofit::Visualization(const Int_t& index)
{
  /*
  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data data = vec_data[i];

      Double_t net_alpha_count = data.net_alpha_count;
      Double_t discharge_count = data.discharge_count;
      
      //discharge prob
      Double_t discharge_prob = discharge_count/net_alpha_count;

      //error calculation
      //30 sec uncertainty is assinged for scale
      Double_t scale = data.duration/600.;
      Double_t scale_error = 30/600.;
      
      Double_t error_alpha = discharge_count/Power(net_alpha_count, 2)*Sqrt(data.alpha_count);
      Double_t error_sf = data.discharge_count/Power(net_alpha_count, 2)*data.background_count*scale_error;
      Double_t error_background = data.discharge_count/Power(net_alpha_count, 2)*scale*Sqrt(data.background_count);

      //asymmetric error for error from discharge count
      TFeldmanCousins f(0.68);
      f.SetMuMax(300);

      Double_t error_discharge_lower = f.CalculateLowerLimit(data.discharge_count, 0);
      Double_t error_discharge_upper = f.CalculateUpperLimit(data.discharge_count, 0);

      error_discharge_lower = data.discharge_count - error_discharge_lower;
      error_discharge_upper = error_discharge_upper - data.discharge_count;

      error_discharge_lower = error_discharge_lower/net_alpha_count;
      error_discharge_upper = error_discharge_upper/net_alpha_count;

      Double_t discharge_prob_error_lower = Sqrt(Power(error_alpha, 2.) + Power(error_sf, 2.) + Power(error_background, 2.) + Power(error_discharge_lower, 2.));
      Double_t discharge_prob_error_upper = Sqrt(Power(error_alpha, 2.) + Power(error_sf, 2.) + Power(error_background, 2.) + Power(error_discharge_upper, 2.));

      gr_discharge_prob.SetPoint(i, data.gain, discharge_prob);
      //gr_discharge_prob.SetPointError(i, gain_error, gain_error, discharge_prob_error_lower, discharge_prob_error_upper);
      gr_discharge_prob.SetPointError(i, 0, 0, discharge_prob_error_lower, discharge_prob_error_upper);
      }
  */

  gr_discharge_prob[index] = new TGraphAsymmErrors();
  
  Int_t n_point = roo_data.numEntries();
  for(Int_t i=0; i<n_point; i++)
    {
      const RooArgSet* data = roo_data.get(i);

      Double_t gain = data->getRealValue("gain");
      Int_t alpha_count = data->getRealValue("alpha");
      Int_t discharge_count = data->getRealValue("discharge");

      //cout << gain << " " << alpha_count << " " << discharge_count << endl;

      //discharge prob
      Double_t discharge_prob = (Double_t)discharge_count/(Double_t)alpha_count;

      //error calculation
      Double_t error_alpha = discharge_count/Power(alpha_count, 2)*Sqrt(alpha_count);

      //asymmetric error for error from discharge count
      //TFeldmanCousins f(0.68);
      //f.SetMuMax(300);

      //Double_t error_discharge_lower = discharge_count - f.CalculateLowerLimit(discharge_count, 0);
      //Double_t error_discharge_upper = f.CalculateUpperLimit(discharge_count, 0) - discharge_count;
      
      Double_t poisson_cl[2];
      Find_Poisson_CL(discharge_count, poisson_cl);
      
      Double_t error_discharge_lower = discharge_count - poisson_cl[0];
      Double_t error_discharge_upper = poisson_cl[1] - discharge_count;

      error_discharge_lower = error_discharge_lower/alpha_count;
      error_discharge_upper = error_discharge_upper/alpha_count;
      
      Double_t discharge_prob_error_lower = Sqrt(Power(error_alpha, 2.) + Power(error_discharge_lower, 2.));
      Double_t discharge_prob_error_upper = Sqrt(Power(error_alpha, 2.) + Power(error_discharge_upper, 2.));
      
      gr_discharge_prob[index]->SetPoint(i, gain, discharge_prob);
      gr_discharge_prob[index]->SetPointError(i, 0, 0, discharge_prob_error_lower, discharge_prob_error_upper);
    }

  string gr_name = "Gr_Discharge_Prob_" + to_string(index);
  gr_discharge_prob[index]->SetName(gr_name.c_str());
  gr_discharge_prob[index]->SetTitle(gr_name.c_str());

  subdir->cd();
  gr_discharge_prob[index]->Write();

  //Draw
  string can_name = "can_" + to_string(index);
  can[index] = new TCanvas(can_name.c_str(), can_name.c_str(), 800, 500);
  can[index]->Draw();

  RooPlot* frame = gain.frame();
  //frame->GetYaxis()->SetRangeUser(1e-9, 1e-9);
  
  fit_discharge_prob[index]->plotOn(frame);

  can[index]->cd();
  frame->Draw();
  gr_discharge_prob[index]->Draw("samep*");
  
  can[index]->SetLogy();

  frame->GetYaxis()->SetRangeUser(1e-9, 1e-4);
  
  subdir->cd();
  can[index]->Write();
  
  return;
}//void Discharge_Prob::Visualization()

//////////

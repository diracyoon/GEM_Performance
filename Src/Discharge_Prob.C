#include "Discharge_Prob.h"

ClassImp(Discharge_Prob);

//////////

Discharge_Prob::Discharge_Prob(const TString& a_gain_file) : gain_file(a_gain_file), fit_discharge_prob("DD", "expo", GAIN_RANGE_LOWER, GAIN_RANGE_UPPER), tl(0.15, 0.65, 0.35, 0.85)
{
  TString path_gain_file = "./Data/" + gain_file;
  fin = new TFile(path_gain_file);

  fit_gain = *(TF1*)fin->Get("Fit_HV_Current_Gain");
  conf_gain = *(TGraphErrors*)fin->Get("Conf_HV_Current_Gain");

  //root file for output
  fout = new TFile("Discharge_Prob.root", "RECREATE");
  
  Read_Count_Data();
  Get_Gain();  
  Calculate_Prob();
  //Calculate_CL();
  //Draw();
}//Discharge_Prob::Discharge_Prob(const TString& a_gain_file)

//////////

Discharge_Prob::~Discharge_Prob()
{
  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data& data = vec_data[i];
      data.h_gain->Write();
    }
  
  gr_discharge_prob.SetName("Gr_Gain_Discharge_Prob");
  gr_discharge_prob.SetTitle("Gr: Gain Vs. Discharge_Prob");
  gr_discharge_prob.GetXaxis()->SetTitle("Gain");
  gr_discharge_prob.GetYaxis()->SetTitle("Discharge Prob.");
  gr_discharge_prob.Write();

  fit_discharge_prob.SetName("Fit_Gain_Discharge_Prob");
  fit_discharge_prob.SetTitle("Fit: Gain Vs. Discharge_Prob");
  fit_discharge_prob.Write();

  gr_discharge_prob_conf.SetName("Conf_Gain_Discharge_Prob");
  gr_discharge_prob_conf.SetTitle("Discharge Prob Vs. Gain: 5.5 GeV");
  gr_discharge_prob_conf.Write();
  
  gr_discharge_prob_scale.SetName("Gr_Gain_Discharge_Prob_Scale");
  gr_discharge_prob_scale.SetTitle("Gain Vs. Discharge Prob Scale");
  gr_discharge_prob_scale.GetXaxis()->SetTitle("Gain/10000");
  gr_discharge_prob_scale.GetYaxis()->SetTitle("Discharge Prob.*10000");
  gr_discharge_prob_scale.Write();

  fit_discharge_prob_scale.SetName("Fit_Gain_Discharge_Prob_Scale");
  fit_discharge_prob_scale.SetTitle("Fit: Gain Vs. Discharge_Prob Scale");
  fit_discharge_prob_scale.Write();
  
  gr_discharge_prob_conf_scale.SetName("Conf_Gain_Discharge_Prob_Scale");
  gr_discharge_prob_conf_scale.SetTitle("Confidence Level: Gain Vs. Discharge Prob Scale");
  gr_discharge_prob_conf_scale.Write();
  
  canvas.SetName("Canvas");
  canvas.SetTitle("Canvas");
  canvas.Write();
  
  fout->Close();
}//Discharge_Prob::~Discharge_Prob()

//////////

void Discharge_Prob::Calculate_CL()
{
  //rescale
  //due to computational error and small discharge prob., covariant mat. can't be calculated correctly
  //that's why the below rescale
  const Double_t scale = 10000;

  Int_t n_point = gr_discharge_prob.GetN();
  Double_t* x = gr_discharge_prob.GetX();
  Double_t* ex = gr_discharge_prob.GetEX();
  Double_t* y = gr_discharge_prob.GetY();
  Double_t* ey = gr_discharge_prob.GetEY();
  for(Int_t i=0; i<n_point; i++)
    {
      gr_discharge_prob_scale.SetPoint(i, x[i]/scale, y[i]*scale);
      gr_discharge_prob_scale.SetPointError(i, ex[i]/scale, ey[i]*scale);
    }

  //gStyle->SetOptFit(1111);
  
  gr_discharge_prob_scale.Fit("expo");
  fit_discharge_prob_scale = *(TF1*)(gr_discharge_prob_scale.GetListOfFunctions()->FindObject("expo"));
 
  //calculate CL
  Int_t n_step = 500;
  Double_t range_lower = GAIN_RANGE_LOWER/scale;
  Double_t range_upper = GAIN_RANGE_UPPER/scale;
  
  for(Int_t i=0; i<n_step; i++)
    {
      Double_t x = x = (range_upper-range_lower)/(Double_t)n_step*(Double_t)i + range_lower;
      Double_t y = fit_discharge_prob_scale.Eval(x);
      
      gr_discharge_prob_conf_scale.SetPoint(i, x, y);
    }
  
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(&gr_discharge_prob_conf_scale, 0.68);
  
  //restore
  Double_t* parameter_scale;
  parameter_scale = fit_discharge_prob_scale.GetParameters();

  Double_t parameter[2];
  parameter[0] = parameter_scale[0] - Log(scale);
  parameter[1] = parameter_scale[1]/scale;

  fit_discharge_prob.SetParameters(parameter);
  
  Double_t* x_conf = gr_discharge_prob_conf_scale.GetX();
  Double_t* y_conf = gr_discharge_prob_conf_scale.GetY();
  Double_t* ey_conf = gr_discharge_prob_conf_scale.GetEY();
  for(Int_t i=0; i<n_step; i++)
    {
      gr_discharge_prob_conf.SetPoint(i, x_conf[i]*scale, y_conf[i]/scale);
      gr_discharge_prob_conf.SetPointError(i, 0, ey_conf[i]/scale);
    }
  
  return;
}//void Discharge_Prob::Calculate_CL()

//////////

void Discharge_Prob::Calculate_Prob()
{
  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data data = vec_data[i];

      Double_t scale = data.duration/600.;
      Double_t scale_error = 30/600.;
      
      Double_t net_count = data.alpha_count - scale*data.background_count;

      //discharge prob
      Double_t discharge_prob = data.discharge_count/net_count;

      //erro calculation
      Double_t common = data.discharge_count/Power(net_count, 2.);
      Double_t discharge_prob_error = data.alpha_count + Power(data.background_count*scale_error, 2.0) + Power(scale, 2.0)*data.background_count;
      discharge_prob_error = common*discharge_prob_error + 1;
      discharge_prob_error = common*discharge_prob_error;
      discharge_prob_error = Sqrt(discharge_prob_error);

      //gain and its error
      //Double_t val[2];
      //Get_Gain_Error(data.current, val);
      Double_t gain = data.gain;
      Double_t gain_error = data.gain_error;

      
      cout << scale << " " << scale_error << " " <<  net_count << " " << data.discharge_count << " " << discharge_prob << " " << discharge_prob_error  << gain << " " << gain_error << endl;

      gr_discharge_prob.SetPoint(i, gain, discharge_prob);
      gr_discharge_prob.SetPointError(i, gain_error, discharge_prob_error);
    }
  
  return;
}//void Discharge_Prob::Calculate_Prob()

/////////

void Discharge_Prob::Draw()
{
  gStyle->SetOptFit(111);
  gStyle->SetStatX(0.5);
  gStyle->SetStatW(0.2);
  gStyle->SetStatY(0.85);
  
  canvas.cd();

  gr_discharge_prob_scale.Draw("AP");

  canvas.Update();
  
  stats = (TPaveStats*)gr_discharge_prob_scale.FindObject("stats");
  
  gr_discharge_prob.Draw("AP");

  gr_discharge_prob_conf.SetFillColor(6);
  gr_discharge_prob_conf.SetFillStyle(3005);
  gr_discharge_prob_conf.SetTitle("Discharge Prob");
  gr_discharge_prob_conf.GetXaxis()->SetTitle("Effective Gas Gain");
  gr_discharge_prob_conf.GetXaxis()->SetLimits(GAIN_RANGE_LOWER, GAIN_RANGE_UPPER);
  gr_discharge_prob_conf.GetYaxis()->SetTitle("Discharge Probability");
  gr_discharge_prob_conf.GetYaxis()->SetRangeUser(1e-10, 1e-3);
  gr_discharge_prob_conf.Draw("a3");

  gr_discharge_prob.Draw("SAMEP");

  fit_discharge_prob.Draw("SAME");

  stats->SetX1NDC(0.45);
  stats->SetX2NDC(0.70);
  stats->SetY1NDC(0.65);
  stats->SetY2NDC(0.85);
  
  stats->SetBorderSize(0);
  stats->Draw("same");
  
  tl.AddEntry(&gr_discharge_prob, "Discharge Prob.", "lep");
  tl.AddEntry(&fit_discharge_prob, "Exponential fit", "l");
  tl.AddEntry(&gr_discharge_prob_conf, "68% C.L.", "f");

  tl.SetBorderSize(0);
  
  tl.Draw("SAME");

  //TLatex* latex = new TLatex(0.1,0.905,"GE1/1-X-S-KOREA-0004");
  //latex->SetNDC();
  //latex->SetLineWidth(2);
  //latex->Draw("SAME");
  
  canvas.SetLogy();

  canvas.Update();
  
  canvas.Print("canvas.png", "png");
  
  return;
}//void Discharge_Prob::Draw()

//////////

void Discharge_Prob::Get_Gain()
{
  TTree* tree = (TTree*)fin->Get("T");

  Double_t temperature_gain;
  Double_t pressure_gain;

  tree->SetBranchAddress("temperature", &temperature_gain);
  tree->SetBranchAddress("pressure", &pressure_gain);

  tree->GetEntry(0);

  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data& data = vec_data[i];

      Float_t current = data.current;

      string h_name = "histo_" + to_string((Int_t)current);
      data.h_gain = new TH1D(h_name.c_str(), h_name.c_str(), 1000, 5000, 200000);
      
      for(Int_t j=0; j<data.env_data.size(); j++)
	{
	  Float_t temperature = data.env_data[j].temperature;
	  Float_t pressure =  data.env_data[j].pressure;
	  
	  Float_t correction_factor = (pressure_gain/temperature_gain)*(temperature/pressure);
	  Float_t current_corr = correction_factor*current;

	  Float_t gain = fit_gain.Eval(current_corr);

	  data.h_gain->Fill(gain);
	  
	  //cout << temperature << " " << pressure << " " << correction_factor << " " << current_corr << " " << gain <<  endl;
	}

      data.gain = data.h_gain->GetMean();
      data.gain_error = data.h_gain->GetRMS();
    }

  return;
}//void Discharge_Prob::Get_Gain()

//////////

void Discharge_Prob::Get_Gain_Error(const Double_t& current, Double_t val[])
{
  Int_t n_point = conf_gain.GetN();
  Double_t* x = conf_gain.GetX();
  Double_t* y = conf_gain.GetY();
  Double_t* ey = conf_gain.GetEY();

  Double_t diff_prev = 1000;
  Int_t index = 0;
  for(Int_t i=0; i<n_point; i++)
    {
      Double_t diff = Abs(current - x[i]);

      if(diff_prev<diff)
	{
	  index = i-1;
	  break;
	}
      
      diff_prev = diff;
    }

  val[0] = y[index];
  val[1] = ey[index];
  
  return;
}//void Discharge_Prob::Get_Gain_Error()

//////////

time_t Discharge_Prob::Parsing_Time(const string& str)
{
  struct tm time = {0};
  time.tm_year = stoi(str.substr(0, 4), nullptr) - 1900;
  time.tm_mon = stoi(str.substr(4, 2), nullptr) - 1;
  time.tm_mday = stoi(str.substr(6, 2), nullptr);
  time.tm_hour = stoi(str.substr(9, 2), nullptr);
  time.tm_min = stoi(str.substr(12, 2), nullptr);

  return mktime(&time);
}//Int_t Discharge_Prob::Parsing_Time(const string& str)

//////////

void Discharge_Prob::Read_Count_Data()
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
      
      vec_data.push_back(data);
    }

  return;
}//void Discharge_Prob::Read_Count_Data() 

//////////

void Discharge_Prob::Return_Dates(const time_t& start_time, const time_t& end_time, vector<TString>& dates)
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

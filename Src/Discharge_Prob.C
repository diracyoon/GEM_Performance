#include "Discharge_Prob.h"

ClassImp(Discharge_Prob);

//////////

Discharge_Prob::Discharge_Prob(const TString& a_gain_file) : fit_discharge_prob("DD", "expo", GAIN_RANGE_LOWER, GAIN_RANGE_UPPER), tl(0.14, 0.5, 0.3, 0.6)
{
  //open root file for input gain curve 
  gain_file = a_gain_file;

  TString path_gain_file = "./Data/" + gain_file;
  fin = new TFile(path_gain_file);

  conf_gain = *(TGraphErrors*)fin->Get("Conf_HV_Current_Gain");

  //root file for output
  fout = new TFile("Discharge_Prob.root", "RECREATE");
  
  Read_Count_Data();
  Calculate_Prob();
  Calculate_CL();
  Draw();
}//Discharge_Prob::Discharge_Prob(const TString& a_gain_file)

//////////

Discharge_Prob::~Discharge_Prob()
{
  fin->Close();

  gr_discharge_prob.SetName("Gr_Gain_Discharge_Prob");
  gr_discharge_prob.SetTitle("Gain Vs. Discharge Prob");
  gr_discharge_prob.GetXaxis()->SetTitle("Gain");
  gr_discharge_prob.GetYaxis()->SetTitle("Discharge Prob.");
  gr_discharge_prob.Write();

  fit_discharge_prob.SetName("Fit_Gain_Discharge_Prob");
  fit_discharge_prob.SetTitle("Fit: Gain Vs. Discharge_Prob");
  fit_discharge_prob.Write();

  gr_discharge_prob_conf.SetName("Conf_Gain_Discharge_Prob");
  gr_discharge_prob_conf.SetTitle("Confidence Level: Gain Vs. Discharge Prob");
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
      Double_t val[2];
      Get_Gain_Error(data.current, val);

      cout << scale << " " << scale_error << " " <<  net_count << " " << data.discharge_count << " " << discharge_prob << " " << discharge_prob_error << " " << val[0] << " " << val[1] << endl;

      gr_discharge_prob.SetPoint(i, val[0], discharge_prob);
      gr_discharge_prob.SetPointError(i, val[1], discharge_prob_error);
    }
  
  return;
}//void Discharge_Prob::Calculate_Prob()

/////////

void Discharge_Prob::Draw()
{
  gStyle->SetOptFit(1111);
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
  gr_discharge_prob_conf.SetTitle("Gain Vs. Discharge Prob., 5.5 MeV #alpha");
  gr_discharge_prob_conf.GetXaxis()->SetTitle("Gain");
  gr_discharge_prob_conf.GetXaxis()->SetLimits(GAIN_RANGE_LOWER, GAIN_RANGE_UPPER);
  gr_discharge_prob_conf.GetYaxis()->SetTitle("Discharge Prob.");
  //gr_conf->GetYaxis()->SetRangeUser(1e-10*scale, 1e-2*scale);
  gr_discharge_prob_conf.Draw("a3");

  gr_discharge_prob.Draw("SAMEP");

  fit_discharge_prob.Draw("SAME");

  stats->Draw("same");
  
  tl.AddEntry(&gr_discharge_prob, "Discharge Prob.", "lep");
  tl.AddEntry(&fit_discharge_prob, "Exponential fit", "l");
  tl.AddEntry(&gr_discharge_prob_conf, "68% C.L.", "f");
  tl.Draw("SAME");

  canvas.SetLogy();
  
  return;
}//void Discharge_Prob::Draw()

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
  time.tm_mon = stoi(str.substr(4, 2), nullptr)-1;
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
      time_t start_time =  Parsing_Time(buf);
      
      getline(ss, buf, ',');
      time_t end_time = Parsing_Time(buf);

      data.duration = difftime(end_time, start_time);

      getline(ss, buf, ',');
      data.background_count = stoi(buf, nullptr);
      
      vec_data.push_back(data);
    }
  
}//void Discharge_Prob::Read_Count_Data() 

//////////

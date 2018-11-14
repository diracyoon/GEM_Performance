#include "Discharge_Prob.h"

ClassImp(Discharge_Prob);

//////////

Discharge_Prob::Discharge_Prob(const TString& a_gain_file)
{
  //open root file for input gain curve 
  gain_file = a_gain_file;

  TString path_gain_file = "./Data/" + gain_file;
  fin = new TFile(path_gain_file);

  conf_gain = (TGraphErrors*)fin->Get("Conf_HV_Current_Gain");

  //root file for output
  fout = new TFile("Discharge_Prob.root", "RECREATE");
  
  Read_Count_Data();
  Calculate_Prob();
}//Discharge_Prob::Discharge_Prob(const TString& a_gain_file)

//////////

Discharge_Prob::~Discharge_Prob()
{
  fin->Close();

  gr_gain_discharge_prob.SetName("Gr_Gain_Discharge_Prob");
  gr_gain_discharge_prob.SetTitle("Gain Vs. Discharge Prob");
  gr_gain_discharge_prob.Write();

  fout->Close();
}//Discharge_Prob::~Discharge_Prob()

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
           
      cout << scale << " " << scale_error << " " <<  net_count << " " << discharge_prob << " " << discharge_prob_error << endl;
    }
  
  return;
}//void Discharge_Prob::Calculate_Prob()

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

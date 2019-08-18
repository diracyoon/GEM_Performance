#include "Gain.h"

ClassImp(Gain);

//////////

Gain::Gain(const TString& a_path, const TString& a_target)
{
  path = a_path;
  target = a_target;
  
  fout = new TFile(target+".root", "RECREATE");

  tree = new TTree("T", "Tree for the environmental variables");
  tree->Branch("temperature", &temperature, "temperature/D");
  tree->Branch("pressure", &pressure, "pressure/D");
  tree->Branch("relative_humidity", &relative_humidity, "relative_humidity/D");
  tree->Branch("source", &source); 
  
  Read_Data();
  Calculate_Gain();
  Fit();
  Draw();
}//Gain::Gain(const TString& a_path, const TString& a_target)

//////////

Gain::~Gain()
{
  gr_rate.SetName("Gr_HV_Current_Rate");
  gr_rate.SetTitle("HV_Current vs Rate");
  gr_rate.Write();

  gr_gain.SetName("Gr_HV_Current_Gain");
  gr_gain.SetTitle("HV_Current vs Gain");
  gr_gain.Write();

  gr_gain_corr.SetName("Gr_HV_Current_Gain_Corr");
  gr_gain_corr.SetTitle("HV_Current vs Gain; Env effect is corrected");
  gr_gain_corr.Write();
  
  fit_gain.SetName("Fit_HV_Current_Gain");
  fit_gain.SetTitle("Fit: HV_Current vs Gain");
  fit_gain.Write();

  gr_gain_conf.SetName("Conf_HV_Current_Gain");
  gr_gain_conf.SetTitle("Confidence Level: HV_Current vs Gain");
  gr_gain_conf.Write();

  canvas.SetName("Canvas");
  canvas.SetTitle("Canvas");
  canvas.Write();

  tree->Write();
  
  fout->Close();
}//Gain::~Gain()

//////////

void Gain::Calculate_Gain()
{
  Data data = vec_data[ref_index];
  
  Int_t count_plateau = data.count_on - data.count_off;
  Float_t count_error_plateau = Sqrt(data.count_on + data.count_off);

  Float_t rate_plateau = count_plateau/60.;
  Float_t rate_error_plateau = count_error_plateau/60.;
  cout << "Rate Plateau = " << rate_plateau << endl;

  Double_t n_primary_electron;
  Double_t n_primary_electron_error;
  if(source=="XRAY")
    {
      n_primary_electron = N_PRIMARY_ELECTRON_XRAY;
      n_primary_electron_error = N_PRIMARY_ELECTRON_ERROR_XRAY;
    }
  else if(source=="Fe55")
    {
      n_primary_electron = N_PRIMARY_ELECTRON_FE55;
      n_primary_electron_error = N_PRIMARY_ELECTRON_ERROR_FE55;
    }
  
  Int_t n_point = vec_data.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data data = vec_data[i];

      Int_t hv_current = data.hv_current;
      Int_t hv_current_corr = data.hv_current_corr;
      
      Int_t count = data.count_on - data.count_off;
      Float_t count_error = Sqrt(data.count_on + data.count_off);

      Float_t rate = count/60.;
      Float_t rate_error = count_error/60.;
      
      Float_t ro_current = data.ro_current_on - data.ro_current_off;
      Float_t ro_current_error = Sqrt(Power(data.ro_current_error_on, 2.) + Power(data.ro_current_error_off, 2));

      Float_t gain = ro_current/rate_plateau/ELECTRON_CHARGE/n_primary_electron;

      Float_t gain_error = Power(ro_current_error/ELECTRON_CHARGE/rate_plateau/n_primary_electron, 2.);
      gain_error += Power(ro_current*((rate_error_plateau/rate_plateau)+(n_primary_electron_error/n_primary_electron))/ELECTRON_CHARGE/rate_plateau/n_primary_electron, 2.);
      gain_error = Sqrt(gain_error);

      gr_rate.SetPoint(i, hv_current, rate);
      gr_rate.SetPointError(i, 0, rate_error);
            
      gr_gain.SetPoint(i, hv_current, gain);
      gr_gain.SetPointError(i, 0, gain_error);

      gr_gain_corr.SetPoint(i, hv_current_corr, gain);
      gr_gain_corr.SetPointError(i, 0, gain_error);
      
      cout << hv_current << " " << hv_current_corr << " "  << rate << " " << ro_current << " " << gain << " " << gain_error << endl;
    }
  
  return;
}//void Gain::Calculate_Gain()

//////////

void Gain::Draw()
{
  gStyle->SetOptFit(1111);
  
  canvas.cd();
  
  gr_gain_conf.SetFillColor(6);
  gr_gain_conf.SetFillStyle(3005);
  gr_gain_conf.GetXaxis()->SetRangeUser(540, 710);
  gr_gain_conf.GetXaxis()->SetTitle("Divider current [#muA]");
  gr_gain_conf.GetYaxis()->SetRangeUser(1e1, 1e5);
  gr_gain_conf.GetYaxis()->SetTitle("Gain");
  gr_gain_conf.Draw("a3");

  gr_gain.Draw("SAMEP");

  canvas.Update();

  stats = (TPaveStats*)gr_gain.FindObject("stats");
  stats->SetX1NDC(0.15);
  stats->SetX2NDC(0.45);
  stats->SetY1NDC(0.70);
  stats->SetY2NDC(0.85);
    
  canvas.SetLogy();
  
  return;
}//void Gain::Draw()

//////////

void Gain::Fit()
{
  gr_gain.Fit("expo", "S", "", 600, 740);
  fit_gain = *((TF1*)gr_gain.GetListOfFunctions()->FindObject("expo"));

  Int_t n_point = 100;
  Float_t range_lower = 550;
  Float_t range_upper = 900;
  
  for(Int_t i=0; i<n_point; i++)
    {
      Double_t x = x = (range_upper-range_lower)/(Double_t)n_point*(Double_t)i + range_lower;
      Double_t y = fit_gain.Eval(x);

      gr_gain_conf.SetPoint(i, x, y);
    }
  
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(&gr_gain_conf, 0.68);
  
  return;
}//void Gain::Fit()

//////////

void Gain::Read_Data()
{
  cout << "Read data..." << endl;
  
  TString count_data_path = path + "/Data/" + target + "/Data.csv";
  
  ifstream count_data;
  count_data.open(count_data_path.Data());
  
  string buf;

  //erase title row
  getline(count_data, buf);

  getline(count_data, buf);
  stringstream ss;
  ss.str(buf);
  
  getline(ss, buf, ',');
  pressure = stof(buf, nullptr);

  getline(ss, buf, ',');
  temperature = stof(buf, nullptr) + 273.15;

  Double_t correction_factor = (temperature/pressure)*(pressure_ref/temperature_ref);
  
  cout << "Temperaure     = " << temperature << ", Pressure    = " << pressure << endl;
  cout << "Temperaure_ref = " << temperature_ref << ", Pressure_ref = " << pressure_ref << endl;
  cout << "PT correction factor = " << correction_factor << endl;

  getline(ss, buf, ',');
  relative_humidity = stof(buf, nullptr);
  
  getline(ss, buf, ',');
  source = buf;
  cout << "Source = " << source << endl;

  tree->Fill();
  
  getline(ss, buf, ',');
  ref_index = stoi(buf, nullptr);

  //erase title row
  getline(count_data, buf);
  while(!count_data.eof())
    {
      getline(count_data, buf);
      if(buf.compare("")==0) break;
      
      stringstream ss;
      ss.str(buf);

      Data data;
      
      getline(ss, buf, ',');
      data.voltage = stoi(buf, nullptr);
      data.voltage_corr = correction_factor*data.voltage;
      
      getline(ss, buf, ',');
      data.hv_current = stoi(buf, nullptr);
      data.hv_current_corr = correction_factor*data.hv_current;
      
      getline(ss, buf, ',');
      data.count_off = stoi(buf, nullptr);

      getline(ss, buf, ',');
      data.count_on = stoi(buf, nullptr);
      
      TString ro_data_off_path = path + "/Data/" + target + "/Off/" + data.hv_current + ".txt";
      
      Read_RO ro_off(ro_data_off_path);
      data.ro_current_off = ro_off.Get_Mean();
      data.ro_current_error_off = ro_off.Get_Mean_Error();
      cout << "read_off " << data.ro_current_off << " " << data.ro_current_error_off << endl;

      TString ro_data_on_path = path + "/Data/" + target + "/On/" + data.hv_current + ".txt";
      cout << ro_data_on_path << endl;
      Read_RO ro_on(ro_data_on_path);
      data.ro_current_on = ro_on.Get_Mean();
      data.ro_current_error_on = ro_on.Get_Mean_Error();
      cout << "read on " << data.ro_current_on << " " << data.ro_current_error_on << endl;
        
      vec_data.push_back(data);

      cout << data.hv_current << " " << data.hv_current_corr << endl;
    }

  sort(vec_data.begin(), vec_data.end(), Compare_Data);
  
  cout << "Reading done." << endl;
  
  return;
}//void Gain::Read_Data()

//////////

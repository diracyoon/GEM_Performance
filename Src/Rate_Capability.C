#include "Rate_Capability.h"

ClassImp(Rate_Capability);

//////////

Rate_Capability::Rate_Capability(const TString& a_hv_current, const TString& a_path) : hv_current(a_hv_current), path(a_path)
{
  gStyle->SetOptFit(1111);

  fout = new TFile("Rate_Capability_" + hv_current + ".root", "RECREATE");

  Read_Attenuation_Data();
  Calculate_Attenuation_Factor();
}//Rate_Capability::Rate_Capability()

//////////

Rate_Capability::~Rate_Capability()
{
  fout->Close();
}//~Rate_Capability::Rate_Capability()

//////////

void Rate_Capability::Calculate_Attenuation_Factor()
{
  gr_rate_current.SetName("Gr_Rate_Current");
  gr_rate_current.SetTitle("Rate vs Current");
  gr_rate_current.GetXaxis()->SetTitle("Rate");
  gr_rate_current.GetYaxis()->SetTitle("1e9*Current");

  gr_n_layer_rate.SetName("Gr_N_Layer_Rate");
  gr_n_layer_rate.SetTitle("#Layer vs. Rate");
  gr_n_layer_rate.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_rate.GetYaxis()->SetTitle("Rate");

  gr_n_layer_current.SetName("Gr_N_Layer_Current");
  gr_n_layer_current.SetTitle("#Layer vs. Current");
  gr_n_layer_current.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_current.GetYaxis()->SetTitle("1e9*Current");

  gr_n_layer_attenuation_factor_rate.SetName("Gr_N_Layer_Attenuation_Factor_Rate");
  gr_n_layer_attenuation_factor_rate.SetTitle("#Layer vs. Attenuation_Factor (Rate)");
  gr_n_layer_attenuation_factor_rate.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_attenuation_factor_rate.GetYaxis()->SetTitle("Attenuation_Factor (Count)");

  gr_n_layer_attenuation_factor_current.SetName("Gr_N_Layer_Attenuation_Factor_Current");
  gr_n_layer_attenuation_factor_current.SetTitle("#Layer vs. Attenuation_Factor (Current)");
  gr_n_layer_attenuation_factor_current.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_attenuation_factor_current.GetYaxis()->SetTitle("Attenuation_Factor (Current)");

  Int_t daq_time = map_attenuation[1].daq_time;
  
  Double_t count_1 = map_attenuation[1].count_on - map_attenuation[1].count_off;
  Double_t count_error_1 = TMath::Sqrt(map_attenuation[1].count_on + map_attenuation[1].count_off);

  Double_t rate_1 = count_1/daq_time;
  Double_t rate_error_1 = count_error_1/daq_time;
  
  Double_t current_1 = map_attenuation[1].current_on - map_attenuation[1].current_off;
  Double_t current_error_1 = TMath::Sqrt(TMath::Power(map_attenuation[1].current_error_on, 2.0)+TMath::Power(map_attenuation[1].current_error_off, 2.0));

  for(auto it=map_attenuation.begin(); it!=map_attenuation.end(); it++)
    {
      Int_t n_layer = it->first;

      Int_t daq_time = map_attenuation[n_layer].daq_time;
      
      Double_t count = map_attenuation[n_layer].count_on - map_attenuation[n_layer].count_off;
      Double_t count_error = TMath::Sqrt(map_attenuation[n_layer].count_on + map_attenuation[n_layer].count_off);

      Double_t rate = count/daq_time;
      Double_t rate_error = count_error/daq_time;
      
      Double_t current = map_attenuation[n_layer].current_on - map_attenuation[n_layer].current_off;
      Double_t current_error = TMath::Sqrt(TMath::Power(map_attenuation[n_layer].current_error_on, 2.0)+TMath::Power(map_attenuation[n_layer].current_error_off, 2.0));
      
      Int_t n_point = gr_rate_current.GetN();

      gr_rate_current.SetPoint(n_point, rate, 1e9*current);
      gr_rate_current.SetPointError(n_point, rate_error, 1e9*current_error);

      gr_n_layer_rate.SetPoint(n_point, n_layer, rate);
      gr_n_layer_rate.SetPointError(n_point, 0, rate_error);

      gr_n_layer_current.SetPoint(n_point, n_layer, 1e9*current);
      gr_n_layer_current.SetPointError(n_point, 0, 1e9*current_error);

      Double_t attenuation_factor_rate = rate/rate_1;
      Double_t attenuation_factor_rate_error = TMath::Sqrt(TMath::Power(rate_error/rate_1, 2) + TMath::Power(rate*rate_error_1/TMath::Power(rate_1, 2.0), 2.0));

      gr_n_layer_attenuation_factor_rate.SetPoint(n_point, n_layer, attenuation_factor_rate);
      gr_n_layer_attenuation_factor_rate.SetPointError(n_point, 0, attenuation_factor_rate_error);

      Double_t attenuation_factor_current = current/current_1;
      Double_t attenuation_factor_current_error = TMath::Sqrt(TMath::Power(current_error/current_1, 2) + TMath::Power(current*current_error_1/current_1/current_1, 2.0));

      gr_n_layer_attenuation_factor_current.SetPoint(n_point, n_layer, attenuation_factor_current);
      gr_n_layer_attenuation_factor_current.SetPointError(n_point, 0, attenuation_factor_current_error);
    }
    
  //Write
  fout->cd();

  gr_rate_current.Write();
  gr_n_layer_rate.Write();
  gr_n_layer_current.Write();
  gr_n_layer_attenuation_factor_rate.Write();
  gr_n_layer_attenuation_factor_current.Write();

  //fit
  gr_rate_current.Fit("pol1", "FS0", "", 0, 8e6);
  gr_n_layer_rate.Fit("expo", "S", "", 2, 12);

  fit_n_layer_rate = (TF1*)(gr_n_layer_rate.GetListOfFunctions()->FindObject("expo"));

  //print
  TCanvas canvas("canvas", "canvas", 1200, 800);
  canvas.Divide(1, 2);
  canvas.Draw();

  canvas.cd(1);
  gr_rate_current.Draw("AP*");

  canvas.cd(2);
  gr_n_layer_rate.Draw("AP*");

  canvas.GetPad(2)->SetLogy();

  canvas.Print("Attenuation.png", "png");
  
  return;
}//void Rate_Capability::Calculate_Attenuation_Factor()

//////////

void Rate_Capability::Calculate_Expected_Rate(const Int_t& n_layer)
{
  TGraphErrors& gr_xray_current_expected_rate = map_gr_xray_current_expected_rate[n_layer];
  gr_xray_current_expected_rate.SetName("Gr_Xray_Current_Expected_Rate_" + TString(to_string(n_layer)) + "Layer");
  gr_xray_current_expected_rate.SetTitle("Xray current vs Expected Rate (" + TString(to_string(n_layer)) + "Layer)");

  Int_t n_point = map_data[n_layer].points.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data_Point point = map_data[n_layer].points[i];
      
      Int_t xray_current = point.xray_current;

      Double_t expected_rate;
      Double_t expected_rate_error;
      
      if(n_layer==10) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "SELF");
      else if(n_layer==4) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "FIT_SELF", );
      else if(n_layer==1) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "FIT_SELF", 50);
      else if(n_layer==0) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "RATE_ATTENUATION");
      
      gr_xray_current_expected_rate.SetPoint(i, xray_current, expected_rate);
      gr_xray_current_expected_rate.SetPointError(i, 0, expected_rate_error); 
    }

  fout->cd();
  gr_xray_current_expected_rate.Write();

  return;
}//void Rate_Capability::Calculate_Expected_Rate(const Int_t& n_layer)

//////////

void Rate_Capability::Calculate_Measured_Rate(const Int_t& n_layer)
{
  TGraphErrors& gr_xray_current_measured_rate = map_gr_xray_current_measured_rate[n_layer];
  gr_xray_current_measured_rate.SetName("Gr_Xray_Current_Measured_Rate_" + TString(to_string(n_layer)) + "Layer");
  gr_xray_current_measured_rate.SetTitle("Xray current vs Measured Rate (" + TString(to_string(n_layer)) + "Layer)");

  Int_t n_point = map_data[n_layer].points.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data_Point point = map_data[n_layer].points[i];
      
      Int_t xray_current = point.xray_current;
      Double_t daq_time = point.daq_time;
      
      //calculate measured rate
      Double_t measured_count = point.count_on - point.count_off;
      Double_t measured_count_error = TMath::Sqrt(point.count_on + point.count_off);

      Double_t measured_rate = measured_count/daq_time;
      Double_t measured_rate_error = measured_count_error/daq_time;

      gr_xray_current_measured_rate.SetPoint(i, xray_current, measured_rate);
      gr_xray_current_measured_rate.SetPointError(i, 0, measured_rate_error);
    }

  fout->cd();
  gr_xray_current_measured_rate.Write();
  
  return;
}//void Rate_Capability::Calculate_Measured_Rate(const Int_t& n_layer)

//////////

void Rate_Capability::Calculate_Single_Layer_Gain(const Int_t& n_layer)
{  
  //calculate gain
  TGraphErrors gr_flux_gain;
  gr_flux_gain.SetName("Gr_Flux_Gain_" + TString(to_string(n_layer)) + "Layer");
  gr_flux_gain.SetTitle("Flux vs Gain (" + TString(to_string(n_layer)) + "Layer)");

  Int_t n_point = map_data[n_layer].points.size();
  for(Int_t i=0; i<n_point; i++)
    {
      Data_Point point = map_data[n_layer].points[i];
      
      Int_t xray_current = point.xray_current;
      
      Double_t current = point.current_on - point.current_off;
      Double_t current_error = TMath::Sqrt(TMath::Power(point.current_error_off, 2.0) + TMath::Power(point.current_error_on, 2.0));
  
      Double_t rate = 0;
      Double_t rate_error = 0;

      GetY(map_gr_xray_current_expected_rate[n_layer], xray_current, rate, rate_error);
           
      Double_t gain = current/rate/N_PRIMARY_ELECTRON_XRAY/ELECTRON_CHARGE;
      Double_t gain_error = TMath::Sqrt(TMath::Power(current_error/rate, 2.0)+TMath::Power(current*rate_error/rate/rate, 2.0))/N_PRIMARY_ELECTRON_XRAY/ELECTRON_CHARGE;
      
      Double_t flux = rate/3.14;
      Double_t flux_error = rate_error/3.14;
      
      gr_flux_gain.SetPoint(i, flux, gain);
      gr_flux_gain.SetPointError(i, flux_error, gain_error);
    }

  map_gr_flux_gain[n_layer] = gr_flux_gain;
  
  //Write
  fout->cd();
  gr_flux_gain.Write();
 
  return;
}//void Rate_Capability::Calculate_Single_Layer_Gain(const Int_t& n_layer)

//////////

void Rate_Capability::Read_Attenuation_Data()
{
  //Read data file
  ifstream fin;
  fin.open(path+"/Data_Attenuation/GE11/Attenuation.csv");

  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find Attenuation.csv. Check it first!!" << endl;
      exit(1);
    }

  TString fin_name_off = path+"/Data_Attenuation/GE11/Background.txt";

  Read_RO read_ro_off(fin_name_off); 

  Double_t current_off = read_ro_off.Get_Mean();
  Double_t current_error_off = read_ro_off.Get_Mean_Error();
  
  string buf;
  getline(fin, buf);
  while(!fin.eof())
    {
      getline(fin, buf);
      if(buf.compare("")==0) break;

      Data_Attenuation data;
      
      sscanf(buf.c_str(), "%d,%d,%d,%d", &data.n_layer, &data.daq_time, &data.count_off, &data.count_on);

      data.current_off = current_off;
      data.current_error_off = current_error_off;
      
      TString fin_name_on = path + "/Data_Attenuation/GE11/";
      fin_name_on += data.n_layer;
      fin_name_on += "Layers.txt";

      Read_RO read_ro_on(fin_name_on);

      data.current_on = read_ro_on.Get_Mean();
      data.current_error_on = read_ro_on.Get_Mean_Error();
      
      map_attenuation[data.n_layer] = data;
    }

  return;
}//void Rate_Capability::Read_Attenuation_Data()

//////////

void Rate_Capability::Read_Single_Layer_Data(const Int_t& n_layer)
{
  //read data
  TString target_data = path + "/Data_Rate_Capability/GE11_0003/" + hv_current + "/" + to_string(n_layer) + "Layers/Rate_Capability_" + hv_current + "_" + to_string(n_layer) + "Layers.csv";

  ifstream fin;
  fin.open(target_data);

  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find " << target_data << ". Check it first." << endl;
      exit(1);
    }
  
  Data data;
  
  string buf;
  getline(fin, buf);
  while(!fin.eof())
    {
      getline(fin, buf);
      if(buf.compare("")==0) break;

      Data_Point point;

      sscanf(buf.c_str(), "%d,%d,%d,%d,%d", &point.xray_current, &point.count_off, &point.count_on, &point.daq_time, &point.resolution);
      
      TString fin_name_off = path + "/Data_Rate_Capability/GE11_0003/" + hv_current + "/" + to_string(n_layer) + "Layers/Background.txt";
      
      Read_RO read_ro_off(fin_name_off, point.resolution);
      
      point.current_off = read_ro_off.Get_Mean();
      point.current_error_off = read_ro_off.Get_Mean_Error();
            
      TString fin_name_on = path + "/Data_Rate_Capability/GE11_0003/" + hv_current + "/" + to_string(n_layer) + "Layers/" + to_string(point.xray_current) + "uA.txt";
      
      Read_RO read_ro_on(fin_name_on, point.resolution);

      point.current_on = read_ro_on.Get_Mean();
      point.current_error_on = read_ro_on.Get_Mean_Error();
      
      data.points.push_back(point);    
    }

  map_data[n_layer] = data;

  return;
}//void Rate_Capability::Read_Single_Layer_Data(const Int_t& n_layer)

//////////
/*
  void Rate_Capability::Renormalize_Attenuation_Gain()
  {
  Int_t n_layer_low = -1;
  Int_t n_layer_up = -1;
  
  TGraphErrors* gr_low = NULL;
  TGraphErrors* gr_up = NULL;
  for(auto it = map_gr_flux_gain.rbegin(); it!=map_gr_flux_gain.rend(); it++)
    {
      n_layer_low = n_layer_up;
      gr_low = gr_up;
	
      n_layer_up = it->first;
      gr_up = &it->second;

      if(gr_low==NULL)
	{
	  map_gr_flux_gain_renormal[n_layer_up] = *gr_up;
	  	  
	  continue;
	}
      
      Double_t* flux = gr_up->GetX();
      Double_t* flux_error = gr_up->GetEX();
      
      Double_t* gain_up = gr_up->GetY();
      Double_t* gain_error_up = gr_up->GetEY();
      
      Double_t gain_low = map_gr_flux_gain_renormal[n_layer_low].Eval(flux[0]);
      Double_t attenuation_renormal = gain_up[0]/gain_low;
      
      TGraphErrors gr_flux_gain_renormal;
      gr_flux_gain_renormal.SetName("Gr_Flux_Gain_Renormalized_Attenuation_" + TString(to_string(n_layer_up)) + "Layer");
      gr_flux_gain_renormal.SetTitle("Flux vs Gain (Renormalized, " + TString(to_string(n_layer_up)) + "Layer)");
      
      Int_t n_point = gr_up->GetN();
      for(Int_t i=0; i<n_point; i++)
	{
	  Double_t flux_renormal = flux[i];//attenuation_renormal;
	  Double_t flux_error_renormal = flux_error[i];//attenuation_renormal;
	  
	  Double_t gain_renormal = gain_up[i]/attenuation_renormal;
	  Double_t gain_error_renormal = gain_error_up[i]/attenuation_renormal;
	  
	  gr_flux_gain_renormal.SetPoint(i, flux_renormal, gain_renormal);
	  gr_flux_gain_renormal.SetPointError(i, flux_error_renormal, gain_error_renormal);
	}
      
      map_gr_flux_gain_renormal[n_layer_up] = gr_flux_gain_renormal;
      
      fout->cd();
      gr_flux_gain_renormal.Write();
    }
    
  return;
}//void Rate_Capability::Renormalize_Attenuation_Gain()
*/
//////////

void Rate_Capability::Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode, const Int_t& range)
{
  Double_t attenuation_factor, attenuation_factor_error;
  
  if(mode=="SELF")
    {
      GetY(map_gr_xray_current_measured_rate[n_layer], xray_current, rate, rate_error);
      
      return;
    }
  else if(mode=="FIT_SELF")
    {
      if(map_fit_xray_current_expected_rate[n_layer]==NULL)
	{
	  map_gr_xray_current_measured_rate[n_layer].Fit("pol1", "F", "", 0, range);
	  map_fit_xray_current_expected_rate[n_layer] = (TF1*)(map_gr_xray_current_measured_rate[n_layer].GetListOfFunctions()->FindObject("pol1"));

	  Int_t n_point = map_gr_xray_current_measured_rate[n_layer].GetN();
	  Double_t* x = map_gr_xray_current_measured_rate[n_layer].GetX();
	  for(Int_t i=0; i<n_point; i++){ map_CI_expected_rate[n_layer].SetPoint(i, x[i], map_fit_xray_current_expected_rate[n_layer]->Eval(xray_current)); }
	  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(&map_CI_expected_rate[n_layer], 0.68);
	}

      GetY(map_CI_expected_rate[n_layer], xray_current, rate, rate_error);
            
      return;
    }
  else if(mode=="RATE_ATTENUATION") GetY(gr_n_layer_attenuation_factor_rate, n_layer, attenuation_factor, attenuation_factor_error);
  else if(mode=="CURRENT_ATTENUATION") GetY(gr_n_layer_attenuation_factor_current, n_layer, attenuation_factor, attenuation_factor_error);

  //attenuation_factor_error = 0.1*attenuation_factor;
  cout << n_layer << " " << attenuation_factor << " " << attenuation_factor_error << endl;
  
  Double_t reference_rate, reference_rate_error;
  GetY(map_gr_xray_current_expected_rate[1], xray_current, reference_rate, reference_rate_error);
  
  //in case no referece points in 1 layer measurement
  if(reference_rate<0) reference_rate = map_gr_xray_current_expected_rate[1].Eval(xray_current);      
  
  rate = attenuation_factor*reference_rate;
  rate_error = TMath::Sqrt(TMath::Power(attenuation_factor_error*reference_rate, 2.0) + TMath::Power(attenuation_factor*reference_rate_error, 2.0));
  
  return;
}//void Rate_Capability::Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode)

//////////

void Rate_Capability::GetY(const TGraphErrors& graph, const Double_t& x, Double_t& y, Double_t& ey)
{
  //stupid. Why root doesn't support this method.

  y = -1;
  ey = -1;
  
  Int_t n_point = graph.GetN();
  for(Int_t i=0; i<n_point; i++)
    {
      Double_t search_x = 1;
      Double_t search_y = 1;
      
      graph.GetPoint(i, search_x, search_y);
      
      if(search_x==x)
	{
	  y = search_y;
	  ey = graph.GetErrorY(i);

	  break;
	}
    }
  
  return;
}//Double_T Rate_Capability::GetY(const TGraphErrors& graph, const Int_t& xray_current, Double_t& y, Double_t& ey)

//////////


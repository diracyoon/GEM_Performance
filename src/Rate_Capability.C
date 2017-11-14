#include "Rate_Capability.h"

ClassImp(Rate_Capability);

//////////

Rate_Capability::Rate_Capability(const TString& a_hv_current, const TString& a_path) : hv_current(a_hv_current), path(a_path)
{
  gStyle->SetOptFit(1111);

  fout = new TFile("Rate_Capability_" + hv_current + ".root", "RECREATE");

  Read_Attenuation();
}//Rate_Capability::Rate_Capability()

//////////

Rate_Capability::~Rate_Capability()
{
  fout->Close();
}//~Rate_Capability::Rate_Capability()

//////////

void Rate_Capability::Calculate_Expected_Rate(const Int_t& n_layer)
{
  TGraphErrors gr_xray_current_expected_rate;
  gr_xray_current_expected_rate.SetName("Gr_Xray_Current_Expected_Rate_" + TString(to_string(n_layer)) + "Layer");
  gr_xray_current_expected_rate.SetTitle("Xray current vs Expected Rate (" + TString(to_string(n_layer)) + "Layer)");

  Int_t n_point = map_xray_current[n_layer].size();
  for(Int_t i=0; i<n_point; i++)
    {
      Int_t xray_current = map_xray_current[n_layer].at(i);

      Double_t expected_rate;
      Double_t expected_rate_error;

      //for layer 10, and layer 4 and xray_current < 40 use count for attenuation 
      if(n_layer==10 || (n_layer==4 && xray_current<30)) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "COUNT_MEASUREMENT");
      else if(n_layer==4 && 30<xray_current) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "FIT_SELF");
      else if(n_layer==1) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "COUNT_MEASUREMENT");
      else if(n_layer==0) Get_Rate(xray_current, n_layer, expected_rate, expected_rate_error, "CURRENT_MEASUREMENT");
      
      
      gr_xray_current_expected_rate.SetPoint(i, xray_current, expected_rate);
      gr_xray_current_expected_rate.SetPointError(i, 0, expected_rate_error); 
    }

  map_gr_xray_current_expected_rate[n_layer] = gr_xray_current_expected_rate;

  fout->cd();
  gr_xray_current_expected_rate.Write();

  return;
}//void Rate_Capability::Calculate_Expected_Rate(const Int_t& n_layer)

//////////

void Rate_Capability::Calculate_Measured_Rate(const Int_t& n_layer)
{
  //reference rate
  TGraphErrors gr_xray_current_measured_rate;
  gr_xray_current_measured_rate.SetName("Gr_Xray_Current_Measured_Rate_" + TString(to_string(n_layer)) + "Layer");
  gr_xray_current_measured_rate.SetTitle("Xray current vs Measured Rate (" + TString(to_string(n_layer)) + "Layer)");


  Int_t n_point = map_xray_current[n_layer].size();
  for(Int_t i=0; i<n_point; i++)
    {
      Int_t xray_current = map_xray_current[n_layer].at(i);

      //calculate measured rate
      Double_t measured_count = map_count_on[n_layer].at(i) - map_count_off[n_layer].at(i);
      Double_t measured_count_error = TMath::Sqrt(map_count_on[n_layer].at(i) + map_count_off[n_layer].at(i));

      Double_t measured_rate = measured_count/60;
      Double_t measured_rate_error = measured_count_error/60.;

      //save reference rate for easy handling
      if(n_layer==10)
	{
	  map_reference_rate[xray_current] = measured_rate;
	  map_reference_rate_error[xray_current] = measured_rate_error;
	}

      gr_xray_current_measured_rate.SetPoint(i, xray_current, measured_rate);
      gr_xray_current_measured_rate.SetPointError(i, 0, measured_rate_error);
    }

  map_gr_xray_current_measured_rate[n_layer] = gr_xray_current_measured_rate;

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
  
  Int_t n_point = map_xray_current[n_layer].size();
  for(Int_t i=0; i<n_point; i++)
    {
      Int_t xray_current = map_xray_current[n_layer].at(i);
      
      Double_t current = map_current_on[n_layer].at(i) - map_current_off[n_layer].at(i);
      Double_t current_error = TMath::Sqrt(TMath::Power(map_current_error_on[n_layer].at(i), 2.0) + TMath::Power(map_current_error_off[n_layer].at(i), 2.0));

      Double_t rate = 0;
      Double_t rate_error = 0;

      if(n_layer==10 || (n_layer==4 && xray_current<30)) GetY(map_gr_xray_current_measured_rate[n_layer], xray_current, rate, rate_error);
      else GetY(map_gr_xray_current_expected_rate[n_layer], xray_current, rate, rate_error);
      
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

void Rate_Capability::Read_Attenuation()
{
  //Read data file
  ifstream fin;
  fin.open(path+"/Data_Attenuation/Attenuation.csv");

  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find Attenuation.txt. Check it first!!" << endl;
      exit(1);
    }

  map<Int_t, Int_t> map_count_off;
  map<Int_t, Int_t> map_count_on;
  map<Int_t, Double_t> map_current_off;
  map<Int_t, Double_t> map_current_error_off;
  map<Int_t, Double_t> map_current_on;
  map<Int_t, Double_t> map_current_error_on;

  string temp;
  getline(fin, temp);
  while(!fin.eof())
    {
      getline(fin, temp);
      if(temp.compare("")==0) break;

      Int_t n_layer;
      Int_t count_off;
      Int_t count_on;
      Double_t current_off;
      Double_t current_error_off;
      Double_t current_on;
      Double_t current_error_on;

      sscanf(temp.c_str(), "%d,%d,%d,%lf,%lf,%lf,%lf", &n_layer, &count_off, &count_on, &current_off, &current_error_off, &current_on, &current_error_on);

      map_count_off[n_layer] = count_off;
      map_count_on[n_layer] = count_on;
      map_current_off[n_layer] = current_off;
      map_current_error_off[n_layer] = current_error_off;
      map_current_on[n_layer] = current_on;
      map_current_error_on[n_layer] = current_error_on;
    }

  //let's calculate
  gr_count_current.SetName("Gr_Count_Current");
  gr_count_current.SetTitle("Count vs Current");
  gr_count_current.GetXaxis()->SetTitle("Count");
  gr_count_current.GetYaxis()->SetTitle("1e9*Current");

  gr_n_layer_count.SetName("Gr_N_Layer_Count");
  gr_n_layer_count.SetTitle("#Layer vs. Count");
  gr_n_layer_count.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_count.GetYaxis()->SetTitle("Count");

  gr_n_layer_current.SetName("Gr_N_Layer_Current");
  gr_n_layer_current.SetTitle("#Layer vs. Current");
  gr_n_layer_current.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_current.GetYaxis()->SetTitle("1e9*Current");

  gr_n_layer_attenuation_factor_count.SetName("Gr_N_Layer_Attenuation_Factor_Count");
  gr_n_layer_attenuation_factor_count.SetTitle("#Layer vs. Attenuation_Factor (Count)");
  gr_n_layer_attenuation_factor_count.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_attenuation_factor_count.GetYaxis()->SetTitle("Attenuation_Factor (Count)");

  gr_n_layer_attenuation_factor_current.SetName("Gr_N_Layer_Attenuation_Factor_Current");
  gr_n_layer_attenuation_factor_current.SetTitle("#Layer vs. Attenuation_Factor (Current)");
  gr_n_layer_attenuation_factor_current.GetXaxis()->SetTitle("#Layer");
  gr_n_layer_attenuation_factor_current.GetYaxis()->SetTitle("Attenuation_Factor (Current)");

  Double_t count_10 = map_count_on[10] - map_count_off[10];
  Double_t count_error_10 = TMath::Sqrt(map_count_on[10] + map_count_off[10]);

  Double_t current_10 = map_current_on[10] - map_current_off[10]; ;
  Double_t current_error_10 = TMath::Sqrt(TMath::Power(map_current_error_on[10], 2.0)+TMath::Power(map_current_error_off[10], 2.0));

  for(auto it=map_count_off.begin(); it!=map_count_off.end(); it++)
    {
      Int_t n_layer = it->first;

      Double_t count = map_count_on[n_layer] - map_count_off[n_layer];
      Double_t count_error = TMath::Sqrt(map_count_on[n_layer] + map_count_off[n_layer]);

      Double_t current = map_current_on[n_layer] - map_current_off[n_layer];
      Double_t current_error = TMath::Sqrt(TMath::Power(map_current_error_on[n_layer], 2.0)+TMath::Power(map_current_error_off[n_layer], 2.0));

      Int_t n_point = gr_count_current.GetN();

      gr_count_current.SetPoint(n_point, count, 1e9*current);
      gr_count_current.SetPointError(n_point, count_error, 1e9*current_error);

      gr_n_layer_count.SetPoint(n_point, n_layer, count);
      gr_n_layer_count.SetPointError(n_point, 0, count_error);

      gr_n_layer_current.SetPoint(n_point, n_layer, 1e9*current);
      gr_n_layer_current.SetPointError(n_point, 0, 1e9*current_error);

      Double_t attenuation_factor_count = count/count_10;
      Double_t attenuation_factor_count_error = TMath::Sqrt(TMath::Power(count_error/count_10, 2) + TMath::Power(count*count_error_10/TMath::Power(count_10, 2.0), 2.0));

      gr_n_layer_attenuation_factor_count.SetPoint(n_point, n_layer, attenuation_factor_count);
      gr_n_layer_attenuation_factor_count.SetPointError(n_point, 0, attenuation_factor_count_error);

      Double_t attenuation_factor_current = current/current_10;
      Double_t attenuation_factor_current_error = TMath::Sqrt(TMath::Power(current_error/current_10, 2) + TMath::Power(current*current_error_10/current_10/current_10, 2.0));

      gr_n_layer_attenuation_factor_current.SetPoint(n_point, n_layer, attenuation_factor_current);
      gr_n_layer_attenuation_factor_current.SetPointError(n_point, 0, attenuation_factor_current_error);
    }

  //Write
  fout->cd();

  gr_count_current.Write();
  gr_n_layer_count.Write();
  gr_n_layer_current.Write();
  gr_n_layer_attenuation_factor_count.Write();
  gr_n_layer_attenuation_factor_current.Write();

  //fit
  gr_count_current.Fit("pol1", "FS0", "", 0, 8e6);
  gr_n_layer_count.Fit("expo", "S", "", 2, 12);

  fit_n_layer_count = (TF1*)(gr_n_layer_count.GetListOfFunctions()->FindObject("expo"));
  
  //print
  TCanvas canvas("canvas", "canvas", 1200, 800);
  canvas.Divide(1, 2);
  canvas.Draw();

  canvas.cd(1);
  gr_count_current.Draw("AP*");

  canvas.cd(2);
  gr_n_layer_count.Draw("AP*");

  canvas.GetPad(2)->SetLogy();

  canvas.Print("Attenuation.png", "png");

  return;
}//void Rate_Capability::Read_Attenuation()
  
//////////

void Rate_Capability::Draw_Multi_Layer_Gain(const Bool_t& chk_renormal)
{
  TCanvas canvas("can", "can", 800, 500);

  TMultiGraph multi_gr;
  multi_gr.SetName("Gr_Flux_Normalized_Gain");
  multi_gr.SetTitle("Flux  vs Normalized Gain");

  TGraphErrors gr_normal[4];

  map<Int_t, TGraphErrors>& map = map_gr_flux_gain;
  if(chk_renormal==kFALSE) map = map_gr_flux_gain;
  else map = map_gr_flux_gain_renormal;
  
  Double_t average = 1, error = 0;
  Int_t count = 0;
  for(auto it = map.rbegin(); it!=map.rend(); it++)
    {
      TGraphErrors* gr = &it->second;

      if(count==0)
	{
	  gr->Fit("pol0");
	  TF1* fit = (TF1*)(gr->GetListOfFunctions()->FindObject("pol0"));

	  average = fit->GetParameter(0);
	  error = fit->GetParError(0);
	}

      Double_t* x = gr->GetX();
      Double_t* ex = gr->GetEX();
      Double_t* y = gr->GetY();
      Double_t* ey = gr->GetEY();

      for(Int_t i=0; i<gr->GetN(); i++)
	{
	  gr_normal[count].SetPoint(i, x[i], y[i]/average);
	  gr_normal[count].SetPointError(i, ex[i], TMath::Sqrt(TMath::Power(ey[i]/average, 2.0) + TMath::Power(ey[i]*error/average/average, 2.0)));
	}
      
      gr_normal[count].SetLineColor(count+1);
      gr_normal[count].SetMarkerStyle(count+27);
      gr_normal[count].SetMarkerColor(count+1);
      
      multi_gr.Add(&gr_normal[count]);

      count++;
    }

  //write
  multi_gr.Write();
  
  multi_gr.Draw("AP");
  multi_gr.SetTitle("Flux_Normalized_Gain_"+hv_current);
  multi_gr.GetXaxis()->SetLimits(1e1, 1e7);
  multi_gr.GetXaxis()->SetTitle("Flux [Hz/mm^{2}]");
  multi_gr.GetYaxis()->SetRangeUser(0, 1.7);
  multi_gr.GetYaxis()->SetTitle("Normalized Gain");
    
  canvas.SetLogx();
  canvas.SetGridy();

  canvas.Update();
  
  canvas.Print("Flux_Normalized_Gain_"+hv_current+".png", "png");
  
  return;
}//void Rate_Capability::Draw_Multi_Layer_Gain(const Bool_t& chk_recal)

//////////

void Rate_Capability::Read_Single_Layer_Data(const Int_t& n_layer)
{
  //read data
  TString target_data = path + "/Data_Rate_Capability/" + hv_current + "/Rate_Capability_" + hv_current + "_" + to_string(n_layer) + "Layer.csv";

  ifstream fin;
  fin.open(target_data);

  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find " << target_data << ". Check it first." << endl;
      exit(1);
    }

  vector<Int_t> vector_xray_current;

  vector<Int_t> vector_count_off;
  vector<Double_t> vector_current_off;
  vector<Double_t> vector_current_error_off;

  vector<Int_t> vector_count_on;
  vector<Double_t> vector_current_on;
  vector<Double_t> vector_current_error_on;
  
  string temp;
  getline(fin, temp);
  while(!fin.eof())
    {
      getline(fin, temp);
      if(temp.compare("")==0) break;

      Int_t xray_current;

      Int_t count_off;
      Double_t current_off;
      Double_t current_error_off;

      Int_t count_on;
      Double_t current_on;
      Double_t current_error_on;

      sscanf(temp.c_str(), "%d,%d,%lf,%lf,%d,%lf,%lf", &xray_current, &count_off, &current_off, &current_error_off, &count_on, &current_on, &current_error_on);

      vector_xray_current.push_back(xray_current);

      vector_count_off.push_back(count_off);
      vector_current_off.push_back(current_off);
      vector_current_error_off.push_back(current_error_off);
            
      vector_count_on.push_back(count_on);
      vector_current_on.push_back(current_on);
      vector_current_error_on.push_back(current_error_on);
    }

  map_xray_current[n_layer] = vector_xray_current;

  map_count_off[n_layer] = vector_count_off;
  map_current_off[n_layer] = vector_current_off;
  map_current_error_off[n_layer] = vector_current_error_off;
  
  map_count_on[n_layer] =  vector_count_on;
  map_current_on[n_layer] = vector_current_on;
  map_current_error_on[n_layer] =  vector_current_error_on;

  return;
}//void Rate_Capability::Read_Single_Layer_Data(const Int_t& n_layer)

//////////

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
	  Double_t flux_renormal = flux[i];//*attenuation_renormal;
	  Double_t flux_error_renormal = flux_error[i];//*attenuation_renormal;
	  
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

//////////

void Rate_Capability::Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode)
{
  Double_t attenuation_factor = 0;
  Double_t attenuation_factor_error = 1;
  
  if(mode=="COUNT_MEASUREMENT") GetY(gr_n_layer_attenuation_factor_count, n_layer, attenuation_factor, attenuation_factor_error);
  else if(mode=="CURRENT_MEASUREMENT") GetY(gr_n_layer_attenuation_factor_current, n_layer, attenuation_factor, attenuation_factor_error);
  else if(mode=="FIT_SELF")
    {
      map_gr_xray_current_measured_rate[n_layer].Fit("pol1", "", "", 0, 30);

      TF1* func = (TF1*)(map_gr_xray_current_measured_rate[n_layer].GetListOfFunctions()->FindObject("pol1"));
      Double_t p0 = func->GetParameter(0);
      Double_t p1 = func->GetParameter(1);

      rate = xray_current*p1 + p0;
      
      return;
    }
       
  //cout << n_layer << " " << attenuation_factor << " " << attenuation_factor_error << endl;
  
  Double_t reference_rate = map_reference_rate[xray_current];
  Double_t reference_rate_error = map_reference_rate_error[xray_current];
  
  //in case no referece points in 10 layer measurement
  if(reference_rate<10)
    {
      reference_rate = map_gr_xray_current_measured_rate[10].Eval(xray_current);      
    }
  
  rate = attenuation_factor*reference_rate;
  rate_error = TMath::Sqrt(TMath::Power(attenuation_factor_error*reference_rate, 2.0) + TMath::Power(attenuation_factor*reference_rate_error, 2.0));

  return;
}//void Rate_Capability::Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode)

//////////

void Rate_Capability::GetY(const TGraphErrors& graph, const Double_t& x, Double_t& y, Double_t& ey)
{
  //stupid. Why root doesn't support this method.
  
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

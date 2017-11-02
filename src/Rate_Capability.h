#ifndef __Rate_Capability_h__
#define __Rate_Capability_h__

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>

#include <TObject.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TF1.h>
#include <TMultiGraph.h>

#include "Const_Def.h"

using namespace std;

class Rate_Capability :  public TObject
{
 public:
  Rate_Capability(const TString& a_hv_current, const TString& a_path);
  ~Rate_Capability();

  void Calculate_Expected_Rate(const Int_t& n_layer);
  void Calculate_Measured_Rate(const Int_t& n_layer);
  void Calculate_Single_Layer_Gain(const Int_t& n_layer);
  void Draw_Multi_Layer_Gain(const Bool_t& chk_recal);
  void Read_Attenuation();
  void Read_Single_Layer_Data(const Int_t& n_layer);
  void Recalculate_Attenuation_Gain();
  
 protected:
  TString hv_current;
  TString path;

  TFile* fout;

  //attenuation
  TGraphErrors gr_count_current;
  TGraphErrors gr_n_layer_count;
  TGraphErrors gr_n_layer_current;
  TGraphErrors gr_n_layer_attenuation_factor_count;
  TGraphErrors gr_n_layer_attenuation_factor_current;
  
  TF1* fit_n_layer_count;

  //data point
  map<Int_t, vector<Int_t>> map_xray_current;
  map<Int_t, vector<Int_t>> map_count_off;
  map<Int_t, vector<Int_t>> map_count_on;
  map<Int_t, vector<Double_t>> map_current_off;
  map<Int_t, vector<Double_t>> map_current_error_off;
  map<Int_t, vector<Double_t>> map_current_on;
  map<Int_t, vector<Double_t>> map_current_error_on;
  
  
  //reference
  map<Int_t, Double_t> map_reference_rate;
  map<Int_t, Double_t> map_reference_rate_error;
  
  map<Int_t, TGraphErrors> map_gr_xray_current_measured_rate;
  map<Int_t, TGraphErrors> map_gr_xray_current_expected_rate;
  
  //graphs for gain results
  map<Int_t, TGraphErrors> map_gr_flux_gain;
  map<Int_t, TGraphErrors> map_gr_flux_gain_recal;
  
  void Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode);
  void GetY(const TGraphErrors& graph, const Double_t& x, Double_t& y, Double_t& ey);
  
  ClassDef(Rate_Capability, 1);
};

#endif /* __Rate_Capability_h__ */

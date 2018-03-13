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
#include "Read_RO.h"

using namespace std;

class Rate_Capability :  public TObject
{
 public:
  Rate_Capability(const TString& a_hv_current, const TString& a_path);
  ~Rate_Capability();

  void Calculate_Attenuation_Factor();
  void Calculate_Expected_Rate(const Int_t& n_layer);
  void Calculate_Measured_Rate(const Int_t& n_layer);
  void Calculate_Single_Layer_Gain(const Int_t& n_layer);
  //void Draw_Multi_Layer_Gain(const Bool_t& chk_recal);
  void Read_Attenuation_Data();
  void Read_Single_Layer_Data(const Int_t& n_layer);
  //void Renormalize_Attenuation_Gain();

  typedef struct _data_attenuation
  {
    Int_t n_layer;
    Int_t daq_time;
    
    Int_t count_off;
    Double_t current_off;
    Double_t current_error_off;

    Int_t count_on;
    Double_t current_on;
    Double_t current_error_on;
  } Data_Attenuation;

  typedef struct _data_point
  {
    Int_t xray_current;
    
    Int_t count_off;
    Double_t current_off;
    Double_t current_error_off;

    Int_t count_on;
    Double_t current_on;
    Double_t current_error_on;
  } Data_Point;

  typedef struct _data
  {
    Int_t n_layer;
    Int_t daq_time;

    vector<Data_Point> points;
  } Data;
  
 protected:
  TString hv_current;
  TString path;

  TFile* fout;

  //attenuation
  map<Int_t, Data_Attenuation> map_attenuation;
  
  TGraphErrors gr_rate_current;
  TGraphErrors gr_n_layer_rate;
  TGraphErrors gr_n_layer_current;
  TGraphErrors gr_n_layer_attenuation_factor_rate;
  TGraphErrors gr_n_layer_attenuation_factor_current;
  
  TF1* fit_n_layer_rate;
  TF1* fit_n_layer_current;
  
  //data point
  map<Int_t, Data> map_data;
  
  map<Int_t, TGraphErrors> map_gr_xray_current_measured_rate;
  map<Int_t, TGraphErrors> map_gr_xray_current_expected_rate;
  
  //graphs for gain results
  map<Int_t, TGraphErrors> map_gr_flux_gain;
  map<Int_t, TGraphErrors> map_gr_flux_gain_renormal;
  
  void Get_Rate(const Int_t& xray_current, const Int_t& n_layer, Double_t& rate, Double_t& rate_error, const TString& mode);
  void GetY(const TGraphErrors& graph, const Double_t& x, Double_t& y, Double_t& ey);
    
  ClassDef(Rate_Capability, 1);
};

#endif /* __Rate_Capability_h__ */

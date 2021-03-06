#ifndef __Gain_h__
#define __Gain_h__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <TObject.h>
#include <TString.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TF1.h>
#include <TVirtualFitter.h>
#include <Const_Def.h>
#include <Read_RO.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <TTree.h>

using namespace std;
using namespace TMath;

namespace namespace_gain
{
  typedef struct _Data
  {
    Int_t voltage;
    Int_t hv_current;

    Double_t voltage_corr;
    Double_t hv_current_corr;
    
    Int_t count_off;
    Int_t count_on;
    Float_t ro_current_off;
    Float_t ro_current_error_off;
    Float_t ro_current_on;
    Float_t ro_current_error_on;
  } Data;

  static Bool_t Compare_Data(Data a, Data b)
  {
    return a.hv_current < b.hv_current;
  }
}

using namespace namespace_gain;

class Gain : public TObject
{
 public:
  Gain(const TString& a_path, const TString& a_target);
  ~Gain();
  
 private:
  TString path;
  TString target;
  
  Double_t pressure;
  Double_t temperature;
  Double_t relative_humidity;
   
  TString source;

  Int_t ref_index;
  
  TGraphErrors gr_gain;
  TGraphErrors gr_gain_corr;   
  TGraphErrors gr_rate;
    
  TF1 fit_gain;

  TGraphErrors gr_gain_conf;
  
  TFile* fout;
  TTree* tree;
  
  TCanvas canvas;
  TPaveStats* stats;
  
  vector<namespace_gain::Data> vec_data;

  void Calculate_Gain();
  void Draw();
  void Fit();
  void Read_Data();
  
  ClassDef(Gain, 1);
};

#endif /* __Gain_h__ */

#ifndef __Discharge_Hardness_h_
#define __Discharge_Hardness_h_

#include <iostream>

#include <TObject.h>
#include <TFile.h>
#include <TString.h>
#include <TH1D.h>
#include <TF1.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TGraphErrors.h>

#include "Read_MCA_Data.h"

using namespace std;

class Discharge_Hardness : public TObject
{
 public:
  Discharge_Hardness(const TString& mca_path_before, const TString& mca_path_after, const TString& gain_path_before, const TString& gain_path_after);
  ~Discharge_Hardness();
  
 protected:
  Read_MCA_Data adc_before;
  Read_MCA_Data adc_after;
  
  Double_t fwhm_before;
  Double_t max_x_before;

  Double_t fwhm_after;
  Double_t max_x_after;

  Double_t resolution_before;
  Double_t resolution_after;

  TH1D* histo_before;
  TH1D* histo_after;

  TLegend tl_adc;
  
  TCanvas can_adc;
  
  TFile* fin_gain_before;
  TFile* fin_gain_after;

  TGraphErrors* gain_before;
  TGraphErrors* gain_after;

  TF1* fit_gain_before;
  TF1* fit_gain_after;

  TLegend tl_gain;

  TCanvas can_gain;
  
  TLatex latex;
  
  TFile* fout;
  
  void Compare_Resolution();
  void Draw_Gain();
  void Draw_Resolution();
  
  ClassDef(Discharge_Hardness, 1);
};

#endif /* __Discharge_Hardness_h_ */

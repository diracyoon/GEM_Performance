#ifndef __ADC_Spectrum_Analyzer_h__
#define __ADC_Spectrum_Analyzer_h__

#include <iostream>
#include <string>

#include <TObject.h>
#include <TFile.h>
#include <TString.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>

#include <Read_MCA_Data.h>
#include <DP5.h>
#include <MC2.h>

using namespace std;

class ADC_Spectrum_Analyzer : public TObject
{
 public:
  ADC_Spectrum_Analyzer(const TString& a_data_file);
  ~ADC_Spectrum_Analyzer();

  void Fit_Histo();
  Float_t Get_FWHM();
  
  static Double_t Landau_Gaus(Double_t* x, Double_t* par);
  static Double_t Polynomial(Double_t* x, Double_t* par); 
  static Double_t Signal(Double_t* x, Double_t* par);
  static Double_t Total(Double_t*x, Double_t* par);

 protected:
  string data_file;
  
  Read_MCA_Data* read_mca_data;

  TH1D histo;

  Float_t fit_range_lower;
  Float_t fit_range_upper;

  Float_t fwhm;
  Float_t resolution;
  
  TF1* f1_peak;
  TF1* f1_escape_peak;
  TF1* f1_signal;
  TF1* f1_total;
  
  TFile* fout;
  
  ClassDef(ADC_Spectrum_Analyzer, 1);
};

#endif /* __ADC_Spectrum_Anlyzer_h__ */

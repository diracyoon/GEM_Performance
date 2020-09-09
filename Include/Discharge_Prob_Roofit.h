#ifndef __Discharge_Prob_Roofit_h__
#define __Discharge_Prob_Roofit_h__

#include <vector>
#include <iostream>
#include <fstream>

#include "TString.h"
#include "TObject.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFeldmanCousins.h"
#include "TDirectory.h"
#include "TCanvas.h"

#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooAbsArg.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooNLLVar.h"
#include "RooMinimizer.h"

#include "Read_Env_Data.h"

#include "PDF_Poisson.h"

namespace namespace_discharge_prob
{
    typedef struct _Data
    {
      Double_t voltage;
      Double_t current;
      time_t start_time;
      time_t end_time;
      Int_t alpha_count;
      Int_t net_alpha_count;
      Int_t discharge_count;
      Int_t duration;
      Int_t background_count;

      Float_t gain;
      Float_t gain_error;
      TH1D* h_tp;
      TH1D* h_gain;

      vector<namespace_Env_Data::Data> env_data;
      vector<TH1D*> h_iteration;
    } Data;

    typedef struct _CL_Data
    {
      Int_t count;
      Double_t lower_cl;
      Double_t upper_cl;
    } CL_Data;
}

using namespace std;
using namespace TMath;
using namespace namespace_discharge_prob;
using namespace RooFit;

class Discharge_Prob_Roofit : public TObject
{
 public:
  Discharge_Prob_Roofit(const TString& a_gain_file);
  virtual ~Discharge_Prob_Roofit();
  
 protected:
  const Int_t n_gain_bin;
  const Int_t gain_range;
  
  TFile* fin_gain;
  TString gain_file;

  TF1 fit_gain;

  TFile* fout;
  
  vector<namespace_discharge_prob::CL_Data> vec_cl_data;
  vector<namespace_discharge_prob::Data> vec_data;

  RooWorkspace* workspace;

  RooRealVar gain;
  RooRealVar a;
  RooRealVar b;
  
  RooRealVar alpha;
  RooRealVar discharge;
  RooArgSet point;

  RooDataSet roo_data;
  
  const Int_t max_iteration;
  
  TDirectory* subdir;
  RooFormulaVar* fit_discharge_prob[10];
  PDF_Poisson* pdf[10];
  TGraphAsymmErrors* gr_discharge_prob[10];
  TCanvas* can[10];

  TGraph* gr_contour;

  TGraphAsymmErrors* gr_conf;
  TCanvas* can_conf;
  
  void Contour();
  void Draw();
  void Fill_RooData(const Int_t& index);
  void Find_Poisson_CL(const Int_t& count, Double_t* cl);
  void Fit(const Int_t& index);
  void Get_Gain();
  void Iteration();
  time_t Parsing_Time(const string& str);
  void Read_Count_Data();
  void Read_Poisson_CL();
  void Return_Dates(const time_t& start_time, const time_t& end_time, vector<TString>& dates);
  void Variation();
  void Visualization();
  void Visualization(const Int_t& index);
  
  ClassDef(Discharge_Prob_Roofit, 1);
};

#endif /* __Discharge_Prob_Roofit_h__ */

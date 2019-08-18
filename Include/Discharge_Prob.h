#ifndef __Discharge_Prob_h_
#define __Discharge_Prob_h_

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>

#include <TObject.h>
#include <TFile.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TAxis.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TVirtualFitter.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TTree.h>
#include <TH1D.h>

#include <Const_Def.h>
#include <Read_Env_Data.h>

namespace namespace_discharge_prob
{
  typedef struct _Data
  {
    Double_t voltage;
    Double_t current;
    time_t start_time;
    time_t end_time;
    Int_t alpha_count;
    Int_t discharge_count;
    Int_t duration;
    Int_t background_count;

    Float_t gain;
    Float_t gain_error;
    TH1D* h_gain;
    
    vector<namespace_Env_Data::Data> env_data;
  } Data;
}

using namespace std;
using namespace TMath;
using namespace namespace_discharge_prob;

class Discharge_Prob : public TObject
{
 public:
  Discharge_Prob(const TString& a_gain_file);
  ~Discharge_Prob();
  
 protected:
  TFile* fin;
  TString gain_file;

  TF1 fit_gain;
  TGraphErrors conf_gain;

  TFile* fout;
  
  vector<namespace_discharge_prob::Data> vec_data;
  
  TGraphErrors gr_discharge_prob;
  TGraphErrors gr_discharge_prob_scale;

  TF1 fit_discharge_prob;
  TF1 fit_discharge_prob_scale;

  TGraphErrors gr_discharge_prob_conf;
  TGraphErrors gr_discharge_prob_conf_scale;

  TPaveStats* stats;
  TLegend tl;
  
  TCanvas canvas;
  
  void Calculate_CL();
  void Calculate_Prob();
  void Draw();
  void Get_Gain();
  void Get_Gain_Error(const Double_t& current, Double_t val[]);
  time_t Parsing_Time(const string& str);
  void Read_Count_Data();
  void Return_Dates(const time_t& start_time, const time_t& end_time, vector<TString>& dates);
  
  ClassDef(Discharge_Prob, 1);
};

#endif /* __Dicharge_Prob_h__ */

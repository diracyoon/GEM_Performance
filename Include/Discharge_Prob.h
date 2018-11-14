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

namespace namespace_discharge_prob
{
  typedef struct _Data
  {
    Double_t voltage;
    Double_t current;
    Int_t alpha_count;
    Int_t discharge_count;
    Int_t duration;
    Int_t background_count;
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
  
  TGraphErrors* conf_gain;

  TFile* fout;
  
  vector<namespace_discharge_prob::Data> vec_data;

  TGraphErrors gr_gain_discharge_prob;
  
  void Calculate_Prob();
  void Get_Mean_Error_Gr();
  time_t Parsing_Time(const string& str);
  void Read_Count_Data();
    
  ClassDef(Discharge_Prob, 1);
};

#endif /* __Dicharge_Prob_h__ */

#ifndef __Read_RO_h__
#define __Read_RO_h__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <map>

#include <TObject.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TVirtualFitter.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
using namespace std;

class Read_RO : public TObject
{
 public:
  Read_RO(const TString& a_fin_name, const Bool_t& a_ro_mode=kTRUE);
  ~Read_RO();

  void Draw();
  Double_t Get_Mean(){ return ro_mean; }
  Double_t Get_Mean_Error(){ return ro_mean_error; }
  TGraphErrors Get_Graph(){ return gr_ro; }
  
 protected:
  TString fin_name;
  ifstream fin;

  //kTRUE="low range", kFALSE="high range"
  Bool_t ro_mode;
  
  TGraphErrors gr_ro;
  TGraphErrors gr_ro_conf;
  
  Double_t ro_mean;
  Double_t ro_mean_error;
  
  void Read_RO_Count();
  
  ClassDef(Read_RO, 1);
};

#endif /* __Read_RO_h__ */

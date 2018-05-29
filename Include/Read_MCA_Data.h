#ifndef __Read_MCA_Data_h__
#define __Read_MCA_Data_h__

#include <iostream>
#include <fstream>

#include <TObject.h>
#include <TString.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

using namespace std;

class Read_MCA_Data : public TObject
{
 public:
  Read_MCA_Data(const TString& a_data_file, const TString& histo_name="histo", const TString& histo_title="histo");
  ~Read_MCA_Data();
  
  void Fit_Histo(const Bool_t& chk_draw=kTRUE);
  Float_t Get_Fit_Parameter(const Int_t& index);
  Float_t Get_Fit_Par_Error(const Int_t& index);
  TH1D* Get_Histo(){ return (TH1D*)histo.Clone(); }
  
 private:
  TString data_file;
  
  ifstream fin;
  TH1D histo;
  
  void Fill_Histo();
    
  ClassDef(Read_MCA_Data, 1);
};

#endif /* __Read_MCA_Data_h__ */

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
#include <TFile.h>

using namespace std;

class Read_MCA_Data : public TObject
{
 public:
  Read_MCA_Data(const TString& a_data_file, const TString& a_histo_name="histo", const TString& a_histo_title="histo");
  virtual ~Read_MCA_Data();

  virtual void Read_Data()=0;
  
  /* Float_t Get_Fit_Parameter(const Int_t& index); */
  /* Float_t Get_Fit_Par_Error(const Int_t& index); */
  /* Float_t Get_Max_X(); */
  
  TH1D Get_Histo();
   
 protected:
   TString data_file; 
   TString histo_name;
   TString histo_title;
   
  /* Float_t fwhm; */

  /* const Double_t fit_range_lower; */
  /* const Double_t fit_range_upper; */
  
   ifstream fin; 

   TH1D* histo; 
  
   ClassDef(Read_MCA_Data, 1);
};

#endif /* __Read_MCA_Data_h__ */

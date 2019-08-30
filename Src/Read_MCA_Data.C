#include "Read_MCA_Data.h"

ClassImp(Read_MCA_Data);

//////////

Read_MCA_Data::Read_MCA_Data(const TString& a_data_file, const TString& a_histo_name, const TString& a_histo_title) : data_file(a_data_file), histo_name(a_histo_name), histo_title(a_histo_title), histo(NULL)//fit_range_lower(150), fit_range_upper(850) 
{
  fin.open(data_file);
  if(fin.is_open()==kFALSE)
     {
       cout << "Can not find data file! " << data_file << endl;
       exit(1);
     }
}//Read_MCA_Data::Read_MCA_Data()

//////////

Read_MCA_Data::~Read_MCA_Data()
{
  fin.close();
}//Read_MCA_Data::~Read_MCA_Data()

//////////

TH1D Read_MCA_Data::Get_Histo()
{
  if(histo!=NULL) return *histo;
  else
    {
      cout << "Blank histogram has been returned." << endl;

      TH1D blank_histo;
      return blank_histo;
    }
}//TH1D Get_Histo()

//////////

// Float_t Read_MCA_Data::Get_Fit_Parameter(const Int_t& index)
// {
//   TF1* fit_func = (TF1*)(histo.GetListOfFunctions()->FindObject("Total"));

//   Double_t parameter = fit_func->GetParameter(index);

//   return parameter;
// }//Float_t Read_MCA_Data::Get_Fit_Parameter(const Int_t& index)

// //////////

// Float_t Read_MCA_Data::Get_Max_X()
// {
//   TF1* f1_func = (TF1*)(histo.GetListOfFunctions()->FindObject("Total"));

//   Double_t par[12];
//   f1_func->GetParameters(par);
  
//   return par[1];
// }//Float_t Read_MCA_Data::Get_Max_X()

// //////////

// Float_t Read_MCA_Data::Get_Fit_Par_Error(const Int_t& index)
// {
//   TF1* fit_func = (TF1*)(histo.GetListOfFunctions()->FindObject("Total"));

//   Double_t par_error = fit_func->GetParError(index);

//   return par_error;
// }//Float_t Read_MCA_Data::Get_Fit_Par_Error(const Int_t& index)

// //////////

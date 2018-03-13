#include "Read_RO.h"

ClassImp(Read_RO);

//////////

Read_RO::Read_RO(const TString& fin_name, const Bool_t& a_ro_mode)
{
  fin.open(fin_name);
  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find txt file!" << endl;
      exit(1);
    }

  ro_mode = a_ro_mode;
  
  Read_RO_Count();
}//Read_RO::Read_RO()

//////////

Read_RO::~Read_RO()
{
}//Read_RO::~Read_RO()

//////////

void Read_RO::Read_RO_Count()
{
  while(!fin.eof())
    {
      string buf;
      
      getline(fin, buf);
      if(buf.compare("")==0) break;

      Double_t ro;
      sscanf(buf.c_str(), "%lf", &ro);

      Int_t n_point = gr_ro.GetN();
      gr_ro.SetPoint(n_point, n_point, ro);

      Double_t ro_error;
      if(ro_mode==kTRUE) ro_error = 1e-12;
      else ro_error = 1e-9;
      gr_ro.SetPointError(n_point, n_point, ro_error);
    }

  gr_ro.Fit("pol0", "FQ");
  TF1* func_fit = (TF1*)(gr_ro.GetListOfFunctions()->FindObject("pol0"));
  ro_mean = func_fit->GetParameter(0);
  ro_mean_error = func_fit->GetParError(0);
  
  return;
}

//////////

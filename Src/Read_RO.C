#include "Read_RO.h"

ClassImp(Read_RO);

//////////

Read_RO::Read_RO(const TString& a_fin_name, const Bool_t& a_ro_mode)
{
  fin_name  = a_fin_name;
  
  fin.open(fin_name);
  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find txt file, " << fin_name << endl;
      exit(1);
    }

  ro_mode = a_ro_mode;
  
  Read_RO_Count();
}//Read_RO::Read_RO()

//////////

Read_RO::~Read_RO()
{
  fin.close();
}//Read_RO::~Read_RO()

//////////

void Read_RO::Draw()
{
  gStyle->SetOptFit(1111);
  
  TCanvas can("Canvas", fin_name, 800, 500);
  can.Draw();

  gr_ro_conf.SetFillColor(6);
  gr_ro_conf.SetFillStyle(3005);
  
  gr_ro_conf.Draw("a3");
  
  gr_ro.Draw("SAMEP");
  
  can.SaveAs(fin_name+".C");
  
  return;
}//void Read_RO::Draw()

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
      gr_ro.SetPointError(n_point, 0, ro_error);
    }
 
  gr_ro.Fit("pol0", "F");

  TF1* func_fit = (TF1*)(gr_ro.GetListOfFunctions()->FindObject("pol0"));

  
  ro_mean = -1*func_fit->GetParameter(0);

  // Int_t n_point = gr_ro.GetN();
  // for(Int_t i=0; i<n_point; i++) gr_ro_conf.SetPoint(i, i, func_fit->GetParameter(0));
    
  // (TVirtualFitter::GetFitter())->GetConfidenceIntervals(&gr_ro_conf, 0.68);

  // ro_mean_error = gr_ro_conf.GetEY()[0];

  Double_t chi2 = func_fit->GetChisquare(); 
  Double_t ndf = func_fit->GetNDF();
  chi2 = chi2/ndf;

  ro_mean_error = func_fit->GetParError(0)*TMath::Sqrt(chi2);
  
  return;
}

//////////

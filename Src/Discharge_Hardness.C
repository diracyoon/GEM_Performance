#include "Discharge_Hardness.h"

ClassImp(Discharge_Hardness);

//////////

Discharge_Hardness::Discharge_Hardness(const TString& mca_path_before, const TString& mca_path_after, const TString& gain_path_before, const TString& gain_path_after) : adc_before(mca_path_before, "Before", "Before"), adc_after(mca_path_after, "After", "After"), tl_adc(0.53, 0.50, 0.83, 0.80), can_adc("Discharge_Hardness_ADC", "Discharge_Hardness_ADC", 800, 500), latex(0.1,0.905,"GE1/1-X-S-KOREA-0004"), tl_gain(0.15, 0.55, 0.5, 0.8), can_gain("Discharge_Hardness_Gain", "Discharge_Hardness_Gain", 800, 500)
{
  gStyle->SetOptStat(0);

  latex.SetNDC();
  latex.SetLineWidth(2);

  fin_gain_before = new TFile(gain_path_before);
  fin_gain_after = new TFile(gain_path_after);
  
  fout = new TFile("Discharge_Hardness.root", "RECREATE");
  
  Compare_Resolution();
  Draw_Resolution();
  Draw_Gain();
}//Discharge_Hardness::Discharge_Hardness(const TString& mca_path_before, const TString& mca_path_after, const TString& gain_path_before, const TString& gain_path_after)

//////////

Discharge_Hardness::~Discharge_Hardness()
{
  fin_gain_before->Close();
  fin_gain_after->Close();
  fout->Close();
}//~Discharge_Hardness::Discharge_Hardness()

//////////

void Discharge_Hardness::Compare_Resolution()
{
  
  adc_before.Fit_Histo(kTRUE, kTRUE);
  adc_after.Fit_Histo(kTRUE, kTRUE);

  fwhm_before = adc_before.Get_FWHM();
  max_x_before = adc_before.Get_Max_X();

  fwhm_after = adc_after.Get_FWHM();
  max_x_after = adc_after.Get_Max_X();

  resolution_before = fwhm_before/max_x_before;
  resolution_after = fwhm_after/max_x_after;
  
  return;
}//void Discharge_Hardness::Compare_Resolution()

//////////

void Discharge_Hardness::Draw_Gain()
{
  gain_before = (TGraphErrors*)fin_gain_before->Get("Gr_HV_Current_Gain");
  gain_after = (TGraphErrors*)fin_gain_after->Get("Gr_HV_Current_Gain");

  //remove unnecessary point x = 710
  gain_before->RemovePoint(9);
  gain_after->RemovePoint(8);
  
  gain_before->Fit("expo", "", "", 550, 700);
  gain_after->Fit("expo", "", "", 550, 700);

  fit_gain_before = (TF1*)(gain_before->GetListOfFunctions()->FindObject("expo"));
  fit_gain_after = (TF1*)(gain_after->GetListOfFunctions()->FindObject("expo"));

  gain_before->SetLineColor(2);
  gain_after->SetLineColor(4);

  fit_gain_before->SetLineColor(2);
  fit_gain_after->SetLineColor(4);

  gain_before->SetTitle("");
  gain_before->GetXaxis()->SetTitle("Divider Current [#muA]");
  gain_before->GetXaxis()->SetRangeUser(530, 710);
  gain_before->GetYaxis()->SetTitle("Effective Gas Gain");
  gain_before->GetYaxis()->SetRangeUser(10e0, 10e4);

  tl_gain.AddEntry(gain_before, "#splitline{Before discharges}{P=965.0hPa, T=22.3#circC}", "l");
  tl_gain.AddEntry(gain_after, "#splitline{After discharges}{P=965.3hPa, T=22.0#circC}", "l");
  tl_gain.SetBorderSize(0);

  can_gain.cd();

  gain_before->Draw("AP");
  gain_after->Draw("SAMEP");

  tl_gain.Draw("SAME");

  latex.Draw("SAME");
  
  can_gain.SetLogy();

  can_gain.Update();
  can_gain.Print("Gain.png", "png");
  
  fout->cd();
  can_gain.Write();
  
  return;
}//void Discharge_Hardness::Draw_Gain()

//////////

void Discharge_Hardness::Draw_Resolution()
{
  histo_before = adc_before.Get_Histo();
  histo_after = adc_after.Get_Histo();

  histo_before->SetLineColor(2);
  histo_after->SetLineColor(4);

  histo_before->SetTitle("");
  histo_before->GetXaxis()->SetTitle("ADC");
  histo_before->GetXaxis()->SetRangeUser(200, 1400);
  histo_before->GetYaxis()->SetRangeUser(0, 10000);
  
  TF1* fit_adc_before = (TF1*)(histo_before->GetListOfFunctions()->FindObject("Total"));
  TF1* fit_adc_after = (TF1*)(histo_after->GetListOfFunctions()->FindObject("Total"));

  fit_adc_before->SetLineColor(2);
  fit_adc_after->SetLineColor(4);

  tl_adc.AddEntry(histo_before, Form("#splitline{#splitline{Before discharges}{FWHM/#mu = %0.3f}}{P=965.0hPa, T=22.3#circC}", resolution_before), "l");
  tl_adc.AddEntry(histo_after, Form("#splitline{#splitline{After discharges}{FWHM/#mu = %0.3f}}{P=965.3hPa, T=22.0#circC}", resolution_after), "l");
  tl_adc.SetFillStyle(0);
  tl_adc.SetBorderSize(0);

  can_adc.cd();
  
  histo_before->Draw();
  histo_after->Draw("SAME");
 
  tl_adc.Draw("SAME");

  latex.Draw("SAME");
  
  can_adc.Update();
  can_adc.Print("ADC.png", "png");

  fout->cd();
  can_adc.Write();
  
  return;
}//void Discharge_Hardness::Draw_Resolution()

//////////

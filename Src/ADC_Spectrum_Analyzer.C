#include <ADC_Spectrum_Analyzer.h>

ClassImp(ADC_Spectrum_Analyzer);

//////////

ADC_Spectrum_Analyzer::ADC_Spectrum_Analyzer(const TString& a_data_file) : data_file(a_data_file)
{
  Int_t pos[2];
  pos[0] = data_file.find_last_of("/");
  pos[1] = data_file.find_last_of(".");

  string file_name = data_file.substr(pos[0]+1, pos[1]-pos[0]-1);
  string extension = data_file.substr(pos[1]+1);
  
  if(extension.compare("mca")==0)
    {
      cout << "DP5 type." << endl;
      read_mca_data = new DP5(data_file, "histo", "histo");   
    }
  else
    {
      cout << "MC2 type." << endl;
      read_mca_data = new MC2(data_file, "histo", "histo");
    }

  histo = read_mca_data->Get_Histo();

  file_name += ".root";
  fout = new TFile(file_name.c_str(), "RECREATE");
}//ADC_Spectrum_Analyzer(const TString& a_data_file);

//////////

ADC_Spectrum_Analyzer::~ADC_Spectrum_Analyzer()
{
  fout->cd();

  f1_peak->Write();
  f1_escape_peak->Write();
  f1_signal->Write();
  
  histo.Write();
  
  fout->Close();

  delete f1_peak;
  delete f1_escape_peak;
  delete f1_signal;
  delete read_mca_data;
}//ADC_Spectrum_Analyzer::~ADC_Spectrum_Analyzer();

//////////

Float_t ADC_Spectrum_Analyzer::Get_FWHM()
{
  cout << "Calculate FWHM." << endl;
  Float_t max = f1_peak->GetMaximum(fit_range_lower, fit_range_upper);
  Float_t max_x = f1_peak->GetMaximumX(fit_range_lower, fit_range_upper);
  
  Float_t lower = f1_peak->GetX(max/2., fit_range_lower, max_x);
  Float_t upper = f1_peak->GetX(max/2., max_x, fit_range_upper);

  fwhm = upper - lower;
  resolution = fwhm/2/max_x;
  
  cout << max << " " << max_x << " " << lower << " " << upper << " " << fwhm << " " << resolution << endl;

  return fwhm;
}//Float_t ADC_Spectrum_Analyzer::Get_FWHM()

//////////

Double_t ADC_Spectrum_Analyzer::Landau_Gaus(Double_t* x, Double_t* par)
{
  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;

  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++)
    {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }

  return (par[2] * step * sum * invsq2pi / par[3]);
}//Double_t ADC_Spectrum_Analyzer::Landau_Gaus(Double_t* x, Double_t* par) 

//////////

Double_t ADC_Spectrum_Analyzer::Polynomial(Double_t* x, Double_t* par)
{
  return par[0] + par[1]*x[0] + par[1]*x[0]*x[0] + par[2]*x[0]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0]*x[0];
}//Double_t ADC_Spectrum_Analyzer::Polynomial(Double_t* x, Double_t* par)

//////////

Double_t ADC_Spectrum_Analyzer::Signal(Double_t* x, Double_t* par)
{
  return Landau_Gaus(x, par) + Landau_Gaus(x, &par[4]);
}//Double_t ADC_Spectrum_Analyzer::Signal(Double_t* x, Double_t* par) 

//////////

Double_t ADC_Spectrum_Analyzer::Total(Double_t* x, Double_t* par)
{
  return Signal(x, par) + Polynomial(x, &par[8]);
}//Double_t ADC_Spectrum_Analyzer::Total(Double_t* x, Double_t* par)

//////////

void ADC_Spectrum_Analyzer::Fit_Histo()
{  
  histo.GetXaxis()->SetRange(200, 6000);
  Double_t peak_x = histo.GetMaximumBin();
  Double_t sd = histo.GetRMS();
  Double_t peak = histo.GetMaximum();
  
  histo.GetXaxis()->SetRange(0, -1);
  histo.GetYaxis()->SetRangeUser(0, 1.1*peak);

  TCanvas canvas_fit("canvas_fit", "canvas_fit", 800, 500);
  canvas_fit.Draw();
  canvas_fit.cd();

  histo.Draw();

  f1_peak = new TF1("Peak", Landau_Gaus, peak_x-5*sd, peak_x+5*sd, 4);
  f1_escape_peak = new TF1("Escape_peak", Landau_Gaus, peak_x-5*sd, peak_x+5*sd, 4);
  //TF1 f1_back("Back", Polynomial, peak_x-3*sd, peak_x+3*sd, 3);
  f1_signal = new TF1("Signal", Signal, peak_x-5*sd, peak_x+5*sd, 8);
  //f1_total = new TF1("Total", Total, peak_x-5*sd, peak_x+5*sd, 13);

  Double_t par[20] = {0};
  fill_n(par, sizeof(par)/sizeof(Double_t), 0.1);
  
  f1_peak->SetParameters(par);
  histo.Fit(f1_peak, "", "", peak_x-0.8*sd, peak_x+0.8*sd);

  f1_escape_peak->SetParameters(par);
  histo.Fit(f1_escape_peak, "", "", 0.5*(peak_x-0.8*sd), 0.5*(peak_x+0.8*sd));

  f1_peak->GetParameters(par);
  f1_escape_peak->GetParameters(&par[4]);
  f1_signal->SetParameters(par);

  //cout << par[0] << " " << par[1] << " " << par[4] << " " << par[5] << endl;

  fit_range_lower = par[5] - 5*par[4];
  fit_range_upper = par[1] + 5*par[0];

  histo.Fit(f1_signal, "", "", fit_range_lower, fit_range_upper);

  f1_signal->GetParameters(par);

  //f1_total->SetParameters(par);
  //histo.Fit(f1_total, "", "", par[5]-10*par[4], par[1]+8*par[0]);

  f1_peak->SetParameters(par);
  f1_escape_peak->SetParameters(&par[4]);

  f1_peak->SetRange(fit_range_lower, fit_range_upper);
  f1_escape_peak->SetRange(fit_range_lower, fit_range_upper);
  f1_signal->SetRange(fit_range_lower, fit_range_upper);
  
  return;
}//void ADC_Spectrum_Analyzer::Fit_Histo()

//////////

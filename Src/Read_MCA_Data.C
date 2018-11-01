#include "Read_MCA_Data.h"

ClassImp(Read_MCA_Data);

//////////

Read_MCA_Data::Read_MCA_Data(const TString& a_data_file, const TString& histo_name, const TString& histo_title) : data_file(a_data_file), histo(histo_name, histo_title, 8192, 0, 8192)
{
  fin.open(data_file);
  if(fin.is_open()==kFALSE)
    {
      cout << "Can not find data file! " << data_file << endl;
      exit(1);
    }
  
  Fill_Histo();
}//Read_MCA_Data::Read_MCA_Data()

//////////

Read_MCA_Data::~Read_MCA_Data()
{
  fin.close();
}//Read_MCA_Data::~Read_MCA_Data()

//////////

void Read_MCA_Data::Fit_Histo(const Bool_t& chk_draw)
{
  histo.GetXaxis()->SetRange(500, 3000);
  Double_t peak = histo.GetMaximum();
  
  histo.GetXaxis()->SetRange(0, -1);
  histo.GetYaxis()->SetRangeUser(0, 1.1*peak);
  
  TCanvas canvas_fit("canvas_fit", "canvas_fit", 800, 500);
  canvas_fit.Draw();
  canvas_fit.cd();
  
  histo.Draw();
  
  const Double_t fit_range_lower = 1000;
  const Double_t fit_range_upper = 2600;
  
  TF1 f1_peak("Peak", "gaus", fit_range_lower, fit_range_upper);
  TF1 f1_escape_peak("Escape_peak", "gaus", fit_range_lower, fit_range_upper);
  TF1 f1_back("Back", "pol3", fit_range_lower, fit_range_upper);
  TF1 f1_total("Total", "gaus(0)+gaus(3)+pol2(6)", fit_range_lower, fit_range_upper);

  histo.Fit(&f1_peak, "Q", "", 1800, 2200);
  histo.Fit(&f1_escape_peak, "Q", "", 1000, 1500);
  
  Double_t par[10] = {1};
  f1_peak.GetParameters(par);
  f1_escape_peak.GetParameters(&par[3]);
  f1_total.SetParameters(par);
  
  histo.Fit(&f1_total, "QR", "");

  if(chk_draw==kTRUE)
    {
      TString substr = data_file(data_file.Last('/')+1, data_file.Last('.')-data_file.Last('/')-1);
      TString print_name = substr+".png";
      canvas_fit.Print(print_name, "png");
    }
  
  return;
}//void Read_MCA_Data::Fit_Histo()

//////////

Float_t Read_MCA_Data::Get_Fit_Parameter(const Int_t& index)
{
  TF1* fit_func = (TF1*)(histo.GetListOfFunctions()->FindObject("Total"));

  Double_t parameter = fit_func->GetParameter(index);

  return parameter;
}//Float_t Read_MCA_Data::Get_Fit_Parameter(const Int_t& index)

//////////

Float_t Read_MCA_Data::Get_Fit_Par_Error(const Int_t& index)
{
  TF1* fit_func = (TF1*)(histo.GetListOfFunctions()->FindObject("Total"));

  Double_t par_error = fit_func->GetParError(index);

  return par_error;
}//Float_t Read_MCA_Data::Get_Fit_Par_Error(const Int_t& index)

//////////

void Read_MCA_Data::Fill_Histo()
{
  Bool_t chk_head = kFALSE;
  Int_t ibin = 0;
  
  while(!fin.eof())
    {
      string buf;
      
      getline(fin, buf);
      if(buf.compare("")==0) break;
      
      TString tbuf = buf;
      if(tbuf.Contains("<<DATA>>")==kTRUE && chk_head==kFALSE) chk_head = kTRUE; 
      else if(tbuf.Contains("<<END>>")==kTRUE && chk_head==kTRUE) break;
      else if(chk_head==kTRUE)
	{
	  ibin++;
	  Int_t content = tbuf.Atoi();
	  histo.SetBinContent(ibin, content);
	}
    }
  
  return;
}//void Read_MCA_Data::Fill_Histo()

//////////

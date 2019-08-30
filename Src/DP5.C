#include "DP5.h"

ClassImp(DP5);

//////////

DP5::DP5(const TString& a_data_file, const TString& a_histo_name, const TString& a_histo_title) : Read_MCA_Data(a_data_file, a_histo_name, a_histo_title)
{
  Read_Data();
}//DP5::DP5(const TString& a_data_file, const TString& histo_name, const TString& histo_title)

//////////

DP5::~DP5()
{
  delete histo;
}//DP5::~DP5()

//////////

void DP5::Read_Data()
{
  histo = new TH1D(histo_name, histo_title, 8192, 0, 8192);
  
  Bool_t chk_head = kFALSE;
  Int_t ibin = 0;

  while(!fin.eof())
    {
      string buf;
      
      getline(fin, buf);
      if(buf.compare("")==0) break;
      
      if(buf.find("<<DATA>>")!=string::npos && chk_head==kFALSE) chk_head = kTRUE;
      else if(buf.find("<<END>>")!=string::npos && chk_head==kTRUE) break;
      else if(chk_head==kTRUE)
	{
	  ibin++;
	  Int_t content = stoi(buf, nullptr);
	  histo->SetBinContent(ibin, content);
	}
    }
  
  return;
}//void DP5::Read_Data()

//////////

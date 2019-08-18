#include "MC2.h"

ClassImp(MC2);

//////////

MC2::MC2(const TString& a_data_file, const TString& a_histo_name, const TString& a_histo_title) : Read_MCA_Data(a_data_file, a_histo_name, a_histo_title) 
{
  Read_Data();
}//MC2::MC2(const TString& a_data_file, const TString& a_histo_name, const TString& a_histo_title)

//////////

MC2::~MC2()
{
  delete histo;
}//MC2::~MC2()

//////////

void MC2::Read_Data()
{
  //get bin number
  string buf;
  while(!fin.eof())
    {
      getline(fin, buf);
      
      Int_t found = buf.find("<EnergySkim>");
      if(found != string::npos) break;
    }

  getline(fin, buf);
  getline(fin, buf);

  Int_t found[2];
  found[0] = buf.find("<LLD>");
  found[1] = buf.find("</LLD>");
 
  Int_t low = stoi(buf.substr(found[0]+5, found[1]-found[0]-5), nullptr);
  
  getline(fin, buf);
  found[0] = buf.find("<ULD>");
  found[1] = buf.find("</ULD>");  
  
  Int_t up = stoi(buf.substr(found[0]+5, found[1]-found[0]-5), nullptr);

  Int_t n_bins = up - low + 1;
  cout << n_bins << endl; 

  histo = new TH1D(histo_name, histo_title, n_bins, 0, n_bins);
  
  Bool_t chk = kFALSE;
  Int_t count = 0;
  while(!fin.eof())
    {
      getline(fin, buf);

      string target = "<ChannelData compressionCode=\"None\"> ";
      Int_t found = buf.find(target);
      
      if(found!=string::npos)
	{
	  buf = buf.substr(found+target.length());
	  chk = kTRUE;
	}
      if(buf.find("</ChannelData>")!=string::npos) break;
	 
      if(chk==kTRUE)
	{
	  stringstream ss;
	  ss.str(buf);

	  for(Int_t i=0; i<12; i++)
	    {
	      count++;
	      if(n_bins<count) break;  

	      Int_t bin_content;
	      ss >> bin_content;

	      histo->SetBinContent(count, bin_content);
	    }
	}
    }
      
  return;
}//void MC2::Read_Data()

//////////

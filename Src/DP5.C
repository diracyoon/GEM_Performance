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
  
}//DP5::~DP5()

//////////

void DP5::Read_Data()
{
  string buf;
  while(!fin.eof())
    {
      getline(fin, buf);

      cout << buf << endl;
    }
    
  return;
}//void DP5::Read_Data()

//////////

#ifndef __Read_MCA_Data_h__
#define __Read_MCA_Data_h__

#include <iostream>
#include <fstream>

#include <TObject.h>
#include <TString.h>

using namespace std;

class Read_MCA_Data : public TObject
{
 public:
  Read_MCA_Data(const TString& data_file);
  ~Read_MCA_Data();

  TH1D Get_Histo();
  
 private:
  Fill_Histo();
  
  ifstream fin;
  
  ClassDef(Read_MCA_Data, 1);
};

#endif /* __Read_MCA_Data_h__ */

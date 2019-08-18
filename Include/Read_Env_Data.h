#ifndef __Read_Env_Data_h__
#define __Read_Env_Data_h__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <TObject.h>
#include <TString.h>

using namespace std;

namespace namespace_Env_Data
{
  typedef struct _Data
  {
    Int_t year;
    Int_t month;
    Int_t day;
    Int_t hour;
    Int_t min;
    Int_t sec;
    Float_t temperature;
    Float_t pressure;
    Float_t relative_humidity;
  } Data;
}  

class Read_Env_Data : public TObject
{
 public:
  Read_Env_Data(const TString& a_data_file, vector<namespace_Env_Data::Data>& a_vec_data);
  ~Read_Env_Data();
  
 protected:
  TString data_file;

  vector<namespace_Env_Data::Data>& vec_data;
  
  Int_t year;
  Int_t month;
  Int_t day;
  
  ifstream fin;
  
  void Read_Data();
    
  ClassDef(Read_Env_Data, 1);
};

#endif /* _Read_Env_Data_h__ */

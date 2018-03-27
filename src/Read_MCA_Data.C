#include "Read_MCA_Data.h"

ClassImp(Read_MCA_Data);

//////////

Read_MCA_Data::Read_MCA_Data(const TString& data_file)
{
  fin.open(data_file);
  
}//Read_MCA_Data::Read_MCA_Data()

//////////

Read_MCA_Data::~Read_MCA_Data()
{
}//Read_MCA_Data::~Read_MCA_Data()

//////////


//////////

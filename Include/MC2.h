#ifndef __MC2_h__
#define __MC2_h__

#include <iostream>
#include <sstream>

#include <TString.h>

#include <Read_MCA_Data.h>

class MC2 : public Read_MCA_Data
{
 public:
  MC2(const TString& a_data_file, const TString& a_histo_name="histo", const TString& a_histo_title="histo");
  virtual ~MC2();

 protected:
  virtual void Read_Data();
  
  ClassDef(MC2, 1);
};

#endif /* __MC2_h__ */

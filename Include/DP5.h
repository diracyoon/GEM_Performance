#ifndef __DP5_h__
#define __DP5_h__

#include <iostream>

#include <Read_MCA_Data.h>

class DP5 : public Read_MCA_Data
{
 public:
  DP5(const TString& a_data_file, const TString& a_histo_name="histo", const TString& a_histo_title="histo");
  virtual ~DP5();
  
 protected:
  virtual void Read_Data(); 
    
  ClassDef(DP5, 1);
};

#endif /* __DP5_h__ */

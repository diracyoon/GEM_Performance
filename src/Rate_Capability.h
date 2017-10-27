#ifndef __Rate_Capability_h__
#define __Rate_Capability_h__

#include <TObject.h>
#include <TFile.h>


using namespace std;

class Rate_Capability :  public TObject
{
 public:
  Rate_Capability();
  ~Rate_Capability();
  
 protected:

  ClassDef(Rate_Capability, 1);
};

#endif /* __Rate_Capability_h__ */

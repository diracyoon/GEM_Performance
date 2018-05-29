#ifndef __Charging_Up_h__
#define __Charging_Up_h__

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TH1D.h"

#include "Read_MCA_Data.h"

using namespace std;

class Charging_Up : public TObject
{
 public:
  Charging_Up(const TString& a_path);
  ~Charging_Up();

  void Run();
  
 protected:
  TString path;
  TGraphErrors gr_time_peak;

  ClassDef(Charging_Up, 1);
};

#endif /* __Charging_Up_h__ */

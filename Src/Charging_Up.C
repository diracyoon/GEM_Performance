#include "Charging_Up.h"

ClassImp(Charging_Up);

//////////

Charging_Up::Charging_Up(const TString& a_path) : path(a_path)
{
}//Charging_Up::Charging_Up()

//////////

Charging_Up::~Charging_Up()
{
}//Charging_Up::~Charging_Up()

//////////

void Charging_Up::Run()
{
  TString path_data = path + "/Ref_A_Initial.mca";
  Read_MCA_Data mca_ref_a_initial(path_data, "Ref_A_Initial", "Ref_A_Initial");
  mca_ref_a_initial.Fit_Histo();
  cout << mca_ref_a_initial.Get_Fit_Parameter(1) << " " << mca_ref_a_initial.Get_Fit_Par_Error(1) << endl;
  
  path_data = path + "/Ref_A_Final.mca";
  Read_MCA_Data mca_ref_a_final(path_data, "Ref_A_Final", "Ref_A_Final");
  mca_ref_a_final.Fit_Histo();
  cout << mca_ref_a_final.Get_Fit_Parameter(1) << " " << mca_ref_a_final.Get_Fit_Par_Error(1) << endl;

  path_data = path + "/Ref_C_Initial.mca";
  Read_MCA_Data mca_ref_c_initial(path_data, "Ref_C_Initial", "Ref_C_Initial");
  mca_ref_c_initial.Fit_Histo();
  cout << mca_ref_c_initial.Get_Fit_Parameter(1) << " " << mca_ref_c_initial.Get_Fit_Par_Error(1) << endl;

  path_data = path + "/Ref_C_Final.mca";
  Read_MCA_Data mca_ref_c_final(path_data, "Ref_C_Final", "Ref_C_Final");
  mca_ref_c_final.Fit_Histo();
  cout << mca_ref_c_final.Get_Fit_Parameter(1) << " " << mca_ref_c_final.Get_Fit_Par_Error(1) << endl;
  
  
  for(Int_t i=1; i<3; i++)
    {
      path_data = path;
      path_data += "/Charging_Up_Data_";
      path_data += string(3 - to_string(i).length(), '0') + to_string(i);
      path_data += ".mca";

      Read_MCA_Data read_mca_data(path_data, to_string(i), to_string(i));

      read_mca_data.Fit_Histo();

      Double_t peak_fit = read_mca_data.Get_Fit_Parameter(1);
      Double_t peak_error_fit = read_mca_data.Get_Fit_Par_Error(1);
      
      Int_t n_point = gr_time_peak.GetN();
      gr_time_peak.SetPoint(n_point, n_point, peak_fit);
      gr_time_peak.SetPointError(n_point, 0, peak_error_fit);
    }

  TCanvas canvas("canvas", "canvas", 800, 500);
  canvas.Draw();
  
  gr_time_peak.Draw("AP*");
  
  canvas.Print("Peak_Time.png", "png");
  
  return;
}//void Charging_Up::Run()

//////////

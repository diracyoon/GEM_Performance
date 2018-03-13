void Run_Rate_Capability(const TString path)
{
  Rate_Capability* rate_capability = new Rate_Capability("690uA", path);

  rate_capability->Read_Single_Layer_Data(10);
  // rate_capability->Read_Single_Layer_Data(4);
  // rate_capability->Read_Single_Layer_Data(1);
  // rate_capability->Read_Single_Layer_Data(0);
  
  rate_capability->Calculate_Measured_Rate(10);
  // rate_capability->Calculate_Measured_Rate(4);
  // rate_capability->Calculate_Measured_Rate(1);
  // rate_capability->Calculate_Measured_Rate(0);

  rate_capability->Calculate_Expected_Rate(10);
  // rate_capability->Calculate_Expected_Rate(4);
  // rate_capability->Calculate_Expected_Rate(1);
  // rate_capability->Calculate_Expected_Rate(0);
    
  rate_capability->Calculate_Single_Layer_Gain(10);
  // rate_capability->Calculate_Single_Layer_Gain(4);
  // rate_capability->Calculate_Single_Layer_Gain(1);
  // rate_capability->Calculate_Single_Layer_Gain(0);

  // rate_capability->Renormalize_Attenuation_Gain();
  // rate_capability->Draw_Multi_Layer_Gain(kTRUE);
  
  delete rate_capability;
  
  return;
}

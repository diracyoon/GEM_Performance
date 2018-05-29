void Run_Draw_ADC()
{
  TString path = "/home/isyoon/GEM_Performance/macro/ADC_Distribution/Data/Data_Layer_Dependence/660uA/";

  Int_t layer[] = {0, 1, 2, 4, 6, 8, 10, 12};
  Int_t n_data = sizeof(layer)/sizeof(Int_t);
  
  TCanvas* c0 = new TCanvas("c0", "c0", 1400, 800);
  c0->Divide(3, 3);
  c0->Draw();

  TH1D histo[n_data];
  Read_MCA_Data* read_mca_data[n_data];
  for(Int_t i=0; i<n_data; i++)
    {
      TString file = path;
      file += "HV3422_660uA_Xray_30_05_Range_10V_Cu_";
      file += to_string(layer[i]);
      file += ".mca";

      TString histo_name = to_string(layer[i]);
      histo_name += "Layers";
      
      read_mca_data[i] = new Read_MCA_Data(file, histo_name, histo_name);
  
      histo[i] = read_mca_data[i]->Get_Histo();
      
      c0->cd(i+1);
      histo[i].Draw();
      
      histo[i].Fit("pol5", "Q", "", 30, 200);

       TF1* fit_func = (TF1*)(histo[i].GetListOfFunctions()->FindObject("pol5"));
       Int_t minimum = fit_func->GetMinimumX();
       cout << minimum << endl;
      
      histo[i].GetXaxis()->SetRangeUser(minimum, 8192);
      //c0->GetPad(i+1)->Update();
      
      
    }
  
  c0->Print("Test.pdf", "pdf");
  c0->SaveAs("Test.C", "C");

  for(Int_t i=0; i<n_data; i++) delete read_mca_data[i];
  
  return;
}

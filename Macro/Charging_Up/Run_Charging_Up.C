void Run_Charging_Up()
{
  Charging_Up* charging_up = new Charging_Up("/home/isyoon/GEM_Performance/Macro/Charging_Up/Data");

  charging_up->Run();
  
  delete charging_up;
  
  return;
}

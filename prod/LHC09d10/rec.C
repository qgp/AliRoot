void rec() {

  AliReconstruction reco;
// switch off cleanESD
  reco.SetCleanESD(kFALSE);

  
  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();
  reco.SetRunQA("ALL:ALL");
// ITS Efficiency
  reco.SetRunPlaneEff(kTRUE);

  reco.SetDefaultStorage("alien://Folder=/alice/simulation/2008/v4-15-Release/Residual/");
//   reco.SetSpecificStorage("GRP/GRP/Data",
// 	                        Form("local://%s",gSystem->pwd()));

// We store the object in AliEn during the simulation

 reco.SetSpecificStorage("GRP/GRP/Data",
                         "alien://Folder=/alice/simulation/2008/v4-15-Release/Ideal/");
// Vertex
//
  reco.SetSpecificStorage("GRP/Calib/MeanVertexSPD",     "alien://folder=/alice/data/2009/OCDB");
  reco.SetSpecificStorage("GRP/Calib/MeanVertexTPC",     "alien://folder=/alice/data/2009/OCDB");
  reco.SetSpecificStorage("GRP/Calib/MeanVertex",        "alien://folder=/alice/data/2009/OCDB");
//
// SPD
//
  reco.SetSpecificStorage("ITS/Calib/SPDDead",           "alien://folder=/alice/data/2009/OCDB");
  reco.SetSpecificStorage("TRIGGER/SPD/PITConditions",   "alien://folder=/alice/data/2009/OCDB");		
//
// Reco Param
  reco.SetSpecificStorage("ITS/Calib/RecoParam","alien://folder=/alice/data/2009/OCDB");
  reco.SetSpecificStorage("GRP/Calib/RecoParam","alien://folder=/alice/data/2009/OCDB"); 
//
// TPC
//
  reco.SetSpecificStorage("TPC/Calib/PadGainFactor",     "alien://folder=/alice/data/2009/OCDB");		
//
// PMD: this object is missing in the MC OCDB
//
  reco.SetSpecificStorage("PMD/Calib/NoiseCut",     "alien://folder=/alice/data/2009/OCDB");		

//
  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}

void sim(Int_t nev=4) {
  gSystem->Load("libProof");
  gSystem->Load("libGui");
  if (!strcmp(gSystem->GetBuildArch(),"macosx")) gSystem->Load("libf95");
  gROOT->Macro("loadlibssim.C");
  new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  AliSimulation simulator;
  simulator.SetMakeSDigits("TOF HMPID VZERO PMD MUON");
  simulator.SetMakeDigitsFromHits("ITS TPC");
  //  simulator.SetWriteRawData("ALL","raw.root",kTRUE);
 
  TStopwatch timer;
  timer.Start();
  simulator.Run(nev);
  timer.Stop();
  timer.Print();
}

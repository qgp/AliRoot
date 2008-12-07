void rec() {
  gROOT->Macro("loadlibsrec.C");
  new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  AliReconstruction reco;

  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();

  reco.SetRecoParam("ITS",AliITSRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TPC",AliTPCRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TRD",AliTRDrecoParam::GetLowFluxParam());
  reco.SetRecoParam("PHOS",AliPHOSRecoParam::GetDefaultParameters());
  reco.SetRecoParam("MUON",AliMUONRecoParam::GetLowFluxParam());

  reco.SetDefaultStorage("local://$ALICE_ROOT");
  reco.SetSpecificStorage("GRP/GRP/Data",
			  Form("local://%s",gSystem->pwd()));

  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}

void rec() {
  AliReconstruction reco;
  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();


  reco.SetRecoParam("ITS",AliITSRecoParam::GetHighFluxParam());
  reco.SetRecoParam("TPC",AliTPCRecoParam::GetHighFluxParam());
  reco.SetRecoParam("TRD",AliTRDrecoParam::GetHighFluxParam());
  reco.SetRecoParam("PHOS",AliPHOSRecoParam::GetDefaultParameters());
  reco.SetRecoParam("MUON",AliMUONRecoParam::GetHighFluxParam());

  // Use the GRP from the backgr
  reco.SetDefaultStorage("local://$ALICE_ROOT");
  reco.SetSpecificStorage("GRP/GRP/Data",
			  Form("local://%s/../backgr",gSystem->pwd()));

  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}

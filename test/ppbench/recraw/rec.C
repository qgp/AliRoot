void rec() {

  AliReconstruction reco;

  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();

  reco.SetRecoParam("ITS",AliITSRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TPC",AliTPCRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TRD",AliTRDrecoParam::GetLowFluxParam());
  reco.SetRecoParam("PHOS",AliPHOSRecoParam::GetDefaultParameters());
  reco.SetRecoParam("MUON",AliMUONRecoParam::GetLowFluxParam());

  reco.SetInput("raw.root");

  reco.SetNumberOfEventsPerFile(-1); // all events in one single file

  reco.SetDefaultStorage("local://$ALICE_ROOT");
  reco.SetSpecificStorage("GRP/GRP/Data",
			  Form("local://%s/..",gSystem->pwd()));

  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}

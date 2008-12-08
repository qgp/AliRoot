void recraw() {

	// AliReconstruction settings
  AliReconstruction reco;

  reco.SetWriteESDfriend(kTRUE);
  reco.SetWriteAlignmentData();
  reco.SetInput("raw.root");

  reco.SetRecoParam("ITS",AliITSRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TPC",AliTPCRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TRD",AliTRDrecoParam::GetLowFluxParam());
  reco.SetRecoParam("PHOS",AliPHOSRecoParam::GetDefaultParameters());
  reco.SetRecoParam("MUON",AliMUONRecoParam::GetLowFluxParam());

  reco.SetDefaultStorage("local://$ALICE_ROOT");
  reco.SetSpecificStorage("GRP/GRP/Data",
			  Form("local://%s/..",gSystem->pwd()));


  reco.SetUseTrackingErrorsForAlignment("ITS");
	
  // all events in one single file
  reco.SetNumberOfEventsPerFile(-1);

  // switch off cleanESD
  reco.SetCleanESD(kFALSE);

  AliQA::SetQARefStorage("local://$ALICE_ROOT") ;
  for (Int_t det = 0 ; det < AliQA::kNDET ; det++) {
    reco.SetQACycles(det, 9999) ;
    reco.SetQAWriteExpert(det) ; 
  }
  
  AliLog::Flush();
	
  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}


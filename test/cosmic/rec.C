void rec(const char *filename="raw.root")
{
  /////////////////////////////////////////////////////////////////////////////////////////
  //
  // First version of the reconstruction
  // script for the 2008 cosmic data (LHC08b)
  //
  /////////////////////////////////////////////////////////////////////////////////////////

  // Debug options
  // AliLog::SetGlobalDebugLevel(2);
  // AliLog::SetModuleDebugLevel("STEER",2);

  // Set the CDB storage location
  AliCDBManager * man = AliCDBManager::Instance();
  //  man->SetDefaultStorage("local://$ALICE_ROOT");
  man->SetDefaultStorage("alien://folder=/alice/data/2008/LHC08b/OCDB/");
  
  // Example in case a specific CDB storage is needed
  //  man->SetSpecificStorage("ITS/Calib/MapsAnodeSDD","local://$ALICE_ROOT");

  // ITS settings
  AliITSRecoParam * itsRecoParam = AliITSRecoParam::GetCosmicTestParam();
  itsRecoParam->SetFactorSAWindowSizes(20);
  itsRecoParam->SetClusterErrorsParam(2);
  itsRecoParam->SetFindV0s(kFALSE);
  itsRecoParam->SetAddVirtualClustersInDeadZone(kFALSE);
  itsRecoParam->SetUseAmplitudeInfo(kFALSE);
  // In case we want to switch off a layer
  //  itsRecoParam->SetLayerToSkip(<N>);
  //  itsRecoParam->SetLayerToSkip(4);
  itsRecoParam->SetLayerToSkip(5);
  itsRecoParam->SetLayerToSkip(2);
  itsRecoParam->SetLayerToSkip(3);
  //itsRecoParam->SetSAOnePointTracks();
  itsRecoParam->SetClusterMisalError(1.0); // [cm]
  itsRecoParam->SetSAUseAllClusters();
  AliITSReconstructor::SetRecoParam(itsRecoParam);

  // TPC settings
  AliLog::SetClassDebugLevel("AliTPCclustererMI",2);
  AliTPCRecoParam * tpcRecoParam = AliTPCRecoParam::GetCosmicTestParam(kFALSE);
  tpcRecoParam->SetTimeInterval(60,940);
  tpcRecoParam->Dump();
  AliTPCReconstructor::SetRecoParam(tpcRecoParam);
  AliTPCReconstructor::SetStreamLevel(1);

  // TRD setting
  // Settings for the TRD Raw Reader
  AliTRDrawStreamBase::SetRawStreamVersion("TB");
  AliTRDrawStreamTB::SetNoErrorWarning();
  AliTRDrawStreamTB::AllowCorruptedData();
  AliTRDrawStreamTB::DisableStackNumberChecker();
  AliTRDrawStreamTB::DisableStackLinkNumberChecker();
  AliTRDrawStreamTB::SetSubtractBaseline(10);
  
  // TRD reconstruction params
  AliTRDrecoParam *fTRDrecoParam = AliTRDrecoParam::GetCosmicTestParam();
  AliTRDReconstructor::SetRecoParam(fTRDrecoParam);
  AliTRDtrackerV1::SetNTimeBins(30);

  // PHOS settings
  AliPHOSRecoParam* recEmc = new AliPHOSRecoParamEmc();
  recEmc->SetSubtractPedestals(kTRUE);
  recEmc->SetMinE(0.05);
  recEmc->SetClusteringThreshold(0.10);
  AliPHOSReconstructor::SetRecoParamEmc(recEmc);

  // T0 settings
  AliLog::SetModuleDebugLevel("T0", 10);

  // MUON settings
  AliLog::SetClassDebugLevel("AliMUONRawStreamTracker",3);
  AliMUONRecoParam *muonRecoParam = AliMUONRecoParam::GetLowFluxParam();
  muonRecoParam->CombineClusterTrackReco(kTRUE);
  muonRecoParam->SetCalibrationMode("NOGAIN");
  //muonRecoParam->SetClusteringMode("PEAKFIT");
  //muonRecoParam->SetClusteringMode("PEAKCOG");
  muonRecoParam->Print("FULL");
  AliRecoParam::Instance()->RegisterRecoParam(muonRecoParam);
 
  // Tracking settings
  //  AliMagFMaps* field = new AliMagFMaps("Maps","Maps", 2, 1., 10., 1);
  AliMagFMaps* field = new AliMagFMaps("Maps","Maps", 2, 0., 10., 2);
  AliTracker::SetFieldMap(field,1);
  Double_t mostProbPt=0.35;
  AliExternalTrackParam::SetMostProbablePt(mostProbPt);

  // AliReconstruction settings
  AliReconstruction rec;
  rec.SetUniformFieldTracking(kFALSE);
  rec.SetWriteESDfriend(kTRUE);
  rec.SetWriteAlignmentData();
  rec.SetInput(filename);
  rec.SetRunReconstruction("ALL");
  rec.SetUseTrackingErrorsForAlignment("ITS");

  // In case some detectors have to be switched off...
  //  rec.SetRunLocalReconstruction("ALL");
  //  rec.SetRunTracking("ALL");
  //  rec.SetFillESD("ALL");
  // Enable vertex finder - it is needed for cosmic track reco
  rec.SetRunVertexFinder(kTRUE);

  // To be enabled if some equipment IDs are not set correctly by DAQ
  //  rec.SetEquipmentIdMap("EquipmentIdMap.data");

  // Detector options if any
  rec.SetOption("ITS","cosmics");
  rec.SetOption("MUON","SAVEDIGITS");
  rec.SetOption("T0","cosmic");

  // To be enabled when CTP readout starts
  rec.SetFillTriggerESD(kFALSE);

  // all events in one single file
  rec.SetNumberOfEventsPerFile(-1);

  // switch off cleanESD
  rec.SetCleanESD(kFALSE);

  // rec.SetEventRange(0,15);

  rec.SetRunQA("ITS TPC:ESD RECPOINT");
  rec.SetRunGlobalQA(kTRUE);
  AliLog::Flush();
  rec.Run();

  //cout << "-----------------------------------------------------------------" << endl;
  //cout << "-----------------------------------------------------------------" << endl;
  //cout << "--------- Reconstruction Completed. Start merging QAs -----------" << endl;
  //cout << "-----------------------------------------------------------------" << endl;
  //cout << "-----------------------------------------------------------------" << endl;
  //AliQADataMakerSteer qas;
  //qas.Merge();
}

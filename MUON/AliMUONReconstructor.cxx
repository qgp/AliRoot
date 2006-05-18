/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for MUON reconstruction                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliMUONReconstructor.h"

#include "AliESD.h"
#include "AliESDMuonTrack.h"
#include "AliLog.h"
#include "AliMUON.h"
#include "AliMUONCalibrationData.h"
#include "AliMUONClusterFinderAZ.h"
#include "AliMUONClusterFinderVS.h"
#include "AliMUONClusterReconstructor.h"
#include "AliMUONData.h"
#include "AliMUONDigitCalibrator.h"
#include "AliMUONEventRecoCombi.h" 
#include "AliMUONRawReader.h"
#include "AliMUONTrack.h"
#include "AliMUONTrackParam.h"
#include "AliMUONTrackReconstructor.h"
#include "AliMUONTriggerTrack.h"
#include "AliRawReader.h"
#include "AliRun.h"
#include "AliRunLoader.h"
#include "TTask.h"
#include "TStopwatch.h"

ClassImp(AliMUONReconstructor)

//_____________________________________________________________________________
AliMUONReconstructor::AliMUONReconstructor()
  : AliReconstructor(), fCalibrationData(0x0)
{
/// Default constructor

    AliDebug(1,"");
}

//______________________________________________________________________________
AliMUONReconstructor::AliMUONReconstructor(const AliMUONReconstructor& right) 
  : AliReconstructor(right) 
{  
/// Protected copy constructor (not implemented)

  AliFatal("Copy constructor not provided.");
}

//_____________________________________________________________________________
AliMUONReconstructor::~AliMUONReconstructor()
{
/// Destructor

  AliDebug(1,"");
  delete fCalibrationData;
}

//______________________________________________________________________________
AliMUONReconstructor& 
AliMUONReconstructor::operator=(const AliMUONReconstructor& right)
{
/// Protected assignement operator (not implemented)

  // check assignement to self
  if (this == &right) return *this;

  AliFatal("Assignement operator not provided.");
    
  return *this;  
}    

//_____________________________________________________________________________
TTask* 
AliMUONReconstructor::GetCalibrationTask(AliMUONData* data) const
{
/// Create the calibration task(s). 
  
  const AliRun* run = fRunLoader->GetAliRun();
  
  // Not really clean, but for the moment we must check whether the
  // simulation has decalibrated the data or not...
  const AliMUON* muon = static_cast<AliMUON*>(run->GetModule("MUON"));
  if ( muon->DigitizerType().Contains("NewDigitizer") )
  {
    AliInfo("Calibration will occur.");
    Int_t runNumber = run->GetRunNumber();     
    fCalibrationData = new AliMUONCalibrationData(runNumber);
    if ( !fCalibrationData->IsValid() )
    {
      AliError("Could not retrieve calibrations !");
      delete fCalibrationData;
      fCalibrationData = 0x0;
      return 0x0;
    }    
    TTask* calibration = new TTask("MUONCalibrator","MUON Digit calibrator");
    calibration->Add(new AliMUONDigitCalibrator(data,fCalibrationData));
    //FIXME: calibration->Add(something about dead channels should go here).
    return calibration;
  }
  else
  {
    AliInfo("Detected the usage of old digitizer (w/o decalibration). "
            "Will not calibrate then.");
    return 0x0;
  }
}

//_____________________________________________________________________________
void
AliMUONReconstructor::Init(AliRunLoader* runLoader)
{
/// Initialize

  fRunLoader = runLoader;
}

//_____________________________________________________________________________
void AliMUONReconstructor::Reconstruct(AliRunLoader* runLoader) const
{
/// Reconstruct
/// \todo add more

  AliLoader* loader = runLoader->GetLoader("MUONLoader");
  Int_t nEvents = runLoader->GetNumberOfEvents();

  AliMUONData* data = new AliMUONData(loader,"MUON","MUON");

// passing loader as argument.
  AliMUONTrackReconstructor* recoEvent = new AliMUONTrackReconstructor(loader, data);

  if (strstr(GetOption(),"Original")) 
    recoEvent->SetTrackMethod(1); // Original tracking
  else if (strstr(GetOption(),"Combi")) 
    recoEvent->SetTrackMethod(3); // Combined cluster / track
  else
    recoEvent->SetTrackMethod(2); // Kalman

  AliMUONClusterReconstructor* recoCluster = new AliMUONClusterReconstructor(data);
  
  AliMUONClusterFinderVS *recModel = recoCluster->GetRecoModel();

  if (!strstr(GetOption(),"VS")) {
    recModel = (AliMUONClusterFinderVS*) new AliMUONClusterFinderAZ();
    recoCluster->SetRecoModel(recModel);
  }
  recModel->SetGhostChi2Cut(10);

  loader->LoadDigits("READ");
  loader->LoadRecPoints("RECREATE");
  loader->LoadTracks("RECREATE");
  
  TTask* calibration = GetCalibrationTask(data);
  
  Int_t chBeg = recoEvent->GetTrackMethod() == 3 ? 6 : 0; 
  //   Loop over events              
  for(Int_t ievent = 0; ievent < nEvents; ievent++) {

    AliDebug(1,Form("Event %d",ievent));
    
    runLoader->GetEvent(ievent);

    //----------------------- digit2cluster & Trigger2Trigger -------------------
    if (!loader->TreeR()) loader->MakeRecPointsContainer();
     
    // tracking branch
    if (recoEvent->GetTrackMethod() != 3) {
      data->MakeBranch("RC");
      data->SetTreeAddress("D,RC");
    } else {
      data->SetTreeAddress("D");
      data->SetTreeAddress("RCC");
    }
    // Important for avoiding a memory leak when reading digits ( to be investigated more in detail)
    // In any case the reading of GLT is needed for the Trigger2Tigger method below
    data->SetTreeAddress("GLT");

    data->GetDigits();
    
    if ( calibration ) 
    {
      calibration->ExecuteTask();
    }
    
    recoCluster->Digits2Clusters(chBeg); 
    
    if (recoEvent->GetTrackMethod() == 3) {
      // Combined cluster / track finder
      AliMUONEventRecoCombi::Instance()->FillEvent(data, (AliMUONClusterFinderAZ*)recModel);
      ((AliMUONClusterFinderAZ*) recModel)->SetReco(2); 
    }
    else data->Fill("RC"); 

    // trigger branch
    data->MakeBranch("TC");
    data->SetTreeAddress("TC");
    recoCluster->Trigger2Trigger(); 
    data->Fill("TC");

    //AZ loader->WriteRecPoints("OVERWRITE");

    //---------------------------- Track & TriggerTrack ---------------------
    if (!loader->TreeT()) loader->MakeTracksContainer();

    // trigger branch
    data->MakeBranch("RL"); //trigger track
    data->SetTreeAddress("RL");
    recoEvent->EventReconstructTrigger();
    data->Fill("RL");

    // tracking branch
    data->MakeBranch("RT"); //track
    data->SetTreeAddress("RT");
    recoEvent->EventReconstruct();
    data->Fill("RT");

    loader->WriteTracks("OVERWRITE"); 
  
    if (recoEvent->GetTrackMethod() == 3) { 
      // Combined cluster / track
      ((AliMUONClusterFinderAZ*) recModel)->SetReco(1);
      data->MakeBranch("RC");
      data->SetTreeAddress("RC");
      AliMUONEventRecoCombi::Instance()->FillRecP(data, recoEvent); 
      data->Fill("RC"); 
    }
    loader->WriteRecPoints("OVERWRITE"); 

    //--------------------------- Resetting branches -----------------------
    data->ResetDigits();
    data->ResetRawClusters();
    data->ResetTrigger();

    data->ResetRawClusters();
    data->ResetTrigger();
    data->ResetRecTracks();  
    data->ResetRecTriggerTracks();

  }
  loader->UnloadDigits();
  loader->UnloadRecPoints();
  loader->UnloadTracks();

  delete recoCluster;
  delete recoEvent;
  delete data;
  delete calibration;
}

//_____________________________________________________________________________
void AliMUONReconstructor::Reconstruct(AliRunLoader* runLoader, AliRawReader* rawReader) const
{
/// Recontruct
/// \todo add more

  //  AliLoader
  AliLoader* loader = runLoader->GetLoader("MUONLoader");
  AliMUONData data(loader,"MUON","MUON");

  // passing loader as argument.
  AliMUONTrackReconstructor recoEvent(loader, &data);

  AliMUONRawReader rawData(&data);

  AliMUONClusterReconstructor recoCluster(&data);

  if (strstr(GetOption(),"Original")) 
  {
    recoEvent.SetTrackMethod(1); // Original tracking
  }
  else
  {
    recoEvent.SetTrackMethod(2); // Kalman
  }
  
  AliMUONClusterFinderVS *recModel = recoCluster.GetRecoModel();
  if (!strstr(GetOption(),"VS")) 
  {
    recModel = (AliMUONClusterFinderVS*) new AliMUONClusterFinderAZ();
    recoCluster.SetRecoModel(recModel);
  }
  recModel->SetGhostChi2Cut(10);

  TTask* calibration = GetCalibrationTask(&data);
  
  loader->LoadRecPoints("RECREATE");
  loader->LoadTracks("RECREATE");
  loader->LoadDigits("READ");
  
  //   Loop over events  
  Int_t iEvent = 0;
           
  TStopwatch totalTimer;
  TStopwatch rawTimer;
  TStopwatch calibTimer;
  TStopwatch clusterTimer;
  TStopwatch trackingTimer;
  
  rawTimer.Start(kTRUE); rawTimer.Stop();
  calibTimer.Start(kTRUE); calibTimer.Stop();
  clusterTimer.Start(kTRUE); clusterTimer.Stop();
  trackingTimer.Start(kTRUE); trackingTimer.Stop();
  
  totalTimer.Start(kTRUE);
  
  while (rawReader->NextEvent()) 
  {
    AliDebug(1,Form("Event %d",iEvent));
    
    runLoader->GetEvent(iEvent++);

    //----------------------- raw2digits & raw2trigger-------------------
    if (!loader->TreeD()) 
    {
      AliDebug(1,Form("Making Digit Container for event %d",iEvent));
      loader->MakeDigitsContainer();
    }
    
    data.SetTreeAddress("D,GLT");
    rawTimer.Start(kFALSE);
    rawData.Raw2Digits(rawReader);
    rawTimer.Stop();
    
    if ( calibration )
    {
      calibTimer.Start(kFALSE);
      calibration->ExecuteTask();
      calibTimer.Stop();
    }
  
    //----------------------- digit2cluster & Trigger2Trigger -------------------
    clusterTimer.Start(kFALSE);

    if (!loader->TreeR()) loader->MakeRecPointsContainer();
     
    // tracking branch
    data.MakeBranch("RC");
    data.SetTreeAddress("RC");
    recoCluster.Digits2Clusters(); 
    data.Fill("RC"); 

    // trigger branch
    data.MakeBranch("TC");
    data.SetTreeAddress("TC");
//    recoCluster.Trigger2Trigger(); 
    data.Fill("TC");
    
    loader->WriteRecPoints("OVERWRITE");

    clusterTimer.Stop();

    //---------------------------- Track & TriggerTrack ---------------------
    trackingTimer.Start(kFALSE);
    if (!loader->TreeT()) loader->MakeTracksContainer();

    // trigger branch
    data.MakeBranch("RL"); //trigger track
    data.SetTreeAddress("RL");
    recoEvent.EventReconstructTrigger();
    data.Fill("RL");

    // tracking branch
    data.MakeBranch("RT"); //track
    data.SetTreeAddress("RT");
    recoEvent.EventReconstruct();
    data.Fill("RT");

    loader->WriteTracks("OVERWRITE");  
    trackingTimer.Stop();
    
    //--------------------------- Resetting branches -----------------------
    data.ResetDigits();
    data.ResetRawClusters();
    data.ResetTrigger();

    data.ResetRawClusters();
    data.ResetTrigger();
    data.ResetRecTracks();
    data.ResetRecTriggerTracks();
  
  }
  
  totalTimer.Stop();
  
  loader->UnloadRecPoints();
  loader->UnloadTracks();
  loader->UnloadDigits();

  AliInfo(Form("Execution time for converting RAW data to digits in MUON : R:%.2fs C:%.2fs",
               rawTimer.RealTime(),rawTimer.CpuTime()));
  AliInfo(Form("Execution time for calibrating MUON : R:%.2fs C:%.2fs",
               calibTimer.RealTime(),calibTimer.CpuTime()));
  AliInfo(Form("Execution time for clusterizing MUON : R:%.2fs C:%.2fs",
               clusterTimer.RealTime(),clusterTimer.CpuTime()));
  AliInfo(Form("Execution time for tracking MUON : R:%.2fs C:%.2fs",
               trackingTimer.RealTime(),trackingTimer.CpuTime()));
  AliInfo(Form("Total Execution time for Reconstruct(from raw) MUON : R:%.2fs C:%.2fs",
               totalTimer.RealTime(),totalTimer.CpuTime()));
}

//_____________________________________________________________________________
void AliMUONReconstructor::FillESD(AliRunLoader* runLoader, AliESD* esd) const
{
/// Fill ESD
/// \todo add more

  TClonesArray* recTracksArray = 0;
  TClonesArray* recTrigTracksArray = 0;
  
  AliLoader* loader = runLoader->GetLoader("MUONLoader");
  loader->LoadTracks("READ");
  AliMUONData* muonData = new AliMUONData(loader,"MUON","MUON");

   // declaration  
  Int_t iEvent;// nPart;
  Int_t nTrackHits;// nPrimary;
  Double_t fitFmin;

  Double_t bendingSlope, nonBendingSlope, inverseBendingMomentum;
  Double_t xRec, yRec, zRec, chi2MatchTrigger;
  Bool_t matchTrigger;

  // setting pointer for tracks, triggertracks & trackparam at vertex
  AliMUONTrack* recTrack = 0;
  AliMUONTrackParam* trackParam = 0;
  AliMUONTriggerTrack* recTriggerTrack = 0;
//   TParticle* particle = new TParticle();
//   AliGenEventHeader* header = 0;
  iEvent = runLoader->GetEventNumber(); 
  runLoader->GetEvent(iEvent);

  // Get vertex 
  Double_t vertex[3] = {0};
  const AliESDVertex *esdVert = esd->GetVertex(); 
  if (esdVert) esdVert->GetXYZ(vertex);
  
  //  nPrimary = 0;
//   if ( (header = runLoader->GetHeader()->GenEventHeader()) ) {
//     header->PrimaryVertex(vertex);
//   } else {
//     runLoader->LoadKinematics("READ");
//     runLoader->TreeK()->GetBranch("Particles")->SetAddress(&particle);
//     nPart = (Int_t)runLoader->TreeK()->GetEntries();
//     for(Int_t iPart = 0; iPart < nPart; iPart++) {
//       runLoader->TreeK()->GetEvent(iPart);
//       if (particle->GetFirstMother() == -1) {
// 	vertex[0] += particle->Vx();
// 	vertex[1] += particle->Vy();
// 	vertex[2] += particle->Vz();
// 	nPrimary++;
//       }
//       if (nPrimary) {
// 	vertex[0] /= (double)nPrimary;
// 	vertex[1] /= (double)nPrimary;
// 	vertex[2] /= (double)nPrimary;
//       }
//     }
//   }
  // setting ESD MUON class
  AliESDMuonTrack* theESDTrack = new  AliESDMuonTrack() ;

  //-------------------- trigger tracks-------------
  Long_t trigPat = 0;
  muonData->SetTreeAddress("RL");
  muonData->GetRecTriggerTracks();
  recTrigTracksArray = muonData->RecTriggerTracks();

  // ready global trigger pattern from first track
  if (recTrigTracksArray) 
    recTriggerTrack = (AliMUONTriggerTrack*) recTrigTracksArray->First();
  if (recTriggerTrack) trigPat = recTriggerTrack->GetGTPattern();

  //printf(">>> Event %d Number of Recconstructed tracks %d \n",iEvent, nrectracks);
 
  // -------------------- tracks-------------
  muonData->SetTreeAddress("RT");
  muonData->GetRecTracks();
  recTracksArray = muonData->RecTracks();
        
  Int_t nRecTracks = 0;
  if (recTracksArray)
    nRecTracks = (Int_t) recTracksArray->GetEntriesFast(); //
  
  // loop over tracks
  for (Int_t iRecTracks = 0; iRecTracks <  nRecTracks;  iRecTracks++) {

    // reading info from tracks
    recTrack = (AliMUONTrack*) recTracksArray->At(iRecTracks);

    trackParam = (AliMUONTrackParam*) (recTrack->GetTrackParamAtHit())->First();
    trackParam->ExtrapToVertex(vertex[0],vertex[1],vertex[2]);

    bendingSlope            = trackParam->GetBendingSlope();
    nonBendingSlope         = trackParam->GetNonBendingSlope();
    inverseBendingMomentum = trackParam->GetInverseBendingMomentum();
    xRec  = trackParam->GetNonBendingCoor();
    yRec  = trackParam->GetBendingCoor();
    zRec  = trackParam->GetZ();

    nTrackHits       = recTrack->GetNTrackHits();
    fitFmin          = recTrack->GetFitFMin();
    matchTrigger     = recTrack->GetMatchTrigger();
    chi2MatchTrigger = recTrack->GetChi2MatchTrigger();

    // setting data member of ESD MUON
    theESDTrack->SetInverseBendingMomentum(inverseBendingMomentum);
    theESDTrack->SetThetaX(TMath::ATan(nonBendingSlope));
    theESDTrack->SetThetaY(TMath::ATan(bendingSlope));
    theESDTrack->SetZ(zRec);
    theESDTrack->SetBendingCoor(yRec); // calculate vertex at ESD or Tracking level ?
    theESDTrack->SetNonBendingCoor(xRec);
    theESDTrack->SetChi2(fitFmin);
    theESDTrack->SetNHit(nTrackHits);
    theESDTrack->SetMatchTrigger(matchTrigger);
    theESDTrack->SetChi2MatchTrigger(chi2MatchTrigger);

    // storing ESD MUON Track into ESD Event 
    if (nRecTracks != 0)  
      esd->AddMuonTrack(theESDTrack);
  } // end loop tracks

  // add global trigger pattern
  if (nRecTracks != 0)  
    esd->SetTrigger(trigPat);

  // reset muondata
  muonData->ResetRecTracks();
  muonData->ResetRecTriggerTracks();

  //} // end loop on event  
  loader->UnloadTracks(); 
 //  if (!header)
//     runLoader->UnloadKinematics();
  delete theESDTrack;
  delete muonData;
  // delete particle;
}//_____________________________________________________________________________
void AliMUONReconstructor::FillESD(AliRunLoader* runLoader, AliRawReader* /*rawReader*/, AliESD* esd) const
{
/// Fill ESD
/// \todo add more

  // don't need rawReader ???
  FillESD(runLoader, esd);
}

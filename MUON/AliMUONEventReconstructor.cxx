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

////////////////////////////////////
//
// MUON event reconstructor in ALICE
//
// This class contains as data:
// * the parameters for the event reconstruction
// * a pointer to the array of hits to be reconstructed (the event)
// * a pointer to the array of segments made with these hits inside each station
// * a pointer to the array of reconstructed tracks
//
// It contains as methods, among others:
// * MakeEventToBeReconstructed to build the array of hits to be reconstructed
// * MakeSegments to build the segments
// * MakeTracks to build the tracks
//
////////////////////////////////////

#include <Riostream.h> // for cout
#include <stdlib.h> // for exit()

#include <TTree.h>

#include "AliMUON.h"
#include "AliMUONChamber.h"
#include "AliMUONEventReconstructor.h"
#include "AliMUONHitForRec.h"
#include "AliMUONTriggerTrack.h"
//#include "AliMUONTriggerConstants.h"
#include "AliMUONTriggerCircuit.h"
#include "AliMUONRawCluster.h"
#include "AliMUONLocalTrigger.h"
#include "AliMUONRecoEvent.h"
#include "AliMUONSegment.h"
#include "AliMUONTrack.h"
#include "AliMUONTrackHit.h"
#include "AliMagF.h"
#include "AliRun.h" // for gAlice
#include "AliConfig.h"
#include "AliRunLoader.h"
#include "AliLoader.h"
#include "AliMUONTrackK.h" //AZ
#include <TMatrixD.h> //AZ
#include "AliMC.h"

//************* Defaults parameters for reconstruction
static const Double_t kDefaultMinBendingMomentum = 3.0;
static const Double_t kDefaultMaxBendingMomentum = 500.0;
static const Double_t kDefaultMaxChi2 = 100.0;
static const Double_t kDefaultMaxSigma2Distance = 16.0;
static const Double_t kDefaultBendingResolution = 0.01;
static const Double_t kDefaultNonBendingResolution = 0.144;
static const Double_t kDefaultChamberThicknessInX0 = 0.03;
// Simple magnetic field:
// Value taken from macro MUONtracking.C: 0.7 T, hence 7 kG
// Length and Position from reco_muon.F, with opposite sign:
// Length = ZMAGEND-ZCOIL
// Position = (ZMAGEND+ZCOIL)/2
// to be ajusted differently from real magnetic field ????
static const Double_t kDefaultSimpleBValue = 7.0;
static const Double_t kDefaultSimpleBLength = 428.0;
static const Double_t kDefaultSimpleBPosition = 1019.0;
static const Int_t kDefaultRecGeantHits = 0;
static const Double_t kDefaultEfficiency = 0.95;

static const Int_t kDefaultPrintLevel = -1;

ClassImp(AliMUONEventReconstructor) // Class implementation in ROOT context

  //__________________________________________________________________________
AliMUONEventReconstructor::AliMUONEventReconstructor(void)
{
  // Constructor for class AliMUONEventReconstructor
  SetReconstructionParametersToDefaults();
  fTrackMethod = 1; //AZ - tracking method (1-default, 2-Kalman)
  // Memory allocation for the TClonesArray of hits for reconstruction
  // Is 10000 the right size ????
  fHitsForRecPtr = new TClonesArray("AliMUONHitForRec", 10000);
  fNHitsForRec = 0; // really needed or GetEntriesFast sufficient ????
  // Memory allocation for the TClonesArray's of segments in stations
  // Is 2000 the right size ????
  for (Int_t st = 0; st < kMaxMuonTrackingStations; st++) {
    fSegmentsPtr[st] = new TClonesArray("AliMUONSegment", 2000);
    fNSegments[st] = 0; // really needed or GetEntriesFast sufficient ????
  }
  // Memory allocation for the TClonesArray of reconstructed tracks
  // Is 10 the right size ????
  fRecTracksPtr = new TClonesArray("AliMUONTrack", 10);
  fNRecTracks = 0; // really needed or GetEntriesFast sufficient ????
// trigger tracks
  fRecTriggerTracksPtr = new TClonesArray("AliMUONTriggerTrack", 10);
  fNRecTriggerTracks = 0; // really needed or GetEntriesFast sufficient ????
  // Memory allocation for the TClonesArray of hits on reconstructed tracks
  // Is 100 the right size ????
  fRecTrackHitsPtr = new TClonesArray("AliMUONTrack", 100);
  fNRecTrackHits = 0; // really needed or GetEntriesFast sufficient ????

  // Sign of fSimpleBValue according to sign of Bx value at (50,50,-950).
  Float_t b[3], x[3];
  x[0] = 50.; x[1] = 50.; x[2] = -950.;
  gAlice->Field()->Field(x, b);
  fSimpleBValue = TMath::Sign(fSimpleBValue,(Double_t) b[0]);
  fSimpleBPosition = TMath::Sign(fSimpleBPosition,(Double_t) x[2]);
  // See how to get fSimple(BValue, BLength, BPosition)
  // automatically calculated from the actual magnetic field ????

  if (fPrintLevel >= 0) {
    cout << "AliMUONEventReconstructor constructed with defaults" << endl; Dump();
    cout << endl << "Magnetic field from root file:" << endl;
    gAlice->Field()->Dump();
    cout << endl;
  }
  
  // Initializions for GEANT background events
  fBkgGeantFile = 0;
  fBkgGeantTK = 0;
  fBkgGeantParticles = 0;
  fBkgGeantTH = 0;
  fBkgGeantHits = 0;
  fBkgGeantEventNumber = -1;
  
  // Initialize to 0 pointers to RecoEvent, tree and tree file
  fRecoEvent = 0;
  fEventTree = 0;
  fTreeFile  = 0;
  
  return;
}

AliMUONEventReconstructor::AliMUONEventReconstructor (const AliMUONEventReconstructor& Reconstructor):TObject(Reconstructor)
{
  // Dummy copy constructor
}

AliMUONEventReconstructor & AliMUONEventReconstructor::operator=(const AliMUONEventReconstructor& /*Reconstructor*/)
{
  // Dummy assignment operator
    return *this;
}

  //__________________________________________________________________________
AliMUONEventReconstructor::~AliMUONEventReconstructor(void)
{
  // Destructor for class AliMUONEventReconstructor
  if (fTreeFile) {
     fTreeFile->Close();
     delete fTreeFile;
  }
//  if (fEventTree) delete fEventTree;
  if (fRecoEvent) delete fRecoEvent;
  delete fHitsForRecPtr; // Correct destruction of everything ???? or delete [] ????
  for (Int_t st = 0; st < kMaxMuonTrackingStations; st++)
    delete fSegmentsPtr[st]; // Correct destruction of everything ????
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::SetReconstructionParametersToDefaults(void)
{
  // Set reconstruction parameters to default values
  // Would be much more convenient with a structure (or class) ????
  fMinBendingMomentum = kDefaultMinBendingMomentum;
  fMaxBendingMomentum = kDefaultMaxBendingMomentum;
  fMaxChi2 = kDefaultMaxChi2;
  fMaxSigma2Distance = kDefaultMaxSigma2Distance;

  AliMUON *pMUON = (AliMUON*) gAlice->GetModule("MUON");
  // ******** Parameters for making HitsForRec
  // minimum radius,
  // like in TRACKF_STAT:
  // 2 degrees for stations 1 and 2, or ch(0...) from 0 to 3;
  // 30 cm for stations 3 to 5, or ch(0...) from 4 to 9
  for (Int_t ch = 0; ch < kMaxMuonTrackingChambers; ch++) {
    if (ch < 4) fRMin[ch] = TMath::Abs((&(pMUON->Chamber(ch)))->Z()) *
		  2.0 * TMath::Pi() / 180.0;
    else fRMin[ch] = 30.0;
    // maximum radius at 10 degrees and Z of chamber
    fRMax[ch] = TMath::Abs((&(pMUON->Chamber(ch)))->Z()) *
		  10.0 * TMath::Pi() / 180.0;
  }

  // ******** Parameters for making segments
  // should be parametrized ????
  // according to interval between chambers in a station ????
  // Maximum distance in non bending plane
  // 5 * 0.22 just to remember the way it was made in TRACKF_STAT
  // SIGCUT*DYMAX(IZ)
  for (Int_t st = 0; st < kMaxMuonTrackingStations; st++)
    fSegmentMaxDistNonBending[st] = 5. * 0.22;
  // Maximum distance in bending plane:
  // values from TRACKF_STAT, corresponding to (J psi 20cm),
  // scaled to the real distance between chambers in a station
  fSegmentMaxDistBending[0] = TMath::Abs( 1.5 *
    ((&(pMUON->Chamber(1)))->Z() - (&(pMUON->Chamber(0)))->Z()) / 20.0);
  fSegmentMaxDistBending[1] =  TMath::Abs( 1.5 *
    ((&(pMUON->Chamber(3)))->Z() - (&(pMUON->Chamber(2)))->Z()) / 20.0);
  fSegmentMaxDistBending[2] =  TMath::Abs( 3.0 *
    ((&(pMUON->Chamber(5)))->Z() - (&(pMUON->Chamber(4)))->Z()) / 20.0);
  fSegmentMaxDistBending[3] =  TMath::Abs( 6.0 *
    ((&(pMUON->Chamber(7)))->Z() - (&(pMUON->Chamber(6)))->Z()) / 20.0);
  fSegmentMaxDistBending[4] =  TMath::Abs( 6.0 *
    ((&(pMUON->Chamber(9)))->Z() - (&(pMUON->Chamber(8)))->Z()) / 20.0);
  
  fBendingResolution = kDefaultBendingResolution;
  fNonBendingResolution = kDefaultNonBendingResolution;
  fChamberThicknessInX0 = kDefaultChamberThicknessInX0;
  fSimpleBValue = kDefaultSimpleBValue;
  fSimpleBLength = kDefaultSimpleBLength;
  fSimpleBPosition = kDefaultSimpleBPosition;
  fRecGeantHits = kDefaultRecGeantHits;
  fEfficiency = kDefaultEfficiency;
  fPrintLevel = kDefaultPrintLevel;
  return;
}

//__________________________________________________________________________
Double_t AliMUONEventReconstructor::GetImpactParamFromBendingMomentum(Double_t BendingMomentum) const
{
  // Returns impact parameter at vertex in bending plane (cm),
  // from the signed bending momentum "BendingMomentum" in bending plane (GeV/c),
  // using simple values for dipole magnetic field.
  // The sign of "BendingMomentum" is the sign of the charge.
  return (-0.0003 * fSimpleBValue * fSimpleBLength * fSimpleBPosition /
	  BendingMomentum);
}

//__________________________________________________________________________
Double_t AliMUONEventReconstructor::GetBendingMomentumFromImpactParam(Double_t ImpactParam) const
{
  // Returns signed bending momentum in bending plane (GeV/c),
  // the sign being the sign of the charge for particles moving forward in Z,
  // from the impact parameter "ImpactParam" at vertex in bending plane (cm),
  // using simple values for dipole magnetic field.
  return (-0.0003 * fSimpleBValue * fSimpleBLength * fSimpleBPosition /
	  ImpactParam);
}

//__________________________________________________________________________
void AliMUONEventReconstructor::SetBkgGeantFile(Text_t *BkgGeantFileName)
{
  // Set background file ... for GEANT hits
  // Must be called after having loaded the firts signal event
  if (fPrintLevel >= 0) {
    cout << "Enter SetBkgGeantFile with BkgGeantFileName ``"
	 << BkgGeantFileName << "''" << endl;}
  if (strlen(BkgGeantFileName)) {
    // BkgGeantFileName not empty: try to open the file
    if (fPrintLevel >= 2) {cout << "Before File(Bkg)" << endl; gDirectory->Dump();}
    fBkgGeantFile = new TFile(BkgGeantFileName);
    if (fPrintLevel >= 2) {cout << "After File(Bkg)" << endl; gDirectory->Dump();}
    if (fBkgGeantFile-> IsOpen()) {
      if (fPrintLevel >= 0) {
	cout << "Background for GEANT hits in file: ``" << BkgGeantFileName
	     << "'' successfully opened" << endl;}
    }
    else {
      cout << "Background for GEANT hits in file: " << BkgGeantFileName << endl;
      cout << "NOT FOUND: EXIT" << endl;
      exit(0); // right instruction for exit ????
    }
    // Arrays for "particles" and "hits"
    fBkgGeantParticles = new TClonesArray("TParticle", 200);
    fBkgGeantHits = new TClonesArray("AliMUONHit", 2000);
    // Event number to -1 for initialization
    fBkgGeantEventNumber = -1;
    // Back to the signal file:
    // first signal event must have been loaded previously,
    // otherwise, Segmentation violation at the next instruction
    // How is it possible to do smething better ????
    ((gAlice->TreeK())->GetCurrentFile())->cd();
    if (fPrintLevel >= 2) {cout << "After cd(gAlice)" << endl; gDirectory->Dump();}
  }
  return;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::NextBkgGeantEvent(void)
{
  // Get next event in background file for GEANT hits
  // Goes back to event number 0 when end of file is reached
  char treeName[20];
  TBranch *branch;
  if (fPrintLevel >= 0) {
    cout << "Enter NextBkgGeantEvent" << endl;}
  // Clean previous event
  if(fBkgGeantTK) delete fBkgGeantTK;
  fBkgGeantTK = NULL;
  if(fBkgGeantParticles) fBkgGeantParticles->Clear();
  if(fBkgGeantTH) delete fBkgGeantTH;
  fBkgGeantTH = NULL;
  if(fBkgGeantHits) fBkgGeantHits->Clear();
  // Increment event number
  fBkgGeantEventNumber++;
  // Get access to Particles and Hits for event from background file
  if (fPrintLevel >= 2) {cout << "Before cd(Bkg)" << endl; gDirectory->Dump();}
  fBkgGeantFile->cd();
  if (fPrintLevel >= 2) {cout << "After cd(Bkg)" << endl; gDirectory->Dump();}
  // Particles: TreeK for event and branch "Particles"
  sprintf(treeName, "TreeK%d", fBkgGeantEventNumber);
  fBkgGeantTK = (TTree*)gDirectory->Get(treeName);
  if (!fBkgGeantTK) {
    if (fPrintLevel >= 0) {
      cout << "Cannot find Kine Tree for background event: " <<
	fBkgGeantEventNumber << endl;
      cout << "Goes back to event 0" << endl;
    }
    fBkgGeantEventNumber = 0;
    sprintf(treeName, "TreeK%d", fBkgGeantEventNumber);
    fBkgGeantTK = (TTree*)gDirectory->Get(treeName);
    if (!fBkgGeantTK) {
      cout << "ERROR: cannot find Kine Tree for background event: " <<
	fBkgGeantEventNumber << endl;
      exit(0);
    }
  }
  if (fBkgGeantTK) 
    fBkgGeantTK->SetBranchAddress("Particles", &fBkgGeantParticles);
  fBkgGeantTK->GetEvent(0); // why event 0 ???? necessary ????
  // Hits: TreeH for event and branch "MUON"
  sprintf(treeName, "TreeH%d", fBkgGeantEventNumber);
  fBkgGeantTH = (TTree*)gDirectory->Get(treeName);
  if (!fBkgGeantTH) {
    cout << "ERROR: cannot find Hits Tree for background event: " <<
      fBkgGeantEventNumber << endl;
      exit(0);
  }
  if (fBkgGeantTH && fBkgGeantHits) {
    branch = fBkgGeantTH->GetBranch("MUON");
    if (branch) branch->SetAddress(&fBkgGeantHits);
  }
  fBkgGeantTH->GetEntries(); // necessary ????
  // Back to the signal file
  ((gAlice->TreeK())->GetCurrentFile())->cd();
  if (fPrintLevel >= 2) {cout << "After cd(gAlice)" << endl; gDirectory->Dump();}
  return;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::EventReconstruct(void)
{
  // To reconstruct one event
  if (fPrintLevel >= 1) cout << "enter EventReconstruct" << endl;
  MakeEventToBeReconstructed();
  MakeSegments();
  MakeTracks();
  return;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::EventReconstructTrigger(void)
{
  // To reconstruct one event
  if (fPrintLevel >= 1) cout << "enter EventReconstructTrigger" << endl;
  MakeTriggerTracks();  
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::ResetHitsForRec(void)
{
  // To reset the array and the number of HitsForRec,
  // and also the number of HitsForRec
  // and the index of the first HitForRec per chamber
  if (fHitsForRecPtr) fHitsForRecPtr->Clear();
  fNHitsForRec = 0;
  for (Int_t ch = 0; ch < kMaxMuonTrackingChambers; ch++)
    fNHitsForRecPerChamber[ch] = fIndexOfFirstHitForRecPerChamber[ch] = 0;
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::ResetSegments(void)
{
  // To reset the TClonesArray of segments and the number of Segments
  // for all stations
  for (Int_t st = 0; st < kMaxMuonTrackingStations; st++) {
    if (fSegmentsPtr[st]) fSegmentsPtr[st]->Clear();
    fNSegments[st] = 0;
  }
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::ResetTracks(void)
{
  // To reset the TClonesArray of reconstructed tracks
  if (fRecTracksPtr) fRecTracksPtr->Delete();
  // Delete in order that the Track destructors are called,
  // hence the space for the TClonesArray of pointers to TrackHit's is freed
  fNRecTracks = 0;
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::ResetTriggerTracks(void)
{
  // To reset the TClonesArray of reconstructed trigger tracks
    if (fRecTriggerTracksPtr) fRecTriggerTracksPtr->Delete();
  // Delete in order that the Track destructors are called,
  // hence the space for the TClonesArray of pointers to TrackHit's is freed
    fNRecTriggerTracks = 0;
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::ResetTrackHits(void)
{
  // To reset the TClonesArray of hits on reconstructed tracks
  if (fRecTrackHitsPtr) fRecTrackHitsPtr->Clear();
  fNRecTrackHits = 0;
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeEventToBeReconstructed(void)
{
  // To make the list of hits to be reconstructed,
  // either from the GEANT hits or from the raw clusters
  // according to the parameter set for the reconstructor
  TString evfoldname = AliConfig::fgkDefaultEventFolderName;//to be interfaced properly
  
  AliRunLoader* rl = AliRunLoader::GetRunLoader(evfoldname);
  if (rl == 0x0)
   {
     Error("MakeEventToBeReconstructed",
           "Can not find Run Loader in Event Folder named %s.",
           evfoldname.Data());
     return;
   }
  AliLoader* gime = rl->GetLoader("MUONLoader");
  if (gime == 0x0)
   {
     Error("MakeEventToBeReconstructed","Can not get MUON Loader from Run Loader.");
     return;
   }
  
  if (fPrintLevel >= 1) cout << "enter MakeEventToBeReconstructed" << endl;
  ResetHitsForRec();
  if (fRecGeantHits == 1) {
    // Reconstruction from GEANT hits
    // Back to the signal file
      TTree* treeH = gime->TreeH();
      if (treeH == 0x0)
       {
         Int_t retval = gime->LoadHits();
         if ( retval)
          {
            Error("MakeEventToBeReconstructed","Error occured while loading hits.");
            return;
          }
         treeH = gime->TreeH();
         if (treeH == 0x0)
          {
           Error("MakeEventToBeReconstructed","Can not get TreeH");
           return;
          }
       }
    
    AddHitsForRecFromGEANT(treeH);
    
    // Background hits
    AddHitsForRecFromBkgGEANT(fBkgGeantTH, fBkgGeantHits);
    // Sort HitsForRec in increasing order with respect to chamber number
    SortHitsForRecWithIncreasingChamber();
  }
  else {
    // Reconstruction from raw clusters
    // AliMUON *MUON  = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
    // Security on MUON ????
    // TreeR assumed to be be "prepared" in calling function
    // by "MUON->GetTreeR(nev)" ????
    TTree *treeR = gime->TreeR();
    AddHitsForRecFromRawClusters(treeR);
    // No sorting: it is done automatically in the previous function
  }
  if (fPrintLevel >= 10) {
    cout << "end of MakeEventToBeReconstructed" << endl;
    cout << "NHitsForRec: " << fNHitsForRec << endl;
    for (Int_t ch = 0; ch < kMaxMuonTrackingChambers; ch++) {
      cout << "chamber(0...): " << ch
	   << "  NHitsForRec: " << fNHitsForRecPerChamber[ch]
	   << "  index(first HitForRec): " << fIndexOfFirstHitForRecPerChamber[ch]
	   << endl;
      for (Int_t hit = fIndexOfFirstHitForRecPerChamber[ch];
	   hit < fIndexOfFirstHitForRecPerChamber[ch] + fNHitsForRecPerChamber[ch];
	   hit++) {
	cout << "HitForRec index(0...): " << hit << endl;
	((*fHitsForRecPtr)[hit])->Dump();
      }
    }
  }
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::AddHitsForRecFromGEANT(TTree *TH)
{
  // To add to the list of hits for reconstruction
  // the GEANT signal hits from a hit tree TH.
  Int_t hitBits, chamBits; //AZ
  if (fPrintLevel >= 2)
    cout << "enter AddHitsForRecFromGEANT with TH: " << TH << endl;
  if (TH == NULL) return;
  AliMUON *pMUON  = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
  AliMUONData * muondata = pMUON->GetMUONData();
  // Security on MUON ????
  // See whether it could be the same for signal and background ????
  // Loop over tracks in tree
  Int_t ntracks = (Int_t) TH->GetEntries();
  if (fPrintLevel >= 2)
    cout << "ntracks: " << ntracks << endl;
  fMuons = 0; //AZ
  for (Int_t track = 0; track < ntracks; track++) {
    muondata->ResetHits();
    TH->GetEvent(track);
    // Loop over hits
    Int_t hit = 0;
    hitBits = 0; // AZ
    chamBits = 0; // AZ
    Int_t itrack = track; //AZ

    Int_t ihit, nhits=0;
      nhits = (Int_t) muondata->Hits()->GetEntriesFast();
      AliMUONHit* mHit=0x0;

      for(ihit=0; ihit<nhits; ihit++) {
	mHit = static_cast<AliMUONHit*>(muondata->Hits()->At(ihit));
	Int_t ipart = TMath::Abs ((Int_t) mHit->Particle()); //AZ
	if (NewHitForRecFromGEANT(mHit,track, hit, 1) && ipart == 13
	    //if (NewHitForRecFromGEANT(mHit,itrack-1, hit, 1) && ipart == 13 
	    && itrack <= 2 && !BIT(mHit->Chamber()-1)  ) chamBits |= BIT(mHit->Chamber()-1); //AZ - set bit
      }

    if (chamBits&3 && chamBits>>2&3 && chamBits>>4&3 && chamBits>>6&3 && 
        chamBits>>8&3 && ((chamBits>>6&3)==3 || (chamBits>>8&3)==3)) 
      fMuons += 1; //AZ
    //if (chamBits&3 && chamBits>>2&3 && chamBits>>4&3 && chamBits>>6&3 && 
    //      chamBits>>8&3 && ((chamBits>>6&3)==3 || (chamBits>>8&3)==3) && 
    //      ((chamBits&3)==3 || (chamBits>>2&3)==3)) fMuons += 1;
  } // end of track loop
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::AddHitsForRecFromBkgGEANT(TTree *TH, TClonesArray *Hits)
{
  // To add to the list of hits for reconstruction
  // the GEANT background hits from a hit tree TH and a pointer Hits to a hit list.
  // How to have only one function "AddHitsForRecFromGEANT" ????
  if (fPrintLevel >= 2)
    cout << "enter AddHitsForRecFromBkgGEANT with TH: " << TH << endl;
  if (TH == NULL) return;
  // Loop over tracks in tree
  Int_t ntracks = (Int_t) TH->GetEntries();
  if (fPrintLevel >= 2)
    cout << "ntracks: " << ntracks << endl;
  for (Int_t track = 0; track < ntracks; track++) {
    if (Hits) Hits->Clear();
    TH->GetEvent(track);
    // Loop over hits
    for (Int_t hit = 0; hit < Hits->GetEntriesFast(); hit++) {
      NewHitForRecFromGEANT((AliMUONHit*) (*Hits)[hit], track, hit, 0);
    } // end of hit loop
  } // end of track loop
  return;
}

  //__________________________________________________________________________
AliMUONHitForRec* AliMUONEventReconstructor::NewHitForRecFromGEANT(AliMUONHit* Hit, Int_t TrackNumber, Int_t HitNumber, Int_t Signal)
{
  // To make a new hit for reconstruction from a GEANT hit pointed to by "Hit",
  // with hit number "HitNumber" in the track numbered "TrackNumber",
  // either from signal ("Signal" = 1) or background ("Signal" = 0) event.
  // Selects hits in tracking (not trigger) chambers.
  // Takes into account the efficiency (fEfficiency)
  // and the smearing from resolution (fBendingResolution and fNonBendingResolution).
  // Adds a condition on the radius between RMin and RMax
  // to better simulate the real chambers.
  // Returns the pointer to the new hit for reconstruction,
  // or NULL in case of inefficiency or non tracking chamber or bad radius.
  // No condition on at most 20 cm from a muon from a resonance
  // like in Fortran TRACKF_STAT.
  AliMUONHitForRec* hitForRec;
  Double_t bendCoor, nonBendCoor, radius;
  Int_t chamber = Hit->Chamber() - 1; // chamber(0...)
  // only in tracking chambers (fChamber starts at 1)
  if (chamber >= kMaxMuonTrackingChambers) return NULL;
  // only if hit is efficient (keep track for checking ????)
  if (gRandom->Rndm() > fEfficiency) return NULL;
  // only if radius between RMin and RMax
  bendCoor = Hit->Y();
  nonBendCoor = Hit->X();
  radius = TMath::Sqrt((bendCoor * bendCoor) + (nonBendCoor * nonBendCoor));
  // This cut is not needed with a realistic chamber geometry !!!!
//   if ((radius < fRMin[chamber]) || (radius > fRMax[chamber])) return NULL;
  // new AliMUONHitForRec from GEANT hit and increment number of AliMUONHitForRec's
  hitForRec = new ((*fHitsForRecPtr)[fNHitsForRec]) AliMUONHitForRec(Hit);
  fNHitsForRec++;
  // add smearing from resolution
  hitForRec->SetBendingCoor(bendCoor + gRandom->Gaus(0., fBendingResolution));
  hitForRec->SetNonBendingCoor(nonBendCoor
			       + gRandom->Gaus(0., fNonBendingResolution));
//   // !!!! without smearing
//   hitForRec->SetBendingCoor(bendCoor);
//   hitForRec->SetNonBendingCoor(nonBendCoor);
  // more information into HitForRec
  //  resolution: angular effect to be added here ????
  hitForRec->SetBendingReso2(fBendingResolution * fBendingResolution);
  hitForRec->SetNonBendingReso2(fNonBendingResolution * fNonBendingResolution);
  //  GEANT track info
  hitForRec->SetHitNumber(HitNumber);
  hitForRec->SetTHTrack(TrackNumber);
  hitForRec->SetGeantSignal(Signal);
  if (fPrintLevel >= 10) {
    cout << "track: " << TrackNumber << " hit: " << HitNumber << endl;
    Hit->Dump();
    cout << "AliMUONHitForRec number (1...): " << fNHitsForRec << endl;
    hitForRec->Dump();}
  return hitForRec;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::SortHitsForRecWithIncreasingChamber()
{
  // Sort HitsForRec's in increasing order with respect to chamber number.
  // Uses the function "Compare".
  // Update the information for HitsForRec per chamber too.
  Int_t ch, nhits, prevch;
  fHitsForRecPtr->Sort();
  for (ch = 0; ch < kMaxMuonTrackingChambers; ch++) {
    fNHitsForRecPerChamber[ch] = 0;
    fIndexOfFirstHitForRecPerChamber[ch] = 0;
  }
  prevch = 0; // previous chamber
  nhits = 0; // number of hits in current chamber
  // Loop over HitsForRec
  for (Int_t hit = 0; hit < fNHitsForRec; hit++) {
    // chamber number (0...)
    ch = ((AliMUONHitForRec*)  ((*fHitsForRecPtr)[hit]))->GetChamberNumber();
    // increment number of hits in current chamber
    (fNHitsForRecPerChamber[ch])++;
    // update index of first HitForRec in current chamber
    // if chamber number different from previous one
    if (ch != prevch) {
      fIndexOfFirstHitForRecPerChamber[ch] = hit;
      prevch = ch;
    }
  }
  return;
}

//   //__________________________________________________________________________
// void AliMUONEventReconstructor::AddHitsForRecFromCathodeCorrelations(TTree* TC)
// {
//   // OLD VERSION WHEN ONE ONE WAS USING SO CALLED CATHODE CORRELATIONS
//   // To add to the list of hits for reconstruction
//   // the (cathode correlated) raw clusters
//   // No condition added, like in Fortran TRACKF_STAT,
//   // on the radius between RMin and RMax.
//   AliMUONHitForRec *hitForRec;
//   if (fPrintLevel >= 1) cout << "enter AddHitsForRecFromCathodeCorrelations" << endl;
//   AliMUON *MUON  = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
//   // Security on MUON ????
//   // Loop over tracking chambers
//   for (Int_t ch = 0; ch < kMaxMuonTrackingChambers; ch++) {
//     // number of HitsForRec to 0 for the chamber
//     fNHitsForRecPerChamber[ch] = 0;
//     // index of first HitForRec for the chamber
//     if (ch == 0) fIndexOfFirstHitForRecPerChamber[ch] = 0;
//     else fIndexOfFirstHitForRecPerChamber[ch] = fNHitsForRec;
//     TClonesArray *reconst_hits  = MUON->ReconstHitsAddress(ch);
//     MUON->ResetReconstHits();
//     TC->GetEvent();
//     Int_t ncor = (Int_t)reconst_hits->GetEntries();
//     // Loop over (cathode correlated) raw clusters
//     for (Int_t cor = 0; cor < ncor; cor++) {
//       AliMUONReconstHit * mCor = 
// 	(AliMUONReconstHit*) reconst_hits->UncheckedAt(cor);
//       // new AliMUONHitForRec from (cathode correlated) raw cluster
//       // and increment number of AliMUONHitForRec's (total and in chamber)
//       hitForRec = new ((*fHitsForRecPtr)[fNHitsForRec]) AliMUONHitForRec(mCor);
//       fNHitsForRec++;
//       (fNHitsForRecPerChamber[ch])++;
//       // more information into HitForRec
//       hitForRec->SetChamberNumber(ch);
//       hitForRec->SetHitNumber(cor);
//       // Z coordinate of the chamber (cm) with sign opposite to GEANT sign
//       // could (should) be more exact from chamber geometry ???? 
//       hitForRec->SetZ(-(&(MUON->Chamber(ch)))->Z());
//       if (fPrintLevel >= 10) {
// 	cout << "chamber (0...): " << ch <<
// 	  " cathcorrel (0...): " << cor << endl;
// 	mCor->Dump();
// 	cout << "AliMUONHitForRec number (1...): " << fNHitsForRec << endl;
// 	hitForRec->Dump();}
//     } // end of cluster loop
//   } // end of chamber loop
//   return;
// }

  //__________________________________________________________________________
void AliMUONEventReconstructor::AddHitsForRecFromRawClusters(TTree* TR)
{
  // To add to the list of hits for reconstruction all the raw clusters
  // No condition added, like in Fortran TRACKF_STAT,
  // on the radius between RMin and RMax.
  AliMUONHitForRec *hitForRec;
  AliMUONRawCluster *clus;
  Int_t iclus, nclus, nTRentries;
  TClonesArray *rawclusters;
  if (fPrintLevel >= 1) cout << "enter AddHitsForRecFromRawClusters" << endl;

  TString evfoldname = AliConfig::fgkDefaultEventFolderName;//to be interfaced properly
  AliRunLoader* rl = AliRunLoader::GetRunLoader(evfoldname);
  if (rl == 0x0)
   {
     Error("MakeEventToBeReconstructed",
           "Can not find Run Loader in Event Folder named %s.",
           evfoldname.Data());
     return;
   }
  AliLoader* gime = rl->GetLoader("MUONLoader");
  if (gime == 0x0)
   {
     Error("MakeEventToBeReconstructed","Can not get MUON Loader from Run Loader.");
     return;
   }
   // Loading AliRun master
  rl->LoadgAlice();
  gAlice = rl->GetAliRun();

  // Loading MUON subsystem
  AliMUON * pMUON = (AliMUON *) gAlice->GetDetector("MUON");

  nTRentries = Int_t(TR->GetEntries());
  if (nTRentries != 1) {
    cout << "Error in AliMUONEventReconstructor::AddHitsForRecFromRawClusters"
	 << endl;
    cout << "nTRentries = " << nTRentries << " not equal to 1" << endl;
    exit(0);
  }
  gime->TreeR()->GetEvent(0); // only one entry  
  // Loop over tracking chambers
  for (Int_t ch = 0; ch < kMaxMuonTrackingChambers; ch++) {
    // number of HitsForRec to 0 for the chamber
    fNHitsForRecPerChamber[ch] = 0;
    // index of first HitForRec for the chamber
    if (ch == 0) fIndexOfFirstHitForRecPerChamber[ch] = 0;
    else fIndexOfFirstHitForRecPerChamber[ch] = fNHitsForRec;
    rawclusters = pMUON->GetMUONData()->RawClusters(ch);
//     pMUON->ResetRawClusters();
//     TR->GetEvent((Int_t) (TR->GetEntries()) - 1); // to be checked ????
    nclus = (Int_t) (rawclusters->GetEntries());
    // Loop over (cathode correlated) raw clusters
    for (iclus = 0; iclus < nclus; iclus++) {
      clus = (AliMUONRawCluster*) rawclusters->UncheckedAt(iclus);
      // new AliMUONHitForRec from raw cluster
      // and increment number of AliMUONHitForRec's (total and in chamber)
      hitForRec = new ((*fHitsForRecPtr)[fNHitsForRec]) AliMUONHitForRec(clus);
      fNHitsForRec++;
      (fNHitsForRecPerChamber[ch])++;
      // more information into HitForRec
      //  resolution: info should be already in raw cluster and taken from it ????
      hitForRec->SetBendingReso2(fBendingResolution * fBendingResolution);
      hitForRec->SetNonBendingReso2(fNonBendingResolution * fNonBendingResolution);
      //  original raw cluster
      hitForRec->SetChamberNumber(ch);
      hitForRec->SetHitNumber(iclus);
      // Z coordinate of the raw cluster (cm)
      hitForRec->SetZ(clus->fZ[0]);
      if (fPrintLevel >= 10) {
	cout << "chamber (0...): " << ch <<
	  " raw cluster (0...): " << iclus << endl;
	clus->Dump();
	cout << "AliMUONHitForRec number (1...): " << fNHitsForRec << endl;
	hitForRec->Dump();}
    } // end of cluster loop
  } // end of chamber loop
  SortHitsForRecWithIncreasingChamber(); //AZ 
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeSegments(void)
{
  // To make the list of segments in all stations,
  // from the list of hits to be reconstructed
  if (fPrintLevel >= 1) cout << "enter MakeSegments" << endl;
  ResetSegments();
  // Loop over stations
  Int_t nb = (fTrackMethod == 2) ? 3 : 0; //AZ
  //AZ for (Int_t st = 0; st < kMaxMuonTrackingStations; st++)
  for (Int_t st = nb; st < kMaxMuonTrackingStations; st++) //AZ
    MakeSegmentsPerStation(st); 
  if (fPrintLevel >= 10) {
    cout << "end of MakeSegments" << endl;
    for (Int_t st = 0; st < kMaxMuonTrackingStations; st++) {
      cout << "station(0...): " << st
	   << "  Segments: " << fNSegments[st]
	   << endl;
      for (Int_t seg = 0;
	   seg < fNSegments[st];
	   seg++) {
	cout << "Segment index(0...): " << seg << endl;
	((*fSegmentsPtr[st])[seg])->Dump();
      }
    }
  }
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeSegmentsPerStation(Int_t Station)
{
  // To make the list of segments in station number "Station" (0...)
  // from the list of hits to be reconstructed.
  // Updates "fNSegments"[Station].
  // Segments in stations 4 and 5 are sorted
  // according to increasing absolute value of "impact parameter"
  AliMUONHitForRec *hit1Ptr, *hit2Ptr;
  AliMUONSegment *segment;
  Bool_t last2st;
  Double_t bendingSlope, distBend, distNonBend, extBendCoor, extNonBendCoor,
      impactParam = 0., maxImpactParam = 0., minImpactParam = 0.; // =0 to avoid compilation warnings.
  AliMUON *pMUON  = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
  if (fPrintLevel >= 1)
    cout << "enter MakeSegmentsPerStation (0...) " << Station << endl;
  // first and second chambers (0...) in the station
  Int_t ch1 = 2 * Station;
  Int_t ch2 = ch1 + 1;
  // variable true for stations downstream of the dipole:
  // Station(0..4) equal to 3 or 4
  if ((Station == 3) || (Station == 4)) {
    last2st = kTRUE;
    // maximum impact parameter (cm) according to fMinBendingMomentum...
    maxImpactParam =
      TMath::Abs(GetImpactParamFromBendingMomentum(fMinBendingMomentum));
    // minimum impact parameter (cm) according to fMaxBendingMomentum...
    minImpactParam =
      TMath::Abs(GetImpactParamFromBendingMomentum(fMaxBendingMomentum));
  }
  else last2st = kFALSE;
  // extrapolation factor from Z of first chamber to Z of second chamber
  // dZ to be changed to take into account fine structure of chambers ????
  Double_t extrapFact =
    (&(pMUON->Chamber(ch2)))->Z() / (&(pMUON->Chamber(ch1)))->Z();
  // index for current segment
  Int_t segmentIndex = 0;
  // Loop over HitsForRec in the first chamber of the station
  for (Int_t hit1 = fIndexOfFirstHitForRecPerChamber[ch1];
       hit1 < fIndexOfFirstHitForRecPerChamber[ch1] + fNHitsForRecPerChamber[ch1];
       hit1++) {
    // pointer to the HitForRec
    hit1Ptr = (AliMUONHitForRec*) ((*fHitsForRecPtr)[hit1]);
    // extrapolation,
    // on the straight line joining the HitForRec to the vertex (0,0,0),
    // to the Z of the second chamber of the station
    extBendCoor = extrapFact * hit1Ptr->GetBendingCoor();
    extNonBendCoor = extrapFact * hit1Ptr->GetNonBendingCoor();
    // Loop over HitsForRec in the second chamber of the station
    for (Int_t hit2 = fIndexOfFirstHitForRecPerChamber[ch2];
	 hit2 < fIndexOfFirstHitForRecPerChamber[ch2] + fNHitsForRecPerChamber[ch2];
	 hit2++) {
      // pointer to the HitForRec
      hit2Ptr = (AliMUONHitForRec*) ((*fHitsForRecPtr)[hit2]);
      // absolute values of distances, in bending and non bending planes,
      // between the HitForRec in the second chamber
      // and the previous extrapolation
      distBend = TMath::Abs(hit2Ptr->GetBendingCoor() - extBendCoor);
      distNonBend = TMath::Abs(hit2Ptr->GetNonBendingCoor() - extNonBendCoor);
      if (last2st) {
	// bending slope
	bendingSlope = (hit1Ptr->GetBendingCoor() - hit2Ptr->GetBendingCoor()) /
	  (hit1Ptr->GetZ() - hit2Ptr->GetZ());
	// absolute value of impact parameter
	impactParam =
	  TMath::Abs(hit1Ptr->GetBendingCoor() - hit1Ptr->GetZ() * bendingSlope);
      }
      // check for distances not too large,
      // and impact parameter not too big if stations downstream of the dipole.
      // Conditions "distBend" and "impactParam" correlated for these stations ????
      if ((distBend < fSegmentMaxDistBending[Station]) &&
	  (distNonBend < fSegmentMaxDistNonBending[Station]) &&
	  (!last2st || (impactParam < maxImpactParam)) &&
	  (!last2st || (impactParam > minImpactParam))) {
	// make new segment
	segment = new ((*fSegmentsPtr[Station])[segmentIndex])
	  AliMUONSegment(hit1Ptr, hit2Ptr);
	// update "link" to this segment from the hit in the first chamber
	if (hit1Ptr->GetNSegments() == 0)
	  hit1Ptr->SetIndexOfFirstSegment(segmentIndex);
	hit1Ptr->SetNSegments(hit1Ptr->GetNSegments() + 1);
	if (fPrintLevel >= 10) {
	  cout << "segmentIndex(0...): " << segmentIndex
	       << "  distBend: " << distBend
	       << "  distNonBend: " << distNonBend
	       << endl;
	  segment->Dump();
	  cout << "HitForRec in first chamber" << endl;
	  hit1Ptr->Dump();
	  cout << "HitForRec in second chamber" << endl;
	  hit2Ptr->Dump();
	};
	// increment index for current segment
	segmentIndex++;
      }
    } //for (Int_t hit2
  } // for (Int_t hit1...
  fNSegments[Station] = segmentIndex;
  // Sorting according to "impact parameter" if station(1..5) 4 or 5,
  // i.e. Station(0..4) 3 or 4, using the function "Compare".
  // After this sorting, it is impossible to use
  // the "fNSegments" and "fIndexOfFirstSegment"
  // of the HitForRec in the first chamber to explore all segments formed with it.
  // Is this sorting really needed ????
  if ((Station == 3) || (Station == 4)) (fSegmentsPtr[Station])->Sort();
  if (fPrintLevel >= 1) cout << "Station: " << Station << "  NSegments: "
			     << fNSegments[Station] << endl;
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeTracks(void)
{
  // To make the tracks,
  // from the list of segments and points in all stations
  if (fPrintLevel >= 1) cout << "enter MakeTracks" << endl;
  // The order may be important for the following Reset's
  ResetTracks();
  ResetTrackHits();
  if (fTrackMethod == 2) { //AZ - Kalman filter
    MakeTrackCandidatesK();
    // Follow tracks in stations(1..) 3, 2 and 1
    FollowTracksK();
    // Remove double tracks
    RemoveDoubleTracksK();
    // Propagate tracks to the vertex thru absorber
    GoToVertex();
  } else { //AZ
    // Look for candidates from at least 3 aligned points in stations(1..) 4 and 5
    MakeTrackCandidates();
    // Follow tracks in stations(1..) 3, 2 and 1
    FollowTracks();
    // Remove double tracks
    RemoveDoubleTracks();
  }
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeTriggerTracks(void)
{
    // To make the trigger tracks from Local Trigger
    if (fPrintLevel >= 1) cout << "enter MakeTriggerTracks" << endl;
    ResetTriggerTracks();
    
    Int_t nTRentries;
    TClonesArray *localTrigger;
    AliMUONLocalTrigger *locTrg;
    AliMUONTriggerCircuit *circuit;
    AliMUONTriggerTrack *recTriggerTrack;
    
    TString evfoldname = AliConfig::fgkDefaultEventFolderName;//to be interfaced properly
    AliRunLoader* rl = AliRunLoader::GetRunLoader(evfoldname);
    if (rl == 0x0)
    {
	Error("MakeTriggerTracks",
	      "Can not find Run Loader in Event Folder named %s.",
	      evfoldname.Data());
	return;
    }
    AliLoader* gime = rl->GetLoader("MUONLoader");
    if (gime == 0x0)
    {
	Error("MakeTriggerTracks","Can not get MUON Loader from Run Loader.");
	return;
    }
    TTree* TR = gime->TreeR();
    
    // Loading AliRun master
    rl->LoadgAlice();
    gAlice = rl->GetAliRun();
    
    // Loading MUON subsystem
    AliMUON * pMUON = (AliMUON *) gAlice->GetDetector("MUON");
    
    nTRentries = Int_t(TR->GetEntries());
    if (nTRentries != 1) {
	cout << "Error in AliMUONEventReconstructor::MakeTriggerTracks"
	     << endl;
	cout << "nTRentries = " << nTRentries << " not equal to 1" << endl;
	exit(0);
    }
    gime->TreeR()->GetEvent(0); // only one entry  

    pMUON->GetMUONData()->SetTreeAddress("GLT");
    pMUON->GetMUONData()->GetTrigger();
    
    localTrigger = pMUON->GetMUONData()->LocalTrigger();    
    Int_t nlocals = (Int_t) (localTrigger->GetEntries());
    Float_t z11 = ( &(pMUON->Chamber(10)) )->Z();
    Float_t z21 = ( &(pMUON->Chamber(12)) )->Z();

    for (Int_t i=0; i<nlocals; i++) { // loop on Local Trigger
        locTrg = (AliMUONLocalTrigger*)localTrigger->UncheckedAt(i);	
	circuit = &(pMUON->TriggerCircuit(locTrg->LoCircuit()));
	Float_t y11 = circuit->GetY11Pos(locTrg->LoStripX()); 
	Float_t y21 = circuit->GetY21Pos(locTrg->LoStripX());
	Float_t x11 = circuit->GetX11Pos(locTrg->LoStripY());
	Float_t thetax = TMath::ATan2( x11 , z11 );
	Float_t thetay = TMath::ATan2( (y21-y11) , (z21-z11) );

	recTriggerTrack = new ((*fRecTriggerTracksPtr)[fNRecTriggerTracks])
	    AliMUONTriggerTrack(x11,y11,thetax,thetay,this);
	fNRecTriggerTracks++;
    } // end of loop on Local Trigger
    return;    
}

  //__________________________________________________________________________
Int_t AliMUONEventReconstructor::MakeTrackCandidatesWithTwoSegments(AliMUONSegment *BegSegment)
{
  // To make track candidates with two segments in stations(1..) 4 and 5,
  // the first segment being pointed to by "BegSegment".
  // Returns the number of such track candidates.
  Int_t endStation, iEndSegment, nbCan2Seg;
  AliMUONSegment *endSegment, *extrapSegment;
  AliMUONTrack *recTrack;
  Double_t mcsFactor;
  if (fPrintLevel >= 1) cout << "enter MakeTrackCandidatesWithTwoSegments" << endl;
  // Station for the end segment
  endStation = 7 - (BegSegment->GetHitForRec1())->GetChamberNumber() / 2;
  // multiple scattering factor corresponding to one chamber
  mcsFactor = 0.0136 /
    GetBendingMomentumFromImpactParam(BegSegment->GetBendingImpact());
  mcsFactor	= fChamberThicknessInX0 * mcsFactor * mcsFactor;
  // linear extrapolation to end station
  extrapSegment =
    BegSegment->CreateSegmentFromLinearExtrapToStation(endStation, mcsFactor);
  // number of candidates with 2 segments to 0
  nbCan2Seg = 0;
  // Loop over segments in the end station
  for (iEndSegment = 0; iEndSegment < fNSegments[endStation]; iEndSegment++) {
    // pointer to segment
    endSegment = (AliMUONSegment*) ((*fSegmentsPtr[endStation])[iEndSegment]);
    // test compatibility between current segment and "extrapSegment"
    // 4 because 4 quantities in chi2
    if ((endSegment->
	 NormalizedChi2WithSegment(extrapSegment,
				   fMaxSigma2Distance)) <= 4.0) {
      // both segments compatible:
      // make track candidate from "begSegment" and "endSegment"
      if (fPrintLevel >= 2)
	cout << "TrackCandidate with Segment " << iEndSegment <<
	  " in Station(0..) " << endStation << endl;
      // flag for both segments in one track:
      // to be done in track constructor ????
      BegSegment->SetInTrack(kTRUE);
      endSegment->SetInTrack(kTRUE);
      recTrack = new ((*fRecTracksPtr)[fNRecTracks])
	AliMUONTrack(BegSegment, endSegment, this);
      fNRecTracks++;
      if (fPrintLevel >= 10) recTrack->RecursiveDump();
      // increment number of track candidates with 2 segments
      nbCan2Seg++;
    }
  } // for (iEndSegment = 0;...
  delete extrapSegment; // should not delete HitForRec's it points to !!!!
  return nbCan2Seg;
}

  //__________________________________________________________________________
Int_t AliMUONEventReconstructor::MakeTrackCandidatesWithOneSegmentAndOnePoint(AliMUONSegment *BegSegment)
{
  // To make track candidates with one segment and one point
  // in stations(1..) 4 and 5,
  // the segment being pointed to by "BegSegment".
  Int_t ch, ch1, ch2, endStation, iHit, iHitMax, iHitMin, nbCan1Seg1Hit;
  AliMUONHitForRec *extrapHitForRec, *hit;
  AliMUONTrack *recTrack;
  Double_t mcsFactor;
  if (fPrintLevel >= 1)
    cout << "enter MakeTrackCandidatesWithOneSegmentAndOnePoint" << endl;
  // station for the end point
  endStation = 7 - (BegSegment->GetHitForRec1())->GetChamberNumber() / 2;
  // multiple scattering factor corresponding to one chamber
  mcsFactor = 0.0136 /
    GetBendingMomentumFromImpactParam(BegSegment->GetBendingImpact());
  mcsFactor	= fChamberThicknessInX0 * mcsFactor * mcsFactor;
  // first and second chambers(0..) in the end station
  ch1 = 2 * endStation;
  ch2 = ch1 + 1;
  // number of candidates to 0
  nbCan1Seg1Hit = 0;
  // Loop over chambers of the end station
  for (ch = ch2; ch >= ch1; ch--) {
    // linear extrapolation to chamber
    extrapHitForRec =
      BegSegment->CreateHitForRecFromLinearExtrapToChamber(ch, mcsFactor);
    // limits for the hit index in the loop
    iHitMin = fIndexOfFirstHitForRecPerChamber[ch];
    iHitMax = iHitMin + fNHitsForRecPerChamber[ch];
    // Loop over HitForRec's in the chamber
    for (iHit = iHitMin; iHit < iHitMax; iHit++) {
      // pointer to HitForRec
      hit = (AliMUONHitForRec*) ((*fHitsForRecPtr)[iHit]);
      // test compatibility between current HitForRec and "extrapHitForRec"
      // 2 because 2 quantities in chi2
      if ((hit->
	   NormalizedChi2WithHitForRec(extrapHitForRec,
				       fMaxSigma2Distance)) <= 2.0) {
	// both HitForRec's compatible:
	// make track candidate from begSegment and current HitForRec
	if (fPrintLevel >= 2)
	  cout << "TrackCandidate with HitForRec " << iHit <<
	    " in Chamber(0..) " << ch << endl;
	// flag for beginning segments in one track:
	// to be done in track constructor ????
	BegSegment->SetInTrack(kTRUE);
	recTrack = new ((*fRecTracksPtr)[fNRecTracks])
	  AliMUONTrack(BegSegment, hit, this);
	// the right place to eliminate "double counting" ???? how ????
	fNRecTracks++;
	if (fPrintLevel >= 10) recTrack->RecursiveDump();
	// increment number of track candidates
	nbCan1Seg1Hit++;
      }
    } // for (iHit = iHitMin;...
    delete extrapHitForRec;
  } // for (ch = ch2;...
  return nbCan1Seg1Hit;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::MakeTrackCandidates(void)
{
  // To make track candidates
  // with at least 3 aligned points in stations(1..) 4 and 5
  // (two Segment's or one Segment and one HitForRec)
  Int_t begStation, iBegSegment, nbCan1Seg1Hit, nbCan2Seg;
  AliMUONSegment *begSegment;
  if (fPrintLevel >= 1) cout << "enter MakeTrackCandidates" << endl;
  // Loop over stations(1..) 5 and 4 for the beginning segment
  for (begStation = 4; begStation > 2; begStation--) {
    // Loop over segments in the beginning station
    for (iBegSegment = 0; iBegSegment < fNSegments[begStation]; iBegSegment++) {
      // pointer to segment
      begSegment = (AliMUONSegment*) ((*fSegmentsPtr[begStation])[iBegSegment]);
      if (fPrintLevel >= 2)
	cout << "look for TrackCandidate's with Segment " << iBegSegment <<
	  " in Station(0..) " << begStation << endl;
      // Look for track candidates with two segments,
      // "begSegment" and all compatible segments in other station.
      // Only for beginning station(1..) 5
      // because candidates with 2 segments have to looked for only once.
      if (begStation == 4)
	nbCan2Seg = MakeTrackCandidatesWithTwoSegments(begSegment);
      // Look for track candidates with one segment and one point,
      // "begSegment" and all compatible HitForRec's in other station.
      // Only if "begSegment" does not belong already to a track candidate.
      // Is that a too strong condition ????
      if (!(begSegment->GetInTrack()))
	nbCan1Seg1Hit = MakeTrackCandidatesWithOneSegmentAndOnePoint(begSegment);
    } // for (iBegSegment = 0;...
  } // for (begStation = 4;...
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::FollowTracks(void)
{
  // Follow tracks in stations(1..) 3, 2 and 1
  // too long: should be made more modular !!!!
  AliMUONHitForRec *bestHit, *extrapHit, *extrapCorrHit, *hit;
  AliMUONSegment *bestSegment, *extrapSegment, *extrapCorrSegment, *segment;
  AliMUONTrack *track, *nextTrack;
  AliMUONTrackParam *trackParam1, trackParam[2], trackParamVertex;
  // -1 to avoid compilation warnings
  Int_t ch = -1, chInStation, chBestHit = -1, iHit, iSegment, station, trackIndex; 
  Double_t bestChi2, chi2, dZ1, dZ2, dZ3, maxSigma2Distance, mcsFactor;
  Double_t bendingMomentum, chi2Norm = 0.;
  AliMUON *pMUON = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
  // local maxSigma2Distance, for easy increase in testing
  maxSigma2Distance = fMaxSigma2Distance;
  if (fPrintLevel >= 2)
    cout << "enter FollowTracks" << endl;
  // Loop over track candidates
  track = (AliMUONTrack*) fRecTracksPtr->First();
  trackIndex = -1;
  while (track) {
    // Follow function for each track candidate ????
    trackIndex++;
    nextTrack = (AliMUONTrack*) fRecTracksPtr->After(track); // prepare next track
    if (fPrintLevel >= 2)
      cout << "FollowTracks: track candidate(0..): " << trackIndex << endl;
    // Fit track candidate
    track->SetFitMCS(0); // without multiple Coulomb scattering
    track->SetFitNParam(3); // with 3 parameters (X = Y = 0)
    track->SetFitStart(0); // from parameters at vertex
    track->Fit();
    if (fPrintLevel >= 10) {
      cout << "FollowTracks: track candidate(0..): " << trackIndex
	   << " after fit in stations(0..) 3 and 4" << endl;
      track->RecursiveDump();
    }
    // Loop over stations(1..) 3, 2 and 1
    // something SPECIAL for stations 2 and 1 for majority 3 coincidence ????
    // otherwise: majority coincidence 2 !!!!
    for (station = 2; station >= 0; station--) {
      // Track parameters at first track hit (smallest Z)
      trackParam1 = ((AliMUONTrackHit*)
		     (track->GetTrackHitsPtr()->First()))->GetTrackParam();
      // extrapolation to station
      trackParam1->ExtrapToStation(station, trackParam);
      extrapSegment = new AliMUONSegment(); //  empty segment
      extrapCorrSegment = new AliMUONSegment(); //  empty corrected segment
      // multiple scattering factor corresponding to one chamber
      // and momentum in bending plane (not total)
      mcsFactor = 0.0136 * trackParam1->GetInverseBendingMomentum();
      mcsFactor	= fChamberThicknessInX0 * mcsFactor * mcsFactor;
      // Z difference from previous station
      dZ1 = (&(pMUON->Chamber(2 * station)))->Z() -
	(&(pMUON->Chamber(2 * station + 2)))->Z();
      // Z difference between the two previous stations
      dZ2 = (&(pMUON->Chamber(2 * station + 2)))->Z() -
	(&(pMUON->Chamber(2 * station + 4)))->Z();
      // Z difference between the two chambers in the previous station
      dZ3 = (&(pMUON->Chamber(2 * station)))->Z() -
	(&(pMUON->Chamber(2 * station + 1)))->Z();
      extrapSegment->SetBendingCoorReso2(fBendingResolution * fBendingResolution);
      extrapSegment->
	SetNonBendingCoorReso2(fNonBendingResolution * fNonBendingResolution);
      extrapSegment->UpdateFromStationTrackParam
	(trackParam, mcsFactor, dZ1, dZ2, dZ3, station,
	 trackParam1->GetInverseBendingMomentum());
      // same thing for corrected segment
      // better to use copy constructor, after checking that it works properly !!!!
      extrapCorrSegment->SetBendingCoorReso2(fBendingResolution * fBendingResolution);
      extrapCorrSegment->
	SetNonBendingCoorReso2(fNonBendingResolution * fNonBendingResolution);
      extrapCorrSegment->UpdateFromStationTrackParam
	(trackParam, mcsFactor, dZ1, dZ2, dZ3, station,
	 trackParam1->GetInverseBendingMomentum());
      bestChi2 = 5.0;
      bestSegment = NULL;
      if (fPrintLevel >= 10) {
	cout << "FollowTracks: track candidate(0..): " << trackIndex
	     << " Look for segment in station(0..): " << station << endl;
      }
      // Loop over segments in station
      for (iSegment = 0; iSegment < fNSegments[station]; iSegment++) {
	// Look for best compatible Segment in station
	// should consider all possibilities ????
	// multiple scattering ????
	// separation in 2 functions: Segment and HitForRec ????
	segment = (AliMUONSegment*) ((*fSegmentsPtr[station])[iSegment]);
	// correction of corrected segment (fBendingCoor and fNonBendingCoor)
	// according to real Z value of "segment" and slopes of "extrapSegment"
	extrapCorrSegment->
	  SetBendingCoor(extrapSegment->GetBendingCoor() +
			 extrapSegment->GetBendingSlope() *
			 (segment->GetHitForRec1()->GetZ() -
			  (&(pMUON->Chamber(2 * station)))->Z()));
	extrapCorrSegment->
	  SetNonBendingCoor(extrapSegment->GetNonBendingCoor() +
			    extrapSegment->GetNonBendingSlope() *
			    (segment->GetHitForRec1()->GetZ() -
			     (&(pMUON->Chamber(2 * station)))->Z()));
	chi2 = segment->
	  NormalizedChi2WithSegment(extrapCorrSegment, maxSigma2Distance);
	if (chi2 < bestChi2) {
	  // update best Chi2 and Segment if better found
	  bestSegment = segment;
	  bestChi2 = chi2;
	}
      }
      if (bestSegment) {
	// best segment found: add it to track candidate
	track->AddSegment(bestSegment);
	// set track parameters at these two TrakHit's
	track->SetTrackParamAtHit(track->GetNTrackHits() - 2, &(trackParam[0]));
	track->SetTrackParamAtHit(track->GetNTrackHits() - 1, &(trackParam[1]));
	if (fPrintLevel >= 10) {
	  cout << "FollowTracks: track candidate(0..): " << trackIndex
	       << " Added segment in station(0..): " << station << endl;
	  track->RecursiveDump();
	}
      }
      else {
	// No best segment found:
	// Look for best compatible HitForRec in station:
	// should consider all possibilities ????
	// multiple scattering ???? do about like for extrapSegment !!!!
	extrapHit = new AliMUONHitForRec(); //  empty hit
	extrapCorrHit = new AliMUONHitForRec(); //  empty corrected hit
	bestChi2 = 3.0;
	bestHit = NULL;
	if (fPrintLevel >= 10) {
	  cout << "FollowTracks: track candidate(0..): " << trackIndex
	       << " Segment not found, look for hit in station(0..): " << station
	       << endl;
	}
	// Loop over chambers of the station
	for (chInStation = 0; chInStation < 2; chInStation++) {
	  // coordinates of extrapolated hit
	  extrapHit->
	    SetBendingCoor((&(trackParam[chInStation]))->GetBendingCoor());
	  extrapHit->
	    SetNonBendingCoor((&(trackParam[chInStation]))->GetNonBendingCoor());
	  // resolutions from "extrapSegment"
	  extrapHit->SetBendingReso2(extrapSegment->GetBendingCoorReso2());
	  extrapHit->SetNonBendingReso2(extrapSegment->GetNonBendingCoorReso2());
	  // same things for corrected hit
	  // better to use copy constructor, after checking that it works properly !!!!
	  extrapCorrHit->
	    SetBendingCoor((&(trackParam[chInStation]))->GetBendingCoor());
	  extrapCorrHit->
	    SetNonBendingCoor((&(trackParam[chInStation]))->GetNonBendingCoor());
	  extrapHit->SetBendingReso2(extrapSegment->GetBendingCoorReso2());
	  extrapHit->SetNonBendingReso2(extrapSegment->GetNonBendingCoorReso2());
	  // Loop over hits in the chamber
	  ch = 2 * station + chInStation;
	  for (iHit = fIndexOfFirstHitForRecPerChamber[ch];
	       iHit < fIndexOfFirstHitForRecPerChamber[ch] +
		 fNHitsForRecPerChamber[ch];
	       iHit++) {
	    hit = (AliMUONHitForRec*) ((*fHitsForRecPtr)[iHit]);
	    // correction of corrected hit (fBendingCoor and fNonBendingCoor)
	    // according to real Z value of "hit" and slopes of right "trackParam"
	    extrapCorrHit->
	      SetBendingCoor((&(trackParam[chInStation]))->GetBendingCoor() +
			     (&(trackParam[chInStation]))->GetBendingSlope() *
			     (hit->GetZ() -
			      (&(trackParam[chInStation]))->GetZ()));
	    extrapCorrHit->
	      SetNonBendingCoor((&(trackParam[chInStation]))->GetNonBendingCoor() +
				(&(trackParam[chInStation]))->GetNonBendingSlope() *
				(hit->GetZ() -
				 (&(trackParam[chInStation]))->GetZ()));
	    // condition for hit not already in segment ????
	    chi2 = hit->NormalizedChi2WithHitForRec(extrapHit, maxSigma2Distance);
	    if (chi2 < bestChi2) {
	      // update best Chi2 and HitForRec if better found
	      bestHit = hit;
	      bestChi2 = chi2;
	      chBestHit = chInStation;
	    }
	  }
	}
	if (bestHit) {
	  // best hit found: add it to track candidate
	  track->AddHitForRec(bestHit);
	  // set track parameters at this TrackHit
	  track->SetTrackParamAtHit(track->GetNTrackHits() - 1,
				    &(trackParam[chBestHit]));
	  if (fPrintLevel >= 10) {
	    cout << "FollowTracks: track candidate(0..): " << trackIndex
		 << " Added hit in station(0..): " << station << endl;
	    track->RecursiveDump();
	  }
	}
	else {
	  // Remove current track candidate
	  // and corresponding TrackHit's, ...
	  track->Remove();
	  delete extrapSegment;
	  delete extrapCorrSegment;
	  delete extrapHit;
	  delete extrapCorrHit;
	  break; // stop the search for this candidate:
	  // exit from the loop over station
	}
	delete extrapHit;
	delete extrapCorrHit;
      }
      delete extrapSegment;
      delete extrapCorrSegment;
      // Sort track hits according to increasing Z
      track->GetTrackHitsPtr()->Sort();
      // Update track parameters at first track hit (smallest Z)
      trackParam1 = ((AliMUONTrackHit*)
		     (track->GetTrackHitsPtr()->First()))->GetTrackParam();
      bendingMomentum = 0.;
      if (TMath::Abs(trackParam1->GetInverseBendingMomentum()) > 0.)
	bendingMomentum = TMath::Abs(1/(trackParam1->GetInverseBendingMomentum()));
      // Track removed if bendingMomentum not in window [min, max]
      if ((bendingMomentum < fMinBendingMomentum) || (bendingMomentum > fMaxBendingMomentum)) {
	track->Remove();
	break; // stop the search for this candidate:
	// exit from the loop over station 
      }
      // Track fit
      // with multiple Coulomb scattering if all stations
      if (station == 0) track->SetFitMCS(1);
      // without multiple Coulomb scattering if not all stations
      else track->SetFitMCS(0);
      track->SetFitNParam(5);  // with 5 parameters (momentum and position)
      track->SetFitStart(1);  // from parameters at first hit
      track->Fit();
      Double_t numberOfDegFree = (2.0 * track->GetNTrackHits() - 5);
      if (numberOfDegFree > 0) {
        chi2Norm =  track->GetFitFMin() / numberOfDegFree;
      } else {
	chi2Norm = 1.e10;
      }
      // Track removed if normalized chi2 too high
      if (chi2Norm > fMaxChi2) {
	track->Remove();
	break; // stop the search for this candidate:
	// exit from the loop over station 
      }
      if (fPrintLevel >= 10) {
	cout << "FollowTracks: track candidate(0..): " << trackIndex
	     << " after fit from station(0..): " << station << " to 4" << endl;
	track->RecursiveDump();
      }
      // Track extrapolation to the vertex through the absorber (Branson)
      // after going through the first station
      if (station == 0) {
	trackParamVertex = *trackParam1;
	(&trackParamVertex)->ExtrapToVertex();
	track->SetTrackParamAtVertex(&trackParamVertex);
	if (fPrintLevel >= 1) {
	  cout << "FollowTracks: track candidate(0..): " << trackIndex
	       << " after extrapolation to vertex" << endl;
	  track->RecursiveDump();
	}
      }
    } // for (station = 2;...
    // go really to next track
    track = nextTrack;
  } // while (track)
  // Compression of track array (necessary after Remove ????)
  fRecTracksPtr->Compress();
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::RemoveDoubleTracks(void)
{
  // To remove double tracks.
  // Tracks are considered identical
  // if they have at least half of their hits in common.
  // Among two identical tracks, one keeps the track with the larger number of hits
  // or, if these numbers are equal, the track with the minimum Chi2.
  AliMUONTrack *track1, *track2, *trackToRemove;
  Bool_t identicalTracks;
  Int_t hitsInCommon, nHits1, nHits2;
  identicalTracks = kTRUE;
  while (identicalTracks) {
    identicalTracks = kFALSE;
    // Loop over first track of the pair
    track1 = (AliMUONTrack*) fRecTracksPtr->First();
    while (track1 && (!identicalTracks)) {
      nHits1 = track1->GetNTrackHits();
      // Loop over second track of the pair
      track2 = (AliMUONTrack*) fRecTracksPtr->After(track1);
      while (track2 && (!identicalTracks)) {
	nHits2 = track2->GetNTrackHits();
	// number of hits in common between two tracks
	hitsInCommon = track1->HitsInCommon(track2);
	// check for identical tracks
	if ((4 * hitsInCommon) >= (nHits1 + nHits2)) {
	  identicalTracks = kTRUE;
	  // decide which track to remove
	  if (nHits1 > nHits2) trackToRemove = track2;
	  else if (nHits1 < nHits2) trackToRemove = track1;
	  else if ((track1->GetFitFMin()) < (track2->GetFitFMin()))
	    trackToRemove = track2;
	  else trackToRemove = track1;
	  // remove it
	  trackToRemove->Remove();
	}
	track2 = (AliMUONTrack*) fRecTracksPtr->After(track2);
      } // track2
      track1 = (AliMUONTrack*) fRecTracksPtr->After(track1);
    } // track1
  }
  return;
}

  //__________________________________________________________________________
void AliMUONEventReconstructor::EventDump(void)
{
  // Dump reconstructed event (track parameters at vertex and at first hit),
  // and the particle parameters

  AliMUONTrack *track;
  AliMUONTrackParam *trackParam, *trackParam1;
  Double_t bendingSlope, nonBendingSlope, pYZ;
  Double_t pX, pY, pZ, x, y, z, c;
  Int_t np, trackIndex, nTrackHits;
 
  if (fPrintLevel >= 1) cout << "****** enter EventDump ******" << endl;
  if (fPrintLevel >= 1) {
    cout << " Number of Reconstructed tracks :" <<  fNRecTracks << endl; 
  }
  fRecTracksPtr->Compress(); // for simple loop without "Next" since no hole
  // Loop over reconstructed tracks
  for (trackIndex = 0; trackIndex < fNRecTracks; trackIndex++) {
    if (fTrackMethod != 1) continue; //AZ - skip the rest for now
    if (fPrintLevel >= 1)
      cout << " track number: " << trackIndex << endl;
    // function for each track for modularity ????
    track = (AliMUONTrack*) ((*fRecTracksPtr)[trackIndex]);
    nTrackHits = track->GetNTrackHits();
    if (fPrintLevel >= 1)
      cout << " number of track hits: " << nTrackHits << endl;
    // track parameters at Vertex
    trackParam = track->GetTrackParamAtVertex();
    x = trackParam->GetNonBendingCoor();
    y = trackParam->GetBendingCoor();
    z = trackParam->GetZ();
    bendingSlope = trackParam->GetBendingSlope();
    nonBendingSlope = trackParam->GetNonBendingSlope();
    pYZ = 1/TMath::Abs(trackParam->GetInverseBendingMomentum());
    pZ = pYZ/TMath::Sqrt(1+bendingSlope*bendingSlope);
    pX = pZ * nonBendingSlope;
    pY = pZ * bendingSlope;
    c = TMath::Sign(1.0, trackParam->GetInverseBendingMomentum());
    if (fPrintLevel >= 1)
      printf(" track parameters at Vertex z= %f: X= %f Y= %f pX= %f pY= %f pZ= %f c= %f\n",
	     z, x, y, pX, pY, pZ, c);

    // track parameters at first hit
    trackParam1 = ((AliMUONTrackHit*)
		   (track->GetTrackHitsPtr()->First()))->GetTrackParam();
    x = trackParam1->GetNonBendingCoor();
    y = trackParam1->GetBendingCoor();
    z = trackParam1->GetZ();
    bendingSlope = trackParam1->GetBendingSlope();
    nonBendingSlope = trackParam1->GetNonBendingSlope();
    pYZ = 1/TMath::Abs(trackParam1->GetInverseBendingMomentum());
    pZ = pYZ/TMath::Sqrt(1.0 + bendingSlope * bendingSlope);
    pX = pZ * nonBendingSlope;
    pY = pZ * bendingSlope;
    c = TMath::Sign(1.0, trackParam1->GetInverseBendingMomentum());
    if (fPrintLevel >= 1)
      printf(" track parameters at z= %f: X= %f Y= %f pX= %f pY= %f pZ= %f c= %f\n",
	     z, x, y, pX, pY, pZ, c);
  }
  // informations about generated particles
  np = gAlice->GetMCApp()->GetNtrack();
  printf(" **** number of generated particles: %d  \n", np);
  
//    for (Int_t iPart = 0; iPart < np; iPart++) {
//      p = gAlice->Particle(iPart);
//      printf(" particle %d: type= %d px= %f py= %f pz= %f pdg= %d\n",
//  	   iPart, p->GetPdgCode(), p->Px(), p->Py(), p->Pz(), p->GetPdgCode());    
//    }
  return;
}


//__________________________________________________________________________
void AliMUONEventReconstructor::EventDumpTrigger(void)
{
  // Dump reconstructed trigger event 
  // and the particle parameters
    
  AliMUONTriggerTrack *triggertrack;
  Int_t trackIndex;
 
  if (fPrintLevel >= 1) cout << "****** enter EventDumpTrigger ******" << endl;
  if (fPrintLevel >= 1) {
      cout << " Number of Reconstructed tracks :" <<  fNRecTriggerTracks << endl;
  }
  // Loop over reconstructed tracks
  for (trackIndex = 0; trackIndex < fNRecTriggerTracks; trackIndex++) {
      triggertrack = (AliMUONTriggerTrack*) ((*fRecTriggerTracksPtr)[trackIndex]);
      printf(" trigger track number %i x11=%f y11=%f thetax=%f thetay=%f \n",
	     trackIndex,
	     triggertrack->GetX11(),triggertrack->GetY11(),
	     triggertrack->GetThetax(),triggertrack->GetThetay());      
  } 
}

//__________________________________________________________________________
void AliMUONEventReconstructor::FillEvent()
{
// Create a new AliMUONRecoEvent, fill its track list, then add it as a
// leaf in the Event branch of TreeRecoEvent tree
   cout << "Enter FillEvent() ...\n";

   if (!fRecoEvent) {
      fRecoEvent = new AliMUONRecoEvent();
   } else {
      fRecoEvent->Clear();
   }
   //save current directory
   TDirectory *current =  gDirectory;
   if (!fTreeFile)  fTreeFile  = new TFile("tree_reco.root", "RECREATE");
   if (!fEventTree) fEventTree = new TTree("TreeRecoEvent", "MUON reconstructed events");
   //AZif (fRecoEvent->MakeDumpTracks(fRecTracksPtr)) {
   if (fRecoEvent->MakeDumpTracks(fMuons, fRecTracksPtr, this)) { //AZ
      if (fPrintLevel > 1) fRecoEvent->EventInfo();
      TBranch *branch = fEventTree->GetBranch("Event");
      if (!branch) branch = fEventTree->Branch("Event", "AliMUONRecoEvent", &fRecoEvent, 64000);
      branch->SetAutoDelete();
      fTreeFile->cd();
      fEventTree->Fill();
      fTreeFile->Write();
   }
   // restore directory
   current->cd();
}

//__________________________________________________________________________
void AliMUONEventReconstructor::MakeTrackCandidatesK(void)
{
  // To make initial tracks for Kalman filter from the list of segments
  Int_t istat, iseg;
  AliMUONSegment *segment;
  AliMUONTrackK *trackK;

  if (fPrintLevel >= 1) cout << "enter MakeTrackCandidatesK" << endl;
  // Reset the TClonesArray of reconstructed tracks
  if (fRecTracksPtr) fRecTracksPtr->Delete();
  // Delete in order that the Track destructors are called,
  // hence the space for the TClonesArray of pointers to TrackHit's is freed
  fNRecTracks = 0;

  AliMUONTrackK a(this, fHitsForRecPtr); // bad idea ???
  // Loop over stations(1...) 5 and 4
  for (istat=4; istat>=3; istat--) {
    // Loop over segments in the station
    for (iseg=0; iseg<fNSegments[istat]; iseg++) {
      // Transform segments to tracks and evaluate covariance matrix
      segment = (AliMUONSegment*) ((*fSegmentsPtr[istat])[iseg]);
      trackK = new ((*fRecTracksPtr)[fNRecTracks]) AliMUONTrackK(segment);
      fNRecTracks++;
    } // for (iseg=0;...)
  } // for (istat=4;...)
  return;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::FollowTracksK(void)
{
  // Follow tracks using Kalman filter
  Bool_t Ok;
  Int_t icand, ichamBeg, ichamEnd, chamBits;
  Double_t zDipole1, zDipole2;
  AliMUONTrackK *trackK;
  AliMUONHitForRec *hit;
  AliMUONRawCluster *clus;
  TClonesArray *rawclusters;
  AliMUON *pMUON;
  clus = 0; rawclusters = 0;

  zDipole1 = GetSimpleBPosition() - GetSimpleBLength()/2;
  zDipole2 = zDipole1 + GetSimpleBLength();

  // Print hits
  pMUON  = (AliMUON*) gAlice->GetModule("MUON");
  for (Int_t i1=0; i1<fNHitsForRec; i1++) {
    hit = (AliMUONHitForRec*) ((*fHitsForRecPtr)[i1]);
    //if (hit->GetTHTrack() > 1 || hit->GetGeantSignal() == 0) continue;
    /*
    cout << " Hit #" << hit->GetChamberNumber() << " ";
    cout << hit->GetBendingCoor() << " ";
    cout << hit->GetNonBendingCoor() << " ";
    cout << hit->GetZ() << " ";
    cout << hit->GetGeantSignal() << " ";
    cout << hit->GetTHTrack() << endl;
    */
    /*
    printf(" Hit # %d %10.4f %10.4f %10.4f",
           hit->GetChamberNumber(), hit->GetBendingCoor(),
           hit->GetNonBendingCoor(), hit->GetZ());
    if (fRecGeantHits) {
      // from GEANT hits
      printf(" %3d %3d \n", hit->GetGeantSignal(), hit->GetTHTrack());
    } else {
      // from raw clusters
      rawclusters = pMUON->RawClustAddress(hit->GetChamberNumber());
      clus = (AliMUONRawCluster*) rawclusters->UncheckedAt(hit->
                                               GetHitNumber());
      printf("%3d", clus->fTracks[1]-1);
      if (clus->fTracks[2] != 0) printf("%3d \n", clus->fTracks[2]-1);
      else printf("\n");
    }
    */
  }

  icand = -1;
  Int_t nSeeds = fNRecTracks; // starting number of seeds
  // Loop over track candidates
  while (icand < fNRecTracks-1) {
    icand ++;
    trackK = (AliMUONTrackK*) ((*fRecTracksPtr)[icand]);

    // Discard candidate which will produce the double track
    if (icand > 0) {
      Ok = CheckCandidateK(icand,nSeeds);
      if (!Ok) {
        //trackK->SetRecover(-1); // mark candidate to be removed
        //continue;
      }
    }

    Ok = kTRUE;
    if (trackK->GetRecover() == 0) hit = (AliMUONHitForRec*) 
                                   trackK->GetHitOnTrack()->Last(); // last hit
    else hit = (AliMUONHitForRec*) (*trackK->GetHitOnTrack())[1]; // 2'nd hit
    ichamBeg = hit->GetChamberNumber();
    ichamEnd = 0;
    // Check propagation direction
    if (trackK->GetTrackDir() > 0) {
      ichamEnd = 9; // forward propagation
      Ok = trackK->KalmanFilter(ichamBeg,ichamEnd,kFALSE,zDipole1,zDipole2);
      if (Ok) {
        ichamBeg = ichamEnd;
        ichamEnd = 6; // backward propagation
	// Change weight matrix and zero fChi2 for backpropagation
        trackK->StartBack();
        Ok = trackK->KalmanFilter(ichamBeg,ichamEnd,kTRUE,zDipole1,zDipole2);
        ichamBeg = ichamEnd;
        ichamEnd = 0;
      }
    } else {
      if (trackK->GetBPFlag()) {
	// backpropagation
        ichamEnd = 6; // backward propagation
	// Change weight matrix and zero fChi2 for backpropagation
        trackK->StartBack();
        Ok = trackK->KalmanFilter(ichamBeg,ichamEnd,kTRUE,zDipole1,zDipole2);
        ichamBeg = ichamEnd;
        ichamEnd = 0;
      }
    }

    if (Ok) {
      trackK->SetTrackDir(-1);
      trackK->SetBPFlag(kFALSE);
      Ok = trackK->KalmanFilter(ichamBeg,ichamEnd,kFALSE,zDipole1,zDipole2);
    }
    if (Ok) trackK->SetTrackQuality(0); // compute "track quality"
    else trackK->SetRecover(-1); // mark candidate to be removed

    // Majority 3 of 4 in first 2 stations
    chamBits = 0;
    for (Int_t i=0; i<trackK->GetNTrackHits(); i++) {
      hit = (AliMUONHitForRec*) (*trackK->GetHitOnTrack())[i];
      chamBits |= BIT(hit->GetChamberNumber()-1);
    }
    //if (!((chamBits&3)==3 || (chamBits>>2&3)==3)) trackK->SetRecover(-1); 
                                 //mark candidate to be removed
  } // while

  for (Int_t i=0; i<fNRecTracks; i++) {
    trackK = (AliMUONTrackK*) ((*fRecTracksPtr)[i]);
    if (trackK->GetRecover() < 0) fRecTracksPtr->RemoveAt(i);
  }

  // Compress TClonesArray
  fRecTracksPtr->Compress();
  fNRecTracks = fRecTracksPtr->GetEntriesFast();
  return;
}

//__________________________________________________________________________
Bool_t AliMUONEventReconstructor::CheckCandidateK(Int_t icand, Int_t nSeeds)
{
  // Discards track candidate if it will produce the double track (having
  // the same seed segment hits as hits of a good track found before)
  AliMUONTrackK *track1, *track2;
  AliMUONHitForRec *hit1, *hit2, *hit;

  track1 = (AliMUONTrackK*) ((*fRecTracksPtr)[icand]);
  hit1 = (AliMUONHitForRec*) (*track1->GetHitOnTrack())[0]; // 1'st hit
  hit2 = (AliMUONHitForRec*) (*track1->GetHitOnTrack())[1]; // 2'nd hit

  for (Int_t i=0; i<icand; i++) {
    track2 = (AliMUONTrackK*) ((*fRecTracksPtr)[i]);
    //if (track2->GetRecover() < 0) continue;
    if (track2->GetRecover() < 0 && icand >= nSeeds) continue;

    if (track1->GetStartSegment() == track2->GetStartSegment()) {
      return kFALSE;
    } else {
      Int_t nSame = 0;
      for (Int_t j=0; j<track2->GetNTrackHits(); j++) {
        hit = (AliMUONHitForRec*) (*track2->GetHitOnTrack())[j];
        if (hit == hit1 || hit == hit2) {
          nSame++;
          if (nSame == 2) return kFALSE;
        }
      } // for (Int_t j=0;
    }
  } // for (Int_t i=0;
  return kTRUE;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::RemoveDoubleTracksK(void)
{
  // Removes double tracks (sharing more than half of their hits). Keeps
  // the track with higher quality
  AliMUONTrackK *track1, *track2, *trackToKill;

  // Sort tracks according to their quality
  fRecTracksPtr->Sort();

  // Loop over first track of the pair
  track1 = (AliMUONTrackK*) fRecTracksPtr->First();
  while (track1) {
    // Loop over second track of the pair
    track2 = (AliMUONTrackK*) fRecTracksPtr->After(track1);
    while (track2) {
      // Check whether or not to keep track2
      if (!track2->KeepTrack(track1)) {
        cout << " Killed track: " << 1/(*track2->GetTrackParameters())(4,0) <<
	  " " << track2->GetTrackQuality() << endl;
        trackToKill = track2;
        track2 = (AliMUONTrackK*) fRecTracksPtr->After(track2);
        trackToKill->Kill();
        fRecTracksPtr->Compress();
      } else track2 = (AliMUONTrackK*) fRecTracksPtr->After(track2);
    } // track2
    track1 = (AliMUONTrackK*) fRecTracksPtr->After(track1);
  } // track1

  fNRecTracks = fRecTracksPtr->GetEntriesFast();
  cout << " Number of Kalman tracks: " << fNRecTracks << endl;
}

//__________________________________________________________________________
void AliMUONEventReconstructor::GoToVertex(void)
{
  // Propagates track to the vertex thru absorber
  // (using Branson correction for now)

  Double_t zVertex;
  zVertex = 0;
  for (Int_t i=0; i<fNRecTracks; i++) {
    //((AliMUONTrackK*)(*fRecTracksPtr)[i])->Branson();
    //((AliMUONTrackK*)(*fRecTracksPtr)[i])->GoToZ(zVertex); // w/out absorber
    ((AliMUONTrackK*)(*fRecTracksPtr)[i])->SetTrackQuality(1); // compute Chi2
    ((AliMUONTrackK*)(*fRecTracksPtr)[i])->GoToVertex(); // with absorber
  }
}

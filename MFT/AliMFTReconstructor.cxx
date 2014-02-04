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

//====================================================================================================================================================
//
//      Event reconstruction class for the ALICE Muon Forward Tracker
//
//      Contact author: antonio.uras@cern.ch
//
//====================================================================================================================================================

#include "TObjArray.h"
#include "TTree.h"
#include "AliMFTSegmentation.h"
#include "AliMFTClusterFinder.h"
#include "AliReconstructor.h"
#include "AliMFTReconstructor.h"

ClassImp(AliMFTReconstructor)

//====================================================================================================================================================

AliMFTReconstructor::AliMFTReconstructor():
  AliReconstructor(), 
  fDigits(0x0),
  fNPlanes(0)
{

  // default constructor 

}

//====================================================================================================================================================

AliMFTReconstructor::~AliMFTReconstructor() {
  
  // destructor
  
  if (fDigits) {
    for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
      if (fDigits->At(iPlane)) fDigits->At(iPlane)->Delete();
    }
    fDigits->Delete();
    delete fDigits;
    fDigits=0;
  }
  
}

//====================================================================================================================================================

void AliMFTReconstructor::Clear(const Option_t* /*opt*/) {
	
  // Clear arrays
  
  if (fDigits) {
    for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
      if (fDigits->At(iPlane)) ((TClonesArray*)fDigits->At(iPlane))->Delete();
    }
    fDigits->Delete();
    delete fDigits;
    fDigits = NULL;
  }
  
}

//====================================================================================================================================================

void AliMFTReconstructor::Init() {

  AliMFTSegmentation *segmentation = new AliMFTSegmentation("AliMFTGeometry.root");
  fNPlanes = segmentation->GetNPlanes();
  delete segmentation;

  fDigits = new TObjArray(fNPlanes);
  fDigits->SetOwner(kTRUE);
  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
    fDigits->AddAt(new TClonesArray("AliMFTDigit"),iPlane);
    ((TClonesArray*)fDigits->At(iPlane))->SetOwner(kTRUE);
  }
  
  AliInfo(" ************* Using the MFT reconstructor! ************ ");
  
  return;

}

//====================================================================================================================================================

void AliMFTReconstructor::ResetDigits() {

  // Reset number of digits and the digits array for the MFT detector.

  if (!fDigits) return;
  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
    ResetDigits(iPlane);
  }

}

//====================================================================================================================================================

void AliMFTReconstructor::ResetDigits(Int_t plane) {

  // Reset number of digits and the digits array for this branch.

  if (fDigits->At(plane)) ((TClonesArray*)fDigits->At(plane))->Clear();

}

//====================================================================================================================================================

void AliMFTReconstructor::Reconstruct(TTree *digitsTree, TTree *clustersTree) const {

  AliInfo("Starting Reconstruction for MFT");

  // Clusterization

  AliDebug(1, Form("nPlanes = %d",fNPlanes));

  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
    AliDebug(1, Form("Setting Address for Branch Plane_%02d", iPlane)); 
    digitsTree->SetBranchAddress(Form("Plane_%02d",iPlane), &(*fDigits)[iPlane]);
  }
 
  digitsTree->GetEntry(0);

  AliDebug(1, "Creating clusterFinder");
  AliMFTClusterFinder *clusterFinder = new AliMFTClusterFinder();
  clusterFinder->Init("AliMFTGeometry.root");
  AliDebug(1, "clusterFinder->MakeClusterBranch(clustersTree)");
  clusterFinder->MakeClusterBranch(clustersTree);
  AliDebug(1, "clusterFinder->SetClusterTreeAddress(clustersTree)");
  clusterFinder->SetClusterTreeAddress(clustersTree);
  AliDebug(1, "clusterFinder->DigitsToClusters(fDigits)");
  clusterFinder->DigitsToClusters(fDigits);
  AliDebug(1, "clustersTree->Fill()");
  clustersTree->Fill();                         // fill tree for current event
  AliDebug(1, "delete clusterFinder");
  delete clusterFinder;

  for (Int_t iPlane=0; iPlane<fNPlanes; iPlane++) {
    AliDebug(1, Form("fDigits->At(%d)->Clear()",iPlane));
    ((TClonesArray*)fDigits->At(iPlane))->Delete();
  }

}

//====================================================================================================================================================

AliTracker* AliMFTReconstructor::CreateTracker() const {

  // Create a MFT tracker: global MUON+MFT tracks

  AliMFTTrackerMU *tracker = new AliMFTTrackerMU();   // tracker for muon tracks (MFT + MUON) 

  return tracker;
  
}

//====================================================================================================================================================

AliTracker* AliMFTReconstructor::CreateTrackleter() const {

  AliInfo("Not implemented");

  // Create a MFT tracker: standalone MFT tracks

  //  AliMFTTrackerSA *tracker = new AliMFTTrackerSA();   // tracker for MFT tracks

  return NULL;
  
}

//====================================================================================================================================================


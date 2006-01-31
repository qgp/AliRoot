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

#include <Riostream.h>
#include <TClonesArray.h>
#include <TArrayS.h>
#include <TArrayD.h>
#include "AliMUONEventRecoCombi.h"
#include "AliMUONData.h"
#include "AliMUONDetElement.h"
#include "AliMUONDigit.h"
#include "AliMUONHitForRec.h"
#include "AliMUONRawCluster.h"
#include "AliMUONTrackK.h"
#include "AliMUONTrackReconstructor.h"
#include "AliMUONConstants.h"
#include "AliLoader.h"

ClassImp(AliMUONEventRecoCombi)

AliMUONEventRecoCombi* AliMUONEventRecoCombi::fgRecoCombi = 0; 

//_________________________________________________________________________
AliMUONEventRecoCombi::AliMUONEventRecoCombi() : TObject()
{
  // Ctor

  fDetElems = new TClonesArray("AliMUONDetElement", 20);
  fZ = new TArrayD(20);
  fNZ = 0;
  fDEvsZ = NULL;
}

//_________________________________________________________________________
AliMUONEventRecoCombi* AliMUONEventRecoCombi::Instance()
{
// return pointer to the singleton instance

  if (fgRecoCombi == 0) {
    fgRecoCombi = new AliMUONEventRecoCombi();
  }
  //fDetElems = new TClonesArray("AliMUONDetElement", 20);
  //fZ = new TArrayD(20);
  //fNZ = 0;
  return fgRecoCombi;
}

//_________________________________________________________________________
AliMUONEventRecoCombi::~AliMUONEventRecoCombi()
{
  // Destructor
  delete fDetElems;
  delete fZ;
  delete [] fDEvsZ;
}

//_________________________________________________________________________
void AliMUONEventRecoCombi::FillEvent(AliMUONData *dataCluster, AliMUONData *dataEvent, AliMUONClusterFinderAZ *recModel)
{
  // Fill event information

  // Clear previous event
  fDetElems->Delete();
  for (Int_t i = 0; i < fNZ; i++) delete [] fDEvsZ[i];
  delete [] fDEvsZ; fDEvsZ = NULL;
  fNZ = -1;

  Int_t nDetElem = 0;
  for (Int_t ich = 0; ich < 6; ich++) {
    // loop over chambers 0-5
    TClonesArray *digs = dataCluster->Digits(ich);
    //cout << ich << " " << digs << " " << digs->GetEntriesFast() << endl;
    Int_t idDE = -1;
    for (Int_t i = 0; i < digs->GetEntriesFast(); i++) {
      AliMUONDigit *dig = (AliMUONDigit*) digs->UncheckedAt(i);
      if (dig->DetElemId() != idDE) {
	idDE = dig->DetElemId();
	new ((*fDetElems)[nDetElem++]) AliMUONDetElement(idDE, dig, recModel);
      }
      else ((AliMUONDetElement*)fDetElems->UncheckedAt(nDetElem-1))->AddDigit(dig);
    }
  }

  // Sort according to Z
  fDetElems->Sort();
  //cout << nDetElem << endl;
  // Fill det. elems. position index in the container
  for (Int_t i = 0; i < nDetElem; i++) 
    ((AliMUONDetElement*)fDetElems->UncheckedAt(i))->SetIndex(i);

  // Find groups of det. elements with the same Z
  Double_t z0 = -99999;
  TArrayS *nPerZ = new TArrayS(20);
  for (Int_t i = 0; i < nDetElem; i++) {
    AliMUONDetElement *detElem = (AliMUONDetElement*) fDetElems->UncheckedAt(i);
    detElem->Fill(dataCluster);
    //cout << i << " " << detElem->Z() << endl;
    if (detElem->Z() - z0 < 0.5) { 
      // the same Z
      (*nPerZ)[fNZ]++;
    } else {
      if (fZ->GetSize() <= fNZ) fZ->Set(fZ->GetSize()+10);
      if (nPerZ->GetSize() <= fNZ) nPerZ->Set(nPerZ->GetSize()+10);
      (*fZ)[++fNZ] = detElem->Z();
      z0 = detElem->Z();
      (*nPerZ)[fNZ]++;
    }
  }
  fNZ++;
  /*
  cout << fNZ << endl;
  for (Int_t i = 0; i < 7; i++) {
    cout << i << " " << dataCluster->RawClusters(i)->GetEntriesFast() << endl;
  }
  */

  // Build list of DE locations vs Z
  fDEvsZ = new Int_t* [fNZ];
  Int_t iPos = 0;
  for (Int_t i = 0; i < fNZ; i++) {
    Int_t *idPerZ = new Int_t[(*nPerZ)[i]+1]; 
    for (Int_t j = 1; j < (*nPerZ)[i]+1; j++) idPerZ[j] = iPos++;
    idPerZ[0] = (*nPerZ)[i]; // number of DE's as first element of array
    fDEvsZ[i] = idPerZ;
    //cout << (*nPerZ)[i] << " ";
  }
  //cout << endl;
  delete nPerZ;

  // Fill rec. point container for stations 4 and 5
  //cout << dataEvent->TreeR() << endl;
  //dataEvent->MakeBranch("RC");
  dataEvent->SetTreeAddress("RCC");
  for (Int_t ch = 6; ch < 10; ch++) {
    TClonesArray *raw = dataCluster->RawClusters(ch);
    //cout << raw->GetEntriesFast() << " " << dataEvent->RawClusters(ch) << endl;
    for (Int_t i = 0; i < raw->GetEntriesFast(); i++) {
      AliMUONRawCluster *clus = (AliMUONRawCluster*) raw->UncheckedAt(i);
      dataEvent->AddRawCluster(ch, *clus);
    }
  }
  //dataCluster->SetTreeAddress("RC");
}

//_________________________________________________________________________
void AliMUONEventRecoCombi::FillRecP(AliMUONData *dataCluster, AliMUONTrackReconstructor *recoTrack)
{
  // Fill rec. points used for tracking from det. elems

  TClonesArray *tracks = recoTrack->GetRecTracksPtr();
  for (Int_t i = 0; i < recoTrack->GetNRecTracks(); i++) {
    AliMUONTrackK *track = (AliMUONTrackK*) tracks->UncheckedAt(i);
    TObjArray *hits = track->GetHitOnTrack();
    for (Int_t j = 0; j < track->GetNTrackHits(); j++) {
      AliMUONHitForRec *hit = (AliMUONHitForRec*) hits->UncheckedAt(j);
      if (hit->GetHitNumber() >= 0) continue;
      // Combined cluster / track finder
      Int_t index = -hit->GetHitNumber() / 100000;
      Int_t iPos = -hit->GetHitNumber() - index * 100000;
      AliMUONRawCluster *clus = (AliMUONRawCluster*) DetElem(index-1)->RawClusters()->UncheckedAt(iPos);
      //cout << j << " " << iPos << " " << clus << " " << index << " " << DetElem(index-1)->Chamber() << endl; 
      dataCluster->AddRawCluster(DetElem(index-1)->Chamber(), *clus);
    }
  }
  /*
  for (Int_t ch = 0; ch < 10; ch++) {
    TClonesArray *raw = dataCluster->RawClusters(ch);
    cout << ch << " " << raw->GetEntriesFast() << endl;
  }
  */
  // Reset raw cluster tree
  /*
  char branchname[30];
  TBranch * branch = 0x0;
  if ( dataCluster->TreeR()) {
    if ( dataCluster->IsTriggerBranchesInTree() ) {
      // Branch per branch filling
      for (int i=0; i<AliMUONConstants::NTrackingCh(); i++) {
        sprintf(branchname,"%sRawClusters%d",dataCluster->GetName(),i+1);
        branch = dataCluster->TreeR()->GetBranch(branchname);
        //branch->Fill();
        //branch->Reset();
        //branch->Clear();
        branch->Delete();
      }
    }
    //else  TreeR()->Fill();
    else  dataCluster->TreeR()->Reset();
  }
  */
}

//_________________________________________________________________________
Int_t AliMUONEventRecoCombi::IZfromHit(AliMUONHitForRec *hit) const
{
  // Get Iz of det. elem. from the hit

  Int_t index = -hit->GetHitNumber() / 100000 - 1, iz0 = -1;
  for (Int_t iz = 0; iz < fNZ; iz++) {
    Int_t *pDEatZ = DEatZ(iz);
    Int_t nDetElem = pDEatZ[-1];
    for (Int_t j = 0; j < nDetElem; j++) {
      if (pDEatZ[j] != index) continue;
      iz0 = iz;
      break;
    }
    if (iz0 >= 0) break;
  }
  return iz0;
}

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

#include <TObjArray.h>
#include <TClonesArray.h>
#include "AliMUONDetElement.h"
#include "AliMUON.h"
#include "AliMUONSegmentation.h"
#include "AliMUONDigit.h"
#include "AliMUONHitMapA1.h"
#include "AliMUONData.h"
#include "AliMUONRawCluster.h"
#include "AliMUONHitForRec.h"
#include "AliMUONClusterInput.h"
#include "AliMUONClusterFinderAZ.h"
#include "AliRun.h"
#include "AliLog.h"

ClassImp(AliMUONDetElement) // Class implementation in ROOT context
  FILE *lun = 0x0; //fopen("hitmap.dat","w");

//_________________________________________________________________________
AliMUONDetElement::AliMUONDetElement()
  : TObject()
{
// Default constructor
  for (Int_t i = 0; i < 2; i++) {
    fHitMap[i] = NULL;
    fDigits[i] = NULL;
    fSeg[i] = NULL;
  }
  fRawClus = fHitsForRec = NULL;
  fRecModel = NULL;
} 

//_________________________________________________________________________
AliMUONDetElement::AliMUONDetElement(Int_t idDE, AliMUONDigit *dig, AliMUONClusterFinderAZ *recModel) 
  : TObject()
{
  // Constructor
  fidDE = idDE;
  fChamber = fidDE / 100 - 1;
  fDigits[0] = new TObjArray(10);
  fDigits[1] = new TObjArray(10);
  fRawClus = new TObjArray(10);
  fHitsForRec = new TClonesArray("AliMUONHitForRec",10);
  fNHitsForRec = 0;
  fRecModel = recModel;
  AliMUON *pMUON = (AliMUON*) gAlice->GetModule("MUON");
  AliMUONSegmentation *pSegmentation = pMUON->GetSegmentation();
  fSeg[0] = pSegmentation->GetModuleSegmentation(fChamber, 0);
  fSeg[1] = pSegmentation->GetModuleSegmentation(fChamber, 1);
  Float_t x, y, z;
  fSeg[dig->Cathode()]->GetPadC(fidDE, dig->PadX(), dig->PadY(), x, y, z);
  fZ = z;
  AddDigit(dig);
}

//_________________________________________________________________________
AliMUONDetElement::~AliMUONDetElement()
{
  // Destructor
  for (Int_t i = 0; i < 2; i++) {
    delete fHitMap[i]; fHitMap[i] = NULL; 
    delete fDigits[i]; fDigits[i] = NULL; 
  }
  if (fRawClus) { fRawClus->Delete(); delete fRawClus; fRawClus = 0; }
  //if (fRawClus) { delete fRawClus; fRawClus = 0; }
  delete fHitsForRec; fHitsForRec = 0;
}

//_________________________________________________________________________
AliMUONDetElement::AliMUONDetElement (const AliMUONDetElement& rhs)
  : TObject(rhs)
{
// Copy constructor

  AliFatal("Not implemented.");
}

//________________________________________________________________________
AliMUONDetElement & AliMUONDetElement::operator = (const AliMUONDetElement& rhs)
{
// Assignement operator

  if (this == &rhs) return *this;
  AliFatal( "Not implemented.");
  return *this;
}

//_________________________________________________________________________
Int_t AliMUONDetElement::Compare(const TObject* detElem) const
{
  // "Compare" function to sort in Z (towards interaction point)
  // Returns -1 (0, +1) if charge of current pixel
  // is greater than (equal to, less than) charge of pixel
  if (fZ > ((AliMUONDetElement*)detElem)->Z()) return(+1);
  else if (fZ == ((AliMUONDetElement*)detElem)->Z()) return( 0);
  else return(-1);
}

//_________________________________________________________________________
void AliMUONDetElement::Fill(AliMUONData */*data*/)
{
  // Fill hit maps
  fLeft[0] = fDigits[0]->GetEntriesFast();
  fLeft[1] = fDigits[1]->GetEntriesFast();

  fHitMap[0] = new AliMUONHitMapA1(fidDE, fSeg[0], fDigits[0]);
  fHitMap[1] = new AliMUONHitMapA1(fidDE, fSeg[1], fDigits[1]);
  fHitMap[0]->FillHits();
  fHitMap[1]->FillHits();

  // The part below is just for debugging (fill rec. points already found)
  /*
  fLeft[0] = fLeft[1] = 0;
  TClonesArray *rawClus = data->RawClusters(fChamber);
  cout << rawClus << " " << rawClus->GetEntriesFast() << endl;
  for (Int_t i = 0; i < rawClus->GetEntriesFast(); i++) {
    AliMUONRawCluster *recP = (AliMUONRawCluster*) rawClus->UncheckedAt(i);
    cout << fChamber << " " << recP->GetZ(0) << " " << recP->GetZ(1) << " " << fZ << endl;
    if (TMath::Abs(recP->GetZ(0)-fZ) > 0.5) continue;
    if (!Inside(recP->GetX(0), recP->GetY(0), recP->GetZ(0))) continue;
    AddHitForRec(recP); // add hit for rec.
    rawClus->RemoveAt(i); // remove
  }
  cout << fHitsForRec->GetEntriesFast() << endl;
  rawClus->Compress();
  */
}

//_________________________________________________________________________
void AliMUONDetElement::AddDigit(AliMUONDigit *dig)
{
  // Add digit

  fDigits[dig->Cathode()]->Add(dig);
}

//_________________________________________________________________________
Bool_t AliMUONDetElement::Inside(Double_t x, Double_t y, Double_t z) const
{
  // Check if point is inside detection element

  Int_t ix, iy;
  for (Int_t i = 0; i < 2; i++) {
    if (!fSeg[i]) continue;
    fSeg[i]->GetPadI(fidDE, x, y, z, ix, iy);
    //cout << x << " " << y << " " << z << " " << fChamber << " " << ix << " " << iy << " " << fSeg[i]->Npx(fidDE) << " " << fSeg[i]->Npy(fidDE) /*<< " " << fSeg[i]->GetPadI(fidDE, x, y, z, ix, iy)*/ << endl; 
    if (ix > 0 && iy > 0 && ix <= fSeg[i]->Npx(fidDE) && iy <= fSeg[i]->Npy(fidDE)) return kTRUE;
  }
  // Check for edge effect (extrapolated track "right outside" det. elem. boundaries (+- 1cm in X and Y)
  for (Int_t i = 0; i < 2; i++) {
    if (!fSeg[i]) continue;
    for (Int_t idx = -1; idx < 2; idx++) {
      Double_t x1 = x + 1. * idx;
      for (Int_t idy = -1; idy < 2; idy++) {
	if (idx == 0 && idy == 0) continue;
	Double_t y1 = y + 1. * idy;
	fSeg[i]->GetPadI(fidDE, x1, y1, z, ix, iy);
	//cout << x1 << " " << y1 << " " << z << " " << fChamber << " " << ix << " " << iy << " " << fSeg[i]->Npx(fidDE) << " " << fSeg[i]->Npy(fidDE) /*<< " " << fSeg[i]->GetPadI(fidDE, x, y, z, ix, iy)*/ << endl; 
	if (ix > 0 && iy > 0 && ix <= fSeg[i]->Npx(fidDE) && iy <= fSeg[i]->Npy(fidDE)) return kTRUE;
      }
    }
  }
  return kFALSE;
}

//_________________________________________________________________________
void AliMUONDetElement::ClusterReco(Double_t xTrack, Double_t yTrack)
{
  // Run cluster reconstruction around point (x,y)

  if (fLeft[0] == 0 && fLeft[1] == 0) return; // all digits have been used 
  Float_t dx, dy;
  dx = dy = 5; // 5 cm for now 
  AliMUONClusterInput::Instance()->SetDigits(fChamber, fidDE, 
             (TClonesArray*)fDigits[0], (TClonesArray*)fDigits[1]);

  // Mark used pads
  for (Int_t cath = 0; cath < 2; cath++) {
    if (fDigits[cath]->GetEntriesFast() == 0) continue; // empty cathode
    for (Int_t i = 0; i < fDigits[cath]->GetEntriesFast(); i++) {
      if (fLeft[cath] == 0) { fRecModel->SetUsed(cath,i); continue; }
      AliMUONDigit *dig = (AliMUONDigit*) fDigits[cath]->UncheckedAt(i);
      //cout << i << " " << dig->PadX() << " " << dig->PadY() << " " << fHitMap[cath]->TestHit(dig->PadX(), dig->PadY()) << endl;
      if (fHitMap[cath]->TestHit(dig->PadX(), dig->PadY()) == kUsed) fRecModel->SetUsed(cath,i);
      else fRecModel->SetUnused(cath,i);
    }
  }

  fRecModel->ResetRawClusters();

  for (Int_t cath = 0; cath < 2; cath++) {
    if (fDigits[cath]->GetEntriesFast() == 0) continue; // empty cathode
    // Loop over pads
    for (fSeg[cath]->FirstPad(fidDE, xTrack, yTrack, fZ, dx, dy);
	 fSeg[cath]->MorePads(fidDE);
	 fSeg[cath]->NextPad(fidDE)) {
      if (fLeft[cath] == 0) break;
      //cout << cath << " " << fSeg[cath]->Ix() << " " << fSeg[cath]->Iy() << " " << fSeg[cath]->DetElemId() << " " << fHitMap[cath]->TestHit(fSeg[cath]->Ix(), fSeg[cath]->Iy()) << endl;
      if (fHitMap[cath]->TestHit(fSeg[cath]->Ix(), fSeg[cath]->Iy()) == kEmpty ||
	  fHitMap[cath]->TestHit(fSeg[cath]->Ix(), fSeg[cath]->Iy()) == kUsed) continue;

      // Set starting pad
      for (Int_t j = 0; j < fDigits[cath]->GetEntriesFast(); j++) {
	AliMUONDigit *dig = (AliMUONDigit*) fDigits[cath]->UncheckedAt(j);
	if (dig->PadX() != fSeg[cath]->Ix() || dig->PadY() != fSeg[cath]->Iy()) continue;
	//cout << fidDE << " " << j << " " << fSeg[cath]->Ix() << " " << fSeg[cath]->Iy() << endl;
	fRecModel->SetStart(cath, j);
	break;
      }

      fRecModel->FindRawClusters();
      Int_t nClusEnd = fRecModel->GetRawClusters()->GetEntriesFast();
      //cout << " ***nclus: " << nClusEnd << endl;
      for (Int_t i = 0; i < nClusEnd; i++) {
	AliMUONRawCluster *clus = (AliMUONRawCluster*) fRecModel->GetRawClusters()->UncheckedAt(i);
	AddHitForRec(clus); // add hit for rec.
	//cout << clus->GetX(0) << " " << clus->GetY(0) << endl;
      }
      // Mark used pads
      for (Int_t cath1 = 0; cath1 < 2; cath1++) {
	for (Int_t j = 0; j < fDigits[cath1]->GetEntriesFast(); j++) {
	  if (fLeft[cath1] == 0) break;
	  AliMUONDigit *dig = (AliMUONDigit*) fDigits[cath1]->UncheckedAt(j);
	  Float_t x, y, z;
	  fSeg[cath1]->GetPadC(fidDE,dig->PadX(),dig->PadY(),x,y,z);
	  //cout << "clus " << cath1 << " " << fLeft[cath1] << " " << dig->PadX() << " " << dig->PadY() << " " << x << " " << y << " " << z << " " << fRecModel->GetUsed(cath1,j) << endl;
	  if (!fRecModel->GetUsed(cath1,j)) continue;
	  if (fHitMap[cath1]->TestHit(dig->PadX(), dig->PadY()) == kUsed) continue;
	  fHitMap[cath1]->FlagHit(dig->PadX(), dig->PadY());
	  if (lun) fprintf(lun," %d %d %d %d \n", cath1, fidDE, dig->PadX(), dig->PadY());
	  fLeft[cath1]--;
	}
      }
    } // for (fSeg[cath]->FirstPad(...
  } // for (Int_t cath = 0;
}

//_________________________________________________________________________
void AliMUONDetElement::AddHitForRec(AliMUONRawCluster *clus)
{
  // Make HitForRec from raw cluster (rec. point)

  fRawClus->Add(new AliMUONRawCluster(*clus));
  AliMUONHitForRec *hitForRec = 
    new ((*fHitsForRec)[fNHitsForRec++]) AliMUONHitForRec(clus);

  // more information into HitForRec
  //  resolution: info should be already in raw cluster and taken from it ????
  hitForRec->SetBendingReso2(-1); //fBendingResolution * fBendingResolution);
  hitForRec->SetNonBendingReso2(-1); //fNonBendingResolution * fNonBendingResolution);
  //  original raw cluster
  hitForRec->SetChamberNumber(fChamber);
  hitForRec->SetZ(clus->GetZ(0));
  //hitForRec->SetHitNumber(-(fIndex+1)*100000-fNHitsForRec+1);
  hitForRec->SetHitNumber(-(fIndex+1)*100000-fRawClus->GetEntriesFast()+1);
  //delete clus; // for now
}

/*
//_________________________________________________________________________
Int_t AliMUONDetElement::GetMapElem(AliMUONDigit *digit)
{
  Int_t cath = digit->Cathode();
  return 0;

}

//_________________________________________________________________________
void AliMUONDetElement::SetMapElem(const AliMUONDigit *digit, Int_t flag)
{
  Int_t cath = digit->Cathode();
}
*/

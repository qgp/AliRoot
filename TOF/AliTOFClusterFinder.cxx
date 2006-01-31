/***************************************************************************
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

/* 

Revision 0.03  2005/07/28 A. De Caro
         Implement public method
	 Raw2Digits(Int_t, AliRawReader *)
	 to convert digits from raw data in MC digits
	 (temporary solution)

Revision 0.02  2005/07/27 A. De Caro
         Implement public method
	 Digits2RecPoint(Int_t)
	 to convert digits in clusters

Revision 0.02  2005/07/26 A. De Caro
         Implement private methods
	 InsertCluster(AliTOFcluster *)
	 FindClusterIndex(Double_t)
	 originally implemented in AliTOFtracker
	 by S. Arcelli and C. Zampolli

Revision 0.01  2005/07/25 A. De Caro
         Implement public methods
	 Digits2RecPoint(AliRawReader *, TTree *)
	 Digits2RecPoint(Int_t, AliRawReader *)
	 to convert raw data in clusters
 */

////////////////////////////////////////////////////////////////
//                                                            //
//         Class for TOF cluster finder                       //
//                                                            //
// Starting from Raw Data, create rec points,                 //
//                         fill TreeR for TOF,                //
//                         write TOF.RecPoints.root file      //
//                                                            //
////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <TTree.h>
#include <TObjArray.h>
#include <TClonesArray.h>

#include "AliLog.h"
#include "AliRunLoader.h"
#include "AliLoader.h"

#include "AliTOFdigit.h"
#include "AliTOFcluster.h"
#include "AliTOFGeometry.h"
#include "AliTOFGeometryV4.h"
#include "AliTOFGeometryV5.h"
#include "AliTOFRawStream.h"

#include "AliTOFClusterFinder.h"

ClassImp(AliTOFClusterFinder)

AliTOFClusterFinder::AliTOFClusterFinder():
  fRunLoader(0),
  fTOFLoader(0),
  fTreeD(0),
  fTreeR(0),
  fDigits(new TClonesArray("AliTOFdigit", 4000)),
  fRecPoints(new TClonesArray("AliTOFcluster", 4000)),
  fNumberOfTofClusters(0)
{
//
// Constructor
//

  fTOFGeometry = new AliTOFGeometry();

}
//______________________________________________________________________________

AliTOFClusterFinder::AliTOFClusterFinder(AliRunLoader* runLoader):
  fRunLoader(runLoader),
  fTOFLoader(runLoader->GetLoader("TOFLoader")),
  fTreeD(0),
  fTreeR(0),
  fDigits(new TClonesArray("AliTOFdigit", 4000)),
  fRecPoints(new TClonesArray("AliTOFcluster", 4000)),
  fNumberOfTofClusters(0)
{
//
// Constructor
//

  runLoader->CdGAFile();
  TFile *in=(TFile*)gFile;
  in->cd();
  fTOFGeometry = (AliTOFGeometry*)in->Get("TOFgeometry");

}
//______________________________________________________________________________

AliTOFClusterFinder::~AliTOFClusterFinder()
{

  //
  // Destructor
  //

  if (fDigits)
    {
      fDigits->Delete();
      delete fDigits;
      fDigits=0;
    }
  if (fRecPoints)
    {
      fRecPoints->Delete();
      delete fRecPoints;
      fRecPoints=0;
    }

  delete fTOFGeometry;

}
//______________________________________________________________________________

void AliTOFClusterFinder::Digits2RecPoints(Int_t iEvent)
{
  //
  // Converts digits to recpoints for TOF
  //

  fRunLoader->GetEvent(iEvent);

  fTreeD = fTOFLoader->TreeD();
  if (fTreeD == 0x0)
    {
      AliFatal("AliTOFClusterFinder: Can not get TreeD");
    }

  TBranch *branch = fTreeD->GetBranch("TOF");
  if (!branch) { 
    AliError("can't get the branch with the TOF digits !");
    return;
  }

  TClonesArray dummy("AliTOFdigit",10000), *digits=&dummy;
  branch->SetAddress(&digits);

  ResetRecpoint();

  fTreeR = fTOFLoader->TreeR();
  if (fTreeR == 0x0)
    {
      fTOFLoader->MakeTree("R");
      fTreeR = fTOFLoader->TreeR();
    }

  Int_t bufsize = 32000;
  fTreeR->Branch("TOF", &fRecPoints, bufsize);

  fTreeD->GetEvent(0);
  Int_t nDigits = digits->GetEntriesFast();
  AliDebug(2,Form("Number of TOF digits: %d",nDigits));

  Int_t ii, jj;
  Int_t dig[5];
  Float_t g[3];
  Double_t h[5];
  for (ii=0; ii<nDigits; ii++) {
    AliTOFdigit *d = (AliTOFdigit*)digits->UncheckedAt(ii);
    dig[0]=d->GetSector();
    dig[1]=d->GetPlate();
    dig[2]=d->GetStrip();
    dig[3]=d->GetPadz();
    dig[4]=d->GetPadx();

    for (jj=0; jj<3; jj++) g[jj] = 0.;
    fTOFGeometry->GetPos(dig,g);

    h[0] = TMath::Sqrt(g[0]*g[0]+g[1]*g[1]);
    h[1] = TMath::ATan2(g[1],g[0]);
    h[2] = g[2];
    h[3] = d->GetTdc();
    h[4] = d->GetAdc();

    AliTOFcluster *tofCluster = new AliTOFcluster(h,d->GetTracks(),dig,ii);
    InsertCluster(tofCluster);

  }

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  FillRecPoint();

  fTreeR->Fill();
  ResetRecpoint();

  fTOFLoader = fRunLoader->GetLoader("TOFLoader");  
  fTOFLoader->WriteRecPoints("OVERWRITE");

}
//______________________________________________________________________________

void AliTOFClusterFinder::Digits2RecPoints(AliRawReader *rawReader,
					   TTree *clustersTree)
{
  //
  // Converts RAW data to recpoints for TOF
  //

  const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();

  ResetRecpoint();

  Int_t bufsize = 32000;
  clustersTree->Branch("TOF", &fRecPoints, bufsize);

  Int_t ii = 0;
  Int_t indexDDL = 0;

  Int_t detectorIndex[5];
  Float_t position[3];
  Double_t cylindricalPosition[5];

  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    rawReader->Select(5, indexDDL, indexDDL);

    while(tofInput.Next()) {

      detectorIndex[0] = tofInput.GetSector();
      detectorIndex[1] = tofInput.GetPlate();
      detectorIndex[2] = tofInput.GetStrip();
      detectorIndex[3] = tofInput.GetPadZ();
      detectorIndex[4] = tofInput.GetPadX();
      
      for (ii=0; ii<3; ii++) position[ii] =  0.;

      fTOFGeometry->GetPos(detectorIndex, position);

      cylindricalPosition[0] = TMath::Sqrt(position[0]*position[0] + position[1]*position[1]);
      cylindricalPosition[1] = TMath::ATan2(position[1], position[0]);
      cylindricalPosition[2] = position[2];
      cylindricalPosition[3] = tofInput.GetTofBin();
      cylindricalPosition[4] = tofInput.GetADCbin();

      AliTOFcluster *tofCluster = new AliTOFcluster(cylindricalPosition, detectorIndex);
      InsertCluster(tofCluster);

    } // while loop

  } // loop on DDL files

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  FillRecPoint();

  clustersTree->Fill();
  ResetRecpoint();

}
//______________________________________________________________________________

void AliTOFClusterFinder::Digits2RecPoints(Int_t iEvent, AliRawReader *rawReader)
{
  //
  // Converts RAW data to recpoints for TOF
  //

  const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();

  fRunLoader->GetEvent(iEvent);

  AliDebug(2,Form(" Event number %2i ", iEvent));

  fTreeR = fTOFLoader->TreeR();

  if (fTreeR == 0x0){
    fTOFLoader->MakeTree("R");
    fTreeR = fTOFLoader->TreeR();
  }

  Int_t bufsize = 32000;
  fTreeR->Branch("TOF", &fRecPoints, bufsize);

  Int_t ii = 0;
  Int_t indexDDL = 0;

  Int_t detectorIndex[5];
  Float_t position[3];
  Double_t cylindricalPosition[5];

  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    rawReader->Select(5, indexDDL, indexDDL);

    while(tofInput.Next()) {

      detectorIndex[0] = (Int_t)tofInput.GetSector();
      detectorIndex[1] = (Int_t)tofInput.GetPlate();
      detectorIndex[2] = (Int_t)tofInput.GetStrip();
      detectorIndex[3] = (Int_t)tofInput.GetPadZ();
      detectorIndex[4] = (Int_t)tofInput.GetPadX();

      for (ii=0; ii<3; ii++) position[ii] =  0.;

      fTOFGeometry->GetPos(detectorIndex, position);
      
      cylindricalPosition[0] = (Double_t)TMath::Sqrt(position[0]*position[0] + position[1]*position[1]);
      cylindricalPosition[1] = (Double_t)TMath::ATan2(position[1], position[0]);
      cylindricalPosition[2] = (Double_t)position[2];
      cylindricalPosition[3] = (Double_t)tofInput.GetTofBin();
      cylindricalPosition[4] = (Double_t)tofInput.GetADCbin();

      AliTOFcluster *tofCluster = new AliTOFcluster(cylindricalPosition, detectorIndex);
      InsertCluster(tofCluster);

    } // while loop

  } // DDL Loop

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  FillRecPoint();

  fTreeR->Fill();
  ResetRecpoint();

  fTOFLoader = fRunLoader->GetLoader("TOFLoader");
  fTOFLoader->WriteRecPoints("OVERWRITE");
  
}
//______________________________________________________________________________

void AliTOFClusterFinder::Raw2Digits(Int_t iEvent, AliRawReader *rawReader)
{
  //
  // Converts RAW data to MC digits for TOF
  //
  //             (temporary solution)
  //

  fRunLoader->GetEvent(iEvent);

  fTreeD = fTOFLoader->TreeD();
  if (fTreeD)
    {
    AliInfo("AliTOFClusterFinder: TreeD re-creation");
    fTreeD = 0x0;
    fTOFLoader->MakeTree("D");
    fTreeD = fTOFLoader->TreeD();
    }


  fTreeR = fTOFLoader->TreeD();
  if (fTreeD == 0x0)
    {
      fTOFLoader->MakeTree("D");
      fTreeD = fTOFLoader->TreeD();
    }

  TClonesArray dummy("AliTOFdigit",10000), *tofDigits=&dummy;
  Int_t bufsize = 32000;
  fTreeD->Branch("TOF", &tofDigits, bufsize);


  const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();

  fRunLoader->GetEvent(iEvent);

  AliDebug(2,Form(" Event number %2i ", iEvent));

  Int_t indexDDL = 0;

  Int_t detectorIndex[5];
  Float_t digit[2];

  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    rawReader->Select(5, indexDDL, indexDDL);

    while(tofInput.Next()) {

      detectorIndex[0] = tofInput.GetSector();
      detectorIndex[1] = tofInput.GetPlate();
      detectorIndex[2] = tofInput.GetStrip();
      detectorIndex[3] = tofInput.GetPadX();
      detectorIndex[4] = tofInput.GetPadZ();
      
      digit[0] = (Float_t)tofInput.GetTofBin();
      digit[1] = (Float_t)tofInput.GetADCbin();

      Int_t tracknum[3]={-1,-1,-1};

      TClonesArray &aDigits = *tofDigits;
      Int_t last=tofDigits->GetEntriesFast();
      new (aDigits[last]) AliTOFdigit(tracknum, detectorIndex, digit);

    } // while loop

  } // DDL Loop

  fTreeD->Fill();

  fTOFLoader = fRunLoader->GetLoader("TOFLoader");
  fTOFLoader->WriteDigits("OVERWRITE");
  
}
//______________________________________________________________________________

Int_t AliTOFClusterFinder::InsertCluster(AliTOFcluster *tofCluster) {
  //---------------------------------------------------------------------------//
  // This function adds a TOF cluster to the array of TOF clusters sorted in Z //
  //---------------------------------------------------------------------------//
  if (fNumberOfTofClusters==kTofMaxCluster) {
    AliError("Too many clusters !");
    return 1;
  }

  if (fNumberOfTofClusters==0) {
    fTofClusters[fNumberOfTofClusters++] = tofCluster;
    return 0;
  }

  Int_t ii = FindClusterIndex(tofCluster->GetZ());
  memmove(fTofClusters+ii+1 ,fTofClusters+ii,(fNumberOfTofClusters-ii)*sizeof(AliTOFcluster*));
  fTofClusters[ii] = tofCluster;
  fNumberOfTofClusters++;
  
  return 0;

}
//_________________________________________________________________________

Int_t AliTOFClusterFinder::FindClusterIndex(Double_t z) const {
  //--------------------------------------------------------------------
  // This function returns the index of the nearest cluster 
  //--------------------------------------------------------------------
  if (fNumberOfTofClusters==0) return 0;
  if (z <= fTofClusters[0]->GetZ()) return 0;
  if (z > fTofClusters[fNumberOfTofClusters-1]->GetZ()) return fNumberOfTofClusters;
  Int_t b = 0, e = fNumberOfTofClusters-1, m = (b+e)/2;
  for (; b<e; m=(b+e)/2) {
    if (z > fTofClusters[m]->GetZ()) b=m+1;
    else e=m;
  }

  return m;

}
//_________________________________________________________________________

void AliTOFClusterFinder::FillRecPoint()
{
  //
  // Copy the global array of AliTOFcluster, i.e. fTofClusters (sorted
  // in Z) in the global TClonesArray of AliTOFcluster,
  // i.e. fRecPoints.
  //

  Int_t ii, jj;

  Int_t detectorIndex[5];
  Double_t cylindricalPosition[5];
  Int_t trackLabels[3];
  Int_t digitIndex = -1;

  TClonesArray &lRecPoints = *fRecPoints;
  
  for (ii=0; ii<fNumberOfTofClusters; ii++) {

    digitIndex = fTofClusters[ii]->GetIndex();
    for(jj=0; jj<5; jj++) detectorIndex[jj] = fTofClusters[ii]->GetDetInd(jj);
    for(jj=0; jj<3; jj++) trackLabels[jj] = fTofClusters[ii]->GetLabel(jj);
    cylindricalPosition[0] = fTofClusters[ii]->GetR();
    cylindricalPosition[1] = fTofClusters[ii]->GetPhi();
    cylindricalPosition[2] = fTofClusters[ii]->GetZ();
    cylindricalPosition[3] = fTofClusters[ii]->GetTDC();
    cylindricalPosition[4] = fTofClusters[ii]->GetADC();

    new(lRecPoints[ii]) AliTOFcluster(cylindricalPosition, trackLabels, detectorIndex, digitIndex);

    //AliInfo(Form("%3i  %3i  %f %f %f %f %f  %2i %2i %2i %1i %2i",ii,digitIndex, cylindricalPosition[2],cylindricalPosition[0],cylindricalPosition[1],cylindricalPosition[3],cylindricalPosition[4],detectorIndex[0],detectorIndex[1],detectorIndex[2],detectorIndex[3],detectorIndex[4]));

  } // loop on clusters

}
//______________________________________________________________________________

void AliTOFClusterFinder::ResetRecpoint()
{
  //
  // Clear the list of reconstructed points
  //

  fNumberOfTofClusters = 0;
  if (fRecPoints) fRecPoints->Clear();

}
//______________________________________________________________________________

void AliTOFClusterFinder::Load()
{
  //
  // Load TOF.Digits.root and TOF.RecPoints.root files
  //

  fTOFLoader->LoadDigits("READ");
  fTOFLoader->LoadRecPoints("recreate");

}
//______________________________________________________________________________

void AliTOFClusterFinder::LoadClusters()
{
  //
  // Load TOF.RecPoints.root file
  //

  fTOFLoader->LoadRecPoints("recreate");

}
//______________________________________________________________________________

void AliTOFClusterFinder::UnLoad()
{
  //
  // Unload TOF.Digits.root and TOF.RecPoints.root files
  //

  fTOFLoader->UnloadDigits();
  fTOFLoader->UnloadRecPoints();

}
//______________________________________________________________________________

void AliTOFClusterFinder::UnLoadClusters()
{
  //
  // Unload TOF.RecPoints.root file
  //

  fTOFLoader->UnloadRecPoints();

}
//______________________________________________________________________________

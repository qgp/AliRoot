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
$Log$
Revision 1.21  2007/04/18 17:28:12  arcelli
Set the ToT bin width to the one actually used...

Revision 1.20  2007/03/09 09:57:23  arcelli
 Remove a forgotten include of Riostrem

Revision 1.19  2007/03/08 15:41:20  arcelli
set uncorrected times when filling RecPoints

Revision 1.18  2007/03/06 16:31:20  arcelli
Add Uncorrected TOF Time signal

Revision 1.17  2007/02/28 18:09:11  arcelli
Add protection against failed retrieval of the CDB cal object, now Reconstruction exits with AliFatal

Revision 1.16  2007/02/20 15:57:00  decaro
Raw data update: to read the TOF raw data defined in UNPACKED mode


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


#include "TClonesArray.h"
//#include "TFile.h"
#include "TTree.h"

#include "AliDAQ.h"
#include "AliLoader.h"
#include "AliLog.h"
#include "AliRawReader.h"
#include "AliRunLoader.h"

#include "AliTOFCal.h"
#include "AliTOFcalib.h"
#include "AliTOFChannel.h"
#include "AliTOFClusterFinder.h"
#include "AliTOFcluster.h"
#include "AliTOFdigit.h"
#include "AliTOFGeometry.h"
#include "AliTOFGeometryV5.h"
#include "AliTOFrawData.h"
#include "AliTOFRawStream.h"
#include "Riostream.h"

//extern TFile *gFile;

ClassImp(AliTOFClusterFinder)

AliTOFClusterFinder::AliTOFClusterFinder():
  fRunLoader(0),
  fTOFLoader(0),
  fTreeD(0),
  fTreeR(0),
  fTOFGeometry(new AliTOFGeometryV5()),
  fDigits(new TClonesArray("AliTOFdigit", 4000)),
  fRecPoints(new TClonesArray("AliTOFcluster", 4000)),
  fNumberOfTofClusters(0),
  fVerbose(0)
{
//
// Constructor
//

  AliInfo("V5 TOF Geometry is taken as the default");

}
//______________________________________________________________________________

AliTOFClusterFinder::AliTOFClusterFinder(AliRunLoader* runLoader):
  fRunLoader(runLoader),
  fTOFLoader(runLoader->GetLoader("TOFLoader")),
  fTreeD(0),
  fTreeR(0),
  fTOFGeometry(new AliTOFGeometryV5()),
  fDigits(new TClonesArray("AliTOFdigit", 4000)),
  fRecPoints(new TClonesArray("AliTOFcluster", 4000)),
  fNumberOfTofClusters(0),
  fVerbose(0)
{
//
// Constructor
//

//  runLoader->CdGAFile();
//  TFile *in=(TFile*)gFile;
//  in->cd();
//  fTOFGeometry = (AliTOFGeometry*)in->Get("TOFgeometry");

}

//------------------------------------------------------------------------
AliTOFClusterFinder::AliTOFClusterFinder(const AliTOFClusterFinder &source)
  :TObject(),
  fRunLoader(0),
  fTOFLoader(0),
  fTreeD(0),
  fTreeR(0),
  fTOFGeometry(new AliTOFGeometryV5()),
  fDigits(new TClonesArray("AliTOFdigit", 4000)),
  fRecPoints(new TClonesArray("AliTOFcluster", 4000)),
  fNumberOfTofClusters(0),
  fVerbose(0)
{
  // copy constructor
  this->fDigits=source.fDigits;
  this->fRecPoints=source.fRecPoints;
  this->fTOFGeometry=source.fTOFGeometry;

}

//------------------------------------------------------------------------
AliTOFClusterFinder& AliTOFClusterFinder::operator=(const AliTOFClusterFinder &source)
{
  // ass. op.
  this->fDigits=source.fDigits;
  this->fRecPoints=source.fRecPoints;
  this->fTOFGeometry=source.fTOFGeometry;
  this->fVerbose=source.fVerbose;
  return *this;

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

  TClonesArray *digits = new TClonesArray("AliTOFdigit",10000);
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
  Float_t tToT;
  Double_t tTdcND;
  for (ii=0; ii<nDigits; ii++) {
    AliTOFdigit *d = (AliTOFdigit*)digits->UncheckedAt(ii);
    dig[0]=d->GetSector();
    dig[1]=d->GetPlate();
    dig[2]=d->GetStrip();
    dig[3]=d->GetPadz();
    dig[4]=d->GetPadx();

    //AliInfo(Form(" %2i  %1i  %2i  %1i  %2i ",dig[0],dig[1],dig[2],dig[3],dig[4]));

    for (jj=0; jj<3; jj++) g[jj] = 0.;
    fTOFGeometry->GetPos(dig,g);

    h[0] = TMath::Sqrt(g[0]*g[0]+g[1]*g[1]);
    h[1] = TMath::ATan2(g[1],g[0]);
    h[2] = g[2];
    h[3] = d->GetTdc();
    h[4] = d->GetAdc();
    tToT = d->GetToT();
    tTdcND = d->GetTdcND();

    AliTOFcluster *tofCluster = new AliTOFcluster(h,d->GetTracks(),dig,ii,tToT, tTdcND);
    tofCluster->SetTDCRAW(d->GetTdc());
    InsertCluster(tofCluster);

  }

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  CalibrateRecPoint();
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

  //const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();
  const Int_t kDDL = AliDAQ::NumberOfDdls("TOF");

  ResetRecpoint();

  Int_t bufsize = 32000;
  clustersTree->Branch("TOF", &fRecPoints, bufsize);

  TClonesArray * clonesRawData;

  Int_t ii = 0;
  Int_t dummy = -1;

  Int_t detectorIndex[5];
  Float_t position[3];
  Double_t cylindricalPosition[5];
  Float_t tToT;
  Double_t tTdcND;

  ofstream ftxt;
  if (fVerbose==2) ftxt.open("TOFdigitsRead.txt",ios::app);

  Int_t indexDDL = 0;
  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    tofInput.LoadRawData(indexDDL);

    clonesRawData = (TClonesArray*)tofInput.GetRawData();

    for (Int_t iRawData = 0; iRawData<clonesRawData->GetEntriesFast(); iRawData++) {

      AliTOFrawData *tofRawDatum = (AliTOFrawData*)clonesRawData->UncheckedAt(iRawData);

      if (tofRawDatum->GetTOT()==-1 || tofRawDatum->GetTOF()==-1) continue;

      if (fVerbose==2) {
	if (indexDDL<10) ftxt << "  " << indexDDL;
	else         ftxt << " " << indexDDL;
	if (tofRawDatum->GetTRM()<10) ftxt << "  " << tofRawDatum->GetTRM();
	else         ftxt << " " << tofRawDatum->GetTRM();
	ftxt << "  " << tofRawDatum->GetTRMchain();
	if (tofRawDatum->GetTDC()<10) ftxt << "  " << tofRawDatum->GetTDC();
	else         ftxt << " " << tofRawDatum->GetTDC();
	ftxt << "  " << tofRawDatum->GetTDCchannel();
      }

      tofInput.EquipmentId2VolumeId(indexDDL, tofRawDatum->GetTRM(), tofRawDatum->GetTRMchain(),
				    tofRawDatum->GetTDC(), tofRawDatum->GetTDCchannel(), detectorIndex);
      dummy = detectorIndex[3];
      detectorIndex[3] = detectorIndex[4];
      detectorIndex[4] = dummy;

      if (fVerbose==2) {
	if (detectorIndex[0]<10) ftxt  << "  ->  " << detectorIndex[0];
	else              ftxt  << "  -> " << detectorIndex[0];
	ftxt << "  " << detectorIndex[1];
	if (detectorIndex[2]<10) ftxt << "  " << detectorIndex[2];
	else              ftxt << " " << detectorIndex[2];
	ftxt << "  " << detectorIndex[3];
	if (detectorIndex[4]<10) ftxt << "  " << detectorIndex[4];
	else              ftxt << " " << detectorIndex[4];
      }

      for (ii=0; ii<3; ii++) position[ii] =  0.;
      fTOFGeometry->GetPos(detectorIndex, position);

      cylindricalPosition[0] = TMath::Sqrt(position[0]*position[0] + position[1]*position[1]);
      cylindricalPosition[1] = TMath::ATan2(position[1], position[0]);
      cylindricalPosition[2] = position[2];
      cylindricalPosition[3] = tofRawDatum->GetTOF();
      cylindricalPosition[4] = tofRawDatum->GetTOT();
      tToT = tofRawDatum->GetTOT();
      tTdcND = -1.;
      AliTOFcluster *tofCluster = new AliTOFcluster(cylindricalPosition, detectorIndex);
      tofCluster->SetToT(tToT);
      tofCluster->SetTDCND(tTdcND);
      tofCluster->SetTDCRAW(tofRawDatum->GetTOF());
      InsertCluster(tofCluster);

      if (fVerbose==2) {
	if (cylindricalPosition[4]<10)                        ftxt << "        " << cylindricalPosition[4];
	else if (cylindricalPosition[4]>=10 && cylindricalPosition[4]<100) ftxt << "       " << cylindricalPosition[4];
	else                                     ftxt << "      " << cylindricalPosition[4];
	if (cylindricalPosition[3]<10)                             ftxt << "      " << cylindricalPosition[3] << endl;
	else if (cylindricalPosition[3]>=10 && cylindricalPosition[3]<100)   ftxt << "     " << cylindricalPosition[3] << endl;
	else if (cylindricalPosition[3]>=100 && cylindricalPosition[3]<1000) ftxt << "    " << cylindricalPosition[3] << endl;
	else                                             ftxt << "   " << cylindricalPosition[3] << endl;
      }

    } // closed loop on TOF raw data per current DDL file

    clonesRawData->Clear();

  } // closed loop on DDL index

  /*
  Int_t indexDDL = 0;
  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    rawReader->Select("TOF", indexDDL, indexDDL);

    while(tofInput.Next()) {

      for (ii=0; ii<5; ii++) detectorIndex[ii] = -1;

      detectorIndex[0] = tofInput.GetSector();
      detectorIndex[1] = tofInput.GetPlate();
      detectorIndex[2] = tofInput.GetStrip();
      detectorIndex[3] = tofInput.GetPadZ();
      detectorIndex[4] = tofInput.GetPadX();
      
      //AliInfo(Form("  %2i  %1i  %2i  %1i  %2i ",detectorIndex[0],detectorIndex[1],detectorIndex[2],detectorIndex[3],detectorIndex[4]));

      if (detectorIndex[0]==-1 ||
	  detectorIndex[1]==-1 ||
	  detectorIndex[2]==-1 ||
	  detectorIndex[3]==-1 ||
	  detectorIndex[4]==-1) continue;

      for (ii=0; ii<3; ii++) position[ii] =  0.;

      fTOFGeometry->GetPos(detectorIndex, position);

      cylindricalPosition[0] = TMath::Sqrt(position[0]*position[0] + position[1]*position[1]);
      cylindricalPosition[1] = TMath::ATan2(position[1], position[0]);
      cylindricalPosition[2] = position[2];
      cylindricalPosition[3] = tofInput.GetTofBin();
      cylindricalPosition[4] = tofInput.GetToTbin();
      tToT = tofInput.GetToTbin();
      tTdcND = -1.;
      AliTOFcluster *tofCluster = new AliTOFcluster(cylindricalPosition, detectorIndex);
      tofCluster->SetToT(tToT);
      tofCluster->SetTDCND(tTdcND);
      InsertCluster(tofCluster);

    } // while loop

  } // loop on DDL files
  */

  if (fVerbose==2) ftxt.close();

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  CalibrateRecPoint();
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

  //const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();
  const Int_t kDDL = AliDAQ::NumberOfDdls("TOF");

  fRunLoader->GetEvent(iEvent);

  AliDebug(2,Form(" Event number %2i ", iEvent));

  fTreeR = fTOFLoader->TreeR();

  if (fTreeR == 0x0){
    fTOFLoader->MakeTree("R");
    fTreeR = fTOFLoader->TreeR();
  }

  Int_t bufsize = 32000;
  fTreeR->Branch("TOF", &fRecPoints, bufsize);

  TClonesArray * clonesRawData;

  Int_t ii = 0;
  Int_t dummy = -1;

  Int_t detectorIndex[5] = {-1, -1, -1, -1, -1};
  Float_t position[3];
  Double_t cylindricalPosition[5];
  Float_t tToT;
  Double_t tTdcND;

  ofstream ftxt;
  if (fVerbose==2) ftxt.open("TOFdigitsRead.txt",ios::app);

  Int_t indexDDL = 0;
  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    tofInput.LoadRawData(indexDDL);

    clonesRawData = (TClonesArray*)tofInput.GetRawData();

    for (Int_t iRawData = 0; iRawData<clonesRawData->GetEntriesFast(); iRawData++) {

      AliTOFrawData *tofRawDatum = (AliTOFrawData*)clonesRawData->UncheckedAt(iRawData);

      if (tofRawDatum->GetTOT()==-1 || tofRawDatum->GetTOF()==-1) continue;

      if (fVerbose==2) {
	if (indexDDL<10) ftxt << "  " << indexDDL;
	else         ftxt << " " << indexDDL;
	if (tofRawDatum->GetTRM()<10) ftxt << "  " << tofRawDatum->GetTRM();
	else         ftxt << " " << tofRawDatum->GetTRM();
	ftxt << "  " << tofRawDatum->GetTRMchain();
	if (tofRawDatum->GetTDC()<10) ftxt << "  " << tofRawDatum->GetTDC();
	else         ftxt << " " << tofRawDatum->GetTDC();
	ftxt << "  " << tofRawDatum->GetTDCchannel();
      }

      tofInput.EquipmentId2VolumeId(indexDDL, tofRawDatum->GetTRM(), tofRawDatum->GetTRMchain(),
				    tofRawDatum->GetTDC(), tofRawDatum->GetTDCchannel(), detectorIndex);
      dummy = detectorIndex[3];
      detectorIndex[3] = detectorIndex[4];
      detectorIndex[4] = dummy;

      if (fVerbose==2) {
	if (detectorIndex[0]<10) ftxt  << "  ->  " << detectorIndex[0];
	else              ftxt  << "  -> " << detectorIndex[0];
	ftxt << "  " << detectorIndex[1];
	if (detectorIndex[2]<10) ftxt << "  " << detectorIndex[2];
	else              ftxt << " " << detectorIndex[2];
	ftxt << "  " << detectorIndex[3];
	if (detectorIndex[4]<10) ftxt << "  " << detectorIndex[4];
	else              ftxt << " " << detectorIndex[4];
      }

      for (ii=0; ii<3; ii++) position[ii] =  0.;
      fTOFGeometry->GetPos(detectorIndex, position);

      cylindricalPosition[0] = TMath::Sqrt(position[0]*position[0] + position[1]*position[1]);
      cylindricalPosition[1] = TMath::ATan2(position[1], position[0]);
      cylindricalPosition[2] = position[2];
      cylindricalPosition[3] = tofRawDatum->GetTOF();
      cylindricalPosition[4] = tofRawDatum->GetTOT();
      tToT = tofRawDatum->GetTOT();
      tTdcND = -1.;
      AliTOFcluster *tofCluster = new AliTOFcluster(cylindricalPosition, detectorIndex);
      tofCluster->SetToT(tToT);
      tofCluster->SetTDCND(tTdcND);
      tofCluster->SetTDCRAW(tofRawDatum->GetTOF());
      InsertCluster(tofCluster);

      if (fVerbose==2) {
	if (cylindricalPosition[4]<10)                        ftxt << "        " << cylindricalPosition[4];
	else if (cylindricalPosition[4]>=10 && cylindricalPosition[4]<100) ftxt << "       " << cylindricalPosition[4];
	else                                     ftxt << "      " << cylindricalPosition[4];
	if (cylindricalPosition[3]<10)                             ftxt << "      " << cylindricalPosition[3] << endl;
	else if (cylindricalPosition[3]>=10 && cylindricalPosition[3]<100)   ftxt << "     " << cylindricalPosition[3] << endl;
	else if (cylindricalPosition[3]>=100 && cylindricalPosition[3]<1000) ftxt << "    " << cylindricalPosition[3] << endl;
	else                                             ftxt << "   " << cylindricalPosition[3] << endl;
      }

    } // closed loop on TOF raw data per current DDL file

    clonesRawData->Clear();

  } // closed loop on DDL index

  if (fVerbose==2) ftxt.close();

  AliInfo(Form("Number of found clusters: %i", fNumberOfTofClusters));

  CalibrateRecPoint();
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

  //const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();
  const Int_t kDDL = fTOFGeometry->NDDL()*fTOFGeometry->NSectors();

  fRunLoader->GetEvent(iEvent);

  fTreeD = fTOFLoader->TreeD();
  if (fTreeD)
    {
    AliInfo("TreeD re-creation");
    fTreeD = 0x0;
    fTOFLoader->MakeTree("D");
    fTreeD = fTOFLoader->TreeD();
    }

  TClonesArray *tofDigits = new TClonesArray("AliTOFdigit",10000);
  Int_t bufsize = 32000;
  fTreeD->Branch("TOF", &tofDigits, bufsize);

  fRunLoader->GetEvent(iEvent);

  AliDebug(2,Form(" Event number %2i ", iEvent));

  TClonesArray * clonesRawData;

  Int_t dummy = -1;

  Int_t detectorIndex[5];
  Float_t digit[4];

  Int_t indexDDL = 0;
  for (indexDDL = 0; indexDDL < kDDL; indexDDL++) {

    rawReader->Reset();
    AliTOFRawStream tofInput(rawReader);
    tofInput.LoadRawData(indexDDL);

    clonesRawData = (TClonesArray*)tofInput.GetRawData();

    for (Int_t iRawData = 0; iRawData<clonesRawData->GetEntriesFast(); iRawData++) {

      AliTOFrawData *tofRawDatum = (AliTOFrawData*)clonesRawData->UncheckedAt(iRawData);

      if (!tofRawDatum->GetTOT() || !tofRawDatum->GetTOF()) continue;

      tofInput.EquipmentId2VolumeId(indexDDL, tofRawDatum->GetTRM(), tofRawDatum->GetTRMchain(),
				    tofRawDatum->GetTDC(), tofRawDatum->GetTDCchannel(), detectorIndex);
      dummy = detectorIndex[3];
      detectorIndex[3] = detectorIndex[4];
      detectorIndex[4] = dummy;

      digit[0] = (Float_t)tofInput.GetTofBin();
      digit[1] = (Float_t)tofInput.GetToTbin();
      digit[2] = (Float_t)tofInput.GetToTbin();
      digit[3] = -1.;

      Int_t tracknum[3]={-1,-1,-1};

      TClonesArray &aDigits = *tofDigits;
      Int_t last=tofDigits->GetEntriesFast();
      new (aDigits[last]) AliTOFdigit(tracknum, detectorIndex, digit);

    } // while loop

    clonesRawData->Clear();

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
  Float_t tToT=0.;
  Double_t tTdcND=0.;
  Double_t tTdcRAW=0.;
  Bool_t cStatus = kTRUE;

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
    tToT = fTofClusters[ii]->GetToT();
    tTdcND = fTofClusters[ii]->GetTDCND();
    cStatus=fTofClusters[ii]->GetStatus();
    tTdcRAW=fTofClusters[ii]->GetTDCRAW();
    new(lRecPoints[ii]) AliTOFcluster(cylindricalPosition, trackLabels, detectorIndex, digitIndex, tToT, tTdcND, tTdcRAW,cStatus);

    //AliInfo(Form("%3i  %3i  %f %f %f %f %f  %2i %2i %2i %1i %2i",ii,digitIndex, cylindricalPosition[2],cylindricalPosition[0],cylindricalPosition[1],cylindricalPosition[3],cylindricalPosition[4],detectorIndex[0],detectorIndex[1],detectorIndex[2],detectorIndex[3],detectorIndex[4]));

  } // loop on clusters

}

//_________________________________________________________________________
void AliTOFClusterFinder::CalibrateRecPoint()
{
  //
  // Copy the global array of AliTOFcluster, i.e. fTofClusters (sorted
  // in Z) in the global TClonesArray of AliTOFcluster,
  // i.e. fRecPoints.
  //

  Int_t ii, jj;

  Int_t detectorIndex[5];
  Int_t digitIndex = -1;
  Float_t tToT;
  Float_t tdcCorr;
  AliInfo(" Calibrating TOF Clusters: ")
  AliTOFcalib *calib = new AliTOFcalib(fTOFGeometry);
  // calib->ReadParFromCDB("TOF/Calib",0); // original
  // Use AliCDBManager's run number
 if(!calib->ReadParFromCDB("TOF/Calib",-1)) {AliFatal("Exiting, no CDB object found!!!");exit(0);}  
  
  AliTOFCal *calTOFArray = calib->GetTOFCalArray();  

  for (ii=0; ii<fNumberOfTofClusters; ii++) {
    digitIndex = fTofClusters[ii]->GetIndex();
    for(jj=0; jj<5; jj++) detectorIndex[jj] = fTofClusters[ii]->GetDetInd(jj);

    Int_t index = calib->GetIndex(detectorIndex);
     
    AliTOFChannel * calChannel = calTOFArray->GetChannel(index);

    // Get channel status 
    Bool_t status=calChannel->GetStatus();
    if(status)fTofClusters[ii]->SetStatus(!status); //odd convention, to avoid conflict with calibration objects currently in the db (temporary solution).

    // Get Rough channel online equalization 
    Float_t roughDelay=calChannel->GetDelay();
    AliDebug(2,Form(" channel delay = %f", roughDelay));
    // Get Refined channel offline calibration parameters
    Float_t par[6];
    for (Int_t j = 0; j<6; j++){
      par[j]=calChannel->GetSlewPar(j);
    }
    tToT = fTofClusters[ii]->GetToT()*AliTOFGeometry::ToTBinWidth()*1.E-3;
    Float_t timeCorr=par[0]+par[1]*tToT+par[2]*tToT*tToT+par[3]*tToT*tToT*tToT+par[4]*tToT*tToT*tToT*tToT+par[5]*tToT*tToT*tToT*tToT*tToT+roughDelay;
    AliDebug(2,Form(" time correction (ns) = %f", timeCorr));
    AliDebug(2,Form(" channel time, uncorr (ns)= %f",fTofClusters[ii]->GetTDC()*AliTOFGeometry::TdcBinWidth()*1.E-3 ));
    tdcCorr=(fTofClusters[ii]->GetTDC()*AliTOFGeometry::TdcBinWidth()+32)*1.E-3-timeCorr;
    tdcCorr=(tdcCorr*1E3-32)/AliTOFGeometry::TdcBinWidth();
    fTofClusters[ii]->SetTDC(tdcCorr);
    AliDebug(2,Form(" channel time, corr (ns)= %f",fTofClusters[ii]->GetTDC()*AliTOFGeometry::TdcBinWidth()*1.E-3 ));

  } // loop on clusters

  delete calib;
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

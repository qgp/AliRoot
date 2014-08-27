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
// TRD cluster finder                                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TClonesArray.h>
#include <TObjArray.h>

#include "AliRunLoader.h"
#include "AliLoader.h"
#include "AliTreeLoader.h"
#include "AliAlignObj.h"

#include "AliTRDclusterizer.h"
#include "AliTRDcluster.h"
#include "AliTRDReconstructor.h"
#include "AliTRDgeometry.h"
#include "AliTRDarrayDictionary.h"
#include "AliTRDarrayADC.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDdigitsParam.h"
#include "AliTRDrawData.h"
#include "AliTRDcalibDB.h"
#include "AliTRDtransform.h"
#include "AliTRDSignalIndex.h"
#include "AliTRDrawStream.h"
#include "AliTRDfeeParam.h"
#include "AliTRDtrackletWord.h"
#include "AliTRDtrackletMCM.h"
#include "AliTRDtrackGTU.h"
#include "AliESDTrdTrack.h"

#include "TTreeStream.h"

#include "Cal/AliTRDCalROC.h"
#include "Cal/AliTRDCalDet.h"
#include "Cal/AliTRDCalSingleChamberStatus.h"
#include "Cal/AliTRDCalOnlineGainTableROC.h"

ClassImp(AliTRDclusterizer)

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer(const AliTRDReconstructor *const rec)
  :TNamed()
  ,fReconstructor(rec)  
  ,fRunLoader(NULL)
  ,fClusterTree(NULL)
  ,fRecPoints(NULL)
  ,fTracklets(NULL)
  ,fTracks(NULL)
  ,fDigitsManager(new AliTRDdigitsManager())
  ,fRawVersion(2)
  ,fTransform(new AliTRDtransform(0))
  ,fDigits(NULL)
  ,fDigitsRaw(NULL)
  ,fIndexes(NULL)
  ,fMaxThresh(0)
  ,fMaxThreshTest(0)
  ,fSigThresh(0)
  ,fMinMaxCutSigma(0)
  ,fMinLeftRightCutSigma(0)
  ,fLayer(0)
  ,fDet(0)
  ,fVolid(0)
  ,fColMax(0)
  ,fTimeTotal(0)
  ,fCalGainFactorROC(NULL)
  ,fCalGainFactorDetValue(0)
  ,fCalNoiseROC(NULL)
  ,fCalNoiseDetValue(0)
  ,fCalPadStatusROC(NULL)
  ,fCalOnlGainROC(NULL)
  ,fClusterROC(0)
  ,firstClusterROC(0)
  ,fNoOfClusters(0)
  ,fBaseline(0)
  ,fRawStream(NULL)
  ,fTrgFlags()
{
  //
  // AliTRDclusterizer default constructor
  //

  SetBit(kLabels, kTRUE);
  SetBit(knewDM, kFALSE);

  fRawVersion = AliTRDfeeParam::Instance()->GetRAWversion();

  // Initialize debug stream
  if(fReconstructor){
    if(fReconstructor->GetRecoParam()->GetStreamLevel(AliTRDrecoParam::kClusterizer) > 1){
      TDirectory *savedir = gDirectory; 
      //fgGetDebugStream    = new TTreeSRedirector("TRD.ClusterizerDebug.root");
      savedir->cd();
    }
  }

}

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer(const Text_t *name
                                   , const Text_t *title
                                   , const AliTRDReconstructor *const rec)
  :TNamed(name,title)
  ,fReconstructor(rec)
  ,fRunLoader(NULL)
  ,fClusterTree(NULL)
  ,fRecPoints(NULL)
  ,fTracklets(NULL)
  ,fTracks(NULL)
  ,fDigitsManager(new AliTRDdigitsManager())
  ,fRawVersion(2)
  ,fTransform(new AliTRDtransform(0))
  ,fDigits(NULL)
  ,fDigitsRaw(NULL)
  ,fIndexes(NULL)
  ,fMaxThresh(0)
  ,fMaxThreshTest(0)
  ,fSigThresh(0)
  ,fMinMaxCutSigma(0)
  ,fMinLeftRightCutSigma(0)
  ,fLayer(0)
  ,fDet(0)
  ,fVolid(0)
  ,fColMax(0)
  ,fTimeTotal(0)
  ,fCalGainFactorROC(NULL)
  ,fCalGainFactorDetValue(0)
  ,fCalNoiseROC(NULL)
  ,fCalNoiseDetValue(0)
  ,fCalPadStatusROC(NULL)
  ,fCalOnlGainROC(NULL)
  ,fClusterROC(0)
  ,firstClusterROC(0)
  ,fNoOfClusters(0)
  ,fBaseline(0)
  ,fRawStream(NULL)
  ,fTrgFlags()
{
  //
  // AliTRDclusterizer constructor
  //

  SetBit(kLabels, kTRUE);
  SetBit(knewDM, kFALSE);

  fDigitsManager->CreateArrays();

  fRawVersion = AliTRDfeeParam::Instance()->GetRAWversion();

  //FillLUT();

}

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer(const AliTRDclusterizer &c)
  :TNamed(c)
  ,fReconstructor(c.fReconstructor)
  ,fRunLoader(NULL)
  ,fClusterTree(NULL)
  ,fRecPoints(NULL)
  ,fTracklets(NULL)
  ,fTracks(NULL)
  ,fDigitsManager(NULL)
  ,fRawVersion(2)
  ,fTransform(NULL)
  ,fDigits(NULL)
  ,fDigitsRaw(NULL)
  ,fIndexes(NULL)
  ,fMaxThresh(0)
  ,fMaxThreshTest(0)
  ,fSigThresh(0)
  ,fMinMaxCutSigma(0)
  ,fMinLeftRightCutSigma(0)
  ,fLayer(0)
  ,fDet(0)
  ,fVolid(0)
  ,fColMax(0)
  ,fTimeTotal(0)
  ,fCalGainFactorROC(NULL)
  ,fCalGainFactorDetValue(0)
  ,fCalNoiseROC(NULL)
  ,fCalNoiseDetValue(0)
  ,fCalPadStatusROC(NULL)
  ,fCalOnlGainROC(NULL)
  ,fClusterROC(0)
  ,firstClusterROC(0)
  ,fNoOfClusters(0)
  ,fBaseline(0)
  ,fRawStream(NULL)
  ,fTrgFlags()
{
  //
  // AliTRDclusterizer copy constructor
  //

  SetBit(kLabels, kTRUE);
  SetBit(knewDM, kFALSE);

  //FillLUT();

}

//_____________________________________________________________________________
AliTRDclusterizer::~AliTRDclusterizer()
{
  //
  // AliTRDclusterizer destructor
  //

  if (fDigitsManager) {
    delete fDigitsManager;
    fDigitsManager = NULL;
  }

  if (fDigitsRaw) {
    delete fDigitsRaw;
    fDigitsRaw = NULL;
  }

  if (fTransform){
    delete fTransform;
    fTransform = NULL;
  }

  if (fRawStream){
    delete fRawStream;
    fRawStream = NULL;
  }
}

//_____________________________________________________________________________
AliTRDclusterizer &AliTRDclusterizer::operator=(const AliTRDclusterizer &c)
{
  //
  // Assignment operator
  //

  if (this != &c) 
    {
      ((AliTRDclusterizer &) c).Copy(*this);
    }

  return *this;

}

//_____________________________________________________________________________
void AliTRDclusterizer::Copy(TObject &c) const
{
  //
  // Copy function
  //

  ((AliTRDclusterizer &) c).fClusterTree   = NULL;
  ((AliTRDclusterizer &) c).fRecPoints     = NULL;  
  ((AliTRDclusterizer &) c).fDigitsManager = NULL;
  ((AliTRDclusterizer &) c).fRawVersion    = fRawVersion;
  ((AliTRDclusterizer &) c).fTransform     = NULL;
  ((AliTRDclusterizer &) c).fDigits        = NULL;
  ((AliTRDclusterizer &) c).fDigitsRaw     = NULL;
  ((AliTRDclusterizer &) c).fIndexes       = NULL;
  ((AliTRDclusterizer &) c).fMaxThresh     = 0;
  ((AliTRDclusterizer &) c).fMaxThreshTest = 0;
  ((AliTRDclusterizer &) c).fSigThresh     = 0;
  ((AliTRDclusterizer &) c).fMinMaxCutSigma= 0;
  ((AliTRDclusterizer &) c).fMinLeftRightCutSigma = 0;
  ((AliTRDclusterizer &) c).fLayer         = 0;
  ((AliTRDclusterizer &) c).fDet           = 0;
  ((AliTRDclusterizer &) c).fVolid         = 0;
  ((AliTRDclusterizer &) c).fColMax        = 0;
  ((AliTRDclusterizer &) c).fTimeTotal     = 0;
  ((AliTRDclusterizer &) c).fCalGainFactorROC = NULL;
  ((AliTRDclusterizer &) c).fCalGainFactorDetValue = 0;
  ((AliTRDclusterizer &) c).fCalNoiseROC   = NULL;
  ((AliTRDclusterizer &) c).fCalNoiseDetValue = 0;
  ((AliTRDclusterizer &) c).fCalPadStatusROC = NULL;
  ((AliTRDclusterizer &) c).fClusterROC    = 0;
  ((AliTRDclusterizer &) c).firstClusterROC= 0;
  ((AliTRDclusterizer &) c).fNoOfClusters  = 0;
  ((AliTRDclusterizer &) c).fBaseline      = 0;
  ((AliTRDclusterizer &) c).fRawStream     = NULL;
  
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::Open(const Char_t *name, Int_t nEvent)
{
  //
  // Opens the AliROOT file. Output and input are in the same file
  //

  TString evfoldname = AliConfig::GetDefaultEventFolderName();
  fRunLoader         = AliRunLoader::GetRunLoader(evfoldname);

  if (!fRunLoader) {
    fRunLoader = AliRunLoader::Open(name);
  }

  if (!fRunLoader) {
    AliError(Form("Can not open session for file %s.",name));
    return kFALSE;
  }

  OpenInput(nEvent);
  OpenOutput();

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::OpenOutput()
{
  //
  // Open the output file
  //

  if (!fReconstructor->IsWritingClusters()) return kTRUE;

  TObjArray *ioArray = 0x0; 

  AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
  loader->MakeTree("R");

  fClusterTree = loader->TreeR();
  fClusterTree->Branch("TRDcluster", "TObjArray", &ioArray, 32000, 0);

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::OpenOutput(TTree *const clusterTree)
{
  //
  // Connect the output tree
  //

  // clusters writing
  if (fReconstructor->IsWritingClusters()){
    TObjArray *ioArray = 0x0;
    fClusterTree = clusterTree;
    fClusterTree->Branch("TRDcluster", "TObjArray", &ioArray, 32000, 0);
  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::OpenInput(Int_t nEvent)
{
  //
  // Opens a ROOT-file with TRD-hits and reads in the digits-tree
  //

  // Import the Trees for the event nEvent in the file
  fRunLoader->GetEvent(nEvent);
  
  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::WriteClusters(Int_t det)
{
  //
  // Fills TRDcluster branch in the tree with the clusters 
  // found in detector = det. For det=-1 writes the tree. 
  //

  if ((det <                      -1) || 
      (det >= AliTRDgeometry::Ndet())) {
    AliError(Form("Unexpected detector index %d.\n",det));
    return kFALSE;
  }
  Int_t nRecPoints = RecPoints()->GetEntriesFast();
  if(!nRecPoints) return kTRUE;

  TObjArray *ioArray = new TObjArray(400);
  TBranch *branch = fClusterTree->GetBranch("TRDcluster");
  if (!branch) {
    fClusterTree->Branch("TRDcluster","TObjArray",&ioArray,32000,0);
  } else branch->SetAddress(&ioArray);
  
  AliTRDcluster *c(NULL);
  if(det >= 0){
    for (Int_t i = 0; i < nRecPoints; i++) {
      if(!(c = (AliTRDcluster *) RecPoints()->UncheckedAt(i))) continue;
      if(det != c->GetDetector()) continue;
      ioArray->AddLast(c);
    }
    fClusterTree->Fill();
    ioArray->Clear();
  } else {
    if(!(c = (AliTRDcluster*)RecPoints()->UncheckedAt(0))){
      AliError("Missing first cluster.");
      delete ioArray;
      return kFALSE;  
    }
    Int_t detOld(c->GetDetector()), nw(0);
    ioArray->AddLast(c);
    for (Int_t i(1); i<nRecPoints; i++) {
      if(!(c = (AliTRDcluster *) RecPoints()->UncheckedAt(i))) continue;
      if(c->GetDetector() != detOld){
        nw += ioArray->GetEntriesFast();
        // fill & clear previously detector set of clusters
        fClusterTree->Fill();
        ioArray->Clear();
        detOld = c->GetDetector();
      } 
      ioArray->AddLast(c);
    }
    if(ioArray->GetEntriesFast()){
      nw += ioArray->GetEntriesFast();
      // fill & clear last detector set of clusters (if any)
      fClusterTree->Fill();
      ioArray->Clear();
    }
    AliDebug(2, Form("Clusters FOUND[%d] WRITTEN[%d] STATUS[%s]", nRecPoints, nw, nw==nRecPoints?"OK":"FAILED"));
    if(nw!=nRecPoints) AliWarning(Form("Clusters FOUND[%d] WRITTEN[%d]", nRecPoints, nw));
  }
  delete ioArray;

  return kTRUE;  
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::ReadDigits()
{
  //
  // Reads the digits arrays from the input aliroot file
  //

  if (!fRunLoader) {
    AliError("No run loader available");
    return kFALSE;
  }

  AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
  if (!loader->TreeD()) {
    loader->LoadDigits();
  }

  // Read in the digit arrays
  return (fDigitsManager->ReadDigits(loader->TreeD()));

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::ReadDigits(TTree *digitsTree)
{
  //
  // Reads the digits arrays from the input tree
  //

  // Read in the digit arrays
  return (fDigitsManager->ReadDigits(digitsTree));

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::ReadDigits(AliRawReader *rawReader)
{
  //
  // Reads the digits arrays from the ddl file
  //

  AliTRDrawData raw;
  fDigitsManager = raw.Raw2Digits(rawReader);

  return kTRUE;

}

Bool_t AliTRDclusterizer::ReadTracklets()
{
  //
  // Reads simulated tracklets from the input aliroot file
  //

  AliRunLoader *runLoader = AliRunLoader::Instance();
  if (!runLoader) {
    AliError("No run loader available");
    return kFALSE;
  }

  AliLoader* loader = runLoader->GetLoader("TRDLoader");

  AliDataLoader *trackletLoader = loader->GetDataLoader("tracklets");
  if (!trackletLoader) {
      return kFALSE;
  }
  trackletLoader->Load();

  Bool_t loaded = kFALSE;
  // look for simulated tracklets
  TTree *trackletTree = trackletLoader->Tree();

  if (trackletTree) {
    TBranch *trklbranch = trackletTree->GetBranch("mcmtrklbranch");
    TClonesArray *trklArray = TrackletsArray("AliTRDtrackletMCM");
    if (trklbranch && trklArray) {
      AliTRDtrackletMCM *trkl = 0x0;
      trklbranch->SetAddress(&trkl);
      Int_t nTracklets = trklbranch->GetEntries();
      for (Int_t iTracklet = 0; iTracklet < nTracklets; iTracklet++) {
        trklbranch->GetEntry(iTracklet);
        new ((*trklArray)[trklArray->GetEntries()]) AliTRDtrackletMCM(*trkl);
      }
      loaded = kTRUE;
    }
  }
  else {
    // if no simulated tracklets found, look for raw tracklets
    AliTreeLoader *treeLoader = (AliTreeLoader*) trackletLoader->GetBaseLoader("tracklets-raw");
    trackletTree = treeLoader ? treeLoader->Load(), treeLoader->Tree() : 0x0;

    if (trackletTree) {
      TClonesArray *trklArray = TrackletsArray("AliTRDtrackletWord");

      Int_t hc;
      TClonesArray *ar = 0x0;
      trackletTree->SetBranchAddress("hc", &hc);
      trackletTree->SetBranchAddress("trkl", &ar);

      Int_t nEntries = trackletTree->GetEntries();
      for (Int_t iEntry = 0; iEntry < nEntries; iEntry++) {
	trackletTree->GetEntry(iEntry);
	Int_t nTracklets = ar->GetEntriesFast();
	AliDebug(2, Form("%i tracklets in HC %i", nTracklets, hc));
	for (Int_t iTracklet = 0; iTracklet < nTracklets; iTracklet++) {
	  AliTRDtrackletWord *trklWord = (AliTRDtrackletWord*) (*ar)[iTracklet];
	  new ((*trklArray)[trklArray->GetEntries()]) AliTRDtrackletWord(trklWord->GetTrackletWord(), hc);
	}
      }
      loaded = kTRUE;
    }
  }

  trackletLoader->UnloadAll();
  trackletLoader->CloseFile();

  return loaded;
}

Bool_t AliTRDclusterizer::ReadTracks()
{
  //
  // Reads simulated GTU tracks from the input aliroot file
  //

  AliRunLoader *runLoader = AliRunLoader::Instance();

  if (!runLoader) {
    AliError("No run loader available");
    return kFALSE;
  }

  AliLoader* loader = runLoader->GetLoader("TRDLoader");
  if (!loader) {
    return kFALSE;
  }

  AliDataLoader *trackLoader = loader->GetDataLoader("gtutracks");
  if (!trackLoader) {
      return kFALSE;
  }

  Bool_t loaded = kFALSE;

  trackLoader->Load();

  TTree *trackTree = trackLoader->Tree();
  if (trackTree) {
    TClonesArray *trackArray = TracksArray();
    AliTRDtrackGTU *trk = 0x0;
    trackTree->SetBranchAddress("TRDtrackGTU", &trk);
    for (Int_t iTrack = 0; iTrack < trackTree->GetEntries(); iTrack++) {
      trackTree->GetEntry(iTrack);
      new ((*trackArray)[trackArray->GetEntries()]) AliESDTrdTrack(*(trk->CreateTrdTrack()));
    }
    loaded = kTRUE;
  }

  trackLoader->UnloadAll();
  trackLoader->CloseFile();

  return loaded;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::MakeClusters()
{
  //
  // Creates clusters from digits
  //

  // Propagate info from the digits manager
  if (TestBit(kLabels)){
    SetBit(kLabels, fDigitsManager->UsesDictionaries());
  }
  
  Bool_t fReturn = kTRUE;
  for (Int_t i = 0; i < AliTRDgeometry::kNdet; i++){
  
    AliTRDarrayADC *digitsIn = (AliTRDarrayADC*) fDigitsManager->GetDigits(i); //mod     
    // This is to take care of switched off super modules
    if (!digitsIn->HasData()) continue;
    digitsIn->Expand();
    digitsIn->DeleteNegatives();  // Restore digits array to >=0 values
    AliTRDSignalIndex* indexes = fDigitsManager->GetIndexes(i);
    if (indexes->IsAllocated() == kFALSE){
      fDigitsManager->BuildIndexes(i);
    }
  
    Bool_t fR(kFALSE);
    if (indexes->HasEntry()){
      if (TestBit(kLabels)){
        Int_t nDict(0);
        for (Int_t iDict = 0; iDict < AliTRDdigitsManager::kNDict; iDict++){
          AliTRDarrayDictionary *tracksIn(NULL); //mod
          tracksIn = (AliTRDarrayDictionary *) fDigitsManager->GetDictionary(i,iDict);  //mod
          // This is to take care of data reconstruction
          if (!tracksIn->GetDim()) continue;
          tracksIn->Expand(); nDict++; 
        }
        if(!nDict){
          AliDebug(1, "MC labels not available. Switch them off.");
          SetUseLabels(kFALSE);
        }
      }
      fR = MakeClusters(i);
      fReturn = fR && fReturn;
    }
  
    //if (fR == kFALSE){
    //  if(IsWritingClusters()) WriteClusters(i);
    //  ResetRecPoints();
    //}
        
    // Clear arrays of this chamber, to prepare for next event
    fDigitsManager->ClearArrays(i);
  }
  
  if(fReconstructor->IsWritingClusters()) WriteClusters(-1);

  AliInfo(Form("Found :: clusters[%d] tracklets[%d] tracks[%d]",
    RecPoints()?RecPoints()->GetEntriesFast():0,
    TrackletsArray()?TrackletsArray()->GetEntriesFast():0,
    TracksArray()?TracksArray()->GetEntriesFast():0));

  return fReturn;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::Raw2Clusters(AliRawReader *rawReader)
{
  //
  // Creates clusters from raw data
  //

  return Raw2ClustersChamber(rawReader);

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::Raw2ClustersChamber(AliRawReader *rawReader)
{
  //
  // Creates clusters from raw data
  //

  // Create the digits manager
  if (!fDigitsManager){
    SetBit(knewDM, kTRUE);
    fDigitsManager = new AliTRDdigitsManager(kTRUE);
    fDigitsManager->CreateArrays();
  }

  fDigitsManager->SetUseDictionaries(TestBit(kLabels));

  if(!fRawStream)
    fRawStream = new AliTRDrawStream(rawReader);
  else
    fRawStream->SetReader(rawReader);

  //if(fReconstructor->IsHLT()){
    fRawStream->DisableErrorStorage();
  //}

  // register tracklet array for output
  fRawStream->SetTrackletArray(TrackletsArray("AliTRDtrackletWord"));
  fRawStream->SetTrackArray(TracksArray());

  UInt_t det = 0;
  while ((det = fRawStream->NextChamber(fDigitsManager)) < AliTRDgeometry::kNdet){
    if (fDigitsManager->GetIndexes(det)->HasEntry()){
      MakeClusters(det);
      fDigitsManager->ClearArrays(det);
    }
  }

  for (Int_t iSector = 0; iSector < AliTRDgeometry::kNsector; iSector++) {
    fTrgFlags[iSector] = fRawStream->GetTriggerFlags(iSector);
  }

  if(fReconstructor->IsWritingClusters()) WriteClusters(-1);

  if(!TestBit(knewDM)){
    delete fDigitsManager;
    fDigitsManager = NULL;
    delete fRawStream;
    fRawStream = NULL;
  }

  AliInfo(Form("Found :: clusters[%d] tracklets[%d] tracks[%d]",
    RecPoints()?RecPoints()->GetEntriesFast():0,
    TrackletsArray()?TrackletsArray()->GetEntriesFast():0,
    TracksArray()?TracksArray()->GetEntriesFast():0));
  return kTRUE;

}

//_____________________________________________________________________________
UChar_t AliTRDclusterizer::GetStatus(Short_t &signal)
{
  //
  // Check if a pad is masked
  //

  UChar_t status = 0;

  if(signal>0 && TESTBIT(signal, 10)){
    CLRBIT(signal, 10);
    for(int ibit=0; ibit<4; ibit++){
      if(TESTBIT(signal, 11+ibit)){
        SETBIT(status, ibit);
        CLRBIT(signal, 11+ibit);
      } 
    }
  }
  return status;
}

//_____________________________________________________________________________
void AliTRDclusterizer::SetPadStatus(const UChar_t status, UChar_t &out) const {
  //
  // Set the pad status into out
  // First three bits are needed for the position encoding
  //
  out |= status << 3;
}

//_____________________________________________________________________________
UChar_t AliTRDclusterizer::GetPadStatus(UChar_t encoding) const {
  //
  // return the staus encoding of the corrupted pad
  //
  return static_cast<UChar_t>(encoding >> 3);
}

//_____________________________________________________________________________
Int_t AliTRDclusterizer::GetCorruption(UChar_t encoding) const {
  //
  // Return the position of the corruption
  //
  return encoding & 7;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::MakeClusters(Int_t det)
{
  //
  // Generates the cluster.
  //

  // Get the digits
  fDigits = (AliTRDarrayADC *) fDigitsManager->GetDigits(det); //mod
  fBaseline = fDigitsManager->GetDigitsParam()->GetADCbaseline(det);
  
  // This is to take care of switched off super modules
  if (!fDigits->HasData()) return kFALSE;

  fIndexes = fDigitsManager->GetIndexes(det);
  if (fIndexes->IsAllocated() == kFALSE) {
    AliError("Indexes do not exist!");
    return kFALSE;      
  }

  AliTRDcalibDB* const calibration = AliTRDcalibDB::Instance();
  if (!calibration) {
    AliFatal("No AliTRDcalibDB instance available\n");
    return kFALSE;  
  }

  if (!fReconstructor){
    AliError("Reconstructor not set\n");
    return kFALSE;
  }

  const AliTRDrecoParam *const recoParam = fReconstructor->GetRecoParam();

  fMaxThresh            = recoParam->GetClusMaxThresh();
  fMaxThreshTest        = (recoParam->GetClusMaxThresh()/2+fBaseline);
  fSigThresh            = recoParam->GetClusSigThresh();
  fMinMaxCutSigma       = recoParam->GetMinMaxCutSigma();
  fMinLeftRightCutSigma = recoParam->GetMinLeftRightCutSigma();
  const Int_t iEveryNTB = recoParam->GetRecEveryNTB();

  Int_t istack  = fIndexes->GetStack();
  fLayer  = fIndexes->GetLayer();
  Int_t isector = fIndexes->GetSM();

  // Start clustering in the chamber

  fDet  = AliTRDgeometry::GetDetector(fLayer,istack,isector);
  if (fDet != det) {
    AliError(Form("Detector number missmatch! Request[%03d] RAW[%03d]", det, fDet));
    return kFALSE;
  }

  AliDebug(2, Form("Det[%d] @ Sec[%d] Stk[%d] Ly[%d]", fDet, isector, istack, fLayer));

  // TRD space point transformation
  fTransform->SetDetector(det);

  Int_t    iGeoLayer  = AliGeomManager::kTRD1 + fLayer;
  Int_t    iGeoModule = istack + AliTRDgeometry::Nstack() * isector;
  fVolid      = AliGeomManager::LayerToVolUID(iGeoLayer,iGeoModule); 

  fColMax    = fDigits->GetNcol();
  fTimeTotal = fDigitsManager->GetDigitsParam()->GetNTimeBins(det);

  // Check consistency between Geometry and raw data
  AliTRDpadPlane *pp(fTransform->GetPadPlane());
  Int_t ncols(pp->GetNcols()), nrows(pp->GetNrows());
  if(ncols != fColMax) AliDebug(1, Form("N cols missmatch in Digits for Det[%3d] :: Geom[%3d] RAW[%3d]", fDet, ncols, fColMax));
  if(nrows != fDigits->GetNrow()) AliDebug(1, Form("N rows missmatch in Digits for Det[%3d] :: Geom[%3d] RAW[%3d]", fDet, nrows, fDigits->GetNrow()));
  if(ncols != fIndexes->GetNcol()) AliDebug(1, Form("N cols missmatch in Digits for Det[%3d] :: Geom[%3d] RAW[%3d]", fDet, ncols, fIndexes->GetNcol()));
  if(nrows != fIndexes->GetNrow()) AliDebug(1, Form("N rows missmatch in Digits for Det[%3d] :: Geom[%3d] RAW[%3d]", fDet, nrows, fIndexes->GetNrow()));

  // Check consistency between OCDB and raw data
  Int_t nTimeOCDB = calibration->GetNumberOfTimeBinsDCS();
  if(fReconstructor->IsHLT()){
    if((nTimeOCDB > -1) && (fTimeTotal != nTimeOCDB)){
      AliWarning(Form("Number of timebins does not match OCDB value (RAW[%d] OCDB[%d]), using raw value"
		      ,fTimeTotal,nTimeOCDB));
    }
  }else{
    if(nTimeOCDB == -1){
      AliDebug(1, "Undefined number of timebins in OCDB, using value from raw data.");
      if(!(fTimeTotal>0)){
        AliError(Form("Number of timebins in raw data is negative, skipping chamber[%3d]!", fDet));
        return kFALSE;
      }
    }else if(nTimeOCDB == -2){
      AliError("Mixed number of timebins in OCDB, no reconstruction of TRD data!"); 
      return kFALSE;
    }else if(fTimeTotal != nTimeOCDB){
      AliError(Form("Number of timebins in raw data does not match OCDB value (RAW[%d] OCDB[%d]), skipping chamber[%3d]!"
        ,fTimeTotal,nTimeOCDB, fDet));
      return kFALSE;
    }
  }
  AliDebug(1, Form("Using %2d number of timebins for Det[%03d].", fTimeTotal, fDet));

  // Detector wise calibration object for the gain factors
  const AliTRDCalDet *calGainFactorDet = calibration->GetGainFactorDet();
  // Calibration object with pad wise values for the gain factors
  fCalGainFactorROC      = calibration->GetGainFactorROC(fDet);
  // Calibration value for chamber wise gain factor
  fCalGainFactorDetValue = calGainFactorDet->GetValue(fDet);

  // Detector wise calibration object for the noise
  const AliTRDCalDet *calNoiseDet = calibration->GetNoiseDet();
  // Calibration object with pad wise values for the noise
  fCalNoiseROC           = calibration->GetNoiseROC(fDet);
  // Calibration value for chamber wise noise
  fCalNoiseDetValue      = calNoiseDet->GetValue(fDet);
  
  // Calibration object with the pad status
  fCalPadStatusROC       = calibration->GetPadStatusROC(fDet);
  // Calibration object of the online gain
  fCalOnlGainROC          = 0x0;  
  if (calibration->HasOnlineFilterGain()) {
    fCalOnlGainROC        = calibration->GetOnlineGainTableROC(fDet);
  }

  firstClusterROC = -1;
  fClusterROC     =  0;

  SetBit(kLUT, recoParam->UseLUT());
  SetBit(kGAUS, recoParam->UseGAUS());

  // Apply the gain and the tail cancelation via digital filter
  // Use the configuration from the DCS to find out whether online 
  // tail cancellation was applied
  if(!calibration->HasOnlineTailCancellation()){
    // save a copy of raw data
    if(TestBit(kRawSignal)){
      if(fDigitsRaw){
        fDigitsRaw->~AliTRDarrayADC();
        new(fDigitsRaw) AliTRDarrayADC(*fDigits);
      } else fDigitsRaw = new AliTRDarrayADC(*fDigits);
    }
    TailCancelation(recoParam);
  }

  MaxStruct curr, last;
  Int_t nMaximas = 0, nCorrupted = 0;

  // Here the clusterfining is happening
  
  for(curr.time = 0; curr.time < fTimeTotal; curr.time+=iEveryNTB){
    while(fIndexes->NextRCIndex(curr.row, curr.col)){
      if(fDigits->GetData(curr.row, curr.col, curr.time) > fMaxThreshTest && IsMaximum(curr, curr.padStatus, &curr.signals[0])){
        if(last.row>-1){
          if(curr.col==last.col+2 && curr.row==last.row && curr.time==last.time) FivePadCluster(last, curr);
          CreateCluster(last);
        }
        last=curr; curr.fivePad=kFALSE;
      }
    }
  }
  if(last.row>-1) CreateCluster(last);

  if(recoParam->GetStreamLevel(AliTRDrecoParam::kClusterizer) > 2 && fReconstructor->IsDebugStreaming()){
    TTreeSRedirector* fDebugStream = fReconstructor->GetDebugStream(AliTRDrecoParam::kClusterizer);
    (*fDebugStream) << "MakeClusters"
      << "Detector="   << det
      << "NMaxima="    << nMaximas
      << "NClusters="  << fClusterROC
      << "NCorrupted=" << nCorrupted
      << "\n";
  }
  if (TestBit(kLabels)) AddLabels();

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::IsMaximum(const MaxStruct &Max, UChar_t &padStatus, Float_t *const Signals)
{
  //
  // Returns true if this row,col,time combination is a maximum. 
  // Gives back the padStatus and the signals of the center pad and the two neighbouring pads.
  //

  Float_t gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(Max.col,Max.row);
  Float_t onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(Max.row,Max.col) : 1;
  Signals[1] = (fDigits->GetData(Max.row, Max.col, Max.time) - fBaseline) /(onlcf * gain) + 0.5f;
  if(Signals[1] <= fMaxThresh) return kFALSE;

  if(Max.col < 1 || Max.col + 1 >= fColMax) return kFALSE;

  Float_t noiseMiddleThresh = fMinMaxCutSigma*fCalNoiseDetValue*fCalNoiseROC->GetValue(Max.col, Max.row);
  if (Signals[1] <= noiseMiddleThresh) return kFALSE;  

  Char_t status[3]={
    fCalPadStatusROC->GetStatus(Max.col-1, Max.row)
   ,fCalPadStatusROC->GetStatus(Max.col,   Max.row)
   ,fCalPadStatusROC->GetStatus(Max.col+1, Max.row)
  };

  Short_t signal(0);
  if((signal = fDigits->GetData(Max.row, Max.col-1, Max.time))){
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(Max.col-1,Max.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(Max.row,Max.col-1) : 1;
    Signals[0] = (signal - fBaseline) /( onlcf * gain) + 0.5f;
  } else Signals[0] = 0.;
  if((signal = fDigits->GetData(Max.row, Max.col+1, Max.time))){
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(Max.col+1,Max.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(Max.row,Max.col+1) : 1;
    Signals[2] = (signal - fBaseline) /( onlcf *  gain) + 0.5f;
  } else Signals[2] = 0.;

  if(!(status[0] | status[1] | status[2])) {//all pads are good
    if ((Signals[2] <= Signals[1]) && (Signals[0] <  Signals[1])) {
      if ((Signals[2] > fSigThresh) || (Signals[0] > fSigThresh)) {
        if(Signals[0]<0) Signals[0]=0.;
        if(Signals[2]<0) Signals[2]=0.;
        Float_t noiseSumThresh = fMinLeftRightCutSigma * fCalNoiseDetValue
                               * fCalNoiseROC->GetValue(Max.col, Max.row);
        if ((Signals[2]+Signals[0]+Signals[1]) <= noiseSumThresh) return kFALSE;
        padStatus = 0;
        return kTRUE;
      }
    }
  } else { // at least one of the pads is bad, and reject candidates with more than 1 problematic pad
    if(Signals[0]<0)Signals[0]=0;
    if(Signals[2]<0)Signals[2]=0;
    if (status[2] && (!(status[0] || status[1])) && Signals[1] > Signals[0] && Signals[0] > fSigThresh) { 
      Signals[2]=0;
      SetPadStatus(status[2], padStatus);
      return kTRUE;
    } 
    else if (status[0] && (!(status[1] || status[2])) && Signals[1] >= Signals[2] && Signals[2] > fSigThresh) {
      Signals[0]=0;
      SetPadStatus(status[0], padStatus);
      return kTRUE;
    }
    else if (status[1] && (!(status[0] || status[2])) && ((Signals[2] > fSigThresh) || (Signals[0] > fSigThresh))) {
      Signals[1] = fMaxThresh;
      SetPadStatus(status[1], padStatus);
      return kTRUE;
    }
  }
  return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::FivePadCluster(MaxStruct &ThisMax, MaxStruct &NeighbourMax)
{
  //
  // Look for 5 pad cluster with minimum in the middle
  // Gives back the ratio
  //
  
  if (ThisMax.col >= fColMax - 3) return kFALSE;
  Float_t gain;
  Float_t onlcf;
  if (ThisMax.col < fColMax - 5){
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(ThisMax.col+4,ThisMax.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(ThisMax.row,ThisMax.col+4) : 1;
    if (fDigits->GetData(ThisMax.row, ThisMax.col+4, ThisMax.time) - fBaseline >= fSigThresh * gain * onlcf)
      return kFALSE;
  }
  if (ThisMax.col > 1) {
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(ThisMax.col-2,ThisMax.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(ThisMax.row,ThisMax.col-2) : 1;
    if (fDigits->GetData(ThisMax.row, ThisMax.col-2, ThisMax.time) - fBaseline >= fSigThresh * gain * onlcf)
      return kFALSE;
  }
  
  const Float_t kEpsilon = 0.01;
  Double_t padSignal[5] = {ThisMax.signals[0], ThisMax.signals[1], ThisMax.signals[2],
      NeighbourMax.signals[1], NeighbourMax.signals[2]};
  
  // Unfold the two maxima and set the signal on 
  // the overlapping pad to the ratio
  Float_t ratio = Unfold(kEpsilon,fLayer,padSignal);
  ThisMax.signals[2] = ThisMax.signals[2]*ratio + 0.5f;
  NeighbourMax.signals[0] = NeighbourMax.signals[0]*(1-ratio) + 0.5f;
  ThisMax.fivePad=kTRUE;
  NeighbourMax.fivePad=kTRUE;
  return kTRUE;

}

//_____________________________________________________________________________
void AliTRDclusterizer::CreateCluster(const MaxStruct &Max)
{
  //
  // Creates a cluster at the given position and saves it in RecPoints
  //

  Int_t nPadCount = 1;
  Short_t signals[7] = { 0, 0, (Short_t)Max.signals[0], (Short_t)Max.signals[1], (Short_t)Max.signals[2], 0, 0 };
  if(!fReconstructor->IsHLT()) CalcAdditionalInfo(Max, signals, nPadCount);

  AliTRDcluster cluster(fDet, ((UChar_t) Max.col), ((UChar_t) Max.row), ((UChar_t) Max.time), signals, fVolid);
  cluster.SetNPads(nPadCount);
  cluster.SetQ(Max.signals[0]+Max.signals[1]+Max.signals[2]);
  if(TestBit(kLUT)) cluster.SetRPhiMethod(AliTRDcluster::kLUT);
  else if(TestBit(kGAUS)) cluster.SetRPhiMethod(AliTRDcluster::kGAUS);
  else cluster.SetRPhiMethod(AliTRDcluster::kCOG);

  cluster.SetFivePad(Max.fivePad);
  // set pads status for the cluster
  UChar_t maskPosition = GetCorruption(Max.padStatus);
  if (maskPosition) { 
    cluster.SetPadMaskedPosition(maskPosition);
    cluster.SetPadMaskedStatus(GetPadStatus(Max.padStatus));
  }
  cluster.SetXcorr(fReconstructor->UseClusterRadialCorrection());

  // Transform the local cluster coordinates into calibrated 
  // space point positions defined in the local tracking system.
  // Here the calibration for T0, Vdrift and ExB is applied as well.
  if(!TestBit(kSkipTrafo)) if(!fTransform->Transform(&cluster)) return;
  // Store raw signals in cluster. This MUST be called after position reconstruction !
  // Xianguo Lu and Alex Bercuci 19.03.2012
  if(TestBit(kRawSignal) && fDigitsRaw){
    Float_t tmp(0.), kMaxShortVal(32767.); // protect against data overflow due to wrong gain calibration
    Short_t rawSignal[7] = {0};
    for(Int_t ipad(Max.col-3), iRawId(0); ipad<=Max.col+3; ipad++, iRawId++){
      if(ipad<0 || ipad>=fColMax) continue;
      if(!fCalOnlGainROC){
        rawSignal[iRawId] = fDigitsRaw->GetData(Max.row, ipad, Max.time);
        continue;
      }
      // Deconvolute online gain calibration when available
      // Alex Bercuci 27.04.2012
      tmp = (fDigitsRaw->GetData(Max.row, ipad, Max.time) - fBaseline)/fCalOnlGainROC->GetGainCorrectionFactor(Max.row, ipad) + 0.5f;
      rawSignal[iRawId] = (Short_t)TMath::Min(tmp, kMaxShortVal);
    }
    cluster.SetSignals(rawSignal, kTRUE);
  }
  // Temporarily store the Max.Row, column and time bin of the center pad
  // Used to later on assign the track indices
  cluster.SetLabel(Max.row, 0);
  cluster.SetLabel(Max.col, 1);
  cluster.SetLabel(Max.time,2);

  //needed for HLT reconstruction
  AddClusterToArray(&cluster);

  // Store the index of the first cluster in the current ROC
  if (firstClusterROC < 0) firstClusterROC = fNoOfClusters;
  
  fNoOfClusters++;
  fClusterROC++;
}

//_____________________________________________________________________________
void AliTRDclusterizer::CalcAdditionalInfo(const MaxStruct &Max, Short_t *const signals, Int_t &nPadCount)
{
// Calculate number of pads/cluster and
// ADC signals at position 0, 1, 5 and 6

  Float_t tmp(0.), kMaxShortVal(32767.); // protect against data overflow due to wrong gain calibration
  Float_t gain(1.); Float_t onlcf(1.); Short_t signal(0);
  // Store the amplitudes of the pads in the cluster for later analysis
  // and check whether one of these pads is masked in the database
  signals[3]=Max.signals[1];
  Int_t ipad(1), jpad(0);
  // Look to the right
  while((jpad = Max.col-ipad)){
    if(!(signal = fDigits->GetData(Max.row, jpad, Max.time))) break; // empty digit !
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(jpad, Max.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(Max.row,jpad) : 1;
    tmp = (signal - fBaseline) / (onlcf * gain) + 0.5f;
    signal = (Short_t)TMath::Min(tmp, kMaxShortVal);
    if(signal<fSigThresh) break; // signal under threshold
    nPadCount++;
    if(ipad<=3) signals[3 - ipad] = signal;
    ipad++;
  }
  ipad=1;
  // Look to the left
  while((jpad = Max.col+ipad)<fColMax){
    if(!(signal = fDigits->GetData(Max.row, jpad, Max.time))) break; // empty digit !
    gain = fCalGainFactorDetValue * fCalGainFactorROC->GetValue(jpad, Max.row);
    onlcf = fCalOnlGainROC ? fCalOnlGainROC->GetGainCorrectionFactor(Max.row,jpad) : 1;
    tmp = (signal - fBaseline) / (onlcf * gain) + 0.5f;
    signal = (Short_t)TMath::Min(tmp, kMaxShortVal);
    if(signal<fSigThresh) break; // signal under threshold
    nPadCount++;
    if(ipad<=3) signals[3 + ipad] = signal;
    ipad++;
  }

  AliDebug(4, Form("Signals[%3d %3d %3d %3d %3d %3d %3d] Npads[%d]."
    , signals[0], signals[1], signals[2], signals[3], signals[4], signals[5], signals[6], nPadCount));
}

//_____________________________________________________________________________
void AliTRDclusterizer::AddClusterToArray(AliTRDcluster* cluster)
{
  //
  // Add a cluster to the array
  //

  Int_t n = RecPoints()->GetEntriesFast();
  if(n!=fNoOfClusters)AliError(Form("fNoOfClusters != RecPoints()->GetEntriesFast %i != %i \n", fNoOfClusters, n));
  new((*RecPoints())[n]) AliTRDcluster(*cluster);
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::AddLabels()
{
  //
  // Add the track indices to the found clusters
  //
  
  const Int_t   kNclus  = 3;  
  const Int_t   kNdict  = AliTRDdigitsManager::kNDict;
  const Int_t   kNtrack = kNdict * kNclus;

  Int_t iClusterROC = 0;

  Int_t row  = 0;
  Int_t col  = 0;
  Int_t time = 0;
  Int_t iPad = 0;

  // Temporary array to collect the track indices
  Int_t *idxTracks = new Int_t[kNtrack*fClusterROC];

  // Loop through the dictionary arrays one-by-one
  // to keep memory consumption low
  AliTRDarrayDictionary *tracksIn(NULL);  //mod
  for (Int_t iDict = 0; iDict < kNdict; iDict++) {

    // tracksIn should be expanded beforehand!
    tracksIn = (AliTRDarrayDictionary *) fDigitsManager->GetDictionary(fDet,iDict);

    // Loop though the clusters found in this ROC
    for (iClusterROC = 0; iClusterROC < fClusterROC; iClusterROC++) {

      AliTRDcluster *cluster = (AliTRDcluster *)
                               RecPoints()->UncheckedAt(firstClusterROC+iClusterROC);
      row  = cluster->GetLabel(0);
      col  = cluster->GetLabel(1);
      time = cluster->GetLabel(2);

      for (iPad = 0; iPad < kNclus; iPad++) {
        Int_t iPadCol = col - 1 + iPad;
        Int_t index   = tracksIn->GetData(row,iPadCol,time);  //Modification of -1 in Track
        idxTracks[3*iPad+iDict + iClusterROC*kNtrack] = index;     
      }

    }

  }

  // Copy the track indices into the cluster
  // Loop though the clusters found in this ROC
  for (iClusterROC = 0; iClusterROC < fClusterROC; iClusterROC++) {

    AliTRDcluster *cluster = (AliTRDcluster *)
      RecPoints()->UncheckedAt(firstClusterROC+iClusterROC);
    cluster->SetLabel(-9999,0);
    cluster->SetLabel(-9999,1);
    cluster->SetLabel(-9999,2);
  
    cluster->AddTrackIndex(&idxTracks[iClusterROC*kNtrack]);

  }

  delete [] idxTracks;

  return kTRUE;

}

//_____________________________________________________________________________
Float_t AliTRDclusterizer::Unfold(Double_t eps, Int_t layer, const Double_t *const padSignal) const
{
  //
  // Method to unfold neighbouring maxima.
  // The charge ratio on the overlapping pad is calculated
  // until there is no more change within the range given by eps.
  // The resulting ratio is then returned to the calling method.
  //

  AliTRDcalibDB *calibration = AliTRDcalibDB::Instance();
  if (!calibration) {
    AliError("No AliTRDcalibDB instance available\n");
    return kFALSE;  
  }
  
  Int_t   irc                = 0;
  Int_t   itStep             = 0;                 // Count iteration steps

  Double_t ratio             = 0.5;               // Start value for ratio
  Double_t prevRatio         = 0.0;               // Store previous ratio

  Double_t newLeftSignal[3]  = { 0.0, 0.0, 0.0 }; // Array to store left cluster signal
  Double_t newRightSignal[3] = { 0.0, 0.0, 0.0 }; // Array to store right cluster signal
  Double_t newSignal[3]      = { 0.0, 0.0, 0.0 };

  // Start the iteration
  while ((TMath::Abs(prevRatio - ratio) > eps) && (itStep < 10)) {

    itStep++;
    prevRatio = ratio;

    // Cluster position according to charge ratio
    Double_t maxLeft  = (ratio*padSignal[2] - padSignal[0]) 
                      / (padSignal[0] + padSignal[1] + ratio * padSignal[2]);
    Double_t maxRight = (padSignal[4] - (1-ratio)*padSignal[2]) 
                      / ((1.0 - ratio)*padSignal[2] + padSignal[3] + padSignal[4]);

    // Set cluster charge ratio
    irc = calibration->PadResponse(1.0, maxLeft, layer, newSignal);
    Double_t ampLeft  = padSignal[1] / newSignal[1];
    irc = calibration->PadResponse(1.0, maxRight, layer, newSignal);
    Double_t ampRight = padSignal[3] / newSignal[1];

    // Apply pad response to parameters
    irc = calibration->PadResponse(ampLeft ,maxLeft ,layer,newLeftSignal );
    irc = calibration->PadResponse(ampRight,maxRight,layer,newRightSignal);

    // Calculate new overlapping ratio
    // Coverity
    if (irc != 0) {
      ratio = TMath::Min((Double_t) 1.0
                        ,newLeftSignal[2] / (newLeftSignal[2] + newRightSignal[0]));
    }

  }

  return ratio;

}

//_____________________________________________________________________________
void AliTRDclusterizer::TailCancelation(const AliTRDrecoParam* const recoParam)
{
  //
  // Applies the tail cancelation
  //

  Int_t nexp = recoParam->GetTCnexp();
  if(!nexp) return;
  
  Int_t iRow  = 0;
  Int_t iCol  = 0;
  Int_t iTime = 0;

  TTreeSRedirector *fDebugStream = fReconstructor->GetDebugStream(AliTRDrecoParam::kClusterizer);
  Bool_t debugStreaming = recoParam->GetStreamLevel(AliTRDrecoParam::kClusterizer) > 7 && fReconstructor->IsDebugStreaming();
  while(fIndexes->NextRCIndex(iRow, iCol))
    {
      // if corrupted then don't make the tail cancallation
      if (fCalPadStatusROC->GetStatus(iCol, iRow)) continue;

      if(debugStreaming){
      	for (iTime = 0; iTime < fTimeTotal; iTime++) 
      	  (*fDebugStream) << "TailCancellation"
      			  << "col="  << iCol
      			  << "row="  << iRow
      			  << "\n";
      }
      
      // Apply the tail cancelation via the digital filter
      //DeConvExp(fDigits->GetDataAddress(iRow,iCol),fTimeTotal,nexp);
      ApplyTCTM(fDigits->GetDataAddress(iRow,iCol),fTimeTotal,nexp);
    } // while irow icol

  return;

}


//_____________________________________________________________________________
void AliTRDclusterizer::ApplyTCTM(Short_t *const arr, const Int_t nTime, const Int_t nexp) 
{
  //
  // Steer tail cancellation
  //


  switch(nexp) {
  case 1:
  case 2:
    DeConvExp(arr,nTime,nexp);
    break;
  case -1:
    ConvExp(arr,nTime);
    break;
  case -2:
    DeConvExp(arr,nTime,1);
    ConvExp(arr,nTime);
    break;
  default:
    break;
  }
}


//_____________________________________________________________________________
void AliTRDclusterizer::ConvExp(Short_t *const arr, const Int_t nTime)
{
  //
  // Tail maker
  //

  // Initialization (coefficient = alpha, rates = lambda)
  Float_t slope = 1.0;
  Float_t coeff = 0.5;
  Float_t rate;

  Double_t par[4];
  fReconstructor->GetRecoParam()->GetTCParams(par);
  slope = par[1];
  coeff = par[3];  

  Double_t dt = 0.1;

  rate = TMath::Exp(-dt/(slope));
   
  Float_t reminder =  .0;
  Float_t correction = 0.0;
  Float_t result     = 0.0;

  for (int i = nTime-1; i >= 0; i--) {

    result = arr[i] + correction - fBaseline;    // No rescaling
    arr[i] = (Short_t)(result + fBaseline + 0.5f);

    correction = 0.0;
    
    correction += reminder = rate * (reminder + coeff * result);
  }
}


//_____________________________________________________________________________
void AliTRDclusterizer::DeConvExp(Short_t *const arr, const Int_t nTime, const Int_t nexp)
{
  //
  // Tail cancellation by deconvolution for PASA v4 TRF
  //

  Float_t rates[2];
  Float_t coefficients[2];

  // Initialization (coefficient = alpha, rates = lambda)
  Float_t r1 = 1.0;
  Float_t r2 = 1.0;
  Float_t c1 = 0.5;
  Float_t c2 = 0.5;

  if (nexp == 1) {   // 1 Exponentials
    r1 = 1.156;
    r2 = 0.130;
    c1 = 0.066;
    c2 = 0.000;
  }
  if (nexp == 2) {   // 2 Exponentials
    Double_t par[4];
    fReconstructor->GetRecoParam()->GetTCParams(par);
    r1 = par[0];//1.156;
    r2 = par[1];//0.130;
    c1 = par[2];//0.114;
    c2 = par[3];//0.624;
  }

  coefficients[0] = c1;
  coefficients[1] = c2;

  Double_t dt = 0.1;

  rates[0] = TMath::Exp(-dt/(r1));
  rates[1] = (nexp == 1) ? .0 : TMath::Exp(-dt/(r2));
  
  Float_t reminder[2] = { .0, .0 };
  Float_t correction = 0.0;
  Float_t result     = 0.0;

  for (int i = 0; i < nTime; i++) {

    result = arr[i] - correction - fBaseline;    // No rescaling
    arr[i] = (Short_t)(result + fBaseline + 0.5f);

    correction = 0.0;
    for (int k = 0; k < 2; k++) {
      correction += reminder[k] = rates[k] * (reminder[k] + coefficients[k] * result);
    }
  }
}

//_____________________________________________________________________________
TClonesArray *AliTRDclusterizer::RecPoints() 
{
  //
  // Returns the list of rec points
  //

  fRecPoints = AliTRDReconstructor::GetClusters();
  if (!fRecPoints) AliError("Missing cluster array");
  return fRecPoints;
}

//_____________________________________________________________________________
TClonesArray *AliTRDclusterizer::TrackletsArray(const TString &trkltype)
{
  //
  // Returns the array of on-line tracklets
  //
  fTracklets = AliTRDReconstructor::GetTracklets(trkltype.Data());
  if (!fTracklets) AliError("Missing online tracklets array");
  return fTracklets;
}

//_____________________________________________________________________________
TClonesArray* AliTRDclusterizer::TracksArray()
{
  // return array of GTU tracks (create TClonesArray if necessary)

  fTracks = AliTRDReconstructor::GetTracks();
  if (!fTracks) AliError("Missing online tracks array");
  return fTracks;
}

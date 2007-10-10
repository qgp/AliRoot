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

/*
$Log$
Revision 1.14  2007/06/06 16:26:30  arcelli
remove fall-back call to local CDB storage

Revision 1.13  2007/04/20 13:59:40  arcelli
make protections agains failed retrieval of the CDB object in a proper way

Revision 1.12  2007/03/23 11:31:16  arcelli
CDB Entry for TOF Reconstruction Parameters

Revision 1.11  2007/02/28 18:08:26  arcelli
Add protection against failed retrieval of the CDB cal object

Revision 1.10  2006/08/22 13:30:49  arcelli
removal of effective c++ warnings (C.Zampolli)

Revision 1.9  2006/04/20 22:30:50  hristov
Coding conventions (Annalisa)

Revision 1.8  2006/04/16 22:29:05  hristov
Coding conventions (Annalisa)

Revision 1.7  2006/04/16 20:12:46  hristov
Removing memory leak in case of cached CDB entries

Revision 1.6  2006/04/11 15:28:32  hristov
Checks on cache status before deleting calibration objects (A.Colla)

Revision 1.5  2006/04/05 08:35:38  hristov
Coding conventions (S.Arcelli, C.Zampolli)

Revision 1.4  2006/03/31 11:26:46  arcelli
 changing CDB Ids according to standard convention

Revision 1.3  2006/03/28 14:57:02  arcelli
updates to handle new V5 geometry & some re-arrangements

Revision 1.2  2006/02/13 17:22:26  arcelli
just Fixing Log info

Revision 1.1  2006/02/13 16:10:48  arcelli
Add classes for TOF Calibration (C.Zampolli)

author: Chiara Zampolli, zampolli@bo.infn.it
*/  

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF calibration                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"
#include "TProfile.h"
#include "TGrid.h"

#include "AliCDBEntry.h"
#include "AliCDBId.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBMetaData.h"
#include "AliESDtrack.h"
#include "AliESD.h"
#include "AliLog.h"

#include "AliTOFcalib.h"
#include "AliTOFChannelOnline.h"
#include "AliTOFChannelOffline.h"
#include "AliTOFGeometry.h"
#include "AliTOFRecoParam.h"

extern TROOT *gROOT;
extern TStyle *gStyle;

ClassImp(AliTOFcalib)

//_______________________________________________________________________
AliTOFcalib::AliTOFcalib():
  TTask("AliTOFcalib",""),
  fNChannels(-1),
  fTOFCalOnline(0x0),
  fTOFCalOffline(0x0),
  fTOFSimCalOnline(0x0),
  fTOFSimCalOffline(0x0),
  fTOFSimToT(0x0),
  fkValidity(0x0),
  fTree(0x0),
  fNruns(0)
{ 
  //TOF Calibration Class ctor
  fNChannels = AliTOFGeometry::NSectors()*(2*(AliTOFGeometry::NStripC()+AliTOFGeometry::NStripB())+AliTOFGeometry::NStripA())*AliTOFGeometry::NpadZ()*AliTOFGeometry::NpadX();
}
//____________________________________________________________________________ 

AliTOFcalib::AliTOFcalib(const AliTOFcalib & calib):
  TTask("AliTOFcalib",""),
  fNChannels(calib.fNChannels),
  fTOFCalOnline(0x0),
  fTOFCalOffline(0x0),
  fTOFSimCalOnline(0x0),
  fTOFSimCalOffline(0x0),
  fTOFSimToT(calib.fTOFSimToT),
  fkValidity(calib.fkValidity),
  fTree(calib.fTree),
  fNruns(calib.fNruns)
{
  //TOF Calibration Class copy ctor
  for (Int_t iarray = 0; iarray<fNChannels; iarray++){
    AliTOFChannelOnline * calChOnline = (AliTOFChannelOnline*)calib.fTOFCalOnline->At(iarray);
    AliTOFChannelOffline * calChOffline = (AliTOFChannelOffline*)calib.fTOFCalOffline->At(iarray);
    fTOFCalOnline->AddAt(calChOnline,iarray);
    fTOFCalOffline->AddAt(calChOffline,iarray);

    AliTOFChannelOnline * simCalChOnline = (AliTOFChannelOnline*)calib.fTOFSimCalOnline->At(iarray);
    AliTOFChannelOffline * simCalChOffline = (AliTOFChannelOffline*)calib.fTOFSimCalOffline->At(iarray);
    fTOFSimCalOnline->AddAt(simCalChOnline,iarray);
    fTOFSimCalOffline->AddAt(simCalChOffline,iarray);
  }
}

//____________________________________________________________________________ 

AliTOFcalib& AliTOFcalib::operator=(const AliTOFcalib &calib)
{
  //TOF Calibration Class assignment operator
  this->fNChannels = calib.fNChannels;
  this->fTOFSimToT = calib.fTOFSimToT;
  this->fkValidity = calib.fkValidity;
  this->fTree = calib.fTree;
  this->fNruns = calib.fNruns;
  for (Int_t iarray = 0; iarray<fNChannels; iarray++){
    AliTOFChannelOnline * calChOnline = (AliTOFChannelOnline*)calib.fTOFCalOnline->At(iarray);
    AliTOFChannelOffline * calChOffline = (AliTOFChannelOffline*)calib.fTOFCalOffline->At(iarray);
    this->fTOFCalOnline->AddAt(calChOnline,iarray);
    this->fTOFCalOffline->AddAt(calChOffline,iarray);
    AliTOFChannelOnline * simCalChOnline = (AliTOFChannelOnline*)calib.fTOFSimCalOnline->At(iarray);
    AliTOFChannelOffline * simCalChOffline = (AliTOFChannelOffline*)calib.fTOFSimCalOffline->At(iarray);
    this->fTOFSimCalOnline->AddAt(simCalChOnline,iarray);
    this->fTOFSimCalOffline->AddAt(simCalChOffline,iarray);
  }
  return *this;
}

//____________________________________________________________________________ 

AliTOFcalib::~AliTOFcalib()
{
  //TOF Calibration Class dtor
  if(!(AliCDBManager::Instance()->GetCacheFlag())){ // CDB objects must NOT be deleted if cache is active!
    if (fTOFCalOnline){
      fTOFCalOnline->Clear();
      delete fTOFCalOnline;
    }
    if (fTOFCalOffline){
      fTOFCalOffline->Clear();
      delete fTOFCalOffline;
    }
    if (fTOFSimCalOnline){
      fTOFSimCalOnline->Clear();
      delete fTOFSimCalOnline;
    }
    if (fTOFSimCalOffline){
      fTOFSimCalOffline->Clear();
      delete fTOFSimCalOffline;
    }
  }
  if (fTree!=0x0) delete fTree;
}
//_____________________________________________________________________________
void AliTOFcalib::CreateCalArrays(){

  // creating arrays for online/offline calibration objs

  fTOFCalOnline = new TObjArray(fNChannels);
  fTOFCalOffline = new TObjArray(fNChannels);
  fTOFCalOnline->SetOwner();
  fTOFCalOffline->SetOwner();
  for (Int_t iarray = 0; iarray<fNChannels; iarray++){
    AliTOFChannelOnline * calChOnline = new AliTOFChannelOnline();
    AliTOFChannelOffline * calChOffline = new AliTOFChannelOffline();
    fTOFCalOnline->AddAt(calChOnline,iarray);
    fTOFCalOffline->AddAt(calChOffline,iarray);
  }
}
//_____________________________________________________________________________
void AliTOFcalib::CreateSimCalArrays(){

  // creating arrays for simulation online/offline calibration objs

  fTOFSimCalOnline = new TObjArray(fNChannels);
  fTOFSimCalOffline = new TObjArray(fNChannels);
  fTOFSimCalOnline->SetOwner();
  fTOFSimCalOffline->SetOwner();
  for (Int_t iarray = 0; iarray<fNChannels; iarray++){
    AliTOFChannelOnline * simCalChOnline = new AliTOFChannelOnline();
    AliTOFChannelOffline * simCalChOffline = new AliTOFChannelOffline();
    fTOFSimCalOnline->AddAt(simCalChOnline,iarray);
    fTOFSimCalOffline->AddAt(simCalChOffline,iarray);
  }
}
//_____________________________________________________________________________

void AliTOFcalib::WriteParOnlineOnCDB(Char_t *sel, Int_t minrun, Int_t maxrun)
{
  //Write calibration parameters to the CDB
  AliCDBManager *man = AliCDBManager::Instance();
  Char_t *sel1 = "OnlineDelay" ;  // to be consistent with TOFPreprocessor
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId id(out,minrun,maxrun);
  AliCDBMetaData *md = new AliCDBMetaData();
  md->SetResponsible("Chiara Zampolli");
  if (!fTOFCalOnline) {
    // deve uscire!!
  }
  man->Put(fTOFCalOnline,id,md);
  delete md;
}
//_____________________________________________________________________________

void AliTOFcalib::WriteParOfflineOnCDB(Char_t *sel, const Char_t *validity, Int_t minrun, Int_t maxrun)
{
  //Write calibration parameters to the CDB
  AliCDBManager *man = AliCDBManager::Instance();
  Char_t *sel1 = "ParOffline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId id(out,minrun,maxrun);
  AliCDBMetaData *md = new AliCDBMetaData();
  md->SetResponsible("Chiara Zampolli");
  md->SetComment(validity);
  man->Put(fTOFCalOffline,id,md);
  delete md;
}
//_____________________________________________________________________________

Bool_t AliTOFcalib::ReadParOnlineFromCDB(Char_t *sel, Int_t nrun)
{
  //Read calibration parameters from the CDB
  AliCDBManager *man = AliCDBManager::Instance();
  Char_t *sel1 = "OnlineDelay" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  if (!man->Get(out,nrun)) { 
    return kFALSE;
  }
  AliCDBEntry *entry = man->Get(out,nrun);
  if(!entry->GetObject()){
    return kFALSE;
  }  
  
  fTOFCalOnline =(TObjArray*)entry->GetObject();

  return kTRUE; 
   
}
//_____________________________________________________________________________

Bool_t AliTOFcalib::ReadParOfflineFromCDB(Char_t *sel, Int_t nrun)
{
  //Read calibration parameters from the CDB
  AliCDBManager *man = AliCDBManager::Instance();
  Char_t *sel1 = "ParOffline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  if (!man->Get(out,nrun)) { 
    return kFALSE;
  }
  AliCDBEntry *entry = man->Get(out,nrun);
  if(!entry->GetObject()){
    return kFALSE;
  }  
  AliCDBMetaData * md = entry->GetMetaData();
  fkValidity = md->GetComment();  
  fTOFCalOffline =(TObjArray*)entry->GetObject();

  return kTRUE; 
   
}
//_____________________________________________________________________________
void AliTOFcalib::WriteSimParOnlineOnCDB(Char_t *sel, Int_t minrun, Int_t maxrun, TObjArray *calOnline){
  //Write Sim miscalibration parameters to the CDB

  fTOFSimCalOnline=calOnline;  
  AliCDBManager *man = AliCDBManager::Instance();
  AliCDBMetaData *md = new AliCDBMetaData();
  md->SetResponsible("Chiara Zampolli");
  Char_t *sel1 = "SimParOnline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId id1(out,minrun,maxrun);
  man->Put(fTOFSimCalOnline,id1,md);
  delete md;
}
//_____________________________________________________________________________
void AliTOFcalib::WriteSimParOfflineOnCDB(Char_t *sel, const Char_t *validity, Int_t minrun, Int_t maxrun, TObjArray *calOffline, TH1F *histo){
  //Write Sim miscalibration parameters to the CDB

  fTOFSimToT=histo;
  fTOFSimCalOffline=calOffline;  
  AliCDBManager *man = AliCDBManager::Instance();
  AliCDBMetaData *md = new AliCDBMetaData();
  md->SetResponsible("Chiara Zampolli");
  md->SetComment(validity);
  Char_t *sel1 = "SimParOffline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId id1(out,minrun,maxrun);
  man->Put(fTOFSimCalOffline,id1,md);
  Char_t *sel2 = "SimHisto" ;
  sprintf(out,"%s/%s",sel,sel2); 
  AliCDBMetaData *mdhisto = new AliCDBMetaData();
  mdhisto->SetResponsible("Chiara Zampolli");
  AliCDBId id2(out,minrun,maxrun);
  man->Put(fTOFSimToT,id2,mdhisto);
  delete md;
  delete mdhisto;
}
//_____________________________________________________________________________
void AliTOFcalib::ReadSimParOnlineFromCDB(Char_t *sel, Int_t nrun)
{
  //Read miscalibration parameters from the CDB
  AliCDBManager *man = AliCDBManager::Instance();

  // The Slewing Pars

  Char_t *sel1 = "SimParOnline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  if (!man->Get(out,nrun)) { 
    AliFatal("Exiting, no CDB object (SimPar) found!!!");
    exit(0);  
  }
  AliCDBEntry *entry1 = man->Get(out,nrun);
  if(!entry1->GetObject()){
    AliFatal("Exiting, no CDB object (SimPar) found!!!");
    exit(0);  
  }  
  TObjArray *cal =(TObjArray*)entry1->GetObject();
  fTOFSimCalOnline=cal;

}
//_____________________________________________________________________________
void AliTOFcalib::ReadSimParOfflineFromCDB(Char_t *sel, Int_t nrun)
{
  //Read miscalibration parameters from the CDB
  AliCDBManager *man = AliCDBManager::Instance();

  // The Slewing Pars

  Char_t *sel1 = "SimParOffline" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  if (!man->Get(out,nrun)) { 
    AliFatal("Exiting, no CDB object (SimPar) found!!!");
    exit(0);  
  }
  AliCDBEntry *entry1 = man->Get(out,nrun);
  if(!entry1->GetObject()){
    AliFatal("Exiting, no CDB object (SimPar) found!!!");
    exit(0);  
  }  
  TObjArray *cal =(TObjArray*)entry1->GetObject();
  AliCDBMetaData *md = (AliCDBMetaData*)entry1->GetMetaData();
  fkValidity = md->GetComment();
  fTOFSimCalOffline=cal;

  // The Tot Histo

  Char_t *sel2 = "SimHisto" ;
  sprintf(out,"%s/%s",sel,sel2); 
  if (!man->Get(out,nrun)) { 
    AliFatal("Exiting, no CDB object (SimHisto) found!!!");
    exit(0);  
  }
  AliCDBEntry *entry2 = man->Get(out,nrun);
  if(!entry2->GetObject()){
    AliFatal("Exiting, no CDB object (SimHisto) found!!!");
    exit(0);  
  }  
  TH1F *histo =(TH1F*)entry2->GetObject();
  fTOFSimToT=histo;
}
//_____________________________________________________________________________
void AliTOFcalib::WriteRecParOnCDB(Char_t *sel, Int_t minrun, Int_t maxrun, AliTOFRecoParam *param){
  //Write reconstruction parameters to the CDB

  AliCDBManager *man = AliCDBManager::Instance();
  AliCDBMetaData *md = new AliCDBMetaData();
  md->SetResponsible("Silvia Arcelli");
  Char_t *sel1 = "RecPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId id(out,minrun,maxrun);
  man->Put(param,id,md);
  delete md;
}
//_____________________________________________________________________________
AliTOFRecoParam * AliTOFcalib::ReadRecParFromCDB(Char_t *sel, Int_t nrun)
{
  //Read reconstruction parameters from the CDB
  AliCDBManager *man = AliCDBManager::Instance();
  Char_t *sel1 = "RecPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  if (!man->Get(out,nrun)) { 
    AliFatal("Exiting, no CDB object (RecPar) found!!!");
    exit(0);  
  }  
  AliCDBEntry *entry = man->Get(out,nrun);
  if(!entry->GetObject()){
    AliFatal("Exiting, no CDB object (RecPar) found!!!");
    exit(0);  
  }  

  AliTOFRecoParam *param=(AliTOFRecoParam*)entry->GetObject();
  return param;
}
//-----------------------------------------------------------------------------
// Calibration methods
//-----------------------------------------------------------------------------
void AliTOFcalib::CreateTreeFromCDB(Int_t minrun, Int_t maxrun){

  // creating the chain with the trees for calibration
  // collecting them from reference data 
  // from minrun to maxrun

  Float_t p[CHENTRIESSMALL];
  Int_t nentries;
  fTree = new TTree("TOFCalib","Tree for TOF Calibration");
  fTree->Branch("nentries",&nentries,"nentries/I");
  fTree->Branch("TOFentries",p,"TOFentries[nentries]/F");
  AliCDBManager *man = AliCDBManager::Instance();
  AliCDBStorage *aStorage = man->GetStorage("local://$ALICE_ROOT");
  for (Int_t irun = minrun;irun<=maxrun;irun++){
    AliCDBEntry *entry = aStorage->Get("TOF/RefData/TreeForCalib",irun);
    if (!entry){
      AliInfo(Form("No entry found for run %i",irun));
    }
    else{
      TTree *tree = new TTree();
      tree = (TTree*)entry->GetObject();
      tree->SetBranchAddress("nentries",&nentries);
      tree->SetBranchAddress("TOFentries",p);      
      fTree->CopyEntries(tree);
      delete tree;
      fNruns++;
    }
  }
  AliInfo(Form("Number of runs being analyzed %i",fNruns));
}
//-----------------------------------------------------------------------------
void AliTOFcalib::CreateTreeFromGrid(Int_t minrun, Int_t maxrun){

  // creating the chain with the trees for calibration
  // collecting them from the Grid 
  // from minrun to maxrun

  Float_t p[CHENTRIESSMALL];
  Int_t nentries;
  fTree = new TTree("TOFCalib","Tree for TOF Calibration");
  fTree->SetDirectory(0);
  fTree->Branch("nentries",&nentries,"nentries/I");
  fTree->Branch("TOFentries",p,"TOFentries[nentries]/F");
  AliInfo("connected to alien");
  TGrid::Connect("alien://");
  
  Char_t filename[100];
  for (Int_t irun = minrun;irun<=maxrun;irun++){
    sprintf(filename,"alien:///alice/cern.ch/user/c/czampolli/TOFCalibReference_%i.root",irun);
    TFile *filegrid = TFile::Open(filename,"READ");
    TTree *tree = (TTree*)filegrid->Get("T");
    tree->SetBranchAddress("nentries",&nentries);
    tree->SetBranchAddress("TOFentries",p);      
    fTree->CopyEntries(tree);
    delete tree;
    fNruns++;    
  }
  
  AliInfo(Form("Number of runs being analyzed %i",fNruns));
}
//-----------------------------------------------------------------------------
void AliTOFcalib::CreateTreeFromFile(Int_t minrun, Int_t maxrun){

  // creating the tree with the trees for calibration
  // collecting them from reference data (from file)
  // from minrun to maxrun

  Float_t p[CHENTRIESSMALL];
  Int_t nentries;
  fTree = new TTree("TOFCalib","Tree for TOF Calibration");
  fTree->SetDirectory(0);
  fTree->Branch("nentries",&nentries,"nentries/I");
  fTree->Branch("TOFentries",p,"TOFentries[nentries]/F");
  Char_t filename[100];
  for (Int_t irun = minrun;irun<=maxrun;irun++){
    sprintf(filename,"$ALICE_ROOT/TOF/RefData/TreeForCalib/fileout_%i.root",irun);
    TFile *file = new TFile(filename,"READ");
    TTree *tree = (TTree*)file->Get("T");
    tree->SetBranchAddress("nentries",&nentries);
    tree->SetBranchAddress("TOFentries",p);      
    fTree->CopyEntries(tree);
    delete tree;
    delete file;
    file = 0x0;
    fNruns++;
  }

  AliInfo(Form("Number of runs being analyzed %i",fNruns));
}
//-----------------------------------------------------------------------------
Int_t AliTOFcalib::Calibrate(Int_t ichmin, Int_t ichmax, Option_t *optionSave, Option_t *optionFit){

  // calibrating summing more than one channels
  // computing calibration parameters
  // Returning codes:
  // 0 -> everything was ok
  // 1 -> no tree for calibration found
  // 2 -> not enough statistics to perform calibration
  // 3 -> problems with arrays
  
  TH1::AddDirectory(0);

  AliInfo(Form("*** Calibrating Histograms %s, summing more channels, from channel %i, to channel %i, storing Calib Pars in channel %i", GetName(),ichmin,ichmax,ichmin)) ; 
  AliInfo(Form("Option for Saving histos = %s",optionSave )) ; 
  AliInfo(Form("Option for Fitting Profile histos = %s",optionFit )) ; 

  Float_t p[CHENTRIESSMALL];
  Int_t nentries;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  Float_t ntracksTotalmean =0;
  for (Int_t i=ichmin; i<ichmax; i++){
    Int_t ientry = -1;
    for (Int_t irun=0;irun<fNruns;irun++){
      ientry = i+irun*fNChannels;
      fTree->GetEntry(ientry);
      Int_t ntracksRun=nentries/3;
      ntracksTotalmean+=ntracksRun;
    }
  }
  
  if (ntracksTotalmean < MEANENTRIES) {
    AliInfo(Form(" Too small mean number of entires per channel (mean number = %f) not calibrating and exiting.....",ntracksTotalmean));
    return 2;
  }

  //filling ToT and Time arrays

  Int_t nbinToT = 100;  // ToT bin width in Profile = 48.8 ps 
  Float_t minToT = 0;   // ns
  Float_t maxToT = 4.88;  // ns

  TH1F *hToT = new TH1F("htot","htot",nbinToT, minToT, maxToT);
  TH1F *hdeltaTime = new TH1F("hdeltaTime","hdeltaTime",200,2,4);
  Int_t ntracksTotal = 0;
  Int_t ntracksRun = 0;
  Double_t binsProfile[101]; // sized larger than necessary, the correct 
                             // dim being set in the booking of the profile
  Int_t nusefulbins=0;
  Float_t meantime=0;
  for (Int_t i = ichmin;i<ichmax;i++){
    Int_t ientry = -1;
    for (Int_t irun=0;irun<fNruns;irun++){
      ientry = i+irun*fNChannels;
      fTree->GetEntry(ientry);
      ntracksTotal+=nentries/3;
      ntracksRun=nentries/3;
      AliDebug(2,Form("run %i, channel %i, nentries = %i, ntracks = %i",irun,i,nentries, ntracksRun));
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	hdeltaTime->Fill(p[idxexTime]-p[idxexExTime]);
	meantime+=p[idxexTime]-p[idxexExTime];
	hToT->Fill(tot);
      }
    }
  }
  nusefulbins = FindBins(hToT,&binsProfile[0]);
  meantime/=ntracksTotal;
  AliDebug(2, Form("meantime = %f",meantime));
  
  for (Int_t j=1;j<=nusefulbins;j++) {
    AliDebug(2,Form(" summing channels from %i to %i, nusefulbins = %i, bin %i = %f",ichmin,ichmax,nusefulbins,j,binsProfile[j])); 
  }

  TProfile* hSlewingProf = new TProfile("hSlewingProf", "hSlewingProf",nusefulbins, binsProfile, "G");  // CHECK THE BUILD OPTION, PLEASE!!!!!!
  TH2F * htimetot = new TH2F("htimetot","htimetot",nbinToT, minToT, maxToT,600,-5,10);

  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t ientry = -1;
    for (Int_t i=ichmin; i<ichmax; i++){
      ientry = i+irun*fNChannels;
      fTree->GetEntry(ientry);
      ntracksRun=nentries/3;
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	Float_t time = p[idxexTime]-p[idxexExTime];
	AliDebug (2, Form("track = %i, time = %f, tot = %f, time-meantime = %f",j,time, tot, time-meantime));
	hSlewingProf->Fill(tot,time);  // if meantime is not used, the fill may be moved in the loop above
	htimetot->Fill(tot,time-meantime);  // if meantime is not used, the fill may be moved in the loop above
      }
    }
  }

  hSlewingProf->Fit("pol5",optionFit,"",0,4);
  TF1 * calibfunc = (TF1*)hSlewingProf->GetFunction("pol5");
  Float_t par[6];    
  for(Int_t kk=0;kk<6;kk++){
    par[kk]=calibfunc->GetParameter(kk);
    AliDebug(2,Form("parameter %i = %f",kk,par[kk]));
  }

  if(strstr(optionSave,"save")){
    TFile * fileProf = new TFile("TOFCalibSave.root","recreate");
    fileProf->cd(); 
    TString profName=Form("Profile%06i_%06i",ichmin,ichmax);
    TString timeTotName=Form("TimeTot%06i_%06i",ichmin,ichmax);
    TString totName=Form("Tot%06i_%06i",ichmin,ichmax);
    TString deltaName=Form("Delta%06i_%06i",ichmin,ichmax);
    hSlewingProf->Write(profName);
    htimetot->Write(timeTotName);
    hToT->Write(totName);
    hdeltaTime->Write(deltaName);
    fileProf->Close();
    delete fileProf;
    fileProf=0x0;
  }

  delete hToT;
  hToT=0x0;
  delete hSlewingProf;
  hSlewingProf=0x0;
  delete htimetot;
  htimetot=0x0;
  delete hdeltaTime;
  hdeltaTime=0x0;

  AliTOFChannelOffline * calChannel = (AliTOFChannelOffline*)fTOFCalOffline->At(ichmin);
  calChannel->SetSlewPar(par);
  WriteParOfflineOnCDB("TOF/Calib","valid",0,0);
  return 0;
}
//----------------------------------------------------------------------------
Int_t AliTOFcalib::Calibrate(Int_t i, Option_t *optionSave, Option_t *optionFit){

  // computing calibration parameters for channel i
  // Returning codes:
  // 0 -> everything was ok
  // 1 -> no tree for calibration found
  // 2 -> not enough statistics to perform calibration
  // 3 -> problems with arrays

  TH1::AddDirectory(0);
  
  AliInfo(Form("*** Calibrating Histograms (one channel) %s", GetName())) ; 
  AliInfo(Form("Option for Saving histos = %s",optionSave )) ; 
  AliInfo(Form("Option for Fitting Profile histos = %s",optionFit )) ; 

  Float_t p[MAXCHENTRIESSMALL];
  Int_t nentries;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  Float_t ntracksTotal =0;
  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t ientry = -1;
    ientry = i+irun*fNChannels;
    fTree->GetEntry(ientry);
    ntracksTotal+=nentries/3;    
  }
  
  if (ntracksTotal < MEANENTRIES) {  
    AliInfo(Form(" Too small mean number of entires per channel (mean number = %f) not calibrating and exiting.....",ntracksTotal));
    return 2;
  }

  //filling ToT and Time arrays

  Int_t nbinToT = 100;  // ToT bin width in Profile = 48.8 ps 
  Float_t minToT = 0;   // ns
  Float_t maxToT = 4.88;  // ns

  TH1F *hToT = new TH1F("htot","htot",nbinToT, minToT, maxToT);
  TH1F *hdeltaTime = new TH1F("hdeltaTime","hdeltaTime",200,2,4);
  Int_t ntracksRun = 0;
  Double_t binsProfile[101]; // sized larger than necessary, the correct 
                             // dim being set in the booking of the profile
  Int_t nusefulbins=0;
  Float_t meantime=0;
  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t ientry = -1;
    ientry = i+irun*fNChannels;
    fTree->GetEntry(ientry);
    ntracksRun=nentries/3;
    AliDebug(2,Form("run %i, channel %i, nentries = %i, ntracksRun = %i",irun, i ,nentries, ntracksRun));
    for (Int_t j=0;j<ntracksRun;j++){
      Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
      Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
      Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
      Float_t tot = p[idxexToT];
      meantime+=p[idxexTime]-p[idxexExTime];
      hdeltaTime->Fill(p[idxexTime]-p[idxexExTime]);
      hToT->Fill(tot);
    }
  }

  nusefulbins = FindBins(hToT,&binsProfile[0]);
  meantime/=ntracksTotal;
  AliDebug(2,Form("meantime = %f",meantime));
  
  for (Int_t j=1;j<=nusefulbins;j++) {
    AliDebug(2,Form(" channel %i, nusefulbins = %i, bin %i = %f",i,nusefulbins,j,binsProfile[j])); 
  }

  TProfile* hSlewingProf = new TProfile("hSlewingProf", "hSlewingProf",nusefulbins, binsProfile, "G");  // CHECK THE BUILD OPTION, PLEASE!!!!!!
  TH2F * htimetot = new TH2F("htimetot","htimetot",nbinToT, minToT, maxToT,600,-5,10);
  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t ientry = -1;
    ientry = i+irun*fNChannels;
    fTree->GetEntry(ientry);
    ntracksRun=nentries/3;
    for (Int_t j=0;j<ntracksRun;j++){
      Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
      Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
      Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
      Float_t tot = p[idxexToT];
      Float_t time = p[idxexTime]-p[idxexExTime];
      AliDebug (2,Form("track = %i, time = %f, tot = %f, time-meantime = %f",j,time, tot, time-meantime));
      hSlewingProf->Fill(tot,time);  // if meantime is not used, the fill may be moved in the loop above
      htimetot->Fill(tot,time-meantime);  // if meantime is not used, the fill may be moved in the loop above
    }
  }

  hSlewingProf->Fit("pol5",optionFit,"",0,4);
  TF1 * calibfunc = (TF1*)hSlewingProf->GetFunction("pol5");
  Float_t par[6];    
  for(Int_t kk=0;kk<6;kk++){
    par[kk]=calibfunc->GetParameter(kk);
    AliDebug(2,Form("parameter %i = %f",kk,par[kk]));
  }


  if(strstr(optionSave,"save")){
    TFile * fileProf = new TFile("TOFCalibSave.root","recreate");
    fileProf->cd();   
    TString profName=Form("Profile%06i",i);
    TString timeTotName=Form("TimeTot%06i",i);
    TString totName=Form("Tot%06i",i);
    TString deltaName=Form("Delta%06i",i);
    hSlewingProf->Write(profName);
    htimetot->Write(timeTotName);
    hToT->Write(totName);
    hdeltaTime->Write(deltaName);
    fileProf->Close();
    delete fileProf;
    fileProf=0x0;
  }

  delete hToT;
  hToT=0x0; 
  delete hSlewingProf;
  hSlewingProf=0x0;
  delete htimetot;
  htimetot=0x0;
  delete hdeltaTime;
  hdeltaTime=0x0;

  AliTOFChannelOffline * calChannel = (AliTOFChannelOffline*)fTOFCalOffline->At(i);
  calChannel->SetSlewPar(par);
  WriteParOfflineOnCDB("TOF/Calib","valid",0,0);
  return 0;
}
//----------------------------------------------------------------------------
Int_t AliTOFcalib::Calibrate(Int_t nch, Int_t *ch, Option_t *optionSave, Option_t *optionFit){

  // calibrating an array of channels
  // computing calibration parameters
  // Returning codes:
  // 0 -> everything was ok
  // 1 -> no tree for calibration found
  // 2 -> not enough statistics to perform calibration
  // 3 -> problems with arrays
  
  TH1::AddDirectory(0);

  AliInfo(Form("*** Calibrating Histograms %s, number of channels = %i", GetName(),nch)) ; 
  AliInfo(Form("Option for Saving histos = %s",optionSave )) ; 
  AliInfo(Form("Option for Fitting Profile histos = %s",optionFit )) ; 
  for (Int_t ich=0; ich<nch; ich++){
    Int_t i = ch[ich];
    AliInfo(Form("Calibrating channel = %i",i )) ; 
  }
  Float_t p[MAXCHENTRIESSMALL];
  Int_t nentries;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  Float_t ntracksTotalmean =0;
  for (Int_t ich=0; ich<nch; ich++){
    Int_t ientry = -1;
      Int_t i = ch[ich];
      for (Int_t irun=0;irun<fNruns;irun++){
      ientry = i+irun*fNChannels;
      fTree->GetEntry(ientry);
      ntracksTotalmean+=nentries/3;
    }
  }

  ntracksTotalmean/=nch;
  if (ntracksTotalmean < MEANENTRIES) { 
    AliInfo(Form(" Too small mean number of entires per channel (mean number = %f) not calibrating and exiting.....",ntracksTotalmean));
    return 2;
  }

  //filling ToT and Time arrays

  Int_t nbinToT = 100;  // ToT bin width in Profile = 48.8 ps 
  Float_t minToT = 0;   // ns
  Float_t maxToT = 4.88;  // ns
  TFile * fileProf=0x0;
  if(strstr(optionSave,"save")){
    fileProf = new TFile("TOFCalibSave.root","recreate");
  }
  for (Int_t ich=0; ich<nch; ich++) {
    TH1F *hToT = new TH1F("htot","htot",nbinToT, minToT, maxToT);
    TH1F *hdeltaTime = new TH1F("hdeltaTime","hdeltaTime",200,2,4);
    Double_t binsProfile[101]; // sized larger than necessary, the correct 
    // dim being set in the booking of the profile
    TH2F * htimetot = new TH2F("htimetot","htimetot",nbinToT, minToT, maxToT,600,-5,10);
    Int_t ntracksTotal = 0;
    Int_t ntracksRun = 0;
    Int_t nusefulbins=0;
    Float_t meantime=0;
    Int_t i=-1;
    for (Int_t irun=0;irun<fNruns;irun++){
      i = ch[ich]+irun*fNChannels;
      AliDebug(2,Form("Calibrating channel %i",i));
      fTree->GetEntry(i);
      ntracksTotal+=nentries/3;
    }
    if (ntracksTotal < MEANENTRIES) {
      AliInfo(Form(" Too small mean number of entires in channel %i (number of tracks = %f), not calibrating channel and continuing.....",i,ntracksTotal));
      continue;
    }
  
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ch[ich]+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksRun=nentries/3;
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	hdeltaTime->Fill(p[idxexTime]-p[idxexExTime]);
	meantime+=p[idxexTime]-p[idxexExTime];
	hToT->Fill(tot);
      }
    }

    nusefulbins = FindBins(hToT,&binsProfile[0]);
    meantime/=ntracksTotal;
    for (Int_t j=1;j<=nusefulbins;j++) {
      AliDebug(2,Form(" channel %i, nusefulbins = %i, bin %i = %f",i,nusefulbins,j,binsProfile[j])); 
    }

    TProfile* hSlewingProf = new TProfile("hSlewingProf", "hSlewingProf",nusefulbins, binsProfile, "G");  // CHECK THE BUILD OPTION, PLEASE!!!!!!
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ch[ich]+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksRun=nentries/3;
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	Float_t time = p[idxexTime]-p[idxexExTime];
	AliDebug(2,Form("track = %i, time = %f, tot = %f, time-meantime = %f",j,time, tot, time-meantime));
	hSlewingProf->Fill(tot,time);  // if meantime is not used, the fill may be moved in the loop above
	htimetot->Fill(tot,time-meantime);  // if meantime is not used, the fill may be moved in the loop above
      }
    }
    
    hSlewingProf->Fit("pol5",optionFit,"",1,4);
    TF1 * calibfunc = (TF1*)hSlewingProf->GetFunction("pol5");
    Float_t par[6];    
    for(Int_t kk=0;kk<6;kk++){
      par[kk]=calibfunc->GetParameter(kk);
      AliDebug(2,Form("parameter %i = %f",kk,par[kk]));
    }
    
    if(strstr(optionSave,"save") && fileProf){
      TString profName=Form("Profile%06i",i);
      TString timeTotName=Form("TimeTot%06i",i);
      TString totName=Form("Tot%06i",i);
      TString deltaName=Form("Delta%06i",i);
      fileProf->cd();
      hSlewingProf->Write(profName);
      htimetot->Write(timeTotName);
      hToT->Write(totName);
      hdeltaTime->Write(deltaName);
    }

    AliTOFChannelOffline * calChannel = (AliTOFChannelOffline*)fTOFCalOffline->At(i);
    calChannel->SetSlewPar(par);
    delete hToT;
    hToT=0x0;
    delete hSlewingProf;
    hSlewingProf=0x0;
    delete htimetot;
    htimetot=0x0;
    delete hdeltaTime;
    hdeltaTime=0x0;
  }

  if(strstr(optionSave,"save") && fileProf){
    fileProf->Close();
    delete fileProf;
    fileProf=0x0;
  }
  WriteParOfflineOnCDB("TOF/Calib","valid",0,0);

  return 0;
}
//----------------------------------------------------------------------------
Int_t AliTOFcalib::CalibrateFromProfile(Int_t ich, Option_t *optionSave, Option_t *optionFit){

  // computing calibration parameters using the old profiling algo
  // Returning codes:
  // 0 -> everything was ok
  // 1 -> no tree for calibration found
  // 2 -> not enough statistics to perform calibration
  // 3 -> problems with arrays

  TH1::AddDirectory(0);

  AliInfo(Form("*** Calibrating Histograms From Profile %s", GetName())) ; 
  AliInfo(Form("Option for Saving histos = %s",optionSave )) ; 
  AliInfo(Form("Option for Fitting Profile histos = %s",optionFit )) ; 
  Float_t p[MAXCHENTRIESSMALL];
  Int_t nentries;
  Int_t ntracksTotal=0;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t i = ich+irun*fNChannels;
    fTree->GetEntry(i);
    ntracksTotal+=nentries/3;
  }

  if (ntracksTotal < MEANENTRIES) {  
    AliInfo(Form(" Too small mean number of entires per channel (mean number = %f) not calibrating and exiting.....",ntracksTotal));
    return 2;
  }

  TH1F * hProf = new TH1F();
  hProf = Profile(ich);
  hProf->Fit("pol5",optionFit,"",0,4);
  TF1 * calibfunc = (TF1*)hProf->GetFunction("pol5");
  Float_t par[6];    
  for(Int_t kk=0;kk<6;kk++){
    par[kk]=calibfunc->GetParameter(kk);
    AliDebug(2,Form("parameter %i = %f",kk,par[kk]));
  }

  if(strstr(optionSave,"save")){
    TFile * fileProf = new TFile("TOFCalibSave.root","recreate");
    fileProf->cd(); 
    TString profName=Form("Profile%06i",ich);
    hProf->Write(profName);
    fileProf->Close();
    delete fileProf;
    fileProf=0x0;
  }

  delete hProf;
  hProf=0x0;
  AliTOFChannelOffline * calChannel = (AliTOFChannelOffline*)fTOFCalOffline->At(ich);
  calChannel->SetSlewPar(par);
  WriteParOfflineOnCDB("TOF/Calib","valid",0,0);
  return 0;
}
//----------------------------------------------------------------------------
Int_t AliTOFcalib::Calibrate(Option_t *optionSave, Option_t *optionFit){

  // calibrating the whole TOF
  // computing calibration parameters
  // Returning codes:
  // 0 -> everything was ok
  // 1 -> no tree for calibration found
  // 2 -> not enough statistics to perform calibration
  // 3 -> problems with arrays

  TH1::AddDirectory(0);

  AliInfo(Form("*** Calibrating Histograms %s, all channels", GetName())) ; 
  AliInfo(Form("Option for Saving histos = %s",optionSave )) ; 
  AliInfo(Form("Option for Fitting Profile histos = %s",optionFit )) ; 

  TFile * fileProf=0x0;
  if(strstr(optionSave,"save")){
    fileProf = new TFile("TOFCalibSave.root","recreate");
  }

  Float_t p[MAXCHENTRIESSMALL];
  Int_t nentries;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  Float_t ntracksTotalmean =0;
  for (Int_t ii=0; ii<fNChannels; ii++){
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ii+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksTotalmean+=nentries/3;
    }
  }

  ntracksTotalmean/=fNChannels;
  if (ntracksTotalmean < MEANENTRIES) {
    AliInfo(Form(" Too small mean number of entires per channel (mean number = %f) not calibrating and exiting.....",ntracksTotalmean));
    return 2;
  }

  //filling ToT and Time arrays

  Int_t nbinToT = 100;  // ToT bin width in Profile = 50.0 ps 
  Float_t minToT = 0;   // ns
  Float_t maxToT = 4.88;// ns
  for (Int_t ii=0; ii<fNChannels; ii++) {
    TH1F *hToT = new TH1F("htot","htot",nbinToT, minToT, maxToT);
    TH1F *hdeltaTime = new TH1F("hdeltaTime","hdeltaTime",200,2,4);
    TH2F * htimetot = new TH2F("htimetot","htimetot",nbinToT, minToT, maxToT,600,-5,10);
    if (ii%1000 == 0) AliDebug(1,Form("Calibrating channel %i ",ii));
    //Int_t i = 3;
    Int_t nusefulbins=0;
    Double_t binsProfile[101]; // sized larger than necessary, the correct 
                              // dim being set in the booking of the profile
    Int_t ntracksRun = 0;
    Int_t ntracksTotal = 0;
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ii+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksTotal+=nentries/3;
    }
    if (ntracksTotal < MEANENTRIES) {
      AliInfo(Form(" Too small mean number of entires in channel %i (number of tracks = %f), not calibrating channel and continuing.....",ii,ntracksTotal));
      continue;
    }
    Float_t meantime=0;
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ii+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksRun=nentries/3;
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	hdeltaTime->Fill(p[idxexTime]-p[idxexExTime]);
	meantime+=p[idxexTime]-p[idxexExTime];
	hToT->Fill(tot);
      }
    }
    nusefulbins = FindBins(hToT,&binsProfile[0]);
    meantime/=ntracksTotal;
    for (Int_t j=0;j<nusefulbins;j++) {
      AliDebug(2,Form(" channel %i, usefulbin = %i, low edge = %f",ii,j,binsProfile[j])); 
    }
    TProfile* hSlewingProf = new TProfile("hSlewingProf", "hSlewingProf",nusefulbins, binsProfile, "G");  // CHECK THE BUILD OPTION, PLEASE!!!!!!
    for (Int_t irun=0;irun<fNruns;irun++){
      Int_t i = ii+irun*fNChannels;
      fTree->GetEntry(i);
      ntracksRun=nentries/3;
      for (Int_t j=0;j<ntracksRun;j++){
	Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
	Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
	Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
	Float_t tot = p[idxexToT];
	Float_t time = p[idxexTime]-p[idxexExTime];
	AliDebug (2,Form("track = %i, time = %f, tot = %f, time-meantime = %f",j,time, tot, time-meantime));
	hSlewingProf->Fill(tot,time);  // if meantime is not used, the fill may be moved in the loop above
	htimetot->Fill(tot,time-meantime);  // if meantime is not used, the fill may be moved in the loop above
      }
    }
    hSlewingProf->Fit("pol5",optionFit,"",1,4);
    TF1 * calibfunc = (TF1*)hSlewingProf->GetFunction("pol5");
    Float_t par[6];    
    for(Int_t kk=0;kk<6;kk++){
      par[kk]=calibfunc->GetParameter(kk);
      AliDebug(2,Form("parameter %i = %f",kk,par[kk]));
    }

    if(strstr(optionSave,"save") && fileProf){
      TString profName=Form("Profile%06i",ii);
      TString timeTotName=Form("TimeTot%06i",ii);
      TString totName=Form("Tot%06i",ii);
      TString deltaName=Form("Delta%06i",ii);
      fileProf->cd();
      hSlewingProf->Write(profName);
      htimetot->Write(timeTotName);
      hToT->Write(totName);
      hdeltaTime->Write(deltaName);
    }
    AliTOFChannelOffline * calChannel = (AliTOFChannelOffline*)fTOFCalOffline->At(ii);
    calChannel->SetSlewPar(par);

    delete hToT;
    hToT=0x0;
    delete hSlewingProf;
    hSlewingProf=0x0;
    delete htimetot;
    htimetot=0x0;
    delete hdeltaTime;
    hdeltaTime=0x0;
  }

  if(strstr(optionSave,"save")){
    fileProf->Close();
    delete fileProf;
    fileProf=0x0;
  }
  WriteParOfflineOnCDB("TOF/Calib","valid",0,0);
  return 0;
}

//-----------------------------------------------------------------------
TH1F* AliTOFcalib::Profile(Int_t ich)
{
  // profiling algo

  Float_t p[MAXCHENTRIESSMALL];
  Int_t nentries;
  fTree->SetBranchAddress("nentries",&nentries);
  fTree->SetBranchAddress("TOFentries",p);

  //Prepare histograms for Slewing Correction
  const Int_t knbinToT = 100;
  Int_t nbinTime = 200;
  Float_t minTime = -5.5; //ns
  Float_t maxTime = 5.5; //ns
  Float_t minToT = 0; //ns
  Float_t maxToT = 5.; //ns
  Float_t deltaToT = (maxToT-minToT)/knbinToT;
  Double_t mTime[knbinToT+1],mToT[knbinToT+1],meanTime[knbinToT+1], meanTime2[knbinToT+1],vToT[knbinToT+1], vToT2[knbinToT+1],meanToT[knbinToT+1],meanToT2[knbinToT+1],vTime[knbinToT+1],vTime2[knbinToT+1],xlow[knbinToT+1],sigmaTime[knbinToT+1];
  Int_t n[knbinToT+1], nentrx[knbinToT+1];
  Double_t sigmaToT[knbinToT+1];
  for (Int_t i = 0; i < knbinToT+1 ; i++){
    mTime[i]=0;
    mToT[i]=0;
    n[i]=0;
    meanTime[i]=0;
    meanTime2[i]=0;
    vToT[i]=0;
    vToT2[i]=0;
    meanToT[i]=0;
    meanToT2[i]=0;
    vTime[i]=0;
    vTime2[i]=0;
    xlow[i]=0;
    sigmaTime[i]=0;
    sigmaToT[i]=0;
    n[i]=0;
    nentrx[i]=0;
  }
  TH2F* hSlewing = new TH2F("hSlewing", "hSlewing", knbinToT, minToT, maxToT, nbinTime, minTime, maxTime);
  Int_t ntracksRun = 0;
  TH1F *histo = new TH1F("histo", "1D Time vs ToT", knbinToT, minToT, maxToT);
  for (Int_t irun=0;irun<fNruns;irun++){
    Int_t i = ich+irun*fNChannels;
    fTree->GetEntry(i);
    ntracksRun=nentries/3;
    for (Int_t j=0;j<ntracksRun;j++){
      Int_t idxexToT = (j* NIDXSMALL)+DELTAIDXTOT; 
      Int_t idxexTime = (j* NIDXSMALL)+DELTAIDXTIME; 
      Int_t idxexExTime = (j* NIDXSMALL)+DELTAIDXPID; 
      Float_t tot = p[idxexToT];
      Float_t time = p[idxexTime]-p[idxexExTime];
      Int_t nx = (Int_t)((tot-minToT)/deltaToT)+1;
      if ((tot != 0) && ( time!= 0)){
	vTime[nx]+=time;
	vTime2[nx]+=time*time;
	vToT[nx]+=tot;
	vToT2[nx]+=tot*tot;
	nentrx[nx]++;
	hSlewing->Fill(tot,time);
      }
    }
  }
  Int_t nbinsToT=hSlewing->GetNbinsX();
  if (nbinsToT != knbinToT) {
    AliError("Profile :: incompatible numbers of bins");
    return 0x0;
  }
  
  Int_t usefulBins=0;
  for (Int_t i=1;i<=nbinsToT;i++){
    if (nentrx[i]!=0){
      n[usefulBins]+=nentrx[i];
      if (n[usefulBins]==0 && i == nbinsToT) {
	break;
      }
      meanTime[usefulBins]+=vTime[i];
      meanTime2[usefulBins]+=vTime2[i];
      meanToT[usefulBins]+=vToT[i];
      meanToT2[usefulBins]+=vToT2[i];
      if (n[usefulBins]<10 && i!=nbinsToT) continue; 
      mTime[usefulBins]=meanTime[usefulBins]/n[usefulBins];
      mToT[usefulBins]=meanToT[usefulBins]/n[usefulBins];
      sigmaTime[usefulBins]=TMath::Sqrt(1./n[usefulBins]/n[usefulBins]
			    *(meanTime2[usefulBins]-meanTime[usefulBins]
			    *meanTime[usefulBins]/n[usefulBins]));
      if ((1./n[usefulBins]/n[usefulBins]
	   *(meanToT2[usefulBins]-meanToT[usefulBins]
	     *meanToT[usefulBins]/n[usefulBins]))< 0) {
	AliError(" too small radical" );
	sigmaToT[usefulBins]=0;
      }
      else{       
	sigmaToT[usefulBins]=TMath::Sqrt(1./n[usefulBins]/n[usefulBins]
			     *(meanToT2[usefulBins]-meanToT[usefulBins]
			     *meanToT[usefulBins]/n[usefulBins]));
      }
      usefulBins++;
    }
  }
  for (Int_t i=0;i<usefulBins;i++){
    Int_t binN = (Int_t)((mToT[i]-minToT)/deltaToT)+1;
    histo->Fill(mToT[i],mTime[i]);
    histo->SetBinError(binN,sigmaTime[i]);
  } 
  delete hSlewing;
  hSlewing=0x0;

  return histo;
}
//----------------------------------------------------------------------------
Int_t AliTOFcalib::FindBins(TH1F* h, Double_t *binsProfile) const{

  // to determine the bins for ToT histo

  Int_t cont = 0;
  Int_t startBin = 1;
  Int_t nbin = h->GetNbinsX();
  Int_t nentries = (Int_t)h->GetEntries();
  Float_t max = h->GetBinLowEdge(nbin);
  Int_t nusefulbins=0;
  Int_t maxcont=0;
  // setting maxvalue of entries per bin
  if (nentries <= 60) maxcont = 2;
  else  if (nentries <= 100) maxcont = 5;
  else  if (nentries <= 500) maxcont = 10;
  else  maxcont = 20;
  for (Int_t j=1;j<=nbin;j++) {
    cont += (Int_t)h->GetBinContent(j);
    if (j<nbin){
      if (cont>=maxcont){
	nusefulbins++;
	binsProfile[nusefulbins-1]=h->GetBinLowEdge(startBin);
	cont=0;
	startBin=j+1;
	continue;
      }
    }
    else{
      if (cont>=maxcont){
	nusefulbins++;
	binsProfile[nusefulbins-1]=h->GetBinLowEdge(startBin);
	binsProfile[nusefulbins]=max;
      }
      else {
	binsProfile[nusefulbins]=h->GetBinLowEdge(startBin);
      }
    }
  }
  return nusefulbins;
}

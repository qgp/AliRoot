 /**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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
///////////////////////////////////////////////////////////////////////////
//  Plane Efficiency class for ITS                      
//  It is used for module by module efficiency of the SSD,        
//  evaluated by tracks
//  (Inherits from AliITSPlaneEff)
//  Author: G.E. Bruno 
//          giuseppe.bruno@ba.infn.it
//
///////////////////////////////////////////////////////////////////////////

/*  $Id$ */

#include <TMath.h>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include "AliITSPlaneEffSSD.h"
#include "AliLog.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
//#include "AliCDBRunRange.h"
#include "AliITSCalibrationSSD.h"

ClassImp(AliITSPlaneEffSSD)	
//______________________________________________________________________
AliITSPlaneEffSSD::AliITSPlaneEffSSD():
  AliITSPlaneEff(),
  fHisResX(0),
  fHisResZ(0),
  fHisResXZ(0),
  fHisClusterSize(0){
  for (UInt_t i=0; i<kNModule; i++){
    fFound[i]=0;
    fTried[i]=0;
  }
  // default constructor
  AliDebug(1,Form("Calling default constructor"));
}
//______________________________________________________________________
AliITSPlaneEffSSD::~AliITSPlaneEffSSD(){
    // destructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //     none.
    DeleteHistos();
}
//______________________________________________________________________
AliITSPlaneEffSSD::AliITSPlaneEffSSD(const AliITSPlaneEffSSD &s) : AliITSPlaneEff(s),
fHisResX(0),
fHisResZ(0),
fHisResXZ(0),
fHisClusterSize(0)
{
    //     Copy Constructor
    // Inputs:
    //    AliITSPlaneEffSSD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:

 for (UInt_t i=0; i<kNModule; i++){
    fFound[i]=s.fFound[i];
    fTried[i]=s.fTried[i];
 }
 if(fHis) {
   InitHistos();
   for(Int_t i=0; i<kNHisto; i++) {
      s.fHisResX[i]->Copy(*fHisResX[i]);
      s.fHisResZ[i]->Copy(*fHisResZ[i]);
      s.fHisResXZ[i]->Copy(*fHisResXZ[i]);
      s.fHisClusterSize[i]->Copy(*fHisClusterSize[i]);
   }
 }
}
//_________________________________________________________________________
AliITSPlaneEffSSD& AliITSPlaneEffSSD::operator+=(const AliITSPlaneEffSSD &add){
    //    Add-to-me operator
    // Inputs:
    //    const AliITSPlaneEffSSD &add  simulation class to be added
    // Outputs:
    //    none.
    // Return:
    //    none
    for (UInt_t i=0; i<kNModule; i++){
      fFound[i] += add.fFound[i];
      fTried[i] += add.fTried[i];
    }
    if(fHis && add.fHis) {
      for(Int_t i=0; i<kNHisto; i++) {
        fHisResX[i]->Add(add.fHisResX[i]);
        fHisResZ[i]->Add(add.fHisResZ[i]);
        fHisResXZ[i]->Add(add.fHisResXZ[i]);
        fHisClusterSize[i]->Add(add.fHisClusterSize[i]);
      }
    }
    return *this;
}
//______________________________________________________________________
AliITSPlaneEffSSD&  AliITSPlaneEffSSD::operator=(const
                                           AliITSPlaneEffSSD &s){
    //    Assignment operator
    // Inputs:
    //    AliITSPlaneEffSSD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:
 
    if(this==&s) return *this;
    s.Copy(*this);
    return *this;
}
//______________________________________________________________________
void AliITSPlaneEffSSD::Copy(TObject &obj) const {
  // protected method. copy this to obj
  AliITSPlaneEff::Copy(obj);
  AliITSPlaneEffSSD& target = (AliITSPlaneEffSSD &) obj;
  for(Int_t i=0;i<kNModule;i++) {
      target.fFound[i] = fFound[i];
      target.fTried[i] = fTried[i];
  }
  CopyHistos(target);
  return;
}
//_______________________________________________________________________
void AliITSPlaneEffSSD::CopyHistos(AliITSPlaneEffSSD &target) const {
  // protected method: copy histos from this to target
  target.fHis  = fHis; // this is redundant only in some cases. Leave as it is.
  if(fHis) {
    target.fHisResX=new TH1F*[kNHisto];
    target.fHisResZ=new TH1F*[kNHisto];
    target.fHisResXZ=new TH2F*[kNHisto];
    target.fHisClusterSize=new TH2I*[kNHisto];
    for(Int_t i=0; i<kNHisto; i++) {
      target.fHisResX[i] = new TH1F(*fHisResX[i]);
      target.fHisResZ[i] = new TH1F(*fHisResZ[i]);
      target.fHisResXZ[i] = new TH2F(*fHisResXZ[i]);
      target.fHisClusterSize[i] = new TH2I(*fHisClusterSize[i]);
    }
  }
return;
}
//______________________________________________________________________
AliITSPlaneEff&  AliITSPlaneEffSSD::operator=(const
                                           AliITSPlaneEff &s){
    //    Assignment operator
    // Inputs:
    //    AliITSPlaneEffSSD &s The original class for which
    //                                this class is a copy of
    // Outputs:
    //    none.
    // Return:

    if(&s == this) return *this;
    AliError("operator=: Not allowed to make a =, use default creater instead");
    return *this;
}
//_______________________________________________________________________
Int_t AliITSPlaneEffSSD::GetMissingTracksForGivenEff(Double_t eff, Double_t RelErr,
          UInt_t im) const {
   
  //   Estimate the number of tracks still to be collected to attain a 
  //   given efficiency eff, with relative error RelErr
  //   Inputs:
  //         eff    -> Expected efficiency (e.g. those from actual estimate)
  //         RelErr -> tollerance [0,1] 
  //         im     -> module number [0,1697]
  //   Outputs: none
  //   Return: the estimated n. of tracks 
  //
if (im>=kNModule) 
 {AliError("GetMissingTracksForGivenEff: you asked for a non existing module");
 return -1;}
else return GetNTracksForGivenEff(eff,RelErr)-fTried[GetKey(im)];
}
//_________________________________________________________________________
Double_t  AliITSPlaneEffSSD::PlaneEff(const UInt_t im) const {
// Compute the efficiency for a basic block, 
// Inputs:
//        im     -> module number [0,1697]
if (im>=kNModule) 
 {AliError("PlaneEff(UInt_t): you asked for a non existing module"); return -1.;}
 Int_t nf=fFound[GetKey(im)];
 Int_t nt=fTried[GetKey(im)];
return AliITSPlaneEff::PlaneEff(nf,nt);
}
//_________________________________________________________________________
Double_t  AliITSPlaneEffSSD::ErrPlaneEff(const UInt_t im) const {
    // Compute the statistical error on efficiency for a basic block,
    // using binomial statistics 
    // Inputs:
    //        im     -> module number [0,1697]
if (im>=kNModule) 
 {AliError("ErrPlaneEff(UInt_t): you asked for a non existing module"); return -1.;}
Int_t nf=fFound[GetKey(im)];
Int_t nt=fTried[GetKey(im)];
return AliITSPlaneEff::ErrPlaneEff(nf,nt);
} 
//_________________________________________________________________________
Bool_t AliITSPlaneEffSSD::UpDatePlaneEff(const Bool_t Kfound, const UInt_t im) {
  // Update efficiency for a basic block
if (im>=kNModule) 
 {AliError("UpDatePlaneEff: you asked for a non existing module"); return kFALSE;}
 fTried[GetKey(im)]++;
 if(Kfound) fFound[GetKey(im)]++;
 return kTRUE;
}
//_________________________________________________________________________
UInt_t AliITSPlaneEffSSD::GetKey(const UInt_t mod) const {
  // get key given a basic block
if(mod>=kNModule)
  {AliError("GetKey: you asked for a non existing block"); return 99999;}
return mod;
}
//__________________________________________________________________________
UInt_t AliITSPlaneEffSSD::GetModFromKey(const UInt_t key) const {
  // get mod. from key
if(key>=kNModule)
  {AliError("GetModFromKey: you asked for a non existing key"); return 9999;}
return key;
}
//__________________________________________________________________________
Double_t AliITSPlaneEffSSD::LivePlaneEff(UInt_t key) const {
  // returns plane efficieny after adding the fraction of sensor which is bad
if(key>=kNModule)
  {AliError("LivePlaneEff: you asked for a non existing key");
   return -1.;}
Double_t leff=AliITSPlaneEff::LivePlaneEff(0); // this just for the Warning
leff=PlaneEff(key)+GetFracBad(key);
return leff>1?1:leff;
}
//____________________________________________________________________________
Double_t AliITSPlaneEffSSD::ErrLivePlaneEff(UInt_t key) const {
  // returns error on live plane efficiency
if(key>=kNModule)
  {AliError("ErrLivePlaneEff: you asked for a non existing key");
   return -1.;}
Int_t nf=fFound[key];
Double_t triedInLive=GetFracLive(key)*fTried[key];
Int_t nt=TMath::Max(nf,TMath::Nint(triedInLive));
return AliITSPlaneEff::ErrPlaneEff(nf,nt); // for the time being: to be checked
}
//_____________________________________________________________________________
Double_t AliITSPlaneEffSSD::GetFracLive(const UInt_t key) const {
  // returns the fraction of the sensor area which is OK (neither noisy nor dead)
  // As for now, it computes only the fraction of good strips / total strips. 
  // If this fraction is large, then the computation is a good approximation. 
  // In any case, it is a lower limit of the fraction of the live area. 
  // The next upgrades would be to add the fraction of area of superoposition 
  // between bad N-side strips and bad P-side strips.  
if(key>=kNModule)
  {AliError("GetFracLive: you asked for a non existing key");
   return -1.;}
AliInfo("GetFracLive: it computes only the fraction of working strips (N+P side) / total strips");
UInt_t bad=0;
GetBadInModule(key,bad);
Double_t live=bad;
live/=(kNChip*kNSide*kNStrip);
return 1.-live;
}
//_____________________________________________________________________________
void AliITSPlaneEffSSD::GetBadInModule(const UInt_t key, UInt_t& nrBadInMod) const {
  // returns the number of dead and noisy strips (sum of P and N sides).
nrBadInMod=0;
if(key>=kNModule)
  {AliError("GetBadInModule: you asked for a non existing key");
   return;}
    // Compute the number of bad (dead+noisy) pixel in a module
//
if(!fInitCDBCalled) 
  {AliError("GetBadInModule: CDB not inizialized: call InitCDB first");
   return;};
AliCDBManager* man = AliCDBManager::Instance();
// retrieve map of dead Pixel 
AliCDBEntry *cdbSSD = man->Get("ITS/Calib/BadChannelsSSD", fRunNumber);
TObjArray* ssdEntry;
if(cdbSSD) {
  ssdEntry = (TObjArray*)cdbSSD->GetObject();
  if(!ssdEntry) 
  {AliError("GetBadInChip: SSDEntry not found in CDB");
   return;}
} else {
  AliError("GetBadInChip: did not find Calib/BadChannelsSSD");
  return;
}
//
UInt_t mod=GetModFromKey(key);
//
AliITSBadChannelsSSD* badchannels=(AliITSBadChannelsSSD*) ssdEntry->At(mod);
// count the  number of bad channels on the p side
nrBadInMod += (badchannels->GetBadPChannelsList()).GetSize();
// add the  number of bad channels on the s side
nrBadInMod += (badchannels->GetBadNChannelsList()).GetSize();
return;
}
//_____________________________________________________________________________
Double_t AliITSPlaneEffSSD::GetFracBad(const UInt_t key) const {
  // returns 1-fractional live
if(key>=kNModule)
  {AliError("GetFracBad: you asked for a non existing key");
   return -1.;}
return 1.-GetFracLive(key);
}
//_____________________________________________________________________________
Bool_t AliITSPlaneEffSSD::WriteIntoCDB() const {
// write onto CDB
if(!fInitCDBCalled)
  {AliError("WriteIntoCDB: CDB not inizialized. Call InitCDB first");
   return kFALSE;}
// to be written properly: now only for debugging 
  AliCDBMetaData *md= new AliCDBMetaData(); // metaData describing the object
  md->SetObjectClassName("AliITSPlaneEff");
  md->SetResponsible("Giuseppe Eugenio Bruno");
  md->SetBeamPeriod(0);
  md->SetAliRootVersion("head 02/01/08"); //root version
  AliCDBId id("ITS/PlaneEff/PlaneEffSSD",0,AliCDBRunRange::Infinity()); 
  AliITSPlaneEffSSD eff; 
  eff=*this;
  Bool_t r=AliCDBManager::Instance()->GetDefaultStorage()->Put(&eff,id,md);
  delete md;
  return r;
}
//_____________________________________________________________________________
Bool_t AliITSPlaneEffSSD::ReadFromCDB() {
// read from CDB
if(!fInitCDBCalled)
  {AliError("ReadFromCDB: CDB not inizialized. Call InitCDB first");
   return kFALSE;}
//if(!AliCDBManager::Instance()->IsDefaultStorageSet()) {
//    AliCDBManager::Instance()->SetDefaultStorage("local://$ALICE_ROOT");
//  }
AliCDBEntry *cdbEntry = AliCDBManager::Instance()->Get("ITS/PlaneEff/PlaneEffSSD",fRunNumber);
AliITSPlaneEffSSD* eff= (AliITSPlaneEffSSD*)cdbEntry->GetObject();
if(this==eff) return kFALSE;
if(fHis) CopyHistos(*eff); // If histos already exist then copy them to eff
eff->Copy(*this);          // copy everything (statistics and histos) from eff to this
return kTRUE;
}
//_____________________________________________________________________________
UInt_t AliITSPlaneEffSSD::GetKeyFromDetLocCoord(Int_t ilay, Int_t idet,
                                                Float_t, Float_t) const {
// method to locate a basic block from Detector Local coordinate (to be used in tracking)
UInt_t key=999999;
if(ilay<4 || ilay>5)
  {AliError("GetKeyFromDetLocCoord: you asked for a non existing layer");
   return key;}
if(ilay==4 && (idet<0 || idet>747))
 {AliError("GetKeyFromDetLocCoord: you asked for a non existing detector");
   return key;}
if(ilay==5 && (idet<0 || idet>949))
 {AliError("GetKeyFromDetLocCoord: you asked for a non existing detector");
   return key;}

UInt_t mod=idet;
if(ilay==1) mod+=748;
key=GetKey(mod);
return key;
}
//__________________________________________________________
void AliITSPlaneEffSSD::InitHistos() {
  // for the moment let's create the histograms
  // module by  module
  TString histnameResX="HistResX_mod_",aux;
  TString histnameResZ="HistResZ_mod_";
  TString histnameResXZ="HistResXZ_mod_";
  TString histnameClusterType="HistClusterType_mod_";

//
  fHisResX=new TH1F*[kNHisto];
  fHisResZ=new TH1F*[kNHisto];
  fHisResXZ=new TH2F*[kNHisto];
  fHisClusterSize=new TH2I*[kNHisto];

  for (Int_t nhist=0;nhist<kNHisto;nhist++){
    aux=histnameResX;
    aux+=nhist;
    fHisResX[nhist]=new TH1F("histname","histname",500,-0.05,0.05); // +- 500 micron; 1 bin=2 micron
    fHisResX[nhist]->SetName(aux.Data());
    fHisResX[nhist]->SetTitle(aux.Data());

    aux=histnameResZ;
    aux+=nhist;
    fHisResZ[nhist]=new TH1F("histname","histname",500,-0.50,0.50); // +-5000 micron; 1 bin=20 micron
    fHisResZ[nhist]->SetName(aux.Data());
    fHisResZ[nhist]->SetTitle(aux.Data());

    aux=histnameResXZ;
    aux+=nhist;
    fHisResXZ[nhist]=new TH2F("histname","histname",40,-0.02,0.02,40,-0.16,0.16); // binning:
                                                                                   // 10 micron in x;
                                                                                   // 80 micron in z;
    fHisResXZ[nhist]->SetName(aux.Data());
    fHisResXZ[nhist]->SetTitle(aux.Data());

    aux=histnameClusterType;
    aux+=nhist;
    fHisClusterSize[nhist]=new TH2I("histname","histname",6,0.5,6.5,6,0.5,6.5);
    fHisClusterSize[nhist]->SetName(aux.Data());
    fHisClusterSize[nhist]->SetTitle(aux.Data());

  }
return;
}
//__________________________________________________________
void AliITSPlaneEffSSD::DeleteHistos() {
  if(fHisResX) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResX[i];
    delete [] fHisResX; fHisResX=0;
  }
  if(fHisResZ) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResZ[i];
    delete [] fHisResZ; fHisResZ=0;
  }
  if(fHisResXZ) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisResXZ[i];
    delete [] fHisResXZ; fHisResXZ=0;
  }
  if(fHisClusterSize) {
    for (Int_t i=0; i<kNHisto; i++ ) delete fHisClusterSize[i];
    delete [] fHisClusterSize; fHisClusterSize=0;
  }

return;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSSD::FillHistos(UInt_t key, Bool_t found,
                                     Float_t tXZ[2], Float_t cXZ[2], Int_t ctXZ[2]) {
// this method fill the histograms
// input: - key: unique key of the basic block
//        - found: Boolean to asses whether a cluster has been associated to the track or not
//        - tXZ[2] local X and Z coordinates of the track prediction
//        - cXZ[2] local X and Z coordinates of the cluster associated to the track
// output: kTRUE if filling was succesfull kFALSE otherwise
// side effects: updating of the histograms.
//
  if (!fHis) {
    AliWarning("FillHistos: histograms do not exist! Call SetCreateHistos(kTRUE) first");
    return kFALSE;
  }
  if(key>=kNModule)
    {AliWarning("FillHistos: you asked for a non existing key"); return kFALSE;}
  Int_t id=GetModFromKey(key);
  if(id>=kNHisto)
    {AliWarning("FillHistos: you want to fill a non-existing histos"); return kFALSE;}
  if(found) {
    Float_t resx=tXZ[0]-cXZ[0];
    Float_t resz=tXZ[1]-cXZ[1];
    fHisResX[id]->Fill(resx);
    fHisResZ[id]->Fill(resz);
    fHisResXZ[id]->Fill(resx,resz);
    fHisClusterSize[id]->Fill((Double_t)ctXZ[0],(Double_t)ctXZ[1]);
  }
  return kTRUE;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSSD::WriteHistosToFile(TString filename, Option_t* option) {
  //
  // Saves the histograms into a tree and saves the trees into a file
  //
  if (!fHis) return kFALSE;
  if (filename.Data()=="") {
     AliWarning("WriteHistosToFile: null output filename!");
     return kFALSE;
  }
//  char branchname[30];
  TFile *hFile=new TFile(filename.Data(),option,
                         "The File containing the TREEs with ITS PlaneEff Histos");
  TTree *SSDTree=new TTree("SSDTree","Tree whith Residuals and Cluster Type distributions for SSD");
  TH1F *histZ,*histX;
  TH2F *histXZ;
  TH2I *histClusterType;

  histZ=new TH1F();
  histX=new TH1F();
  histXZ=new TH2F();
  histClusterType=new TH2I();

  SSDTree->Branch("histX","TH1F",&histX,128000,0);
  SSDTree->Branch("histZ","TH1F",&histZ,128000,0);
  SSDTree->Branch("histXZ","TH2F",&histXZ,128000,0);
  SSDTree->Branch("histClusterType","TH2I",&histClusterType,128000,0);

  for(Int_t j=0;j<kNHisto;j++){
    histX=fHisResX[j];
    histZ=fHisResZ[j];
    histXZ=fHisResXZ[j];
    histClusterType=fHisClusterSize[j];

    SSDTree->Fill();
  }
  hFile->Write();
  hFile->Close();
return kTRUE;
}
//__________________________________________________________
Bool_t AliITSPlaneEffSSD::ReadHistosFromFile(TString filename) {
  //
  // Read histograms from an already existing file
  //
  if (!fHis) return kFALSE;
  if (filename.Data()=="") {
     AliWarning("ReadHistosFromFile: incorrect output filename!");
     return kFALSE;
  }

  TH1F *h  = 0;
  TH2F *h2 = 0;
  TH2I *h2i= 0;

  TFile *file=TFile::Open(filename.Data(),"READONLY");

  if (!file || file->IsZombie()) {
    AliWarning(Form("Can't open %s !",filename.Data()));
    delete file;
    return kFALSE;
  }
  TTree *tree = (TTree*) file->Get("SSDTree");

  TBranch *histX = (TBranch*) tree->GetBranch("histX");
  TBranch *histZ = (TBranch*) tree->GetBranch("histZ");
  TBranch *histXZ = (TBranch*) tree->GetBranch("histXZ");
  TBranch *histClusterType = (TBranch*) tree->GetBranch("histClusterType");

  gROOT->cd();

  Int_t nevent = (Int_t)histX->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histX->SetAddress(&h);
  for(Int_t j=0;j<kNHisto;j++){
    delete h; h=0;
    histX->GetEntry(j);
    fHisResX[j]->Add(h);
  }

  nevent = (Int_t)histZ->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histZ->SetAddress(&h);
  for(Int_t j=0;j<kNHisto;j++){
    delete h; h=0;
    histZ->GetEntry(j);
    fHisResZ[j]->Add(h);
  }

  nevent = (Int_t)histXZ->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histXZ->SetAddress(&h2);
  for(Int_t j=0;j<kNHisto;j++){
    delete h2; h2=0;
    histXZ->GetEntry(j);
    fHisResXZ[j]->Add(h2);
  }

  nevent = (Int_t)histClusterType->GetEntries();
  if(nevent!=kNHisto)
    {AliWarning("ReadHistosFromFile: trying to read too many or too few histos!"); return kFALSE;}
  histClusterType->SetAddress(&h2i);
  for(Int_t j=0;j<kNHisto;j++){
    delete h2i; h2i=0;
    histClusterType->GetEntry(j);
    fHisClusterSize[j]->Add(h2i);
  }

  delete h;   h=0;
  delete h2;  h2=0;
  delete h2i; h2i=0;

  if (file) {
    file->Close();
  }
return kTRUE;
}


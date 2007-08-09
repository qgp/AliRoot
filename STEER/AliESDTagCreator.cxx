/**************************************************************************
 * Author: Panos Christakoglou.                                           *
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

//-----------------------------------------------------------------
//           AliESDTagCreator class
//   This is the class to deal with the tag creation (post process)
//   Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//-----------------------------------------------------------------

//ROOT
#include <Riostream.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TSystem.h>
#include <TChain.h>
#include <TLorentzVector.h>

//ROOT-AliEn
#include <TGrid.h>
#include <TGridResult.h>

//AliRoot
#include "AliRunTag.h"
#include "AliEventTag.h"
#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliLog.h"

#include "AliESDTagCreator.h"


ClassImp(AliESDTagCreator)


//______________________________________________________________________________
  AliESDTagCreator::AliESDTagCreator() :
    AliTagCreator() {
  //==============Default constructor for a AliESDTagCreator================
}

//______________________________________________________________________________
AliESDTagCreator::~AliESDTagCreator() {
//================Default destructor for a AliESDTagCreator===================
}

//______________________________________________________________________________
Bool_t AliESDTagCreator::ReadGridCollection(TGridResult *fresult) {
  // Reads the entry of the TGridResult and creates the tags
  Int_t nEntries = fresult->GetEntries();

  TString alienUrl;
  const char *guid;
  const char *md5;
  const char *turl;
  Long64_t size = -1;

  Int_t counter = 0;
  for(Int_t i = 0; i < nEntries; i++) {
    alienUrl = fresult->GetKey(i,"turl");
    guid = fresult->GetKey(i,"guid");
    if(fresult->GetKey(i,"size")) size = atol (fresult->GetKey(i,"size"));
    md5 = fresult->GetKey(i,"md5");
    turl = fresult->GetKey(i,"turl");
    if(md5 && !strlen(guid)) md5 = 0;
    if(guid && !strlen(guid)) guid = 0;

    TFile *f = TFile::Open(alienUrl,"READ");
    CreateTag(f,guid,md5,turl,size,counter);
    f->Close();
    delete f;	 
    counter += 1;
  }//grid result loop

  return kTRUE;
}

//______________________________________________________________________________
Bool_t AliESDTagCreator::ReadLocalCollection(const char *localpath) {
  // Checks the different subdirs of the given local path and in the
  // case where it finds an AliESDs.root file it creates the tags
  
  void *dira =  gSystem->OpenDirectory(localpath);
  Char_t fPath[256];
  const char * dirname = 0x0;
  const char * filename = 0x0;
  const char * pattern = "AliESDs.root"; 

  Int_t counter = 0;
  while((dirname = gSystem->GetDirEntry(dira))) {
    sprintf(fPath,"%s/%s",localpath,dirname);
    void *dirb =  gSystem->OpenDirectory(fPath);
    while((filename = gSystem->GetDirEntry(dirb))) {
      if(strstr(filename,pattern)) {
	TString fESDFileName;
	fESDFileName = fPath;
	fESDFileName += "/";
	fESDFileName += pattern;
	TFile *f = TFile::Open(fESDFileName,"READ");
	CreateTag(f,fESDFileName,counter);
	f->Close();
	delete f;	 
	
	counter += 1;
      }//pattern check
    }//child directory's entry loop
  }//parent directory's entry loop

  return kTRUE;
}

//______________________________________________________________________________
Bool_t AliESDTagCreator::ReadCAFCollection(const char *filename) {
  // Temporary solution for CAF: Takes as an input the ascii file that
  // lists the ESDs stored in the SE of the CAF and creates the tags.

  // Open the input stream
  ifstream in;
  in.open(filename);

  Int_t counter = 0;
  TString esdfile;
  // Read the input list of files and add them to the chain
  while(in.good()) {
    in >> esdfile;
    if (!esdfile.Contains("root")) continue; // protection
    TFile *f = TFile::Open(esdfile,"READ");
    CreateTag(f,esdfile,counter);
    f->Close();
    delete f;	 
    
    counter += 1;
  }

  return kTRUE;
}

//_____________________________________________________________________________
void AliESDTagCreator::CreateTag(TFile* file, const char *guid, const char *md5, const char *turl, Long64_t size, Int_t Counter) {
  //private method that creates tag files
  TString fguid = guid;
  TString fmd5 = md5;
  TString fturl = turl;
  /////////////
  //muon code//
  ////////////
  Double_t fMUONMASS = 0.105658369;
  //Variables
  Double_t fX,fY,fZ ;
  Double_t fThetaX, fThetaY, fPyz, fChisquare;
  Double_t fPxRec, fPyRec, fPzRec, fEnergy;
  Int_t fCharge;
  TLorentzVector fEPvector;

  Float_t fZVertexCut = 10.0; 
  Float_t fRhoVertexCut = 2.0; 

  Float_t fLowPtCut = 1.0;
  Float_t fHighPtCut = 3.0;
  Float_t fVeryHighPtCut = 10.0;
  ////////////

  Double_t partFrac[5] = {0.01, 0.01, 0.85, 0.10, 0.05};

  // Creates the tags for all the events in a given ESD file
  Bool_t fIsSim = kTRUE;
  Int_t ntrack;
  Int_t nProtons, nKaons, nPions, nMuons, nElectrons;
  Int_t nPos, nNeg, nNeutr;
  Int_t nK0s, nNeutrons, nPi0s, nGamas;
  Int_t nCh1GeV, nCh3GeV, nCh10GeV;
  Int_t nMu1GeV, nMu3GeV, nMu10GeV;
  Int_t nEl1GeV, nEl3GeV, nEl10GeV;
  Float_t maxPt = .0, meanPt = .0, totalP = .0;
  Int_t fVertexflag;
  Int_t iRunNumber = 0;
  TString fVertexName;

  AliRunTag *tag = new AliRunTag();
  AliEventTag *evTag = new AliEventTag();
  TTree ttag("T","A Tree with event tags");
  TBranch * btag = ttag.Branch("AliTAG", &tag);
  btag->SetCompressionLevel(9);
  
  AliInfo(Form("Creating the tags......."));	
  
  Int_t firstEvent = 0,lastEvent = 0;
  TTree *t = (TTree*) file->Get("esdTree");
  AliESDEvent *esd = new AliESDEvent();
  esd->ReadFromTree(t);
  
  t->GetEntry(0);
  Int_t iInitRunNumber = esd->GetRunNumber();

  Int_t iNumberOfEvents = (Int_t)t->GetEntries();
  for (Int_t iEventNumber = 0; iEventNumber < iNumberOfEvents; iEventNumber++) {
    ntrack = 0;
    nPos = 0;
    nNeg = 0;
    nNeutr =0;
    nK0s = 0;
    nNeutrons = 0;
    nPi0s = 0;
    nGamas = 0;
    nProtons = 0;
    nKaons = 0;
    nPions = 0;
    nMuons = 0;
    nElectrons = 0;	  
    nCh1GeV = 0;
    nCh3GeV = 0;
    nCh10GeV = 0;
    nMu1GeV = 0;
    nMu3GeV = 0;
    nMu10GeV = 0;
    nEl1GeV = 0;
    nEl3GeV = 0;
    nEl10GeV = 0;
    maxPt = .0;
    meanPt = .0;
    totalP = .0;
    fVertexflag = 1;
    
    t->GetEntry(iEventNumber);
    iRunNumber = esd->GetRunNumber();
    if(iRunNumber != iInitRunNumber) AliFatal("Inconsistency of run numbers in the AliESD!!!");
    const AliESDVertex * vertexIn = esd->GetVertex();
    fVertexName = vertexIn->GetName();
    if(fVertexName == "default") fVertexflag = 0;

    for (Int_t iTrackNumber = 0; iTrackNumber < esd->GetNumberOfTracks(); iTrackNumber++) {
      AliESDtrack * esdTrack = esd->GetTrack(iTrackNumber);
      if(esdTrack->GetLabel() != 0) fIsSim = kTRUE;
      else if(esdTrack->GetLabel() == 0) fIsSim = kFALSE;
      UInt_t status = esdTrack->GetStatus();
      
      //select only tracks with ITS refit
      if ((status&AliESDtrack::kITSrefit)==0) continue;
      //select only tracks with TPC refit
      if ((status&AliESDtrack::kTPCrefit)==0) continue;
      
      //select only tracks with the "combined PID"
      if ((status&AliESDtrack::kESDpid)==0) continue;
      Double_t p[3];
      esdTrack->GetPxPyPz(p);
      Double_t momentum = sqrt(pow(p[0],2) + pow(p[1],2) + pow(p[2],2));
      Double_t fPt = sqrt(pow(p[0],2) + pow(p[1],2));
      totalP += momentum;
      meanPt += fPt;
      if(fPt > maxPt) maxPt = fPt;
      
      if(esdTrack->GetSign() > 0) {
	nPos++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() < 0) {
	nNeg++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() == 0) nNeutr++;
      
      //PID
      Double_t prob[5];
      esdTrack->GetESDpid(prob);
      
      Double_t rcc = 0.0;
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) rcc += prob[i]*partFrac[i];
      if(rcc == 0.0) continue;
      //Bayes' formula
      Double_t w[5];
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) w[i] = prob[i]*partFrac[i]/rcc;
      
      //protons
      if ((w[4]>w[3])&&(w[4]>w[2])&&(w[4]>w[1])&&(w[4]>w[0])) nProtons++;
      //kaons
      if ((w[3]>w[4])&&(w[3]>w[2])&&(w[3]>w[1])&&(w[3]>w[0])) nKaons++;
      //pions
      if ((w[2]>w[4])&&(w[2]>w[3])&&(w[2]>w[1])&&(w[2]>w[0])) nPions++; 
      //electrons
      if ((w[0]>w[4])&&(w[0]>w[3])&&(w[0]>w[2])&&(w[0]>w[1])) {
	nElectrons++;
	if(fPt > fLowPtCut) nEl1GeV++;
	if(fPt > fHighPtCut) nEl3GeV++;
	if(fPt > fVeryHighPtCut) nEl10GeV++;
      }	  
      ntrack++;
    }//esd track loop
    
    /////////////
    //muon code//
    ////////////
    Int_t nMuonTracks = esd->GetNumberOfMuonTracks();
    // loop over all reconstructed tracks (also first track of combination)
    for (Int_t iTrack = 0; iTrack <  nMuonTracks;  iTrack++) {
      AliESDMuonTrack* muonTrack = esd->GetMuonTrack(iTrack);
      if (muonTrack == 0x0) continue;
      
      // Coordinates at vertex
      fZ = muonTrack->GetZ(); 
      fY = muonTrack->GetBendingCoor();
      fX = muonTrack->GetNonBendingCoor(); 
      
      fThetaX = muonTrack->GetThetaX();
      fThetaY = muonTrack->GetThetaY();
      
      fPyz = 1./TMath::Abs(muonTrack->GetInverseBendingMomentum());
      fPzRec = - fPyz / TMath::Sqrt(1.0 + TMath::Tan(fThetaY)*TMath::Tan(fThetaY));
      fPxRec = fPzRec * TMath::Tan(fThetaX);
      fPyRec = fPzRec * TMath::Tan(fThetaY);
      fCharge = Int_t(TMath::Sign(1.,muonTrack->GetInverseBendingMomentum()));
      
      //ChiSquare of the track if needed
      fChisquare = muonTrack->GetChi2()/(2.0 * muonTrack->GetNHit() - 5);
      fEnergy = TMath::Sqrt(fMUONMASS * fMUONMASS + fPxRec * fPxRec + fPyRec * fPyRec + fPzRec * fPzRec);
      fEPvector.SetPxPyPzE(fPxRec, fPyRec, fPzRec, fEnergy);
      
      // total number of muons inside a vertex cut 
      if((TMath::Abs(fZ)<fZVertexCut) && (TMath::Sqrt(fY*fY+fX*fX)<fRhoVertexCut)) {
	nMuons++;
	if(fEPvector.Pt() > fLowPtCut) {
	  nMu1GeV++; 
	  if(fEPvector.Pt() > fHighPtCut) {
	    nMu3GeV++; 
	    if (fEPvector.Pt() > fVeryHighPtCut) {
	      nMu10GeV++;
	    }
	  }
	}
      }
    }//muon track loop
    
    // Fill the event tags 
    if(ntrack != 0) meanPt = meanPt/ntrack;
    
    evTag->SetEventId(iEventNumber+1);
    evTag->SetGUID(fguid);
    evTag->SetMD5(fmd5);
    evTag->SetTURL(fturl);
    evTag->SetSize(size);
    evTag->SetVertexX(vertexIn->GetXv());
    evTag->SetVertexY(vertexIn->GetYv());
    evTag->SetVertexZ(vertexIn->GetZv());
    evTag->SetVertexZError(vertexIn->GetZRes());
    evTag->SetVertexFlag(fVertexflag);
    
    evTag->SetT0VertexZ(esd->GetT0zVertex());
    
    evTag->SetTriggerMask(esd->GetTriggerMask());
    evTag->SetTriggerCluster(esd->GetTriggerCluster());
    
    evTag->SetZDCNeutron1Energy(esd->GetZDCN1Energy());
    evTag->SetZDCProton1Energy(esd->GetZDCP1Energy());
    evTag->SetZDCEMEnergy(esd->GetZDCEMEnergy());
    evTag->SetZDCNeutron1Energy(esd->GetZDCN2Energy());
    evTag->SetZDCProton1Energy(esd->GetZDCP2Energy());
    evTag->SetNumOfParticipants(esd->GetZDCParticipants());
    
    
    evTag->SetNumOfTracks(esd->GetNumberOfTracks());
    evTag->SetNumOfPosTracks(nPos);
    evTag->SetNumOfNegTracks(nNeg);
    evTag->SetNumOfNeutrTracks(nNeutr);
    
    evTag->SetNumOfV0s(esd->GetNumberOfV0s());
    evTag->SetNumOfCascades(esd->GetNumberOfCascades());
    evTag->SetNumOfKinks(esd->GetNumberOfKinks());
    evTag->SetNumOfPMDTracks(esd->GetNumberOfPmdTracks());
    
    evTag->SetNumOfProtons(nProtons);
    evTag->SetNumOfKaons(nKaons);
    evTag->SetNumOfPions(nPions);
    evTag->SetNumOfMuons(nMuons);
    evTag->SetNumOfElectrons(nElectrons);
    evTag->SetNumOfPhotons(nGamas);
    evTag->SetNumOfPi0s(nPi0s);
    evTag->SetNumOfNeutrons(nNeutrons);
    evTag->SetNumOfKaon0s(nK0s);
    
    evTag->SetNumOfChargedAbove1GeV(nCh1GeV);
    evTag->SetNumOfChargedAbove3GeV(nCh3GeV);
    evTag->SetNumOfChargedAbove10GeV(nCh10GeV);
    evTag->SetNumOfMuonsAbove1GeV(nMu1GeV);
    evTag->SetNumOfMuonsAbove3GeV(nMu3GeV);
    evTag->SetNumOfMuonsAbove10GeV(nMu10GeV);
    evTag->SetNumOfElectronsAbove1GeV(nEl1GeV);
    evTag->SetNumOfElectronsAbove3GeV(nEl3GeV);
    evTag->SetNumOfElectronsAbove10GeV(nEl10GeV);
    
    evTag->SetNumOfPHOSClusters(esd->GetNumberOfPHOSClusters());
    evTag->SetNumOfEMCALClusters(esd->GetNumberOfEMCALClusters());
    
    evTag->SetTotalMomentum(totalP);
    evTag->SetMeanPt(meanPt);
    evTag->SetMaxPt(maxPt);
    
    tag->SetRunId(iInitRunNumber);
    if(fIsSim) tag->SetDataType(0);
    else tag->SetDataType(1);
    tag->AddEventTag(*evTag);
  }//event loop
  lastEvent = iNumberOfEvents;
  
  t->Delete("");
  
  ttag.Fill();
  tag->Clear();

  TString localFileName = "Run"; localFileName += tag->GetRunId(); 
  localFileName += ".Event"; localFileName += firstEvent; localFileName += "_"; localFileName += lastEvent; localFileName += "."; localFileName += Counter;
  localFileName += ".ESD.tag.root";

  TString fileName;
  
  if(fStorage == 0) {
    fileName = localFileName.Data();      
    AliInfo(Form("Writing tags to local file: %s",fileName.Data()));
  }
  else if(fStorage == 1) {
    TString alienLocation = "/alien";
    alienLocation += gGrid->Pwd();
    alienLocation += fgridpath.Data();
    alienLocation += "/";
    alienLocation +=  localFileName;
    alienLocation += "?se=";
    alienLocation += fSE.Data();
    fileName = alienLocation.Data();
    AliInfo(Form("Writing tags to grid file: %s",fileName.Data()));
  }

  TFile* ftag = TFile::Open(fileName, "recreate");
  ftag->cd();
  ttag.Write();
  ftag->Close();

  delete ftag;
  delete esd;

  delete tag;
}

//_____________________________________________________________________________
void AliESDTagCreator::CreateTag(TFile* file, const char *filepath, Int_t Counter) {
  //private method that creates tag files
  /////////////
  //muon code//
  ////////////
  Double_t fMUONMASS = 0.105658369;
  //Variables
  Double_t fX,fY,fZ ;
  Double_t fThetaX, fThetaY, fPyz, fChisquare;
  Double_t fPxRec, fPyRec, fPzRec, fEnergy;
  Int_t fCharge;
  TLorentzVector fEPvector;

  Float_t fZVertexCut = 10.0; 
  Float_t fRhoVertexCut = 2.0; 

  Float_t fLowPtCut = 1.0;
  Float_t fHighPtCut = 3.0;
  Float_t fVeryHighPtCut = 10.0;
  ////////////

  Double_t partFrac[5] = {0.01, 0.01, 0.85, 0.10, 0.05};

  // Creates the tags for all the events in a given ESD file
  Bool_t fIsSim = kTRUE;
  Int_t ntrack;
  Int_t nProtons, nKaons, nPions, nMuons, nElectrons;
  Int_t nPos, nNeg, nNeutr;
  Int_t nK0s, nNeutrons, nPi0s, nGamas;
  Int_t nCh1GeV, nCh3GeV, nCh10GeV;
  Int_t nMu1GeV, nMu3GeV, nMu10GeV;
  Int_t nEl1GeV, nEl3GeV, nEl10GeV;
  Float_t maxPt = .0, meanPt = .0, totalP = .0;
  Int_t fVertexflag;
  Int_t iRunNumber = 0;
  TString fVertexName;

  AliRunTag *tag = new AliRunTag();
  AliEventTag *evTag = new AliEventTag();
  TTree ttag("T","A Tree with event tags");
  TBranch * btag = ttag.Branch("AliTAG", &tag);
  btag->SetCompressionLevel(9);
  
  AliInfo(Form("Creating the tags......."));	
  
  Int_t firstEvent = 0,lastEvent = 0;
  
  TTree *t = (TTree*) file->Get("esdTree");
  AliESDEvent *esd = new AliESDEvent();
  esd->ReadFromTree(t);
  
  t->GetEntry(0);
  Int_t iInitRunNumber = esd->GetRunNumber();

  Int_t iNumberOfEvents = (Int_t)t->GetEntries();
  for (Int_t iEventNumber = 0; iEventNumber < iNumberOfEvents; iEventNumber++) {
    ntrack = 0;
    nPos = 0;
    nNeg = 0;
    nNeutr =0;
    nK0s = 0;
    nNeutrons = 0;
    nPi0s = 0;
    nGamas = 0;
    nProtons = 0;
    nKaons = 0;
    nPions = 0;
    nMuons = 0;
    nElectrons = 0;	  
    nCh1GeV = 0;
    nCh3GeV = 0;
    nCh10GeV = 0;
    nMu1GeV = 0;
    nMu3GeV = 0;
    nMu10GeV = 0;
    nEl1GeV = 0;
    nEl3GeV = 0;
    nEl10GeV = 0;
    maxPt = .0;
    meanPt = .0;
    totalP = .0;
    fVertexflag = 1;
    
    t->GetEntry(iEventNumber);
    iRunNumber = esd->GetRunNumber();
    if(iRunNumber != iInitRunNumber) AliFatal("Inconsistency of run numbers in the AliESD!!!");
    const AliESDVertex * vertexIn = esd->GetVertex();
    fVertexName = vertexIn->GetName();
    if(fVertexName == "default") fVertexflag = 0;

    for (Int_t iTrackNumber = 0; iTrackNumber < esd->GetNumberOfTracks(); iTrackNumber++) {
      AliESDtrack * esdTrack = esd->GetTrack(iTrackNumber);
      if(esdTrack->GetLabel() != 0) fIsSim = kTRUE;
      else if(esdTrack->GetLabel() == 0) fIsSim = kFALSE;
      UInt_t status = esdTrack->GetStatus();
      
      //select only tracks with ITS refit
      if ((status&AliESDtrack::kITSrefit)==0) continue;
      //select only tracks with TPC refit
      if ((status&AliESDtrack::kTPCrefit)==0) continue;
      
      //select only tracks with the "combined PID"
      if ((status&AliESDtrack::kESDpid)==0) continue;
      Double_t p[3];
      esdTrack->GetPxPyPz(p);
      Double_t momentum = sqrt(pow(p[0],2) + pow(p[1],2) + pow(p[2],2));
      Double_t fPt = sqrt(pow(p[0],2) + pow(p[1],2));
      totalP += momentum;
      meanPt += fPt;
      if(fPt > maxPt) maxPt = fPt;
      
      if(esdTrack->GetSign() > 0) {
	nPos++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() < 0) {
	nNeg++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() == 0) nNeutr++;
      
      //PID
      Double_t prob[5];
      esdTrack->GetESDpid(prob);
      
      Double_t rcc = 0.0;
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) rcc += prob[i]*partFrac[i];
      if(rcc == 0.0) continue;
      //Bayes' formula
      Double_t w[5];
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) w[i] = prob[i]*partFrac[i]/rcc;
      
      //protons
      if ((w[4]>w[3])&&(w[4]>w[2])&&(w[4]>w[1])&&(w[4]>w[0])) nProtons++;
      //kaons
      if ((w[3]>w[4])&&(w[3]>w[2])&&(w[3]>w[1])&&(w[3]>w[0])) nKaons++;
      //pions
      if ((w[2]>w[4])&&(w[2]>w[3])&&(w[2]>w[1])&&(w[2]>w[0])) nPions++; 
      //electrons
      if ((w[0]>w[4])&&(w[0]>w[3])&&(w[0]>w[2])&&(w[0]>w[1])) {
	nElectrons++;
	if(fPt > fLowPtCut) nEl1GeV++;
	if(fPt > fHighPtCut) nEl3GeV++;
	if(fPt > fVeryHighPtCut) nEl10GeV++;
      }	  
      ntrack++;
    }//esd track loop
    
    /////////////
    //muon code//
    ////////////
    Int_t nMuonTracks = esd->GetNumberOfMuonTracks();
    // loop over all reconstructed tracks (also first track of combination)
    for (Int_t iTrack = 0; iTrack <  nMuonTracks;  iTrack++) {
      AliESDMuonTrack* muonTrack = esd->GetMuonTrack(iTrack);
      if (muonTrack == 0x0) continue;
      
      // Coordinates at vertex
      fZ = muonTrack->GetZ(); 
      fY = muonTrack->GetBendingCoor();
      fX = muonTrack->GetNonBendingCoor(); 
      
      fThetaX = muonTrack->GetThetaX();
      fThetaY = muonTrack->GetThetaY();
      
      fPyz = 1./TMath::Abs(muonTrack->GetInverseBendingMomentum());
      fPzRec = - fPyz / TMath::Sqrt(1.0 + TMath::Tan(fThetaY)*TMath::Tan(fThetaY));
      fPxRec = fPzRec * TMath::Tan(fThetaX);
      fPyRec = fPzRec * TMath::Tan(fThetaY);
      fCharge = Int_t(TMath::Sign(1.,muonTrack->GetInverseBendingMomentum()));
      
      //ChiSquare of the track if needed
      fChisquare = muonTrack->GetChi2()/(2.0 * muonTrack->GetNHit() - 5);
      fEnergy = TMath::Sqrt(fMUONMASS * fMUONMASS + fPxRec * fPxRec + fPyRec * fPyRec + fPzRec * fPzRec);
      fEPvector.SetPxPyPzE(fPxRec, fPyRec, fPzRec, fEnergy);
      
      // total number of muons inside a vertex cut 
      if((TMath::Abs(fZ)<fZVertexCut) && (TMath::Sqrt(fY*fY+fX*fX)<fRhoVertexCut)) {
	nMuons++;
	if(fEPvector.Pt() > fLowPtCut) {
	  nMu1GeV++; 
	  if(fEPvector.Pt() > fHighPtCut) {
	    nMu3GeV++; 
	    if (fEPvector.Pt() > fVeryHighPtCut) {
	      nMu10GeV++;
	    }
	  }
	}
      }
    }//muon track loop
    
    // Fill the event tags 
    if(ntrack != 0) meanPt = meanPt/ntrack;
    
    evTag->SetEventId(iEventNumber+1);
    evTag->SetPath(filepath);
 
    evTag->SetVertexX(vertexIn->GetXv());
    evTag->SetVertexY(vertexIn->GetYv());
    evTag->SetVertexZ(vertexIn->GetZv());
    evTag->SetVertexZError(vertexIn->GetZRes());
    evTag->SetVertexFlag(fVertexflag);
    
    evTag->SetT0VertexZ(esd->GetT0zVertex());
    
    evTag->SetTriggerMask(esd->GetTriggerMask());
    evTag->SetTriggerCluster(esd->GetTriggerCluster());
    
    evTag->SetZDCNeutron1Energy(esd->GetZDCN1Energy());
    evTag->SetZDCProton1Energy(esd->GetZDCP1Energy());
    evTag->SetZDCEMEnergy(esd->GetZDCEMEnergy());
    evTag->SetZDCNeutron1Energy(esd->GetZDCN2Energy());
    evTag->SetZDCProton1Energy(esd->GetZDCP2Energy());
    evTag->SetNumOfParticipants(esd->GetZDCParticipants());
    
    
    evTag->SetNumOfTracks(esd->GetNumberOfTracks());
    evTag->SetNumOfPosTracks(nPos);
    evTag->SetNumOfNegTracks(nNeg);
    evTag->SetNumOfNeutrTracks(nNeutr);
    
    evTag->SetNumOfV0s(esd->GetNumberOfV0s());
    evTag->SetNumOfCascades(esd->GetNumberOfCascades());
    evTag->SetNumOfKinks(esd->GetNumberOfKinks());
    evTag->SetNumOfPMDTracks(esd->GetNumberOfPmdTracks());
    
    evTag->SetNumOfProtons(nProtons);
    evTag->SetNumOfKaons(nKaons);
    evTag->SetNumOfPions(nPions);
    evTag->SetNumOfMuons(nMuons);
    evTag->SetNumOfElectrons(nElectrons);
    evTag->SetNumOfPhotons(nGamas);
    evTag->SetNumOfPi0s(nPi0s);
    evTag->SetNumOfNeutrons(nNeutrons);
    evTag->SetNumOfKaon0s(nK0s);
    
    evTag->SetNumOfChargedAbove1GeV(nCh1GeV);
    evTag->SetNumOfChargedAbove3GeV(nCh3GeV);
    evTag->SetNumOfChargedAbove10GeV(nCh10GeV);
    evTag->SetNumOfMuonsAbove1GeV(nMu1GeV);
    evTag->SetNumOfMuonsAbove3GeV(nMu3GeV);
    evTag->SetNumOfMuonsAbove10GeV(nMu10GeV);
    evTag->SetNumOfElectronsAbove1GeV(nEl1GeV);
    evTag->SetNumOfElectronsAbove3GeV(nEl3GeV);
    evTag->SetNumOfElectronsAbove10GeV(nEl10GeV);
    
    evTag->SetNumOfPHOSClusters(esd->GetNumberOfPHOSClusters());
    evTag->SetNumOfEMCALClusters(esd->GetNumberOfEMCALClusters());
    
    evTag->SetTotalMomentum(totalP);
    evTag->SetMeanPt(meanPt);
    evTag->SetMaxPt(maxPt);
    
    tag->SetRunId(iInitRunNumber);
    if(fIsSim) tag->SetDataType(0);
    else tag->SetDataType(1);
    tag->AddEventTag(*evTag);
  }//event loop
  lastEvent = iNumberOfEvents;
  
  t->Delete("");
  
  ttag.Fill();
  tag->Clear();

  TString localFileName = "Run"; localFileName += tag->GetRunId(); 
  localFileName += ".Event"; localFileName += firstEvent; localFileName += "_"; localFileName += lastEvent; localFileName += "."; localFileName += Counter;
  localFileName += ".ESD.tag.root";

  TString fileName;
  
  if(fStorage == 0) {
    fileName = localFileName.Data();      
    AliInfo(Form("Writing tags to local file: %s",fileName.Data()));
  }
  else if(fStorage == 1) {
    TString alienLocation = "/alien";
    alienLocation += gGrid->Pwd();
    alienLocation += fgridpath.Data();
    alienLocation += "/";
    alienLocation +=  localFileName;
    alienLocation += "?se=";
    alienLocation += fSE.Data();
    fileName = alienLocation.Data();
    AliInfo(Form("Writing tags to grid file: %s",fileName.Data()));
  }

  TFile* ftag = TFile::Open(fileName, "recreate");
  ftag->cd();
  ttag.Write();
  ftag->Close();

  delete ftag;
  delete esd;

  delete tag;
}

//_____________________________________________________________________________
void AliESDTagCreator::CreateESDTags(Int_t fFirstEvent, Int_t fLastEvent) {
  //GRP
  Float_t lhcLuminosity = 0.0;
  TString lhcState = "test";
  UInt_t detectorMask = 0;

  /////////////
  //muon code//
  ////////////
  Double_t fMUONMASS = 0.105658369;
  //Variables
  Double_t fX,fY,fZ ;
  Double_t fThetaX, fThetaY, fPyz, fChisquare;
  Double_t fPxRec,fPyRec, fPzRec, fEnergy;
  Int_t fCharge;
  TLorentzVector fEPvector;

  Float_t fZVertexCut = 10.0; 
  Float_t fRhoVertexCut = 2.0; 

  Float_t fLowPtCut = 1.0;
  Float_t fHighPtCut = 3.0;
  Float_t fVeryHighPtCut = 10.0;
  ////////////

  Double_t partFrac[5] = {0.01, 0.01, 0.85, 0.10, 0.05};

  // Creates the tags for all the events in a given ESD file
  Int_t ntrack;
  Int_t nProtons, nKaons, nPions, nMuons, nElectrons;
  Int_t nPos, nNeg, nNeutr;
  Int_t nK0s, nNeutrons, nPi0s, nGamas;
  Int_t nCh1GeV, nCh3GeV, nCh10GeV;
  Int_t nMu1GeV, nMu3GeV, nMu10GeV;
  Int_t nEl1GeV, nEl3GeV, nEl10GeV;
  Float_t maxPt = .0, meanPt = .0, totalP = .0;
  Int_t fVertexflag;
  Int_t iRunNumber = 0;
  TString fVertexName("default");

  AliRunTag *tag = new AliRunTag();
  AliEventTag *evTag = new AliEventTag();
  TTree ttag("T","A Tree with event tags");
  TBranch * btag = ttag.Branch("AliTAG", &tag);
  btag->SetCompressionLevel(9);
  
  AliInfo(Form("Creating the tags......."));	

  TFile *file = TFile::Open("AliESDs.root");
  if (!file || !file->IsOpen()) {
    AliError(Form("opening failed"));
    delete file;
    return ;
  }  
  Int_t lastEvent = 0;
  TTree *b = (TTree*) file->Get("esdTree");
  AliESDEvent *esd = new AliESDEvent();
  esd->ReadFromTree(b);

  b->GetEntry(fFirstEvent);
  Int_t iInitRunNumber = esd->GetRunNumber();
  
  Int_t iNumberOfEvents = (Int_t)b->GetEntries();
  if(fLastEvent != -1) iNumberOfEvents = fLastEvent + 1;
  for (Int_t iEventNumber = fFirstEvent; iEventNumber < iNumberOfEvents; iEventNumber++) {
    ntrack = 0;
    nPos = 0;
    nNeg = 0;
    nNeutr =0;
    nK0s = 0;
    nNeutrons = 0;
    nPi0s = 0;
    nGamas = 0;
    nProtons = 0;
    nKaons = 0;
    nPions = 0;
    nMuons = 0;
    nElectrons = 0;	  
    nCh1GeV = 0;
    nCh3GeV = 0;
    nCh10GeV = 0;
    nMu1GeV = 0;
    nMu3GeV = 0;
    nMu10GeV = 0;
    nEl1GeV = 0;
    nEl3GeV = 0;
    nEl10GeV = 0;
    maxPt = .0;
    meanPt = .0;
    totalP = .0;
    fVertexflag = 0;

    b->GetEntry(iEventNumber);
    iRunNumber = esd->GetRunNumber();
    if(iRunNumber != iInitRunNumber) AliFatal("Inconsistency of run numbers in the AliESD!!!");
    const AliESDVertex * vertexIn = esd->GetVertex();
    if (!vertexIn) AliError("ESD has not defined vertex.");
    if (vertexIn) fVertexName = vertexIn->GetName();
    if(fVertexName != "default") fVertexflag = 1;
    for (Int_t iTrackNumber = 0; iTrackNumber < esd->GetNumberOfTracks(); iTrackNumber++) {
      AliESDtrack * esdTrack = esd->GetTrack(iTrackNumber);
      UInt_t status = esdTrack->GetStatus();
      
      //select only tracks with ITS refit
      if ((status&AliESDtrack::kITSrefit)==0) continue;
      //select only tracks with TPC refit
      if ((status&AliESDtrack::kTPCrefit)==0) continue;
      
      //select only tracks with the "combined PID"
      if ((status&AliESDtrack::kESDpid)==0) continue;
      Double_t p[3];
      esdTrack->GetPxPyPz(p);
      Double_t momentum = sqrt(pow(p[0],2) + pow(p[1],2) + pow(p[2],2));
      Double_t fPt = sqrt(pow(p[0],2) + pow(p[1],2));
      totalP += momentum;
      meanPt += fPt;
      if(fPt > maxPt) maxPt = fPt;
      
      if(esdTrack->GetSign() > 0) {
	nPos++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() < 0) {
	nNeg++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(esdTrack->GetSign() == 0) nNeutr++;
      
      //PID
      Double_t prob[5];
      esdTrack->GetESDpid(prob);
      
      Double_t rcc = 0.0;
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) rcc += prob[i]*partFrac[i];
      if(rcc == 0.0) continue;
      //Bayes' formula
      Double_t w[5];
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) w[i] = prob[i]*partFrac[i]/rcc;
      
      //protons
      if ((w[4]>w[3])&&(w[4]>w[2])&&(w[4]>w[1])&&(w[4]>w[0])) nProtons++;
      //kaons
      if ((w[3]>w[4])&&(w[3]>w[2])&&(w[3]>w[1])&&(w[3]>w[0])) nKaons++;
      //pions
      if ((w[2]>w[4])&&(w[2]>w[3])&&(w[2]>w[1])&&(w[2]>w[0])) nPions++; 
      //electrons
      if ((w[0]>w[4])&&(w[0]>w[3])&&(w[0]>w[2])&&(w[0]>w[1])) {
	nElectrons++;
	if(fPt > fLowPtCut) nEl1GeV++;
	if(fPt > fHighPtCut) nEl3GeV++;
	if(fPt > fVeryHighPtCut) nEl10GeV++;
      }	  
      ntrack++;
    }//track loop
    
    /////////////
    //muon code//
    ////////////
    Int_t nMuonTracks = esd->GetNumberOfMuonTracks();
    // loop over all reconstructed tracks (also first track of combination)
    for (Int_t iTrack = 0; iTrack <  nMuonTracks;  iTrack++) {
      AliESDMuonTrack* muonTrack = esd->GetMuonTrack(iTrack);
      if (muonTrack == 0x0) continue;
      
      // Coordinates at vertex
      fZ = muonTrack->GetZ(); 
      fY = muonTrack->GetBendingCoor();
      fX = muonTrack->GetNonBendingCoor(); 
      
      fThetaX = muonTrack->GetThetaX();
      fThetaY = muonTrack->GetThetaY();
      
      fPyz = 1./TMath::Abs(muonTrack->GetInverseBendingMomentum());
      fPzRec = - fPyz / TMath::Sqrt(1.0 + TMath::Tan(fThetaY)*TMath::Tan(fThetaY));
      fPxRec = fPzRec * TMath::Tan(fThetaX);
      fPyRec = fPzRec * TMath::Tan(fThetaY);
      fCharge = Int_t(TMath::Sign(1.,muonTrack->GetInverseBendingMomentum()));
      
      //ChiSquare of the track if needed
      fChisquare = muonTrack->GetChi2()/(2.0 * muonTrack->GetNHit() - 5);
      fEnergy = TMath::Sqrt(fMUONMASS * fMUONMASS + fPxRec * fPxRec + fPyRec * fPyRec + fPzRec * fPzRec);
      fEPvector.SetPxPyPzE(fPxRec, fPyRec, fPzRec, fEnergy);
      
      // total number of muons inside a vertex cut 
      if((TMath::Abs(fZ)<fZVertexCut) && (TMath::Sqrt(fY*fY+fX*fX)<fRhoVertexCut)) {
	nMuons++;
	if(fEPvector.Pt() > fLowPtCut) {
	  nMu1GeV++; 
	  if(fEPvector.Pt() > fHighPtCut) {
	    nMu3GeV++; 
	    if (fEPvector.Pt() > fVeryHighPtCut) {
	      nMu10GeV++;
	    }
	  }
	}
      }
    }//muon track loop
    
    // Fill the event tags 
    if(ntrack != 0)
      meanPt = meanPt/ntrack;
    
    evTag->SetEventId(iEventNumber+1);
    if (vertexIn) {
      evTag->SetVertexX(vertexIn->GetXv());
      evTag->SetVertexY(vertexIn->GetYv());
      evTag->SetVertexZ(vertexIn->GetZv());
      evTag->SetVertexZError(vertexIn->GetZRes());
    }  
    evTag->SetVertexFlag(fVertexflag);

    evTag->SetT0VertexZ(esd->GetT0zVertex());
    
    evTag->SetTriggerMask(esd->GetTriggerMask());
    evTag->SetTriggerCluster(esd->GetTriggerCluster());
    
    evTag->SetZDCNeutron1Energy(esd->GetZDCN1Energy());
    evTag->SetZDCProton1Energy(esd->GetZDCP1Energy());
    evTag->SetZDCNeutron2Energy(esd->GetZDCN2Energy());
    evTag->SetZDCProton2Energy(esd->GetZDCP2Energy());
    evTag->SetZDCEMEnergy(esd->GetZDCEMEnergy());
    evTag->SetNumOfParticipants(esd->GetZDCParticipants());
    
    
    evTag->SetNumOfTracks(esd->GetNumberOfTracks());
    evTag->SetNumOfPosTracks(nPos);
    evTag->SetNumOfNegTracks(nNeg);
    evTag->SetNumOfNeutrTracks(nNeutr);
    
    evTag->SetNumOfV0s(esd->GetNumberOfV0s());
    evTag->SetNumOfCascades(esd->GetNumberOfCascades());
    evTag->SetNumOfKinks(esd->GetNumberOfKinks());
    evTag->SetNumOfPMDTracks(esd->GetNumberOfPmdTracks());
    
    evTag->SetNumOfProtons(nProtons);
    evTag->SetNumOfKaons(nKaons);
    evTag->SetNumOfPions(nPions);
    evTag->SetNumOfMuons(nMuons);
    evTag->SetNumOfElectrons(nElectrons);
    evTag->SetNumOfPhotons(nGamas);
    evTag->SetNumOfPi0s(nPi0s);
    evTag->SetNumOfNeutrons(nNeutrons);
    evTag->SetNumOfKaon0s(nK0s);
    
    evTag->SetNumOfChargedAbove1GeV(nCh1GeV);
    evTag->SetNumOfChargedAbove3GeV(nCh3GeV);
    evTag->SetNumOfChargedAbove10GeV(nCh10GeV);
    evTag->SetNumOfMuonsAbove1GeV(nMu1GeV);
    evTag->SetNumOfMuonsAbove3GeV(nMu3GeV);
    evTag->SetNumOfMuonsAbove10GeV(nMu10GeV);
    evTag->SetNumOfElectronsAbove1GeV(nEl1GeV);
    evTag->SetNumOfElectronsAbove3GeV(nEl3GeV);
    evTag->SetNumOfElectronsAbove10GeV(nEl10GeV);
    
    evTag->SetNumOfPHOSClusters(esd->GetNumberOfPHOSClusters());
    evTag->SetNumOfEMCALClusters(esd->GetNumberOfEMCALClusters());
    
    evTag->SetTotalMomentum(totalP);
    evTag->SetMeanPt(meanPt);
    evTag->SetMaxPt(maxPt);
    
    tag->SetLHCTag(lhcLuminosity,lhcState);
    tag->SetDetectorTag(detectorMask);

    tag->SetRunId(iInitRunNumber);
    tag->AddEventTag(*evTag);
  }
  if(fLastEvent == -1) lastEvent = (Int_t)b->GetEntries();
  else lastEvent = fLastEvent;
	
  ttag.Fill();
  tag->Clear();

  char fileName[256];
  sprintf(fileName, "Run%d.Event%d_%d.ESD.tag.root", 
	  tag->GetRunId(),fFirstEvent,lastEvent );
  AliInfo(Form("writing tags to file %s", fileName));
  AliDebug(1, Form("writing tags to file %s", fileName));
 
  TFile* ftag = TFile::Open(fileName, "recreate");
  ftag->cd();
  ttag.Write();
  ftag->Close();
  file->cd();
  delete tag;
  delete evTag;
}

//__________________________________________________________________________
/*void AliTagCreator::CreateAODTags(Int_t fFirstEvent, Int_t fLastEvent) {
  //creates tag files for AODs
  
  Float_t fLowPtCut = 1.0;
  Float_t fHighPtCut = 3.0;
  Float_t fVeryHighPtCut = 10.0;
  ////////////

  Double_t partFrac[10] = {0.01, 0.01, 0.85, 0.10, 0.05, 0., 0., 0., 0., 0.};

  // Creates the tags for all the events in a given ESD file
  Int_t ntrack;
  Int_t nProtons, nKaons, nPions, nMuons, nElectrons;
  Int_t nPos, nNeg, nNeutr;
  Int_t nKinks, nV0s, nCascades;
  Int_t nK0s, nNeutrons, nPi0s, nGamas;
  Int_t nCh1GeV, nCh3GeV, nCh10GeV;
  Int_t nMu1GeV, nMu3GeV, nMu10GeV;
  Int_t nEl1GeV, nEl3GeV, nEl10GeV;
  Float_t maxPt = .0, meanPt = .0, totalP = .0;

  AliRunTag *tag = new AliRunTag();
  TTree ttag("T","A Tree with event tags");
  TBranch * btag = ttag.Branch("AliTAG", &tag);
  btag->SetCompressionLevel(9);
  
  //reading the esd tag file
  TChain *oldTagTree = new TChain("T");
  const char * tagPattern = "ESD.tag";
  // Open the working directory
  void * dirp = gSystem->OpenDirectory(gSystem->pwd());
  const char * name = 0x0;
  // Add all files matching *pattern* to the chain
  while((name = gSystem->GetDirEntry(dirp))) {
    if (strstr(name,tagPattern)) oldTagTree->Add(name);  
  }//directory loop
  AliInfo(Form("Chained tag files: %d",oldTagTree->GetEntries()));

  //reading the esd tag file
  AliRunTag *oldtag = new AliRunTag();
  TString tagFilename;
  oldTagTree->SetBranchAddress("AliTAG",&oldtag);
  oldTagTree->GetEntry(0);
  tag->CopyStandardContent(oldtag);
  const TClonesArray *evTagList = oldtag->GetEventTags();

  AliInfo(Form("Creating the tags......."));	

  TFile *file = TFile::Open("AliAOD.root");
  if (!file || !file->IsOpen()) {
    AliError(Form("opening failed"));
    delete file;
    return ;
  }
  TTree *aodTree = (TTree*)file->Get("AOD");
  AliAODEvent *aod = (AliAODEvent*)aodTree->GetUserInfo()->FindObject("AliAODEvent");
  TIter next(aod->GetList());
  TObject *el;

  Int_t lastEvent = 0;  
  if(fLastEvent == -1) lastEvent = (Int_t)aodTree->GetEntries();
  else lastEvent = fLastEvent;

  while((el=(TNamed*)next())) 
    aodTree->SetBranchAddress(el->GetName(),aod->GetList()->GetObjectRef(el));
  
  // loop over events
  Int_t nEvents = aodTree->GetEntries();
  for (Int_t iEventNumber = 0; iEventNumber < nEvents; iEventNumber++) {
    AliEventTag *evTag = (AliEventTag *)evTagList->At(iEventNumber);    
    ntrack = 0;
    nPos = 0; nNeg = 0; nNeutr =0;
    nKinks = 0; nV0s = 0; nCascades = 0;
    nK0s = 0; nNeutrons = 0; nPi0s = 0; nGamas = 0;
    nProtons = 0;  nKaons = 0; nPions = 0; nMuons = 0; nElectrons = 0;	  
    nCh1GeV = 0; nCh3GeV = 0; nCh10GeV = 0;
    nMu1GeV = 0; nMu3GeV = 0; nMu10GeV = 0;
    nEl1GeV = 0; nEl3GeV = 0; nEl10GeV = 0;
    maxPt = .0; meanPt = .0; totalP = .0;

    // read events
    aodTree->GetEvent(iEventNumber);
    
    // set pointers
    aod->GetStdContent();
    
    Int_t nTracks = aod->GetNTracks();
    // loop over vertices
    Int_t nVtxs = aod->GetNVertices();
    for (Int_t nVtx = 0; nVtx < nVtxs; nVtx++) {
      // print track info
      AliAODVertex *vertex = aod->GetVertex(nVtx);
      if(vertex->GetType() == 1) nKinks += 1;
      if(vertex->GetType() == 2) nV0s += 1;
      if(vertex->GetType() == 3) nCascades += 1;
    }
    for (Int_t nTr = 0; nTr < nTracks; nTr++) {
      AliAODTrack *track = aod->GetTrack(nTr);
      
      Double_t fPt = track->Pt();
      if(fPt > maxPt) maxPt = fPt;
      if(track->Charge() > 0) {
	nPos++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(track->Charge() < 0) {
	nNeg++;
	if(fPt > fLowPtCut) nCh1GeV++;
	if(fPt > fHighPtCut) nCh3GeV++;
	if(fPt > fVeryHighPtCut) nCh10GeV++;
      }
      if(track->Charge() == 0) nNeutr++;

      //PID
      const Double32_t *prob = track->PID();
      Double_t rcc = 0.0;
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) rcc += prob[i]*partFrac[i];
      if(rcc == 0.0) continue;
      //Bayes' formula
      Double_t w[10];
      for(Int_t i = 0; i < AliPID::kSPECIES; i++) w[i] = prob[i]*partFrac[i]/rcc;
      
      //protons
      if ((w[4]>w[3])&&(w[4]>w[2])&&(w[4]>w[1])&&(w[4]>w[0])) nProtons++;
      //kaons
      if ((w[3]>w[4])&&(w[3]>w[2])&&(w[3]>w[1])&&(w[3]>w[0])) nKaons++;
      //pions
      if ((w[2]>w[4])&&(w[2]>w[3])&&(w[2]>w[1])&&(w[2]>w[0])) nPions++; 
      //muons
      if ((w[1]>w[4])&&(w[1]>w[3])&&(w[1]>w[2])&&(w[1]>w[0])) {
	nMuons++;
	if(fPt > fLowPtCut) nMu1GeV++;
	if(fPt > fHighPtCut) nMu3GeV++;
	if(fPt > fVeryHighPtCut) nMu10GeV++;
	}
      //electrons
      if ((w[0]>w[4])&&(w[0]>w[3])&&(w[0]>w[2])&&(w[0]>w[1])) {
	nElectrons++;
	if(fPt > fLowPtCut) nEl1GeV++;
	if(fPt > fHighPtCut) nEl3GeV++;
	if(fPt > fVeryHighPtCut) nEl10GeV++;
      }

      totalP += track->P();
      meanPt += fPt;
      ntrack++;
    }//track loop    
    // Fill the event tags 
    if(ntrack != 0)
      meanPt = meanPt/ntrack;

    evTag->SetEventId(iEventNumber+1);
        
    evTag->SetNumOfTracks(nTracks);
    evTag->SetNumOfPosTracks(nPos);
    evTag->SetNumOfNegTracks(nNeg);
    evTag->SetNumOfNeutrTracks(nNeutr);
    
    evTag->SetNumOfV0s(nV0s);
    evTag->SetNumOfCascades(nCascades);
    evTag->SetNumOfKinks(nKinks);
    
    evTag->SetNumOfProtons(nProtons);
    evTag->SetNumOfKaons(nKaons);
    evTag->SetNumOfPions(nPions);
    evTag->SetNumOfMuons(nMuons);
    evTag->SetNumOfElectrons(nElectrons);
    evTag->SetNumOfPhotons(nGamas);
    evTag->SetNumOfPi0s(nPi0s);
    evTag->SetNumOfNeutrons(nNeutrons);
    evTag->SetNumOfKaon0s(nK0s);
    
    evTag->SetNumOfChargedAbove1GeV(nCh1GeV);
    evTag->SetNumOfChargedAbove3GeV(nCh3GeV);
    evTag->SetNumOfChargedAbove10GeV(nCh10GeV);
    evTag->SetNumOfMuonsAbove1GeV(nMu1GeV);
    evTag->SetNumOfMuonsAbove3GeV(nMu3GeV);
    evTag->SetNumOfMuonsAbove10GeV(nMu10GeV);
    evTag->SetNumOfElectronsAbove1GeV(nEl1GeV);
    evTag->SetNumOfElectronsAbove3GeV(nEl3GeV);
    evTag->SetNumOfElectronsAbove10GeV(nEl10GeV);
    
    evTag->SetTotalMomentum(totalP);
    evTag->SetMeanPt(meanPt);
    evTag->SetMaxPt(maxPt);
    tag->AddEventTag(*evTag);
  }//event loop
  if(fLastEvent == -1) lastEvent = (Int_t)aodTree->GetEntries();
  else lastEvent = fLastEvent;

  ttag.Fill();
  tag->Clear();

  char fileName[256];
  sprintf(fileName, "Run%d.Event%d_%d.AOD.tag.root", 
	  tag->GetRunId(),fFirstEvent,lastEvent );
  AliInfo(Form("writing tags to file %s", fileName));
  AliDebug(1, Form("writing tags to file %s", fileName));
 
  TFile* ftag = TFile::Open(fileName, "recreate");
  ftag->cd();
  ttag.Write();
  ftag->Close();
  file->cd();
  file->Close();
  }*/

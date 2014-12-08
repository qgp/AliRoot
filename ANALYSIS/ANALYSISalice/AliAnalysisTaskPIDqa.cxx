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

/* $Id: AliAnalysisTaskPIDqa.cxx 43811 2010-09-23 14:13:31Z wiechula $ */
#include <TList.h>
#include <TVectorD.h>
#include <TObjArray.h>
#include <TH2.h>
#include <TFile.h>
#include <TPRegexp.h>
#include <TChain.h>
#include <TF1.h>
#include <TSpline.h>

#include <AliAnalysisManager.h>
#include <AliInputEventHandler.h>
#include <AliVEventHandler.h>
#include <AliVEvent.h>
#include <AliVParticle.h>
#include <AliVTrack.h>
#include <AliLog.h>
#include <AliPID.h>
#include <AliPIDResponse.h>
#include <AliITSPIDResponse.h>
#include <AliTPCPIDResponse.h>
#include <AliTRDPIDResponse.h>
#include <AliTOFPIDResponse.h>
#include <AliTPCdEdxInfo.h>

#include <AliESDEvent.h>
#include <AliAODEvent.h>
#include <AliESDv0.h>
#include <AliAODv0.h>
#include <AliESDv0KineCuts.h>
#include <AliESDtrackCuts.h>

#include <AliMCEvent.h>

#include "AliAnalysisTaskPIDqa.h"


ClassImp(AliAnalysisTaskPIDqa)

//______________________________________________________________________________
AliAnalysisTaskPIDqa::AliAnalysisTaskPIDqa():
AliAnalysisTaskSE(),
fPIDResponse(0x0),
fV0cuts(0x0),
fV0electrons(0x0),
fV0pions(0x0),
fV0kaons(0x0),
fV0protons(0x0),
fListQA(0x0),
fListQAits(0x0),
fListQAitsSA(0x0),
fListQAitsPureSA(0x0),
fListQAtpc(0x0),
fListQAtpcBasic(0x0),
fListQAtpcMCtruth(0x0),
fListQAtpcHybrid(0x0),
fListQAtpcOROChigh(0x0),
fListQAtpcV0(0x0),
fListQAtrd(0x0),
fListQAtrdNsig(0x0),
fListQAtrdNsigTPCTOF(0x0),
fListQAtof(0x0),
fListQAt0(0x0),
fListQAemcal(0x0),
fListQAhmpid(0x0),
fListQAtofhmpid(0x0),
fListQAtpctof(0x0),
fListQAV0(0x0),
fListQAinfo(0x0)
{
  //
  // Dummy constructor
  //
}

//______________________________________________________________________________
AliAnalysisTaskPIDqa::AliAnalysisTaskPIDqa(const char* name):
AliAnalysisTaskSE(name),
fPIDResponse(0x0),
fV0cuts(0x0),
fV0electrons(0x0),
fV0pions(0x0),
fV0kaons(0x0),
fV0protons(0x0),
fListQA(0x0),
fListQAits(0x0),
fListQAitsSA(0x0),
fListQAitsPureSA(0x0),
fListQAtpc(0x0),
fListQAtpcBasic(0x0),
fListQAtpcMCtruth(0x0),
fListQAtpcHybrid(0x0),
fListQAtpcOROChigh(0x0),
fListQAtpcV0(0x0),
fListQAtrd(0x0),
fListQAtrdNsig(0x0),
fListQAtrdNsigTPCTOF(0x0),
fListQAtof(0x0),
fListQAt0(0x0),
fListQAemcal(0x0),
fListQAhmpid(0x0),
fListQAtofhmpid(0x0),
fListQAtpctof(0x0),
fListQAV0(0x0),
fListQAinfo(0x0)
{
  //
  // Default constructor
  //
  DefineInput(0,TChain::Class());
  DefineOutput(1,TList::Class());
}

//______________________________________________________________________________
AliAnalysisTaskPIDqa::~AliAnalysisTaskPIDqa()
{
  //
  // Destructor
  //

  delete fV0cuts;
  delete fV0electrons;
  delete fV0pions;
  delete fV0kaons;
  delete fV0protons;

  if (!AliAnalysisManager::GetAnalysisManager()->IsProofMode()) delete fListQA;
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::UserCreateOutputObjects()
{
  //
  // Create the output QA objects
  //

  AliLog::SetClassDebugLevel("AliAnalysisTaskPIDqa",10);

  //input hander
  AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler *inputHandler=dynamic_cast<AliInputEventHandler*>(man->GetInputEventHandler());
  if (!inputHandler) AliFatal("Input handler needed");

  //pid response object
  fPIDResponse=inputHandler->GetPIDResponse();
  if (!fPIDResponse) AliError("PIDResponse object was not created");
  
  // V0 Kine cuts 
  fV0cuts = new AliESDv0KineCuts;
 
  // V0 PID Obj arrays
  fV0electrons = new TObjArray;
  fV0pions     = new TObjArray;
  fV0kaons     = new TObjArray;
  fV0protons   = new TObjArray;

  //
  fListQA=new TList;
  fListQA->SetOwner();
  
  fListQAits=new TList;
  fListQAits->SetOwner();
  fListQAits->SetName("ITS");

  fListQAitsSA=new TList;
  fListQAitsSA->SetOwner();
  fListQAitsSA->SetName("ITS_SA");

  fListQAitsPureSA=new TList;
  fListQAitsPureSA->SetOwner();
  fListQAitsPureSA->SetName("ITS_PureSA");

  fListQAtpc=new TList;
  fListQAtpc->SetOwner();
  fListQAtpc->SetName("TPC");

  fListQAtrd=new TList;
  fListQAtrd->SetOwner();
  fListQAtrd->SetName("TRD");

  fListQAtrdNsig=new TList;
  fListQAtrdNsig->SetOwner();
  fListQAtrdNsig->SetName("TRDnSigma");
  
  fListQAtrdNsigTPCTOF=new TList;
  fListQAtrdNsigTPCTOF->SetOwner();
  fListQAtrdNsigTPCTOF->SetName("TRDnSigma_TPCTOF");
  
  fListQAtof=new TList;
  fListQAtof->SetOwner();
  fListQAtof->SetName("TOF");

  fListQAt0=new TList;
  fListQAt0->SetOwner();
  fListQAt0->SetName("T0");
  
  fListQAemcal=new TList;
  fListQAemcal->SetOwner();
  fListQAemcal->SetName("EMCAL");
  
  fListQAhmpid=new TList;
  fListQAhmpid->SetOwner();
  fListQAhmpid->SetName("HMPID");
  
  fListQAtpctof=new TList;
  fListQAtpctof->SetOwner();
  fListQAtpctof->SetName("TPC_TOF");

  fListQAtofhmpid=new TList;
  fListQAtofhmpid->SetOwner();
  fListQAtofhmpid->SetName("TOF_HMPID");
  
  fListQAV0=new TList;
  fListQAV0->SetOwner();
  fListQAV0->SetName("V0decay");

  fListQAinfo=new TList;
  fListQAinfo->SetOwner();
  fListQAinfo->SetName("QAinfo");
  
  fListQA->Add(fListQAits);
  fListQA->Add(fListQAitsSA);
  fListQA->Add(fListQAitsPureSA);
  fListQA->Add(fListQAtpc);
  fListQA->Add(fListQAtrd);
  fListQA->Add(fListQAtof);
  fListQA->Add(fListQAt0);
  fListQA->Add(fListQAemcal);
  fListQA->Add(fListQAhmpid);
  fListQA->Add(fListQAtpctof);
  fListQA->Add(fListQAtofhmpid);
  fListQA->Add(fListQAV0);
  fListQA->Add(fListQAinfo);

  SetupITSqa();
//  SetupTPCqa(kFALSE, kTRUE, kFALSE);
  SetupTRDqa();
  SetupTOFqa();
  SetupT0qa();
  SetupEMCALqa();
  SetupHMPIDqa();
  SetupTPCTOFqa();
  SetupTOFHMPIDqa();
  SetupV0qa();
  SetupQAinfo();
  
  PostData(1,fListQA);
}


//______________________________________________________________________________
void AliAnalysisTaskPIDqa::UserExec(Option_t */*option*/)
{
  //
  // Setup the PID response functions and fill the QA histograms
  //

  AliVEvent *event=InputEvent();
  if (!event||!fPIDResponse) return;

  // Start with the V0 task (only possible for ESDs?)
  FillV0PIDlist();
  
  FillITSqa();
  FillTPCqa();
  FillTRDqa();
  FillTOFqa();
  FillEMCALqa();
  FillHMPIDqa();
  FillT0qa();
  
  //combined detector QA
  FillTPCTOFqa();
  FillTOFHMPIDqa();
  
  // Clear the V0 PID arrays
  ClearV0PIDlist();

  //QA info
  FillQAinfo();
  
  PostData(1,fListQA);
}

//______________________________________________________________________________
void  AliAnalysisTaskPIDqa::FillV0PIDlist(){

  //
  // Fill the PID object arrays holding the pointers to identified particle tracks
  //

  // Dynamic cast to ESD events (DO NOTHING for AOD events)
  AliESDEvent *event = dynamic_cast<AliESDEvent *>(InputEvent());
  if ( !event )  return;
  
  if(TString(event->GetBeamType())=="Pb-Pb" || TString(event->GetBeamType())=="A-A"){
    fV0cuts->SetMode(AliESDv0KineCuts::kPurity,AliESDv0KineCuts::kPbPb); 
  }
  else{
    fV0cuts->SetMode(AliESDv0KineCuts::kPurity,AliESDv0KineCuts::kPP); 
  }

  // V0 selection
  // set event
  fV0cuts->SetEvent(event);

  // loop over V0 particles
  for(Int_t iv0=0; iv0<event->GetNumberOfV0s();iv0++){

    AliESDv0 *v0 = (AliESDv0 *) event->GetV0(iv0);
 
    if(!v0) continue;
    if(v0->GetOnFlyStatus()) continue; 
  
    // Get the particle selection 
    Bool_t foundV0 = kFALSE;
    Int_t pdgV0, pdgP, pdgN;

    foundV0 = fV0cuts->ProcessV0(v0, pdgV0, pdgP, pdgN);
    if(!foundV0) continue;
    
    Int_t iTrackP = v0->GetPindex();  // positive track
    Int_t iTrackN = v0->GetNindex();  // negative track

    // v0 Armenteros plot (QA)
    Float_t armVar[2] = {0.0,0.0};
    fV0cuts->Armenteros(v0, armVar);

    TH2 *h=(TH2*)fListQAV0->At(0);
    if (!h) continue;
    h->Fill(armVar[0],armVar[1]);

    // fill the Object arrays
    // positive particles
    if( pdgP == -11){
      fV0electrons->Add((AliVTrack*)event->GetTrack(iTrackP));
    }
    else if( pdgP == 211){
      fV0pions->Add((AliVTrack*)event->GetTrack(iTrackP));
    }
    else if( pdgP == 321){
      fV0kaons->Add((AliVTrack*)event->GetTrack(iTrackP));
    }
    else if( pdgP == 2212){
      fV0protons->Add((AliVTrack*)event->GetTrack(iTrackP));
    }

    // negative particles
    if( pdgN == 11){
      fV0electrons->Add((AliVTrack*)event->GetTrack(iTrackN));
    }
    else if( pdgN == -211){
      fV0pions->Add((AliVTrack*)event->GetTrack(iTrackN));
    }
    else if( pdgN == -321){
      fV0kaons->Add((AliVTrack*)event->GetTrack(iTrackN));
    }
    else if( pdgN == -2212){
      fV0protons->Add((AliVTrack*)event->GetTrack(iTrackN));
    }
  

  }
}
//______________________________________________________________________________
void  AliAnalysisTaskPIDqa::ClearV0PIDlist(){

  //
  // Clear the PID object arrays
  //

  fV0electrons->Clear();
  fV0pions->Clear();
  fV0kaons->Clear();
  fV0protons->Clear();

}
//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillITSqa()
{
  //
  // Fill PID qa histograms for the ITS
  //

  AliVEvent *event=InputEvent();
  
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // ITS refit + ITS pid selection
    if (!( ( (status & AliVTrack::kITSrefit)==AliVTrack::kITSrefit ) ||
	   ! ( (status & AliVTrack::kITSpid  )==AliVTrack::kITSpid   ) )) continue;
    Double_t mom=track->P();
    
    TList *theList = 0x0;
    if(( (status & AliVTrack::kTPCin)==AliVTrack::kTPCin )){
      //ITS+TPC tracks
      theList=fListQAits;
    }else{
      if(!( (status & AliVTrack::kITSpureSA)==AliVTrack::kITSpureSA )){ 
	//ITS Standalone tracks
    	theList=fListQAitsSA;
      }else{
	//ITS Pure Standalone tracks
	theList=fListQAitsPureSA;
      }
    }
    
    
    for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
      TH2 *h=(TH2*)theList->At(ispecie);
      if (!h) continue;
      Double_t nSigma=fPIDResponse->NumberOfSigmasITS(track, (AliPID::EParticleType)ispecie);
      h->Fill(mom,nSigma);
    }
    TH2 *h=(TH2*)theList->At(AliPID::kSPECIESC);
    if (h) {
      Double_t sig=track->GetITSsignal();
      h->Fill(mom,sig);
    }
  }
}


//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTPCHistogramsSignal(TList *sublist, Int_t scenario, AliVTrack *track, Int_t mult)
{
  //
  // Fill PID qa histograms for the TPC: Fill the histograms for the TPC signal for different settings
  //

  AliMCEvent *eventMC=MCEvent();  // MC event for MC truth PID

  Double_t mom=0.;      // track momentum
  Double_t eta=0.;      // track eta
  Double_t sig=0.;      // TPC dE/dx signal
  Double_t sigStd=0.;         // TPC dE/dx signal (standard = all ROCs)
  Double_t sigIROC=0.;        // TPC dE/dx signal (IROC) 
  Double_t sigOROCmedium=0.;  // TPC dE/dx signal (OROCmedium) 
  Double_t sigOROClong=0.;    // TPC dE/dx signal (OROClong) 
  Double_t eleLineDist=0.;    // difference between TPC signal and electron expectation
  Int_t trackLabel=0;   // label of the AliVTrack to identify the corresponding MCtrack
  Int_t pdgCode=0;      // pdgcode of MC track for MC truth scenario
  Int_t pdgCodeAbs=0;   // absolute value of pdgcode to get both particles and antiparticles
  Int_t iSigMax=1;      // number of TPC signals (std = 1, set automatically higher if available)
  Int_t nSpecies=0;     // number of particle species under study
  Int_t count=0;        // counter for the number of plot sets for all species (i.e. nsigma vs. p, eta and mult)

  mom=track->GetTPCmomentum();
  eta=track->Eta();
  sigStd=track->GetTPCsignal();

  eleLineDist=sigStd-fPIDResponse->GetTPCResponse().GetExpectedSignal(track,AliPID::kElectron);

  // Get number of particle species (less for V0 candidates = scenarios 40-44)
  if (scenario > 39) nSpecies=(Int_t)AliPID::kSPECIES;
  else nSpecies=(Int_t)AliPID::kSPECIESC;

  // Set number of plot sets for all species
  // (i.e. only nsigma vs. p => count=1; also vs. eta and mult => count=3)
  if ( scenario == 1 || scenario > 39) count=3;
  else count=1;

  // Get MC track ( --> can be deleted if TPC signal is NOT filled for scenario=1 (MC truth)
  if (eventMC) {
    trackLabel=TMath::Abs(track->GetLabel());
    AliVTrack *mcTrack=(AliVTrack*)eventMC->GetTrack(trackLabel);
    pdgCode=mcTrack->PdgCode();
    pdgCodeAbs=TMath::Abs(pdgCode);
  }

  // Get TPC dE/dx info and different TPC signals (IROC, OROCmedium, OROClong)
  AliTPCdEdxInfo* fTPCdEdxInfo = 0x0;
  fTPCdEdxInfo = track->GetTPCdEdxInfo();

  if (fTPCdEdxInfo) {
    sigIROC=fTPCdEdxInfo->GetTPCsignalShortPad();
    sigOROCmedium=fTPCdEdxInfo->GetTPCsignalMediumPad();
    sigOROClong=fTPCdEdxInfo->GetTPCsignalLongPad();
    iSigMax=4;

    //printf("mom = %.3f  sigStd = %.3f  sigIROC = %.3f  sigOROCmedium = %.3f  sigOROClong = %.3f \n",mom,sigStd,sigIROC,sigOROCmedium,sigOROClong);
  }


  // TPC signal for all particles vs. momentum (standard, IROC, OROCmedium, OROClong)
  TH2 *h1std=(TH2*)sublist->At(count*nSpecies+4); 
  if (h1std) {
    h1std->Fill(mom,sigStd);
  }

  TH2 *h1iroc=(TH2*)sublist->At(count*nSpecies+5);
  if ( h1iroc && sigIROC ) {
    h1iroc->Fill(mom,sigIROC);
  }

  TH2 *h1orocm=(TH2*)sublist->At(count*nSpecies+6);
  if  (h1orocm && sigOROCmedium ) {
    h1orocm->Fill(mom,sigOROCmedium);
  }

  TH2 *h1orocl=(TH2*)sublist->At(count*nSpecies+7);
  if ( h1orocl && sigOROClong ) {
    h1orocl->Fill(mom,sigOROClong);
  }


  // - Beginn: MIP pions: TPC signal vs. eta, TPC signal vs. mult -
  if (mom>0.45 && mom<0.5 && sigStd>40 && sigStd<60) {

    Bool_t isPionMC=kTRUE;

    if (scenario == 1) {
      if ( pdgCodeAbs != 211 && pdgCodeAbs != 111 ) isPionMC=kFALSE;
    }

    // MIP pions: TPC signal vs. eta (standard, IROC, OROCmedium, OROClong)
    for (Int_t iSig=0; iSig<iSigMax; iSig++) {
      if (iSig==0) sig=sigStd;
      else if (iSig==1) sig=sigIROC;
      else if (iSig==2) sig=sigOROCmedium;
      else if (iSig==3) sig=sigOROClong;

      TH2 *h2=(TH2*)sublist->At(count*nSpecies+8+iSig);
      if ( h2 && isPionMC ) {
        h2->Fill(eta,sig);
      }
    }

    // MIP pions: TPC signal vs. mult (standard, IROC, OROCmedium, OROClong)
    for (Int_t iSig=0; iSig<iSigMax; iSig++) {
      if (iSig==0) sig=sigStd;
      else if (iSig==1) sig=sigIROC;
      else if (iSig==2) sig=sigOROCmedium;
      else if (iSig==3) sig=sigOROClong;

      TH2 *h3=(TH2*)sublist->At(count*nSpecies+12+iSig);
      if ( h3 && isPionMC && mult > 0 ) {
        h3->Fill(mult,sig);
      }
    }
  } // - End: MIP pions -

  // - Beginn: Electrons: TPC signal vs. eta, TPC signal vs. mult -
  if (mom>0.32 && mom<0.38 && eleLineDist>-10. && eleLineDist<15.) {

    Bool_t isElectronMC=kTRUE;

    if (scenario == 1) {
      if ( pdgCodeAbs != 11 ) isElectronMC=kFALSE;
    }

    // Electrons: TPC signal vs. eta (standard, IROC, OROCmedium, OROClong)
    for (Int_t iSig=0; iSig<iSigMax; iSig++) {
      if (iSig==0) sig=sigStd;
      else if (iSig==1) sig=sigIROC;
      else if (iSig==2) sig=sigOROCmedium;
      else if (iSig==3) sig=sigOROClong;

      TH2 *h4=(TH2*)sublist->At(count*nSpecies+16+iSig);
      if ( h4 && isElectronMC ) {
        h4->Fill(eta,sig);
      }
    }

    // Electrons: TPC signal vs. mult (standard, IROC, OROCmedium, OROClong)
    for (Int_t iSig=0; iSig<iSigMax; iSig++) {
      if (iSig==0) sig=sigStd;
      else if (iSig==1) sig=sigIROC;
      else if (iSig==2) sig=sigOROCmedium;
      else if (iSig==3) sig=sigOROClong;

      TH2 *h5=(TH2*)sublist->At(count*nSpecies+20+iSig);
      if ( h5 && isElectronMC && mult > 0 ) {
        h5->Fill(mult,sig);
      }
    }
  } // - End: Electrons -

}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTPCHistogramsNsigma(TList *sublist, Int_t scenario, AliVTrack *track, Int_t mult)
{
  //
  // Fill PID qa histograms for the TPC: Fill the histograms for TPC Nsigma for different settings
  //

  AliMCEvent *eventMC=MCEvent();  // MC event for MC truth PID

  Double_t mom=0.;      // track momentum
  Double_t eta=0.;      // track eta
  Double_t nSigma=0.;   // number of sigmas wrt. expected signal
  Double_t sig=0.;      // TPC dE/dx signal
  Double_t eleLineDist=0.;  // difference between TPC signal and electron expectation
  Int_t trackLabel=0;   // label of the AliVTrack to identify the corresponding MCtrack
  Int_t pdgCode=0;      // pdgcode of MC track for MC truth scenario
  Int_t pdgCodeAbs=0;   // absolute value of pdgcode to get both particles and antiparticles
  Int_t nSpecies=0;     // number of particle species under study
  Int_t count=0;        // counter for the number of plot sets for all species (i.e. vs. p, eta and mult)

  mom=track->GetTPCmomentum();
  eta=track->Eta();
  sig=track->GetTPCsignal();

  eleLineDist=sig-fPIDResponse->GetTPCResponse().GetExpectedSignal(track,AliPID::kElectron);

  // Get number of particle species (less for V0 candidates = scenarios 40-44)
  if (scenario > 39) nSpecies=(Int_t)AliPID::kSPECIES;
  else nSpecies=(Int_t)AliPID::kSPECIESC;

  // Set number of plot sets for all species
  // (i.e. only vs. p => count=1; also vs. eta and mult => count=3)
  if ( scenario == 1 || scenario > 39 ) count=3;
  else count=1;

  // Get MC track
  if (eventMC) {
    trackLabel=TMath::Abs(track->GetLabel());
    AliVTrack *mcTrack=(AliVTrack*)eventMC->GetTrack(trackLabel);
    pdgCode=mcTrack->PdgCode();
    pdgCodeAbs=TMath::Abs(pdgCode);
  }


  // - Beginn: Nsigma vs. p, vs. eta and vs. multiplicity for different particle species -
  for (Int_t ispecie=0; ispecie<nSpecies; ++ispecie){

    TH2 *h=(TH2*)sublist->At(ispecie);
    if (!h) continue;

    if (scenario == 1) {
      if ( ispecie == 0 && pdgCodeAbs != 11 ) continue;  // Electron
      if ( ispecie == 1 && pdgCodeAbs != 13 ) continue;  // Muon
      if ( ispecie == 2 && pdgCodeAbs != 211 && pdgCodeAbs!=111 ) continue;  // Pion
      if ( ispecie == 3 && pdgCodeAbs != 321 && pdgCodeAbs!=311 ) continue;  // Kaon
      if ( ispecie == 4 && pdgCodeAbs != 2212 ) continue;  // Proton
      if ( ispecie == 5 && pdgCodeAbs != 1000010020 ) continue;  // Deuteron
      if ( ispecie == 6 && pdgCodeAbs != 1000010030 ) continue;  // Triton
      if ( ispecie == 7 && pdgCodeAbs != 1000020030 ) continue;  // Helium-3
      if ( ispecie == 8 && pdgCodeAbs != 1000020040 ) continue;  // Alpha
    }
    else if (scenario > 39) {
      if ( ispecie == 0 && scenario != 40 ) continue;  // Electron
      if ( ispecie == 1 ) continue;  // Muon
      if ( ispecie == 2 && scenario != 42 ) continue;  // Pion
      if ( ispecie == 3 && scenario != 43 ) continue;  // Kaon
      if ( ispecie == 4 && scenario != 44 ) continue;  // Proton
    }

    if (scenario == 2) {
      nSigma=fPIDResponse->NumberOfSigmasTPC(track, (AliPID::EParticleType)ispecie, AliTPCPIDResponse::kdEdxHybrid);
    }
    else if (scenario == 3) {
      nSigma=fPIDResponse->NumberOfSigmasTPC(track, (AliPID::EParticleType)ispecie, AliTPCPIDResponse::kdEdxOROC);
    }
    else {
      nSigma=fPIDResponse->NumberOfSigmasTPC(track, (AliPID::EParticleType)ispecie);
    }

    h->Fill(mom,nSigma);

    if (count == 3) {
      TH2 *hEta=(TH2*)sublist->At(ispecie+nSpecies);
      TH2 *hMult=(TH2*)sublist->At(ispecie+2*nSpecies);
 
      if ( hEta ) hEta->Fill(eta,nSigma);
      if ( hMult && mult > 0 ) hMult->Fill(mult,nSigma);
    }
  } // - End: different particle species -


  // -- Beginn: Fill histograms for MIP pions and electrons (only for some scenarios) --
  if ( scenario == 0 || scenario == 2 || scenario == 3 ) {

    // - Beginn: MIP pions: Nsigma vs. eta, Nsigma vs. mult -
    if (mom>0.45 && mom<0.5 && sig>40 && sig<60) {

      Bool_t isPionMC=kTRUE;

      TH2 *h1=(TH2*)sublist->At(count*nSpecies);
      if (h1) {
        if (scenario == 1) {
          if ( pdgCodeAbs != 211 && pdgCodeAbs != 111 ) isPionMC=kFALSE;
          if (isPionMC) {
            nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);
          }
        }
        else if (scenario == 2) {
          nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion, AliTPCPIDResponse::kdEdxHybrid);
        }
        else if (scenario == 3) {
          nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion, AliTPCPIDResponse::kdEdxOROC);
        }
        else nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);

        if (isPionMC) h1->Fill(eta,nSigma);
      }

      TH2 *h2m=(TH2*)sublist->At(count*nSpecies+1);
      if ( h2m && isPionMC && mult > 0 ) {
        h2m->Fill(mult,nSigma);
      }
   
    } // - End: MIP pions -

    // - Beginn: Electrons: Nsigma vs. eta, Nsigma vs. mult -
    if (mom>0.32 && mom<0.38 && eleLineDist>-10. && eleLineDist<15.) {

      Bool_t isElectronMC=kTRUE;

      TH2 *h3=(TH2*)sublist->At(count*nSpecies+2);
      if (h3) {
        if (scenario == 1) {
          if ( pdgCodeAbs != 11 ) isElectronMC=kFALSE;
          if (isElectronMC) {
            nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron);
          }
        }
        if (scenario == 2) {
          nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron, AliTPCPIDResponse::kdEdxHybrid);
        }
        else if (scenario == 3) {
          nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron, AliTPCPIDResponse::kdEdxOROC);
        }
        else nSigma=fPIDResponse->NumberOfSigmasTPC(track, AliPID::kElectron);

        if (isElectronMC) h3->Fill(eta,nSigma);
      }

      TH2 *h4m=(TH2*)sublist->At(count*nSpecies+3);
      if ( h4m && isElectronMC && mult > 0 ) {
        h4m->Fill(mult,nSigma);
      }

    } // - End: Electrons -
  } // -- End: Fill histograms for MIP pions and electrons --

}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTPCqa()
{
  //
  // Fill PID qa histograms for the TPC
  //

  // switches for the different scenarios
  Bool_t scBasic=1;     // default/basic
  Bool_t scMCtruth=1;   // for MC truth tracks
  Bool_t scHybrid=1;    // for hybrid PID (only LHC11h)
  Bool_t scOROChigh=1;  // only OROC signal (only LHC11h)
  Bool_t scV0=1;        // for V0 candidates (only for ESDs available)
  Int_t scCounter=0;    // counter of scenarios, used for the histograms at the end of FillTPCqa

  // input handler
  AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler *inputHandler=dynamic_cast<AliInputEventHandler*>(man->GetInputEventHandler());
  if (!inputHandler) AliFatal("Input handler needed");

  AliVEvent *event=InputEvent();

  // ESD or AOD event needed to get reference multiplicity (not in AliVEvent)
  AliAODEvent *fAODevent = 0x0;   // AOD event
  AliESDEvent *fESDevent = 0x0;   // ESD event
  AliESDtrackCuts *esdTrackCuts = 0x0;  // ESD track Cuts (ref mult is in AliESDtrackCuts)

  Double_t eta=0.;    // track eta
  Int_t mult=0;       // event multiplicity (TPConlyRefMult)
  //Int_t nacc=0;       // counter for accepted multiplicity

  // Check for MC
  scMCtruth=(MCEvent()!=0x0);

  // Check if period is data LHC11h by checking if
  // the splines for ALLhigh have been set by AliPIDResponse
  AliTPCPIDResponse &tpcResp=fPIDResponse->GetTPCResponse();
  if (tpcResp.GetResponseFunction(AliPID::kPion, AliTPCPIDResponse::kALLhigh)==0x0) {
    scHybrid   = kFALSE;
    scOROChigh = kFALSE;
  }

  // Check if "ESD" or "AOD" and get the corresponding event and the beam type (or centrality)
  TString analysisType = inputHandler->GetDataType(); // can be "ESD" or "AOD"
  if (analysisType == "ESD") {
    fESDevent = dynamic_cast<AliESDEvent*>( InputEvent() );
    esdTrackCuts = new AliESDtrackCuts("esdTrackCuts");
    //printf("\n--- New event - event type = ESD \n");
  }
  else if (analysisType == "AOD") {
    fAODevent = dynamic_cast<AliAODEvent*>( InputEvent() );
    //printf("\n--- New event - event type = AOD \n");

    // disable V0 scenario, because V0s are not available for AODs in this current implementation
    scV0=0;
  }

  // Check if Basic list is already created
  // If not: Go to SetupTPCqa and creat lists and histograms
  if(!fListQAtpcBasic) {
    //printf("\n--- No list QA TPC Basic found -> go to SetupTPCqa! ---\n");
    SetupTPCqa(scMCtruth, scHybrid, scV0);
  }

  // Get the number of scenarios by counting those, which are switched on
  if (scBasic) scCounter++;
  if (scMCtruth) scCounter++;
  if (scHybrid) scCounter++;
  if (scOROChigh) scCounter++;
  if (scV0) scCounter++;

  // Get reference multiplicity for ESDs
  if ( analysisType == "ESD" && esdTrackCuts ) {
    mult=esdTrackCuts->GetReferenceMultiplicity(fESDevent,kTRUE);
  }

  // Get reference multiplicity for AODs
  if ( analysisType == "AOD" && fAODevent ) {
    AliAODHeader * header=dynamic_cast<AliAODHeader*>(fAODevent->GetHeader());
    if(!header) AliFatal("Not a standard AOD");
    mult=header->GetTPConlyRefMultiplicity();
  }

  /*if (mult < 0) {
    printf("Reference multiplicity not available \n");
    //return;
  }*/

  //printf("The multiplicity is = %i ",mult);


  // -- Begin: track loop --
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // TPC refit + ITS refit + TPC pid
    if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

    // The TPC pid cut removes the light nuclei (>5 sigma from proton line)
    //||        !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  )
    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }

    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

    eta=track->Eta();
    if ( TMath::Abs(eta)>0.9 ) continue;

    //nacc++; // counter for accepted multiplicity

    // the default ("basic") scenario
    if (scBasic == 1) {
      FillTPCHistogramsNsigma(fListQAtpcBasic,0,track,mult);
      FillTPCHistogramsSignal(fListQAtpcBasic,0,track,mult);
    }

    // only MC truth identified particles
    if (scMCtruth == 1) {
      FillTPCHistogramsNsigma(fListQAtpcMCtruth,1,track,mult);
    }

    // the "hybrid" scenario (only for LHC11h)
    if (scHybrid == 1) {
      FillTPCHistogramsNsigma(fListQAtpcHybrid,2,track,mult);
    }

    // the "OROC high" scenario (only for LHC11h)
    if (scOROChigh == 1) {
      FillTPCHistogramsNsigma(fListQAtpcOROChigh,3,track,mult);
    }

  } // -- End: track loop --


  // -- Begin: track loops for V0 candidates --
  if (scV0 == 1) {

    // - Begin: track loop for electrons from V0 -
    for(Int_t itrack = 0; itrack < fV0electrons->GetEntries(); itrack++){
      AliVTrack *track=(AliVTrack*)fV0electrons->At(itrack);

      //
      //basic track cuts
      //
      ULong_t status=track->GetStatus();
      // not that nice. status bits not in virtual interface
      // TPC refit + ITS refit + TPC pid
      if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
          !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

      // The TPC pid cut removes the light nuclei (>5 sigma from proton line)
      //||        !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  )
      Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
      Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
      if (track->GetTPCNclsF()>0) {
        ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
      }

      if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

      eta=track->Eta();
      if ( TMath::Abs(eta)>0.9 ) continue;

      // fill histograms for V0 candidates
      FillTPCHistogramsNsigma(fListQAtpcV0,40,track,mult);

    } // - End: track loop for electrons from V0 -


    // - Begin: track loop for pions from V0 -
    for(Int_t itrack = 0; itrack < fV0pions->GetEntries(); itrack++){
      AliVTrack *track=(AliVTrack*)fV0pions->At(itrack);

      //
      //basic track cuts
      //
      ULong_t status=track->GetStatus();
      // not that nice. status bits not in virtual interface
      // TPC refit + ITS refit + TPC pid
      if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
          !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

      // The TPC pid cut removes the light nuclei (>5 sigma from proton line)
      //||        !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  )
      Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
      Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
      if (track->GetTPCNclsF()>0) {
        ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
      }

      if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

      eta=track->Eta();
      if ( TMath::Abs(eta)>0.9 ) continue;

      // fill histograms for V0 candidates
      FillTPCHistogramsNsigma(fListQAtpcV0,42,track,mult);

    } // - End: track loop for pions from V0 -


    // - Begin: track loop for kaons from V0 -
    for(Int_t itrack = 0; itrack < fV0kaons->GetEntries(); itrack++){
      AliVTrack *track=(AliVTrack*)fV0kaons->At(itrack);

      //
      //basic track cuts
      //
      ULong_t status=track->GetStatus();
      // not that nice. status bits not in virtual interface
      // TPC refit + ITS refit + TPC pid
      if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
          !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

      // The TPC pid cut removes the light nuclei (>5 sigma from proton line)
      //||        !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  )
      Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
      Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
      if (track->GetTPCNclsF()>0) {
        ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
      }

      if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

      eta=track->Eta();
      if ( TMath::Abs(eta)>0.9 ) continue;

      // fill histograms for V0 candidates
      FillTPCHistogramsNsigma(fListQAtpcV0,43,track,mult);

    } // - End: track loop for kaons from V0 -


    // - Begin: track loop for protons from V0 -
    for(Int_t itrack = 0; itrack < fV0protons->GetEntries(); itrack++){
      AliVTrack *track=(AliVTrack*)fV0protons->At(itrack);

      //
      //basic track cuts
      //
      ULong_t status=track->GetStatus();
      // not that nice. status bits not in virtual interface
      // TPC refit + ITS refit + TPC pid
      if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
          !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

      // The TPC pid cut removes the light nuclei (>5 sigma from proton line)
      //||        !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  )
      Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
      Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
      if (track->GetTPCNclsF()>0) {
        ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
      }

      if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

      eta=track->Eta();
      if ( TMath::Abs(eta)>0.9 ) continue;

      // fill histograms for V0 candidates
      FillTPCHistogramsNsigma(fListQAtpcV0,44,track,mult);

    } // - End: track loop for protons from V0 -

  } // -- End: track loops for V0 candidates --


  // Multiplicity distribution
  TH1 *hm=(TH1*)fListQAtpc->At(scCounter);
  if (hm) {
    hm->Fill(mult);
  }

  //printf("\nAccepted multiplicity = %i \n --- END of event --- \n",nacc);

}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTRDqa()
{
  //
  // Fill PID qa histograms for the TRD
  //
  AliVEvent *event=InputEvent();
  Int_t ntracks = event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack <  ntracks; itrack++){
    AliVTrack *track = (AliVTrack *)event->GetTrack(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // TPC refit + ITS refit + TPC pid + TRD out
    if (!( (status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !( (status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ||
//         !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid  ) || //removes light nuclei. So it is out for the moment
        !( (status & AliVTrack::kTRDout  ) == AliVTrack::kTRDout  )) continue;
    
    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }
    
    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

    Double_t likelihoods[AliPID::kSPECIES];
    if(fPIDResponse->ComputeTRDProbability(track, AliPID::kSPECIES, likelihoods) != AliPIDResponse::kDetPidOk) continue;
    Int_t ntracklets = 0;
    Double_t momentum = -1.;
    for(Int_t itl = 0; itl < 6; itl++) {
      if(track->GetTRDmomentum(itl) > 0.) {
        ntracklets++;
        if(momentum < 0) momentum = track->GetTRDmomentum(itl);
      }
    }
    
    for(Int_t ispecie = 0; ispecie < AliPID::kSPECIES; ispecie++){
      TH2F *hLike = (TH2F *)fListQAtrd->At(ntracklets*AliPID::kSPECIES+ispecie);
      if (hLike) hLike->Fill(momentum,likelihoods[ispecie]);
    }

    //=== nSigma and signal ===
    for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
      TH2 *h=(TH2*)fListQAtrdNsig->At(ispecie);
      TH2 *hTPCTOF=(TH2*)fListQAtrdNsigTPCTOF->At(ispecie);
      if (!h || !hTPCTOF) continue;
      Float_t nSigmaTPC=fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, track, (AliPID::EParticleType)ispecie);
      Float_t nSigmaTRD=fPIDResponse->NumberOfSigmas(AliPIDResponse::kTRD, track, (AliPID::EParticleType)ispecie);
      Float_t nSigmaTOF=fPIDResponse->NumberOfSigmas(AliPIDResponse::kTOF, track, (AliPID::EParticleType)ispecie);
      h->Fill(momentum,nSigmaTRD);

      if (TMath::Abs(nSigmaTPC)<3 && TMath::Abs(nSigmaTOF)<3) {
        hTPCTOF->Fill(momentum,nSigmaTRD);
      }
    }

    TH2 *h=(TH2*)fListQAtrdNsig->Last();
    
    if (h) {
      Double_t sig=track->GetTRDsignal();
      h->Fill(momentum,sig);
    }
    
  }
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTOFqa()
{
  //
  // Fill TOF information
  //
  AliVEvent *event=InputEvent();

  Int_t ntracks=event->GetNumberOfTracks();
  Int_t tracksAtTof = 0;
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // TPC refit + ITS refit +
    // TOF out + kTIME
    // kTIME
    // (we don't use kTOFmismatch because it depends on TPC and kTOFpid because it prevents light nuclei
    if (!((status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !((status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ||
        !((status & AliVTrack::kTOFout  ) == AliVTrack::kTOFout  ) ||
	//        !((status & AliVTrack::kTOFpid  ) == AliVTrack::kTOFpid  ) ||
        !((status & AliVTrack::kTIME    ) == AliVTrack::kTIME    ) ) continue;

    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }

    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

    tracksAtTof++;

    Double_t mom=track->P();

    for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
      TH2 *h=(TH2*)fListQAtof->At(ispecie);
      if (!h) continue;
      Double_t nSigma=fPIDResponse->NumberOfSigmasTOF(track, (AliPID::EParticleType)ispecie);
      h->Fill(mom,nSigma);
    }

    TH2 *h=(TH2*)fListQAtof->FindObject("hSigP_TOF");
    if (h) {
      Double_t sig=track->GetTOFsignal()/1000.;
      h->Fill(mom,sig);
    }

    Int_t mask = fPIDResponse->GetTOFResponse().GetStartTimeMask(mom);
    ((TH1F*)fListQAtof->FindObject("hStartTimeMask_TOF"))->Fill((Double_t)(mask+0.5));

    if (mom >= 0.75 && mom <= 1.25 ) {
      Double_t nsigma= fPIDResponse->NumberOfSigmasTOF(track, (AliPID::EParticleType)AliPID::kPion);
      if (mask == 0) {
	((TH1F*)fListQAtof->FindObject("hNsigma_TOF_Pion_T0-Fill"))->Fill(nsigma);
      } else if (mask == 1) {
	((TH1F*)fListQAtof->FindObject("hNsigma_TOF_Pion_T0-TOF"))->Fill(nsigma);
      } else if ( (mask == 2) || (mask == 4) || (mask == 6) ) {
	((TH1F*)fListQAtof->FindObject("hNsigma_TOF_Pion_T0-T0"))->Fill(nsigma);
      } else {
	((TH1F*)fListQAtof->FindObject("hNsigma_TOF_Pion_T0-Best"))->Fill(nsigma);
      }
      if (mask & 0x1) { //at least TOF-T0 present
	Double_t delta=0;
	(void)fPIDResponse->GetSignalDelta((AliPIDResponse::EDetector)AliPIDResponse::kTOF,track,(AliPID::EParticleType)AliPID::kPion,delta);
	((TH1F*)fListQAtof->FindObject("hDelta_TOF_Pion"))->Fill(delta);
      }
    }

    Double_t res = (Double_t)fPIDResponse->GetTOFResponse().GetStartTimeRes(mom);
    ((TH1F*)fListQAtof->FindObject("hStartTimeRes_TOF"))->Fill(res);

    Double_t startTimeT0 = event->GetT0TOF(0);
    if (startTimeT0 < 90000) ((TH1F*)fListQAtof->FindObject("hStartTimeAC_T0"))->Fill(startTimeT0);
    else {
      startTimeT0 = event->GetT0TOF(1);
      if (startTimeT0 < 90000) ((TH1F*)fListQAtof->FindObject("hStartTimeA_T0"))->Fill(startTimeT0);
      startTimeT0 = event->GetT0TOF(2);
      if (startTimeT0 < 90000) ((TH1F*)fListQAtof->FindObject("hStartTimeC_T0"))->Fill(startTimeT0);
    }
  }
  if (tracksAtTof > 0) {
    ((TH1F* )fListQAtof->FindObject("hnTracksAt_TOF"))->Fill(tracksAtTof);
    Int_t mask = fPIDResponse->GetTOFResponse().GetStartTimeMask(5.);
    if (mask & 0x1) ((TH1F*)fListQAtof->FindObject("hT0MakerEff"))->Fill(tracksAtTof);
  }
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillT0qa()
{
  //
  // Fill TOF information
  //
  AliVEvent *event=InputEvent();

  Int_t ntracks=event->GetNumberOfTracks();

  Int_t tracksAtT0 = 0;

  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // TPC refit + ITS refit +
    if (!((status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !((status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;
    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }
    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;

    tracksAtT0++;
  }

  Bool_t t0A = kFALSE;
  Bool_t t0C = kFALSE;
  Bool_t t0And = kFALSE;
  Double_t startTimeT0 = event->GetT0TOF(0);     // AND
  if (startTimeT0 < 90000) {
    t0And = kTRUE;
    ((TH1F*)fListQAt0->FindObject("hStartTimeAC_T0"))->Fill(startTimeT0);
    }
  startTimeT0 = event->GetT0TOF(1);             // T0A 
  if (startTimeT0 < 90000) {
    t0A = kTRUE;
    ((TH1F*)fListQAt0->FindObject("hStartTimeA_T0"))->Fill(startTimeT0);
    
  }
  startTimeT0 = event->GetT0TOF(2);             // T0C 
  if (startTimeT0 < 90000) {
    t0C = kTRUE;
    ((TH1F*)fListQAt0->FindObject("hStartTimeC_T0"))->Fill(startTimeT0);
  }
  
  ((TH1F* )fListQAt0->FindObject("hnTracksAt_T0"))->Fill(tracksAtT0);
  if (t0A) ((TH1F*)fListQAt0->FindObject("hT0AEff"))->Fill(tracksAtT0);
  if (t0C) ((TH1F*)fListQAt0->FindObject("hT0CEff"))->Fill(tracksAtT0);
  if (t0And) ((TH1F*)fListQAt0->FindObject("hT0AndEff"))->Fill(tracksAtT0);
  if (t0A || t0C) ((TH1F*)fListQAt0->FindObject("hT0OrEff"))->Fill(tracksAtT0);
}


//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillEMCALqa()
{
  //
  // Fill PID qa histograms for the EMCAL
  //

  AliVEvent *event=InputEvent();
  
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);
    
    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    if (!( (status & AliVTrack::kEMCALmatch) == AliVTrack::kEMCALmatch) ) continue;

    Double_t pt=track->Pt();
   
    //EMCAL nSigma (only for electrons at the moment)
    TH2 *h=(TH2*)fListQAemcal->At(0);
    if (!h) continue;
    Double_t nSigma=fPIDResponse->NumberOfSigmasEMCAL(track, (AliPID::EParticleType)0);
    h->Fill(pt,nSigma);
    
  }

   //EMCAL signal (E/p vs. pT) for electrons from V0
  for(Int_t itrack = 0; itrack < fV0electrons->GetEntries(); itrack++){
    AliVTrack *track=(AliVTrack*)fV0electrons->At(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    if (!( (status & AliVTrack::kEMCALmatch) == AliVTrack::kEMCALmatch) ) continue;

    Double_t pt=track->Pt();

    TH2 *h=(TH2*)fListQAemcal->At(1);
    if (h) {

      Int_t nMatchClus = track->GetEMCALcluster();
      Double_t mom     = track->P();
      Double_t eop     = -1.;

      if(nMatchClus > -1){
    
        AliVCluster *matchedClus = (AliVCluster*)event->GetCaloCluster(nMatchClus);

        if(matchedClus){

          // matched cluster is EMCAL
          if(matchedClus->IsEMCAL()){

            Double_t fClsE       = matchedClus->E();
            eop                  = fClsE/mom;

            h->Fill(pt,eop);

          }
        }
      }
    }
  }

   //EMCAL signal (E/p vs. pT) for pions from V0
  for(Int_t itrack = 0; itrack < fV0pions->GetEntries(); itrack++){
    AliVTrack *track=(AliVTrack*)fV0pions->At(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    if (!( (status & AliVTrack::kEMCALmatch) == AliVTrack::kEMCALmatch) ) continue;

    Double_t pt=track->Pt();

    TH2 *h=(TH2*)fListQAemcal->At(2);
    if (h) {

      Int_t nMatchClus = track->GetEMCALcluster();
      Double_t mom     = track->P();
      Double_t eop     = -1.;

      if(nMatchClus > -1){
    
        AliVCluster *matchedClus = (AliVCluster*)event->GetCaloCluster(nMatchClus);

        if(matchedClus){

          // matched cluster is EMCAL
          if(matchedClus->IsEMCAL()){

            Double_t fClsE       = matchedClus->E();
            eop                  = fClsE/mom;

            h->Fill(pt,eop);

          }
        }
      }
    }
  }

   //EMCAL signal (E/p vs. pT) for protons from V0
  for(Int_t itrack = 0; itrack < fV0protons->GetEntries(); itrack++){
    AliVTrack *track=(AliVTrack*)fV0protons->At(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    if (!( (status & AliVTrack::kEMCALmatch) == AliVTrack::kEMCALmatch) ) continue;

    Double_t pt=track->Pt();

    TH2 *hP=(TH2*)fListQAemcal->At(3);
    TH2 *hAP=(TH2*)fListQAemcal->At(4);
    if (hP && hAP) {

      Int_t nMatchClus = track->GetEMCALcluster();
      Double_t mom     = track->P();
      Int_t charge     = track->Charge();	      
      Double_t eop     = -1.;

      if(nMatchClus > -1){
    
        AliVCluster *matchedClus = (AliVCluster*)event->GetCaloCluster(nMatchClus);

        if(matchedClus){

          // matched cluster is EMCAL
          if(matchedClus->IsEMCAL()){

            Double_t fClsE       = matchedClus->E();
            eop                  = fClsE/mom;

            if(charge > 0)      hP->Fill(pt,eop);
            else if(charge < 0) hAP->Fill(pt,eop);

          }
        }
      }
    }
  }

}


//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillHMPIDqa()
{
  //
  // Fill PID qa histograms for the HMPID
  //
  
  AliVEvent *event=InputEvent();
  
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);
    
    //
    //basic track cuts
    //
    const ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // TPC refit + ITS refit +
    // TOF out + TOFpid +
    // kTIME
    if (!((status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !((status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ) continue;

    const Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }

    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;
    
    const Double_t mom = track->P();
    const Double_t ckovAngle = track->GetHMPIDsignal();

    Int_t nhists=0;
    for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
      if (ispecie==AliPID::kElectron || ispecie==AliPID::kMuon) continue;
      TH2 *h=(TH2*)fListQAhmpid->At(nhists);
      if (!h) {++nhists; continue;}
      const Double_t nSigma=fPIDResponse->NumberOfSigmasHMPID(track, (AliPID::EParticleType)ispecie);
      h->Fill(mom,nSigma);
      ++nhists;
    }
    
    TH1F *hThetavsMom = (TH1F*)fListQAhmpid->At(AliPID::kSPECIESC);
    
    if (hThetavsMom) hThetavsMom->Fill(mom,ckovAngle);
  
  }
}
//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTOFHMPIDqa()
{
  //
  // Fill PID qa histograms for the HMPID
  //
  
  AliVEvent *event=InputEvent();
  
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);
    
    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // TPC refit + ITS refit +
    // TOF out + TOFpid +
    // kTIME
    if (!((status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !((status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ||
        !((status & AliVTrack::kTOFout  ) == AliVTrack::kTOFout  ) ||
        !((status & AliVTrack::kTOFpid  ) == AliVTrack::kTOFpid  ) ||
        !((status & AliVTrack::kTIME    ) == AliVTrack::kTIME    ) ) continue;

    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }

    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;
    
    Double_t mom = track->P();
    Double_t ckovAngle = track->GetHMPIDsignal();
    
    Double_t nSigmaTOF[3]; 
    TH1F *h[3];
    
    for (Int_t ispecie=2; ispecie<5; ++ispecie){
      //TOF nSigma
      nSigmaTOF[ispecie-2]=fPIDResponse->NumberOfSigmasTOF(track, (AliPID::EParticleType)ispecie);
      h[ispecie-2] = (TH1F*)fListQAtofhmpid->At(ispecie-2);}
      
    if(TMath::Abs(nSigmaTOF[0])<2)                                                              h[0]->Fill(mom,ckovAngle);
    
    if(TMath::Abs(nSigmaTOF[1])<2 && TMath::Abs(nSigmaTOF[0])>3)                                h[1]->Fill(mom,ckovAngle);

    if(TMath::Abs(nSigmaTOF[2])<2 && TMath::Abs(nSigmaTOF[1])>3 && TMath::Abs(nSigmaTOF[0])>3)  h[2]->Fill(mom,ckovAngle);
      
  }
  
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::FillTPCTOFqa()
{
  //
  // Fill PID qa histograms for the TOF
  //   Here also the TPC histograms after TOF selection are filled
  //

  AliVEvent *event=InputEvent();

  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    AliVTrack *track=(AliVTrack*)event->GetTrack(itrack);

    //
    //basic track cuts
    //
    ULong_t status=track->GetStatus();
    // not that nice. status bits not in virtual interface
    // TPC refit + ITS refit +
    // TOF out + TOFpid +
    // kTIME
    if (!((status & AliVTrack::kTPCrefit) == AliVTrack::kTPCrefit) ||
        !((status & AliVTrack::kITSrefit) == AliVTrack::kITSrefit) ||
//         !( (status & AliVTrack::kTPCpid  ) == AliVTrack::kTPCpid ) || //removes light nuclei, so it is out for the moment
        !((status & AliVTrack::kTOFout  ) == AliVTrack::kTOFout  ) ||
        !((status & AliVTrack::kTOFpid  ) == AliVTrack::kTOFpid  ) ||
        !((status & AliVTrack::kTIME    ) == AliVTrack::kTIME    ) ) continue;

    Float_t nCrossedRowsTPC = track->GetTPCClusterInfo(2,1);
    Float_t  ratioCrossedRowsOverFindableClustersTPC = 1.0;
    if (track->GetTPCNclsF()>0) {
      ratioCrossedRowsOverFindableClustersTPC = nCrossedRowsTPC/track->GetTPCNclsF();
    }

    if ( nCrossedRowsTPC<70 || ratioCrossedRowsOverFindableClustersTPC<.8 ) continue;


    Double_t mom=track->P();
    Double_t momTPC=track->GetTPCmomentum();

    for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
      //TOF nSigma
      Double_t nSigmaTOF=fPIDResponse->NumberOfSigmasTOF(track, (AliPID::EParticleType)ispecie);
      Double_t nSigmaTPC=fPIDResponse->NumberOfSigmasTPC(track, (AliPID::EParticleType)ispecie);

      //TPC after TOF cut
      TH2 *h=(TH2*)fListQAtpctof->At(ispecie);
      if (h && TMath::Abs(nSigmaTOF)<3.) h->Fill(momTPC,nSigmaTPC);

      //TOF after TPC cut
      h=(TH2*)fListQAtpctof->At(ispecie+AliPID::kSPECIESC);
      if (h && TMath::Abs(nSigmaTPC)<3.) h->Fill(mom,nSigmaTOF);

      //EMCAL after TOF and TPC cut
      h=(TH2*)fListQAtpctof->At(ispecie+2*AliPID::kSPECIESC);
      if (h && TMath::Abs(nSigmaTOF)<3. && TMath::Abs(nSigmaTPC)<3. ){

	Int_t nMatchClus = track->GetEMCALcluster();
	Double_t pt      = track->Pt();
	Double_t eop     = -1.;
	
	if(nMatchClus > -1){
	  
	  AliVCluster *matchedClus = (AliVCluster*)event->GetCaloCluster(nMatchClus);
	  
	  if(matchedClus){
	    
	    // matched cluster is EMCAL
	    if(matchedClus->IsEMCAL()){
	      
	      Double_t fClsE       = matchedClus->E();
	      eop                  = fClsE/mom;

	      h->Fill(pt,eop);
 
	      
	    }
	  }
	}
      }
    }
  }
}

//_____________________________________________________________________________
void AliAnalysisTaskPIDqa::FillQAinfo()
{
  //
  // Fill the QA information
  //


  //TPC QA info
  TObjArray *arrTPC=static_cast<TObjArray*>(fListQAinfo->At(0));
  if (fPIDResponse && arrTPC){
    AliTPCPIDResponse &tpcResp=fPIDResponse->GetTPCResponse();
    // fill spline names
    if (!arrTPC->UncheckedAt(0)){
      
      TObjArray *arrTPCsplineNames=new TObjArray(AliPID::kSPECIESC);
      arrTPCsplineNames->SetOwner();
      arrTPCsplineNames->SetName("TPC_spline_names");
      arrTPC->AddAt(arrTPCsplineNames,0);
      
      for (Int_t iresp=0; iresp<AliPID::kSPECIESC; ++iresp){
        const TObject *o=tpcResp.GetResponseFunction((AliPID::EParticleType)iresp);
        if (!o) continue;
        arrTPCsplineNames->Add(new TObjString(Form("%02d: %s",iresp, o->GetName())));
      }
    }

    // tpc response config
    if (!arrTPC->UncheckedAt(1)){
      
      TObjArray *arrTPCconfigInfo=new TObjArray;
      arrTPCconfigInfo->SetOwner();
      arrTPCconfigInfo->SetName("TPC_config_info");
      arrTPC->AddAt(arrTPCconfigInfo,1);

      TObjString *ostr=0x0;
      ostr=new TObjString;
      ostr->String().Form("Eta Corr map: %s", tpcResp.GetEtaCorrMap()?tpcResp.GetEtaCorrMap()->GetName():"none");
      arrTPCconfigInfo->Add(ostr);

      ostr=new TObjString;
      ostr->String().Form("Sigma Par map: %s", tpcResp.GetSigmaPar1Map()?tpcResp.GetSigmaPar1Map()->GetName():"none");
      arrTPCconfigInfo->Add(ostr);

      ostr=new TObjString;
      ostr->String().Form("MIP: %.2f", tpcResp.GetMIP());
      arrTPCconfigInfo->Add(ostr);
      
      ostr=new TObjString;
      ostr->String().Form("Res: Def %.3g (%.3g) : AllHigh %.3g (%.3g) : OROC high %.3g (%.3g)",
                          tpcResp.GetRes0(AliTPCPIDResponse::kDefault), tpcResp.GetResN2(AliTPCPIDResponse::kDefault),
                          tpcResp.GetRes0(AliTPCPIDResponse::kALLhigh), tpcResp.GetResN2(AliTPCPIDResponse::kALLhigh),
                          tpcResp.GetRes0(AliTPCPIDResponse::kOROChigh), tpcResp.GetResN2(AliTPCPIDResponse::kOROChigh)
                         );
      arrTPCconfigInfo->Add(ostr);
    }
  }
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupITSqa()
{
  //
  // Create the ITS qa objects
  //
  
  TVectorD *vX=MakeLogBinning(200,.1,30);
  
  //ITS+TPC tracks
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_ITS_%s",AliPID::ParticleName(ispecie)),
                              Form("ITS n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    fListQAits->Add(hNsigmaP);
  }
  TH2F *hSig = new TH2F("hSigP_ITS",
                        "ITS signal vs. p;p [GeV]; ITS signal [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        300,0,300);
  fListQAits->Add(hSig);

  //ITS Standalone tracks
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaPSA = new TH2F(Form("hNsigmaP_ITSSA_%s",AliPID::ParticleName(ispecie)),
				Form("ITS n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
				vX->GetNrows()-1,vX->GetMatrixArray(),
				200,-10,10);
    fListQAitsSA->Add(hNsigmaPSA);
  }
  TH2F *hSigSA = new TH2F("hSigP_ITSSA",
			  "ITS signal vs. p;p [GeV]; ITS signal [arb. units]",
			  vX->GetNrows()-1,vX->GetMatrixArray(),
			  300,0,300);
  fListQAitsSA->Add(hSigSA);
  
  //ITS Pure Standalone tracks
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaPPureSA = new TH2F(Form("hNsigmaP_ITSPureSA_%s",AliPID::ParticleName(ispecie)),
				    Form("ITS n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
				    vX->GetNrows()-1,vX->GetMatrixArray(),
				    200,-10,10);
    fListQAitsPureSA->Add(hNsigmaPPureSA);
  }
  TH2F *hSigPureSA = new TH2F("hSigP_ITSPureSA",
			      "ITS signal vs. p;p [GeV]; ITS signal [arb. units]",
			      vX->GetNrows()-1,vX->GetMatrixArray(),
			      300,0,300);
  fListQAitsPureSA->Add(hSigPureSA);
  
  delete vX;  
}

//_____________________________________________________________________________
void AliAnalysisTaskPIDqa::AddTPCHistogramsSignal(TList *sublist, const char *scenario)
{
  //
  // Create the TPC qa objects: create histograms for the TPC signal for different settings
  //

  TVectorD *vX=MakeLogBinning(200,.1,30);
  Int_t nBinsMult = 38;
  Double_t xBinsMult[39] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 
                           120, 140, 160, 180, 200, 
                           300, 400, 500, 600, 700, 800, 900, 1000, 
                           1200, 1400, 1600, 1800, 2000, 
                           2200, 2400, 2600, 2800, 3000, 
                           3200, 3400, 3600, 3800, 4000
                           };
  const Int_t binsEta=110;
  Float_t etaMin=-1.1;
  Float_t etaMax=1.1;

  char signal[4][12]={"std","IROC","OROCmedium","OROClong"};


  // TPC signal vs. p for all particles (standard, IROC, OROCmedium, OROClong)
  for (Int_t iSig=0; iSig<4; iSig++) {
    TH2F *hSigP = new TH2F(Form("hSigP_TPC_%s_%s",signal[iSig],scenario),
                             Form("TPC_%s signal (%s) vs. p;p [GeV]; TPC signal [arb. units]",scenario,signal[iSig]),
                             vX->GetNrows()-1,vX->GetMatrixArray(),
                             300,0,300);
    sublist->Add(hSigP);
  }

  // MIP pions: TPC signal vs. eta
  for (Int_t iSig=0; iSig<4; iSig++) {
    TH2F *hSigEtaMIPpi = new TH2F(Form("hSigEta_TPC_%s_%s_MIPpi",signal[iSig],scenario),
                                  Form("TPC_%s signal (%s) MIPpi vs. eta;#eta;TPC signal [arb. units]",scenario,signal[iSig]),
                                  binsEta,etaMin,etaMax,
                                  300,0,300);
    sublist->Add(hSigEtaMIPpi);
  }

  // MIP pions: TPC signal vs. multiplicity
  for (Int_t iSig=0; iSig<4; iSig++) {
    TH2F *hSigMultMPIpi = new TH2F(Form("hSigMult_TPC_%s_%s_MIPpi",signal[iSig],scenario),
                                   Form("TPC_%s signal (%s) MIPpi vs. mult;multiplicity;TPC signal [arb. units]",scenario,signal[iSig]),
                                   nBinsMult,xBinsMult,
                                   300,0,300);
    sublist->Add(hSigMultMPIpi);
  }
 
  // Electrons: TPC signal vs. eta
  for (Int_t iSig=0; iSig<4; iSig++) {
    TH2F *hSigEtaEle = new TH2F(Form("hSigEta_TPC_%s_%s_Ele",signal[iSig],scenario),
                                Form("TPC_%s signal (%s) electrons vs. eta;#eta;TPC signal [arb. units]",scenario,signal[iSig]),
                                binsEta,etaMin,etaMax,
                                300,0,300);
    sublist->Add(hSigEtaEle);
  }

  // Electrons: TPC signal vs. multiplicity
  for (Int_t iSig=0; iSig<4; iSig++) {
    TH2F *hSigMultEle = new TH2F(Form("hSigMult_TPC_%s_%s_Ele",signal[iSig],scenario),
                                 Form("TPC_%s signal (%s) electrons vs. mult;multiplicity;TPC signal [arb. units]",scenario,signal[iSig]),
                                 nBinsMult,xBinsMult,
                                 300,0,300);
    sublist->Add(hSigMultEle);
  }

  delete vX;

}

//_____________________________________________________________________________
void AliAnalysisTaskPIDqa::AddTPCHistogramsNsigma(TList *sublist, const char *scenario, Int_t scnumber)
{
  //
  // Create the TPC qa objects: create histograms for TPC Nsigma for different settings
  //

  TVectorD *vX=MakeLogBinning(200,.1,30.);
  Int_t nBinsMult = 38;
  Double_t xBinsMult[39] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 
                           120, 140, 160, 180, 200, 
                           300, 400, 500, 600, 700, 800, 900, 1000, 
                           1200, 1400, 1600, 1800, 2000, 
                           2200, 2400, 2600, 2800, 3000, 
                           3200, 3400, 3600, 3800, 4000
                           };
  const Int_t binsEta=110;
  Float_t etaMin=-1.1;
  Float_t etaMax=1.1;

  Int_t nSpecies=0;
  
  if (scnumber == 4) nSpecies=(Int_t)AliPID::kSPECIES;
  else nSpecies=(Int_t)AliPID::kSPECIESC; 

  // Nsigma vs. p for different particle species
  for (Int_t ispecie=0; ispecie<nSpecies; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TPC_%s_%s",scenario,AliPID::ParticleName(ispecie)),
                              Form("TPC_%s n#sigma %s vs. p;p [GeV]; n#sigma",scenario,AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    sublist->Add(hNsigmaP);
  }

  // Nsigma vs. eta for different particle species (only for some scenarios)
  if ( scnumber == 1 || scnumber == 4 ) {
    for (Int_t ispecie=0; ispecie<nSpecies; ++ispecie){
      TH2F *hNsigmaEta = new TH2F(Form("hNsigmaEta_TPC_%s_%s",scenario,AliPID::ParticleName(ispecie)),
                              Form("TPC_%s n#sigma %s vs. eta;#eta; n#sigma",scenario,AliPID::ParticleName(ispecie)),
                              binsEta,etaMin,etaMax,
                              200,-10,10);
      sublist->Add(hNsigmaEta);
    }
  }

  // Nsigma vs. multiplicity for different particle species (only for some scenarios)
  if ( scnumber == 1 || scnumber == 4 ) {
    for (Int_t ispecie=0; ispecie<nSpecies; ++ispecie){
      TH2F *hNsigmaMult = new TH2F(Form("hNsigmaMult_TPC_%s_%s",scenario,AliPID::ParticleName(ispecie)),
                              Form("TPC_%s n#sigma %s vs. mult;multiplicity; n#sigma",scenario,AliPID::ParticleName(ispecie)),
                              nBinsMult,xBinsMult,
                              200,-10,10);
      sublist->Add(hNsigmaMult);
    }
  }

  // - Beginn: Adding histograms for MIP pions and electrons (only for some scenarios) -
  if ( scnumber == 0 || scnumber == 2 || scnumber == 3 ) {

    // MIP pions: Nsigma vs. eta 
    TH2F *hNsigmaEtaMIPpi = new TH2F(Form("hNsigmaEta_TPC_%s_MIPpi",scenario),
                              Form("TPC_%s n#sigma MIPpi vs. eta;#eta; n#sigma",scenario),
                              binsEta,etaMin,etaMax,
                              200,-10,10);
    sublist->Add(hNsigmaEtaMIPpi);

    // MIP pions: Nsigma vs. multiplicity
    TH2F *hNsigmaMultMIPpi = new TH2F(Form("hNsigmaMult_TPC_%s_MIPpi",scenario),
                               Form("TPC_%s n#sigma MIPpi vs. mult;multiplicity; n#sigma",scenario),
                               nBinsMult,xBinsMult,
                               200,-10,10);
    sublist->Add(hNsigmaMultMIPpi);

    // Electrons: Nsigma vs. eta
    TH2F *hNsigmaEtaEle = new TH2F(Form("hNsigmaEta_TPC_%s_Ele",scenario),
                              Form("TPC_%s n#sigma electrons vs. eta;#eta; n#sigma",scenario),
                              binsEta,etaMin,etaMax,
                              200,-10,10);
    sublist->Add(hNsigmaEtaEle);

    // Electrons: Nsigma vs. multiplicity
    TH2F *hNsigmaMultEle = new TH2F(Form("hNsigmaMult_TPC_%s_Ele",scenario),
                               Form("TPC_%s n#sigma electrons vs. mult;multiplicity; n#sigma",scenario),
                               nBinsMult,xBinsMult,
                               200,-10,10);
    sublist->Add(hNsigmaMultEle);
  } // - End: Adding histograms for MIP pions and electrons

  delete vX;

}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupTPCqa(Bool_t fillMC, Bool_t fill11h, Bool_t fillV0)
{
  //
  // Create the TPC qa objects
  //
  
  // Set up the multiplicity binning
  Int_t nBinsMult = 38;
  Double_t xBinsMult[39] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 
                           120, 140, 160, 180, 200, 
                           300, 400, 500, 600, 700, 800, 900, 1000, 
                           1200, 1400, 1600, 1800, 2000, 
                           2200, 2400, 2600, 2800, 3000, 
                           3200, 3400, 3600, 3800, 4000
                           };


  // Create TPC sublists for different scenarios 
  // corresponding to available information, 
  // e.g. MC or not, special settings for LHC11h

  // basic/default scenario, used always
  fListQAtpcBasic=new TList;
  fListQAtpcBasic->SetOwner();
  fListQAtpcBasic->SetName("TPCBasic");
  fListQAtpc->Add(fListQAtpcBasic);
 
  // MC truth scenario: use only MC truth identified particles
  // only available for MC
  if (fillMC == kTRUE) {
    fListQAtpcMCtruth=new TList;
    fListQAtpcMCtruth->SetOwner();
    fListQAtpcMCtruth->SetName("TPCMCtruth");
    fListQAtpc->Add(fListQAtpcMCtruth);
  }
  
  // Hybrid and OROChigh scenarios, 
  // special settings only available for PbPb LHC11h data
  if (fill11h == kTRUE) {
    fListQAtpcHybrid=new TList;
    fListQAtpcHybrid->SetOwner();
    fListQAtpcHybrid->SetName("TPCHybrid");
    fListQAtpc->Add(fListQAtpcHybrid);
  
    fListQAtpcOROChigh=new TList;
    fListQAtpcOROChigh->SetOwner();
    fListQAtpcOROChigh->SetName("TPCOROChigh");
    fListQAtpc->Add(fListQAtpcOROChigh);
  }

  // scenario only for V0s, 
  // only available for ESDs
  if (fillV0 == kTRUE) {
    fListQAtpcV0=new TList;
    fListQAtpcV0->SetOwner();
    fListQAtpcV0->SetName("TPCV0");
    fListQAtpc->Add(fListQAtpcV0);
  }


  // the default ("basic") scenario
  AddTPCHistogramsNsigma(fListQAtpcBasic,"Basic",0);
  AddTPCHistogramsSignal(fListQAtpcBasic,"Basic");

  // only MC truth identified particles
  if (fillMC) {
    AddTPCHistogramsNsigma(fListQAtpcMCtruth,"MCtruth",1);
  }

  // the "hybrid" scenario (only for period LHC11h)
  if (fill11h) {
    AddTPCHistogramsNsigma(fListQAtpcHybrid,"Hybrid",2);
  }

  // the "OROC high" scenario (only for period LHC11h)
  if (fill11h) {
    AddTPCHistogramsNsigma(fListQAtpcOROChigh,"OROChigh",3);
  }

  // only for V0s
  if (fillV0) {
    AddTPCHistogramsNsigma(fListQAtpcV0,"V0",4);
  }
 

  // Multiplicity distribution --- as check
  TH1F *hMult = new TH1F("hMult_TPC",
                         "Multiplicity distribution;multiplicity;counts",
                         nBinsMult,xBinsMult);
  fListQAtpc->Add(hMult);

}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupTRDqa()
{
  //
  // Create the TRD qa objects
  //
  TVectorD *vX=MakeLogBinning(200,.1,30);
  for(Int_t itl = 0; itl < 6; ++itl){
    for(Int_t ispecie = 0; ispecie < AliPID::kSPECIES; ispecie++){
      TH2F *hLikeP = new TH2F(Form("hLikeP_TRD_%dtls_%s", itl, AliPID::ParticleName(ispecie)),
                              Form("TRD Likelihood to be %s %s for tracks having %d %s; p (GeV/c); TRD %s Likelihood", ispecie == 0 ? "an" : "a", AliPID::ParticleName(ispecie), itl+1, itl == 0 ? "tracklet" : "tracklets", AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1, vX->GetMatrixArray(),
                              100, 0., 1.);
      fListQAtrd->Add(hLikeP);
    }
  }

  // === nSigma Values and signal ===
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TRD_%s",AliPID::ParticleName(ispecie)),
                              Form("TRD n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              100,-10,10);
    fListQAtrdNsig->Add(hNsigmaP);
  }

  TH2F *hSig = new TH2F("hSigP_TRD",
                        "TRD signal vs. p;p [GeV]; TRD signal [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        100,0,100);
  fListQAtrdNsig->Add(hSig);

  fListQAtrd->Add(fListQAtrdNsig);

  // === Same after 3 sigma in TPC and TOF
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TRD_TPCTOF_%s",AliPID::ParticleName(ispecie)),
                              Form("TRD n#sigma %s vs. p after 3#sigma cut in TPC&TOF;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              100,-10,10);
    fListQAtrdNsigTPCTOF->Add(hNsigmaP);
  }
  
  fListQAtrd->Add(fListQAtrdNsigTPCTOF);
  
  delete vX;
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupTOFqa()
{
  //
  // Create the TOF qa objects
  //
  
  TVectorD *vX=MakeLogBinning(200,.1,30);

  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TOF_%s",AliPID::ParticleName(ispecie)),
                              Form("TOF n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    fListQAtof->Add(hNsigmaP);
  }

  TH1F *hnSigT0Fill = new TH1F("hNsigma_TOF_Pion_T0-Fill","TOF n#sigma (Pion) T0-FILL [0.75-1.25. GeV/c]",200,-10,10);
  fListQAtof->Add(hnSigT0Fill);
  TH1F *hnSigT0T0 = new TH1F("hNsigma_TOF_Pion_T0-T0","TOF n#sigma (Pion) T0-T0 [0.75-1.25 GeV/c]",200,-10,10);
  fListQAtof->Add(hnSigT0T0);
  TH1F *hnSigT0TOF = new TH1F("hNsigma_TOF_Pion_T0-TOF","TOF n#sigma (Pion) T0-TOF [0.75-1.25 GeV/c]",200,-10,10);
  fListQAtof->Add(hnSigT0TOF);
  TH1F *hnSigT0Best = new TH1F("hNsigma_TOF_Pion_T0-Best","TOF n#sigma (Pion) T0-Best [0.75-1.25 GeV/c]",200,-10,10);
  fListQAtof->Add(hnSigT0Best);
  TH1F *hnDeltaPi = new TH1F("hDelta_TOF_Pion","DeltaT (Pion) [0.75-1.25 GeV/c]",50,-500,500);
  fListQAtof->Add(hnDeltaPi);
  
  TH2F *hSig = new TH2F("hSigP_TOF",
                        "TOF signal vs. p;p [GeV]; TOF signal [ns]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        300,0,30);

  delete vX;
  
  fListQAtof->Add(hSig);

  TH1F *hStartTimeMaskTOF = new TH1F("hStartTimeMask_TOF","StartTime mask",8,0,8);
  fListQAtof->Add(hStartTimeMaskTOF);
  TH1F *hStartTimeResTOF = new TH1F("hStartTimeRes_TOF","StartTime resolution [ps]",100,0,500);
  fListQAtof->Add(hStartTimeResTOF);

  TH1F *hnTracksAtTOF = new TH1F("hnTracksAt_TOF","Matched tracks at TOF",100,0,100);
  fListQAtof->Add(hnTracksAtTOF);
  TH1F *hT0MakerEff = new TH1F("hT0MakerEff","Events with T0-TOF vs nTracks",100,0,100);
  fListQAtof->Add(hT0MakerEff);

  // this in principle should stay on a T0 PID QA, but are just the data prepared for TOF use
  TH1F *hStartTimeAT0 = new TH1F("hStartTimeA_T0","StartTime from T0A [ps]",1000,-1000,1000);
  fListQAtof->Add(hStartTimeAT0);
  TH1F *hStartTimeCT0 = new TH1F("hStartTimeC_T0","StartTime from T0C [ps]",1000,-1000,1000);
  fListQAtof->Add(hStartTimeCT0);
  TH1F *hStartTimeACT0 = new TH1F("hStartTimeAC_T0","StartTime from T0AC [ps]",1000,-1000,1000);;
  fListQAtof->Add(hStartTimeACT0);
}


//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupT0qa()
{
  //
  // Create the T0 qa objects
  //
  
  // these are similar to plots inside TOFqa, but these are for all events
  TH1F *hStartTimeAT0 = new TH1F("hStartTimeA_T0","StartTime from T0A [ps]",1000,-1000,1000);
  fListQAt0->Add(hStartTimeAT0);
  TH1F *hStartTimeCT0 = new TH1F("hStartTimeC_T0","StartTime from T0C [ps]",1000,-1000,1000);
  fListQAt0->Add(hStartTimeCT0);
  TH1F *hStartTimeACT0 = new TH1F("hStartTimeAC_T0","StartTime from T0AC [ps]",1000,-1000,1000);;
  fListQAt0->Add(hStartTimeACT0);

  TH1F *hnTracksAtT0 = new TH1F("hnTracksAt_T0","Tracks for events selected for T0",100,0,100);
  fListQAt0->Add(hnTracksAtT0);
  TH1F *hT0AEff = new TH1F("hT0AEff","Events with T0A vs nTracks",100,0,100);
  fListQAt0->Add(hT0AEff);
  TH1F *hT0CEff = new TH1F("hT0CEff","Events with T0C vs nTracks",100,0,100);
  fListQAt0->Add(hT0CEff);
  TH1F *hT0AndEff = new TH1F("hT0AndEff","Events with T0AC (AND) vs nTracks",100,0,100);
  fListQAt0->Add(hT0AndEff);
  TH1F *hT0OrEff = new TH1F("hT0OrEff","Events with T0AC (OR) vs nTracks",100,0,100);
  fListQAt0->Add(hT0OrEff);


}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupEMCALqa()
{
  //
  // Create the EMCAL qa objects
  //

  TVectorD *vX=MakeLogBinning(200,.1,30);
  
  TH2F *hNsigmaPt = new TH2F(Form("hNsigmaPt_EMCAL_%s",AliPID::ParticleName(0)),
			     Form("EMCAL n#sigma %s vs. p_{T};p_{T} [GeV]; n#sigma",AliPID::ParticleName(0)),
			     vX->GetNrows()-1,vX->GetMatrixArray(),
			     200,-10,10);
  fListQAemcal->Add(hNsigmaPt);  
  

  TH2F *hSigPtEle = new TH2F("hSigPt_EMCAL_Ele",
                        "EMCAL signal (E/p) vs. p_{T} for electrons;p_{T} [GeV]; EMCAL signal (E/p) [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        200,0,2);
  fListQAemcal->Add(hSigPtEle);

  TH2F *hSigPtPions = new TH2F("hSigPt_EMCAL_Pions",
                        "EMCAL signal (E/p) vs. p_{T} for pions;p_{T} [GeV]; EMCAL signal (E/p) [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        200,0,2);
  fListQAemcal->Add(hSigPtPions);

  TH2F *hSigPtProtons = new TH2F("hSigPt_EMCAL_Protons",
                        "EMCAL signal (E/p) vs. p_{T} for protons;p_{T} [GeV]; EMCAL signal (E/p) [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        200,0,2);
  fListQAemcal->Add(hSigPtProtons);

  TH2F *hSigPtAntiProtons = new TH2F("hSigPt_EMCAL_Antiprotons",
                        "EMCAL signal (E/p) vs. p_{T} for antiprotons;p_{T} [GeV]; EMCAL signal (E/p) [arb. units]",
                        vX->GetNrows()-1,vX->GetMatrixArray(),
                        200,0,2);
  fListQAemcal->Add(hSigPtAntiProtons);

  delete vX;  
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupHMPIDqa()
{
  //
  // Create the HMPID qa objects
  //

  TVectorD *vX=MakeLogBinning(200,.1,30);

  // nSigmas
  Int_t nhists=0;
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIES; ++ispecie){
    if (ispecie==AliPID::kElectron || ispecie==AliPID::kMuon) continue;
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_HMPID_%s",AliPID::ParticleName(ispecie)),
                              Form("HMPID n#sigma %s vs. p;p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    fListQAhmpid->AddAt(hNsigmaP, nhists);
    ++nhists;
  }
  
  // cherenkov angle
  TH2F *hCkovAnglevsMom   = new TH2F("hCkovAnglevsMom",  "Cherenkov angle vs momentum",
                                     vX->GetNrows()-1,vX->GetMatrixArray(),
                                     500,0,1);
  fListQAhmpid->AddAt(hCkovAnglevsMom,nhists);
  
  delete vX;
}

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupTOFHMPIDqa()
{
  //
  // Create the HMPID qa objects
  //
  
  TH2F *hCkovAnglevsMomPion   = new TH2F("hCkovAnglevsMom_pion",  "Cherenkov angle vs momentum for pions",500,0,5.,500,0,1);
  fListQAtofhmpid->Add(hCkovAnglevsMomPion);
  
  TH2F *hCkovAnglevsMomKaon   = new TH2F("hCkovAnglevsMom_kaon",  "Cherenkov angle vs momentum for kaons",500,0,5.,500,0,1);
  fListQAtofhmpid->Add(hCkovAnglevsMomKaon);
  
  TH2F *hCkovAnglevsMomProton = new TH2F("hCkovAnglevsMom_proton","Cherenkov angle vs momentum for protons",500,0,5.,500,0,1);
  fListQAtofhmpid->Add(hCkovAnglevsMomProton);
  
  
}  

//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupTPCTOFqa()
{
  //
  // Create the qa objects for TPC + TOF combination
  //
  
  TVectorD *vX=MakeLogBinning(200,.1,30);

  //TPC signals after TOF cut
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TPC_TOF_%s",AliPID::ParticleName(ispecie)),
                              Form("TPC n#sigma %s vs. p (after TOF 3#sigma cut);p_{TPC} [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    fListQAtpctof->Add(hNsigmaP);
  }

  //TOF signals after TPC cut
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIESC; ++ispecie){
    TH2F *hNsigmaP = new TH2F(Form("hNsigmaP_TOF_TPC_%s",AliPID::ParticleName(ispecie)),
                              Form("TOF n#sigma %s vs. p (after TPC n#sigma cut);p [GeV]; n#sigma",AliPID::ParticleName(ispecie)),
                              vX->GetNrows()-1,vX->GetMatrixArray(),
                              200,-10,10);
    fListQAtpctof->Add(hNsigmaP);
  }

  //EMCAL signal after TOF and TPC cut
  for (Int_t ispecie=0; ispecie<AliPID::kSPECIES; ++ispecie){
    TH2F *heopPt = new TH2F(Form("heopPt_TOF_TPC_%s",AliPID::ParticleName(ispecie)),
			    Form("EMCAL signal (E/p) %s vs. p_{T};p_{T} [GeV]; EMCAL signal (E/p) [arb. units]",AliPID::ParticleName(ispecie)),
			    vX->GetNrows()-1,vX->GetMatrixArray(),
			    200,0,2);
    fListQAtpctof->Add(heopPt);
  }

  delete vX;
}
//______________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupV0qa()
{
  //
  // Create the qa objects for V0 Kine cuts
  //
  
  TH2F *hArmenteros  = new TH2F("hArmenteros",  "Armenteros plot",200,-1.,1.,200,0.,0.4);
  fListQAV0->Add(hArmenteros);
 
}

//_____________________________________________________________________________
void AliAnalysisTaskPIDqa::SetupQAinfo(){
  //
  // Setup the info of QA objects
  //

  TObjArray *arr=new TObjArray;
  arr->SetName("TPC_info");
  fListQAinfo->Add(arr);
}

//______________________________________________________________________________
TVectorD* AliAnalysisTaskPIDqa::MakeLogBinning(Int_t nbinsX, Double_t xmin, Double_t xmax)
{
  //
  // Make logarithmic binning
  // the user has to delete the array afterwards!!!
  //
  
  //check limits
  if (xmin<1e-20 || xmax<1e-20){
    AliError("For Log binning xmin and xmax must be > 1e-20. Using linear binning instead!");
    return MakeLinBinning(nbinsX, xmin, xmax);
  }
  if (xmax<xmin){
    Double_t tmp=xmin;
    xmin=xmax;
    xmax=tmp;
  }
  TVectorD *binLim=new TVectorD(nbinsX+1);
  Double_t first=xmin;
  Double_t last=xmax;
  Double_t expMax=TMath::Log(last/first);
  for (Int_t i=0; i<nbinsX+1; ++i){
    (*binLim)[i]=first*TMath::Exp(expMax/nbinsX*(Double_t)i);
  }
  return binLim;
}

//______________________________________________________________________________
TVectorD* AliAnalysisTaskPIDqa::MakeLinBinning(Int_t nbinsX, Double_t xmin, Double_t xmax)
{
  //
  // Make linear binning
  // the user has to delete the array afterwards!!!
  //
  if (xmax<xmin){
    Double_t tmp=xmin;
    xmin=xmax;
    xmax=tmp;
  }
  TVectorD *binLim=new TVectorD(nbinsX+1);
  Double_t first=xmin;
  Double_t last=xmax;
  Double_t binWidth=(last-first)/nbinsX;
  for (Int_t i=0; i<nbinsX+1; ++i){
    (*binLim)[i]=first+binWidth*(Double_t)i;
  }
  return binLim;
}

//_____________________________________________________________________________
TVectorD* AliAnalysisTaskPIDqa::MakeArbitraryBinning(const char* bins)
{
  //
  // Make arbitrary binning, bins separated by a ','
  //
  TString limits(bins);
  if (limits.IsNull()){
    AliError("Bin Limit string is empty, cannot add the variable");
    return 0x0;
  }
  
  TObjArray *arr=limits.Tokenize(",");
  Int_t nLimits=arr->GetEntries();
  if (nLimits<2){
    AliError("Need at leas 2 bin limits, cannot add the variable");
    delete arr;
    return 0x0;
  }
  
  TVectorD *binLimits=new TVectorD(nLimits);
  for (Int_t iLim=0; iLim<nLimits; ++iLim){
    (*binLimits)[iLim]=(static_cast<TObjString*>(arr->At(iLim)))->GetString().Atof();
  }
  
  delete arr;
  return binLimits;
}


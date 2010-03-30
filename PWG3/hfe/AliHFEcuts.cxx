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
//
// Cut menagement class implemented by the
// ALICE Heavy Flavour Electron Group
// Interface to the correction framework
// Provides a set of standard cuts 
// 
// Authors:
//   Raphaelle Bailhache <R.Bailhache@gsi.de>
//   Markus Fasel <M.Fasel@gsi.de>
//   Markus Heide <mheide@uni-muenster.de>
//   Matus Kalisky <m.kalisky@uni-muenster.de>
//
#include <TClass.h>
#include <TList.h>
#include <TObjArray.h>
#include <TString.h>

#include "AliCFAcceptanceCuts.h"
#include "AliCFCutBase.h"
#include "AliCFEventGenCuts.h"
#include "AliCFEventRecCuts.h"
#include "AliCFManager.h"
#include "AliCFParticleGenCuts.h"
#include "AliCFTrackIsPrimaryCuts.h"
#include "AliCFTrackKineCuts.h"
#include "AliCFTrackQualityCuts.h"
#include "AliESDtrack.h"

#include "AliHFEcuts.h"

ClassImp(AliHFEcuts)

//__________________________________________________________________
AliHFEcuts::AliHFEcuts():
  fRequirements(0),
  fMinClustersTPC(0),
  fMinTrackletsTRD(0),
  fCutITSPixel(0),
  fCheckITSLayerStatus(kTRUE),
  fMaxChi2clusterTPC(0.),
  fMinClusterRatioTPC(0.),
  fSigmaToVtx(0.),
  fHistQA(0x0),
  fCutList(0x0),
  fDebugLevel(0)
{
  //
  // Default Constructor
  //
  memset(fProdVtx, 0, sizeof(Double_t) * 4);
  memset(fDCAtoVtx, 0, sizeof(Double_t) * 2);
  memset(fPtRange, 0, sizeof(Double_t) * 2);
  fCutList = new TObjArray();
  fCutList->SetOwner();
}

//__________________________________________________________________
AliHFEcuts::AliHFEcuts(const AliHFEcuts &c):
  TObject(c),
  fRequirements(c.fRequirements),
  fMinClustersTPC(c.fMinClustersTPC),
  fMinTrackletsTRD(c.fMinTrackletsTRD),
  fCutITSPixel(c.fCutITSPixel),
  fCheckITSLayerStatus(c.fCheckITSLayerStatus),
  fMaxChi2clusterTPC(c.fMaxChi2clusterTPC),
  fMinClusterRatioTPC(c.fMinClusterRatioTPC),
  fSigmaToVtx(c.fSigmaToVtx),
  fHistQA(0x0),
  fCutList(0x0),
  fDebugLevel(0)
{
  //
  // Copy Constructor
  //
  memcpy(fProdVtx, c.fProdVtx, sizeof(Double_t) * 4);
  memcpy(fDCAtoVtx, c.fDCAtoVtx, sizeof(Double_t) * 4);
  memcpy(fPtRange, c.fPtRange, sizeof(Double_t) *2);
  fCutList = dynamic_cast<TObjArray *>(c.fCutList->Clone());
  fCutList->SetOwner();
}

//__________________________________________________________________
AliHFEcuts::~AliHFEcuts(){
  //
  // Destruktor
  //
  if(fCutList){
    fCutList->Delete();
    delete fCutList;
  }
  fCutList = 0x0;
  if(fHistQA) fHistQA->Clear();
  delete fHistQA;
}

//__________________________________________________________________
void AliHFEcuts::Initialize(AliCFManager *cfm){
  //
  // Initializes the cut objects from the correction framework
  // Publishes the cuts to the correction framework manager
  //
  if(IsInDebugMode()){
     fHistQA = new TList;
    fHistQA->SetName("CutQAhistograms");
    fHistQA->SetOwner(kFALSE);
  }
 
  // Call all the setters for the cuts
  SetParticleGenCutList();
  SetAcceptanceCutList();
  SetRecKineITSTPCCutList();
  SetRecPrimaryCutList();
  SetHFElectronITSCuts();
  SetHFElectronTRDCuts();

  // Publish to the cuts which analysis type they are (ESD Analysis by default)
  if(IsAOD()){
    AliInfo("Setting AOD Analysis");
    TObjArray *genCuts = dynamic_cast<TObjArray *>(fCutList->FindObject("fPartGenCuts"));
    if(genCuts){
      AliCFParticleGenCuts *myGenCut = dynamic_cast<AliCFParticleGenCuts *>(genCuts->FindObject("fCutsGenMC"));
      if(myGenCut) myGenCut->SetAODMC(kTRUE);
    }
  }

  // Connect the event cuts
  /*SetEventCutList(kEventStepGenerated);
  SetEventCutList(kEventStepReconstructed);
  cfm->SetEventCutsList(kEventStepGenerated, dynamic_cast<TObjArray *>(fCutList->FindObject("fEvGenCuts")));
  cfm->SetEventCutsList(kEventStepReconstructed, dynamic_cast<TObjArray *>(fCutList->FindObject("fEvRecCuts")));*/
  
  // Connect the particle cuts
  cfm->SetParticleCutsList(kStepMCGenerated, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartGenCuts")));
  cfm->SetParticleCutsList(kStepMCInAcceptance, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartAccCuts")));
  cfm->SetParticleCutsList(kStepRecKineITSTPC, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartRecKineITSTPCCuts")));
  cfm->SetParticleCutsList(kStepRecPrim, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartPrimCuts")));
  cfm->SetParticleCutsList(kStepHFEcutsITS, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartHFECutsITS")));
  cfm->SetParticleCutsList(kStepHFEcutsTRD, dynamic_cast<TObjArray *>(fCutList->FindObject("fPartHFECutsTRD")));

}

//__________________________________________________________________
void AliHFEcuts::Initialize(){
  // Call all the setters for the cuts
  SetParticleGenCutList();
  SetAcceptanceCutList();
  SetRecKineITSTPCCutList();
  SetRecPrimaryCutList();
  SetHFElectronITSCuts();
  SetHFElectronTRDCuts();

}

//__________________________________________________________________
void AliHFEcuts::SetEventCutList(Int_t istep){
  // 
  // Cuts for Event Selection
  //
  TObjArray *arr = new TObjArray;
  if(istep == kEventStepGenerated){
    AliCFEventGenCuts *evGenCuts = new AliCFEventGenCuts("fCutsEvGen", "Event Generated cuts");
    evGenCuts->SetNTracksCut(1);
    evGenCuts->SetRequireVtxCuts(kTRUE);
    evGenCuts->SetVertexXCut(-1, 1);
    evGenCuts->SetVertexYCut(-1, 1);
    evGenCuts->SetVertexZCut(-30, 30);

    arr->SetName("fEvGenCuts");
    arr->AddLast(evGenCuts);
  } else {
    AliCFEventRecCuts *evRecCuts = new AliCFEventRecCuts("fCutsEvRec", "Event Reconstructed cuts");
    evRecCuts->SetUseSPDVertex();
    evRecCuts->SetNTracksCut(1);
    evRecCuts->SetRequireVtxCuts(kTRUE);
    evRecCuts->SetVertexXCut(-1, 1);
    evRecCuts->SetVertexYCut(-1, 1);
    evRecCuts->SetVertexZCut(-30, 30);


    arr->SetName("fEvRecCuts");
    arr->AddLast(evRecCuts);
  }
  fCutList->AddLast(arr);
}

//__________________________________________________________________
void AliHFEcuts::SetParticleGenCutList(){
  //
  // Initialize Particle Cuts for Monte Carlo Tracks
  // Production Vertex: < 1cm (Beampipe)
  // Particle Species: Electrons
  // Eta: < 0.9 (TRD-TOF acceptance)
  //
  AliCFParticleGenCuts *genCuts = new AliCFParticleGenCuts("fCutsGenMC", "Particle Generation Cuts");
  genCuts->SetRequireIsCharged();
  if(IsRequirePrimary()) genCuts->SetRequireIsPrimary();
  if(IsRequireProdVertex()){
    genCuts->SetProdVtxRangeX(fProdVtx[0], fProdVtx[1]);
    genCuts->SetProdVtxRangeY(fProdVtx[2], fProdVtx[3]);
  }
  genCuts->SetRequirePdgCode(11, kTRUE);
  
  AliCFTrackKineCuts *kineMCcuts = new AliCFTrackKineCuts("fCutsKineMC","MC Kine Cuts");
  kineMCcuts->SetPtRange(fPtRange[0], fPtRange[1]);
  kineMCcuts->SetEtaRange(-0.9, 0.9);

  if(IsInDebugMode()){
    genCuts->SetQAOn(fHistQA);
    kineMCcuts->SetQAOn(fHistQA);
  }

  TObjArray *mcCuts = new TObjArray;
  mcCuts->SetName("fPartGenCuts");
  mcCuts->AddLast(genCuts);
  mcCuts->AddLast(kineMCcuts);
  fCutList->AddLast(mcCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetAcceptanceCutList(){
  //
  // Initialize Particle (Monte Carlo) acceptance cuts
  // Min. Required Hist for Detectors:
  //          ITS [3]
  //          TPC [2]
  //          TRD [2*nTracklets]
  //          TOF [0]
  //
  AliCFAcceptanceCuts *accCuts = new AliCFAcceptanceCuts("fCutsAccMC", "MC Acceptance Cuts");
  accCuts->SetMinNHitITS(3);
  accCuts->SetMinNHitTPC(2);
  accCuts->SetMinNHitTRD(2*fMinTrackletsTRD);
  if(IsInDebugMode()) accCuts->SetQAOn(fHistQA);
  
  TObjArray *partAccCuts = new TObjArray();
  partAccCuts->SetName("fPartAccCuts");
  partAccCuts->AddLast(accCuts);
  fCutList->AddLast(partAccCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetRecKineITSTPCCutList(){
  //
  // Track Kinematics and Quality cuts (Based on the Standard cuts from PWG0)
  //
  // ITS refit
  // Variances:
  //  y: 2
  //  z: 2
  //  sin(phi): 0.5
  //  tan(theta): 0.5
  //  1/pt: 2
  // Min. Number of Clusters:
  //  TPC: 50
  // RefitRequired:
  //  TPC
  // Chi2 per TPC cluster: 3.5
  //
  // Kinematics:
  //  Momentum Range: 100MeV - 20GeV
  //  Eta: < 0.9 (TRD-TOF acceptance)
  //
  AliCFTrackQualityCuts *trackQuality = new AliCFTrackQualityCuts("fCutsQualityRec","REC Track Quality Cuts");
  trackQuality->SetMinNClusterTPC(fMinClustersTPC);
  trackQuality->SetMaxChi2PerClusterTPC(fMaxChi2clusterTPC);
  trackQuality->SetStatus(AliESDtrack::kTPCrefit | AliESDtrack::kITSrefit);
  trackQuality->SetMaxCovDiagonalElements(2., 2., 0.5, 0.5, 2); 

  AliHFEextraCuts *hfecuts = new AliHFEextraCuts("fCutsHFElectronGroupTPC","Extra cuts from the HFE group");
  if(fMinClusterRatioTPC > 0.) hfecuts->SetClusterRatioTPC(fMinClusterRatioTPC);
  hfecuts->SetDebugLevel(fDebugLevel);
  
  AliCFTrackKineCuts *kineCuts = new AliCFTrackKineCuts("fCutsKineRec", "REC Kine Cuts");
  kineCuts->SetPtRange(fPtRange[0], fPtRange[1]);
  kineCuts->SetEtaRange(-0.9, 0.9);
  
  if(IsInDebugMode()){
    trackQuality->SetQAOn(fHistQA);
    hfecuts->SetQAOn(fHistQA);
    kineCuts->SetQAOn(fHistQA);
  }
  
  TObjArray *recCuts = new TObjArray;
  recCuts->SetName("fPartRecKineITSTPCCuts");
  recCuts->AddLast(trackQuality);
  recCuts->AddLast(hfecuts);
  recCuts->AddLast(kineCuts);
  fCutList->AddLast(recCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetRecPrimaryCutList(){
  //
  // Primary cuts (based on standard cuts from PWG0):
  //  DCA to Vertex: 
  //    XY: 3. cm
  //    Z:  10. cm
  //  No Kink daughters
  //
  AliCFTrackIsPrimaryCuts *primaryCut = new AliCFTrackIsPrimaryCuts("fCutsPrimaryCuts", "REC Primary Cuts");
  if(IsRequireDCAToVertex()){
    //primaryCut->SetDCAToVertex2D(kTRUE);
    primaryCut->SetMaxDCAToVertexXY(fDCAtoVtx[0]);
    primaryCut->SetMaxDCAToVertexZ(fDCAtoVtx[1]);
  }
  if(IsRequireSigmaToVertex()){
    primaryCut->SetRequireSigmaToVertex(kTRUE);
    primaryCut->SetMaxNSigmaToVertex(fSigmaToVtx);
  }
  primaryCut->SetAcceptKinkDaughters(kFALSE);
  if(IsInDebugMode()) primaryCut->SetQAOn(fHistQA);
  
  TObjArray *primCuts = new TObjArray;
  primCuts->SetName("fPartPrimCuts");
  primCuts->AddLast(primaryCut);
  fCutList->AddLast(primCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetHFElectronITSCuts(){
  //
  // Special Cuts introduced by the HFElectron Group: ITS
  //
  AliHFEextraCuts *hfecuts = new AliHFEextraCuts("fCutsHFElectronGroupPixels","Extra cuts from the HFE group");
  if(IsRequireITSpixel()){
    hfecuts->SetRequireITSpixel(AliHFEextraCuts::ITSPixel_t(fCutITSPixel));
    hfecuts->SetCheckITSstatus(fCheckITSLayerStatus);
  }
  
  if(IsInDebugMode()) hfecuts->SetQAOn(fHistQA);
  hfecuts->SetDebugLevel(fDebugLevel);

  TObjArray *hfeCuts = new TObjArray;
  hfeCuts->SetName("fPartHFECutsITS");
  hfeCuts->AddLast(hfecuts);
  fCutList->AddLast(hfeCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetHFElectronTRDCuts(){
  //
  // Special Cuts introduced by the HFElectron Group: TRD
  //
  AliHFEextraCuts *hfecuts = new AliHFEextraCuts("fCutsHFElectronGroupTRD","Extra cuts from the HFE group");
  if(fMinTrackletsTRD > 0.) hfecuts->SetMinTrackletsTRD(fMinTrackletsTRD);
  if(IsInDebugMode()) hfecuts->SetQAOn(fHistQA);
  hfecuts->SetDebugLevel(fDebugLevel);
  
  TObjArray *hfeCuts = new TObjArray;
  hfeCuts->SetName("fPartHFECutsTRD");
  hfeCuts->AddLast(hfecuts);
  fCutList->AddLast(hfeCuts);
}

//__________________________________________________________________
void AliHFEcuts::SetDebugMode(){ 
  //
  // Switch on QA
  //
  SetBit(kDebugMode, kTRUE); 
}

//__________________________________________________________________
Bool_t AliHFEcuts::CheckParticleCuts(CutStep_t step, TObject *o){
  //
  // Checks the cuts without using the correction framework manager
  // 
  TString stepnames[kNcutSteps] = {"fPartGenCuts", "fPartAccCuts", "fPartRecCuts", "fPartPrimCuts", "fPartHFECuts"};
  TObjArray *cuts = dynamic_cast<TObjArray *>(fCutList->FindObject(stepnames[step].Data()));
  if(!cuts) return kTRUE;
  TIterator *it = cuts->MakeIterator();
  AliCFCutBase *mycut;
  Bool_t status = kTRUE;
  while((mycut = dynamic_cast<AliCFCutBase *>(it->Next()))){
    status &= mycut->IsSelected(o);
  }
  delete it;
  return status;
}

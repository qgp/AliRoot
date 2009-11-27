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
/* $Id: $ */

//_________________________________________________________________________
// Class to check results from simulations or reconstructed real data. 
// Fill few histograms and do some checking plots
//
//-- Author: Gustavo Conesa (INFN-LNF)
//_________________________________________________________________________


// --- ROOT system ---
//#include "Riostream.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
//#include "TH3F.h"
#include "TH2F.h"
#include "TLegend.h"

//---- AliRoot system ----
#include "AliAnaCalorimeterQA.h"
#include "AliCaloTrackReader.h"
#include "AliStack.h"
#include "AliAODCaloCells.h"
#include "AliESDCaloCells.h"
#include "AliAODCaloCluster.h"
#include "AliFiducialCut.h"
#include "AliESDtrack.h"
#include "AliAODTrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliVEventHandler.h"
#include "AliAnalysisManager.h"
#include "AliAODMCParticle.h"
#include "AliMCAnalysisUtils.h"

ClassImp(AliAnaCalorimeterQA)
  
//____________________________________________________________________________
  AliAnaCalorimeterQA::AliAnaCalorimeterQA() : 
    AliAnaPartCorrBaseClass(), fCalorimeter(""), fStyleMacro(""), fMakePlots(kFALSE),
	fhE(0),fhPt(0),fhPhi(0),fhEta(0), fhEtaPhi(0), 
    fhECharged(0),fhPtCharged(0),fhPhiCharged(0),fhEtaCharged(0), fhEtaPhiCharged(0), 
    fhEChargedNoOut(0),fhPtChargedNoOut(0),fhPhiChargedNoOut(0),fhEtaChargedNoOut(0), fhEtaPhiChargedNoOut(0), 
    fhDeltaE(0), fhDeltaPt(0),fhDeltaPhi(0),fhDeltaEta(0), fhRatioE(0), fhRatioPt(0),fhRatioPhi(0),fhRatioEta(0),
    fh2E(0),fh2Pt(0),fh2Phi(0),fh2Eta(0), fhIM(0), fhAsym(0), fhNCellsPerCluster(0), fhNClusters(0), fhNCells(0), fhAmplitude(0), 
    fhGenGamPt(0),fhGenGamEta(0),fhGenGamPhi(0),fhGenPi0Pt(0),fhGenPi0Eta(0),fhGenPi0Phi(0),
    fhGenEtaPt(0),fhGenEtaEta(0),fhGenEtaPhi(0),fhGenOmegaPt(0),fhGenOmegaEta(0),fhGenOmegaPhi(0),
    fhGenElePt(0),fhGenEleEta(0),fhGenElePhi(0), fhEMVxyz(0),  fhEMR(0), fhHaVxyz(0),  fhHaR(0),
    fhGamE(0),fhGamPt(0),fhGamPhi(0),fhGamEta(0), 
    fhGamDeltaE(0), fhGamDeltaPt(0),fhGamDeltaPhi(0),fhGamDeltaEta(0), fhGamRatioE(0), fhGamRatioPt(0),fhGamRatioPhi(0),fhGamRatioEta(0),
    fhEleE(0),fhElePt(0),fhElePhi(0),fhEleEta(0),
    fhPi0E(0),fhPi0Pt(0),fhPi0Phi(0),fhPi0Eta(0), fhNeHadE(0),fhNeHadPt(0),fhNeHadPhi(0),fhNeHadEta(0), 
    fhChHadE(0),fhChHadPt(0),fhChHadPhi(0),fhChHadEta(0),
	fhGamECharged(0),fhGamPtCharged(0),fhGamPhiCharged(0),fhGamEtaCharged(0), 
	fhEleECharged(0),fhElePtCharged(0),fhElePhiCharged(0),fhEleEtaCharged(0),
	fhPi0ECharged(0),fhPi0PtCharged(0),fhPi0PhiCharged(0),fhPi0EtaCharged(0), 
	fhNeHadECharged(0),fhNeHadPtCharged(0),fhNeHadPhiCharged(0),fhNeHadEtaCharged(0), 
	fhChHadECharged(0),fhChHadPtCharged(0),fhChHadPhiCharged(0),fhChHadEtaCharged(0),
    fhGenGamAccE(0),fhGenGamAccPt(0),fhGenGamAccEta(0),fhGenGamAccPhi(0),
    fhGenPi0AccE(0),fhGenPi0AccPt(0),fhGenPi0AccEta(0),fhGenPi0AccPhi(0),
    fh1pOverE(0),fh1dR(0),fh2EledEdx(0), fh2MatchdEdx(0),fhMCEle1pOverE(0),fhMCEle1dR(0),fhMCEle2MatchdEdx(0),
	fhMCChHad1pOverE(0),fhMCChHad1dR(0),fhMCChHad2MatchdEdx(0),fhMCNeutral1pOverE(0),fhMCNeutral1dR(0),fhMCNeutral2MatchdEdx(0),
    fh1pOverER02(0), fhMCEle1pOverER02(0), fhMCChHad1pOverER02(0), fhMCNeutral1pOverER02(0)
{
  //Default Ctor

  //Initialize parameters
  InitParameters();
}

//____________________________________________________________________________
AliAnaCalorimeterQA::AliAnaCalorimeterQA(const AliAnaCalorimeterQA & qa) :   
  AliAnaPartCorrBaseClass(qa), fCalorimeter(qa.fCalorimeter), fStyleMacro(qa.fStyleMacro), fMakePlots(qa.fMakePlots),
  fhE(qa.fhE),fhPt(qa.fhPt), fhPhi(qa.fhPhi), fhEta(qa.fhEta), 
  fhEtaPhi(qa.fhEtaPhi), fhECharged(qa.fhECharged),fhPtCharged(qa.fhPtCharged),fhPhiCharged(qa.fhPhiCharged),
  fhEtaCharged(qa.fhEtaCharged), fhEtaPhiCharged(qa.fhEtaPhiCharged), 
  fhEChargedNoOut(qa.fhEChargedNoOut),fhPtChargedNoOut(qa.fhPtChargedNoOut),fhPhiChargedNoOut(qa.fhPhiChargedNoOut),
  fhEtaChargedNoOut(qa.fhEtaChargedNoOut), fhEtaPhiChargedNoOut(qa.fhEtaPhiChargedNoOut), 
  fhDeltaE(qa.fhDeltaE), fhDeltaPt(qa.fhDeltaPt), fhDeltaPhi(qa.fhDeltaPhi), fhDeltaEta(qa.fhDeltaEta),
  fhRatioE(qa.fhRatioE), fhRatioPt(qa.fhRatioPt), fhRatioPhi(qa.fhRatioPhi), fhRatioEta(qa.fhRatioEta),
  fh2E(qa.fh2E), fh2Pt(qa.fh2Pt), fh2Phi(qa.fh2Phi),fh2Eta(qa.fh2Eta), 
  fhIM(qa.fhIM), fhAsym(qa.fhAsym), fhNCellsPerCluster(qa.fhNCellsPerCluster), fhNClusters(qa.fhNClusters), 
  fhNCells(qa.fhNCells), fhAmplitude(qa.fhAmplitude),
  fhGenGamPt(qa.fhGenGamPt), fhGenGamEta(qa.fhGenGamEta), fhGenGamPhi(qa.fhGenGamPhi),
  fhGenPi0Pt(qa.fhGenPi0Pt), fhGenPi0Eta(qa.fhGenPi0Eta), fhGenPi0Phi(qa.fhGenPi0Phi),
  fhGenEtaPt(qa.fhGenEtaPt), fhGenEtaEta(qa.fhGenEtaEta), fhGenEtaPhi(qa.fhGenEtaPhi),
  fhGenOmegaPt(qa.fhGenOmegaPt), fhGenOmegaEta(qa.fhGenOmegaEta), fhGenOmegaPhi(qa.fhGenOmegaPhi),
  fhGenElePt(qa.fhGenElePt), fhGenEleEta(qa.fhGenEleEta), fhGenElePhi(qa.fhGenElePhi), 
  fhEMVxyz(qa.fhEMVxyz),  fhEMR(qa.fhEMR), fhHaVxyz(qa.fhHaVxyz),  fhHaR(qa.fhHaR),
  fhGamE(qa.fhGamE),fhGamPt(qa.fhGamPt),fhGamPhi(qa.fhGamPhi),fhGamEta(qa.fhGamEta), 
  fhGamDeltaE(qa.fhGamDeltaE), fhGamDeltaPt(qa.fhGamDeltaPt), fhGamDeltaPhi(qa.fhGamDeltaPhi), fhGamDeltaEta(qa.fhGamDeltaEta),
  fhGamRatioE(qa.fhGamRatioE), fhGamRatioPt(qa.fhGamRatioPt), fhGamRatioPhi(qa.fhGamRatioPhi), fhGamRatioEta(qa.fhGamRatioEta),
  fhEleE(qa.fhEleE),fhElePt(qa.fhElePt),fhElePhi(qa.fhElePhi),fhEleEta(qa.fhEleEta),
  fhPi0E(qa.fhPi0E),fhPi0Pt(qa.fhPi0Pt),fhPi0Phi(qa.fhPi0Phi),fhPi0Eta(qa.fhPi0Eta), 
  fhNeHadE(qa.fhNeHadE),fhNeHadPt(qa.fhNeHadPt),fhNeHadPhi(qa.fhNeHadPhi),fhNeHadEta(qa.fhNeHadEta), 
  fhChHadE(qa.fhChHadE),fhChHadPt(qa.fhChHadPt),fhChHadPhi(qa.fhChHadPhi),fhChHadEta(qa.fhChHadEta),
  fhGamECharged(qa.fhGamECharged),fhGamPtCharged(qa.fhGamPtCharged),fhGamPhiCharged(qa.fhGamPhiCharged),fhGamEtaCharged(qa.fhGamEtaCharged), 
  fhEleECharged(qa.fhEleECharged),fhElePtCharged(qa.fhElePtCharged),fhElePhiCharged(qa.fhElePhiCharged),fhEleEtaCharged(qa.fhEleEtaCharged),
  fhPi0ECharged(qa.fhPi0ECharged),fhPi0PtCharged(qa.fhPi0PtCharged),fhPi0PhiCharged(qa.fhPi0PhiCharged),fhPi0EtaCharged(qa.fhPi0EtaCharged), 
  fhNeHadECharged(qa.fhNeHadECharged),fhNeHadPtCharged(qa.fhNeHadPtCharged),fhNeHadPhiCharged(qa.fhNeHadPhiCharged),fhNeHadEtaCharged(qa.fhNeHadEtaCharged), 
  fhChHadECharged(qa.fhChHadECharged),fhChHadPtCharged(qa.fhChHadPtCharged),fhChHadPhiCharged(qa.fhChHadPhiCharged),fhChHadEtaCharged(qa.fhChHadEtaCharged),
  fhGenGamAccE(qa.fhGenGamAccE),fhGenGamAccPt(qa.fhGenGamAccPt),fhGenGamAccEta(qa.fhGenGamAccEta),fhGenGamAccPhi(qa.fhGenGamAccPhi),
  fhGenPi0AccE(qa.fhGenPi0AccE),fhGenPi0AccPt(qa.fhGenPi0AccPt),fhGenPi0AccEta(qa.fhGenPi0AccEta),fhGenPi0AccPhi(qa.fhGenPi0AccPhi),
  fh1pOverE(qa.fh1pOverE),fh1dR(qa.fh1dR),fh2EledEdx(qa.fh2EledEdx), fh2MatchdEdx(qa.fh2MatchdEdx),
  fhMCEle1pOverE(qa.fhMCEle1pOverE),fhMCEle1dR(qa.fhMCEle1dR), fhMCEle2MatchdEdx(qa.fhMCEle2MatchdEdx),
  fhMCChHad1pOverE(qa.fhMCChHad1pOverE),fhMCChHad1dR(qa.fhMCChHad1dR), fhMCChHad2MatchdEdx(qa.fhMCChHad2MatchdEdx),
  fhMCNeutral1pOverE(qa.fhMCNeutral1pOverE),fhMCNeutral1dR(qa.fhMCNeutral1dR), fhMCNeutral2MatchdEdx(qa.fhMCNeutral2MatchdEdx),
  fh1pOverER02(qa.fh1pOverER02), fhMCEle1pOverER02(qa.fhMCEle1pOverER02),fhMCChHad1pOverER02(qa.fhMCChHad1pOverER02), fhMCNeutral1pOverER02(qa.fhMCNeutral1pOverER02)
{
  // cpy ctor
  
}

//_________________________________________________________________________
AliAnaCalorimeterQA & AliAnaCalorimeterQA::operator = (const AliAnaCalorimeterQA & qa)
{
  // assignment operator

  if(this == &qa)return *this;
  ((AliAnaPartCorrBaseClass *)this)->operator=(qa);

  fCalorimeter     = qa.fCalorimeter;
  fStyleMacro      = qa.fStyleMacro;	
  fMakePlots       = qa.fMakePlots;
	
  fhE      = qa.fhE;
  fhPt     = qa.fhPt;
  fhPhi    = qa.fhPhi;
  fhEta    = qa.fhEta;
  fhEtaPhi = qa.fhEtaPhi;

  fhECharged      = qa.fhECharged;
  fhPtCharged     = qa.fhPtCharged;
  fhPhiCharged    = qa.fhPhiCharged;
  fhEtaCharged    = qa.fhEtaCharged;
  fhEtaPhiCharged = qa.fhEtaPhiCharged;

  fhEChargedNoOut      = qa.fhEChargedNoOut;
  fhPtChargedNoOut     = qa.fhPtChargedNoOut;
  fhPhiChargedNoOut    = qa.fhPhiChargedNoOut;
  fhEtaChargedNoOut    = qa.fhEtaChargedNoOut;
  fhEtaPhiChargedNoOut = qa.fhEtaPhiChargedNoOut;
	
  fhIM   = qa.fhIM;
  fhAsym = qa.fhAsym;
	
  fhNCellsPerCluster = qa.fhNCellsPerCluster;
  fhNClusters        = qa.fhNClusters;
	
  fhDeltaE   = qa.fhDeltaE;	
  fhDeltaPt  = qa.fhDeltaPt;
  fhDeltaPhi = qa.fhDeltaPhi;
  fhDeltaEta = qa.fhDeltaEta;
	
  fhRatioE   = qa.fhRatioE;	
  fhRatioPt  = qa.fhRatioPt;
  fhRatioPhi = qa.fhRatioPhi;
  fhRatioEta = qa.fhRatioEta;
	
	
  fh2E   = qa.fh2E;	
  fh2Pt  = qa.fh2Pt;
  fh2Phi = qa.fh2Phi;
  fh2Eta = qa.fh2Eta;
	
  fhNCells    = qa.fhNCells;
  fhAmplitude = qa.fhAmplitude;

  fhGenGamPt = qa.fhGenGamPt  ; fhGenGamEta = qa.fhGenGamEta  ; fhGenGamPhi = qa.fhGenGamPhi  ;
  fhGenPi0Pt = qa.fhGenPi0Pt  ; fhGenPi0Eta = qa.fhGenPi0Eta  ; fhGenPi0Phi = qa.fhGenPi0Phi  ;
  fhGenEtaPt = qa.fhGenEtaPt  ; fhGenEtaEta = qa.fhGenEtaEta  ; fhGenEtaPhi = qa.fhGenEtaPhi  ;
  fhGenOmegaPt = qa.fhGenOmegaPt  ; fhGenOmegaEta = qa.fhGenOmegaEta  ; fhGenOmegaPhi = qa.fhGenOmegaPhi  ;
  fhGenElePt = qa.fhGenElePt  ; fhGenEleEta = qa.fhGenEleEta  ; fhGenElePhi = qa.fhGenElePhi ;	

  fhEMVxyz = qa.fhEMVxyz ;  fhEMR = qa.fhEMR ; fhHaVxyz = qa.fhHaVxyz ;  fhHaR = qa.fhHaR ;
  fhGamE = qa.fhGamE ;fhGamPt = qa.fhGamPt ;fhGamPhi = qa.fhGamPhi ;fhGamEta = qa.fhGamEta ; 
  fhGamDeltaE   = qa.fhDeltaE; fhGamDeltaPt  = qa.fhDeltaPt; fhGamDeltaPhi = qa.fhDeltaPhi; fhGamDeltaEta = qa.fhDeltaEta;
	
  fhGamRatioE   = qa.fhGamRatioE;  fhGamRatioPt  = qa.fhGamRatioPt;  fhGamRatioPhi = qa.fhGamRatioPhi;  fhGamRatioEta = qa.fhGamRatioEta;

  fhEleE = qa.fhEleE ;fhElePt = qa.fhElePt ;fhElePhi = qa.fhElePhi ;fhEleEta = qa.fhEleEta ;
  fhPi0E = qa.fhPi0E ;fhPi0Pt = qa.fhPi0Pt ;fhPi0Phi = qa.fhPi0Phi ;fhPi0Eta = qa.fhPi0Eta ; 
  fhNeHadE = qa.fhNeHadE ;fhNeHadPt = qa.fhNeHadPt ;fhNeHadPhi = qa.fhNeHadPhi ;fhNeHadEta = qa.fhNeHadEta ; 
  fhChHadE = qa.fhChHadE ;fhChHadPt = qa.fhChHadPt ;fhChHadPhi = qa.fhChHadPhi ;fhChHadEta = qa.fhChHadEta ;
  fhGenGamAccE = qa.fhGenGamAccE ;  fhGenPi0AccE = qa.fhGenPi0AccE ;
  fhGenGamAccPt = qa.fhGenGamAccPt ;fhGenGamAccEta = qa.fhGenGamAccEta ;fhGenGamAccPhi = qa.fhGenGamAccPhi ;
  fhGenPi0AccPt = qa.fhGenPi0AccPt ;fhGenPi0AccEta = qa.fhGenPi0AccEta; fhGenPi0AccPhi = qa.fhGenPi0AccPhi ;	
		
  fhGamECharged = qa.fhGamECharged; fhGamPtCharged = qa.fhGamPtCharged; fhGamPhiCharged = qa.fhGamPhiCharged; fhGamEtaCharged = qa.fhGamEtaCharged;  
  fhEleECharged = qa.fhEleECharged; fhElePtCharged = qa.fhElePtCharged; fhElePhiCharged = qa.fhElePhiCharged; fhEleEtaCharged = qa.fhEleEtaCharged; 
  fhPi0ECharged = qa.fhPi0ECharged; fhPi0PtCharged = qa.fhPi0PtCharged; fhPi0PhiCharged = qa.fhPi0PhiCharged; fhPi0EtaCharged = qa.fhPi0EtaCharged;  
  fhNeHadECharged = qa.fhNeHadECharged; fhNeHadPtCharged = qa.fhNeHadPtCharged; fhNeHadPhiCharged = qa.fhNeHadPhiCharged; fhNeHadEtaCharged = qa.fhNeHadEtaCharged;  
  fhChHadECharged = qa.fhChHadECharged; fhChHadPtCharged = qa.fhChHadPtCharged; fhChHadPhiCharged = qa.fhChHadPhiCharged; fhChHadEtaCharged = qa.fhChHadEtaCharged; 	
	
  fh1pOverE    = qa.fh1pOverE;
  fh1dR        = qa.fh1dR;
  fh2MatchdEdx = qa.fh2MatchdEdx;
  fh2EledEdx   = qa.fh2EledEdx;	
	
  fhMCEle1pOverE    = qa.fhMCEle1pOverE; 
  fhMCEle1dR        = qa.fhMCEle1dR; 
  fhMCEle2MatchdEdx = qa.fhMCEle2MatchdEdx ;
	
  fhMCChHad1pOverE = qa.fhMCChHad1pOverE; fhMCChHad1dR = qa.fhMCChHad1dR;  fhMCChHad2MatchdEdx = qa.fhMCChHad2MatchdEdx; 
  fhMCNeutral1pOverE = qa.fhMCNeutral1pOverE; fhMCNeutral1dR = qa.fhMCNeutral1dR;  fhMCNeutral2MatchdEdx = qa.fhMCNeutral2MatchdEdx; 
  fh1pOverER02 = qa.fh1pOverER02;  fhMCEle1pOverER02 = qa.fhMCEle1pOverER02;  fhMCChHad1pOverER02 = qa.fhMCChHad1pOverER02;  
  fhMCNeutral1pOverER02 = qa.fhMCNeutral1pOverER02;
	
  return *this;

}

//________________________________________________________________________
TList *  AliAnaCalorimeterQA::GetCreateOutputObjects()
{  
	// Create histograms to be saved in output file and 
	// store them in fOutputContainer
    
	TList * outputContainer = new TList() ; 
	outputContainer->SetName("QAHistos") ; 
	
	Int_t nptbins  = GetHistoNPtBins();
	Int_t nphibins = GetHistoNPhiBins();
	Int_t netabins = GetHistoNEtaBins();
	Float_t ptmax  = GetHistoPtMax();
	Float_t phimax = GetHistoPhiMax();
	Float_t etamax = GetHistoEtaMax();
	Float_t ptmin  = GetHistoPtMin();
	Float_t phimin = GetHistoPhiMin();
	Float_t etamin = GetHistoEtaMin();	
	
	fhE  = new TH1F ("hE","E reconstructed", nptbins,ptmin,ptmax); 
	fhE->SetXTitle("E (GeV)");
	outputContainer->Add(fhE);
	
	fhPt  = new TH1F ("hPt","p_{T} reconstructed", nptbins,ptmin,ptmax); 
	fhPt->SetXTitle("p_{T} (GeV/c)");
	outputContainer->Add(fhPt);
	
	fhPhi  = new TH1F ("hPhi","#phi reconstructed",nphibins,phimin,phimax); 
	fhPhi->SetXTitle("#phi (rad)");
	outputContainer->Add(fhPhi);
	
	fhEta  = new TH1F ("hEta","#eta reconstructed",netabins,etamin,etamax); 
	fhEta->SetXTitle("#eta ");
	outputContainer->Add(fhEta);
	
	fhEtaPhi  = new TH2F ("hEtaPhi","#eta vs #phi, reconstructed",netabins,etamin,etamax,nphibins,phimin,phimax); 
	fhEtaPhi->SetXTitle("#eta ");
	fhEtaPhi->SetYTitle("#phi ");
	outputContainer->Add(fhEtaPhi);
	
	fhECharged  = new TH1F ("hECharged","E reconstructed, matched with track", nptbins,ptmin,ptmax); 
	fhECharged->SetXTitle("E (GeV)");
	outputContainer->Add(fhECharged);
	
	fhPtCharged  = new TH1F ("hPtCharged","p_{T} reconstructed, matched with track", nptbins,ptmin,ptmax); 
	fhPtCharged->SetXTitle("p_{T} (GeV/c)");
	outputContainer->Add(fhPtCharged);
	
	fhPhiCharged  = new TH1F ("hPhiCharged","#phi reconstructed, matched with track",nphibins,phimin,phimax); 
	fhPhiCharged->SetXTitle("#phi (rad)");
	outputContainer->Add(fhPhiCharged);
	
	fhEtaCharged  = new TH1F ("hEtaCharged","#eta reconstructed, matched with track",netabins,etamin,etamax); 
	fhEtaCharged->SetXTitle("#eta ");
	outputContainer->Add(fhEtaCharged);
	
	fhEtaPhiCharged  = new TH2F ("hEtaPhiCharged","#eta vs #phi, reconstructed , matched with track",netabins,etamin,etamax,nphibins,phimin,phimax); 
	fhEtaPhiCharged->SetXTitle("#eta ");
	fhEtaPhiCharged->SetYTitle("#phi ");
	outputContainer->Add(fhEtaPhiCharged);	
	

	fhEChargedNoOut  = new TH1F ("hEChargedNoOut","E reconstructed, matched with track, no output track params", nptbins,ptmin,ptmax); 
	fhEChargedNoOut->SetXTitle("E (GeV)");
	outputContainer->Add(fhEChargedNoOut);
	
	fhPtChargedNoOut  = new TH1F ("hPtChargedNoOut","p_{T} reconstructed, matched with track, no output track params", nptbins,ptmin,ptmax); 
	fhPtChargedNoOut->SetXTitle("p_{T} (GeV/c)");
	outputContainer->Add(fhPtChargedNoOut);
	
	fhPhiChargedNoOut  = new TH1F ("hPhiChargedNoOut","#phi reconstructed, matched with track, no output track params",nphibins,phimin,phimax); 
	fhPhiChargedNoOut->SetXTitle("#phi (rad)");
	outputContainer->Add(fhPhiChargedNoOut);
	
	fhEtaChargedNoOut  = new TH1F ("hEtaChargedNoOut","#eta reconstructed, matched with track, no output track params",netabins,etamin,etamax); 
	fhEtaChargedNoOut->SetXTitle("#eta ");
	outputContainer->Add(fhEtaChargedNoOut);
	
	fhEtaPhiChargedNoOut  = new TH2F ("hEtaPhiChargedNoOut","#eta vs #phi, reconstructed , matched with track, no output track params",netabins,etamin,etamax,nphibins,phimin,phimax); 
	fhEtaPhiChargedNoOut->SetXTitle("#eta ");
	fhEtaPhiChargedNoOut->SetYTitle("#phi ");
	outputContainer->Add(fhEtaPhiChargedNoOut);	

	fhIM  = new TH2F ("hIM","Cluster pairs Invariant mass vs reconstructed pair energy",nptbins,ptmin,ptmax,200,0,1); 
	fhIM->SetXTitle("E_{cluster pairs} (GeV) ");
	fhIM->SetYTitle("M_{cluster pairs} (GeV/c^{2})");
	outputContainer->Add(fhIM);
	
	fhAsym  = new TH2F ("hAssym","Cluster pairs Asymmetry vs reconstructed pair energy",nptbins,ptmin,ptmax,100,0,1); 
	fhAsym->SetXTitle("E_{cluster pairs} (GeV) ");
	fhAsym->SetYTitle("Asymmetry");
	outputContainer->Add(fhAsym);	
	
	fhNCellsPerCluster  = new TH2F ("hNCellsPerCluster","# cells per cluster vs cluster energy", nptbins,ptmin,ptmax, 100,0,1000); 
	fhNCellsPerCluster->SetXTitle("E (GeV)");
	fhNCellsPerCluster->SetYTitle("n cells");
	outputContainer->Add(fhNCellsPerCluster);
	
	fhNClusters  = new TH1F ("hNClusters","# clusters", 300,0,300); 
	fhNClusters->SetXTitle("number of clusters");
	outputContainer->Add(fhNClusters);
	
	//Calo cells
	fhNCells  = new TH1F ("hNCells","# cells", 13000,0,13000); 
	fhNCells->SetXTitle("n cells");
	outputContainer->Add(fhNCells);
    
	fhAmplitude  = new TH1F ("hAmplitude","Amplitude", 100,0,1000); 
	fhAmplitude->SetXTitle("Amplitude ");
	outputContainer->Add(fhAmplitude);
	
	if(IsDataMC()){
		
		fhDeltaE  = new TH1F ("hDeltaE","MC - Reco E ", 200,-50,50); 
		fhDeltaE->SetXTitle("#Delta E (GeV)");
		outputContainer->Add(fhDeltaE);
		
		fhDeltaPt  = new TH1F ("hDeltaPt","MC - Reco p_{T} ", 200,-50,50); 
		fhDeltaPt->SetXTitle("#Delta p_{T} (GeV/c)");
		outputContainer->Add(fhDeltaPt);
		
		fhDeltaPhi  = new TH1F ("hDeltaPhi","MC - Reco #phi ",100,-2,2); 
		fhDeltaPhi->SetXTitle("#Delta #phi (rad)");
		outputContainer->Add(fhDeltaPhi);
		
		fhDeltaEta  = new TH1F ("hDeltaEta","MC- Reco #eta",100,-1,1); 
		fhDeltaEta->SetXTitle("#Delta #eta ");
		outputContainer->Add(fhDeltaEta);
		
		fhRatioE  = new TH1F ("hRatioE","Reco/MC E ", 200,0,2); 
		fhRatioE->SetXTitle("E_{reco}/E_{gen}");
		outputContainer->Add(fhRatioE);
		
		fhRatioPt  = new TH1F ("hRatioPt","Reco/MC p_{T} ", 200,0,2); 
		fhRatioPt->SetXTitle("p_{T, reco}/p_{T, gen}");
		outputContainer->Add(fhRatioPt);
		
		fhRatioPhi  = new TH1F ("hRatioPhi","Reco/MC #phi ",200,0,2); 
		fhRatioPhi->SetXTitle("#phi_{reco}/#phi_{gen}");
		outputContainer->Add(fhRatioPhi);
		
		fhRatioEta  = new TH1F ("hRatioEta","Reco/MC #eta",200,0,2); 
		fhRatioEta->SetXTitle("#eta_{reco}/#eta_{gen} ");
		outputContainer->Add(fhRatioEta);
		
		fh2E  = new TH2F ("h2E","E distribution, reconstructed vs generated", nptbins,ptmin,ptmax,nptbins,ptmin,ptmax); 
		fh2E->SetXTitle("E_{rec} (GeV)");
		fh2E->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fh2E);	  
		
		fh2Pt  = new TH2F ("h2Pt","p_T distribution, reconstructed vs generated", nptbins,ptmin,ptmax,nptbins,ptmin,ptmax); 
		fh2Pt->SetXTitle("p_{T,rec} (GeV/c)");
		fh2Pt->SetYTitle("p_{T,gen} (GeV/c)");
		outputContainer->Add(fh2Pt);
		
		fh2Phi  = new TH2F ("h2Phi","#phi distribution, reconstructed vs generated", nphibins,phimin,phimax, nphibins,phimin,phimax); 
		fh2Phi->SetXTitle("#phi_{rec} (rad)");
		fh2Phi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fh2Phi);
		
		fh2Eta  = new TH2F ("h2Eta","#eta distribution, reconstructed vs generated", netabins,etamin,etamax,netabins,etamin,etamax); 
		fh2Eta->SetXTitle("#eta_{rec} ");
		fh2Eta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fh2Eta);
		
		//Fill histos depending on origin of cluster
		fhGamE  = new TH2F ("hGamE","E reconstructed vs E generated from #gamma", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhGamE->SetXTitle("E_{rec} (GeV)");
		fhGamE->SetXTitle("E_{gen} (GeV)");
		outputContainer->Add(fhGamE);
		
		fhGamPt  = new TH2F ("hGamPt","p_{T} reconstructed vs E generated from #gamma", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhGamPt->SetXTitle("p_{T rec} (GeV/c)");
		fhGamPt->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhGamPt);
		
		fhGamPhi  = new TH2F ("hGamPhi","#phi reconstructed vs E generated from #gamma",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhGamPhi->SetXTitle("#phi_{rec} (rad)");
		fhGamPhi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhGamPhi);
		
		fhGamEta  = new TH2F ("hGamEta","#eta reconstructed vs E generated from #gamma",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhGamEta->SetXTitle("#eta_{rec} ");
		fhGamEta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhGamEta);
		
		fhGamDeltaE  = new TH1F ("hGamDeltaE","#gamma MC - Reco E ", 200,-50,50); 
		fhGamDeltaE->SetXTitle("#Delta E (GeV)");
		outputContainer->Add(fhGamDeltaE);
		
		fhGamDeltaPt  = new TH1F ("hGamDeltaPt","#gamma MC - Reco p_{T} ", 200,-50,50); 
		fhGamDeltaPt->SetXTitle("#Delta p_{T} (GeV/c)");
		outputContainer->Add(fhGamDeltaPt);
		
		fhGamDeltaPhi  = new TH1F ("hGamDeltaPhi","#gamma MC - Reco #phi ",100,-2,2); 
		fhGamDeltaPhi->SetXTitle("#Delta #phi (rad)");
		outputContainer->Add(fhGamDeltaPhi);
		
		fhGamDeltaEta  = new TH1F ("hGamDeltaEta","#gamma MC- Reco #eta",100,-1,1); 
		fhGamDeltaEta->SetXTitle("#Delta #eta ");
		outputContainer->Add(fhGamDeltaEta);
		
		fhGamRatioE  = new TH1F ("hGamRatioE","#gamma Reco/MC E ", 200,0,2); 
		fhGamRatioE->SetXTitle("E_{reco}/E_{gen}");
		outputContainer->Add(fhGamRatioE);
		
		fhGamRatioPt  = new TH1F ("hGamRatioPt","#gamma Reco/MC p_{T} ", 200,0,2); 
		fhGamRatioPt->SetXTitle("p_{T, reco}/p_{T, gen}");
		outputContainer->Add(fhGamRatioPt);
		
		fhGamRatioPhi  = new TH1F ("hGamRatioPhi","#gamma Reco/MC #phi ",200,0,2); 
		fhGamRatioPhi->SetXTitle("#phi_{reco}/#phi_{gen}");
		outputContainer->Add(fhGamRatioPhi);
		
		fhGamRatioEta  = new TH1F ("hGamRatioEta","#gamma Reco/MC #eta",200,0,2); 
		fhGamRatioEta->SetXTitle("#eta_{reco}/#eta_{gen} ");
		outputContainer->Add(fhGamRatioEta);

		fhPi0E  = new TH2F ("hPi0E","E reconstructed vs E generated from #pi^{0}", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhPi0E->SetXTitle("E_{rec} (GeV)");
		fhPi0E->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhPi0E);
		
		fhPi0Pt  = new TH2F ("hPi0Pt","p_{T} reconstructed vs E generated from #pi^{0}", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhPi0Pt->SetXTitle("p_{T rec} (GeV/c)");
		fhPi0Pt->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhPi0Pt);
		
		fhPi0Phi  = new TH2F ("hPi0Phi","#phi reconstructed vs E generated from #pi^{0}",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhPi0Phi->SetXTitle("#phi_{rec} (rad)");
		fhPi0Phi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhPi0Phi);
		
		fhPi0Eta  = new TH2F ("hPi0Eta","#eta reconstructed vs E generated from #pi^{0}",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhPi0Eta->SetXTitle("#eta_{rec} ");
		fhPi0Eta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhPi0Eta);
		
		fhEleE  = new TH2F ("hEleE","E reconstructed vs E generated from e^{#pm}", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhEleE->SetXTitle("E_{rec} (GeV)");
		fhEleE->SetXTitle("E_{gen} (GeV)");		
		outputContainer->Add(fhEleE);		
		
		fhElePt  = new TH2F ("hElePt","p_{T} reconstructed vs E generated from e^{#pm}", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhElePt->SetXTitle("p_{T rec} (GeV/c)");
		fhElePt->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhElePt);
		
		fhElePhi  = new TH2F ("hElePhi","#phi reconstructed vs E generated from e^{#pm}",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhElePhi->SetXTitle("#phi_{rec} (rad)");
		fhElePhi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhElePhi);
		
		fhEleEta  = new TH2F ("hEleEta","#eta reconstructed vs E generated from e^{#pm}",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhEleEta->SetXTitle("#eta_{rec} ");
		fhEleEta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhEleEta);
		
		fhNeHadE  = new TH2F ("hNeHadE","E reconstructed vs E generated from neutral hadron", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhNeHadE->SetXTitle("E_{rec} (GeV)");
		fhNeHadE->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhNeHadE);
		
		fhNeHadPt  = new TH2F ("hNeHadPt","p_{T} reconstructed vs E generated from neutral hadron", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhNeHadPt->SetXTitle("p_{T rec} (GeV/c)");
		fhNeHadPt->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhNeHadPt);
		
		fhNeHadPhi  = new TH2F ("hNeHadPhi","#phi reconstructed vs E generated from neutral hadron",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhNeHadPhi->SetXTitle("#phi_{rec} (rad)");
		fhNeHadPhi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhNeHadPhi);
		
		fhNeHadEta  = new TH2F ("hNeHadEta","#eta reconstructed vs E generated from neutral hadron",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhNeHadEta->SetXTitle("#eta_{rec} ");
		fhNeHadEta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhNeHadEta);
		
		fhChHadE  = new TH2F ("hChHadE","E reconstructed vs E generated from charged hadron", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhChHadE->SetXTitle("E_{rec} (GeV)");
		fhChHadE->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhChHadE);
		
		fhChHadPt  = new TH2F ("hChHadPt","p_{T} reconstructed vs E generated from charged hadron", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhChHadPt->SetXTitle("p_{T rec} (GeV/c)");
		fhChHadPt->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhChHadPt);
		
		fhChHadPhi  = new TH2F ("hChHadPhi","#phi reconstructed vs E generated from charged hadron",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhChHadPhi->SetXTitle("#phi_{rec} (rad)");
		fhChHadPhi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhChHadPhi);
		
		fhChHadEta  = new TH2F ("hChHadEta","#eta reconstructed vs E generated from charged hadron",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhChHadEta->SetXTitle("#eta_{rec} ");
		fhChHadEta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhChHadEta);
		
		//Charged clusters
		
		fhGamECharged  = new TH2F ("hGamECharged","E reconstructed vs E generated from #gamma, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhGamECharged->SetXTitle("E_{rec} (GeV)");
		fhGamECharged->SetXTitle("E_{gen} (GeV)");
		outputContainer->Add(fhGamECharged);
		
		fhGamPtCharged  = new TH2F ("hGamPtCharged","p_{T} reconstructed vs E generated from #gamma, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhGamPtCharged->SetXTitle("p_{T rec} (GeV/c)");
		fhGamPtCharged->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhGamPtCharged);
		
		fhGamPhiCharged  = new TH2F ("hGamPhiCharged","#phi reconstructed vs E generated from #gamma, track matched cluster",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhGamPhiCharged->SetXTitle("#phi_{rec} (rad)");
		fhGamPhiCharged->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhGamPhiCharged);
		
		fhGamEtaCharged  = new TH2F ("hGamEtaCharged","#eta reconstructed vs E generated from #gamma, track matched cluster",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhGamEtaCharged->SetXTitle("#eta_{rec} ");
		fhGamEtaCharged->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhGamEtaCharged);
		
		fhPi0ECharged  = new TH2F ("hPi0ECharged","E reconstructed vs E generated from #pi^{0}, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhPi0ECharged->SetXTitle("E_{rec} (GeV)");
		fhPi0ECharged->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhPi0ECharged);
		
		fhPi0PtCharged  = new TH2F ("hPi0PtCharged","p_{T} reconstructed vs E generated from #pi^{0}, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhPi0PtCharged->SetXTitle("p_{T rec} (GeV/c)");
		fhPi0PtCharged->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhPi0PtCharged);
		
		fhPi0PhiCharged  = new TH2F ("hPi0PhiCharged","#phi reconstructed vs E generated from #pi^{0}, track matched cluster",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhPi0PhiCharged->SetXTitle("#phi_{rec} (rad)");
		fhPi0PhiCharged->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhPi0PhiCharged);
		
		fhPi0EtaCharged  = new TH2F ("hPi0EtaCharged","#eta reconstructed vs E generated from #pi^{0}, track matched cluster",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhPi0EtaCharged->SetXTitle("#eta_{rec} ");
		fhPi0EtaCharged->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhPi0EtaCharged);
		
		fhEleECharged  = new TH2F ("hEleECharged","E reconstructed vs E generated from e^{#pm}, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhEleECharged->SetXTitle("E_{rec} (GeV)");
		fhEleECharged->SetXTitle("E_{gen} (GeV)");		
		outputContainer->Add(fhEleECharged);		
		
		fhElePtCharged  = new TH2F ("hElePtCharged","p_{T} reconstructed vs E generated from e^{#pm}, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhElePtCharged->SetXTitle("p_{T rec} (GeV/c)");
		fhElePtCharged->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhElePtCharged);
		
		fhElePhiCharged  = new TH2F ("hElePhiCharged","#phi reconstructed vs E generated from e^{#pm}, track matched cluster",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhElePhiCharged->SetXTitle("#phi_{rec} (rad)");
		fhElePhiCharged->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhElePhiCharged);
		
		fhEleEtaCharged  = new TH2F ("hEleEtaCharged","#eta reconstructed vs E generated from e^{#pm}, track matched cluster",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhEleEtaCharged->SetXTitle("#eta_{rec} ");
		fhEleEtaCharged->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhEleEtaCharged);
		
		fhNeHadECharged  = new TH2F ("hNeHadECharged","E reconstructed vs E generated from neutral hadron, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhNeHadECharged->SetXTitle("E_{rec} (GeV)");
		fhNeHadECharged->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhNeHadECharged);
		
		fhNeHadPtCharged  = new TH2F ("hNeHadPtCharged","p_{T} reconstructed vs E generated from neutral hadron, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhNeHadPtCharged->SetXTitle("p_{T rec} (GeV/c)");
		fhNeHadPtCharged->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhNeHadPtCharged);
		
		fhNeHadPhiCharged  = new TH2F ("hNeHadPhiCharged","#phi reconstructed vs E generated from neutral hadron, track matched cluster",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhNeHadPhiCharged->SetXTitle("#phi_{rec} (rad)");
		fhNeHadPhiCharged->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhNeHadPhiCharged);
		
		fhNeHadEtaCharged  = new TH2F ("hNeHadEtaCharged","#eta reconstructed vs E generated from neutral hadron, track matched cluster",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhNeHadEtaCharged->SetXTitle("#eta_{rec} ");
		fhNeHadEtaCharged->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhNeHadEtaCharged);
		
		fhChHadECharged  = new TH2F ("hChHadECharged","E reconstructed vs E generated from charged hadron, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhChHadECharged->SetXTitle("E_{rec} (GeV)");
		fhChHadECharged->SetYTitle("E_{gen} (GeV)");
		outputContainer->Add(fhChHadECharged);
		
		fhChHadPtCharged  = new TH2F ("hChHadPtCharged","p_{T} reconstructed vs E generated from charged hadron, track matched cluster", nptbins,ptmin,ptmax, nptbins,ptmin,ptmax); 
		fhChHadPtCharged->SetXTitle("p_{T rec} (GeV/c)");
		fhChHadPtCharged->SetYTitle("p_{T gen} (GeV/c)");
		outputContainer->Add(fhChHadPtCharged);
		
		fhChHadPhiCharged  = new TH2F ("hChHadPhiCharged","#phi reconstructed vs E generated from charged hadron, track matched cluster",nphibins,phimin,phimax,nphibins,phimin,phimax); 
		fhChHadPhiCharged->SetXTitle("#phi (rad)");
		fhChHadPhiCharged->SetXTitle("#phi_{rec} (rad)");
		fhChHadPhiCharged->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fhChHadPhiCharged);
		
		fhChHadEtaCharged  = new TH2F ("hChHadEtaCharged","#eta reconstructed vs E generated from charged hadron, track matched cluster",netabins,etamin,etamax,netabins,etamin,etamax); 
		fhChHadEtaCharged->SetXTitle("#eta_{rec} ");
		fhChHadEtaCharged->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fhChHadEtaCharged);
		
		//Vertex of generated particles 
		
		fhEMVxyz  = new TH2F ("hEMVxyz","Production vertex of reconstructed ElectroMagnetic particles",100,0,500,100,0,500);//,100,0,500); 
		fhEMVxyz->SetXTitle("v_{x}");
		fhEMVxyz->SetYTitle("v_{y}");
		//fhEMVxyz->SetZTitle("v_{z}");
		outputContainer->Add(fhEMVxyz);
		
		fhHaVxyz  = new TH2F ("hHaVxyz","Production vertex of reconstructed hadrons",100,0,500,100,0,500);//,100,0,500); 
		fhHaVxyz->SetXTitle("v_{x}");
		fhHaVxyz->SetYTitle("v_{y}");
		//fhHaVxyz->SetZTitle("v_{z}");
		outputContainer->Add(fhHaVxyz);
		
		fhEMR  = new TH2F ("hEMR","Distance to production vertex of reconstructed ElectroMagnetic particles vs E rec",nptbins,ptmin,ptmax,100,0,500); 
		fhEMR->SetXTitle("E (GeV)");
		fhEMR->SetYTitle("TMath::Sqrt(v_{x}^{2}+v_{y}^{2})");
		outputContainer->Add(fhEMR);
		
		fhHaR  = new TH2F ("hHaR","Distance to production vertex of reconstructed Hadrons vs E rec",nptbins,ptmin,ptmax,100,0,500); 
		fhHaR->SetXTitle("E (GeV)");
		fhHaR->SetYTitle("TMath::Sqrt(v_{x}^{2}+v_{y}^{2})");
		outputContainer->Add(fhHaR);
		

		
		//Pure MC
		fhGenGamPt  = new TH1F("hGenGamPt" ,"p_{T} of generated #gamma",nptbins,ptmin,ptmax);
		fhGenGamEta = new TH1F("hGenGamEta","Y of generated #gamma",netabins,etamin,etamax);
		fhGenGamPhi = new TH1F("hGenGamPhi","#phi of generated #gamma",nphibins,phimin,phimax);
		
		fhGenPi0Pt  = new TH1F("hGenPi0Pt" ,"p_{T} of generated #pi^{0}",nptbins,ptmin,ptmax);
		fhGenPi0Eta = new TH1F("hGenPi0Eta","Y of generated #pi^{0}",netabins,etamin,etamax);
		fhGenPi0Phi = new TH1F("hGenPi0Phi","#phi of generated #pi^{0}",nphibins,phimin,phimax);
		
		fhGenEtaPt  = new TH1F("hGenEtaPt" ,"p_{T} of generated #eta",nptbins,ptmin,ptmax);
		fhGenEtaEta = new TH1F("hGenEtaEta","Y of generated #eta",netabins,etamin,etamax);
		fhGenEtaPhi = new TH1F("hGenEtaPhi","#phi of generated #eta",nphibins,phimin,phimax);
		
		fhGenOmegaPt  = new TH1F("hGenOmegaPt" ,"p_{T} of generated #omega",nptbins,ptmin,ptmax);
		fhGenOmegaEta = new TH1F("hGenOmegaEta","Y of generated #omega",netabins,etamin,etamax);
		fhGenOmegaPhi = new TH1F("hGenOmegaPhi","#phi of generated #omega",nphibins,phimin,phimax);		
		
		fhGenElePt  = new TH1F("hGenElePt" ,"p_{T} of generated e^{#pm}",nptbins,ptmin,ptmax);
		fhGenEleEta = new TH1F("hGenEleEta","Y of generated  e^{#pm}",netabins,etamin,etamax);
		fhGenElePhi = new TH1F("hGenElePhi","#phi of generated  e^{#pm}",nphibins,phimin,phimax);		
		
		fhGenGamPt->SetXTitle("p_{T} (GeV/c)");
		fhGenGamEta->SetXTitle("#eta");
		fhGenGamPhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenGamPt);
		outputContainer->Add(fhGenGamEta);
		outputContainer->Add(fhGenGamPhi);

		fhGenPi0Pt->SetXTitle("p_{T} (GeV/c)");
		fhGenPi0Eta->SetXTitle("#eta");
		fhGenPi0Phi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenPi0Pt);
		outputContainer->Add(fhGenPi0Eta);
		outputContainer->Add(fhGenPi0Phi);
		
		fhGenEtaPt->SetXTitle("p_{T} (GeV/c)");
		fhGenEtaEta->SetXTitle("#eta");
		fhGenEtaPhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenEtaPt);
		outputContainer->Add(fhGenEtaEta);
		outputContainer->Add(fhGenEtaPhi);
				
		fhGenOmegaPt->SetXTitle("p_{T} (GeV/c)");
		fhGenOmegaEta->SetXTitle("#eta");
		fhGenOmegaPhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenOmegaPt);
		outputContainer->Add(fhGenOmegaEta);
		outputContainer->Add(fhGenOmegaPhi);
		
		fhGenElePt->SetXTitle("p_{T} (GeV/c)");
		fhGenEleEta->SetXTitle("#eta");
		fhGenElePhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenElePt);
		outputContainer->Add(fhGenEleEta);
		outputContainer->Add(fhGenElePhi);
		
		fhGenGamAccE   = new TH1F("hGenGamAccE" ,"E of generated #gamma in calorimeter acceptance",nptbins,ptmin,ptmax);
		fhGenGamAccPt  = new TH1F("hGenGamAccPt" ,"p_{T} of generated #gamma in calorimeter acceptance",nptbins,ptmin,ptmax);
		fhGenGamAccEta = new TH1F("hGenGamAccEta","Y of generated #gamma in calorimeter acceptance",netabins,etamin,etamax);
		fhGenGamAccPhi = new TH1F("hGenGamAccPhi","#phi of generated #gamma  in calorimeter acceptance",nphibins,phimin,phimax);
		
		fhGenPi0AccE  = new TH1F("hGenPi0AccE" ,"E of generated #pi^{0} in calorimeter acceptance",nptbins,ptmin,ptmax);
		fhGenPi0AccPt  = new TH1F("hGenPi0AccPt" ,"p_{T} of generated #pi^{0} in calorimeter acceptance",nptbins,ptmin,ptmax);
		fhGenPi0AccEta = new TH1F("hGenPi0AccEta","Y of generated #pi^{0} in calorimeter acceptance",netabins,etamin,etamax);
		fhGenPi0AccPhi = new TH1F("hGenPi0AccPhi","#phi of generated #pi^{0} in calorimeter acceptance",nphibins,phimin,phimax);
		
		fhGenGamAccE  ->SetXTitle("E (GeV)");
		fhGenGamAccPt ->SetXTitle("p_{T} (GeV/c)");
		fhGenGamAccEta->SetXTitle("#eta");
		fhGenGamAccPhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenGamAccE);		
		outputContainer->Add(fhGenGamAccPt);
		outputContainer->Add(fhGenGamAccEta);
		outputContainer->Add(fhGenGamAccPhi);
		
		fhGenPi0AccE  ->SetXTitle("E (GeV)");		
		fhGenPi0AccPt ->SetXTitle("p_{T} (GeV/c)");
		fhGenPi0AccEta->SetXTitle("#eta");
		fhGenPi0AccPhi->SetXTitle("#phi (rad)");
		outputContainer->Add(fhGenPi0AccE);		
		outputContainer->Add(fhGenPi0AccPt);
		outputContainer->Add(fhGenPi0AccEta);
		outputContainer->Add(fhGenPi0AccPhi);
		
	}
	
	
	fh1pOverE = new TH1F("h1pOverE","TRACK matches p/E",100,0.,10.);
	fh1pOverE->SetXTitle("p/E");
	outputContainer->Add(fh1pOverE);
	
	fh1dR = new TH1F("h1dR","TRACK matches dR",300, 0.,TMath::Pi());
	fh1dR->SetXTitle("#Delta R (rad)");
	outputContainer->Add(fh1dR) ;
	
	fh2MatchdEdx = new TH2F("h2MatchdEdx","dE/dx vs. p for all matches",200,0.,50.,200,0.,400.);
	fh2MatchdEdx->SetXTitle("p (GeV/c)");
	fh2MatchdEdx->SetYTitle("<dE/dx>");
	outputContainer->Add(fh2MatchdEdx);
	
	fh2EledEdx = new TH2F("h2EledEdx","dE/dx vs. p for electrons",200,0.,50.,200,0.,400.);
	fh2EledEdx->SetXTitle("p (GeV/c)");
	fh2EledEdx->SetYTitle("<dE/dx>");
	outputContainer->Add(fh2EledEdx) ;
	
	fhMCEle1pOverE = new TH1F("hMCEle1pOverE","TRACK matches p/E, MC electrons",100,0.,10.);
	fhMCEle1pOverE->SetXTitle("p/E");
	outputContainer->Add(fhMCEle1pOverE);
	
	fhMCEle1dR = new TH1F("hMCEle1dR","TRACK matches dR, MC electrons",300, 0.,TMath::Pi());
	fhMCEle1dR->SetXTitle("#Delta R (rad)");
	outputContainer->Add(fhMCEle1dR) ;
	
	fhMCEle2MatchdEdx = new TH2F("hMCEle2MatchdEdx","dE/dx vs. p for all matches, MC electrons",200,0.,50.,200,0.,400.);
	fhMCEle2MatchdEdx->SetXTitle("p (GeV/c)");
	fhMCEle2MatchdEdx->SetYTitle("<dE/dx>");
	outputContainer->Add(fhMCEle2MatchdEdx);
	
	fhMCChHad1pOverE = new TH1F("hMCChHad1pOverE","TRACK matches p/E, MC charged hadrons",100,0.,10.);
	fhMCChHad1pOverE->SetXTitle("p/E");
	outputContainer->Add(fhMCChHad1pOverE);
	
	fhMCChHad1dR = new TH1F("hMCChHad1dR","TRACK matches dR, MC charged hadrons",300, 0.,TMath::Pi());
	fhMCChHad1dR->SetXTitle("#Delta R (rad)");
	outputContainer->Add(fhMCChHad1dR) ;
	
	fhMCChHad2MatchdEdx = new TH2F("hMCChHad2MatchdEdx","dE/dx vs. p for all matches, MC charged hadrons",200,0.,50.,200,0.,400.);
	fhMCChHad2MatchdEdx->SetXTitle("p (GeV/c)");
	fhMCChHad2MatchdEdx->SetYTitle("<dE/dx>");
	outputContainer->Add(fhMCChHad2MatchdEdx);
	
	fhMCNeutral1pOverE = new TH1F("hMCNeutral1pOverE","TRACK matches p/E, MC neutrals",100,0.,10.);
	fhMCNeutral1pOverE->SetXTitle("p/E");
	outputContainer->Add(fhMCNeutral1pOverE);
	
	fhMCNeutral1dR = new TH1F("hMCNeutral1dR","TRACK matches dR, MC neutrals",300, 0.,TMath::Pi());
	fhMCNeutral1dR->SetXTitle("#Delta R (rad)");
	outputContainer->Add(fhMCNeutral1dR) ;
	
	fhMCNeutral2MatchdEdx = new TH2F("hMCNeutral2MatchdEdx","dE/dx vs. p for all matches, MC neutrals",200,0.,50.,200,0.,400.);
	fhMCNeutral2MatchdEdx->SetXTitle("p (GeV/c)");
	fhMCNeutral2MatchdEdx->SetYTitle("<dE/dx>");
	outputContainer->Add(fhMCNeutral2MatchdEdx);
	
	fh1pOverER02 = new TH1F("h1pOverER02","TRACK matches p/E, all",100,0.,10.);
	fh1pOverER02->SetXTitle("p/E");
	outputContainer->Add(fh1pOverER02);
	
	fhMCEle1pOverER02 = new TH1F("hMCEle1pOverER02","TRACK matches p/E, MC electrons",100,0.,10.);
	fhMCEle1pOverER02->SetXTitle("p/E");
	outputContainer->Add(fhMCEle1pOverER02);
	
	fhMCChHad1pOverER02 = new TH1F("hMCChHad1pOverER02","TRACK matches p/E, MC charged hadrons",100,0.,10.);
	fhMCChHad1pOverER02->SetXTitle("p/E");
	outputContainer->Add(fhMCChHad1pOverER02);
	
	fhMCNeutral1pOverER02 = new TH1F("hMCNeutral1pOverER02","TRACK matches p/E, MC neutrals",100,0.,10.);
	fhMCNeutral1pOverER02->SetXTitle("p/E");
	outputContainer->Add(fhMCNeutral1pOverER02);
	
	return outputContainer;
}

//__________________________________________________
void AliAnaCalorimeterQA::Init()
{ 
	//Check if the data or settings are ok
	if(fCalorimeter != "PHOS" && fCalorimeter !="EMCAL"){
		printf("AliAnaCalorimeterQA::Init() - Wrong calorimeter name <%s>, END\n", fCalorimeter.Data());
		abort();
	}	
	
	if(GetReader()->GetDataType()== AliCaloTrackReader::kMC){
		printf("AliAnaCalorimeterQA::Init() - Analysis of reconstructed data, MC reader not aplicable\n");
		abort();
	}	
	
}


//__________________________________________________
void AliAnaCalorimeterQA::InitParameters()
{ 
  //Initialize the parameters of the analysis.
  AddToHistogramsName("AnaCaloQA_");

  fCalorimeter = "EMCAL"; //or PHOS
  fStyleMacro = "" ;
}

//__________________________________________________________________
void AliAnaCalorimeterQA::Print(const Option_t * opt) const
{
  //Print some relevant parameters set for the analysis
  if(! opt)
    return;
  
  printf("**** Print %s %s ****\n", GetName(), GetTitle() ) ;
  AliAnaPartCorrBaseClass::Print(" ");

  printf("Select Calorimeter %s \n",fCalorimeter.Data());
  printf("Plots style macro %s \n",fStyleMacro.Data()); 
} 

//__________________________________________________________________
void  AliAnaCalorimeterQA::MakeAnalysisFillHistograms() 
{
	//Fill Calorimeter QA histograms
	
	TLorentzVector mom ;
	TLorentzVector mom2 ;
	TRefArray * caloClusters = new TRefArray();
	Int_t nLabel = 0;
	Int_t *labels=0x0;
	Int_t nCaloClusters = 0;
	Int_t nCaloCellsPerCluster = 0;
	Int_t nTracksMatched = 0;
	Int_t trackIndex = 0;
	
	//Play with the MC stack if available	
	//Get the MC arrays and do some checks
	if(IsDataMC()){
		if(GetReader()->ReadStack()){

			if(!GetMCStack()) {
				printf("AliAnaPhoton::MakeAnalysisFillHistograms() - Stack not available, is the MC handler called? STOP\n");
				abort();
			}
			//Fill some pure MC histograms, only primaries.
			for(Int_t i=0 ; i<GetMCStack()->GetNprimary(); i++){//Only primary particles, for all MC transport put GetNtrack()
				TParticle *primary = GetMCStack()->Particle(i) ;
				//printf("i %d, %s: status = %d, primary? %d\n",i, primary->GetName(), primary->GetStatusCode(), primary->IsPrimary());
				if (primary->GetStatusCode() > 11) continue; //Working for PYTHIA and simple generators, check for HERWIG 
				primary->Momentum(mom);
				MCHistograms(mom,TMath::Abs(primary->GetPdgCode()));
			} //primary loop
		}
		else if(GetReader()->ReadAODMCParticles()){

			if(!GetReader()->GetAODMCParticles(0)) 	{
				printf("AliAnaPhoton::MakeAnalysisFillHistograms() -  AODMCParticles not available!\n");
				abort();
			}
			//Fill some pure MC histograms, only primaries.
			for(Int_t i=0 ; i < (GetReader()->GetAODMCParticles(0))->GetEntriesFast(); i++){
				AliAODMCParticle *aodprimary = (AliAODMCParticle*) (GetReader()->GetAODMCParticles(0))->At(i) ;
				//printf("i %d, %s: primary? %d physical primary? %d, flag %d\n",
				//	   i,(TDatabasePDG::Instance()->GetParticle(aodprimary->GetPdgCode()))->GetName(), 
				//	   aodprimary->IsPrimary(), aodprimary->IsPhysicalPrimary(), aodprimary->GetFlag());
				if (!aodprimary->IsPrimary()) continue; //accept all which is not MC transport generated. Don't know how to avoid partons
				//aodprimary->Momentum(mom);
				mom.SetPxPyPzE(aodprimary->Px(),aodprimary->Py(),aodprimary->Pz(),aodprimary->E());
				MCHistograms(mom,TMath::Abs(aodprimary->GetPdgCode()));
			} //primary loop
			
		}
	}// is data and MC	
	
	
	//Get List with CaloClusters  
	
	if(GetReader()->GetDataType()==AliCaloTrackReader::kESD) {
		if     (fCalorimeter == "EMCAL") ((AliESDEvent*)GetReader()->GetInputEvent())->GetEMCALClusters(caloClusters);//GetAODEMCAL();
		else if(fCalorimeter == "PHOS")  ((AliESDEvent*)GetReader()->GetInputEvent())->GetPHOSClusters (caloClusters);//GetAODPHOS();
		else {
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - Wrong calorimeter name <%s>, END\n", fCalorimeter.Data());
			abort();
		}
	}
	else if(GetReader()->GetDataType()==AliCaloTrackReader::kAOD) {
		if     (fCalorimeter == "EMCAL") ((AliAODEvent*)GetReader()->GetInputEvent())->GetEMCALClusters(caloClusters);//GetAODEMCAL();
		else if(fCalorimeter == "PHOS")  ((AliAODEvent*)GetReader()->GetInputEvent())->GetPHOSClusters (caloClusters);//GetAODPHOS();
		else {
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - Wrong calorimeter name <%s>, END\n", fCalorimeter.Data());
			abort();
		}
	}
	
	if(!caloClusters) {
		printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - No CaloClusters available\n");
		abort();
	}
	
	nCaloClusters = caloClusters->GetEntriesFast() ; 
	fhNClusters->Fill(nCaloClusters);

	if(GetDebug() > 0)
		printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - In %s there are %d clusters \n", fCalorimeter.Data(), nCaloClusters);
	
	//Get vertex for photon momentum calculation
	Double_t v[3] ; //vertex ;
	GetReader()->GetVertex(v);
	TObject * track = 0x0;
	//Loop over CaloClusters
	for(Int_t iclus = 0; iclus < nCaloClusters; iclus++){
		
		if(GetDebug() > 0) printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - cluster: %d/%d, data %d \n",
									iclus+1,nCaloClusters,GetReader()->GetDataType());
		
		if(GetReader()->GetDataType()==AliCaloTrackReader::kESD){
			AliESDCaloCluster* clus =  (AliESDCaloCluster*) (caloClusters->At(iclus));
			//Get cluster kinematics
			clus->GetMomentum(mom,v);
			//MC labels
			nLabel = clus->GetNLabels();
			if(clus->GetLabels()) labels =  (clus->GetLabels())->GetArray();
			//Cells per cluster
			nCaloCellsPerCluster =  clus->GetNCells();			
			//matched cluster with tracks
			nTracksMatched = clus->GetNTracksMatched();
			trackIndex     = clus->GetTrackMatched();
			if(trackIndex >= 0){
				track = (AliESDtrack*) ((AliESDEvent*)GetReader()->GetInputEvent())->GetTrack(trackIndex);
			}
			else{
				if(nTracksMatched == 1) nTracksMatched = 0;
				track = 0;
			}
		}
		else{
			AliAODCaloCluster* clus =  (AliAODCaloCluster*) (caloClusters->At(iclus));

			//Get cluster kinematics
			clus->GetMomentum(mom,v);
			//MC labels
			nLabel = clus->GetNLabel();
			if(clus->GetLabels()) labels =  clus->GetLabels();
			//Cells per cluster
			nCaloCellsPerCluster = clus->GetNCells();
			//matched cluster with tracks
			nTracksMatched = clus->GetNTracksMatched();
			if(nTracksMatched > 0)
				track  = (AliAODTrack*)clus->GetTrackMatched(0);
		}

        //Fill histograms related to single cluster or track matching
		ClusterHistograms(mom, nCaloCellsPerCluster, nTracksMatched,  track, labels, nLabel);	
			
		//Invariant mass
		if (nCaloClusters > 1 ) {
			for(Int_t jclus = iclus + 1 ; jclus < nCaloClusters-1 ; jclus++) {
				if(GetReader()->GetDataType()==AliCaloTrackReader::kESD){
					AliESDCaloCluster* clus2 =  (AliESDCaloCluster*) (caloClusters->At(jclus));
					//Get cluster kinematics
					clus2->GetMomentum(mom2,v);
				}
				if(GetReader()->GetDataType()==AliCaloTrackReader::kAOD){
					AliAODCaloCluster* clus2 =  (AliAODCaloCluster*) (caloClusters->At(jclus));
					//Get cluster kinematics
					clus2->GetMomentum(mom2,v);
				}
				if(GetEventNumber() > 139 && GetEventNumber() < 145)
					printf("Event %d: epair %f mass %f\n", GetEventNumber(), (mom+mom2).E(),(mom+mom2).M());
				fhIM  ->Fill((mom+mom2).E(),(mom+mom2).M());
				fhAsym->Fill((mom+mom2).E(),TMath::Abs((mom.E()-mom2.E())/(mom.E()+mom2.E())));
					
			}// 2nd cluster loop
		}////more than 1 cluster in calorimeter  	
	}//cluster loop
	
	//CaloCells
	if(GetReader()->GetDataType()==AliCaloTrackReader::kESD){
		AliESDCaloCells * cell = 0x0; 
		Int_t ncells = 0;
		if(fCalorimeter == "PHOS") cell =  ((AliESDEvent*)GetReader()->GetInputEvent())->GetPHOSCells();
		else					   cell =  ((AliESDEvent*)GetReader()->GetInputEvent())->GetEMCALCells();
		
		if(!cell) {
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - STOP: No CELLS available for analysis");
			abort();
		}
		
		ncells = cell->GetNumberOfCells() ;
		fhNCells->Fill(ncells) ;
		if(GetDebug() > 0) 
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - In ESD %s cell entries %d\n", fCalorimeter.Data(), ncells);    
		
		for (Int_t iCell = 0; iCell < ncells; iCell++) {      
			if(GetDebug() > 2)  printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - Cell : amp %f, absId %d \n", cell->GetAmplitude(iCell), cell->GetCellNumber(iCell));
			fhAmplitude->Fill(cell->GetAmplitude(iCell));
		}
		
	}//ESD
	else{//AOD
		AliESDCaloCells * cell = 0x0; 
		Int_t ncells = 0;
		
		if(fCalorimeter == "PHOS") cell = (AliESDCaloCells *) ((AliAODEvent*)GetReader()->GetInputEvent())->GetPHOSCells();
		else					   cell = (AliESDCaloCells *) ((AliAODEvent*)GetReader()->GetInputEvent())->GetEMCALCells();	
		
		if(!cell) {
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - STOP: No CELLS available for analysis");
			abort();
		}
		
		ncells = cell->GetNumberOfCells() ;
		fhNCells->Fill(ncells) ;
		if(GetDebug() > 0) 
			printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - In ESD %s cell entries %d\n", fCalorimeter.Data(), ncells); 
	
		for (Int_t iCell = 0; iCell < ncells; iCell++) {      
			if(GetDebug() > 2 )  printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - Cell : amp %f, absId %d \n", cell->GetAmplitude(iCell), cell->GetCellNumber(iCell));
			fhAmplitude->Fill(cell->GetAmplitude(iCell));
		}
	
	}//AOD
	
	if(GetDebug() > 0)
		printf("AliAnaCalorimeterQA::MakeAnalysisFillHistograms() - End \n");
}

//__________________________________
void AliAnaCalorimeterQA::ClusterHistograms(const TLorentzVector mom, const Int_t nCaloCellsPerCluster,
											const Int_t nTracksMatched, const TObject * track,  
											const Int_t * labels, const Int_t nLabels){
	//Fill CaloCluster related histograms
	
	AliAODMCParticle * aodprimary  = 0x0;
	TParticle * primary = 0x0;
    Int_t tag = 0;	
	
	Float_t e   = mom.E();
	Float_t pt  = mom.Pt();
	Float_t eta = mom.Eta();
	Float_t phi = mom.Phi();
	if(phi < 0) phi +=TMath::TwoPi();
	if(GetDebug() > 0) {
		printf("AliAnaCalorimeterQA::ClusterHistograms() - cluster: E %2.3f, pT %2.3f, eta %2.3f, phi %2.3f \n",e,pt,eta,phi*TMath::RadToDeg());
		if(IsDataMC()) {
			printf("\t Primaries: nlabels %d, labels pointer %p\n",nLabels,labels);
			if(!nLabels || !labels) printf("\t Strange, no labels!!!\n");
		}
	}

	fhE     ->Fill(e);		
	fhPt    ->Fill(pt);
	fhPhi   ->Fill(phi);
	fhEta   ->Fill(eta);
	fhEtaPhi->Fill(eta,phi);
	//Cells per cluster
	fhNCellsPerCluster->Fill(e, nCaloCellsPerCluster);

	//Fill histograms only possible when simulation
	if(IsDataMC() && nLabels > 0 && labels){

		//Play with the MC stack if available
		Int_t label = labels[0];

		if(label < 0) {
			if(GetDebug() >= 0) printf("AliAnaCalorimeterQA::ClusterHistograms() *** bad label ***:  label %d \n", label);
			return;
		}

		Int_t pdg  =-1; Int_t pdg0  =-1;Int_t status = -1; Int_t iMother = -1; Int_t iParent = -1;
		Float_t vxMC= 0; Float_t vyMC = 0;	
		Float_t eMC = 0; Float_t ptMC= 0; Float_t phiMC =0; Float_t etaMC = 0;
		Int_t charge = 0;	
		
		//Check the origin.
		tag = GetMCAnalysisUtils()->CheckOrigin(labels,nLabels, GetReader(),0);

		if(GetReader()->ReadStack() && !GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCUnknown)){ //it MC stack and known tag

			if( label >= GetMCStack()->GetNtrack()) {
				if(GetDebug() >= 0) printf("AliAnaCalorimeterQA::ClusterHistograms() *** large label ***:  label %d, n tracks %d \n", label, GetMCStack()->GetNtrack());
				return ;
			}
			
			primary = GetMCStack()->Particle(label);
			iMother = label;
			pdg0    = TMath::Abs(primary->GetPdgCode());
			pdg     = pdg0;
			status  = primary->GetStatusCode();
			vxMC    = primary->Vx();
			vyMC    = primary->Vy();
			iParent = primary->GetFirstMother();
				
			if(GetDebug() > 1 ) {
				printf("AliAnaCalorimeterQA::ClusterHistograms() - Cluster most contributing mother: \n");
				printf("\t Mother label %d, pdg %d, %s, status %d, parent %d \n",iMother, pdg0, primary->GetName(),status, iParent);
			}
				
			//Get final particle, no conversion products
			if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCConversion)){
				//Get the parent
				primary = GetMCStack()->Particle(iParent);
				pdg = TMath::Abs(primary->GetPdgCode());
				if(GetDebug() > 1 ) printf("AliAnaCalorimeterQA::ClusterHistograms() - Converted cluster!. Find before conversion: \n");
				while((pdg == 22 || pdg == 11) && status != 1){
					iMother = iParent;
					primary = GetMCStack()->Particle(iMother);
					status  = primary->GetStatusCode();
					iParent = primary->GetFirstMother();
					pdg     = TMath::Abs(primary->GetPdgCode());
					if(GetDebug() > 1 )printf("\t pdg %d, index %d, %s, status %d \n",pdg, iMother,  primary->GetName(),status);	
				}	
					
				if(GetDebug() > 1 ) {
					printf("AliAnaCalorimeterQA::ClusterHistograms() - Converted Cluster mother before conversion: \n");
					printf("\t Mother label %d, pdg %d, %s, status %d, parent %d \n",iMother, pdg, primary->GetName(), status, iParent);
				}
					
			}
				
			//Overlapped pi0 (or eta, there will be very few), get the meson
			if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCPi0) || 
				GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCEta)){
				if(GetDebug() > 1 ) printf("AliAnaCalorimeterQA::ClusterHistograms() - Overlapped Meson decay!, Find it: \n");
				while(pdg != 111 && pdg != 221){
					iMother = iParent;
					primary = GetMCStack()->Particle(iMother);
					status  = primary->GetStatusCode();
					iParent = primary->GetFirstMother();
					pdg     = TMath::Abs(primary->GetPdgCode());
					if(GetDebug() > 1 ) printf("\t pdg %d, %s, index %d\n",pdg,  primary->GetName(),iMother);
					if(iMother==-1) {
						printf("AliAnaCalorimeterQA::ClusterHistograms() - Tagged as Overlapped photon but meson not found, why?\n");
						//break;
					}
				}

				if(GetDebug() > 2 ) printf("AliAnaCalorimeterQA::ClusterHistograms() - Overlapped %s decay, label %d \n", 
										primary->GetName(),iMother);
			}
				
			eMC    = primary->Energy();
			ptMC   = primary->Pt();
			phiMC  = primary->Phi();
			etaMC  = primary->Eta();
			pdg    = TMath::Abs(primary->GetPdgCode());
			charge = (Int_t) TDatabasePDG::Instance()->GetParticle(pdg)->Charge();

		}
		else if(GetReader()->ReadAODMCParticles() && !GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCUnknown)){//it MC AOD and known tag
			//Get the list of MC particles
			if(!GetReader()->GetAODMCParticles(0)) 	{
				printf("AliAnaCalorimeterQA::ClusterHistograms() -  MCParticles not available!\n");
				abort();
			}		
			
			aodprimary = (AliAODMCParticle*) (GetReader()->GetAODMCParticles(0))->At(label);
			iMother = label;
			pdg0    = TMath::Abs(aodprimary->GetPdgCode());
			pdg     = pdg0;
			status  = aodprimary->IsPrimary();
			vxMC    = aodprimary->Xv();
			vyMC    = aodprimary->Yv();
			iParent = aodprimary->GetMother();
				
			if(GetDebug() > 1 ) {
				printf("AliAnaCalorimeterQA::ClusterHistograms() - Cluster most contributing mother: \n");
				printf("\t Mother label %d, pdg %d, Primary? %d, Physical Primary? %d, parent %d \n",
					   iMother, pdg0, aodprimary->IsPrimary(), aodprimary->IsPhysicalPrimary(), iParent);
			}
			
			//Get final particle, no conversion products
			if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCConversion)){
				if(GetDebug() > 1 ) 
					printf("AliAnaCalorimeterQA::ClusterHistograms() - Converted cluster!. Find before conversion: \n");
				//Get the parent
				aodprimary = (AliAODMCParticle*)(GetReader()->GetAODMCParticles(0))->At(iParent);
				pdg = TMath::Abs(aodprimary->GetPdgCode());
				while ((pdg == 22 || pdg == 11) && !aodprimary->IsPhysicalPrimary()) {
					iMother    = iParent;
					aodprimary = (AliAODMCParticle*)(GetReader()->GetAODMCParticles(0))->At(iMother);
					status     = aodprimary->IsPrimary();
					iParent    = aodprimary->GetMother();
					pdg        = TMath::Abs(aodprimary->GetPdgCode());
					if(GetDebug() > 1 )
						printf("\t pdg %d, index %d, Primary? %d, Physical Primary? %d \n",
								pdg, iMother, aodprimary->IsPrimary(), aodprimary->IsPhysicalPrimary());	
				}	
				
				if(GetDebug() > 1 ) {
					printf("AliAnaCalorimeterQA::ClusterHistograms() - Converted Cluster mother before conversion: \n");
					printf("\t Mother label %d, pdg %d, parent %d, Primary? %d, Physical Primary? %d \n",
						   iMother, pdg, iParent, aodprimary->IsPrimary(), aodprimary->IsPhysicalPrimary());
				}
				
			}
				
			//Overlapped pi0 (or eta, there will be very few), get the meson
			if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCPi0) || 
				GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCEta)){
				if(GetDebug() > 1 ) printf("AliAnaCalorimeterQA::ClusterHistograms() - Overlapped Meson decay!, Find it: PDG %d, mom %d \n",pdg, iMother);
				while(pdg != 111 && pdg != 221){
					
					iMother    = iParent;
					aodprimary = (AliAODMCParticle*)(GetReader()->GetAODMCParticles(0))->At(iMother);
					status     = aodprimary->IsPrimary();
					iParent    = aodprimary->GetMother();
					pdg        = TMath::Abs(aodprimary->GetPdgCode());

					if(GetDebug() > 1 ) printf("\t pdg %d, index %d\n",pdg, iMother);
					
					if(iMother==-1) {
						printf("AliAnaCalorimeterQA::ClusterHistograms() - Tagged as Overlapped photon but meson not found, why?\n");
						//break;
					}
				}	
				
				if(GetDebug() > 2 ) printf("AliAnaCalorimeterQA::ClusterHistograms() - Overlapped %s decay, label %d \n", 
										   aodprimary->GetName(),iMother);
			}	
						
			status = aodprimary->IsPrimary();
			eMC    = aodprimary->E();
			ptMC   = aodprimary->Pt();
			phiMC  = aodprimary->Phi();
			etaMC  = aodprimary->Eta();
			pdg    = TMath::Abs(aodprimary->GetPdgCode());
			charge = aodprimary->Charge();
				
		}
	
		//Float_t vz = primary->Vz();
		Float_t r = TMath::Sqrt(vxMC*vxMC + vyMC*vyMC);
		if((pdg == 22 || TMath::Abs(pdg)==11) && status!=1) {
			fhEMVxyz   ->Fill(vxMC,vyMC);//,vz);
			fhEMR      ->Fill(e,r);
		}
		    
		//printf("reco e %f, pt %f, phi %f, eta %f \n", e, pt, phi, eta);
		//printf("prim e %f, pt %f, phi %f, eta %f \n", eMC,ptMC,phiMC ,etaMC );
		//printf("vertex: vx %f, vy %f, vz %f, r %f \n", vxMC, vyMC, vz, r);
			

		fh2E      ->Fill(e, eMC);
		fh2Pt     ->Fill(pt, ptMC);
		fh2Phi    ->Fill(phi, phiMC);
		fh2Eta    ->Fill(eta, etaMC);
		fhDeltaE  ->Fill(eMC-e);
		fhDeltaPt ->Fill(ptMC-pt);
		fhDeltaPhi->Fill(phiMC-phi);
		fhDeltaEta->Fill(etaMC-eta);
		if(eMC   > 0) fhRatioE  ->Fill(e/eMC);
		if(ptMC  > 0) fhRatioPt ->Fill(pt/ptMC);
		if(phiMC > 0) fhRatioPhi->Fill(phi/phiMC);
		if(etaMC > 0) fhRatioEta->Fill(eta/etaMC);			
			
			
		//Overlapped pi0 (or eta, there will be very few)
		if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCPi0) || 
			GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCEta)){
			//cout<<"Fill pi0"<< "E  "<< e <<" prim E "<<eMC<<endl;
				fhPi0E     ->Fill(e,eMC);	
				fhPi0Pt    ->Fill(pt,ptMC);
				fhPi0Eta   ->Fill(eta,etaMC);	
				fhPi0Phi   ->Fill(phi,phiMC);
				if( nTracksMatched > 0){
					fhPi0ECharged     ->Fill(e,eMC);		
					fhPi0PtCharged    ->Fill(pt,ptMC);
					fhPi0PhiCharged   ->Fill(phi,phiMC);
					fhPi0EtaCharged   ->Fill(eta,etaMC);
				}
		}//Overlapped pizero decay
		else if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCPhoton)){
				fhGamE     ->Fill(e,eMC);	
				fhGamPt    ->Fill(pt,ptMC);
				fhGamEta   ->Fill(eta,etaMC);	
				fhGamPhi   ->Fill(phi,phiMC);
				fhGamDeltaE  ->Fill(eMC-e);
				fhGamDeltaPt ->Fill(ptMC-pt);	
				fhGamDeltaPhi->Fill(phiMC-phi);
				fhGamDeltaEta->Fill(etaMC-eta);
				if(eMC > 0) fhGamRatioE  ->Fill(e/eMC);
				if(ptMC     > 0) fhGamRatioPt ->Fill(pt/ptMC);
				if(phiMC    > 0) fhGamRatioPhi->Fill(phi/phiMC);
				if(etaMC    > 0) fhGamRatioEta->Fill(eta/etaMC);
				if( nTracksMatched > 0){
					fhGamECharged     ->Fill(e,eMC);		
					fhGamPtCharged    ->Fill(pt,ptMC);
					fhGamPhiCharged   ->Fill(phi,phiMC);
					fhGamEtaCharged   ->Fill(eta,etaMC);
				}
		}//photon
		else if(GetMCAnalysisUtils()->CheckTagBit(tag, AliMCAnalysisUtils::kMCElectron)){
			fhEleE     ->Fill(e,eMC);	
			fhElePt    ->Fill(pt,ptMC);
			fhEleEta   ->Fill(eta,etaMC);	
			fhElePhi   ->Fill(phi,phiMC);
			fhEMVxyz   ->Fill(vxMC,vyMC);//,vz);
			fhEMR      ->Fill(e,r);
			if( nTracksMatched > 0){
				fhEleECharged     ->Fill(e,eMC);		
				fhElePtCharged    ->Fill(pt,ptMC);
				fhElePhiCharged   ->Fill(phi,phiMC);
				fhEleEtaCharged   ->Fill(eta,etaMC);
			}
		}
		else if(charge == 0){
			fhNeHadE     ->Fill(e,eMC);	
			fhNeHadPt    ->Fill(pt,ptMC);
			fhNeHadEta   ->Fill(eta,etaMC);	
			fhNeHadPhi   ->Fill(phi,phiMC);	
			fhHaVxyz     ->Fill(vxMC,vyMC);//,vz);
			fhHaR        ->Fill(e,r);
			if( nTracksMatched > 0){
				fhNeHadECharged     ->Fill(e,eMC);		
				fhNeHadPtCharged    ->Fill(pt,ptMC);
				fhNeHadPhiCharged   ->Fill(phi,phiMC);
				fhNeHadEtaCharged   ->Fill(eta,etaMC);
			}
		}
		else if(charge!=0){
			fhChHadE     ->Fill(e,eMC);	
			fhChHadPt    ->Fill(pt,ptMC);
			fhChHadEta   ->Fill(eta,etaMC);	
			fhChHadPhi   ->Fill(phi,phiMC);	
			fhHaVxyz     ->Fill(vxMC,vyMC);//,vz);
			fhHaR        ->Fill(e,r);
			if( nTracksMatched > 0){
				fhChHadECharged     ->Fill(e,eMC);		
				fhChHadPtCharged    ->Fill(pt,ptMC);
				fhChHadPhiCharged   ->Fill(phi,phiMC);
				fhChHadEtaCharged   ->Fill(eta,etaMC);
			}
		}
	}//Work with MC
		
	
	//Match tracks and clusters
	//To be Modified in case of AODs
	
	//if(ntracksmatched==1 && trackIndex==-1) ntracksmatched=0;
	
	if( nTracksMatched > 0){
		fhECharged     ->Fill(e);		
		fhPtCharged    ->Fill(pt);
		fhPhiCharged   ->Fill(phi);
		fhEtaCharged   ->Fill(eta);
		fhEtaPhiCharged->Fill(eta,phi);		
		
		//printf("track index %d ntracks %d\n", esd->GetNumberOfTracks());
		if(GetReader()->GetDataType()==AliCaloTrackReader::kESD) {
			if (track && ((AliESDtrack*)track)->GetOuterParam() ) {
				
				Double_t tphi  = ((AliESDtrack*)track)->GetOuterParam()->Phi();
				Double_t teta  = ((AliESDtrack*)track)->GetOuterParam()->Eta();
				Double_t tmom  = ((AliESDtrack*)track)->GetOuterParam()->P();
				
				Double_t tmom2     = ((AliESDtrack*)track)->P();
				Double_t tpcSignal = ((AliESDtrack*)track)->GetTPCsignal();
				
				Double_t deta = teta - eta;
				Double_t dphi = tphi - phi;
				if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
				if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
				Double_t dR = sqrt(dphi*dphi + deta*deta);
				
				Double_t pOverE = tmom/e;
				
				fh1pOverE->Fill(pOverE);
				if(dR < 0.02) fh1pOverER02->Fill(pOverE);
				
				fh1dR->Fill(dR);
				fh2MatchdEdx->Fill(tmom2,tpcSignal);
				
				if(IsDataMC() && primary){ 
					Int_t pdg = primary->GetPdgCode();
					Double_t  charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
					
					if(TMath::Abs(pdg) == 11){
						fhMCEle1pOverE->Fill(pOverE);
						fhMCEle1dR->Fill(dR);
						fhMCEle2MatchdEdx->Fill(tmom2,tpcSignal);		
						if(dR < 0.02) fhMCEle1pOverER02->Fill(pOverE);
					}
					else if(charge!=0){
						fhMCChHad1pOverE->Fill(pOverE);
						fhMCChHad1dR->Fill(dR);
						fhMCChHad2MatchdEdx->Fill(tmom2,tpcSignal);	
						if(dR < 0.02) fhMCChHad1pOverER02->Fill(pOverE);
					}
					else if(charge == 0){
						fhMCNeutral1pOverE->Fill(pOverE);
						fhMCNeutral1dR->Fill(dR);
						fhMCNeutral2MatchdEdx->Fill(tmom2,tpcSignal);	
						if(dR < 0.02) fhMCNeutral1pOverER02->Fill(pOverE);
					}
				}//DataMC
				int nITS = ((AliESDtrack*)track)->GetNcls(0);
				int nTPC = ((AliESDtrack*)track)->GetNcls(1);
				if(dR < 0.02 && pOverE > 0.5 && pOverE < 1.5
				   && nCaloCellsPerCluster > 1 && nITS > 3 && nTPC > 20) {
					fh2EledEdx->Fill(tmom2,tpcSignal);
				}
			}//Outer param available 
			else if(!((AliESDtrack*)track)->GetOuterParam()){
				ULong_t status=AliESDtrack::kTPCrefit;
				status|=AliESDtrack::kITSrefit;
				//printf("track status %d\n", track->GetStatus() );
				fhEChargedNoOut     ->Fill(e);		
				fhPtChargedNoOut     ->Fill(pt);
				fhPhiChargedNoOut    ->Fill(phi);
				fhEtaChargedNoOut    ->Fill(eta);
				fhEtaPhiChargedNoOut ->Fill(eta,phi);	
				if(GetDebug() >= 0 && ((((AliESDtrack*)track)->GetStatus() & status) == status)) printf("ITS+TPC\n");
			}
			else {
				if(GetDebug() >= 0) printf("AliAnaCalorimeterQA::ClusterHistograms() ERROR: Could not receive track %d\n", ((AliESDtrack*)track)->GetID());
			}
		}//do only if input are ESDs
	}
	
}// Clusters
	
//______________________________________________________________________________
void AliAnaCalorimeterQA::MCHistograms(const TLorentzVector mom, const Int_t pdg){
	//Fill pure monte carlo related histograms
	
	Float_t eMC    = mom.E();
	Float_t ptMC   = mom.Pt();
	Float_t phiMC  = mom.Phi();
	if(phiMC < 0) 
		phiMC  += TMath::TwoPi();
	Float_t etaMC  = mom.Eta();
	
	if (TMath::Abs(etaMC) > 1) return;

	Bool_t in = kTRUE;
	if(IsFiducialCutOn()) in =  GetFiducialCut()->IsInFiducialCut(mom,fCalorimeter) ;
	
	if (pdg==22) {
		fhGenGamPt ->Fill(ptMC);
		fhGenGamEta->Fill(etaMC);
		fhGenGamPhi->Fill(phiMC);
		if(in){
			fhGenGamAccE  ->Fill(eMC);
			fhGenGamAccPt ->Fill(ptMC);
			fhGenGamAccEta->Fill(etaMC);
			fhGenGamAccPhi->Fill(phiMC);					
		}
	}
	else if (pdg==111) {
		fhGenPi0Pt ->Fill(ptMC);
		fhGenPi0Eta->Fill(etaMC);
		fhGenPi0Phi->Fill(phiMC);
		if(in){
			fhGenPi0AccE  ->Fill(eMC);					
			fhGenPi0AccPt ->Fill(ptMC);
			fhGenPi0AccEta->Fill(etaMC);
			fhGenPi0AccPhi->Fill(phiMC);					
		}
	}
	else if (pdg==221) {
		fhGenEtaPt ->Fill(ptMC);
		fhGenEtaEta->Fill(etaMC);
		fhGenEtaPhi->Fill(phiMC);
	}
	else if (pdg==223) {
		fhGenOmegaPt ->Fill(ptMC);
		fhGenOmegaEta->Fill(etaMC);
		fhGenOmegaPhi->Fill(phiMC);
	}
	else if (TMath::Abs(pdg)==11) {
		fhGenElePt ->Fill(ptMC);
		fhGenEleEta->Fill(etaMC);
		fhGenElePhi->Fill(phiMC);
	}	
	
}
	
//________________________________________________________________________
void AliAnaCalorimeterQA::ReadHistograms(TList* outputList)
{
	// Needed when Terminate is executed in distributed environment
	// Refill analysis histograms of this class with corresponding histograms in output list. 
	
	// Histograms of this analsys are kept in the same list as other analysis, recover the position of
	// the first one and then add the next 
	Int_t index = outputList->IndexOf(outputList->FindObject(GetAddedHistogramsStringToName()+"hE"));
	//printf("Calo: %s, index: %d\n",fCalorimeter.Data(),index);
	//Read histograms, must be in the same order as in GetCreateOutputObject.
	fhE      = (TH1F *) outputList->At(index++); 	
	fhPt     = (TH1F *) outputList->At(index++); 
	fhPhi    = (TH1F *) outputList->At(index++); 
	fhEta    = (TH1F *) outputList->At(index++);
	fhEtaPhi = (TH2F *) outputList->At(index++);
	
	fhECharged      = (TH1F *) outputList->At(index++); 	
	fhPtCharged     = (TH1F *) outputList->At(index++); 
	fhPhiCharged    = (TH1F *) outputList->At(index++); 
	fhEtaCharged    = (TH1F *) outputList->At(index++);
	fhEtaPhiCharged = (TH2F *) outputList->At(index++);
	
	fhEChargedNoOut      = (TH1F *) outputList->At(index++); 	
	fhPtChargedNoOut     = (TH1F *) outputList->At(index++); 
	fhPhiChargedNoOut    = (TH1F *) outputList->At(index++); 
	fhEtaChargedNoOut    = (TH1F *) outputList->At(index++);
	fhEtaPhiChargedNoOut = (TH2F *) outputList->At(index++);

	fhIM     = (TH2F *) outputList->At(index++);
	fhAsym   = (TH2F *) outputList->At(index++);
	
	fhNCellsPerCluster = (TH2F *) outputList->At(index++);
	fhNClusters  = (TH1F *) outputList->At(index++); 
	fhNCells     = (TH1F *) outputList->At(index++); 
	fhAmplitude  = (TH1F *) outputList->At(index++); 
	
	if(IsDataMC()){
		fhDeltaE   = (TH1F *) outputList->At(index++); 
		fhDeltaPt  = (TH1F *) outputList->At(index++); 
		fhDeltaPhi = (TH1F *) outputList->At(index++); 
		fhDeltaEta = (TH1F *) outputList->At(index++); 
		
		fhRatioE   = (TH1F *) outputList->At(index++); 
		fhRatioPt  = (TH1F *) outputList->At(index++); 
		fhRatioPhi = (TH1F *) outputList->At(index++); 
		fhRatioEta = (TH1F *) outputList->At(index++); 
		
		fh2E       = (TH2F *) outputList->At(index++); 
		fh2Pt      = (TH2F *) outputList->At(index++); 
		fh2Phi     = (TH2F *) outputList->At(index++); 
		fh2Eta     = (TH2F *) outputList->At(index++); 
		
		fhGamE     = (TH2F *) outputList->At(index++); 
		fhGamPt    = (TH2F *) outputList->At(index++); 
		fhGamPhi   = (TH2F *) outputList->At(index++); 
		fhGamEta   = (TH2F *) outputList->At(index++); 
		
		fhGamDeltaE   = (TH1F *) outputList->At(index++); 
		fhGamDeltaPt  = (TH1F *) outputList->At(index++); 
		fhGamDeltaPhi = (TH1F *) outputList->At(index++); 
		fhGamDeltaEta = (TH1F *) outputList->At(index++); 
		
		fhGamRatioE   = (TH1F *) outputList->At(index++); 
		fhGamRatioPt  = (TH1F *) outputList->At(index++); 
		fhGamRatioPhi = (TH1F *) outputList->At(index++); 
		fhGamRatioEta = (TH1F *) outputList->At(index++); 

		fhPi0E     = (TH2F *) outputList->At(index++); 
		fhPi0Pt    = (TH2F *) outputList->At(index++); 
		fhPi0Phi   = (TH2F *) outputList->At(index++); 
		fhPi0Eta   = (TH2F *) outputList->At(index++); 		
		
		fhEleE     = (TH2F *) outputList->At(index++); 
		fhElePt    = (TH2F *) outputList->At(index++); 
		fhElePhi   = (TH2F *) outputList->At(index++); 
		fhEleEta   = (TH2F *) outputList->At(index++); 		
		
		fhNeHadE     = (TH2F *) outputList->At(index++); 
		fhNeHadPt    = (TH2F *) outputList->At(index++); 
		fhNeHadPhi   = (TH2F *) outputList->At(index++); 
		fhNeHadEta   = (TH2F *) outputList->At(index++); 		
		
		fhChHadE     = (TH2F *) outputList->At(index++); 
		fhChHadPt    = (TH2F *) outputList->At(index++); 
		fhChHadPhi   = (TH2F *) outputList->At(index++); 
		fhChHadEta   = (TH2F *) outputList->At(index++); 				
		
		fhGamECharged     = (TH2F *) outputList->At(index++); 
		fhGamPtCharged    = (TH2F *) outputList->At(index++); 
		fhGamPhiCharged   = (TH2F *) outputList->At(index++); 
		fhGamEtaCharged   = (TH2F *) outputList->At(index++); 
				
		fhPi0ECharged     = (TH2F *) outputList->At(index++); 
		fhPi0PtCharged    = (TH2F *) outputList->At(index++); 
		fhPi0PhiCharged   = (TH2F *) outputList->At(index++); 
		fhPi0EtaCharged   = (TH2F *) outputList->At(index++); 		
		
		fhEleECharged     = (TH2F *) outputList->At(index++); 
		fhElePtCharged    = (TH2F *) outputList->At(index++); 
		fhElePhiCharged   = (TH2F *) outputList->At(index++); 
		fhEleEtaCharged   = (TH2F *) outputList->At(index++); 		
		
		fhNeHadECharged     = (TH2F *) outputList->At(index++); 
		fhNeHadPtCharged    = (TH2F *) outputList->At(index++); 
		fhNeHadPhiCharged   = (TH2F *) outputList->At(index++); 
		fhNeHadEtaCharged   = (TH2F *) outputList->At(index++); 		
		
		fhChHadECharged     = (TH2F *) outputList->At(index++); 
		fhChHadPtCharged    = (TH2F *) outputList->At(index++); 
		fhChHadPhiCharged   = (TH2F *) outputList->At(index++); 
		fhChHadEtaCharged   = (TH2F *) outputList->At(index++); 				
		
//		fhEMVxyz     = (TH3F *) outputList->At(index++); 
//		fhHaVxyz     = (TH3F *) outputList->At(index++); 
		
		fhEMVxyz     = (TH2F *) outputList->At(index++); 
		fhHaVxyz     = (TH2F *) outputList->At(index++); 
		fhEMR        = (TH2F *) outputList->At(index++); 
		fhHaR        = (TH2F *) outputList->At(index++); 
		
		fhGenGamPt    = (TH1F *) outputList->At(index++); 
		fhGenGamEta   = (TH1F *) outputList->At(index++); 
		fhGenGamPhi   = (TH1F *) outputList->At(index++); 
		
		fhGenPi0Pt    = (TH1F *) outputList->At(index++); 
		fhGenPi0Eta   = (TH1F *) outputList->At(index++); 
		fhGenPi0Phi   = (TH1F *) outputList->At(index++); 
		
		fhGenEtaPt    = (TH1F *) outputList->At(index++); 
		fhGenEtaEta   = (TH1F *) outputList->At(index++); 
		fhGenEtaPhi   = (TH1F *) outputList->At(index++); 
		
		fhGenOmegaPt  = (TH1F *) outputList->At(index++); 
		fhGenOmegaEta = (TH1F *) outputList->At(index++); 
		fhGenOmegaPhi = (TH1F *) outputList->At(index++); 
		
		fhGenElePt    = (TH1F *) outputList->At(index++); 
		fhGenEleEta   = (TH1F *) outputList->At(index++); 
		fhGenElePhi   = (TH1F *) outputList->At(index++); 
		
		fhGenGamAccE   = (TH1F *) outputList->At(index++); 		
		fhGenGamAccPt  = (TH1F *) outputList->At(index++); 
		fhGenGamAccEta = (TH1F *) outputList->At(index++); 
		fhGenGamAccPhi = (TH1F *) outputList->At(index++); 
		
		fhGenPi0AccE   = (TH1F *) outputList->At(index++); 		
		fhGenPi0AccPt  = (TH1F *) outputList->At(index++); 
		fhGenPi0AccEta = (TH1F *) outputList->At(index++); 
		fhGenPi0AccPhi = (TH1F *) outputList->At(index++); 
		
	}//Is data MC	
	
	fh1pOverE =    (TH1F *) outputList->At(index++);
	fh1dR =        (TH1F *) outputList->At(index++);
	fh2MatchdEdx = (TH2F *) outputList->At(index++);
	fh2EledEdx =   (TH2F *) outputList->At(index++);
	
	if(IsDataMC()){
		fhMCEle1pOverE =    (TH1F *) outputList->At(index++);
		fhMCEle1dR =        (TH1F *) outputList->At(index++);
		fhMCEle2MatchdEdx = (TH2F *) outputList->At(index++);
		
		fhMCChHad1pOverE =    (TH1F *) outputList->At(index++);
		fhMCChHad1dR =        (TH1F *) outputList->At(index++);
		fhMCChHad2MatchdEdx = (TH2F *) outputList->At(index++);
		
		fhMCNeutral1pOverE    = (TH1F *) outputList->At(index++);
		fhMCNeutral1dR        = (TH1F *) outputList->At(index++);
		fhMCNeutral2MatchdEdx = (TH2F *) outputList->At(index++);
		
		fh1pOverER02          =    (TH1F *) outputList->At(index++);
		fhMCEle1pOverER02     =    (TH1F *) outputList->At(index++);
		fhMCChHad1pOverER02   =    (TH1F *) outputList->At(index++);
		fhMCNeutral1pOverER02 =    (TH1F *) outputList->At(index++);
	}
	//for(Int_t i = 0;  i<index ; i++) cout<<outputList->At(i)->GetName()<<endl;
}

//__________________________________________________________________
void  AliAnaCalorimeterQA::Terminate(TList* outputList) 
{
	//Do plots if requested	
	char line[1024] ; 

	//if(fRemoveOutputAOD){
	//	sprintf(line, ".!rm -fR %s",((AliVEventHandler*)((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler()))->GetOutputFileName()); 
	//	gROOT->ProcessLine(line);
	//}
	
	if(!fMakePlots) return;
	
	//Do some plots to end
	 if(fStyleMacro!="")gROOT->Macro(fStyleMacro); 
	//Recover histograms from output histograms list, needed for distributed analysis.	
	ReadHistograms(outputList);
	
	//printf(" AliAnaCalorimeterQA::Terminate()  *** %s Report:", GetName()) ; 
	//printf(" AliAnaCalorimeterQA::Terminate()        pt         : %5.3f , RMS : %5.3f \n", fhPt->GetMean(),   fhPt->GetRMS() ) ;

	char name[128];
	char cname[128];
		
	//CaloCells
	//printf("c9\n");
	sprintf(cname,"QA_%s_nclustercellsamp",fCalorimeter.Data());
	TCanvas  * c9 = new TCanvas(cname, " CaloClusters and CaloCells", 400, 400) ;
	c9->Divide(2, 2);
	
	c9->cd(1) ; 
	if(fhNClusters->GetEntries() > 0) gPad->SetLogy();
	gPad->SetLogx();
	fhNClusters->SetLineColor(4);
	fhNClusters->Draw();
	
	c9->cd(2) ; 
	if(fhNCells->GetEntries() > 0) gPad->SetLogy();
	gPad->SetLogx();
	fhNCells->SetLineColor(4);
	fhNCells->Draw();
	
	c9->cd(3) ; 
	if(fhNCellsPerCluster->GetEntries() > 0) gPad->SetLogy();
	gPad->SetLogx();
	fhNCellsPerCluster->SetLineColor(4);
	fhNCellsPerCluster->Draw();
	
	c9->cd(4) ; 
	if(fhAmplitude->GetEntries() > 0) gPad->SetLogy();
	//gPad->SetLogx();
	fhAmplitude->SetLineColor(4);
	fhAmplitude->Draw();
	
	sprintf(name,"QA_%s_CaloClustersAndCaloCells.eps",fCalorimeter.Data());
	c9->Print(name); printf("Plot: %s\n",name);
	
	
	//Reconstructed distributions
	//printf("c1\n");
	sprintf(cname,"QA_%s_rec",fCalorimeter.Data());
	TCanvas  * c = new TCanvas(cname, "Reconstructed distributions", 400, 400) ;
	c->Divide(2, 2);
	
	c->cd(1) ; 
	if(fhE->GetEntries() > 0) gPad->SetLogy();
	fhE->SetLineColor(4);
	fhE->Draw();
	
	c->cd(2) ; 
	if(fhPt->GetEntries() > 0) gPad->SetLogy();
	fhPt->SetLineColor(4);
	fhPt->Draw();
	
	c->cd(3) ; 
	fhPhi->SetLineColor(4);
	fhPhi->Draw();
	
	c->cd(4) ; 
	fhEta->SetLineColor(4);
	fhEta->Draw();
	
	sprintf(name,"QA_%s_ReconstructedDistributions.eps",fCalorimeter.Data());
	c->Print(name); printf("Plot: %s\n",name);
	
	//Reconstructed distributions, matched with tracks
	//printf("c2\n");
	sprintf(cname,"QA_%s_rectrackmatch",fCalorimeter.Data());
	TCanvas  * c2 = new TCanvas(cname, "Reconstructed distributions, matched with tracks", 400, 400) ;
	c2->Divide(2, 2);
	
	c2->cd(1) ; 
	if(fhECharged->GetEntries() > 0) gPad->SetLogy();
	fhECharged->SetLineColor(4);
	fhECharged->Draw();
	
	c2->cd(2) ; 
	if(fhPtCharged->GetEntries() > 0) gPad->SetLogy();
	fhPtCharged->SetLineColor(4);
	fhPtCharged->Draw();
	
	c2->cd(3) ; 
	fhPhiCharged->SetLineColor(4);
	fhPhiCharged->Draw();
	
	c2->cd(4) ; 
	fhEtaCharged->SetLineColor(4);
	fhEtaCharged->Draw();
	
	sprintf(name,"QA_%s_ReconstructedDistributions_TrackMatched.eps",fCalorimeter.Data());
	c2->Print(name); printf("Plot: %s\n",name);
	
	TH1F *	hEChargedClone   = (TH1F*)   fhECharged->Clone("EChargedClone");
	TH1F *	hPtChargedClone  = (TH1F*)   fhPtCharged->Clone("PtChargedClone");
	TH1F *	hEtaChargedClone = (TH1F*)   fhEtaCharged->Clone("EtaChargedClone");
	TH1F *	hPhiChargedClone = (TH1F*)   fhPhiCharged->Clone("PhiChargedClone");
	
	TH1F *	hEChargedClone2   = (TH1F*)   fhECharged->Clone("EChargedClone2");
	TH1F *	hPtChargedClone2  = (TH1F*)   fhPtCharged->Clone("PtChargedClone2");
	TH1F *	hEtaChargedClone2 = (TH1F*)   fhEtaCharged->Clone("EtaChargedClone2");
	TH1F *	hPhiChargedClone2 = (TH1F*)   fhPhiCharged->Clone("PhiChargedClone2");
	
	//Ratio: reconstructed track matched/ all reconstructed
	//printf("c3\n");
	sprintf(cname,"QA_%s_rectrackmatchrat",fCalorimeter.Data());
	TCanvas  * c3 = new TCanvas(cname, "Ratio: reconstructed track matched/ all reconstructed", 400, 400) ;
	c3->Divide(2, 2);
	
	c3->cd(1) ;
	if(hEChargedClone->GetEntries() > 0) gPad->SetLogy();
	hEChargedClone->SetTitleOffset(1.6,"Y");
    hEChargedClone->SetYTitle("track matched / all   ");
	hEChargedClone->SetXTitle("E (GeV)");
	hEChargedClone->Divide(fhE);
	hEChargedClone->Draw();
	
	c3->cd(2) ; 
	if(hPtChargedClone->GetEntries() > 0) gPad->SetLogy();
	hPtChargedClone->SetTitleOffset(1.6,"Y");
	hPtChargedClone->SetYTitle("track matched / all   ");
    hPtChargedClone->SetXTitle("p_{T} (GeV/c)");
	hPtChargedClone->Divide(fhPt);
	hPtChargedClone->Draw();
	
	c3->cd(3) ;
	if(hPhiChargedClone->GetEntries() > 0) gPad->SetLogy();
	hPhiChargedClone->SetTitleOffset(1.6,"Y");
	hPhiChargedClone->SetYTitle("track matched / all   ");
	hPhiChargedClone->SetXTitle("#phi (rad)");
	hPhiChargedClone->Divide(fhPhi);
	hPhiChargedClone->Draw();
	
	c3->cd(4) ; 
	if(hEtaChargedClone->GetEntries() > 0) gPad->SetLogy();
	hEtaChargedClone->SetTitleOffset(1.6,"Y");
	hEtaChargedClone->SetYTitle("track matched / all   ");
	hEtaChargedClone->SetXTitle("#eta");
    hEtaChargedClone->Divide(fhEta);
	hEtaChargedClone->Draw();
	
	sprintf(name,"QA_%s_RatioReconstructedMatchedDistributions.eps",fCalorimeter.Data());
	c3->Print(name); printf("Plot: %s\n",name);
	
	//Ratio: reconstructed track matched (minus no track param) / all
	//printf("c3\n");
	sprintf(cname,"QA_%s_rectrackmatchratout",fCalorimeter.Data());
	TCanvas  * c333 = new TCanvas(cname, "Ratio: reconstructed track matched (with outer track param)/ all", 400, 400) ;
	c333->Divide(2, 2);
	
	c333->cd(1) ;
	hEChargedClone2->Add(fhEChargedNoOut,-1);
	hEChargedClone2->SetYTitle("track matched / all");
	hEChargedClone2->SetXTitle("E (GeV)");
	hEChargedClone2->Divide(fhE);
	hEChargedClone2->Draw();
	
	c333->cd(2) ; 
	hPtChargedClone2->Add(fhPtChargedNoOut,-1);
	hPtChargedClone2->SetYTitle("track matched / all");
	hPtChargedClone2->SetXTitle("p_{T} (GeV/c)");
	hPtChargedClone2->Divide(fhPt);
	hPtChargedClone2->Draw();
	
	c333->cd(3) ;
	hPhiChargedClone2->Add(fhPhiChargedNoOut,-1);
	hPhiChargedClone2->SetYTitle("track matched / all");
	hPhiChargedClone2->SetXTitle("#phi (rad)");
	hPhiChargedClone2->Divide(fhPhi);
	hPhiChargedClone2->Draw();
	
	c333->cd(4) ; 
	hEtaChargedClone2->Add(fhEtaChargedNoOut,-1);
	hEtaChargedClone2->SetYTitle("track matched / all");
	hEtaChargedClone2->SetXTitle("#eta");
	hEtaChargedClone2->Divide(fhEta);
	hEtaChargedClone2->Draw();
	
	sprintf(name,"QA_%s_RatioReconstructedMatchedDistributionsOuter.eps",fCalorimeter.Data());
	c333->Print(name); printf("Plot: %s\n",name);
	
	//Reconstructed distributions, matched with tracks but no outer param
	//printf("c2\n");
	sprintf(cname,"QA_%s_rectrackmatch_noout",fCalorimeter.Data());
	TCanvas  * c22 = new TCanvas(cname, "Reconstructed distributions, matched with tracks, no outer track param", 400, 400) ;
	c22->Divide(2, 2);
	
	c22->cd(1) ; 
	if(fhEChargedNoOut->GetEntries() > 0) gPad->SetLogy();
	fhEChargedNoOut->SetLineColor(4);
	fhEChargedNoOut->Draw();
	
	c22->cd(2) ; 
	if(fhEChargedNoOut->GetEntries() > 0) gPad->SetLogy();
	fhPtChargedNoOut->SetLineColor(4);
	fhPtChargedNoOut->Draw();
	
	c22->cd(3) ; 
	fhPhiChargedNoOut->SetLineColor(4);
	fhPhiChargedNoOut->Draw();
	
	c22->cd(4) ; 
	fhEtaChargedNoOut->SetLineColor(4);
	fhEtaChargedNoOut->Draw();
	
	sprintf(name,"QA_%s_ReconstructedDistributions_TrackMatched_NoOutParam.eps",fCalorimeter.Data());
	c22->Print(name); printf("Plot: %s\n",name);
	
	//Ratio: reconstructed track matched/ all reconstructed
	//printf("c3\n");
	
//	TH1F *	hEChargedNoOutClone   = (TH1F*)   fhEChargedNoOut->Clone("EChargedNoOutClone");
//	TH1F *	hPtChargedNoOutClone  = (TH1F*)   fhPtChargedNoOut->Clone("PtChargedNoOutClone");
//	TH1F *	hEtaChargedNoOutClone = (TH1F*)   fhEtaChargedNoOut->Clone("EtaChargedNoOutClone");
//	TH1F *	hPhiChargedNoOutClone = (TH1F*)   fhPhiChargedNoOut->Clone("PhiChargedNoOutClone");	
	
//	sprintf(cname,"QA_%s_rectrackmatchratnoout",fCalorimeter.Data());
//	TCanvas  * c33 = new TCanvas(cname, "Ratio: reconstructed track matched/ all reconstructed", 400, 400) ;
//	c33->Divide(2, 2);
//	
//	c33->cd(1) ;
//	hEChargedNoOutClone->SetYTitle("track matched no out/ all matched");
//	hEChargedNoOutClone->SetXTitle("E (GeV)");
//	hEChargedNoOutClone->Divide(fhECharged);
//	hEChargedNoOutClone->Draw();
//	
//	c33->cd(2) ; 
//	hPtChargedNoOutClone->SetYTitle("track matched no out / all matched");
//	hPtChargedNoOutClone->SetXTitle("p_{T} (GeV/c)");
//	hPtChargedNoOutClone->Divide(fhPtCharged);
//	hPtChargedNoOutClone->Draw();
//	
//	c33->cd(3) ;
//	hPhiChargedNoOutClone->SetYTitle("track matched no out/ all matched");
//	hPhiChargedNoOutClone->SetXTitle("#phi (rad)");
//	hPhiChargedNoOutClone->Divide(fhPhiCharged);
//	hPhiChargedNoOutClone->Draw();
//	
//	c33->cd(4) ; 
//	hEtaChargedNoOutClone->SetYTitle("track matched no out/ all matched");
//	hEtaChargedNoOutClone->SetXTitle("#eta");
//	hEtaChargedNoOutClone->Divide(fhEtaCharged);
//	hEtaChargedNoOutClone->Draw();
//	
//	sprintf(name,"QA_%s_RatioMatchedDistributionsAllToNoOut.eps",fCalorimeter.Data());
//	c33->Print(name); printf("Plot: %s\n",name);
	
	
	//eta vs phi
	//printf("c4\n");
	sprintf(cname,"QA_%s_etavsphi",fCalorimeter.Data());
	//	TCanvas  * c4 = new TCanvas(cname, "reconstructed #eta vs #phi", 600, 200) ;
	//	c4->Divide(3, 1);
	
	TCanvas  * c4 = new TCanvas(cname, "reconstructed #eta vs #phi", 400, 200) ;
	c4->Divide(2, 1);
	
	c4->cd(1) ;
	fhEtaPhi->Draw("cont");
	
	c4->cd(2) ; 
	fhEtaPhiCharged->Draw("cont");
	
	//	c4->cd(3) ; 
	//	fhEtaPhiChargedNoOut->Draw("cont");
	
	sprintf(name,"QA_%s_ReconstructedEtaVsPhi.eps",fCalorimeter.Data());
	c4->Print(name); printf("Plot: %s\n",name);
	
	//Invariant mass
	Int_t binmin = -1;
	Int_t binmax = -1;
	
	if(fhIM->GetEntries() > 1){
		Int_t nebins  = fhIM->GetNbinsX();
		Int_t emax = (Int_t) fhIM->GetXaxis()->GetXmax();
		Int_t emin = (Int_t) fhIM->GetXaxis()->GetXmin();
		if (emin != 0 ) printf("emin != 0 \n");
		//printf("IM: nBinsX %d, emin %2.2f, emax %2.2f\n",nebins,emin,emax);
		
		sprintf(cname,"QA_%s_IM",fCalorimeter.Data());
		//	printf("c5\n");
		TCanvas  * c5 = new TCanvas(cname, "Invariant mass", 400, 400) ;
		c5->Divide(2, 2);
		
		c5->cd(1) ; 
		fhIM->SetLineColor(4);
		fhIM->Draw("lego");
		
		c5->cd(2) ; 
		binmin = 0;
		binmax =  (Int_t) (1-emin)*nebins/emax;
		TH1D *pyim5 = fhIM->ProjectionY("pyim5",binmin,binmax);
		pyim5->SetTitle("0 < E_{pair} < 1 GeV");
		if(pyim5->GetEntries() > 1) gPad->SetLogy();
		pyim5->SetTitle("All Pairs");
		pyim5->SetLineColor(4);
		pyim5->Draw();
		
		c5->cd(3) ; 
		//binmin =  (Int_t) (5-emin)*nebins/emax;
		//binmax =  (Int_t) (10-emin)*nebins/emax;
		//TH1D *pyim510 = fhIM->ProjectionY("pyim5_10",binmin,binmax);
		//pyim510->SetTitle("5 < E_{pair} < 10 GeV");
		binmin =  (Int_t) (1-emin)*nebins/emax;
		binmax =  (Int_t) (2-emin)*nebins/emax;
		TH1D *pyim510 = fhIM->ProjectionY("pyim0_2",binmin,binmax);
		if(pyim510->GetEntries() > 1) gPad->SetLogy();
		pyim510->SetTitle("1 < E_{pair} < 2 GeV");
		
		pyim510->SetLineColor(4);
		pyim510->Draw();
		
		c5->cd(4) ;
		binmin =  (Int_t) (2-emin)*nebins/emax;
		binmax = -1;
		TH1D *pyim10 = fhIM->ProjectionY("pyim10",binmin,binmax);
		pyim10->SetTitle("E_{pair} > 2 GeV");
		if(pyim10->GetEntries() > 1) gPad->SetLogy();
		pyim10->SetLineColor(4);
		pyim10->Draw();
		
		sprintf(name,"QA_%s_InvariantMass.eps",fCalorimeter.Data());
		c5->Print(name); printf("Plot: %s\n",name);
	}
	
	//Asymmetry
	if(fhAsym->GetEntries() > 1){
		Int_t nebins  = fhAsym->GetNbinsX();
		Int_t emax = (Int_t) fhAsym->GetXaxis()->GetXmax();
		Int_t emin = (Int_t) fhAsym->GetXaxis()->GetXmin();
		if (emin != 0 ) printf("emin != 0 \n");
		//printf("Asym: nBinsX %d, emin %2.2f, emax %2.2f\n",nebins,emin,emax);
		
		sprintf(cname,"QA_%s_Asym",fCalorimeter.Data());
		//	printf("c5\n");
		TCanvas  * c5b = new TCanvas(cname, "Asymmetry", 400, 400) ;
		c5b->Divide(2, 2);
		
		c5b->cd(1) ; 
		fhAsym->SetTitleOffset(1.6,"Y");
		fhAsym->SetLineColor(4);
		fhAsym->Draw();
		
		c5b->cd(2) ; 
		binmin = 0;
		binmax = (Int_t) (5-emin)*nebins/emax;
		TH1D *pyAsym5 = fhAsym->ProjectionY("pyAsym5",binmin,binmax);
		pyAsym5->SetTitle("E_{pair} < 5 GeV");
		pyAsym5->SetLineColor(4);
		pyAsym5->Draw();
		
		c5b->cd(3) ; 
		binmin = (Int_t) (5-emin)*nebins/emax;
		binmax = (Int_t) (10-emin)*nebins/emax;
		TH1D *pyAsym510 = fhAsym->ProjectionY("pyAsym5_10",binmin,binmax);
		pyAsym510->SetTitle("5 < E_{pair} < 10 GeV");
		pyAsym510->SetLineColor(4);
		pyAsym510->Draw();
		
		c5b->cd(4) ;
		binmin = (Int_t) (10-emin)*nebins/emax;
		binmax = -1;
		TH1D *pyAsym10 = fhAsym->ProjectionY("pyAsym10",binmin,binmax);
		pyAsym10->SetTitle("E_{pair} > 10 GeV");
		pyAsym10->SetLineColor(4);
		pyAsym10->Draw();
		
		sprintf(name,"QA_%s_Asymmetry.eps",fCalorimeter.Data());
		c5b->Print(name); printf("Plot: %s\n",name);
	}
	
	if(IsDataMC()){
	//Reconstructed vs MC distributions
	//printf("c6\n");
	sprintf(cname,"QA_%s_recvsmc",fCalorimeter.Data());
	TCanvas  * c6 = new TCanvas(cname, "Reconstructed vs MC distributions", 400, 400) ;
	c6->Divide(2, 2);
	
	c6->cd(1) ; 
	fh2E->SetTitleOffset(1.6,"Y");
	fh2E->SetLineColor(4);
	fh2E->Draw();
	
	c6->cd(2) ; 
	fh2Pt->SetTitleOffset(1.6,"Y");
	fh2Pt->SetLineColor(4);
	fh2Pt->Draw();
	
	c6->cd(3) ; 
	fh2Phi->SetTitleOffset(1.6,"Y");
	fh2Phi->SetLineColor(4);
	fh2Phi->Draw();
	
	c6->cd(4) ; 
	fh2Eta->SetTitleOffset(1.6,"Y");
	fh2Eta->SetLineColor(4);
	fh2Eta->Draw();
	
	sprintf(name,"QA_%s_ReconstructedVSMCDistributions.eps",fCalorimeter.Data());
	c6->Print(name); printf("Plot: %s\n",name);	
	
	//Reconstructed vs MC distributions
	//printf("c6\n");
	sprintf(cname,"QA_%s_gamrecvsmc",fCalorimeter.Data());
	TCanvas  * c6Gam = new TCanvas(cname, "Reconstructed vs MC distributions", 400, 400) ;
	c6Gam->Divide(2, 2);
	
	c6Gam->cd(1) ; 
	fhGamE->Draw();
	
	c6Gam->cd(2) ; 
	fhGamPt->Draw();
	
	c6Gam->cd(3) ; 
	fhGamPhi->Draw();
	
	c6Gam->cd(4) ; 
	fhGamEta->Draw();
	
	sprintf(name,"QA_%s_GammaReconstructedVSMCDistributions.eps",fCalorimeter.Data());
	c6->Print(name); printf("Plot: %s\n",name);	
	
	//Generated - reconstructed  
	//printf("c7\n");
	sprintf(cname,"QA_%s_diffgenrec",fCalorimeter.Data());
	TCanvas  * c7 = new TCanvas(cname, "generated - reconstructed", 400, 400) ;
	c7->Divide(2, 2);
	
	c7->cd(1) ; 
	if(fhDeltaE->GetEntries() > 0) gPad->SetLogy();
	fhGamDeltaE->SetLineColor(4);
	fhDeltaE->Draw();
	fhGamDeltaE->Draw("same");
	
	TLegend pLegendd(0.65,0.55,0.9,0.8);
	pLegendd.SetTextSize(0.06);
	pLegendd.AddEntry(fhDeltaE,"all","L");
	pLegendd.AddEntry(fhGamDeltaE,"from  #gamma","L");
	pLegendd.SetFillColor(10);
	pLegendd.SetBorderSize(1);
	pLegendd.Draw();
	
	c7->cd(2) ; 
	if(fhDeltaPt->GetEntries() > 0) gPad->SetLogy();
	fhGamDeltaPt->SetLineColor(4);
	fhDeltaPt->Draw();
	fhGamDeltaPt->Draw("same");
	
	c7->cd(3) ; 
	fhGamDeltaPhi->SetLineColor(4);
	fhDeltaPhi->Draw();
	fhGamDeltaPhi->Draw("same");
	
	c7->cd(4) ; 
	fhGamDeltaEta->SetLineColor(4);
	fhDeltaEta->Draw();
	fhGamDeltaEta->Draw("same");
	
	sprintf(name,"QA_%s_DiffGeneratedReconstructed.eps",fCalorimeter.Data());
	c7->Print(name); printf("Plot: %s\n",name);
	
	// Reconstructed / Generated 
	//printf("c8\n");
	sprintf(cname,"QA_%s_ratiorecgen",fCalorimeter.Data());
	TCanvas  * c8 = new TCanvas(cname, " reconstructed / generated", 400, 400) ;
	c8->Divide(2, 2);
	
	c8->cd(1) ; 
	if(fhRatioE->GetEntries() > 0) gPad->SetLogy();
	fhGamRatioE->SetLineColor(4);
	fhRatioE->Draw();
	fhGamRatioE->Draw("same");
	
	TLegend pLegendr(0.65,0.55,0.9,0.8);
	pLegendr.SetTextSize(0.06);
	pLegendr.AddEntry(fhRatioE,"all","L");
	pLegendr.AddEntry(fhGamRatioE,"from  #gamma","L");
	pLegendr.SetFillColor(10);
	pLegendr.SetBorderSize(1);
	pLegendr.Draw();
	
	c8->cd(2) ; 
	if(fhRatioPt->GetEntries() > 0) gPad->SetLogy();
	fhGamRatioPt->SetLineColor(4);
	fhRatioPt->Draw();
	fhGamRatioPt->Draw("same");
	
	c8->cd(3) ; 
	fhGamRatioPhi->SetLineColor(4);
	fhRatioPhi->Draw();
	fhGamRatioPhi->Draw("same");
	
	c8->cd(4) ; 
	fhGamRatioEta->SetLineColor(4);
	fhRatioEta->Draw();
	fhGamRatioEta->Draw("same");
	
	sprintf(name,"QA_%s_ReconstructedDivGenerated.eps",fCalorimeter.Data());
	c8->Print(name); printf("Plot: %s\n",name);
	
	//MC
	
	//Generated distributions
	//printf("c1\n");
	sprintf(cname,"QA_%s_gen",fCalorimeter.Data());
	TCanvas  * c10 = new TCanvas(cname, "Generated distributions", 600, 200) ;
	c10->Divide(3, 1);
	
	c10->cd(1) ; 
	gPad->SetLogy();
	TH1F * haxispt  = (TH1F*) fhGenPi0Pt->Clone("axispt");  
	haxispt->SetTitle("Generated Particles p_{T}, |#eta| < 1");
	fhGenPi0Pt->SetLineColor(1);
	fhGenGamPt->SetLineColor(4);
	fhGenEtaPt->SetLineColor(2);
	fhGenOmegaPt->SetLineColor(7);
	fhGenElePt->SetLineColor(6);
	
	//Select the maximum of the histogram to show all lines.
	if(fhGenPi0Pt->GetMaximum() >= fhGenGamPt->GetMaximum() && fhGenPi0Pt->GetMaximum() >= fhGenEtaPt->GetMaximum() && 
	   fhGenPi0Pt->GetMaximum() >= fhGenOmegaPt->GetMaximum() && fhGenPi0Pt->GetMaximum() >= fhGenElePt->GetMaximum())
		haxispt->SetMaximum(fhGenPi0Pt->GetMaximum());
	else if(fhGenGamPt->GetMaximum() >= fhGenPi0Pt->GetMaximum() && fhGenGamPt->GetMaximum() >= fhGenEtaPt->GetMaximum() && 
			fhGenGamPt->GetMaximum() >= fhGenOmegaPt->GetMaximum() && fhGenGamPt->GetMaximum() >= fhGenElePt->GetMaximum())
		haxispt->SetMaximum(fhGenGamPt->GetMaximum());
	else if(fhGenEtaPt->GetMaximum() >= fhGenPi0Pt->GetMaximum() && fhGenEtaPt->GetMaximum() >= fhGenGamPt->GetMaximum() && 
			fhGenEtaPt->GetMaximum() >= fhGenOmegaPt->GetMaximum() && fhGenEtaPt->GetMaximum() >= fhGenElePt->GetMaximum())
		haxispt->SetMaximum(fhGenEtaPt->GetMaximum());	
	else if(fhGenOmegaPt->GetMaximum() >= fhGenPi0Pt->GetMaximum() && fhGenOmegaPt->GetMaximum() >= fhGenEtaPt->GetMaximum() && 
			fhGenOmegaPt->GetMaximum() >= fhGenGamPt->GetMaximum() && fhGenOmegaPt->GetMaximum() >= fhGenElePt->GetMaximum())
		haxispt->SetMaximum(fhGenOmegaPt->GetMaximum());
	else if(fhGenElePt->GetMaximum() >= fhGenPi0Pt->GetMaximum() && fhGenElePt->GetMaximum() >= fhGenEtaPt->GetMaximum() && 
			fhGenElePt->GetMaximum() >= fhGenOmegaPt->GetMaximum() && fhGenElePt->GetMaximum() >= fhGenGamPt->GetMaximum())
		haxispt->SetMaximum(fhGenElePt->GetMaximum());
	haxispt->SetMinimum(1);
	haxispt->Draw("axis");
	fhGenPi0Pt->Draw("same");
	fhGenGamPt->Draw("same");
	fhGenEtaPt->Draw("same");
	fhGenOmegaPt->Draw("same");
	fhGenElePt->Draw("same");
	
	TLegend pLegend(0.85,0.65,0.95,0.93);
	pLegend.SetTextSize(0.06);
	pLegend.AddEntry(fhGenPi0Pt,"  #pi^{0}","L");
	pLegend.AddEntry(fhGenGamPt,"  #gamma","L");
	pLegend.AddEntry(fhGenEtaPt,"  #eta","L");
	pLegend.AddEntry(fhGenOmegaPt,"  #omega","L");
	pLegend.AddEntry(fhGenElePt,"  e^{#pm}","L");
	pLegend.SetFillColor(10);
	pLegend.SetBorderSize(1);
	pLegend.Draw();
	
	c10->cd(2) ;
	gPad->SetLogy();
	TH1F * haxiseta  = (TH1F*) fhGenPi0Eta->Clone("axiseta");  
	haxiseta->SetTitle("Generated Particles #eta, |#eta| < 1");
	fhGenPi0Eta->SetLineColor(1);
	fhGenGamEta->SetLineColor(4);
	fhGenEtaEta->SetLineColor(2);
	fhGenOmegaEta->SetLineColor(7);
	fhGenEleEta->SetLineColor(6);
	//Select the maximum of the histogram to show all lines.
	if(fhGenPi0Eta->GetMaximum() >= fhGenGamEta->GetMaximum() && fhGenPi0Eta->GetMaximum() >= fhGenEtaEta->GetMaximum() && 
	   fhGenPi0Eta->GetMaximum() >= fhGenOmegaEta->GetMaximum() && fhGenPi0Eta->GetMaximum() >= fhGenEleEta->GetMaximum())
		haxiseta->SetMaximum(fhGenPi0Eta->GetMaximum());
	else if(fhGenGamEta->GetMaximum() >= fhGenPi0Eta->GetMaximum() && fhGenGamEta->GetMaximum() >= fhGenEtaEta->GetMaximum() && 
			fhGenGamEta->GetMaximum() >= fhGenOmegaEta->GetMaximum() && fhGenGamEta->GetMaximum() >= fhGenEleEta->GetMaximum())
		haxiseta->SetMaximum(fhGenGamEta->GetMaximum());
	else if(fhGenEtaEta->GetMaximum() >= fhGenPi0Eta->GetMaximum() && fhGenEtaEta->GetMaximum() >= fhGenGamEta->GetMaximum() && 
			fhGenEtaEta->GetMaximum() >= fhGenOmegaEta->GetMaximum() && fhGenEtaEta->GetMaximum() >= fhGenEleEta->GetMaximum())
		haxiseta->SetMaximum(fhGenEtaEta->GetMaximum());	
	else if(fhGenOmegaEta->GetMaximum() >= fhGenPi0Eta->GetMaximum() && fhGenOmegaEta->GetMaximum() >= fhGenEtaEta->GetMaximum() && 
			fhGenOmegaEta->GetMaximum() >= fhGenGamEta->GetMaximum() && fhGenOmegaEta->GetMaximum() >= fhGenEleEta->GetMaximum())
		haxiseta->SetMaximum(fhGenOmegaEta->GetMaximum());
	else if(fhGenEleEta->GetMaximum() >= fhGenPi0Eta->GetMaximum() && fhGenEleEta->GetMaximum() >= fhGenEtaEta->GetMaximum() && 
			fhGenEleEta->GetMaximum() >= fhGenOmegaEta->GetMaximum() && fhGenEleEta->GetMaximum() >= fhGenGamEta->GetMaximum())
		haxiseta->SetMaximum(fhGenEleEta->GetMaximum());
	haxiseta->SetMinimum(100);
	haxiseta->Draw("axis");
	fhGenPi0Eta->Draw("same");
	fhGenGamEta->Draw("same");
	fhGenEtaEta->Draw("same");
	fhGenOmegaEta->Draw("same");
	fhGenEleEta->Draw("same");
	
	
	c10->cd(3) ; 
	gPad->SetLogy();
	TH1F * haxisphi  = (TH1F*) fhGenPi0Phi->Clone("axisphi");  
	haxisphi->SetTitle("Generated Particles #phi, |#eta| < 1");
	fhGenPi0Phi->SetLineColor(1);
	fhGenGamPhi->SetLineColor(4);
	fhGenEtaPhi->SetLineColor(2);
	fhGenOmegaPhi->SetLineColor(7);
	fhGenElePhi->SetLineColor(6);
	//Select the maximum of the histogram to show all lines.
	if(fhGenPi0Phi->GetMaximum() >= fhGenGamPhi->GetMaximum() && fhGenPi0Phi->GetMaximum() >= fhGenEtaPhi->GetMaximum() && 
	   fhGenPi0Phi->GetMaximum() >= fhGenOmegaPhi->GetMaximum() && fhGenPi0Phi->GetMaximum() >= fhGenElePhi->GetMaximum())
		haxisphi->SetMaximum(fhGenPi0Phi->GetMaximum());
	else if(fhGenGamPhi->GetMaximum() >= fhGenPi0Phi->GetMaximum() && fhGenGamPhi->GetMaximum() >= fhGenEtaPhi->GetMaximum() && 
			fhGenGamPhi->GetMaximum() >= fhGenOmegaPhi->GetMaximum() && fhGenGamPhi->GetMaximum() >= fhGenElePhi->GetMaximum())
		haxisphi->SetMaximum(fhGenGamPhi->GetMaximum());
	else if(fhGenEtaPhi->GetMaximum() >= fhGenPi0Phi->GetMaximum() && fhGenEtaPhi->GetMaximum() >= fhGenGamPhi->GetMaximum() && 
			fhGenEtaPhi->GetMaximum() >= fhGenOmegaPhi->GetMaximum() && fhGenEtaPhi->GetMaximum() >= fhGenElePhi->GetMaximum())
		haxisphi->SetMaximum(fhGenEtaPhi->GetMaximum());	
	else if(fhGenOmegaPhi->GetMaximum() >= fhGenPi0Phi->GetMaximum() && fhGenOmegaPhi->GetMaximum() >= fhGenEtaPhi->GetMaximum() && 
			fhGenOmegaPhi->GetMaximum() >= fhGenGamPhi->GetMaximum() && fhGenOmegaPhi->GetMaximum() >= fhGenElePhi->GetMaximum())
		haxisphi->SetMaximum(fhGenOmegaPhi->GetMaximum());
	else if(fhGenElePhi->GetMaximum() >= fhGenPi0Phi->GetMaximum() && fhGenElePhi->GetMaximum() >= fhGenEtaPhi->GetMaximum() && 
			fhGenElePhi->GetMaximum() >= fhGenOmegaPhi->GetMaximum() && fhGenElePhi->GetMaximum() >= fhGenGamPhi->GetMaximum())
		haxisphi->SetMaximum(fhGenElePhi->GetMaximum());
	haxisphi->SetMinimum(100);
	haxisphi->Draw("axis");
	fhGenPi0Phi->Draw("same");
	fhGenGamPhi->Draw("same");
	fhGenEtaPhi->Draw("same");
	fhGenOmegaPhi->Draw("same");
	fhGenElePhi->Draw("same");
	
	sprintf(name,"QA_%s_GeneratedDistributions.eps",fCalorimeter.Data());
	c10->Print(name); printf("Plot: %s\n",name);
	
	
	//Reconstructed clusters depending on its original particle.
	//printf("c1\n");
	sprintf(cname,"QA_%s_recgenid",fCalorimeter.Data());
	TCanvas  * c11 = new TCanvas(cname, "Reconstructed particles, function of their original particle ID", 400, 400) ;
	c11->Divide(2, 2);
	
	
	c11->cd(1) ; 
	gPad->SetLogy();
	TH1F * hGamE   = (TH1F*) fhGamE->ProjectionX("hGamE",-1,-1);
	TH1F * hPi0E   = (TH1F*) fhPi0E->ProjectionX("hPi0E",-1,-1);
	TH1F * hEleE   = (TH1F*) fhEleE->ProjectionX("hEleE",-1,-1);
	TH1F * hNeHadE = (TH1F*) fhNeHadE->ProjectionX("hNeHadE",-1,-1);
	TH1F * hChHadE = (TH1F*) fhChHadE->ProjectionX("hChHadE",-1,-1);
	TH1F * haxisE  = (TH1F*) hPi0E->Clone("axisE");  
	haxisE->SetTitle("Reconstructed particles E, function of their original particle ID");
	hPi0E->SetLineColor(1);
	hGamE->SetLineColor(4);
	hNeHadE->SetLineColor(2);
	hChHadE->SetLineColor(7);
	hEleE->SetLineColor(6);
	
	//Select the maximum of the histogram to show all lines.
	if(hPi0E->GetMaximum() >= hGamE->GetMaximum() && hPi0E->GetMaximum() >= hNeHadE->GetMaximum() && 
	   hPi0E->GetMaximum() >= hChHadE->GetMaximum() && hPi0E->GetMaximum() >= hEleE->GetMaximum())
		haxisE->SetMaximum(hPi0E->GetMaximum());
	else if(hGamE->GetMaximum() >= hPi0E->GetMaximum() && hGamE->GetMaximum() >= hNeHadE->GetMaximum() && 
			hGamE->GetMaximum() >= hChHadE->GetMaximum() && hGamE->GetMaximum() >= hEleE->GetMaximum())
		haxisE->SetMaximum(hGamE->GetMaximum());
	else if(hNeHadE->GetMaximum() >= hPi0E->GetMaximum() && hNeHadE->GetMaximum() >= hGamE->GetMaximum() && 
			hNeHadE->GetMaximum() >= hChHadE->GetMaximum() && hNeHadE->GetMaximum() >= hEleE->GetMaximum())
		haxisE->SetMaximum(hNeHadE->GetMaximum());	
	else if(hChHadE->GetMaximum() >= hPi0E->GetMaximum() && hChHadE->GetMaximum() >= hNeHadE->GetMaximum() && 
			hChHadE->GetMaximum() >= hGamE->GetMaximum() && hChHadE->GetMaximum() >= hEleE->GetMaximum())
		haxisE->SetMaximum(hChHadE->GetMaximum());
	else if(hEleE->GetMaximum() >= hPi0E->GetMaximum() && hEleE->GetMaximum() >= hNeHadE->GetMaximum() && 
			hEleE->GetMaximum() >= hChHadE->GetMaximum() && hEleE->GetMaximum() >= hGamE->GetMaximum())
		haxisE->SetMaximum(hEleE->GetMaximum());
	haxisE->SetXTitle("E (GeV)");
	haxisE->SetMinimum(1);
	haxisE->Draw("axis");
	hPi0E->Draw("same");
	hGamE->Draw("same");
	hNeHadE->Draw("same");
	hChHadE->Draw("same");
	hEleE->Draw("same");
	
	TLegend pLegend2(0.8,0.65,0.95,0.93);
	pLegend2.SetTextSize(0.06);
	pLegend2.AddEntry(hPi0E,"  #pi^{0}","L");
	pLegend2.AddEntry(hGamE,"  #gamma","L");
	pLegend2.AddEntry(hEleE,"  e^{#pm}","L");
	pLegend2.AddEntry(hChHadE,"  h^{#pm}","L");
	pLegend2.AddEntry(hNeHadE,"  h^{0}","L");
	pLegend2.SetFillColor(10);
	pLegend2.SetBorderSize(1);
	pLegend2.Draw();
	
	
	c11->cd(2) ; 
	gPad->SetLogy();
	//printf("%s, %s, %s, %s, %s\n",fhGamPt->GetName(),fhPi0Pt->GetName(),fhElePt->GetName(),fhNeHadPt->GetName(), fhChHadPt->GetName());
	TH1F * hGamPt   = (TH1F*) fhGamPt->ProjectionX("hGamPt",-1,-1);
	TH1F * hPi0Pt   = (TH1F*) fhPi0Pt->ProjectionX("hPi0Pt",-1,-1);
	TH1F * hElePt   = (TH1F*) fhElePt->ProjectionX("hElePt",-1,-1);
	TH1F * hNeHadPt = (TH1F*) fhNeHadPt->ProjectionX("hNeHadPt",-1,-1);
	TH1F * hChHadPt = (TH1F*) fhChHadPt->ProjectionX("hChHadPt",-1,-1);
	haxispt  = (TH1F*) hPi0Pt->Clone("axispt");  
	haxispt->SetTitle("Reconstructed particles p_{T}, function of their original particle ID");
	hPi0Pt->SetLineColor(1);
	hGamPt->SetLineColor(4);
	hNeHadPt->SetLineColor(2);
	hChHadPt->SetLineColor(7);
	hElePt->SetLineColor(6);
	
	//Select the maximum of the histogram to show all lines.
	if(hPi0Pt->GetMaximum() >= hGamPt->GetMaximum() && hPi0Pt->GetMaximum() >= hNeHadPt->GetMaximum() && 
	   hPi0Pt->GetMaximum() >= hChHadPt->GetMaximum() && hPi0Pt->GetMaximum() >= hElePt->GetMaximum())
		haxispt->SetMaximum(hPi0Pt->GetMaximum());
	else if(hGamPt->GetMaximum() >= hPi0Pt->GetMaximum() && hGamPt->GetMaximum() >= hNeHadPt->GetMaximum() && 
			hGamPt->GetMaximum() >= hChHadPt->GetMaximum() && hGamPt->GetMaximum() >= hElePt->GetMaximum())
		haxispt->SetMaximum(hGamPt->GetMaximum());
	else if(hNeHadPt->GetMaximum() >= hPi0Pt->GetMaximum() && hNeHadPt->GetMaximum() >= hGamPt->GetMaximum() && 
			hNeHadPt->GetMaximum() >= hChHadPt->GetMaximum() && hNeHadPt->GetMaximum() >= hElePt->GetMaximum())
		haxispt->SetMaximum(hNeHadPt->GetMaximum());	
	else if(hChHadPt->GetMaximum() >= hPi0Pt->GetMaximum() && hChHadPt->GetMaximum() >= hNeHadPt->GetMaximum() && 
			hChHadPt->GetMaximum() >= hGamPt->GetMaximum() && hChHadPt->GetMaximum() >= hElePt->GetMaximum())
		haxispt->SetMaximum(hChHadPt->GetMaximum());
	else if(hElePt->GetMaximum() >= hPi0Pt->GetMaximum() && hElePt->GetMaximum() >= hNeHadPt->GetMaximum() && 
			hElePt->GetMaximum() >= hChHadPt->GetMaximum() && hElePt->GetMaximum() >= hGamPt->GetMaximum())
		haxispt->SetMaximum(hElePt->GetMaximum());
	haxispt->SetXTitle("p_{T} (GeV/c)");
	haxispt->SetMinimum(1);
	haxispt->Draw("axis");
	hPi0Pt->Draw("same");
	hGamPt->Draw("same");
	hNeHadPt->Draw("same");
	hChHadPt->Draw("same");
    hElePt->Draw("same");
	
	
	c11->cd(3) ;
	gPad->SetLogy();
	
	TH1F * hGamEta   = (TH1F*) fhGamEta->ProjectionX("hGamEta",-1,-1);
	TH1F * hPi0Eta   = (TH1F*) fhPi0Eta->ProjectionX("hPi0Eta",-1,-1);
	TH1F * hEleEta   = (TH1F*) fhEleEta->ProjectionX("hEleEta",-1,-1);
	TH1F * hNeHadEta = (TH1F*) fhNeHadEta->ProjectionX("hNeHadEta",-1,-1);
	TH1F * hChHadEta = (TH1F*) fhChHadEta->ProjectionX("hChHadEta",-1,-1);
	haxiseta  = (TH1F*) hPi0Eta->Clone("axiseta");  
	haxiseta->SetTitle("Reconstructed particles #eta, function of their original particle ID");
	hPi0Eta->SetLineColor(1);
	hGamEta->SetLineColor(4);
	hNeHadEta->SetLineColor(2);
	hChHadEta->SetLineColor(7);
	hEleEta->SetLineColor(6);
	//Select the maximum of the histogram to show all lines.
	if(hPi0Eta->GetMaximum() >= hGamEta->GetMaximum() && hPi0Eta->GetMaximum() >= hNeHadEta->GetMaximum() && 
	   hPi0Eta->GetMaximum() >= hChHadEta->GetMaximum() && hPi0Eta->GetMaximum() >= hEleEta->GetMaximum())
		haxiseta->SetMaximum(hPi0Eta->GetMaximum());
	else if(hGamEta->GetMaximum() >= hPi0Eta->GetMaximum() && hGamEta->GetMaximum() >= hNeHadEta->GetMaximum() && 
			hGamEta->GetMaximum() >= hChHadEta->GetMaximum() && hGamEta->GetMaximum() >= hEleEta->GetMaximum())
		haxiseta->SetMaximum(hGamEta->GetMaximum());
	else if(hNeHadEta->GetMaximum() >= hPi0Eta->GetMaximum() && hNeHadEta->GetMaximum() >= hGamEta->GetMaximum() && 
			hNeHadEta->GetMaximum() >= hChHadEta->GetMaximum() && hNeHadEta->GetMaximum() >= hEleEta->GetMaximum())
		haxiseta->SetMaximum(hNeHadEta->GetMaximum());	
	else if(hChHadEta->GetMaximum() >= hPi0Eta->GetMaximum() && hChHadEta->GetMaximum() >= hNeHadEta->GetMaximum() && 
			hChHadEta->GetMaximum() >= hGamEta->GetMaximum() && hChHadEta->GetMaximum() >= hEleEta->GetMaximum())
		haxiseta->SetMaximum(hChHadEta->GetMaximum());
	else if(hEleEta->GetMaximum() >= hPi0Eta->GetMaximum() && hEleEta->GetMaximum() >= hNeHadEta->GetMaximum() && 
			hEleEta->GetMaximum() >= hChHadEta->GetMaximum() && hEleEta->GetMaximum() >= hGamEta->GetMaximum())
		haxiseta->SetMaximum(hEleEta->GetMaximum());
	
    haxiseta->SetXTitle("#eta");
	haxiseta->Draw("axis");
	hPi0Eta->Draw("same");
	hGamEta->Draw("same");
	hNeHadEta->Draw("same");
	hChHadEta->Draw("same");
	hEleEta->Draw("same");
	
	
	c11->cd(4) ; 
	gPad->SetLogy();
	TH1F * hGamPhi   = (TH1F*) fhGamPhi->ProjectionX("hGamPhi",-1,-1);
	TH1F * hPi0Phi   = (TH1F*) fhPi0Phi->ProjectionX("hPi0Phi",-1,-1);
	TH1F * hElePhi   = (TH1F*) fhElePhi->ProjectionX("hElePhi",-1,-1);
	TH1F * hNeHadPhi = (TH1F*) fhNeHadPhi->ProjectionX("hNeHadPhi",-1,-1);
	TH1F * hChHadPhi = (TH1F*) fhChHadPhi->ProjectionX("hChHadPhi",-1,-1);
	haxisphi  = (TH1F*) hPi0Phi->Clone("axisphi");  
	haxisphi->SetTitle("Reconstructed particles #phi, function of their original particle ID");
	
	hPi0Phi->SetLineColor(1);
	hGamPhi->SetLineColor(4);
	hNeHadPhi->SetLineColor(2);
	hChHadPhi->SetLineColor(7);
	hElePhi->SetLineColor(6);
	//Select the maximum of the histogram to show all lines.
	if(hPi0Phi->GetMaximum() >= hGamPhi->GetMaximum() && hPi0Phi->GetMaximum() >= hNeHadPhi->GetMaximum() && 
	   hPi0Phi->GetMaximum() >= hChHadPhi->GetMaximum() && hPi0Phi->GetMaximum() >= hElePhi->GetMaximum())
		haxisphi->SetMaximum(hPi0Phi->GetMaximum());
	else if(hGamPhi->GetMaximum() >= hPi0Phi->GetMaximum() && hGamPhi->GetMaximum() >= hNeHadPhi->GetMaximum() && 
			hGamPhi->GetMaximum() >= hChHadPhi->GetMaximum() && hGamPhi->GetMaximum() >= hElePhi->GetMaximum())
		haxisphi->SetMaximum(hGamPhi->GetMaximum());
	else if(hNeHadPhi->GetMaximum() >= hPi0Phi->GetMaximum() && hNeHadPhi->GetMaximum() >= hGamPhi->GetMaximum() && 
			hNeHadPhi->GetMaximum() >= hChHadPhi->GetMaximum() && hNeHadPhi->GetMaximum() >= hElePhi->GetMaximum())
		haxisphi->SetMaximum(hNeHadPhi->GetMaximum());	
	else if(hChHadPhi->GetMaximum() >= hPi0Phi->GetMaximum() && hChHadPhi->GetMaximum() >= hNeHadPhi->GetMaximum() && 
			hChHadPhi->GetMaximum() >= hGamPhi->GetMaximum() && hChHadPhi->GetMaximum() >= hElePhi->GetMaximum())
		haxisphi->SetMaximum(hChHadPhi->GetMaximum());
	else if(hElePhi->GetMaximum() >= hPi0Phi->GetMaximum() && hElePhi->GetMaximum() >= hNeHadPhi->GetMaximum() && 
			hElePhi->GetMaximum() >= hChHadPhi->GetMaximum() && hElePhi->GetMaximum() >= hGamPhi->GetMaximum())
		haxisphi->SetMaximum(hElePhi->GetMaximum());
	haxisphi->SetXTitle("#phi (rad)");
	haxisphi->Draw("axis");
	hPi0Phi->Draw("same");
	hGamPhi->Draw("same");
	hNeHadPhi->Draw("same");
	hChHadPhi->Draw("same");
	hElePhi->Draw("same");
	
	sprintf(name,"QA_%s_RecDistributionsGenID.eps",fCalorimeter.Data());
	c11->Print(name); printf("Plot: %s\n",name);
	
	
	//Ratio reconstructed clusters / generated particles in acceptance, for different particle ID
	//printf("c1\n");
	
	TH1F *	hPi0EClone   = (TH1F*)   hPi0E  ->Clone("hPi0EClone");
	TH1F *	hGamEClone   = (TH1F*)   hGamE  ->Clone("hGamEClone");
	TH1F *	hPi0PtClone  = (TH1F*)   hPi0Pt ->Clone("hPi0PtClone");
	TH1F *	hGamPtClone  = (TH1F*)   hGamPt ->Clone("hGamPtClone");	
	TH1F *	hPi0EtaClone = (TH1F*)   hPi0Eta->Clone("hPi0EtaClone");
	TH1F *	hGamEtaClone = (TH1F*)   hGamEta->Clone("hGamEtaClone");	
	TH1F *	hPi0PhiClone = (TH1F*)   hPi0Phi->Clone("hPi0PhiClone");
	TH1F *	hGamPhiClone = (TH1F*)   hGamPhi->Clone("hGamPhiClone");	
	
	sprintf(cname,"QA_%s_recgenidratio",fCalorimeter.Data());
	TCanvas  * c12 = new TCanvas(cname, "Ratio reconstructed clusters / generated particles in acceptance, for different particle ID", 400, 400) ;
	c12->Divide(2, 2);
	
	c12->cd(1) ; 
	gPad->SetLogy();
	haxisE->SetTitle("Ratio reconstructed clusters / generated particles in acceptance, for different particle ID");
	hPi0EClone->Divide(fhGenPi0AccE);
	hGamEClone->Divide(fhGenGamAccE);
	haxisE->SetMaximum(5);
	haxisE->SetMinimum(1e-2);
	haxisE->SetXTitle("E (GeV)");
	haxisE->SetYTitle("ratio = rec/gen");
	haxisE->Draw("axis");
	hPi0E->Draw("same");
	hGamE->Draw("same");
	
	TLegend pLegend3(0.75,0.2,0.9,0.4);
	pLegend3.SetTextSize(0.06);
	pLegend3.AddEntry(hPi0EClone,"  #pi^{0}","L");
	pLegend3.AddEntry(hGamEClone,"  #gamma","L");
	pLegend3.SetFillColor(10);
	pLegend3.SetBorderSize(1);
	pLegend3.Draw();
	
	c12->cd(2) ; 
	gPad->SetLogy();
	haxispt->SetTitle("Ratio reconstructed clusters / generated particles in acceptance, for different particle ID");
	hPi0PtClone->Divide(fhGenPi0AccPt);
	hGamPtClone->Divide(fhGenGamAccPt);
	haxispt->SetMaximum(5);
	haxispt->SetMinimum(1e-2);
	haxispt->SetXTitle("p_{T} (GeV/c)");
	haxispt->SetYTitle("ratio = rec/gen");
	haxispt->Draw("axis");
	hPi0PtClone->Draw("same");
	hGamPtClone->Draw("same");
	
	c12->cd(3) ;
	gPad->SetLogy();
	
	haxiseta->SetTitle("Ratio reconstructed clusters / generated particles in acceptance, for different particle ID");
	hPi0EtaClone->Divide(fhGenPi0AccEta);
	hGamEtaClone->Divide(fhGenGamAccEta);
	haxiseta->SetMaximum(1.2);
	haxiseta->SetMinimum(1e-2);
	haxiseta->SetYTitle("ratio = rec/gen");
	haxiseta->SetXTitle("#eta");
	haxiseta->Draw("axis");
	hPi0EtaClone->Draw("same");
	hGamEtaClone->Draw("same");
	
	
	c12->cd(4) ; 
	gPad->SetLogy();
	haxisphi->SetTitle("Ratio reconstructed clusters / generated particles in acceptance, for different particle ID");
	hPi0PhiClone->Divide(fhGenPi0AccPhi);
	hGamPhiClone->Divide(fhGenGamAccPhi);
    haxisphi->SetYTitle("ratio = rec/gen");
	haxisphi->SetXTitle("#phi (rad)");
	haxisphi->SetMaximum(1.2);
	haxisphi->SetMinimum(1e-2);
	haxisphi->Draw("axis");
	hPi0PhiClone->Draw("same");
	hGamPhiClone->Draw("same");
	
	sprintf(name,"QA_%s_EfficiencyGenID.eps",fCalorimeter.Data());
	c12->Print(name); printf("Plot: %s\n",name);
	
	
	
	//Reconstructed distributions
	//printf("c1\n");
	sprintf(cname,"QA_%s_vertex",fCalorimeter.Data());
	TCanvas  * c13 = new TCanvas(cname, "Particle vertex", 400, 400) ;
	c13->Divide(2, 2);
	
	c13->cd(1) ; 
	//gPad->SetLogy();
	fhEMVxyz->SetTitleOffset(1.6,"Y");
    fhEMVxyz->Draw();
	
	c13->cd(2) ; 
	//gPad->SetLogy();
	fhHaVxyz->SetTitleOffset(1.6,"Y");
	fhHaVxyz->Draw();
	
	c13->cd(3) ;
	gPad->SetLogy();
	TH1F * hEMR = (TH1F*) fhEMR->ProjectionY("hEM",-1,-1); 
	hEMR->SetLineColor(4);
	hEMR->Draw();
	
	c13->cd(4) ; 
	gPad->SetLogy();
	TH1F * hHaR = (TH1F*) fhHaR->ProjectionY("hHa",-1,-1); 
	hHaR->SetLineColor(4);
	hHaR->Draw();
	
	
	sprintf(name,"QA_%s_ParticleVertex.eps",fCalorimeter.Data());
	c13->Print(name); printf("Plot: %s\n",name);
	
	
	//Track-matching distributions

	//Reconstructed distributions, matched with tracks, generated particle dependence
	//printf("c2\n");
	sprintf(cname,"QA_%s_rectrackmatchGenID",fCalorimeter.Data());
	TCanvas  * c22ch = new TCanvas(cname, "Reconstructed distributions, matched with tracks, for different particle ID", 400, 400) ;
	c22ch->Divide(2, 2);
	
	c22ch->cd(1) ; 
	
	TH1F * hGamECharged   = (TH1F*) fhGamECharged->ProjectionX("hGamECharged",-1,-1);
	TH1F * hPi0ECharged   = (TH1F*) fhPi0ECharged->ProjectionX("hPi0ECharged",-1,-1);
	TH1F * hEleECharged   = (TH1F*) fhEleECharged->ProjectionX("hEleECharged",-1,-1);
	TH1F * hNeHadECharged = (TH1F*) fhNeHadECharged->ProjectionX("hNeHadECharged",-1,-1);
	TH1F * hChHadECharged = (TH1F*) fhChHadECharged->ProjectionX("hChHadECharged",-1,-1);
	hPi0ECharged->SetLineColor(1);
	hGamECharged->SetLineColor(4);
	hNeHadECharged->SetLineColor(2);
	hChHadECharged->SetLineColor(7);
	hEleECharged->SetLineColor(6);	
	gPad->SetLogy();
	fhECharged->SetLineColor(3);
	fhECharged->SetMinimum(0.5);
	fhECharged->Draw();
	hPi0ECharged->Draw("same");
	hGamECharged->Draw("same");
	hNeHadECharged->Draw("same");
	hChHadECharged->Draw("same");
	hEleECharged->Draw("same");
	TLegend pLegend22(0.75,0.45,0.9,0.8);
	pLegend22.SetTextSize(0.06);
	pLegend22.AddEntry(fhECharged,"all","L");
	pLegend22.AddEntry(hPi0ECharged,"#pi^{0}","L");
	pLegend22.AddEntry(hGamECharged,"#gamma","L");
	pLegend22.AddEntry(hEleECharged,"e^{#pm}","L");
	pLegend22.AddEntry(hChHadECharged,"h^{#pm}","L");
	pLegend22.AddEntry(hNeHadECharged,"h^{0}","L");
	pLegend22.SetFillColor(10);
	pLegend22.SetBorderSize(1);
	pLegend22.Draw();
	
	c22ch->cd(2) ; 
	
	TH1F * hGamPtCharged   = (TH1F*) fhGamPtCharged->ProjectionX("hGamPtCharged",-1,-1);
	TH1F * hPi0PtCharged   = (TH1F*) fhPi0PtCharged->ProjectionX("hPi0PtCharged",-1,-1);
	TH1F * hElePtCharged   = (TH1F*) fhElePtCharged->ProjectionX("hElePtCharged",-1,-1);
	TH1F * hNeHadPtCharged = (TH1F*) fhNeHadPtCharged->ProjectionX("hNeHadPtCharged",-1,-1);
	TH1F * hChHadPtCharged = (TH1F*) fhChHadPtCharged->ProjectionX("hChHadPtCharged",-1,-1);
	hPi0PtCharged->SetLineColor(1);
	hGamPtCharged->SetLineColor(4);
	hNeHadPtCharged->SetLineColor(2);
	hChHadPtCharged->SetLineColor(7);
	hElePtCharged->SetLineColor(6);	
	gPad->SetLogy();
	fhPtCharged->SetLineColor(3);
	fhPtCharged->SetMinimum(0.5);
	fhPtCharged->Draw();
	hPi0PtCharged->Draw("same");
	hGamPtCharged->Draw("same");
	hNeHadPtCharged->Draw("same");
	hChHadPtCharged->Draw("same");
	hElePtCharged->Draw("same");	
	
	c22ch->cd(4) ; 
	
	TH1F * hGamEtaCharged   = (TH1F*) fhGamEtaCharged->ProjectionX("hGamEtaCharged",-1,-1);
	TH1F * hPi0EtaCharged   = (TH1F*) fhPi0EtaCharged->ProjectionX("hPi0EtaCharged",-1,-1);
	TH1F * hEleEtaCharged   = (TH1F*) fhEleEtaCharged->ProjectionX("hEleEtaCharged",-1,-1);
	TH1F * hNeHadEtaCharged = (TH1F*) fhNeHadEtaCharged->ProjectionX("hNeHadEtaCharged",-1,-1);
	TH1F * hChHadEtaCharged = (TH1F*) fhChHadEtaCharged->ProjectionX("hChHadEtaCharged",-1,-1);
	hPi0EtaCharged->SetLineColor(1);
	hGamEtaCharged->SetLineColor(4);
	hNeHadEtaCharged->SetLineColor(2);
	hChHadEtaCharged->SetLineColor(7);
	hEleEtaCharged->SetLineColor(6);	
	gPad->SetLogy();
	fhEtaCharged->SetLineColor(3);
	fhEtaCharged->SetMinimum(0.5);
	fhEtaCharged->Draw();
	hPi0EtaCharged->Draw("same");
	hGamEtaCharged->Draw("same");
	hNeHadEtaCharged->Draw("same");
	hChHadEtaCharged->Draw("same");
	hEleEtaCharged->Draw("same");
	
	c22ch->cd(3) ; 
	
	TH1F * hGamPhiCharged   = (TH1F*) fhGamPhiCharged->ProjectionX("hGamPhiCharged",-1,-1);
	TH1F * hPi0PhiCharged   = (TH1F*) fhPi0PhiCharged->ProjectionX("hPi0PhiCharged",-1,-1);
	TH1F * hElePhiCharged   = (TH1F*) fhElePhiCharged->ProjectionX("hElePhiCharged",-1,-1);
	TH1F * hNeHadPhiCharged = (TH1F*) fhNeHadPhiCharged->ProjectionX("hNeHadPhiCharged",-1,-1);
	TH1F * hChHadPhiCharged = (TH1F*) fhChHadPhiCharged->ProjectionX("hChHadPhiCharged",-1,-1);
	hPi0PhiCharged->SetLineColor(1);
	hGamPhiCharged->SetLineColor(4);
	hNeHadPhiCharged->SetLineColor(2);
	hChHadPhiCharged->SetLineColor(7);
	hElePhiCharged->SetLineColor(6);	
	gPad->SetLogy();
	fhPhiCharged->SetLineColor(3);
	fhPhiCharged->SetMinimum(0.5);
	fhPhiCharged->Draw();
	hPi0PhiCharged->Draw("same");
	hGamPhiCharged->Draw("same");
	hNeHadPhiCharged->Draw("same");
	hChHadPhiCharged->Draw("same");
	hElePhiCharged->Draw("same");
	
	
	sprintf(name,"QA_%s_ReconstructedDistributions_TrackMatchedGenID.eps",fCalorimeter.Data());
	c22ch->Print(name); printf("Plot: %s\n",name);
	
	TH1F *	hGamEChargedClone   = (TH1F*)   hGamECharged->Clone("GamEChargedClone");
	TH1F *	hGamPtChargedClone  = (TH1F*)   hGamPtCharged->Clone("GamPtChargedClone");
	TH1F *	hGamEtaChargedClone = (TH1F*)   hGamEtaCharged->Clone("GamEtaChargedClone");
	TH1F *	hGamPhiChargedClone = (TH1F*)   hGamPhiCharged->Clone("GamPhiChargedClone");
	
	TH1F *	hPi0EChargedClone   = (TH1F*)   hPi0ECharged->Clone("Pi0EChargedClone");
	TH1F *	hPi0PtChargedClone  = (TH1F*)   hPi0PtCharged->Clone("Pi0PtChargedClone");
	TH1F *	hPi0EtaChargedClone = (TH1F*)   hPi0EtaCharged->Clone("Pi0EtaChargedClone");
	TH1F *	hPi0PhiChargedClone = (TH1F*)   hPi0PhiCharged->Clone("Pi0PhiChargedClone");
	
	TH1F *	hEleEChargedClone   = (TH1F*)   hEleECharged->Clone("EleEChargedClone");
	TH1F *	hElePtChargedClone  = (TH1F*)   hElePtCharged->Clone("ElePtChargedClone");
	TH1F *	hEleEtaChargedClone = (TH1F*)   hEleEtaCharged->Clone("EleEtaChargedClone");
	TH1F *	hElePhiChargedClone = (TH1F*)   hElePhiCharged->Clone("ElePhiChargedClone");	
	
	TH1F *	hNeHadEChargedClone   = (TH1F*)   hNeHadECharged->Clone("NeHadEChargedClone");
	TH1F *	hNeHadPtChargedClone  = (TH1F*)   hNeHadPtCharged->Clone("NeHadPtChargedClone");
	TH1F *	hNeHadEtaChargedClone = (TH1F*)   hNeHadEtaCharged->Clone("NeHadEtaChargedClone");
	TH1F *	hNeHadPhiChargedClone = (TH1F*)   hNeHadPhiCharged->Clone("NeHadPhiChargedClone");
	
	TH1F *	hChHadEChargedClone   = (TH1F*)   hChHadECharged->Clone("ChHadEChargedClone");
	TH1F *	hChHadPtChargedClone  = (TH1F*)   hChHadPtCharged->Clone("ChHadPtChargedClone");
	TH1F *	hChHadEtaChargedClone = (TH1F*)   hChHadEtaCharged->Clone("ChHadEtaChargedClone");
	TH1F *	hChHadPhiChargedClone = (TH1F*)   hChHadPhiCharged->Clone("ChHadPhiChargedClone");	
	
	//Ratio: reconstructed track matched/ all reconstructed
	//printf("c3\n");
	sprintf(cname,"QA_%s_rectrackmatchratGenID",fCalorimeter.Data());
	TCanvas  * c3ch = new TCanvas(cname, "Ratio: reconstructed track matched/ all reconstructed, for different particle ID", 400, 400) ;
	c3ch->Divide(2, 2);
	
	c3ch->cd(1) ;
	hEChargedClone->SetMaximum(1.2);
	hEChargedClone->SetMinimum(0.001);	
	hEChargedClone->SetLineColor(3);
	hEChargedClone->SetYTitle("track matched / all");
	hPi0EChargedClone->Divide(hPi0E);
	hGamEChargedClone->Divide(hGamE);
	hEleEChargedClone->Divide(hEleE);
	hNeHadEChargedClone->Divide(hNeHadE);
	hChHadEChargedClone->Divide(hChHadE);
	hEChargedClone->Draw();
	hPi0EChargedClone->Draw("same");
	hGamEChargedClone->Draw("same");
	hEleEChargedClone->Draw("same");
	hNeHadEChargedClone->Draw("same");
	hChHadEChargedClone->Draw("same");
	
	TLegend pLegend3ch(0.75,0.45,0.9,0.8);
	pLegend3ch.SetTextSize(0.06);
	pLegend3ch.AddEntry(hEChargedClone,"all","L");
	pLegend3ch.AddEntry(hPi0EChargedClone,"#pi^{0}","L");
	pLegend3ch.AddEntry(hGamEChargedClone,"#gamma","L");
	pLegend3ch.AddEntry(hEleEChargedClone,"e^{#pm}","L");
	pLegend3ch.AddEntry(hChHadEChargedClone,"h^{#pm}","L");
	pLegend3ch.AddEntry(hNeHadEChargedClone,"h^{0}","L");
	pLegend3ch.SetFillColor(10);
	pLegend3ch.SetBorderSize(1);
	pLegend3ch.Draw();
	
	c3ch->cd(2) ;
	hPtChargedClone->SetMaximum(1.2);
	hPtChargedClone->SetMinimum(0.001);	
	hPtChargedClone->SetLineColor(3);
	hPtChargedClone->SetYTitle("track matched / all");
	hPi0PtChargedClone->Divide(hPi0Pt);
	hGamPtChargedClone->Divide(hGamPt);
	hElePtChargedClone->Divide(hElePt);
	hNeHadPtChargedClone->Divide(hNeHadPt);
	hChHadPtChargedClone->Divide(hChHadPt);
	hPtChargedClone->Draw();
	hPi0PtChargedClone->Draw("same");
	hGamPtChargedClone->Draw("same");
	hElePtChargedClone->Draw("same");
	hNeHadPtChargedClone->Draw("same");
	hChHadPtChargedClone->Draw("same");
	
	c3ch->cd(4) ;
	hEtaChargedClone->SetMaximum(1.2);
	hEtaChargedClone->SetMinimum(0.001);	
	hEtaChargedClone->SetLineColor(3);
	hEtaChargedClone->SetYTitle("track matched / all");
	hPi0EtaChargedClone->Divide(hPi0Eta);
	hGamEtaChargedClone->Divide(hGamEta);
	hEleEtaChargedClone->Divide(hEleEta);
	hNeHadEtaChargedClone->Divide(hNeHadEta);
	hChHadEtaChargedClone->Divide(hChHadEta);
	hEtaChargedClone->Draw();
	hPi0EtaChargedClone->Draw("same");
	hGamEtaChargedClone->Draw("same");
	hEleEtaChargedClone->Draw("same");
	hNeHadEtaChargedClone->Draw("same");
	hChHadEtaChargedClone->Draw("same");
	
	c3ch->cd(3) ;
	hPhiChargedClone->SetMaximum(1.2);
	hPhiChargedClone->SetMinimum(0.001);
	hPhiChargedClone->SetLineColor(3);
	hPhiChargedClone->SetYTitle("track matched / all");
	hPi0PhiChargedClone->Divide(hPi0Phi);
	hGamPhiChargedClone->Divide(hGamPhi);
	hElePhiChargedClone->Divide(hElePhi);
	hNeHadPhiChargedClone->Divide(hNeHadPhi);
	hChHadPhiChargedClone->Divide(hChHadPhi);
	hPhiChargedClone->Draw();
	hPi0PhiChargedClone->Draw("same");
	hGamPhiChargedClone->Draw("same");
	hElePhiChargedClone->Draw("same");
	hNeHadPhiChargedClone->Draw("same");
	hChHadPhiChargedClone->Draw("same");
	
	sprintf(name,"QA_%s_RatioReconstructedMatchedDistributionsGenID.eps",fCalorimeter.Data());
	c3ch->Print(name); printf("Plot: %s\n",name);
	
	}	
	//Track-matching distributions
	if(!strcmp(GetReader()->GetInputEvent()->GetName(),"AliESDEvent")){
		
		
		sprintf(cname,"QA_%s_trkmatch",fCalorimeter.Data());
		TCanvas *cme = new TCanvas(cname,"Track-matching distributions", 400, 400);
		cme->Divide(2,2);
		
		TLegend pLegendpE0(0.6,0.55,0.9,0.8);
		pLegendpE0.SetTextSize(0.04);
		pLegendpE0.AddEntry(fh1pOverE,"all","L");
		pLegendpE0.AddEntry(fh1pOverER02,"dR < 0.02","L");
		pLegendpE0.SetFillColor(10);
		pLegendpE0.SetBorderSize(1);
		//pLegendpE0.Draw();
		
		cme->cd(1);
		if(fh1pOverE->GetEntries() > 0) gPad->SetLogy();
		fh1pOverE->SetTitle("Track matches p/E");
		fh1pOverE->Draw();
		fh1pOverER02->SetLineColor(4);
		fh1pOverER02->Draw("same");
		pLegendpE0.Draw();
		
		cme->cd(2);
		if(fh1dR->GetEntries() > 0) gPad->SetLogy();
		fh1dR->Draw();
		
		cme->cd(3);
		fh2MatchdEdx->Draw();
		
		cme->cd(4);
		fh2EledEdx->Draw();
		
		sprintf(name,"QA_%s_TrackMatchingEleDist.eps",fCalorimeter.Data());
		cme->Print(name); printf("Plot: %s\n",name);       
		
		if(IsDataMC()){
		sprintf(cname,"QA_%s_trkmatchMCEle",fCalorimeter.Data());
		TCanvas *cmemc = new TCanvas(cname,"Track-matching distributions from MC electrons", 600, 200);
		cmemc->Divide(3,1);
		
		cmemc->cd(1);
		gPad->SetLogy();
		fhMCEle1pOverE->Draw();
		fhMCEle1pOverER02->SetLineColor(4);
		fhMCEle1pOverE->SetLineColor(1);
		fhMCEle1pOverER02->Draw("same");
		pLegendpE0.Draw();
		
		cmemc->cd(2);
		gPad->SetLogy();
		fhMCEle1dR->Draw();
		
		cmemc->cd(3);
		fhMCEle2MatchdEdx->Draw();
		
		sprintf(name,"QA_%s_TrackMatchingDistMCEle.eps",fCalorimeter.Data());
		cmemc->Print(name); printf("Plot: %s\n",name);  
	
		
		sprintf(cname,"QA_%s_trkmatchMCChHad",fCalorimeter.Data());
		TCanvas *cmemchad = new TCanvas(cname,"Track-matching distributions from MC charged hadrons", 600, 200);
		cmemchad->Divide(3,1);
		
		cmemchad->cd(1);
		gPad->SetLogy();
		fhMCChHad1pOverE->Draw();
		fhMCChHad1pOverER02->SetLineColor(4);
		fhMCChHad1pOverE->SetLineColor(1);
		fhMCChHad1pOverER02->Draw("same");
		pLegendpE0.Draw();
		
		cmemchad->cd(2);
		gPad->SetLogy();
		fhMCChHad1dR->Draw();
		
		cmemchad->cd(3);
		fhMCChHad2MatchdEdx->Draw();
		
		sprintf(name,"QA_%s_TrackMatchingDistMCChHad.eps",fCalorimeter.Data());
		cmemchad->Print(name); printf("Plot: %s\n",name);       
		
		sprintf(cname,"QA_%s_trkmatchMCNeutral",fCalorimeter.Data());
		TCanvas *cmemcn = new TCanvas(cname,"Track-matching distributions from MC neutrals", 600, 200);
		cmemcn->Divide(3,1);
		
		cmemcn->cd(1);
		gPad->SetLogy();
		fhMCNeutral1pOverE->Draw();
		fhMCNeutral1pOverE->SetLineColor(1);
		fhMCNeutral1pOverER02->SetLineColor(4);
		fhMCNeutral1pOverER02->Draw("same");
		pLegendpE0.Draw();
		
		cmemcn->cd(2);
		gPad->SetLogy();
		fhMCNeutral1dR->Draw();
		
		cmemcn->cd(3);
		fhMCNeutral2MatchdEdx->Draw();
		
		sprintf(name,"QA_%s_TrackMatchingDistMCNeutral.eps",fCalorimeter.Data());
		cmemcn->Print(name); printf("Plot: %s\n",name);       
		
		sprintf(cname,"QA_%s_trkmatchpE",fCalorimeter.Data());
		TCanvas *cmpoe = new TCanvas(cname,"Track-matching distributions, p/E", 400, 200);
		cmpoe->Divide(2,1);
		
		cmpoe->cd(1);
		gPad->SetLogy();
		fh1pOverE->SetLineColor(1);
		fhMCEle1pOverE->SetLineColor(4);
		fhMCChHad1pOverE->SetLineColor(2);
		fhMCNeutral1pOverE->SetLineColor(7);
		fh1pOverER02->SetMinimum(0.5);
		fh1pOverE->Draw();
		fhMCEle1pOverE->Draw("same");
		fhMCChHad1pOverE->Draw("same");
		fhMCNeutral1pOverE->Draw("same");
		TLegend pLegendpE(0.65,0.55,0.9,0.8);
		pLegendpE.SetTextSize(0.06);
		pLegendpE.AddEntry(fh1pOverE,"all","L");
		pLegendpE.AddEntry(fhMCEle1pOverE,"e^{#pm}","L");
		pLegendpE.AddEntry(fhMCChHad1pOverE,"h^{#pm}","L");
		pLegendpE.AddEntry(fhMCNeutral1pOverE,"neutrals","L");
		pLegendpE.SetFillColor(10);
		pLegendpE.SetBorderSize(1);
		pLegendpE.Draw();
		
		cmpoe->cd(2);
		gPad->SetLogy();
		fh1pOverER02->SetTitle("Track matches p/E, dR<0.2");
		fh1pOverER02->SetLineColor(1);
		fhMCEle1pOverER02->SetLineColor(4);
		fhMCChHad1pOverER02->SetLineColor(2);
		fhMCNeutral1pOverER02->SetLineColor(7);
		fh1pOverER02->SetMaximum(fh1pOverE->GetMaximum());
		fh1pOverER02->SetMinimum(0.5);
		fh1pOverER02->Draw();
		fhMCEle1pOverER02->Draw("same");
		fhMCChHad1pOverER02->Draw("same");
		fhMCNeutral1pOverER02->Draw("same");
		
		//		TLegend pLegendpE2(0.65,0.55,0.9,0.8);
		//		pLegendpE2.SetTextSize(0.06);
		//		pLegendpE2.SetHeader("dR < 0.02");
		//		pLegendpE2.SetFillColor(10);
		//		pLegendpE2.SetBorderSize(1);
		//		pLegendpE2.Draw();
		
		sprintf(name,"QA_%s_TrackMatchingPOverE.eps",fCalorimeter.Data());
		cmpoe->Print(name); printf("Plot: %s\n",name);       			
		}
		
	}
	
	
	sprintf(line, ".!tar -zcf QA_%s_%s.tar.gz *%s*.eps", fCalorimeter.Data(), GetName(),fCalorimeter.Data()) ; 
	gROOT->ProcessLine(line);
	sprintf(line, ".!rm -fR *.eps"); 
	gROOT->ProcessLine(line);
	
	printf("AliAnaCalorimeterQA::Terminate() - !! All the eps files are in QA_%s_%s.tar.gz !!!\n",  fCalorimeter.Data(), GetName());
	
}

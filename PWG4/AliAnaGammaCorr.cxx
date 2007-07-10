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

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class for the analysis of gamma correlations (gamma-jet, 
// gamma-hadron, input is an isolated photon 
//
//  Class created from old AliPHOSGammaJet 
//  (see AliRoot versions previous Release 4-09)
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include <TFile.h>
#include <TParticle.h>
#include <TH2.h>
#include <TChain.h>
#include "AliAnaGammaCorr.h" 
#include "AliESD.h"
#include "AliESDtrack.h"
#include "AliESDCaloCluster.h"
#include "Riostream.h"
#include "AliLog.h"

ClassImp(AliAnaGammaCorr)

//____________________________________________________________________________
  AliAnaGammaCorr::AliAnaGammaCorr(const char *name) : 
    AliAnalysisTask(name,""),  fDirectGamma(0x0), fCaloList(0x0),  fCTSList(0x0),
    fOutputContainer(0x0), fAnaType(0), fDataType(0),
    //kParton
    fhDeltaEtaParton(0x0), fhDeltaPhiParton(0x0), fhDeltaPtParton(0x0), fhPtRatParton(0x0),
    //kHadron and kJetLeadCone 
    fJetsOnlyInCTS(0),
    fInvMassMaxCut(0.), fInvMassMinCut(0.),
    fAngleMaxParam(),  
    fhPhiCharged(0), fhPhiNeutral(0), fhEtaCharged(0), fhEtaNeutral(0), 
    fhDeltaPhiGammaCharged(0),  fhDeltaPhiGammaNeutral(0), 
    fhDeltaEtaGammaCharged(0), fhDeltaEtaGammaNeutral(0), 
    fhAnglePairAccepted(0), fhAnglePairNoCut(0), fhAnglePairAzimuthCut(0), 
    fhAnglePairOpeningAngleCut(0), fhAnglePairAllCut(0), 
    fhInvMassPairNoCut(0), fhInvMassPairAzimuthCut(0), 
    fhInvMassPairOpeningAngleCut(0), fhInvMassPairAllCut(0) ,
    //kHadron 
    fMinPtPion(0),
    fPhiMaxCut(0.), fPhiMinCut(0.), 
    fhCorrelationGammaNeutral(0), fhCorrelationGammaCharged(0),
    //kJetLeadCone 
    fPbPb(kFALSE),     
    fSeveralConeAndPtCuts(0),  
    fEtaEMCALCut(0.),
    fPhiLeadingMaxCut(0.), fPhiLeadingMinCut(0.), 
    fRatioMaxCut(0), fRatioMinCut(0),
    fJetCTSRatioMaxCut(0.), fJetCTSRatioMinCut(0.), fJetRatioMaxCut(0.),
    fJetRatioMinCut(0.), 
    fJetNCone(0),fJetNPt(0), fJetCone(0), 
    fJetPtThreshold(0),fJetPtThresPbPb(0),
    fPtJetSelectionCut(0.0), fSelect(0),
    fhAnglePairLeading(), fhInvMassPairLeading(), 
    fhChargeRatio(0), fhPi0Ratio (0), 
    fhNBkg (0), fhNLeading(0), fhNJet(0), fhJetRatio(0), fhJetPt (0), 
    fhBkgRatio (0), fhBkgPt(0),  fhJetFragment(0), fhBkgFragment(0), 
    fhJetPtDist(0),  fhBkgPtDist(0) 
{
  //Ctor

  // kGammaHadron and kGammaJet 
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.Reset(0.);
  
  // kGammaJet 
  for(Int_t i = 0; i<10; i++){
    fJetCones[i]         = 0.0 ;
    fJetNameCones[i]     = ""  ;
    fJetPtThres[i]      = 0.0 ;
    fJetNamePtThres[i]  = ""  ;
    if( i < 6 ){
      fJetXMin1[i]     = 0.0 ;
      fJetXMin2[i]     = 0.0 ;
      fJetXMax1[i]     = 0.0 ;
      fJetXMax2[i]     = 0.0 ;
      fBkgMean[i]      = 0.0 ;
      fBkgRMS[i]       = 0.0 ;
      if( i < 2 ){
	fJetE1[i]        = 0.0 ;
	fJetE2[i]        = 0.0 ;
	fJetSigma1[i]    = 0.0 ;
	fJetSigma2[i]    = 0.0 ;
	fPhiEMCALCut[i]  = 0.0 ;
      }
    }
  }
    
  //Initialize parameters
  InitParameters();

  // Input slot #0 works with an Ntuple
  DefineInput(0, TParticle::Class());
  DefineInput(1, TClonesArray::Class());
  DefineInput(2, TClonesArray::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(0,  TList::Class()) ; 
}


//____________________________________________________________________________
AliAnaGammaCorr::AliAnaGammaCorr(const AliAnaGammaCorr & g) : 
  AliAnalysisTask(g), fDirectGamma(g.fDirectGamma), fCaloList(g.fCaloList),  fCTSList(g.fCTSList),
  fOutputContainer(g.fOutputContainer), fAnaType(g.fAnaType), fDataType(g.fDataType),  
  //kParton
  fhDeltaEtaParton(g.fhDeltaEtaParton), fhDeltaPhiParton(g.fhDeltaPhiParton), 
  fhDeltaPtParton(g.fhDeltaPtParton), fhPtRatParton(g.fhPtRatParton),
  //kGammaHadron and kGammaJet 
  fJetsOnlyInCTS(g.fJetsOnlyInCTS), 
  fInvMassMaxCut(g.fInvMassMaxCut), fInvMassMinCut(g.fInvMassMinCut),
  fAngleMaxParam(g.fAngleMaxParam),
  fhPhiCharged(g.fhPhiCharged), fhPhiNeutral(g.fhPhiNeutral), 
  fhEtaCharged(g.fhEtaCharged), fhEtaNeutral(g.fhEtaNeutral), 
  fhDeltaPhiGammaCharged(g.fhDeltaPhiGammaCharged),  
  fhDeltaPhiGammaNeutral(g.fhDeltaPhiGammaNeutral), 
  fhDeltaEtaGammaCharged(g.fhDeltaEtaGammaCharged), 
  fhDeltaEtaGammaNeutral(g.fhDeltaEtaGammaNeutral), 
  fhAnglePairAccepted(g.fhAnglePairAccepted), 
  fhAnglePairNoCut(g. fhAnglePairNoCut), fhAnglePairAzimuthCut(g.fhAnglePairAzimuthCut), 
  fhAnglePairOpeningAngleCut(g. fhAnglePairOpeningAngleCut), 
  fhAnglePairAllCut(g. fhAnglePairAllCut), 
  fhInvMassPairNoCut(g.fhInvMassPairNoCut), 
  fhInvMassPairAzimuthCut(g.fhInvMassPairAzimuthCut), 
  fhInvMassPairOpeningAngleCut(g.fhInvMassPairOpeningAngleCut), 
  fhInvMassPairAllCut(g.fhInvMassPairAllCut),
  //kGammaHadron 
  fMinPtPion(g.fMinPtPion),
  fPhiMaxCut(g.fPhiMaxCut), fPhiMinCut(g.fPhiMinCut), 
  fhCorrelationGammaNeutral(g.fhCorrelationGammaNeutral), 
  fhCorrelationGammaCharged(g.fhCorrelationGammaCharged),
  //kGammaJet 
  fPbPb(g.fPbPb),
  fSeveralConeAndPtCuts(g.fSeveralConeAndPtCuts), 
  fEtaEMCALCut(g.fEtaEMCALCut),
  fPhiLeadingMaxCut(g.fPhiLeadingMaxCut), fPhiLeadingMinCut(g.fPhiLeadingMinCut), 
  fRatioMaxCut(g.fRatioMaxCut), fRatioMinCut(g.fRatioMinCut), 
  fJetCTSRatioMaxCut(g.fJetCTSRatioMaxCut),
  fJetCTSRatioMinCut(g.fJetCTSRatioMinCut), fJetRatioMaxCut(g.fJetRatioMaxCut),
  fJetRatioMinCut(g.fJetRatioMinCut),  fJetNCone(g.fJetNCone),
  fJetNPt(g.fJetNPt), fJetCone(g.fJetCone),
  fJetPtThreshold(g.fJetPtThreshold),fJetPtThresPbPb(g.fJetPtThresPbPb),
  fPtJetSelectionCut(g.fPtJetSelectionCut), fSelect(g.fSelect),  
  fhAnglePairLeading(g.fhAnglePairLeading), fhInvMassPairLeading(g.fhInvMassPairLeading), 
  fhChargeRatio(g.fhChargeRatio), fhPi0Ratio(g.fhPi0Ratio), 
  fhNBkg(g. fhNBkg), fhNLeading(g. fhNLeading), fhNJet(g.fhNJet), fhJetRatio(g.fhJetRatio), fhJetPt(g.fhJetPt), 
  fhBkgRatio (g.fhBkgRatio), fhBkgPt(g.fhBkgPt),  fhJetFragment(g.fhJetFragment), fhBkgFragment(g.fhBkgFragment), 
  fhJetPtDist(g.fhJetPtDist),  fhBkgPtDist(g.fhBkgPtDist)   
{
  // cpy ctor
  SetName (g.GetName()) ; 
  SetTitle(g.GetTitle()) ; 

 //kGammaJet and kGammaHadron
  fPhiEMCALCut[0]  =   g.fPhiEMCALCut[0];
  fPhiEMCALCut[1]  =   g.fPhiEMCALCut[1] ;

 //kGammaJet
  for(Int_t i = 0; i<10; i++){
    fJetCones[i]        = g.fJetCones[i] ;
    fJetNameCones[i]    = g.fJetNameCones[i] ;
    fJetPtThres[i]      = g.fJetPtThres[i] ;
    fJetNamePtThres[i]  = g.fJetNamePtThres[i] ;
    if( i < 6 ){
      fJetXMin1[i]       = g.fJetXMin1[i] ;
      fJetXMin2[i]       = g.fJetXMin2[i] ;
      fJetXMax1[i]       = g.fJetXMax1[i] ;
      fJetXMax2[i]       = g.fJetXMax2[i] ;
      fBkgMean[i]        = g.fBkgMean[i] ;
      fBkgRMS[i]         = g.fBkgRMS[i] ;
      if( i < 2 ){
	fJetE1[i]        = g.fJetE1[i] ;
	fJetE2[i]        = g.fJetE2[i] ;
	fJetSigma1[i]    = g.fJetSigma1[i] ;
	fJetSigma2[i]    = g.fJetSigma2[i] ;
	fPhiEMCALCut[i]  = g.fPhiEMCALCut[i] ;
      }
    }          
  } 

}

//_________________________________________________________________________
AliAnaGammaCorr & AliAnaGammaCorr::operator = (const AliAnaGammaCorr & source)
{
  // assignment operator

  if(&source == this) return *this;

  fOutputContainer = source.fOutputContainer ;
  fDirectGamma = source.fDirectGamma ;
  fAnaType = source.fAnaType ;
  fDataType = source.fDataType ;

  //kParton
  fhDeltaEtaParton = source.fhDeltaEtaParton;
  fhDeltaPhiParton = source.fhDeltaPhiParton;
  fhDeltaPtParton = source.fhDeltaPtParton;
  fhPtRatParton = source.fhPtRatParton;
  
 //kGammaHadron and kGammaJet
  fJetsOnlyInCTS = source.fJetsOnlyInCTS ; 
  fInvMassMaxCut = source.fInvMassMaxCut ; fInvMassMinCut = source.fInvMassMinCut ;
  fAngleMaxParam = source.fAngleMaxParam ;

  fhPhiCharged = source.fhPhiCharged ; fhPhiNeutral = source.fhPhiNeutral ; 
  fhEtaCharged = source.fhEtaCharged ; fhEtaNeutral = source.fhEtaNeutral ; 
  fhDeltaPhiGammaCharged = source.fhDeltaPhiGammaCharged ;  
  fhDeltaPhiGammaNeutral = source.fhDeltaPhiGammaNeutral ; 
  fhDeltaEtaGammaCharged = source.fhDeltaEtaGammaCharged ; 
  fhDeltaEtaGammaNeutral = source.fhDeltaEtaGammaNeutral ; 

  fhAnglePairAccepted = source.fhAnglePairAccepted ; 
  fhAnglePairNoCut = source. fhAnglePairNoCut ; 
  fhAnglePairAzimuthCut = source.fhAnglePairAzimuthCut ; 
  fhAnglePairOpeningAngleCut = source. fhAnglePairOpeningAngleCut ; 
  fhAnglePairAllCut = source. fhAnglePairAllCut ; 
  fhInvMassPairNoCut = source.fhInvMassPairNoCut ; 
  fhInvMassPairAzimuthCut = source.fhInvMassPairAzimuthCut ; 
  fhInvMassPairOpeningAngleCut = source.fhInvMassPairOpeningAngleCut ; 
  fhInvMassPairAllCut = source.fhInvMassPairAllCut ; 

  //kGammaHadron
  fMinPtPion = source.fMinPtPion ;
  fPhiMaxCut = source.fPhiMaxCut ; fPhiMinCut = source.fPhiMinCut ; 

  fhCorrelationGammaNeutral = source.fhCorrelationGammaNeutral ; 
  fhCorrelationGammaCharged = source.fhCorrelationGammaCharged ; 

 //kGammaJet
  fSeveralConeAndPtCuts = source.fSeveralConeAndPtCuts ; 
  fPbPb = source.fPbPb ;
  fEtaEMCALCut = source.fEtaEMCALCut ;
  fPhiLeadingMaxCut = source.fPhiLeadingMaxCut ; fPhiLeadingMinCut = source.fPhiLeadingMinCut ; 
  fInvMassMaxCut = source.fInvMassMaxCut ; fInvMassMinCut = source.fInvMassMinCut ;
  fRatioMaxCut = source.fRatioMaxCut ; fRatioMinCut = source.fRatioMinCut ; 
  fJetCTSRatioMaxCut = source.fJetCTSRatioMaxCut ;
  fJetCTSRatioMinCut = source.fJetCTSRatioMinCut ; fJetRatioMaxCut = source.fJetRatioMaxCut ;
  fJetRatioMinCut = source.fJetRatioMinCut ;  fJetNCone = source.fJetNCone ;
  fJetNPt = source.fJetNPt ; fJetCone = source.fJetCone ; 
  fJetPtThreshold = source.fJetPtThreshold ;
  fJetPtThresPbPb = source.fJetPtThresPbPb ;
  fPtJetSelectionCut = source.fPtJetSelectionCut ;
  fAngleMaxParam = source.fAngleMaxParam ; 
  fSelect = source.fSelect ;  fhChargeRatio = source.fhChargeRatio ; fhPi0Ratio = source.fhPi0Ratio ; 

  fhAnglePairLeading = source.fhAnglePairLeading ; 
  fhInvMassPairLeading = source.fhInvMassPairLeading ; 
  fhNBkg = source. fhNBkg ; fhNLeading = source. fhNLeading ; 
  fhNJet = source.fhNJet ; fhJetRatio = source.fhJetRatio ; fhJetPt = source.fhJetPt ; 
  fhBkgRatio  = source.fhBkgRatio ; fhBkgPt = source.fhBkgPt ;  
  fhJetFragment = source.fhJetFragment ; fhBkgFragment = source.fhBkgFragment ; 
  fhJetPtDist = source.fhJetPtDist ;  fhBkgPtDist = source.fhBkgPtDist ;


  for(Int_t i = 0; i<10; i++){
    fJetCones[i]        = source.fJetCones[i] ;
    fJetNameCones[i]    = source.fJetNameCones[i] ;
    fJetPtThres[i]      = source.fJetPtThres[i] ;
    fJetNamePtThres[i]  = source.fJetNamePtThres[i] ;
    if( i < 6 ){
      fJetXMin1[i]       = source.fJetXMin1[i] ;
      fJetXMin2[i]       = source.fJetXMin2[i] ;
      fJetXMax1[i]       = source.fJetXMax1[i] ;
      fJetXMax2[i]       = source.fJetXMax2[i] ;
      fBkgMean[i]        = source.fBkgMean[i] ;
      fBkgRMS[i]         = source.fBkgRMS[i] ;
      if( i < 2 ){
	fJetE1[i]        = source.fJetE1[i] ;
	fJetE2[i]        = source.fJetE2[i] ;
	fJetSigma1[i]    = source.fJetSigma1[i] ;
	fJetSigma2[i]    = source.fJetSigma2[i] ;
	fPhiEMCALCut[i]  = source.fPhiEMCALCut[i] ;
      }
    }          
  } 

  return *this;

}

//____________________________________________________________________________
AliAnaGammaCorr::~AliAnaGammaCorr() 
{
  // Remove all pointers
  fOutputContainer->Clear() ; 
  delete fOutputContainer ;

  fCaloList->Clear() ; 
  delete fCaloList ;

  fCTSList->Clear() ; 
  delete fCTSList ;

  delete fDirectGamma ;
  //kParton
  delete fhDeltaEtaParton ;
  delete fhDeltaPhiParton ;
  delete fhDeltaPtParton ;
  delete fhPtRatParton ;

  //kJetLeadCone, kHadron 
  delete fhPhiCharged  ;  
  delete fhPhiNeutral   ; 
  delete fhEtaCharged  ; 
  delete fhEtaNeutral  ; 
  delete fhDeltaPhiGammaCharged  ;  
  delete fhDeltaPhiGammaNeutral   ; 
  delete fhDeltaEtaGammaCharged  ; 
  delete fhDeltaEtaGammaNeutral  ; 

  delete fhAnglePairNoCut  ; 
  delete fhAnglePairAzimuthCut  ; 
  delete fhAnglePairOpeningAngleCut   ; 
  delete fhAnglePairAllCut   ;  
  delete fhInvMassPairNoCut    ; 
  delete fhInvMassPairAzimuthCut  ; 
  delete fhInvMassPairOpeningAngleCut  ; 
  delete fhInvMassPairAllCut   ;    

  //kHadron
  delete fhCorrelationGammaNeutral  ; 
  delete fhCorrelationGammaCharged  ;
 
  //kJetLeadCone   
  delete fhChargeRatio  ; 
  delete fhPi0Ratio   ; 
  delete fhDeltaPhiCharge  ;  
  delete fhDeltaPhiPi0   ; 
  delete fhDeltaEtaCharge  ; 
  delete fhDeltaEtaPi0  ; 

  delete fhAnglePairLeading  ; 
  delete fhInvMassPairLeading  ; 
  delete fhNBkg   ; 
  delete fhNLeading  ; 
  delete fhNJet  ; 
  delete fhJetRatio  ; 
  delete fhJetPt   ; 
  delete fhBkgRatio   ; 
  delete fhBkgPt  ; 
  delete fhJetFragment  ; 
  delete fhBkgFragment  ; 
  delete fhJetPtDist  ; 
  delete fhBkgPtDist  ; 
  
  delete [] fhJetRatios;  
  delete [] fhJetPts;  
  delete [] fhBkgRatios;
  delete [] fhBkgPts;  

  delete [] fhNLeadings;
  delete [] fhNJets;  
  delete [] fhNBkgs;
  
  delete [] fhJetFragments;
  delete [] fhBkgFragments;
  delete [] fhJetPtDists;
  delete [] fhBkgPtDists;
}

//______________________________________________________________________________
void AliAnaGammaCorr::ConnectInputData(const Option_t*)
{
  // Initialisation of branch container and histograms 
    
  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  
  // Get input data
  fDirectGamma = dynamic_cast<TParticle *>(GetInputData(0)) ;
  if (!fDirectGamma) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }
  fCaloList = dynamic_cast<TClonesArray *>(GetInputData(1)) ;
  if (!fCaloList) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }
  
  fCTSList = dynamic_cast<TClonesArray *>(GetInputData(2)) ;
  if (!fCTSList) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }

}

//________________________________________________________________________
void AliAnaGammaCorr::CreateOutputObjects()
{  

  // Create histograms to be saved in output file and 
  // store them in fOutputContainer
  OpenFile(0) ; 
  fOutputContainer = new TList() ; 
  fOutputContainer->SetName("DirectGammaHistos") ; 
 
  //kParton
  fhDeltaPhiParton  = new TH2F
    ("DeltaPhiParton","#phi_{#gamma} - #phi_{parton} vs p_{T #gamma}",
     200,0,120,200,0,6.4); 
  fhDeltaPhiParton->SetYTitle("#Delta #phi");
  fhDeltaPhiParton->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhDeltaEtaParton  = new TH2F
    ("DeltaEtaParton","#eta_{#gamma} - #eta_{parton} vs p_{T #gamma}",
     200,0,120,200,-2,2); 
  fhDeltaEtaParton->SetYTitle("#Delta #eta");
  fhDeltaEtaParton->SetXTitle("p_{T #gamma} (GeV/c)");

  fhDeltaPtParton  = new TH2F
    ("DeltaPtParton","#p_{T #gamma} - #p_{T parton} vs p_{T #gamma}",
     200,0,120,100,-10,10); 
  fhDeltaPtParton->SetYTitle("#Delta #p_{T}");
  fhDeltaPtParton->SetXTitle("p_{T #gamma} (GeV/c)");

  fhPtRatParton  = new TH2F
    ("PtRatParton","#p_{T parton} / #p_{T #gamma} vs p_{T #gamma}",
     200,0,120,200,0,5); 
  fhPtRatParton->SetYTitle("ratio");
  fhPtRatParton->SetXTitle("p_{T #gamma} (GeV/c)");
  
  //---- kHadron and kJetLeadCone ----
  fhPhiCharged  = new TH2F
    ("PhiCharged","#phi_{#pi^{#pm}}  vs p_{T #gamma}",
     120,0,120,120,0,7); 
  fhPhiCharged->SetYTitle("#phi_{#pi^{#pm}} (rad)");
  fhPhiCharged->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhEtaCharged  = new TH2F
    ("EtaCharged","#eta_{#pi^{#pm}}  vs p_{T #gamma}",
     120,0,120,120,-1,1); 
  fhEtaCharged->SetYTitle("#eta_{#pi^{#pm}} (rad)");
  fhEtaCharged->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhDeltaPhiGammaCharged  = new TH2F
    ("DeltaPhiGammaCharged","#phi_{#gamma} - #phi_{charged #pi} vs p_{T #gamma}",
     200,0,120,200,0,6.4); 
  fhDeltaPhiGammaCharged->SetYTitle("#Delta #phi");
  fhDeltaPhiGammaCharged->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhDeltaEtaGammaCharged  = new TH2F
    ("DeltaEtaGammaCharged","#eta_{#gamma} - #eta_{#pi^{#pm}} vs p_{T #gamma}",
     200,0,120,200,-2,2); 
  fhDeltaEtaGammaCharged->SetYTitle("#Delta #eta");
  fhDeltaEtaGammaCharged->SetXTitle("p_{T #gamma} (GeV/c)");
  
  //---- kHadron  ---- 
  fhCorrelationGammaCharged  = 
    new TH2F("CorrelationGammaCharged","z_{#gamma #pi} = p_{T #pi^{#pm}} / p_{T #gamma}",
	     240,0.,120.,1000,0.,1.2); 
  fhCorrelationGammaCharged->SetYTitle("z_{#gamma #pi}");
  fhCorrelationGammaCharged->SetXTitle("p_{T #gamma}");
  
  //---- kJetLeadCone ----
  fhChargeRatio  = new TH2F
    ("ChargeRatio","p_{T leading charge} /p_{T #gamma} vs p_{T #gamma}",
     120,0,120,120,0,1); 
  fhChargeRatio->SetYTitle("p_{T lead charge} /p_{T #gamma}");
  fhChargeRatio->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhDeltaPhiCharge  = new TH2F
    ("DeltaPhiCharge","#phi_{#gamma} - #phi_{charge} vs p_{T #gamma}",
     200,0,120,200,0,6.4); 
  fhDeltaPhiCharge->SetYTitle("#Delta #phi");
  fhDeltaPhiCharge->SetXTitle("p_{T #gamma} (GeV/c)");
  
  fhDeltaEtaCharge  = new TH2F
    ("DeltaEtaCharge","#eta_{#gamma} - #eta_{charge} vs p_{T #gamma}",
     200,0,120,200,-2,2); 
  fhDeltaEtaCharge->SetYTitle("#Delta #eta");
  fhDeltaEtaCharge->SetXTitle("p_{T #gamma} (GeV/c)");
  
  //
  
  if(!fJetsOnlyInCTS){
    //---- kHadron and kJetLeadCone ----
    fhPhiNeutral  = new TH2F
      ("PhiNeutral","#phi_{#pi^{0}}  vs p_{T #gamma}",
       120,0,120,120,0,7); 
    fhPhiNeutral->SetYTitle("#phi_{#pi^{0}} (rad)");
    fhPhiNeutral->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhEtaNeutral  = new TH2F
      ("EtaNeutral","#eta_{#pi^{0}}  vs p_{T #gamma}",
       120,0,120,120,-1,1); 
    fhEtaNeutral->SetYTitle("#eta_{#pi^{0}} (rad)");
    fhEtaNeutral->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhDeltaPhiGammaNeutral  = new TH2F
      ("DeltaPhiGammaNeutral","#phi_{#gamma} - #phi_{#pi^{0}} vs p_{T #gamma}",
       200,0,120,200,0,6.4); 
    fhDeltaPhiGammaNeutral->SetYTitle("#Delta #phi");
    fhDeltaPhiGammaNeutral->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhDeltaEtaGammaNeutral  = new TH2F
      ("DeltaEtaGammaNeutral","#eta_{#gamma} - #eta_{#pi^{#pm}} vs p_{T #gamma}",
       200,0,120,200,-2,2); 
    fhDeltaEtaGammaNeutral->SetYTitle("#Delta #eta");
    fhDeltaEtaGammaNeutral->SetXTitle("p_{T #gamma} (GeV/c)");
    
    //
    fhAnglePairAccepted  = new TH2F
      ("AnglePairAccepted",
       "Angle between #pi^{0} #gamma pair vs p_{T  #pi^{0}}, both #gamma in eta<0.7, inside window",
       200,0,50,200,0,0.2); 
    fhAnglePairAccepted->SetYTitle("Angle (rad)");
    fhAnglePairAccepted->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    
    fhAnglePairNoCut  = new TH2F
      ("AnglePairNoCut",
       "Angle between all #gamma pair vs p_{T  #pi^{0}}",200,0,50,200,0,0.2); 
    fhAnglePairNoCut->SetYTitle("Angle (rad)");
    fhAnglePairNoCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    
    fhAnglePairAzimuthCut  = new TH2F
      ("AnglePairAzimuthCut",
       "Angle between all #gamma pair that have a good phi and pt vs p_{T  #pi^{0}}",
       200,0,50,200,0,0.2); 
    fhAnglePairAzimuthCut->SetYTitle("Angle (rad)");
    fhAnglePairAzimuthCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    
    fhAnglePairOpeningAngleCut  = new TH2F
      ("AnglePairOpeningAngleCut",
       "Angle between all #gamma pair (opening angle + azimuth cut) vs p_{T  #pi^{0}}"
       ,200,0,50,200,0,0.2); 
    fhAnglePairOpeningAngleCut->SetYTitle("Angle (rad)");
    fhAnglePairOpeningAngleCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    
    fhAnglePairAllCut  = new TH2F
      ("AnglePairAllCut",
       "Angle between all #gamma pair (opening angle + inv mass cut+azimuth) vs p_{T  #pi^{0}}"
       ,200,0,50,200,0,0.2); 
    fhAnglePairAllCut->SetYTitle("Angle (rad)");
    fhAnglePairAllCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");    
    
    //
    fhInvMassPairNoCut  = new TH2F
      ("InvMassPairNoCut","Invariant Mass of all #gamma pair vs p_{T #gamma}",
       120,0,120,360,0,0.5); 
    fhInvMassPairNoCut->SetYTitle("Invariant Mass (GeV/c^{2})");
    fhInvMassPairNoCut->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhInvMassPairAzimuthCut  = new TH2F
      ("InvMassPairAzimuthCut",
       "Invariant Mass of #gamma pair (azimuth cuts) vs p_{T #gamma}",
       120,0,120,360,0,0.5); 
    fhInvMassPairAzimuthCut->SetYTitle("Invariant Mass (GeV/c^{2})");
    fhInvMassPairAzimuthCut->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhInvMassPairOpeningAngleCut  = new TH2F
      ("InvMassPairOpeningAngleCut",
       "Invariant Mass of #gamma pair (angle cut) vs p_{T #gamma}",
       120,0,120,360,0,0.5); 
    fhInvMassPairOpeningAngleCut->SetYTitle("Invariant Mass (GeV/c^{2})");
    fhInvMassPairOpeningAngleCut->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhInvMassPairAllCut  = new TH2F
      ("InvMassPairAllCut",
       "Invariant Mass of #gamma pair (opening angle+invmass cut+azimuth) vs p_{T #gamma}",
       120,0,120,360,0,0.5); 
    fhInvMassPairAllCut->SetYTitle("Invariant Mass (GeV/c^{2})");
    fhInvMassPairAllCut->SetXTitle("p_{T #gamma} (GeV/c)");
    
    //---- kHadron  ---- 
    fhCorrelationGammaNeutral  = 
      new TH2F("CorrelationGammaNeutral","z_{#gamma #pi} = p_{T #pi^{0}} / p_{T #gamma}",
	       240,0.,120.,1000,0.,1.2); 
    fhCorrelationGammaNeutral->SetYTitle("z_{#gamma #pi}");
    fhCorrelationGammaNeutral->SetXTitle("p_{T #gamma}");
    
    //---- kJetLeadCone ----
    fhPi0Ratio  = new TH2F
      ("Pi0Ratio","p_{T leading  #pi^{0}} /p_{T #gamma} vs p_{T #gamma}",
       120,0,120,120,0,1); 
    fhPi0Ratio->SetYTitle("p_{T lead  #pi^{0}} /p_{T #gamma}");
    fhPi0Ratio->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhDeltaPhiPi0  = new TH2F
      ("DeltaPhiPi0","#phi_{#gamma} - #phi_{ #pi^{0}} vs p_{T #gamma}",
       200,0,120,200,0,6.4); 
    fhDeltaPhiPi0->SetYTitle("#Delta #phi");
    fhDeltaPhiPi0->SetXTitle("p_{T #gamma} (GeV/c)");
    
    fhDeltaEtaPi0  = new TH2F
      ("DeltaEtaPi0","#eta_{#gamma} - #eta_{ #pi^{0}} vs p_{T #gamma}",
       200,0,120,200,-2,2); 
    fhDeltaEtaPi0->SetYTitle("#Delta #eta");
    fhDeltaEtaPi0->SetXTitle("p_{T #gamma} (GeV/c)");
    
    //
    fhAnglePairLeading  = new TH2F
      ("AnglePairLeading",
       "Angle between all #gamma pair finally selected vs p_{T  #pi^{0}}",
       200,0,50,200,0,0.2); 
    fhAnglePairLeading->SetYTitle("Angle (rad)");
    fhAnglePairLeading->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    
    fhInvMassPairLeading  = new TH2F
      ("InvMassPairLeading",
       "Invariant Mass of #gamma pair selected vs p_{T #gamma}",
       120,0,120,360,0,0.5); 
    fhInvMassPairLeading->SetYTitle("Invariant Mass (GeV/c^{2})");
    fhInvMassPairLeading->SetXTitle("p_{T #gamma} (GeV/c)");
  }
  
  switch(fAnaType)
    {
    case kParton:
      {
	if(fDataType == kMC){
	  fOutputContainer->Add(fhDeltaPtParton);
	  fOutputContainer->Add(fhPtRatParton);
	  fOutputContainer->Add(fhDeltaEtaParton); 
	  fOutputContainer->Add(fhDeltaPhiParton);
	}
	else
	  AliError("Wrong data type for Parton analysis");

      }// case kParton
      break;
      
    case kJetFinder:
      {
	
	AliInfo("Analysis case not implemented yet");
	
      }// case kJetFinder
      break;
      
    case kHadron:
      {
	fOutputContainer->Add(fhPhiCharged) ;
	fOutputContainer->Add(fhEtaCharged) ;
	fOutputContainer->Add(fhCorrelationGammaCharged) ;
	
	if(!fJetsOnlyInCTS){
	  
	  fOutputContainer->Add(fhPhiNeutral) ;  
	  fOutputContainer->Add(fhEtaNeutral) ;  
	  
	  fOutputContainer->Add(fhDeltaPhiGammaCharged) ; 
	  fOutputContainer->Add(fhDeltaEtaGammaCharged) ; 
	  fOutputContainer->Add(fhDeltaPhiGammaNeutral) ; 
	  fOutputContainer->Add(fhDeltaEtaGammaNeutral) ; 
	  
	  fOutputContainer->Add(fhAnglePairAccepted) ; 
	  fOutputContainer->Add(fhAnglePairNoCut) ; 
	  fOutputContainer->Add(fhAnglePairAzimuthCut) ; 
	  fOutputContainer->Add(fhAnglePairOpeningAngleCut) ;
	  fOutputContainer->Add(fhAnglePairAllCut) ; 
	  
	  fOutputContainer->Add(fhInvMassPairNoCut) ; 
	  fOutputContainer->Add(fhInvMassPairAzimuthCut) ; 
	  fOutputContainer->Add(fhInvMassPairOpeningAngleCut) ; 
	  fOutputContainer->Add(fhInvMassPairAllCut) ; 
	  
	  fOutputContainer->Add(fhCorrelationGammaNeutral) ;
	  
	}
	
      }//  case kHadron
      break;
      
    case kJetLeadCone:
      {
	
	fOutputContainer->Add(fhPhiCharged) ;
	fOutputContainer->Add(fhEtaCharged) ;
	fOutputContainer->Add(fhChargeRatio) ;
	fOutputContainer->Add(fhDeltaPhiCharge) ; 
	fOutputContainer->Add(fhDeltaEtaCharge) ;
	
	fOutputContainer->Add(fhDeltaPhiGammaCharged) ; 
	fOutputContainer->Add(fhDeltaEtaGammaCharged) ; 
	
	if(!fJetsOnlyInCTS){
	  
	  fOutputContainer->Add(fhPhiNeutral) ;  
	  fOutputContainer->Add(fhEtaNeutral) ;  
	  fOutputContainer->Add(fhPi0Ratio) ; 
	  fOutputContainer->Add(fhDeltaPhiPi0) ; 
	  fOutputContainer->Add(fhDeltaEtaPi0) ; 
	  
	  fOutputContainer->Add(fhDeltaPhiGammaNeutral) ; 
	  fOutputContainer->Add(fhDeltaEtaGammaNeutral) ; 
	  
	  fOutputContainer->Add(fhAnglePairAccepted) ; 
	  fOutputContainer->Add(fhAnglePairNoCut) ; 
	  fOutputContainer->Add(fhAnglePairAzimuthCut) ; 
	  fOutputContainer->Add(fhAnglePairOpeningAngleCut) ;
	  fOutputContainer->Add(fhAnglePairAllCut) ; 
	  fOutputContainer->Add(fhAnglePairLeading) ; 
	  
	  fOutputContainer->Add(fhInvMassPairNoCut) ; 
	  fOutputContainer->Add(fhInvMassPairAzimuthCut) ; 
	  fOutputContainer->Add(fhInvMassPairOpeningAngleCut) ; 
	  fOutputContainer->Add(fhInvMassPairAllCut) ; 
	  fOutputContainer->Add(fhInvMassPairLeading) ; 
	  
	}
	
	if(!fSeveralConeAndPtCuts){
	  
	  //Count
	  fhNBkg = new TH1F("NBkg","bkg multiplicity",9000,0,9000); 
	  fhNBkg->SetYTitle("counts");
	  fhNBkg->SetXTitle("N");
	  fOutputContainer->Add(fhNBkg) ; 
	  
	  fhNLeading  = new TH2F
	    ("NLeading","Accepted Jet Leading", 240,0,120,240,0,120); 
	  fhNLeading->SetYTitle("p_{T charge} (GeV/c)");
	  fhNLeading->SetXTitle("p_{T #gamma}(GeV/c)");
	  fOutputContainer->Add(fhNLeading) ; 
	  
	  fhNJet  = new TH1F("NJet","Accepted jets",240,0,120); 
	  fhNJet->SetYTitle("N");
	  fhNJet->SetXTitle("p_{T #gamma}(GeV/c)");
	  fOutputContainer->Add(fhNJet) ; 
	  
	  //Ratios and Pt dist of reconstructed (not selected) jets
	  //Jet
	  fhJetRatio  = new TH2F
	    ("JetRatio","p_{T jet lead}/p_{T #gamma} vs p_{T #gamma}",
	     240,0,120,200,0,10);
	  fhJetRatio->SetYTitle("p_{T jet lead #pi^{0}}/p_{T #gamma}");
	  fhJetRatio->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhJetRatio) ; 
	  
	  fhJetPt  = new TH2F
	    ("JetPt", "p_{T jet lead} vs p_{T #gamma}",240,0,120,400,0,200);
	  fhJetPt->SetYTitle("p_{T jet}");
	  fhJetPt->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhJetPt) ; 
	  
	  //Bkg
	  
	  fhBkgRatio  = new TH2F
	    ("BkgRatio","p_{T bkg lead}/p_{T #gamma} vs p_{T #gamma}",
	     240,0,120,200,0,10);
	  fhBkgRatio->SetYTitle("p_{T bkg lead charge}/p_{T #gamma}");
	  fhBkgRatio->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhBkgRatio) ;
      
	  fhBkgPt  = new TH2F
	    ("BkgPt","p_{T jet lead} vs p_{T #gamma}",240,0,120,400,0,200);
	  fhBkgPt->SetYTitle("p_{T jet lead charge}/p_{T #gamma}");
	  fhBkgPt->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhBkgPt) ;
	  
	  //Jet Distributions
	  
	  fhJetFragment  = 
	    new TH2F("JetFragment","x = p_{T i charged}/p_{T #gamma}",
		     240,0.,120.,1000,0.,1.2); 
	  fhJetFragment->SetYTitle("x_{T}");
	  fhJetFragment->SetXTitle("p_{T #gamma}");
	  fOutputContainer->Add(fhJetFragment) ;
	  
	  fhBkgFragment  = new TH2F
	    ("BkgFragment","x = p_{T i charged}/p_{T #gamma}",
	 240,0.,120.,1000,0.,1.2);
	  fhBkgFragment->SetYTitle("x_{T}");
	  fhBkgFragment->SetXTitle("p_{T #gamma}");
	  fOutputContainer->Add(fhBkgFragment) ;
	  
	  fhJetPtDist  = 
	    new TH2F("JetPtDist","x = p_{T i charged}",240,0.,120.,400,0.,200.); 
	  fhJetPtDist->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhJetPtDist) ;
	  
	  fhBkgPtDist  = new TH2F
	    ("BkgPtDist","x = p_{T i charged}",240,0.,120.,400,0.,200.); 
	  fhBkgPtDist->SetXTitle("p_{T #gamma} (GeV/c)");
	  fOutputContainer->Add(fhBkgPtDist) ;
      
	}
	else{
	  //If we want to study the jet for different cones and pt
	  
	  for(Int_t icone = 0; icone<fJetNCone; icone++){
	    for(Int_t ipt = 0; ipt<fJetNPt;ipt++){ 
	      
	      //Jet
	      
	      fhJetRatios[icone][ipt]  = new TH2F
		("JetRatioCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt], 
		 "p_{T jet lead #pi^{0}}/p_{T #gamma} vs p_{T #gamma}, cone ="
		 +fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+" GeV/c",
		 240,0,120,200,0,10);
	      fhJetRatios[icone][ipt]->
		SetYTitle("p_{T jet lead #pi^{0}}/p_{T #gamma}");
	      fhJetRatios[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhJetRatios[icone][ipt]) ; 
	      
	      
	      fhJetPts[icone][ipt]  = new TH2F
		("JetPtCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt], 
		 "p_{T jet lead #pi^{0}}/p_{T #gamma} vs p_{T #gamma}, cone ="
		 +fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+" GeV/c",
		 240,0,120,400,0,200);
	      fhJetPts[icone][ipt]->
		SetYTitle("p_{T jet lead #pi^{0}}/p_{T #gamma}");
	      fhJetPts[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhJetPts[icone][ipt]) ; 
	      
	      //Bkg
	      fhBkgRatios[icone][ipt]  = new TH2F
		("BkgRatioCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt], 
		 "p_{T bkg lead #pi^{0}}/p_{T #gamma} vs p_{T #gamma}, cone ="
		 +fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+" GeV/c",
		 240,0,120,200,0,10);
	      fhBkgRatios[icone][ipt]->
		SetYTitle("p_{T bkg lead #pi^{0}}/p_{T #gamma}");
	      fhBkgRatios[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhBkgRatios[icone][ipt]) ; 
	      
	      fhBkgPts[icone][ipt]  = new TH2F
		("BkgPtCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt], 
		 "p_{T jet lead #pi^{0}}/p_{T #gamma} vs p_{T #gamma}, cone ="
		 +fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+" GeV/c",
		 240,0,120,400,0,200);
	      fhBkgPts[icone][ipt]->
		SetYTitle("p_{T jet lead #pi^{0}}/p_{T #gamma}");
	      fhBkgPts[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhBkgPts[icone][ipt]) ; 
	      
	      //Counts
	      fhNBkgs[icone][ipt]  = new TH1F
		("NBkgCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "bkg multiplicity cone ="+fJetNameCones[icone]+", pt>" 
		 +fJetNamePtThres[ipt]+" GeV/c",9000,0,9000); 
	      fhNBkgs[icone][ipt]->SetYTitle("counts");
	      fhNBkgs[icone][ipt]->SetXTitle("N");
	      fOutputContainer->Add(fhNBkgs[icone][ipt]) ; 
	      
	      fhNLeadings[icone][ipt]  = new TH2F
		("NLeadingCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "p_{T #gamma} vs p_{T #pi^{0}} cone ="+fJetNameCones[icone]+", pt>" 
		 +fJetNamePtThres[ipt]+" GeV/c",120,0,120,120,0,120); 
	      fhNLeadings[icone][ipt]->SetYTitle("p_{T #pi^{0}}(GeV/c)");
	      fhNLeadings[icone][ipt]->SetXTitle("p_{T #gamma}(GeV/c)");
	      fOutputContainer->Add(fhNLeadings[icone][ipt]) ; 
	      
	      fhNJets[icone][ipt]  = new TH1F
		("NJetCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "Number of neutral jets, cone ="+fJetNameCones[icone]+", pt>" 
		 +fJetNamePtThres[ipt]+" GeV/c",120,0,120); 
	      fhNJets[icone][ipt]->SetYTitle("N");
	      fhNJets[icone][ipt]->SetXTitle("p_{T #gamma}(GeV/c)");
	      fOutputContainer->Add(fhNJets[icone][ipt]) ; 
	      
	      //Fragmentation Function
	      fhJetFragments[icone][ipt]  = new TH2F
		("JetFragmentCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "x_{T} = p_{T i}/p_{T #gamma}, cone ="+fJetNameCones[icone]+", pt>" 
		 +fJetNamePtThres[ipt]+" GeV/c",120,0.,120.,240,0.,1.2); 
	      fhJetFragments[icone][ipt]->SetYTitle("x_{T}");
	      fhJetFragments[icone][ipt]->SetXTitle("p_{T #gamma}");
	      fOutputContainer->Add(fhJetFragments[icone][ipt]) ; 
	      
	      fhBkgFragments[icone][ipt]  = new TH2F
		("BkgFragmentCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "x_{T} = p_{T i}/p_{T #gamma}, cone ="+fJetNameCones[icone]+", pt>" 
		 +fJetNamePtThres[ipt]+" GeV/c",120,0.,120.,240,0.,1.2); 
	      fhBkgFragments[icone][ipt]->SetYTitle("x_{T}");
	      fhBkgFragments[icone][ipt]->SetXTitle("p_{T #gamma}");
	      fOutputContainer->Add(fhBkgFragments[icone][ipt]) ; 
	      
	      //Jet particle distribution
	      
	      fhJetPtDists[icone][ipt]  = new TH2F
		("JetPtDistCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "p_{T i}, cone ="+fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+
		 " GeV/c",120,0.,120.,120,0.,120.); 
	      fhJetPtDists[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhJetPtDists[icone][ipt]) ; 
	      
	      fhBkgPtDists[icone][ipt]  = new TH2F
		("BkgPtDistCone"+fJetNameCones[icone]+"Pt"+fJetNamePtThres[ipt],
		 "p_{T i}, cone ="+fJetNameCones[icone]+", pt>" +fJetNamePtThres[ipt]+
		 " GeV/c",120,0.,120.,120,0.,120.); 
	      fhBkgPtDists[icone][ipt]->SetXTitle("p_{T #gamma} (GeV/c)");
	      fOutputContainer->Add(fhBkgPtDists[icone][ipt]) ; 
	      
	    }//ipt
	  } //icone
	}//If we want to study any cone or pt threshold
	
      }//  case kJetLeadCone
      break;
    }
      
}

//____________________________________________________________________________
void AliAnaGammaCorr::Exec(Option_t *) 
{
  
  switch(fAnaType)
    {
    case kParton:
      {
	if(fDataType == kMC)
	  MakeGammaPartonCorrelationAnalysis();
	else
	  AliError("Wrong data type for kParton analysis");

      }// case kParton:
      break;
      
    case kJetFinder:
      {
	MakeGammaJetFinderCorrelationAnalysis();
      }// case kJetFinder
      break;
      
    case kHadron:
      {
	MakeGammaHadronCorrelationAnalysis();
      }//  case kGammaHadron
      break;
      
    case kJetLeadCone:
      {
	MakeGammaJetCorrelationAnalysis();
      }//  case kGammaJet
      break;
      
    }//switch

  PostData(0, fOutputContainer);
  
  AliDebug(2, "End of analysis");
}    


//____________________________________________________________________________
void AliAnaGammaCorr::InitParameters()
{
 
  //Initialize the parameters of the analysis.
 //-------------kGammaHadron, kGammaJet-----------------
  fJetsOnlyInCTS    = kFALSE ;
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.AddAt(0.4,0);//={0.4,-0.25,0.025,-2e-4};
  fAngleMaxParam.AddAt(-0.25,1) ;
  fAngleMaxParam.AddAt(0.025,2) ;
  fAngleMaxParam.AddAt(-2e-4,3) ;

  fInvMassMaxCut  = 0.16 ;
  fInvMassMinCut  = 0.11 ;

  //-----------kGammaHadron----------------
  fMinPtPion = 0.   ;
  fPhiMaxCut      = 4.5;
  fPhiMinCut      = 1.5 ;

  //------------kGammaJet---------------
  fPbPb                = kFALSE ;
  fInvMassMaxCut  = 0.16 ;
  fInvMassMinCut  = 0.11 ;
  fEtaEMCALCut     = 0.7 ;
  fPhiEMCALCut[0] = 80. *TMath::Pi()/180.;
  fPhiEMCALCut[1] = 190.*TMath::Pi()/180.;
  fPhiLeadingMaxCut      = 3.4 ;
  fPhiLeadingMinCut      = 2.9 ;

  //Jet selection parameters
  //Fixed cut 
  fRatioMaxCut    = 1.0 ;
  fRatioMinCut    = 0.1 ; 
  fJetRatioMaxCut = 1.2 ; 
  fJetRatioMinCut = 0.3 ; 
  fJetsOnlyInCTS = kFALSE ;
  fJetCTSRatioMaxCut = 1.2 ;
  fJetCTSRatioMinCut = 0.3 ;
  fSelect         = 0  ;

  //Cut depending on gamma energy

  fPtJetSelectionCut = 20.; //For Low pt jets+BKG, another limits applied
  //Reconstructed jet energy dependence parameters 
  //e_jet = a1+e_gamma b2. 
  //Index 0-> Pt>2 GeV r = 0.3; Index 1-> Pt>0.5 GeV r = 0.3
  fJetE1[0] = -5.75; fJetE1[1] = -4.1;
  fJetE2[0] = 1.005; fJetE2[1] = 1.05;

  //Reconstructed sigma of jet energy dependence parameters 
  //s_jet = a1+e_gamma b2. 
  //Index 0-> Pt>2 GeV r = 0.3; Index 1-> Pt>0.5 GeV r = 0.3
  fJetSigma1[0] = 2.65;   fJetSigma1[1] = 2.75;
  fJetSigma2[0] = 0.0018; fJetSigma2[1] = 0.033;

  //Background mean energy and RMS
  //Index 0-> No BKG; Index 1-> BKG > 2 GeV; 
  //Index 2-> (low pt jets)BKG > 0.5 GeV;
  //Index > 2, same for CTS conf
  fBkgMean[0] = 0.; fBkgMean[1] = 8.8 ; fBkgMean[2] = 69.5;
  fBkgMean[3] = 0.; fBkgMean[4] = 6.4;  fBkgMean[5] = 48.6;
  fBkgRMS[0]  = 0.; fBkgRMS[1]  = 7.5;  fBkgRMS[2]  = 22.0; 
  fBkgRMS[3]  = 0.; fBkgRMS[4]  = 5.4;  fBkgRMS[5]  = 13.2; 

  //Factor x of min/max = E -+ x * sigma. Obtained after selecting the
  //limits for monoenergetic jets.
  //Index 0-> No BKG; Index 1-> BKG > 2 GeV; 
  //Index 2-> (low pt jets) BKG > 0.5 GeV;
  //Index > 2, same for CTS conf

  fJetXMin1[0] =-0.69 ; fJetXMin1[1] = 0.39 ; fJetXMin1[2] =-0.88 ; 
  fJetXMin1[3] =-2.0  ; fJetXMin1[4] =-0.442 ; fJetXMin1[5] =-1.1  ;
  fJetXMin2[0] = 0.066; fJetXMin2[1] = 0.038; fJetXMin2[2] = 0.034; 
  fJetXMin2[3] = 0.25 ; fJetXMin2[4] = 0.113; fJetXMin2[5] = 0.077 ;
  fJetXMax1[0] =-3.8  ; fJetXMax1[1] =-0.76 ; fJetXMax1[2] =-3.6  ; 
  fJetXMax1[3] =-2.7  ; fJetXMax1[4] =-1.21 ; fJetXMax1[5] =-3.7  ;
  fJetXMax2[0] =-0.012; fJetXMax2[1] =-0.022; fJetXMax2[2] = 0.016; 
  fJetXMax2[3] =-0.024; fJetXMax2[4] =-0.008; fJetXMax2[5] = 0.027;


  //Different cones and pt thresholds to construct the jet

  fJetCone        = 0.3  ;
  fJetPtThreshold = 0.5   ;
  fJetPtThresPbPb = 2.   ;
  fJetNCone       = 4    ;
  fJetNPt         = 4    ;
  fJetCones[0]    = 0.2  ; fJetNameCones[0]   = "02" ;
  fJetCones[1]    = 0.3  ; fJetNameCones[1]   = "03" ;
  fJetCones[2]    = 0.4  ; fJetNameCones[2]   = "04" ;
  fJetCones[2]    = 0.5  ; fJetNameCones[2]   = "05" ;

  fJetPtThres[0]  = 0.0  ; fJetNamePtThres[0] = "00" ;
  fJetPtThres[1]  = 0.5  ; fJetNamePtThres[1] = "05" ;
  fJetPtThres[2]  = 1.0  ; fJetNamePtThres[2] = "10" ;
  fJetPtThres[3]  = 2.0  ; fJetNamePtThres[3] = "20" ;

}


void AliAnaGammaCorr::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  Info("Print", "%s %s", GetName(), GetTitle() ) ;
  printf("Analysis Algorithm           =     %d\n", fAnaType) ;
  switch(fAnaType)
    {

    case kParton:
      {
	
	
      }// case kParton
      break;
      
    case kJetFinder:
      {
	
	
      }// case kParton
      break;
      
    case kHadron:
      {
	printf("pT Hadron       >    %f\n", fMinPtPion) ; 
	printf("Phi Hadron      <     %f\n", fPhiMaxCut) ; 
	printf("Phi Hadron      >     %f\n", fPhiMinCut) ;
	printf("M_pair        <     %f\n", fInvMassMaxCut) ; 
	printf("M_pair        >     %f\n", fInvMassMinCut) ; 
      }//  case kGammaHadron
      break;
      
    case kJetLeadCone:
      {
	printf("Phi Leading      <     %f\n", fPhiLeadingMaxCut) ; 
	printf("Phi Leading      >     %f\n", fPhiLeadingMinCut) ;
	printf("pT Leading / pT Gamma             <     %f\n", fRatioMaxCut) ; 
	printf("pT Leading / pT Gamma             >     %f\n", fRatioMinCut) ;

	if(fSelect == 2){
	  printf("pT Jet / pT Gamma                     <    %f\n", fJetRatioMaxCut) ; 
	  printf("pT Jet / pT Gamma                     >    %f\n", fJetRatioMinCut) ;
	  printf("pT Jet (Only CTS)/ pT Gamma   <    %f\n", fJetCTSRatioMaxCut) ; 
	  printf("pT Jet (Only CTS)/ pT Gamma   >    %f\n", fJetCTSRatioMinCut) ;
	}

	printf("M_pair        <     %f\n", fInvMassMaxCut) ; 
	printf("M_pair        >     %f\n", fInvMassMinCut) ; 

      }//  case kGammaJet
      break;
      
    }//switch
} 

void AliAnaGammaCorr::Terminate(Option_t *)
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
    

}




//__________________________________________________________________________-
Bool_t AliAnaGammaCorr::IsAngleInWindow(const Float_t angle,const Float_t e) {
  //Check if the opening angle of the candidate pairs is inside 
  //our selection windowd

  Bool_t result = kFALSE;
  Double_t mpi0 = 0.1349766;
  Double_t max =  fAngleMaxParam.At(0)*TMath::Exp(fAngleMaxParam.At(1)*e)
    +fAngleMaxParam.At(2)+fAngleMaxParam.At(3)*e;
  Double_t arg = (e*e-2*mpi0*mpi0)/(e*e);
  Double_t min = 100. ;
  if(arg>0.)
    min = TMath::ACos(arg);

  if((angle<max)&&(angle>=min))
    result = kTRUE;
 
  return result;
}

//__________________________________________________________________
void  AliAnaGammaCorr::MakeGammaPartonCorrelationAnalysis() 
{
  //Gamma Hadron Correlation Analysis
 
  if(fDirectGamma && fDirectGamma->Energy() > 0){
    
    AliDebug(1,Form("Prompt Gamma: pt %f, phi %f, eta %f", fDirectGamma->Pt(),fDirectGamma->Phi(),fDirectGamma->Eta())) ;
    
    AliDebug(2, "Make correlation");

    Double_t ptg  = fDirectGamma->Pt();
    Double_t phig = fDirectGamma->Phi();
    Double_t etag = fDirectGamma->Eta();

    Double_t pt    = -100.;
    Double_t eta   = -100.; 
    Double_t phi   = -100. ;
    for(Int_t ipr = 0;ipr < fCaloList->GetEntries() ; ipr ++ ){//fCaloList==parton list
      TParticle * particle = dynamic_cast<TParticle *>(fCaloList->At(ipr)) ;

      if(particle->GetPdgCode() !=22){
	//Only in gamma-jet events
	pt    = particle->Pt();
	phi   = particle->Phi() ;
	eta   = particle->Eta() ;
      }
    }
      
      fhDeltaEtaParton->Fill(ptg,etag-eta);
      fhDeltaPhiParton->Fill(ptg,phig-phi);
      fhDeltaPtParton->Fill(ptg,pt);
      fhPtRatParton->Fill(ptg,pt/ptg);

  }//Gamma in Calo
  
  AliDebug(2, "End of analysis, delete pointers");

} 

//################ HADRON CORRELATION METHODS #######################
//__________________________________________________________________
void  AliAnaGammaCorr::MakeGammaHadronCorrelationAnalysis() 
{
  //Gamma Hadron Correlation Analysis
 
  if(fDirectGamma && fDirectGamma->Energy() > 0){

    AliDebug(1,Form("Prompt Gamma: pt %f, phi %f, eta %f", fDirectGamma->Pt(),fDirectGamma->Phi(),fDirectGamma->Eta())) ;
    
    AliDebug(2, "Make correlation");
    
    //Search correlation 
    MakeGammaChargedCorrelation(fCTSList, fDirectGamma);
    if(!fJetsOnlyInCTS)
      MakeGammaNeutralCorrelation(fCaloList, fDirectGamma);

  }//Gamma in Calo
     
  AliDebug(2, "End of analysis, delete pointers");

} 

//____________________________________________________________________________
void  AliAnaGammaCorr::MakeGammaChargedCorrelation(TClonesArray * pl, TParticle * pGamma)
{  
  //Search for the charged particle with highest with 
  //Phi=Phi_gamma-Pi and pT=0.1E_gamma 
  Double_t ptg  = pGamma->Pt();
  Double_t phig = pGamma->Phi();
  Double_t pt    = -100.;
  Double_t rat   = -100.; 
  Double_t phi   = -100. ;

  for(Int_t ipr = 0;ipr < pl->GetEntries() ; ipr ++ ){
    
    TParticle * particle = dynamic_cast<TParticle *>(pl->At(ipr)) ;

    pt    = particle->Pt();
    rat   = pt/ptg ;
    phi   = particle->Phi() ;
    
    AliDebug(3,Form("pt %f, phi %f, phi gamma %f. Cuts:  delta phi min %f,  max%f, pT min %f",pt,phi,phig,fPhiMinCut,fPhiMaxCut,fMinPtPion));
    
    fhEtaCharged->Fill(ptg,particle->Eta());
    fhPhiCharged->Fill(ptg,phi);
    fhDeltaEtaGammaCharged->Fill(ptg,pGamma->Eta()-particle->Eta());
    fhDeltaPhiGammaCharged->Fill(ptg,phig-phi);
    //Selection within angular and energy limits
    if(((phig-phi)> fPhiMinCut) && ((phig-phi)<fPhiMaxCut) && pt > fMinPtPion){
      AliDebug(2,Form("Selected: pt %f, phi %f",pt,phi));
      fhCorrelationGammaCharged->Fill(ptg,rat);
    } 
  }//particle loop
}


//____________________________________________________________________________
void  AliAnaGammaCorr::MakeGammaNeutralCorrelation(TClonesArray * pl, TParticle * pGamma)  
{  

  //Search for the neutral pion with highest with 
  //Phi=Phi_gamma-Pi and pT=0.1E_gamma 
  Double_t pt = -100.;
  Double_t rat = -100.; 
  Double_t phi = -100. ;
  Double_t ptg  = pGamma->Pt();
  Double_t phig = pGamma->Phi();

  TIter next(pl);
  TParticle * particle = 0;
  
  Int_t iPrimary = -1;
  TLorentzVector gammai,gammaj;
  Double_t angle = 0., e = 0., invmass = 0.;
  Int_t ksPdg = 0;
  Int_t jPrimary=-1;

  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;	  
    ksPdg = particle->GetPdgCode();

    //2 gamma overlapped, found with PID
    if(ksPdg == 111){ 
      pt  = particle->Pt();
      rat = pt/ptg ;
      phi = particle->Phi() ;
      fhEtaNeutral->Fill(ptg,particle->Eta());
      fhPhiNeutral->Fill(ptg,phi);
      fhDeltaEtaGammaNeutral->Fill(ptg,pGamma->Eta()-particle->Eta());
      fhDeltaPhiGammaNeutral->Fill(ptg,phig-phi);
      
      //Selection within angular and energy limits
      if((pt> ptg)&& ((phig-phi)>fPhiMinCut)&&((phig-phi)<fPhiMaxCut)){
	fhCorrelationGammaNeutral ->Fill(ptg,rat);
	AliDebug(2,Form("Selected: pt %f, phi %f",pt,phi));
      }// cuts
    }// pdg = 111

    //Make invariant mass analysis
    else if(ksPdg == 22){
      //Search the photon companion in case it comes from  a Pi0 decay
      //Apply several cuts to select the good pair
      particle->Momentum(gammai);
      jPrimary=-1;
      TIter next2(pl);
      while ( (particle = (TParticle*)next2()) ) {
	jPrimary++;
	if(jPrimary>iPrimary){
	  ksPdg = particle->GetPdgCode();

	  if(ksPdg == 22){
	    particle->Momentum(gammaj);
	    //Info("GetLeadingPi0","Egammai %f, Egammaj %f", 
	    //gammai.Pt(),gammaj.Pt());
	    pt  = (gammai+gammaj).Pt();
	    phi = (gammai+gammaj).Phi();
	    if(phi < 0)
	      phi+=TMath::TwoPi();
	    rat          = pt/ptg ;
	    invmass = (gammai+gammaj).M();
	    angle      = gammaj.Angle(gammai.Vect());
	    e             = (gammai+gammaj).E();
	    fhEtaNeutral->Fill(ptg,(gammai+gammaj).Eta());
	    fhPhiNeutral->Fill(ptg,phi);
	    fhDeltaEtaGammaNeutral->Fill(ptg,pGamma->Eta()-(gammai+gammaj).Eta());
	    fhDeltaPhiGammaNeutral->Fill(ptg,phig-phi);
	    AliDebug(3,Form("No cut: pt %f, phi %f",pt,phi));

	    //Fill histograms with no cuts applied.
	    fhAnglePairNoCut->Fill(e,angle);
	    fhInvMassPairNoCut->Fill(ptg,invmass);

	    //First cut on the energy and azimuth of the pair
	    if((phig-phi) > fPhiMinCut && (phig-phi) < fPhiMaxCut 
	       && pt > fMinPtPion){
	      fhAnglePairAzimuthCut     ->Fill(e,angle);
	      fhInvMassPairAzimuthCut->Fill(ptg,invmass);
	      AliDebug(3,Form("1st cut: pt %f, phi %f",pt,phi));

	      //Second cut on the aperture of the pair
	      if(IsAngleInWindow(angle,e)){
		fhAnglePairOpeningAngleCut     ->Fill(e,angle);
		fhInvMassPairOpeningAngleCut->Fill(ptg,invmass);
		 AliDebug(3,Form("2nd cut: pt %f, phi %f",pt,phi));

		//Third cut on the invariant mass of the pair
		if((invmass>fInvMassMinCut) && (invmass<fInvMassMaxCut)){ 
		  fhInvMassPairAllCut  ->Fill(ptg,invmass);
		  fhAnglePairAllCut       ->Fill(e,angle);
		  //Fill correlation histogram
		  fhCorrelationGammaNeutral ->Fill(ptg,rat);
		  AliDebug(2,Form("Selected: pt %f, phi %f",pt,phi));
		  
		}//(invmass>0.125) && (invmass<0.145)
	      }//Opening angle cut
	    }//Azimuth and pt cut.
	  }//if pdg = 22
	}//iprimary<jprimary
      }//while
    }// if pdg = 22
  }//while
}

//################ JET CORRELATION METHODS #######################

//__________________________________________________________________
void  AliAnaGammaCorr::MakeGammaJetCorrelationAnalysis() 
{
  //Gamma Jet Correlation Analysis

  TParticle *pLeading = new TParticle; //It will contain the kinematics of the found leading particle
  
  AliDebug(2, "Make correlation");
  
  //Search leading particles in CTS and EMCAL 
  if(fDirectGamma && fDirectGamma->Energy()<0 && GetLeadingParticle(fCTSList, fCaloList, fDirectGamma, pLeading)){
    
    AliDebug(1,Form("Leading: pt %f, phi %f, eta %f", pLeading->Pt(),pLeading->Phi(),pLeading->Eta())) ;
    //Search Jet
    if(!fSeveralConeAndPtCuts)
      MakeJet(fCaloList, fCTSList, fDirectGamma,pLeading,"");
    else{
      for(Int_t icone = 0; icone<fJetNCone; icone++) {
	for(Int_t ipt = 0; ipt<fJetNPt;ipt++) {  
	  TString lastname ="Cone"+ fJetNameCones[icone]+"Pt"+ fJetNamePtThres[ipt];
	  fJetCone=fJetCones[icone];
	  fJetPtThreshold=fJetPtThres[ipt];
	  MakeJet(fCaloList, fCTSList, fDirectGamma, pLeading, lastname);
	}//icone
      }//ipt
    }//fSeveralConeAndPtCuts
  }//Leading
  
  AliDebug(2, "End of analysis, delete pointers");
  
  pLeading->Delete();

} 

//____________________________________________________________________________
Bool_t  AliAnaGammaCorr::GetLeadingParticle(TClonesArray * plCTS, TClonesArray * plNe,  
					    TParticle * pGamma, TParticle * pLeading) 
{
  //Search Charged or Neutral leading particle, select the highest one.
  
  TParticle * pLeadingCh = new TParticle();
  TParticle * pLeadingPi0 = new TParticle();
  
  Double_t ptg  =  pGamma->Pt(); 
  Double_t phig = pGamma->Phi(); 
  Double_t etag = pGamma->Eta(); 
  
  if(!fJetsOnlyInCTS)
    {
      AliDebug(3, "GetLeadingPi0");
      GetLeadingPi0   (plNe, pGamma, pLeadingPi0) ;
      AliDebug(3, "GetLeadingCharge");
      GetLeadingCharge(plCTS, pGamma, pLeadingCh) ;
      
      Double_t ptch = pLeadingCh->Pt(); 
      Double_t phich = pLeadingCh->Phi(); 
      Double_t etach = pLeadingCh->Eta(); 
      Double_t ptpi = pLeadingPi0->Pt(); 
      Double_t phipi = pLeadingPi0->Phi(); 
      Double_t etapi = pLeadingPi0->Eta(); 

      //Is leading cone inside EMCAL acceptance?
      
      Bool_t insidech = kFALSE ;
      if((phich - fJetCone) >  fPhiEMCALCut[0] && 
	 (phich + fJetCone) <  fPhiEMCALCut[1] && 
	(etach-fJetCone) < fEtaEMCALCut )
	insidech = kTRUE ;
      
      Bool_t insidepi = kFALSE ;
      if((phipi - fJetCone) >  fPhiEMCALCut[0] && 
	 (phipi + fJetCone) <  fPhiEMCALCut[1] &&
	(etapi-fJetCone) < fEtaEMCALCut )
	insidepi = kTRUE ;

      AliDebug(2,Form("Leading:  charged pt %f, pi0 pt  %f",ptch,ptpi)) ;
      
      if (ptch > 0 || ptpi > 0)
	{
	  if(insidech && (ptch > ptpi))
	    {
	    
	      AliDebug(1,"Leading found in CTS");
	      pLeading->SetMomentum(pLeadingCh->Px(),pLeadingCh->Py(),pLeadingCh->Pz(),pLeadingCh->Energy());
	      AliDebug(3,Form("Final leading found in CTS, pt %f, phi %f, eta %f",ptch,phich,etach)) ;
	      fhChargeRatio->Fill(ptg,ptch/pGamma->Pt());
	      fhDeltaPhiCharge->Fill(ptg,pGamma->Phi()-phich);
	      fhDeltaEtaCharge->Fill(ptg,pGamma->Eta()-etach);
	      return 1;
	    }
	  
	  else if((ptpi > ptch) && insidepi)
	    {
	      AliDebug(1,"Leading found in EMCAL");
	      pLeading->SetMomentum(pLeadingPi0->Px(),pLeadingPi0->Py(),pLeadingPi0->Pz(),pLeadingPi0->Energy());
	      AliDebug(3,Form("Final leading found in EMCAL, pt %f, phi %f, eta %f",ptpi,phipi,etapi)) ;
	      fhPi0Ratio     ->Fill(ptg,ptpi/ptg);
	      fhDeltaPhiPi0->Fill(ptg,phig-phipi);
	      fhDeltaEtaPi0->Fill(ptg,etag-etapi);
	      return 1;	    
	    }

	  else{
	    AliDebug(3,"NO LEADING PARTICLE FOUND");}
	  return 0; 
	}
      else{
	AliDebug(3,"NO LEADING PARTICLE FOUND");
	return 0;
      }
    }
  else
    {
      //No calorimeter present for Leading particle detection
      GetLeadingCharge(plCTS, pGamma, pLeading) ;
      Double_t ptch = pLeading->Pt(); 
      Double_t phich = pLeading->Phi(); 
      Double_t etach = pLeading->Eta(); 
      if(ptch > 0){
	fhChargeRatio->Fill(ptg,ptch/ptg);
	fhDeltaPhiCharge->Fill(ptg,phig-phich);
	fhDeltaEtaCharge->Fill(ptg,etag-etach);
	AliDebug(3,Form("Leading found :  pt %f, phi %f, eta %f",ptch,phich,etach)) ;
	return 1;
      }
      else
	{
	  AliDebug(3,"NO LEADING PARTICLE FOUND");	
	  return 0;
	}
    }
}

//____________________________________________________________________________
void  AliAnaGammaCorr::GetLeadingCharge(TClonesArray * pl, TParticle * pGamma, TParticle * pLeading) const 
{  
  //Search for the charged particle with highest with 
  //Phi=Phi_gamma-Pi and pT=0.1E_gamma 
  Double_t pt  = -100.;
  Double_t phi = -100.;

  for(Int_t ipr = 0;ipr < pl->GetEntries() ; ipr ++ ){

    TParticle * particle = dynamic_cast<TParticle *>(pl->At(ipr)) ;

    Double_t ptl  = particle->Pt();
    Double_t rat  = ptl/pGamma->Pt() ;
    Double_t phil = particle->Phi() ;
    Double_t phig = pGamma->Phi();

    //Selection within angular and energy limits
    if(((phig-phil)> fPhiLeadingMinCut) && ((phig-phil)<fPhiLeadingMaxCut) &&
        (rat > fRatioMinCut) && (rat < fRatioMaxCut)  && (ptl  > pt)) {
       phi = phil ;
       pt  = ptl ;
       pLeading->SetMomentum(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
       AliDebug(4,Form("Charge in CTS: pt %f eta %f phi %f pt/Eg %f \n", pt, particle->Eta(), phi,rat)) ;
     }
  }
  
  AliDebug(1,Form("Leading in CTS: pt %f eta %f phi %f pt/Eg %f \n", pt, pLeading->Eta(), phi,pt/pGamma->Pt())) ;

}


//____________________________________________________________________________
void  AliAnaGammaCorr::GetLeadingPi0(TClonesArray * pl, TParticle * pGamma, TParticle * pLeading)  
{  

  //Search for the neutral pion with highest with 
  //Phi=Phi_gamma-Pi and pT=0.1E_gamma 
  Double_t pt  = -100.;
  Double_t phi = -100.;
  Double_t ptl = -100.;
  Double_t rat = -100.; 
  Double_t phil = -100. ;
  Double_t ptg  = pGamma->Pt();
  Double_t phig = pGamma->Phi();

  TIter next(pl);
  TParticle * particle = 0;
  
  Int_t iPrimary = -1;
  TLorentzVector gammai,gammaj;
  Double_t angle = 0., e = 0., invmass = 0.;
  Double_t anglef = 0., ef = 0., invmassf = 0.;
  Int_t ksPdg = 0;
  Int_t jPrimary=-1;

  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;	  
    
    ksPdg = particle->GetPdgCode();
    ptl  = particle->Pt();
    if(ksPdg == 111){ //2 gamma overlapped, found with PID
      rat = ptl/ptg ;
      phil = particle->Phi() ;
      //Selection within angular and energy limits
      if((ptl> pt)&& (rat > fRatioMinCut) && (rat < fRatioMaxCut) && 
	 ((phig-phil)>fPhiLeadingMinCut)&&((phig-phil)<fPhiLeadingMaxCut)){
	phi = phil ;
	pt  = ptl ;
	pLeading->SetMomentum(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
	AliDebug(4,Form("Pi0 candidate: pt %f eta %f phi %f pt/Eg %f \n",  pLeading->Pt(), pLeading->Eta(),  pLeading->Phi(),  pLeading->Pt()/pGamma->Pt())) ;
      }// cuts
    }// pdg = 111
    else if(ksPdg == 22){//1 gamma
      //Search the photon companion in case it comes from  a Pi0 decay
      //Apply several cuts to select the good pair
      particle->Momentum(gammai);
      jPrimary=-1;
      TIter next2(pl);
      while ( (particle = (TParticle*)next2()) ) {
	jPrimary++;
	if(jPrimary>iPrimary){
	  ksPdg = particle->GetPdgCode();

	  if(ksPdg == 22){
	    particle->Momentum(gammaj);
	    //Info("GetLeadingPi0","Egammai %f, Egammaj %f", 
	    //gammai.Pt(),gammaj.Pt());
	    ptl  = (gammai+gammaj).Pt();
	    phil = (gammai+gammaj).Phi();
	    if(phil < 0)
	      phil+=TMath::TwoPi();
	    rat = ptl/ptg ;
	    invmass = (gammai+gammaj).M();
	    angle = gammaj.Angle(gammai.Vect());
	    //Info("GetLeadingPi0","Angle %f", angle);
	    e = (gammai+gammaj).E();
	    //Fill histograms with no cuts applied.
	    fhAnglePairNoCut->Fill(e,angle);
	    fhInvMassPairNoCut->Fill(ptg,invmass);
	    //First cut on the energy and azimuth of the pair
	    if((rat > fRatioMinCut) && (rat < fRatioMaxCut) && 
	       ((phig-phil)>fPhiMinCut)&&((phig-phil)<fPhiMaxCut)){
	      
	      fhAnglePairAzimuthCut->Fill(e,angle);
	      fhInvMassPairAzimuthCut->Fill(ptg,invmass);
	      //Second cut on the aperture of the pair
	      if(IsAngleInWindow(angle,e)){
		fhAnglePairOpeningAngleCut->Fill(e,angle);
		fhInvMassPairOpeningAngleCut->Fill(ptg,invmass);
		
		//Info("GetLeadingPi0","InvMass %f", invmass);
		//Third cut on the invariant mass of the pair
		if((invmass>fInvMassMinCut) && (invmass<fInvMassMaxCut)){ 
		  fhInvMassPairAllCut->Fill(ptg,invmass);
		  fhAnglePairAllCut->Fill(e,angle);
		  if(ptl > pt ){
		    pt       = ptl;
		    phi      = phil ;
		    ef       = e ;
		    anglef   = angle ;
		    invmassf = invmass ;
		    pLeading->SetMomentum(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
		    AliDebug(4,Form("Pi0 candidate: pt %f eta %f phi %f pt/Eg %f \n",  pLeading->Pt(), pLeading->Eta(),  pLeading->Phi(),  pLeading->Pt()/pGamma->Pt())) ;
		  }
		}//cuts
	      }//(invmass>0.125) && (invmass<0.145)
	    }//gammaj.Angle(gammai.Vect())<0.04
	  }//if pdg = 22
	}//iprimary<jprimary
      }//while
    }// if pdg = 22
    //     }
  }//while
  
  if(ef > 0.){//Final pi0 found, highest pair energy, fill histograms
    fhInvMassPairLeading->Fill(ptg,invmassf);
    fhAnglePairLeading->Fill(ef,anglef);
  }
  
  AliDebug(3,Form("Leading EMCAL: pt %f eta %f phi %f pt/Eg %f \n",  pLeading->Pt(), pLeading->Eta(),  pLeading->Phi(),  pLeading->Pt()/pGamma->Pt())) ;
}


//__________________________________________________________________________-
Bool_t AliAnaGammaCorr::IsJetSelected(const Double_t ptg, const Double_t ptj){
  //Check if the energy of the reconstructed jet is within an energy window

  Double_t par[6];
  Double_t xmax[2];
  Double_t xmin[2];

  Int_t iCTS = 0;
  if(fJetsOnlyInCTS)
    iCTS = 3 ;

  if(!fPbPb){
    //Phythia alone, jets with pt_th > 0.2, r = 0.3 
    par[0] = fJetE1[0]; par[1] = fJetE2[0]; 
    //Energy of the jet peak
    //e_jet = fJetE1[0]+fJetE2[0]*e_gamma, simulation fit
    par[2] = fJetSigma1[0]; par[3] = fJetSigma2[0];
    //Sigma  of the jet peak
    //sigma_jet = fJetSigma1[0]+fJetSigma2[0]*e_gamma, simulation fit
    par[4] = fBkgMean[0 + iCTS]; par[5] = fBkgRMS[0 + iCTS];
    //Parameters reserved for PbPb bkg.
    xmax[0] = fJetXMax1[0 + iCTS]; xmax[1] = fJetXMax2[0 + iCTS];
    xmin[0] = fJetXMin1[0 + iCTS]; xmin[1] = fJetXMin2[0 + iCTS];
    //Factor that multiplies sigma to obtain the best limits, 
    //by observation, of mono jet ratios (ptjet/ptg)
    //X_jet = fJetX1[0]+fJetX2[0]*e_gamma
   
  }
  else{
    if(ptg > fPtJetSelectionCut){
      //Phythia +PbPb with  pt_th > 2 GeV/c, r = 0.3 
      par[0] = fJetE1[0]; par[1] = fJetE2[0]; 
      //Energy of the jet peak, same as in pp
      //e_jet = fJetE1[0]+fJetE2[0]*e_gamma, simulation fit
      par[2] = fJetSigma1[0]; par[3] = fJetSigma2[0];
      //Sigma  of the jet peak, same as in pp
      //sigma_jet = fJetSigma1[0]+fJetSigma2[0]*e_gamma, simulation fit
      par[4] = fBkgMean[1 + iCTS]; par[5] = fBkgRMS[1 + iCTS];
      //Mean value and RMS of PbPb Bkg 
      xmax[0] = fJetXMax1[1 + iCTS]; xmax[1] = fJetXMax2[1 + iCTS];
      xmin[0] = fJetXMin1[1 + iCTS]; xmin[1] = fJetXMin2[1 + iCTS];
      //Factor that multiplies sigma to obtain the best limits, 
      //by observation, of mono jet ratios (ptjet/ptg) mixed with PbPb Bkg, 
      //pt_th > 2 GeV, r = 0.3
      //X_jet = fJetX1[0]+fJetX2[0]*e_gamma
     
    }
    else{
      //Phythia + PbPb with  pt_th > 0.5 GeV/c, r = 0.3
      par[0] = fJetE1[1]; par[1] = fJetE2[1]; 
      //Energy of the jet peak, pt_th > 2 GeV/c, r = 0.3 
      //e_jet = fJetE1[0]+fJetE2[0]*e_gamma, simulation fit
      par[2] = fJetSigma1[1]; par[3] = fJetSigma2[1];
      //Sigma  of the jet peak, pt_th > 2 GeV/c, r = 0.3
      //sigma_jet = fJetSigma1[0]+fJetSigma2[0]*e_gamma, simulation fit
      par[4] = fBkgMean[2 + iCTS]; par[5] = fBkgRMS[2 + iCTS];
      //Mean value and RMS of PbPb Bkg in a 0.3 cone, pt > 2 GeV.
      xmax[0] = fJetXMax1[2 + iCTS]; xmax[1] = fJetXMax2[2 + iCTS];
      xmin[0] = fJetXMin1[2 + iCTS]; xmin[1] = fJetXMin2[2 + iCTS];
      //Factor that multiplies sigma to obtain the best limits, 
      //by observation, of mono jet ratios (ptjet/ptg) mixed with PbPb Bkg, 
      //pt_th > 2 GeV, r = 0.3
      //X_jet = fJetX1[0]+fJetX2[0]*e_gamma
     
    }//If low pt jet in bkg
  }//if Bkg

 //Calculate minimum and maximum limits of the jet ratio.
  Double_t min = CalculateJetRatioLimit(ptg, par, xmin);
  Double_t max = CalculateJetRatioLimit(ptg, par, xmax);
  
  AliDebug(3,Form("Jet selection?  : Limits min %f, max %f,  pt_jet %f,  pt_gamma %f, pt_jet / pt_gamma %f",min,max,ptj,ptg,ptj/ptg));

  if(( min < ptj/ptg ) && ( max > ptj/ptg))
    return kTRUE;
  else
    return kFALSE;

}

//____________________________________________________________________________
void AliAnaGammaCorr::MakeJet(TClonesArray * plCalo, TClonesArray * plCTS, TParticle * pGamma, TParticle* pLeading,TString lastname)
{
  //Fill the jet with the particles around the leading particle with 
  //R=fJetCone and pt_th = fJetPtThres. Calculate the energy of the jet and 
  //check if we select it. Fill jet histograms
  
  TClonesArray * jetList = new TClonesArray("TParticle",1000);
  TClonesArray * bkgList = new TClonesArray("TParticle",1000);

  TLorentzVector jet   (0,0,0,0);  
  TLorentzVector bkg(0,0,0,0);
  TLorentzVector lv (0,0,0,0);

  Double_t ptjet = 0.0;
  Double_t ptbkg = 0.0;
  Int_t n0 = 0;
  Int_t n1 = 0;  
  Bool_t b1 = kFALSE;
  Bool_t b0 = kFALSE;
  
  Double_t ptg  = pGamma->Pt();
  Double_t phig = pGamma->Phi();
  Double_t ptl  = pLeading->Pt();
  Double_t phil = pLeading->Phi();
  Double_t etal = pLeading->Eta();

  Float_t ptcut = fJetPtThreshold;
  if(fPbPb && !fSeveralConeAndPtCuts && ptg > fPtJetSelectionCut)  ptcut = fJetPtThresPbPb ;
  
  TIter nextch(plCTS) ; 
  TParticle * particle = 0 ; 
  while ( (particle = dynamic_cast<TParticle*>(nextch())) ) {
    
    b0 = kFALSE;
    b1 = kFALSE;

    //Particles in jet 
    SetJet(particle, b0, fJetCone, etal, phil) ;  

    if(b0){
      new((*jetList)[n0++]) TParticle(*particle) ;
      particle->Momentum(lv);
      if(particle->Pt() > ptcut ){
	jet+=lv;
	ptjet+=particle->Pt();
      }
    }

    //Background around (phi_gamma-pi, eta_leading)
    SetJet(particle, b1, fJetCone,etal, phig) ;

    if(b1) { 
      new((*bkgList)[n1++]) TParticle(*particle) ;
      particle->Momentum(lv);
      if(particle->Pt() > ptcut ){
	bkg+=lv;
	ptbkg+=particle->Pt();    
      }  
    }
  }
  
  TIter nextne(plCalo) ; 
  particle = 0 ; 
  while ( (particle = dynamic_cast<TParticle*>(nextne())) ) {
    
    b0 = kFALSE;
    b1 = kFALSE;

    //Particles in jet 
    SetJet(particle, b0, fJetCone, etal, phil) ;  

    if(b0){
      new((*jetList)[n0++]) TParticle(*particle) ;
      particle->Momentum(lv);
      if(particle->Pt() > ptcut ){
	jet+=lv;
	ptjet+=particle->Pt();
      }
    }

    //Background around (phi_gamma-pi, eta_leading)
    SetJet(particle, b1, fJetCone,etal, phig) ;

    if(b1) { 
      new((*bkgList)[n1++]) TParticle(*particle) ;
      particle->Momentum(lv);
      if(particle->Pt() > ptcut ){
	bkg+=lv;
	ptbkg+=particle->Pt();    
      }  
    }
  }


  ptjet = jet.Pt();
  ptbkg = bkg.Pt();

  if(ptjet > 0.) {

    AliDebug(2,Form("Gamma   pt %f, Jet pt %f, Bkg pt %f",ptg,ptjet,ptbkg));
    
    //Fill histograms
    
    Double_t ratjet   = ptjet/ptg ;
    Double_t ratbkg  = ptbkg/ptg ;
    
    dynamic_cast<TH2F*>
      (fOutputContainer->FindObject("JetRatio"+lastname))
      ->Fill(ptg,ratjet);	 
    dynamic_cast<TH2F*>
      (fOutputContainer->FindObject("JetPt"+lastname))
      ->Fill(ptg,ptjet);
    
    dynamic_cast<TH2F*>
      (fOutputContainer->FindObject("BkgRatio"+lastname))
      ->Fill(ptg,ratbkg);
    
    dynamic_cast<TH2F*>
      (fOutputContainer->FindObject("BkgPt"+lastname))
      ->Fill(ptg,ptbkg);


    //Jet selection
    Bool_t kSelect = kFALSE;
    if(fSelect == 0)
      kSelect = kTRUE; //Accept all jets, no restriction
    else if(fSelect == 1){
      //Selection with parametrized cuts
      if(IsJetSelected(ptg,ptjet))   kSelect = kTRUE;
    }
    else if(fSelect == 2){
      //Simple selection
      if(!fJetsOnlyInCTS){
	if((ratjet <  fJetRatioMaxCut) && (ratjet > fJetRatioMinCut )) kSelect = kTRUE;
      }
      else{
	if((ratjet <  fJetCTSRatioMaxCut) && (ratjet > fJetCTSRatioMinCut )) kSelect = kTRUE;
      }
    }
    else
      AliError("Jet selection option larger than 2, DONT SELECT JETS");
    
    
    if(kSelect){
      AliDebug(1,Form("Jet Selected: pt %f ", ptjet)) ;
      
      FillJetHistos(jetList, ptg, ptl,"Jet",lastname);
      FillJetHistos(bkgList, ptg, ptl, "Bkg",lastname);
    }
  } //ptjet > 0
  
  jetList ->Delete();
  bkgList ->Delete();
  
}

//___________________________________________________________________
void AliAnaGammaCorr::SetJet(TParticle * part, Bool_t & b, Float_t cone, 
			     Double_t eta, Double_t phi)
{

  //Check if the particle is inside the cone defined by the leading particle
  b = kFALSE;
  
  if(phi > TMath::TwoPi())
    phi-=TMath::TwoPi();
  if(phi < 0.)
    phi+=TMath::TwoPi();
  
  Double_t  rad = 10000 + cone;
  
  if(TMath::Abs(part->Phi()-phi) <= (TMath::TwoPi() - cone))
    rad = TMath::Sqrt(TMath::Power(part->Eta()-eta,2)+
		      TMath::Power(part->Phi()-phi,2));
  else{
    if(part->Phi()-phi > TMath::TwoPi() - cone)
      rad = TMath::Sqrt(TMath::Power(part->Eta()-eta,2)+
			TMath::Power((part->Phi()-TMath::TwoPi())-phi,2));
    if(part->Phi()-phi < -(TMath::TwoPi() - cone))
      rad = TMath::Sqrt(TMath::Power(part->Eta()-eta,2)+
			TMath::Power((part->Phi()+TMath::TwoPi())-phi,2));
  }

  if(rad < cone )
    b = kTRUE;
  
}

//____________________________________________________________________________
void AliAnaGammaCorr::FillJetHistos(TClonesArray * pl, Double_t ptg, Double_t ptl, TString type, TString lastname)
{
  //Fill histograms wth jet fragmentation 
  //and number of selected jets and leading particles
  //and the background multiplicity
  TParticle * particle = 0 ;
  Int_t ipr = 0;
  Float_t  charge = 0;

  TIter next(pl) ; 
  while ( (particle = dynamic_cast<TParticle*>(next())) ) {
    ipr++ ;
    Double_t pt = particle->Pt();

    charge = TDatabasePDG::Instance()
      ->GetParticle(particle->GetPdgCode())->Charge();
    if(charge != 0){//Only jet Charged particles 
      dynamic_cast<TH2F*>
	(fOutputContainer->FindObject(type+"Fragment"+lastname))
	->Fill(ptg,pt/ptg);
      dynamic_cast<TH2F*>
	(fOutputContainer->FindObject(type+"PtDist"+lastname))
	->Fill(ptg,pt);
    }//charged

  }//while

  if(type == "Bkg")
    dynamic_cast<TH1F*>
      (fOutputContainer->FindObject("NBkg"+lastname))
      ->Fill(ipr);
  else{
    dynamic_cast<TH1F*>
      (fOutputContainer->FindObject("NJet"+lastname))->
      Fill(ptg);
    dynamic_cast<TH2F*>
      (fOutputContainer->FindObject("NLeading"+lastname))
      ->Fill(ptg,ptl);
  }
  
}

//__________________________________________________________________
void  AliAnaGammaCorr::MakeGammaJetFinderCorrelationAnalysis() 
{
  //Gamma Jet, jet reconstructed with standard algorithm, Correlation Analysis
  AliError("Nothing implemented yet");
}

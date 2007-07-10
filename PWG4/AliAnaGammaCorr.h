#ifndef ALIANAGAMMACORR_H
#define ALIANAGAMMACORR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 *
 *
 */

//_________________________________________________________________________
// Class for the analysis of gamma correlations (gamma-jet, 
// gamma-hadron, input is an isolated photon 
//
//  Class created from old AliPHOSGammaJet
//  (see AliRoot versions previous Release 4-09)
//
//*-- Author: Gustavo Conesa (INFN-LNF)
/////////////////////////////////////////////////////////////////////////

// --- ROOT system ---
#include <TParticle.h> 
#include <TClonesArray.h> 
#include <TTree.h> 
#include "AliAnalysisTask.h" 
#include <TH2F.h>
 
class AliAnaGammaCorr : public AliAnalysisTask {

public: 

  AliAnaGammaCorr(const char *name) ; // default ctor
  AliAnaGammaCorr(const AliAnaGammaCorr & g) ; // cpy ctor
  AliAnaGammaCorr & operator = (const AliAnaGammaCorr & g) ;//cpy assignment
  virtual ~AliAnaGammaCorr() ; //virtual dtor

  enum anatype_t  {kParton, kJetFinder, kJetLeadCone, kHadron};
  enum datatype_t {kData, kMC, kMCData};

  virtual void Exec(Option_t * opt = "") ;
  virtual void ConnectInputData(Option_t *);
  virtual void CreateOutputObjects();
  virtual void Terminate(Option_t * opt = "");
  
  void InitParameters();
  
  Int_t GetAnalysisType(){  return fAnaType ; }
  void SetAnalysisType(Int_t ana ){  fAnaType = ana ; }

  Int_t GetDataType(){  return fDataType ; }
  void SetDataType(Int_t dat ){  fDataType = dat ; }
  
  void Print(const Option_t * opt)const;

  //Gamma-Parton
  void MakeGammaPartonCorrelationAnalysis();
  //Gamma-Parton
  void MakeGammaJetFinderCorrelationAnalysis();

  //Gamma correlations methods kHadron, kJetLeadCone
  Bool_t     AreJetOnlyInCTS() const {return fJetsOnlyInCTS ; } 
  void SetJetsOnlyInCTS(Bool_t opt){fJetsOnlyInCTS = opt; }
  Double_t GetAngleMaxParam(Int_t i) const {return fAngleMaxParam.At(i) ; }
  void SetAngleMaxParam(Int_t i, Double_t par){fAngleMaxParam.AddAt(par,i) ; }
  Double_t GetInvMassMaxCut() const {return fInvMassMaxCut ; }
  Double_t GetInvMassMinCut() const {return fInvMassMinCut ; }
  void SetInvMassCutRange(Double_t invmassmin, Double_t invmassmax)
    {fInvMassMaxCut =invmassmax;  fInvMassMinCut =invmassmin;}	

  Bool_t IsAngleInWindow(const Float_t angle, const Float_t e);

  //Gamma hadron correlations methods kHadron
  Float_t    GetMinPtPion() const {return fMinPtPion ; }
  void SetMinPtPion(Float_t pt){fMinPtPion = pt; };
  
  Double_t GetPhiMaxCut() const {return fPhiMaxCut ; }
  Double_t GetPhiMinCut() const {return fPhiMinCut ; }
  void SetPhiCutRange(Double_t phimin, Double_t phimax)
  {fPhiMaxCut =phimax;  fPhiMinCut =phimin;}

  private:
  void MakeGammaHadronCorrelationAnalysis();
  void MakeGammaChargedCorrelation(TClonesArray * pl, TParticle *pGamma) ;
  void MakeGammaNeutralCorrelation(TClonesArray * pl, TParticle *pGamma)  ;

  //Gamma jet correlations  methods kJetLeadCone
  public: 
  Bool_t   AreSeveralConeAndPtCuts() const {return fSeveralConeAndPtCuts ; }
  void SetSeveralConeAndPtCuts(Bool_t several){fSeveralConeAndPtCuts = several ;}
  Bool_t   IsPbPb() const {return fPbPb ; }
  void SetPbPb(Bool_t opt){fPbPb = opt; }

  Double_t GetEtaEMCALCut() const {return fEtaEMCALCut;}
  Double_t GetPhiEMCALCut(Int_t i) const {return fPhiEMCALCut[i];}
  void SetEtaEMCALCut(Double_t etacut) {fEtaEMCALCut = etacut ; }
  void SetPhiEMCALCut(Double_t phi, Int_t i){fPhiEMCALCut[i] = phi; }

  Double_t GetPhiLeadingMaxCut() const {return fPhiLeadingMaxCut ; }
  Double_t GetPhiLeadingMinCut() const {return fPhiLeadingMinCut ; }
  void SetPhiLeadingCutRange(Double_t phimin, Double_t phimax)
  {fPhiLeadingMaxCut =phimax;  fPhiLeadingMinCut =phimin;}

  Double_t GetPtJetSelectionCut() const {return fPtJetSelectionCut ; }
  Double_t GetJetRatioMaxCut() const {return fJetRatioMaxCut ; }
  Double_t GetJetRatioMinCut() const {return fJetRatioMinCut ; }
  Double_t GetRatioMaxCut() const {return fRatioMaxCut ; }
  Double_t GetRatioMinCut() const {return fRatioMinCut ; }
  void SetPtJetSelectionCut(Double_t cut){fPtJetSelectionCut = cut; }
  void SetJetSelection(UInt_t select){ fSelect= select ; }
  void SetRatioCutRange(Double_t ratiomin, Double_t ratiomax)
  {fRatioMaxCut = ratiomax;  fRatioMinCut = ratiomin;}

  Int_t       GetJetNCones() const {return fJetNCone ; }
  Int_t       GetJetNPtThres() const {return fJetNPt ; }
  Float_t    GetJetCone() const {return fJetCone ; }
  Float_t    GetJetPtThreshold() const {return fJetPtThreshold ; }
  Float_t    GetJetPtThresPbPb() const {return fJetPtThresPbPb ; }
  Float_t    GetJetCones(Int_t i) const {return fJetCones[i] ; }
  Float_t    GetJetPtThreshold(Int_t i) const {return fJetPtThres[i] ; }
  TString   GetJetConeName(Int_t i) const {return fJetNameCones[i] ; }
  TString   GetJetPtThresName(Int_t i) const {return fJetNamePtThres[i] ; }

  void SetJetNCones(Int_t n){fJetNCone = n ; }
  void SetJetNPtThresholds(Int_t n){fJetNPt = n ; }
  void SetJetCones(Int_t i, Float_t cone, TString sc)
    {fJetCones[i] = cone ; fJetNameCones[i] = sc; };
  void SetCone(Float_t cone)
    {fJetCone = cone; }
  void SetJetPtThreshold(Float_t pt){fJetPtThreshold = pt; };
  void SetJetPtThresPbPb(Float_t pt){fJetPtThresPbPb = pt; };
  void SetJetPtThresholds(Int_t i,Float_t pt, TString spt){fJetPtThres[i] = pt ; 
  fJetNamePtThres[i] = spt; };

  void SetJetRatioCutRange(Double_t ratiomin, Double_t ratiomax)
    {fJetRatioMaxCut =ratiomax;  fJetRatioMinCut = ratiomin ; }
  void SetJetCTSRatioCutRange(Double_t ratiomin, Double_t ratiomax)
    {fJetCTSRatioMaxCut =ratiomax;  fJetCTSRatioMinCut = ratiomin ; }

  private:

  Double_t CalculateJetRatioLimit(const Double_t ptg, const Double_t *param, 
				  const Double_t *x);
  void FillJetHistos(TClonesArray * pl, Double_t ptg, Double_t ptl,TString type, TString lastname);

  Bool_t IsJetSelected(const Double_t ptg, const Double_t ptjet);

  void MakeJet(TClonesArray * plCTS, TClonesArray * plCalo, TParticle * pGamma, TParticle* pLeading, TString lastname); 
  void MakeGammaJetCorrelationAnalysis() ;

  void GetLeadingCharge(TClonesArray * pl, TParticle *pGamma, TParticle * pLeading) const ;
  void GetLeadingPi0   (TClonesArray * pl, TParticle *pGamma, TParticle * pLeading)  ;
  Bool_t GetLeadingParticle(TClonesArray * plCTS, TClonesArray * plNe, 
			    TParticle *pGamma, TParticle * pLeading) ;
 
  void SetJet(TParticle * part, Bool_t & b, Float_t cone, Double_t eta, 
	      Double_t phi);

  
  private:
  
  TParticle * fDirectGamma; //! output corr photon 
  TClonesArray * fCaloList; //!input clusters
  TClonesArray * fCTSList; //!input tracks
  TList  *fOutputContainer ; //! output data container with histograms
  Int_t fAnaType ;  //Type of analysis
  Int_t fDataType ; //Type of data

  //kPartons
  TH2F * fhDeltaEtaParton;
  TH2F * fhDeltaPhiParton;
  TH2F * fhDeltaPtParton;
  TH2F * fhPtRatParton;

  //Gamma correlations data members kHadron, kJetConeLead
  Bool_t       fJetsOnlyInCTS ;    // Jets measured only in TPC+ITS.
  
  Double_t   fInvMassMaxCut ;  // Invariant Mass cut maximum
  Double_t   fInvMassMinCut ;  // Invariant Masscut minimun
  TArrayD    fAngleMaxParam ; //Max opening angle selection parameters
  //Histograms 
  TH2F * fhPhiCharged  ; 
  TH2F * fhPhiNeutral   ; 
  TH2F * fhEtaCharged  ; 
  TH2F * fhEtaNeutral   ; 
  TH2F * fhDeltaPhiGammaCharged  ;  
  TH2F * fhDeltaPhiGammaNeutral   ; 
  TH2F * fhDeltaEtaGammaCharged  ; 
  TH2F * fhDeltaEtaGammaNeutral  ; 

  TH2F * fhAnglePairAccepted  ; 
  TH2F * fhAnglePairNoCut  ; 
  TH2F * fhAnglePairAzimuthCut  ; 
  TH2F * fhAnglePairOpeningAngleCut   ; 
  TH2F * fhAnglePairAllCut   ; 
  TH2F * fhInvMassPairNoCut    ; 
  TH2F * fhInvMassPairAzimuthCut  ; 
  TH2F * fhInvMassPairOpeningAngleCut  ; 
  TH2F * fhInvMassPairAllCut   ; 

  //Gamma hadron correlations data members kHadron
  Double_t   fMinPtPion;       // Minimum pt of pion
  Double_t   fPhiMaxCut ;      // 
  Double_t   fPhiMinCut ;      //  
  //Histograms
  TH2F * fhCorrelationGammaNeutral  ; 
  TH2F * fhCorrelationGammaCharged  ; 

  //Gamma jet correlations  data members kJetConeLead

  Bool_t       fPbPb;          // PbPb event
  Bool_t       fSeveralConeAndPtCuts;     //  To play with the jet cone size and pt th.

  Double_t   fEtaEMCALCut ;         // Eta EMCAL acceptance
  Double_t   fPhiEMCALCut[2] ; // Phi cut maximum
  Double_t   fPhiLeadingMaxCut ;      // Phi EMCAL maximum acceptance
  Double_t   fPhiLeadingMinCut ;      // Phi EMCAL minimum acceptance
  Double_t   fRatioMaxCut ;    // Leading particle/gamma Ratio cut maximum
  Double_t   fRatioMinCut ;    // Leading particle/gamma Ratio cut minimum

  //Jet selection parameters
  //Fixed cuts (old)
  Double_t   fJetCTSRatioMaxCut ; // Leading particle/gamma Ratio cut maximum
  Double_t   fJetCTSRatioMinCut ; // Leading particle/gamma Ratio cut minimum
  Double_t   fJetRatioMaxCut ; // Jet/gamma Ratio cut maximum
  Double_t   fJetRatioMinCut ; // Jet/gamma Ratio cut minimum

  //Cuts depending on jet pt
  Double_t fJetE1[2];    //Rec. jet energy parameters
  Double_t fJetE2[2];    //Rec. jet energy parameters
  Double_t fJetSigma1[2];//Rec. sigma of jet energy  parameters
  Double_t fJetSigma2[2];//Rec. sigma of jet energy  parameters
  Double_t fBkgMean[6];  //Background mean energy 
  Double_t fBkgRMS[6];   //Background RMS
  Double_t fJetXMin1[6]; //X Factor to set jet min limit for pp
  Double_t fJetXMin2[6]; //X Factor to set jet min limit for PbPb
  Double_t fJetXMax1[6]; //X Factor to set jet max limit for pp
  Double_t fJetXMax2[6]; //X Factor to set jet max limit for PbPb

  Int_t         fJetNCone ;            // Number of jet cones sizes
  Int_t         fJetNPt   ;            // Number of jet particle pT threshold
  Double_t   fJetCone  ;            // Jet cone sizes under study (!fSeveralConeAndPtCuts)
  Double_t   fJetCones[10];         // Jet cone sizes under study (fSeveralConeAndPtCuts)
  TString     fJetNameCones[10];     // String name of cone to append to histos
  Double_t   fJetPtThreshold;       // Jet pT threshold under study(!fSeveralConeAndPtCuts)
  Double_t   fJetPtThresPbPb;       // Jet pT threshold under study(!fSeveralConeAndPtCuts)
  Double_t   fJetPtThres[10];       // Jet pT threshold under study(fSeveralConeAndPtCuts)
  TString     fJetNamePtThres[10];   // String name of pt th to append to histos
  Double_t   fPtJetSelectionCut; // Jet pt to change to low pt jets analysis
  UInt_t       fSelect  ;   //kTRUE: Selects all jets, no limits.
 
  //Histograms
 
  TH2F * fhAnglePairLeading  ; 
  TH2F * fhInvMassPairLeading  ; 
  TH2F * fhChargeRatio  ; 
  TH2F * fhPi0Ratio   ; 
  TH2F * fhDeltaPhiCharge  ;  
  TH2F * fhDeltaPhiPi0   ; 
  TH2F * fhDeltaEtaCharge  ; 
  TH2F * fhDeltaEtaPi0  ; 
  TH1F * fhNBkg   ; 
  TH2F * fhNLeading  ; 
  TH1F * fhNJet  ; 
  TH2F * fhJetRatio  ; 
  TH2F * fhJetPt   ; 
  TH2F * fhBkgRatio   ; 
  TH2F * fhBkgPt  ; 
  TH2F * fhJetFragment  ; 
  TH2F * fhBkgFragment  ; 
  TH2F * fhJetPtDist  ; 
  TH2F * fhBkgPtDist  ; 

  TH2F * fhJetRatios[5][5];
  TH2F * fhJetPts[5][5];
  TH2F * fhBkgRatios[5][5];  
  TH2F * fhBkgPts[5][5];
  
  TH2F * fhNLeadings[5][5];
  TH1F * fhNJets[5][5];
  TH1F * fhNBkgs[5][5];
  
  TH2F * fhJetFragments[5][5];
  TH2F * fhBkgFragments[5][5];
  TH2F * fhJetPtDists[5][5];
  TH2F * fhBkgPtDists[5][5];
 
  ClassDef(AliAnaGammaCorr,0)
} ;
 

#endif //ALIANAGAMMACORR_H




#ifndef ALIPHOSGammaJet_H
#define ALIPHOSGammaJet_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

//_________________________________________________________________________
//  Class for the analysis of gamma-jet correlations     
//                  
//*-- Author: Gustavo Conesa & Yves Schutz (IFIC, CERN)

// --- ROOT system ---
#include "TTask.h"
#include "TH1.h"
#include "TH2.h"
#include "TMatrix.h"
#include "TList.h"
#include "AliPHOSFastGlobalReconstruction.h"
#include "../PYTHIA6/AliGenPythia.h"
// --- AliRoot header files ---

class AliPHOSGammaJet : public TTask {

public: 

  AliPHOSGammaJet() ; // default ctor
  AliPHOSGammaJet(const TString inputfilename) ; //ctor 
  AliPHOSGammaJet(const AliPHOSGammaJet & gj) ; // cpy ctor
  ~AliPHOSGammaJet() ; // dtor
  virtual void   Exec(Option_t *option); 
  void List() const; 
  Double_t GetAngleMaxParam(Int_t i){return fAngleMaxParam.At(i) ; }
  Double_t GetEtaCut(){return fEtaCut;} 
  Bool_t   GetFastReconstruction(){return fOptFast ; }
  Bool_t   GetHIJING(){return fHIJING ; }
  TString  GetHIJINGFileName(){return fHIJINGFileName ; }
  TString  GetHistosFileName(){return fOutputFileName ; }
  TString  GetJetQuenchingFileName(){return fJetQuenchingFileName ; }
  Double_t GetInvMassMaxCut(){return fInvMassMaxCut ; }
  Double_t GetInvMassMinCut(){return fInvMassMinCut ; }
  Bool_t   GetOnlyCharged(){return fOnlyCharged ; }
  Double_t GetPhiMaxCut(){return fPhiMaxCut ; }
  Double_t GetPhiMinCut(){return fPhiMinCut ; }
  Double_t GetMinDistance(){return fMinDistance ; }
  Double_t GetJetRatioMaxCut(){return fJetRatioMaxCut ; }
  Double_t GetJetRatioMinCut(){return fJetRatioMinCut ; }
  Bool_t   GetJetJetSimulation(){return fJetJet ; }
  Bool_t   GetJetQuenching(){return fJetQuenching ; }
  Double_t GetRatioMaxCut(){return fRatioMaxCut ; }
  Double_t GetRatioMinCut(){return fRatioMinCut ; }
  Double_t GetScalarProductCut(){return fScalarProductCut;}
  Int_t    GetNEvent(){return fNEvent ; }
  Int_t    GetNIterations(){return fNIter ; }

  void Plot(TString what="all", Option_t *option="") const;
  void Print(char * opt);

  void SetAngleMaxParam(Int_t i, Double_t par){fAngleMaxParam.AddAt(par,i) ; }
  void SetEtaCut(Double_t etacut) {fEtaCut = etacut ; }
  void SetFastReconstruction(Bool_t fast){fOptFast = fast ; }
  void SetHIJING(Bool_t opt){fHIJING = opt; }
  void SetHIJINGFileName(TString file){fHIJINGFileName = file ; }
  void SetOnlyCharged(Bool_t opt){fOnlyCharged = opt; }
  void SetPythiaFileName(TString file){fInputFileName = file ; }
  void SetHistosFileName(TString file){fOutputFileName = file ; }
  void SetJetQuenchingFileName(TString file){fJetQuenchingFileName  = file ; }
  void SetInvMassCutRange(Double_t invmassmin, Double_t invmassmax)
    {fInvMassMaxCut =invmassmax;  fInvMassMinCut =invmassmin;}	
  void SetJetRatioCutRange(Double_t ratiomin, Double_t ratiomax)
    {fJetRatioMaxCut =ratiomax;  fJetRatioMinCut = ratiomin ; }
  void SetJetJetSimulation(Bool_t jet){fJetJet  = jet ; }
  void SetJetQuenching(Bool_t quench){fJetQuenching  = quench ; }
  void SetNEvent(Int_t n){fNEvent  = n ; }
  void SetNIterations(Int_t n){fNIter  = n ; }
  void SetMinDistance(Double_t min){fMinDistance  = min ; }
  void SetPhiCutRange(Double_t phimin, Double_t phimax)
    {fPhiMaxCut =phimax;  fPhiMinCut =phimin;}
  void SetRatioCutRange(Double_t ratiomin, Double_t ratiomax)
    {fRatioMaxCut =ratiomax;  fRatioMinCut =ratiomin;}
  void SetScalarProductCut(Double_t cut){fScalarProductCut = cut ; }
  
 

 private:
  void AddHIJINGToList(TList & particleList, const Int_t iEvent, const TLorentzVector gamma); 
  void CreateParticleList(AliPHOSGetter *gime, TList & particleList, const Int_t idg);
  void EraseParticlesAwayFromJet(TList &particleList, TLorentzVector &jet);
  void FillJetParticleHistos(TList & particleList, TLorentzVector &jet, const TLorentzVector gamma,
			     const TString cone, const TString ptcut, const TString type);
  void MakeJet(TList &particleList, const TLorentzVector gamma, const TLorentzVector leading, 
	      TString & type); 
  void SetJetParticleListInCone(TList & particlelist, TList &newparticlelist,  TLorentzVector &jet,
				const TLorentzVector leading,const Double_t cone, 
				const Double_t ptcut);
  TLorentzVector GetGammaJet(AliPHOSGetter *gime, Bool_t & is, Int_t & idg) ;
  TLorentzVector GetAnyLeading(TList & particleList, const TLorentzVector gamma) ;
  TLorentzVector GetLeadingCharge(TList & particleList, const TLorentzVector gamma) ;
  TLorentzVector GetLeadingNeutral(TList & particleList, const TLorentzVector gamma) ;
  TLorentzVector GetLeadingPi0(TList & particleList, const TLorentzVector gamma) ;
  //TLorentzVector GetPythiaJet(AliGenPythia * pyth, AliPHOSGetter * gime, 
  //		      const Int_t event, const TLorentzVector gamma);
  //void InitJetQuenching(const TString input, const TString quench);
  void InitParameters();
  Double_t MakeEnergy(const Double_t energy) ;
  void MakeHistos() ;
  void MakePhoton(TParticle * particle) ; 
  TVector3 MakePosition(const Double_t energy, const TVector3 pos) ;
  void MakeJetQuenching(AliPHOSGetter * gime, TList &particleList, const Int_t iEvent, 
			TLorentzVector & gamma, const Int_t idg);
  //Bool_t  PhotonDetected(TParticle *particle);
  //Float_t PhotonEfficiency(Float_t pt);
  void Pi0Decay(Double_t mPi0, TLorentzVector &p0, 
		TLorentzVector &p1, TLorentzVector &p2, Double_t &angle) ;
  Double_t SigmaE(Double_t energy) ;
  Double_t SigmaP(Double_t energy) ;
  void  TransformAddJetToList(AliPHOSGetter * gime, TList & particleList, 
			      const TLorentzVector jet1, TLorentzVector jet2);
  void  TransformParticle(TParticle *particle,const TLorentzVector jet1, const TLorentzVector jet2) ;

 private: 
  TFile *  fOutputFile ; //! Output file
  TString  fOutputFileName;//! Output file Name
  TString  fInputFileName;//!
  TString  fHIJINGFileName;//!
  TString  fJetQuenchingFileName;//!
  Bool_t   fHIJING;
  Double_t fScalarProductCut;
  Double_t fEtaCut ; // Eta cut
  Bool_t fOnlyCharged ; // Only jets of charged particles
  Double_t fPhiMaxCut ; // Phi cut maximum
  Double_t fPhiMinCut ; // Phi cut minimun
  //AliGenPythia * fPyth1 ;
  //AliGenPythia * fPyth2 ;
  Double_t fInvMassMaxCut ; // Invariant Mass cut maximum
  Double_t fInvMassMinCut ; // Invariant Masscut minimun
  Double_t fMinDistance ; // Minimal distance to resolve gamma decay.
  Double_t fRatioMaxCut ; // Leading particle/gamma Ratio cut maximum
  Double_t fRatioMinCut ; // Leading particle/gamma Ratio cut minimum
  Double_t fJetRatioMaxCut ; // Jet/gamma Ratio cut maximum
  Double_t fJetRatioMinCut ; // Jet/gamma Ratio cut minimum
  Bool_t   fJetJet ; //JetJet simultions
  Bool_t   fJetQuenching ; //JetQuenching analysis
  Int_t    fNEvent ;
  Int_t    fNIter ; 
  TObjArray * fListHistos ; //! list of Histograms
  AliPHOSFastGlobalReconstruction * fFastRec;
  Bool_t fOptFast;
  TRandom fRan ;                             //! random number generator
  //Energy and position parameters
  Double_t fResPara1 ;                       // parameter for the energy resolution dependence  
  Double_t fResPara2 ;                       // parameter for the energy resolution dependence  
  Double_t fResPara3 ;                       // parameter for the energy resolution dependence 
  Double_t fPosParaA ;                      // parameter for the position resolution
  Double_t fPosParaB ;                      // parameter for the position resolution 
  TArrayD fAngleMaxParam ;

  ClassDef(AliPHOSGammaJet,10)
} ;
 

#endif //ALIPHOSGammaJet_H




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
// --- AliRoot header files ---

class AliPHOSGammaJet : public TTask {

public: 

  AliPHOSGammaJet() ; // default ctor
  AliPHOSGammaJet(const TString inputfilename) ; //ctor 
  AliPHOSGammaJet(const AliPHOSGammaJet & gj) ; // cpy ctor
  ~AliPHOSGammaJet() ; // dtor
  virtual void   Exec(Option_t *option); 
  void List() const; 
  Double_t GetAngleMaxParam(Int_t i){return fAngleMaxParam.At(i);}
  Double_t GetScalarProductCut(){return fScalarProductCut;}
  Bool_t   GetFastReconstruction(){return fOptFast ; }
  Double_t GetInvMassMaxCut(){return fInvMassMaxCut;}
  Double_t GetInvMassMinCut(){return fInvMassMinCut;}
  Double_t GetPhiMaxCut(){return fPhiMaxCut;}
  Double_t GetPhiMinCut(){return fPhiMinCut;}
  Double_t GetPtEffCut(){return fPtEffCut;}
  Double_t GetRatioMaxCut(){return fRatioMaxCut;}
  Double_t GetRatioMinCut(){return fRatioMinCut;}

  void Plot(TString what="all", Option_t *option="") const;
  void Print(char * opt);

  void SetAngleMaxParam(Int_t i, Double_t par){fAngleMaxParam.AddAt(par,i);}
  void SetScalarProductCut(Double_t cut){fScalarProductCut=cut;}
  void SetFastReconstruction(Bool_t fast){fOptFast = fast;}
  void SetHistosFileName(TString s){fOutputFileName = s;}
  void SetInvMassCutRange(Double_t invmassmin, Double_t invmassmax)
    {fInvMassMaxCut =invmassmax;  fInvMassMinCut =invmassmin;}	
  void SetPhiCutRange(Double_t phimin, Double_t phimax)
    {fPhiMaxCut =phimax;  fPhiMinCut =phimin;}
  void SetPtEffCut(Double_t pt){fPtEffCut =pt;}
  void SetRatioCutRange(Double_t ratiomin, Double_t ratiomax)
    {fRatioMaxCut =ratiomax;  fRatioMinCut =ratiomin;}
  Double_t GetEtaCut(){return fEtaCut;}
  void SetEtaCut(Double_t etacut) {fEtaCut =etacut;}
 

 private:
  void CreateParticleList(AliPHOSGetter *gime, TList & particleList);
  void EraseParticlesAwayFromJet(TList &particleList, TLorentzVector &jet);
  void FillJetParticleHistos(TList & particleList, TLorentzVector &jet, Double_t ptg, Double_t phig,
			     TString cone,TString ptcut, TString type, Double_t baseline);
  void GetJet(TList &particleList, TLorentzVector & gamma, TLorentzVector & leading, 
	      TString & type,Int_t & id); 
  void GetJetParticleListInCone(TList & particlelist, TList &newparticlelist,  TLorentzVector &jet,Double_t eta, 
				Double_t phi ,Double_t cone, Double_t ptcut, Int_t idg);
  void GetGammaJet(AliPHOSGetter *gime, TList & particleList, TLorentzVector & gamma, Int_t & id) ; 
  void GetAnyLeading(TList & particleList, TLorentzVector & any, Int_t & id, TLorentzVector & gamma) ;
  void GetLeadingCharge(TList & particleList, TLorentzVector & charge, Int_t & id, TLorentzVector & gamma) ;
  void GetLeadingNeutral(TList & particleList, TLorentzVector & neutral, Int_t & id, TLorentzVector & gamma) ;
  void GetLeadingPi0(TList & particleList, TLorentzVector & pi0, Int_t & id,TLorentzVector & gamma) ;
  void GetPi0FromGamma(TList &particleList, TLorentzVector &pi0, const Int_t id, const Int_t iPrimary, 
		       TParticle * particle) ;
  void InitParameters();
  Double_t MakeEnergy(const Double_t energy) ;
  void MakeHistos() ;
  void MakePhoton(TParticle * particle) ; 
  TVector3 MakePosition(const Double_t energy, const TVector3 pos) ;
  void MakeThetaPhi(const Double_t energy, TLorentzVector &lv, Double_t &modtheta, Double_t &modphi) ;
  //Bool_t  PhotonDetected(TParticle *particle);
  //Float_t PhotonEfficiency(Float_t pt);
  void Pi0Decay(Double_t mPi0, TLorentzVector &p0, 
		TLorentzVector &p1, TLorentzVector &p2, Double_t &angle) ;
  Double_t SigmaE(Double_t energy) ;
  Double_t SigmaP(Double_t energy) ;

 private: 
  TFile *  fOutputFile ; //! Output file
  TString  fOutputFileName;//! Output file Name
  Double_t fScalarProductCut;
  Double_t fEtaCut; // Eta cut
  Double_t fPhiMaxCut; // Phi cut maximum
  Double_t fPhiMinCut; // Phi cut minimun
  Double_t fInvMassMaxCut; // Invariant Mass cut maximum
  Double_t fInvMassMinCut; // Invariant Masscut minimun
  Double_t fPtEffCut;  // Cut on jet pt to calculate efficiency
  Double_t fRatioMaxCut; // Ratio cut maximum
  Double_t fRatioMinCut; // Ratio cut minimum
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
  TMatrix    *fMatrixParam;               //! Matrix of baseline Parameters
  TArrayD fAngleMaxParam ;

  ClassDef(AliPHOSGammaJet,5)
} ;
 

#endif //ALIPHOSGammaJet_H

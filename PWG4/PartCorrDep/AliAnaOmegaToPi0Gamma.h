/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id: $ */

//_________________________________________________________________________
// class to extract omega(782)->pi0+gamma->3gamma
//
//-- Author: Renzhuo Wan (IOPP-Wuhan, China)
//_________________________________________________________________________
#ifndef ALIANAOMEGATOPI0GAMMA_H
#define ALIANAOMEGATOPI0GAMMA_H
//Root
class TList;
class TH2F ;
class TLorentzVector;
//Analysis
#include "AliAnaPartCorrBaseClass.h"
class TParticle;

class AliAnaOmegaToPi0Gamma : public AliAnaPartCorrBaseClass {
  
  public: 
  
  AliAnaOmegaToPi0Gamma() ; // default ctor
  AliAnaOmegaToPi0Gamma(const char *name) ; // default ctor
  AliAnaOmegaToPi0Gamma(const AliAnaOmegaToPi0Gamma & g) ; // cpy ctor
  AliAnaOmegaToPi0Gamma & operator = (const AliAnaOmegaToPi0Gamma & api0) ;//cpy assignment
  virtual ~AliAnaOmegaToPi0Gamma() ;//virtual dtor
 
  TList * GetCreateOutputObjects(); 
  void Print(const Option_t * opt) const;
  
  void InitParameters();
  void MakeAnalysisFillHistograms();
  void Terminate(TList * outList);

  TString GetInputAODPhotonName() const {return fInputAODGammaName;}
  void SetInputAODPhotonName(TString name) {fInputAODGammaName=name;}
  Bool_t IsBadRun(Int_t /*iRun*/) const {return kFALSE;} //Tests if this run bad according to private list

  void SetNPtBinsMinMax(Int_t bins, Double_t min, Double_t max) {fNbinsPt=bins; fPtBegin=min; fPtEnd=max; } //set pt bins, min and max   
  void SetNMassBinsMinMas(Int_t bins, Double_t min, Double_t max) {fNbinsM=bins; fMinM=min; fMaxM=max; } //set mass pt bins, min and max   
  void SetNEventsMixed(Int_t nevents) { fNmaxMixEv=nevents;} //events to be mixed 
  void SetNPID(Int_t pid) {fNpid=pid;} 
  void SetPi0MassPeakWidthCut(Double_t win){fPi0MassWindow=win;} 

  void ReadHistograms(TList * outputList);

  private:

  TClonesArray * fInputAODGamma; //Input AOD gamma array 
  TClonesArray * fInputAODPi0;   //Input AOD pi0 array
  TString fInputAODGammaName;    //Input AOD gamma name
  TList ** fEventsList;          //event list for mixing 

  Double_t *fVtxZCut;            //vtertx z cut
  Double_t *fCent;               //centrality cut
  Double_t *fRp;                 //reaction plane cut
  Int_t *fBadChDist;             //bad channel dist

  Int_t fNVtxZBin;               //Number of vertex z cut
  Int_t fNCentBin;               //Number of centrality cut
  Int_t fNRpBin;                 //Number of reaction plane cut
 
  Int_t fNBadChDistBin;          //Number of bad channel dist cut
  Int_t fNpid;                   //Number of PID cut

  Int_t fNmaxMixEv;              //buffer size events to be mixed
  Double_t fPi0Mass;             //nominal pi0 mass
  Double_t fPi0MassWindow;       //pi0 mass windows

  Int_t fNbinsPt;                //Pt bin number, min and max
  Double_t fPtBegin;             //pt minmum
  Double_t fPtEnd;               //pt maxium
  Int_t fNbinsM;                 //mass bin number, min and max  
  Double_t fMinM;                //mass minmum
  Double_t fMaxM;                //mass maxium

  TH2F * fhEtalon;               //an etalon of 3D histograms

  TH2F **fRealOmega;             //real omega IVM(asy, pt, m), with Asy_pi0<1 
  TH2F **fMixAOmega;             //mixA omega IVM(asy, pt, m) 
  TH2F **fMixBOmega;             //mixB omega IVM(asy, pt, m) 
  TH2F **fMixCOmega;             //mixC omega IVM(asy, pt, m) 

  TH2F **fRealOmega1;            //real omega IVM(asy, pt, m), with Asy_pi0<0.7
  TH2F **fMixAOmega1;            //mixA omega IVM(asy, pt, m)
  TH2F **fMixBOmega1;            //mixB omega IVM(asy, pt, m)
  TH2F **fMixCOmega1;            //mixC omega IVM(asy, pt, m)

  TH2F **fRealOmega2;            //real omega IVM(asy, pt, m), with Asy_pi0<0.8
  TH2F **fMixAOmega2;            //mixA omega IVM(asy, pt, m)
  TH2F **fMixBOmega2;            //mixB omega IVM(asy, pt, m)
  TH2F **fMixCOmega2;            //mixC omega IVM(asy, pt, m)

  TH1F *fhOmegaPriPt;            //MC primary omega pt in 2pi and |y|<0.5
  ClassDef(AliAnaOmegaToPi0Gamma,1)
} ;

#endif 




#ifndef ALIANAGAMMAMC_H
#define ALIANAGAMMAMC_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class to select MC kinematics data for the analysis gamma isolation and correlations
//
//  Class created from old AliPHOSGammaJet
//  (see AliRoot versions previous Release 4-09)

//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
#include <TParticle.h> 
#include <TClonesArray.h> 
#include "AliStack.h"
#include "AliAnalysisTaskRL.h" 
 
class AliESD ; 

class TH2F ; 

class AliAnaGammaMC : public AliAnalysisTaskRL {

public: 

  AliAnaGammaMC(const char *name) ; // default ctor
  AliAnaGammaMC(const AliAnaGammaMC & g) ; // cpy ctor
  AliAnaGammaMC & operator = (const AliAnaGammaMC & g) ;//cpy assignment
  virtual ~AliAnaGammaMC() ; //virtual dtor

  virtual void Exec(Option_t * opt = "") ;
  virtual void ConnectInputData(Option_t *);
  virtual void CreateOutputObjects();
  virtual void Terminate(Option_t * opt = "");
  
  void InitParameters();

  TString  GetCalorimeter()       const {return fCalorimeter ; } 
  Bool_t   IsDecayPi0On() const {return fDecayPi0 ; }
  Float_t   GetCTSEtaCut() const {return fCTSEtaCut ; }
  Float_t   GetEMCALEtaCut() const {return fEMCALEtaCut ; }
  Float_t   GetPHOSEtaCut() const {return fPHOSEtaCut ; }
  Float_t  GetPhiEMCALCut(Int_t i) { return  fPhiEMCALCut[i]  ; }
  Float_t  GetPhiPHOSCut(Int_t i) { return  fPhiPHOSCut[i]  ; }
  Float_t  GetNeutralPtCut()    {  return fNeutralPtCut  ; }
  Float_t  GetChargedPtCut()  {  return fChargedPtCut  ; }
  Float_t  GetEMCALIPDistance()  {  return fEMCALIPDistance ; }
  Float_t  GetPHOSIPDistance()  {  return fPHOSIPDistance ; }
  Float_t  GetEMCALMinDistance()  {  return fEMCALMinDistance ; }
  Float_t  GetPHOSMinDistance()  {  return fPHOSMinDistance ; }

  void Print(const Option_t * opt)const;
  
  void SetCalorimeter(TString calo){ fCalorimeter= calo ; }   
  void SetDecayPi0(Bool_t d){ fDecayPi0 = d ; }
  void SetCTSEtaCut(Float_t eta){ fCTSEtaCut= eta ; }
  void SetEMCALEtaCut(Float_t eta){ fEMCALEtaCut= eta ; }
  void SetPHOSEtaCut(Float_t eta){ fPHOSEtaCut= eta ; }
  void SetPhiEMCALCut(Float_t  phi0, Float_t  phi1)
  { fPhiEMCALCut[0]= phi0 ; fPhiEMCALCut[1]= phi1 ;}
  void SetPhiPHOSCut(Float_t  phi0, Float_t  phi1)
  { fPhiPHOSCut[0]= phi0 ; fPhiPHOSCut[1]= phi1 ;}
  void SetNeutralPtCut(Float_t  pt){  fNeutralPtCut = pt ; }
  void SetChargedPtCut(Float_t  pt){  fChargedPtCut = pt ; }
  void SetEMCALIPDistance(Float_t  d){  fEMCALIPDistance = d ; }
  void SetPHOSIPDistance(Float_t  d){  fPHOSIPDistance = d ; }
  void SetEMCALMinDistance(Float_t  d){  fEMCALMinDistance = d ; }
  void SetPHOSMinDistance(Float_t  d){  fPHOSMinDistance = d ; }

  void CreateParticleList(AliStack* stack);
  void MakePi0Decay(TString calo,TParticle * particle,  TClonesArray * plCalo,  Int_t &indexCalo);

  void Pi0Decay(TLorentzVector &p0, TLorentzVector &p1, TLorentzVector &p2, 
		Double_t &angle);
  
 private:

  TTree       *fChain ;   //!pointer to the analyzed TTree or TChain
  AliESD       *fESD ;     //! Declaration of leave types
  TClonesArray  * fCTSList ; //! output charged particles container
  TClonesArray  * fEMCALList ; //! output emcal clusters container
  TClonesArray  * fPHOSList ; //! output phos clusters container
  TClonesArray  * fPartonList ; //! output phos clusters container

  TString      fCalorimeter ; //PHOS or EMCAL detects Gamma
  Float_t      fCTSEtaCut ;//CTS  pseudorapidity acceptance
  Float_t      fEMCALEtaCut ;//EMCAL pseudorapidity acceptance
  Float_t      fPHOSEtaCut ;//PHOS pseudorapidity acceptance
  Float_t      fPhiEMCALCut[2]; //EMCAL phi acceptance 
  Float_t      fPhiPHOSCut[2];  //PHOS phi acceptance
  Float_t      fNeutralPtCut; //
  Float_t      fChargedPtCut;  // 
  Float_t      fEMCALIPDistance; //Calorimeter IP distance.
  Float_t      fPHOSIPDistance; //Calorimeter IP distance
  Float_t      fEMCALMinDistance; //Gamma decay minimum aperture.
  Float_t      fPHOSMinDistance; //Gamma decay minimum aperture.

  Bool_t      fDecayPi0; //Decay Pi0.


  ClassDef(AliAnaGammaMC,0)
} ;
 

#endif //ALIANAGAMMAMC_H




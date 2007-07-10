#ifndef ALIANAGAMMADATA_H
#define ALIANAGAMMADATA_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class to select ESD and AOD data for the analysis gamma isolation and correlations
//
//  Class created from old AliPHOSGammaJet
//  (see AliRoot versions previous Release 4-09)

//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
#include <TParticle.h> 
#include <TLorentzVector.h>
#include <TClonesArray.h> 
#include "AliAnalysisTask.h" 

class AliESD ; 
class TH2F ; 
class TTree ;  

class AliAnaGammaData : public AliAnalysisTask {

public: 

  AliAnaGammaData(const char *name) ; // default ctor
  AliAnaGammaData(const AliAnaGammaData & g) ; // cpy ctor
  AliAnaGammaData & operator = (const AliAnaGammaData & g) ;//cpy assignment
  virtual ~AliAnaGammaData() ; //virtual dtor

  virtual void Exec(Option_t * opt = "") ;
  virtual void ConnectInputData(Option_t *);
  virtual void CreateOutputObjects();
  virtual void Terminate(Option_t * opt = "");
  
  void InitParameters();

  TString    GetCalorimeter()       const {return fCalorimeter ; } 
  Bool_t   IsEMCALPIDOn() const {return fEMCALPID ; }
  Bool_t   IsPHOSPIDOn() const {return fPHOSPID ; }
  Float_t  GetEMCALPhotonWeight() { return  fEMCALPhotonWeight  ; }
  Float_t  GetEMCALPi0Weight()    {  return fEMCALPi0Weight  ; }
  Float_t  GetPHOSPhotonWeight()  {  return fPHOSPhotonWeight  ; }
  
  
  void Print(const Option_t * opt)const;
  
  void SetCalorimeter(TString calo){ fCalorimeter= calo ; }   
  void SetEMCALPIDOn(Bool_t pid){ fEMCALPID= pid ; }
  void SetPHOSPIDOn(Bool_t pid){ fPHOSPID= pid ; }
  void SetEMCALPhotonWeight(Float_t  w){  fEMCALPhotonWeight = w ; }
  void SetEMCALPi0Weight(Float_t  w){  fEMCALPi0Weight = w ; }
  void SetPHOSPhotonWeight(Float_t  w){  fPHOSPhotonWeight = w ; }


  void CreateParticleList();

  
 private:

  TTree       *fChain ;   //!pointer to the analyzed TTree or TChain
  AliESD       *fESD ;     //! Declaration of leave types
  TClonesArray  * fCTSList ; //! output charged particles container
  TClonesArray  * fEMCALList ; //! output emcal clusters container
  TClonesArray  * fPHOSList ; //! output phos clusters container
  TString      fCalorimeter ; //PHOS or EMCAL detects Gamma
  Bool_t       fEMCALPID ;//Fill EMCAL particle lists with particles with corresponding pid
  Bool_t       fPHOSPID;  //Fill PHOS particle lists with particles with corresponding pid
  Float_t      fEMCALPhotonWeight; //Bayesian PID weight for photons in EMCAL 
  Float_t      fEMCALPi0Weight;  //Bayesian PID weight for pi0 in EMCAL 
  Float_t      fPHOSPhotonWeight; //Bayesian PID weight for photons in PHOS 
  
  ClassDef(AliAnaGammaData,0)
} ;
 

#endif //ALIANAGAMMADATA_H




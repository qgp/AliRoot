#ifndef ALIGAMMAMCDATAREADER_H
#define ALIGAMMAMCDATAREADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class for reading data (Kinematics and ESDs) in order to do prompt gamma correlations
//  Class created from old AliPHOSGammaJet
//  (see AliRoot versions previous Release 4-09)

//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
#include <TParticle.h> 
#include <TClonesArray.h> 
#include "AliGammaReader.h" 

class AliESD ;

class AliGammaMCDataReader : public AliGammaReader {

public: 

  AliGammaMCDataReader() ; // ctor
  AliGammaMCDataReader(const AliGammaMCDataReader & g) ; // cpy ctor
  AliGammaMCDataReader & operator = (const AliGammaMCDataReader & g) ;//cpy assignment
  virtual ~AliGammaMCDataReader() {;} //virtual dtor

 
  void InitParameters();

  Bool_t   IsEMCALPIDOn() const {return fEMCALPID ; }
  Bool_t   IsPHOSPIDOn() const {return fPHOSPID ; }
  Float_t  GetEMCALPhotonWeight() { return  fEMCALPhotonWeight  ; }
  Float_t  GetEMCALPi0Weight()    {  return fEMCALPi0Weight  ; }
  Float_t  GetPHOSPhotonWeight()  {  return fPHOSPhotonWeight  ; }
  Float_t  GetPHOSPi0Weight()  {  return fPHOSPi0Weight  ; }
  
  void Print(const Option_t * opt)const;
  
  void SetEMCALPIDOn(Bool_t pid){ fEMCALPID= pid ; }
  void SetPHOSPIDOn(Bool_t pid){ fPHOSPID= pid ; }
  void SetEMCALPhotonWeight(Float_t  w){  fEMCALPhotonWeight = w ; }
  void SetEMCALPi0Weight(Float_t  w){  fEMCALPi0Weight = w ; }
  void SetPHOSPhotonWeight(Float_t  w){  fPHOSPhotonWeight = w ; }
  void SetPHOSPi0Weight(Float_t  w){  fPHOSPi0Weight = w ; }

  void CreateParticleList(TObject * esd, TObject * stack, TClonesArray * plCh, 
			  TClonesArray * plEMCAL, TClonesArray * plPHOS, TClonesArray *);

  
 private:

  Bool_t       fEMCALPID ;//Fill EMCAL particle lists with particles with corresponding pid
  Bool_t       fPHOSPID;  //Fill PHOS particle lists with particles with corresponding pid
  Float_t      fEMCALPhotonWeight; //Bayesian PID weight for photons in EMCAL 
  Float_t      fEMCALPi0Weight;  //Bayesian PID weight for pi0 in EMCAL 
  Float_t      fPHOSPhotonWeight; //Bayesian PID weight for photons in PHOS 
  Float_t      fPHOSPi0Weight; //Bayesian PID weight for pi0 in PHOS 

  ClassDef(AliGammaMCDataReader,0)
} ;
 

#endif //ALIGAMMAMCDATAREADER_H




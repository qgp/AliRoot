#ifndef ALIPHOSVFAST_H
#define ALIPHOSVFAST_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
// Implementation of the PHOS manager class for fast simulations     
// Tracks particles until the reach a grossly designed PHOS module
// Modify the particles property (momentum, energy, type) according to
//  the PHOS response function. The result is called a virtual reconstructed
//  particle.                                
//                  
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---
#include "TClonesArray.h"
#include "TRandom.h"

class TVector3 ;
class TFile;

// --- AliRoot header files ---
#include "AliPHOS.h"
class AliPHOSGeometry ;
class AliPHOSFastRecParticle ;

class AliPHOSvFast : public AliPHOS {

public:

  AliPHOSvFast() ;
  AliPHOSvFast(const char *name, const char *title="") ;
  AliPHOSvFast(const AliPHOSvFast & fast) {
    // cpy ctor: no implementation yet
    // requested by the Coding Convention
    assert(0==1) ; 
  }
  virtual ~AliPHOSvFast(void) ;

  virtual void   AddHit( Int_t shunt, Int_t primary, Int_t track, Int_t id, Float_t *hits ) {
    // useless since there are no hits
    assert(0==1) ; 
  }
  void           AddRecParticle(const AliPHOSFastRecParticle & rp) ; // adds primary particle to the RecParticles list
  virtual void   BuildGeometry(void) ;                               // creates the geometry for the ROOT display
  virtual void   CreateGeometry(void) ;                              // creates the geometry for GEANT
  Float_t        GetBigBox(Int_t index) const;                             
  virtual void   Init(void) ;                                        // does nothing
  virtual Int_t  IsVersion(void) const {
    // Gives the version number 
    return 99 ; 
  }

  void    MakeBranch(Option_t* opt);
  Double_t MakeEnergy(const Double_t energy) ;                       // makes the detected energy    
  TVector3 MakePosition(const Double_t energy, const TVector3 pos, const Double_t th, const Double_t ph) ; 
                                                                     // makes the detected position
  void MakeRecParticle(const Int_t modid, const TVector3 pos, AliPHOSFastRecParticle & rp) ;  // makes a reconstructes particle from primary
  Int_t   MakeType(AliPHOSFastRecParticle & rp) ;                    // gets the detected type of particle
  // gets TClonesArray of reconstructed particles
  TClonesArray * FastRecParticles() const { return fFastRecParticles ; } 
  virtual void ResetPoints() ; 
  void         ResetFastRecParticles() ; 
  void         SetBigBox(Int_t index, Float_t value) ;                             
  Double_t     SigmaE(Double_t energy) ;    // calulates the energy resolution at a given Energy                           
  Double_t     SigmaP(Double_t energy, Double_t inc) ; // calulates the position resolution at a given Energy at a given incidence                           
  virtual void StepManager(void) ;          // does the tracking through PHOS and a preliminary digitalization
  virtual const TString Version(void)const { 
    // As IsVersion
    return TString("vFast") ; 
  }

  AliPHOSvFast & operator = (const AliPHOSvFast & )  {
    // assignement operator requested by coding convention but not needed
    assert(0==1) ;
    return *this ; 
  }
  
private:
  
  Float_t fBigBoxX ;                         // main box containing all PHOS (EMC+PPSD)
  Float_t fBigBoxY ;                         // main box containing all PHOS (EMC+PPSD)
  Float_t fBigBoxZ ;                         // main box containing all PHOS (EMC+PPSD)
  TClonesArray * fFastRecParticles ;         // list of particles modified by the response function 
  Int_t fNRecParticles ;                     // number of detected particles
  TRandom fRan ;                             // random number generator
  Double_t fResPara1 ;                       // parameter for the energy resolution dependence  
  Double_t fResPara2 ;                       // parameter for the energy resolution dependence  
  Double_t fResPara3 ;                       // parameter for the energy resolution dependence 
  Double_t fPosParaA0 ;                      // parameter for the position resolution
  Double_t fPosParaA1 ;                      // parameter for the position resolution 
  Double_t fPosParaB0 ;                      // parameter for the position resolution 
  Double_t fPosParaB1 ;                      // parameter for the position resolution 
  Double_t fPosParaB2 ;                      // parameter for the position resolution

  ClassDef(AliPHOSvFast,1)  //  Implementation of the PHOS manager class for fast simulations  

};

#endif // AliPHOSVFAST_H

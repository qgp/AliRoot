#ifndef ALIPHOSHIT_H
#define ALIPHOSHIT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  Hits class for PHOS     
//  A hit in PHOS is the sum of all hits in a single crystal
//               
//*-- Author: Maxime Volkov (RRC KI) & Yves Schutz (SUBATECH)

// --- ROOT system ---
#include <TLorentzVector.h>

// --- AliRoot header files ---
#include "AliHit.h"

// --- Standard library ---


class AliPHOSHit : public AliHit {

  friend ostream& operator << (ostream&, const AliPHOSHit&) ;
  
 public:
  
  AliPHOSHit() {
    // default ctor 
  }
  AliPHOSHit(const AliPHOSHit & hit) ; 
  AliPHOSHit(Int_t shunt, Int_t primary, Int_t tracknumber, Int_t id, Float_t *hits);
  virtual ~AliPHOSHit(void) {
    // dtor 
  }  
  
  Float_t GetEnergy(void)   const { 
    // returns the energy loss for this hit 
    return fELOS ; 
  }
  Int_t   GetId(void)       const { 
    // return the identificator of this his
    return fId ; 
  }
  Int_t   GetPrimary(void)  const { 
    // returns the primary particle id at the origine of this hit 
    return fPrimary ; 
  }

  Float_t GetTime(void)     const {
    // returns the time of the first energy deposition
    return fTime ;
  }

  virtual Float_t X() const ;
  virtual Float_t Y() const ;
  virtual Float_t Z() const ;

  Bool_t operator == (AliPHOSHit const &rValue) const ;
  AliPHOSHit operator + (const AliPHOSHit& rValue) ;


 private:

  Int_t          fId ;        // Absolute Id number of PHOS Xtal or PPSD pad
  Float_t        fELOS ;      // Energy deposited
  Int_t          fPrimary ;   // Primary particles at the origine of the hit
  Float_t        fTime ;      // Time of the energy deposition

  ClassDef(AliPHOSHit,1)  // Hit for PHOS

} ;

//////////////////////////////////////////////////////////////////////////////

#endif // ALIPHOSHIT_H

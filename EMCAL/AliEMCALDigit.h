#ifndef ALIEMCALDIGIT_H
#define ALIEMCALDIGIT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  EMCAL digit: 
// 
//  A  Digit is the sum of energy in a Tower (Hit sum) and stores information, about primaries
//  and enterring particle contributing to a Digit
//
//*-- Author: Sahal Yacoob (LBL)
// based on : AliPHOSDigit
//___________________________________________________________________________

// --- ROOT system ---

#include "TObject.h" 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliDigitNew.h"

class AliEMCALDigit : public AliDigitNew {

  friend ostream& operator << ( ostream& , const AliEMCALDigit&) ;

 public:
  
  AliEMCALDigit() ;
  AliEMCALDigit(Int_t primary, Int_t iparent, Int_t id, Int_t DigEnergy, Float_t Time, Int_t index = -1) ;
  AliEMCALDigit(const AliEMCALDigit & digit) ;
  virtual ~AliEMCALDigit() ;

  Bool_t operator==(const AliEMCALDigit &rValue) const;
  AliEMCALDigit& operator+(AliEMCALDigit const &rValue) ;
  AliEMCALDigit& operator*(Float_t factor) ; 
 
  Int_t   Compare(const TObject * obj) const ;  
  const Float_t GetEta() const ; 
  Int_t   GetNprimary() const { 
    // returns the number of primaries
    return fNprimary ; }
  Int_t   GetPrimary(Int_t index) const ; 
  Int_t   GetNiparent() const {return fNiparent;}
  Int_t   GetIparent(Int_t index) const ;
  const Float_t GetPhi() const;
  Float_t GetTime(void) const {return fTime ;}
  const Bool_t IsInPreShower() const ;
  Bool_t  IsSortable() const { 
    // says that AliEMCALDigits are sortable (needed for Sort method
    return kTRUE ; }
  void    SetAmp(Int_t amp) { 
    // sets the amplitude data member 
    fAmp= amp ; } 
  void SetId(Int_t id) {fId = id ;}
  void SetTime(Float_t time) {fTime = time ;}
  void ShiftPrimary(Int_t shift); // shift to semarate different TreeK in merging
 
 private: 
  Int_t fNprimary ;     // Number of primaries
  Int_t fNMaxPrimary ;  // Max Number of primaries
  Int_t *fPrimary ;     //[fNMaxPrimary]  Array of primaries       
    
  Int_t fNiparent ;     // Number of initial parents 
  Int_t fNMaxiparent ;  // Max Number of parents 
  Int_t *fIparent ;     //[fNMaxiparent] Array of parents       
  Int_t fMaxIter  ;     // Number to Increment Maxiparent, and MaxPrimary if default is not sufficient
  Float_t fTime ;       // Calculated time  

  ClassDef(AliEMCALDigit,1)   // Digit in EMCAL 

} ;

#endif //  ALIEMCALDIGIT_H

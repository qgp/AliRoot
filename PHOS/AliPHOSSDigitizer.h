#ifndef ALIPHOSSDigitizer_H
#define ALIPHOSSDigitizer_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//_________________________________________________________________________
//  Task Class for making SDigits in PHOS      
// A Summable Digits is the sum of all hits originating 
// from one primary in one active cell
//*--
//*-- Author: Dmitri Peressounko(SUBATECH & KI)


// --- ROOT system ---
#include "TTask.h"
class TFile ;

// --- Standard library ---

// --- AliRoot header files ---
#include "AliConfig.h"

class AliPHOSSDigitizer: public TTask {

public:
  AliPHOSSDigitizer() ;          // ctor
  AliPHOSSDigitizer(const char * alirunFileName, const char * eventFolderName = AliConfig::fgkDefaultEventFolderName) ; 
  AliPHOSSDigitizer(const AliPHOSSDigitizer & sd) ; // cpy ctor
  virtual ~AliPHOSSDigitizer() ; // dtor

  Float_t  Calibrate(Int_t amp)const {return (amp - fA)/fB ; }
  Int_t    Digitize(Float_t Energy)const { return (Int_t ) ( fA + Energy*fB); }
  virtual void   Exec(Option_t *option); 
  const char *   GetSDigitsBranch()const{return GetName();}  
  const Int_t    GetSDigitsInRun() const {return fSDigitsInRun ;}  
  virtual void Print(Option_t* option) const ;
  void SetSDigitsBranch(const char * title ) ;
  void UseHitsFrom(const char * filename) {;}      
  Bool_t operator == (const AliPHOSSDigitizer & sd) const ;
  AliPHOSSDigitizer & operator = (const AliPHOSSDigitizer & sd) {return *this ;}
  
private:
  void     Init() ;
  void     InitParameters() ;
  void     PrintSDigits(Option_t * option) ;
  void     Unload() const ;


private:
  Float_t fA ;              // Pedestal parameter
  Float_t fB ;              // Slope Digitizition parameters
  Float_t fPrimThreshold ;  // To store primari if Elos > threshold
  Bool_t  fDefaultInit;     //! Says if the task was created by defaut ctor (only parameters are initialized)
  Int_t   fSDigitsInRun ;   //! Total number of sdigits in one run
  Bool_t  fInit ;           //! tells if initialisation wennt OK, will revent exec if not
  TString fEventFolderName; // event folder name
  ClassDef(AliPHOSSDigitizer,1)  // description 

};

#endif // AliPHOSSDigitizer_H

#ifndef ALIPHOSRAW2DIGITS_H
#define ALIPHOSRAW2DIGITS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $id */

//_________________________________________________________________________
//  Base Class for PHOS     
//                  
/*-- Author: Maxim Volkov (RRC KI)
              Dmitri Peressounko (RRC KI & SUBATECH)
              Yuri Kharlov (IHEP & SUBATECH)     */

// --- ROOT system ---
#include "TTask.h"
class TClonesArray ;

// --- Standard library ---

// --- AliRoot header files ---
class AliPHOSGeometry ;
class AliPHOSBeamTestEvent ;
class AliPHOSConTableDB ;

class AliPHOSRaw2Digits : public TTask {

public:
  AliPHOSRaw2Digits() ;          // ctor
  AliPHOSRaw2Digits(const char * inputFileName) ;         
  virtual ~AliPHOSRaw2Digits() ; // dtor

  void Exec(Option_t *option) ;

  void SetInputFile(TString inname="Run_1234.fz"){fInName=inname ; }
  void SetDebugLevel(Int_t idebug=1){fDebug=idebug ;}

  //Set position of the target in the given run.
  //The reference system is following
  //Z axis along beam direction, from target to prototype (0-surface of prototype)
  //X axis along columns of prototype (0-center of prototype)
  //Y axis along raws of prototype    (0-center of prototype)
  void SetTargetPosition(Double_t * pos)
    {for(Int_t i=0;i<3;i++)fTarget[i]=pos[i] ;}
  void SetConTableDB(AliPHOSConTableDB * ctdb){fctdb = ctdb ;}
  void Print(Option_t *option="")const ;

private:
  void FinishRun() ;
  Bool_t ProcessRawFile() ;
  void Swab4(void *from, void *to, size_t nwords)  ;
  void Swab2(void *from, void *to, size_t nwords)  ;
  Bool_t Init() ;
  void WriteDigits(void) ;

  TClonesArray * fDigits ;             //!list of final digits
  AliPHOSBeamTestEvent * fPHOSHeader ; //!
  AliPHOSConTableDB * fctdb ;          //!
  Double_t fTarget[3] ;                //!Position of the target
  Int_t   fEvent ;         //
  Int_t   fStatus ;        //status of input file: OK, not found etc.
  TString fInName ;        // FileName of the input file
  Bool_t  fDebug ;         //!
  Bool_t  fIsInitialized ; //!
  UInt_t  fMK1 ;     //!ZEBRA markers
  UInt_t  fMK2 ;     //!
  UInt_t  fMK3 ;     //!
  UInt_t  fMK4 ;     //!
  UInt_t  fCKW ;     //!

  ClassDef(AliPHOSRaw2Digits,1)  // description 

};

#endif // AliPHOSRAW2DIGITS_H

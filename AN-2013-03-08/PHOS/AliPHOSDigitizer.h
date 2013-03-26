#ifndef ALIPHOSDigitizer_H
#define ALIPHOSDigitizer_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 * Revision 1.37  2007/10/10 09:05:10  schutz
 * Changing name QualAss to QA
 *
 * Revision 1.36  2007/09/30 17:08:20  schutz
 * Introducing the notion of QA data acquisition cycle (needed by online)
 *
 * Revision 1.35  2007/08/07 14:12:03  kharlov
 * Quality assurance added (Yves Schutz)
 *
 * Revision 1.34  2006/04/29 20:25:30  hristov
 * Decalibration is implemented (Yu.Kharlov)
 *
 * Revision 1.33  2005/05/28 14:19:04  schutz
 * Compilation warnings fixed by T.P.
 *
 */

//_________________________________________________________________________
//  Task Class for making SDigits in PHOS      
// Class performs digitization of Summable digits (in the PHOS case this is just
// sum of contributions of all primary particles into given cell). 
// In addition it performs mixing of summable digits from different events.
//                  
//*-- Author: Dmitri Peressounko(SUBATECH & KI)


// --- ROOT system ---
//#include "TObjString.h"
class TArrayI ;
class TClonesArray ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliDigitizer.h"
#include "AliConfig.h"
#include "AliPHOSPulseGenerator.h"
class AliDigitizationInput ;
class AliPHOSCalibData ; 

class AliPHOSDigitizer: public AliDigitizer {

public:
  AliPHOSDigitizer() ;          // ctor
  AliPHOSDigitizer(TString alirunFileNameFile, TString eventFolderName = AliConfig::GetDefaultEventFolderName()) ; 
  AliPHOSDigitizer(AliDigitizationInput * digInput) ;
  virtual ~AliPHOSDigitizer() ;       

  void    Digitize(Int_t event) ;            // Make Digits from SDigits 
  void    Digitize(Option_t *option);                  // Supervising method

  void   SetEventRange(Int_t first=0, Int_t last=-1) {fFirstEvent=first; fLastEvent=last; }

  //General
  Int_t   GetDigitsInRun()  const { return fDigitsInRun ;}  

  void    Print(const Option_t * = "")const ;
 
private:
  AliPHOSDigitizer(const AliPHOSDigitizer & dtizer) ;
  AliPHOSDigitizer & operator = (const AliPHOSDigitizer & /*rvalue*/);

  virtual Bool_t Init() ; 
  void    InitParameters() ; 
  void    PrintDigits(Option_t * option) ;
  void    Unload() ; 
  void    WriteDigits() ;                     // Writes Digits for the current event
  Float_t TimeOfNoise(void) const;            // Calculate time signal generated by noise

  Float_t TimeResolution(Float_t energy) ;    //TOF resolution

  //Calculate the time of crossing of the threshold by front edge
  //  Float_t FrontEdgeTime(TClonesArray * ticks) const ; 
  //Calculate digitized signal with gived ADC parameters
  Float_t Calibrate(Float_t amp, Int_t absId) ;
  Float_t CalibrateT(Float_t time, Int_t absId) ;
  void    Decalibrate(AliPHOSDigit * digit);
  Int_t   DigitizeCPV(Float_t charge, Int_t absId) ;

private:

  Bool_t  fDefaultInit;             //! Says if the task was created by defaut ctor (only parameters are initialized)
  Int_t   fDigitsInRun ;            //! Total number of digits in one run
  Bool_t  fInit ;                   //! To avoid overwriting existing files

  Int_t   fInput ;                  // Number of files to merge
  TString * fInputFileNames ;       //[fInput] List of file names to merge 
  TString * fEventNames ;           //[fInput] List of event names to merge

  Int_t   fEmcCrystals ;            // Number of EMC crystalls in the given geometry

  TString fEventFolderName;         // skowron: name of EFN to read data from in stand alone mode
  Int_t   fFirstEvent;              // first event to process
  Int_t   fLastEvent;               // last  event to process 
  AliPHOSCalibData* fcdb;           //! Calibration parameters DB

  Int_t fEventCounter ;             //! counts the events processed

  AliPHOSPulseGenerator *fPulse;    //! Pulse shape generator
  Int_t *fADCValuesLG;              //! Array of low-gain ALTRO samples
  Int_t *fADCValuesHG;              //! Array of high-gain ALTRO samples
  
  ClassDef(AliPHOSDigitizer,5)  // description 

};


#endif // AliPHOSDigitizer_H

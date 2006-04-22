#ifndef ALIEMCALDigitizer_H
#define ALIEMCALDigitizer_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  Task Class for making Digits in EMCAL      
//                  
//*-- Author: Sahal Yacoob (LBL)
// based on : AliPHOSDigit
// July     2003 Yves Schutz : NewIO 
// November 2003 Aleksei Pavlinov : Shish-Kebab geometry 
//_________________________________________________________________________ 


// --- ROOT system ---
#include "TObjString.h"
class TArrayI ;
class TClonesArray ; 
class TList;
class TBrowser;

// --- Standard library ---

// --- AliRoot header files ---
#include "AliDigitizer.h"
#include "AliConfig.h"
class AliEMCALSDigitizer ;
class AliRunDigitizer ;

class AliEMCALDigitizer: public AliDigitizer {

public:
  AliEMCALDigitizer() ;          // ctor
  AliEMCALDigitizer(TString alirunFileNameFile, TString eventFolderName = AliConfig::GetDefaultEventFolderName()) ;  
  AliEMCALDigitizer(const AliEMCALDigitizer & dtizer) ;
  AliEMCALDigitizer(AliRunDigitizer * manager) ;
  virtual ~AliEMCALDigitizer() ;       

  void    Digitize(Int_t event);          // Make Digits from SDigits stored in fSDigits
  void    Exec(Option_t *option);               // Supervising method

  Float_t GetDigitThreshold() const { return fDigitThreshold;}
  Float_t GetPedestal()       const { return fPedestal; }
  Float_t GetPinNoise()       const { return fPinNoise;}
  Float_t GetSlope()          const { return fSlope; }
  Float_t GetTimeResolution() const { return fTimeResolution ; }
  Float_t GetECAchannel()     const { return fADCchannelEC ; }
  Float_t GetECApedestal()    const { return fADCpedestalEC ; }
 void   SetEventRange(Int_t first=0, Int_t last=-1) {fFirstEvent=first; fLastEvent=last; }
  void    SetDigitThreshold(Float_t EMCThreshold)  {fDigitThreshold = EMCThreshold;}
  void    SetPinNoise(Float_t PinNoise )         {fPinNoise = PinNoise;}

  //General
  Int_t   GetDigitsInRun()  const { return fDigitsInRun; } 
  void  MixWith(TString alirunFileName, 
		TString eventFolderName = AliConfig::GetDefaultEventFolderName()) ; // Add another one file to mix
  void  Print(Option_t* option="") const ;
  void  Print1(Option_t * option); // *MENU*
 
  AliEMCALDigitizer & operator = (const AliEMCALDigitizer & /*rvalue*/)  {
    // assignement operator requested by coding convention but not needed
   Fatal("operator =", "not implemented") ;  
   return *this ; 
  }

  virtual void Browse(TBrowser* b);
  // hists
  void   SetControlHists(Int_t var=0) {fControlHists=var;}
  Int_t  GetControlHist() const {return fControlHists;}
  TList *GetListOfHists() {return fHists;}
  TList* BookControlHists(int var=0);
  void   SaveHists(const char* name="RF/TRD1/Digitizations/DigiVar?",
  Bool_t kSingleKey=kTRUE, const char* opt="RECREATE"); // *MENU*

private:

  Bool_t  Init();                   
  void    InitParameters() ; 
  void    PrintDigits(Option_t * option) ;
  void    Unload() ; 
  void    WriteDigits() ;         // Writes Digits the current event
  Float_t TimeOfNoise(void) ;     // Calculate time signal generated by noise

  //Calculate the time of crossing of the threshold by front edge
  Float_t FrontEdgeTime(TClonesArray * ticks) ;
  Int_t   DigitizeEnergy(Float_t energy, Int_t AbsId) ;

private:
  
  Bool_t  fDefaultInit;           //! Says if the task was created by defaut ctor (only parameters are initialized)
  Int_t   fDigitsInRun ;          //! Total number of digits in one run
  Bool_t  fInit ;                 //! To avoid overwriting existing files

  Int_t   fInput ;                // Number of files to merge
  TString * fInputFileNames ;     //[fInput] List of file names to merge 
  TString * fEventNames ;         //[fInput] List of event names to merge

  Float_t fDigitThreshold  ;      // Threshold for storing digits in EMC
  Int_t   fMeanPhotonElectron ;   // number of photon electrons per GeV deposited energy 
  Float_t fPedestal ;             // Calibration parameters 
  Float_t fSlope ;                // read from SDigitizer
  Float_t fPinNoise ;             // Electronics noise in EMC
  Float_t fTimeResolution ;       // Time resolution of FEE electronics
  Float_t fTimeThreshold ;        // Threshold to start timing for given crystall
  Float_t fTimeSignalLength ;     // Length of the timing signal 
  Float_t fADCchannelEC ;         // width of one ADC channel in EC section (GeV)
  Float_t fADCpedestalEC ;        // pedestal for one ADC channel
  Int_t   fNADCEC ;               // number of channels in EC section ADC

  TString fEventFolderName;         // skowron: name of EFN to read data from in stand alone mode
  Int_t   fFirstEvent;        // first event to process
  Int_t   fLastEvent;         // last  event to process
  // Control hists
  Int_t   fControlHists;          //!
  TList  *fHists;                 //!

  ClassDef(AliEMCALDigitizer,5)  // description 
};


#endif // AliEMCALDigitizer_H

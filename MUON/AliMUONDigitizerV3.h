/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup sim
/// \class AliMUONDigitizerV3
/// \brief Digitizer (from SDigit to Digit), performing digit de-calibration.
///
//  Author Laurent Aphecetche

#ifndef ALIMUONDIGITIZERV3_H
#define ALIMUONDIGITIZERV3_H

#ifndef ALIDIGITIZER_H
#include "AliDigitizer.h"
#endif

#ifndef ROOT_TStopwatch
#  include "TStopwatch.h"
#endif

class AliMUONCalibrationData;
class AliMUONVDigit;
class AliMUONLogger;
class AliMUONTriggerEfficiencyCells;
class TClonesArray;
class TF1;
class TString;
class AliMUONVDigitStore;
class AliLoader;
class AliMUONVTriggerStore;
class AliMUONTriggerElectronics;

class AliMUONDigitizerV3 : public AliDigitizer
{
public:
  AliMUONDigitizerV3(AliRunDigitizer* manager=0, 
                     Bool_t generateNoisyDigits=kTRUE);
  virtual ~AliMUONDigitizerV3();

  virtual void Exec(Option_t* opt="");
  
  virtual Bool_t Init();

private:
  /// Not implemented
  AliMUONDigitizerV3(const AliMUONDigitizerV3& other);
  /// Not implemented
  AliMUONDigitizerV3& operator=(const AliMUONDigitizerV3& other);
    
  void ApplyResponse(const AliMUONVDigitStore& store, AliMUONVDigitStore& filteredStore);

  void ApplyResponseToTrackerDigit(AliMUONVDigit& digit, Bool_t addNoise);
  void ApplyResponseToTriggerDigit(const AliMUONVDigitStore& digitStore, AliMUONVDigit& digit);

  AliLoader* GetLoader(const TString& foldername);
  
private:  

  AliMUONVDigit* FindCorrespondingDigit(const AliMUONVDigitStore& digitStore,
                                       AliMUONVDigit& digit) const;

  void GenerateNoisyDigits(AliMUONVDigitStore& digitStore);
  void GenerateNoisyDigitsForOneCathode(AliMUONVDigitStore& digitStore, 
                                        Int_t detElemId, Int_t cathode);

  void MergeWithSDigits(AliMUONVDigitStore*& digitStore,
                        const AliMUONVDigitStore& input,
                        Int_t mask);
  
private:
  Bool_t fIsInitialized; ///< are we initialized ?
  AliMUONCalibrationData* fCalibrationData; //!< pointer to access calib parameters
  AliMUONTriggerElectronics* fTriggerProcessor; ///< pointer to the trigger part of the job
  AliMUONTriggerEfficiencyCells* fTriggerEfficiency; ///< trigger efficiency map  
  TStopwatch fGenerateNoisyDigitsTimer; //!< counting time spent in GenerateNoisyDigits()
  TStopwatch fExecTimer; //!< couting time spent in Exec()  
  TF1* fNoiseFunction; //!< function to randomly get signal above n*sigma_ped
  Bool_t fGenerateNoisyDigits; //!< whether or not we should generate noise-only digits for tracker
  static const Double_t fgkNSigmas; ///< \brief number of sigmas above ped to use 
  /// for noise-only digit generation and zero-suppression
  AliMUONLogger* fLogger; //!< to keep track of messages
  AliMUONVTriggerStore* fTriggerStore; //!< trigger objects
  AliMUONVDigitStore* fDigitStore; //!< temporary digits
  AliMUONVDigitStore* fOutputDigitStore; //!< digits we'll output to disk
  
  ClassDef(AliMUONDigitizerV3,5) // MUON Digitizer V3-5
};

#endif

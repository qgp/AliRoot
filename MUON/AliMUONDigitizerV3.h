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

class AliMUONCalibrationData;
class AliMUONVDigit;
class AliMUONLogger;
class TClonesArray;
class TF1;
class TString;
class AliMUONVDigitStore;
class AliLoader;
class AliMUONVTriggerStore;
class AliMUONTriggerElectronics;
class AliMUONVCalibParam;

class AliMUONDigitizerV3 : public AliDigitizer
{
public:
  AliMUONDigitizerV3(AliRunDigitizer* manager=0, 
                     Int_t generateNoisyDigits=1);
  virtual ~AliMUONDigitizerV3();

  virtual void Exec(Option_t* opt="");
  
  virtual Bool_t Init();

  static Int_t DecalibrateTrackerDigit(const AliMUONVCalibParam& pedestals,
                                       const AliMUONVCalibParam& gains,
                                       Int_t channel,
                                       Float_t charge,
                                       Bool_t addNoise=kFALSE,
                                       Bool_t noiseOnly=kFALSE);
  
         /// Set calibration data
  void setCalibrationData(AliMUONCalibrationData* calibrationData) 
                          {fCalibrationData = calibrationData;}
  
private:
  /// Not implemented
  AliMUONDigitizerV3(const AliMUONDigitizerV3& other);
  /// Not implemented
  AliMUONDigitizerV3& operator=(const AliMUONDigitizerV3& other);
    
  void ApplyResponse(const AliMUONVDigitStore& store, AliMUONVDigitStore& filteredStore);

  void ApplyResponseToTrackerDigit(AliMUONVDigit& digit, Bool_t addNoise);

  AliLoader* GetLoader(const TString& foldername);
  
private:  

  void GenerateNoisyDigits(AliMUONVDigitStore& digitStore);
  void GenerateNoisyDigitsForOneCathode(AliMUONVDigitStore& digitStore, 
                                        Int_t detElemId, Int_t cathode);
  void GenerateNoisyDigitsForTrigger(AliMUONVDigitStore& digitStore);

  void MergeWithSDigits(AliMUONVDigitStore*& digitStore,
                        const AliMUONVDigitStore& input,
                        Int_t mask);
  
  static TF1* NoiseFunction();
  
private:
  Bool_t fIsInitialized; ///< are we initialized ?
  AliMUONCalibrationData* fCalibrationData; //!< pointer to access calib parameters
  AliMUONTriggerElectronics* fTriggerProcessor; ///< pointer to the trigger part of the job
  TF1* fNoiseFunctionTrig; //!< function to get noise disribution on trig. chambers
  Int_t fGenerateNoisyDigits; //!< whether or not we should generate noise-only digits for tracker (1) and trigger (2)
  static const Double_t fgkNSigmas; ///< \brief number of sigmas above ped to use 
  /// for noise-only digit generation and zero-suppression
  AliMUONLogger* fLogger; //!< to keep track of messages
  AliMUONVTriggerStore* fTriggerStore; //!< trigger objects
  AliMUONVDigitStore* fDigitStore; //!< temporary digits
  AliMUONVDigitStore* fOutputDigitStore; //!< digits we'll output to disk

  ClassDef(AliMUONDigitizerV3,7) // MUON Digitizer V3-5
};

#endif

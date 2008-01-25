#ifndef ALIPHOSRAWDECODER_H
#define ALIPHOSRAWDECODER_H
/* Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                          */

/* $Id$ */

// This class extracts the PHOS "digits" of current event
// (amplitude,time, position,gain) from the raw stream 
// provided by AliRawReader. See cxx source for use case.

#include "AliRawReader.h"
#include "AliCaloRawStream.h"

class TArrayI;

class AliPHOSRawDecoder {

public:

  AliPHOSRawDecoder();
  AliPHOSRawDecoder(AliRawReader* rawReader, AliAltroMapping **mapping = NULL);
  AliPHOSRawDecoder(const AliPHOSRawDecoder& rawDecoder);
  AliPHOSRawDecoder& operator = (const AliPHOSRawDecoder& rawDecoder);
  virtual ~AliPHOSRawDecoder();

  virtual Bool_t NextDigit();

  void SetOldRCUFormat(Bool_t isOldRCU) {fCaloStream->SetOldRCUFormat(isOldRCU);}
  void SubtractPedestals(Bool_t subtract) {fPedSubtract=subtract;}

  Double_t GetEnergy() { return fEnergy; }
  Double_t GetTime() { return fTime; }
  Double_t GetSampleQuality(){return fQuality ;}
  Int_t GetModule() { return fModule; }
  Int_t GetColumn() { return fColumn; }
  Int_t GetRow() { return fRow; }
  Bool_t IsLowGain() { return fLowGainFlag; }
  Bool_t IsOverflow(){ return fOverflow ;}

  const AliRawReader* GetRawReader() const { return fRawReader; }

protected:   
  
  AliRawReader* fRawReader;      // raw data reader
  AliCaloRawStream* fCaloStream; // PHOS/EMCAL raw data stream
  Bool_t fPedSubtract;           // pedestals subtraction (kTRUE="yes")


  Double_t fEnergy; // "digit" energy
  Double_t fTime;   // "digit" time
  Double_t fQuality ; //Sample quality
  Int_t fModule;    // PHOS module number (1-5)
  Int_t fColumn;    // column in the module
  Int_t fRow;       // row
  Int_t fNewModule;    // PHOS module number (1-5) of keeped sample
  Int_t fNewColumn;    // column in the module  of keeped sample
  Int_t fNewRow;       // row  of keeped sample
  Int_t fNewAmp ;      //Keeped amp
  Int_t fNewTime ;     //Time of keeped sample
  Bool_t fLowGainFlag;
  Bool_t fNewLowGainFlag;
  Bool_t fOverflow ;   //Wether there was overflow
  TArrayI* fSamples;   // array of samples
  TArrayI* fTimes ;    // array of times corresponding to samples

  ClassDef(AliPHOSRawDecoder,1)
};

#endif

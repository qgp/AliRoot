#ifndef ALISIGNAL_H
#define ALISIGNAL_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

#include "TObject.h"
#include "TArrayF.h"

#include "AliPosition.h"

class AliSignal : public TObject,public AliPosition
{
 public:
  AliSignal(Int_t n=1);                                 // Default constructor
  ~AliSignal();                                         // Destructor
  AliSignal(AliSignal& s);                              // Copy constructor
  virtual void SetSignal(Double_t sig,Int_t j=1);       // Store j-th signal value
  virtual void AddSignal(Double_t sig,Int_t j=1);       // Add value to j-th signal value
  virtual Float_t GetSignal(Int_t j=1);                 // Provide j-th signal value
  virtual void SetSignalError(Double_t dsig,Int_t j=1); // Store error on j-th signal value
  virtual Float_t GetSignalError(Int_t j=1);            // Provide error j-th signal value
  virtual void ResetSignals();                          // Reset all signal values and errors to 0
  virtual void ResetPosition();                         // Reset position and errors to 0
  virtual void Reset();                                 // Reset signal and pos. values and errors
  void Info(TString f="car");                           // Print signal info for coord. frame f
  void SetName(TString name);                           // Set the name tag to indicate the kind of signal
  TString GetName();                                    // Provide the name tag indicating the kind of signal
  Int_t GetNvalues();                                   // Provide the number of signal values

 protected:
  Int_t fNvalues;    // The number of values per signal
  TArrayF* fSignal;  // Signal values
  TArrayF* fDsignal; // Errors on signal values
  TString fName;     // Name tag to identify the kind of signal

 ClassDef(AliSignal,3) // Handling of ALICE (extrapolated) signals.
};
#endif

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
  virtual void ResetSignals(Int_t mode=0);              // User selected reset of signal values and/or errors
  virtual void DeleteSignals(Int_t mode=0);             // User selected delete of signal values and/or errors
  virtual void ResetPosition();                         // Reset position and errors to 0
  virtual void Reset(Int_t mode=0);                     // Reset signal and position values and errors
  void Data(TString f="car");                           // Print signal info for coord. frame f
  void SetName(TString name);                           // Set the name tag to indicate the kind of signal
  TString GetName();                                    // Provide the name tag indicating the kind of signal
  Int_t GetNvalues();                                   // Provide the number of signal values
  Int_t GetNerrors();                                   // Provide the number of specified errors

 protected:
  TArrayF* fSignal;  // Signal values
  TArrayF* fDsignal; // Errors on signal values
  TString fName;     // Name tag to identify the kind of signal

 ClassDef(AliSignal,6) // Generic handling of (extrapolated) detector signals.
};
#endif

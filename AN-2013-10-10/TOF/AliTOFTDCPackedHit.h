#ifndef ALITOFTDCPACKEDHIT_H
#define ALITOFTDCPACKEDHIT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTOFRawDataFormat.h 23881 2008-02-12 16:46:22Z decaro $ */

///////////////////////////////////////////////////////////////
//                                                           //
//   This classes provide the TOF raw data bit fields.       //
//                                                           //
///////////////////////////////////////////////////////////////

#include "TROOT.h"

class AliTOFTDCPackedHit
{
 public:
  UInt_t GetHitTime() const {return fHitTime;};
  UInt_t GetTOTWidth() const {return fTOTWidth;};
  UInt_t GetChan() const {return fChan;};
  UInt_t GetTDCID() const {return fTDCID;};
  UInt_t GetEBit() const {return fEBit;};
  UInt_t GetPSBits() const {return fPSBits;};
  UInt_t GetMBO() const {return fMBO;};
 private:
  UInt_t fHitTime:  13; // time-of-flight measurement
  UInt_t fTOTWidth:  8; // time-over-threshold measurement
  UInt_t fChan:      3; // TDC channel number
  UInt_t fTDCID:     4; // TDC ID
  UInt_t fEBit:      1; // E bit
  UInt_t fPSBits:    2; // PS bits
  UInt_t fMBO:       1; // must-be-zero bits
};

#endif

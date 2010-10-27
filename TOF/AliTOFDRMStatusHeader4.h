#ifndef ALITOFDRMSTATUSHEADER4_H
#define ALITOFDRMSTATUSHEADER4_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTOFRawDataFormat.h 23881 2008-02-12 16:46:22Z decaro $ */

///////////////////////////////////////////////////////////////
//                                                           //
//   This classes provide the TOF raw data bit fields.       //
//                                                           //
///////////////////////////////////////////////////////////////

#include "TROOT.h"

class AliTOFDRMStatusHeader4
{
 public:
  UInt_t GetSlotID() const {return fSlotID;};
  UInt_t GetTemperature() const {return fTemperature;};
  UInt_t GetMBZ1() const {return fMBZ1;};
  UInt_t GetACKBit() const {return fACKBit;};
  UInt_t GetSensAD() const {return fSensAD;};
  UInt_t GetMBZ2() const {return fMBZ2;};
  UInt_t GetUNDEFINED() const {return fUNDEFINED;};
  UInt_t GetWordType() const {return fWordType;};
 private:
  UInt_t fSlotID:      4;
  UInt_t fTemperature: 10;
  UInt_t fMBZ1:        1;
  UInt_t fACKBit:      1;
  UInt_t fSensAD:      3;
  UInt_t fMBZ2:        1;
  UInt_t fUNDEFINED:   8;
  UInt_t fWordType:    4;
};

#endif

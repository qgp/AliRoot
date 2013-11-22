#ifndef ALITOFDRMSTATUSHEADER1_H
#define ALITOFDRMSTATUSHEADER1_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTOFRawDataFormat.h 23881 2008-02-12 16:46:22Z decaro $ */

///////////////////////////////////////////////////////////////
//                                                           //
//   This classes provide the TOF raw data bit fields.       //
//                                                           //
///////////////////////////////////////////////////////////////

#include "TROOT.h"

class AliTOFDRMStatusHeader1
{
 public:
  UInt_t GetSlotID() const {return fSlotID;};
  UInt_t GetPartecipatingSlotID() const {return fPartecipatingSlotID;};
  UInt_t GetCBit() const {return fCBit;};
  UInt_t GetVersID() const {return fVersID;};
  UInt_t GetDRMhSize() const {return fDRMhSize;};
  UInt_t GetUNDEFINED() const {return fUNDEFINED;};
  UInt_t GetWordType() const {return fWordType;};
 private:
  UInt_t fSlotID:              4;
  UInt_t fPartecipatingSlotID: 11;
  UInt_t fCBit:                1;
  UInt_t fVersID:              5;
  UInt_t fDRMhSize:            4;
  UInt_t fUNDEFINED:           3;
  UInt_t fWordType:            4;
};

#endif

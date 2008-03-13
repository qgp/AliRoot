#ifndef ALIITSRAWSTREAMSSD_H
#define ALIITSRAWSTREAMSSD_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
///
/// This class provides access to ITS SSD digits in raw data.
//  Revised by Enrico Fragiacomo
//  Last update: 2007/09/06
///
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStream.h"

class AliRawReader;


class AliITSRawStreamSSD: public AliITSRawStream {
  public :
    AliITSRawStreamSSD(AliRawReader* rawReader);
    virtual ~AliITSRawStreamSSD() {};

    virtual Bool_t   Next();

    Int_t            GetSideFlag() const {return fCoord1;}
    Int_t            GetStrip() const {return fCoord2;}
    Int_t GetDDL() const {return fddl;}
    Int_t GetAD() const {return fad;}
    Int_t GetADC() const {return fadc;}

    void Setv11HybridDDLMapping();
    void SetvPPRasymmFMDDDLMapping();

    enum {kDDLsNumber = 16};      // number of DDLs in SSD
    enum {kModulesPerDDL = 108};  // number of modules in each DDL

    static Int_t     GetModuleNumber(UInt_t iDDL, UInt_t iModule)
      {return fgkDDLModuleMap[iDDL][iModule];}

    enum ESSDRawStreamError {
      kWrongModuleIdErr = 1
    };

    Int_t fddl;
    Int_t fad;
    Int_t fadc;

    Bool_t flag;

  protected :
    static Int_t fgkDDLModuleMap[kDDLsNumber][kModulesPerDDL];  // mapping DDL/module -> module number

    UInt_t           fData;         // data read for file

    ClassDef(AliITSRawStreamSSD, 0) // class for reading ITS SSD raw digits
};

#endif

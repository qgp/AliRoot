#ifndef ALISTRUCFUNCTYPE_H
#define ALISTRUCFUNCTYPE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TObject.h>

class AliStructFuncType : public TObject {
    
 public:
    AliStructFuncType(){;}
    virtual ~AliStructFuncType(){;}
    static void PdfSet(char parm[20][20], Double_t value[20]);
    static void StructA(Double_t xx, Double_t qq, Double_t a,
			Double_t& upv, Double_t& dnv, Double_t& usea,
			Double_t& dsea,
			Double_t& str, Double_t& chm, Double_t& bot,
			Double_t& top, Double_t& gl);
    ClassDef(AliStructFuncType,1) // Library for partonic energy loss
};

typedef enum
{
    kDOSet1     = 1006,
    kGRVLO      = 5005,
    kGRVHO      = 5006,
    kMRSDminus  = 3031,
    kMRSD0      = 3030,
    kMRSG       = 3041,
    kCTEQ2pM    = 4024,
    kCTEQ4L     = 4032,
    kCTEQ4M     = 4034,
    kMRSTcgLO   = 3072,
    kCTEQ5L     = 4046,
    kGRVLO98    = 5012
}
StrucFunc_t;

#endif


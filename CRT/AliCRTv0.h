#ifndef ALICRTV1_H
#define ALICRTV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//  Manager class for detector: CRTv0         //
////////////////////////////////////////////////

#include "AliCRT.h"


class AliCRTv0 : public AliCRT 
{
 
public:
                   AliCRTv0();
                   AliCRTv0(const char *name, const char *title);
   virtual         ~AliCRTv0() {}
   virtual void    CreateGeometry();
   virtual void    BuildGeometry();
   virtual void    CreateMaterials();
   virtual void    Init();
   virtual Int_t   IsVersion() const {return 0;}
   virtual void    DrawDetector();
   virtual void    DrawModule();
   virtual TString Version(void) {return TString("v0");}
   virtual void    StepManager();

private: 
  Int_t fMucur;

    ClassDef(AliCRTv0,1)  //Class for CRT, version 0

};

#endif

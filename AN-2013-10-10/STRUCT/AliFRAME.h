#ifndef ALIFRAME_H
#define ALIFRAME_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//  Manager class for detector: FRAME         //
////////////////////////////////////////////////
 
#include "AliModule.h"


class AliFRAME : public AliModule {
  
public:
  AliFRAME();
  AliFRAME(const char *name, const char *title);
  virtual      ~AliFRAME() {}
  virtual void   Init() {}
  virtual Int_t IsVersion() const =0;
 protected:
  Int_t fRefVolumeId1;    // Id of the reference volume
  Int_t fRefVolumeId2;    // Id of the reference volume
   ClassDef(AliFRAME,2)  //Class for Space Frame
};

#endif




#ifndef ALIFLATESDV0_H
#define ALIFLATESDV0_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 * Primary Authors : Sergey Gorbunov, Jochen Thaeder, Chiara Zampolli     */

/**
 * >> Flat structure representing a ESD vertex <<
 */

#include "Rtypes.h"
#include "AliFlatESDMisc.h"

class AliFlatESDV0
{
 public:
  AliFlatESDV0();
  virtual ~AliFlatESDV0() {}
  void Reinitialize(){
  	new (this) AliFlatESDV0(AliFlatESDReinitialize);
  }
  Int_t fNegTrackID;
  Int_t fPosTrackID;
 private:
 	AliFlatESDV0(AliFlatESDSpecialConstructorFlag){}
};
//typedef struct AliFlatESDV0 AliFlatESDV0;

#endif

#ifndef ALIPHOSRECOPARAM_H
#define ALIPHOSRECOPARAM_H
/* Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                          */

/* $Id$ */
                                              
// Base class for the PHOS reconstruction parameters.
// Do not use in the reconstruction; use derivative classes instead.

#include "TNamed.h"


class AliPHOSRecoParam : public TNamed {

public:

  AliPHOSRecoParam();
  AliPHOSRecoParam(const AliPHOSRecoParam& recoParam);
  AliPHOSRecoParam& operator = (const AliPHOSRecoParam& recoParam);
  virtual ~AliPHOSRecoParam() {}

  Float_t GetClusteringThreshold() const { return fClusteringThreshold; }
  Float_t GetLocalMaxCut() const { return fLocMaxCut;}
  Float_t GetMinE() const { return fMinE; }
  Float_t GetLogWeight() const { return fW0; }
  Bool_t  SubtractPedestals() const { return fSubtractPedestals; }
  Bool_t  ToUnfold() const { return fUnfold; }
  const char* DecoderVersion()const{ return fDecoderVersion.Data() ; }

  void SetClusteringThreshold(Float_t cluth) { fClusteringThreshold=cluth; }
  void SetLocalMaxCut(Float_t cut) { fLocMaxCut=cut;}
  void SetMinE(Float_t minE) { fMinE=minE; }
  void SetLogWeight(Float_t w) { fW0=w; }
  void SetSubtractPedestals(Bool_t subtract) { fSubtractPedestals=subtract; } 
  void SetDecoderVersion(const char* version="v1"){fDecoderVersion=version ;}
  void SetUnfolding(Bool_t toUnfold=kFALSE){fUnfold=toUnfold ;}

protected:

  Float_t fClusteringThreshold;
  Float_t fLocMaxCut;
  Float_t fMinE;
  Float_t fW0;
  Bool_t  fSubtractPedestals;
  Bool_t  fUnfold;
  TString fDecoderVersion ;

  ClassDef(AliPHOSRecoParam,1)
};

#endif

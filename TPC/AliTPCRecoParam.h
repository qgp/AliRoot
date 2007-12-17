#ifndef ALITPCRECOPARAM_H
#define ALITPCRECOPARAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class with TPC reconstruction parameters                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliDetectorRecoParam.h"

class AliTPCRecoParam : public AliDetectorRecoParam
{
 public: 
  AliTPCRecoParam();
  virtual ~AliTPCRecoParam();
  void     SetClusterSharing(Bool_t sharing){fBClusterSharing=sharing;}
  Bool_t   GetClusterSharing() const {return fBClusterSharing;}
  Double_t GetCtgRange() const     { return fCtgRange;}
  Double_t GetMaxSnpTracker() const{ return fMaxSnpTracker;}
  Double_t GetMaxSnpTrack() const  { return fMaxSnpTrack;}
  //
  Bool_t   DumpSignal()     const  { return fDumpSignal;}
  void     SetTimeInterval(Int_t first, Int_t last) { fFirstBin=first, fLastBin =last;}
  Int_t    GetFirstBin() const     { return fFirstBin;}
  Int_t    GetLastBin() const      { return fLastBin;}
  void     SetTimeBinRange(Int_t first, Int_t last){ fFirstBin = first; fLastBin = last;}
  Bool_t   GetCalcPedestal()       const  { return fBCalcPedestal;}
  Bool_t   GetDoUnfold()           const  { return fBDoUnfold;}
  void     SetDoUnfold(Bool_t unfold)     { fBDoUnfold = unfold;}
  Float_t  GetDumpAmplitudeMin()   const  { return fDumpAmplitudeMin;}
  Float_t  GetMaxNoise()           const  { return fMaxNoise;}  
  //
  Float_t  GetMinMaxCutAbs()       const  { return fMinMaxCutAbs; }
  Float_t  GetMinLeftRightCutAbs() const  { return fMinLeftRightCutAbs;}  // minimal amplitude left right - PRF
  Float_t  GetMinUpDownCutAbs()    const  { return fMinUpDownCutAbs;}  // minimal amplitude up-down - TRF 
  Float_t  GetMinMaxCutSigma()       const  { return fMinMaxCutSigma; }
  Float_t  GetMinLeftRightCutSigma() const  { return fMinLeftRightCutSigma;}  // minimal amplitude left right - PRF
  Float_t  GetMinUpDownCutSigma()    const  { return fMinUpDownCutSigma;}  // minimal amplitude up-down - TRF 
  //
  void SetMinMaxCutAbs(Float_t th)         {   fMinMaxCutAbs=th; }
  void SetMinLeftRightCutAbs(Float_t th)   {   fMinLeftRightCutAbs=th;}  // minimal amplitude left right - PRF
  void SetMinUpDownCutAbs(Float_t th)      {   fMinUpDownCutAbs=th;}  // minimal amplitude up-down - TRF 
  void SetMinMaxCutSigma(Float_t th)       {   fMinMaxCutSigma=th; }
  void SetMinLeftRightCutSigma(Float_t th) {   fMinLeftRightCutSigma=th;}  // minimal amplitude left right - PRF
  void SetMinUpDownCutSigma(Float_t th)    {   fMinUpDownCutSigma=th;}  // minimal amplitude up-down - TRF 
  //
  Int_t    GetLastSeedRowSec()       const  { return fLastSeedRowSec;} 
  void     SetDoKinks(Bool_t on)   { fBKinkFinder=on; }
  Bool_t   GetDoKinks() const      { return fBKinkFinder;}
  Float_t  GetMaxC()    const      { return fMaxC;}
  Bool_t   GetSpecialSeeding() const { return fBSpecialSeeding;}
  Bool_t   GetBYMirror() const { return fBYMirror;}
  void     SetBYMirror(Bool_t mirror)  { fBYMirror = mirror;} //
  static   AliTPCRecoParam *GetLowFluxParam();        // make reco parameters for low  flux env.
  static   AliTPCRecoParam *GetHighFluxParam();       // make reco parameters for high flux env. 
  static   AliTPCRecoParam *GetLaserTestParam(Bool_t bPedestal);  // special setting for laser 
  static   AliTPCRecoParam *GetCosmicTestParam(Bool_t bPedestal); // special setting for cosmic  
  //
 protected:
  Bool_t   fBClusterSharing; // allows or disable cluster sharing during tracking 
  Double_t fCtgRange;        // +-fCtgRange is the ctg(Theta) window used for clusterization and tracking (MI) 
  Double_t fMaxSnpTracker;   // max sin of local angle  - for TPC tracker
  Double_t fMaxSnpTrack;     // max sin of local angle  - for track 
  Bool_t   fBYMirror;        // mirror of the y - pad coordinate 
  //
  //   clusterer parameters
  //
  Bool_t   fDumpSignal;      // Dump Signal information flag
  Int_t    fFirstBin;        // first time bin used by cluster finder
  Int_t    fLastBin;         // last time bin  used by cluster finder 
  Bool_t   fBCalcPedestal;   // calculate Pedestal
  Bool_t   fBDoUnfold;       // do unfolding of clusters
  Float_t  fDumpAmplitudeMin; // minimal amplitude of signal to be dumped 
  Float_t  fMaxNoise;        // maximal noise sigma on pad to be used in cluster finder
  Float_t  fMinMaxCutAbs;    // minimal amplitude at cluster maxima
  Float_t  fMinLeftRightCutAbs;  // minimal amplitude left right - PRF
  Float_t  fMinUpDownCutAbs;  // minimal amplitude up-down - TRF 
  Float_t  fMinMaxCutSigma;    // minimal amplitude at cluster maxima
  Float_t  fMinLeftRightCutSigma;  // minimal amplitude left right - PRF
  Float_t  fMinUpDownCutSigma;  // minimal amplitude up-down - TRF 
  //
  //
  Float_t  fMaxC;            // maximal curvature for tracking
  Bool_t   fBSpecialSeeding; // special seeding with big inclination angles allowed (for Cosmic and laser)
  Bool_t   fBKinkFinder;     // do kink finder reconstruction
  Int_t    fLastSeedRowSec;     // Most Inner Row to make seeding for secondaries
  ClassDef(AliTPCRecoParam, 2)
};


#endif

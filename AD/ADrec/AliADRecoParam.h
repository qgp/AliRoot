/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#ifndef ALIADRECOPARAM_H
#define ALIADRECOPARAM_H

#include "AliDetectorRecoParam.h"

class AliADRecoParam : public AliDetectorRecoParam
{
 public: 
  AliADRecoParam();
  virtual ~AliADRecoParam();
  
  void SetNSigmaPed(Float_t nSigma) { fNSigmaPed = nSigma; }
  void SetStartClock(Int_t start) { fStartClock = start; }
  void SetEndClock(Int_t end) {fEndClock = end; }
  void SetNPreClocks(Int_t preClocks) { fNPreClocks = preClocks; }
  void SetNPostClocks(Int_t postClocks) { fNPostClocks = postClocks; }
  void SetAdcThresHold(Float_t val) { fAdcThresHold = val;}
  void SetTimeWindowBBALow(Float_t val) { fTimeWindowBBALow = val; }
  void SetTimeWindowBBAUp (Float_t val) { fTimeWindowBBAUp  = val; }
  void SetTimeWindowBGALow(Float_t val) { fTimeWindowBGALow = val; }
  void SetTimeWindowBGAUp (Float_t val) { fTimeWindowBGAUp  = val; }
  void SetTimeWindowBBCLow(Float_t val) { fTimeWindowBBCLow = val; }
  void SetTimeWindowBBCUp (Float_t val) { fTimeWindowBBCUp  = val; }
  void SetTimeWindowBGCLow(Float_t val) { fTimeWindowBGCLow = val; }
  void SetTimeWindowBGCUp (Float_t val) { fTimeWindowBGCUp  = val; }
  void SetMaxResid (Float_t val) { fMaxResid  = val; }
  void SetNTdcTimeBins (Int_t val) { fNTdcTimeBins = val; }
  void SetTdcTimeMin (Float_t val) { fTdcTimeMin = val; }
  void SetTdcTimeMax (Float_t val) { fTdcTimeMax = val; }
  void SetNTdcWidthBins (Int_t val) { fNTdcWidthBins = val; }
  void SetTdcWidthMin (Float_t val) { fTdcWidthMin = val; }
  void SetTdcWidthMax (Float_t val) { fTdcWidthMax = val; }
  void SetNChargeChannelBins (Int_t val) { fNChargeChannelBins = val; }
  void SetNChargeSideBins (Int_t val) { fNChargeSideBins = val; }
  void SetNChargeCorrBins (Int_t val) { fNChargeCorrBins = val; }

  Float_t GetNSigmaPed() const { return fNSigmaPed; }
  Int_t  GetStartClock() const { return fStartClock; }
  Int_t  GetEndClock() const { return fEndClock; }
  Int_t  GetNPreClocks() const { return fNPreClocks; }
  Int_t  GetNPostClocks() const { return fNPostClocks; }
  Float_t  GetAdcThresHold() const { return fAdcThresHold; }
  Float_t  GetTimeWindowBBALow() const { return fTimeWindowBBALow; }
  Float_t  GetTimeWindowBBAUp () const { return fTimeWindowBBAUp ; }
  Float_t  GetTimeWindowBGALow() const { return fTimeWindowBGALow; }
  Float_t  GetTimeWindowBGAUp () const { return fTimeWindowBGAUp ; }
  Float_t  GetTimeWindowBBCLow() const { return fTimeWindowBBCLow; }
  Float_t  GetTimeWindowBBCUp () const { return fTimeWindowBBCUp ; }
  Float_t  GetTimeWindowBGCLow() const { return fTimeWindowBGCLow; }
  Float_t  GetTimeWindowBGCUp () const { return fTimeWindowBGCUp ; }
  Float_t  GetMaxResid () const { return fMaxResid; } 
  Int_t GetNTdcTimeBins() const { return fNTdcTimeBins; }
  Float_t GetTdcTimeMin() const { return fTdcTimeMin; }
  Float_t GetTdcTimeMax() const { return fTdcTimeMax; }
  Int_t GetNTdcWidthBins() const { return fNTdcWidthBins; }
  Float_t GetTdcWidthMin() const { return fTdcWidthMin; }
  Float_t GetTdcWidthMax() const { return fTdcWidthMax; }
  Int_t GetNChargeChannelBins() const { return fNChargeChannelBins; }
  Int_t GetNChargeSideBins() const { return fNChargeSideBins; }
  Int_t GetNChargeCorrBins() const { return fNChargeCorrBins; }

 private:

  Float_t fNSigmaPed;  // Number of pedestal sigmas for adc cut
  Int_t fStartClock;   // Start clock for max adc search
  Int_t fEndClock;     // End clock for max adc search
  Int_t fNPreClocks;   // Number of pre-clocks used in adc charge sum
  Int_t fNPostClocks;  // Number of post-clocks used in adc charge sum
  
  // Cuts used in the trigger mask creation
  Float_t fAdcThresHold;      // Threshold on the ADC
  Float_t fTimeWindowBBALow;  // BBA window (lower cut)
  Float_t fTimeWindowBBAUp;   // BBA window (upper cut)
  Float_t fTimeWindowBGALow;  // BGA window (lower cut)
  Float_t fTimeWindowBGAUp;   // BGA window (upper cut)
  Float_t fTimeWindowBBCLow;  // BBC window (lower cut)
  Float_t fTimeWindowBBCUp;   // BBC window (upper cut)
  Float_t fTimeWindowBGCLow;  // BGC window (lower cut)
  Float_t fTimeWindowBGCUp;   // BGC window (upper cut)
  Float_t fMaxResid;   	      // Maximum residual of a single channel time
  
  //QA histogram bins and limits
  Int_t fNTdcTimeBins;	//Time bining
  Float_t fTdcTimeMin;	
  Float_t fTdcTimeMax;
  Int_t fNTdcWidthBins; //Width binning
  Float_t fTdcWidthMin;
  Float_t fTdcWidthMax;
  Int_t fNChargeChannelBins;	//Charge binnings
  Int_t fNChargeSideBins;
  Int_t fNChargeCorrBins;


  ClassDef(AliADRecoParam, 2)
};
#endif

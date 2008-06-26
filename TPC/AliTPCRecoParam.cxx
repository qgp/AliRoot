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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class with TPC reconstruction parameters                                  //
//                                                                           //  
//
/*
  The reconstruction parameters are used in the AliTPCclustererMI and AliTPCtrackerMI
  
  They are retrieved:
  0. User speciefied it in reconstruction macro
  1. if (not 0) from OCDB  - AliTPCcalibDB::GetRecoParam(eventtype)
  2. if (not 0 or 1) default parameter - High flux enevironment used  

  FIXME:
  In the future  reconstruction parameters should be changed on event basis
  But for the moment, event types are still not defined 


  // Setting for systematic errors addition
  [0] - systematic RMSY
  [1] - systematic RMSZ
  [2] - systematic RMSSNP
  [3] - systematic RMSTheta
  [4] - systematic RMSCuravture -  systematic error in 1/cm not in 1/pt
  //
  //  How to add it example - 3 mm systematic error y, 3 cm systematic error z (drift)
  Double_t sysError[5]={0.3,3, 0.3/150., 3./150.,1/(0.3*150*150.)}
  param->SetSystematicError(sysError);

*/
                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliTPCRecoParam.h"

ClassImp(AliTPCRecoParam)




//_____________________________________________________________________________
AliTPCRecoParam::AliTPCRecoParam():
  AliDetectorRecoParam(),
  fBClusterSharing(kTRUE),
  fCtgRange(1.05),       
  fMaxSnpTracker(0.95),
  fMaxSnpTrack(0.999),
  fDumpSignal(kFALSE),
  fFirstBin(0),
  fLastBin(-1),
  fBCalcPedestal(kFALSE),
  fBDoUnfold(kTRUE),
  fDumpAmplitudeMin(100),
  fMaxNoise(2.),
  fMinMaxCutAbs(5.),
  fMinLeftRightCutAbs(9.),
  fMinUpDownCutAbs(10.),
  fMinMaxCutSigma(4.),
  fMinLeftRightCutSigma(7.),
  fMinUpDownCutSigma(8.),
  fMaxC(0.3),
  fBSpecialSeeding(kFALSE),
  fBKinkFinder(kTRUE),
  fLastSeedRowSec(120)
{
  //
  // constructor
  //
  SetName("TPC");
  SetTitle("TPC");
  for (Int_t i=0;i<5;i++) fSystematicErrors[i]=0;
}

//_____________________________________________________________________________
AliTPCRecoParam::~AliTPCRecoParam() 
{
  //
  // destructor
  //  
}




AliTPCRecoParam *AliTPCRecoParam::GetLowFluxParam(){
  //
  // make default reconstruction  parameters for low  flux env.
  //
  AliTPCRecoParam *param = new AliTPCRecoParam;
  param->fCtgRange = 10;
  param->fFirstBin = 0;
  param->fLastBin  = 1000;
  return param;
}

AliTPCRecoParam *AliTPCRecoParam::GetHighFluxParam(){
  //
  // make reco parameters for high flux env.
  //
  AliTPCRecoParam *param = new AliTPCRecoParam;
  param->fCtgRange = 1.05;
  param->fFirstBin = 0;
  param->fLastBin  = 1000;  
  return param;
}

AliTPCRecoParam *AliTPCRecoParam::GetLaserTestParam(Bool_t bPedestal){
  //
  // special setting for laser
  //
  AliTPCRecoParam *param = new AliTPCRecoParam;
  param->fDumpSignal=kTRUE;
  param->fCtgRange = 10.05;
  param->fFirstBin = 0;
  param->fLastBin  = 1000;
  param->fBCalcPedestal = bPedestal;
  param->fBDoUnfold     = kFALSE;
  param->fDumpAmplitudeMin = 150;
  param->fBKinkFinder   = kFALSE;
  param->fMaxSnpTracker = 0.98;
  param->fMaxC          = 0.02;
  param->fBSpecialSeeding = kTRUE;
  //
  //
  return param;
}

AliTPCRecoParam *AliTPCRecoParam::GetCosmicTestParam(Bool_t bPedestal){
  //
  // special setting for cosmic 
  // 
  AliTPCRecoParam *param = new AliTPCRecoParam;
  param->fDumpSignal=kTRUE;
  param->fCtgRange = 10.05;    // full TPC
  param->fFirstBin = 60;
  param->fLastBin  = 1000;
  param->fBCalcPedestal = bPedestal;
  param->fBDoUnfold     = kFALSE;
  param->fBSpecialSeeding = kTRUE;
  param->fMaxC          = 0.07;
  param->fBKinkFinder   = kFALSE;
  return param;
}




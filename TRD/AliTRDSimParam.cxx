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

/* $Id$ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Class containing constant simulation parameters                        //
//                                                                        //
// Request an instance with AliTRDSimParam::Instance()                    //
// Then request the needed values                                         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TMath.h>

#include "AliRun.h"

#include "AliTRDSimParam.h"
#include "AliTRDCommonParam.h"
#include "AliLog.h"

ClassImp(AliTRDSimParam)

AliTRDSimParam *AliTRDSimParam::fgInstance   = 0;
Bool_t          AliTRDSimParam::fgTerminated = kFALSE;

//_ singleton implementation __________________________________________________
AliTRDSimParam* AliTRDSimParam::Instance()
{
  //
  // Singleton implementation
  // Returns an instance of this class, it is created if neccessary
  // 
  
  if (fgTerminated != kFALSE) {
    return 0;
  }

  if (fgInstance == 0) {
    fgInstance = new AliTRDSimParam();
  }  

  return fgInstance;

}

//_ singleton implementation __________________________________________________
void AliTRDSimParam::Terminate()
{
  //
  // Singleton implementation
  // Deletes the instance of this class and sets the terminated flag,
  // instances cannot be requested anymore
  // This function can be called several times.
  //
  
  fgTerminated = kTRUE;
  
  if (fgInstance != 0) {
    delete fgInstance;
    fgInstance = 0;
  }

}

//_____________________________________________________________________________
AliTRDSimParam::AliTRDSimParam()
  :TObject()
  ,fGasGain(0.0)
  ,fNoise(0.0)
  ,fChipGain(0.0)
  ,fADCoutRange(0.0)
  ,fADCinRange(0.0)
  ,fADCbaseline(0)
  ,fDiffusionOn(kFALSE)
  ,fElAttachOn(kFALSE)
  ,fElAttachProp(0.0)
  ,fTRFOn(kFALSE)
  ,fTRFsmp(0)
  ,fTRFbin(0)
  ,fTRFlo(0.0)
  ,fTRFhi(0.0)
  ,fTRFwid(0.0)
  ,fCTOn(kFALSE)
  ,fCTsmp(0)
  ,fPadCoupling(0.0)
  ,fTimeCoupling(0.0)
  ,fTimeStructOn(kFALSE)
  ,fPRFOn(kFALSE)
  ,fNTimeBins(0)
  ,fNTBoverwriteOCDB(kFALSE)
{
  //
  // Default constructor
  //
  
  Init();

}

//_____________________________________________________________________________
void AliTRDSimParam::Init()
{
  // 
  // Default initializiation
  //
  
  // The default parameter for the digitization
  fGasGain           = 4000.0;
  fChipGain          =   12.4;
  fNoise             = 1250.0;
  fADCoutRange       = 1023.0;          // 10-bit ADC
  fADCinRange        = 2000.0;          // 2V input range
  fADCbaseline       =   10;

  // Diffusion on
  fDiffusionOn       = kTRUE;
  
  // Propability for electron attachment
  fElAttachOn        = kFALSE;
  fElAttachProp      = 0.0;

  // The time response function
  fTRFOn             = kTRUE;

  // The cross talk
  fCTOn              = kTRUE;

  // The pad coupling factor
  // Use 0.46, instead of the theroetical value 0.3, since it reproduces better 
  // the test beam data, even tough it is not understood why.
  fPadCoupling       = 0.46;

  // The time coupling factor (same number as for the TPC)
  fTimeCoupling      = 0.4;

  // Use drift time maps
  fTimeStructOn      = kTRUE;
  
  // The pad response function
  fPRFOn             = kTRUE;

  // The number of time bins
  fNTimeBins         = 22;
  fNTBoverwriteOCDB  = kFALSE;

  ReInit();

}

//_____________________________________________________________________________
AliTRDSimParam::~AliTRDSimParam() 
{
  //
  // Destructor
  //
  
  if (fTRFsmp) {
    delete [] fTRFsmp;
    fTRFsmp = 0;
  }

  if (fCTsmp) {
    delete [] fCTsmp;
    fCTsmp  = 0;
  }

}

//_____________________________________________________________________________
AliTRDSimParam::AliTRDSimParam(const AliTRDSimParam &p)
  :TObject(p)
  ,fGasGain(p.fGasGain)
  ,fNoise(p.fNoise)
  ,fChipGain(p.fChipGain)
  ,fADCoutRange(p.fADCoutRange)
  ,fADCinRange(p.fADCinRange)
  ,fADCbaseline(p.fADCbaseline)
  ,fDiffusionOn(p.fDiffusionOn)
  ,fElAttachOn(p.fElAttachOn)
  ,fElAttachProp(p.fElAttachProp)
  ,fTRFOn(p.fTRFOn)
  ,fTRFsmp(0)
  ,fTRFbin(p.fTRFbin)
  ,fTRFlo(p.fTRFlo)
  ,fTRFhi(p.fTRFhi)
  ,fTRFwid(p.fTRFwid)
  ,fCTOn(p.fCTOn)
  ,fCTsmp(0)
  ,fPadCoupling(p.fPadCoupling)
  ,fTimeCoupling(p.fTimeCoupling)
  ,fTimeStructOn(p.fTimeStructOn)
  ,fPRFOn(p.fPRFOn)
  ,fNTimeBins(p.fNTimeBins)
  ,fNTBoverwriteOCDB(p.fNTBoverwriteOCDB)
{
  //
  // Copy constructor
  //

  Int_t iBin = 0;

  fTRFsmp = new Float_t[fTRFbin];
  for (iBin = 0; iBin < fTRFbin; iBin++) {
    fTRFsmp[iBin] = ((AliTRDSimParam &) p).fTRFsmp[iBin];
  }                                                                             

  fCTsmp  = new Float_t[fTRFbin];
  for (iBin = 0; iBin < fTRFbin; iBin++) {
    fCTsmp[iBin]  = ((AliTRDSimParam &) p).fCTsmp[iBin];
  }                                                                             

}

//_____________________________________________________________________________
AliTRDSimParam &AliTRDSimParam::operator=(const AliTRDSimParam &p)
{
  //
  // Assignment operator
  //

  if (this == &p) {
    return *this;
  }

  Init();

  fGasGain          = p.fGasGain;
  fNoise            = p.fNoise;
  fChipGain         = p.fChipGain;
  fADCoutRange      = p.fADCoutRange;
  fADCinRange       = p.fADCinRange;
  fADCbaseline      = p.fADCbaseline;
  fDiffusionOn      = p.fDiffusionOn;
  fElAttachOn       = p.fElAttachOn;
  fElAttachProp     = p.fElAttachProp;
  fTRFOn            = p.fTRFOn;
  fTRFsmp           = 0;
  fTRFbin           = p.fTRFbin;
  fTRFlo            = p.fTRFlo;
  fTRFhi            = p.fTRFhi;
  fTRFwid           = p.fTRFwid;
  fCTOn             = p.fCTOn;
  fCTsmp            = 0;
  fPadCoupling      = p.fPadCoupling;
  fTimeCoupling     = p.fTimeCoupling;
  fTimeStructOn     = p.fTimeStructOn;
  fPRFOn            = p.fPRFOn;
  fNTimeBins        = p.fNTimeBins;
  fNTBoverwriteOCDB = p.fNTBoverwriteOCDB;

  Int_t iBin = 0;

  if (fTRFsmp) {
    delete[] fTRFsmp;
  }
  fTRFsmp = new Float_t[fTRFbin];
  for (iBin = 0; iBin < fTRFbin; iBin++) {
    fTRFsmp[iBin] = ((AliTRDSimParam &) p).fTRFsmp[iBin];
  }                                                                             

  if (fCTsmp) {
    delete[] fCTsmp;
  }
  fCTsmp  = new Float_t[fTRFbin];
  for (iBin = 0; iBin < fTRFbin; iBin++) {
    fCTsmp[iBin]  = ((AliTRDSimParam &) p).fCTsmp[iBin];
  }                                                                             

  return *this;

}

//_____________________________________________________________________________
void AliTRDSimParam::Copy(TObject &p) const
{
  //
  // Copy function
  //
  
  AliTRDSimParam *target = dynamic_cast<AliTRDSimParam *> (&p);
  if (!target) {
    return;
  }

  target->fGasGain            = fGasGain;
  target->fNoise              = fNoise;
  target->fChipGain           = fChipGain;  
  target->fADCoutRange        = fADCoutRange;
  target->fADCinRange         = fADCinRange;
  target->fADCbaseline        = fADCbaseline; 
  target->fDiffusionOn        = fDiffusionOn; 
  target->fElAttachOn         = fElAttachOn;
  target->fElAttachProp       = fElAttachProp;
  target->fTRFOn              = fTRFOn;
  target->fTRFbin             = fTRFbin;
  target->fTRFlo              = fTRFlo;
  target->fTRFhi              = fTRFhi;
  target->fTRFwid             = fTRFwid;
  target->fCTOn               = fCTOn;
  target->fPadCoupling        = fPadCoupling;
  target->fTimeCoupling       = fTimeCoupling;
  target->fPRFOn              = fPRFOn;
  target->fNTimeBins          = fNTimeBins;
  target->fNTBoverwriteOCDB   = fNTBoverwriteOCDB;

  if (target->fTRFsmp) {
    delete[] target->fTRFsmp;
  }
  target->fTRFsmp = new Float_t[fTRFbin];
  for (Int_t iBin = 0; iBin < fTRFbin; iBin++) {
    target->fTRFsmp[iBin] = fTRFsmp[iBin];
  }

  if (target->fCTsmp) {
    delete[] target->fCTsmp;
  }
  target->fCTsmp  = new Float_t[fTRFbin];
  for (Int_t iBin = 0; iBin < fTRFbin; iBin++) {
    target->fCTsmp[iBin]  = fCTsmp[iBin];
  }
  
}

//_____________________________________________________________________________
void AliTRDSimParam::ReInit()
{
  //
  // Reinitializes the parameter class after a change
  //

  if      (AliTRDCommonParam::Instance()->IsXenon()) {
    // The range and the binwidth for the sampled TRF 
    fTRFbin = 200;
    // Start 0.2 mus before the signal
    fTRFlo  = -0.4;
    // End the maximum drift time after the signal 
    fTRFhi  =  3.58;
    // Standard gas gain
    fGasGain = 4000.0;
  }
  else if (AliTRDCommonParam::Instance()->IsArgon()) {
    // The range and the binwidth for the sampled TRF 
    fTRFbin  =  50;
    // Start 0.2 mus before the signal
    fTRFlo   =  0.02;
    // End the maximum drift time after the signal 
    fTRFhi   =  1.98;
    // Higher gas gain
    fGasGain = 8000.0;
  }
  else {
    AliFatal("Not a valid gas mixture!");
  }
  fTRFwid = (fTRFhi - fTRFlo) / ((Float_t) fTRFbin);

  // Create the sampled TRF
  SampleTRF();

}

//_____________________________________________________________________________
void AliTRDSimParam::SampleTRF()
{
  //
  // Samples the new time response function.
  //

  Int_t ipasa = 0;

  // Xenon
  // From Antons measurements with Fe55 source, adjusted by C. Lippmann.
  // time bins are -0.4, -0.38, -0.36, ...., 3.54, 3.56, 3.58 microseconds
  const Int_t kNpasa     = 200;  // kNpasa should be equal to fTRFbin!
  Float_t xtalk[kNpasa];
  Float_t signal[kNpasa]     = { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000
			       , 0.0002, 0.0007, 0.0026, 0.0089, 0.0253, 0.0612, 0.1319
			       , 0.2416, 0.3913, 0.5609, 0.7295, 0.8662, 0.9581, 1.0000
			       , 0.9990, 0.9611, 0.8995, 0.8269, 0.7495, 0.6714, 0.5987
			       , 0.5334, 0.4756, 0.4249, 0.3811, 0.3433, 0.3110, 0.2837
			       , 0.2607, 0.2409, 0.2243, 0.2099, 0.1974, 0.1868, 0.1776
			       , 0.1695, 0.1627, 0.1566, 0.1509, 0.1457, 0.1407, 0.1362
			       , 0.1317, 0.1274, 0.1233, 0.1196, 0.1162, 0.1131, 0.1102
			       , 0.1075, 0.1051, 0.1026, 0.1004, 0.0979, 0.0956, 0.0934
			       , 0.0912, 0.0892, 0.0875, 0.0858, 0.0843, 0.0829, 0.0815
			       , 0.0799, 0.0786, 0.0772, 0.0757, 0.0741, 0.0729, 0.0718
			       , 0.0706, 0.0692, 0.0680, 0.0669, 0.0655, 0.0643, 0.0630
			       , 0.0618, 0.0607, 0.0596, 0.0587, 0.0576, 0.0568, 0.0558
			       , 0.0550, 0.0541, 0.0531, 0.0522, 0.0513, 0.0505, 0.0497
			       , 0.0490, 0.0484, 0.0474, 0.0465, 0.0457, 0.0449, 0.0441
			       , 0.0433, 0.0425, 0.0417, 0.0410, 0.0402, 0.0395, 0.0388
			       , 0.0381, 0.0374, 0.0368, 0.0361, 0.0354, 0.0348, 0.0342
			       , 0.0336, 0.0330, 0.0324, 0.0318, 0.0312, 0.0306, 0.0301
			       , 0.0296, 0.0290, 0.0285, 0.0280, 0.0275, 0.0270, 0.0265
			       , 0.0260, 0.0256, 0.0251, 0.0246, 0.0242, 0.0238, 0.0233
			       , 0.0229, 0.0225, 0.0221, 0.0217, 0.0213, 0.0209, 0.0206
			       , 0.0202, 0.0198, 0.0195, 0.0191, 0.0188, 0.0184, 0.0181
			       , 0.0178, 0.0175, 0.0171, 0.0168, 0.0165, 0.0162, 0.0159
			       , 0.0157, 0.0154, 0.0151, 0.0148, 0.0146, 0.0143, 0.0140
			       , 0.0138, 0.0135, 0.0133, 0.0131, 0.0128, 0.0126, 0.0124
			       , 0.0121, 0.0119, 0.0120, 0.0115, 0.0113, 0.0111, 0.0109
			       , 0.0107, 0.0105, 0.0103, 0.0101, 0.0100, 0.0098, 0.0096
			       , 0.0094, 0.0092, 0.0091, 0.0089, 0.0088, 0.0086, 0.0084
			       , 0.0083, 0.0081, 0.0080, 0.0078 };
  signal[0] = 0.0;
  signal[1] = 0.0;
  signal[2] = 0.0;
  // With undershoot, positive peak corresponds to ~3% of the main signal:
  for (ipasa = 3; ipasa < kNpasa; ipasa++) {
    xtalk[ipasa] = 0.2 * (signal[ipasa-2] - signal[ipasa-3]);
  }
  xtalk[0]  = 0.0;   
  xtalk[1]  = 0.0;  
  xtalk[2]  = 0.0;  

  // Argon
  // Ar measurement with Fe55 source by Anton
  // time bins are 0.02, 0.06, 0.10, ...., 1.90, 1.94, 1.98 microseconds
  const Int_t kNpasaAr = 50;
  Float_t xtalkAr[kNpasaAr];
  Float_t signalAr[kNpasaAr] = { -0.01,  0.01,  0.00,  0.00,  0.01
                               , -0.01,  0.01,  2.15, 22.28, 55.53
                               , 68.52, 58.21, 40.92, 27.12, 18.49
                               , 13.42, 10.48,  8.67,  7.49,  6.55
                               ,  5.71,  5.12,  4.63,  4.22,  3.81
                               ,  3.48,  3.20,  2.94,  2.77,  2.63
                               ,  2.50,  2.37,  2.23,  2.13,  2.03
                               ,  1.91,  1.83,  1.75,  1.68,  1.63
                               ,  1.56,  1.49,  1.50,  1.49,  1.29
			       ,  1.19,  1.21,  1.21,  1.20,  1.10 };
  // Normalization to maximum
  for (ipasa = 0; ipasa < kNpasaAr; ipasa++) {
    signalAr[ipasa] /= 68.52;
  }
  signalAr[0] = 0.0;
  signalAr[1] = 0.0;
  signalAr[2] = 0.0;
  // With undershoot, positive peak corresponds to ~3% of the main signal:
  for (ipasa = 3; ipasa < kNpasaAr; ipasa++) {
    xtalkAr[ipasa] = 0.2 * (signalAr[ipasa-2] - signalAr[ipasa-3]);
  }
  xtalkAr[0]  = 0.0;   
  xtalkAr[1]  = 0.0;  
  xtalkAr[2]  = 0.0;  

  if (fTRFsmp) {
    delete [] fTRFsmp;
  }
  fTRFsmp = new Float_t[fTRFbin];

  if (fCTsmp)  {
    delete [] fCTsmp;
  }
  fCTsmp  = new Float_t[fTRFbin];

  if      (AliTRDCommonParam::Instance()->IsXenon()) {
    if (fTRFbin != kNpasa) {
      AliError("Array mismatch (xenon)\n\n");
    }
  }
  else if (AliTRDCommonParam::Instance()->IsArgon()) {
    if (fTRFbin != kNpasaAr) {
      AliError("Array mismatch (argon)\n\n");
    }
  }

  for (Int_t iBin = 0; iBin < fTRFbin; iBin++) {
    if      (AliTRDCommonParam::Instance()->IsXenon()) {
      fTRFsmp[iBin] = signal[iBin];
      fCTsmp[iBin]  = xtalk[iBin];
    }
    else if (AliTRDCommonParam::Instance()->IsArgon()) {
      fTRFsmp[iBin] = signalAr[iBin];
      fCTsmp[iBin]  = xtalkAr[iBin];
    }
  }

}

//_____________________________________________________________________________
Double_t AliTRDSimParam::TimeResponse(Double_t time) const
{
  //
  // Applies the preamp shaper time response
  // (We assume a signal rise time of 0.2us = fTRFlo/2.
  //

  Double_t rt   = (time - .5*fTRFlo) / fTRFwid;
  Int_t    iBin = (Int_t) rt; 
  Double_t dt   = rt - iBin; 
  if ((iBin >= 0) && (iBin+1 < fTRFbin)) {
    return fTRFsmp[iBin] + (fTRFsmp[iBin+1] - fTRFsmp[iBin])*dt;
  } 
  else {
    return 0.0;
  }

}

//_____________________________________________________________________________
Double_t AliTRDSimParam::CrossTalk(Double_t time) const
{
  //
  // Applies the pad-pad capacitive cross talk
  //

  Double_t rt   = (time - fTRFlo) / fTRFwid;
  Int_t    iBin = (Int_t) rt; 
  Double_t dt   = rt - iBin; 
  if ((iBin >= 0) && (iBin+1 < fTRFbin)) {
    return fCTsmp[iBin] + (fCTsmp[iBin+1] - fCTsmp[iBin])*dt;
  } 
  else {
    return 0.0;
  }

}

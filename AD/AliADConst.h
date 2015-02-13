#ifndef ALIADCONST_H
#define ALIADCONST_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */

#include "TROOT.h"

const Float_t kIntTimeRes = 0.39; // intrinsic time resolution of the scintillator
const Float_t kADOffset = -1488.6; // general AD offset between the TDCs and the trigger
const Int_t   kNClocks = 21; // Number of ADC clocks that are read out
const Float_t kChargePerADC = 0.6e-12; // Charge per ADC
const Int_t   kNPhotonsPerMIP = 30000;// Number of photons per MIP
const Int_t   kMinTDCWidth = 13; // minimum signal width measured by TDC
const Int_t   kMaxTDCWidth = 128; // maximum signal width measured by TDC
const Float_t kPMRespTime = 6.0; // PM response time (corresponds to 1.9 ns rise time)
const Float_t kPMTransparency = 0.25; // Transparency of the first dynode of the PM
const Float_t kPMNbOfSecElec = 6.0;   // Number of secondary electrons emitted from first dynode (per ph.e.)
const Float_t kPhotoCathodeEfficiency = 0.18; // Photocathode efficiency
const Int_t   kNCIUBoards = 2; //Number of CIU boards
/*				    |------------Cside------------|----------Aside-------|   */	
const Int_t   kOfflineChannel[16] = {15, 11, 14, 10, 13, 9, 12, 8, 7, 3, 6, 2, 5, 1, 4, 0};

#endif


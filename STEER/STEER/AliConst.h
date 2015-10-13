#ifndef ALICONST_H 
#define ALICONST_H 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TMath.h> 
  
#define  k2PI     TMath::TwoPi()
#define  kDegrad  TMath::DegToRad()
#define  kRaddeg  TMath::RadToDeg()

enum {
  kNDetectors = 18   // number of detectors    + AD +FIT //alla
};

static const char* fgkDetectorName[kNDetectors] = {"ITS", "TPC", "TRD",
"TOF", "PHOS", 
"HMPID", "EMCAL", "MUON", "FMD", "ZDC", "PMD", "T0", "VZERO", "ACORDE","AD","FIT","MFT", "HLT"};
//_____________________________________________________________________________
static Int_t   GetDetIndex(const char * detector)
{
  // return the detector index corresponding to detector
  Int_t index = -1 ; 
  for (index = 0; index < kNDetectors ; index++) {
    if ( strcmp(detector, fgkDetectorName[index]) == 0 )
	break ; 
  }	
  return index ; 
}
//_____________________________________________________________________________
static const char** GetDetectorNames() { return fgkDetectorName; }

#endif 

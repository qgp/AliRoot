/**************************************************************************
 * Copyright(c) 2004, ALICE Experiment at CERN, All rights reserved. *
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

//////////////////////////////////////////////////////////////////////////////
//                                                                          
// Concrete implementation of AliFMDSubDetector 
//
// This implements the geometry for FMD1 
//
//////////////////////////////////////////////////////////////////////////////
#ifndef ALIFMD1_H
# include "AliFMD1.h"
#endif 
#ifndef ROOT_TVirtualMC
# include <TVirtualMC.h>
#endif
#ifndef ALILOG_H
# include "AliLog.h"
#endif


//____________________________________________________________________
ClassImp(AliFMD1);

//____________________________________________________________________
AliFMD1::AliFMD1() 
  : AliFMDSubDetector(1) 
{}

//____________________________________________________________________
AliFMD1::~AliFMD1() 
{}

//____________________________________________________________________
void 
AliFMD1::SetupGeometry(Int_t airId, Int_t kaptionId) 
{
  fInnerHoneyLowR  = fInner->GetLowR() + 1;
  fInnerHoneyHighR = fInner->GetHighR() + 1;
  fOuterHoneyLowR  = 0;
  fOuterHoneyHighR = 0;

  Double_t par[3];
  par[0] = fInner->GetLowR();
  par[1] = fInnerHoneyHighR;
  par[2] = fDz = (fInner->GetLegLength() 
		  + fInner->GetSiThickness() 
		  + fInner->GetPrintboardThickness() 
		  + fInner->GetModuleSpacing() 
		  + fHoneycombThickness) / 2;
  fVolumeId = gMC->Gsvolu("FMD1", "TUBE", airId, par, 3);

  // Rotate the full sub-detector 
  gMC->Matrix(fRotationId, 270, 180, 90, 90, 180, 0); 

  AliFMDSubDetector::SetupGeometry(airId, kaptionId);
}

//____________________________________________________________________
void 
AliFMD1::Geometry(const char* mother, Int_t pbRotId, 
		  Int_t idRotId, Double_t z) 
{
  // The Z passed in isn't used. 
  z = fInnerZ + fDz;
  gMC->Gspos("FMD1", 1, mother, 0, 0, z, fRotationId);

  AliFMDSubDetector::Geometry("FMD1", pbRotId, idRotId, z);
}

  

//____________________________________________________________________
//
// EOF
//

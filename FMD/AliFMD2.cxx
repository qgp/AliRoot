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
// This implements the geometry for FMD2
//
//////////////////////////////////////////////////////////////////////////////
#ifndef ALIFMD2_H
# include "AliFMD2.h"
#endif 
#ifndef ROOT_TVirtualMC
# include <TVirtualMC.h>
#endif
#ifndef ALILOG_H
# include "AliLog.h"
#endif

//____________________________________________________________________
ClassImp(AliFMD2);

//____________________________________________________________________
AliFMD2::AliFMD2() 
  : AliFMDSubDetector(2) 
{}

//____________________________________________________________________
AliFMD2::~AliFMD2() 
{}

//____________________________________________________________________
void 
AliFMD2::SetupGeometry(Int_t airId, Int_t kaptionId) 
{
  fInnerHoneyLowR  = fInner->GetLowR() + 1;
  fInnerHoneyHighR = fInner->GetHighR() + 1;
  fOuterHoneyLowR  = fOuter->GetLowR() + 1;
  fOuterHoneyHighR = fOuter->GetHighR() + 1;

  Double_t par[3];
  par[0] = fInner->GetLowR();
  par[1] = fOuterHoneyHighR;
  par[2] = fDz = (TMath::Abs(fInnerZ - fOuterZ)
		  + fInner->GetSiThickness() 
		  + fInner->GetPrintboardThickness() 
		  + fInner->GetLegLength() 
		  + fInner->GetModuleSpacing() 
		  + fHoneycombThickness) / 2;
  fVolumeId = gMC->Gsvolu("FMD2", "TUBE", airId, par, 3);

  // Rotate the full sub-detector 
  gMC->Matrix(fRotationId, 270, 180, 90, 90, 180, 0); 

  AliFMDSubDetector::SetupGeometry(airId, kaptionId);
}

//____________________________________________________________________
void 
AliFMD2::Geometry(const char* mother, Int_t pbRotId, 
		  Int_t idRotId, Double_t z) 
{
  z = fDz + fOuterZ;
  AliFMDSubDetector::Geometry("FMD2", pbRotId, idRotId, z);
  gMC->Gspos("FMD2", 1, mother, 0, 0, z, fRotationId);  
}

  

//____________________________________________________________________
//
// EOF
//

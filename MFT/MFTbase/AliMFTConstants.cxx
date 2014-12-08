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

//====================================================================================================================================================
//
//      Constants for the Muon Forward Tracker
//
//      Contact author: antonio.uras@cern.ch
//
//====================================================================================================================================================

#include "TClass.h"
#include "AliMFTConstants.h"

ClassImp(AliMFTConstants)

const Double_t AliMFTConstants::fCutForAvailableDigits = 5.;
const Double_t AliMFTConstants::fCutForAttachingDigits = 1.;

const Double_t AliMFTConstants::fElossPerElectron = 3.62e-09;

const Double_t AliMFTConstants::fActiveSuperposition = 0.45;
                                 
const Double_t AliMFTConstants::fHeightActive = 1.3;
const Double_t AliMFTConstants::fHeightReadout = 0.3;

const Double_t AliMFTConstants::fSupportExtMargin = fHeightReadout + 0.3;

const Double_t AliMFTConstants::fRadLengthSi = 9.37;

const Double_t AliMFTConstants::fWidthChip = 3.0;

const Double_t AliMFTConstants::fPrecisionPointOfClosestApproach = 10.e-4;  // 10 micron

const Double_t AliMFTConstants::fZEvalKinem = 0.;

const Double_t AliMFTConstants::fXVertexTolerance = 500.e-4;    // 500 micron
const Double_t AliMFTConstants::fYVertexTolerance = 500.e-4;    // 500 micron

const Double_t AliMFTConstants::fPrimaryVertexResX = 5.e-4;   // 5 micron
const Double_t AliMFTConstants::fPrimaryVertexResY = 5.e-4;   // 5 micron
const Double_t AliMFTConstants::fPrimaryVertexResZ = 5.e-4;   // 5 micron

const Double_t AliMFTConstants::fMisalignmentMagnitude = 15.e-4;    // 15 micron

//====================================================================================================================================================

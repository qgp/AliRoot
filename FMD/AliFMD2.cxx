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

//____________________________________________________________________
//                                                                          
// Concrete implementation of AliFMDDetector 
//
// This implements the geometry for FMD2
//
#include "AliFMD2.h"		// ALIFMD2_H 
#include "AliFMDRing.h"		// ALIFMDRING_H 

//====================================================================
ClassImp(AliFMD2)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliFMD2::AliFMD2(AliFMDRing* inner, AliFMDRing* outer) 
  : AliFMDDetector(2, inner, outer)
{
  SetInnerZ(83.4);
  SetOuterZ(75.2);
}


//____________________________________________________________________
void
AliFMD2::Init() 
{
  AliFMDDetector::Init();
  SetInnerHoneyHighR(GetOuterHoneyHighR());
}

//____________________________________________________________________
//
// EOF
//

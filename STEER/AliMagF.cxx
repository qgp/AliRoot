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

//----------------------------------------------------------------------
// Basic magnetic field class
// Used in all the detectors, and also in the traking classes
// Author:
//----------------------------------------------------------------------

#include "AliMagF.h"

ClassImp(AliMagF)

//_______________________________________________________________________
AliMagF::AliMagF():
  fMap(0),
  fType(0),
  fInteg(0),
  fFactor(0),
  fMax(0),
  fDebug(0)
{
  //
  // Default constructor
  //
}

//_______________________________________________________________________
AliMagF::AliMagF(const char *name, const char *title, const Int_t integ, 
                 const Float_t factor, const Float_t fmax):
  TNamed(name,title),
  fMap(0),
  fType(0),
  fInteg(0),
  fFactor(factor),
  fMax(fmax),
  fDebug(0)
{
  //
  // Standard constructor
  //
    if(integ<0 || integ > 2) {
      Warning("SetField",
              "Invalid magnetic field flag: %5d; Helix tracking chosen instead\n"
              ,integ);
      fInteg = 2;
    } else {
      fInteg = integ;
    }
    fType = kUndef;
    //
    fDebug = 0;
}

//_______________________________________________________________________
void AliMagF::Field(Float_t*, Float_t *b)
{
  //
  // Method to return the field in one point -- dummy in this case
  //
  Warning("Field","Undefined MagF Field called, returning 0\n");
  b[0]=b[1]=b[2]=0;
}

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

#include "TVirtualMC.h"

#include "AliRun.h"
#include "AliTrackReference.h"

// 
// Track Reference object is created every time particle is 
// crossing detector bounds. The object is created by Step Manager
//
// The class stores the following informations:
// track label, 
// track position: X,Y,X
// track momentum px, py, pz
// track length and time of fligth: both in cm
// status bits from Monte Carlo
//


ClassImp(AliTrackReference)

//_______________________________________________________________________
 AliTrackReference::AliTrackReference():
   TObject(),
   fTrack(0),
   fX(0),
   fY(0),
   fZ(0),
   fPx(0),
   fPy(0),
   fPz(0),
   fLength(0),
   fTime(0)
{
  //
  // Default constructor
  // Creates empty object

  for(Int_t i=0; i<16; i++) ResetBit(BIT(i));
}

//_______________________________________________________________________
AliTrackReference::AliTrackReference(Int_t label) :
  TObject(),
  fTrack(label),
  fX(0),
  fY(0),
  fZ(0),
  fPx(0),
  fPy(0),
  fPz(0),
  fLength(gMC->TrackLength()),
  fTime(gMC->TrackTime())
{
  //
  // Create Reference object out of label and
  // data in TVirtualMC object
  //
  // Creates an object and fill all parameters 
  // from data in VirtualMC
  //
  // Sylwester Radomski, (S.Radomski@gsi.de)
  // GSI, Jan 31, 2003
  //
    
  Double_t vec[4];
  
  gMC->TrackPosition(vec[0],vec[1],vec[2]);

  fX = vec[0];
  fY = vec[1];
  fZ = vec[2];
  
  gMC->TrackMomentum(vec[0],vec[1],vec[2],vec[3]);
  
  fPx = vec[0];
  fPy = vec[1];
  fPz = vec[2];

  // Set Up status code 
  // Copy Bits from virtual MC

  for(Int_t i=0; i<16; i++) ResetBit(BIT(i));

  SetBit(BIT(0), gMC->IsNewTrack());
  SetBit(BIT(1), gMC->IsTrackAlive());
  SetBit(BIT(2), gMC->IsTrackDisappeared());
  SetBit(BIT(3), gMC->IsTrackEntering());
  SetBit(BIT(4), gMC->IsTrackExiting());
  SetBit(BIT(5), gMC->IsTrackInside());
  SetBit(BIT(6), gMC->IsTrackOut());
  SetBit(BIT(7), gMC->IsTrackStop()); 
}
//_______________________________________________________________________

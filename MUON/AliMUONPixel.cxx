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

#include "AliMUONPixel.h"

ClassImp(AliMUONPixel) // Class implementation in ROOT context

//_____________________________________________________________________________
AliMUONPixel::AliMUONPixel(Double_t xc, Double_t yc, Double_t wx, Double_t wy, Double_t charge)
{
  // Constructor
  fXY[0] = xc; fXY[1] = yc; fSize[0] = wx; fSize[1] = wy; fCharge = charge;
}

//_____________________________________________________________________________
AliMUONPixel::~AliMUONPixel()
{
  // Destructor
}

//__________________________________________________________________________
Int_t AliMUONPixel::Compare(const TObject* pixel) const
{
  // "Compare" function to sort with decreasing pixel charge.
  // Returns -1 (0, +1) if charge of current pixel
  // is greater than (equal to, less than) charge of pixel
  if (fCharge > ((AliMUONPixel*)pixel)->Charge()) return(-1);
  else if (fCharge == ((AliMUONPixel*)pixel)->Charge()) return( 0);
  else return(+1);
}

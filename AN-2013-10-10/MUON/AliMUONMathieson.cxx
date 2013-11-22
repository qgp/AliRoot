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

//-----------------------------------------------------------------------------
// Class AliMUONMathieson
// -----------------------
// Implementation of Mathieson response
// Separated from other classes by CH. Finck with removing circular
// dependencies 
//-----------------------------------------------------------------------------

#include "AliMUONMathieson.h"

#include "AliLog.h"

#include <TClass.h>
#include <TMath.h>
#include <TRandom.h>

/// \cond CLASSIMP
ClassImp(AliMUONMathieson)
/// \endcond
	
//__________________________________________________________________________
AliMUONMathieson::AliMUONMathieson() :
    fSqrtKx3(0.),
    fKx2(0.),
    fKx4(0.),
    fSqrtKy3(0.),
    fKy2(0.),
    fKy4(0.),
    fPitch(0.),
    fInversePitch(0.)
{
/// Default constructor

}

//__________________________________________________________________________
AliMUONMathieson::~AliMUONMathieson()
{
/// Destructor
}

  //__________________________________________________________________________
void AliMUONMathieson::SetSqrtKx3AndDeriveKx2Kx4(Float_t SqrtKx3)
{
/// Set to "SqrtKx3" the Mathieson parameter K3 ("fSqrtKx3")
/// in the X direction, perpendicular to the wires,
/// and derive the Mathieson parameters K2 ("fKx2") and K4 ("fKx4")
/// in the same direction
  fSqrtKx3 = SqrtKx3;
  fKx2 = TMath::Pi() / 2. * (1. - 0.5 * fSqrtKx3);
  Float_t cx1 = fKx2 * fSqrtKx3 / 4. / TMath::ATan(Double_t(fSqrtKx3));
  fKx4 = cx1 / fKx2 / fSqrtKx3;
}
	
  //__________________________________________________________________________
void AliMUONMathieson::SetSqrtKy3AndDeriveKy2Ky4(Float_t SqrtKy3)
{
/// Set to "SqrtKy3" the Mathieson parameter K3 ("fSqrtKy3")
/// in the Y direction, along the wires,
/// and derive the Mathieson parameters K2 ("fKy2") and K4 ("fKy4")
/// in the same direction
  fSqrtKy3 = SqrtKy3;
  fKy2 = TMath::Pi() / 2. * (1. - 0.5 * fSqrtKy3);
  Float_t cy1 = fKy2 * fSqrtKy3 / 4. / TMath::ATan(Double_t(fSqrtKy3));
  fKy4 = cy1 / fKy2 / fSqrtKy3;
}

//_____________________________________________________________________________
Float_t
AliMUONMathieson::IntXY(Float_t xi1, Float_t yi1, Float_t xi2, Float_t yi2) const
{
/// Integrate the Mathieson over x and y

  xi1 *= fInversePitch;
  xi2 *= fInversePitch;
  yi1 *= fInversePitch;
  yi2 *= fInversePitch;
  //
  // The Mathieson function 
  Double_t ux1=fSqrtKx3*TMath::TanH(fKx2*xi1);
  Double_t ux2=fSqrtKx3*TMath::TanH(fKx2*xi2);
  
  Double_t uy1=fSqrtKy3*TMath::TanH(fKy2*yi1);
  Double_t uy2=fSqrtKy3*TMath::TanH(fKy2*yi2);
  
  
  return Float_t(4.*fKx4*(TMath::ATan(ux2)-TMath::ATan(ux1))*
                 fKy4*(TMath::ATan(uy2)-TMath::ATan(uy1)));
}

//______________________________________________________________________________
void 
AliMUONMathieson::SetPitch(Float_t p1)
{
/// Defines the pitch, and store its inverse, which is what is used in fact.

  fPitch = p1;
  if ( fPitch )
  {
    fInversePitch = 1/fPitch;
  }
  else
  {
    AliError(Form("Invalid pitch %e",p1));
    fInversePitch = 0.0;
  }
}


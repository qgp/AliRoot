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

/*
  $Log$
  Revision 1.3  2002/10/29 14:15:13  morsch
  omega, theta, phi added (J. Barbosa)

  Revision 1.2  2001/05/10 12:33:24  jbarbosa
  Added mean radius data member.

  Revision 1.1  2000/11/01 15:31:56  jbarbosa
  New rec. point object for 3D Hough reconstruction.

  Revision 1.2  2000/06/30 16:36:58  dibari
  Several new data members for Bari reconstruction

  Revision 1.1  2000/06/12 15:28:06  jbarbosa
  Cleaned up version.

*/


#include "AliRICHRecHit3D.h"
 
ClassImp(AliRICHRecHit3D)

AliRICHRecHit3D::AliRICHRecHit3D(Int_t id, Float_t *rechit, Float_t omega, Float_t theta, Float_t phi)
{
    //
    // Creates a RICH rec. hit object
    //
    fTheta        = rechit[0];
    fPhi          = rechit[1];
    fOmega        = rechit[2];
    fX            = rechit[3];
    fY            = rechit[4];
    fMeanRadius   = rechit[5];
    fOriginalOmega = omega;
    fOriginalTheta = theta;
    fOriginalPhi = phi;
}



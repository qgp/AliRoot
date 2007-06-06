/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id:*/

//-------------------------------------------------------------------------
//     AOD track base class
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include "AliAODPhoton.h"

ClassImp(AliAODPhoton)


//______________________________________________________________________________
AliAODPhoton::AliAODPhoton() 
{
  // constructor
}

//______________________________________________________________________________
AliAODPhoton::~AliAODPhoton() 
{
  // destructor
}

//______________________________________________________________________________
AliAODPhoton::AliAODPhoton(const AliAODPhoton& photon) :
  AliVirtualParticle(photon)
{
  // Copy constructor

}

//______________________________________________________________________________
AliAODPhoton& AliAODPhoton::operator=(const AliAODPhoton& photon)
{
  // Assignment operator
  if(this!=&photon) {
  }

  return *this;
}


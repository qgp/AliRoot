// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveHOMERSource.h"

//______________________________________________________________________________
// AliEveHOMERSource
//

ClassImp(AliEveHOMERSource)

AliEveHOMERSource::AliEveHOMERSource(const Text_t* n, const Text_t* t) :
  TEveElement (),
  TNamed      (n, t),
  fSrcId      (0),
  fSrcState   (0)
{}

/******************************************************************************/

//______________________________________________________________________________
#if ROOT_VERSION_CODE <= 332548
void AliEveHOMERSource::SetRnrState(Bool_t rnr)
#else
Bool_t AliEveHOMERSource::SetRnrState(Bool_t rnr)
#endif
{
   // Set render state of this element and of its children to the same
   // value.

  if (fSrcState)
    fSrcState->fState = rnr;

#if ROOT_VERSION_CODE <= 332548
  return;
#else
  return TEveElement::SetRnrState(rnr);
#endif
}

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
//_________________________________________________________________________
//  Algorithm class used only by AliPHOSTrackSegmentMaker 
//  Links recpoints into tracksegments
//  Why should I put meaningless comments
//  just to satisfy
//  the code checker                                
//*-- Author: Dmitri Peressounko (SUBATECH)

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliPHOSLink.h"

ClassImp(AliPHOSLink)
//____________________________________________________________________________
  AliPHOSLink::AliPHOSLink() : 
    fEmcN(-1), fCpvN(-1), fR(-1.), fTrack(-1)
{
}

//____________________________________________________________________________
  AliPHOSLink::AliPHOSLink(Float_t r, Int_t emc, Int_t cpv, Int_t track)
{
  // ctor

  fR     = r ;  
  fEmcN  = emc ;
  fCpvN  = cpv ;   
  fTrack = track ; 
}

//____________________________________________________________________________
Int_t AliPHOSLink::Compare(const TObject * obj) const
{
  // Compare according to the distance between EMC and CPV RecPoints in a track segment 

  Int_t rv ;

  AliPHOSLink * link = (AliPHOSLink *) obj ;

  if(this->fR < link->GetR() ) 
    rv = -1 ;
  else 
    rv = 1 ;

  return rv ;
}

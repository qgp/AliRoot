/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpStationType.h,v 1.9 2006/05/24 13:58:07 ivana Exp $

//-----------------------------------------------------------------------------
// \enum AliMpStationType
// Enumeration for refering to a MUON station
//
// Author: David Guez, Ivana Hrivnacova; IPN Orsay
//-----------------------------------------------------------------------------
 
#include "AliMpStationType.h"

#include "AliLog.h" 

//_____________________________________________________________________________
TString AliMp::StationTypeName(AliMp::StationType stationType)
{
/// Return the station name for given station type

  switch ( stationType ) {
    case kStation12:      return "st12";    break;
    case kStation345:     return "slat";    break;
    case kStationTrigger: return "trigger"; break;
  }
  
  // Cannot reach this line
  AliFatalGeneral("AliMpStationType.h", "Unknown station type"); 
  return "invalidStation";
}

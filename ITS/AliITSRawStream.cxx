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

///////////////////////////////////////////////////////////////////////////////
//
// This is a base class for reading ITS raw data files and providing
// information about digits
//
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStream.h"

ClassImp(AliITSRawStream)


AliITSRawStream::AliITSRawStream()
{
// create an object to read ITS raw digits

  fModuleID = fPrevModuleID = fCoord1 = fCoord2 = fSignal = -1;
}

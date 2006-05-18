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
///
/// This class provides access to MUON digits in raw data.
///
/// It loops over all MUON digits in the raw data given by the AliRawReader.
/// The Next method goes to the next digit. If there are no digits left
/// it returns kFALSE.
/// 
/// First version implement only the structure for the moment
///
///////////////////////////////////////////////////////////////////////////////

#include "AliMUONRawStream.h"

#include "AliRawReader.h"
#include "AliLog.h"


ClassImp(AliMUONRawStream)


AliMUONRawStream::AliMUONRawStream(AliRawReader* rawReader)
{
// create an object to read MUON raw digits

  fRawReader = rawReader;
  fRawReader->Select(0);
  fData = new UShort_t[fgkDataMax];
  fDataSize = fPosition = 0;
  fCount = 0;

}

AliMUONRawStream::AliMUONRawStream(const AliMUONRawStream& stream) :
  TObject(stream)
{
  AliFatal("copy constructor not implemented");
}

AliMUONRawStream& AliMUONRawStream::operator = (const AliMUONRawStream& 
					      /* stream */)
{
  AliFatal("assignment operator not implemented");
  return *this;
}

AliMUONRawStream::~AliMUONRawStream()
{
// clean up

  delete[] fData;
}


Bool_t AliMUONRawStream::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left

  AliFatal("method not implemented for raw data input");


  return kFALSE;
}



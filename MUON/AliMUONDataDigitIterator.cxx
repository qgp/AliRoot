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

// $Id$

#include "AliMUONDataDigitIterator.h"

#include "AliMUONData.h"
#include "AliMUONDigit.h"
#include "TClonesArray.h"

///
/// An iterator to access digits (stored into AliMUONData).
///
/// Iteration can occur on tracking chambers only, trigger chambers only,
/// or both.
///

//_____________________________________________________________________________
AliMUONDataDigitIterator::AliMUONDataDigitIterator(const AliMUONData* data,
                                                   Int_t firstChamber, 
                                                   Int_t lastChamber)
: 
AliMUONVDataIterator(),
fData(data),
fFirstChamber(firstChamber),
fLastChamber(lastChamber)
{
  // 
  // Ctor
  // 
  Reset();
}

//_____________________________________________________________________________
AliMUONDataDigitIterator::AliMUONDataDigitIterator(const AliMUONDataDigitIterator& rhs)
: 
AliMUONVDataIterator()
{
  rhs.CopyTo(*this);
}

//_____________________________________________________________________________
AliMUONDataDigitIterator&
AliMUONDataDigitIterator::operator=(const AliMUONDataDigitIterator& rhs)
{
  rhs.CopyTo(*this);
  return *this;
}

//_____________________________________________________________________________
void
AliMUONDataDigitIterator::CopyTo(AliMUONDataDigitIterator& destination) const
{
  destination.fData=fData;
  destination.fFirstChamber=fFirstChamber;
  destination.fLastChamber=fLastChamber;
  destination.fCurrentDigit=fCurrentDigit;
  destination.fCurrentChamber=fCurrentChamber;
  destination.fDigits=fDigits;
}

//_____________________________________________________________________________
TObject*
AliMUONDataDigitIterator::Next()
{
  // Return current element and self-position to the next one.
  
  TObject* rv(0x0);
  
  if ( fDigits ) 
  {
    // get the pointer to be returned
    rv = fDigits->At(fCurrentDigit);
    // prepare for the next position, if it exists
    if ( fCurrentDigit < fDigits->GetLast() ) 
    {
      ++fCurrentDigit;
    }
    else
    {
      fCurrentDigit = 0;
      ++fCurrentChamber;
      if ( fCurrentChamber <= fLastChamber )
      {
        fDigits = fData->Digits(fCurrentChamber);
      }
      else
      {
        fDigits = 0x0;
      }
    }
  }
  
  return rv;
}

//_____________________________________________________________________________
Bool_t
AliMUONDataDigitIterator::Remove()
{
  // Remove current element.
  
  if ( fDigits ) 
  {
    fDigits->RemoveAt(fCurrentDigit);
    fDigits->Compress();
    return kTRUE;
  }
  
  return kFALSE;
}

//_____________________________________________________________________________
void
AliMUONDataDigitIterator::Reset()
{
  fData->GetDigits();
  fCurrentDigit = 0;
  fCurrentChamber = fFirstChamber;
  fDigits = fData->Digits(fCurrentChamber);
}

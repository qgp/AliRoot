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
// This is a class for reading a raw data from a root file and providing
// information about digits
//
///////////////////////////////////////////////////////////////////////////////

#include "AliRawReaderRoot.h"


ClassImp(AliRawReaderRoot)


AliRawReaderRoot::AliRawReaderRoot(const char* fileName, Int_t eventNumber)
{
// create an object to read digits from the given input file for the
// event with the given number

  fFile = TFile::Open(fileName);
  if (!fFile || !fFile->IsOpen()) {
    Error("AliRawReaderRoot", "could not open file %s", fileName);
    return;
  }
  TTree* tree = (TTree*) fFile->Get("RAW");
  if (!tree) {
    Error("AliRawReaderRoot", "no raw data tree found");
    return;
  }
  TBranch* branch = tree->GetBranch("rawevent");
  if (!branch) {
    Error("AliRawReaderRoot", "no raw data branch found");
    return;
  }

  fEvent = new AliRawEvent;
  branch->SetAddress(&fEvent);
  if (branch->GetEntry(eventNumber) <= 0) {
    Error("AliRawReaderRoot", "no event with number %d found", eventNumber);
    return;
  }
  
  fSubEventIndex = 0;
  fSubEvent = NULL;
  fRawData = NULL;
  fMiniHeader = NULL;

  fCount = 0;
  fPosition = fEnd = NULL;
}

AliRawReaderRoot::AliRawReaderRoot(AliRawEvent* event)
{
// create an object to read digits from the given raw event

  fFile = NULL;
  fEvent = event;
  
  fSubEventIndex = 0;
  fSubEvent = NULL;
  fRawData = NULL;
  fMiniHeader =  NULL;

  fCount = 0;
  fPosition = fEnd = NULL;
}

AliRawReaderRoot::~AliRawReaderRoot()
{
// delete objects and close root file

  if (fFile) {
    if (fEvent) delete fEvent;
    fFile->Close();
    delete fFile;
  }
}


UInt_t AliRawReaderRoot::GetType()
{
// get the type from the event header

  if (!fEvent) return 0;
  return fEvent->GetHeader()->GetType();
}

UInt_t AliRawReaderRoot::GetRunNumber()
{
// get the run number from the event header

  if (!fEvent) return 0;
  return fEvent->GetHeader()->GetRunNumber();
}

const UInt_t* AliRawReaderRoot::GetEventId()
{
// get the event id from the event header

  if (!fEvent) return NULL;
  return fEvent->GetHeader()->GetId();
}

const UInt_t* AliRawReaderRoot::GetTriggerPattern()
{
// get the trigger pattern from the event header

  if (!fEvent) return NULL;
  return fEvent->GetHeader()->GetTriggerPattern();
}

const UInt_t* AliRawReaderRoot::GetDetectorPattern()
{
// get the detector pattern from the event header

  if (!fEvent) return NULL;
  return fEvent->GetHeader()->GetDetectorPattern();
}

const UInt_t* AliRawReaderRoot::GetAttributes()
{
// get the type attributes from the event header

  if (!fEvent) return NULL;
  return fEvent->GetHeader()->GetTypeAttribute();
}

UInt_t AliRawReaderRoot::GetGDCId()
{
// get the GDC Id from the event header

  if (!fEvent) return 0;
  return fEvent->GetHeader()->GetGDCId();
}


Bool_t AliRawReaderRoot::ReadMiniHeader()
{
// read a mini header at the current position
// returns kFALSE if the mini header could not be read

  if (!fEvent) return kFALSE;
  do {
    if (fCount > 0) fPosition += fCount;      // skip payload if event was not selected
    if (!fSubEvent || (fPosition >= fEnd)) {  // new sub event
      if (fSubEventIndex >= fEvent->GetNSubEvents()) return kFALSE;
      fSubEvent = fEvent->GetSubEvent(fSubEventIndex++);
      fRawData = fSubEvent->GetRawData();
      fCount = 0;
      fPosition = (UChar_t*) fRawData->GetBuffer();
      fEnd = ((UChar_t*) fRawData->GetBuffer()) + fRawData->GetSize();
    }
    if (fPosition >= fEnd) continue;          // no data left in the payload
    if (fPosition + sizeof(AliMiniHeader) > fEnd) {
      Error("ReadMiniHeader", "could not read data!");
      return kFALSE;
    }
    fMiniHeader = (AliMiniHeader*) fPosition;
    fPosition += sizeof(AliMiniHeader);
    CheckMiniHeader();
    fCount = fMiniHeader->fSize;
    if (fPosition + fCount > fEnd) {  // check data size in mini header and sub event
      Error("ReadMiniHeader", "size in mini header exceeds event size!");
      fMiniHeader->fSize = fCount = fEnd - fPosition;
    }
  } while (!IsSelected());
  return kTRUE;
}

Bool_t AliRawReaderRoot::ReadNextData(UChar_t*& data)
{
// reads the next payload at the current position
// returns kFALSE if the data could not be read

  while (fCount == 0) {
    if (!ReadMiniHeader()) return kFALSE;
  }
  data = fPosition;
  fPosition += fCount;  
  fCount = 0;
  return kTRUE;
}

Bool_t AliRawReaderRoot::ReadNext(UChar_t* data, Int_t size)
{
// reads the next block of data at the current position
// returns kFALSE if the data could not be read

  if (fPosition + size > fEnd) {
    Error("ReadNext", "could not read data!");
    return kFALSE;
  }
  memcpy(data, fPosition, size);
  fPosition += size;
  fCount -= size;
  return kTRUE;
}


Bool_t AliRawReaderRoot::Reset()
{
// reset the current position to the beginning of the event

  fSubEventIndex = 0;
  fSubEvent = NULL;
  fRawData = NULL;
  fMiniHeader = NULL;

  fCount = 0;
  fPosition = fEnd = NULL;
  return kTRUE;
}


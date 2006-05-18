// $Header$

#include "TPCData.h"

#include <Alieve/TPCSectorData.h>

#include <AliSimDigits.h>
#include <AliTPCParam.h>
#include <AliTPCRawStream.h>
#include <AliTPCRawStreamOld.h>
#include <TTree.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// TPCData
//
// A central manager for TPC data of an event.  Can read digits (from
// a tree: LoadDigits()) and raw-data (via AliRawReader: LoadRaw()).
//
// The sector data is stored in 36 TPCSectorData objects.
// Sectors 0 - 17: +z side, 18 - 35: -z side.
// No separation of inner/outer segments, use row numbers for addressing.
//

ClassImp(TPCData);

TPCData::TPCData() :
  fSectors(36), fSectorBlockSize(65536),
  fLoadThreshold(0)
{
  TPCSectorData::InitStatics();
}

TPCData::~TPCData()
{
  // !!!! delete sectors
}

/**************************************************************************/

void TPCData::CreateSector(Int_t sector)
{
  if(fSectors[sector] == 0)
    fSectors[sector] = new TPCSectorData(sector, fSectorBlockSize);
}

void TPCData::CreateAllSectors()
{
  for(Int_t s=0; s<36; ++s)
    CreateSector(s);
}

/**************************************************************************/

TPCSectorData* TPCData::GetSectorData(Int_t sector, Bool_t spawnSectors)
{
  if(sector < 0 || sector > 35) return 0;
  if(fSectors[sector] == 0 && spawnSectors)
    CreateSector(sector);
  return fSectors[sector];
}

/**************************************************************************/

void TPCData::LoadDigits(TTree* tree, Bool_t spawnSectors)
{
  // Load data from TTree of AliSimDigits.
  // If spawnSectors is false only sectors that have been created previously
  // via CreateSector() are loaded.
  // If spawnSectors is true sectors are created if data for them is encountered.

  AliSimDigits digit, *digitPtr = &digit;
  tree->GetBranch("Segment")->SetAddress(&digitPtr);
  
  Int_t sector, row, pad, curPad;
  Short_t time, signal;
  Bool_t  inFill = kFALSE;
  TPCSectorData* secData = 0;

  Int_t numEnt = (Int_t) tree->GetEntries();
  for (Int_t ent=0; ent<numEnt; ent++) {
    tree->GetEntry(ent);
    Alieve::TPCSectorData::GetParam().AdjustSectorRow(digit.GetID(), sector, row);
    if(sector >= 36) {
      sector -= 36;
      row    += TPCSectorData::GetInnSeg().GetNRows();
    }
    secData = GetSectorData(sector, spawnSectors);
    if(secData == 0)
      continue;

    if(digit.First() == kFALSE)
      continue;
    curPad = -1;
    do {
      pad    = digit.CurrentColumn();
      time   = digit.CurrentRow();
      signal = digit.CurrentDigit();

      if(pad != curPad) {
	if(inFill)
	  secData->EndPad();
	secData->BeginPad(row, pad, kFALSE);
	curPad = pad;
	inFill = kTRUE;
      }
      if(signal > fLoadThreshold)
	secData->RegisterData(time, signal);

    } while (digit.Next());
    if(inFill) {
      secData->EndPad();
      inFill = kFALSE;
    }
  }
}

void TPCData::LoadRaw(AliTPCRawStream& input, Bool_t spawnSectors)
{
  // Load data from AliTPCRawStream.
  // If spawnSectors is false only sectors that have been created previously
  // via CreateSector() are loaded.
  // If spawnSectors is true sectors are created if data for them is encountered.

  Int_t  sector = -1, row = -1, pad = -1, rowOffset = 0;
  Bool_t inFill = kFALSE;
  TPCSectorData* secData = 0;

  while (input.Next()) {
    if (input.IsNewSector()) {
      if(inFill) {
	secData->EndPad();
	inFill = kFALSE;
      }
      sector = input.GetSector();
      if(sector >= 36) {
	sector -= 36;
	rowOffset = TPCSectorData::GetInnSeg().GetNRows();
      } else {
	rowOffset = 0;
      }
      secData = GetSectorData(sector, spawnSectors);
    }
    if (secData == 0)
      continue;

    if (input.IsNewPad()) {
      if(inFill) {
	secData->EndPad();
	inFill = kFALSE;
      }
      row = input.GetRow() + rowOffset;
      pad = input.GetPad();

      secData->BeginPad(row, pad, kTRUE);
      inFill = kTRUE;
    }

    if(input.GetSignal() > fLoadThreshold)
      secData->RegisterData(input.GetTime(), input.GetSignal());
  }

  if(inFill) {
    secData->EndPad();
    inFill = kFALSE;
  }
}

void TPCData::LoadRaw(AliTPCRawStreamOld& input, Bool_t spawnSectors, Bool_t warn)
{
  // Load data from AliTPCRawStream.
  // If spawnSectors is false only sectors that have been created previously
  // via CreateSector() are loaded.
  // If spawnSectors is true sectors are created if data for them is encountered.

  static const Exc_t eH("TPCData::LoadRaw ");

  Int_t  sector = -1, row = -1, pad = -1, rowOffset = 0;
  Bool_t inFill = kFALSE;
  TPCSectorData* secData = 0;

  while (input.Next()) {
    if (input.IsNewSector()) {
      if(inFill) {
	secData->EndPad();
	inFill = kFALSE;
      }
      sector = input.GetSector();
      if(sector >= 36) {
	sector -= 36;
	rowOffset = TPCSectorData::GetInnSeg().GetNRows();
      } else {
	rowOffset = 0;
      }
      secData = GetSectorData(sector, spawnSectors);
    }
    if (secData == 0)
      continue;

    if (input.IsNewPad()) {
      if(inFill) {
	secData->EndPad();
	inFill = kFALSE;
      }
      row = input.GetRow() + rowOffset;
      pad = input.GetPad();

      if(pad >= TPCSectorData::GetNPadsInRow(row)) {
	if(warn) {
	  Warning(eH.Data(), "pad out of range (row=%d, pad=%d, maxpad=%d).",
		  row, pad, TPCSectorData::GetNPadsInRow(row));
	}
	continue;
      }

      secData->BeginPad(row, pad, kTRUE);
      inFill = kTRUE;
    }

    if(input.GetSignal() > fLoadThreshold)
      secData->RegisterData(input.GetTime(), input.GetSignal());
  }

  if(inFill) {
    secData->EndPad();
    inFill = kFALSE;
  }
}

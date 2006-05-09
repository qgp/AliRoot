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

////////////////////////////////////
///
/// MUON Digit maker from rawdata in ALICE-MUON
/// Using new interface with AliMUONRawStreamTracker(Trigger)
/// (New interface of AliMUONRawReader class)
/// Class version 1 (further details could be found in Alice-note)
///
/// Implemented non-constant buspatch numbers for tracking
/// with correct DDL id (first guess)
/// (Ch. Finck, dec 2005)
///
///
/// Raw2Digits:
/// Using real mapping  for tracker
/// Indranil Das (Adapted for runloader: Ch. Finck) july 05
/// Add reader for scaler trigger events
/// Use memcpy instead of assignment elt by elt
/// (Ch. Finck, Jan 06)
/// 
////////////////////////////////////

#include <fstream>
#include <string>

#include <TClonesArray.h>

#include "AliRawReader.h"
#include "AliRawDataHeader.h"
#include "AliLog.h"
#include "AliRun.h"

#include "AliMUON.h"
#include "AliMUONDigitMaker.h"
#include "AliMUONDigit.h"

#include "AliMUONConstants.h"
#include "AliMUONData.h"

#include "AliMUONRawStreamTracker.h"
#include "AliMUONDDLTracker.h"
#include "AliMUONDspHeader.h"
#include "AliMUONBlockHeader.h"
#include "AliMUONBusStruct.h"

#include "AliMUONRawStreamTrigger.h"
#include "AliMUONDDLTrigger.h"
#include "AliMUONDarcHeader.h"
#include "AliMUONRegHeader.h"
#include "AliMUONLocalStruct.h"

#include "AliMUONLocalTrigger.h"
#include "AliMUONGlobalTrigger.h"

#include "AliMpSegFactory.h"
#include "AliMpVSegmentation.h"
#include "AliMpPad.h"
#include "AliMpDEManager.h"

ClassImp(AliMUONDigitMaker) // Class implementation in ROOT context
//__________________________________________________________________________
AliMUONDigitMaker::AliMUONDigitMaker(AliMUONData* data)
  : TObject(),
    fScalerEvent(kFALSE)
{
  //
  // ctor with AliMUONData as argument
  // for reconstruction
  //

  AliDebug(1,"");

  // Standard Constructor

  // initialize segmentation factory
  fSegFactory = new AliMpSegFactory();

  // initialize container
  fMUONData  = data;

  // bus patch 
  fBusPatchManager = new AliMpBusPatch();
  fBusPatchManager->ReadBusPatchFile();

  // raw streamers
  fRawStreamTracker = new AliMUONRawStreamTracker();    
  fRawStreamTrigger = new AliMUONRawStreamTrigger();    

  // digit
  fDigit = new AliMUONDigit();

  // local trigger
  fLocalTrigger  =  new AliMUONLocalTrigger();
  fGlobalTrigger =  new AliMUONGlobalTrigger();

  fTrackerTimer.Start(kTRUE); fTrackerTimer.Stop();
  fTriggerTimer.Start(kTRUE); fTriggerTimer.Stop();
  fMappingTimer.Start(kTRUE); fMappingTimer.Stop();

}

//__________________________________________________________________________
AliMUONDigitMaker::AliMUONDigitMaker()
  : TObject(),
    fMUONData(0),
    fSegFactory(0),
    fBusPatchManager(0),
    fScalerEvent(kFALSE),
    fRawStreamTracker(0),
    fRawStreamTrigger(0),
    fDigit(0),
    fLocalTrigger(0)
{
  //
  // Default Constructor
  //
  AliDebug(1,""); 
  fTrackerTimer.Start(kTRUE); fTrackerTimer.Stop();
  fTriggerTimer.Start(kTRUE); fTriggerTimer.Stop();
  fMappingTimer.Start(kTRUE); fMappingTimer.Stop();
  
}

//_______________________________________________________________________
AliMUONDigitMaker::AliMUONDigitMaker (const AliMUONDigitMaker& rhs)
  : TObject(rhs)
{
  //
  // Protected copy constructor
  //
  AliFatal("Not implemented.");
}

//_______________________________________________________________________
AliMUONDigitMaker & 
AliMUONDigitMaker::operator=(const AliMUONDigitMaker& rhs)
{
  //
  // Protected assignement operator
  //
  if (this == &rhs) return *this;

  AliFatal("Not implemented.");
    
  return *this;  
}

//__________________________________________________________________________
AliMUONDigitMaker::~AliMUONDigitMaker(void)
{
  //
  // clean up
  // and time processing measure
  //
  delete fSegFactory;  

  delete fRawStreamTracker;
  delete fRawStreamTrigger;

  delete fDigit;
  delete fLocalTrigger;
  delete fGlobalTrigger;

  delete fBusPatchManager;

  AliInfo(Form("Execution time for MUON tracker : R:%.2fs C:%.2fs",
               fTrackerTimer.RealTime(),fTrackerTimer.CpuTime()));
  AliInfo(Form("   Execution time for MUON tracker (mapping calls part) "
               ": R:%.2fs C:%.2fs",
               fMappingTimer.RealTime(),fMappingTimer.CpuTime()));
  AliInfo(Form("Execution time for MUON trigger : R:%.2fs C:%.2fs",
               fTriggerTimer.RealTime(),fTriggerTimer.CpuTime()));

  return;
}

//____________________________________________________________________
Int_t AliMUONDigitMaker::Raw2Digits(AliRawReader* rawReader)
{
  // Main method to creates digit
  // for tracker 
  // and trigger

  // generate digits
  ReadTrackerDDL(rawReader);

  // generate trigger
  ReadTriggerDDL(rawReader);

  return kTRUE;

}

//____________________________________________________________________
Int_t AliMUONDigitMaker::ReadTrackerDDL(AliRawReader* rawReader)
{

  // reading tracker DDL
  // filling the TClonesArray in MUONData
  //
  fTrackerTimer.Start(kFALSE);

  // elex info
  Int_t    buspatchId;
  UChar_t  channelId;
  UShort_t manuId;
  Char_t   parity;
  UShort_t charge; 
  Int_t    dataSize;

  AliMUONDDLTracker*   ddlTracker = 0x0;
  AliMUONBlockHeader*  blkHeader  = 0x0;
  AliMUONDspHeader*    dspHeader  = 0x0;
  AliMUONBusStruct*    busStruct  = 0x0;


  fRawStreamTracker->SetReader(rawReader);

  while(fRawStreamTracker->NextDDL()) {

    ddlTracker =  fRawStreamTracker->GetDDLTracker();

    Int_t nBlock = ddlTracker->GetBlkHeaderEntries();
    for(Int_t iBlock = 0; iBlock < nBlock ;iBlock++){

      blkHeader = ddlTracker->GetBlkHeaderEntry(iBlock);
 
      Int_t nDsp = blkHeader->GetDspHeaderEntries();

      for(Int_t iDsp = 0; iDsp < nDsp ;iDsp++){   //DSP loop

	dspHeader =  blkHeader->GetDspHeaderEntry(iDsp);

	Int_t nBusPatch = dspHeader->GetBusPatchEntries();

	for(Int_t iBusPatch = 0; iBusPatch < nBusPatch; iBusPatch++) {  

	  busStruct = dspHeader->GetBusPatchEntry(iBusPatch);

	  dataSize   = busStruct->GetLength();
	  buspatchId = busStruct->GetBusPatchId();

	  for (Int_t iData = 0; iData < dataSize; iData++) {

	    // digits info
	    parity    = busStruct->GetParity(iData); // test later for parity
	    manuId    = busStruct->GetManuId(iData);
	    channelId = busStruct->GetChannelId(iData);
	    charge    = busStruct->GetCharge(iData);
	    // set charge
	    fDigit->SetSignal(charge);
	    fDigit->SetPhysicsSignal(charge);
	    fDigit->SetADC(charge);

	    // Get Back the hits at pads
	    Int_t error = GetMapping(buspatchId,manuId,channelId,fDigit); 
	    if (error) continue;

	    // debugging 
	    if (AliLog::GetGlobalDebugLevel() == 3) {
	      Int_t padX  = fDigit->PadX();
	      Int_t padY  = fDigit->PadY();
	      Int_t iCath = fDigit->Cathode();  
	      Int_t idDE  = fDigit->DetElemId();

	      AliDebug(1,Form("output  IdDE %d busPatchid %d PadX %d PadY %d iCath %d \n", 
			      idDE, buspatchId, padX, padY, iCath));
		
	      AliDebug(3,Form("idDE %d Padx %d Pady %d, Cath %d, charge %d",
			      idDE, padX, padY, iCath, charge));
	    }

	    // fill digits
	    fMUONData->AddDigit(fRawStreamTracker->GetDDL()/2, *fDigit);

	  } // iData
	} // iBusPatch
      } // iDsp
    } // iBlock
  } // NextDDL

  fTrackerTimer.Stop();

  return kTRUE;
}
//____________________________________________________________________
Int_t AliMUONDigitMaker::GetMapping(Int_t busPatchId, UShort_t manuId, 
					 UChar_t channelId, AliMUONDigit* digit )
{
  //
  // mapping  for tracker
  //
  fMappingTimer.Start(kFALSE);
  
  // getting DE from buspatch
  Int_t  detElemId = fBusPatchManager->GetDEfromBus(busPatchId);
  AliDebug(3,Form("detElemId: %d busPatchId %d\n", detElemId, busPatchId));

  AliMpVSegmentation* seg = fSegFactory->CreateMpSegmentationByElectronics(detElemId, manuId);  
  AliMpPad pad = seg->PadByLocation(AliMpIntPair(manuId,channelId),kTRUE);

  if (!pad.IsValid())
  {
    AliWarning(Form("No pad for detElemId: %d, busPatchId %d, manuId: %d, channelId: %d\n",
		  detElemId, busPatchId, manuId, channelId));
    fMappingTimer.Stop();
    return kTRUE;
  } // return error

  // Getting padX, padY and cathode number.
  Int_t padX = pad.GetIndices().GetFirst();
  Int_t padY = pad.GetIndices().GetSecond();
  Int_t iCath = AliMpDEManager::GetCathod(detElemId,seg->PlaneType());

  // storing into digits
  digit->SetPadX(padX);
  digit->SetPadY(padY);
  digit->SetCathode(iCath);
  digit->SetDetElemId(detElemId);
  digit->SetElectronics(manuId,channelId);
  
  AliDebug(3,Form("detElemId: %d, busPatchId %d, manuId: %d, channelId: %d, padx: %d pady %d\n",
		  detElemId, busPatchId, manuId, channelId, padX, padY));
  StdoutToAliDebug(3,digit->Print(););
  
  fMappingTimer.Stop();
  return kFALSE;
}

//____________________________________________________________________
Int_t AliMUONDigitMaker::ReadTriggerDDL(AliRawReader* rawReader)
{
  // reading tracker DDL
  // filling the TClonesArray in MUONData
  //

  AliMUONDDLTrigger*       ddlTrigger      = 0x0;
  AliMUONDarcHeader*       darcHeader      = 0x0;
  AliMUONRegHeader*        regHeader       = 0x0;
  AliMUONLocalStruct*      localStruct     = 0x0;

  Int_t loCircuit;

  fTriggerTimer.Start(kFALSE);

  fRawStreamTrigger->SetReader(rawReader);

  while(fRawStreamTrigger->NextDDL()) {

    ddlTrigger = fRawStreamTrigger->GetDDLTrigger();
    darcHeader = ddlTrigger->GetDarcHeader();

    // fill global trigger information
    if (darcHeader->GetGlobalFlag() == 2) {
      fGlobalTrigger->SetGlobalPattern(darcHeader->GetGlobalOutput());
      fMUONData->AddGlobalTrigger(*fGlobalTrigger);
    }

    Int_t nReg = darcHeader->GetRegHeaderEntries();

    for(Int_t iReg = 0; iReg < nReg ;iReg++){   //reg loop

      regHeader =  darcHeader->GetRegHeaderEntry(iReg);

      Int_t nLocal = regHeader->GetLocalEntries();

      for(Int_t iLocal = 0; iLocal < nLocal; iLocal++) {  

	localStruct = regHeader->GetLocalEntry(iLocal);

	if (localStruct->GetTriggerY() == 0) {
	  loCircuit = localStruct->GetId()+ 16*regHeader->GetId() 
                                              + 128*fRawStreamTrigger->GetDDL(); 
	    
	  // fill local trigger
	  fLocalTrigger->SetLocalStruct(loCircuit, *localStruct);

	  fMUONData->AddLocalTrigger(*fLocalTrigger);
	} // if triggerY
      } // iLocal
    } // iReg
  } // NextDDL

  fTriggerTimer.Stop();

  return kTRUE;

}


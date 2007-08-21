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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This class provides access to TRD digits in raw data.                     //
//                                                                           //
// It loops over all TRD digits in the raw data given by the AliRawReader.   //
// The Next method goes to the next digit. If there are no digits left       //
// it returns kFALSE.                                                        //
// Several getters provide information about the current digit.              //
//                                                                           //
// Author: C. Lippmann (C.Lippmann@gsi.de)                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliLog.h"
#include "AliRawReader.h"
#include "AliTRDRawStreamV2.h"
#include "AliTRDgeometry.h"
#include "AliTRDcalibDB.h"
#include "AliTRDfeeParam.h"

#include "AliTRDdigitsManager.h"
#include "AliTRDdataArrayI.h"
#include "AliTRDSignalIndex.h"

#include "TMath.h"
ClassImp(AliTRDRawStreamV2)

//_____________________________________________________________________________
AliTRDRawStreamV2::AliTRDRawStreamV2() 
  :TObject()
  ,fSig()
  ,fADC(0)
  ,fTB(0)
  ,fEv(0)
  ,fROB(0)
  ,fMCM(0)
  ,fSM(0)
  ,fLAYER(0)
  ,fSTACK(0)
  ,fROC(0)
  ,fSIDE(0)
  ,fDCS(0)
  ,fROW(0)
  ,fCOL(0)
  ,fDET(0)
  ,fLastDET(-1)
  ,fBCctr(0)
  ,fPTctr(0)
  ,fPTphase(0)
  ,fRVmajor(0)
  ,fRVminor(0)
  ,fHCHWords(0)
  ,fTBins(0)
  ,fTCon(0)
  ,fPEDon(0)
  ,fGAINon(0)
  ,fXTon(0)
  ,fNonLinOn(0)
  ,fBypass(0)
  ,fCommonAdditive(0)
  ,fZeroSuppressed(0)
  ,fHCHctr1(0)
  ,fHCHctr2(0)
  ,fMCMHctr1(0)
  ,fMCMHctr2(0)
  ,fGTUctr1(0)
  ,fGTUctr2(0)
  ,fHCdataCtr(0)
  ,fTracklPID(0.)
  ,fTracklDefL(0.)
  ,fTracklPadPos(0.)
  ,fTracklPadRow(0)
  ,fGTUlinkMask()
  ,fMCMWordCrt(0)
  ,fMCMWordsExpected(0)
  ,fRawReader(NULL)
  ,fRawVersion(2)
  ,fRawDigitThreshold(0)
  ,fNextStatus(0)
  ,fLastStatus(0)
  ,fTbSwitch(0)
  ,fTbSwitchCtr(0)
  ,fTimeWords(0)
  ,fWordCtr(0)
  ,fRowMax(0)
  ,fColMax(0)
  ,fADCmask()
  ,fLastADCmask(0)
  ,fChamberDone()
  ,fRetVal(0)
  ,fEqID(0)
  ,fDataSize(0)
  ,fSizeOK(kFALSE)
  ,fCountBytes(0)
  ,fBufSize(0)
  ,fkBufferSet(kFALSE)
  ,fPos(NULL)
  ,fDataWord(NULL)
  ,fTimeBinsCalib(0)
  ,fADClookup()
  ,fNActiveADCs(0)
  ,fEndOfDataFlag(kFALSE)
  ,fGeo(NULL) 
{
  //
  // Default constructor
  //

  for (Int_t i = 0; i < 540; i++) {
    fChamberDone[i] = 0;
  }

}

//_____________________________________________________________________________
AliTRDRawStreamV2::AliTRDRawStreamV2(AliRawReader *rawReader) 
  :TObject()
  ,fSig()
  ,fADC(0)
  ,fTB(0)
  ,fEv(0)
  ,fROB(0)
  ,fMCM(0)
  ,fSM(0)
  ,fLAYER(0)
  ,fSTACK(0)
  ,fROC(0)
  ,fSIDE(0)
  ,fDCS(0)
  ,fROW(0)
  ,fCOL(0)
  ,fDET(0)
  ,fLastDET(-1)
  ,fBCctr(0)
  ,fPTctr(0)
  ,fPTphase(0)
  ,fRVmajor(0)
  ,fRVminor(0)
  ,fHCHWords(0)
  ,fTBins(0)
  ,fTCon(0)
  ,fPEDon(0)
  ,fGAINon(0)
  ,fXTon(0)
  ,fNonLinOn(0)
  ,fBypass(0)
  ,fCommonAdditive(0)
  ,fZeroSuppressed(0)
  ,fHCHctr1(0)
  ,fHCHctr2(0)
  ,fMCMHctr1(0)
  ,fMCMHctr2(0)
  ,fGTUctr1(0)
  ,fGTUctr2(0)
  ,fHCdataCtr(0)
  ,fTracklPID(0.)
  ,fTracklDefL(0.)
  ,fTracklPadPos(0.)
  ,fTracklPadRow(0)
  ,fGTUlinkMask()
  ,fMCMWordCrt(0)
  ,fMCMWordsExpected(0)
  ,fRawReader(rawReader)
  ,fRawVersion(2)
  ,fRawDigitThreshold(0)
  ,fNextStatus(0)
  ,fLastStatus(0)
  ,fTbSwitch(0)
  ,fTbSwitchCtr(0)
  ,fTimeWords(0)
  ,fWordCtr(0)
  ,fRowMax(0)
  ,fColMax(0)
  ,fADCmask()
  ,fLastADCmask(0)
  ,fChamberDone()
  ,fRetVal(0)
  ,fEqID(0)
  ,fDataSize(0)
  ,fSizeOK(kFALSE)
  ,fCountBytes(0)
  ,fBufSize(0)
  ,fkBufferSet(kFALSE)
  ,fPos(NULL)
  ,fDataWord(NULL)
  ,fTimeBinsCalib(0)
  ,fADClookup()
  ,fNActiveADCs(0)
  ,fEndOfDataFlag(kFALSE)
  ,fGeo(NULL) 
{
  //
  // Create an object to read TRD raw digits
  //

  fRawReader->Select("TRD");

  for (Int_t i = 0; i < 540; i++) {
    fChamberDone[i] = 0;
  }

}

//_____________________________________________________________________________
AliTRDRawStreamV2::AliTRDRawStreamV2(const AliTRDRawStreamV2& stream)
  :TObject(stream)
  ,fSig()
  ,fADC(-1)
  ,fTB(-1)
  ,fEv(-1)
  ,fROB(-1)
  ,fMCM(-1)
  ,fSM(-1)
  ,fLAYER(-1)
  ,fSTACK(-1)
  ,fROC(-1)
  ,fSIDE(-1)
  ,fDCS(-1)
  ,fROW(-1)
  ,fCOL(-1)
  ,fDET(0)
  ,fLastDET(-1)
  ,fBCctr(-1)
  ,fPTctr(-1)
  ,fPTphase(-1)
  ,fRVmajor(-1)
  ,fRVminor(-1)
  ,fHCHWords(-1)
  ,fTBins(-1)
  ,fTCon(0)
  ,fPEDon(0)
  ,fGAINon(0)
  ,fXTon(0)
  ,fNonLinOn(-1)
  ,fBypass(-1)
  ,fCommonAdditive(-1)
  ,fZeroSuppressed(0)
  ,fHCHctr1(-1)
  ,fHCHctr2(-1)
  ,fMCMHctr1(-1)
  ,fMCMHctr2(-1)
  ,fGTUctr1(-1)
  ,fGTUctr2(-1)
  ,fHCdataCtr(-1)
  ,fTracklPID(-1.)
  ,fTracklDefL(-1.)
  ,fTracklPadPos(-1.)
  ,fTracklPadRow(-1)
  ,fGTUlinkMask()
  ,fMCMWordCrt(0)
  ,fMCMWordsExpected(0)
  ,fRawReader(NULL)
  ,fRawVersion(-1)
  ,fRawDigitThreshold(0)
  ,fNextStatus(0)
  ,fLastStatus(0)
  ,fTbSwitch(0)
  ,fTbSwitchCtr(0)
  ,fTimeWords(0)
  ,fWordCtr(0)
  ,fRowMax(-1)
  ,fColMax(-1)
  ,fADCmask()
  ,fLastADCmask(0)
  ,fChamberDone()
  ,fRetVal(0)
  ,fEqID(0)
  ,fDataSize(0)
  ,fSizeOK(kFALSE)
  ,fCountBytes(0)
  ,fBufSize(0)
  ,fkBufferSet(kFALSE)
  ,fPos(NULL)
  ,fDataWord(NULL)
  ,fTimeBinsCalib(0)
  ,fADClookup()
  ,fNActiveADCs(0)
  ,fEndOfDataFlag(kFALSE)
  ,fGeo(NULL)
{
  //
  // Copy constructor
  //

  AliFatal("Copy constructor not implemented");

}

//_____________________________________________________________________________
AliTRDRawStreamV2& AliTRDRawStreamV2::operator = (const AliTRDRawStreamV2& 
					      /* stream */)
{
  //
  // Assigment operator
  //

  Fatal("operator =", "assignment operator not implemented");
  return *this;

}

//_____________________________________________________________________________
AliTRDRawStreamV2::~AliTRDRawStreamV2()
{
  //
  // Destructor
  //

  if (fGeo) {  
    delete fGeo;
  }

}

//_____________________________________________________________________________
void AliTRDRawStreamV2::SetRawReader(AliRawReader *rawReader) 
{
  //
  // Set the rawreader
  //

  if (rawReader)
    {
      fRawReader = rawReader;
    }
}

//_____________________________________________________________________________
Bool_t AliTRDRawStreamV2::SetRawVersion(Int_t rv)
{
  //
  // Set the raw data version
  //

  if ( rv >= 0 && rv <= 3 ) {
    fRawVersion = rv;
    return kTRUE;
  }

  return kFALSE;

}


//____________________________________________________________________________
Int_t AliTRDRawStreamV2::Init()
{
  //
  // Initialization
  //

  if (!AliTRDcalibDB::Instance()) {
    AliError("Could not get calibration object");
    return 0;
  }

  if (!fGeo) {
    fGeo = new AliTRDgeometry();
  }
  
  fTimeBinsCalib = AliTRDcalibDB::Instance()->GetNumberOfTimeBins();
  //AliDebug(2, Form("Number of Timebins read from CDB: %d", fTimeBinsCalib));

  // The number of data words needed for this number of time bins (there
  // are 3 time bins in one word)
  fTimeWords = (fTimeBinsCalib-1)/3 + 1;

  fTbSwitch    = 3;
  fTbSwitchCtr = 0;

  fHCHctr1 = fHCHctr2 =  0;
  fGTUctr1 = fGTUctr2 = -1;

  fHCdataCtr = 0;
  fWordCtr   = 0;  

  fDET     = 0;
  fLastDET     = -1;
  fRetVal = 0;
  fEqID     = 0;
  fDataSize = 0;
  fSizeOK = kFALSE;
  
  fLastStatus = fkStart;
  fNextStatus = fkStart;

  fCountBytes = 0;
  fBufSize = 0;
  fDataWord = NULL;
  fPos = NULL;
  fWordCtr = 0;
  fkBufferSet = kFALSE;

  fMCMWordCrt = 0;
  fMCMWordsExpected = 0;

  fEndOfDataFlag = kFALSE;
  // set all ADC active
  fNActiveADCs = ChannelsToRead(0x1fffff); // should be 1111 1111 1111 1111 1111 1 = 21 bits active (0-20)

  fLastADCmask = 0;

  return kTRUE;
}

//____________________________________________________________________________
Int_t AliTRDRawStreamV2::NextData()
{
  //
  // Updates the next data word pointer
  //

  if (fCountBytes + kSizeWord >= fBufSize)
    {
      fkBufferSet = fRawReader->ReadNextData(fPos);
      if (fkBufferSet == kTRUE)
	{
	  fBufSize = fRawReader->GetDataSize();
	  fCountBytes = 0;	  
	  fDataWord = (UInt_t*)fPos;
	  ChangeStatus(fkNextSM);
	  fWordCtr = 0;
	  return fkNextSM;
	}
      else
	{
	  ChangeStatus(fkStop);
	  return fkNoMoreData;
	}
    }
  else
    {
      fPos += kSizeWord;
      fCountBytes += kSizeWord;	  
      fDataWord = (UInt_t*)fPos;
      fWordCtr++;
      return fkWordOK;
    }
}

//============================================================================
// Decoding functions
//============================================================================


//____________________________________________________________________________
void AliTRDRawStreamV2::DecodeHCheader(Int_t timeBins)
{
  //
  // Decode the HC header (fRawVersion == 2, 3, 4, ???)
  //

  fRVmajor = (*fDataWord >> 24) & 0x7f;
  fRVminor = (*fDataWord >> 17) & 0x7f;

  if (fRVmajor < 2 || fRVmajor > 4)
    AliError(Form(" Unsupported raw version: %d", fRawVersion))
  
  if ( fRawVersion != fRVmajor ) {
    
    AliWarning("===============================================================================");
    AliWarning(Form("Mismatch between fRawVersion (%d) and fRVmajor from HC header (%d)"
		    ,fRawVersion,fRVmajor));
    AliWarning(Form("Setting fRawVersion to %d", fRVmajor));
    AliWarning("===============================================================================");
    fRawVersion = fRVmajor;

  }

  //
  // check for zero suppression
  if ( fRawVersion >= 3 || fRawVersion <= 4 ) fZeroSuppressed = kTRUE;
  else                                        fZeroSuppressed = kFALSE;
  
  // 1st word (h[0])
  if ( (*fDataWord & 0x3) == 1 ) {

    fHCHWords = (*fDataWord >> 14) & 0x7;
    fSM       = (*fDataWord >>  9) & 0x1f;
    fLAYER    = (*fDataWord >>  6) & 0x7;
    fSTACK    = (*fDataWord >>  3) & 0x7;
    fSIDE     = (*fDataWord >>  2) & 0x1;

    fROC      = fGeo->GetDetectorSec(fLAYER, fSTACK);

    //AliDebug(3, Form("0x%08x: HC header: sm=%d; roc=%d; side=%x", *fDataWord, fSM, fROC, fSIDE+10));
    //AliDebug(5, Form("0x%08x: HC header: expecting %d HC words", *fDataWord, fHCHWords));

    if ((fSM    <  0) || 
        (fSM    > 17) || 
        (fLAYER <  0) || 
        (fLAYER >  5) || 
        (fSTACK <  0) || 
        (fSTACK >  4) || 
        (fSIDE  <  0) || 
        (fSIDE  >  1)) 
      {
	AliWarning(Form("0x%08x: Strange HC header: dcs=%d; sm=%d; layer=%d; stack=%d.",
			*fDataWord, fDCS, fSM, fLAYER, fSTACK));
	fRawReader->AddMajorErrorLog(kHCHeaderCorrupt,Form("0x%08x:dcs=%d; sm=%d; layer=%d; stack=%d.",
							   *fDataWord, fDCS, fSM, fLAYER, fSTACK));
      } 
    else 
      {
	fHCHctr1++;
	fHCHctr2++;
      }
  } 
  else 
    { 
      AliWarning(Form("0x%08x: No HC header when it was expected.", *fDataWord)); 
      fRawReader->AddMajorErrorLog(kHCHeaderMissing,Form("0x%08x", *fDataWord));
    }

  // 2nd word (h[1])
  if ( fHCHWords >= 1 ) 
    {
      // read one more word
      if (NextData() != fkWordOK)
	{
	  AliWarning("Next HC word missing");
	  fRawReader->AddMajorErrorLog(kHCWordMissing,"Next HC word missing"); 
	  fNextStatus = fkNextHC;
	  return;
	}

      if ( (*fDataWord & 0x3) == 1 ) 
	{
	  
	  fBCctr   =  (*fDataWord >> 16);
	  fPTctr   =  (*fDataWord >> 12) & 0xf;
	  fPTphase =  (*fDataWord >>  8) & 0xf;
	  fTBins   = ((*fDataWord >>  2) & 0x3f) + 1;
	  fTimeWords = (fTBins - 1)/3 + 1;	
	  
// 	  AliDebug(3, Form("0x%08x: HC header 2: BCctr=%d PTctr=%d PTph=%d TB=%d"
// 			   , *fDataWord, fBCctr, fPTctr, fPTphase, fTBins));

	  if( fTBins != timeBins ) 
	    {	      
	      AliWarning("===============================================================================");
	      AliError(Form("Mismatch between nNTB from CDB (%d) and from HC header (%d)"
			    , timeBins, fTBins));
	      AliWarning(Form("We will use the value from the raw data (HC header): %d", fTBins));
	      AliWarning("===============================================================================");	      

	      fTimeWords = (fTBins - 1)/3 + 1;	
	    }
	}      
    }

  // 3nd word (h[2])
  if ( fHCHWords >= 2 ) {
    // read one more word
    if (NextData() != fkWordOK)
      {
	AliWarning("Next HC word missing");
        fRawReader->AddMajorErrorLog(kHCWordMissing,"Next HC word missing"); 
	fNextStatus = fkNextHC;
	return;
      }
    if ( (*fDataWord & 0x3) == 1 ) {
       
      fTCon     = (*fDataWord >> 29) & 0x1;
      fPEDon    = (*fDataWord >> 31) & 0x1;
      fGAINon   = (*fDataWord >> 30) & 0x1;
      fXTon     = (*fDataWord >> 28) & 0x1;
      fNonLinOn = (*fDataWord >> 27) & 0x1;
      fBypass   = (*fDataWord >> 26) & 0x1;

      fCommonAdditive = (*fDataWord >> 20) & 0x3f;

//       AliDebug(3, Form("0x%08x: HC header 3: TC=%d, PED=%d, GAIN=%d, XT=%d, NonLin=%d, Bypass=%d, Add=%d"
// 		      , fTCon, fPEDon, fGAINon, fXTon, fNonLinOn, fBypass, fCommonAdditive));
    }
  }

}  

//____________________________________________________________________________
Int_t AliTRDRawStreamV2::ChannelsToRead(Int_t ADCmask)
{
  memset(fADClookup, -1, 32 * sizeof(Int_t));
  fADClookup[0] = 0; // count entries
  fADClookup[1] = 2; // index - data start at 2
  UInt_t mask = 0;
  for (Int_t i = 0; i < 30; i++)
    {
      mask = 1 << i;
      if ((ADCmask & mask))
	{
	  //AliDebug(9, Form("fDataWord=0x%08x mask=0x%08x i=%d", *fDataWord, mask, i));
	  fADClookup[fADClookup[1]] = i;
	  ++fADClookup[0];
	  ++fADClookup[1];
	}
    }

  // test the iteration - comment out for production
  // begin of comment out section
  char schannels[512];
  sprintf(schannels, "ADC Channels to read: ");
  fADClookup[1] = 2;
  while(fADClookup[1] - 2 < fADClookup[0])
    {
      //AliDebug(9, Form("max=%d index=%d adc=%d", fADClookup[0], fADClookup[1], fADClookup[fADClookup[1]]));
      strcat(schannels, Form("%d ", fADClookup[fADClookup[1]]));
      fADClookup[1]++;
    }
  //AliDebug(9, Form("%s", schannels));
  //AliDebug(9, Form("ADC channels = %d", fADClookup[0]));
  // end of comment out section

  fADClookup[1] = 2;
  return fADClookup[0];
}

//____________________________________________________________________________
void AliTRDRawStreamV2::DecodeTracklet()
{

  //
  // Decode the Tracklet
  //
  // this function is not tested yet on real tracklets
  //

  if ( fRawVersion < 1 || fRawVersion > 3 ) 
    {
      AliError(Form(" Unsupported raw version: %d", fRawVersion));      
    }

  fTracklPID    = (*fDataWord >> 24) & 0xff;
  fTracklPadRow = (*fDataWord >> 20) & 0xf;    // 0:15
  fTracklDefL   = (*fDataWord >> 13) & 0x7f;
  fTracklPadPos = (*fDataWord)       & 0x1fff;

  fTracklPID    /= (Float_t)((1<<8) - 1);                      // 0:1 (steps of 0.39%)
  fTracklDefL    = (fTracklDefL  - ((1<< 7)-1)/2.) * 140.e-4;  // -0.889:0.889cm 
  fTracklPadPos  = (fTracklPadPos - ((1<<13)-1)/2.) * 160.e-4; // -65.528:65.528 cm

  //AliDebug(4, Form("0x%08x: Tracklet found: SM%d L%dS%d side %x: PadRow=%d PadPos=%f DefL=%f PID=%f"
  //		  , *fDataWord, fSM, fLAYER, fSTACK, fSIDE+10
  //                , fTracklPadRow, fTracklPadPos, fTracklDefL, fTracklPID));

  if( (fSTACK == 2) && (fTracklPadRow >= (Int_t) fGeo->RowmaxC0()) ||
      (fSTACK != 2) && (fTracklPadRow >= (Int_t) fGeo->RowmaxC1()) ) {
    AliWarning(Form("Strange Row read from Tracklet Word: %d", fTracklPadRow));
    fRawReader->AddMajorErrorLog(kTrackletRowMismatch,Form("Word: %d", fTracklPadRow));
  }

}

//____________________________________________________________________________
void AliTRDRawStreamV2::DecodeMCMheader()
{

  //
  // Decode the MCM header
  //

  if ( fRawVersion < 1 || fRawVersion > 3 ) 
    {
      AliError(Form(" Unsupported raw version: %d", fRawVersion));      
    }

  fMCM  = (*fDataWord & 0xff000000) >> 24;
  fEv   = (*fDataWord & 0x00fffff0) >> 4;

  fROB  = fMCM / 16;
  fMCM  = fMCM % 16;

  fROW  = AliTRDfeeParam::Instance()->GetPadRowFromMCM(fROB, fMCM);

//   AliDebug(4, Form("0x%08x: SM%d L%dS%d. MCM Header: fROB=%d fMCM=%02d fEv=%02d"
// 		  , *fDataWord, fSM, fLAYER, fSTACK, fROB, fMCM, fEv));

  if ( fROB % 2 == 0 && fSIDE == 1 ) {
    AliWarning(Form("SM%d L%dS%d: Mismatch between fROB (%d) and fSIDE (%d): fMCM=%02d"
                   , fSM, fLAYER, fSTACK, fROB, fSIDE, fMCM ));
    fRawReader->AddMajorErrorLog(kROBSideMismatch,Form("SM%d L%dS%d: fROB (%d) fSIDE (%d): fMCM=%02d"
						       , fSM, fLAYER, fSTACK, fROB, fSIDE, fMCM ));
  }
  if ( fROB % 2 != 0 && fSIDE == 0 ) {
    AliWarning(Form("SM%d L%dS%d: Mismatch between fROB (%d) and fSIDE (%d): fMCM=%02d"
                   , fSM, fLAYER, fSTACK, fROB, fSIDE, fMCM ));
    fRawReader->AddMajorErrorLog(kROBSideMismatch,Form("SM%d L%dS%d: fROB (%d) fSIDE (%d): fMCM=%02d"
						       , fSM, fLAYER, fSTACK, fROB, fSIDE, fMCM ));
  }
  if ( (fSTACK == 2 && fROW >= fGeo->RowmaxC0()) ||
       (fSTACK != 2 && fROW >= fGeo->RowmaxC1()) || fROW < 0 ) {
    AliWarning(Form("SM%d L%dS%d: Wrong Padrow (%d) fROB=%d, fSIDE=%d, fMCM=%02d"
                   , fSM, fLAYER, fSTACK, fROW, fROB, fSIDE, fMCM ));
    fRawReader->AddMajorErrorLog(kWrongPadrow,Form("SM%d L%dS%d: Padrow (%d) fROB=%d, fSIDE=%d, fMCM=%02d"
						  , fSM, fLAYER, fSTACK, fROW, fROB, fSIDE, fMCM ));
  }
  
  fMCMHctr1++;
  fMCMHctr2++;

  fMCMWordCrt = 1; // MCM header

  // AdcMask for Zero supressed data
  if ( fRawVersion == 3 ) 
    {
      // read one more word
      if (NextData() != fkWordOK)
	{
	  AliWarning("MCM ADC mask missing");
	  fRawReader->AddMajorErrorLog(kMCMADCMaskMissing,"Missing"); 
	  fNextStatus = fkNextHC;
	  return;
	}
      else
	{
	  ++fMCMWordCrt;

	  for ( Int_t ctr = 0; ctr < fGeo->ADCmax(); ctr++ ) {
	    if ( (*fDataWord >> (11+ctr)) == 0x1 ) fADCmask[ctr] = kTRUE;
	    else                                  fADCmask[ctr] = kFALSE;
	  }
	  
	  //AliDebug(4, Form("0x%08x: ADC mask", *fDataWord));
	  fNActiveADCs = ChannelsToRead(*fDataWord);	  
	}
    }

  if (fRawVersion <= 2)
    {
      // no zero suppression
      // raw version 2:
      // 1 MCM header + 21 * ( Ntimebin/3)
      // If NTimebin = 30, it is 211 words. 
      //fMCMWordsExpected = 1 + 21 * (fTBins / 3);
      //fNActiveADCs = 21;
      fNActiveADCs = ChannelsToRead(0x1fffff); // should be 1111 1111 1111 1111 1111 1 = 21 bits active (0-20)
      //fMCMWordsExpected = 1 + fNActiveADCs * ((fTBins-1) / 3. + 1.);      
      //directly get it like that:
      fMCMWordsExpected = 1 + fNActiveADCs * fTBins / 3;      
    }

  if (fRawVersion >= 3)
    {
      // raw version 3:
      // 1 MCM header + 1 ADC mask + NofActiveADCs * ( Ntimebin/3 )
      //directly get it like that:
      fMCMWordsExpected = 1 + 1 + (fTBins * fNActiveADCs) / 3;
    }
  
  //AliDebug(5, Form("We expect %d MCM words. We read %d so far.", fMCMWordsExpected, fMCMWordCrt));
}
//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::DecodeNextRawWord()
{
  //AliDebug(8, Form("-----------------------------------------"));
  //AliDebug(8, Form("DATA IS 0x%x", *fDataWord));

  if (fADClookup[1] - 2 > fADClookup[0])
    {
//       AliDebug(8, Form("Overflow Index ADC = %d Max Index = %d Value = %d. Done with ADCs in this MCM. Is this already MCM header 0x%x?", 
// 		       fADClookup[1] - 2, fADClookup[0], fADClookup[fADClookup[1]], *fDataWord));
      fTbSwitchCtr = 0;
      fMCMWordsExpected = 0;
      AliWarning("Trying to recover. Fall back to DecodeMCM.");
      DecodeMCM();
      //ChangeStatus(fkNextMCM);
      return kFALSE;
    }

  if ( (*fDataWord & 0x00000003) != 0x2 && (*fDataWord & 0x00000003) != 0x3) {
    AliWarning(Form("Data %08x : Data Word ends neither with b11 nor b10", (Int_t)*fDataWord));
    fRawReader->AddMinorErrorLog(kDataMaskError,Form("Data %08x", (Int_t)*fDataWord));
    fMCMWordsExpected = 0;
    AliWarning("Trying to recover. Fall back to DecodeMCM.");
    DecodeMCM();
    //ChangeStatus(fkNextMCM);
    return kFALSE;
  }

  if ( (*fDataWord & 0x3) != fLastADCmask || fTbSwitchCtr > fTimeWords) 
    {    
      fADC = fADClookup[fADClookup[1]];
//       AliDebug(8, Form("Next fADC = %d at index = %d MCM Word Number: %d Max MCM Words is %d", 
// 		       fADC, fADClookup[1] - 2, fMCMWordCrt, fMCMWordsExpected));
      ++fADClookup[1];
      fTB = 0;    
      fTbSwitchCtr = 0;
      fLastStatus = fkNextData;
      fLastADCmask = (*fDataWord) & 0x3;
    }

  ++fTbSwitchCtr;

  //decode data here
  Bool_t kIsDataOK = kFALSE;

  // We have only 21 ADC channels.
  if ( fADC > (Int_t)fGeo->ADCmax() - 1 ) 
    {
      AliWarning(Form("Data %08x : Data is strange. fADC is already %d", (Int_t)*fDataWord, (Int_t)fADC));
      fRawReader->AddMinorErrorLog(kADCChannelOverflow,Form("Data %08x : fADC=%d", (Int_t)*fDataWord, (Int_t)fADC));
    }
  else
    {
      // There are 18 pads connected to each MCM ADC channels 2...19. The other channels cross to other
      // MCMs and are good for online tracking in the MCM.
      if ( fADC > 1 && fADC < (Int_t)fGeo->ADCmax() - 1 ) 
	{
	  
	  // Get Pad column
	  // fCOL = fFee->GetPadColFromADC(fROB, fMCM, fADC);
	  fCOL = AliTRDfeeParam::Instance()->GetPadColFromADC(fROB, fMCM, fADC);
	  
	  // We have only 144 Pad Columns
	  //if ( fCOL > fColMax-1 || fCOL < 0 ) 
	  if ( fCOL >= 0 && fCOL < fColMax && fROW >= 0 && fROW < fRowMax ) 
	    {
	      // Decode 32 bit data words with information from 3 time bins and copy the data
	      fSig[0] = (*fDataWord & 0x00000ffc) >> 2;
	      fSig[1] = (*fDataWord & 0x003ff000) >> 12;
	      fSig[2] = (*fDataWord & 0xffc00000) >> 22;
	      
	      // Print data to screen:
	      //AliDebug(5, Form("DATA : 0x%x", *fDataWord));
	      // 	  AliDebug(5, Form("SM%d L%dS%d: ROB%d MCM=%d ADC=%d (ROW=%d COL=%d): Data %04d %04d %04d\n",
	      // 			   fSM, fLAYER, fSTACK, fROB, fMCM, fADC, fROW, fCOL, fSig[0], fSig[1], fSig[2]));	      
	      kIsDataOK = kTRUE;
	    }
	  else
	    {
	      AliWarning(Form("SM%d L%dS%d: Wrong Pad column (%d) fROB=%d, fSIDE=%d, fMCM=%02d", fSM,
			      fLAYER, fSTACK, fCOL, fROB, fSIDE, fMCM ));
	      fRawReader->AddMajorErrorLog(kWrongPadcolumn,Form("SM%d L%dS%d: column (%d) fROB=%d, fSIDE=%d, fMCM=%02d", fSM,
								fLAYER, fSTACK, fCOL, fROB, fSIDE, fMCM ));
	      kIsDataOK = kFALSE;
	    }
	}
      else 
	{      
	  //AliDebug(5, Form("fADC not accepted %d - DATA : 0x%x", fADC, *fDataWord));
	  fCOL = -1;
	  kIsDataOK = kFALSE;
	}
    }// if fADC is ok

  ++fMCMWordCrt;
  //AliDebug(5, Form("We expect %d MCM words. We read %d so far.", fMCMWordsExpected, fMCMWordCrt));

  // all mcm data processed go to next one
  if ( fMCMWordCrt >= fMCMWordsExpected)
    {
      ChangeStatus(fkNextMCM);
    }

  return kIsDataOK;
}

//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::DecodeMCM()
{
  if( ((*fDataWord & 0x80000000) == 0x0) && ((*fDataWord & 0x0000000f) == 0xC) )
    { // MCM Header
      DecodeMCMheader();
      if ( fMCM < 0 || fMCM > 15 || fROB < 0 || fROB > 7 ) 
	{
	  AliWarning("Wrong fMCM or fROB. Skip this data");
	  fRawReader->AddMajorErrorLog(kWrongMCMorROB,Form("MCM=%d, ROB=%d",fMCM,fROB));
	  ChangeStatus(fkNextHC);
	  return kFALSE;
	}

      fTbSwitch    = 3;  // For first adc channel we expect: (*fDataWord & 3) = 3
      fTbSwitchCtr = 0;  // 
      fADC = fTB   = 0;  // Reset Counter
      fLastADCmask = 0; // Reset

      if (fMCMWordCrt < fMCMWordsExpected)
	{
	  ChangeStatus(fkNextData);
	}
      else
	{
	  ChangeStatus(fkNextMCM);
	}
      return kTRUE;
    }

  if ( *fDataWord == kEndofrawdatamarker ) 
    {  // End of half-chamber data, finished
      fGTUctr1 = -1;
      ChangeStatus(fkNextHC);
      fEndOfDataFlag = kTRUE;
      //AliDebug(5, "Expecting MCM header but got End-Of-Raw-Data Marker");
      if (fMCMWordsExpected == 0 || fMCMWordsExpected == fMCMWordCrt)
	return kTRUE;
      else
	{
	  //AliDebug(5, Form("MCM words missing? %d [expected=%d got=%d] ", fMCMWordsExpected - fMCMWordCrt, fMCMWordsExpected, fMCMWordCrt));	  
	  //AliWarning(Form("MCM words missing? %d [expected=%d got=%d] ", fMCMWordsExpected - fMCMWordCrt, fMCMWordsExpected, fMCMWordCrt));	  
	  return kFALSE;
	}
    }

  //AliDebug(3, Form("Expecting MCM header but got 0x%x. Going to Next MCM header.", *fDataWord));
  AliWarning(Form("Expecting MCM header but got 0x%x. Fall back: Next MCM header.", *fDataWord));
  ChangeStatus(fkNextMCM);      

  return kFALSE;
}

//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::DecodeHC()
{
  if ( fNextStatus == fkNextHC )
    {
      //AliDebug(5, "fkNextHC");
      //
      // 1) Find end_of_tracklet_marker
      //
      // GTU Link Mask?
      if (DecodeGTUlinkMask())
	{
	  return kTRUE;
	}
      
      // endoftrackletmarker?
      if ( *fDataWord == kEndoftrackletmarker ) 
	{
	  //AliDebug(3, "End-of-tracklet-marker found");
	  //AliDebug(5, Form("Data 0x%x", *fDataWord));
	  ChangeStatus(fkSeekNonEoTracklet);
	  return kTRUE;
	} 
      else 
	{
	  // Tracklets found
	  //AliDebug(3, "Tracklet found");
	  //AliDebug(5, Form("Tracklet data 0x%x", *fDataWord));
	  DecodeTracklet();
	  return kTRUE;
	}
    } // if next HC

  if (fNextStatus == fkSeekNonEoTracklet)
    {
      //AliDebug(5, "fkSeekNonEoTracklet");

      //
      // 2) Look for non-end_of_tracklet_marker
      //
      //printf("Word %d: 0x%08x\n", fWordCtr, *fDataWord); 
      
      if ( *fDataWord != kEndoftrackletmarker ) 
	{
	  ChangeStatus(fkDecodeHC);
	  //AliDebug(3, "NON end-of-tracklet-marker found");
	  //AliDebug(5, Form("Data 0x%x", *fDataWord));
	  //// no do not continue - this should be the hcheader
	}
      else
	{
	  //just go on and find the non-end_of_tracklet_marker
	  return kTRUE;
	}
    }

  if ( fNextStatus == fkDecodeHC )
    {
      //AliDebug(5, "fkDecodeHC");
      
      //
      // 3) This Word must be Half Chamber Header
      //
      if ( (*fDataWord & 0x00000003) == 1 ) 
	{ // HC header
	  DecodeHCheader(fTimeBinsCalib); // This is the new header!
	  fLastDET = fDET;
	  fDET    = fGeo->GetDetector(fLAYER, fSTACK, fSM);
	  fRowMax = fGeo->GetRowMax(fLAYER,fSTACK,fSM);
	  fColMax = fGeo->GetColMax(fROC);
	  
	  fMCMHctr2 = 0;
	  fHCdataCtr = 0;
	  fChamberDone[fDET]++;
	  //AliDebug(6, Form("--------------      DET %d fChamberDone[fDET]=%d", fDET, fChamberDone[fDET]));

	  ChangeStatus(fkNextMCM);
	  return kTRUE;
	} //HC header
      else
	{
	  AliWarning(Form("Expecting HC header mask but got 0x%x. Fall back: Next HC.", *fDataWord));
	  ChangeStatus(fkNextHC);
	  // before we went to //ChangeStatus(fkNextSM);
	}
    } // if decode HC
  
  return kFALSE;
}
//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::DecodeGTUlinkMask()
{
  //
  // Decode the link masks sent by the GTU. These marke the active optical links
  // between GTU and Super Module. Up to now only fully active links are found
  // (0xfff = 12 active links).
  //

  if ( (*fDataWord & 0xfffff000) ==  0xe0000000 )
    {
      if ( fRawVersion < 1 || fRawVersion > 3 ) 
	{
	  AliError(Form(" Unsupported raw version: %d", fRawVersion));      
	}
      
      if ( fGTUctr1 == -1 ) fGTUctr2++;
      fGTUctr1++;

      if ( (fGTUctr1 >= 0) && (fGTUctr1 < 5) && (fGTUctr2 >= 0) && (fGTUctr2 < 18) ) 
	{
	  fGTUlinkMask[fGTUctr2][fGTUctr1] = (*fDataWord & 0xfff);
	}
     
      //AliDebug(5, Form("GTU link mask 0x%x decoded 0x%x", *fDataWord, fGTUlinkMask[fGTUctr2][fGTUctr1]));
      return kTRUE;
    }

  return kFALSE;
}

// //____________________________________________________________________________
void AliTRDRawStreamV2::ChangeStatus(Int_t kstat)
{
  fLastStatus = fNextStatus;
  fNextStatus = kstat;  
}

//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::DecodeSM()
{
  fDET     = 0;
  fRetVal = 0;
  fEqID     = 0;
  fDataSize = 0;
  fSizeOK = kFALSE;
  
  // After reading the first word check for size of this data and get Eq. ID
  if ( fWordCtr == 1 ) 
    {
      fDataSize = fRawReader->GetDataSize()/4;  // Size of this payload in 32bit words
      fEqID     = fRawReader->GetEquipmentId(); // Get Equipment ID
      if ( fDataSize > 0 ) fSizeOK = kTRUE;
      //AliDebug(3, Form("fDataSize=%d fEqID=%d", fDataSize, fEqID));
    }
  
  // GTU Link Mask?
  if ( DecodeGTUlinkMask() ) 
    {
      ChangeStatus(fkNextHC);
      return kTRUE;
    } 
  else 
    {
      AliWarning(Form("Equipment %d: First data word is not GTU Link Mask! Fall back: None. Stop.", fEqID));
      fRawReader->AddMajorErrorLog(kGTULinkMaskMissing,Form("Equipment %d",fEqID));
      ChangeStatus(fkStop);
    }	    

  return kFALSE;
}

//____________________________________________________________________________
Bool_t AliTRDRawStreamV2::Next()
{
  //
  // Updates the next data word pointer
  //

  if (fNextStatus == fkStart)
    {
      Init();
    }

  while (fNextStatus != fkStop)
    { // !fkStop
      NextData();
      
      switch (fNextStatus)
	{
	case fkNextData:
	  {
	  if (DecodeNextRawWord() == kTRUE)
	    {
	      fTB += 3;
	      if (fSig[0] > fRawDigitThreshold || fSig[1] > fRawDigitThreshold || fSig[2] > fRawDigitThreshold) 
		return kTRUE;
	    }
	  }; break;
	case fkNextMCM:
	  {
	    if (DecodeMCM() == kFALSE)
	      AliWarning(Form("Decode MCM unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	  }; break;
	case fkNextHC:
	case fkSeekNonEoTracklet:
	case fkDecodeHC:
	  {
	    if (DecodeHC() == kFALSE)
	      {
		AliWarning(Form("Decode HC unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	      }
	    else
	      {
		//the hc header should be decoded by now
		if (fLastStatus == fkDecodeHC)
		  {
		    fLastDET = fDET;
		    fChamberDone[fDET]++;
		  }
	      }
	  }; break;
	case fkNextSM:
	  {
	    if (DecodeSM() == kFALSE)
		AliWarning(Form("Decode SM unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	  }; break;
	case fkStop:
	  ; break;
	default:
	  AliWarning(Form("Unknown state %d. Last state %d. Current Word 0x%x at pos 0x%x", fNextStatus, fLastStatus, *fDataWord, fPos));  
	  ChangeStatus(fkStop);
	};

    } // not fkStop

  //AliDebug(1, Form("That's all folks! %d", fSM));
  return kFALSE;
}

//____________________________________________________________________________
Int_t AliTRDRawStreamV2::NextChamber(AliTRDdigitsManager *man)
{
  //
  // Fills single chamber digit array 
  // Return value is the detector number
  //

  AliTRDdataArrayI *digits = 0;
  AliTRDdataArrayI *track0 = 0;
  AliTRDdataArrayI *track1 = 0;
  AliTRDdataArrayI *track2 = 0; 
  AliTRDSignalIndex *indexes = 0;
	  
  if (fNextStatus == fkStart)
    {
      Init();
    }

//   while (fNextStatus != fkStop)
//     { // !fkStop
//       NextData();
//       // catch 3 things
//       // 1) if end of raw data - if chamber complete return
//       // 2) fill the data with signals if data decoded ok
//       // 3) initialize (destroy old) after the det has changed -> just after HC header decoding
//     } // not fkStop

  while (fNextStatus != fkStop)
    { // !fkStop
      NextData();

      switch (fNextStatus)
	{
	  
	case fkNextData:
	  {
	    if (DecodeNextRawWord() == kTRUE)
	      {
		for (Int_t it = 0; it < 3; it++)
		  {
		    if ( fTB + it < fTBins )
		      {
			if ( fSig[it] > fRawDigitThreshold )
			  {
			    digits->SetDataUnchecked(fROW, fCOL, fTB + it, fSig[it]);
			    indexes->AddIndexTBin(fROW, fCOL, fTB + it);
			    if (man->UsesDictionaries())
			      {
				track0->SetDataUnchecked(fROW, fCOL, fTB + it, 0);
				track1->SetDataUnchecked(fROW, fCOL, fTB + it, 0);
				track2->SetDataUnchecked(fROW, fCOL, fTB + it, 0);
			      } // if dictionaries
			  } // signal above zero
		      } // check the tbins range
		  } // for each tbin of current 3	      
		fTB += 3;
	      }
	    else
	      {
		// can be here as a fall back from decode raw data calling decodemcm
		if (fEndOfDataFlag == kTRUE)
		  {
		    if (fChamberDone[fDET] == 2)
		      {
			return fDET;
		      }		  		
		    fEndOfDataFlag = kFALSE;
		  }		
	      }
	  }; break;

	case fkNextMCM:
	  {
	    if (DecodeMCM() == kFALSE)
	      {
		AliWarning(Form("Decode MCM unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	      }
	    // default place for end of raw data...
	    if (fEndOfDataFlag == kTRUE)
	      {
		if (fChamberDone[fDET] == 2)
		  {
		    return fDET;
		  }		  		
		fEndOfDataFlag = kFALSE;
	      }	     
	  }; break;

	case fkNextHC:
	case fkSeekNonEoTracklet:
	case fkDecodeHC:
	  {
	    if (DecodeHC() == kFALSE)
	      {
		AliWarning(Form("Decode HC unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	      }
	    else
	      {
		//the hc header should be decoded by now
		if (fLastStatus == fkDecodeHC)
		  {
// 		    AliDebug(4, Form("???? New DET ???? %d last %d", fDET, fLastDET));
		    // allocate stuff for the new det
		    //man->ResetArrays();
		    digits = man->GetDigits(fDET);
		    track0 = man->GetDictionary(fDET,0);
		    track1 = man->GetDictionary(fDET,1);
		    track2 = man->GetDictionary(fDET,2);
		    
		    // Allocate memory space for the digits buffer
		    if (digits->GetNtime() == 0) 
		      {
// 			AliDebug(5, Form("Alloc digits for det %d rows %d cols %d tbins %d", fDET, fRowMax, fColMax, fTBins));
			digits->Allocate(fRowMax, fColMax, fTBins);
			if (man->UsesDictionaries())
			  {
			    track0->Allocate(fRowMax, fColMax, fTBins);
			    track1->Allocate(fRowMax, fColMax, fTBins);
			    track2->Allocate(fRowMax, fColMax, fTBins);
			  }
		      }
		    
		    indexes = man->GetIndexes(fDET);
		    indexes->SetSM(fSM);
		    indexes->SetStack(fSTACK);
		    indexes->SetLayer(fLAYER);
		    indexes->SetDetNumber(fDET);
		    
		    if (indexes->IsAllocated() == kFALSE)
		      {
// 			AliDebug(4, "Allocating indexes");	      
			indexes->Allocate(fRowMax, fColMax, fTBins);
		      }
		  } // is the HC header already decoded?
	      } // decode hc ok
	  }; break;

	case fkNextSM:
	  {
	    if (DecodeSM() == kFALSE)
	      AliWarning(Form("Decode SM unsuccessfull. Current Word 0x%x at pos 0x%x", *fDataWord, fPos));	  
	  }; break;

	case fkStop:
	  ; break;

	default:
	  AliWarning(Form("Unknown state %d. Last state %d. Current Word 0x%x at pos 0x%x", fNextStatus, fLastStatus, *fDataWord, fPos));  
	  ChangeStatus(fkStop);
	};

    } // not fkStop

  // we do not return chambers for which the end-of-data was not received twice (for each HC)

  //AliDebug(1, Form("That's all folks! %d", fSM));
  //return kFALSE;
  return -1;
}

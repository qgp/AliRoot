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
/// This is a class for reading the AD DDL raw data
/// The format of the raw data corresponds to the one
/// implemented in AliADBuffer class. 
///
///////////////////////////////////////////////////////////////////////////////

#include "AliADRawStream.h"
#include "AliRawReader.h"
#include "AliLog.h"
#include "AliDAQ.h"
#include "AliADCalibData.h"
ClassImp(AliADRawStream)

//_____________________________________________________________________________
AliADRawStream::AliADRawStream(AliRawReader* rawReader) :
  fTrigger(0),
  fTriggerMask(0),
  fPosition(-1),
  fRawReader(rawReader),
  fData(NULL)
{
  // create an object to read AD raw data
  //
  // select the raw data corresponding to
  // the AD detector id
  fRawReader->Reset();
  AliDebug(1,Form("Selecting raw data for detector %d",AliDAQ::DetectorID("VZERO")));
  fRawReader->Select("VZERO");

  // Initalize the containers
  for(Int_t i = 0; i < kNChannels; i++) {
    fTime[i] = fWidth[i] = 0;
    for(Int_t j = 0; j < kNEvOfInt; j++) {
      fADC[i][j] = 0;
      fIsInt[i][j] = fIsBB[i][j] = fIsBG[i][j] = kFALSE;
    }
    fBBScalers[i] = fBGScalers[i] = 0;
    for(Int_t j = 0; j < kNBunches; j++) {
      fChargeMB[i][j] = 0;
      fIsIntMB[i][j] = fIsBBMB[i][j] = fIsBGMB[i][j] = kFALSE;
    }
  }
  for(Int_t i = 0; i < kNScalers; i++) fScalers[i] = 0;
  for(Int_t i = 0; i < kNBunches; i++) fBunchNumbers[i] = 0;
}

//_____________________________________________________________________________
AliADRawStream::~AliADRawStream()
{
  // destructor
}

//_____________________________________________________________________________
void AliADRawStream::Reset()
{
  // reset raw stream params

  // Reinitalize the containers
  for(Int_t i = 0; i < kNChannels; i++) {
    fTime[i] = fWidth[i] = 0;
    for(Int_t j = 0; j < kNEvOfInt; j++) {
      fADC[i][j] = 0;
      fIsInt[i][j] = fIsBB[i][j] = fIsBG[i][j] = kFALSE;
    }
    fBBScalers[i] = fBGScalers[i] = 0;
    for(Int_t j = 0; j < kNBunches; j++) {
      fChargeMB[i][j] = 0;
      fIsIntMB[i][j] = fIsBBMB[i][j] = fIsBGMB[i][j] = kFALSE;
    }
  }
  for(Int_t i = 0; i < kNScalers; i++) fScalers[i] = 0;
  for(Int_t i = 0; i < kNBunches; i++) fBunchNumbers[i] = 0;

  fTrigger = fTriggerMask = 0;
  fPosition = -1;
  fData = NULL;

  if (fRawReader) fRawReader->Reset();
}

//_____________________________________________________________________________
Bool_t AliADRawStream::Next()
{
  // read next digit from the AD raw data stream
  // return kFALSE in case of error or no digits left

  if (fPosition >= 0) return kFALSE;

  if (!fRawReader->ReadNextData(fData)) return kFALSE;
  if (fRawReader->GetDataSize() == 0) return kFALSE;
     
  if (fRawReader->GetDataSize() != 5936) {
     fRawReader->AddFatalErrorLog(kRawDataSizeErr,Form("size %d != 5936",fRawReader->GetDataSize()));
     AliWarning(Form("Wrong AD raw data size: %d, expected 5936 bytes!",fRawReader->GetDataSize()));
     return kFALSE;
  }

  fPosition = 0;

  fTrigger = GetNextWord() & 0xffff;
  fTriggerMask = GetNextWord() & 0xffff;

  for(Int_t iScaler = 0; iScaler < kNScalers; iScaler++)
     fScalers[iScaler] = GetNextWord();

  for(Int_t iBunch = 0; iBunch < kNBunches; iBunch++)
     fBunchNumbers[iBunch] = GetNextWord();
  
  Int_t iCIU=0;
  for (Int_t  iV0CIU = 0; iV0CIU < 8; iV0CIU++) {
    
    if(iV0CIU != 1 || iV0CIU != 3) {
      for(Int_t iWord = 0; iWord<182; iWord++) GetNextWord();
      continue;
      	}
 
  // decoding of one Channel Interface Unit numbered iCIU - there are 8 channels per CIU (and 2 CIUs) :
  
    for (Int_t iChannel_Offset = iCIU*8; iChannel_Offset < (iCIU*8)+8; iChannel_Offset=iChannel_Offset+4) { 
      for(Int_t iChannel = iChannel_Offset; iChannel < iChannel_Offset+4; iChannel++) {
        for(Int_t iEvOfInt = 0; iEvOfInt < kNEvOfInt; iEvOfInt++) {
          UShort_t data = GetNextShort();
          fADC[iChannel][iEvOfInt] = data & 0x3ff;
          fIsInt[iChannel][iEvOfInt] = (data >> 10) & 0x1;
        }
      }
      for(Int_t iEvOfInt = 0; iEvOfInt < kNEvOfInt; iEvOfInt=iEvOfInt+2) {
        UShort_t data = GetNextShort();
        for(Int_t iChannel = iChannel_Offset; iChannel < iChannel_Offset+4; iChannel++) {          
          fIsBB[iChannel][iEvOfInt] = (data >>  2*(iChannel-iChannel_Offset)) & 0x1;
          fIsBG[iChannel][iEvOfInt] = (data >> (2*(iChannel-iChannel_Offset)+1)) & 0x1; 
	  if(iEvOfInt < (kNEvOfInt - 1)) {      
             fIsBB[iChannel][iEvOfInt+1] = (data >> (8+ 2*(iChannel-iChannel_Offset))) & 0x1;
             fIsBG[iChannel][iEvOfInt+1] = (data >> (8+ 2*(iChannel-iChannel_Offset)+1)) & 0x1;
	  }
        }
      }

      GetNextShort();

      for(Int_t iChannel = iChannel_Offset; iChannel < iChannel_Offset+4; iChannel++) {
        for(Int_t iBunch = 0; iBunch < kNBunches; iBunch++) {
          UShort_t data = GetNextShort();
          fChargeMB[iChannel][iBunch] = data & 0x3ff;
          fIsIntMB[iChannel][iBunch] = (data >> 10) & 0x1;
        } 
      }
   
      for(Int_t iBunch = 0; iBunch < kNBunches; iBunch=iBunch+2) {
        UShort_t data = GetNextShort();
        for(Int_t iChannel = iChannel_Offset; iChannel < iChannel_Offset+4; iChannel++) {  
          fIsBBMB[iChannel][iBunch] = (data >>  2*iBunch) & 0x1;
          fIsBGMB[iChannel][iBunch] = (data >> (2*iBunch+1)) & 0x1;
	  if(iBunch < (kNBunches - 1)) {
             fIsBBMB[iChannel][iBunch+1] = (data >> (8+2*iBunch)) & 0x1;
             fIsBGMB[iChannel][iBunch+1] = (data >> (8+2*iBunch+1)) & 0x1;
	  }	  
        }
      }
  
      GetNextShort();
   
      for(Int_t iChannel = iChannel_Offset; iChannel < iChannel_Offset+4; iChannel++) {
        fBBScalers[iChannel] = ((ULong64_t)GetNextWord()) << 32;
        fBBScalers[iChannel] |= GetNextWord();
        fBGScalers[iChannel] = ((ULong64_t)GetNextWord()) << 32;
        fBGScalers[iChannel] |= GetNextWord();
      }

    } 

    for(Int_t iChannel = (iCIU*8) + 7; iChannel >= iCIU*8; iChannel--) { 
      UInt_t time = GetNextWord();
      fTime[iChannel]  = time & 0xfff;
      fWidth[iChannel] = ((time >> 12) & 0x7f); // HPTDC used in pairing mode
    }
    iCIU++;
    // End of decoding of one CIU card
    AliWarning(Form("Number of bytes used at end of reading CIU card number %d %d", iCIU+1, fPosition)); 
    
  } // end of decoding the eight CIUs
    
  return kTRUE;
}

//_____________________________________________________________________________
UInt_t AliADRawStream::GetNextWord()
{
  // This method returns the next 32 bit word
  // inside the raw data payload.
  // The method is supposed to be endian (platform)
  // independent.
  if (!fData || fPosition < 0) AliFatal("Raw data payload buffer is not yet initialized !");

  UInt_t word = 0;
  word |= fData[fPosition++];
  word |= fData[fPosition++] << 8;
  word |= fData[fPosition++] << 16;
  word |= fData[fPosition++] << 24;

  return word;
}

//_____________________________________________________________________________
UShort_t AliADRawStream::GetNextShort()
{
  // This method returns the next 16 bit word
  // inside the raw data payload.
  // The method is supposed to be endian (platform)
  // independent.
  if (!fData || fPosition < 0) AliFatal("Raw data payload buffer is not yet initialized !");

  UShort_t word = 0;
  word |= fData[fPosition++];
  word |= fData[fPosition++] << 8;

  return word;
}

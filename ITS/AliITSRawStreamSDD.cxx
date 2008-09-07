/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id$*/

///////////////////////////////////////////////////////////////////////////////
///
/// This class provides access to ITS SDD digits in raw data.
///
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStreamSDD.h"
#include "AliRawReader.h"
#include "AliLog.h"

ClassImp(AliITSRawStreamSDD)
  
const UInt_t AliITSRawStreamSDD::fgkCodeLength[8] =  {8, 18, 2, 3, 4, 5, 6, 7};

//______________________________________________________________________
AliITSRawStreamSDD::AliITSRawStreamSDD(AliRawReader* rawReader) :
  AliITSRawStream(rawReader),
fDDLModuleMap(0),
fData(0),
fEventId(0),
fCarlosId(-1),
fChannel(0),
fJitter(0),
fDDL(0),
fResetSkip(0),
fEightBitSignal(0),
fDecompressAmbra(kTRUE)
{
// create an object to read ITS SDD raw digits
  fDDLModuleMap=new AliITSDDLModuleMapSDD();
  fDDLModuleMap->SetDefaultMap();
  Reset();
  for(Int_t im=0;im<kSDDModules;im++){
    fLowThresholdArray[im][0]=0;
    fLowThresholdArray[im][1]=0;
  }
  for(Int_t i=0;i<kFifoWords;i++) fNfifo[i]=0;
  for(Int_t i=0;i<kDDLsNumber;i++) fSkip[i]=0;
  fRawReader->Reset();
  fRawReader->Select("ITSSDD");

  for(Short_t i=0; i<kCarlosWords; i++) fICarlosWord[i]=0x30000000 + i; // 805306368+i;
  for(Short_t i=0; i<kFifoWords; i++) fIFifoWord[i]=0x30000010 + i;  // 805306384+i;
}

//______________________________________________________________________
AliITSRawStreamSDD::AliITSRawStreamSDD(const AliITSRawStreamSDD& rs) :
AliITSRawStream(rs.fRawReader),
fDDLModuleMap(rs.fDDLModuleMap),
fData(0),
fEventId(0),
fCarlosId(-1),
fChannel(0),
fJitter(0),
fDDL(0),
fResetSkip(0),
fEightBitSignal(0),
fDecompressAmbra(kTRUE)
{
  // copy constructor
  AliError("Copy constructor should not be used.");
}
//__________________________________________________________________________
AliITSRawStreamSDD& AliITSRawStreamSDD::operator=(const AliITSRawStreamSDD& rs) {
  // assignment operator
  if (this!=&rs) {}
  AliError("Assignment opertator should not be used.");
  return *this;
}

//______________________________________________________________________
AliITSRawStreamSDD::~AliITSRawStreamSDD(){
  if(fDDLModuleMap) delete fDDLModuleMap;
}
//______________________________________________________________________
UInt_t AliITSRawStreamSDD::ReadBits()
{
// read bits from the given channel
  UInt_t result = (fChannelData[fCarlosId][fChannel] & ((1<<fReadBits[fCarlosId][fChannel]) - 1));
  fChannelData[fCarlosId][fChannel] >>= fReadBits[fCarlosId][fChannel]; 
  fLastBit[fCarlosId][fChannel] -= fReadBits[fCarlosId][fChannel];
  return result;
}

//______________________________________________________________________
Int_t AliITSRawStreamSDD::DecompAmbra(Int_t value) const
{
  // AMBRA decompression (from 8 to 10 bit)
  
  if ((value & 0x80) == 0) {
    return value & 0x7f;
  } else if ((value & 0x40) == 0) {
    return 0x081 + ((value & 0x3f) << 1);
  } else if ((value & 0x20) == 0) {
    return 0x104 + ((value & 0x1f) << 3);
  } else {
    return 0x208 + ((value & 0x1f) << 4);
  }
  
}

//______________________________________________________________________
Bool_t AliITSRawStreamSDD::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left
// returns kTRUE and fCompletedModule=kFALSE when a digit is found
// returns kTRUE and fCompletedModule=kTRUE  when a module is completed (=3x3FFFFFFF footer words)

  fPrevModuleID = fModuleID;
  fDDL=fRawReader->GetDDLID();
  fCompletedModule=kFALSE;

  while (kTRUE) {
    if(fResetSkip==0){
      Bool_t kSkip = SkipHeaderWord();
      fResetSkip=1;
      if(!kSkip) return kSkip;
    }
  
    if ((fChannel < 0) || (fCarlosId < 0) || (fChannel >= 2) || (fCarlosId >= kModulesPerDDL) || (fLastBit[fCarlosId][fChannel] < fReadBits[fCarlosId][fChannel]) ) {
      if (!fRawReader->ReadNextInt(fData)) return kFALSE;  // read next word
      Int_t ddln = fRawReader->GetDDLID();
      if(ddln!=fDDL) { 
	Reset();
	fDDL=fRawReader->GetDDLID();
      }

      fChannel = -1;
      if((fData >> 16) == 0x7F00){ // jitter word
	fResetSkip=0;
	fEndWords=0;
	continue;
      }

      UInt_t nData28= fData >> 28;
      UInt_t nData30= fData >> 30;


      if (nData28== 0x02) {           // header
	fEventId = (fData >> 3) & 0x07FF; 
      } else if (nData28== 0x04) {
	// JTAG word -- do nothing
      } else if (nData28== 0x03) {    // Carlos and FIFO words or Footers
	if(fData>=fICarlosWord[0]&&fData<=fICarlosWord[11]) { // Carlos Word
	  if(fEndWords==12) continue; // out of event
	  fCarlosId = fData-fICarlosWord[0];
	  Int_t iFifoIdx = fCarlosId/3;
	  fNfifo[iFifoIdx] = fCarlosId;
	} else if (fData>=fIFifoWord[0]&&fData<=fIFifoWord[3]){ // FIFO word
	  if(fEndWords==12) continue; // out of event
	  fCarlosId = fNfifo[fData-fIFifoWord[0]];	    
	} else if(fData==0x3FFFFFFF){ // Carlos footer
	  fICountFoot[fCarlosId]++; // stop before the last word (last word=jitter)
	  if(fICountFoot[fCarlosId]==3){
	    fCompletedModule=kTRUE;
	    //	      printf("Completed module %d DDL %d\n",fCarlosId,ddln);
	    return kTRUE;
	  }
	} else if(fData==0x3F1F1F1F){ // CarlosRX footer
	  fEndWords++;
	  if(fEndWords<=12) continue;
	}else{
	  fRawReader->AddMajorErrorLog(kDataError,"Too many footers");
	  AliWarning(Form("invalid data: too many footers\n", fData));
	  return kFALSE;	    
	}
      } else if (nData30 == 0x02 || nData30 == 0x03) {
	fChannel = nData30-2;
	fChannelData[fCarlosId][fChannel] += 
	  (ULong64_t(fData & 0x3FFFFFFF) << fLastBit[fCarlosId][fChannel]);
	fLastBit[fCarlosId][fChannel] += 30;
      } else {                               // unknown data format
	fRawReader->AddMajorErrorLog(kDataFormatErr,Form("Invalid data %8.8x",fData));
	AliWarning(Form("invalid data: %8.8x\n", fData));
	return kFALSE;
      }
      
      if(fCarlosId>=0 && fCarlosId <kModulesPerDDL){
	 fModuleID = GetModuleNumber(ddln,fCarlosId);
      }
    } else {  // decode data
      if (fReadCode[fCarlosId][fChannel]) {// read the next code word
	fChannelCode[fCarlosId][fChannel] = ReadBits();
	fReadCode[fCarlosId][fChannel] = kFALSE;
	fReadBits[fCarlosId][fChannel] = fgkCodeLength[fChannelCode[fCarlosId][fChannel]];
      } else {                      // read the next data word
	UInt_t data = ReadBits();
	fReadCode[fCarlosId][fChannel] = kTRUE;
	fReadBits[fCarlosId][fChannel] = 3;
	if (fChannelCode[fCarlosId][fChannel] == 0) {         // set the time bin	  
	  fTimeBin[fCarlosId][fChannel] = data;
	} else if (fChannelCode[fCarlosId][fChannel] == 1) {  // next anode
	  fTimeBin[fCarlosId][fChannel] = 0;
	  fAnode[fCarlosId][fChannel]++;
	} else {                                   // ADC signal data
	  fEightBitSignal=data + (1 << fChannelCode[fCarlosId][fChannel]);
	  if(fDecompressAmbra) fSignal = DecompAmbra(fEightBitSignal + fLowThresholdArray[fModuleID-kSPDModules][fChannel]);
	  fCoord1 = fAnode[fCarlosId][fChannel];
	  fCoord2 = fTimeBin[fCarlosId][fChannel];
	  fTimeBin[fCarlosId][fChannel]++;
	  //printf("Data read, Module=%d , Anode=%d , Time=%d , Charge=%d\n",fModuleID,fCoord1,fCoord2,fSignal);
	  return kTRUE;
	}
      }
    }
  }
  return kFALSE;
}

//______________________________________________________________________
void AliITSRawStreamSDD::Reset(){

  //reset data member for a new ddl
  for(Int_t i=0;i<2;i++){
    for(Int_t ic=0;ic<kModulesPerDDL;ic++){
      fChannelData[ic][i]=0;
      fLastBit[ic][i]=0;
      fChannelCode[ic][i]=0;
      fReadCode[ic][i]=kTRUE;
      fReadBits[ic][i]=3;
      fTimeBin[ic][i]=0;
      fAnode[ic][i]=0;     
    }
    fLowThreshold[i]=0;
  }
  for(Int_t i=0;i<kModulesPerDDL;i++) fICountFoot[i]=0;
}

//______________________________________________________________________
Bool_t AliITSRawStreamSDD::SkipHeaderWord(){
  // skip the 1 DDL header word = 0xffffffff
  while (kTRUE) {
    if (!fRawReader->ReadNextInt(fData)) return kFALSE;    
    if ((fData >> 30) == 0x01) continue;  // JTAG word
    if(fData==0xFFFFFFFF) return kTRUE;
  }
}


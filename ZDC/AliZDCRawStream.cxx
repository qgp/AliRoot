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
//									     //
// This class provides access to ZDC digits in raw data.		     //
//									     //
// It loops over all ZDC digits in the raw data given by the AliRawReader.   //
// The Next method goes to the next digit. If there are no digits left	     //
// it returns kFALSE.							     //
// Getters provide information about the current digit. 		     //
//									     //
///////////////////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include "AliZDCRawStream.h"
#include "AliRawReader.h"
#include "AliRawDataHeader.h"
#include "AliRawEventHeaderBase.h"
#include "AliLog.h"

ClassImp(AliZDCRawStream)


//_____________________________________________________________________________
AliZDCRawStream::AliZDCRawStream(AliRawReader* rawReader) :
  fRawReader(rawReader),
  fBuffer(0),
  fReadOutCard(-1),
  fEvType(0),
  fPosition(0),
  fIsCalib(kFALSE),
  fIsDARCHeader(kFALSE),
  fIsHeaderMapping(kFALSE),
  fIsChMapping(kFALSE),
  fIsADCDataWord(kFALSE),
  fIsADCHeader(kFALSE),
  fIsADCEOB(kFALSE),
  fSODReading(kFALSE),
  fIsMapRead(kFALSE),
  fDeadfaceOffset(-1),
  fDeadbeefOffset(-1),
  fDataOffset(0),
  fModType(-1),
  fADCModule(-1),
  fADCNChannels(-1),	 
  fADCChannel(-1),	 
  fADCValue(-1),	 
  fADCGain(-1),
  fIsUnderflow(kFALSE),
  fIsOverflow(kFALSE),
  fScGeo(0),	  
  fScNWords(0),	  
  fScTriggerSource(0),	  
  fScTriggerNumber(0),
  fIsScEventGood(kTRUE),
  fIsScHeaderRead(kFALSE),
  fScStartCounter(0),
  fScEvCounter(0),
  fIsScalerWord(kFALSE),
  fDetPattern(0),
  fTrigCountNWords(0),
  fIsTriggerScaler(kFALSE),
  fTrigCountStart(0),
  fMBTrigInput(0),	   
  fCentralTrigInput(0), 
  fSCentralTrigInput(0),
  fEMDTrigInput(0),     
  fL0Received(0),	   
  fMBtrig2CTP(0),	   
  fCentralTrig2CTP(0),  
  fSCentralTrig2CTP(0), 
  fEMDTrig2CTP(0),	      
  fTrigHistNWords(0),
  fIsTriggerHistory(kFALSE),
  fTrigHistStart(0),
  fPileUpBit1stWord(0),
  fL0Bit1stWord(0),
  fCentralTrigHist(0),
  fMBTrigHist(0),
  fPileUpBit2ndWord(0),
  fL0Bit2ndWord(0), 
  fSCentralTrigHist(0),
  fEMDTrigHist(0),
  fNChannelsOn(0),
  fCurrentCh(-1),
  fCabledSignal(-1),
  fCurrScCh(-1),
  fIsADCEventGood(kTRUE),
  fIsL0BitSet(kTRUE),
  fIsPileUpEvent(kFALSE),
  fIsZDCTDCHeader(kFALSE),
  fIsTDCHeaderRead(kFALSE),
  fTDCStartCounter(0),
  fIsADDChannel(kFALSE),
  fIsADDTDCHeader(kFALSE),
  fIsADDTDCdatum(kFALSE)
{
  // Create an object to read ZDC raw digits
  fRawReader->Reset();
  fRawReader->Select("ZDC");
  //
  const int kNch = 48;
  for(Int_t i=0; i<kNch; i++){
    for(Int_t j=0; j<5; j++){
      fMapADC[i][j]=-1;
      if(i<32) fScalerMap[i][j]=-1;
    }
  }
  
  for(Int_t k=0; k<4; k++) fCPTInput[k] = 0;

}

//_____________________________________________________________________________
AliZDCRawStream::AliZDCRawStream(const AliZDCRawStream& stream) :
  TObject(stream),
  fRawReader(stream.fRawReader),
  fBuffer(stream.GetRawBuffer()),
  fReadOutCard(stream.GetReadOutCard()),
  fEvType(stream.fEvType),
  fPosition(stream.fPosition),
  fIsCalib(stream.fIsCalib),
  fIsDARCHeader(stream.fIsDARCHeader), 
  fIsHeaderMapping(stream.fIsHeaderMapping),
  fIsChMapping(stream.fIsChMapping),
  fIsADCDataWord(stream.fIsADCDataWord), 
  fIsADCHeader(stream.fIsADCHeader), 
  fIsADCEOB(stream.fIsADCEOB), 
  fSODReading(stream.fSODReading),
  fIsMapRead(stream.fIsMapRead),
  fDeadfaceOffset(stream.GetDeadfaceOffset()),
  fDeadbeefOffset(stream.GetDeadbeefOffset()),
  fDataOffset(stream.GetDataOffset()),
  fModType(stream.GetModType()),
  fADCModule(stream.GetADCModule()),	 
  fADCNChannels(stream.GetADCNChannels()),	 
  fADCChannel(stream.GetADCChannel()),	 
  fADCValue(stream.GetADCValue()),	 
  fADCGain(stream.GetADCGain()),
  fIsUnderflow(stream.fIsUnderflow),
  fIsOverflow(stream.fIsOverflow),
  fScGeo(stream.GetScGeo()),	  
  fScNWords(stream.GetScNWords()),	  
  fScTriggerSource(stream.GetScTriggerSource()),	  
  fScTriggerNumber(stream.fScTriggerNumber),
  fIsScEventGood(stream.fIsScEventGood),
  fIsScHeaderRead(stream.fIsScHeaderRead),
  fScStartCounter(stream.fScStartCounter),
  fScEvCounter(stream.fScEvCounter),
  fIsScalerWord(stream.fIsScalerWord),
  fDetPattern(stream.fDetPattern),
  fTrigCountNWords(stream.fTrigCountNWords),
  fIsTriggerScaler(stream.fIsTriggerScaler),
  fTrigCountStart(stream.fTrigCountStart),
  fMBTrigInput(stream.fMBTrigInput),	   
  fCentralTrigInput(stream.fCentralTrigInput), 
  fSCentralTrigInput(stream.fSCentralTrigInput),
  fEMDTrigInput(stream.fEMDTrigInput),     
  fL0Received(stream.fL0Received),	   
  fMBtrig2CTP(stream.fMBtrig2CTP),	   
  fCentralTrig2CTP(stream.fCentralTrig2CTP),  
  fSCentralTrig2CTP(stream.fSCentralTrig2CTP), 
  fEMDTrig2CTP(stream.fEMDTrig2CTP),	      
  fTrigHistNWords(stream.fTrigHistNWords),
  fIsTriggerHistory(stream.fIsTriggerHistory),
  fTrigHistStart(stream.fTrigHistStart),
  fPileUpBit1stWord(stream.fPileUpBit1stWord),
  fL0Bit1stWord(stream.fL0Bit1stWord), 
  fCentralTrigHist(stream.fCentralTrigHist),
  fMBTrigHist(stream.fMBTrigHist),
  fPileUpBit2ndWord(stream.fPileUpBit2ndWord),
  fL0Bit2ndWord(stream.fL0Bit2ndWord), 
  fSCentralTrigHist(stream.fSCentralTrigHist),
  fEMDTrigHist(stream.fEMDTrigHist),
  fNChannelsOn(stream.fNChannelsOn),
  fCurrentCh(stream.fCurrentCh),
  fCabledSignal(stream.GetCabledSignal()),
  fCurrScCh(stream.fCurrScCh),
  fIsADCEventGood(stream.fIsADCEventGood),
  fIsL0BitSet(stream.fIsL0BitSet),
  fIsPileUpEvent(stream.fIsPileUpEvent),
  fIsZDCTDCHeader(stream.fIsZDCTDCHeader),
  fIsTDCHeaderRead(stream.fIsTDCHeaderRead),
  fTDCStartCounter(stream.fTDCStartCounter),
  fIsADDChannel(stream.fIsADDChannel),
  fIsADDTDCHeader(stream.fIsADDTDCHeader),
  fIsADDTDCdatum(stream.fIsADDTDCdatum)

{
  // Copy constructor
  const int kNch = 48;
  for(Int_t j=0; j<2; j++) fSector[j] = stream.GetSector(j);	 
  for(Int_t i=0; i<kNch; i++){
    for(Int_t j=0; j<5; j++){
      fMapADC[i][j] = stream.fMapADC[i][j];
      if(i<32) fScalerMap[i][j] = stream.fMapADC[i][j];
    }
  }
  
  for(Int_t k=0; k<4; k++) fCPTInput[k] = stream.fCPTInput[k];
}

//_____________________________________________________________________________
AliZDCRawStream& AliZDCRawStream::operator = (const AliZDCRawStream& 
					      /* stream */)
{
  // Assignment operator
  Fatal("operator =", "assignment operator not implemented");
  return *this;
}

//_____________________________________________________________________________
AliZDCRawStream::~AliZDCRawStream()
{
// Destructor

}

//_____________________________________________________________________________
void AliZDCRawStream::ReadChMap()
{
  // Reading channel map
  const int kNch = 48;
  AliDebug(2,"\t Reading ZDC ADC mapping from OCDB\n");
  AliZDCChMap * chMap = GetChMap();
  //chMap->Print("");
  for(Int_t i=0; i<kNch; i++){
    fMapADC[i][0] = chMap->GetADCModule(i);
    fMapADC[i][1] = chMap->GetADCChannel(i);
    fMapADC[i][2] = -1;
    fMapADC[i][3] = chMap->GetDetector(i);
    fMapADC[i][4] = chMap->GetSector(i);
  }
  fIsMapRead = kTRUE;
}

//_____________________________________________________________________________
void AliZDCRawStream::ReadCDHHeader()
{
  // Reading CDH 
  const AliRawDataHeader* header = fRawReader->GetDataHeader();
  if(!header) {
      AliError(" No CDH in raw data streaming");
      fRawReader->AddMajorErrorLog(kCDHError);
      return;
  }
  else{
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Data Size = %x\n",fRawReader->GetDataSize());

    UChar_t message = header->GetAttributes();
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Attributes %x\n",message);
    
    if((message & 0xf0) == 0x0){ // PHYSICS RUN
       //printf("\t PHYSICS RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x10){ // COSMIC RUN
       //printf("\t STANDALONE_COSMIC RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x20){ // PEDESTAL RUN
       //printf("\t STANDALONE_PEDESTAL RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x30){ // LASER RUN
       //printf("\t STANDALONE_LASER RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x40){ // CALIBRATION_CENTRAL RUN
       //printf("\t CALIBRATION_CENTRAL RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x50){ // CALIBRATION_SEMICENTRAL
       //printf("\t CALIBRATION_SEMICENTRAL RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x60){ // CALIBRATION_MB
       //printf("\t CALIBRATION_MB RUN raw data found\n");
    }
    else if((message & 0xf0) == 0x70){ // CALIBRATION_EMD
       //printf("\t CALIBRATION_EMD RUN raw data found\n");
    }
    // *** Checking the bit indicating the used readout card
    // (the payload is different in the 2 cases!)
    if((message & 0x08) == 0){  // ** DARC card
       fReadOutCard = 0;
       fIsDARCHeader = kTRUE;
       //AliInfo("\t ZDC readout card used: DARC");
    }
    else if((message & 0x08) == 0x08){  // ** ZRC card
       fReadOutCard = 1;
       //AliInfo("\t ZDC readout card used: ZRC");
    }

    if(header->GetL1TriggerMessage() & 0x1){ // Calibration bit set in CDH
      fIsCalib = kTRUE;
    }
    //printf("\t AliZDCRawStream::ReadCDHHeader -> L1TriggerMessage %x\n",header->GetL1TriggerMessage());
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Calibration bit = %d\n",fIsCalib);    
    
/*    UInt_t status = header->GetStatus();
    //printf("\t AliZDCRawStream::ReadCDHHeader -> status = %d\n",status);
    if((status & 0x000f) == 0x0001){
      AliDebug(2,"CDH -> DARC trg0 overlap error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x000f) == 0x0002){
      AliDebug(2,"CDH -> DARC trg0 missing error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x000f) == 0x0004){
      AliDebug(2,"CDH -> DARC data parity error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x000f) == 0x0008){
      AliDebug(2,"CDH -> DARC ctrl parity error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if((status & 0x00f0) == 0x0010){
      AliDebug(2,"CDH -> DARC trg unavailable");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x00f0) == 0x0020){
      AliDebug(2,"CDH -> DARC FEE error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if((status & 0x0f00) == 0x0200){
      AliDebug(2,"CDH -> DARC L1 time violation");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x0f00) == 0x0400){
      AliDebug(2,"CDH -> DARC L2 time-out");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if((status & 0x0f00) == 0x0800){
      AliDebug(2,"CDH -> DARC prepulse time violation");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if((status & 0xf000) == 0x1000){
      AliDebug(2,"CDH -> DARC other error");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    */
  }
  
}

//_____________________________________________________________________________
Bool_t AliZDCRawStream::Next()
{
  // Read the next raw digit
  // Returns kFALSE if there is no digit left

  if(!fRawReader->ReadNextInt((UInt_t&) fBuffer)) return kFALSE;
  const int kNch = 48;
  Int_t kFirstADCGeo=0, kLastADCGeo=3;
  Int_t kADDADCGeo=5, kADDTDCGeo=6;
  Int_t kZDCTDCGeo=8, kScalerGeo=16;
  Int_t kPUGeo=29, kTrigScales=30, kTrigHistory=31;
  //
  fIsHeaderMapping = kFALSE; fIsChMapping = kFALSE; 
  fIsADCHeader = kFALSE; fIsADCDataWord = kFALSE; fIsADCEOB = kFALSE;
  fIsADDChannel = kFALSE; fIsADDTDCHeader= kFALSE; fIsADDTDCdatum=kFALSE;
  fIsUnderflow = kFALSE; fIsOverflow = kFALSE; fIsScalerWord = kFALSE;
  fSector[0] = fSector[1] = -1;
  for(Int_t kl=0; kl<4; kl++) fCPTInput[kl] = 0;

  fEvType = fRawReader->GetType();
  if(fPosition==0){
    //if(fEvType==7 || fEvType ==8){ //Physics or calibration event
      ReadCDHHeader();
    //}
    fCurrentCh=0; fCurrScCh=0; fNChannelsOn=0;
    // Ch. debug
    //printf("\n  AliZDCRawStream::Next() - ev. type %d",fEvType);
  }
  
  // Ch. debug
  //printf("\n  AliZDCRawStream::Next() - fBuffer[%d] = %x\n",fPosition, fBuffer);
  
  // *** End of ZDC event
  if(fBuffer == 0xcafefade){
    //printf("\n  AliZDCRawStream::Next() ***** End of ZDC event *****\n\n");
    return kFALSE;
  }
  
  // -------------------------------------------
  // --- DARC header
  // -------------------------------------------
  // If the CDH has been read then 
  // the DARC header must follow
  if(fReadOutCard==0 && fIsDARCHeader){
    //printf("\t ---- DARC header ----\n");
    if(fIsCalib){
      fDeadfaceOffset = 9;
      fDeadbeefOffset = 25;
    }
    else{
      fDeadfaceOffset = 1;
      fDeadbeefOffset = 7;
    }
    fDataOffset = 1+fDeadbeefOffset;
    fIsDARCHeader = kFALSE;
  }
    
  // ---------------------------------------------
  // --- Start of data event (SOD)             ---
  // --- decoding mapping of connected ADC ch. ---
  // ---------------------------------------------
  // In the SOD event ADC ch. mapping is written
  if(fEvType==10){
   if(fSODReading){
    
    if(fPosition>=fDataOffset){
      if((fBuffer&0xff000001) == 0xff000001){ // ************** Mapping
        // DARC 1st datum @ fDataOffset+1 \ ZRC 1st valid datum @ fDataOffset=0
        if((fPosition==fDataOffset+1) || (fPosition==fDataOffset)){ 
	   printf("\n\n ------ AliZDCRawStream -> Reading mapping from StartOfData event ------\n");
	   fCurrentCh=0; fCurrScCh=0;	
        }
	else{
	  printf(" ------ AliZDCRawStream -> End of ZDC StartOfData event ------\n\n");
          fSODReading = kFALSE;
	  return kFALSE;
	}
      }
      else if((fBuffer&0xff000002) == 0xff000002){ // ************** Threshold settings
        fPosition++;
	return kFALSE; // !!!!!!!!!!!!!!!!!!!!!  For the moment thresholds are not read
      }
      else if((fBuffer&0x80000000)>>31 == 1){ // --- Mapping header
        fIsHeaderMapping = kTRUE;
	fADCModule = ((fBuffer & 0x7f000000)>>24); // GEO address!!!
	fModType = ((fBuffer & 0x7ff00)>>8); 
	fADCNChannels = (fBuffer & 0xff);          // # of channels following the header
	//
	printf("  ******** GEO %d, mod. type %d, #ch. %d\n",fADCModule,fModType,fADCNChannels);
      }
      else if((fBuffer&0x80000000)>>31 == 0){ // --- Mapping channel
	fADCChannel = ((fBuffer & 0x3fff0000)>>16);
	fCabledSignal = (fBuffer&0xffff);
        //
	if(fModType == kV965){ // ******** ADCs ********************************
          // Channel signal
	  if((fBuffer&0x40000000)>>30==0 && fADCModule<=kLastADCGeo){ // *ZDC* high range chain ADC
            fIsChMapping = kTRUE;
	    fMapADC[fCurrentCh][0] = fADCModule;
	    fMapADC[fCurrentCh][1] = fADCChannel;
	    fMapADC[fCurrentCh][2] = fCabledSignal;
	    //  - No. of channels on
	    fNChannelsOn++;
	    //
	    // Determining detector and sector
	    // -----------------------------------------
	    //  For the decoding of the following lines 
	    //  look the enum in AliZDCRawStream.h file
	    // -----------------------------------------
	    if((fCabledSignal>=2 && fCabledSignal<=6) || (fCabledSignal>=26 && fCabledSignal<=30)
	      || fCabledSignal==24 || fCabledSignal==48){
	      fMapADC[fCurrentCh][3] = 4; //ZNA
	      //
	      if(fCabledSignal==kZNAC || fCabledSignal==kZNACoot)   fMapADC[fCurrentCh][4]=0;
	      else if(fCabledSignal==3 || fCabledSignal==27)  fMapADC[fCurrentCh][4]=1;
	      else if(fCabledSignal==4 || fCabledSignal==28)  fMapADC[fCurrentCh][4]=2;
	      else if(fCabledSignal==5 || fCabledSignal==29)  fMapADC[fCurrentCh][4]=3;
	      else if(fCabledSignal==6 || fCabledSignal==30)  fMapADC[fCurrentCh][4]=4;
	      else if(fCabledSignal==24 || fCabledSignal==48) fMapADC[fCurrentCh][4]=5; //Reference PTM
	    }
	    else if((fCabledSignal>=7 && fCabledSignal<=11) || (fCabledSignal>=31 && fCabledSignal<=35)){
	      fMapADC[fCurrentCh][3] = 5; //ZPA
	      //
	      if(fCabledSignal==7 || fCabledSignal==31)       fMapADC[fCurrentCh][4]=0;
	      else if(fCabledSignal==8 || fCabledSignal==32)  fMapADC[fCurrentCh][4]=1;
	      else if(fCabledSignal==9 || fCabledSignal==33)  fMapADC[fCurrentCh][4]=2;
	      else if(fCabledSignal==10 || fCabledSignal==34) fMapADC[fCurrentCh][4]=3;
	      else if(fCabledSignal==11 || fCabledSignal==35) fMapADC[fCurrentCh][4]=4;
	    }
	    else if((fCabledSignal>=12 && fCabledSignal<=16) || (fCabledSignal>=36 && fCabledSignal<=40)
	       || fCabledSignal==25 || fCabledSignal==49){
	      fMapADC[fCurrentCh][3] = 1; //ZNC
	      //
	      if(fCabledSignal==12 || fCabledSignal==36)      fMapADC[fCurrentCh][4]=0;
	      else if(fCabledSignal==13 || fCabledSignal==37) fMapADC[fCurrentCh][4]=1;
	      else if(fCabledSignal==14 || fCabledSignal==38) fMapADC[fCurrentCh][4]=2;
	      else if(fCabledSignal==15 || fCabledSignal==39) fMapADC[fCurrentCh][4]=3;
	      else if(fCabledSignal==16 || fCabledSignal==40) fMapADC[fCurrentCh][4]=4;
	      else if(fCabledSignal==25 || fCabledSignal==49) fMapADC[fCurrentCh][4]=5; //Reference PTM
	    }
	    else if((fCabledSignal>=17 && fCabledSignal<=21) || (fCabledSignal>=41 && fCabledSignal<=45)){
	      fMapADC[fCurrentCh][3] = 2; //ZPC
	      //
	      if(fCabledSignal==17 || fCabledSignal==41)      fMapADC[fCurrentCh][4]=0;
	      else if(fCabledSignal==18 || fCabledSignal==42) fMapADC[fCurrentCh][4]=1;
	      else if(fCabledSignal==19 || fCabledSignal==43) fMapADC[fCurrentCh][4]=2;
	      else if(fCabledSignal==20 || fCabledSignal==44) fMapADC[fCurrentCh][4]=3;
	      else if(fCabledSignal==21 || fCabledSignal==45) fMapADC[fCurrentCh][4]=4;
	    }
	    else if(fCabledSignal==22 || fCabledSignal==23 || fCabledSignal==46 || fCabledSignal==47){
	      fMapADC[fCurrentCh][3] = 3; // ZEM
	      //
	      if(fCabledSignal==22 || fCabledSignal==46)      fMapADC[fCurrentCh][4]=1;
	      else if(fCabledSignal==23 || fCabledSignal==47) fMapADC[fCurrentCh][4]=2;
	    }
	    //Ch. debug
      	    //printf("\tADC mod. %d ch. %d signal %d ",fADCModule,fADCChannel,fCabledSignal);
	    //printf("  det. %d sec. %d\n",fMapADC[fCurrentCh][3],fMapADC[fCurrentCh][4]);
	    //
	    fCurrentCh++;
	    //
	  } // high range channels
        }// ModType=1 (ADC mapping)
        else if(fModType == kV830){  // ******** VME scaler **************************
          fIsChMapping = kTRUE;
	  fScalerMap[fCurrScCh][0] = fADCModule;
	  fScalerMap[fCurrScCh][1] = fADCChannel;
	  fScalerMap[fCurrScCh][2] = fCabledSignal;
	  //
	  // Determining detector and sector
	  // -----------------------------------------
	  //  For the decoding of the following lines 
	  //  look the enum in AliZDCRawStream.h file
	  // -----------------------------------------
	  // NOT CONSIDERING OUT OF TIME OR REFERENCE SIGNALS FOR SCALER!!!!!
	  if((fCabledSignal>=2 && fCabledSignal<=6) ||
	     (fCabledSignal>=61 && fCabledSignal<=65)){
	    fScalerMap[fCurrScCh][3] = 4; //ZNA
	    //
	    if(fCabledSignal==2 || fCabledSignal==61)      fScalerMap[fCurrScCh][4]=0;
	    else if(fCabledSignal==3 || fCabledSignal==62)  fScalerMap[fCurrScCh][4]=1;
	    else if(fCabledSignal==4 || fCabledSignal==63)  fScalerMap[fCurrScCh][4]=2;
	    else if(fCabledSignal==5 || fCabledSignal==64)  fScalerMap[fCurrScCh][4]=3;
	    else if(fCabledSignal==6 || fCabledSignal==65)  fScalerMap[fCurrScCh][4]=4;
	  }
	  else if((fCabledSignal>=7 && fCabledSignal<=11) ||
	     (fCabledSignal>=66 && fCabledSignal<=70)){
	    fScalerMap[fCurrScCh][3] = 5; //ZPA
	    //
	    if(fCabledSignal==7 || fCabledSignal==66)      fScalerMap[fCurrScCh][4]=0;
	    else if(fCabledSignal==8 || fCabledSignal==67)  fScalerMap[fCurrScCh][4]=1;
	    else if(fCabledSignal==9 || fCabledSignal==68)  fScalerMap[fCurrScCh][4]=2;
	    else if(fCabledSignal==10 || fCabledSignal==69) fScalerMap[fCurrScCh][4]=3;
	    else if(fCabledSignal==11 || fCabledSignal==70) fScalerMap[fCurrScCh][4]=4;
	  }
	  else if((fCabledSignal>=12 && fCabledSignal<=16) ||
	     (fCabledSignal>=71 && fCabledSignal<=75)){
	    fScalerMap[fCurrScCh][3] = 1; //ZNC
	    //
	    if(fCabledSignal==12 || fCabledSignal==71)      fScalerMap[fCurrScCh][4]=0;
	    else if(fCabledSignal==13 || fCabledSignal==72) fScalerMap[fCurrScCh][4]=1;
	    else if(fCabledSignal==14 || fCabledSignal==73) fScalerMap[fCurrScCh][4]=2;
	    else if(fCabledSignal==15 || fCabledSignal==74) fScalerMap[fCurrScCh][4]=3;
	    else if(fCabledSignal==16 || fCabledSignal==75) fScalerMap[fCurrScCh][4]=4;
	  }
	  else if((fCabledSignal>=17 && fCabledSignal<=21) ||
	     (fCabledSignal>=76 && fCabledSignal<=80)){
	    fScalerMap[fCurrScCh][3] = 2; //ZPC
	    //
	    if(fCabledSignal==17 || fCabledSignal==76)      fScalerMap[fCurrScCh][4]=0;
	    else if(fCabledSignal==18 || fCabledSignal==77) fScalerMap[fCurrScCh][4]=1;
	    else if(fCabledSignal==19 || fCabledSignal==78) fScalerMap[fCurrScCh][4]=2;
	    else if(fCabledSignal==20 || fCabledSignal==79) fScalerMap[fCurrScCh][4]=3;
	    else if(fCabledSignal==21 || fCabledSignal==80) fScalerMap[fCurrScCh][4]=4;
	  }
	  else if(fCabledSignal==22 || fCabledSignal==23 ||
	          fCabledSignal==81 || fCabledSignal==82){
	    fScalerMap[fCurrScCh][3] = 3; // ZEM
	    //
	    if(fCabledSignal==22 || fCabledSignal==81)     fScalerMap[fCurrScCh][4]=1;
	    else if(fCabledSignal==23 || fCabledSignal==82) fScalerMap[fCurrScCh][4]=2;
	  }
      	  // Ch debug.
	  //printf("\t VME scaler mod. %d ch. %d, signal %d",fScalerMap[fCurrScCh][0],fADCChannel,fCabledSignal);
	  //if(fCabledSignal!=0 && fCabledSignal!=1) printf("  det. %d sec. %d\n",fScalerMap[fCurrScCh][3],fScalerMap[fCurrScCh][4]);
	  //else printf("  Signal void/not connected\n");
	  
          fCurrScCh++;
        }
	/*else if(fModType == kTRG){ // **** scalers from trigger card
      	  //printf("\t Trigger scaler mod. %d ch. %d, signal %d\n",fADCModule,fADCChannel,fCabledSignal);	  
        }
	else if(fModType == kTRGI){ // **** trigger history from trigger card
      	  //printf("\t Trigger card ch. %d, signal %d\n",fADCChannel,fCabledSignal);
        }
	else if(fModType == kPU){ // **** pattern unit
      	  //printf("\t P.U. mod. %d ch. %d, signal %d\n",fADCModule,fADCChannel,fCabledSignal);
        }*/
      }//reading channel mapping
    }
   } // if fSODREading
   fPosition++;
   return kTRUE;
  } // ------------------------------- SOD event
  
  // -------------------------------------------
  // --- DARC data
  // -------------------------------------------
  if(fPosition<fDeadfaceOffset && fReadOutCard==0){
    fPosition++;
    return kTRUE;
  }
  else if(fPosition==fDeadfaceOffset && fReadOutCard==0){
    if(fBuffer != 0xdeadface){
      //AliWarning(" NO deadface after DARC data");
      fRawReader->AddMajorErrorLog(kDARCError); 
    }
    else{
      fPosition++;
      return kTRUE;
    }
  }
  else if(fPosition>fDeadfaceOffset && fPosition<fDeadbeefOffset && fReadOutCard==0){
    fPosition++;
    return kTRUE;
  }
  else if(fPosition==fDeadbeefOffset && fReadOutCard==0){
    if(fBuffer != 0xdeadbeef){
      //AliWarning(" NO deadbeef after DARC global data");
      fRawReader->AddMajorErrorLog(kDARCError);  
      fPosition++;
      return kFALSE;
    }
    else{
      fPosition++;
      return kTRUE;
    }
  } // ------------------------------- End of DARC data
  
  // ---------------------------------------------
  // --- ZDC data
  // --- ADCs + VME scaler + trigger card + P.U.
  // ---------------------------------------------
  else if(fPosition>=fDataOffset){
    
    //printf("AliZDCRawStream: fSODReading = %d\n", fSODReading);
    if(!fSODReading && !fIsMapRead) ReadChMap();
    
    //  !!!!!!!!!!!!!!! DARC readout card only !!!!!!!!!!!
    // Not valid datum before the event 
    // there MUST be a NOT valid datum before the event!!!
    if(fReadOutCard==0){
      if(fPosition==fDataOffset){ 
        //printf("\t **** ZDC data begin ****\n");
        if((fBuffer & 0x07000000) != 0x06000000){
          fRawReader->AddMajorErrorLog(kZDCDataError);
        }
        else if((fBuffer & 0x07000000) == 0x06000001){ // Corrupted event!!!
          fIsADCEventGood = kFALSE;
        }
      }
    
      // If the not valid datum isn't followed by the 1st ADC header
      // the event is corrupted (i.e., 2 gates arrived before trigger)
      else if(fPosition==fDataOffset+1){ 
        if((fBuffer & 0x07000000) != 0x02000000){
          AliWarning("ZDC ADC -> The not valid datum is NOT followed by an ADC header!");
          fRawReader->AddMajorErrorLog(kZDCDataError);
          fIsADCEventGood = kFALSE;
	  fPosition++;
	  return kFALSE;
        }
      }
    }
     
    // Get geo address of current word
    fADCModule = (Int_t) ((fBuffer & 0xf8000000)>>27);
    if(fIsTDCHeaderRead)fADCModule = kZDCTDCGeo;
    
    // ************************************ ADC MODULES ************************************
    if(fADCModule>=kFirstADCGeo && fADCModule<=kLastADCGeo){
      // *** ADC header
      if((fBuffer & 0x07000000) == 0x02000000){
        fIsADCHeader = kTRUE;
    	fADCNChannels = ((fBuffer & 0x00003f00)>>8);
    	//printf("  AliZDCRawStream -> ADC HEADER: mod.%d has %d ch. \n",fADCModule,fADCNChannels);
      }
      // *** ADC data word
      else if((fBuffer & 0x07000000) == 0x00000000){
        fIsADCDataWord = kTRUE;
        fADCChannel = ((fBuffer & 0x1e0000) >> 17);
        fADCGain = ((fBuffer & 0x10000) >> 16);       
        fADCValue = (fBuffer & 0xfff);  
    	//
	//printf("  AliZDCRawStream -> ADC DATUM: mod. %d ch. %d gain %d value %d\n",
	//  fADCModule,fADCChannel,fADCGain,fADCValue);
	
	// Checking if the channel map for the ADCs has been provided/read
	if(fMapADC[0][0]==-1){
	  printf("\t ATTENTION!!! No ADC mapping has been found/provided!!!\n");
	  return kFALSE;
	}
	//
	/*for(Int_t ci=0; ci<kNch; ci++){
	  printf("  %d mod %d ch %d det %d sec %d\n",ci,fMapADC[ci][0],
          fMapADC[ci][1], fMapADC[ci][3], fMapADC[ci][4]);
	}*/
		
	// Scan of the map to assign the correct volumes
	Int_t foundMapEntry = kFALSE;
	for(Int_t k=0; k<kNch; k++){
	   if(fADCModule==fMapADC[k][0] && fADCChannel==fMapADC[k][1]){
	     fSector[0] = fMapADC[k][3];
	     fSector[1] = fMapADC[k][4];
	     foundMapEntry = kTRUE;
	     break;
	   } 
	}
	if(foundMapEntry==kFALSE){
	  AliWarning(Form(" No valid entry in ADC mapping for raw data %d ADCmod. %d ch. %d\n",
	      fPosition,fADCModule,fADCChannel));
	}
	//
	//printf("AliZDCRawStream -> ADCmod. %d ch. %d -> det %d sec %d\n",
	//  fADCModule,fADCChannel,fSector[0],fSector[1]);
	
	// Final checks
	if(foundMapEntry==kTRUE){
	  if(fSector[0]<1 || fSector[0]>5){
            AliWarning(Form(" No valid detector assignment: %d",fSector[0]));
            fRawReader->AddMajorErrorLog(kInvalidSector);
	  }
	  //
	  if(fSector[1]<0 || fSector[1]>5){
            AliWarning(Form(" No valid sector assignment: %d",fSector[1]));
            fRawReader->AddMajorErrorLog(kInvalidSector);
	  }
	  //
	  if(fADCModule<0 || fADCModule>3){
            AliError(Form(" No valid ADC module: %d",fADCModule));
            fRawReader->AddMajorErrorLog(kInvalidADCModule);
          }
	}

	// Checking the underflow and overflow bits
        if(fBuffer & 0x1000)       fIsUnderflow = kTRUE;
	else if (fBuffer & 0x2000) fIsOverflow = kTRUE; 
        	
      }//ADC data word
      // *** ADC EOB
      else if((fBuffer & 0x07000000) == 0x04000000){
        fIsADCEOB = kTRUE;
    	//printf("  AliZDCRawStream -> EOB --------------------------\n");
      }
    }//ADC module
    // ********************************* ADD ADC *********************************
    else if(fADCModule == kADDADCGeo){
      // *** ADC header
      if((fBuffer & 0x07000000) == 0x02000000){
        fIsADCHeader = kTRUE;
    	fADCNChannels = ((fBuffer & 0x00003f00)>>8);
    	//printf("  AliZDCRawStream -> ADD ADC HEADER: mod.%d has %d ch. \n",fADCModule,fADCNChannels);
      }
      // *** ADC data word
      else if((fBuffer & 0x07000000) == 0x00000000){
        fIsADDChannel = kTRUE;
        fADCChannel = ((fBuffer & 0x1e0000) >> 17);
        fADCGain = ((fBuffer & 0x10000) >> 16);       
        fADCValue = (fBuffer & 0xfff);  
    	//
	//printf("  ADD ADC DATUM -> mod. %d ch. %d gain %d value %d\n",
	//  fADCModule,fADCChannel,fADCGain,fADCValue);
      }
      // *** ADC EOB
      else if((fBuffer & 0x07000000) == 0x04000000){
        fIsADCEOB = kTRUE;
    	//printf("  AliZDCRawStream -> EOB --------------------------\n");
      }
    }
    // ********************************* ZDC TDC *********************************
    else if(fADCModule==kZDCTDCGeo && fIsTDCHeaderRead==kFALSE){
      fIsZDCTDCHeader = kTRUE;
      fIsTDCHeaderRead = kTRUE;
      fTDCStartCounter = fPosition;
      //Ch. debug
      //printf("  AliZDCRawStream -> ZDC TDC: mod.%d\n",fADCModule);
    }
    // ********************************* ADD TDC *********************************
    else if(fADCModule == kADDTDCGeo){
      // *** TDC header
      if((fBuffer & 0x07000000) == 0x02000000){
        fIsADDTDCHeader = kTRUE;
    	fADCNChannels = ((fBuffer & 0x00003f00)>>8);
        //printf("  AliZDCRawStream -> ADD TDC: mod.%d has %d ch. \n",fADCModule,fADCNChannels);
      }
      // *** TDC  word
      else if((fBuffer & 0x07000000) == 0x00000000){
        fIsADDTDCdatum = kTRUE;
      }
      // *** TDC  EOB
      if((fBuffer & 0x07000000) == 0x04000000){
        fIsADCEOB = kTRUE;
    	//printf("  AliZDCRawStream -> ADD TDC EOB\n");
      }
   }
    // ********************************* VME SCALER HEADER *********************************
    else if(fADCModule == kScalerGeo){
      if(fBuffer & 0x04000000 && fIsScHeaderRead==kFALSE){ // *** Scaler header
        fScGeo = (fBuffer & 0xf8000000)>>27;	   
        fScNWords = (fBuffer & 0x00fc0000)>>18;	   
        fScTriggerSource = (fBuffer & 0x00030000)>>16;	   
        fScTriggerNumber = (fBuffer & 0x0000ffff);
        fIsScHeaderRead = kTRUE; 
	fScStartCounter = fPosition;
        //Ch. debug
        //printf("  AliZDCRawStream -> VME SCALER HEADER: geo %d Nwords %d TrigSource %d TrigNo. %d\n",
        //   fScGeo,fScNWords,fScTriggerSource,fScTriggerNumber);
      } 
      else if(!(fBuffer & 0x04000000)){
        fIsScEventGood = kFALSE;
      }
    }
    // *********************************** PATTERN UNIT ***********************************
    else if(fADCModule == kPUGeo){
      // still to be implemented!!! Not yet in data!!!
      fDetPattern = (fBuffer & 0x0000003f);
      // Ch. debug
      //printf("  AliZDCRawStream -> Pattern Unit\n");
      
    }
    // ******************************** TRIGGER CARD COUNTS ********************************
    else if(fADCModule == kTrigScales){
      if(fIsTriggerScaler == kFALSE){
        fTrigCountNWords = (Int_t) ((fBuffer & 0xfc0000)>>17);
        fTrigCountStart = fPosition;
	fIsTriggerScaler = kTRUE;
      }
      // Ch. debug
      //printf("  AliZDCRawStream -> Trigger Scaler header\n");      
    }
    // ********************************** TRIGGER HISTORY **********************************
    else if(fADCModule == kTrigHistory){
      if(fIsTriggerHistory == kFALSE){
        fTrigHistNWords = (Int_t) ((fBuffer & 0xfc0000)>>17);
	fTrigHistStart = fPosition;
        fIsTriggerHistory = kTRUE;
      }
      // Ch. debug
      //printf("  AliZDCRawStream -> Trigger History header\n");
      
    } 
    // ********************************** VME SCALER DATA **********************************
    //  Reading VME scaler data 
    if(fIsScHeaderRead && fPosition>=fScStartCounter+1){ // *** Scaler word
      fADCModule=kScalerGeo; fIsADCDataWord=kFALSE; fIsScalerWord=kTRUE;
      fScEvCounter = fBuffer;
      Int_t nWords = (Int_t) (fScNWords);
      if(fPosition == fScStartCounter+nWords) fIsScHeaderRead = kFALSE;
      //Ch. debug
      //printf("  AliZDCRawStream -> scaler datum %d", fScEvCounter);
    }
    // ********************************** ZDC TDC DATA **********************************
    //  ZDC TDC data
    if(fIsTDCHeaderRead && fPosition>=fTDCStartCounter+1){ 
      fIsADCDataWord=kFALSE; fIsScalerWord=kFALSE;
      //printf("  AliZDCRawStream -> ...skipping ZDC TDC datum\n");
      // For the moment we skip the TDC data
      if(((fBuffer & 0xf0000000) == 0x80000000) && ((fBuffer & 0x08000000) >> 27) == 0){
	fIsTDCHeaderRead = kFALSE;
        // Ch. debug
        //printf("  AliZDCRawStream -> ZDC TDC trailer\n");
      }
    }
    // ******************************** TRIGGER SCALER DATA ********************************
    //  Reading trigger scaler data 
    if(fIsTriggerScaler && fPosition>=fTrigCountStart+1){
      fADCModule = kTrigScales; fIsADCDataWord = kFALSE;	
      if(fPosition == fTrigCountStart+1)      fMBTrigInput = fBuffer;		    
      else if(fPosition == fTrigCountStart+2) fCentralTrigInput = fBuffer;		    
      else if(fPosition == fTrigCountStart+3) fSCentralTrigInput = fBuffer;
      else if(fPosition == fTrigCountStart+4) fEMDTrigInput = fBuffer; 
      else if(fPosition == fTrigCountStart+5) fL0Received = fBuffer;
      else if(fPosition == fTrigCountStart+6) fMBtrig2CTP = fBuffer;	 
      else if(fPosition == fTrigCountStart+7) fCentralTrig2CTP = fBuffer;  
      else if(fPosition == fTrigCountStart+8) fSCentralTrig2CTP = fBuffer; 
      else if(fPosition == fTrigCountStart+9){
        fEMDTrig2CTP = fBuffer;       
        fIsTriggerScaler = kFALSE;
      }
      // Ch. debug
      //printf("  AliZDCRawStream -> Trigger Scaler datum %d\n", fPosition-fTrigCountStart);
    }
    // ******************************* TRIGGER HISTORY WORDS ******************************
    //  Reading trigger history
    if(fIsTriggerHistory && fPosition>=fTrigHistStart+1){
	fADCModule = kTrigHistory; fIsADCDataWord = kFALSE;	
	if(fPosition == fTrigHistStart+1){
	  fPileUpBit1stWord = (fBuffer & 0x80000000) >> 31;
	  fL0Bit1stWord = (fBuffer & 0x40000000) >> 30;        
	  fCentralTrigHist = (fBuffer & 0x3fff8000) >> 14; 
	  fMBTrigHist =  (fBuffer & 0x00007fff);        
	  //
	  fCPTInput[0] = (fBuffer & 0x00000080) >> 6;  // MB bit
	  fCPTInput[1] = (fBuffer & 0x00400000) >> 21; // CENTRAL bit
        }
	
	else if(fPosition == fTrigHistStart+fTrigHistNWords){
          fPileUpBit2ndWord = (fBuffer & 0x80000000) >> 31;
          fL0Bit2ndWord = (fBuffer & 0x40000000) >> 30;	     
          fSCentralTrigHist = (fBuffer & 0x3fff8000) >> 14; 
          fEMDTrigHist =  (fBuffer & 0x00007fff); 	 
          //
          fCPTInput[2] = (fBuffer & 0x00000080) >> 6;  // SEMICENTRAL bit
          fCPTInput[3] = (fBuffer & 0x00400000) >> 21; // EMD bit
	  //
	  fIsTriggerHistory = kFALSE;
          
	  // Checking if the event is good
          // (1) both history word pile up bits must be = 0
          if(fPileUpBit1stWord==0 && fPileUpBit2ndWord==0) fIsPileUpEvent = kFALSE;
          else{
            fIsPileUpEvent = kTRUE;
	    printf("  AliZDCRawStream -> PILE UP EVENT: bitPileUp0 %d bitPileUp1 %d\n",
	    	fPileUpBit1stWord, fPileUpBit2ndWord);
          }
	  // (2) both history word L0 bits must be = 1
          if(fL0Bit1stWord==1 && fL0Bit2ndWord==1) fIsL0BitSet = kTRUE;
          else{
            fIsL0BitSet = kFALSE;
	    printf("  AliZDCRawStream -> L0 wrongly set: bitL0word0 %d bitL0word1 %d\n",
	    	fL0Bit1stWord, fL0Bit2ndWord);
          }
        }       
        // Ch. debug
        //printf("  AliZDCRawStream -> Trigger history word %d\n", fPosition-fTrigHistStart);
    }
    
  }

  fPosition++;

  return kTRUE;
}

//_____________________________________________________________________________
AliCDBStorage* AliZDCRawStream::SetStorage(const char *uri) 
{
  // Setting the storage
  
  AliCDBStorage *storage = AliCDBManager::Instance()->GetStorage(uri); 

  return storage; 
}


//_____________________________________________________________________________
AliZDCChMap* AliZDCRawStream::GetChMap() const
{

  // Getting calibration object for ZDC

  AliCDBEntry  *entry = AliCDBManager::Instance()->Get("ZDC/Calib/ChMap");
  if(!entry) AliFatal("No calibration data loaded!");  

  AliZDCChMap *calibdata = dynamic_cast<AliZDCChMap*> (entry->GetObject());
  if(!calibdata) AliFatal("Wrong calibration object in calibration  file!");

  return calibdata;
}

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
  fEvType(0),
  fPosition(0),
  fIsCalib(kFALSE),
  fIsDARCHeader(kFALSE),
  fIsChMapping(kFALSE),
  fIsADCDataWord(kFALSE),
  fIsADCHeader(kFALSE),
  fIsADCEOB(kFALSE),
  fSODReading(kFALSE),
  fIsMapRead(kFALSE),
  fDARCEvBlockLenght(0),  
  fDARCBlockAttributes(0),
  fDeadfaceOffset(0),
  fDeadbeefOffset(0),
  fDataOffset(0),
  fModType(-1),
  fADCModule(-1),
  fADCNChannels(-1),	 
  fADCChannel(-1),	 
  fADCValue(-1),	 
  fADCGain(-1),
  fScNWords(0),	  
  fScGeo(0),	  
  fScTS(0),	  
  fScTriggerNumber(0),
  fIsScEventGood(kFALSE),
  fNConnCh(-1),
  fCabledSignal(-1)
{
  // Create an object to read ZDC raw digits
  fRawReader->Reset();
  fRawReader->Select("ZDC");
  //
  for(Int_t i=0; i<48; i++){
    for(Int_t j=0; j<5; j++) fMapADC[i][j]=-1;
  }

}

//_____________________________________________________________________________
AliZDCRawStream::AliZDCRawStream(const AliZDCRawStream& stream) :
  TObject(stream),
  fRawReader(stream.fRawReader),
  fBuffer(stream.GetRawBuffer()),
  fEvType(stream.fEvType),
  fPosition(stream.fPosition),
  fIsCalib(stream.fIsCalib),
  fIsDARCHeader(stream.fIsDARCHeader), 
  fIsChMapping(stream.fIsChMapping),
  fIsADCDataWord(stream.fIsADCDataWord), 
  fIsADCHeader(stream.fIsADCHeader), 
  fIsADCEOB(stream.fIsADCEOB), 
  fSODReading(stream.fSODReading),
  fIsMapRead(stream.fIsMapRead),
  fDARCEvBlockLenght(stream.fDARCEvBlockLenght),  
  fDARCBlockAttributes(stream.fDARCBlockAttributes),
  fDeadfaceOffset(stream.GetDeadfaceOffset()),
  fDeadbeefOffset(stream.GetDeadbeefOffset()),
  fDataOffset(stream.GetDataOffset()),
  fModType(stream.GetModType()),
  fADCModule(stream.GetADCModule()),	 
  fADCNChannels(stream.GetADCNChannels()),	 
  fADCChannel(stream.GetADCChannel()),	 
  fADCValue(stream.GetADCValue()),	 
  fADCGain(stream.GetADCGain()),
  fScNWords(stream.GetScNWords()),	  
  fScGeo(stream.GetScGeo()),	  
  fScTS(stream.GetScTS()),	  
  fScTriggerNumber(stream.fScTriggerNumber),
  fIsScEventGood(stream.fIsScEventGood),
  fNConnCh(stream.fNConnCh),
  fCabledSignal(stream.GetCabledSignal())
{
  // Copy constructor
  for(Int_t j=0; j<2; j++) fSector[j] = stream.GetSector(j);	 
  for(Int_t i=0; i<48; i++){
    for(Int_t j=0; j<5; j++) fMapADC[i][j] = stream.fMapADC[i][j];
  }
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
  //printf("\n\t Reading map from OCDB\n");
  AliZDCChMap * chMap = GetChMap();
  for(Int_t i=0; i<48; i++){
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
      AliError("\t No CDH in raw data streaming\n");
      fRawReader->AddMajorErrorLog(kCDHError);
      return;
  }
  else{
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Data Size = %d\n",fRawReader->GetDataSize());

    fDARCEvBlockLenght = header->fSize;
    //printf("\t AliZDCRawStream::ReadCDHHeader -> fDARCEvBlockLenght = %d\n",fDARCEvBlockLenght);
    
    //UChar_t message = header->GetAttributes();
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Attributes %x\n",message);
    
    /*if(message & 0x10){ // COSMIC RUN
       printf("\t STANDALONE_COSMIC RUN raw data found\n");
    }
    else if(message & 0x20){ // PEDESTAL RUN
       printf("\t STANDALONE_PEDESTAL RUN raw data found\n");
    }
    else if(message & 0x30){ // LASER RUN
       printf("\t STANDALONE_LASER RUN raw data found\n");
    }*/
    
    if(header->GetL1TriggerMessage() & 0x1){ // Calibration bit set in CDH
      fIsCalib = kTRUE;
    }
    //printf("\t AliZDCRawStream::ReadCDHHeader -> L1TriggerMessage %x\n",header->GetL1TriggerMessage());
    //printf("\t AliZDCRawStream::ReadCDHHeader -> Calibration bit = %d\n",fIsCalib);    
    
    UInt_t status = header->GetStatus();
    //printf("\t AliZDCRawStream::ReadCDHHeader -> status = %d\n",status);
    if(status & 0x000f == 0x0001){
      AliWarning("CDH -> DARC trg0 overlap error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x000f == 0x0002){
      AliWarning("CDH -> DARC trg0 missing error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x000f == 0x0004){
      AliWarning("CDH -> DARC data parity error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x000f == 0x0008){
      AliWarning("CDH -> DARC ctrl parity error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if(status & 0x00f0 == 0x0010){
      AliWarning("CDH -> DARC trg unavailable\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x00f0 == 0x0020){
      AliWarning("CDH -> DARC FEE error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if(status & 0x0f00 == 0x0200){
      AliWarning("CDH -> DARC L1 time violation\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x0f00 == 0x0400){
      AliWarning("CDH -> DARC L2 time-out\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    if(status & 0x0f00 == 0x0800){
      AliWarning("CDH -> DARC prepulse time violation\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
    //
    if(status & 0xf000 == 0x1000){
      AliWarning("CDH -> DARC other error\n");
      fRawReader->AddMajorErrorLog(kDARCError);
    }
  }
  //
  fIsDARCHeader = kTRUE;
}

//_____________________________________________________________________________
Bool_t AliZDCRawStream::Next()
{
  // Read the next raw digit
  // Returns kFALSE if there is no digit left

  if(!fRawReader->ReadNextInt((UInt_t&) fBuffer)) return kFALSE;
  fIsChMapping = kFALSE; fIsADCHeader = kFALSE; 
  fIsADCDataWord = kFALSE; fIsADCEOB = kFALSE;
  
  fEvType = fRawReader->GetType();
  //printf("\n\t AliZDCRawStream::Next() -> ev. type %d\n",fEvType);
  
  if(fPosition==0){
    //if(fEvType==7 || fEvType ==8){ //Physics or calibration event
      //ReadEventHeader();
      ReadCDHHeader();
    //}
    fNConnCh=0;
  }
  //printf("\n  AliZDCRawStream::Next() - fBuffer[%d] = %x\n",fPosition, fBuffer);
  
  // -------------------------------------------
  // --- DARC header
  // -------------------------------------------
  // If the CDH has been read then 
  // the DARC header must follow
  if(fIsDARCHeader){
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
  
    
  // -------------------------------------------
  // --- Start of data event
  // --- decoding mapping of connected ADC ch.
  // -------------------------------------------
  // In the SOD event ADC ch. mapping is written
  if(fEvType==10 && fSODReading){
    //printf("\n  AliZDCRawStream::Next() - fBuffer[%d] = %x\n",fPosition, fBuffer);
    
    if(fPosition>fDataOffset){
      if((fBuffer&0xff000000) == 0xff000000){
        if(fPosition==(fDataOffset+1)){ 
	   printf("\n\n\t Reading ZDC mapping from StartOfData event\n");
	   fNConnCh=0;	
        }
	else{
	  printf("\n\t End of ZDC StartOfData event\n\n");
          //printf("AliZDCRawStream: fSODReading after SOD reading set to %d\n", fSODReading);
	  return kFALSE;
	}
      }
      else if((fBuffer&0x80000000)>>31 == 1){
        // Mapping identification
	fADCModule = ((fBuffer & 0x7f000000)>>24);
	fModType = ((fBuffer & 0xfff00)>>8);
	fADCNChannels = (fBuffer & 0xff);
	//
	//printf("\tGEO %d, mod. type %d, #ch. %d\n",fADCModule,fModType,fADCNChannels);
      }
      else if(fModType==1 && (fBuffer&0x80000000)>>31 == 0){
        // Channel signal
	if((fBuffer&0x40000000)>>30==0){ // high range chain ADC
	  fIsChMapping = kTRUE;
	  fADCChannel = ((fBuffer & 0x3fff0000)>>16);
	  fCabledSignal = (fBuffer&0xffff);
	  //
      	  //printf("\tADC ch. %d, signal %d\n",fADCChannel,fCabledSignal);
	  //
	  fMapADC[fNConnCh][0] = fADCModule;
	  fMapADC[fNConnCh][1] = fADCChannel;
	  fMapADC[fNConnCh][2] = fCabledSignal;
	  //
	  // Determining detector and sector
	  // -----------------------------------------
	  //  For the decoding of the following lines 
	  //  look the enum in AliZDCRawStream.h file
	  // -----------------------------------------
	  if((fCabledSignal>=2 && fCabledSignal<=6) || (fCabledSignal>=26 && fCabledSignal<=30)
	     || fCabledSignal==24 || fCabledSignal==48){
	    fMapADC[fNConnCh][3] = 4; //ZNA
	    //
	    if(fCabledSignal==2 || fCabledSignal==26)       fMapADC[fNConnCh][4]=0;
	    else if(fCabledSignal==3 || fCabledSignal==27)  fMapADC[fNConnCh][4]=1;
	    else if(fCabledSignal==4 || fCabledSignal==28)  fMapADC[fNConnCh][4]=2;
	    else if(fCabledSignal==5 || fCabledSignal==29)  fMapADC[fNConnCh][4]=3;
	    else if(fCabledSignal==6 || fCabledSignal==30)  fMapADC[fNConnCh][4]=4;
	    else if(fCabledSignal==24 || fCabledSignal==48) fMapADC[fNConnCh][4]=5;
	  }
	  else if((fCabledSignal>=7 && fCabledSignal<=11) || (fCabledSignal>=31 && fCabledSignal<=35)){
	    fMapADC[fNConnCh][3] = 5; //ZPA
	    //
	    if(fCabledSignal==7 || fCabledSignal==31) 	    fMapADC[fNConnCh][4]=0;
	    else if(fCabledSignal==8 || fCabledSignal==32)  fMapADC[fNConnCh][4]=1;
	    else if(fCabledSignal==9 || fCabledSignal==33)  fMapADC[fNConnCh][4]=2;
	    else if(fCabledSignal==10 || fCabledSignal==34) fMapADC[fNConnCh][4]=3;
	    else if(fCabledSignal==11 || fCabledSignal==35) fMapADC[fNConnCh][4]=4;
	  }
	  else if((fCabledSignal>=12 && fCabledSignal<=16) || (fCabledSignal>=36 && fCabledSignal<=40)
	     || fCabledSignal==25 || fCabledSignal==49){
	    fMapADC[fNConnCh][3] = 1; //ZNC
	    //
	    if(fCabledSignal==12 || fCabledSignal==36)      fMapADC[fNConnCh][4]=0;
	    else if(fCabledSignal==13 || fCabledSignal==37) fMapADC[fNConnCh][4]=1;
	    else if(fCabledSignal==14 || fCabledSignal==38) fMapADC[fNConnCh][4]=2;
	    else if(fCabledSignal==15 || fCabledSignal==39) fMapADC[fNConnCh][4]=3;
	    else if(fCabledSignal==16 || fCabledSignal==40) fMapADC[fNConnCh][4]=4;
	    else if(fCabledSignal==25 || fCabledSignal==49) fMapADC[fNConnCh][4]=5;
	  }
	  else if((fCabledSignal>=17 && fCabledSignal<=21) || (fCabledSignal>=41 && fCabledSignal<=45)){
	    fMapADC[fNConnCh][3] = 2; //ZPC
	    //
	    if(fCabledSignal==17 || fCabledSignal==41) 	    fMapADC[fNConnCh][4]=0;
	    else if(fCabledSignal==18 || fCabledSignal==42) fMapADC[fNConnCh][4]=1;
	    else if(fCabledSignal==19 || fCabledSignal==43) fMapADC[fNConnCh][4]=2;
	    else if(fCabledSignal==20 || fCabledSignal==44) fMapADC[fNConnCh][4]=3;
	    else if(fCabledSignal==21 || fCabledSignal==45) fMapADC[fNConnCh][4]=4;
	  }
	  else if(fCabledSignal==22 || fCabledSignal==23 || fCabledSignal==46 || fCabledSignal==47){
	    fMapADC[fNConnCh][3] = 3;
	    //
	    if(fCabledSignal==22 || fCabledSignal==46)      fMapADC[fNConnCh][4]=1;
	    else if(fCabledSignal==23 || fCabledSignal==47) fMapADC[fNConnCh][4]=2;
	  }
	  //
	  //printf("AliZDCRawStream -> datum %d mod. %d ch. %d signal %d, det %d, tow %d\n",
	  //   fNConnCh,fADCModule,fADCChannel,fBuffer&0xffff,fMapADC[fNConnCh][3],fMapADC[fNConnCh][4]);
	  //
	  fNConnCh++;
	  if(fNConnCh>48){
	    // Protection manually set since it returns:
	    // RawData48 mod. 3 ch. 2048 signal 515
	    // to be checked with Pietro!!!!!!!!!!!!!!!!!!!!!!!
	    //AliWarning("\t AliZDCRawStream -> ERROR: no. of cabled channels > 48!!!\n");
	    return kTRUE;
	  }
	}
      }// ModType=1 (ADC mapping)
    }
    fPosition++;
    return kTRUE;
  } // SOD event
  
  // -------------------------------------------
  // --- DARC data
  // -------------------------------------------
  if(fPosition<fDeadfaceOffset){
    fPosition++;
    return kTRUE;
  }
  else if(fPosition==fDeadfaceOffset){
    if(fBuffer != 0xdeadface){
      AliError("AliZDCRawStream -> NO deadface after DARC data\n");
      fRawReader->AddMajorErrorLog(kDARCError);  
    }
    else{
      fPosition++;
      return kTRUE;
    }
  }
  
  // -------------------------------------------
  // --- DARC global data
  // -------------------------------------------
  else if(fPosition>fDeadfaceOffset && fPosition<fDeadbeefOffset){
    fPosition++;
    return kTRUE;
  }
  else if(fPosition==fDeadbeefOffset){
    if(fBuffer != 0xdeadbeef){
      AliError("AliZDCRawStream -> NO deadbeef after DARC global data\n");
      fRawReader->AddMajorErrorLog(kDARCError);  
    }
    else{
      fPosition++;
      return kTRUE;
    }
  }

  // -------------------------------------------
  // --- ZDC data
  // --- ADC buffer + scaler
  // -------------------------------------------
  else if(fPosition>=fDataOffset){
    
    //printf("AliZDCRawStream: fSODReading = %d\n", fSODReading);
    if(!fSODReading && !fIsMapRead) ReadChMap();
    
    // Not valid datum before the event 
    // there MUST be a NOT valid datum before the event!!!
    if(fPosition==fDataOffset){
      //printf("\t **** ZDC data begin ****\n");
      if((fBuffer & 0x07000000) == 0x06000000){
        //printf("    AliZDCRawStream -> Not valid datum in ADC %d,"
        //       "position %d in word data buffer\n",fADCModule,fPosition);
      }
      else fRawReader->AddMajorErrorLog(kZDCDataError);
    }
    
    // If the not valid datum isn't followed by the 1st ADC header
    // the event is corrupted (i.e., 2 gates arrived before trigger)
    else if(fPosition==fDataOffset+1){
      if((fBuffer & 0x07000000) != 0x02000000){
        AliWarning("ZDC ADC -> The not valid datum is NOT followed by an ADC header!\n");
        fRawReader->AddMajorErrorLog(kZDCDataError);
      }
    }
     
    // Get geo address of current word to determine
    // if it is a scaler word (geo address == kScalerAddress)
    // if it is an ADC word (geo address != 8)
    Int_t kScalerAddress=8;
    fADCModule = ((fBuffer & 0xf8000000)>>27);
    if(fADCModule == kScalerAddress){
      DecodeScaler();
    }
    else{//ADC module
      // *** End of event
      if(fBuffer == 0xcafefade){
        //printf("  AliZDCRawStream ->  End of ZDC event!\n");
      }
      // *** ADC header
      else if((fBuffer & 0x07000000) == 0x02000000){
        fIsADCHeader = kTRUE;
    	fADCNChannels = ((fBuffer & 0x00003f00)>>8);
    	//printf("  AliZDCRawStream -> HEADER: ADC mod.%d has %d ch. \n",fADCModule,fADCNChannels);
      }
      // *** ADC data word
      else if((fBuffer & 0x07000000) == 0x00000000){
        fIsADCDataWord = kTRUE;
        fADCChannel = ((fBuffer & 0x1e0000) >> 17);
        fADCGain = ((fBuffer & 0x10000) >> 16);       
        fADCValue = (fBuffer & 0xfff);  
    	
	//printf("  AliZDCRawStream -> DATA: ADC mod. %d ch. %d gain %d value %d\n",
	//  fADCModule,fADCChannel,fADCGain,fADCValue);

	// Valid ADC data (not underflow nor overflow)
        if(!(fBuffer & 0x1000) && !(fBuffer & 0x2000)){ 
	
	  // Checking if the channel map for the ADCs has been provided/read
	  if(fMapADC[0][0]==-1){
	    printf("\t ATTENTION!!! No ADC mapping has been found/provided!!!\n");
	    return kFALSE;
	    // Temporary solution (to be changed!!!!)
	    // mapping read from ShuttleInput dir
	    /*char * mapFileName=gSystem->ExpandPathName("$(ALICE_ROOT)/ZDC/ShuttleInput/ZDCChMapping.dat");
	    FILE *file; 
	    Int_t ival[48][6];
	    if((file = fopen(mapFileName,"r")) != NULL){
	      for(Int_t j=0; j<48; j++){
	        for(Int_t k=0; k<6; k++){
	          fscanf(file,"%d",&ival[j][k]);
		}
		fMapADC[j][0] = ival[j][1];
		fMapADC[j][1] = ival[j][2];
		fMapADC[j][2] = ival[j][3];
		fMapADC[j][3] = ival[j][4];
		fMapADC[j][4] = ival[j][5];
              }
  	    }
	    else{
     		printf("File %s not found\n",mapFileName);
     		return kFALSE;
	    }*/
	  }
	  //
	  /*for(Int_t ci=0; ci<48; ci++){
	    printf("  %d mod. %d ch. %d signal %d\n",ci,fMapADC[ci][0],
            fMapADC[ci][1], fMapADC[ci][2]);
	  }
	  */
	  
	  // Scan of the map to assign the correct volumes
	  for(Int_t k=0; k<48; k++){
	     if(fADCModule==fMapADC[k][0] && fADCChannel==fMapADC[k][1]){
	       fSector[0] = fMapADC[k][3];
	       fSector[1] = fMapADC[k][4];
	       break;
	     } 
	  }
	  //
	  //printf("AliZDCRawStream -> ADCmod. %d ADCch %d det %d, sec %d\n",
	  //  fADCModule,fADCChannel,fSector[0],fSector[1]);
	  
	  // Final checks
	  if(fSector[0]<1 || fSector[0]>5){
            AliError(Form("	 AliZDCRawStream -> No valid detector assignment: %d\n",fSector[0]));
            fRawReader->AddMajorErrorLog(kInvalidSector);
	  }
	  //
	  if(fSector[1]<0 || fSector[1]>5){
            AliError(Form("	 AliZDCRawStream -> No valid sector assignment: %d\n",fSector[1]));
            fRawReader->AddMajorErrorLog(kInvalidSector);
	  }
	  //
	  if(fADCModule<0 || fADCModule>3){
            AliError(Form("	 AliZDCRawStream -> No valid ADC module: %d\n",fADCModule));
            fRawReader->AddMajorErrorLog(kInvalidADCModule);
          }

        }//No underflow nor overflow	
      }//ADC data word
      // *** ADC EOB
      else if((fBuffer & 0x07000000) == 0x04000000){
        fIsADCEOB = kTRUE;
    	//printf("  AliZDCRawStream -> EOB --------------------------\n");
      }
     }//ADC module
        
    
  }
  fPosition++;

  return kTRUE;
}

//_____________________________________________________________________________
void AliZDCRawStream::DecodeScaler()
{
  // Decoding scaler event
  
  if(!fBuffer & 0x04000000){
    AliWarning("	AliZDCRawStream -> Scaler header corrupted\n");
    fIsScEventGood = kFALSE; 
  }
  Int_t scNwords = (Int_t) fScNWords;
  if(fPosition==scNwords && fBuffer != 0x0){
    AliWarning("	AliZDCRawStream -> Scaler trailer corrupted\n");
    fIsScEventGood = kFALSE; 
  }
  fIsScEventGood = kTRUE;
  
  if(fPosition==0){
    fScNWords = (fBuffer & 0x00fc0000)>>18;	   
    fScGeo = (fBuffer & 0xf8000000)>>27;	   
    fScTS = (fBuffer & 0x00030000)>>16;	   
    fScTriggerNumber = (fBuffer & 0x0000ffff);
  }
   
  fPosition++;
  
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

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

/*
$Log$
Revision 1.19.1  2008/09/19  preghenella
  Decode method updated:
  it reads the CDH from the rawReader and sends it to the decoder;
 LoadRawDataBuffers modified:
     it corrects tof hit infos per ddlBC and deltaBC offsets
     (in case of the static member fgApplyBCCorrections
      has been setted to kTRUE);
 Added static member fgApplyBCCorrections (kTRUE by default)
 and the related static method ApplyBCCorrections;

Revision 1.19  2007/05/18 13:07:53  decaro
Error messages stored in the global raw-reader error log (Cvetan, Chiara)

Revision 1.18  2007/05/08 11:53:29  arcelli
Improved class flexibility for further use (R.Preghenella)

Revision 1.17  2007/05/03 08:53:50  decaro
Coding convention: RS3 violation -> suppression

Revision 1.16  2007/05/03 08:22:22  decaro
Coding convention: RN17 violation -> suppression

Revision 1.15  2007/04/30 15:22:06  arcelli
Change TOF digit Time, Tot etc to int type

Revision 1.14  2007/04/27 11:11:53  arcelli
updates for the new decoder

Revision 1.13  2007/03/16 11:46:35  decaro
Coding convention: RN17 rule violation -> suppression

Revision 1.12  2007/02/22 09:43:45  decaro
Added AliTOFRawStream::GetIndex method for online calibration (C.Zampolli)

Revision 1.11  2007/02/20 15:57:00  decaro
Raw data update: to read the TOF raw data defined in UNPACKED mode

Revision 1.10  2006/12/15 14:01:38  cvetan
Memory leak fixed

Revision 1.9  2006/10/13 11:22:27  arcelli
remove warnings due to uninitialized AliTOFtdcDigit data members

Revision 1.8  2006/08/22 13:30:17  arcelli
removal of effective c++ warnings (C.Zampolli)

Revision 1.7  2006/08/10 14:46:54  decaro
TOF raw data format: updated version

Revision 1.6.1  2006/06/28 A. De Caro, R. Preghenella:
        Update TOF raw data format
        according to the final version
        (see the ALICE internal note in preparation
         'ALICE TOF raw data format')
        Added the methods for the correspoonding numbering
         between the equipment IDs and the volume IDs:
           Equip2VolNPlate(...)
           Equip2VolNStrip(...)
           Equip2VolNPad(...)

Revision 0.02  2005/07/28 A. De Caro:
        Update format TOF raw data
               (temporary solution) 
        Correction of few wrong corrispondences
               between 'software' and 'hardware' numberings

Revision 0.01  2005/07/22 A. De Caro
        Implement methods Next()
	                  GetSector(),
	                  GetPlate(),
	                  GetStrip(),
	                  GetPadZ(),
	                  GetPadX()
*/

////////////////////////////////////////////////////////////////////////
//                                                                    //
//     This class provides access to TOF raw data in DDL files.       //
//                                                                    //
//      It loops over all TOF raw data given by the AliRawReader.     //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include "Riostream.h"

#include "TClonesArray.h"

#include "AliDAQ.h"
#include "AliLog.h"
#include "AliRawReader.h"

#include "AliTOFGeometry.h"
#include "AliTOFrawData.h"
#include "AliTOFRawMap.h"
#include "AliTOFRawStream.h"
//#include "AliTOFCableLengthMap.h"

#include "AliTOFHitData.h"

#include "AliRawEventHeaderBase.h"
#include "AliRawDataHeader.h"

ClassImp(AliTOFRawStream)

const Int_t AliTOFRawStream::fgkddlBCshift[72] = 
{
  2, 2, -1, -1,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2,  0,  0,
  2, 2, -1, -1,
  2, 2, -1, -1,
  2, 2, -2, -2,
  2, 2, -2, -2,
  2, 2, -2, -2,
  2, 2, -1, -1,
  2, 2, -1, -1,
  2, 2, -1, -1
};

Bool_t AliTOFRawStream::fgApplyBCCorrections = kTRUE;
//_____________________________________________________________________________
AliTOFRawStream::AliTOFRawStream(AliRawReader* rawReader):
  fRawReader(rawReader),
  fTOFrawData(0x0),
  fDecoder(new AliTOFDecoder()),
  fDDL(-1),
  fTRM(-1),
  fTRMchain(-1),
  fTDC(-1),
  fTDCchannel(-1),
  fTime(-1),
  fToT(-1),
  fLeadingEdge(-1),
  fTrailingEdge(-1),
  fErrorFlag(-1),
  fSector(-1),
  fPlate(-1),
  fStrip(-1),
  fPadX(-1),
  fPadZ(-1),
  fPackedDigits(0),
  fWordType(-1),
  fSlotID(-1),
  fACQ(-1),
  fPSbit(-1),
  fTDCerrorFlag(-1),
  fInsideDRM(kFALSE),
  fInsideTRM(kFALSE),
  fInsideLTM(kFALSE),
  fInsideTRMchain0(kFALSE),
  fInsideTRMchain1(kFALSE),
  fLocalEventCounterDRM(-1),
  fLocalEventCounterLTM(-1),
  fLocalEventCounterTRM(0x0),
  fLocalEventCounterChain(0x0),
  fChainBunchID(0x0),
  fCableLengthMap(0x0),
  fEventID(0)
{
  //
  // create an object to read TOF raw digits
  //

  for (Int_t i=0;i<AliDAQ::NumberOfDdls("TOF");i++){
    fDataBuffer[i]=new AliTOFHitDataBuffer();
    fPackedDataBuffer[i]=new AliTOFHitDataBuffer();
  }

  fTOFrawData = new TClonesArray("AliTOFrawData",1000);
  fTOFrawData->SetOwner();

  fRawReader->Reset();
  fRawReader->Select("TOF");

  fLocalEventCounterTRM = new Int_t[13];
  fLocalEventCounterChain = new Int_t*[13];
  fChainBunchID = new Int_t*[13];
  for (Int_t j=0;j<13;j++){
    fLocalEventCounterTRM[j] = -1;
    fLocalEventCounterChain[j] = new Int_t[2];
    fChainBunchID[j] = new Int_t[2];
    for (Int_t k=0;k<2;k++){
      fLocalEventCounterChain[j][k] = -1;
      fChainBunchID[j][k] = -1;
    }
  }

  fCableLengthMap = new AliTOFCableLengthMap();

  fEventID = (Int_t)fRawReader->GetBCID(); //bunch crossing

}

//_____________________________________________________________________________
AliTOFRawStream::AliTOFRawStream():
  fRawReader(0x0), 
  fTOFrawData(0x0),
  fDecoder(new AliTOFDecoder()),
  fDDL(-1),
  fTRM(-1),
  fTRMchain(-1),
  fTDC(-1),
  fTDCchannel(-1),
  fTime(-1),
  fToT(-1),
  fLeadingEdge(-1),
  fTrailingEdge(-1),
  fErrorFlag(-1),
  fSector(-1),
  fPlate(-1),
  fStrip(-1),
  fPadX(-1),
  fPadZ(-1),
  fPackedDigits(0),
  fWordType(-1),
  fSlotID(-1),
  fACQ(-1),
  fPSbit(-1),
  fTDCerrorFlag(-1),
  fInsideDRM(kFALSE),
  fInsideTRM(kFALSE),
  fInsideLTM(kFALSE),
  fInsideTRMchain0(kFALSE),
  fInsideTRMchain1(kFALSE),
  fLocalEventCounterDRM(-1),
  fLocalEventCounterLTM(-1),
  fLocalEventCounterTRM(0x0),
  fLocalEventCounterChain(0x0),
  fChainBunchID(0x0),
  fCableLengthMap(0x0),
  fEventID(0)
{
  //
  // default ctr
  //
  for (Int_t i=0;i<AliDAQ::NumberOfDdls("TOF");i++){
    fDataBuffer[i]=new AliTOFHitDataBuffer();
    fPackedDataBuffer[i]=new AliTOFHitDataBuffer();
  }

  fTOFrawData = new TClonesArray("AliTOFrawData",1000);
  fTOFrawData->SetOwner();

  fLocalEventCounterTRM = new Int_t[13];
  fLocalEventCounterChain = new Int_t*[13];
  fChainBunchID = new Int_t*[13];
  for (Int_t j=0;j<13;j++){
    fLocalEventCounterTRM[j] = -1;
    fLocalEventCounterChain[j] = new Int_t[2];
    fChainBunchID[j] = new Int_t[2];
    for (Int_t k=0;k<2;k++){
      fLocalEventCounterChain[j][k] = -1;
      fChainBunchID[j][k] = -1;
    }
  }

  fCableLengthMap = new AliTOFCableLengthMap();

}

//_____________________________________________________________________________
AliTOFRawStream::AliTOFRawStream(const AliTOFRawStream& stream) :
  TObject(stream),
  fRawReader(stream.fRawReader),
  fTOFrawData(stream.fTOFrawData),
  fDecoder(new AliTOFDecoder()),
  fDDL(stream.fDDL),
  fTRM(stream.fTRM),
  fTRMchain(stream.fTRMchain),
  fTDC(stream.fTDC),
  fTDCchannel(stream.fTDCchannel),
  fTime(stream.fTime),
  fToT(-stream.fToT),
  fLeadingEdge(stream.fLeadingEdge),
  fTrailingEdge(stream.fTrailingEdge),
  fErrorFlag(stream.fErrorFlag),
  fSector(stream.fSector),
  fPlate(stream.fPlate),
  fStrip(stream.fStrip),
  fPadX(stream.fPadX),
  fPadZ(stream.fPadZ),
  fPackedDigits(stream.fPackedDigits),
  fWordType(stream.fWordType),
  fSlotID(stream.fSlotID),
  fACQ(stream.fACQ),
  fPSbit(stream.fPSbit),
  fTDCerrorFlag(stream.fTDCerrorFlag),
  fInsideDRM(stream.fInsideDRM),
  fInsideTRM(stream.fInsideTRM),
  fInsideLTM(stream.fInsideLTM),
  fInsideTRMchain0(stream.fInsideTRMchain0),
  fInsideTRMchain1(stream.fInsideTRMchain1),
  fLocalEventCounterDRM(stream.fLocalEventCounterDRM),
  fLocalEventCounterLTM(stream.fLocalEventCounterLTM),
  fLocalEventCounterTRM(0x0),
  fLocalEventCounterChain(0x0),
  fChainBunchID(0x0),
  fCableLengthMap(stream.fCableLengthMap),
  fEventID(stream.fEventID)
{
  //
  // copy constructor
  //

  for (Int_t i=0;i<AliDAQ::NumberOfDdls("TOF");i++){
    fDataBuffer[i]= new AliTOFHitDataBuffer(*stream.fDataBuffer[i]);
    fPackedDataBuffer[i]= new AliTOFHitDataBuffer(*stream.fPackedDataBuffer[i]);
  }

  fTOFrawData = new TClonesArray(*stream.fTOFrawData);

  for (Int_t j=0;j<13;j++){
    fLocalEventCounterTRM[j] = stream.fLocalEventCounterTRM[j];
    for (Int_t k=0;k<2;k++){
      fLocalEventCounterChain[j][k] = stream.fLocalEventCounterChain[j][k];
      fChainBunchID[j][k] = stream.fChainBunchID[j][k];
    }
  }

}

//_____________________________________________________________________________
AliTOFRawStream& AliTOFRawStream::operator = (const AliTOFRawStream& stream)
{
  //
  // assignment operator
  //

  if (this == &stream)
    return *this;

  TObject::operator=(stream);

  fRawReader = stream.fRawReader;

  fTOFrawData = stream.fTOFrawData;

  fDDL = stream.fDDL;
  fTRM = stream.fTRM;
  fTRMchain = stream.fTRMchain;
  fTDC = stream.fTDC;
  fTDCchannel = stream.fTDCchannel;
  fTime = stream.fTime;
  fToT = stream.fToT;
  fLeadingEdge = stream.fLeadingEdge;
  fTrailingEdge = stream.fTrailingEdge;
  fErrorFlag = stream.fErrorFlag;

  fSector = stream.fSector;
  fPlate = stream.fPlate;
  fStrip = stream.fStrip;
  fPadX = stream.fPadX;
  fPadZ = stream.fPadZ;

  fPackedDigits = stream.fPackedDigits;

  fWordType = stream.fWordType;
  fSlotID = stream.fSlotID;
  fACQ = stream.fACQ;
  fPSbit = stream.fPSbit;
  fTDCerrorFlag = stream.fTDCerrorFlag;
  fInsideDRM = stream.fInsideDRM;
  fInsideTRM = stream.fInsideTRM;
  fInsideLTM = stream.fInsideLTM;
  fInsideTRMchain0 = stream.fInsideTRMchain0;
  fInsideTRMchain1 = stream.fInsideTRMchain1;
  
  for (Int_t i=0;i<AliDAQ::NumberOfDdls("TOF");i++){ 
    fDataBuffer[i] = stream.fDataBuffer[i];
    fPackedDataBuffer[i] = stream.fPackedDataBuffer[i];
  }
  
  fTOFrawData = stream.fTOFrawData;

  fLocalEventCounterDRM = stream.fLocalEventCounterDRM;
  fLocalEventCounterLTM = stream.fLocalEventCounterLTM;
  for (Int_t j=0;j<13;j++){
    fLocalEventCounterTRM[j] = stream.fLocalEventCounterTRM[j];
    for (Int_t k=0;k<2;k++){
      fLocalEventCounterChain[j][k] = stream.fLocalEventCounterChain[j][k];
      fChainBunchID[j][k] = stream.fChainBunchID[j][k];
    }
  }

  fCableLengthMap = stream.fCableLengthMap;

  fEventID = stream.fEventID;

  return *this;

}

//_____________________________________________________________________________
AliTOFRawStream::~AliTOFRawStream()
{
// destructor

  fPackedDigits = 0;

  for (Int_t i=0;i<72;i++){ 
    delete fDataBuffer[i];
    delete fPackedDataBuffer[i];
  }

  delete fDecoder;

  fTOFrawData->Clear();
  delete fTOFrawData;

  delete [] fLocalEventCounterTRM;
  for (Int_t ii=0; ii<2; ii++) {
    delete [] fLocalEventCounterChain[ii];
    delete [] fChainBunchID[ii];
  }
  delete fCableLengthMap;

}


//_____________________________________________________________________________

void AliTOFRawStream::LoadRawData(Int_t indexDDL)
{
  //
  // To load raw data
  //

  fTOFrawData->Clear();

  TClonesArray &arrayTofRawData =  *fTOFrawData;

  fPackedDigits = 0;

  // create raw data map
  AliTOFRawMap *rawMap = new AliTOFRawMap(fTOFrawData);
  rawMap->Clear();

  Int_t slot[4] = {-1, -1, -1, -1};

  fLocalEventCounterDRM = -1;
  fLocalEventCounterLTM = -1;
  for (Int_t ii=0; ii<13; ii++)
    fLocalEventCounterTRM[ii] = -1;
  for (Int_t ii=0; ii<13; ii++)
    for (Int_t jj=0; jj<2; jj++)
      fLocalEventCounterChain[ii][jj] = -1;

  fRawReader->Reset();
  fRawReader->Select("TOF", indexDDL, indexDDL);
    
  Bool_t signal = kFALSE;

  AliTOFrawData *rawDigit = NULL;

  while(Next()) {

    signal = (fSector!=-1 && fPlate!=-1 && fStrip!=-1 && fPadZ!=-1 && fPadX!=-1);
    if (signal) {
      AliDebug(2,Form("  %2i  %1i  %2i  %1i  %2i", fSector, fPlate, fStrip, fPadZ, fPadX));

      slot[0] = fTRM;
      slot[1] = fTRMchain;
      slot[2] = fTDC;
      slot[3] = fTDCchannel;

      if (rawMap->TestHit(slot) != kEmpty) {

	rawDigit = static_cast<AliTOFrawData*>(rawMap->GetHit(slot));

	if (rawDigit->GetLeading()!=-1 && rawDigit->GetTrailing()==-1 &&
	    fLeadingEdge==-1 && fTrailingEdge!=-1) {

	  rawDigit->Update(fTime, fToT, fLeadingEdge, fTrailingEdge, fPSbit, fACQ, fErrorFlag);
	}
	else if ( ((rawDigit->GetTOF()!=-1 || rawDigit->GetLeading()!=-1 || rawDigit->GetTrailing()!=-1) &&
		   (fLeadingEdge!=-1 || fTrailingEdge!=-1 || fTime!=-1) )

		  )
	  {

	    new (arrayTofRawData[fPackedDigits++]) AliTOFrawData(fTRM, fTRMchain, fTDC, fTDCchannel, fTime, fToT, fLeadingEdge, fTrailingEdge, fPSbit, fACQ, fErrorFlag);

	    rawMap->SetHit(slot);

	  }


      }
      else {

	new (arrayTofRawData[fPackedDigits++]) AliTOFrawData(fTRM, fTRMchain, fTDC, fTDCchannel, fTime, fToT, fLeadingEdge, fTrailingEdge, fPSbit, fACQ, fErrorFlag);

	rawMap->SetHit(slot);

      } // else if (rawMap->TestHit(slot) == kEmpty)

    } // if (signal)

  } // closed -> while (Next())

  rawMap->Delete();

}

//_____________________________________________________________________________
Bool_t AliTOFRawStream::Next()
{
  //
  // Read next 32-bit word in TOF raw data files
  // returns kFALSE if there is no word left
  //

  UInt_t data;

  Int_t dummy = 0;

  if (!fRawReader->ReadNextInt(data)) return kFALSE;

  if (fSector!=-1 && fPlate!=-1 && fStrip!=-1 && fPadZ!=-1 && fPadX!=-1) {
    fSector = -1;
    fPlate  = -1;
    fStrip  = -1;
    fPadZ   = -1;
    fPadX   = -1;
    fTime   = -1;
    fToT    = -1;
    fLeadingEdge  = -1;
    fTrailingEdge = -1;
  }

  fDDL  = fRawReader->GetDDLID();

  fWordType = GetField(data,WORD_TYPE_MASK,WORD_TYPE_POSITION);

  switch (fWordType) { // switch word type

  case GLOBAL_HEADER_TYPE: // global header
    fSlotID = GetField(data, HEADER_SLOT_ID_MASK, HEADER_SLOT_ID_POSITION);
    fTRM = fSlotID;


    switch (fSlotID) { // switch global header slot ID

    case DRM_ID_NUMBER: //DRM global header
      if (fInsideDRM) { // unexpected DRM global headers -> exit
	break;
      }
      fInsideDRM = kTRUE; // DRM global header accepted
      break;

    case LTM_ID_NUMBER: // LTM global header
      if (fInsideLTM) { // unexpected LTM global headers -> exit
	break;
      }
      fInsideLTM = kTRUE; // LTM global header accepted
      break;

    case  3: //TRM header
    case  4: //TRM header
    case  5: //TRM header
    case  6: //TRM header
    case  7: //TRM header
    case  8: //TRM header
    case  9: //TRM header
    case 10: //TRM header
    case 11: //TRM header
    case 12: //TRM header
      if (fInsideTRM) { // unexpected TRM global headers -> exit
	break;
      }
      fInsideTRM = kTRUE; // TRM global header accepted
      fACQ =  GetField(data,TRM_ACQ_BITS_MASK,TRM_ACQ_BITS_POSITION);
      break;

    default: // unexpected global header slot ID
      break;

    } //end switch global header slot id

    break;


  case GLOBAL_TRAILER_TYPE: // global trailer
    fSlotID = GetField(data,HEADER_SLOT_ID_MASK,HEADER_SLOT_ID_POSITION);

    switch (fSlotID) { // switch global trailer slot ID

    case DRM_ID_NUMBER: // DRM global trailer
      if (!fInsideDRM) { // unexpected DRM global trailers -> exit
	break;
      }
      dummy = 0x0000fff0;
      //AliInfo(Form("  DRM local event counter = %i", GetField(data,dummy,4)));
      fLocalEventCounterDRM = GetField(data,dummy,4);
      fInsideDRM = kFALSE; // DRM global trailer accepted
      fInsideTRM = kFALSE;
      fInsideLTM = kFALSE;
      fInsideTRMchain0 = kFALSE;
      fInsideTRMchain1 = kFALSE;
      fSector = -1;
      fPlate  = -1;
      fStrip  = -1;
      fPadZ   = -1;
      fPadX   = -1;
      fDDL        = -1;
      fTRM        = -1;
      fTDC        = -1;
      fTRMchain   = -1;
      fTDCchannel = -1;
      fTime = -1;
      fToT  = -1;
      fLeadingEdge  = -1;
      fTrailingEdge = -1;
      fErrorFlag = -1;
      fACQ   = -1;
      fPSbit = -1;
      fTDCerrorFlag = -1;
      break;
    case LTM_ID_NUMBER: // LTM global trailer
      if (!fInsideLTM) { // unexpected LTM global trailer -> exit
	break;
      }
      dummy = 0x0fff0000;
      //AliInfo(Form("  LTM local event counter = %i", GetField(data,dummy,16)));
      fLocalEventCounterLTM = GetField(data,dummy,16);
      fInsideLTM = kFALSE; // LTM global trailer accepted
      break;
    case 15: //TRM global trailer
      if (!fInsideTRM) { // unexpected TRM global trailers -> exit
	break;
      }
      dummy = 0x0fff0000;
      //AliInfo(Form("  TRM local event counter = %i", GetField(data,dummy,16)));
      fLocalEventCounterTRM[fTRM] = GetField(data,dummy,16);
      fInsideTRM = kFALSE; // TRM global trailer accepted
      break;
    default: // unexpected global trailer slot ID
      break;
    } //end switch global trailer slot id


    break;


  case ERROR_TYPE: // TDC error
    fTDC          = GetField(data,TRM_TDC_ERROR_TDC_ID_MASK,TRM_TDC_ERROR_TDC_ID_POSITION);
    fTDCerrorFlag = GetField(data,TRM_TDC_ERROR_FLAGS_MASK,TRM_TDC_ERROR_FLAGS_POSITION);
    break;


  case FILLER_TYPE: // filler
    break;


  default: // other word types

    if (fInsideTRM) { // inside TRM

      switch (fWordType) { // switch word type inside TRM
      case TRM_CHAIN0_HEADER_TYPE: // TRM chain0 header
	if (fInsideTRMchain0) { // unexpected TRM chain0 header
	  break;
	}
	fInsideTRMchain0 = kTRUE;
	fTRMchain = 0;
        dummy = 0x0000fff0;
        //AliInfo(Form("  chain bunch ID = %i", GetField(data,dummy,4)));
        fChainBunchID[fTRM][fTRMchain] = GetField(data,dummy,4);
	break;
      case TRM_CHAIN0_TRAILER_TYPE: // TRM chain0 trailer
	if (!fInsideTRMchain0) { // unexpected TRM chain0 trailer
	  break;
	}
        dummy = 0x0fff0000;
        //AliInfo(Form("  chain local event counter = %i", GetField(data,dummy,16)));
        fLocalEventCounterChain[fTRM][fTRMchain] = GetField(data,dummy,16);
	fInsideTRMchain0 = kFALSE;
	fTRMchain = -1;
	break;
      case TRM_CHAIN1_HEADER_TYPE: // TRM chain1 header
	if (fInsideTRMchain1) { // unexpected TRM chain1 header
	  break;
	}
	fInsideTRMchain1 = kTRUE;
	fTRMchain = 1;
        dummy = 0x0000fff0;
        //AliInfo(Form("  chain bunch ID = %i", GetField(data,dummy,4)));
        fChainBunchID[fTRM][fTRMchain] = GetField(data,dummy,4);
	break;
      case TRM_CHAIN1_TRAILER_TYPE: // TRM chain1 trailer
	if (!fInsideTRMchain1) { // unexpected TRM chain1 trailer
	  break;
	}
        dummy = 0x0fff0000;
        //AliInfo(Form("  chain local event counter = %i", GetField(data,dummy,16)));
        fLocalEventCounterChain[fTRM][fTRMchain] = GetField(data,dummy,16);
	fInsideTRMchain1 = kFALSE;
	fTRMchain = -1;
	break;
      } // end switch word type inside TRM

    } // end if (fInsideTRM)

      
    if (
	((fInsideTRMchain0&&!fInsideTRMchain1) || (!fInsideTRMchain0&&fInsideTRMchain1)) 
	&& fWordType!=TRM_CHAIN0_HEADER_TYPE && fWordType!=TRM_CHAIN0_TRAILER_TYPE
	&& fWordType!=TRM_CHAIN1_HEADER_TYPE && fWordType!=TRM_CHAIN1_TRAILER_TYPE
	){ // inside TRM chains

      fPSbit      = GetField(data,TRM_PS_BITS_MASK,TRM_PS_BITS_POSITION);
      fTDC        = GetField(data,TRM_TDC_ID_MASK,TRM_TDC_ID_POSITION);
      fTDCchannel = GetField(data,TRM_CHAN_MASK,TRM_CHAN_POSITION);
      fErrorFlag  = GetField(data,TRM_E_BIT_MASK,TRM_E_BIT_POSITION);

      SetSector();
      SetPlate();
      SetStrip();
      SetPadZ();
      SetPadX();


      switch (fPSbit) { // switch fPSbit bits inside TRM chains

      case 0: // packing ok, digit time and TOT
	fToT  = GetField(data,TRM_TOT_WIDTH_MASK, TRM_TOT_WIDTH_POSITION);
	fTime = GetField(data,TRM_DIGIT_TIME_MASK,TRM_DIGIT_TIME_POSITION)
	  /*-
	  fCableLengthMap->GetCableTimeShiftBin(fDDL, fTRM, fTRMchain, fTDC)*/
	  ;
	if (fgApplyBCCorrections) {
	  AliDebug(2,"Apply nominal DDL BC time-shift correction");
	  AliDebug(2,"Apply deltaBC time-shift correction");
	  AliDebug(2,Form(" fChainBunchID[%d][%d] = %d ,fEventID = %d",fTRM,fTRMchain,fChainBunchID[fTRM][fTRMchain],fEventID));
	  fTime +=
	    (Int_t)(fgkddlBCshift[fDDL]*25./AliTOFGeometry::TdcBinWidth()*1.E+03)
	    +
	    (Int_t)((fChainBunchID[fTRM][fTRMchain]-fEventID)*25./AliTOFGeometry::TdcBinWidth()*1.E+03);
	}
	break;

      case 1: // leading edge digit, long digit time, no TOT
	//fToT  = -1;
	//fTime  = -1;
	fLeadingEdge = GetField(data,TRM_LONG_DIGIT_TIME_MASK,TRM_LONG_DIGIT_TIME_POSITION)
	  /*-
	  fCableLengthMap->GetCableTimeShiftBin(fDDL, fTRM, fTRMchain, fTDC)*/
	  ;
	if (fgApplyBCCorrections) {
	  AliDebug(2,"Apply nominal DDL BC time-shift correction");
	  AliDebug(2,"Apply deltaBC time-shift correction");
	  AliDebug(2,Form(" fChainBunchID[%d][%d] = %d ,fEventID = %d",fTRM,fTRMchain,fChainBunchID[fTRM][fTRMchain],fEventID));
	  fLeadingEdge +=
	    (Int_t)(fgkddlBCshift[fDDL]*25./AliTOFGeometry::TdcBinWidth()*1.E+03)
	    +
	    (Int_t)((fChainBunchID[fTRM][fTRMchain]-fEventID)*25./AliTOFGeometry::TdcBinWidth()*1.E+03);
	}
	break;

      case 2: // trailing edge digit, long digit time, no TOT
	//fToT  = -1;
	//fTime  = -1;
	fTrailingEdge = GetField(data,TRM_LONG_DIGIT_TIME_MASK,TRM_LONG_DIGIT_TIME_POSITION)
	  /*-
	  fCableLengthMap->GetCableTimeShiftBin(fDDL, fTRM, fTRMchain, fTDC)*/
	  ;
	if (fgApplyBCCorrections) {
	  AliDebug(2,"Apply nominal DDL BC time-shift correction");
	  AliDebug(2,"Apply deltaBC time-shift correction");
	  AliDebug(2,Form(" fChainBunchID[%d][%d] = %d ,fEventID = %d",fTRM,fTRMchain,fChainBunchID[fTRM][fTRMchain],fEventID));
	  fTrailingEdge +=
	    (Int_t)(fgkddlBCshift[fDDL]*25./AliTOFGeometry::TdcBinWidth()*1.E+03)
	    +
	    (Int_t)((fChainBunchID[fTRM][fTRMchain]-fEventID)*25./AliTOFGeometry::TdcBinWidth()*1.E+03);
	}
	break;

      case 3: // TOT overflow
	fToT  = GetField(data,TRM_TOT_WIDTH_MASK, TRM_TOT_WIDTH_POSITION);
	fTime = GetField(data,TRM_DIGIT_TIME_MASK,TRM_DIGIT_TIME_POSITION)
	  /*-
	  fCableLengthMap->GetCableTimeShiftBin(fDDL, fTRM, fTRMchain, fTDC)*/
	  ;
	if (fgApplyBCCorrections) {
	  AliDebug(2,"Apply nominal DDL BC time-shift correction");
	  AliDebug(2,"Apply deltaBC time-shift correction");
	  AliDebug(2,Form(" fChainBunchID[%d][%d] = %d ,fEventID = %d",fTRM,fTRMchain,fChainBunchID[fTRM][fTRMchain],fEventID));
	  fTime +=
	    (Int_t)(fgkddlBCshift[fDDL]*25./AliTOFGeometry::TdcBinWidth()*1.E+03)
	    +
	    (Int_t)((fChainBunchID[fTRM][fTRMchain]-fEventID)*25./AliTOFGeometry::TdcBinWidth()*1.E+03);
	}
	break;

      } // end switch PS bits inside TRM chains

    } // end if is inside TRM chains

  } // end switch on fWordType


  return kTRUE;
  
}
//_____________________________________________________________________________

void AliTOFRawStream::SetSector()
{
  //
  // Evaluate the TOF sector number -> [ 0;17]
  // corresponding to the TOF equipment IDs:
  //                                  fDDL        -> [ 0;71]
  //                                  fTRM        -> [ 3;12]
  //                                  fTRMchain   -> [ 0; 1]
  //                                  fTDC        -> [ 0;14]
  //                                  fTDCchannel -> [ 0; 7]
  //

  Int_t iSector = -1;

  if (!(fDDL==-1)) iSector = Int_t((Float_t)(fDDL)/AliTOFGeometry::NDDL());

  fSector = iSector;

}
//_____________________________________________________________________________


void AliTOFRawStream::SetPlate()
{
  //
  // Evaluate the TOF plate number ->[ 0; 4]
  // corresponding to the TOF equipment IDs:
  //                                  fDDL        -> [ 0;71]
  //                                  fTRM        -> [ 3;12]
  //                                  fTRMchain   -> [ 0; 1]
  //                                  fTDC        -> [ 0;14]
  //                                  fTDCchannel -> [ 0; 7]
  //

  Int_t iPlate = -1;
  if (!(fDDL==-1 || fTRM==-1 || fTDC==-1
	|| fSector==-1))
    iPlate = Equip2VolNplate(GetDDLnumberPerSector(fDDL), fTRM, fTDC);

  fPlate = iPlate;

}
//_____________________________________________________________________________

void AliTOFRawStream::SetStrip()
{
  //
  // Evaluate the TOF strip number per module -> [ 0; 14/18]
  // corresponding to the TOF equipment IDs:
  //                                  fDDL        -> [ 0;71]
  //                                  fTRM        -> [ 3;12]
  //                                  fTRMchain   -> [ 0; 1]
  //                                  fTDC        -> [ 0;14]
  //                                  fTDCchannel -> [ 0; 7]
  //

  Int_t iStrip = -1;

  if (!(fDDL==-1 || fTRM==-1 || fTDC==-1
	|| fSector==-1 || fPlate==-1))
    iStrip = Equip2VolNstrip(GetDDLnumberPerSector(fDDL), fTRM, fTDC);

  fStrip = iStrip;

}
//_____________________________________________________________________________

void AliTOFRawStream::SetPadZ()
{
  //
  // Evaluate the TOF padRow number per strip -> [ 0; 1]
  // corresponding to the TOF equipment IDs:
  //                                  fDDL        -> [ 0;71]
  //                                  fTRM        -> [ 3;12]
  //                                  fTRMchain   -> [ 0; 1]
  //                                  fTDC        -> [ 0;14]
  //                                  fTDCchannel -> [ 0; 7]
  //

  Int_t iPadZ = -1;

  if (!(fDDL==-1 || fTRM==-1 || fTRMchain==-1 || fTDC==-1 || fTDCchannel==-1
	|| fSector==-1 || fPlate==-1 || fStrip==-1))
    {
      Int_t iPadAlongTheStrip = Equip2VolNpad(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel);
      if (iPadAlongTheStrip!=-1)
	iPadZ  = iPadAlongTheStrip%AliTOFGeometry::NpadZ();
    }

  //iPadZ = Equip2VolNpad(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel)%AliTOFGeometry::NpadZ();
  //iPadZ = Equip2VolNpadZ(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel);

  fPadZ = iPadZ;

}
//_____________________________________________________________________________

void AliTOFRawStream::SetPadX()
{
  //
  // Evaluate the TOF pad number per strip padRow -> [ 0;47]
  // corresponding to the TOF equipment IDs:
  //                                  fDDL        -> [ 0;71]
  //                                  fTRM        -> [ 3;12]
  //                                  fTRMchain   -> [ 0; 1]
  //                                  fTDC        -> [ 0;14]
  //                                  fTDCchannel -> [ 0; 7]
  //

  Int_t iPadX = -1;

  if (!(fDDL==-1 || fTRM==-1 || fTRMchain==-1 || fTDC==-1 || fTDCchannel==-1
	|| fSector==-1 || fPlate==-1 || fStrip==-1))
    {
      Int_t iPadAlongTheStrip = Equip2VolNpad(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel);
      if (iPadAlongTheStrip!=-1)
	iPadX  = (Int_t)(iPadAlongTheStrip/(Float_t(AliTOFGeometry::NpadZ())));
    }

  //iPadX = (Int_t)(Equip2VolNpad(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel)/(Float_t(AliTOFGeometry::NpadZ())));
  //iPadX = Equip2VolNpadX(GetDDLnumberPerSector(fDDL), fTRMchain, fTDC, fTDCchannel);

  fPadX = iPadX;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::GetField(UInt_t word, Int_t fieldMask, Int_t fieldPosition) const
{
  // 
  // 
  // 

  return ((word & fieldMask) >> fieldPosition);
}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::Equip2VolNplate(Int_t iDDL, Int_t nTRM, Int_t nTDC)
{
  //
  // Returns the TOF plate number [0;4]
  // corresponding to the TOF equipment ID numbers:
  //                          iDDL -> DDL number per sector [0;3]
  //                          nTRM -> TRM number [3;12]
  //                          nTDC -> TDC number [0;14]
  //

  Int_t iPlate = -1;
  if (iDDL==0) {

    if (nTRM>=4 && nTRM<7) {
      iPlate = 0;
    } else if (nTRM==7) {
      if (nTDC<12) iPlate = 0;
      else iPlate = 1;
    } else if (nTRM>=8 && nTRM<11) {
      iPlate = 1;
    } else if (nTRM==11) {
      if (nTDC<9) iPlate = 1;
      else iPlate = 2;
    }else if (nTRM==12) {
      iPlate = 2;
    } 

  } else if (iDDL==1) {

    if (nTRM==3) {
      if (nTDC<3) iPlate = 0;
    } else if (nTRM>=4 && nTRM<7) {
      iPlate = 0;
    } else if (nTRM==7) {
      if (nTDC<6) iPlate = 1;
      else iPlate = 0;
    } else if (nTRM>=8 && nTRM<11) {
      iPlate = 1;
    } else if (nTRM==11) {
      if (nTDC<9) iPlate = 2;
      else iPlate = 1;
    } else if (nTRM==12) {
      iPlate = 2;
    } 

  } else if (iDDL==2) {

    if (nTRM>=4 && nTRM<7) {
      iPlate = 4;
    } else if (nTRM==7) {
      if (nTDC<12) iPlate = 4;
      else iPlate = 3;
    } else if (nTRM>=8 && nTRM<11) {
      iPlate = 3;
    } else if (nTRM==11) {
      if (nTDC<9) iPlate = 3;
      else iPlate = 2;
    }else if (nTRM==12) {
      iPlate = 2;
    } 

  }  else if (iDDL==3) {

    if (nTRM==3) {
      if (nTDC<3) iPlate = 4;
    } else if (nTRM>=4 && nTRM<7) {
      iPlate = 4;
    } else if (nTRM==7) {
      if (nTDC<6) iPlate = 3;
      else iPlate = 4;
    } else if (nTRM>=8 && nTRM<11) {
      iPlate = 3;
    } else if (nTRM==11) {
      if (nTDC<9) iPlate = 2;
      else iPlate = 3;
    } else if (nTRM==12) {
      iPlate = 2;
    } 

  }

  return iPlate;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::Equip2VolNstrip(Int_t iDDL, Int_t nTRM, Int_t nTDC)
{
  //
  // Returns the TOF strip number per module:
  //                                [0;14], in the central plates,
  //                                [0;18], in the intermediate and external plates
  // corresponding to the TOF equipment ID numbers:
  //                                iDDL -> DDL number per sector [0;3]
  //                                nTRM -> TRM number [3;12]
  //                                nTDC -> TDC number [0;14]
  //

  Int_t iStrip = -1;

  if (iDDL==0) {

    if (nTRM== 4) iStrip =  (Int_t)(nTDC/3.);
    else if (nTRM== 5) iStrip =  5 + (Int_t)(nTDC/3.);
    else if (nTRM== 6) iStrip = 10 + (Int_t)(nTDC/3.);
    else if (nTRM== 7) {
      if (nTDC<12) iStrip =  15 + (Int_t)(nTDC/3.);
      else iStrip = (Int_t)(nTDC/3.) -  4;
    }
    else if (nTRM== 8) iStrip =  1 + (Int_t)(nTDC/3.);
    else if (nTRM== 9) iStrip =  6 + (Int_t)(nTDC/3.);
    else if (nTRM==10) iStrip = 11 + (Int_t)(nTDC/3.);
    else if (nTRM==11) {
      if (nTDC<9) iStrip = 16 + (Int_t)(nTDC/3.);
      else iStrip = (Int_t)(nTDC/3.) -  3;
    }
    else if (nTRM==12) iStrip =  2 + (Int_t)(nTDC/3.);

  } else if (iDDL==1) {

    if (nTRM==3 && nTDC<3) iStrip = (Int_t)(nTDC/3.);
    else if (nTRM== 4) iStrip =  5 - (Int_t)(nTDC/3.);
    else if (nTRM== 5) iStrip = 10 - (Int_t)(nTDC/3.);
    else if (nTRM== 6) iStrip = 15 - (Int_t)(nTDC/3.);
    else if (nTRM== 7) {
      if (nTDC<6) iStrip =  1 - (Int_t)(nTDC/3.);
      else iStrip = 20 - (Int_t)(nTDC/3.);
    }
    else if (nTRM== 8) iStrip =  6 - (Int_t)(nTDC/3.);
    else if (nTRM== 9) iStrip = 11 - (Int_t)(nTDC/3.);
    else if (nTRM==10) iStrip = 16 - (Int_t)(nTDC/3.);
    else if (nTRM==11) {
      if (nTDC<9) iStrip =  2 - (Int_t)(nTDC/3.);
      else iStrip = 21 - (Int_t)(nTDC/3.);
    }
    else if (nTRM==12) iStrip =  7 - (Int_t)(nTDC/3.);

  } else if (iDDL==2) {

    if (nTRM== 4) iStrip =  18 - (Int_t)(nTDC/3.);
    else if (nTRM== 5) iStrip = 18 - ( 5 + (Int_t)(nTDC/3.));
    else if (nTRM== 6) iStrip = 18 - (10 + (Int_t)(nTDC/3.));
    else if (nTRM== 7) {
      if (nTDC<12) iStrip =  18 - (15 + (Int_t)(nTDC/3.));
      else iStrip = 18 - ((Int_t)(nTDC/3.) -  4);
    }
    else if (nTRM== 8) iStrip = 18 - ( 1 + (Int_t)(nTDC/3.));
    else if (nTRM== 9) iStrip = 18 - ( 6 + (Int_t)(nTDC/3.));
    else if (nTRM==10) iStrip = 18 - (11 + (Int_t)(nTDC/3.));
    else if (nTRM==11) {
      if (nTDC<9) iStrip = 18 - (16 + (Int_t)(nTDC/3.));
      else iStrip = 14 - ((Int_t)(nTDC/3.) -  3);
    }
    else if (nTRM==12) iStrip = 14 - ( 2 + (Int_t)(nTDC/3.));

  } else if (iDDL==3) {

    if (nTRM==3 && nTDC<3) iStrip = 18 - (Int_t)(nTDC/3.);
    else if (nTRM== 4) iStrip = 18 - ( 5 - (Int_t)(nTDC/3.));
    else if (nTRM== 5) iStrip = 18 - (10 - (Int_t)(nTDC/3.));
    else if (nTRM== 6) iStrip = 18 - (15 - (Int_t)(nTDC/3.));
    else if (nTRM== 7) {
      if (nTDC<6) iStrip =  18 - (1 - (Int_t)(nTDC/3.));
      else iStrip = 18 - (20 - (Int_t)(nTDC/3.));
    }
    else if (nTRM== 8) iStrip = 18 - ( 6 - (Int_t)(nTDC/3.));
    else if (nTRM== 9) iStrip = 18 - (11 - (Int_t)(nTDC/3.));
    else if (nTRM==10) iStrip = 18 - (16 - (Int_t)(nTDC/3.));
    else if (nTRM==11) {
      if (nTDC<9) iStrip = 14 - ( 2 - (Int_t)(nTDC/3.));
      else iStrip = 18 - (21 - (Int_t)(nTDC/3.));
    }
    else if (nTRM==12) iStrip = 14 - ( 7 - (Int_t)(nTDC/3.));

  } 

  return iStrip;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::Equip2VolNpad(Int_t iDDL, Int_t iChain, Int_t nTDC,
				     Int_t iCH)
{
  //
  // Returns the TOF pad number per strip [0;95]
  // corresponding to the TOF equipment ID numbers:
  //                          iDDL -> DDL number per sector [0;3]
  //                        iChain -> TRM chain number [0;1]
  //                          nTDC -> TDC number [0;14]
  //                           iCH -> TDC channel number [0;7]
  //

  Int_t iPadAlongTheStrip = -1;

  // wrong
  //Int_t iTDClocal = nTDC%3 + (1-iChain)*3;
  //if (iDDL==0 || iDDL==3) iTDClocal = 5 - iTDClocal;
  //else if (iDDL==1 || iDDL==2) iTDClocal = 6 + (5 - iTDClocal);

  // right
  Int_t iTDClocal = -1;
  Int_t iTDClocal03 = nTDC%3 + (1-iChain)*3;
  Int_t iTDClocal12 = 2-nTDC%3 + iChain*3;
  if (iDDL==0 || iDDL==3) iTDClocal = 5 - iTDClocal03;
  else if (iDDL==1 || iDDL==2) iTDClocal = 6 + (5 - iTDClocal12);

  Int_t iCHlocal = iCH;
  if (iDDL==0 || iDDL==3) iCHlocal = 7 - iCH;

  iPadAlongTheStrip = iTDClocal*AliTOFGeometry::NCh() + iCHlocal;

  if (((iDDL==1 || iDDL==2) && iPadAlongTheStrip< AliTOFGeometry::NpadX()) ||
      ((iDDL==0 || iDDL==3) && iPadAlongTheStrip>=AliTOFGeometry::NpadX())) {
    std::cerr << "Problems with the padX number!" << endl;
    //AliWarning("Problems with the padX number!");
  }
  return iPadAlongTheStrip;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::Equip2VolNpadX(Int_t iDDL, Int_t iChain, Int_t nTDC,
				      Int_t iCH)
{
  //
  // Returns the TOF padX number [0;47]
  // corresponding to the TOF equipment ID numbers:
  //                          iDDL -> DDL number per sector [0;3]
  //                        iChain -> TRM chain number [0;1]
  //                          nTDC -> TDC number [0;14]
  //                           iCH -> TDC channel number [0;7]
  //

  Int_t iPadX = (Int_t)(AliTOFRawStream::Equip2VolNpad(iDDL, iChain, nTDC, iCH)/
			(Float_t(AliTOFGeometry::NpadZ())));

  return iPadX;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::Equip2VolNpadZ(Int_t iDDL, Int_t iChain, Int_t nTDC,
				      Int_t iCH)
{
  //
  // Returns the TOF padZ number [0;1]
  // corresponding to the TOF equipment ID numbers:
  //                          iDDL -> DDL number per sector [0;3]
  //                        iChain -> TRM chain number [0;1]
  //                          nTDC -> TDC number [0;14]
  //                           iCH -> TDC channel number [0;7]
  //

  Int_t iPadZ  = AliTOFRawStream::Equip2VolNpad(iDDL, iChain, nTDC, iCH)%AliTOFGeometry::NpadZ();

  return iPadZ;

}

//----------------------------------------------------------------------------
Int_t AliTOFRawStream::GetSectorNumber(Int_t nDDL) const
{
  //
  // Returns the sector number [0;17]
  // corresponing to the assigned DRM/DDL number [0;71]
  //

  Int_t iSector = Int_t((Float_t)(nDDL)/AliTOFGeometry::NDDL());

  return iSector;

}
//----------------------------------------------------------------------------
Int_t AliTOFRawStream::GetDDLnumberPerSector(Int_t nDDL) const
{
  //
  // Return the DRM/DDL number per sector [0;3]
  // corresponing to the assigned DRM/DDL number [0;71]
  //

  Int_t iDDL = nDDL%AliTOFGeometry::NDDL();

  return iDDL;

}

//----------------------------------------------------------------------------
void AliTOFRawStream::EquipmentId2VolumeId(AliTOFHitData *hitData, Int_t *volume) const
{
  EquipmentId2VolumeId(hitData->GetDDLID(),hitData->GetSlotID(),hitData->GetChain(),hitData->GetTDC(),hitData->GetChan(),volume);
}
//----------------------------------------------------------------------------
void AliTOFRawStream::EquipmentId2VolumeId(Int_t nDDL, Int_t nTRM, Int_t iChain,
					Int_t nTDC, Int_t iCH,
					Int_t *volume) const
{
  //
  // To convert:
  //            nDDL   (variable in [0;71]) -> number of the DDL file 
  //            nTRM   (variable in [3;12]) -> number of the TRM slot
  //            iChain (variable in [0; 1]) -> number of the TRM chain
  //            nTDC   (variable in [0;14]) -> number of the TDC
  //            iCH    (variable in [0; 7]) -> number of the TDC channel
  //
  // in:
  //      sector number, i.e. volume[0] (variable in [0,17])
  //      plate  number, i.e. volume[1] (variable in [0, 5])
  //      strip  number, i.e. volume[2] (variable in [0,14/18])
  //      padX   number, i.e. volume[3] (variable in [0,47])
  //      padZ   number, i.e. volume[4] (variable in [0, 1])
  //

  Int_t iDDL = GetDDLnumberPerSector(nDDL);

  Int_t iSector = GetSectorNumber(nDDL);

  Int_t iPlate = Equip2VolNplate(iDDL, nTRM, nTDC);
  if (iPlate==-1) {
    if (fRawReader)
      fRawReader->AddMajorErrorLog(kPlateError,"plate = -1");
    AliWarning("Problems with the plate number!");
  }

  Int_t iStrip = Equip2VolNstrip(iDDL, nTRM, nTDC);
  if (iStrip==-1) {
    if (fRawReader)
      fRawReader->AddMajorErrorLog(kStripError,"strip = -1");
    AliWarning("Problems with the strip number!");
  }

  Int_t iPadAlongTheStrip  = Equip2VolNpad(iDDL, iChain, nTDC, iCH);
  if (iPadAlongTheStrip==-1){
    if (fRawReader)
      fRawReader->AddMajorErrorLog(kPadAlongStripError,"pad = -1");
    AliWarning("Problems with the pad number along the strip!");
  }
  
  Int_t iPadX  = (Int_t)(iPadAlongTheStrip/(Float_t(AliTOFGeometry::NpadZ())));
  Int_t iPadZ  = iPadAlongTheStrip%AliTOFGeometry::NpadZ();

  //Int_t iPadX  = (Int_t)(Equip2VolNpad(iDDL, iChain, nTDC, iCH)/(Float_t(AliTOFGeometry::NpadZ())));
  //Int_t iPadZ  = Equip2VolNpad(iDDL, iChain, nTDC, iCH)%AliTOFGeometry::NpadZ();

  //Int_t iPadX  = Equip2VolNpadX(iDDL, iChain, nTDC, iCH);
  //Int_t iPadZ  = Equip2VolNpadZ(iDDL, iChain, nTDC, iCH);

  volume[0] = iSector;
  volume[1] = iPlate;
  volume[2] = iStrip;
  volume[3] = iPadX;
  volume[4] = iPadZ;

}
//-----------------------------------------------------------------------------
Bool_t AliTOFRawStream::DecodeDDL(Int_t nDDLMin, Int_t nDDLMax, Int_t verbose = 0) {
  //
  // To decode raw data for DDL number in [nDDLmin; nDDLmax]
  //

  //check and fix valid DDL range
  if (nDDLMin < 0){
    nDDLMin = 0;
    fRawReader->AddMinorErrorLog(kDDLMinError);
    AliWarning("Wrong DDL range: setting first DDL ID to 0");
  }
  if (nDDLMax > 71){
    nDDLMax = 71;
    fRawReader->AddMinorErrorLog(kDDLMaxError);
    AliWarning("Wrong DDL range: setting last DDL ID to 71");
  }  

  //select required DDLs
  fRawReader->Select("TOF", nDDLMin, nDDLMax);

  if (verbose)
    AliInfo(Form("Selected TOF DDL range: %d-%d", nDDLMin, nDDLMax));

  return(Decode(verbose));
}
//-----------------------------------------------------------------------------
Bool_t AliTOFRawStream::Decode(Int_t verbose = 0) {
  //
  // New decoder method
  //

  Int_t currentEquipment;
  Int_t currentDDL;
  const AliRawDataHeader *currentCDH;

  //pointers
  UChar_t *data = 0x0;
  
  //loop and read DDL headers 
  while(fRawReader->ReadHeader()){

    //memory leak prevention (actually data should be always 0x0 here)
    if (data != 0x0)
      delete [] data;

    //get equipment infos
    currentEquipment = fRawReader->GetEquipmentId();
    currentDDL = fRawReader->GetDDLID();
    currentCDH = fRawReader->GetDataHeader();
    const Int_t kDataSize = fRawReader->GetDataSize();
    const Int_t kDataWords = kDataSize / 4;
    data = new UChar_t[kDataSize];
    
    if (verbose)
      AliInfo(Form("Found equipment # %d header (DDL # %d): %d bytes (%d words)", currentEquipment, currentDDL, kDataSize, kDataWords));
    
    if (verbose)
      AliInfo(Form("Reading equipment #%d (DDL # %d) data...", currentEquipment, currentDDL));
    
    //read equipment payload
    if (!fRawReader->ReadNext(data, kDataSize))
      {
	fRawReader->AddMajorErrorLog(kDDLdataReading);
	if (verbose)
	  AliWarning("Error while reading DDL data. Go to next equipment");
	delete [] data;
	data = 0x0;
	continue;
      }
    
    if (verbose)
      AliInfo(Form("Equipment # %d (DDL # %d) data has been readed", currentEquipment, currentDDL));
    
    
    //set up the decoder
    fDecoder->SetVerbose(verbose);
    fDecoder->SetDataBuffer(fDataBuffer[currentDDL]);
    fDecoder->SetPackedDataBuffer(fPackedDataBuffer[currentDDL]);
    
    //start decoding
    if (fDecoder->Decode((UInt_t *)data, kDataWords, currentCDH) == kTRUE) {
      fRawReader->AddMajorErrorLog(kDDLDecoder,Form("DDL # = %d",currentDDL));
      AliWarning(Form("Error while decoding DDL # %d: decoder returned with errors", currentDDL));
    }
    
    delete [] data;
    data = 0x0;
  }
  
  //reset reader
  fRawReader->Reset();

  if (verbose)
    AliInfo("All done");
    
  return kFALSE;
  
}
//---------------------------------------------------------------------------
void
AliTOFRawStream::ResetBuffers()
{
  //
  // To reset the buffers
  //

  for (Int_t iDDL = 0; iDDL < AliDAQ::NumberOfDdls("TOF"); iDDL++){
    ResetDataBuffer(iDDL);
    ResetPackedDataBuffer(iDDL);
  }
}
  
//---------------------------------------------------------------------------
Bool_t
AliTOFRawStream::LoadRawDataBuffers(Int_t indexDDL, Int_t verbose)
{
  //
  // To load the buffers
  //

  fTOFrawData->Clear();
  fPackedDigits = 0;
  
  if (verbose > 0)
    AliInfo(Form("Decoding raw data for DDL # %d ...", indexDDL));

  if (DecodeDDL(indexDDL, indexDDL, verbose) != 0){ //decode DDL
    fRawReader->AddMajorErrorLog(kDDLDecoder,Form("DDL # = %d",indexDDL));
    AliWarning(Form("Error while decoding DDL # %d", indexDDL));
    return kTRUE;
  }
  
  if (verbose > 0)
    AliInfo(Form("Done. %d packed %s been found.", fPackedDataBuffer[indexDDL]->GetEntries(), fPackedDataBuffer[indexDDL]->GetEntries() > 1 ? "hits have" : "hit has"));
  
  AliTOFHitData *hitData; //hit data pointer
  
  if (verbose > 0)
    AliInfo("Filling TClonesArray ...");

  if (verbose > 0)
    if (fgApplyBCCorrections) {
      AliInfo("Apply nominal DDL BC time-shift correction");
      AliInfo("Apply deltaBC time-shift correction");
    }

  //loop over DDL packed hits
  for (Int_t iHit = 0; iHit < fPackedDataBuffer[indexDDL]->GetEntries(); iHit++){
    hitData = fPackedDataBuffer[indexDDL]->GetHit(iHit); //get hit data
    Int_t   hitACQ = hitData->GetACQ();
    Int_t   hitPS = hitData->GetPS();
    Int_t   hitSlotID = hitData->GetSlotID();
    Int_t   hitChain = hitData->GetChain();
    Int_t   hitTDC = hitData->GetTDC();
    Int_t   hitChan = hitData->GetChan();
    Int_t   hitTimeBin = hitData->GetTimeBin();
    Int_t   hitTOTBin = hitData->GetTOTBin();

    if (fgApplyBCCorrections) {
      /* DDL BC shift time correction */
      hitTimeBin += fgkddlBCshift[indexDDL];
      /* deltaBC shift time correction */
      hitTimeBin += hitData->GetDeltaBunchID();
    }

    Int_t hitLeading = hitData->GetTimeBin();
    Int_t hitTrailing = -1;
    Int_t hitError = -1;
    
    TClonesArray &arrayTofRawData =  *fTOFrawData;
    new (arrayTofRawData[fPackedDigits++]) AliTOFrawData(hitSlotID, hitChain, hitTDC, hitChan, hitTimeBin, hitTOTBin, hitLeading, hitTrailing, hitPS, hitACQ, hitError);
  }

  if (verbose > 0)
    AliInfo("Done.");

  if (verbose > 0)
    AliInfo("Resetting buffers ...");

  fDataBuffer[indexDDL]->Reset();
  fPackedDataBuffer[indexDDL]->Reset();

  if (verbose > 0)
    AliInfo("Done.");

  return kFALSE;
}


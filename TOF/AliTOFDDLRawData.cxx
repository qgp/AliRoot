/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
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
Revision 1.14  2007/03/28 10:50:33  decaro
Rounding off problem in rawData coding/decoding: solved

Revision 1.13  2007/02/20 15:57:00  decaro
Raw data update: to read the TOF raw data defined in UNPACKED mode

Revision 1.12  2006/08/22 13:29:42  arcelli
removal of effective c++ warnings (C.Zampolli)

Revision 1.11  2006/08/10 14:46:54  decaro
TOF raw data format: updated version

Revision 1.10.1  2006/06/28 A.De Caro
        Update TOF raw data format
               according to the final version
               (see ALICE internal note in preparation
                'ALICE TOF raw data format')

Revision 0.02  2005/7/25 A.De Caro
        Update number of bits allocated for time-of-flight
               and 'charge' measurements

Revision 0.01  2004/6/11 A.De Caro, S.B.Sellitto, R.Silvestri
        First implementation: global methods RawDataTOF
                                             GetDigits
*/

////////////////////////////////////////////////////////////////////
//                                                                //
// This class contains the methods to create the Raw Data files   //
// for the TOF detector starting from the Digits.                 //
// In this implementation, we defined the structure               //
// of the ALICE-TOF raw data (according to the                    //
// ALICE technical note, in preparation)                          //
// starting from the TOF digit format.                            //
//                                                                //
////////////////////////////////////////////////////////////////////

#include "Riostream.h"

#include "TBranch.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "AliBitPacking.h"
#include "AliDAQ.h"
#include "AliLog.h"
//#include "AliRawDataHeader.h"
#include "AliRawDataHeaderSim.h"

#include "AliTOFDDLRawData.h"
#include "AliTOFDigitMap.h"
#include "AliTOFdigit.h"
#include "AliTOFGeometry.h"
#include "AliTOFRawStream.h"

extern TRandom *gRandom;

ClassImp(AliTOFDDLRawData)

//---------------------------------------------------------------------------
AliTOFDDLRawData::AliTOFDDLRawData():
  fVerbose(0),
  fIndex(-1),
  fPackedAcquisition(kTRUE),
  fFakeOrphaneProduction(kFALSE),
  fMatchingWindow(8192),
  fTOFgeometry(0),
  fTOFdigitMap(new AliTOFDigitMap()),
  fTOFdigitArray(0x0),
  fTOFrawStream(new AliTOFRawStream())
{
  //Default constructor
}

//----------------------------------------------------------------------------
AliTOFDDLRawData::AliTOFDDLRawData(AliTOFGeometry *tofGeom):
  fVerbose(0),
  fIndex(-1),
  fPackedAcquisition(kTRUE),
  fFakeOrphaneProduction(kFALSE),
  fMatchingWindow(8192),
  fTOFgeometry(tofGeom),
  fTOFdigitMap(new AliTOFDigitMap()),
  fTOFdigitArray(0x0),
  fTOFrawStream(new AliTOFRawStream())
{
  //Constructor

}

//----------------------------------------------------------------------------
AliTOFDDLRawData::AliTOFDDLRawData(const AliTOFDDLRawData &source) :
  TObject(source),
  fVerbose(0),
  fIndex(-1),
  fPackedAcquisition(kTRUE),
  fFakeOrphaneProduction(kFALSE),
  fMatchingWindow(8192),
  fTOFgeometry(0),
  fTOFdigitMap(new AliTOFDigitMap()),
  fTOFdigitArray(0x0),
  fTOFrawStream(new AliTOFRawStream())
 {
  //Copy Constructor
  this->fIndex=source.fIndex;
  this->fVerbose=source.fVerbose;
  this->fPackedAcquisition=source.fPackedAcquisition;
  this->fFakeOrphaneProduction=source.fFakeOrphaneProduction;
  this->fMatchingWindow=source.fMatchingWindow;
  this->fTOFgeometry=source.fTOFgeometry;
  this->fTOFdigitMap=source.fTOFdigitMap;
  this->fTOFdigitArray=source.fTOFdigitArray;
  this->fTOFrawStream=source.fTOFrawStream;
  return;
}

//----------------------------------------------------------------------------
AliTOFDDLRawData& AliTOFDDLRawData::operator=(const AliTOFDDLRawData &source) {
  //Assigment operator
  this->fIndex=source.fIndex;
  this->fVerbose=source.fVerbose;
  this->fPackedAcquisition=source.fPackedAcquisition;
  this->fFakeOrphaneProduction=source.fFakeOrphaneProduction;
  this->fMatchingWindow=source.fMatchingWindow;
  this->fTOFgeometry=source.fTOFgeometry;
  this->fTOFdigitMap=source.fTOFdigitMap;
  this->fTOFdigitArray=source.fTOFdigitArray;
  this->fTOFrawStream=source.fTOFrawStream;
  return *this;
}

//----------------------------------------------------------------------------
Int_t AliTOFDDLRawData::RawDataTOF(TBranch* branch)
{
  //
  // This method creates the Raw data files for TOF detector
  //

  const Int_t kSize = 5000; //max number of digits per DDL file times 2

  UInt_t buf[kSize];

  fIndex = -1;

  fTOFdigitArray = * (TClonesArray**) branch->GetAddress();

  char fileName[15];
  ofstream outfile;         // logical name of the output file 

  //AliRawDataHeader header;
  AliRawDataHeaderSim header;

  UInt_t sizeRawData = 0;

  branch->GetEvent();
  
  GetDigits();

  Int_t jj = -1;
  Int_t iDDL = -1;
  Int_t nDDL = -1;
  Int_t nTRM =  0;
  Int_t iChain = -1;

  UInt_t nWordsPerTRM = 0;

  //loop over TOF DDL files
  for (nDDL=0; nDDL<AliDAQ::NumberOfDdls("TOF"); nDDL++) {

    strcpy(fileName,AliDAQ::DdlFileName("TOF",nDDL)); //The name of the output file
#ifndef __DECCXX
    outfile.open(fileName,ios::binary);
#else
    outfile.open(fileName);
#endif

    iDDL = fTOFrawStream->GetDDLnumberPerSector(nDDL);

    // write Dummy DATA HEADER
    UInt_t dataHeaderPosition = outfile.tellp();
    outfile.write((char*)(&header),sizeof(header));

    // DRM section: trailer
    MakeDRMtrailer(buf);

    // LTM section
    fIndex++;
    buf[fIndex] = MakeFiller();
    MakeLTMtrailer(buf);
    MakeLTMdata(buf);
    MakeLTMheader(buf);

    // loop on TRM number per DRM
    for (nTRM=AliTOFGeometry::NTRM(); nTRM>=3; nTRM--) {

      nWordsPerTRM = 0;

      // the slot number 3 of the even DRM (i.e. right) doesn't contain TDC digit data
      if (iDDL%2==0 && nTRM==3) continue;

      // TRM global trailer
      MakeTRMtrailer(buf);

      // loop on TRM chain number per TRM
      for (iChain=AliTOFGeometry::NChain()-1; iChain>=0; iChain--) {

	// TRM chain trailer
	MakeTRMchainTrailer(iChain, buf);
	nWordsPerTRM++;

	// TRM TDC digits
	MakeTDCdigits(nDDL, nTRM, iChain, buf, nWordsPerTRM);

	// TRM chain header
	MakeTRMchainHeader(nTRM, iChain, buf);
	nWordsPerTRM++;

      } // end loop on iChain

      // TRM global header
      MakeTRMheader(nTRM, buf);

      // TRM filler in case where TRM data number is odd
      if (nWordsPerTRM%2!=0) MakeTRMfiller(buf, nWordsPerTRM);

    } // end loop on nTRM
       
    // DRM section: in
    MakeDRMheader(nDDL, buf);

    ReverseArray(buf, fIndex+1);

    outfile.write((char *)buf,((fIndex+1)*sizeof(UInt_t)));

    for (jj=0; jj<(fIndex+1); jj++) buf[jj]=0;
    fIndex = -1;
    
    //Write REAL DATA HEADER
    UInt_t currentFilePosition = outfile.tellp();
    sizeRawData = currentFilePosition - dataHeaderPosition - sizeof(header);
    header.fSize = currentFilePosition - dataHeaderPosition;
    header.SetAttribute(0);  // valid data
    outfile.seekp(dataHeaderPosition);
    outfile.write((char*)(&header),sizeof(header));
    outfile.seekp(currentFilePosition);

    outfile.close();

  } //end loop on DDL file number

  return 0;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::GetDigits()
{
  //
  // Fill the TOF volumes' map with the TOF digit indices
  //

  Int_t vol[5] = {-1,-1,-1,-1,-1};

  Int_t digit = -1;
  Int_t ndigits = fTOFdigitArray->GetEntries();

  AliTOFdigit *digs;

  // loop on TOF digits
  for (digit=0; digit<ndigits; digit++) {
    digs = (AliTOFdigit*)fTOFdigitArray->UncheckedAt(digit);

    vol[0] = digs->GetSector(); // Sector Number (0-17)
    vol[1] = digs->GetPlate();  // Plate Number (0-4)
    vol[2] = digs->GetStrip();  // Strip Number (0-14/18)
    vol[3] = digs->GetPadx();   // Pad Number in x direction (0-47)
    vol[4] = digs->GetPadz();   // Pad Number in z direction (0-1)

    fTOFdigitMap->AddDigit(vol, digit);

  } // close loop on digit del TOF

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeDRMheader(Int_t nDDL, UInt_t *buf)
{
  //
  // DRM global header
  //

  Int_t iDDL = fTOFrawStream->GetDDLnumberPerSector(nDDL);

  Int_t iSector = fTOFrawStream->GetSectorNumber(nDDL);

  UInt_t baseWord=0;
  UInt_t word;

  // DRM event CRC
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // event CRC
  AliBitPacking::PackWord(word,baseWord, 4,19);
  word = 0; // reserved for future use
  AliBitPacking::PackWord(word,baseWord,20,27);
  word = 4; // 0100 -> DRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

  // DRM status header 3
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // TTC event counter
  AliBitPacking::PackWord(word,baseWord, 4,27);
  word = 4; // 0100 -> DRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

  // DRM status header 2
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);

  if (iDDL%2==1) {
    word = 2047; // enable ID: [00000000000;11111111111] for odd
                 // (i.e. right) crates
    AliBitPacking::PackWord(word,baseWord, 4,14);
  } else {
    word = 2045; // enable ID: [00000000000;11111111101] for even
	         // (i.e. left) crates
    AliBitPacking::PackWord(word,baseWord, 4,14);
  }

  word = 0; //
  AliBitPacking::PackWord(word,baseWord,15,15);
  word = 0; // fault ID
  AliBitPacking::PackWord(word,baseWord,16,27);
  word = 4; // 0100 -> DRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;
  
  // DRM status header 1
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);

  if (iDDL%2==1) {
    word = 2047; // slot ID: [00000000000;11111111111] for odd
	         // (i.e. right) crates
    AliBitPacking::PackWord(word,baseWord, 4,14);
  } else {
    word = 2045; // slot ID: [00000000000;11111111101] for even
		 // (i.e. left) crates
    AliBitPacking::PackWord(word,baseWord, 4,14);
  }
      
  word = 1; // LHC clock status: 1/0
  AliBitPacking::PackWord(word,baseWord,15,15);
  word = 0; // reserved for future use
  AliBitPacking::PackWord(word,baseWord,16,27);
  word = 4; // 0100 -> DRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

  // DRM global header
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = fIndex+1 + 1; // event words
  AliBitPacking::PackWord(word,baseWord, 4,20);
  word = iDDL; // crate ID [0;3]
  AliBitPacking::PackWord(word,baseWord,21,22);
  word = iSector; // sector ID [0;17]
  AliBitPacking::PackWord(word,baseWord,23,27);
  word = 4; // 0100 -> DRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeDRMtrailer(UInt_t *buf)
{
  //
  // DRM global trailer
  //
  
  UInt_t baseWord;
  UInt_t word;
  
  baseWord=0;
  word = 1; // 0001 -> DRM data are coming from the VME slot number 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // local event counter
  AliBitPacking::PackWord(word,baseWord, 4,15);
  word = 0; // reserved for future use
  AliBitPacking::PackWord(word,baseWord,16,27);
  word = 5; // 0101 -> DRM trailer ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeLTMheader(UInt_t *buf)
{
  //
  // LTM header
  //

  UInt_t baseWord;
  UInt_t word;
  
  baseWord=0;
  word = 2; // 0010 -> LTM data are coming from the VME slot number 2
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 35; // event words
  AliBitPacking::PackWord(word,baseWord, 4,16);
  word = 0; // crc error
  AliBitPacking::PackWord(word,baseWord,17,17);
  word = 0; // fault
  AliBitPacking::PackWord(word,baseWord,18,23);
  word = 0;
  AliBitPacking::PackWord(word,baseWord,24,27);
  word = 4; // 0100 -> LTM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeLTMdata(UInt_t *buf)
{
  //
  // LTM data
  //

  UInt_t baseWord;
  UInt_t word;
  
  baseWord=0;
  word = 100; // Local temperature in LTM5 -> 4 X 25 degree (environment temperature)
  AliBitPacking::PackWord(word,baseWord, 0, 9);
  word = 100; // Local temperature in LTM6 -> 4 X 25 degree (environment temperature)
  AliBitPacking::PackWord(word,baseWord,10,19);
  word = 100; // Local temperature in LTM7 -> 4 X 25 degree (environment temperature)
  AliBitPacking::PackWord(word,baseWord,20,29);
  word = 0;
  AliBitPacking::PackWord(word,baseWord,30,31);

  fIndex++;
  buf[fIndex]=baseWord;

  // Local temperature in LTM2, LMT3, LTM4 -> 4 X 25 degree (environment temperature)
  fIndex++;
  buf[fIndex]=baseWord;

  // Local temperature in FEAC7, FEAC8, LTM1 -> 4 X 25 degree (environment temperature)
  fIndex++;
  buf[fIndex]=baseWord;

  // Local temperature in FEAC4, FEAC5, FEAC6 -> 4 X 25 degree (environment temperature)
  fIndex++;
  buf[fIndex]=baseWord;

  // Local temperature in FEAC1, FEAC2, FEAC3 -> 4 X 25 degree (environment temperature)
  fIndex++;
  buf[fIndex]=baseWord;

  baseWord=0;
  word = 0; // GND-FEAC15 -> Voltage drop between GND and FEAC15
  AliBitPacking::PackWord(word,baseWord, 0, 9);
  word = 0; // VTH16 -> Thereshould voltage for FEAC16
  AliBitPacking::PackWord(word,baseWord,10,19);
  word = 0; // GND-FEAC16 -> Voltage drop between GND and FEAC16
  AliBitPacking::PackWord(word,baseWord,20,29);
  word = 0;
  AliBitPacking::PackWord(word,baseWord,30,31);

  fIndex++;
  buf[fIndex]=baseWord;

  // VTH14 -> Thereshould voltage for FEAC14
  // GND-FEAC14 -> Voltage drop between GND and FEAC14
  // VTH15 -> Thereshould voltage for FEAC15
  fIndex++;
  buf[fIndex]=baseWord;

  // GND-FEAC12 -> Voltage drop between GND and FEAC12
  // VTH13 -> Thereshould voltage for FEAC13
  // GND-FEAC13 -> Voltage drop between GND and FEAC13
  fIndex++;
  buf[fIndex]=baseWord;

  // VTH11 -> Thereshould voltage for FEAC11
  // GND-FEAC11 -> Voltage drop between GND and FEAC11
  // VTH12 -> Thereshould voltage for FEAC12
  fIndex++;
  buf[fIndex]=baseWord;

  // GND-FEAC9 -> Voltage drop between GND and FEAC9
  // VTH10 -> Thereshould voltage for FEAC10
  // GND-FEAC10 -> Voltage drop between GND and FEAC10
  fIndex++;
  buf[fIndex]=baseWord;

  // VTH8 -> Thereshould voltage for FEAC8
  // GND-FEAC8 -> Voltage drop between GND and FEAC8
  // VTH9 -> Thereshould voltage for FEAC9
  fIndex++;
  buf[fIndex]=baseWord;

  // GND-FEAC6 -> Voltage drop between GND and FEAC6
  // VTH7 -> Thereshould voltage for FEAC7
  // GND-FEAC7 -> Voltage drop between GND and FEAC7
  fIndex++;
  buf[fIndex]=baseWord;

  // VTH5 -> Thereshould voltage for FEAC5
  // GND-FEAC5 -> Voltage drop between GND and FEAC5
  // VTH6 -> Thereshould voltage for FEAC6
  fIndex++;
  buf[fIndex]=baseWord;

  // GND-FEAC3 -> Voltage drop between GND and FEAC3
  // VTH4 -> Thereshould voltage for FEAC4
  // GND-FEAC4 -> Voltage drop between GND and FEAC4
  fIndex++;
  buf[fIndex]=baseWord;

  // VTH2 -> Thereshould voltage for FEAC2
  // GND-FEAC2 -> Voltage drop between GND and FEAC2
  // VTH3 -> Thereshould voltage for FEAC3
  fIndex++;
  buf[fIndex]=baseWord;

  // LV16 -> Low Voltage measured by FEAC16
  // GND-FEAC1 -> Voltage drop between GND and FEAC1
  // VTH1 -> Thereshould voltage for FEAC1
  fIndex++;
  buf[fIndex]=baseWord;

  // Low Voltage measured by FEAC13, FEAC14, FEAC15
  fIndex++;
  buf[fIndex]=baseWord;

  // Low Voltage measured by FEAC10, FEAC11, FEAC12
  fIndex++;
  buf[fIndex]=baseWord;

  // Low Voltage measured by FEAC7, FEAC8, FEAC9
  fIndex++;
  buf[fIndex]=baseWord;

  // Low Voltage measured by FEAC4, FEAC5, FEAC6
  fIndex++;
  buf[fIndex]=baseWord;

  // Low Voltage measured by FEAC1, FEAC2, FEAC3
  fIndex++;
  buf[fIndex]=baseWord;


  baseWord=0;
  word = 0; // PDL45 -> Delay Line setting for PDL45
  AliBitPacking::PackWord(word,baseWord, 0, 7);
  word = 0; // PDL46 -> Delay Line setting for PDL46
  AliBitPacking::PackWord(word,baseWord, 8,15);
  word = 0; // PDL47 -> Delay Line setting for PDL47
  AliBitPacking::PackWord(word,baseWord,16,23);
  word = 0; // PDL48 -> Delay Line setting for PDL48
  AliBitPacking::PackWord(word,baseWord,24,31);
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL41, PDL42, PDL43, PDL44
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL37, PDL38, PDL39, PDL40
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL33, PDL34, PDL35, PDL36
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL29, PDL30, PDL31, PDL32
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL25, PDL26, PDL27, PDL28
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL21, PDL22, PDL23, PDL24
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL17, PDL18, PDL19, PDL20
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL13, PDL14, PDL15, PDL16
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL9, PDL10, PDL11, PDL12
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL5, PDL6, PDL7, PDL8
  fIndex++;
  buf[fIndex]=baseWord;

  // Delay Line setting for PDL1, PDL2, PDL3, PDL4
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeLTMtrailer(UInt_t *buf)
{
  //
  // LTM trailer
  //
 
  UInt_t baseWord;
  UInt_t word;
  
  baseWord=0;
  word = 2; // 0010 -> LTM data are coming from the VME slot number 2
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // event crc
  AliBitPacking::PackWord(word,baseWord, 4,15);
  word = 0; // event number
  AliBitPacking::PackWord(word,baseWord,16,27);
  word = 5; // 0101 -> LTM trailer ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTRMheader(Int_t nTRM, UInt_t *buf)
{
  //
  // TRM header for the TRM number nTRM [ 3;12]
  //

  if (nTRM<3 || nTRM>12) {
    AliWarning(Form(" TRM number is out of the right range [3;12] (nTRM = %3i",nTRM));
    return;
  }

  UInt_t baseWord;
  UInt_t word;

  baseWord = 0;
  word = nTRM; // TRM data coming from the VME slot number nTRM
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // event words
  AliBitPacking::PackWord(word,baseWord, 4,16);

  if (fPackedAcquisition)
    word = 0; // ACQuisition mode: [0;3] see document
  else
    word = 3; // ACQuisition mode: [0;3] see document
  AliBitPacking::PackWord(word,baseWord,17,18);
  word = 0; // description of a SEU inside LUT tables for INL compensation;
            // the data are unaffected
  AliBitPacking::PackWord(word,baseWord,19,19);
  word = 0; // Must Be Zero (MBZ)
  AliBitPacking::PackWord(word,baseWord,20,27);
  word = 4; // 0100 -> TRM header ID
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTRMtrailer(UInt_t *buf)
{
  //
  // TRM trailer
  //

  UInt_t baseWord;
  UInt_t word;

  baseWord=0;
  word = 15; // 1111 -> TRM trailer ID 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // event CRC
  AliBitPacking::PackWord(word,baseWord, 4,15);
  word = 0; // local event counter == DRM local event counter
  AliBitPacking::PackWord(word,baseWord,16,27);
  word = 5; // 0101 -> TRM trailer ID 2
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTRMchainHeader(Int_t nTRM, Int_t iChain,
					  UInt_t *buf)
{
  //
  // TRM chain header
  //
  
  UInt_t baseWord;
  UInt_t word;

  if (nTRM<3 || nTRM>12) {
    AliWarning(Form(" TRM number is out of the right range [3;12] (nTRM = %3i", nTRM));
    return;
  }
  
  if (iChain<0 || iChain>1) {
    AliWarning(Form(" Chain number is out of the right range [0;1] (iChain = %3i", iChain));
    return;
  }

  baseWord=0;
  word = nTRM; // TRM data coming from the VME slot ID nTRM
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // bunch ID
  AliBitPacking::PackWord(word,baseWord, 4,15);
  word = 100; // PB24 temperature -> 4 X 25 degree (environment temperature)
  AliBitPacking::PackWord(word,baseWord,16,23);
  word = (Int_t)(5 * gRandom->Rndm()); // PB24 ID [0;4]
  AliBitPacking::PackWord(word,baseWord,24,26);
  word = 0; // TS
  AliBitPacking::PackWord(word,baseWord,27,27);
  switch (iChain) {
    case 0:
      word = 0; // 0000 -> TRM chain 0 ID
      break;
    case 1:
      word = 2; // 0010 -> TRM chain 1 ID
      break;
    }
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;
	    
}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTRMfiller(UInt_t *buf, UInt_t nWordsPerTRM)
{
  //
  // TRM filler
  //

  Int_t jj = -1;

  fIndex++;
  for (jj=fIndex; jj>fIndex-(Int_t)nWordsPerTRM; jj--) {
    buf[jj] = buf[jj-1];
  }

  buf[fIndex-nWordsPerTRM] = MakeFiller();

}
  
//----------------------------------------------------------------------------
UInt_t AliTOFDDLRawData::MakeFiller()
{
  //
  // Filler word definition: to make even the number of words per TRM/LTM
  //

  UInt_t baseWord;
  UInt_t word;

  baseWord=0;
  word = 0; // 0000 -> filler ID 1
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // MBZ
  AliBitPacking::PackWord(word,baseWord, 4,27);
  word = 7; // 0111 -> filler ID 2
  AliBitPacking::PackWord(word,baseWord, 28,31);
  
  return baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTRMchainTrailer(Int_t iChain, UInt_t *buf)
{
  //
  // TRM chain trailer
  //

  if (iChain<0 || iChain>1) {
    AliWarning(Form(" Chain number is out of the right range [0;1] (iChain = %3i", iChain));
    return;
  }

  UInt_t baseWord;
  UInt_t word;
  
  baseWord=0;
  word = 0; // status
  AliBitPacking::PackWord(word,baseWord, 0, 3);
  word = 0; // MBZ
  AliBitPacking::PackWord(word,baseWord, 4,15);
  word = 0; // event counter
  AliBitPacking::PackWord(word,baseWord,16,27);
  switch (iChain) {
    case 0:
      word = 1; // 0001 -> TRM chain 0 trailer ID
      break;
    case 1:
      word = 3; // 0011 -> TRM chain 1 trailer ID
      break;
    }
  AliBitPacking::PackWord(word,baseWord,28,31);
  fIndex++;
  buf[fIndex]=baseWord;

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::MakeTDCdigits(Int_t nDDL, Int_t nTRM, Int_t iChain,
				     UInt_t *buf, UInt_t &nWordsPerTRM)
{
  //
  // TRM TDC digit
  //

  const Double_t kOneMoreFilledCell = 1./(fTOFgeometry->NPadXSector()*fTOFgeometry->NSectors());
  Double_t percentFilledCells = Double_t(fTOFdigitMap->GetFilledCellNumber())/(fTOFgeometry->NPadXSector()*fTOFgeometry->NSectors());

  if (nDDL<0 || nDDL>71) {
    AliWarning(Form(" DDL number is out of the right range [0;71] (nDDL = %3i", nDDL));
    return;
  }
  
  if (nTRM<3 || nTRM>12) {
    AliWarning(Form(" TRM number is out of the right range [3;12] (nTRM = %3i", nTRM));
    return;
  }
  
  if (iChain<0 || iChain>1) {
    AliWarning(Form(" Chain number is out of the right range [0;1] (iChain = %3i", iChain));
    return;
  }
  
  Int_t psArray[1000];
  UInt_t localBuffer[1000];
  Int_t localIndex = -1;

  Int_t iDDL = nDDL%AliTOFGeometry::NDDL();

  Int_t volume[5] = {-1, -1, -1, -1, -1};
  Int_t indexDigit[3] = {-1, -1, -1};

  Int_t totCharge = -1;
  Int_t timeOfFlight = -1;

  Int_t trailingSpurious = -1;
  Int_t leadingSpurious = -1;

  AliTOFdigit *digs;

  UInt_t baseWord=0;
  UInt_t word=0;

  Int_t jj = -1;
  Int_t nTDC = -1;
  Int_t iCH = -1;

  Int_t numberOfMeasuresPerChannel = 0;
  Int_t maxMeasuresPerChannelInTDC = 0;

  Bool_t outOut = HeadOrTail();

  ofstream ftxt;

  if (fVerbose==2) ftxt.open("TOFdigits.txt",ios::app);

  for (jj=0; jj<5; jj++) volume[jj] = -1;

  // loop on TDC number
  for (nTDC=AliTOFGeometry::NTdc()-1; nTDC>=0; nTDC--) {

    maxMeasuresPerChannelInTDC = 0;

    // the DRM odd (i.e. left) slot number 3 doesn't contain TDC digit data
    // for TDC numbers 3-14
    if (iDDL%2==1 && nTRM==3 && (Int_t)(nTDC/3.)!=0) continue;

    // loop on TDC channel number
    for (iCH=AliTOFGeometry::NCh()-1; iCH>=0; iCH--) {

      if (numberOfMeasuresPerChannel) {
	maxMeasuresPerChannelInTDC = numberOfMeasuresPerChannel;
	numberOfMeasuresPerChannel = 0;
      }

      fTOFrawStream->EquipmentId2VolumeId(nDDL, nTRM, iChain, nTDC, iCH, volume);
	
      if (volume[0]==-1 || volume[1]==-1 || volume[2]==-1 ||
	  volume[3]==-1 || volume[4]==-1) continue;

      for (jj=0; jj<3; jj++) indexDigit[jj] = -1;

      fTOFdigitMap->GetDigitIndex(volume, indexDigit);

      if (indexDigit[0]<0) {

	trailingSpurious = Int_t(2097152*gRandom->Rndm());
	leadingSpurious = Int_t(2097152*gRandom->Rndm());

	if ( fFakeOrphaneProduction &&
	     ( ( fPackedAcquisition && percentFilledCells<0.12 && gRandom->Rndm()<(0.12-percentFilledCells) ) ||
	       (!fPackedAcquisition && percentFilledCells<0.24 && gRandom->Rndm()<(0.24-percentFilledCells) )  )  ) {

	  percentFilledCells+=kOneMoreFilledCell;

	  Int_t dummyPS = 0;

	  if (outOut) {
	    word = trailingSpurious; // trailing edge measurement
	    dummyPS = 2;
	  }
	  else {
	    word = leadingSpurious; // leading edge measurement
	    dummyPS = 1;
	  }

	  if (fVerbose==2) {
	    if (nDDL<10) ftxt << "  " << nDDL;
	    else         ftxt << " " << nDDL;
	    if (nTRM<10) ftxt << "  " << nTRM;
	    else         ftxt << " " << nTRM;
	    ftxt << "  " << iChain;
	    if (nTDC<10) ftxt << "  " << nTDC;
	    else         ftxt << " " << nTDC;
	    ftxt << "  " << iCH;
	    if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	    else              ftxt  << "  -> " << volume[0];
	    ftxt << "  " << volume[1];
	    if (volume[2]<10) ftxt << "  " << volume[2];
	    else              ftxt << " " << volume[2];
	    ftxt << "  " << volume[4];
	    if (volume[3]<10) ftxt << "  " << volume[3];
	    else              ftxt << " " << volume[3];
	    ftxt << "   " << -1;
	    if (word<10)                           ftxt << "        " << word;
	    else if (word>=10     && word<100)     ftxt << "       " << word;
	    else if (word>=100    && word<1000)    ftxt << "      " << word;
	    else if (word>=1000   && word<10000)   ftxt << "     " << word;
	    else if (word>=10000  && word<100000)  ftxt << "    " << word;
	    else if (word>=100000 && word<1000000) ftxt << "   " << word;
	    else                                   ftxt << "  " << word;
	    ftxt << "   " << dummyPS << endl;
	  }

	  AliBitPacking::PackWord(word,baseWord, 0,20);
	  word = iCH; // TDC channel ID [0;7]
	  AliBitPacking::PackWord(word,baseWord,21,23);
	  word = nTDC; // TDC ID [0;14]
	  AliBitPacking::PackWord(word,baseWord,24,27);
	  word = 0; // error flag
	  AliBitPacking::PackWord(word,baseWord,28,28);
	  word = dummyPS; // Packing Status [0;3]
	  AliBitPacking::PackWord(word,baseWord,29,30);
	  word = 1; // TRM TDC digit ID
	  AliBitPacking::PackWord(word,baseWord,31,31);

	  localIndex++;
	  localBuffer[localIndex]=baseWord;
	  psArray[localIndex]=dummyPS;

	  nWordsPerTRM++;
	  baseWord=0;

	} // if ( fFakeOrphaneProduction && ( ( fPackedAcquisition && percentFilledCells<0.12 && gRandom->Rndm()<(0.12-percentFilledCells) ) or ... ) )
      } // if (indexDigit[0]<0)

      for (jj=0; jj<3;jj++) {

	numberOfMeasuresPerChannel++;

	if (indexDigit[jj]<0) continue;
	digs = (AliTOFdigit*)fTOFdigitArray->UncheckedAt(indexDigit[jj]);
	  
	if (digs->GetSector()!=volume[0] ||
	    digs->GetPlate() !=volume[1] ||
	    digs->GetStrip() !=volume[2] ||
	    digs->GetPadx()  !=volume[3] ||
	    digs->GetPadz()  !=volume[4]) AliWarning(" --- ERROR --- ");

	timeOfFlight = (Int_t)(digs->GetTdc())%8192;

	if (timeOfFlight>fMatchingWindow) continue;

	// totCharge = (Int_t)digs->GetAdc(); //Use realistic ToT, for Standard production with no miscalibration/Slewing it == fAdC in digit (see AliTOFDigitizer)
	totCharge = (Int_t)(digs->GetToT());
	// temporary control
	if (totCharge<0) totCharge = 0;//TMath::Abs(totCharge);

	if (fPackedAcquisition) {

	if (fVerbose==2) {
	  if (nDDL<10) ftxt << "  " << nDDL;
	  else         ftxt << " " << nDDL;
	  if (nTRM<10) ftxt << "  " << nTRM;
	  else         ftxt << " " << nTRM;
	  ftxt << "  " << iChain;
	  if (nTDC<10) ftxt << "  " << nTDC;
	  else         ftxt << " " << nTDC;
	  ftxt << "  " << iCH;
	  if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	  else              ftxt  << "  -> " << volume[0];
	  ftxt << "  " << volume[1];
	  if (volume[2]<10) ftxt << "  " << volume[2];
	  else              ftxt << " " << volume[2];
	  ftxt << "  " << volume[4];
	  if (volume[3]<10) ftxt << "  " << volume[3];
	  else              ftxt << " " << volume[3];
	  if (totCharge<10)                        ftxt << "    " << totCharge;
	  else if (totCharge>=10 && totCharge<100) ftxt << "   " << totCharge;
	  else                                     ftxt << "  " << totCharge;
	  if (timeOfFlight<10)                             ftxt << "     " << timeOfFlight << endl;
	  else if (timeOfFlight>=10  && timeOfFlight<100)  ftxt << "    " << timeOfFlight << endl;
	  else if (timeOfFlight>=100 && timeOfFlight<1000) ftxt << "   " << timeOfFlight << endl;
	  else                                             ftxt << "  " << timeOfFlight << endl;
	}

	word = timeOfFlight%8192; // time-of-fligth measurement
	AliBitPacking::PackWord(word,baseWord, 0,12);

	if (totCharge>=256) totCharge = 255;
	word = totCharge; // time-over-threshould measurement
	AliBitPacking::PackWord(word,baseWord,13,20);

	word = iCH; // TDC channel ID [0;7]
	AliBitPacking::PackWord(word,baseWord,21,23);
	word = nTDC; // TDC ID [0;14]
	AliBitPacking::PackWord(word,baseWord,24,27);
	word = 0; // error flag
	AliBitPacking::PackWord(word,baseWord,28,28);
	word = 0; // Packing Status [0;3]
	AliBitPacking::PackWord(word,baseWord,29,30);
	word = 1; // TRM TDC digit ID
	AliBitPacking::PackWord(word,baseWord,31,31);

	localIndex++;
	localBuffer[localIndex]=baseWord;

	nWordsPerTRM++;
	baseWord=0;

	if ( fFakeOrphaneProduction &&
	     percentFilledCells<0.12 && gRandom->Rndm()<(0.12-percentFilledCells) ) {

	  percentFilledCells+=kOneMoreFilledCell;

	  trailingSpurious = Int_t(2097152*gRandom->Rndm());
	  leadingSpurious = Int_t(2097152*gRandom->Rndm());

	  Int_t dummyPS = 0;

	  if (outOut) {
	    word = trailingSpurious; // trailing edge measurement
	    dummyPS = 2;
	  }
	  else {
	    word = leadingSpurious; // leading edge measurement
	    dummyPS = 1;
	  }

	  if (fVerbose==2) {
	    if (nDDL<10) ftxt << "  " << nDDL;
	    else         ftxt << " " << nDDL;
	    if (nTRM<10) ftxt << "  " << nTRM;
	    else         ftxt << " " << nTRM;
	    ftxt << "  " << iChain;
	    if (nTDC<10) ftxt << "  " << nTDC;
	    else         ftxt << " " << nTDC;
	    ftxt << "  " << iCH;
	    if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	    else              ftxt  << "  -> " << volume[0];
	    ftxt << "  " << volume[1];
	    if (volume[2]<10) ftxt << "  " << volume[2];
	    else              ftxt << " " << volume[2];
	    ftxt << "  " << volume[4];
	    if (volume[3]<10) ftxt << "  " << volume[3];
	    else              ftxt << " " << volume[3];
	    ftxt << "   " << -1;
	    if (word<10)                           ftxt << "        " << word;
	    else if (word>=10     && word<100)     ftxt << "       " << word;
	    else if (word>=100    && word<1000)    ftxt << "      " << word;
	    else if (word>=1000   && word<10000)   ftxt << "     " << word;
	    else if (word>=10000  && word<100000)  ftxt << "    " << word;
	    else if (word>=100000 && word<1000000) ftxt << "   " << word;
	    else                                   ftxt << "  " << word;
	    ftxt << "   " << dummyPS << endl;
	  }

	  AliBitPacking::PackWord(word,baseWord, 0,20);
	  word = iCH; // TDC channel ID [0;7]
	  AliBitPacking::PackWord(word,baseWord,21,23);
	  word = nTDC; // TDC ID [0;14]
	  AliBitPacking::PackWord(word,baseWord,24,27);
	  word = 0; // error flag
	  AliBitPacking::PackWord(word,baseWord,28,28);
	  word = dummyPS; // Packing Status [0;3]
	  AliBitPacking::PackWord(word,baseWord,29,30);
	  word = 1; // TRM TDC digit ID
	  AliBitPacking::PackWord(word,baseWord,31,31);

	  localIndex++;
	  localBuffer[localIndex]=baseWord;
	  psArray[localIndex]=dummyPS;

	  nWordsPerTRM++;
	  baseWord=0;

	} // if ( fFakeOrphaneProduction && percentFilledCells<0.12 && gRandom->Rndm()<(0.12-percentFilledCells) )


	} // if (fPackedAcquisition)
	else { // if (!fPackedAcquisition)

	if ( fFakeOrphaneProduction &&
	     percentFilledCells<0.24 && gRandom->Rndm()<(0.24-percentFilledCells) && outOut ) {

	  percentFilledCells+=kOneMoreFilledCell;

	  trailingSpurious = Int_t(2097152*gRandom->Rndm());
	  word = trailingSpurious;
	  Int_t dummyPS = 2;

	  if (fVerbose==2) {
	    if (nDDL<10) ftxt << "  " << nDDL;
	    else         ftxt << " " << nDDL;
	    if (nTRM<10) ftxt << "  " << nTRM;
	    else         ftxt << " " << nTRM;
	    ftxt << "  " << iChain;
	    if (nTDC<10) ftxt << "  " << nTDC;
	    else         ftxt << " " << nTDC;
	    ftxt << "  " << iCH;
	    if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	    else              ftxt  << "  -> " << volume[0];
	    ftxt << "  " << volume[1];
	    if (volume[2]<10) ftxt << "  " << volume[2];
	    else              ftxt << " " << volume[2];
	    ftxt << "  " << volume[4];
	    if (volume[3]<10) ftxt << "  " << volume[3];
	    else              ftxt << " " << volume[3];
	    ftxt << "   " << -1;
	    if (word<10)                           ftxt << "        " << word;
	    else if (word>=10     && word<100)     ftxt << "       " << word;
	    else if (word>=100    && word<1000)    ftxt << "      " << word;
	    else if (word>=1000   && word<10000)   ftxt << "     " << word;
	    else if (word>=10000  && word<100000)  ftxt << "    " << word;
	    else if (word>=100000 && word<1000000) ftxt << "   " << word;
	    else                                   ftxt << "  " << word;
	    ftxt << "   " << dummyPS << endl;
	  }

	  AliBitPacking::PackWord(word,baseWord, 0,20);
	  word = iCH; // TDC channel ID [0;7]
	  AliBitPacking::PackWord(word,baseWord,21,23);
	  word = nTDC; // TDC ID [0;14]
	  AliBitPacking::PackWord(word,baseWord,24,27);
	  word = 0; // error flag
	  AliBitPacking::PackWord(word,baseWord,28,28);
	  word = dummyPS; // Packing Status [0;3]
	  AliBitPacking::PackWord(word,baseWord,29,30);
	  word = 1; // TRM TDC digit ID
	  AliBitPacking::PackWord(word,baseWord,31,31);

	  localIndex++;
	  localBuffer[localIndex]=baseWord;
	  psArray[localIndex]=dummyPS;

	  nWordsPerTRM++;
	  baseWord=0;

	} // if ( fFakeOrphaneProduction && percentFilledCells<0.24 && gRandom->Rndm()<(0.24-percentFilledCells)  && outOut )


	word = (timeOfFlight + Int_t(totCharge*AliTOFGeometry::ToTBinWidth()/AliTOFGeometry::TdcBinWidth()))%2097152; // trailing edge measurement

	if (fVerbose==2) {
	  if (nDDL<10) ftxt << "  " << nDDL;
	  else         ftxt << " " << nDDL;
	  if (nTRM<10) ftxt << "  " << nTRM;
	  else         ftxt << " " << nTRM;
	  ftxt << "  " << iChain;
	  if (nTDC<10) ftxt << "  " << nTDC;
	  else         ftxt << " " << nTDC;
	  ftxt << "  " << iCH;
	  if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	  else              ftxt  << "  -> " << volume[0];
	  ftxt << "  " << volume[1];
	  if (volume[2]<10) ftxt << "  " << volume[2];
	  else              ftxt << " " << volume[2];
	  ftxt << "  " << volume[4];
	  if (volume[3]<10) ftxt << "  " << volume[3];
	  else              ftxt << " " << volume[3];
	  ftxt << "   " << -1;
	  if (word<10)                           ftxt << "        " << word;
	  else if (word>=10     && word<100)     ftxt << "       " << word;
	  else if (word>=100    && word<1000)    ftxt << "      " << word;
	  else if (word>=1000   && word<10000)   ftxt << "     " << word;
	  else if (word>=10000  && word<100000)  ftxt << "    " << word;
	  else if (word>=100000 && word<1000000) ftxt << "   " << word;
	  else                                   ftxt << "  " << word;
	  ftxt << "   " << 2 << endl;
	}

	AliBitPacking::PackWord(word,baseWord, 0,20);

	word = iCH; // TDC channel ID [0;7]
	AliBitPacking::PackWord(word,baseWord,21,23);
	word = nTDC; // TDC ID [0;14]
	AliBitPacking::PackWord(word,baseWord,24,27);
	word = 0; // error flag
	AliBitPacking::PackWord(word,baseWord,28,28);
	word = 2; // Packing Status [0;3]
	AliBitPacking::PackWord(word,baseWord,29,30);
	word = 1; // TRM TDC digit ID
	AliBitPacking::PackWord(word,baseWord,31,31);

	localIndex++;
	localBuffer[localIndex]=baseWord;
	psArray[localIndex]=2;

	nWordsPerTRM++;
	baseWord=0;

	word = timeOfFlight%2097152; // leading edge measurement

	if (fVerbose==2) {
	  if (nDDL<10) ftxt << "  " << nDDL;
	  else         ftxt << " " << nDDL;
	  if (nTRM<10) ftxt << "  " << nTRM;
	  else         ftxt << " " << nTRM;
	  ftxt << "  " << iChain;
	  if (nTDC<10) ftxt << "  " << nTDC;
	  else         ftxt << " " << nTDC;
	  ftxt << "  " << iCH;
	  if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	  else              ftxt  << "  -> " << volume[0];
	  ftxt << "  " << volume[1];
	  if (volume[2]<10) ftxt << "  " << volume[2];
	  else              ftxt << " " << volume[2];
	  ftxt << "  " << volume[4];
	  if (volume[3]<10) ftxt << "  " << volume[3];
	  else              ftxt << " " << volume[3];
	  ftxt << "   " << -1;
	  if (word<10)                           ftxt << "        " << word;
	  else if (word>=10     && word<100)     ftxt << "       " << word;
	  else if (word>=100    && word<1000)    ftxt << "      " << word;
	  else if (word>=1000   && word<10000)   ftxt << "     " << word;
	  else if (word>=10000  && word<100000)  ftxt << "    " << word;
	  else if (word>=100000 && word<1000000) ftxt << "   " << word;
	  else                                   ftxt << "  " << word;
	  ftxt << "   " << 1 << endl;
	}

	AliBitPacking::PackWord(word,baseWord, 0,20);

	word = iCH; // TDC channel ID [0;7]
	AliBitPacking::PackWord(word,baseWord,21,23);
	word = nTDC; // TDC ID [0;14]
	AliBitPacking::PackWord(word,baseWord,24,27);
	word = 0; // error flag
	AliBitPacking::PackWord(word,baseWord,28,28);
	word = 1; // Packing Status [0;3]
	AliBitPacking::PackWord(word,baseWord,29,30);
	word = 1; // TRM TDC digit ID
	AliBitPacking::PackWord(word,baseWord,31,31);

	localIndex++;
	localBuffer[localIndex]=baseWord;
	psArray[localIndex]=1;

	nWordsPerTRM++;
	baseWord=0;


	if ( fFakeOrphaneProduction &&
	     percentFilledCells<0.24 && gRandom->Rndm()<(0.24-percentFilledCells) && !outOut ) {

	  percentFilledCells+=kOneMoreFilledCell;

	  leadingSpurious = Int_t(2097152*gRandom->Rndm());
	  word = leadingSpurious;
	  Int_t dummyPS = 1;

	  if (fVerbose==2) {
	    if (nDDL<10) ftxt << "  " << nDDL;
	    else         ftxt << " " << nDDL;
	    if (nTRM<10) ftxt << "  " << nTRM;
	    else         ftxt << " " << nTRM;
	    ftxt << "  " << iChain;
	    if (nTDC<10) ftxt << "  " << nTDC;
	    else         ftxt << " " << nTDC;
	    ftxt << "  " << iCH;
	    if (volume[0]<10) ftxt  << "  ->  " << volume[0];
	    else              ftxt  << "  -> " << volume[0];
	    ftxt << "  " << volume[1];
	    if (volume[2]<10) ftxt << "  " << volume[2];
	    else              ftxt << " " << volume[2];
	    ftxt << "  " << volume[4];
	    if (volume[3]<10) ftxt << "  " << volume[3];
	    else              ftxt << " " << volume[3];
	    ftxt << "   " << -1;
	    if (word<10)                           ftxt << "        " << word;
	    else if (word>=10     && word<100)     ftxt << "       " << word;
	    else if (word>=100    && word<1000)    ftxt << "      " << word;
	    else if (word>=1000   && word<10000)   ftxt << "     " << word;
	    else if (word>=10000  && word<100000)  ftxt << "    " << word;
	    else if (word>=100000 && word<1000000) ftxt << "   " << word;
	    else                                   ftxt << "  " << word;
	    ftxt << "   " << dummyPS << endl;
	  }

	  AliBitPacking::PackWord(word,baseWord, 0,20);
	  word = iCH; // TDC channel ID [0;7]
	  AliBitPacking::PackWord(word,baseWord,21,23);
	  word = nTDC; // TDC ID [0;14]
	  AliBitPacking::PackWord(word,baseWord,24,27);
	  word = 0; // error flag
	  AliBitPacking::PackWord(word,baseWord,28,28);
	  word = dummyPS; // Packing Status [0;3]
	  AliBitPacking::PackWord(word,baseWord,29,30);
	  word = 1; // TRM TDC digit ID
	  AliBitPacking::PackWord(word,baseWord,31,31);

	  localIndex++;
	  localBuffer[localIndex]=baseWord;
	  psArray[localIndex]=dummyPS;

	  nWordsPerTRM++;
	  baseWord=0;

	} // if ( fFakeOrphaneProduction && percentFilledCells<0.24 && gRandom->Rndm()<(0.24-percentFilledCells) && !outOut )


	} // if (!fPackedAcquisition)

      } //end loop on digits in the same volume

    } // end loop on TDC channel number

    if (localIndex==-1) continue;

    if (fPackedAcquisition) {

      for (Int_t jj=0; jj<=localIndex; jj++) {
	fIndex++;
	buf[fIndex] = localBuffer[jj];
	localBuffer[jj] = 0;
	psArray[jj] = -1;
      }

    }
    else {

      if (maxMeasuresPerChannelInTDC = 1) {

	for (Int_t jj=0; jj<=localIndex; jj++) {
	  if (psArray[jj]==2) {
	    fIndex++;
	    buf[fIndex] = localBuffer[jj];
	    localBuffer[jj] = 0;
	    psArray[jj] = -1;
	  }
	}
	for (Int_t jj=0; jj<=localIndex; jj++) {
	  if (psArray[jj]==1) {
	    fIndex++;
	    buf[fIndex] = localBuffer[jj];
	    localBuffer[jj] = 0;
	    psArray[jj] = -1;
	  }
	}

      } // if (maxMeasuresPerChannelInTDC = 1)
      else if (maxMeasuresPerChannelInTDC>1) {

	AliInfo(Form(" In the TOF DDL %2i, TRM %2i, TDC %2i, chain %1i, there are %2i t.o.f. good measurements ",
		     nDDL, nTRM, iChain, nTDC, iCH, maxMeasuresPerChannelInTDC));

	for (Int_t jj=0; jj<=localIndex; jj++) {
	    fIndex++;
	    buf[fIndex] = localBuffer[jj];
	    localBuffer[jj] = 0;
	    psArray[jj] = -1;
	}

      } // else if (maxMeasuresPerChannelInTDC>1)

    } // else (!fPackedAcquisition)

    localIndex = -1;

  } // end loop on TDC number


  if (fVerbose==2) ftxt.close();

}

//----------------------------------------------------------------------------
void AliTOFDDLRawData::ReverseArray(UInt_t a[], Int_t n) const
{
  //
  // Reverses the n elements of array a
  //

  Int_t ii, temp;

  for (ii=0; ii<n/2; ii++) {
    temp      = a[ii];
    a[ii]     = a[n-ii-1];
    a[n-ii-1] = temp;
  }

  return;

}

//----------------------------------------------------------------------------
Bool_t AliTOFDDLRawData::HeadOrTail() const
{
  //
  // Returns the result of a 'pitch and toss'
  //

  Double_t dummy = gRandom->Rndm();

  if (dummy<0.5) return kFALSE;
  else return kTRUE;

}

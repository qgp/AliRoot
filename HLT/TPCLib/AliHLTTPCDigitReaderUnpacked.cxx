// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          Jochen Thaeder <thaeder@kip.uni-heidelberg.de>                *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTTPCDigitReaderUnpacked.cxx
    @author Timm Steinbeck, Jochen Thaeder, Matthias Richter
    @date   
    @brief  A digit reader implementation for unpacked TPC data.
*/

#if __GNUC__== 3
using namespace std;
#endif

#include "AliHLTTPCDigitReaderUnpacked.h"
#include "AliHLTTPCDigitData.h"
#include "AliHLTTPCRawDataFormat.h"
#include "AliHLTTPCTransform.h"
#include "AliHLTStdIncludes.h"

ClassImp(AliHLTTPCDigitReaderUnpacked)

AliHLTTPCDigitReaderUnpacked::AliHLTTPCDigitReaderUnpacked()
  :
  fDigitRowData(NULL),
  fActRowData(NULL),
  fData(NULL),
  fPtr(NULL),
  fSize(0),
  fBin(0),
  fRow(0),
  fFirstRow(0),
  fLastRow(0)
{
}

AliHLTTPCDigitReaderUnpacked::AliHLTTPCDigitReaderUnpacked(const AliHLTTPCDigitReaderUnpacked& src)
  :
  fDigitRowData(NULL),
  fActRowData(NULL),
  fData(NULL),
  fPtr(NULL),
  fSize(0),
  fBin(0),
  fRow(0),
  fFirstRow(0),
  fLastRow(0)
{
  HLTFatal("copy constructor not for use");
}

AliHLTTPCDigitReaderUnpacked& AliHLTTPCDigitReaderUnpacked::operator=(const AliHLTTPCDigitReaderUnpacked& src)
{
  fDigitRowData=NULL;
  fActRowData=NULL;
  fData=NULL;
  fPtr=NULL;
  fSize=0;
  fBin=0;
  fRow=0;
  fFirstRow=0;
  fLastRow=0;
  HLTFatal("assignment operator not for use");
  return (*this);
}

AliHLTTPCDigitReaderUnpacked::~AliHLTTPCDigitReaderUnpacked(){
}

int AliHLTTPCDigitReaderUnpacked::InitBlock(void* ptr,unsigned long size, Int_t patch, Int_t slice){
  AliHLTTPCUnpackedRawData *tmpptr;
  fPtr = ptr;
  fSize = size;

  tmpptr = (AliHLTTPCUnpackedRawData*) fPtr;
  fDigitRowData = (AliHLTTPCDigitRowData*) tmpptr->fDigits;
  fActRowData = (AliHLTTPCDigitRowData*) fDigitRowData;

  fBin = -1;

  fFirstRow=AliHLTTPCTransform::GetFirstRow(patch);
  fLastRow=AliHLTTPCTransform::GetLastRow(patch);

  return 0;
  fRow = fFirstRow; 

  if ((Int_t)fActRowData->fRow != fRow){
      HLTWarning("Row number should match! fActRowData->fRow=%d fRow=%d", fActRowData->fRow, fRow);
  }
}

bool AliHLTTPCDigitReaderUnpacked::Next(){
  bool rreadvalue = true;

  fBin++;

  if ( fBin >= (Int_t)fActRowData->fNDigit ){

    fRow++;

    if ((fRow >= fFirstRow) && (fRow <= fLastRow)){

      //new row 
      Byte_t *tmp = (Byte_t*) fActRowData;
      Int_t size = sizeof(AliHLTTPCDigitRowData) + fActRowData->fNDigit*sizeof(AliHLTTPCDigitData);
      tmp += size;
      fActRowData = (AliHLTTPCDigitRowData*) tmp;
     
      if (((Byte_t*)fPtr) + fSize <= tmp){
	rreadvalue = false;
	return rreadvalue;
      }

      fBin = 0;
    }
    else {
      rreadvalue = false;
      return rreadvalue;
    }
    
    if ((Int_t)fActRowData->fRow != fRow){
      HLTWarning("Row number should match! fActRowData->fRow=%d fRow=%d", fActRowData->fRow, fRow);
    }
  }

  fData = fActRowData->fDigitData;
 
  return rreadvalue;
}

int AliHLTTPCDigitReaderUnpacked::GetRow(){
  int rrow;
  rrow = fRow;
  return rrow;
}

int AliHLTTPCDigitReaderUnpacked::GetPad(){
  int rpad;
  rpad = (int)fData[fBin].fPad;
  return rpad   ;
}

int AliHLTTPCDigitReaderUnpacked::GetSignal(){ 
  int rsignal;
  rsignal = (int)fData[fBin].fCharge;
  return rsignal;
}

int AliHLTTPCDigitReaderUnpacked::GetTime(){
  int rtime;
  rtime = (int)fData[fBin].fTime;
  return rtime;
}

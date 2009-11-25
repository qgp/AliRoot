//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTPHOSSHAREDMEMORYINTERFACEV2_H
#define ALIHLTPHOSSHAREDMEMORYINTERFACEV2_H

/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Dep. of Physics                                         *
 * University of Oslo, Norway, 2007                                       *
 *                                                                        *
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project.*
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to perthi@fys.uio.no                                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "Rtypes.h"
//#include "AliHLTPHOSBase.h"

#include "AliHLTPHOSChannelRawDataStruct.h"
#include "AliHLTDataTypes.h"

class AliHLTPHOSChannelDataHeaderStruct;
class AliHLTPHOSChannelDataStruct;
class AliHLTPHOSCoordinate;

//class AliHLTPHOSChannelRawDataStruct;

class  AliHLTPHOSSharedMemoryInterfacev2
{
 public:
  AliHLTPHOSSharedMemoryInterfacev2();
  virtual ~AliHLTPHOSSharedMemoryInterfacev2();
  AliHLTPHOSChannelDataStruct*   NextChannel();
  void  NextRawChannel();
  void SetMemory(AliHLTPHOSChannelDataHeaderStruct* channelDataHeaderPtr);
  void Reset();
  
  const AliHLTPHOSChannelRawDataStruct & GetRawData() { return  fRawData; };
  
 private:
  AliHLTPHOSSharedMemoryInterfacev2(const  AliHLTPHOSSharedMemoryInterfacev2 & );
  AliHLTPHOSSharedMemoryInterfacev2 & operator = (const  AliHLTPHOSSharedMemoryInterfacev2 &);
  
  void Reset(AliHLTPHOSChannelRawDataStruct &str);
  
  AliHLTPHOSChannelDataStruct* fCurrentChannel;
  AliHLTUInt8_t* fChannelDataPtr;
  bool fIsSetMemory;
  bool fHasRawData;
  int fMaxCnt;
  int fCurrentCnt; 
  UShort_t *fRawDataPtr;

  AliHLTPHOSChannelRawDataStruct fRawData;

};

#endif

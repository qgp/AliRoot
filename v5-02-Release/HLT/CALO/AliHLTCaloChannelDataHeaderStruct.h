//-*- Mode: C++ -*-
// $Id: AliHLTPHOSChannelDataHeaderStruct.h 29824 2008-11-10 13:43:55Z richterm $

#ifndef ALIHLTCALOCHANNELDATAHEADERSTRUCT_H
#define ALIHLTCALOCHANNELDATAHEADERSTRUCT_H

/**************************************************************************
 * Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project.*
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
//#include "AliHLTPHOSCommonDefs.h"

#include "Rtypes.h"

//struct AliHLTCaloChannelDataHeaderStruct
struct AliHLTCaloChannelDataHeaderStruct
{
  //  Int_t fShmAddress;
  Short_t fNChannels;         //size in charcters
  Short_t fAlgorithm;
  Int_t fInfo;
  Bool_t fHasRawData;
};

#endif

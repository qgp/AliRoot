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
/// This class provides access to ITS SDD digits in test beam raw data,
/// for beam test of August 2004
///
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStreamSDDv2.h"
#include "AliRawReader.h"

ClassImp(AliITSRawStreamSDDv2)


AliITSRawStreamSDDv2::AliITSRawStreamSDDv2(AliRawReader* rawReader) :
  AliITSRawStreamSDD(rawReader)
    
{
// create an object to read ITS SDD raw digits


  fRawReader->Reset();
  fRawReader->SelectEquipment(17, 204, 204);
}


Bool_t AliITSRawStreamSDDv2::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left

  // skip the first 8 words
  while (fSkip < 8) {
    if (!fRawReader->ReadNextInt(fData)) return kFALSE;
    if ((fData >> 30) == 0x01) continue;  // JTAG word
    if (fSkip == 4) {
      if (fData != 0) {
	Error("Next", "data not valid: %8.8d", fData);
	return kFALSE;
      }
    }
    fSkip++;
  }

  while (kTRUE) {
    if ((fChannel < 0) || (fLastBit[fChannel] < fReadBits[fChannel])) {
      if (!fRawReader->ReadNextInt(fData)) return kFALSE;  // read next word

      fChannel = -1;
      if ((fData >> 28) == 0x02) {           // header
	fEventId = (fData >> 3) & 0x07FF;
	fCarlosId = (fData >> 1) & 0x03;
      } else if ((fData >> 28) == 0x03) {    // footer
	// ignored
      } else if ((fData >> 29) == 0x00) {    // error
	if ((fData & 0x1FFFFFFF) != 0) {
	  Error("Next", "error codes = %x, %x\n", 
		(fData >> 0) & 0x3FFF, (fData >> 14) & 0x3FFF);
	  return kFALSE;
	}
      } else if ((fData >> 30) == 0x01) {    // JTAG word
	// ignored
      } else if ((fData >> 30) == 0x02) {    // channel 0 data
	fChannel = 0;
      } else if ((fData >> 30) == 0x03) {    // channel 1 data
	fChannel = 1;
      } else {                               // unknown data format
	Error("Next", "invalid data: %8.8x\n", fData);
	return kFALSE;
      }

      if (fChannel >= 0) {          // add read word to the data
	fChannelData[fChannel] += 
	  (ULong64_t(fData & 0x3FFFFFFF) << fLastBit[fChannel]);
	fLastBit[fChannel] += 30;
      }

    } else {  // decode data
      if (fReadCode[fChannel]) {    // read the next code word
	fChannelCode[fChannel] = ReadBits();
	fReadCode[fChannel] = kFALSE;
	fReadBits[fChannel] = fgkCodeLength[fChannelCode[fChannel]];

      } else {                      // read the next data word
	UInt_t data = ReadBits();
	fReadCode[fChannel] = kTRUE;
	fReadBits[fChannel] = 3;
	if (fChannelCode[fChannel] == 0) {         // set the time bin
	  fTimeBin[fChannel] = data;
	} else if (fChannelCode[fChannel] == 1) {  // next anode
	  fTimeBin[fChannel] = 0;
	  fAnode[fChannel]++;
	} else {                                   // ADC signal data
	  fSignal = DecompAmbra(data + (1 << fChannelCode[fChannel]) + 
	    fLowThreshold[fChannel]);
	  fCoord1 = fAnode[fChannel];
	  fCoord2 = fTimeBin[fChannel];
	  fTimeBin[fChannel]++;
	  return kTRUE;
	}
      }
    }
  }

  return kFALSE;
}



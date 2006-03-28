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

//____________________________________________________________________
//                                                                          
// Buffer to read RAW ALTRO FMD format from a AliRawReader 
// 
// This class derives from AliAltroBuffer, but overloads the memer
// function Next to do some extra processing.  In particular, it tries
// to autodetect the sample rate.  If zero-suppression was used when
// writing the raw data, then the automatic discovery will not work,
// and the sample rate should be set explicitly. 
//
#include "AliFMDRawStream.h"		// ALIFMDRAWSTREAM_H
#include <AliRawReader.h>		// ALIRAWREADER_H
#include "AliFMDParameters.h"
#include <AliLog.h>
#include <iomanip>
#include <iostream>

//____________________________________________________________________
ClassImp(AliFMDRawStream)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliFMDRawStream::AliFMDRawStream(AliRawReader* reader) 
  : AliAltroRawStream(reader)
{
  fNoAltroMapping = kFALSE;
}

//_____________________________________________________________________________
Bool_t 
AliFMDRawStream::ReadChannel(UInt_t& ddl, UInt_t& addr, 
			     UInt_t& len, UShort_t* data)
{
  UInt_t       prevddl   = 0;
  Int_t        l         = 0;
  static Int_t last      = 0xFFFF; // 0xFFFF means signal is used
  Bool_t       next      = kTRUE;
  do {
    Int_t signal = last;
    if (last > 0x3FF) {
      AliDebug(30, Form("Last is 0x%x, so reading a new word", last));
      next   = Next();
      if (!next) break;
      signal = GetSignal();
      if (GetHWAddress() != GetPrevHWAddress() && GetPrevHWAddress() >= 0) {
	AliDebug(15, Form("New hardware address, was 0x%x, now 0x%x", 
			  GetPrevHWAddress(), GetHWAddress()));
	addr = GetPrevHWAddress();
	ddl  = AliFMDParameters::kBaseDDL + prevddl;
	len  = l+1;
	last = signal;
	break;
      }
    }
    prevddl  = fRawReader->GetDDLID();
    Int_t t  = GetTime();
    l        = TMath::Max(l, t);
    data[t]  = signal;
    last     = 0xFFFF;
  } while (next);
  return next;
}


//_____________________________________________________________________________
//
// EOF
//

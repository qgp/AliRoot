/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

//-------------------------------------------------------------------------
//     base class for ESD and AOD events
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include "AliVEvent.h"
const Int_t AliVEvent::NTRIGGERBITS = 56;
const AliBits AliVEvent::kMB                 = AliBits(0);
const AliBits AliVEvent::kINT7               = AliBits(1);
const AliBits AliVEvent::kMUON               = AliBits(2);
const AliBits AliVEvent::kHighMult           = AliBits(3);
const AliBits AliVEvent::kEMC1               = AliBits(4);
const AliBits AliVEvent::kCINT5              = AliBits(5);
const AliBits AliVEvent::kCMUS5              = AliBits(6);
const AliBits AliVEvent::kMUSPB              = AliBits(6);
const AliBits AliVEvent::kMUSH7              = AliBits(7);
const AliBits AliVEvent::kMUSHPB             = AliBits(7);
const AliBits AliVEvent::kMUL7               = AliBits(8);
const AliBits AliVEvent::kMuonLikePB         = AliBits(8);
const AliBits AliVEvent::kMUU7               = AliBits(9);
const AliBits AliVEvent::kMuonUnlikePB       = AliBits(9);
const AliBits AliVEvent::kEMC7               = AliBits(10);
const AliBits AliVEvent::kMUS7               = AliBits(11);
const AliBits AliVEvent::kPHI1               = AliBits(12);
const AliBits AliVEvent::kPHI7               = AliBits(13);
const AliBits AliVEvent::kPHOSPb             = AliBits(13);
const AliBits AliVEvent::kEMCEJE             = AliBits(14);
const AliBits AliVEvent::kEMCEGA             = AliBits(15);
const AliBits AliVEvent::kCentral            = AliBits(16);
const AliBits AliVEvent::kSemiCentral        = AliBits(17);
const AliBits AliVEvent::kDG5                = AliBits(18);
const AliBits AliVEvent::kZED                = AliBits(19);
const AliBits AliVEvent::kSPI7               = AliBits(20);
const AliBits AliVEvent::kSPI                = AliBits(20);
const AliBits AliVEvent::kINT8               = AliBits(21);
const AliBits AliVEvent::kMuonSingleLowPt8   = AliBits(22);
const AliBits AliVEvent::kMuonSingleHighPt8  = AliBits(23);
const AliBits AliVEvent::kMuonLikeLowPt8     = AliBits(24);
const AliBits AliVEvent::kMuonUnlikeLowPt8   = AliBits(25);
const AliBits AliVEvent::kMuonUnlikeLowPt0   = AliBits(26);
const AliBits AliVEvent::kUserDefined        = AliBits(27);
const AliBits AliVEvent::kTRD                = AliBits(28);
const AliBits AliVEvent::kUserDefined2       = AliBits(29);
const AliBits AliVEvent::kFastOnly           = AliBits(30);
const AliBits AliVEvent::kUserDefined3       = AliBits(31);
const AliBits AliVEvent::kAny                = AliBits(AliVEvent::NTRIGGERBITS,"set");
const AliBits AliVEvent::kAnyINT             = AliVEvent::kMB | AliVEvent::kINT7 | AliVEvent::kCINT5 | AliVEvent::kINT8 | AliVEvent::kSPI7;
const AliBits AliVEvent::kPHI8               = AliBits(32);
const AliBits AliVEvent::kEmcalL1GammaHigh7  = AliBits(33);
const AliBits AliVEvent::kEmcalL1GammaLow7   = AliBits(34);
const AliBits AliVEvent::kEmcalL1JetHigh7    = AliBits(35);
const AliBits AliVEvent::kEmcalL1JetLow7     = AliBits(36);
const AliBits AliVEvent::kEMC8               = AliBits(37);
const AliBits AliVEvent::kEmcalL1GammaHigh8  = AliBits(38);
const AliBits AliVEvent::kEmcalL1GammaLow8   = AliBits(39);
const AliBits AliVEvent::kEmcalL1JetHigh8    = AliBits(40);
const AliBits AliVEvent::kEmcalL1JetLow8     = AliBits(41);
const AliBits AliVEvent::kINT7HJT            = AliBits(42);
const AliBits AliVEvent::kINT7HSE            = AliBits(43);
const AliBits AliVEvent::kINT7HQU            = AliBits(44);
const AliBits AliVEvent::kEMC7HQU            = AliBits(45);
const AliBits AliVEvent::kEMC7HEE            = AliBits(46);
const AliBits AliVEvent::kINT8HJT            = AliBits(47);
const AliBits AliVEvent::kINT8HSE            = AliBits(48);
const AliBits AliVEvent::kINT8HQU            = AliBits(49);
const AliBits AliVEvent::kEMC8HQU            = AliBits(50);
const AliBits AliVEvent::kEMC8HEE            = AliBits(51);
const AliBits AliVEvent::kSPI8               = AliBits(52);
const AliBits AliVEvent::kSTP                = AliBits(53);
const AliBits AliVEvent::kOMU                = AliBits(54);
const AliBits AliVEvent::kCUP7               = AliBits(55);

AliVEvent::AliVEvent(const AliVEvent& vEvnt) :
  TObject(vEvnt)  { } // Copy constructor

AliVEvent& AliVEvent::operator=(const AliVEvent& vEvnt)
{ if (this!=&vEvnt) { 
    TObject::operator=(vEvnt); 
  }
  
  return *this; 
}

const char* AliVEvent::Whoami()
{
  switch (GetDataLayoutType())
  {
    case AliVEvent::kESD :
      return "ESD";
    case AliVEvent::kFlat :
      return "Flat";
    case AliVEvent::kAOD :
      return "AOD";
    case AliVEvent::kMC :
      return "MC";
    case AliVEvent::kMixed :
      return "Mixed";
    default:
      return "unknown";
  }
}

ClassImp(AliVEvent)

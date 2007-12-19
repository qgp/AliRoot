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
Revision 1.5  2007/01/19 13:36:10  jgrosseo
added Print method, fixed missing virtual

Revision 1.4  2006/09/04 17:42:34  hristov
Changes required by Effective C++

Revision 1.3  2006/07/20 09:43:46  jgrosseo
removing dynamic types

Revision 1.2  2006/07/04 14:58:11  jgrosseo
revision of AliDCSValue: Removed wrapper classes, reduced storage size per value by factor 2

Revision 1.1  2006/06/02 14:14:36  hristov
Separate library for CDB (Jan)

Revision 1.2  2006/03/07 07:52:34  hristov
New version (B.Yordanov)

Revision 1.2  2005/11/17 14:43:23  byordano
import to local CVS

Revision 1.1.1.1  2005/10/28 07:33:58  hristov
Initial import as subdirectory in AliRoot

Revision 1.1.1.1  2005/09/12 22:11:40  byordano
SHUTTLE package

Revision 1.2  2005/08/30 10:53:23  byordano
some more descriptions added

*/

//
// This class represents the value(s) of
// a DCS data point at a given timestamp.
// It stores different data types, the current implementation has a member for all of them.
// This is definitly not efficient, but the only way to use the automatic streamers generated by ROOT.
//
// Each element of this value series has two fields:
// fValue - primitive value which represents the real measured value
// fTimestamp - timestamp when the measurement was made
//

#include "AliDCSValue.h"

#include "TTimeStamp.h"
#include <TString.h>

ClassImp(AliDCSValue)

AliDCSValue::AliDCSValue() :
  TObject(),
  fType(kInvalid),
  fBool(kFALSE),
  fChar(0),
  fInt(0),
  fUInt(0),
  fFloat(0),
  fTimeStamp(0)
{
  // default constructor
}

AliDCSValue::AliDCSValue(Bool_t value, UInt_t timeStamp) : 
  TObject(),
  fType(kBool),
  fBool(value),
  fChar(0),
  fInt(0),
  fUInt(0),
  fFloat(0),
  fTimeStamp(timeStamp)
{
  // constructor
}

AliDCSValue::AliDCSValue(Char_t value, UInt_t timeStamp) :
  TObject(),
  fType(kChar),
  fBool(kFALSE),
  fChar(value),
  fInt(0),
  fUInt(0),
  fFloat(0),
  fTimeStamp(timeStamp)
{
  // constructor
}

AliDCSValue::AliDCSValue(Int_t value, UInt_t timeStamp) :
  TObject(),
  fType(kInt),
  fBool(kFALSE),
  fChar(0),
  fInt(value),
  fUInt(0),
  fFloat(0),
  fTimeStamp(timeStamp)
{
  // constructor
}

AliDCSValue::AliDCSValue(UInt_t value, UInt_t timeStamp) :
  TObject(),
  fType(kUInt),
  fBool(kFALSE),
  fChar(0),
  fInt(0),
  fUInt(value),
  fFloat(0),
  fTimeStamp(timeStamp)
{
  // constructor
}

AliDCSValue::AliDCSValue(Float_t value, UInt_t timeStamp) :
  TObject(),
  fType(kFloat),
  fBool(kFALSE),
  fChar(0),
  fInt(0),
  fUInt(0),
  fFloat(value),
  fTimeStamp(timeStamp)
{
  // constructor

  Init();

  fTimeStamp = timeStamp;

  fType = kFloat;
  fFloat = value;
}

AliDCSValue::AliDCSValue(const AliDCSValue& c) :
  TObject(c),
  fType(c.fType),
  fBool(c.fBool),
  fChar(c.fChar),
  fInt(c.fInt),
  fUInt(c.fUInt),
  fFloat(c.fFloat),
  fTimeStamp(c.fTimeStamp)
{
  // copy constructor
}

void AliDCSValue::Init()
{
  // init helper, that initializes everything to 0

  fType = kInvalid;

  fBool = kFALSE;
  fChar = 0;
  fInt = 0;
  fUInt = 0;
  fFloat = 0;

  fTimeStamp = 0;
}

AliDCSValue::~AliDCSValue()
{
  // destructor
}

AliDCSValue &AliDCSValue::operator=(const AliDCSValue &c)
{
  // assigment operator

  if (this != &c) 
    ((AliDCSValue &) c).Copy(*this);

  return *this;
}

void AliDCSValue::Copy(TObject& c) const
{
  // copy function

  AliDCSValue& target = (AliDCSValue &) c;

  target.Init();

  target.fType = fType;

  target.fBool = fBool;
  target.fChar = fChar;
  target.fInt = fInt;
  target.fUInt = fUInt;
  target.fFloat = fFloat;

  target.fTimeStamp = fTimeStamp;
}

Int_t AliDCSValue::GetSize() const
{
  // returns size in bytes of stored structure

  Int_t size = sizeof(fTimeStamp);

  switch (fType)
  {
    case kBool:  size += sizeof(Bool_t);  break;
    case kChar:  size += sizeof(Char_t);  break;
    case kInt:   size += sizeof(Int_t);   break;
    case kUInt:  size += sizeof(UInt_t);  break;
    case kFloat: size += sizeof(Float_t); break;

    case kInvalid: break;
  }

  return size;
}

const Char_t* AliDCSValue::ToString() const
{
  TString str;

  switch (fType)
  {
    case kBool:  str = (fBool == kFALSE) ? "FALSE" : "TRUE"; break;
    case kChar:  str.Form("%d", fChar);  break;
    case kInt:   str.Form("%d", fInt);  break;
    case kUInt:  str.Form("%d", fUInt);  break;
    case kFloat: str.Form("%f", fFloat);  break;

    case kInvalid: break;
  }

  return Form("%s Timestamp: %s", str.Data(), TTimeStamp((time_t) fTimeStamp, 0).AsString());
}

void AliDCSValue::Print(Option_t* /*opt*/) const
{
  printf("%s\n", ToString());
}

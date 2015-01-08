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
#include "AliBits.h"
#include "AliLog.h"
#include "TMath.h"
#include "TString.h"


AliBits::AliBits() : TObject(),fBits() {}


AliBits::AliBits(UInt_t nBits, TString option): TObject(),fBits(nBits) {
  if (option.EqualTo("set")) for (UInt_t i=0;i<nBits;i++) fBits.SetBitNumber(i);
}


AliBits::AliBits(UInt_t bitNumber)
: TObject(),fBits() {
  fBits.SetBitNumber(bitNumber);
}


AliBits::~AliBits() {}


void AliBits::Print(Option_t* option) const {
  fBits.Print(option);
}


void AliBits::Set(UInt_t nbits, UInt_t* array) {
  fBits.Set(nbits,array);
}


UInt_t AliBits::GetActiveBit() const{
  if (fBits.CountBits()>1)  { AliWarning("More than one active bit found"); }
  return fBits.FirstSetBit();
  
}


TString AliBits::GetBitString() const {
  TString s('0',fBits.GetNbits());
  for (UInt_t i=0;i<fBits.GetNbits();i++) if (fBits.TestBitNumber(i)) s[i]='1';
  return s;
}


AliBits& AliBits::operator|=(const AliBits& rhs) {
  const UInt_t nBits(TMath::Max(fBits.GetNbits(), rhs.fBits.GetNbits()));
  fBits.SetBitNumber(nBits, kFALSE);
  fBits |= rhs.fBits;
  return *this;
}


AliBits& AliBits::operator&=(const AliBits& rhs) {
  const UInt_t nBits(TMath::Max(fBits.GetNbits(), rhs.fBits.GetNbits()));
  fBits.SetBitNumber(nBits, kFALSE);
  fBits &= rhs.fBits;
  return *this;
}


AliBits& AliBits::operator^=(const AliBits& rhs) {
  const UInt_t nBits(TMath::Max(fBits.GetNbits(), rhs.fBits.GetNbits()));
  fBits.SetBitNumber(nBits, kFALSE);
  fBits ^= rhs.fBits;
  return *this;
}


Bool_t AliBits::operator==(const AliBits& rhs){
  return fBits == rhs.fBits;
}


Bool_t AliBits::operator!=(const AliBits& rhs){
  return fBits != rhs.fBits;
}

ClassImp(AliBits)

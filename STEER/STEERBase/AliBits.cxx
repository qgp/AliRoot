#include "AliBits.h"
#include "AliLog.h"
#include "TMath.h"
#include "TString.h"
AliBits::AliBits() : TObject(),fBits() {}

AliBits::AliBits(UInt_t nBits, TString option): TObject(),fBits() {
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

void AliBits::Set(UInt_t nbits, UInt_t a) {
  fBits.Set(nbits,&a);
}

UInt_t AliBits::GetActiveBit() const{
  if (fBits.CountBits()>1)  { AliWarning("More than one active bit found"); }
  return fBits.FirstSetBit();
  
}

AliBits& AliBits::operator|=(const AliBits& psb) {
  fBits |= psb.fBits;
  return *this;
}

AliBits& AliBits::operator&=(const AliBits& psb) {
  fBits &= psb.fBits;
  return *this;
}

AliBits& AliBits::operator^=(const AliBits& psb) {
  fBits ^= psb.fBits;
  return *this;
}

Bool_t AliBits::operator==(const AliBits& psb){
  return fBits == psb.fBits;
}

Bool_t AliBits::operator!=(const AliBits& psb){
  return fBits != psb.fBits;
}

ClassImp(AliBits)

#ifndef AliBits_h
#define AliBits_h 1
#include "TObject.h"
#include "TBits.h"

class AliBits : public TObject {
public:
  AliBits();
  AliBits(UInt_t nBits, TString option);
  explicit AliBits(UInt_t bitNumber);
  virtual ~AliBits();
  virtual void Print(Option_t* option = "") const;
  void Set(UInt_t nbits,UInt_t a);
  UInt_t GetActiveBit() const;
  AliBits& operator|=(const AliBits& psb);
  AliBits& operator&=(const AliBits& psb);
  AliBits& operator^=(const AliBits& psb);
  Bool_t operator==(const AliBits& psb);
  Bool_t operator!=(const AliBits& psb);
  operator bool() const { return fBits.CountBits() > 0; };
public:
  TBits fBits;
  ClassDef(AliBits,1)
};

inline AliBits operator|(const AliBits& lhs, const AliBits& rhs) {
  AliBits result(lhs);
  return (result |= rhs);
}

inline AliBits operator&(const AliBits& lhs, const AliBits& rhs) {
  AliBits result(lhs);
  return (result &= rhs);
}

inline AliBits operator^(const AliBits& lhs, const AliBits& rhs) {
  AliBits result(lhs);
  return (result ^= rhs);
}

#endif

#ifndef AliBits_h
#define AliBits_h 1
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* 
 * AliBits class is an extended version of TBits allowing to store infinite 
 * number of bits (e.g. physics selection decisions) and perform the same bit 
 * operations as with 32-bit integers. 
 * 
 * Examples of valid syntax:
 * AliBits bitsUsedInUserAnalysis = AliVEvent::kCentral | AliVEvent::kSemiCentral;
 * AliBits physicsSelectionDecisions = fInputHandler->IsEventSelected();
 * AliBits selectedBits = physicsSelectionDecisions & bitsUsedInUserAnalysis;
 * printf("%s\n",selectedBits.GetBitString().Data());
 * if (!selectedBits) { return; } // skip event
 */ 


#include "TObject.h"
#include "TBits.h"

class AliBits : public TObject {
public:
  
  // Default constructor 
  // Creates AliBits object without active bits
  AliBits();
  
  // Explicit constructor
  // Creates AliBits object with bit bitNumber set to 1
  explicit AliBits(UInt_t bitNumber);
  
  // Constructor
  // Creates AliBits object with nBits size.
  // if option="set", sets all bits to 1
  AliBits(UInt_t nBits, TString option);
  
  // Destructor
  virtual ~AliBits();
  
  // Read nbits from array, see TBits::Set()
  void Set(UInt_t nbits,UInt_t* array);
  
  // Prints the list active bits, see TBits::Print()
  virtual void Print(Option_t* option = "") const;
  
  // Returns position of the 1st active bit
  UInt_t GetActiveBit() const;
  
  // Converts AliBits to TString of 0s and 1s
  TString GetBitString() const;
  
  // operators
  AliBits& operator|=(const AliBits& rhs);
  AliBits& operator&=(const AliBits& rhs);
  AliBits& operator^=(const AliBits& rhs);
  Bool_t operator==(const AliBits& rhs);
  Bool_t operator!=(const AliBits& rhs);
  
  // bool operator for implicit conversion of AliBits to bool
  operator bool() const { return fBits.CountBits()>0; }
protected:
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

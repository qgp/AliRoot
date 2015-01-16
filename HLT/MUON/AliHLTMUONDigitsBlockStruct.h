#ifndef ALIHLTMUONDIGITSBLOCKSTRUCT_H
#define ALIHLTMUONDIGITSBLOCKSTRUCT_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

///
/// @file   AliHLTMUONDigitsBlockStruct.h
/// @author Philippe Pillot <pillot@subatech.in2p3.fr>
/// @date   16 December 2014
/// @brief  Definition of internal dHLT digits block structure.
///
/// The structures are defined with C linkage since C generally gives us more
/// binary compatibility between compilers.
///

#include "AliHLTMUONDataTypes.h"
#include <ostream>

extern "C"
{
  
  /**
   * Gives the fired digit/pad information.
   */
  struct AliHLTMUONDigitStruct
  {
    AliHLTUInt32_t fId;    // Digit ID in the current mapping (from OCDB)
    AliHLTUInt16_t fIndex; // Digit index in the new mapping (produced internally)
    AliHLTUInt16_t fADC;   // ADC value of signal
  };
  
  /**
   * AliHLTMUONDigitBlockStruct defines the format of the internal digit data block.
   */
  struct AliHLTMUONDigitsBlockStruct
  {
    AliHLTMUONDataBlockHeader fHeader; // Common data block header
    
    // Array of digits/pads.
    //AliHLTMUONDigitStruct fDigit[/*fHeader.fNrecords*/];
  };
  
} // extern "C"


/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONDigitStruct in the following format:
 *   {fId = xx, fIndex = yy, fADC = zz}
 */
std::ostream& operator << (
                           std::ostream& stream, const AliHLTMUONDigitStruct& digit
                           );

/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONDigitsBlockStruct in the following format:
 *   {fHeader = xx, fDigit[] = [{..}, {..}, ...]}
 */
std::ostream& operator << (
                           std::ostream& stream, const AliHLTMUONDigitsBlockStruct& block
                           );


inline bool operator == (
                         const AliHLTMUONDigitStruct& a,
                         const AliHLTMUONDigitStruct& b
                         )
{
  return a.fId == b.fId and a.fIndex == b.fIndex and a.fADC == b.fADC;
}

inline bool operator != (
                         const AliHLTMUONDigitStruct& a,
                         const AliHLTMUONDigitStruct& b
                         )
{
  return not operator == (a, b);
}


bool operator == (
                  const AliHLTMUONDigitsBlockStruct& a,
                  const AliHLTMUONDigitsBlockStruct& b
                  );

inline bool operator != (
                         const AliHLTMUONDigitsBlockStruct& a,
                         const AliHLTMUONDigitsBlockStruct& b
                         )
{
  return not operator == (a, b);
}

#endif // ALIHLTMUONDIGITSBLOCKSTRUCT_H

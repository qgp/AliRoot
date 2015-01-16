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

// $Id$

///
/// @file   AliHLTMUONDigitsBlockStruct.cxx
/// @author Philippe Pillot <pillot@subatech.in2p3.fr>
/// @date   16 December 2014
/// @brief  Implementation of the stream and comparison operators.
///
/// The digits blocks are internal dimuon HLT block structures
/// containing information about reconstructed digits.
///

#include "AliHLTMUONDigitsBlockStruct.h"
#include "AliHLTMUONUtils.h"
#include <cassert>

std::ostream& operator << (
                           std::ostream& stream, const AliHLTMUONDigitStruct& digit
                           )
{
  std::ios::fmtflags oldflags = stream.flags();
  stream	<< "{fId = " << digit.fId
		<< ", fIndex = " << digit.fIndex
		<< ", fADC = " << digit.fADC << "}";
  stream.flags(oldflags);
  return stream;
}


std::ostream& operator << (
                           std::ostream& stream, const AliHLTMUONDigitsBlockStruct& block
                           )
{
  assert( AliHLTMUONUtils::IntegrityOk(block) );
  
  const AliHLTMUONDigitStruct* digit =
		reinterpret_cast<const AliHLTMUONDigitStruct*>(&block + 1);
  stream 	<< "{fHeader = " << block.fHeader << ", fDigit[] = [";
  if (block.fHeader.fNrecords > 0) stream << digit[0];
  for (AliHLTUInt32_t i = 1; i < block.fHeader.fNrecords; i++)
    stream << ", " << digit[i];
  stream << "]}";
  return stream;
}


bool operator == (
                  const AliHLTMUONDigitsBlockStruct& a,
                  const AliHLTMUONDigitsBlockStruct& b
                  )
{
  assert( AliHLTMUONUtils::IntegrityOk(a) );
  assert( AliHLTMUONUtils::IntegrityOk(b) );
  
  const AliHLTMUONDigitStruct* digitA =
		reinterpret_cast<const AliHLTMUONDigitStruct*>(&a + 1);
  const AliHLTMUONDigitStruct* digitB =
		reinterpret_cast<const AliHLTMUONDigitStruct*>(&b + 1);
  
  // First check if the blocks have the same header. If they do then check
  // if every digit is the same. In either case if we find a difference
  // return false.
  if (a.fHeader != b.fHeader) return false;
  for (AliHLTUInt32_t i = 0; i < a.fHeader.fNrecords; i++)
    if (digitA[i] != digitB[i]) return false;
  return true;
}

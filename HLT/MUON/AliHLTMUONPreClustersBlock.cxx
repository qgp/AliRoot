/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Philippe Pillot <pillot@subatech.in2p3.fr>                           *
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

/**
 * @file   AliHLTMUONPreClustersBlock.cxx
 * @author Philippe Pillot <pillot@subatech.in2p3.fr>
 * @date   01 December 2014
 * @brief  Implementation of the class to read/write a data block of preclusters.
 */

#include <cassert>
#include "AliHLTMUONPreClustersBlock.h"

//_________________________________________________________________________________________________
AliHLTMUONPreClustersBlock::AliHLTMUONPreClustersBlock() :
AliHLTLogging(),
fSize(0),
fWriteMode(false),
fSize4PreCl(0),
fSize4Digit(0),
fCurrentSize(1),
fNPreClusters(NULL),
fLastNDigits(NULL),
fLastDigit(NULL),
fPreClusters(NULL)
{
  // default constructor
}

//_________________________________________________________________________________________________
AliHLTMUONPreClustersBlock::AliHLTMUONPreClustersBlock(void *buffer, AliHLTUInt32_t size, bool write) :
AliHLTLogging(),
fSize(size),
fWriteMode(write),
fSize4PreCl((size > fgkSizeOfUShort+fgkSizeOfDigit) ? size - fgkSizeOfUShort - fgkSizeOfDigit : 0),
fSize4Digit((size > fgkSizeOfDigit) ? size - fgkSizeOfDigit : 0),
fCurrentSize(size+1),
fNPreClusters(NULL),
fLastNDigits(NULL),
fLastDigit(NULL),
fPreClusters(NULL)
{
  // constructor

  assert(buffer != NULL);

  if (size >= fgkSizeOfUShort) {
    fCurrentSize = fgkSizeOfUShort;
    fNPreClusters = reinterpret_cast<AliHLTUInt16_t*>(buffer);
    if (fWriteMode) *fNPreClusters = 0;
    else ReadBuffer();
  } else if (fWriteMode) {
    HLTError("The buffer is too small to store the data block.");
  } else {
    HLTError("The buffer is too small to contain the data block.");
  }

}

//_________________________________________________________________________________________________
AliHLTMUONPreClustersBlock::~AliHLTMUONPreClustersBlock()
{
  // destructor

  delete fPreClusters;
  fPreClusters = NULL;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::Reset(void *buffer, AliHLTUInt32_t size, bool write)
{
  /// initialize the internal structure in write mode

  assert(buffer != NULL);

  int status = 0;

  fSize = size;
  fWriteMode = write;
  fSize4PreCl = (size > fgkSizeOfUShort+fgkSizeOfDigit) ? size - fgkSizeOfUShort - fgkSizeOfDigit : 0;
  fSize4Digit = (size > fgkSizeOfDigit) ? size - fgkSizeOfDigit : 0;
  fLastNDigits = NULL;
  fLastDigit = NULL;
  if (fPreClusters) fPreClusters->clear();

  if (size >= fgkSizeOfUShort) {
    fCurrentSize = fgkSizeOfUShort;
    fNPreClusters = reinterpret_cast<AliHLTUInt16_t*>(buffer);
    if (fWriteMode) *fNPreClusters = 0;
    else status = ReadBuffer();
  } else {
    fCurrentSize = fSize+1;
    fNPreClusters = NULL;
    if (fWriteMode) {
      HLTError("The buffer is too small to store the data block.");
      status = -ENOBUFS;
    } else {
      HLTError("The buffer is too small to contain the data block.");
      status = -EILSEQ;
    }
  }

  return status;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::StartPreCluster(const AliHLTMUONDigitStruct &digit)
{
  // start a new precluster

  assert(fWriteMode);

  if (fCurrentSize > fSize4PreCl) {
    HLTError("The buffer is too small to store a new precluster.");
    fLastNDigits = NULL;
    fLastDigit = NULL;
    return -ENOBUFS;
  }

  if (fLastNDigits == NULL) fLastNDigits = fNPreClusters + 1;
  else fLastNDigits = reinterpret_cast<AliHLTUInt16_t*>(fLastDigit + 1);
  *fLastNDigits = 1;

  fLastDigit = reinterpret_cast<AliHLTMUONDigitStruct*>(fLastNDigits + 1);
  *fLastDigit = digit;

  *fNPreClusters += 1;
  fCurrentSize += fgkSizeOfUShort + fgkSizeOfDigit;

  if (fPreClusters && (fPreClusters->size() > 0 || *fNPreClusters == 1)) {
    AliHLTMUONPreClusterStruct preCluster = {*fLastNDigits, fLastDigit};
    fPreClusters->push_back(preCluster);
  }

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::AddDigit(const AliHLTMUONDigitStruct &digit)
{
  // add a new digit to the current precluster

  assert(fWriteMode);

  if (fCurrentSize > fSize4Digit) {
    HLTError("The buffer is too small to store a new digit.");
    fLastDigit = NULL;
    return -ENOBUFS;
  }

  if (fLastNDigits == NULL || fLastDigit == NULL) {
    HLTError("No precluster to attach the new digit to.");
    return -EPERM;
  }

  *fLastNDigits += 1;

  fLastDigit += 1;
  *fLastDigit = digit;

  fCurrentSize += fgkSizeOfDigit;

  if (fPreClusters && fPreClusters->size() > 0) fPreClusters->back().fNDigits++;

  return 0;

}

//_________________________________________________________________________________________________
const std::vector<AliHLTMUONPreClusterStruct>& AliHLTMUONPreClustersBlock::GetPreClusters()
{
  // return the vector of preclusters currently stored in the data block

  if (!fPreClusters) ReadBuffer();

  return *fPreClusters;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::ReadBuffer()
{
  /// read the buffer to fill the internal structure
  /// fNPreclus,fNdig,dig1, dig2 ... dign, fNdig, dig1 ... digN, ... last_precluster
  if (!fPreClusters) {
    fPreClusters = new std::vector<AliHLTMUONPreClusterStruct>;
    fPreClusters->reserve(100);
  }

  if (fNPreClusters == NULL) return -EILSEQ;

  int status = 0;
  AliHLTMUONPreClusterStruct preCluster;
  AliHLTUInt32_t currentSize(fgkSizeOfUShort);
  AliHLTUInt16_t *nDigits(0x0);
  AliHLTMUONDigitStruct *digit(0x0);

  for (AliHLTUInt16_t i = 0; i < *fNPreClusters; ++i) {

    currentSize += fgkSizeOfUShort;
    if (currentSize < fSize) {
      if (nDigits == NULL) nDigits = fNPreClusters + 1;
      else nDigits = reinterpret_cast<AliHLTUInt16_t*>(digit + 1);
    } else {
      HLTError("Cannot read the expected number of preclusters.");
      status = -EILSEQ;
      break;
    }

    currentSize += (*nDigits) * fgkSizeOfDigit;
    if (*nDigits > 0 && currentSize <= fSize) {
      digit = reinterpret_cast<AliHLTMUONDigitStruct*>(nDigits + 1);
      preCluster.fNDigits = *nDigits;
      preCluster.fDigits = digit;
      fPreClusters->push_back(preCluster);
      digit += (*nDigits) - 1;
    } else {
      if (*nDigits == 0) {
        HLTError("The precluster cannot contain 0 digit.");
      } else {
        HLTError("Cannot read the expected number of digits.");
      }
      status = -EILSEQ;
      break;
    }

  }

  if (fWriteMode) {

    if (currentSize != fCurrentSize && status >= 0) {
      HLTError("Different number of bytes read than currently written!?");
      status = -EILSEQ;
    }

    if (status < 0) fPreClusters->clear();

  } else {

    if (currentSize != fSize && status >= 0) {
      HLTError("The number of bytes read differs from the buffer size");
      status = -EILSEQ;
    }

    if (status < 0) {
      fCurrentSize = fSize+1;
      fNPreClusters = NULL;
      fPreClusters->clear();
    } else fCurrentSize = currentSize;

  }

  return status;

}


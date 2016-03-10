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
    fNPreClusters(0x0),
    fLastNDigits(0x0),
    fLastDigit(0x0)
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
  fNPreClusters(0x0),
  fLastNDigits(0x0),
  fLastDigit(0x0)
{

  /// constructor
  // initialization is the same as in the "reset" method
  Reset( buffer, size, write );

}

//_________________________________________________________________________________________________
AliHLTMUONPreClustersBlock::~AliHLTMUONPreClustersBlock()
{}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::Reset(void *buffer, AliHLTUInt32_t size, bool write)
{

  /// initialize the internal structure in write mode
  assert(buffer != 0x0);

  int status = 0;

  // store buffer
  fBuffer = buffer;

  // store buffer size
  fSize = size;

  // store write mode
  fWriteMode = write;

  // reset
  fSize4PreCl = (size > fgkSizeOfUShort+fgkSizeOfDigit) ? size - fgkSizeOfUShort - fgkSizeOfDigit : 0;
  fSize4Digit = (size > fgkSizeOfDigit) ? size - fgkSizeOfDigit : 0;
  fLastNDigits = 0x0;
  fLastDigit = 0x0;
  fPreClusters.clear();

  if (size >= fgkSizeOfUShort) {

    // store number of precluster
    fNPreClusters = reinterpret_cast<AliHLTUInt16_t*>(fBuffer);

    // increment buffer and size read
    // reinterpret_cast<AliHLTUInt16_t*>(fBuffer) += 1;
    fBuffer = (reinterpret_cast<AliHLTUInt16_t*>(fBuffer) + 1);
    fCurrentSize = sizeof(AliHLTUInt16_t);

    if (fWriteMode)
    {

      // assign 0 clusters in write mode
      *fNPreClusters = 0;

    } else {

      // read
      status = ReadBuffer();

    }

  } else {

    fCurrentSize = fSize+1;
    fNPreClusters = 0x0;

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

  if (fCurrentSize > fSize4PreCl)
  {
    HLTError("The buffer is too small to store a new precluster.");
    fLastNDigits = 0x0;
    fLastDigit = 0x0;
    return -ENOBUFS;
  }

  // use current buffer position to store number of digits
  // and increment buffer
  fLastNDigits = reinterpret_cast<AliHLTUInt16_t*>( fBuffer );
  // reinterpret_cast<AliHLTUInt16_t*>( fBuffer ) += 1;
  fBuffer = (reinterpret_cast<AliHLTUInt16_t*>( fBuffer ) + 1);
  fCurrentSize += sizeof(AliHLTUInt16_t);

  // assign
  *fLastNDigits = 1;

  // store digit and increment buffer
  fLastDigit = reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer );
  // reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) += 1;
  fBuffer = (reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) + 1);
  fCurrentSize += sizeof(AliHLTMUONDigitStruct);

  // increment number of pre-clusters
  (*fNPreClusters) += 1;

  // insert in list
  AliHLTMUONPreClusterStruct preCluster = {*fLastNDigits, fLastDigit};
  fPreClusters.push_back(preCluster);

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::AddDigit(const AliHLTMUONDigitStruct &digit)
{

  /// add a new digit to the current precluster

  assert(fWriteMode);

  if (fCurrentSize > fSize4Digit)
  {
    HLTError("The buffer is too small to store a new digit.");
    fLastDigit = 0x0;
    return -ENOBUFS;
  }

  if (fLastNDigits == 0x0 || fLastDigit == 0x0)
  {
    HLTError("No precluster to attach the new digit to.");
    return -EPERM;
  }

  // increment number of digits
  *fLastNDigits += 1;

  // assign digit to the buffer and increment buffer
  *(reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer )) = digit;
  // reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer )+=1;
  fBuffer = (reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) + 1);
  fCurrentSize += sizeof(AliHLTMUONDigitStruct);

  // increment number of digits in the stored cluster
  if( !fPreClusters.empty() ) fPreClusters.back().fNDigits++;

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::ReadBuffer()
{

  /// read the buffer to fill the internal structure
  /// fNPreclus,fNdig,dig1, dig2 ... dign, fNdig, dig1 ... digN, ... last_precluster
  assert( !fWriteMode );

  // make sure fNPreClusters was assigned
  if (fNPreClusters == 0x0) return -EILSEQ;

  int status = 0;

  // rolling pre-cluster
  AliHLTMUONPreClusterStruct preCluster;

  // loop over
  for (AliHLTUInt16_t i = 0; i < *fNPreClusters; ++i)
  {

    // store number of digits from buffer and increment
    AliHLTUInt16_t* nDigits = 0x0;
    if( fCurrentSize < fSize )
    {

      nDigits = reinterpret_cast<AliHLTUInt16_t*>(fBuffer);
      // reinterpret_cast<AliHLTUInt16_t*>(fBuffer)+=1;
      fBuffer = (reinterpret_cast<AliHLTUInt16_t*>(fBuffer) + 1);
      fCurrentSize += sizeof( AliHLTUInt16_t );

    } else {

      HLTError("Cannot read the expected number of preclusters.");
      status = -EILSEQ;
      break;

    }

    // read the digits
    if( nDigits && (*nDigits) > 0 && fCurrentSize + (*nDigits) * sizeof( AliHLTMUONDigitStruct ) <= fSize )
    {

      AliHLTMUONDigitStruct* digit = reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer );
      // reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) += (*nDigits);
      fBuffer = (reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) + (*nDigits));
      fCurrentSize += (*nDigits) * sizeof( AliHLTMUONDigitStruct );

      // store
      preCluster.fNDigits = *nDigits;
      preCluster.fDigits = digit;
      fPreClusters.push_back(preCluster);

    } else {

      if (*nDigits == 0) HLTError("The precluster cannot contain 0 digit.");
      else HLTError("Cannot read the expected number of digits.");

      status = -EILSEQ;
      break;

    }

  }

  return status;

}

//_________________________________________________________________
std::ostream& operator << ( std::ostream& stream, const AliHLTMUONPreClusterStruct& cluster )
{
  stream << "{fNDigits= " << cluster.fNDigits;
  for( AliHLTUInt16_t i = 0; i < cluster.fNDigits; ++i )
  { stream << ", digit[i]= " << cluster.fDigits[i]; }
  stream << "}";

  return stream;
}

//_________________________________________________________________
std::ostream& operator << ( std::ostream& stream, const AliHLTMUONPreClustersBlock& clustersBlock )
{

  stream << "{fNClusters= " << clustersBlock.GetNPreClusters() << endl;
  const std::vector<AliHLTMUONPreClusterStruct>& clusters( clustersBlock.GetPreClusters() );
  for( std::vector<AliHLTMUONPreClusterStruct>::const_iterator iter = clusters.begin(); iter != clusters.end(); ++iter )
  { stream << "  cluster[i]= " << *iter << std::endl; }
  stream << "}";

  return stream;
}

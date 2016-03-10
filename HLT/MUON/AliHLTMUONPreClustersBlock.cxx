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
    fCurrentSize(0),
    fNPreClusters(0x0),
    fLastNDigits(0x0)
{
  // default constructor
}

//_________________________________________________________________________________________________
AliHLTMUONPreClustersBlock::AliHLTMUONPreClustersBlock(void *buffer, AliHLTUInt32_t size, bool write) :
  AliHLTLogging(),
  fSize(size),
  fWriteMode(write),
  fCurrentSize(0),
  fSize4PreCl(0),
  fSize4Digit(0),
  fNPreClusters(0x0),
  fLastNDigits(0x0)
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
  const AliHLTUInt32_t minSizeOfCluster = fgkSizeOfUShort + fgkSizeOfDigit;
  fSize4PreCl = ( size > minSizeOfCluster ) ? size - minSizeOfCluster : 0;
  fSize4Digit = ( size > fgkSizeOfDigit ) ? size - fgkSizeOfDigit : 0;
  fLastNDigits = 0x0;
  fPreClusters.clear();

  if( size >= fgkSizeOfUShort )
  {

    // store number of precluster and increment buffer
    fNPreClusters = reinterpret_cast<AliHLTUInt16_t*>(fBuffer);
    fBuffer = (reinterpret_cast<AliHLTUInt16_t*>(fBuffer) + 1);
    fCurrentSize = fgkSizeOfUShort;

    // assign 0 clusters in write mode
    if (fWriteMode)
    {

      *fNPreClusters = 0;

    } else {

      // read buffer otherwise
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

  // could move back to constructor (or reset)
  if( fCurrentSize > fSize4PreCl )
  {
    HLTError("The buffer is too small to store a new precluster.");
    fLastNDigits = 0x0;
    return -ENOBUFS;
  }

  // use current buffer position to store number of digits
  // and increment buffer
  fLastNDigits = reinterpret_cast<AliHLTUInt16_t*>( fBuffer );
  fBuffer = (reinterpret_cast<AliHLTUInt16_t*>( fBuffer ) + 1);
  fCurrentSize += fgkSizeOfUShort;

  // assign
  *fLastNDigits = 1;

  // store digit and increment buffer
  AliHLTMUONDigitStruct* lastDigit = reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer );
  *lastDigit = digit;
  fBuffer = (reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) + 1);
  fCurrentSize += fgkSizeOfDigit;

  // increment number of pre-clusters
  (*fNPreClusters) += 1;

  // insert in list
  AliHLTMUONPreClusterStruct preCluster = {*fLastNDigits, lastDigit};
  fPreClusters.push_back(preCluster);

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONPreClustersBlock::AddDigit(const AliHLTMUONDigitStruct &digit)
{

  /// add a new digit to the current precluster

  assert(fWriteMode);
  if( fCurrentSize > fSize4Digit )
  {
    HLTError("The buffer is too small to store a new digit.");
    return -ENOBUFS;
  }

  if( !fLastNDigits )
  {
    HLTError("No precluster to attach the new digit to.");
    return -EPERM;
  }

  // increment number of digits
  *fLastNDigits += 1;

  // assign digit to the buffer and increment buffer
  *(reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer )) = digit;
  fBuffer = (reinterpret_cast<AliHLTMUONDigitStruct*>( fBuffer ) + 1);
  fCurrentSize += fgkSizeOfDigit;

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

  // initialize status
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

  // sanity check on read size
  if( fCurrentSize != fSize && status >= 0 )
  {
    HLTError("The number of bytes read differs from the buffer size");
    status = -EILSEQ;
  }

  // reset in case of negative status
  if( status < 0 )
  {
    fCurrentSize = fSize+1;
    fNPreClusters = 0x0;
    fPreClusters.clear();
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

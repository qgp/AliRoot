#ifndef ALIHLTMUONPRECLUSTERSBLOCK_H
#define ALIHLTMUONPRECLUSTERSBLOCK_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

/**
 * @file   AliHLTMUONPreClustersBlock.h
 * @author Philippe Pillot <pillot@subatech.in2p3.fr>
 * @date   01 December 2014
 * @brief  Class to read/write a data block of preclusters.
 */

#include <vector>
#include "AliHLTLogging.h"
#include "AliHLTMUONChannelsBlockStruct.h"

extern "C"
{
  /**
   * Gives the fired channel/pad information which was considered during
   * hit reconstruction and correlated into a common cluster.
   */
  struct AliHLTMUONPreClusterStruct
  {
    AliHLTUInt16_t fNDigits; // number of digits attached to this precluster
    const AliHLTMUONChannelStruct *fDigits; // pointer to the 1st element of the array of digits
  };
}

/**
 * Class to handle the writing/reading of a block of preclusters
 * to/from the provided buffer of the given size.
 */
class AliHLTMUONPreClustersBlock : public AliHLTLogging
{
public:
  
  /**
   * Default constructor that produces an object not usable yet.
   * The method Reset(...) must be called to initialize it properly.
   */
  AliHLTMUONPreClustersBlock();
  
  /**
   * Constructor that sets the internal pointer to the start of the buffer
   * space to read/write to and the total size of the buffer in bytes.
   * @param buffer  The pointer to the first byte of the memory buffer.
   * @param size    The total size of the buffer in bytes.
   * @param write   Tell wether the buffer is for writing or reading.
   */
  AliHLTMUONPreClustersBlock(void *buffer, AliHLTUInt32_t size, bool write = false);
  
  /**
   * Destructor to delete internal list of preclusters
   */
  virtual ~AliHLTMUONPreClustersBlock();
  
  /**
   * Method that resets the internal pointer to the start of the new buffer
   * space to read/write to and the total size of this buffer in bytes.
   * @param buffer  The pointer to the first byte of the memory buffer.
   * @param size    The total size of the buffer in bytes.
   * @param write   Tell wether the buffer is for writing or reading.
   */
  int Reset(void *buffer, AliHLTUInt32_t size, bool write = false);
  
  /**
   * Method to fill the buffer with a new precluster and its first digit
   * @param digit  Reference to the first digit to add to the new precluster.
   */
  int StartPreCluster(AliHLTMUONChannelStruct &digit);
  
  /**
   * Method to add a new digit to the current precluster
   * @param digit  Reference to the digit to add to the current precluster.
   */
  int AddDigit(AliHLTMUONChannelStruct &digit);
  
  /**
   * Return the number of bytes currently used for the data block in the buffer.
   */
  AliHLTUInt32_t BytesUsed() const {return (fCurrentSize > fSize) ? 0 : fCurrentSize;}
  
  /**
   * Return the number of preclusters currently stored in the data block.
   */
  AliHLTUInt16_t GetNPreClusters() const {return (fNPreClusters == NULL) ? 0 : *fNPreClusters;}
  
  /**
   * Return the vector of preclusters currently stored in the data block.
   */
  const std::vector<AliHLTMUONPreClusterStruct>& GetPreClusters();
  
  /**
   * Return the name of the class for log messages.
   */
  virtual const char* Class_Name() { return "AliHLTMUONPreClustersBlock"; }
  
private:
  
  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONPreClustersBlock(const AliHLTMUONPreClustersBlock& /*obj*/);
  /// Not implemented.
  AliHLTMUONPreClustersBlock& operator = (const AliHLTMUONPreClustersBlock& /*obj*/);
  
  int ReadBuffer();
  
  static const AliHLTUInt32_t fgkSizeOfUShort = sizeof(AliHLTUInt16_t);
  static const AliHLTUInt32_t fgkSizeOfDigit = sizeof(AliHLTMUONChannelStruct);
  
  AliHLTUInt32_t fSize; ///< total size of the buffer
  bool fWriteMode; ///< read/write mode
  AliHLTUInt32_t fSize4PreCl; ///< total buffer size minus the one resquested to add a new precluster
  AliHLTUInt32_t fSize4Digit; ///< total buffer size minus the one resquested to add a new digit
  AliHLTUInt32_t fCurrentSize; ///< size of the used part of the buffer
  AliHLTUInt16_t *fNPreClusters; ///< number of preclusters
  AliHLTUInt16_t *fLastNDigits; ///< number of digits in the last precluster (write mode)
  AliHLTMUONChannelStruct *fLastDigit; ///< current digit in the last precluster (write mode)
  std::vector<AliHLTMUONPreClusterStruct> *fPreClusters; ///< list of preclusters
  
};

#endif

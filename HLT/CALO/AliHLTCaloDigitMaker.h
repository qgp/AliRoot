//-*- Mode: C++ -*-
// $Id$


/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors: Oystein Djuvsland                                     *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#ifndef ALIHLTCALODIGITMAKER_H
#define ALIHLTCALODIGITMAKER_H

/**
 * Class makes digits from information from raw data
 *
 * @file   AliHLTCaloDigitMaker.h
 * @author Oystein Djuvsland
 * @date
 * @brief  Digit maker for Calo HLT
 */

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

//#include "AliHLTCaloBase.h"
#include "AliHLTCaloConstantsHandler.h"
#include "AliHLTCaloDigitDataStruct.h"
#include "AliHLTCaloChannelDataStruct.h"
#include "AliHLTDataTypes.h"
#include "TString.h"

/**
 * @class AliHLTCaloDigitMaker
 * Digit maker for CALO HLT. Takes input from AliHLTCaloRawAnalyzer, and 
 * outputs a block of AliHLTCaloDigitDataStruct container
 * @ingroup alihlt_calo
 */

class TH2F;
class AliHLTCaloSharedMemoryInterfacev2; // added by PTH
class AliHLTCaloChannelDataHeaderStruct;
class AliHLTCaloMapper;
class AliHLTCaloCoordinate;
class TString;

//using namespace CaloHLTConst;
//class AliHLTCaloDigitMaker : public AliHLTCaloBase



class AliHLTCaloDigitMaker : AliHLTCaloConstantsHandler
{

public:

  /** Constructor */
  AliHLTCaloDigitMaker(TString det);

  /** Destructor */
  virtual ~AliHLTCaloDigitMaker();

  /**
   * Sets the pointer to the output
   * @param digitDataPtr the output pointer
   */
  void SetDigitDataPtr(AliHLTCaloDigitDataStruct *digitDataPtr) 
  { fDigitStructPtr = digitDataPtr; }

  /**
   * Set the global high gain conversion factory 
   * @param factor is the conversion factor
   */
  void SetGlobalHighGainFactor(Float_t factor);

  /**
   * Set the global low gain conversion factory 
   * @param factor is the conversion factor
   */
  void SetGlobalLowGainFactor(Float_t factor);

  /**
   * Make the digits for one event.
   * @param channelDataHeader is the data header from the AliHLTCaloRawAnalyzer
   * @return the number of digits found
   */
  Int_t MakeDigits(AliHLTCaloChannelDataHeaderStruct* channelDataHeader, AliHLTUInt32_t availableSize);

  /**
   * Set the mask for dead channels
   * @param badChannelHGHist is a pointer to a high gain bad channel histogram
   * @param badChannelLGHist is a pointer to a low gain bad channel histogram
   * @param qCut is the cut 
   */
  void SetBadChannelMask(TH2F* badChannelHGHist, TH2F* badChannelLGHist, Float_t qCut);

  /** Reset the channel book */
  void Reset();

  /** Set the mapper */
  void SetMapper(AliHLTCaloMapper *mapper) { fMapperPtr = mapper; }

private:
  
  AliHLTCaloDigitMaker();
  
  /**
   * Add a new digit
   * @param channelData is the channel data
   * @param coordinates is the coordinates of the channel, including gain and module
   */
  void AddDigit(AliHLTCaloChannelDataStruct* channelData, AliHLTCaloCoordinate &coord);

  /**
   * Check if we already have this crystal. If so, keep the high gain as long as it 
   * is not in overflow. 
   * @param channelCoordinates is a array of coordinates for the channel.
   * @param channel is a pointer to a struct containing channel information
   * @return true if we should use the digit. 
   */
  bool UseDigit(AliHLTCaloCoordinate &coord, AliHLTCaloChannelDataStruct *channel);

  /** Pointer to shared memory interface */
  AliHLTCaloSharedMemoryInterfacev2* fShmPtr;                    //! transient

  /** Pointer to the AliHLTCaloDigitDataStruct */
  AliHLTCaloDigitDataStruct *fDigitStructPtr;                    //! transient

  /** Digit count */
  Int_t fDigitCount;                                             //COMMENT

  /** Mapper */
  AliHLTCaloMapper* fMapperPtr;                                  //COMMENT

  /** High gain energy conversion factors */
  Float_t **fHighGainFactors;                                    //! transient

  /** Low gain energy conversion factors */
  Float_t **fLowGainFactors;                                     //!transient

  /** Bad channel mask */
  Float_t ***fBadChannelMask;                                    //! transient

  /** Channel book keeping variable */
  AliHLTCaloDigitDataStruct ***fChannelBook;                     //! transient

  /** Maximum energy we allow in a channel */
  Float_t fMaxEnergy;                                            //COMMENT

  /** Assignment operator and copy constructor not implemented */
  AliHLTCaloDigitMaker(const AliHLTCaloDigitMaker &);
  AliHLTCaloDigitMaker & operator = (const AliHLTCaloDigitMaker &);

  ClassDef(AliHLTCaloDigitMaker, 0); 

};

#endif
 

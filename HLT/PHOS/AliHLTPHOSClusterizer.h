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

#ifndef ALIHLTPHOSCLUSTERIZER_H
#define ALIHLTPHOSCLUSTERIZER_H


/**
 * Class does clusterization in for PHOS on an event basis. It is intended 
 * for use in HLT, but can also be used offline
 *
 * @file   AliHLTPHOSClusterizer.h
 * @author Oystein Djuvsland
 * @date
 * @brief  Clusterizer for PHOS HLT
 */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt


#include "AliHLTPHOSRecPointContainerStruct.h"
#include "AliHLTPHOSRecPointDataStruct.h"
#include "AliHLTPHOSDigitContainerDataStruct.h"
#include "AliHLTPHOSDigitDataStruct.h"
#include "AliHLTLogging.h"

#include "AliPHOSGeometry.h"

class AliHLTPHOSDigitReader;

/** 
 * @class AliHLTPHOSClusterizer
 * Clusterizer for PHOS HLT. The clusterizer takes digits as input, either
 * in the form of a container of AliHLTPHOSDigitDataStruct or a
 * TClonesArray of AliPHOSDigit through an instance of a AliPHOSLoader
 *
 * @ingroup alihlt_phos
 */
//class AliHLTPHOSClusterizer : public AliHLTPHOSBase
class AliHLTPHOSClusterizer : public AliHLTLogging
{
  
public:
  
  /** Constructor */
  AliHLTPHOSClusterizer();    
  
  /** Destructor */
  virtual ~AliHLTPHOSClusterizer();

  /** Copy constructor */  
  AliHLTPHOSClusterizer(const AliHLTPHOSClusterizer &) : 
    AliHLTLogging(),
    fRecPointDataPtr(0),
    fDigitDataPtr(0),
    fCurrentDigit(0),
    fStartDigit(0),
    fPreviousDigit(0),
    fEmcClusteringThreshold(0),
    fEmcMinEnergyThreshold(0),
    fEmcTimeGate(0),
    fDigitsInCluster(0),
    fDigitContainerPtr(0),
    fMaxDigitIndexDiff(2*56),
    fAvailableSize(0),
    fDigitReader(0)
  {
    //Copy constructor not implemented
  }
  
  /** Assignment */
  AliHLTPHOSClusterizer & operator = (const AliHLTPHOSClusterizer)
  {
    //Assignment
    return *this; 
  }
  
  /** Set digit container */
  void SetDigitContainer(AliHLTPHOSDigitContainerDataStruct* digitContainerPtr)
  { fDigitContainerPtr = digitContainerPtr; }

  /** Set rec point data buffer */
  void SetRecPointDataPtr(AliHLTPHOSRecPointDataStruct* recPointDataPtr);

  /** Set emc clustering threshold */
  void SetEmcClusteringThreshold(Float_t threshold) { fEmcClusteringThreshold = threshold; }

  /** Set emc min energy threshold */
  void SetEmcMinEnergyThreshold(Float_t threshold) { fEmcMinEnergyThreshold = threshold; }

  /** Set emc time gate */
  void SetEmcTimeGate(Float_t gate) { fEmcTimeGate = gate; }

  /** Starts clusterization of the event */ 
  virtual Int_t ClusterizeEvent(AliHLTPHOSDigitHeaderStruct *digitHeader, UInt_t availableSize, UInt_t& totSize);

  /**
   * For a given digit this digit scans for neighbouring digits which 
   * passes the threshold for inclusion in a rec point. If one is found 
   * it is added to the current rec point
   * @param recPoint pointer to the current rec point
   */
  virtual Int_t ScanForNeighbourDigits(AliHLTPHOSRecPointDataStruct* recPoint, AliHLTPHOSDigitDataStruct *digit);

  /**
   * Checks if two digits are neighbours
   * @param d1 first digit
   * @param d2 second digit
   */
  virtual Int_t AreNeighbours(AliHLTPHOSDigitDataStruct* d1, AliHLTPHOSDigitDataStruct* d2);


protected:

  /** Pointer to the rec point output */
  AliHLTPHOSRecPointDataStruct* fRecPointDataPtr;              //! transient

  /** Pointer to the digit output */
  AliHLTPHOSDigitDataStruct* fDigitDataPtr;                    //! transient

  /** Pointer to the digit output */
  AliHLTPHOSDigitDataStruct* fCurrentDigit;                    //! transient

  /** Pointer to the digit output */
  AliHLTPHOSDigitDataStruct* fStartDigit;                    //! transient

  /** Pointer to the digit output */
  AliHLTPHOSDigitDataStruct* fPreviousDigit;                    //! transient
  /** Energy threshold for starting a cluster for the calorimeter */
  Float_t fEmcClusteringThreshold;                             //COMMENT

  /** Energy threshold for including a crystal in a cluster */
  Float_t fEmcMinEnergyThreshold;                              //COMMENT

  /** Maximum time difference for inclusion in a rec point */
  Float_t fEmcTimeGate;                                        //COMMENT

  /** Counts the digits in a rec point */
  Int_t fDigitsInCluster;                                      //COMMENT

  /** Contains the digits from one event */
  AliHLTPHOSDigitContainerDataStruct *fDigitContainerPtr;      //! transient

  /** Maximum difference in index to be a neighbour */
  Int_t fMaxDigitIndexDiff;                                    //COMMENT

  /** Current available buffer size */
  UInt_t fAvailableSize;                                       //COMMENT

  /** object reading the digit */
  AliHLTPHOSDigitReader *fDigitReader;                         //COMMENT

  ClassDef(AliHLTPHOSClusterizer, 0);
};

#endif

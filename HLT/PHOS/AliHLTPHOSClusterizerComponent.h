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

#ifndef ALIHLTPHOSCLUSTERIZERCOMPONENT_H
#define ALIHLTPHOSCLUSTERIZERCOMPONENT_H



/**
 * Clusterizer component for PHOS HLT
 *
 * @file   AliHLTPHOSClusterizerComponent.h
 * @author Oystein Djuvsland
 * @date   
 * @brief  A clusterizer component for PHOS HLT
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliHLTPHOSProcessor.h"

class AliHLTPHOSClusterizer;
class AliHLTPHOSRcuCellEnergyDataStruct;
class AliHLTPHOSRecPointDataStruct;
class AliHLTPHOSRecPointContainerStruct;
class AliHLTPHOSRecPointListDataStruct;
class AliHLTPHOSDigitContainerDataStruct;

/**
 * @class AliHLTPHOSClusterizerComponent
 *
 * Class for running clusterization for PHOS in HLT. It takes digits as input and
 * gives reconstruction points as output. 
 * 
 * The component has the following component arguments:
 * -clusterthreshold       The energy threshold for starting a new rec point
 * -energythreshold        The energy threshold for including a digit in a
 *                         rec point
 * @ingroup alihlt_phos
 */

/**
 * @class AliHLTPHOSClusterizerComponent
 *
 * Class for running clusterization for PHOS in HLT. 
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b PhosClusterizer <br>
 * Library: \b libAliHLTPHOS.so     <br>
 * Input Data Types: @ref AliHLTPHOSDefinitions::fgkDigitDataType<br>
 * Output Data Types: @ref AliHLTPHOSDefinitions::fgkRecPointDataType<br>
 *
 * <h2>Mandatory arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li No mandatory arguments for component                           <br>
 *
 * <h2>Optional arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -digitthreshold      <i> value </i> <br>
 *      threshold for a digit to be added to a rec point in GeV (default value: 0.03)
 * \li -recpointthreshold <i> value </i> <br>
 *      threshold for starting a new rec point  (default value: 0.2)
 * \li -partitionmode
 *      if we want to do clusterisation on the partition level (not available...) (defaul value: false)
 *
 * <h2>Configuration:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li No configuration arguments 
 *
 * <h2>Default CDB entries:</h2>
 * \li No CDB entry yet, will come.
 *
 * <h2>Performance:</h2>
 * Pretty good (~ 3 kHz), depends on amount of data...
 *
 * <h2>Memory consumption:</h2>
 * Depends on the amount of data, but pretty godd
 *
 * <h2>Output size:</h2>
 * Depends on the amount of data...
 *
 * More detailed description. (At some point...)
 *
 * @ingroup alihlt_phos
 */ 

class AliHLTPHOSClusterizerComponent: public AliHLTPHOSProcessor
{
 public:

  /** Constructor */
  AliHLTPHOSClusterizerComponent();

  /** Destructor */
  virtual ~AliHLTPHOSClusterizerComponent();

  /** Copy constructor */  
  AliHLTPHOSClusterizerComponent(const AliHLTPHOSClusterizerComponent &) : 
    AliHLTPHOSProcessor(),
    fAllDigitsPtr(0),
    fClusterizerPtr(0),
    //    fRecPointStructArrayPtr(0),
    fDigitCount(0),
    fNoCrazyness(0)
  {
    //Copy constructor not implemented
  }
  
  /** Assignment */
  AliHLTPHOSClusterizerComponent & operator = (const AliHLTPHOSClusterizerComponent)
  {
    //Assignment
    return *this; 
  }

  /** interface function, see @ref AliHLTComponent for description */
  const char* GetComponentID();

  /** interface function, see @ref AliHLTComponent for description */
  void GetInputDataTypes(std::vector<AliHLTComponentDataType>& list);

  /** interface function, see @ref AliHLTComponent for description */
  AliHLTComponentDataType GetOutputDataType();

  /** interface function, see @ref AliHLTComponent for description */
  void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);

  /** interface function, see @ref AliHLTComponent for description */
  
  using  AliHLTPHOSProcessor::DoEvent;
  int DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
		AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
		std::vector<AliHLTComponentBlockData>& outputBlocks);
  // Int_t DoEvent( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData);
  /** interface function, see @ref AliHLTComponent for description */
  AliHLTComponent* Spawn();
  
protected:

  /** interface function, see @ref AliHLTComponent for description */
  int DoInit(int argc, const char** argv);

  /** interface function, see @ref AliHLTComponent for description */
  int Deinit();

  /** interface function, see @ref AliHLTComponent for description */
  int Reconfigure(const char* cdbEntry, const char* chainId);

  /** interface function, see @ref AliHLTComponent for description */
  int ScanConfigurationArgument(int argc, const char** argv);

 private:

  /** All digits in the event */
  AliHLTPHOSDigitContainerDataStruct *fAllDigitsPtr;            //! transient

  /** Pointer to the clusterizer it self */
  AliHLTPHOSClusterizer* fClusterizerPtr;                       //! transient

//   /** Pointer to rec points used in clusterization */
//   AliHLTPHOSRecPointDataStruct* fRecPointStructArrayPtr;        //! transient

  /** Number of digits in event */
  Int_t fDigitCount;                                            //COMMENT

  /** If one should consider crazyness or not */                              
  Bool_t fNoCrazyness;                                          //COMMENT

};

#endif

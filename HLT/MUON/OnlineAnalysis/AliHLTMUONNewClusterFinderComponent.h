#ifndef AliHLTMUONNewClusterFinderComponent_H
#define AliHLTMUONNewClusterFinderComponent_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
///  @file   AliHLTMUONNewClusterFinderComponent.h
///  @author Hugo Pereira Da Costa <pereira@hep.saclay.cea.fr>
///  @date   March 08 2016
///  @brief  load digits from raw data
///

#include "AliHLTProcessor.h"
#include "AliHLTMUONClustersBlockStruct.h"

#include "AliMUONTrackerDDLDecoderEventHandler.h"
#include "AliMUONTrackerDDLDecoder.h"
#include "AliHLTMUONDataBlockWriter.h"
#include "AliHLTMUONPreClustersBlock.h"

#include <TExMap.h>

/**
 * @class AliHLTMUONNewClusterFinderComponent
 * @brief A processing component for the dHLT tracker DDL reconstruction.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONNewClusterFinder <br>
 * Library: \b libAliHLTMUON.so  <br>
 *
 * @ingroup alihlt_muon_components
 */
class AliHLTMUONNewClusterFinderComponent : public AliHLTProcessor
{

  public:
  AliHLTMUONNewClusterFinderComponent();
  virtual ~AliHLTMUONNewClusterFinderComponent();

  // Public functions to implement AliHLTProcessor's interface.
  // These functions are required for the registration process

  virtual const char* GetComponentID();
  virtual void GetInputDataTypes(AliHLTComponentDataTypeList& );
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long&, double& );
  virtual AliHLTComponent* Spawn();

  protected:
  // Protected functions to implement AliHLTProcessor's interface.
  // These functions provide initialization as well as the actual processing
  // capabilities of the component.

  virtual int DoInit(int , const char** );
  virtual int DoDeinit();
  virtual int DoEvent(
    const AliHLTComponentEventData&,
    const AliHLTComponentBlockData*,
    AliHLTComponentTriggerData&,
    AliHLTUInt8_t*,
    AliHLTUInt32_t&,
    AliHLTComponentBlockDataList&);

  using AliHLTProcessor::DoEvent;

  private:

  // create cluster block
  int CreateClustersBlock(
    AliHLTUInt8_t*, AliHLTUInt32_t ,
    AliHLTMUONClustersBlockWriter*& ) const;

  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONNewClusterFinderComponent(const AliHLTMUONNewClusterFinderComponent& /*obj*/);

  /// Not implemented.
  AliHLTMUONNewClusterFinderComponent& operator = (const AliHLTMUONNewClusterFinderComponent& /*obj*/);

  // to read pre clusters data blocks
  AliHLTMUONPreClustersBlock fPreClusterBlock;

//   // digit block writer
//   AliHLTMUONDigitsBlockWriter* fClustersBlock;

  ClassDef(AliHLTMUONNewClusterFinderComponent, 0)
};

#endif

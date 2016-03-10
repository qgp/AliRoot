/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
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
///  @file   AliHLTMUONNewClusterFinderComponent.cxx
///  @author Hugo Pereira Da Costa <pereira@hep.saclay.cea.fr>
///  @date   March 08 2016
///  @brief  load digits from raw data
///

// HLT MUON
#include "AliHLTMUONNewClusterFinderComponent.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTMUONUtils.h"

#include "AliCodeTimer.h"

ClassImp(AliHLTMUONNewClusterFinderComponent)

//_________________________________________________________________________________________________
AliHLTMUONNewClusterFinderComponent::AliHLTMUONNewClusterFinderComponent()
{}

//_________________________________________________________________________________________________
AliHLTMUONNewClusterFinderComponent::~AliHLTMUONNewClusterFinderComponent()
{
  /// Default destructor
}

//_________________________________________________________________________________________________
const char* AliHLTMUONNewClusterFinderComponent::GetComponentID()
{
  /// Inherited from AliHLTComponent. Returns the component ID.
  return "MUONNewClusterFinder";
}

//_________________________________________________________________________________________________
void AliHLTMUONNewClusterFinderComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  /// Inherited from AliHLTComponent. Returns the list of expected input data types.
  list.clear();
  list.push_back(AliHLTMUONConstants::PreClustersBlockDataType());
}

//_________________________________________________________________________________________________
AliHLTComponentDataType AliHLTMUONNewClusterFinderComponent::GetOutputDataType()
{
  /// Inherited from AliHLTComponent. Returns the output data type.
  return AliHLTMUONConstants::ClusterBlockDataType();
}

//_________________________________________________________________________________________________
void AliHLTMUONNewClusterFinderComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  /// Inherited from AliHLTComponent. Returns an estimate of the expected output data size.
  constBase = 0;
  inputMultiplier = 2;
}

//_________________________________________________________________________________________________
AliHLTComponent* AliHLTMUONNewClusterFinderComponent::Spawn()
{
  /// Inherited from AliHLTComponent. Creates a new object instance.
  return new AliHLTMUONNewClusterFinderComponent();
}

//_________________________________________________________________________________________________
int AliHLTMUONNewClusterFinderComponent::DoInit(int argc, const char** argv)
{
  /// Inherited from AliHLTComponent.
  /// Parses the command line parameters and initialises the component.

  HLTInfo("Initialising dHLT digit loader component component.");

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONNewClusterFinderComponent::DoDeinit()
{
  /// Inherited from AliHLTComponent. Performs a cleanup of the component.
  HLTInfo("Deinitialising dHLT digit loader component.");
  AliCodeTimer::Instance()->Print();

  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONNewClusterFinderComponent::DoEvent(
  const AliHLTComponentEventData& /*evtData*/,
  const AliHLTComponentBlockData* /*blocks*/,
  AliHLTComponentTriggerData& /*trigData*/,
  AliHLTUInt8_t* outputPtr,
  AliHLTUInt32_t& size,
  AliHLTComponentBlockDataList& outputBlocks )
{

  /// Inherited from AliHLTProcessor. Processes the new event data.
  AliCodeTimerAutoGeneral("",0);

  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return 0;

//   // create digit block
//   int status = CreateClustersBlock( outputPtr, size, clusterBlock );
//   if( status < 0 ) return status;

  // loop over blocks of the data type "DDL_RAW"
  AliCodeTimerStartGeneral("LoadDigitsFromRaw");
  const AliHLTComponentBlockData* pBlock = GetFirstInputBlock(AliHLTMUONConstants::PreClustersBlockDataType());
  while (pBlock)
  {

    int status = fPreClusterBlock.Reset(pBlock->fPtr, pBlock->fSize, false);
    if( status < 0 )
    {

      HLTError("Invalid data block of type \"PreClustersBlock\" for DE %d.",pBlock->fSpecification);
      return status;

    }

    std::cerr << fPreClusterBlock << std::endl;

    pBlock = GetNextInputBlock();

  }

}

//_________________________________________________________________________________________________
int AliHLTMUONNewClusterFinderComponent::CreateClustersBlock(
  AliHLTUInt8_t *outputPtr,
  AliHLTUInt32_t size,
  AliHLTMUONClustersBlockWriter *&clustersblock ) const
{

  /// Create a digits data block.
  clustersblock = new AliHLTMUONClustersBlockWriter(outputPtr, size);
  if (!clustersblock->InitCommonHeader())
  {
    HLTError("The buffer is too small to store a new clusters block.");
    return -ENOBUFS;
  }

  return 0;

}

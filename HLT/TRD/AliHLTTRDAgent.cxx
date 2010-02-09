//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/** @file   AliHLTTRDAgent.cxx
    @author Matthias Richter
    @date   
    @brief  Agent of the libAliHLTTRD library
*/

#include "AliHLTTRDAgent.h"
#include "AliHLTConfiguration.h"
#include "AliHLTTRDDefinitions.h"

// #include "AliHLTOUT.h"
// #include "AliHLTOUTHandlerChain.h"
// #include "AliRunLoader.h"

/** global instance for agent registration */
AliHLTTRDAgent gAliHLTTRDAgent;

// component headers
#include "AliHLTTRDClusterizerComponent.h"
#include "AliHLTTRDTrackerV1Component.h"
#include "AliHLTTRDCalibrationComponent.h"
#include "AliHLTTRDEsdWriterComponent.h"
#include "AliHLTTRDClusterHistoComponent.h"
#include "AliHLTTRDTrackHistoComponent.h"
#include "AliHLTTRDOfflineClusterizerComponent.h"
#include "AliHLTTRDOfflineTrackerV1Component.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTRDAgent)

  AliHLTTRDAgent::AliHLTTRDAgent()
    :
    AliHLTModuleAgent("TRD"),
    fRawDataHandler(NULL)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTRDAgent::~AliHLTTRDAgent()
{
  // see header file for class documentation
}

int AliHLTTRDAgent::CreateConfigurations(AliHLTConfigurationHandler* /*handler*/,
					 AliRawReader* /*rawReader*/,
					 AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation

  return 0;
}

const char* AliHLTTRDAgent::GetReconstructionChains(AliRawReader* /*rawReader*/,
						    AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation

  return "";
}

const char* AliHLTTRDAgent::GetRequiredComponentLibraries() const
{
  return "";
}

int AliHLTTRDAgent::RegisterComponents(AliHLTComponentHandler* pHandler) const
{
  // see header file for class documentation
  if (!pHandler) return -EINVAL;
  pHandler->AddComponent(new AliHLTTRDClusterizerComponent);
  pHandler->AddComponent(new AliHLTTRDTrackerV1Component);
  pHandler->AddComponent(new AliHLTTRDCalibrationComponent);
  pHandler->AddComponent(new AliHLTTRDEsdWriterComponent);
  pHandler->AddComponent(new AliHLTTRDClusterHistoComponent);
  pHandler->AddComponent(new AliHLTTRDTrackHistoComponent);
  pHandler->AddComponent(new AliHLTTRDOfflineClusterizerComponent);
  pHandler->AddComponent(new AliHLTTRDOfflineTrackerV1Component);
  
  return 0;
}

int AliHLTTRDAgent::GetHandlerDescription(AliHLTComponentDataType dt,
					  AliHLTUInt32_t spec,
					  AliHLTOUTHandlerDesc& desc) const
{
  // see header file for class documentation

  // raw data blocks to be fed into offline reconstruction
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTRD)) {
    desc=AliHLTOUTHandlerDesc(kRawReader, dt, GetModuleId());
    HLTInfo("module %s handles data block type %s specification %d (0x%x)", 
	    GetModuleId(), AliHLTComponent::DataType2Text(dt).c_str(), spec, spec);
    return 1;
  }
  return 0;
}

AliHLTOUTHandler* AliHLTTRDAgent::GetOutputHandler(AliHLTComponentDataType dt,
						   AliHLTUInt32_t /*spec*/)
{
  // see header file for class documentation

  // raw data blocks to be fed into offline reconstruction
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTRD)) {
    // use the default handler
    if (!fRawDataHandler) {
      fRawDataHandler=new AliHLTOUTHandlerEquId;
    }
    return fRawDataHandler;
  }
  return NULL;
}

int AliHLTTRDAgent::DeleteOutputHandler(AliHLTOUTHandler* pInstance)
{
  // see header file for class documentation
  if (pInstance==NULL) return -EINVAL;

  if (pInstance==fRawDataHandler) {
    delete fRawDataHandler;
    fRawDataHandler=NULL;
    return 0;
  }

  delete pInstance;
  return 0;
}


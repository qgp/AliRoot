// $Id$

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

/** @file   AliHLTITSAgent.cxx
    @author Matthias Richter
    @date   25.08.2008
    @brief  Agent of the libAliHLTITS library
*/

#include <cassert>
#include "AliHLTITSAgent.h"
#include "AliHLTConfiguration.h"
#include "AliHLTOUT.h"
#include "AliHLTDAQ.h"

// header files of library components

// header file of the module preprocessor
#include "AliHLTITSCompressRawDataSDDComponent.h"
#include "AliHLTITSSSDQARecPointsComponent.h"
#include "AliHLTITSQAComponent.h"
#include "AliHLTITSClusterFinderComponent.h"
#include "AliHLTITSClusterHistoComponent.h"
#include "AliHLTITSTrackerComponent.h"
#include "AliHLTITSVertexerSPDComponent.h"

/** global instance for agent registration */
AliHLTITSAgent gAliHLTITSAgent;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTITSAgent)

AliHLTITSAgent::AliHLTITSAgent()
  :
  AliHLTModuleAgent("ITS"),
  fRawDataHandler(NULL)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTITSAgent::~AliHLTITSAgent()
{
  // see header file for class documentation
}

int AliHLTITSAgent::CreateConfigurations(AliHLTConfigurationHandler* handler,
					 AliRawReader* rawReader,
					 AliRunLoader* runloader) const
{
  // see header file for class documentation
  int iResult=0;
  if (!handler) return -EINVAL;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // ITS tracking is currently only working on raw data
  // to run on digits, a digit publisher needs to be implemented

  if (rawReader || !runloader) {
    // AliSimulation: use the AliRawReaderPublisher if the raw reader is available
    // Alireconstruction: indicated by runloader==NULL, run always on raw data

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // define the ITS cluster finder configurations
    //

    TString trackerInput;
    for (int detectorId=0; detectorId<3; detectorId++) {
      iResult=CreateCFConfigurations(handler, detectorId, trackerInput);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // define the ITS tracker configuration
    //
    if (handler->FindConfiguration("TPC-globalmerger")) {
      trackerInput+=" TPC-globalmerger";
    }
    handler->CreateConfiguration("ITS-tracker","ITSTracker",trackerInput.Data(),"");
  }
  return 0;
}

const char* AliHLTITSAgent::GetReconstructionChains(AliRawReader* /*rawReader*/,
						       AliRunLoader* runloader) const
{
  // see header file for class documentation
  if (runloader) {
    // reconstruction chains for AliRoot simulation
    // Note: run loader is only available while running embedded into
    // AliRoot simulation

    // the chain is just defined and can be used as input for subsequent
    // components
    //return "ITS-tracker";
  }

  return "";
}

const char* AliHLTITSAgent::GetRequiredComponentLibraries() const
{
  // see header file for class documentation
  return "libAliHLTUtil.so libAliHLTRCU.so libAliHLTTPC.so";
}

int AliHLTITSAgent::RegisterComponents(AliHLTComponentHandler* pHandler) const
{
  // see header file for class documentation
  assert(pHandler);
  if (!pHandler) return -EINVAL;
  pHandler->AddComponent(new AliHLTITSCompressRawDataSDDComponent);
  pHandler->AddComponent(new AliHLTITSSSDQARecPointsComponent);
  pHandler->AddComponent(new AliHLTITSQAComponent);
  pHandler->AddComponent(new AliHLTITSClusterFinderComponent(AliHLTITSClusterFinderComponent::kClusterFinderSPD));
  pHandler->AddComponent(new AliHLTITSClusterFinderComponent(AliHLTITSClusterFinderComponent::kClusterFinderSDD));
  pHandler->AddComponent(new AliHLTITSClusterFinderComponent(AliHLTITSClusterFinderComponent::kClusterFinderSSD));
  pHandler->AddComponent(new AliHLTITSClusterHistoComponent);
  pHandler->AddComponent(new AliHLTITSTrackerComponent);
  pHandler->AddComponent(new AliHLTITSVertexerSPDComponent);

  return 0;
}

AliHLTModulePreprocessor* AliHLTITSAgent::GetPreprocessor()
{
  // see header file for class documentation
  return NULL;
}

int AliHLTITSAgent::GetHandlerDescription(AliHLTComponentDataType dt,
					     AliHLTUInt32_t spec,
					     AliHLTOUTHandlerDesc& desc) const
{
  // see header file for class documentation

  // Handlers for ITS raw data. Even though there are 3 detectors
  // everything is handled in one module library and one HLTOUT handler.
  // This assumes that the data blocks are sent with data type
  // {DDL_RAW :ISDD} and the bit set in the specification corresponding.
  // to detector DDL id.
  // An HLTOUT handler is implemented to extract the equipment id from
  // the specification.
  // Note: Future versions of the framework will provide a default handler
  // class with that functionality.
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSDD)) {
      desc=AliHLTOUTHandlerDesc(kRawReader, dt, GetModuleId());
      HLTInfo("module %s handles data block type %s specification %d (0x%x)", 
	      GetModuleId(), AliHLTComponent::DataType2Text(dt).c_str(), spec, spec);
      return 1;
  }
  return 0;
}

AliHLTOUTHandler* AliHLTITSAgent::GetOutputHandler(AliHLTComponentDataType dt,
						   AliHLTUInt32_t /*spec*/)
{
  // see header file for class documentation
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSDD)) {
    // use the default handler
    if (!fRawDataHandler) {
      fRawDataHandler=new AliHLTOUTSDDRawDataHandler;
    }
    return fRawDataHandler;
  }
  return NULL;
}

int AliHLTITSAgent::DeleteOutputHandler(AliHLTOUTHandler* pInstance)
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

int AliHLTITSAgent::AliHLTOUTSDDRawDataHandler::ProcessData(AliHLTOUT* pData)
{
  // see header file for class documentation
  if (!pData) return -EINVAL;
  static int errorCount=0;
  const int maxErrorCount=10;
  AliHLTComponentDataType dt=kAliHLTVoidDataType;
  AliHLTUInt32_t spec=kAliHLTVoidDataSpec;
  int iResult=pData->GetDataBlockDescription(dt, spec);
  if (iResult>=0) {
    if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSDD)) {
      int ddlOffset=256;//AliDAQ::DdlIDOffset("ITSSDD");
      int numberOfDDLs=24;//AliDAQ::NumberOfDdls("ITSSDD");
      int ddlNo=0;
      for (;ddlNo<32 && ddlNo<numberOfDDLs; ddlNo++) {
	if (spec&(0x1<<ddlNo)) break;
      }
      if (ddlNo>=32 || ddlNo>=numberOfDDLs) {
	HLTError("invalid specification 0x%08x: can not extract DDL id for data block %s", spec, AliHLTComponent::DataType2Text(dt).c_str());
	iResult=-ENODEV;
      } else if (spec^(0x1<<ddlNo)) {
	iResult=-EEXIST;
	HLTError("multiple links set in specification 0x%08x: can not extract DDL id for data block %s", spec, AliHLTComponent::DataType2Text(dt).c_str());
      } else {
	iResult=ddlOffset+ddlNo;
      }
    } else {
      if (errorCount++<10) {
	HLTError("wrong data type: expecting %s, got %s; %s",
		 AliHLTComponent::DataType2Text(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSDD).c_str(),
		 AliHLTComponent::DataType2Text(dt).c_str(),
		   errorCount==maxErrorCount?"suppressing further error messages":"");
      }
      iResult=-EFAULT;
    }
  }
  return iResult;
}

int AliHLTITSAgent::CreateCFConfigurations(AliHLTConfigurationHandler* pHandler, int detectorId, TString& output) const
{
  // see header file for class documentation
  if (!pHandler) return -EINVAL;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // define the ITS cluster finder components
  //
    
  //The spec starts from 0x1 in SPD, SDD and SSD. So 0x1 is ddl 0 for SPD, 0x10 is ddl 1, and so on
  //in SDD 0x1 is ddl 256, 0x10 is ddl 257, and so on. This means that the spec has to be set to 0x1 
  //before the loops over the clusterfinder

  TString idString=AliHLTDAQ::DetectorName(detectorId);
  if (idString.CompareTo("ITSSPD") &&
      idString.CompareTo("ITSSDD") &&
      idString.CompareTo("ITSSSD")) {
    HLTError("invalid detector id %d does not describe any ITS detector", detectorId);
    return -ENOENT;
  }

  int minddl=AliHLTDAQ::DdlIDOffset(detectorId);
  int maxddl=minddl+=AliHLTDAQ::NumberOfDdls(detectorId);
  int spec=0x1;
  int ddlno=0;

  TString origin=idString; origin.ReplaceAll("ITS", "I");
  TString cfBase=idString; cfBase+="_CF";
  TString componentId=idString; componentId.ReplaceAll("ITS", "ITSClusterFinder");
  for(ddlno=minddl;ddlno<=maxddl;ddlno++){  
    TString arg, publisher, cf;
 
    // the HLT origin defines are 4 chars: ISPD, ISSD, ISDD respectively
    arg.Form("-minid %d -datatype 'DDL_RAW ' '%s' -dataspec 0x%08x -verbose",ddlno, origin.Data(), spec);
    publisher.Form("ITS-DP_%d", ddlno);
    pHandler->CreateConfiguration(publisher.Data(), "AliRawReaderPublisher", NULL , arg.Data());

    cf.Form("%s_%d",cfBase.Data(), ddlno);
    pHandler->CreateConfiguration(cf.Data(), componentId.Data(), publisher.Data(), "");

    if (output.Length()>0) output+=" ";
    output+=cf;

    spec=spec<<1;
  }
  
  return 0;
}

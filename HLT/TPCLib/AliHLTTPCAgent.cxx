// @(#) $Id$

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

/** @file   AliHLTTPCAgent.cxx
    @author Matthias Richter
    @date   
    @brief  Agent of the libAliHLTTPC library
*/

#include "AliHLTTPCAgent.h"
#include "AliHLTConfiguration.h"
#include "AliHLTTPCDefinitions.h"
#include "AliHLTOUT.h"
#include "AliHLTOUTHandlerChain.h"
#include "AliRunLoader.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"
#include "AliTPCParam.h"

/** global instance for agent registration */
AliHLTTPCAgent gAliHLTTPCAgent;

// component headers
#include "AliHLTTPCRunStatisticsProducerComponent.h"
#include "AliHLTTPCEventStatisticsProducerComponent.h"
#include "AliHLTTPCCompModelInflaterComponent.h"
#include "AliHLTTPCCompModelDeflaterComponent.h"
#include "AliHLTTPCCompModelDeconverterComponent.h"
#include "AliHLTTPCCompModelConverterComponent.h"
#include "AliHLTTPCCompDumpComponent.h"
//#include "AliHLTTPCCalibCEComponent.h"
//#include "AliHLTTPCCalibPulserComponent.h"
//#include "AliHLTTPCCalibPedestalComponent.h"
#include "AliHLTTPCCAInputDataCompressorComponent.h"
#include "AliHLTTPCCATrackerComponent.h"
#include "AliHLTTPCTrackMCMarkerComponent.h"
#include "AliHLTTPCCAGlobalMergerComponent.h"
#include "AliHLTTPCdEdxComponent.h"
#include "AliHLTTPCGlobalMergerComponent.h"
#include "AliHLTTPCSliceTrackerComponent.h"
#include "AliHLTTPCVertexFinderComponent.h"
#include "AliHLTTPCClusterFinderComponent.h"
#include "AliHLTTPCRawDataUnpackerComponent.h"
#include "AliHLTTPCDigitPublisherComponent.h"
#include "AliHLTTPCZeroSuppressionComponent.h"
#include "AliHLTTPCDigitDumpComponent.h"
#include "AliHLTTPCClusterDumpComponent.h"
#include "AliHLTTPCEsdWriterComponent.h"
#include "AliHLTTPCOfflineClustererComponent.h"
#include "AliHLTTPCOfflineTrackerComponent.h"
#include "AliHLTTPCOfflineTrackerCalibComponent.h"
#include "AliHLTTPCOfflineCalibrationComponent.h" // to be added to the calibration library agent
#include "AliHLTTPCClusterHistoComponent.h"
#include "AliHLTTPCNoiseMapComponent.h"
#include "AliHLTTPCHistogramHandlerComponent.h"
//#include "AliHLTTPCCalibTracksComponent.h"
#include "AliHLTTPCTrackHistoComponent.h"
#include "AliHLTTPCTrackDumpComponent.h"
#include "AliHLTTPCHWCFDataReverterComponent.h"
#include "AliHLTTPCHWClusterTransformComponent.h"
// #include "AliHLTTPCCalibSeedMakerComponent.h"
// #include "AliHLTTPCCalibTimeComponent.h"
// #include "AliHLTTPCCalibTimeGainComponent.h"
// #include "AliHLTTPCCalibrationComponent.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCAgent)

AliHLTTPCAgent::AliHLTTPCAgent()
  :
  AliHLTModuleAgent("TPC"),
  fRawDataHandler(NULL),
  fTracksegsDataHandler(NULL)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCAgent::~AliHLTTPCAgent()
{
  // see header file for class documentation
}

int AliHLTTPCAgent::CreateConfigurations(AliHLTConfigurationHandler* handler,
					 AliRawReader* rawReader,
					 AliRunLoader* runloader) const
{
  // see header file for class documentation
  if (handler) {

    // This the tracking configuration for the full TPC
    // - 216 clusterfinders (1 per partition)
    // - 36 slice trackers
    // - one global merger
    // - the esd converter
    // The ESD is shipped embedded into a TTree
    int iMinSlice=0; 
    int iMaxSlice=35;
    int iMinPart=0;
    int iMaxPart=5;
    TString mergerInput;
    TString sinkClusterInput;
    for (int slice=iMinSlice; slice<=iMaxSlice; slice++) {
      TString trackerInput;
      for (int part=iMinPart; part<=iMaxPart; part++) {
	TString arg, publisher, cf;

	// digit publisher components
	publisher.Form("TPC-DP_%02d_%d", slice, part);
	if (rawReader || !runloader) {
	  // AliSimulation: use the AliRawReaderPublisher if the raw reader is available
	  // Alireconstruction: indicated by runloader==NULL, run always on raw data
	  int ddlno=768;
	  if (part>1) ddlno+=72+4*slice+(part-2);
	  else ddlno+=2*slice+part;
	  arg.Form("-minid %d -datatype 'DDL_RAW ' 'TPC '  -dataspec 0x%02x%02x%02x%02x -silent", ddlno, slice, slice, part, part);
	  handler->CreateConfiguration(publisher.Data(), "AliRawReaderPublisher", NULL , arg.Data());
	} else {
	  arg.Form("-slice %d -partition %d", slice, part);
	  handler->CreateConfiguration(publisher.Data(), "TPCDigitPublisher", NULL , arg.Data());
	}

	// cluster finder components
	cf.Form("TPC-CF_%02d_%d", slice, part);
	arg="-release-memory";
	if (!rawReader && runloader) {
	  arg+=" -do-mc";
	  handler->CreateConfiguration(cf.Data(), "TPCClusterFinderUnpacked", publisher.Data(), arg.Data());
	} else {
#ifndef HAVE_NOT_ALTRORAWSTREAMV3
	  handler->CreateConfiguration(cf.Data(), "TPCClusterFinder32Bit", publisher.Data(),arg.Data());
#else
	  // using the AltroDecoder if the official V3 decoder is not
	  // available in the offline code
	  handler->CreateConfiguration(cf.Data(), "TPCClusterFinderDecoder", publisher.Data(), arg.Data());
#endif 
	}
	if (trackerInput.Length()>0) trackerInput+=" ";
	trackerInput+=cf;
	if (sinkClusterInput.Length()>0) sinkClusterInput+=" ";
	sinkClusterInput+=cf;
      }
      TString tracker;
      // tracker finder components
      tracker.Form("TPC-TR_%02d", slice);
      handler->CreateConfiguration(tracker.Data(), "TPCCATracker", trackerInput.Data(), "");

      if (mergerInput.Length()>0) mergerInput+=" ";
      mergerInput+=tracker;

    }

    // GlobalMerger component
    handler->CreateConfiguration("TPC-globalmerger","TPCCAGlobalMerger",mergerInput.Data(),"");

    // the esd converter configuration
    TString converterInput="TPC-globalmerger";
    if (!rawReader && runloader) {
      // propagate cluster info to the esd converter in order to fill the MC information
      handler->CreateConfiguration("TPC-clustermc-info", "BlockFilter"   , sinkClusterInput.Data(), "-datatype 'CLMCINFO' 'TPC '");  
      handler->CreateConfiguration("TPC-mcTrackMarker","TPCTrackMCMarker","TPC-globalmerger TPC-clustermc-info","" );
      converterInput+=" ";
      converterInput+="TPC-mcTrackMarker";
    }
    handler->CreateConfiguration("TPC-esd-converter", "TPCEsdConverter"   , converterInput.Data(), "");

    // cluster dump collection
    handler->CreateConfiguration("TPC-clusters", "BlockFilter"   , sinkClusterInput.Data(), "-datatype 'CLUSTERS' 'TPC '");

    /////////////////////////////////////////////////////////////////////////////////////
    //
    // a kChain HLTOUT configuration for processing of {'TRAKSEGS':'TPC '} data blocks
    // collects the data blocks, merges the tracks and produces an ESD object

    // publisher component
    handler->CreateConfiguration("TPC-hltout-tracksegs-publisher", "AliHLTOUTPublisher"   , NULL, "");

    // GlobalMerger component
    handler->CreateConfiguration("TPC-hltout-tracksegs-merger", "TPCGlobalMerger", "TPC-hltout-tracksegs-publisher", "");

    // the esd converter configuration
    handler->CreateConfiguration("TPC-hltout-tracksegs-esd-converter", "TPCEsdConverter", "TPC-hltout-tracksegs-merger", "");

    /////////////////////////////////////////////////////////////////////////////////////
    //
    // a kChain HLTOUT configuration for processing of {'TRACKS  ':'TPC '} data blocks
    // produces an ESD object from the track structure

    // publisher component
    handler->CreateConfiguration("TPC-hltout-tracks-publisher", "AliHLTOUTPublisher"   , NULL, "");

    // the esd converter configuration
    handler->CreateConfiguration("TPC-hltout-tracks-esd-converter", "TPCEsdConverter", "TPC-hltout-tracks-publisher", "");
  }
  return 0;
}

const char* AliHLTTPCAgent::GetReconstructionChains(AliRawReader* /*rawReader*/,
						    AliRunLoader* runloader) const
{
  // see header file for class documentation
  if (runloader) {
    // reconstruction chains for AliRoot simulation
    // Note: run loader is only available while running embedded into
    // AliRoot simulation
    //if (runloader->GetLoader("TPCLoader") != NULL)
      //return "TPC-esd-converter TPC-clusters";
      return "TPC-clusters";
  }
  return NULL;
}

const char* AliHLTTPCAgent::GetRequiredComponentLibraries() const
{
  // see header file for class documentation

  // actually, the TPC library has dependencies to Util and RCU
  // so the two has to be loaded anyhow before we get here
  //return "libAliHLTUtil.so libAliHLTRCU.so";
  return "libAliHLTUtil.so";
}

int AliHLTTPCAgent::RegisterComponents(AliHLTComponentHandler* pHandler) const
{
  // see header file for class documentation
  if (!pHandler) return -EINVAL;

  pHandler->AddComponent(new AliHLTTPCRunStatisticsProducerComponent);
  pHandler->AddComponent(new AliHLTTPCEventStatisticsProducerComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibCEComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibPulserComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibPedestalComponent);
  pHandler->AddComponent(new AliHLTTPCCompModelInflaterComponent);
  pHandler->AddComponent(new AliHLTTPCCompModelDeflaterComponent);
  pHandler->AddComponent(new AliHLTTPCCompModelDeconverterComponent);
  pHandler->AddComponent(new AliHLTTPCCompModelConverterComponent);
  pHandler->AddComponent(new AliHLTTPCCompDumpComponent);
  pHandler->AddComponent(new AliHLTTPCCAInputDataCompressorComponent);
  pHandler->AddComponent(new AliHLTTPCCATrackerComponent);
  pHandler->AddComponent(new AliHLTTPCCAGlobalMergerComponent);
  pHandler->AddComponent(new AliHLTTPCTrackMCMarkerComponent);
  pHandler->AddComponent(new AliHLTTPCGlobalMergerComponent);
  pHandler->AddComponent(new AliHLTTPCdEdxComponent);
  pHandler->AddComponent(new AliHLTTPCSliceTrackerComponent);
  pHandler->AddComponent(new AliHLTTPCVertexFinderComponent);
  pHandler->AddComponent(new AliHLTTPCClusterFinderComponent(AliHLTTPCClusterFinderComponent::kClusterFinderPacked));
  pHandler->AddComponent(new AliHLTTPCClusterFinderComponent(AliHLTTPCClusterFinderComponent::kClusterFinderUnpacked));
  pHandler->AddComponent(new AliHLTTPCClusterFinderComponent(AliHLTTPCClusterFinderComponent::kClusterFinderDecoder));
  pHandler->AddComponent(new AliHLTTPCClusterFinderComponent(AliHLTTPCClusterFinderComponent::kClusterFinder32Bit));
  pHandler->AddComponent(new AliHLTTPCRawDataUnpackerComponent);
  pHandler->AddComponent(new AliHLTTPCDigitPublisherComponent);
  pHandler->AddComponent(new AliHLTTPCZeroSuppressionComponent);
  pHandler->AddComponent(new AliHLTTPCDigitDumpComponent);
  pHandler->AddComponent(new AliHLTTPCClusterDumpComponent);
  pHandler->AddComponent(new AliHLTTPCEsdWriterComponent::AliWriter);
  pHandler->AddComponent(new AliHLTTPCEsdWriterComponent::AliConverter);
  pHandler->AddComponent(new AliHLTTPCOfflineClustererComponent);
  pHandler->AddComponent(new AliHLTTPCOfflineTrackerComponent);
  pHandler->AddComponent(new AliHLTTPCOfflineTrackerCalibComponent);
  pHandler->AddComponent(new AliHLTTPCOfflineCalibrationComponent);
  pHandler->AddComponent(new AliHLTTPCClusterHistoComponent);
  pHandler->AddComponent(new AliHLTTPCNoiseMapComponent);
  pHandler->AddComponent(new AliHLTTPCHistogramHandlerComponent);
  //pHandler->AddComponent(new AliHLTTPCCalibTracksComponent);
  pHandler->AddComponent(new AliHLTTPCTrackHistoComponent);
  pHandler->AddComponent(new AliHLTTPCTrackDumpComponent);
  pHandler->AddComponent(new AliHLTTPCHWCFDataReverterComponent);
  pHandler->AddComponent(new AliHLTTPCHWClusterTransformComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibSeedMakerComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibTimeComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibTimeGainComponent);
//   pHandler->AddComponent(new AliHLTTPCCalibrationComponent);

  return 0;
}

int AliHLTTPCAgent::GetHandlerDescription(AliHLTComponentDataType dt,
					  AliHLTUInt32_t spec,
					  AliHLTOUTHandlerDesc& desc) const
{
  // see header file for class documentation

  // raw data blocks to be fed into offline reconstruction
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC)) {
    int slice=AliHLTTPCDefinitions::GetMinSliceNr(spec);
    int part=AliHLTTPCDefinitions::GetMinPatchNr(spec);
    if (slice==AliHLTTPCDefinitions::GetMaxSliceNr(spec) &&
	part==AliHLTTPCDefinitions::GetMaxPatchNr(spec)) {
      desc=AliHLTOUTHandlerDesc(kRawReader, dt, GetModuleId());
      return 1;
    } else {
      HLTWarning("handler can not process merged data from multiple ddls:"
		 " min slice %d, max slice %d, min part %d, max part %d",
		 slice, AliHLTTPCDefinitions::GetMaxSliceNr(spec),
		 part, AliHLTTPCDefinitions::GetMaxPatchNr(spec));
      return 0;
    }
  }

  // dump for {'CLUSTERS':'TPC '} currently not used any more
  if (dt==AliHLTTPCDefinitions::fgkClustersDataType) {
      desc=AliHLTOUTHandlerDesc(kProprietary, dt, GetModuleId());
      return 1;
  }

  // afterburner for {'TRAKSEGS':'TPC '} blocks to be converted to ESD format
  if (dt==AliHLTTPCDefinitions::fgkTrackSegmentsDataType) {
      desc=AliHLTOUTHandlerDesc(kChain, dt, GetModuleId());
      return 1;
  }

  // afterburner for {'TRACKS  ':'TPC '} block to be converted to ESD format
  // there is only one data block
  if (dt==AliHLTTPCDefinitions::fgkTracksDataType) {
      desc=AliHLTOUTHandlerDesc(kChain, dt, GetModuleId());
      return 1;
  }
  return 0;
}

AliHLTOUTHandler* AliHLTTPCAgent::GetOutputHandler(AliHLTComponentDataType dt,
						   AliHLTUInt32_t /*spec*/)
{
  // see header file for class documentation

  // raw data blocks to be fed into offline reconstruction
  if (dt==(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC)) {
    if (!fRawDataHandler) {
      fRawDataHandler=new AliHLTTPCAgent::AliHLTTPCRawDataHandler;
    }
    return fRawDataHandler;
  }

  // afterburner for {'TRAKSEGS':'TPC '} blocks to be converted to ESD format
  // in a kChain HLTOUT handler
  if (dt==AliHLTTPCDefinitions::fgkTrackSegmentsDataType) {
    if (fTracksegsDataHandler==NULL)
      fTracksegsDataHandler=new AliHLTOUTHandlerChain("chains=TPC-hltout-tracksegs-esd-converter");
    return fTracksegsDataHandler;
  }

  // afterburner for {'TRACKS  ':'TPC '} block to be converted to ESD format
  // there is only one data block
  if (dt==AliHLTTPCDefinitions::fgkTracksDataType) {
    return new AliHLTOUTHandlerChain("chains=TPC-hltout-tracks-esd-converter");
  }

  return NULL;
}

int AliHLTTPCAgent::DeleteOutputHandler(AliHLTOUTHandler* pInstance)
{
  // see header file for class documentation
  if (pInstance==NULL) return -EINVAL;

  if (pInstance==fRawDataHandler) {
    delete fRawDataHandler;
    fRawDataHandler=NULL;
  }

  if (pInstance==fTracksegsDataHandler) {
    delete fTracksegsDataHandler;
    fTracksegsDataHandler=NULL;
  }
  return 0;
}

AliHLTTPCAgent::AliHLTTPCRawDataHandler::AliHLTTPCRawDataHandler()
{
  // see header file for class documentation
}

AliHLTTPCAgent::AliHLTTPCRawDataHandler::~AliHLTTPCRawDataHandler()
{
  // see header file for class documentation
}

int AliHLTTPCAgent::AliHLTTPCRawDataHandler::ProcessData(AliHLTOUT* pData)
{
  // see header file for class documentation
  if (!pData) return -EINVAL;
  AliHLTComponentDataType dt=kAliHLTVoidDataType;
  AliHLTUInt32_t spec=kAliHLTVoidDataSpec;
  int iResult=pData->GetDataBlockDescription(dt, spec);
  if (iResult>=0) {
    int slice=AliHLTTPCDefinitions::GetMinSliceNr(spec);
    int part=AliHLTTPCDefinitions::GetMinPatchNr(spec);
    if (slice==AliHLTTPCDefinitions::GetMaxSliceNr(spec) &&
	part==AliHLTTPCDefinitions::GetMaxPatchNr(spec)) {
      iResult=768;
      if (part>1) iResult+=72+4*slice+(part-2);
      else iResult+=2*slice+part;
    } else {
      HLTError("handler can not process merged data from multiple ddls:"
	       " min slice %d, max slice %d, min part %d, max part %d",
	       slice, AliHLTTPCDefinitions::GetMaxSliceNr(spec),
	       part, AliHLTTPCDefinitions::GetMaxPatchNr(spec));
      iResult=-EBADMSG;
    }
  }
  return iResult;
}

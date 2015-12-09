// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Mikolaj Krzewicki <mikolaj.krzewicki@cern.ch>         *
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

//  @file   AliHLTGlobalPromptRecoQAComponent.cxx
//  @author Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
//  @date   
//  @brief  Simple HLT reco QA/monitor
// 

#include <cassert>
#include "AliHLTGlobalPromptRecoQAComponent.h"
#include "AliHLTGlobalBarrelTrack.h"
#include "AliHLTExternalTrackParam.h"
#include "AliHLTTrackMCLabel.h"
#include "AliHLTCTPData.h"
#include "AliHLTTPCDefinitions.h"
#include "AliHLTTPCSpacePointData.h"
#include "AliHLTTPCClusterDataFormat.h"
#include "AliTPCclusterMI.h"
#include "AliTPCseed.h"
#include "AliITStrackV2.h"
#include "AliESDfriend.h"
#include "AliESDfriendTrack.h"
#include "AliHLTTPCGeometry.h"
#include "AliHLTErrorGuard.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliESDMuonTrack.h"
#include "AliESDMuonCluster.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliPID.h"
#include "TTree.h"
#include "TList.h"
#include "TClonesArray.h"
#include "TTimeStamp.h"
#include "THnSparse.h"
#include "AliHLTESDCaloClusterMaker.h"
#include "AliHLTCaloClusterDataStruct.h"
#include "AliHLTCaloClusterReader.h"
#include "AliESDCaloCluster.h"
#include "AliESDVZERO.h"
#include "AliESDZDC.h"
#include "AliHLTGlobalVertexerComponent.h"
#include "AliHLTVertexFinderBase.h"
#include "AliSysInfo.h"
#include "AliHLTSAPTrackerData.h"
#include "AliFlatESDVertex.h"
#include "tracking-ca/AliHLTTPCCADefinitions.h"
#include "tracking-ca/AliHLTTPCCACompressedInputData.h"
#include "tracking-ca/AliHLTTPCCASliceOutput.h"
#include "AliHLTEMCALDefinitions.h"
#include "AliHLTTPCHWCFData.h"
#include "AliGRPManager.h"
#include "AliGRPObject.h"

#include "TH1I.h"
#include "TH2F.h"
#include <string>
#include "AliHLTITSClusterDataFormat.h"
#include "AliHLTCDHWrapper.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTGlobalPromptRecoQAComponent)

AliHLTGlobalPromptRecoQAComponent::AliHLTGlobalPromptRecoQAComponent()
  : AliHLTProcessor()
  , fVerbosity(0)
  , fBenchmark("PromptRecoQA")
  , fpHWCFData(NULL)
  , fSkipEvents(0)
  , fPrintStats(0)
  , fPrintDownscale(1)
  , fEventsSinceSkip(0)
  , fPushEmptyHistograms(false)
  , fHistograms()
  , fAxes()
  , fnClustersSPD(0.)
  , frawSizeSPD(0.)
  , fnClustersSDD(0.)
  , frawSizeSDD(0.)
  , fnClustersSSD(0.)
  , frawSizeSSD(0.)
  , fnClustersITS(0.)
  , frawSizeITS(0.)
  , frawSizeVZERO(0.)
  , frawSizeEMCAL(0.)
  , frawSizeZDC(0.)
  , fnClustersTPC(0.)
  , frawSizeTPC(0.)
  , fhwcfSizeTPC(0.)
  , fclusterSizeTPCtransformed(0.)
  , fclusterSizeTPC(0.)
  , fcompressedSizeTPC(0.)
  , fnITSSAPtracks(0.)
  , fnTPCtracklets(0.)
  , fnTPCtracks(0.)
  , fnITSTracks(0.)
  , fnITSOutTracks(0.)
  , fvZEROMultiplicity(0.)
  , fvZEROTriggerChargeA(0.)
  , fvZEROTriggerChargeC(0.)
  , fvZEROTriggerChargeAC(0.)
  , fzdcZNC(0.)
  , fzdcZNA(0.)
  , fzdcZNAC(0.)
  , fzdcRecoSize(0.)
  , femcalRecoSize(0.)
  , femcalTRU(0.)
  , femcalSTU(0.)
  , fcompressionRatio(0.)
  , fcompressionRatioFull(0.)
  , fnESDSize(0.)
  , fnESDFriendSize(0.)
  , fnFlatESDSize(0.)
  , fnFlatESDFriendSize(0.)
  , fnHLTInSize(0.)
  , fnHLTOutSize(0.)
  , fhltRatio(0.)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
}

AliHLTGlobalPromptRecoQAComponent::~AliHLTGlobalPromptRecoQAComponent()
{
  // see header file for class documentation
}

int AliHLTGlobalPromptRecoQAComponent::ProcessOption(TString option, TString value)
{
  // see header file for class documentation
  int iResult=0;

  if (option.BeginsWith("#")) return 0;
  if (option.BeginsWith("//")) return 0;

  if (option.EqualTo("reset")) {
    HLTImportant("received RESET, destroying histograms");
    Reset();
  } else if (option.EqualTo("resetIncludingDownstream")) {
    HLTImportant("received RESET, destroying histograms");
    Reset(true);
  } else if (option.EqualTo("skip-events")) {
    fSkipEvents = value.Atoi();
  } else if (option.EqualTo("axis")) {
    NewAxis(value.Data());
  } else if (option.EqualTo("histogram")) {
    NewHistogram(value.Data());
  }	else if (option.EqualTo("print-stats")) {
    fPrintStats = 1;
  }	else if (option.EqualTo("print-stats-verbose")) {
    fPrintStats = 2;
  }	else if (option.EqualTo("print-stats-downscale")) {
    fPrintDownscale = atoi(value);
  } else if (option.EqualTo("PushEmptyHistograms")) {
    fPushEmptyHistograms=kTRUE;
  } else if (option.EqualTo("pushback-period")) {
  } else {
    HLTError("invalid option: %s", value.Data());
    return -EINVAL;
  }  
  return iResult;
}

int AliHLTGlobalPromptRecoQAComponent::Reset(bool resetDownstream)
{
  int rc = 0;
  //reset the histograms
  for (std::map<string,histStruct>::iterator i=fHistograms.begin(); i!=fHistograms.end(); ++i)
  {
    delete i->second.hist;
  }
  fHistograms.clear();
  //reset axes
  for (std::map<string,axisStruct>::iterator i=fAxes.begin(); i!=fAxes.end(); ++i)
  {
    i->second.histograms.clear();
  }
  
  if (resetDownstream) 
  {
    rc = PushBack("reset", kAliHLTDataTypeConfig|kAliHLTDataOriginHLT);
  }
  return 0;
}

int AliHLTGlobalPromptRecoQAComponent::Reconfigure(const char* cdbEntry, const char* chainId)
{
  // see header file for class documentation
  int iResult=0;
  const char* path=NULL;
  const char* defaultNotify="";
  if (cdbEntry) {
    path=cdbEntry;
    defaultNotify=" (default)";
  }
  if (path) {
    HLTInfo("reconfigure from entry %s%s, chain id %s", path, defaultNotify,(chainId!=NULL && chainId[0]!=0)?chainId:"<none>");
    AliCDBEntry *pEntry = AliCDBManager::Instance()->Get(path/*,GetRunNo()*/);
    if (pEntry) {
      TObjString* pString=dynamic_cast<TObjString*>(pEntry->GetObject());
      if (pString) {
	HLTInfo("received configuration object string: \'%s\'", pString->String().Data());
	iResult=ProcessOptionString(pString->String());
      } else {
	HLTError("configuration object \"%s\" has wrong type, required TObjString", path);
      }
    } else {
      HLTError("can not fetch object \"%s\" from CDB", path);
    }
  }
  
  return iResult;
}

void AliHLTGlobalPromptRecoQAComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  // see header file for class documentation
  list.push_back(kAliHLTDataTypeClusters | kAliHLTDataOriginITSSPD);
  list.push_back(kAliHLTDataTypeClusters | kAliHLTDataOriginITSSDD);
  list.push_back(kAliHLTDataTypeClusters | kAliHLTDataOriginITSSSD);
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSPD);
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSDD);
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSSD);
  
  list.push_back(kAliHLTDataTypeITSSAPData | kAliHLTDataOriginITS);
  list.push_back(kAliHLTDataTypeESDVertex | kAliHLTDataOriginITSSPD); //SPD Vertex

  //config
  list.push_back(kAliHLTDataTypeConfig);
  
  list.push_back(AliHLTTPCDefinitions::fgkHWClustersDataType | kAliHLTDataOriginTPC); //HLT-TPC clusters from HWCF
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC); //TPC DDL raw data
  list.push_back(AliHLTTPCDefinitions::fgkClustersDataType | kAliHLTDataOriginTPC); //Transformed HLT-TPC clusters
  list.push_back(AliHLTTPCCADefinitions::fgkCompressedInputDataType | kAliHLTDataOriginTPC); //Transformed HLT-TPC clusters (internally compressed form)
  list.push_back(AliHLTTPCDefinitions::fgkRawClustersDataType | kAliHLTDataOriginTPC); //Non-transformed HLT-TPC clusters
  
  list.push_back(AliHLTTPCCADefinitions::fgkTrackletsDataType); //HLT-TPC Tracklets (before TPC global merger)
  list.push_back(kAliHLTDataTypeTrack | kAliHLTDataOriginTPC); //HLT-TPC merged tracks
  list.push_back(kAliHLTDataTypeTrack | kAliHLTDataOriginITS); //TPC-ITS tracks
  list.push_back(kAliHLTDataTypeTrack | kAliHLTDataOriginITSOut); //ITS-Out merged tracks
  
  list.push_back(kAliHLTDataTypeESDContent | kAliHLTDataOriginVZERO); //VZERO-RECO output
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginVZERO);
  list.push_back(kAliHLTDataTypeESDContent | kAliHLTDataOriginZDC); //ZDC-RECO output
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginZDC);
  
  list.push_back(AliHLTEMCALDefinitions::fgkTriggerPatchDataType); //EMCAL-RECO output
  list.push_back(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginEMCAL);
  list.push_back(AliHLTEMCALDefinitions::fgkTriggerSTUDataType); //STU
  list.push_back(AliHLTEMCALDefinitions::fgkTriggerRawDigitDataType); //TRU
  
  list.push_back(kAliHLTDataTypeESDObject|kAliHLTDataOriginOut);
  list.push_back(kAliHLTDataTypeESDfriendObject|kAliHLTDataOriginOut);
  list.push_back(kAliHLTDataTypeFlatESD|kAliHLTDataOriginOut);
  list.push_back(kAliHLTDataTypeFlatESDFriend|kAliHLTDataOriginOut);

  //All this is TPC Data compression
  list.push_back(AliHLTTPCDefinitions::DataCompressionDescriptorDataType());
  list.push_back(AliHLTTPCDefinitions::RawClustersDataTypeNotCompressed());
  list.push_back(AliHLTTPCDefinitions::RemainingClustersCompressedDataType());
  list.push_back(AliHLTTPCDefinitions::RemainingClusterIdsDataType());
  list.push_back(AliHLTTPCDefinitions::ClusterTracksCompressedDataType());
  list.push_back(AliHLTTPCDefinitions::ClusterIdTracksDataType());
}

AliHLTComponentDataType AliHLTGlobalPromptRecoQAComponent::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTMultipleDataType;
}

int AliHLTGlobalPromptRecoQAComponent::GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList){ 
// see header file for class documentation

  tgtList.clear();
  tgtList.push_back( kAliHLTDataTypeHistogram|kAliHLTDataOriginOut );
  tgtList.push_back( kAliHLTDataTypeTObject|kAliHLTDataOriginOut );
  return tgtList.size();
}

void AliHLTGlobalPromptRecoQAComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  // see header file for class documentation
  constBase=2000000;
  inputMultiplier=0.0;
}

int AliHLTGlobalPromptRecoQAComponent::DoInit(int argc, const char** argv)
{
  // see header file for class documentation
  int iResult=0;

  //Init the CTP data
  if (SetupCTPData() == -ENOMEM) 
  {
    HLTError("could not SetupCTPData(); ENOMEM");
    return -ENOMEM;
  }

  AliGRPManager mgr;
  mgr.ReadGRPEntry();
  
  if (mgr.GetGRPData()->GetBeamType() == "Pb-Pb" ||
      mgr.GetGRPData()->GetBeamType() == "PbPb" ||
      mgr.GetGRPData()->GetBeamType() == "A-A" ||
      mgr.GetGRPData()->GetBeamType() == "AA" )
  {
    fAxes["nClustersSPD"].set( 100, 0., 30e3,  &fnClustersSPD );
    fAxes["rawSizeSPD"].set( 100, 0., 140e3, &frawSizeSPD );
    fAxes["nClustersSDD"].set( 100, 0., 26e3,  &fnClustersSDD );
    fAxes["rawSizeSDD"].set( 100, 0., 700e3, &frawSizeSDD );
    fAxes["nClustersSSD"].set( 100, 0., 31e3,  &fnClustersSSD );
    fAxes["rawSizeSSD"].set( 100, 0., 700e3, &frawSizeSSD );
    fAxes["nClustersITS"].set( 100, 0., 100e3, &fnClustersITS );
    fAxes["rawSizeITS"].set( 100, 0., 1e6, &frawSizeITS );
    fAxes["rawSizeVZERO"].set( 100, 0., 6e3, &frawSizeVZERO );
    fAxes["rawSizeEMCAL"].set( 100, 0., 100e3, &frawSizeEMCAL );
    fAxes["rawSizeZDC"].set( 100, 0., 100e3, &frawSizeZDC );
    fAxes["nClustersTPC"].set( 100, 0., 600e3, &fnClustersTPC );
    fAxes["rawSizeTPC"].set( 100, 0., 185e6, &frawSizeTPC );
    fAxes["hwcfSizeTPC"].set( 100, 0., 185e6, &fhwcfSizeTPC );
    fAxes["clusterSizeTPCtransformed"].set( 100, 0., 1., &fclusterSizeTPCtransformed );
    fAxes["clusterSizeTPC"].set( 100, 0., 6500e3, &fclusterSizeTPC );
    fAxes["compressedSizeTPC"].set( 100, 0., 50e6, &fcompressedSizeTPC );
    fAxes["nITSSAPtracks"].set( 100, 0., 2e3, &fnITSSAPtracks );
    fAxes["nTPCtracklets"].set( 100, 0., 60e3, &fnTPCtracklets );
    fAxes["nTPCtracks"].set( 100, 0., 40e3, &fnTPCtracks );
    fAxes["nITSTracks"].set( 100, 0., 1., &fnITSTracks );
    fAxes["nITSOutTracks"].set( 100, 0., 1., &fnITSOutTracks );
    fAxes["vZEROMultiplicity"].set( 100, 0., 60e3, &fvZEROMultiplicity );
    fAxes["vZEROTriggerChargeA"].set( 100, 0., 30e3, &fvZEROTriggerChargeA );
    fAxes["vZEROTriggerChargeC"].set( 100, 0., 30e3, &fvZEROTriggerChargeC );
    fAxes["vZEROTriggerChargeAC"].set( 100, 0., 60e3, &fvZEROTriggerChargeAC );
    fAxes["zdcZNC"].set( 100, 0., 20e3, &fzdcZNC );
    fAxes["zdcZNA"].set( 100, 0., 20e3, &fzdcZNA );
    fAxes["zdcZNAC"].set( 100, 0., 40e3, &fzdcZNAC );
    fAxes["zdcRecoSize"].set( 100, 0., 1., &fzdcRecoSize );
    fAxes["emcalRecoSize"].set( 100, 0., 1., &femcalRecoSize );
    fAxes["emcalTRU"].set( 100, 0., 1., &femcalTRU );
    fAxes["emcalSTU"].set( 100, 0., 1., &femcalSTU );
    fAxes["compressionRatio"].set( 100, 0., 8., &fcompressionRatio );
    fAxes["compressionRatioFull"].set( 100, 0., 12., &fcompressionRatioFull );
    fAxes["hltRatio"].set( 100, 0., 1., &fhltRatio );
    fAxes["nESDSize"].set( 100, 0., 1., &fnESDSize );
    fAxes["nESDFriendSize"].set( 100, 0., 1., &fnESDFriendSize );
    fAxes["nFlatESDSize"].set( 100, 0., 1., &fnFlatESDSize );
    fAxes["nFlatESDFriendSize"].set( 100, 0., 1., &fnFlatESDFriendSize );
    fAxes["nHLTInSize"].set( 100, 0., 200e6, &fnHLTInSize );
    fAxes["nHLTOutSize"].set( 100, 0., 70e6, &fnHLTOutSize );

  }
  else
  {
    fAxes["nClustersSPD"].set( 100, 0., 500.,  &fnClustersSPD );
    fAxes["rawSizeSPD"].set( 100, 0., 10e3, &frawSizeSPD );
    fAxes["nClustersSDD"].set( 100, 0., 500.,  &fnClustersSDD );
    fAxes["rawSizeSDD"].set( 100, 0., 50e3, &frawSizeSDD );
    fAxes["nClustersSSD"].set( 100, 0., 600.,  &fnClustersSSD );
    fAxes["rawSizeSSD"].set( 100, 0., 8e3, &frawSizeSSD );
    fAxes["nClustersITS"].set( 100, 0., 10e3, &fnClustersITS );
    fAxes["rawSizeITS"].set( 100, 0., 100e3, &frawSizeITS );
    fAxes["rawSizeVZERO"].set( 100, 0., 6e3, &frawSizeVZERO );
    fAxes["rawSizeEMCAL"].set( 100, 0., 100e3, &frawSizeEMCAL );
    fAxes["rawSizeZDC"].set( 100, 0., 10e3, &frawSizeZDC );
    fAxes["nClustersTPC"].set( 100, 0., 20e3, &fnClustersTPC );
    fAxes["rawSizeTPC"].set( 100, 0., 5e6, &frawSizeTPC );
    fAxes["hwcfSizeTPC"].set( 100, 0., 1., &fhwcfSizeTPC );
    fAxes["clusterSizeTPCtransformed"].set( 100, 0., 1., &fclusterSizeTPCtransformed );
    fAxes["clusterSizeTPC"].set( 100, 0., 6500e3, &fclusterSizeTPC );
    fAxes["compressedSizeTPC"].set( 100, 0., 1500e3, &fcompressedSizeTPC );
    fAxes["nITSSAPtracks"].set( 100, 0., 100., &fnITSSAPtracks );
    fAxes["nTPCtracklets"].set( 100, 0., 1e3, &fnTPCtracklets );
    fAxes["nTPCtracks"].set( 100, 0., 700., &fnTPCtracks );
    fAxes["nITSTracks"].set( 100, 0., 1., &fnITSTracks );
    fAxes["nITSOutTracks"].set( 100, 0., 1., &fnITSOutTracks );
    fAxes["vZEROMultiplicity"].set( 100, 0., 40e3, &fvZEROMultiplicity );
    fAxes["vZEROTriggerChargeA"].set( 100, 0., 2e3, &fvZEROTriggerChargeA );
    fAxes["vZEROTriggerChargeC"].set( 100, 0., 2e3, &fvZEROTriggerChargeC );
    fAxes["vZEROTriggerChargeAC"].set( 100, 0., 4e3, &fvZEROTriggerChargeAC );
    fAxes["zdcZNC"].set( 100, 0., 500., &fzdcZNC );
    fAxes["zdcZNA"].set( 100, 0., 500., &fzdcZNA );
    fAxes["zdcZNAC"].set( 100, 0., 1e3, &fzdcZNAC );
    fAxes["zdcRecoSize"].set( 100, 0., 1., &fzdcRecoSize );
    fAxes["emcalRecoSize"].set( 100, 0., 1., &femcalRecoSize );
    fAxes["emcalTRU"].set( 100, 0., 1., &femcalTRU );
    fAxes["emcalSTU"].set( 100, 0., 1., &femcalSTU );
    fAxes["compressionRatio"].set( 100, 0., 8., &fcompressionRatio );
    fAxes["compressionRatioFull"].set( 100, 0., 12., &fcompressionRatioFull );
    fAxes["hltRatio"].set( 100, 0., 1., &fhltRatio );
    fAxes["nESDSize"].set( 100, 0., 1., &fnESDSize );
    fAxes["nESDFriendSize"].set( 100, 0., 1., &fnESDFriendSize );
    fAxes["nFlatESDSize"].set( 100, 0., 1., &fnFlatESDSize );
    fAxes["nFlatESDFriendSize"].set( 100, 0., 1., &fnFlatESDFriendSize );
    fAxes["nHLTInSize"].set( 100, 0., 6e6, &fnHLTInSize );
    fAxes["nHLTOutSize"].set( 100, 0., 2e6, &fnHLTOutSize );
  }

  NewHistogram(",fHistSPDclusters_SPDrawSize,SPD clusters vs SPD raw size,rawSizeSPD,nClustersSPD");
  NewHistogram(",fHistSDDclusters_SDDrawSize,SDD clusters vs SDD raw size,rawSizeSDD,nClustersSDD");
  NewHistogram(",fHistSSDclusters_SSDrawSize,SSD clusters vs SSD raw size,rawSizeSSD,nClustersSSD");
  NewHistogram(",fHistITSSAtracks_SPDclusters,ITSSAP tracks vs SPD clusters,nClustersSPD,nITSSAPtracks");
  NewHistogram(",fHistSPDclusters_SSDclusters,SSD clusters vs SPD clusters,nClustersSPD,nClustersSSD");
  NewHistogram(",fHistSPDclusters_SDDclusters,SDD clusters vs SPD clusters,nClustersSPD,nClustersSDD");
  NewHistogram(",fHistSSDclusters_SDDclusters,SDD clusters vs SSD clusters,nClustersSSD,nClustersSDD");
  NewHistogram(",fHistTPCtracks_TPCtracklets,TPC Tracks vs TPC Tracklets,nTPCtracklets,nTPCtracks");
  NewHistogram(",fHistTPCClusterSize_TPCCompressedSize,TPC compressed size vs TPC HWCF Size,hwcfSizeTPC,compressedSizeTPC");
  NewHistogram(",fHistTPCRawSize_TPCCompressedSize,TPC compressed size vs TPC Raw Size,rawSizeTPC,compressedSizeTPC");
  NewHistogram(",fHistTPCHLTclusters_TPCCompressionRatio,Huffman compression ratio vs TPC HLT clusters,nClustersTPC,compressionRatio");
  NewHistogram(",fHistTPCHLTclusters_TPCFullCompressionRatio,Full compression ratio vs TPC HLT clusters,nClustersTPC,compressionRatioFull");
  NewHistogram(",fHistHLTInSize_HLTOutSize,HLT Out Size vs HLT In Size,nHLTInSize,nHLTOutSize");
  NewHistogram(",fHistHLTSize_HLTInOutRatio,HLT Out/In Size Ratio vs HLT Input Size,nHLTInSize,hltRatio");
  NewHistogram(",fHistZNA_VZEROTrigChargeA,ZNA vs. VZERO Trigger Charge A,vZEROTriggerChargeA,zdcZNA");
  NewHistogram(",fHistZNC_VZEROTrigChargeC,ZNC vs. VZERO Trigger Charge C,vZEROTriggerChargeC,zdcZNC");
  NewHistogram(",fHistZNT_VZEROTrigChargeT,ZN (A+C) vs. VZERO Trigger Charge (A+C),vZEROTriggerChargeAC,zdcZNAC");
  NewHistogram(",fHistVZERO_SPDClusters,SPD Clusters vs VZERO Trigger Charge (A+C),vZEROTriggerChargeAC,nClustersSPD");
  NewHistogram(",fHistVZERO_ITSSAPTracks,ITS SAP Tracks vs VZERO Trigger Charge (A+C),vZEROTriggerChargeAC,nITSSAPtracks");

  NewHistogram("MUFAST,fHistVZERO_ITSSAPTracks,ITS SAP Tracks vs VZERO Trigger Charge (A+C),vZEROTriggerChargeAC,nITSSAPtracks");
  NewHistogram("CINT7,fHistVZERO_ITSSAPTracks,ITS SAP Tracks vs VZERO Trigger Charge (A+C),vZEROTriggerChargeAC,nITSSAPtracks");
  NewHistogram("MUFAST,fHistITSSAtracks_SPDclusters,ITSSAP tracks vs SPD clusters,nClustersSPD,nITSSAPtracks");
  NewHistogram("CINT7,fHistITSSAtracks_SPDclusters,ITSSAP tracks vs SPD clusters,nClustersSPD,nITSSAPtracks");

  fpHWCFData = new AliHLTTPCHWCFData;
  
  setlocale(LC_NUMERIC, ""); //Make printf with 1000 separators work

  //parse the config string AFTER the defaults are set
  if (ProcessOptionString(GetComponentArgs())<0) 
  {
    HLTFatal("wrong config string! %s", GetComponentArgs().c_str());
    return -EINVAL;
  }

  return iResult;
}

void AliHLTGlobalPromptRecoQAComponent::NewHistogram(std::string histConfig)
{
  //tokenize string
  std::vector<string> tokens;
  std::string delimiter = ",\n\r";
  size_t  start = 0, end = 0;
  while ( end != string::npos)
  {
    end = histConfig.find_first_of(delimiter, start);
    string token = histConfig.substr( 
        start,(end == string::npos) ? string::npos : end - start );
    size_t tokenStartPos = token.find_first_not_of(" \t\n");
    size_t tokenEndPos = token.find_last_not_of(" \t\n");
    if (tokenStartPos!=std::string::npos && tokenEndPos!=std::string::npos)
    { token = token.substr(tokenStartPos,tokenEndPos-tokenStartPos+1); }
    tokens.push_back(token);
    start = (( end > (string::npos - 1) )
        ?  string::npos  :  end + 1);
  }
  if (tokens.size()==5)
  {
    NewHistogram(tokens[0],tokens[1],tokens[2],tokens[3],tokens[4], histConfig);
  }
  else
  {
    HLTWarning("histogram token string should contain trigName,histName,histTitle,x,y (%s)", 
        histConfig.c_str());
  }

}

void AliHLTGlobalPromptRecoQAComponent::NewHistogram(string trigName, string histName, string histTitle, string xname, string yname, string config )
{
  //some sanity checks
  if (histTitle.size()==0)
  {
    HLTWarning("histogram title cannot be empty!");
    return;
  }
  if (xname.empty() && yname.empty())
  {
    HLTWarning("both axis names empty! at least one needed (%s)", config.c_str());
    return;
  }

  if (histName.empty()) histName=histTitle;
  if (!trigName.empty()) histName=histName+"_"+trigName;
  if (!trigName.empty()) histTitle=histTitle+" <"+trigName+">";

  //get ref to the old histogram (if any)
  histStruct& hist = fHistograms[histName];

  if (fHistograms.size()>100)
  {
    HLTWarning("ignoring hist %s, too many histograms", histName.c_str());
    return;
  }

  axisStruct x = fAxes[xname];
  axisStruct y = fAxes[yname];
  if (!x.value && !xname.empty()) 
  {
    HLTWarning("empty variable %s",xname.c_str());
    return;
  }
  if (!y.value && !yname.empty()) 
  {
    HLTWarning("empty variable %s",yname.c_str());
    return;
  }
  delete hist.hist;
  if (!xname.empty() && !yname.empty())
  {
    //both axes specified, TH2
    hist.hist = new TH2F(histName.c_str(), histTitle.c_str(), x.bins, x.low, x.high, y.bins, y.low, y.high);
  } 
  else
  {
    //only one axis specified (the case of both axes empty is excluded above)
    axisStruct* ax = &x;
    if (xname.empty()) ax=&y;
    hist.hist = new TH1F(histName.c_str(), histTitle.c_str(), (*ax).bins, (*ax).low, (*ax).high);
  }
  hist.x = x;
  hist.y = y;
  hist.trigger = trigName;
  hist.config=config;
  //register which axes we're using
  fAxes[xname].histograms[histName]=true;
  fAxes[yname].histograms[histName]=true;
}

void AliHLTGlobalPromptRecoQAComponent::NewAxis(string config)
{
  std::vector<string> tokens;
  std::string delimiter = ",\n\r";
  size_t  start = 0, end = 0;
  while ( end != string::npos)
  {
    end = config.find_first_of(delimiter, start);
    string token = config.substr( 
        start,(end == string::npos) ? string::npos : end - start );
    size_t tokenStartPos = token.find_first_not_of(" \t\n");
    size_t tokenEndPos = token.find_last_not_of(" \t\n");
    if (tokenStartPos!=std::string::npos && tokenEndPos!=std::string::npos)
    { token = token.substr(tokenStartPos,tokenEndPos-tokenStartPos+1); }
    tokens.push_back(token);
    start = (( end > (string::npos - 1) )
        ?  string::npos  :  end + 1);
  }
  if (tokens.size()==4)
  {
    NewAxis(tokens[0],atoi(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()));
  }
  else
  {
    HLTWarning("axis token string should contain varName,nbins,low,high (%s)", 
        config.c_str());
  }
}

void AliHLTGlobalPromptRecoQAComponent::NewAxis(string name, int bins, float low, float high)
{
  if (bins>200)
  {
    HLTWarning("%i is too many bins for %s, setting max=200",bins,name.c_str());
    bins=200;
  }
  fAxes[name].bins=bins;
  fAxes[name].low=low;
  fAxes[name].high=high;
  //reinitialize the histograms that use this axis
  for (std::map<std::string,bool>::iterator i=fAxes[name].histograms.begin(); i!=fAxes[name].histograms.end(); ++i)
  {
    NewHistogram(fHistograms[i->first].config);
  }
}

int AliHLTGlobalPromptRecoQAComponent::DoDeinit()
{
  // see header file for class documentation
  
  delete fpHWCFData;
  return 0;
}

int AliHLTGlobalPromptRecoQAComponent::FillHistograms()
{
  int nPushedHistograms=0;
  for (std::map<string,histStruct>::iterator i=fHistograms.begin(); i!=fHistograms.end(); ++i)
  {
    histStruct hist = i->second;
    if (!hist.hist) {HLTInfo("no histo"); continue;}

    const AliHLTCTPData* ctp = CTPData();
    if (!ctp) {HLTWarning("no CTPData!"); return 0;}
    bool triggerMatched = ctp->MatchTriggerRE(hist.trigger.c_str());


    if ( triggerMatched && 
         ((hist.Fill() > 0) || fPushEmptyHistograms) && 
         PushBack(hist.hist, kAliHLTDataTypeHistogram|kAliHLTDataOriginHLT) > 0 )
    {
      nPushedHistograms++;
      hist.hist->Reset();
    }
  }
  return nPushedHistograms;
}

int histStruct::Fill()
{
  if ((x.value && y.value) && (*x.value * *y.value)!=0 )
  {
    if (!hist) return 0;
    hist->Fill(*x.value, *y.value);
    return 1;
  }
  return 0;
}

int AliHLTGlobalPromptRecoQAComponent::DoEvent( const AliHLTComponentEventData& evtData,
    const AliHLTComponentBlockData* blocks, 
    AliHLTComponentTriggerData& /*trigData*/,
    AliHLTUInt8_t* /*outputPtr*/, 
    AliHLTUInt32_t& /*size*/,
    AliHLTComponentBlockDataList& /*outputBlocks*/ )
{
  // see header file for class documentation
  int iResult=0;

  if (!IsDataEvent()) {return iResult;}

  //perhaps downscale a bit
  if (fEventsSinceSkip++<fSkipEvents)
  {
    fEventsSinceSkip=0;
    return 0;
  }

  //what are we interested in?
  AliHLTUInt32_t nClustersSPD = 0;
  AliHLTUInt32_t rawSizeSPD = 0;
  AliHLTUInt32_t nClustersSDD = 0;
  AliHLTUInt32_t rawSizeSDD = 0;
  AliHLTUInt32_t nClustersSSD = 0;
  AliHLTUInt32_t rawSizeSSD = 0;
  AliHLTUInt32_t nClustersITS = 0;
  AliHLTUInt32_t rawSizeITS = 0;
  AliHLTUInt32_t rawSizeVZERO = 0;
  AliHLTUInt32_t rawSizeEMCAL = 0;
  AliHLTUInt32_t rawSizeZDC = 0;
  
  AliHLTUInt32_t nClustersTPC = 0;
  AliHLTUInt32_t rawSizeTPC = 0;
  AliHLTUInt32_t hwcfSizeTPC = 0;
  AliHLTUInt32_t clusterSizeTPCtransformed = 0;
  AliHLTUInt32_t clusterSizeTPC = 0;
  AliHLTUInt32_t compressedSizeTPC = 0;

  AliHLTUInt32_t nITSSAPtracks = 0;
  AliHLTUInt32_t nTPCtracklets = 0;
  AliHLTUInt32_t nTPCtracks = 0;
  AliHLTUInt32_t nITSTracks = 0;
  AliHLTUInt32_t nITSOutTracks = 0;

  float vZEROMultiplicity = 0.;
  UShort_t vZEROTriggerChargeA = 0;
  UShort_t vZEROTriggerChargeC = 0;

  Double_t zdcZNC = 0.;
  Double_t zdcZNA = 0.;

  int zdcRecoSize = 0;
  int emcalRecoSize = 0;
  int emcalTRU = 0;
  int emcalSTU = 0;
  
  static int nEvents = 0;
  
  Bool_t bITSSPDVertex = kFALSE;
  
  float compressionRatio, compressionRatioFull;
  
  AliHLTUInt32_t nESDSize = 0;
  AliHLTUInt32_t nESDFriendSize = 0;
  AliHLTUInt32_t nFlatESDSize = 0;
  AliHLTUInt32_t nFlatESDFriendSize = 0;
  
  AliHLTUInt32_t nHLTInSize = 0;
  AliHLTUInt32_t nHLTOutSize = 0;
  float hltRatio;

  nEvents++;

  //loop over input blocks and extract basic stats
  int nBlocks = evtData.fBlockCnt;  
  for (int ndx=0; ndx<nBlocks; ndx++) {
    const AliHLTComponentBlockData* iter = blocks+ndx;
   
    //reconfigure on request
    if (iter->fDataType == kAliHLTDataTypeConfig)
    {
      char* configCharArr = reinterpret_cast<char*>(iter->fPtr);
      string configString;
      configString.assign(configCharArr,iter->fSize);
      HLTImportant("reconfiguring with: %s", configString.c_str());
      ProcessOptionString(configString.c_str());
    }

    //SPD Vertex Found
    if (iter->fDataType == (kAliHLTDataTypeESDVertex | kAliHLTDataOriginITSSPD))
    {
      bITSSPDVertex = kTRUE;
    }

    //numbers of clusters
    if (iter->fDataType == (kAliHLTDataTypeClusters | kAliHLTDataOriginITSSPD))
    {
      AliHLTITSClusterData *inPtr=reinterpret_cast<AliHLTITSClusterData*>( iter->fPtr );
      nClustersSPD += inPtr->fSpacePointCnt;
    }
    if (iter->fDataType == (kAliHLTDataTypeClusters | kAliHLTDataOriginITSSDD))
    {
      AliHLTITSClusterData *inPtr=reinterpret_cast<AliHLTITSClusterData*>( iter->fPtr );
      nClustersSDD += inPtr->fSpacePointCnt;
    }
    if (iter->fDataType == (kAliHLTDataTypeClusters | kAliHLTDataOriginITSSSD))
    {
      AliHLTITSClusterData *inPtr=reinterpret_cast<AliHLTITSClusterData*>( iter->fPtr );
      nClustersSSD += inPtr->fSpacePointCnt;
    }
    if (iter->fDataType == (kAliHLTDataTypeClusters | kAliHLTDataOriginITS))
    {
      AliHLTITSClusterData *inPtr=reinterpret_cast<AliHLTITSClusterData*>( iter->fPtr );
      nClustersITS += inPtr->fSpacePointCnt;
    }

    if (iter->fDataType == AliHLTTPCDefinitions::fgkClustersDataType) //Transformed TPC clusters used in TPCCATracker
    {
      AliHLTTPCClusterData* inPtrSP = ( AliHLTTPCClusterData* )( iter->fPtr );
      nClustersTPC += inPtrSP->fSpacePointCnt;
    }
    else if (iter->fDataType == AliHLTTPCCADefinitions::fgkCompressedInputDataType) //Compressed (internally) form of transformed HLT TPC clusters (currently not used)
    {
      const AliHLTUInt8_t * inPtr = (const AliHLTUInt8_t *)iter->fPtr;
      while(inPtr< ((const AliHLTUInt8_t *) iter->fPtr) + iter->fSize)
      {
        AliHLTTPCCACompressedClusterRow *row = (AliHLTTPCCACompressedClusterRow*) inPtr;
        nClustersTPC+= row->fNClusters;
        inPtr = (const AliHLTUInt8_t *)(row->fClusters+row->fNClusters);
      }
    }

    if (iter->fDataType == AliHLTTPCDefinitions::DataCompressionDescriptorDataType() ||
      iter->fDataType == AliHLTTPCDefinitions::RawClustersDataTypeNotCompressed() ||
      iter->fDataType == AliHLTTPCDefinitions::RemainingClustersCompressedDataType() ||
      iter->fDataType == AliHLTTPCDefinitions::RemainingClusterIdsDataType() ||
      iter->fDataType == AliHLTTPCDefinitions::ClusterTracksCompressedDataType() ||
      iter->fDataType == AliHLTTPCDefinitions::ClusterIdTracksDataType())
    {
      compressedSizeTPC += iter->fSize;
    }
    
    //VZERO Multiplicity
    if (iter->fDataType == (kAliHLTDataTypeESDContent | kAliHLTDataOriginVZERO))
    {
      const TObject* o = GetInputObjectFromIndex(ndx);
      if (o)
      {
        const AliESDVZERO* esdVZERO = dynamic_cast<const AliESDVZERO*>(o);
        if (esdVZERO)
        {
          for (int i = 0;i < 64;i++) vZEROMultiplicity += esdVZERO->GetMultiplicity(i);
          vZEROTriggerChargeA = esdVZERO->GetTriggerChargeA();
          vZEROTriggerChargeC = esdVZERO->GetTriggerChargeC();
        }
      }
    }

    //EMCAL Reco Size
    if (iter->fDataType == AliHLTEMCALDefinitions::fgkTriggerPatchDataType)
    {
      emcalRecoSize += iter->fSize;
    }
    if (iter->fDataType == AliHLTEMCALDefinitions::fgkTriggerSTUDataType)
    {
      emcalSTU += iter->fSize;
    }
    if (iter->fDataType == AliHLTEMCALDefinitions::fgkTriggerRawDigitDataType)
    {
      emcalTRU += iter->fSize;
    }

    //ZDC tower energies
    //ZDC Reco size
    if (iter->fDataType == (kAliHLTDataTypeESDContent | kAliHLTDataOriginZDC))
    {
      zdcRecoSize += iter->fSize;
      const TObject* o = GetInputObjectFromIndex(ndx);
      const AliESDZDC* esdZDC = dynamic_cast<const AliESDZDC*>(o);
      if (esdZDC)
      {
        zdcZNC = esdZDC->GetZNCTowerEnergy()[0];
        zdcZNA = esdZDC->GetZNATowerEnergy()[0];
      }
    }

    //RAW sizes
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSPD))
    {
      rawSizeSPD += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSDD))
    {
      rawSizeSDD += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITSSSD))
    {
      rawSizeSSD += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginITS))
    {
      rawSizeITS += iter->fSize;
    }
    if (iter->fDataType == (AliHLTTPCDefinitions::fgkHWClustersDataType | kAliHLTDataOriginTPC)) //Size of HLT-TPC hardware clusters
    {
      hwcfSizeTPC += iter->fSize;
      AliHLTCDHWrapper header(iter->fPtr);
      AliHLTUInt8_t* pData = reinterpret_cast<AliHLTUInt8_t*>(iter->fPtr);
      pData+=header.GetHeaderSize();
      int rc = fpHWCFData->Init(pData, iter->fSize-header.GetHeaderSize());
      if (rc>=0)
      {
        const AliHLTUInt32_t* pRCUTrailer = reinterpret_cast<const AliHLTUInt32_t*>(fpHWCFData->GetRCUTrailer());
        AliHLTUInt32_t payloadSize = (*pRCUTrailer) & 0x00ffffff;
        rawSizeTPC += header.GetHeaderSize() + payloadSize * sizeof(AliHLTUInt32_t) + fpHWCFData->GetRCUTrailerSize();
      }
    }
    if (iter->fDataType == (AliHLTTPCDefinitions::fgkClustersDataType | kAliHLTDataOriginTPC)) //Size of transformed HLT-TPC clusters
    {
      clusterSizeTPCtransformed += iter->fSize;
    }
    if (iter->fDataType == (AliHLTTPCDefinitions::fgkRawClustersDataType | kAliHLTDataOriginTPC)) //Size of HLT-TPC clusters (uncompressed from HWCF, not yet transformed)
    {
      clusterSizeTPC += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC)) //TPC RAW DDL Size
    {
      rawSizeTPC += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginVZERO))
    {
      rawSizeVZERO += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginZDC))
    {
      rawSizeZDC += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginEMCAL))
    {
      rawSizeEMCAL += iter->fSize;
    }

    //esd size
    if (iter->fDataType == (kAliHLTDataTypeESDObject|kAliHLTDataOriginOut))
    {
      nESDSize += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeESDfriendObject|kAliHLTDataOriginOut))
    {
      nESDFriendSize += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeFlatESD|kAliHLTDataOriginOut))
    {
      nFlatESDSize += iter->fSize;
    }
    if (iter->fDataType == (kAliHLTDataTypeFlatESDFriend|kAliHLTDataOriginOut))
    {
      nFlatESDFriendSize += iter->fSize;
    }

    //numbers of tracks
    if (iter->fDataType == AliHLTTPCCADefinitions::fgkTrackletsDataType) //HLT-TPC CA-trackets (before TPC global merger)
    {
      AliHLTTPCCASliceOutput* out = reinterpret_cast<AliHLTTPCCASliceOutput*>(iter->fPtr);
      nTPCtracklets += out->NTracks();
    }

    if (iter->fDataType == (kAliHLTDataTypeTrack | kAliHLTDataOriginTPC))
    {
      nTPCtracks += ((AliHLTTracksData*) iter->fPtr)->fCount;
    }

    if (iter->fDataType == (kAliHLTDataTypeTrack | kAliHLTDataOriginITS))
    {
      nITSTracks += ((AliHLTTracksData*) iter->fPtr)->fCount;
    }
    if (iter->fDataType == (kAliHLTDataTypeTrack | kAliHLTDataOriginITSOut))
    {
      nITSOutTracks += ((AliHLTTracksData*) iter->fPtr)->fCount;
    }

    if (iter->fDataType == (kAliHLTDataTypeITSSAPData | kAliHLTDataOriginITS))
    {
      AliHLTITSSAPTrackerDataContainer* inPtr = reinterpret_cast<AliHLTITSSAPTrackerDataContainer*>(iter->fPtr);
      nITSSAPtracks += inPtr->fCount;
    }
  }// end read input blocks

  compressionRatio = compressedSizeTPC > 0 ? ((float) hwcfSizeTPC / (float) compressedSizeTPC) : 0.f;
  compressionRatioFull = compressedSizeTPC > 0 ? ((float) rawSizeTPC / (float) compressedSizeTPC) : 0.f;

  nHLTInSize = rawSizeSPD + rawSizeSSD + rawSizeSDD + rawSizeTPC + rawSizeVZERO + rawSizeEMCAL + rawSizeZDC;
  nHLTOutSize = nESDSize + compressedSizeTPC;
  hltRatio = nHLTInSize > 0 ? ((float) nHLTOutSize / (float) nHLTInSize) : 0.f;

  //convert the numbers fo floats for histograms
  fnClustersSPD = nClustersSPD;
  frawSizeSPD = rawSizeSPD;
  fnClustersSDD = nClustersSDD;
  frawSizeSDD = rawSizeSDD;
  fnClustersSSD = nClustersSSD;
  frawSizeSSD = rawSizeSSD;
  fnClustersITS = nClustersITS;
  frawSizeITS = rawSizeITS;
  frawSizeVZERO = rawSizeVZERO;
  frawSizeEMCAL = rawSizeEMCAL;
  frawSizeZDC = rawSizeZDC;
  fnClustersTPC = nClustersTPC;
  frawSizeTPC = rawSizeTPC;
  fhwcfSizeTPC = hwcfSizeTPC;
  fclusterSizeTPCtransformed = clusterSizeTPCtransformed;
  fclusterSizeTPC = clusterSizeTPC;
  fcompressedSizeTPC = compressedSizeTPC;
  fnITSSAPtracks = nITSSAPtracks;
  fnTPCtracklets = nTPCtracklets;
  fnTPCtracks = nTPCtracks;
  fnITSTracks = nITSTracks;
  fnITSOutTracks = nITSOutTracks;
  fvZEROMultiplicity = vZEROMultiplicity;
  fvZEROTriggerChargeA = vZEROTriggerChargeA;
  fvZEROTriggerChargeC = vZEROTriggerChargeC;
  fvZEROTriggerChargeAC = vZEROTriggerChargeA+vZEROTriggerChargeC;
  fzdcZNC = zdcZNC;
  fzdcZNA = zdcZNA;
  fzdcZNAC = zdcZNA+zdcZNC;
  fzdcRecoSize = zdcRecoSize;
  femcalRecoSize = emcalRecoSize;
  femcalTRU = emcalTRU;
  femcalSTU = emcalSTU;
  fcompressionRatio = compressionRatio;
  fcompressionRatioFull = compressionRatioFull;
  fhltRatio = hltRatio;
  fnESDSize = nESDSize;
  fnESDFriendSize = nESDFriendSize;
  fnFlatESDSize = nFlatESDSize;
  fnFlatESDFriendSize = nFlatESDFriendSize;
  fnHLTInSize = nHLTInSize;
  fnHLTOutSize = nHLTOutSize;

  //Fill the histograms
  int pushed_something = FillHistograms();

  static int nPrinted = 0;
  if (fPrintStats && (fPrintStats == 2 || pushed_something) && nPrinted++ % fPrintDownscale == 0) //Don't print this for every event if we use a pushback period
  {
    HLTImportant("Events %d Blocks %4d: HLT Reco QA Stats: HLTInOut %'d / %'d / %4.1f%%, SPD-Cl %d (%d), SDD-Cl %d (%d), SSD-Cl %d (%d) TPC-Cl %'d (%'d / %'d / %'d / %'d), TPC-Comp %5.3fx / %5.3fx (%'d)"
      ", ITSSAP-Tr %d, TPC-Tr %'d / %'d, ITS-Tr %d / %d, SPD-Ver %d, V0 %6.2f (%d), EMCAL %d (%d / %d / %d), ZDC %d (%d), ESD %'d / %'d (%'d / %'d)",
      nEvents, nBlocks, nHLTInSize, nHLTOutSize, hltRatio * 100, nClustersSPD, rawSizeSPD, nClustersSDD, rawSizeSDD, nClustersSSD, rawSizeSSD, nClustersTPC, rawSizeTPC, hwcfSizeTPC, clusterSizeTPC, clusterSizeTPCtransformed, compressionRatio, compressionRatioFull, compressedSizeTPC,
      nITSSAPtracks, nTPCtracklets, nTPCtracks, nITSTracks, nITSOutTracks, (int) bITSSPDVertex, vZEROMultiplicity, rawSizeVZERO, emcalRecoSize, emcalTRU, emcalSTU, rawSizeEMCAL, zdcRecoSize, rawSizeZDC, nESDSize, nFlatESDSize, nESDFriendSize, nFlatESDFriendSize);
  }

  return iResult;
}

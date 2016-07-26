// $Id$
//****************************************************************************
//* This file is property of and copyright by the ALICE HLT Project          * 
//* ALICE Experiment at CERN, All rights reserved.                           *
//*                                                                          *
//* Primary Authors: Sergey Gorbunov, Torsten Alt                            *
//* Developers:      Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de> *
//*                  Torsten Alt <talt@cern.ch>                              *
//*                  for The ALICE HLT Project.                              *
//*                                                                          *
//* Permission to use, copy, modify and distribute this software and its     *
//* documentation strictly for non-commercial purposes is hereby granted     *
//* without fee, provided that the above copyright notice appears in all     *
//* copies and that both the copyright notice and this permission notice     *
//* appear in the supporting documentation. The authors make no claims       *
//* about the suitability of this software for any purpose. It is            *
//* provided "as is" without express or implied warranty.                    *
//****************************************************************************

//  @file   AliHLTTPCHWCFEmulatorComponent.cxx
//  @author Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de>
//  @author Torsten Alt <talt@cern.ch> 
//  @brief  HLT Component interface for for FPGA ClusterFinder Emulator for TPC
//  @brief  ( see AliHLTTPCHWCFEmulator class )
//  @note


#include "AliHLTTPCHWCFEmulatorComponent.h"

#include "AliHLTTPCDefinitions.h"
#include "AliHLTTPCHWCFDataTypes.h"
#include "AliHLTTPCClusterMCData.h"
#include "AliHLTTPCHWCFData.h"

#include "AliGRPObject.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliHLTCDHWrapper.h"
#include <cstdlib>
#include <cerrno>
#include <memory>
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"


#include <sys/time.h>
#include "TFile.h"

using namespace std;

AliHLTTPCHWCFEmulatorComponent::AliHLTTPCHWCFEmulatorComponent()
  :
  AliHLTProcessor(),
  fDoDeconvTime(0),
  fDoDeconvPad(0),
  fDoMC(0),
  fDoFlowControl(0),
  fDoSinglePadSuppression(0),
  fBypassMerger(0),
  fClusterLowerLimit(0),
  fSingleSeqLimit(0),
  fMergerDistance(0),
  fUseTimeBinWindow(0),
  fUseTimeFollow(0),
  fChargeFluctuation(0),
  fTagDeconvolutedClusters(0),
  fProcessingRCU2Data(0),
  fNoiseSuppression(0),
  fDebug(0),
  fCFSupport(),
  fCFEmulator(),
  fBenchmark("TPCHWClusterFinderEmulator")
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}


AliHLTTPCHWCFEmulatorComponent::AliHLTTPCHWCFEmulatorComponent(const AliHLTTPCHWCFEmulatorComponent&)
  :
  AliHLTProcessor(),
  fDoDeconvTime(0),
  fDoDeconvPad(0),
  fDoMC(0),
  fDoFlowControl(0),
  fDoSinglePadSuppression(0),
  fBypassMerger(0),
  fClusterLowerLimit(0),
  fSingleSeqLimit(0),
  fMergerDistance(0),
  fUseTimeBinWindow(0),
  fUseTimeFollow(0),
  fChargeFluctuation(0),
  fTagDeconvolutedClusters(0),
  fProcessingRCU2Data(0),
  fDebug(0),
  fCFSupport(),
  fCFEmulator(),
  fBenchmark("TPCHWClusterFinderEmulator")
{
  // dummy
}

AliHLTTPCHWCFEmulatorComponent& AliHLTTPCHWCFEmulatorComponent::operator=(const AliHLTTPCHWCFEmulatorComponent&)
{
  // dummy
  return *this;
}

AliHLTTPCHWCFEmulatorComponent::~AliHLTTPCHWCFEmulatorComponent()
{
  // see header file for class documentation
}

// Public functions to implement AliHLTComponent's interface.
// These functions are required for the registration process

const char* AliHLTTPCHWCFEmulatorComponent::GetComponentID()
{
  // see header file for class documentation
  return "TPCHWClusterFinderEmulator";
}

void AliHLTTPCHWCFEmulatorComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  // see header file for class documentation
  list.clear();
  list.push_back( AliHLTTPCDefinitions::fgkUnpackedRawDataType ); 	 
  list.push_back( kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC );
  list.push_back( AliHLTTPCDefinitions::RawClustersDataType() | kAliHLTDataOriginTPC );
  list.push_back( AliHLTTPCDefinitions::RawClustersDescriptorDataType() | kAliHLTDataOriginTPC );
}

AliHLTComponentDataType AliHLTTPCHWCFEmulatorComponent::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTMultipleDataType;
}

int AliHLTTPCHWCFEmulatorComponent::GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList)
{
  // see header file for class documentation
  tgtList.clear();
  tgtList.push_back(AliHLTTPCDefinitions::fgkHWClustersDataType | kAliHLTDataOriginTPC );
  tgtList.push_back(AliHLTTPCDefinitions::fgkAliHLTDataTypeClusterMCInfo | kAliHLTDataOriginTPC );
  tgtList.push_back( AliHLTTPCDefinitions::RawClustersDataType() | kAliHLTDataOriginTPC );
  tgtList.push_back( AliHLTTPCDefinitions::RawClustersDescriptorDataType() | kAliHLTDataOriginTPC );
  return tgtList.size();
}


void AliHLTTPCHWCFEmulatorComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // see header file for class documentation
  // XXX TODO: Find more realistic values.  
  constBase = 0;
  inputMultiplier = (6 * 0.7);
}


AliHLTComponent* AliHLTTPCHWCFEmulatorComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTTPCHWCFEmulatorComponent();
}

void AliHLTTPCHWCFEmulatorComponent::GetOCDBObjectDescription( TMap* const targetMap){
// Get a list of OCDB object description needed for the particular component
  
  if (!targetMap) return;
  
  // OCDB entries for component arguments
  targetMap->Add(new TObjString("HLT/ConfigTPC/TPCHWClusterFinder"), new TObjString("component arguments, empty at the moment"));
}


int AliHLTTPCHWCFEmulatorComponent::DoInit( int argc, const char** argv )
{
  // see header file for class documentation

  TString arguments = "";
  for ( int i = 0; i < argc; i++ ) {
    if ( !arguments.IsNull() ) arguments += " ";
    arguments += argv[i];
  }

  return Configure( NULL, NULL, arguments.Data()  );
}

int AliHLTTPCHWCFEmulatorComponent::Reconfigure( const char* cdbEntry, const char* chainId )
{
  // Reconfigure the component from OCDB

  return Configure( cdbEntry, chainId, NULL );
}

int AliHLTTPCHWCFEmulatorComponent::ScanConfigurationArgument(int argc, const char** argv)
{
  // see header file for class documentation
  TString arguments = "";
  for ( int i = 0; i < argc; i++ ) {
    if ( !arguments.IsNull() ) arguments += " ";
    arguments += argv[i];
  }
  return ReadConfigurationString(arguments);
}



void AliHLTTPCHWCFEmulatorComponent::SetDefaultConfiguration()
{
  // Set default configuration for the FPGA ClusterFinder Emulator component
  // Some parameters can be later overwritten from the OCDB

  fDoDeconvTime = 1;
  fDoDeconvPad = 1;
  fDoMC = 0;
  fDoFlowControl = 0;
  fDoSinglePadSuppression = 0;
  fBypassMerger = 0;
  fClusterLowerLimit = 10;
  fSingleSeqLimit = 0;
  fMergerDistance = 4;
  fUseTimeBinWindow = 1;
  fUseTimeFollow = 1;
  fChargeFluctuation = 0;
  fTagDeconvolutedClusters = 0;
  fProcessingRCU2Data = 0;
  fDebug = 0;
  fBenchmark.Reset();
  fBenchmark.SetTimer(0,"total");
  fBenchmark.SetTimer(1,"reco");    
}

int AliHLTTPCHWCFEmulatorComponent::ReadConfigurationString(  const char* arguments )
{
  // Set configuration parameters for the FPGA ClusterFinder Emulator component
  // from the string

  int iResult = 0;
  if ( !arguments ) return iResult;
  //cout<<"["<<arguments<<"]"<<endl;
  TString allArgs = arguments;
  TString argument;
  int bMissingParam = 0;

  TObjArray* pTokens = allArgs.Tokenize( " " );

  int nArgs =  pTokens ? pTokens->GetEntries() : 0;

  for ( int i = 0; i < nArgs; i++ ) {
    argument = ( ( TObjString* )pTokens->At( i ) )->GetString();
    if ( argument.IsNull() ) continue;

    if ( argument.CompareTo( "-deconvolute-time" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoDeconvTime  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      fUseTimeBinWindow = fDoDeconvTime;
      HLTInfo( "Time deconvolution and using of TimeBin window are set to: %d", fDoDeconvTime );
      continue;
    }

    if ( argument.CompareTo( "-deconvolute-pad" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoDeconvPad  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Pad deconvolution is set to: %d", fDoDeconvPad );
      continue;
    }

    if ( argument.CompareTo( "-deconvolute" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoDeconvTime  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      fUseTimeBinWindow = fDoDeconvTime;
      fDoDeconvPad  = fDoDeconvTime;
      HLTInfo( "Time and pad deconvolution and using of TimeBin window are set to: %d", fDoDeconvPad );
      continue;
    }
 
    if ( argument.CompareTo( "-do-mc" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoMC  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "MC processing is set to: %d", fDoMC );
      continue;
    }

    if ( argument.CompareTo( "-flow-control" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoFlowControl  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Flow control is set to: %d", fDoFlowControl );
      continue;
    }

    if ( argument.CompareTo( "-single-pad-suppression" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDoSinglePadSuppression  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Single pad suppression is set to: %d", fDoSinglePadSuppression );
      continue;
    }
    
    if ( argument.CompareTo( "-bypass-merger" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fBypassMerger  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Bypassing merger is set to: %d", fBypassMerger );
      continue;
    }

    if ( argument.CompareTo( "-cluster-lower-limit" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fClusterLowerLimit  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Cluster lower limit is set to: %d", fClusterLowerLimit );
      continue;
    }

    if ( argument.CompareTo( "-single-sequence-limit" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fSingleSeqLimit  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Single sequence limit is set to: %d", fSingleSeqLimit );
      continue;
    }
    
    if ( argument.CompareTo( "-merger-distance" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fMergerDistance  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Merger distance is set to: %d", fMergerDistance );
      continue;
    }
 
    if ( argument.CompareTo( "-use-timebin-window" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fUseTimeBinWindow  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      fDoDeconvTime = fUseTimeBinWindow;
      HLTInfo( "Using TimeBin window and Time deconvolution are set to: %d", fUseTimeBinWindow );
      continue;
    }
   
    if ( argument.CompareTo( "-charge-fluctuation" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fChargeFluctuation  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Charge fluctuation is set to: %d", fChargeFluctuation );
      continue;
    }
    
    if ( argument.CompareTo( "-noise-suppression" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fNoiseSuppression  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Noise Suppression is set to: %d", fNoiseSuppression );
      continue;
    }

    if ( argument.CompareTo( "-use-time-follow" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fUseTimeFollow  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Use of time follow algorithm is set to: %d", fUseTimeFollow );
      continue;
    }
   
    if ( argument.CompareTo( "-debug-level" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fDebug  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Debug level is set to: %d", fDebug );
      continue;
    }

    if ( argument.CompareTo( "-tag-deconvoluted-clusters" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fTagDeconvolutedClusters  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Tag Deconvoluted Clusters is set to: %d", fTagDeconvolutedClusters );
      continue;
    }
 
    if ( argument.CompareTo( "-rcu2-data" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fProcessingRCU2Data  = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
      HLTInfo( "Processing RCU2 data flag is set to: %d", fProcessingRCU2Data );
      continue;
    }
   
    HLTError( "Unknown option \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }
  delete pTokens;

  if ( bMissingParam ) {
    HLTError( "Specifier missed for parameter \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }

  return iResult;
}


int AliHLTTPCHWCFEmulatorComponent::ReadCDBEntry( const char* cdbEntry, const char* chainId )
{
  // Read configuration from OCDB

  const char* defaultNotify = "";

  if ( !cdbEntry ){
    cdbEntry = "HLT/ConfigTPC/TPCHWClusterFinder";
    defaultNotify = " (default)";
    chainId = 0;
  }

  HLTInfo( "configure from entry \"%s\"%s, chain id %s", cdbEntry, defaultNotify, ( chainId != NULL && chainId[0] != 0 ) ? chainId : "<none>" );
  AliCDBEntry *pEntry = AliCDBManager::Instance()->Get( cdbEntry );//,GetRunNo());
  
  if ( !pEntry ) {
    HLTError( "cannot fetch object \"%s\" from CDB", cdbEntry );
    return -EINVAL;
  }

  TObjString* pString = dynamic_cast<TObjString*>( pEntry->GetObject() );

  if ( !pString ) {
    HLTError( "configuration object \"%s\" has wrong type, required TObjString", cdbEntry );
    return -EINVAL;
  }

  HLTInfo( "received configuration object string: \"%s\"", pString->GetString().Data() );

  return  ReadConfigurationString( pString->GetString().Data() );
}


int AliHLTTPCHWCFEmulatorComponent::Configure( const char* cdbEntry, const char* chainId, const char *commandLine )
{
  // Configure the component
  // There are few levels of configuration,
  // parameters which are set on one step can be overwritten on the next step

  //* read hard-coded values

  SetDefaultConfiguration();

  //* read the default CDB entry

  int iResult1 = ReadCDBEntry( NULL, chainId );

  //* read the actual CDB entry if required

  int iResult2 = ( cdbEntry ) ? ReadCDBEntry( cdbEntry, chainId ) : 0;

  //* read extra parameters from input (if they are)

  int iResult3 = 0;

  if ( commandLine && commandLine[0] != '\0' ) {
    HLTInfo( "received configuration string from HLT framework: \"%s\"", commandLine );
    iResult3 = ReadConfigurationString( commandLine );
  }
  
  if( fDebug>1 ) fCFEmulator.SetDebugLevel( fDebug );
  else fCFEmulator.SetDebugLevel(0);

  fCFSupport.UnloadMapping();
  fCFSupport.SetProcessingRCU2Data( fProcessingRCU2Data );

  return iResult1 ? iResult1 : ( iResult2 ? iResult2 : iResult3 );
}


int AliHLTTPCHWCFEmulatorComponent::DoDeinit()
{
  // see header file for class documentation 
  return 0;
}


int AliHLTTPCHWCFEmulatorComponent::DoEvent( const AliHLTComponentEventData& evtData, 
							const AliHLTComponentBlockData* blocks, 
							AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, 
							AliHLTUInt32_t& size, 
							vector<AliHLTComponentBlockData>& outputBlocks )
{
  // see header file for class documentation

  int iResult=0;
  AliHLTUInt32_t maxSize = size;
  size = 0;
  
  if(!IsDataEvent()){
    return 0;
  }

  fBenchmark.StartNewEvent();
  fBenchmark.Start(0);

  AliHLTUInt32_t configWord1=0, configWord2=0; 
  AliHLTTPCHWCFEmulator::CreateConfiguration
    ( fDoDeconvTime, fDoDeconvPad, fDoFlowControl, fDoSinglePadSuppression, fBypassMerger, fClusterLowerLimit, fSingleSeqLimit, fMergerDistance, fUseTimeBinWindow, fChargeFluctuation, fUseTimeFollow, configWord1, configWord2 );

  AliHLTUInt8_t *outBlock = NULL;
  AliHLTTPCClusterMCLabel *allocOutMC = NULL;

  for ( unsigned long ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      const AliHLTComponentBlockData* iter = blocks+ndx;
      
      fBenchmark.AddInput(iter->fSize);

      if (  iter->fDataType != (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC) 
	    &&  iter->fDataType != AliHLTTPCDefinitions::fgkUnpackedRawDataType ) continue;

      int slice = AliHLTTPCDefinitions::GetMinSliceNr( *iter );
      int patch = AliHLTTPCDefinitions::GetMinPatchNr( *iter );
 
  
      if (!iter->fPtr) continue;
 
      // create input block for the HW cluster finder

      const AliHLTUInt32_t *rawEvent=0;
      const AliHLTTPCClusterMCLabel *mcLabels = 0;
      AliHLTUInt32_t rawEventSize32 = 0;
      AliHLTUInt32_t nMCLabels = 0;

      if( fCFSupport.CreateRawEvent( iter, rawEvent, rawEventSize32, mcLabels, nMCLabels )<0 ) continue; 
      if( !fDoMC ){
	mcLabels = 0;
	nMCLabels = 0;
      }

      
      AliHLTUInt32_t maxNClusters = rawEventSize32 + 1; // N 32-bit words in input
      AliHLTUInt32_t clustersSize32 = maxNClusters*AliHLTTPCHWCFData::fgkAliHLTTPCHWClusterSize;
      AliHLTUInt32_t nOutputMC = maxNClusters;

      // create or forward the CDH header

      AliRawDataHeaderV3 dummyCDHHeader;

      bool isRawInput = ( iter->fDataType == kAliHLTDataTypeDDLRaw );

      AliHLTCDHWrapper header( isRawInput ?iter->fPtr :&dummyCDHHeader );

      AliHLTUInt32_t headerSize = header.GetHeaderSize();

      // book memory for the output

      AliHLTUInt32_t outBlockSize = headerSize+clustersSize32*sizeof(AliHLTUInt32_t);

      if( outBlock ) delete[] outBlock;
      if( allocOutMC ) delete[] allocOutMC;      
      outBlock = new AliHLTUInt8_t[outBlockSize];
      allocOutMC = new AliHLTTPCClusterMCLabel[nOutputMC+1];
 
      if (!outBlock || !allocOutMC) {
	iResult=-ENOMEM;
	break;
      } 

      memset(outBlock, 0, outBlockSize*sizeof(AliHLTUInt8_t));
      memset(allocOutMC, 0, (nOutputMC+1)*sizeof(AliHLTTPCClusterMCLabel));
      AliHLTTPCClusterMCData *outMC = reinterpret_cast<AliHLTTPCClusterMCData *>(allocOutMC);
      
      // fill CDH header here, since the HW clusterfinder does not receive it
      memcpy(outBlock, header.GetHeader(), headerSize );
      memset(outBlock,0xFF,4);

      //AliRawDataHeader *cdhHeader = reinterpret_cast<AliRawDataHeader*>(iter->fPtr);
      //AliRawDataHeader *outCDHHeader = reinterpret_cast<AliRawDataHeader*>(outBlock);
      //*outCDHHeader = *cdhHeader;
      //outCDHHeader->fSize = 0xFFFFFFFF;

      AliHLTUInt32_t *outClusters = reinterpret_cast<AliHLTUInt32_t*> (outBlock + headerSize);
     
      fBenchmark.Start(1);
      fCFEmulator.Init
	( fCFSupport.GetMapping(slice,patch), configWord1, configWord2 );

      fCFEmulator.SetTagDeconvolutedClusters( fTagDeconvolutedClusters );
      fCFEmulator.SetProcessingRCU2Data( fProcessingRCU2Data );
      fCFEmulator.SetNoiseSuppression( fNoiseSuppression );

      int err = fCFEmulator.FindClusters( rawEvent, rawEventSize32, 
					  outClusters, clustersSize32, 
					  mcLabels, nMCLabels,
					  outMC );
      fBenchmark.Stop(1);
      if( err==-1 ){ HLTWarning("NULL input pointer (warning %d)",err);}
      else if( err==-2 ){  HLTWarning("No space left in the output buffer (warning %d)",err); }
      else if( err<0 ){ HLTWarning("HWCF emulator finished with error code %d",err); }
      if( err<0 ){
	continue;
      }

      if( fDebug ){
	int elsize=AliHLTTPCHWCFData::fgkAliHLTTPCHWClusterSize;
	printf("\nHWCF Emulator: output clusters for slice%d patch %d:\n",slice,patch);
	for( AliHLTUInt32_t i=0; i<clustersSize32; i+=elsize ){
	  AliHLTUInt32_t *c = outClusters+i;
	  AliHLTUInt32_t flag = (c[0]>>30);  	  
	  if( flag == 0x3){ //beginning of a cluster
	    int padRow  = (c[0]>>24)&0x3f;
	    int q  = c[1];
	    double p   = *((AliHLTFloat32_t*)&c[2]);
	    double t  = *((AliHLTFloat32_t*)&c[3]);
	    AliHLTFloat32_t p2 = *((AliHLTFloat32_t*)&c[4]);
	    AliHLTFloat32_t t2 = *((AliHLTFloat32_t*)&c[5]);
	    printf("N: %3d    R: %3d    C: %4d    P:  %7.4f    T:  %8.4f    DP: %6.4f    DT: %6.4f\n", 
		   i/elsize+1, padRow, q, p, t, sqrt(fabs(p2-p*p)), sqrt(fabs(t2-t*t)));

	    if( outMC && outMC->fCount>0 ){
	      printf("        MC: (%3d,%6.1f) (%3d,%6.1f) (%3d,%6.1f)\n",
		     outMC->fLabels[i/elsize].fClusterID[0].fMCID,outMC->fLabels[i/elsize].fClusterID[0].fWeight,
		     outMC->fLabels[i/elsize].fClusterID[1].fMCID,outMC->fLabels[i/elsize].fClusterID[1].fWeight,
		     outMC->fLabels[i/elsize].fClusterID[2].fMCID,outMC->fLabels[i/elsize].fClusterID[2].fWeight
		     );
	    }
	  }
	}
      }
          

      AliHLTUInt32_t outSize = headerSize + clustersSize32*sizeof(AliHLTUInt32_t);
      
      if( size + outSize <= maxSize ){
	
	memcpy( outputPtr, outBlock, outSize );
	
	AliHLTComponentBlockData bd;
	FillBlockData( bd );
	bd.fOffset = size;
	bd.fSize = outSize;
	bd.fSpecification = iter->fSpecification;
	bd.fDataType = AliHLTTPCDefinitions::fgkHWClustersDataType | kAliHLTDataOriginTPC;
	outputBlocks.push_back( bd );
	fBenchmark.AddOutput(bd.fSize);
	size+= bd.fSize;
	outputPtr+=bd.fSize;
      } else {
	HLTWarning( "Output buffer (%db) is too small, required %db", maxSize, size+outSize);
	iResult=-ENOSPC;
	break;
      }

      if( fDoMC && outMC && outMC->fCount>0 ){
	int s = sizeof(AliHLTTPCClusterMCData) + outMC->fCount*sizeof(AliHLTTPCClusterMCLabel);
	if( size + s <= maxSize ){
	  memcpy( outputPtr, outMC, s );	  	  
	  AliHLTComponentBlockData bdMCInfo;
	  FillBlockData( bdMCInfo );
	  bdMCInfo.fOffset = size;
	  bdMCInfo.fSize = s;
	  bdMCInfo.fSpecification = iter->fSpecification;
	  bdMCInfo.fDataType = AliHLTTPCDefinitions::fgkAliHLTDataTypeClusterMCInfo | kAliHLTDataOriginTPC;
	  outputBlocks.push_back( bdMCInfo );
	  fBenchmark.AddOutput(bdMCInfo.fSize);
	  size+=bdMCInfo.fSize;
	  outputPtr+=bdMCInfo.fSize; 
	} else {	
	  HLTWarning( "Output buffer (%db) is too small, required %db", maxSize, size+s);
	  iResult=-ENOSPC;
	  break;
	}
      }
    }
 
  if( outBlock ) delete[] outBlock;
  if( allocOutMC ) delete[] allocOutMC;      
  
  if( iResult>=0 ){
    //
    // forward unpacked clusters if they are present
    // to forward input data, one should only forward the block descriptors, without copying the data. 
    // The framework will recognise that these blocks are forwarded, as they have fPtr field !=NULL, and take the data from fPtr pointer   
    //
    for( unsigned long ndx = 0; ndx < evtData.fBlockCnt; ndx++ ){
      const AliHLTComponentBlockData* iter = blocks+ndx;      
      if(  iter->fDataType == AliHLTTPCDefinitions::RawClustersDataType() || iter->fDataType == AliHLTTPCDefinitions::RawClustersDescriptorDataType() ){
	if( !iter->fPtr ) continue;
	fBenchmark.AddOutput(iter->fSize);
	outputBlocks.push_back( *iter );
      }
    } 
  }

  fBenchmark.Stop(0);  
  HLTInfo(fBenchmark.GetStatistics());
  fCFSupport.ReleaseEventMemory();
  return iResult;
}



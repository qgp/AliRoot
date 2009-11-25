// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Timm Steinbeck, Matthias Richter                      *
//* Developers:      Kenneth Aamodt <kenneth.aamodt@student.uib.no>        *
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

/** @file   AliHLTTPCClusterFinderComponent.cxx
    @author Kenneth Aamodt <kenneth.aamodt@student.uib.no>
    @date   
    @brief  The TPC cluster finder processing component
*/

#if __GNUC__>= 3
using namespace std;
#endif
#include "AliHLTTPCClusterFinderComponent.h"
#include "AliHLTTPCDigitReaderPacked.h"
#include "AliHLTTPCDigitReaderUnpacked.h"
#include "AliHLTTPCDigitReaderDecoder.h"
#include "AliHLTTPCDigitReader32Bit.h"
#include "AliHLTTPCClusterFinder.h"
#include "AliHLTTPCSpacePointData.h"
#include "AliHLTTPCClusterDataFormat.h"
#include "AliHLTTPCTransform.h"
#include "AliHLTTPCClusters.h"
#include "AliHLTTPCDefinitions.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliTPCcalibDB.h"
#include "AliTPCCalPad.h"
#include "AliTPCParam.h"

#include <cstdlib>
#include <cerrno>
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "TGeoGlobalMagField.h"

#include <sys/time.h>

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCClusterFinderComponent)

const char* AliHLTTPCClusterFinderComponent::fgkOCDBEntryPacked="HLT/ConfigTPC/TPCClusterFinderPacked";
const char* AliHLTTPCClusterFinderComponent::fgkOCDBEntryUnpacked="HLT/ConfigTPC/TPCClusterFinderUnpacked";
const char* AliHLTTPCClusterFinderComponent::fgkOCDBEntryDecoder="HLT/ConfigTPC/TPCClusterFinderDecoder";
const char* AliHLTTPCClusterFinderComponent::fgkOCDBEntry32Bit="HLT/ConfigTPC/TPCClusterFinder32Bit";

AliHLTTPCClusterFinderComponent::AliHLTTPCClusterFinderComponent(int mode)
  :
  fClusterFinder(NULL),
  fReader(NULL),
  fDeconvTime(kFALSE),
  fDeconvPad(kFALSE),
  fClusterDeconv(false),
  fXYClusterError(-1),
  fZClusterError(-1),
  fModeSwitch(mode),
  fUnsorted(1),
  fPatch(0),
  fGetActivePads(0),
  fFirstTimeBin(-1),
  fLastTimeBin(-1),
  fDoMC(kFALSE),
  fReleaseMemory( kFALSE )
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
  if (fModeSwitch!=kClusterFinderPacked &&
      fModeSwitch!=kClusterFinderUnpacked &&
      fModeSwitch!=kClusterFinderDecoder &&
      fModeSwitch!=kClusterFinder32Bit) {
    HLTFatal("unknown digit reader type");
  }
}

AliHLTTPCClusterFinderComponent::~AliHLTTPCClusterFinderComponent()
{
  // see header file for class documentation
}

// Public functions to implement AliHLTComponent's interface.
// These functions are required for the registration process

const char* AliHLTTPCClusterFinderComponent::GetComponentID()
{
  // see header file for class documentation
  switch(fModeSwitch){
  case kClusterFinderPacked:
    return "TPCClusterFinderPacked";
    break;
  case kClusterFinderUnpacked: 	 
    return "TPCClusterFinderUnpacked"; 	 
    break;
  case kClusterFinderDecoder:
    return "TPCClusterFinderDecoder";
    break;
  case kClusterFinder32Bit:
    return "TPCClusterFinder32Bit";
    break;
  }
  return "";
}

void AliHLTTPCClusterFinderComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  // see header file for class documentation
  list.clear(); 
  switch(fModeSwitch){
  case kClusterFinderPacked:
    list.push_back( kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC );
    break;
  case kClusterFinderUnpacked: 	 
    list.push_back( AliHLTTPCDefinitions::fgkUnpackedRawDataType ); 	 
    break;
  case kClusterFinderDecoder:
    list.push_back( kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC );
    break;
  case kClusterFinder32Bit:
    list.push_back( kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC );
    break;
  }
}

AliHLTComponentDataType AliHLTTPCClusterFinderComponent::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTMultipleDataType;
}

int AliHLTTPCClusterFinderComponent::GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList)

{
  // see header file for class documentation
  tgtList.clear();
  tgtList.push_back(AliHLTTPCDefinitions::fgkClustersDataType);
  tgtList.push_back(kAliHLTDataTypeHwAddr16);
  return tgtList.size();
}

void AliHLTTPCClusterFinderComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // see header file for class documentation
  // XXX TODO: Find more realistic values.  
  constBase = 0;
  switch(fModeSwitch){
  case kClusterFinderPacked:
    inputMultiplier = (6 * 0.4);
    break;
  case kClusterFinderUnpacked:
    inputMultiplier = 0.4;
    break;
  case kClusterFinderDecoder:
    inputMultiplier = (6 * 0.4);
    break;
  case kClusterFinder32Bit:
    inputMultiplier = (6 * 0.4);
    break;
  }
}

AliHLTComponent* AliHLTTPCClusterFinderComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTTPCClusterFinderComponent(fModeSwitch);
}
	
int AliHLTTPCClusterFinderComponent::DoInit( int argc, const char** argv )
{
  // see header file for class documentation
  if ( fClusterFinder )
    return -EINPROGRESS;

  //Test if the OCDB entries used by AliTPCTransform is availible
  AliTPCcalibDB*  calib=AliTPCcalibDB::Instance();  
  //
  if(!calib){
    HLTError("AliTPCcalibCD does not exist");
    return -ENOENT;
  }
  AliTPCCalPad * time0TPC = calib->GetPadTime0(); 
  if(!time0TPC){
    HLTError("OCDB entry TPC/Calib/PadTime0 (AliTPCcalibDB::GetPadTime0()) is not available.");
    return -ENOENT;
  }

  AliTPCParam  * param    = calib->GetParameters();
  if(!param){
    HLTError("OCDB entry TPC/Calib/Parameters (AliTPCcalibDB::GetParameters()) is not available.");
    return -ENOENT;
  }

  // Check field
  if (!TGeoGlobalMagField::Instance()) {
    HLTError("magnetic field not initialized, please set up TGeoGlobalMagField and AliMagF");
    return -ENODEV;
  }
  calib->SetExBField(GetBz());

  fClusterFinder = new AliHLTTPCClusterFinder();

  // first configure the default
  int iResult = 0;
  switch(fModeSwitch){
  case kClusterFinderPacked:
    iResult=ConfigureFromCDBTObjString(fgkOCDBEntryPacked);
    break;
  case kClusterFinderUnpacked: 	 
    iResult=ConfigureFromCDBTObjString(fgkOCDBEntryUnpacked);
    break;
  case kClusterFinderDecoder:
    iResult=ConfigureFromCDBTObjString(fgkOCDBEntryDecoder);
    break;
  case kClusterFinder32Bit:
    iResult=ConfigureFromCDBTObjString(fgkOCDBEntry32Bit);
    break;
  }

  // configure from the command line parameters if specified
  if (iResult>=0 && argc>0)
    iResult=ConfigureFromArgumentString(argc, argv);
  // return iResult;

  /*
  Int_t iResult=0;
  TString configuration="";
  TString argument="";
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (!configuration.IsNull()) configuration+=" ";
    configuration+=argument;
  }
  
  if (!configuration.IsNull()) {
    iResult=Configure(configuration.Data());
  } else {
    iResult=Reconfigure(NULL, NULL);
  }
  */

  //Checking for conflicting arguments
  if(fClusterDeconv){
    if(fDeconvPad==kTRUE || fDeconvTime==kTRUE){
      HLTWarning("Conflicting arguments: argument 'pp-run' will be ignored.");
    }
  }
  if(fClusterFinder->GetOccupancyLimit()!=1.0 && fUnsorted){
    HLTWarning("Argument 'occupancy-limit' is deprecated when doing unsorted data reading.");
  }
  if(fGetActivePads==kTRUE && fUnsorted==kFALSE){
    HLTWarning("Argument '-active-pads' only work with unsorted data reading. Active pads list will not be produced.");
  }
  

  // Choose reader
  if (fModeSwitch==kClusterFinderPacked) {
      HLTDebug("using AliHLTTPCDigitReaderPacked");
      fReader = new AliHLTTPCDigitReaderPacked();
      if(fUnsorted==1){	fReader->SetUnsorted(kTRUE); }
      fClusterFinder->SetReader(fReader);
  }
  else if(fModeSwitch==kClusterFinderUnpacked){ 	 
    HLTDebug("using AliHLTTPCDigitReaderUnpacked"); 	 
    fReader = new AliHLTTPCDigitReaderUnpacked(); 	 
    if(fUnsorted==1){	fReader->SetUnsorted(kTRUE); }
    fClusterFinder->SetReader(fReader);
  } 
  else if(fModeSwitch==kClusterFinderDecoder){
    HLTDebug("using AliHLTTPCDigitReaderDecoder");
    fReader = new AliHLTTPCDigitReaderDecoder();
    fClusterFinder->SetReader(fReader);
  }
  else if(fModeSwitch==kClusterFinder32Bit){
    HLTDebug("using AliHLTTPCDigitReader32Bit");
    fReader = new AliHLTTPCDigitReader32Bit();
    fClusterFinder->SetReader(fReader);
    fClusterFinder->Set32BitFormat(kTRUE);
  }
  else{
    HLTFatal("No mode set for clusterfindercomponent");
  }

  if(fClusterDeconv){
    fClusterFinder->SetOccupancyLimit(1.0);
  }
  
  fClusterFinder->SetDeconv(fClusterDeconv);
  fClusterFinder->SetDeconvPad(fDeconvPad);
  fClusterFinder->SetDeconvTime(fDeconvPad);
  fClusterFinder->SetXYError( fXYClusterError );
  fClusterFinder->SetZError( fZClusterError );
  if ( (fXYClusterError>0) && (fZClusterError>0) ){
    fClusterFinder->SetCalcErr( false );
  }

  if(fFirstTimeBin>0){
    fClusterFinder->SetFirstTimeBin(fFirstTimeBin);
  }
  if(fLastTimeBin>0 && fLastTimeBin>fFirstTimeBin && fLastTimeBin<=AliHLTTPCTransform::GetNTimeBins()){
    fClusterFinder->SetLastTimeBin(fLastTimeBin);
  }

  return iResult;
}

int AliHLTTPCClusterFinderComponent::DoDeinit()
{
  // see header file for class documentation

  if ( fClusterFinder )
    delete fClusterFinder;
  fClusterFinder = NULL;
 
  if ( fReader )
    delete fReader;
  fReader = NULL;
    
  return 0;
}

int AliHLTTPCClusterFinderComponent::DoEvent( const AliHLTComponentEventData& evtData, 
					      const AliHLTComponentBlockData* blocks, 
					      AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, 
					      AliHLTUInt32_t& size, 
					      vector<AliHLTComponentBlockData>& outputBlocks )
{
  // see header file for class documentation
  int iResult=0;

  if(fReader == NULL){
    HLTFatal("Digit reader not initialized, skipping HLT TPC cluster reconstruction.");
    size=0;
    return -ENODEV;    
  }

  if(!IsDataEvent()){
    size=0;
    return 0;
  }

  //  == init iter (pointer to datablock)
  const AliHLTComponentBlockData* iter = NULL;
  unsigned long ndx;

  //  == OUTdatatype pointer
  AliHLTTPCClusterData* outPtr;

  AliHLTUInt8_t* outBPtr;
  UInt_t offset, mysize, nSize, tSize = 0;

  outBPtr = outputPtr;
  outPtr = (AliHLTTPCClusterData*)outBPtr;

  Int_t slice, patch;
  unsigned long maxPoints, realPoints = 0;

  for ( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      mysize = 0;
      offset = tSize;

      // Kenneth 16. July 2009
      // 32 byte is the size of the common data header (CDH)
      // this is introduced as a protection of empty files
      // normally when running with file publisher where 
      // Timms script is used to create the missing files
      if(iter->fSize <= 32){
	continue;
      }


      if (fModeSwitch==0 || fModeSwitch==2 || fModeSwitch==3) {
	HLTDebug("Event 0x%08LX (%Lu) received datatype: %s - required datatype: %s",
		 evtData.fEventID, evtData.fEventID, 
		 DataType2Text( iter->fDataType).c_str(), 
		 DataType2Text(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC).c_str());

	if (iter->fDataType == AliHLTTPCDefinitions::fgkDDLPackedRawDataType &&
	    GetEventCount()<2) {
	  HLTWarning("data type %s is depricated, use %s (kAliHLTDataTypeDDLRaw)!",
		     DataType2Text(AliHLTTPCDefinitions::fgkDDLPackedRawDataType).c_str(),
		     DataType2Text(kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC).c_str());
	  }

	if ( iter->fDataType != (kAliHLTDataTypeDDLRaw | kAliHLTDataOriginTPC) &&
	     iter->fDataType != AliHLTTPCDefinitions::fgkDDLPackedRawDataType ) continue;

      }
      else if(fModeSwitch==1){
	HLTDebug("Event 0x%08LX (%Lu) received datatype: %s - required datatype: %s",
		 evtData.fEventID, evtData.fEventID, 
		 DataType2Text( iter->fDataType).c_str(), 
		 DataType2Text(AliHLTTPCDefinitions::fgkUnpackedRawDataType).c_str());

	if ( iter->fDataType != AliHLTTPCDefinitions::fgkUnpackedRawDataType ) continue;

      }

      slice = AliHLTTPCDefinitions::GetMinSliceNr( *iter );
      patch = AliHLTTPCDefinitions::GetMinPatchNr( *iter );

      if(fUnsorted){
	fClusterFinder->SetUnsorted(fUnsorted);
	fClusterFinder->SetPatch(patch);
      }

      outPtr = (AliHLTTPCClusterData*)outBPtr;

      maxPoints = (size-tSize-sizeof(AliHLTTPCClusterData))/sizeof(AliHLTTPCSpacePointData);

      fClusterFinder->InitSlice( slice, patch, maxPoints );
      fClusterFinder->SetOutputArray( (AliHLTTPCSpacePointData*)outPtr->fSpacePoints );
	
      if(fUnsorted){
      	if(fGetActivePads){
	  fClusterFinder->SetDoPadSelection(kTRUE);
	}	
	if(fDeconvTime){
	  fClusterFinder->ReadDataUnsortedDeconvoluteTime(iter->fPtr, iter->fSize);
	}
	else{
	  fClusterFinder->ReadDataUnsorted(iter->fPtr, iter->fSize);
	}

	fClusterFinder->FindClusters();
      }
      else{
	fClusterFinder->Read(iter->fPtr, iter->fSize );
	fClusterFinder->ProcessDigits();
      }
      fReader->Reset();

      realPoints = fClusterFinder->GetNumberOfClusters();
	
      outPtr->fSpacePointCnt = realPoints;
      nSize = sizeof(AliHLTTPCSpacePointData)*realPoints;
      mysize += nSize+sizeof(AliHLTTPCClusterData);

      Logging( kHLTLogDebug, "HLT::TPCClusterFinder::DoEvent", "Spacepoints", 
	       "Number of spacepoints: %lu Slice/Patch/RowMin/RowMax: %d/%d/%d/%d.",
	       realPoints, slice, patch,AliHLTTPCTransform::GetFirstRow( patch ) , AliHLTTPCTransform::GetLastRow( patch ) );
      AliHLTComponentBlockData bd;
      FillBlockData( bd );
      bd.fOffset = offset;
      bd.fSize = mysize;
      bd.fSpecification = iter->fSpecification;
      bd.fDataType = AliHLTTPCDefinitions::fgkClustersDataType;
      outputBlocks.push_back( bd );
	
      tSize += mysize;
      outBPtr += mysize;
      outPtr = (AliHLTTPCClusterData*)outBPtr;
	

      if ( tSize > size )
	{
	  Logging( kHLTLogFatal, "HLT::TPCClusterFinder::DoEvent", "Too much data", 
		   "Data written over allowed buffer. Amount written: %lu, allowed amount: %lu.",
		   tSize, size );
	  iResult=-ENOSPC;
	  break;
	}
	
      if(fUnsorted && fGetActivePads){
       Int_t maxNumberOfHW=(Int_t)((size-tSize)/sizeof(AliHLTUInt16_t)-1);
       AliHLTUInt16_t* outputHWPtr= (AliHLTUInt16_t*)(outputPtr+tSize);
       Int_t nHWAdd = fClusterFinder->FillHWAddressList(outputHWPtr, maxNumberOfHW);
      
       AliHLTComponentBlockData bdHW;
       FillBlockData( bdHW );
       bdHW.fOffset = tSize ;
       bdHW.fSize = nHWAdd*sizeof(AliHLTUInt16_t);
       bdHW.fSpecification = iter->fSpecification;
       bdHW.fDataType = kAliHLTDataTypeHwAddr16;
       outputBlocks.push_back( bdHW );
       
       tSize+=nHWAdd*sizeof(AliHLTUInt16_t);
      }

      if(fDoMC){
	Int_t maxNumberOfClusterMCInfo = (Int_t)((size-tSize)/sizeof(AliHLTTPCClusterFinder::ClusterMCInfo)-1);
	AliHLTTPCClusterFinder::ClusterMCInfo* outputMCInfo= (AliHLTTPCClusterFinder::ClusterMCInfo*)(outputPtr+tSize);
	Int_t nMCInfo = fClusterFinder->FillOutputMCInfo(outputMCInfo, maxNumberOfClusterMCInfo);
	
	AliHLTComponentBlockData bdMCInfo;
	FillBlockData( bdMCInfo );
	bdMCInfo.fOffset = tSize ;
	bdMCInfo.fSize = nMCInfo*sizeof(AliHLTTPCClusterFinder::ClusterMCInfo);
	bdMCInfo.fSpecification = iter->fSpecification;
	bdMCInfo.fDataType = AliHLTTPCDefinitions::fgkAliHLTDataTypeClusterMCInfo;
	outputBlocks.push_back( bdMCInfo );
	
	tSize+=nMCInfo*sizeof(AliHLTTPCClusterFinder::ClusterMCInfo);

      }
    }

  if (iResult>=0)
    size = tSize;

  return iResult;
}

int AliHLTTPCClusterFinderComponent::ScanConfigurationArgument(int argc, const char** argv){

  // see header file for class documentation

  if (argc<=0) return 0;
  int i=0;
  TString argument=argv[i];

  if (argument.CompareTo("-solenoidBz")==0){
    if (++i>=argc) return -EPROTO;
    HLTWarning("argument -solenoidBz is deprecated, magnetic field set up globally (%f)", GetBz());
    return 2;
  }

  if (argument.CompareTo("-update-calibdb")==0 || argument.CompareTo("-update-transform")==0 ){
    if(fClusterFinder->UpdateCalibDB()){
      HLTDebug("CalibDB and offline transform successfully updated.");
    }
    else{
      HLTError("CalibDB could not be updated.");
    }
    return 1;
  }

  if (argument.CompareTo("-deconvolute-time")==0){
    HLTDebug("Switching on deconvolution in time direction.");
    fDeconvTime = kTRUE;
    fClusterFinder->SetDeconvTime(fDeconvTime);
    return 1;
  }

  if (argument.CompareTo("-deconvolute-pad")==0){
    HLTDebug("Switching on deconvolution in pad direction.");
    fDeconvPad = kTRUE;
    fClusterFinder->SetDeconvPad(fDeconvPad);
    return 1;
  }

  if (argument.CompareTo("-timebins")==0 || argument.CompareTo("timebins" )==0){
    HLTWarning("Argument %s is depreciated after moving to the offline AliTPCTransform class for xyz calculations.",argument.Data());
    /*
      if (++i>=argc) return -EPROTO;
      argument=argv[i];
      AliHLTTPCTransform::SetNTimeBins(argument.Atoi());
      fClusterFinder->UpdateLastTimeBin();
      HLTInfo("number of timebins set to %d, zbin=%f", AliHLTTPCTransform::GetNTimeBins(), AliHLTTPCTransform::GetZWidth());
      return 2;
    */
    if(argument.CompareTo("timebins")==0){
      HLTWarning("Argument 'timebins' is old, please switch to new argument naming convention (-timebins). The timebins argument will still work, but please change anyway.");
    }
    return 2;
  }
  
  if (argument.CompareTo("-first-timebin")==0){
    if (++i>=argc) return -EPROTO;
    argument=argv[i];
    fFirstTimeBin = argument.Atoi();
    if(fFirstTimeBin>=0){
      HLTDebug("fFirstTimeBin set to %d",fFirstTimeBin);
      fClusterFinder->SetFirstTimeBin(fFirstTimeBin);
    }
    else{
      HLTError("-first-timebin specifier is negative: %d",fFirstTimeBin);
    }
    return 2;
  }

  if (argument.CompareTo("-last-timebin")==0){
    if (++i>=argc) return -EPROTO;
    argument=argv[i];
    fLastTimeBin = argument.Atoi();
    if(fLastTimeBin<AliHLTTPCTransform::GetNTimeBins()){
      HLTDebug("fLastTimeBin set to %d",fLastTimeBin);
    }
    else{
      HLTError("fLastTimeBins is too big: %d. Maximum: %d",fLastTimeBin,AliHLTTPCTransform::GetNTimeBins());
    }
    return 2;
  }
 
  if (argument.CompareTo("-sorted")==0) {
    fUnsorted=0;
    HLTDebug("Swithching unsorted off.");
    fClusterFinder->SetUnsorted(0);
    return 1;
  } 
  
  if (argument.CompareTo("-do-mc")==0) {
    fDoMC=kTRUE;
    fClusterFinder->SetDoMC(fDoMC);
    HLTDebug("Setting fDoMC to true.");
    return 1;
  }
  if (argument.CompareTo("-release-memory")==0) {
    fReleaseMemory=kTRUE;
    fClusterFinder->SetReleaseMemory( fReleaseMemory );
    HLTDebug("Setting fReleaseMemory to true.");
    return 1;
  }

  if (argument.CompareTo("-active-pads")==0 || argument.CompareTo("activepads")==0){
    if(argument.CompareTo("activepads" )==0){
      HLTWarning("Please change to new component argument naming scheme and use '-active-pads' instead of 'activepads'");
    }
    HLTDebug("Switching on ActivePads");
    fGetActivePads = 1;
    fClusterFinder->SetDoPadSelection(kTRUE);
    return 1;
  }

  if (argument.CompareTo("-occupancy-limit")==0 || argument.CompareTo("occupancy-limit")==0){
    if(argument.CompareTo("occupancy-limit" )==0){
      HLTWarning("Please switch to new component argument naming convention, use '-occupancy-limit' instead of 'occupancy-limit'");
    }
    if (++i>=argc) return -EPROTO;
    argument=argv[i];
    fClusterFinder->SetOccupancyLimit(argument.Atof());
    HLTDebug("Occupancy limit set to occulimit %f", argument.Atof());
    return 2;
  }

  if (argument.CompareTo("rawreadermode")==0){
    if (++i>=argc) return -EPROTO;
    HLTWarning("Argument 'rawreadermode' is deprecated");
    return 2;
  }
  
  if (argument.CompareTo("pp-run")==0){
    HLTWarning("Argument 'pp-run' is obsolete, deconvolution is swiched off in both time and pad directions by default.");
    fClusterDeconv = false;
    return 1;
  }

  if (argument.CompareTo("adc-threshold" )==0){
    if (++i>=argc) return -EPROTO;
    HLTWarning("'adc-threshold' is no longer a valid argument, please use TPCZeroSuppression component if you want to zerosuppress data.");
    return 2;
  } 
  
  if (argument.CompareTo("oldrcuformat" )==0){
    if (++i>=argc) return -EPROTO;
    HLTWarning("Argument 'oldrcuformat' is deprecated.");
    return 2;
  }
  
  if (argument.CompareTo("unsorted" )==0 || argument.CompareTo("-unsorted" )==0){
    HLTWarning("Argument is obsolete, unsorted reading is default.");
    //    fClusterFinder->SetUnsorted(1);
    return 1;
  }
  if (argument.CompareTo("nsigma-threshold")==0){
    if (++i>=argc) return -EPROTO;
    HLTWarning("Argument 'nsigma-threshold' argument is obsolete.");
    return 2;
  }

  // unknown argument
  return -EINVAL;
}

int AliHLTTPCClusterFinderComponent::Reconfigure(const char* cdbEntry, const char* /*chainId*/)
{  
  // see header file for class documentation

  const char* entry=cdbEntry;
  if (!entry || entry[0]==0){
    switch(fModeSwitch){
    case kClusterFinderPacked:
      entry=fgkOCDBEntryPacked;
      break;
    case kClusterFinderUnpacked: 	 
      entry=fgkOCDBEntryUnpacked;
      break;
    case kClusterFinderDecoder:
      entry=fgkOCDBEntryDecoder;
      break;
    case kClusterFinder32Bit:
      entry=fgkOCDBEntry32Bit;
      break;
    }
  }

  return ConfigureFromCDBTObjString(entry);

  /*
  int iResult=0;
  
  const char* path="HLT/ConfigTPC/ClusterFinderComponent";
  if (cdbEntry) path=cdbEntry;
  if (path) {
    HLTInfo("reconfigure from entry %s, chain id %s", path, (chainId!=NULL && chainId[0]!=0)?chainId:"<none>");
    AliCDBEntry *pEntry = AliCDBManager::Instance()->Get(path);//,GetRunNo());
    if (pEntry) {
      TObjString* pString=dynamic_cast<TObjString*>(pEntry->GetObject());
      if (pString) {
	HLTInfo("received configuration object: %s", pString->GetString().Data());
	iResult = Configure(pString->GetString().Data());
      } else {
	HLTError("configuration object \"%s\" has wrong type, required TObjString", path);
      }
    } else {
      HLTError("can not fetch object \"%s\" from CDB", path);
    }
  }
  return iResult;
  */
}

int AliHLTTPCClusterFinderComponent::Configure(const char* arguments){
  // see header file for class documentation
  int iResult=0;
  if (!arguments) return iResult;
  
  TString allArgs=arguments;
  TString argument;
  int bMissingParam=0;

  TObjArray* pTokens=allArgs.Tokenize(" ");
  if (pTokens) {
    
    for (int i=0; i<pTokens->GetEntries() && iResult>=0; i++) {
      argument=((TObjString*)pTokens->At(i))->GetString();

      if (argument.IsNull()) continue;
      
      // -- deconvolute-time option
      if (argument.CompareTo("-deconvolute-time")==0){
	HLTDebug("Switching on deconvolution in time direction.");
	fDeconvTime = kTRUE;
	fClusterFinder->SetDeconvTime(fDeconvTime);
      }
      else if (argument.CompareTo("-deconvolute-pad")==0){
	HLTDebug("Switching on deconvolution in pad direction.");
	fDeconvPad = kTRUE;
	fClusterFinder->SetDeconvPad(fDeconvPad);
      }
      else if (argument.CompareTo("-timebins")==0 || argument.CompareTo("timebins" )==0){
	HLTWarning("Argument %s is depreciated after moving to the offline AliTPCTransform class for xyz calculations.",argument.Data());
	/*
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	AliHLTTPCTransform::SetNTimeBins(((TObjString*)pTokens->At(i))->GetString().Atoi());
	fClusterFinder->UpdateLastTimeBin();
	HLTInfo("number of timebins set to %d, zbin=%f", AliHLTTPCTransform::GetNTimeBins(), AliHLTTPCTransform::GetZWidth());
	*/
	if(argument.CompareTo("timebins")==0){
	  HLTWarning("Argument 'timebins' is old, please switch to new argument naming convention (-timebins). The timebins argument will still work, but please change anyway.");
	}
	
      }
      else if (argument.CompareTo("-first-timebin")==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	fFirstTimeBin = ((TObjString*)pTokens->At(i))->GetString().Atoi();
	if(fFirstTimeBin>=0){
	  HLTDebug("fFirstTimeBin set to %d",fFirstTimeBin);
	  fClusterFinder->SetFirstTimeBin(fFirstTimeBin);
	}
	else{
	  HLTError("-first-timebin specifier is negative: %d",fFirstTimeBin);
	}
      }
      else if (argument.CompareTo("-last-timebin")==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	fLastTimeBin = ((TObjString*)pTokens->At(i))->GetString().Atoi();
	if(fLastTimeBin<AliHLTTPCTransform::GetNTimeBins()){
	  HLTDebug("fLastTimeBin set to %d",fLastTimeBin);
	}
	else{
	  HLTError("fLastTimeBins is too big: %d. Maximum: %d",fLastTimeBin,AliHLTTPCTransform::GetNTimeBins());
	}
      }
      else if (argument.CompareTo("-sorted")==0) {
	fUnsorted=0;
	HLTDebug("Swithching unsorted off.");
	fClusterFinder->SetUnsorted(0);
      }
      else if (argument.CompareTo("-do-mc")==0) {
	fDoMC=kTRUE;
	fClusterFinder->SetDoMC(fDoMC);
	HLTInfo("Setting fDoMC to true.");
      }
      else if (argument.CompareTo("-release-memory")==0) {
	fReleaseMemory = kTRUE;
	fClusterFinder->SetReleaseMemory( kTRUE );
	HLTInfo("Setting fReleaseMemory to true.");
      }
      else if (argument.CompareTo("-active-pads")==0 || argument.CompareTo("activepads")==0){
	if(argument.CompareTo("activepads" )==0){
	  HLTWarning("Please change to new component argument naming scheme and use '-active-pads' instead of 'activepads'");
	}
	HLTDebug("Switching on ActivePads");
	fGetActivePads = 1;
	fClusterFinder->SetDoPadSelection(kTRUE);
      }
      else if (argument.CompareTo("-occupancy-limit")==0 || argument.CompareTo("occupancy-limit")==0){
	if(argument.CompareTo("occupancy-limit" )==0){
	  HLTWarning("Please switch to new component argument naming convention, use '-occupancy-limit' instead of 'occupancy-limit'");
	}
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	fClusterFinder->SetOccupancyLimit(((TObjString*)pTokens->At(i))->GetString().Atof());
	HLTDebug("Occupancy limit set to occulimit %f", ((TObjString*)pTokens->At(i))->GetString().Atof());
      }
      else if (argument.CompareTo("rawreadermode")==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTWarning("Argument 'rawreadermode' is deprecated");      
      }
      else if (argument.CompareTo("pp-run")==0){
	HLTWarning("Argument 'pp-run' is obsolete, deconvolution is swiched off in both time and pad directions by default.");
	fClusterDeconv = false;
      }
      else if (argument.CompareTo("adc-threshold" )==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTWarning("'adc-threshold' is no longer a valid argument, please use TPCZeroSuppression component if you want to zerosuppress data.");
      }
      else if (argument.CompareTo("oldrcuformat" )==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTWarning("Argument 'oldrcuformat' is deprecated.");
      }
      else if (argument.CompareTo("unsorted" )==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTDebug("Using unsorted reading.");
	fClusterFinder->SetUnsorted(1);
      }
      else if (argument.CompareTo("nsigma-threshold")==0){
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTWarning("Argument 'nsigma-threshold' argument is obsolete.");
      }
      else if (argument.CompareTo("-update-calibdb")==0){
	fClusterFinder->UpdateCalibDB();
      }
      else {
	HLTError("unknown argument %s", argument.Data());
	iResult=-EINVAL;
	break;
      }
    }
    delete pTokens;
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }
  return iResult;
}


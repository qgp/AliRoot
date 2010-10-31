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

#include "AliHLTPHOSDigitMakerComponent.h"
#include "AliHLTCaloDigitMaker.h"
#include "AliHLTCaloDigitDataStruct.h"
#include "AliHLTPHOSMapper.h"
#include "AliHLTCaloChannelDataHeaderStruct.h"
#include "AliHLTCaloChannelDataStruct.h"
#include "AliPHOSEmcBadChannelsMap.h"
#include "AliPHOSEmcCalibData.h"
#include "TFile.h"
#include "AliCDBEntry.h"
#include "AliCDBPath.h"
#include "AliCDBManager.h"
#include <sys/stat.h>
#include <sys/types.h>


/** 
 * @file   AliHLTPHOSDigitMakerComponent.cxx
 * @author Oystein Djuvsland
 * @date   
 * @brief  A digit maker component for PHOS HLT
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

ClassImp(AliHLTPHOSDigitMakerComponent);

AliHLTPHOSDigitMakerComponent gAliHLTPHOSDigitMakerComponent;

AliHLTPHOSDigitMakerComponent::AliHLTPHOSDigitMakerComponent() :
  AliHLTCaloProcessor(),
  AliHLTCaloConstantsHandler("PHOS"),
  fDigitMakerPtr(0),
  fDigitContainerPtr(0),
  fBadChannelMap(0),
  fCalibData(0),
  fBCMInitialised(true),
  fGainsInitialised(true)
{
  //see header file for documentation
}


AliHLTPHOSDigitMakerComponent::~AliHLTPHOSDigitMakerComponent()
{
  //see header file for documentation
}

int 
AliHLTPHOSDigitMakerComponent::Deinit()
{ 
  //see header file for documentation
  if(fDigitMakerPtr)
    {
      delete fDigitMakerPtr;
      fDigitMakerPtr = 0;
    }
  return 0;
}

const char*
AliHLTPHOSDigitMakerComponent::GetComponentID()
{
  //see header file for documentation
  return "PhosDigitMaker";
}


void
AliHLTPHOSDigitMakerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
  //see header file for documentation
  list.clear();
  list.push_back(AliHLTPHOSDefinitions::fgkChannelDataType);
}

AliHLTComponentDataType 
AliHLTPHOSDigitMakerComponent::GetOutputDataType()
{
  //see header file for documentation
  return AliHLTPHOSDefinitions::fgkDigitDataType;
}


void 
AliHLTPHOSDigitMakerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  //see header file for documentation
  constBase = 0;
  inputMultiplier = (float)sizeof(AliHLTCaloDigitDataStruct)/sizeof(AliHLTCaloChannelDataStruct) + 1;
}

int 
AliHLTPHOSDigitMakerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
					std::vector<AliHLTComponentBlockData>& outputBlocks)
{
  //see header file for documentation
  UInt_t offset           = 0; 
  UInt_t mysize           = 0;
  Int_t digitCount        = 0;
  Int_t ret               = 0;

  const AliHLTComponentBlockData* iter = 0; 
  unsigned long ndx; 

  UInt_t specification = 0;
  AliHLTCaloChannelDataHeaderStruct* tmpChannelData = 0;
  
  //  fDigitMakerPtr->SetDigitHeaderPtr(reinterpret_cast<AliHLTCaloDigitHeaderStruct*>(outputPtr));

  fDigitMakerPtr->SetDigitDataPtr(reinterpret_cast<AliHLTCaloDigitDataStruct*>(outputPtr));

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      
      iter = blocks+ndx;
      
      if(iter->fDataType != AliHLTPHOSDefinitions::fgkChannelDataType)
	{
//	  HLTDebug("Data block is not of type fgkChannelDataType");
	  continue;
	}
      if(!fBCMInitialised)
      {
	 AliHLTPHOSMapper mapper;
	 Int_t module = mapper.GetModuleFromSpec(iter->fSpecification);
	 for(Int_t x = 0; x < fCaloConstants->GetNXCOLUMNSMOD(); x++)
	 {
	    for(Int_t z = 0; z < fCaloConstants->GetNZROWSMOD(); z++)
	    {
	       fDigitMakerPtr->SetBadChannel(x, z, fBadChannelMap->IsBadChannel(5-module, z+1, x+1));
	    }
	 }
	 //delete fBadChannelMap;
	 fBCMInitialised = true;
      }
      if(!fGainsInitialised)
      {
	 AliHLTPHOSMapper mapper;
	 Int_t module = mapper.GetModuleFromSpec(iter->fSpecification);
	 for(Int_t x = 0; x < fCaloConstants->GetNXCOLUMNSMOD(); x++)
	 {
	    for(Int_t z = 0; z < fCaloConstants->GetNZROWSMOD(); z++)
	    {
		fDigitMakerPtr->SetGain(x, z, fCalibData->GetHighLowRatioEmc(5-module, z+1, x+1), fCalibData->GetADCchannelEmc(5-module, z+1, x+1));
	    }
	 }
	 fGainsInitialised = true;
      }

      specification |= iter->fSpecification;
      tmpChannelData = reinterpret_cast<AliHLTCaloChannelDataHeaderStruct*>(iter->fPtr);
    
      ret = fDigitMakerPtr->MakeDigits(tmpChannelData, size-(digitCount*sizeof(AliHLTCaloDigitDataStruct)));
      if(ret == -1) 
	{
	  HLTError("Trying to write over buffer size");
	  return -ENOBUFS;
	}
      digitCount += ret; 
    }
  
  mysize += digitCount*sizeof(AliHLTCaloDigitDataStruct);

  HLTDebug("# of digits: %d, used memory size: %d, available size: %d", digitCount, mysize, size);

  if(mysize > 0) 
    {
      AliHLTComponentBlockData bd;
      FillBlockData( bd );
      bd.fOffset = offset;
      bd.fSize = mysize;
      bd.fDataType = AliHLTPHOSDefinitions::fgkDigitDataType;
      bd.fSpecification = specification;
      outputBlocks.push_back(bd);
    }

  fDigitMakerPtr->Reset();

  size = mysize; 

  return 0;
}

int
AliHLTPHOSDigitMakerComponent::DoInit(int argc, const char** argv )
{
  //see header file for documentation

  fDigitMakerPtr = new AliHLTCaloDigitMaker("PHOS");

  AliHLTCaloMapper *mapper = new AliHLTPHOSMapper();
  fDigitMakerPtr->SetMapper(mapper);
  
  Float_t mintime = 0.;
  Float_t maxtime =50.;
  
  for(int i = 0; i < argc; i++)
    {
      if(!strcmp("-lowgainfactor", argv[i]))
	{
	  fDigitMakerPtr->SetGlobalLowGainFactor(atof(argv[i+1]));
	}
      if(!strcmp("-highgainfactor", argv[i]))
	{
	  fDigitMakerPtr->SetGlobalHighGainFactor(atof(argv[i+1]));
	}
	if(!strcmp("-mintime", argv[i]))
	{
	   mintime = atof(argv[i+1]);
	}
	if(!strcmp("-maxtime", argv[i]))
	{
	   maxtime = atof(argv[i+1]);
	}
    }
 
 fDigitMakerPtr->SetTimeWindow(mintime, maxtime);

 if(GetBCMFromCDB()) return -1;
 if(GetGainsFromCDB()) return -1;
  
  //fDigitMakerPtr->SetDigitThreshold(2);

  return 0;
}


int AliHLTPHOSDigitMakerComponent::GetBCMFromCDB()
{
   fBCMInitialised = false;
   
//   HLTInfo("Getting bad channel map...");

  AliCDBPath path("PHOS","Calib","EmcBadChannels");
  if(path.GetPath())
    {
      //      HLTInfo("configure from entry %s", path.GetPath());
      AliCDBEntry *pEntry = AliCDBManager::Instance()->Get(path/*,GetRunNo()*/);
	if (pEntry) 
	{
	    fBadChannelMap = (AliPHOSEmcBadChannelsMap*)pEntry->GetObject();
	}
      else
	{
	    HLTError("can not fetch object \"%s\" from CDB", path.GetPath().Data());
	    return -1;
	}
    }
   if(!fBadChannelMap) return -1;
   return 0;
}

int AliHLTPHOSDigitMakerComponent::GetGainsFromCDB()
{
   fGainsInitialised = false;
   
//   HLTInfo("Getting bad channel map...");

  AliCDBPath path("PHOS","Calib","EmcGainPedestals");
  if(path.GetPath())
    {
      //      HLTInfo("configure from entry %s", path.GetPath());*/
      AliCDBEntry *pEntry = AliCDBManager::Instance()->Get(path/*,GetRunNo()*/);
      if (pEntry) 
	{
	    fCalibData = (AliPHOSEmcCalibData*)pEntry->GetObject();
	}
      else	
	{
	    HLTError("can not fetch object \"%s\" from CDB", path.GetPath().Data());
	    return -1;
	}
    }
    
    if(!fCalibData) return -1;
   return 0;
   
}


AliHLTComponent*
AliHLTPHOSDigitMakerComponent::Spawn()
{
  //see header file for documentation
  return new AliHLTPHOSDigitMakerComponent();
}

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

#include "AliHLTEMCALDigitMakerComponent.h"
#include "AliHLTCaloDigitMaker.h"
#include "AliHLTCaloDigitDataStruct.h"
#include "AliHLTCaloChannelDataHeaderStruct.h"
#include "AliHLTCaloChannelDataStruct.h"
#include "AliHLTEMCALMapper.h"
#include "AliHLTEMCALDefinitions.h"
#include "TFile.h"
#include <sys/stat.h>
#include <sys/types.h>


/** 
 * @file   AliHLTEMCALDigitMakerComponent.cxx
 * @author Oystein Djuvsland
 * @date   
 * @brief  A digit maker component for EMCAL HLT
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt


AliHLTEMCALDigitMakerComponent gAliHLTEMCALDigitMakerComponent;

AliHLTEMCALDigitMakerComponent::AliHLTEMCALDigitMakerComponent() :
  AliHLTCaloProcessor(),
  fDigitMakerPtr(0),
  fDigitContainerPtr(0)
{
  //see header file for documentation
}


AliHLTEMCALDigitMakerComponent::~AliHLTEMCALDigitMakerComponent()
{
  //see header file for documentation
}

int 
AliHLTEMCALDigitMakerComponent::Deinit()
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
AliHLTEMCALDigitMakerComponent::GetComponentID()
{
  //see header file for documentation
  return "EmcalDigitMaker";
}


void
AliHLTEMCALDigitMakerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
  //see header file for documentation
  list.clear();
  list.push_back(AliHLTEMCALDefinitions::fgkChannelDataType);
}

AliHLTComponentDataType 
AliHLTEMCALDigitMakerComponent::GetOutputDataType()
{
  //see header file for documentation
//  return AliHLTCaloDefinitions::fgkDigitDataType|kAliHLTDataOriginEMCAL;
  return AliHLTEMCALDefinitions::fgkDigitDataType;
}


void 
AliHLTEMCALDigitMakerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  //see header file for documentation
  constBase = 0;
  inputMultiplier = (float)sizeof(AliHLTCaloDigitDataStruct)/sizeof(AliHLTCaloChannelDataStruct) + 1;
}

int 
AliHLTEMCALDigitMakerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
					std::vector<AliHLTComponentBlockData>& outputBlocks)
{
  //see header file for documentation
  UInt_t offset           = 0; 
  UInt_t mysize           = 0;
  Int_t digitCount        = 0;
  Int_t ret               = 0;

  AliHLTUInt8_t* outBPtr;
  outBPtr = outputPtr;
  const AliHLTComponentBlockData* iter = 0; 
  unsigned long ndx; 

  UInt_t specification = 0;
  AliHLTCaloChannelDataHeaderStruct* tmpChannelData = 0;
  
  //  fDigitMakerPtr->SetDigitHeaderPtr(reinterpret_cast<AliHLTCaloDigitHeaderStruct*>(outputPtr));

  fDigitMakerPtr->SetDigitDataPtr(reinterpret_cast<AliHLTCaloDigitDataStruct*>(outputPtr));

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      
      if(iter->fDataType != AliHLTEMCALDefinitions::fgkChannelDataType)
	{
	  HLTDebug("Data block is not of type fgkChannelDataType");
	  continue;
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
      bd.fDataType = AliHLTEMCALDefinitions::fgkDigitDataType;
      bd.fSpecification = specification;
      outputBlocks.push_back(bd);
    }

  fDigitMakerPtr->Reset();

  size = mysize; 

  return 0;
}


int
AliHLTEMCALDigitMakerComponent::DoInit(int argc, const char** argv )
{
  //see header file for documentation

  fDigitMakerPtr = new AliHLTCaloDigitMaker("EMCAL");

  AliHLTCaloMapper *mapper = new AliHLTEMCALMapper(2);
  fDigitMakerPtr->SetMapper(mapper);
  
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
    }
 
  //fDigitMakerPtr->SetDigitThreshold(2);

  return 0;
}

int AliHLTEMCALDigitMakerComponent::GetBCMFromCDB()
{
   // See header file for class documentation




   return 0;
}

int AliHLTEMCALDigitMakerComponent::GetGainsFromCDB()
{
   // See header file for class documentation
   
   return 0;
}



AliHLTComponent*
AliHLTEMCALDigitMakerComponent::Spawn()
{
  //see header file for documentation
  return new AliHLTEMCALDigitMakerComponent();
}

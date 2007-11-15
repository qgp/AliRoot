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

#include "AliHLTPHOSRcuDigitMakerComponent.h"
#include "AliHLTPHOSRcuDigitMaker.h"
#include "TTree.h"
#include "AliHLTPHOSRcuProcessor.h"
#include "AliHLTPHOSRcuCellEnergyDataStruct.h"
#include "AliHLTPHOSRcuDigitContainerDataStruct.h"
#include "TClonesArray.h"
#include "TFile.h"
#include <sys/stat.h>
#include <sys/types.h>

const AliHLTComponentDataType AliHLTPHOSRcuDigitMakerComponent::fgkInputDataTypes[]={kAliHLTVoidDataType,{0,"",""}};

AliHLTPHOSRcuDigitMakerComponent gAliHLTPHOSRcuDigitMakerComponent;

AliHLTPHOSRcuDigitMakerComponent::AliHLTPHOSRcuDigitMakerComponent() :
  AliHLTPHOSRcuProcessor(),
  fDigitMakerPtr(0),
  fEvtCnt(0)
{
  //comment
}

AliHLTPHOSRcuDigitMakerComponent::~AliHLTPHOSRcuDigitMakerComponent()
{
  //comment
}

int 
AliHLTPHOSRcuDigitMakerComponent::Deinit()
{ 
  //comment
  if(fDigitMakerPtr)
    {
      delete fDigitMakerPtr;
      fDigitMakerPtr = 0;
    }
  return 0;
}

const char*
AliHLTPHOSRcuDigitMakerComponent::GetComponentID()
{
  //comment
  return "PhosRcuDigitMaker";
}

void

AliHLTPHOSRcuDigitMakerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
 //Get datatypes for input
  const AliHLTComponentDataType* pType=fgkInputDataTypes;
  while (pType->fID!=0) {
    list.push_back(*pType); 
    pType++;
  }
}

AliHLTComponentDataType 
AliHLTPHOSRcuDigitMakerComponent::GetOutputDataType()
{
  //comment
  return AliHLTPHOSDefinitions::fgkAliHLTDigitDataType;
}


void 
AliHLTPHOSRcuDigitMakerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  //comment
  constBase = 30;
  inputMultiplier = 1;
}

int 
AliHLTPHOSRcuDigitMakerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
					std::vector<AliHLTComponentBlockData>& outputBlocks)
{
   //Do event
     
  UInt_t tSize            = 0;
  UInt_t offset           = 0; 
  UInt_t mysize           = 0;
  //Int_t nRecPoints        = 0;
  //Int_t index             = 0;
  
  //Int_t fileCount = 0;
  Int_t digitCount = 0;
  //char filename [50];


  AliHLTUInt8_t* outBPtr;
  outBPtr = outputPtr;
  const AliHLTComponentBlockData* iter = 0; 
  unsigned long ndx; 
  fDigitContainerPtr = (AliHLTPHOSRcuDigitContainerDataStruct*)outBPtr;
  //fDigitMakerPtr->SetDigitContainerStruct(fDigitContainerPtr);
  fDigitMakerPtr->SetDigitContainerStruct((AliHLTPHOSRcuDigitContainerDataStruct*)outBPtr);

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      
      if(iter->fDataType != AliHLTPHOSDefinitions::fgkCellEnergyDataType)
	{
	  //	  cout << "Warning: data type is not fgkCellEnergyDataType " << endl;
	  continue;

	}
      digitCount = fDigitMakerPtr->MakeDigits(reinterpret_cast<AliHLTPHOSRcuCellEnergyDataStruct*>(iter->fPtr));
    }
  fEvtCnt++;
  
  mysize = 0;
  offset = tSize;
      
  mysize += sizeof(AliHLTPHOSRcuDigitContainerDataStruct);
  ((AliHLTPHOSRcuDigitContainerDataStruct*)outBPtr)->fNDigits = digitCount;
  AliHLTComponentBlockData bd;
  FillBlockData( bd );
  bd.fOffset = offset;
  bd.fSize = mysize;
  bd.fDataType = AliHLTPHOSDefinitions::fgkAliHLTDigitDataType;
  bd.fSpecification = 0xFFFFFFFF;
  outputBlocks.push_back( bd );
       
  tSize += mysize;
  outBPtr += mysize;
      
  if( tSize > size )
    {
      Logging( kHLTLogFatal, "HLT::AliHLTPHOSRcuDigitMakerComponent::DoEvent", "Too much data",
	       "Data written over allowed buffer. Amount written: %lu, allowed amount: %lu."
	       , tSize, size );
      return EMSGSIZE;
    }
      
  fDigitMakerPtr->Reset();
  
  if(fEvtCnt % 10 == 0)
    {
      cout << "Event #: " << fEvtCnt << endl;
      cout << "  - Number of digits found: " << digitCount << endl;
    }
  
  return 0;
}


int
AliHLTPHOSRcuDigitMakerComponent::DoInit(int argc, const char** argv )
{
  //Do initialization

  fDigitMakerPtr = new AliHLTPHOSRcuDigitMaker();
  
  for(int i = 0; i < argc; i++)
    {
      if(!strcmp("-threshold", argv[i]))
	fDigitMakerPtr->SetDigitThreshold(atoi(argv[i+1]));
      if(!strcmp("-presamples", argv[i]))
	fDigitMakerPtr->SetNrPresamples(atoi(argv[i+1]));
    }
 
  //fDigitMakerPtr->SetDigitThreshold(2);

  return 0;
}

AliHLTComponent*
AliHLTPHOSRcuDigitMakerComponent::Spawn()
{
  //comment
  return new AliHLTPHOSRcuDigitMakerComponent();
}

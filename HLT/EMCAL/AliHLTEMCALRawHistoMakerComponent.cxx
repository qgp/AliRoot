

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 * INFN, Laboratori Nazionali di Frascati                                 *
 * Primary Authors: Federico Ronchetti                                    *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliHLTEMCALRawHistoMakerComponent.h"
#include "AliHLTEMCALRawHistoMaker.h"

#include "AliHLTCaloChannelDataHeaderStruct.h"
#include "AliHLTCaloChannelDataStruct.h"

#include "AliHLTCaloDefinitions.h"

#include "TString.h"



// root stuff
#include "TFile.h"
#include "TProfile2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include <sys/stat.h>
#include <sys/types.h>

#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliCDBPath.h"
/** 
 * @file   AliHLTEMCALRawHistoMakerComponent.cxx
 * @author Federico Ronchetti
 * @date   
 * @brief  A component to pusk back histograms for EMCAL HLT
 */

//FIXME
AliHLTEMCALRawHistoMakerComponent gAliHLTEMCALRawHistoMakerComponent;

AliHLTEMCALRawHistoMakerComponent::AliHLTEMCALRawHistoMakerComponent() :
				  AliHLTCaloProcessor(),
				  fRawHistoMakerPtr(0),
				  fPushFraction(10),
				  fLocalEventCount(0),
				  fRootFileName("histofile_local.root"),
				  fBeVerbose(0)
{
	//see header file for documentation
}


AliHLTEMCALRawHistoMakerComponent::~AliHLTEMCALRawHistoMakerComponent()
{
	//see header file for documentation
}


int
AliHLTEMCALRawHistoMakerComponent::DoInit(int argc, const char** argv )
{
	//see header file for documentation

	fRawHistoMakerPtr = new AliHLTEMCALRawHistoMaker();

	for(int i = 0; i < argc; i++)
	{
		if(!strcmp("-roothistofilename", argv[i]))
			fRootFileName = argv[i+1];

		if(!strcmp("-pushfraction", argv[i]))
			fPushFraction = atoi(argv[i+1]);

		if(!strcmp("-beverbose", argv[i]))
			fBeVerbose = atoi(argv[i+1]);

	}

	if (fBeVerbose)
		cout << "\nI-RAWHISTOMAKERCOMPONENT: local root file name is: " << fRootFileName << endl;

	return 0;
}


int 
AliHLTEMCALRawHistoMakerComponent::Deinit()
{ 
	//see header file for documentation
	if(fRawHistoMakerPtr)
	{
		delete fRawHistoMakerPtr;
		fRawHistoMakerPtr = 0;
	}

	return 0;
}

const char*
AliHLTEMCALRawHistoMakerComponent::GetComponentID()
{
	//see header file for documentation
	return "EmcalRawHistoMaker";
}


void
AliHLTEMCALRawHistoMakerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
	//see header file for documentation
	list.clear();
	list.push_back(AliHLTEMCALDefinitions::fgkChannelDataType);
}

AliHLTComponentDataType 
AliHLTEMCALRawHistoMakerComponent::GetOutputDataType()
{
	//see header file for documentation
	return kAliHLTDataTypeHistogram | kAliHLTDataOriginEMCAL;
}

void 
AliHLTEMCALRawHistoMakerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
	//see header file for documentation
	constBase = 0;
	// to be reviewed later
	inputMultiplier = 100;
}

int 
AliHLTEMCALRawHistoMakerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
		AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
		std::vector<AliHLTComponentBlockData>& outputBlocks)
{
	//see header file for documentation
	UInt_t mysize           = 0;
	Int_t ret               = 0;


	AliHLTUInt8_t* outBPtr;
	outBPtr = outputPtr;
	const AliHLTComponentBlockData* iter = 0;
	unsigned long ndx;

	UInt_t specification = 0;
	
	for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
	{
	  
	  	AliHLTCaloClusterHeaderStruct *caloClusterHeaderPtr = 0;
		AliHLTCaloChannelDataHeaderStruct* tmpChannelData = 0;

		iter = blocks+ndx;
		
		if (fBeVerbose) 
		  PrintComponentDataTypeInfo(iter->fDataType);
		
		
		if (iter->fDataType == AliHLTEMCALDefinitions::fgkChannelDataType)
		  {
		  
		    tmpChannelData = reinterpret_cast<AliHLTCaloChannelDataHeaderStruct*>(iter->fPtr);
				    
		    if (fBeVerbose)
		      printf ("\nI-RAWHISTOMAKERCOMPONENT: Number of active channels in block: %d\n",tmpChannelData->fNChannels);
		    
		  } 

		else if (iter->fDataType == kAliHLTDataTypeCaloCluster)
		  caloClusterHeaderPtr = reinterpret_cast<AliHLTCaloClusterHeaderStruct*>(iter->fPtr);
		    
		else
		  
		  {
		    if (fBeVerbose)
        {
		      HLTWarning("\nI-RAWHISTOMAKERCOMPONENT: Data block does not of channel or cluster type \n");
        }
		    else
        {
		      HLTDebug("\nI-RAWHISTOMAKERCOMPONENT: Data block does not of channel or cluster type \n");
        }
		    continue;
		  }

	
		specification |= iter->fSpecification;
		ret = fRawHistoMakerPtr->MakeHisto(tmpChannelData, caloClusterHeaderPtr, fBeVerbose);

	}


	fLocalEventCount++;

	TFile rootHistFile(fRootFileName,"recreate");
	
	fRawHistoMakerPtr->GetHistograms()->Write();
	
	if (fLocalEventCount%fPushFraction == 0) {
	  
	  if (fBeVerbose)
	    cout << "\nI-RAWHISTOMAKERCOMPONENT: pushback done at " << fLocalEventCount << " events " << endl;
	  
	  PushBack(fRawHistoMakerPtr->GetHistograms(), kAliHLTDataTypeTObjArray | kAliHLTDataOriginEMCAL , specification);
	}
	
	size = mysize;
	
	return 0;
}


AliHLTComponent*
AliHLTEMCALRawHistoMakerComponent::Spawn()
{
	//see header file for documentation
	return new AliHLTEMCALRawHistoMakerComponent();
}

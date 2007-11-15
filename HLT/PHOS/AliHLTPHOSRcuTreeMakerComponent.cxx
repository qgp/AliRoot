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


 
#include "AliHLTPHOSRcuTreeMakerComponent.h"
#include "AliHLTPHOSRcuTreeMaker.h"
#include "AliHLTPHOSRcuProcessor.h"
#include "AliHLTPHOSDigitDataStruct.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TObject.h"
#include <fstream>
#include "TFile.h"
#include <sys/stat.h>
#include <sys/types.h>

const AliHLTComponentDataType AliHLTPHOSRcuTreeMakerComponent::fgkInputDataTypes[]={kAliHLTVoidDataType,{0,"",""}};

AliHLTPHOSRcuTreeMakerComponent gAliHLTPHOSRcuTreeMakerComponent;

AliHLTPHOSRcuTreeMakerComponent::AliHLTPHOSRcuTreeMakerComponent() :
  AliHLTPHOSRcuProcessor(),
  fDigitTreePtr(0),
  fEventCount(0),
  fWriteInterval(1000)
{
  //comment
}

AliHLTPHOSRcuTreeMakerComponent::~AliHLTPHOSRcuTreeMakerComponent()
{
  //comment
}

int 
AliHLTPHOSRcuTreeMakerComponent::Deinit()
{
  //comment
  //  cout << "Printing file...";

  /*
  char filename [50];
  sprintf(filename, "%s/run%d_digitTree_rcuX_%d_rcuZ_%d_%d.root", fDirectory, fRunNb,(fEventCount/fWriteInterval));
  TFile *outfile = new TFile(filename,"recreate");
  fDigitTreePtr->Write();
  delete outfile;
  outfile = 0;
  cout << "Done!\n";
  */
  
  Write();


  if(fDigitTreePtr) 
    {
      delete fDigitTreePtr;
      fDigitTreePtr = 0;
    }
  return 0;
}



const char*
AliHLTPHOSRcuTreeMakerComponent::GetComponentID()
{
  //comment
  return "PhosRcuTreeMaker";
}

void
AliHLTPHOSRcuTreeMakerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
  //comment
 //Get datatypes for input
  const AliHLTComponentDataType* pType=fgkInputDataTypes;
  while (pType->fID!=0) {
    list.push_back(*pType); 
    pType++;
  }
}

AliHLTComponentDataType 
AliHLTPHOSRcuTreeMakerComponent::GetOutputDataType()
{
  //comment
  return AliHLTPHOSDefinitions::fgkAliHLTRootTreeDataType;
}

void 
AliHLTPHOSRcuTreeMakerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  //comment
  constBase = 30;
  inputMultiplier = 1;
}

int 
AliHLTPHOSRcuTreeMakerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* /*outputPtr*/, AliHLTUInt32_t& /*size*/,  //TODO: I think size should be set to zero when returning from this method if not data was written to the output buffer.
					std::vector<AliHLTComponentBlockData>& /*outputBlocks*/)

{
  //Do event

  Bool_t digitEvent;
  Int_t nDigits = 0;
  Int_t totalDigits = 0;

  const AliHLTComponentBlockData* iter = 0;
  unsigned long ndx;

  for ( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks + ndx;

      if ( iter->fDataType == AliHLTPHOSDefinitions::fgkAliHLTDigitDataType )

        {
          digitEvent = true;
          nDigits  = fTreeMakerPtr->MakeDigitArray ( reinterpret_cast<AliHLTPHOSRcuDigitContainerDataStruct*> ( iter->fPtr ), totalDigits );
          totalDigits += nDigits;
	  //cout << totalDigits << endl;
          continue;
        }
      if ( iter->fDataType == AliHLTPHOSDefinitions::fgkAliHLTClusterDataType )
        {
          //
        }
    }
  fEventCount++;
  fTreeMakerPtr->FillDigitTree();
  
  if(fEventCount%fWriteInterval == 0)
    {
      Write();
      ResetTrees();
    }

return 0;

}

int
AliHLTPHOSRcuTreeMakerComponent::DoInit ( int argc, const char** argv )
{
  //comment
  fTreeMakerPtr = new AliHLTPHOSRcuTreeMaker();
  fDigitTreePtr = new TTree ( "digitTree", "Digits tree" );
  fDirectory = new char[50];

  for ( int i = 0; i < argc; i++ )
    {
      if ( !strcmp ( "-path", argv[i] ) )
        {
          strcpy ( fDirectory, argv[i+1] );
        }
      if ( !strcmp ( "-writeinterval", argv[i] ) )
	{
	  fWriteInterval = atoi(argv[i+1]);
	}
    }

  fTreeMakerPtr->SetDigitTree(fDigitTreePtr);
    
  fstream runNbFile;
  //Int_t newRunNb;
  runNbFile.open("/opt/HLT-public/rundir/runNumber.txt");
  runNbFile >> fRunNb;
  runNbFile.close();
  /*  newRunNb = fRunNb + 1;
  runNbFile.open("/opt/HLT-public/rundir/runNumber.txt");
  runNbFile << newRunNb;
  runNbFile.close();*/
  
  cout << endl << "Run number is: " << fRunNb  << "  -- Check that this is correct!!!\n";

  return 0;
  
}


AliHLTComponent*
AliHLTPHOSRcuTreeMakerComponent::Spawn()
{
  //comment
  return new AliHLTPHOSRcuTreeMakerComponent();
}

void
AliHLTPHOSRcuTreeMakerComponent::Write()
{
  //comment
  cout << "Writing file...";

  char filename [256];
  sprintf(filename, "%s/run%d_%d_digitTree_mod%d_rcuX%d_rcuZ%d_.root", fDirectory, fRunNb,(fEventCount/fWriteInterval - 1), fModuleID, fRcuX, fRcuZ);
  TFile *outfile = new TFile(filename,"recreate");
  fDigitTreePtr->Write();
  delete outfile;
  outfile = 0;
  cout << "Done!\n";

}

void
AliHLTPHOSRcuTreeMakerComponent::ResetTrees()
{
  //comment
  delete fDigitTreePtr;
  fDigitTreePtr = new TTree("digitTree", "Digits tree");
  fTreeMakerPtr->SetDigitTree(fDigitTreePtr);
}
  
  
  
  
  
  
  




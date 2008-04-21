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

/** @file   AliHLTTPCEsdWriterComponent.cxx
    @author Matthias Richter
    @date   
    @brief  Writer component to store tracks of the HLT TPC conformal
            mapping tracker in the AliESD format
*/

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include <cassert>
#include "AliHLTTPCEsdWriterComponent.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "TTree.h"
#include "TList.h"
#include "AliHLTTPCTrack.h"
#include "AliHLTTPCTrackArray.h"
#include "AliHLTTPCTrackletDataFormat.h"
#include "AliHLTTPCDefinitions.h"
#include "AliHLTTPCTransform.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCEsdWriterComponent)

AliHLTTPCEsdWriterComponent::AliHLTTPCEsdWriterComponent()
  :
  fSolenoidBz(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCEsdWriterComponent::~AliHLTTPCEsdWriterComponent()
{
  // see header file for class documentation
}

AliHLTTPCEsdWriterComponent::AliWriter::AliWriter()
  :
  fTree(NULL),
  fESD(NULL),
  fBase(new AliHLTTPCEsdWriterComponent)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCEsdWriterComponent::AliWriter::~AliWriter()
{
  // see header file for class documentation
  if (fBase) delete fBase;
  fBase=NULL;
}

void AliHLTTPCEsdWriterComponent::AliWriter::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  // see header file for class documentation
  list.push_back(AliHLTTPCDefinitions::fgkTrackSegmentsDataType);
  list.push_back(AliHLTTPCDefinitions::fgkTracksDataType);
}

int AliHLTTPCEsdWriterComponent::AliWriter::InitWriter()
{
  // see header file for class documentation
  int iResult=0;
  fESD = new AliESDEvent;
  if (fESD) {
    fESD->CreateStdContent();
    fTree = new TTree("esdTree", "Tree with HLT ESD objects");
    if (fTree) {
      fTree->SetDirectory(0);
      fESD->WriteToTree(fTree);
    }
  }
  if (fTree==NULL) {
    iResult=-ENOMEM;
  }

  if (iResult>=0) {
    iResult=fBase->Reconfigure(NULL, NULL);
  }

  return iResult;
}

int AliHLTTPCEsdWriterComponent::AliWriter::CloseWriter()
{
  // see header file for class documentation
  int iResult=0;
  if (fTree) {
    // the esd structure is written to the user info and is
    // needed in te ReadFromTree method to read all objects correctly
    if (fESD) fTree->GetUserInfo()->Add(fESD);
    WriteObject(kAliHLTVoidEventID, fTree);
    fTree->GetUserInfo()->Clear();
    TTree* pTree=fTree;
    fTree=NULL;
    delete pTree;
  } else {
    HLTWarning("not initialized");
  }

  if (fESD) {
    delete fESD;
  }
  iResult=AliHLTRootFileWriterComponent::CloseWriter();
  return iResult;
}

int AliHLTTPCEsdWriterComponent::AliWriter::DumpEvent( const AliHLTComponentEventData& evtData,
					    const AliHLTComponentBlockData* blocks, 
					    AliHLTComponentTriggerData& /*trigData*/ )
{
  // see header file for class documentation
  int iResult=0;
  TTree* pTree=fTree;
  assert(fBase);
  if (pTree && fBase) {
    if (fESD) {
      AliESDEvent* pESD=fESD;

      iResult=fBase->ProcessBlocks(pTree, pESD, blocks, (int)evtData.fBlockCnt);

    } else {
      iResult=-ENOMEM;
    }
  }
  return iResult;
}

int AliHLTTPCEsdWriterComponent::AliWriter::ScanArgument(int argc, const char** argv)
{
  // see header file for class documentation
  int iResult=AliHLTRootFileWriterComponent::ScanArgument(argc, argv);
  return iResult;
}

int AliHLTTPCEsdWriterComponent::ProcessBlocks(TTree* pTree, AliESDEvent* pESD,
					       const AliHLTComponentBlockData* blocks,
					       int nBlocks, int* pMinSlice,
					       int* pMaxSlice)
{
  // see header file for class documentation
  int iResult=0;
  if (pESD && blocks) {
      pESD->SetMagneticField(fSolenoidBz);
      const AliHLTComponentBlockData* iter = NULL;
      AliHLTTPCTrackletData* inPtr=NULL;
      int bIsTrackSegs=0;
 
      for (int ndx=0; ndx<nBlocks && iResult>=0; ndx++) {
	iter = blocks+ndx;
	if ( (bIsTrackSegs=(iter->fDataType == AliHLTTPCDefinitions::fgkTrackSegmentsDataType))==1 ||
	     iter->fDataType == AliHLTTPCDefinitions::fgkTracksDataType ) {
	  Int_t minslice=AliHLTTPCDefinitions::GetMinSliceNr(iter->fSpecification);
	  Int_t maxslice=AliHLTTPCDefinitions::GetMaxSliceNr(iter->fSpecification);
	  if (bIsTrackSegs==0) {
	    // slice parameter and data specification ignored, tracks already in global coordinates
	    minslice=-1;
	    maxslice=-1;
	    if (pMinSlice) *pMinSlice=0;
	    if (pMaxSlice) *pMaxSlice=AliHLTTPCTransform::GetNSlice()-1;
	  } else {
	    if (pMinSlice && (*pMinSlice==-1 || *pMinSlice>minslice)) *pMinSlice=minslice;
	    if (pMaxSlice && (*pMaxSlice==-1 || *pMaxSlice<maxslice)) *pMaxSlice=maxslice;
	  }
	  //HLTDebug("dataspec %#x minslice %d", iter->fSpecification, minslice);
	  if (minslice >=-1 && minslice<AliHLTTPCTransform::GetNSlice()) {
	    if (minslice!=maxslice) {
	      HLTWarning("data from multiple sectors in one block: "
			 "possible mismatch in treatment of local coordinate system");
	    }
	    AliHLTTPCTrackArray tracks;
	    inPtr=(AliHLTTPCTrackletData*)iter->fPtr;
	    HLTDebug("reading block %d (slice %d): %d tracklets", ndx, minslice, inPtr->fTrackletCnt);
	    tracks.FillTracks(inPtr->fTrackletCnt, inPtr->fTracklets, minslice, 0/*don't rotate*/);
	    if ((iResult=Tracks2ESD(&tracks, pESD))>=0) {
	    }
	  } else {
	    HLTError("invalid sector number");
	    iResult=-EBADF;
	  }
	}
      }
      if (iResult>=0 && pTree) {
	pTree->Fill();
      }

      pESD->Reset();
    
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTTPCEsdWriterComponent::Tracks2ESD(AliHLTTPCTrackArray* pTracks, AliESDEvent* pESD)
{
  // see header file for class documentation
  int iResult=0;
  if (pTracks && pESD) {
    HLTDebug("converting %d tracks from track array", pTracks->GetNTracks());
    for (int i=0; i<pTracks->GetNTracks() && iResult>=0; i++) {
      AliHLTTPCTrack* pTrack=(*pTracks)[i];
      if (pTrack) {
	//HLTDebug("convert track %d", i);
	//pTrack->Print();
	int iLocal=pTrack->Convert2AliKalmanTrack();
	if (iLocal>=0) {
	AliESDtrack iotrack;
	iotrack.UpdateTrackParams(pTrack,AliESDtrack::kTPCin);
	iotrack.SetTPCPoints(pTrack->GetPoints());
	pESD->AddTrack(&iotrack);
	} else {
	  HLTError("conversion to AliKalmanTrack failed for track %d of %d", i, pTracks->GetNTracks());
	}
      } else {
	HLTError("internal mismatch in array");
	iResult=-EFAULT;
      }
    }
    
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTTPCEsdWriterComponent::Configure(const char* arguments)
{
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
      
      if (argument.CompareTo("-solenoidBz")==0) {
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;
	HLTInfo("Magnetic Field set to: %s", ((TObjString*)pTokens->At(i))->GetString().Data());
	fSolenoidBz=((TObjString*)pTokens->At(i))->GetString().Atof();
	continue;
      } else {
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

int AliHLTTPCEsdWriterComponent::Reconfigure(const char* cdbEntry, const char* chainId)
{
  // see header file for class documentation
  int iResult=0;
  const char* path="HLT/ConfigHLT/SolenoidBz";
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
	HLTInfo("received configuration object string: \'%s\'", pString->GetString().Data());
	iResult=Configure(pString->GetString().Data());
      } else {
	HLTError("configuration object \"%s\" has wrong type, required TObjString", path);
      }
    } else {
      HLTError("can not fetch object \"%s\" from CDB", path);
    }
  }
  
  return iResult;
}

AliHLTTPCEsdWriterComponent::AliConverter::AliConverter()
  :
  fBase(new AliHLTTPCEsdWriterComponent),
  fWriteTree(1)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCEsdWriterComponent::AliConverter::~AliConverter()
{
  // see header file for class documentation
  if (fBase) delete fBase;
  fBase=NULL;
}

void AliHLTTPCEsdWriterComponent::AliConverter::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  // see header file for class documentation
  list.push_back(AliHLTTPCDefinitions::fgkTrackSegmentsDataType);
  list.push_back(AliHLTTPCDefinitions::fgkTracksDataType);
}

AliHLTComponentDataType AliHLTTPCEsdWriterComponent::AliConverter::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTDataTypeESDTree;
}

void AliHLTTPCEsdWriterComponent::AliConverter::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  // see header file for class documentation
  constBase=2000000;
  inputMultiplier=10.0;
}

int AliHLTTPCEsdWriterComponent::AliConverter::DoInit(int argc, const char** argv)
{
  // see header file for class documentation
  int iResult=0;
  TString argument="";
  int bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

    // -notree
    if (argument.CompareTo("-notree")==0) {
      fWriteTree=0;

      // -tree
    } else if (argument.CompareTo("-tree")==0) {
      fWriteTree=1;

    } else {
      HLTError("unknown argument %s", argument.Data());
      break;
    }
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }

  if (iResult>=0) {
    iResult=fBase->Reconfigure(NULL, NULL);
  }

  return iResult;
}

int AliHLTTPCEsdWriterComponent::AliConverter::DoDeinit()
{
  // see header file for class documentation
  return 0;
}

int AliHLTTPCEsdWriterComponent::AliConverter::DoEvent(const AliHLTComponentEventData& evtData, 
						       const AliHLTComponentBlockData* blocks, 
						       AliHLTComponentTriggerData& /*trigData*/,
						       AliHLTUInt8_t* /*outputPtr*/, 
						       AliHLTUInt32_t& /*size*/,
						       AliHLTComponentBlockDataList& /*outputBlocks*/ )
{
  // see header file for class documentation
  int iResult=0;
  assert(fBase);
  AliESDEvent* pESD = new AliESDEvent;
  if (pESD && fBase) {
    pESD->CreateStdContent();
    TTree* pTree = NULL;
    // TODO: Matthias 06.12.2007
    // Tried to write the ESD directly instead to a tree, but this did not work
    // out. Information in the ESD is different, needs investigation.
    if (fWriteTree)
      pTree = new TTree("esdTree", "Tree with HLT ESD objects");
    if (pTree) {
      pTree->SetDirectory(0);
      pESD->WriteToTree(pTree);
    }

    if ((iResult=fBase->ProcessBlocks(pTree, pESD, blocks, (int)evtData.fBlockCnt))>=0) {
	// TODO: set the specification correctly
      if (pTree) {
	// the esd structure is written to the user info and is
	// needed in te ReadFromTree method to read all objects correctly
	pTree->GetUserInfo()->Add(pESD);
	iResult=PushBack(pTree, kAliHLTDataTypeESDTree|kAliHLTDataOriginTPC, 0);
      } else {
	iResult=PushBack(pESD, kAliHLTDataTypeESDObject|kAliHLTDataOriginTPC, 0);
      }
    }
    if (pTree) {
      // clear user info list to prevent objects from being deleted
      pTree->GetUserInfo()->Clear();
      delete pTree;
    }

    delete pESD;
  }
  return iResult;
}


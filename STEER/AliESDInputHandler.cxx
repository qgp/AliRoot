/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//-------------------------------------------------------------------------
//     Event handler for ESD input 
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <TArchiveFile.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjString.h>
#include <TProcessID.h>

#include "AliESDInputHandler.h"
#include "AliESDEvent.h"
#include "AliESD.h"
#include "AliRunTag.h"
#include "AliEventTag.h"
#include "AliLog.h"

ClassImp(AliESDInputHandler)

static Option_t *gESDDataType = "ESD";

//______________________________________________________________________________
AliESDInputHandler::AliESDInputHandler() :
  AliInputEventHandler(),
  fEvent(0x0),
  fAnalysisType(0),
  fNEvents(0),
  fHLTEvent(0x0),
  fHLTTree(0x0),
  fUseHLT(kFALSE),
  fUseTags(kFALSE),
  fChainT(0),
  fTreeT(0),
  fRunTag(0)
{
  // default constructor
}

//______________________________________________________________________________
AliESDInputHandler::~AliESDInputHandler() 
{
  //  destructor
  //  delete fEvent;
}

//______________________________________________________________________________
AliESDInputHandler::AliESDInputHandler(const char* name, const char* title):
    AliInputEventHandler(name, title), fEvent(0x0), fAnalysisType(0),
     fNEvents(0),  fHLTEvent(0x0), fHLTTree(0x0), fUseHLT(kFALSE), fUseTags(kFALSE), fChainT(0), fTreeT(0), fRunTag(0)
{
    // Constructor
}

Bool_t AliESDInputHandler::Init(TTree* tree,  Option_t* opt)
{
    // Initialisation necessary for each new tree 
    fAnalysisType = opt;
    fTree         = tree;
    
    if (!fTree) return kFALSE;
    fTree->GetEntry(0);
    
    // Get pointer to ESD event
    SwitchOffBranches();
    SwitchOnBranches();
    
    if (!fEvent) fEvent = new AliESDEvent();
    fEvent->ReadFromTree(fTree);
    fNEvents = fTree->GetEntries();

    if (fUseHLT) {
	// Get HLTesdTree from current file
	TTree* cTree = tree;
	if (fTree->GetTree()) cTree = fTree->GetTree();
	TFile* cFile = cTree->GetCurrentFile();
	cFile->GetObject("HLTesdTree", fHLTTree);
	if (fHLTEvent) {
	    delete fHLTEvent;
	    fHLTEvent = 0;
	}
	if (fHLTTree) {
	  if (!fHLTEvent) fHLTEvent = new AliESDEvent();
	  fHLTEvent->ReadFromTree(fHLTTree);
	}
    }
    return kTRUE;
}

Bool_t AliESDInputHandler::BeginEvent(Long64_t entry)
{
    // Copy from old to new format if necessary
  AliESD* old = ((AliESDEvent*) fEvent)->GetAliESDOld();
  if (old) {
	((AliESDEvent*)fEvent)->CopyFromOldESD();
	old->Reset();
  }

  if (fHLTTree) {
      fHLTTree->GetEntry(entry);
  }
  
  return kTRUE;
}

Bool_t  AliESDInputHandler::FinishEvent()
{
    // Finish the event 
    if(fEvent)fEvent->Reset();
    return kTRUE;
} 

Bool_t AliESDInputHandler::Notify(const char* path)
{
    // Notify a directory change
    AliInfo(Form("Directory change %s \n", path));
    //
    if (!fUseTags) return (kTRUE);
    
    Bool_t zip = kFALSE;
    
    TString fileName(path);
    if(fileName.Contains("#AliESDs.root")){
	zip = kTRUE;
    } 
    else if (fileName.Contains("AliESDs.root")){
	fileName.ReplaceAll("AliESDs.root", "");
    }
    else if(fileName.Contains("#AliAOD.root")){
	zip = kTRUE;
    }
    else if(fileName.Contains("AliAOD.root")){
	fileName.ReplaceAll("AliAOD.root", "");
    }
    else if(fileName.Contains("#galice.root")){
	// For running with galice and kinematics alone...
	zip = kTRUE;
    }
    else if(fileName.Contains("galice.root")){
	// For running with galice and kinematics alone...
	fileName.ReplaceAll("galice.root", "");
    }

    
    TString pathName("./");
    if (fileName.Length() != 0) {
	pathName = fileName;
    }
    
    printf("AliESDInputHandler::Notify() Path: %s\n", pathName.Data());

    if (fRunTag) {
	fRunTag->Clear();
    } else {
	fRunTag = new AliRunTag();
    }
    
    delete fTreeT; fTreeT = 0;
    
    if (fChainT) {
	delete fChainT;
	fChainT = 0;
    }
    
    if (!fChainT) {
	fChainT = new TChain("T");
    }
    


    const char* tagPattern = "ESD.tag.root";
    const char* name = 0x0;
    TString tagFilename;
    if (zip) {
	TFile* file = fTree->GetCurrentFile();
	TArchiveFile* arch = file->GetArchive();
	TObjArray* arr = arch->GetMembers();
	TIter next(arr);
	
	while ((file = (TFile*) next())) {
	    name = file->GetName();
	    if (strstr(name,tagPattern)) { 
		tagFilename = pathName.Data();
		tagFilename += "#";
		tagFilename += name;
		fChainT->Add(tagFilename);  
		AliInfo(Form("Adding %s to tag chain \n", tagFilename.Data()));
	    }//pattern check
	} // archive file loop
    } else {
	void * dirp = gSystem->OpenDirectory(pathName.Data());
	while((name = gSystem->GetDirEntry(dirp))) {
	    if (strstr(name,tagPattern)) { 
		tagFilename = pathName.Data();
		tagFilename += "/";
		tagFilename += name;
		fChainT->Add(tagFilename);  
		AliInfo(Form("Adding %s to tag chain \n", tagFilename.Data()));
	    }//pattern check
	}//directory loop
    }
    fChainT->SetBranchAddress("AliTAG",&fRunTag);
    fChainT->GetEntry(0);
    return kTRUE;
}



Option_t *AliESDInputHandler::GetDataType() const
{
// Returns handled data type.
   return gESDDataType;
}

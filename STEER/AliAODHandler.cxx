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
//     Implementation of the Virtual Event Handler Interface for AOD
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------


#include <TTree.h>
#include <TFile.h>

#include "AliAODHandler.h"
#include "AliAODEvent.h"

ClassImp(AliAODHandler)

//______________________________________________________________________________
AliAODHandler::AliAODHandler() :
    AliVEventHandler(),
    fIsStandard(kTRUE),
    fAODEvent(NULL),
    fTreeA(NULL),
    fFileA(NULL),
    fName("")
{
  // default constructor
}

//______________________________________________________________________________
AliAODHandler::AliAODHandler(const char* name, const char* title):
    AliVEventHandler(name, title),
    fIsStandard(kTRUE),
    fAODEvent(NULL),
    fTreeA(NULL),
    fFileA(NULL),
    fName("")
{
}

//______________________________________________________________________________
AliAODHandler::~AliAODHandler() 
{
  delete fAODEvent;
  if(fFileA){
    // is already handled in TerminateIO
    fFileA->Close();
    delete fFileA;
  }
  delete fTreeA;
  delete fName;
 // destructor
}


Bool_t AliAODHandler::Init(Option_t* opt)
{
  // Initialize IO
  //
  // Create the AODevent object
  if(!fAODEvent){
    fAODEvent = new AliAODEvent();
    if (fIsStandard) fAODEvent->CreateStdContent();
  }
  //
  // File opening according to execution mode
  
  if (!(strcmp(opt, "proof"))) {
    // proof
    CreateTree(0);
  } else {
    // local and grid
    TDirectory *owd = gDirectory;
    fFileA = new TFile(fName, "RECREATE");
    CreateTree(1);
    owd->cd();
  }
  return kTRUE;
}

Bool_t AliAODHandler::FinishEvent()
{
    // Fill data structures
    fAODEvent->MakeEntriesReferencable();
    FillTree();
    if (fIsStandard) fAODEvent->ResetStd();
    return kTRUE;
}

Bool_t AliAODHandler::Terminate()
{
    // Terminate 
    AddAODtoTreeUserInfo();
    return kTRUE;
}

Bool_t AliAODHandler::TerminateIO()
{
    // Terminate IO
    if (fFileA) {
	fFileA->ls();
	fFileA->Close();
	delete fFileA;
    }
    return kTRUE;
}


void AliAODHandler::CreateTree(Int_t flag)
{
    // Creates the AOD Tree
    fTreeA = new TTree("aodTree", "AliAOD tree");
    fTreeA->Branch(fAODEvent->GetList());
    if (flag == 0) fTreeA->SetDirectory(0);
}

void AliAODHandler::FillTree()
{
    // Fill the AOD Tree
    fTreeA->Fill();
}


void AliAODHandler::AddAODtoTreeUserInfo()
{
    // Add aod event to tree user info
    fTreeA->GetUserInfo()->Add(fAODEvent);
}

void AliAODHandler::AddBranch(const char* cname, void* addobj)
{
    // Add a new branch to the aod 
    TDirectory *owd = gDirectory;
    if (fFileA) {
	fFileA->cd();
    }
    char** apointer = (char**) addobj;
    TObject* obj = (TObject*) *apointer;
    fTreeA->Branch(obj->GetName(), cname, addobj);
    fAODEvent->AddObject(obj);
    owd->cd();
}

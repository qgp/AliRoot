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
    AliVirtualEventHandler(),
    fAODEvent(NULL),
    fTreeA(NULL),
    fFileA(NULL),
    fName("")
{
  // default constructor
}

//______________________________________________________________________________
AliAODHandler::AliAODHandler(const char* name, const char* title):
    AliVirtualEventHandler(name, title),
    fAODEvent(NULL),
    fTreeA(NULL),
    fFileA(NULL),
    fName("")
{
}

//______________________________________________________________________________
AliAODHandler::~AliAODHandler() 
{
// destructor
}


Bool_t AliAODHandler::InitIO(Option_t* opt)
{
    // Initialize IO
    //
    // Create the AODevent object
    fAODEvent = new AliAODEvent();
    fAODEvent->CreateStdContent();
    //
    // File opening according to execution mode

    if (!(strcmp(opt, "proof"))) {
	// proof
    } else {
	// local and grid
	fFileA = new TFile(fName, "RECREATE");
    }
    //
    // Create the output tree
    CreateTree();
    
    return kTRUE;
}

Bool_t AliAODHandler::FinishEvent()
{
    // Fill data structures
    FillTree();
    fAODEvent->ClearStd();
    
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

    return kTRUE;
}


void AliAODHandler::CreateTree()
{
    // Creates the AOD Tree
    fTreeA = new TTree("AOD", "AliAOD tree");
    fTreeA->Branch(fAODEvent->GetList());
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

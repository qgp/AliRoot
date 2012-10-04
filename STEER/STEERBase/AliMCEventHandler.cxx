/************************************************************************** 
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved  *
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
//---------------------------------------------------------------------------------
//                          Class AliMCEventHandler
// This class gives access to MC truth during the analysis.
// Monte Carlo truth is containe in the kinematics tree (produced particles) and 
// the tree of reference hits.
//      
// Origin: Andreas Morsch, CERN, andreas.morsch@cern.ch 
//---------------------------------------------------------------------------------



#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"
#include "AliPDG.h"
#include "AliTrackReference.h"
#include "AliHeader.h"
#include "AliStack.h"
#include "AliLog.h"

#include <TTree.h>
#include <TTreeCache.h>
#include <TFile.h>
#include <TList.h>
#include <TParticle.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TDirectoryFile.h>

ClassImp(AliMCEventHandler)

AliMCEventHandler::AliMCEventHandler() :
    AliInputEventHandler(),
    fMCEvent(new AliMCEvent()),
    fFileE(0),
    fFileK(0),
    fFileTR(0),
    fTreeE(0),
    fTreeK(0),
    fTreeTR(0),
    fDirK(0),
    fDirTR(0),
    fParticleSelected(0),
    fLabelMap(0),
    fNEvent(-1),
    fEvent(-1),
    fPathName(new TString("./")),
    fkExtension(""),
    fFileNumber(0),
    fEventsPerFile(0),
    fReadTR(kTRUE),
    fInitOk(kFALSE),
    fSubsidiaryHandlers(0),
    fEventsInContainer(0),
    fPreReadMode(kNoPreRead),
    fCacheSize(0),
    fCacheTK(0),
    fCacheTR(0)
{
  //
  // Default constructor
  //
  // Be sure to add all particles to the PDG database
  AliPDG::AddParticlesToPdgDataBase();
}

AliMCEventHandler::AliMCEventHandler(const char* name, const char* title) :
    AliInputEventHandler(name, title),
    fMCEvent(new AliMCEvent()),
    fFileE(0),
    fFileK(0),
    fFileTR(0),
    fTreeE(0),
    fTreeK(0),
    fTreeTR(0),
    fDirK(0),
    fDirTR(0),
    fParticleSelected(0),
    fLabelMap(0),
    fNEvent(-1),
    fEvent(-1),
    fPathName(new TString("./")),
    fkExtension(""),
    fFileNumber(0),
    fEventsPerFile(0),
    fReadTR(kTRUE),
    fInitOk(kFALSE),
    fSubsidiaryHandlers(0),
    fEventsInContainer(0),
    fPreReadMode(kNoPreRead),
    fCacheSize(0),
    fCacheTK(0),
    fCacheTR(0)
{
  //
  // Constructor
  //
  // Be sure to add all particles to the PDG database
  AliPDG::AddParticlesToPdgDataBase();
}
AliMCEventHandler::~AliMCEventHandler()
{ 
    // Destructor
  delete fPathName;
    delete fMCEvent;
    delete fFileE;
    delete fFileK;
    delete fFileTR;
    delete fCacheTK;
    delete fCacheTR;
}

Bool_t AliMCEventHandler::Init(Option_t* opt)
{ 
    // Initialize input
    //
    if (!(strcmp(opt, "proof")) || !(strcmp(opt, "local"))) return kTRUE;
    //
    fFileE = TFile::Open(Form("%sgalice.root", fPathName->Data()));
    if (!fFileE) {
	AliError(Form("AliMCEventHandler:galice.root not found in directory %s ! \n", fPathName->Data()));
	fInitOk = kFALSE;
	return kFALSE;
    }
    
    //
    // Tree E
    fFileE->GetObject("TE", fTreeE);
    // Connect Tree E to the MCEvent
    fMCEvent->ConnectTreeE(fTreeE);
    fNEvent = fTreeE->GetEntries();
    //
    // Tree K
    fFileK = TFile::Open(Form("%sKinematics%s.root", fPathName->Data(), fkExtension));
    if (!fFileK) {
	AliError(Form("AliMCEventHandler:Kinematics.root not found in directory %s ! \n", fPathName->Data()));
	fInitOk = kFALSE;
	return kTRUE;
    }
    
    fEventsPerFile = fFileK->GetNkeys() - fFileK->GetNProcessIDs();
    //
    // Tree TR
    if (fReadTR) {
	fFileTR = TFile::Open(Form("%sTrackRefs%s.root", fPathName->Data(), fkExtension));
	if (!fFileTR) {
	    AliError(Form("AliMCEventHandler:TrackRefs.root not found in directory %s ! \n", fPathName->Data()));
	    fInitOk = kFALSE;
	    return kTRUE;
	}
    }
    //
    // Reset the event number
    fEvent      = -1;
    fFileNumber =  0;
    AliInfo(Form("Number of events in this directory %5d \n", fNEvent));
    fInitOk = kTRUE;


    if (fSubsidiaryHandlers) {
	TIter next(fSubsidiaryHandlers);
	AliMCEventHandler *handler;
	while((handler = (AliMCEventHandler*)next())) {
	    handler->Init(opt);
	    handler->SetNumberOfEventsInContainer(fNEvent);
	}
    }

    return kTRUE;
}

Bool_t AliMCEventHandler::LoadEvent(Int_t iev)
{
    // Load the event number iev
    //
    // Calculate the file number
  if (!fInitOk) return kFALSE;
    
  Int_t inew  = iev / fEventsPerFile;
  Bool_t firsttree = (fTreeK==0) ? kTRUE : kFALSE;
  Bool_t newtree = firsttree;
  if (inew != fFileNumber) {
    newtree = kTRUE;
    fFileNumber = inew;
    if (!OpenFile(fFileNumber)){
      return kFALSE;
    }
  }
  // Folder name
  char folder[20];
  snprintf(folder, 20, "Event%d", iev);
  // TreeE
  fTreeE->GetEntry(iev);
  // Tree K
  fFileK->GetObject(folder, fDirK);
  if (!fDirK) {
    AliWarning(Form("AliMCEventHandler: Event #%5d - Cannot get kinematics\n", iev));
    return kFALSE;
  }
    
  fDirK ->GetObject("TreeK", fTreeK);
  if (!fTreeK) {
    AliError(Form("AliMCEventHandler: Event #%5d - Cannot get TreeK\n",iev));
    return kFALSE;
  }  
  // Connect TreeK to MCEvent
  fMCEvent->ConnectTreeK(fTreeK);
  //Tree TR 
  if (fFileTR) {
    // Check which format has been read
    fFileTR->GetObject(folder, fDirTR);
    if (!fDirTR) {
      AliError(Form("AliMCEventHandler: Event #%5d - Cannot get track references\n",iev));
      return kFALSE;
    }  
     
    fDirTR->GetObject("TreeTR", fTreeTR);
    //
    if (!fTreeTR) {
      AliError(Form("AliMCEventHandler: Event #%5d - Cannot get TreeTR\n",iev));
      return kFALSE;
    }  
    // Connect TR to MCEvent
    fMCEvent->ConnectTreeTR(fTreeTR);
  }
  // Now setup the caches if not yet done
  if (fCacheSize) {
    if (firsttree) {
      fTreeK->SetCacheSize(fCacheSize);
      fCacheTK = (TTreeCache*) fFileK->GetCacheRead(fTreeK);
      TTreeCache::SetLearnEntries(1);
      fTreeK->AddBranchToCache("*",kTRUE);
      Info("LoadEvent","Read cache enabled %lld bytes for TreeK",fCacheSize);
      if (fTreeTR) {
        fTreeTR->SetCacheSize(fCacheSize);
        fCacheTR = (TTreeCache*) fFileTR->GetCacheRead(fTreeTR);
        TTreeCache::SetLearnEntries(1);
        fTreeTR->AddBranchToCache("*",kTRUE);
        Info("LoadEvent","Read cache enabled %lld bytes for TreeTR",fCacheSize);
      } 
    } else {
      // We need to reuse the previous caches and every new event is a new tree
      if (fCacheTK) {
         fCacheTK->ResetCache();
         if (fFileK) fFileK->SetCacheRead(fCacheTK, fTreeK);
         fCacheTK->UpdateBranches(fTreeK);
      }
      if (fCacheTR) {
         fCacheTR->ResetCache();
         if (fFileTR) fFileTR->SetCacheRead(fCacheTR, fTreeTR);
         fCacheTR->UpdateBranches(fTreeTR);
      }   
    }
  }  
  return kTRUE;
}

Bool_t AliMCEventHandler::OpenFile(Int_t i)
{
    // Open file i
    if (i > 0) {
	fkExtension = Form("%d", i);
    } else {
	fkExtension = "";
    }
    
    if (fFileK && fCacheTK) fFileK->SetCacheRead(0, fTreeK);
    delete fFileK;
    fFileK = TFile::Open(Form("%sKinematics%s.root", fPathName->Data(), fkExtension));
    if (!fFileK) {
	AliError(Form("AliMCEventHandler:Kinematics%s.root not found in directory %s ! \n", fkExtension, fPathName->Data()));
	fInitOk = kFALSE;
	return kFALSE;
    }
    
    if (fReadTR) {
      if (fFileTR && fCacheTR) fFileTR->SetCacheRead(0, fTreeTR);
      delete fFileTR;
      fFileTR = TFile::Open(Form("%sTrackRefs%s.root", fPathName->Data(), fkExtension));
      if (!fFileTR) {
        AliWarning(Form("AliMCEventHandler:TrackRefs%s.root not found in directory %s ! \n", fkExtension, fPathName->Data()));
        fInitOk = kFALSE;
        return kFALSE;
      }
    }
    
    fInitOk = kTRUE;

    return kTRUE;
}

Bool_t AliMCEventHandler::BeginEvent(Long64_t entry)
{ 
    // Begin event
    fParticleSelected.Delete();
    fLabelMap.Delete();
    // Read the next event

    if (fEventsInContainer != 0) {
	entry = (Long64_t) ( entry * Float_t(fNEvent) / Float_t (fEventsInContainer));
    }


    if (entry == -1) {
	fEvent++;
	entry = fEvent;
    } else {
	fEvent = entry;
    }

    if (entry >= fNEvent) {
	AliWarning(Form("AliMCEventHandler: Event number out of range %5lld %5d\n", entry, fNEvent));
	return kFALSE;
    }
    
    Bool_t result = LoadEvent(entry);

    if (fSubsidiaryHandlers) {
	TIter next(fSubsidiaryHandlers);
	AliMCEventHandler *handler;
	while((handler = (AliMCEventHandler*)next())) {
	    handler->BeginEvent(entry);
	}
	next.Reset();
	while((handler = (AliMCEventHandler*)next())) {
	    fMCEvent->AddSubsidiaryEvent(handler->MCEvent());
	}
	fMCEvent->InitEvent();
    }
    
    if (fPreReadMode == kLmPreRead) {
	fMCEvent->PreReadAll();
    }

    return result;
    
}

void AliMCEventHandler::SelectParticle(Int_t i){
  // taking the absolute values here, need to take care 
  // of negative daughter and mother
  // IDs when setting!
    if (TMath::Abs(i) >= AliMCEvent::BgLabelOffset()) i =  fMCEvent->BgLabelToIndex(TMath::Abs(i));
    if(!IsParticleSelected(TMath::Abs(i)))fParticleSelected.Add(TMath::Abs(i),1);
}

Bool_t AliMCEventHandler::IsParticleSelected(Int_t i)  {
  // taking the absolute values here, need to take 
  // care with negative daughter and mother
  // IDs when setting!
  return (fParticleSelected.GetValue(TMath::Abs(i))==1);
}


void AliMCEventHandler::CreateLabelMap(){

  //
  // this should be called once all selections where done 
  //

  fLabelMap.Delete();
  if(!fMCEvent){
    fParticleSelected.Delete();
    return;
  }

  VerifySelectedParticles();

  Int_t iNew = 0;
  for(int i = 0;i < fMCEvent->GetNumberOfTracks();++i){
    if(IsParticleSelected(i)){
      fLabelMap.Add(i,iNew);
      iNew++;
    }
  }
}

Int_t AliMCEventHandler::GetNewLabel(Int_t i) {
  // Gets the label from the new created Map
  // Call CreatLabelMap before
  // otherwise only 0 returned
  return fLabelMap.GetValue(TMath::Abs(i));
}

void  AliMCEventHandler::VerifySelectedParticles(){

  //  
  // Make sure that each particle has at least it's predecessors
  // selected so that we have the complete ancestry tree
  // Private, should be only called by CreateLabelMap

  if(!fMCEvent){
      fParticleSelected.Delete();
      return;
  }

  Int_t nprim = fMCEvent->GetNumberOfPrimaries();

  for(int i = 0;i < fMCEvent->GetNumberOfTracks(); ++i){
      if(i < nprim){
	  SelectParticle(i);// take all primaries
	  continue;
      }

      if(!IsParticleSelected(i))continue;

      AliMCParticle* mcpart = (AliMCParticle*) fMCEvent->GetTrack(i);
      Int_t imo = mcpart->GetMother();
      while((imo >= nprim)&&!IsParticleSelected(imo)){
	  // Mother not yet selected
	  SelectParticle(imo);
	  mcpart = (AliMCParticle*) fMCEvent->GetTrack(imo);
	  imo = mcpart->GetMother();
      }
    // after last step we may have an unselected primary
    // mother
    if(imo>=0){
      if(!IsParticleSelected(imo))
	SelectParticle(imo);
    } 
  }// loop over all tracks
}

Int_t AliMCEventHandler::GetParticleAndTR(Int_t i, TParticle*& particle, TClonesArray*& trefs)
{
    // Retrieve entry i
    if (!fInitOk) {
	return 0;
    } else {
	return (fMCEvent->GetParticleAndTR(i, particle, trefs));
    }
}

void AliMCEventHandler::DrawCheck(Int_t i, Int_t search)
{
    // Retrieve entry i and draw momentum vector and hits
    fMCEvent->DrawCheck(i, search);
}

Bool_t AliMCEventHandler::Notify(const char *path)
{
  // Notify about directory change
  // The directory is taken from the 'path' argument
  // Reconnect trees
    TString fileName(path);
    if(fileName.Contains("AliESDs.root")){
	fileName.ReplaceAll("AliESDs.root", "");
    }
    else if(fileName.Contains("AliESDs_wSDD.root")){
	fileName.ReplaceAll("AliESDs_wSDD.root", "");
    }
    else if(fileName.Contains("AliESDs_nob.root")){
	fileName.ReplaceAll("AliESDs_nob.root", "");
    }
    else if(fileName.Contains("AliAOD.root")){
	fileName.ReplaceAll("AliAOD.root", "");
    }
    else if(fileName.Contains("galice.root")){
	// for running with galice and kinematics alone...
	fileName.ReplaceAll("galice.root", "");
    }
    else if (fileName.BeginsWith("root:")) {
      fileName.Append("?ZIP=");
    }

    *fPathName = fileName;
    AliInfo(Form("Path: -%s-\n", fPathName->Data()));
    
    ResetIO();
    InitIO("");

// Handle subsidiary handlers
    if (fSubsidiaryHandlers) {
	TIter next(fSubsidiaryHandlers);
	AliMCEventHandler *handler;
	while((handler = (AliMCEventHandler*) next())) {
	    TString* spath = handler->GetInputPath();
	    if (spath->Contains("merged")) {
		if (! fPathName->IsNull()) {
		    handler->Notify(Form("%s/../.", fPathName->Data()));
		} else {
		    handler->Notify("../");
		}
	    }
	}
    }
    
    return kTRUE;
}

void AliMCEventHandler::ResetIO()
{
//  Clear header and stack
    
    if (fInitOk) fMCEvent->Clean();
    
// Delete Tree E
    delete fTreeE; fTreeE = 0;
     
// Reset files
    if (fFileE)  {delete fFileE;  fFileE  = 0;}
    if (fFileK)  {delete fFileK;  fFileK  = 0;}
    if (fFileTR) {delete fFileTR; fFileTR = 0;}
    fkExtension="";
    fInitOk = kFALSE;

    if (fSubsidiaryHandlers) {
	TIter next(fSubsidiaryHandlers);
	AliMCEventHandler *handler;
	while((handler = (AliMCEventHandler*)next())) {
	    handler->ResetIO();
	}
    }

}

			    
Bool_t AliMCEventHandler::FinishEvent()
{
    // Clean-up after each event
   if (fFileK && fCacheTK) fFileK->SetCacheRead(0, fTreeK);
   if (fFileTR && fCacheTR) fFileTR->SetCacheRead(0, fTreeTR);
   delete fDirTR;  fDirTR = 0;
   delete fDirK;   fDirK  = 0;    
    if (fInitOk) fMCEvent->FinishEvent();

    if (fSubsidiaryHandlers) {
	TIter next(fSubsidiaryHandlers);
	AliMCEventHandler *handler;
	while((handler = (AliMCEventHandler*)next())) {
	    handler->FinishEvent();
	}
    }

    return kTRUE;
}

Bool_t AliMCEventHandler::Terminate()
{ 
    // Dummy 
    return kTRUE;
}

Bool_t AliMCEventHandler::TerminateIO()
{ 
    // Dummy
    return kTRUE;
}
    

void AliMCEventHandler::SetInputPath(const char* fname)
{
    // Set the input path name
    delete fPathName;
    fPathName = new TString(fname);
}

void AliMCEventHandler::AddSubsidiaryHandler(AliMCEventHandler* handler)
{
    // Add a subsidiary handler. For example for background events

    if (!fSubsidiaryHandlers) fSubsidiaryHandlers = new TList();
    fSubsidiaryHandlers->Add(handler);
}

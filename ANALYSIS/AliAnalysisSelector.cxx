/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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
// Author: Andrei Gheata, 31/05/2006

//==============================================================================
//   AliAnalysisSelector - A transparent selector to be created by 
// AliAnalysisManager to handle analysis.
//==============================================================================

#include <Riostream.h>
#include <TProcessID.h>

#include "AliAnalysisManager.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisDataContainer.h"
#include "AliAnalysisSelector.h"

ClassImp(AliAnalysisSelector)

//______________________________________________________________________________
AliAnalysisSelector::AliAnalysisSelector(AliAnalysisManager *mgr)
                    :TSelector(),
                     fInitialized(kFALSE),
                     fAnalysis(mgr)
{
// Constructor. Called by AliAnalysisManager which registers itself on the
// selector running on the master.
}

//______________________________________________________________________________
AliAnalysisSelector::~AliAnalysisSelector()
{
// Dtor. The analysis manager object is sent in the input list and duplicated
// on the workers - it needs to be deleted (?)
//   if (fAnalysis) delete fAnalysis;
}

//______________________________________________________________________________
void AliAnalysisSelector::Init(TTree *tree)
{
// Called after Begin/SlaveBegin, assumes that fAnalysis is already initialized.
// Is Init called on workers in case of PROOF.
   if (!fAnalysis) {
      Error("Init", "Analysis manager NULL !");
      return;
   }
   if (fAnalysis->GetDebugLevel()>0) {
      cout << "->AliAnalysisSelector->Init()" << endl;
   }   
   if (!tree) {
      Error("Init", "Input tree is NULL !");
      return;
   }
   fAnalysis->Init(tree);
   fInitialized = kTRUE;
   if (fAnalysis->GetDebugLevel()>0) {
      cout << "<-AliAnalysisSelector->Init()" << endl;
   }   
}

//______________________________________________________________________________
void AliAnalysisSelector::Begin(TTree *)
{
// Assembly the input list.
   RestoreAnalysisManager();
   if (fAnalysis && fAnalysis->GetDebugLevel()>0) {
      cout << "->AliAnalysisSelector->Begin: Analysis manager restored" << endl;
   }
}

//______________________________________________________________________________
void AliAnalysisSelector::SlaveBegin(TTree *tree)
{
// Called on each worker. We "unpack" analysis manager here and call InitAnalysis.
   RestoreAnalysisManager();
   if (fAnalysis) {
      if (fAnalysis->GetDebugLevel()>0) {
         cout << "->AliAnalysisSelector->SlaveBegin() after Restore" << endl;
      }   
      fAnalysis->SlaveBegin(tree);   
      if (fAnalysis->GetDebugLevel()>0) {
         cout << "<-AliAnalysisSelector->SlaveBegin()" << endl;
      }   
   }   
}

//______________________________________________________________________________
Bool_t AliAnalysisSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
   if (fAnalysis) return fAnalysis->Notify();
   return kFALSE;
}

//______________________________________________________________________________
Bool_t AliAnalysisSelector::Process(Long64_t entry)
{
// Event loop.
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "->AliAnalysisSelector::Process()" << endl;
   }
   Int_t nobjCount = TProcessID::GetObjectCount();
   fAnalysis->GetEntry(entry);
   fAnalysis->ExecAnalysis();
   TProcessID::SetObjectCount(nobjCount);
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "<-AliAnalysisSelector::Process()" << endl;
   }   
   return kTRUE;
}   

//______________________________________________________________________________
void AliAnalysisSelector::RestoreAnalysisManager()
{
// Restores analysis manager from the input list.
   if (!fAnalysis) {
      TIter next(fInput);
      TObject *obj;
      while ((obj=next())) {
         if (obj->IsA() == AliAnalysisManager::Class()) {
            fAnalysis = (AliAnalysisManager*)obj;
            fAnalysis->SetSelector(this);
            if (fAnalysis->GetDebugLevel()>0) {
               cout << "->AliAnalysisSelector->RestoreAnalysisManager: Analysis manager restored" << endl;
            }   
            break;
         }
      }
      if (!fAnalysis) {
         Error("SlaveBegin", "Analysis manager not found in the input list");
         return;
      }   
   }
}

//______________________________________________________________________________
void AliAnalysisSelector::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "->AliAnalysisSelector::SlaveTerminate()" << endl;
   }   
   fAnalysis->PackOutput(fOutput);
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "<-AliAnalysisSelector::SlaveTerminate()" << endl;
   }   
}  

//______________________________________________________________________________
void AliAnalysisSelector::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
   if (!fAnalysis) {
      Error("Terminate","AliAnalysisSelector::Terminate: No analysis manager!!!");
      return;
   }   
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "->AliAnalysisSelector::Terminate()" << endl;
   }   
   fAnalysis->UnpackOutput(fOutput);
   fAnalysis->Terminate();   
   if (fAnalysis->GetDebugLevel() > 0) {
      cout << "<-AliAnalysisSelector::Terminate()" << endl;
   }   
}

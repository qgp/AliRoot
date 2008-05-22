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
//   AliAnalysysManager - Manager analysis class. Allows creation of several
// analysis tasks and data containers storing their input/output. Allows
// connecting/chaining tasks via shared data containers. Serializes the current
// event for all tasks depending only on initial input data.
//==============================================================================
//
//==============================================================================

#include <Riostream.h>

#include <TClass.h>
#include <TFile.h>
#include <TMethodCall.h>
#include <TChain.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>

#include "AliAnalysisSelector.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisDataContainer.h"
#include "AliAnalysisDataSlot.h"
#include "AliVEventHandler.h"
#include "AliVEventPool.h"
#include "AliSysInfo.h"
#include "AliAnalysisManager.h"

ClassImp(AliAnalysisManager)

AliAnalysisManager *AliAnalysisManager::fgAnalysisManager = NULL;

//______________________________________________________________________________
AliAnalysisManager::AliAnalysisManager(const char *name, const char *title)
                   :TNamed(name,title),
                    fTree(NULL),
                    fInputEventHandler(NULL),
                    fOutputEventHandler(NULL),
                    fMCtruthEventHandler(NULL),
		    fEventPool(NULL),
                    fCurrentEntry(-1),
                    fNSysInfo(0),
                    fMode(kLocalAnalysis),
                    fInitOK(kFALSE),
                    fDebug(0),
                    fSpecialOutputLocation(""), 
                    fTasks(NULL),
                    fTopTasks(NULL),
                    fZombies(NULL),
                    fContainers(NULL),
                    fInputs(NULL),
                    fOutputs(NULL),
                    fSelector(NULL)
{
// Default constructor.
   fgAnalysisManager = this;
   fTasks      = new TObjArray();
   fTopTasks   = new TObjArray();
   fZombies    = new TObjArray();
   fContainers = new TObjArray();
   fInputs     = new TObjArray();
   fOutputs    = new TObjArray();
   SetEventLoop(kTRUE);
}

//______________________________________________________________________________
AliAnalysisManager::AliAnalysisManager(const AliAnalysisManager& other)
                   :TNamed(other),
                    fTree(NULL),
                    fInputEventHandler(NULL),
                    fOutputEventHandler(NULL),
                    fMCtruthEventHandler(NULL),
		    fEventPool(NULL),
                    fCurrentEntry(-1),
                    fNSysInfo(0),
                    fMode(other.fMode),
                    fInitOK(other.fInitOK),
                    fDebug(other.fDebug),
                    fSpecialOutputLocation(""), 
                    fTasks(NULL),
                    fTopTasks(NULL),
                    fZombies(NULL),
                    fContainers(NULL),
                    fInputs(NULL),
                    fOutputs(NULL),
                    fSelector(NULL)
{
// Copy constructor.
   fTasks      = new TObjArray(*other.fTasks);
   fTopTasks   = new TObjArray(*other.fTopTasks);
   fZombies    = new TObjArray(*other.fZombies);
   fContainers = new TObjArray(*other.fContainers);
   fInputs     = new TObjArray(*other.fInputs);
   fOutputs    = new TObjArray(*other.fOutputs);
   fgAnalysisManager = this;
}
   
//______________________________________________________________________________
AliAnalysisManager& AliAnalysisManager::operator=(const AliAnalysisManager& other)
{
// Assignment
   if (&other != this) {
      TNamed::operator=(other);
      fInputEventHandler   = other.fInputEventHandler;
      fOutputEventHandler  = other.fOutputEventHandler;
      fMCtruthEventHandler = other.fMCtruthEventHandler;
      fEventPool           = other.fEventPool;
      fTree       = NULL;
      fCurrentEntry = -1;
      fNSysInfo   = other.fNSysInfo;
      fMode       = other.fMode;
      fInitOK     = other.fInitOK;
      fDebug      = other.fDebug;
      fTasks      = new TObjArray(*other.fTasks);
      fTopTasks   = new TObjArray(*other.fTopTasks);
      fZombies    = new TObjArray(*other.fZombies);
      fContainers = new TObjArray(*other.fContainers);
      fInputs     = new TObjArray(*other.fInputs);
      fOutputs    = new TObjArray(*other.fOutputs);
      fSelector   = NULL;
      fgAnalysisManager = this;
   }
   return *this;
}

//______________________________________________________________________________
AliAnalysisManager::~AliAnalysisManager()
{
// Destructor.
   if (fTasks) {fTasks->Delete(); delete fTasks;}
   if (fTopTasks) delete fTopTasks;
   if (fZombies) delete fZombies;
   if (fContainers) {fContainers->Delete(); delete fContainers;}
   if (fInputs) delete fInputs;
   if (fOutputs) delete fOutputs;
   if (fgAnalysisManager==this) fgAnalysisManager = NULL;
}

//______________________________________________________________________________
Int_t AliAnalysisManager::GetEntry(Long64_t entry, Int_t getall)
{
// Read one entry of the tree or a whole branch.
   if (fDebug > 0) printf("== AliAnalysisManager::GetEntry(%lld)\n", entry);
   fCurrentEntry = entry;
   return fTree ? fTree->GetTree()->GetEntry(entry, getall) : 0;
}
   
//______________________________________________________________________________
void AliAnalysisManager::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses of the tree
  // will be set. It is normaly not necessary to make changes to the
  // generated code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running with PROOF.
   if (!tree) return;
   if (fDebug > 0) {
      printf("->AliAnalysisManager::Init(%s)\n", tree->GetName());
   }

   // Call InitTree of EventHandler
   if (fOutputEventHandler) {
      if (fMode == kProofAnalysis) {
         fOutputEventHandler->Init(0x0, "proof");
      } else {
         fOutputEventHandler->Init(0x0, "local");
      }
   }

   if (fInputEventHandler) {
      if (fMode == kProofAnalysis) {
         fInputEventHandler->Init(tree, "proof");
      } else {
         fInputEventHandler->Init(tree, "local");
      }
   } else {
      // If no input event handler we need to get the tree once
      // for the chain
      if(!tree->GetTree()) tree->LoadTree(0);
   }
   

   if (fMCtruthEventHandler) {
      if (fMode == kProofAnalysis) {
         fMCtruthEventHandler->Init(0x0, "proof");
      } else {
         fMCtruthEventHandler->Init(0x0, "local");
      }
   }

   if (!fInitOK) InitAnalysis();
   if (!fInitOK) return;
   fTree = tree;
   AliAnalysisDataContainer *top = (AliAnalysisDataContainer*)fInputs->At(0);
   if (!top) {
      Error("Init","No top input container !");
      return;
   }
   top->SetData(tree);
   if (fDebug > 0) {
      printf("<-AliAnalysisManager::Init(%s)\n", tree->GetName());
   }
}

//______________________________________________________________________________
void AliAnalysisManager::SlaveBegin(TTree *tree)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).
   if (fDebug > 0) printf("->AliAnalysisManager::SlaveBegin()\n");

   // Call Init of EventHandler
   if (fOutputEventHandler) {
      if (fMode == kProofAnalysis) {
         fOutputEventHandler->Init("proof");
      } else {
         fOutputEventHandler->Init("local");
      }
   }

   if (fInputEventHandler) {
      fInputEventHandler->SetInputTree(tree);
      if (fMode == kProofAnalysis) {
         fInputEventHandler->Init("proof");
      } else {
         fInputEventHandler->Init("local");
      }
   }

   if (fMCtruthEventHandler) {
      if (fMode == kProofAnalysis) {
         fMCtruthEventHandler->Init("proof");
      } else {
         fMCtruthEventHandler->Init("local");
      }
   }

   TIter next(fTasks);
   AliAnalysisTask *task;
   // Call CreateOutputObjects for all tasks
   while ((task=(AliAnalysisTask*)next())) {
      TDirectory *curdir = gDirectory;
      task->CreateOutputObjects();
      if (curdir) curdir->cd();
   }

   if (fDebug > 0) printf("<-AliAnalysisManager::SlaveBegin()\n");
}

//______________________________________________________________________________
Bool_t AliAnalysisManager::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
   if (!fTree) {
      Error("Notify","No current tree.");
      return kFALSE;
   }   
   TFile *curfile = fTree->GetCurrentFile();
   if (!curfile) {
      Error("Notify","No current file");
      return kFALSE;
   }   
   
   if (fDebug > 0) printf("->AliAnalysisManager::Notify() file: %s\n", curfile->GetName());
   TIter next(fTasks);
   AliAnalysisTask *task;
   // Call Notify for all tasks
   while ((task=(AliAnalysisTask*)next())) 
      task->Notify();
	
   // Call Notify of the event handlers
   if (fInputEventHandler) {
       fInputEventHandler->Notify(curfile->GetName());
   }

   if (fOutputEventHandler) {
       fOutputEventHandler->Notify(curfile->GetName());
   }

   if (fMCtruthEventHandler) {
       fMCtruthEventHandler->Notify(curfile->GetName());
   }
   if (fDebug > 0) printf("<-AliAnalysisManager::Notify()\n");
   return kTRUE;
}    

//______________________________________________________________________________
Bool_t AliAnalysisManager::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either TTree::GetEntry() or TBranch::GetEntry()
  // to read either all or the required parts of the data. When processing
  // keyed objects with PROOF, the object is already loaded and is available
  // via the fObject pointer.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.

  // WARNING when a selector is used with a TChain, you must use
  //  the pointer to the current TTree to call GetEntry(entry).
  //  The entry is always the local entry number in the current tree.
  //  Assuming that fChain is the pointer to the TChain being processed,
  //  use fChain->GetTree()->GetEntry(entry).
   if (fDebug > 0) printf("->AliAnalysisManager::Process(%lld)\n", entry);

   if (fInputEventHandler)   fInputEventHandler  ->BeginEvent(entry);
   if (fOutputEventHandler)  fOutputEventHandler ->BeginEvent(entry);
   if (fMCtruthEventHandler) fMCtruthEventHandler->BeginEvent(entry);
   
   GetEntry(entry);
   ExecAnalysis();
   if (fDebug > 0) printf("<-AliAnalysisManager::Process()\n");
   return kTRUE;
}

//______________________________________________________________________________
void AliAnalysisManager::PackOutput(TList *target)
{
  // Pack all output data containers in the output list. Called at SlaveTerminate
  // stage in PROOF case for each slave.
   if (fDebug > 0) printf("->AliAnalysisManager::PackOutput()\n");
   if (!target) {
      Error("PackOutput", "No target. Aborting.");
      return;
   }
   if (fInputEventHandler)   fInputEventHandler  ->Terminate();
   if (fOutputEventHandler)  fOutputEventHandler ->Terminate();
   if (fMCtruthEventHandler) fMCtruthEventHandler->Terminate();

   // Call FinishTaskOutput() for each event loop task (not called for 
   // post-event loop tasks - use Terminate() fo those)
   TIter nexttask(fTasks);
   AliAnalysisTask *task;
   while ((task=(AliAnalysisTask*)nexttask())) {
      if (!task->IsPostEventLoop()) {
         if (fDebug > 0) printf("->FinishTaskOutput: task %s\n", task->GetName());
         task->FinishTaskOutput();
         if (fDebug > 0) printf("<-FinishTaskOutput: task %s\n", task->GetName());
      }
   }      
   
   if (fMode == kProofAnalysis) {
      TIter next(fOutputs);
      AliAnalysisDataContainer *output;
      while ((output=(AliAnalysisDataContainer*)next())) {
         // Do not consider outputs of post event loop tasks
         if (output->GetProducer()->IsPostEventLoop()) continue;
         // Check if data was posted to this container. If not, issue an error.
         if (!output->GetData() ) {
            Error("PackOutput", "No data for output container %s. Forgot to PostData ?\n", output->GetName());
            continue;
         }   
         if (!output->IsSpecialOutput()) {
            // Normal outputs
            const char *filename = output->GetFileName();
            if (!(strcmp(filename, "default"))) {
               if (fOutputEventHandler) filename = fOutputEventHandler->GetOutputFileName();
            }      
            if (strlen(filename)) {
            // File resident outputs
               TFile *file = output->GetFile();
               // Backup current folder
               TDirectory *opwd = gDirectory;
               // Create file if not existing and register to container.
               if (file) file->cd();
               else      file = new TFile(filename, "RECREATE"); 
               if (file->IsZombie()) {
                  Fatal("PackOutput", "Could not recreate file %s\n", filename);
                  return;
               }   
               output->SetFile(file);
               // Clear file list to release object ownership to user.
               file->Clear();
               // Save data to file, then close.
               if (output->GetData()->InheritsFrom(TCollection::Class())) {
                  // If data is a collection, we set the name of the collection 
                  // as the one of the container and we save as a single key.
                  TCollection *coll = (TCollection*)output->GetData();
                  coll->SetName(output->GetName());
                  coll->Write(output->GetName(), TObject::kSingleKey);
               } else {
                  output->GetData()->Write();
               }      
               if (fDebug > 1) printf("PackOutput %s: memory merge, file resident output\n", output->GetName());
               if (fDebug > 2) {
                  printf("   file %s listing content:\n", filename);
                  file->ls();
               }   
               file->Close();
               // Restore current directory
               if (opwd) opwd->cd();
            } else {
               // Memory-resident outputs   
               if (fDebug > 1) printf("PackOutput %s: memory merge memory resident output\n", output->GetName());
            }   
            AliAnalysisDataWrapper *wrap = output->ExportData();
            // Output wrappers must delete data after merging (AG 13/11/07)
            wrap->SetDeleteData(kTRUE);
            target->Add(wrap);
         }   
         // Special outputs
         if (output->IsSpecialOutput()) {
            TDirectory *opwd = gDirectory;
            TFile *file = output->GetFile();
            if (!file) {
               AliAnalysisTask *producer = output->GetProducer();
               Error("PackOutput", 
                     "File %s for special container %s was NOT opened in %s::CreateOutputObjects !!!",
                     output->GetFileName(), output->GetName(), producer->ClassName());
               continue;
            }   
            file->cd();
            // Release object ownership to users after writing data to file
            if (output->GetData()->InheritsFrom(TCollection::Class())) {
               // If data is a collection, we set the name of the collection 
               // as the one of the container and we save as a single key.
               TCollection *coll = (TCollection*)output->GetData();
               coll->SetName(output->GetName());
               coll->Write(output->GetName(), TObject::kSingleKey);
            } else {
               output->GetData()->Write();
            }      
            file->Clear();
            if (fDebug > 2) {
               printf("   file %s listing content:\n", output->GetFileName());
               file->ls();
            }   
            file->Close();
            // Restore current directory
            if (opwd) opwd->cd();
            // Check if a special output location was provided or the output files have to be merged
            if (strlen(fSpecialOutputLocation.Data())) {
               TString remote = fSpecialOutputLocation;
               remote += "/";
               Int_t gid = gROOT->ProcessLine("gProofServ->GetGroupId();");
               remote += Form("%s_%d_", gSystem->HostName(), gid);
               remote += output->GetFileName();
               TFile::Cp(output->GetFileName(), remote.Data());
            } else {
            // No special location specified-> use TProofOutputFile as merging utility
            // The file at this output slot must be opened in CreateOutputObjects
               if (fDebug > 1) printf("   File %s to be merged...\n", output->GetFileName());
            }
         }      
      }
   } 
   if (fDebug > 0) printf("<-AliAnalysisManager::PackOutput: output list contains %d containers\n", target->GetSize());
}

//______________________________________________________________________________
void AliAnalysisManager::ImportWrappers(TList *source)
{
// Import data in output containers from wrappers coming in source.
   if (fDebug > 0) printf("->AliAnalysisManager::ImportWrappers()\n");
   TIter next(fOutputs);
   AliAnalysisDataContainer *cont;
   AliAnalysisDataWrapper   *wrap;
   Int_t icont = 0;
   while ((cont=(AliAnalysisDataContainer*)next())) {
      if (cont->GetProducer()->IsPostEventLoop()) continue;
      if (cont->IsSpecialOutput()) {
         if (strlen(fSpecialOutputLocation.Data())) continue;
         // Copy merged file from PROOF scratch space
         if (fDebug > 1) 
            printf("   Copying file %s from PROOF scratch space\n", cont->GetFileName());
         Bool_t gotit = TFile::Cp(Form("root://lxb6045.cern.ch:11094//pool/scratch/%s",cont->GetFileName()),
                   cont->GetFileName()); 
         if (!gotit) {
            Error("ImportWrappers", "Could not get file %s from proof scratch space", cont->GetFileName());
         }
         // Normally we should connect data from the copied file to the
         // corresponding output container, but it is not obvious how to do this
         // automatically if several objects in file...
         continue;
      }   
      wrap = (AliAnalysisDataWrapper*)source->FindObject(cont->GetName());
      if (!wrap) {
         Error("ImportWrappers","Container %s not found in analysis output !", cont->GetName());
         continue;
      }
      icont++;
      if (fDebug > 1) {
         printf("   Importing data for container %s", cont->GetName());
         if (strlen(cont->GetFileName())) printf("    -> file %s\n", cont->GetFileName());
         else printf("\n");
      }   
      cont->ImportData(wrap);
   }         
   if (fDebug > 0) printf("<-AliAnalysisManager::ImportWrappers(): %d containers imported\n", icont);
}

//______________________________________________________________________________
void AliAnalysisManager::UnpackOutput(TList *source)
{
  // Called by AliAnalysisSelector::Terminate only on the client.
   if (fDebug > 0) printf("->AliAnalysisManager::UnpackOutput()\n");
   if (!source) {
      Error("UnpackOutput", "No target. Aborting.");
      return;
   }
   if (fDebug > 1) printf("   Source list contains %d containers\n", source->GetSize());

   if (fMode == kProofAnalysis) ImportWrappers(source);

   TIter next(fOutputs);
   AliAnalysisDataContainer *output;
   while ((output=(AliAnalysisDataContainer*)next())) {
      if (!output->GetData()) continue;
      // Check if there are client tasks that run post event loop
      if (output->HasConsumers()) {
         // Disable event loop semaphore
         output->SetPostEventLoop(kTRUE);
         TObjArray *list = output->GetConsumers();
         Int_t ncons = list->GetEntriesFast();
         for (Int_t i=0; i<ncons; i++) {
            AliAnalysisTask *task = (AliAnalysisTask*)list->At(i);
            task->CheckNotify(kTRUE);
            // If task is active, execute it
            if (task->IsPostEventLoop() && task->IsActive()) {
               if (fDebug > 0) printf("== Executing post event loop task %s\n", task->GetName());
               task->ExecuteTask();
            }   
         }
      }   
   }
   if (fDebug > 0) printf("<-AliAnalysisManager::UnpackOutput()\n");
}

//______________________________________________________________________________
void AliAnalysisManager::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically.
   if (fDebug > 0) printf("->AliAnalysisManager::Terminate()\n");
   AliAnalysisTask *task;
   TIter next(fTasks);
   // Call Terminate() for tasks
   while ((task=(AliAnalysisTask*)next())) task->Terminate();
   //
   TIter next1(fOutputs);
   AliAnalysisDataContainer *output;
   while ((output=(AliAnalysisDataContainer*)next1())) {
      // Close all files at output
      // Special outputs have the files already closed and written.
      if (output->IsSpecialOutput()) continue;
      const char *filename = output->GetFileName();
      if (!(strcmp(filename, "default"))) {
         if (fOutputEventHandler) filename = fOutputEventHandler->GetOutputFileName();
         TFile *aodfile = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
         if (aodfile) {
            if (fDebug > 1) printf("Writing output handler file: %s\n", filename);
            aodfile->Write();
            continue;
         }   
      }      
      if (!strlen(filename)) continue;
      if (!output->GetData()) continue;
      TFile *file = output->GetFile();
      TDirectory *opwd = gDirectory;
      if (file) {
         file->cd();
      } else {
         file = new TFile(filename, "RECREATE");
         if (file->IsZombie()) continue;
         output->SetFile(file);
      }   
      if (fDebug > 1) printf("   writing output data %s to file %s\n", output->GetData()->GetName(), file->GetName());
      if (output->GetData()->InheritsFrom(TCollection::Class())) {
      // If data is a collection, we set the name of the collection 
      // as the one of the container and we save as a single key.
         TCollection *coll = (TCollection*)output->GetData();
         coll->SetName(output->GetName());
         coll->Write(output->GetName(), TObject::kSingleKey);
      } else {
         output->GetData()->Write();
      }      
      file->Close();
      if (opwd) opwd->cd();
   }   

   if (fInputEventHandler)   fInputEventHandler  ->TerminateIO();
   if (fOutputEventHandler)  fOutputEventHandler ->TerminateIO();
   if (fMCtruthEventHandler) fMCtruthEventHandler->TerminateIO();

   Bool_t getsysInfo = ((fNSysInfo>0) && (fMode==kLocalAnalysis))?kTRUE:kFALSE;
   if (getsysInfo) {
      TDirectory *cdir = gDirectory;
      TFile f("syswatch.root", "RECREATE");
      if (!f.IsZombie()) {
         TTree *tree = AliSysInfo::MakeTree("syswatch.log");
         tree->SetMarkerStyle(kCircle);
         tree->SetMarkerColor(kBlue);
         tree->SetMarkerSize(0.5);
         if (!gROOT->IsBatch()) {
            tree->SetAlias("event", "id0");
            tree->SetAlias("memUSED", "pI.fMemVirtual");
            tree->SetAlias("userCPU", "pI.fCpuUser");
            TCanvas *c = new TCanvas("SysInfo","SysInfo",10,10,800,600);
            c->Divide(2,1,0.01,0.01);
            c->cd(1);
            tree->Draw("memUSED:event","","", 1234567890, 0);
            c->cd(2);
            tree->Draw("userCPU:event","","", 1234567890, 0);
         }   
         tree->Write();
         f.Close();
         delete tree;
      }
      if (cdir) cdir->cd();
   }      
   if (fDebug > 0) printf("<-AliAnalysisManager::Terminate()\n");
}

//______________________________________________________________________________
void AliAnalysisManager::AddTask(AliAnalysisTask *task)
{
// Adds a user task to the global list of tasks.
   if (fTasks->FindObject(task)) {
      Warning("AddTask", "Task %s: the same object already added to the analysis manager. Not adding.", task->GetName());
      return;
   }   
   task->SetActive(kFALSE);
   fTasks->Add(task);
}  

//______________________________________________________________________________
AliAnalysisTask *AliAnalysisManager::GetTask(const char *name) const
{
// Retreive task by name.
   if (!fTasks) return NULL;
   return (AliAnalysisTask*)fTasks->FindObject(name);
}

//______________________________________________________________________________
AliAnalysisDataContainer *AliAnalysisManager::CreateContainer(const char *name, 
                                TClass *datatype, EAliAnalysisContType type, const char *filename)
{
// Create a data container of a certain type. Types can be:
//   kExchangeContainer  = 0, used to exchange date between tasks
//   kInputContainer   = 1, used to store input data
//   kOutputContainer  = 2, used for posting results
   if (fContainers->FindObject(name)) {
      Error("CreateContainer","A container named %s already defined !\n",name);
      return NULL;
   }   
   AliAnalysisDataContainer *cont = new AliAnalysisDataContainer(name, datatype);
   fContainers->Add(cont);
   switch (type) {
      case kInputContainer:
         fInputs->Add(cont);
         break;
      case kOutputContainer:
         fOutputs->Add(cont);
         if (filename && strlen(filename)) {
            cont->SetFileName(filename);
            cont->SetDataOwned(kFALSE);  // data owned by the file
         }   
         break;
      case kExchangeContainer:
         break;   
   }
   return cont;
}
         
//______________________________________________________________________________
Bool_t AliAnalysisManager::ConnectInput(AliAnalysisTask *task, Int_t islot,
                                        AliAnalysisDataContainer *cont)
{
// Connect input of an existing task to a data container.
   if (!fTasks->FindObject(task)) {
      AddTask(task);
      Info("ConnectInput", "Task %s was not registered. Now owned by analysis manager", task->GetName());
   } 
   Bool_t connected = task->ConnectInput(islot, cont);
   return connected;
}   

//______________________________________________________________________________
Bool_t AliAnalysisManager::ConnectOutput(AliAnalysisTask *task, Int_t islot,
                                        AliAnalysisDataContainer *cont)
{
// Connect output of an existing task to a data container.
   if (!fTasks->FindObject(task)) {
      AddTask(task);
      Warning("ConnectOutput", "Task %s not registered. Now owned by analysis manager", task->GetName());
   } 
   Bool_t connected = task->ConnectOutput(islot, cont);
   return connected;
}   
                               
//______________________________________________________________________________
void AliAnalysisManager::CleanContainers()
{
// Clean data from all containers that have already finished all client tasks.
   TIter next(fContainers);
   AliAnalysisDataContainer *cont;
   while ((cont=(AliAnalysisDataContainer *)next())) {
      if (cont->IsOwnedData() && 
          cont->IsDataReady() && 
          cont->ClientsExecuted()) cont->DeleteData();
   }
}

//______________________________________________________________________________
Bool_t AliAnalysisManager::InitAnalysis()
{
// Initialization of analysis chain of tasks. Should be called after all tasks
// and data containers are properly connected
   // Check for input/output containers
   fInitOK = kFALSE;
   // Check for top tasks (depending only on input data containers)
   if (!fTasks->First()) {
      Error("InitAnalysis", "Analysis has no tasks !");
      return kFALSE;
   }   
   TIter next(fTasks);
   AliAnalysisTask *task;
   AliAnalysisDataContainer *cont;
   Int_t ntop = 0;
   Int_t nzombies = 0;
   Bool_t iszombie = kFALSE;
   Bool_t istop = kTRUE;
   Int_t i;
   while ((task=(AliAnalysisTask*)next())) {
      istop = kTRUE;
      iszombie = kFALSE;
      Int_t ninputs = task->GetNinputs();
      for (i=0; i<ninputs; i++) {
         cont = task->GetInputSlot(i)->GetContainer();
         if (!cont) {
            if (!iszombie) {
               task->SetZombie();
               fZombies->Add(task);
               nzombies++;
               iszombie = kTRUE;
            }   
            Error("InitAnalysis", "Input slot %d of task %s has no container connected ! Declared zombie...", 
                  i, task->GetName()); 
         }
         if (iszombie) continue;
         // Check if cont is an input container
         if (istop && !fInputs->FindObject(cont)) istop=kFALSE;
         // Connect to parent task
      }
      if (istop) {
         ntop++;
         fTopTasks->Add(task);
      }
   }
   if (!ntop) {
      Error("InitAnalysis", "No top task defined. At least one task should be connected only to input containers");
      return kFALSE;
   }                        
   // Check now if there are orphan tasks
   for (i=0; i<ntop; i++) {
      task = (AliAnalysisTask*)fTopTasks->At(i);
      task->SetUsed();
   }
   Int_t norphans = 0;
   next.Reset();
   while ((task=(AliAnalysisTask*)next())) {
      if (!task->IsUsed()) {
         norphans++;
         Warning("InitAnalysis", "Task %s is orphan", task->GetName());
      }   
   }          
   // Check the task hierarchy (no parent task should depend on data provided
   // by a daughter task)
   for (i=0; i<ntop; i++) {
      task = (AliAnalysisTask*)fTopTasks->At(i);
      if (task->CheckCircularDeps()) {
         Error("InitAnalysis", "Found illegal circular dependencies between following tasks:");
         PrintStatus("dep");
         return kFALSE;
      }   
   }
   // Check that all containers feeding post-event loop tasks are in the outputs list
   TIter nextcont(fContainers); // loop over all containers
   while ((cont=(AliAnalysisDataContainer*)nextcont())) {
      if (!cont->IsPostEventLoop() && !fOutputs->FindObject(cont)) {
         if (cont->HasConsumers()) {
         // Check if one of the consumers is post event loop
            TIter nextconsumer(cont->GetConsumers());
            while ((task=(AliAnalysisTask*)nextconsumer())) {
               if (task->IsPostEventLoop()) {
                  fOutputs->Add(cont);
                  break;
               }
            }
         }
      }
   }   
   // Check if all special output containers have a file name provided
   TIter nextout(fOutputs);
   while ((cont=(AliAnalysisDataContainer*)nextout())) {
      if (cont->IsSpecialOutput() && !strlen(cont->GetFileName())) {
         Error("InitAnalysis", "Wrong container %s : a file name MUST be provided for special outputs", cont->GetName());
         return kFALSE;
      }
   }      
   fInitOK = kTRUE;
   return kTRUE;
}   

//______________________________________________________________________________
void AliAnalysisManager::PrintStatus(Option_t *option) const
{
// Print task hierarchy.
   if (!fInitOK) {
      Info("PrintStatus", "Analysis manager %s not initialized : call InitAnalysis() first", GetName());
      return;
   }   
   Bool_t getsysInfo = ((fNSysInfo>0) && (fMode==kLocalAnalysis))?kTRUE:kFALSE;
   if (getsysInfo)
      Info("PrintStatus", "System information will be collected each %lld events", fNSysInfo);
   TIter next(fTopTasks);
   AliAnalysisTask *task;
   while ((task=(AliAnalysisTask*)next()))
      task->PrintTask(option);
}

//______________________________________________________________________________
void AliAnalysisManager::ResetAnalysis()
{
// Reset all execution flags and clean containers.
   CleanContainers();
}

//______________________________________________________________________________
void AliAnalysisManager::StartAnalysis(const char *type, TTree *tree, Long64_t nentries, Long64_t firstentry)
{
// Start analysis for this manager. Analysis task can be: LOCAL, PROOF or GRID.
// Process nentries starting from firstentry
   if (!fInitOK) {
      Error("StartAnalysis","Analysis manager was not initialized !");
      return;
   }
   if (fDebug > 0) printf("StartAnalysis %s\n",GetName());
   TString anaType = type;
   anaType.ToLower();
   fMode = kLocalAnalysis;
   if (tree) {
      if (anaType.Contains("proof"))     fMode = kProofAnalysis;
      else if (anaType.Contains("grid")) fMode = kGridAnalysis;
   }
   if (fMode == kGridAnalysis) {
      Warning("StartAnalysis", "GRID analysis mode not implemented. Running local.");
      fMode = kLocalAnalysis;
   }
   char line[256];
   SetEventLoop(kFALSE);
   // Enable event loop mode if a tree was provided
   if (tree) SetEventLoop(kTRUE);

   TChain *chain = 0;
   TString ttype = "TTree";
   if (tree->IsA() == TChain::Class()) {
      chain = (TChain*)tree;
      if (!chain || !chain->GetListOfFiles()->First()) {
         Error("StartAnalysis", "Cannot process null or empty chain...");
         return;
      }   
      ttype = "TChain";
   }   

   // Initialize locally all tasks
   TIter next(fTasks);
   AliAnalysisTask *task;
   while ((task=(AliAnalysisTask*)next())) {
      task->LocalInit();
   }
   
   switch (fMode) {
      case kLocalAnalysis:
         if (!tree) {
            TIter nextT(fTasks);
            // Call CreateOutputObjects for all tasks
            while ((task=(AliAnalysisTask*)nextT())) {
               TDirectory *curdir = gDirectory;
               task->CreateOutputObjects();
               if (curdir) curdir->cd();
            }   
            ExecAnalysis();
            Terminate();
            return;
         } 
         // Run tree-based analysis via AliAnalysisSelector  
         cout << "===== RUNNING LOCAL ANALYSIS " << GetName() << " ON TREE " << tree->GetName() << endl;
         sprintf(line, "AliAnalysisSelector *selector = new AliAnalysisSelector((AliAnalysisManager*)0x%lx);",(ULong_t)this);
         gROOT->ProcessLine(line);
         sprintf(line, "((%s*)0x%lx)->Process(selector, \"\",%lld, %lld);",ttype.Data(),(ULong_t)tree, nentries, firstentry);
         gROOT->ProcessLine(line);
         break;
      case kProofAnalysis:
         if (!gROOT->GetListOfProofs() || !gROOT->GetListOfProofs()->GetEntries()) {
            printf("StartAnalysis: no PROOF!!!\n");
            return;
         }   
         sprintf(line, "gProof->AddInput((TObject*)0x%lx);", (ULong_t)this);
         gROOT->ProcessLine(line);
         if (chain) {
            chain->SetProof();
            cout << "===== RUNNING PROOF ANALYSIS " << GetName() << " ON CHAIN " << chain->GetName() << endl;
            chain->Process("AliAnalysisSelector", "", nentries, firstentry);
         } else {
            printf("StartAnalysis: no chain\n");
            return;
         }      
         break;
      case kGridAnalysis:
         Warning("StartAnalysis", "GRID analysis mode not implemented. Running local.");
   }   
}   

//______________________________________________________________________________
void AliAnalysisManager::StartAnalysis(const char *type, const char *dataset, Long64_t nentries, Long64_t firstentry)
{
// Start analysis for this manager on a given dataset. Analysis task can be: 
// LOCAL, PROOF or GRID. Process nentries starting from firstentry.
   if (!fInitOK) {
      Error("StartAnalysis","Analysis manager was not initialized !");
      return;
   }
   if (fDebug > 0) printf("StartAnalysis %s\n",GetName());
   TString anaType = type;
   anaType.ToLower();
   if (!anaType.Contains("proof")) {
      Error("Cannot process datasets in %s mode. Try PROOF.", type);
      return;
   }   
   fMode = kProofAnalysis;
   char line[256];
   SetEventLoop(kTRUE);
   // Set the dataset flag
   TObject::SetBit(kUseDataSet);
   fTree = 0;

   // Initialize locally all tasks
   TIter next(fTasks);
   AliAnalysisTask *task;
   while ((task=(AliAnalysisTask*)next())) {
      task->LocalInit();
   }
   
   if (!gROOT->GetListOfProofs() || !gROOT->GetListOfProofs()->GetEntries()) {
      printf("StartAnalysis: no PROOF!!!\n");
      return;
   }   
   sprintf(line, "gProof->AddInput((TObject*)0x%lx);", (ULong_t)this);
   gROOT->ProcessLine(line);
   sprintf(line, "gProof->GetDataSet(\"%s\");", dataset);
   if (!gROOT->ProcessLine(line)) {
      Error("StartAnalysis", "Dataset %s not found", dataset);
      return;
   }   
   sprintf(line, "gProof->Process(\"%s\", \"AliAnalysisSelector\", \"\", %lld, %lld);",
           dataset, nentries, firstentry);
   cout << "===== RUNNING PROOF ANALYSIS " << GetName() << " ON DATASET " << dataset << endl;
   gROOT->ProcessLine(line);
}   

//______________________________________________________________________________
TFile *AliAnalysisManager::OpenProofFile(const char *filename, const char *option)
{
// Opens a special output file used in PROOF.
   char line[256];
   if (fMode!=kProofAnalysis || !fSelector) {
      Error("OpenProofFile","Cannot open PROOF file %s",filename);
      return NULL;
   }   
   sprintf(line, "TProofOutputFile *pf = new TProofOutputFile(\"%s\");", filename);
   if (fDebug > 1) printf("=== %s\n", line);
   gROOT->ProcessLine(line);
   sprintf(line, "pf->OpenFile(\"%s\");", option);
   gROOT->ProcessLine(line);
   if (fDebug > 1) {
      gROOT->ProcessLine("pf->Print()");
      printf(" == proof file name: %s\n", gFile->GetName());
   }   
   sprintf(line, "((TList*)0x%lx)->Add(pf);",(ULong_t)fSelector->GetOutputList());
   if (fDebug > 1) printf("=== %s\n", line);
   gROOT->ProcessLine(line);
   return gFile;
}   

//______________________________________________________________________________
void AliAnalysisManager::ExecAnalysis(Option_t *option)
{
// Execute analysis.
   static Long64_t ncalls = 0;
   Bool_t getsysInfo = ((fNSysInfo>0) && (fMode==kLocalAnalysis))?kTRUE:kFALSE;
   if (getsysInfo && ncalls==0) AliSysInfo::AddStamp("Start", (Int_t)ncalls);
   ncalls++;
   if (!fInitOK) {
     Error("ExecAnalysis", "Analysis manager was not initialized !");
      return;
   }   
   AliAnalysisTask *task;
   // Check if the top tree is active.
   if (fTree) {
      TIter next(fTasks);
   // De-activate all tasks
      while ((task=(AliAnalysisTask*)next())) task->SetActive(kFALSE);
      AliAnalysisDataContainer *cont = (AliAnalysisDataContainer*)fInputs->At(0);
      if (!cont) {
	      Error("ExecAnalysis","Cannot execute analysis in TSelector mode without at least one top container");
         return;
      }   
      cont->SetData(fTree); // This will notify all consumers
      Long64_t entry = fTree->GetTree()->GetReadEntry();
      
//
//    Call BeginEvent() for optional input/output and MC services 
      if (fInputEventHandler)   fInputEventHandler  ->BeginEvent(entry);
      if (fOutputEventHandler)  fOutputEventHandler ->BeginEvent(entry);
      if (fMCtruthEventHandler) fMCtruthEventHandler->BeginEvent(entry);
//
//    Execute the tasks
//      TIter next1(cont->GetConsumers());
      TIter next1(fTopTasks);
      while ((task=(AliAnalysisTask*)next1())) {
         if (fDebug >1) {
            cout << "    Executing task " << task->GetName() << endl;
         }   
	 
         task->ExecuteTask(option);
      }
//
//    Call FinishEvent() for optional output and MC services 
      if (fInputEventHandler)   fInputEventHandler  ->FinishEvent();
      if (fOutputEventHandler)  fOutputEventHandler ->FinishEvent();
      if (fMCtruthEventHandler) fMCtruthEventHandler->FinishEvent();
      // Gather system information if requested
      if (getsysInfo && ((ncalls%fNSysInfo)==0)) 
         AliSysInfo::AddStamp(Form("Event#%lld",ncalls),(Int_t)ncalls);
      return;
   }   
   // The event loop is not controlled by TSelector   
//
//  Call BeginEvent() for optional input/output and MC services 
   if (fInputEventHandler)   fInputEventHandler  ->BeginEvent(-1);
   if (fOutputEventHandler)  fOutputEventHandler ->BeginEvent(-1);
   if (fMCtruthEventHandler) fMCtruthEventHandler->BeginEvent(-1);
   TIter next2(fTopTasks);
   while ((task=(AliAnalysisTask*)next2())) {
      task->SetActive(kTRUE);
      if (fDebug > 1) {
         cout << "    Executing task " << task->GetName() << endl;
      }   
      task->ExecuteTask(option);
   }   
//
// Call FinishEvent() for optional output and MC services 
   if (fInputEventHandler)   fInputEventHandler  ->FinishEvent();
   if (fOutputEventHandler)  fOutputEventHandler ->FinishEvent();
   if (fMCtruthEventHandler) fMCtruthEventHandler->FinishEvent();
}

//______________________________________________________________________________
void AliAnalysisManager::FinishAnalysis()
{
// Finish analysis.
}

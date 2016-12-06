/// \file AliO2AnalysisManager.cxx
/// \brief implementation of the AliO2AnalysisManager class.
/// \since 2016-12-05
/// \author R.G.A. Deckers
/// \copyright
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html
///

#include "AliO2AnalysisManager.h"
#include "AliO2InputHandler.h"
#include <Riostream.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TClass.h>
#include <TEnv.h>
#include <TError.h>
#include <TFile.h>
#include <TFileCollection.h>
#include <TH1.h>
#include <TMap.h>
#include <TMath.h>
#include <TMethodCall.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <TTreeCache.h>
#include <cerrno>

#include "AliAnalysisDataContainer.h"
#include "AliAnalysisDataSlot.h"
#include "AliAnalysisGrid.h"
#include "AliAnalysisSelector.h"
#include "AliAnalysisStatistics.h"
#include "AliAnalysisTask.h"
#include "AliLog.h"
#include "AliSysInfo.h"
#include "AliVEvent.h"
#include "AliVEventHandler.h"
#include "AliVEventPool.h"

// root specific
ClassImp(AliO2AnalysisManager);

// default destructor
AliO2AnalysisManager::~AliO2AnalysisManager() {}

Bool_t AliO2AnalysisManager::initialize(const std::list<std::string> &files) {
  std::cout << "Starting to Init manager" << std::endl;
  AliO2InputHandler *handler =
      dynamic_cast<AliO2InputHandler *>(this->GetInputEventHandler());
  if (!handler) {
    std::cerr << "AliO2AnalysisManager used without a AliO2InputHandler!";
    return kFALSE;
  } else {
    fTree = nullptr;
    return handler->initialize(files);
  }
}

void AliO2AnalysisManager::ExecAnalysis(Option_t *option) {
  std::cout << "Starting to ExecAnalysis" << std::endl;
  AliO2InputHandler *handler =
      dynamic_cast<AliO2InputHandler *>(this->GetInputEventHandler());
  if (!handler) {
    report(FAIL, "AliO2AnalysisManager used without a AliO2InputHandler!");
    return;
  } else {
    // printf("Our tree contains: \n");
    AliAnalysisTask *task;
    // Call create output objects for each task.
    {
      report(INFO, "Calling CreateOutputObjects for each task...");
      TIter task_iter(fTasks);
      while ((task = (AliAnalysisTask *)task_iter())) {
        task->CreateOutputObjects();
      }
    }
    report(PASS, "Called CreateOutputObjects for each task");
    // Call each task for each event.
    report(INFO, "Calling looping of events...");
    while (handler->NextEvent()) {
      // report(INFO, "Got an event");
      TIter task_iter(fTasks);
      while ((task = (AliAnalysisTask *)task_iter())) {
        task->SetActive(kTRUE);
        task->ExecuteTask(option);
      }
      // report(PASS, "Executed all tasks");
    }
    report(PASS, "...over all events");
    // call Terminate for each task.
    {
      report(INFO, "Terminating every task...");
      TIter task_iter(fTasks);
      while ((task = (AliAnalysisTask *)task_iter())) {
        task->Terminate();
      }
      report(PASS, "DONE! Enjoy your fresh analysis results! :)");
    }
  }
}
//
// // Execute analysis.
// static Long64_t nentries = 0;
// static TTree *lastTree = 0;
// static TStopwatch *timer = new TStopwatch();
// // Only the first call to Process will trigger a true Notify. Other
// Notify
// // coming before is ignored.
// if (!TObject::TestBit(AliAnalysisManager::kTrueNotify)) {
//   TObject::SetBit(AliAnalysisManager::kTrueNotify);
//   Notify();
// }
//
// if (fDebug > 0)
//   printf("MGR: Processing timeframe #%d\n", fNcalls);
// else {
//   if (fTree && (fTree != lastTree)) {
//     nentries += fTree->GetEntries();
//     lastTree = fTree;
//   }
//   if (!fNcalls)
//     timer->Start();
//   if (!fIsRemote && TObject::TestBit(kUseProgressBar))
//     ProgressBar("Processing timeframe", fNcalls,
//                 TMath::Min(fMaxEntries, nentries), timer, kFALSE);
// }
// fIOTimer->Start(kTRUE);
// gROOT->cd();
// TDirectory *cdir = gDirectory;
// Bool_t getsysInfo =
//     ((fNSysInfo > 0) && (fMode == kLocalAnalysis)) ? kTRUE : kFALSE;
// if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//   AliSysInfo::AddStamp("Exec_start", (Int_t)fNcalls);
// if (!fInitOK) {
//   Error("ExecAnalysis", "Analysis manager was not initialized !");
//   if (cdir)
//     cdir->cd();
//   return;
// }
// fNcalls++;
// AliAnalysisTask *task;
// // Reset the analysis
// ResetAnalysis();
// // Check if the top tree is active.
// if (fTree) {
//   if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//     AliSysInfo::AddStamp("Handlers_BeginEventGroup", fNcalls, 1002, 0);
//   TIter next(fTasks);
//   // De-activate all tasks (not needed anymore after ResetAnalysis
//   //      while ((task=(AliAnalysisTask*)next()))
//   task->SetActive(kFALSE);
//   AliAnalysisDataContainer *cont = fCommonInput;
//   if (!cont)
//     cont = (AliAnalysisDataContainer *)fInputs->At(0);
//   if (!cont) {
//     Error("ExecAnalysis",
//           "Cannot execute analysis in TSelector mode without "
//           "at least one top container");
//     if (cdir)
//       cdir->cd();
//     return;
//   }
//   cont->SetData(fTree); // This set activity for all tasks reading only
//   from
//                         // the top container
//   Long64_t entry = fTree->GetTree()->GetReadEntry();
//   //
//   //    Call BeginEvent() for optional input/output and MC services
//   if (fInputEventHandler)
//     fInputEventHandler->BeginEvent(entry);
//   if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//     AliSysInfo::AddStamp("Handlers_BeginEvent", fNcalls, 1000, 0);
//   //
//   //    Execute the tasks
//   //      TIter next1(cont->GetConsumers());
//   fIOTimer->Stop();
//   fIOTime += fIOTimer->RealTime();
//   fCPUTimer->Start(kTRUE);
//   TIter next1(fTopTasks);
//   Int_t itask = 0;
// while ((task = (AliAnalysisTask *)next1())) {
//   task->SetActive(kTRUE);
//   if (fDebug > 1) {
//     cout << "    Executing task " << task->GetName() << endl;
//   }
//     if (fStatistics)
//       fStatistics->StartTimer(GetTaskIndex(task), task->GetName(),
//                               task->ClassName());
//     do {
//       task->ExecuteTask(option);
//     } while (handler->NextEvent());
//     if (fStatistics)
//       fStatistics->StopTimer();
//     gROOT->cd();
//     if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//       AliSysInfo::AddStamp(task->ClassName(), fNcalls, itask, 1);
//     itask++;
//   }
//   fCPUTimer->Stop();
//   fCPUTime += fCPUTimer->RealTime();
//   fIOTimer->Start(kTRUE);
//   //
//   //    Call FinishEvent() for optional output and MC services
//   if (fInputEventHandler)
//     fInputEventHandler->FinishEvent();
//   if (fOutputEventHandler)
//     fOutputEventHandler->FinishEvent();
//   if (fMCtruthEventHandler)
//     fMCtruthEventHandler->FinishEvent();
//   // Gather system information if requested
//   if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//     AliSysInfo::AddStamp("Handlers_FinishEvent", fNcalls, 1001, 1);
//   if (cdir)
//     cdir->cd();
//   fIOTimer->Stop();
//   fIOTime += fIOTimer->RealTime();
//   return;
// }
// // The event loop is not controlled by TSelector
// //
// //  Call BeginEvent() for optional input/output and MC services
// fIOTimer->Start(kTRUE);
// if (fInputEventHandler)
//   fInputEventHandler->BeginEvent(-1);
// if (fOutputEventHandler)
//   fOutputEventHandler->BeginEvent(-1);
// if (fMCtruthEventHandler)
//   fMCtruthEventHandler->BeginEvent(-1);
// fIOTimer->Stop();
// fIOTime += fIOTimer->RealTime();
// gROOT->cd();
// if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//   AliSysInfo::AddStamp("Handlers_BeginEvent", fNcalls, 1000, 0);
// fCPUTimer->Start(kTRUE);
// TIter next2(fTopTasks);
// while ((task = (AliAnalysisTask *)next2())) {
//   task->SetActive(kTRUE);
//   if (fDebug > 1) {
//     cout << "    Executing task " << task->GetName() << endl;
//   }
//   if (fStatistics)
//     fStatistics->StartTimer(GetTaskIndex(task), task->GetName(),
//                             task->ClassName());
//   task->ExecuteTask(option);
//   if (fStatistics)
//     fStatistics->StopTimer();
//   gROOT->cd();
// }
// fCPUTimer->Stop();
// fCPUTime += fCPUTimer->RealTime();
// //
// // Call FinishEvent() for optional output and MC services
// fIOTimer->Start(kTRUE);
// if (fInputEventHandler)
//   fInputEventHandler->FinishEvent();
// if (fOutputEventHandler)
//   fOutputEventHandler->FinishEvent();
// if (fMCtruthEventHandler)
//   fMCtruthEventHandler->FinishEvent();
// if (getsysInfo && ((fNcalls % fNSysInfo) == 0))
//   AliSysInfo::AddStamp("Handlers_FinishEvent", fNcalls, 1000, 1);
// if (cdir)
//   cdir->cd();
// fIOTimer->Stop();
// fIOTime += fIOTimer->RealTime();

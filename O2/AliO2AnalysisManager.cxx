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
    // We do not have an O2 input
    report(
        FAIL,
        "AliO2AnalysisManager::initialize used without a AliO2InputHandler!");
  } else {
    fTree = nullptr;
    return handler->initialize(files);
  }
}

void AliO2AnalysisManager::ExecAnalysis(Option_t *option) {
  AliO2InputHandler *handler =
      dynamic_cast<AliO2InputHandler *>(this->GetInputEventHandler());
  if (!handler) {
    // report(WARN, "AliO2AnalysisManager used without a AliO2InputHandler! "
    //              "giving back control to AliAnalysisManager");
    return AliAnalysisManager::ExecAnalysis(option);
  } else {
    RunLocalInit();
    // printf("Our tree contains: \n");
    AliAnalysisTask *task;
    // Call create output objects for each task.
    {
      report(INFO, "Calling CreateOutputObjects for each task...");
      TIter task_iter(fTasks);
      while ((task = (AliAnalysisTask *)task_iter())) {
        task->CreateOutputObjects();
        if (!task->CheckPostData()) {
          Error("SlaveBegin", "####### IMPORTANT! ####### \n\n\n\
                        Task %s (%s) did not call PostData() for all its outputs in (User)CreateOutputObjects()\n\n\
                        ########### FIX YOUR CODE, THIS WILL PRODUCE A FATAL ERROR IN FUTURE! ###########",
                task->GetName(), task->ClassName());
        }
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
      if (fOutputEventHandler)
        fOutputEventHandler->FinishEvent();
      report(PASS, "DONE! Enjoy your fresh analysis results! :)");
    }
  }
  Terminate();
}

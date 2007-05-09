// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTSystem.cxx
    @author Matthias Richter
    @date   
    @brief  Implementation of HLT module management.
*/

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTStdIncludes.h"
#include "AliHLTSystem.h"
#include "AliHLTComponentHandler.h"
#include "AliHLTComponent.h"
#include "AliHLTConfiguration.h"
#include "AliHLTConfigurationHandler.h"
#include "AliHLTTask.h"
#include "AliHLTModuleAgent.h"
#include "AliHLTOfflineInterface.h"
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TStopwatch.h>

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTSystem)

AliHLTSystem::AliHLTSystem()
  :
  fpComponentHandler(new AliHLTComponentHandler()),
  fpConfigurationHandler(new AliHLTConfigurationHandler()),
  fTaskList(),
  fState(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

  if (fgNofInstances++>0)
    HLTWarning("multiple instances of AliHLTSystem, you should not use more than one at a time");

  SetGlobalLoggingLevel(kHLTLogDefault);
  if (fpComponentHandler) {
    AliHLTComponentEnvironment env;
    memset(&env, 0, sizeof(AliHLTComponentEnvironment));
    env.fAllocMemoryFunc=AliHLTSystem::AllocMemory;
    env.fLoggingFunc=NULL;
    fpComponentHandler->SetEnvironment(&env);
  } else {
    HLTFatal("can not create Component Handler");
  }
  if (fpConfigurationHandler) {
    AliHLTConfiguration::GlobalInit(fpConfigurationHandler);
  } else {
    HLTFatal("can not create Configuration Handler");
  }
}

AliHLTSystem::AliHLTSystem(const AliHLTSystem&)
  :
  AliHLTLogging(),
  fpComponentHandler(NULL),
  fpConfigurationHandler(NULL),
  fTaskList(),
  fState(0)
{
  // see header file for class documentation
  if (fgNofInstances++>0)
    HLTWarning("multiple instances of AliHLTSystem, you should not use more than one at a time");

  HLTFatal("copy constructor untested");
}

AliHLTSystem& AliHLTSystem::operator=(const AliHLTSystem&)
{ 
  // see header file for class documentation
  HLTFatal("assignment operator untested");
  return *this;
}

AliHLTSystem::~AliHLTSystem()
{
  // see header file for class documentation
  fgNofInstances--;
  CleanTaskList();
  AliHLTConfiguration::GlobalDeinit(fpConfigurationHandler);
  if (fpConfigurationHandler) {
    delete fpConfigurationHandler;
  }
  fpConfigurationHandler=NULL;
  
  if (fpComponentHandler) {
    delete fpComponentHandler;
  }
  fpComponentHandler=NULL;
}

int AliHLTSystem::fgNofInstances=0;

int AliHLTSystem::AddConfiguration(AliHLTConfiguration* pConf)
{
  // see header file for class documentation
  int iResult=0;
  if (pConf) {
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::InsertConfiguration(AliHLTConfiguration* pConf, AliHLTConfiguration* pPrec)
{
  // see header file for class documentation
  int iResult=0;
  if (pConf) {
    if (pPrec) {
      // find the position
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::DeleteConfiguration(AliHLTConfiguration* pConf)
{
  // see header file for class documentation
  int iResult=0;
  if (pConf) {
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::BuildTaskList(AliHLTConfiguration* pConf)
{
  // see header file for class documentation
  int iResult=0;
  if (pConf) {
    AliHLTTask* pTask=NULL;
    if ((pTask=FindTask(pConf->GetName()))!=NULL) {
      if (pTask->GetConf()!=pConf) {
	HLTError("configuration missmatch, there is already a task with configuration name \"%s\", but it is different. Most likely configuration %p is not registered properly", pConf->GetName(), pConf);
	iResult=-EEXIST;
	pTask=NULL;
      }
    } else if (pConf->SourcesResolved(1)!=1) {
	HLTError("configuration \"%s\" has unresolved sources, aborting ...", pConf->GetName());
	iResult=-ENOLINK;
    } else {
      pTask=new AliHLTTask(pConf);
      if (pTask==NULL) {
	iResult=-ENOMEM;
      }
    }
    if (pTask) {
      // check for circular dependencies
      if ((iResult=pConf->FollowDependency(pConf->GetName()))>0) {
	HLTError("detected circular dependency for configuration \"%s\"", pTask->GetName());
	pTask->PrintDependencyTree(pTask->GetName(), 1/*use the configuration list*/);
	HLTError("aborted ...");
	iResult=-ELOOP;
      }
      if (iResult>=0) {
	// check whether all dependencies are already in the task list
	// create the missing ones
	// this step is an iterative process which calls this function again for the missing
	// configurations, in order to avoid the currently processed task to be created
	// again it is added to the list temporarily and removed afterwards
	// This is of high importance to preserve the order of the tasks. Furthermore, the
	// InsertTask method has to be used in order to set all the cross links right 
	fTaskList.Add(pTask);
	AliHLTConfiguration* pDep=pConf->GetFirstSource();
	while (pDep!=NULL && iResult>=0) {
	  if (FindTask(pDep->GetName())==NULL) {
	    iResult=BuildTaskList(pDep);
	  }
	  pDep=pConf->GetNextSource();
	}
	// remove the temporarily added task
	fTaskList.Remove(pTask);

	// insert the task and set the cross-links
	if (iResult>=0) {
	  iResult=InsertTask(pTask);
	}
      } else {
	delete pTask;
	pTask=NULL;
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::CleanTaskList()
{
  // see header file for class documentation
  int iResult=0;
  TObjLink* lnk=NULL;
  while ((lnk=fTaskList.FirstLink())!=NULL) {
    fTaskList.Remove(lnk);
    delete (lnk->GetObject());
  }
  return iResult;
}

int AliHLTSystem::InsertTask(AliHLTTask* pTask)
{
  // see header file for class documentation
  int iResult=0;
  TObjLink *lnk = NULL;
  if ((iResult=pTask->CheckDependencies())>0)
    lnk=fTaskList.FirstLink();
  while (lnk && iResult>0) {
    AliHLTTask* pCurr = (AliHLTTask*)lnk->GetObject();
    //HLTDebug("checking  \"%s\"", pCurr->GetName());
    iResult=pTask->Depends(pCurr);
    if (iResult>0) {
      iResult=pTask->SetDependency(pCurr);
      pCurr->SetTarget(pTask);
      HLTDebug("set dependency  \"%s\" for configuration \"%s\"", pCurr->GetName(), pTask->GetName());
    }
    if (pCurr->Depends(pTask)) {
      // circular dependency
      HLTError("circular dependency: can not resolve dependencies for configuration \"%s\"", pTask->GetName());
      iResult=-ELOOP;
    } else if ((iResult=pTask->CheckDependencies())>0) {
      lnk = lnk->Next();
    }
  }
  if (iResult==0) {
      if (lnk) {
	fTaskList.AddAfter(lnk, pTask);
      } else {
	fTaskList.AddFirst(pTask);
      }
      HLTDebug("task \"%s\" inserted", pTask->GetName());
  } else if (iResult>0) {
    HLTError("can not resolve dependencies for configuration \"%s\" (%d unresolved)", pTask->GetName(), iResult);
    iResult=-ENOLINK;
  }
  return iResult;
}

AliHLTTask* AliHLTSystem::FindTask(const char* id)
{
  // see header file for class documentation
  AliHLTTask* pTask=NULL;
  if (id) {
    pTask=(AliHLTTask*)fTaskList.FindObject(id); 
  }
  return pTask;
}

void AliHLTSystem::PrintTaskList()
{
  // see header file for class documentation
  HLTLogKeyword("task list");
  TObjLink *lnk = NULL;
  HLTMessage("Task List");
  lnk=fTaskList.FirstLink();
  while (lnk) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      HLTMessage("  %s - status:", obj->GetName());
      AliHLTTask* pTask=(AliHLTTask*)obj;
      pTask->PrintStatus();
    } else {
    }
    lnk = lnk->Next();
  }
}

int AliHLTSystem::Run(Int_t iNofEvents) 
{
  // see header file for class documentation
  int iResult=0;
  int iCount=0;
  SetStatusFlags(kRunning);
  TStopwatch StopwatchBase; StopwatchBase.Reset();
  TStopwatch StopwatchDA; StopwatchDA.Reset();
  TStopwatch StopwatchInput; StopwatchInput.Reset();
  TStopwatch StopwatchOutput; StopwatchOutput.Reset();
  TObjArray Stopwatches;
  Stopwatches.AddAt(&StopwatchBase, (int)AliHLTComponent::kSWBase);
  Stopwatches.AddAt(&StopwatchDA, (int)AliHLTComponent::kSWDA);
  Stopwatches.AddAt(&StopwatchInput, (int)AliHLTComponent::kSWInput);
  Stopwatches.AddAt(&StopwatchOutput, (int)AliHLTComponent::kSWOutput);
  if ((iResult=InitTasks())>=0 && (iResult=InitBenchmarking(&Stopwatches))>=0) {
    if ((iResult=StartTasks())>=0) {
      for (int i=0; i<iNofEvents && iResult>=0; i++) {
	iResult=ProcessTasks(i);
	if (iResult>=0) {
	  HLTInfo("Event %d successfully finished (%d)", i, iResult);
	  iResult=0;
	  iCount++;
	} else {
	  HLTError("Processing of event %d failed (%d)", i, iResult);
	  // TODO: define different running modes to either ignore errors in
	  // event processing or not
	  // currently ignored 
	  //iResult=0;
	}
      }
      StopTasks();
    } else {
      HLTError("can not start task list");
    }
    DeinitTasks();
  } else if (iResult!=-ENOENT) {
    HLTError("can not initialize task list");
  }
  if (iResult>=0) {
    iResult=iCount;
    HLTInfo("HLT statistics:\n"
	    "    base:              R:%.3fs C:%.3fs\n"
	    "    input:             R:%.3fs C:%.3fs\n"
	    "    output:            R:%.3fs C:%.3fs\n"
	    "    event processing : R:%.3fs C:%.3fs"
	    , StopwatchBase.RealTime(),StopwatchBase.CpuTime()
	    , StopwatchInput.RealTime(),StopwatchInput.CpuTime()
	    , StopwatchOutput.RealTime(),StopwatchOutput.CpuTime()
	    , StopwatchDA.RealTime(),StopwatchDA.CpuTime());
  }
  ClearStatusFlags(kRunning);
  return iResult;
}

int AliHLTSystem::InitTasks()
{
  // see header file for class documentation
  int iResult=0;
  TObjLink *lnk=fTaskList.FirstLink();
  if (lnk==NULL) {
    HLTWarning("Task list is empty, aborting ...");
    return -ENOENT;
  }
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      iResult=pTask->Init(NULL, fpComponentHandler);
    } else {
    }
    lnk = lnk->Next();
  }
  if (iResult<0) {
  }
  return iResult;
}

int AliHLTSystem::InitBenchmarking(TObjArray* pStopwatches)
{
  // see header file for class documentation
  if (pStopwatches==NULL) return -EINVAL;

  int iResult=0;
  TObjLink *lnk=fTaskList.FirstLink();
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      AliHLTComponent* pComp=NULL;
      if (iResult>=0 && (pComp=pTask->GetComponent())!=NULL) {
	switch (pComp->GetComponentType()) {
	case AliHLTComponent::kProcessor:
	  pComp->SetStopwatches(pStopwatches);
	  break;
	case AliHLTComponent::kSource:
	  {
	    // this switch determines whether the time consumption of the
	    // AliHLTComponent base methods should be counted to the input
	    // stopwatch or base stopwatch.
	    //int inputBase=(int)AliHLTComponent::kSWBase;
	    int inputBase=(int)AliHLTComponent::kSWInput;
	    pComp->SetStopwatch(pStopwatches->At(inputBase), AliHLTComponent::kSWBase);
	    pComp->SetStopwatch(pStopwatches->At((int)AliHLTComponent::kSWInput), AliHLTComponent::kSWDA);
	  }
	  break;
	case AliHLTComponent::kSink:
	  {
	    // this switch determines whether the time consumption of the
	    // AliHLTComponent base methods should be counted to the output
	    // stopwatch or base stopwatch.
	    //int outputBase=(int)AliHLTComponent::kSWBase;
	    int outputBase=(int)AliHLTComponent::kSWOutput;
	    pComp->SetStopwatch(pStopwatches->At(outputBase), AliHLTComponent::kSWBase);
	    pComp->SetStopwatch(pStopwatches->At((int)AliHLTComponent::kSWOutput), AliHLTComponent::kSWDA);
	  }
	  break;
	default:
	  HLTWarning("unknown component type %d", (int)pComp->GetComponentType());
	}
      }
    } else {
    }
    lnk = lnk->Next();
  }
  return iResult;
}

int AliHLTSystem::StartTasks()
{
  // see header file for class documentation
  int iResult=0;
  TObjLink *lnk=fTaskList.FirstLink();
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      iResult=pTask->StartRun();
    } else {
    }
    lnk = lnk->Next();
  }
  if (iResult<0) {
  }
  return iResult;
}

int AliHLTSystem::ProcessTasks(Int_t eventNo)
{
  // see header file for class documentation
  int iResult=0;
  HLTDebug("processing event no %d", eventNo);
  TObjLink *lnk=fTaskList.FirstLink();
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      iResult=pTask->ProcessTask(eventNo);
      HLTDebug("task %s finnished (%d)", pTask->GetName(), iResult);
    } else {
    }
    lnk = lnk->Next();
  }
  return iResult;
}

int AliHLTSystem::StopTasks()
{
  // see header file for class documentation
  int iResult=0;
  TObjLink *lnk=fTaskList.FirstLink();
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      iResult=pTask->EndRun();
    } else {
    }
    lnk = lnk->Next();
  }
  return iResult;
}

int AliHLTSystem::DeinitTasks()
{
  // see header file for class documentation
  int iResult=0;
  TObjLink *lnk=fTaskList.FirstLink();
  while (lnk && iResult>=0) {
    TObject* obj=lnk->GetObject();
    if (obj) {
      AliHLTTask* pTask=(AliHLTTask*)obj;
      iResult=pTask->Deinit();
    } else {
    }
    lnk = lnk->Next();
  }
  return iResult;
}

void* AliHLTSystem::AllocMemory( void* param, unsigned long size )
{
  // see header file for class documentation
  if (param==NULL) {
    // get rid of 'unused parameter' warning
  }
  return (void*)new char[size];
}

int AliHLTSystem::Reconstruct(int nofEvents, AliRunLoader* runLoader, 
			      AliRawReader* rawReader)
{
  // see header file for class documentation
  int iResult=0;
  if (runLoader) {
    HLTInfo("Run Loader %p, Raw Reader %p , %d events", runLoader, rawReader, nofEvents);
    if (CheckStatus(kReady)) {
      if ((iResult=AliHLTOfflineInterface::SetParamsToComponents(runLoader, rawReader))>=0) {
	iResult=Run(nofEvents);
      }
    } else {
      HLTError("wrong state %#x, required flags %#x", GetStatusFlags(), kReady);
    }
  } else {
    HLTError("missing run loader instance");
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::FillESD(int eventNo, AliRunLoader* runLoader, AliESD* esd)
{
  // see header file for class documentation
  int iResult=0;
  if (runLoader) {
    HLTInfo("Event %d: Run Loader %p, ESD %p", eventNo, runLoader, esd);
    iResult=AliHLTOfflineInterface::FillComponentESDs(eventNo, runLoader, esd);
  } else {
    HLTError("missing run loader/ESD instance(s)");
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::LoadComponentLibraries(const char* libraries)
{
  // see header file for class documentation
  int iResult=0;
  if (libraries) {
    if (fpComponentHandler) {
      TString libs(libraries);
      TObjArray* pTokens=libs.Tokenize(" ");
      if (pTokens) {
	int iEntries=pTokens->GetEntries();
	for (int i=0; i<iEntries && iResult>=0; i++) {
	  iResult=fpComponentHandler->LoadLibrary((((TObjString*)pTokens->At(i))->GetString()).Data());
	}
	delete pTokens;
      }
      if (iResult>=0) {
	SetStatusFlags(kLibrariesLoaded);
      } else {
	// lets see if we need this, probably not
	//fpComponentHandler->UnloadLibraries();
	ClearStatusFlags(kLibrariesLoaded);
      }
    } else {
      iResult=-EFAULT;
      HLTFatal("no component handler available");
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTSystem::Configure(AliRunLoader* runloader)
{
  // see header file for class documentation
  int iResult=0;
  if (CheckStatus(kRunning)) {
    HLTError("HLT system in running state, can not configure");
    return -EBUSY;
  }
  if (CheckFilter(kHLTLogDebug))
    AliHLTModuleAgent::PrintStatus();
  ClearStatusFlags(kConfigurationLoaded|kTaskListCreated);
  iResult=LoadConfigurations(runloader);
  if (iResult>=0) {
    SetStatusFlags(kConfigurationLoaded);
    iResult=BuildTaskListsFromTopConfigurations(runloader);
    if (iResult>=0) {
      SetStatusFlags(kTaskListCreated);
    }
  }
  if (iResult<0) SetStatusFlags(kError);
  
  return iResult;
}

int AliHLTSystem::Reset(int bForce)
{
  // see header file for class documentation
  int iResult=0;
  if (!bForce && CheckStatus(kRunning)) {
    HLTError("HLT system in running state, can not configure");
    return -EBUSY;
  }
  CleanTaskList();
  ClearStatusFlags(~kUninitialized);
  return iResult;
}

int AliHLTSystem::LoadConfigurations(AliRunLoader* runloader)
{
  // see header file for class documentation
  if (CheckStatus(kRunning)) {
    HLTError("HLT system in running state, can not configure");
    return -EBUSY;
  }
  int iResult=0;
  AliHLTModuleAgent* pAgent=AliHLTModuleAgent::GetFirstAgent();
  while (pAgent && iResult>=0) {
    const char* deplibs=pAgent->GetRequiredComponentLibraries();
    if (deplibs) {
      HLTDebug("load libraries \'%s\' for agent %s (%p)", deplibs, pAgent->GetName(), pAgent);
      iResult=LoadComponentLibraries(deplibs);
    }
    if (iResult>=0) {
      HLTDebug("load configurations for agent %s (%p)", pAgent->GetName(), pAgent);
      pAgent->CreateConfigurations(fpConfigurationHandler, runloader);
      pAgent=AliHLTModuleAgent::GetNextAgent();
    }
  }
  return iResult;
}

int AliHLTSystem::BuildTaskListsFromTopConfigurations(AliRunLoader* runloader)
{
  // see header file for class documentation
  if (CheckStatus(kRunning)) {
    HLTError("HLT system in running state, can not configure");
    return -EBUSY;
  }
  if (!CheckStatus(kConfigurationLoaded)) {
    HLTWarning("configurations not yet loaded");
    return 0;
  }

  int iResult=0;
  AliHLTModuleAgent* pAgent=AliHLTModuleAgent::GetFirstAgent();
  while (pAgent && iResult>=0) {
    TString tops=pAgent->GetLocalRecConfigurations(runloader);
    HLTDebug("top configurations for agent %s (%p): %s", pAgent->GetName(), pAgent, tops.Data());
    TObjArray* pTokens=tops.Tokenize(" ");
    if (pTokens) {
      int iEntries=pTokens->GetEntries();
      for (int i=0; i<iEntries && iResult>=0; i++) {
	const char* pCID=((TObjString*)pTokens->At(i))->GetString().Data();
	AliHLTConfiguration* pConf=fpConfigurationHandler->FindConfiguration(pCID);
	if (pConf) {
	  iResult=BuildTaskList(pConf);
	} else {
	  HLTWarning("can not find top configuration %s", pCID);
	}
      }
      delete pTokens;
    }
    
    pAgent=AliHLTModuleAgent::GetNextAgent();
  }
  if (iResult>=0) SetStatusFlags(kTaskListCreated);

  return iResult;
}

int AliHLTSystem::CheckStatus(int flag)
{
  // see header file for class documentation
  if (flag==kUninitialized && flag==fState) return 1;
  if ((fState&flag)==flag) return 1;
  return 0;
}

int AliHLTSystem::GetStatusFlags()
{
  // see header file for class documentation
  return fState;
}

int AliHLTSystem::SetStatusFlags(int flags)
{
  // see header file for class documentation
  fState|=flags;
  return fState;
}

int AliHLTSystem::ClearStatusFlags(int flags)
{
  // see header file for class documentation
  fState&=~flags;
  return fState;
}

void* AliHLTSystem::FindDynamicSymbol(const char* library, const char* symbol)
{
  if (fpComponentHandler==NULL) return NULL;
  return fpComponentHandler->FindSymbol(library, symbol);
}

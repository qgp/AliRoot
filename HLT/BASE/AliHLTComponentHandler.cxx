// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
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

/** @file   AliHLTComponentHandler.cxx
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  Implementation of HLT component handler. */

#if __GNUC__>= 3
using namespace std;
#endif
//#undef HAVE_DLFCN_H
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#else
//#include <Riostream.h>
#include <TSystem.h>
#endif //HAVE_DLFCN_H
#include "AliHLTStdIncludes.h"
#include "AliHLTComponentHandler.h"
#include "AliHLTComponent.h"
#include "AliHLTDataTypes.h"
#include "AliHLTSystem.h"

// the standard components
// #include "AliHLTFilePublisher.h"
// #include "AliHLTFileWriter.h"
// #include "AliHLTRootFilePublisherComponent.h"
// #include "AliHLTRootFileWriterComponent.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTComponentHandler)

AliHLTComponentHandler::AliHLTComponentHandler()
  :
  fComponentList(),
  fScheduleList(),
  fLibraryList(),
  fEnvironment(),
  fStandardList()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
  memset(&fEnvironment, 0, sizeof(AliHLTComponentEnvironment));
  AddStandardComponents();
}

AliHLTComponentHandler::AliHLTComponentHandler(AliHLTComponentEnvironment* pEnv)
  :
  fComponentList(),
  fScheduleList(),
  fLibraryList(),
  fEnvironment(),
  fStandardList()
{
  // see header file for class documentation
  if (pEnv) {
    memcpy(&fEnvironment, pEnv, sizeof(AliHLTComponentEnvironment));
    if (pEnv->fLoggingFunc) {
      // the AliHLTLogging::Init method also sets the stream output
      // and notification handler to AliLog. This should only be done
      // if the logging environment contains a logging function
      // for redirection
      AliHLTLogging::Init(pEnv->fLoggingFunc);
    }
  }  else
    memset(&fEnvironment, 0, sizeof(AliHLTComponentEnvironment));
  AddStandardComponents();
}

AliHLTComponentHandler::~AliHLTComponentHandler()
{
  // see header file for class documentation
  DeleteStandardComponents();
  UnloadLibraries();
}

int AliHLTComponentHandler::AnnounceVersion()
{
  // see header file for class documentation
  int iResult=0;
#ifdef PACKAGE_STRING
  void HLTbaseCompileInfo( char*& date, char*& time);
  char* date="";
  char* time="";
  HLTbaseCompileInfo(date, time);
  if (!date) date="unknown";
  if (!time) time="unknown";
  HLTInfo("%s build on %s (%s)", PACKAGE_STRING, date, time);
#else
  HLTInfo("ALICE High Level Trigger build on %s (%s) (embedded AliRoot build)", __DATE__, __TIME__);
#endif
  return iResult;
}

Int_t AliHLTComponentHandler::RegisterComponent(AliHLTComponent* pSample)
{
  // see header file for class documentation
  Int_t iResult=0;
  if (pSample) {
    if (FindComponent(pSample->GetComponentID())==NULL) {
      iResult=InsertComponent(pSample);
      if (iResult>=0) {
	HLTInfo("component %s registered", pSample->GetComponentID());
      }
    } else {
      // component already registered
      HLTDebug("component %s already registered, skipped", pSample->GetComponentID());
      iResult=-EEXIST;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::DeregisterComponent( const char* componentID )
{
  // see header file for class documentation
  int iResult=0;
  if (componentID) {
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

Int_t AliHLTComponentHandler::ScheduleRegister(AliHLTComponent* pSample)
{
  // see header file for class documentation
  Int_t iResult=0;
  if (pSample) {
    fScheduleList.push_back(pSample);
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::CreateComponent(const char* componentID, void* pEnvParam, int argc, const char** argv, AliHLTComponent*& component )
{
  // see header file for class documentation
  int iResult=0;
  if (componentID) {
    AliHLTComponent* pSample=FindComponent(componentID);
    if (pSample!=NULL) {
      component=pSample->Spawn();
      if (component) {
	HLTDebug("component \"%s\" created (%p)", componentID, component);
	if ((iResult=component->Init(&fEnvironment, pEnvParam, argc, argv))!=0) {
	  HLTError("Initialization of component \"%s\" failed with error %d", componentID, iResult);
	  delete component;
	  component=NULL;
	}
      } else {
	HLTError("can not spawn component \"%s\"", componentID);
	iResult=-ENOENT;
      }
    } else {
      HLTWarning("can not find component \"%s\"", componentID);
      iResult=-ENOENT;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

Int_t AliHLTComponentHandler::FindComponentIndex(const char* componentID)
{
  // see header file for class documentation
  Int_t iResult=0;
  if (componentID) {
    vector<AliHLTComponent*>::iterator element=fComponentList.begin();
    while (element!=fComponentList.end() && iResult>=0) {
      if (strcmp(componentID, (*element)->GetComponentID())==0) {
	break;
      }
      element++;
      iResult++;
    }
    if (element==fComponentList.end()) iResult=-ENOENT;
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

AliHLTComponent* AliHLTComponentHandler::FindComponent(const char* componentID)
{
  // see header file for class documentation
  AliHLTComponent* pSample=NULL;
  Int_t index=FindComponentIndex(componentID);
  if (index>=0) {
    pSample=(AliHLTComponent*)fComponentList.at(index);
  }
  return pSample;
}

Int_t AliHLTComponentHandler::InsertComponent(AliHLTComponent* pSample)
{
  // see header file for class documentation
  Int_t iResult=0;
  if (pSample!=NULL) {
    fComponentList.push_back(pSample);
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

void AliHLTComponentHandler::List() 
{
  // see header file for class documentation
  vector<AliHLTComponent*>::iterator element=fComponentList.begin();
  int index=0;
  while (element!=fComponentList.end()) {
    HLTInfo("%d. %s", index++, (*element++)->GetComponentID());
  }
}

void AliHLTComponentHandler::SetEnvironment(AliHLTComponentEnvironment* pEnv) 
{
  // see header file for class documentation
  if (pEnv) {
    memcpy(&fEnvironment, pEnv, sizeof(AliHLTComponentEnvironment));
    if (fEnvironment.fLoggingFunc) {
      // the AliHLTLogging::Init method also sets the stream output
      // and notification handler to AliLog. This should only be done
      // if the logging environment contains a logging function
      // for redirection
      AliHLTLogging::Init(fEnvironment.fLoggingFunc);
    }
  }
}

int AliHLTComponentHandler::LoadLibrary( const char* libraryPath )
{
  // see header file for class documentation
  int iResult=0;
  if (libraryPath) {
    AliHLTComponent::SetGlobalComponentHandler(this);
    AliHLTLibHandle hLib;
    const char* loadtype="";
#ifdef HAVE_DLFCN_H
    // use interface to the dynamic linking loader
    hLib.handle=dlopen(libraryPath, RTLD_NOW);
    loadtype="dlopen";
#else
    // use ROOT dynamic loader
    // check if the library was already loaded, as Load returns
    // 'failure' if the library was already loaded
    AliHLTLibHandle* pLib=FindLibrary(libraryPath);
    if (pLib) {
	int* pRootHandle=reinterpret_cast<int*>(pLib->handle);
	(*pRootHandle)++;
	HLTDebug("instance %d of library %s loaded", (*pRootHandle), libraryPath);
	hLib.handle=pRootHandle;
    }
    
    if (hLib.handle==NULL && gSystem->Load(libraryPath)==0) {
      int* pRootHandle=new int;
      if (pRootHandle) *pRootHandle=1;
      hLib.handle=pRootHandle;
      //HLTDebug("library %s loaded via gSystem", libraryPath);
    }
    loadtype="gSystem";
#endif //HAVE_DLFCN_H
    if (hLib.handle!=NULL) {
      // create TString object to store library path and use pointer as handle 
      hLib.name=new TString(libraryPath);
      HLTInfo("library %s loaded (%s)", libraryPath, loadtype);
      fLibraryList.insert(fLibraryList.begin(), hLib);
      iResult=RegisterScheduledComponents();
    } else {
      HLTError("can not load library %s", libraryPath);
#ifdef HAVE_DLFCN_H
      HLTError("dlopen error: %s", dlerror());
#endif //HAVE_DLFCN_H
#ifdef __APPLE__
      iResult=-EFTYPE;
#else
      iResult=-ELIBACC;
#endif
    }
    AliHLTComponent::UnsetGlobalComponentHandler();
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::UnloadLibrary( const char* libraryPath )
{
  // see header file for class documentation
  int iResult=0;
  if (libraryPath) {
    vector<AliHLTLibHandle>::iterator element=fLibraryList.begin();
    while (element!=fLibraryList.end()) {
      TString* pName=reinterpret_cast<TString*>((*element).name);
      if (pName->CompareTo(libraryPath)==0) {
	UnloadLibrary(*element);
	fLibraryList.erase(element);
	break;
      }
      element++;
  }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::UnloadLibrary(AliHLTComponentHandler::AliHLTLibHandle &handle)
{
  // see header file for class documentation
  int iResult=0;
  fgAliLoggingFunc=NULL;
  TString* pName=reinterpret_cast<TString*>(handle.name);
#ifdef HAVE_DLFCN_H
  dlclose(handle.handle);
#else
  int* pCount=reinterpret_cast<int*>(handle.handle);
  if (--(*pCount)==0) {
    if (pName) {
      /** Matthias 26.04.2007
       * I spent about a week to investigate a bug which seems to be in ROOT.
       * Under certain circumstances, TSystem::Unload crashes. The crash occured
       * for the first time, when libAliHLTUtil was loaded from AliHLTSystem right
       * after the ComponentHandler was created. It does not occur when dlopen is
       * used. 
       * It has most likely to do with the garbage collection and automatic
       * cleanup in ROOT. The crash occurs when ROOT is terminated and before
       * an instance of AliHLTSystem was created.
       *   root [0] AliHLTSystem gHLT
       * It does not occur when the instance was created dynamically (but not even
       * deleted)
       *   root [0] AliHLTSystem* gHLT=new AliHLTSystem
       *
       * For that reason, the libraries are not unloaded here, even though there
       * will be memory leaks.
      gSystem->Unload(pName->Data());
       */
    }
    else {
      HLTError("missing library name, can not unload");
    }
    delete pCount;
  }
#endif //HAVE_DLFCN_H
  handle.name=NULL;
  handle.handle=NULL;
  if (pName) {
    HLTDebug("unload library %s", pName->Data());
    delete pName;
  } else {
    HLTWarning("missing name for unloaded library");
  }
  pName=NULL;
  return iResult;
}

int AliHLTComponentHandler::UnloadLibraries()
{
  // see header file for class documentation
  int iResult=0;
  vector<AliHLTLibHandle>::iterator element=fLibraryList.begin();
  while (element!=fLibraryList.end()) {
    UnloadLibrary(*element);
    fLibraryList.erase(element);
    element=fLibraryList.begin();
  }
  return iResult;
}

void* AliHLTComponentHandler::FindSymbol(const char* library, const char* symbol)
{
  // see header file for class documentation
  AliHLTLibHandle* hLib=FindLibrary(library);
  if (hLib==NULL) return NULL;
  void* pFunc=NULL;
#ifdef HAVE_DLFCN_H
  pFunc=dlsym(hLib->handle, symbol);
#else
  TString* name=reinterpret_cast<TString*>(hLib->name);
  pFunc=gSystem->DynFindSymbol(name->Data(), symbol);
#endif
  return pFunc;
}

AliHLTComponentHandler::AliHLTLibHandle* AliHLTComponentHandler::FindLibrary(const char* library)
{
  // see header file for class documentation
  AliHLTLibHandle* hLib=NULL;
  vector<AliHLTLibHandle>::iterator element=fLibraryList.begin();
  while (element!=fLibraryList.end()) {
    TString* name=reinterpret_cast<TString*>((*element).name);
    if (name->CompareTo(library)==0) {
      hLib=&(*element);
      break;
    }
    element++;
  }
  return hLib;
}

int AliHLTComponentHandler::AddStandardComponents()
{
  // see header file for class documentation
  int iResult=0;
  AliHLTComponent::SetGlobalComponentHandler(this);
//   fStandardList.push_back(new AliHLTFilePublisher);
//   fStandardList.push_back(new AliHLTFileWriter);
//   fStandardList.push_back(new AliHLTRootFilePublisherComponent);
//   fStandardList.push_back(new AliHLTRootFileWriterComponent);
  AliHLTComponent::UnsetGlobalComponentHandler();
  iResult=RegisterScheduledComponents();
  return iResult;
}

int AliHLTComponentHandler::RegisterScheduledComponents()
{
  // see header file for class documentation
  int iResult=0;
  vector<AliHLTComponent*>::iterator element=fScheduleList.begin();
  int iLocalResult=0;
  while (element!=fScheduleList.end()) {
    iLocalResult=RegisterComponent(*element);
    if (iResult==0) iResult=iLocalResult;
    fScheduleList.erase(element);
    element=fScheduleList.begin();
  }
  return iResult;
}

int AliHLTComponentHandler::DeleteStandardComponents()
{
  // see header file for class documentation
  int iResult=0;
  vector<AliHLTComponent*>::iterator element=fStandardList.begin();
  while (element!=fStandardList.end()) {
    DeregisterComponent((*element)->GetComponentID());
    delete(*element);
    fStandardList.erase(element);
    element=fStandardList.begin();
  }
  return iResult;
}

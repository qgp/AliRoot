// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          Artur Szostak <artursz@iafrica.com>                           *
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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// handler class for HLT analysis components                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#if __GNUC__== 3
using namespace std;
#endif

#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#include "AliL3StandardIncludes.h"
#include "AliHLTComponentHandler.h"
#include "AliHLTComponent.h"
#include "AliHLTDataTypes.h"
#include "AliHLTSystem.h"

ClassImp(AliHLTComponentHandler)

AliHLTComponentHandler::AliHLTComponentHandler()
{
}


AliHLTComponentHandler::~AliHLTComponentHandler()
{
  UnloadLibraries();
}

Int_t AliHLTComponentHandler::RegisterComponent(AliHLTComponent* pSample)
{
  Int_t iResult=0;
  if (pSample) {
    if (FindComponent(pSample->GetComponentID())==NULL) {
      iResult=InsertComponent(pSample);
      if (iResult>=0) {
	HLTInfo("component %s registered", pSample->GetComponentID());
      }
    } else {
      // component already registered
      HLTInfo("component %s already registered, skipped", pSample->GetComponentID());
      iResult=-EEXIST;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::DeregisterComponent( const char* componentID )
{
  return 0;
}

Int_t AliHLTComponentHandler::ScheduleRegister(AliHLTComponent* pSample)
{
  Int_t iResult=0;
  if (pSample) {
    fScheduleList.push_back(pSample);
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::CreateComponent(const Char_t* componentID, void* environ_param, int argc, const char** argv, AliHLTComponent*& component )
{
  int iResult=0;
  if (componentID) {
    AliHLTComponent* pSample=FindComponent(componentID);
    if (pSample!=NULL) {
      component=pSample->Spawn();
      if (component) {
	HLTDebug("component \"%s\" created (%p)", componentID, component);
	component->Init(&fEnvironment, environ_param, argc, argv);
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

Int_t AliHLTComponentHandler::FindComponentIndex(const Char_t* componentID)
{
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

AliHLTComponent* AliHLTComponentHandler::FindComponent(const Char_t* componentID)
{
  AliHLTComponent* pSample=NULL;
  Int_t index=FindComponentIndex(componentID);
  if (index>=0) {
    pSample=(AliHLTComponent*)fComponentList.at(index);
  }
  return pSample;
}

Int_t AliHLTComponentHandler::InsertComponent(AliHLTComponent* pSample)
{
  Int_t iResult=0;
  if (pSample!=NULL) {
    fComponentList.push_back(pSample);
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

void AliHLTComponentHandler::List() {
  vector<AliHLTComponent*>::iterator element=fComponentList.begin();
  int index=0;
  while (element!=fComponentList.end()) {
    HLTInfo("%d. %s", index++, (*element++)->GetComponentID());
  }
}

void AliHLTComponentHandler::SetEnvironment(AliHLTComponentEnvironment* pEnv) {
  if (pEnv) {
    memcpy(&fEnvironment, pEnv, sizeof(AliHLTComponentEnvironment));
    AliHLTLogging::Init(fEnvironment.fLoggingFunc);
  }
}

int AliHLTComponentHandler::LoadLibrary( const char* libraryPath )
{
  int iResult=0;
  if (libraryPath) {
    AliHLTComponent::SetGlobalComponentHandler(this);
    AliHLTLibHandle hLib=dlopen(libraryPath, RTLD_NOW);
    if (hLib) {
      AliHLTComponent::UnsetGlobalComponentHandler();
      HLTDebug("library %s loaded", libraryPath);
      fLibraryList.push_back(hLib);
      vector<AliHLTComponent*>::iterator element=fScheduleList.begin();
      int iSize=fScheduleList.size();
      int iLocalResult=0;
      while (iSize-- > 0) {
	element=fScheduleList.begin();
	iLocalResult=RegisterComponent(*element);
	if (iResult==0) iResult=iLocalResult;
	fScheduleList.erase(element);
      }
    } else {
      HLTError("can not load library %s", libraryPath);
      HLTError("dlopen error: %s", dlerror());
      iResult=-ELIBACC;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponentHandler::UnloadLibrary( const char* libraryPath )
{
  int iResult=0;
  return iResult;
}

int AliHLTComponentHandler::UnloadLibraries()
{
  int iResult=0;
  vector<AliHLTLibHandle>::iterator element=fLibraryList.begin();
  while (element!=fLibraryList.end()) {
    dlclose(*element);
    element++;
  }
  return iResult;
}

// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

//  @file   AliHLTDimServer.cxx
//  @author Matthias Richter
//  @date   2010-03-10
//  @brief  HLT DIM server implementation and dynamic access
//          to DIM library

#include "AliHLTDimServer.h"
#include "TObjArray.h"
#include "TSystem.h"
#include "TThread.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTDimServer)

AliHLTDimServer::AliHLTDimServer()
  : TNamed()
  , fServices()
  , fState(kStateOff)
  , fpServerThread(NULL)
  , fUpdatePeriod(10000)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTDimServer::AliHLTDimServer(const char* servername)
  : TNamed(servername, "AliHLTDimServer")
  , fServices()
  , fState(kStateOff)
  , fpServerThread(NULL)
  , fUpdatePeriod(10000)
{
  // see header file for class documentation
}

AliHLTDimServer::AliHLTDimInterface* AliHLTDimServer::fgpInterface=NULL;

AliHLTDimServer::~AliHLTDimServer()
{
  // see header file for class documentation
}

int AliHLTDimServer::RegisterService(AliHLTDimService* pService)
{
  // see header file for class documentation
  if (!pService) return -EINVAL;
  if (fServices.FindObject(pService)) {
    AliHLTLogging log;
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "RegisterService" , __FILE__ , __LINE__ , "duplicate service name %s, skip ...", pService->GetName());
    return -EEXIST;
  }
  fServices.Add(pService);
  return 0;
}

AliHLTDimServer::AliHLTDimService* AliHLTDimServer::CreateService(enum AliHLTDimServer::AliHLTDimServiceDataType type, const char* name)
{
  // see header file for class documentation
  AliHLTDimService* service=new AliHLTDimService(type, name);
  int iResult=RegisterService(service);
  if (iResult<0) {
    AliHLTLogging log;
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "CreateService" , __FILE__ , __LINE__ , "failed to register service %s: %d", name, iResult);
    if (service) delete service;
    return NULL;
  }
  return service;
}

TObjArray* AliHLTDimServer::CreateServiceGroup(enum AliHLTDimServer::AliHLTDimServiceDataType type, const char* basename, int count)
{
  // see header file for class documentation
  int iResult=0;
  TObjArray* pServices=new TObjArray;
  AliHLTLogging log;
  if (pServices) {
    if (basename && count>0) {
      int digits=1;
      int i=count;
      while ((i/=10)>0) digits++;
      if (digits<9) {
	log.LoggingVarargs(kHLTLogDebug, "AliHLTDimServer", "CreateServiceGroup" , __FILE__ , __LINE__ , "basename=%s count=%d digits=%d\n", basename, count, digits);
	// length of name is at max strlen(basename) + 1 '_' + digits + 1 terminating zero
	// length of format is independent of number of digits
	// at max we need strlen(basename) + 5 chars + 1 terminating zero
	int namelen=strlen(basename)+5+digits;
	char* name=(char*)malloc(namelen);
	char* format=(char*)malloc(namelen);
	if (name && format) {
	  memset(name, 0, namelen);
	  memset(format, 0, namelen);
	  const char* key=strchr(basename, '%');
	  strncpy(format, basename, namelen-1);
	  if (key && key[1]!=0) {
	    int iPos=(key-basename)+1;
	    if (key[1]=='d') {
	      snprintf(format+iPos, namelen-iPos, "0*d"); // additional 3 chars
	      iPos+=3;
	    } else {
	      *(format+iPos++)='%';
	      *(format+iPos++)=key[1];
	    }
	    strncpy(format+iPos, &key[2], namelen-1-iPos);
	  } else {
	    snprintf(format+strlen(basename), namelen-strlen(basename), "_%%0*d"); // additional 5 chars
	  }
	  for (i=0; i<count && iResult>=0; i++) {
	    snprintf(name, namelen, format, digits, i);
	    AliHLTDimService* service=new AliHLTDimService(type, name);
	    iResult=RegisterService(service);
	  }
	} else {
	  iResult=-ENOMEM;
	}
	if (name) free(name);
	if (format) free(format);
      }
    }
  }
  return pServices;
}

int AliHLTDimServer::UpdateServices()
{
  /// Update all services via the Dim channel
  return 0;
}

AliHLTDimServer::AliHLTDimInterface* AliHLTDimServer::Interface()
{
  // get instance of the interface
  if (!fgpInterface) {
    fgpInterface=new AliHLTDimInterface;
    if (fgpInterface) {
      if (fgpInterface->Init() != 0) {
        delete fgpInterface;
        fgpInterface = NULL;
      }
    }
  }
  return fgpInterface;
}

int AliHLTDimServer::Init(const char* dimNameServer)
{
  // init the dim server, check if the interface is available and set the
  // DIM DNS node name
  AliHLTLogging log;
  const char* myname=GetName();
  if (myname==NULL || myname[0]==0) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Init" , __FILE__ , __LINE__ , "can not start without a server name, skipping initialization ...");
    return -EINVAL;
  }

  if (Interface()==NULL) return -ENODEV;

  Interface()->DisSetDnsNode(dimNameServer);
  return 0;
}

int AliHLTDimServer::Reset()
{
  // reset the DIM server, functionality needs to be clarified
  return 0;
}

int AliHLTDimServer::Start()
{
  // start the server in a separate thread
  int iResult=0;
  AliHLTLogging log;
  if (GetState()!=kStateOff) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Start" , __FILE__ , __LINE__ , "server is not off, currently in state %d", GetState());
    return -ENOENT;
  }
  fpServerThread=new TThread(ServerLoop, (void*)this);
  if (!fpServerThread) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Start" , __FILE__ , __LINE__ , "unable to create server thread");
    return -ENOMEM;
  }
  SetState(kStateStarting);
  fpServerThread->Run();
  
  int count=0;
  const int maxcount=100;
  const int maxsleepms=1000;
  while (GetState()==kStateStarting) {
    if (count++==maxcount) break;
    gSystem->Sleep(maxsleepms/maxcount);
  }

  if (GetState()!=kStateRunning) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Start" , __FILE__ , __LINE__ , "could not start server, currently in state %d", GetState());
    Reset();
    iResult=-EFAULT;
  }
  return iResult;
}

int AliHLTDimServer::Stop() 
{
  // stop the server thread
  int iResult=0;
  AliHLTLogging log;
  if (GetState()!=kStateRunning) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Stop" , __FILE__ , __LINE__ , "server is not running, currently in state %d", GetState());
    return -ENOENT;
  }
  SetState(kStateStopping);
  int count=0;
  const int maxcount=100;
  const int maxsleepms=2*fUpdatePeriod;
  while (GetState()==kStateStopping) {
    if (count++==maxcount) break;
    gSystem->Sleep(maxsleepms/maxcount);
  }

  if (GetState()!=kStateOff) {
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "Stop" , __FILE__ , __LINE__ , "could not stop server, currently in state %d", GetState());
    Reset();
    iResult=-EFAULT;
  }
  return iResult;
}

void* AliHLTDimServer::ServerLoop(void* param)
{
  // see header file for class documentation
  if (!param) return (void*)0;
  AliHLTDimServer* server=reinterpret_cast<AliHLTDimServer*>(param);
  return server->ServerLoop();
}

void* AliHLTDimServer::ServerLoop()
{
  // see header file for class documentation
  if (!Interface()) return (void*)-ENODEV;

  AliHLTLogging log;
  TIter next(&fServices);
  TObject* obj=NULL;
  while ((obj=next())!=NULL) {
    AliHLTDimService* pService=dynamic_cast<AliHLTDimService*>(obj);
    if (!pService) continue;
    TString name=GetName(); name+="_"; name+=pService->GetName();
    const char* type="";
    void* buffer=pService->GetLocation();
    int size=0;
    switch (pService->GetType()) {
    case kDataTypeCustom:
    case kDataTypeInt:
    case kDataTypeFloat:
      type=pService->GetTypeString();
      size=pService->GetDataSize();
      break;
    case kDataTypeString:
      log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "ServerLoop" , __FILE__ , __LINE__ , "ignoring dim service %s: type 'string' not yet implemented", name.Data());
      break;
    default:
      log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "ServerLoop" , __FILE__ , __LINE__ , "ignoring dim service %s: unknown type %d", name.Data(), pService->GetType());
    }
    if (type[0]!=0) {
      int id=Interface()->DisAddService(name.Data(), type, buffer, size);
      if (id<0) {
	log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "ServerLoop" , __FILE__ , __LINE__ , "failed to add dim service %s: error %d", name.Data(), id);
      } else {
	 pService->SetId(id);
      }
    }
  }

  SetState(kStateRunning);
  Interface()->DisStartServing(GetName());
  while (GetState()==kStateRunning) {
    gSystem->Sleep(fUpdatePeriod);
  }

  if (GetState()!=kStateStopping) return (void*)0;

  // cleanup
  Interface()->DisStopServing();
  next.Reset();
  while ((obj=next())!=NULL) {
    const AliHLTDimService* pService=dynamic_cast<const AliHLTDimService*>(obj);
    if (!pService || pService->GetId()<0) continue;
    // int iResult=Interface()->DisRemoveService(pService->GetId());
    // if (iResult<0) {
    //   log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "ServerLoop" , __FILE__ , __LINE__ , "failed to remove dim service %s: error %d", pService->GetName(), iResult);
    // }
  }

  SetState(kStateOff);

  return (void*)0;
}

AliHLTDimServer::AliHLTDimService::AliHLTDimService()
  : TNamed()
  , fData()
  , fType(kDataTypeUnknown)
  , fTypeString()
  , fDataBuffer(NULL)
  , fDataSize(0)
  , fId(-1)
{
  // see header file for class documentation
}

AliHLTDimServer::AliHLTDimService::AliHLTDimService(enum AliHLTDimServiceDataType type, const char* servicename)
  : TNamed(servicename, "AliHLTDimService")
  , fData()
  , fType(type)
  , fTypeString()
  , fDataBuffer(NULL)
  , fDataSize(0)
  , fId(-1)
{
  // see header file for class documentation
  AliHLTLogging log;
  switch (fType) {
  case kDataTypeCustom:
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "AliHLTDimService" , __FILE__ , __LINE__ , "cannot use the kDataTypeCustom type with this method");
    break;
  case kDataTypeInt:
    fTypeString="I";
    fDataBuffer=&fData.iVal;
    fDataSize=sizeof(int);
    break;
  case kDataTypeFloat:
    fTypeString="F";
    fDataBuffer=&fData.fVal;
    fDataSize=sizeof(float);
    break;
  case kDataTypeString:
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "AliHLTDimService" , __FILE__ , __LINE__ , "dim service type 'string' not yet implemented");
    break;
  default:
    log.LoggingVarargs(kHLTLogError, "AliHLTDimServer", "AliHLTDimService" , __FILE__ , __LINE__ , "Unknown dim service type %d", fType);
  };
}

AliHLTDimServer::AliHLTDimService::AliHLTDimService(const char* type, void* data, int size, const char* servicename)
  : TNamed(servicename, "AliHLTDimService")
  , fData()
  , fType(kDataTypeCustom)
  , fTypeString(type)
  , fDataBuffer(data)
  , fDataSize(size)
  , fId(-1)
{
  // see header file for class documentation
}

void AliHLTDimServer::AliHLTDimService::Update()
{
  // see header file for class documentation
  static bool bWarning=true;
  AliHLTLogging log;
  switch (fType) {
  case kDataTypeCustom: break;
  default:
    if (bWarning) log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "Update" , __FILE__ , __LINE__ , "The service %s must be a custom type (kDataTypeCustom) to use this form of Update", GetName());
    bWarning=false;
  };

  AliHLTDimServer::Interface()->DisUpdateService(fId);
}

void AliHLTDimServer::AliHLTDimService::Update(const AliHLTDimServicePoint_t& sp)
{
  // see header file for class documentation
  static bool bWarning=true;
  AliHLTLogging log;
  switch (fType) {
  case kDataTypeCustom:
    if (bWarning) log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "Update" , __FILE__ , __LINE__ , "Should not call this form of Update for custom type (kDataTypeCustom) service %s", GetName());
    bWarning=false;
    return;
  case kDataTypeInt: fData.iVal=sp.iVal; break;
  case kDataTypeFloat: fData.fVal=sp.fVal; break;
  case kDataTypeString:
    if (bWarning) log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "Update" , __FILE__ , __LINE__ , "Failed to update dim service %s: 'string' not yet implemented", GetName());
    bWarning=false;
    break;
  default:
    if (bWarning) log.LoggingVarargs(kHLTLogError, "AliHLTDimServer::AliHLTDimService", "Update" , __FILE__ , __LINE__ , "Failed to update dim service %s: unknown type %d", GetName(), fType);
    bWarning=false;
  };

  AliHLTDimServer::Interface()->DisUpdateService(fId);
}

AliHLTDimServer::AliHLTDimInterface::AliHLTDimInterface()
  : AliHLTLogging()
  , fpDisAddService(NULL)
  , fpDisRemoveService(NULL)
  , fpDisUpdateService(NULL)
  , fpDisStartServing(NULL)
  , fpDisStopServing(NULL)
  , fpDisSetDnsNode(NULL)
{
}

AliHLTDimServer::AliHLTDimInterface::~AliHLTDimInterface()
{
}


const char* AliHLTDimServer::AliHLTDimInterface::fgkDimLibraryName="libdim.so";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisAddServiceSymbol="dis_add_service";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisRemoveServiceSymbol="dis_remove_service";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisUpdateServiceSymbol="dis_update_service";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisStartServingSymbol="dis_start_serving";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisStopServingSymbol="dis_stop_serving";
const char* AliHLTDimServer::AliHLTDimInterface::fgkDisSetDnsNodeSymbol="dis_set_dns_node";

int AliHLTDimServer::AliHLTDimInterface::Init()
{
  /// load the dim library and function pointers
  if (gSystem->Load(fgkDimLibraryName)) {
    HLTFatal("failed to load dim library: %s", fgkDimLibraryName);
    return -ENODEV;
  }

  fpDisAddService=(fctDisAddService)FindSymbol(fgkDimLibraryName, fgkDisAddServiceSymbol);
  if (!fpDisAddService) return -ENODEV;

  fpDisRemoveService=(fctDisRemoveService)FindSymbol(fgkDimLibraryName, fgkDisRemoveServiceSymbol);
  if (!fpDisRemoveService) return -ENODEV;

  fpDisUpdateService=(fctDisUpdateService)FindSymbol(fgkDimLibraryName, fgkDisUpdateServiceSymbol);
  if (!fpDisUpdateService) return -ENODEV;

  fpDisStartServing=(fctDisCharArg)FindSymbol(fgkDimLibraryName, fgkDisStartServingSymbol);
  if (!fpDisStartServing) return -ENODEV;

  fpDisStopServing=(fctDisNoArg)FindSymbol(fgkDimLibraryName, fgkDisStopServingSymbol);
  if (!fpDisStopServing) return -ENODEV;

  fpDisSetDnsNode=(fctDisCharArg)FindSymbol(fgkDimLibraryName, fgkDisSetDnsNodeSymbol);
  if (!fpDisSetDnsNode) return -ENODEV;

  return 0;
}

AliHLTDimServer::fctVoid AliHLTDimServer::AliHLTDimInterface::FindSymbol(const char* library, const char* symbol) const
{
  /// Find symbol in the dim library
  TString tmp=symbol;
  fctVoid fctptr=gSystem->DynFindSymbol(library, tmp.Data());
  if (!fctptr) {
    // do a 2nd try with appended '_'
    tmp+="_";
    fctptr=gSystem->DynFindSymbol(library, tmp.Data());
  }
  if (fctptr) return fctptr;

  HLTError("can not find symbol '%s' in %s", symbol, library);
  return NULL;
}

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

/** @file   AliHLTComponent.cxx
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  Base class implementation for HLT components. */

#if __GNUC__>= 3
using namespace std;
#endif

//#include "AliHLTStdIncludes.h"
#include "AliHLTComponent.h"
#include "AliHLTComponentHandler.h"
#include "AliHLTMessage.h"
#include "TString.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TStopwatch.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTComponent);

/** stopwatch macro using the stopwatch guard */
#define ALIHLTCOMPONENT_STOPWATCH(type) AliHLTStopwatchGuard swguard(fpStopwatches!=NULL?reinterpret_cast<TStopwatch*>(fpStopwatches->At((int)type)):NULL)
//#define ALIHLTCOMPONENT_STOPWATCH(type) 

/** stopwatch macro for operations of the base class */
#define ALIHLTCOMPONENT_BASE_STOPWATCH() ALIHLTCOMPONENT_STOPWATCH(kSWBase)
/** stopwatch macro for operations of the detector algorithm (DA) */
#define ALIHLTCOMPONENT_DA_STOPWATCH() ALIHLTCOMPONENT_STOPWATCH(kSWDA)

AliHLTComponent::AliHLTComponent()
  :
  fEnvironment(),
  fCurrentEvent(0),
  fEventCount(-1),
  fFailedEvents(0),
  fCurrentEventData(),
  fpInputBlocks(NULL),
  fCurrentInputBlock(-1),
  fSearchDataType(kAliHLTVoidDataType),
  fClassName(),
  fpInputObjects(NULL),
  fpOutputBuffer(NULL),
  fOutputBufferSize(0),
  fOutputBufferFilled(0),
  fOutputBlocks(),
  fpStopwatches(new TObjArray(kSWTypeCount))
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
  memset(&fEnvironment, 0, sizeof(AliHLTComponentEnvironment));
  if (fgpComponentHandler)
    fgpComponentHandler->ScheduleRegister(this);
  SetLocalLoggingLevel(kHLTLogDefault);
}

AliHLTComponent::AliHLTComponent(const AliHLTComponent&)
  :
  AliHLTLogging(),
  fEnvironment(),
  fCurrentEvent(0),
  fEventCount(-1),
  fFailedEvents(0),
  fCurrentEventData(),
  fpInputBlocks(NULL),
  fCurrentInputBlock(-1),
  fSearchDataType(kAliHLTVoidDataType),
  fClassName(),
  fpInputObjects(NULL),
  fpOutputBuffer(NULL),
  fOutputBufferSize(0),
  fOutputBufferFilled(0),
  fOutputBlocks(),
  fpStopwatches(NULL)
{
  // see header file for class documentation
  HLTFatal("copy constructor untested");
}

AliHLTComponent& AliHLTComponent::operator=(const AliHLTComponent&)
{ 
  // see header file for class documentation
  HLTFatal("assignment operator untested");
  return *this;
}

AliHLTComponent::~AliHLTComponent()
{
  // see header file for function documentation
  CleanupInputObjects();
}

AliHLTComponentHandler* AliHLTComponent::fgpComponentHandler=NULL;

int AliHLTComponent::SetGlobalComponentHandler(AliHLTComponentHandler* pCH, int bOverwrite) 
{
  // see header file for function documentation
  int iResult=0;
  if (fgpComponentHandler==NULL || bOverwrite!=0)
    fgpComponentHandler=pCH;
  else
    iResult=-EPERM;
  return iResult;
}

int AliHLTComponent::UnsetGlobalComponentHandler() 
{
  // see header file for function documentation
  return SetGlobalComponentHandler(NULL,1);
}

int AliHLTComponent::Init( AliHLTComponentEnvironment* environ, void* environParam, int argc, const char** argv )
{
  // see header file for function documentation
  int iResult=0;
  if (environ) {
    memcpy(&fEnvironment, environ, sizeof(AliHLTComponentEnvironment));
    fEnvironment.fParam=environParam;
  }
  const char** pArguments=NULL;
  int iNofChildArgs=0;
  TString argument="";
  int bMissingParam=0;
  if (argc>0) {
    pArguments=new const char*[argc];
    if (pArguments) {
      for (int i=0; i<argc && iResult>=0; i++) {
	argument=argv[i];
	if (argument.IsNull()) continue;

	// benchmark
	if (argument.CompareTo("benchmark")==0) {

	  // loglevel
	} else if (argument.CompareTo("loglevel")==0) {
	  if ((bMissingParam=(++i>=argc))) break;
	  TString parameter(argv[i]);
	  parameter.Remove(TString::kLeading, ' '); // remove all blanks
	  if (parameter.BeginsWith("0x") &&
	      parameter.Replace(0,2,"",0).IsHex()) {
	    AliHLTComponentLogSeverity loglevel=kHLTLogNone;
	    sscanf(parameter.Data(),"%x", (unsigned int*)&loglevel);
	    SetLocalLoggingLevel(loglevel);
	  } else {
	    HLTError("wrong parameter for argument %s, hex number expected", argument.Data());
	    iResult=-EINVAL;
	  }
	} else {
	  pArguments[iNofChildArgs++]=argv[i];
	}
      }
    } else {
      iResult=-ENOMEM;
    }
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }
  if (iResult>=0) {
    iResult=DoInit(iNofChildArgs, pArguments);
  }
  if (iResult>=0) fEventCount=0;
  if (pArguments) delete [] pArguments;
  return iResult;
}

int AliHLTComponent::Deinit()
{
  // see header file for function documentation
  int iResult=0;
  iResult=DoDeinit();
  return iResult;
}

int AliHLTComponent::DoInit( int argc, const char** argv )
{
  // see header file for function documentation
  if (argc==0 && argv==NULL) {
    // this is currently just to get rid of the warning "unused parameter"
  }
  fEventCount=0;
  return 0;
}

int AliHLTComponent::DoDeinit()
{
  // see header file for function documentation
  fEventCount=0;
  return 0;
}

void AliHLTComponent::DataType2Text( const AliHLTComponentDataType& type, char output[kAliHLTComponentDataTypefIDsize+kAliHLTComponentDataTypefOriginSize+2] ) const
{
  // see header file for function documentation
  memset( output, 0, kAliHLTComponentDataTypefIDsize+kAliHLTComponentDataTypefOriginSize+2 );
  strncat( output, type.fOrigin, kAliHLTComponentDataTypefOriginSize );
  strcat( output, ":" );
  strncat( output, type.fID, kAliHLTComponentDataTypefIDsize );
}

string AliHLTComponent::DataType2Text( const AliHLTComponentDataType& type )
{
  // see header file for function documentation
  string out("");
  
  if (type==kAliHLTVoidDataType) {
    out="VOID:VOID";
  } else {
    // some gymnastics in order to avoid a '0' which is part of either or both
    // ID and origin terminating the whole string. Unfortunately, string doesn't
    // stop appending at the '0' if the number of elements to append was 
    // explicitely specified
    string tmp("");
    tmp.append(type.fOrigin, kAliHLTComponentDataTypefOriginSize);
    out.append(tmp.c_str());
    out.append(":");
    tmp="";
    tmp.append(type.fID, kAliHLTComponentDataTypefIDsize);
    out.append(tmp.c_str());
  }
  return out;
}


void* AliHLTComponent::AllocMemory( unsigned long size ) 
{
  // see header file for function documentation
  if (fEnvironment.fAllocMemoryFunc)
    return (*fEnvironment.fAllocMemoryFunc)(fEnvironment.fParam, size );
  HLTFatal("no memory allocation handler registered");
  return NULL;
}

int AliHLTComponent::MakeOutputDataBlockList( const vector<AliHLTComponentBlockData>& blocks, AliHLTUInt32_t* blockCount,
					      AliHLTComponentBlockData** outputBlocks ) 
{
  // see header file for function documentation
    if ( blockCount==NULL || outputBlocks==NULL )
	return -EFAULT;
    AliHLTUInt32_t count = blocks.size();
    if ( !count )
	{
	*blockCount = 0;
	*outputBlocks = NULL;
	return 0;
	}
    *outputBlocks = reinterpret_cast<AliHLTComponentBlockData*>( AllocMemory( sizeof(AliHLTComponentBlockData)*count ) );
    if ( !*outputBlocks )
	return -ENOMEM;
    for ( unsigned long i = 0; i < count; i++ ) {
	(*outputBlocks)[i] = blocks[i];
	if (blocks[i].fDataType==kAliHLTAnyDataType) {
	  (*outputBlocks)[i].fDataType=GetOutputDataType();
	  /* data type was set to the output data type by the PubSub AliRoot
	     Wrapper component, if data type of the block was ********:****.
	     Now handled by the component base class in order to have same
	     behavior when running embedded in AliRoot
	  memset((*outputBlocks)[i].fDataType.fID, '*', kAliHLTComponentDataTypefIDsize);
	  memset((*outputBlocks)[i].fDataType.fOrigin, '*', kAliHLTComponentDataTypefOriginSize);
	  */
	}
    }
    *blockCount = count;
    return 0;

}

int AliHLTComponent::GetEventDoneData( unsigned long size, AliHLTComponentEventDoneData** edd ) 
{
  // see header file for function documentation
  if (fEnvironment.fGetEventDoneDataFunc)
    return (*fEnvironment.fGetEventDoneDataFunc)(fEnvironment.fParam, fCurrentEvent, size, edd );
  return -ENOSYS;
}

int AliHLTComponent::FindMatchingDataTypes(AliHLTComponent* pConsumer, vector<AliHLTComponentDataType>* tgtList) 
{
  // see header file for function documentation
  int iResult=0;
  if (pConsumer) {
    vector<AliHLTComponentDataType> ctlist;
    ((AliHLTComponent*)pConsumer)->GetInputDataTypes(ctlist);
    vector<AliHLTComponentDataType>::iterator type=ctlist.begin();
    //AliHLTComponentDataType ouptdt=GetOutputDataType();
    //PrintDataTypeContent(ouptdt, "publisher \'%s\'");
    while (type!=ctlist.end() && iResult==0) {
      //PrintDataTypeContent((*type), "consumer \'%s\'");
      if ((*type)==GetOutputDataType() ||
	  (*type)==kAliHLTAnyDataType) {
	if (tgtList) tgtList->push_back(*type);
	iResult++;
	// this loop has to be changed in case of multiple output types
	break;
      }
      type++;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

void AliHLTComponent::PrintDataTypeContent(AliHLTComponentDataType& dt, const char* format) const
{
  // see header file for function documentation
  const char* fmt="publisher \'%s\'";
  if (format) fmt=format;
  HLTMessage(fmt, (DataType2Text(dt)).c_str());
  HLTMessage("%x %x %x %x %x %x %x %x : %x %x %x %x", 
	     dt.fID[0],
	     dt.fID[1],
	     dt.fID[2],
	     dt.fID[3],
	     dt.fID[4],
	     dt.fID[5],
	     dt.fID[6],
	     dt.fID[7],
	     dt.fOrigin[0],
	     dt.fOrigin[1],
	     dt.fOrigin[2],
	     dt.fOrigin[3]);
}

void AliHLTComponent::FillBlockData( AliHLTComponentBlockData& blockData ) const
{
  // see header file for function documentation
  blockData.fStructSize = sizeof(blockData);
  FillShmData( blockData.fShmKey );
  blockData.fOffset = ~(AliHLTUInt32_t)0;
  blockData.fPtr = NULL;
  blockData.fSize = 0;
  FillDataType( blockData.fDataType );
  blockData.fSpecification = kAliHLTVoidDataSpec;
}

void AliHLTComponent::FillShmData( AliHLTComponentShmData& shmData ) const
{
  // see header file for function documentation
  shmData.fStructSize = sizeof(shmData);
  shmData.fShmType = gkAliHLTComponentInvalidShmType;
  shmData.fShmID = gkAliHLTComponentInvalidShmID;
}

void AliHLTComponent::FillDataType( AliHLTComponentDataType& dataType ) const
{
  // see header file for function documentation
  dataType=kAliHLTAnyDataType;
}

void AliHLTComponent::CopyDataType(AliHLTComponentDataType& tgtdt, const AliHLTComponentDataType& srcdt) 
{
  // see header file for function documentation
  memcpy(&tgtdt.fID[0], &srcdt.fID[0], kAliHLTComponentDataTypefIDsize);
  memcpy(&tgtdt.fOrigin[0], &srcdt.fOrigin[0], kAliHLTComponentDataTypefOriginSize);
}

void AliHLTComponent::SetDataType(AliHLTComponentDataType& tgtdt, const char* id, const char* origin) 
{
  // see header file for function documentation
  tgtdt.fStructSize = sizeof(AliHLTComponentDataType);
  memset(&tgtdt.fID[0], 0, kAliHLTComponentDataTypefIDsize);
  memset(&tgtdt.fOrigin[0], 0, kAliHLTComponentDataTypefOriginSize);

  if ((int)strlen(id)>kAliHLTComponentDataTypefIDsize) {
    HLTWarning("data type id %s is too long, truncated to %d", id, kAliHLTComponentDataTypefIDsize);
  }
  strncpy(&tgtdt.fID[0], id, kAliHLTComponentDataTypefIDsize);

  if ((int)strlen(origin)>kAliHLTComponentDataTypefOriginSize) {
    HLTWarning("data type origin %s is too long, truncated to %d", origin, kAliHLTComponentDataTypefOriginSize);
  }
  strncpy(&tgtdt.fOrigin[0], origin, kAliHLTComponentDataTypefOriginSize);
}

void AliHLTComponent::FillEventData(AliHLTComponentEventData& evtData)
{
  // see header file for function documentation
  memset(&evtData, 0, sizeof(AliHLTComponentEventData));
  evtData.fStructSize=sizeof(AliHLTComponentEventData);
}

void AliHLTComponent::PrintComponentDataTypeInfo(const AliHLTComponentDataType& dt) 
{
  // see header file for function documentation
  TString msg;
  msg.Form("AliHLTComponentDataType(%d): ID=\"", dt.fStructSize);
  for ( int i = 0; i < kAliHLTComponentDataTypefIDsize; i++ ) {
   if (dt.fID[i]!=0) msg+=dt.fID[i];
   else msg+="\\0";
  }
  msg+="\" Origin=\"";
  for ( int i = 0; i < kAliHLTComponentDataTypefOriginSize; i++ ) {
   if (dt.fOrigin[i]!=0) msg+=dt.fOrigin[i];
   else msg+="\\0";
  }
  msg+="\"";
  AliHLTLogging::Message(NULL, kHLTLogNone, NULL , NULL, msg.Data());
}

int AliHLTComponent::GetEventCount() const
{
  // see header file for function documentation
  return fEventCount;
}

int AliHLTComponent::IncrementEventCounter()
{
  // see header file for function documentation
  if (fEventCount>=0) fEventCount++;
  return fEventCount;
}

int AliHLTComponent::GetNumberOfInputBlocks() const
{
  // see header file for function documentation
  if (fpInputBlocks!=NULL) {
    return fCurrentEventData.fBlockCnt;
  }
  return 0;
}

const TObject* AliHLTComponent::GetFirstInputObject(const AliHLTComponentDataType& dt,
						    const char* classname,
						    int bForce)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  fSearchDataType=dt;
  if (classname) fClassName=classname;
  else fClassName.clear();
  int idx=FindInputBlock(fSearchDataType, 0);
  HLTDebug("found block %d when searching for data type %s", idx, DataType2Text(dt).c_str());
  TObject* pObj=NULL;
  if (idx>=0) {
    if ((pObj=GetInputObject(idx, fClassName.c_str(), bForce))!=NULL) {
      fCurrentInputBlock=idx;
    } else {
    }
  }
  return pObj;
}

const TObject* AliHLTComponent::GetFirstInputObject(const char* dtID, 
						    const char* dtOrigin,
						    const char* classname,
						    int         bForce)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTComponentDataType dt;
  SetDataType(dt, dtID, dtOrigin);
  return GetFirstInputObject(dt, classname, bForce);
}

const TObject* AliHLTComponent::GetNextInputObject(int bForce)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  int idx=FindInputBlock(fSearchDataType, fCurrentInputBlock+1);
  //HLTDebug("found block %d when searching for data type %s", idx, DataType2Text(fSearchDataType).c_str());
  TObject* pObj=NULL;
  if (idx>=0) {
    if ((pObj=GetInputObject(idx, fClassName.c_str(), bForce))!=NULL) {
      fCurrentInputBlock=idx;
    }
  }
  return pObj;
}

int AliHLTComponent::FindInputBlock(const AliHLTComponentDataType& dt, int startIdx) const
{
  // see header file for function documentation
  int iResult=-ENOENT;
  if (fpInputBlocks!=NULL) {
    int idx=startIdx<0?0:startIdx;
    for ( ; (UInt_t)idx<fCurrentEventData.fBlockCnt && iResult==-ENOENT; idx++) {
      if (dt == kAliHLTAnyDataType || fpInputBlocks[idx].fDataType == dt) {
	iResult=idx;
      }
    }
  }
  return iResult;
}

TObject* AliHLTComponent::CreateInputObject(int idx, int bForce)
{
  // see header file for function documentation
  TObject* pObj=NULL;
  if (fpInputBlocks!=NULL) {
    if ((UInt_t)idx<fCurrentEventData.fBlockCnt) {
      if (fpInputBlocks[idx].fPtr) {
	AliHLTUInt32_t firstWord=*((AliHLTUInt32_t*)fpInputBlocks[idx].fPtr);
	if (firstWord==fpInputBlocks[idx].fSize-sizeof(AliHLTUInt32_t)) {
	  HLTDebug("create object from block %d size %d", idx, fpInputBlocks[idx].fSize);
	  AliHLTMessage msg(fpInputBlocks[idx].fPtr, fpInputBlocks[idx].fSize);
	  TClass* objclass=msg.GetClass();
	  pObj=msg.ReadObject(objclass);
	  if (pObj && objclass) {
	    HLTDebug("object %p type %s created", pObj, objclass->GetName());
	  } else {
	  }
	} else {
	  //	} else if (bForce!=0) {
	  HLTError("size missmatch: block size %d, indicated %d", fpInputBlocks[idx].fSize, firstWord+sizeof(AliHLTUInt32_t));
	}
      } else {
	HLTFatal("block descriptor empty");
      }
    } else {
      HLTError("index %d out of range %d", idx, fCurrentEventData.fBlockCnt);
    }
  } else {
    HLTError("no input blocks available");
  }
  
  return pObj;
}

TObject* AliHLTComponent::GetInputObject(int idx, const char* classname, int bForce)
{
  // see header file for function documentation
  if (fpInputObjects==NULL) {
    fpInputObjects=new TObjArray(fCurrentEventData.fBlockCnt);
  }
  TObject* pObj=NULL;
  if (fpInputObjects) {
    pObj=fpInputObjects->At(idx);
    if (pObj==NULL) {
      pObj=CreateInputObject(idx, bForce);
      if (pObj) {
	fpInputObjects->AddAt(pObj, idx);
      }
    }
  } else {
    HLTFatal("memory allocation failed: TObjArray of size %d", fCurrentEventData.fBlockCnt);
  }
  return pObj;
}

int AliHLTComponent::CleanupInputObjects()
{
  // see header file for function documentation
  if (!fpInputObjects) return 0;
  TObjArray* array=fpInputObjects;
  fpInputObjects=NULL;
  for (int i=0; i<array->GetEntries(); i++) {
    TObject* pObj=array->At(i);
    if (pObj) delete pObj;
  }
  delete array;
  return 0;
}

AliHLTComponentDataType AliHLTComponent::GetDataType(const TObject* pObject)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTComponentDataType dt=kAliHLTVoidDataType;
  int idx=fCurrentInputBlock;
  if (pObject) {
    if (fpInputObjects==NULL || (idx=fpInputObjects->IndexOf(pObject))>=0) {
    } else {
      HLTError("unknown object %p", pObject);
    }
  }
  if (idx>=0) {
    if ((UInt_t)idx<fCurrentEventData.fBlockCnt) {
      dt=fpInputBlocks[idx].fDataType;
    } else {
      HLTFatal("severe internal error, index out of range");
    }
  }
  return dt;
}

AliHLTUInt32_t AliHLTComponent::GetSpecification(const TObject* pObject)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTUInt32_t iSpec=kAliHLTVoidDataSpec;
  int idx=fCurrentInputBlock;
  if (pObject) {
    if (fpInputObjects==NULL || (idx=fpInputObjects->IndexOf(pObject))>=0) {
    } else {
      HLTError("unknown object %p", pObject);
    }
  }
  if (idx>=0) {
    if ((UInt_t)idx<fCurrentEventData.fBlockCnt) {
      iSpec=fpInputBlocks[idx].fSpecification;
    } else {
      HLTFatal("severe internal error, index out of range");
    }
  }
  return iSpec;
}

const AliHLTComponentBlockData* AliHLTComponent::GetFirstInputBlock(const AliHLTComponentDataType& dt)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  fSearchDataType=dt;
  fClassName.clear();
  int idx=FindInputBlock(fSearchDataType, 0);
  const AliHLTComponentBlockData* pBlock=NULL;
  if (idx>=0) {
    // check for fpInputBlocks pointer done in FindInputBlock
    pBlock=&fpInputBlocks[idx];
  }
  return pBlock;
}

const AliHLTComponentBlockData* AliHLTComponent::GetFirstInputBlock(const char* dtID, 
								    const char* dtOrigin)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTComponentDataType dt;
  SetDataType(dt, dtID, dtOrigin);
  return GetFirstInputBlock(dt);
}

const AliHLTComponentBlockData* AliHLTComponent::GetNextInputBlock()
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  int idx=FindInputBlock(fSearchDataType, fCurrentInputBlock+1);
  const AliHLTComponentBlockData* pBlock=NULL;
  if (idx>=0) {
    // check for fpInputBlocks pointer done in FindInputBlock
    pBlock=&fpInputBlocks[idx];
  }
  return pBlock;
}

int AliHLTComponent::FindInputBlock(const AliHLTComponentBlockData* pBlock) const
{
  // see header file for function documentation
  int iResult=-ENOENT;
  if (fpInputBlocks!=NULL) {
    if (pBlock) {
      if (pBlock>=fpInputBlocks && pBlock<fpInputBlocks+fCurrentEventData.fBlockCnt) {
	iResult=(int)(pBlock-fpInputBlocks);
      }
    } else {
      iResult=-EINVAL;
    }
  }
  return iResult;
}

AliHLTUInt32_t AliHLTComponent::GetSpecification(const AliHLTComponentBlockData* pBlock)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTUInt32_t iSpec=kAliHLTVoidDataSpec;
  int idx=fCurrentInputBlock;
  if (pBlock) {
    if (fpInputObjects==NULL || (idx=FindInputBlock(pBlock))>=0) {
    } else {
      HLTError("unknown Block %p", pBlock);
    }
  }
  if (idx>=0) {
    // check for fpInputBlocks pointer done in FindInputBlock
    iSpec=fpInputBlocks[idx].fSpecification;
  }
  return iSpec;
}

int AliHLTComponent::PushBack(TObject* pObject, const AliHLTComponentDataType& dt, AliHLTUInt32_t spec)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  int iResult=0;
  if (pObject) {
    AliHLTMessage msg(kMESS_OBJECT);
    msg.WriteObject(pObject);
    Int_t iMsgLength=msg.Length();
    if (iMsgLength>0) {
      msg.SetLength(); // sets the length to the first (reserved) word
      iResult=InsertOutputBlock(msg.Buffer(), iMsgLength, dt, spec);
      if (iResult>=0) {
	HLTDebug("object %s (%p) size %d inserted to output", pObject->ClassName(), pObject, iMsgLength);
      }
    } else {
      HLTError("object serialization failed for object %p", pObject);
      iResult=-ENOMSG;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponent::PushBack(TObject* pObject, const char* dtID, const char* dtOrigin, AliHLTUInt32_t spec)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTComponentDataType dt;
  SetDataType(dt, dtID, dtOrigin);
  return PushBack(pObject, dt, spec);
}

int AliHLTComponent::PushBack(void* pBuffer, int iSize, const AliHLTComponentDataType& dt, AliHLTUInt32_t spec)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  return InsertOutputBlock(pBuffer, iSize, dt, spec);
}

int AliHLTComponent::PushBack(void* pBuffer, int iSize, const char* dtID, const char* dtOrigin, AliHLTUInt32_t spec)
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  AliHLTComponentDataType dt;
  SetDataType(dt, dtID, dtOrigin);
  return PushBack(pBuffer, iSize, dt, spec);
}

int AliHLTComponent::InsertOutputBlock(void* pBuffer, int iSize, const AliHLTComponentDataType& dt, AliHLTUInt32_t spec)
{
  // see header file for function documentation
  int iResult=0;
  if (pBuffer) {
    if (fpOutputBuffer && iSize<=(int)(fOutputBufferSize-fOutputBufferFilled)) {
      AliHLTUInt8_t* pTgt=fpOutputBuffer+fOutputBufferFilled;
      AliHLTComponentBlockData bd;
      FillBlockData( bd );
      bd.fOffset        = fOutputBufferFilled;
      bd.fPtr           = pTgt;
      bd.fSize          = iSize;
      bd.fDataType      = dt;
      bd.fSpecification = spec;
      if (pBuffer!=NULL && pBuffer!=pTgt) {
	memcpy(pTgt, pBuffer, iSize);
	//AliHLTUInt32_t firstWord=*((AliHLTUInt32_t*)pBuffer);	
	//HLTDebug("copy %d bytes from %p to output buffer %p, first word %#x", iSize, pBuffer, pTgt, firstWord);
      }
      fOutputBufferFilled+=bd.fSize;
      fOutputBlocks.push_back( bd );
      //HLTDebug("buffer inserted to output: size %d data type %s spec %#x", iSize, DataType2Text(dt).c_str(), spec);
    } else {
      if (fpOutputBuffer) {
	HLTError("too little space in output buffer: %d, required %d", fOutputBufferSize-fOutputBufferFilled, iSize);
      } else {
	HLTError("output buffer not available");
      }
      iResult=-ENOSPC;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponent::EstimateObjectSize(TObject* pObject) const
{
  // see header file for function documentation
  if (!pObject) return -EINVAL;
    AliHLTMessage msg(kMESS_OBJECT);
    msg.WriteObject(pObject);
    return msg.Length();  
}

int AliHLTComponent::CreateEventDoneData(AliHLTComponentEventDoneData edd)
{
  // see header file for function documentation
  int iResult=-ENOSYS;
  //#warning  function not yet implemented
  HLTWarning("function not yet implemented");
  return iResult;
}

int AliHLTComponent::ProcessEvent( const AliHLTComponentEventData& evtData,
				   const AliHLTComponentBlockData* blocks, 
				   AliHLTComponentTriggerData& trigData,
				   AliHLTUInt8_t* outputPtr, 
				   AliHLTUInt32_t& size,
				   AliHLTUInt32_t& outputBlockCnt, 
				   AliHLTComponentBlockData*& outputBlocks,
				   AliHLTComponentEventDoneData*& edd )
{
  // see header file for function documentation
  ALIHLTCOMPONENT_BASE_STOPWATCH();
  int iResult=0;
  fCurrentEvent=evtData.fEventID;
  fCurrentEventData=evtData;
  fpInputBlocks=blocks;
  fCurrentInputBlock=-1;
  fSearchDataType=kAliHLTAnyDataType;
  fpOutputBuffer=outputPtr;
  fOutputBufferSize=size;
  fOutputBufferFilled=0;
  fOutputBlocks.clear();
  
  vector<AliHLTComponentBlockData> blockData;
  { // dont delete, sets the scope for the stopwatch guard
    ALIHLTCOMPONENT_DA_STOPWATCH();
    iResult=DoProcessing(evtData, blocks, trigData, outputPtr, size, blockData, edd);
  } // end of the scope of the stopwatch guard
  if (iResult>=0) {
    if (fOutputBlocks.size()>0) {
      //HLTDebug("got %d block(s) via high level interface", fOutputBlocks.size());
      if (blockData.size()>0) {
	HLTError("low level and high interface must not be mixed; use PushBack methods to insert data blocks");
	iResult=-EFAULT;
      } else {
	iResult=MakeOutputDataBlockList(fOutputBlocks, &outputBlockCnt, &outputBlocks);
	size=fOutputBufferFilled;
      }
    } else {
      iResult=MakeOutputDataBlockList(blockData, &outputBlockCnt, &outputBlocks);
    }
    if (iResult<0) {
      HLTFatal("component %s (%p): can not convert output block descriptor list", GetComponentID(), this);
    }
  }
  if (iResult<0) {
    outputBlockCnt=0;
    outputBlocks=NULL;
  }
  CleanupInputObjects();
  IncrementEventCounter();
  return iResult;
}

AliHLTComponent::AliHLTStopwatchGuard::AliHLTStopwatchGuard()
  :
  fpStopwatch(NULL),
  fpPrec(NULL)
{
  // standard constructor (not for use)
}

AliHLTComponent::AliHLTStopwatchGuard::AliHLTStopwatchGuard(TStopwatch* pStopwatch)
  :
  fpStopwatch(pStopwatch),
  fpPrec(NULL)
{
  // constructor

  // check for already existing guard
  if (fgpCurrent) fpPrec=fgpCurrent;
  fgpCurrent=this;

  // stop the preceeding guard if it controls a different stopwatch
  int bStart=1;
  if (fpPrec && fpPrec!=this) bStart=fpPrec->Hold(fpStopwatch);

  // start the stopwatch if the current guard controls a different one
  if (fpStopwatch && bStart==1) fpStopwatch->Start(kFALSE);
}

AliHLTComponent::AliHLTStopwatchGuard::AliHLTStopwatchGuard(AliHLTStopwatchGuard&)
  :
  fpStopwatch(NULL),
  fpPrec(NULL)
{
  // copy constructor (not for use)
}

AliHLTComponent::AliHLTStopwatchGuard* AliHLTComponent::AliHLTStopwatchGuard::fgpCurrent=NULL;

AliHLTComponent::AliHLTStopwatchGuard::~AliHLTStopwatchGuard()
{
  // destructor

  // resume the preceeding guard if it controls a different stopwatch
  int bStop=1;
  if (fpPrec && fpPrec!=this) bStop=fpPrec->Resume(fpStopwatch);

  // stop the stopwatch if the current guard controls a different one
  if (fpStopwatch && bStop==1) fpStopwatch->Stop();

  // resume to the preceeding guard
  fgpCurrent=fpPrec;
}

int AliHLTComponent::AliHLTStopwatchGuard::Hold(TStopwatch* pSucc)
{
  // see header file for function documentation
  if (fpStopwatch!=NULL && fpStopwatch!=pSucc) fpStopwatch->Stop();
  return fpStopwatch!=pSucc?1:0;
}

int AliHLTComponent::AliHLTStopwatchGuard::Resume(TStopwatch* pSucc)
{
  // see header file for function documentation
  if (fpStopwatch!=NULL && fpStopwatch!=pSucc) fpStopwatch->Start(kFALSE);
  return fpStopwatch!=pSucc?1:0;
}

int AliHLTComponent::SetStopwatch(TObject* pSW, AliHLTStopwatchType type) 
{
  // see header file for function documentation
  int iResult=0;
  if (pSW!=NULL && type<kSWTypeCount) {
    if (fpStopwatches) {
      TObject* pObj=fpStopwatches->At((int)type);
      if (pSW==NULL        // explicit reset
	  || pObj==NULL) { // explicit set
	fpStopwatches->AddAt(pSW, (int)type);
      } else if (pObj!=pSW) {
	HLTWarning("stopwatch %d already set, reset first", (int)type);
	iResult=-EBUSY;
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTComponent::SetStopwatches(TObjArray* pStopwatches)
{
  // see header file for function documentation
  if (pStopwatches==NULL) return -EINVAL;

  int iResult=0;
  for (int i=0 ; i<(int)kSWTypeCount && pStopwatches->GetEntries(); i++)
    SetStopwatch(pStopwatches->At(i), (AliHLTStopwatchType)i);
  return iResult;
}

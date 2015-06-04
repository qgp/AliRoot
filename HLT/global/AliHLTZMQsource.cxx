// $Id$

///**************************************************************************
///* This file is property of and copyright by the                          * 
///* ALICE Experiment at CERN, All rights reserved.                         *
///*                                                                        *
///* Primary Authors: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch          *
///*                                                                        *
///* Permission to use, copy, modify and distribute this software and its   *
///* documentation strictly for non-commercial purposes is hereby granted   *
///* without fee, provided that the above copyright notice appears in all   *
///* copies and that both the copyright notice and this permission notice   *
///* appear in the supporting documentation. The authors make no claims     *
///* about the suitability of this software for any purpose. It is          *
///* provided "as is" without express or implied warranty.                  *
///**************************************************************************

/// @file   AliHLTZMQsource.cxx
/// @author Mikolaj Krzewicki
/// @date   
/// @brief  HLT ZMQ component implementation. */
///

#include "AliHLTZMQsource.h"
#include "AliHLTErrorGuard.h"
#include "AliLog.h"
#include <TPRegexp.h>
#include "zmq.h"

using namespace std;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTZMQsource)

//______________________________________________________________________________
AliHLTZMQsource::AliHLTZMQsource()
  : AliHLTDataSource()
  , fOutputDataTypes()
  , fZMQcontext(NULL)
  , fZMQin(NULL)
  , fZMQsocketType(ZMQ_PUB)
  , fZMQconnectMode("connect")
  , fZMQendpoint("tcp://localhost:60201")
{
}

//______________________________________________________________________________
AliHLTZMQsource::~AliHLTZMQsource()
{
  //dtor
  zmq_close(fZMQin);
  zmq_ctx_destroy(fZMQcontext);
}

//______________________________________________________________________________
const char* AliHLTZMQsource::GetComponentID()
{
  // overloaded from AliHLTComponent
  return "ZMQsource";
}

//______________________________________________________________________________
AliHLTComponentDataType AliHLTZMQsource::GetOutputDataType()
{
  // overloaded from AliHLTComponent
  if (fOutputDataTypes.size()==0) return kAliHLTVoidDataType;
  else if (fOutputDataTypes.size()==1) return fOutputDataTypes[0];
  return kAliHLTMultipleDataType;
}

//______________________________________________________________________________
int AliHLTZMQsource::GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList)
{
  // overloaded from AliHLTComponent
  tgtList.assign(fOutputDataTypes.begin(), fOutputDataTypes.end());
  HLTInfo("%s %p provides %d output data types", GetComponentID(), this, fOutputDataTypes.size());
  return fOutputDataTypes.size();
}

//______________________________________________________________________________
void AliHLTZMQsource::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // overloaded from AliHLTComponent
  constBase=10000000;
  inputMultiplier=1.0;
}

//______________________________________________________________________________
AliHLTComponent* AliHLTZMQsource::Spawn()
{
  // overloaded from AliHLTComponent
  return new AliHLTZMQsource;
}

//______________________________________________________________________________
int AliHLTZMQsource::DoInit( int argc, const char** argv )
{
  // overloaded from AliHLTComponent: initialization
  int retCode=0;
  //process arguments
  ProcessOptionString(GetComponentArgs());

  int rc = 0;
  //init ZMQ stuff
  fZMQcontext = zmq_ctx_new();
  HLTMessage(Form("ctx create rc %i errno %i\n",rc,errno));
  fZMQin = zmq_socket(fZMQcontext, fZMQsocketType); 
  HLTMessage(Form("socket create rc %i errno %i\n",rc,errno));

  //set socket options
  int lingerValue = 10;
  rc = zmq_setsockopt(fZMQin, ZMQ_LINGER, &lingerValue, sizeof(lingerValue));
  HLTMessage(Form("setopt ZMQ_LINGER=%i rc=%i errno=%i\n", lingerValue, rc, errno));
  int highWaterMarkSend = 20;
  rc = zmq_setsockopt(fZMQin, ZMQ_SNDHWM, &highWaterMarkSend, sizeof(highWaterMarkSend));
  HLTMessage(Form("setopt ZMQ_SNDHWM rc = %i errno=%i\n",highWaterMarkSend, rc, errno));
  int highWaterMarkRecv = 20;
  rc = zmq_setsockopt(fZMQin, ZMQ_RCVHWM, &highWaterMarkRecv, sizeof(highWaterMarkRecv));
  HLTMessage(Form("setopt ZMQ_RCVHWM=%i rc=%i errno=%i\n",highWaterMarkRecv, rc, errno));
  int rcvtimeo = 1000;
  rc = zmq_setsockopt(fZMQin, ZMQ_RCVTIMEO, &rcvtimeo, sizeof(rcvtimeo));
  HLTMessage(Form("setopt ZMQ_RCVTIMEO=%i rc=%i errno=%i\n",rcvtimeo, rc, errno));
  int sndtimeo = 1000;
  rc = zmq_setsockopt(fZMQin, ZMQ_SNDTIMEO, &sndtimeo, sizeof(sndtimeo));
  HLTMessage(Form("setopt ZMQ_SNDTIMEO=%i rc=%i errno=%i\n",sndtimeo, rc, errno));

  //connect or bind, after setting socket options
  if (fZMQconnectMode.Contains("connect")) 
  {
    HLTMessage(Form("ZMQ connect to %s\n",fZMQendpoint.Data()));
    rc = zmq_connect(fZMQin,fZMQendpoint.Data());
    HLTMessage(Form("connect rc %i errno %i\n",rc,errno));
  }
  else 
  {
    HLTMessage(Form("ZMQ bind to %s\n",fZMQendpoint.Data()));
    rc = zmq_bind(fZMQin,fZMQendpoint.Data());
    HLTMessage(Form("bind rc %i errno %i\n",rc,errno));
  }

  return retCode;
}

//______________________________________________________________________________
int AliHLTZMQsource::DoDeinit()
{
  // overloaded from AliHLTComponent: cleanup
  int retCode=0;
  return retCode;
}

//______________________________________________________________________________
int AliHLTZMQsource::GetEvent( const AliHLTComponentEventData& /*evtData*/,
    AliHLTComponentTriggerData& /*trigData*/,
    AliHLTUInt8_t* outputBuffer, 
    AliHLTUInt32_t& outputBufferSize,
    AliHLTComponentBlockDataList& outputBlocks )
{
  // overloaded from AliHLTDataSource: event processing
  int retCode=0;

  // process data events only
  if (!IsDataEvent()) return 0;

  //init internal
  AliHLTUInt32_t outputBufferCapacity = outputBufferSize;
  outputBufferSize=0;
  int blockSize = 0;
  void* block = NULL;

  int blockTopicSize=-1;
  char blockTopic[kAliHLTComponentDataTypeTopicSize];
  memset(blockTopic, 0, kAliHLTComponentDataTypeTopicSize);
  
  int rc = -1;
  int64_t more=0;
  size_t moreSize=sizeof(more);
  do //multipart, get all parts
  {
    outputBufferCapacity -= blockSize;
    outputBufferSize += blockSize;
    block = outputBuffer + outputBufferSize;
    
    blockTopicSize = zmq_recv (fZMQin, blockTopic, kAliHLTComponentDataTypeTopicSize, ZMQ_DONTWAIT);
    if (blockTopicSize<0 && errno==EAGAIN) break; //nothing on the socket
    zmq_getsockopt(fZMQin, ZMQ_RCVMORE, &more, &moreSize);
    if (more) {
      blockSize = zmq_recv(fZMQin, block, outputBufferCapacity, ZMQ_DONTWAIT);
      if (blockSize < 0 && errno == EAGAIN) break; //nothing on the socket
      if (blockSize > outputBufferCapacity) {retCode = ENOSPC; break;}//no space for message
      zmq_getsockopt(fZMQin, ZMQ_RCVMORE, &more, &moreSize);
    }

    if (blockSize <= 0) continue; //empty data, dont push back
    
    AliHLTComponentBlockData blockHeader; FillBlockData(blockHeader);
    blockHeader.fPtr      = outputBuffer;
    blockHeader.fOffset   = outputBufferSize;
    blockHeader.fSize     = blockSize;
    blockHeader.fDataType = blockTopic;
    blockHeader.fSpecification = 0;
    
    outputBlocks.push_back(blockHeader);
  } while (more==1);

  return retCode;
}

//______________________________________________________________________________
int AliHLTZMQsource::ProcessOption(TString option, TString value)
{
  //process option
  //to be implemented by the user
  
  if (option.Contains("ZMQsocketMode")) 
  {
    if (value.Contains("SUB"))  fZMQsocketType=ZMQ_SUB;
    if (value.Contains("PULL")) fZMQsocketType=ZMQ_PULL;
  }
 
  if (option.Contains("ZMQconnectMode"))
  {
    if (! (
          value.Contains("connect") ||
          value.Contains("bind")
          )
       ) {return 1;}
    fZMQconnectMode = value;
  }
 
  if (option.Contains("ZMQendpoint"))
  {
    fZMQendpoint = value;
  }

  return 1; 
}

////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int AliHLTZMQsource::ProcessOptionString(TString arguments)
{
  //process passed options
  HLTInfo("Argument string: %s\n", arguments.Data());
  stringMap* options = TokenizeOptionString(arguments);
  for (stringMap::iterator i=options->begin(); i!=options->end(); ++i)
  {
    HLTInfo("  %s : %s\n", i->first.data(), i->second.data());
    ProcessOption(i->first,i->second);
  }
  delete options; //tidy up

  return 1; 
}

//______________________________________________________________________________
AliHLTZMQsource::stringMap* AliHLTZMQsource::TokenizeOptionString(const TString str)
{
  //options have the form:
  // -option value
  // -option=value
  // -option
  // --option value
  // --option=value
  // --option
  // option=value
  // option value
  // (value can also be a string like 'some string')
  //
  // options can be separated by ' ' or ',' arbitrarily combined, e.g:
  //"-option option1=value1 --option2 value2, -option4=\'some string\'"
  
  //optionRE by construction contains a pure option name as 3rd submatch (without --,-, =)
  //valueRE does NOT match options
  TPRegexp optionRE("(?:(-{1,2})|((?='?[^,=]+=?)))"
                    "((?(2)(?:(?(?=')'(?:[^'\\\\]++|\\.)*+'|[^, =]+))(?==?))"
                    "(?(1)[^, =]+(?=[= ,$])))");
  TPRegexp valueRE("(?(?!(-{1,2}|[^, =]+=))"
                   "(?(?=')'(?:[^'\\\\]++|\\.)*+'"
                   "|[^, =]+))");

  stringMap* options = new stringMap;

  TArrayI pos;
  const TString mods="";
  Int_t start = 0;
  while (1) {
    Int_t prevStart=start;
    TString optionStr="";
    TString valueStr="";
    
    //check if we have a new option in this field
    Int_t nOption=optionRE.Match(str,mods,start,10,&pos);
    if (nOption>0)
    {
      optionStr = str(pos[6],pos[7]-pos[6]);
      optionStr=optionStr.Strip(TString::kBoth,'\'');
      start=pos[1]; //update the current character to the end of match
    }

    //check if the next field is a value
    Int_t nValue=valueRE.Match(str,mods,start,10,&pos);
    if (nValue>0)
    {
      valueStr = str(pos[0],pos[1]-pos[0]);
      valueStr=valueStr.Strip(TString::kBoth,'\'');
      start=pos[1]; //update the current character to the end of match
    }
    
    //skip empty entries
    if (nOption>0 || nValue>0)
    {
      (*options)[optionStr.Data()] = valueStr.Data();
    }
    
    if (start>=str.Length()-1 || start==prevStart ) break;
  }

  for (stringMap::iterator i=options->begin(); i!=options->end(); ++i)
  {
    Printf("%s : %s", i->first.data(), i->second.data());
  }
  return options;
}

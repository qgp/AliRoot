// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTDataSink.cxx
    @author Matthias Richter
    @date   
    @brief  Base class implementation for HLT data source components. */

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTDataSink.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTDataSink)

AliHLTDataSink::AliHLTDataSink()
{ 
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTDataSink::~AliHLTDataSink()
{ 
  // see header file for class documentation
}

AliHLTComponentDataType AliHLTDataSink::GetOutputDataType()
{
  // see header file for class documentation
  AliHLTComponentDataType dt =
    {sizeof(AliHLTComponentDataType),
     kAliHLTVoidDataTypeID,
     kAliHLTVoidDataOrigin};
  return dt;
}

void AliHLTDataSink::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // see header file for class documentation
  constBase=0;
  inputMultiplier=0;
}

int AliHLTDataSink::DoProcessing( const AliHLTComponentEventData& evtData,
				  const AliHLTComponentBlockData* blocks, 
				  AliHLTComponentTriggerData& trigData,
				  AliHLTUInt8_t* outputPtr, 
				  AliHLTUInt32_t& size,
				  vector<AliHLTComponentBlockData>& outputBlocks,
				  AliHLTComponentEventDoneData*& edd )
{
  // see header file for class documentation
  int iResult=0;
  if (outputPtr==NULL
      && size==0 
      && edd==NULL) {
    // this is currently just to get rid of the warning "unused parameter"
  }
  outputBlocks.clear();
  iResult=DumpEvent(evtData, blocks, trigData);
  return iResult;
}

int AliHLTDataSink::DumpEvent( const AliHLTComponentEventData& evtData,
			       const AliHLTComponentBlockData* /*blocks*/, 
			       AliHLTComponentTriggerData& trigData )
{
  // we just forward to the high level method, all other parameters already
  // have been stored internally
  return DumpEvent(evtData, trigData);
}

int AliHLTDataSink::DumpEvent( const AliHLTComponentEventData& /*evtData*/, AliHLTComponentTriggerData& /*trigData*/)
{
  HLTFatal("no processing method implemented");
  return -ENOSYS;
}

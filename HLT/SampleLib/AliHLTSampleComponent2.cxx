// $Id$

//**************************************************************************
//* This file is property of and copyright by the                          * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
//*                  Timm Steinbeck <timm@kip.uni-heidelberg.de>           *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************/

/// @file   AliHLTSampleComponent2.cxx
/// @author Matthias Richter, Timm Steinbeck
/// @date   
/// @brief  Another sample processing component for the HLT.
///

#include "AliHLTSampleComponent2.h"

using namespace std;

ClassImp(AliHLTSampleComponent2)

AliHLTSampleComponent2::AliHLTSampleComponent2()
{
  // see header file for class documentation
}

AliHLTSampleComponent2::~AliHLTSampleComponent2()
{
  // see header file for class documentation
}

int AliHLTSampleComponent2::DoInit( int argc, const char** argv ){
  // see header file for class documentation
  Logging(kHLTLogInfo, "HLT", "Sample", "Sample component2, DoInit");
  if (argc==0 && argv==NULL) {
    // this is just to get rid of the warning "unused parameter"
  }
  return 0;
}

int AliHLTSampleComponent2::DoDeinit(){
  // see header file for class documentation
  Logging(kHLTLogInfo, "HLT", "Sample", "Sample component2, DoDeinit");
  return 0;
}

int AliHLTSampleComponent2::DoEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
				      AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
				      AliHLTUInt32_t& size, AliHLTComponentBlockDataList& outputBlocks ) {
  // see header file for class documentation
  Logging(kHLTLogInfo, "HLT", "Sample", "Sample component2, DoEvent");
  if (evtData.fStructSize==0 && blocks==NULL && trigData.fStructSize==0 &&
      outputPtr==0 && size==0)
  {
    outputBlocks.clear();
    // this is just to get rid of the warning "unused parameter"
  }
  return 0;
}

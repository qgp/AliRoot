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

/** @file   AliHLTRootFilePublisherComponent.cxx
    @author Matthias Richter
    @date   
    @brief  HLT file publisher component implementation. */

#include "AliHLTRootFilePublisherComponent.h"
//#include <TObjString.h>
//#include <TMath.h>
//#include <TFile.h>

// temporary
#include "TH1F.h"

/** the global object for component registration */
AliHLTRootFilePublisherComponent gAliHLTRootFilePublisherComponent;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTRootFilePublisherComponent)

AliHLTRootFilePublisherComponent::AliHLTRootFilePublisherComponent()
  :
  AliHLTFilePublisher()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

}

AliHLTRootFilePublisherComponent::AliHLTRootFilePublisherComponent(const AliHLTRootFilePublisherComponent&)
  :
  AliHLTFilePublisher()
{
  // see header file for class documentation
  HLTFatal("copy constructor untested");
}

AliHLTRootFilePublisherComponent& AliHLTRootFilePublisherComponent::operator=(const AliHLTRootFilePublisherComponent&)
{ 
  // see header file for class documentation
  HLTFatal("assignment operator untested");
  return *this;
}

AliHLTRootFilePublisherComponent::~AliHLTRootFilePublisherComponent()
{
  // see header file for class documentation

  // file list and file name list are owner of their objects and
  // delete all the objects
}

const char* AliHLTRootFilePublisherComponent::GetComponentID()
{
  // see header file for class documentation
  return "ROOTFilePublisher";
}

AliHLTComponentDataType AliHLTRootFilePublisherComponent::GetOutputDataType()
{
  // see header file for class documentation
  AliHLTComponentDataType dt =
    {sizeof(AliHLTComponentDataType),
     kAliHLTVoidDataTypeID,
     kAliHLTVoidDataOrigin};
  return dt;
}

void AliHLTRootFilePublisherComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // see header file for class documentation
  constBase=10000;
  inputMultiplier=1.0;
}

AliHLTComponent* AliHLTRootFilePublisherComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTRootFilePublisherComponent;
}

int AliHLTRootFilePublisherComponent::ScanArgument(int argc, const char** argv)
{
  // see header file for class documentation

  // there are no other arguments than the standard ones
  if (argc==0 && argv==NULL) {
    // this is just to get rid of the warning "unused parameter"
  }
  int iResult=-EPROTO;
  return iResult;
}

int AliHLTRootFilePublisherComponent::OpenFiles()
{
  // see header file for class documentation
  int iResult=0;
  return iResult;
}

int AliHLTRootFilePublisherComponent::GetEvent( const AliHLTComponentEventData& evtData,
	      AliHLTComponentTriggerData& trigData,
	      AliHLTUInt8_t* outputPtr, 
	      AliHLTUInt32_t& size,
	      vector<AliHLTComponentBlockData>& outputBlocks )
{
  int iResult=0;
  if (GetCurrentSpecification()==0) {
    TH1F *hpx = new TH1F("hpx","px distribution",100,-4,4);
    hpx->FillRandom("gaus",1000);
    PushBack(hpx, "TH1F", "ROOT");
  } else {
    TH1F *hpy = new TH1F("hpy","py distribution",100,-10,10);
    hpy->FillRandom("gaus",10000);
    PushBack(hpy, "TH1F", "ROOT");
  }

  return iResult;
}

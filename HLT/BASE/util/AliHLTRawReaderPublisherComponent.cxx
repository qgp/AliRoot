// @(#) $Id$

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

/** @file   AliHLTRawReaderPublisherComponent.cxx
    @author Matthias Richter
    @date   
    @brief  A general tree publisher component for the AliRawReader.
*/

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliHLTRawReaderPublisherComponent.h"
#include "AliRawReader.h"
#include "AliLog.h"
#include <cerrno>
#include <cassert>

/** global instance for agent registration */
AliHLTRawReaderPublisherComponent gAliHLTRawReaderPublisherComponent;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTRawReaderPublisherComponent)

AliHLTRawReaderPublisherComponent::AliHLTRawReaderPublisherComponent()
  :
  fMaxSize(50000),
  fDetector(),
  fMinEquId(-1),
  fMaxEquId(-1),
  fVerbose(kFALSE),
  fDataType(kAliHLTAnyDataType),
  fSpecification(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTRawReaderPublisherComponent::~AliHLTRawReaderPublisherComponent()
{
  // see header file for class documentation
}

const char* AliHLTRawReaderPublisherComponent::GetComponentID()
{
  // see header file for class documentation
  return "AliRawReaderPublisher";
}

AliHLTComponentDataType AliHLTRawReaderPublisherComponent::GetOutputDataType()
{
  return fDataType;
}

void AliHLTRawReaderPublisherComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  constBase=fMaxSize;
  inputMultiplier=1;
}

AliHLTComponent* AliHLTRawReaderPublisherComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTRawReaderPublisherComponent;
}

int AliHLTRawReaderPublisherComponent::DoInit( int argc, const char** argv )
{
  // see header file for class documentation
  int iResult=0;

  // scan arguments
  TString argument="";
  int bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

    // -detector
    if (argument.CompareTo("-detector")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      fDetector=argv[i];

      // -equipmentid, -minid
    } else if (argument.CompareTo("-equipmentid")==0 ||
	       argument.CompareTo("-minid")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      parameter.Remove(TString::kLeading, ' '); // remove all blanks
      if (parameter.IsDigit()) {
	fMinEquId=(AliHLTUInt32_t)parameter.Atoi();
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }

      // -maxid
    } else if (argument.CompareTo("-maxid")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      parameter.Remove(TString::kLeading, ' '); // remove all blanks
      if (parameter.IsDigit()) {
	fMaxEquId=(AliHLTUInt32_t)parameter.Atoi();
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }

      // -verbose
    } else if (argument.CompareTo("-verbose")==0) {
      fVerbose=kTRUE;

      // -datatype
    } else if (argument.CompareTo("-datatype")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fID, argv[i], TMath::Min(kAliHLTComponentDataTypefIDsize, (Int_t)strlen(argv[i])));
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fOrigin, argv[i], TMath::Min(kAliHLTComponentDataTypefOriginSize, (Int_t)strlen(argv[i])));

      // -dataspec
    } else if (argument.CompareTo("-dataspec")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      parameter.Remove(TString::kLeading, ' '); // remove all blanks
      if (parameter.IsDigit()) {
	fSpecification=(AliHLTUInt32_t)parameter.Atoi();
      } else if (parameter.BeginsWith("0x") &&
		 parameter.Replace(0,2,"",0).IsHex()) {
	sscanf(parameter.Data(),"%x", &fSpecification);
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
    } else {
      HLTError("unknown argument %s", argument.Data());
      iResult=-EINVAL;
    }
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }

  if (iResult<0) return iResult;

  if (fDetector.IsNull()) {
    AliErrorStream() << "detector required, use \'-detector\' option" << endl;
    return -EINVAL;
  }

  if (fMinEquId>fMaxEquId) fMaxEquId=fMinEquId;

  if (fMinEquId<0) {
    AliErrorStream() << "equipment id required, use \'-equipmentid\' option" << endl;
    return -EINVAL;
  }

  AliHLTUInt32_t dummy;
  if (fMinEquId!=fMaxEquId && GetSpecificationFromEquipmentId(0, dummy)==-ENOSYS) {
    AliWarningStream() << "publication of multiple equipment ids needs implementation of a child and function GetSpecificationFromEquipmentId to set correct specifications" << endl;
    //return -EINVAL;
  }

  AliRawReader* pRawReader=GetRawReader();
  if ((pRawReader=GetRawReader())!=NULL) {
    pRawReader->Select(fDetector.Data(), fMinEquId, fMaxEquId);
    if (!pRawReader->RewindEvents()) {
      AliWarning(Form("can not rewind RawReader %p", pRawReader));
    }
  } else {
    AliErrorStream() << "RawReader instance needed" << endl;
    return -EINVAL;
  }

  return iResult;
}

int AliHLTRawReaderPublisherComponent::DoDeinit()
{
  // see header file for class documentation
  int iResult=0;
  return iResult;
}

int AliHLTRawReaderPublisherComponent::GetEvent(const AliHLTComponentEventData& evtData, 
						AliHLTComponentTriggerData& trigData, 
						AliHLTUInt8_t* outputPtr, 
						AliHLTUInt32_t& size, 
						vector<AliHLTComponentBlockData>& outputBlocks)
{
  // see header file for class documentation
  int iResult=0;
  int offset=0;
  AliHLTUInt8_t* pTgt=outputPtr;
  assert(outputPtr!=NULL);
  AliRawReader* pRawReader=GetRawReader();
  if (pRawReader) {
    AliInfo(Form("get event from RawReader %p", pRawReader));
    while (pRawReader->ReadHeader() && (iResult>=0 || iResult==-ENOSPC)) {
      int readSize=pRawReader->GetDataSize();
      int id=pRawReader->GetEquipmentId();
      AliInfo(Form("got header for id %d, size %d", readSize, id));
      if (fMinEquId<id || fMaxEquId>id) {
	AliError(Form("id %d returned from RawReader is outside range [%d,%d]", id, fMinEquId, fMaxEquId));
	continue;
      }
      if (readSize>0) {
	if (readSize<=size-offset) {
	  if (!pRawReader->ReadNext(pTgt, readSize)) {
	    AliError(Form("error reading %d bytes from RawReader %p", readSize, pRawReader));
	    iResult=-ENODATA;
	    break;
	  }
	} else {
	  fMaxSize=offset+readSize;
	  iResult=-ENOSPC;
	}
      }
      if (iResult>=0) {
	AliHLTComponentBlockData bd;
	FillBlockData( bd );
	bd.fOffset = offset;
	bd.fSize = readSize;
	bd.fDataType = fDataType;
	bd.fSpecification = 0;
	GetSpecificationFromEquipmentId(id, bd.fSpecification);
	outputBlocks.push_back( bd );
      }
      offset+=readSize;
    }
    // go to next event, or beginning if last event was processed
    if (pRawReader->NextEvent()) {
      pRawReader->RewindEvents();
    }
    if (offset<=size) size=offset;
  } else {
    AliErrorStream() << "RawReader uninitialized" << endl;
    iResult=-EFAULT;
  }
  return iResult;
}

int AliHLTRawReaderPublisherComponent::GetSpecificationFromEquipmentId(int id, AliHLTUInt32_t& /*specification*/) const {
  return -ENOSYS;
}

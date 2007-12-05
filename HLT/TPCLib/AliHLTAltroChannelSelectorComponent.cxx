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

/** @file   AliHLTAltroChannelSelectorComponent.cxx
    @author Matthias Richter
    @date   
    @brief  A filter/selective readout component for TPC Altro data. */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include <cassert>
#include "AliHLTAltroChannelSelectorComponent.h"
#include "AliHLTTPCTransform.h"
#include "AliHLTTPCDigitReaderRaw.h"
#include "AliHLTTPCDefinitions.h"
#include "AliHLTTPCPadArray.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTAltroChannelSelectorComponent)

AliHLTAltroChannelSelectorComponent::AliHLTAltroChannelSelectorComponent()
  :
  AliHLTProcessor(),
  fRawreaderMode(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTAltroChannelSelectorComponent::~AliHLTAltroChannelSelectorComponent()
{
  // see header file for class documentation
}

const char* AliHLTAltroChannelSelectorComponent::GetComponentID()
{
  // see header file for class documentation
  return "AltroChannelSelector";
}

void AliHLTAltroChannelSelectorComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  // see header file for class documentation
  list.clear();
  list.push_back(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC);
  list.push_back(AliHLTTPCDefinitions::fgkActivePadsDataType);
  list.push_back(kAliHLTDataTypeHwAddr16);
}

AliHLTComponentDataType AliHLTAltroChannelSelectorComponent::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC;
}

void AliHLTAltroChannelSelectorComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  // see header file for class documentation
  constBase=0;
  inputMultiplier=1.0;
}

AliHLTComponent* AliHLTAltroChannelSelectorComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTAltroChannelSelectorComponent;
}

int AliHLTAltroChannelSelectorComponent::DoInit(int argc, const char** argv)
{
  // see header file for class documentation
  int iResult=0;
  TString argument="";
  bool bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

    // -rawreadermode
    if (argument.CompareTo("-rawreadermode")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      int mode=AliHLTTPCDigitReaderRaw::DecodeMode(argv[i]);
      if (mode<0) {
	HLTError("invalid rawreadermode specifier '%s'", argv[i]);
	iResult=-EINVAL;
      } else {
	fRawreaderMode=static_cast<unsigned>(mode);
	// always use the reader in unsorted mode regardless of the
	// argument 
	if (fRawreaderMode%2==0) fRawreaderMode++;
      }
    } else {
      iResult=-EINVAL;
    }
  }

  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }

  return iResult;
}

int AliHLTAltroChannelSelectorComponent::DoDeinit()
{
  // see header file for class documentation
  return 0;
}

int AliHLTAltroChannelSelectorComponent::DoEvent(const AliHLTComponentEventData& evtData,
						 const AliHLTComponentBlockData* blocks, 
						 AliHLTComponentTriggerData& /*trigData*/,
						 AliHLTUInt8_t* outputPtr, 
						 AliHLTUInt32_t& size,
						 AliHLTComponentBlockDataList& outputBlocks )
{
  // see header file for class documentation
  int iResult=0;

  // process the DLL input
  int blockno=0;
  const AliHLTComponentBlockData* pDesc=NULL;

  for (pDesc=GetFirstInputBlock(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC); pDesc!=NULL; pDesc=GetNextInputBlock(), blockno++) {

    // search for the active pad information
    AliHLTTPCPadArray::AliHLTTPCActivePads* pActivePadsArray=NULL;
    AliHLTUInt16_t* pActiveHwAddressArray=NULL;
    int iArraySize=0;
    for (int i=0; i<(int)evtData.fBlockCnt; i++ ) {
      const AliHLTComponentBlockData* iter=NULL;
      // search for selection data of row/pad type
      for (iter=GetFirstInputBlock(AliHLTTPCDefinitions::fgkActivePadsDataType); iter!=NULL; iter=GetNextInputBlock()) {
	if (iter->fSpecification==pDesc->fSpecification) {
	  pActivePadsArray=reinterpret_cast<AliHLTTPCPadArray::AliHLTTPCActivePads*>(iter->fPtr);
	  iArraySize=iter->fSize/sizeof(AliHLTTPCPadArray::AliHLTTPCActivePads);
	  break;
	}
      }

      // search for selection data of hw address type
      for (iter=GetFirstInputBlock(kAliHLTDataTypeHwAddr16); iter!=NULL; iter=GetNextInputBlock()) {
	if (iter->fSpecification==pDesc->fSpecification) {
	  pActiveHwAddressArray=reinterpret_cast<AliHLTUInt16_t*>(iter->fPtr);
	  iArraySize=iter->fSize/sizeof(AliHLTUInt16_t);
	  break;
	}
      }
    }
    if (pActivePadsArray==NULL && pActiveHwAddressArray==NULL) {
      HLTWarning("no block of type %s or %s for specification 0x%08x available, data block unchanged", 
		 DataType2Text(AliHLTTPCDefinitions::fgkActivePadsDataType).c_str(), 
		 DataType2Text(kAliHLTDataTypeHwAddr16).c_str(), 
		 pDesc->fSpecification);
      // forward the whole block
      outputBlocks.push_back(*pDesc);
      continue;
    }

    int part=AliHLTTPCDefinitions::GetMinPatchNr(*pDesc);
    assert(part==AliHLTTPCDefinitions::GetMaxPatchNr(*pDesc));
    int slice=AliHLTTPCDefinitions::GetMinSliceNr(*pDesc);
    assert(slice==AliHLTTPCDefinitions::GetMaxSliceNr(*pDesc));
    int firstRow=AliHLTTPCTransform::GetFirstRow(part);
    int lastRow=AliHLTTPCTransform::GetLastRow(part);
    AliHLTTPCDigitReaderRaw reader(fRawreaderMode);
    HLTDebug("init reader %p size %d", pDesc->fPtr,pDesc->fSize);
    reader.InitBlock(pDesc->fPtr,pDesc->fSize,firstRow,lastRow,part,slice);
    int iSelected=0;
    int iTotal=0;
    AliHLTUInt32_t iOutputSize=0;
    AliHLTUInt32_t iCapacity=size;
    while (reader.NextAltroBlock()) {
      iTotal++;

      void* pChannel=NULL;
      AliHLTUInt16_t hwAddress=~(AliHLTUInt16_t)0;
      int channelSize=reader.GetAltroChannelRawData(pChannel, hwAddress);

      int active=0;
      if (pActivePadsArray) {
	for (; active<iArraySize; active++) {
	  if ((int)pActivePadsArray[active].fRow==reader.GetRow() &&
	      (int)pActivePadsArray[active].fPad==reader.GetPad()) {
	    break;
	  }
	}
      } else {
	for (; active<iArraySize; active++) {
	  if (pActiveHwAddressArray[active]==hwAddress) {
	    break;
	  }
	}
      }
      if (active>=iArraySize) {
	HLTDebug("ALTRO block Row %d, Pad %d discarded (inactive)", reader.GetRow(), reader.GetPad());
	continue;
      }

      iSelected++;
      HLTDebug("ALTRO block hwAddress 0x%08x Row/Pad %d/%d selected (active), size %d", hwAddress, reader.GetRow(), reader.GetPad(), channelSize);
      if (channelSize>0 && pChannel!=NULL) {
	if (iOutputSize==0) {
	  // first add the RCU trailer
	  unsigned rcuTrailerLength=reader.GetRCUDataBlockLength();
	  AliHLTUInt8_t* pSrc=reinterpret_cast<AliHLTUInt8_t*>(pDesc->fPtr);
	  pSrc+=pDesc->fSize-rcuTrailerLength;
	  if ((iResult=CopyBlockToEnd(outputPtr, iCapacity, iOutputSize, pSrc, rcuTrailerLength))>=0) {
	    assert(iResult==rcuTrailerLength);
	    iOutputSize+=rcuTrailerLength;
	  } else {
	    HLTError("failed to write RCU trailer of length %d for block %d", rcuTrailerLength, blockno);
	    break;
	  }
	}
      }
      if ((iResult=CopyBlockToEnd(outputPtr, iCapacity, iOutputSize, pChannel, channelSize))>=0) {
	assert(iResult==channelSize);
	iOutputSize+=channelSize;
      } else {
	HLTError("failed to write ALTRO channel of length %d for block %d", channelSize, blockno);
	break;
      }
    }
    if (iResult>=0) {
      // write the common data header
      int cdhSize=reader.GetCommonDataHeaderSize();
      if ((iResult=CopyBlockToEnd(outputPtr, iCapacity, iOutputSize, pDesc->fPtr, cdhSize))>=0) {
	assert(iResult==cdhSize);
	iOutputSize+=cdhSize;

	// set new length of the data block
	AliHLTUInt32_t* pCdhSize=reinterpret_cast<AliHLTUInt32_t*>(outputPtr+iCapacity-iOutputSize+1);
	*pCdhSize=iOutputSize;

	// insert block descriptor
	AliHLTComponentBlockData bd;
	FillBlockData(bd);
	bd.fOffset=iCapacity-iOutputSize;
	bd.fSize=iOutputSize;
	bd.fDataType=pDesc->fDataType;
	bd.fSpecification=pDesc->fSpecification;
	outputBlocks.push_back(bd);
	iCapacity-=iOutputSize;
      } else {
	HLTError("failed to write CDH of length %d for block %d", cdhSize, blockno);
	break;
      }
    }
    HLTInfo("data block %d (0x%08x): selected %d out of %d ALTRO channels", blockno, pDesc->fSpecification, iSelected, iTotal);
  }

  if (iResult<0) {
    outputBlocks.clear();
  }

  // !!! do not change the size since the output buffer is filled from the end !!!

  return iResult;
}

int AliHLTAltroChannelSelectorComponent::CopyBlockToEnd(AliHLTUInt8_t* pTgt, unsigned capacity, unsigned position, void* pSrc, unsigned size)
{
  int iResult=0;
  if (pTgt==NULL || pSrc==NULL) return -EINVAL;
  if (capacity-position<size) return -ENOSPC;
  
  memcpy(pTgt+(capacity-position-size), pSrc, size);
  iResult=size;
  
  return iResult;
}

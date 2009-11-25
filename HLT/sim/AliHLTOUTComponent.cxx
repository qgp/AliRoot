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

/** @file   AliHLTOUTComponent.cxx
    @author Matthias Richter
    @date   
    @brief  The HLTOUT data sink component similar to HLTOUT nodes
*/

#if __GNUC__>= 3
using namespace std;
#endif

#include <cassert>
//#include <iostream>
#include "AliHLTOUTComponent.h"
#include "AliHLTOUT.h"
#include "AliHLTHOMERLibManager.h"
#include "AliHLTHOMERWriter.h"
#include "AliDAQ.h" // equipment Ids
#include "AliRawDataHeader.h" // Common Data Header 
#include <TDatime.h> // seed for TRandom
#include <TRandom.h> // random int generation for DDL no
#include <TFile.h>
#include <TTree.h>
#include <TArrayC.h>

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTOUTComponent)

AliHLTOUTComponent::AliHLTOUTComponent()
  :
  AliHLTOfflineDataSink(),
  fWriters(),
  fNofDDLs(10),
  fIdFirstDDL(7680), // 0x1e<<8
  fBuffer(),
  fpLibManager(NULL),
  fpDigitFile(NULL),
  fpDigitTree(NULL),
  fppDigitArrays(NULL),
  fReservedWriter(-1),
  fReservedData(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

  // I guess DDL definitions should never change any more
  assert(fNofDDLs==AliDAQ::NumberOfDdls("HLT"));
  fNofDDLs=AliDAQ::NumberOfDdls("HLT");
  
  /* AliDAQ::DdlIDOffset returns wrong offset for HLT links
  assert(fIdFirstDDL==AliDAQ::DdlIDOffset("HLT"));
  fIdFirstDDL=AliDAQ::DdlIDOffset("HLT");
  */
}

int AliHLTOUTComponent::fgOptions=kWriteRawFiles|kWriteDigits;

AliHLTOUTComponent::~AliHLTOUTComponent()
{
  // see header file for class documentation
  if (fpLibManager) delete fpLibManager;
  fpLibManager=NULL;
}

const char* AliHLTOUTComponent::GetComponentID()
{
  // see header file for class documentation
  return "HLTOUT";
}

void AliHLTOUTComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  // see header file for class documentation
  list.clear();
  list.push_back(kAliHLTAnyDataType);
}

AliHLTComponent* AliHLTOUTComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTOUTComponent;
}

int AliHLTOUTComponent::DoInit( int argc, const char** argv )
{
  // see header file for class documentation
  int iResult=0;
  TString argument="";
  int bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

    // -links
    if (argument.CompareTo("-links")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      parameter.Remove(TString::kLeading, ' '); // remove all blanks
      if (parameter.IsDigit()) {
	fNofDDLs=parameter.Atoi();
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
    } else {
      HLTError("unknown argument %s", argument.Data());
      iResult=-EINVAL;
      break;
    }
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }
  if (iResult>=0) {
  }

  // Make sure there is no library manager before we try and create a new one.
  if (fpLibManager) {
    delete fpLibManager;
    fpLibManager=NULL;
  }
  
  // Create a new library manager and allocate the appropriate number of
  // HOMER writers for the HLTOUT component.
  fpLibManager=new AliHLTHOMERLibManager;
  if (fpLibManager) {
    int writerNo=0;
    for (writerNo=0; writerNo<fNofDDLs; writerNo++) {
      AliHLTMonitoringWriter* pWriter=fpLibManager->OpenWriter();
      if (pWriter) {
	HLTDebug("HOMER writer %p added", pWriter);
	fWriters.push_back(pWriter);
      } else {
	HLTError("can not open HOMER writer");
	iResult=-ENODEV;
	break;
      }
    }
  } else {
    iResult=-ENOMEM;
  }

  return iResult;
}

int AliHLTOUTComponent::DoDeinit()
{
  // see header file for class documentation
  int iResult=0;

  if (fpLibManager) {
    AliHLTMonitoringWriterPVector::iterator element=fWriters.begin();
    while (element!= fWriters.end()) {
      assert(*element);
      // wanted to have a dynamic_cast<AliHLTHOMERWriter*> here, but this results into
      // undefined symbol when loading the library
      if (*element!=NULL) {
	(*element)->Clear();
	fpLibManager->DeleteWriter((AliHLTHOMERWriter*)(*element));
      } else {
	HLTError("writer instance is NULL");
      }
      element=fWriters.erase(element);
    }
  }
  if (fpLibManager) {
    delete fpLibManager;
    fpLibManager=NULL;
  }

  if (fpDigitTree) {
    delete fpDigitTree;
    fpDigitTree=NULL;
  }

  if (fpDigitFile) {
    fpDigitFile->Close();
    delete fpDigitFile;
    fpDigitFile=NULL;
  }

  if (fppDigitArrays) {
    for (int i=0; i<fNofDDLs; i++) {
      if (fppDigitArrays[i]) delete fppDigitArrays[i];
    }
    delete[] fppDigitArrays;
    fppDigitArrays=NULL;
  }
  
  return iResult;
}

int AliHLTOUTComponent::DumpEvent( const AliHLTComponentEventData& evtData,
			 const AliHLTComponentBlockData* blocks, 
			 AliHLTComponentTriggerData& /*trigData*/ )
{
  // see header file for class documentation
  int iResult=0;
  HLTInfo("write %d output block(s)", evtData.fBlockCnt);
  int writerNo=0;
  AliHLTUInt32_t eventType=gkAliEventTypeUnknown;
  bool bIsDataEvent=IsDataEvent(&eventType);
  if (iResult>=0) {
    homer_uint64 homerHeader[kCount_64b_Words];
    HOMERBlockDescriptor homerDescriptor(homerHeader);
    for (int n=0; n<(int)evtData.fBlockCnt; n++ ) {
      if (blocks[n].fDataType==kAliHLTDataTypeEvent ||
	  blocks[n].fDataType==kAliHLTDataTypeSOR ||
	  blocks[n].fDataType==kAliHLTDataTypeEOR ||
	  blocks[n].fDataType==kAliHLTDataTypeComConf ||
	  blocks[n].fDataType==kAliHLTDataTypeUpdtDCS)
	{
	  // the special events have to be ignored.
	  continue;
	}
      if (!bIsDataEvent &&
	  (blocks[n].fDataType!=kAliHLTDataTypeComponentTable))
	{
	  // In simulation, there are no SOR and EOR events created. Thats
	  // why all data blocks of those events are currently ignored.
	  // Strictly speaking, components should not create output blocks
	  // on the SOR/EOR event
	  //
	  // Exeptions: some blocks are added, the buffer must be prepared and
	  // kept since the pointers will be invalid
	  // - kAliHLTDataTypeComponentTable component table entries
	  continue;
	}
      memset( homerHeader, 0, sizeof(homer_uint64)*kCount_64b_Words );
      homerDescriptor.Initialize();
      // for some traditional reason the TCPDumpSubscriber swaps the bytes
      // of the data type id and data type origin. Actually I do not understand
      // the corresponding code line
      // homerBlock.SetType( blocks[n].fDataType.fID );
      // this compiles in the PubSub framework and in addition does a byte swap
      homer_uint64 id=0;
      homer_uint64 origin=0;
      memcpy(&id, blocks[n].fDataType.fID, sizeof(homer_uint64));
      memcpy(((AliHLTUInt8_t*)&origin)+sizeof(homer_uint32), blocks[n].fDataType.fOrigin, sizeof(homer_uint32));
      homerDescriptor.SetType(AliHLTOUT::ByteSwap64(id));
      homerDescriptor.SetSubType1(AliHLTOUT::ByteSwap64(origin));
      homerDescriptor.SetSubType2(blocks[n].fSpecification);
      homerDescriptor.SetBlockSize(blocks[n].fSize);
      if (bIsDataEvent) {
	writerNo=ShuffleWriters(fWriters, blocks[n].fSize);
      }
      assert(writerNo>=0 && writerNo<(int)fWriters.size());
      // I'm puzzled by the different headers, buffers etc. used in the
      // HOMER writer/data. In additional, there is no type check as there
      // are void pointers used and names mixed.
      // It seems that HOMERBlockDescriptor is just a tool to set the
      // different fields in the homer header, which is an array of 64 bit
      // words.
      fWriters[writerNo]->AddBlock(homerHeader, blocks[n].fPtr);
    }
  }

  if (iResult>=0 && !bIsDataEvent) {
    // data blocks from a special event are kept to be added to the
    // following event.
    if (fReservedWriter>=0) {
      HLTWarning("overriding previous buffer of non-data event data blocks");
    }
    const AliHLTUInt8_t* pBuffer=NULL;
    int bufferSize=0;
    // TODO: not yet clear whether it is smart to send the event id of
    // this special event or if it should be set from the id of the
    // following event where the data will be added
    if ((bufferSize=FillOutputBuffer(evtData.fEventID, fWriters[writerNo], pBuffer))>0) {
      fReservedWriter=writerNo;
      fReservedData=bufferSize;
    }
    fWriters[writerNo]->Clear();
  }

  return iResult;
}

int AliHLTOUTComponent::FillESD(int eventNo, AliRunLoader* runLoader, AliESDEvent* /*esd*/)
{
  // see header file for class documentation
  int iResult=0;

  if (fWriters.size()==0) return 0;
  
  if (fReservedWriter>=0) {
    if (fgOptions&kWriteDigits) WriteDigitArray(fReservedWriter, &fBuffer[0], fReservedData);
    if (fgOptions&kWriteRawFiles) WriteRawFile(eventNo, runLoader, fReservedWriter, &fBuffer[0], fReservedData);
    fReservedData=0;
  }

  // search for the writer with the biggest data volume in order to allocate the
  // output buffer of sufficient size
  vector<int> sorted;
  for (size_t i=0; i<fWriters.size(); i++) {
    if ((int)i==fReservedWriter) continue;    
    assert(fWriters[i]);
    if (fWriters[i]) {
      if (sorted.size()==0 || fWriters[i]->GetTotalMemorySize()<=fWriters[sorted[0]]->GetTotalMemorySize()) {
	sorted.push_back(i);
      } else {
	sorted.insert(sorted.begin(), i);
      }
    }
  }
  fReservedWriter=-1;

  vector<int>::iterator ddlno=sorted.begin();
  while (ddlno!=sorted.end()) {
    const AliHLTUInt8_t* pBuffer=NULL;
    int bufferSize=0;
    
    if ((bufferSize=FillOutputBuffer(eventNo, fWriters[*ddlno], pBuffer))>0) {
      if (fgOptions&kWriteDigits) WriteDigitArray(*ddlno, pBuffer, bufferSize);
      if (fgOptions&kWriteRawFiles) WriteRawFile(eventNo, runLoader, *ddlno, pBuffer, bufferSize);
    }
    fWriters[*ddlno]->Clear();
    ddlno++;
  }
  if (fgOptions&kWriteDigits) WriteDigits(eventNo, runLoader);
  return iResult;
}

int AliHLTOUTComponent::ShuffleWriters(AliHLTMonitoringWriterPVector &list, AliHLTUInt32_t /*size*/)
{
  // see header file for class documentation
  int iResult=-ENOENT;
  assert(list.size()>0);
  if (list.size()==0) return iResult;
  vector<int> writers;
  size_t i=0;
  for (i=0; i<list.size(); i++) {
    if ((int)i==fReservedWriter) continue;
    if (list[i]->GetTotalMemorySize()==0)
      writers.push_back(i);
    else if (iResult<0 ||
	     list[i]->GetTotalMemorySize()<list[iResult]->GetTotalMemorySize())
      iResult=i;
      
  }
  if (writers.size()>0) {
    iResult=writers[0];
    if (writers.size()>0) {
      // shuffle among the empty writers
      TDatime dt;
      TRandom rand;
      rand.SetSeed(dt.Get()*(iResult+1));
      i=rand.Integer(writers.size()-1);
      assert(i>0 && i<writers.size()-1);
      iResult=writers[i];
    }
  } else {
    // take the writer with the least data volume
    assert(iResult>=0);
  }
  return iResult;
}

int AliHLTOUTComponent::FillOutputBuffer(int eventNo, AliHLTMonitoringWriter* pWriter, const AliHLTUInt8_t* &pBuffer)
{
  // see header file for class documentation
  int iResult=0;
  unsigned int bufferSize=0;

  // space for common data header
  bufferSize+=sizeof(AliRawDataHeader);
  assert(sizeof(AliRawDataHeader)==32);

  // space for HLT event header
  bufferSize+=sizeof(AliHLTOUT::AliHLTOUTEventHeader);

  // space for payload from the writer
  if (pWriter) bufferSize+=pWriter->GetTotalMemorySize();

  // payload data must be aligned to 32bit
  bufferSize=(bufferSize+3)/4;
  bufferSize*=4;

  if (bufferSize>fBuffer.size())
    fBuffer.resize(bufferSize);

  // reset the last 32bit word, rest will be overwritten
  memset(&fBuffer[bufferSize-4], 0, 4);

  if (bufferSize<=fBuffer.size()) {
    AliRawDataHeader* pCDH=reinterpret_cast<AliRawDataHeader*>(&fBuffer[0]);
    AliHLTOUT::AliHLTOUTEventHeader* pHLTH=reinterpret_cast<AliHLTOUT::AliHLTOUTEventHeader*>(&fBuffer[sizeof(AliRawDataHeader)]);
    *pCDH = AliRawDataHeader();  // Fill with default values.
    memset(pHLTH, 0, sizeof(AliHLTOUT::AliHLTOUTEventHeader));

    if (pWriter) {
      // copy payload
      pWriter->Copy(&fBuffer[sizeof(AliRawDataHeader)+sizeof(AliHLTOUT::AliHLTOUTEventHeader)], 0, 0, 0, 0);
      pHLTH->fLength=pWriter->GetTotalMemorySize();
      // set status bit to indicate HLT payload
      pCDH->fStatusMiniEventID|=0x1<<(AliHLTOUT::kCDHStatusFlagsOffset+AliHLTOUT::kCDHFlagsHLTPayload);
    }
    pHLTH->fLength+=sizeof(AliHLTOUT::AliHLTOUTEventHeader);
    // pHLTH->fEventIDLow is already set to zero in memset above.
    pHLTH->fEventIDLow = eventNo;
    // version does not really matter since we do not add decision data
    pHLTH->fVersion=AliHLTOUT::kVersion1;

    pCDH->fSize=sizeof(AliRawDataHeader)+pHLTH->fLength;
    pCDH->fStatusMiniEventID|=0x1<<(AliHLTOUT::kCDHStatusFlagsOffset + AliHLTOUT::kCDHFlagsHLTPayload);
    
    pBuffer=&fBuffer[0];
    iResult=(int)bufferSize;
  } else {
    pBuffer=NULL;
    iResult=-ENOMEM;
  }

  return iResult;
}

int AliHLTOUTComponent::WriteDigitArray(int hltddl, const AliHLTUInt8_t* pBuffer, unsigned int bufferSize)
{
  // see header file for class documentation
  int iResult=0;
  assert(hltddl<fNofDDLs);
  if (hltddl>=fNofDDLs) return -ERANGE;

  if (!fppDigitArrays) {
    fppDigitArrays=new TArrayC*[fNofDDLs];
    if (fppDigitArrays) {
      for (int i=0; i<fNofDDLs; i++) {
	fppDigitArrays[i]=new TArrayC(0);
      }
    }
  }
  if (fppDigitArrays && fppDigitArrays[hltddl]) {
    fppDigitArrays[hltddl]->Set(bufferSize, reinterpret_cast<const Char_t*>(pBuffer));
  } else {
    iResult=-ENOMEM;    
  }
  return iResult;
}

int AliHLTOUTComponent::WriteDigits(int /*eventNo*/, AliRunLoader* /*runLoader*/)
{
  // see header file for class documentation
  int iResult=0;
  const char* digitFileName="HLT.Digits.root";
  if (!fpDigitFile) {
    fpDigitFile=new TFile(digitFileName, "RECREATE");
  }
  if (fpDigitFile && !fpDigitFile->IsZombie()) {
    if (!fpDigitTree) {
      fpDigitTree=new TTree("rawhltout","HLTOUT raw data");
      if (fpDigitTree && fppDigitArrays) {
	for (int i=0; i<fNofDDLs; i++) {
	  const char* branchName=AliDAQ::DdlFileName("HLT", i);
	  if (fppDigitArrays[i]) fpDigitTree->Branch(branchName, "TArrayC", &fppDigitArrays[i], 32000/*just as the default*/, 0);
	}
      }
    }
    if (fpDigitTree) {
      int res=fpDigitTree->Fill();
      HLTDebug("writing digit tree: %d", res);
      fpDigitFile->cd();
      res=fpDigitTree->Write("",TObject::kOverwrite);
      HLTDebug("writing digit tree: %d", res);
      if (fppDigitArrays) for (int i=0; i<fNofDDLs; i++) {
	if (fppDigitArrays[i]) fppDigitArrays[i]->Set(0);
      }
    }
  } else {
    const char* errorMsg="";
    if (GetEventCount()==5) {
      errorMsg=" (suppressing further error messages)";
    }
    if (GetEventCount()<5) {
      HLTError("can not open HLT digit file %s%s", digitFileName, errorMsg);
    }
    iResult=-EBADF;
  }
  return iResult;
}

int AliHLTOUTComponent::WriteRawFile(int eventNo, AliRunLoader* /*runLoader*/, int hltddl, const AliHLTUInt8_t* pBuffer, unsigned int bufferSize)
{
  // see header file for class documentation
  int iResult=0;
  const char* fileName=AliDAQ::DdlFileName("HLT", hltddl);
  assert(fileName!=NULL);
  TString filePath;
  filePath.Form("raw%d/", eventNo);
  filePath+=fileName;
  if (fileName) {
    ios::openmode filemode=(ios::openmode)0;
    ofstream rawfile(filePath.Data(), filemode);
    if (rawfile.good()) {
      if (pBuffer && bufferSize>0) {
	rawfile.write(reinterpret_cast<const char*>(pBuffer), bufferSize);
      } else {
	HLTWarning("writing zero length raw data file %s");
      }
      HLTDebug("wrote %d byte(s) to file %s", bufferSize, filePath.Data());
    } else {
      HLTError("can not open file %s for writing", filePath.Data());
      iResult=-EBADF;
    }
    rawfile.close();
  }
  return iResult;
}

void AliHLTOUTComponent::SetGlobalOption(unsigned int options)
{
  // see header file for class documentation
  fgOptions|=options;
}

void AliHLTOUTComponent::ClearGlobalOption(unsigned int options)
{
  // see header file for class documentation
  fgOptions&=~options;
}

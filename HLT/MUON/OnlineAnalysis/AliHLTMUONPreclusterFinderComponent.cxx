/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Philippe Pillot <pillot@subatech.in2p3.fr>                           *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$

///
///  @file   AliHLTMUONPreclusterFinderComponent.cxx
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   14 October 2014
///  @brief  Implementation of precluster reconstruction processing component for the dimuon HLT.
///

// ROOT
#include <TSystem.h>

// HLT MUON
#include "AliHLTMUONPreclusterFinderComponent.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTMUONUtils.h"

// MUON
#include "AliMUONCDB.h"
#include "AliMUONConstants.h"
#include "AliMUONVDigit.h"
#include "AliMUONVDigitStore.h"

// MUON mapping
#include "AliMpDDLStore.h"
#include "AliMpCDB.h"
#include "AliMpDEIterator.h"
#include "AliMpVSegmentation.h"
#include "AliMpSegmentation.h"
#include "AliMpVPadIterator.h"
#include "AliMpPad.h"
#include "AliMpConstants.h"

// HLT
#include "AliHLTCDHWrapper.h"

// STEER
#include "AliCDBManager.h"
#include "AliCodeTimer.h"


ClassImp(AliHLTMUONPreclusterFinderComponent)


//_________________________________________________________________________________________________
AliHLTMUONPreclusterFinderComponent::AliHLTMUONPreclusterFinderComponent() :
AliHLTProcessor(),
fRawDecoder(),
fBadEvent(kFALSE),
fDEIndices(),
fPreClusterBlock()
{
  /// Default constructor
  fRawDecoder.GetHandler().SetParent(this);
}

//_________________________________________________________________________________________________
AliHLTMUONPreclusterFinderComponent::~AliHLTMUONPreclusterFinderComponent()
{
  /// Default destructor
}

//_________________________________________________________________________________________________
const char* AliHLTMUONPreclusterFinderComponent::GetComponentID()
{
  /// Inherited from AliHLTComponent. Returns the component ID.
  return "MUONPreclusterFinder";
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  /// Inherited from AliHLTComponent. Returns the list of expected input data types.
  list.clear();
  list.push_back(AliHLTMUONConstants::DDLRawDataType());
  list.push_back(AliHLTMUONConstants::DigitStoreDataType());
}

//_________________________________________________________________________________________________
AliHLTComponentDataType AliHLTMUONPreclusterFinderComponent::GetOutputDataType()
{
  /// Inherited from AliHLTComponent. Returns the output data type.
  return AliHLTMUONConstants::PreClustersBlockDataType();
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  /// Inherited from AliHLTComponent. Returns an estimate of the expected output data size.
  constBase = 1024*1024;
  inputMultiplier = 1;
}

//_________________________________________________________________________________________________
AliHLTComponent* AliHLTMUONPreclusterFinderComponent::Spawn()
{
  /// Inherited from AliHLTComponent. Creates a new object instance.
  return new AliHLTMUONPreclusterFinderComponent;
}

//_________________________________________________________________________________________________
int AliHLTMUONPreclusterFinderComponent::DoInit(int argc, const char** argv)
{
  /// Inherited from AliHLTComponent.
  /// Parses the command line parameters and initialises the component.
  
  HLTInfo("Initialising dHLT precluster finder component.");
  
  AliCodeTimer::Instance()->Reset();
  
  const char* cdbPath = NULL;
  Int_t run = -1;
  
  for (int i = 0; i < argc; i++) {
    
    // get the CDB path
    if (strcmp(argv[i], "-cdbpath") == 0) {
      
      if (cdbPath != NULL) HLTWarning("CDB path was already specified. Will replace previous value given by -cdbpath.");
      
      if (argc <= i+1) {
        HLTError("The CDB path was not specified.");
        return -EINVAL;
      }
      
      cdbPath = argv[i+1];
      i++;
      continue;
      
    }
    
    // get the run number
    if (strcmp(argv[i], "-run") == 0) {
      
      if (run != -1) HLTWarning("Run number was already specified. Will replace previous value given by -run.");
      
      if (argc <= i+1) {
        HLTError("The run number was not specified.");
        return -EINVAL;
      }
      
      char* cpErr = NULL;
      run = Int_t( strtol(argv[i+1], &cpErr, 0) );
      if (cpErr == NULL or *cpErr != '\0' or run < 0) {
        HLTError("Cannot convert '%s' to a valid run number. Expected a positive integer value.", argv[i+1]);
        return -EINVAL;
      }
      
      i++;
      continue;
      
    }
    
  }
  
  AliCDBManager* cdbManager = AliCDBManager::Instance();
  if (cdbManager == NULL) {
    HLTError("CDB manager instance does not exist.");
    return -EIO;
  }
  
  // Set the CDB path
  if (cdbPath == NULL) {
    HLTError("The CDB path was not set.");
    return -EINVAL;
  } else {
    cdbManager->SetDefaultStorage(cdbPath);
    if (!cdbManager->IsDefaultStorageSet()) {
      HLTError("Invalid CDB path: %s.", cdbPath);
      return -EINVAL;
    }
  }
  
  // Set the run number
  if (run == -1) {
    HLTError("The run number was not set.");
    return -EINVAL;
  } else {
    cdbManager->SetRun(run);
  }
  
  // Load the MUON mapping from CDB and create the internal mapping
  // MUON mapping also needed for raw data decoding
  if (!AliMUONCDB::LoadMapping()) return -EIO;
  CreateMapping();
  
  // prepare storage of preclusters
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      fPreClusters[iDE][iPlane].reserve(100);
    }
  }
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONPreclusterFinderComponent::DoDeinit()
{
  /// Inherited from AliHLTComponent. Performs a cleanup of the component.
  
  HLTInfo("Deinitialising dHLT precluster finder component.");
  
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    
    mpDE &de(fMpDEs[iDE]);
    
    delete[] de.pads;
    
    for (UShort_t iDigit = 0; iDigit < static_cast<UShort_t>(de.digits.size()); ++iDigit)
      delete de.digits[iDigit];
    
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      for (UShort_t iCluster = 0; iCluster < static_cast<UShort_t>(fPreClusters[iDE][iPlane].size()); ++iCluster) {
        delete fPreClusters[iDE][iPlane][iCluster];
      }
    }
    
  }
  
  AliMpCDB::UnloadAll();
  
  AliCodeTimer::Instance()->Print();
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONPreclusterFinderComponent::DoEvent(const AliHLTComponentEventData& /*evtData*/,
                                                 const AliHLTComponentBlockData* /*blocks*/,
                                                 AliHLTComponentTriggerData& /*trigData*/,
                                                 AliHLTUInt8_t* outputPtr,
                                                 AliHLTUInt32_t& size,
                                                 AliHLTComponentBlockDataList& /*outputBlocks*/)
{
  /// Inherited from AliHLTProcessor. Processes the new event data.
  
  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return 0;
  
  fBadEvent = kFALSE;
  Bool_t inBlocks = kFALSE;
  
  // reset fired pad and precluster information
  ResetPadsAndPreclusters();
  
  // loop over blocks of the data type "DDLRaw"
  const AliHLTComponentBlockData* pBlock = GetFirstInputBlock(AliHLTMUONConstants::DDLRawDataType());
  while (pBlock) {
    
    // get the pointer to the begining and the size of the raw data block (after the CDH)
    AliHLTCDHWrapper cdh(pBlock->fPtr);
    AliHLTUInt32_t headerSize = cdh.GetHeaderSize();
    AliHLTUInt32_t totalDDLSize = pBlock->fSize;
    AliHLTUInt32_t ddlRawDataSize = totalDDLSize - headerSize;
    AliHLTUInt32_t *buffer = reinterpret_cast<AliHLTUInt32_t*>(pBlock->fPtr) + headerSize/sizeof(AliHLTUInt32_t);
    
    // decode the block to get the fired pads
    fRawDecoder.Decode(buffer,ddlRawDataSize);
    if (fBadEvent) {
      HLTError("Error found while decoding the raw data block.");
      return -EFTYPE;
    }
    
    inBlocks = kTRUE;
    
    pBlock = GetNextInputBlock();
    
  }
  
  // loop over objects in blocks of data type "DigitStore"
  const TObject* pObj = GetFirstInputObject(AliHLTMUONConstants::DigitStoreDataType());
  while (pObj) {
    
    // make sure the block indeed contains a digit store
    const AliMUONVDigitStore *pDigitStore = dynamic_cast<const AliMUONVDigitStore*>(pObj);
    if (! pDigitStore) {
      HLTError("The data block of type \"DigitStore\" does not contains a digit store.");
      return -EFTYPE;
    }
    
    // load the digits to get the fired pads
    LoadDigits(pDigitStore);
    
    inBlocks = kTRUE;
    
    pObj = GetNextInputObject();
    
  }
  
  if (inBlocks) {
    
    // preclusterize each cathod separately then merge them
    PreClusterizeRecursive();
    MergePreClusters();
    
    // store preclusters in a cluster store
    Int_t status = StorePreClusters(outputPtr, size);
    if (status < 0) return status;
    
  }
  
  return 0;
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::CreateMapping()
{
  /// fill the mpDE and mpPad structures once for all
  
  AliCodeTimerAutoGeneral("",0);
  
  MemInfo_t memBefore, memAfter;
  ProcInfo_t procBefore, procAfter;
  gSystem->GetMemInfo(&memBefore);
  gSystem->GetProcInfo(&procBefore);
  
  // loop over DEs
  UChar_t iDE = 0;
  UInt_t padId = 0;
  for (Int_t iCh = 0; iCh < AliMUONConstants::NTrackingCh(); ++iCh) {
    AliMpDEIterator deIt;
    deIt.First(iCh);
    while (!deIt.IsDone()) {
      
      Int_t deId = deIt.CurrentDEId();
      mpDE &de = fMpDEs[iDE];
      de.id = deId;
      fDEIndices.Add(deId, iDE);
      
      const AliMpVSegmentation* seg[2] =
      { AliMpSegmentation::Instance()->GetMpSegmentation(deId,AliMp::kCath0),
        AliMpSegmentation::Instance()->GetMpSegmentation(deId,AliMp::kCath1)
      };
      
      UChar_t iPlane[2];
      if (seg[0]->PlaneType() == AliMp::kNonBendingPlane) {
        iPlane[0] = 1;
        iPlane[1] = 0;
      } else {
        iPlane[0] = 0;
        iPlane[1] = 1;
      }
      
      de.nPads[iPlane[0]] = seg[0]->NofPads();
      de.nPads[iPlane[1]] = seg[1]->NofPads();
      de.pads = new mpPad[de.nPads[0]+de.nPads[1]];
      de.digits.reserve((de.nPads[0]/10+de.nPads[1]/10)); // 10% occupancy
      de.nOrderedPads[0] = 0;
      de.orderedPads[0].reserve((de.nPads[0]/10+de.nPads[1]/10)); // 10% occupancy
      de.nOrderedPads[1] = 0;
      de.orderedPads[1].reserve((de.nPads[0]/10+de.nPads[1]/10)); // 10% occupancy
      
      // loop over cathods
      for (UChar_t iCath = 0; iCath < 2; ++iCath) {
        
        de.iCath[iPlane[iCath]] = iCath;
        de.nFiredPads[iPlane[iCath]] = 0;
        de.firedPads[iPlane[iCath]].reserve(de.nPads[iPlane[iCath]]/10); // 10% occupancy
        
        // loop over pads to associate an index to their Id
        UShort_t iPad = (iPlane[iCath] == 0) ? 0 : de.nPads[0];
        AliMpVPadIterator* padIt = seg[iCath]->CreateIterator();
        padIt->First();
        while (!padIt->IsDone()) {
          
          AliMpPad pad = padIt->CurrentItem();
          
          padId = AliMUONVDigit::BuildUniqueID(deId, pad.GetManuId(), pad.GetManuChannel(), iCath);
          de.padIndices[iPlane[iCath]].Add(padId, iPad+1);
          
          de.pads[iPad].iDigit = 0;
          de.pads[iPad].nNeighbours = 0;
          de.pads[iPad].area[0][0] = pad.GetPositionX() - pad.GetDimensionX();
          de.pads[iPad].area[0][1] = pad.GetPositionX() + pad.GetDimensionX();
          de.pads[iPad].area[1][0] = pad.GetPositionY() - pad.GetDimensionY();
          de.pads[iPad].area[1][1] = pad.GetPositionY() + pad.GetDimensionY();
          de.pads[iPad].useMe = kFALSE;
          
          ++iPad;
          padIt->Next();
          
        }
        
        FindNeighbours(de, iPlane[iCath]);
        
        delete padIt;
        
      }
      
      ++iDE;
      deIt.Next();
      
    }
    
  }
  
  gSystem->GetMemInfo(&memAfter);
  gSystem->GetProcInfo(&procAfter);
  printf("Memory comsumption for mapping: %d MB\n", memAfter.fMemUsed - memBefore.fMemUsed);
  printf("Memory comsumption for mapping: %d MB\n", memBefore.fMemFree - memAfter.fMemFree);
  printf("Memory comsumption for mapping: %ld MB\n", (procAfter.fMemResident - procBefore.fMemResident + 500) / 1000);
  printf("Memory comsumption for mapping: %ld MB\n", (procAfter.fMemVirtual - procBefore.fMemVirtual + 500) / 1000);
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::FindNeighbours(mpDE &de, UChar_t iPlane)
{
  /// fill the mpPad neighbours structures of given DE/plane
  
  AliCodeTimerAutoGeneral("",0);
  
  UShort_t firstPad = (iPlane == 0) ? 0 : de.nPads[0];
  UShort_t lastPad = firstPad + de.nPads[iPlane];
  
  // loop over pads
  for (UShort_t iPad1 = firstPad; iPad1 < lastPad; ++iPad1) {
    
    // loop over next pads to find the neighbours
    for (UShort_t iPad2 = iPad1+1; iPad2 < lastPad; ++iPad2) {
      
      if (AreOverlapping(de.pads[iPad1].area, de.pads[iPad2].area, 1.e-4)) {
        
        de.pads[iPad1].neighbours[de.pads[iPad1].nNeighbours] = iPad2;
        ++de.pads[iPad1].nNeighbours;
        de.pads[iPad2].neighbours[de.pads[iPad2].nNeighbours] = iPad1;
        ++de.pads[iPad2].nNeighbours;
        
      }
      
    }
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::LoadDigits(const AliMUONVDigitStore* digitStore)
{
  /// fill the mpDE structure with fired pads
  
  AliCodeTimerAutoGeneral("",0);
  
  UChar_t iPlane(0);
  UShort_t iPad(0);
  
  // loop over digits
  AliMUONVDigit* digit = 0x0;
  TIter nextDigit(digitStore->CreateIterator());
  while ((digit = static_cast<AliMUONVDigit*>(nextDigit()))) {
    
    if (digit->Charge() <= 0) continue;
    
    mpDE &de(fMpDEs[fDEIndices.GetValue(digit->DetElemId())]);
    iPlane = (digit->Cathode() == de.iCath[0]) ? 0 : 1;
    iPad = de.padIndices[iPlane].GetValue(digit->GetUniqueID());
    if (iPad == 0) {
      HLTWarning("pad ID %u does not exist in the mapping");
      continue;
    }
    --iPad;
    
    // register this digit
    UShort_t iDigit = de.nFiredPads[0] + de.nFiredPads[1];
    if (iDigit >= static_cast<UShort_t>(de.digits.size())) de.digits.push_back(new AliHLTMUONChannelStruct);
    AliHLTMUONChannelStruct *digit2 = de.digits[iDigit];
    digit2->fSignal = static_cast<UShort_t>(digit->ADC());
    digit2->fRawDataWord = digit->GetUniqueID();
    de.pads[iPad].iDigit = iDigit;
    de.pads[iPad].useMe = kTRUE;
    
    // set this pad as fired
    if (de.nFiredPads[iPlane] < static_cast<UShort_t>(de.firedPads[iPlane].size()))
      de.firedPads[iPlane][de.nFiredPads[iPlane]] = iPad;
    else de.firedPads[iPlane].push_back(iPad);
    ++de.nFiredPads[iPlane];
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::ResetPadsAndPreclusters()
{
  /// reset fired pad and precluster information
  
  AliCodeTimerAutoGeneral("",0);
  
  mpPad *pad(0x0);
  
  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    
    mpDE &de(fMpDEs[iDE]);
    
    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      
      // clear number of preclusters
      fNPreClusters[iDE][iPlane] = 0;
      
      // loop over fired pads
      for (UShort_t iFiredPad = 0; iFiredPad < de.nFiredPads[iPlane]; ++iFiredPad) {
        
        pad = &de.pads[de.firedPads[iPlane][iFiredPad]];
        pad->iDigit = 0;
        pad->useMe = kFALSE;
        
      }
      
      // clear number of fired pads
      de.nFiredPads[iPlane] = 0;
      
    }
    
    // clear ordered number of fired pads
    de.nOrderedPads[0] = 0;
    de.nOrderedPads[1] = 0;
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::PreClusterizeRecursive()
{
  /// preclusterize both planes of every DE using recursive algorithm
  
  AliCodeTimerAutoGeneral("",0);
  
  preCluster *cl(0x0);
  UShort_t iPad(0);
  
  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    
    mpDE &de(fMpDEs[iDE]);
    
    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      
      // loop over fired pads
      for (UShort_t iFiredPad = 0; iFiredPad < de.nFiredPads[iPlane]; ++iFiredPad) {
        
        iPad = de.firedPads[iPlane][iFiredPad];
        
        if (de.pads[iPad].useMe) {
          
          // create the precluster if needed
          if (fNPreClusters[iDE][iPlane] >= static_cast<UShort_t>(fPreClusters[iDE][iPlane].size()))
            fPreClusters[iDE][iPlane].push_back(new preCluster);
          
          // get the precluster
          cl = fPreClusters[iDE][iPlane][fNPreClusters[iDE][iPlane]];
          ++fNPreClusters[iDE][iPlane];
          
          // reset its content
          cl->area[0][0] = 1.e6;
          cl->area[0][1] = -1.e6;
          cl->area[1][0] = 1.e6;
          cl->area[1][1] = -1.e6;
          cl->useMe = kTRUE;
          cl->storeMe = kFALSE;
          
          // add the pad and its fired neighbours recusively
          cl->firstPad = de.nOrderedPads[0];
          AddPad(de, iPad, *cl);
          
        }
        
      }
      
    }
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::AddPad(mpDE &de, UShort_t iPad, preCluster &cl)
{
  /// add the given mpPad and its fired neighbours (recursive method)
  
  // add the given pad
  mpPad &pad(de.pads[iPad]);
  if (de.nOrderedPads[0] < static_cast<UShort_t>(de.orderedPads[0].size()))
    de.orderedPads[0][de.nOrderedPads[0]] = iPad;
  else de.orderedPads[0].push_back(iPad);
  cl.lastPad = de.nOrderedPads[0];
  ++de.nOrderedPads[0];
  if (pad.area[0][0] < cl.area[0][0]) cl.area[0][0] = pad.area[0][0];
  if (pad.area[0][1] > cl.area[0][1]) cl.area[0][1] = pad.area[0][1];
  if (pad.area[1][0] < cl.area[1][0]) cl.area[1][0] = pad.area[1][0];
  if (pad.area[1][1] > cl.area[1][1]) cl.area[1][1] = pad.area[1][1];
  
  pad.useMe = kFALSE;
  
  // loop over its neighbours
  for (UShort_t iNeighbour = 0; iNeighbour < pad.nNeighbours; ++iNeighbour) {
    
    if (de.pads[pad.neighbours[iNeighbour]].useMe) {
      
      // add the pad to the precluster
      AddPad(de, pad.neighbours[iNeighbour], cl);
      
    }
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::MergePreClusters()
{
  /// merge overlapping preclusters on every DE
  
  AliCodeTimerAutoGeneral("",0);
  
  preCluster *cl(0x0);
  
  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    
    mpDE &de(fMpDEs[iDE]);
    
    // loop over preclusters of one plane
    for (UShort_t iCluster = 0; iCluster < fNPreClusters[iDE][0]; ++iCluster) {
      
      if (!fPreClusters[iDE][0][iCluster]->useMe) continue;
      
      cl = fPreClusters[iDE][0][iCluster];
      cl->useMe = kFALSE;
      
      // look for overlapping preclusters in the other plane
      preCluster *mergedCl(0x0);
      MergePreClusters(*cl, fPreClusters[iDE], fNPreClusters[iDE], de, 1, mergedCl);
      
      // add the current one
      if (!mergedCl) mergedCl = UsePreClusters(cl, de);
      else MergePreClusters(*mergedCl, *cl, de);
      
    }
    
    // loop over preclusters of the other plane
    for (UShort_t iCluster = 0; iCluster < fNPreClusters[iDE][1]; ++iCluster) {
      
      if (!fPreClusters[iDE][1][iCluster]->useMe) continue;
      
      // all remaining preclusters have to be stored
      UsePreClusters(fPreClusters[iDE][1][iCluster], de);
      
    }
    
  }
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::MergePreClusters(preCluster &cl, std::vector<preCluster*> preClusters[2],
                                                           UShort_t nPreClusters[2], mpDE &de, UChar_t iPlane,
                                                           preCluster *&mergedCl)
{
  /// merge preclusters on the given plane overlapping with the given one (recursive method)
  
  preCluster *cl2(0x0);
  
  // loop over preclusters in the given plane
  for (UShort_t iCluster = 0; iCluster < nPreClusters[iPlane]; ++iCluster) {
    
    if (!preClusters[iPlane][iCluster]->useMe) continue;
    
    cl2 = preClusters[iPlane][iCluster];
    if (AreOverlapping(cl.area, cl2->area, -1.e-4) && AreOverlapping(cl, *cl2, de, -1.e-4)) {
      
      cl2->useMe = kFALSE;
      
      // look for new overlapping preclusters in the other plane
      MergePreClusters(*cl2, preClusters, nPreClusters, de, (iPlane+1)%2, mergedCl);
      
      // store overlapping preclusters and merge them
      if (!mergedCl) mergedCl = UsePreClusters(cl2, de);
      else MergePreClusters(*mergedCl, *cl2, de);
      
    }
    
  }
  
}

//_________________________________________________________________________________________________
AliHLTMUONPreclusterFinderComponent::preCluster* AliHLTMUONPreclusterFinderComponent::UsePreClusters(preCluster *cl, mpDE &de)
{
  /// use this precluster as a new merged precluster
  
  UShort_t firstPad = de.nOrderedPads[1];
  
  // move the fired pads
  for (UShort_t iOrderPad = cl->firstPad; iOrderPad <= cl->lastPad; ++iOrderPad) {
    
    if (de.nOrderedPads[1] < static_cast<UShort_t>(de.orderedPads[1].size()))
      de.orderedPads[1][de.nOrderedPads[1]] = de.orderedPads[0][iOrderPad];
    else de.orderedPads[1].push_back(de.orderedPads[0][iOrderPad]);
    
    ++de.nOrderedPads[1];
    
  }
  
  cl->firstPad = firstPad;
  cl->lastPad = de.nOrderedPads[1] - 1;
  
  cl->storeMe = kTRUE;
  
  return cl;
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::MergePreClusters(preCluster &cl1, preCluster &cl2, mpDE &de)
{
  /// merge precluster2 into precluster1
  
  // move the fired pads
  for (UShort_t iOrderPad = cl2.firstPad; iOrderPad <= cl2.lastPad; ++iOrderPad) {
    
    if (de.nOrderedPads[1] < static_cast<UShort_t>(de.orderedPads[1].size()))
      de.orderedPads[1][de.nOrderedPads[1]] = de.orderedPads[0][iOrderPad];
    else de.orderedPads[1].push_back(de.orderedPads[0][iOrderPad]);
    
    ++de.nOrderedPads[1];
    
  }
  
  cl1.lastPad = de.nOrderedPads[1] - 1;
  
}

//_________________________________________________________________________________________________
Bool_t AliHLTMUONPreclusterFinderComponent::AreOverlapping(preCluster &cl1, preCluster &cl2, mpDE &de, Float_t precision)
{
  /// check if the two preclusters overlap
  /// precision in cm: positive = increase pad size / negative = decrease pad size
  
  // loop over all pads of the precluster1
  for (UShort_t iOrderPad1 = cl1.firstPad; iOrderPad1 <= cl1.lastPad; ++iOrderPad1) {
    
    // loop over all pads of the precluster2
    for (UShort_t iOrderPad2 = cl2.firstPad; iOrderPad2 <= cl2.lastPad; ++iOrderPad2) {
      
      if (AreOverlapping(de.pads[de.orderedPads[0][iOrderPad1]].area,
                         de.pads[de.orderedPads[0][iOrderPad2]].area, precision)) return kTRUE;
      
    }
    
  }
  
  return kFALSE;
  
}

//_________________________________________________________________________________________________
Bool_t AliHLTMUONPreclusterFinderComponent::AreOverlapping(Float_t area1[2][2], Float_t area2[2][2], Float_t precision)
{
  /// check if the two areas overlap
  /// precision in cm: positive = increase pad size / negative = decrease pad size
  
  if (area1[0][0] - area2[0][1] > precision) return kFALSE;
  if (area2[0][0] - area1[0][1] > precision) return kFALSE;
  if (area1[1][0] - area2[1][1] > precision) return kFALSE;
  if (area2[1][0] - area1[1][1] > precision) return kFALSE;
  
  return kTRUE;
  
}

//_________________________________________________________________________________________________
Int_t AliHLTMUONPreclusterFinderComponent::StorePreClusters(AliHLTUInt8_t* outputPtr, AliHLTUInt32_t size)
{
  /// store the preclusters into AliMUONVCluster
  
  AliCodeTimerAutoGeneral("",0);
  
  preCluster *cl(0x0);
  AliHLTMUONChannelStruct *digit(0x0);
  AliHLTUInt32_t totalBytesUsed(0);
  Int_t status(0);
  
  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {
    
    if (fNPreClusters[iDE][0] + fNPreClusters[iDE][1] == 0) continue;
    
    status = fPreClusterBlock.Reset(outputPtr+totalBytesUsed, size-totalBytesUsed, true);
    if (status < 0) return status;
    
    mpDE &de(fMpDEs[iDE]);
    
    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      
      // loop over preclusters
      for (UShort_t iCluster = 0; iCluster < fNPreClusters[iDE][iPlane]; ++iCluster) {
        
        if (!fPreClusters[iDE][iPlane][iCluster]->storeMe) continue;
        
        // add the precluster with its first digit
        cl = fPreClusters[iDE][iPlane][iCluster];
        digit = de.digits[de.pads[de.orderedPads[1][cl->firstPad]].iDigit];
        status = fPreClusterBlock.StartPreCluster(*digit);
        if (status < 0) return status;
        
        // loop over other pads and add corresponding digits
        for (UShort_t iOrderPad = cl->firstPad+1; iOrderPad <= cl->lastPad; ++iOrderPad) {
          
          digit = de.digits[de.pads[de.orderedPads[1][iOrderPad]].iDigit];
          status = fPreClusterBlock.AddDigit(*digit);
          if (status < 0) return status;
          
        }
        
      }
      
    }
    
    if (fPreClusterBlock.GetNPreClusters() == 0) continue;
    
    // register the preclusters data block for this DE
    AliHLTUInt32_t bytesUsed(fPreClusterBlock.BytesUsed());
    status = PushBack(outputPtr+totalBytesUsed, bytesUsed, AliHLTMUONConstants::PreClustersBlockDataType(), de.id);
    if (status < 0) return status;
    totalBytesUsed += bytesUsed;
    
  }
  
  return status;
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::RawDecoderHandler::OnData(UInt_t data, bool /*parityError*/)
{
  /// OnData is called for every raw data word found within a bus patch.
  /// Every data ward received by a call to OnData is associated to the bus patch
  /// header received in the most recent call to OnNewBusPatch.
  /// The default behaviour of this method is to do nothing.
  /// - param UInt_t  This is the raw data word as found within the bus patch payload.
  /// - param bool  Flag indicating if the raw data word had a parity error.
  ///       This will always be set to false if fSendDataOnParityError in the
  ///       AliMUONTrackerDDLDecoder class was set to false.
  
  // decode the raw data word
  UShort_t manuId;
  UChar_t manuChannel;
  UShort_t adc;
  UnpackADC(data, manuId, manuChannel, adc);
  
  if (adc <= 0) return;
  
  // get plane index
  AliMp::PlaneType planeType = (manuId & AliMpConstants::ManuMask(AliMp::kNonBendingPlane)) ? AliMp::kNonBendingPlane : AliMp::kBendingPlane;
  UChar_t iPlane = (planeType == AliMp::kNonBendingPlane) ? 1 : 0;
  
  // get cathod index
  mpDE &de(fParent->fMpDEs[fParent->fDEIndices.GetValue(fDetElemId)]);
  UChar_t iCath = de.iCath[iPlane];
  
  // get pad index
  UInt_t padId = AliMUONVDigit::BuildUniqueID(fDetElemId, manuId, manuChannel, iCath);
  UShort_t iPad = de.padIndices[iPlane].GetValue(padId);
  if (iPad == 0) return;
  --iPad;
  
  // register this digit
  UShort_t iDigit = de.nFiredPads[0] + de.nFiredPads[1];
  if (iDigit >= static_cast<UShort_t>(de.digits.size())) de.digits.push_back(new AliHLTMUONChannelStruct);
  AliHLTMUONChannelStruct *digit = de.digits[iDigit];
  digit->fSignal = adc;
  digit->fRawDataWord = padId;
  de.pads[iPad].iDigit = iDigit;
  de.pads[iPad].useMe = kTRUE;
  
  // set this pad as fired
  if (de.nFiredPads[iPlane] < static_cast<UShort_t>(de.firedPads[iPlane].size()))
    de.firedPads[iPlane][de.nFiredPads[iPlane]] = iPad;
  else de.firedPads[iPlane].push_back(iPad);
  ++de.nFiredPads[iPlane];
  
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::RawDecoderHandler::OnNewBusPatch(const AliMUONBusPatchHeaderStruct* header, const void* /*data*/)
{
  /// OnNewBusPatch is called whenever a new bus patch header is found in
  /// the payload. Every bus patch received by a call to OnNewBusPatch is
  /// associated to the DSP header received in the most recent call to OnNewDSP.
  /// The default behaviour of this method is to do nothing.
  /// - param const AliMUONBusPatchHeaderStruct*  This is a pointer to the bus patch
  ///                header as found in the DDL payload.
  /// - param const void*  This is a pointer to the start of the bus patch's contents,
  ///              specifically the raw data words.
  /// Note: both pointers point into the memory buffer being parsed, so the
  /// contents must not be modified. On the other hand this is very efficient
  /// because no memory copying is required.
  
  // get the detection element ID containing this bus patch
  fDetElemId = AliMpDDLStore::Instance()->GetDEfromBus(header->fBusPatchId);
}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::RawDecoderHandler::OnError(ErrorCode /*error*/, const void* /*location*/)
{
  /// Whenever a parsing error of the DDL payload is encountered because of
  /// corruption of the raw data (eg. bit flips) the OnError method is called
  /// immediately at the point this error is discovered.
  /// The default behaviour of this method is to do nothing.
  /// - param ErrorCode  This is an error code indicating the kind of problem
  ///               encountered with the DDL payload.
  /// - param const void*  This is a pointer into the DDL payload memory buffer
  ///         indicating the exact location where the parsing error happened
  ///         or i.e. the location of the corruption.
  /// Note that a relative offset in bytes from the start of the memory buffer
  /// can be calculated by: storing the buffer pointer received in OnNewBuffer
  /// earlier in fBufferStart for example, and then the offset is given by:
  ///   offset = (unsigned long)location - (unsigned long)fBufferStart;
  fParent->fBadEvent = kTRUE;
}


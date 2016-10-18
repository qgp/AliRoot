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
#include "AliMUONConstants.h"
#include "AliMUONVDigit.h"

// STEER
#include "AliCodeTimer.h"


ClassImp(AliHLTMUONPreclusterFinderComponent)

//_________________________________________________________________________________________________
AliHLTMUONPreclusterFinderComponent::AliHLTMUONPreclusterFinderComponent()
{}

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
  list.push_back(AliHLTMUONConstants::DigitBlockDataType());
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
  constBase = 0;
  inputMultiplier = 2;
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

  std::string binmapfile;

  for (int i = 0; i < argc; i++) {

    if (strcmp(argv[i],"-binmapfile") == 0 ) {
      if (argc <= i+1) {
        HLTError("The binary mapping file was not specified.");
        return -EINVAL;
      }

      binmapfile = gSystem->ExpandPathName(argv[i+1]);
      continue;
    }
  }

  // Load the mapping from the binary file
  if (!ReadMapping(binmapfile.c_str())) {
    HLTError("Could not read binary mapping file %s",binmapfile.c_str());
    return -EINVAL;
  }

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

    digiPads& de(fMpDEs[iDE]);

    delete[] de.mapping->pads;

    for (UShort_t iDigit = 0; iDigit < static_cast<UShort_t>(de.ownDigits.size()); ++iDigit) {
      delete de.ownDigits[iDigit];
    }

    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {
      for (UShort_t iCluster = 0; iCluster < static_cast<UShort_t>(fPreClusters[iDE][iPlane].size()); ++iCluster) {
        delete fPreClusters[iDE][iPlane][iCluster];
      }
    }

  }

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

  AliCodeTimerAutoGeneral("",0);

  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return 0;

  Bool_t inBlocks = kFALSE;

  // reset fired pad and precluster information
  ResetPadsAndPreclusters();

  // loop over objects in blocks of data type "DIGITS"
  AliCodeTimerStartGeneral("LoadDigits");
  const AliHLTComponentBlockData* pBlock = GetFirstInputBlock(AliHLTMUONConstants::DigitBlockDataType());
  while (pBlock) {

    AliHLTMUONDigitsBlockReader dblock(pBlock->fPtr, pBlock->fSize);

    // make sure the block indeed contains a valid digits block
    if (!dblock.BufferSizeOk()) {
      HLTError("The buffer size does not match the expected size of the digit block.");
      return -EILSEQ;
    }
    AliHLTMUONUtils::WhyNotValid reason;
    if (!AliHLTMUONUtils::HeaderOk(dblock.BlockHeader(), &reason)) {
      HLTError("Invalid digit block: %s", AliHLTMUONUtils::FailureReasonToMessage(reason));
      #if __APPLE__
      return -EFTYPE;
      #else
      return -ENODATA;
      #endif
    }

    // load the digits to get the fired pads
    LoadDigits(dblock);

    inBlocks = kTRUE;

    pBlock = GetNextInputBlock();

  }
  AliCodeTimerStopGeneral("LoadDigits");

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
void AliHLTMUONPreclusterFinderComponent::LoadDigits(const AliHLTMUONDigitsBlockReader &dblock)
{
  /// fill the AliMUONHLTMapping::mpDE structure with fired pads

  UChar_t iPlane(0);
  UShort_t iPad(0);

  // loop over digits
  for (AliHLTUInt32_t i = 0; i < dblock.Nentries(); ++i) {

    const AliHLTMUONDigitStruct &digit(dblock[i]);

    Long64_t deIndex = fDEIndices.GetValue(AliMUONVDigit::DetElemId(digit.fId));

    assert(deIndex>=0 && deIndex<fgkNDEs);

    digiPads &de(fMpDEs[deIndex]);

    iPlane = (AliMUONVDigit::Cathode(digit.fId) == de.mapping->iCath[0]) ? 0 : 1;
    iPad = de.mapping->padIndices[iPlane].GetValue(digit.fId);
    if (iPad == 0) {
      HLTWarning("pad ID %u does not exist in the mapping");
      continue;
    }
    --iPad;

    // register this digit
    UShort_t iDigit = de.nFiredPads[0] + de.nFiredPads[1];
    if (iDigit >= static_cast<UShort_t>(de.digits.size())) {
      de.digits.push_back(&digit);
    }
    else {
      de.digits[iDigit] = &digit;
    }
    de.mapping->pads[iPad].iDigit = iDigit;
    de.mapping->pads[iPad].useMe = kTRUE;

    // set this pad as fired
    if (de.nFiredPads[iPlane] < static_cast<UShort_t>(de.firedPads[iPlane].size()))
    {
      de.firedPads[iPlane][de.nFiredPads[iPlane]] = iPad;
    }
    else {
      de.firedPads[iPlane].push_back(iPad);
    }
    ++de.nFiredPads[iPlane];

  }

}

//_________________________________________________________________________________________________
void AliHLTMUONPreclusterFinderComponent::ResetPadsAndPreclusters()
{
  /// reset fired pad and precluster information

  AliCodeTimerAutoGeneral("",0);

  AliHLTMUONMapping::mpPad *pad(0x0);

  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {

    digiPads& de(fMpDEs[iDE]);

    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {

      // clear number of preclusters
      fNPreClusters[iDE][iPlane] = 0;

      // loop over fired pads
      for (UShort_t iFiredPad = 0; iFiredPad < de.nFiredPads[iPlane]; ++iFiredPad) {

        pad = &de.mapping->pads[de.firedPads[iPlane][iFiredPad]];
        pad->iDigit = 0;
        pad->useMe = kFALSE;

      }

      // clear number of fired pads
      de.nFiredPads[iPlane] = 0;

    }

    // clear number of digits produced when decoding raw data
    de.nOwnDigits = 0;

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

    digiPads& de(fMpDEs[iDE]);

    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {

      // loop over fired pads
      for (UShort_t iFiredPad = 0; iFiredPad < de.nFiredPads[iPlane]; ++iFiredPad) {

        iPad = de.firedPads[iPlane][iFiredPad];

        if (de.mapping->pads[iPad].useMe) {

          // create the precluster if needed
          if (fNPreClusters[iDE][iPlane] >= static_cast<UShort_t>(fPreClusters[iDE][iPlane].size())) {
            fPreClusters[iDE][iPlane].push_back(new preCluster);
          }

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
void AliHLTMUONPreclusterFinderComponent::AddPad(digiPads &de, UShort_t iPad, preCluster &cl)
{
  /// add the given mpPad and its fired neighbours (recursive method)

  // add the given pad
  AliHLTMUONMapping::mpPad* pads = de.mapping->pads;

  AliHLTMUONMapping::mpPad &pad(pads[iPad]);
  if (de.nOrderedPads[0] < static_cast<UShort_t>(de.orderedPads[0].size())) {
    de.orderedPads[0][de.nOrderedPads[0]] = iPad;
  }
  else {
    de.orderedPads[0].push_back(iPad);
  }
  cl.lastPad = de.nOrderedPads[0];
  ++de.nOrderedPads[0];
  if (pad.area[0][0] < cl.area[0][0]) cl.area[0][0] = pad.area[0][0];
  if (pad.area[0][1] > cl.area[0][1]) cl.area[0][1] = pad.area[0][1];
  if (pad.area[1][0] < cl.area[1][0]) cl.area[1][0] = pad.area[1][0];
  if (pad.area[1][1] > cl.area[1][1]) cl.area[1][1] = pad.area[1][1];

  pad.useMe = kFALSE;

  // loop over its neighbours
  for (UShort_t iNeighbour = 0; iNeighbour < pad.nNeighbours; ++iNeighbour) {

    if (pads[pad.neighbours[iNeighbour]].useMe) {

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

    digiPads &de(fMpDEs[iDE]);

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
                                                           UShort_t nPreClusters[2], digiPads &de, UChar_t iPlane,
                                                           preCluster *&mergedCl)
{
  /// merge preclusters on the given plane overlapping with the given one (recursive method)

  preCluster *cl2(0x0);

  // loop over preclusters in the given plane
  for (UShort_t iCluster = 0; iCluster < nPreClusters[iPlane]; ++iCluster) {

    if (!preClusters[iPlane][iCluster]->useMe) continue;

    cl2 = preClusters[iPlane][iCluster];
    if (AliHLTMUONMapping::AreOverlapping(cl.area, cl2->area, -1.e-4) &&
        AreOverlapping(cl, *cl2, de, -1.e-4)) {

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
AliHLTMUONPreclusterFinderComponent::preCluster* AliHLTMUONPreclusterFinderComponent::UsePreClusters(preCluster *cl, digiPads &de)
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
void AliHLTMUONPreclusterFinderComponent::MergePreClusters(preCluster &cl1, preCluster &cl2, digiPads &de)
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
Bool_t AliHLTMUONPreclusterFinderComponent::AreOverlapping(preCluster &cl1, preCluster &cl2, digiPads &de, Float_t precision)
{
  /// check if the two preclusters overlap
  /// precision in cm: positive = increase pad size / negative = decrease pad size

  // loop over all pads of the precluster1
  for (UShort_t iOrderPad1 = cl1.firstPad; iOrderPad1 <= cl1.lastPad; ++iOrderPad1) {

    // loop over all pads of the precluster2
    for (UShort_t iOrderPad2 = cl2.firstPad; iOrderPad2 <= cl2.lastPad; ++iOrderPad2) {

      if (AliHLTMUONMapping::AreOverlapping(de.mapping->pads[de.orderedPads[0][iOrderPad1]].area,
                         de.mapping->pads[de.orderedPads[0][iOrderPad2]].area, precision)) return kTRUE;

    }

  }

  return kFALSE;

}

//_________________________________________________________________________________________________
Int_t AliHLTMUONPreclusterFinderComponent::StorePreClusters(AliHLTUInt8_t* outputPtr, AliHLTUInt32_t size)
{
  /// store the preclusters into AliMUONVCluster

  AliCodeTimerAutoGeneral("",0);

  preCluster *cl(0x0);
  const AliHLTMUONDigitStruct *digit(0x0);
  AliHLTUInt32_t totalBytesUsed(0);
  Int_t status(0);

  // loop over DEs
  for (UChar_t iDE = 0; iDE < fgkNDEs; ++iDE) {

    if (fNPreClusters[iDE][0] + fNPreClusters[iDE][1] == 0) continue;

    status = fPreClusterBlock.Reset(outputPtr+totalBytesUsed, size-totalBytesUsed, true);
    if (status < 0) return status;

    digiPads &de(fMpDEs[iDE]);

    // loop over planes
    for (UChar_t iPlane = 0; iPlane < 2; ++iPlane) {

      // loop over preclusters
      for (UShort_t iCluster = 0; iCluster < fNPreClusters[iDE][iPlane]; ++iCluster) {

        if (!fPreClusters[iDE][iPlane][iCluster]->storeMe) continue;

        // add the precluster with its first digit
        cl = fPreClusters[iDE][iPlane][iCluster];
        digit = de.digits[de.mapping->pads[de.orderedPads[1][cl->firstPad]].iDigit];
        status = fPreClusterBlock.StartPreCluster(*digit);
        if (status < 0) return status;

        // loop over other pads and add corresponding digits
        for (UShort_t iOrderPad = cl->firstPad+1; iOrderPad <= cl->lastPad; ++iOrderPad) {

          digit = de.digits[de.mapping->pads[de.orderedPads[1][iOrderPad]].iDigit];
          status = fPreClusterBlock.AddDigit(*digit);
          if (status < 0) return status;

        }

      }

    }

    if (fPreClusterBlock.GetNPreClusters() == 0) continue;

    std::cout << "NPreClusters=" << fPreClusterBlock.GetNPreClusters() << std::endl;

    // register the preclusters data block for this DE
    AliHLTUInt32_t bytesUsed(fPreClusterBlock.BytesUsed());
    status = PushBack(outputPtr+totalBytesUsed, bytesUsed, AliHLTMUONConstants::PreClustersBlockDataType(), de.mapping->id);
    if (status < 0) return status;
    totalBytesUsed += bytesUsed;

  }

  return status;

}

//_________________________________________________________________________________________________
Bool_t AliHLTMUONPreclusterFinderComponent::ReadMapping(const char* filename)
{
  AliCodeTimerAutoGeneral("",0);

  AliHLTMUONMapping::mpDE* mpde = AliHLTMUONMapping::ReadMapping(filename);

  if (!mpde) {
    return kFALSE;
  }

  for (Int_t i = 0; i < fgkNDEs; ++i ) {

    fMpDEs[i].mapping = &mpde[i];

    fDEIndices.Add(mpde[i].id,i);

    digiPads& de(fMpDEs[i]);

    Int_t initialSize = (mpde[i].nPads[0]/10+mpde[i].nPads[1]/10); // 10 % occupancy

    de.ownDigits.reserve(initialSize);
    de.nOwnDigits = 0;
    de.digits.reserve(initialSize);
    de.nOrderedPads[0] = 0;
    de.orderedPads[0].reserve(initialSize);
    de.nOrderedPads[1] = 0;
    de.orderedPads[1].reserve(initialSize);

    for (UChar_t iCath = 0; iCath < 2; ++iCath) {
      de.nFiredPads[iCath] = 0;
      de.firedPads[iCath].reserve(mpde[i].nPads[iCath]/10); // 10% occupancy
    }
  }
  return kTRUE;
}

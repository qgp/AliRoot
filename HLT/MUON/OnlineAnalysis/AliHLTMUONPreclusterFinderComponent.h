#ifndef ALIHLTMUONPRECLUSTERFINDERCOMPONENT_H
#define ALIHLTMUONPRECLUSTERFINDERCOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
///  @file   AliHLTMUONPreclusterFinderComponent.h
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   14 October 2014
///  @brief  Precluster reconstruction processing component for the dimuon HLT.
///

#include <vector>
#include <TExMap.h>
#include "AliHLTProcessor.h"
#include "AliHLTMUONPreClustersBlock.h"
#include "AliHLTMUONDataBlockReader.h"
#include "AliHLTMUONMapping.h"

/**
 * @class AliHLTMUONPreclusterFinderComponent
 * @brief A processing component for the dHLT tracker DDL reconstruction.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONPreclusterFinder <br>
 * Library: \b libAliHLTMUON.so  <br>
 * Input Data Types: ('DDL_RAW ', 'MUON') <br>
 * Output Data Types: ('CLUSTORE', 'MUON') <br>
 *
 * @ingroup alihlt_muon_components
 */
class AliHLTMUONPreclusterFinderComponent : public AliHLTProcessor
{
public:
  AliHLTMUONPreclusterFinderComponent();
  virtual ~AliHLTMUONPreclusterFinderComponent();

  // Public functions to implement AliHLTProcessor's interface.
  // These functions are required for the registration process

  virtual const char* GetComponentID();
  virtual void GetInputDataTypes(AliHLTComponentDataTypeList& list);
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
  virtual AliHLTComponent* Spawn();

protected:

  // Protected functions to implement AliHLTProcessor's interface.
  // These functions provide initialization as well as the actual processing
  // capabilities of the component.

  virtual int DoInit(int argc, const char** argv);
  virtual int DoDeinit();
  virtual int DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
                      AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr,
                      AliHLTUInt32_t& size, AliHLTComponentBlockDataList& outputBlocks);
  using AliHLTProcessor::DoEvent;

private:

  struct digiPads {

    AliHLTMUONMapping::mpDE* mapping;

    std::vector<AliHLTMUONDigitStruct*> ownDigits; // list of digits produced when decoding raw data (owner)
    UShort_t nOwnDigits; // number of digits produced when decoding raw data
    std::vector<const AliHLTMUONDigitStruct*> digits; // list of pointers to digits (not owner)
    UShort_t nFiredPads[2]; // number of fired pads on each plane
    std::vector<UShort_t> firedPads[2]; // indices of fired pads on each plane
    UShort_t nOrderedPads[2]; // current number of fired pads in the following arrays
    std::vector<UShort_t> orderedPads[2]; // indices of fired pads ordered after preclustering and merging
  };

  // precluster structure
  struct preCluster {
    UShort_t firstPad; // index of first associated pad in the orderedPads array
    UShort_t lastPad; // index of last associated pad in the orderedPads array
    Float_t area[2][2]; // 2D area containing the precluster
    Bool_t useMe; // kFALSE if precluster already merged to another one
    Bool_t storeMe; // kTRUE if precluster to be saved (merging result)
  };

  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONPreclusterFinderComponent(const AliHLTMUONPreclusterFinderComponent& /*obj*/);
  /// Not implemented.
  AliHLTMUONPreclusterFinderComponent& operator = (const AliHLTMUONPreclusterFinderComponent& /*obj*/);

  Bool_t ReadMapping(const char* binmapfile);

  void LoadDigits(const AliHLTMUONDigitsBlockReader &dblock);
  void ResetPadsAndPreclusters();

  void PreClusterizeRecursive();
  void AddPad(digiPads &de, UShort_t iPad, preCluster &cl);

  void MergePreClusters();
  void MergePreClusters(preCluster &cl, std::vector<preCluster*> preClusters[2],
                        UShort_t nPreClusters[2], digiPads &de, UChar_t iPlane,
                        preCluster *&mergedCl);
  preCluster* UsePreClusters(preCluster *cl, digiPads &de);
  void MergePreClusters(preCluster &cl1, preCluster &cl2, digiPads &de);

  Bool_t AreOverlapping(preCluster &cl1, preCluster &cl2, digiPads &de, Float_t precision);

  //void StorePreClusters();
  Int_t StorePreClusters(AliHLTUInt8_t* outputPtr, AliHLTUInt32_t size);

  static const UChar_t fgkNDEs = 156; ///< number of DEs

  digiPads fMpDEs[fgkNDEs]; ///< internal mapping
  TExMap fDEIndices; ///< maps DE indices from DE IDs

  UShort_t fNPreClusters[fgkNDEs][2]; ///< number of preclusters in each cathods of each DE
  std::vector<preCluster*> fPreClusters[fgkNDEs][2]; ///< list of preclusters in each cathods of each DE

  AliHLTMUONPreClustersBlock fPreClusterBlock; ///< to fill preclusters data blocks

  ClassDef(AliHLTMUONPreclusterFinderComponent, 0)
};

#endif // ALIHLTMUONPRECLUSTERFINDERCOMPONENT_H

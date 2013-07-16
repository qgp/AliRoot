#ifndef ALIMUONSIMPLECLUSTERSERVER_H
#define ALIMUONSIMPLECLUSTERSERVER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup rec
/// \class AliMUONSimpleClusterServer
/// \brief Implementation of AliMUONVClusterServer interface
/// 
// Author Laurent Aphecetche, Subatech

#ifndef ALIMUONVCLUSTERSERVER_H
#  include "AliMUONVClusterServer.h"
#endif

class AliMUONGeometryTransformer;
class AliMUONTriggerTrackToTrackerClusters;
class AliMUONVClusterFinder;
class AliMUONCluster;
class AliMpVSegmentation;
class AliMpExMap;
class AliMpExMapIterator;
class TClonesArray;

class AliMUONSimpleClusterServer : public AliMUONVClusterServer
{
public:
  AliMUONSimpleClusterServer(AliMUONVClusterFinder* clusterFinder,
                             const AliMUONGeometryTransformer& transformer);
  
  virtual ~AliMUONSimpleClusterServer();
  
  Int_t Clusterize(Int_t chamberId,
                   AliMUONVClusterStore& clusterStore,
                   const AliMpArea& area,
                   const AliMUONRecoParam* recoParam = 0x0);
  
  void UseDigits(TIter& next, AliMUONVDigitStore* digitStore = 0x0);
  
  void Print(Option_t* opt="") const;

  /// Use trigger tracks. Return kFALSE if not used.
  virtual Bool_t UseTriggerTrackStore(AliMUONVTriggerTrackStore* trackStore);

private:
  /// Not implemented
  AliMUONSimpleClusterServer(const AliMUONSimpleClusterServer& rhs);
  /// Not implemented
  AliMUONSimpleClusterServer& operator=(const AliMUONSimpleClusterServer& rhs);
  
  Bool_t Overlap(Int_t detElemId, const AliMpArea& area, AliMpArea& deArea) const;
    
  void Global2Local(Int_t detElemId, const AliMpArea& globalArea, AliMpArea& localArea) const;

  TObjArray* PadArray(Int_t detElemId, Int_t cathode) const;
  
  Int_t FindMCLabel(const AliMUONCluster& cluster, Int_t detElemId, const AliMpVSegmentation* seg[2]) const;
  
private:
  AliMUONVDigitStore* fDigitStore; //!< the digit store (not owner)
  AliMUONVClusterFinder* fClusterFinder; //!< the cluster finder (owner)
  const AliMUONGeometryTransformer& fkTransformer; //!< the geometry transformer (not owner)
  AliMpExMap* fPads[2]; ///< map of TClonesArray of AliMUONPads
  AliMpExMapIterator* fPadsIterator[2]; ///< iterator for the map of TClonesArray of AliMUONPads
  AliMUONVTriggerTrackStore* fTriggerTrackStore; ///< trigger track store (if bypassing of St45 was requested) (not owner)
  AliMUONTriggerTrackToTrackerClusters* fBypass; ///< to convert trigger track into tracker clusters (owner)
  
  ClassDef(AliMUONSimpleClusterServer,0) // Cluster server
};

#endif

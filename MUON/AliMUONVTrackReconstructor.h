#ifndef ALIMUONVTRACKRECONSTRUCTOR_H
#define ALIMUONVTRACKRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// \ingroup rec
/// \class AliMUONVTrackReconstructor
/// \brief Virtual class for the MUON track reconstruction
///
//  Author: Philippe Pillot

#include "AliMUONReconstructor.h"
#include "AliMUONRecoParam.h"

#include <TObject.h>

class AliMUONTrack;
class AliMUONTrackParam;
class AliMUONVCluster;
class AliMUONTriggerTrack;
class AliMUONTrackHitPattern;
class AliMUONVClusterServer;
class AliMUONVClusterStore;
class AliMUONVTrackStore;
class AliMUONVTriggerTrackStore;
class AliMUONVTriggerStore;
class AliMUONGeometryTransformer;
class AliMUONDigitMaker;
class AliMUONTriggerCircuit;
class TClonesArray;

class AliMUONVTrackReconstructor : public TObject {

 public:
  AliMUONVTrackReconstructor(AliMUONVClusterServer& clusterServer); // default Constructor
  virtual ~AliMUONVTrackReconstructor(); // Destructor

  void EventReconstruct(AliMUONVClusterStore& clusterStore,
                        AliMUONVTrackStore& trackStore);
  
  void EventReconstructTrigger(const AliMUONTriggerCircuit& triggerCircuit,
                               const AliMUONVTriggerStore& triggerStore,
                               AliMUONVTriggerTrackStore& triggerTrackStore);
  
  void ValidateTracksWithTrigger(AliMUONVTrackStore& trackStore,
                                 const AliMUONVTriggerTrackStore& triggerTrackStore,
                                 const AliMUONVTriggerStore& triggerStore,
                                 const AliMUONTrackHitPattern& trackHitPattern);
  
  /// re-fit the given track
  virtual Bool_t RefitTrack(AliMUONTrack &track) = 0;
  
  
 protected:

  TClonesArray *fRecTracksPtr; ///< pointer to array of reconstructed tracks
  Int_t fNRecTracks; ///< number of reconstructed tracks

  AliMUONVClusterServer& fClusterServer; ///< reference to our cluster server

  // Functions
  AliMUONVTrackReconstructor (const AliMUONVTrackReconstructor& rhs); ///< copy constructor
  AliMUONVTrackReconstructor& operator=(const AliMUONVTrackReconstructor& rhs); ///< assignment operator
  
  /// Make track candidates from clusters in stations(1..) 4 and 5
  virtual void MakeTrackCandidates(AliMUONVClusterStore& clusterStore) = 0;
  /// Make extra track candidates from clusters in stations(1..) 4 and 5
  virtual void MakeMoreTrackCandidates(AliMUONVClusterStore& clusterStore) = 0;
  /// Follow tracks in stations(1..) 3, 2 and 1
  virtual void FollowTracks(AliMUONVClusterStore& clusterStore) = 0;
  /// Complement the reconstructed tracks
  virtual void ComplementTracks(const AliMUONVClusterStore& clusterStore) = 0;
  void ImproveTracks();
  /// Improve the given reconstructed track
  virtual void ImproveTrack(AliMUONTrack &track) = 0;
  void Finalize();
  /// Finalize the given track
  virtual void FinalizeTrack(AliMUONTrack &track) = 0;
  
  TClonesArray* MakeSegmentsBetweenChambers(const AliMUONVClusterStore& clusterStore, Int_t ch1, Int_t ch2);

  void RemoveIdenticalTracks();
  void RemoveDoubleTracks();

  void AskForNewClustersInStation(const AliMUONTrackParam &trackParam,
				  AliMUONVClusterStore& clusterStore, Int_t station);
  void AskForNewClustersInChamber(const AliMUONTrackParam &trackParam,
				  AliMUONVClusterStore& clusterStore, Int_t chamber);
  
  Double_t TryOneCluster(const AliMUONTrackParam &trackParam, AliMUONVCluster* cluster,
			 AliMUONTrackParam &trackParamAtCluster, Bool_t updatePropagator = kFALSE);
  Bool_t   TryOneClusterFast(const AliMUONTrackParam &trackParam, AliMUONVCluster* cluster);
  Double_t TryTwoClustersFast(const AliMUONTrackParam &trackParamAtCluster1, AliMUONVCluster* cluster2,
			      AliMUONTrackParam &trackParamAtCluster2);

  Bool_t FollowLinearTrackInChamber(AliMUONTrack &trackCandidate, const AliMUONVClusterStore& clusterStore, Int_t nextChamber);
  Bool_t FollowLinearTrackInStation(AliMUONTrack &trackCandidate, const AliMUONVClusterStore& clusterStore, Int_t nextStation);
  

 private:
  
  // Functions
  void ResetTracks();
  
  
  ClassDef(AliMUONVTrackReconstructor, 0) // MUON track reconstructor in ALICE
};
	
#endif

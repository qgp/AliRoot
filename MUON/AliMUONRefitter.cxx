/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
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

#include <cstdlib>
#include "AliMUONRefitter.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONClusterFinderCOG.h"
#include "AliMUONClusterFinderMLEM.h"
#include "AliMUONClusterFinderSimpleFit.h"
#include "AliMUONPreClusterFinder.h"
#include "AliMUONPreClusterFinderV2.h"
#include "AliMUONPreClusterFinderV3.h"
#include "AliMUONSimpleClusterServer.h"
#include "AliMUONTrackReconstructor.h"
#include "AliMUONTrackReconstructorK.h"
#include "AliMUONRecoParam.h"
#include "AliMUONESDInterface.h"
#include "AliMUONVClusterStore.h"
#include "AliMUONVTrackStore.h"
#include "AliMUONTrack.h"
#include "AliMUONTracker.h"
#include "AliLog.h"

//-----------------------------------------------------------------------------
/// \class AliMUONRefitter
///
/// create new MUON object from ESD objects given as input (through the ESDInterface):
///
/// - re-clusterize the ESD clusters using the attached ESD pads
///   (several new clusters can be reconstructed per ESD cluster)
/// - re-fit the ESD tracks using the attached ESD clusters
/// - reconstruct the ESD tracks from ESD pads (i.e. re-clusterized the attached clusters)
///
/// note:
/// - connexion between an ESD cluster and corresponding MUON clusters from re-clustering
///   can be made through the detection element ID
/// - connexion between an ESD track and the corresponding refitted MUON track
///   can be made through their unique ID
///
/// \author Philippe Pillot
//-----------------------------------------------------------------------------

/// \cond CLASSIMP
ClassImp(AliMUONRefitter)
/// \endcond

//_____________________________________________________________________________
AliMUONRefitter::AliMUONRefitter()
: TObject(),
  fGeometryTransformer(0x0),
  fClusterServer(0x0),
  fTracker(0x0),
  fESDInterface(0x0)
{
  /// Default constructor
  CreateGeometryTransformer();
  CreateClusterServer(*fGeometryTransformer);
  if (fClusterServer) fTracker = AliMUONTracker::CreateTrackReconstructor(AliMUONReconstructor::GetRecoParam()->GetTrackingMode(),fClusterServer);
  if (!fClusterServer || !fTracker) {
    AliFatal("refitter initialization failed");
    exit(-1);
  }
}

//_____________________________________________________________________________
AliMUONRefitter::~AliMUONRefitter()
{
  /// Destructor
  delete fGeometryTransformer;
  delete fClusterServer;
  delete fTracker;
}

//_____________________________________________________________________________
AliMUONVTrackStore* AliMUONRefitter::ReconstructFromDigits()
{
  /// re-reconstruct all tracks and attached clusters from the digits
  /// it is the responsability of the user to delete the returned store
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // prepare new track(s)
  AliMUONVTrackStore* newTrackStore = AliMUONESDInterface::NewTrackStore();
  if (!newTrackStore) return 0x0;
  
  // loop over tracks and refit them (create new tracks)
  Int_t nTracks = fESDInterface->GetNTracks();
  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    AliMUONTrack *track = RetrackFromDigits(iTrack);
    newTrackStore->Add(track);
    delete track;
  }
  
  return newTrackStore;
}

//_____________________________________________________________________________
AliMUONVTrackStore* AliMUONRefitter::ReconstructFromClusters()
{
  /// refit all tracks from the attached clusters
  /// it is the responsability of the user to delete the returned store
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // prepare new track(s)
  AliMUONVTrackStore* newTrackStore = AliMUONESDInterface::NewTrackStore();
  if (!newTrackStore) return 0x0;
  
  // loop over tracks and refit them (create new tracks)
  AliMUONTrack *track;
  TIter next(fESDInterface->CreateTrackIterator());
  while ((track = static_cast<AliMUONTrack*>(next()))) {
    AliMUONTrack* newTrack = newTrackStore->Add(*track);
    if (!fTracker->RefitTrack(*newTrack)) newTrackStore->Remove(*newTrack);
  }
  
  return newTrackStore;
}

//_____________________________________________________________________________
AliMUONTrack* AliMUONRefitter::RetrackFromDigits(Int_t iTrack)
{
  /// refit track "iTrack" from the digits (i.e. re-clusterized the attached clusters):
  /// several new clusters may be reconstructed per initial ESD cluster:
  /// -> all the combinations of clusters are considered to build the new tracks
  /// -> return the best track (largest number of clusters or best chi2 in case of equality)
  /// it is the responsability of the user to delete the returned track
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // get the track to refit
  AliMUONTrack* track = fESDInterface->GetTrack(iTrack);
  if (!track) return 0x0;
  
  // check if digits exist
  if (fESDInterface->GetNDigits(iTrack) == 0) {
    AliError(Form("no digit attached to track #%d",iTrack));
    return 0x0;
  }
  
  // prepare new track(s)
  AliMUONVTrackStore* newTrackStore = AliMUONESDInterface::NewTrackStore();
  if (!newTrackStore) return 0x0;
  newTrackStore->Add(*track)->Clear("C");
  
  // prepare new cluster store
  AliMUONVClusterStore* newClusterStore = AliMUONESDInterface::NewClusterStore();
  if (!newClusterStore) {
    delete newTrackStore;
    return 0x0;
  }
  
  // loop over clusters, re-clusterize and build new tracks
  Int_t nClusters = track->GetNClusters();
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    
    // reset the new cluster store
    newClusterStore->Clear();
    
    // get the current cluster
    AliMUONVCluster* cluster = fESDInterface->GetClusterFast(iTrack,iCluster);
    
    // re-clusterize current cluster
    TIter next(fESDInterface->CreateDigitIterator(iTrack, iCluster));
    fClusterServer->UseDigits(next);
    Int_t nNewClusters = fClusterServer->Clusterize(cluster->GetChamberId(),*newClusterStore,AliMpArea());
    
    // check that re-clusterizing succeeded
    if (nNewClusters == 0) {
      AliWarning(Form("refit gave no cluster (chamber %d)",cluster->GetChamberId()));
      AliInfo("initial ESD cluster:");
      cluster->Print("FULL");
      continue;
    }
    
    // add the new cluster(s) to the tracks
    AddClusterToTracks(*newClusterStore, *newTrackStore);
    
  }
  
  // refit the tracks and pick up the best one
  AliMUONTrack *currentTrack, *bestTrack = 0x0;
  Double_t currentChi2, bestChi2 = 1.e10;
  Int_t currentNCluster, bestNClusters = 0;
  TIter next(newTrackStore->CreateIterator());
  while ((currentTrack = static_cast<AliMUONTrack*>(next()))) {
    
    // set the track parameters at first cluster if any (used as seed in original tracking)
    AliMUONTrackParam* param = (AliMUONTrackParam*) currentTrack->GetTrackParamAtCluster()->First();
    if (param) *param = *((AliMUONTrackParam*) track->GetTrackParamAtCluster()->First());
    
    // refit the track
    if (!fTracker->RefitTrack(*currentTrack)) break;
    
    // find best track (the one with the higher number of cluster or the best chi2 in case of equality)
    currentNCluster = currentTrack->GetNClusters();
    currentChi2 = currentTrack->GetGlobalChi2();
    if (currentNCluster > bestNClusters || (currentNCluster == bestNClusters && currentChi2 < bestChi2)) {
      bestTrack = currentTrack;
      bestNClusters = currentNCluster;
      bestChi2 = currentChi2;
    }
    
  }
  
  // copy best track and free memory
  AliMUONTrack* newTrack = bestTrack ? new AliMUONTrack(*bestTrack) : 0x0;
  delete newClusterStore;
  delete newTrackStore;
  
  return newTrack;
}

//_____________________________________________________________________________
AliMUONTrack* AliMUONRefitter::RetrackFromClusters(Int_t iTrack)
{
  /// refit track "iTrack" form the clusters (i.e. do not re-clusterize)
  /// it is the responsability of the user to delete the returned track
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // get the track to refit
  AliMUONTrack* track = fESDInterface->GetTrack(iTrack);
  if (!track) return 0x0;
  
  // refit the track (create a new one)
  AliMUONTrack* newTrack = new AliMUONTrack(*track);
  if (!fTracker->RefitTrack(*newTrack)) {
    delete newTrack;
    return 0x0;
  }
  
  return newTrack;
}

//_____________________________________________________________________________
AliMUONVClusterStore* AliMUONRefitter::ReClusterize(Int_t iTrack, Int_t iCluster)
{
  /// re-clusterize cluster numbered "iCluster" in track "iTrack"
  /// several new clusters may be reconstructed
  /// it is the responsability of the user to delete the returned store
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // get the cluster to re-clusterize
  AliMUONVCluster* cluster = fESDInterface->GetCluster(iTrack,iCluster);
  if (!cluster) return 0x0;
  
  // check if digits exist
  if (cluster->GetNDigits() == 0) {
    AliError(Form("no digit attached to cluster #%d in track %d",iCluster,iTrack));
    return 0x0;
  }
  
  // create the cluster store
  AliMUONVClusterStore* clusterStore = AliMUONESDInterface::NewClusterStore();
  if (!clusterStore) return 0x0;
  
  // re-clusterize
  TIter next(fESDInterface->CreateDigitIterator(iTrack, iCluster));
  fClusterServer->UseDigits(next);
  fClusterServer->Clusterize(cluster->GetChamberId(),*clusterStore,AliMpArea());
  
  return clusterStore;
}

//_____________________________________________________________________________
AliMUONVClusterStore* AliMUONRefitter::ReClusterize(UInt_t clusterId)
{
  /// re-clusterize cluster "clusterId"
  /// several new clusters may be reconstructed
  /// it is the responsability of the user to delete the returned store
  
  if (!fESDInterface) {
    AliError("the refitter must be connected to an ESDInterface containing the ESD event to reconstruct");
    return 0x0;
  }
  
  // get the cluster to re-clusterize
  AliMUONVCluster* cluster = fESDInterface->FindCluster(clusterId);
  if (!cluster) return 0x0;
  
  // check if digits exist
  if (cluster->GetNDigits() == 0) {
    AliError(Form("no digit attached to cluster %d",clusterId));
    return 0x0;
  }
  
  // create the cluster store
  AliMUONVClusterStore* clusterStore = AliMUONESDInterface::NewClusterStore();
  if (!clusterStore) return 0x0;
  
  // re-clusterize
  TIter next(fESDInterface->CreateDigitIteratorInCluster(clusterId));
  fClusterServer->UseDigits(next);
  fClusterServer->Clusterize(cluster->GetChamberId(),*clusterStore,AliMpArea());
  
  return clusterStore;
}

//_____________________________________________________________________________
void AliMUONRefitter::CreateGeometryTransformer()
{
  /// Create geometry transformer (local<->global)
  /// and load geometry data
  fGeometryTransformer = new AliMUONGeometryTransformer();
  fGeometryTransformer->LoadGeometryData();
}

//_____________________________________________________________________________
void AliMUONRefitter::CreateClusterServer(AliMUONGeometryTransformer& transformer)
{
  /// Create cluster server
  AliMUONVClusterFinder* clusterFinder = AliMUONReconstructor::CreateClusterFinder(AliMUONReconstructor::GetRecoParam()->GetClusteringMode());
  fClusterServer = clusterFinder ? new AliMUONSimpleClusterServer(clusterFinder,transformer) : 0x0;
}

//_____________________________________________________________________________
void AliMUONRefitter::AddClusterToTracks(const AliMUONVClusterStore &clusterStore, AliMUONVTrackStore &trackStore)
{
  /// add clusters to each of the given tracks
  /// duplicate the tracks if there are several clusters and add one cluster per copy
  
  // create new track store if there are more than 1 cluster to add per track
  Int_t nClusters = clusterStore.GetSize();
  if (nClusters < 1) return;
  
  AliMUONTrackParam dummyParam;
  AliMUONTrack *currentTrack, *track;
  AliMUONVCluster *newCluster;
  Int_t nTracks = trackStore.GetSize();
  Int_t iTrack = 0;
  Int_t iCluster = 0;
  
  // loop over existing tracks to add the cluster(s)
  TIter nextTrack(trackStore.CreateIterator());
  while ((currentTrack = static_cast<AliMUONTrack*>(nextTrack())) && (iTrack < nTracks)) {
    
    iTrack++;
    
    // add the new cluster(s) to the tracks
    // duplicate the tracks if there are several clusters
    // the loop after loading the last cluster which is added to the current track
    iCluster = 0;
    TIter nextCluster(clusterStore.CreateIterator());
    while ((newCluster = static_cast<AliMUONVCluster*>(nextCluster())) && (iCluster < nClusters - 1)) {
      
      iCluster++;
      
      // add a copy of the current track to the store
      track = trackStore.Add(AliMUONTrack(*currentTrack));
      
      // only set Z parameter to avoid error in AddTrackParamAtCluster()
      // the rest will be recomputed during refit
      dummyParam.SetZ(newCluster->GetZ());
      
      // add new cluster to the new track
      track->AddTrackParamAtCluster(dummyParam, *newCluster, kTRUE);
      
    }
    
    // only set Z parameter to avoid error in AddTrackParamAtCluster()
    // the rest will be recomputed during refit
    dummyParam.SetZ(newCluster->GetZ());
    
    // add new cluster to the current track
    currentTrack->AddTrackParamAtCluster(dummyParam, *newCluster, kTRUE);
    
  }
  
}


//$Id$

#ifndef ALIHLTGLOBALTRACKMATCHER_H
#define ALIHLTGLOBALTRACKMATCHER_H


//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTGlobalTrackMatcher.h
    @author Svein Lindal (svein.lindal@fys.uio.no)
    @date   
    @brief  The HLT class matching TPC tracks to calorimeter clusters
*/


#include "AliHLTLogging.h"
#include "AliESDtrack.h"
#include "TObjArray.h"
#include "TArrayI.h"

class AliHLTGlobalTrackMatcher : public AliHLTLogging{

public:
  AliHLTGlobalTrackMatcher();

  /** destructor */
  virtual ~AliHLTGlobalTrackMatcher();

  //Main function, loops over tracks and calls appropriate functions to establish matches
  template <class T>
  Int_t Match( TObjArray * trackArray, vector<T*>  &clustersVector, Double_t bz ); 
  
private:
  
  void DoInit();

  //Loops over clusters and decides if track is a good match to any of these
  template <class T>
  Int_t MatchTrackToClusters( AliExternalTrackParam * track, vector<T*>  &clustersVector, Int_t nClusters, Float_t * bestMatch, Double_t bz); 

  //Add track Id to cluster's list of matching tracks
  Int_t AddTrackToCluster(Int_t tId, Int_t* clustersArray, Bool_t bestMatch, Int_t nMatches);
  Int_t AddTrackToCluster(Int_t tId, TArrayI* clustersArray, Bool_t bestMatch, Int_t nMatches);

  //Projects track to detector volume and decides if it's anywhere near calorimeter volume
  Bool_t IsTrackCloseToDetector(AliExternalTrackParam * track, Double_t bz, Double_t fMaxX, Bool_t ySign, Double_t fMaxZ, Double_t dRadius);

  // Geometrical cut off values used to decide whether track is anywhere near calorimeter volumes
  Float_t fPhosMaxZ;              // max Z track    (cm)
  Float_t fPhosMaxX;              // max X track    (cm)
  Float_t fEmcalMaxZ;             // max Z track    (cm)
  Float_t fEmcalMaxX;             // max X track    (cm)

  Float_t fMatchDistance;        // Square of maximum distance where track is considered a match to cluster (cm^2)

  const Double_t fPhosRadius;          // Radial position of PHOS 
  const Double_t fEmcalRadius;         // Radial position of EMCAL

  AliHLTGlobalTrackMatcher(const AliHLTGlobalTrackMatcher & );
  AliHLTGlobalTrackMatcher & operator = (const AliHLTGlobalTrackMatcher &);

  ClassDef(AliHLTGlobalTrackMatcher,1) 
};


template <class T>
Int_t AliHLTGlobalTrackMatcher::Match( TObjArray * trackArray, vector<T*>  &clustersVector, Double_t bz ) {
  //See above for documentation

  Int_t nTracks = trackArray->GetEntriesFast();
  Int_t nPhosClusters = clustersVector.size();
 
  //TODO emcal not yet implemented
  Int_t nEmcalClusters = 0; //BALLE event->GetEMCALClusters(fEmcalClustersArray);
  
  if ( nTracks <= 0 ) {
    //    HLTWarning("No tracks in event");
    return 0;
  } else if  ( (nEmcalClusters <= 0) && (nPhosClusters <= 0))  {
    //HLTWarning("No calorimeter clusters in Event"); 
    return 0;
  }

  Float_t bestMatchPhos[nPhosClusters];   
  for(int ic = 0; ic < nPhosClusters; ic++) {
    bestMatchPhos[ic] = 999999;
  }
    
  //BALLE TODO EMCAL implement
  // Float_t bestMatchEmcal[nEmcalClusters];    
  //   for(int ic = 0; ic < nEmcalClusters; ic++) {
  //     bestMatchEmcal[ic] = 999999;
  //   }
    
    
  //Loop over tracks
  for (int it = 0; it < nTracks; it++ ) {
    AliExternalTrackParam * track = static_cast<AliExternalTrackParam*>(trackArray->At(it));

    if ( IsTrackCloseToDetector(track, bz, fPhosMaxX, kFALSE, fPhosMaxZ, fPhosRadius ) ) {
      MatchTrackToClusters( track, clustersVector, nPhosClusters, bestMatchPhos, bz);
      
      //BALLE TODO EMCAL !!!!
      //     } else if ( IsTrackCloseToDetector(track, bz, fEmcalMaxX, kTRUE, fEmcalMaxZ, fEmcalRadius ) ) {
      //       MatchTrackToClusters( track, fEmcalClustersArray, nEmcalClusters, bestMatchEmcal, bz);
    } 
    

  } // track loop 
  
    
  return 0;
} 



template <class T>
Int_t AliHLTGlobalTrackMatcher::MatchTrackToClusters( AliExternalTrackParam * track, vector<T*>  &clustersVector, Int_t nClusters, Float_t * bestMatch, Double_t bz) {
  
  //See header file for documentation
  Int_t iResult = 0;
 
  Float_t clusterPosition[3];
  Double_t trackPosition[3];
 
  
  for(int ic = 0; ic < nClusters; ic++) {
    
    T * cluster = clustersVector.at(ic);
    
    //Get cluster global coordinates
    cluster->GetPosition(clusterPosition);
   
    //Get track postion at radius of cluster
    Double_t rCluster = TMath::Sqrt(clusterPosition[0]*clusterPosition[0] + clusterPosition[1]*clusterPosition[1]);      
    if (! (track->GetXYZAt(rCluster, bz, trackPosition)) ) {
      HLTWarning("Track reached detector but not cluster!!!!!!");
      continue;
    }


    //    HLTInfo("Cluster global position %f %f %f", clusterPosition[0],clusterPosition[1],clusterPosition[2]);

    
    //Calculate track - cluster residual
  

    //Get residual in z= 0 plane (squared)
    Double_t dxy = 0;
    for(int i = 0; i < 2; i++) {
      Double_t dd = trackPosition[i] - clusterPosition[i];
      dxy += dd*dd;
    }

    //Get z residual (squared)
    Double_t dd = trackPosition[2] - clusterPosition[2];
    Double_t dz = dd*dd;
  
    Double_t match = dz + dxy;
    
    //    HLTInfo("Track cluster residual %f, maxmatch %f", match, fMatchDistance);
    
    if( match > fMatchDistance  )  {     
      continue;
    }


    if (match < bestMatch[ic]) {
      bestMatch[ic] = match;
      cluster->SetEmcCpvDistance(TMath::Sqrt(match));
      cluster->SetTrackDistance(TMath::Sqrt(dxy), TMath::Sqrt(dz));
    }
    
    //Add track to cluster's array of matching tracks
    Int_t nTracksMatched = cluster->GetNTracksMatched();
    iResult = AddTrackToCluster(track->GetID(), cluster->GetTracksMatched(), match < bestMatch[ic], nTracksMatched);
    //HLTInfo("Added track %d to cluster %d, it now has %d matching tracks", track->GetID(), cluster->GetID(), cluster->GetNTracksMatched());
  }
  
  return iResult;
}

#endif

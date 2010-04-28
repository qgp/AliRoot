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

class AliHLTGlobalTrackMatcher : public AliHLTLogging {

public:

  ///Constructor
  AliHLTGlobalTrackMatcher();

  ///Destructor
  virtual ~AliHLTGlobalTrackMatcher();

  ///Main function, loops over tracks and calls appropriate functions to establish matches
  template <class T>
  Int_t Match( TObjArray * trackArray, vector<T*>  &clustersVector, Double_t bz ); 


  ///Set the maximum Z to be within distance of detector volume
  void SetMaxZ(Float_t z) { fMaxZ = z; }
  ///Set the maximum X to be within distance of detector volume
  void SetMaxX(Float_t x) { fMaxX = x; }
  ///Set whether the detector is in positive y direction
  void SetYSign(Bool_t y) { fYSign = y; }
  ///Set the max distance to be considered a match
  void SetMatchDistance(Float_t d)  { fMatchDistance = d*d; }
  ///Set the radius of detector volume
  void SetRadius(Float_t r) { fRadius = r; }

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
  Float_t fMaxZ;              // max Z track    (cm)
  Float_t fMaxX;              // max X track    (cm)

  Float_t fMatchDistance;     // Square of maximum distance where track is considered a match to cluster (cm^2)
  Double_t fRadius;           // Radial position of detector volume
  Bool_t fYSign;               // Positive or negative y

  AliHLTGlobalTrackMatcher(const AliHLTGlobalTrackMatcher & );
  AliHLTGlobalTrackMatcher & operator = (const AliHLTGlobalTrackMatcher &);

  ClassDef(AliHLTGlobalTrackMatcher,1) 
};



template <class T>
Int_t AliHLTGlobalTrackMatcher::Match( TObjArray * trackArray, vector<T*>  &clustersVector, Double_t bz ) {
  //See above for documentation


  Int_t nTracks = trackArray->GetEntriesFast();
  Int_t nClusters = clustersVector.size();

  if ( nTracks <= 0 ) {
    return 0;
  } else if  ( nClusters <= 0 )  {
    return 0;
  }

  Float_t bestMatch[nClusters];   
  for(int ic = 0; ic < nClusters; ic++) {
    bestMatch[ic] = 999999;
  }
    
  for (int it = 0; it < nTracks; it++ ) {
    AliExternalTrackParam * track = static_cast<AliExternalTrackParam*>(trackArray->At(it));
    if ( IsTrackCloseToDetector(track, bz, fMaxX, fYSign, fMaxZ, fRadius ) ) {
      MatchTrackToClusters( track, clustersVector, nClusters, bestMatch, bz);
    } 
  } 
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
    
    cluster->GetPosition(clusterPosition);
   
    //Get track postion at radius of cluster
    Double_t rCluster = TMath::Sqrt(clusterPosition[0]*clusterPosition[0] + clusterPosition[1]*clusterPosition[1]);      
    if (! (track->GetXYZAt(rCluster, bz, trackPosition)) ) {
      HLTInfo("Track reached detector but not cluster!!!!!!");
      continue;
    }

    //Calculate track - cluster residual
    Double_t dxy = 0;
    for(int i = 0; i < 2; i++) {
      Double_t dd = trackPosition[i] - clusterPosition[i];
      dxy += dd*dd;
    }

    Double_t dd = trackPosition[2] - clusterPosition[2];
    Double_t dz = dd*dd;
  
    Double_t match = dz + dxy;
    
    if( match > fMatchDistance  )  {     
      continue;
    }

    if (match < bestMatch[ic]) {
      bestMatch[ic] = match;
      cluster->SetEmcCpvDistance(TMath::Sqrt(match));
      cluster->SetTrackDistance(TMath::Sqrt(dxy), TMath::Sqrt(dz));
    }
    
    Int_t nTracksMatched = cluster->GetNTracksMatched();
    iResult = AddTrackToCluster(track->GetID(), cluster->GetTracksMatched(), match < bestMatch[ic], nTracksMatched);
  }
  
  return iResult;
}

#endif

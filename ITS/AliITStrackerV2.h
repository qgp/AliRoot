#ifndef ALIITSTRACKER_H
#define ALIITSTRACKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                          ITS tracker
//
//       Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------
#include "AliTracker.h"
#include "AliITSrecoV2.h"
#include "AliITStrackV2.h"

class AliITSclusterV2;
class AliITSgeom;
class TFile;


//-------------------------------------------------------------------------
class AliITStrackerV2 : public AliTracker {
public:
  AliITStrackerV2():AliTracker(){}
  AliITStrackerV2(const AliITSgeom *geom);
  AliCluster *GetCluster(Int_t index) const;
  Int_t LoadClusters();
  void UnloadClusters();
  Int_t Clusters2Tracks(const TFile *in, TFile *out);
  Int_t PropagateBack(const TFile *in, TFile *out);
  Int_t RefitInward(const TFile *in, TFile *out);
  void SetupFirstPass(Int_t *flags, Double_t *cuts=0);
  void SetupSecondPass(Int_t *flags, Double_t *cuts=0);

  void SetLastLayerToTrackTo(Int_t l=0) {fLastLayerToTrackTo=l;} 
  void SetLayersNotToSkip(Int_t *l);

  void UseClusters(const AliKalmanTrack *t, Int_t from=0) const;

  class AliITSdetector {
  public:
    AliITSdetector(){}
    AliITSdetector(Double_t r,Double_t phi) {fR=r; fPhi=phi;}
    void *operator new(size_t s,AliITSdetector *p) {return p;}
    Double_t GetR()   const {return fR;}
    Double_t GetPhi() const {return fPhi;}
  private:
    Double_t fR;    // polar coordinates 
    Double_t fPhi;  // of this detector
  };

  class AliITSlayer {
  public:
    AliITSlayer();
    AliITSlayer(Double_t r, Double_t p, Double_t z, Int_t nl, Int_t nd);
   ~AliITSlayer();
    Int_t InsertCluster(AliITSclusterV2 *c);
    void ResetClusters();
    void SelectClusters(Double_t zmi,Double_t zma,Double_t ymi,Double_t yma);
    const AliITSclusterV2 *GetNextCluster(Int_t &ci);
    void *operator new(size_t s, AliITSlayer *p) {return p;}
    void ResetRoad();
    Double_t GetRoad() const {return fRoad;}
    Double_t GetR() const {return fR;}
    AliITSclusterV2 *GetCluster(Int_t i) const {return fClusters[i];} 
    AliITSdetector &GetDetector(Int_t n) const { return fDetectors[n]; }
    Int_t FindDetectorIndex(Double_t phi, Double_t z) const;
    Double_t GetThickness(Double_t y, Double_t z, Double_t &x0) const;
    Int_t InRoad() const ;
    Int_t GetNumberOfClusters() const {return fN;}
    Int_t GetNladders() const {return fNladders;}
    Int_t GetNdetectors() const {return fNdetectors;}
  private:
    Double_t fR;                // mean radius of this layer
    Double_t fPhiOffset;        // offset of the first detector in Phi
    Int_t fNladders;            // number of ladders
    Double_t fZOffset;          // offset of the first detector in Z
    Int_t fNdetectors;          // detectors/ladder
    AliITSdetector *fDetectors; // array of detectors
    Int_t fN;                   // number of clusters
    AliITSclusterV2 *fClusters[kMaxClusterPerLayer]; // pointers to clusters
    Double_t fZmax;      //    edges
    Double_t fYmin;      //   of  the
    Double_t fYmax;      //   "window"
    Int_t fI;            // index of the current cluster within the "window"
    Double_t fRoad;      // road defined by the cluster density
    Int_t FindClusterIndex(Double_t z) const;
  };

private:
  void CookLabel(AliKalmanTrack *t,Float_t wrong) const;
  Double_t GetEffectiveThickness(Double_t y, Double_t z) const;
  void  FollowProlongation();
  Int_t TakeNextProlongation();
  Bool_t RefitAt(Double_t x, const AliITStrackV2 *t, AliITStrackV2 *tt);
  void ResetBestTrack() {
     fBestTrack.~AliITStrackV2();
     new(&fBestTrack) AliITStrackV2(fTrackToFollow);
  }
  void ResetTrackToFollow(const AliITStrackV2 &t) {
     fTrackToFollow.~AliITStrackV2();
     new(&fTrackToFollow) AliITStrackV2(t);
  }
  Int_t fI;                              // index of the current layer
  static AliITSlayer fLayers[kMaxLayer]; // ITS layers
  AliITStrackV2 fTracks[kMaxLayer];      // track estimations at the ITS layers
  AliITStrackV2 fBestTrack;              // "best" track 
  AliITStrackV2 fTrackToFollow;          // followed track
  Int_t fPass;                           // current pass through the data 
  Int_t fConstraint[2];                  // constraint flags

  Int_t fLayersNotToSkip[kMaxLayer];     // layer masks
  Int_t fLastLayerToTrackTo;             // the innermost layer to track to

  ClassDef(AliITStrackerV2,1)   //ITS tracker V2
};


inline void AliITStrackerV2::SetupFirstPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the first tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[0]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerV2::SetupSecondPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the second tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[1]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerV2::CookLabel(AliKalmanTrack *t,Float_t wrong) const {
  //--------------------------------------------------------------------
  //This function "cooks" a track label. If label<0, this track is fake.
  //--------------------------------------------------------------------
   Int_t tpcLabel=t->GetLabel();
   if (tpcLabel<0) return;
   AliTracker::CookLabel(t,wrong);
   if (tpcLabel != t->GetLabel()) t->SetLabel(-tpcLabel); 
}

#endif

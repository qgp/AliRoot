#ifndef ALIITSTRACKERV2_H
#define ALIITSTRACKERV2_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                          ITS tracker
//     reads AliITSclusterV2 clusters and creates AliITStrackV2 tracks
//           Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TObjArray.h>

#include "AliTracker.h"
#include "AliITSrecoV2.h"
#include "AliITStrackV2.h"

class AliITSclusterV2;
class AliESD;
class AliITSgeom;
class TTree;


//-------------------------------------------------------------------------
class AliITStrackerV2 : public AliTracker {
public:
  AliITStrackerV2():AliTracker(){}
  AliITStrackerV2(const AliITSgeom *geom);
  AliCluster *GetCluster(Int_t index) const;
  AliITSclusterV2 *GetClusterLayer(Int_t layn, Int_t ncl) const
                        {return fgLayers[layn].GetCluster(ncl);}
  Int_t GetNumberOfClustersLayer(Int_t layn) const 
                        {return fgLayers[layn].GetNumberOfClusters();}
  Int_t LoadClusters(TTree *cf);
  void UnloadClusters();
  Int_t Clusters2Tracks(TTree *in, TTree *out);
  Int_t Clusters2Tracks(AliESD *event);
  Int_t PropagateBack(AliESD *event);
  Int_t RefitInward(AliESD *event);
  Bool_t RefitAt(Double_t x, AliITStrackV2 *seed, const AliITStrackV2 *t);
  void SetupFirstPass(Int_t *flags, Double_t *cuts=0);
  void SetupSecondPass(Int_t *flags, Double_t *cuts=0);

  void SetLastLayerToTrackTo(Int_t l=0) {fLastLayerToTrackTo=l;} 
  void SetLayersNotToSkip(Int_t *l);

  void UseClusters(const AliKalmanTrack *t, Int_t from=0) const;

  class AliITSdetector { 
  public:
    AliITSdetector(){}
    AliITSdetector(Double_t r,Double_t phi) {fR=r; fPhi=phi;}
    Double_t GetR()   const {return fR;}
    Double_t GetPhi() const {return fPhi;}
  private:
    Double_t fR;    // polar coordinates 
    Double_t fPhi;  // of this detector
  };

  class AliITSlayer {
    friend class AliITStrackerV2;
  public:
    AliITSlayer();
    AliITSlayer(Double_t r, Double_t p, Double_t z, Int_t nl, Int_t nd);
   ~AliITSlayer();
    Int_t InsertCluster(AliITSclusterV2 *c);
    void ResetClusters();
    void ResetWeights();
    void SelectClusters(Double_t zmi,Double_t zma,Double_t ymi,Double_t yma);
    const AliITSclusterV2 *GetNextCluster(Int_t &ci);
    void ResetRoad();
    Double_t GetRoad() const {return fRoad;}
    Double_t GetR() const {return fR;}
    AliITSclusterV2 *GetCluster(Int_t i) const {return i<fN? fClusters[i]:0;} 
    Float_t         *GetWeight(Int_t i) {return i<fN ?&fClusterWeight[i]:0;}
    AliITSdetector &GetDetector(Int_t n) const { return fDetectors[n]; }
    Int_t FindDetectorIndex(Double_t phi, Double_t z) const;
    Double_t GetThickness(Double_t y, Double_t z, Double_t &x0) const;
    Int_t InRoad() const ;
    Int_t GetNumberOfClusters() const {return fN;}
    Int_t GetNladders() const {return fNladders;}
    Int_t GetNdetectors() const {return fNdetectors;}
  protected:
    Double_t fR;                // mean radius of this layer
    Double_t fPhiOffset;        // offset of the first detector in Phi
    Int_t fNladders;            // number of ladders
    Double_t fZOffset;          // offset of the first detector in Z
    Int_t fNdetectors;          // detectors/ladder
    AliITSdetector *fDetectors; // array of detectors
    Int_t fN;                   // number of clusters
    AliITSclusterV2 *fClusters[kMaxClusterPerLayer]; // pointers to clusters
    Float_t  fClusterWeight[kMaxClusterPerLayer]; // probabilistic weight of the cluster
    Double_t fZmax;      //    edges
    Double_t fYmin;      //   of  the
    Double_t fYmax;      //   "window"
    Int_t fI;            // index of the current cluster within the "window"
    Double_t fRoad;      // road defined by the cluster density
    Int_t FindClusterIndex(Double_t z) const;
  };
  
protected:
  void CookLabel(AliKalmanTrack *t,Float_t wrong) const;
  Double_t GetEffectiveThickness(Double_t y, Double_t z) const;
  void  FollowProlongation();
  Int_t TakeNextProlongation();
  void ResetBestTrack() {
     fBestTrack.~AliITStrackV2();
     new(&fBestTrack) AliITStrackV2(fTrackToFollow);
  }
  void ResetTrackToFollow(const AliITStrackV2 &t) {
     fTrackToFollow.~AliITStrackV2();
     new(&fTrackToFollow) AliITStrackV2(t);
  }
  Float_t    *GetWeight(Int_t index);
  void AddTrackHypothesys(AliITStrackV2 * track, Int_t esdindex);
  void SortTrackHypothesys(Int_t esdindex, Float_t likelihoodlevel);
  void CompressTrackHypothesys(Int_t esdindex, Float_t likelihoodlevel, Int_t maxsize);
  AliITStrackV2 * GetBestHypothesys(Int_t esdindex, AliITStrackV2 * original, Int_t checkmax); 
  AliITStrackV2 * GetBestHypothesysMIP(Int_t esdindex, AliITStrackV2 * original); 
  Int_t fI;                              // index of the current layer
  static AliITSlayer fgLayers[kMaxLayer];// ITS layers
  AliITStrackV2 fTracks[kMaxLayer];      // track estimations at the ITS layers
  AliITStrackV2 fBestTrack;              // "best" track 
  AliITStrackV2 fTrackToFollow;          // followed track
  TObjArray     fTrackHypothesys;        // ! array with track hypothesys- ARRAY is the owner of tracks- MI
  Int_t         fCurrentEsdTrack;        // ! current esd track           - MI
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
   if (tpcLabel!=TMath::Abs(t->GetLabel())){
     t->SetFakeRatio(1.);
   }
   if (tpcLabel !=t->GetLabel()) {
     t->SetLabel(-tpcLabel);      
   }
}

#endif

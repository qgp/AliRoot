#ifndef ALIITSTRACKERMI_H
#define ALIITSTRACKERMI_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                          ITS tracker
//     reads AliITSclusterMI clusters and creates AliITStrackV2 tracks
//           Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TObjArray.h>

#include "AliTracker.h"
#include "AliITSrecoV2.h"
#include "AliITStrackV2.h"
#include "AliITSclusterV2.h"

class AliITSclusterV2;
class AliESD;
class AliITSgeom;
class TTree;
class AliHelix;
class AliV0vertex;



class AliITSRecV0Info: public TObject {
public:
  void Update(Float_t vertex[3], Float_t mass1, Float_t mass2);
  Double_t       fDist1;    //info about closest distance according closest MC - linear DCA
  Double_t       fDist2;    //info about closest distance parabolic DCA
  Double_t       fInvMass;  //reconstructed invariant mass -
  //
  Double_t       fPdr[3];    //momentum at vertex daughter  - according approx at DCA
  Double_t       fXr[3];     //rec. position according helix
  //
  Double_t       fPm[3];    //momentum at the vertex mother
  Double_t       fAngle[3]; //three angles
  Double_t       fRr;       // rec position of the vertex 
  Int_t          fLab[2];   //MC label of the partecle 
  Float_t        fPointAngleFi; //point angle fi
  Float_t        fPointAngleTh; //point angle theta
  Float_t        fPointAngle;   //point angle full
  ClassDef(AliITSRecV0Info,1)  // container for  
};




//-------------------------------------------------------------------------
class AliITStrackerMI : public AliTracker {
public:
  AliITStrackerMI():AliTracker(){}
  AliITStrackerMI(const AliITSgeom *geom);
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
  void GetNTeor(Int_t layer, const AliITSclusterV2* cl, Float_t theta, Float_t phi, Float_t &ny, Float_t &nz);
  Int_t  GetError(Int_t layer, const AliITSclusterV2*cl, Float_t theta, Float_t phi, Float_t expQ, Float_t &erry, Float_t &errz);
  Double_t GetPredictedChi2MI(AliITStrackV2* track, const AliITSclusterV2 *cluster,Int_t layer);
  Int_t UpdateMI(AliITStrackV2* track, const AliITSclusterV2* cl,Double_t chi2,Int_t layer);
  class AliITSdetector { 
  public:
    AliITSdetector(){}
    AliITSdetector(Double_t r,Double_t phi) {fR=r; fPhi=phi; fSinPhi = TMath::Sin(phi); fCosPhi = TMath::Cos(phi);
    fYmin=10000;fYmax=-1000; fZmin=10000;fZmax=-1000;}
    inline void GetGlobalXYZ( const AliITSclusterV2 *cl, Double_t xyz[3]) const;
    Double_t GetR()   const {return fR;}
    Double_t GetPhi() const {return fPhi;}
    Double_t GetYmin() const {return fYmin;}
    Double_t GetYmax() const {return fYmax;}
    Double_t GetZmin() const {return fZmin;}
    Double_t GetZmax() const {return fZmax;}
    void SetYmin(Double_t min) {fYmin = min;}
    void SetYmax(Double_t max) {fYmax = max;}
    void SetZmin(Double_t min) {fZmin = min;}
    void SetZmax(Double_t max) {fZmax = max;}
  private:
    Double_t fR;    // polar coordinates 
    Double_t fPhi;  // of this detector
    Double_t fSinPhi; // sin of phi;
    Double_t fCosPhi; // cos of phi 
    Double_t fYmin;   //  local y minimal
    Double_t fYmax;   //  local max y
    Double_t fZmin;   //  local z min
    Double_t fZmax;   //  local z max
  };

  class AliITSlayer {
    friend class AliITStrackerMI;
  public:
    AliITSlayer();
    AliITSlayer(Double_t r, Double_t p, Double_t z, Int_t nl, Int_t nd);
   ~AliITSlayer();
    Int_t InsertCluster(AliITSclusterV2 *c);
    void  SortClusters();
    void ResetClusters();
    void ResetWeights();
    void SelectClusters(Double_t zmi,Double_t zma,Double_t ymi,Double_t yma);
    const AliITSclusterV2 *GetNextCluster(Int_t &ci);
    void ResetRoad();
    Double_t GetRoad() const {return fRoad;}
    Double_t GetR() const {return fR;}
    Int_t FindClusterIndex(Float_t z) const;
    AliITSclusterV2 *GetCluster(Int_t i) const {return i<fN? fClusters[i]:0;} 
    Float_t         *GetWeight(Int_t i) {return i<fN ?&fClusterWeight[i]:0;}
    AliITSdetector &GetDetector(Int_t n) const { return fDetectors[n]; }
    Int_t FindDetectorIndex(Double_t phi, Double_t z) const;
    Double_t GetThickness(Double_t y, Double_t z, Double_t &x0) const;
    Int_t InRoad() const ;
    Int_t GetNumberOfClusters() const {return fN;}
    Int_t GetNladders() const {return fNladders;}
    Int_t GetNdetectors() const {return fNdetectors;}
    Int_t GetSkip() const {return fSkip;}
    void  SetSkip(Int_t skip){fSkip=skip;}
    void IncAccepted(){fAccepted++;}
    Int_t GetAccepted() const {return fAccepted;}
  protected:
    Double_t fR;                // mean radius of this layer
    Double_t fPhiOffset;        // offset of the first detector in Phi
    Int_t fNladders;            // number of ladders
    Double_t fZOffset;          // offset of the first detector in Z
    Int_t fNdetectors;          // detectors/ladder
    AliITSdetector *fDetectors; // array of detectors
    Int_t fN;                   // number of clusters
    AliITSclusterV2 *fClusters[kMaxClusterPerLayer]; // pointers to clusters
    Int_t        fClusterIndex[kMaxClusterPerLayer]; // pointers to clusters
    Float_t fY[kMaxClusterPerLayer];                // y position of the clusters      
    Float_t fZ[kMaxClusterPerLayer];                // z position of the clusters      
    Float_t fYB[2];                                       // ymin and ymax
    //
    AliITSclusterV2 *fClusters5[6][kMaxClusterPerLayer5]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex5[6][kMaxClusterPerLayer5]; // pointers to clusters -     slice in y    
    Float_t fY5[6][kMaxClusterPerLayer5];                // y position of the clusters  slice in y    
    Float_t fZ5[6][kMaxClusterPerLayer5];                // z position of the clusters  slice in y 
    Int_t fN5[6];                                       // number of cluster in slice
    Float_t fDy5;                                       //delta y
    Float_t fBy5[6][2];                                    //slice borders
    //
    AliITSclusterV2 *fClusters10[11][kMaxClusterPerLayer10]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex10[11][kMaxClusterPerLayer10]; // pointers to clusters -     slice in y    
    Float_t fY10[11][kMaxClusterPerLayer10];                // y position of the clusters  slice in y    
    Float_t fZ10[11][kMaxClusterPerLayer10];                // z position of the clusters  slice in y 
    Int_t fN10[11];                                       // number of cluster in slice
    Float_t fDy10;
    Float_t fBy10[11][2];                  
    //
    AliITSclusterV2 *fClusters20[21][kMaxClusterPerLayer20]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex20[21][kMaxClusterPerLayer20]; // pointers to clusters -     slice in y    
    Float_t fY20[21][kMaxClusterPerLayer20];                // y position of the clusters  slice in y    
    Float_t fZ20[21][kMaxClusterPerLayer20];                // z position of the clusters  slice in y 
    Int_t fN20[21];                                       // number of cluster in slice
    Float_t fDy20;
    Float_t fBy20[21][2];
    //
    AliITSclusterV2** fClustersCs;                         //clusters table in current slice
    Int_t   *fClusterIndexCs;                             //cluster index in current slice 
    Float_t *fYcs;                                        //y position in current slice
    Float_t *fZcs;                                        //z position in current slice
    Int_t    fNcs;                                        //number of clusters in current slice    
    Int_t fCurrentSlice;                                  //current slice
    //
    Float_t  fClusterWeight[kMaxClusterPerLayer]; // probabilistic weight of the cluster
    Int_t    fClusterTracks[4][kMaxClusterPerLayer]; //tracks registered to given cluster
    Float_t fZmax;      //    edges
    Float_t fYmin;      //   of  the
    Float_t fYmax;      //   "window"
    Int_t fI;            // index of the current cluster within the "window"
    Int_t fImax;            // index of the last cluster within the "window"    
    Int_t fSkip;     // indicates possibility to skip cluster
    Int_t fAccepted;
    Double_t fRoad;      // road defined by the cluster density
  };
  AliITStrackerMI::AliITSlayer    & GetLayer(Int_t layer) const;
  AliITStrackerMI::AliITSdetector & GetDetector(Int_t layer, Int_t n) const {return GetLayer(layer).GetDetector(n); }

protected:
  void FindV0(AliESD *event);  //try to find V0
  Double_t  TestV0(AliHelix *h1, AliHelix *h2, AliITSRecV0Info *vertex);  //try to find V0 - return DCA
  Double_t  FindBestPair(Int_t esdtrack0, Int_t esdtrack1,AliITSRecV0Info *vertex);  // try to find best pair from the tree of track hyp.
  void CookLabel(AliKalmanTrack *t,Float_t wrong) const;
  void CookLabel(AliITStrackV2 *t,Float_t wrong) const;
  Double_t GetEffectiveThickness(Double_t y, Double_t z) const;
  void FollowProlongationTree(AliITStrackV2 * otrack, Int_t esdindex);
  void ResetBestTrack() {
     fBestTrack.~AliITStrackV2();
     new(&fBestTrack) AliITStrackV2(fTrackToFollow);
  }
  void ResetTrackToFollow(const AliITStrackV2 &t) {
     fTrackToFollow.~AliITStrackV2();
     new(&fTrackToFollow) AliITStrackV2(t);
  }
  void CookdEdx(AliITStrackV2* track);
  Double_t GetNormalizedChi2(AliITStrackV2 * track, Int_t mode);
  Double_t GetTruncatedChi2(AliITStrackV2 * track, Float_t fac);
  Double_t NormalizedChi2(AliITStrackV2 * track, Int_t layer);
  Double_t GetInterpolatedChi2(AliITStrackV2 * forwardtrack, AliITStrackV2 * backtrack);  
  Double_t GetMatchingChi2(AliITStrackV2 * track1, AliITStrackV2 * track2);
  Double_t GetDeadZoneProbability(Double_t zpos, Double_t zerr);

  Float_t    *GetWeight(Int_t index);
  void AddTrackHypothesys(AliITStrackV2 * track, Int_t esdindex);
  void SortTrackHypothesys(Int_t esdindex, Int_t maxcut, Int_t mode);
  AliITStrackV2 * GetBestHypothesys(Int_t esdindex, AliITStrackV2 * original, Int_t checkmax); 
  void  GetBestHypothesysMIP(TObjArray &itsTracks); 
  void RegisterClusterTracks(AliITStrackV2* track, Int_t id);
  void UnRegisterClusterTracks(AliITStrackV2* track, Int_t id);
  Float_t GetNumberOfSharedClusters(AliITStrackV2* track,Int_t id, Int_t list[6], AliITSclusterV2 *clist[6]);
  Int_t GetOverlapTrack(AliITStrackV2 *track, Int_t trackID, Int_t &shared, Int_t clusterlist[6], Int_t overlist[6]);
  AliITStrackV2 * GetBest2Tracks(Int_t trackID1, Int_t treackID2, Float_t th0, Float_t th1);
  Float_t  * GetErrY(Int_t trackindex) const {return &fCoeficients[trackindex*48];}
  Float_t  * GetErrZ(Int_t trackindex) const {return &fCoeficients[trackindex*48+12];}
  Float_t  * GetNy(Int_t trackindex) const {return &fCoeficients[trackindex*48+24];}
  Float_t  * GetNz(Int_t trackindex) const {return &fCoeficients[trackindex*48+36];}
  void       SignDeltas( TObjArray *ClusterArray, Float_t zv);
  void MakeCoeficients(Int_t ntracks);
  void UpdateESDtrack(AliITStrackV2* track, ULong_t flags);
  Int_t fI;                              // index of the current layer
  static AliITSlayer fgLayers[kMaxLayer];// ITS layers
  AliITStrackV2 fTracks[kMaxLayer];      // track estimations at the ITS layers
  AliITStrackV2 fBestTrack;              // "best" track 
  AliITStrackV2 fTrackToFollow;          // followed track
  TObjArray     fTrackHypothesys;        // ! array with track hypothesys- ARRAY is the owner of tracks- MI
  Int_t         fBestTrackIndex[100000]; // ! index of the best track
  Int_t         fCurrentEsdTrack;        // ! current esd track           - MI
  Int_t fPass;                           // current pass through the data 
  Int_t fConstraint[2];                  // constraint flags

  Int_t fLayersNotToSkip[kMaxLayer];     // layer masks
  Int_t fLastLayerToTrackTo;             // the innermost layer to track to
  Float_t * fCoeficients;                //! working array with errors and mean cluser shape
  ClassDef(AliITStrackerMI,1)   //ITS tracker V2
};




/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////





inline void AliITStrackerMI::SetupFirstPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the first tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[0]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerMI::SetupSecondPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the second tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[1]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerMI::CookLabel(AliKalmanTrack *t,Float_t wrong) const {
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

inline Double_t AliITStrackerMI::NormalizedChi2(AliITStrackV2 * track, Int_t layer)
{
  //--------------------------------------------------------------------
  //get normalize chi2
  //--------------------------------------------------------------------
  track->fNormChi2[layer] = 2.*track->fNSkipped+0.25*track->fNDeadZone+track->fdEdxMismatch+track->GetChi2()/
  //track->fNormChi2[layer] = 2.*track->fNSkipped+0.25*track->fNDeadZone+track->fdEdxMismatch+track->fChi22/
    TMath::Max(double(track->GetNumberOfClusters()-track->fNSkipped),
	       1./(1.+track->fNSkipped));
  return track->fNormChi2[layer];
}
inline void  AliITStrackerMI::AliITSdetector::GetGlobalXYZ(const AliITSclusterV2 *cl, Double_t xyz[3]) const
{
  //
  // get cluster coordinates in global cooordinate 
  //
  xyz[2] = cl->GetZ();
  xyz[0] = fR*fCosPhi - cl->GetY()*fSinPhi;
  xyz[1] = fR*fSinPhi + cl->GetY()*fCosPhi;
}
#endif

#ifndef ALIITSTRACKERU_H
#define ALIITSTRACKERU_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                ITS upgrade tracker base class
//-------------------------------------------------------------------------

#include "AliTracker.h"
#include "AliESDEvent.h"
#include "AliITSUSeed.h"
#include "AliITSUTrackCond.h"
#include "AliITSUTrackHyp.h"

class AliITSUReconstructor;
class AliITSURecoDet;
class AliITSUClusterPix;
class AliESDtrack;
class AliITSURecoLayer;
class TTree;


//-------------------------------------------------------------------------
class AliITSUTrackerGlo : public AliTracker {

  public:
  enum {kClus2Tracks,kPropBack,kRefitInw};   // tracking phases
  enum { // info from track extrapolation to layer for cluster check
    kTrXIn ,kTrYIn ,kTrZIn ,kTrPhiIn , // entrance (outer) point on the layer from above 
    kTrXOut,kTrYOut,kTrZOut,kTrPhiOut, // exit (inner) point on the layer
    kTrPhi0, kTrDPhi, kTrZ0, kTrDZ,     // mean phi,dPhi, mean z, dZ (don't change this order)
    kNTrImpData};
  //
  enum {kMissingCluster=0  // no cluster found on this layer
	,kTransportFailed=1  // seed did not reach target layer
	,kRWCheckFailed =2  // failed to rotate the seed to frame of the layer impact point
  };
  enum {kStopSearchOnSensor,kClusterNotMatching,kClusterMatching}; // flags for track-to-cluster checks
  //
  enum {kDummyLabel=-3141593};
  AliITSUTrackerGlo(AliITSUReconstructor* rec);
  virtual ~AliITSUTrackerGlo();

  virtual Int_t          Clusters2Tracks(AliESDEvent *event);
  virtual Int_t          PropagateBack(AliESDEvent *event);
  virtual Int_t          RefitInward(AliESDEvent *event);
  virtual Int_t          LoadClusters(TTree * treeRP=0);
  virtual void           UnloadClusters();
  virtual AliCluster*    GetCluster(Int_t index) const;
  //------------------------------------
  AliITSURecoDet*        GetITSInterface()       const {return fITS;}
  //
  //------------------------------------
  Bool_t                 NeedToProlong(AliESDtrack* estTr);
  void                   Init(AliITSUReconstructor* rec);
  void                   FindTrack(AliESDtrack* esdTr, Int_t esdID);
  Bool_t                 InitHypothesis(AliESDtrack *esdTr, Int_t esdID);
  Bool_t                 TransportToLayer(AliITSUSeed* seed, Int_t lFrom, Int_t lTo);
  Bool_t                 TransportToLayer(AliExternalTrackParam* seed, Int_t lFrom, Int_t lTo);
  Bool_t                 GoToExitFromLayer(AliITSUSeed* seed, AliITSURecoLayer* lr, Int_t dir, Bool_t check=kFALSE);
  Bool_t                 GoToExitFromLayer(AliExternalTrackParam* seed, AliITSURecoLayer* lr, Int_t dir, Bool_t check=kFALSE);
  Bool_t                 GoToEntranceToLayer(AliITSUSeed* seed, AliITSURecoLayer* lr, Int_t dir, Bool_t check=kFALSE);
  Bool_t                 GoToEntranceToLayer(AliExternalTrackParam* seed, AliITSURecoLayer* lr, Int_t dir, Bool_t check=kFALSE);
  Bool_t                 PropagateSeed(AliITSUSeed *seed, Double_t xToGo, Double_t mass, Double_t maxStep=1.0, Bool_t matCorr=kTRUE);
  Bool_t                 PropagateSeed(AliExternalTrackParam *seed, Double_t xToGo, Double_t mass, Double_t maxStep=1.0, Bool_t matCorr=kTRUE);
  Bool_t                 RefitTrack(AliITSUTrackHyp* trc, Double_t r, Bool_t stopAtLastCl=kFALSE);
  //
  void                   KillSeed(AliITSUSeed* seed, Bool_t branch=kFALSE);
  Bool_t                 NeedToKill(AliITSUSeed* seed, Int_t flag);
  Bool_t                 GetRoadWidth(AliITSUSeed* seed, int ilrA);
  Int_t                  CheckCluster(AliITSUSeed* seed, Int_t lr, Int_t clID);
  void                   AddProlongationHypothesis(AliITSUSeed* seed, Int_t lr);
  //
  AliITSUSeed*           NewSeedFromPool(const AliITSUSeed* src=0);
  void                   ResetSeedsPool();
  void                   MarkSeedFree(AliITSUSeed* seed );

  AliITSUTrackHyp*       GetTrackHyp(Int_t id)               const  {return (AliITSUTrackHyp*)fHypStore.UncheckedAt(id);}
  void                   SetTrackHyp(AliITSUTrackHyp* hyp,Int_t id) {fHypStore.AddAtAndExpand(hyp,id);}
  void                   DeleteLastSeedFromPool()                   {fSeedsPool.RemoveLast();}
  void                   SaveCurrentTrackHypotheses();
  void                   FinalizeHypotheses();
  void                   UpdateESDTrack(AliITSUTrackHyp* hyp,Int_t flag);
  void                   CookMCLabel(AliITSUTrackHyp* hyp);
 //
 protected:
  TObject*&              NextFreeSeed();
  //
 private:
  //
  AliITSUTrackerGlo(const AliITSUTrackerGlo&);
  AliITSUTrackerGlo &operator=(const AliITSUTrackerGlo &tr);
  //
 protected:
  AliITSUReconstructor*           fReconstructor;  // ITS global reconstructor 
  AliITSURecoDet*                 fITS;            // interface to ITS
  AliESDtrack*                    fCurrESDtrack;   // current esd track in processing
  Double_t                        fCurrMass;       // current track mass
  Double_t                        fTrImpData[kNTrImpData];  // data on track impact on the layer
  //
  // the seeds management to be optimized
  TObjArray                       fHypStore;       // storage for tracks hypotheses
  AliITSUTrackHyp*                fCurrHyp;        // hypotheses container for current track
  TClonesArray                    fSeedsPool;      //! pool for seeds
  TArrayI                         fFreeSeedsID;    //! array of ID's of freed seeds
  Int_t                           fNFreeSeeds;     //! number of seeds freed in the pool
  Int_t                           fLastSeedID;     //! id of the pool seed on which is returned by the NextFreeSeed method
  //
  AliITSUTrackCond                fTrCond;         // tmp, to be moved to recoparam
  Int_t                           fTrackPhase;     // tracking phase
  Int_t*                          fClInfo;         //! auxiliary track cluster info
  //
  static const Double_t           fgkToler;        // tracking tolerance
  //
  ClassDef(AliITSUTrackerGlo,1)   //ITS upgrade tracker
    
};

//_________________________________________________________________________
inline void AliITSUTrackerGlo::AddProlongationHypothesis(AliITSUSeed* seed, Int_t lr)
{
  // add new seed prolongation hypothesis 
  fCurrHyp->AddSeed(seed,lr);
  printf("*** Adding: "); seed->Print();
}

//________________________________________
inline TObject *&AliITSUTrackerGlo::NextFreeSeed()
{
  // return next free slot where the seed can be created
  fLastSeedID = fNFreeSeeds ? fFreeSeedsID.GetArray()[--fNFreeSeeds] : fSeedsPool.GetEntriesFast();
  //  AliInfo(Form("%d",fLastSeedID));
  return fSeedsPool[ fLastSeedID ];
  //
}

//_________________________________________________________________________
inline AliITSUSeed* AliITSUTrackerGlo::NewSeedFromPool(const AliITSUSeed* src)
{
  // create new seed, optionally copying from the source
  AliITSUSeed* sd =  src ? new( NextFreeSeed() ) AliITSUSeed(*src) : new( NextFreeSeed() ) AliITSUSeed();
  sd->SetPoolID(fLastSeedID);
  return sd;
}

//_________________________________________________________________________
inline void AliITSUTrackerGlo::KillSeed(AliITSUSeed* seed, Bool_t branch)
{
  // flag seed as killed, if requested, kill recursively its parents whose sole child is the seed being killed
  seed->Kill();
  seed = (AliITSUSeed*)seed->GetParent();
  if (seed && !seed->DecChildren() && branch) KillSeed(seed,branch);
}

#endif


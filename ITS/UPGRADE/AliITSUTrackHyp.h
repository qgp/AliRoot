#ifndef ALIITSUTRACKHYP_H
#define ALIITSUTRACKHYP_H

#include <TObject.h>
#include <TObjArray.h>
#include "AliKalmanTrack.h"
#include "AliITSUSeed.h"
class AliESDtrack;
class AliCluster;

// Container for track hypotheses

class AliITSUTrackHyp: public AliKalmanTrack
{
 public:
  AliITSUTrackHyp(Int_t nlr=0);
  AliITSUTrackHyp(const AliITSUTrackHyp& src);
  AliITSUTrackHyp &operator=(const AliITSUTrackHyp &src);
  virtual ~AliITSUTrackHyp();
  //
  Int_t              GetNLayers()        const {return fNLayers;}
  Int_t              GetNSeeds(Int_t lr) const {return fLayerSeeds[lr].GetEntriesFast();}
  AliITSUSeed*       GetSeed(Int_t lr, Int_t id) const {return (AliITSUSeed*)fLayerSeeds[lr].UncheckedAt(id);}
  AliITSUSeed*       GetWinner()         const;
  AliESDtrack*       GetESDTrack()       const {return fESDTrack;}
  void               DefineWinner(Int_t lr=0, Int_t id=0);
  const TObjArray*   GetLayerSeeds(Int_t lr) const {return lr<fNLayers ? &fLayerSeeds[lr] : 0;}
  void               AddSeed(AliITSUSeed* seed, Int_t lr) {fLayerSeeds[lr].AddLast(seed);}
  void               SetESDTrack(AliESDtrack* esdtr) {fESDTrack = esdtr;}
  //
  void               SetChi2(Double_t chi2) {fChi2 = chi2;}
  //
  virtual Double_t   GetPredictedChi2(const AliCluster *c) const;
  virtual Bool_t     PropagateTo(Double_t xr, Double_t x0, Double_t rho);
  virtual Bool_t     Update(const AliCluster* c, Double_t chi2, Int_t index);
  virtual Int_t      GetClusterIndex(Int_t)  const { return -1;}
  virtual Int_t      GetNumberOfTracklets()  const { return 0;}
  virtual Int_t      GetTrackletIndex(Int_t) const { return -1;}
  virtual Double_t   GetPIDsignal()          const { return 0;}
  //
  virtual void       Print(Option_t* option = "") const;
  //
 protected:
  UChar_t          fNLayers;               // number of layers
  AliESDtrack*     fESDTrack;              // reference esd track
  TObjArray*       fLayerSeeds;            // seeds of given layer
  //
  ClassDef(AliITSUTrackHyp,1)
};


#endif

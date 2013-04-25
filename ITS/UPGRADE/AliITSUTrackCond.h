#ifndef ALIITSUTRACKCOND_H
#define ALIITSUTRACKCOND_H

#include <TObject.h>
#include <TArrayS.h>

//------------------------------------------------------------------------------
//
// This class defines a set of hit patterns (conditions) to consider the track reconstructable
// Each condidition (few consequitive elements from fConditions array) is set of bit patterns, 
// with each element of condition defining a group of layers which must be present in the tracks.
// For instance, if we require the track to have contributions from
// {lr0 or lr1 or lr2} AND {lr2 or lr 3 or lr 4} AND {lr5 or lr6} then the condition should
// be {BIT(0)|BIT(1)|BIT(2), BIT(2)|BIT(3)|BIT(4), BIT(5)|BIT(6)}.
// Additionally, each condition may request min number of hits to be present
//
// Each AliITSUTrackCond should correspond to single track finding pass and may contain multiple
// conditions. To consider the track reconstructable it is enough to satisfy 1 condition
//
//------------------------------------------------------------------------------


class AliITSUTrackCond : public TObject
{
 public:
  enum {kCondStart,kNGroups,kMinClus,kNAuxSz};
  enum {kMaxBranches=50,kMaxCandidates=500};     // max number of branches one can create for single seed on any layer (except branch w/o cl. attached)
  

  AliITSUTrackCond(Int_t nLayers=0);
  AliITSUTrackCond(const AliITSUTrackCond& src);
  AliITSUTrackCond &operator=(const AliITSUTrackCond& src);

  ~AliITSUTrackCond() {}
  
  void        SetNLayers(Int_t nl);
  void        SetMaxBranches(Int_t lr, Int_t mb)   {fMaxBranches[lr] = mb;}
  void        SetMaxCandidates(Int_t lr, Int_t nc) {fMaxCandidates[lr] = nc;}
  void        SetID(Int_t id)                      {SetUniqueID(id);}
  void        AddNewCondition(Int_t minClusters);
  void        AddGroupPattern(UShort_t patt);

  Int_t       GetID()                                  const {return GetUniqueID();}
  Int_t       GetMaxBranches(Int_t lr)                 const {return fMaxBranches[lr];}
  Int_t       GetMaxCandidates(Int_t lr)               const {return fMaxCandidates[lr];}
  Int_t       GetNConditions()                         const {return fNConditions;}
  UShort_t    GetGroup(Int_t condID,Int_t grID)        const {return fConditions[fAuxData[condID*kNAuxSz+kCondStart]+grID];}
  Bool_t      CheckPattern(UShort_t patt)    const;
  //
  virtual void  Print(Option_t* option = "")           const;

  void        SetMaxTr2ClChi2(Int_t lr, Float_t v)           {fMaxTr2ClChi2[lr] = v;}
  void        SetMissPenalty(Int_t lr,  Float_t v)           {fMissPenalty[lr] = v;}
  void        SetNSigmaRoadY(Int_t lr,  Float_t v)           {fNSigmaRoadY[lr] = v;}
  void        SetNSigmaRoadZ(Int_t lr,  Float_t v)           {fNSigmaRoadZ[lr] = v;}
  //
  Float_t     GetMissPenalty(Int_t lr)                 const {return fMissPenalty[lr];}
  Float_t     GetMaxTr2ClChi2(Int_t lr)                const {return fMaxTr2ClChi2[lr];}
  Float_t     GetNSigmaRoadY(Int_t lr)                 const {return fNSigmaRoadY[lr];}
  Float_t     GetNSigmaRoadZ(Int_t lr)                 const {return fNSigmaRoadZ[lr];}
  Bool_t      IsLayerExcluded(Int_t lr)                const {return GetMaxTr2ClChi2(lr)<=0;}
  //
  void        Init();
  Bool_t      IsInitDone()                             const {return fInitDone;}
  //
 protected: 
  //
  Bool_t      fInitDone;                 // initialization flag
  Int_t       fNLayers;                  // total number of layers
  Short_t*    fMaxBranches;              // [fNLayers] max allowed branches per seed on each layer
  Short_t*    fMaxCandidates;            // [fNLayers] max allowed candidates per TPC seed on each layer
  Float_t*    fMaxTr2ClChi2;             // [fNLayers] max track-to-cluster chi2
  Float_t*    fMissPenalty;              // [fNLayers] chi2 penalty for missing hit on the layer
  Float_t*    fNSigmaRoadY;              // [fNLayers] number of sigmas in Y
  Float_t*    fNSigmaRoadZ;              // [fNLayers] number of sigmas in Z
  //
  Short_t     fNConditions;              // number of conditions defined
  TArrayS     fConditions;               // fNConditions  set of conditions
  TArrayS     fAuxData;                  // condition beginning (1st group), n groups, min clus
  //
  static Int_t   fgkMaxBranches;          // def max number of branches per seed on current layer 
  static Int_t   fgkMaxCandidates;        // def max number of total candidates on current layer 
  static Float_t fgkMaxTr2ClChi2;         // def track-to-cluster chi2 cut
  static Float_t fgkMissPenalty;          // penalty for missing cluster
  //
  ClassDef(AliITSUTrackCond,3)           // set of requirements on track hits pattern
};



#endif

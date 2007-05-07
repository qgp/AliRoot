// @(#) $Id$

#ifndef ALIHLTTPCCATRACKER_H
#define ALIHLTTPCCATRACKER_H

//
// CA Tracking class 
// !
// !
// Author: Ivan Kisel 
//*-- Copyright &copy ALICE HLT Group

#include <vector>

class AliHLTTPCTrackSegmentData;
class AliHLTTPCConfMapPoint;
class AliHLTTPCConfMapTrack;
class AliHLTTPCVertex;
class AliHLTTPCTrackArray;
class AliHLTTPCSpacePointData;
class TApplication;
class TCanvas;
class TList;
class TH1F;
class TH2F;

#include "TCanvas.h"

class AliHLTTPCCATracker {

 public:

  AliHLTTPCCATracker();
  AliHLTTPCCATracker( const AliHLTTPCCATracker &x );
  AliHLTTPCCATracker &operator=( const AliHLTTPCCATracker &x );

  virtual ~AliHLTTPCCATracker();

  Bool_t ReadHits(UInt_t count, AliHLTTPCSpacePointData* hits );

  //getters
#if 0
  Int_t GetNumberOfTracks()    const {return fNTracks;}
  AliHLTTPCTrackArray *GetTracks() const {return fTrack;}
  Double_t GetMaxDca()         const {return fMaxDca;}
  AliHLTTPCVertex* GetVertex()     const {return fVertex;}
#endif


  //setters
#if 0
  void SetNSegments(Int_t f,Int_t g) {fNumPhiSegment=f,fNumEtaSegment=g;} //Set number of subvolumes (#segments in (phi,eta)
  void SetMaxDca(Double_t f) {fMaxDca = f;}

  //setter:
  void SetMinPoints(Int_t f,Bool_t vconstraint) {fMinPoints[(Int_t)vconstraint] = f; }  
  void SetVertexConstraint(Bool_t f) {fVertexConstraint =f;}
  
  void SetHitChi2Cut(Double_t f,Bool_t vert) {fHitChi2Cut[(Int_t)vert]=f;}
  void SetGoodHitChi2(Double_t f,Bool_t vert) {fGoodHitChi2[(Int_t)vert]=f;}
  void SetTrackChi2Cut(Double_t f,Bool_t vert) {fTrackChi2Cut[(Int_t)vert]=f;}
  void SetMaxDist(Int_t f,Bool_t vert) {fMaxDist[(Int_t)vert]=f;}
  void SetTrackletLength(Int_t f,Bool_t vert) {fTrackletLength[(Int_t)vert]=f;}
  void SetRowScopeTrack(Int_t f, Bool_t vc){fRowScopeTrack[(Int_t)vc] = f;}
  void SetRowScopeTracklet(Int_t f, Bool_t vc){fRowScopeTracklet[(Int_t)vc] = f;}
  void SetMaxAngleTracklet(Double_t f, Bool_t vc){fMaxAngleTracklet[(Int_t)vc] = f;}
#endif

  void CACreateHistos() ;
  void CAWriteHistos();

  void CAInitialize();
  void CAReadPatchHits(Int_t patch, UInt_t count, AliHLTTPCSpacePointData* hits );
  void CAFindPatchTracks(Int_t patch);
  void CAFindSliceTracks();


  // JMT 2006/11/13
  void SetOutPtr( AliHLTTPCTrackSegmentData* tr ){fOutputPtr = tr;}
  UInt_t GetOutputSize()  const { return fOutputSize; }
  UInt_t GetOutputNTracks()  const { return fOutputNTracks; }


 private:

  AliHLTTPCTrackSegmentData* fOutputPtr; //!
  UInt_t fOutputNTracks; //!
  UInt_t fOutputSize;    //!

  struct AliHLTTPCCAHit{
    Double_t fX, fY, fZ;          // position
    Double_t fErrx, fErry, fErrz; // position errors 
    Int_t fIndex, fCounter;       // addidtional
  };
  
  std::vector<AliHLTTPCCAHit> fVecHits; //!
  Int_t fPatchFirstHitInd, fPatchLastHitInd; // indices of the first and the last hits in the current patch

  struct AliHLTTPCCATrack{
    Int_t fPatch, fNhits, fNdf, fGood, fUsed, fNext; // flags    
    Double_t fX, fY, fZ, fTy, fTz;//parameters    
    Double_t fCovy, fCovty, fCovyty, fCovz, fCovtz, fCovztz, fChi2;//cov matrix    
    std::vector<Int_t> fVecIHits;//indices of hits
  };

  std::vector<AliHLTTPCCATrack> fVecPatchTracks; //!
  Int_t fPatchFirstTrackInd, fPatchLastTrackInd; // indices of the first and the last tracks in the current patch
  std::vector<AliHLTTPCCATrack> fVecSliceTracks; // tracks

  static bool CompareCATracks(const AliHLTTPCCATrack &a, const AliHLTTPCCATrack &b){
    if (a.fPatch != b.fPatch) return (a.fPatch < b.fPatch);
    else return (a.fNhits > b.fNhits);
  }

  static bool CompareCAHitsX(const Int_t &i, const Int_t &j){
    return (i < j);
  }

  Bool_t fBench; //run-time measurements
  Int_t fNTracks; //number of tracks build.

  AliHLTTPCVertex *fVertex; //!
  Bool_t fParamSet[2];  //!
  Bool_t fVertexFinder; //Include vertexfinding or not 
                        //(latter case vertex=(0,0,0))

  AliHLTTPCConfMapPoint *fHit;  //!
  AliHLTTPCTrackArray *fTrack;  //!
  Double_t fMaxDca;      //cut value for momentum fit
  
   //Number of cells (segments)
  Int_t  fNumRowSegment;          // Total number of padrows
  Int_t  fNumPhiSegment;          // number of phi segments 
  Int_t  fNumEtaSegment;          // number of eta segments
  Int_t  fNumRowSegmentPlusOne;   // row+1
  Int_t  fNumPhiSegmentPlusOne;   // phi+1
  Int_t  fNumEtaSegmentPlusOne;   // eta+1
  Int_t  fNumPhiEtaSegmentPlusOne;// phieta+1
  Int_t  fBounds;                 // bounds
  Int_t  fPhiHitsOutOfRange;      // phi hits out of range
  Int_t  fEtaHitsOutOfRange;      // eta hits out of range

  //tracking range:
  Float_t fPhiMin; //MinPhi angle to consider
  Float_t fPhiMax; //MaxPhi angle to consider
  Float_t fEtaMin; //MinEta to consider
  Float_t fEtaMax; //MaxEta to consider
  Int_t fRowMin;   //Minimum row to consider
  Int_t fRowMax;   //Maximum row to consider

  Bool_t fVertexConstraint;       //vertex constraint (true or false)
  Int_t fTrackletLength[2];       //minimal length of tracks 
  Int_t fRowScopeTracklet[2];     //number of row segments to look for the next point of a tracklet
  Int_t fRowScopeTrack[2];        //number of row segments to look for the next point of a track
  Int_t fMinPoints[2];            //minimum number of points on one track
  
  // Cuts
  Double_t fMaxAngleTracklet[2];  //limit of angle between to pieces of a tracklet
  Int_t fMaxDist[2];              //maximum distance between two hits 
  Double_t fHitChi2Cut[2];        //Maximum hit chi2
  Double_t fGoodHitChi2[2];       //Chi2 to stop looking for next hit
  Double_t fTrackChi2Cut[2];      //Maximum track chi2
  Double_t fGoodDist;             //In segment building, distance consider good enough
  Double_t fMaxPhi;               //Maximum phi
  Double_t fMaxEta;               //Maximum eta

  // Tracking informtion
  Int_t fMainVertexTracks;  //number of tracks coming from the main vertex
  Int_t fClustersUnused;    //number of unused clusters

  Int_t fHistEvent;         //!
  Bool_t fDRAW;             //!
  Bool_t fAsk;              //!

  TApplication *fMyapp;     //!
  TCanvas *fYX, *fZX;       //!
  
  TList *fListHisto;        //!
  TH1F *fHistNClusters;     //!
  TH2F *fHistClustersXY[10];//!


  ClassDef(AliHLTTPCCATracker,1) //class for cellular automaton tracking
};

#endif

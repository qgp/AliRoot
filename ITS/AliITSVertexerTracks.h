#ifndef ALIITSVERTEXERTRACKS_H
#define ALIITSVERTEXERTRACKS_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


//-------------------------------------------------------
// Class for primary vertex determination with ITS tracks
//
//   Origin: A.Dainese, Padova, andrea.dainese@pd.infn.it
//           M.Masera,  Torino, massimo.masera@to.infn.it 
//-------------------------------------------------------

/*****************************************************************************
 *                                                                           *
 * This class determines the primary vertex position using ITS tracks.       *
 * This is done in two steps:                                                *
 * 1) Vertex Finding: a reasonable estimate of the vertex position is        *
 *    obtained from a mean of "points of closest approach" between all       *
 *    possible pairs of tracks.                                              *
 * 2) Vertex Fitting: once tracks are propagated to the position given by    *
 *    first step, the optimal estimate of the position of vertex is obtained *
 *    from a weighted average of the track positions. A covariance           *
 *    matrix and a chi2 for the vertex are given.                            *
 *                                                                           *
 *****************************************************************************/

#include "AliKalmanTrack.h"
#include "AliITSVertexer.h"

#include <TObjArray.h>

class TTree; 
class AliITSVertex; 

class AliITSVertexerTracks : public AliITSVertexer {
  
 public:
  // default constructor
  AliITSVertexerTracks();  
  // standard constructor     
  AliITSVertexerTracks(Double_t field, TString fn,
                      Double_t xStart=0,Double_t yStart=0,Int_t useThFr=0); 
  // destructor
  virtual ~AliITSVertexerTracks() {}
  // return vertex from the set of tracks in the tree
  AliITSVertex *VertexOnTheFly(TTree &trkTree);
  // computes the vertex for the current event
  virtual AliITSVertex*    FindVertexForCurrentEvent(Int_t evnumb);
  // computes the vertex for each event and stores it on file
  virtual void  FindVertices();
  virtual void  PrintStatus() const;
  // computes the vertex for the current event    
  void  SetField(Double_t field) const
    { AliKalmanTrack::SetConvConst(100./0.299792458/field); return; }
  void  SetMinTracks(Int_t n=3) { fMinTracks = n; return; }
  void  SetSkipTracks(Int_t n,Int_t *skipped);
  void  SetUseThrustFrame(Int_t utf=0) { fUseThrustFrame = utf; return; } 
  void  SetVtxStart(Double_t x=0,Double_t y=0) 
    { fNominalPos[0]=x; fNominalPos[1]=y; return; }
  
 private:
  Double_t  fInitPos[3];      // vertex position after vertex finder
  Double_t  fNominalPos[2];   // initial knowledge on vertex position
  Int_t     fMinTracks;       // minimum number of tracks
  Double_t  fMaxChi2PerTrack; // maximum contribition to the chi2 
  Int_t     fUseThrustFrame;  // if !=0 vertex is given in thrust ref. frame 
  Double_t  fPhiThrust;       // thrust direction
  TObjArray fTrkArray;        // array with tracks to be processed
  Int_t     *fTrksToSkip;     // tracks to be skipped for find and fit 
  Int_t     fNTrksToSkip;     // number of tracks to be skipped 

  Bool_t   CheckField() const; 
  void     ComputeMaxChi2PerTrack(Int_t nTracks);
  Int_t    PrepareTracks(TTree &trkTree);
  void     SetPhiThrust(Double_t phi=0.) { fPhiThrust=phi; return; } 
  Double_t SumPl(TTree &momTree,Double_t phi) const;
  void     ThrustFinderXY();
  void     TooFewTracks();
  void     VertexFinder();
  void     VertexFitter();

  ClassDef(AliITSVertexerTracks,1) // 3D Vertexing with ITS tracks 
};

#endif




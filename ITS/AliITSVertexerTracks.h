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
#include "AliITSVertexer.h"
#include "AliITSSimpleVertex.h"

#include <TObjArray.h>

class TTree; 
class AliESDVertex; 
class AliITSSimpleVertex; 
class AliESD;
class AliITStrackV2;

class AliITSVertexerTracks : public AliITSVertexer {
  
 public:
  // default constructor
  AliITSVertexerTracks(); 
  // standard constructor     
  AliITSVertexerTracks(TFile *inFile,TFile *outFile,
	               Int_t fEv=0,Int_t lEv=0,
		       Double_t xStart=0.,Double_t yStart=0.);
  // alternative constructor
  AliITSVertexerTracks(TString fn,Double_t xStart=0,Double_t yStart=0); 
  // destructor
  virtual ~AliITSVertexerTracks();
  // return vertex from the set of tracks in the tree
  AliESDVertex* VertexOnTheFly(TTree &trkTree);
  // computes the vertex for the current event
  virtual AliESDVertex* FindPrimaryVertexForCurrentEvent(Int_t evnumb);
  virtual AliESDVertex* FindVertexForCurrentEvent(Int_t evnumb){
    Warning(" FindVertexForCurrentEvent","Deprecated method use FindPrimaryVertexForCurrentEvent instead");
    return FindPrimaryVertexForCurrentEvent(evnumb);
  }
  // computes the vertex for the current event using the ESD
  AliESDVertex*         FindPrimaryVertexForCurrentEvent(AliESD *esdEvent);
  AliESDVertex*         FindVertexForCurrentEvent(AliESD *esdEvent){
    Warning(" FindVertexForCurrentEvent","Deprecated method use FindPrimaryVertexForCurrentEvent instead");
    return FindPrimaryVertexForCurrentEvent(esdEvent);
  }
  // computes the vertex for each event and stores it on file
  virtual void  FindVertices();
  // computes the vertex for each event and stores it in the ESD
  void FindVerticesESD();

  // computes the vertex for selected tracks 
  // (TrkPos=vector with track positions in ESD)
  AliITSSimpleVertex* VertexForSelectedTracks(AliESD *esdEvent,Int_t nofCand, Int_t *trkPos, Int_t opt=1); 
  // opt=1 (default) finds minimum-distance point among all the selected tracks
  //       approximated as straight lines 
  //       and uses errors on track parameters as weights
  // opt=2 finds minimum-distance point among all the selected tracks
  //       approximated as straight lines 
  // opt=3 finds the average point among DCA points of all pairs of tracks
  //       treated as helices
  // opt=4 finds the average point among DCA points of all pairs of tracks
  //       approximated as straight lines 
  //       and uses errors on track parameters as weights
  // opt=5 finds the average point among DCA points of all pairs of tracks
  //       approximated as straight lines 


  virtual void  PrintStatus() const;
  void  SetMinTracks(Int_t n=2) { fMinTracks = n; return; }
  void  SetSkipTracks(Int_t n,Int_t *skipped); 
  void  SetVtxStart(Double_t x=0,Double_t y=0) 
    { fNominalPos[0]=x; fNominalPos[1]=y; return; }
  void  SetDCAcut(Double_t maxdca)
    { fDCAcut=maxdca; return;}
  
 private:
    // copy constructor (NO copy allowed: the constructor is protected
    // to avoid misuse)
    AliITSVertexerTracks(const AliITSVertexerTracks& vtxr);
    // assignment operator (NO assignment allowed)
    AliITSVertexerTracks& operator=(const AliITSVertexerTracks& /* vtxr */);
  TFile    *fInFile;          // input file (with tracks)
  TFile    *fOutFile;         // output file for vertices
  AliITSSimpleVertex  fSimpVert; // vertex after vertex finder
  Double_t  fNominalPos[2];   // initial knowledge on vertex position
  Int_t     fMinTracks;       // minimum number of tracks
  Double_t  fDCAcut;          // maximum DCA between 2 tracks used for vertex
  Double_t  fMaxChi2PerTrack; // maximum contribition to the chi2 
  TObjArray fTrkArray;        // array with tracks to be processed
  Int_t     *fTrksToSkip;     // tracks to be skipped for find and fit 
  Int_t     fNTrksToSkip;     // number of tracks to be skipped 

  Bool_t   CheckField() const; 
  void     ComputeMaxChi2PerTrack(Int_t nTracks);
  Int_t    PrepareTracks(TTree &trkTree);
  Int_t    PrepareTracks(AliESD* esdEvent,Int_t NofCand, Int_t *TrkPos);
  Double_t Prepare(AliITStrackV2* itstrack);
  void     TooFewTracks();
  void     VertexFinder(Int_t OptUseWeights=0);
  void     HelixVertexFinder();
  void     StrLinVertexFinderMinDist(Int_t OptUseWeights=0);
  static void GetStrLinDerivMatrix(Double_t *p0,Double_t *p1,Double_t m[][3],Double_t *d);
  static void GetStrLinDerivMatrix(Double_t *p0,Double_t *p1,Double_t *sigmasq,Double_t m[][3],Double_t *d);
  static Double_t GetStrLinMinDist(Double_t *p0,Double_t *p1,Double_t *x0);
  static Double_t GetDeterminant3X3(Double_t matr[][3]);
 
  void     VertexFitter();

  ClassDef(AliITSVertexerTracks,1) // 3D Vertexing with ITS tracks 
};

#endif




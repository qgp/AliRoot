#ifndef ALIVERTEXERTRACKS_H
#define ALIVERTEXERTRACKS_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


//-------------------------------------------------------
// Class for vertex determination with ESD tracks
//
//   Origin: AliITSVertexerTracks  
//           A.Dainese, Padova, andrea.dainese@pd.infn.it
//           M.Masera,  Torino, massimo.masera@to.infn.it 
//   Moved to STEER and adapted to ESD tracks: 
//           F.Prino, Torino, prino@to.infn.it 
//-------------------------------------------------------

/*****************************************************************************
 *                                                                           *
 * This class determines the vertex of a set of ESD tracks.                  *
 * Different algorithms are implemented, see data member fAlgo.              *
 *                                                                           *
 *****************************************************************************/

#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliLog.h"

#include <TObjArray.h>
#include <TMatrixD.h>

class TTree; 
class AliESD;

class AliVertexerTracks : public TObject {
  
 public:
  AliVertexerTracks(); 
  AliVertexerTracks(Double_t fieldkG); 
  virtual ~AliVertexerTracks();

  AliESDVertex* FindPrimaryVertex(const AliESD *esdEvent);
  AliESDVertex* VertexForSelectedTracks(TTree *trkTree,Bool_t optUseFitter=kTRUE, Bool_t optPropagate=kTRUE);
  AliESDVertex* VertexForSelectedTracks(TObjArray *trkArray, Bool_t optUseFitter=kTRUE, Bool_t optPropagate=kTRUE);
  AliESDVertex* RemoveTracksFromVertex(AliESDVertex *inVtx,TTree *trksTree,Float_t *diamondxy); 
  void  SetConstraintOff() { fConstraint=kFALSE; return; }
  void  SetConstraintOn() { fConstraint=kTRUE; return; }
  void  SetDebug(Int_t optdebug=0) { fDebug=optdebug; return; }
  void  SetDCAcut(Double_t maxdca) { fDCAcut=maxdca; return; }
  void  SetFinderAlgorithm(Int_t opt=1) { fAlgo=opt; return; }
  void  SetITSRequired() { fITSin=kTRUE; return; }
  void  SetITSrefitRequired() { fITSin=kTRUE;fITSrefit=kTRUE; return; }
  void  SetITSNotRequired() { fITSrefit=kFALSE;fITSin=kFALSE; return; }
  void  SetITSrefitNotRequired() { fITSrefit=kFALSE; return; }
  void  SetMaxd0z0(Double_t maxd0z0=0.5) { fMaxd0z0=maxd0z0; return; }
  void  SetMinITSClusters(Int_t n=5) { fMinITSClusters = n; return; }
  void  SetMinTracks(Int_t n=1) { fMinTracks = n; return; }
  void  SetNSigmad0(Double_t n=3) { fNSigma=n; return; }
  Double_t GetNSigmad0() const { return fNSigma; }
  void  SetOnlyFitter() { if(!fConstraint) AliFatal("Set constraint first!"); 
     fOnlyFitter=kTRUE; return; }
  void  SetSkipTracks(Int_t n,Int_t *skipped);
  void  SetVtxStart(Double_t x=0,Double_t y=0,Double_t z=0) 
    { fNominalPos[0]=x; fNominalPos[1]=y; fNominalPos[2]=z; return; }
  void  SetVtxStartSigma(Double_t sx=3,Double_t sy=3,Double_t sz=6) 
    { fNominalCov[0]=sx*sx; fNominalCov[2]=sy*sy; fNominalCov[5]=sz*sz;
      fNominalCov[1]=0.; fNominalCov[3]=0.; fNominalCov[4]=0.; return; }
  void  SetVtxStart(AliESDVertex *vtx);
  static Double_t GetStrLinMinDist(Double_t *p0,Double_t *p1,Double_t *x0);
  static Double_t GetDeterminant3X3(Double_t matr[][3]);
  static void GetStrLinDerivMatrix(Double_t *p0,Double_t *p1,Double_t (*m)[3],Double_t *d);
  static void GetStrLinDerivMatrix(Double_t *p0,Double_t *p1,Double_t *sigmasq,Double_t (*m)[3],Double_t *d);
  static AliVertex TrackletVertexFinder(TClonesArray *lines, Int_t optUseWeights=0);
  void     SetFieldkG(Double_t field=-999.) { fFieldkG=field; return; }
  Double_t GetFieldkG() const { 
    if(fFieldkG<-99.) AliFatal("Field value not set");
    return fFieldkG; } 

 protected:
  void     HelixVertexFinder();
  void     OneTrackVertFinder();
  Int_t    PrepareTracks(TTree &trkTree,Int_t optImpParCut);
  Bool_t   TrackToPoint(AliESDtrack *t,
		        TMatrixD &ri,TMatrixD &wWi) const;     
  void     VertexFinder(Int_t optUseWeights=0);
  void     VertexFitter(Bool_t useConstraint=kFALSE);
  void     StrLinVertexFinderMinDist(Int_t optUseWeights=0);
  void     TooFewTracks(const AliESD *esdEvent);

   
  AliVertex fVert;         // vertex after vertex finder
  AliESDVertex *fCurrentVertex;  // ESD vertex after fitter
  Double_t  fFieldkG;         // z component of field (kGauss) 
  Double_t  fNominalPos[3];   // initial knowledge on vertex position
  Double_t  fNominalCov[6];   // initial knowledge on vertex position
  Bool_t    fConstraint;      // true when "mean vertex" was set in 
                              // fNominal ... and must be used in the fit
  Bool_t    fOnlyFitter;      // primary with one fitter shot only
                              // (use only with beam constraint)
  Int_t     fMinTracks;       // minimum number of tracks
  Int_t     fMinITSClusters;  // minimum number of ITS clusters per track
  TObjArray fTrkArray;        // array with tracks to be processed
  Int_t     *fTrksToSkip;     // tracks to be skipped for find and fit 
  Int_t     fNTrksToSkip;     // number of tracks to be skipped 
  Double_t  fDCAcut;          // maximum DCA between 2 tracks used for vertex
  Int_t     fAlgo;            // option for vertex finding algorythm
  Double_t  fNSigma;          // number of sigmas for d0 cut in PrepareTracks()
  Double_t  fMaxd0z0;         // value [mm] for sqrt(d0d0+z0z0) cut 
                              // in PrepareTracks(1) if fConstraint=kFALSE
  Bool_t    fITSin;           // if kTRUE (default), use only kITSin tracks
                              // if kFALSE, use all tracks (also TPC only)
  Bool_t    fITSrefit;        // if kTRUE (default), use only kITSrefit tracks
                              // if kFALSE, use all tracks (also TPC only)
  Int_t     fDebug;           //! debug flag - verbose printing if >0
  // fAlgo=1 (default) finds minimum-distance point among all selected tracks
  //         approximated as straight lines 
  //         and uses errors on track parameters as weights
  // fAlgo=2 finds minimum-distance point among all the selected tracks
  //         approximated as straight lines 
  // fAlgo=3 finds the average point among DCA points of all pairs of tracks
  //         treated as helices
  // fAlgo=4 finds the average point among DCA points of all pairs of tracks
  //         approximated as straight lines 
  //         and uses errors on track parameters as weights
  // fAlgo=5 finds the average point among DCA points of all pairs of tracks
  //         approximated as straight lines 

 private:
  AliVertexerTracks(const AliVertexerTracks & source);
  AliVertexerTracks & operator=(const AliVertexerTracks & source);

  ClassDef(AliVertexerTracks,7) // 3D Vertexing with ESD tracks 
};

#endif

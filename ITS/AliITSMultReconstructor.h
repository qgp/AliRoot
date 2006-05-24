#ifndef ALIITSMULTRECONSTRUCTOR_H
#define ALIITSMULTRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/////////////////////////////////////////////////////////////////////////
// 
// AliITSMultReconstructor - find clusters in the pixels (theta and
// phi) and tracklets.
// 
// These can be used to extract charged particles multiplcicity from the ITS.
//
// A tracklet consist of two ITS clusters, one in the first pixel
// layer and one in the second. The clusters are associates if the 
// differencies in Phi (azimuth) and Zeta (longitudinal) are inside 
// a fiducial volume. In case of multiple candidates it is selected the
// candidate with minimum distance in Phi. 
// The boolean fOnlyOneTrackletPerC2 allows to control if two clusters 
// in layer 2 can be associated to the same cluster in layer 1 or not.
//
/////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TTree;
class TH1F;
class TH2F;
class AliITSgeom;

class AliITSMultReconstructor : public TObject 
{
public:
  AliITSMultReconstructor();
  virtual ~AliITSMultReconstructor();

  void SetGeometry(AliITSgeom* geo) {fGeometry = geo;}

  void Reconstruct(TTree* tree, Float_t* vtx, Float_t* vtxRes);

  void SetPhiWindow(Float_t w=0.08) {fPhiWindow=w;}
  void SetZetaWindow(Float_t w=0.5) {fZetaWindow=w;}
  void SetOnlyOneTrackletPerC2(Float_t b = kFALSE) {fOnlyOneTrackletPerC2 = b;}
  
  Int_t GetNClustersLayer1() const {return fNClustersLay1;}
  Int_t GetNClustersLayer2() const {return fNClustersLay2;}
  Int_t GetNTracklets() const {return fNTracklets;}

  Float_t* GetClusterLayer1(Int_t n) {return fClustersLay1[n];}
  Float_t* GetClusterLayer2(Int_t n) {return fClustersLay2[n];}
  Float_t* GetTracklet(Int_t n) {return fTracklets[n];}

  void SetHistOn(Bool_t b=kFALSE) {fHistOn=b;}
  void SaveHists();
protected:

  AliITSgeom*   fGeometry;            // ITS geometry
  
  Float_t**     fClustersLay1;        // clusters in the 1st layer of ITS 
  Float_t**     fClustersLay2;        // clusters in the 2nd layer of ITS 
  Float_t**     fTracklets;           // tracklets 
  Bool_t*       fAssociationFlag;     // flag for the associations 
  
  Int_t         fNClustersLay1; // Number of clusters (Layer1)
  Int_t         fNClustersLay2; // Number of clusters (Layer2)
  Int_t         fNTracklets;    // Number of tracklets

  Float_t       fPhiWindow;     // Search window in phi
  Float_t       fZetaWindow;    // SEarch window in eta

  Bool_t        fOnlyOneTrackletPerC2; // only one tracklet per cluster in layer 2
  
  Bool_t        fHistOn;

  TH1F*         fhClustersDPhiAcc;
  TH1F*         fhClustersDThetaAcc;
  TH1F*         fhClustersDZetaAcc;
  TH1F*         fhClustersDPhiAll;
  TH1F*         fhClustersDThetaAll;
  TH1F*         fhClustersDZetaAll;
 
  TH2F*         fhDPhiVsDThetaAll;
  TH2F*         fhDPhiVsDThetaAcc;
  TH2F*         fhDPhiVsDZetaAll;
  TH2F*         fhDPhiVsDZetaAcc;

  TH1F*         fhetaTracklets;
  TH1F*         fhphiTracklets;
  TH1F*         fhetaClustersLay1;
  TH1F*         fhphiClustersLay1;

  void LoadClusterArrays(TTree* tree);

  ClassDef(AliITSMultReconstructor,0)
};

#endif

#ifndef ALICLUSTER_H
#define ALICLUSTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliCluster
//
//       Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>

//_____________________________________________________________________________
class AliCluster : public TObject {
public:
  AliCluster();
  virtual ~AliCluster() {;}
  AliCluster(Int_t *lab, Float_t *hit);
  void SetLabel(Int_t lab, Int_t i) {fTracks[i]=lab;}
  void SetY(Float_t y)              {fY=y;}
  void SetZ(Float_t z)              {fZ=z;}
  void SetSigmaY2(Float_t sy2)      {fSigmaY2=sy2;}
  void SetSigmaZ2(Float_t sz2)      {fSigmaZ2=sz2;}

  Int_t GetLabel(Int_t i) const {return fTracks[i];}
  Float_t GetY()          const {return fY;}
  Float_t GetZ()          const {return fZ;}
  Float_t GetSigmaY2()    const {return fSigmaY2;}
  Float_t GetSigmaZ2()    const {return fSigmaZ2;}

  //PH  virtual void Use() = 0;
  //PH The pure virtual function has been temporarily replaced by 
  //PH virtual function with empty body. This correction somehow helps
  //PH to write/read TClonesArray with AliITSclusterV2 objects, but obviously
  //PH hides some more tricky problems (to be investigated)
  virtual void Use() {;}

protected:
  Int_t     fTracks[3];//labels of overlapped tracks
  Float_t   fY ;       //Y of cluster
  Float_t   fZ ;       //Z of cluster
  Float_t   fSigmaY2;  //Sigma Y square of cluster
  Float_t   fSigmaZ2;  //Sigma Z square of cluster
  
  ClassDef(AliCluster,1)  // Time Projection Chamber clusters
};

#endif



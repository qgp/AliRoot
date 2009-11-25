#ifndef ALIHLTVERTEXER_H
#define ALIHLTVERTEXER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


#include "AliKFParticle.h"
#include "AliKFVertex.h"

class AliESDtrack;
class AliESDVertex;
class AliTracker;
class AliESDtrack;
class AliESDEvent;

class AliHLTVertexer
{
public:

  class AliESDTrackInfo
    {
    public:
      AliESDTrackInfo(): fParticle(),fPrimDeviation(0),fPrimUsedFlag(0),fOK(0){}
      
      AliKFParticle fParticle; //* assigned KFParticle
      Double_t fPrimDeviation; //* deviation from the primary vertex
      Bool_t fPrimUsedFlag;    //* flag shows that the particle was used for primary vertex fit
      Bool_t fOK;              //* is the track good enough
    };

  AliHLTVertexer();
  virtual ~AliHLTVertexer(){ delete[] fTrackInfos; }

  void SetESD( AliESDEvent *event );
  void FindPrimaryVertex();
  void FindV0s();
  void SetFillVtxConstrainedTracks( bool v ){ fFillVtxConstrainedTracks = v; }
  const AliESDTrackInfo *TrackInfos(){ return fTrackInfos; }  

 private:
  
  AliHLTVertexer(const AliHLTVertexer &t);
  AliHLTVertexer &operator = (const AliHLTVertexer & ){ return *this; }

  AliESDEvent *fESD; // pointer to esd event
  AliESDTrackInfo *fTrackInfos; // information about esd tracks
  AliKFVertex fPrimaryVtx; // reconstructed KF primary vertex
  bool fFillVtxConstrainedTracks; // flag to fill vtx constrained tracks to esd

  ClassDef(AliHLTVertexer,0)   //HLT vertex finder
};



#endif



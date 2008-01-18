// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/
#ifndef ALIEVE_MUONTrack_H
#define ALIEVE_MUONTrack_H

#include <TEveTrack.h>

class AliMUONTrack;
class AliMUONTriggerTrack;
class AliMagF;
class AliESDMuonTrack;
class AliTrackReference;

class TParticle;

class TEveTrackPropagator;
class TEveRecTrack;


class AliEveMUONTrack: public TEveTrack
{

  AliEveMUONTrack(const AliEveMUONTrack&);            // Not implemented
  AliEveMUONTrack& operator=(const AliEveMUONTrack&); // Not implemented

 public:

  AliEveMUONTrack(TEveRecTrack* t, TEveTrackPropagator* rs);
  virtual ~AliEveMUONTrack();

  virtual void MakeTrack(Bool_t /*recurse*/=kFALSE) {}

  void  MakeMUONTrack(AliMUONTrack *mtrack);
  void  MakeMUONTriggerTrack(AliMUONTriggerTrack *mtrack);
  void  MakeESDTrack(AliESDMuonTrack *mtrack);
  void  MakeMCTrack(TParticle *part);
  void  MakeRefTrack(AliMUONTrack *mtrack);
  void  GetField(Double_t *position, Double_t *field);
  void  Propagate(Float_t *xr, Float_t *yr, Float_t *zr, Int_t i1, Int_t i2);
  void  OneStepRungekutta(Double_t charge, Double_t step,
			  Double_t* vect, Double_t* vout);
  Int_t ColorIndex(Float_t val);

  Bool_t IsMUONTrack()        { return fIsMUONTrack;  };
  Bool_t IsMUONTriggerTrack() { return fIsMUONTrack;  };
  Bool_t IsESDTrack()         { return fIsESDTrack;   };
  Bool_t IsMCTrack()          { return fIsMCTrack;    };
  Bool_t IsRefTrack()         { return fIsRefTrack;   };

  void PrintMCTrackInfo();
  void PrintMUONTrackInfo();
  void PrintMUONTriggerTrackInfo();
  void PrintESDTrackInfo();

  void  MUONTrackInfo();          // *MENU*
  void  MUONTriggerInfo();        // *MENU*

 private:

  AliMUONTrack *fTrack;              // pointer to the MUON track
  TParticle    *fPart;               // pointer to the MC particle
  Int_t         fCount;              // track points counter
  Bool_t        fIsMUONTrack;        // track from MUON.Tracks.root
  Bool_t        fIsMUONTriggerTrack; // trigger track from MUON.Tracks.root
  Bool_t        fIsESDTrack;         // track from AliESDs.root
  Bool_t        fIsMCTrack;          // track from Kinematics.root
  Bool_t        fIsRefTrack;         // track from TrackRefs.root

  static AliMagF      *fFieldMap;    // pointer to the magnetic field map

  ClassDef(AliEveMUONTrack, 1);    // Produce TEveUtil:TEveTrack from AliMUONTrack

};

#endif


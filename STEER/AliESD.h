#ifndef ALIESDEVENT_H
#define ALIESDEVENT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliESD
//   This is the class to deal with during the physical analysis of data
//      
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include "TObject.h"
#include "TClonesArray.h"
#include  "AliESDtrack.h"
#include  "AliESDv0.h"
#include  "AliESDcascade.h"

class AliESD : public TObject {
public:
  AliESD();
  virtual ~AliESD() {
    fTracks.Delete();
    fV0s.Delete();
    fCascades.Delete();
  }

  void SetEventNumber(Int_t n) {fEventNumber=n;}

  AliESDtrack *GetTrack(Int_t i) {
    return (AliESDtrack *)fTracks.UncheckedAt(i);
  }
  void AddTrack(const AliESDtrack *t) {
    new(fTracks[fTracks.GetEntriesFast()]) AliESDtrack(*t);
  }

  AliESDv0 *GetV0(Int_t i) {
    return (AliESDv0 *)fV0s.UncheckedAt(i);
  }
  void AddV0(const AliESDv0 *v) {
    new(fV0s[fV0s.GetEntriesFast()]) AliESDv0(*v);
  }

  AliESDcascade *GetCascade(Int_t i) {
    return (AliESDcascade *)fCascades.UncheckedAt(i);
  }
  void AddCascade(const AliESDcascade *c) {
    new(fCascades[fCascades.GetEntriesFast()]) AliESDcascade(*c);
  }

  Int_t  GetEventNumber() const {return fEventNumber;}
  Int_t  GetRunNumber() const {return fRunNumber;}
  Long_t GetTrigger() const {return fTrigger;}
  
  Int_t GetNumberOfTracks()   const {return fTracks.GetEntriesFast();}
  Int_t GetNumberOfV0s()      const {return fV0s.GetEntriesFast();}
  Int_t GetNumberOfCascades() const {return fCascades.GetEntriesFast();}
  
protected:

  // Event Identification
  Int_t        fEventNumber;     // Event Number
  Int_t        fRunNumber;       // Run Number
  Long_t       fTrigger;         // Trigger Type
  Int_t        fRecoVersion;     // Version of reconstruction 

  TClonesArray  fTracks;         // ESD tracks
  TClonesArray  fV0s;            // V0 vertices
  TClonesArray  fCascades;       // Cascade vertices
  
  ClassDef(AliESD,1)  //ESD class 
};

#endif 


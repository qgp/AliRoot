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
#include  "AliESDMuonTrack.h"
#include  "AliESDCaloTrack.h"
#include  "AliESDv0.h"
#include  "AliESDcascade.h"

class AliESD : public TObject {
public:
  AliESD();
  virtual ~AliESD() {
    fTracks.Delete();
    fCaloTracks.Delete();
    fMuonTracks.Delete();
    fV0s.Delete();
    fCascades.Delete();
  }

  void SetEventNumber(Int_t n) {fEventNumber=n;}
  void SetRunNumber(Int_t n) {fRunNumber=n;}
  void SetMagneticField(Float_t mf){fMagneticField = mf;}
  Float_t GetMagneticField() const {return fMagneticField;}
  
  AliESDtrack *GetTrack(Int_t i) const {
    return (AliESDtrack *)fTracks.UncheckedAt(i);
  }
  AliESDCaloTrack *GetCaloTrack(Int_t i) const {
    return (AliESDCaloTrack *)fCaloTracks.UncheckedAt(i);
  }
  AliESDMuonTrack *GetMuonTrack(Int_t i) const {
    return (AliESDMuonTrack *)fMuonTracks.UncheckedAt(i);
  }

  void AddTrack(const AliESDtrack *t) {
    new(fTracks[fTracks.GetEntriesFast()]) AliESDtrack(*t);
  }
  void AddCaloTrack(const AliESDCaloTrack *t) {
    new(fCaloTracks[fCaloTracks.GetEntriesFast()]) AliESDCaloTrack(*t);
  }
  void AddMuonTrack(const AliESDMuonTrack *t) {
    new(fMuonTracks[fMuonTracks.GetEntriesFast()]) AliESDMuonTrack(*t);
  }

  AliESDv0 *GetV0(Int_t i) const {
    return (AliESDv0 *)fV0s.UncheckedAt(i);
  }
  void AddV0(const AliESDv0 *v) {
    new(fV0s[fV0s.GetEntriesFast()]) AliESDv0(*v);
  }

  AliESDcascade *GetCascade(Int_t i) const {
    return (AliESDcascade *)fCascades.UncheckedAt(i);
  }
  void AddCascade(const AliESDcascade *c) {
    new(fCascades[fCascades.GetEntriesFast()]) AliESDcascade(*c);
  }

  void SetVertex(const Double_t *vtx, const Double_t *cvtx=0);
  void GetVertex(Double_t *vtx, Double_t *cvtx) const;

  Int_t  GetEventNumber() const {return fEventNumber;}
  Int_t  GetRunNumber() const {return fRunNumber;}
  Long_t GetTrigger() const {return fTrigger;}
  
  Int_t GetNumberOfTracks()     const {return fTracks.GetEntriesFast();}
  Int_t GetNumberOfCaloTracks() const {return fCaloTracks.GetEntriesFast();}
  Int_t GetNumberOfMuonTracks() const {return fMuonTracks.GetEntriesFast();}
  Int_t GetNumberOfV0s()      const {return fV0s.GetEntriesFast();}
  Int_t GetNumberOfCascades() const {return fCascades.GetEntriesFast();}

  void  Print(Option_t *option="") const;
   
protected:

  // Event Identification
  Int_t        fEventNumber;     // Event Number
  Int_t        fRunNumber;       // Run Number
  Long_t       fTrigger;         // Trigger Type
  Int_t        fRecoVersion;     // Version of reconstruction 
  Float_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF

  Double_t fVtx[3];              // Primary vertex position
  Double_t fCovVtx[6];           // Cov. matrix of the primary vertex position

  TClonesArray  fTracks;         // ESD tracks
  TClonesArray  fCaloTracks;     // Calorimeters' ESD tracks
  TClonesArray  fMuonTracks;     // MUON ESD tracks
  TClonesArray  fV0s;            // V0 vertices
  TClonesArray  fCascades;       // Cascade vertices
  
  ClassDef(AliESD,2)  //ESD class 
                      //ver. 2: Magnetic Field Added; skowron
};

#endif 


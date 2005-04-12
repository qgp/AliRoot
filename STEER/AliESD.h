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
#include  "AliESDVertex.h"
#include  "AliESDtrack.h"
#include  "AliESDMuonTrack.h"
#include  "AliESDv0.h"
#include  "AliESDcascade.h"
#include  "AliESDHLTtrack.h"

class AliESD : public TObject {
public:
  AliESD();
  virtual ~AliESD() {
    fTracks.Delete();
    fMuonTracks.Delete();
    fHLTConfMapTracks.Delete();
    fHLTHoughTracks.Delete();
    fV0s.Delete();
    fCascades.Delete();
  }

  void SetEventNumber(Int_t n) {fEventNumber=n;}
  void SetRunNumber(Int_t n) {fRunNumber=n;}
  void SetTrigger(Long_t n) {fTrigger=n;}
  void SetMagneticField(Float_t mf){fMagneticField = mf;}
  Float_t GetMagneticField() const {return fMagneticField;}
  
  AliESDtrack *GetTrack(Int_t i) const {
    return (AliESDtrack *)fTracks.UncheckedAt(i);
  }
  AliESDMuonTrack *GetMuonTrack(Int_t i) const {
    return (AliESDMuonTrack *)fMuonTracks.UncheckedAt(i);
  }
  AliESDHLTtrack *GetHLTConfMapTrack(Int_t i) const {
    return (AliESDHLTtrack *)fHLTConfMapTracks.UncheckedAt(i);
  }
  AliESDHLTtrack *GetHLTHoughTrack(Int_t i) const {
    return (AliESDHLTtrack *)fHLTHoughTracks.UncheckedAt(i);
  }

  void AddTrack(const AliESDtrack *t) {
    new(fTracks[fTracks.GetEntriesFast()]) AliESDtrack(*t);
  }
  void AddMuonTrack(const AliESDMuonTrack *t) {
    new(fMuonTracks[fMuonTracks.GetEntriesFast()]) AliESDMuonTrack(*t);
  }
  void AddHLTConfMapTrack(const AliESDHLTtrack *t) {
    new(fHLTConfMapTracks[fHLTConfMapTracks.GetEntriesFast()]) AliESDHLTtrack(*t);
  }
  void AddHLTHoughTrack(const AliESDHLTtrack *t) {
    new(fHLTHoughTracks[fHLTHoughTracks.GetEntriesFast()]) AliESDHLTtrack(*t);
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

  void SetVertex(AliESDVertex* vertex) {
    new(&fPrimaryVertex) AliESDVertex(*vertex);
  }
  const AliESDVertex* GetVertex() const {return &fPrimaryVertex;};

  Int_t  GetEventNumber() const {return fEventNumber;}
  Int_t  GetRunNumber() const {return fRunNumber;}
  Long_t GetTrigger() const {return fTrigger;}
  
  Int_t GetNumberOfTracks()     const {return fTracks.GetEntriesFast();}
  Int_t GetNumberOfMuonTracks() const {return fMuonTracks.GetEntriesFast();}
  Int_t GetNumberOfHLTConfMapTracks()     const {return fHLTConfMapTracks.GetEntriesFast();}
  Int_t GetNumberOfHLTHoughTracks()     const {return fHLTHoughTracks.GetEntriesFast();}
  Int_t GetNumberOfV0s()      const {return fV0s.GetEntriesFast();}
  Int_t GetNumberOfCascades() const {return fCascades.GetEntriesFast();}
  Int_t GetNumberOfPHOSParticles() const {return fPHOSParticles;}
  void  SetNumberOfPHOSParticles(Int_t part) { fPHOSParticles = part ; }
  void  SetFirstPHOSParticle(Int_t index) { fFirstPHOSParticle = index ; } 
  Int_t GetNumberOfEMCALParticles() const {return fEMCALParticles;}
  void  SetNumberOfEMCALParticles(Int_t part) { fEMCALParticles = part ; }
  void  SetFirstEMCALParticle(Int_t index) { fFirstEMCALParticle = index ; }
 
  void  ResetV0s() { fV0s.Clear(); }
  void  ResetCascades() { fCascades.Clear(); }
  
  void  Print(Option_t *option="") const;
   
protected:

  // Event Identification
  Int_t        fEventNumber;     // Event Number
  Int_t        fRunNumber;       // Run Number
  Long_t       fTrigger;         // Trigger Type
  Int_t        fRecoVersion;     // Version of reconstruction 
  Float_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF

  AliESDVertex  fPrimaryVertex;  // Primary vertex

  TClonesArray  fTracks;         // ESD tracks
  TClonesArray  fMuonTracks;     // MUON ESD tracks
  TClonesArray  fHLTConfMapTracks; // HLT ESD tracks from Conformal Mapper method
  TClonesArray  fHLTHoughTracks; // HLT ESD tracks from Hough Transform method
  TClonesArray  fV0s;            // V0 vertices
  TClonesArray  fCascades;       // Cascade vertices
  Int_t         fPHOSParticles;  // Number of PHOS particles (stored as fTracks)
  Int_t         fEMCALParticles; // Number of EMCAL particles (stored as fTracks)
  Int_t         fFirstPHOSParticle; // First PHOS particle in the fTracks list 
  Int_t         fFirstEMCALParticle;// First EMCAL particle in the fTracks list 
  
  ClassDef(AliESD,5)  //ESD class 
                      //ver. 2: Magnetic Field Added; skowron
};

#endif 


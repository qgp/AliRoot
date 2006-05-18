// -*- mode: C++ -*- 
#ifndef ALIESD_H
#define ALIESD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliESD
//   This is the class to deal with during the physical analysis of data
//      
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TClonesArray.h>
#include <TObject.h>

#include "AliESDMuonTrack.h"
#include "AliESDPmdTrack.h"
#include "AliESDTrdTrack.h"
#include "AliESDVertex.h"
#include "AliESDcascade.h"
#include "AliESDkink.h"
#include "AliESDtrack.h"
#include "AliESDHLTtrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDv0.h"
#include "AliESDV0MI.h"
#include "AliESDFMD.h"

class AliESD : public TObject {
public:
  AliESD();
  virtual ~AliESD(); 

  void SetEventNumber(Int_t n) {fEventNumber=n;}
  void SetRunNumber(Int_t n) {fRunNumber=n;}
  void SetTrigger(Long_t n) {fTrigger=n;}
  void SetMagneticField(Float_t mf){fMagneticField = mf;}
  Float_t GetMagneticField() const {return fMagneticField;}
  
  AliESDtrack *GetTrack(Int_t i) const {
    return (AliESDtrack *)fTracks.UncheckedAt(i);
  }
  AliESDHLTtrack *GetHLTConfMapTrack(Int_t i) const {
    return (AliESDHLTtrack *)fHLTConfMapTracks.UncheckedAt(i);
  }
  AliESDHLTtrack *GetHLTHoughTrack(Int_t i) const {
    return (AliESDHLTtrack *)fHLTHoughTracks.UncheckedAt(i);
  }
  AliESDMuonTrack *GetMuonTrack(Int_t i) const {
    return (AliESDMuonTrack *)fMuonTracks.UncheckedAt(i);
  }
  AliESDPmdTrack *GetPmdTrack(Int_t i) const {
    return (AliESDPmdTrack *)fPmdTracks.UncheckedAt(i);
  }
  AliESDTrdTrack *GetTrdTrack(Int_t i) const {
    return (AliESDTrdTrack *)fTrdTracks.UncheckedAt(i);
  }

  Int_t  AddTrack(const AliESDtrack *t) {
    AliESDtrack * track = new(fTracks[fTracks.GetEntriesFast()]) AliESDtrack(*t);
    track->SetID(fTracks.GetEntriesFast()-1);
    return  track->GetID();
    
  }
  void AddHLTConfMapTrack(const AliESDHLTtrack *t) {
    new(fHLTConfMapTracks[fHLTConfMapTracks.GetEntriesFast()]) AliESDHLTtrack(*t);
  }
  void AddHLTHoughTrack(const AliESDHLTtrack *t) {
    new(fHLTHoughTracks[fHLTHoughTracks.GetEntriesFast()]) AliESDHLTtrack(*t);
  }
  void AddMuonTrack(const AliESDMuonTrack *t) {
    new(fMuonTracks[fMuonTracks.GetEntriesFast()]) AliESDMuonTrack(*t);
  }
  void AddPmdTrack(const AliESDPmdTrack *t) {
    new(fPmdTracks[fPmdTracks.GetEntriesFast()]) AliESDPmdTrack(*t);
  }
  void AddTrdTrack(const AliESDTrdTrack *t) {
    new(fTrdTracks[fTrdTracks.GetEntriesFast()]) AliESDTrdTrack(*t);
  }

  AliESDv0 *GetV0(Int_t i) const {
    return (AliESDv0 *)fV0s.UncheckedAt(i);
  }
  void AddV0(const AliESDv0 *v) {
    new(fV0s[fV0s.GetEntriesFast()]) AliESDv0(*v);
  }
  void UpdateV0PIDs();

  AliESDcascade *GetCascade(Int_t i) const {
    return (AliESDcascade *)fCascades.UncheckedAt(i);
  }
  void AddCascade(const AliESDcascade *c) {
    new(fCascades[fCascades.GetEntriesFast()]) AliESDcascade(*c);
  }

  AliESDkink *GetKink(Int_t i) const {
    return (AliESDkink *)fKinks.UncheckedAt(i);
  }
  Int_t AddKink(const AliESDkink *c) {
    AliESDkink * kink = new(fKinks[fKinks.GetEntriesFast()]) AliESDkink(*c);
    kink->SetID(fKinks.GetEntriesFast());
    return fKinks.GetEntriesFast()-1;
  }

  AliESDV0MI *GetV0MI(Int_t i) const {
    return (AliESDV0MI *)fV0MIs.UncheckedAt(i);
  }
  Int_t AddV0MI(const AliESDV0MI *c) {
    AliESDV0MI * v0 = new(fV0MIs[fV0MIs.GetEntriesFast()]) AliESDV0MI(*c);
    v0->SetID(fV0MIs.GetEntriesFast()-1);
    return fV0MIs.GetEntriesFast()-1;
  }

  AliESDCaloCluster *GetCaloCluster(Int_t i) const {
    return (AliESDCaloCluster *)fCaloClusters.UncheckedAt(i);
  }
  Int_t AddCaloCluster(const AliESDCaloCluster *c) {
    AliESDCaloCluster *clus = new(fCaloClusters[fCaloClusters.GetEntriesFast()]) AliESDCaloCluster(*c);
    clus->SetID(fCaloClusters.GetEntriesFast()-1);
    return fCaloClusters.GetEntriesFast()-1;
  }
    
  void SetVertex(const AliESDVertex* vertex) {
    new(&fPrimaryVertex) AliESDVertex(*vertex);
  }
  const AliESDVertex* GetVertex() const {return &fPrimaryVertex;};

  Int_t  GetEventNumber() const {return fEventNumber;}
  Int_t  GetRunNumber() const {return fRunNumber;}
  Long_t GetTrigger() const {return fTrigger;}
  
  Int_t GetNumberOfTracks()     const {return fTracks.GetEntriesFast();}
  Int_t GetNumberOfHLTConfMapTracks()     const {return fHLTConfMapTracks.GetEntriesFast();}
  Int_t GetNumberOfHLTHoughTracks()     const {return fHLTHoughTracks.GetEntriesFast();}
  Int_t GetNumberOfMuonTracks() const {return fMuonTracks.GetEntriesFast();}
  Int_t GetNumberOfPmdTracks() const {return fPmdTracks.GetEntriesFast();}
  Int_t GetNumberOfTrdTracks() const {return fTrdTracks.GetEntriesFast();}
  Int_t GetNumberOfV0s()      const {return fV0s.GetEntriesFast();}
  Int_t GetNumberOfCascades() const {return fCascades.GetEntriesFast();}
  Int_t GetNumberOfKinks() const {return fKinks.GetEntriesFast();}
  Int_t GetNumberOfV0MIs() const {return fV0MIs.GetEntriesFast();}
  Int_t GetNumberOfCaloClusters() const {return fCaloClusters.GetEntriesFast();}

  Int_t GetNumberOfEMCALClusters() const {return fEMCALClusters;}
  void  SetNumberOfEMCALClusters(Int_t clus) {fEMCALClusters = clus;}
  Int_t GetFirstEMCALCluster() const {return fFirstEMCALCluster;}
  void  SetFirstEMCALCluster(Int_t index) {fFirstEMCALCluster = index;}

  Int_t GetNumberOfPHOSClusters() const {return fPHOSClusters;}
  void  SetNumberOfPHOSClusters(Int_t part) { fPHOSClusters = part ; }
  void  SetFirstPHOSCluster(Int_t index) { fFirstPHOSCluster = index ; } 
  Int_t GetFirstPHOSCluster() const  { return fFirstPHOSCluster ; }

  Float_t GetT0zVertex() const {return fT0zVertex;}
  void SetT0zVertex(Float_t z) {fT0zVertex=z;}

  Float_t GetZDCN1Energy() const {return fZDCN1Energy;}
  Float_t GetZDCP1Energy() const {return fZDCP1Energy;}
  Float_t GetZDCN2Energy() const {return fZDCN2Energy;}
  Float_t GetZDCP2Energy() const {return fZDCP2Energy;}
  Float_t GetZDCEMEnergy() const {return fZDCEMEnergy;}
  Int_t   GetZDCParticipants() const {return fZDCParticipants;}
  void    SetZDC(Float_t n1Energy, Float_t p1Energy, Float_t emEnergy,
                 Float_t n2Energy, Float_t p2Energy, Int_t participants) 
   {fZDCN1Energy=n1Energy; fZDCP1Energy=p1Energy; fZDCEMEnergy=emEnergy;
    fZDCN2Energy=n2Energy; fZDCP2Energy=p2Energy; fZDCParticipants=participants;}

  void ResetV0s() { fV0s.Clear(); }
  void ResetCascades() { fCascades.Clear(); }
  void Reset();

  void  Print(Option_t *option="") const;

  void SetFMDData(AliESDFMD * obj) {
    fESDFMD = new AliESDFMD(*obj);
  }

  AliESDFMD * GetFMDData(){ return fESDFMD;}
   
protected:

  // Event Identification
  Int_t        fEventNumber;     // Event Number
  Int_t        fRunNumber;       // Run Number
  Long_t       fTrigger;         // Trigger Type
  Int_t        fRecoVersion;     // Version of reconstruction 
  Float_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF

  Float_t      fZDCN1Energy;      // reconstructed energy in the neutron ZDC
  Float_t      fZDCP1Energy;      // reconstructed energy in the proton ZDC
  Float_t      fZDCN2Energy;      // reconstructed energy in the neutron ZDC
  Float_t      fZDCP2Energy;      // reconstructed energy in the proton ZDC
  Float_t      fZDCEMEnergy;     // reconstructed energy in the electromagnetic ZDC
  Int_t        fZDCParticipants; // number of participants estimated by the ZDC

  Float_t      fT0zVertex;       // vertex z position estimated by the START
  AliESDVertex fPrimaryVertex;   // Primary vertex estimated by the ITS

  TClonesArray fTracks;          // ESD tracks
  TClonesArray fHLTConfMapTracks;// HLT ESD tracks from Conformal Mapper method
  TClonesArray fHLTHoughTracks;  // HLT ESD tracks from Hough Transform method
  TClonesArray fMuonTracks;      // MUON ESD tracks
  TClonesArray fPmdTracks;       // PMD ESD tracks
  TClonesArray fTrdTracks;       // TRD ESD tracks (triggered)
  TClonesArray fV0s;             // V0 vertices
  TClonesArray fCascades;        // Cascade vertices
  TClonesArray fKinks;           // Kinks
  TClonesArray fV0MIs;           // V0MI
  TClonesArray fCaloClusters;    // Calorimeter clusters for PHOS/EMCAL
  Int_t        fEMCALClusters;   // Number of EMCAL clusters (subset of caloclusters)
  Int_t        fFirstEMCALCluster; // First EMCAL cluster in the fCaloClusters list 

  Int_t        fPHOSClusters;     // Number of PHOS clusters (subset of caloclusters)
  Int_t        fFirstPHOSCluster; // First PHOS cluster in the fCaloClusters list 
 
  AliESDFMD *  fESDFMD; // FMD object containing rough multiplicity

  ClassDef(AliESD,9)  //ESD class 
};
#endif 


// -*- mode: C++ -*- 
#ifndef ALIVEVENT_H
#define ALIVEVENT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliVEvent
//      
// Origin: Markus Oldenburg, CERN, Markus.Oldenburg@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>
#include <TTree.h>
#include <TGeoMatrix.h>
#include "AliVHeader.h"
#include "AliVParticle.h"
#include "AliVVertex.h"
#include "AliVCluster.h"
#include "AliVCaloCells.h"
#include "AliVCaloTrigger.h"
#include "TRefArray.h"
#include "AliTOFHeader.h"
#include "AliVTrdTrack.h"
#include "AliVMultiplicity.h"
class AliVfriendEvent;
class AliCentrality;
class AliEventplane;
class AliVVZERO;
class AliVZDC;
class AliVMFT;   // AU
class AliESDkink;
class AliESDv0;
class AliESDVertex;
class AliESDVZERO;
class AliVTrack;
class AliVAD;

class AliVEvent : public TObject {

public:
  enum EDataLayoutType { kESD, kMC, kAOD, kMixed, kFlat };
  enum EOfflineTriggerTypes { 
      kMB                = BIT( 0), // Minimum bias trigger, i.e. interaction trigger, offline SPD or V0 selection
      kINT1              = BIT( 0), // V0A | V0C | SPD minimum bias trigger
      kINT7              = BIT( 1), // V0AND trigger, offline V0 selection
      kMUON              = BIT( 2), // Muon trigger, offline SPD or V0 selection
      kHighMult          = BIT( 3), // High-multiplicity trigger (threshold defined online), offline SPD or V0 selection
      kEMC1              = BIT( 4), // EMCAL trigger
      kCINT5             = BIT( 5), // V0OR minimum bias trigger
      kINT5              = BIT( 5), // V0OR minimum bias trigger      
      kCMUS5             = BIT( 6), // Muon trigger, offline V0 selection
      kMUSPB             = BIT( 6), // idem for PbPb
      kMuonSingleHighPt7 = BIT( 7), // Muon trigger : single muon high pt, CINT7 suite
      kMUSH7             = BIT( 7), // Muon trigger : single muon high pt, CINT7 suite
      kMUSHPB            = BIT( 7), // idem for PbPb
      kMuonLikeLowPt7    = BIT( 8), // Muon trigger : like sign muon low pt, CINT7 suite
      kMUL7              = BIT( 8), // Muon trigger : like sign muon low pt, CINT7 suite
      kMuonLikePB        = BIT( 8), // idem for PbPb
      kMuonUnlikeLowPt7  = BIT( 9), // Muon trigger : unlike sign muon low pt, CINT7 suite
      kMUU7              = BIT( 9), // Muon trigger : unlike sign muon low pt, CINT7 suite
      kMuonUnlikePB      = BIT( 9), // idem for PbPb
      kEMC7              = BIT(10), // EMCAL trigger, CINT7 suite
      kEMC8              = BIT(10), // EMCAL trigger, CINT8 suite
      kMUS7              = BIT(11), // Muon trigger : single muon low pt, CINT7 suite
      kMuonSingleLowPt7  = BIT(11), // Muon trigger : single muon low pt, CINT7 suite
      kPHI1              = BIT(12), // PHOS trigger, CINT1 suite
      kPHI7              = BIT(13), // PHOS trigger, CINT7 suite
      kPHI8              = BIT(13), // PHOS trigger, CINT8 suite
      kPHOSPb            = BIT(13), // idem for PbPb
      kEMCEJE            = BIT(14), // EMCAL jet patch trigger
      kEMCEGA            = BIT(15), // EMCAL gamma trigger
      kCentral           = BIT(16), // PbPb central collision trigger
      kSemiCentral       = BIT(17), // PbPb semicentral collision trigger
      kDG5               = BIT(18), // Double gap diffractive
      kZED               = BIT(19), // ZDC electromagnetic dissociation
      kSPI7              = BIT(20), // Power interaction trigger
      kSPI               = BIT(20), // Power interaction trigger
      kINT8              = BIT(21), // CINT8 trigger: 0TVX (T0 vertex) trigger
      kMuonSingleLowPt8  = BIT(22), // Muon trigger : single muon, low pt, T0 selection, CINT8 suite
      kMuonSingleHighPt8 = BIT(23), // Muon trigger : single muon, high pt, T0 selection, CINT8 suite
      kMuonLikeLowPt8    = BIT(24), // Muon trigger : like sign muon, low pt, T0 selection, CINT8 suite
      kMuonUnlikeLowPt8  = BIT(25), // Muon trigger : unlike sign muon, low pt, T0 selection, CINT8 suite
      kMuonUnlikeLowPt0  = BIT(26), // Muon trigger : unlike sign muon, low pt, no additional L0 requirement
      kUserDefined       = BIT(27), // Set when custom trigger classes are set in AliPhysicsSelection, offline SPD or V0 selection
      kTRD               = BIT(28), // TRD trigger
      // Bits 29 and above are reserved for FLAGS
      kFastOnly          = BIT(30), // The fast cluster fired. This bit is set in to addition another trigger bit, e.g. kMB
      kAny               = 0xffffffff, // to accept any trigger
      kAnyINT            = kMB | kINT7 | kINT5 | kINT8 | kSPI7 // to accept any interaction (aka minimum bias) trigger
  };

  AliVEvent() { }
  virtual ~AliVEvent() { } 
  AliVEvent(const AliVEvent& vEvnt); 
  AliVEvent& operator=(const AliVEvent& vEvnt);

  // Services
  virtual void AddObject(TObject* obj) = 0;
  virtual TObject* FindListObject(const char *name) const = 0;
  virtual TList* GetList() const = 0;

  virtual void CreateStdContent() = 0;
  virtual void GetStdContent() = 0;

  virtual void ReadFromTree(TTree *tree, Option_t* opt) = 0;
  virtual void WriteToTree(TTree* tree) const = 0;

  virtual void Reset() = 0;
  //virtual void ResetStdContent() = 0;
  virtual void SetStdNames() = 0;

  virtual void Print(Option_t *option="") const = 0;

  // Header
  virtual AliVHeader* GetHeader() const = 0;
  //
  // field initialization
  virtual Bool_t InitMagneticField() const {return kFALSE;}

  // Delegated methods for fESDRun or AODHeader
  
  virtual void     SetRunNumber(Int_t n) = 0;
  virtual void     SetPeriodNumber(UInt_t n) = 0;
  virtual void     SetMagneticField(Double_t mf) = 0;
  
  virtual Int_t    GetRunNumber() const = 0;
  virtual UInt_t   GetPeriodNumber() const = 0;
  virtual Double_t GetMagneticField() const = 0;

  virtual Double_t GetDiamondX() const {return -999.;}
  virtual Double_t GetDiamondY() const {return -999.;}
  virtual void     GetDiamondCovXY(Float_t cov[3]) const
             {cov[0]=-999.; return;}

  // Delegated methods for fHeader
  virtual void      SetOrbitNumber(UInt_t n) = 0;
  virtual void      SetBunchCrossNumber(UShort_t n) = 0;
  virtual void      SetEventType(UInt_t eventType)= 0;
  virtual void      SetTriggerMask(ULong64_t n) = 0;
  virtual void      SetTriggerCluster(UChar_t n) = 0;

  virtual UInt_t    GetOrbitNumber() const = 0;
  virtual UShort_t  GetBunchCrossNumber() const = 0;
  virtual UInt_t    GetEventType()  const = 0;
  virtual ULong64_t GetTriggerMask() const = 0;
  virtual UChar_t   GetTriggerCluster() const = 0;
  virtual TString   GetFiredTriggerClasses() const = 0;
  virtual Bool_t    IsTriggerClassFired(const char* /*name*/) const {return 0;}
  virtual Double_t  GetZDCN1Energy() const = 0;
  virtual Double_t  GetZDCP1Energy() const = 0;
  virtual Double_t  GetZDCN2Energy() const = 0;
  virtual Double_t  GetZDCP2Energy() const = 0;
  virtual Double_t  GetZDCEMEnergy(Int_t i) const = 0;
 
  // Tracks
  virtual AliVParticle *GetTrack(Int_t i) const = 0;
  virtual AliVTrack    *GetVTrack(Int_t /*i*/) const {return NULL;}
  //virtual AliVTrack    *GetVTrack(Int_t /*i*/) {return NULL;}
  //virtual Int_t        AddTrack(const AliVParticle *t) = 0;
  virtual Int_t        GetNumberOfTracks() const = 0;
  virtual Int_t        GetNumberOfV0s() const = 0;
  virtual Int_t        GetNumberOfCascades() const = 0;

  // TOF header and T0 methods
  virtual const AliTOFHeader *GetTOFHeader() const {return NULL;}
  virtual Float_t GetEventTimeSpread() const {return 0.;}
  virtual Float_t GetTOFTimeResolution() const {return 0.;}
  virtual Double32_t GetT0TOF(Int_t icase) const {return 0.0*icase;}
  virtual const Double32_t * GetT0TOF() const {return NULL;}
  virtual Float_t GetT0spread(Int_t /*i*/) const {return 0.;}

  // Calorimeter Clusters/Cells
  virtual AliVCluster *GetCaloCluster(Int_t)   const {return 0;}
  virtual Int_t GetNumberOfCaloClusters()      const {return 0;}
  virtual Int_t GetEMCALClusters(TRefArray *)  const {return 0;}
  virtual Int_t GetPHOSClusters (TRefArray *)  const {return 0;}
  virtual AliVCaloCells *GetEMCALCells()       const {return 0;}
  virtual AliVCaloCells *GetPHOSCells()        const {return 0;}
  const TGeoHMatrix* GetPHOSMatrix(Int_t /*i*/)    const {return NULL;}
  const TGeoHMatrix* GetEMCALMatrix(Int_t /*i*/)   const {return NULL;}
  virtual AliVCaloTrigger *GetCaloTrigger(TString /*calo*/) const {return NULL;} 

	
  // Primary vertex
  virtual Bool_t IsPileupFromSPD(Int_t /*minContributors*/, 
				 Double_t /*minZdist*/, 
				 Double_t /*nSigmaZdist*/, 
				 Double_t /*nSigmaDiamXY*/, 
				 Double_t /*nSigmaDiamZ*/)
				 const{
    return kFALSE;
  }

  // Tracklets
  virtual AliVMultiplicity* GetMultiplicity() const {return 0;}
  virtual Int_t             GetNumberOfITSClusters(Int_t) const {return 0;}

  virtual Bool_t IsPileupFromSPDInMultBins() const {
    return kFALSE;    
  }
  virtual AliCentrality* GetCentrality()                          = 0;
  virtual AliEventplane* GetEventplane()                          = 0;
  virtual Int_t        EventIndex(Int_t itrack)             const = 0;
  virtual Int_t        EventIndexForCaloCluster(Int_t iclu) const = 0;
  virtual Int_t        EventIndexForPHOSCell(Int_t icell)   const = 0;
  virtual Int_t        EventIndexForEMCALCell(Int_t icell)  const = 0;  

  virtual AliVVZERO *GetVZEROData() const = 0;   
  virtual const Float_t* GetVZEROEqFactors() const {return NULL;}
  virtual Float_t        GetVZEROEqMultiplicity(Int_t /* i */) const {return -1;}
  virtual void           SetVZEROEqFactors(Float_t /* factors */[64]) const {return;}
  virtual AliVZDC   *GetZDCData() const = 0;

  virtual AliVAD *GetADData() const { return NULL;}  

  virtual Int_t GetNumberOfTrdTracks() const { return 0; }
  virtual AliVTrdTrack* GetTrdTrack(Int_t /* iTrack */) const { return 0x0; }

  virtual Int_t     GetNumberOfESDTracks()  const { return 0; }
  virtual Int_t     GetEventNumberInFile() const {return 0;}

  //used in calibration:
  virtual Int_t            GetV0(AliESDv0&, Int_t /*iv0*/) const {return 0;}
  virtual UInt_t           GetTimeStamp() const { return 0; }
  virtual AliVfriendEvent* FindFriend() const { return 0; }
  virtual void             SetFriendEvent( AliVfriendEvent* ) {}
  virtual UInt_t           GetEventSpecie() const { return 0; }
  virtual AliESDkink*      GetKink(Int_t /*i*/) const { return NULL; }
  virtual Int_t            GetNumberOfKinks() const { return 0; }
 
  virtual Int_t GetVZEROData( AliESDVZERO & ) const {return -1;}

  // Primary vertex
  virtual const AliVVertex   *GetPrimaryVertex() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexSPD() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexTPC() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexTracks() const {return 0x0;}

  virtual Int_t GetPrimaryVertex( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexTPC( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexSPD( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexTracks( AliESDVertex & ) const {return 0;}

  // event status
  virtual Bool_t IsIncompleteDAQ() {return kFALSE;}


  virtual void ConnectTracks() {}
  virtual EDataLayoutType GetDataLayoutType() const = 0;
  const char* Whoami();
  virtual ULong64_t  GetSize()  const {return 0;}

  ClassDef(AliVEvent, 3)  // base class for AliEvent data
};
#endif 


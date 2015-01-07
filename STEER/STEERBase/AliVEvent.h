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
#include "AliBits.h"
class AliVfriendEvent;
class AliCentrality;
class AliEventplane;
class AliVVZERO;
class AliVZDC;
class AliVMFT;   // AU
class AliESDkink;
class AliESDv0;
class AliESDVertex;
class AliVTrack;


class AliVEvent : public TObject {

public:
  enum EDataLayoutType { kESD, kMC, kAOD, kMixed, kFlat };
  typedef AliBits EOfflineTriggerTypes;
  static const Int_t NTRIGGERBITS;
  static const AliBits kMB;                // Minimum bias [INT1 = (V0A | V0C | SPD) in pp] or [PBI = (V0A & V0C & ZDCtime) in PbPb2011] 
  static const AliBits kINT7;              // Minimum bias INT7 = V0A & V0C
  static const AliBits kMUON;              // low-pt single muon, INT1 suite
  static const AliBits kHighMult;          // High-multiplicity trigger (threshold defined online)
  static const AliBits kEMC1;              // EMCAL L0 trigger, INT1 suite
  static const AliBits kCINT5;             // Minimum bias INT5 = V0A | V0C
  static const AliBits kCMUS5;             // low-pt single muon, INT5 suite
  static const AliBits kMUSPB;             // low-pt single muon, PBI suite
  static const AliBits kMUSH7;             // high-pt single muon, INT7 suite
  static const AliBits kMUSHPB;            // high-pt single muon, PBI suite
  static const AliBits kMUL7;              // low-pt like sign dimuon, INT7 suite
  static const AliBits kMuonLikePB;        // low-pt like sign dimuon, PBI suite
  static const AliBits kMUU7;              // low-pt unlike sign dimuon, INT7 suite
  static const AliBits kMuonUnlikePB;      // low-pt unlike sign dimuon, PBI suite
  static const AliBits kEMC7;              // EMCAL L0 trigger, INT7 suite
  static const AliBits kMUS7;              // low-pt single muon, INT7 suite
  static const AliBits kPHI1;              // PHOS L0 trigger, INT1 suite
  static const AliBits kPHI7;              // PHOS L0 trigger, INT7 suite
  static const AliBits kPHOSPb;            // PHOS L0 trigger, PBI suite
  static const AliBits kEMCEJE;            // EMCAL L1 jet trigger
  static const AliBits kEMCEGA;            // EMCAL L1 gamma trigger
  static const AliBits kCentral;           // Central PbPb trigger
  static const AliBits kSemiCentral;       // Semicentral PbPb trigger
  static const AliBits kDG5;               // Double gap diffractive
  static const AliBits kZED;               // ZDC electromagnetic dissociation
  static const AliBits kSPI7;              // Power interaction trigger, INT7 suite
  static const AliBits kSPI;               // Power interaction trigger
  static const AliBits kINT8;              // INT8 = 0TVX = T0-vertex requirement
  static const AliBits kMuonSingleLowPt8;  // low-pt single muon, INT8 suite
  static const AliBits kMuonSingleHighPt8; // high-pt single muon, INT8 suite
  static const AliBits kMuonLikeLowPt8;    // low-pt like sign dimuon, INT8 suite
  static const AliBits kMuonUnlikeLowPt8;  // low-pt unlike sign dimuon, INT8 suite
  static const AliBits kMuonUnlikeLowPt0;  // low-pt unlike sign dimuon, no additional requirement
  static const AliBits kUserDefined;       // Set when custom trigger classes are set in AliPhysicsSelection
  static const AliBits kTRD;               // Mixture of TRD triggers
  static const AliBits kUserDefined2;      // Set when custom trigger classes are set in AliPhysicsSelection
  static const AliBits kFastOnly;          // The fast cluster fired. Set in addition to another trigger bit
  static const AliBits kUserDefined3;      // Set when custom trigger classes are set in AliPhysicsSelection
  static const AliBits kAny;               // Accept any trigger
  static const AliBits kAnyINT;            // Any interaction (minimum bias) trigger
  static const AliBits kPHI8;              // PHOS L0 trigger, INT8 suite
  static const AliBits kEmcalL1GammaHigh7; // EMCAL L1 gamma trigger, high threshold, INT7 suite
  static const AliBits kEmcalL1GammaLow7;  // EMCAL L1 gamma trigger, low threshold, INT7 suite
  static const AliBits kEmcalL1JetHigh7;   // EMCAL L1 jet trigger, high threshold, INT7 suite
  static const AliBits kEmcalL1JetLow7;    // EMCAL L1 jet trigger, low threshold, INT7 suite
  static const AliBits kEMC8;              // EMCAL L0 trigger, INT8 suite
  static const AliBits kEmcalL1GammaHigh8; // EMCAL L1 gamma trigger, high threshold, INT8 suite
  static const AliBits kEmcalL1GammaLow8;  // EMCAL L1 gamma trigger, low threshold, INT8 suite
  static const AliBits kEmcalL1JetHigh8;   // EMCAL L1 jet trigger, high threshold, INT8 suite
  static const AliBits kEmcalL1JetLow8;    // EMCAL L1 jet trigger, low threshold, INT8 suite
  static const AliBits kINT7HJT;           // TRD jet trigger, INT7 suite
  static const AliBits kINT7HSE;           // TRD high-pt electron trigger, INT7 suite
  static const AliBits kINT7HQU;           // TRD quarkonium trigger, INT7 suite
  static const AliBits kEMC7HQU;           // TRD quarkonium trigger + EMCAL L0, INT7 suite
  static const AliBits kEMC7HEE;           // TRD high-pt electron trigger in EMCAL acceptance + EMCAL L0, INT7 suite
  static const AliBits kINT8HJT;           // TRD jet trigger, INT7 suite
  static const AliBits kINT8HSE;           // TRD high-pt electron trigger, INT7 suite
  static const AliBits kINT8HQU;           // TRD quarkonium trigger, INT7 suite
  static const AliBits kEMC8HQU;           // TRD quarkonium trigger + EMCAL L0, INT7 suite
  static const AliBits kEMC8HEE;           // TRD high-pt electron trigger in EMCAL acceptance + EMCAL L0, INT7 suite
  static const AliBits kSPI8;              // Power interaction trigger, INT8 suite
  static const AliBits kSTP;               // SPD topology trigger (2 hits in layer0 + 2 hits in layer1 + topology)
  static const AliBits kOMU;               // TOF topology trigger (2 hits back-to-back)
  static const AliBits kCUP7;              // Central barrel ultra-peripheral trigger (SPD and TOF topology, V0 veto)

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

  virtual Int_t GetNumberOfTrdTracks() const { return 0; }
  virtual AliVTrdTrack* GetTrdTrack(Int_t /* iTrack */) const { return 0x0; }

  virtual Int_t     GetNumberOfESDTracks()  const { return 0; }
  virtual Int_t     GetEventNumberInFile() const {return 0;}

  //used in calibration:
  virtual Int_t            GetV0(AliESDv0&, Int_t /*iv0*/) const {return 0;}
  virtual UInt_t           GetTimeStamp() const { return 0; }
  virtual AliVfriendEvent* FindFriend() const { return 0; }
  virtual UInt_t           GetEventSpecie() const { return 0; }
  virtual AliESDkink*      GetKink(Int_t /*i*/) const { return NULL; }
  virtual Int_t            GetNumberOfKinks() const { return 0; }

  // Primary vertex
  virtual const AliVVertex   *GetPrimaryVertex() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexSPD() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexTPC() const {return 0x0;}
  virtual const AliVVertex   *GetPrimaryVertexTracks() const {return 0x0;}

  virtual Int_t GetPrimaryVertex( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexTPC( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexSPD( AliESDVertex & ) const {return 0;}
  virtual Int_t GetPrimaryVertexTracks( AliESDVertex & ) const {return 0;}

  virtual void ConnectTracks() {}
  virtual EDataLayoutType GetDataLayoutType() const = 0;
  const char* Whoami();

  ClassDef(AliVEvent, 3)  // base class for AliEvent data
};
#endif 


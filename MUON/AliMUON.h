#ifndef ALIMUON_H
#define ALIMUON_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* $Id$ */


////////////////////////////////////////////////
//  AliDetector Class for MUON subsystem      //
////////////////////////////////////////////////
#include "AliDetector.h"
#include "AliMUONData.h"
#include "AliMUONChamber.h"

class AliLoader;
class AliMUONLocalTrigger;
class AliMUONGlobalTrigger;
class AliMUONTriggerCircuit;
class AliMUONTriggerDecision;
class AliSegmentation;
class AliMUONResponse;
class AliMUONMerger;
class AliMUONHit;
class AliMUONPadHit;
class AliMUONRawCluster;
class AliMUONClusterFinderVS;
class AliMUONReconstHit;
class AliMUONMerger;
class AliMUONVGeometryBuilder;
class AliESD;

class TVector;
#include "TObjArray.h"
class TFile;
class TTree;


class AliMUON : public  AliDetector {
 public:
    AliMUON();
    AliMUON(const char *name, const char *title);
    AliMUON(const AliMUON& rMUON);
    virtual       ~AliMUON();
   
    void           AddGeometryBuilder(AliMUONVGeometryBuilder* geomBuilder);
    virtual void   BuildGeometry();
    Int_t          DistancetoPrimitive(Int_t px, Int_t py);
    AliMUONData*   GetMUONData() {return fMUONData;}
    virtual Int_t  IsVersion()   const {return 0;}

    // MUONLoader definition
    virtual AliLoader* MakeLoader(const char* topfoldername); //builds standard getter (AliLoader type)
    // Interface with AliMUONData
    virtual void       MakeBranch(Option_t *opt=" ") {GetMUONData()->MakeBranch(opt);}
    virtual void       SetTreeAddress();
    virtual void       ResetHits()                   {GetMUONData()->ResetHits();}
    virtual void       ResetDigits()                 {GetMUONData()->ResetDigits();}
    virtual void       ResetTrigger()                {GetMUONData()->ResetTrigger();}
    virtual void       ResetRawClusters()            {GetMUONData()->ResetRawClusters();}
    virtual void       SetSplitLevel(Int_t SplitLevel)     {fSplitLevel=SplitLevel;}
    // Cluster Finding
    virtual void   Digits2Reco();
    virtual void   FindClusters();
    // Digitisation 
    virtual AliDigitizer* CreateDigitizer(AliRunDigitizer* manager) const;
    virtual void   SDigits2Digits();      
    virtual void   Hits2SDigits();
    // Configuration Methods (per station id)
    //
    // Set Chamber Segmentation Parameters
    // id refers to the station and isec to the cathode plane
    // Set Z values for all chambers
    virtual void SetChambersZ(const Float_t *Z);
    virtual void SetChambersZToDefault(void);
    virtual void SetPadSize(Int_t id, Int_t isec, Float_t p1, Float_t p2);
    // Set Signal Generation Parameters
    virtual void   SetSigmaIntegration(Int_t id, Float_t p1);
    virtual void   SetChargeSlope(Int_t id, Float_t p1);
    virtual void   SetChargeSpread(Int_t id, Float_t p1, Float_t p2);
    virtual void   SetMaxAdc(Int_t id, Int_t p1);
    // Set Segmentation and Response Model
    virtual void   SetSegmentationModel(Int_t id, Int_t isec,
					AliSegmentation *segmentation);
    virtual void   SetResponseModel(Int_t id, AliMUONResponse *response);
    virtual void   SetNsec(Int_t id, Int_t nsec);
    // Set Reconstruction Model
    virtual void   SetReconstructionModel(Int_t id, AliMUONClusterFinderVS *reconstruction);
    // Set Merger/Digitizer
    virtual void   SetMerger(AliMUONMerger* merger);
    virtual AliMUONMerger* Merger();
    
    // Set Stepping Parameters
    virtual void   SetMaxStepGas(Float_t p1);
    virtual void   SetMaxStepAlu(Float_t p1);
    virtual void   SetMaxDestepGas(Float_t p1);
    virtual void   SetMaxDestepAlu(Float_t p1);
    virtual void   SetAcceptance(Bool_t acc=0, Float_t angmin=2, Float_t angmax=9);
   
    // Get Stepping Parameters
    virtual Float_t  GetMaxStepGas() const;
    virtual Float_t  GetMaxStepAlu() const;
    virtual Float_t  GetMaxDestepGas() const;
    virtual Float_t  GetMaxDestepAlu() const;
   
    // get Trigger answer
    void   Trigger(Int_t nev);
    // Return reference to Chamber #id
    virtual AliMUONChamber& Chamber(Int_t id)
      {return *((AliMUONChamber *) (*fChambers)[id]);}
    // Return reference to Circuit #id
    virtual AliMUONTriggerCircuit& TriggerCircuit(Int_t id)
      {return *((AliMUONTriggerCircuit *) (*fTriggerCircuits)[id]);}
    // Retrieve pad hits for a given Hit
    virtual AliMUONPadHit* FirstPad(AliMUONHit *hit, TClonesArray *padHits);
    virtual AliMUONPadHit* NextPad(TClonesArray *padHits);
    // Return pointers to digits
    AliMUONRawCluster    *RawCluster(Int_t ichamber, Int_t icathod,
				     Int_t icluster);
    // Copy Operator
    AliMUON& operator = (const AliMUON& rhs);
    
    // Reconstruct fct for AliModule
    virtual void Reconstruct() const;

    
    // ESD filling for AliModule
    virtual void FillESD(AliESD* ) const;

 protected:
    Int_t                 fNCh;                // Number of chambers   
    Int_t                 fNTrackingCh;        // Number of tracking chambers*
    AliMUONData*          fMUONData;           // Data container for MUON subsystem  
    Int_t                 fSplitLevel;         // Splitlevel when making branches in outfiles.
    TObjArray*            fChambers;           // List of Tracking Chambers
    TObjArray            *fGeometryBuilders;   // List of Geometry Builders
    TObjArray*            fTriggerCircuits;    // List of Trigger Circuits
   
    //
    Bool_t   fAccCut;          //Transport acceptance cut
    Float_t  fAccMin;          //Minimum acceptance cut used during transport
    Float_t  fAccMax;          //Minimum acceptance cut used during transport
    //  
    //  Stepping Parameters
    Float_t fMaxStepGas;      // Maximum step size inside the chamber gas
    Float_t fMaxStepAlu;      // Maximum step size inside the chamber aluminum
    Float_t fMaxDestepGas;    // Maximum relative energy loss in gas
    Float_t fMaxDestepAlu;    // Maximum relative energy loss in aluminum
    
    // Pad Iterator
    Int_t fMaxIterPad;        // Maximum pad index
    Int_t fCurIterPad;        // Current pad index
    // Background eent for event mixing
    Text_t *fFileName;           // ! File with background hits
    AliMUONMerger *fMerger;   // ! pointer to merger
    
    ClassDef(AliMUON,3)  // MUON Detector base class
};
#endif


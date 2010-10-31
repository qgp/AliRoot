/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/
//
// Cut container class for the ALICE HFE group
// Serves also as interface to the correction Framework
// Provides a set of standard cuts
//
#ifndef ALIHFECUTS_H
#define ALIHFECUTS_H

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

#ifndef ALIHFEEXTRACUTS_H
#include "AliHFEextraCuts.h"
#endif

class AliCFManager;
class AliESDtrack;
class AliMCParticle;

class TObjArray;
class TList;

class AliHFEcuts : public TNamed{
  public:
    typedef enum{
      kStepMCGenerated = 0,
      kStepMCsignal = 1,
      kStepMCInAcceptance = 2,
      kStepRecNoCut = 3,
      kStepRecKineITSTPC = 4,
      kStepRecPrim = 5,
      kStepHFEcutsITS = 6,
      kStepHFEcutsTRD = 7,
      kStepPID = 8
    } CutStep_t;
    typedef enum{
      kEventStepGenerated = 0,
      kEventStepRecNoCut = 1,
      kEventStepReconstructed = 2
    } EventCutStep_t;
    enum{
      kNcutStepsEvent = 3,
      kNcutStepsTrack = 9,
      kNcutStepsESDtrack = 6 
    };    // Additional constants

    AliHFEcuts();
    AliHFEcuts(const Char_t *name, const Char_t *title);
    AliHFEcuts(const AliHFEcuts &c);
    AliHFEcuts &operator=(const AliHFEcuts &c);
    void Copy(TObject &o) const;
    ~AliHFEcuts();
    
    void Initialize(AliCFManager *cfm);
    void Initialize();

    Bool_t CheckParticleCuts(CutStep_t step, TObject *o);
  
    TList *GetQAhistograms() const { return fHistQA; }
    
    void SetQAOn() {SetBit(kDebugMode, kTRUE); };
    void UnsetQA() {SetBit(kDebugMode, kFALSE); };
    Bool_t IsQAOn() const { return TestBit(kDebugMode); };
    void SetAOD() { SetBit(kAOD, kTRUE); }
    void SetESD() { SetBit(kAOD, kFALSE); }
    Bool_t IsAOD() const { return TestBit(kAOD); }
    Bool_t IsESD() const { return !TestBit(kAOD); }
    
    // Getters
    Bool_t IsRequireITSpixel() const { return TESTBIT(fRequirements, kITSPixel); };
    Bool_t IsRequireMaxImpactParam() const { return TESTBIT(fRequirements, kMaxImpactParam); };
    Bool_t IsRequirePrimary() const { return TESTBIT(fRequirements, kPrimary); };
    Bool_t IsRequireProdVertex() const { return TESTBIT(fRequirements, kProductionVertex); };
    Bool_t IsRequireSigmaToVertex() const { return TESTBIT(fRequirements, kSigmaToVertex); };
    Bool_t IsRequireDCAToVertex() const {return TESTBIT(fRequirements, kDCAToVertex); };
    Bool_t IsRequireKineMCCuts() const {return TESTBIT(fRequirements, kKineMCCuts); };
    
    // Setters
    inline void SetCutITSpixel(UChar_t cut);
    void SetCheckITSLayerStatus(Bool_t checkITSLayerStatus) { fCheckITSLayerStatus = checkITSLayerStatus; }
    void SetMinNClustersTPC(UChar_t minClustersTPC) { fMinClustersTPC = minClustersTPC; }
    void SetMinNTrackletsTRD(UChar_t minNtrackletsTRD) { fMinTrackletsTRD = minNtrackletsTRD; }
    void SetMaxChi2perClusterTPC(Double_t chi2) { fMaxChi2clusterTPC = chi2; };
    inline void SetMaxImpactParam(Double_t radial, Double_t z);
    void SetMinRatioTPCclusters(Double_t minRatioTPC) { fMinClusterRatioTPC = minRatioTPC; };
    void SetPtRange(Double_t ptmin, Double_t ptmax){fPtRange[0] = ptmin; fPtRange[1] = ptmax;};
    inline void SetProductionVertex(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax);
    inline void SetSigmaToVertex(Double_t sig);
    
    inline void CreateStandardCuts();
    
    // Requirements
    void SetRequireDCAToVertex() { SETBIT(fRequirements, kDCAToVertex); CLRBIT(fRequirements, kSigmaToVertex); };
    void SetRequireIsPrimary() { SETBIT(fRequirements, kPrimary); };
    void SetRequireITSPixel() { SETBIT(fRequirements, kITSPixel); }
    void SetRequireProdVertex() { SETBIT(fRequirements, kProductionVertex); };
    void SetRequireSigmaToVertex() { SETBIT(fRequirements, kSigmaToVertex); CLRBIT(fRequirements, kDCAToVertex); };
    void SetRequireKineMCCuts() { SETBIT(fRequirements, kKineMCCuts); };

    void SetDebugLevel(Int_t level) { fDebugLevel = level; };
    Int_t GetDebugLevel() const { return fDebugLevel; };

  private:
    enum{
      kDebugMode = BIT(14),
      kAOD = BIT(15)
    };
    typedef enum{
      kPrimary = 0,
      kProductionVertex = 1,
      kSigmaToVertex = 2,
      kDCAToVertex = 3,
      kITSPixel = 4,
      kMaxImpactParam = 5,
      kKineMCCuts = 6
    } Require_t;
    void SetParticleGenCutList();
    void SetAcceptanceCutList();
    void SetRecKineITSTPCCutList();
    void SetRecPrimaryCutList();
    void SetHFElectronITSCuts();
    void SetHFElectronTRDCuts();
    void SetEventCutList(Int_t istep);
  
    ULong64_t fRequirements;  	  // Bitmap for requirements
    Double_t fDCAtoVtx[2];	      // DCA to Vertex
    Double_t fProdVtx[4];	        // Production Vertex
    Double_t fPtRange[2];	        // pt range
    UChar_t fMinClustersTPC;	    // Min.Number of TPC clusters
    UChar_t fMinTrackletsTRD;	    // Min. Number of TRD tracklets
    UChar_t fCutITSPixel;	        // Cut on ITS pixel
    Bool_t  fCheckITSLayerStatus;       // Check ITS layer status
    Double_t fMaxChi2clusterTPC;	// Max Chi2 per TPC cluster
    Double_t fMinClusterRatioTPC;	// Min. Ratio findable / found TPC clusters
    Double_t fSigmaToVtx;	        // Sigma To Vertex
    
    TList *fHistQA;		            //! QA Histograms
    TObjArray *fCutList;	        //! List of cut objects(Correction Framework Manager)

    Int_t fDebugLevel;            // Debug Level
    
  ClassDef(AliHFEcuts, 1)         // Container for HFE cuts
};

//__________________________________________________________________
void AliHFEcuts::SetProductionVertex(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax){
  // Set the production vertex constraint
  SetRequireProdVertex();
  fProdVtx[0] = xmin;
  fProdVtx[1] = xmax;
  fProdVtx[2] = ymin;
  fProdVtx[3] = ymax;
}

//__________________________________________________________________
void AliHFEcuts::SetSigmaToVertex(Double_t sig){
  SetRequireSigmaToVertex();
  fSigmaToVtx = sig;
}

//__________________________________________________________________
void AliHFEcuts::SetMaxImpactParam(Double_t radial, Double_t z){
  SetRequireDCAToVertex();
  fDCAtoVtx[0] = radial;
  fDCAtoVtx[1] = z;
}

//__________________________________________________________________
void AliHFEcuts::SetCutITSpixel(UChar_t cut){
  SetRequireITSPixel();
  fCutITSPixel = cut;
}

//__________________________________________________________________
void AliHFEcuts::CreateStandardCuts(){
  //
  // Standard Cuts defined by the HFE Group
  //
  SetRequireProdVertex();
  fProdVtx[0] = 0;
  fProdVtx[1] = 3;
  fProdVtx[2] = 0;
  fProdVtx[3] = 3;
  //SetRequireDCAToVertex();
  //fDCAtoVtx[0] = 0.5;
  //fDCAtoVtx[1] = 1.5;
  fMinClustersTPC = 80;
  fMinTrackletsTRD = 0;
  SetRequireITSPixel();
  fCutITSPixel = AliHFEextraCuts::kFirst;
  fMaxChi2clusterTPC = 3.5;
  fMinClusterRatioTPC = 0.6;
  fPtRange[0] = 0.1;
  fPtRange[1] = 20.;
  SetRequireKineMCCuts();
}
#endif

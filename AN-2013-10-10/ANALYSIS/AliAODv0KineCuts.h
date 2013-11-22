/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
/*
 * plesae see source file for more details
 */
#ifndef ALIAODV0KINECUTS_H
#define ALIAODV0KINECUTS_H

#include <TObject.h>
#include <TArrayI.h>

class AliAODv0;
class AliAODEvent;
class AliVEvent;
class AliAODTrack;
class AliVTrack;
class AliKFParticle;
class AliKFVertex;

class AliAODv0KineCuts : public TObject{
 public:
  enum{ // Reconstructed V0
    kUndef = -1,
      kGamma = 0,
      kK0 = 1,
      kLambda = 2,
      kALambda = 3
      };
  enum{ // data types
      kPP = 0,
      kPbPb = 1,  // not yet implemented
      };
  enum{ // operation modes
      kPurity = 0, // purely kinematical selection
      kEffGamma = 1  // !!! involves TPC dEdx or nSimga cuts !!!
      };
  
  AliAODv0KineCuts();
  virtual ~AliAODv0KineCuts();

  AliAODv0KineCuts(const AliAODv0KineCuts &ref);
  AliAODv0KineCuts &operator=(const AliAODv0KineCuts &ref);

  // main selection function - called once per V0 candidate
  Bool_t ProcessV0(AliAODv0* const v0, Int_t &pdgV0, Int_t &pdgP, Int_t &pdgN) const;
  Bool_t ProcessV0(AliAODv0* const v0, Int_t &pdgP, Int_t &pdgN) const;

  // must be called by the user
  void SetEvent(AliAODEvent* const event);
  void SetEvent(AliVEvent* const event);
  void SetPrimaryVertex(AliKFVertex* const v);

  // user can select an operation modes [see .cxx for details]
  void   SetMode(Int_t mode, Int_t type);
  void   SetMode(Int_t mode, const char* type);
  void	 UseExternalVertex(Bool_t use_external=kTRUE);
  AliKFParticle *CreateMotherParticle(const AliVTrack* const pdaughter, const AliVTrack* const ndaughter, Int_t pspec, Int_t nspec) const;
  void      SetCuts();                          // setup cuts for selected fMode and fType, see source file for details
  //
  // setter functions for V0 cut values
  // for default values see the constructor
  // see the default contructor for comments
  //

  // single track cuts
  void   SetNTPCclusters(Int_t n) { fTPCNcls = n; };
  void   SetTPCrefit(Bool_t r = kTRUE) { fTPCrefit = r; };
  void   SetTPCclusterratio(Float_t r) { fTPCclsRatio = r; };
  void   SetNoKinks(Bool_t k = kTRUE) { fNoKinks = k; };

  // gamma cuts
  void   SetGammaCutChi2NDF(Float_t val)  { fGcutChi2NDF = val; };
  void   SetGammaCutCosPoint(Float_t * const val) { 
    fGcutCosPoint[0] = val[0];
    fGcutCosPoint[1] = val[1];
  };
  void   SetGammaCutDCA(Float_t * const val){
    fGcutDCA[0] = val[0];
    fGcutDCA[1] = val[1];
  };
  void   SetGammaCutVertexR(Float_t * const val){
    fGcutVertexR[0] = val[0];
    fGcutVertexR[1] = val[1];
  };
  void   SetGammaCutPsiPair(Float_t * const val){
    fGcutPsiPair[0] = val[0];
    fGcutPsiPair[1] = val[1];
  };
  void   SetGammaCutInvMass(Float_t val){
    fGcutInvMass = val;
  };
  // K0 cuts
  void   SetK0CutChi2NDF(Float_t val)  { fK0cutChi2NDF = val; };
  void   SetK0CutCosPoint(Float_t * const val) { 
    fK0cutCosPoint[0] = val[0];
    fK0cutCosPoint[1] = val[1];
  };
  void   SetK0CutDCA(Float_t * const val){
    fK0cutDCA[0] = val[0];
    fK0cutDCA[1] = val[1];
  };
  void   SetK0CutVertexR(Float_t * const val){
    fK0cutVertexR[0] = val[0];
    fK0cutVertexR[1] = val[1];
  };
  void   SetK0CutInvMass(Float_t * const val){
    fK0cutInvMass[0] = val[0];
    fK0cutInvMass[1] = val[1];
  };
  // lambda & anti-lambda cuts
  void   SetLambdaCutChi2NDF(Float_t val)  { fLcutChi2NDF = val; };
  void   SetLambdaCutCosPoint(Float_t * const val) { 
    fLcutCosPoint[0] = val[0];
    fLcutCosPoint[1] = val[1];
  };
  void   SetLambdaCutDCA(Float_t * const val){
    fLcutDCA[0] = val[0];
    fLcutDCA[1] = val[1];
  };
  void   SetLambdaCutVertexR(Float_t * const val){
    fLcutVertexR[0] = val[0];
    fLcutVertexR[1] = val[1];
  };
  void   SetLambdaCutInvMass(Float_t * const val){
    fLcutInvMass[0] = val[0];
    fLcutInvMass[1] = val[1];
  };
  

  Int_t  PreselectV0(AliAODv0* const v0) const;

  Bool_t CaseGamma(AliAODv0* const v0, Int_t &pdgV0, Int_t &pdgP, Int_t &pdgN) const;
  Bool_t CaseK0(AliAODv0* const v0, Int_t &pdgV0, Int_t &pdgP, Int_t &pdgN) const;
  Bool_t CaseLambda(AliAODv0* const v0, Int_t &pdgV0, Int_t &pdgP, Int_t &pdgN, Int_t id) const;

  Bool_t V0CutsCommon(const AliAODv0 * const v0) const;
  Bool_t SingleTrackCuts(AliAODv0 * const v0) const;
  Bool_t CheckSigns(AliAODv0* const v0) const;

  Double_t PsiPair(AliAODv0* const v0) const;
  Bool_t   GetConvPosXY(AliAODTrack * const ptrack, AliAODTrack * const ntrack, Double_t convpos[2]) const;
  Bool_t   GetHelixCenter(AliAODTrack * const track, Double_t b, Int_t charge, Double_t center[2]) const;

 protected:
  void Copy(TObject &ref) const;

 private:
  Bool_t    GammaEffCuts(AliAODv0 * const v0);  // set of cuts optimized for high gamma efficiency
  Bool_t    IsKinkMother(const AliAODTrack * const track) const;
  Bool_t    IsKinkDaughter(const AliAODTrack * const track) const;

 private:
  AliAODEvent           *fEvent;          // current event
  AliKFVertex           *fPrimaryVertex;  // primary vertex

  Int_t                 fType;            // data type: p-p or Pb-Pb
  Int_t                 fMode;            // current operation mode

  // single track cuts
  Int_t                 fTPCNcls;          // number of TPC clusters
  Bool_t                fTPCrefit;         // TPC refit - yes [kTRUE] or do not care [kFALSE]
  Float_t               fTPCclsRatio;      // min. TPC cluster ratio
  Bool_t                fNoKinks;          // kinks - no [kTRUE] or do not care [kFalse]
  TArrayI               fKinkMotherList;   // Kink mothers
  Int_t                 fNumberKinkMothers; // Number of kink mothers

  // gamma cut values
  Float_t               fGcutChi2NDF;      // Chi2NF cut value for the AliKFparticle gamma
  Float_t               fGcutCosPoint[2];  // cos of the pointing angle [min, max]
  Float_t               fGcutDCA[2];       // DCA between the daughter tracks [min, max]
  Float_t               fGcutVertexR[2];   // radius of the conversion point [min, max]
  Float_t               fGcutPsiPair[2];   // value of the psi pair cut [min, max]
  Float_t               fGcutInvMass;      // upper value on the gamma invariant mass
  // K0 cut values
  Float_t               fK0cutChi2NDF;     // Chi2NF cut value for the AliKFparticle K0
  Float_t               fK0cutCosPoint[2]; // cos of the pointing angle [min, max]
  Float_t               fK0cutDCA[2];      // DCA between the daughter tracks [min, max]
  Float_t               fK0cutVertexR[2];  // radius of the decay point [min, max]
  Float_t               fK0cutInvMass[2];  // invariant mass window
  // Lambda & anti-Lambda cut values
  Float_t               fLcutChi2NDF;      // Chi2NF cut value for the AliKFparticle K0
  Float_t               fLcutCosPoint[2];  // cos of the pointing angle [min, max]
  Float_t               fLcutDCA[2];       // DCA between the daughter tracks [min, max]
  Float_t               fLcutVertexR[2];   // radius of the decay point [min, max]
  Float_t               fLcutInvMass[2];   // invariant mass window
  Bool_t		fUseExternalVertex; // Is kTRUE if Vertex is set via SetPrimaryVertex()
  Bool_t		fDeleteVertex;	    // Is kTRUE if Vertex has been created in SetEvent() function
  
  ClassDef(AliAODv0KineCuts, 0);

};

#endif

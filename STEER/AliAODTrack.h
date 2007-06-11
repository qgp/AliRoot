#ifndef AliAODTrack_H
#define AliAODTrack_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD track base class
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include <TRef.h>
#include <TParticle.h>

#include "AliVirtualParticle.h"
#include "AliAODVertex.h"
#include "AliAODRedCov.h"

class AliAODTrack : public AliVirtualParticle {

 public:
  
  enum AODTrk_t {kUndef = -1, 
		 kPrimary, 
		 kSecondary, 
		 kOrphan};

  enum AODTrkBits_t {
    kIsDCA=BIT(14),   // set if fPosition is the DCA and not the position of the first point
    kUsedForVtxFit=BIT(15), // set if this track was used to fit the vertex it is attached to
    kUsedForPrimVtxFit=BIT(16) // set if this track was used to fit the primary vertex
  };

  enum AODTrkPID_t {
    kElectron     =  0,
    kMuon         =  1,
    kPion         =  2,
    kKaon         =  3,
    kProton       =  4,
    kDeuteron     =  5,
    kTriton       =  6,
    kHelium3      =  7,
    kAlpha        =  8,
    kUnknown      =  9,
    kMostProbable = -1
  };

  AliAODTrack();
  AliAODTrack(Int_t id,
	      Int_t label,
	      Double_t p[3],
	      Bool_t cartesian,
	      Double_t x[3],
	      Bool_t dca,
	      Double_t covMatrix[21],
	      Short_t q,
	      UChar_t itsClusMap,
	      Double_t pid[10],
	      AliAODVertex *prodVertex,
	      Bool_t usedForVtxFit,
	      Bool_t usedForPrimVtxFit,
	      AODTrk_t ttype=kUndef);

  AliAODTrack(Int_t id,
	      Int_t label,
	      Float_t p[3],
	      Bool_t cartesian,
	      Float_t x[3],
	      Bool_t dca,
	      Float_t covMatrix[21],
	      Short_t q,
	      UChar_t itsClusMap,
	      Float_t pid[10],
	      AliAODVertex *prodVertex,
	      Bool_t usedForVtxFit,
	      Bool_t usedForPrimVtxFit,
	      AODTrk_t ttype=kUndef);

  virtual ~AliAODTrack();
  AliAODTrack(const AliAODTrack& trk); 
  AliAODTrack& operator=(const AliAODTrack& trk);

  // kinematics
  virtual Double_t OneOverPt() const { return (fMomentum[0] != 0.) ? 1./fMomentum[0] : -999.; }
  virtual Double_t Phi()       const { return fMomentum[1]; }
  virtual Double_t Theta()     const { return fMomentum[2]; }
  
  virtual Double_t Px() const { return fMomentum[0] * TMath::Cos(fMomentum[1]); }
  virtual Double_t Py() const { return fMomentum[0] * TMath::Sin(fMomentum[1]); }
  virtual Double_t Pz() const { return fMomentum[0] / TMath::Tan(fMomentum[2]); }
  virtual Double_t Pt() const { return fMomentum[0]; }
  virtual Double_t P()  const { return TMath::Sqrt(Pt()*Pt()+Pz()*Pz()); }
  
  Double_t Chi2perNDF() const { return fChi2perNDF; }
  
  virtual Double_t M() const { return M(GetMostProbablePID()); }
  Double_t M(AODTrkPID_t pid) const;
  virtual Double_t E() const { return E(GetMostProbablePID()); }
  Double_t E(AODTrkPID_t pid) const;
  Double_t E(Double_t m) const { return TMath::Sqrt(P()*P() + m*m); }
  virtual Double_t Y() const { return Y(GetMostProbablePID()); }
  Double_t Y(AODTrkPID_t pid) const;
  Double_t Y(Double_t m) const;
  
  virtual Double_t Eta() const { return -TMath::Log(TMath::Tan(0.5 * fMomentum[2])); }

  virtual Short_t  Charge() const {return fCharge; }

  // PID
  virtual const Double_t *PID() const { return fPID; }
  AODTrkPID_t GetMostProbablePID() const;
  void ConvertAliPIDtoAODPID();

  template <class T> void GetPID(T *pid) const {
    for(Int_t i=0; i<10; ++i) pid[i]=fPID[i];}
 
  template <class T> void SetPID(const T *pid) {
    if(pid) for(Int_t i=0; i<10; ++i) fPID[i]=pid[i];
    else {for(Int_t i=0; i<10; fPID[i++]=0.); fPID[AliAODTrack::kUnknown]=1.;}}

  Int_t GetID() const { return fID; }
  Int_t GetLabel() const { return fLabel; } 
  Char_t GetType() const { return fType;}
  Bool_t GetUsedForVtxFit() const { return TestBit(kUsedForVtxFit); }
  Bool_t GetUsedForPrimVtxFit() const { return TestBit(kUsedForPrimVtxFit); }

  template <class T> void GetP(T *p) const {
    p[0]=fMomentum[0]; p[1]=fMomentum[1]; p[2]=fMomentum[2];}

  template <class T> void GetPxPyPz(T *p) const {
    p[0] = Px(); p[1] = Py(); p[2] = Pz();}

  template <class T> Bool_t GetPosition(T *x) const {
    x[0]=fPosition[0]; x[1]=fPosition[1]; x[2]=fPosition[2];
    return TestBit(kIsDCA);}

  template <class T> void SetCovMatrix(const T *covMatrix) {
    if(!fCovMatrix) fCovMatrix=new AliAODRedCov<6>();
    fCovMatrix->SetCovMatrix(covMatrix);}

  template <class T> Bool_t GetCovMatrix(T *covMatrix) const {
    if(!fCovMatrix) return kFALSE;
    fCovMatrix->GetCovMatrix(covMatrix); return kTRUE;}

  void RemoveCovMatrix() {delete fCovMatrix; fCovMatrix=NULL;}

  UChar_t GetITSClusterMap() const     { return (UChar_t)fITSMuonClusterMap; }
  UInt_t  GetMUONClusterMap() const    { return fITSMuonClusterMap/65536; }
  UInt_t  GetITSMUONClusterMap() const { return fITSMuonClusterMap; }

  AliAODVertex *GetProdVertex() const { return (AliAODVertex*)fProdVertex.GetObject(); }
  
  // print
  void  Print(const Option_t *opt = "") const;

  // setters
  void SetID(Int_t id) { fID = id; }
  void SetLabel(Int_t label) {fLabel = label; }

  template <class T> void SetPosition(const T *x, Bool_t isDCA = kFALSE);
  void SetDCA(Double_t d, Double_t z);
  void SetUsedForVtxFit(Bool_t used = kTRUE) { used ? SetBit(kUsedForVtxFit) : ResetBit(kUsedForVtxFit); }
  void SetUsedForPrimVtxFit(Bool_t used = kTRUE) { used ? SetBit(kUsedForPrimVtxFit) : ResetBit(kUsedForPrimVtxFit); }

  void SetOneOverPt(Double_t oneOverPt) { fMomentum[0] = oneOverPt; }
  void SetPt(Double_t pt) { fMomentum[0] = pt; };
  void SetPhi(Double_t phi) { fMomentum[1] = phi; }
  void SetTheta(Double_t theta) { fMomentum[2] = theta; }
  template <class T> void SetP(const T *p, Bool_t cartesian = kTRUE);
  void SetP() {fMomentum[0]=fMomentum[1]=fMomentum[2]=-999.;}

  void SetCharge(Short_t q) { fCharge = q; }
  void SetChi2perNDF(Double_t chi2perNDF) { fChi2perNDF = chi2perNDF; }

  void SetITSClusterMap(UChar_t itsClusMap)        { fITSMuonClusterMap = (UInt_t)itsClusMap; }
  void SetMuonClusterMap(UInt_t muonClusMap)       { fITSMuonClusterMap = muonClusMap*65536; }
  void SetITSMuonClusterMap(UInt_t itsMuonClusMap) { fITSMuonClusterMap = itsMuonClusMap; }

  void SetProdVertex(TObject *vertex) { fProdVertex = vertex; }

  // name and title
  void SetType(AODTrk_t ttype) { fType=ttype; }

 private :

  // Momentum & position
  Double32_t    fMomentum[3];       // momemtum stored in pt, phi, theta
  Double32_t    fPosition[3];       // position of first point on track or dca

  Double32_t    fPID[10];           // [0.,1.,8] pointer to PID object
  Double32_t    fChi2perNDF;        // chi2/NDF of mometum fit

  Int_t         fID;                // unique track ID, points back to the ESD track
  Int_t         fLabel;             // track label, points back to MC track
  
  AliAODRedCov<6> *fCovMatrix;      // covariance matrix (x, y, z, px, py, pz)
  TRef          fProdVertex;        // vertex of origin

  Char_t        fCharge;            // particle charge
  UInt_t        fITSMuonClusterMap; // map of ITS and muon clusters, one bit per layer (ITS: bit 1-8, muon: bit 17-32) 
  Char_t        fType;              // Track Type


  ClassDef(AliAODTrack,2);
};

#endif

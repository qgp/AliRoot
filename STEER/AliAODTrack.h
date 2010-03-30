#ifndef AliAODTrack_H
#define AliAODTrack_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD track implementation of AliVTrack
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include <TRef.h>
#include <TBits.h>

#include "AliVTrack.h"
#include "AliAODVertex.h"
#include "AliAODRedCov.h"
#include "AliAODPid.h"

class AliAODTrack : public AliVTrack {

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
  AliAODTrack(Short_t id,
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
	      AODTrk_t ttype=kUndef,
	      UInt_t selectInfo=0,
	      Float_t chi2perNDF = -999.);

  AliAODTrack(Short_t id,
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
	      AODTrk_t ttype=kUndef,
	      UInt_t selectInfo=0,
	      Float_t chi2perNDF = -999.);

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
  virtual Bool_t   PxPyPz(Double_t p[3]) const { p[0] = Px(); p[1] = Py(); p[2] = Pz(); return kTRUE; }

  virtual Double_t Xv() const { return GetProdVertex() ? GetProdVertex()->GetX() : -999.; }
  virtual Double_t Yv() const { return GetProdVertex() ? GetProdVertex()->GetY() : -999.; }
  virtual Double_t Zv() const { return GetProdVertex() ? GetProdVertex()->GetZ() : -999.; }
  virtual Bool_t   XvYvZv(Double_t x[3]) const { x[0] = Xv(); x[1] = Yv(); x[2] = Zv(); return kTRUE; }

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
  void SetDetPID(AliAODPid *aodpid) {fDetPid = aodpid;}

  template <class T> void GetPID(T *pid) const {
    for(Int_t i=0; i<10; ++i) pid[i]=fPID[i];}
 
  template <class T> void SetPID(const T *pid) {
    if(pid) for(Int_t i=0; i<10; ++i) fPID[i]=pid[i];
    else {for(Int_t i=0; i<10; fPID[i++]=0.) ; fPID[AliAODTrack::kUnknown]=1.;}}

  Bool_t IsOn(Int_t mask) const {return (fFlags&mask)>0;}
  ULong_t GetStatus() const { return GetFlags(); }
  ULong_t GetFlags() const { return fFlags; }

  Int_t   GetID() const { return (Int_t)fID; }
  Int_t   GetLabel() const { return fLabel; } 
  Char_t  GetType() const { return fType;}
  Bool_t  IsPrimaryCandidate() const;
  Bool_t  GetUsedForVtxFit() const { return TestBit(kUsedForVtxFit); }
  Bool_t  GetUsedForPrimVtxFit() const { return TestBit(kUsedForPrimVtxFit); }

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

  Bool_t GetXYZ(Double_t *p) const {
    return GetPosition(p); }

  Bool_t GetCovarianceXYZPxPyPz(Double_t cv[21]) const {
    return GetCovMatrix(cv);}

  void RemoveCovMatrix() {delete fCovMatrix; fCovMatrix=NULL;}

  Double_t XAtDCA() const { return fPositionAtDCA[0]; }
  Double_t YAtDCA() const { return fPositionAtDCA[1]; }
  Double_t ZAtDCA() const {
    if (IsMuonTrack()) return fPosition[2];
    else if (TestBit(kIsDCA)) return fPosition[1];
    else return -999.; }
  Bool_t   XYZAtDCA(Double_t x[3]) const { x[0] = XAtDCA(); x[1] = YAtDCA(); x[2] = ZAtDCA(); return kTRUE; }
  
  Double_t DCA() const {
    if (IsMuonTrack()) return TMath::Sqrt(XAtDCA()*XAtDCA() + YAtDCA()*YAtDCA());
    else if (TestBit(kIsDCA)) return fPosition[0];
    else return -999.; }
  
  Double_t PxAtDCA() const { return fMomentumAtDCA[0]; }
  Double_t PyAtDCA() const { return fMomentumAtDCA[1]; }
  Double_t PzAtDCA() const { return fMomentumAtDCA[2]; }
  Double_t PAtDCA() const { return TMath::Sqrt(PxAtDCA()*PxAtDCA() + PyAtDCA()*PyAtDCA() + PzAtDCA()*PzAtDCA()); }
  Bool_t   PxPyPzAtDCA(Double_t p[3]) const { p[0] = PxAtDCA(); p[1] = PyAtDCA(); p[2] = PzAtDCA(); return kTRUE; }
  
  Double_t GetRAtAbsorberEnd() const { return fRAtAbsorberEnd; }
  
  UChar_t  GetITSClusterMap() const       { return (UChar_t)(fITSMuonClusterMap&0xff); }
  UShort_t GetHitsPatternInTrigCh() const { return (UShort_t)((fITSMuonClusterMap&0xff00)>>8); }
  UInt_t   GetMUONClusterMap() const      { return (fITSMuonClusterMap&0x3ff0000)>>16; }
  UInt_t   GetITSMUONClusterMap() const   { return fITSMuonClusterMap; }
  
  Bool_t  TestFilterBit(UInt_t filterBit) const {return (Bool_t) ((filterBit & fFilterMap) != 0);}
  Bool_t  TestFilterMask(UInt_t filterMask) const {return (Bool_t) ((filterMask & fFilterMap) == filterMask);}

  const TBits& GetTPCClusterMap() const {return fTPCClusterMap;}
  const TBits& GetTPCSharedMap() const {return fTPCSharedMap;}
  void    SetTPCClusterMap(const TBits amap) {fTPCClusterMap = amap;}
  void    SetTPCSharedMap(const TBits amap) {fTPCSharedMap = amap;}
  
  AliAODPid    *GetDetPid() const { return fDetPid; }
  AliAODVertex *GetProdVertex() const { return (AliAODVertex*)fProdVertex.GetObject(); }
  
  // print
  void  Print(const Option_t *opt = "") const;

  // setters
  void SetFlags(ULong_t flags) { fFlags = flags; }
  void SetStatus(ULong_t flags) { fFlags|=flags; }
  void ResetStatus(ULong_t flags) { fFlags&=~flags; }

  void SetID(Short_t id) { fID = id; }
  void SetLabel(Int_t label) { fLabel = label; }

  template <class T> void SetPosition(const T *x, Bool_t isDCA = kFALSE);
  void SetDCA(Double_t d, Double_t z);
  void SetUsedForVtxFit(Bool_t used = kTRUE) { used ? SetBit(kUsedForVtxFit) : ResetBit(kUsedForVtxFit); }
  void SetUsedForPrimVtxFit(Bool_t used = kTRUE) { used ? SetBit(kUsedForPrimVtxFit) : ResetBit(kUsedForPrimVtxFit); }

  void SetOneOverPt(Double_t oneOverPt) { fMomentum[0] = 1. / oneOverPt; }
  void SetPt(Double_t pt) { fMomentum[0] = pt; };
  void SetPhi(Double_t phi) { fMomentum[1] = phi; }
  void SetTheta(Double_t theta) { fMomentum[2] = theta; }
  template <class T> void SetP(const T *p, Bool_t cartesian = kTRUE);
  void SetP() {fMomentum[0]=fMomentum[1]=fMomentum[2]=-999.;}

  void SetXYAtDCA(Double_t x, Double_t y) {fPositionAtDCA[0] = x; fPositionAtDCA[1] = y;}
  void SetPxPyPzAtDCA(Double_t pX, Double_t pY, Double_t pZ) {fMomentumAtDCA[0] = pX; fMomentumAtDCA[1] = pY; fMomentumAtDCA[2] = pZ;}
  
  void SetRAtAbsorberEnd(Double_t r) { fRAtAbsorberEnd = r; }
  
  void SetCharge(Short_t q) { fCharge = q; }
  void SetChi2perNDF(Double_t chi2perNDF) { fChi2perNDF = chi2perNDF; }

  void SetITSClusterMap(UChar_t itsClusMap)                 { fITSMuonClusterMap = (fITSMuonClusterMap&0xffffff00)|(((UInt_t)itsClusMap)&0xff); }
  void SetHitsPatternInTrigCh(UShort_t hitsPatternInTrigCh) { fITSMuonClusterMap = (fITSMuonClusterMap&0xffff00ff)|((((UInt_t)hitsPatternInTrigCh)&0xff)<<8); }
  void SetMuonClusterMap(UInt_t muonClusMap)                { fITSMuonClusterMap = (fITSMuonClusterMap&0xfc00ffff)|((muonClusMap&0x3ff)<<16); }
  void SetITSMuonClusterMap(UInt_t itsMuonClusMap)          { fITSMuonClusterMap = itsMuonClusMap; }

  Int_t GetMatchTrigger() const {return fITSMuonClusterMap>>30;}
					//  0 Muon track does not match trigger
					//  1 Muon track match but does not pass pt cut
					//  2 Muon track match Low pt cut
					//  3 Muon track match High pt cut
  void     SetMatchTrigger(Int_t MatchTrigger);
  Bool_t   MatchTrigger() const { return (GetMatchTrigger()>0); }	  //  Muon track matches trigger track
  Bool_t   MatchTriggerLowPt()   const  { return (GetMatchTrigger()>1); } //  Muon track matches trigger track and passes Low pt cut
  Bool_t   MatchTriggerHighPt()  const  { return (GetMatchTrigger()>2); } //  Muon track matches trigger track and passes High pt cut
  Bool_t   MatchTriggerDigits()  const;                                   //  Muon track matches trigger digits
  Double_t GetChi2MatchTrigger() const  { return fChi2MatchTrigger;}
  void     SetChi2MatchTrigger(Double_t Chi2MatchTrigger) {fChi2MatchTrigger = Chi2MatchTrigger; }
  Bool_t   HitsMuonChamber(Int_t MuonChamber, Int_t cathode = -1) const;  // Check if track hits Muon chambers
  Bool_t   IsMuonTrack() const { return (GetMUONClusterMap()>0) ? kTRUE : kFALSE; }
  
  void     Connected(Bool_t flag) {flag ? SETBIT(fITSMuonClusterMap,26) : CLRBIT(fITSMuonClusterMap,26);}
  Bool_t   IsConnected() const {return TESTBIT(fITSMuonClusterMap,26);}

  void     SetProdVertex(TObject *vertex) { fProdVertex = vertex; }
  void     SetType(AODTrk_t ttype) { fType=ttype; }


 private :

  // Momentum & position
  Double32_t    fMomentum[3];       // momemtum stored in pt, phi, theta
  Double32_t    fPosition[3];       // position of first point on track or dca
  
  Double32_t    fMomentumAtDCA[3];  // momentum (px,py,pz) at DCA
  Double32_t    fPositionAtDCA[2];  // trasverse position (x,y) at DCA
  
  Double32_t    fRAtAbsorberEnd;    // transverse position r at the end of the muon absorber
  
  Double32_t    fChi2perNDF;        // chi2/NDF of momentum fit
  Double32_t    fChi2MatchTrigger;  // chi2 of trigger/track matching
  Double32_t    fPID[10];           // [0.,1.,8] pointer to PID object

  ULong_t       fFlags;             // reconstruction status flags 
  Int_t         fLabel;             // track label, points back to MC track
  
  UInt_t        fITSMuonClusterMap; // map of ITS and muon clusters, one bit per layer
                                    // (ITS: bit 1-8, muon trigger: bit 9-16, muon tracker: bit 17-26, muon match trigger: bit 31-32) 
  UInt_t        fFilterMap;         // filter information, one bit per set of cuts

  TBits         fTPCClusterMap;     // Map of clusters, one bit per padrow; 1 if has a cluster on given padrow
  TBits         fTPCSharedMap;      // Map of clusters, one bit per padrow; 1 if has a shared cluster on given padrow

  Short_t       fID;                // unique track ID, points back to the ESD track

  Char_t        fCharge;            // particle charge
  Char_t        fType;              // Track Type
  
  AliAODRedCov<6> *fCovMatrix;      // covariance matrix (x, y, z, px, py, pz)
  AliAODPid    *fDetPid;            // more detailed or detector specific pid information
  TRef          fProdVertex;        // vertex of origin

  ClassDef(AliAODTrack, 10);
};

inline Bool_t  AliAODTrack::IsPrimaryCandidate() const
{
    // True of track passes primary particle selection (independent of type) 
    // 
    if (fFilterMap) {
	return kTRUE;
    } else {
	return kFALSE;
    }
}

#endif

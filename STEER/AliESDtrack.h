
#ifndef ALIESDTRACK_H
#define ALIESDTRACK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliESDtrack
//   This is the class to deal with during the physics analysis of data
//      
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------
/*****************************************************************************
 *  Use GetExternalParameters() and GetExternalCovariance() to access the    *
 *      track information regardless of its internal representation.         *
 * This formation is now fixed in the following way:                         *
 *      external param0:   local Y-coordinate of a track (cm)                *
 *      external param1:   local Z-coordinate of a track (cm)                *
 *      external param2:   local sine of the track momentum azimuthal angle  *
 *      external param3:   tangent of the track momentum dip angle           *
 *      external param4:   1/pt (1/(GeV/c))                                  *
 *****************************************************************************/

#include <TBits.h>
#include "AliExternalTrackParam.h"
#include "AliPID.h"
#include "AliESDfriendTrack.h"

class TParticle;
class AliESDVertex;
class AliKalmanTrack;
class AliTrackPointArray;

class AliESDtrack : public AliExternalTrackParam {
public:
  AliESDtrack();
  AliESDtrack(const AliESDtrack& track);
  AliESDtrack(TParticle * part);
  virtual ~AliESDtrack();
  const AliESDfriendTrack *GetFriendTrack() const {return fFriendTrack;}
  void SetFriendTrack(const AliESDfriendTrack *t) {
    delete fFriendTrack; fFriendTrack=new AliESDfriendTrack(*t);
    // CKB
  }
  void ReleaseESDfriendTrack() { delete fFriendTrack;  fFriendTrack=0; }
  void AddCalibObject(TObject * object);     // add calib object to the list
  TObject *  GetCalibObject(Int_t index);    // return calib objct at given position
  void MakeMiniESDtrack();
  void SetID(Int_t id) { fID =id;}
  Int_t GetID() const { return fID;}
  void SetStatus(ULong_t flags) {fFlags|=flags;}
  void ResetStatus(ULong_t flags) {fFlags&=~flags;}
  Bool_t UpdateTrackParams(const AliKalmanTrack *t, ULong_t flags);
  void SetIntegratedLength(Double_t l) {fTrackLength=l;}
  void SetIntegratedTimes(const Double_t *times);
  void SetESDpid(const Double_t *p);
  void GetESDpid(Double_t *p) const;
  
  Bool_t IsOn(Int_t mask) const {return (fFlags&mask)>0;}
  ULong_t GetStatus() const {return fFlags;}
  Int_t GetLabel() const {return fLabel;}
  void SetLabel(Int_t label) {fLabel = label;}

  void GetExternalParameters(Double_t &x, Double_t p[5]) const;
  void GetExternalCovariance(Double_t cov[15]) const;

  Double_t GetIntegratedLength() const {return fTrackLength;}
  void GetIntegratedTimes(Double_t *times) const;
  Double_t GetMass() const;
  Double_t M() const { return GetMass(); }
  Double_t E() const;
  Double_t Y() const;

  Bool_t GetConstrainedPxPyPz(Double_t *p) const {
    if (!fCp) return kFALSE;
    return fCp->GetPxPyPz(p);
  }
  Bool_t GetConstrainedXYZ(Double_t *r) const {
    if (!fCp) return kFALSE;
    return fCp->GetXYZ(r);
  }
  const AliExternalTrackParam *GetConstrainedParam() const {return fCp;}
  Bool_t GetConstrainedExternalParameters
              (Double_t &alpha, Double_t &x, Double_t p[5]) const;
  Bool_t GetConstrainedExternalCovariance(Double_t cov[15]) const;
  Double_t GetConstrainedChi2() const {return fCchi2;}
  //
    


  Bool_t GetInnerPxPyPz(Double_t *p) const {
    if (!fIp) return kFALSE;
    return fIp->GetPxPyPz(p);
  }
  const AliExternalTrackParam * GetInnerParam() const { return fIp;}
  const AliExternalTrackParam * GetTPCInnerParam() const {return fTPCInner;}
  Bool_t GetInnerXYZ(Double_t *r) const {
    if (!fIp) return kFALSE;
    return fIp->GetXYZ(r);
  }
  Bool_t GetInnerExternalParameters
        (Double_t &alpha, Double_t &x, Double_t p[5]) const;
  Bool_t GetInnerExternalCovariance(Double_t cov[15]) const;
 
  const AliExternalTrackParam * GetOuterParam() const { return fOp;}
  Bool_t GetOuterPxPyPz(Double_t *p) const {
    if (!fOp) return kFALSE;
    return fOp->GetPxPyPz(p);
  }
  Bool_t GetOuterXYZ(Double_t *r) const {
    if (!fOp) return kFALSE;
    return fOp->GetXYZ(r);
  }
  Bool_t GetOuterExternalParameters
        (Double_t &alpha, Double_t &x, Double_t p[5]) const;
  Bool_t GetOuterExternalCovariance(Double_t cov[15]) const;


  Int_t GetNcls(Int_t idet) const;
  Int_t GetClusters(Int_t idet, Int_t *idx) const;
 
  void    SetITSpid(const Double_t *p);
  void    GetITSpid(Double_t *p) const;
  Float_t GetITSsignal() const {return fITSsignal;}
  Float_t GetITSchi2() const {return fITSchi2;}
  Int_t   GetITSclusters(Int_t *idx) const;
  UChar_t GetITSClusterMap() const {return fITSClusterMap;}
  Int_t   GetITSLabel() const {return fITSLabel;}
  void    SetITStrack(AliKalmanTrack * track){
     fFriendTrack->SetITStrack(track);
  }
  AliKalmanTrack *GetITStrack(){
     return fFriendTrack->GetITStrack();
  }

  void    SetTPCpid(const Double_t *p);
  void    GetTPCpid(Double_t *p) const;
  void    SetTPCPoints(Float_t points[4]){
     for (Int_t i=0;i<4;i++) fTPCPoints[i]=points[i];
  }
  void    SetTPCPointsF(UChar_t  findable){fTPCnclsF = findable;}
  Int_t   GetTPCNcls() const { return fTPCncls;}
  Int_t   GetTPCNclsF() const { return fTPCnclsF;}
  Float_t GetTPCPoints(Int_t i) const {return fTPCPoints[i];}
  void    SetKinkIndexes(Int_t points[3]) {
     for (Int_t i=0;i<3;i++) fKinkIndexes[i] = points[i];
  }
  void    SetV0Indexes(Int_t points[3]) {
     for (Int_t i=0;i<3;i++) fV0Indexes[i] = points[i];
  }
  void    SetTPCsignal(Float_t signal, Float_t sigma, UChar_t npoints){ 
     fTPCsignal = signal; fTPCsignalS = sigma; fTPCsignalN = npoints;
  }
  Float_t GetTPCsignal() const {return fTPCsignal;}
  Float_t GetTPCsignalSigma() const {return fTPCsignalS;}
  UShort_t GetTPCsignalN() const {return fTPCsignalN;}
  Float_t GetTPCchi2() const {return fTPCchi2;}
  Int_t   GetTPCclusters(Int_t *idx) const;
  Float_t GetTPCdensity(Int_t row0, Int_t row1) const;
  Int_t   GetTPCLabel() const {return fTPCLabel;}
  Int_t   GetKinkIndex(Int_t i) const { return fKinkIndexes[i];}
  Int_t   GetV0Index(Int_t i) const { return fV0Indexes[i];}
  const TBits& GetTPCClusterMap() const {return fTPCClusterMap;}
  const TBits& GetTPCSharedMap() const {return fTPCSharedMap;}
  void    SetTPCClusterMap(const TBits amap) {fTPCClusterMap = amap;}
  void    SetTPCSharedMap(const TBits amap) {fTPCSharedMap = amap;}
  void    SetTRDpid(const Double_t *p);
  
// A.Bercuci
  void    SetTRDpidQuality(UChar_t q){fTRDpidQuality = q;}
  UChar_t GetTRDpidQuality() const {return fTRDpidQuality;}
// end A.Bercuci
	
	void    SetTRDQuality(Float_t quality){fTRDQuality=quality;}
  Float_t GetTRDQuality()const {return fTRDQuality;}
  void    SetTRDBudget(Float_t budget){fTRDBudget=budget;}
  Float_t GetTRDBudget()const {return fTRDBudget;}
  void    SetTRDsignals(Float_t dedx, Int_t i, Int_t j) {fTRDsignals[i][j]=dedx;}
  void    SetTRDTimBin(Int_t timbin, Int_t i) {fTRDTimBin[i]=timbin;}
  void    GetTRDpid(Double_t *p) const;
  Float_t GetTRDsignal() const {return fTRDsignal;}
  Float_t GetTRDsignals(Int_t iPlane, Int_t iSlice=-1) const { if (iSlice == -1) 
    return (fTRDsignals[iPlane][0] + fTRDsignals[iPlane][1] + fTRDsignals[iPlane][2])/3.0;
    return fTRDsignals[iPlane][iSlice];
  }
  Int_t   GetTRDTimBin(Int_t i) const {return fTRDTimBin[i];}
  Float_t GetTRDchi2() const {return fTRDchi2;}
  Int_t   GetTRDclusters(Int_t *idx) const;
  Int_t   GetTRDncls() const {return fTRDncls;}
  void    SetTRDpid(Int_t iSpecies, Float_t p);
  Float_t GetTRDpid(Int_t iSpecies) const;
  Int_t   GetTRDLabel() const {return fTRDLabel;}

  void    SetTRDtrack(AliKalmanTrack * track){
     fFriendTrack->SetTRDtrack(track);
  }
  AliKalmanTrack *GetTRDtrack(){
     return fFriendTrack->GetTRDtrack();
  }

  void    SetTOFsignal(Double_t tof) {fTOFsignal=tof;}
  Float_t GetTOFsignal() const {return fTOFsignal;}
  void    SetTOFsignalToT(Double_t ToT) {fTOFsignalToT=ToT;}
  Float_t GetTOFsignalToT() const {return fTOFsignalToT;}
  void    SetTOFsignalRaw(Double_t tof) {fTOFsignalRaw=tof;}
  Float_t GetTOFsignalRaw() const {return fTOFsignalRaw;}
  void    SetTOFsignalDz(Double_t dz) {fTOFsignalDz=dz;}
  Float_t GetTOFsignalDz() const {return fTOFsignalDz;}
  Float_t GetTOFchi2() const {return fTOFchi2;}
  void    SetTOFpid(const Double_t *p);
  void    SetTOFLabel(const Int_t *p);
  void    GetTOFpid(Double_t *p) const;
  void    GetTOFLabel(Int_t *p) const;
  void    GetTOFInfo(Float_t *info) const;
  void    SetTOFInfo(Float_t *info);
  Int_t   GetTOFCalChannel() const {return fTOFCalChannel;}
  Int_t   GetTOFcluster() const {return fTOFindex;}
  void    SetTOFcluster(Int_t index) {fTOFindex=index;}
  void    SetTOFCalChannel(Int_t index) {fTOFCalChannel=index;}

// HMPID methodes +++++++++++++++++++++++++++++++++ (kir)
  void    SetHMPIDsignal(Double_t theta) {fHMPIDsignal=theta;}
  Float_t GetHMPIDsignal() const {return fHMPIDsignal;}
  void    SetHMPIDpid(const Double_t *p);
  void    GetHMPIDpid(Double_t *p) const;  
  void    SetHMPIDchi2(Double_t chi2) {fHMPIDchi2=chi2;}
  Float_t GetHMPIDchi2() const {return fHMPIDchi2;}
  void    SetHMPIDcluster(Int_t index) {fHMPIDcluIdx=index;}
  Int_t   GetHMPIDcluster() const {return fHMPIDcluIdx;}
  void    SetHMPIDcluIdx(Int_t ch,Int_t idx) {fHMPIDcluIdx=ch*1000000+idx;}
  Int_t   GetHMPIDcluIdx() const {return fHMPIDcluIdx;}
  void    SetHMPIDtrk(Float_t  x, Float_t  y, Float_t  th, Float_t  ph) {
     fHMPIDtrkX=x; fHMPIDtrkY=y; fHMPIDtrkTheta=th; fHMPIDtrkPhi=ph;
  }
  void    GetHMPIDtrk(Float_t &x, Float_t &y, Float_t &th, Float_t &ph) const {
     x=fHMPIDtrkX; y=fHMPIDtrkY; th=fHMPIDtrkTheta; ph=fHMPIDtrkPhi;
  }
  void    SetHMPIDmip(Float_t  x, Float_t  y, Int_t q, Int_t nph=0) {
     fHMPIDmipX=x; fHMPIDmipY=y; fHMPIDqn=1000000*q+nph;
  }
  void    GetHMPIDmip(Float_t &x,Float_t &y,Int_t &q,Int_t &nph) const {
     x=fHMPIDmipX; y=fHMPIDmipY; q=fHMPIDqn/1000000; nph=fHMPIDqn%1000000;
  }
  Bool_t  IsHMPID() const {return fFlags&kHMPIDpid;}


  Int_t GetEMCALcluster() {return fEMCALindex;}
  void SetEMCALcluster(Int_t index) {fEMCALindex=index;}
  Bool_t IsEMCAL() const {return fFlags&kEMCALmatch;}

  void SetTrackPointArray(AliTrackPointArray *points) {
    fFriendTrack->SetTrackPointArray(points);
  }
  const AliTrackPointArray *GetTrackPointArray() const {
    return fFriendTrack->GetTrackPointArray(); 
  }
  Bool_t RelateToVertex(const AliESDVertex *vtx, Double_t b, Double_t maxd);
  void GetImpactParameters(Float_t &xy,Float_t &z) const {xy=fD; z=fZ;}
  void GetImpactParameters(Float_t p[2], Float_t cov[3]) const {
    p[0]=fD; p[1]=fZ; cov[0]=fCdd; cov[1]=fCdz; cov[2]=fCzz;
  }
  virtual void Print(Option_t * opt) const ; 

  enum {
    kITSin=0x0001,kITSout=0x0002,kITSrefit=0x0004,kITSpid=0x0008,
    kTPCin=0x0010,kTPCout=0x0020,kTPCrefit=0x0040,kTPCpid=0x0080,
    kTRDin=0x0100,kTRDout=0x0200,kTRDrefit=0x0400,kTRDpid=0x0800,
    kTOFin=0x1000,kTOFout=0x2000,kTOFrefit=0x4000,kTOFpid=0x8000,
    kHMPIDpid=0x20000,
    kEMCALmatch=0x40000,
    kTRDbackup=0x80000,
    kTRDStop=0x20000000,
    kESDpid=0x40000000,
    kTIME=0x80000000
  }; 
  enum {
    kNPlane = 6,
    kNSlice = 3,
    kEMCALNoMatch = -999999999
  };
protected:
  

  ULong_t   fFlags;         // Reconstruction status flags 
  Int_t     fLabel;         // Track label
  Int_t     fID;            // Unique ID of the track
  Float_t   fTrackLength;   // Track length
  Float_t   fD;             // Impact parameter in XY plane
  Float_t   fZ;             // Impact parameter in Z
  Float_t   fCdd,fCdz,fCzz; // Covariance matrix of the impact parameters 
  Float_t   fTrackTime[AliPID::kSPECIES]; // TOFs estimated by the tracking
  Float_t   fR[AliPID::kSPECIES]; // combined "detector response probability"

  Int_t   fStopVertex;  // Index of the stop vertex

  AliExternalTrackParam *fCp; // Track parameters constrained to the primary vertex
  Double32_t fCchi2; // chi2 at the primary vertex


  AliExternalTrackParam *fIp; // Track parameters at the first measured point (TPC)
  AliExternalTrackParam *fTPCInner; // Track parameters at the first measured point (TPC) - first itteration


  AliExternalTrackParam *fOp; // Track parameters at the last measured point (TPC or TRD) 

  // ITS related track information
  Float_t fITSchi2;        // chi2 in the ITS
  Int_t   fITSncls;        // number of clusters assigned in the ITS
  UChar_t fITSClusterMap;  // map of clusters, one bit per a layer 
  Float_t fITSsignal;      // detector's PID signal
  Float_t fITSr[AliPID::kSPECIES]; // "detector response probabilities" (for the PID)
  Int_t   fITSLabel;       // label according TPC

  // TPC related track information
  Float_t  fTPCchi2;       // chi2 in the TPC
  Int_t    fTPCncls;       // number of clusters assigned in the TPC
  UShort_t fTPCnclsF;      // number of findable clusters in the TPC
  TBits    fTPCClusterMap; // Map of clusters, one bit per padrow; 1 if has a cluster on given padrow
  TBits    fTPCSharedMap;  // Map of clusters, one bit per padrow; 1 if has a shared cluster on given padrow
  Float_t  fTPCsignal;     // detector's PID signal
  UShort_t fTPCsignalN;    // number of points used for dEdx
  Float_t  fTPCsignalS;    // RMS of dEdx measurement
  Float_t  fTPCr[AliPID::kSPECIES]; // "detector response probabilities" (for the PID)
  Int_t    fTPCLabel;      // label according TPC
  Float_t  fTPCPoints[4];  // TPC points -first, max. dens, last and max density
  Int_t    fKinkIndexes[3];// array of indexes of posible kink candidates 
  Int_t    fV0Indexes[3];  // array of indexes of posible kink candidates 

  // TRD related track information
  Float_t fTRDchi2;        // chi2 in the TRD
  UChar_t fTRDncls;        // number of clusters assigned in the TRD
  UChar_t fTRDncls0;       // number of clusters assigned in the TRD before first material cross
  Float_t fTRDsignal;      // detector's PID signal
  Float_t fTRDsignals[kNPlane][kNSlice];  // TRD signals from all six planes in 3 slices each
  Int_t   fTRDTimBin[kNPlane];   // Time bin of Max cluster from all six planes
  Float_t fTRDr[AliPID::kSPECIES]; // "detector response probabilities" (for the PID)
	// A.Bercuci
	UChar_t fTRDpidQuality;   // TRD PID quality according to number of planes. 6 is the best
  Int_t   fTRDLabel;       // label according TRD
  Float_t fTRDQuality;     // trd quality factor for TOF
  Float_t fTRDBudget;      // trd material budget


  // TOF related track information
  Float_t fTOFchi2;        // chi2 in the TOF
  Int_t   fTOFindex;       // index of the assigned TOF cluster
  Int_t   fTOFCalChannel;  // Channel Index of the TOF Signal 
  Float_t fTOFsignal;      // detector's PID signal
  Float_t fTOFsignalToT;   // detector's ToT signal
  Float_t fTOFsignalRaw;   // detector's uncorrected time signal
  Float_t fTOFsignalDz;    // local z  of track's impact on the TOF pad 
  Float_t fTOFr[AliPID::kSPECIES]; // "detector response probabilities" (for the PID)
  Int_t   fTOFLabel[3];    // TOF label 
  Float_t fTOFInfo[10];    //! TOF informations

  // HMPID related track information                 (kir)
  Float_t fHMPIDchi2;       // chi2 in the HMPID
  Int_t   fHMPIDqn;         // 1000000*QDC + number of photon clusters
  Int_t   fHMPIDcluIdx;     // 1000000*chamber id + cluster idx of the assigned MIP cluster
  Float_t fHMPIDsignal;     // HMPID PID signal (Theta ckov, rad)
  Float_t fHMPIDr[AliPID::kSPECIES];// "detector response probabilities" (for the PID)
  Float_t fHMPIDtrkTheta;   // theta of the track extrapolated to the HMPID, LORS
  Float_t fHMPIDtrkPhi;     // phi of the track extrapolated to the HMPID, LORS
  Float_t fHMPIDtrkX;       // x of the track impact, LORS 
  Float_t fHMPIDtrkY;       // y of the track impact, LORS 
  Float_t fHMPIDmipX;       // x of the MIP in LORS
  Float_t fHMPIDmipY;       // y of the MIP in LORS
  
  // EMCAL related track information
  Int_t fEMCALindex;   // index of associated EMCAL cluster (AliESDCaloCluster)

  AliESDfriendTrack *fFriendTrack; //! All the complementary information

 private:

  AliESDtrack & operator=(const AliESDtrack & ) {return *this;}

  ClassDef(AliESDtrack,39)  //ESDtrack 
};

#endif 


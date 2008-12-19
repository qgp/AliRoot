#ifndef ALIITSTRACKV2_H
#define ALIITSTRACKV2_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                       ITS Track Class
//
//        Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//     dEdx analysis by: Boris Batyunya, JINR, Boris.Batiounia@cern.ch
//-------------------------------------------------------------------------

#include <AliKalmanTrack.h>
#include "AliITSRecoParam.h"
#include "AliITSgeomTGeo.h"

/* $Id$ */

class AliESDtrack;
class AliESDVertex;

//_____________________________________________________________________________
class AliITStrackV2 : public AliKalmanTrack {
public:
  AliITStrackV2();
  AliITStrackV2(AliESDtrack& t,Bool_t c=kFALSE) throw (const Char_t *);
  AliITStrackV2(const AliITStrackV2& t);
  ~AliITStrackV2(){fESDtrack=0;}

  Bool_t CorrectForMeanMaterial(Double_t xOverX0, Double_t xTimesRho,
				Bool_t anglecorr=kFALSE) {
    return AliExternalTrackParam::CorrectForMeanMaterial(xOverX0,xTimesRho,GetMass(),anglecorr);
  }
  Bool_t CorrectForMaterial(Double_t d, Double_t x0=AliITSRecoParam::GetX0Air()) {
    // deprecated: use CorrectForMeanMaterial instead
    return AliExternalTrackParam::CorrectForMaterial(d,x0,GetMass());
  }
  Bool_t PropagateTo(Double_t xr, Double_t d, Double_t x0=AliITSRecoParam::GetX0Air());
  Bool_t PropagateToTGeo(Double_t xToGo, Int_t nstep, Double_t &xOverX0, Double_t &xTimesRho);
  Bool_t PropagateToTGeo(Double_t xToGo, Int_t nstep=1) {
    Double_t dummy1,dummy2; return PropagateToTGeo(xToGo,nstep,dummy1,dummy2);
  }
  Double_t GetPredictedChi2(const AliCluster *cluster) const;
  Bool_t Update(const AliCluster *cl, Double_t chi2, Int_t i);

  Bool_t PropagateToVertex(const AliESDVertex *v,Double_t d=0.,Double_t x0=0.);
  Bool_t Propagate(Double_t alpha, Double_t xr);
  Bool_t Propagate(Double_t xr) { return Propagate(GetAlpha(),xr); }
  Bool_t MeanBudgetToPrimVertex(Double_t xyz[3], Double_t step, Double_t &d) const;
  Bool_t Improve(Double_t x0,Double_t xyz[3],Double_t ers[3]);

  void SetdEdx(Double_t dedx) {fdEdx=dedx;}
  void SetSampledEdx(Float_t q, Int_t i);
  Float_t GetSampledEdx(Int_t i) const {return fdEdxSample[i];}
  void CookdEdx(Double_t low=0., Double_t up=0.51);
  void SetDetectorIndex(Int_t i) {SetLabel(i);}
  void ResetClusters();
  void UpdateESDtrack(ULong_t flags) const;
  
  AliESDtrack *GetESDtrack() const {return fESDtrack;}

  Int_t GetDetectorIndex() const {return GetLabel();}
  Double_t GetdEdx() const {return fdEdx;}
  Double_t GetPIDsignal() const {return GetdEdx();}
  Double_t GetC() const {return AliExternalTrackParam::GetC(GetBz());}
  Double_t GetD(Double_t x, Double_t y) const {
    return AliExternalTrackParam::GetD(x,y,GetBz());
  }
  void GetDZ(Double_t xv, Double_t yv, Double_t zv, Float_t dz[2]) const {
    return AliExternalTrackParam::GetDZ(xv,yv,zv,GetBz(),dz);
  }

  Bool_t GetGlobalXYZat(Double_t xloc,Double_t &x,Double_t &y,Double_t &z) const;
  Bool_t GetPhiZat(Double_t r,Double_t &phi,Double_t &z) const;
  Bool_t GetLocalXat(Double_t r,Double_t &xloc) const;

  Int_t Compare(const TObject *o) const;
  Int_t GetClusterIndex(Int_t i) const {return fIndex[i];}
  void  SetModuleIndex(Int_t ilayer,Int_t idx) {fModule[ilayer]=idx;}
  Int_t GetModuleIndex(Int_t ilayer) const {return fModule[ilayer];}
  void  SetModuleIndexInfo(Int_t ilayer,Int_t idet,Int_t status=1,Float_t xloc=0,Float_t zloc=0);
  Bool_t GetModuleIndexInfo(Int_t ilayer,Int_t &idet,Int_t &status,Float_t &xloc,Float_t &zloc) const;
  Bool_t Invariant() const;

  void  SetExtraCluster(Int_t ilayer, Int_t idx) {fIndex[AliITSgeomTGeo::kNLayers+ilayer]=idx;}
  Int_t GetExtraCluster(Int_t ilayer) const {return fIndex[AliITSgeomTGeo::kNLayers+ilayer];}

  void  SetExtraModule(Int_t ilayer, Int_t idx) {fModule[AliITSgeomTGeo::kNLayers+ilayer]=idx;}
  Int_t GetExtraModule(Int_t ilayer) const {return fModule[AliITSgeomTGeo::kNLayers+ilayer];}

protected:
  Double_t GetBz() const ;
  Double_t fdEdx;            // dE/dx

  static const Int_t fgkWARN; //! used for debugging purposes
  Float_t fdEdxSample[4];    // array of dE/dx samples b.b.

  Int_t fIndex[2*AliITSgeomTGeo::kNLayers]; // indices of associated clusters 

  Int_t fModule[2*AliITSgeomTGeo::kNLayers]; // indices of crossed modules: 
                                             // see SetModuleIndexInfo()
  AliESDtrack *fESDtrack;    //! pointer to the connected ESD track

private:
  AliITStrackV2 &operator=(const AliITStrackV2 &tr);
  ClassDef(AliITStrackV2,7)  //ITS reconstructed track
};

inline void AliITStrackV2::SetSampledEdx(Float_t q, Int_t i) {
  //----------------------------------------------------------------------
  // This function stores dEdx sample corrected for the track segment length 
  // Origin: Boris Batyunya, JINR, Boris.Batiounia@cern.ch
  //----------------------------------------------------------------------
  if (i<0) return;
  if (i>3) return;
  Double_t s=GetSnp(), t=GetTgl();
  q *= TMath::Sqrt((1-s*s)/(1+t*t));
  fdEdxSample[i]=q;
  return;
}

inline void  AliITStrackV2::SetModuleIndexInfo(Int_t ilayer,Int_t idet,Int_t status,
					       Float_t xloc,Float_t zloc) {
  //----------------------------------------------------------------------
  // This function encodes in the module number also the status of cluster association
  // "status" can have the following values: 
  // 1 "found" (cluster is associated), 
  // 2 "dead" (module is dead from OCDB), 
  // 3 "skipped" (module or layer forced to be skipped),
  // 4 "outinz" (track out of z acceptance), 
  // 5 "nocls" (no clusters in the road), 
  // 6 "norefit" (cluster rejected during refit) 
  // 7 "deadzspd" (holes in z in SPD)
  // WARNING: THIS METHOD HAS TO BE SYNCHRONIZED WITH AliESDtrack::GetITSModuleIndexInfo()!
  //----------------------------------------------------------------------

  if(idet<0) {
    idet=0;
  } else {
    // same detector numbering as in AliITSCalib classes
    if(ilayer==1) idet+=AliITSgeomTGeo::GetNLadders(1)*AliITSgeomTGeo::GetNDetectors(1);
    if(ilayer==3) idet+=AliITSgeomTGeo::GetNLadders(3)*AliITSgeomTGeo::GetNDetectors(3);
    if(ilayer==5) idet+=AliITSgeomTGeo::GetNLadders(5)*AliITSgeomTGeo::GetNDetectors(5);
  }

  Int_t xInt = Int_t(xloc*10.);
  Int_t zInt = Int_t(zloc*10.);

  if(TMath::Abs(xloc*10.-(Float_t)xInt)>0.5)
    if(zloc>0) { xInt++; } else { xInt--; }
  if(TMath::Abs(zloc*10.-(Float_t)zInt)>0.5)
    if(zloc>0) { zInt++; } else { zInt--; }

  Int_t signs=0;
  if(xInt>=0 && zInt>=0) signs=10000;
  if(xInt>=0 && zInt<0)  signs=20000;
  if(xInt<0 && zInt>=0)  signs=30000;
  if(xInt<0 && zInt<0)   signs=40000;

  Int_t modindex = signs;
  
  modindex += TMath::Abs(zInt);
  modindex += TMath::Abs(xInt)*100;

  modindex += status*100000;

  modindex += idet*1000000;

  SetModuleIndex(ilayer,modindex);
  return;
}

inline Bool_t AliITStrackV2::GetModuleIndexInfo(Int_t ilayer,Int_t &idet,Int_t &status,
					       Float_t &xloc,Float_t &zloc) const {
  //----------------------------------------------------------------------
  // This function encodes in the module number also the status of cluster association
  // "status" can have the following values: 
  // 1 "found" (cluster is associated), 
  // 2 "dead" (module is dead from OCDB), 
  // 3 "skipped" (module or layer forced to be skipped),
  // 4 "outinz" (track out of z acceptance), 
  // 5 "nocls" (no clusters in the road), 
  // 6 "norefit" (cluster rejected during refit), 
  // 7 "deadzspd" (holes in z in SPD)
  // Also given are the coordinates of the crossing point of track and module
  // (in the local module ref. system)
  // WARNING: THIS METHOD HAS TO BE SYNCHRONIZED WITH AliESDtrack::GetITSModuleIndexInfo()!
  //----------------------------------------------------------------------


  if(fModule[ilayer]==-1) {
    AliError("fModule was not set !");
    idet = -1;
    status=0;
    xloc=-99.; zloc=-99.;
    return kFALSE;
  }

  Int_t module = fModule[ilayer];

  idet = Int_t(module/1000000);

  module -= idet*1000000;

  status = Int_t(module/100000);

  module -= status*100000;

  Int_t signs = Int_t(module/10000);

  module-=signs*10000;

  Int_t xInt = Int_t(module/100);
  module -= xInt*100;

  Int_t zInt = module;

  if(signs==1) { xInt*=1; zInt*=1; }
  if(signs==2) { xInt*=1; zInt*=-1; }
  if(signs==3) { xInt*=-1; zInt*=1; }
  if(signs==4) { xInt*=-1; zInt*=-1; }

  xloc = 0.1*(Float_t)xInt;
  zloc = 0.1*(Float_t)zInt;

  if(status==4) idet = -1;

  return kTRUE;
}


#endif



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

/// \class AliTPCTransform
/// \brief Implementation of the TPC transformation class
///
/// Class for tranformation of the coordinate frame
/// Transformation
/// local coordinate frame (sector, padrow, pad, timebine) ==>
/// rotated global (tracking) cooridnate frame (sector, lx,ly,lz)
///
/// Unisochronity  - (substract time0 - pad by pad)
/// Drift velocity - Currently common drift velocity - functionality of AliTPCParam
/// ExB effect     -
///
/// Time of flight correction -
/// - Depends on the vertex position
/// - by default
///
/// Usage:
///
/// ~~~{.cxx}
/// AliTPCclusterer::AddCluster
/// AliTPCtracker::Transform
/// ~~~
///
/// To test it:
///
/// ~~~{.cxx}
/// cdb=AliCDBManager::Instance()
/// cdb->SetDefaultStorage("local:///u/mmager/mycalib1")
/// c=AliTPCcalibDB::Instance()
/// c->SetRun(0)
/// Double_t x[]={1.0,2.0,3.0}
/// Int_t i[]={4}
/// AliTPCTransform trafo
/// trafo.Transform(x,i,0,1)
/// ~~~
///
/// \author Magnus Mager, Marian Ivanov

#include "AliTPCROC.h"
#include "AliTPCCalPad.h"
#include "AliTPCCalROC.h"
#include "AliTPCcalibDB.h"
#include "AliTPCParam.h"
#include "TMath.h"
#include "AliLog.h"
#include "AliTPCExB.h"
#include "AliTPCCorrection.h"
#include "TGeoMatrix.h"
#include "AliTPCRecoParam.h"
#include "AliTPCCalibVdrift.h"
#include "AliTPCTransform.h"
#include "AliMagF.h"
#include "TGeoGlobalMagField.h"
#include "AliTracker.h"
#include <AliCTPTimeParams.h>
#include "AliTPCChebCorr.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"

/// \cond CLASSIMP
ClassImp(AliTPCTransform)
/// \endcond


AliTPCTransform::AliTPCTransform():
  AliTransform(),
  fCurrentRecoParam(0),       //! current reconstruction parameters
  fCorrMapCache0(0),
  fCorrMapCache1(0),
  fCurrentRun(0),             //! current run
  fCurrentTimeStamp(0)        //! current time stamp
{
  //
  // Speed it up a bit!
  //
  for (Int_t i=0;i<18;++i) {
    Double_t alpha=TMath::DegToRad()*(10.+20.*(i%18));
    fSins[i]=TMath::Sin(alpha);
    fCoss[i]=TMath::Cos(alpha);
  }
  fPrimVtx[0]=0;
  fPrimVtx[1]=0;
  fPrimVtx[2]=0;
}
AliTPCTransform::AliTPCTransform(const AliTPCTransform& transform):
  AliTransform(transform),
  fCurrentRecoParam(transform.fCurrentRecoParam),       //! current reconstruction parameters
  fCorrMapCache0(transform.fCorrMapCache0),
  fCorrMapCache1(transform.fCorrMapCache1),
  fCurrentRun(transform.fCurrentRun),             //! current run
  fCurrentTimeStamp(transform.fCurrentTimeStamp)        //! current time stamp
{
  /// Speed it up a bit!

  for (Int_t i=0;i<18;++i) {
    Double_t alpha=TMath::DegToRad()*(10.+20.*(i%18));
    fSins[i]=TMath::Sin(alpha);
    fCoss[i]=TMath::Cos(alpha);
  }
  fPrimVtx[0]=0;
  fPrimVtx[1]=0;
  fPrimVtx[2]=0;
}

AliTPCTransform::~AliTPCTransform() {
  /// Destructor

}

void AliTPCTransform::SetPrimVertex(Double_t *vtx){
  ///

  fPrimVtx[0]=vtx[0];
  fPrimVtx[1]=vtx[1];
  fPrimVtx[2]=vtx[2];
}


void AliTPCTransform::Transform(Double_t *x,Int_t *i,UInt_t /*time*/,
				Int_t /*coordinateType*/) {
  /// input: x[0] - pad row
  ///        x[1] - pad
  ///        x[2] - time in us
  ///        i[0] - sector
  /// output: x[0] - x (all in the rotated global coordinate frame)
  /// x[1] - y
  /// x[2] - z
  ///
  ///  primvtx     - position of the primary vertex
  /// used for the TOF correction
  /// TOF of particle calculated assuming the speed-of-light and
  /// line approximation

  if (!fCurrentRecoParam) return;
  Int_t row=TMath::Nint(x[0]);
  Int_t pad=TMath::Nint(x[1]);
  Int_t sector=i[0];
  AliTPCcalibDB*  calib=AliTPCcalibDB::Instance();
  //
  AliMagF* magF= (AliMagF*)TGeoGlobalMagField::Instance()->GetField();
  Double_t bzField = magF->SolenoidField(); //field in kGaus

  AliTPCCalPad * time0TPC = calib->GetPadTime0();
  AliTPCParam  * param    = calib->GetParameters();

  if (!param){
    AliFatal("Parameters missing");
    return; // make coverity happy
  }
  if (!time0TPC){
    AliFatal("Time unisochronity missing");
    return ; // make coverity happy
  }
  
  // Apply Time0 correction - Pad by pad fluctuation
  if (!calib->HasAlignmentOCDB()) x[2]-=time0TPC->GetCalROC(sector)->GetValue(row,pad);
  //
  // Tranform from pad - time coordinate system to the rotated global (tracking) system
  Local2RotatedGlobal(sector,x);
  Bool_t isInRotated = kTRUE;
  //
  //
  if (fCurrentRecoParam->GetUseCorrectionMap()) {
    const int kNInnerSectors=36, kInnerNRow=63;
    float delta0[3], y2x=x[1]/x[0], z2x = fCorrMapCache0->GetUseZ2R() ? x[2]/x[0] : x[2];
    int rowTot = sector<kNInnerSectors ? row : row+kInnerNRow;
    int sectTot = sector%18;
    fCorrMapCache0->Eval(rowTot,sectTot,y2x,z2x,delta0);
    // 
    // for time dependent correction need to evaluate 2 maps, assuming linear dependence
    if (fCorrMapCache1) {
      float delta1[3];
      fCorrMapCache1->Eval(rowTot,sectTot,y2x,z2x,delta1);   
      UInt_t t0 = fCorrMapCache0->GetTimeStampCenter();
      UInt_t t1 = fCorrMapCache1->GetTimeStampCenter();
      // possible division by 0 is checked at upload of maps
      double dtScale = (t1-fCurrentTimeStamp)/double(t1-t0);
      for (int i=3;i--;) delta0[i] += (delta1[i]-delta0[i])*dtScale;
    }
    for (int i=3;i--;) x[i] += delta0[i];
  }
  // Alignment
  //TODO:  calib->GetParameters()->GetClusterMatrix(sector)->LocalToMaster(x,xx);
  //
  if(fCurrentRecoParam->GetUseExBCorrection()) {
    RotatedGlobal2Global(sector,x);
    isInRotated = kFALSE;    
    Double_t xx[3];
    calib->GetExB()->Correct(x,xx);   // old ExB correction
    for (int i=3;i--;) x[i] = xx[i];
  }

  //
  // new composed  correction  - will replace soon ExB correction
  //
  if(fCurrentRecoParam->GetUseComposedCorrection()) {
    //
    if (isInRotated) {
      RotatedGlobal2Global(sector,x);
      isInRotated = kFALSE;          
    }
    AliTPCCorrection * correction = calib->GetTPCComposedCorrection();   // first user defined correction  // if does not exist  try to get it from calibDB array
    if (!correction) correction = calib->GetTPCComposedCorrection(bzField);
    AliTPCCorrection * correctionDelta = calib->GetTPCComposedCorrectionDelta();
    if (correction) {
      Float_t distPoint[3]={static_cast<Float_t>(x[0]),static_cast<Float_t>(x[1]),static_cast<Float_t>(x[2])};
      correction->CorrectPoint(distPoint, sector);
      for (int i=3;i--;) x[i]=distPoint[i];
      if (correctionDelta&&fCurrentRecoParam->GetUseAlignmentTime()){  // appply time dependent correction if available and enabled
	Float_t distPointDelta[3]={static_cast<Float_t>(x[0]),static_cast<Float_t>(x[1]),static_cast<Float_t>(x[2])};
	correctionDelta->CorrectPoint(distPointDelta, sector);
	for (int i=3;i--;) x[i]=distPointDelta[i];
      }
    }
  }
  
  //
  // Time of flight correction
  //
  if (fCurrentRecoParam->GetUseTOFCorrection()){
    const Int_t kNIS=param->GetNInnerSector(), kNOS=param->GetNOuterSector();
    Float_t sign=1;
    if (sector < kNIS) {
      sign = (sector < kNIS/2) ? 1 : -1;
    } else {
      sign = ((sector-kNIS) < kNOS/2) ? 1 : -1;
    }
    Float_t deltaDr =0;
    Float_t dist=0;
    dist+=x[0]*x[0];  // (fPrimVtx[0]-x[0])*(fPrimVtx[0]-x[0]); // RS the vertex is anyway close to 0
    dist+=x[1]*x[1];  // (fPrimVtx[1]-x[1])*(fPrimVtx[1]-x[1]);
    dist+=(fPrimVtx[2]-x[2])*(fPrimVtx[2]-x[2]);
    dist = TMath::Sqrt(dist);
    // drift length correction because of TOF
    // the drift velocity is in cm/s therefore multiplication by 0.01
    deltaDr = (dist*(0.01*param->GetDriftV()))/TMath::C();
    x[2]+=sign*deltaDr;
  }
  //
  //
  if (!isInRotated) {
    Global2RotatedGlobal(sector,x);
    isInRotated = kTRUE;
  }
  //
  // Apply non linear distortion correction
  //
  int useFieldCorr = fCurrentRecoParam->GetUseFieldCorrection();
  if (useFieldCorr&(0x2|0x4|0x8)) {
    AliTPCCalPad * distortionMapY=0,*distortionMapZ=0,*distortionMapR=0;
    //
    // wt - to get it form the OCDB
    // ignore T1 and T2
    Double_t vdrift = param->GetDriftV()/1000000.; // [cm/us]   // From dataBase: to be updated: per second (ideally)
    Double_t ezField = 400; // [V/cm]   // to be updated: never (hopefully)
    if (sector%36<18) ezField*=-1;
    //    Double_t wt = -10.0 * (bzField*10) * vdrift / ezField ;
    Double_t wt = -10.0 * (bzField) * vdrift / ezField ; // RS: field is already in kGauss
    Double_t c0=1./(1.+wt*wt);
    Double_t c1=wt/c0;
    
    //can be switch on for each dimension separatelly
    if (useFieldCorr&0x2 && (distortionMapY=calib->GetDistortionMap(0))) {
      x[1]-= c0*distortionMapY->GetCalROC(sector)->GetValue(row,pad);
      x[0]-= c1*distortionMapY->GetCalROC(sector)->GetValue(row,pad);
    }
    if (useFieldCorr&0x4 && (distortionMapZ=calib->GetDistortionMap(1))) {
      x[2]-=distortionMapZ->GetCalROC(sector)->GetValue(row,pad);
    }
    if (useFieldCorr&0x8 && (distortionMapR=calib->GetDistortionMap(2))) {
      x[0]-= c0*distortionMapR->GetCalROC(sector)->GetValue(row,pad);
      x[1]-=-c1*distortionMapR->GetCalROC(sector)->GetValue(row,pad)*wt;
    }
    //
  }
  //
}

void AliTPCTransform::Local2RotatedGlobal(Int_t sector, Double_t *x) const {
  /// Tranform coordinate from
  /// row, pad, time to x,y,z
  ///
  /// Drift Velocity
  /// Current implementation - common drift velocity - for full chamber
  /// TODO: use a map or parametrisation!

  if (!fCurrentRecoParam) return;
  const  Int_t kMax =60;  // cache for 60 seconds
  static Int_t lastStamp=-1;  //cached values
  static Double_t lastCorr = 1;
  //
  AliTPCcalibDB*  calib=AliTPCcalibDB::Instance();
  AliTPCParam  * param    = calib->GetParameters();
  AliTPCCalibVdrift *driftCalib = AliTPCcalibDB::Instance()->GetVdrift(fCurrentRun);
  Double_t driftCorr = 1.;
  if (driftCalib){
    //
    // caching drift correction - temp. fix
    // Extremally slow procedure
    if ( TMath::Abs((lastStamp)-Int_t(fCurrentTimeStamp))<kMax){
      driftCorr = lastCorr;
    }else{
      driftCorr = 1.+(driftCalib->GetPTRelative(fCurrentTimeStamp,0)+ driftCalib->GetPTRelative(fCurrentTimeStamp,1))*0.5;
      lastCorr=driftCorr;
      lastStamp=fCurrentTimeStamp;

    }
  }
  //
  // simple caching non thread save
  static Double_t vdcorrectionTime=1;
  static Double_t vdcorrectionTimeGY=0;
  static Double_t time0corrTime=0;
  static Double_t deltaZcorrTime=0;
  static Int_t    lastStampT=-1;
  //
  if (lastStampT!=(Int_t)fCurrentTimeStamp){
    lastStampT=fCurrentTimeStamp;
    if(fCurrentRecoParam->GetUseDriftCorrectionTime()>0) {
      vdcorrectionTime = (1+AliTPCcalibDB::Instance()->
			  GetVDriftCorrectionTime(fCurrentTimeStamp,
						  fCurrentRun,
						  sector%36>=18,
						  fCurrentRecoParam->GetUseDriftCorrectionTime()));
      time0corrTime= AliTPCcalibDB::Instance()->
	GetTime0CorrectionTime(fCurrentTimeStamp,
			       fCurrentRun,
			       sector%36>=18,
			       fCurrentRecoParam->GetUseDriftCorrectionTime());
      //
      deltaZcorrTime= AliTPCcalibDB::Instance()->
	GetVDriftCorrectionDeltaZ(fCurrentTimeStamp,
			       fCurrentRun,
			       sector%36>=18,
			       0);

    }
    //
    if(fCurrentRecoParam->GetUseDriftCorrectionGY()>0) {

      Double_t corrGy= AliTPCcalibDB::Instance()->
			GetVDriftCorrectionGy(fCurrentTimeStamp,
					      AliTPCcalibDB::Instance()->GetRun(),
					      sector%36>=18,
					      fCurrentRecoParam->GetUseDriftCorrectionGY());
      vdcorrectionTimeGY = corrGy;
    }
  }


  if (!param){
    AliFatal("Parameters missing");
    return; // make coverity happy
  }
  Int_t row=TMath::Nint(x[0]);
  //  Int_t pad=TMath::Nint(x[1]);
  //
  const Int_t kNIS=param->GetNInnerSector(), kNOS=param->GetNOuterSector();
  Double_t sign = 1.;
  Double_t zwidth    = param->GetZWidth()*driftCorr;
  Float_t xyzPad[3];
  AliTPCROC::Instance()->GetPositionGlobal(sector, TMath::Nint(x[0]) ,TMath::Nint(x[1]), xyzPad);
  if (AliTPCRecoParam:: GetUseTimeCalibration()) zwidth*=vdcorrectionTime*(1+xyzPad[1]*vdcorrectionTimeGY);
  Double_t padWidth  = 0;
  Double_t padLength = 0;
  Double_t    maxPad    = 0;
  //
  if (sector < kNIS) {
    maxPad = param->GetNPadsLow(row);
    sign = (sector < kNIS/2) ? 1 : -1;
    padLength = param->GetPadPitchLength(sector,row);
    padWidth = param->GetPadPitchWidth(sector);
  } else {
    maxPad = param->GetNPadsUp(row);
    sign = ((sector-kNIS) < kNOS/2) ? 1 : -1;
    padLength = param->GetPadPitchLength(sector,row);
    padWidth  = param->GetPadPitchWidth(sector);
  }
  //
  // X coordinate
  x[0] = param->GetPadRowRadii(sector,row);  // padrow X position - ideal
  //
  // Y coordinate
  //
  x[1]=(x[1]-0.5*maxPad)*padWidth;
  // pads are mirrorred on C-side
  if (sector%36>17){
    x[1]*=-1;
  }

  //

  //
  // Z coordinate
  //
  if (AliTPCcalibDB::Instance()->IsTrgL0()){
    // by defualt we assume L1 trigger is used - make a correction in case of  L0
    AliCTPTimeParams* ctp = AliTPCcalibDB::Instance()->GetCTPTimeParams();
    if (ctp){
      //for TPC standalone runs no ctp info
      Double_t delay = ctp->GetDelayL1L0()*0.000000025;
      x[2]-=delay/param->GetTSample();
    }
  }
  x[2]-= param->GetNTBinsL1();
  x[2]*= zwidth;  // tranform time bin to the distance to the ROC
  x[2]-= 3.*param->GetZSigma() + time0corrTime;
  // subtract the time offsets
  x[2] = sign*( param->GetZLength(sector) - x[2]);
  x[2]-=deltaZcorrTime;   // subtrack time dependent z shift (calibrated together with the drift velocity and T0)
}

void AliTPCTransform::RotatedGlobal2Global(Int_t sector,Double_t *x) const {
  /// transform possition rotated global to the global

  Double_t cos,sin;
  GetCosAndSin(sector,cos,sin);
  Double_t tmp=x[0];
  x[0]= cos*tmp-sin*x[1];
  x[1]=+sin*tmp+cos*x[1];
}

void AliTPCTransform::Global2RotatedGlobal(Int_t sector,Double_t *x) const {
  /// tranform possition Global2RotatedGlobal

  Double_t cos,sin;
  GetCosAndSin(sector,cos,sin);
  Double_t tmp=x[0];
  x[0]= cos*tmp+sin*x[1];
  x[1]= -sin*tmp+cos*x[1];
}

void AliTPCTransform::GetCosAndSin(Int_t sector,Double_t &cos,
					  Double_t &sin) const {
  cos=fCoss[sector%18];
  sin=fSins[sector%18];
}


void AliTPCTransform::ApplyTransformations(Double_t */*xyz*/, Int_t /*volID*/){
  /// Modify global position
  /// xyz    - global xyz position
  /// volID  - volID of detector (sector number)

}

void AliTPCTransform::SetCurrentTimeStamp(Int_t timeStamp) 
{
  // set event time stamp and if needed, upload caches
  fCurrentTimeStamp = timeStamp;
  UpdateTimeDependentCache();
}

void AliTPCTransform::UpdateTimeDependentCache()
{
  // update cache for time-dependent parameters
  //
  // correction maps, potentially time dependent
  TObjArray* mapsArr = 0;
  while (fCurrentRecoParam->GetUseCorrectionMap()) {
    //
    if (fCorrMapCache0) {
      // 1: easiest case: map is already cached, it is either time-static or there is 
      // no other map to follow
      if (!fCorrMapCache0->GetTimeDependent() || !fCorrMapCache1) break;
      //
      // 2: still easy: time dependent but we have in memory all what we need
      if (fCorrMapCache1 && fCurrentTimeStamp<=fCorrMapCache1->GetTimeStampCenter()
	  && fCurrentTimeStamp>=fCorrMapCache0->GetTimeStampStart()) break;
    }
    else  { // no map yet: 1st query
      mapsArr = LoadCorrectionMaps();
      fCorrMapCache0 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(0);
      //
      // 3: easy case: time-static map, fCorrMapCache1 is not neaded
      if (!fCorrMapCache0->GetTimeDependent()) {
	if (mapsArr->GetEntriesFast()>2) { 
	  // time independent maps are field dependent!
	  AliMagF* magF= (AliMagF*)TGeoGlobalMagField::Instance()->GetField();
	  Double_t bz = magF->SolenoidField(); //field in kGaus
	  if (bz>0.1) fCorrMapCache0 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(1);
	  else if (bz<-0.1) fCorrMapCache0 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(2);  
	}
	else AliWarning("Time-independent corrections array must provide 1 map per field polarity");
      } 
      break; // done for static map, if needed, look for other stuff 
    }
    // need to scan whole array to find matching maps
    if (!mapsArr) mapsArr = LoadCorrectionMaps();
    int nmaps = mapsArr->GetEntriesFast();
    const AliTPCChebCorr* prv=0,*nxt=0;
    for (int i=0;i<nmaps;i++) { // maps are ordered in time
      fCorrMapCache0 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(i);
      if (fCorrMapCache0->GetTimeStampEnd()<=fCurrentTimeStamp) {
	prv = fCorrMapCache0;
	continue;
      }
      // we found a chunk which contain our time stamp
      if (fCurrentTimeStamp <= fCorrMapCache0->GetTimeStampCenter()) { // we are in the 1st half
	if (prv) { // previous chunk will be used
	  fCorrMapCache1 = fCorrMapCache0;
	  fCorrMapCache0 = prv;
	  break;
	}
	// we are at the 1st chunk, read the next one
	if (i<nmaps-1) fCorrMapCache1 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(i+1);
	else fCorrMapCache1 = 0;
	break;
      }
      else { // we are in the 2nd half of cache0,  look for the next for cache1
	if (i<nmaps-1) fCorrMapCache1 = (const AliTPCChebCorr*)mapsArr->UncheckedAt(i+1);
	else { // no next one, do we have previous?
	  if (prv) {
	    fCorrMapCache1 = fCorrMapCache0;
	    fCorrMapCache0 = prv;
	  }
	  else fCorrMapCache1 = 0;
	}
      }
      break;
    }
    break;
  } // while
  // clean unneaded object to save the memory
  if (mapsArr) {
    mapsArr->Remove((TObject*)fCorrMapCache0);
    if (fCorrMapCache1) mapsArr->Remove((TObject*)fCorrMapCache1);
    mapsArr->SetOwner(kTRUE);
    delete mapsArr;
    if (!fCorrMapCache1) {
      AliInfoF("Loaded static correction map for time stamp %d",fCurrentTimeStamp);
      fCorrMapCache0->Print();
    }
    else {
      AliInfoF("Switched to pair of time-dependent correction maps at time stamp %d",fCurrentTimeStamp);
      fCorrMapCache0->Print();
      fCorrMapCache1->Print();
    }
  }
  // other time dependent stuff if needed
}

//______________________________________________________
TObjArray* AliTPCTransform::LoadCorrectionMaps() const
{
  // TPC fast Chebyshev correction map, loaded on demand, not handler by calibDB
  AliCDBManager* man = AliCDBManager::Instance();
  AliCDBEntry* entry = man->Get("TPC/Calib/CorrectionMaps");
  TObjArray* correctionMaps = (TObjArray*)entry->GetObject();
  entry->SetOwner(0);
  man->UnloadFromCache("TPC/Calib/CorrectionMaps");
  return correctionMaps;
  //
}

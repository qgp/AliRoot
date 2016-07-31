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
#include "AliTPCChebDist.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"
#include "TTreeStream.h"
#include "AliLumiTools.h"
#include "AliTPCclusterMI.h"
#include "../HLT/TPCLib/transform/AliHLTTPCReverseTransformInfoV1.h"
#include <TGraph.h>

/// \cond CLASSIMP
ClassImp(AliTPCTransform)
/// \endcond


const Double_t AliTPCTransform::fgkSin20 = TMath::Sin(TMath::Pi()/9);       // sin(20)
const Double_t AliTPCTransform::fgkCos20 = TMath::Cos(TMath::Pi()/9);       // cos(20)
const Double_t AliTPCTransform::fgkMaxY2X = TMath::Tan(TMath::Pi()/18);      // tg(10)



AliTPCTransform::AliTPCTransform():
  AliTransform(),
  fCurrentRecoParam(0),       //! current reconstruction parameters
  fCorrMapCacheRef(0),
  fCorrMapCache0(0),
  fCorrMapCache1(0),
  fCurrentMapScaling(1.0),
  fCorrMapLumiCOG(0.0),
  fLumiGraphRun(0),
  fLumiGraphMap(0),
  fCurrentRun(0),             //! current run
  fCurrentTimeStamp(0),       //! current time stamp
  fTimeDependentUpdated(kFALSE),
  fDebugStreamer(0),
  fTmpReverseTransformInfo(NULL)
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
  for (int i=0;i<4;i++) {fLastCorr[i] = fLastCorrRef[i] = 0.0f;}
}

AliTPCTransform::AliTPCTransform(const AliTPCTransform& transform):
  AliTransform(transform),
  fCurrentRecoParam(transform.fCurrentRecoParam),       //! current reconstruction parameters
  fCorrMapCacheRef(transform.fCorrMapCacheRef),
  fCorrMapCache0(transform.fCorrMapCache0),
  fCorrMapCache1(transform.fCorrMapCache1),
  fLumiGraphRun(transform.fLumiGraphRun ? new TGraph(*transform.fLumiGraphRun) : 0),
  fLumiGraphMap(transform.fLumiGraphMap ? new TGraph(*transform.fLumiGraphMap) : 0),
  fCurrentRun(transform.fCurrentRun),             //! current run
  fCurrentTimeStamp(transform.fCurrentTimeStamp),       //! current time stamp
  fTimeDependentUpdated(transform.fTimeDependentUpdated),
  fDebugStreamer(0),
  fTmpReverseTransformInfo(NULL)
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
  for (int i=0;i<4;i++) {fLastCorr[i] = fLastCorrRef[i] = 0.0f;}
}

AliTPCTransform::~AliTPCTransform() {
  /// Destructor
  delete fLumiGraphRun; // own copy should be attached
  delete fLumiGraphMap; // own copy should be attached
  delete fCorrMapCacheRef;
  delete fCorrMapCache0;
  delete fCorrMapCache1;
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
  if (fCurrentRecoParam->GetUseCorrectionMap()) ApplyCorrectionMap(sector, row, x);
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
  /// 
  /// If called with NULL x pointer, just reset the cache
  static time_t lastStamp=-1;  //cached values
  static Double_t lastCorr = 1;
  // simple caching non thread save
  static Double_t vdcorrectionTime=1;
  static Double_t vdcorrectionTimeGY=0;
  static Double_t time0corrTime=0;
  static Double_t deltaZcorrTime=0;
  static time_t    lastStampT=-1;
  static Int_t lastSide = -1;

  if (!x && fTmpReverseTransformInfo == NULL) {
    AliInfo("Reseting cache");
    lastStamp=-1;
    lastCorr = 1;
    vdcorrectionTime=1;
    vdcorrectionTimeGY=0;
    time0corrTime=0;
    deltaZcorrTime=0;
    lastStampT=-1;
    return;
  }

  if (!fCurrentRecoParam) return;
  const  Int_t kMax =60;  // cache for 60 seconds
  //
  AliTPCcalibDB*  calib=AliTPCcalibDB::Instance();
  AliTPCParam  * param    = calib->GetParameters();
  AliTPCCalibVdrift *driftCalib = AliTPCcalibDB::Instance()->GetVdrift(fCurrentRun);
  Double_t driftCorr = 1.;
  if (driftCalib){
    //
    // caching drift correction - temp. fix
    // Extremally slow procedure
    if ( TMath::Abs(Int_t(lastStamp)-Int_t(fCurrentTimeStamp))<kMax){
      driftCorr = lastCorr;
    }else{
      driftCorr = 1.+(driftCalib->GetPTRelative(fCurrentTimeStamp,0)+ driftCalib->GetPTRelative(fCurrentTimeStamp,1))*0.5;
      lastCorr=driftCorr;
      lastStamp=fCurrentTimeStamp;

    }
  }
  //
  //
  
  Int_t side = (Int_t) sector%36>=18;
  
  Bool_t isChange=(lastStampT!=(Int_t)fCurrentTimeStamp)||side!=lastSide||lastStamp<0;
  if (lastStampT!=(Int_t)fCurrentTimeStamp||side!=lastSide){
    lastStampT=fCurrentTimeStamp;
    lastSide=side;
    if(fCurrentRecoParam->GetUseDriftCorrectionTime()>0) {
      vdcorrectionTime = (1+AliTPCcalibDB::Instance()->
			  GetVDriftCorrectionTime(fCurrentTimeStamp,
						  fCurrentRun,
						  side,
						  fCurrentRecoParam->GetUseDriftCorrectionTime()));
      time0corrTime= AliTPCcalibDB::Instance()->
	GetTime0CorrectionTime(fCurrentTimeStamp,
			       fCurrentRun,
			       side,
			       fCurrentRecoParam->GetUseDriftCorrectionTime());
      //
      deltaZcorrTime= AliTPCcalibDB::Instance()->
	GetVDriftCorrectionDeltaZ(fCurrentTimeStamp,
			       fCurrentRun,
			       side,
			       0);

    }
    //
    if(fCurrentRecoParam->GetUseDriftCorrectionGY()>0) {

      Double_t corrGy= AliTPCcalibDB::Instance()->
			GetVDriftCorrectionGy(fCurrentTimeStamp,
					      AliTPCcalibDB::Instance()->GetRun(),
					      side,
					      fCurrentRecoParam->GetUseDriftCorrectionGY());
      vdcorrectionTimeGY = corrGy;
    }
  }
  
  if (!param){
    AliFatal("Parameters missing");
    return; // make coverity happy
  }

  if (fTmpReverseTransformInfo) {
    fTmpReverseTransformInfo->fNTBinsL1 = param->GetNTBinsL1();
    fTmpReverseTransformInfo->fZWidth = param->GetZWidth();
    fTmpReverseTransformInfo->fZSigma = param->GetZSigma();
    fTmpReverseTransformInfo->fDriftCorr = driftCorr;
    if (side == 0) {
      fTmpReverseTransformInfo->fTime0corrTimeA = time0corrTime;
      fTmpReverseTransformInfo->fDeltaZcorrTimeA = deltaZcorrTime;
      fTmpReverseTransformInfo->fVdcorrectionTimeA = vdcorrectionTime;
      fTmpReverseTransformInfo->fVdcorrectionTimeGYA = vdcorrectionTimeGY;
      fTmpReverseTransformInfo->fZLengthA = param->GetZLength(sector);
    } else {
      fTmpReverseTransformInfo->fTime0corrTimeC = time0corrTime;
      fTmpReverseTransformInfo->fDeltaZcorrTimeC = deltaZcorrTime;
      fTmpReverseTransformInfo->fVdcorrectionTimeC = vdcorrectionTime;
      fTmpReverseTransformInfo->fVdcorrectionTimeGYC = vdcorrectionTimeGY;
      fTmpReverseTransformInfo->fZLengthC = param->GetZLength(sector);
    }
    if (!AliTPCRecoParam::GetUseTimeCalibration()) {
      fTmpReverseTransformInfo->fVdcorrectionTimeA = fTmpReverseTransformInfo->fVdcorrectionTimeC = 1.f;
      fTmpReverseTransformInfo->fVdcorrectionTimeGYA = fTmpReverseTransformInfo->fVdcorrectionTimeGYC = 0.f;
    }
    return;
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
  Double_t delay=0;
  if (AliTPCcalibDB::Instance()->IsTrgL0()){
    // by defualt we assume L1 trigger is used - make a correction in case of  L0
    AliCTPTimeParams* ctp = AliTPCcalibDB::Instance()->GetCTPTimeParams();
    if (ctp){
      //for TPC standalone runs no ctp info
      delay = ctp->GetDelayL1L0()*0.000000025;
      delay/=param->GetTSample();
      x[2]-=delay;
    }
  }
  if (isChange && fDebugStreamer!=NULL){
    //
    //
    Double_t zwidth=param->GetZWidth();
    Double_t binsL1=param->GetNTBinsL1();
    Double_t zsigma=param->GetZSigma();
    (*fDebugStreamer)<<"transformDump"<<
      "fCurrentTimeStamp="<<lastStampT<<
      "zwidth="<<zwidth<<                                      // nominal z drift
      "delay="<<delay<<                                           // trigger delay
      "binsL1="<<binsL1<<                                       // L1 delay
      "zsigma="<<zsigma<<                                     // z sigma
      "driftCorr="<<driftCorr<<
      "vdcorrectionTime="<<vdcorrectionTime<<
      "time0corrTime="<<time0corrTime<<
      "deltaZcorrTime="<<deltaZcorrTime<<
      "vdcorrectionTimeGY="<<vdcorrectionTimeGY<<
      "\n";
      
      
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

void AliTPCTransform::SetCurrentTimeStamp(time_t timeStamp) 
{
  // set event time stamp and if needed, upload caches
  fCurrentTimeStamp = timeStamp;
  fTimeDependentUpdated = kFALSE;
  UpdateTimeDependentCache();
}

Bool_t AliTPCTransform::UpdateTimeDependentCache()
{
  // update cache for time-dependent parameters
  //
  static time_t lastTimeStamp = -1;
  fTimeDependentUpdated = kFALSE;
  //
  Bool_t timeChanged = lastTimeStamp!=fCurrentTimeStamp;
  if (!fCurrentRecoParam) {
    AliWarning("RecoParam is not set, reseting last timestamp");
    lastTimeStamp = -1;
    return fTimeDependentUpdated;
  }
  while (fCurrentRecoParam->GetUseCorrectionMap()) {
    if (!fCorrMapCacheRef) fCorrMapCacheRef = LoadFieldDependendStaticCorrectionMap(kTRUE); // need to load the reference correction map
    //
    int mapTimeDepMethod = fCurrentRecoParam->GetCorrMapTimeDepMethod();
    Bool_t needToLoad = timeChanged;
    if (fCorrMapCache0 && timeChanged) { // do we need to update already loaded maps?
      // 1: easiest case: map is already cached, it is either time-static or there is 
      // no other map to follow
      if (!fCorrMapCache0->GetTimeDependent()) needToLoad = kFALSE;   // maps are not time dependent, no update needed
      //
      // 2: still easy: time dependent in interpolation mode but we have in memory all what we need
      if (fCorrMapCache1 && // interpolation method already triggered loading of 2nd map
	  ( (fCurrentTimeStamp>=fCorrMapCache0->GetTimeStampCenter()&& // still covered by already loaded
	     fCurrentTimeStamp<=fCorrMapCache1->GetTimeStampCenter()) // interpolation region
	    ||
	    (fCurrentTimeStamp<fCorrMapCache0->GetTimeStampEnd()&&fCorrMapCache0->IsFirst()) // no earlier object
	    ||
	    (fCurrentTimeStamp>fCorrMapCache1->GetTimeStampStart()&&fCorrMapCache1->IsLast()) // no later object
	    ) ) {
	needToLoad = kFALSE; // no update needed
      }
      // 3: still easy: time depenedent but only 1 map needed (no interpolation)
      if ( (mapTimeDepMethod!=AliTPCRecoParam::kCorrMapInterpolation) 
	   &&
	   ((fCurrentTimeStamp<=fCorrMapCache0->GetTimeStampEnd() && 
	     (fCurrentTimeStamp>=fCorrMapCache0->GetTimeStampStart()||fCorrMapCache0->IsFirst()))
	    ||
	    (fCurrentTimeStamp>=fCorrMapCache0->GetTimeStampStart() && 
	     (fCurrentTimeStamp<=fCorrMapCache0->GetTimeStampEnd()||fCorrMapCache0->IsLast()))
	    )) {
	needToLoad = kFALSE; // still covered by existing map or no other map to load
      }
    }
    //
    if (needToLoad) { // need to upload correction maps, potentially time dependent
      TObjArray* mapsArr = LoadCorrectionMaps(kFALSE);
      // are these time-static maps?
      if (!((AliTPCChebCorr*)mapsArr->UncheckedAt(0))->GetTimeDependent()) {
	fCorrMapCache0 = LoadFieldDependendStaticCorrectionMap(kFALSE,mapsArr); // static maps are field-dependent
      }
      else {
	LoadCorrectionMapsForTimeBin(mapsArr); // load maps matching to time stamp
      }
      // clean unneeded object to save the memory
      mapsArr->Remove((TObject*)fCorrMapCache0);
      if (fCorrMapCache1) mapsArr->Remove((TObject*)fCorrMapCache1);
      mapsArr->SetOwner(kTRUE);
      delete mapsArr;
      //
      if (fCorrMapCache0 && !fCorrMapCache0->IsCorrection()) 
	AliFatalF("Uploaded map is not correction: %s",fCorrMapCache0->IsA()->GetName());
      if (fCorrMapCache1 && !fCorrMapCache1->IsCorrection()) 
	AliFatalF("Uploaded map is not correction: %s",fCorrMapCache1->IsA()->GetName());
      
      // check time stamps
      if (fCorrMapCache0 && fCorrMapCache0->GetTimeStampStart()>fCurrentTimeStamp) {
	AliWarningF("Event timestamp %ld < map0 beginning %ld",fCurrentTimeStamp,fCorrMapCache0->GetTimeStampStart());
      }
      if (fCorrMapCache1) {
	if (fCorrMapCache1->GetTimeStampEnd()<fCurrentTimeStamp) {
	  AliWarningF("Event timestamp %ld > map1 end %ld",fCurrentTimeStamp,fCorrMapCache1->GetTimeStampEnd());
	}
      } 
      else if (fCorrMapCache0->GetTimeStampEnd()<fCurrentTimeStamp) {
	AliWarningF("Event timestamp %ld > map0 end %ld",fCurrentTimeStamp,fCorrMapCache0->GetTimeStampEnd());
      }
    }
    // do we need to update luminosity scaling params?
    if (timeChanged) { 
      switch(mapTimeDepMethod) {
      case AliTPCRecoParam::kCorrMapInterpolation :
      case AliTPCRecoParam::kCorrMapNoScaling     : break;
      case AliTPCRecoParam::kCorrMapGlobalScalingLumi: 
	// load lumi graph used for map (run may be different from current one if the map is default one)
	if (!fLumiGraphMap) {
	  //if (!fLumiGraphMap || fLumiGraphMap->GetUniqueID()!=fCorrMapCache0->GetRun()) {
	  //if (fLumiGraphMap) delete fLumiGraphMap;
	  int runMap = fCorrMapCache0->GetRun();
	  if (runMap<0) AliErrorF("CorrectionMap Lumi scaling requested but run is not set, current %d will be used",fCurrentRun);
	  fLumiGraphMap = AliLumiTools::GetLumiGraph(fCurrentRecoParam->GetUseLumiType(),runMap);
	  // if (!fLumiGraphMap) AliErrorF("Failed to load map lumi graph of type %d, will use constant map",
	  //				fCurrentRecoParam->GetUseLumiType());
	}
	// load lumi graph for this run
	if (!fLumiGraphRun) {
	  fLumiGraphRun = AliLumiTools::GetLumiGraph(fCurrentRecoParam->GetUseLumiType(),fCurrentRun);
	  if (!fLumiGraphRun) AliErrorF("Failed to load run lumi graph of type %d, will use reference correction only",
					fCurrentRecoParam->GetUseLumiType());
	}
	if (needToLoad) {  // calculate average luminosity used for current corr. map
	  fCorrMapLumiCOG = fLumiGraphMap ? fCorrMapCache0->GetLuminosityCOG(fLumiGraphMap) : 0.0; // recalculate lumi for new map
	  if (!fCorrMapLumiCOG) AliError("Correction map rescaling with luminosity cannot be done, will use constant map");
	}
	//
	fCurrentMapScaling = fLumiGraphRun ? 1.0 : 0.0;
	if (fCorrMapLumiCOG>0) {
	  double lumi = fLumiGraphRun ? fLumiGraphRun->Eval(fCurrentTimeStamp) : 0.0;
	  fCurrentMapScaling = lumi/fCorrMapLumiCOG;
	  AliInfoF("Luminosity scaling factor %.3f will be used for time %ld (Lumi: current: %.2e COG:%.2e)",
		   fCurrentMapScaling,fCurrentTimeStamp,lumi,fCorrMapLumiCOG);
	}
	//
	break;
      default: AliFatalF("Unknown corrections time-evolution mode %d",mapTimeDepMethod);
      };
    }
    //
    break;
  } // loading of correction maps
  //
  // other time dependent stuff if needed
  //
  lastTimeStamp = fCurrentTimeStamp;
  fTimeDependentUpdated = kTRUE;
  return fTimeDependentUpdated;
}

//______________________________________________________
void AliTPCTransform::LoadCorrectionMapsForTimeBin(TObjArray* mapsArrProvided)
{
  // loads time-independent correction map for given time bin
  // 
  TObjArray* mapsArr = mapsArrProvided;
  if (!mapsArr) mapsArr = LoadCorrectionMaps(kFALSE);
  int entries = mapsArr->GetEntriesFast();
  delete fCorrMapCache0;
  delete fCorrMapCache1;
  fCorrMapCache0 = fCorrMapCache1 = 0;
  //1) choose map covering current time stamp.
  int selID=0;
  for (selID=0;selID<entries;selID++) { // maps are ordered in time
    fCorrMapCache0 = (AliTPCChebCorr*)mapsArr->UncheckedAt(selID);
    if (fCurrentTimeStamp > fCorrMapCache0->GetTimeStampEnd()) { 
      if (selID==entries-1) break; // in case the maps end limit < timestamp close to EOR
    }
    else if (fCurrentTimeStamp < fCorrMapCache0->GetTimeStampStart()) {
      if (!selID) break; // in case the maps start limit > timestamp close to SOR
    }
    else break;  // exact match
  }
  // this should not happen
  if (!fCorrMapCache0) AliFatalF("Did not find map matching to timestamp %ld",fCurrentTimeStamp);
  //
  if (!selID) fCorrMapCache0->SetFirst(); // flag if there are maps before/after
  if (selID==entries-1) fCorrMapCache0->SetLast();
  //
  // if we are in the interpolation mode, load 2nd closest map
  if (fCurrentRecoParam->GetCorrMapTimeDepMethod()==AliTPCRecoParam::kCorrMapInterpolation) {
    if (fCurrentTimeStamp<=fCorrMapCache0->GetTimeStampCenter()) { // load map before or closest, if any
      if (selID) fCorrMapCache1 = (AliTPCChebCorr*)mapsArr->UncheckedAt(--selID);
      else if (selID<entries-1) fCorrMapCache1 = (AliTPCChebCorr*)mapsArr->UncheckedAt(++selID);
    }
    else { // load map after or closest, if any
      if (selID<entries-1) fCorrMapCache1 = (AliTPCChebCorr*)mapsArr->UncheckedAt(++selID);
      else if (selID) fCorrMapCache1 = (AliTPCChebCorr*)mapsArr->UncheckedAt(--selID);
    }
    //
    if (fCorrMapCache1) {
      if (!selID) fCorrMapCache1->SetFirst(); // flag if there are maps before/after
      if (selID==entries-1) fCorrMapCache0->SetLast();
      if (fCorrMapCache1->GetTimeStampStart()<fCorrMapCache0->GetTimeStampStart()) {
	AliTPCChebCorr* tmp = fCorrMapCache1; // swap to order in time
	fCorrMapCache1 = fCorrMapCache0;
	fCorrMapCache0 = tmp;
      }
    }
    //
  }
  //
  AliInfoF("Loaded %d maps for time stamp %ld",fCorrMapCache1?2:1,fCurrentTimeStamp);
  fCorrMapCache0->Print();
  if (fCorrMapCache1) fCorrMapCache1->Print();
  //
  if (!mapsArrProvided) { // if loaded locally, clean unnecessary stuff
    mapsArr->Remove((TObject*)fCorrMapCache0);
    if (fCorrMapCache1) mapsArr->Remove((TObject*)fCorrMapCache1);
    mapsArr->SetOwner(kTRUE);
    delete mapsArr;
  }
}

//______________________________________________________
AliTPCChebCorr* AliTPCTransform::LoadFieldDependendStaticCorrectionMap(Bool_t ref, TObjArray* mapsArrProvided)
{
  // loads time-independent correction map for relevan field polarity. If ref is true, then the
  // reference map is loaded
  // 
  const float kZeroField = 0.1;
  TObjArray* mapsArr = mapsArrProvided;
  if (!mapsArr) mapsArr = LoadCorrectionMaps(ref);
  int entries = mapsArr->GetEntriesFast();
  AliMagF* magF= (AliMagF*)TGeoGlobalMagField::Instance()->GetField(); // think on extracting field once only
  Double_t bzField = magF->SolenoidField(); //field in kGaus
  Char_t expectType = AliTPCChebCorr::kFieldZero;
  if      (bzField<-kZeroField) expectType = AliTPCChebCorr::kFieldNeg;
  else if (bzField> kZeroField) expectType = AliTPCChebCorr::kFieldPos;
  //
  AliTPCChebCorr* cormap = 0;
  for (int i=0;i<entries;i++) {
    AliTPCChebCorr* map = (AliTPCChebCorr*)mapsArr->At(i); if (!map) continue;
    if (!map->IsCorrection()) continue;
    Char_t mtp = map->GetFieldType();
    if (mtp==expectType || mtp==AliTPCChebCorr::kFieldAny) cormap = map;
    if (mtp==expectType) break;
  }
  if (!cormap) AliFatalGeneralF("AliTPCTransform","Did not find %s correction map",ref ? "reference":"");

  AliInfoGeneralF("AliTPCTransform","Loaded  %s correction map",ref ? "reference":"");
  cormap->Print();
  if (cormap->GetFieldType() == AliTPCChebCorr::kFieldAny) {
    AliWarningGeneralF("AliTPCTransform","ATTENTION: no map for field %+.1f was found, placeholder map is used",bzField);
  }
  //
  cormap->SetFirst(); // flag absence of maps before
  cormap->SetLast();  // and after
  //
  if (!mapsArrProvided) { // if loaded locally, clean unnecessary stuff
    mapsArr->Remove((TObject*)cormap);
    mapsArr->SetOwner(kTRUE);
    delete mapsArr;
  }
  return cormap;
}

//______________________________________________________
TObjArray* AliTPCTransform::LoadCorrectionMaps(Bool_t refMap)
{
  // TPC fast Chebyshev correction map, loaded on demand, not handler by calibDB
  const char* kNameRef = "TPC/Calib/CorrectionMapsRef";
  const char* kNameRun = "TPC/Calib/CorrectionMaps";
  const char* mapTypeName = refMap ? kNameRef : kNameRun;
  //
  AliCDBManager* man = AliCDBManager::Instance();
  AliCDBEntry* entry = man->Get(mapTypeName);
  TObjArray* correctionMaps = (TObjArray*)entry->GetObject();
  for (int i=correctionMaps->GetEntriesFast();i--;) {
    AliTPCChebCorr* map = (AliTPCChebCorr*)correctionMaps->At(i);
    map->Init();
  }
  entry->SetOwner(0);
  entry->SetObject(0);
  man->UnloadFromCache(mapTypeName);
  return correctionMaps;
  //
}

//______________________________________________________
void AliTPCTransform::ApplyCorrectionMap(int roc, int row, double xyzSect[3])
{
  // apply correction from the map to a point at given ROC and row (IROC/OROC convention)
  const float kDistDispThresh = 300e-4; // assume fluctuation dispersion if D[3]>Dref[3]+threshold
  EvalCorrectionMap(roc, row, xyzSect, fLastCorrRef, kTRUE);
  EvalCorrectionMap(roc, row, xyzSect, fLastCorr, kFALSE);
  if (fLastCorr[3]<1e-6) { // run specific map had no parameterization for this region, override by default
    for (int i=3;i--;) fLastCorr[i] = fLastCorrRef[i];
    fLastCorr[3] = 0.f;
  }
  else {
    fLastCorr[3] = fLastCorr[3]>(fLastCorrRef[3]+kDistDispThresh) ? TMath::Sqrt(fLastCorr[3]*fLastCorr[3] - fLastCorrRef[3]*fLastCorrRef[3]) : 0;
    if (fCurrentMapScaling!=1.0f) {
      for (int i=3;i--;) fLastCorr[i] = (fLastCorr[i]-fLastCorrRef[i])*fCurrentMapScaling + fLastCorrRef[i];
      fLastCorr[3] *= fCurrentMapScaling;
    }
  }
  for (int i=3;i--;) xyzSect[i] += fLastCorr[i];
  //
}

//______________________________________________________
Float_t AliTPCTransform::GetCorrMapComponent(int roc, int row, const double xyz[3], int dimOut)
{
  // calculate final correction component from the map
  float corr = EvalCorrectionMap(roc, row, xyz, dimOut, kFALSE); // run specific correction
  // do we need to rescale?
  if (fCurrentMapScaling!=1.0f) {
    float corrRef =  EvalCorrectionMap(roc, row, xyz, dimOut, kTRUE); // ref correction
    if (dimOut<3) corr = (corr-corrRef)*fCurrentMapScaling + corrRef;    // rescale with lumi
  }
  return corr;
}

//______________________________________________________
void AliTPCTransform::EvalCorrectionMap(int roc, int row, const double xyz[3], float *res, Bool_t ref)
{
  // get correction from the map for a point at given ROC and row (IROC/OROC convention)
  if (!fTimeDependentUpdated && !UpdateTimeDependentCache()) AliFatal("Failed to update time-dependent cache");

  AliTPCChebCorr* map = ref ? fCorrMapCacheRef : fCorrMapCache0;
  float y2x=xyz[1]/xyz[0], z2x = map->GetUseZ2R() ? xyz[2]/xyz[0] : xyz[2];
  map->Eval(roc,row,y2x,z2x,res);

  if (ref) return; // this was time-independent ref.map query request
  // 
  // for time dependent correction need to evaluate 2 maps, assuming linear dependence
  if (fCorrMapCache1) {
    float delta1[4]={0};
    fCorrMapCache1->Eval(roc,row,y2x,z2x,delta1);   
    UInt_t t0 = fCorrMapCache0->GetTimeStampCenter();
    UInt_t t1 = fCorrMapCache1->GetTimeStampCenter();
      // possible division by 0 is checked at upload of maps
    double dtScale = (fCurrentTimeStamp-t0)/double(t1-t0);
    for (int i=4;i--;) res[i] += (delta1[i]-res[i])*dtScale;
  }
  //
}

//______________________________________________________
Float_t AliTPCTransform::EvalCorrectionMap(int roc, int row, const double xyz[3], int dimOut, Bool_t ref)
{
  // get correction for dimOut-th dimension from the map for a point at given ROC and row (IROC/OROC convention)
  if (!fTimeDependentUpdated && !UpdateTimeDependentCache()) AliFatal("Failed to update time-dependent cache");

  AliTPCChebCorr* map = ref ? fCorrMapCacheRef : fCorrMapCache0;
  float y2x=xyz[1]/xyz[0], z2x = map->GetUseZ2R() ? xyz[2]/xyz[0] : xyz[2];
  float corr = map->Eval(roc,row,y2x,z2x,dimOut);
  // 
  if (ref) return corr; // this was time-independent query request
  //
  // for time dependent correction need to evaluate 2 maps, assuming linear dependence
  if (fCorrMapCache1) {
    float corr1 = fCorrMapCache1->Eval(roc,row,y2x,z2x,dimOut);   
    UInt_t t0 = fCorrMapCache0->GetTimeStampCenter();
    UInt_t t1 = fCorrMapCache1->GetTimeStampCenter();
      // possible division by 0 is checked at upload of maps
    double dtScale = (t1-fCurrentTimeStamp)/double(t1-t0);
    corr += (corr1-corr)*dtScale;
  }
  //
  return corr;
}

//______________________________________________________
void AliTPCTransform::EvalDistortionMap(int roc, const double xyzSector[3], float *res)
{
  // get distortions from the map for a point at given ROC
  if (!fTimeDependentUpdated && !UpdateTimeDependentCache()) AliFatal("Failed to update time-dependent cache");
  if (!fCorrMapCache0->IsDistortion()) AliFatalF("Uploaded map is not distortion: %s",fCorrMapCache0->IsA()->GetName());
  float y2x=xyzSector[1]/xyzSector[0], z2x = fCorrMapCache0->GetUseZ2R() ? xyzSector[2]/xyzSector[0] : xyzSector[2];
  ((AliTPCChebDist*)fCorrMapCache0)->Eval(roc,xyzSector[0],y2x,z2x,res);
  // 
  // for time dependent correction need to evaluate 2 maps, assuming linear dependence
  if (fCorrMapCache1) {
    float delta1[4] = {0.0f};
    ((AliTPCChebDist*)fCorrMapCache1)->Eval(roc,xyzSector[0],y2x,z2x,res);
    UInt_t t0 = fCorrMapCache0->GetTimeStampCenter();
    UInt_t t1 = fCorrMapCache1->GetTimeStampCenter();
      // possible division by 0 is checked at upload of maps
    double dtScale = (t1-fCurrentTimeStamp)/double(t1-t0);
    for (int i=4;i--;) res[i] += (delta1[i]-res[i])*dtScale;
  }
  //
}

//______________________________________________________
void AliTPCTransform::ApplyDistortionMap(int roc, double xyzLab[3])
{
  // apply distortion from the map to a point provided in LAB coordinate 
  // at given ROC and row (IROC/OROC convention)
  double xyzSect[3];
  float  res[3];
  Global2RotatedGlobal(roc,xyzLab);
  EvalDistortionMap(roc, xyzLab, res); // now we are in sector coordinates
  for (int i=3;i--;) xyzLab[i] += res[i];
  RotatedGlobal2Global(roc,xyzLab);
  //
}

//_____________________________________________________________________________________
void AliTPCTransform::ErrY2Z2Syst(const AliTPCclusterMI * cl, const double tgPhi, const double tgLam,
				  double &serry2, double &serrz2)
{
  // static function to calculate systematic errorY^2 on the cluster with current reco param
  // Must be static to be used also by external routines
  // tgPhi is the ab.tg(inclination wrt padrow), tgLam is tg of dip angle 
  const float kEpsZBoundary = 1.e-6; // to disentangle A,C and A+C-common regions
  const float kMaxExpArg = 9.; // limit r-dumped error to this exp. argument
  const float kMaxExpArgZ = TMath::Sqrt(2.*kMaxExpArg);
  serry2 = serrz2 = 0;
  double sysErrY = 0, sysErrZ = 0;
  //
  // error on particular TPC subvolume (charging up)
  const Double_t *errInner = fCurrentRecoParam->GetSystematicErrorClusterInner();
  if (errInner[0]>0) {
    //
    double dr = TMath::Abs(cl->GetX()-85.);
    float argExp = dr/errInner[1];     // is the Z-range limited ?
    if (argExp<kMaxExpArg) {
      const TVectorF* zranges = fCurrentRecoParam->GetSystErrClInnerRegZ(); // is the Z-range of dumped error defined?
      int nz;
      if (zranges && (nz=zranges->GetNoElements())) {
	Bool_t sideC = (cl->GetDetector()/18)&0x1; // is this C-side cluster
	const TVectorF* zrangesSigI = fCurrentRecoParam->GetSystErrClInnerRegZSigInv();
	float zcl = cl->GetZ(); // chose the closest range within 3 sigma
	const float* zr = zranges->GetMatrixArray();
	const float* zsi= zrangesSigI->GetMatrixArray();
	float dzarg = 999.;
	for (int i=nz;i--;) { // find closest region
	  if      (zr[i]<-kEpsZBoundary && !sideC) continue; // don't apply to A-side clusters if the Z-boundary is for C-region
	  else if (zr[i]>kEpsZBoundary  &&  sideC) continue; // don't apply to C-side clusters if the Z-boundary is for A-region
	  float dz = TMath::Abs( (zr[i]-zcl)*zsi[i] );
	  if (dz<dzarg) dzarg = dz;
	}
	if (dzarg<kMaxExpArgZ) { // is it small enough to call exp?
	  argExp += 0.5*dzarg*dzarg;
	  if (argExp<kMaxExpArg) sysErrY = sysErrZ = errInner[0]*TMath::Exp(-argExp);
	}	
      }
      else sysErrY = sysErrZ = errInner[0]*TMath::Exp(-argExp); // no condition on Z
    }

  }
  serry2 += sysErrY*sysErrY;
  serrz2 += sysErrZ*sysErrZ;
  //
  sysErrY = fCurrentRecoParam->GetSystematicErrorClusterCustom() ?  
    fCurrentRecoParam->GetSystematicErrorClusterCustom()[0] : fCurrentRecoParam->GetSystematicErrorCluster()[0];
  sysErrZ = fCurrentRecoParam->GetSystematicErrorClusterCustom() ?  
    fCurrentRecoParam->GetSystematicErrorClusterCustom()[1] : fCurrentRecoParam->GetSystematicErrorCluster()[1];
  if (sysErrY) serry2 += sysErrY*sysErrY;  // common syst error
  if (sysErrZ) serrz2 += sysErrZ*sysErrZ;  // common syst error
  //
  double useDistY = fCurrentRecoParam->GetUseDistortionFractionAsErrorY();
  if (useDistY>0) {
    float dstY = cl->GetDistortionY()*useDistY;
    float dstX = cl->GetDistortionX()*useDistY*tgPhi;
    serry2 += dstY*dstY + dstX*dstX;
  }
  double useDispY = fCurrentRecoParam->GetUseDistDispFractionAsErrorY();
  if (useDispY>0) {
    useDispY *= cl->GetDistortionDispersion();
    serry2 += useDispY*useDispY;
  }
  //
  double useDistZ = fCurrentRecoParam->GetUseDistortionFractionAsErrorZ();
  if (useDistZ>0) {
    float dstZ = cl->GetDistortionZ()*useDistZ;
    float dstX = cl->GetDistortionX()*useDistZ*tgLam;
    serrz2 += dstZ*dstZ + dstX*dstX;
  }
  double useDispZ = fCurrentRecoParam->GetUseDistDispFractionAsErrorZ();
  if (useDispZ>0) {
    useDispZ *= cl->GetDistortionDispersion();
    serrz2 += useDispZ*useDispZ;
  }
  //
}

//_____________________________________________________________________________________
Double_t AliTPCTransform::ErrY2Syst(const AliTPCclusterMI * cl, const double tgAng)
{
  // static function to calculate systematic errorY^2 on the cluster with current reco param
  // Must be static to be used also by external routines
  // tgAng is the ab.tg(inclination wrt padrow)
  const float kEpsZBoundary = 1.e-6; // to disentangle A,C and A+C-common regions
  const float kMaxExpArg = 9.; // limit r-dumped error to this exp. argument
  const float kMaxExpArgZ = TMath::Sqrt(2.*kMaxExpArg);
  double sysErr2 = 0., sysErr = 0.;
  //
  // error on particular TPC subvolume (charging up)
  const Double_t *errInner = fCurrentRecoParam->GetSystematicErrorClusterInner();
  if (errInner[0]>0) {
    //
    double dr = TMath::Abs(cl->GetX()-85.);
    float argExp = dr/errInner[1];     // is the Z-range limited ?
    if (argExp<kMaxExpArg) {
      const TVectorF* zranges = fCurrentRecoParam->GetSystErrClInnerRegZ(); // is the Z-range of dumped error defined?
      int nz;
      if (zranges && (nz=zranges->GetNoElements())) {
	Bool_t sideC = (cl->GetDetector()/18)&0x1; // is this C-side cluster
	const TVectorF* zrangesSigI = fCurrentRecoParam->GetSystErrClInnerRegZSigInv();
	float zcl = cl->GetZ(); // chose the closest range within 3 sigma
	const float* zr = zranges->GetMatrixArray();
	const float* zsi= zrangesSigI->GetMatrixArray();
	float dzarg = 999.;
	for (int i=nz;i--;) { // find closest region
	  if      (zr[i]<-kEpsZBoundary && !sideC) continue; // don't apply to A-side clusters if the Z-boundary is for C-region
	  else if (zr[i]>kEpsZBoundary  &&  sideC) continue; // don't apply to C-side clusters if the Z-boundary is for A-region
	  float dz = TMath::Abs( (zr[i]-zcl)*zsi[i] );
	  if (dz<dzarg) dzarg = dz;
	}
	if (dzarg<kMaxExpArgZ) { // is it small enough to call exp?
	  argExp += 0.5*dzarg*dzarg;
	  if (argExp<kMaxExpArg) sysErr = errInner[0]*TMath::Exp(-argExp);
	}	
      }
      else sysErr = errInner[0]*TMath::Exp(-argExp); // no condition on Z
    }

  }
  sysErr2 += sysErr*sysErr;
  //
  sysErr = fCurrentRecoParam->GetSystematicErrorClusterCustom() ?  
    fCurrentRecoParam->GetSystematicErrorClusterCustom()[0] : fCurrentRecoParam->GetSystematicErrorCluster()[0];
  if (sysErr) sysErr2 += sysErr*sysErr;  // common syst error
  //
  double useDist = fCurrentRecoParam->GetUseDistortionFractionAsErrorY();
  if (useDist>0) {
    float dstY = cl->GetDistortionY()*useDist;
    float dstX = cl->GetDistortionX()*useDist*tgAng;
    sysErr2 += dstY*dstY + dstX*dstX;
  }
  double useDisp = fCurrentRecoParam->GetUseDistDispFractionAsErrorY();
  if (useDisp>0) {
    useDisp *= cl->GetDistortionDispersion();
    sysErr2 += useDisp*useDisp;
  }
  return sysErr2;
}

//_____________________________________________________________________________________
Double_t AliTPCTransform::ErrZ2Syst(const AliTPCclusterMI * cl, const double tgAng)
{
  // static function to calculate systematic errorY^2 on the cluster with current reco param
  // Must be static to be used also by external routines
  // tgAng is the ab.tg(lambda)
  const float kEpsZBoundary = 1.e-6; // to disentangle A,C and A+C-common regions
  const float kMaxExpArg = 9.; // limit r-dumped error to this exp. argument
  const float kMaxExpArgZ = TMath::Sqrt(2.*kMaxExpArg);
  double sysErr2 = 0., sysErr = 0.;
  //
  // error on particular TPC subvolume (charging up)
  const Double_t *errInner = fCurrentRecoParam->GetSystematicErrorClusterInner();
  if (errInner[0]>0) {
    //
    double dr = TMath::Abs(cl->GetX()-85.);
    float argExp = dr/errInner[1];     // is the Z-range limited ?
    if (argExp<kMaxExpArg) {
      const TVectorF* zranges = fCurrentRecoParam->GetSystErrClInnerRegZ(); // is the Z-range of dumped error defined?
      int nz;
      if (zranges && (nz=zranges->GetNoElements())) {
	Bool_t sideC = (cl->GetDetector()/18)&0x1; // is this C-side cluster
	const TVectorF* zrangesSigI = fCurrentRecoParam->GetSystErrClInnerRegZSigInv();
	float zcl = cl->GetZ(); // chose the closest range within 3 sigma
	const float* zr = zranges->GetMatrixArray();
	const float* zsi= zrangesSigI->GetMatrixArray();
	float dzarg = 999.;
	for (int i=nz;i--;) { // find closest region
	  if      (zr[i]<-kEpsZBoundary && !sideC) continue; // don't apply to A-side clusters if the Z-boundary is for C-region
	  else if (zr[i]>kEpsZBoundary  &&  sideC) continue; // don't apply to C-side clusters if the Z-boundary is for A-region
	  float dz = TMath::Abs( (zr[i]-zcl)*zsi[i] );
	  if (dz<dzarg) dzarg = dz;
	}
	if (dzarg<kMaxExpArgZ) { // is it small enough to call exp?
	  argExp += 0.5*dzarg*dzarg;
	  if (argExp<kMaxExpArg) sysErr = errInner[0]*TMath::Exp(-argExp);
	}	
      }
      else sysErr = errInner[0]*TMath::Exp(-argExp); // no condition on Z
    }

  }
  sysErr2 += sysErr*sysErr;
  //
  sysErr = fCurrentRecoParam->GetSystematicErrorClusterCustom() ?  
    fCurrentRecoParam->GetSystematicErrorClusterCustom()[1] : fCurrentRecoParam->GetSystematicErrorCluster()[1];
  if (sysErr) sysErr2 += sysErr*sysErr;  // common syst error
  //
  double useDist = fCurrentRecoParam->GetUseDistortionFractionAsErrorZ();
  if (useDist>0) {
    float dstZ = cl->GetDistortionZ()*useDist;
    float dstX = cl->GetDistortionX()*useDist*tgAng;
    sysErr2 += dstZ*dstZ + dstX*dstX;
  }
  double useDisp = fCurrentRecoParam->GetUseDistDispFractionAsErrorZ();
  if (useDisp>0) {
    useDisp *= cl->GetDistortionDispersion();
    sysErr2 += useDisp*useDisp;
  }
  return sysErr2;
}

//_________________________________
void AliTPCTransform::ResetCache()
{
  // reset cached values for last time bin, consider as tmp measure. Preferable to use data members
  // instead of static vars.
  //
  AliInfo("Reseting Transform in view of possible RecoParam modification");
  fCurrentRecoParam = 0;
  fCurrentMapScaling = 1.0;
  SetCurrentTimeStamp(-1);
  Local2RotatedGlobal(-1,0); // this will reset the cached values for VDrift
  //
}

AliHLTTPCReverseTransformInfoV1* AliTPCTransform::GetReverseTransformInfo()
{
    AliHLTTPCReverseTransformInfoV1* info = new AliHLTTPCReverseTransformInfoV1;
    fTmpReverseTransformInfo = info;
    Local2RotatedGlobal(0, NULL);
    Local2RotatedGlobal(18, NULL);
    fTmpReverseTransformInfo = NULL;
    return(info);
}

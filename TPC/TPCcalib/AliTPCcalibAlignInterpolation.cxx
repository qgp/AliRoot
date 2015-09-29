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


/// \class AliTPCcalibAlignInterpolation
/// Class to produce TPC time dependent space point distortion maps using the ITS, TRD and TOF 
/// as a reference detector
///  
/// Related to task https://alice.its.cern.ch/jira/browse/ATO-108
///  - code created addopting compiled macro for open gating grid analysis form TPC git:
///    $NOTES/SpaceChargeDistortion/code/spaceChargeDistortions.C
/// 
/// \author Marian Ivanov,  marian.ivanov@cern.ch

#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "TTreeStream.h"
#include "AliESDfriendTrack.h"
#include "AliExternalTrackParam.h"
#include "AliTrackPointArray.h"
#include "TChain.h"
#include "AliXRDPROOFtoolkit.h"
#include "AliTrackerBase.h"
#include "AliGeomManager.h"
//#include "THnSparse.h"
//#include "THnBase.h"
#include "THn.h"
#include "AliSysInfo.h"
#include "TMatrixD.h"
#include "TF1.h"
#include "TDatabasePDG.h"
#include "TTreeStream.h"
#include "TStatToolkit.h"


#include "AliTPCcalibAlignInterpolation.h"

const Int_t AliTPCcalibAlignInterpolation_kMaxPoints=500;

ClassImp(AliTPCcalibAlignInterpolation)


AliTPCcalibAlignInterpolation::AliTPCcalibAlignInterpolation() : 
  AliTPCcalibBase(),
  fOnTheFlyFill(0),  // flag - on the fly filling of histograms
  fHisITSDRPhi(0),      // TPC-ITS residual histograms
  fHisITSTRDDRPhi(0),   // TPC-ITS+TRD residual histograms
  fHisITSTOFDRPhi(0),   // TPC-ITS_TOF residual histograms
  fStreamer(0),         // calibration streamer 
  fStreamLevel(0),      // stream level - In mode 0 only basic information needed for calibration  stored (see EStream
  fSyswatchStep(0),      // dump system resource information after  fSyswatchStep tracks
  fTrackCounter(0)           // processed track counter
{
  
}   
AliTPCcalibAlignInterpolation::AliTPCcalibAlignInterpolation(const Text_t *name, const Text_t *title, Bool_t onTheFlyFill):
  AliTPCcalibBase(),
  fOnTheFlyFill(onTheFlyFill),  // flag - on the fly filling of histograms
  fHisITSDRPhi(0),      // TPC-ITS residual histograms
  fHisITSTRDDRPhi(0),   // TPC-ITS+TRD residual histograms
  fHisITSTOFDRPhi(0),   // TPC-ITS_TOF residual histograms  
  fStreamer(0),         // calibration streamer 
  fStreamLevel(0),      // stream level - In mode 0 only basic information needed for calibration  stored (see EStream
  fSyswatchStep(0),      // dump system resource information after  fSyswatchStep tracks  
  fTrackCounter(0)           // processed track counter
{
  // create output histograms
  SetName(name);
  SetTitle(title);
  if (onTheFlyFill) CreateResidualHistosInterpolation();
}   

AliTPCcalibAlignInterpolation::~AliTPCcalibAlignInterpolation(){
  //
  //
  //
  delete fStreamer;
  delete fHisITSDRPhi;
  delete fHisITSTRDDRPhi;
  delete fHisITSTOFDRPhi;
}



Bool_t  AliTPCcalibAlignInterpolation::RefitITStrack(AliESDfriendTrack *friendTrack, Double_t mass, AliExternalTrackParam &trackITS, Double_t &chi2, Double_t &npoints){
  //
  // Makes a refit of the ITS track
  // Input: AliESDfriendTrack, particle mass, outer ITS TrackParam 
  // Output: AliExternalTrackParam of the ITS track refit at the last layer of ITS
  //
  const Double_t kMaxRadius=50;
  Int_t sortedIndex[AliTPCcalibAlignInterpolation_kMaxPoints]={0};
  if (friendTrack->GetITSOut()==NULL) return kFALSE;  
  Double_t sigma[2] = {0.002,0.01};    // ITS intrincsic resolution in (y,z)  - error from the points can be used SD layer 2-3 sighnificantly bigger error
  // !!!! We should set ITS error parameterization form outside !!!!
  //
  trackITS = *((AliExternalTrackParam*)friendTrack->GetITSOut());
  // Reset track to the vertex
  AliTrackerBase::PropagateTrackToBxByBz(&trackITS,0.,mass,1,kFALSE);
  trackITS.ResetCovariance(1000.);
  
  // Get space points
  AliTrackPointArray *pointarray = (AliTrackPointArray*)friendTrack->GetTrackPointArray();
  if (!pointarray){
    printf("Space points are not stored in the friendTrack!\n");
    return kFALSE;
  };
  Int_t nPoints = pointarray->GetNPoints();  // # space points of all available detectors                                            
                                             // Sort space points first
  SortPointArray(pointarray, sortedIndex);  // space point indices sorted by radius in increasing order
  
  // Propagate track through ITS space points
  AliTrackPoint spacepoint;
  chi2=0;
  npoints=0; 
  Int_t volId=0,modId=0,layerId=0;
  
  for (Int_t iPoint=0;iPoint<nPoints;iPoint++){
    pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
    Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
    Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
    trackITS.Global2LocalPosition(xyz,alpha);
    if (xyz[0]>kMaxRadius) break;  // use only ITS points - maybe we should indexes of elements
    trackITS.Rotate(alpha);
    Bool_t status = AliTrackerBase::PropagateTrackToBxByBz(&trackITS,xyz[0],mass,1,kFALSE);
    if (status){
      Double_t pos[2] = {xyz[1], xyz[2]};
      Double_t cov[3] = {sigma[0]*sigma[0],0, sigma[1]*sigma[1]};   
      volId = spacepoint.GetVolumeID();
      layerId = AliGeomManager::VolUIDToLayer(volId,modId);
      if (layerId ==AliGeomManager::kSDD1 || layerId ==AliGeomManager::kSDD2) {
        cov[0]*=16.; cov[2]*=16.;
      }      
      double chi2cl = trackITS.GetPredictedChi2(pos,cov);
      chi2 += chi2cl;
      npoints++;
      trackITS.Update(pos,cov);      
    }
  }
  if (fStreamer && ((fStreamLevel&kStreamITSRefit)!=0)){
    (*fStreamer)<<"its"<<
    "trackITS.="<<&trackITS<<     // should be ITS track at the outermost ITS point
                                  //      "status="<<status<<
    "chi2="<<chi2<<
    "npoints="<<npoints<<         
    "\n";
  }
  return kTRUE;
}


Bool_t AliTPCcalibAlignInterpolation::RefitTRDtrack(AliESDfriendTrack *friendTrack, Double_t mass, AliExternalTrackParam &trackTRD, Double_t &chi2, Double_t &npoints){
  //
  // Makes a refit of the TRD track
  // Input: AliESDfriendTrack, particle mass, inner TRD TrackParam 
  // Output: AliExternalTrackParam of the TRD track refit - in the first layer of TRD
  // Here we forgot about the tiliting pads of TRD - we assume delta Z and delta y are uncorelated
  //      given approximation is in average tru - in case avearaging of significantly bigger than pad length
  //  
  Double_t sigma[2] = {0.02, 5};
  Int_t sortedIndex[AliTPCcalibAlignInterpolation_kMaxPoints]={0};
  const Double_t kMaxRadius=370;
  const Double_t kMinRadius=280;
  if (friendTrack->GetTRDIn() == NULL) return kFALSE;
  trackTRD = *((AliExternalTrackParam*)friendTrack->GetTRDIn());
  
  
  // Reset track outside TRD
  AliTrackerBase::PropagateTrackToBxByBz(&trackTRD,370.,mass,1,kFALSE);
  trackTRD.ResetCovariance(1000.);
  
  // Get space points
  AliTrackPointArray *pointarray = (AliTrackPointArray*)friendTrack->GetTrackPointArray();
  if (!pointarray){
    printf("Space points are not stored in the friendTrack!\n");
    return kFALSE;
  };
  Int_t nPoints = pointarray->GetNPoints();  // # space points of all available detectors
                                             // Sort space points first
  SortPointArray(pointarray, sortedIndex);  // space point indices sorted by radius in increasing order

  // Propagate track through TRD space points
  AliTrackPoint spacepoint;
  chi2=0; 
  npoints=0;
  for (Int_t iPoint=nPoints-1;iPoint>=0;iPoint--){
    pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
    Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
    Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
    trackTRD.Global2LocalPosition(xyz,alpha);
    if (xyz[0]>kMaxRadius) continue;  // use only TRD points
    if (xyz[0]<kMinRadius) break;  // use only TRD points
    trackTRD.Rotate(alpha);
    Bool_t status = AliTrackerBase::PropagateTrackToBxByBz(&trackTRD,xyz[0],mass,1,kFALSE);
    if (status){
      Double_t pos[2] = {xyz[1], xyz[2]};
      Double_t cov[3] = {sigma[0]*sigma[0],0,sigma[1]*sigma[1]};
      double chi2cl = trackTRD.GetPredictedChi2(pos,cov);
      chi2 += chi2cl;
      npoints++;
      trackTRD.Update(pos,cov);
    }
  }
  if (fStreamer && ((fStreamLevel&kStreamTRDRefit)!=0)){
    (*fStreamer)<<"trd"<<
    "trackTRD.="<<&trackTRD<<     // should be TRD track at the innermost TRD point
                                  //"status="<<status<<    
    "chi2="<<chi2<< 
    "npoints="<<npoints<<
    "\n";
  }
  
  return kTRUE;
}


Bool_t  AliTPCcalibAlignInterpolation::RefitTOFtrack(AliESDfriendTrack *friendTrack, Double_t mass, AliExternalTrackParam &trackTOF, Double_t &chi2, Double_t &npoints){
  //
  // Makes a refit of the TRD track
  // Input: AliESDfriendTrack, particle mass, OUTER ITS track - propagated to the TOF point and updated by TOF point 
  // Output: AliExternalTrackParam of the TOF track refit - at the TOF point
  Double_t sigma[2] = {1., 1.};      // should be parameterized
  const Double_t kTOFRadius = 370;
  Int_t sortedIndex[AliTPCcalibAlignInterpolation_kMaxPoints]={0};
  AliTrackerBase::PropagateTrackToBxByBz(&trackTOF,kTOFRadius,mass,10,kTRUE);
  Int_t volId,modId,layerId;
  
  // Get space points
  AliTrackPointArray *pointarray = (AliTrackPointArray*)friendTrack->GetTrackPointArray();
  if (!pointarray){
    printf("Space points are not stored in the friendTrack!\n");
    return kFALSE;
  };
  Int_t nPoints = pointarray->GetNPoints();  // # space points of all available detectors
                                             // Sort space points first
  SortPointArray(pointarray, sortedIndex);  // space point indices sorted by radius in increasing order
  // Propagate track through TRD space points
  AliTrackPoint spacepoint;
  chi2=0; 
  npoints=0;
  for (Int_t iPoint=nPoints-1;iPoint>=0;iPoint--){  
    pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
    volId = spacepoint.GetVolumeID();
    layerId = AliGeomManager::VolUIDToLayer(volId,modId);
    if (layerId !=AliGeomManager::kTOF) continue;
    
    Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
    Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
    trackTOF.Global2LocalPosition(xyz,alpha);
    trackTOF.Rotate(alpha);
    Bool_t status = AliTrackerBase::PropagateTrackToBxByBz(&trackTOF,xyz[0],mass,1,kFALSE);
    if (status){
      Double_t pos[2] = {xyz[1], xyz[2]};
      Double_t cov[3] = {sigma[0]*sigma[0],0,sigma[1]*sigma[1]};
      double chi2cl = trackTOF.GetPredictedChi2(pos,cov);
      chi2 += chi2cl;
      npoints++;
      trackTOF.Update(pos,cov);
    }
  }
  if (fStreamer && ((fStreamLevel&kStreamTOFRefit)!=0)){
    (*fStreamer)<<"tof"<<
      "trackTOF.="<<&trackTOF<<     // should be TOF-ITS  track at the TOF point
      //"status="<<status<<
      "chi2="<<chi2<< 
      "npoints="<<npoints<<
      "\n";
  }  
  return kTRUE;  
}




Bool_t  AliTPCcalibAlignInterpolation::SortPointArray(AliTrackPointArray *pointarray, Int_t * sortedIndex){
  //
  // Fill array of indexes to the pointArray (array sorted in increasing order)
  //
  if (sortedIndex==NULL) return kFALSE;
  Int_t nPoints = pointarray->GetNPoints();
  Double_t xunsorted[nPoints];
  AliTrackPoint spacepoint;
  AliExternalTrackParam param;
  for (Int_t iPoint=0;iPoint<nPoints;iPoint++){
    pointarray->GetPoint(spacepoint,iPoint);
    Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
    Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
    param.Global2LocalPosition(xyz,alpha);
    xunsorted[iPoint] = xyz[0];
  }
  if (nPoints==0) {
    return kFALSE;
  }
  TMath::Sort(nPoints,xunsorted,sortedIndex,kFALSE);
  return kTRUE;
}





void  AliTPCcalibAlignInterpolation::Process(AliESDEvent *esdEvent){
  //
  // Create distortion maps out of residual histograms of ITS-TRD interpolation and TPC space points
  // JIRA ticket: https://alice.its.cern.ch/jira/browse/ATO-108
  //
  AliESDfriend *esdFriend=static_cast<AliESDfriend*>(esdEvent->FindListObject("AliESDfriend"));
  if (!esdFriend) return;
  if (esdFriend->TestSkipBit()) return;

  Int_t nTracks = esdEvent->GetNumberOfTracks();  // Get number of tracks in ESD
  if (nTracks==0) return;
  if (!fStreamer) fStreamer = new TTreeSRedirector("ResidualHistos.root","recreate");
  //
  const Int_t nPointsAlloc=AliTPCcalibAlignInterpolation_kMaxPoints; 
  AliExternalTrackParam trackArrayITS[nPointsAlloc];
  AliExternalTrackParam trackArrayTRD[nPointsAlloc];
  AliExternalTrackParam trackArrayTOF[nPointsAlloc];
  AliExternalTrackParam trackArrayITSTRD[nPointsAlloc];
  AliExternalTrackParam trackArrayITSTOF[nPointsAlloc];
  //
  //MakeResidualHistosInterpolation();
  //
  Int_t sortedIndex[AliTPCcalibAlignInterpolation_kMaxPoints];
  
  for (Int_t iTrack=0;iTrack<nTracks;iTrack++){ // Track loop
    // 0.) For each track in each event, get the AliESDfriendTrack
    AliESDtrack *esdTrack = esdEvent->GetTrack(iTrack);
    AliESDfriendTrack *friendTrack = esdFriend->GetTrack(iTrack);
    if (!friendTrack) continue;      
    Double_t mass = esdTrack->GetMass();  // particle mass      
    //
    // 1.) Start with AliExternalTrackParam *ITSOut and *TRDIn 
    //
    AliExternalTrackParam paramITS;
    Double_t itsChi2=0, itsNCl=0;
    AliExternalTrackParam paramTRD;
    Double_t trdChi2=0, trdNCl=0;
    AliExternalTrackParam paramTOF;
    Double_t tofChi2=0, tofNCl=0;            
    //
    // 2.) ITS, TRD and ITS-TRD refits
    //
    RefitITStrack(friendTrack,mass,paramITS, itsChi2, itsNCl );
    if (itsNCl<3) continue; 
    RefitTRDtrack(friendTrack,mass,paramTRD, trdChi2, trdNCl); 
    paramTOF=paramITS;
    RefitTOFtrack(friendTrack,mass,paramTOF, tofChi2, tofNCl );
    if (fTrackCounter%fSyswatchStep==0) AliSysInfo::AddStamp("Refitting",fTrackCounter,1,0,0);        
    //
    // 3.) Propagate to TPC volume, histogram residuals to TPC clusters and dump all information to TTree
    //
    AliTrackPoint spacepoint;  
    Int_t volId,modId,layerId;      
    AliTrackPointArray *pointarray = (AliTrackPointArray*)friendTrack->GetTrackPointArray();
    if (!pointarray) continue;
    Int_t nPointsAll = pointarray->GetNPoints();  // # space points of all available detectors
    SortPointArray(pointarray, sortedIndex);  // space point indices sorted by radius in increasing order
    fTrackCounter++; // increase total track number
    //
    // 4.) Propagate  ITS tracks outward
    // 
    //       
    for (Int_t iPoint=0;iPoint<nPointsAll;iPoint++){
      trackArrayITS[sortedIndex[iPoint]].SetUniqueID(0);
      if (itsNCl<3) continue; 
      pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
      volId = spacepoint.GetVolumeID();
      layerId = AliGeomManager::VolUIDToLayer(volId,modId);
      if (layerId !=AliGeomManager::kTPC1 && layerId !=AliGeomManager::kTPC2) continue;
      Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
      Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
      paramITS.Global2LocalPosition(xyz,alpha);	
      paramITS.Rotate(alpha);
      Bool_t itsOK = AliTrackerBase::PropagateTrackToBxByBz(&paramITS,xyz[0],mass,1,kFALSE);
      if (itsOK){
	trackArrayITS[sortedIndex[iPoint]]=paramITS;
	trackArrayITS[sortedIndex[iPoint]].SetUniqueID(1);
      }
      if (fTrackCounter%fSyswatchStep==0) AliSysInfo::AddStamp("ExtrapolateITS",fTrackCounter,2,0,0);  
    }
    //
    // 5.) Propagate  TRD/TOF tracks inwards
    //
    for (Int_t iPoint=nPointsAll-1;iPoint>0;iPoint--){
      trackArrayTRD[sortedIndex[iPoint]].SetUniqueID(0);
      trackArrayTOF[sortedIndex[iPoint]].SetUniqueID(0);
      trackArrayITSTRD[sortedIndex[iPoint]].SetUniqueID(0);
      trackArrayITSTOF[sortedIndex[iPoint]].SetUniqueID(0);
      //
      pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
      volId = spacepoint.GetVolumeID();
      layerId = AliGeomManager::VolUIDToLayer(volId,modId);
      if (layerId !=AliGeomManager::kTPC1 && layerId !=AliGeomManager::kTPC2) continue;
      if (trdNCl>0){
	Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
	Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
	paramTRD.Global2LocalPosition(xyz,alpha);	
	paramTRD.Rotate(alpha);
	Bool_t trdOK = AliTrackerBase::PropagateTrackToBxByBz(&paramTRD,xyz[0],mass,1,kFALSE);
	if (trdOK){
	  trackArrayTRD[sortedIndex[iPoint]]=paramTRD;
	  trackArrayITSTRD[sortedIndex[iPoint]]=paramTRD;
	  trackArrayTRD[sortedIndex[iPoint]].SetUniqueID(1);
	  trackArrayITSTRD[sortedIndex[iPoint]].SetUniqueID(1);
	  AliTrackerBase::UpdateTrack(trackArrayITSTRD[sortedIndex[iPoint]], trackArrayITS[sortedIndex[iPoint]]);	  
	}
	if (fTrackCounter%fSyswatchStep==0) AliSysInfo::AddStamp("InterpolateTRD",fTrackCounter,3,0,0);  
      }
      if (tofNCl>0){
	Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
	Double_t alpha = TMath::ATan2(xyz[1],xyz[0]);
	paramTOF.Global2LocalPosition(xyz,alpha);	
	paramTOF.Rotate(alpha);
	Bool_t tofOK = AliTrackerBase::PropagateTrackToBxByBz(&paramTOF,xyz[0],mass,1,kFALSE);
	if (tofOK){
	  trackArrayTOF[sortedIndex[iPoint]]=paramTOF;
	  trackArrayITSTOF[sortedIndex[iPoint]]=paramTOF;
	  trackArrayTOF[sortedIndex[iPoint]].SetUniqueID(1);
	  trackArrayITSTOF[sortedIndex[iPoint]].SetUniqueID(1);
	  AliTrackerBase::UpdateTrack(trackArrayITSTOF[sortedIndex[iPoint]], trackArrayITS[sortedIndex[iPoint]]);	  
	}
	if ((fTrackCounter%fSyswatchStep)==0) AliSysInfo::AddStamp("InterpolateTOF",fTrackCounter,4,0,0);  
      }
    }
    if ( ((fStreamLevel&kStremInterpolation)>0) &&  ((fTrackCounter%fSyswatchStep)==0)){
      for (Int_t iPoint=nPointsAll-1;iPoint>0;iPoint--){
	pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
	volId = spacepoint.GetVolumeID();
	layerId = AliGeomManager::VolUIDToLayer(volId,modId);
	if (layerId !=AliGeomManager::kTPC1 && layerId !=AliGeomManager::kTPC2) continue;
        
	(*fStreamer)<<"interpolation"<<
          "itrack="<<fTrackCounter<<  // total track #
          "point.="<<&spacepoint<<  // space points
                                    //
          "itsNCl="<<itsNCl<<
          "trdNCl="<<trdNCl<<
          "tofNCl="<<tofNCl<<
          //
          "itsChi2="<<itsChi2<<
          "trdChi2="<<trdChi2<<
          "tofChi2="<<tofChi2<<
          //
          "trackITS.="<<&trackArrayITS[sortedIndex[iPoint]]<<  // ITS fit
          "trackTRD.="<<&trackArrayTRD[sortedIndex[iPoint]]<<  // TRD fit
          "trackTOF.="<<&trackArrayTOF[sortedIndex[iPoint]]<<  // TOF fit
          "trackITSTRD.="<<&trackArrayITSTRD[sortedIndex[iPoint]]<<  // ITS-TRD fit
          "trackITSTOF.="<<&trackArrayITSTOF[sortedIndex[iPoint]]<<  // ITS-TOF fit
          "\n";	
      }
    }
    
    //
    // 6.) Fill residual histograms
    //
    if (fHisITSDRPhi!=NULL){
      for (Int_t iPoint=nPointsAll-1;iPoint>0;iPoint--){
	pointarray->GetPoint(spacepoint,sortedIndex[iPoint]);
	Double_t xyz[3] = {(Double_t)spacepoint.GetX(),(Double_t)spacepoint.GetY(),(Double_t)spacepoint.GetZ()};
	Double_t sec = (9.*TMath::ATan2(xyz[1],xyz[0])/TMath::Pi()>0) ? (9.*TMath::ATan2(xyz[1],xyz[0])/TMath::Pi()) : (9.*TMath::ATan2(xyz[1],xyz[0])/TMath::Pi()+18.);
	Double_t r = TMath::Sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
	Double_t theta = xyz[2] / r;
	if (trackArrayITS[sortedIndex[iPoint]].GetUniqueID()==1){
	  Double_t itsQPt = trackArrayITS[sortedIndex[iPoint]].GetSigned1Pt();
	  Double_t itsDy = -1*trackArrayITS[sortedIndex[iPoint]].GetY();
	  Double_t itsDelta[5] = {itsDy, sec, r, theta, itsQPt};
	  fHisITSDRPhi->Fill(itsDelta);
	}
	if (trackArrayITSTRD[sortedIndex[iPoint]].GetUniqueID()==1){
	  Double_t itstrdQPt = trackArrayITSTRD[sortedIndex[iPoint]].GetSigned1Pt();
	  Double_t itstrdDy = -1*trackArrayITSTRD[sortedIndex[iPoint]].GetY();
	  Double_t itstrdDelta[5] = {itstrdDy, sec, r, theta, itstrdQPt};
	  fHisITSTRDDRPhi->Fill(itstrdDelta);
	}
	if (trackArrayITSTOF[sortedIndex[iPoint]].GetUniqueID()==1){
	  Double_t itstofQPt = trackArrayITSTOF[sortedIndex[iPoint]].GetSigned1Pt();
	  Double_t itstofDy = -1*trackArrayITSTOF[sortedIndex[iPoint]].GetY();
	  Double_t itstofDelta[5] = {itstofDy, sec, r, theta, itstofQPt};
	  fHisITSTOFDRPhi->Fill(itstofDelta);
	}
      if (fTrackCounter%fSyswatchStep==0) AliSysInfo::AddStamp("FillHistos",fTrackCounter,5,0,0);  
      }
    }
    //
  } // end of track loop
    //
  //
  //
  fStreamer->GetFile()->cd();
}

void AliTPCcalibAlignInterpolation::CreateResidualHistosInterpolation(){
  //
  // Make cluster residual histograms
  //
  Double_t xminTrack[9], xmaxTrack[9];
  Double_t xminTrackITS[9], xmaxTrackITS[9];
  Int_t    binsTrack[9], binsTrackITS[9];
  TString  axisName[9],axisTitle[9];
  //
  // 0 - delta   of interest
  // 1 - global  phi in sector number  as float
  // 2 - local   r
  // 3 - local   kz
  // 4 - local   q/pt
  // 
  // gx,gy,gz - will be taken from the TPC
  //
  axisName[0]="delta";   axisTitle[0]="#Delta (cm)";                 // to fill    local(clusterY-track.y)
  binsTrack[0]=90;       xminTrack[0]=-1.5;        xmaxTrack[0]=1.5; 
  binsTrackITS[0]=90;    xminTrackITS[0]=-1.5;     xmaxTrackITS[0]=1.5; 
  //
  axisName[1]="sector";  axisTitle[1]="Sector Number";              // to fill:   9*atan2(gy,gx)/pi+ if (sector>0) sector+18
  binsTrack[1]=180;      xminTrack[1]=0;           xmaxTrack[1]=18; 
  binsTrackITS[1]=180;   xminTrackITS[1]=0;        xmaxTrackITS[1]=18; 
  //
  axisName[2]="R";       axisTitle[2]="r (cm)";                          // to fill:    gr=sqrt(gy**2+gx**2)
  binsTrack[2]=53;       xminTrack[2]=85.;         xmaxTrack[2]=245.; 
  binsTrackITS[2]=53;    xminTrackITS[2]=85.;      xmaxTrackITS[2]=245.; 
  //
  //
  axisName[3]="kZ";      axisTitle[3]="z/r";                          // to fill : gz/gr 
  binsTrack[3]=20;       xminTrack[3]=-1.0;        xmaxTrack[3]=1.0;  // +-1 for ITS+TRD and ITS+TOF 
  binsTrackITS[3]=36;    xminTrackITS[3]=-1.8;     xmaxTrackITS[3]=1.8;  // +-1.8 for the ITS 
  //
  axisName[4]="qpt";    axisTitle[4]="q/pt (c/GeV)";                         // to fill : track.GetSigned1Pt() 
  binsTrack[4]=5;        xminTrack[4]=-2.5;        xmaxTrack[4]=2.5; 
  binsTrackITS[4]=5;     xminTrackITS[4]=-2.5;     xmaxTrackITS[4]=2.5; 


  //
  fHisITSDRPhi = new THnF("deltaRPhiTPCITS","#Delta_{Y} (cm)", 5, binsTrackITS,xminTrackITS, xmaxTrackITS);
  fHisITSTRDDRPhi = new THnF("deltaRPhiTPCITSTRD","#Delta_{Y} (cm) TPC-(ITS+TRD)", 5, binsTrack,xminTrack, xmaxTrack);
  fHisITSTOFDRPhi = new THnF("deltaRPhiTPCITSTOF","#Delta_{Y} (cm) TPC-(ITS+TOF)", 5, binsTrack,xminTrack, xmaxTrack);
  //
  //
  //
  for (Int_t ivar2=0;ivar2<5;ivar2++){
    fHisITSDRPhi->GetAxis(ivar2)->SetName(axisName[ivar2].Data());
    fHisITSDRPhi->GetAxis(ivar2)->SetTitle(axisName[ivar2].Data());
    fHisITSTRDDRPhi->GetAxis(ivar2)->SetName(axisName[ivar2].Data());
    fHisITSTRDDRPhi->GetAxis(ivar2)->SetTitle(axisName[ivar2].Data());
    fHisITSTOFDRPhi->GetAxis(ivar2)->SetName(axisName[ivar2].Data());
    fHisITSTOFDRPhi->GetAxis(ivar2)->SetTitle(axisName[ivar2].Data());
  }

}



void  AliTPCcalibAlignInterpolation::CreateDistortionMapsFromFile(const char * inputFile, const char *outputFile){
  //
  // Create distortion maps from residual histograms
  // TPC cluster to ITS, ITS-TRD and ITS-TOF track fits
  //
  TFile *fHistos  = TFile::Open(inputFile);
  
  THnF *histoITS = (THnF*) fHistos->Get("deltaRPhiTPCITS");
  THnF *histoITSTRD= (THnF*) fHistos->Get("deltaRPhiTPCITSTRD");
  THnF *histoITSTOF = (THnF*) fHistos->Get("deltaRPhiTPCITSTOF");
  
  TTreeSRedirector * pcstream = new TTreeSRedirector(outputFile,"recreate");
  
  TMatrixD projectionInfo(5,5);
  projectionInfo(0,0)=0;  projectionInfo(0,1)=0;  projectionInfo(0,2)=0;
  projectionInfo(1,0)=4;  projectionInfo(1,1)=0;  projectionInfo(1,2)=1; 
  projectionInfo(2,0)=3;  projectionInfo(2,1)=0;  projectionInfo(2,2)=1;
  projectionInfo(3,0)=2;  projectionInfo(3,1)=0;  projectionInfo(3,2)=1;
  projectionInfo(4,0)=1;  projectionInfo(4,1)=0;  projectionInfo(4,2)=1;
  
  TStatToolkit::MakeDistortionMap(4, histoITS, pcstream, projectionInfo); 
  TStatToolkit::MakeDistortionMap(4, histoITSTRD, pcstream, projectionInfo); 
  TStatToolkit::MakeDistortionMap(4, histoITSTOF, pcstream, projectionInfo); 
  delete pcstream;
  //
}


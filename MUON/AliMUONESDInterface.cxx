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

// $Id$

#include "AliMUONESDInterface.h"
#include "AliMUONTrack.h"
#include "AliMUONVTrackStore.h"
#include "AliMUONVCluster.h"
#include "AliMUONVClusterStore.h"
#include "AliMUONVDigit.h"
#include "AliMUONVDigitStore.h"
#include "AliMUON1DMapIterator.h"
#include "AliMUON2DMapIterator.h"
#include "AliMUONTrackParam.h"
#include "AliMUONTrackExtrap.h"
#include "AliMUONConstants.h"

#include "AliMpVSegmentation.h"
#include "AliMpSegmentation.h"
#include "AliMpIntPair.h"
#include "AliMpPad.h"

#include "AliESDEvent.h"
#include "AliESDMuonTrack.h"
#include "AliESDMuonCluster.h"
#include "AliESDMuonPad.h"
#include "AliLog.h"

#include <TClass.h>
#include <TIterator.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <Riostream.h>

//-----------------------------------------------------------------------------
/// \class AliMUONESDInterface
///
/// There are 2 way of using thid converter between MUON track/cluster/digit
/// and ESDMuon track/cluster/pad:
/// 
/// 1) using the static methods converting the objects one by one
///
/// 2) loading a whole ESDEvent and using the getters and/or the iterators
///    to access the corresponding MUON objects
///
/// \author Philippe Pillot
//-----------------------------------------------------------------------------

/// \cond CLASSIMP
ClassImp(AliMUONESDInterface)
/// \endcond

TString AliMUONESDInterface::fgTrackStoreName = "AliMUONTrackStoreV1";
TString AliMUONESDInterface::fgClusterStoreName = "AliMUONClusterStoreV2";
TString AliMUONESDInterface::fgDigitStoreName = "AliMUONDigitStoreV2R";

//_____________________________________________________________________________
AliMUONESDInterface::AliMUONESDInterface()
: TObject(),
  fTracks(0x0),
  fDigits(0x0),
  fTrackMap(0x0),
  fClusterMap(0x0),
  fDigitMap(0x0)
{
  /// Default constructor
}

//_____________________________________________________________________________
AliMUONESDInterface::~AliMUONESDInterface()
{
  /// Destructor
  delete fTracks;
  delete fDigits;
  delete fTrackMap;
  delete fClusterMap;
  delete fDigitMap;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                    methods to play with internal objects                    //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//_____________________________________________________________________________
void AliMUONESDInterface::Clear(Option_t*)
{
  /// clear memory
  delete fTracks; fTracks = 0x0;
  delete fDigits; fDigits = 0x0;
  delete fTrackMap; fTrackMap = 0x0;
  delete fClusterMap; fClusterMap = 0x0;
  delete fDigitMap; fDigitMap = 0x0;
}

//_____________________________________________________________________________
void AliMUONESDInterface::Reset()
{
  /// reset stores and maps
  
  if (fTracks) fTracks->Clear("C");
  else fTracks = NewTrackStore();
  
  if (fDigits) fDigits->Clear("C");
  else fDigits = NewDigitStore();
  
  if (fTrackMap) fTrackMap->Clear();
  else fTrackMap = new AliMpExMap(kTRUE);
  fTrackMap->SetOwner(kFALSE);
  
  if (fClusterMap) fClusterMap->Clear();
  else fClusterMap = new AliMpExMap(kTRUE);
  fClusterMap->SetOwner(kTRUE);
  
  if (fDigitMap) fDigitMap->Clear("C");
  else fDigitMap = new TClonesArray("AliMpExMap",10);
}

//_____________________________________________________________________________
void AliMUONESDInterface::LoadEvent(AliESDEvent& esdEvent)
{
  /// Extract MUON data from the given ESD event
  
  // reset data members
  Reset();
  
  // loop over ESD tracks and fill the stores
  Int_t nTracks = (Int_t) esdEvent.GetNumberOfMuonTracks(); 
  for (Int_t iTrack = 0; iTrack <  nTracks; iTrack++) {
    
    // get ESD track
    AliESDMuonTrack* esdTrack = esdEvent.GetMuonTrack(iTrack);
    
    // add it to track store
    AliMUONTrack* track = Add(*esdTrack, *fTracks);
    
    // fill track map
    fTrackMap->Add(iTrack, track);
    
    // prepare cluster map
    fClusterMap->Add(iTrack, new AliMpExMap(kTRUE));
    AliMpExMap* cMap = (AliMpExMap*) fClusterMap->GetObjectFast(iTrack);
    cMap->SetOwner(kFALSE);
    
    // prepare digit maps
    AliMpExMap* dMaps = new((*fDigitMap)[iTrack]) AliMpExMap(kTRUE);
    dMaps->SetOwner(kTRUE);
    
    // loop over ESD clusters
    Int_t nClusters = esdTrack->GetNClusters();
    for (Int_t iCluster = 0; iCluster <  nClusters; iCluster++) {
      
      // get ESD cluster
      AliESDMuonCluster *esdCluster = (AliESDMuonCluster*) esdTrack->GetClusters().UncheckedAt(iCluster);
      
      // get the corresponding MUON cluster
      AliMUONVCluster* cluster = FindClusterInTrack(*track, esdCluster->GetUniqueID());
      
      // fill cluster map
      cMap->Add(cluster->GetUniqueID(), cluster);
      
      // prepare digit map
      dMaps->Add(esdCluster->GetUniqueID(), new AliMpExMap(kTRUE));
      AliMpExMap* dMap = (AliMpExMap*) dMaps->GetObjectFast(iCluster);
      dMap->SetOwner(kFALSE);
      
      // loop over ESD pads
      Int_t nPads = esdCluster->GetNPads();
      for (Int_t iPad = 0; iPad <  nPads; iPad++) {
	
	// get ESD pad
	AliESDMuonPad *esdPad = (AliESDMuonPad*) esdCluster->GetPads().UncheckedAt(iPad);
	
	// add it to digit store
	AliMUONVDigit* digit = Add(*esdPad, *fDigits);
	
	// fill digit map
	if (digit) dMap->Add(esdPad->GetUniqueID(), digit);
	else dMap->Add(esdPad->GetUniqueID(), fDigits->FindObject(esdPad->GetUniqueID()));
	
      } // end of loop over pads
      
    } // end of loop over clusters
    
  } // end of loop over tracks
  
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNTracks() const
{
  /// return the number of tracks
  return fTrackMap ? fTrackMap->GetSize() : 0;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNClusters() const
{
  /// return the number of clusters
  Int_t nClusters = 0;
  Int_t nTracks = GetNTracks();
  for (Int_t i = 0; i < nTracks; i++) nClusters += GetTrackFast(i)->GetNClusters();
  return nClusters;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNClusters(Int_t iTrack) const
{
  /// return the number of clusters in track "iTrack"
  AliMUONTrack* track = GetTrack(iTrack);
  return track ? track->GetNClusters() : 0;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNDigits() const
{
  /// return the number of digits
  return fDigits ? fDigits->GetSize() : 0;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNDigits(Int_t iTrack) const
{
  /// return the number of digits in all clusters of track "iTrack"
  Int_t nDigits = 0;
  Int_t nClusters = GetNClusters(iTrack);
  for (Int_t j = 0; j <  nClusters; j++) nDigits += GetClusterFast(iTrack,j)->GetNDigits();
  return nDigits;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNDigits(Int_t iTrack, Int_t iCluster) const
{
  /// return the number of digits in cluster numbered "iCluster" of track "iTrack"
  AliMUONVCluster* cluster = GetCluster(iTrack, iCluster);
  return cluster ? cluster->GetNDigits() : 0;
}

//___________________________________________________________________________
Int_t AliMUONESDInterface::GetNDigitsInCluster(UInt_t clusterId) const
{
  /// return the number of digits in cluster "clusterId"
  AliMUONVCluster* cluster = FindCluster(clusterId);
  return cluster ? cluster->GetNDigits() : 0;
}

//___________________________________________________________________________
AliMUONTrack* AliMUONESDInterface::GetTrack(Int_t iTrack) const
{
  /// return MUON track "iTrack" (0x0 if not found)
  AliMUONTrack* track = fTrackMap ? (AliMUONTrack*) fTrackMap->GetObject(iTrack) : 0x0;
  if (!track) AliWarning(Form("track %d does not exist",iTrack));
  return track;
}

//___________________________________________________________________________
AliMUONVCluster* AliMUONESDInterface::GetCluster(Int_t iTrack, Int_t iCluster) const
{
  /// return MUON cluster numbered "iCluster" in track "iTrack" (0x0 if not found)
  AliMpExMap* cMap = fClusterMap ? (AliMpExMap*) fClusterMap->GetObject(iTrack) : 0x0;
  AliMUONVCluster* cluster = cMap ? (AliMUONVCluster*) cMap->GetObject(iCluster) : 0x0;
  if (!cluster) AliWarning(Form("cluster #%d in track %d does not exist",iCluster,iTrack));
  return cluster;
}

//___________________________________________________________________________
AliMUONVDigit* AliMUONESDInterface::GetDigit(Int_t iTrack, Int_t iCluster, Int_t iDigit) const
{
  /// return MUON digit numbered "iDigit" in cluster numbered "iCluster" of track "iTrack" (0x0 if not found)
  AliMpExMap* dMaps = fDigitMap ? (AliMpExMap*) fDigitMap->At(iTrack) : 0x0;
  AliMpExMap* dMap = dMaps ? (AliMpExMap*) dMaps->GetObject(iCluster) : 0x0;
  AliMUONVDigit* digit = dMap ? (AliMUONVDigit*) dMap->GetObject(iDigit) : 0x0;
  if (!digit) AliWarning(Form("digit #%d in cluster #%d of track %d does not exist",iDigit,iCluster,iTrack));
  return digit;
}

//___________________________________________________________________________
AliMUONVCluster* AliMUONESDInterface::FindCluster(UInt_t clusterId) const
{
  /// return cluster "clusterId" (0x0 if not found)
  AliMUONVCluster* cluster = 0x0;
  
  Int_t nTracks = GetNTracks();
  for (Int_t i = 0; i < nTracks; i++) {
    
    cluster = (AliMUONVCluster*) ((AliMpExMap*) fClusterMap->GetObjectFast(i))->GetValue(clusterId);
    if (cluster) break;
    
  }
  
  if (!cluster) AliWarning(Form("cluster %d does not exist",clusterId));
  return cluster;
}

//___________________________________________________________________________
AliMUONVDigit* AliMUONESDInterface::FindDigit(UInt_t digitId) const
{
  /// return digit "digitId" (0x0 if not found)
  AliMUONVDigit *digit = fDigits ? fDigits->FindObject(digitId) : 0x0;
  if (!digit) AliWarning(Form("digit %d does not exist",digitId));
  return digit;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateTrackIterator() const
{
  /// return iterator over all tracks
  return fTracks ? fTracks->CreateIterator() : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateClusterIterator() const
{
  /// return iterator over all clusters
  return fClusterMap ? new AliMUON2DMapIterator(*fClusterMap) : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateClusterIterator(Int_t iTrack) const
{
  /// return iterator over clusters of track "iTrack"
  AliMpExMap* cMap = fClusterMap ? (AliMpExMap*) fClusterMap->GetObject(iTrack) : 0x0;
  return cMap ? new AliMUON1DMapIterator(*cMap) : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateDigitIterator() const
{
  /// return iterator over all digits
  return fDigits ? fDigits->CreateIterator() : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateDigitIterator(Int_t iTrack) const
{
  /// return iterator over all digits of track "iTrack"
  AliMpExMap* dMaps = fDigitMap ? (AliMpExMap*) fDigitMap->At(iTrack) : 0x0;
  return dMaps ? new AliMUON2DMapIterator(*dMaps) : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateDigitIterator(Int_t iTrack, Int_t iCluster) const
{
  /// return iterator over digits of cluster numbered "iCluster" in track "iTrack"
  AliMpExMap* dMaps = fDigitMap ? (AliMpExMap*) fDigitMap->At(iTrack) : 0x0;
  AliMpExMap* dMap = dMaps ? (AliMpExMap*) dMaps->GetObject(iCluster) : 0x0;
  return dMap ? new AliMUON1DMapIterator(*dMap) : 0x0;
}

//___________________________________________________________________________
TIterator* AliMUONESDInterface::CreateDigitIteratorInCluster(UInt_t clusterId) const
{
  /// return iterator over digits of cluster "clusterId"
  AliMpExMap* dMap = 0x0;
  
  Int_t nTracks = GetNTracks();
  for (Int_t i = 0; i < nTracks; i++) {
    
    dMap = (AliMpExMap*) ((AliMpExMap*) fDigitMap->UncheckedAt(i))->GetValue(clusterId);
    if (dMap) break;
    
  }
  
  return dMap ? new AliMUON1DMapIterator(*dMap) : 0x0;
}

//___________________________________________________________________________
AliMUONVCluster* AliMUONESDInterface::FindClusterInTrack(const AliMUONTrack& track, UInt_t clusterId) const
{
  /// find the cluster with the given Id into the track
  
  Int_t nClusters = track.GetNClusters();
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    
    AliMUONVCluster* cluster = ((AliMUONTrackParam*) track.GetTrackParamAtCluster()->UncheckedAt(iCluster))->GetClusterPtr();
    if (cluster->GetUniqueID() == clusterId) return cluster;
    
  }
  
  return 0x0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                static methods                               //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//_____________________________________________________________________________
AliMUONVTrackStore* AliMUONESDInterface::NewTrackStore()
{
  /// Create an empty track store of type fgTrackStoreName
  TClass* classPtr = TClass::GetClass(fgTrackStoreName);
  if (!classPtr || !classPtr->InheritsFrom("AliMUONVTrackStore")) {
    cout<<"E-AliMUONESDInterface::NewTrackStore: Unable to create store of type "<<fgTrackStoreName.Data()<<endl;
    return 0x0;
  }
  return reinterpret_cast<AliMUONVTrackStore*>(classPtr->New());
}

//_____________________________________________________________________________
AliMUONVClusterStore* AliMUONESDInterface::NewClusterStore()
{
  /// Create an empty cluster store of type fgClusterStoreName
  TClass* classPtr = TClass::GetClass(fgClusterStoreName);
  if (!classPtr || !classPtr->InheritsFrom("AliMUONVClusterStore")) {
    cout<<"E-AliMUONESDInterface::NewClusterStore: Unable to create store of type "<<fgClusterStoreName.Data()<<endl;
    return 0x0;
  }
  return reinterpret_cast<AliMUONVClusterStore*>(classPtr->New());
}

//_____________________________________________________________________________
AliMUONVDigitStore* AliMUONESDInterface::NewDigitStore()
{
  /// Create an empty digit store of type fgDigitStoreName
  TClass* classPtr = TClass::GetClass(fgDigitStoreName);
  if (!classPtr || !classPtr->InheritsFrom("AliMUONVDigitStore")) {
    cout<<"E-AliMUONESDInterface::NewDigitStore: Unable to create store of type "<<fgDigitStoreName.Data()<<endl;
    return 0x0;
  }
  return reinterpret_cast<AliMUONVDigitStore*>(classPtr->New());
}

//_________________________________________________________________________
void AliMUONESDInterface::GetParamAtVertex(const AliESDMuonTrack& esdTrack, AliMUONTrackParam& trackParam)
{
  /// Get parameters at vertex from ESDMuon track
  trackParam.SetZ(esdTrack.GetZ());
  trackParam.SetNonBendingCoor(esdTrack.GetNonBendingCoor());
  trackParam.SetNonBendingSlope(TMath::Tan(esdTrack.GetThetaX()));
  trackParam.SetBendingCoor(esdTrack.GetBendingCoor());
  trackParam.SetBendingSlope(TMath::Tan(esdTrack.GetThetaY()));
  trackParam.SetInverseBendingMomentum(esdTrack.GetInverseBendingMomentum());
}

//_________________________________________________________________________
void AliMUONESDInterface::GetParamAtDCA(const AliESDMuonTrack& esdTrack, AliMUONTrackParam& trackParam)
{
  /// Get parameters at DCA from ESDMuon track
  trackParam.SetZ(esdTrack.GetZ());
  trackParam.SetNonBendingCoor(esdTrack.GetNonBendingCoorAtDCA());
  trackParam.SetNonBendingSlope(TMath::Tan(esdTrack.GetThetaXAtDCA()));
  trackParam.SetBendingCoor(esdTrack.GetBendingCoorAtDCA());
  trackParam.SetBendingSlope(TMath::Tan(esdTrack.GetThetaYAtDCA()));
  trackParam.SetInverseBendingMomentum(esdTrack.GetInverseBendingMomentumAtDCA());
}

//_________________________________________________________________________
void AliMUONESDInterface::GetParamAtFirstCluster(const AliESDMuonTrack& esdTrack, AliMUONTrackParam& trackParam)
{
  /// Get parameters at first cluster from ESDMuon track
  trackParam.SetZ(esdTrack.GetZUncorrected());
  trackParam.SetNonBendingCoor(esdTrack.GetNonBendingCoorUncorrected());
  trackParam.SetNonBendingSlope(TMath::Tan(esdTrack.GetThetaXUncorrected()));
  trackParam.SetBendingCoor(esdTrack.GetBendingCoorUncorrected());
  trackParam.SetBendingSlope(TMath::Tan(esdTrack.GetThetaYUncorrected()));
  trackParam.SetInverseBendingMomentum(esdTrack.GetInverseBendingMomentumUncorrected());
}

//_________________________________________________________________________
void AliMUONESDInterface::GetParamCov(const AliESDMuonTrack& esdTrack, AliMUONTrackParam& trackParam)
{
  /// Get parameters covariances from ESD track
  
  // get ESD covariance matrix
  TMatrixD covariances(5,5);
  esdTrack.GetCovariances(covariances);
  
  // compute Jacobian to change the coordinate system
  // from (X,thetaX,Y,thetaY,c/pYZ) to (X,slopeX,Y,slopeY,c/pYZ)
  Double_t cosThetaX = TMath::Cos(esdTrack.GetThetaXUncorrected());
  Double_t cosThetaY = TMath::Cos(esdTrack.GetThetaYUncorrected());
  TMatrixD jacob(5,5);
  jacob.Zero();
  jacob(0,0) = 1.;
  jacob(1,1) = 1. / cosThetaX / cosThetaX;
  jacob(2,2) = 1.;
  jacob(3,3) = 1. / cosThetaY / cosThetaY;
  jacob(4,4) = 1.;
  
  // compute covariance matrix in ESD coordinate system
  TMatrixD tmp(covariances,TMatrixD::kMultTranspose,jacob);
  trackParam.SetCovariances(TMatrixD(jacob,TMatrixD::kMult,tmp));
  
}

//_________________________________________________________________________
void AliMUONESDInterface::SetParamAtVertex(const AliMUONTrackParam& trackParam, AliESDMuonTrack& esdTrack)
{
  /// Set parameters in ESD track
  esdTrack.SetZ(trackParam.GetZ());
  esdTrack.SetNonBendingCoor(trackParam.GetNonBendingCoor());
  esdTrack.SetThetaX(TMath::ATan(trackParam.GetNonBendingSlope()));
  esdTrack.SetBendingCoor(trackParam.GetBendingCoor()); 
  esdTrack.SetThetaY(TMath::ATan(trackParam.GetBendingSlope()));
  esdTrack.SetInverseBendingMomentum(trackParam.GetInverseBendingMomentum());
}

//_________________________________________________________________________
void AliMUONESDInterface::SetParamAtDCA(const AliMUONTrackParam& trackParam, AliESDMuonTrack& esdTrack)
{
  /// Set parameters in ESD track
  esdTrack.SetNonBendingCoorAtDCA(trackParam.GetNonBendingCoor());
  esdTrack.SetThetaXAtDCA(TMath::ATan(trackParam.GetNonBendingSlope()));
  esdTrack.SetBendingCoorAtDCA(trackParam.GetBendingCoor()); 
  esdTrack.SetThetaYAtDCA(TMath::ATan(trackParam.GetBendingSlope()));
  esdTrack.SetInverseBendingMomentumAtDCA(trackParam.GetInverseBendingMomentum());
}

//_________________________________________________________________________
void AliMUONESDInterface::SetParamAtFirstCluster(const AliMUONTrackParam& trackParam, AliESDMuonTrack& esdTrack)
{
  /// Set parameters in ESD track
  esdTrack.SetZUncorrected(trackParam.GetZ());
  esdTrack.SetNonBendingCoorUncorrected(trackParam.GetNonBendingCoor());
  esdTrack.SetThetaXUncorrected(TMath::ATan(trackParam.GetNonBendingSlope()));
  esdTrack.SetBendingCoorUncorrected(trackParam.GetBendingCoor()); 
  esdTrack.SetThetaYUncorrected(TMath::ATan(trackParam.GetBendingSlope()));
  esdTrack.SetInverseBendingMomentumUncorrected(trackParam.GetInverseBendingMomentum());
}

//_________________________________________________________________________
void AliMUONESDInterface::SetParamCov(const AliMUONTrackParam& trackParam, AliESDMuonTrack& esdTrack)
{
  /// Set parameters covariances in ESD track
  
  // set null matrix if covariances does not exist
  if (!trackParam.CovariancesExist()) {
    TMatrixD tmp(5,5);
    tmp.Zero();
    esdTrack.SetCovariances(tmp);
    return;
  }
  
  // compute Jacobian to change the coordinate system
  // from (X,slopeX,Y,slopeY,c/pYZ) to (X,thetaX,Y,thetaY,c/pYZ)
  Double_t cosThetaX = TMath::Cos(TMath::ATan(trackParam.GetNonBendingSlope()));
  Double_t cosThetaY = TMath::Cos(TMath::ATan(trackParam.GetBendingSlope()));
  TMatrixD jacob(5,5);
  jacob.Zero();
  jacob(0,0) = 1.;
  jacob(1,1) = cosThetaX * cosThetaX;
  jacob(2,2) = 1.;
  jacob(3,3) = cosThetaY * cosThetaY;
  jacob(4,4) = 1.;
  
  // compute covariance matrix in ESD coordinate system
  TMatrixD tmp(trackParam.GetCovariances(),TMatrixD::kMultTranspose,jacob);
  esdTrack.SetCovariances(TMatrixD(jacob,TMatrixD::kMult,tmp));
  
}

//_____________________________________________________________________________
void AliMUONESDInterface::ESDToMUON(const AliESDMuonTrack& esdTrack, AliMUONTrack& track)
{
  /// Transfert data from ESDMuon track to MUON track
  
  track.Clear("C");
  
  // global info
  track.SetUniqueID(esdTrack.GetUniqueID());
  track.FitWithVertex(kFALSE);
  track.FitWithMCS(kFALSE);
  track.SetImproved(kFALSE);
  track.SetVertexErrXY2(0.,0.);
  track.SetGlobalChi2(esdTrack.GetChi2());
  track.SetMatchTrigger(esdTrack.GetMatchTrigger());
  track.SetLoTrgNum(-1);
  track.SetChi2MatchTrigger(esdTrack.GetChi2MatchTrigger());
  track.SetTrackID(0);
  track.SetHitsPatternInTrigCh(esdTrack.GetHitsPatternInTrigCh());
  track.SetLocalTrigger(esdTrack.LoCircuit(), esdTrack.LoStripX(), esdTrack.LoStripY(),
			esdTrack.LoDev(), esdTrack.LoLpt(), esdTrack.LoHpt());
  
  // track parameters at vertex
  AliMUONTrackParam paramAtVertex;
  GetParamAtVertex(esdTrack, paramAtVertex);
  track.SetTrackParamAtVertex(&paramAtVertex);
    
  // track parameters at first cluster
  AliMUONTrackParam param;
  GetParamAtFirstCluster(esdTrack, param);
  GetParamCov(esdTrack, param);
  
  // create empty cluster
  AliMUONVClusterStore* cStore = NewClusterStore();
  if (!cStore) return;
  AliMUONVCluster* cluster = cStore->CreateCluster(0,0,0);
  
  // fill TrackParamAtCluster with track parameters at each cluster if available
  // or with only track parameters at first (fake) cluster if not
  if(esdTrack.ClustersStored()) {
    
    // loop over ESD clusters
    AliESDMuonCluster *esdCluster = (AliESDMuonCluster*) esdTrack.GetClusters().First();
    while (esdCluster) {
      
      // copy cluster information
      ESDToMUON(*esdCluster, *cluster);
      
      // only set the Z parameter to avoid error in the AddTrackParamAtCluster(...) method
      param.SetZ(cluster->GetZ());
      
      // add common track parameters at current cluster
      track.AddTrackParamAtCluster(param, *cluster, kTRUE);
      
      esdCluster = (AliESDMuonCluster*) esdTrack.GetClusters().After(esdCluster);
    }
    
    // recompute parameters at first cluster in case of those stored
    // in ESD are not related to the most upstream cluster
    AliMUONTrackParam *firstTrackParam = (AliMUONTrackParam*) track.GetTrackParamAtCluster()->First();
    firstTrackParam->SetZ(esdTrack.GetZUncorrected()); // reset the z to the one stored in ESD
    AliMUONTrackExtrap::ExtrapToZCov(firstTrackParam,firstTrackParam->GetClusterPtr()->GetZ());
    
    // Compute track parameters and covariances at each cluster from those at the first one
    track.UpdateCovTrackParamAtCluster();
    
  } else {
    
    // get number of the first hit chamber (according to the MUONClusterMap if not empty)
    Int_t firstCh = 0;
    if (esdTrack.GetMuonClusterMap() != 0) while (!esdTrack.IsInMuonClusterMap(firstCh)) firstCh++;
    else firstCh = AliMUONConstants::ChamberNumber(param.GetZ());
    
    // produce fake cluster at this chamber
    cluster->SetUniqueID(AliMUONVCluster::BuildUniqueID(firstCh, 0, 0));
    cluster->SetXYZ(param.GetNonBendingCoor(), param.GetBendingCoor(), param.GetZ());
    cluster->SetErrXY(0., 0.);
    
    // add track parameters at first (fake) cluster
    track.AddTrackParamAtCluster(param, *cluster, kTRUE);
    
  }
  
  delete cluster;
  delete cStore;
  
}

//_____________________________________________________________________________
void AliMUONESDInterface::ESDToMUON(const AliESDMuonCluster& esdCluster, AliMUONVCluster& cluster)
{
  /// Transfert data from ESDMuon cluster to MUON cluster
  
  cluster.Clear("C");
  
  cluster.SetUniqueID(esdCluster.GetUniqueID());
  cluster.SetXYZ(esdCluster.GetX(), esdCluster.GetY(), esdCluster.GetZ());
  cluster.SetErrXY(esdCluster.GetErrX(),esdCluster.GetErrY());
  cluster.SetCharge(esdCluster.GetCharge());
  cluster.SetChi2(esdCluster.GetChi2());
  
  if (esdCluster.PadsStored()) {
    Int_t nPads = esdCluster.GetNPads();
    for (Int_t iPad = 0; iPad < nPads; iPad++)
      cluster.AddDigitId(((AliESDMuonPad*)esdCluster.GetPads().UncheckedAt(iPad))->GetUniqueID());
  }
  
}

//___________________________________________________________________________
void AliMUONESDInterface::ESDToMUON(const AliESDMuonPad& esdPad, AliMUONVDigit& digit)
{
  /// Transfert data from ESDMuon pad to MUON digit
  
  const AliMpVSegmentation* seg = AliMpSegmentation::Instance()->GetMpSegmentationByElectronics(esdPad.GetDetElemId(), esdPad.GetManuId());  
  AliMpPad pad = seg->PadByLocation(AliMpIntPair(esdPad.GetManuId(), esdPad.GetManuChannel()), kFALSE);
  
  digit.Saturated(kFALSE);
  digit.Used(kFALSE);
  digit.Calibrated(kTRUE);
  digit.SetUniqueID(esdPad.GetUniqueID());
  digit.SetCharge(esdPad.GetCharge());
  digit.SetADC(esdPad.GetADC());
  digit.SetPadXY(pad.GetIndices().GetFirst(), pad.GetIndices().GetSecond());
  
}

//_____________________________________________________________________________
void AliMUONESDInterface::MUONToESD(const AliMUONTrack& track, AliESDMuonTrack& esdTrack, const Double_t vertex[3], const AliMUONVDigitStore* digits)
{
  /// Transfert data from MUON track to ESDMuon track
  /// Incorporate the ESDPads if the digits are provided
  
  esdTrack.Clear("C");
  
  // set param at first cluster
  AliMUONTrackParam* trackParam = static_cast<AliMUONTrackParam*>((track.GetTrackParamAtCluster())->First());
  SetParamAtFirstCluster(*trackParam, esdTrack);
  SetParamCov(*trackParam, esdTrack);
  
  // set param at vertex
  AliMUONTrackParam trackParamAtVtx(*trackParam);
  AliMUONTrackExtrap::ExtrapToVertex(&trackParamAtVtx, vertex[0], vertex[1], vertex[2], 0., 0.);
  SetParamAtVertex(trackParamAtVtx, esdTrack);
  
  // set param at Distance of Closest Approach
  AliMUONTrackParam trackParamAtDCA(*trackParam);
  AliMUONTrackExtrap::ExtrapToVertexWithoutBranson(&trackParamAtDCA, vertex[2]);
  SetParamAtDCA(trackParamAtDCA, esdTrack);
  
  // set global info
  esdTrack.SetUniqueID(track.GetUniqueID());
  esdTrack.SetChi2(track.GetGlobalChi2());
  esdTrack.SetNHit(track.GetNClusters());
  esdTrack.SetLocalTrigger(track.GetLocalTrigger());
  esdTrack.SetChi2MatchTrigger(track.GetChi2MatchTrigger());
  esdTrack.SetHitsPatternInTrigCh(track.GetHitsPatternInTrigCh());
  
  // set muon cluster info
  AliESDMuonCluster esdCluster;
  esdTrack.SetMuonClusterMap(0);
  while (trackParam) {
    MUONToESD(*(trackParam->GetClusterPtr()), esdCluster, digits);
    esdTrack.AddCluster(esdCluster);
    esdTrack.AddInMuonClusterMap(esdCluster.GetChamberId());
    trackParam = static_cast<AliMUONTrackParam*>(track.GetTrackParamAtCluster()->After(trackParam));
  }
  
}

//_____________________________________________________________________________
void AliMUONESDInterface::MUONToESD(const AliMUONVCluster& cluster, AliESDMuonCluster& esdCluster, const AliMUONVDigitStore* digits)
{
  /// Transfert data from MUON cluster to ESDMuon cluster
  /// Incorporate the ESDPads if the digits are provided
  
  esdCluster.Clear("C");
  
  esdCluster.SetUniqueID(cluster.GetUniqueID());
  esdCluster.SetXYZ(cluster.GetX(), cluster.GetY(), cluster.GetZ());
  esdCluster.SetErrXY(cluster.GetErrX(), cluster.GetErrY());
  
  if (digits) { // transfert all data...
    
    esdCluster.SetCharge(cluster.GetCharge());
    esdCluster.SetChi2(cluster.GetChi2());
    AliESDMuonPad esdPad;
    for (Int_t i=0; i<cluster.GetNDigits(); i++) {
      AliMUONVDigit* digit = digits->FindObject(cluster.GetDigitId(i));
      if (!digit) {
	cout<<"E-AliMUONESDInterface::MUONToESD: digit "<<cluster.GetDigitId(i)<<" not found"<<endl;
	continue;
      }
      MUONToESD(*digit, esdPad);
      esdCluster.AddPad(esdPad);
    }
    
  } else { // ...or not
    
    esdCluster.SetCharge(0.);
    esdCluster.SetChi2(0.);
    
  }
  
}

//_____________________________________________________________________________
void AliMUONESDInterface::MUONToESD(const AliMUONVDigit& digit, AliESDMuonPad& esdPad)
{
  /// Transfert data from MUON digit to ESDMuon pad
  esdPad.SetUniqueID(digit.GetUniqueID());
  esdPad.SetADC(digit.ADC());
  esdPad.SetCharge(digit.Charge());
}

//___________________________________________________________________________
AliMUONTrack* AliMUONESDInterface::Add(const AliESDMuonTrack& esdTrack, AliMUONVTrackStore& trackStore)
{
  /// Create MUON track from ESDMuon track and add it to the store
  /// return a pointer to the track into the store
  AliMUONTrack* track = trackStore.Add(AliMUONTrack());
  ESDToMUON(esdTrack, *track);
  return track;
}

//___________________________________________________________________________
AliMUONVCluster* AliMUONESDInterface::Add(const AliESDMuonCluster& esdCluster, AliMUONVClusterStore& clusterStore)
{
  /// Create MUON cluster from ESDMuon cluster and add it to the store
  /// return a pointer to the cluster into the store (0x0 if the cluster already exist)
  AliMUONVCluster* cluster = clusterStore.Add(esdCluster.GetChamberId(), esdCluster.GetDetElemId(), esdCluster.GetClusterIndex());
  if (cluster) ESDToMUON(esdCluster, *cluster);
  return cluster;
}

//___________________________________________________________________________
AliMUONVDigit* AliMUONESDInterface::Add(const AliESDMuonPad& esdPad, AliMUONVDigitStore& digitStore)
{
  /// Create MUON digit from ESDMuon digit and add it to the store
  /// return a pointer to the digit into the store (0x0 if the digit already exist)
  AliMUONVDigit* digit = digitStore.Add(esdPad.GetDetElemId(), esdPad.GetManuId(), esdPad.GetManuChannel(), esdPad.GetCathode(), AliMUONVDigitStore::kDeny);
  if (digit) ESDToMUON(esdPad, *digit);
  return digit;
}


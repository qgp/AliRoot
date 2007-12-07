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

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//
//  Class to describe the MUON tracks
//  in the Event Summary Data class
//  This is where the results of reconstruction
//  are stored for the muons
//  Author: G.Martinez
//
///////////////////////////////////////////////////////////////////////////////

#include "AliESDMuonTrack.h"
#include "AliESDMuonCluster.h"

#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TMath.h>

ClassImp(AliESDMuonTrack)

//_____________________________________________________________________________
AliESDMuonTrack::AliESDMuonTrack ():
  AliVParticle(),
  fInverseBendingMomentum(0),
  fThetaX(0),
  fThetaY(0),
  fZ(0),
  fBendingCoor(0),
  fNonBendingCoor(0),
  fInverseBendingMomentumAtDCA(0),
  fThetaXAtDCA(0),
  fThetaYAtDCA(0),
  fBendingCoorAtDCA(0),
  fNonBendingCoorAtDCA(0),
  fInverseBendingMomentumUncorrected(0),
  fThetaXUncorrected(0),
  fThetaYUncorrected(0),
  fZUncorrected(0),
  fBendingCoorUncorrected(0),
  fNonBendingCoorUncorrected(0),
  fChi2(0),
  fChi2MatchTrigger(0),
  fLocalTrigger(0),
  fMuonClusterMap(0),
  fHitsPatternInTrigCh(0),
  fNHit(0),
  fClusters(0x0)
{
  //
  // Default constructor
  //
  for (Int_t i = 0; i < 15; i++) fCovariances[i] = 0;
}


//_____________________________________________________________________________
AliESDMuonTrack::AliESDMuonTrack (const AliESDMuonTrack& muonTrack):
  AliVParticle(muonTrack),
  fInverseBendingMomentum(muonTrack.fInverseBendingMomentum),
  fThetaX(muonTrack.fThetaX),
  fThetaY(muonTrack.fThetaY),
  fZ(muonTrack.fZ),
  fBendingCoor(muonTrack.fBendingCoor),
  fNonBendingCoor(muonTrack.fNonBendingCoor),
  fInverseBendingMomentumAtDCA(muonTrack.fInverseBendingMomentumAtDCA),
  fThetaXAtDCA(muonTrack.fThetaXAtDCA),
  fThetaYAtDCA(muonTrack.fThetaYAtDCA),
  fBendingCoorAtDCA(muonTrack.fBendingCoorAtDCA),
  fNonBendingCoorAtDCA(muonTrack.fNonBendingCoorAtDCA),
  fInverseBendingMomentumUncorrected(muonTrack.fInverseBendingMomentumUncorrected),
  fThetaXUncorrected(muonTrack.fThetaXUncorrected),
  fThetaYUncorrected(muonTrack.fThetaYUncorrected),
  fZUncorrected(muonTrack.fZUncorrected),
  fBendingCoorUncorrected(muonTrack.fBendingCoorUncorrected),
  fNonBendingCoorUncorrected(muonTrack.fNonBendingCoorUncorrected),
  fChi2(muonTrack.fChi2),
  fChi2MatchTrigger(muonTrack.fChi2MatchTrigger),
  fLocalTrigger(muonTrack.fLocalTrigger),
  fMuonClusterMap(muonTrack.fMuonClusterMap),
  fHitsPatternInTrigCh(muonTrack.fHitsPatternInTrigCh),
  fNHit(muonTrack.fNHit),
  fClusters(0x0)
{
  //
  // Copy constructor
  // Deep copy implemented
  //
  for (Int_t i = 0; i < 15; i++) fCovariances[i] = muonTrack.fCovariances[i];
  
  // necessary to make a copy of the objects and not only the pointers in TClonesArray
  if (muonTrack.fClusters) {
    fClusters = new TClonesArray("AliESDMuonCluster",muonTrack.fClusters->GetEntriesFast());
    AliESDMuonCluster *cluster = (AliESDMuonCluster*) muonTrack.fClusters->First();
    while (cluster) {
      new ((*fClusters)[fClusters->GetEntriesFast()]) AliESDMuonCluster(*cluster);
      cluster = (AliESDMuonCluster*) muonTrack.fClusters->After(cluster);
    }
  }
}

//_____________________________________________________________________________
AliESDMuonTrack& AliESDMuonTrack::operator=(const AliESDMuonTrack& muonTrack)
{
  // 
  // Equal operator for a deep copy
  //
  if (this == &muonTrack)
    return *this;

  AliVParticle::operator=(muonTrack); // don't forget to invoke the base class' assignment operator
  
  fInverseBendingMomentum = muonTrack.fInverseBendingMomentum; 
  fThetaX                 = muonTrack.fThetaX;           
  fThetaY                 = muonTrack.fThetaY;           
  fZ                      = muonTrack.fZ;                
  fBendingCoor            = muonTrack.fBendingCoor;      
  fNonBendingCoor         = muonTrack.fNonBendingCoor;   
  
  fInverseBendingMomentumAtDCA = muonTrack.fInverseBendingMomentumAtDCA; 
  fThetaXAtDCA                 = muonTrack.fThetaXAtDCA;           
  fThetaYAtDCA                 = muonTrack.fThetaYAtDCA;           
  fBendingCoorAtDCA            = muonTrack.fBendingCoorAtDCA;      
  fNonBendingCoorAtDCA         = muonTrack.fNonBendingCoorAtDCA;   
  
  fInverseBendingMomentumUncorrected = muonTrack.fInverseBendingMomentumUncorrected; 
  fThetaXUncorrected                 = muonTrack.fThetaXUncorrected;           
  fThetaYUncorrected                 = muonTrack.fThetaYUncorrected;           
  fZUncorrected                      = muonTrack.fZUncorrected;                
  fBendingCoorUncorrected            = muonTrack.fBendingCoorUncorrected;      
  fNonBendingCoorUncorrected         = muonTrack.fNonBendingCoorUncorrected;   
  
  for (Int_t i = 0; i < 15; i++) fCovariances[i] = muonTrack.fCovariances[i];
  
  fChi2                   = muonTrack.fChi2;             
  fNHit                   = muonTrack.fNHit; 

  fLocalTrigger           = muonTrack.fLocalTrigger;  
  fChi2MatchTrigger       = muonTrack.fChi2MatchTrigger; 

  fHitsPatternInTrigCh    = muonTrack.fHitsPatternInTrigCh;
 
  fMuonClusterMap	  = muonTrack.fMuonClusterMap;
  
  // necessary to make a copy of the objects and not only the pointers in TClonesArray
  delete fClusters;
  if (muonTrack.fClusters) {
    fClusters = new TClonesArray("AliESDMuonCluster",muonTrack.fClusters->GetEntriesFast());
    AliESDMuonCluster *cluster = (AliESDMuonCluster*) muonTrack.fClusters->First();
    while (cluster) {
      new ((*fClusters)[fClusters->GetEntriesFast()]) AliESDMuonCluster(*cluster);
      cluster = (AliESDMuonCluster*) muonTrack.fClusters->After(cluster);
    }
  } else fClusters = 0x0;
  
  return *this;
}

//__________________________________________________________________________
AliESDMuonTrack::~AliESDMuonTrack()
{
  /// Destructor
  delete fClusters;
}

//__________________________________________________________________________
void AliESDMuonTrack::Clear(Option_t* opt)
{
  /// Clear arrays
  if (fClusters) fClusters->Clear(opt);
}

//_____________________________________________________________________________
void AliESDMuonTrack::GetCovariances(TMatrixD& cov) const
{
  // return covariance matrix of uncorrected parameters
  cov.ResizeTo(5,5);
  for (Int_t i = 0; i < 5; i++)
    for (Int_t j = 0; j <= i; j++)
      cov(i,j) = cov (j,i) = fCovariances[i*(i+1)/2 + j];
}

//_____________________________________________________________________________
void AliESDMuonTrack::SetCovariances(const TMatrixD& cov)
{
  // set reduced covariance matrix of uncorrected parameters
  for (Int_t i = 0; i < 5; i++)
    for (Int_t j = 0; j <= i; j++)
      fCovariances[i*(i+1)/2 + j] = cov(i,j);

}

//_____________________________________________________________________________
void AliESDMuonTrack::GetCovarianceXYZPxPyPz(Double_t cov[21]) const
{
  // return reduced covariance matrix of uncorrected parameters in (X,Y,Z,Px,Py,Pz) coordinate system
  // 
  // Cov(x,x) ... :   cov[0]
  // Cov(y,x) ... :   cov[1]  cov[2]
  // Cov(z,x) ... :   cov[3]  cov[4]  cov[5]
  // Cov(px,x)... :   cov[6]  cov[7]  cov[8]  cov[9]
  // Cov(py,x)... :   cov[10] cov[11] cov[12] cov[13] cov[14]
  // Cov(pz,x)... :   cov[15] cov[16] cov[17] cov[18] cov[19] cov[20]
  //
  // Get ESD covariance matrix into a TMatrixD
  TMatrixD covESD(5,5);
  GetCovariances(covESD);

  // compute Jacobian to change the coordinate system
  // from (X,thetaX,Y,thetaY,c/pYZ) to (X,Y,Z,pX,pY,pZ)
  Double_t tanThetaX = TMath::Tan(fThetaXUncorrected);
  Double_t tanThetaY = TMath::Tan(fThetaYUncorrected);
  Double_t cosThetaX2 = TMath::Cos(fThetaXUncorrected) * TMath::Cos(fThetaXUncorrected);
  Double_t cosThetaY2 = TMath::Cos(fThetaYUncorrected) * TMath::Cos(fThetaYUncorrected);
  Double_t pZ = PzUncorrected();
  Double_t dpZdthetaY = - fInverseBendingMomentumUncorrected * fInverseBendingMomentumUncorrected *
			  pZ * pZ * pZ * tanThetaY / cosThetaY2;
  Double_t dpZdinvpYZ = (fInverseBendingMomentumUncorrected != 0.) ? - pZ / fInverseBendingMomentumUncorrected : - FLT_MAX;
  TMatrixD jacob(6,5);
  jacob.Zero();
  jacob(0,0) = 1.;
  jacob(1,2) = 1.;
  jacob(3,1) = pZ / cosThetaX2;
  jacob(3,3) = dpZdthetaY * tanThetaX;
  jacob(3,4) = dpZdinvpYZ * tanThetaX;
  jacob(4,3) = dpZdthetaY * tanThetaY + pZ / cosThetaY2;
  jacob(4,4) = dpZdinvpYZ * tanThetaY;
  jacob(5,3) = dpZdthetaY;
  jacob(5,4) = dpZdinvpYZ;
  
  // compute covariance matrix in AOD coordinate system
  TMatrixD tmp(covESD,TMatrixD::kMultTranspose,jacob);
  TMatrixD covAOD(jacob,TMatrixD::kMult,tmp);
  
  // Get AOD covariance matrix into co[21]
  for (Int_t i = 0; i < 6; i++)
    for (Int_t j = 0; j <= i; j++)
      cov[i*(i+1)/2 + j] = covAOD(i,j);
  
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::Px() const
{
  // return p_x from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaX);
  Double_t bendingSlope    = TMath::Tan(fThetaY);
  Double_t pYZ = (fInverseBendingMomentum != 0.) ? TMath::Abs(1. / fInverseBendingMomentum) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * nonBendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::Py() const
{
  // return p_y from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaY);
  Double_t pYZ = (fInverseBendingMomentum != 0.) ? TMath::Abs(1. / fInverseBendingMomentum) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * bendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::Pz() const
{
  // return p_z from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaY);
  Double_t pYZ = (fInverseBendingMomentum != 0.) ? TMath::Abs(1. / fInverseBendingMomentum) : - FLT_MAX;
  return -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::P() const
{
  // return p from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaX);
  Double_t bendingSlope    = TMath::Tan(fThetaY);
  Double_t pYZ = (fInverseBendingMomentum != 0.) ? TMath::Abs(1. / fInverseBendingMomentum) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return -pZ * TMath::Sqrt(1.0 + bendingSlope*bendingSlope + nonBendingSlope*nonBendingSlope);
}

//_____________________________________________________________________________
void AliESDMuonTrack::LorentzP(TLorentzVector& vP) const
{
  // return Lorentz momentum vector from track parameters
  Double_t muonMass = M();
  Double_t nonBendingSlope = TMath::Tan(fThetaX);
  Double_t bendingSlope    = TMath::Tan(fThetaY);
  Double_t pYZ = (fInverseBendingMomentum != 0.) ? TMath::Abs(1. / fInverseBendingMomentum) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  Double_t pX  = pZ * nonBendingSlope;
  Double_t pY  = pZ * bendingSlope;
  Double_t e   = TMath::Sqrt(muonMass*muonMass + pX*pX + pY*pY + pZ*pZ);
  vP.SetPxPyPzE(pX, pY, pZ, e);
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PxAtDCA() const
{
  // return p_x from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaXAtDCA);
  Double_t bendingSlope    = TMath::Tan(fThetaYAtDCA);
  Double_t pYZ = (fInverseBendingMomentumAtDCA != 0.) ? TMath::Abs(1. / fInverseBendingMomentumAtDCA) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * nonBendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PyAtDCA() const
{
  // return p_y from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaYAtDCA);
  Double_t pYZ = (fInverseBendingMomentumAtDCA != 0.) ? TMath::Abs(1. / fInverseBendingMomentumAtDCA) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * bendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PzAtDCA() const
{
  // return p_z from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaYAtDCA);
  Double_t pYZ = (fInverseBendingMomentumAtDCA != 0.) ? TMath::Abs(1. / fInverseBendingMomentumAtDCA) : - FLT_MAX;
  return -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PAtDCA() const
{
  // return p from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaXAtDCA);
  Double_t bendingSlope    = TMath::Tan(fThetaYAtDCA);
  Double_t pYZ = (fInverseBendingMomentumAtDCA != 0.) ? TMath::Abs(1. / fInverseBendingMomentumAtDCA) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return -pZ * TMath::Sqrt(1.0 + bendingSlope*bendingSlope + nonBendingSlope*nonBendingSlope);
}

//_____________________________________________________________________________
void AliESDMuonTrack::LorentzPAtDCA(TLorentzVector& vP) const
{
  // return Lorentz momentum vector from track parameters
  Double_t muonMass = M();
  Double_t nonBendingSlope = TMath::Tan(fThetaXAtDCA);
  Double_t bendingSlope    = TMath::Tan(fThetaYAtDCA);
  Double_t pYZ = (fInverseBendingMomentumAtDCA != 0.) ? TMath::Abs(1. / fInverseBendingMomentumAtDCA) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  Double_t pX  = pZ * nonBendingSlope;
  Double_t pY  = pZ * bendingSlope;
  Double_t e   = TMath::Sqrt(muonMass*muonMass + pX*pX + pY*pY + pZ*pZ);
  vP.SetPxPyPzE(pX, pY, pZ, e);
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PxUncorrected() const
{
  // return p_x from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaXUncorrected);
  Double_t bendingSlope    = TMath::Tan(fThetaYUncorrected);
  Double_t pYZ = (fInverseBendingMomentumUncorrected != 0.) ? TMath::Abs(1. / fInverseBendingMomentumUncorrected) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * nonBendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PyUncorrected() const
{
  // return p_y from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaYUncorrected);
  Double_t pYZ = (fInverseBendingMomentumUncorrected != 0.) ? TMath::Abs(1. / fInverseBendingMomentumUncorrected) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return pZ * bendingSlope;
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PzUncorrected() const
{
  // return p_z from track parameters
  Double_t bendingSlope = TMath::Tan(fThetaYUncorrected);
  Double_t pYZ = (fInverseBendingMomentumUncorrected != 0.) ? TMath::Abs(1. / fInverseBendingMomentumUncorrected) : - FLT_MAX;
  return -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
}

//_____________________________________________________________________________
Double_t AliESDMuonTrack::PUncorrected() const
{
  // return p from track parameters
  Double_t nonBendingSlope = TMath::Tan(fThetaXUncorrected);
  Double_t bendingSlope    = TMath::Tan(fThetaYUncorrected);
  Double_t pYZ = (fInverseBendingMomentumUncorrected != 0.) ? TMath::Abs(1. / fInverseBendingMomentumUncorrected) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  return -pZ * TMath::Sqrt(1.0 + bendingSlope*bendingSlope + nonBendingSlope*nonBendingSlope);
}

//_____________________________________________________________________________
void AliESDMuonTrack::LorentzPUncorrected(TLorentzVector& vP) const
{
  // return Lorentz momentum vector from track parameters
  Double_t muonMass = M();
  Double_t nonBendingSlope = TMath::Tan(fThetaXUncorrected);
  Double_t bendingSlope    = TMath::Tan(fThetaYUncorrected);
  Double_t pYZ = (fInverseBendingMomentumUncorrected != 0.) ? TMath::Abs(1. / fInverseBendingMomentumUncorrected) : - FLT_MAX;
  Double_t pZ  = -pYZ / TMath::Sqrt(1.0 + bendingSlope*bendingSlope);  // spectro. (z<0)
  Double_t pX  = pZ * nonBendingSlope;
  Double_t pY  = pZ * bendingSlope;
  Double_t e   = TMath::Sqrt(muonMass*muonMass + pX*pX + pY*pY + pZ*pZ);
  vP.SetPxPyPzE(pX, pY, pZ, e);
}

//_____________________________________________________________________________
Int_t AliESDMuonTrack::GetMatchTrigger() const
{
  //  backward compatibility after replacing fMatchTrigger by fLocalTrigger
  //  0 track does not match trigger
  //  1 track match but does not pass pt cut
  //  2 track match Low pt cut
  //  3 track match High pt cut

  if (!LoCircuit()) {
    return 0;
  } else if (LoLpt() == 0 && LoHpt() == 0) {
    return 1;
  } else if (LoLpt() >  0 && LoHpt() == 0) {
    return 2;
  } else {
    return 3;
  }

}

//_____________________________________________________________________________
void AliESDMuonTrack::AddInMuonClusterMap(Int_t chamber)
{
  // Update the muon cluster map by adding this chamber(0..)
  
  static const UInt_t kMask[10] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200};
  
  fMuonClusterMap |= kMask[chamber];
  
}

//_____________________________________________________________________________
Bool_t AliESDMuonTrack::IsInMuonClusterMap(Int_t chamber) const
{
  // return kTRUE if this chamber(0..) is in the muon cluster map
  
  static const UInt_t kMask[10] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200};
  
  return ((fMuonClusterMap | kMask[chamber]) == fMuonClusterMap) ? kTRUE : kFALSE;
  
}

//_____________________________________________________________________________
Int_t AliESDMuonTrack::GetNClusters() const
{
  // return the number of clusters associated to the track
  if (!fClusters) return 0;
  
  return fClusters->GetEntriesFast();
}

//_____________________________________________________________________________
TClonesArray& AliESDMuonTrack::GetClusters() const
{
  // return the array of clusters associated to the track
  if (!fClusters) fClusters = new TClonesArray("AliESDMuonCluster",10);
  
  return *fClusters;
}

//_____________________________________________________________________________
void AliESDMuonTrack::AddCluster(const AliESDMuonCluster &cluster)
{
  // add a cluster to the TClonesArray of clusters associated to the track
  if (!fClusters) fClusters = new TClonesArray("AliESDMuonCluster",10);
  
  new ((*fClusters)[fClusters->GetEntriesFast()]) AliESDMuonCluster(cluster);
}

//_____________________________________________________________________________
Bool_t AliESDMuonTrack::ClustersStored() const
{
  // return kTRUE if the clusters associated to the track are registered
  if (GetNClusters() == 0) return kFALSE;
  
  return kTRUE;
}


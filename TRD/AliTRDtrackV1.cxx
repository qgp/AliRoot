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

#include "AliESDtrack.h"
#include "AliTracker.h"

#include "AliTRDtrackV1.h"
#include "AliTRDcluster.h"
#include "AliTRDcalibDB.h"
#include "AliTRDReconstructor.h"
#include "AliTRDrecoParam.h"

ClassImp(AliTRDtrackV1)

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Represents a reconstructed TRD track                                     //
//  Local TRD Kalman track                                                   //
//                                                                           //
//  Authors:                                                                 //
//    Alex Bercuci <A.Bercuci@gsi.de>                                        //
//    Markus Fasel <M.Fasel@gsi.de>                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//_______________________________________________________________
AliTRDtrackV1::AliTRDtrackV1() : AliKalmanTrack()
  ,fPIDquality(0)
  ,fDE(0.)
  ,fBackupTrack(0x0)
{
  //
  // Default constructor
  //
  for(int i =0; i<3; i++) fBudget[i] = 0.;
  
  Float_t pid = 1./AliPID::kSPECIES;
  for(int is =0; is<AliPID::kSPECIES; is++) fPID[is] = pid;

  for(int ip=0; ip<kNplane; ip++){
    fTrackletIndex[ip] = 0xffff;
    fTracklet[ip]      = 0x0;
  }
}

//_______________________________________________________________
AliTRDtrackV1::AliTRDtrackV1(const AliTRDtrackV1 &ref) : AliKalmanTrack(ref)
  ,fPIDquality(ref.fPIDquality)
  ,fDE(ref.fDE)
  ,fBackupTrack(0x0)
{
  //
  // Copy constructor
  //

  for(int ip=0; ip<kNplane; ip++){ 
    fTrackletIndex[ip] = ref.fTrackletIndex[ip];
    fTracklet[ip]      = ref.fTracklet[ip];
  }

  for (Int_t i = 0; i < 3;i++) fBudget[i]      = ref.fBudget[i];

	for(Int_t is = 0; is<AliPID::kSPECIES; is++) fPID[is] = ref.fPID[is];
  
  AliKalmanTrack::SetNumberOfClusters(ref.GetNumberOfClusters());
}

//_______________________________________________________________
AliTRDtrackV1::AliTRDtrackV1(const AliESDtrack &t) : AliKalmanTrack()
  ,fPIDquality(0)
  ,fDE(0.)
  ,fBackupTrack(0x0)
{
  //
  // Constructor from AliESDtrack
  //

  SetLabel(t.GetLabel());
  SetChi2(0.0);
  SetMass(t.GetMass());
  AliKalmanTrack::SetNumberOfClusters(t.GetTRDncls()); 
  Int_t ti[kNplane]; t.GetTRDtracklets(&ti[0]);
  for(int ip=0; ip<kNplane; ip++){ 
    fTrackletIndex[ip] = ti[ip] < 0 ? 0xffff : ti[ip];
    fTracklet[ip]      = 0x0;
  }
  for(int i =0; i<3; i++) fBudget[i] = 0.;
  
  Float_t pid = 1./AliPID::kSPECIES;
  for(int is =0; is<AliPID::kSPECIES; is++) fPID[is] = pid;

  const AliExternalTrackParam *par = &t;
  if (t.GetStatus() & AliESDtrack::kTRDbackup) { 
    par = t.GetOuterParam();
    if (!par) {
      AliError("No backup info!"); 
      par = &t;
    }
  }
  Set(par->GetX() 
     ,par->GetAlpha()
     ,par->GetParameter()
     ,par->GetCovariance());

  if(t.GetStatus() & AliESDtrack::kTIME) {
    StartTimeIntegral();
    Double_t times[10]; 
    t.GetIntegratedTimes(times); 
    SetIntegratedTimes(times);
    SetIntegratedLength(t.GetIntegratedLength());
  }
}

//_______________________________________________________________
AliTRDtrackV1::AliTRDtrackV1(AliTRDseedV1 *trklts, const Double_t p[5], const Double_t cov[15]
             , Double_t x, Double_t alpha) : AliKalmanTrack()
  ,fPIDquality(0)
  ,fDE(0.)
  ,fBackupTrack(0x0)
{
  //
  // The stand alone tracking constructor
  // TEMPORARY !!!!!!!!!!!
  // to check :
  // 1. covariance matrix
  // 2. dQdl calculation
  //

  Double_t cnv = GetBz() < 1.e-5 ? 1.e5 : 1.0 / (GetBz() * kB2C);
  //  Double_t cnv   = 1.0 / (GetBz() * kB2C);

  Double_t pp[5] = { p[0]    
                   , p[1]
                   , x*p[4] - p[2]
                   , p[3]
                   , p[4]*cnv      };

  Double_t c22 = x*x*cov[14] - 2*x*cov[12] + cov[ 5];
  Double_t c32 =   x*cov[13] -     cov[ 8];
  Double_t c20 =   x*cov[10] -     cov[ 3];
  Double_t c21 =   x*cov[11] -     cov[ 4];
  Double_t c42 =   x*cov[14] -     cov[12];

  Double_t cc[15] = { cov[ 0]
                    , cov[ 1],     cov[ 2]
                    , c20,         c21,         c22
                    , cov[ 6],     cov[ 7],     c32,     cov[ 9]
                    , cov[10]*cnv, cov[11]*cnv, c42*cnv, cov[13]*cnv, cov[14]*cnv*cnv };

  Double_t mostProbablePt=AliExternalTrackParam::GetMostProbablePt();
  Double_t p0=TMath::Sign(1/mostProbablePt,pp[4]);
  Double_t w0=cc[14]/(cc[14] + p0*p0), w1=p0*p0/(cc[14] + p0*p0);
  pp[4] = w0*p0 + w1*pp[4];
  cc[10]*=w1; cc[11]*=w1; cc[12]*=w1; cc[13]*=w1; cc[14]*=w1;

	Set(x,alpha,pp,cc);
  Int_t ncls = 0;
	for(int iplane=0; iplane<kNplane; iplane++){
    fTrackletIndex[iplane] = 0xffff;
		if(!trklts[iplane].IsOK()) fTracklet[iplane] = 0x0;
    else{ 
      fTracklet[iplane] = new AliTRDseedV1(trklts[iplane]);
      ncls += fTracklet[iplane]->GetN();
    }
	}
  AliKalmanTrack::SetNumberOfClusters(ncls);		
  for(int i =0; i<3; i++) fBudget[i] = 0.;
  
  Float_t pid = 1./AliPID::kSPECIES;
  for(int is =0; is<AliPID::kSPECIES; is++) fPID[is] = pid;

}

//_______________________________________________________________
AliTRDtrackV1::~AliTRDtrackV1()
{
  if(fBackupTrack) {
    delete fBackupTrack;
  }
  fBackupTrack = 0x0;

  if(TestBit(1)){
    for(Int_t ip=0; ip<kNplane; ip++){
      if(fTracklet[ip]) delete fTracklet[ip];
      fTracklet[ip] = 0x0;
    }
  }
}
	
//_______________________________________________________________
Bool_t AliTRDtrackV1::CookLabel(Float_t wrong)
{
  // set MC label for this track
  
  Int_t s[kMAXCLUSTERSPERTRACK][2];
  for (Int_t i = 0; i < kMAXCLUSTERSPERTRACK; i++) {
    s[i][0] = -1;
    s[i][1] =  0;
  }
  
  Bool_t labelAdded;
  Int_t label;
  AliTRDcluster *c    = 0x0;
  for (Int_t ip = 0; ip < kNplane; ip++) {
    if(fTrackletIndex[ip] == 0xffff) continue;
    for (Int_t ic = 0; ic < AliTRDseed::knTimebins; ic++) {
      if(!(c = fTracklet[ip]->GetClusters(ic))) continue;
      for (Int_t k = 0; k < 3; k++) { 
        label      = c->GetLabel(k);
        labelAdded = kFALSE; 
        Int_t j = 0;
        if (label >= 0) {
          while ((!labelAdded) && (j < kMAXCLUSTERSPERTRACK)) {
            if ((s[j][0] == label) || 
                (s[j][1] ==     0)) {
              s[j][0] = label; 
              s[j][1]++; 
              labelAdded = kTRUE;
            }
            j++;
          }
        }
      }
    }
  }
  
  Int_t max = 0;
  label = -123456789;
  for (Int_t i = 0; i < kMAXCLUSTERSPERTRACK; i++) {
    if (s[i][1] <= max) continue;
    max   = s[i][1]; 
    label = s[i][0];
  }
  
  if ((1. - Float_t(max)/GetNumberOfClusters()) > wrong) label = -label;
  
  SetLabel(label); 
  
  return kTRUE;
}

//_______________________________________________________________
Bool_t AliTRDtrackV1::CookPID()
{
  //
  // Cook the PID information
  //
  
  // Reset the a priori probabilities
  Double_t pid = 1. / AliPID::kSPECIES;
  for(int ispec=0; ispec<AliPID::kSPECIES; ispec++) {
    fPID[ispec] = pid;	
  }
  fPIDquality = 0;
  
  // steer PID calculation @ tracklet level
  Double_t *prob = 0x0;
  for(int ip=0; ip<kNplane; ip++){
    if(fTrackletIndex[ip] == 0xffff) continue;
    if(!fTracklet[ip]->IsOK()) continue;
    if(!(prob = fTracklet[ip]->GetProbability())) return kFALSE;
    
    Int_t nspec = 0; // quality check of tracklet dEdx
    for(int ispec=0; ispec<AliPID::kSPECIES; ispec++){
      if(prob[ispec] < 0.) continue;
      fPID[ispec] *= prob[ispec];
      nspec++;
    }
    if(!nspec) continue;
    
    fPIDquality++;
  }
  
  // no tracklet found for PID calculations
  if(!fPIDquality) return kTRUE;
  
  // slot for PID calculation @ track level
  
  
  // normalize probabilities
  Double_t probTotal = 0.0;
  for (Int_t is = 0; is < AliPID::kSPECIES; is++) probTotal += fPID[is];
  
  
  if (probTotal <= 0.0) {
    AliWarning("The total probability over all species <= 0. This may be caused by some error in the reference data.");
    return kFALSE;
  }
  
  for (Int_t iSpecies = 0; iSpecies < AliPID::kSPECIES; iSpecies++) fPID[iSpecies] /= probTotal;
  
  return kTRUE;
}

//_____________________________________________________________________________
Double_t AliTRDtrackV1::GetBz() const 
{
  //
  // Returns Bz component of the magnetic field (kG)
  //

  if (AliTracker::UniformField()) return AliTracker::GetBz();

  Double_t r[3]; 
  GetXYZ(r);
  return AliTracker::GetBz(r);
}

//_______________________________________________________________
Int_t  AliTRDtrackV1::GetClusterIndex(Int_t id) const
{
  Int_t n = 0;
  for(Int_t ip=0; ip<kNplane; ip++){
    if(!fTracklet[ip]) continue;
    if(n+fTracklet[ip]->GetN() < id){ 
      n+=fTracklet[ip]->GetN();
      continue;
    }
    for(Int_t ic=34; ic>=0; ic--){
      if(!fTracklet[ip]->GetClusters(ic)) continue;
      n++;
      if(n<id) continue;
      return fTracklet[ip]->GetIndexes(ic);
    }
  }
  return -1;
}

//_______________________________________________________________
Double_t AliTRDtrackV1::GetPredictedChi2(const AliTRDseedV1 *trklt) const
{
  //
  // Get the predicted chi2
  //
  
  Double_t x      = trklt->GetX0();
  Double_t p[2]   = { trklt->GetYat(x)
                    , trklt->GetZat(x) };
  Double_t cov[3];
  trklt->GetCovAt(x, cov);
  
  return AliExternalTrackParam::GetPredictedChi2(p, cov);
}

	
//_____________________________________________________________________________
void AliTRDtrackV1::MakeBackupTrack()
{
  //
  // Creates a backup track
  //

  if(fBackupTrack) {
    fBackupTrack->~AliTRDtrackV1();
    new(fBackupTrack) AliTRDtrackV1((AliTRDtrackV1&)(*this));
    return;
  }
  fBackupTrack = new AliTRDtrackV1((AliTRDtrackV1&)(*this));
}

//_____________________________________________________________________________
Int_t AliTRDtrackV1::GetProlongation(Double_t xk, Double_t &y, Double_t &z)
{
  //
  // Find a prolongation at given x
  // Return 0 if it does not exist
  //  

  Double_t bz = GetBz();
  if (!AliExternalTrackParam::GetYAt(xk,bz,y)) return 0;
  if (!AliExternalTrackParam::GetZAt(xk,bz,z)) return 0;

  return 1;  

}

//_____________________________________________________________________________
Bool_t AliTRDtrackV1::PropagateTo(Double_t xk, Double_t xx0, Double_t xrho)
{
  //
  // Propagates this track to a reference plane defined by "xk" [cm] 
  // correcting for the mean crossed material.
  //
  // "xx0"  - thickness/rad.length [units of the radiation length] 
  // "xrho" - thickness*density    [g/cm^2] 
  // 

  if (xk == GetX()) {
    return kTRUE;
  }

  Double_t oldX = GetX();
  Double_t oldY = GetY();
  Double_t oldZ = GetZ();

  Double_t bz   = GetBz();

  if (!AliExternalTrackParam::PropagateTo(xk,bz)) {
    return kFALSE;
  }

  Double_t x = GetX();
  Double_t y = GetY();
  Double_t z = GetZ();

  if (oldX < xk) {
    xrho = -xrho;
    if (IsStartedTimeIntegral()) {
      Double_t l2  = TMath::Sqrt((x-oldX)*(x-oldX) 
                               + (y-oldY)*(y-oldY) 
                               + (z-oldZ)*(z-oldZ));
      Double_t crv = AliExternalTrackParam::GetC(bz);
      if (TMath::Abs(l2*crv) > 0.0001) {
        // Make correction for curvature if neccesary
        l2 = 0.5 * TMath::Sqrt((x-oldX)*(x-oldX) 
                             + (y-oldY)*(y-oldY));
        l2 = 2.0 * TMath::ASin(l2 * crv) / crv;
        l2 = TMath::Sqrt(l2*l2 + (z-oldZ)*(z-oldZ));
      }
      AddTimeStep(l2);
    }
  }

  if (!AliExternalTrackParam::CorrectForMeanMaterial(xx0,xrho,GetMass())) { 
    return kFALSE;
  }

  {

    // Energy losses
    Double_t p2    = (1.0 + GetTgl()*GetTgl()) / (GetSigned1Pt()*GetSigned1Pt());
    Double_t beta2 = p2 / (p2 + GetMass()*GetMass());
    if ((beta2 < 1.0e-10) || 
        ((5940.0 * beta2/(1.0 - beta2 + 1.0e-10) - beta2) < 0.0)) {
      return kFALSE;
    }

    Double_t dE    = 0.153e-3 / beta2 
                   * (TMath::Log(5940.0 * beta2/(1.0 - beta2 + 1.0e-10)) - beta2)
                   * xrho;
    fBudget[0] += xrho;

    /*
    // Suspicious part - think about it ?
    Double_t kinE =  TMath::Sqrt(p2);
    if (dE > 0.8*kinE) dE = 0.8 * kinE;  //      
    if (dE < 0)        dE = 0.0;         // Not valid region for Bethe bloch 
    */
 
    fDE += dE;

    /*
    // Suspicious ! I.B.
    Double_t sigmade = 0.07 * TMath::Sqrt(TMath::Abs(dE));   // Energy loss fluctuation 
    Double_t sigmac2 = sigmade*sigmade*fC*fC*(p2+GetMass()*GetMass())/(p2*p2);
    fCcc += sigmac2;
    fCee += fX*fX * sigmac2;  
    */

  }

  return kTRUE;
}

//_____________________________________________________________________________
Int_t   AliTRDtrackV1::PropagateToR(Double_t r,Double_t step)
{
  //
  // Propagate track to the radial position
  // Rotation always connected to the last track position
  //

  Double_t xyz0[3];
  Double_t xyz1[3];
  Double_t y;
  Double_t z; 

  Double_t radius = TMath::Sqrt(GetX()*GetX() + GetY()*GetY());
  // Direction +-
  Double_t dir    = (radius > r) ? -1.0 : 1.0;   

  for (Double_t x = radius+dir*step; dir*x < dir*r; x += dir*step) {

    GetXYZ(xyz0);	
    Double_t alpha = TMath::ATan2(xyz0[1],xyz0[0]);
    Rotate(alpha,kTRUE);
    GetXYZ(xyz0);	
    GetProlongation(x,y,z);
    xyz1[0] = x * TMath::Cos(alpha) + y * TMath::Sin(alpha); 
    xyz1[1] = x * TMath::Sin(alpha) - y * TMath::Cos(alpha);
    xyz1[2] = z;
    Double_t param[7];
    AliTracker::MeanMaterialBudget(xyz0,xyz1,param);
    if (param[1] <= 0) {
      param[1] = 100000000;
    }
    PropagateTo(x,param[1],param[0]*param[4]);

  } 

  GetXYZ(xyz0);	
  Double_t alpha = TMath::ATan2(xyz0[1],xyz0[0]);
  Rotate(alpha,kTRUE);
  GetXYZ(xyz0);	
  GetProlongation(r,y,z);
  xyz1[0] = r * TMath::Cos(alpha) + y * TMath::Sin(alpha); 
  xyz1[1] = r * TMath::Sin(alpha) - y * TMath::Cos(alpha);
  xyz1[2] = z;
  Double_t param[7];
  AliTracker::MeanMaterialBudget(xyz0,xyz1,param);

  if (param[1] <= 0) {
    param[1] = 100000000;
  }
  PropagateTo(r,param[1],param[0]*param[4]);

  return 0;

}

//_____________________________________________________________________________
Bool_t AliTRDtrackV1::Rotate(Double_t alpha, Bool_t absolute)
{
  //
  // Rotates track parameters in R*phi plane
  // if absolute rotation alpha is in global system
  // otherwise alpha rotation is relative to the current rotation angle
  //  

  if (absolute) alpha -= GetAlpha();
  //else fNRotate++;

  return AliExternalTrackParam::Rotate(GetAlpha()+alpha);
}

//___________________________________________________________
void AliTRDtrackV1::SetNumberOfClusters() 
{
// Calculate the number of clusters attached to this track
	
  Int_t ncls = 0;
  for(int ip=0; ip<kNplane; ip++){
    if(fTracklet[ip] && fTrackletIndex[ip] != 0xffff) ncls += fTracklet[ip]->GetN();
  }
  AliKalmanTrack::SetNumberOfClusters(ncls);	
}

	
//_______________________________________________________________
void AliTRDtrackV1::SetOwner()
{
  //
  // Toggle ownership of tracklets
  //

  if(TestBit(1)) return;
  for (Int_t ip = 0; ip < kNplane; ip++) {
    if(fTrackletIndex[ip] == 0xffff) continue;
    fTracklet[ip] = new AliTRDseedV1(*fTracklet[ip]);
    fTracklet[ip]->SetOwner();
  }
  SetBit(1);
}

//_______________________________________________________________
void AliTRDtrackV1::SetTracklet(AliTRDseedV1 *trklt, Int_t index)
{
  //
  // Set the tracklets
  //
  Int_t plane = trklt->GetPlane();
  if(fTrackletIndex[plane]==0xffff && fTracklet[plane]){ 
    delete fTracklet[plane];
  }
  fTracklet[plane]      = trklt;
  fTrackletIndex[plane] = index;
}

//_______________________________________________________________
Bool_t  AliTRDtrackV1::Update(AliTRDseedV1 *trklt, Double_t chisq)
{
  //
  // Update track and tracklet parameters 
  //
  
  Double_t x      = trklt->GetX0();
  Double_t p[2]   = { trklt->GetYat(x)
                    , trklt->GetZat(x) };
  Double_t cov[3];
  trklt->GetCovAt(x, cov);
  
  // insert systematic uncertainties calibration and misalignment
  Double_t sys[15];
  AliTRDReconstructor::RecoParam()->GetSysCovMatrix(sys);
  cov[0] += (sys[0]*sys[0]);
  cov[2] += (sys[1]*sys[1]);

  if(!AliExternalTrackParam::Update(p, cov)) return kFALSE;
  
  AliTRDcluster *c = 0x0;
  Int_t ic = 0; while(!(c = trklt->GetClusters(ic))) ic++;
  AliTracker::FillResiduals(this, p, cov, c->GetVolumeId());
  
  
  // Register info to track
  SetNumberOfClusters();
  SetChi2(GetChi2() + chisq);
  
  // update tracklet
  trklt->SetMomentum(GetP());
  trklt->SetSnp(GetSnp());
  trklt->SetTgl(GetTgl());
  return kTRUE;
}

//_______________________________________________________________
void AliTRDtrackV1::UpdateESDtrack(AliESDtrack *track)
{
  //
  // Update the TRD PID information in the ESD track
  //

  track->SetNumberOfTRDslices(kNslice);

  for (Int_t ip = 0; ip < kNplane; ip++) {
    if(fTrackletIndex[ip] == 0xffff) continue;
    fTracklet[ip]->CookdEdx(kNslice);
    Float_t *dedx = fTracklet[ip]->GetdEdx();
    for (Int_t js = 0; js < kNslice; js++) track->SetTRDslice(dedx[js], ip, js);
  }

  // copy PID to ESD
  track->SetTRDpid(fPID);
  track->SetTRDpidQuality(fPIDquality);
}

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

//-------------------------------------------------------------------------
//                Implementation of the AliKalmanTrack class
//   that is the base for AliTPCtrack, AliITStrackV2 and AliTRDtrack
//        Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------

#include "AliKalmanTrack.h"
#include "AliPDG.h"
#include "TPDGCode.h"
#include "TDatabasePDG.h"

ClassImp(AliKalmanTrack)

Double_t AliKalmanTrack::fgConvConst;

//_______________________________________________________________________
AliKalmanTrack::AliKalmanTrack():
  fLab(-3141593),
  fChi2(0),
  fMass(0.13957),
  fN(0)
{
  //
  // Default constructor
  //
    if (fgConvConst==0) 
      Fatal("AliKalmanTrack()","The magnetic field has not been set !\n"); 
    
    fStartTimeIntegral = kFALSE;
    fIntegratedLength = 0;
    for(Int_t i=0; i<5; i++) fIntegratedTime[i] = 0;
}

//_______________________________________________________________________
AliKalmanTrack::AliKalmanTrack(const AliKalmanTrack &t):
  TObject(t),
  fLab(t.fLab),
  fChi2(t.fChi2),
  fMass(t.fMass),
  fN(t.fN)
{
  //
  // Copy constructor
  //
  if (fgConvConst==0) 
    Fatal("AliKalmanTrack(const AliKalmanTrack&)",
          "The magnetic field has not been set !\n"); 

  fStartTimeIntegral = t.fStartTimeIntegral;
  fIntegratedLength = t.fIntegratedLength;
  
  for (Int_t i=0; i<5; i++) 
    fIntegratedTime[i] = t.fIntegratedTime[i];
}

//_______________________________________________________________________
Double_t AliKalmanTrack::GetX() const
{
  Warning("GetX()","Method must be overloaded !\n");
  return 0.;
}
//_______________________________________________________________________
Double_t AliKalmanTrack::GetdEdx() const
{
  Warning("GetdEdx()","Method must be overloaded !\n");
  return 0.;
}

//_______________________________________________________________________
Double_t AliKalmanTrack::GetY() const
{
  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[0];
}
//_______________________________________________________________________
Double_t AliKalmanTrack::GetZ() const
{
  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[1];
}
//_______________________________________________________________________
Double_t AliKalmanTrack::GetSnp() const
{
  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[2];
}
//_______________________________________________________________________
Double_t AliKalmanTrack::GetTgl() const
{
  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[3];
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Get1Pt() const
{
  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[4];
}

//_______________________________________________________________________
Double_t AliKalmanTrack::Phi() const
{
// return global phi of track

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  if (par[2] >  1.) par[2] =  1.;
  if (par[2] < -1.) par[2] = -1.;
  Double_t phi = TMath::ASin(par[2]) + GetAlpha();
  while (phi < 0) phi += TMath::TwoPi();
  while (phi > TMath::TwoPi()) phi -= TMath::TwoPi();
  return phi;
}
//_______________________________________________________________________
Double_t AliKalmanTrack::SigmaPhi() const
{
// return error of global phi of track

  Double_t par[5];
  Double_t cov[15];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  GetExternalCovariance(cov);
  return TMath::Sqrt(TMath::Abs(cov[5] / (1. - par[2]*par[2])));
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Theta() const
{
// return global theta of track

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return TMath::Pi()/2. - TMath::ATan(par[3]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::SigmaTheta() const
{
// return error of global theta of track

  Double_t par[5];
  Double_t cov[15];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  GetExternalCovariance(cov);
  return TMath::Sqrt(TMath::Abs(cov[5])) / (1. + par[3]*par[3]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Px() const
{
// return x component of track momentum

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  Double_t phi = TMath::ASin(par[2]) + GetAlpha();
  return TMath::Cos(phi) / TMath::Abs(par[4]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Py() const
{
// return y component of track momentum

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  Double_t phi = TMath::ASin(par[2]) + GetAlpha();
  return TMath::Sin(phi) / TMath::Abs(par[4]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Pz() const
{
// return z component of track momentum

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return par[3] / TMath::Abs(par[4]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::Pt() const
{
// return transverse component of track momentum

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return 1. / TMath::Abs(par[4]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::SigmaPt() const
{
// return error of transverse component of track momentum

  Double_t par[5];
  Double_t cov[15];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  GetExternalCovariance(cov);
  return TMath::Sqrt(cov[14]) / TMath::Abs(par[4]);
}
//_______________________________________________________________________
Double_t AliKalmanTrack::P() const
{
// return total track momentum

  Double_t par[5];
  Double_t localX = GetX();
  GetExternalParameters(localX, par);
  return 1. / TMath::Abs(par[4] * TMath::Cos(TMath::ATan(par[3])));
}
//_______________________________________________________________________
void AliKalmanTrack::StartTimeIntegral() 
{
  // Sylwester Radomski, GSI
  // S.Radomski@gsi.de
  //
  // Start time integration
  // To be called at Vertex by ITS tracker
  //
  
  //if (fStartTimeIntegral) 
  //  Warning("StartTimeIntegral", "Reseting Recorded Time.");

  fStartTimeIntegral = kTRUE;
  for(Int_t i=0; i<fgkTypes; i++) fIntegratedTime[i] = 0;  
  fIntegratedLength = 0;
}
//_______________________________________________________________________
void AliKalmanTrack:: AddTimeStep(Double_t length) 
{
  // 
  // Add step to integrated time
  // this method should be called by a sublasses at the end
  // of the PropagateTo function or by a tracker
  // each time step is made.
  //
  // If integration not started function does nothing
  //
  // Formula
  // dt = dl * sqrt(p^2 + m^2) / p
  // p = pT * (1 + tg^2 (lambda) )
  //
  // pt = 1/external parameter [4]
  // tg lambda = external parameter [3]
  //
  //
  // Sylwester Radomski, GSI
  // S.Radomski@gsi.de
  // 
  
  static const Double_t kcc = 2.99792458e-2;

  if (!fStartTimeIntegral) return;
  
  fIntegratedLength += length;

  static Int_t pdgCode[fgkTypes]  = {kElectron, kMuonMinus, kPiPlus, kKPlus, kProton};
  TDatabasePDG *db = TDatabasePDG::Instance();

  Double_t xr, param[5];
  Double_t pt, tgl;
  
  GetExternalParameters(xr, param);
  pt =  1/param[4] ;
  tgl = param[3];

  Double_t p = TMath::Abs(pt * TMath::Sqrt(1+tgl*tgl));

  if (length > 100) return;

  for (Int_t i=0; i<fgkTypes; i++) {
    
    Double_t mass = db->GetParticle(pdgCode[i])->Mass();
    Double_t correction = TMath::Sqrt( pt*pt * (1 + tgl*tgl) + mass * mass ) / p;
    Double_t time = length * correction / kcc;

    //cout << mass << "\t" << pt << "\t" << p << "\t" 
    //     << correction << endl;

    fIntegratedTime[i] += time;
  }
}

//_______________________________________________________________________

Double_t AliKalmanTrack::GetIntegratedTime(Int_t pdg) const 
{
  // Sylwester Radomski, GSI
  // S.Radomski@gsi.de
  //
  // Return integrated time hypothesis for a given particle
  // type assumption.
  //
  // Input parameter:
  // pdg - Pdg code of a particle type
  //


  if (!fStartTimeIntegral) {
    Warning("GetIntegratedTime","Time integration not started");
    return 0.;
  }

  static Int_t pdgCode[fgkTypes] = {kElectron, kMuonMinus, kPiPlus, kKPlus, kProton};

  for (Int_t i=0; i<fgkTypes; i++)
    if (pdgCode[i] == TMath::Abs(pdg)) return fIntegratedTime[i];

  Warning(":GetIntegratedTime","Particle type [%d] not found", pdg);
  return 0;
}

void AliKalmanTrack::GetIntegratedTimes(Double_t *times) const {
  for (Int_t i=0; i<fgkTypes; i++) times[i]=fIntegratedTime[i];
}

void AliKalmanTrack::SetIntegratedTimes(const Double_t *times) {
  for (Int_t i=0; i<fgkTypes; i++) fIntegratedTime[i]=times[i];
}

//_______________________________________________________________________

void AliKalmanTrack::PrintTime() const
{
  // Sylwester Radomski, GSI
  // S.Radomski@gsi.de
  //
  // For testing
  // Prints time for all hypothesis
  //

  static Int_t pdgCode[fgkTypes] = {kElectron, kMuonMinus, kPiPlus, kKPlus, kProton};

  for (Int_t i=0; i<fgkTypes; i++)
    printf("%d: %.2f  ", pdgCode[i], fIntegratedTime[i]);
  printf("\n");  
}

static void External2Helix(const AliKalmanTrack *t, Double_t helix[6]) { 
  //--------------------------------------------------------------------
  // External track parameters -> helix parameters 
  //--------------------------------------------------------------------
  Double_t alpha,x,cs,sn;
  t->GetExternalParameters(x,helix); alpha=t->GetAlpha();

  cs=TMath::Cos(alpha); sn=TMath::Sin(alpha);
  helix[5]=x*cs - helix[0]*sn;            // x0
  helix[0]=x*sn + helix[0]*cs;            // y0
//helix[1]=                               // z0
  helix[2]=TMath::ASin(helix[2]) + alpha; // phi0
//helix[3]=                               // tgl
  helix[4]=helix[4]/t->GetConvConst();    // C
}

static void Evaluate(const Double_t *h, Double_t t,
                     Double_t r[3],  //radius vector
                     Double_t g[3],  //first defivatives
                     Double_t gg[3]) //second derivatives
{
  //--------------------------------------------------------------------
  // Calculate position of a point on a track and some derivatives
  //--------------------------------------------------------------------
  Double_t phase=h[4]*t+h[2];
  Double_t sn=TMath::Sin(phase), cs=TMath::Cos(phase);

  r[0] = h[5] + (sn - h[6])/h[4];
  r[1] = h[0] - (cs - h[7])/h[4];  
  r[2] = h[1] + h[3]*t;

  g[0] = cs; g[1]=sn; g[2]=h[3];
  
  gg[0]=-h[4]*sn; gg[1]=h[4]*cs; gg[2]=0.;
}

Double_t AliKalmanTrack::
GetDCA(const AliKalmanTrack *p, Double_t &xthis, Double_t &xp) const {
  //------------------------------------------------------------
  // Returns the (weighed !) distance of closest approach between 
  // this track and the track passed as the argument.
  // Other returned values:
  //   xthis, xt - coordinates of tracks' reference planes at the DCA 
  //-----------------------------------------------------------
  Double_t dy2=GetSigmaY2() + p->GetSigmaY2();
  Double_t dz2=GetSigmaZ2() + p->GetSigmaZ2();
  Double_t dx2=dy2; 

  //dx2=dy2=dz2=1.;

  Double_t p1[8]; External2Helix(this,p1);
  p1[6]=TMath::Sin(p1[2]); p1[7]=TMath::Cos(p1[2]);
  Double_t p2[8]; External2Helix(p,p2);
  p2[6]=TMath::Sin(p2[2]); p2[7]=TMath::Cos(p2[2]);


  Double_t r1[3],g1[3],gg1[3]; Double_t t1=0.;
  Evaluate(p1,t1,r1,g1,gg1);
  Double_t r2[3],g2[3],gg2[3]; Double_t t2=0.;
  Evaluate(p2,t2,r2,g2,gg2);

  Double_t dx=r2[0]-r1[0], dy=r2[1]-r1[1], dz=r2[2]-r1[2];
  Double_t dm=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;

  Int_t max=27;
  while (max--) {
     Double_t gt1=-(dx*g1[0]/dx2 + dy*g1[1]/dy2 + dz*g1[2]/dz2);
     Double_t gt2=+(dx*g2[0]/dx2 + dy*g2[1]/dy2 + dz*g2[2]/dz2);
     Double_t h11=(g1[0]*g1[0] - dx*gg1[0])/dx2 + 
                  (g1[1]*g1[1] - dy*gg1[1])/dy2 +
                  (g1[2]*g1[2] - dz*gg1[2])/dz2;
     Double_t h22=(g2[0]*g2[0] + dx*gg2[0])/dx2 + 
                  (g2[1]*g2[1] + dy*gg2[1])/dy2 +
                  (g2[2]*g2[2] + dz*gg2[2])/dz2;
     Double_t h12=-(g1[0]*g2[0]/dx2 + g1[1]*g2[1]/dy2 + g1[2]*g2[2]/dz2);

     Double_t det=h11*h22-h12*h12;

     Double_t dt1,dt2;
     if (TMath::Abs(det)<1.e-33) {
        //(quasi)singular Hessian
        dt1=-gt1; dt2=-gt2;
     } else {
        dt1=-(gt1*h22 - gt2*h12)/det; 
        dt2=-(h11*gt2 - h12*gt1)/det;
     }

     if ((dt1*gt1+dt2*gt2)>0) {dt1=-dt1; dt2=-dt2;}

     //check delta(phase1) ?
     //check delta(phase2) ?

     if (TMath::Abs(dt1)/(TMath::Abs(t1)+1.e-3) < 1.e-4)
     if (TMath::Abs(dt2)/(TMath::Abs(t2)+1.e-3) < 1.e-4) {
        if ((gt1*gt1+gt2*gt2) > 1.e-4/dy2/dy2) 
	  Warning("GetDCA"," stopped at not a stationary point !\n");
        Double_t lmb=h11+h22; lmb=lmb-TMath::Sqrt(lmb*lmb-4*det);
        if (lmb < 0.) 
	  Warning("GetDCA"," stopped at not a minimum !\n");
        break;
     }

     Double_t dd=dm;
     for (Int_t div=1 ; ; div*=2) {
        Evaluate(p1,t1+dt1,r1,g1,gg1);
        Evaluate(p2,t2+dt2,r2,g2,gg2);
        dx=r2[0]-r1[0]; dy=r2[1]-r1[1]; dz=r2[2]-r1[2];
        dd=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;
	if (dd<dm) break;
        dt1*=0.5; dt2*=0.5;
        if (div>512) {
           Warning("GetDCA"," overshoot !\n"); break;
        }   
     }
     dm=dd;

     t1+=dt1;
     t2+=dt2;

  }

  if (max<=0) Warning("GetDCA"," too many iterations !\n");  

  Double_t cs=TMath::Cos(GetAlpha());
  Double_t sn=TMath::Sin(GetAlpha());
  xthis=r1[0]*cs + r1[1]*sn;

  cs=TMath::Cos(p->GetAlpha());
  sn=TMath::Sin(p->GetAlpha());
  xp=r2[0]*cs + r2[1]*sn;

  return TMath::Sqrt(dm*TMath::Sqrt(dy2*dz2));
}

Double_t AliKalmanTrack::
PropagateToDCA(AliKalmanTrack *p, Double_t d, Double_t x0) {
  //--------------------------------------------------------------
  // Propagates this track and the argument track to the position of the
  // distance of closest approach. 
  // Returns the (weighed !) distance of closest approach.
  //--------------------------------------------------------------
  Double_t xthis,xp;
  Double_t dca=GetDCA(p,xthis,xp);

  if (!PropagateTo(xthis,d,x0)) {
    //Warning("PropagateToDCA"," propagation failed !\n");
    return 1e+33;
  }  

  if (!p->PropagateTo(xp,d,x0)) {
    //Warning("PropagateToDCA"," propagation failed !\n";
    return 1e+33;
  }  

  return dca;
}

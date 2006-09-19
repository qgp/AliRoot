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

//-----------------------------------------------------------------
//           Implementation of the TPC track class
//        This class is used by the AliTPCtracker class
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-----------------------------------------------------------------

#include <Riostream.h>

#include "AliTPCtrack.h"
#include "AliCluster.h"
#include "AliESDtrack.h"
#include "AliTPCReconstructor.h"

ClassImp(AliTPCtrack)

//_________________________________________________________________________
AliTPCtrack::AliTPCtrack(): 
  AliKalmanTrack(),
  fX(0),
  fAlpha(0),
  fdEdx(0),
  fP0(0),
  fP1(0),
  fP2(0),
  fP3(0),
  fP4(0),
  fC00(1e10),
  fC10(0),
  fC11(1e10),
  fC20(0),
  fC21(0),
  fC22(1e10),
  fC30(0),
  fC31(0),
  fC32(0),
  fC33(1e10),
  fC40(0),
  fC41(0),
  fC42(0),
  fC43(0),
  fC44(0),
  fNumber(0),
  fSdEdx(1e10),
  fNFoundable(0),
  fBConstrain(kFALSE),
  fLastPoint(-1),
  fFirstPoint(-1),
  fRemoval(0),
  fTrackType(0),
  fLab2(-1),
  fNShared(0),
  fReference()
{
  //-------------------------------------------------
  // default constructor
  //-------------------------------------------------
  for (Int_t i=0; i<kMaxRow;i++) fIndex[i]=-2;
  for (Int_t i=0; i<4;i++) fPoints[i]=0.;
  for (Int_t i=0; i<12;i++) fKinkPoint[i]=0.;
  for (Int_t i=0; i<3;i++) fKinkIndexes[i]=0;
  for (Int_t i=0; i<3;i++) fV0Indexes[i]=0;
}

//_________________________________________________________________________



AliTPCtrack::AliTPCtrack(UInt_t index, const Double_t xx[5],
const Double_t cc[15], Double_t xref, Double_t alpha) :
  AliKalmanTrack(),
  fX(xref),
  fdEdx(0),
  fP0(xx[0]),
  fP1(xx[1]),
  fP2(xx[2]),
  fP3(xx[3]),
  fP4(xx[4]),
  fC00(cc[0]),
  fC10(cc[1]),
  fC11(cc[2]),
  fC20(cc[3]),
  fC21(cc[4]),
  fC22(cc[5]),
  fC30(cc[6]),
  fC31(cc[7]),
  fC32(cc[8]),
  fC33(cc[9]),
  fC40(cc[10]),
  fC41(cc[11]),
  fC42(cc[12]),
  fC43(cc[13]),
  fC44(cc[14]),
  fNumber(1),
  fSdEdx(1e10),
  fNFoundable(0),
  fBConstrain(kFALSE),
  fLastPoint(0),
  fFirstPoint(0),
  fRemoval(0),
  fTrackType(0),
  fLab2(0),
  fNShared(0),
  fReference()
{
  //-----------------------------------------------------------------
  // This is the main track constructor.
  //-----------------------------------------------------------------
  fAlpha=alpha;
  if (fAlpha<-TMath::Pi()) fAlpha += 2*TMath::Pi();
  if (fAlpha>=TMath::Pi()) fAlpha -= 2*TMath::Pi();

  SaveLocalConvConst();

  Double_t c0=TMath::Sign(1/(kMostProbableMomentum*GetConvConst()),fP4);
  Double_t w0=fC44/(fC44 + c0*c0), w1=c0*c0/(fC44 + c0*c0);
  fP4 = w0*c0 + w1*fP4;
  fC40*=w1; fC41*=w1; fC42*=w1; fC43*=w1; fC44*=w1;

  SetNumberOfClusters(1);
  
  fIndex[0]=index;
  for (Int_t i=1; i<kMaxRow;i++) fIndex[i]=-2;
  for (Int_t i=0; i<4;i++) fPoints[i]=0.;
  for (Int_t i=0; i<12;i++) fKinkPoint[i]=0.;
  for (Int_t i=0; i<3;i++) fKinkIndexes[i]=0;
  for (Int_t i=0; i<3;i++) fV0Indexes[i]=0;
}

//_____________________________________________________________________________
AliTPCtrack::AliTPCtrack(const AliESDtrack& t) :
  AliKalmanTrack(),
  fSdEdx(1e10),
  fNFoundable(0),
  fBConstrain(kFALSE),
  fLastPoint(0),
  fFirstPoint(0),
  fRemoval(0),
  fTrackType(0),
  fLab2(0),
  fNShared(0),
  fReference()
{
  //-----------------------------------------------------------------
  // Conversion AliESDtrack -> AliTPCtrack.
  //-----------------------------------------------------------------
  SetNumberOfClusters(t.GetTPCclusters(fIndex));
  SetLabel(t.GetLabel());
  SetMass(t.GetMass());
  for (Int_t i=0; i<4;i++) fPoints[i]=0.;
  for (Int_t i=0; i<12;i++) fKinkPoint[i]=0.;
  for (Int_t i=0; i<3;i++) fKinkIndexes[i]=0;
  for (Int_t i=0; i<3;i++) fV0Indexes[i]=0;

  fdEdx  = t.GetTPCsignal();
  fAlpha = t.GetAlpha();
  if      (fAlpha < -TMath::Pi()) fAlpha += 2*TMath::Pi();
  else if (fAlpha >= TMath::Pi()) fAlpha -= 2*TMath::Pi();

  //Conversion of the track parameters
  Double_t x,p[5]; t.GetExternalParameters(x,p);
  Double_t c[15]; t.GetExternalCovariance(c);

  fX=x;    
  fP0=p[0];
  fP1=p[1];    SaveLocalConvConst();
  fP3=p[3];    x=GetLocalConvConst();
  fP4=p[4]/x;
  fP2=fP4*fX - p[2];

  //Conversion of the covariance matrix
  c[10]/=x; c[11]/=x; c[12]/=x; c[13]/=x; c[14]/=x*x;

  Double_t c22=fX*fX*c[14] - 2*fX*c[12] + c[5];
  Double_t c32=fX*c[13] - c[8];
  Double_t c20=fX*c[10] - c[3], c21=fX*c[11] - c[4], c42=fX*c[14] - c[12];

  fC00=c[0 ];
  fC10=c[1 ];   fC11=c[2 ];
  fC20=c20;     fC21=c21;     fC22=c22;
  fC30=c[6 ];   fC31=c[7 ];   fC32=c32;   fC33=c[9 ];
  fC40=c[10];   fC41=c[11];   fC42=c42;   fC43=c[13]; fC44=c[14];

  if ((t.GetStatus()&AliESDtrack::kTIME) == 0) return;
  StartTimeIntegral();
  Double_t times[10]; t.GetIntegratedTimes(times); SetIntegratedTimes(times);
  SetIntegratedLength(t.GetIntegratedLength());
}

//_____________________________________________________________________________
AliTPCtrack::AliTPCtrack(const AliTPCtrack& t) :
  AliKalmanTrack(t),
  fX(t.fX),
  fAlpha(t.fAlpha),
  fdEdx(t.fdEdx),
  fP0(t.fP0),
  fP1(t.fP1),
  fP2(t.fP2),
  fP3(t.fP3),
  fP4(t.fP4),
  fC00(t.fC00),
  fC10(t.fC10),
  fC11(t.fC11),
  fC20(t.fC20),
  fC21(t.fC21),
  fC22(t.fC22),
  fC30(t.fC30),
  fC31(t.fC31),
  fC32(t.fC32),
  fC33(t.fC33),
  fC40(t.fC40),
  fC41(t.fC41),
  fC42(t.fC42),
  fC43(t.fC43),
  fC44(t.fC44),
  fNumber(t.fNumber),
  fSdEdx(t.fSdEdx),
  fNFoundable(t.fNFoundable),
  fBConstrain(t.fBConstrain),
  fLastPoint(t.fLastPoint),
  fFirstPoint(t.fFirstPoint),
  fRemoval(t.fRemoval),
  fTrackType(t.fTrackType),
  fLab2(t.fLab2),
  fNShared(t.fNShared),
  fReference(t.fReference)

{
  //-----------------------------------------------------------------
  // This is a track copy constructor.
  //-----------------------------------------------------------------
  for (Int_t i=0; i<kMaxRow; i++) fIndex[i]=t.fIndex[i];
  for (Int_t i=0; i<4;i++) fPoints[i]=t.fPoints[i];
  for (Int_t i=0; i<12;i++) fKinkPoint[i]=t.fKinkPoint[i];
  for (Int_t i=0; i<3;i++) fKinkIndexes[i]=t.fKinkIndexes[i];
  for (Int_t i=0; i<3;i++) fV0Indexes[i]=t.fV0Indexes[i];
}

//_____________________________________________________________________________
Int_t AliTPCtrack::Compare(const TObject *o) const {
  //-----------------------------------------------------------------
  // This function compares tracks according to the their curvature
  //-----------------------------------------------------------------
  AliTPCtrack *t=(AliTPCtrack*)o;
  //Double_t co=TMath::Abs(t->Get1Pt());
  //Double_t c =TMath::Abs(Get1Pt());
  Double_t co=t->GetSigmaY2()*t->GetSigmaZ2();
  Double_t c =GetSigmaY2()*GetSigmaZ2();
  if (c>co) return 1;
  else if (c<co) return -1;
  return 0;
}

//_____________________________________________________________________________
void AliTPCtrack::GetExternalCovariance(Double_t cc[15]) const {
 //-------------------------------------------------------------------------
  // This function returns an external representation of the covriance matrix.
  //   (See comments in AliTPCtrack.h about external track representation)
  //-------------------------------------------------------------------------
  Double_t a=GetLocalConvConst();

  Double_t c22=fX*fX*fC44-2*fX*fC42+fC22;
  Double_t c32=fX*fC43-fC32;
  Double_t c20=fX*fC40-fC20, c21=fX*fC41-fC21, c42=fX*fC44-fC42;
  
  cc[0 ]=fC00;
  cc[1 ]=fC10;   cc[2 ]=fC11;
  cc[3 ]=c20;    cc[4 ]=c21;    cc[5 ]=c22;
  cc[6 ]=fC30;   cc[7 ]=fC31;   cc[8 ]=c32;   cc[9 ]=fC33; 
  cc[10]=fC40*a; cc[11]=fC41*a; cc[12]=c42*a; cc[13]=fC43*a; cc[14]=fC44*a*a;

}

//_____________________________________________________________________________
Double_t AliTPCtrack::GetPredictedChi2(const AliCluster *c) const 
{
  //-----------------------------------------------------------------
  // This function calculates a predicted chi2 increment.
  //-----------------------------------------------------------------
  Double_t r00=c->GetSigmaY2(), r01=0., r11=c->GetSigmaZ2();
  r00+=fC00; r01+=fC10; r11+=fC11;

  Double_t det=r00*r11 - r01*r01;
  if (TMath::Abs(det) < 1.e-10) {
    Int_t n=GetNumberOfClusters();
    if (n>4) cerr<<n<<" AliKalmanTrack warning: Singular matrix !\n";
    return 1e10;
  }
  Double_t tmp=r00; r00=r11; r11=tmp; r01=-r01;
  
  Double_t dy=c->GetY() - fP0, dz=c->GetZ() - fP1;
  
  return (dy*r00*dy + 2*r01*dy*dz + dz*r11*dz)/det;
}

Double_t AliTPCtrack::GetYat(Double_t xk) const {
//-----------------------------------------------------------------
// This function calculates the Y-coordinate of a track at the plane x=xk.
//-----------------------------------------------------------------
  if (TMath::Abs(fP4*fX - fP2)>AliTPCReconstructor::GetMaxSnpTrack()) return 0.; //patch 01 jan 06
    Double_t c1=fP4*fX - fP2, r1=TMath::Sqrt(1.- c1*c1);
    Double_t c2=fP4*xk - fP2;
    if (c2*c2>AliTPCReconstructor::GetMaxSnpTrack()) {
      //       Int_t n=GetNumberOfClusters();
      // if (n>4) cerr<<n<<"AliTPCtrack::GetYat: can't evaluate the y-coord !\n";
       return 0;
    } 
    Double_t r2=TMath::Sqrt(1.- c2*c2);
    return fP0 + (xk-fX)*(c1+c2)/(r1+r2);
}

//_____________________________________________________________________________
Int_t AliTPCtrack::PropagateTo(Double_t xk,Double_t /*x0*/,Double_t rho) {
  //-----------------------------------------------------------------
  // This function propagates a track to a reference plane x=xk.
  //-----------------------------------------------------------------
  if (TMath::Abs(fP4*xk - fP2) >= AliTPCReconstructor::GetMaxSnpTrack()) {
    //    Int_t n=GetNumberOfClusters();
    //if (n>4) cerr<<n<<" AliTPCtrack warning: Propagation failed !\n";
    return 0;
  }
  Double_t lcc=GetLocalConvConst();  

  // old position for time [SR, GSI 17.02.2003]
  Double_t oldX = fX;
  Double_t oldY = fP0;
  Double_t oldZ = fP1;
  //

  Double_t x1=fX, x2=x1+(xk-x1), dx=x2-x1, y1=fP0, z1=fP1;
  Double_t c1=fP4*x1 - fP2, r1=sqrt(1.- c1*c1);
  Double_t c2=fP4*x2 - fP2, r2=sqrt(1.- c2*c2);
  
  fP0 += dx*(c1+c2)/(r1+r2);
  fP1 += dx*(c1+c2)/(c1*r2 + c2*r1)*fP3;

  //f = F - 1
  Double_t rr=r1+r2, cc=c1+c2, xx=x1+x2;
  Double_t f02=-dx*(2*rr + cc*(c1/r1 + c2/r2))/(rr*rr);
  Double_t f04= dx*(rr*xx + cc*(c1*x1/r1+c2*x2/r2))/(rr*rr);
  Double_t cr=c1*r2+c2*r1;
  Double_t f12=-dx*fP3*(2*cr + cc*(c2*c1/r1-r1 + c1*c2/r2-r2))/(cr*cr);
  Double_t f13= dx*cc/cr; 
  Double_t f14=dx*fP3*(cr*xx-cc*(r1*x2-c2*c1*x1/r1+r2*x1-c1*c2*x2/r2))/(cr*cr);

  //b = C*ft
  Double_t b00=f02*fC20 + f04*fC40, b01=f12*fC20 + f14*fC40 + f13*fC30;
  Double_t b10=f02*fC21 + f04*fC41, b11=f12*fC21 + f14*fC41 + f13*fC31;
  Double_t b20=f02*fC22 + f04*fC42, b21=f12*fC22 + f14*fC42 + f13*fC32;
  Double_t b30=f02*fC32 + f04*fC43, b31=f12*fC32 + f14*fC43 + f13*fC33;
  Double_t b40=f02*fC42 + f04*fC44, b41=f12*fC42 + f14*fC44 + f13*fC43;
  
  //a = f*b = f*C*ft
  Double_t a00=f02*b20+f04*b40,a01=f02*b21+f04*b41,a11=f12*b21+f14*b41+f13*b31;

  //F*C*Ft = C + (a + b + bt)
  fC00 += a00 + 2*b00;
  fC10 += a01 + b01 + b10; 
  fC20 += b20;
  fC30 += b30;
  fC40 += b40;
  fC11 += a11 + 2*b11;
  fC21 += b21; 
  fC31 += b31; 
  fC41 += b41; 

  fX=x2;

  //Change of the magnetic field *************
  SaveLocalConvConst();
  cc=fP4;
  fP4*=lcc/GetLocalConvConst();
  fP2+=fX*(fP4-cc);

  //Multiple scattering ******************
  Double_t d=sqrt((x1-fX)*(x1-fX)+(y1-fP0)*(y1-fP0)+(z1-fP1)*(z1-fP1));
  Double_t p2=P()*P();
  Double_t beta2=p2/(p2 + GetMass()*GetMass());
  beta2 = TMath::Min(beta2,0.99999999999);
  //Double_t theta2=14.1*14.1/(beta2*p2*1e6)*d/x0*rho;
  Double_t theta2=1.0259e-6*10*10/20/(beta2*p2)*d*rho;

  Double_t ey=fP4*fX - fP2, ez=fP3;
  Double_t xz=fP4*ez, zz1=ez*ez+1, xy=fP2+ey;
    
  fC22 += (2*ey*ez*ez*fP2+1-ey*ey+ez*ez+fP2*fP2*ez*ez)*theta2;
  fC32 += ez*zz1*xy*theta2;
  fC33 += zz1*zz1*theta2;
  fC42 += xz*ez*xy*theta2;
  fC43 += xz*zz1*theta2;
  fC44 += xz*xz*theta2;
  /*
  //
  //MI coeficients
  Double_t dc22 = (1-ey*ey+xz*xz*fX*fX)*theta2;
  Double_t dc32 = (xz*fX*zz1)*theta2;
  Double_t dc33 = (zz1*zz1)*theta2;
  Double_t dc42 = (xz*fX*xz)*theta2;
  Double_t dc43 = (zz1*xz)*theta2;
  Double_t dc44 = (xz*xz)*theta2; 
  fC22 += dc22;
  fC32 += dc32;
  fC33 += dc33;
  fC42 += dc42;
  fC43 += dc43;
  fC44 += dc44;
  */
  //Energy losses ************************
  Double_t dE=0.153e-3/beta2*(log(5940*beta2/(1-beta2)) - beta2)*d*rho;
  if (x1 < x2) dE=-dE;
  cc=fP4;

  //Double_t E = sqrt(p2+GetMass()*GetMass());
  //Double_t mifac  = TMath::Sqrt(1.+dE*dE/p2+2*E*dE/p2)-1;
  //Double_t belfac = E*dE/p2;
			       //
  fP4*=(1.- sqrt(p2+GetMass()*GetMass())/p2*dE);
  fP2+=fX*(fP4-cc);

  // Integrated Time [SR, GSI, 17.02.2003]
 if (x1 < x2)
 if (IsStartedTimeIntegral()) {
    Double_t l2 = (fX-oldX)*(fX-oldX)+(fP0-oldY)*(fP0-oldY)+(fP1-oldZ)*(fP1-oldZ);
    AddTimeStep(TMath::Sqrt(l2));
  }
  //

  return 1;
}

//_____________________________________________________________________________
Int_t AliTPCtrack::PropagateToVertex(Double_t x0,Double_t rho) 
{
  //-----------------------------------------------------------------
  // This function propagates tracks to the "vertex".
  //-----------------------------------------------------------------
  Double_t c=fP4*fX - fP2;
  Double_t tgf=-fP2/(fP4*fP0 + sqrt(1-c*c));
  Double_t snf=tgf/sqrt(1.+ tgf*tgf);
  Double_t xv=(fP2+snf)/fP4;
  return PropagateTo(xv,x0,rho);
}

//_____________________________________________________________________________
Int_t AliTPCtrack::Update(const AliCluster *c, Double_t chisq, UInt_t index) {
  //-----------------------------------------------------------------
  // This function associates a cluster with this track.
  //-----------------------------------------------------------------
  Double_t r00=c->GetSigmaY2(), r01=0., r11=c->GetSigmaZ2();
  r00+=fC00; r01+=fC10; r11+=fC11;
  Double_t det=r00*r11 - r01*r01;
  Double_t tmp=r00; r00=r11/det; r11=tmp/det; r01=-r01/det;

  Double_t k00=fC00*r00+fC10*r01, k01=fC00*r01+fC10*r11;
  Double_t k10=fC10*r00+fC11*r01, k11=fC10*r01+fC11*r11;
  Double_t k20=fC20*r00+fC21*r01, k21=fC20*r01+fC21*r11;
  Double_t k30=fC30*r00+fC31*r01, k31=fC30*r01+fC31*r11;
  Double_t k40=fC40*r00+fC41*r01, k41=fC40*r01+fC41*r11;

  Double_t dy=c->GetY() - fP0, dz=c->GetZ() - fP1;
  Double_t cur=fP4 + k40*dy + k41*dz, eta=fP2 + k20*dy + k21*dz;
  if (TMath::Abs(cur*fX-eta) >= AliTPCReconstructor::GetMaxSnpTrack()) {
    //    Int_t n=GetNumberOfClusters();
    //if (n>4) cerr<<n<<" AliTPCtrack warning: Filtering failed !\n";
    return 0;
  }

  fP0 += k00*dy + k01*dz;
  fP1 += k10*dy + k11*dz;
  fP2  = eta;
  fP3 += k30*dy + k31*dz;
  fP4  = cur;

  Double_t c01=fC10, c02=fC20, c03=fC30, c04=fC40;
  Double_t c12=fC21, c13=fC31, c14=fC41;

  fC00-=k00*fC00+k01*fC10; fC10-=k00*c01+k01*fC11;
  fC20-=k00*c02+k01*c12;   fC30-=k00*c03+k01*c13;
  fC40-=k00*c04+k01*c14; 

  fC11-=k10*c01+k11*fC11;
  fC21-=k10*c02+k11*c12;   fC31-=k10*c03+k11*c13;
  fC41-=k10*c04+k11*c14; 

  fC22-=k20*c02+k21*c12;   fC32-=k20*c03+k21*c13;
  fC42-=k20*c04+k21*c14; 

  fC33-=k30*c03+k31*c13;
  fC43-=k40*c03+k41*c13; 

  fC44-=k40*c04+k41*c14; 

  Int_t n=GetNumberOfClusters();
  fIndex[n]=index;
  SetNumberOfClusters(n+1);
  SetChi2(GetChi2()+chisq);

  return 1;
}

//_____________________________________________________________________________
Int_t AliTPCtrack::Rotate(Double_t alpha)
{
  //-----------------------------------------------------------------
  // This function rotates this track.
  //-----------------------------------------------------------------
  fAlpha += alpha;
  if (fAlpha<-TMath::Pi()) fAlpha += 2*TMath::Pi();
  if (fAlpha>=TMath::Pi()) fAlpha -= 2*TMath::Pi();
  
  Double_t x1=fX, y1=fP0;
  Double_t ca=cos(alpha), sa=sin(alpha);
  Double_t r1=fP4*fX - fP2;
  
  if (TMath::Abs(r1)>=AliTPCReconstructor::GetMaxSnpTrack()) return 0; //patch 01 jan 06

  fX = x1*ca + y1*sa;
  fP0=-x1*sa + y1*ca;
  fP2=fP2*ca + (fP4*y1 + sqrt(1.- r1*r1))*sa;
  
  Double_t r2=fP4*fX - fP2;
  if (TMath::Abs(r2) >= AliTPCReconstructor::GetMaxSnpTrack()) {
    //Int_t n=GetNumberOfClusters();
    //    if (n>4) cerr<<n<<" AliTPCtrack warning: Rotation failed !\n";
    return 0;
  }
  
  Double_t y0=fP0 + sqrt(1.- r2*r2)/fP4;
  if ((fP0-y0)*fP4 >= 0.) {
    //Int_t n=GetNumberOfClusters();
    //    if (n>4) cerr<<n<<" AliTPCtrack warning: Rotation failed !!!\n";
    return 0;
  }

  //f = F - 1
  Double_t f00=ca-1,    f24=(y1 - r1*x1/sqrt(1.- r1*r1))*sa, 
           f20=fP4*sa,  f22=(ca + sa*r1/sqrt(1.- r1*r1))-1;

  //b = C*ft
  Double_t b00=fC00*f00, b02=fC00*f20+fC40*f24+fC20*f22;
  Double_t b10=fC10*f00, b12=fC10*f20+fC41*f24+fC21*f22;
  Double_t b20=fC20*f00, b22=fC20*f20+fC42*f24+fC22*f22;
  Double_t b30=fC30*f00, b32=fC30*f20+fC43*f24+fC32*f22;
  Double_t b40=fC40*f00, b42=fC40*f20+fC44*f24+fC42*f22;

  //a = f*b = f*C*ft
  Double_t a00=f00*b00, a02=f00*b02, a22=f20*b02+f24*b42+f22*b22;

  // *** Double_t dy2=fCyy;

  //F*C*Ft = C + (a + b + bt)
  fC00 += a00 + 2*b00;
  fC10 += b10;
  fC20 += a02+b20+b02;
  fC30 += b30;
  fC40 += b40;
  fC21 += b12;
  fC32 += b32;
  fC22 += a22 + 2*b22;
  fC42 += b42; 

  // *** fCyy+=dy2*sa*sa*r1*r1/(1.- r1*r1);
  // *** fCzz+=d2y*sa*sa*fT*fT/(1.- r1*r1);

  return 1;
}

void AliTPCtrack::ResetCovariance() {
  //------------------------------------------------------------------
  //This function makes a track forget its history :)  
  //------------------------------------------------------------------

  fC00*=10.;
  fC10=0.;  fC11*=10.;
  fC20=0.;  fC21=0.;  fC22*=10.;
  fC30=0.;  fC31=0.;  fC32=0.;  fC33*=10.;
  fC40=0.;  fC41=0.;  fC42=0.;  fC43=0.;  fC44*=10.;

}


////////////////////////////////////////////////////////////////////////
// MI ADDITION

Float_t AliTPCtrack::Density(Int_t row0, Int_t row1)
{
  //
  // calculate cluster density
  Int_t good  = 0;
  Int_t found = 0;
  //if (row0<fFirstPoint) row0 = fFirstPoint;
  if (row1>fLastPoint) row1 = fLastPoint;

  
  for (Int_t i=row0;i<=row1;i++){ 
    //    Int_t index = fClusterIndex[i];
    Int_t index = fIndex[i];
    if (index!=-1)  good++;
    if (index>0)    found++;
  }
  Float_t density=0;
  if (good>0) density = Float_t(found)/Float_t(good);
  return density;
}


Float_t AliTPCtrack::Density2(Int_t row0, Int_t row1)
{
  //
  // calculate cluster density
  Int_t good  = 0;
  Int_t found = 0;
  //  
  for (Int_t i=row0;i<=row1;i++){     
    Int_t index = fIndex[i];
    if (index!=-1)  good++;
    if (index>0)    found++;
  }
  Float_t density=0;
  if (good>0) density = Float_t(found)/Float_t(good);
  return density;
}


Double_t AliTPCtrack::GetZat0() const
{
  //
  // return virtual z - supposing that x = 0
  if (TMath::Abs(fP2)>1) return 0;
  if (TMath::Abs(fX*fP4-fP2)>1) return 0;
  Double_t vz = fP1+fP3/fP4*(asin(-fP2)-asin(fX*fP4-fP2));
  return vz;
}


Double_t AliTPCtrack::GetD(Double_t x, Double_t y) const {
  //------------------------------------------------------------------
  // This function calculates the transverse impact parameter
  // with respect to a point with global coordinates (x,y)
  //------------------------------------------------------------------
  //Double_t xt=fX, yt=fP0;

  Double_t sn=TMath::Sin(fAlpha), cs=TMath::Cos(fAlpha);
  Double_t a = x*cs + y*sn;
  y = -x*sn + y*cs; x=a;
  //
  Double_t r  = TMath::Abs(1/fP4);
  Double_t x0 = TMath::Abs(fP2*r);
  Double_t y0 = fP0;
  y0= fP0+TMath::Sqrt(1-(fP4*fX-fP2)*(fP4*fX-fP2))/fP4;
  
  Double_t  delta = TMath::Sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));  
  //  Double_t  delta = TMath::Sqrt(TMath::Abs(x*x-2*x0*x+x0*x0+ y*y-2*y*y0+y0*y0));
  delta -= TMath::Abs(r);
  return delta;  
}

//
//

void  AliTPCtrack::UpdatePoints()
{
  //--------------------------------------------------
  //calculates first ,amx dens and last points
  //--------------------------------------------------
  Float_t density[160];
  for (Int_t i=0;i<160;i++) density[i]=-1.;
  fPoints[0]= 160;
  fPoints[1] = -1;
  //
  Int_t ngood=0;
  Int_t undeff=0;
  Int_t nall =0;
  Int_t range=20;
  for (Int_t i=0;i<160;i++){
    Int_t last = i-range;
    if (nall<range) nall++;
    if (last>=0){
      if (fIndex[last]>0&& (fIndex[last]&0x8000)==0) ngood--;
      if (fIndex[last]==-1) undeff--;
    }
    if (fIndex[i]>0&& (fIndex[i]&0x8000)==0)   ngood++;
    if (fIndex[i]==-1) undeff++;
    if (nall==range &&undeff<range/2) density[i-range/2] = Float_t(ngood)/Float_t(nall-undeff);
  }
  Float_t maxdens=0;
  Int_t indexmax =0;
  for (Int_t i=0;i<160;i++){
    if (density[i]<0) continue;
    if (density[i]>maxdens){
      maxdens=density[i];
      indexmax=i;
    }
  }
  //
  //max dens point
  fPoints[3] = maxdens;
  fPoints[1] = indexmax;
  //
  // last point
  for (Int_t i=indexmax;i<160;i++){
    if (density[i]<0) continue;
    if (density[i]<maxdens/2.) {
      break;
    }
    fPoints[2]=i;
  }
  //
  // first point
  for (Int_t i=indexmax;i>0;i--){
    if (density[i]<0) continue;
    if (density[i]<maxdens/2.) {
      break;
    }
    fPoints[0]=i;
  }
  //
}

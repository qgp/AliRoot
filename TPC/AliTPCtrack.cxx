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

/*
$Log$
Revision 1.18  2003/02/19 08:57:04  hristov
Control^M removed

Revision 1.17  2003/02/19 08:49:46  hristov
Track time measurement (S.Radomski)

Revision 1.16  2003/02/06 11:11:36  kowal2
Added a few get methods by Jiri Chudoba

Revision 1.15  2002/11/25 09:33:30  hristov
Tracking of secondaries (M.Ivanov)

Revision 1.14  2002/10/23 13:45:00  hristov
Fatal if no magnetic field set for the reconstruction (Y.Belikov)

Revision 1.13  2002/10/23 07:17:34  alibrary
Introducing Riostream.h

Revision 1.12  2002/10/14 14:57:43  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.9.6.1  2002/10/11 08:34:48  hristov
Updating VirtualMC to v3-09-02

Revision 1.11  2002/07/19 07:34:42  kowal2
Logs added

*/


//-----------------------------------------------------------------
//           Implementation of the TPC track class
//        This class is used by the AliTPCtracker class
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-----------------------------------------------------------------

#include <Riostream.h>

#include "AliTPCtrack.h"
#include "AliCluster.h"

ClassImp(AliTPCtrack)

//_________________________________________________________________________
AliTPCtrack::AliTPCtrack(): AliKalmanTrack() 
{
  fX = fP0 = fP1 = fP2 = fP3 = fP3 = fP4 = 0.0;
  fAlpha = fdEdx = 0.0;
}

//_________________________________________________________________________
AliTPCtrack::AliTPCtrack(UInt_t index, const Double_t xx[5],
const Double_t cc[15], Double_t xref, Double_t alpha) : AliKalmanTrack() {
  //-----------------------------------------------------------------
  // This is the main track constructor.
  //-----------------------------------------------------------------
  fX=xref;
  fAlpha=alpha;
  if (fAlpha<-TMath::Pi()) fAlpha += 2*TMath::Pi();
  if (fAlpha>=TMath::Pi()) fAlpha -= 2*TMath::Pi();
  fdEdx=0.;

  fP0=xx[0]; fP1=xx[1]; fP2=xx[2]; fP3=xx[3]; fP4=xx[4];

  fC00=cc[0];
  fC10=cc[1];  fC11=cc[2];
  fC20=cc[3];  fC21=cc[4];  fC22=cc[5];
  fC30=cc[6];  fC31=cc[7];  fC32=cc[8];  fC33=cc[9];
  fC40=cc[10]; fC41=cc[11]; fC42=cc[12]; fC43=cc[13]; fC44=cc[14];

  fIndex[0]=index;
  SetNumberOfClusters(1);

}

//_____________________________________________________________________________
AliTPCtrack::AliTPCtrack(const AliKalmanTrack& t,Double_t alpha) :
AliKalmanTrack(t) {
  //-----------------------------------------------------------------
  // Conversion AliKalmanTrack -> AliTPCtrack.
  //-----------------------------------------------------------------
  SetChi2(0.);
  SetNumberOfClusters(0);

  fdEdx  = 0.;
  fAlpha = alpha;
  if      (fAlpha < -TMath::Pi()) fAlpha += 2*TMath::Pi();
  else if (fAlpha >= TMath::Pi()) fAlpha -= 2*TMath::Pi();

  //Conversion of the track parameters
  Double_t x,p[5]; t.GetExternalParameters(x,p);
  fX=x;    x=GetConvConst();
  fP0=p[0]; 
  fP1=p[1]; 
  fP3=p[3];
  fP4=p[4]/x; 
  fP2=fP4*fX - p[2];

  //Conversion of the covariance matrix
  Double_t c[15]; t.GetExternalCovariance(c);
  c[10]/=x; c[11]/=x; c[12]/=x; c[13]/=x; c[14]/=x*x;

  Double_t c22=fX*fX*c[14] - 2*fX*c[12] + c[5];
  Double_t c32=fX*c[13] - c[8];
  Double_t c20=fX*c[10] - c[3], c21=fX*c[11] - c[4], c42=fX*c[14] - c[12];
  
  fC00=c[0 ];
  fC10=c[1 ];   fC11=c[2 ];
  fC20=c20;     fC21=c21;     fC22=c22;
  fC30=c[6 ];   fC31=c[7 ];   fC32=c32;   fC33=c[9 ];
  fC40=c[10];   fC41=c[11];   fC42=c42;   fC43=c[13]; fC44=c[14];

}

//_____________________________________________________________________________
AliTPCtrack::AliTPCtrack(const AliTPCtrack& t) : AliKalmanTrack(t) {
  //-----------------------------------------------------------------
  // This is a track copy constructor.
  //-----------------------------------------------------------------
  fX=t.fX;
  fAlpha=t.fAlpha;
  fdEdx=t.fdEdx;

  fP0=t.fP0; fP1=t.fP1; fP2=t.fP2; fP3=t.fP3; fP4=t.fP4;

  fC00=t.fC00;
  fC10=t.fC10;  fC11=t.fC11;
  fC20=t.fC20;  fC21=t.fC21;  fC22=t.fC22;
  fC30=t.fC30;  fC31=t.fC31;  fC32=t.fC32;  fC33=t.fC33;
  fC40=t.fC40;  fC41=t.fC41;  fC42=t.fC42;  fC43=t.fC43;  fC44=t.fC44;

  Int_t n=GetNumberOfClusters();
  for (Int_t i=0; i<n; i++) fIndex[i]=t.fIndex[i];
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
  Double_t a=GetConvConst();

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

//_____________________________________________________________________________
Int_t AliTPCtrack::PropagateTo(Double_t xk,Double_t x0,Double_t rho) {
  //-----------------------------------------------------------------
  // This function propagates a track to a reference plane x=xk.
  //-----------------------------------------------------------------
  if (TMath::Abs(fP4*xk - fP2) >= 0.9) {
    //    Int_t n=GetNumberOfClusters();
    //if (n>4) cerr<<n<<" AliTPCtrack warning: Propagation failed !\n";
    return 0;
  }
  
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

  //Multiple scattering******************
  Double_t d=sqrt((x1-fX)*(x1-fX)+(y1-fP0)*(y1-fP0)+(z1-fP1)*(z1-fP1));
  Double_t p2=(1.+ GetTgl()*GetTgl())/(Get1Pt()*Get1Pt());
  Double_t beta2=p2/(p2 + GetMass()*GetMass());
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

  //Energy losses************************
  Double_t dE=0.153e-3/beta2*(log(5940*beta2/(1-beta2)) - beta2)*d*rho;
  if (x1 < x2) dE=-dE;
  cc=fP4;
  fP4*=(1.- sqrt(p2+GetMass()*GetMass())/p2*dE);
  fP2+=fX*(fP4-cc);

  // Integrated Time [SR, GSI, 17.02.2003]
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
  if (TMath::Abs(cur*fX-eta) >= 0.9) {
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
  
  fX = x1*ca + y1*sa;
  fP0=-x1*sa + y1*ca;
  fP2=fP2*ca + (fP4*y1 + sqrt(1.- r1*r1))*sa;
  
  Double_t r2=fP4*fX - fP2;
  if (TMath::Abs(r2) >= 0.99999) {
    Int_t n=GetNumberOfClusters();
    if (n>4) cerr<<n<<" AliTPCtrack warning: Rotation failed !\n";
    return 0;
  }
  
  Double_t y0=fP0 + sqrt(1.- r2*r2)/fP4;
  if ((fP0-y0)*fP4 >= 0.) {
    Int_t n=GetNumberOfClusters();
    if (n>4) cerr<<n<<" AliTPCtrack warning: Rotation failed !!!\n";
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
Double_t AliTPCtrack::Phi() const {
//
//
//
  Double_t phi =  TMath::ASin(GetSnp()) + fAlpha;
  if (phi<0) phi+=2*TMath::Pi();
  if (phi>=2*TMath::Pi()) phi-=2*TMath::Pi();
  return phi;
}
////////////////////////////////////////////////////////////////////////


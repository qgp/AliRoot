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

//-------------------------------------------------------------------------
//                Implementation of the ITS track class
//
//          Origin: Marian Ivanov, CERN, Marian.Ivanov@cern.ch
//     dEdx analysis by: Boris Batyunya, JINR, Boris.Batiounia@cern.ch
//-------------------------------------------------------------------------

/* $Id$ */

#include <TMatrixD.h>

#include <TMath.h>

#include "AliITSRecoParam.h"
#include "AliCluster.h"
#include "AliESDtrack.h"
#include "AliITSgeomTGeo.h"
#include "AliITStrackMI.h"

ClassImp(AliITStrackMI)

const Int_t kWARN=5;

//____________________________________________________________________________
AliITStrackMI::AliITStrackMI():AliITStrackV2(),
fNUsed(0),
fNSkipped(0),
fNDeadZone(0),			       
fReconstructed(kFALSE),
fExpQ(40),
fChi22(0),
fdEdxMismatch(0),
fConstrain(kFALSE),
fWinner(0),
fGoldV0(kFALSE)

{
  //constructor
    for(Int_t i=0; i<AliITSgeomTGeo::GetNLayers(); i++) fClIndex[i]=-1;
    for(Int_t i=0; i<6; i++) { fNy[i]=0; fNz[i]=0; fNormQ[i]=0; fNormChi2[i]=1000; fDeadZoneProbability[i]=0;}
    for(Int_t i=0; i<12; i++) {fDy[i]=0; fDz[i]=0; fSigmaY[i]=0; fSigmaZ[i]=0; fSigmaYZ[i]=0; fChi2MIP[i]=0;}
    fD[0]=0; fD[1]=0;
    fDnorm[0]=0; fDnorm[1]=0;
}

//____________________________________________________________________________
AliITStrackMI::AliITStrackMI(AliESDtrack& t,Bool_t c):
AliITStrackV2(t,c),
fNUsed(0),
fNSkipped(0),
fNDeadZone(0),
fReconstructed(kFALSE),
fExpQ(40),
fChi22(0),
fdEdxMismatch(0),
fConstrain(kFALSE),
fWinner(0),
fGoldV0(kFALSE) {
  //------------------------------------------------------------------
  // Conversion ESD track -> ITS track.
  // If c==kTRUE, create the ITS track out of the constrained params.
  //------------------------------------------------------------------
  for(Int_t i=0; i<6; i++) {fClIndex[i]=-1; fNy[i]=0; fNz[i]=0; fNormQ[i]=0; fNormChi2[i]=1000; fDeadZoneProbability[i]=0;}
  for(Int_t i=0; i<12; i++) {fDy[i]=0; fDz[i]=0; fSigmaY[i]=0; fSigmaZ[i]=0; fSigmaYZ[i]=0; fChi2MIP[i]=0;}
  fD[0]=0; fD[1]=0;
  fDnorm[0]=0; fDnorm[1]=0;

}

//____________________________________________________________________________
AliITStrackMI::AliITStrackMI(const AliITStrackMI& t) : AliITStrackV2(t),
fNUsed(t.fNUsed),
fNSkipped(t.fNSkipped),
fNDeadZone(t.fNDeadZone),
fReconstructed(t.fReconstructed),
fExpQ(t.fExpQ),
fChi22(t.fChi22),
fdEdxMismatch(t.fdEdxMismatch),
fConstrain(t.fConstrain),
fWinner(0),
fGoldV0(t.fGoldV0) {
  //------------------------------------------------------------------
  //Copy constructor
  //------------------------------------------------------------------
  fLab = t.fLab;
  fFakeRatio = t.fFakeRatio;

  fD[0]=t.fD[0]; fD[1]=t.fD[1];
  fDnorm[0] = t.fDnorm[0]; fDnorm[1]=t.fDnorm[1];
  for(Int_t i=0; i<6; i++) {
    fClIndex[i]= t.fClIndex[i]; fNy[i]=t.fNy[i]; fNz[i]=t.fNz[i]; fNormQ[i]=t.fNormQ[i]; fNormChi2[i] = t.fNormChi2[i];  fDeadZoneProbability[i]=t.fDeadZoneProbability[i];
  }
  for(Int_t i=0; i<12; i++) {fDy[i]=t.fDy[i]; fDz[i]=t.fDz[i]; 
    fSigmaY[i]=t.fSigmaY[i]; fSigmaZ[i]=t.fSigmaZ[i]; fSigmaYZ[i]=t.fSigmaYZ[i]; fChi2MIP[i]=t.fChi2MIP[i];}
  //memcpy(fDy,t.fDy,6*sizeof(Float_t));
  //memcpy(fDz,t.fDz,6*sizeof(Float_t));
  //memcpy(fSigmaY,t.fSigmaY,6*sizeof(Float_t));
  //memcpy(fSigmaZ,t.fSigmaZ,6*sizeof(Float_t));
  //memcpy(fChi2MIP,t.fChi2MIP,12*sizeof(Float_t));  
}

//_____________________________________________________________________________
Int_t AliITStrackMI::Compare(const TObject *o) const {
  //-----------------------------------------------------------------
  // This function compares tracks according to the their curvature
  //-----------------------------------------------------------------
  AliITStrackMI *t=(AliITStrackMI*)o;
  //Double_t co=TMath::Abs(t->Get1Pt());
  //Double_t c =TMath::Abs(Get1Pt());
  Double_t co=t->GetSigmaY2()*t->GetSigmaZ2()*(0.5+TMath::Sqrt(0.5*t->fD[0]*t->fD[0]+t->fD[1]*t->fD[1]));
  Double_t c =GetSigmaY2()*GetSigmaZ2()*(0.5+TMath::Sqrt(0.5*fD[0]*fD[0]+fD[1]*fD[1]));
  if (c>co) return 1;
  else if (c<co) return -1;
  return 0;
}


Double_t AliITStrackMI::GetPredictedChi2MI(Double_t cy, Double_t cz, Double_t cerry, Double_t cerrz, Double_t covyz) const
{
  //-----------------------------------------------------------------
  // This function calculates a predicted chi2 increment.
  //-----------------------------------------------------------------
  Double_t p[2]={cy, cz};
  Double_t cov[3]={cerry*cerry, covyz, cerrz*cerrz};
  return AliExternalTrackParam::GetPredictedChi2(p,cov);
}

//____________________________________________________________________________
Bool_t AliITStrackMI::UpdateMI(const AliCluster *c, Double_t chi2, Int_t index) {
  //------------------------------------------------------------------
  //This function updates track parameters
  //------------------------------------------------------------------
  Double_t dy=c->GetY() - GetY(), dz=c->GetZ() - GetZ();
  Int_t layer = (index & 0xf0000000) >> 28;
  fDy[layer] = dy;
  fDz[layer] = dz;
  fSigmaY[layer] = TMath::Sqrt(c->GetSigmaY2()+GetSigmaY2());
  fSigmaZ[layer] = TMath::Sqrt(c->GetSigmaZ2()+GetSigmaZ2());
  fSigmaYZ[layer] = c->GetSigmaYZ()+GetSigmaZY();


  return Update(c,chi2,index);
}

Int_t AliITStrackMI::GetProlongationFast(Double_t alp, Double_t xk,Double_t &y, Double_t &z)
{
  //-----------------------------------------------------------------------------
  //get fast prolongation 
  //-----------------------------------------------------------------------------
  Double_t ca=TMath::Cos(alp-GetAlpha()), sa=TMath::Sin(alp-GetAlpha());
  Double_t cf=TMath::Sqrt((1.-GetSnp())*(1.+GetSnp()));  
  // **** rotation **********************  
  y= -GetX()*sa + GetY()*ca;
  // **** translation ******************  
  Double_t dx = xk- GetX()*ca - GetY()*sa;
  Double_t f1=GetSnp()*ca - cf*sa, f2=f1 + GetC()*dx;
  if (TMath::Abs(f2) >= 0.9999 || TMath::Abs(f1) >= 0.9999) {
    return 0;
  }
  Double_t r1=TMath::Sqrt((1.-f1)*(1.+f1)), r2=TMath::Sqrt((1.-f2)*(1.+f2));  
  y += dx*(f1+f2)/(r1+r2);
  z  = GetZ()+dx*(f1+f2)/(f1*r2 + f2*r1)*GetTgl();  
  return 1;
}


Bool_t AliITStrackMI::IsGoldPrimary()
{
  //
  // Indicates gold pimary track
  //
  Bool_t isGold=kTRUE;
  if (!fConstrain) return kFALSE;                // 
  if (fNDeadZone+fNDeadZone<5.5) isGold =  kFALSE; // short track
  //
  if (fChi2MIP[0]*fChi2MIP[3]>2) isGold = kFALSE; // RS: cut on chi2*interpolated_chi2
  if (fChi2/Float_t(fN)>2.){
    if (fChi2MIP[0]+fNUsed>3.5) isGold = kFALSE;    
  }
  if (fChi2MIP[2]>4.5) isGold = kFALSE;         //back propagation chi2
  //
  if (fDnorm[0]>0&&fDnorm[1]>0){
    const Float_t distcut2 =2.5*2.5;  //normalize distance  cut 
    Float_t dist2 = fD[0]*fD[0]/(fDnorm[0]*fDnorm[0])+fD[1]*fD[1]/(fDnorm[1]*fDnorm[1]);  //normalize distance to the vertex (pools)
    if (dist2>distcut2) isGold = kFALSE;
  }
  return isGold;
}

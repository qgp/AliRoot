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
*/

#include "AliFastMuonTrackingRes.h"
#include "AliMUONFastTracking.h"
#include <TRandom.h>
#include <TF1.h>

ClassImp(AliFastMuonTrackingRes)


AliFastMuonTrackingRes::AliFastMuonTrackingRes() :
    AliFastResponse("Resolution", "Muon Tracking Resolution")
{
    SetBackground();
}

void AliFastMuonTrackingRes::Init()
{
    fFastTracking = AliMUONFastTracking::Instance();
    fFastTracking->Init(fBackground);
}



void AliFastMuonTrackingRes::Evaluate(Float_t   p,  Float_t  theta , Float_t   phi,
					 Float_t& pS,  Float_t& thetaS, Float_t&  phiS)
{

    Double_t meanp    = fFastTracking->MeanP  (p, theta, phi, Int_t(fCharge));
    Double_t sigmap   = fFastTracking->SigmaP (p, theta, phi, Int_t(fCharge));
    Double_t sigma1p  = fFastTracking->Sigma1P(p, theta, phi, Int_t(fCharge));
    Double_t normg2   = fFastTracking->NormG2 (p, theta, phi, Int_t(fCharge));
    Double_t meang2   = fFastTracking->MeanG2 (p, theta, phi, Int_t(fCharge));
    Double_t sigmag2  = fFastTracking->SigmaG2(p, theta, phi, Int_t(fCharge));
    
    Int_t ip,itheta,iphi;
    TF1* fitp = fFastTracking->GetFitP();
    
    fFastTracking->GetIpIthetaIphi(p, theta, phi, Int_t(fCharge), ip, itheta, iphi);
    if (sigmap == 0) {
	printf ("WARNING!!! sigmap=0:    ");
	printf ("ip= %d itheta = %d iphi = %d   ", ip, itheta, iphi);  
	printf ("p= %f theta = %f phi = %f\n", p, theta, phi);  
    }

    fitp->SetParameters(meanp,sigmap,sigma1p,normg2,meang2,sigmag2);
  
    Double_t meantheta  = fFastTracking->MeanTheta (p, theta, phi, Int_t(fCharge));
    Double_t sigmatheta = fFastTracking->SigmaTheta(p, theta, phi, Int_t(fCharge));
    Double_t meanphi    = fFastTracking->MeanPhi   (p, theta, phi, Int_t(fCharge));
    Double_t sigmaphi   = fFastTracking->SigmaPhi  (p, theta, phi, Int_t(fCharge));
  
    // Components different from ip=0 have the RMS bigger than mean
    Float_t ptp[3]  =  { 1.219576,-0.354764,-0.690117 };
    Float_t ptph[3] =  { 0.977522, 0.016269, 0.023158 }; 
    Float_t pphp[3] =  { 1.303256,-0.464847,-0.869322 };

    // Smeared momentum
    pS   = p + fitp->GetRandom();
    Float_t dp = pS - p; 

    // Smeared phi
    if (ip==0) sigmaphi *= pphp[0] + pphp[1] * dp + pphp[2] * dp*dp; 
    phiS = phi + gRandom->Gaus(meanphi, sigmaphi);
    Float_t dphi = phiS - phi; 
    // Smeared theta
    if (ip==0) sigmatheta *= ptp[0] + ptp[1] * dp + ptp[2] * dp*dp; 
    if (ip==0) sigmatheta *= ptph[0] + ptph[1] * dphi + ptph[2] * dphi*dphi; 
    thetaS = theta + gRandom->Gaus(meantheta,sigmatheta);
}







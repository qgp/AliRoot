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
Revision 1.2  2003/01/08 10:29:33  morsch
Path to data file changed.

Revision 1.1  2003/01/06 10:13:33  morsch
First commit.

*/

#include "AliMUONFastTracking.h"
#include "AliMUONFastTrackingEntry.h"
#include <TMatrixD.h>
#include <TSpline.h>
#include <TFile.h>
#include <TH1.h>
#include <TH3.h>
#include <TF1.h>
#include <TRandom.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Riostream.h>

ClassImp(AliMUONFastTracking)

AliMUONFastTracking* AliMUONFastTracking::fgMUONFastTracking=NULL;

static Double_t FitP(Double_t *x, Double_t *par){
    Double_t dx = x[0] - par[0];
    Double_t dx2 = x[0] - par[4];
    Double_t sigma = par[1] * ( 1 + par[2] * dx);
    if (sigma == 0) {    

	return 0.;
    }
    Double_t fasymm = TMath::Exp(-0.5 * dx * dx / (sigma * sigma));
    Double_t sigma2 = par[1] * par[5];
    Double_t fgauss = TMath::Exp(-0.5 * dx2 * dx2 / (sigma2 * sigma2));
    return TMath::Abs(fasymm + par[3] * fgauss);
} 

AliMUONFastTracking* AliMUONFastTracking::Instance()
{ 
// Set random number generator 
    if (fgMUONFastTracking) {
	return fgMUONFastTracking;
    } else {
	fgMUONFastTracking = new AliMUONFastTracking();
	return fgMUONFastTracking;
    }
}

AliMUONFastTracking::AliMUONFastTracking() 
{
//    SetBackground();
    
    fPrintLevel = 1;  
    // read binning; temporarily put by hand
    Float_t pmin = 0, pmax = 200;
    Int_t   nbinp = 10;
    Float_t thetamin = 2, thetamax = 9;
    Int_t   nbintheta=10;
    Float_t phimin = -180, phimax =180;
    Int_t   nbinphi=10;
    //--------------------------------------

    fNbinp = nbinp;
    fPmin  = pmin;
    fPmax  = pmax;
    
    fNbintheta = nbintheta;
    fThetamin  = thetamin;
    fThetamax  = thetamax;
    
    fNbinphi = nbinphi;
    fPhimin  = phimin;
    fPhimax  = phimax;
    
    fDeltaP     = (fPmax-fPmin)/fNbinp;
    fDeltaTheta = (fThetamax-fThetamin)/fNbintheta;
    fDeltaPhi   = (fPhimax-fPhimin)/fNbinphi;
}

void AliMUONFastTracking::Init(Float_t bkg)
{
  //
  //  Initialization
  //
  for (Int_t ip=0; ip< fNbinp; ip++){
    for (Int_t itheta=0; itheta< fNbintheta; itheta++){
      for (Int_t iphi=0; iphi< fNbinphi; iphi++){
	fCurrentEntry[ip][itheta][iphi] = new AliMUONFastTrackingEntry;
	for (Int_t ibkg=0; ibkg<4; ibkg++){
	  fEntry[ip][itheta][iphi][ibkg] = new AliMUONFastTrackingEntry;
	}
      }
    }
  }
  
  char filename [100]; 
  sprintf (filename,"$(ALICE_ROOT)/FASTSIM/data/MUONtrackLUT.root"); 
  TFile *file = new TFile(filename); 
  ReadLUT(file);
  SetBackground(bkg);
  UseSpline(1);
  fFitp = new TF1("fit1",FitP,-20.,20.,6);
  fFitp->SetNpx(200);    
}


void AliMUONFastTracking::ReadLUT(TFile* file)
{
  TH3F *heff[5][3], *hacc[5][3], *hmeanp, *hsigmap, *hsigma1p, *hchi2p;
  TH3F *hnormg2, *hmeang2, *hsigmag2, *hmeantheta, *hsigmatheta, *hchi2theta;
  TH3F *hmeanphi, *hsigmaphi, *hchi2phi;
  char tag[40], tag2[40]; 
  
  const Float_t bkg[4] = {0, 0.5, 1, 2};
  for (Int_t ibkg=0; ibkg<4; ibkg++) {
    sprintf (tag,"BKG%g",bkg[ibkg]); 
    file->cd(tag);
    for (Int_t isplp = 0; isplp<kSplitP; isplp++) { 
      for (Int_t ispltheta = 0; ispltheta<kSplitTheta; ispltheta++) { 
	sprintf (tag2,"heff[%d][%d]",isplp,ispltheta); 
	heff[isplp][ispltheta] = (TH3F*)gDirectory->Get(tag2);
	sprintf (tag2,"hacc[%d][%d]",isplp,ispltheta); 
	hacc[isplp][ispltheta] = (TH3F*)gDirectory->Get(tag2);
      }
    }    
    hmeanp      = (TH3F*)gDirectory->Get("hmeanp");
    hsigmap     = (TH3F*)gDirectory->Get("hsigmap");
    hsigma1p    = (TH3F*)gDirectory->Get("hsigma1p");
    hchi2p      = (TH3F*)gDirectory->Get("hchi2p");
    hnormg2     = (TH3F*)gDirectory->Get("hnormg2");
    hmeang2     = (TH3F*)gDirectory->Get("hmeang2");
    hsigmag2    = (TH3F*)gDirectory->Get("hsigmag2");
    hmeantheta  = (TH3F*)gDirectory->Get("hmeantheta");
    hsigmatheta = (TH3F*)gDirectory->Get("hsigmatheta");
    hchi2theta  = (TH3F*)gDirectory->Get("hchi2theta");
    hmeanphi    = (TH3F*)gDirectory->Get("hmeanphi");
    hsigmaphi   = (TH3F*)gDirectory->Get("hsigmaphi");
    hchi2phi    = (TH3F*)gDirectory->Get("hchi2phi");
    
    printf ("Reading parameters from LUT file %s...\n",file->GetName());
    for (Int_t ip=0; ip<fNbinp ;ip++) {
      for (Int_t itheta=0; itheta<fNbintheta ;itheta++) {
	for (Int_t iphi=0; iphi<fNbinphi ;iphi++) {
	  Float_t p     = fPmin     + fDeltaP     * (ip     + 0.5);
	  Float_t theta = fThetamin + fDeltaTheta * (itheta + 0.5);
	  Float_t phi   = fPhimin   + fDeltaPhi   * (iphi   + 0.5);
	  
	  fEntry[ip][itheta][iphi][ibkg]->fP          = p;
	  fEntry[ip][itheta][iphi][ibkg]->fMeanp      = 
	    hmeanp->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fSigmap     = 
	    TMath::Abs(hsigmap->GetBinContent(ip+1,itheta+1,iphi+1));
	  fEntry[ip][itheta][iphi][ibkg]->fSigma1p    = 
	    hsigma1p->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fChi2p      = 
	    hchi2p->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fNormG2     = 
	    hnormg2->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fMeanG2     = 
	    hmeang2->GetBinContent(ip+1,itheta+1,iphi+1);
	  if (ibkg == 0)  fEntry[ip][itheta][iphi][ibkg]->fSigmaG2 = 9999;
	  else fEntry[ip][itheta][iphi][ibkg]->fSigmaG2    = 
	    hsigmag2->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fTheta      = theta;
	  fEntry[ip][itheta][iphi][ibkg]->fMeantheta  = 
	    hmeantheta->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fSigmatheta = 
	    TMath::Abs(hsigmatheta->GetBinContent(ip+1,itheta+1,iphi+1));
	  fEntry[ip][itheta][iphi][ibkg]->fChi2theta  = 
	    hchi2theta->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fPhi        = phi;
	  fEntry[ip][itheta][iphi][ibkg]->fMeanphi    = 
	    hmeanphi->GetBinContent(ip+1,itheta+1,iphi+1);
	  fEntry[ip][itheta][iphi][ibkg]->fSigmaphi   = 
	    TMath::Abs(hsigmaphi->GetBinContent(ip+1,itheta+1,iphi+1));
	  fEntry[ip][itheta][iphi][ibkg]->fChi2phi    = 
	    hchi2phi->GetBinContent(ip+1,itheta+1,iphi+1);
	  for (Int_t i=0; i<kSplitP; i++) { 
	    for (Int_t j=0; j<kSplitTheta; j++) { 
	      fEntry[ip][itheta][iphi][ibkg]->fAcc[i][j] = 
		hacc[i][j]->GetBinContent(ip+1,itheta+1,iphi+1);
	      fEntry[ip][itheta][iphi][ibkg]->fEff[i][j] = 
		heff[i][j]->GetBinContent(ip+1,itheta+1,iphi+1);
	    }
	  }
	} // iphi 
      }  // itheta
    }   // ip
  }    // ibkg

  TGraph *graph = new TGraph(3); 
  TF1 *f = new TF1("f","[0]+[1]*x"); 
  
  for (Int_t ip=0; ip< fNbinp; ip++){
    for (Int_t itheta=0; itheta< fNbintheta; itheta++){
      for (Int_t iphi=0; iphi< fNbinphi; iphi++){
	graph->SetPoint(0,0.5,fEntry[ip][itheta][iphi][1]->fSigmaG2);
	graph->SetPoint(1,1,fEntry[ip][itheta][iphi][2]->fSigmaG2);
	graph->SetPoint(2,2,fEntry[ip][itheta][iphi][3]->fSigmaG2);
	graph->Fit("f","q"); 
	fEntry[ip][itheta][iphi][0]->fSigmaG2 = f->Eval(0); 
      }
    }
  }
  f->Delete();
  graph->Delete();
  printf ("parameters read. \n");
}

void AliMUONFastTracking::GetBinning(Int_t &nbinp, Float_t &pmin, Float_t &pmax,
				     Int_t &nbintheta, Float_t &thetamin, 
				     Float_t &thetamax,
				     Int_t &nbinphi, Float_t &phimin, Float_t &phimax)
{
    nbinp = fNbinp;
    pmin  = fPmin;
    pmax  = fPmax;
    nbintheta = fNbintheta;
    thetamin  = fThetamin;
    thetamax  = fThetamax;
    nbinphi = fNbinphi;
    phimin  = fPhimin;
    phimax  = fPhimax;
}


void AliMUONFastTracking::GetIpIthetaIphi(Float_t p, Float_t theta, Float_t phi, 
					  Int_t charge, Int_t &ip, Int_t &itheta, 
					  Int_t &iphi)
{
    if (charge < 0) phi = -phi;
    ip           = Int_t (( p - fPmin ) / fDeltaP);
    itheta       = Int_t (( theta - fThetamin ) / fDeltaTheta);
    iphi         = Int_t (( phi - fPhimin ) / fDeltaPhi);
    
    if (ip< 0) {
	printf ("Warning: ip= %d. Set to 0\n",ip);
	ip = 0;
    } 
    if (ip>= fNbinp) {
      //	printf ("Warning: ip = %d. Set to %d\n",ip,fNbinp-1);
	ip = fNbinp-1;
    } 
    if (itheta< 0) {
      //	printf ("Warning: itheta= %d. Set to 0\n",itheta);
	itheta = 0;
    } 
    if (itheta>= fNbintheta) {
      //	printf ("Warning: itheta = %d. Set to %d\n",itheta,fNbintheta-1);
	itheta = fNbintheta-1;
    } 
    
    if (iphi< 0) {
     	printf ("Warning: iphi= %d. Set to 0\n",iphi);
	iphi = 0;
    } 
    if (iphi>= fNbinphi) {
	printf ("Warning: iphi = %d. Set to %d\n",iphi,fNbinphi-1);
	iphi = fNbinphi-1;
    } 
}

void AliMUONFastTracking::GetSplit(Int_t ip, Int_t itheta, 
				   Int_t &nSplitP, Int_t &nSplitTheta) { 
  if (ip==0) nSplitP = 5; 
  else nSplitP = 2; 
  if (itheta==0) nSplitTheta = 3; 
  else nSplitTheta = 1; 
}

Float_t AliMUONFastTracking::Efficiency(Float_t p,   Float_t theta, 
					Float_t phi, Int_t charge){
  Int_t ip=0, itheta=0, iphi=0;
  GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
  Int_t nSplitP, nSplitTheta; 
  GetSplit(ip,itheta,nSplitP,nSplitTheta); 

  Float_t dp = p - fPmin;
  Int_t ibinp  = Int_t(nSplitP*(dp - fDeltaP * Int_t(dp / fDeltaP))/fDeltaP); 
  Float_t dtheta = theta - fThetamin;
  Int_t ibintheta  = Int_t(nSplitTheta*(dtheta - fDeltaTheta * Int_t(dtheta / fDeltaTheta))/fDeltaTheta); 
  Float_t eff = fCurrentEntry[ip][itheta][iphi]->fEff[ibinp][ibintheta];
  return eff;   
}

Float_t AliMUONFastTracking::Acceptance(Float_t p,   Float_t theta, 
					Float_t phi, Int_t charge){
  if (theta<fThetamin || theta>fThetamax) return 0; 
  
  Int_t ip=0, itheta=0, iphi=0;
  GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
  Int_t nSplitP, nSplitTheta; 
  GetSplit(ip,itheta,nSplitP,nSplitTheta); 
  // central value and corrections with spline 
  
  Float_t dp = p - fPmin;
  Int_t ibinp  = Int_t(nSplitP*(dp - fDeltaP * Int_t(dp / fDeltaP))/fDeltaP); 
  Float_t dtheta = theta - fThetamin;
  Int_t ibintheta  = Int_t(nSplitTheta*(dtheta - fDeltaTheta * Int_t(dtheta / fDeltaTheta))/fDeltaTheta); 
  Float_t acc = fCurrentEntry[ip][itheta][iphi]->fAcc[ibinp][ibintheta];
  return acc;   
}

Float_t AliMUONFastTracking::MeanP(Float_t p,   Float_t theta, 
			    Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    return fCurrentEntry[ip][itheta][iphi]->fMeanp;
}

Float_t AliMUONFastTracking::SigmaP(Float_t p,   Float_t theta, 
				    Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    Int_t index;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    // central value and corrections with spline 
    Float_t sigmap = fCurrentEntry[ip][itheta][iphi]->fSigmap;
    if (!fSpline) return sigmap;
    // corrections vs p, theta, phi 
    index = iphi + fNbinphi * itheta;
    Double_t xmin,ymin,xmax,ymax;
    Float_t frac1 = fSplineSigmap[index][0]->Eval(p)/sigmap; 
    
    if (p>fPmax-fDeltaP/2.) {
	Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fSigmap;
	Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fSigmap;
	Float_t s3 = fCurrentEntry[fNbinp-3][itheta][iphi]->fSigmap;
	Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
	Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
	Float_t p3  = fDeltaP * (fNbinp - 3 + 0.5) + fPmin;
	Float_t p12 = p1 * p1, p22 = p2 * p2, p32 = p3 * p3;
	Float_t d = p12*p2 + p1*p32 + p22*p3 - p32*p2 - p3*p12 - p22*p1;
	Float_t a = (s1*p2 + p1*s3 + s2*p3 - s3*p2 - p3*s1 - s2*p1) / d;
	Float_t b = (p12*s2 + s1*p32 + p22*s3 - p32*s2 - s3*p12 - p22*s1)/d;
	Float_t c = (p12*p2*s3 + p1*p32*s2 + p22*p3*s1 
		     - p32*p2*s1 - p3*p12*s2 - p22*p1*s3) / d;
	Float_t sigma = a * p * p + b * p + c;
  	frac1 = sigma/sigmap;
    }
    index = iphi + fNbinphi * ip;
    fSplineEff[index][1]->GetKnot(0,xmin,ymin);
    fSplineEff[index][1]->GetKnot(9,xmax,ymax);
    if (theta>xmax) theta = xmax;
    Float_t frac2 = fSplineSigmap[index][1]->Eval(theta)/sigmap; 
    index = itheta + fNbintheta * ip;
    fSplineEff[index][2]->GetKnot(0,xmin,ymin);
    fSplineEff[index][2]->GetKnot(9,xmax,ymax);
    if (phi>xmax) phi = xmax;
    Float_t frac3 = fSplineSigmap[index][2]->Eval(phi)/sigmap;
    Float_t sigmatot = sigmap * frac1 * frac2 * frac3;
    if (sigmatot<0) sigmatot = sigmap;
    return sigmatot;
}

Float_t AliMUONFastTracking::Sigma1P(Float_t p,   Float_t theta, 
			    Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    if (p>fPmax) {
	// linear extrapolation of sigmap for p out of range 
	Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fSigma1p;
	Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fSigma1p;
	Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
	Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
	Float_t sigma = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
	return sigma;
    }
    else return fCurrentEntry[ip][itheta][iphi]->fSigma1p;
}

Float_t AliMUONFastTracking::NormG2(Float_t p,   Float_t theta, 
				    Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    if (p>fPmax) {
	// linear extrapolation of sigmap for p out of range 
	Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fNormG2;
	Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fNormG2;
	Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
	Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
	Float_t norm = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
	return norm;
    }
    else return fCurrentEntry[ip][itheta][iphi]->fNormG2;
}

Float_t AliMUONFastTracking::MeanG2(Float_t p,   Float_t theta, 
				    Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    if (p>fPmax) {
	// linear extrapolation of sigmap for p out of range 
	Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fMeanG2;
	Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fMeanG2;
	Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
	Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
	Float_t norm = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
	return norm;
    }
    else return fCurrentEntry[ip][itheta][iphi]->fMeanG2;
}

Float_t AliMUONFastTracking::SigmaG2(Float_t p,   Float_t theta, 
				     Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    if (p>fPmax) {
	// linear extrapolation of sigmap for p out of range 
	Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fSigmaG2;
	Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fSigmaG2;
	Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
	Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
	Float_t sigma = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
	return sigma;
    }
    else return fCurrentEntry[ip][itheta][iphi]->fSigmaG2;
}


Float_t AliMUONFastTracking::MeanTheta(Float_t p,   Float_t theta, 
				       Float_t phi, Int_t charge)
{
    Int_t ip=0, itheta=0, iphi=0;
    GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
    return fCurrentEntry[ip][itheta][iphi]->fMeantheta;
}

Float_t AliMUONFastTracking::SigmaTheta(Float_t p,   Float_t theta, 
			    Float_t phi, Int_t charge){
  Int_t ip=0, itheta=0, iphi=0;
  Int_t index;
  GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
  // central value and corrections with spline 
  Float_t sigmatheta = fCurrentEntry[ip][itheta][iphi]->fSigmatheta;
  if (!fSpline) return sigmatheta;
  // corrections vs p, theta, phi 
  index = iphi + fNbinphi * itheta;
  Double_t xmin,ymin,xmax,ymax;
  Float_t frac1 = fSplineSigmatheta[index][0]->Eval(p)/sigmatheta; 
  if (p>fPmax-fDeltaP/2.) {
    // linear extrapolation of sigmap for p out of range 
    Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fSigmatheta;
    Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fSigmatheta;
    Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
    Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
    Float_t sigma = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
    frac1=sigma/sigmatheta;
  }
  index = iphi + fNbinphi * ip;
  fSplineEff[index][1]->GetKnot(0,xmin,ymin);
  fSplineEff[index][1]->GetKnot(9,xmax,ymax);
  if (theta>xmax) theta = xmax;
  Float_t frac2 = fSplineSigmatheta[index][1]->Eval(theta)/sigmatheta; 
  index = itheta + fNbintheta * ip;
  fSplineEff[index][2]->GetKnot(0,xmin,ymin);
  fSplineEff[index][2]->GetKnot(9,xmax,ymax);
  if (phi>xmax) phi = xmax;
  Float_t frac3 = fSplineSigmatheta[index][2]->Eval(phi)/sigmatheta;
  return sigmatheta * frac1 * frac2 * frac3;
}


Float_t AliMUONFastTracking::MeanPhi(Float_t p,   Float_t theta, 
			    Float_t phi, Int_t charge){
  Int_t ip=0, itheta=0, iphi=0;
  GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
  return fCurrentEntry[ip][itheta][iphi]->fMeanphi;
}

Float_t AliMUONFastTracking::SigmaPhi(Float_t p,   Float_t theta, 
			    Float_t phi, Int_t charge){
  Int_t ip=0, itheta=0, iphi=0;
  Int_t index;
  GetIpIthetaIphi(p,theta,phi,charge,ip,itheta,iphi);
  // central value and corrections with spline 
  Float_t sigmaphi = fCurrentEntry[ip][itheta][iphi]->fSigmaphi;
  if (!fSpline) return sigmaphi;
  // corrections vs p, theta, phi 
  index = iphi + fNbinphi * itheta;
  Float_t frac1 = fSplineSigmaphi[index][0]->Eval(p)/sigmaphi; 
  Double_t xmin,ymin,xmax,ymax;
  if (p>fPmax-fDeltaP/2.) {
    Float_t s1 = fCurrentEntry[fNbinp-1][itheta][iphi]->fSigmaphi;
    Float_t s2 = fCurrentEntry[fNbinp-2][itheta][iphi]->fSigmaphi;
    Float_t p1  = fDeltaP * (fNbinp - 1 + 0.5) + fPmin;
    Float_t p2  = fDeltaP * (fNbinp - 2 + 0.5) + fPmin;
    Float_t sigma = 1./(p1-p2) * ( (s1-s2)*p + (s2-s1)*p1 + s1*(p1-p2) ); 
    frac1 = sigma/sigmaphi;
  }
  
  index = iphi + fNbinphi * ip;
  fSplineEff[index][1]->GetKnot(0,xmin,ymin);
  fSplineEff[index][1]->GetKnot(9,xmax,ymax);
  if (theta>xmax) theta = xmax;
  Float_t frac2 = fSplineSigmaphi[index][1]->Eval(theta)/sigmaphi; 
  index = itheta + fNbintheta * ip;
  fSplineEff[index][2]->GetKnot(0,xmin,ymin);
  fSplineEff[index][2]->GetKnot(9,xmax,ymax);
  if (phi>xmax) phi = xmax;
  Float_t frac3 = fSplineSigmaphi[index][2]->Eval(phi)/sigmaphi;
  return sigmaphi * frac1 * frac2 * frac3;
}

void AliMUONFastTracking::SetSpline(){
  printf ("Setting spline functions...");
  char splname[40];
  Double_t x[20][3];
  Double_t x2[50][3];
  Int_t nbins[3] = {fNbinp, fNbintheta, fNbinphi};
  Double_t xspl[20],yeff[50],ysigmap[20],ysigma1p[20];
  Double_t yacc[50], ysigmatheta[20],ysigmaphi[20];
  Double_t xsp2[50];
  // let's calculate the x axis for p, theta, phi
  
  Int_t i, ispline, ivar;
  for (i=0; i< fNbinp; i++) x[i][0] = fPmin + fDeltaP * (i + 0.5);
  for (i=0; i< fNbintheta; i++) x[i][1] = fThetamin + fDeltaTheta * (i + 0.5);
  for (i=0; i< fNbinphi; i++) x[i][2] = fPhimin + fDeltaPhi * (i + 0.5);
  
  for (i=0; i< 5 * fNbinp; i++) x2[i][0] = fPmin + fDeltaP * (i + 0.5)/5.;
  for (i=0; i< 5 * fNbintheta; i++) x2[i][1] = fThetamin + fDeltaTheta * (i + 0.5)/5.;
  for (i=0; i< 5 * fNbinphi; i++) x2[i][2] = fPhimin + fDeltaPhi * (i + 0.5)/5.;
  
  // splines in p
  ivar = 0; 
  for (i=0; i<nbins[ivar]; i++) xspl[i] = x[i][ivar];
  for (i=0; i<5 * nbins[ivar]; i++) xsp2[i] = x2[i][ivar];
  ispline=0;
  for (Int_t itheta=0; itheta< fNbintheta; itheta++){
    for (Int_t iphi=0; iphi< fNbinphi; iphi++){
      for (Int_t ip=0; ip<fNbinp; ip++) {
	//	for (Int_t i=0; i<5; i++) { 
	//	  yeff[5 * ip + i]  = fCurrentEntry[ip][itheta][iphi]->fEff[i];
	//	  yacc[5 * ip + i]  = fCurrentEntry[ip][itheta][iphi]->fAcc[i];
	//	}
	ysigmap[ip]     = fCurrentEntry[ip][itheta][iphi]->fSigmap;
	ysigma1p[ip]    = fCurrentEntry[ip][itheta][iphi]->fSigma1p;
	ysigmatheta[ip] = fCurrentEntry[ip][itheta][iphi]->fSigmatheta;
	ysigmaphi[ip]   = fCurrentEntry[ip][itheta][iphi]->fSigmaphi;
      }
      if (fPrintLevel>3) cout << " creating new spline " << splname << endl;
      sprintf (splname,"fSplineEff[%d][%d]",ispline,ivar);
      fSplineEff[ispline][ivar] = new TSpline3(splname,xsp2,yeff,5 * nbins[ivar]);
      sprintf (splname,"fSplineAcc[%d][%d]",ispline,ivar);
      fSplineAcc[ispline][ivar] = new TSpline3(splname,xsp2,yacc,5 * nbins[ivar]);
      sprintf (splname,"fSplineSigmap[%d][%d]",ispline,ivar);
      fSplineSigmap[ispline][ivar] = new TSpline3(splname,xspl,ysigmap,nbins[ivar]);
      sprintf (splname,"fSplineSigma1p[%d][%d]",ispline,ivar);
      fSplineSigma1p[ispline][ivar] = new TSpline3(splname,xspl,ysigma1p,nbins[ivar]);
      sprintf (splname,"fSplineSigmatheta[%d][%d]",ispline,ivar);
      fSplineSigmatheta[ispline][ivar] = new TSpline3(splname,xspl,ysigmatheta,nbins[ivar]);
      sprintf (splname,"fSplineSigmaphi[%d][%d]",ispline,ivar);
      fSplineSigmaphi[ispline][ivar] = new TSpline3(splname,xspl,ysigmaphi,nbins[ivar]);
      ispline++;
    }
  }

  ivar = 1; 
  for (i=0; i<nbins[ivar]; i++) xspl[i] = x[i][ivar];
  ispline=0;
  for (Int_t ip=0; ip<fNbinp; ip++) {
    for (Int_t iphi=0; iphi< fNbinphi; iphi++){
      for (Int_t itheta=0; itheta< fNbintheta; itheta++){
	// for efficiency and acceptance let's take the central value
	//	yeff[itheta]        = fCurrentEntry[ip][itheta][iphi]->fEff[2];
	//	yacc[itheta]        = fCurrentEntry[ip][itheta][iphi]->fAcc[2];
	ysigmap[itheta]     = fCurrentEntry[ip][itheta][iphi]->fSigmap;
	ysigma1p[itheta]    = fCurrentEntry[ip][itheta][iphi]->fSigma1p;
	ysigmatheta[itheta] = fCurrentEntry[ip][itheta][iphi]->fSigmatheta;
	ysigmaphi[itheta]   = fCurrentEntry[ip][itheta][iphi]->fSigmaphi;
      }
      if (fPrintLevel>3) cout << " creating new spline " << splname << endl;
      sprintf (splname,"fSplineEff[%d][%d]",ispline,ivar);
      fSplineEff[ispline][ivar] = new TSpline3(splname,xspl,yeff, nbins[ivar]);
      sprintf (splname,"fSplineAcc[%d][%d]",ispline,ivar);
      fSplineAcc[ispline][ivar] = new TSpline3(splname,xspl,yacc, nbins[ivar]);
      sprintf (splname,"fSplineSigmap[%d][%d]",ispline,ivar);
      fSplineSigmap[ispline][ivar] = new TSpline3(splname,xspl,ysigmap,nbins[ivar]);
      sprintf (splname,"fSplineSigma1p[%d][%d]",ispline,ivar);
      fSplineSigma1p[ispline][ivar] = new TSpline3(splname,xspl,ysigma1p,nbins[ivar]);
      sprintf (splname,"fSplineSigmatheta[%d][%d]",ispline,ivar);
      fSplineSigmatheta[ispline][ivar] = new TSpline3(splname,xspl,ysigmatheta,nbins[ivar]);
      sprintf (splname,"fSplineSigmaphi[%d][%d]",ispline,ivar);
      fSplineSigmaphi[ispline][ivar] = new TSpline3(splname,xspl,ysigmaphi,nbins[ivar]);
      ispline++;
    }
  }

  ivar = 2; 
  for (i=0; i<nbins[ivar]; i++) xspl[i] = x[i][ivar];
  ispline=0;
  for (Int_t ip=0; ip<fNbinp; ip++) {
    for (Int_t itheta=0; itheta< fNbintheta; itheta++){
      for (Int_t iphi=0; iphi< fNbinphi; iphi++){
	// for efficiency and acceptance let's take the central value
	//	yeff[iphi]        = fCurrentEntry[ip][itheta][iphi]->fEff[2];
	//	yacc[iphi]        = fCurrentEntry[ip][itheta][iphi]->fAcc[2];
	ysigmap[iphi]     = fCurrentEntry[ip][itheta][iphi]->fSigmap;
	ysigma1p[iphi]    = fCurrentEntry[ip][itheta][iphi]->fSigma1p;
	ysigmatheta[iphi] = fCurrentEntry[ip][itheta][iphi]->fSigmatheta;
	ysigmaphi[iphi]   = fCurrentEntry[ip][itheta][iphi]->fSigmaphi;
      }
      if (fPrintLevel>3) cout << " creating new spline " << splname << endl;
      sprintf (splname,"fSplineEff[%d][%d]",ispline,ivar);
      fSplineEff[ispline][ivar] = new TSpline3(splname,xspl,yeff, nbins[ivar]);
      sprintf (splname,"fSplineAcc[%d][%d]",ispline,ivar);
      fSplineAcc[ispline][ivar] = new TSpline3(splname,xspl,yacc, nbins[ivar]);
      sprintf (splname,"fSplineSigmap[%d][%d]",ispline,ivar);
      fSplineSigmap[ispline][ivar] = new TSpline3(splname,xspl,ysigmap,nbins[ivar]);
      sprintf (splname,"fSplineSigma1p[%d][%d]",ispline,ivar);
      fSplineSigma1p[ispline][ivar] = new TSpline3(splname,xspl,ysigma1p,nbins[ivar]);
      sprintf (splname,"fSplineSigmatheta[%d][%d]",ispline,ivar);
      fSplineSigmatheta[ispline][ivar] = new TSpline3(splname,xspl,ysigmatheta,nbins[ivar]);
      sprintf (splname,"fSplineSigmaphi[%d][%d]",ispline,ivar);
      fSplineSigmaphi[ispline][ivar] = new TSpline3(splname,xspl,ysigmaphi,nbins[ivar]);
      ispline++;
    }
  }
  printf ("...done\n");
}
  
void AliMUONFastTracking::SmearMuon(Float_t pgen, Float_t thetagen, Float_t phigen, 
			    Int_t charge, Float_t &psmear, Float_t &thetasmear,
			    Float_t &phismear, Float_t &eff, Float_t &acc){

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // IMPORTANT NOTICE TO THE USER
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // THIS METHOD HAS BEEN REPLACED BY AliFastMuonTrackingEff::Evaluate()
  // AND WILL BE DELETED SOON
  // DO NOT USE THIS METHOD
  // 

  printf ("AliMUONFastTracking::SmearMuon()   THIS METHOD IS OBSOLETE ");
  printf ("PLEASE REFER TO AliFastMuonTrackingEff::Evaluate()\n");
  // angles are in degrees   
  
  Double_t meanp    = MeanP  (pgen, thetagen, phigen, charge);
  Double_t sigmap   = SigmaP (pgen, thetagen, phigen, charge);
  Double_t sigma1p  = Sigma1P(pgen, thetagen, phigen, charge);
  Double_t normg2   = NormG2 (pgen, thetagen, phigen, charge);
  Double_t meang2   = MeanG2 (pgen, thetagen, phigen, charge);
  Double_t sigmag2  = SigmaG2(pgen, thetagen, phigen, charge);

  printf ("fBkg = %g    normg2 (100,5,0,1) = %g \n",fBkg,NormG2(100,5,0,1));
  printf ("fBkg = %g    meang2 (100,5,0,1) = %g \n",fBkg,MeanG2(100,5,0,1));
  printf ("fBkg = %g    sigmag2 (100,5,0,1) = %g \n",fBkg,SigmaG2(100,5,0,1));
  Int_t ip,itheta,iphi;
  GetIpIthetaIphi(pgen, thetagen, phigen, charge, ip, itheta, iphi);
  if (sigmap == 0) {
    if (fPrintLevel>0) {
      printf ("WARNING!!! sigmap=0:    ");
      printf ("ip= %d itheta = %d iphi = %d   ", ip, itheta, iphi);  
      printf ("p= %f theta = %f phi = %f\n", pgen, thetagen, phigen);  
    }
  }
  
  if (fPrintLevel>1) printf ("setting parameters: meanp = %f sigmap = %f sigma1p = %f normg2 = %f meang2 = %f sigmag2 = %f \n",meanp,sigmap,sigma1p,normg2,meang2,sigmag2);
  fFitp->SetParameters(meanp,sigmap,sigma1p,normg2,meang2,sigmag2);
  
  Double_t meantheta  = MeanTheta (pgen, thetagen, phigen, charge);
  Double_t sigmatheta = SigmaTheta(pgen, thetagen, phigen, charge);
  Double_t meanphi    = MeanPhi   (pgen, thetagen, phigen, charge);
  Double_t sigmaphi   = SigmaPhi  (pgen, thetagen, phigen, charge);
  
  // components different from ip=0 have the RMS bigger than mean
  Float_t ptp[3]  =  { 1.219576,-0.354764,-0.690117 };
  Float_t ptph[3] =  { 0.977522, 0.016269, 0.023158 }; 
  Float_t pphp[3] =  { 1.303256,-0.464847,-0.869322 };
  psmear   = pgen + fFitp->GetRandom();
  Float_t dp = psmear - pgen; 
  if (ip==0) sigmaphi *= pphp[0] + pphp[1] * dp + pphp[2] * dp*dp; 
  phismear = phigen + gRandom->Gaus(meanphi, sigmaphi);
  Float_t dphi = phismear - phigen; 

  if (ip==0) sigmatheta *= ptp[0] + ptp[1] * dp + ptp[2] * dp*dp; 
  if (ip==0) sigmatheta *= ptph[0] + ptph[1] * dphi + ptph[2] * dphi*dphi; 
  thetasmear = thetagen + gRandom->Gaus(meantheta,sigmatheta);
  eff      = Efficiency(pgen, thetagen, phigen, charge);
  acc      = Acceptance(pgen, thetagen, phigen, charge);
}

void AliMUONFastTracking::SetBackground(Float_t bkg){
  // linear interpolation of the parameters in the LUT between 2 values where
  // the background has been actually calculated

  if (bkg>2) printf ("WARNING: unsafe extrapolation!\n");
  fBkg = bkg;

  Float_t BKG[4] = {0, 0.5, 1, 2}; // bkg values for which LUT is calculated
  Int_t ibkg;
  for (ibkg=0; ibkg<4; ibkg++) if ( bkg < BKG[ibkg]) break;
  if (ibkg == 4) ibkg--;
  if (ibkg == 0) ibkg++;
  
  Float_t x0 = BKG[ibkg-1];
  Float_t x1 = BKG[ibkg];
  Float_t x = (bkg - x0) / (x1 - x0); 
  
  Float_t y0, y1;

  for (Int_t ip=0; ip< fNbinp; ip++){
    for (Int_t itheta=0; itheta< fNbintheta; itheta++){
      for (Int_t iphi=0; iphi< fNbinphi; iphi++){
	fCurrentEntry[ip][itheta][iphi]->fP = fEntry[ip][itheta][iphi][ibkg]->fP;
	fCurrentEntry[ip][itheta][iphi]->fTheta = fEntry[ip][itheta][iphi][ibkg]->fTheta;
	fCurrentEntry[ip][itheta][iphi]->fPhi = fEntry[ip][itheta][iphi][ibkg]->fPhi;
	fCurrentEntry[ip][itheta][iphi]->fChi2p = -1;
	fCurrentEntry[ip][itheta][iphi]->fChi2theta = -1;
	fCurrentEntry[ip][itheta][iphi]->fChi2phi = -1;

	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fMeanp;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fMeanp;
	fCurrentEntry[ip][itheta][iphi]      ->fMeanp = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fMeantheta;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fMeantheta;
	fCurrentEntry[ip][itheta][iphi]      ->fMeantheta = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fMeanphi;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fMeanphi;
	fCurrentEntry[ip][itheta][iphi]      ->fMeanphi = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fSigmap;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fSigmap;
	fCurrentEntry[ip][itheta][iphi]      ->fSigmap = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fSigmatheta;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fSigmatheta;
	fCurrentEntry[ip][itheta][iphi]      ->fSigmatheta = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fSigmaphi;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fSigmaphi;
	fCurrentEntry[ip][itheta][iphi]      ->fSigmaphi = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fSigma1p;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fSigma1p;
	fCurrentEntry[ip][itheta][iphi]      ->fSigma1p = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fNormG2;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fNormG2;
	fCurrentEntry[ip][itheta][iphi]      ->fNormG2 = (y1 - y0) * x + y0;
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fMeanG2;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fMeanG2;
	fCurrentEntry[ip][itheta][iphi]      ->fMeanG2 = (y1 - y0) * x + y0;
	
	y0 = fEntry[ip][itheta][iphi][ibkg-1]->fSigmaG2;
	y1 =   fEntry[ip][itheta][iphi][ibkg]->fSigmaG2;
	fCurrentEntry[ip][itheta][iphi]      ->fSigmaG2 = (y1 - y0) * x + y0;
	for (Int_t i=0; i<kSplitP; i++) {
	  for (Int_t j=0; j<kSplitTheta; j++) {
	    fCurrentEntry[ip][itheta][iphi]->fAcc[i][j] = fEntry[ip][itheta][iphi][ibkg]->fAcc[i][j];
	    y0 = fEntry[ip][itheta][iphi][ibkg-1]->fEff[i][j];
	    y1 =   fEntry[ip][itheta][iphi][ibkg]->fEff[i][j];
	    fCurrentEntry[ip][itheta][iphi]->fEff[i][j] = (y1 - y0) * x + y0;
	  }
	}
      }
    }
  }
  SetSpline();
}



  // to guarantee a safe extrapolation for sigmag2 to 0<bkg<0.5, let's fit 
  // with a straight line sigmag2 vs bkg for bkg=0.5, 1 and 2, and put the 
  // sigma2(BKG=0) as the extrapolation of this fit 









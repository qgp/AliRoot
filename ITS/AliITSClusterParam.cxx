/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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
//                                                                           //
//  ITS cluster error and shape parameterization                             //
//                                                                           //
//  andrea.dainese@lnl.infn.it                                               //
///////////////////////////////////////////////////////////////////////////////
//#include "TFile.h"
//#include "TTree.h"
//#include <TVectorF.h>
//#include <TLinearFitter.h>
//#include <TH1F.h>
//#include <TProfile2D.h>
#include <TVector3.h>
#include "TMath.h"
#include "AliGeomManager.h"
#include "AliITSRecPoint.h"
#include "AliITSClusterParam.h"
#include "AliITSReconstructor.h"
#include "AliExternalTrackParam.h"

ClassImp(AliITSClusterParam)


AliITSClusterParam* AliITSClusterParam::fgInstance = 0;


/*
  Example usage fitting parameterization (NOT YET...):
  TFile fres("resol.root");    //tree with resolution and shape 
  TTree * treeRes =(TTree*)fres.Get("Resol");
  
  AliITSClusterParam param;
  param.SetInstance(&param);
  param.FitResol(treeRes);
  param.FitRMS(treeRes);
  TFile fparam("ITSClusterParam.root","recreate");
  param.Write("Param");
  //
  //
  TFile fparam("ITSClusterParam.root");
  AliITSClusterParam *param2  =  (AliITSClusterParam *) fparam.Get("Param"); 
  param2->SetInstance(param2);
  param2->Test(treeRes);
  

  treeRes->Draw("(Resol-AliITSClusterParam::SGetError0(Dim,Pad,Zm,AngleM))/Resol","Dim==0&&QMean<0")
*/

//-------------------------------------------------------------------------
AliITSClusterParam* AliITSClusterParam::Instance()
{
  //
  // Singleton implementation
  // Returns an instance of this class, it is created if neccessary
  //
  if (fgInstance == 0){
    fgInstance = new AliITSClusterParam();
  }
  return fgInstance;
}
//-------------------------------------------------------------------------
void AliITSClusterParam::GetNTeor(Int_t layer,const AliITSRecPoint* /*cl*/, 
				  Float_t tgl,Float_t tgphitr,
				  Float_t &ny,Float_t &nz)
{
  //
  // Get "mean shape" (original parametrization from AliITStrackerMI)
  //
  tgl = TMath::Abs(tgl);
  tgphitr = TMath::Abs(tgphitr);

  // SPD
  if (layer==0) {
    ny = 1.+tgphitr*3.2;
    nz = 1.+tgl*0.34;
    return;
  }
  if (layer==1) {
    ny = 1.+tgphitr*3.2;
    nz = 1.+tgl*0.28;
    return;
  }
  // SSD
  if (layer==4 || layer==5) {
    ny = 2.02+tgphitr*1.95;
    nz = 2.02+tgphitr*2.35;
    return;
  }
  // SDD
  ny  = 6.6-2.7*tgphitr;
  nz  = 2.8-3.11*tgphitr+0.45*tgl;
  return;
}
//--------------------------------------------------------------------------
Int_t AliITSClusterParam::GetError(Int_t layer,
				   const AliITSRecPoint *cl,
				   Float_t tgl,Float_t tgphitr,Float_t expQ,
				   Float_t &erry,Float_t &errz,
				   Bool_t addMisalErr)
{
  //
  // Calculate cluster position error
  //
  Int_t retval=0;
  switch(AliITSReconstructor::GetRecoParam()->GetClusterErrorsParam()) {
  case 0: 
    retval = GetErrorOrigRecPoint(cl,erry,errz);
    break;
  case 1: 
    retval = GetErrorParamMI(layer,cl,tgl,tgphitr,expQ,erry,errz);
    break;
  case 2: 
    retval = GetErrorParamAngle(layer,cl,tgl,tgphitr,erry,errz);
    break;
  default: 
    retval = GetErrorParamMI(layer,cl,tgl,tgphitr,expQ,erry,errz);
    break;
  }

  if(addMisalErr) {
    // add error due to misalignment (to be improved)
    Float_t errmisal2 = AliITSReconstructor::GetRecoParam()->GetClusterMisalError()
      *AliITSReconstructor::GetRecoParam()->GetClusterMisalError();
    erry = TMath::Sqrt(erry*erry+errmisal2);
    errz = TMath::Sqrt(errz*errz+errmisal2);
  }

  return retval;

}
//--------------------------------------------------------------------------
Int_t AliITSClusterParam::GetErrorOrigRecPoint(const AliITSRecPoint*cl,
				   Float_t &erry,Float_t &errz)
{
  //
  // Calculate cluster position error (just take error from AliITSRecPoint)
  //
  erry   = TMath::Sqrt(cl->GetSigmaY2()); 
  errz   = TMath::Sqrt(cl->GetSigmaZ2()); 

  return 1;
}
//--------------------------------------------------------------------------
Int_t AliITSClusterParam::GetErrorParamMI(Int_t layer,const AliITSRecPoint*cl,
					  Float_t tgl,Float_t tgphitr,
					  Float_t expQ,
					  Float_t &erry,Float_t &errz)
{
  //
  // Calculate cluster position error (original parametrization from 
  // AliITStrackerMI)
  //
  Float_t nz,ny;
  GetNTeor(layer, cl,tgl,tgphitr,ny,nz);  
  erry   = TMath::Sqrt(cl->GetSigmaY2()); 
  errz   = TMath::Sqrt(cl->GetSigmaZ2()); 
  //
  // PIXELS
  if (layer<2){
    if (TMath::Abs(ny-cl->GetNy())>0.6)  {
      if (ny<cl->GetNy()){
	erry*=0.4+TMath::Abs(ny-cl->GetNy());
	errz*=0.4+TMath::Abs(ny-cl->GetNy());
      }else{
	erry*=0.7+0.5*TMath::Abs(ny-cl->GetNy());
	errz*=0.7+0.5*TMath::Abs(ny-cl->GetNy());
      }
    }
    if (TMath::Abs(nz-cl->GetNz())>1.)  {
      erry*=TMath::Abs(nz-cl->GetNz());
      errz*=TMath::Abs(nz-cl->GetNz());	      
    }
    erry*=0.85;
    errz*=0.85;
    erry= TMath::Min(erry,float(0.0050));
    errz= TMath::Min(errz,float(0.0300));
    return 10;
  }

  //STRIPS
  if (layer>3){ 
    //factor 1.8 appears in new simulation
    //
    Float_t scale=1.8;
    if (cl->GetNy()==100||cl->GetNz()==100){
      erry = 0.004*scale;
      errz = 0.2*scale;
      return 100;
    }
    if (cl->GetNy()+cl->GetNz()>12){
      erry = 0.06*scale;
      errz = 0.57*scale;
      return 100;
    }
    Float_t normq = cl->GetQ()/(TMath::Sqrt(1+tgl*tgl+tgphitr*tgphitr));
    Float_t chargematch = TMath::Max(double(normq/expQ),2.);
    //
    if (cl->GetType()==1 || cl->GetType()==10 ){     							       
      if (chargematch<1.0 || (cl->GetNy()+cl->GetNz()<nz+ny+0.5)){
	errz = 0.043*scale;
	erry = 0.00094*scale;
	return 101;
      }
      if (cl->GetNy()+cl->GetNz()<nz+ny+1.2){
	errz = 0.06*scale;
	erry =0.0013*scale;
	return 102;
      }
      erry = 0.0027*scale;
      errz = TMath::Min(0.028*(chargematch+cl->GetNy()+cl->GetNz()-nz+ny),0.15)*scale;
      return 103;
    }
    if (cl->GetType()==2 || cl->GetType()==11 ){ 
      erry = TMath::Min(0.0010*(1+chargematch+cl->GetNy()+cl->GetNz()-nz+ny),0.05)*scale;
      errz = TMath::Min(0.025*(1+chargematch+cl->GetNy()+cl->GetNz()-nz+ny),0.5)*scale;
      return 104;
    }
    
    if (cl->GetType()>100 ){     							       
      if ((chargematch+cl->GetNy()+cl->GetNz()-nz-ny<1.5)){
	errz = 0.05*scale;
	erry = 0.00096*scale;
	return 105;
      }
      if (cl->GetNy()+cl->GetNz()-nz-ny<1){
	errz = 0.10*scale;
	erry = 0.0025*scale;
	return 106;
      }

      errz = TMath::Min(0.05*(chargematch+cl->GetNy()+cl->GetNz()-nz-ny),0.4)*scale;
      erry = TMath::Min(0.003*(chargematch+cl->GetNy()+cl->GetNz()-nz-ny),0.05)*scale;
      return 107;
    }    
    Float_t diff = cl->GetNy()+cl->GetNz()-ny-nz;
    if (diff<1) diff=1;
    if (diff>4) diff=4;
        
    if (cl->GetType()==5||cl->GetType()==6||cl->GetType()==7||cl->GetType()==8){
      errz = 0.14*diff;
      erry = 0.003*diff;
      return 108;
    }  
    erry = 0.04*diff;
    errz = 0.06*diff;
    return 109;
  }
  //DRIFTS
  Float_t normq = cl->GetQ()/(TMath::Sqrt(1+tgl*tgl+tgphitr*tgphitr));
  Float_t chargematch = normq/expQ;
  chargematch/=2.4; // F. Prino Sept. 2007: SDD charge conversion keV->ADC
  Float_t factorz=1;
  Int_t   cnz = cl->GetNz()%10;
  //charge match
  if (cl->GetType()==1){
    if (chargematch<1.25){
      erry =  0.0028*(1.+6./cl->GetQ());  // gold clusters
    }
    else{
      erry = 0.003*chargematch;
      if (cl->GetNz()==3) erry*=1.5;
    }
    if (chargematch<1.0){
      errz =  0.0011*(1.+6./cl->GetQ());
    }
    else{
      errz = 0.002*(1+2*(chargematch-1.));
    }
    if (cnz>nz+0.6) {
      erry*=(cnz-nz+0.5);
      errz*=1.4*(cnz-nz+0.5);
    }
  }
  if (cl->GetType()>1){
    if (chargematch<1){
      erry =  0.00385*(1.+6./cl->GetQ());  // gold clusters
      errz =  0.0016*(1.+6./cl->GetQ());
    }
    else{
      errz = 0.0014*(1+3*(chargematch-1.));
      erry = 0.003*(1+3*(chargematch-1.));
    } 
    if (cnz>nz+0.6) {
      erry*=(cnz-nz+0.5);
      errz*=1.4*(cnz-nz+0.5);
    }
  }

  if (TMath::Abs(cl->GetY())>2.5){
    factorz*=1+2*(TMath::Abs(cl->GetY())-2.5);
  }
  if (TMath::Abs(cl->GetY())<1){
    factorz*=1.+0.5*TMath::Abs(TMath::Abs(cl->GetY())-1.);
  }
  factorz= TMath::Min(factorz,float(4.));  
  errz*=factorz;

  erry= TMath::Min(erry,float(0.05));
  errz= TMath::Min(errz,float(0.05));  
  return 200;
}
//--------------------------------------------------------------------------
Int_t AliITSClusterParam::GetErrorParamAngle(Int_t layer,
					     const AliITSRecPoint *cl,
					     Float_t tgl,Float_t tgphitr,
					     Float_t &erry,Float_t &errz)
{
  //
  // Calculate cluster position error (parametrization extracted from rp-hit
  // residuals, as a function of angle between track and det module plane.
  // Origin: M.Lunardon, S.Moretto)
  //

  Double_t maxSigmaSPDx=100.;
  Double_t maxSigmaSPDz=400.;
  Double_t maxSigmaSDDx=100.;
  Double_t maxSigmaSDDz=400.;
  Double_t maxSigmaSSDx=100.;
  Double_t maxSigmaSSDz=1000.;
  
  Double_t paramSPDx[3]={-6.417,0.18,11.14};
  Double_t paramSPDz[2]={118.,-0.155};
  Double_t paramSDDx[2]={30.93,0.059};
  Double_t paramSDDz[2]={33.09,0.011};
  Double_t paramSSDx[2]={18.64,-0.0046};
  Double_t paramSSDz[2]={784.4,-0.828};
  Double_t sigmax=1000.0,sigmaz=1000.0;
  
  Int_t volId = (Int_t)cl->GetVolumeId();
  Double_t tra[3]; AliGeomManager::GetOrigTranslation(volId,tra);
  Double_t rot[9]; AliGeomManager::GetOrigRotation(volId,rot);


  Double_t phitr = TMath::ATan(tgphitr);
  Double_t alpha = TMath::ATan2(tra[1],tra[0]);
  Double_t phiglob = alpha+phitr;
  Double_t p[3]; 
  p[0] = TMath::Cos(phiglob);
  p[1] = TMath::Sin(phiglob);
  p[2] = tgl;
  TVector3 pvec(p[0],p[1],p[2]);
  TVector3 normvec(rot[1],rot[4],rot[7]);
  Double_t angle = pvec.Angle(normvec);
  if(angle>0.5*TMath::Pi()) angle = TMath::Pi()-angle;
  Double_t angleDeg = angle*180./TMath::Pi();
  
  if(layer==0 || layer==1) { // SPD

    sigmax = TMath::Exp(angleDeg*paramSPDx[1]+paramSPDx[0])+paramSPDx[2];
    sigmaz = paramSPDz[0]+paramSPDz[1]*angleDeg;
    if(sigmax > maxSigmaSPDx) sigmax = maxSigmaSPDx;
    if(sigmaz > maxSigmaSPDz) sigmax = maxSigmaSPDz;

  } else if(layer==2 || layer==3) { // SDD

    sigmax = angleDeg*paramSDDx[1]+paramSDDx[0];
    sigmaz = paramSDDz[0]+paramSDDz[1]*angleDeg;
    if(sigmax > maxSigmaSDDx) sigmax = maxSigmaSDDx;
    if(sigmaz > maxSigmaSDDz) sigmax = maxSigmaSDDz;
    
  } else if(layer==4 || layer==5) { // SSD

    sigmax = angleDeg*paramSSDx[1]+paramSSDx[0];
    sigmaz = paramSSDz[0]+paramSSDz[1]*angleDeg;
    if(sigmax > maxSigmaSSDx) sigmax = maxSigmaSSDx;
    if(sigmaz > maxSigmaSSDz) sigmax = maxSigmaSSDz;
    
  }

  // convert from micron to cm
  erry = 1.e-4*sigmax; 
  errz = 1.e-4*sigmaz;
  
  return 1;
}
//--------------------------------------------------------------------------
void AliITSClusterParam::Print(Option_t* /*option*/) const {
  //
  // Print param Information
  //

  //
  // Error parameterization
  //
  printf("NOT YET...\n");
  return;
}






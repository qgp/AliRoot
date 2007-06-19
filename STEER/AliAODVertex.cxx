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

//-------------------------------------------------------------------------
//     AOD track base class
//     Base class for Analysis Object Data
//     Generic version
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include "AliAODVertex.h"

#include "AliAODTrack.h"

ClassImp(AliAODVertex)

//______________________________________________________________________________
AliAODVertex::AliAODVertex() : 
  TObject(),
  fChi2perNDF(-999.),
  fCovMatrix(NULL),
  fParent(0x0),
  fDaughters(),
  fType(kUndef)
 {
  // default constructor

  fPosition[0] = fPosition[1] = fPosition[2] = -999.;
}

//______________________________________________________________________________
AliAODVertex::AliAODVertex(const Double_t position[3], 
			   const Double_t covMatrix[6],
			   Double_t  chi2perNDF,
			   TObject  *parent,
			   Char_t vtype) :
  TObject(),
  fChi2perNDF(chi2perNDF),
  fCovMatrix(NULL),
  fParent(parent),
  fDaughters(),
  fType(vtype)
{
  // constructor

  SetPosition(position);
  if (covMatrix) SetCovMatrix(covMatrix);
}

//______________________________________________________________________________
AliAODVertex::AliAODVertex(const Float_t position[3], 
			   const Float_t  covMatrix[6],
			   Double_t  chi2perNDF,
			   TObject  *parent,
			   Char_t vtype) :

  TObject(),
  fChi2perNDF(chi2perNDF),
  fCovMatrix(NULL),
  fParent(parent),
  fDaughters(),
  fType(vtype)
{
  // constructor

  SetPosition(position);
  if (covMatrix) SetCovMatrix(covMatrix);
}

//______________________________________________________________________________
AliAODVertex::AliAODVertex(const Double_t position[3], 
			   Double_t  chi2perNDF,
			   Char_t vtype) :
  TObject(),
  fChi2perNDF(chi2perNDF),
  fCovMatrix(NULL),
  fParent(0x0),
  fDaughters(),
  fType(vtype)
{
  // constructor without covariance matrix

  SetPosition(position);  
}

//______________________________________________________________________________
AliAODVertex::AliAODVertex(const Float_t position[3], 
			   Double_t  chi2perNDF,
			   Char_t vtype) :
  TObject(),
  fChi2perNDF(chi2perNDF),
  fCovMatrix(NULL),
  fParent(0x0),
  fDaughters(),
  fType(vtype)
{
  // constructor without covariance matrix

  SetPosition(position);  
}

//______________________________________________________________________________
AliAODVertex::~AliAODVertex() 
{
  // Destructor

  delete fCovMatrix;
}

//______________________________________________________________________________
AliAODVertex::AliAODVertex(const AliAODVertex& vtx) :
  TObject(vtx),
  fChi2perNDF(vtx.fChi2perNDF),
  fCovMatrix(NULL),
  fParent(vtx.fParent),
  fDaughters(vtx.fDaughters),
  fType(vtx.fType)
{
  // Copy constructor.
  
  for (int i = 0; i < 3; i++) 
    fPosition[i] = vtx.fPosition[i];

  if (vtx.fCovMatrix) fCovMatrix=new AliAODRedCov<3>(*vtx.fCovMatrix);
}

//______________________________________________________________________________
AliAODVertex& AliAODVertex::operator=(const AliAODVertex& vtx) 
{
  // Assignment operator
  if (this != &vtx) {

    // name and type
    TObject::operator=(vtx);

    //momentum
    for (int i = 0; i < 3; i++) 
      fPosition[i] = vtx.fPosition[i];
    
    fChi2perNDF = vtx.fChi2perNDF;

    //covariance matrix
    delete fCovMatrix;
    fCovMatrix = NULL;   
    if (vtx.fCovMatrix) fCovMatrix=new AliAODRedCov<3>(*vtx.fCovMatrix);
    
    //other stuff
    fParent = vtx.fParent;
    fDaughters = vtx.fDaughters;
    fType = vtx.fType;
  }
  
  return *this;
}

//______________________________________________________________________________
template <class T> void AliAODVertex::GetSigmaXYZ(T sigma[3]) const
{
  // Return errors on vertex position in thrust frame
  
  if(fCovMatrix) {
    sigma[0]=fCovMatrix[3]; //GetCovXZ
    sigma[1]=fCovMatrix[4]; //GetCovYZ
    sigma[2]=fCovMatrix[5]; //GetCovZZ
  } else 
    sigma[0]=sigma[1]=sigma[2]=-999.;

  /*
  for (int i = 0, j = 6; i < 3; i++) {
    j -= i+1;
    sigma[2-i] = fCovMatrix ? TMath::Sqrt(fCovMatrix[j]) : -999.;
  }
  */
}

//______________________________________________________________________________
Int_t AliAODVertex::GetNContributors() const 
{
  // Returns the number of tracks used to fit this vertex.
  
  Int_t cont = 0;

  for (Int_t iDaug = 0; iDaug < GetNDaughters(); iDaug++) {
    if (((AliAODTrack*)fDaughters.At(iDaug))->GetUsedForVtxFit()) cont++;
  }

  return cont;
}

//______________________________________________________________________________
Bool_t AliAODVertex::HasDaughter(TObject *daughter) const 
{
  // Checks if the given daughter (particle) is part of this vertex.

  TRefArrayIter iter(&fDaughters);
  while (TObject *daugh = iter.Next()) {
    if (daugh == daughter) return kTRUE;
  }
  return kFALSE;
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixXX(Double_t phi, Double_t theta) const
{
  // XX term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  Double_t ct = TMath::Cos(theta);
  Double_t st = TMath::Sin(theta);
  return
     covMatrix[0]*cp*cp*ct*ct  // GetCovXX
    +covMatrix[1]*2.*cp*sp*ct*ct  // GetCovXY
    +covMatrix[3]*2.*cp*ct*st  // GetCovXZ
    +covMatrix[2]*sp*sp*ct*ct  // GetCovYY
    +covMatrix[4]*2.*sp*ct*st  // GetCovYZ
    +covMatrix[5]*st*st;  // GetCovZZ
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixXY(Double_t phi, Double_t theta) const
{
  // XY term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  Double_t ct = TMath::Cos(theta);
  Double_t st = TMath::Sin(theta);
  return 
    -covMatrix[0]*cp*sp*ct  // GetCovXX
    +covMatrix[1]*ct*(cp*cp-sp*sp)  // GetCovXY
    -covMatrix[3]*sp*st  // GetCovXZ
    +covMatrix[2]*cp*sp*ct  // GetCovYY
    +covMatrix[4]*cp*st;  // GetCovYZ
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixXZ(Double_t phi, Double_t theta) const
{
  // XZ term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  Double_t ct = TMath::Cos(theta);
  Double_t st = TMath::Sin(theta);
  return 
    -covMatrix[0]*cp*cp*ct*st  // GetCovXX
    -covMatrix[1]*2.*cp*sp*ct*st  // GetCovXY
    +covMatrix[3]*cp*(ct*ct-st*st)  // GetCovXZ
    -covMatrix[2]*sp*sp*ct*st  // GetCovYY
    +covMatrix[4]*sp*(ct*ct-st*st)  // GetCovYZ
    +covMatrix[5]*ct*st;  // GetCovZZ
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixYY(Double_t phi) const
{
  // YY term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  return
     covMatrix[0]*sp*sp  // GetCovXX
    -covMatrix[1]*2.*cp*sp  // GetCovXY
    +covMatrix[2]*cp*cp;  // GetCovYY
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixYZ(Double_t phi, Double_t theta) const
{
  // YZ term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  Double_t ct = TMath::Cos(theta);
  Double_t st = TMath::Sin(theta);
  return 
     covMatrix[0]*cp*sp*st  // GetCovXX
    +covMatrix[1]*st*(sp*sp-cp*cp)  // GetCovXY
    -covMatrix[3]*sp*ct  // GetCovXZ
    -covMatrix[2]*cp*sp*st  // GetCovYY
    +covMatrix[4]*cp*ct;  // GetCovYZ
}

//______________________________________________________________________________
Double_t AliAODVertex::RotatedCovMatrixZZ(Double_t phi, Double_t theta) const
{
  // ZZ term of covariance matrix after rotation by phi around z-axis
  // and, then, by theta around new y-axis

  if (!fCovMatrix) {
    //AliFatal("Covariance matrix not set");
    return -999.;
  }

  Double_t covMatrix[6];

  GetCovMatrix(covMatrix);

  Double_t cp = TMath::Cos(phi);
  Double_t sp = TMath::Sin(phi);
  Double_t ct = TMath::Cos(theta);
  Double_t st = TMath::Sin(theta);
  return
     covMatrix[0]*cp*cp*st*st  // GetCovXX
    +covMatrix[1]*2.*cp*sp*st*st  // GetCovXY
    -covMatrix[3]*2.*cp*ct*st  // GetCovXZ
    +covMatrix[2]*sp*sp*st*st  // GetCovYY
    -covMatrix[4]*2.*sp*sp*ct*st  // GetCovYZ
    +covMatrix[5]*ct*ct;  // GetCovZZ
}

//______________________________________________________________________________
Double_t AliAODVertex::DistanceToVertex(AliAODVertex *vtx) const
{
  // distance in 3D to another AliAODVertex

  Double_t dx = GetX()-vtx->GetX();
  Double_t dy = GetY()-vtx->GetY();
  Double_t dz = GetZ()-vtx->GetZ();

  return TMath::Sqrt(dx*dx+dy*dy+dz*dz);
}

//______________________________________________________________________________
Double_t AliAODVertex::DistanceXYToVertex(AliAODVertex *vtx) const
{
  // distance in XY to another AliAODVertex

  Double_t dx = GetX()-vtx->GetX();
  Double_t dy = GetY()-vtx->GetY();

  return TMath::Sqrt(dx*dx+dy*dy);
}

//______________________________________________________________________________
Double_t AliAODVertex::ErrorDistanceToVertex(AliAODVertex *vtx) const
{
  // error on the distance in 3D to another AliAODVertex

  Double_t phi,theta;
  PhiAndThetaToVertex(vtx,phi,theta);
  // error2 due to this vertex
  Double_t error2 = RotatedCovMatrixXX(phi,theta);
  // error2 due to vtx vertex
  Double_t error2vtx = vtx->RotatedCovMatrixXX(phi,theta);

  return TMath::Sqrt(error2+error2vtx);
}

//______________________________________________________________________________
Double_t AliAODVertex::ErrorDistanceXYToVertex(AliAODVertex *vtx) const
{
  // error on the distance in XY to another AliAODVertex

  Double_t phi,theta;
  PhiAndThetaToVertex(vtx,phi,theta);
  // error2 due to this vertex
  Double_t error2 = RotatedCovMatrixXX(phi);
  // error2 due to vtx vertex
  Double_t error2vtx = vtx->RotatedCovMatrixXX(phi);

  return TMath::Sqrt(error2+error2vtx);
}

//______________________________________________________________________________
template <class T, class P> 
void AliAODVertex::PhiAndThetaToVertex(AliAODVertex *vtx, P &phi, T &theta) const
{
  // rotation angles around z-axis (phi) and around new y-axis (theta)
  // with which vtx is seen (used by RotatedCovMatrix... methods)

  phi = TMath::ATan2(vtx->GetY()-GetY(),vtx->GetX()-GetX());
  Double_t vtxxphi = vtx->GetX()*TMath::Cos(phi)+vtx->GetY()*TMath::Sin(phi);
  Double_t xphi = GetX()*TMath::Cos(phi)+GetY()*TMath::Sin(phi);
  theta = TMath::ATan2(vtx->GetZ()-GetZ(),vtxxphi-xphi);
}

//______________________________________________________________________________
void AliAODVertex::PrintIndices() const 
{
  // Print indices of particles originating form this vertex

  TRefArrayIter iter(&fDaughters);
  while (TObject *daugh = iter.Next()) {
    printf("Particle %p originates from this vertex.\n", (void*)daugh);
  }
}

//______________________________________________________________________________
void AliAODVertex::Print(Option_t* /*option*/) const 
{
  // Print information of all data members

  printf("Vertex position:\n");
  printf("     x = %f\n", fPosition[0]);
  printf("     y = %f\n", fPosition[1]);
  printf("     z = %f\n", fPosition[2]);
  printf(" parent particle: %p\n", (void*)fParent.GetObject());
  printf(" origin of %d particles\n", fDaughters.GetEntriesFast());
  printf(" vertex type %d\n", fType);
  
  /*
  if (fCovMatrix) {
    printf("Covariance matrix:\n");
    printf(" %12.10f  %12.10f  %12.10f\n %12.10f  %12.10f  %12.10f\n %12.10f  %12.10f  %12.10f\n", 
	   fCovMatrix[0],
	   fCovMatrix[1],
	   fCovMatrix[3],
	   fCovMatrix[1],
	   fCovMatrix[2],
	   fCovMatrix[4],
	   fCovMatrix[3],
	   fCovMatrix[4],
	   fCovMatrix[5]); 
	   } */
  printf(" Chi^2/NDF = %f\n", fChi2perNDF);
}


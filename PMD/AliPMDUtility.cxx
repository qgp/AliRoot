/***************************************************************************
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
//-----------------------------------------------------//
//                                                     //
//                                                     //
//  Date   : August 05 2003                            //
//                                                     //
//  Utility code for ALICE-PMD                         //
//                                                     //
//-----------------------------------------------------//

#include "Riostream.h"
#include "AliPMDUtility.h"
#include "TMath.h"
#include <stdio.h>
#include <math.h>


ClassImp(AliPMDUtility)

AliPMDUtility::AliPMDUtility()
{
  // Default constructor
  fPx    = 0.;
  fPy    = 0.;
  fPz    = 0.;
  fTheta = 0.;
  fEta   = 0.;
  fPhi   = 0.;
}

AliPMDUtility::AliPMDUtility(Float_t px, Float_t py, Float_t pz)
{
  // Constructor
  fPx = px;
  fPy = py;
  fPz = pz;
  fTheta = 0.;
  fEta   = 0.;
  fPhi   = 0.;
}

AliPMDUtility::~AliPMDUtility()
{
  // Default destructor
}
void AliPMDUtility::HexGeomCellPos(Int_t ism, Int_t xpad, Int_t ypad, Float_t &xpos, Float_t &ypos)
{
  // This converts PMD cluster or CELL coordinates
  // to Global coordinates.
  // Written by Prof. S.C. Phatak

  const Float_t  kCellDia = 0.5;
  const Float_t  kPi      = TMath::Pi();    //3.14159;
  const Double_t kSqroot3by2   = 0.8660254;  // sqrth = sqrt(3.)/2.

  Int_t i;
  Int_t j = xpad;
  Int_t k = ypad;

  /*
    Supermodeule number starting from 0
    ism --> supermodule no ( 0 - 26 )
    idet --> detector ( pmd or cpv : not required now )
    j --> xpad ( goes from 1 to 72 )
    k --> ypad ( goes from 1 to 72 )
    xp --> global x coordinate
    yp --> global y coordinate
    
    (xp0,yp0) corner positions of all supermodules in global
    coordinate system. That is the origin
    of the local ( supermodule ) coordinate system.
  */ 
  
  Float_t xp0[27] = 
  {
    -17.9084, 18.2166, 54.3416, -35.9709, 0.154144, 
    36.2791, -54.0334, -17.9084, 18.2166, 36.7791, 
    18.7166, 0.654194, 72.9041, 54.8416, 36.7792, 
    109.029, 90.9666, 72.9042, -18.8708, -36.9334, 
    -54.996, -36.9332, -54.9958, -73.0584, -54.9956, 
    -73.0582, -91.1208
  };

  Float_t yp0[27] = 
  {
    -32.1395, -32.1395, -32.1395, -63.4247, -63.4247, 
    -63.4247, -94.7098, -94.7098, -94.7098, 0.545689, 
    31.8309, 63.1161, 0.545632, 31.8308, 63.116, 
    0.545573, 31.8308, 63.116, 31.5737, 0.288616, 
    -30.9965, 62.859, 31.5738, 0.288733, 94.1442, 
    62.8591, 31.574
  };

  /* 
     angles of rotation for three sets of supermodules
     The angle is same for first nine, next nine and last nine 
     supermodules 
  */
  
  Float_t th[3] = {0., -2.*kPi/3., 2.*kPi/3.};
  Float_t xr, yr, xinit, yinit, cs, sn;
  
  /* 
     xinit and yinit are coordinates of the cell in local coordinate system
  */
  
  xinit = (j)*kCellDia+(k)/2.*kCellDia;
  yinit = kSqroot3by2*(k)/2.;
  i=ism/9;
  cs=cos(th[i]);
  sn=sin(th[i]);
  //
  // rotate first
  //
  xr=cs*xinit+sn*yinit;
  yr=-sn*xinit+cs*yinit;
  //
  // then translate
  //
  xpos=xr+xp0[ism];
  ypos=yr+yp0[ism];

}

void AliPMDUtility::RectGeomCellPos(Int_t ism, Int_t ium, Int_t xpad, Int_t ypad, Float_t &xpos, Float_t &ypos)
{
  // This routine finds the cell eta,phi for the new PMD rectangular 
  // geometry in ALICE
  // Authors : Bedanga Mohanty and Dipak Mishra - 29.4.2003
  // modified by B. K. Nnadi for change of coordinate sys
  //
  // SMA  ---> Supermodule Type A           ( SM - 0)
  // SMAR ---> Supermodule Type A ROTATED   ( SM - 1)
  // SMB  ---> Supermodule Type B           ( SM - 2)
  // SMBR ---> Supermodule Type B ROTATED   ( SM - 3)
  //
  // ism   : number of supermodules in one plane = 4
  // ium   : number of unitmodules  in one SM    = 6
  // gbum  : (global) unit module numbering in a supermodule
  //

  Int_t gbum = ism*6 + ium;
  Int_t irow  = xpad;
  Int_t icol  = ypad;

  // Corner positions (x,y) of the 24 unit moudles in ALICE PMD
  
  Double_t xcorner[24] =
  {
    85.15,  60.85,  36.55,  85.15,  60.85,  36.55, //SMA 
    -85.15, -60.85, -36.55, -85.15, -60.85, -36.55, //SMAR
    84.90,  36.60,  84.90,  36.60,  84.90,  36.60, //SMB
    -84.90, -36.60, -84.90, -36.60, -84.90, -36.60  //SMBR
  };
  
  Double_t ycorner[24] =
  { 
    32.45708755,  32.45708755,  32.45708755,        //SMA
    -9.30645245,  -9.30645245,  -9.30645245,        //SMA
    -32.45708755, -32.45708755, -32.45708755,        //SMAR
    9.30645245,   9.30645245,   9.30645245,        //SMAR
    -31.63540818, -31.63540818, -52.61435544,        //SMB
    -52.61435544, -73.59330270, -73.59330270,        //SMB
    31.63540818,  31.63540818,  52.61435544,        //SMBR
    52.61435544,  73.59330270,  73.59330270         //SMBR
  };
  
  const Float_t kSqroot3      = 1.73205;  // sqrt(3.);
  const Float_t kCellRadius   = 0.25;
  
  //
  //Every even row of cells is shifted and placed
  //in geant so this condition
  //
  Float_t shift = 0.0;
  if(irow%2 == 0)
    {
      shift = 0.25;
    }
  else
    {
      shift = 0.0;
    }
  if(ism == 0 || ism == 2)
    {
      ypos = ycorner[gbum] + 
	irow*kCellRadius*kSqroot3;

      xpos = xcorner[gbum] - 
	icol*2.0*kCellRadius - shift;
    }
  else if(ism == 1 || ism == 3)
    {
      ypos = ycorner[gbum] -
	irow*kCellRadius*kSqroot3;

      xpos = xcorner[gbum] +
	icol*2.0*kCellRadius + shift;
    }
}

void AliPMDUtility::RectGeomCellPos(Int_t ism, Int_t ium, Float_t xpad, Float_t ypad, Float_t &xpos, Float_t &ypos)
{
  // If the xpad and ypad inputs are float, then 0.5 is added to it
  // to find the layer which is shifted.
  // This routine finds the cell eta,phi for the new PMD rectangular 
  // geometry in ALICE
  // Authors : Bedanga Mohanty and Dipak Mishra - 29.4.2003
  // modified by B. K. Nnadi for change of coordinate sys
  //
  // SMA  ---> Supermodule Type A           ( SM - 0)
  // SMAR ---> Supermodule Type A ROTATED   ( SM - 1)
  // SMB  ---> Supermodule Type B           ( SM - 2)
  // SMBR ---> Supermodule Type B ROTATED   ( SM - 3)
  //
  // ism   : number of supermodules in one plane = 4
  // ium   : number of unitmodules  in one SM    = 6
  // gbum  : (global) unit module numbering in a supermodule
  //

  Int_t gbum    = ism*6 + ium;
  Float_t irow  = xpad;
  Float_t icol  = ypad;

  // Corner positions (x,y) of the 24 unit moudles in ALICE PMD
  
  Double_t xcorner[24] =
  {
    85.15,  60.85,  36.55,  85.15,  60.85,  36.55, //SMA 
    -85.15, -60.85, -36.55, -85.15, -60.85, -36.55, //SMAR
    84.90,  36.60,  84.90,  36.60,  84.90,  36.60, //SMB
    -84.90, -36.60, -84.90, -36.60, -84.90, -36.60  //SMBR
  };
  
  Double_t ycorner[24] =
  { 
    32.45708755,  32.45708755,  32.45708755,        //SMA
    -9.30645245,  -9.30645245,  -9.30645245,        //SMA
    -32.45708755, -32.45708755, -32.45708755,        //SMAR
    9.30645245,   9.30645245,   9.30645245,        //SMAR
    -31.63540818, -31.63540818, -52.61435544,        //SMB
    -52.61435544, -73.59330270, -73.59330270,        //SMB
    31.63540818,  31.63540818,  52.61435544,        //SMBR
    52.61435544,  73.59330270,  73.59330270         //SMBR
  };
  
  const Float_t kSqroot3    = 1.73205;  // sqrt(3.);
  const Float_t kCellRadius = 0.25;
  
  //
  //Every even row of cells is shifted and placed
  //in geant so this condition
  //
  Float_t shift = 0.0;
  Int_t iirow = (Int_t) (irow+0.5);
  if(iirow%2 == 0)
    {
      shift = 0.25;
    }
  else
    {
      shift = 0.0;
    }
  if(ism == 0 || ism == 2)
    {
      ypos = ycorner[gbum] + 
	irow*kCellRadius*kSqroot3;

      xpos = xcorner[gbum] - 
	icol*2.0*kCellRadius - shift;
    }
  else if(ism == 1 || ism == 3)
    {
      ypos = ycorner[gbum] -
	irow*kCellRadius*kSqroot3;

      xpos = xcorner[gbum] +
	icol*2.0*kCellRadius + shift;
    }
}

void AliPMDUtility::SetPxPyPz(Float_t px, Float_t py, Float_t pz)
{
  fPx = px;
  fPy = py;
  fPz = pz;
}

void AliPMDUtility::SetXYZ(Float_t xpos, Float_t ypos, Float_t zpos)
{
  fPx = xpos;
  fPy = ypos;
  fPz = zpos;
}
void AliPMDUtility::CalculateEta()
{
  Float_t rpxpy, theta, eta;

  rpxpy  = TMath::Sqrt(fPx*fPx + fPy*fPy);
  theta  = TMath::ATan2(rpxpy,fPz);
  eta    = -TMath::Log(TMath::Tan(0.5*theta));
  fTheta = theta;
  fEta   = eta;
}
void AliPMDUtility::CalculatePhi()
{
  Float_t pybypx, phi = 0., phi1;

  if(fPx==0)
    {
      if(fPy>0) phi = 90.;
      if(fPy<0) phi = 270.;
    }
  if(fPx != 0)
    {
      pybypx = fPy/fPx;
      if(pybypx < 0) pybypx = - pybypx;
      phi1 = TMath::ATan(pybypx)*180./3.14159;

      if(fPx > 0 && fPy > 0) phi = phi1;        // 1st Quadrant
      if(fPx < 0 && fPy > 0) phi = 180 - phi1;  // 2nd Quadrant
      if(fPx < 0 && fPy < 0) phi = 180 + phi1;  // 3rd Quadrant
      if(fPx > 0 && fPy < 0) phi = 360 - phi1;  // 4th Quadrant

    }
  phi = phi*3.14159/180.;

  fPhi = phi;

}
void AliPMDUtility::CalculateEtaPhi()
{
  Float_t rpxpy, theta, eta;
  Float_t pybypx, phi = 0., phi1;

  rpxpy = TMath::Sqrt(fPx*fPx + fPy*fPy);
  theta = TMath::ATan2(rpxpy,fPz);
  eta   = -TMath::Log(TMath::Tan(0.5*theta));
  
  if(fPx==0)
    {
      if(fPy>0) phi = 90.;
      if(fPy<0) phi = 270.;
    }
  if(fPx != 0)
    {
      pybypx = fPy/fPx;
      if(pybypx < 0) pybypx = - pybypx;
      phi1 = TMath::ATan(pybypx)*180./3.14159;
      if(fPx > 0 && fPy > 0) phi = phi1;        // 1st Quadrant
      if(fPx < 0 && fPy > 0) phi = 180 - phi1;  // 2nd Quadrant
      if(fPx < 0 && fPy < 0) phi = 180 + phi1;  // 3rd Quadrant
      if(fPx > 0 && fPy < 0) phi = 360 - phi1;  // 4th Quadrant

    }
  phi = phi*3.14159/180.;

  fTheta = theta;
  fEta   = eta;
  fPhi   = phi;
}
Float_t AliPMDUtility::GetTheta() const
{
  return fTheta;
}
Float_t AliPMDUtility::GetEta() const
{
  return fEta;
}
Float_t AliPMDUtility::GetPhi() const
{
  return fPhi;
}


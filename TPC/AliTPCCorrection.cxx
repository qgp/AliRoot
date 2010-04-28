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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliTPCCorrection class                                                     //
//                                                                            //
// This class provides a general framework to deal with space point           //
// distortions. An correction class which inherits from here is for example   //
// AliTPCExBBShape or AliTPCExBTwist                                          //
//                                                                            //
// General functions are (for example):                                       //
//   CorrectPoint(x,roc) where x is the vector of inital positions in         //
//   cartesian coordinates and roc represents the Read Out chamber number     //
//   according to the offline naming convention. The vector x is overwritten  //
//   with the corrected coordinates.                                          //
//                                                                            //
// An alternative usage would be CorrectPoint(x,roc,dx), which leaves the     //
//   vector x untouched, put returns the distortions via the vector dx        //
//                                                                            //
// The class allows "effective Omega Tau" corrections to be shifted to the    //
// single distortion classes.                                                 //
//                                                                            //
// Note: This class is normally used via the class AliTPCComposedCorrection   //
//                                                                            //
// date: 27/04/2010                                                           //
// Authors: Magnus Mager, Stefan Rossegger, Jim Thomas                        //
////////////////////////////////////////////////////////////////////////////////

#include <TH2F.h>
#include <TMath.h>
#include <TROOT.h>

#include "AliTPCCorrection.h"

// FIXME: the following values should come from the database
const Double_t AliTPCCorrection::fgkTPC_Z0   =249.7;     // nominal gating grid position 
const Double_t AliTPCCorrection::fgkIFCRadius= 83.06;    // Mean Radius of the Inner Field Cage ( 82.43 min,  83.70 max) (cm)
const Double_t AliTPCCorrection::fgkOFCRadius=254.5;     // Mean Radius of the Outer Field Cage (252.55 min, 256.45 max) (cm)
const Double_t AliTPCCorrection::fgkZOffSet  = 0.2;      // Offset from CE: calculate all distortions closer to CE as if at this point
const Double_t AliTPCCorrection::fgkCathodeV =-100000.0; // Cathode Voltage (volts)
const Double_t AliTPCCorrection::fgkGG       =-70.0;     // Gating Grid voltage (volts)


// FIXME: List of interpolation points (course grid in the middle, fine grid on the borders)
const Double_t AliTPCCorrection::fgkRList[AliTPCCorrection::kNR] =  {   
84.0,   84.5,   85.0,   85.5,   86.0,  87.0,    88.0,
90.0,   92.0,   94.0,   96.0,   98.0,  100.0,  102.0,  104.0,  106.0,  108.0, 
110.0,  112.0,  114.0,  116.0,  118.0,  120.0,  122.0,  124.0,  126.0,  128.0, 
130.0,  132.0,  134.0,  136.0,  138.0,  140.0,  142.0,  144.0,  146.0,  148.0, 
150.0,  152.0,  154.0,  156.0,  158.0,  160.0,  162.0,  164.0,  166.0,  168.0, 
170.0,  172.0,  174.0,  176.0,  178.0,  180.0,  182.0,  184.0,  186.0,  188.0,
190.0,  192.0,  194.0,  196.0,  198.0,  200.0,  202.0,  204.0,  206.0,  208.0,
210.0,  212.0,  214.0,  216.0,  218.0,  220.0,  222.0,  224.0,  226.0,  228.0,
230.0,  232.0,  234.0,  236.0,  238.0,  240.0,  242.0,  244.0,  246.0,  248.0,
249.0,  249.5,  250.0,  251.5,  252.0  } ;
  
const Double_t AliTPCCorrection::fgkZList[AliTPCCorrection::kNZ]     =   { 
-249.5, -249.0, -248.5, -248.0, -247.0, -246.0, -245.0, -243.0, -242.0, -241.0,
-240.0, -238.0, -236.0, -234.0, -232.0, -230.0, -228.0, -226.0, -224.0, -222.0,
-220.0, -218.0, -216.0, -214.0, -212.0, -210.0, -208.0, -206.0, -204.0, -202.0,
-200.0, -198.0, -196.0, -194.0, -192.0, -190.0, -188.0, -186.0, -184.0, -182.0,
-180.0, -178.0, -176.0, -174.0, -172.0, -170.0, -168.0, -166.0, -164.0, -162.0,
-160.0, -158.0, -156.0, -154.0, -152.0, -150.0, -148.0, -146.0, -144.0, -142.0,
-140.0, -138.0, -136.0, -134.0, -132.0, -130.0, -128.0, -126.0, -124.0, -122.0,
-120.0, -118.0, -116.0, -114.0, -112.0, -110.0, -108.0, -106.0, -104.0, -102.0,
-100.0,  -98.0,  -96.0,  -94.0,  -92.0,  -90.0,  -88.0,  -86.0,  -84.0,  -82.0,
-80.0,  -78.0,  -76.0,  -74.0,  -72.0,  -70.0,  -68.0,  -66.0,  -64.0,  -62.0,
-60.0,  -58.0,  -56.0,  -54.0,  -52.0,  -50.0,  -48.0,  -46.0,  -44.0,  -42.0,
-40.0,  -38.0,  -36.0,  -34.0,  -32.0,  -30.0,  -28.0,  -26.0,  -24.0,  -22.0,
-20.0,  -18.0,  -16.0,  -14.0,  -12.0,  -10.0,   -8.0,   -6.0,   -4.0,   -2.0,
-1.0,   -0.5,   -0.2,   -0.1,  -0.05,   0.05,    0.1,    0.2,    0.5,    1.0, 
 2.0,    4.0,    6.0,    8.0,   10.0,   12.0,   14.0,   16.0,   18.0,   20.0,
 22.0,   24.0,   26.0,   28.0,   30.0,   32.0,   34.0,   36.0,   38.0,   40.0, 
 42.0,   44.0,   46.0,   48.0,   50.0,   52.0,   54.0,   56.0,   58.0,   60.0, 
 62.0,   64.0,   66.0,   68.0,   70.0,   72.0,   74.0,   76.0,   78.0,   80.0, 
 82.0,   84.0,   86.0,   88.0,   90.0,   92.0,   94.0,   96.0,   98.0,  100.0, 
102.0,  104.0,  106.0,  108.0,  110.0,  112.0,  114.0,  116.0,  118.0,  120.0, 
122.0,  124.0,  126.0,  128.0,  130.0,  132.0,  134.0,  136.0,  138.0,  140.0, 
142.0,  144.0,  146.0,  148.0,  150.0,  152.0,  154.0,  156.0,  158.0,  160.0, 
162.0,  164.0,  166.0,  168.0,  170.0,  172.0,  174.0,  176.0,  178.0,  180.0, 
182.0,  184.0,  186.0,  188.0,  190.0,  192.0,  194.0,  196.0,  198.0,  200.0,
202.0,  204.0,  206.0,  208.0,  210.0,  212.0,  214.0,  216.0,  218.0,  220.0,
222.0,  224.0,  226.0,  228.0,  230.0,  232.0,  234.0,  236.0,  238.0,  240.0,
242.0,  243.0,  244.0,  245.0,  246.0,  247.0,  248.0,  248.5,  249.0,  249.5   } ;



AliTPCCorrection::AliTPCCorrection() 
  : TNamed("correction_unity","unity"),fJLow(0),fKLow(0)
{
  //
  // default constructor
  //
}

AliTPCCorrection::AliTPCCorrection(const char *name,const char *title)
  : TNamed(name,title),fJLow(0),fKLow(0)
{
  //
  // default constructor, that set the name and title
  //
}

AliTPCCorrection::~AliTPCCorrection() {
  // 
  // virtual destructor
  //
}

void AliTPCCorrection::CorrectPoint(Float_t x[],const Short_t roc) {
  //
  // Corrects the initial coordinates x (cartesian coordinates)
  // according to the given effect (inherited classes)
  // roc represents the TPC read out chamber (offline numbering convention)
  //
  Float_t dx[3];
  GetCorrection(x,roc,dx);
  for (Int_t j=0;j<3;++j) x[j]+=dx[j];
}

void AliTPCCorrection::CorrectPoint(const Float_t x[],const Short_t roc,Float_t xp[]) {
  //
  // Corrects the initial coordinates x (cartesian coordinates) and stores the new 
  // (distorted) coordinates in xp. The distortion is set according to the given effect (inherited classes)
  // roc represents the TPC read out chamber (offline numbering convention)
  //
  Float_t dx[3];
  GetCorrection(x,roc,dx);
  for (Int_t j=0;j<3;++j) xp[j]=x[j]+dx[j];
}

void AliTPCCorrection::DistortPoint(Float_t x[],const Short_t roc) {
  //
  // Distorts the initial coordinates x (cartesian coordinates)
  // according to the given effect (inherited classes)
  // roc represents the TPC read out chamber (offline numbering convention)
  //
  Float_t dx[3];
  GetDistortion(x,roc,dx);
  for (Int_t j=0;j<3;++j) x[j]+=dx[j];
}

void AliTPCCorrection::DistortPoint(const Float_t x[],const Short_t roc,Float_t xp[]) {
  //
  // Distorts the initial coordinates x (cartesian coordinates) and stores the new 
  // (distorted) coordinates in xp. The distortion is set according to the given effect (inherited classes)
  // roc represents the TPC read out chamber (offline numbering convention)
  //
  Float_t dx[3];
  GetDistortion(x,roc,dx);
  for (Int_t j=0;j<3;++j) xp[j]=x[j]+dx[j];
}

void AliTPCCorrection::GetCorrection(const Float_t /*x*/[],const Short_t /*roc*/,Float_t dx[]) {
  //
  // This function delivers the correction values dx in respect to the inital coordinates x
  // roc represents the TPC read out chamber (offline numbering convention)
  // Note: The dx is overwritten by the inherited effectice class ...
  //
  for (Int_t j=0;j<3;++j) { dx[j]=0.; }
}

void AliTPCCorrection::GetDistortion(const Float_t x[],const Short_t roc,Float_t dx[]) {
  //
  // This function delivers the distortion values dx in respect to the inital coordinates x
  // roc represents the TPC read out chamber (offline numbering convention)
  //
  GetCorrection(x,roc,dx);
  for (Int_t j=0;j<3;++j) dx[j]=-dx[j];
}

void AliTPCCorrection::Init() {
  //
  // Initialization funtion (not used at the moment)
  //
}

void AliTPCCorrection::Print(Option_t* /*option*/) const {
  //
  // Print function to check which correction classes are used 
  // option=="d" prints details regarding the setted magnitude 
  // option=="a" prints the C0 and C1 coefficents for calibration purposes
  //
  printf("TPC spacepoint correction: \"%s\"\n",GetTitle());
}

void AliTPCCorrection:: SetOmegaTauT1T2(Float_t /*omegaTau*/,Float_t /*t1*/,Float_t /*t2*/) {
  //
  // Virtual funtion to pass the wt values (might become event dependent) to the inherited classes
  // t1 and t2 represent the "effective omegaTau" corrections and were measured in a dedicated
  // calibration run
  //
  // SetOmegaTauT1T2(omegaTau, t1, t2);
}

TH2F* AliTPCCorrection::CreateHistoDRinXY(Float_t z,Int_t nx,Int_t ny) {
  //
  // Simple plot functionality.
  // Returns a 2d hisogram which represents the corrections in radial direction (dr)
  // in respect to position z within the XY plane.
  // The histogramm has nx times ny entries. 
  //
  
  TH2F *h=CreateTH2F("dr_xy",GetTitle(),"x [cm]","y [cm]","dr [cm]",
		     nx,-250.,250.,ny,-250.,250.);
  Float_t x[3],dx[3];
  x[2]=z;
  Int_t roc=z>0.?0:18; // FIXME
  for (Int_t iy=1;iy<=ny;++iy) {
    x[1]=h->GetYaxis()->GetBinCenter(iy);
    for (Int_t ix=1;ix<=nx;++ix) {
      x[0]=h->GetXaxis()->GetBinCenter(ix);
      GetCorrection(x,roc,dx);
      Float_t r0=TMath::Sqrt((x[0]      )*(x[0]      )+(x[1]      )*(x[1]      ));
      if (90.<=r0 && r0<=250.) {
	Float_t r1=TMath::Sqrt((x[0]+dx[0])*(x[0]+dx[0])+(x[1]+dx[1])*(x[1]+dx[1]));
	h->SetBinContent(ix,iy,r1-r0);
      }
      else
	h->SetBinContent(ix,iy,0.);
    }
  }
  return h;
}

TH2F* AliTPCCorrection::CreateHistoDRPhiinXY(Float_t z,Int_t nx,Int_t ny) {
  //
  // Simple plot functionality.
  // Returns a 2d hisogram which represents the corrections in rphi direction (drphi) 
  // in respect to position z within the XY plane.
  // The histogramm has nx times ny entries. 
  //

  TH2F *h=CreateTH2F("drphi_xy",GetTitle(),"x [cm]","y [cm]","drphi [cm]",
		     nx,-250.,250.,ny,-250.,250.);
  Float_t x[3],dx[3];
  x[2]=z;
  Int_t roc=z>0.?0:18; // FIXME
  for (Int_t iy=1;iy<=ny;++iy) {
    x[1]=h->GetYaxis()->GetBinCenter(iy);
    for (Int_t ix=1;ix<=nx;++ix) {
      x[0]=h->GetXaxis()->GetBinCenter(ix);
      GetCorrection(x,roc,dx);
      Float_t r0=TMath::Sqrt((x[0]      )*(x[0]      )+(x[1]      )*(x[1]      ));
      if (90.<=r0 && r0<=250.) {
 	Float_t phi0=TMath::ATan2(x[1]      ,x[0]      );
	Float_t phi1=TMath::ATan2(x[1]+dx[1],x[0]+dx[0]);

	Float_t dphi=phi1-phi0;
	if (dphi<TMath::Pi()) dphi+=TMath::TwoPi();
	if (dphi>TMath::Pi()) dphi-=TMath::TwoPi();
      
	h->SetBinContent(ix,iy,r0*dphi);
      }
      else
	h->SetBinContent(ix,iy,0.);
    }
  }
  return h;
}

TH2F* AliTPCCorrection::CreateHistoDRinZR(Float_t phi,Int_t nz,Int_t nr) {
  //
  // Simple plot functionality.
  // Returns a 2d hisogram which represents the corrections in r direction (dr) 
  // in respect to angle phi within the ZR plane.
  // The histogramm has nx times ny entries. 
  //
  TH2F *h=CreateTH2F("dr_zr",GetTitle(),"z [cm]","r [cm]","dr [cm]",
		     nz,-250.,250.,nr,85.,250.);
  Float_t x[3],dx[3];
  for (Int_t ir=1;ir<=nr;++ir) {
    Float_t radius=h->GetYaxis()->GetBinCenter(ir);
    x[0]=radius*TMath::Cos(phi);
    x[1]=radius*TMath::Sin(phi);
    for (Int_t iz=1;iz<=nz;++iz) {
      x[2]=h->GetXaxis()->GetBinCenter(iz);
      Int_t roc=x[2]>0.?0:18; // FIXME
      GetCorrection(x,roc,dx);
      Float_t r0=TMath::Sqrt((x[0]      )*(x[0]      )+(x[1]      )*(x[1]      ));
      Float_t r1=TMath::Sqrt((x[0]+dx[0])*(x[0]+dx[0])+(x[1]+dx[1])*(x[1]+dx[1]));
      h->SetBinContent(iz,ir,r1-r0);
    }
  }
  printf("SDF\n");
  return h;

}

TH2F* AliTPCCorrection::CreateHistoDRPhiinZR(Float_t phi,Int_t nz,Int_t nr) {
  //
  // Simple plot functionality.
  // Returns a 2d hisogram which represents the corrections in rphi direction (drphi) 
  // in respect to angle phi within the ZR plane.
  // The histogramm has nx times ny entries. 
  //
  TH2F *h=CreateTH2F("drphi_zr",GetTitle(),"z [cm]","r [cm]","drphi [cm]",
		     nz,-250.,250.,nr,85.,250.);
  Float_t x[3],dx[3];
  for (Int_t iz=1;iz<=nz;++iz) {
    x[2]=h->GetXaxis()->GetBinCenter(iz);
    Int_t roc=x[2]>0.?0:18; // FIXME
    for (Int_t ir=1;ir<=nr;++ir) {
      Float_t radius=h->GetYaxis()->GetBinCenter(ir);
      x[0]=radius*TMath::Cos(phi);
      x[1]=radius*TMath::Sin(phi);
      GetCorrection(x,roc,dx);
      Float_t r0=TMath::Sqrt((x[0]      )*(x[0]      )+(x[1]      )*(x[1]      ));
      Float_t phi0=TMath::ATan2(x[1]      ,x[0]      );
      Float_t phi1=TMath::ATan2(x[1]+dx[1],x[0]+dx[0]);
      
      Float_t dphi=phi1-phi0;
      if (dphi<TMath::Pi()) dphi+=TMath::TwoPi();
      if (dphi>TMath::Pi()) dphi-=TMath::TwoPi();
      
      h->SetBinContent(iz,ir,r0*dphi);
    }
  }
  return h;
}

TH2F* AliTPCCorrection::CreateTH2F(const char *name,const char *title,
				   const char *xlabel,const char *ylabel,const char *zlabel,
				  Int_t nbinsx,Double_t xlow,Double_t xup,
				  Int_t nbinsy,Double_t ylow,Double_t yup) {
  //
  // Helper function to create a 2d histogramm of given size
  //
  
  TString hname=name;
  Int_t i=0;
  if (gDirectory) {
    while (gDirectory->FindObject(hname.Data())) {
      hname =name;
      hname+="_";
      hname+=i;
      ++i;
    }
  }
  TH2F *h=new TH2F(hname.Data(),title,
		   nbinsx,xlow,xup,
		   nbinsy,ylow,yup);
  h->GetXaxis()->SetTitle(xlabel);
  h->GetYaxis()->SetTitle(ylabel);
  h->GetZaxis()->SetTitle(zlabel);
  h->SetStats(0);
  return h;
}


// Simple Interpolation functions: e.g. with bi(tri)cubic interpolations (not yet in TH2 and TH3)

void AliTPCCorrection::Interpolate2DEdistortion( const Int_t order, const Double_t r, const Double_t z, 
						  const Double_t er[kNZ][kNR], Double_t &er_value )
{
  //
  // Interpolate table - 2D interpolation
  //
  Double_t save_er[10] ;

  Search( kNZ,   fgkZList,  z,   fJLow   ) ;
  Search( kNR,   fgkRList,  r,   fKLow   ) ;
  if ( fJLow < 0 ) fJLow = 0 ;   // check if out of range
  if ( fKLow < 0 ) fKLow = 0 ;
  if ( fJLow + order  >=    kNZ - 1 ) fJLow =   kNZ - 1 - order ;
  if ( fKLow + order  >=    kNR - 1 ) fKLow =   kNR - 1 - order ;

  for ( Int_t j = fJLow ; j < fJLow + order + 1 ; j++ ) {
      save_er[j-fJLow]     = Interpolate( &fgkRList[fKLow], &er[j][fKLow], order, r )   ;
  }
  er_value = Interpolate( &fgkZList[fJLow], save_er, order, z )   ;

}


Double_t AliTPCCorrection::Interpolate( const Double_t xArray[], const Double_t yArray[], 
				       const Int_t order, const Double_t x )
{
  //
  // Interpolate function Y(x) using linear (order=1) or quadratic (order=2) interpolation.
  //

  Double_t y ;
  if ( order == 2 ) {                // Quadratic Interpolation = 2 
    y  = (x-xArray[1]) * (x-xArray[2]) * yArray[0] / ( (xArray[0]-xArray[1]) * (xArray[0]-xArray[2]) ) ; 
    y += (x-xArray[2]) * (x-xArray[0]) * yArray[1] / ( (xArray[1]-xArray[2]) * (xArray[1]-xArray[0]) ) ; 
    y += (x-xArray[0]) * (x-xArray[1]) * yArray[2] / ( (xArray[2]-xArray[0]) * (xArray[2]-xArray[1]) ) ; 
  } else {                           // Linear Interpolation = 1
    y  = yArray[0] + ( yArray[1]-yArray[0] ) * ( x-xArray[0] ) / ( xArray[1] - xArray[0] ) ;
  }

  return (y);

}


void AliTPCCorrection::Search( const Int_t n, const Double_t xArray[], const Double_t x, Int_t &low )
{
  //
  // Search an ordered table by starting at the most recently used point
  //

  Long_t middle, high ;
  Int_t  ascend = 0, increment = 1 ;

  if ( xArray[n-1] >= xArray[0] ) ascend = 1 ;  // Ascending ordered table if true
  
  if ( low < 0 || low > n-1 ) { 
    low = -1 ; high = n ; 
  } else {                                            // Ordered Search phase
    if ( (Int_t)( x >= xArray[low] ) == ascend )  {
      if ( low == n-1 ) return ;          
      high = low + 1 ;
      while ( (Int_t)( x >= xArray[high] ) == ascend ) {
	low = high ;
	increment *= 2 ;
	high = low + increment ;
	if ( high > n-1 )  {  high = n ; break ;  }
      }
    } else {
      if ( low == 0 )  {  low = -1 ;  return ;  }
      high = low - 1 ;
      while ( (Int_t)( x < xArray[low] ) == ascend ) {
	high = low ;
	increment *= 2 ;
	if ( increment >= high )  {  low = -1 ;  break ;  }
	else  low = high - increment ;
      }
    }
  }
  
  while ( (high-low) != 1 ) {                     // Binary Search Phase
    middle = ( high + low ) / 2 ;
    if ( (Int_t)( x >= xArray[middle] ) == ascend )
      low = middle ;
    else
      high = middle ;
  }
  
  if ( x == xArray[n-1] ) low = n-2 ;
  if ( x == xArray[0]   ) low = 0 ;
  
}

ClassImp(AliTPCCorrection)

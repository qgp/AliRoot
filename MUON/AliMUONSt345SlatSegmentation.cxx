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

//*********************************************************
//  Segmentation classes for slat modules          
//  This class works with local coordinates
//  of the slats via the class AliMUONGeometrySegmentation
//  This class contains the size of the slats and the
//  and the differents PCB densities. 
//  (from old AliMUONSegmentationSlatModule)
//  Gines, Subatech, Nov04
//*********************************************************

#include <TArrayI.h>
#include <TArrayF.h>
#include "AliMUONSt345SlatSegmentation.h"
#include "AliLog.h"

ClassImp(AliMUONSt345SlatSegmentation)


AliMUONSt345SlatSegmentation::AliMUONSt345SlatSegmentation() 
  : AliMUONVGeometryDESegmentation(),
 	fBending(0),
 	fId(0),
        fNsec(0),
        fNDiv(0),
        fDpxD(0),
        fDpyD(0),
	fDpx(0),
	fDpy(0),
	fNpx(999999),
	fNpy(999999),
	fWireD(0.0),
	fXhit(0.),
	fYhit(0.),
	fIx(0),
	fIy(0),
	fX(0.),
	fY(0.),
	fIxmin(0),
	fIxmax(0),
	fIymin(0),
	fIymax(0)
{
  // default constructor

}

//___________________________________________
AliMUONSt345SlatSegmentation::AliMUONSt345SlatSegmentation(Bool_t bending) 
  :     AliMUONVGeometryDESegmentation(),
 	fBending(bending),
 	fId(0),
        fNsec(0),
        fNDiv(0),
        fDpxD(0),
        fDpyD(0),
	fDpx(0),
	fDpy(0),
	fNpx(999999),
	fNpy(999999),
	fWireD(0.25),
	fXhit(0.),
	fYhit(0.),
	fIx(0),
	fIy(0),
	fX(0.),
	fY(0.),
	fIxmin(0),
	fIxmax(0),
	fIymin(0),
	fIymax(0)
{
  // Non default constructor
  fNsec = 4;  // 4 sector densities at most per slat 
  fNDiv = new TArrayI(fNsec);      
  fDpxD = new TArrayF(fNsec);      
  fDpyD = new TArrayF(fNsec);      
  (*fNDiv)[0]=(*fNDiv)[1]=(*fNDiv)[2]=(*fNDiv)[3]=0;     
  (*fDpxD)[0]=(*fDpxD)[1]=(*fDpxD)[2]=(*fDpxD)[3]=0;       
  (*fDpyD)[0]=(*fDpyD)[1]=(*fDpyD)[2]=(*fDpyD)[3]=0;       
}
//----------------------------------------------------------------------
AliMUONSt345SlatSegmentation::AliMUONSt345SlatSegmentation(const AliMUONSt345SlatSegmentation& rhs) 
:       AliMUONVGeometryDESegmentation(rhs),
	fBending(0),
 	fId(0),
	fDpx(0),
	fDpy(0),
	fNpx(999999),
	fNpy(999999),
	fWireD(0.25),
	fXhit(0.),
	fYhit(0.),
	fIx(0),
	fIy(0),
	fX(0.),
	fY(0.),
	fIxmin(0),
	fIxmax(0),
	fIymin(0),
	fIymax(0)
{
  // default constructor
}
//----------------------------------------------------------------------
AliMUONSt345SlatSegmentation::~AliMUONSt345SlatSegmentation() 
{
  // Destructor
  if (fNDiv) delete fNDiv;
  if (fDpxD) delete fDpxD;
  if (fDpyD) delete fDpyD;
}
//----------------------------------------------------------------------
AliMUONSt345SlatSegmentation& AliMUONSt345SlatSegmentation::operator=(const AliMUONSt345SlatSegmentation& rhs)
{
  // Protected assignement operator
  if (this == &rhs) return *this;
  AliFatal("Not implemented.");
  return *this;  
}


//------------------------------------------------------------------------
Float_t AliMUONSt345SlatSegmentation::Distance2AndOffset(Int_t iX, Int_t iY, Float_t X, Float_t Y, Int_t * /*dummy*/)
{
  // Returns the square of the distance between 1 pad
  // labelled by its Channel numbers and a coordinate
  Float_t x,y;
  GetPadC(iX,iY,x,y);
  return (x-X)*(x-X) + (y-Y)*(y-Y);
}
//____________________________________________________________________________
Float_t AliMUONSt345SlatSegmentation::Dpx(Int_t isec) const
{
  // Return x-strip width
  return (*fDpxD)[isec];
} 

//____________________________________________________________________________
Float_t AliMUONSt345SlatSegmentation::Dpy(Int_t  isec) const
{
  // Return y-strip width
  return (*fDpyD)[isec];
}
//_____________________________________________________________________________
Float_t AliMUONSt345SlatSegmentation::GetAnod(Float_t xhit) const
{
  // Returns for a hit position xhit the position of the nearest anode wire    
  Float_t wire= (xhit>0)? Int_t(xhit/fWireD)+0.5:Int_t(xhit/fWireD)-0.5;
  return fWireD*wire;
}



//--------------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::GetPadC(Int_t ix, Int_t iy, Float_t &x, Float_t &y) 
{
  if (ix < 1 || ix > Npx() || iy < 1 || iy > Npy() ){
    AliWarning(Form("ix %d or iy %d out of boundaries: Npx=%d and Npy=%d",ix, iy, Npx(), Npy()));
    x=-99999.; y=-99999.;

  } else { 

    //  Returns real coordinates (x,y) for given pad coordinates (ix,iy)
    //  Find sector isec
    Int_t isec = Sector(ix,iy);
    if (isec == -1) AliWarning(Form("isector = %d  with ix %d, iy %d", isec, ix, iy));
    if (iy > fNpyS[isec]) {
      x=-99999.; y=-99999.;
      return;
    }
    if (isec>0) {
      x = fCx[isec-1]+(ix-fNpxS[isec-1])*(*fDpxD)[isec];
      x = x-(*fDpxD)[isec]/2;
      y = Float_t(iy*(*fDpyD)[isec])-(*fDpyD)[isec]/2.- fCy;  // !!!  
    } else {
      x = y = 0;
    }
  }
}


//_____________________________________________________________________________
void AliMUONSt345SlatSegmentation::GetPadI(Float_t x, Float_t y, Int_t &ix, Int_t &iy) 
{
//  Returns pad coordinates (ix,iy) for given real coordinates (x,y)

  //  Find sector isec    
  Int_t isec=-1;
  for (Int_t i=fNsec-1; i > 0; i--) {
    if (x >= fCx[i-1]) {
      isec=i;
      if (fCx[isec] == fCx[isec-1]  && isec > 1) isec--;
      break;
    }
  }
  if (isec == -1) AliWarning(Form("isector equal to %d  with xl %f, yl %f", isec, x, y));
  if (isec>0) {
    ix= Int_t((x-fCx[isec-1])/(*fDpxD)[isec])
      +fNpxS[isec-1]+1;
    iy= Int_t((y+fCy)/(*fDpyD)[isec])+1;
  } else if (isec == 0) {
    ix= Int_t(x/(*fDpxD)[isec])+1;
    iy= Int_t((y+fCy)/(*fDpyD)[isec])+1;
  } else {
    ix=0;
    iy=0;
  }
}
//-------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::GetPadI(Float_t x, Float_t y , Float_t /*z*/, Int_t &ix, Int_t &iy)
{
  GetPadI(x, y, ix, iy);
}
//_______________________________________________________________
void AliMUONSt345SlatSegmentation::SetPadDivision(Int_t ndiv[4])
{
  // Defines the pad size perp. to the anode wire (y) for different sectors. 
  // Pad sizes are defined as integral fractions ndiv of a basis pad size
  // fDpx
  // 
  for (Int_t i=0; i<4; i++) {
    (*fNDiv)[i]=ndiv[i];
  }
  ndiv[0]=ndiv[1];
}
//____________________________________________________________________________
void AliMUONSt345SlatSegmentation::SetPadSize(Float_t p1, Float_t p2)
{
  //  Sets the padsize 
  fDpx=p1;
  fDpy=p2;
}
//_______________________________________________________________          
void AliMUONSt345SlatSegmentation::SetPcbBoards(Int_t n[4])
{
  //
  // Set PcbBoard segmentation zones for each density
  // n[0] PcbBoards for maximum density sector fNDiv[0]
  // n[1] PcbBoards for next density sector fNDiv[1] etc ...
  for (Int_t i=0; i<4; i++) fPcbBoards[i]=n[i];
}
//-------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::SetPad(Int_t ix, Int_t iy)
{
  //
  // Sets virtual pad coordinates, needed for evaluating pad response 
  // outside the tracking program 
  GetPadC(ix,iy,fX,fY);
  fSector=Sector(ix,iy);
}
//---------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::SetHit(Float_t x, Float_t y)
{
  // Set current hit 
  //
  fXhit = x;
  fYhit = y;
    
  if (x <  fCx[0])    fXhit = fCx[0];
  if (y < -fDyPCB/2.) fYhit = -fDyPCB/2.;
    
  if (x > fCx[fNsec-1]) fXhit = fCx[fNsec-1];
  if (y > fDyPCB/2.)    fYhit = fDyPCB/2.;
    
}
//----------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::SetHit(Float_t xhit, Float_t yhit, Float_t /*zhit*/)
{
  SetHit(xhit, yhit);
}

//----------------------------------------------------------
void AliMUONSt345SlatSegmentation::FirstPad(Float_t xhit, Float_t yhit, Float_t dx, Float_t dy)
{
// Initialises iteration over pads for charge distribution algorithm
//
    //
    // Find the wire position (center of charge distribution)
    Float_t x0a = GetAnod(xhit);
    fXhit = x0a;
    fYhit = yhit;
    //
    // and take fNsigma*sigma around this center
    Float_t x01 = x0a  - dx ;
    Float_t x02 = x0a  + dx;
    Float_t y01 = yhit - dy;
    Float_t y02 = yhit + dy;

    // check the limits after adding (fNsigma*sigma)
    if (x01 <  fCx[0])   x01 =  fCx[0];
    if (y01 < -fDyPCB/2) y01 = -fDyPCB/2;

    if (x02 >= fCx[fNsec-1]) x02 = fCx[fNsec-1]; // still ok ? (CF)

   
    Int_t isec=-1;
    for (Int_t i=fNsec-1; i > 0; i--) {
      if (x02 >= fCx[i-1]) {
	isec=i;
	if (fCx[isec] == fCx[isec-1] && isec > 1) isec--;
	break;
      }
    }

    y02 += Dpy(isec);// why ? (CF)
    if (y02 >= fDyPCB/2.) y02 = fDyPCB/2;
   
    //
    // find the pads over which the charge distributes
    GetPadI(x01,y01,fIxmin,fIymin);
    GetPadI(x02,y02,fIxmax,fIymax);
    
    if (fIxmax > fNpx) fIxmax=fNpx;
    if (fIymax > fNpyS[isec]) fIymax = fNpyS[isec];    
    if (fIxmin < 1) fIxmin = 1;    // patch for the moment (Ch. Finck)
    if (fIymin < 1) fIymin = 1;    

    fXmin = x01;
    fXmax = x02;    
    fYmin = y01;
    fYmax = y02;    
  
    // 
    // Set current pad to lower left corner
    if (fIxmax < fIxmin) fIxmax = fIxmin;
    if (fIymax < fIymin) fIymax = fIymin;    
    fIx = fIxmin;
    fIy = fIymin;
    
    GetPadC(fIx,fIy,fX,fY);
    fSector = Sector(fIx,fIy);
/*
    printf("\n \n First Pad: %d %d %f %f %d %d %d %f" , 
	   fIxmin, fIxmax, fXmin, fXmax, fNpx, fId, isec, Dpy(isec));    
    printf("\n \n First Pad: %d %d %f %f %d %d %d %f",
	   fIymin, fIymax, fYmin, fYmax,  fNpyS[isec], fId, isec, Dpy(isec));
*/
}



//----------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::FirstPad(Float_t xhit, Float_t yhit, Float_t /*zhit*/, Float_t dx, Float_t dy)
{
  FirstPad(xhit, yhit, dx, dy);
}
//----------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::NextPad()
{
  // Stepper for the iteration over pads
  //
  // Step to next pad in the integration region
  //  step from left to right    
  if (fIx != fIxmax) {
    fIx++;
    GetPadC(fIx,fIy,fX,fY);
    fSector=Sector(fIx,fIy);
    //  step up 
  } else if (fIy != fIymax) {
    fIx=fIxmin;
    fIy++;
    GetPadC(fIx,fIy,fX,fY);
    fSector=Sector(fIx,fIy);

  } else {
    fIx=-999;
    fIy=-999;
  }
}
//-------------------------------------------------------------------------
Int_t AliMUONSt345SlatSegmentation::MorePads()
{
  // Stopping condition for the iterator over pads
  //
  // Are there more pads in the integration region
    
  return  (fIx != -999  || fIy != -999);
}
//--------------------------------------------------------------------------
Int_t AliMUONSt345SlatSegmentation::Sector(Int_t ix, Int_t iy) 
{
  //
  // Determine segmentation zone from pad coordinates
  //
  Int_t isec = -1;
  for (Int_t i = 0; i < fNsec; i++) {
    if (ix <= fNpxS[i]) {
      isec = i;
      break;
    }
  }
  if (isec == -1) AliWarning(Form("Sector = %d  with ix %d and iy %d, Npx %d",
				  isec, ix, iy, fNpx));

  return isec;

}
//-----------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::
IntegrationLimits(Float_t& x1,Float_t& x2,Float_t& y1, Float_t& y2) 
{
  //  Returns integration limits for current pad
  //
  x1=fXhit-fX-Dpx(fSector)/2.;
  x2=x1+Dpx(fSector);
  y1=fYhit-fY-Dpy(fSector)/2.;
  y2=y1+Dpy(fSector);    
  //  printf("\n Integration Limits %f %f %f %f %d %f", x1, x2, y1, y2, fSector, Dpx(fSector));

}
//-----------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::
Neighbours(Int_t iX, Int_t iY, Int_t* Nlist, Int_t Xlist[10], Int_t Ylist[10]) 
{
  // Returns list of next neighbours for given Pad (iX, iY)
  Int_t i=0;
  //  step right
  if (iX+1 <= fNpx) {
    Xlist[i]=iX+1;
    Ylist[i++]=iY;
  }
  //  step left    
  if (iX-1 > 0) {
    Xlist[i]=iX-1;
    Ylist[i++]=iY;
  } 
  Int_t sector = Sector(iX,iY);
  //  step up
  if (iY+1 <= fNpyS[sector]) {
    Xlist[i]=iX;
    Ylist[i++]=iY+1;
  }
  //  step down    
  if (iY-1 > 0) {
    Xlist[i]=iX;
    Ylist[i++]=iY-1;
  }
  *Nlist=i;
}

//--------------------------------------------------------------------------
void AliMUONSt345SlatSegmentation::Init(Int_t detectionElementId)
{
  //
  //  Fill the arrays fCx (x-contour) and fNpxS (ix-contour) for each sector
  //  These arrays help in converting from real to pad co-ordinates and
  //  vice versa
  //   
  //  Segmentation is defined by rectangular modules approximating
  //  concentric circles as shown below
  //
  //  PCB module size in cm
  //  printf("\n Initialise Segmentation SlatModule \n");

  
  //  printf(" fBending: %d \n",fBending);

  fDxPCB=40;
  fDyPCB=40;

  //  Calculate padsize along x
  (*fDpxD)[fNsec-1]=fDpx;
  (*fDpyD)[fNsec-1]=fDpy;
  if (fNsec > 1) {
    for (Int_t i=fNsec-1; i>=0; i--){ // fNsec-2
      if (!fBending) {
	(*fDpxD)[i]=fDpx;
	(*fDpyD)[i]=(*fDpyD)[fNsec-1]/(*fNDiv)[i];
      } else {
	(*fDpxD)[i]=(*fDpxD)[fNsec-1]/(*fNDiv)[i];
	(*fDpyD)[i]=fDpy;
      }
    }
  }
  //
  // fill the arrays defining the pad segmentation boundaries
  //
  //  
  //  Loop over sectors (isec=0 for secto close to the beam pipe)
  Float_t totalLength = 0;
  for (Int_t isec=0; isec<4; isec++) totalLength += fPcbBoards[isec]*fDxPCB;  // !!!!

  fNpy = 0;   // maximum number of pads in y
  for (Int_t isec=0; isec<4; isec++) {
    if (isec==0) {
      fNpxS[0] = 0;
      fNpyS[0] = 0;
      fCx[0]   = -totalLength/2;
    } else {
      fNpxS[isec] = fNpxS[isec-1] + fPcbBoards[isec]*Int_t(fDxPCB/(*fDpxD)[isec]); 
      fNpyS[isec] = Int_t(fDyPCB/(*fDpyD)[isec]);
      if (fNpyS[isec] >= fNpy) fNpy = fNpyS[isec]; 
      fCx[isec]= fCx[isec-1] + fPcbBoards[isec]*fDxPCB;
    }
  } // sectors

  fNpx = fNpxS[3];  // maximum number of pads in x
  fCy = fDyPCB/2.;
  //
  fId = detectionElementId;
}












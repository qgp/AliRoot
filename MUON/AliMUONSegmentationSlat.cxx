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

#include "AliMUONSegmentationSlat.h"
#include "AliMUONSegmentationSlatModule.h"
#include "AliMUON.h"
#include "AliMUONChamber.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "AliRun.h"
#include <TMath.h>
#include <TBRIK.h>
#include <TNode.h>
#include <TGeometry.h>
#include <Riostream.h>

//___________________________________________
ClassImp(AliMUONSegmentationSlat)

AliMUONSegmentationSlat::AliMUONSegmentationSlat() 
{
// Default constructor
  fChamber = 0;
  fNDiv = 0;
  fSlats = 0;
  fCurrentSlat = 0;
}

AliMUONSegmentationSlat::AliMUONSegmentationSlat(Int_t nsec) 
{
// Non default constructor
    fSlats=0;            
    fNDiv = new TArrayI(4);
    fChamber = 0;
    fCurrentSlat = 0;
}

AliMUONSegmentationSlat::~AliMUONSegmentationSlat(){
  //PH Delete TObjArrays
  if (fSlats) {
    fSlats->Delete();
    delete fSlats;
  }

  if (fNDiv) {
    delete fNDiv;
  }

}

void AliMUONSegmentationSlat::SetPadSize(Float_t p1, Float_t p2)
{
//  Sets the pad (strip) size 
//  
    fDpx=p1;
    fDpy=p2;
}

Float_t AliMUONSegmentationSlat::GetAnod(Float_t xhit) const
{
// Returns for a hit position xhit the position of the nearest anode wire    
    Float_t wire= (xhit>0)? Int_t(xhit/fWireD)+0.5:Int_t(xhit/fWireD)-0.5;
    return fWireD*wire;
}

Float_t AliMUONSegmentationSlat::Dpx(Int_t isec) const
{
//
// Returns x-pad size for given sector isec
// isec = 100*islat+iregion
//
    Int_t islat, iregion;
    islat    = isec/100;
    iregion  = isec%100;
    return Slat(islat)->Dpx(iregion);
}

Float_t AliMUONSegmentationSlat::Dpy(Int_t isec) const
{
//
// Returns y-pad (strip)  size for given sector isec
   return fDpy;
}

void AliMUONSegmentationSlat::SetPadDivision(Int_t ndiv[4])
{
//
// Defines the pad size perp. to the anode wire (y) for different sectors. 
// Pad sizes are defined as integral fractions ndiv of a basis pad size
// fDpx
// 
    for (Int_t i=0; i<4; i++) {
	(*fNDiv)[i]=ndiv[i];
    }
}

void AliMUONSegmentationSlat::GlobalToLocal(
    Float_t x, Float_t y, Float_t z, Int_t &islat, Float_t &xlocal, Float_t &ylocal)
{
//
// Perform local to global transformation for space coordinates
//
    Float_t zlocal;
    Int_t i;
    Int_t index=-1;
    Float_t eps = 1.e-4;
    
// Transform According to slat plane z-position: negative side is shifted down 
//                                                 positive side is shifted up
// by half the overlap
    zlocal = z-fChamber->Z();
    zlocal = (x>0) ? zlocal-2.*fDz : zlocal+2.*fDz;
//  Set the signs for the symmetry transformation and transform to first quadrant
    SetSymmetry(x);
    Float_t xabs=TMath::Abs(x);

    Int_t ifirst = (zlocal < Float_t(0))? 0:1;
//
// Find slat number                      
    for (i=ifirst; i<fNSlats; i+=2) {
	index=i;
	if ((y >= fYPosition[i]-eps) && (y <= fYPosition[i]+fSlatY+eps)) break;
    }
    
//
// Transform to local coordinate system

    
    if (index >= fNSlats || index < 0 ) {
      islat = -1; xlocal=-1; ylocal = -1; }
    else {
      ylocal = y   -fYPosition[index];
      xlocal = xabs-fXPosition[index];
      islat  = index;
    }
}

void AliMUONSegmentationSlat::GlobalToLocal(
    Int_t ix, Int_t iy, Int_t &islat, Int_t &ixlocal, Int_t &iylocal)
{
//
// Perform global to local transformation for pad coordinates
//
    Int_t iytemp = iy;
    Int_t index  =  0;
    
    iylocal = iytemp;

//
// Find slat number (index) and iylocal  
    for (Int_t i=0; i<fNSlats; i++) {
	iytemp-=Slat(i)->Npy();
	
	
	if (iytemp <= 0) break;
	iylocal = iytemp;
	index=i+1;
    }

    ixlocal=TMath::Abs(ix);
    islat=index;
}

void AliMUONSegmentationSlat::
LocalToGlobal(Int_t islat, Float_t  xlocal, Float_t  ylocal, Float_t  &x, Float_t  &y, Float_t &z)
{
// Transform from local to global space coordinates
//
// upper plane (y>0) even slat number is shifted down
// upper plane (y>0)  odd slat number is shifted up 
// lower plane (y<0) even slat number is shifted up
// lower plane (y<0)  odd slat number is shifted down
//

    x = (xlocal+fXPosition[islat])*fSym;
    y=(ylocal+fYPosition[islat]);

    z = (TMath::Even(islat)) ?     -fDz : fDz ; 
    z = (x>0)                ? z+2.*fDz : z-2.*fDz ; 

    z+=fChamber->Z();
}


void AliMUONSegmentationSlat::LocalToGlobal(
    Int_t islat, Int_t ixlocal, Int_t iylocal, Int_t &ix, Int_t &iy)
{
// Transform from local to global pad coordinates
//
    Int_t i;
    iy=iylocal;
    
//
// Find slat number (index) and iylocal  
    for (i=0; i<islat; i++) iy+=Slat(islat)->Npy();

    ix=ixlocal*fSym;
    iy=iy;
}


void AliMUONSegmentationSlat::SetSymmetry(Int_t   ix)
{
// Set set signs for symmetry transformation
    fSym=TMath::Sign(1,ix);
}

void AliMUONSegmentationSlat::SetSymmetry(Float_t  x)
{
// Set set signs for symmetry transformation
    fSym=Int_t (TMath::Sign((Float_t)1.,x));
}

void AliMUONSegmentationSlat::
GetPadI(Float_t x, Float_t y, Float_t z, Int_t &ix, Int_t &iy)
{
// Returns pad coordinates for given set of space coordinates

    Int_t islat, i;
    Float_t xlocal, ylocal;
    
    GlobalToLocal(x,y,z,islat,xlocal,ylocal);
    if (islat == -1) {
	ix=0; iy=0; return;
    }
    
    Slat(islat)->GetPadI(xlocal, ylocal, ix, iy);
    for (i=0; i<islat; i++) iy+=Slat(islat)->Npy();

    ix=ix*Int_t(TMath::Sign((Float_t)1.,x));    
}


void AliMUONSegmentationSlat::
GetPadC(Int_t ix, Int_t iy, Float_t &x, Float_t &y, Float_t &z)
{
//  Returns real coordinates (x,y) for given pad coordinates (ix,iy)
//
    Int_t islat, ixlocal, iylocal;
//
// Delegation of transforamtion to slat
    GlobalToLocal(ix,iy,islat,ixlocal,iylocal);
    Slat(islat)->GetPadC(ixlocal, iylocal, x, y);
// Slat offset
    x+=fXPosition[islat];
    y+=fYPosition[islat];    

// Symmetry transformation of half planes
    x=x*TMath::Sign(1,ix);

// z-position
    z = (TMath::Even(islat)) ?      -fDz : fDz ; 
    z = (x>0)                ?  z+2.*fDz : z-2.*fDz ; 
    z += fChamber->Z();
}

Int_t AliMUONSegmentationSlat::ISector()
{
// Returns current sector during tracking
    Int_t iregion;
    
    iregion =  fCurrentSlat->ISector();
    return 100*fSlatIndex+iregion;
}

Int_t AliMUONSegmentationSlat::Sector(Int_t ix, Int_t iy)
{
// Returns sector for pad coordiantes (ix,iy)
    Int_t ixlocal, iylocal, iregion, islat;

    GlobalToLocal(ix,iy,islat,ixlocal,iylocal);
    
    iregion =  Slat(islat)->Sector(ixlocal, iylocal);
    return 100*islat+iregion;
}


void AliMUONSegmentationSlat::SetPad(Int_t ix, Int_t iy)
{
    //
    // Sets virtual pad coordinates, needed for evaluating pad response 
    // outside the tracking program
    Int_t islat, ixlocal, iylocal;

    SetSymmetry(ix);
    
    GlobalToLocal(ix,iy,islat,ixlocal,iylocal);
    fSlatIndex=islat;
    fCurrentSlat=Slat(islat);
    fCurrentSlat->SetPad(ixlocal, iylocal);
}

void  AliMUONSegmentationSlat::SetHit(Float_t xhit, Float_t yhit, Float_t zhit)
{   //
    // Sets current hit coordinates

    Float_t xlocal, ylocal;
    Int_t islat;

    

    GlobalToLocal(xhit,yhit,zhit,islat,xlocal,ylocal);
    fSlatIndex=islat;
    if (islat < 0) printf("\n SetHit: %d", islat);
    
    fCurrentSlat=Slat(islat);
    fCurrentSlat->SetHit(xlocal, ylocal);
}


void AliMUONSegmentationSlat::
FirstPad(Float_t xhit, Float_t yhit, Float_t zhit, Float_t dx, Float_t dy)
{
// Initialises iteration over pads for charge distribution algorithm
//


    Int_t islat;
    Float_t xlocal, ylocal;
    GlobalToLocal(xhit, yhit, zhit, islat, xlocal, ylocal);
    fSlatIndex=islat;
    if (islat>-1) {
      fCurrentSlat=Slat(islat);
      fCurrentSlat->FirstPad(xlocal, ylocal, dx, dy);
    }

}


void AliMUONSegmentationSlat::NextPad()
{
// Stepper for the iteration over pads
//
    fCurrentSlat->NextPad();
}


Int_t AliMUONSegmentationSlat::MorePads()
// Stopping condition for the iterator over pads
//
// Are there more pads in the integration region
{ 
    return fCurrentSlat->MorePads();
}

void AliMUONSegmentationSlat::
IntegrationLimits(Float_t& x1,Float_t& x2,Float_t& y1, Float_t& y2)
{
//  Returns integration limits for current pad
//
    
    fCurrentSlat->IntegrationLimits(x1, x2, y1, y2);

}

void AliMUONSegmentationSlat::
Neighbours(Int_t iX, Int_t iY, Int_t* Nlist, Int_t Xlist[10], Int_t Ylist[10])
{
// Returns list of neighbours of pad with coordinates iX, iY

    Int_t i, xListLocal[10], yListLocal[10], iXlocal, iYlocal, islat;
    
    SetSymmetry(iX);

    GlobalToLocal(iX, iY, islat, iXlocal, iYlocal);
 
    Slat(islat)->Neighbours(iXlocal, iYlocal, Nlist, xListLocal, yListLocal);
    
    for (i=0; i<*Nlist; i++) LocalToGlobal(islat, xListLocal[i], yListLocal[i], Xlist[i], Ylist[i]);

}


Int_t  AliMUONSegmentationSlat::Ix()
{
// Return current pad coordinate ix during stepping
    Int_t ixl,iyl,ix,iy;
    ixl=fCurrentSlat->Ix();
    iyl=fCurrentSlat->Iy();
    
    LocalToGlobal(fSlatIndex, ixl, iyl, ix, iy);
    Int_t ixc, iyc, isc;
    Float_t xc, yc;
    GlobalToLocal(ix, iy, isc, ixc, iyc);
    Slat(isc)->GetPadC(ixc,iyc,xc,yc);
    return ix;
}


Int_t  AliMUONSegmentationSlat::Iy()
{
// Return current pad coordinate iy during stepping
    Int_t ixl,iyl,ix,iy;
    ixl=fCurrentSlat->Ix();
    iyl=fCurrentSlat->Iy();
    LocalToGlobal(fSlatIndex, ixl, iyl, ix, iy);
    return iy;
}



   // Signal Generation Condition during Stepping
Int_t AliMUONSegmentationSlat::SigGenCond(Float_t x, Float_t y, Float_t z)
{ 
//
//  True if signal generation condition fullfilled
    Float_t xlocal, ylocal;
    Int_t islat;
    GlobalToLocal(x, y, z, islat, xlocal, ylocal);
    return Slat(islat)->SigGenCond(xlocal, ylocal, z);
}

// Initialise signal generation at coord (x,y,z)
void  AliMUONSegmentationSlat::SigGenInit(Float_t x, Float_t y, Float_t z)
{
// Initialize the signal generation condition
//
    Float_t xlocal, ylocal;
    Int_t islat;

    GlobalToLocal(x, y, z, islat, xlocal, ylocal);
    Slat(islat)->SigGenInit(xlocal, ylocal, z);
}



void AliMUONSegmentationSlat::Init(Int_t chamber)
{
//    
// Initialize slat modules of quadrant +/+    
// The other three quadrants are handled through symmetry transformations
//
  //printf("\n Initialise Segmentation Slat \n");
//

// Initialize Slat modules
    Int_t islat, i;
    Int_t ndiv[4];
// Pad division
    for (i=0; i<4; i++) ndiv[i]=(*fNDiv)[i];
//
    fDz=0.813;
// Slat height    
    fSlatY=40.;
    for (i=0; i<15; i++) fSlatX[i]=0.;
    
// Initialize array of slats 
    fSlats  = new TObjArray(fNSlats);
// Maximum number of strips (pads) in x and y
    fNpy=0;   
    fNpx=0;
// for each slat in the quadrant (+,+)    
    for (islat=0; islat<fNSlats; islat++) {
        fSlats->AddAt(CreateSlatModule(),islat);

	AliMUONSegmentationSlatModule *slat =  Slat(islat);
	// Configure Slat
	slat->SetId(islat);
	
// Foward pad size
	slat->SetPadSize(fDpx, fDpy);
// Forward wire pitch
	slat->SetDAnod(fWireD);
// Foward segmentation 
	slat->SetPadDivision(ndiv);
	slat->SetPcbBoards(fPcb[islat]);
// Initialize slat module
	slat->Init(chamber);
// y-position of slat module relative to the first (closest to the beam)
	fYPosition[islat]= fYPosOrigin+islat*(fSlatY-2.*fShift);
//
	fNpy+=slat->Npy();
	if (slat->Npx() > fNpx) fNpx=slat->Npx();
	Int_t isec;
	for (isec=0; isec< 4; isec++)
	{
	    fSlatX[islat]+=40.*fPcb[islat][isec];
	}
	
    }
// Set parent chamber number
    AliMUON *pMUON  = (AliMUON *) gAlice->GetModule("MUON");
    fChamber=&(pMUON->Chamber(chamber));
    fId=chamber;
}





void AliMUONSegmentationSlat::SetNPCBperSector(Int_t *npcb)
{ 
    //  PCB distribution for station 4 (6 rows with 1+3 segmentation regions)
    for (Int_t islat=0; islat<fNSlats; islat++){ 
	fPcb[islat][0] = *(npcb + 4 * islat);
	fPcb[islat][1] = *(npcb + 4 * islat + 1);
	fPcb[islat][2] = *(npcb + 4 * islat + 2);
	fPcb[islat][3] = *(npcb + 4 * islat + 3);
    }
}


void  AliMUONSegmentationSlat::SetSlatXPositions(Float_t *xpos)
{
// Set x-positions of Slats
    for (Int_t islat=0; islat<fNSlats; islat++) fXPosition[islat]=xpos[islat];
}

AliMUONSegmentationSlatModule*  AliMUONSegmentationSlat::Slat(Int_t index) const
  //PH { return ((AliMUONSegmentationSlatModule*) (*fSlats)[index]);}
{ return ((AliMUONSegmentationSlatModule*) fSlats->At(index));}


AliMUONSegmentationSlatModule* AliMUONSegmentationSlat::
CreateSlatModule()
{
    // Factory method for slat module
    return new AliMUONSegmentationSlatModule(4);
}


void AliMUONSegmentationSlat::Draw(const char* opt) const
{
// Draw method for event display
// 
  if (!strcmp(opt,"eventdisplay")) { 
    const int kColorMUON1 = kYellow;
    const int kColorMUON2 = kBlue; 
    //
    //  Drawing Routines for example for Event Display
    Int_t i,j;
    Int_t npcb[15];
    char nameChamber[9], nameSlat[9], nameNode[9];
    
    //
    // Number of modules per slat
    for (i=0; i<fNSlats; i++) {
      npcb[i]=0;
      for (j=0; j<4; j++) npcb[i]+=fPcb[i][j];
    }  
    //
    TNode* top=gAlice->GetGeometry()->GetNode("alice");
    sprintf(nameChamber,"C_MUON%d",fId+1);
    new TBRIK(nameChamber,"Mother","void",340,340,5.);
    top->cd();
    sprintf(nameNode,"MUON%d",100+fId+1);
    TNode* node = new TNode(nameNode,"Chambernode",nameChamber,0,0,fChamber->Z(),"");
    
    node->SetLineColor(kBlack);
    AliMUON *pMUON  = (AliMUON *) gAlice->GetModule("MUON");
    (pMUON->Nodes())->Add(node);
    TNode* nodeSlat;
    Int_t color;
    
    for (j=0; j<fNSlats; j++)
      {
	sprintf(nameSlat,"SLAT%d",100*fId+1+j);
	Float_t dx = 20.*npcb[j];
	Float_t dy = 20;
	new TBRIK(nameSlat,"Slat Module","void",dx,20.,0.25);
	node->cd();
	color =  TMath::Even(j) ? kColorMUON1 : kColorMUON2;
	
	sprintf(nameNode,"SLAT%d",100*fId+1+j);
	nodeSlat = 
	  new TNode(nameNode,"Slat Module",nameSlat, dx+fXPosition[j],fYPosition[j]+dy,0,"");
	nodeSlat->SetLineColor(color);
	node->cd();
	sprintf(nameNode,"SLAT%d",100*fId+1+j+fNSlats);
	nodeSlat = 
	  new TNode(nameNode,"Slat Module",nameSlat,-dx-fXPosition[j],fYPosition[j]+dy,0,"");
	nodeSlat->SetLineColor(color);
      }
  }
}




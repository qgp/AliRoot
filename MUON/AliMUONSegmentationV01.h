#ifndef ALIMUONSEGMENTATIONV01_H
#define ALIMUONSEGMENTATIONV01_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/////////////////////////////////////////////////////
//  Segmentation and Response classes version 01   //
/////////////////////////////////////////////////////
class AliMUON;
class TObjArray;

#include "AliMUONSegmentationV0.h"
#include "TArrayI.h" // because the object, and not the pointer,
#include "TArrayF.h" // belongs to the class


class AliMUONSegmentationV01 :
public AliMUONSegmentationV0 {
 public:
    AliMUONSegmentationV01();
    AliMUONSegmentationV01(Int_t nsec);
    AliMUONSegmentationV01(const AliMUONSegmentationV01 & segmentation);
    
    virtual ~AliMUONSegmentationV01();
    
    //    
    // Set Chamber Segmentation Parameters
    // 
    virtual  void    SetPadDivision(Int_t ndiv[4]);
    // Radii
    virtual  void    SetSegRadii(Float_t  r[4]);
    virtual  void    SetOffsetY(Float_t off) {fOffsetY = off;}
    //
    // Transform from pad (wire) to real coordinates and vice versa
    //
    // Transform from pad to real coordinates
    virtual void    GetPadI(Float_t x ,Float_t y ,Int_t   &ix,Int_t &iy);
    virtual void    GetPadI(Float_t x, Float_t y , Float_t z, Int_t &ix, Int_t &iy);
    // Transform from real to pad coordinates
    virtual void    GetPadC(Int_t   ix,Int_t   iy,Float_t &x ,Float_t &y );
    virtual void    GetPadC(Int_t ix, Int_t iy, Float_t &x, Float_t &y, Float_t &z) 
	{z=fZ; GetPadC(ix, iy, x , y);}
    //
    // Initialisation
    virtual void Init(Int_t chamber);
    //
    // Get member data
    //
    // Pad size in x by Sector
    virtual Float_t Dpx(Int_t isec) const;
    // Pad size in y by Sector
    virtual Float_t Dpy(Int_t isec) const;
    // Max number of Pads in x
    virtual Int_t   Npx() const {return fNpxS[fNsec-1][1]+1;}
    //
    virtual void    SetPad(Int_t ix,Int_t iy);
    //
    // Iterate over pads
    // Initialiser
    virtual void  FirstPad(Float_t xhit, Float_t yhit, Float_t dx, Float_t dy);
    virtual void  FirstPad(Float_t xhit, Float_t yhit, Float_t zhit, Float_t dx, Float_t dy);
    // Stepper
    virtual void  NextPad();
    // Condition
    virtual Int_t MorePads();
    // Get next neighbours 
    virtual void Neighbours
	(Int_t iX, Int_t iY, Int_t* Nlist, Int_t Xlist[10], Int_t Ylist[10]);
    //
    // Current Pad during Integration
    // current sector
    virtual Int_t ISector()  {return fSector;}
    // calculate sector from pad coordinates
    virtual Int_t Sector(Int_t ix, Int_t iy);
    //
    // Integration
    // Current integration limits
     virtual void IntegrationLimits
	(Float_t& x1, Float_t& x2, Float_t& y1, Float_t& y2);
    // Test points for auto calibration
    void GiveTestPoints(Int_t &n, Float_t *x, Float_t *y) const;
    //
    // Draw segmentation zones
    virtual void Draw(const char *opt="") const;
    // Function for systematic corrections
    // Set the correction function
    virtual void SetCorrFunc(Int_t dum, TF1* func);
    // Get the correction function
    virtual TF1* CorrFunc(Int_t iZone) const;
    // assignment operator
    AliMUONSegmentationV01& operator=(const AliMUONSegmentationV01& rhs);
    ClassDef(AliMUONSegmentationV01,1) // Segmentation approximating circular zones with different pad size
 protected:
    //  Geometry
    //
    Int_t       fNsec;           // Number of sectors
    TArrayF*    fRSec;           // Sector outer radia
    TArrayI*    fNDiv;           // Pad size division
    TArrayF*    fDpxD;           // y pad width per sector
    Float_t     fOffsetY;        // Staggering offset in y
    // Segmentation map
    Int_t      fNpxS[10][1000];  //  Number of pads per sector in x
    Float_t    fCx[10][1000];    //  pad-sector contour x vs y  
    // Chamber region consideres during disintegration
    // (lower left and upper right corner)
    //
    Float_t fXmin; // ! lower left  x
    Float_t fXmax; // ! lower left  y
    Float_t fYmin; // ! upper right x
    Float_t fYmax; // ! upper right y 

    //
    // Current pad during integration (cursor for disintegration)
    Int_t   fSector;   // ! Current sector
    //
    TObjArray *fCorrA; // ! Array of correction functions
};
#endif









#ifndef ALIMUONSEGMENTATIONV0_H
#define ALIMUONSEGMENTATIONV0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliSegmentation.h"

class AliMUONChamber;
class TF1;

//----------------------------------------------
//
// Chamber segmentation for homogeneously segmented circular chamber
//
class AliMUONSegmentationV0 :
public AliSegmentation {
 public:
    AliMUONSegmentationV0(){fCorr=0;fChamber=0;}
    AliMUONSegmentationV0(const AliMUONSegmentationV0 & segmentation);
    
    virtual ~AliMUONSegmentationV0(){}
    // Set Chamber Segmentation Parameters
    //
    // Pad size Dx*Dy 
    virtual  void    SetPadSize(Float_t p1, Float_t p2);
    // Anod Pitch
    virtual  void    SetDAnod(Float_t D) {fWireD = D;};
    // Transform from pad (wire) to real coordinates and vice versa
    //
    // Anod wire coordinate closest to xhit
    virtual Float_t GetAnod(Float_t xhit) const;
    // Transform from pad to real coordinates
    virtual void    GetPadI(Float_t x, Float_t y , Int_t &ix, Int_t &iy) ;
    virtual void    GetPadI(Float_t x, Float_t y , Float_t z, Int_t &ix, Int_t &iy)  
	{GetPadI(x, y, ix, iy);}
    // Transform from real to pad coordinates
    virtual void    GetPadC(Int_t ix, Int_t iy, Float_t &x, Float_t &y) ;
    virtual void    GetPadC(Int_t ix, Int_t iy, Float_t &x, Float_t &y, Float_t &z) 
	{z=fZ; GetPadC(ix, iy, x , y);}
    //
    // Initialisation
    virtual void Init(Int_t chamber);
    //
    // Get member data
    //
    // Pad size in x
    virtual Float_t Dpx() const {return fDpx;}
    // Pad size in y
    virtual Float_t Dpy() const {return fDpy;}
    // Pad size in x by Sector
    virtual Float_t Dpx(Int_t) const {return fDpx;}
    // Pad size in y by Secto
    virtual Float_t Dpy(Int_t) const {return fDpy;}
    // Maximum number of Pads in x
    virtual Int_t   Npx() const {return fNpx;}
    // Maximum number of Pads in y
    virtual Int_t   Npy() const {return fNpy;}
    // Set pad position
    virtual void     SetPad(Int_t ix, Int_t iy);
    // Set hit position
    virtual void     SetHit(Float_t xhit, Float_t yhit);
    virtual void     SetHit(Float_t xhit, Float_t yhit, Float_t zhit)
	{SetHit(xhit, yhit);}
    //
    // Iterate over pads
    // Initialiser
    virtual void  FirstPad(Float_t xhit, Float_t yhit, Float_t dx, Float_t dy);
    virtual void  FirstPad(Float_t xhit, Float_t yhit, Float_t zhit, Float_t dx, Float_t dy)
	{FirstPad(xhit, yhit, dx, dy);}
    // Stepper
    virtual void  NextPad();
    // Condition
    virtual Int_t MorePads();
    //
    // Distance between 1 pad and a position
    virtual Float_t Distance2AndOffset(Int_t iX, Int_t iY, Float_t X, Float_t Y, Int_t *
dummy);
    // Number of pads read in parallel and offset to add to x 
    // (specific to LYON, but mandatory for display)
    virtual void GetNParallelAndOffset(Int_t iX, Int_t iY,
        Int_t *Nparallel, Int_t *Offset) {*Nparallel=1;*Offset=0;}
    // Get next neighbours 
    virtual void Neighbours
	(Int_t iX, Int_t iY, Int_t* Nlist, Int_t Xlist[10], Int_t Ylist[10]) ;
    //
    // Current Pad during Integration
    // x-coordinaten
    virtual Int_t  Ix() {return fIx;}
    // y-coordinate
    virtual Int_t  Iy() {return fIy;}
    // current sector
    virtual Int_t  ISector() {return 1;}
    // calculate sector from pad coordinates
    virtual Int_t  Sector(Int_t ix, Int_t iy)  {return 1;}
    virtual Int_t  Sector(Float_t x, Float_t y)  {return 1;}
    //
    // Signal Generation Condition during Stepping
    virtual Int_t SigGenCond(Float_t x, Float_t y, Float_t z) ;
    // Initialise signal gneration at coord (x,y,z)
    virtual void  SigGenInit(Float_t x, Float_t y, Float_t z);
    // Current integration limits
    virtual void IntegrationLimits
	(Float_t& x1, Float_t& x2, Float_t& y1, Float_t& y2);
    // Test points for auto calibration
    virtual void GiveTestPoints(Int_t &n, Float_t *x, Float_t *y) const;
    // Draw segmentation zones
    virtual void Draw(const char *opt="") const;
    // Function for systematic corrections
    // Set the correction function
    virtual void SetCorrFunc(Int_t dum, TF1* func) {fCorr=func;}
    // Get the correction Function
    virtual TF1* CorrFunc(Int_t) const {return fCorr;}
    // assignment operator
    AliMUONSegmentationV0& operator=(const AliMUONSegmentationV0& rhs);
    
    ClassDef(AliMUONSegmentationV0,1) //Class for homogeneous segmentation
	protected:
    //
    // Implementation of the segmentation class:
    // Version 0 models rectangular pads with the same dimensions all
    // over the cathode plane. Chamber has circular geometry.
    // 
    //  Geometry parameters
    //
    Float_t    fDpx;           // x pad width per sector  
    Float_t    fDpy;           // y pad base width
    Int_t      fNpx;           // Number of pads in x
    Int_t      fNpy;           // Number of pads in y
    Float_t    fWireD;         // wire pitch
    Float_t    fRmin;          // inner radius
    Float_t    fRmax;          // outer radius
    
    
    // Chamber region consideres during disintegration   
    Int_t fIxmin; // ! lower left  x
    Int_t fIxmax; // ! lower left  y
    Int_t fIymin; // ! upper right x
    Int_t fIymax; // ! upper right y 
    //
    // Current pad during integration (cursor for disintegration)
    Int_t fIx;  // ! pad coord.  x 
    Int_t fIy;  // ! pad coord.  y 
    Float_t fX; // ! real coord. x
    Float_t fY; // ! real ccord. y
    //
    // Current pad and wire during tracking (cursor at hit centre)
    //
    //
    Float_t fXhit;  // ! x-position of hit
    Float_t fYhit;  // ! y-position of hit
    // Reference point to define signal generation condition
    Int_t fIxt;     // ! pad coord. x
    Int_t fIyt;     // ! pad coord. y
    Int_t fIwt;     // ! wire number
    Float_t fXt;    // ! x
    Float_t fYt;    // ! y
    TF1*    fCorr;  // ! correction function
    //
    AliMUONChamber* fChamber; // ! Reference to mother chamber
    Int_t fId;                // Identifier
    Float_t fZ;               // z-position of chamber
};
#endif












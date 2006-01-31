#ifndef ALIMUONST345SLATSEGMENTATIONV2_H
#define ALIMUONST345SLATSEGMENTATIONV2_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// \ingroup base
/// \class AliMUONSt345SlatSegmentationV2
/// \brief Segmentation for slat modules

#ifndef ALIMUONVGEOMETRYDESEGMENTATION_H
#include "AliMUONVGeometryDESegmentation.h"
#endif

#ifndef ALI_MP_PLANE_TYPE_H
#include "AliMpPlaneType.h"
#endif

#ifndef ALI_MP_PAD_H
#include "AliMpPad.h"
#endif

class AliMpSlat;
class AliMpSlatSegmentation;
class AliMpVPadIterator;

class AliMUONSt345SlatSegmentationV2 : public AliMUONVGeometryDESegmentation
{
 public:

  AliMUONSt345SlatSegmentationV2();
  AliMUONSt345SlatSegmentationV2(Int_t detElemId,
				 AliMpPlaneType bendingOrNonBending);
  virtual ~AliMUONSt345SlatSegmentationV2();

  void FirstPad(Float_t xhit, Float_t yhit, Float_t zhit, 
		Float_t dx, Float_t dy);

  void  NextPad();

  Int_t MorePads();

  Int_t  Ix();
  Int_t  Iy();
  Int_t  ISector();

  Float_t Distance2AndOffset(Int_t ix, Int_t iy, 
			     Float_t X, Float_t Y, 
			     Int_t* dummy);

  void GetNParallelAndOffset(Int_t iX, Int_t iY,
			     Int_t* Nparallel, Int_t* Offset);

  void Neighbours(Int_t iX, Int_t iY, Int_t* Nlist, 
		  Int_t Xlist[10], Int_t Ylist[10]);

  Int_t  Sector(Int_t ix, Int_t iy);
  Int_t  Sector(Float_t x, Float_t y);

  void  IntegrationLimits(Float_t& x1, Float_t& x2, 
			  Float_t& y1, Float_t& y2);

  void GiveTestPoints(Int_t& n, Float_t* x, Float_t* y) const;

  Int_t SigGenCond(Float_t x, Float_t y, Float_t z);
  void SigGenInit(Float_t x, Float_t y, Float_t z);

  void SetCorrFunc(Int_t,  TF1*);

  TF1* CorrFunc(Int_t) const;

  void SetPadSize(float,float);

  void SetDAnod(float);

  void Init(int);
  void Draw(Option_t * = "");

  Float_t Dpx() const;
  Float_t Dpy() const;
  
  Float_t Dpx(int) const;
  Float_t Dpy(int) const;
  
  Float_t GetAnod(Float_t xhit) const;

  Int_t Npx() const;
  Int_t Npy() const;

  /// Sets the current pad.
  void SetPad(Int_t ix,Int_t iy);
  
  /// Sets the current hit.
  void SetHit(Float_t x, Float_t y, Float_t z_is_not_used);
 
  AliMUONGeometryDirection GetDirection();// { return kDirUndefined; }

  const AliMpVSegmentation* GetMpSegmentation() const;

  /// to be deprecated. Use the one below w/o z instead.
  void GetPadC(Int_t ix, Int_t iy, Float_t& x, Float_t& y, Float_t& z);

  /// From pad indices to coordinates (cm).
  void GetPadC(Int_t ix, Int_t iy, Float_t& x, Float_t& y);

  // Transform from pad to real coordinates
  void GetPadI(Float_t x ,Float_t y ,Int_t   &ix,Int_t &iy);

  // to be deprecated. Use the one above w/o z instead.
  void GetPadI(Float_t x, Float_t y , Float_t z, Int_t &ix, Int_t &iy);


  /// Whether a pad exists at a given position.
  Bool_t HasPad(Float_t x, Float_t y, Float_t z);

  /// Whether a pad exists, given its indices.
  Bool_t HasPad(Int_t ix, Int_t iy);
  
  /// Print.
  void Print(Option_t* opt = "") const;

 private:

   void ReadMappingData();
	 
 private:

  Int_t fDetElemId;
	AliMpPlaneType fPlaneType;
  const AliMpSlat* fSlat; //!
  AliMpSlatSegmentation* fSlatSegmentation; //!
  AliMpVPadIterator* fPadIterator; //!
  AliMpPad fCurrentPad; //!FIXME: should not be needed, if we externalise the SetPad, SetHit, IntegrationLimits methods which have nothing to do here anyway, together with the iteration methods FirstPad, NextPad, MorePads, which have nothing to do here either.
  Float_t fXhit; //!
  Float_t fYhit; //!
  ClassDef(AliMUONSt345SlatSegmentationV2,1)
};

#endif

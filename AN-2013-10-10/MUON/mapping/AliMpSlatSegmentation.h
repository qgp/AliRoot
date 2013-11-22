/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpSlatSegmentation.h,v 1.12 2006/05/24 13:58:24 ivana Exp $

/// \ingroup slat
/// \class AliMpSlatSegmentation
/// \brief Implementation of AliMpVSegmentation for St345 slats.
/// 
/// Note that integer indices start at (0,0) on the bottom-left of the slat,
/// while floating point positions are relative to the center of the slat
/// (where the slat is to be understood as N PCBs of fixed size = 40cm
/// even if not all pads of a given PCBs are actually physically there).
///
/// \author Laurent Aphecetche

#ifndef ALI_MP_SLAT_SEGMENTATION_H
#define ALI_MP_SLAT_SEGMENTATION_H

#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ALI_MP_V_SEGMENTATION_H
#include "AliMpVSegmentation.h"
#endif

#ifndef ALI_MP_PAD_H
#include "AliMpPad.h"
#endif

class AliMpMotifPosition;
class AliMpPCB;
class AliMpSlat;

class AliMpSlatSegmentation : public AliMpVSegmentation
{
 public:
  AliMpSlatSegmentation();
  AliMpSlatSegmentation(const AliMpSlat* slat, Bool_t own = false);
  virtual ~AliMpSlatSegmentation();

  virtual AliMpVPadIterator* CreateIterator(const AliMpArea& area) const;
  virtual AliMpVPadIterator* CreateIterator() const;

  virtual Int_t GetNeighbours(const AliMpPad& pad, TObjArray& neighbours,
                              Bool_t includeSelf=kFALSE,
                              Bool_t includeVoid=kFALSE) const;
  
  const char* GetName() const;
  
  Int_t MaxPadIndexX() const;
  Int_t MaxPadIndexY() const;
  Int_t NofPads() const;
  
  virtual AliMpPad PadByLocation(Int_t manuId, Int_t manuChannel, 
			 Bool_t warning) const;

  virtual AliMpPad PadByIndices(Int_t ix, Int_t iy,  
			Bool_t warning) const;

  virtual AliMpPad PadByPosition(Double_t x, Double_t y,
			 Bool_t warning) const;

  virtual void Print(Option_t* opt) const;
  
  const AliMpSlat* Slat() const;
  
  void GetAllElectronicCardIDs(TArrayI& ecn) const;
  
  virtual AliMp::PlaneType PlaneType() const;
   
  virtual AliMp::StationType StationType() const;
  
  virtual Double_t  GetDimensionX() const;
  virtual Double_t  GetDimensionY() const;
  
  virtual Bool_t HasPadByIndices(Int_t ix, Int_t iy) const;
  
  virtual Bool_t HasPadByLocation(Int_t manuId, Int_t manuChannel) const;

  virtual Int_t GetNofElectronicCards() const;
  
  virtual Double_t  GetPositionX() const;
  virtual Double_t  GetPositionY() const;
  
  virtual Bool_t HasMotifPosition(Int_t manuId) const;
  
  virtual AliMpMotifPosition* MotifPosition(Int_t manuId) const;
  
 private:
  /// Not implemented
  AliMpSlatSegmentation(const AliMpSlatSegmentation& right);
  /// Not implemented
  AliMpSlatSegmentation&  operator = (const AliMpSlatSegmentation& right);

  const AliMpSlat* fkSlat;  ///< Slat
  Bool_t           fIsOwner;///< Slat ownership     

  ClassDef(AliMpSlatSegmentation,2) // A slat for stations 3,4,5
};

/// Return station type
inline AliMp::StationType AliMpSlatSegmentation::StationType() const
{ return AliMp::kStation345; }

#endif

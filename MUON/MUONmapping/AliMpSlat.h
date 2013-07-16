/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpSlat.h,v 1.8 2006/05/24 13:58:24 ivana Exp $

/// \ingroup slat
/// \class AliMpSlat
/// \brief A slat (building block of stations 3, 4 and 5)
/// 
//  Author: Laurent Aphecetche

#ifndef ALI_MP_SLAT_H
#define ALI_MP_SLAT_H

#ifndef ALI_MP_PAD_H
#  include "AliMpPad.h"
#endif

#ifndef ALI_MP_V_SEGMENTATION_H
#  include "AliMpVSegmentation.h"
#endif

#ifndef ALI_MP_PLANE_TYPE_H
#  include "AliMpPlaneType.h"
#endif

#ifndef ALI_MP_EX_MAP_H
  #include "AliMpExMap.h"
#endif

#ifndef ROOT_TObject
  #include <TObject.h>
#endif

#ifndef ROOT_TString
#  include "TString.h"
#endif

#ifndef ROOT_TObjArray
#  include "TObjArray.h"
#endif

class TArrayI;


class AliMpMotifPosition;
class AliMpPCB;
class TArrayI;

class AliMpSlat : public TObject
{
 public:

  AliMpSlat(TRootIOCtor* ioCtor);
  AliMpSlat(const char* id, AliMp::PlaneType bendingOrNonBending);
  virtual ~AliMpSlat();

  /// Return x position
  Double_t  GetPositionX() const { return fPositionX; }
  /// Return y position
  Double_t  GetPositionY() const { return fPositionY; }
  
  const char* GetName() const;
  
  const char* GetID() const;

  void Add(const AliMpPCB& pcbType, const TArrayI& manuList);

  Double_t DX() const;
  Double_t DY() const;

  /// Find the PCB containing the pad at location (ix,any iy).
  AliMpPCB* FindPCB(Int_t ix) const;

  /// Find the index of the PCB containing the pad at location ix.
  Int_t FindPCBIndex(Int_t ix) const;

  /// Find the index of the PCB containing a given manu
  Int_t FindPCBIndexByMotifPositionID(Int_t manuId) const;
  
  /// Find the PCB containing location (x,y).
  AliMpPCB* FindPCB(Double_t x, Double_t y) const;

  /// Find the index of the PCB containing the pad at location (x,y).
  Int_t FindPCBIndex(Double_t x, Double_t y) const;

  /// Returns the i-th PCB of this slat.
  AliMpPCB* GetPCB(Int_t i) const;

  /// Returns the MotifPosition containing location (x,y).
  AliMpMotifPosition* FindMotifPosition(Double_t x, Double_t y) const;

  /// Returns the MotifPosition which id is manuid.
  AliMpMotifPosition* FindMotifPosition(Int_t manuid) const;

  /// Returns the MotifPosition containing the pad located at (ix,iy).
  AliMpMotifPosition* FindMotifPosition(Int_t ix, Int_t iy) const;

  /// Return the ids of the electronic cards (either manu or local board).
  void GetAllMotifPositionsIDs(TArrayI& ecn) const;
  
  /** Returns the max. number of pads in the x-direction contained in this slat.
    This is a max only as for e.g. non-bending slats, the y-dimension depends
    on the x-position.
    */
  Int_t GetMaxNofPadsY() const;
  
  /** Returns the max index useable in x-direction. 
    Note that this can be different from GetNofPadsX()-1 for rounded slats.
    */
  Int_t GetMaxPadIndexX() const;
  
  /// Return the number of electronic cards (either manu or local board).
  Int_t GetNofElectronicCards() const;
  
  /// Returns the number of pads in the x-direction contained in this slat.
  Int_t GetNofPadsX() const;
 
  /// Returns the number of PCBs of this slat.
  Int_t GetSize() const;
    
  void Print(Option_t* option="") const;

  /** This is normally only used by triggerSlats, as for ST345 slats,
    the position is DX(),DY() simply.
    */
  void ForcePosition(Double_t x, Double_t y);
  
  /// Return the plane type
  AliMp::PlaneType PlaneType() const { return fPlaneType; }
  
  /// Return the number of pads in this slat
  Int_t NofPads() const { return fNofPads; }
 
 private:
  /// Not implemented
  AliMpSlat();
  /// Not implemented
  AliMpSlat(const AliMpSlat& rhs);
  /// Not implemented
  AliMpSlat& operator=(const AliMpSlat& rhs);

  TString fId; ///< The name of this slat, e.g. 112233N
  AliMp::PlaneType fPlaneType; ///< Whether it's bending or non-bending plane
  Double_t fDX; ///< Half-size in X (cm)
  Double_t fDY; ///< Half-size in Y (cm)
  Int_t fNofPadsX; ///< Actual number of pads in x direction
  Int_t fMaxNofPadsY; ///< Maximum number of pads in y direction
  mutable AliMpExMap fManuMap; ///< map of int to AliMpMotifPosition*
  TObjArray fPCBs; ///< array of AliMpPCB*
  Double_t fPositionX; ///< x Position of the slat center.
  Double_t fPositionY; ///< y Position of the slat center.
  Int_t fNofPads; ///< number of pads in this slat
  
  ClassDef(AliMpSlat,3) // A slat for stations 3,4,5
};

#endif

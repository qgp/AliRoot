// $Id$

#ifndef SPACEPOINTDATA_H
#define SPACEPOINTDATA_H

#include "Rtypes.h"

/**
 * @struct AliHLTTPCSpacePointData
 * Primitive data exchange structure for TPC clusters.
 * Together with the AliHLTTPCClusterDataFormat this defines
 * the output of the TPC online Cluster Finder.
 *
 * To translate between local coordinates, global coordinates, and row-pad-time coordinates 
 * one cann use AliHLTTPCTransform. 
 *
 * See AliHLTTPCClusterFinder::WriteClusters() for example. 
 *
 * @ingroup alihlt_tpc_datastructs
 */
struct AliHLTTPCSpacePointData{
  Float_t fPad;     // Pad coordinate in raw coordinates
  Float_t fTime;    // Time coordinate in raw coordinates
  Float_t fX;       // X coordinate in local coordinates
  Float_t fY;       // Y coordinate in local coordinates
  Float_t fZ;       // Z coordinate in local coordinates
  UInt_t fID;       // contains slice patch and number
  UChar_t fPadRow;  // Pad row number
  Float_t fSigmaY2; // error (former width) of the clusters
  Float_t fSigmaZ2; // error (former width) of the clusters
  UInt_t fCharge;   // total charge of cluster
  UInt_t fQMax;     // QMax of cluster
  Bool_t fUsed;     // only used in AliHLTTPCDisplay 
  Int_t fTrackN;    // only used in AliHLTTPCDisplay 

  void SetPad(Float_t pad)         {fPad=pad;}
  void SetTime(Float_t time)       {fTime=time;}
  void SetX(Float_t x)             {fX=x;}
  void SetY(Float_t y)             {fY=y;}
  void SetZ(Float_t z)             {fZ=z;}
  void SetID(UInt_t id)            {fID=id;}
  void SetPadRow(Short_t padrow)   {fPadRow=padrow;}
  void SetSigmaY2(Float_t sigmaY2) {fSigmaY2=sigmaY2;}
  void SetSigmaZ2(Float_t sigmaZ2) {fSigmaZ2=sigmaZ2;}
  void SetCharge(UShort_t charge)  {fCharge=charge;}
  void SetQMax(UShort_t qmax)      {fQMax=qmax;}

  Float_t  GetPad()     const      {return fPad;}
  Float_t  GetTime()    const      {return fTime;}
  Float_t  GetX()       const      {return fX;}
  Float_t  GetY()       const      {return fY;}
  Float_t  GetZ()       const      {return fZ;}
  UInt_t   GetID()      const      {return fID;}
  UShort_t GetPadRow()  const      {return fPadRow;}
  Float_t  GetSigmaY2() const      {return fSigmaY2;}
  Float_t  GetSigmaZ2() const      {return fSigmaZ2;}
  UShort_t GetCharge()  const      {return fCharge;}
  UShort_t GetQMax()    const      {return fQMax;}

  static UInt_t GetSlice( UInt_t Id )  { return (Id>>25)&0x3F; }
  static UInt_t GetPatch( UInt_t Id )  { return (Id>>22)&0x7; }
  static UInt_t GetNumber( UInt_t Id ) { return Id&0x003FFFFF; }
  static UInt_t GetID( UInt_t Slice, UInt_t Patch, UInt_t Number ){
    return ((Slice&0x3F)<<25)+((Patch&0x7)<<22) + (Number&0x003FFFFF);
  }

  AliHLTTPCSpacePointData() 
  : fPad(0.), fTime(0.), fX(0.), fY(0.), fZ(0.), fID(0), fPadRow(0), fSigmaY2(0.), fSigmaZ2(0.), fCharge(0), fQMax(0), fUsed(kFALSE), fTrackN(0) {}
  void SetID( UInt_t Slice, UInt_t Patch, UInt_t Number ){
    fID = GetID(Slice, Patch,Number);
  }
  UInt_t GetSlice() const { return GetSlice(fID); }
  UInt_t GetPatch() const { return GetPatch(fID); }
  UInt_t GetNumber() const { return GetNumber(fID); }

  Bool_t IsUsed() const {return fUsed;}
  void SetUsed(Bool_t used) {fUsed=used;}
  Int_t GetTrackNumber() const {return fTrackN;}
  void SetTrackNumber(Int_t trackN) {fTrackN=trackN;}
};
typedef struct AliHLTTPCSpacePointData AliHLTTPCSpacePointData;


#endif /* SPACEPOINTDATA_H */

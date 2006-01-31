#ifndef ALIALIGNOBJ_H
#define ALIALIGNOBJ_H

//************************************************************************
// AliAlignObj: alignment base class for the storage of alignment        *
//   information for a single volume, that is a translation, a rotation  *
//   and a the identity of the volume itself in form of a TGeo path and  *
//   as a unique integer identifier                                      *
//************************************************************************
#include "TObject.h"
#include "TString.h"
#include "TGeoMatrix.h"

class AliAlignObj : public TObject {

 public:

  AliAlignObj();
  AliAlignObj(const AliAlignObj& theAlignObj);
  AliAlignObj& operator= (const AliAlignObj& theAlignObj);
  virtual ~AliAlignObj();
  enum ELayerID{kSPD1=1, kSPD2=2,
		kSDD1=3, kSDD2=4,
		kSSD1=5, kSSD2=6,
		kTPC1=7, kTPC2=8,
		kTRD1=9, kTRD2=10, kTRD3=11, kTRD4=12, kTRD5=13, kTRD6=14,
		kTOF=15,
		kPHOS1=16, kPHOS2=17,
		kRICH=18,
		kMUON=19};

  //Setters
  virtual void SetTranslation(Double_t x, Double_t y, Double_t z) = 0;
  virtual void SetTranslation(const TGeoMatrix& m) = 0;
  virtual void SetRotation(Double_t psi, Double_t theta, Double_t phi) = 0;
  virtual Bool_t SetRotation(const TGeoMatrix& m) = 0;
  virtual void SetPars(Double_t x, Double_t y, Double_t z, Double_t psi,
               Double_t theta, Double_t phi) = 0;
  virtual void SetMatrix(const TGeoMatrix& m) = 0;
  void  SetVolPath(const TString& volpath) {fVolPath=volpath;}
  void  SetVolUID(UShort_t voluid) {fVolUID=voluid;}
  void  SetVolUID(ELayerID layerId, Int_t modId);

  //Getters
  const char  *GetVolPath()    const {return fVolPath.Data();}
  UShort_t     GetVolUID()     const {return fVolUID;}
  void         GetVolUID(ELayerID &layerId, Int_t &modId) const;
  virtual void GetTranslation(Double_t* tr)  const=0;
  virtual Bool_t GetAngles(Double_t* angles) const=0;
  virtual void GetPars(Double_t transl[], Double_t rot[]) const=0;
  virtual void GetMatrix(TGeoHMatrix& m) const=0;

  void  Print(Option_t *) const;

  static UShort_t LayerToVolUID(ELayerID layerId, Int_t modId);
  static ELayerID VolUIDToLayer(UShort_t voluid, Int_t &modId);
  static ELayerID VolUIDToLayer(UShort_t voluid);
 protected:

  void AnglesToMatrix(const Double_t *angles, Double_t *rot) const;
  Bool_t MatrixToAngles(const Double_t *rot, Double_t *angles) const;

  //Volume identifiers
  TString fVolPath; // Volume path inside TGeo geometry
  UShort_t fVolUID; // Unique volume ID
  
  ClassDef(AliAlignObj, 1)
};

#endif

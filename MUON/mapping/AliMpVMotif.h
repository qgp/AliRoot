// $Id$
// Category: motif
//
// Class AliMpVMotif
// -----------------
// Class that defines a motif with its unique ID
// and the motif type.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_V_MOTIF_H
#define ALI_MP_V_MOTIF_H

#include <TObject.h>
#include <TString.h>
#include <TVector2.h>

class AliMpMotifType;
class AliMpConnection;
class AliMpIntPair;

class AliMpVMotif : public TObject
{
 public:
  AliMpVMotif(const TString &id, AliMpMotifType *motifType);
  AliMpVMotif();

  // Access methods
  AliMpMotifType  *GetMotifType() const;
  TString          GetID() const;
  virtual Int_t    GetNofPadDimensions() const=0;
  virtual TVector2 GetPadDimensions(Int_t i) const=0;
  virtual TVector2 GetPadDimensions(const AliMpIntPair& localIndices) const=0;

  // Geometry
  virtual TVector2 Dimensions() const=0;

  // Other methods
  AliMpConnection *FindConnectionByLocalPos(const TVector2& localPos) const;
  virtual void Print(Option_t *option) const;
  virtual TVector2     PadPositionLocal(const AliMpIntPair& localIndices) const=0;
  virtual AliMpIntPair PadIndicesLocal(const TVector2& localPos) const=0;

 private:
  // methods

  // data members 
  TString         fID;            //identifier
  AliMpMotifType *fMotifType;     //the motif type

  ClassDef(AliMpVMotif,1) // A motif with its ID
};

// inline functions

inline  AliMpMotifType* AliMpVMotif::GetMotifType() const {return fMotifType;}
inline  TString  AliMpVMotif::GetID() const {return fID;}

#endif //ALI_MP_V_MOTIF_H

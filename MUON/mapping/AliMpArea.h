// $Id$
// Category: basic
//
// Class AliMpArea
// ----------------
// Class that defines a rectangle area positioned in plane..
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_AREA_H
#define ALI_MP_AREA_H

#include <TObject.h>
#include <TVector2.h>

class AliMpArea : public TObject
{
 public:
  AliMpArea(const TVector2& position, const TVector2& dimensions);
  AliMpArea(const AliMpArea& rhs);
  AliMpArea();
  virtual ~AliMpArea();

  // operators
  AliMpArea& operator = (const AliMpArea& right);

  // methods
  Double_t LeftBorder() const;
  Double_t RightBorder() const;
  Double_t UpBorder() const;
  Double_t DownBorder() const;

  TVector2 LeftDownCorner() const;
  TVector2 LeftUpCorner() const;
  TVector2 RightDownCorner() const;
  TVector2 RightUpCorner() const;

  // get methods
  TVector2  Position() const;
  TVector2  Dimensions() const;    
  Bool_t    IsValid() const;
  
 private:
  // data members
  TVector2  fPosition;  // position
  TVector2  fDimensions;// dimensions (half lengths)
  Bool_t    fValidity;  // validity

  ClassDef(AliMpArea,1) //utility class for area iterators
};

ostream& operator << (ostream &stream,const AliMpArea& area);

// inline functions

inline TVector2  AliMpArea::Position() const   { return fPosition; }
inline TVector2  AliMpArea::Dimensions() const { return fDimensions; }    
inline Bool_t    AliMpArea::IsValid() const    { return fValidity; }

#endif //ALI_MP_AREA_H

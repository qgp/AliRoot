// $Id$
// Category: basic
//
// Class AliMpPad
// ---------------
// Class which encapsuate all informations about a pad
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_PAD_H
#define ALI_MP_PAD_H

#include <TObject.h>
#include <TVector2.h>

#include "AliMpIntPair.h"

class AliMpPad : public TObject
{
 public:
  AliMpPad(const AliMpIntPair& location, const AliMpIntPair& indices,
           const TVector2& position, const TVector2& dimensions,
           Bool_t validity = true);
  AliMpPad();
  AliMpPad(const AliMpPad& src);
  virtual ~AliMpPad();

  // operators  
  Bool_t operator == (const AliMpPad& pos2) const;
  Bool_t operator != (const AliMpPad& pos2) const;
  AliMpPad& operator = (const AliMpPad& src) ;
  
  // methods
  void Print() const;

  // static get methods
  static AliMpPad Invalid() {return AliMpPad();}

  // get methods
  AliMpIntPair GetLocation() const {return fLocation  ;}
  AliMpIntPair GetIndices()  const {return fIndices   ;}
  TVector2 Position()    const {return fPosition  ;}
  TVector2 Dimensions()  const {return fDimensions;}
  Bool_t   IsValid()     const {return fValidity  ;}

 private:
  // unused derived functions
  virtual void Print(const char* option) const {}

  // data members
  AliMpIntPair  fLocation;  //pad location
  AliMpIntPair  fIndices;   //pad indices
  TVector2  fPosition;  // the pad position (in cm)
  TVector2  fDimensions;// the pad dimensions (in cm)
  Bool_t    fValidity;  // validity

  ClassDef(AliMpPad,1) //utility class for the motif type
};

ostream& operator << (ostream &out, const AliMpPad& op);
Bool_t operator < (const AliMpPad& left, const AliMpPad& right);

#endif //ALI_MP_PAD_H

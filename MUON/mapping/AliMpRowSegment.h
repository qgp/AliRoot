// $Id$
// Category: sector
//
// Class AliMpRowSegment
// ---------------------
// Class describing a row segment composed of the 
// the identic motifs.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_ROW_SEGMENT_H
#define ALI_MP_ROW_SEGMENT_H

#include <TVector2.h>

#include "AliMpVRowSegment.h"

class AliMpRow;
class AliMpVMotif;

class AliMpRowSegment : public AliMpVRowSegment
{
  public:
    AliMpRowSegment(AliMpRow* row, AliMpVMotif* motif, AliMpIntPair padOffset, 
                Int_t nofMotifs, Int_t motifPositionId, Int_t motifPositionDId);
    AliMpRowSegment();
    virtual ~AliMpRowSegment();

    // methods
    virtual Double_t  LeftBorderX() const;
    virtual Double_t  RightBorderX() const;
    virtual Double_t  HalfSizeY() const;

    // find methods
    virtual AliMpVMotif*  FindMotif(const TVector2& position) const;    
    virtual Int_t     FindMotifPositionId(const TVector2& position) const;
    virtual Bool_t    HasMotifPosition(Int_t motifPositionId) const;
    virtual TVector2  MotifCenter(Int_t motifPositionId) const;

    // geometry
    virtual TVector2  Position() const;
    virtual TVector2  Dimensions() const;

    // set methods
    virtual void      SetOffset(const TVector2& offset);
    virtual void      SetGlobalIndices();
    virtual Int_t     SetIndicesToMotifPosition(Int_t i, AliMpIntPair indices);

    // get methods
    virtual AliMpRow*     GetRow() const;
    virtual Int_t         GetNofMotifs() const;
    virtual AliMpVMotif*  GetMotif(Int_t i) const;
    virtual Int_t         GetMotifPositionId(Int_t i) const;

  private:
    // methods
    Double_t  FirstMotifCenterX() const;
    Double_t  LastMotifCenterX() const;
    Double_t  MotifCenterX(Int_t motifPositionId) const;
    Double_t  MotifCenterY(Int_t motifPositionId) const;
    Bool_t    IsInside(const TVector2& position, Bool_t warn = true) const;

    // data members
    Int_t         fNofMotifs;  //number of motifs
    AliMpIntPair  fPadOffset;  //the offset in nof pads 
    TVector2      fOffset;     //the position of the centre of the first motif
                               //(x wtr to left border, y wtr to row center)
    AliMpRow*     fRow;        //the row containing this segment 
    AliMpVMotif*  fMotif;      //the motif 
    Int_t   fMotifPositionId;  // the first motif position id
    Int_t   fMotifPositionDId; // +1 if ids are increasing, -1 if decreasing
    
  ClassDef(AliMpRowSegment,1)  //Row segment
};

#endif //ALI_MP_ROW_SEGMENT_H


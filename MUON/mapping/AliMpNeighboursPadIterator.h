// $Id$
// Category: sector
//
// Class AliMpNeighboursPadIterator
// --------------------------------
// Class, which defines an iterator over the pads surrounding a given pad
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_NEIGHBOURS_PAD_ITERATOR_H
#define ALI_MP_NEIGHBOURS_PAD_ITERATOR_H

#include <TObject.h>
#include <TVector2.h>

#include "AliMpSectorTypes.h"
#include "AliMpVPadIterator.h"
#include "AliMpMotifTypePadIterator.h"
#include "AliMpIntPair.h"
#include "AliMpPad.h"

class AliMpSectorSegmentation;

class AliMpNeighboursPadIterator : public AliMpVPadIterator
{
  public:
    AliMpNeighboursPadIterator();
    AliMpNeighboursPadIterator(const AliMpSectorSegmentation* segmentation,
                               const AliMpPad& centerPad,
                               Bool_t includeCenter=kFALSE);
    AliMpNeighboursPadIterator(const AliMpNeighboursPadIterator& right);
    virtual ~AliMpNeighboursPadIterator();

    // operators
    AliMpNeighboursPadIterator& 
      operator = (const AliMpNeighboursPadIterator& right);

    // methods
    virtual void First();
    virtual void Next();
    virtual Bool_t IsDone() const;
    virtual AliMpPad CurrentItem() const;
    virtual void Invalidate();

  private:
    // static members
    static const UInt_t   fgkInvalidIndex;

    // private methods
    Bool_t    IsNeighbours(const AliMpPad& pad) const;
    PadVector PadVectorLine(const AliMpPad& from,
                            const AliMpIntPair& direction) const;
    void      FillPadsVector(Bool_t includeCenter);
    Bool_t    IsValid() const;

    // private data members
    const AliMpSectorSegmentation* fkSegmentation; // The sector segmentation 
                                                   // over which to iterate
    AliMpPad   fCenterPad; // Pad arround which we iterate
    PadVector  fPads;      // The list of pad arround fCenterIndices
    UInt_t     fIndex;     // Current index inside the fPads vector

  ClassDef(AliMpNeighboursPadIterator,1) // iterator over motif's pads
};

#endif // ALI_MP_NEIGHBOURS_PAD_ITERATOR_H

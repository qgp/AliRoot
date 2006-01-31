/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpNeighboursPadIterator.h,v 1.8 2005/09/26 16:12:11 ivana Exp $

/// \ingroup sector
/// \class AliMpNeighboursPadIterator
/// \brief An iterator over the pads surrounding a given pad
///
/// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_NEIGHBOURS_PAD_ITERATOR_H
#define ALI_MP_NEIGHBOURS_PAD_ITERATOR_H

#include "AliMpContainers.h"

#ifdef WITH_STL
#include <vector>
#include <set>
#endif

#ifdef WITH_ROOT
#include <TObjArray.h>
#endif

#include "AliMpVPadIterator.h"
#include "AliMpPad.h"

class AliMpSectorSegmentation;
class AliMpIntPair;

class AliMpNeighboursPadIterator : public AliMpVPadIterator
{
  public:
#ifdef WITH_STL
    typedef std::vector<AliMpPad>  PadVector;
    typedef std::set<AliMpPad>     PadSet;
    typedef PadSet::const_iterator PadSetCIterator;
#endif
#ifdef WITH_ROOT
    typedef TObjArray  PadVector;
    typedef TObjArray  PadSet;
#endif

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
    static const UInt_t   fgkInvalidIndex; // invalid index number

    // private methods
    Bool_t    IsNeighbours(const AliMpPad& pad) const;
#ifdef WITH_STL
    PadVector PadVectorLine(const AliMpPad& from,
                            const AliMpIntPair& direction) const;
    void      UpdateTotalSet(PadSet& setTotal, const PadVector& from) const;
#endif
#ifdef WITH_ROOT
    PadVector* PadVectorLine(const AliMpPad& from,
                             const AliMpIntPair& direction) const;
    void      UpdateTotalSet(PadSet& setTotal, PadVector* from) const;
#endif
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

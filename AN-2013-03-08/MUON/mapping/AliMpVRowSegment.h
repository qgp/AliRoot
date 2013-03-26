/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpVRowSegment.h,v 1.9 2006/05/24 13:58:21 ivana Exp $

/// \ingroup sector
/// \class AliMpVRowSegment
/// \brief An interface for a row segment.
///
/// \author David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_V_ROW_SEGMENT_H
#define ALI_MP_V_ROW_SEGMENT_H

#include "AliMpVIndexed.h"

class AliMpRow;
class AliMpVMotif;

class AliMpVRowSegment : public AliMpVIndexed
{
  public:
    AliMpVRowSegment();
    virtual ~AliMpVRowSegment();

    //
    // methods  
    //
    
    /// Return the x coordinate of the left border in the global coordinate system.
    virtual Double_t  LeftBorderX() const = 0;
    /// Return the x coordinate of the right border in the global coordinate system.
    virtual Double_t  RightBorderX() const = 0;
    /// Return the half size in y of this row segment.
    virtual Double_t  HalfSizeY() const = 0;
    virtual AliMpVPadIterator* CreateIterator() const;

    //
    // find methods
    //

    /// Find the motif in the given positions
    virtual AliMpVMotif*  FindMotif(Double_t x, Double_t y) const = 0;    
    /// Find the motif position Id in the given positions
    virtual Int_t     FindMotifPositionId(Double_t x, Double_t y) const = 0;
    /// Has the motif position with the given Id ?
    virtual Bool_t    HasMotifPosition(Int_t motifPositionId) const = 0;

    /// Fill the coordinates of the motif specified with the given motif position Id
    virtual void  MotifCenter(Int_t motifPositionId,
                               Double_t& x, Double_t& y) const = 0;

    //
    // geometry
    //
    
    /// Return the x position of the row segment centre.
    virtual Double_t  GetPositionX() const = 0;
    /// Return the y position of the row segment centre.
    virtual Double_t  GetPositionY() const = 0;

    /// Return the x dimension of the row segment centre.
    virtual Double_t  GetDimensionX() const = 0;
    /// Return the y dimension of the row segment centre.
    virtual Double_t  GetDimensionY() const = 0;
   
    //
    // set methods
    //

    /// Calculate offset
    virtual void      SetOffset(Double_t x, Double_t y) = 0;
    /// Set global indices limits.
    virtual void      SetGlobalIndices(AliMpRow* rowBefore) = 0;
    /// Set global indices to i-th motif position and returns next index in x.
    virtual Int_t     SetIndicesToMotifPosition(Int_t i, MpPair_t indices) = 0;
    
    //
    // get methods
    //
    
    /// Return the row.which this row segment belongs to
    virtual AliMpRow*  GetRow() const = 0;
    /// Return the number of motifs in this this row segment.
    virtual Int_t      GetNofMotifs() const = 0;
    /// Return the i-th motif of this row segment.
    virtual AliMpVMotif*  GetMotif(Int_t i) const = 0;
    /// Return the i-th motif position Id of this row segment.
    virtual Int_t      GetMotifPositionId(Int_t i) const = 0;
    
  ClassDef(AliMpVRowSegment,1)  //Row segment
};

#endif //ALI_MP_V_ROW_SEGMENT_H


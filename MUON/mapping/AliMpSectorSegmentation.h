// $Id$
// Category: sector
//
// Class AliMpSectorSegmentation
// -----------------------------
// Class describing the segmentation of the sector.        
// Provides methods related to pads:
// conversion between pad indices, pad location, pad position;
// finding pad neighbour.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_SECTOR_SEGMENTATION_H
#define ALI_MP_SECTOR_SEGMENTATION_H

#include <TVector2.h>

#include "AliMpSectorTypes.h"
#include "AliMpVSegmentation.h"
#include "AliMpIntPair.h"
#include "AliMpPadPair.h"
#include "AliMpPad.h"
#include "AliMpArea.h"

class AliMpSector;
class AliMpMotifPosition;
class AliMpVPadIterator;

class AliMpSectorSegmentation : public AliMpVSegmentation
{
  public:
    AliMpSectorSegmentation(const AliMpSector* sector);
    AliMpSectorSegmentation();
    virtual ~AliMpSectorSegmentation();
    
    // factory methods  
    virtual AliMpVPadIterator* CreateIterator(const AliMpArea& area) const;
    AliMpVPadIterator* CreateIterator(const AliMpPad& centerPad,
                                  Bool_t includeCenter=kFALSE) const;

    // methods  
    virtual AliMpPad PadByLocation(const AliMpIntPair& location,
                               Bool_t warning = kTRUE) const;
    virtual AliMpPad PadByIndices (const AliMpIntPair& indices, 
                               Bool_t warning = kTRUE) const;
    virtual AliMpPad PadByPosition(const TVector2& position ,
                               Bool_t warning = kTRUE) const;
    virtual AliMpPad PadByDirection(const TVector2& startPosition, 
                               Double_t distance) const;

    virtual Int_t    Zone(const AliMpPad& pad, Bool_t warning = kTRUE) const;
    virtual TVector2 PadDimensions(Int_t zone, Bool_t warning = kTRUE) const;

    virtual Bool_t HasPad(const AliMpIntPair& indices) const;
    Bool_t HasMotifPosition(Int_t motifPositionID) const;
    TVector2 GetMinPadDimensions() const;
    Bool_t CircleTest(const AliMpIntPair& indices) const;

  private:
    // methods
    void  FillPadDimensionsMap();
    AliMpMotifPosition*  FindMotifPosition(const AliMpIntPair& indices) const;
    virtual AliMpPad PadByXDirection(const TVector2& startPosition, 
                                     Double_t maxX) const;
    virtual AliMpPad PadByYDirection(const TVector2& startPosition, 
                                     Double_t maxY) const;
    virtual AliMpVPadIterator* CreateIterator() const;
 
    // data members        
    const AliMpSector*  fkSector;   // Sector
    AliMpPad*           fPadBuffer; // The pad buffer
    PadDimensionsMap    fPadDimensionsMap; //! Map between zone IDs and pad dimensions
                              // EXCLUDED FOR CINT (does not compile on HP)    

  ClassDef(AliMpSectorSegmentation,1)  // Segmentation
};

#endif //ALI_MP_SECTOR_SEGMENTATION_H


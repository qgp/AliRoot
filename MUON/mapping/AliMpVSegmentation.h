// $Id$
// Category: basic
//
// Class AliMpVSegmentation
// ------------------------
// The abstract base class for the segmentation.
// Provides methods related to pads:
// conversion between pad indices, pad location, pad position;
// finding pad neighbour.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_V_SEGMENTATION_H
#define ALI_MP_V_SEGMENTATION_H

#include <TObject.h>
#include <TVector2.h>

#include "AliMpIntPair.h"
#include "AliMpPadPair.h"
#include "AliMpPad.h"
#include "AliMpArea.h"

class AliMpVPadIterator;

class AliMpVSegmentation : public TObject
{
  public:
    AliMpVSegmentation();
    virtual ~AliMpVSegmentation();
  
    // factory method 
    virtual AliMpVPadIterator* CreateIterator(const AliMpArea& area) const = 0;

    // methods  
    virtual AliMpPad PadByLocation(const AliMpIntPair& location, 
                               Bool_t warning) const = 0;
    virtual AliMpPad PadByIndices (const AliMpIntPair& indices,  
                               Bool_t warning) const = 0;
    virtual AliMpPad PadByPosition(const TVector2& position,
                               Bool_t warning) const = 0;

    virtual AliMpPadPair PadsUp(const AliMpPad& pad) const;
    virtual AliMpPadPair PadsDown(const AliMpPad& pad) const;
    virtual AliMpPadPair PadsLeft(const AliMpPad& pad) const;
    virtual AliMpPadPair PadsRight(const AliMpPad& pad) const;

    virtual Bool_t HasPad(const AliMpIntPair& indices) const = 0;
    
  private:  
    // methods
    AliMpPadPair FindPads(const TVector2& position1, 
                          const TVector2& position2) const;

  ClassDef(AliMpVSegmentation,1)  // Segmentation
};

#endif //ALI_MP_V_SEGMENTATION_H


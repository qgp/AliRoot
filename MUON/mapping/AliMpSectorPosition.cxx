// $Id$
// Category: plane
//
// Class AliMpSectorPosition
// -------------------------
// Class that represents a placed sector.
// Only translation + reflection transformations can
// be applied.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#include "AliMpSectorPosition.h"

ClassImp(AliMpSectorPosition)

//______________________________________________________________________________
AliMpSectorPosition::AliMpSectorPosition(const AliMpSector* sector,
                                         const TVector2& offset, 
				         const AliMpIntPair& scale) 
  : TObject(),
    fkSector(sector),
    fOffset(offset),
    fScale(scale)
{
//
}

//_____________________________________________________________________________
AliMpSectorPosition::AliMpSectorPosition() 
  : TObject(),
    fkSector(),
    fOffset(),
    fScale()
{
//
}

//_____________________________________________________________________________
AliMpSectorPosition::~AliMpSectorPosition() {
// 
}

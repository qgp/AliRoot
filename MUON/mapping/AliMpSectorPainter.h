// $Id$
// Category: graphics
//
// Class AliMpSectorPainter
// ------------------------
// Class for drawing a sector into canvas
//
// Authors: David Guez, IPN Orsay

#ifndef M_SECTOR_PAINTER_H
#define M_SECTOR_PAINTER_H

#include "AliMpVPainter.h"

class AliMpSector;

class AliMpSectorPainter : public AliMpVPainter
{
 public:
  AliMpSectorPainter();
  AliMpSectorPainter(AliMpSector *sector);
  virtual void Draw(Option_t *option);
  virtual void Paint(Option_t *option);
  virtual void DumpObject(); // *MENU*
  virtual TVector2 GetPosition() const;
  virtual TVector2 GetDimensions() const;
 private:
  AliMpSector *fSector;          // the sector to draw
  ClassDef(AliMpSectorPainter,1) // Sector painter
};
#endif //M_SECTOR_PAINTER_H

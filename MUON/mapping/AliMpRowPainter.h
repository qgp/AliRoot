// $Id$
// Category: graphics
//
// Class AliMpRowPainter
// ---------------------
// Class for drawing a row into canvas
//
// Authors: David Guez, IPN Orsay

#ifndef ALI_MP_ROW_PAINTER_H
#define ALI_MP_ROW_PAINTER_H

#include "AliMpVPainter.h"

class AliMpRow;

class AliMpRowPainter : public AliMpVPainter
{
 public:
  AliMpRowPainter();
  AliMpRowPainter(AliMpRow *row);
  virtual void DumpObject(); //*MENU*
  virtual void Draw(Option_t *option);
  virtual void Paint(Option_t *option);
  virtual TVector2 GetPosition() const;
  virtual TVector2 GetDimensions() const;
 private: 
  AliMpRow *fRow;             // the row to paint
  ClassDef(AliMpRowPainter,1) // Row painter
};
#endif //ALI_MP_ROW_PAINTER_H

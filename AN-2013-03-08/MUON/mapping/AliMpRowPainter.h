/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpRowPainter.h,v 1.8 2006/05/24 13:58:13 ivana Exp $

/// \ingroup mpgraphics
/// \class AliMpRowPainter
/// \brief Class for drawing a row into canvas
///
/// \author David Guez, IPN Orsay

#ifndef ALI_MP_ROW_PAINTER_H
#define ALI_MP_ROW_PAINTER_H

#include "AliMpVPainter.h"

class AliMpRow;

class AliMpRowPainter : public AliMpVPainter
{
 public:
  AliMpRowPainter();
  AliMpRowPainter(AliMpRow *row);
  virtual ~AliMpRowPainter();
  
  virtual void DumpObject(); //-MENU-
  virtual void Draw(Option_t *option);
  virtual void Paint(Option_t *option);
  virtual TVector2 GetPosition() const;
  virtual TVector2 GetDimensions() const;

 private: 
  /// Not implemented
  AliMpRowPainter(const AliMpRowPainter& right);
  /// Not implemented
  AliMpRowPainter&  operator = (const AliMpRowPainter& right);

  AliMpRow *fRow; ///< the row to paint
  
  ClassDef(AliMpRowPainter,1) // Row painter
};
#endif //ALI_MP_ROW_PAINTER_H

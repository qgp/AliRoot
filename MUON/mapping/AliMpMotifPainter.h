/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpMotifPainter.h,v 1.7 2006/05/23 13:07:35 ivana Exp $

/// \ingroup graphics
/// \class AliMpMotifPainter
/// \brief Class for drawing a motif into canvas
//
/// Authors: David Guez, IPN Orsay

#ifndef ALI_MP_MOTIF_PAINTER_H
#define ALI_MP_MOTIF_PAINTER_H

#include "AliMpVPainter.h"

class AliMpMotifPosition;

class AliMpMotifPainter : public AliMpVPainter
{
 public:
  AliMpMotifPainter();
  AliMpMotifPainter(AliMpMotifPosition *motifPos);
  virtual ~AliMpMotifPainter();
  
  virtual void DumpObject(); //-MENU-
  virtual void Paint(Option_t *option);
  virtual TVector2 GetPosition() const;
  virtual TVector2 GetDimensions() const;

 protected:
  AliMpMotifPainter(const AliMpMotifPainter& right);
  AliMpMotifPainter&  operator = (const AliMpMotifPainter& right);

 private:
  AliMpMotifPosition *fMotifPos; ///< the motif to draw

  ClassDef(AliMpMotifPainter,1) // Motif painter
};
#endif //ALI_MP_MOTIF_PAINTER_H

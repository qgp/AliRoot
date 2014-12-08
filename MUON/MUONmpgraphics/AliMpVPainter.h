/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpVPainter.h,v 1.8 2006/05/24 13:58:13 ivana Exp $

/// \ingroup mpgraphics
/// \class AliMpVPainter
/// \brief Abstract base class for drawing objects into canvas
///
/// \author David Guez, IPN Orsay

#ifndef ALI_MP_V_PAINTER_H
#define ALI_MP_V_PAINTER_H

#include <TObject.h>
#include <TVector2.h>

class TList;

class AliMpVPainter : public TObject
{
 public:
  AliMpVPainter();
  virtual ~AliMpVPainter();

  void DumpObject() const; // *MENU*
          /// Paint the associated object
  virtual void Paint(Option_t *option)=0;
  virtual TObject* Clone(const char* newname="") const;
  virtual TObject* DrawClone(Option_t* option) const; // *MENU*

  //
  // get methods
  //
           /// Return the position inside the graphics pad
  TVector2 GetPadPosition() const {return fPadPosition;}
           /// Return the dimensions inside the graphics pad
  TVector2 GetPadDimensions() const {return fPadDimensions;}
           /// Return the color
  Int_t GetColor() const {return fColor;}

  //
  // set methods
  //
           /// Set the position inside the graphics pad
  void SetPadPosition(const TVector2 &padPosition){fPadPosition=padPosition;}
           /// Set the dimensions inside the graphics pad
  void SetPadDimension(const TVector2 &padDimensions){fPadDimensions=padDimensions;}
           /// Set the color
  void SetColor(Int_t color){fColor=color;}

  //
  // methods
  //
  Bool_t IsInside(const TVector2 &point,const TVector2& pos,const TVector2& dim);
          /// Return the owned object's position
  virtual TVector2 GetPosition() const=0;
          /// Return the owned object's dimensions
  virtual TVector2 GetDimensions() const=0;
  void InitGraphContext();
  void PaintWholeBox(Bool_t fill=kTRUE);
  virtual Int_t DistancetoPrimitive(Int_t x, Int_t y);
  TVector2 RealToPad(const TVector2& realPos);

  static AliMpVPainter *CreatePainter(TObject *object);

 protected:
  /// Not implemented
  void AddPainter(AliMpVPainter *painter);
  /// Not implemented
  AliMpVPainter *DrawObject(TObject *object,Option_t *option="");

 private:
  /// Not implemented
  AliMpVPainter(const AliMpVPainter& right);
  /// Not implemented
  AliMpVPainter&  operator = (const AliMpVPainter& right);

  Int_t fColor;            ///< color
  TVector2 fPadPosition;   ///< position inside the graphics pad
  TVector2 fPadDimensions; ///< dimensions inside the graphics pad
  TList *fTrashList;       ///< list of painter object created

  ClassDef(AliMpVPainter,1) // abstract object painter
};

#endif //ALI_MP_V_PAINTER_H

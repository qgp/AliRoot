// $Id$
// Category: graphics
//
// Class AliMpVPainter
// --------------
// Class for drawing objects into canvas
//
// Authors: David Guez, IPN Orsay

#ifndef ALI_MP_V_PAINTER_H
#define ALI_MP_V_PAINTER_H

#include <TObject.h>
#include <TClass.h>
#include <TVector2.h>
#include <TVirtualX.h>
#include <TPad.h>
#include <TList.h>

class AliMpVPainter : public TObject
{
 public:
  AliMpVPainter();
  virtual ~AliMpVPainter();
  void DumpObject() const; // *MENU*
  virtual void Paint(Option_t *option)=0;
  virtual TObject* Clone(const char* newname="") const;
  virtual TObject* DrawClone(Option_t* option) const; // *MENU*
  // get methods
  TVector2 GetPadPosition() const {return fPadPosition;}
  TVector2 GetPadDimensions() const {return fPadDimensions;}
  Int_t GetColor() const {return fColor;}

  // set methods
  void SetPadPosition(const TVector2 &padPosition){fPadPosition=padPosition;}
  void SetPadDimension(const TVector2 &padDimensions){fPadDimensions=padDimensions;}
  void SetColor(Int_t color){fColor=color;}

  // methods
  Bool_t IsInside(const TVector2 &point,const TVector2& pos,const TVector2& dim);
  virtual TVector2 GetPosition() const=0;
  virtual TVector2 GetDimensions() const=0;
  void InitGraphContext();
  void PaintWholeBox(Bool_t fill=kTRUE);
  virtual Int_t DistancetoPrimitive(Int_t x, Int_t y);
  TVector2 RealToPad(const TVector2& realPos);

  static AliMpVPainter *CreatePainter(TObject *object);
 protected:
  void AddPainter(AliMpVPainter *painter);
  AliMpVPainter *DrawObject(TObject *object,Option_t *option="");
 private:
  Int_t fColor;               //  color
  TVector2 fPadPosition;      // position inside the graphics pad
  TVector2 fPadDimensions;    // dimensions inside the graphics pad
  TList *fTrashList;           // list of painter object created
  ClassDef(AliMpVPainter,1) // abstract object painter
};

#endif //ALI_MP_V_PAINTER_H

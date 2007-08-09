// $Header$

#ifndef REVE_NLTProjectorGL_H
#define REVE_NLTProjectorGL_H

#include <TGLObject.h>
#include <Reve/RenderElement.h>

class TGLViewer;
class TGLScene;
class TGLText;

namespace Reve {
class NLTProjector;
class NLTProjectorGL : public TGLObject
{ 
public:
  typedef std::list<Float_t> TMList_t;
private:
  NLTProjectorGL(const NLTProjectorGL&);            // Not implemented
  NLTProjectorGL& operator=(const NLTProjectorGL&); // Not implemented

  // tick marks
  mutable TMList_t           fPos;
  mutable TMList_t           fVals;

  Int_t              fSplitLevel;
  Bool_t             fSplitByVal;
  void               SplitInterval(Int_t axis) const;
  void               SplitIntervalByPos(Float_t min, Float_t max, Int_t axis, Int_t level)const; 
  void               SplitIntervalByVal(Float_t min, Float_t max, Int_t axis, Int_t level)const; 

protected:
  NLTProjector*      fM; // fModel dynamic-casted to NLTProjectorGL
  TGLText*           fText;

  virtual void DirectDraw(TGLRnrCtx & rnrCtx) const;

public:
  NLTProjectorGL();
  virtual ~NLTProjectorGL();

  virtual Bool_t SetModel(TObject* obj, const Option_t* opt=0);
  virtual void   SetBBox();
  Bool_t IgnoreSizeForOfInterest() const { return kTRUE;}

  // To support two-level selection
  // virtual Bool_t SupportsSecondarySelect() const { return kTRUE; }
  // virtual void ProcessSelection(TGLRnrCtx & rnrCtx, TGLSelectRecord & rec);

  ClassDef(NLTProjectorGL, 0);
}; // endclass NLTProjectorGL

}

#endif

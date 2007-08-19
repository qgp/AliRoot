// $Header$

#ifndef REVE_NLTProjectorEditor_H
#define REVE_NLTProjectorEditor_H

#include <TGedFrame.h>

class TGComboBox;

namespace Reve {

class NLTProjector;
class RGValuator;

class NLTProjectorEditor : public TGedFrame
{
private:
  NLTProjectorEditor(const NLTProjectorEditor&);            // Not implemented
  NLTProjectorEditor& operator=(const NLTProjectorEditor&); // Not implemented

protected:
  NLTProjector *fM; // fModel dynamic-casted to NLTProjectorEditor

  TGComboBox   *fType;
  RGValuator   *fDistortion;

public:
  NLTProjectorEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30, UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
  virtual ~NLTProjectorEditor();

  virtual void SetModel(TObject* obj);

  void DoType(Int_t type);
  void DoDistortion();

  ClassDef(NLTProjectorEditor, 0); // Editor for NLTProjector
}; // endclass NLTProjectorEditor

}

#endif

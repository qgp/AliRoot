// $Header$

#ifndef REVE_RGEditor_H
#define REVE_RGEditor_H

#include <TGedEditor.h>

namespace Reve {

class RenderElement;

class RGEditor : public TGedEditor
{
  RGEditor(const RGEditor&);            // Not implemented
  RGEditor& operator=(const RGEditor&); // Not implemented

protected:
  RenderElement* fRnrElement;

public:
  RGEditor(TCanvas* canvas=0);
  virtual ~RGEditor() {}

  RenderElement* GetRnrElement() const { return fRnrElement; }
  
  void DisplayRenderElement(RenderElement* re);
  void DisplayObject(TObject* obj);

  virtual void Update(TGedFrame* gframe=0);

  // virtual Bool_t HandleButton(Event_t *event);

  ClassDef(RGEditor, 1);
}; // endclass RGEditor

}

#endif

// $Header$

#include "RGEditor.h"
#include "RenderElement.h"
#include "RGTopFrame.h"

#include <TGedFrame.h>
#include <TGCanvas.h>
#include <TCanvas.h>

//______________________________________________________________________
// RGEditor
//

using namespace Reve;

ClassImp(RGEditor)

RGEditor::RGEditor(TCanvas* canvas) :
  TGedEditor(canvas),
  fRnrElement(0)
{
  Resize(250, 400);
}

void RGEditor::DisplayRenderElement(RenderElement* re)
{
  fRnrElement = re;
  TObject* obj = fRnrElement ? fRnrElement->GetObject() : 0;
  SetModel(fPad, obj, kButton1Down);
}

void RGEditor::DisplayObject(TObject* obj)
{
  fRnrElement = 0;
  SetModel(fPad, obj, kButton1Down);
}

void RGEditor::Update(TGedFrame* /*gframe*/)
{
  // Virtual method from TGedEditor ... called on every change.

  if (fRnrElement) {
    fRnrElement->UpdateItems();
    gReve->ElementChanged(fRnrElement);
  }

  gReve->Redraw3D();
}

/**************************************************************************/

/*
// Attempt to enable mouse-wheel in geditor -- failed.
Bool_t RGEditor::HandleButton(Event_t *event)
{
  // Handle mouse button event in container.

  printf("odfjgsf\n");
  if (event->fCode == kButton4 || event->fCode == kButton5) {
    return fCan->GetContainer()->HandleButton(event);
  } else {
    return TGedEditor::HandleButton(event);
  }
}
*/

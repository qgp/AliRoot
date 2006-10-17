// $Header$

#include "RenderElementEditor.h"
#include <Reve/RenderElement.h>

#include <TVirtualPad.h>
#include <TColor.h>

#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>
#include <TGDoubleSlider.h>

using namespace Reve;

//______________________________________________________________________
// RenderElementEditor
//

ClassImp(RenderElementEditor)

RenderElementEditor::RenderElementEditor(const TGWindow *p,
                                         Int_t width, Int_t height,
                                         UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),

  fRE         (0),
  fHFrame     (0),
  fRnrElement (0),
  fMainColor  (0)
{
  MakeTitle("RenderElement");
  fPriority = 0;

  fHFrame = new TGHorizontalFrame(this);

  fMainColor = new TGColorSelect(fHFrame, 0, -1);
  fHFrame->AddFrame(fMainColor, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fMainColor->Associate(fHFrame);
  fMainColor->Connect
    ("ColorSelected(Pixel_t)",
     "Reve::RenderElementEditor", this, "DoMainColor(Pixel_t)");

  fRnrElement = new TGCheckButton(fHFrame, "Render element");
  fHFrame->AddFrame(fRnrElement, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fRnrElement->Connect
    ("Toggled(Bool_t)",
     "Reve::RenderElementEditor", this, "DoRnrElement()");

  AddFrame(fHFrame, new TGLayoutHints(kLHintsTop, 0, 0, 1, 1));    
}

RenderElementEditor::~RenderElementEditor()
{}

/**************************************************************************/

void RenderElementEditor::SetModel(TObject* obj)
{
  fRE = dynamic_cast<RenderElement*>(obj);

  if (fRE->CanEditRnrElement()) {
    fRnrElement->SetState(fRE->GetRnrElement() ? kButtonDown : kButtonUp);
    fRnrElement->MapWindow();
  } else {
    fRnrElement->UnmapWindow();
  }

  if (fRE->CanEditMainColor()) {
    fMainColor->SetColor(TColor::Number2Pixel(fRE->GetMainColor()), kFALSE);
    fMainColor->MapWindow();
  } else {
    fMainColor->UnmapWindow();
  }

  fHFrame->Layout();
}

/**************************************************************************/


void RenderElementEditor::DoRnrElement()
{
  Bool_t fd = fRnrElement->IsOn();
  fRE->SetRnrElement(fd);
  Update();
}

void RenderElementEditor::DoMainColor(Pixel_t color)
{
  // Slot connected to the axis color.
   
  fRE->SetMainColor(color);
  Update();
}

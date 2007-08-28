// $Header$

#include "GridStepperEditor.h"
#include <Reve/GridStepper.h>
#include <Reve/RGValuators.h>

#include <TVirtualPad.h>
#include <TColor.h>

#include <TGLabel.h>
#include <TGSlider.h>
#include <TGButton.h>
#include <TGNumberEntry.h>

using namespace Reve;

//______________________________________________________________________
// GridStepperSubEditor
//
//

ClassImp(GridStepperSubEditor)

//______________________________________________________________________
GridStepperSubEditor::GridStepperSubEditor(const TGWindow *p) :
  TGVerticalFrame(p),
  fM             (0),

  fNx            (0), 
  fNy            (0),
  fNz            (0),
  fDx            (0), 
  fDy            (0),
  fDz            (0)
{
  Int_t labelW = 22;
  {
    TGHorizontalFrame* f = new TGHorizontalFrame(this);
    AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));

    fNx = new RGValuator(f,"Nx:", 200, 0);
    fNx->SetNELength(4);
    fNx->SetLabelWidth(labelW);
    fNx->SetShowSlider(kFALSE);
    fNx->Build();
    fNx->GetSlider()->SetWidth(224);
    fNx->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fNx->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoNs()");
    f->AddFrame(fNx, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    fNy = new RGValuator(f,"Nx:", 200, 0);
    fNy->SetNELength(4);
    fNy->SetLabelWidth(labelW);
    fNy->SetShowSlider(kFALSE);
    fNy->Build();
    fNy->GetSlider()->SetWidth(224);
    fNy->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fNy->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoNs()");
    f->AddFrame(fNy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    fNz = new RGValuator(f,"Nx:", 200, 0);
    fNz->SetNELength(4);
    fNz->SetLabelWidth(labelW);
    fNz->SetShowSlider(kFALSE);
    fNz->Build();
    fNz->GetSlider()->SetWidth(224);
    fNz->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fNz->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoNs()");
    f->AddFrame(fNz, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    AddFrame(f, new TGLayoutHints(kLHintsExpandX, 2, 0, 0, 0));
  }
  {
    TGHorizontalFrame* f = new TGHorizontalFrame(this);
    AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));

    fDx = new RGValuator(f,"Nx:", 200, 0);
    fDx->SetNELength(4);
    fDx->SetLabelWidth(labelW);
    fDx->SetShowSlider(kFALSE);
    fDx->Build();
    fDx->GetSlider()->SetWidth(224);
    fDx->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fDx->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoDs()");
    f->AddFrame(fDx, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    fDy = new RGValuator(f,"Nx:", 200, 0);
    fDy->SetNELength(4);
    fDy->SetLabelWidth(labelW);
    fDy->SetShowSlider(kFALSE);
    fDy->Build();
    fDy->GetSlider()->SetWidth(224);
    fDy->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fDy->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoDs()");
    f->AddFrame(fDy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    fDz = new RGValuator(f,"Nx:", 200, 0);
    fDz->SetNELength(4);
    fDz->SetLabelWidth(labelW);
    fDz->SetShowSlider(kFALSE);
    fDz->Build();
    fDz->GetSlider()->SetWidth(224);
    fDz->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
    fDz->Connect("ValueSet()",
		 "Reve::RGBAPaletteSubEditor", this, "DoDs()");
    f->AddFrame(fDz, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    AddFrame(f, new TGLayoutHints(kLHintsExpandX, 2, 0, 0, 0));
  }
}

//______________________________________________________________________
void GridStepperSubEditor::SetModel(GridStepper* m)
{
  // Set model object.
  fM = m;

  fNx->SetValue(m->Nx);
  fNx->SetValue(m->Ny);
  fNx->SetValue(m->Nz);

  fNx->SetValue(m->Dx);
  fNx->SetValue(m->Dy);
  fNx->SetValue(m->Dz);
}

//______________________________________________________________________
void GridStepperSubEditor::Changed()
{
  // Emit Changed signal.

  Emit("Changed()");
}

//______________________________________________________________________
void GridStepperSubEditor::DoNs()
{
   // Set some value from some widget
  fM->SetNs((Int_t)fNx->GetValue(), (Int_t)fNy->GetValue(), (Int_t)fNz->GetValue()); 
   Changed();
}

//______________________________________________________________________
void GridStepperSubEditor::DoDs()
{
   // Set some value from some widget
  fM->SetDs((Int_t)fDx->GetValue(), (Int_t)fDy->GetValue(),(Int_t)fDz->GetValue()); 
  Changed();
}

//______________________________________________________________________
// GridStepperEditor
//
//

ClassImp(GridStepperEditor)

//______________________________________________________________________
GridStepperEditor::GridStepperEditor(const TGWindow *p, Int_t width, Int_t height,
	     UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM  (0),
  fSE (0)
{
  // Constructor.

  MakeTitle("GridStepper");

  fSE = new GridStepperSubEditor(this);
  AddFrame(fSE, new TGLayoutHints(kLHintsTop, 2, 0, 2, 2));
  fSE->Connect("Changed()", "GridStepperEditor", this, "Update()");
}

/**************************************************************************/

//______________________________________________________________________
void GridStepperEditor::SetModel(TObject* obj)
{
  // Set model object.
  fM = dynamic_cast<GridStepper*>(obj);
  fSE->SetModel(fM);
}

// $Header$

#include "NLTProjectorEditor.h"
#include <Reve/NLTProjector.h>

#include <Reve/RGValuators.h>

#include <TGComboBox.h>
#include <TGLabel.h>

using namespace Reve;

//______________________________________________________________________
// NLTProjectorEditor
//

ClassImp(NLTProjectorEditor)

NLTProjectorEditor::NLTProjectorEditor(const TGWindow *p,
				       Int_t width, Int_t height,
				       UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0)
  // Initialize widget pointers to 0
{
  MakeTitle("NLTProjector");

  // Create widgets
  {
    TGHorizontalFrame* f = new TGHorizontalFrame(this);
    TGLabel* lab = new TGLabel(f, "Type");
    f->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsBottom, 1, 27, 1, 2));
    fType = new TGComboBox(f);
    fType->AddEntry("CFishEye", NLTProjection::PT_CFishEye);
    fType->AddEntry("RhoZ",     NLTProjection::PT_RhoZ);
    TGListBox* lb = fType->GetListBox();
    lb->Resize(lb->GetWidth(), 2*18);
    fType->Resize(80, 20);
    fType->Connect("Selected(Int_t)", "Reve::NLTProjectorEditor",
		   this, "DoType(Int_t)");
    f->AddFrame(fType, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
    AddFrame(f);
  }

  fDistortion = new RGValuator(this, "Distortion:", 90, 0);
  fDistortion->SetShowSlider(kFALSE);
  fDistortion->SetNELength(8);
  fDistortion->Build();
  fDistortion->SetLimits(0, 200, 100, TGNumberFormat::kNESRealTwo);
  fDistortion->SetToolTip("Minimal angular step between two helix points.");
  fDistortion->Connect("ValueSet(Double_t)", "Reve::NLTProjectorEditor",
		       this, "DoDistortion()");
  AddFrame(fDistortion, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
}

NLTProjectorEditor::~NLTProjectorEditor()
{}

/**************************************************************************/

void NLTProjectorEditor::SetModel(TObject* obj)
{
  fM = dynamic_cast<NLTProjector*>(obj);

  fType->Select(fM->GetProjection()->fType, kFALSE);  
  fDistortion->SetValue(1000.0f * fM->GetProjection()->fDistortion);
}

/**************************************************************************/

void NLTProjectorEditor::DoType(Int_t type)
{
  fM->SetProjection((NLTProjection::PType_e)type, 0.001f * fDistortion->GetValue());
  fM->ProjectChildren();
  Update();
}

void NLTProjectorEditor::DoDistortion()
{
  fM->GetProjection()->fDistortion = 0.001f * fDistortion->GetValue();
  fM->ProjectChildren();
  Update();
}

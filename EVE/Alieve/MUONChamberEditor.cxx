#include "MUONChamberEditor.h"

#include <Alieve/MUONChamber.h>

#include <TEveGValuators.h>

#include <TVirtualPad.h>
#include <TColor.h>

#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
using namespace Alieve;

//______________________________________________________________________
// MUONChamberEditor
//

ClassImp(MUONChamberEditor)

//______________________________________________________________________
MUONChamberEditor::MUONChamberEditor(const TGWindow *p,
                                     Int_t width, Int_t height,
                                     UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0),
  fThreshold(0),
  fMaxVal(0),
  fClusterSize(0)
{
  //
  // constructor
  //

  MakeTitle("MUONChamber");

  Int_t labelW = 60;

  fThreshold = new TEveGValuator(this, "ADC min", 200, 0);
  fThreshold->SetNELength(4);
  fThreshold->SetLabelWidth(labelW);
  fThreshold->Build();
  fThreshold->GetSlider()->SetWidth(120);
  fThreshold->SetLimits(0,4096);
  fThreshold->Connect("ValueSet(Double_t)",
                      "Alieve::MUONChamberEditor", this, "DoThreshold()");
  AddFrame(fThreshold, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

  fMaxVal = new TEveGValuator(this,"ADC max", 200, 0);
  fMaxVal->SetNELength(4);
  fMaxVal->SetLabelWidth(labelW);
  fMaxVal->Build();
  fMaxVal->GetSlider()->SetWidth(120);
  fMaxVal->SetLimits(0, 4096);
  fMaxVal->Connect("ValueSet(Double_t)",
                   "Alieve::MUONChamberEditor", this, "DoMaxVal()");
  AddFrame(fMaxVal, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

  fClusterSize = new TEveGValuator(this,"Cls size", 200, 0);
  fClusterSize->SetLabelWidth(labelW);
  fClusterSize->SetShowSlider(kFALSE);
  fClusterSize->SetNELength(4);
  fClusterSize->Build();
  fClusterSize->SetLimits(0, 24);
  fClusterSize->SetToolTip("Size of displayed clusters");
  fClusterSize->Connect("ValueSet(Double_t)",
                      "Alieve::MUONChamberEditor", this, "DoClusterSize()");
  AddFrame(fClusterSize, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

  fHitSize = new TEveGValuator(this,"TEveHit size", 200, 0);
  fHitSize->SetLabelWidth(labelW);
  fHitSize->SetShowSlider(kFALSE);
  fHitSize->SetNELength(4);
  fHitSize->Build();
  fHitSize->SetLimits(0, 24);
  fHitSize->SetToolTip("Size of displayed clusters");
  fHitSize->Connect("ValueSet(Double_t)",
                      "Alieve::MUONChamberEditor", this, "DoHitSize()");
  AddFrame(fHitSize, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

}

//______________________________________________________________________
MUONChamberEditor::~MUONChamberEditor()
{
  //
  // destructor
  //

}

//______________________________________________________________________
void MUONChamberEditor::SetModel(TObject* obj)
{
  //
  // ...
  //

  fM = dynamic_cast<MUONChamber*>(obj);

  fThreshold->SetValue(fM->fThreshold);
  fMaxVal->SetValue(fM->fMaxVal);
  fClusterSize->SetValue(fM->fClusterSize);
  fHitSize->SetValue(fM->fHitSize);

}

//______________________________________________________________________
void MUONChamberEditor::DoThreshold()
{
  //
  // set digit minimum amplitude
  //

  fM->SetThreshold((Short_t) fThreshold->GetValue());
  fThreshold->SetValue(fM->fThreshold);
  Update();

}

//______________________________________________________________________
void MUONChamberEditor::DoMaxVal()
{
  //
  // set digit maximum amplitude
  //

  fM->SetMaxVal((Int_t) fMaxVal->GetValue());
  fMaxVal->SetValue(fM->fMaxVal);
  Update();

}

//______________________________________________________________________
void MUONChamberEditor::DoClusterSize()
{
  //
  // set the cluster point size
  //

  fM->SetClusterSize((Int_t) fClusterSize->GetValue());
  fClusterSize->SetValue(fM->fClusterSize);
  Update();

}

//______________________________________________________________________
void MUONChamberEditor::DoHitSize()
{
  //
  // set the hit point size
  //

  fM->SetHitSize((Int_t) fHitSize->GetValue());
  fHitSize->SetValue(fM->fHitSize);
  Update();

}

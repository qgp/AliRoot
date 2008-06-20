// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveCosmicRayFitterEditor.h"
#include "AliEveCosmicRayFitter.h"

#include <TGButton.h>


//_____________________________________________________________________________
// AliEveCosmicRayFitterEditor
//

ClassImp(AliEveCosmicRayFitterEditor)

AliEveCosmicRayFitterEditor::AliEveCosmicRayFitterEditor(const TGWindow *p, Int_t width, Int_t height,
    UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0),
  fFit(0),
  fReset(0),
  fStart(0),
  fStop(0),
  fGraph(0)
{
  // Constructor.

  MakeTitle("AliEveCosmicRayFitter");

  fStart = new TGTextButton(this, "Start");
  AddFrame(fStart, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 4, 1, 3, 1));
  fStart->Connect("Clicked()",
                  "AliEveCosmicRayFitterEditor", this, "DoStart()");

  fFit = new TGTextButton(this, "Fit");
  AddFrame(fFit, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 4, 1, 1, 1));
  fFit->Connect("Clicked()",
                "AliEveCosmicRayFitterEditor", this, "DoFit()");

  fReset = new TGTextButton(this, "Reset");
  AddFrame(fReset, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 4, 1, 1, 1));
  fReset->Connect("Clicked()",
                  "AliEveCosmicRayFitterEditor", this, "DoReset()");

  fStop = new TGTextButton(this, "Stop");
  AddFrame(fStop, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 4, 1, 1, 4));
  fStop->Connect("Clicked()",
                 "AliEveCosmicRayFitterEditor", this, "DoStop()");

  fGraph = new TGTextButton(this, "DebugGraph");
  AddFrame(fGraph, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 4, 2, 4, 1));
  fGraph->Connect("Clicked()",
                 "AliEveCosmicRayFitterEditor", this, "DoGraph()");
 }

/******************************************************************************/

void AliEveCosmicRayFitterEditor::SetModel(TObject* obj)
{
  // Set model object.

  fM = dynamic_cast<AliEveCosmicRayFitter*>(obj);

  if(fM->GetConnected())
  {
    fStart->SetState(kButtonDisabled);
    fStop->SetState(kButtonUp);
  }
  else
  {
    fStop->SetState(kButtonDisabled);
    fStart->SetState(kButtonEngaged);
    fStart->SetState(kButtonUp);
  }
}

/**************************************************************************/

void AliEveCosmicRayFitterEditor::DoFit()
{
  // Fit slot.

  fM->FitTrack();
  Update();
}

void AliEveCosmicRayFitterEditor::DoReset()
{
  // Reset slot.

  fM->Reset();
  Update();
}

void AliEveCosmicRayFitterEditor::DoStart()
{
  // Start selection slot.

  fM->Start();
  fStart->SetState(kButtonDisabled);
  fStop->SetState(kButtonUp);
}

void AliEveCosmicRayFitterEditor::DoStop()
{
  // Stop selection slot.

  fM->Stop();
  fStop->SetState(kButtonDisabled);
  fStart->SetState(kButtonUp);
}

void AliEveCosmicRayFitterEditor::DoGraph()
{
  // Draw graph slot.

  fM->DrawDebugGraph();
  Update();
}

// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 * 
 **************************************************************************/

#include "AliEveTOFDigitsInfoEditor.h"
#include <Alieve/AliEveTOFDigitsInfo.h>

#include <TVirtualPad.h>
#include <TColor.h>

#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>
#include <TGDoubleSlider.h>


//______________________________________________________________________
// AliEveTOFDigitsInfoEditor
//

ClassImp(AliEveTOFDigitsInfoEditor)

AliEveTOFDigitsInfoEditor::AliEveTOFDigitsInfoEditor(const TGWindow *p, Int_t width, Int_t height,
	     UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0)
  // Initialize widget pointers to 0
{
  MakeTitle("AliEveTOFDigitsInfo");

  // Create widgets
  // fXYZZ = new TGSomeWidget(this, ...);
  // AddFrame(fXYZZ, new TGLayoutHints(...));
  // fXYZZ->Connect("SignalName()", "AliEveTOFDigitsInfoEditor", this, "DoXYZZ()");
}

AliEveTOFDigitsInfoEditor::~AliEveTOFDigitsInfoEditor()
{}

/**************************************************************************/

void AliEveTOFDigitsInfoEditor::SetModel(TObject* obj)
{
  fM = dynamic_cast<AliEveTOFDigitsInfo*>(obj);

  // Set values of widgets
  // fXYZZ->SetValue(fM->GetXYZZ());
}

/**************************************************************************/

// Implements callback/slot methods

// void AliEveTOFDigitsInfoEditor::DoXYZZ()
// {
//   fM->SetXYZZ(fXYZZ->GetValue());
//   Update();
// }

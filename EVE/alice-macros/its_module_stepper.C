#include "TGLViewer.h"
namespace Alieve
{
class ITSModuleStepper;
}

void its_module_stepper(Int_t det = 0, Int_t col = 5, Int_t row = 4)
{
  TFile *file = TFile::Open("ITS.Digits.root");
  TDirectory* dir = (TDirectory*) file->Get("Event0");
  TTree* tree =  (TTree*)dir->Get("TreeD");
  Alieve::ITSDigitsInfo* di = new Alieve::ITSDigitsInfo();
  di->SetTree(tree);

  gReve->DisableRedraw();
  Alieve::ITSModuleStepper* ms = new Alieve::ITSModuleStepper(di);
  ms->SetMainColor(Color_t(8));
  gStyle->SetPalette(1, 0);
  ms->ConfigStepper(col, row);
  ms->DisplayDet(det, -1);
  gReve->AddRenderElement(ms);
  gReve->EnableRedraw();

  TGLViewer* v = (TGLViewer *)gReve->GetGLViewer();
  v->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  TGLCameraMarkupStyle* mup = v->GetCameraMarkup();
  if(mup) mup->SetShow(kFALSE);

  gReve->Redraw3D(kTRUE);
}

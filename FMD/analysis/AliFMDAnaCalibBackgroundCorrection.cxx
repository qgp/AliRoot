
#include "AliFMDAnaCalibBackgroundCorrection.h"
#include <TH2F.h>
#include <TBrowser.h>

ClassImp(AliFMDAnaCalibBackgroundCorrection)
#if 0
; // For Emacs
#endif 

//____________________________________________________________________
AliFMDAnaCalibBackgroundCorrection::AliFMDAnaCalibBackgroundCorrection() : TObject(),
									   fArray(),
									   fAxis(),
									   fIsInit(kFALSE)
{
  
  
  
}


//____________________________________________________________________
AliFMDAnaCalibBackgroundCorrection::AliFMDAnaCalibBackgroundCorrection(const AliFMDAnaCalibBackgroundCorrection& o)
  : TObject(o), fArray(o.fArray), fAxis(o.fAxis), fIsInit(o.fIsInit)
{
  // Copy ctor 
}
//____________________________________________________________________
AliFMDAnaCalibBackgroundCorrection&
AliFMDAnaCalibBackgroundCorrection::operator=(const AliFMDAnaCalibBackgroundCorrection& o) 
{
  // Assignment operator 
  
  fArray     = o.fArray;
  
  return (*this);
}

//____________________________________________________________________
TH2F* AliFMDAnaCalibBackgroundCorrection::GetBgCorrection(Int_t det, 
							  Char_t ring, 
							  Int_t vtxbin) {
  TObjArray* ringArray = GetRingArray(det,ring);
  TH2F* hCorrection    = (TH2F*)ringArray->At(vtxbin);
  return hCorrection;
}

//____________________________________________________________________
void AliFMDAnaCalibBackgroundCorrection::SetBgCorrection(Int_t det, 
							 Char_t ring, 
							 Int_t vtxbin, 
							 TH2F* hCorrection) {
  if(!fIsInit)
    Init();
  
  TObjArray* ringArray = GetRingArray(det,ring);
  ringArray->AddAtAndExpand(hCorrection,vtxbin);
  
}
//____________________________________________________________________
void AliFMDAnaCalibBackgroundCorrection::SetRefAxis(TAxis* axis) {
  
 
  fAxis.Set(axis->GetNbins(),axis->GetXmin(),axis->GetXmax());
    
}
//____________________________________________________________________
void AliFMDAnaCalibBackgroundCorrection::Init() {
  
  fArray.SetOwner();
  for(Int_t det = 1; det<=3;det++) {
    TObjArray* detArray = new TObjArray();
    detArray->SetOwner();
    fArray.AddAtAndExpand(detArray,det);
    Int_t nRings = (det == 1 ? 1 : 2);
    for(Int_t ring = 0;ring<nRings;ring++) {
      TObjArray* ringArray = new TObjArray();
      ringArray->SetOwner();
      detArray->AddAtAndExpand(ringArray,ring);
      
    }
  }
  fIsInit = kTRUE;
  
}
//____________________________________________________________________
TObjArray* AliFMDAnaCalibBackgroundCorrection::GetRingArray(Int_t det, 
							    Char_t ring) {
  
  Int_t ringNumber      = (ring == 'I' ? 0 : 1);
  TObjArray* detArray  = (TObjArray*)fArray.At(det);
  TObjArray* ringArray = (TObjArray*)detArray->At(ringNumber);
  
  return ringArray;
}
//____________________________________________________________________
Int_t AliFMDAnaCalibBackgroundCorrection::GetNvtxBins() {
  
  return fAxis.GetNbins();
  
}
//____________________________________________________________________
Float_t AliFMDAnaCalibBackgroundCorrection::GetVtxCutZ() {
  
  return fAxis.GetXmax();
  
}

//____________________________________________________________________
void
AliFMDAnaCalibBackgroundCorrection::Browse(TBrowser* b)
{
  b->Add(&fAxis, "Vertex bins");
  b->Add(&fArray, "Array of histograms w/BG corrections");
}

//____________________________________________________________________
//
// EOF
//

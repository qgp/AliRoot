#ifndef ALIFMDDNDETA_H
#define ALIFMDDNDETA_H

#include "TObject.h"
#include "TList.h"
#include "TString.h"

class AliFMDDndeta : public TObject
{

 public:
  AliFMDDndeta();
  
  enum Analysis {kHits, kMult, kMultTrVtx};
  
  void Init(const Char_t* filename); 
  void GenerateMult(Analysis what);
  void DrawDndeta(Analysis what, Int_t rebin = 1);
  void SetNbinsToCut(Int_t nbins) {fNbinsToCut = nbins;}
 
 private:
  void GenerateHits();
  void SetNames(Analysis what);
  const char* GetAnalysisName(Analysis what, UShort_t det, Char_t ring, Int_t vtxbin);
  const char* GetPrimName(Analysis what, UShort_t det, Char_t ring, Int_t vtxbin);
  TList* fList;
  TList  fMultList;
  Int_t  fNbinsToCut;
  Int_t  fVtxCut;
  Bool_t fIsInit;
  Bool_t fIsGenerated[3];
  TString fPrimEvents;
  TString fEvents;
  TString fPrimdNdeta;
  
  ClassDef(AliFMDDndeta,2);
};


#endif
// Local Variables:
//  mode: C++
// End Variables;

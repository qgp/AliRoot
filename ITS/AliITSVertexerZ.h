#ifndef ALIITSVERTEXERZ_H
#define ALIITSVERTEXERZ_H

#include<AliITSVertexer.h>

///////////////////////////////////////////////////////////////////
//                                                               //
// Class for primary vertex finding                              //
//                                                               //
///////////////////////////////////////////////////////////////////

class TFile;
class TString;
class AliITSVertex;
class AliITS;
class TH1F;

class AliITSVertexerZ : public AliITSVertexer {

 public:

  AliITSVertexerZ();
  AliITSVertexerZ(TString filename,Float_t x0=0., Float_t y0=0.);
  virtual ~AliITSVertexerZ();
  virtual AliITSVertex* FindVertexForCurrentEvent(Int_t evnumb);
  virtual void FindVertices();
  virtual void PrintStatus() const;
  void SetDiffPhiMax(Float_t pm = 0.01){fDiffPhiMax = pm;}
  void SetFirstLayerModules(Int_t m1 = 0, Int_t m2 = 79){fFirstL1 = m1; fLastL1 = m2;}
  void SetSecondLayerModules(Int_t m1 = 80, Int_t m2 = 239){fFirstL2 = m1; fLastL2 = m2;}
  void SetLowLimit(Float_t lim=-10.){fLowLim = lim;}
  void SetHighLimit(Float_t lim=10.){fHighLim = lim;}
  Float_t GetLowLimit() const {return fLowLim;}
  Float_t GetHighLimit() const {return fHighLim;}
  void SetBinWidthCoarse(Float_t bw=0.01){fStepCoarse = bw;}
  void SetBinWidthFine(Float_t bw=0.0005){fStepFine = bw;}
  Float_t GetBinWidthCoarse() const {return fStepCoarse;}
  Float_t GetBinWidthFine() const {return fStepFine;}
  void SetTolerance(Float_t tol = 20./10000.){fTolerance = tol;}
  Float_t GetTolerance() const {return fTolerance;}

 protected:
  AliITSVertexerZ(const AliITSVertexerZ& vtxr);
  AliITSVertexerZ& operator=(const AliITSVertexerZ& /* vtxr */);
  void ResetHistograms();

  Int_t fFirstL1;          // first module of the first pixel layer used
  Int_t fLastL1;           // last module of the first pixel layer used
  Int_t fFirstL2;          // first module of the second pixel layer used
  Int_t fLastL2;           // last module of the second pixel layer used
  Float_t fDiffPhiMax;     // Maximum delta phi allowed among corr. pixels
  Float_t fX0;             // Nominal x coordinate of the vertex
  Float_t fY0;             // Nominal y coordinate of the vertex
  AliITS *fITS;            //! pointer to the AliITS object
  Float_t fZFound;         //! found value for the current event
  Float_t fZsig;           //! RMS of Z
  TH1F *fZCombc;           //! histogram with coarse z distribution
  TH1F *fZCombf;           //! histogram with fine z distribution
  Float_t fLowLim;         // low limit for fZComb histograms
  Float_t fHighLim;        // high limit for fZComb histograms
  Float_t fStepCoarse;     // bin width for fZCombc
  Float_t fStepFine;       // bin width for fZCombf
  Float_t fTolerance;      // tolerance on the symmetry of the Z interval 

  ClassDef(AliITSVertexerZ,1);
};

#endif

#ifndef ALICORRECTIONMATRIX2D_H
#define ALICORRECTIONMATRIX2D_H

/* $Id$ */

// ------------------------------------------------------
//
// Class to handle 2d-corrections.
//
// ------------------------------------------------------

#include <AliCorrectionMatrix.h>

class TH2F;
class TH1F;

class AliCorrectionMatrix2D : public AliCorrectionMatrix
{
public:
  AliCorrectionMatrix2D();
  AliCorrectionMatrix2D(const AliCorrectionMatrix2D& c);
  AliCorrectionMatrix2D(const Char_t* name, const Char_t* title,
		     Int_t nBinX=10, Float_t Xmin=0., Float_t Xmax=10.,
		     Int_t nBinY=10, Float_t Ymin=0., Float_t Ymax=10.);

  AliCorrectionMatrix2D(const Char_t* name, const Char_t* title,
		     Int_t nBinX, Float_t *X, Int_t nBinY, Float_t *Y);

  virtual ~AliCorrectionMatrix2D();

  TH2F* GetGeneratedHistogram() const;
  TH2F* GetMeasuredHistogram() const;

  TH1F* Get1DCorrection(Char_t* opt="x");

  void FillMeas(Float_t ax, Float_t ay);
  void FillGene(Float_t ax, Float_t ay);
  Float_t GetCorrection(Float_t ax, Float_t ay) const;

  void RemoveEdges(Float_t cut=2, Int_t nBinsX=0, Int_t nBinsY=0);

protected:
  ClassDef(AliCorrectionMatrix2D,1)
};

#endif


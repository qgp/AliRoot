#ifndef ALICORRECTIONMATRIX_H
#define ALICORRECTIONMATRIX_H

/* $Id$ */

// ------------------------------------------------------
//
// Class to handle corrections.
//
// ------------------------------------------------------
//
// TODO:
//
// - add options in draw method
//

#include <TNamed.h>

class TH1;

class AliCorrectionMatrix : public TNamed
{
protected: // do not create this baseclass
  AliCorrectionMatrix();
  AliCorrectionMatrix(const Char_t* name, const Char_t* title);
  AliCorrectionMatrix(const AliCorrectionMatrix& c);
  virtual ~AliCorrectionMatrix();

public:
  AliCorrectionMatrix& operator=(const AliCorrectionMatrix& corrMatrix);
  virtual void Copy(TObject& c) const;
  virtual Long64_t Merge(TCollection* list);

  TH1* GetGeneratedHistogram() { return fhGene; }
  TH1* GetMeasuredHistogram()  { return fhMeas; }

  void SetGeneratedHistogram(TH1* agene) { fhGene = agene; }
  void SetMeasuredHistogram(TH1* ameas)  { fhMeas = ameas; }

  void Divide();

  void SetAxisTitles(const Char_t* titleX="", const Char_t* titleY="", const Char_t* titleZ="");

  virtual Bool_t LoadHistograms(const Char_t* fileName, const Char_t* dir = ".");
  virtual void SaveHistograms();

  virtual void DrawHistograms();

  virtual void ReduceInformation();

protected:
  TH1*    fhMeas;  // histogram of measured particles (or tracks)
  TH1*    fhGene;  // histogram of generated particles

  TH1*    fhCorr;  // correction histogram (ratio generated/measured)

  ClassDef(AliCorrectionMatrix,1)
};

#endif


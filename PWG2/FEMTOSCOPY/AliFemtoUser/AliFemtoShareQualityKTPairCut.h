/***************************************************************************
 *
 * $Id$
 *
 * Author: Adam Kisiel, Ohio State University, kisiel@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: AliFemtoMaker package
 *   a cut to remove "shared" and "split" pairs
 *
 ***************************************************************************
 *
 *
 **************************************************************************/


#ifndef ALIFEMTOSHAREQUALITYKTPAIRCUT_H
#define ALIFEMTOSHAREQUALITYKTPAIRCUT_H

// do I need these lines ?
//#ifndef StMaker_H
//#include "StMaker.h"
//#endif

#include "AliFemtoPairCut.h"
#include "AliFemtoShareQualityPairCut.h"

class AliFemtoShareQualityKTPairCut : public AliFemtoShareQualityPairCut{
public:
  AliFemtoShareQualityKTPairCut();
  AliFemtoShareQualityKTPairCut(const AliFemtoShareQualityKTPairCut&);
  ~AliFemtoShareQualityKTPairCut();

  virtual bool Pass(const AliFemtoPair*);
  virtual AliFemtoString Report();
  virtual TList *ListSettings();
  AliFemtoShareQualityKTPairCut* Clone();
  void SetKTRange(double ktmin, double ktmax);
  
 protected:
  Double_t fKTMin;          // Minimum allowed pair transverse momentum
  Double_t fKTMax;          // Maximum allowed pair transverse momentum 

#ifdef __ROOT__
  ClassDef(AliFemtoShareQualityKTPairCut, 0)
#endif
};

inline AliFemtoShareQualityKTPairCut* AliFemtoShareQualityKTPairCut::Clone() { AliFemtoShareQualityKTPairCut* c = new AliFemtoShareQualityKTPairCut(*this); return c;}

#endif

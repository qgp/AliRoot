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


#ifndef ALIFEMTOKTPAIRCUT_H
#define ALIFEMTOKTPAIRCUT_H

// do I need these lines ?
//#ifndef StMaker_H
//#include "StMaker.h"
//#endif

#include "AliFemtoPairCut.h"

class AliFemtoKTPairCut : public AliFemtoPairCut{
public:
  AliFemtoKTPairCut();
  AliFemtoKTPairCut(const AliFemtoKTPairCut&);
  ~AliFemtoKTPairCut();

  virtual bool Pass(const AliFemtoPair*);
  virtual AliFemtoString Report();
  virtual TList *ListSettings();
  AliFemtoPairCut* Clone();
  void SetKTRange(double ktmin, double ktmax);
  
 protected:
  Double_t fKTMin;          // Minimum allowed pair transverse momentum
  Double_t fKTMax;          // Maximum allowed pair transverse momentum 

#ifdef __ROOT__
  ClassDef(AliFemtoKTPairCut, 0)
#endif
};

inline AliFemtoPairCut* AliFemtoKTPairCut::Clone() { AliFemtoKTPairCut* c = new AliFemtoKTPairCut(*this); return c;}

#endif

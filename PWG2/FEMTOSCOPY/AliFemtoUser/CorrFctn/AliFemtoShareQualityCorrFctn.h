/***************************************************************************
 *
 * $Id$
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: AliFemtoMaker package
 *   a simple Q-invariant correlation function           
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2007/03/07 10:14:49  mchojnacki
 * First version on CVS
 *
 * Revision 1.3  2000/01/25 17:34:45  laue
 * I. In order to run the stand alone version of the AliFemtoMaker the following
 * changes have been done:
 * a) all ClassDefs and ClassImps have been put into #ifdef __ROOT__ statements
 * b) unnecessary includes of StMaker.h have been removed
 * c) the subdirectory AliFemtoMaker/doc/Make has been created including everything
 * needed for the stand alone version
 *
 * II. To reduce the amount of compiler warning
 * a) some variables have been type casted
 * b) some destructors have been declared as virtual
 *
 * Revision 1.2  1999/07/06 22:33:20  lisa
 * Adjusted all to work in pro and new - dev itself is broken
 *
 * Revision 1.1.1.1  1999/06/29 16:02:57  lisa
 * Installation of AliFemtoMaker
 *
 **************************************************************************/

#ifndef AliFemtoShareQualityCorrFctn_hh
#define AliFemtoShareQualityCorrFctn_hh

#include "TH1D.h"
#include "TH2D.h"
#include "Base/AliFemtoCorrFctn.h"

class AliFemtoShareQualityCorrFctn : public AliFemtoCorrFctn {
public:
  AliFemtoShareQualityCorrFctn(char* title, const int& nbins, const float& QinvLo, const float& QinvHi);
  virtual ~AliFemtoShareQualityCorrFctn();

  virtual AliFemtoString Report();
  virtual void AddRealPair(const AliFemtoPair*);
  virtual void AddMixedPair(const AliFemtoPair*);

  virtual void Finish();

  void WriteHistos();
private:
  
  TH2D *fShareNumerator;
  TH2D *fShareDenominator;

  TH2D *fQualityNumerator;
  TH2D *fQualityDenominator;

#ifdef __ROOT__
  ClassDef(AliFemtoShareQualityCorrFctn, 1)
#endif
};


#endif


#ifndef ALICALORAWANALYZERLMS_H
#define ALICALORAWANALYZERLMS_H
/**************************************************************************
 * This file is property of and copyright by                              *
 * the Relativistic Heavy Ion Group (RHIG), Yale University, US, 2009     *
 *                                                                        *
 * Primary Author: Per Thomas Hille <p.t.hille@fys.uio.no>                *
 *                                                                        *
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to p.t.hille@fys.uio.no                             *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


// Extraction of amplitude and peak position
// FRom CALO raw data using
// Chi square fit

#include "AliCaloRawAnalyzer.h"


class  TF1;
class  TGraph;

class  AliCaloRawAnalyzerLMS : public AliCaloRawAnalyzer
{
 public:
  AliCaloRawAnalyzerLMS();
  virtual ~AliCaloRawAnalyzerLMS();
  virtual AliCaloFitResults  Evaluate( const vector<AliCaloBunchInfo> &bunchvector, const UInt_t altrocfg1,  const UInt_t altrocfg2 );
  void PrintFitResult(const TF1 *f) const;
  
 private:
  AliCaloRawAnalyzerLMS(const AliCaloRawAnalyzerLMS & );
  AliCaloRawAnalyzerLMS  & operator = (const AliCaloRawAnalyzerLMS  &);
 
  double fXaxis[MAXSAMPLES]; //Axis if time bins, ( used by TGraph )
  const double fkEulerSquared; //e^2 = 7.389056098930650227
  TGraph *fSig;  // Signale holding the data to be fitted.
  TF1 *fTf1;     // Analytical formula of the Semi Gaussian to be fitted

  ClassDef(AliCaloRawAnalyzerLMS, 1)

};

#endif

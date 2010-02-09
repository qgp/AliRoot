#ifndef ALICALORAWANALYZER_H
#define ALICALORAWANALYZER_H
/**************************************************************************
 * This file is property of and copyright by                              *
 * the Relatvistic Heavy Ion Group (RHIG), Yale University, US, 2009     *
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


//Base class for extraction 
//of signal amplitude and peak position
//From CALO Calorimeter RAW data


#include "Rtypes.h"
#include "TObject.h"
#include <vector>
using namespace std;

#define MAXSAMPLES 1008 //CRAP PTH
#include "AliCaloRawAnalyzer.h"

class AliCaloBunchInfo;
class AliCaloFitResults;


class  AliCaloRawAnalyzer : public TObject
{
 public:
  AliCaloRawAnalyzer();
  virtual ~AliCaloRawAnalyzer();
  virtual AliCaloFitResults Evaluate( const vector<AliCaloBunchInfo> &bunchvector, const UInt_t altrocfg1,  const UInt_t altrocfg2 );
 
  void PrintBunches( const vector<AliCaloBunchInfo> &bunchvector ) const;
  void PrintBunch( const AliCaloBunchInfo &bunch ) const ;

  virtual int PreFitEvaluateSamples( const vector<AliCaloBunchInfo>  &bunchvector, const UInt_t altrocfg1,  const UInt_t altrocfg2, Int_t & index, Float_t & maxf, short & maxamp, short & maxampindex, Float_t & ped, int & first, int & last);
  void SetTimeConstraint(const int min, const int max );
  void SetVerbose(bool verbose = true){ fVerbose = verbose; };
  void SetIsZeroSuppressed(const bool iszs = true) { fIsZerosupressed = iszs; } ;
  void SetAmpCut(const Float_t cut) { fAmpCut = cut ; } ;
  void SetFitArrayCut(const Int_t cut) { fFitArrayCut = cut ; } ;
  void SetNsampleCut(const Int_t cut) { fNsampleCut = cut ; } ;

  // access to array info
  Double_t GetReversed(const int i) const { return fReversed[i]; }

 protected:
  short Max( const AliCaloBunchInfo *const bunch, int *const maxindex) const;
  UShort_t Max(const UShort_t *data, const int length ) const;
  bool IsInTimeRange( const int maxindex ) const;
  Float_t  ReverseAndSubtractPed( const AliCaloBunchInfo *bunch, const UInt_t altrocfg1,  const UInt_t altrocfg2, double *outarray ) const;
  int  SelectBunch( const vector<AliCaloBunchInfo> &bunchvector, short *const maxampbin, short *const maxamplitude ) const;
  void SelectSubarray( const Double_t *fData, const int length, const short maxindex, int *const  first, int *const last ) const;
  Float_t EvaluatePedestal(const UShort_t * const data, const int length ) const;
  
  Double_t fReversed[MAXSAMPLES]; //Reversed sequence of samples (pedestalsubtracted)

  // private:
  int fMinTimeIndex; //The timebin of the max signal value must be between fMinTimeIndex and fMaxTimeIndex
  int fMaxTimeIndex; //The timebin of the max signal value must be between fMinTimeIndex and fMaxTimeIndex
  int fFitArrayCut;  //Cut on ADC value (after ped. subtraction) for signals used for fit
  Float_t fAmpCut;   //Max ADC - pedestal must be higher than this befor attemting to extract the amplitude 
  int fNsampleCut;   //Minimum number of sample require before attemting to extract signal parameters 
  bool fIsZerosupressed; //Wether or not the data is zeros supressed, by default its assumed that the baseline is also subtracted if set to true
  bool fVerbose;     //Print debug information to std out if set to true

};

#endif

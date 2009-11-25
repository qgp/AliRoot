//-*- Mode: C++ -*-
// $Id: AliHLTPHOSFourier.h 34951 2009-09-23 14:35:38Z phille $

#ifndef ALIHLTCALOFOURIER_H
#define ALIHLTCALOFOURIER_H

/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Dep. of Physics                                         *
 * University of Oslo, Norway, 2007                                       *
 *                                                                        *
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project.*
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to perthi@fys.uio.no                                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "Rtypes.h"
#include "TVirtualFFT.h"
#include "TMath.h"
#include "AliHLTCaloConstants.h"
#include <iostream>

#include "AliHLTCaloRcuFFTDataStruct.h"

//using namespace PhosHLTConst;
//using namespace CaloHLTConst;

using namespace std;

#define SAMPLING_FREQUENCY 10 

class  AliHLTCaloFourier
{
 public:
  AliHLTCaloFourier();
  virtual ~AliHLTCaloFourier();
  AliHLTCaloRcuFFTDataStruct GetPSD();
  void ProcessFourier(const Int_t *data, const int length, const int z, const int x, const int gain, const int event =-1);

  int GetDataSize() {return fFixedDataSize;};
private:
  void Init();
  void Int2Double(const Int_t *inputarray, double *outputarray, const int size);
  bool CheckSignal(const Int_t *data, const int length);

  double EvaluateMagnitude(const double re, const double im);
  
  //  double *fAccumulateFFTsAll[N_GAINS];
  // int fNFFTsAll[N_GAINS];
  // double *fAccumulateFFTs [N_ZROWS_MOD][N_XCOLUMNS_MOD][N_GAINS];
  // int fNFFTs[N_ZROWS_MOD][N_XCOLUMNS_MOD][N_GAINS];
 
  void ResetEventPSD(const int gain);
  TVirtualFFT *fFFT_own;

  double *fFFTInputArray;
  double *fFFTOutputArray;
  bool fIsFirstChannel;
  int  fFixedDataSize;
  
  // AliHLTCaloRcuFFTDataStruct fFFTOupuStruct[N_GAINS];
  AliHLTCaloRcuFFTDataStruct fFFTOupuStruct;

  int fCurrentEvent;

  template<typename T> 
    T  Max(T *array, int N) const
    {
      T tmpMax = 0;

      for(int i = 0; i < N; i++)
	{
	  if(array[i] > tmpMax)
	    {
	      tmpMax = array[i];
	    }
	}
  
      return tmpMax;
    }


  template<typename T> 
    T  Min(T *array, int N) const
    {
      
      T tmpMin = Max(array , N);
      
      //   T tmpMin = 100;

      for(int i = 0; i < N; i++)
	{
	  if(array[i] < tmpMin)
	    {
	      tmpMin = array[i];
	    }
	}
  
      return tmpMin;
    }

  
  AliHLTCaloFourier(const AliHLTCaloFourier&);
  AliHLTCaloFourier & operator=(const AliHLTCaloFourier&){return *this;};

};

#endif

// AliHLTCaloRcuAltroPatternTestComponent(const AliHLTCaloRcuAltroPatternTestComponent &);
//  AliHLTCaloRcuAltroPatternTestComponent & operator = (const AliHLTCaloRcuAltroPatternTestComponent &);

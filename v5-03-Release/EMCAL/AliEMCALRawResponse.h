// -*- mode: c++ -*-

#ifndef ALIEMCALRAWRESPONSE_H
#define ALIEMCALRAWRESPONSE_H

/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Yale University, US 2011                                *
 *                                                                        *
 * Author: Per Thomas Hille <perthomas.hille@yale.edu> for the ALICE      *
 * experiment. Contributors are mentioned in the code where appropriate.  *
 * Please report bugs to  perthomas.hille@yale.edu                        *
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

class  AliEMCALRawResponse
{
public:
  AliEMCALRawResponse();
  virtual ~AliEMCALRawResponse();
  static Double_t RawResponseFunction(Double_t *x, Double_t *par); 
  static Bool_t   RawSampledResponse(Double_t dtime, Double_t damp, Int_t * adcH, Int_t * adcL, 
			      const Int_t keyErr=0);  
  
  static Int_t    GetPedestalValue()     {return fgPedestalValue;}
  static Double_t GetFEENoise()          {return fgFEENoise;}
  static void SetFEENoise(Double_t val)                   {fgFEENoise = val;}
  static Double_t GetRawFormatTimeTrigger()  { return fgTimeTrigger ; }
  static Int_t GetRawFormatThreshold()       { return fgThreshold ; }   
  static void SetPedestalValue(Int_t val)                 {fgPedestalValue = val;}
  
private:
  static Double_t fgTimeTrigger ;       // time of the trigger for the RO signal 
  static Int_t fgThreshold;             // threshold
  static Int_t fgPedestalValue;         // pedestal value for Digits2Raw
  static Double_t fgFEENoise;           // electronics noise in ADC units
  
  ClassDef(AliEMCALRawResponse,1)
};

#endif

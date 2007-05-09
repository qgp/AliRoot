/**************************************************************************
 * Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Author: Per Thomas Hille for the ALICE HLT Project.                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


#include "AliHLTPHOSRawAnalyzerLMS.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(AliHLTPHOSRawAnalyzerLMS) 


AliHLTPHOSRawAnalyzerLMS::AliHLTPHOSRawAnalyzerLMS(const AliHLTPHOSRawAnalyzerLMS&):AliHLTPHOSRawAnalyzer()
{

}



AliHLTPHOSRawAnalyzerLMS::AliHLTPHOSRawAnalyzerLMS():AliHLTPHOSRawAnalyzer() 
{
  cout <<"You cannot invoke the Fitter without arguments"<<endl;;
}


/**
* Main constructor
* @param dataPtr Data array for wich a subarray will be taken to perform the fit
* @param fs the sampling frequency in entities of MHz. Needed in order to calculate physical time
**/
AliHLTPHOSRawAnalyzerLMS::AliHLTPHOSRawAnalyzerLMS(double *dtaPtr, double fs):AliHLTPHOSRawAnalyzer() 
{
  fFloatDataPtr = dtaPtr;  
  fSampleFrequency = fs;
} //end   AliHLTPHOSRawAnalyzerLMS 


AliHLTPHOSRawAnalyzerLMS::~AliHLTPHOSRawAnalyzerLMS()
{

} //end AliHLTPHOSRawAnalyzerLMS



void 
AliHLTPHOSRawAnalyzerLMS::Evaluate(int start, int length)
{

} //end FitLMS






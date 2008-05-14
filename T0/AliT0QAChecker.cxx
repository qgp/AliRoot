/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
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




//...
//  Checks the quality assurance. 
//  By comparing with reference data
//  Skeleton for T0
//...

// --- ROOT system ---
#include <Riostream.h>
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 
#include <TMath.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQA.h"
#include "AliQAChecker.h"
#include "AliT0QAChecker.h"

ClassImp(AliT0QAChecker)

//__________________________________________________________________
AliT0QAChecker& AliT0QAChecker::operator = (const AliT0QAChecker& qac )
{
  // Equal operator.
  this->~AliT0QAChecker();
  new(this) AliT0QAChecker(qac);
  return *this;
}
//__________________________________________________________________

const Double_t AliT0QAChecker::Check(AliQA::ALITASK_t index,TObjArray * list)
{

  // Super-basic check on the QA histograms on the input list:
  // look whether they are empty!

  cout<<" GetAliTaskName "<<AliQA::GetAliTaskName(index)<<" index "<<index<<endl;


  Double_t test = 0.0  ;
  Int_t count = 0 ;
  Double_t nent[100];
  memset(nent,0,100*sizeof(Double_t));
  Double_t w[100];
  memset(w,1.,100*sizeof(Double_t));
  TH1 *fhRecLEDAmp[24];  TH1 * fhRecQTC[24];
  TH1 *fhOnlineMean;  TH1 * fhRecMean;
  TString dataType = AliQA::GetAliTaskName(index);

  if (list->GetEntries() == 0){
    test = 1. ; // nothing to check
  }
  else {
    
    TIter next(list) ;
    TH1 * hdata ;
    
    count = 0 ;
    while ( (hdata = dynamic_cast<TH1 *>(next())) ) {
      if (hdata) {
	nent[count] = hdata->GetEntries();
       	AliDebug(10,Form("count %i %s -> %f",count, hdata->GetName(),nent[count])) ;

	if(index==2){
	  if(count>23 && count<48)fhRecLEDAmp[count-24] = hdata;
	  if(count>47 && count<72)fhRecQTC[count-48] = hdata;
	  if(count == 72) fhOnlineMean = hdata; 
	  if(count == 73) fhRecMean = hdata; 
	}
	count++ ;
        Double_t rv = 0.;
        if(hdata->GetEntries()>0) rv = 1;
	//   AliInfo(Form("%s -> %f", hdata->GetName(), rv)) ;
        test += rv ;
     }
      else{
        AliError("Data type cannot be processed") ;
      }

    }

    if (count != 0) {
      if (test==0) {
        AliWarning("Histograms are there, but they are all empty: setting flag to kWARNING");
        test = 0.5;  //upper limit value to set kWARNING flag for a task
      }
      else {

	if(index == 2){
	  if ( TMath::Abs(fhRecMean->GetMean() - fhOnlineMean->GetMean()) > 5) 
	    AliWarning(Form("rec mean %f -> online mean %f",fhRecMean->GetMean(), fhOnlineMean->GetMean())) ;
	  Double_t meanLED, meanQTC;
	  for (Int_t idet=0; idet<24; idet++) {
	    meanLED = fhRecLEDAmp[idet]->GetMean();
	    meanQTC = fhRecQTC[idet]->GetMean();
	    if (TMath::Abs(meanLED-meanQTC)> 1.) 
	    AliWarning(Form("Amplitude measurements are different in channel %i : Amp LED %f -> Amp QTC %f",idet,meanLED, meanQTC)) ;
	  }
	}	
	else
	  {
	    AliDebug(10,Form(" MaxElement %f ", TMath::MaxElement(count,nent)));	
	    if(TMath::MaxElement(count,nent) > 1000) {
	      Double_t mean = TMath::Mean(count,nent,w);
	      AliDebug(10,Form(" Mean %f ", mean));	
	      for (Int_t i=0; i<count; i++) 
		{
		  Double_t diff = TMath::Abs(nent[i]-mean);
		  if (diff > 0.1*mean )
		    AliWarning(Form("Problem in Number of entried in hist %i  is %f\n", i, nent[i])) ; 
	  }
	}
	  }
      }
    }
  } //  if (list->GetEntries() != 0
  AliInfo(Form("Test Result = %f", test)) ;
  return test ;
}

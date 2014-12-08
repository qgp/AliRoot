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


/* $Id$ */

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  Checks the quality assurance.                                  //
//  By analysis of the histograms & comparing with reference data  //
//  S.Arcelli                                                      //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TObjArray.h"

#include "AliLog.h"
//#include "AliQAv1.h"
//#include "AliQAChecker.h"
#include "AliTOFQAChecker.h"
#include <TPaveText.h>
#include <TList.h>

ClassImp(AliTOFQAChecker)

//____________________________________________________________________________
void AliTOFQAChecker::Check(Double_t * test, AliQAv1::ALITASK_t /*index*/,
				  TObjArray ** list,
				  const AliDetectorRecoParam * /*recoParam*/) 
{
  // Super-basic check on the QA histograms on the input list: 
  // look whether they are empty!

  Int_t count[AliRecoParam::kNSpecies] = { 0 }; 

  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    if (! AliQAv1::Instance(AliQAv1::GetDetIndex(GetName()))->IsEventSpecieSet(AliRecoParam::ConvertIndex(specie)) ) 
      continue ;
    test[specie] = 1.0 ; 
    if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) 
      continue ; 
    if (list[specie]->GetEntries() == 0){  
      test[specie] = 0.0 ; // nothing to check
    }
    else {
      TIter next(list[specie]) ; 
      TH1 * hdata ;
      count[specie] = 0 ; 
      while ( (hdata = static_cast<TH1 *>(next())) ) {
        if (hdata && hdata->InheritsFrom("TH1")) { 
          Double_t rv = 0.;

	  switch ( CheckRaws(hdata,specie) ) 
	    {
	    case AliQAv1::kINFO:
	      rv = 1.0;
	      break;
	    case AliQAv1::kWARNING:
	      rv = 0.75;
	      break;
	    case AliQAv1::kERROR:
	      rv = 0.25;
	      break;
	    case AliQAv1::kFATAL:
	      rv = -1.0;
	      break;
	    default:
	      //AliError("Invalid ecc value. FIXME !");
	      rv = AliQAv1::kNULLBit;
	      break;
	    }	  

          AliDebug(AliQAv1::GetQADebugLevel(), Form("%s -> %f", hdata->GetName(), rv)) ; 
          count[specie]++ ; 
          test[specie] += rv ; 
        }
        else{
          AliError("Data type cannot be processed") ;
        }
      }
      if (count[specie] != 0) { 
        if (test[specie]==0) {
          AliWarning("Histograms are there, but they are all empty: setting flag to kWARNING");
          test[specie] = 0.5;  //upper limit value to set kWARNING flag for a task
        }
        else {
	  test[specie] /= count[specie] ;
        }
        AliDebug(AliQAv1::GetQADebugLevel(), Form("Test Result = %f", test[specie])) ; 
      }
    }
  }
}  

//------------------------------------------------------
AliTOFQAChecker& AliTOFQAChecker::operator = (const AliTOFQAChecker& qac)
{

  if (this==&qac) return *this;
  return *this;

}

//____________________________________________________________________________
Int_t AliTOFQAChecker::CheckRaws(TH1* histo, Int_t specie)
{
  /*
  checker for RAWS
  */
  Int_t flag = AliQAv1::kNULLBit;
  
  if(histo->GetEntries()>0) flag = AliQAv1::kINFO; 
  
  Double_t binWidthTOFrawTime = 2.44;
  Float_t minTOFrawTime, maxTOFrawTime;
  if (AliRecoParam::ConvertIndex(specie) == AliRecoParam::kCosmic){
    minTOFrawTime=200.;//ns
    maxTOFrawTime=300.;//ns
  } else {
    minTOFrawTime=200.;//ns
    maxTOFrawTime=275.;//ns
  } 
  Float_t minTOFrawTot = 10.;
  Double_t maxTOFrawTot = 15.;
  // Double_t minTOFrawTot = 200;
  // Double_t maxTOFrawTot = 250;

  TString histname = histo->GetName();
  TPaveText text(0.65,0.5,0.9,0.75,"NDC");   
  
  if (histname.EndsWith("TOFRaws")) {
    if (histo->GetEntries()==0) {
      text.Clear();
      text.AddText("No entries. IF TOF IN RUN"); 
      text.AddText("Check the TOF TWiki"); 
      text.SetFillColor(kYellow);
      histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      flag = AliQAv1::kWARNING;
    } else {
      Float_t multiMean = histo->GetMean();
      Float_t lowMIntegral = histo->Integral(1,20);
      Float_t totIntegral = histo->Integral(2, histo->GetNbinsX());
      
      if (totIntegral==0){ //if only "0 hits per event" bin is filled -> error
	if (histo->GetBinContent(1)>0) {
	  text.Clear();
	  text.AddText("No TOF hits for all events."); 
	  text.AddText("Call TOF on-call."); 
	  text.SetFillColor(kRed);
	  histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
	  flag = AliQAv1::kERROR;
	}
      } else { 
	if (AliRecoParam::ConvertIndex(specie) == AliRecoParam::kCosmic) {
	  if (multiMean<10.){
	    text.Clear();
	    text.AddText(Form("Multiplicity within limits"));
	    text.AddText("for COSMICS: OK!!!");
	    text.SetFillColor(kGreen);
	    histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
	    flag = AliQAv1::kINFO;
	  } else {
	    text.Clear();
	    text.AddText(Form("Multiplicity too high"));
	    text.AddText("for COSMICS: email TOF on-call");
	    text.SetFillColor(kYellow);
	    histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
	    flag = AliQAv1::kWARNING;
	  }
	} else {
	  if ( (AliRecoParam::ConvertIndex(specie) == AliRecoParam::kLowMult)
	       &&((lowMIntegral>0.9*totIntegral) || (multiMean>100))){
	    text.Clear();
	    text.AddText(Form("Unexpected mean value = %5.2f",multiMean));
	    text.AddText("OK for COSMICS and technical.");
	    text.AddText("Check TOF TWiki for pp.");
	    text.SetFillColor(kYellow);
	    histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
	    flag = AliQAv1::kWARNING;
	  } else {
	    text.Clear();
	    text.AddText(Form("Multiplicity within limits"));
	    text.AddText("    OK!!!    ");
	    text.SetFillColor(kGreen);
	    histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
	    flag = AliQAv1::kINFO;
	  }
	}
      }
    }
  }
  if (histname.EndsWith("RawsTime")) {
    if (histo->GetEntries()==0) {
      //AliWarning("Raw time histogram is empty");
      text.Clear();
      text.AddText("No entries. If TOF in the run"); 
      text.AddText("check TOF TWiki"); 
      text.SetFillColor(kYellow);
      histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      flag = AliQAv1::kWARNING;
    } else {
      Float_t timeMean = histo->GetMean();
      Int_t lowBinId = TMath::Nint(200./binWidthTOFrawTime);
      Int_t highBinId = TMath::Nint(250./binWidthTOFrawTime);
      Float_t peakIntegral = histo->Integral(lowBinId,highBinId);
      Float_t totIntegral = histo->Integral(1, histo->GetNbinsX());      
      if ( (timeMean > minTOFrawTime) && (timeMean < maxTOFrawTime) ) {
	flag = AliQAv1::kINFO;
	text.Clear();
	text.AddText("Mean inside limits: OK!!!"); 
	text.SetFillColor(kGreen);
	histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      } else {
	if ( (peakIntegral/totIntegral > 0.1) && (peakIntegral/totIntegral < 0.75)) {
	  AliWarning(Form("Raw time: peak/total integral = %5.2f, mean = %5.2f ns -> Check filling scheme...",peakIntegral/totIntegral,timeMean));
	  text.Clear();
	  text.AddText("If multiple peaks,"); 
	  text.AddText("check filling scheme."); 
	  text.AddText("See TOF TWiki."); 
	  text.SetFillColor(kYellow);
	  histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	   
	  flag = AliQAv1::kWARNING; 
	} else {
	  AliWarning(Form("Raw time: peak/total integral = %5.2f, mean = %5.2f ns", peakIntegral/totIntegral,timeMean));
	  text.Clear();
	  text.AddText("Mean outside limits."); 
	  text.AddText("Call TOF on-call."); 
	  text.SetFillColor(kRed);
	  histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	 
	  flag = AliQAv1::kERROR;     
	} 	
      }
    }
  }

  if (histname.EndsWith("RawsToT")) {
    if (histo->GetEntries()==0) {
      text.Clear();
      text.AddText("No entries. Check TOF TWiki"); 
      text.SetFillColor(kYellow);
      histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      flag = AliQAv1::kWARNING;
    } else {
      Float_t timeMean = histo->GetMean();
      if ( (timeMean > minTOFrawTot) && (timeMean < maxTOFrawTot) ) {
	flag = AliQAv1::kINFO;
	text.Clear();
	text.AddText("Mean inside limits: OK!!!"); 
	text.SetFillColor(kGreen);
	histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      } else {
	flag = AliQAv1::kERROR;
	AliWarning(Form("ToT mean = %5.2f ns", timeMean));
	text.Clear();
	text.AddText("Mean outside limits."); 
	text.AddText("If NOT a technical run,"); 
	text.AddText("call TOF on-call."); 
	text.SetFillColor(kRed);
	histo->GetListOfFunctions()->Add((TPaveText*)text.Clone());	      
      }
    }
  }
  return flag;
}

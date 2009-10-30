/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id: $ */

/*
  Checks implemented a la AliMUONQAChecker.
  Checks the quality assurance by realzed checks on histogram content.
  P. Christiansen, Lund, September 2009.

  Based on AliPHOSQAChecker.
  Checks the quality assurance by comparing with reference data.
  P. Christiansen, Lund, January 2008.
*/

// --- ROOT header files ---
#include <TH1.h>

// --- AliRoot header files ---
#include "AliTPCQAChecker.h"
#include "AliTPCQADataMakerRec.h"

ClassImp(AliTPCQAChecker)

//__________________________________________________________________
Double_t *
AliTPCQAChecker::Check(AliQAv1::ALITASK_t index, TObjArray ** list, 
		       AliDetectorRecoParam * recoParam)
{
  /* It is important to understand the destinction between indexed tasks (AliQAv1::TASKINDEX_t) which are used in the DataMaker classes and indexed tasks (AliQAv1::ALITASK_t) whihc are used in the checker class.

     From the AliQAChecker::Run() methods we have:
     AliQAv1::kRAW
     - AliQAv1::kRAWS 
     
     AliQAv1::kSIM 
     - AliQAv1::kHITS
     - AliQAv1::kSDIGITS
     - AliQAv1::kDIGITS
     
     AliQAv1::kREC
     - AliQAv1::kDIGITSR 
     - AliQAv1::kRECPOINTS
     - AliQAv1::kTRACKSEGMENTS 
     - AliQAv1::kRECPARTICLES
     
     AliQAv1::kESD ; 
     -AliQAv1::kESDS

     This means that for each group of tasks the Check will be called
     one or more times.  This also mean that we cannot know what
     histograms will be or not be there in a single call... And we
     also do not know the position in the list of the histogram.
  */
  
  /// Check objects in list
  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::Check");
  
  if (index!=AliQAv1::kRAW&&index!=AliQAv1::kREC&&index!=AliQAv1::kESD) {
    
    AliWarning(Form("Checker for task %d not implement for the moment",index));
    return NULL;
  }
  
  Double_t * rv = new Double_t[AliRecoParam::kNSpecies] ; 
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) 
    rv[specie] = 1.0; // All is fine 
  
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    
    if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) 
      continue ; 
    
    if (index == AliQAv1::kRAW)
      rv[specie] = CheckRAW(specie, list[specie]);
    if (index == AliQAv1::kREC)
      rv[specie] = CheckREC(specie, list[specie]);
    if (index == AliQAv1::kESD)
      rv[specie] = CheckESD(specie, list[specie]);

    if(fDebug>3)
      AliInfo(Form("Specie: %s. Task: %s. Value: %f",  
		   AliRecoParam::GetEventSpecieName(specie),
		   AliQAv1::GetAliTaskName(index),
		   rv[specie]));
  }
  
  return rv;
}

//______________________________________________________________________________
Double_t AliTPCQAChecker::CheckRAW(Int_t specie, TObjArray* list)
{
  /// Check ESD
  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::CheckRAW");
  
  if(fDebug>2)
    list->Print();

  Char_t specieName[256];
  sprintf(specieName, AliRecoParam::GetEventSpecieName(specie));

  TH1* hRawsOccupancyVsSector = static_cast<TH1*>
    (list->FindObject(Form("%s_hRawsOccupancyVsSector",specieName)));
  TH1* hRawsQmaxVsSector = static_cast<TH1*>
    (list->FindObject(Form("%s_hRawsQmaxVsSector",specieName)));
  TH1* hRawsOccupancyVsEvent = static_cast<TH1*>
    (list->FindObject(Form("%s_hRawsOccupancyVsEvent",specieName)));
  TH1* hRawsNclustersVsEvent = static_cast<TH1*>
    (list->FindObject(Form("%s_hRawsNclustersVsEvent",specieName)));
  
  if (!hRawsOccupancyVsSector || !hRawsQmaxVsSector ||
      !hRawsOccupancyVsEvent  || !hRawsNclustersVsEvent) 
    return -0.5; // fatal
  
  if(hRawsOccupancyVsSector->GetEntries()==0) {
    return 0.25; // error - No TPC data!
  }
  
  Int_t nBinsX = hRawsOccupancyVsSector->GetNbinsX();
  for(Int_t i = 1; i <= nBinsX; i++) {
    
    if(hRawsOccupancyVsSector->GetBinContent(i)==0)
      return 0.75; // warning - no TPC data for at least one sector
  }
  
  return 1.0; // ok
}

//______________________________________________________________________________
Double_t AliTPCQAChecker::CheckREC(Int_t specie, TObjArray* list)
{
  // This method checks the QA histograms associated with reconstruction
  //
  // For TPC this is:
  // DigitsR : 
  // The digit histogram gives the ADC distribution for all sigbnals
  // above threshold. The check is just that there are digits.
  // RecPoints : 
  // The cluster histograms are meant to give an idea about the gain
  // from the cluster charge and to indicate iof there are rows with
  // noise clusters, i.e., they are very visual.
  //
  // The check methods are simple:
  // If there are no histogram at all (set fatal)
  // If digit histogram is there, but there are no digits (set error)
  // If cluster histogram is there but there are less than 1000 
  //    clusters (set warning)
  // If there are more than 1000 clusters but no clusters for either short, 
  // medium, or long pads (set error)
  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::CheckREC");
  
  if(fDebug>2)
    list->Print();

  Char_t specieName[256];
  sprintf(specieName, AliRecoParam::GetEventSpecieName(specie));

  TH1* hDigits = static_cast<TH1*>
    (list->FindObject(Form("%s_hDigitsADC",specieName)));
  TH1* hNclustersVsRow = static_cast<TH1*> 
    (list->FindObject(Form("%s_hRecPointsRow",specieName)));
  TH1* hQshort         = static_cast<TH1*> 
    (list->FindObject(Form("%s_hRecPointsQShort",specieName)));
  TH1* hQmedium        = static_cast<TH1*> 
    (list->FindObject(Form("%s_hRecPointsQMedium",specieName)));
  TH1* hQlong          = static_cast<TH1*> 
    (list->FindObject(Form("%s_hRecPointsQLong",specieName)));
  // The Qmax histograms are for now ignored

  if (!hDigits &&                                             // digits missing
      (!hNclustersVsRow || !hQshort || !hQmedium || !hQlong)) // 1 recpoint hist missing
    return -0.5; // fatal

  if (hDigits && hDigits->GetEntries()==0) 
    return 0.25; // error
  
  if (hNclustersVsRow && hNclustersVsRow->GetEntries() < 1000) {
    return 0.75; // warning
  } else { 
    if (!hQshort || !hQlong || !hQlong)
      return -0.5;// fatal - they should be there if the cluster vs row hist is there
    if (hQshort->GetEntries()==0 || hQmedium->GetEntries()==0 ||
	hQlong->GetEntries()==0) 
      return 0.25; // error
  }
  return 1; // ok
}

//______________________________________________________________________________
Double_t AliTPCQAChecker::CheckESD(Int_t specie, TObjArray* list)
{
  // This method checks the QA histograms associated with ESDs
  // (Note that there is aslo a globalQA which is running on all
  //  the ESD information so for now this is just a few basic
  //  histograms)
  //
  // The check methods are simple:
  // If there are no histogram at all (set fatal)
  // 
  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::CheckESD");

  if(fDebug>2)
    list->Print();

  Char_t specieName[256];
  sprintf(specieName, AliRecoParam::GetEventSpecieName(specie));

  TH1* hESDclusters = static_cast<TH1*>
    (list->FindObject(Form("%s_hESDclusters",specieName)));
  TH1* hESDratio = static_cast<TH1*>
    (list->FindObject(Form("%s_hESDratio",specieName)));
  TH1* hESDpt = static_cast<TH1*>
    (list->FindObject(Form("%s_hESDpt",specieName)));
  
  if (!hESDclusters || !hESDratio || !hESDpt) 
    return -0.5; // fatal

  return 1.0; // ok
}

//______________________________________________________________________________
void AliTPCQAChecker::Init(const AliQAv1::DETECTORINDEX_t det) 
{
  /// intialises QA and QA checker settings
  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::Init");
  AliQAv1::Instance(det) ; 
  Float_t hiValue[AliQAv1::kNBIT] ; 
  Float_t lowValue[AliQAv1::kNBIT] ;
  hiValue[AliQAv1::kINFO]       = 1.00; 
  lowValue[AliQAv1::kINFO]      = 0.99; 
  hiValue[AliQAv1::kWARNING]    = 0.99; 
  lowValue[AliQAv1::kWARNING]   = 0.50; 
  hiValue[AliQAv1::kERROR]      = 0.50; 
  lowValue[AliQAv1::kERROR]     = 0.00; 
  hiValue[AliQAv1::kFATAL]      = 0.00; 
  lowValue[AliQAv1::kFATAL]     =-1.00; 
  //  SetHiLo(&hiValue[0], &lowValue[0]) ; 
  SetHiLo(hiValue, lowValue) ; 
}

//______________________________________________________________________________
void 
AliTPCQAChecker::SetQA(AliQAv1::ALITASK_t index, Double_t * value) const
{
  /// sets the QA according the return value of the Check

  if(fDebug>0)
    AliInfo("In AliTPCQAChecker::SetQA");

  AliQAv1 * qa = AliQAv1::Instance(index);
  
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {

    if (value==NULL) { // No checker is implemented, set all QA to Fatal
      
      if(fDebug>1)
	AliInfo(Form("Fatal QA. Task: %s. Specie: %s", 
		     AliQAv1::GetAliTaskName(index), 
		     AliRecoParam::GetEventSpecieName(specie)));
      qa->Set(AliQAv1::kFATAL, specie) ; 
    } else {
      
      if ( value[specie] >= fLowTestValue[AliQAv1::kFATAL] && 
	   value[specie] < fUpTestValue[AliQAv1::kFATAL] ) {
	
	if(fDebug>1)
	  AliInfo(Form("QA-Fatal. Task: %s. Specie: %s", 
		       AliQAv1::GetAliTaskName(index), 
		       AliRecoParam::GetEventSpecieName(specie)));
        qa->Set(AliQAv1::kFATAL, specie) ; 
      } else if ( value[specie] > fLowTestValue[AliQAv1::kERROR] && 
		  value[specie] <= fUpTestValue[AliQAv1::kERROR]  ) {
	
	if(fDebug>1)
	  AliInfo(Form("QA-Error. Task: %s. Specie: %s", 
		       AliQAv1::GetAliTaskName(index), 
		       AliRecoParam::GetEventSpecieName(specie)));
        qa->Set(AliQAv1::kERROR, specie) ; 
      } else if (value[specie] > fLowTestValue[AliQAv1::kWARNING] && 
		 value[specie] <= fUpTestValue[AliQAv1::kWARNING]) {
	
	if(fDebug>1)
	  AliInfo(Form("QA-Warning. Task: %s. Specie: %s", 
		       AliQAv1::GetAliTaskName(index), 
		       AliRecoParam::GetEventSpecieName(specie)));
	qa->Set(AliQAv1::kWARNING, specie) ;
      } else if (value[specie] > fLowTestValue[AliQAv1::kINFO] && 
		 value[specie] <= fUpTestValue[AliQAv1::kINFO] ) { 
	
	if(fDebug>1)
	  AliInfo(Form("QA-Info. Task: %s. Specie: %s", 
		       AliQAv1::GetAliTaskName(index), 
		       AliRecoParam::GetEventSpecieName(specie)));
	qa->Set(AliQAv1::kINFO, specie) ; 	
      }
    }
  }
}

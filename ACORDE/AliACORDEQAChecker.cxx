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
//  Checks the quality assurance for ACORDE. 
//  Default implementation
//  Authors:
//	Mario Rodriguez Cahuantzi <mrodrigu@mail.cern.ch> (FCFM-BUAP) 
//	Luciano Diaz Gonzalez <luciano.diaz@nucleares.unam.mx> (ICN-UNAM)
//	Arturo Fernandez <afernan@mail.cern.ch> (FCFM-BUAP)
//  Last update: Nov. 14t 2009 --> MRC <mrodrigu@mail.cern.ch> (FCFM-BUAP) 
//...

// --- ROOT system ---
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQAv1.h"
#include "AliQAChecker.h"
#include "AliACORDEQAChecker.h"
#include "AliCDBEntry.h"
#include "AliQAManager.h"

ClassImp(AliACORDEQAChecker)

//____________________________________________________________________________
Double_t * AliACORDEQAChecker::Check(AliQAv1::ALITASK_t /*index*/)
{
  Double_t * rv = new Double_t[AliRecoParam::kNSpecies] ; 
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) 
    rv[specie] = 0.0 ; 
  return rv ;  
}
//____________________________________________________________________________
Double_t * AliACORDEQAChecker::Check(AliQAv1::ALITASK_t /*index*/, TObjArray ** list, AliDetectorRecoParam * /*recoParam*/)
{

// Close version to the final one for the ACORDE QA Checker

  Double_t * test = new Double_t[AliRecoParam::kNSpecies]  ; 
  Double_t * acoTest = new Double_t[AliRecoParam::kNSpecies] ;
  
// Loop over the run species (for specie!= cosmic by now we set QA to INFO) 
  
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) 
  {
	if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) continue ; 
	if (list[specie]->GetEntries() == 0) acoTest[specie] = 1.; // Nothing to check
	else 
	{
		TIter next(list[specie]) ; 
		TH1 * hdata ; // Data created by the AliACORDEQADataMakerXXX (Sim/Rec)
		while ( (hdata = dynamic_cast<TH1 *>(next())) ) 
		{
			if (hdata) 
			{ 
				Double_t rv = 0.0 ; 
				if(hdata->GetEntries()>0) rv=1; 
				AliDebug(AliQAv1::GetQADebugLevel(), Form("%s -> %f", hdata->GetName(), rv)) ; 
				TString hdataName = hdata->GetName();
				
				// Here we use the QAref ACORDE data from fRefOCDBSubDir
				
				if ( (fRefOCDBSubDir[specie]) && (hdataName.Contains("ACORDEBitPattern")) ) 
				{
					TH1 * href = NULL;
					if (fRefSubDir) href = static_cast<TH1*>(fRefSubDir->Get(hdata->GetName()));
					else if (fRefOCDBSubDir[specie]) href = static_cast<TH1*>(fRefOCDBSubDir[specie]->FindObject(hdata->GetName()));
					acoTest[specie] = CheckAcordeRefHits(href,hdata);
				}else if (hdataName.Contains("ACORDEBitPattern"))
				// Here we use an inner QA Checher without the QAref data
				{
					Float_t acoDataMax = hdata->GetMaximum();
					Int_t flagAcoQA = 0;
					if (acoDataMax!=0)
					{
						for(Int_t i=0;i<60;i++)
						{
							if ((hdata->GetBinContent(i)/acoDataMax) < 0.75) flagAcoQA++; 
						}
					}
					Double_t simpleFlag = 1.-flagAcoQA/60.;
					if ( (simpleFlag >= 0.90) && (simpleFlag <= 1.0) ) acoTest[specie] = 0.75; // INFO
					if ( (simpleFlag >= 0.70) && (simpleFlag < 0.90) ) acoTest[specie] = 0.50; // WARNING
					if ( (simpleFlag >= 0.25) && (simpleFlag < 0.70) ) acoTest[specie] = 0.25; // ERROR
					if ( (simpleFlag >= 0.0) && (simpleFlag < 0.25) )  acoTest[specie] = -1.0; // FATAL
				}	
				// Setting Warning message for possible Empty Events with the ACORDE-Trigger
					
				if (hdataName.Contains("ACORDEMultiplicity") && (hdata->GetBinContent(0)!=0)) AliWarning("Empty event registered with ACORDE Trigger !!!");
					
				
			}else AliError("Data type cannot be processed") ;
        			
		}
	}
	if ( (specie == AliRecoParam::kHighMult) || (specie == AliRecoParam::kLowMult) || (specie == AliRecoParam::kCalib) ) acoTest[specie] = 0.75;
	test[specie] = acoTest[specie]; // Assign of the acoTest to the test for final QAChecker value	
  }
  	return test ; 
}
//____________________________________________________________________________
Double_t AliACORDEQAChecker::CheckAcordeRefHits(const TH1 * href, const TH1 * hdata) const
{
	Double_t acoTest = 0.;
	Int_t flag=0;
	for (Int_t i=0;i<60;i++)
	{
		if (TMath::Abs(href->GetBinContent(i)-hdata->GetBinContent(i))) flag++;
	}
	if ((flag/60>50)&&(flag/60<=60)) acoTest = -1.;
	if ((flag/60>30)&&(flag/60<=50)) acoTest = 0.25;
	if ((flag/60>10)&&(flag/60<=30)) acoTest = 0.5;
	if ((flag/60>0)&&(flag/60<=10)) acoTest = 0.75;
	return acoTest;
}

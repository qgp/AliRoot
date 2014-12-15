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


/* $Id: AliADQADataMakerSim.cxx 23123 2007-12-18 09:08:18Z hristov $ */

//---
//  Produces the data needed to calculate the quality assurance. 
//  All data must be mergeable objects.
//  Author : BC
//---

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TDirectory.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliADdigit.h"
#include "AliADSDigit.h" 
#include "AliADhit.h"
#include "AliADQADataMakerSim.h"
#include "AliQAChecker.h"

ClassImp(AliADQADataMakerSim)
           
//____________________________________________________________________________ 
  AliADQADataMakerSim::AliADQADataMakerSim() : 
  AliQADataMakerSim(AliQAv1::GetDetName(AliQAv1::kAD), "AD Quality Assurance Data Maker")

{
  // constructor

  
}

//____________________________________________________________________________ 
AliADQADataMakerSim::AliADQADataMakerSim(const AliADQADataMakerSim& qadm) :
  AliQADataMakerSim() 
{
  //copy constructor 

  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliADQADataMakerSim& AliADQADataMakerSim::operator = (const AliADQADataMakerSim& qadm )
{
  // Assign operator.
  this->~AliADQADataMakerSim();
  new(this) AliADQADataMakerSim(qadm);
  return *this;
}
//____________________________________________________________________________
void AliADQADataMakerSim::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  ResetEventTrigClasses();
  AliQAChecker::Instance()->Run(AliQAv1::kAD, task, list) ;
}

 
//____________________________________________________________________________ 
void AliADQADataMakerSim::InitHits()
{
 
  // create Hits histograms in Hits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1I * h0 = new TH1I("hHitMultiplicity", "Hit multiplicity distribution in AD;# of Hits;Entries", 300, 0, 299) ; 
  h0->Sumw2() ;
  Add2HitsList(h0, 0, !expert, image) ;  
  
  TH1I * h1 = new TH1I("hHitCellNumber", "Hit cell distribution in AD;Cell;# of Hits", 16, 0, 16) ; 
  h1->Sumw2() ;
  Add2HitsList(h1, 1, !expert, image) ; 
  
  TH1I * h2 = new TH1I("hHitNPhotons", "Number of photons per hit in AD;# of Photons;Entries", 1000, 0, 50000) ; 
  h2->Sumw2() ;
  Add2HitsList(h2, 2, expert, image) ;  
 
  //
  ClonePerTrigClass(AliQAv1::kHITS); // this should be the last line    
}


//____________________________________________________________________________ 
void AliADQADataMakerSim::InitSDigits()
{
  // create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1I *fhSDigCharge[16]; 

  // create SDigits histograms in SDigits subdir
  TH1I * h0 = new TH1I("hSDigitMultiplicity", "SDigits multiplicity distribution in AD;# of Digits;Entries", 100, 0, 99) ; 
  h0->Sumw2() ;
  Add2DigitsList(h0, 0, !expert, image) ;
  
  for (Int_t i=0; i<16; i++)
    {
       fhSDigCharge[i] = new TH1I(Form("hSDigitCharge%d", i),Form("SDigit charges in cell %d; Time;Entries",i),1700,0.,1700);
       
       Add2SDigitsList(fhSDigCharge[i],i+1, !expert, image);
       
     }  
  //
  ClonePerTrigClass(AliQAv1::kSDIGITS); // this should be the last line
}



//____________________________________________________________________________ 
void AliADQADataMakerSim::InitDigits()
{
  // create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1I *fhDigTDC[16]; 
  TH1I *fhDigADC[16]; 

  // create Digits histograms in Digits subdir
  TH1I * h0 = new TH1I("hDigitMultiplicity", "Digits multiplicity distribution in AD;# of Digits;Entries", 100, 0, 99) ; 
  h0->Sumw2() ;
  Add2DigitsList(h0, 0, !expert, image) ;
  
  for (Int_t i=0; i<16; i++)
    {
       fhDigTDC[i] = new TH1I(Form("hDigitTDC%d", i),Form("Digit TDC in cell %d; TDC value;Entries",i),300,0.,149.);
       fhDigADC[i]= new TH1I(Form("hDigitADC%d", i),Form("Digit ADC in cell %d;ADC value;Entries",i),1024,0.,1023.);
       
       Add2DigitsList(fhDigTDC[i],i+1, !expert, image);
       Add2DigitsList(fhDigADC[i],i+1+16, !expert, image);  
     }  
  //
  ClonePerTrigClass(AliQAv1::kDIGITS); // this should be the last line
}


//____________________________________________________________________________
void AliADQADataMakerSim::MakeHits()
{
	//make QA data from Hits

  Int_t nhits = fHitsArray->GetEntriesFast();
  FillHitsData(0,nhits) ;    // fills Hit multiplicity
  for (Int_t ihit=0;ihit<nhits;ihit++) 
    {
	   AliADhit  * ADHit   = (AliADhit*) fHitsArray->UncheckedAt(ihit);
	   if (!ADHit) {
 	      AliError("The unchecked hit doesn't exist");
	      break;
	   }
	   FillHitsData(1,ADHit->GetCell());
	   FillHitsData(2,ADHit->GetNphot());
	}
}


//____________________________________________________________________________

void AliADQADataMakerSim::MakeHits(TTree *hitTree)
{
  //fills QA histos for Hits
 if (fHitsArray)
   fHitsArray->Clear() ; 
  else 
    fHitsArray = new TClonesArray("AliADhit", 1000);
  
  TBranch * branch = hitTree->GetBranch("AD") ;
  if ( ! branch ) {
    AliWarning("AD branch in Hit Tree not found") ;
  } else {

   if (branch) {
      branch->SetAddress(&fHitsArray);
    }else{
      AliError("Branch AD hit not found");
      exit(111);
    } 
    // Check id histograms already created for this Event Specie
    if ( ! GetHitsData(0) )
      InitHits() ;
    
    Int_t ntracks    = (Int_t) hitTree->GetEntries();
    
    if (ntracks<=0) return;
    // Start loop on tracks in the hits containers
    for (Int_t track=0; track<ntracks;track++) {
      branch->GetEntry(track);
      Int_t nhits = fHitsArray->GetEntriesFast();
      FillHitsData(0,nhits) ;    // fills Hit multiplicity
      for (Int_t ihit=0;ihit<nhits;ihit++) 
	{
	  AliADhit  * ADHit   = (AliADhit*) fHitsArray->UncheckedAt(ihit);
	  if (!ADHit) {
 	    AliError("The unchecked hit doesn't exist");
	    break;
	  }
	  FillHitsData(1,ADHit->GetCell());
	  FillHitsData(2,ADHit->GetNphot());	 
	}
    }
  }
  //
  IncEvCountCycleHits();
  IncEvCountTotalHits();
  //
}



//____________________________________________________________________________
void AliADQADataMakerSim::MakeSDigits(TTree *sdigitTree)
{
    // makes data from Digit Tree
	
  if (fSDigitsArray)
    fSDigitsArray->Clear() ; 
  else 
    fSDigitsArray = new TClonesArray("AliADSDigit", 1000) ; 

    TBranch * branch = sdigitTree->GetBranch("ADSDigit") ;
    if ( ! branch ) {
         AliWarning("AD branch in SDigit Tree not found") ; 
    } else {
         branch->SetAddress(&fSDigitsArray) ;
         branch->GetEntry(0) ; 
         MakeSDigits() ; 
    }  
    //
    IncEvCountCycleDigits();
    IncEvCountTotalDigits();
    //    
}

//____________________________________________________________________________
void AliADQADataMakerSim::MakeSDigits()
{
  // makes data from SDigits

  FillSDigitsData(0,fSDigitsArray->GetEntriesFast()) ; 
  TIter next(fSDigitsArray) ; 
    AliADSDigit *ADSDigit ; 
    while ( (ADSDigit = dynamic_cast<AliADSDigit *>(next())) ) {
         Int_t   PMNumber  = ADSDigit->PMNumber();       
         FillSDigitsData(PMNumber +1, ADSDigit->GetNBins()) ; 
	 
    }  
}

//____________________________________________________________________________
void AliADQADataMakerSim::MakeDigits()
{
  // makes data from Digits

  FillDigitsData(0,fDigitsArray->GetEntriesFast()) ; 
  TIter next(fDigitsArray) ; 
    AliADdigit *ADDigit ; 
    while ( (ADDigit = dynamic_cast<AliADdigit *>(next())) ) {
         Int_t   PMNumber  = ADDigit->PMNumber();    
         FillDigitsData(PMNumber +1, ADDigit->Time()) ;    // in 100 of picoseconds
	 FillDigitsData(PMNumber +1+16, ADDigit->ADC()) ;
    }  
}

//____________________________________________________________________________
void AliADQADataMakerSim::MakeDigits(TTree *digitTree)
{
    // makes data from Digit Tree
	
  if (fDigitsArray)
    fDigitsArray->Clear() ; 
  else 
    fDigitsArray = new TClonesArray("AliADdigit", 1000) ; 

    TBranch * branch = digitTree->GetBranch("ADDigit") ;
    if ( ! branch ) {
         AliWarning("AD branch in Digit Tree not found") ; 
    } else {
         branch->SetAddress(&fDigitsArray) ;
         branch->GetEntry(0) ; 
         MakeDigits() ; 
    }  
    //
    IncEvCountCycleDigits();
    IncEvCountTotalDigits();
    //    
}


//____________________________________________________________________________
void AliADQADataMakerSim::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle

}

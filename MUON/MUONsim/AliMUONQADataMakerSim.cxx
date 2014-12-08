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

// $Id$

#include "AliMUONQADataMakerSim.h"
#include "AliMUONHit.h"  
#include "AliMUONDigit.h"  
#include "AliMUONVHitStore.h"
#include "AliMUONVDigitStore.h"

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQAChecker.h"

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH1I.h> 
#include <TH2F.h> 
#include <TTree.h>

//-----------------------------------------------------------------------------
/// \class AliMUONQADataMakerSim
///
/// MUON base class for quality assurance data (histo) maker
///
/// \author C. Finck

/// \cond CLASSIMP
ClassImp(AliMUONQADataMakerSim)
/// \endcond
           
//____________________________________________________________________________ 
AliMUONQADataMakerSim::AliMUONQADataMakerSim() : 
    AliQADataMakerSim(AliQAv1::GetDetName(AliQAv1::kMUON), "MUON Quality Assurance Data Maker"),
    fHitStore(0x0),
    fDigitStore(0x0)   
{
  /// Default constructor

      AliDebug(AliQAv1::GetQADebugLevel(),"");
}

//____________________________________________________________________________ 
AliMUONQADataMakerSim::AliMUONQADataMakerSim(const AliMUONQADataMakerSim& qadm) :
    AliQADataMakerSim(),
  fHitStore(0x0),
  fDigitStore(0x0)
{
  /// Copy constructor

    AliDebug(AliQAv1::GetQADebugLevel(),"");

    if ( qadm.fHitStore ) 
    {
      fHitStore = static_cast<AliMUONVHitStore*>(qadm.fHitStore->Clone());
    }
    if ( qadm.fDigitStore ) 
    {
      fDigitStore = static_cast<AliMUONVDigitStore*>(qadm.fDigitStore->Clone());
    }
    SetName((const char*)qadm.GetName()) ; 
    SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliMUONQADataMakerSim& AliMUONQADataMakerSim::operator = (const AliMUONQADataMakerSim& qadm )
{
  /// Assignment operator

  AliDebug(AliQAv1::GetQADebugLevel(),"");

    this->~AliMUONQADataMakerSim();
    new(this) AliMUONQADataMakerSim(qadm);
    return *this;
}

//__________________________________________________________________
AliMUONQADataMakerSim::~AliMUONQADataMakerSim()
{
  /// Destructor

  AliDebug(AliQAv1::GetQADebugLevel(),"");

  delete fHitStore;
  delete fDigitStore;
}

//__________________________________________________________________
void AliMUONQADataMakerSim::InitHits() 
{
  /// Initialized hit spectra
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1F* h0 = new TH1F("hHitDetElem", "DetElemId distribution in Hits;Detection element Id;Counts", 1400, 100., 1500.); 
  Add2HitsList(h0, 0, !expert, image);

  TH1F* h1 = new TH1F("hHitPtot", "P distribution in Hits;P [erg];Counts ", 300, 0., 300.); 
  Add2HitsList(h1, 1, !expert, image);
  //
  ClonePerTrigClass(AliQAv1::kHITS); // this should be the last line
} 

//__________________________________________________________________
void AliMUONQADataMakerSim::InitSDigits() 
{
  /// Initialized SDigits spectra
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1I* h0 = new TH1I("hSDigitsDetElem", "Detection element distribution in SDigits;Detection element Id;Counts",  1400, 100, 1500); 
  Add2SDigitsList(h0, 0, !expert, image);

  TH1F* h1 = new TH1F("hSDigitsCharge", "Charge distribution in SDigits;Charge [??];Counts", 4096, 0, 4095); 
  Add2SDigitsList(h1, 1, !expert, image);
  //
  ClonePerTrigClass(AliQAv1::kSDIGITS); // this should be the last line
}  

//__________________________________________________________________
void AliMUONQADataMakerSim::InitDigits() 
{
  /// Initialized Digits spectra 
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1I* h0 = new TH1I("hDigitsDetElem", "Detection element distribution in Digits;Detection element Id;Counts",  1400, 100, 1500); 
  Add2DigitsList(h0, 0, !expert, image);

  TH1I* h1 = new TH1I("hDigitsADC", "ADC distribution in Digits;ACD value;Counts", 4096, 0, 4095); 
  Add2DigitsList(h1, 1, !expert, image);  
  //
  ClonePerTrigClass(AliQAv1::kDIGITS); // this should be the last line
} 

//__________________________________________________________________
void AliMUONQADataMakerSim::MakeHits(TTree* hitsTree)        
{
  /// makes data from Hits

  if (!fHitStore)
    fHitStore = AliMUONVHitStore::Create(*hitsTree);
  fHitStore->Connect(*hitsTree, false);
  hitsTree->GetEvent(0);
    
  TIter next(fHitStore->CreateIterator());

  AliMUONHit* hit = 0x0;

  while ( ( hit = static_cast<AliMUONHit*>(next()) ) )
  {
    FillHitsData(0,hit->DetElemId());
    FillHitsData(1,hit->Momentum());
  }
  //
  IncEvCountCycleHits();
  IncEvCountTotalHits();
  //
}

//__________________________________________________________________
void AliMUONQADataMakerSim::MakeSDigits(TTree* sdigitsTree)        
{
  /// makes data from SDigits

  if (!fDigitStore)
    fDigitStore = AliMUONVDigitStore::Create(*sdigitsTree);
  fDigitStore->Connect(*sdigitsTree, false);
  sdigitsTree->GetEvent(0);
    
  TIter next(fDigitStore->CreateIterator());

  AliMUONVDigit* dig = 0x0;

  while ( ( dig = static_cast<AliMUONVDigit*>(next()) ) )
  {
    FillSDigitsData(0,dig->DetElemId());
    FillSDigitsData(1,dig->Charge());
  }
  //
  IncEvCountCycleSDigits();
  IncEvCountTotalSDigits();
  //
} 

//__________________________________________________________________
void AliMUONQADataMakerSim::MakeDigits(TTree* digitsTree)         
{
   /// makes data from Digits
  
  if (!fDigitStore)
    fDigitStore = AliMUONVDigitStore::Create(*digitsTree);
  fDigitStore->Connect(*digitsTree, false);
  digitsTree->GetEvent(0);
    
  TIter next(fDigitStore->CreateIterator());

  AliMUONVDigit* dig = 0x0;

  while ( ( dig = static_cast<AliMUONVDigit*>(next()) ) )
  {
    FillDigitsData(0,dig->DetElemId());
    FillDigitsData(1,dig->ADC());
  }
  //
  IncEvCountCycleDigits();
  IncEvCountTotalDigits();
  //
}
      
//____________________________________________________________________________ 
void AliMUONQADataMakerSim::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray** list)
{
    ///Detector specific actions at end of cycle
    // do the QA checking
  ResetEventTrigClasses();
  AliQAChecker::Instance()->Run(AliQAv1::kMUON, task, list) ;  
}


//____________________________________________________________________________ 
void AliMUONQADataMakerSim::StartOfDetectorCycle()
{
    /// Detector specific actions at start of cycle
  
}

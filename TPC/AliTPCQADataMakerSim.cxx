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
  Based on AliPHOSQADataMaker
  Produces the data needed to calculate the quality assurance. 
  All data must be mergeable objects.
  P. Christiansen, Lund, January 2008
*/

/*
  Implementation:

  We have chosen to have the histograms as non-persistent meber to
  allow better debugging. In the copy constructor we then have to
  assign the pointers to the existing histograms in the copied
  list. This have been implemented but not tested.
*/

#include "AliTPCQADataMakerSim.h"

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---
#include "AliQAChecker.h"
#include "AliTPC.h"
#include "AliTPCv2.h"
#include "AliSimDigits.h"

ClassImp(AliTPCQADataMakerSim)

//____________________________________________________________________________ 
AliTPCQADataMakerSim::AliTPCQADataMakerSim() : 
  AliQADataMakerSim(AliQA::GetDetName(AliQA::kTPC), 
		    "TPC Sim Quality Assurance Data Maker")
{
  // ctor
}

//____________________________________________________________________________ 
AliTPCQADataMakerSim::AliTPCQADataMakerSim(const AliTPCQADataMakerSim& qadm) :
  AliQADataMakerSim()
{
  //copy ctor 
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
  
  //
  // Associate class histogram objects to the copies in the list
  // Could also be done with the indexes
  //
 }

//__________________________________________________________________
AliTPCQADataMakerSim& AliTPCQADataMakerSim::operator = (const AliTPCQADataMakerSim& qadm )
{
  // Equal operator.
  this->~AliTPCQADataMakerSim();
  new(this) AliTPCQADataMakerSim(qadm);
  return *this;
}
 
//____________________________________________________________________________ 
void AliTPCQADataMakerSim::EndOfDetectorCycle(AliQA::TASKINDEX_t task, TObjArray ** list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  AliQAChecker::Instance()->Run(AliQA::kTPC, task, list) ;  
}

//____________________________________________________________________________ 
void AliTPCQADataMakerSim::InitDigits()
{
  TH1F * histDigitsADC = 
    new TH1F("hDigitsADC", "Digit ADC distribution; ADC; Counts",
	     1000, 0, 1000);
  histDigitsADC->Sumw2();
  Add2DigitsList(histDigitsADC, kDigitsADC);
}

//____________________________________________________________________________ 
void AliTPCQADataMakerSim::InitHits()
{
  TH1F * histHitsNhits = 
    new TH1F("hHitsNhits", "Interactions per primary track in the TPC volume; Number of primary interactions; Counts",
	     100, 0, 10000);
  histHitsNhits->Sumw2();
  Add2HitsList(histHitsNhits, kNhits);

  TH1F * histHitsElectrons = 
    new TH1F("hHitsElectrons", "Electrons per interaction (primaries); Electrons; Counts",
	     300, 0, 300);
  histHitsElectrons->Sumw2();
  Add2HitsList(histHitsElectrons, kElectrons);  

  TH1F * histHitsRadius = 
    new TH1F("hHitsRadius", "Position of interaction (Primary tracks only); Radius; Counts",
	     300, 0., 300.);  
  histHitsRadius->Sumw2();
  Add2HitsList(histHitsRadius, kRadius);  

  TH1F * histHitsPrimPerCm = 
    new TH1F("hHitsPrimPerCm", "Primaries per cm (Primary tracks only); Primaries; Counts",
	     100, 0., 100.);  
  histHitsPrimPerCm->Sumw2();
  Add2HitsList(histHitsPrimPerCm, kPrimPerCm);  

  TH1F * histHitsElectronsPerCm = 
    new TH1F("hHitsElectronsPerCm", "Electrons per cm (Primary tracks only); Electrons; Counts",
	     300, 0., 300.);  
  histHitsElectronsPerCm->Sumw2();
  Add2HitsList(histHitsElectronsPerCm, kElectronsPerCm);  
}

//____________________________________________________________________________
void AliTPCQADataMakerSim::MakeDigits(TTree* digitTree)
{
  TBranch* branch = digitTree->GetBranch("Segment");
  AliSimDigits* digArray = 0;
  branch->SetAddress(&digArray);
  
  Int_t nEntries = Int_t(digitTree->GetEntries());
  
  for (Int_t n = 0; n < nEntries; n++) {
    
    digitTree->GetEvent(n);
    
    if (digArray->First())
      do {
        Float_t dig = digArray->CurrentDigit();
	
        GetDigitsData(kDigitsADC)->Fill(dig);
      } while (digArray->Next());    
  }
}

//____________________________________________________________________________
void AliTPCQADataMakerSim::MakeHits(TTree * hitTree)
{
  // make QA data from Hit Tree
  const Int_t nTracks = hitTree->GetEntries();
  TBranch* branch = hitTree->GetBranch("TPC2");
  AliTPCv2* tpc = (AliTPCv2*)gAlice->GetDetector("TPC");
  
  //
  // loop over tracks
  //
  for(Int_t n = 0; n < nTracks; n++){
    Int_t nHits = 0;
    branch->GetEntry(n);
    
    AliTPChit* tpcHit = (AliTPChit*)tpc->FirstHit(-1);  
    
    if (tpcHit) {
      Float_t dist  = 0.;
      Int_t   nprim = 0;
      Float_t xold  = tpcHit->X();
      Float_t yold  = tpcHit->Y();
      Float_t zold  = tpcHit->Z(); 
      Float_t radiusOld = TMath::Sqrt(xold*xold + yold*yold);
      Float_t q     = 0.;
      while(tpcHit) {
        Float_t x = tpcHit->X();
        Float_t y = tpcHit->Y();
        Float_t z = tpcHit->Z(); 
        Float_t radius = TMath::Sqrt(x*x + y*y);
	
        if(radius>50) { // Skip hits at interaction point
	  
          nHits++;
	  
          Int_t trackNo = tpcHit->GetTrack();
	  
          if(trackNo==n) { // primary track
	    
            GetDigitsData(kElectrons)->Fill(tpcHit->fQ);
            GetDigitsData(kRadius)->Fill(radius);
	    
            // find the new distance
            dist += TMath::Sqrt((x-xold)*(x-xold) + (y-yold)*(y-yold) + 
                                (z-zold)*(z-zold));
            if(dist<1.){  
              // add data to this 1 cm step
              nprim++;  
              q += tpcHit->fQ;
	      
            } else{
              // Fill the histograms normalized to per cm 
	      
              if(nprim==1)
                cout << radius << ", " << radiusOld << ", " << dist << endl; 
	      
              GetDigitsData(kPrimPerCm)->Fill((Float_t)nprim);
              GetDigitsData(kElectronsPerCm)->Fill(q);
	      
              dist  = 0;
              q     = 0;
              nprim = 0;
            }
          }
        }
	
        radiusOld = radius;
        xold = x;
        yold = y;
        zold = z;
	
        tpcHit = (AliTPChit*) tpc->NextHit();
        }
      }
      GetDigitsData(kNhits)->Fill(nHits);
    }
  }


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

//_________________________________________________________________________
// This is a TTask that constructs SDigits out of Hits
// A Summable Digits is the "sum" of all hits in a pad
// Detector response has been simulated via the method
// SimulateDetectorResponse
//
//-- Authors: F. Pierella, A. De Caro
// Use case: see AliTOFhits2sdigits.C macro in the CVS
//////////////////////////////////////////////////////////////////////////////


#include <Riostream.h>
#include <stdlib.h>

#include <TBenchmark.h>
#include <TF1.h>
#include <TFile.h>
#include <TFolder.h>
#include <TH1.h>
#include <TParticle.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTask.h>
#include <TTree.h>
#include <TVirtualMC.h>

#include "AliDetector.h"
#include "AliLoader.h"
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliTOF.h"
#include "AliTOFConstants.h"
#include "AliTOFHitMap.h"
#include "AliTOFSDigit.h"
#include "AliTOFSDigitizer.h"
#include "AliTOFhit.h"
#include "AliTOFhitT0.h"
#include "AliTOFv1.h"
#include "AliTOFv2.h"
#include "AliTOFv3.h"
#include "AliTOFv4.h"

ClassImp(AliTOFSDigitizer)

//____________________________________________________________________________ 
  AliTOFSDigitizer::AliTOFSDigitizer():TTask("AliTOFSDigitizer","") 
{
  // ctor

  fRunLoader     = 0 ;

  fEvent1=0;
  fEvent2=0;
  ftail    = 0;
  fSelectedSector=0;
  fSelectedPlate =0;
}
           
//____________________________________________________________________________ 
  AliTOFSDigitizer::AliTOFSDigitizer(char* HeaderFile, Int_t evNumber1, Int_t nEvents):TTask("AliTOFSDigitizer","") 
{
  fEvent1=evNumber1;
  fEvent2=fEvent1+nEvents;
  ftail    = 0;
  fSelectedSector=0; // by default we sdigitize all sectors
  fSelectedPlate =0; // by default we sdigitize all plates in all sectors

  fHeadersFile = HeaderFile ; // input filename (with hits)
  TFile * file = (TFile*) gROOT->GetFile(fHeadersFile.Data() ) ;

  //File was not opened yet
  // open file and get alirun object
  if(file == 0){
      file =    TFile::Open(fHeadersFile.Data(),"update") ;
      gAlice = (AliRun *) file->Get("gAlice") ;
  }

  // init parameters for sdigitization
  InitParameters();

  // add Task to //root/Tasks folder
  fRunLoader = AliRunLoader::Open(HeaderFile);//open session and mount on default event folder
  if (fRunLoader == 0x0)
   {
     Fatal("AliTOFSDigitizer","Event is not loaded. Exiting");
     return;
   }
  AliLoader* gime = fRunLoader->GetLoader("TOFLoader");
  if (gime == 0x0)
   {
     Fatal("AliTOFSDigitizer","Can not find TOF loader in event. Exiting.");
     return;
   }
  gime->PostSDigitizer(this);
}

//____________________________________________________________________________ 
  AliTOFSDigitizer::~AliTOFSDigitizer()
{
  // dtor
}

//____________________________________________________________________________ 
void AliTOFSDigitizer::InitParameters()
{
  // set parameters for detector simulation

  fTimeResolution =0.120;
  fpadefficiency  =0.99 ;
  fEdgeEffect     = 2   ;
  fEdgeTails      = 0   ;
  fHparameter     = 0.4 ;
  fH2parameter    = 0.15;
  fKparameter     = 0.5 ;
  fK2parameter    = 0.35;
  fEffCenter      = fpadefficiency;
  fEffBoundary    = 0.65;
  fEff2Boundary   = 0.90;
  fEff3Boundary   = 0.08;
  fResCenter      = 50. ;
  fResBoundary    = 70. ;
  fResSlope       = 40. ;
  fTimeWalkCenter = 0.  ;
  fTimeWalkBoundary=0.  ;
  fTimeWalkSlope  = 0.  ;
  fTimeDelayFlag  = 1   ;
  fPulseHeightSlope=2.0 ;
  fTimeDelaySlope =0.060;
  // was fMinimumCharge = TMath::Exp(fPulseHeightSlope*fKparameter/2.);
  fMinimumCharge = TMath::Exp(-fPulseHeightSlope*fHparameter);
  fChargeSmearing=0.0   ;
  fLogChargeSmearing=0.13;
  fTimeSmearing   =0.022;
  fAverageTimeFlag=0    ;
  fTdcBin   = 50.;      // 1 TDC bin = 50 ps
  fAdcBin   = 0.25;     // 1 ADC bin = 0.25 pC (or 0.03 pC)
  fAdcMean  = 50.;     // ADC distribution mpv value for Landau (in bins)
                       // it corresponds to a mean value of ~100 bins
  fAdcRms   = 25.;     // ADC distribution rms value (in bins)
                       // it corresponds to distribution rms ~50 bins
}

//__________________________________________________________________
Double_t TimeWithTail(Double_t* x, Double_t* par)
{
  // sigma - par[0], alpha - par[1], part - par[2]
  //  at x<part*sigma - gauss
  //  at x>part*sigma - TMath::Exp(-x/alpha)
  Float_t xx =x[0];
  Double_t f;
  if(xx<par[0]*par[2]) {
    f = TMath::Exp(-xx*xx/(2*par[0]*par[0]));
  } else {
    f = TMath::Exp(-(xx-par[0]*par[2])/par[1]-0.5*par[2]*par[2]);
  }
  return f;
}


//____________________________________________________________________________
void AliTOFSDigitizer::Exec(Option_t *verboseOption, Option_t *allEvents) { 

  fRunLoader->LoadgAlice();
  fRunLoader->LoadHeader();
  gAlice = fRunLoader->GetAliRun();
  
  AliLoader* gime = fRunLoader->GetLoader("TOFLoader");
  gime->LoadHits("read");
  gime->LoadSDigits("recreate");
  if(strstr(verboseOption,"tim") || strstr(verboseOption,"all"))
    gBenchmark->Start("TOFSDigitizer");

  AliTOF *TOF = (AliTOF *) gAlice->GetDetector("TOF");

  if (!TOF) {
    Error("AliTOFSDigitizer","TOF not found");
    return;
  }

  // is pointer to fSDigits non zero after changes?
  cout<<"TOF fSDigits pointer:"<<TOF->SDigits()<<endl;

  // recreate TClonesArray fSDigits - for backward compatibility
  if (TOF->SDigits() == 0) {
    TOF->CreateSDigitsArray();
  } else {
    TOF->RecreateSDigitsArray();
  }

  Int_t version=TOF->IsVersion();

  if (fEdgeTails) ftail = new TF1("tail",TimeWithTail,-2,2,3);

  Int_t nselectedHits=0;
  Int_t ntotalsdigits=0;
  Int_t ntotalupdates=0;
  Int_t nnoisesdigits=0;
  Int_t nsignalsdigits=0;
  Int_t nHitsFromPrim=0;
  Int_t nHitsFromSec=0;
  Int_t nlargeTofDiff=0;

  if (strstr(allEvents,"all")){
    fEvent1=0;
    fEvent2= (Int_t) gAlice->TreeE()->GetEntries();
  }

  Bool_t thereIsNotASelection=(fSelectedSector==0) && (fSelectedPlate==0);

  for (Int_t ievent = fEvent1; ievent < fEvent2; ievent++) {
    cout << "------------------- "<< GetName() << " -------------" << endl ;
    cout << "Sdigitizing event " << ievent << endl;

    Int_t nselectedHitsinEv=0;
    Int_t ntotalsdigitsinEv=0;
    Int_t ntotalupdatesinEv=0;
    Int_t nnoisesdigitsinEv=0;
    Int_t nsignalsdigitsinEv=0;

    fRunLoader->GetEvent(ievent);
    TOF->SetTreeAddress();
    TTree *TH = gime->TreeH ();
    if (!TH)
      return;
    if (gime->TreeS () == 0)
      gime->MakeTree ("S");

      
    //Make branches
    char branchname[20];
    sprintf (branchname, "%s", TOF->GetName ());
    //Make branch for digits
    TOF->MakeBranch("S");
    
    //Now made SDigits from hits


    TParticle *particle;
    //AliTOFhit *tofHit;
    TClonesArray *TOFhits = TOF->Hits();

    // create hit map
    AliTOFHitMap *hitMap = new AliTOFHitMap(TOF->SDigits());

    // increase performances in terms of CPU time
    TH->SetBranchStatus("*",0); // switch off all branches
    TH->SetBranchStatus("TOF*",1); // switch on only TOF

    Int_t ntracks = static_cast<Int_t>(TH->GetEntries());
    for (Int_t track = 0; track < ntracks; track++)
    {
      gAlice->ResetHits();
      TH->GetEvent(track);
      particle = gAlice->Particle(track);
      Int_t nhits = TOFhits->GetEntriesFast();
      // cleaning all hits of the same track in the same pad volume
      // it is a rare event, however it happens

      Int_t previousTrack =0;
      Int_t previousSector=0;
      Int_t previousPlate =0;
      Int_t previousStrip =0;
      Int_t previousPadX  =0;
      Int_t previousPadZ  =0;

      for (Int_t hit = 0; hit < nhits; hit++)
      {
	Int_t    vol[5];       // location for a digit
	Float_t  digit[2];     // TOF digit variables
	Int_t tracknum;
	Float_t Xpad;
	Float_t Zpad;
	Float_t geantTime;

	// fp: really sorry for this, it is a temporary trick to have
	// track length too
	if(version!=6){
	  AliTOFhit *tofHit = (AliTOFhit *) TOFhits->UncheckedAt(hit);
	  tracknum = tofHit->GetTrack();
	  vol[0] = tofHit->GetSector();
	  vol[1] = tofHit->GetPlate();
	  vol[2] = tofHit->GetStrip();
	  vol[3] = tofHit->GetPadx();
	  vol[4] = tofHit->GetPadz();
	  Xpad = tofHit->GetDx();
	  Zpad = tofHit->GetDz();
	  geantTime = tofHit->GetTof(); // unit [s]
	} else {
	  AliTOFhitT0 *tofHit = (AliTOFhitT0 *) TOFhits->UncheckedAt(hit);
	  tracknum = tofHit->GetTrack();
	  vol[0] = tofHit->GetSector();
	  vol[1] = tofHit->GetPlate();
	  vol[2] = tofHit->GetStrip();
	  vol[3] = tofHit->GetPadx();
	  vol[4] = tofHit->GetPadz();
	  Xpad = tofHit->GetDx();
	  Zpad = tofHit->GetDz();
	  geantTime = tofHit->GetTof(); // unit [s]
	}

	geantTime *= 1.e+09;  // conversion from [s] to [ns]
	    
	// selection case for sdigitizing only hits in a given plate of a given sector
	if(thereIsNotASelection || (vol[0]==fSelectedSector && vol[1]==fSelectedPlate)){
	  
	  Bool_t dummy=((tracknum==previousTrack) && (vol[0]==previousSector) && (vol[1]==previousPlate) && (vol[2]==previousStrip));
	  
	  Bool_t isCloneOfThePrevious=dummy && ((vol[3]==previousPadX) && (vol[4]==previousPadZ));
	  
	  Bool_t isNeighOfThePrevious=dummy && ((((vol[3]==previousPadX-1) || (vol[3]==previousPadX+1)) && (vol[4]==previousPadZ)) || ((vol[3]==previousPadX) && ((vol[4]==previousPadZ+1) || (vol[4]==previousPadZ-1))));
	  
	  if(!isCloneOfThePrevious && !isNeighOfThePrevious){
	    // update "previous" values
	    // in fact, we are yet in the future, so the present is past
	    previousTrack=tracknum;
	    previousSector=vol[0];
	    previousPlate=vol[1];
	    previousStrip=vol[2];
	    previousPadX=vol[3];
	    previousPadZ=vol[4];
	    
	    nselectedHits++;
	    nselectedHitsinEv++;
	    if (particle->GetFirstMother() < 0){
	      nHitsFromPrim++;
	    } // counts hits due to primary particles
	    
	    Float_t xStrip=AliTOFConstants::fgkXPad*(vol[3]-0.5-0.5*AliTOFConstants::fgkNpadX)+Xpad;
	    Float_t zStrip=AliTOFConstants::fgkZPad*(vol[4]-0.5-0.5*AliTOFConstants::fgkNpadZ)+Zpad;

	    //cout << "geantTime " << geantTime << " [ns]" << endl;
	    Int_t nActivatedPads = 0, nFiredPads = 0;
	    Bool_t isFired[4] = {kFALSE, kFALSE, kFALSE, kFALSE};
	    Float_t tofAfterSimul[4] = {0., 0., 0., 0.};
	    Float_t qInduced[4] = {0.,0.,0.,0.};
	    Int_t nPlace[4] = {0, 0, 0, 0};
	    Float_t averageTime = 0.;
	    SimulateDetectorResponse(zStrip,xStrip,geantTime,nActivatedPads,nFiredPads,isFired,nPlace,qInduced,tofAfterSimul,averageTime);
	    if(nFiredPads) {
	      for(Int_t indexOfPad=0; indexOfPad<nActivatedPads; indexOfPad++) {
		if(isFired[indexOfPad]){ // the pad has fired
		  Float_t timediff=geantTime-tofAfterSimul[indexOfPad];
		  
		  if(timediff>=0.2) nlargeTofDiff++;
		  
		  digit[0] = (Int_t) ((tofAfterSimul[indexOfPad]*1.e+03)/fTdcBin); // TDC bin number (each bin -> 50. ps)
		  
		  Float_t landauFactor = gRandom->Landau(fAdcMean, fAdcRms); 
		  digit[1] = (Int_t) (qInduced[indexOfPad] * landauFactor); // ADC bins (each bin -> 0.25 (or 0.03) pC)
		  
		  // recalculate the volume only for neighbouring pads
		  if(indexOfPad){
		    (nPlace[indexOfPad]<=AliTOFConstants::fgkNpadX) ? vol[4] = 1 : vol[4] = 2;
		    (nPlace[indexOfPad]<=AliTOFConstants::fgkNpadX) ? vol[3] = nPlace[indexOfPad] : vol[3] = nPlace[indexOfPad] - AliTOFConstants::fgkNpadX;
		  }
		  
		  // check if two sdigit are on the same pad; in that case we sum
		  // the two or more sdigits
		  if (hitMap->TestHit(vol) != kEmpty) {
		    AliTOFSDigit *sdig = static_cast<AliTOFSDigit*>(hitMap->GetHit(vol));
		    Int_t tdctime = (Int_t) digit[0];
		    Int_t adccharge = (Int_t) digit[1];
		    sdig->Update(fTdcBin,tdctime,adccharge,tracknum);
		    ntotalupdatesinEv++;
		    ntotalupdates++;
		  } else {
		    
		    TOF->AddSDigit(tracknum, vol, digit);
		    
		    if(indexOfPad){
		      nnoisesdigits++;
		      nnoisesdigitsinEv++;
		    } else {
		      nsignalsdigits++;
		      nsignalsdigitsinEv++;
		    }
		    ntotalsdigitsinEv++;  
		    ntotalsdigits++;
		    hitMap->SetHit(vol);
		  } // if (hitMap->TestHit(vol) != kEmpty)
		} // if(isFired[indexOfPad])
	      } // end loop on nActivatedPads
	    } // if(nFiredPads) i.e. if some pads has fired
	  } // close if(!isCloneOfThePrevious)
	} // close the selection on sector and plate
      } // end loop on hits for the current track
    } // end loop on ntracks
    
    delete hitMap;
      
    gime->TreeS()->Reset();
    gime->TreeS()->Fill();
    //gAlice->TreeS()->Write(0,TObject::kOverwrite) ;
    gime->WriteSDigits("OVERWRITE");

    if(strstr(verboseOption,"all")){
      cout << "----------------------------------------" << endl;
      cout << "       <AliTOFSDigitizer>     " << endl;
      cout << "After sdigitizing " << nselectedHitsinEv << " hits" << " in event " << ievent << endl;
      //" (" << nHitsFromPrim << " from primaries and " << nHitsFromSec << " from secondaries) TOF hits, " 
      cout << ntotalsdigitsinEv << " digits have been created " << endl;
      cout << "(" << nsignalsdigitsinEv << " due to signals and " <<  nnoisesdigitsinEv << " due to border effect)" << endl;
      cout << ntotalupdatesinEv << " total updates of the hit map have been performed in current event" << endl;
      cout << "----------------------------------------" << endl;
    }

  } //event loop on events

  // free used memory
  if (ftail){
    delete ftail;
    ftail = 0;
  }
  
  nHitsFromSec=nselectedHits-nHitsFromPrim;
  if(strstr(verboseOption,"all")){
    cout << "----------------------------------------" << endl;
    cout << "----------------------------------------" << endl;
    cout << "-----------SDigitization Summary--------" << endl;
    cout << "       <AliTOFSDigitizer>     " << endl;
    cout << "After sdigitizing " << nselectedHits << " hits" << endl;
    cout << "in " << (fEvent2-fEvent1) << " events" << endl;
//" (" << nHitsFromPrim << " from primaries and " << nHitsFromSec << " from secondaries) TOF hits, " 
    cout << ntotalsdigits << " sdigits have been created " << endl;
    cout << "(" << nsignalsdigits << " due to signals and " <<  nnoisesdigits << " due to border effect)" << endl;
    cout << ntotalupdates << " total updates of the hit map have been performed" << endl;
    cout << "in " << nlargeTofDiff << " cases the time of flight difference is greater than 200 ps" << endl;
  }


  if(strstr(verboseOption,"tim") || strstr(verboseOption,"all")){
    gBenchmark->Stop("TOFSDigitizer");
    cout << "AliTOFSDigitizer:" << endl ;
    cout << "   took " << gBenchmark->GetCpuTime("TOFSDigitizer") << " seconds in order to make sdigits " 
	 <<  gBenchmark->GetCpuTime("TOFSDigitizer")/(fEvent2-fEvent1) << " seconds per event " << endl ;
    cout << endl ;
  }

  Print("");
}
//__________________________________________________________________
void AliTOFSDigitizer::Print(Option_t* opt)const
{
  cout << "------------------- "<< GetName() << " -------------" << endl ;

}

//__________________________________________________________________
void AliTOFSDigitizer::SelectSectorAndPlate(Int_t sector, Int_t plate)
{
  Bool_t isaWrongSelection=(sector < 1) || (sector > AliTOFConstants::fgkNSectors) || (plate < 1) || (plate > AliTOFConstants::fgkNPlates);
  if(isaWrongSelection){
    cout << "You have selected an invalid value for sector or plate " << endl;
    cout << "The correct range for sector is [1,"<< AliTOFConstants::fgkNSectors <<"]" << endl;
    cout << "The correct range for plate  is [1,"<< AliTOFConstants::fgkNPlates  <<"]" << endl;
    cout << "By default we continue sdigitizing all hits in all plates of all sectors" << endl;
  } else {
    fSelectedSector=sector;
    fSelectedPlate =plate;
    cout << "SDigitizing only hits in plate " << fSelectedPlate << " of the sector " << fSelectedSector << endl;
  }
}

//__________________________________________________________________
void AliTOFSDigitizer::SimulateDetectorResponse(Float_t z0, Float_t x0, Float_t geantTime, Int_t& nActivatedPads, Int_t& nFiredPads, Bool_t* isFired, Int_t* nPlace, Float_t* qInduced, Float_t* tofTime, Float_t& averageTime)
{
  // Description:
  // Input:  z0, x0 - hit position in the strip system (0,0 - center of the strip), cm
  //         geantTime - time generated by Geant, ns
  // Output: nActivatedPads - the number of pads activated by the hit (1 || 2 || 4)
  //         nFiredPads - the number of pads fired (really activated) by the hit (nFiredPads <= nActivatedPads)
  //         qInduced[iPad]- charge induced on pad, arb. units
  //                         this array is initialized at zero by the caller
  //         tofAfterSimul[iPad] - time calculated with edge effect algorithm, ns
  //                                   this array is initialized at zero by the caller
  //         averageTime - time given by pad hited by the Geant track taking into account the times (weighted) given by the pads fired for edge effect also.
  //                       The weight is given by the qInduced[iPad]/qCenterPad
  //                                   this variable is initialized at zero by the caller
  //         nPlace[iPad] - the number of the pad place, iPad = 0, 1, 2, 3
  //                                   this variable is initialized at zero by the caller
  //
  // Description of used variables:
  //         eff[iPad] - efficiency of the pad
  //         res[iPad] - resolution of the pad, ns
  //         timeWalk[iPad] - time walk of the pad, ns
  //         timeDelay[iPad] - time delay for neighbouring pad to hited pad, ns
  //         PadId[iPad] - Pad Identifier
  //                    E | F    -->   PadId[iPad] = 5 | 6
  //                    A | B    -->   PadId[iPad] = 1 | 2
  //                    C | D    -->   PadId[iPad] = 3 | 4
  //         nTail[iPad] - the tail number, = 1 for tailA, = 2 for tailB
  //         qCenterPad - charge extimated for each pad, arb. units
  //         weightsSum - sum of weights extimated for each pad fired, arb. units
  
  const Float_t kSigmaForTail[2] = {AliTOFConstants::fgkSigmaForTail1,AliTOFConstants::fgkSigmaForTail2}; //for tail                                                   
  Int_t iz = 0, ix = 0;
  Float_t dX = 0., dZ = 0., x = 0., z = 0.;
  Float_t h = fHparameter, h2 = fH2parameter, k = fKparameter, k2 = fK2parameter;
  Float_t effX = 0., effZ = 0., resX = 0., resZ = 0., timeWalkX = 0., timeWalkZ = 0.;
  Float_t logOfqInd = 0.;
  Float_t weightsSum = 0.;
  Int_t nTail[4]  = {0,0,0,0};
  Int_t padId[4]  = {0,0,0,0};
  Float_t eff[4]  = {0.,0.,0.,0.};
  Float_t res[4]  = {0.,0.,0.,0.};
  //  Float_t qCenterPad = fMinimumCharge * fMinimumCharge;
  Float_t qCenterPad = 1.;
  Float_t timeWalk[4]  = {0.,0.,0.,0.};
  Float_t timeDelay[4] = {0.,0.,0.,0.};
  
  nActivatedPads = 0;
  nFiredPads = 0;
  
  (z0 <= 0) ? iz = 0 : iz = 1;
  dZ = z0 + (0.5 * AliTOFConstants::fgkNpadZ - iz - 0.5) * AliTOFConstants::fgkZPad; // hit position in the pad frame, (0,0) - center of the pad
  z = 0.5 * AliTOFConstants::fgkZPad - TMath::Abs(dZ);                               // variable for eff., res. and timeWalk. functions
  iz++;                                                                              // z row: 1, ..., AliTOFConstants::fgkNpadZ = 2
  ix = (Int_t)((x0 + 0.5 * AliTOFConstants::fgkNpadX * AliTOFConstants::fgkXPad) / AliTOFConstants::fgkXPad);
  dX = x0 + (0.5 * AliTOFConstants::fgkNpadX - ix - 0.5) * AliTOFConstants::fgkXPad; // hit position in the pad frame, (0,0) - center of the pad
  x = 0.5 * AliTOFConstants::fgkXPad - TMath::Abs(dX);                               // variable for eff., res. and timeWalk. functions;
  ix++;                                                                              // x row: 1, ..., AliTOFConstants::fgkNpadX = 48
  
  ////// Pad A:
  nActivatedPads++;
  nPlace[nActivatedPads-1] = (iz - 1) * AliTOFConstants::fgkNpadX + ix;
  qInduced[nActivatedPads-1] = qCenterPad;
  padId[nActivatedPads-1] = 1;
  
  if (fEdgeEffect == 0) {
    eff[nActivatedPads-1] = fEffCenter;
    if (gRandom->Rndm() < eff[nActivatedPads-1]) {
      nFiredPads = 1;
      res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + fResCenter * fResCenter); // 10400=30^2+20^2+40^2+50^2+50^2+50^2  ns;
      isFired[nActivatedPads-1] = kTRUE;
      tofTime[nActivatedPads-1] = gRandom->Gaus(geantTime + fTimeWalkCenter, res[0]);
      averageTime = tofTime[nActivatedPads-1];
    }
  } else {
     
    if(z < h) {
      if(z < h2) {
	effZ = fEffBoundary + (fEff2Boundary - fEffBoundary) * z / h2;
      } else {
	effZ = fEff2Boundary + (fEffCenter - fEff2Boundary) * (z - h2) / (h - h2);
      }
      resZ = fResBoundary + (fResCenter - fResBoundary) * z / h;
      timeWalkZ = fTimeWalkBoundary + (fTimeWalkCenter - fTimeWalkBoundary) * z / h;
      nTail[nActivatedPads-1] = 1;
    } else {
      effZ = fEffCenter;
      resZ = fResCenter;
      timeWalkZ = fTimeWalkCenter;
    }
    
    if(x < h) {
      if(x < h2) {
	effX = fEffBoundary + (fEff2Boundary - fEffBoundary) * x / h2;
      } else {
	effX = fEff2Boundary + (fEffCenter - fEff2Boundary) * (x - h2) / (h - h2);
      }
      resX = fResBoundary + (fResCenter - fResBoundary) * x / h;
      timeWalkX = fTimeWalkBoundary + (fTimeWalkCenter - fTimeWalkBoundary) * x / h;
      nTail[nActivatedPads-1] = 1;
    } else {
      effX = fEffCenter;
      resX = fResCenter;
      timeWalkX = fTimeWalkCenter;
    }
    
    (effZ<effX) ? eff[nActivatedPads-1] = effZ : eff[nActivatedPads-1] = effX;
    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resZ * resZ); // 10400=30^2+20^2+40^2+50^2+50^2+50^2  ns
    (timeWalkZ<timeWalkX) ? timeWalk[nActivatedPads-1] = 0.001 *  timeWalkZ : timeWalk[nActivatedPads-1] = 0.001 * timeWalkX; // ns


    ////// Pad B:
    if(z < k2) {
      effZ = fEffBoundary - (fEffBoundary - fEff3Boundary) * (z / k2);
    } else {
      effZ = fEff3Boundary * (k - z) / (k - k2);
    }
    resZ = fResBoundary + fResSlope * z / k;
    timeWalkZ = fTimeWalkBoundary + fTimeWalkSlope * z / k;
    
    if(z < k && z > 0) {
      if( (iz == 1 && dZ > 0) || (iz == 2 && dZ < 0) ) {
	nActivatedPads++;
	nPlace[nActivatedPads-1] = nPlace[0] + (3 - 2 * iz) * AliTOFConstants::fgkNpadX;
	eff[nActivatedPads-1] = effZ;
	res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resZ * resZ); // 10400=30^2+20^2+40^2+50^2+50^2+50^2 ns 
	timeWalk[nActivatedPads-1] = 0.001 * timeWalkZ; // ns
	nTail[nActivatedPads-1] = 2;
	if (fTimeDelayFlag) {
	  //	  qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * z / 2.);
	  //	  qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * z / 2.);
	  qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * z);
	  logOfqInd = gRandom->Gaus(-fPulseHeightSlope * z, fLogChargeSmearing);
	  timeDelay[nActivatedPads-1] = gRandom->Gaus(-fTimeDelaySlope * logOfqInd, fTimeSmearing);
	} else {
	  timeDelay[nActivatedPads-1] = 0.;
	}
	padId[nActivatedPads-1] = 2;
      }
    }

    
    ////// Pad C, D, E, F:
    if(x < k2) {
      effX = fEffBoundary - (fEffBoundary - fEff3Boundary) * (x / k2);
    } else {
      effX = fEff3Boundary * (k - x) / (k - k2);
    }
    resX = fResBoundary + fResSlope*x/k;
    timeWalkX = fTimeWalkBoundary + fTimeWalkSlope*x/k;
    
    if(x < k && x > 0) {
      //   C:
      if(ix > 1 && dX < 0) {
	nActivatedPads++;
	nPlace[nActivatedPads-1] = nPlace[0] - 1;
	eff[nActivatedPads-1] = effX;
	res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resX * resX); // 10400=30^2+20^2+40^2+50^2+50^2+50^2 ns 
	timeWalk[nActivatedPads-1] = 0.001 * timeWalkX; // ns
	nTail[nActivatedPads-1] = 2;
	if (fTimeDelayFlag) {
	  //	  qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * x / 2.);
	  //	  qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * x / 2.);
	  qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * x);
	  logOfqInd = gRandom->Gaus(-fPulseHeightSlope * x, fLogChargeSmearing);
	  timeDelay[nActivatedPads-1] = gRandom->Gaus(-fTimeDelaySlope * logOfqInd, fTimeSmearing);
	} else {
	  timeDelay[nActivatedPads-1] = 0.;
	}
	padId[nActivatedPads-1] = 3;

	//     D:
	if(z < k && z > 0) {
	  if( (iz == 1 && dZ > 0) || (iz == 2 && dZ < 0) ) {
	    nActivatedPads++;
	    nPlace[nActivatedPads-1] = nPlace[0] + (3 - 2 * iz) * AliTOFConstants::fgkNpadX - 1;
	    eff[nActivatedPads-1] = effX * effZ;
	    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resZ * resZ); // 10400=30^2+20^2+40^2+50^2+50^2+50^2 ns
	    (timeWalkZ<timeWalkX) ? timeWalk[nActivatedPads-1] = 0.001 * timeWalkZ : timeWalk[nActivatedPads-1] = 0.001 * timeWalkX; // ns
	    
	    nTail[nActivatedPads-1] = 2;
	    if (fTimeDelayFlag) {
	      if (TMath::Abs(x) < TMath::Abs(z)) {
		//		qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * z / 2.);
		//		qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * z / 2.);
		qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * z);
		logOfqInd = gRandom->Gaus(-fPulseHeightSlope * z, fLogChargeSmearing);
	      } else {
		//		qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * x / 2.);
		//		qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * x / 2.);
		qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * x);
		logOfqInd = gRandom->Gaus(-fPulseHeightSlope * x, fLogChargeSmearing);
	      }
	      timeDelay[nActivatedPads-1] = gRandom->Gaus(-fTimeDelaySlope * logOfqInd, fTimeSmearing);
	    } else {
	      timeDelay[nActivatedPads-1] = 0.;
	    }
	    padId[nActivatedPads-1] = 4;
	  }
	}  // end D
      }  // end C
      
      //   E:
      if(ix < AliTOFConstants::fgkNpadX && dX > 0) {
	nActivatedPads++;
	nPlace[nActivatedPads-1] = nPlace[0] + 1;
	eff[nActivatedPads-1] = effX;
	res[nActivatedPads-1] = 0.001 * (TMath::Sqrt(10400 + resX * resX)); // ns
	timeWalk[nActivatedPads-1] = 0.001 * timeWalkX; // ns
	nTail[nActivatedPads-1] = 2;
	if (fTimeDelayFlag) {
	  //	  qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * x / 2.);
	  //	  qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * x / 2.);
	  qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * x);
	  logOfqInd = gRandom->Gaus(-fPulseHeightSlope * x, fLogChargeSmearing);
	  timeDelay[nActivatedPads-1] = gRandom->Gaus(-fTimeDelaySlope * logOfqInd, fTimeSmearing);
	} else {
	  timeDelay[nActivatedPads-1] = 0.;
	}
	padId[nActivatedPads-1] = 5;


	//     F:
	if(z < k && z > 0) {
	  if( (iz == 1 && dZ > 0) || (iz == 2 && dZ < 0) ) {
	    nActivatedPads++;
	    nPlace[nActivatedPads - 1] = nPlace[0] + (3 - 2 * iz) * AliTOFConstants::fgkNpadX + 1;
	    eff[nActivatedPads - 1] = effX * effZ;
	    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(10400 + resZ * resZ); // 10400=30^2+20^2+40^2+50^2+50^2+50^2 ns
	    (timeWalkZ<timeWalkX) ? timeWalk[nActivatedPads-1] = 0.001 * timeWalkZ : timeWalk[nActivatedPads-1] = 0.001*timeWalkX; // ns
	    nTail[nActivatedPads-1] = 2;
	    if (fTimeDelayFlag) {
	      if (TMath::Abs(x) < TMath::Abs(z)) {
		//		qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * z / 2.);
		//		qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * z / 2.);
		qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * z);
		logOfqInd = gRandom->Gaus(-fPulseHeightSlope * z, fLogChargeSmearing);
	      } else {
		//		qInduced[0] = fMinimumCharge * TMath::Exp(fPulseHeightSlope * x / 2.);
		//		qInduced[nActivatedPads-1] = fMinimumCharge * TMath::Exp(-fPulseHeightSlope * x / 2.);
		qInduced[nActivatedPads-1] = TMath::Exp(-fPulseHeightSlope * x);
		logOfqInd = gRandom->Gaus(-fPulseHeightSlope * x, fLogChargeSmearing);
	      }
	      timeDelay[nActivatedPads-1] = gRandom->Gaus(-fTimeDelaySlope * logOfqInd, fTimeSmearing);
	    } else {
	      timeDelay[nActivatedPads-1] = 0.;
	    }
	    padId[nActivatedPads-1] = 6;
	  }
	}  // end F
      }  // end E
    } // end if(x < k)


    for (Int_t iPad = 0; iPad < nActivatedPads; iPad++) {
      if (res[iPad] < fTimeResolution) res[iPad] = fTimeResolution;
      if(gRandom->Rndm() < eff[iPad]) {
	isFired[iPad] = kTRUE;
	nFiredPads++;
	if(fEdgeTails) {
	  if(nTail[iPad] == 0) {
	    tofTime[iPad] = gRandom->Gaus(geantTime + timeWalk[iPad] + timeDelay[iPad], res[iPad]);
	  } else {
	    ftail->SetParameters(res[iPad], 2. * res[iPad], kSigmaForTail[nTail[iPad]-1]);
	    Double_t timeAB = ftail->GetRandom();
	    tofTime[iPad] = geantTime + timeWalk[iPad] + timeDelay[iPad] + timeAB;
	  }
	} else {
	  tofTime[iPad] = gRandom->Gaus(geantTime + timeWalk[iPad] + timeDelay[iPad], res[iPad]);
	}
	if (fAverageTimeFlag) {
	  averageTime += tofTime[iPad] * qInduced[iPad];
	  weightsSum += qInduced[iPad];
	} else {
	  averageTime += tofTime[iPad];
	  weightsSum += 1.;
	}
      }
    }
    if (weightsSum!=0) averageTime /= weightsSum;
  } // end else (fEdgeEffect != 0)
}

//__________________________________________________________________
void AliTOFSDigitizer::PrintParameters()const
{
  //
  // Print parameters used for sdigitization
  //
  cout << " ------------------- "<< GetName() << " -------------" << endl ;
  cout << " Parameters used for TOF SDigitization " << endl ;
  //  Printing the parameters
  
  cout << " Number of events:                        " << (fEvent2-fEvent1) << endl; 
  cout << " from event " << fEvent1 << " to event " << (fEvent2-1) << endl; 
  cout << " Time Resolution (ns) "<< fTimeResolution <<" Pad Efficiency: "<< fpadefficiency << endl;
  cout << " Edge Effect option:  "<<  fEdgeEffect<< endl;

  cout << " Boundary Effect Simulation Parameters " << endl;
  cout << " Hparameter: "<< fHparameter<<"  H2parameter:"<< fH2parameter <<"  Kparameter:"<< fKparameter<<"  K2parameter: "<< fK2parameter << endl;
  cout << " Efficiency in the central region of the pad: "<< fEffCenter << endl;
  cout << " Efficiency at the boundary region of the pad: "<< fEffBoundary << endl;
  cout << " Efficiency value at H2parameter "<< fEff2Boundary << endl;
  cout << " Efficiency value at K2parameter "<< fEff3Boundary << endl;
  cout << " Resolution (ps) in the central region of the pad: "<< fResCenter << endl;
  cout << " Resolution (ps) at the boundary of the pad      : "<< fResBoundary << endl;
  cout << " Slope (ps/K) for neighbouring pad               : "<< fResSlope <<endl;
  cout << " Time walk (ps) in the central region of the pad : "<< fTimeWalkCenter << endl;
  cout << " Time walk (ps) at the boundary of the pad       : "<< fTimeWalkBoundary<< endl;
  cout << " Slope (ps/K) for neighbouring pad               : "<< fTimeWalkSlope<<endl;
  cout << " Pulse Heigth Simulation Parameters " << endl;
  cout << " Flag for delay due to the PulseHeightEffect: "<< fTimeDelayFlag <<endl;
  cout << " Pulse Height Slope                           : "<< fPulseHeightSlope<<endl;
  cout << " Time Delay Slope                             : "<< fTimeDelaySlope<<endl;
  cout << " Minimum charge amount which could be induced : "<< fMinimumCharge<<endl;
  cout << " Smearing in charge in (q1/q2) vs x plot      : "<< fChargeSmearing<<endl;
  cout << " Smearing in log of charge ratio              : "<< fLogChargeSmearing<<endl;
  cout << " Smearing in time in time vs log(q1/q2) plot  : "<< fTimeSmearing<<endl;
  cout << " Flag for average time                        : "<< fAverageTimeFlag<<endl;
  cout << " Edge tails option                            : "<< fEdgeTails << endl;
  
}

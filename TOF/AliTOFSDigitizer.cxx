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

#include "AliDetector.h"
#include "AliLoader.h"
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliTOF.h"
#include "AliTOFGeometry.h"
#include "AliTOFHitMap.h"
#include "AliTOFSDigit.h"
#include "AliTOFSDigitizer.h"
#include "AliTOFhit.h"
#include "AliTOFhitT0.h"
#include "AliMC.h"

ClassImp(AliTOFSDigitizer)

//____________________________________________________________________________ 
  AliTOFSDigitizer::AliTOFSDigitizer():TTask("TOFSDigitizer","") 
{
  // ctor

  fRunLoader      = 0;
  fTOFLoader      = 0;

  fEvent1         = 0;
  fEvent2         = 0;
  ftail           = 0;
  fSelectedSector = -1;
  fSelectedPlate  = -1;
}

//____________________________________________________________________________ 
AliTOFSDigitizer::AliTOFSDigitizer(const char* HeaderFile, Int_t evNumber1, Int_t nEvents):TTask("TOFSDigitizer","")
{
  ftail    = 0;
  fSelectedSector=-1; // by default we sdigitize all sectors
  fSelectedPlate =-1; // by default we sdigitize all plates in all sectors
  
  fHeadersFile = HeaderFile ; // input filename (with hits)
  TFile * file = (TFile*) gROOT->GetFile(fHeadersFile.Data());
  
  //File was not opened yet open file and get alirun object
  if (file == 0) {
    file   = TFile::Open(fHeadersFile.Data(),"update") ;
    gAlice = (AliRun *) file->Get("gAlice") ;
  }
  
  // add Task to //root/Tasks folder
  TString evfoldname = AliConfig::GetDefaultEventFolderName();
  fRunLoader = AliRunLoader::GetRunLoader(evfoldname);
  if (!fRunLoader)
    fRunLoader = AliRunLoader::Open(HeaderFile);//open session and mount on default event folder
  if (fRunLoader == 0x0)
    {
      Fatal("AliTOFSDigitizer","Event is not loaded. Exiting");
      return;
    }

  if (fRunLoader->TreeE() == 0x0) fRunLoader->LoadHeader();
  
  if (evNumber1>=0) fEvent1 = evNumber1;
  else fEvent1=0;
  
  if (nEvents==0) fEvent2 = (Int_t)(fRunLoader->GetNumberOfEvents());
  else if (nEvents>0) fEvent2 = evNumber1+nEvents;
  else fEvent2 = 1;
  
  if (!(fEvent2>fEvent1)) {
    cout << " ERROR: fEvent2 = " << fEvent2 << " <= fEvent1 = " << fEvent1 << endl;
    fEvent1 = 0;
    fEvent2 = 1;
    cout << " Correction: fEvent2 = " << fEvent2 << " <= fEvent1 = " << fEvent1 << endl;
  }
  
  // init parameters for sdigitization
  InitParameters();
  
  fTOFLoader = fRunLoader->GetLoader("TOFLoader");
  if (fTOFLoader == 0x0)
    {
      Fatal("AliTOFSDigitizer","Can not find TOF loader in event. Exiting.");
      return;
    }
  fTOFLoader->PostSDigitizer(this);
}

//____________________________________________________________________________ 
AliTOFSDigitizer::~AliTOFSDigitizer()
{
  // dtor
  fTOFLoader->CleanSDigitizer();
}

//____________________________________________________________________________ 
void AliTOFSDigitizer::InitParameters()
{
  // set parameters for detector simulation
  
  fTimeResolution = 0.080; //0.120; OLD
  fpadefficiency  = 0.99 ;
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
  fAddTRes        = 68. ; // \sqrt{2x20^2 + 15^2 + 2x10^2 + 30^2 + 50^2} (Pb-Pb)
  //fAddTRes      = 48. ; // \sqrt{2x20^2 + 15^2 + 2x10^2 + 30^2 + 15^2} (p-p)
  // 30^2+20^2+40^2+50^2+50^2+50^2 = 10400 ps^2 (very old value)
  fResCenter      = 35. ; //50. ; // OLD
  fResBoundary    = 70. ;
  fResSlope       = 37. ; //40. ; // OLD
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
  fTdcBin   = 50.;     // 1 TDC bin = 50 ps
  fAdcBin   = 0.25;    // 1 ADC bin = 0.25 pC (or 0.03 pC)
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
void AliTOFSDigitizer::Exec(Option_t *verboseOption) { 

  if (strstr(verboseOption,"tim") || strstr(verboseOption,"all"))
    gBenchmark->Start("TOFSDigitizer");

  if (fEdgeTails) ftail = new TF1("tail",TimeWithTail,-2,2,3);
  
  Int_t nselectedHits=0;
  Int_t ntotalsdigits=0;
  Int_t ntotalupdates=0;
  Int_t nnoisesdigits=0;
  Int_t nsignalsdigits=0;
  Int_t nHitsFromPrim=0;
  Int_t nHitsFromSec=0;
  Int_t nlargeTofDiff=0;

  Bool_t thereIsNotASelection=(fSelectedSector==-1) && (fSelectedPlate==-1);

  if (fRunLoader->GetAliRun() == 0x0) fRunLoader->LoadgAlice();
  gAlice = fRunLoader->GetAliRun();

  fRunLoader->LoadKinematics();
  
  AliTOF *tof = (AliTOF *) gAlice->GetDetector("TOF");
  
  if (!tof) {
    Error("AliTOFSDigitizer","TOF not found");
    return;
  }
  
  fTOFLoader->LoadHits("read");
  fTOFLoader->LoadSDigits("recreate");
  
  for (Int_t iEvent=fEvent1; iEvent<fEvent2; iEvent++) {
//     cout << "------------------- "<< GetName() << " ------------- \n";
//     cout << "Sdigitizing event " << iEvent << endl;

    fRunLoader->GetEvent(iEvent);

    TTree *hitTree = fTOFLoader->TreeH ();
    if (!hitTree) return;

    if (fTOFLoader->TreeS () == 0) fTOFLoader->MakeTree ("S");
    
    //Make branch for digits
    tof->MakeBranch("S");
    
    // recreate TClonesArray fSDigits - for backward compatibility
    if (tof->SDigits() == 0) {
      tof->CreateSDigitsArray();
    } else {
      tof->RecreateSDigitsArray();
    }

    tof->SetTreeAddress();

    Int_t version=tof->IsVersion();

    Int_t nselectedHitsinEv=0;
    Int_t ntotalsdigitsinEv=0;
    Int_t ntotalupdatesinEv=0;
    Int_t nnoisesdigitsinEv=0;
    Int_t nsignalsdigitsinEv=0;

    TParticle *particle;
    //AliTOFhit *tofHit;
    TClonesArray *tofHitArray = tof->Hits();

    // create hit map
    AliTOFHitMap *hitMap = new AliTOFHitMap(tof->SDigits());

    TBranch * tofHitsBranch = hitTree->GetBranch("TOF");

    Int_t ntracks = static_cast<Int_t>(hitTree->GetEntries());
    for (Int_t track = 0; track < ntracks; track++)
    {
      gAlice->ResetHits();
      tofHitsBranch->GetEvent(track);
      particle = gAlice->GetMCApp()->Particle(track);
      Int_t nhits = tofHitArray->GetEntriesFast();
      // cleaning all hits of the same track in the same pad volume
      // it is a rare event, however it happens

      Int_t previousTrack =-1;
      Int_t previousSector=-1;
      Int_t previousPlate =-1;
      Int_t previousStrip =-1;
      Int_t previousPadX  =-1;
      Int_t previousPadZ  =-1;

      for (Int_t hit = 0; hit < nhits; hit++) {
	Int_t    vol[5];       // location for a digit
	Float_t  digit[2];     // TOF digit variables
	Int_t tracknum;
	Float_t dxPad;
	Float_t dzPad;
	Float_t geantTime;

	// fp: really sorry for this, it is a temporary trick to have
	// track length too
	if(version!=6){
	  AliTOFhit *tofHit = (AliTOFhit *) tofHitArray->UncheckedAt(hit);
	  tracknum = tofHit->GetTrack();
	  vol[0] = tofHit->GetSector();
	  vol[1] = tofHit->GetPlate();
	  vol[2] = tofHit->GetStrip();
	  vol[3] = tofHit->GetPadx();
	  vol[4] = tofHit->GetPadz();
	  dxPad = tofHit->GetDx();
	  dzPad = tofHit->GetDz();
	  geantTime = tofHit->GetTof(); // unit [s]
	} else {
	  AliTOFhitT0 *tofHit = (AliTOFhitT0 *) tofHitArray->UncheckedAt(hit);
	  tracknum = tofHit->GetTrack();
	  vol[0] = tofHit->GetSector();
	  vol[1] = tofHit->GetPlate();
	  vol[2] = tofHit->GetStrip();
	  vol[3] = tofHit->GetPadx();
	  vol[4] = tofHit->GetPadz();
	  dxPad = tofHit->GetDx();
	  dzPad = tofHit->GetDz();
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
	    if (particle->GetFirstMother() < 0) nHitsFromPrim++; // counts hits due to primary particles
	    
	    Float_t xStrip=AliTOFGeometry::XPad()*(vol[3]+0.5-0.5*AliTOFGeometry::NpadX())+dxPad;
	    Float_t zStrip=AliTOFGeometry::ZPad()*(vol[4]+0.5-0.5*AliTOFGeometry::NpadZ())+dzPad;

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
		    (nPlace[indexOfPad]<=AliTOFGeometry::NpadX()) ? vol[4] = 0 : vol[4] = 1;
		    (nPlace[indexOfPad]<=AliTOFGeometry::NpadX()) ? vol[3] = nPlace[indexOfPad] - 1 : vol[3] = nPlace[indexOfPad] - AliTOFGeometry::NpadX() - 1;
		  }
		  // check if two sdigit are on the same pad;
		  // in that case we sum the two or more sdigits
		  if (hitMap->TestHit(vol) != kEmpty) {
		    AliTOFSDigit *sdig = static_cast<AliTOFSDigit*>(hitMap->GetHit(vol));
		    Int_t tdctime = (Int_t) digit[0];
		    Int_t adccharge = (Int_t) digit[1];
		    sdig->Update(fTdcBin,tdctime,adccharge,tracknum);
		    ntotalupdatesinEv++;
		    ntotalupdates++;
		  } else {
		    
		    tof->AddSDigit(tracknum, vol, digit);
		    
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
    
    fTOFLoader->TreeS()->Reset();
    fTOFLoader->TreeS()->Fill();
    fTOFLoader->WriteSDigits("OVERWRITE");
    
    if (tof->SDigits()) tof->ResetSDigits();
    
    if (strstr(verboseOption,"all")) {
      cout << "---------------------------------------- \n";
      cout << "       <AliTOFSDigitizer>    \n";
      cout << "After sdigitizing " << nselectedHitsinEv << " hits" << " in event " << iEvent << endl;
      //" (" << nHitsFromPrim << " from primaries and " << nHitsFromSec << " from secondaries) TOF hits, " 
      cout << ntotalsdigitsinEv << " digits have been created \n";
      cout << "(" << nsignalsdigitsinEv << " due to signals and " <<  nnoisesdigitsinEv << " due to border effect) \n";
      cout << ntotalupdatesinEv << " total updates of the hit map have been performed in current event \n";
      cout << "---------------------------------------- \n";
    }

  } //event loop on events

    fTOFLoader->UnloadSDigits();
    fTOFLoader->UnloadHits();
    fRunLoader->UnloadKinematics();
    //fRunLoader->UnloadgAlice();

  // free used memory
  if (ftail){
    delete ftail;
    ftail = 0;
  }
  
  nHitsFromSec=nselectedHits-nHitsFromPrim;
  if(strstr(verboseOption,"all")){
    cout << "---------------------------------------- \n";
    cout << "---------------------------------------- \n";
    cout << "-----------SDigitization Summary-------- \n";
    cout << "       <AliTOFSDigitizer>     \n";
    cout << "After sdigitizing " << nselectedHits << " hits  \n";
    cout << "in " << (fEvent2-fEvent1) << " events  \n";
//" (" << nHitsFromPrim << " from primaries and " << nHitsFromSec << " from secondaries) TOF hits, " 
    cout << ntotalsdigits << " sdigits have been created \n";
    cout << "(" << nsignalsdigits << " due to signals and " 
         <<  nnoisesdigits << " due to border effect) \n";
    cout << ntotalupdates << " total updates of the hit map have been performed \n";
    cout << "in " << nlargeTofDiff << " cases the time of flight difference is greater than 200 ps \n";
  }


  if(strstr(verboseOption,"tim") || strstr(verboseOption,"all")){
    gBenchmark->Stop("TOFSDigitizer");
    cout << "AliTOFSDigitizer: \n";
    cout << "   took " << gBenchmark->GetCpuTime("TOFSDigitizer") << " seconds in order to make sdigits " 
	 <<  gBenchmark->GetCpuTime("TOFSDigitizer")/(fEvent2-fEvent1) << " seconds per event \n";
    cout << " +++++++++++++++++++++++++++++++++++++++++++++++++++  \n";
  }

}

//__________________________________________________________________
void AliTOFSDigitizer::Print(Option_t* /*opt*/)const
{
  cout << "------------------- "<< GetName() << " ------------- \n";
}

//__________________________________________________________________
void AliTOFSDigitizer::SelectSectorAndPlate(Int_t sector, Int_t plate)
{
  Bool_t isaWrongSelection=(sector < 0) || (sector >= AliTOFGeometry::NSectors()) || (plate < 0) || (plate >= AliTOFGeometry::NPlates());
  if(isaWrongSelection){
    cout << "You have selected an invalid value for sector or plate " << endl;
    cout << "The correct range for sector is [0,"<< AliTOFGeometry::NSectors()-1 <<"]\n";
    cout << "The correct range for plate  is [0,"<< AliTOFGeometry::NPlates()-1  <<"]\n";
    cout << "By default we continue sdigitizing all hits in all plates of all sectors \n";
  } else {
    fSelectedSector=sector;
    fSelectedPlate =plate;
    cout << "SDigitizing only hits in plate " << fSelectedPlate << " of the sector " 
         << fSelectedSector << endl;
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
  
  const Float_t kSigmaForTail[2] = {AliTOFGeometry::SigmaForTail1(),AliTOFGeometry::SigmaForTail2()}; //for tail                                                   
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
  dZ = z0 + (0.5 * AliTOFGeometry::NpadZ() - iz - 0.5) * AliTOFGeometry::ZPad(); // hit position in the pad frame, (0,0) - center of the pad
  z = 0.5 * AliTOFGeometry::ZPad() - TMath::Abs(dZ);                               // variable for eff., res. and timeWalk. functions
  iz++;                                                                              // z row: 1, ..., AliTOFGeometry::NpadZ = 2
  ix = (Int_t)((x0 + 0.5 * AliTOFGeometry::NpadX() * AliTOFGeometry::XPad()) / AliTOFGeometry::XPad());
  dX = x0 + (0.5 * AliTOFGeometry::NpadX() - ix - 0.5) * AliTOFGeometry::XPad(); // hit position in the pad frame, (0,0) - center of the pad
  x = 0.5 * AliTOFGeometry::XPad() - TMath::Abs(dX);                               // variable for eff., res. and timeWalk. functions;
  ix++;                                                                              // x row: 1, ..., AliTOFGeometry::NpadX = 48
  
  ////// Pad A:
  nActivatedPads++;
  nPlace[nActivatedPads-1] = (iz - 1) * AliTOFGeometry::NpadX() + ix;
  qInduced[nActivatedPads-1] = qCenterPad;
  padId[nActivatedPads-1] = 1;
  
  if (fEdgeEffect == 0) {
    eff[nActivatedPads-1] = fEffCenter;
    if (gRandom->Rndm() < eff[nActivatedPads-1]) {
      nFiredPads = 1;
      res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + fResCenter * fResCenter); // ns
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
    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resZ * resZ); // ns
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
	nPlace[nActivatedPads-1] = nPlace[0] + (3 - 2 * iz) * AliTOFGeometry::NpadX();
	eff[nActivatedPads-1] = effZ;
	res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resZ * resZ); // ns 
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
	res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resX * resX); // ns 
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
	    nPlace[nActivatedPads-1] = nPlace[0] + (3 - 2 * iz) * AliTOFGeometry::NpadX() - 1;
	    eff[nActivatedPads-1] = effX * effZ;
	    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resZ * resZ); // ns
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
      if(ix < AliTOFGeometry::NpadX() && dX > 0) {
	nActivatedPads++;
	nPlace[nActivatedPads-1] = nPlace[0] + 1;
	eff[nActivatedPads-1] = effX;
	res[nActivatedPads-1] = 0.001 * (TMath::Sqrt(fAddTRes*fAddTRes + resX * resX)); // ns
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
	    nPlace[nActivatedPads - 1] = nPlace[0] + (3 - 2 * iz) * AliTOFGeometry::NpadX() + 1;
	    eff[nActivatedPads - 1] = effX * effZ;
	    (resZ<resX) ? res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resX * resX) : res[nActivatedPads-1] = 0.001 * TMath::Sqrt(fAddTRes*fAddTRes + resZ * resZ); // ns
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

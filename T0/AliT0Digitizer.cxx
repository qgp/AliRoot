
/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
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

#include <TArrayI.h>
#include <TDirectory.h>
#include <TError.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TMath.h>
#include <TRandom.h>
#include <TTree.h> 

#include "AliLog.h"
#include "AliT0Digitizer.h"
#include "AliT0.h"
#include "AliT0hit.h"
#include "AliT0digit.h"
#include "AliRunDigitizer.h"
#include <AliDetector.h>
#include "AliRun.h"
#include <AliLoader.h>
#include <AliRunLoader.h>
#include <stdlib.h>
#include <Riostream.h>
#include <Riostream.h>
#include "AliT0Parameters.h"
#include "AliCDBLocal.h"
#include "AliCDBStorage.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"

ClassImp(AliT0Digitizer)

//___________________________________________
  AliT0Digitizer::AliT0Digitizer()  :AliDigitizer(),
				     fT0(0),
				     fHits(0),
				     fdigits(0),
				     ftimeCFD(new TArrayI(24)), 
				     ftimeLED (new TArrayI(24)), 
				     fADC(new TArrayI(24)), 
				     fADC0 (new TArrayI(24)),
				     fSumMult(0),
				     fEffPMT(0)



{
// Default ctor - don't use it
  ;
}

//___________________________________________
AliT0Digitizer::AliT0Digitizer(AliRunDigitizer* manager) 
  :AliDigitizer(manager),
   fT0(0),
   fHits(0),
   fdigits(0),
   ftimeCFD(new TArrayI(24)), 
   ftimeLED (new TArrayI(24)), 
   fADC(new TArrayI(24)), 
   fADC0 (new TArrayI(24)),
   fSumMult(0),
   fEffPMT(0)
{
// ctor which should be used

  AliDebug(1,"processed");


}

//------------------------------------------------------------------------
AliT0Digitizer::~AliT0Digitizer()
{
// Destructor

  AliDebug(1,"T0");

  delete ftimeCFD;
  delete fADC;
  delete ftimeLED;
  delete  fADC0;
}

 //------------------------------------------------------------------------
Bool_t AliT0Digitizer::Init()
{
// Initialization
  AliDebug(1," Init");
 return kTRUE;
}
 

//---------------------------------------------------------------------

void AliT0Digitizer::Exec(Option_t* /*option*/)
{

  /*
    Produde digits from hits
        digits is TObject and includes
	We are writing array if C & A  TDC
	C & A  ADC (will need for slow simulation)
	TOF first particle C & A
	mean time and time difference (vertex position)
	
  */

  //output loader 
  AliRunLoader *outRL = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  AliLoader * pOutStartLoader = outRL->GetLoader("T0Loader");

  AliDebug(1,"start...");
  //input loader
  //
  // From hits to digits
  //
 Int_t hit, nhits;
  Int_t countE[24];
  Int_t volume, pmt, trCFD, trLED; 
  Float_t sl, qt;
  Int_t  bestATDC, bestCTDC, qtCh;
  Float_t time[24], besttime[24], timeGaus[24] ;
    //Q->T-> coefficients !!!! should be asked!!!
  Float_t timeDelayCFD[24], timeDelayLED[24];
  Int_t threshold =50; //photoelectrons
  Float_t zdetA, zdetC;
   Int_t sumMultCoeff = 100;
  TObjArray slewingLED;
  TObjArray walk;
  TH1F *hr ;

  AliT0Parameters* param = AliT0Parameters::Instance();
  param->Init();

  Int_t ph2Mip = param->GetPh2Mip();     
  Int_t channelWidth = param->GetChannelWidth() ;  
  Float_t delayVertex = param->GetTimeDelayTVD();
  for (Int_t i=0; i<24; i++){
    timeDelayCFD[i] = param->GetTimeDelayCFD(i);
    timeDelayLED[i] = param->GetTimeDelayLED(i);
    TGraph* gr = param ->GetSlew(i);
    slewingLED.AddAtAndExpand(gr,i);

    TGraph* fu = param ->GetWalk(i);
    walk.AddAtAndExpand(fu,i);

    TGraph* grEff = param ->GetPMTeff(i);
    fEffPMT.AddAtAndExpand(grEff,i);
  }
 
 
   zdetC = TMath::Abs(param->GetZPosition("T0/C/PMT1"));
   zdetA  = TMath::Abs(param->GetZPosition("T0/A/PMT15"));

   AliT0hit  *startHit;
   TBranch *brHits=0;
   
  Int_t nFiles=fManager->GetNinputs();
  for (Int_t inputFile=0; inputFile<nFiles;  inputFile++) {
    if (inputFile < nFiles-1) {
	AliWarning(Form("ignoring input stream %d", inputFile));
	continue;
	
    }
    
    Float_t besttimeC=99999.;
    Float_t besttimeA=99999.;
    Int_t pmtBestC=9999;
    Int_t pmtBestA=9999;
    Int_t timeDiff=999, meanTime=0;
    Int_t sumMult =0;   fSumMult=0;
    bestATDC = 99999;  bestCTDC = 99999;
 

    ftimeCFD -> Reset();
    fADC -> Reset();
    fADC0 -> Reset();
    ftimeLED ->Reset();
    for (Int_t i0=0; i0<24; i0++)
      {
	time[i0]=besttime[i0]=timeGaus[i0]=99999; countE[i0]=0;
      }
    AliRunLoader * inRL = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(inputFile));
    AliLoader * pInStartLoader = inRL->GetLoader("T0Loader");
    if (!inRL->GetAliRun()) inRL->LoadgAlice();
    AliT0 *fT0  = (AliT0*)inRL ->GetAliRun()->GetDetector("T0");

       //read Hits 
    pInStartLoader->LoadHits("READ");//probably it is necessary to load them before
    TClonesArray *fHits = fT0->Hits ();
    TTree *th = pInStartLoader->TreeH();
    brHits = th->GetBranch("T0");
    if (brHits) {
      fT0->SetHitsAddressBranch(brHits);
    }else{
      AliError("Branch T0 hit not found");
      exit(111);
    } 
    Int_t ntracks    = (Int_t) th->GetEntries();
    
    if (ntracks<=0) return;
    // Start loop on tracks in the hits containers
    for (Int_t track=0; track<ntracks;track++) {
      brHits->GetEntry(track);
      nhits = fHits->GetEntriesFast();
      for (hit=0;hit<nhits;hit++) 
	{
	  startHit   = (AliT0hit*) fHits->UncheckedAt(hit);
	  if (!startHit) {
 	    AliError("The unchecked hit doesn't exist");
	    break;
	  }
	  pmt=startHit->Pmt();
	  Int_t numpmt=pmt-1;
	  Double_t e=startHit->Etot();
	  volume = startHit->Volume();
	  
	  //	  if(e>0 && RegisterPhotoE(numpmt,e)) {
	  if(e>0 ) {
	    countE[numpmt]++;
	    besttime[numpmt] = startHit->Time();
	    if(besttime[numpmt]<time[numpmt])
	      {
		time[numpmt]=besttime[numpmt];
	      }
	  } //photoelectron accept 
	} //hits loop
    } //track loop
    
    //spread time A&C by 25ps   && besttime
    Float_t c = 0.0299792; // cm/ps
    
    Float_t koef=(zdetA-zdetC)/c; //correction position difference by cable
    for (Int_t ipmt=0; ipmt<12; ipmt++){
      if(countE[ipmt] > threshold) {
	timeGaus[ipmt]=gRandom->Gaus(time[ipmt],25)+koef;
	if(timeGaus[ipmt]<besttimeC){
	  besttimeC=timeGaus[ipmt]; //timeC
	  pmtBestC=ipmt;}
     }
    }
     for ( Int_t ipmt=12; ipmt<24; ipmt++){
      if(countE[ipmt] > threshold) {
	timeGaus[ipmt]=gRandom->Gaus(time[ipmt],25); 
	if(timeGaus[ipmt]<besttimeA) {
	  besttimeA=timeGaus[ipmt]; //timeA
	  pmtBestA=ipmt;}
      }	
    }
   //folding with alignmentz position distribution  
    if( besttimeC > 10000. && besttimeC <15000)
      bestCTDC=Int_t ((besttimeC+timeDelayCFD[pmtBestC])
			 /channelWidth);
 
    if( besttimeA > 10000. && besttimeA <15000)
      bestATDC=Int_t ((besttimeA+timeDelayCFD[pmtBestA])
			/channelWidth);

    if (bestATDC < 99999 && bestCTDC < 99999)
      {
	timeDiff=Int_t (((besttimeC-besttimeA)+1000*delayVertex)
			/channelWidth);
	meanTime=Int_t (((besttimeC+timeDelayCFD[pmtBestC]+
			  besttimeA+timeDelayCFD[pmtBestA])/2.)
			/channelWidth);
      }
	AliDebug(10,Form(" time A& C %i %i  time diff && mean time in channels %i %i",bestATDC,bestCTDC, timeDiff, meanTime));
    for (Int_t i=0; i<24; i++)
      {
       	Float_t  al = countE[i]; 
	if (al>threshold && timeGaus[i]<50000 ) {
	  //fill ADC
	  // QTC procedure:
	  // phe -> mV 0.3; 1MIP ->500phe -> ln (amp (mV)) = 5;
	  // max 200ns, HIJING  mean 50000phe -> 15000mv -> ln = 15 (s zapasom)
	  // channel 25ps
	  qt= 50.*al/ph2Mip;  // 50mv/Mip amp in mV 
	  //  fill TDC
	  trCFD = Int_t (timeGaus[i]/channelWidth + (timeDelayCFD[i]-timeDelayCFD[0])); 
	  trLED= Int_t (timeGaus[i] + timeDelayLED[i]); 
	  sl = ((TGraph*)slewingLED.At(i))->Eval(qt);
	  trLED = Int_t(( trLED + 1000*sl )/channelWidth);
	  qtCh=Int_t (1000.*TMath::Log(qt)) / channelWidth;
	  fADC0->AddAt(0,i);
	  fADC->AddAt(qtCh,i);
	  ftimeLED->AddAt(trLED,i); 
	  //	  sumMult += Int_t ((al*gain[i]/ph2Mip)*50) ;
	  sumMult += Int_t (qt/sumMultCoeff)  ;
	 
	AliDebug(10,Form("  pmt %i : time in ns %f time in channels %i   ",
			i, timeGaus[i],trCFD ));
	AliDebug(10,Form(" qt in mV %f qt in ns %f qt in channels %i   ",qt, 
			TMath::Log(qt), qtCh));
	// put slewing 
	TGraph *fu1=(TGraph*) walk.At(i);
	Float_t slew=fu1->Eval(Float_t(qtCh));
	hr=fu1->GetHistogram();
	Float_t maxValue=hr->GetMaximum(50);
	trCFD=trCFD-Int_t((maxValue-slew)/channelWidth);
	ftimeCFD->AddAt(Int_t (trCFD),i);
	}
      } //pmt loop

    if (sumMult > threshold){
      fSumMult =  Int_t (1000.* TMath::Log(Double_t(sumMult) / Double_t(sumMultCoeff))
			 /channelWidth);
      AliDebug(10,Form("summult mv %i   mult  in chammens %i in ps %i ", 
		      sumMult, fSumMult, fSumMult*channelWidth));
    }

      fT0->AddDigit(bestATDC,bestCTDC,meanTime,timeDiff,fSumMult,
		       ftimeCFD,fADC0,ftimeLED,fADC);
     
      AliDebug(10,Form(" Digits wrote bestATDC %i bestCTDC %i  meanTime %i  timeDiff %i fSumMult %i ", bestATDC,bestCTDC,meanTime,timeDiff,fSumMult ));
    pOutStartLoader->UnloadHits();
  } //input streams loop
  
    //load digits    
    pOutStartLoader->LoadDigits("UPDATE");
    TTree *treeD  = pOutStartLoader->TreeD();
    if (treeD == 0x0) {
      pOutStartLoader->MakeTree("D");
      treeD = pOutStartLoader->TreeD();
    }
    treeD->Reset();
    fT0  = (AliT0*)outRL ->GetAliRun()->GetDetector("T0");
    // Make a branch in the tree 
    fT0->MakeBranch("D");
     treeD->Fill();
  
     pOutStartLoader->WriteDigits("OVERWRITE");
     
     fT0->ResetDigits();
     pOutStartLoader->UnloadDigits();
     
}

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

///////////////////////////////////////////////////////////////////////////////
// PHOS Preprocessor class. It runs by Shuttle at the end of the run,
// calculates calibration coefficients and dead/bad channels
// to be posted in OCDB
//
// Author: Boris Polichtchouk, 4 October 2006
///////////////////////////////////////////////////////////////////////////////

#include "AliPHOSPreprocessor.h"
#include "AliLog.h"
#include "AliCDBMetaData.h"
#include "AliPHOSEmcCalibData.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TMap.h"
#include "TRandom.h"
#include "TKey.h"
#include "TList.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"
#include "TMath.h"
#include "AliPHOSEmcBadChannelsMap.h"

ClassImp(AliPHOSPreprocessor)

  Double_t rgaus(Double_t *x, Double_t *par)
{
  Double_t gaus = par[0] * TMath::Exp( -(x[0]-par[1])*(x[0]-par[1]) /
                                       (2*par[2]*par[2]) );
  return gaus;
}

//_______________________________________________________________________________________
AliPHOSPreprocessor::AliPHOSPreprocessor() :
AliPreprocessor("PHS",0)
{
  //default constructor
}

//_______________________________________________________________________________________
AliPHOSPreprocessor::AliPHOSPreprocessor(AliShuttleInterface* shuttle):
AliPreprocessor("PHS",shuttle)
{
  // Constructor
}

//_______________________________________________________________________________________
UInt_t AliPHOSPreprocessor::Process(TMap* /*valueSet*/)
{
  // process data retrieved by the Shuttle
  
  TString runType = GetRunType();
  Log(Form("Run type: %s",runType.Data()));

  if(runType=="STANDALONE") {
    Bool_t ledOK = ProcessLEDRun();
    if(ledOK) return 0;
    else
      return 1;
  }
  
  if(runType=="PHYSICS") {
    
    Bool_t badmap_OK = FindBadChannelsEmc();
    if(!badmap_OK) Log(Form("WARNING!! FindBadChannels() completed with BAD status!"));
    
    Bool_t calibEmc_OK = CalibrateEmc();

    if(calibEmc_OK && badmap_OK) return 0;
    else
      return 1;
  }

  Log(Form("Unknown run type %s. Do nothing and return OK.",runType.Data()));
  return 0;

}


Bool_t AliPHOSPreprocessor::ProcessLEDRun()
{
  //Process LED run, fill bad channels map.

  AliPHOSEmcBadChannelsMap badMap;

  TList* list = GetFileSources(kDAQ, "LED");
  if(!list) {
    Log("Sources list for LED run not found, exit.");
    return kFALSE;
  }

  TIter iter(list);
  TObjString *source;
  char hnam[80];
  TH1F* histo=0;
  
  while ((source = dynamic_cast<TObjString *> (iter.Next()))) {

    AliInfo(Form("found source %s", source->String().Data()));

    TString fileName = GetFile(kDAQ, "LED", source->GetName());
    AliInfo(Form("Got filename: %s",fileName.Data()));

    TFile f(fileName);

    if(!f.IsOpen()) {
      Log(Form("File %s is not opened, something goes wrong!",fileName.Data()));
      return kFALSE;
    }

    const Int_t nMod=5; // 1:5 modules
    const Int_t nCol=56; //1:56 columns in each module
    const Int_t nRow=64; //1:64 rows in each module

    // Check for dead channels    
    Log(Form("Begin check for dead channels."));

    for(Int_t mod=0; mod<nMod; mod++) {
      for(Int_t col=0; col<nCol; col++) {
	for(Int_t row=0; row<nRow; row++) {
	  sprintf(hnam,"mod%dcol%drow%d",mod,col,row);
	  histo = (TH1F*)f.Get(hnam);
	  if(histo)
	    if (histo->GetMean()<1) {
	      Log(Form("Channel: [%d,%d,%d] seems dead, <E>=%.1f.",mod,col,row,histo->GetMean()));
	      badMap.SetBadChannel(mod,col,row);
	    }
	}
      }
    }

  }

  //Store bad channels map
  AliCDBMetaData badMapMetaData;

  //Bad channels data valid from current run fRun until updated (validityInfinite=kTRUE)
  Bool_t result = Store("Calib", "EmcBadChannels", &badMap, &badMapMetaData, fRun, kTRUE);
  return result;

}

Float_t AliPHOSPreprocessor::HG2LG(Int_t mod, Int_t X, Int_t Z, TFile* f)
{
  //Calculates High gain to Low gain ratio 
  //for crystal at the position (X,Z) in the PHOS module mod.
  
  char hname[128];
  sprintf(hname,"%d_%d_%d",mod,X,Z);

  TH1F* h1 = (TH1F*)f->Get(hname);
  if(!h1) return 16.;

  if(!h1->GetEntries()) return 16.;
  if(h1->GetMaximum()<10.) return 16.;

  Double_t max = h1->GetBinCenter(h1->GetMaximumBin()); // peak
  Double_t xmin = max - (h1->GetRMS()/3);
  Double_t xmax = max + (h1->GetRMS()/2);
  //       Double_t xmin = max - (h1->GetRMS());
  //       Double_t xmax = max + (h1->GetRMS());

  TF1* gaus1 = new TF1("gaus1",rgaus,xmin,xmax,3);
  gaus1->SetParNames("Constant","Mean","Sigma");
  gaus1->SetParameter("Constant",h1->GetMaximum());
  gaus1->SetParameter("Mean",max);
  gaus1->SetParameter("Sigma",1.);
  gaus1->SetLineColor(kBlue);
  h1->Fit(gaus1,"LERQ+");

  Log(Form("\t%s: %d entries, mean=%.3f, peak=%.3f, rms= %.3f. HG/LG = %.3f\n",
	   h1->GetTitle(),h1->GetEntries(),h1->GetMean(),max,h1->GetRMS(),
	   gaus1->GetParameter("Mean"))); 

  return gaus1->GetParameter("Mean");

}

Bool_t AliPHOSPreprocessor::FindBadChannelsEmc()
{
  //Creates the bad channels map for PHOS EMC.

  // The file fileName contains histograms which have been produced by DA2 detector algorithm.
  // It is a responsibility of the SHUTTLE framework to form the fileName.

  AliPHOSEmcBadChannelsMap badMap;

  TList* list = GetFileSources(kDAQ, "BAD_CHANNELS");

  if(!list) {
    Log("Sources list for BAD_CHANNELS not found, exit.");
    return kFALSE;
  }

  if(!list->GetEntries()) {
    Log(Form("Got empty sources list. It seems DA2 did not produce any files!"));
    return kFALSE;
  }

  TIter iter(list);
  TObjString *source;
  char hnam[80];
  TH1F* h1=0;

  const Float_t fQualityCut = 1.;
  Int_t nGoods[5] = {0,0,0,0,0};
  
  while ((source = dynamic_cast<TObjString *> (iter.Next()))) {

    AliInfo(Form("found source %s", source->String().Data()));

    TString fileName = GetFile(kDAQ, "BAD_CHANNELS", source->GetName());
    AliInfo(Form("Got filename: %s",fileName.Data()));

    TFile f(fileName);

    if(!f.IsOpen()) {
      Log(Form("File %s is not opened, something goes wrong!",fileName.Data()));
      return kFALSE;
    }
    
    Log(Form("Begin check for bad channels."));
    
    for(Int_t mod=0; mod<5; mod++) {
      for(Int_t iX=0; iX<64; iX++) {
	for(Int_t iZ=0; iZ<56; iZ++) {

	  sprintf(hnam,"%d_%d_%d_%d",mod,iX,iZ,1); // high gain	
	  h1 = (TH1F*)f.Get(hnam);

	  if(h1) {
	    Double_t mean = h1->GetMean();
	    
	    if(mean)
	      Log(Form("iX=%d iZ=%d gain=%d   mean=%.3f\n",iX,iZ,1,mean));

	    if( mean>0 && mean<fQualityCut ) { 
	      nGoods[mod]++; 
	    }
	    else
	      badMap.SetBadChannel(mod+1,iZ+1,iX+1); //module, col,row
	  }

	}
      }

      if(nGoods[mod])
	Log(Form("Module %d: %d good channels.",mod,nGoods[mod]));
    }

    
  } // end of loop over sources
  
  // Store the bad channels map.
  
  AliCDBMetaData md;
  md.SetResponsible("Boris Polishchuk");

  // Data valid from current run fRun until being updated (validityInfinite=kTRUE)
  Bool_t result = Store("Calib", "EmcBadChannels", &badMap, &md, fRun, kTRUE);
  return result;

}

Bool_t AliPHOSPreprocessor::CalibrateEmc()
{
  // I.  Calculates the set of calibration coefficients to equalyze the mean energies deposited at high gain.
  // II. Extracts High_Gain/Low_Gain ratio for each channel.

  // The file fileName contains histograms which have been produced by DA1 detector algorithm.
  // It is a responsibility of the SHUTTLE framework to form the fileName.

  gRandom->SetSeed(0); //the seed is set to the current  machine clock!
  AliPHOSEmcCalibData calibData;
  
  TList* list = GetFileSources(kDAQ, "AMPLITUDES");
  
  if(!list) {
    Log("Sources list not found, exit.");
    return 1;
  }

  if(!list->GetEntries()) {
    Log(Form("Got empty sources list. It seems DA1 did not produce any files!"));
    return kFALSE;
  }

  TIter iter(list);
  TObjString *source;
  
  while ((source = dynamic_cast<TObjString *> (iter.Next()))) {
    AliInfo(Form("found source %s", source->String().Data()));

    TString fileName = GetFile(kDAQ, "AMPLITUDES", source->GetName());
    AliInfo(Form("Got filename: %s",fileName.Data()));

    TFile f(fileName);

    if(!f.IsOpen()) {
      Log(Form("File %s is not opened, something goes wrong!",fileName.Data()));
      return 1;
    }
    
    const Int_t nMod=5; // 1:5 modules
    const Int_t nCol=56; //1:56 columns in each module
    const Int_t nRow=64; //1:64 rows in each module

    Double_t coeff;
    char hnam[80];
    TH2F* h2=0;
    TH1D* h1=0;
    
    //Get the reference histogram
    //(author: Gustavo Conesa Balbastre)

    TList * keylist = f.GetListOfKeys();
    Int_t nkeys   = f.GetNkeys();
    Bool_t ok = kFALSE;
    TKey  *key;
    Int_t ikey = 0;
    Int_t counter = 0;
    TH1D* hRef = 0;

    //Check if the file contains any histogram
    
    if(nkeys< 2){
      Log(Form("Not enough histograms (%d) for calibration.",nkeys));
      return 1;
    }
    
    while(!ok){
      ikey = gRandom->Integer(nkeys);
      key = (TKey*)keylist->At(ikey);
      TObject* obj = f.Get(key->GetName());
      TString cname(obj->ClassName());
      if(cname == "TH2F") {
	h2 = (TH2F*)obj;
	TString htitl = h2->GetTitle();
	if(htitl.Contains("and gain 1")) {
	  hRef = h2->ProjectionX();
	  hRef->SetBins(1000,0.,1000.); // to cut off saturation peak at 1023
	  // Check if the reference histogram has too little statistics
	  if(hRef->GetEntries()>2) ok=kTRUE;
	}
      }
      
      counter++;
      
      if(!ok && counter > nkeys){
	Log("No histogram with enough statistics for reference. Exit.");
	return 1;
      }
    }
    
    Log(Form("reference histogram %s, %.1f entries, mean=%.3f, rms=%.3f.",
	     hRef->GetName(),hRef->GetEntries(),
	     hRef->GetMean(),hRef->GetRMS()));

    Double_t refMean=hRef->GetMean();
    
    // Calculates relative calibration coefficients for all non-zero channels
    
    for(Int_t mod=0; mod<nMod; mod++) {
      for(Int_t col=0; col<nCol; col++) {
	for(Int_t row=0; row<nRow; row++) {

	  //High Gain to Low Gain ratio
	  Float_t ratio = HG2LG(mod,row,col,&f);
	  calibData.SetHighLowRatioEmc(mod+1,col+1,row+1,ratio);
	  
	  sprintf(hnam,"%d_%d_%d_1",mod,row,col); // high gain!
	  h2 = (TH2F*)f.Get(hnam);

	  //TODO: dead channels exclusion!
	  if(h2) {
	    h1 = h2->ProjectionX();
	    h1->SetBins(1000,0.,1000.); // to cut off saturation peak at 1023
	    coeff = h1->GetMean()/refMean;
	    if(coeff>0)
	      calibData.SetADCchannelEmc(mod+1,col+1,row+1,1./coeff);
	    else 
	      calibData.SetADCchannelEmc(mod+1,col+1,row+1,1.);
	    AliInfo(Form("mod %d col %d row %d  coeff %f\n",mod,col,row,coeff));
	  }
	  else
	    calibData.SetADCchannelEmc(mod+1,col+1,row+1,1.); 
	}
      }
    }
    
    f.Close();
  }
  
  //Store EMC calibration data
  
  AliCDBMetaData emcMetaData;
  Bool_t result = Store("Calib", "EmcGainPedestals", &calibData, &emcMetaData, 0, kTRUE); // valid from 0 to infinity);
  return result;

}

     


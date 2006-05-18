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

#include <Riostream.h>

#include <TDirectory.h>

#include "AliRunLoader.h"
#include "AliRun.h"
#include <AliESD.h>
#include "AliLog.h"
#include <TClonesArray.h>
#include "AliSTARTRecPoint.h"
#include "AliRawReader.h"
#include "AliSTARTRawReader.h"
#include "AliSTARTLoader.h"
#include "AliSTARTdigit.h"
#include "AliSTARTReconstructor.h"
#include "AliSTARTParameters.h"
#include "AliCDBLocal.h"
#include "AliCDBStorage.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"

#include <TArrayI.h>
#include <TGraph.h>

ClassImp(AliSTARTReconstructor)

  void  AliSTARTReconstructor::ConvertDigits(AliRawReader* rawReader, TTree* digitsTree) const
{
  //START raw data-> digits conversion
 // reconstruct time information from raw data
  AliSTARTRawReader myrawreader(rawReader,digitsTree);
  // myrawreader.NextThing();
   myrawreader.Next();
   cout<<" AliSTARTReconstructor::ConvertDigits "<< myrawreader.Next()<<endl;
}
  void AliSTARTReconstructor::Reconstruct(TTree*digitsTree, TTree*clustersTree) const
{
// START digits reconstruction
// STARTRecPoint writing 

    //Q->T-> coefficients !!!! should be asked!!!
  //  Float_t ph2MIP=500;
  Float_t gain[24], timeDelayCFD[24], timeDelayLED[24];
  Float_t zdetA,zdetC;
  TObjArray slewingLED;
    
  TArrayI * fADC = new TArrayI(24); 
  TArrayI * fTimeCFD = new TArrayI(24); 
  TArrayI * fADCLED = new TArrayI(24); 
  TArrayI * fTimeLED = new TArrayI(24); 

  AliSTARTParameters* param = AliSTARTParameters::Instance();
  param->Init();

  Int_t mV2Mip = param->GetmV2Mip();     
  //mV2Mip = param->GetmV2Mip();     
  Int_t channelWidth = param->GetChannelWidth() ;  
  
  for (Int_t i=0; i<24; i++){
    timeDelayCFD[i] = param->GetTimeDelayCFD(i);
    timeDelayLED[i] = param->GetTimeDelayLED(i);
    gain[i] = param->GetGain(i);
    slewingLED.AddAtAndExpand(param->GetSlew(i),i);
  }
  zdetC = param->GetZposition(0);
  zdetA  = param->GetZposition(1);
    
  AliDebug(1,Form("Start DIGITS reconstruction "));
 
  TBranch *brDigits=digitsTree->GetBranch("START");
  AliSTARTdigit *fDigits = new AliSTARTdigit();
  if (brDigits) {
    brDigits->SetAddress(&fDigits);
  }else{
    cerr<<"EXEC Branch START digits not found"<<endl;
    return;
  }
  brDigits->GetEntry(0);
  fDigits->GetTime(*fTimeCFD);
  fDigits->GetADC(*fADC);
  fDigits->GetTimeAmp(*fTimeLED);
  fDigits->GetADCAmp(*fADCLED);

  Float_t besttimeright=999999;
  Float_t besttimeleft=999999;
  Int_t pmtBestRight=99999;
  Int_t pmtBestLeft=99999;
  Float_t timeDiff=999999, meanTime=0;
  
  AliSTARTRecPoint* frecpoints= new AliSTARTRecPoint ();
  clustersTree->Branch( "START", "AliSTARTRecPoint" ,&frecpoints, 405,1);

  Float_t time[24], adc[24];
  for (Int_t ipmt=0; ipmt<24; ipmt++) {
      
    if(fTimeCFD->At(ipmt)>0 ){
      time[ipmt] = channelWidth *( fTimeCFD->At(ipmt)) - 1000*timeDelayCFD[ipmt];
      Float_t adc_digPs = channelWidth * Float_t (fADC->At(ipmt)) ;
      adc[ipmt] = TMath::Exp(adc_digPs/1000) /gain[ipmt];
      AliDebug(1,Form(" time %f ps,  adc %f mv in MIP %i\n ",
		      time[ipmt], adc[ipmt], Int_t (adc[ipmt]/mV2Mip +0.5)));
      frecpoints->SetTime(ipmt,time[ipmt]);
      frecpoints->SetAmp(ipmt,adc[ipmt]);
    }
    else {
      time[ipmt] = 0;
      adc[ipmt] = 0;
    }
  }
  for (Int_t ipmt=0; ipmt<12; ipmt++){
    if(time[ipmt] > 1 ) {
      if(time[ipmt]<besttimeleft){
	besttimeleft=time[ipmt]; //timeleft
	pmtBestLeft=ipmt;
      }
    }
  }
  for ( Int_t ipmt=12; ipmt<24; ipmt++){
    if(time[ipmt] > 1) {
      if(time[ipmt]<besttimeright) {
	besttimeright=time[ipmt]; //timeright
        pmtBestRight=ipmt;}
    }
  }
  if(besttimeright !=999999)  frecpoints->SetTimeBestRight(Int_t(besttimeright));
  if( besttimeleft != 999999 ) frecpoints->SetTimeBestLeft(Int_t(besttimeleft));
  AliDebug(1,Form(" besttimeright %f ps,  besttimeleft %f ps",besttimeright, besttimeleft));
  Float_t c = 0.0299792; // cm/ps
  Float_t vertex = 0;
  if(besttimeright !=999999 && besttimeleft != 999999 ){
    timeDiff = besttimeright - besttimeleft;
    meanTime = (besttimeright + besttimeleft)/2.;
    vertex = c*(timeDiff); //-(lenr-lenl))/2;
    AliDebug(1,Form("  timeDiff %f ps,  meanTime %f ps, vertex %f cm",timeDiff, meanTime,vertex ));
    frecpoints->SetVertex(vertex);
    frecpoints->SetMeanTime(Int_t(meanTime));
    
  }
  clustersTree->Fill();
}


void AliSTARTReconstructor::FillESD(AliRunLoader* runLoader, AliESD *pESD) const
{

  /***************************************************
  Resonstruct digits to vertex position
  ****************************************************/
  
  
  if (!runLoader) {
    AliError("Reconstruct >> No run loader");
    return;
  }
  
  AliDebug(1,Form("Start FillESD START"));

  AliSTARTLoader* pStartLoader = (AliSTARTLoader*) runLoader->GetLoader("STARTLoader");
 
  pStartLoader->LoadRecPoints("READ");

  TTree *treeR = pStartLoader->TreeR();
  
   AliSTARTRecPoint* frecpoints= new AliSTARTRecPoint ();
    if (!frecpoints) {
    AliError("Reconstruct Fill ESD >> no recpoints found");
    return;
  }
  
  AliDebug(1,Form("Start FillESD START"));
  TBranch *brRec = treeR->GetBranch("START");
  if (brRec) {
    brRec->SetAddress(&frecpoints);
  }else{
    cerr<<"EXEC Branch START rec not found"<<endl;
    // exit(111);
    return;
  } 
    
    brRec->GetEntry(0);
    Float_t timeStart, Zposition, amp[24], time[24];
    Int_t i;
    Zposition = frecpoints -> GetVertex();
    timeStart = frecpoints -> GetMeanTime();
    for ( i=0; i<24; i++) {
       time[i] = Float_t (frecpoints -> GetTime(i)) / 1000.; // ps to ns
      amp[i] = frecpoints -> GetAmp(i);
    }
    pESD->SetT0zVertex(Zposition); //vertex Z position 
    /*    
    pESD->SetT0(timeStart);        // interaction time 
    pESD->SetT0time(*time);        // best TOF on each PMT 
    pESD->SetT0amplitude(*amp);    // number of particles(MIPs) on each PMT
    */    
    //    pESD->Dump();
    pStartLoader->UnloadRecPoints();
   
} // vertex in 3 sigma







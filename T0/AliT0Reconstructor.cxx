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
/*********************************************************************
 *  T0 reconstruction and filling ESD
 *  - reconstruct mean time (interation time) 
 *  - vertex position
 *  -  multiplicity
 ********************************************************************/

#include <AliESDEvent.h>
#include "AliLog.h"
#include "AliT0RecPoint.h"
#include "AliRawReader.h"
#include "AliT0RawReader.h"
#include "AliT0digit.h"
#include "AliT0Reconstructor.h"
#include "AliT0Parameters.h"
#include "AliT0Calibrator.h"
#include "AliESDfriend.h"
#include "AliESDTZERO.h"
#include "AliESDTZEROfriend.h"
#include "AliLog.h"
#include "AliCDBEntry.h" 
#include "AliCDBManager.h"
#include "AliCTPTimeParams.h"
#include "AliLHCClockPhase.h"
#include "AliT0CalibSeasonTimeShift.h"
#include "AliESDRun.h"

#include <TArrayI.h>
#include <TGraph.h>
#include <TMath.h>
#include <Riostream.h>

ClassImp(AliT0Reconstructor)

  AliT0Reconstructor:: AliT0Reconstructor(): AliReconstructor(),
					     fdZonA(0),
					     fdZonC(0),
					     fZposition(0),
					     fParam(NULL),
					     fAmpLEDrec(),
					     fQTC(0),
					     fAmpLED(0),
                                             fCalib(),
                                             fLatencyHPTDC(9000),
                                             fLatencyL1(0),
                                             fLatencyL1A(0),
                                             fLatencyL1C(0),
					     fGRPdelays(0),
					     fTimeMeanShift(0x0),
					     fTimeSigmaShift(0x0),
                                             fESDTZEROfriend(NULL),
                                             fESDTZERO(NULL),
					     fIsCDFfromGRP(kFALSE)

{
  for (Int_t i=0; i<24; i++)  fTime0vertex[i] =0;

  //constructor
  AliCDBEntry *entry = AliCDBManager::Instance()->Get("GRP/CTP/CTPtiming");
  if (!entry) AliFatal("CTP timing parameters are not found in OCDB !");
  AliCTPTimeParams *ctpParams = (AliCTPTimeParams*)entry->GetObject();
  Float_t l1Delay = (Float_t)ctpParams->GetDelayL1L0()*25.0;

  AliCDBEntry *entry1 = AliCDBManager::Instance()->Get("GRP/CTP/TimeAlign");
  if (!entry1) AliFatal("CTP time-alignment is not found in OCDB !");
  AliCTPTimeParams *ctpTimeAlign = (AliCTPTimeParams*)entry1->GetObject();
  l1Delay += ((Float_t)ctpTimeAlign->GetDelayL1L0()*25.0);
 
  AliCDBEntry *entry4 = AliCDBManager::Instance()->Get("GRP/Calib/LHCClockPhase");
  if (!entry4) AliFatal("LHC clock-phase shift is not found in OCDB !");
  AliLHCClockPhase *phase = (AliLHCClockPhase*)entry4->GetObject();
  
  fGRPdelays = l1Delay - phase->GetMeanPhase();
  
  AliCDBEntry *entry5 = AliCDBManager::Instance()->Get("T0/Calib/TimeAdjust");
  if (entry5) {
    AliT0CalibSeasonTimeShift *timeshift = (AliT0CalibSeasonTimeShift*)entry5->GetObject();
    fTimeMeanShift = timeshift->GetT0Means();
    fTimeSigmaShift  = timeshift->GetT0Sigmas();
  }
  else
    AliWarning("Time Adjust is not found in OCDB !");
 
  fParam = AliT0Parameters::Instance();
  fParam->Init();
 
  for (Int_t i=0; i<24; i++){
        TGraph* gr = fParam ->GetAmpLEDRec(i);
	if (gr) fAmpLEDrec.AddAtAndExpand(gr,i) ; 
	  TGraph* gr1 = fParam ->GetAmpLED(i);
	  if (gr1) fAmpLED.AddAtAndExpand(gr1,i) ; 
	  TGraph* gr2 = fParam ->GetQTC(i);
	  if (gr2) fQTC.AddAtAndExpand(gr2,i) ; 	
	  fTime0vertex[i] = fParam->GetCFD(i);
 }
  fLatencyL1 = fParam->GetLatencyL1();
  fLatencyL1A = fParam->GetLatencyL1A(); 
  fLatencyL1C = fParam->GetLatencyL1C();
  fLatencyHPTDC = fParam->GetLatencyHPTDC();
  AliDebug(2,Form(" LatencyL1 %f latencyL1A %f latencyL1C %f latencyHPTDC %f \n",fLatencyL1, fLatencyL1A, fLatencyL1C, fLatencyHPTDC));
 
  for (Int_t i=0; i<24; i++) {
     if( fTime0vertex[i] < 500 || fTime0vertex[i] > 60000)
       { fTime0vertex[i] =( 1000.*fLatencyHPTDC - 1000.*fLatencyL1 + 1000.*fGRPdelays)/24.4;
	 fIsCDFfromGRP=kTRUE;
       }
     AliDebug(2,Form("OCDB mean CFD time %i %f \n",i, fTime0vertex[i]));
  }
  //here real Z position
  fdZonC = TMath::Abs(fParam->GetZPosition("T0/C/PMT1"));
  fdZonA = TMath::Abs(fParam->GetZPosition("T0/A/PMT15"));
  
  fCalib = new AliT0Calibrator();
  fESDTZEROfriend = new AliESDTZEROfriend();
  fESDTZERO  = new AliESDTZERO();
  
 
}

//_____________________________________________________________________________
void AliT0Reconstructor::Reconstruct(TTree*digitsTree, TTree*clustersTree) const
{
  // T0 digits reconstruction
  Int_t refAmp = 0 ; /*Int_t (GetRecoParam()->GetRefAmp());*/
  
  TArrayI * timeCFD = new TArrayI(24); 
  TArrayI * timeLED = new TArrayI(24); 
  TArrayI * chargeQT0 = new TArrayI(24); 
  TArrayI * chargeQT1 = new TArrayI(24); 

 
  Float_t c = 29.9792458; // cm/ns
  Float_t channelWidth = fParam->GetChannelWidth() ;  
  Double32_t vertex = 9999999, meanVertex = 0 ;
  Double32_t timeDiff=999999, meanTime=999999, timeclock=999999;
  
  
  AliDebug(1,Form("Start DIGITS reconstruction "));
  
  Float_t lowAmpThreshold =  GetRecoParam()->GetAmpLowThreshold();  
  Float_t highAmpThreshold =  GetRecoParam()->GetAmpHighThreshold(); 
  printf("Reconstruct(TTree*digitsTree highAmpThreshold %f  lowAmpThreshold %f \n",lowAmpThreshold, highAmpThreshold);

  //shift T0A, T0C , T0AC
  Float_t shiftA = GetRecoParam() -> GetLow(310);  
  Float_t shiftC = GetRecoParam() -> GetLow(311);  
  Float_t shiftAC = GetRecoParam() -> GetLow(312);
  AliDebug(2, Form("Reconstruct(TTree*digitsTree shiftA %f shiftC %f shiftAC %f \n",shiftA, shiftC, shiftAC));
  
  Double32_t besttimeA=9999999;  Double32_t besttimeA_best=9999999;
  Double32_t besttimeC=9999999;  Double32_t besttimeC_best=9999999;
  Int_t timeDelayCFD[24]; 
  Int_t badpmt[24];
  //Bad channel
  for (Int_t i=0; i<24; i++) {
    badpmt[i] = GetRecoParam() -> GetBadChannels(i);
    timeDelayCFD[i] =  Int_t (fParam->GetTimeDelayCFD(i));
  }
  fCalib->SetEq(0);
  TBranch *brDigits=digitsTree->GetBranch("T0");
  AliT0digit *fDigits = new AliT0digit() ;
  if (brDigits) {
    brDigits->SetAddress(&fDigits);
  }else{
    AliError(Form("EXEC Branch T0 digits not found"));
    return;
  }
  
  digitsTree->GetEvent(0);
  digitsTree->GetEntry(0);
  brDigits->GetEntry(0);
  fDigits->GetTimeCFD(*timeCFD);
  fDigits->GetTimeLED(*timeLED);
  fDigits->GetQT0(*chargeQT0);
  fDigits->GetQT1(*chargeQT1);
  Int_t onlineMean =  fDigits->MeanTime();
  
  Bool_t tr[5];
  for (Int_t i=0; i<5; i++) tr[i]=false; 
  
  
  AliT0RecPoint frecpoints;
  AliT0RecPoint * pfrecpoints = &frecpoints;
  clustersTree->Branch( "T0", "AliT0RecPoint" ,&pfrecpoints);
  
  Float_t time[24], adc[24], adcmip[24];
  for (Int_t ipmt=0; ipmt<24; ipmt++) {
    if(timeCFD->At(ipmt)>0 ) {
      Float_t timefull = 0.001*( timeCFD->At(ipmt) - 511 - timeDelayCFD[ipmt])  * channelWidth;
      frecpoints.SetTimeFull(ipmt, 0 ,timefull) ;
      if(( chargeQT1->At(ipmt) - chargeQT0->At(ipmt))>0)  
	adc[ipmt] = chargeQT1->At(ipmt) - chargeQT0->At(ipmt);
      else
	adc[ipmt] = 0;
      
      time[ipmt] = fCalib-> WalkCorrection(refAmp, ipmt, Int_t(adc[ipmt]),  timeCFD->At(ipmt)) ;
      time[ipmt] =   time[ipmt] - 511;   
      Double_t sl = Double_t(timeLED->At(ipmt) - timeCFD->At(ipmt));
      //    time[ipmt] = fCalib-> WalkCorrection( refAmp,ipmt, Int_t(sl),  timeCFD->At(ipmt) ) ;
      AliDebug(5,Form(" ipmt %i QTC  %i , time in chann %i (led-cfd) %i ",
		      ipmt, Int_t(adc[ipmt]) ,Int_t(time[ipmt]),Int_t( sl)));
      
      Double_t ampMip = 0;
      TGraph* ampGraph = (TGraph*)fAmpLED.At(ipmt);
      if (ampGraph) ampMip = ampGraph->Eval(sl);
      Double_t qtMip = 0;
      TGraph* qtGraph = (TGraph*)fQTC.At(ipmt);
      if (qtGraph) qtMip = qtGraph->Eval(adc[ipmt]);
      AliDebug(5,Form("  Amlitude in MIPS LED %f ,  QTC %f in channels %f\n ",ampMip,qtMip, adc[ipmt]));
      frecpoints.SetTime(ipmt, Float_t(time[ipmt]) );
      frecpoints.SetAmpLED(ipmt, Float_t( ampMip)); 
      frecpoints.SetAmp(ipmt, Float_t(qtMip));
      adcmip[ipmt]=qtMip;
      
    }
    else {
      time[ipmt] = -99999;
      adc[ipmt] = 0;
      adcmip[ipmt] = 0;
      
    }
  }
  
  for (Int_t ipmt=0; ipmt<12; ipmt++){
    if(time[ipmt] !=0 &&  time[ipmt] > 0 
       &&  adcmip[ipmt]>lowAmpThreshold && adcmip[ipmt]<highAmpThreshold )
      {
	if(time[ipmt]<besttimeC) besttimeC=time[ipmt]; //timeC
	if(TMath::Abs(time[ipmt])<TMath::Abs(besttimeC_best)) 
	  besttimeC_best=time[ipmt]; //timeC	     
      }
  }
  for ( Int_t ipmt=12; ipmt<24; ipmt++)
    {
      if(time[ipmt] != 0 &&  time[ipmt] > 0 
	 && adcmip[ipmt]>lowAmpThreshold && adcmip[ipmt]<highAmpThreshold)
	{
	  if(time[ipmt]<besttimeA) besttimeA=time[ipmt]; 
	  if(TMath::Abs(time[ipmt] ) < TMath::Abs(besttimeA_best)) 
	    besttimeA_best=time[ipmt]; //timeA
	}
    }
  
  if( besttimeA < 999999 && besttimeA!=0) {
    frecpoints.SetTimeBestA((besttimeA_best * channelWidth  - fdZonA/c)  );
    frecpoints.SetTime1stA((besttimeA * channelWidth  - fdZonA/c - shiftA) );
    tr[1]=true;
  }
  
  if( besttimeC < 999999 && besttimeC!=0) {
    frecpoints.SetTimeBestC((besttimeC_best * channelWidth  - fdZonC/c) );
    frecpoints.SetTime1stC((besttimeC * channelWidth  - fdZonC/c - shiftC) );
    tr[2]=true;
  }
  
  AliDebug(5,Form("1stimeA %f , besttimeA %f 1sttimeC %f besttimeC %f ",
		  besttimeA, besttimeA_best,
		  besttimeC, besttimeC_best) );

  if(besttimeA <999999 && besttimeC < 999999 ){
    //    timeDiff = (besttimeC - besttimeA)*channelWidth;
    timeDiff = (besttimeA - besttimeC)*channelWidth;
    meanTime = channelWidth * (besttimeA_best + besttimeC_best)/2. ; 
    timeclock = channelWidth * (besttimeA + besttimeC)/2. - shiftAC ;
    vertex = meanVertex - 0.001* c*(timeDiff)/2.;// + (fdZonA - fdZonC)/2;
    tr[0]=true; 
  }
  frecpoints.SetVertex(vertex);
  frecpoints.SetMeanTime(meanTime );
  frecpoints.SetT0clock(timeclock );
  frecpoints.SetT0Trig(tr);
  
 AliDebug(5,Form("fRecPoints:::  1stimeA %f , besttimeA %f 1sttimeC %f besttimeC %f vertex %f",
		  frecpoints.Get1stTimeA(),  frecpoints.GetBestTimeA(),
		  frecpoints.Get1stTimeC(),  frecpoints.GetBestTimeC(), 
		  vertex ) );
  
  AliDebug(5,Form("T0 triggers %d %d %d %d %d",tr[0],tr[1],tr[2],tr[3],tr[4]));
  
  //online mean
  frecpoints.SetOnlineMean(Int_t(onlineMean));
  AliDebug(10,Form("  timeDiff %f #channel,  meanTime %f #channel, vertex %f cm online mean %i timeclock %f ps",timeDiff, meanTime,vertex, Int_t(onlineMean), timeclock));
  
  clustersTree->Fill();
  
  delete timeCFD;
  delete timeLED;
  delete chargeQT0; 
  delete chargeQT1; 
}


//_______________________________________________________________________

void AliT0Reconstructor::Reconstruct(AliRawReader* rawReader, TTree*recTree) const
{
  // T0 raw ->
  //
  
  Float_t meanOrA = fTime0vertex[0] + 587;
  Float_t meanOrC = fTime0vertex[0] + 678;
  Float_t meanTVDC = fTime0vertex[0] + 2564;
  Float_t meanQT1 = fTime0vertex[0] + 2564;
  Float_t meanQT0 = fTime0vertex[0] + 3564;
  
  Int_t timeDelayCFD[24]; 
  Int_t corridor = GetRecoParam() -> GetCorridor();  
  if(fIsCDFfromGRP) corridor *=5;
  AliDebug(10,Form("fIsCDFfromGRP %i  corridor %i \n",fIsCDFfromGRP, corridor) );
  Int_t badpmt[24];
  //Bad channel
  for (Int_t i=0; i<24; i++) {
    badpmt[i] = GetRecoParam() -> GetBadChannels(i);
    timeDelayCFD[i] =  Int_t (fParam->GetTimeDelayCFD(i));
  }
  Int_t badLEDminCFD = GetRecoParam() -> GetHigh(350);
  Int_t equalize = GetRecoParam() -> GetEq();
  AliDebug(10,Form( "AliT0Reconstructor::Reconstruct::: RecoParam %i LEDminCFD threshold %i \n",equalize, badLEDminCFD) );
  fCalib->SetEq(equalize);
  Int_t low[500], high[500];
  Float_t timefull=-99999;;
  Float_t tvdc  = -99999; Float_t ora = -99999; Float_t orc = -99999;
  
  Int_t allData[110][5];
  
  Int_t timeCFD[24], timeLED[24], chargeQT0[24], chargeQT1[24];
  Float_t time2zero[24]; 
  Double32_t timeDiff, meanTime, timeclock;
  timeDiff =  meanTime = timeclock = 9999999;
  Float_t c = 29.9792458; // cm/ns
  Double32_t vertex = 9999999;
  Int_t onlineMean=0;
  Float_t meanVertex = 0;
  Int_t pedestal[24];
  for (Int_t i0=0; i0<24; i0++) {
    low[i0] = Int_t(fTime0vertex[i0]) - corridor;
    high[i0] = Int_t(fTime0vertex[i0]) + corridor;
    time2zero[i0] = 99999;
    pedestal[i0]=Int_t (GetRecoParam()->GetLow(100+i0) );
  }
  
  for (Int_t i0=0; i0<110; i0++)
    for (Int_t j0=0; j0<5; j0++)  allData[i0][j0]=0; 
  
  Float_t lowAmpThreshold =  GetRecoParam()->GetAmpLowThreshold();  
  Float_t highAmpThreshold =  GetRecoParam()->GetAmpHighThreshold(); 
  
  Double32_t besttimeA=9999999;  Double32_t besttimeA_best=9999999;
  Double32_t besttimeC=9999999;  Double32_t besttimeC_best=9999999;

   Float_t channelWidth = fParam->GetChannelWidth() ;  
	
  AliT0RecPoint frecpoints;
  AliT0RecPoint * pfrecpoints = &frecpoints;
  
  recTree->Branch( "T0", "AliT0RecPoint" ,&pfrecpoints);
   
  AliDebug(10," before read data ");
  AliT0RawReader myrawreader(rawReader);

  UInt_t type =rawReader->GetType();

  if (!myrawreader.Next())
    AliDebug(1,Form(" no raw data found!!"));
  else
    {  
   for (Int_t i=0; i<24; i++)
    {
      timeCFD[i]=0; timeLED[i]=0; chargeQT0[i]=0; chargeQT1[i]=0;
    }
  
      if(type == 7  ) {  //only physics 
	for (Int_t i=0; i<107; i++) {
	for (Int_t iHit=0; iHit<5; iHit++) 
	  {
	    allData[i][iHit] = myrawreader.GetData(i,iHit);
	  }
	}
	
	Int_t fBCID=Int_t (rawReader->GetBCID());
	Int_t trmbunch= myrawreader.GetTRMBunchID();
	AliDebug(10,Form(" CDH BC ID %i, TRM BC ID %i \n", fBCID, trmbunch ));
	if( (trmbunch-fBCID)!=37  ) {
	  AliDebug(0,Form("wrong :::: CDH BC ID %i, TRM BC ID %i \n", fBCID, trmbunch ));
	  //	type = -1;
	}
	for (Int_t in=0; in<12; in++)  
	  {
	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
		if(allData[in+1][iHit] > low[in] && 
		   allData[in+1][iHit] < high[in])
		  {
		    timeCFD[in] = allData[in+1][iHit] ; 
 		    break;
		  }
	      }
	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
		if(allData[in+1+56][iHit] > low[in+12] && 
		   allData[in+1+56][iHit] < high[in+12])
		  {
		    timeCFD[in+12] = allData[in+56+1][iHit] ;
		    break;
		  }
	      }
	    timeLED[in+12] = allData[in+68+1][0] ;
	    timeLED[in] = allData[in+12+1][0] ;
	    AliDebug(50, Form(" readed i %i cfdC %i cfdA %i ledC %i ledA%i ",
			      in, timeCFD[in],timeCFD[in+12],timeLED[in], 
			     timeLED[in+12]));   
	    
	  }
	
	for (Int_t in=0; in<12;  in++)
	  {
	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
                if (allData[2*in+26][iHit] > fTime0vertex[0]+2000 &&  
		    allData[2*in+26][iHit] <fTime0vertex[0]+3000 ) {
		    chargeQT1[in]=allData[2*in+26][0];
		    break;
		}
	      }

	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
		if( (allData[2*in+25][iHit] -  chargeQT1[in])>800 
		    &&  chargeQT1[in]>0)		
		  {
		    chargeQT0[in]=allData[2*in+25][0];
		    AliDebug(25, Form(" readed Raw %i %i %i",
				      in, chargeQT0[in],chargeQT1[in]));
		    break;
		  }
		}
	  }	
	for (Int_t in=12; in<24;  in++)
	  {
     	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
		if (allData[2*in+58][iHit] > fTime0vertex[0]+2000 &&  
		    allData[2*in+58][iHit] <fTime0vertex[0]+3000 )
		  {
		    chargeQT1[in]=allData[2*in+58][0];
		    break;
		  }
	      }
    	    for (Int_t iHit=0; iHit<5; iHit++) 
	      {
		if( (allData[2*in+57][iHit] -  chargeQT1[in])>800 && 
		    chargeQT1[in]>0 )
		  {
		    chargeQT0[in]=allData[2*in+57][0];
		    AliDebug(25, Form(" readed Raw %i %i %i",
				      in, chargeQT0[in],chargeQT1[in]));
		    break;
		  }
	      }
	  }
	
	onlineMean = allData[49][0];
	
	Double32_t time[24], adc[24], adcmip[24], noncalibtime[24];
	for (Int_t ipmt=0; ipmt<24; ipmt++) {
	  if(timeCFD[ipmt] >  0 && (chargeQT0[ipmt] - chargeQT1[ipmt])> 0 &&
	  (timeLED[ipmt]-timeCFD[ipmt])<badLEDminCFD){
	   //for simulated data
	     //for physics  data
	   adc[ipmt] = chargeQT0[ipmt] - chargeQT1[ipmt];
	   Int_t refAmp = Int_t (fTime0vertex[ipmt]);
	   time[ipmt] = fCalib-> WalkCorrection( refAmp, ipmt, Int_t(adc[ipmt]), timeCFD[ipmt] ) ;
      	   Double_t sl = timeLED[ipmt] - timeCFD[ipmt];
	   // time[ipmt] = fCalib-> WalkCorrection( refAmp,ipmt, Int_t(sl), timeCFD[ipmt] ) ;
	   AliDebug(5,Form(" ipmt %i QTC %i , time in chann %i (led-cfd) %i ",
			    ipmt, Int_t(adc[ipmt]) ,Int_t(time[ipmt]),Int_t( sl)));
	   Double_t ampMip = 0;
	   TGraph * ampGraph =  (TGraph*)fAmpLED.At(ipmt);
	   if (ampGraph) ampMip = ampGraph->Eval(sl);
	   Double_t qtMip = 0;
	   TGraph * qtGraph = (TGraph*)fQTC.At(ipmt);
	   if (qtGraph) qtMip = qtGraph->Eval(adc[ipmt]);
	   AliDebug(10,Form("  Amlitude in MIPS LED %f ; QTC %f;  in channels %f\n ",ampMip,qtMip, adc[ipmt]));
	   if( equalize  ==0 ) 
	     frecpoints.SetTime(ipmt, Float_t(time[ipmt]) );
	   else 
	     frecpoints.SetTime(ipmt, Float_t(time[ipmt] + fTime0vertex[ipmt]) );
	   // frecpoints.SetTime(ipmt, Float_t(time[ipmt] ) );
	   if(qtMip<0) qtMip=0;
	   frecpoints.SetAmp(ipmt, Double32_t( qtMip)); 
	   adcmip[ipmt]=qtMip;
	   frecpoints.SetAmpLED(ipmt, Double32_t(ampMip));	     
	   noncalibtime[ipmt]= Double32_t (timeCFD[ipmt]);
	  }
	  else {
	   time[ipmt] = -9999;
	   adc[ipmt] = 0;
	   adcmip[ipmt] = 0;
	   noncalibtime[ipmt] = -9999;
	  }
       }
       fESDTZEROfriend->SetT0timeCorr(noncalibtime) ; 
    
       for (Int_t ipmt=0; ipmt<12; ipmt++){
	 if(time[ipmt] !=0 &&  time[ipmt] > -9000 
	    /*&& badpmt[ipmt]==0 */
	    &&  adcmip[ipmt]>lowAmpThreshold )
	   {
	     if(time[ipmt]<besttimeC) besttimeC=time[ipmt]; //timeC
	     if(TMath::Abs(time[ipmt])<TMath::Abs(besttimeC_best)) 
	       besttimeC_best=time[ipmt]; //timeC	     
	   }
       }
       for ( Int_t ipmt=12; ipmt<24; ipmt++)
	 {
	   if(time[ipmt] != 0 &&  time[ipmt] > -9000 
	      /* && badpmt[ipmt]==0*/ 
	      && adcmip[ipmt]>lowAmpThreshold )
	     {
	       if(time[ipmt]<besttimeA) besttimeA=time[ipmt]; 
	       if(TMath::Abs(time[ipmt] ) < TMath::Abs(besttimeA_best)) 
		 besttimeA_best=time[ipmt]; //timeA
	     }
	 }
       
       if(besttimeA < 999999 && besttimeA!=0 ) {
	 if( equalize  ==0 ) 
	   frecpoints.SetTime1stA((besttimeA * channelWidth)- 1000.*fLatencyHPTDC + 1000.*fLatencyL1A - 1000.*fGRPdelays - fTimeMeanShift[1] ); 
	 else
	   {
	     frecpoints.SetTimeBestA((besttimeA_best * channelWidth )); 
	     frecpoints.SetTime1stA((besttimeA * channelWidth - fTimeMeanShift[1])); 
	   }
       }
       if( besttimeC < 999999 && besttimeC!=0) {
	 if( equalize  ==0 ) 
	   frecpoints.SetTime1stC((besttimeC * channelWidth)- 1000.*fLatencyHPTDC +1000.*fLatencyL1C - 1000.*fGRPdelays - fTimeMeanShift[2]);
	 else
	   {
	     frecpoints.SetTimeBestC((besttimeC_best * channelWidth ));
	     frecpoints.SetTime1stC((besttimeC * channelWidth - fTimeMeanShift[2]));
	   }
       }
       AliDebug(5,Form("1stimeA %f , besttimeA %f 1sttimeC %f besttimeC %f ",
		       besttimeA, besttimeA_best,
		       besttimeC, besttimeC_best) );
       AliDebug(5,Form("fRecPoints:::  1stimeA %f , besttimeA %f 1sttimeC %f besttimeC %f shiftA %f shiftC %f ",
		       frecpoints.Get1stTimeA(),  frecpoints.GetBestTimeA(),
		       frecpoints.Get1stTimeC(),  frecpoints.GetBestTimeC(), 
		       fTimeMeanShift[1],fTimeMeanShift[2] ) );
       if( besttimeC < 999999 &&  besttimeA < 999999) { 
	 if(equalize  ==0 )
	   timeclock = (channelWidth*(besttimeC + besttimeA)/2.- 1000.*fLatencyHPTDC +1000.*fLatencyL1 - 1000.*fGRPdelays - fTimeMeanShift[0]);
	 else
	   {
	     timeclock = channelWidth * Float_t( besttimeA+besttimeC)/2. - fTimeMeanShift[0];
	     meanTime = channelWidth * Float_t(besttimeA_best + besttimeC_best )/2.;
	   }
	 timeDiff = ( besttimeA - besttimeC)* 0.001* channelWidth ;
	 vertex =  meanVertex - c*(timeDiff)/2. ; //+ (fdZonA - fdZonC)/2; 
       }
       
      }  //if phys event       
       AliDebug(1,Form("  timeDiff %f #channel,  meanTime %f #ps, TOFmean%f  vertex %f cm meanVertex %f  \n",timeDiff, meanTime,timeclock, vertex,meanVertex));
       frecpoints.SetT0clock(timeclock);
       frecpoints.SetVertex(vertex);
       frecpoints.SetMeanTime(meanTime);
       frecpoints.SetOnlineMean(Int_t(onlineMean));
      
      // Set triggers
      Bool_t tr[5];
      Int_t trchan[5] = {50,51,52,55,56};
      Float_t lowtr[5] = {meanTVDC-700, meanOrA-700, meanOrC-700, meanOrC-1000, meanOrC-1000 };
      Float_t hightr[5] = {meanTVDC+700, meanOrA+700, meanOrC+700, meanOrC+1000, meanOrC+1000};
      
      for (Int_t i=0; i<5; i++) tr[i] = false; 
      for (Int_t itr=0; itr<5; itr++) {
 	for (Int_t iHit=0; iHit<1; iHit++) 
	  {
	    Int_t trr=trchan[itr];
	    if( allData[trr][iHit] > lowtr[itr] && allData[trr][iHit] < hightr[itr])  tr[itr]=true;
	    
	    AliDebug(15,Form("Reconstruct :::  T0 triggers iHit %i tvdc %d orA %d orC %d centr %d semicentral %d",iHit, tr[0],tr[1],tr[2],tr[3],tr[4]));
	  }	  
      }
      frecpoints.SetT0Trig(tr);
      
      // all times with amplitude correction 
      Float_t timecent;
      for (Int_t iHit=0; iHit<5; iHit++) 
	{
	  timefull = timecent = -9999; 
	  tvdc = ora = orc = -9999;
	  if(allData[50][iHit]>0) 
	    tvdc = (Float_t(allData[50][iHit]) - meanTVDC) * channelWidth* 0.001; 
	  if(allData[51][iHit]>0)
	    ora = (Float_t(allData[51][iHit]) - meanOrA) * channelWidth* 0.001;
	  
	  if(allData[52][iHit]>0) 
	    orc = (Float_t(allData[52][iHit]) - meanOrC) * channelWidth* 0.001;
	  
	  frecpoints.SetOrC( iHit, orc);
	  frecpoints.SetOrA( iHit, ora);
	  frecpoints.SetTVDC( iHit, tvdc);
	  for (Int_t i0=0; i0<12; i0++) {
	    if (equalize  ==0 )  
	      timecent = fTime0vertex[i0] + timeDelayCFD[i0];
	    else
	      timecent = fTime0vertex[i0];
	    timefull = -9999; 
	    if(allData[i0+1][iHit]>1) 
	      timefull = (Float_t(allData[i0+1][iHit]) - timecent)* channelWidth* 0.001;
	    frecpoints.SetTimeFull(i0, iHit,timefull) ;
	    //	    if(allData[i0+1][iHit]>1)  printf("i0 %d iHit %d data %d fTime0vertex %f timefull %f \n",i0, iHit, allData[i0+1][iHit], fTime0vertex[i0], timefull);
	    
	  }
	  
	  for (Int_t i0=12; i0<24; i0++) {
	    timefull = -9999; 
	    if (equalize  ==0 )  
	      timecent = fTime0vertex[i0] + timeDelayCFD[i0];
	    else
	      timecent = fTime0vertex[i0];
	    if(allData[i0+45][iHit]>1) {
	      timefull = (Float_t(allData[i0+45][iHit]) - timecent)* channelWidth* 0.001;
	    }
	    //  if(allData[i0+45][iHit]>1)  printf("i0 %d iHit %d data %d fTime0vertex %f timefull %f \n",i0, iHit, allData[i0+45][iHit], fTime0vertex[i0], timefull);
	    frecpoints.SetTimeFull(i0, iHit, timefull) ;
	  }
	}
      
      
      //Set MPD
      Float_t mpda_start=0, mpdc_start=0;
      for (Int_t iHit=0; iHit<5; iHit++) 
	{
	  if (allData[54][iHit] > fTime0vertex[0]+2000 &&  
	      allData[54][iHit] <fTime0vertex[0]+3000 ) {
	    mpda_start=Float_t(allData[54][iHit]);
	    AliDebug(15,Form("Reconstruct :::  T0 MPD iHit %i MPDA start %f",iHit, mpda_start ));

	    break;
	  }
	}
      for (Int_t iHit=0; iHit<5; iHit++) 
	{
	  if(allData[53][iHit] > fTime0vertex[0]+3000 && 
	     (allData[53][iHit]-mpda_start)>800 && mpda_start>0 && tr[1]) {
	    frecpoints.SetMultA(Float_t(allData[53][iHit] - mpda_start) );
	    AliDebug(15,Form("Reconstruct :::  T0 MPD iHit %i MPDA stop %i MPD %f",iHit, allData[53][iHit],Float_t(allData[53][iHit] - mpda_start) ));
	    break;
	  }
	}
      for (Int_t iHit=0; iHit<5; iHit++) 
	{
	  if (allData[106][iHit] > fTime0vertex[0]+2000 &&  
	      allData[106][iHit] <fTime0vertex[0]+3000 ) {
	    mpdc_start=allData[54][iHit];
	    AliDebug(15,Form("Reconstruct :::  T0 MPD iHit %i MPDC start %f ",iHit, mpdc_start));
	    break;
	  }
	}
      for (Int_t iHit=0; iHit<5; iHit++) 
	{
	  if(allData[105][iHit] > fTime0vertex[0]+3000 && 
	     (allData[105][iHit]-mpdc_start)>800 && mpdc_start>0 && tr[2]) {
	    frecpoints.SetMultC(Float_t(allData[105][iHit] - mpdc_start) );
	    AliDebug(15,Form("Reconstruct :::  T0 MPD iHit %i MPDC stop %i MPD %f",iHit, allData[105][iHit],Float_t(allData[105][iHit] - mpdc_start) ));
	    break;
	  }
	}
    } // if (else )raw data
  recTree->Fill();
}
  
  
//____________________________________________________________
  
  void AliT0Reconstructor::FillESD(TTree */*digitsTree*/, TTree *clustersTree, AliESDEvent *pESD) const
  {

  /***************************************************
  Resonstruct digits to vertex position
  ****************************************************/
  
  AliDebug(1,Form("Start FillESD T0"));
  if(!pESD) {
    AliError("No ESD Event");
    return;
  }
  pESD ->SetT0spread(fTimeSigmaShift);
 

  Float_t channelWidth = fParam->GetChannelWidth() ;  
  Float_t c = 0.0299792458; // cm/ps
  Float_t currentVertex=0, shift=0;
  Int_t ncont=-1;
  const AliESDVertex* vertex = pESD->GetPrimaryVertex();
  if (!vertex)        vertex = pESD->GetPrimaryVertexSPD();
  if (!vertex)        vertex = pESD->GetPrimaryVertexTPC();
  if (!vertex)        vertex = pESD->GetVertex();

  if (vertex) {
    AliDebug(2, Form("Got %s (%s) from ESD: %f", 
		    vertex->GetName(), vertex->GetTitle(), vertex->GetZ()));
    currentVertex = vertex->GetZ();
    
    ncont = vertex->GetNContributors();
    if(ncont>0 ) {
      shift = currentVertex/c;
    }
  }
  TTree *treeR = clustersTree;
  
  AliT0RecPoint frecpoints;
  AliT0RecPoint * pfrecpoints = &frecpoints;
  
  AliDebug(1,Form("Start FillESD T0"));
  TBranch *brRec = treeR->GetBranch("T0");
  if (brRec) {
    brRec->SetAddress(&pfrecpoints);
  }else{
    AliError(Form("EXEC Branch T0 rec not found"));
    return;
  } 
  
  brRec->GetEntry(0);
  Double32_t amp[24], time[24], ampQTC[24], timecorr[24];  
  Double32_t* tcorr;
  for(Int_t i=0; i<24; i++) 
    amp[i]=time[i]=ampQTC[i]=timecorr[i]=0;

  //1st time 
  Double32_t timeClock[3];
  Double32_t zPosition = frecpoints.GetVertex();

  timeClock[0] = frecpoints.GetT0clock() ;
  timeClock[1] = frecpoints.Get1stTimeA() + shift;
  timeClock[2] = frecpoints.Get1stTimeC() - shift;
  //best time
  Double32_t timemean[3];
  timemean[0] = frecpoints.GetMeanTime();
  timemean[1] = frecpoints.GetBestTimeA() + shift;
  timemean[2] = frecpoints.GetBestTimeC() - shift;

  for(Int_t i=0; i<3; i++) {
    fESDTZERO->SetT0TOF(i,timeClock[i]);   // interaction time (ns) 
    fESDTZERO->SetT0TOFbest(i,timemean[i]);   // interaction time (ns) 
  }
  for ( Int_t i=0; i<24; i++) {
    time[i] =  frecpoints.GetTime(i); // ps to ns
    if ( time[i] != 0 && time[i]>-9999) {
      ampQTC[i] = frecpoints.GetAmp(i);
      amp[i] = frecpoints.AmpLED(i);
      AliDebug(1,Form("T0: %i  time %f  ampQTC %f ampLED %f \n", i, time[i], ampQTC[i], amp[i]));
   }
  }
  fESDTZERO->SetT0time(time);         // best TOF on each PMT 
  fESDTZERO->SetT0amplitude(ampQTC);     // number of particles(MIPs) on each PMT
  Int_t trig= frecpoints.GetT0Trig();
  frecpoints.PrintTriggerSignals( trig);
  //  printf(" !!!!! FillESD trigger %i \n",trig);
  fESDTZERO->SetT0Trig(trig);
  fESDTZERO->SetT0zVertex(zPosition); //vertex Z position 

  Double32_t multA=frecpoints.GetMultA();
  Double32_t multC=frecpoints.GetMultC();
  fESDTZERO->SetMultA(multA); // for backward compatubility
  fESDTZERO->SetMultC(multC); // for backward compatubility


  for (Int_t iHit =0; iHit<5; iHit++ ) {
       AliDebug(10,Form("FillESD ::: iHit %i tvdc %f orA %f orC %f\n", iHit,
	   frecpoints.GetTVDC(iHit),
	   frecpoints.GetOrA(iHit),
		       frecpoints.GetOrC(iHit) ));
    fESDTZERO->SetTVDC(iHit,frecpoints.GetTVDC(iHit));
    fESDTZERO->SetOrA(iHit,frecpoints.GetOrA(iHit));
    fESDTZERO->SetOrC(iHit,frecpoints.GetOrC(iHit));
    
    for (Int_t i0=0; i0<24; i0++) {
      //  if(frecpoints.GetTimeFull(i0,iHit)>0){
      //      	printf("FillESD ::: iHit %i cfd %i time %f \n", iHit, i0, frecpoints.GetTimeFull(i0,iHit));
	fESDTZERO->SetTimeFull(i0, iHit,frecpoints.GetTimeFull(i0,iHit));
	// }
	
    }	     	     
  }
  
  AliDebug(1,Form("T0: SPDshift %f Vertex %f (T0A+T0C)/2 best %f #ps T0signal %f ps OrA %f ps OrC %f ps T0trig %i\n",shift, zPosition, timemean[0], timeClock[0], timeClock[1], timeClock[2], trig));

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // background flags
  Bool_t background = BackgroundFlag();
  fESDTZERO->SetBackgroundFlag(background);
  Bool_t pileup =  PileupFlag();
  fESDTZERO->SetPileupFlag(pileup);
  for (Int_t i=0; i<5; i++) {
    fESDTZERO->SetPileupTime(i, frecpoints.GetTVDC(i) ) ;
    //   printf("!!!!!! FillESD :: pileup %i %f %f \n", i,fESDTZERO->GetPileupTime(i), frecpoints.GetTVDC(i));
  }
  Bool_t sat  = SatelliteFlag();
  fESDTZERO->SetSatelliteFlag(sat);
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (pESD) {
    
    AliESDfriend *fr = (AliESDfriend*)pESD->FindListObject("AliESDfriend");
    if (fr) {
      AliDebug(10, Form("Writing TZERO friend data to ESD tree"));

      //     if (ncont>2) {
	tcorr = fESDTZEROfriend->GetT0timeCorr();
	for ( Int_t i=0; i<24; i++) {
  	  if(i<12 && time[i]>1) timecorr[i] = tcorr[i] -  shift/channelWidth;
	  if(i>11 && time[i]>1) timecorr[i] = tcorr[i] +  shift/channelWidth;
	  if(time[i]>1)	  AliDebug(10,Form("T0 friend : %i time %f  ampQTC %f ampLED %f \n", i, timecorr[i], ampQTC[i], amp[i]));
	}
	fESDTZEROfriend->SetT0timeCorr( timecorr) ;
	fESDTZEROfriend->SetT0ampLEDminCFD(amp);
	fESDTZEROfriend->SetT0ampQTC(ampQTC);
 	fr->SetTZEROfriend(fESDTZEROfriend);
	//      }//
    }

    pESD->SetTZEROData(fESDTZERO);
  }

} // vertex in 3 sigma

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 //____________________________________________________________
  
Bool_t AliT0Reconstructor::PileupFlag() const
{
  //
  Bool_t pileup = false;
  Float_t tvdc[5];
  for (Int_t ih=0; ih<5; ih++) 
    {
      tvdc[ih] =  fESDTZERO->GetTVDC(ih);
      
     if(  tvdc[0] !=0 &&  tvdc[0]> -10 && tvdc[0]< 10 )
       if(ih>0 && tvdc[ih]>20 ) 	pileup = true; 
     if( tvdc[0] >20 || (tvdc[0] < -20  &&  tvdc[0] > -9000) ) pileup =true;
     //    if (pileup) printf(" !!!!! pile up %i  tvdc %f \n",ih,  tvdc[ih]); 
    }


  return pileup;

}

 //____________________________________________________________
  
Bool_t AliT0Reconstructor::BackgroundFlag() const
{
 
  Bool_t background = false;
  /*  
      Float_t orA = fESDTZERO->GetOrA(0);
      Float_t orC = fESDTZERO->GetOrC(0);
      Float_t tvdc =  fESDTZERO->GetTVDC(ih);
      
      if ( (orA > -5 && orA <5) && (orC > -5 && orC <5) && (tvdc < -5 || tvdc > 5)) {
      background = true;
      //   printf(" orA %f orC %f tvdc %f\n", orA, orC, tvdc);
      } 
  */ 
  return background;


}


 //____________________________________________________________
  
Bool_t  AliT0Reconstructor::SatelliteFlag() const
{

 Float_t satelliteLow = GetRecoParam() -> GetLowSatelliteThreshold();
  Float_t satelliteHigh = GetRecoParam() -> GetHighSatelliteThreshold();
  Bool_t satellite = false;
  for (Int_t i0=0; i0<24; i0++) {
    Float_t timefull =  fESDTZERO -> GetTimeFull(i0,0);
    if( timefull > satelliteLow && timefull < satelliteHigh)  satellite=true;
  }
	
  return satellite;

}

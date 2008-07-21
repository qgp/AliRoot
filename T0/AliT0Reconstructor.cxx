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
					     fCalib()
{
  //constructor

 AliDebug(1,"Start reconstructor ");
  
  fParam = AliT0Parameters::Instance();
  fParam->Init();
   TString option = GetOption(); 
 
  for (Int_t i=0; i<24; i++){
        TGraph* gr = fParam ->GetAmpLEDRec(i);
	if (gr) fAmpLEDrec.AddAtAndExpand(gr,i) ; 
	  TGraph* gr1 = fParam ->GetAmpLED(i);
	  if (gr1) fAmpLED.AddAtAndExpand(gr1,i) ; 
	  TGraph* gr2 = fParam ->GetQTC(i);
	  if (gr2) fQTC.AddAtAndExpand(gr2,i) ; 
	
  }
  
  fdZonC = TMath::Abs(fParam->GetZPositionShift("T0/C/PMT1"));
  fdZonA = TMath::Abs(fParam->GetZPositionShift("T0/A/PMT15"));
  fCalib = new AliT0Calibrator();

}
//____________________________________________________________________

AliT0Reconstructor::AliT0Reconstructor(const AliT0Reconstructor &r):
  AliReconstructor(r),
					     fdZonA(0),
					     fdZonC(0),
					     fZposition(0),
					     fParam(NULL),
                                             fAmpLEDrec(),
					     fQTC(0),
					     fAmpLED(0),
					     fCalib()

 {
  //
  // AliT0Reconstructor copy constructor
  //

  ((AliT0Reconstructor &) r).Copy(*this);

}

//_____________________________________________________________________________
AliT0Reconstructor &AliT0Reconstructor::operator=(const AliT0Reconstructor &r)
{
  //
  // Assignment operator
  //

  if (this != &r) ((AliT0Reconstructor &) r).Copy(*this);
  return *this;

}

//_____________________________________________________________________________

void AliT0Reconstructor::Reconstruct(TTree*digitsTree, TTree*clustersTree) const
  
{
  // T0 digits reconstruction
  // T0RecPoint writing 
  
   
  TArrayI * timeCFD = new TArrayI(24); 
  TArrayI * timeLED = new TArrayI(24); 
  TArrayI * chargeQT0 = new TArrayI(24); 
  TArrayI * chargeQT1 = new TArrayI(24); 


  //  Int_t mV2Mip = param->GetmV2Mip();     
  //mV2Mip = param->GetmV2Mip();     
  Float_t channelWidth = fParam->GetChannelWidth() ;  
  //  Int_t meanT0 = fParam->GetMeanT0();
  
  AliDebug(1,Form("Start DIGITS reconstruction "));
  

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

  
  Float_t besttimeA=999999;
  Float_t besttimeC=999999;
  Int_t pmtBestA=99999;
  Int_t pmtBestC=99999;
  Float_t timeDiff=999999, meanTime=0;
  
  Int_t mv2MIP = fParam-> GetmV2Mip();     


  AliT0RecPoint* frecpoints= new AliT0RecPoint ();
  clustersTree->Branch( "T0", "AliT0RecPoint" ,&frecpoints, 405,1);
  
 Float_t time[24], adc[24];
  for (Int_t ipmt=0; ipmt<24; ipmt++) {
    if(timeCFD->At(ipmt)>0 ){
      Double_t qt0 = Double_t(chargeQT0->At(ipmt));
      Double_t qt1 = Double_t(chargeQT1->At(ipmt));
      if((qt1-qt0)>0)  adc[ipmt] = Int_t (TMath::Exp( Double_t (channelWidth*(qt1-qt0)/1000)));

      time[ipmt] = fCalib-> WalkCorrection( ipmt, Int_t(qt1) , timeCFD->At(ipmt), "pdc" ) ;
     
      //LED
      Float_t ampLED;    //LED
      Double_t sl = (timeLED->At(ipmt) - time[ipmt])*channelWidth;
      if( adc[ipmt]/Float_t(mv2MIP) < 20) 
	{
	  Double_t qt=((TGraph*)fAmpLEDrec.At(ipmt))->Eval(sl/1000.);
	  ampLED = qt/Float_t(mv2MIP);
	  AliDebug(1,Form(" ipmt %i QTC %f ch QTC in MIP %f, time in chann %f (led-cfd) %f in MIPs %f",
			  ipmt, adc[ipmt], adc[ipmt]/Float_t(mv2MIP), time[ipmt],sl,ampLED ));
	}
      else
	{ AliDebug(1,Form(" amplitude %f MIPs on PMT %i, write QTC in both ", (adc[ipmt]/Float_t(mv2MIP)), ipmt));
	  ampLED=adc[ipmt]/Float_t(mv2MIP);}
      frecpoints->SetTime(ipmt,time[ipmt]);
      frecpoints->SetAmp(ipmt,adc[ipmt]/Float_t(mv2MIP));
      frecpoints->SetAmpLED(ipmt,ampLED);
    }
    else {
      time[ipmt] = 0;
      adc[ipmt] = 0;
    }
  }
  
  for (Int_t ipmt=0; ipmt<12; ipmt++){
    if(time[ipmt] > 1 ) {
      if(time[ipmt]<besttimeC){
	besttimeC=time[ipmt]; //timeC
	pmtBestC=ipmt;
      }
    }
  }
  for ( Int_t ipmt=12; ipmt<24; ipmt++){
    if(time[ipmt] > 1) {
      if(time[ipmt]<besttimeA) {
	besttimeA=time[ipmt]; //timeA
        pmtBestA=ipmt;}
    }
  }
  if(besttimeA !=999999)  frecpoints->SetTimeBestA(Int_t(besttimeA));
  if( besttimeC != 999999 ) frecpoints->SetTimeBestC(Int_t(besttimeC));
  AliDebug(1,Form(" besttimeA %f ch,  besttimeC %f ch",besttimeA, besttimeC));
  Float_t c = 0.0299792; // cm/ps
  Float_t vertex = 0;
  if(besttimeA !=999999 && besttimeC != 999999 ){
    timeDiff = (besttimeC - besttimeA)*channelWidth;
    meanTime = (Float_t((besttimeA + besttimeC -4000)/2) * channelWidth); 
    //    meanTime = (meanT0 - (besttimeA + besttimeC)/2) * channelWidth;
    vertex = c*(timeDiff)/2. + (fdZonA - fdZonC)/2; //-(lenr-lenl))/2;
    frecpoints->SetVertex(vertex);
    frecpoints->SetMeanTime(Int_t(meanTime));
    //online mean
    frecpoints->SetOnlineMean(Int_t(onlineMean * channelWidth));
    AliDebug(1,Form("  timeDiff %f ps,  meanTime %f ps, vertex %f cm online mean %i ps",timeDiff, meanTime,vertex, Int_t(onlineMean * channelWidth )));
    
  }
 
  //time in each channel as time[ipmt]-MeanTimeinThisChannel(with vertex=0)
  /*  
  for (Int_t ipmt=0; ipmt<24; ipmt++) {
    if(time[ipmt]>1) {
      //      time[ipmt] = (time[ipmt] - fTime0vertex[ipmt])*channelWidth;
      time[ipmt] =Int_t  ( Float_t(time[ipmt]) * channelWidth);
      frecpoints->SetTime(ipmt,time[ipmt]);
    }
  }
*/
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
  // T0RecPoint writing 
  
  //Q->T-> coefficients !!!! should be measured!!!
  Int_t allData[110][5];
  
  Int_t timeCFD[24], timeLED[24], chargeQT0[24], chargeQT1[24];
  TString option = GetOption(); 
  AliDebug(10,Form("Option: %s\n", option.Data()));
   
   
  for (Int_t i0=0; i0<105; i0++)
    {
      for (Int_t j0=0; j0<5; j0++) allData[i0][j0]=0; 	
    }
   
  Float_t besttimeA=9999999;
  Float_t besttimeC=9999999;
  Int_t pmtBestA=99999;
  Int_t pmtBestC=99999;
  Float_t timeDiff=9999999, meanTime=0;
  Double_t qt=0;
  Int_t mv2MIP = fParam-> GetmV2Mip();     
  UInt_t type =rawReader->GetType();	 
  
  AliT0RecPoint* frecpoints= new AliT0RecPoint ();
  
  recTree->Branch( "T0", "AliT0RecPoint" ,&frecpoints, 405,1);
   
   
  AliDebug(10," before read data ");
  AliT0RawReader myrawreader(rawReader);
  if (!myrawreader.Next())
    AliDebug(1,Form(" no raw data found!!"));
  else
    {  
      for (Int_t i=0; i<105; i++) {
	for (Int_t iHit=0; iHit<5; iHit++) 
	  {
	    allData[i][iHit] = myrawreader.GetData(i,iHit);
	  }
      }
    
      Float_t channelWidth = fParam->GetChannelWidth() ;  
      
      //       Int_t meanT0 = fParam->GetMeanT0();
      if(option == "pdc"){
	for (Int_t in=0; in<24; in++)  
	  {
	    
	    timeLED[in] = allData[in+1][0] ;
	    timeCFD[in] = allData[in+25][0] ;
	    chargeQT1[in] = allData[in+57][0] ;
	     chargeQT0[in] = allData[in+81][0] ;
	  }
       }
      
      if(option == "cosmic" && type == 7 )
	{
	  
	  for (Int_t in=0; in<12; in++)  
	    {
	      timeCFD[in] = allData[in+1][0] ;
	      timeCFD[in+12] = allData[in+56+1][0] ;
	      timeLED[in] = allData[in+12+1][0] ;
	      timeLED[in+12] = allData[in+68+1][0] ;
	    }
	  
	  for (Int_t in=0; in<12;  in++)
	    {
	      chargeQT1[in]=allData[2*in+25][0];
	      chargeQT0[in]=allData[2*in+26][0];
	    }
	  
	   for (Int_t in=12; in<24;  in++)
	     {
	       chargeQT1[in]=allData[2*in+57][0];
	       chargeQT0[in]=allData[2*in+58][0];
	     }
	   
	 } //cosmic with physics event
       for (Int_t in=0; in<24; in++)  
	 AliDebug(10, Form(" readed Raw %i %i %i %i %i",
			   in, timeLED[in],timeCFD[in],chargeQT0[in],chargeQT1[in]));
       Int_t onlineMean = allData[49][0];       
       
       Float_t time[24], adc[24];
       for (Int_t ipmt=0; ipmt<24; ipmt++) {
	 if(timeCFD[ipmt]>0 && timeLED[ipmt]>0){
	   //for simulated data
	   if(option == "pdc"){
	     Double_t qt0 = Double_t(chargeQT0[ipmt]);
	     Double_t qt1 = Double_t(chargeQT1[ipmt]);
	     if((qt1-qt0)>0)  adc[ipmt] = Int_t(TMath::Exp( Double_t (channelWidth*(qt1-qt0)/1000.)));
	     time[ipmt] = fCalib-> WalkCorrection( ipmt,Int_t(qt1) , timeCFD[ipmt], "pdc" ) ;
	     Double_t sl = (timeLED[ipmt] - time[ipmt])*channelWidth;
	     //pp collison
	     if( adc[ipmt]/Float_t(mv2MIP) < 20) 
	       {
		 if(fAmpLEDrec.At(ipmt)) 
		   qt=((TGraph*)fAmpLEDrec.At(ipmt))->Eval(sl/1000.);
	       }
	     else //PbPb
	       { AliDebug(1,Form(" amplitude %f MIPs on PMT %i, write QTC in both ", (adc[ipmt]/Float_t(mv2MIP)), ipmt));
		 qt=adc[ipmt]/Float_t(mv2MIP);}
	     
	     frecpoints->SetTime(ipmt,time[ipmt]);
	     frecpoints->SetAmp(ipmt,adc[ipmt]/Float_t(mv2MIP));
	     frecpoints->SetAmpLED(ipmt,qt/Float_t(mv2MIP));
	     AliDebug(10,Form(" QTC %f mv,  time in chann %f ampLED %f",adc[ipmt] ,time[ipmt], qt));
	     AliDebug(10,Form("  Amlitude in MIPS LED %f ,  QTC %f \n ", adc[ipmt]/Float_t(mv2MIP),qt/Float_t(mv2MIP)));
	   }
	   
	   
	     //for physics  data
	   if(option == "cosmic" && type == 7) {
	     if(( chargeQT1[ipmt] - chargeQT0[ipmt])>0)  
	       adc[ipmt] = chargeQT1[ipmt] - chargeQT0[ipmt];
	     else
	       adc[ipmt] = 0;
	     //      time[ipmt] = fCalib-> WalkCorrection( ipmt, adc[ipmt], timeCFD[ipmt],"cosmic" ) ;
	     
	     Double_t sl = timeLED[ipmt] - timeCFD[ipmt];
	     time[ipmt] = fCalib-> WalkCorrection( ipmt, Int_t(sl), timeCFD[ipmt],"cosmic" ) ;
	     time[ipmt] = time[ipmt] - allData[0][0] + 5000;
	     AliDebug(10,Form(" ipmt %i QTC %i , time in chann %i (led-cfd) %i ",
			      ipmt, Int_t(adc[ipmt]) ,Int_t(time[ipmt]),Int_t( sl)));
	     Double_t ampMip =( (TGraph*)fAmpLED.At(ipmt))->Eval(sl);
	     Double_t qtMip = ((TGraph*)fQTC.At(ipmt))->Eval(adc[ipmt]);
	     AliDebug(10,Form("  Amlitude in MIPS LED %f ,  QTC %f \n ",ampMip,qtMip));
	     
	     frecpoints->SetTime(ipmt, Float_t(time[ipmt]) );
	     frecpoints->SetAmp(ipmt, Float_t( ampMip)); //for cosmic &pp beam 
	     frecpoints->SetAmpLED(ipmt, Float_t(qtMip));
	   } //if physic data end
	   
	 }
	 else {
	   time[ipmt] = 0;
	   adc[ipmt] = 0;
	 }
       }
       
       for (Int_t ipmt=0; ipmt<12; ipmt++){
	 if(time[ipmt] > 1 ) {
	   if(time[ipmt]<besttimeC){
	     besttimeC=time[ipmt]; //timeC
	     pmtBestC=ipmt;
	   }
	 }
       }
       for ( Int_t ipmt=12; ipmt<24; ipmt++){
	 if(time[ipmt] > 1) {
	   if(time[ipmt]<besttimeA) {
	     besttimeA=time[ipmt]; //timeA
	     pmtBestA=ipmt;}
	 }
       }
       if(besttimeA !=9999999)  frecpoints->SetTimeBestA(Int_t(besttimeA));
       if( besttimeC != 9999999 ) frecpoints->SetTimeBestC(Int_t(besttimeC));
       AliDebug(1,Form(" besttimeA %f ps,  besttimeC %f ps",besttimeA, besttimeC));
       Float_t c = 0.0299792; // cm/ps
       Float_t vertex = 99999;
       if(besttimeA <9999999 && besttimeC < 9999999 ){
	 timeDiff = ( besttimeC - besttimeA) *channelWidth;
	 if(option == "pdc"){ 
	   //	   meanTime = (besttimeA + besttimeC)/2 * channelWidth; 
	   meanTime = (besttimeA + besttimeC-4000.)/2 * channelWidth; 
	   onlineMean = Int_t (onlineMean * channelWidth);
	 }
	 if(option == "cosmic") {
	   meanTime =  Float_t((besttimeA + besttimeC)/2);  
	   onlineMean = onlineMean -allData[0][0];;
	 }
	 vertex = c*(timeDiff)/2.+ (fdZonA - fdZonC)/2; 
	 frecpoints->SetVertex(vertex);
	 frecpoints->SetMeanTime(Int_t(meanTime));
	 AliDebug(1,Form("  timeDiff %f ps,  meanTime %f ps, vertex %f cm online mean %i ",timeDiff, meanTime,vertex, onlineMean));
	 
       }
    } // if (else )raw data
	 recTree->Fill();
	 if(frecpoints) delete frecpoints;
}
       
       
//____________________________________________________________

void AliT0Reconstructor::FillESD(TTree */*digitsTree*/, TTree *clustersTree, AliESDEvent *pESD) const
{

  /***************************************************
  Resonstruct digits to vertex position
  ****************************************************/
  
  AliDebug(1,Form("Start FillESD T0"));

  TTree *treeR = clustersTree;
  
   AliT0RecPoint* frecpoints= new AliT0RecPoint ();
    if (!frecpoints) {
    AliError("Reconstruct Fill ESD >> no recpoints found");
    return;
  }
  
  AliDebug(1,Form("Start FillESD T0"));
  TBranch *brRec = treeR->GetBranch("T0");
  if (brRec) {
    brRec->SetAddress(&frecpoints);
  }else{
    AliError(Form("EXEC Branch T0 rec not found"));
    return;
  } 
    
    brRec->GetEntry(0);
    Float_t amp[24], time[24];
    Float_t  zPosition = frecpoints -> GetVertex();
    Float_t timeStart = frecpoints -> GetMeanTime() ;
    for ( Int_t i=0; i<24; i++) {
      time[i] = Float_t (frecpoints -> GetTime(i)); // ps to ns
      amp[i] = frecpoints -> GetAmp(i);
    }
    pESD->SetT0zVertex(zPosition); //vertex Z position 
    pESD->SetT0(timeStart);        // interaction time 
    pESD->SetT0time(time);         // best TOF on each PMT 
    pESD->SetT0amplitude(amp);     // number of particles(MIPs) on each PMT
 
    AliDebug(1,Form(" Z position %f cm,  T0  %f ps",zPosition , timeStart));

} // vertex in 3 sigma







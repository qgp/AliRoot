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
/* $Id: AliTOFT0maker.cxx,v 1.8 2010/01/19 16:32:20 noferini Exp $ */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  This class contains the basic functions for the time zero              //
//  evaluation with TOF detector informations.                             //
// Use case in an analysis task:                                           //
//                                                                         //
// Create the object in the task constructor (fTOFmaker is a private var)  //
// fTOFmaker = new AliTOFT0maker();                                        //
// fTOFmaker->SetTimeResolution(130.0); // if you want set the TOF res     //
// 115 ps is the TOF default resolution value                              //
//                                                                         //
// Use the RemakePID method in the task::Exec                              //
// Double_t* calcolot0;                                                    //
// calcolot0=fTOFmaker->RemakePID(fESD);                                   //
// //calcolot0[0] = calculated event time                                  // 
// //calcolot0[1] = event time time resolution                             //
// //calcolot0[2] = average event time for the current fill                //
// //calcolot0[3] = tracks at TOF                                          // 
// //calcolot0[4] = calculated event time (only TOF)                       //
// //calcolot0[5] = event time time resolution (only TOF)                  //
// //calcolot0[6] = sigma t0 fill                                          //
// //calcolot0[7] = tracks at TOF really used in tht algorithm             // 
//                                                                         //
// Let consider that:                                                      //
// - the PIF is automatically recalculated with the event time subtrction  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "AliTOFT0v1.h"
#include "AliTOFT0maker.h"
#include "AliTOFcalibHisto.h"
#include "AliPID.h"
#include "AliESDpid.h"
#include "AliESDEvent.h"
#include "TFile.h"
#include "TH1F.h"

ClassImp(AliTOFT0maker)
           
//____________________________________________________________________________ 
  AliTOFT0maker::AliTOFT0maker():
    fCalib(new AliTOFcalibHisto()),
    fnT0(0),
    fiT0(0),
    fNoTOFT0(0),
    fESDswitch(0),
    fTimeResolution(115),
    fT0sigma(1000),
    fHmapChannel(0),
    fKmask(0)
{
  // ctr
  fCalculated[0] = 0;
  fCalculated[1] = 0;
  fCalculated[2] = 0;
  fCalculated[3] = 0;

  // fCalib->SetCalibParFileName("./AliTOFcalibPar.LHC10b.7000GeV.20100405.root");
  fCalib->LoadCalibPar();

  if(AliPID::ParticleMass(0) == 0) new AliPID();

  SetESDdata();
}
//____________________________________________________________________________ 
AliTOFT0maker::AliTOFT0maker(const AliTOFT0maker & t) :
  TObject(),
  fCalib(t.fCalib),
  fnT0(t.fnT0),
  fiT0(t.fiT0),
  fNoTOFT0(t.fNoTOFT0),
  fESDswitch(t.fESDswitch),
  fTimeResolution(t.fTimeResolution),
  fT0sigma(t.fT0sigma),
  fHmapChannel(t.fHmapChannel),
  fKmask(t.fKmask)
{
  // copy ctr
}

//____________________________________________________________________________ 
AliTOFT0maker& AliTOFT0maker::operator=(const AliTOFT0maker &t)
{
  //
  // assign. operator
  //

  if (this == &t)
    return *this;
  fCalib = t.fCalib;
  fESDswitch = t.fESDswitch;
  fTimeResolution = t.fTimeResolution;
  fT0sigma = t.fT0sigma;

  return *this;
}
//____________________________________________________________________________ 
AliTOFT0maker::~AliTOFT0maker()
{
  // dtor
  if(fCalib) delete fCalib;
}
//____________________________________________________________________________ 
Double_t* AliTOFT0maker::RemakePID(AliESDEvent *esd,Double_t t0time,Double_t t0sigma){
  //
  // Remake TOF PID probabilities
  //

  Double_t *t0tof;

  if(fKmask) ApplyMask(esd);

  AliTOFT0v1* t0maker=new AliTOFT0v1(esd);
//   t0maker->SetCalib(fCalib);
  t0maker->SetTimeResolution(fTimeResolution*1e-12);

  if(! fESDswitch){
    TakeTimeRawCorrection(esd);
  }

  t0tof=t0maker->DefineT0("all");

  Float_t lT0Current=0.;
  fT0sigma=1000;

//   Int_t nrun = esd->GetRunNumber();
  Double_t t0fill = GetT0Fill();
  t0time += t0fill;

  Float_t sigmaFill = (t0fill - Int_t(t0fill))*1000;
  if(sigmaFill < 0) sigmaFill += 1000;

  fCalculated[0]=-1000*t0tof[0]; // best t0
  fCalculated[1]=1000*t0tof[1]; // sigma best t0
  fCalculated[2] = t0fill;    //t0 fill
  fCalculated[3] = t0tof[2];  // n TOF tracks
  fCalculated[4]=-1000*t0tof[0]; // TOF t0
  fCalculated[5]=1000*t0tof[1]; // TOF t0 sigma
  fCalculated[6]=sigmaFill; // sigma t0 fill
  fCalculated[7] = t0tof[3];  // n TOF tracks used for T0

  if(fCalculated[1] < sigmaFill){
    if(fnT0 < 10){
      fT0fill[fiT0] = fCalculated[0];
      fT0sigmaTOF[fiT0] = fCalculated[1];
      fiT0++;
      fnT0++;
    }
    else if(TMath::Abs(fCalculated[0] - t0fill) < 500){
      fT0fill[fiT0] = fCalculated[0];
      fT0sigmaTOF[fiT0] = fCalculated[1];
      fiT0++;
      fnT0++;
    }

    //        printf("%i - %i) %f\n",fiT0,fnT0,t0fill);
  }
  if(fnT0==10) fiT0=0;

  if(fiT0 > fgkNmaxT0step-1) fiT0=0;

  if(fnT0 < 100){
    t0time -= t0fill;
    sigmaFill=200;
    t0fill=0;
    fCalculated[2] = t0fill;    //t0 fill
  }

  if(fCalculated[1] < sigmaFill && TMath::Abs(fCalculated[0] - t0fill) < 500){
    fT0sigma=fCalculated[1];
    lT0Current=fCalculated[0];
  }
  else{
    fCalculated[4] = t0fill;
    fCalculated[5] = sigmaFill;
  }

  if(fCalculated[1] < 1 || fT0sigma > sigmaFill){
    fT0sigma =1000;
    fCalculated[4] = t0fill;
    fCalculated[5] = sigmaFill;
  }

  if(t0sigma < 1000){
    if(fT0sigma < 1000){
      Double_t w1 = 1./t0sigma/t0sigma;
      Double_t w2 = 1./fCalculated[1]/fCalculated[1];

      Double_t wtot = w1+w2;

      lT0Current = (w1*t0time + w2*fCalculated[0]) / wtot;
      fT0sigma = TMath::Sqrt(1./wtot);
    }
    else{
      lT0Current=t0time;
      fT0sigma=t0sigma;
    }
  }

  if(fT0sigma < sigmaFill && TMath::Abs(lT0Current - t0fill) < 500){
    fCalculated[1]=fT0sigma;
    fCalculated[0]=lT0Current;
  }

  if(fT0sigma >= 1000 || fNoTOFT0){
    lT0Current = t0fill;
    fT0sigma = sigmaFill;

    fCalculated[0] = t0fill;
    fCalculated[1] = sigmaFill;
  }



  RemakeTOFpid(esd,lT0Current);
  
  return fCalculated;
}
//____________________________________________________________________________ 
void AliTOFT0maker::TakeTimeRawCorrection(AliESDEvent * const esd){
  //
  // Take raw corrections for time measurements
  //
  
  Int_t ntracks = esd->GetNumberOfTracks();
  
  while (ntracks--) {
    AliESDtrack *t=esd->GetTrack(ntracks);
    
    if ((t->GetStatus()&AliESDtrack::kTOFout)==0) continue;
    
    Double_t time=t->GetTOFsignalRaw();

    Double_t tot = t->GetTOFsignalToT();
    Int_t chan = t->GetTOFCalChannel();
    Double_t corr = fCalib->GetFullCorrection(chan,tot) - fCalib->GetCorrection(AliTOFcalibHisto::kTimeSlewingCorr,chan,0);
    time -= corr*1000;

    t->SetTOFsignal(time);

  }
}
//____________________________________________________________________________ 
void AliTOFT0maker::RemakeTOFpid(AliESDEvent *esd,Float_t timezero){
  //
  // Recalculate TOF PID probabilities
  //

  AliESDpid pidESD;
  pidESD.GetTOFResponse().SetTimeResolution(TMath::Sqrt(fT0sigma*fT0sigma + fTimeResolution*fTimeResolution));
  pidESD.MakePID(esd,kFALSE,timezero);
  
}
//____________________________________________________________________________ 
Double_t AliTOFT0maker::GetT0Fill() const {
  //
  // Return T0 of filling
  //
  
  Double_t t0=0.200;

  Int_t n=fnT0;

  if(n >10 && n <= 20) n = 10; 
  else if(n > 20){
    n -= 10;
  }
  
  if(n > fgkNmaxT0step) n = fgkNmaxT0step;

  if(n>1){
    Double_t lT0av=0;
    Double_t lT0sigmaav=0;
    Double_t lT0avErr=0;
    for(Int_t i=0;i<n;i++){
      lT0av+=fT0fill[i];
      lT0sigmaav += fT0sigmaTOF[fiT0];
      lT0avErr+=fT0fill[i]*fT0fill[i];
    }
    lT0avErr -= lT0av*lT0av/n;
    lT0av /= n;
    lT0sigmaav /= n;
    lT0avErr = TMath::Sqrt(TMath::Max(lT0avErr/(n-1) - lT0sigmaav*lT0sigmaav,0.00001));
    

    if(lT0avErr > 300) lT0avErr = 300;

    lT0av = Int_t(lT0av) + lT0avErr/1000.;

    return lT0av;
  }


  return t0;
}
//____________________________________________________________________________ 
void  AliTOFT0maker::LoadChannelMap(char *filename){
  // Load the histo with the channel off map
  TFile *f= new TFile(filename);
  if(!f){
    printf("Cannot open the channel map file (%s)\n",filename);
    return;
  }
  
  fHmapChannel = (TH1F *) f->Get("hChEnabled");
  
  if(!fHmapChannel){
    printf("Cannot laod the channel map histo (from %s)\n",filename);
    return;
  }
    
}
//____________________________________________________________________________ 
void AliTOFT0maker::ApplyMask(AliESDEvent * const esd){
  // Switch off the disable channel
  if(!fHmapChannel){
    printf("Channel Map is not available\n");
    return;
  }
  
  Int_t ntracks = esd->GetNumberOfTracks();
  
  while (ntracks--) {
    AliESDtrack *t=esd->GetTrack(ntracks);    

    if ((t->GetStatus()&AliESDtrack::kTOFout)==0) continue;

    Int_t chan = t->GetTOFCalChannel();
 
    if(fHmapChannel->GetBinContent(chan) < 0.01){
      t->ResetStatus(AliESDtrack::kTOFout);
    }
  }
}

//____________________________________________________________________________ 

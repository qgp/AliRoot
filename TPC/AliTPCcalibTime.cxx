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

/*
Comments to be written here:

1. What do we calibrate.

  Time dependence of gain and drift velocity in order to account for changes in: temperature, pressure, gas composition.

  AliTPCcalibTime *calibTime = new AliTPCcalibTime("cosmicTime","cosmicTime",0, 1213.9e+06, 1213.96e+06, 0.04e+04, 0.04e+04);

2. How to interpret results

3. Simple example

  a) determine the required time range:

  AliXRDPROOFtoolkit tool;
  TChain * chain = tool.MakeChain("pass2.txt","esdTree",0,6000);
  chain->Draw("GetTimeStamp()")

  b) analyse calibration object on Proof in calibration train 

  AliTPCcalibTime *calibTime = new AliTPCcalibTime("cosmicTime","cosmicTime", StartTimeStamp, EndTimeStamp, IntegrationTimeVdrift);

  c) plot results
  .x ~/NimStyle.C
  gSystem->Load("libANALYSIS");
  gSystem->Load("libTPCcalib");

  TFile f("CalibObjectsTrain1.root");
  AliTPCcalibTime *calib = (AliTPCcalibTime *)f->Get("calibTime");
  calib->GetHistoDrift("all")->Projection(2,0)->Draw()
  calib->GetFitDrift("all")->Draw("lp")

4. Analysis using debug streamers.    

  gSystem->AddIncludePath("-I$ALICE_ROOT/TPC/macros");
  gROOT->LoadMacro("$ALICE_ROOT/TPC/macros/AliXRDPROOFtoolkit.cxx+")
  AliXRDPROOFtoolkit tool;
  TChain * chainTime = tool.MakeChainRandom("time.txt","trackInfo",0,10000);

  AliXRDPROOFtoolkit::FilterList("timetpctpc.txt","* tpctpc",1) 
  AliXRDPROOFtoolkit::FilterList("timetoftpc.txt","* toftpc",1) 
  AliXRDPROOFtoolkit::FilterList("timeitstpc.txt","* itstpc",1) 
  AliXRDPROOFtoolkit::FilterList("timelaser.txt","* laserInfo",1)  
  TChain * chainTPCTPC = tool.MakeChainRandom("timetpctpc.txt.Good","tpctpc",0,10000); 
  TChain * chainTPCITS = tool.MakeChainRandom("timeitstpc.txt.Good","itstpc",0,10000); 
  TChain * chainTPCTOF = tool.MakeChainRandom("timetoftpc.txt.Good","toftpc",0,10000); 
  TChain * chainLaser = tool.MakeChainRandom("timelaser.txt.Good","laserInfo",0,10000);
  chainTime->Lookup();
  chainLaser->Lookup();
*/

#include "Riostream.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "THnSparse.h"
#include "TList.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"
#include "TVectorD.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "AliTPCclusterMI.h"
#include "AliTPCseed.h"
#include "AliESDVertex.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "AliESDInputHandler.h"
#include "AliAnalysisManager.h"

#include "AliTracker.h"
#include "AliMagF.h"
#include "AliTPCCalROC.h"
#include "AliTPCParam.h"

#include "AliLog.h"

#include "AliTPCcalibTime.h"
#include "AliRelAlignerKalman.h"

#include "TTreeStream.h"
#include "AliTPCTracklet.h"
#include "TTimeStamp.h"
#include "AliTPCcalibDB.h"
#include "AliTPCcalibLaser.h"
#include "AliDCSSensorArray.h"
#include "AliDCSSensor.h"

#include "TDatabasePDG.h"
#include "AliTrackPointArray.h"

ClassImp(AliTPCcalibTime)


AliTPCcalibTime::AliTPCcalibTime() 
  :AliTPCcalibBase(), 
   fLaser(0),       // pointer to laser calibration
   fDz(0),          // current delta z
   fCutMaxD(3),        // maximal distance in rfi ditection
   fCutMaxDz(25),      // maximal distance in rfi ditection
   fCutTheta(0.03),    // maximal distan theta
   fCutMinDir(-0.99),  // direction vector products
   fCutTracks(10),
   fArrayDz(0),          //NEW! Tmap of V drifts for different triggers
   fAlignITSTPC(0),      //alignemnt array ITS TPC match
   fAlignTRDTPC(0),      //alignemnt array TRD TPC match 
   fAlignTOFTPC(0),      //alignemnt array TOF TPC match
   fTimeBins(0),
   fTimeStart(0),
   fTimeEnd(0),
   fPtBins(0),
   fPtStart(0),
   fPtEnd(0),
   fVdriftBins(0),
   fVdriftStart(0),
   fVdriftEnd(0),
   fRunBins(0),
   fRunStart(0),
   fRunEnd(0)
//   fBinsVdrift(fTimeBins,fPtBins,fVdriftBins),
//   fXminVdrift(fTimeStart,fPtStart,fVdriftStart),
//   fXmaxVdrift(fTimeEnd,fPtEnd,fVdriftEnd)
{  
  AliInfo("Default Constructor");  
  for (Int_t i=0;i<3;i++) {
    fHistVdriftLaserA[i]=0;
    fHistVdriftLaserC[i]=0;
  }
  for (Int_t i=0;i<10;i++) {
    fCosmiMatchingHisto[i]=0;
  }
}

AliTPCcalibTime::AliTPCcalibTime(const Text_t *name, const Text_t *title, UInt_t StartTime, UInt_t EndTime, Int_t deltaIntegrationTimeVdrift)
  :AliTPCcalibBase(),
   fLaser(0),            // pointer to laser calibration
   fDz(0),               // current delta z
   fCutMaxD(5*0.5356),   // maximal distance in rfi ditection
   fCutMaxDz(40),   // maximal distance in rfi ditection
   fCutTheta(5*0.004644),// maximal distan theta
   fCutMinDir(-0.99),    // direction vector products
   fCutTracks(10),
   fArrayDz(0),            //Tmap of V drifts for different triggers
   fAlignITSTPC(0),      //alignemnt array ITS TPC match
   fAlignTRDTPC(0),      //alignemnt array TRD TPC match 
   fAlignTOFTPC(0),      //alignemnt array TOF TPC match
   fTimeBins(0),
   fTimeStart(0),
   fTimeEnd(0),
   fPtBins(0),
   fPtStart(0),
   fPtEnd(0),
   fVdriftBins(0),
   fVdriftStart(0),
   fVdriftEnd(0),
   fRunBins(0),
   fRunStart(0),
   fRunEnd(0)
{
  SetName(name);
  SetTitle(title);
  for (Int_t i=0;i<3;i++) {
    fHistVdriftLaserA[i]=0;
    fHistVdriftLaserC[i]=0;
  }

  AliInfo("Non Default Constructor");
  fTimeBins   =(EndTime-StartTime)/deltaIntegrationTimeVdrift;
  fTimeStart  =StartTime; //(((TObjString*)(mapGRP->GetValue("fAliceStartTime")))->GetString()).Atoi();
  fTimeEnd    =EndTime;   //(((TObjString*)(mapGRP->GetValue("fAliceStopTime")))->GetString()).Atoi();
  fPtBins     = 400;
  fPtStart    = -0.04;
  fPtEnd      =  0.04;
  fVdriftBins = 500;
  fVdriftStart= -0.1;
  fVdriftEnd  =  0.1;
  fRunBins    = 100001;
  fRunStart   = -1.5;
  fRunEnd     = 99999.5;

  Int_t    binsVdriftLaser[4] = {fTimeBins , fPtBins , fVdriftBins*20, fRunBins };
  Double_t xminVdriftLaser[4] = {fTimeStart, fPtStart, fVdriftStart  , fRunStart};
  Double_t xmaxVdriftLaser[4] = {fTimeEnd  , fPtEnd  , fVdriftEnd    , fRunEnd  };
  TString axisTitle[4]={
    "T",
    "#delta_{P/T}",
    "value",
    "run"
  };
  TString histoName[3]={
    "Loffset",
    "Lcorr",
    "Lgy"
  };

  
  for (Int_t i=0;i<3;i++) {
    fHistVdriftLaserA[i] = new THnSparseF("HistVdriftLaser","HistVdriftLaser;time;p/T ratio;Vdrift;run",4,binsVdriftLaser,xminVdriftLaser,xmaxVdriftLaser);
    fHistVdriftLaserC[i] = new THnSparseF("HistVdriftLaser","HistVdriftLaser;time;p/T ratio;Vdrift;run",4,binsVdriftLaser,xminVdriftLaser,xmaxVdriftLaser);
    fHistVdriftLaserA[i]->SetName(histoName[i]);
    fHistVdriftLaserC[i]->SetName(histoName[i]);
    for (Int_t iaxis=0; iaxis<4;iaxis++){
      fHistVdriftLaserA[i]->GetAxis(iaxis)->SetName(axisTitle[iaxis]);
      fHistVdriftLaserC[i]->GetAxis(iaxis)->SetName(axisTitle[iaxis]);
    }
  }
  fBinsVdrift[0] = fTimeBins;
  fBinsVdrift[1] = fPtBins;
  fBinsVdrift[2] = fVdriftBins;
  fBinsVdrift[3] = fRunBins;
  fXminVdrift[0] = fTimeStart;
  fXminVdrift[1] = fPtStart;
  fXminVdrift[2] = fVdriftStart;
  fXminVdrift[3] = fRunStart;
  fXmaxVdrift[0] = fTimeEnd;
  fXmaxVdrift[1] = fPtEnd;
  fXmaxVdrift[2] = fVdriftEnd;
  fXmaxVdrift[3] = fRunEnd;

  fArrayDz=new TObjArray();
  fAlignITSTPC = new TObjArray;      //alignemnt array ITS TPC match
  fAlignTRDTPC = new TObjArray;      //alignemnt array ITS TPC match
  fAlignTOFTPC = new TObjArray;      //alignemnt array ITS TPC match
  fAlignITSTPC->SetOwner(kTRUE);
  fAlignTRDTPC->SetOwner(kTRUE);
  fAlignTOFTPC->SetOwner(kTRUE);
  
  // fArrayDz->AddLast(fHistVdriftLaserA[0]);
//   fArrayDz->AddLast(fHistVdriftLaserA[1]);
//   fArrayDz->AddLast(fHistVdriftLaserA[2]);
//   fArrayDz->AddLast(fHistVdriftLaserC[0]);
//   fArrayDz->AddLast(fHistVdriftLaserC[1]);
//   fArrayDz->AddLast(fHistVdriftLaserC[2]);

  fCosmiMatchingHisto[0]=new TH1F("Cosmics matching","p0-all"   ,100,-10*0.5356  ,10*0.5356  );
  fCosmiMatchingHisto[1]=new TH1F("Cosmics matching","p1-all"   ,100,-10*4.541   ,10*4.541   );
  fCosmiMatchingHisto[2]=new TH1F("Cosmics matching","p2-all"   ,100,-10*0.01134 ,10*0.01134 );
  fCosmiMatchingHisto[3]=new TH1F("Cosmics matching","p3-all"   ,100,-10*0.004644,10*0.004644);
  fCosmiMatchingHisto[4]=new TH1F("Cosmics matching","p4-all"   ,100,-10*0.03773 ,10*0.03773 );
  fCosmiMatchingHisto[5]=new TH1F("Cosmics matching","p0-isPair",100,-10*0.5356  ,10*0.5356  );
  fCosmiMatchingHisto[6]=new TH1F("Cosmics matching","p1-isPair",100,-10*4.541   ,10*4.541   );
  fCosmiMatchingHisto[7]=new TH1F("Cosmics matching","p2-isPair",100,-10*0.01134 ,10*0.01134 );
  fCosmiMatchingHisto[8]=new TH1F("Cosmics matching","p3-isPair",100,-10*0.004644,10*0.004644);
  fCosmiMatchingHisto[9]=new TH1F("Cosmics matching","p4-isPair",100,-10*0.03773 ,10*0.03773 );
//  Char_t nameHisto[3]={'p','0','\n'};
//  for (Int_t i=0;i<10;i++){
//    fCosmiMatchingHisto[i]=new TH1F("Cosmics matching",nameHisto,8192,0,0);
//    nameHisto[1]++;
//    if(i==4) nameHisto[1]='0';
//  }
}

AliTPCcalibTime::~AliTPCcalibTime(){
  //
  // Destructor
  //
  for(Int_t i=0;i<3;i++){
    if(fHistVdriftLaserA[i]){
      delete fHistVdriftLaserA[i];
      fHistVdriftLaserA[i]=NULL;
    }
    if(fHistVdriftLaserC[i]){
      delete fHistVdriftLaserC[i];
      fHistVdriftLaserC[i]=NULL;
    }
  }
  if(fArrayDz){
    fArrayDz->SetOwner();
    fArrayDz->Delete();
    delete fArrayDz;
    fArrayDz=NULL;
  }
  for(Int_t i=0;i<5;i++){
    if(fCosmiMatchingHisto[i]){
      delete fCosmiMatchingHisto[i];
      fCosmiMatchingHisto[i]=NULL;
    }
  }
  fAlignITSTPC->SetOwner(kTRUE);
  fAlignTRDTPC->SetOwner(kTRUE);
  fAlignTOFTPC->SetOwner(kTRUE);

  fAlignITSTPC->Delete();
  fAlignTRDTPC->Delete();
  fAlignTOFTPC->Delete();
  delete fAlignITSTPC;
  delete fAlignTRDTPC;
  delete fAlignTOFTPC;
}

Bool_t AliTPCcalibTime::IsLaser(AliESDEvent */*event*/){
  return kTRUE; //More accurate creteria to be added
}
Bool_t AliTPCcalibTime::IsCosmics(AliESDEvent */*event*/){
  return kTRUE; //More accurate creteria to be added
}
Bool_t AliTPCcalibTime::IsBeam(AliESDEvent */*event*/){
  return kTRUE; //More accurate creteria to be added
}
void AliTPCcalibTime::ResetCurrent(){
  fDz=0; //Reset current dz
}
void AliTPCcalibTime::Process(AliESDEvent *event){
  if(!event) return;
  if (event->GetNumberOfTracks()<2) return;
  ResetCurrent();
  if(IsLaser  (event)) ProcessLaser (event);
  if(IsCosmics(event)) ProcessCosmic(event);
  if(IsBeam   (event)) ProcessBeam  (event);
}

void AliTPCcalibTime::ProcessLaser(AliESDEvent *event){
  //
  // Fit drift velocity using laser 
  // 
  // 0. cuts
  const Int_t    kMinTracks     = 40;    // minimal number of laser tracks
  const Int_t    kMinTracksSide = 20;    // minimal number of tracks per side
  const Float_t  kMaxDeltaZ     = 30.;   // maximal trigger delay
  const Float_t  kMaxDeltaV     = 0.05;  // maximal deltaV 
  const Float_t  kMaxRMS        = 0.1;   // maximal RMS of tracks
  //
  /*
    TCut cutRMS("sqrt(laserA.fElements[4])<0.1&&sqrt(laserC.fElements[4])<0.1");
    TCut cutZ("abs(laserA.fElements[0]-laserC.fElements[0])<3");
    TCut cutV("abs(laserA.fElements[1]-laserC.fElements[1])<0.01");
    TCut cutY("abs(laserA.fElements[2]-laserC.fElements[2])<2");
    TCut cutAll = cutRMS+cutZ+cutV+cutY;
  */
  if (event->GetNumberOfTracks()<kMinTracks) return;
  //
  if(!fLaser) fLaser = new AliTPCcalibLaser("laserTPC","laserTPC",kFALSE);
  fLaser->Process(event);
  if (fLaser->GetNtracks()<kMinTracks) return;   // small amount of tracks cut
  if (fLaser->fFitAside->GetNrows()==0  && fLaser->fFitCside->GetNrows()==0) return;  // no fit neither a or C side
  //
  // debug streamer  - activate stream level
  // Use it for tuning of the cuts
  //
  // cuts to be applied
  //
  Int_t isReject[2]={0,0};
  //
  // not enough tracks 
  if (TMath::Abs((*fLaser->fFitAside)[3]) < kMinTracksSide) isReject[0]|=1; 
  if (TMath::Abs((*fLaser->fFitCside)[3]) < kMinTracksSide) isReject[1]|=1; 
  // unreasonable z offset
  if (TMath::Abs((*fLaser->fFitAside)[0])>kMaxDeltaZ)  isReject[0]|=2;
  if (TMath::Abs((*fLaser->fFitCside)[0])>kMaxDeltaZ)  isReject[1]|=2;
  // unreasonable drift velocity
  if (TMath::Abs((*fLaser->fFitAside)[1]-1)>kMaxDeltaV)  isReject[0]|=4;
  if (TMath::Abs((*fLaser->fFitCside)[1]-1)>kMaxDeltaV)  isReject[1]|=4;
  // big chi2
  if (TMath::Sqrt(TMath::Abs((*fLaser->fFitAside)[4]))>kMaxRMS ) isReject[0]|=8;
  if (TMath::Sqrt(TMath::Abs((*fLaser->fFitCside)[4]))>kMaxRMS ) isReject[1]|=8;




  if (fStreamLevel>0){
    printf("Trigger: %s\n",event->GetFiredTriggerClasses().Data());

    TTreeSRedirector *cstream = GetDebugStreamer();
    if (cstream){
      TTimeStamp tstamp(fTime);
      Float_t valuePressure0 = AliTPCcalibDB::GetPressure(tstamp,fRun,0);
      Float_t valuePressure1 = AliTPCcalibDB::GetPressure(tstamp,fRun,1);
      Double_t ptrelative0   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
      Double_t ptrelative1   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
      Double_t temp0         = AliTPCcalibDB::GetTemperature(tstamp,fRun,0);
      Double_t temp1         = AliTPCcalibDB::GetTemperature(tstamp,fRun,1);
      TVectorD vecGoofie(20);
      AliDCSSensorArray* goofieArray = AliTPCcalibDB::Instance()->GetGoofieSensors(fRun);
      if (goofieArray){
	for (Int_t isensor=0; isensor<goofieArray->NumSensors();isensor++){
	  AliDCSSensor *gsensor = goofieArray->GetSensor(isensor);
	  if (gsensor) vecGoofie[isensor]=gsensor->GetValue(tstamp);
	}
      }
      (*cstream)<<"laserInfo"<<
	"run="<<fRun<<              //  run number
	"event="<<fEvent<<          //  event number
	"time="<<fTime<<            //  time stamp of event
	"trigger="<<fTrigger<<      //  trigger
	"mag="<<fMagF<<             //  magnetic field
	// Environment values
	"press0="<<valuePressure0<<
	"press1="<<valuePressure1<<
	"pt0="<<ptrelative0<<
	"pt1="<<ptrelative1<<
	"temp0="<<temp0<<
	"temp1="<<temp1<<
	"vecGoofie.="<<&vecGoofie<<
	//laser
	"rejectA="<<isReject[0]<<
	"rejectC="<<isReject[1]<<
	"laserA.="<<fLaser->fFitAside<<
	"laserC.="<<fLaser->fFitCside<<
	"laserAC.="<<fLaser->fFitACside<<
	"trigger="<<event->GetFiredTriggerClasses()<<
	"\n";
    }
  }
  //
  // fill histos
  //
  TVectorD vdriftA(5), vdriftC(5),vdriftAC(5);
  vdriftA=*(fLaser->fFitAside);
  vdriftC=*(fLaser->fFitCside);
  vdriftAC=*(fLaser->fFitACside);
  Int_t npointsA=0, npointsC=0;
  Float_t chi2A=0, chi2C=0;
  npointsA= TMath::Nint(vdriftA[3]);
  chi2A= vdriftA[4];
  npointsC= TMath::Nint(vdriftC[3]);
  chi2C= vdriftC[4];

  TTimeStamp tstamp(fTime);
  Double_t ptrelative0 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
  Double_t ptrelative1 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
  Double_t driftA=0, driftC=0;
  if (vdriftA[1]>1.-kMaxDeltaV) driftA = 1./vdriftA[1]-1.;
  if (vdriftC[1]>1.-kMaxDeltaV) driftC = 1./vdriftC[1]-1.;
  //
  Double_t vecDriftLaserA[4]={fTime,(ptrelative0+ptrelative1)/2.0,driftA,event->GetRunNumber()};
  Double_t vecDriftLaserC[4]={fTime,(ptrelative0+ptrelative1)/2.0,driftC,event->GetRunNumber()};
  //  Double_t vecDrift[4]      ={fTime,(ptrelative0+ptrelative1)/2.0,1./((*(fLaser->fFitACside))[1])-1,event->GetRunNumber()};

  for (Int_t icalib=0;icalib<3;icalib++){
    if (icalib==0){ //z0 shift
      vecDriftLaserA[2]=vdriftA[0]/250.;
      vecDriftLaserC[2]=vdriftC[0]/250.;
    }
    if (icalib==1){ //vdrel shift
      vecDriftLaserA[2]=driftA;
      vecDriftLaserC[2]=driftC;
    }
    if (icalib==2){ //gy shift - full gy - full drift
      vecDriftLaserA[2]=vdriftA[2]/250.;
      vecDriftLaserC[2]=vdriftC[2]/250.;
    }
    if (isReject[0]==0) fHistVdriftLaserA[icalib]->Fill(vecDriftLaserA);
    if (isReject[1]==0) fHistVdriftLaserC[icalib]->Fill(vecDriftLaserC);
  }

//   THnSparse* curHist=new THnSparseF("","HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//   TString shortName=curHist->ClassName();
//   shortName+="_MEAN_DRIFT_LASER_";
//   delete curHist;
//   curHist=NULL;
//   TString name="";

//   name=shortName;
//   name+=event->GetFiredTriggerClasses();
//   name.ToUpper();
//   curHist=(THnSparseF*)fArrayDz->FindObject(name);
//   if(!curHist){
//     curHist=new THnSparseF(name,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//     fArrayDz->AddLast(curHist);
//   }
//   curHist->Fill(vecDrift);
	  
//   name=shortName;
//   name+="ALL";
//   name.ToUpper();
//   curHist=(THnSparseF*)fArrayDz->FindObject(name);
//   if(!curHist){
//     curHist=new THnSparseF(name,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//     fArrayDz->AddLast(curHist);
//   }
//   curHist->Fill(vecDrift);
}

void AliTPCcalibTime::ProcessCosmic(AliESDEvent *event){
  if (!event) {
    Printf("ERROR: ESD not available");
    return;
  }  
  if (event->GetTimeStamp() == 0 ) {
    Printf("no time stamp!");
    return;
  }
  
  //fd
  // Find cosmic pairs
  // 
  // Track0 is choosen in upper TPC part
  // Track1 is choosen in lower TPC part
  //
  Int_t ntracks=event->GetNumberOfTracks();
  if (ntracks==0) return;
  if (ntracks > fCutTracks) return;
  
  if (GetDebugLevel()>1) printf("Hallo world: Im here\n");
  AliESDfriend *ESDfriend=static_cast<AliESDfriend*>(event->FindListObject("AliESDfriend"));
  
  TObjArray  tpcSeeds(ntracks);
  Double_t vtxx[3]={0,0,0};
  Double_t svtxx[3]={0.000001,0.000001,100.};
  AliESDVertex vtx(vtxx,svtxx);
  //
  // track loop
  //
  for (Int_t i=0;i<ntracks;++i) {
    AliESDtrack *track = event->GetTrack(i);
    
    const AliExternalTrackParam * trackIn = track->GetInnerParam();
    const AliExternalTrackParam * trackOut = track->GetOuterParam();
    if (!trackIn) continue;
    if (!trackOut) continue;
    
    AliESDfriendTrack *friendTrack = ESDfriend->GetTrack(i);
    if (friendTrack) ProcessSame(track,friendTrack,event);
    if (friendTrack) ProcessAlignITS(track,friendTrack);
    if (friendTrack) ProcessAlignTRD(track,friendTrack);
    if (friendTrack) ProcessAlignTOF(track,friendTrack);
    TObject *calibObject;
    AliTPCseed *seed = 0;
    for (Int_t l=0;(calibObject=friendTrack->GetCalibObject(l));++l) if ((seed=dynamic_cast<AliTPCseed*>(calibObject))) break;
    if (seed) tpcSeeds.AddAt(seed,i);
  }
  if (ntracks<2) return;
  //
  // Find pairs
  //

  for (Int_t i=0;i<ntracks;++i) {
    AliESDtrack *track0 = event->GetTrack(i);
    // track0 - choosen upper part
    if (!track0) continue;
    if (!track0->GetOuterParam()) continue;
    if (track0->GetOuterParam()->GetAlpha()<0) continue;
    Double_t d1[3];
    track0->GetDirection(d1);    
    for (Int_t j=0;j<ntracks;++j) {
      if (i==j) continue;
      AliESDtrack *track1 = event->GetTrack(j);   
      //track 1 lower part
      if (!track1) continue;
      if (!track1->GetOuterParam()) continue;
      //      if (track1->GetOuterParam()->GetAlpha()>0) continue;
      //
      Double_t d2[3];
      track1->GetDirection(d2);
      
      AliTPCseed * seed0 = (AliTPCseed*) tpcSeeds.At(i);
      AliTPCseed * seed1 = (AliTPCseed*) tpcSeeds.At(j);
      if (! seed0) continue;
      if (! seed1) continue;
      Float_t dir = (d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2]);
      Float_t dist0  = track0->GetLinearD(0,0);
      Float_t dist1  = track1->GetLinearD(0,0);
      //
      // conservative cuts - convergence to be guarantied
      // applying before track propagation
      if (TMath::Abs(TMath::Abs(dist0)-TMath::Abs(dist1))>fCutMaxD) continue;   // distance to the 0,0
      if (TMath::Abs(dir)<TMath::Abs(fCutMinDir)) continue;               // direction vector product
      Float_t bz = AliTracker::GetBz();
      Float_t dvertex0[2];   //distance to 0,0
      Float_t dvertex1[2];   //distance to 0,0 
      track0->GetDZ(0,0,0,bz,dvertex0);
      track1->GetDZ(0,0,0,bz,dvertex1);
      if (TMath::Abs(dvertex0[1])>250) continue;
      if (TMath::Abs(dvertex1[1])>250) continue;
      //
      //
      //
      Float_t dmax = TMath::Max(TMath::Abs(dist0),TMath::Abs(dist1));
      AliExternalTrackParam param0(*track0);
      AliExternalTrackParam param1(*track1);
      //
      // Propagate using Magnetic field and correct fo material budget
      //
      AliTracker::PropagateTrackTo(&param0,dmax+1,0.0005,3,kTRUE);
      AliTracker::PropagateTrackTo(&param1,dmax+1,0.0005,3,kTRUE);
      //
      // Propagate rest to the 0,0 DCA - z should be ignored
      //
      //Bool_t b0 = ;
      param0.PropagateToDCA(&vtx,bz,1000);
      //Bool_t b1 = 
      param1.PropagateToDCA(&vtx,bz,1000);
      param0.GetDZ(0,0,0,bz,dvertex0);
      param1.GetDZ(0,0,0,bz,dvertex1);
      Double_t xyz0[3];
      Double_t xyz1[3];
      param0.GetXYZ(xyz0);
      param1.GetXYZ(xyz1);
      Bool_t isPair = IsPair(&param0,&param1);
      Bool_t isCross = IsCross(track0, track1);
      Bool_t isSame = IsSame(track0, track1);

      THnSparse* hist=new THnSparseF("","HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
      TString shortName=hist->ClassName();
      shortName+="_MEAN_VDRIFT_COSMICS_";
      delete hist;
      hist=NULL;

      if((isSame) || (isCross && isPair)){
	if (track0->GetTPCNcls()+ track1->GetTPCNcls()> 80) {
	  fDz = param0.GetZ() - param1.GetZ();
	  if(track0->GetOuterParam()->GetZ()<0) fDz=-fDz;
	  TTimeStamp tstamp(fTime);
	  Double_t ptrelative0 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
	  Double_t ptrelative1 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
	  Double_t vecDrift[4]={fTime,(ptrelative0+ptrelative1)/2.0,fDz/500.0,event->GetRunNumber()};
          THnSparse* curHist=NULL;
          TString name="";

          name=shortName;
	  name+=event->GetFiredTriggerClasses();
	  name.ToUpper();
	  curHist=(THnSparseF*)fArrayDz->FindObject(name);
	  if(!curHist){
	    curHist=new THnSparseF(name,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
	    fArrayDz->AddLast(curHist);
	  }
//	  curHist=(THnSparseF*)(fMapDz->GetValue(event->GetFiredTriggerClasses()));
//	  if(!curHist){
//	    curHist=new THnSparseF(event->GetFiredTriggerClasses(),"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//	    fMapDz->Add(new TObjString(event->GetFiredTriggerClasses()),curHist);
//	  }
	  curHist->Fill(vecDrift);
	  
          name=shortName;
	  name+="ALL";
	  name.ToUpper();
	  curHist=(THnSparseF*)fArrayDz->FindObject(name);
	  if(!curHist){
	    curHist=new THnSparseF(name,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
	    fArrayDz->AddLast(curHist);
	  }
//	  curHist=(THnSparseF*)(fMapDz->GetValue("all"));
//	  if(!curHist){
//	    curHist=new THnSparseF("all","HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//	    fMapDz->Add(new TObjString("all"),curHist);
//	  }
	  curHist->Fill(vecDrift);
	}
      }
      TTreeSRedirector *cstream = GetDebugStreamer();
      if (fStreamLevel>0){
        if (cstream){
        (*cstream)<<"trackInfo"<<
	  "tr0.="<<track0<<
	  "tr1.="<<track1<<
	  "p0.="<<&param0<<
	  "p1.="<<&param1<<
	  "isPair="<<isPair<<
	  "isCross="<<isCross<<
	  "isSame="<<isSame<<
	  "fDz="<<fDz<<
	  "fRun="<<fRun<<
	  "fTime="<<fTime<<
	  "\n";
	}
      }
    } // end 2nd order loop        
  } // end 1st order loop
  
  if (fStreamLevel>0){
    TTreeSRedirector *cstream = GetDebugStreamer();
    if (cstream){
      TTimeStamp tstamp(fTime);
      Float_t valuePressure0 = AliTPCcalibDB::GetPressure(tstamp,fRun,0);
      Float_t valuePressure1 = AliTPCcalibDB::GetPressure(tstamp,fRun,1);
      Double_t ptrelative0   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
      Double_t ptrelative1   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
      Double_t temp0         = AliTPCcalibDB::GetTemperature(tstamp,fRun,0);
      Double_t temp1         = AliTPCcalibDB::GetTemperature(tstamp,fRun,1);
      TVectorD vecGoofie(20);
      AliDCSSensorArray* goofieArray = AliTPCcalibDB::Instance()->GetGoofieSensors(fRun);
      if (goofieArray){
	for (Int_t isensor=0; isensor<goofieArray->NumSensors();isensor++){
	  AliDCSSensor *gsensor = goofieArray->GetSensor(isensor);
	  if (gsensor) vecGoofie[isensor]=gsensor->GetValue(tstamp);
	}
      }
      (*cstream)<<"timeInfo"<<
	"run="<<fRun<<              //  run number
	"event="<<fEvent<<          //  event number
	"time="<<fTime<<            //  time stamp of event
	"trigger="<<fTrigger<<      //  trigger
	"mag="<<fMagF<<             //  magnetic field
	// Environment values
	"press0="<<valuePressure0<<
	"press1="<<valuePressure1<<
	"pt0="<<ptrelative0<<
	"pt1="<<ptrelative1<<
	"temp0="<<temp0<<
	"temp1="<<temp1<<
	"vecGoofie.=<<"<<&vecGoofie<<
	//
	// accumulated values
	//
	"fDz="<<fDz<<          //! current delta z
	"trigger="<<event->GetFiredTriggerClasses()<<
	"\n";
    }
  }
  printf("Trigger: %s\n",event->GetFiredTriggerClasses().Data());
}

void AliTPCcalibTime::ProcessBeam(AliESDEvent */*event*/){
}

void AliTPCcalibTime::Analyze(){}

THnSparse* AliTPCcalibTime::GetHistoDrift(const char* name){
  TIterator* iterator = fArrayDz->MakeIterator();
  iterator->Reset();
  TString newName=name;
  newName.ToUpper();
  THnSparse* newHist=new THnSparseF(newName,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
  THnSparse* addHist=NULL;
  while((addHist=(THnSparseF*)iterator->Next())){
  if(!addHist) continue;
    TString histName=addHist->GetName();
    if(!histName.Contains(newName)) continue;
    addHist->Print();
    newHist->Add(addHist);
  }
  return newHist;
}

TObjArray* AliTPCcalibTime::GetHistoDrift(){
  return fArrayDz;
}

TGraphErrors* AliTPCcalibTime::GetGraphDrift(const char* name){
  THnSparse* histoDrift=GetHistoDrift(name);
  TGraphErrors* graphDrift=NULL;
  if(histoDrift){
    graphDrift=FitSlices(histoDrift,2,0,400,100,0.05,0.95, kTRUE);
    TString end=histoDrift->GetName();
    Int_t pos=end.Index("_");
    end=end(pos,end.Capacity()-pos);
    TString graphName=graphDrift->ClassName();
    graphName+=end;
    graphName.ToUpper();
    graphDrift->SetName(graphName);
  }
  return graphDrift;
}

TObjArray* AliTPCcalibTime::GetGraphDrift(){
  TObjArray* arrayGraphDrift=new TObjArray();
  TIterator* iterator=fArrayDz->MakeIterator();
  iterator->Reset();
  THnSparse* addHist=NULL;
  while((addHist=(THnSparseF*)iterator->Next())) arrayGraphDrift->AddLast(GetGraphDrift(addHist->GetName()));
  return arrayGraphDrift;
}

AliSplineFit* AliTPCcalibTime::GetFitDrift(const char* name){
  TGraph* graphDrift=GetGraphDrift(name);
  AliSplineFit* fitDrift=NULL;
  if(graphDrift && graphDrift->GetN()){
    fitDrift=new AliSplineFit();
    fitDrift->SetGraph(graphDrift);
    fitDrift->SetMinPoints(graphDrift->GetN()+1);
    fitDrift->InitKnots(graphDrift,2,0,0.001);
    fitDrift->SplineFit(0);
    TString end=graphDrift->GetName();
    Int_t pos=end.Index("_");
    end=end(pos,end.Capacity()-pos);
    TString fitName=fitDrift->ClassName();
    fitName+=end;
    fitName.ToUpper();
    //fitDrift->SetName(fitName);
    delete graphDrift;
    graphDrift=NULL;
  }
  return fitDrift;
}

//TObjArray* AliTPCcalibTime::GetFitDrift(){
//  TObjArray* arrayFitDrift=new TObjArray();
//  TIterator* iterator = fArrayDz->MakeIterator();
//  iterator->Reset();
//  THnSparse* addHist=NULL;
//  while((addHist=(THnSparseF*)iterator->Next())) arrayFitDrift->AddLast(GetFitDrift(addHist->GetName()));
//  return arrayFitDrift;
//}

Long64_t AliTPCcalibTime::Merge(TCollection *li) {
  TIterator* iter = li->MakeIterator();
  AliTPCcalibTime* cal = 0;

  while ((cal = (AliTPCcalibTime*)iter->Next())) {
    if (!cal->InheritsFrom(AliTPCcalibTime::Class())) {
      Error("Merge","Attempt to add object of class %s to a %s", cal->ClassName(), this->ClassName());
      return -1;
    }
    for (Int_t imeas=0; imeas<3; imeas++){
      if (cal->GetHistVdriftLaserA(imeas) && cal->GetHistVdriftLaserA(imeas)){
	fHistVdriftLaserA[imeas]->Add(cal->GetHistVdriftLaserA(imeas));
	fHistVdriftLaserC[imeas]->Add(cal->GetHistVdriftLaserC(imeas));
      }
    }
    TObjArray* addArray=cal->GetHistoDrift();
    if(!addArray) return 0;
    TIterator* iterator = addArray->MakeIterator();
    iterator->Reset();
    THnSparse* addHist=NULL;
    while((addHist=(THnSparseF*)iterator->Next())){
      if(!addHist) continue;
      addHist->Print();
      THnSparse* localHist=(THnSparseF*)fArrayDz->FindObject(addHist->GetName());
      if(!localHist){
        localHist=new THnSparseF(addHist->GetName(),"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
        fArrayDz->AddLast(localHist);
      }
      localHist->Add(addHist);
    }
//    TMap * addMap=cal->GetHistoDrift();
//    if(!addMap) return 0;
//    TIterator* iterator = addMap->MakeIterator();
//    iterator->Reset();
//    TPair* addPair=0;
//    while((addPair=(TPair *)(addMap->FindObject(iterator->Next())))){
//      THnSparse* addHist=dynamic_cast<THnSparseF*>(addPair->Value());
//      if (!addHist) continue;
//      addHist->Print();
//      THnSparse* localHist=dynamic_cast<THnSparseF*>(fMapDz->GetValue(addHist->GetName()));
//      if(!localHist){
//        localHist=new THnSparseF(addHist->GetName(),"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
//        fMapDz->Add(new TObjString(addHist->GetName()),localHist);
//      }
//      localHist->Add(addHist);
//    }
    for(Int_t i=0;i<10;i++) if (cal->GetCosmiMatchingHisto(i)) fCosmiMatchingHisto[i]->Add(cal->GetCosmiMatchingHisto(i));
    //
    // Merge alignment
    //
    for (Int_t itype=0; itype<3; itype++){
      //
      //
      TObjArray *arr0= 0;
      TObjArray *arr1= 0;
      if (itype==0) {arr0=fAlignITSTPC; arr1=cal->fAlignITSTPC;}
      if (itype==1) {arr0=fAlignTRDTPC; arr1=cal->fAlignTRDTPC;}
      if (itype==2) {arr0=fAlignTOFTPC; arr1=cal->fAlignTOFTPC;}
      if (!arr1) continue;
      if (!arr0) arr0=new TObjArray(arr1->GetEntriesFast());
      if (arr1->GetEntriesFast()>arr0->GetEntriesFast()){
	arr0->Expand(arr1->GetEntriesFast());
      }
      for (Int_t i=0;i<arr1->GetEntriesFast(); i++){
	AliRelAlignerKalman *kalman1 = (AliRelAlignerKalman *)arr1->UncheckedAt(i);
	AliRelAlignerKalman *kalman0 = (AliRelAlignerKalman *)arr0->UncheckedAt(i);
	if (!kalman1)  continue;
	if (!kalman0) {arr0->AddAt(new AliRelAlignerKalman(*kalman1),i); continue;}
	kalman0->SetRejectOutliers(kFALSE);
	kalman0->Merge(kalman1);
      }
    }

  }
  return 0;
}

Bool_t  AliTPCcalibTime::IsPair(AliExternalTrackParam *tr0, AliExternalTrackParam *tr1){
  /*
  // 0. Same direction - OPOSITE  - cutDir +cutT    
  TCut cutDir("cutDir","dir<-0.99")
  // 1. 
  TCut cutT("cutT","abs(Tr1.fP[3]+Tr0.fP[3])<0.03")
  //
  // 2. The same rphi 
  TCut cutD("cutD","abs(Tr0.fP[0]+Tr1.fP[0])<5")
  //
  TCut cutPt("cutPt","abs(Tr1.fP[4]+Tr0.fP[4])<1&&abs(Tr0.fP[4])+abs(Tr1.fP[4])<10");  
  // 1/Pt diff cut
  */
  const Double_t *p0 = tr0->GetParameter();
  const Double_t *p1 = tr1->GetParameter();
  fCosmiMatchingHisto[0]->Fill(p0[0]+p1[0]);
  fCosmiMatchingHisto[1]->Fill(p0[1]-p1[1]);
  fCosmiMatchingHisto[2]->Fill(tr1->GetAlpha()-tr0->GetAlpha()+TMath::Pi());
  fCosmiMatchingHisto[3]->Fill(p0[3]+p1[3]);
  fCosmiMatchingHisto[4]->Fill(p0[4]+p1[4]);
  
  if (TMath::Abs(p0[3]+p1[3])>fCutTheta) return kFALSE;
  if (TMath::Abs(p0[0]+p1[0])>fCutMaxD)  return kFALSE;
  if (TMath::Abs(p0[1]-p1[1])>fCutMaxDz)  return kFALSE;
  Double_t d0[3], d1[3];
  tr0->GetDirection(d0);    
  tr1->GetDirection(d1);       
  if (d0[0]*d1[0] + d0[1]*d1[1] + d0[2]*d1[2] >fCutMinDir) return kFALSE;

  fCosmiMatchingHisto[5]->Fill(p0[0]+p1[0]);
  fCosmiMatchingHisto[6]->Fill(p0[1]-p1[1]);
  fCosmiMatchingHisto[7]->Fill(tr1->GetAlpha()-tr0->GetAlpha()+TMath::Pi());
  fCosmiMatchingHisto[8]->Fill(p0[3]+p1[3]);
  fCosmiMatchingHisto[9]->Fill(p0[4]+p1[4]);

  return kTRUE;  
}
Bool_t AliTPCcalibTime::IsCross(AliESDtrack *tr0, AliESDtrack *tr1){
  return  tr0->GetOuterParam()->GetZ()*tr1->GetOuterParam()->GetZ()<0 && tr0->GetInnerParam()->GetZ()*tr1->GetInnerParam()->GetZ()<0 && tr0->GetOuterParam()->GetZ()*tr0->GetInnerParam()->GetZ()>0 && tr1->GetOuterParam()->GetZ()*tr1->GetInnerParam()->GetZ()>0;
}

Bool_t AliTPCcalibTime::IsSame(AliESDtrack *tr0, AliESDtrack *tr1){
  // 
  // track crossing the CE
  // 0. minimal number of clusters 
  // 1. Same sector +-1
  // 2. Inner and outer track param on opposite side
  // 3. Outer and inner track parameter close each to other
  // 3. 
  Bool_t result=kTRUE;
  //
  // inner and outer on opposite sides in z
  //
  const Int_t knclCut0  = 30;
  const Double_t kalphaCut = 0.4;
  //
  // 0. minimal number of clusters
  //
  if (tr0->GetTPCNcls()<knclCut0) return kFALSE;
  if (tr1->GetTPCNcls()<knclCut0) return kFALSE;
  //
  // 1. alpha cut - sector+-1
  //
  if (TMath::Abs(tr0->GetOuterParam()->GetAlpha()-tr1->GetOuterParam()->GetAlpha())>kalphaCut) return kFALSE;
  //
  // 2. Z crossing
  //
  if (tr0->GetOuterParam()->GetZ()*tr0->GetInnerParam()->GetZ()>0) result&=kFALSE;
  if (tr1->GetOuterParam()->GetZ()*tr1->GetInnerParam()->GetZ()>0) result&=kFALSE;
  if (result==kFALSE){
    return result;
  }
  //
  //
  const Double_t *p0I = tr0->GetInnerParam()->GetParameter();
  const Double_t *p1I = tr1->GetInnerParam()->GetParameter();
  const Double_t *p0O = tr0->GetOuterParam()->GetParameter();
  const Double_t *p1O = tr1->GetOuterParam()->GetParameter();
  //
  if (TMath::Abs(p0I[0]-p1I[0])>fCutMaxD)  result&=kFALSE;
  if (TMath::Abs(p0I[1]-p1I[1])>fCutMaxDz) result&=kFALSE;
  if (TMath::Abs(p0I[2]-p1I[2])>fCutTheta) result&=kFALSE;
  if (TMath::Abs(p0I[3]-p1I[3])>fCutTheta) result&=kFALSE;
  if (TMath::Abs(p0O[0]-p1O[0])>fCutMaxD)  result&=kFALSE;
  if (TMath::Abs(p0O[1]-p1O[1])>fCutMaxDz) result&=kFALSE;
  if (TMath::Abs(p0O[2]-p1O[2])>fCutTheta) result&=kFALSE;
  if (TMath::Abs(p0O[3]-p1O[3])>fCutTheta) result&=kFALSE;
  if (result==kTRUE){
    result=kTRUE; // just to put break point here
  }
  return result;
}


void  AliTPCcalibTime::ProcessSame(AliESDtrack* track, AliESDfriendTrack *friendTrack,AliESDEvent *event){
  //
  // Process  TPC tracks crossing CE
  //
  // 0. Select only track crossing the CE
  // 1. Cut on the track length
  // 2. Refit the terack on A and C side separatelly
  // 3. Fill time histograms
  const Int_t kMinNcl=100;
  const Int_t kMinNclS=25;  // minimul number of clusters on the sides
  if (!friendTrack->GetTPCOut()) return;
  //
  // 0. Select only track crossing the CE
  //
  if (track->GetInnerParam()->GetZ()*friendTrack->GetTPCOut()->GetZ()>0) return;
  //
  // 1. cut on track length
  //
  if (track->GetTPCNcls()<kMinNcl) return;
  //
  // 2. Refit track sepparatel on A and C side
  //
  TObject *calibObject;
  AliTPCseed *seed = 0;
  for (Int_t l=0;(calibObject=friendTrack->GetCalibObject(l));++l) {
    if ((seed=dynamic_cast<AliTPCseed*>(calibObject))) break;
  }
  if (!seed) return;
  //
  AliExternalTrackParam trackIn(*track->GetInnerParam());
  AliExternalTrackParam trackOut(*track->GetOuterParam());
  Double_t cov[3]={0.01,0.,0.01}; //use the same errors
  Double_t xyz[3]={0,0.,0.0};  
  Double_t bz   =0;
  Int_t nclIn=0,nclOut=0;
  trackIn.ResetCovariance(30.);
  trackOut.ResetCovariance(30.);
  //
  //2.a Refit inner
  // 
  for (Int_t irow=0;irow<159;irow++) {
    AliTPCclusterMI *cl=seed->GetClusterPointer(irow);
    if (!cl) continue;
    if (cl->GetX()<80) continue;
    if (track->GetInnerParam()->GetZ()<0 &&(cl->GetDetector()%36)<18) break;
    if (track->GetInnerParam()->GetZ()>0 &&(cl->GetDetector()%36)>=18) break;
    Int_t sector = cl->GetDetector();
    Float_t dalpha = TMath::DegToRad()*(sector%18*20.+10.)-trackIn.GetAlpha();
    if (TMath::Abs(dalpha)>0.01){
      if (!trackIn.Rotate(TMath::DegToRad()*(sector%18*20.+10.))) break;
    }
    Double_t r[3]={cl->GetX(),cl->GetY(),cl->GetZ()};
    trackIn.GetXYZ(xyz);
    bz = AliTracker::GetBz(xyz);
    if (!trackIn.PropagateTo(r[0],bz)) break;
    nclIn++;
    trackIn.Update(&r[1],cov);    
  }
  //
  //2.b Refit outer
  // 
  for (Int_t irow=159;irow>0;irow--) {
    AliTPCclusterMI *cl=seed->GetClusterPointer(irow);
    if (!cl) continue;
    if (cl->GetX()<80) continue;
    if (cl->GetZ()*track->GetOuterParam()->GetZ()<0) break;
    if (friendTrack->GetTPCOut()->GetZ()<0 &&(cl->GetDetector()%36)<18) break;
    if (friendTrack->GetTPCOut()->GetZ()>0 &&(cl->GetDetector()%36)>=18) break;
    Int_t sector = cl->GetDetector();
    Float_t dalpha = TMath::DegToRad()*(sector%18*20.+10.)-trackOut.GetAlpha();
    if (TMath::Abs(dalpha)>0.01){
      if (!trackOut.Rotate(TMath::DegToRad()*(sector%18*20.+10.))) break;
    }
    Double_t r[3]={cl->GetX(),cl->GetY(),cl->GetZ()};
    trackOut.GetXYZ(xyz);
    bz = AliTracker::GetBz(xyz);
    if (!trackOut.PropagateTo(r[0],bz)) break;
    nclOut++;
    trackOut.Update(&r[1],cov);    
  }
  trackOut.Rotate(trackIn.GetAlpha());
  Double_t meanX = (trackIn.GetX()+trackOut.GetX())*0.5;
  trackIn.PropagateTo(meanX,bz); 
  trackOut.PropagateTo(meanX,bz); 
  TTreeSRedirector *cstream = GetDebugStreamer();
  if (cstream){
    TVectorD gxyz(3);
    trackIn.GetXYZ(gxyz.GetMatrixArray());
    TTimeStamp tstamp(fTime);
    Double_t ptrelative0 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
    Double_t ptrelative1 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
    (*cstream)<<"tpctpc"<<
      "run="<<fRun<<              //  run number
      "event="<<fEvent<<          //  event number
      "time="<<fTime<<            //  time stamp of event
      "trigger="<<fTrigger<<      //  trigger
      "mag="<<fMagF<<             //  magnetic field
      "ptrel0.="<<ptrelative0<<
      "ptrel1.="<<ptrelative1<<
      //
      "xyz.="<<&gxyz<<             // global position
      "tIn.="<<&trackIn<<         // refitterd track in 
      "tOut.="<<&trackOut<<       // refitter track out
      "nclIn="<<nclIn<<           // 
      "nclOut="<<nclOut<<         //
      "\n";  
  }
  //
  // 3. Fill time histograms
  // Debug stremaer expression
  // chainTPCTPC->Draw("(tIn.fP[1]-tOut.fP[1])*sign(-tIn.fP[3]):tIn.fP[3]","min(nclIn,nclOut)>30","")
  if (TMath::Min(nclIn,nclOut)>kMinNclS){
    fDz = trackOut.GetZ()-trackIn.GetZ();
    if (trackOut.GetTgl()<0) fDz*=-1.;
    TTimeStamp tstamp(fTime);
    Double_t ptrelative0 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
    Double_t ptrelative1 = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
    Double_t vecDrift[4]={fTime,(ptrelative0+ptrelative1)/2.0,fDz/500.0,event->GetRunNumber()};
    //
    // fill histograms per trigger class and itegrated
    //
    THnSparse* curHist=NULL;
    for (Int_t itype=0; itype<2; itype++){
      TString name="MEAN_VDRIFT_CROSS_";  
      if (itype==0){
	name+=event->GetFiredTriggerClasses();
	name.ToUpper();
      }else{
	name+="ALL";
      }
      curHist=(THnSparseF*)fArrayDz->FindObject(name);
      if(!curHist){
	curHist=new THnSparseF(name,"HistVdrift;time;p/T ratio;Vdrift;run",4,fBinsVdrift,fXminVdrift,fXmaxVdrift);
	fArrayDz->AddLast(curHist);
      }
      curHist->Fill(vecDrift);
    }
  }

}

void  AliTPCcalibTime::ProcessAlignITS(AliESDtrack* track, AliESDfriendTrack *friendTrack){
  //
  // Process track - Update TPC-ITS alignment
  // Updates: 
  // 0. Apply standartd cuts 
  // 1. Recalucluate the current statistic median/RMS
  // 2. Apply median+-rms cut
  // 3. Update kalman filter
  //
  const Int_t    kMinTPC  = 80;    // minimal number of TPC cluster
  const Int_t    kMinITS  = 3;     // minimal number of ITS cluster
  const Double_t kMinZ    = 10;    // maximal dz distance
  const Double_t kMaxDy   = 1.;    // maximal dy distance
  const Double_t kMaxAngle= 0.01;  // maximal angular distance
  const Double_t kSigmaCut= 5;     // maximal sigma distance to median
  const Double_t kVdErr   = 0.1;  // initial uncertainty of the vd correction 
  const Double_t kVdYErr  = 0.05;  // initial uncertainty of the vd correction 
  const Double_t kOutCut  = 1.0;   // outlyer cut in AliRelAlgnmentKalman
  const  Int_t     kN=500;         // deepnes of history
  static Int_t     kglast=0;
  static Double_t* kgdP[4]={new Double_t[kN], new Double_t[kN], new Double_t[kN], new Double_t[kN]};
  /*
    0. Standrd cuts:
    TCut cut="abs(pTPC.fP[2]-pITS.fP[2])<0.01&&abs(pTPC.fP[3]-pITS.fP[3])<0.01&&abs(pTPC.fP[2]-pITS.fP[2])<1";
  */
  //
  // 0. Apply standard cuts
  //
  Int_t dummycl[1000];
  if (track->GetITSclusters(dummycl)<kMinITS) return;  // minimal amount of clusters
  if (track->GetTPCNcls()<kMinTPC) return;  // minimal amount of clusters cut
  if (!friendTrack->GetITSOut()) return;  
  if (!track->GetInnerParam())   return;
  if (!track->GetOuterParam())   return;
  // exclude crossing track
  if (track->GetOuterParam()->GetZ()*track->GetInnerParam()->GetZ()<0)   return;
  if (TMath::Abs(track->GetInnerParam()->GetZ())<kMinZ)   return;
  //
  AliExternalTrackParam &pTPC=(AliExternalTrackParam &)(*(track->GetInnerParam()));
  AliExternalTrackParam pITS(*(friendTrack->GetITSOut()));
  pITS.Rotate(pTPC.GetAlpha());
  pITS.PropagateTo(pTPC.GetX(),fMagF);
  if (TMath::Abs(pITS.GetY()-pTPC.GetY())    >kMaxDy)    return;
  if (TMath::Abs(pITS.GetSnp()-pTPC.GetSnp())>kMaxAngle) return;
  if (TMath::Abs(pITS.GetTgl()-pTPC.GetTgl())>kMaxAngle) return;
  //
  // 1. Update median and RMS info
  //
  TVectorD vecDelta(4),vecMedian(4), vecRMS(4);
  TVectorD vecDeltaN(5);
  Double_t sign=(pITS.GetParameter()[1]>0)? 1.:-1.;
  vecDelta[4]=0;
  for (Int_t i=0;i<4;i++){
    vecDelta[i]=(pITS.GetParameter()[i]-pTPC.GetParameter()[i])*sign;
    kgdP[i][kglast%kN]=vecDelta[i];
  }
  kglast=(kglast+1);
  Int_t entries=(kglast<kN)?kglast:kN;
  for (Int_t i=0;i<4;i++){
    vecMedian[i] = TMath::Median(entries,kgdP[i]);
    vecRMS[i]    = TMath::RMS(entries,kgdP[i]);
    vecDeltaN[i] = 0;
    if (vecRMS[i]>0.){
      vecDeltaN[i] = (vecDelta[i]-vecMedian[i])/vecRMS[i];
      vecDeltaN[4]+= TMath::Abs(vecDeltaN[i]);  //sum of abs residuals
    }
  }
  //
  // 2. Apply median+-rms cut
  //
  if (kglast<3)  return;   //median and RMS to be defined
  if ( vecDeltaN[4]/4.>kSigmaCut) return;
  //
  // 3. Update alignment
  //
  Int_t htime = fTime/3600; //time in hours
  if (fAlignITSTPC->GetEntries()<htime){
    fAlignITSTPC->Expand(htime*2+20);
  }
  AliRelAlignerKalman* align =  (AliRelAlignerKalman*)fAlignITSTPC->At(htime);
  if (!align){
    // make Alignment object if doesn't exist
    align=new AliRelAlignerKalman(); 
    align->SetRunNumber(fRun);
    (*align->GetStateCov())(6,6)=kVdErr*kVdErr;
    (*align->GetStateCov())(8,8)=kVdYErr*kVdYErr;
    align->SetOutRejSigma(kOutCut+kOutCut*kN);
    align->SetRejectOutliers(kFALSE);

    align->SetTPCvd(AliTPCcalibDB::Instance()->GetParameters()->GetDriftV()/1000000.);
    align->SetMagField(fMagF); 
    fAlignITSTPC->AddAt(align,htime);
  }
  align->AddTrackParams(&pITS,&pTPC);
  align->SetTimeStamp(fTime);
  align->SetRunNumber(fRun );
  //
  Int_t nupdates=align->GetNUpdates();
  align->SetOutRejSigma(kOutCut+kOutCut*kN/Double_t(nupdates));
  align->SetRejectOutliers(kFALSE);
  TTreeSRedirector *cstream = GetDebugStreamer();  
  if (cstream && align->GetState() && align->GetState()->GetNrows()>2 ){
    TTimeStamp tstamp(fTime);
    Float_t valuePressure0 = AliTPCcalibDB::GetPressure(tstamp,fRun,0);
    Float_t valuePressure1 = AliTPCcalibDB::GetPressure(tstamp,fRun,1);
    Double_t ptrelative0   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
    Double_t ptrelative1   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
    Double_t temp0         = AliTPCcalibDB::GetTemperature(tstamp,fRun,0);
    Double_t temp1         = AliTPCcalibDB::GetTemperature(tstamp,fRun,1);
    TVectorD vecGoofie(20);
    AliDCSSensorArray* goofieArray = AliTPCcalibDB::Instance()->GetGoofieSensors(fRun);
    if (goofieArray){
      for (Int_t isensor=0; isensor<goofieArray->NumSensors();isensor++){
	AliDCSSensor *gsensor = goofieArray->GetSensor(isensor);
	if (gsensor) vecGoofie[isensor]=gsensor->GetValue(tstamp);
      }
    }
    TVectorD gpTPC(3), gdTPC(3);
    TVectorD gpITS(3), gdITS(3);
    pTPC.GetXYZ(gpTPC.GetMatrixArray());
    pTPC.GetDirection(gdTPC.GetMatrixArray());
    pITS.GetXYZ(gpITS.GetMatrixArray());
    pITS.GetDirection(gdITS.GetMatrixArray());
    (*cstream)<<"itstpc"<<
      "run="<<fRun<<              //  run number
      "event="<<fEvent<<          //  event number
      "time="<<fTime<<            //  time stamp of event
      "trigger="<<fTrigger<<      //  trigger
      "mag="<<fMagF<<             //  magnetic field
      // Environment values
      "press0="<<valuePressure0<<
      "press1="<<valuePressure1<<
      "pt0="<<ptrelative0<<
      "pt1="<<ptrelative1<<
      "temp0="<<temp0<<
      "temp1="<<temp1<<
      "vecGoofie.="<<&vecGoofie<<
      //
      "nmed="<<kglast<<        // number of entries to define median and RMS
      "vMed.="<<&vecMedian<<    // median of deltas
      "vRMS.="<<&vecRMS<<       // rms of deltas
      "vDelta.="<<&vecDelta<<   // delta in respect to median
      "vDeltaN.="<<&vecDeltaN<< // normalized delta in respect to median
      "t.="<<track<<            // ful track - find proper cuts
      "a.="<<align<<            // current alignment
      "pITS.="<<&pITS<<         // track param ITS
      "pTPC.="<<&pTPC<<         // track param TPC
      "gpTPC.="<<&gpTPC<<       // global position  TPC
      "gdTPC.="<<&gdTPC<<       // global direction TPC
      "gpITS.="<<&gpITS<<       // global position  ITS
      "gdITS.="<<&gdITS<<       // global position  ITS
      "\n";
  }
}




void  AliTPCcalibTime::ProcessAlignTRD(AliESDtrack* track, AliESDfriendTrack *friendTrack){
  //
  // Process track - Update TPC-TRD alignment
  // Updates: 
  // 0. Apply standartd cuts 
  // 1. Recalucluate the current statistic median/RMS
  // 2. Apply median+-rms cut
  // 3. Update kalman filter
  //
  const Int_t    kMinTPC  = 80;    // minimal number of TPC cluster
  const Int_t    kMinTRD  = 50;    // minimal number of TRD cluster
  const Double_t kMinZ    = 20;    // maximal dz distance
  const Double_t kMaxDy   = 1.;    // maximal dy distance
  const Double_t kMaxAngle= 0.01;  // maximal angular distance
  const Double_t kSigmaCut= 5;     // maximal sigma distance to median
  const Double_t kVdErr   = 0.1;  // initial uncertainty of the vd correction 
  const Double_t kVdYErr  = 0.05;  // initial uncertainty of the vd correction 
  const Double_t kOutCut  = 1.0;   // outlyer cut in AliRelAlgnmentKalman
  const  Int_t     kN=500;         // deepnes of history
  static Int_t     kglast=0;
  static Double_t* kgdP[4]={new Double_t[kN], new Double_t[kN], new Double_t[kN], new Double_t[kN]};
  //
  // 0. Apply standard cuts
  //
  Int_t dummycl[1000];
  if (track->GetTRDclusters(dummycl)<kMinTRD) return;  // minimal amount of clusters
  if (track->GetTPCNcls()<kMinTPC) return;  // minimal amount of clusters cut
  if (!friendTrack->GetTRDIn()) return;  
  if (!track->GetInnerParam())   return;
  if (!track->GetOuterParam())   return;
  // exclude crossing track
  if (track->GetOuterParam()->GetZ()*track->GetInnerParam()->GetZ()<0)   return;
  if (TMath::Abs(track->GetInnerParam()->GetZ())<kMinZ)   return;
  //
  AliExternalTrackParam &pTPC=(AliExternalTrackParam &)(*(track->GetOuterParam()));
  AliExternalTrackParam pTRD(*(friendTrack->GetTRDIn()));
  pTRD.Rotate(pTPC.GetAlpha());
  pTRD.PropagateTo(pTPC.GetX(),fMagF);
  ((Double_t*)pTRD.GetCovariance())[2]+=3.*3.;   // increas sys errors
  ((Double_t*)pTRD.GetCovariance())[9]+=0.1*0.1; // increse sys errors

  if (TMath::Abs(pTRD.GetY()-pTPC.GetY())    >kMaxDy)    return;
  if (TMath::Abs(pTRD.GetSnp()-pTPC.GetSnp())>kMaxAngle) return;
  if (TMath::Abs(pTRD.GetTgl()-pTPC.GetTgl())>kMaxAngle) return;
  //
  // 1. Update median and RMS info
  //
  TVectorD vecDelta(4),vecMedian(4), vecRMS(4);
  TVectorD vecDeltaN(5);
  Double_t sign=(pTRD.GetParameter()[1]>0)? 1.:-1.;
  vecDelta[4]=0;
  for (Int_t i=0;i<4;i++){
    vecDelta[i]=(pTRD.GetParameter()[i]-pTPC.GetParameter()[i])*sign;
    kgdP[i][kglast%kN]=vecDelta[i];
  }
  kglast=(kglast+1);
  Int_t entries=(kglast<kN)?kglast:kN;
  for (Int_t i=0;i<4;i++){
    vecMedian[i] = TMath::Median(entries,kgdP[i]);
    vecRMS[i]    = TMath::RMS(entries,kgdP[i]);
    vecDeltaN[i] = 0;
    if (vecRMS[i]>0.){
      vecDeltaN[i] = (vecDelta[i]-vecMedian[i])/vecRMS[i];
      vecDeltaN[4]+= TMath::Abs(vecDeltaN[i]);  //sum of abs residuals
    }
  }
  //
  // 2. Apply median+-rms cut
  //
  if (kglast<3)  return;   //median and RMS to be defined
  if ( vecDeltaN[4]/4.>kSigmaCut) return;
  //
  // 3. Update alignment
  //
  Int_t htime = fTime/3600; //time in hours
  if (fAlignTRDTPC->GetEntries()<htime){
    fAlignTRDTPC->Expand(htime*2+20);
  }
  AliRelAlignerKalman* align =  (AliRelAlignerKalman*)fAlignTRDTPC->At(htime);
  if (!align){
    // make Alignment object if doesn't exist
    align=new AliRelAlignerKalman(); 
    align->SetRunNumber(fRun);
    (*align->GetStateCov())(6,6)=kVdErr*kVdErr;
    (*align->GetStateCov())(8,8)=kVdYErr*kVdYErr;
    align->SetOutRejSigma(kOutCut+kOutCut*kN);
    align->SetRejectOutliers(kFALSE);
    align->SetTPCvd(AliTPCcalibDB::Instance()->GetParameters()->GetDriftV()/1000000.);
    align->SetMagField(fMagF); 
    fAlignTRDTPC->AddAt(align,htime);
  }
  align->AddTrackParams(&pTRD,&pTPC);
  align->SetTimeStamp(fTime);
  align->SetRunNumber(fRun );
  //
  Int_t nupdates=align->GetNUpdates();
  align->SetOutRejSigma(kOutCut+kOutCut*kN/Double_t(nupdates));
  align->SetRejectOutliers(kFALSE);
  TTreeSRedirector *cstream = GetDebugStreamer();  
  if (cstream && align->GetState() && align->GetState()->GetNrows()>2 ){
    TTimeStamp tstamp(fTime);
    Float_t valuePressure0 = AliTPCcalibDB::GetPressure(tstamp,fRun,0);
    Float_t valuePressure1 = AliTPCcalibDB::GetPressure(tstamp,fRun,1);
    Double_t ptrelative0   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
    Double_t ptrelative1   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
    Double_t temp0         = AliTPCcalibDB::GetTemperature(tstamp,fRun,0);
    Double_t temp1         = AliTPCcalibDB::GetTemperature(tstamp,fRun,1);
    TVectorD vecGoofie(20);
    AliDCSSensorArray* goofieArray = AliTPCcalibDB::Instance()->GetGoofieSensors(fRun);
    if (goofieArray){
      for (Int_t isensor=0; isensor<goofieArray->NumSensors();isensor++){
	AliDCSSensor *gsensor = goofieArray->GetSensor(isensor);
	if (gsensor) vecGoofie[isensor]=gsensor->GetValue(tstamp);
      }
    }
    TVectorD gpTPC(3), gdTPC(3);
    TVectorD gpTRD(3), gdTRD(3);
    pTPC.GetXYZ(gpTPC.GetMatrixArray());
    pTPC.GetDirection(gdTPC.GetMatrixArray());
    pTRD.GetXYZ(gpTRD.GetMatrixArray());
    pTRD.GetDirection(gdTRD.GetMatrixArray());
    (*cstream)<<"trdtpc"<<
      "run="<<fRun<<              //  run number
      "event="<<fEvent<<          //  event number
      "time="<<fTime<<            //  time stamp of event
      "trigger="<<fTrigger<<      //  trigger
      "mag="<<fMagF<<             //  magnetic field
      // Environment values
      "press0="<<valuePressure0<<
      "press1="<<valuePressure1<<
      "pt0="<<ptrelative0<<
      "pt1="<<ptrelative1<<
      "temp0="<<temp0<<
      "temp1="<<temp1<<
      "vecGoofie.="<<&vecGoofie<<
      //
      "nmed="<<kglast<<        // number of entries to define median and RMS
      "vMed.="<<&vecMedian<<    // median of deltas
      "vRMS.="<<&vecRMS<<       // rms of deltas
      "vDelta.="<<&vecDelta<<   // delta in respect to median
      "vDeltaN.="<<&vecDeltaN<< // normalized delta in respect to median
      "t.="<<track<<            // ful track - find proper cuts
      "a.="<<align<<            // current alignment
      "pTRD.="<<&pTRD<<         // track param TRD
      "pTPC.="<<&pTPC<<         // track param TPC
      "gpTPC.="<<&gpTPC<<       // global position  TPC
      "gdTPC.="<<&gdTPC<<       // global direction TPC
      "gpTRD.="<<&gpTRD<<       // global position  TRD
      "gdTRD.="<<&gdTRD<<       // global position  TRD
      "\n";
  }
}


void  AliTPCcalibTime::ProcessAlignTOF(AliESDtrack* track, AliESDfriendTrack *friendTrack){
  //
  //
  // Process track - Update TPC-TOF alignment
  // Updates: 
  // -1. Make a TOF "track"
  // 0. Apply standartd cuts 
  // 1. Recalucluate the current statistic median/RMS
  // 2. Apply median+-rms cut
  // 3. Update kalman filter
  //
  const Int_t      kMinTPC  = 80;    // minimal number of TPC cluster
  const Double_t   kMinZ    = 10;    // maximal dz distance
  const Double_t   kMaxDy   = 5.;    // maximal dy distance
  const Double_t   kMaxAngle= 0.01;  // maximal angular distance
  const Double_t   kSigmaCut= 5;     // maximal sigma distance to median
  const Double_t   kVdErr   = 0.1;  // initial uncertainty of the vd correction 
  const Double_t   kVdYErr  = 0.05;  // initial uncertainty of the vd correction 

  const Double_t   kOutCut  = 1.0;   // outlyer cut in AliRelAlgnmentKalman
  const  Int_t     kN=1000;         // deepnes of history
  static Int_t     kglast=0;
  static Double_t* kgdP[4]={new Double_t[kN], new Double_t[kN], new Double_t[kN], new Double_t[kN]};
  //
  // -1. Make a TOF track-
  //     Clusters are not in friends - use alingment points
  //
  if (track->GetTOFsignal()<=0)  return;
  if (!friendTrack->GetTPCOut()) return;
  if (!track->GetInnerParam())   return;
  if (!track->GetOuterParam())   return;
  const AliTrackPointArray *points=friendTrack->GetTrackPointArray();
  if (!points) return;
  AliExternalTrackParam pTPC(*(track->GetOuterParam()));
  AliExternalTrackParam pTOF(pTPC);
  Double_t mass = TDatabasePDG::Instance()->GetParticle("mu+")->Mass();
  Int_t npoints = points->GetNPoints();
  AliTrackPoint point;
  Int_t naccept=0;
  //
  for (Int_t ipoint=0;ipoint<npoints;ipoint++){
    points->GetPoint(point,ipoint);
    Float_t xyz[3];
    point.GetXYZ(xyz);
    Double_t r=TMath::Sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
    if (r<350)  continue;
    if (r>400)  continue;
    AliTracker::PropagateTrackToBxByBz(&pTPC,r,mass,2.,kTRUE);
    AliTracker::PropagateTrackToBxByBz(&pTPC,r,mass,0.1,kTRUE);    
    AliTrackPoint lpoint = point.Rotate(pTPC.GetAlpha());
    pTPC.PropagateTo(lpoint.GetX(),fMagF);
    pTOF=pTPC;
    ((Double_t*)pTOF.GetParameter())[0] =lpoint.GetY();
    ((Double_t*)pTOF.GetParameter())[1] =lpoint.GetZ();
    ((Double_t*)pTOF.GetCovariance())[0]+=3.*3./12.;
    ((Double_t*)pTOF.GetCovariance())[2]+=3.*3./12.;
    ((Double_t*)pTOF.GetCovariance())[5]+=0.1*0.1;
    ((Double_t*)pTOF.GetCovariance())[9]+=0.1*0.1;
    naccept++;
  }
  if (naccept==0) return;  // no tof match clusters
  //
  // 0. Apply standard cuts
  //
  if (track->GetTPCNcls()<kMinTPC) return;  // minimal amount of clusters cut
  // exclude crossing track
  if (track->GetOuterParam()->GetZ()*track->GetInnerParam()->GetZ()<0)   return;
  //
  if (TMath::Abs(pTOF.GetY()-pTPC.GetY())    >kMaxDy)    return;
  if (TMath::Abs(pTOF.GetSnp()-pTPC.GetSnp())>kMaxAngle) return;
  if (TMath::Abs(pTOF.GetTgl()-pTPC.GetTgl())>kMaxAngle) return;
  //
  // 1. Update median and RMS info
  //
  TVectorD vecDelta(4),vecMedian(4), vecRMS(4);
  TVectorD vecDeltaN(5);
  Double_t sign=(pTOF.GetParameter()[1]>0)? 1.:-1.;
  vecDelta[4]=0;
  for (Int_t i=0;i<4;i++){
    vecDelta[i]=(pTOF.GetParameter()[i]-pTPC.GetParameter()[i])*sign;
    kgdP[i][kglast%kN]=vecDelta[i];
  }
  kglast=(kglast+1);
  Int_t entries=(kglast<kN)?kglast:kN;
  Bool_t isOK=kTRUE;
  for (Int_t i=0;i<4;i++){
    vecMedian[i] = TMath::Median(entries,kgdP[i]);
    vecRMS[i]    = TMath::RMS(entries,kgdP[i]);
    vecDeltaN[i] = 0;
    if (vecRMS[i]>0.){
      vecDeltaN[i] = (vecDelta[i]-vecMedian[i])/(vecRMS[i]+1.);
      vecDeltaN[4]+= TMath::Abs(vecDeltaN[i]);  //sum of abs residuals
      if (TMath::Abs(vecDeltaN[i])>kSigmaCut) isOK=kFALSE;
    }
  }
  //
  // 2. Apply median+-rms cut
  //
  if (kglast<10)  return;   //median and RMS to be defined
  if (!isOK) return;
  //
  // 3. Update alignment
  //
  Int_t htime = fTime/3600; //time in hours
  if (fAlignTOFTPC->GetEntries()<htime){
    fAlignTOFTPC->Expand(htime*2+20);
  }
  AliRelAlignerKalman* align =  (AliRelAlignerKalman*)fAlignTOFTPC->At(htime);
  if (!align){
    // make Alignment object if doesn't exist
    align=new AliRelAlignerKalman(); 
    align->SetRunNumber(fRun);
    (*align->GetStateCov())(6,6)=kVdErr*kVdErr;
    (*align->GetStateCov())(8,8)=kVdYErr*kVdYErr;
    align->SetOutRejSigma(kOutCut+kOutCut*kN);
    align->SetRejectOutliers(kFALSE);
    align->SetTPCvd(AliTPCcalibDB::Instance()->GetParameters()->GetDriftV()/1000000.);
    align->SetMagField(fMagF); 
    fAlignTOFTPC->AddAt(align,htime);
  }
  align->AddTrackParams(&pTOF,&pTPC);
  align->SetTimeStamp(fTime);
  align->SetRunNumber(fRun );
  //
  Int_t nupdates=align->GetNUpdates();
  align->SetOutRejSigma(kOutCut+kOutCut*kN/Double_t(nupdates));
  align->SetRejectOutliers(kFALSE);
  TTreeSRedirector *cstream = GetDebugStreamer();  
  if (cstream && align->GetState() && align->GetState()->GetNrows()>2 ){
    TTimeStamp tstamp(fTime);
    Float_t valuePressure0 = AliTPCcalibDB::GetPressure(tstamp,fRun,0);
    Float_t valuePressure1 = AliTPCcalibDB::GetPressure(tstamp,fRun,1);
    Double_t ptrelative0   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,0);
    Double_t ptrelative1   = AliTPCcalibDB::GetPTRelative(tstamp,fRun,1);
    Double_t temp0         = AliTPCcalibDB::GetTemperature(tstamp,fRun,0);
    Double_t temp1         = AliTPCcalibDB::GetTemperature(tstamp,fRun,1);
    TVectorD vecGoofie(20);
    AliDCSSensorArray* goofieArray = AliTPCcalibDB::Instance()->GetGoofieSensors(fRun);
    if (goofieArray){
      for (Int_t isensor=0; isensor<goofieArray->NumSensors();isensor++){
	AliDCSSensor *gsensor = goofieArray->GetSensor(isensor);
	if (gsensor) vecGoofie[isensor]=gsensor->GetValue(tstamp);
      }
    }
    TVectorD gpTPC(3), gdTPC(3);
    TVectorD gpTOF(3), gdTOF(3);
    pTPC.GetXYZ(gpTPC.GetMatrixArray());
    pTPC.GetDirection(gdTPC.GetMatrixArray());
    pTOF.GetXYZ(gpTOF.GetMatrixArray());
    pTOF.GetDirection(gdTOF.GetMatrixArray());
    (*cstream)<<"toftpc"<<
      "run="<<fRun<<              //  run number
      "event="<<fEvent<<          //  event number
      "time="<<fTime<<            //  time stamp of event
      "trigger="<<fTrigger<<      //  trigger
      "mag="<<fMagF<<             //  magnetic field
      // Environment values
      "press0="<<valuePressure0<<
      "press1="<<valuePressure1<<
      "pt0="<<ptrelative0<<
      "pt1="<<ptrelative1<<
      "temp0="<<temp0<<
      "temp1="<<temp1<<
      "vecGoofie.="<<&vecGoofie<<
      //
      "nmed="<<kglast<<        // number of entries to define median and RMS
      "vMed.="<<&vecMedian<<    // median of deltas
      "vRMS.="<<&vecRMS<<       // rms of deltas
      "vDelta.="<<&vecDelta<<   // delta in respect to median
      "vDeltaN.="<<&vecDeltaN<< // normalized delta in respect to median
      "t.="<<track<<            // ful track - find proper cuts
      "a.="<<align<<            // current alignment
      "pTOF.="<<&pTOF<<         // track param TOF
      "pTPC.="<<&pTPC<<         // track param TPC
      "gpTPC.="<<&gpTPC<<       // global position  TPC
      "gdTPC.="<<&gdTPC<<       // global direction TPC
      "gpTOF.="<<&gpTOF<<       // global position  TOF
      "gdTOF.="<<&gdTOF<<       // global position  TOF
      "\n";
  }
}



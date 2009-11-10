/*
 The class for calculating the global (not detector specific) quality assurance.
 It reuses the following TLists from its base class 
    AliQADataMaker::fRecPointsQAList (for keeping the track residuals)
    AliQADataMaker::fESDsQAList      (for keeping global ESD QA data)
*/

#include <TPDGCode.h>
#include <TH1F.h>

#include "AliQAChecker.h"
#include "AliGlobalQADataMaker.h"
#include "AliGeomManager.h"
#include "AliESDEvent.h"
#include "AliESDv0.h"
#include "AliRawReader.h"

ClassImp(AliGlobalQADataMaker)
 
//____________________________________________________________________________ 
void AliGlobalQADataMaker::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  AliQAChecker::Instance()->Run(AliQAv1::kGLOBAL, task, list) ;  
}

//____________________________________________________________________________ 
void AliGlobalQADataMaker::InitRaws()
{
  // create Raws histograms in Raws subdir
}

//____________________________________________________________________________ 
void AliGlobalQADataMaker::InitRecPoints() {
  //------------------------------------------------------
  // This function books the histograms of *track*residuals*
  // as a part of global QA
  //------------------------------------------------------
  const Char_t *name[]={
    "hGlobalSPD1ResidualsY","SPD1ResidualsZ",
    "hGlobalSPD2ResidualsY","SPD2ResidualsZ",
    "hGlobalSDD1ResidualsY","SDD1ResidualsZ",
    "hGlobalSDD2ResidualsY","SDD2ResidualsZ",
    "hGlobalSSD1ResidualsY","SSD1ResidualsZ",
    "hGlobalSSD2ResidualsY","SSD2ResidualsZ",
    
    "hGlobalTPC1ResidualsY","TPC1ResidualsZ",
    "hGlobalTPC2ResidualsY","TPC2ResidualsZ",
    
    "hGlobalTRD1ResidualsY","TRD1ResidualsZ",
    "hGlobalTRD2ResidualsY","TRD2ResidualsZ",
    "hGlobalTRD3ResidualsY","TRD3ResidualsZ",
    "hGlobalTRD4ResidualsY","TRD4ResidualsZ",
    "hGlobalTRD5ResidualsY","TRD5ResidualsZ",
    "hGlobalTRD6ResidualsY","TRD6ResidualsZ",
    
    "hGlobalTOFResidualsY","TOFResidualsZ",
    
    "hGlobalPHOS1ResidualsY","PHOS1ResidualsZ",
    "hGlobalPHOS2ResidualsY","PHOS2ResidualsZ",
    
    "hGlobalHMPIDResidualsY","HMPIDResidualsZ",
    
    "hGlobalMUONResidualsY","MUONResidualsZ",
    
    "hGlobalEMCALResidualsY","EMCALResidualsZ"
  };
  const Char_t *title[]={
    "SPD1 residuals Y","SPD1 residuals Z",
    "SPD2 residuals Y","SPD2 residuals Z",
    "SDD1 residuals Y","SDD1 residuals Z",
    "SDD2 residuals Y","SDD2 residuals Z",
    "SSD1 residuals Y","SSD1 residuals Z",
    "SSD2 residuals Y","SSD2 residuals Z",
    
    "TPC1 residuals Y","TPC1 residuals Z",
    "TPC2 residuals Y","TPC2 residuals Z",
    
    "TRD1 residuals Y","TRD1 residuals Z",
    "TRD2 residuals Y","TRD2 residuals Z",
    "TRD3 residuals Y","TRD3 residuals Z",
    "TRD4 residuals Y","TRD4 residuals Z",
    "TRD5 residuals Y","TRD5 residuals Z",
    "TRD6 residuals Y","TRD6 residuals Z",
    
    "TOF residuals Y","TOF residuals Z",
    
    "PHOS1 residuals Y","PHOS1 residuals Z",
    "PHOS2 residuals Y","PHOS2 residuals Z",
    
    "HMPID residuals Y","HMPID residuals Z",
    
    "MUON residuals Y","MUON residuals Z",
    
    "EMCAL residuals Y","EMCAL residuals Z"
  };
  
  for (Int_t m=1; m<AliGeomManager::kLastLayer; m++) {
    Int_t i=2*m-2;
    TH1F *h=new TH1F(name[i],title[i],100,-5.,5.);
    Add2RecPointsList(h,i);    
    h=new TH1F(name[i+1],title[i+1],100,-5.,5.);
    Add2RecPointsList(h,i+1);    
  }

  Add2RecPointsList(
  new TH1F("hGlobalSSD1AbsoluteResidualsYNegZ",
           "SSD1 Absolute Residuals Y Neg Z",100,-2.,2.),40);
  Add2RecPointsList(
  new TH1F("hGlobalSSD1AbsoluteResidualsZNegZ",
           "SSD1 Absolute Residuals Z Neg Z",100,-2.,2.),41);
  Add2RecPointsList(
  new TH1F("hGlobalSSD1AbsoluteResidualsYPosZ",
           "SSD1 Absolute Residuals Y Pos Z",100,-2.,2.),42);
  Add2RecPointsList(
  new TH1F("hGlobalSSD1AbsoluteResidualsZPosZ",
           "SSD1 Absolute Residuals Z Pos Z",100,-2.,2.),43);


  Add2RecPointsList(
  new TH1F("hGlobalSSD2AbsoluteResidualsYNegZ",
           "SSD2 Absolute Residuals Y Neg Z",100,-3.,3.),44);
  Add2RecPointsList(
  new TH1F("hGlobalSSD2AbsoluteResidualsZNegZ",
           "SSD2 Absolute Residuals Z Neg Z",100,-3.,3.),45);
  Add2RecPointsList(
  new TH1F("hGlobalSSD2AbsoluteResidualsYPosZ",
           "SSD2 Absolute Residuals Y Pos Z",100,-3.,3.),46);
  Add2RecPointsList(
  new TH1F("hGlobalSSD2AbsoluteResidualsZPosZ",
           "SSD2Absolute Residuals Z Pos Z",100,-3.,3.),47);
  
}

//____________________________________________________________________________ 
void AliGlobalQADataMaker::InitESDs() {
  //------------------------------------------------------
  // This function books the ESD QA histograms
  // as a part of global QA
  //------------------------------------------------------

  {// Cluster related QA
    const Char_t *name[]={
      "hGlobalFractionAssignedClustersITS",
      "hGlobalFractionAssignedClustersTPC",
      "hGlobalFractionAssignedClustersTRD"
    };
    const Char_t *title[]={
      "Fraction of the assigned clusters in ITS",
      "Fraction of the assigned clusters in TPC",
      "Fraction of the assigned clusters in TRD"
    };
    Add2ESDsList(new TH1F(name[0],title[0],100,0.,2.),kClr0);
    Add2ESDsList(new TH1F(name[1],title[1],100,0.,2.),kClr1);
    Add2ESDsList(new TH1F(name[2],title[2],100,0.,2.),kClr2);
  }

  {// Track related QA
    const Char_t *name[]={
      "hGlobalTrackAzimuthe",                               // kTrk0
      "hGlobalTrackEta",                                    // kTrk1
      "hGlobalTPCTrackpT",                                  // kTrk2
      "hGlobalTPCITSMatchedpT",                             // kTrk3
      "hGlobalTPCTOFMatchedpT",                             // kTrk4
      "hGlobalTPCITSMatchingProbability",                   // kTrk5
      "hGlobalTPCTOFMatchingProbability"                    // kTrk6
  };
    const Char_t *title[]={
      "Track azimuthal distribution (rad)",                   // kTrk0
      "Track pseudo-rapidity distribution",                   // kTrk1
      "TPC: track momentum distribution (GeV)",               // kTrk2
      "TPC-ITS matched: track momentum distribution (GeV)",   // kTrk3
      "TPC-TOF matched: track momentum distribution (GeV)",   // kTrk4
      "TPC-ITS track-matching probability",                   // kTrk5
      "TPC-TOF track-matching probability"                    // kTrk6
    };
  Add2ESDsList(new TH1F(name[0],title[0],100, 0.,TMath::TwoPi()),kTrk0);
  Add2ESDsList(new TH1F(name[1],title[1],100,-2.00,2.00),kTrk1);
  Add2ESDsList(new TH1F(name[2],title[2],50,  0.20,5.00),kTrk2);
  Add2ESDsList(new TH1F(name[3],title[3],50,  0.20,5.00),kTrk3);
  Add2ESDsList(new TH1F(name[4],title[4],50,  0.20,5.00),kTrk4);
  Add2ESDsList(new TH1F(name[5],title[5],50,  0.20,5.00),kTrk5);
  Add2ESDsList(new TH1F(name[6],title[6],50,  0.20,5.00),kTrk6);
  }

  {// V0 related QA
    const Char_t *name[]={
      "hGlobalPromptK0sMass",
      "hGlobalOfflineK0sMass",
      "hGlobalPromptLambda0Lambda0BarMass",
      "hGlobalOfflineLambda0Lambda0BarMass"
    };
    const Char_t *title[]={
      "On-the-fly K0s mass (GeV)",
      "Offline K0s mass (GeV)",
      "On-the-fly Lambda0 + Lambda0Bar mass (GeV)",
      "Offline Lambda0 + Lambda0Bar mass (GeV)"
    };
    Add2ESDsList(new TH1F(name[0],title[0],50,  0.4477,0.5477),kK0on);
    Add2ESDsList(new TH1F(name[1],title[1],50,  0.4477,0.5477),kK0off);
    Add2ESDsList(new TH1F(name[2],title[2],50,  1.0657,1.1657),kL0on);
    Add2ESDsList(new TH1F(name[3],title[3],50,  1.0657,1.1657),kL0off);
  }

  {// PID related QA
  const Char_t *name[]={
    "hGlobalITSdEdx",
    "hGlobalTPCdEdx",
    "hGlobalTOFTrackingvsMeasured"
  };
    const Char_t *title[]={
      "ITS: dEdx (ADC) for particles with momentum 0.4 - 0.5 (GeV)",
      "TPC: dEdx (ADC) for particles with momentum 0.4 - 0.5 (GeV)",
      "TOF: tracking - measured (ps)"
    };
    Add2ESDsList(new TH1F(name[0],title[0],50,0.00,200.),kPid0);
    Add2ESDsList(new TH1F(name[1],title[1],50,0.00,100.),kPid1);
    Add2ESDsList(new TH1F(name[2],title[2],50,-3500.,3500.),kPid2);
  }

}

//____________________________________________________________________________
void AliGlobalQADataMaker::MakeRaws(AliRawReader* rawReader)
{
  //Fill prepared histograms with Raw digit properties
  rawReader->Reset() ;

}

//____________________________________________________________________________ 
void AliGlobalQADataMaker::MakeESDs(AliESDEvent * event) {
  //-----------------------------------------------------------
  // This function fills the ESD QA histograms
  // as a part of global QA
  //-----------------------------------------------------------
  // Check id histograms already created for this Event Specie
  if ( ! GetESDsData(kClr0) )
    InitESDs() ;


  const AliESDEvent *esd=event;

  Int_t ntrk=esd->GetNumberOfTracks() ; 
  for (Int_t i=0; i<ntrk; i++) {
    const AliESDtrack *track=esd->GetTrack(i);

    // Cluster related QA
    if (track->IsOn(AliESDtrack::kITSrefit)) {
      Int_t n=track->GetITSclusters(0);
      GetESDsData(kClr0)->Fill(Float_t(n)/6.); //6 is the number of ITS layers
    }

    if (track->IsOn(AliESDtrack::kTPCrefit)) {
      Int_t n =track->GetTPCNcls();
      Int_t nf=track->GetTPCNclsF();      // number of crossed TPC pad rows
      if (nf>0) {
        Double_t val = n*1.0/nf; 
        GetESDsData(kClr1)->Fill(val); 
      }
    }

    if (track->IsOn(AliESDtrack::kTRDrefit)) {
      Int_t n=track->GetTRDclusters(0);
      GetESDsData(kClr2)->Fill(Float_t(n)/(6*24));//(6*24) is the number of TRD time bins
    }

    Double_t p=track->GetP();

    // Track related QA
    if (track->IsOn(AliESDtrack::kTPCrefit)) {
      Float_t dz[2]; 
      track->GetDZ(0.,0.,0.,esd->GetMagneticField(),dz); 
      if ((TMath::Abs(dz[0])<3.) && (TMath::Abs(dz[1])<3.)) { // beam pipe
        Double_t phi=track->Phi();
	GetESDsData(kTrk0)->Fill(phi);
	Double_t y=track->Eta();
	GetESDsData(kTrk1)->Fill(y);

        if (TMath::Abs(y)<0.9) {
	   GetESDsData(kTrk2)->Fill(p);
	   if (track->IsOn(AliESDtrack::kITSrefit)) GetESDsData(kTrk3)->Fill(p);
	  //if (track->IsOn(AliESDtrack::kTOFout)) GetESDsData(kTrk4)->Fill(p);
	   if (track->GetTOFsignal()>0) GetESDsData(kTrk4)->Fill(p);
	}
      }
    }

    // PID related QA
    if ((p>0.4) && (p<0.5)) {
      if (track->IsOn(AliESDtrack::kITSpid)) {
	Double_t dedx=track->GetITSsignal();
        GetESDsData(kPid0)->Fill(dedx);
      }
      if (track->IsOn(AliESDtrack::kTPCpid)) {
	Double_t dedx=track->GetTPCsignal();
        GetESDsData(kPid1)->Fill(dedx);
      }
    }
    if (p>1.0) {
      if (track->IsOn(AliESDtrack::kTOFpid)) {
        Double_t times[10];
        track->GetIntegratedTimes(times);
        Double_t tof=track->GetTOFsignal();
        GetESDsData(kPid2)->Fill(times[2]-tof);
      }
    }
  }

  TH1 *tpc=GetESDsData(kTrk2); tpc->Sumw2();
  TH1 *its=GetESDsData(kTrk3); its->Sumw2();
  TH1 *tof=GetESDsData(kTrk4); tof->Sumw2();
  GetESDsData(kTrk5)->Divide(its,tpc,1,1.,"b");
  GetESDsData(kTrk6)->Divide(tof,tpc,1,1.,"b");

  // V0 related QA
  Int_t nV0=esd->GetNumberOfV0s();
  for (Int_t i=0; i<nV0; i++) {
    Double_t mass;
    AliESDv0 v0(*esd->GetV0(i));

    v0.ChangeMassHypothesis(kK0Short);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kK0on)->Fill(mass);
    else
       GetESDsData(kK0off)->Fill(mass);

    v0.ChangeMassHypothesis(kLambda0);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kL0on)->Fill(mass);
    else
       GetESDsData(kL0off)->Fill(mass);

    v0.ChangeMassHypothesis(kLambda0Bar);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kL0on)->Fill(mass);
    else
       GetESDsData(kL0off)->Fill(mass);
  }

}

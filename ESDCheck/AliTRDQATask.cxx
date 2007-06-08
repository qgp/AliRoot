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
// An analysis task to check the TRD data in simulated data
//
//*-- Sylwester Radomski
//////////////////////////////////////////////////////////////////////////////
// track type codding
//
// tpci = kTPCin
// tpco = kTPCout
// tpcz = kTPCout && !kTRDout
// trdo = kTRDout
// trdr = kTRDref
// trdz = kTRDout && !kTRDref
// 

#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TString.h> 

#include "AliTRDQATask.h"
#include "AliESD.h"
#include "AliLog.h"

//______________________________________________________________________________
AliTRDQATask::AliTRDQATask(const char *name) : 
  AliAnalysisTask(name,""),  
  fChain(0),
  fESD(0)
{
  // Constructor.
  // Input slot #0 works with an Ntuple
  DefineInput(0, TChain::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(0,  TObjArray::Class()) ; 
}

//______________________________________________________________________________
void AliTRDQATask::ConnectInputData(const Option_t *)
{
  // Initialisation of branch container and histograms 

  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  
  // Get input data
  fChain = dynamic_cast<TChain *>(GetInputData(0)) ;
  if (!fChain) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }

  // One should first check if the branch address was taken by some other task
  char ** address = (char **)GetBranchAddress(0, "ESD");
  if (address) {
    fESD = (AliESD*)(*address);
  } else {
    fESD = new AliESD();
    SetBranchAddress(0, "ESD", &fESD);
  }
}

//________________________________________________________________________
void AliTRDQATask::CreateOutputObjects()
{
  // create histograms 

  OpenFile(0) ; 

  fNTracks     = new TH1D("ntracks", ";number of all tracks", 500, -0.5, 499.5); 
  fEventSize   = new TH1D("evSize", ";event size (MB)", 100, 0, 5);

  fTrackStatus = new TH1D("trackStatus", ";status bit", 32, -0.5, 31.5);
  fKinkIndex   = new TH1D("kinkIndex", ";kink index", 41, -20.5, 20.5);
  
  fParIn  = new TH1D("parIn", "Inner Plane", 2, -0.5, 1.5);
  fParOut = new TH1D("parOut", "Outer Plane", 2, -0.5, 1.5);

  fXIn  = new TH1D("xIn", ";X at the inner plane (cm)", 200, 50, 250);
  fXOut = new TH1D("xOut", ";X at the outer plane (cm)", 300, 50, 400);
  
  const int knNameAlpha = 4;
  const char *namesAlpha[knNameAlpha] = {"alphaTPCi", "alphaTPCo", "alphaTRDo", "alphaTRDr"};
  //TH1D *fAlpha[4];
  for(int i=0; i<knNameAlpha; i++) {
    fAlpha[i] = new TH1D(namesAlpha[i], "alpha", 100, -4, 4);
  }
  fSectorTRD = new TH1D ("sectorTRD", ";sector TRD", 20, -0.5, 19.5);


  // track parameters
  const int knbits = 6;
  const char *suf[knbits] = {"TPCi", "TPCo", "TPCz", "TRDo", "TRDr", "TRDz"};
  for(int i=0; i<knbits; i++) {
    fPt[i]      = new TH1D(Form("pt%s",suf[i]), ";p_{T} (GeV/c);entries TPC", 50, 0, 10);
    fTheta[i]   = new TH1D(Form("theta%s", suf[i]), "theta (rad)", 100, -4, 4); 
    fSigmaY[i]  = new TH1D(Form("sigmaY%s",suf[i]),  ";sigma Y (cm)", 200, 0, 1);
    fChi2[i]    = new TH1D(Form("Chi2%s", suf[i]), ";#chi2 / ndf", 100, 0, 10);
    fPlaneYZ[i] = new TH2D(Form("planeYZ%s", suf[i]), Form("%sy (cm);z (cm)", suf[i]), 
			   100, -60, 60, 500, -500, 500);
  }
  
  // efficiency
  fEffPt[0] = (TH1D*) fPt[0]->Clone(Form("eff_%s_%s", suf[0], suf[1]));
  fEffPt[1] = (TH1D*) fPt[0]->Clone(Form("eff_%s_%s", suf[1], suf[3]));
  fEffPt[2] = (TH1D*) fPt[0]->Clone(Form("eff_%s_%s", suf[3], suf[4]));
  fEffPt[3] = (TH1D*) fPt[0]->Clone(Form("eff_%s_%s", suf[1], suf[4]));

  for(int i=0; i<4; i++) {
    fEffPt[i]->Sumw2();
    fEffPt[i]->SetMarkerStyle(20);
    fEffPt[i]->SetMinimum(0);
    fEffPt[i]->SetMaximum(1.1);
  }

  // track features
  fClustersTRD[0] = new TH1D("clsTRDo", "TRDo;number of clusters", 130, -0.5, 129.5);;
  fClustersTRD[1] = new TH1D("clsTRDr", "TRDr;number of clusters", 130, -0.5, 129.5);;
  fClustersTRD[2] = new TH1D("clsTRDz", "TRDz;number of clusters", 130, -0.5, 129.5);;

  // for good refitted tracks only
  fTime    = new TH1D("time", ";time bin", 25, -0.5, 24.5);
  fBudget  = new TH1D("budget", ";material budget", 100, 0, 100);
  fQuality = new TH1D("quality", ";track quality", 100, 0, 1.1);
  fSignal  = new TH1D("signal", ";signal", 100, 0, 1e3);  
  
  // dEdX and PID
  fTrdSigMom = new TH2D("trdSigMom", ";momentum (GeV/c);signal", 100, 0, 3, 100, 0, 1e3);
  fTpcSigMom = new TH2D("tpcSigMom", ";momentum (GeV/c);signal", 100, 0, 3, 100, 0, 200);
  
  const char *pidName[6] = {"El", "Mu", "Pi", "K", "P", "Ch"};
  for(int i=0; i<6; i++) {
    
    // TPC
    fTpcPID[i] = new TH1D(Form("tpcPid%s",pidName[i]), pidName[i], 100, 0, 1.5);
    fTpcPID[i]->GetXaxis()->SetTitle("probability");
    
    fTpcSigMomPID[i] = new TH2D(Form("tpcSigMom%s",pidName[i]), "", 100, 0, 3, 100, 0, 200);
    fTpcSigMomPID[i]->SetTitle(Form("%s;momentum (GeV/c);signal",pidName[i])); 
    
    // TRD
    fTrdPID[i] = new TH1D(Form("trdPid%s",pidName[i]), pidName[i], 100, 0, 1.5);
    fTrdPID[i]->GetXaxis()->SetTitle("probability");
     
    fTrdSigMomPID[i] = new TH2D(Form("trdSigMom%s",pidName[i]), "", 100, 0, 3, 100, 0, 1e3);
    fTrdSigMomPID[i]->SetTitle(Form("%s;momentum (GeV/c);signal",pidName[i]));  
  }

  
  // create output container
  fOutputContainer = new TObjArray(150); 
  
  // register histograms to the container  
  int counter = 0;
  
  fOutputContainer->AddAt(fNTracks,     counter++);
  fOutputContainer->AddAt(fEventSize,   counter++);
  fOutputContainer->AddAt(fTrackStatus, counter++);
  fOutputContainer->AddAt(fKinkIndex,   counter++);
  fOutputContainer->AddAt(fParIn,       counter++);
  fOutputContainer->AddAt(fParOut,      counter++);
  fOutputContainer->AddAt(fXIn,         counter++);
  fOutputContainer->AddAt(fXOut,        counter++);
  fOutputContainer->AddAt(fAlpha[0],    counter++);
  fOutputContainer->AddAt(fAlpha[1],    counter++);
  fOutputContainer->AddAt(fAlpha[2],    counter++);
  fOutputContainer->AddAt(fAlpha[3],    counter++);

  fOutputContainer->AddAt(fSectorTRD,   counter++);
  for(int i=0; i<knbits; i++) {
     fOutputContainer->AddAt(fPt[i],      counter++);
     fOutputContainer->AddAt(fTheta[i],   counter++);
     fOutputContainer->AddAt(fSigmaY[i],  counter++);
     fOutputContainer->AddAt(fChi2[i],    counter++);
     fOutputContainer->AddAt(fPlaneYZ[i], counter++);
  }   
  fOutputContainer->AddAt(fEffPt[0], counter++);
  fOutputContainer->AddAt(fEffPt[1], counter++);
  fOutputContainer->AddAt(fEffPt[2], counter++);
  fOutputContainer->AddAt(fEffPt[3], counter++);

  fOutputContainer->AddAt(fClustersTRD[0], counter++);
  fOutputContainer->AddAt(fClustersTRD[1], counter++);
  fOutputContainer->AddAt(fClustersTRD[2], counter++);
  fOutputContainer->AddAt(fTime,      counter++);
  fOutputContainer->AddAt(fBudget,    counter++);
  fOutputContainer->AddAt(fQuality,   counter++);
  fOutputContainer->AddAt(fSignal,    counter++);
  fOutputContainer->AddAt(fTrdSigMom, counter++);
  fOutputContainer->AddAt(fTpcSigMom, counter++);
  for(int i=0; i<6; i++) {
     fOutputContainer->AddAt(fTpcPID[i],       counter++);
     fOutputContainer->AddAt(fTpcSigMomPID[i], counter++);
     fOutputContainer->AddAt(fTrdPID[i],       counter++);
     fOutputContainer->AddAt(fTrdSigMomPID[i], counter++);
  }

  //AliInfo(Form("Number of histograms = %d", counter));

 }

//______________________________________________________________________________
void AliTRDQATask::Exec(Option_t *) 
{
  // Process one event
  
   Long64_t entry = fChain->GetReadEntry() ;
  
  // Processing of one event 
   
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  }
  
  if ( !((entry-1)%100) ) 
    AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), entry)) ; 

  int nTracks = fESD->GetNumberOfTracks();
  fNTracks->Fill(nTracks); 

  // track loop
  for(int i=0; i<nTracks; i++) {
    
    AliESDtrack *track = fESD->GetTrack(i);
    const AliExternalTrackParam *paramOut = track->GetOuterParam();
    const AliExternalTrackParam *paramIn = track->GetInnerParam();

    fParIn->Fill(!!paramIn);
    if (!paramIn) continue;
    fXIn->Fill(paramIn->GetX());

    fParOut->Fill(!!paramOut);
    if (!paramOut) continue;
    fXOut->Fill(paramOut->GetX());
 
    int sector = GetSector(paramOut->GetAlpha());
    if (!CheckSector(sector)) continue;
    fSectorTRD->Fill(sector);

    fKinkIndex->Fill(track->GetKinkIndex(0));
    if (track->GetKinkIndex(0)) continue;    

    UInt_t u = 1;
    UInt_t status = track->GetStatus();
    for(int bit=0; bit<32; bit++) 
      if (u<<bit & status) fTrackStatus->Fill(bit);

    const int knbits = 6; 
    int bit[6] = {0,0,0,0,0,0};    
    bit[0] = status & AliESDtrack::kTPCin;
    bit[1] = status & AliESDtrack::kTPCout;
    bit[2] = (status & AliESDtrack::kTPCout) && !(status & AliESDtrack::kTRDout);
    bit[3] = status & AliESDtrack::kTRDout;
    bit[4] = status & AliESDtrack::kTRDrefit;
    bit[5] = (status & AliESDtrack::kTRDout) && !(status & AliESDtrack::kTRDrefit);

    
    // transverse momentum
    const double *val = track->GetParameter(); // parameters at the vertex
    double pt = 1./TMath::Abs(val[4]);

    for(int b=0; b<knbits; b++) {
      if (bit[b]) {
	fPt[b]->Fill(pt); 
	fTheta[b]->Fill(val[3]);
	fSigmaY[b]->Fill(TMath::Sqrt(paramOut->GetSigmaY2()));
	fChi2[b]->Fill(track->GetTRDchi2()/track->GetTRDncls());    
	fPlaneYZ[b]->Fill(paramOut->GetY(), paramOut->GetZ()); 
      }
    }

    // sectors
    if (bit[1]) {
      fAlpha[0]->Fill(paramIn->GetAlpha());
      fAlpha[1]->Fill(paramOut->GetAlpha());
    }
    
    if (bit[3]) fAlpha[2]->Fill(paramOut->GetAlpha());
    if (bit[4]) fAlpha[3]->Fill(paramOut->GetAlpha());

    // clusters
    for(int b=0; b<3; b++) 
      if (bit[3+b]) fClustersTRD[b]->Fill(track->GetTRDncls());

    // refitted only
    if (!bit[4]) continue;

    fQuality->Fill(track->GetTRDQuality());
    fBudget->Fill(track->GetTRDBudget());
    fSignal->Fill(track->GetTRDsignal());
	
    fTrdSigMom->Fill(track->GetP(), track->GetTRDsignal());
    fTpcSigMom->Fill(track->GetP(), track->GetTPCsignal());

    // PID only
    if (status & AliESDtrack::kTRDpid) {
      
      for(int l=0; l<6; l++) fTime->Fill(track->GetTRDTimBin(l));

      // fill pid histograms
      double trdr0 = 0, tpcr0 = 0;
      int trdBestPid = 5, tpcBestPid = 5;  // charged
      const double kminPidValue =  0.9;

      double pp[5];
      track->GetTPCpid(pp); // ESD inconsequence

      for(int pid=0; pid<5; pid++) {
	
	trdr0 += track->GetTRDpid(pid);
	tpcr0 += pp[pid];
	
	fTrdPID[pid]->Fill(track->GetTRDpid(pid));
	fTpcPID[pid]->Fill(pp[pid]);
	
	if (track->GetTRDpid(pid) > kminPidValue) trdBestPid = pid;
	if (pp[pid] > kminPidValue) tpcBestPid = pid;
      }
      
      fTrdPID[5]->Fill(trdr0); // check unitarity
      fTrdSigMomPID[trdBestPid]->Fill(track->GetP(), track->GetTRDsignal());
      
      fTpcPID[5]->Fill(tpcr0); // check unitarity
      fTpcSigMomPID[tpcBestPid]->Fill(track->GetP(), track->GetTPCsignal());
    }
    
  }

  CalculateEff();
  PostData(0, fOutputContainer);
}

//______________________________________________________________________________
void AliTRDQATask::Terminate(Option_t *)
{
  // Processing when the event loop is ended
  fOutputContainer = (TObjArray*)GetOutputData(0);
  int counter = 0;
  fNTracks     = (TH1D*)fOutputContainer->At(counter++);
  fEventSize   = (TH1D*)fOutputContainer->At(counter++);
  fTrackStatus = (TH1D*)fOutputContainer->At(counter++);
  fKinkIndex   = (TH1D*)fOutputContainer->At(counter++);
  fParIn       = (TH1D*)fOutputContainer->At(counter++);
  fParOut      = (TH1D*)fOutputContainer->At(counter++);
  fXIn         = (TH1D*)fOutputContainer->At(counter++);
  fXOut        = (TH1D*)fOutputContainer->At(counter++);
  fAlpha[0]    = (TH1D*)fOutputContainer->At(counter++);
  fAlpha[1]    = (TH1D*)fOutputContainer->At(counter++);
  fAlpha[2]    = (TH1D*)fOutputContainer->At(counter++);
  fAlpha[3]    = (TH1D*)fOutputContainer->At(counter++);

  fSectorTRD   = (TH1D*)fOutputContainer->At(counter++);
  const int knbits = 6;
  for(int i=0; i<knbits; i++) {
     fPt[i]      = (TH1D*)fOutputContainer->At(counter++);
     fTheta[i]   = (TH1D*)fOutputContainer->At(counter++);
     fSigmaY[i]  = (TH1D*)fOutputContainer->At(counter++);
     fChi2[i]    = (TH1D*)fOutputContainer->At(counter++);
     fPlaneYZ[i] = (TH2D*)fOutputContainer->At(counter++);
  }   
  fEffPt[0] = (TH1D*)fOutputContainer->At(counter++);
  fEffPt[1] = (TH1D*)fOutputContainer->At(counter++);
  fEffPt[2] = (TH1D*)fOutputContainer->At(counter++);
  fEffPt[3] = (TH1D*)fOutputContainer->At(counter++);

  fClustersTRD[0] = (TH1D*)fOutputContainer->At(counter++);
  fClustersTRD[1] = (TH1D*)fOutputContainer->At(counter++);
  fClustersTRD[2] = (TH1D*)fOutputContainer->At(counter++);
  fTime      = (TH1D*)fOutputContainer->At(counter++);
  fBudget    = (TH1D*)fOutputContainer->At(counter++);
  fQuality   = (TH1D*)fOutputContainer->At(counter++);
  fSignal    = (TH1D*)fOutputContainer->At(counter++);
  fTrdSigMom = (TH2D*)fOutputContainer->At(counter++);
  fTpcSigMom = (TH2D*)fOutputContainer->At(counter++);
  for(int i=0; i<6; i++) {
     fTpcPID[i]       = (TH1D*)fOutputContainer->At(counter++);
     fTpcSigMomPID[i] = (TH2D*)fOutputContainer->At(counter++);
     fTrdPID[i]       = (TH1D*)fOutputContainer->At(counter++);
     fTrdSigMomPID[i] = (TH2D*)fOutputContainer->At(counter++);
  }

  // create efficiency histograms
  Bool_t problem = kFALSE ; 
  AliInfo(Form(" *** %s Report:", GetName())) ; 
  
  CalculateEff();

  DrawESD() ; 
  DrawGeoESD() ; 
  //DrawConvESD() ; 
  DrawPidESD() ; 

  char line[1024] ; 
  sprintf(line, ".!tar -zcf %s.tar.gz *.eps", GetName()) ; 
  gROOT->ProcessLine(line);
  
  AliInfo(Form("!!! All the eps files are in %s.tar.gz !!!", GetName())) ;

  TString report ; 
  if(problem)
    report="Problems found, please check!!!";  
  else 
    report="OK";

  AliInfo(Form("*** %s Summary Report: %s\n",GetName(), report.Data())) ; 

}

//______________________________________________________________________________
const int AliTRDQATask::GetSector(const double alpha) const
{
  // Gets the sector number 

  double size = TMath::DegToRad() * 20.;
  int sector = (int)((alpha + TMath::Pi())/size);
  return sector;
}

//______________________________________________________________________________
const int AliTRDQATask::CheckSector(const int sector) const  
{  
  // Checks the sector number
  const int knSec = 8;
  int sec[] = {2,3,5,6,11,12,13,15};
  
  for(int i=0; i<knSec; i++) 
    if (sector == sec[i]) return 1;
  
  return 0;
}

//______________________________________________________________________________
void AliTRDQATask::CalculateEff() 
{
  // calculates the efficiency
  
  for(int i=0; i<4; i++) fEffPt[i]->Reset();
  
  fEffPt[0]->Add(fPt[1]);
  fEffPt[0]->Divide(fPt[0]);
  
  fEffPt[1]->Add(fPt[3]);
  fEffPt[1]->Divide(fPt[1]);
  
  fEffPt[2]->Add(fPt[4]);
  fEffPt[2]->Divide(fPt[3]);
  
  fEffPt[3]->Add(fPt[4]);
  fEffPt[3]->Divide(fPt[1]);
}

//______________________________________________________________________________
void AliTRDQATask::DrawESD() const 
{
  // Makes a few plots

  TCanvas * cTRD = new TCanvas("cTRD", "TRD ESD Test", 400, 10, 600, 700) ;
  cTRD->Divide(6,3) ;

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  
  TGaxis::SetMaxDigits(3);
  
  gStyle->SetLabelFont(52, "XYZ");
  gStyle->SetTitleFont(62, "XYZ");
  gStyle->SetPadRightMargin(0.02);

  // draw all 
  
  const int knplots = 18;
  const int knover[knplots] = {1,1,1,4,1,1,1,1,1,1,2,1,1,3,1,1,1,1};
  const int knnames = 24;
  const char *names[knnames] = {
    "ntracks", "kinkIndex", "trackStatus", 
    "ptTPCi", "ptTPCo", "ptTRDo", "ptTRDr",  "ptTPCz", "ptTRDz",
    "eff_TPCi_TPCo",  "eff_TPCo_TRDo", "eff_TRDo_TRDr",  "eff_TPCo_TRDr",
    "clsTRDo", "clsTRDr", "clsTRDz", 
    "alphaTPCi", "alphaTPCo", "alphaTRDo", "alphaTRDr", "sectorTRD",
    "time", "budget", "signal"
  };
  
  const int klogy[knnames] = {
    1,1,1,
    1,1,1,
    0,0,0,0,
    1,1,
    0,0,0,0,0,
    0,1,1
  };

  int nhist=0;
  for(int i=0; i<knplots; i++) {
  cTRD->cd(i+1) ;
  
  //  new TCanvas(names[i], names[nhist], 500, 300);
      
    for(int j=0; j<knover[i]; j++) {
      TH1D *hist = dynamic_cast<TH1D*>(gDirectory->FindObject(names[nhist++]));
      if (!hist) continue;
      if (hist->GetMaximum() > 0 ) 
	gPad->SetLogy(klogy[i]);
      if (strstr(hist->GetName(), "eff")) {
	hist->SetMarkerStyle(20);
	hist->SetMinimum(0);
	hist->SetMaximum(1.2);
      }

      if (!j) hist->Draw();
      else hist->Draw("SAME");
    }
  }
  cTRD->Print("TRD_ESD.eps");
}

//______________________________________________________________________________
void AliTRDQATask::DrawGeoESD() const
{
  // Makes a few plots

  TCanvas * cTRDGeo = new TCanvas("cTRDGeo", "TRD ESDGeo Test", 400, 10, 600, 700) ;
  cTRDGeo->Divide(4,2) ;

  gStyle->SetOptStat(0);
  TGaxis::SetMaxDigits(3);
  
  gStyle->SetLabelFont(52, "XYZ");
  gStyle->SetTitleFont(62, "XYZ");
  
  gStyle->SetPadTopMargin(0.06);
  gStyle->SetTitleBorderSize(0);
  
  // draw all   
  const int knnames = 7;
  const char *names[knnames] = {
    "xIn", "xOut",
    "planeYZTPCo", "planeYZTPCz", "planeYZTRDo", "planeYZTRDr", "planeYZTRDz",
  };
  
  const char *opt[knnames] = {
    "", "",
    "colz","colz", "colz", "colz", "colz"
  };
  
  const int klogy[knnames] = {
    1,1,
    0,0,0,0,0
  };
  
  for(int i=0; i<knnames; i++) {
  cTRDGeo->cd(i+1) ;
    TH1D *hist = dynamic_cast<TH1D*>(gDirectory->FindObject(names[i]));
    if (!hist) continue;
    
    //if (i<2) new TCanvas(names[i], names[i], 500, 300);
    //else new TCanvas(names[i], names[i], 300, 900);
   
    if (hist->GetMaximum() > 0 ) 
      gPad->SetLogy(klogy[i]);
    if (strstr(opt[i],"colz")) gPad->SetRightMargin(0.1);
    
    hist->Draw(opt[i]);    
    AliInfo(Form("%s\t%d", names[i], hist->GetEntries()));
  }
  
  cTRDGeo->Print("TRD_Geo.eps");
}

//______________________________________________________________________________
void AliTRDQATask::DrawConvESD() const
{
  // Makes a few plots

  AliInfo("Plotting....") ; 
  TCanvas * cTRDConv = new TCanvas("cTRDConv", "TRD ESDConv Test", 400, 10, 600, 700) ;
  cTRDConv->Divide(3,2) ;

  gROOT->SetStyle("Plain");
  gROOT->ForceStyle();
  gStyle->SetPalette(1);
  
  TGaxis::SetMaxDigits(3);
  
  gStyle->SetLabelFont(52, "XYZ");
  gStyle->SetTitleFont(62, "XYZ");
  gStyle->SetPadRightMargin(0.02);

  const int knnames = 9;
  const int knplots = 5;
  const int knover[knplots] = {3,1,1,3,1}; 
  
  const char *names[knnames] = {
    "sigmaYTPCo","sigmaYTRDo", "sigmaYTRDr", "sigmaYTPCz", "sigmaYTRDz",
    "Chi2TPCo", "Chi2TRDo", "Chi2TRDr", "Chi2TRDz"
  };
  
  const char *opt[knplots] = {
    "", "", "","","",
  };
  
  const int klogy[knplots] = {
    0,0,0,1,1
  };

  int nhist = 0;
  for(int i=0; i<knplots; i++) {
    cTRDConv->cd(i+1) ;
    //new TCanvas(names[i], names[i], 500, 300);
    if (strstr(opt[i],"colz")) gPad->SetRightMargin(0.1);
   
    for(int j=0; j<knover[i]; j++) {
      TH1D *hist = dynamic_cast<TH1D*>(gDirectory->FindObject(names[nhist++]));
      if ( hist->GetMaximum() > 0 ) 
	gPad->SetLogy(klogy[i]);
      if (!j) hist->Draw(opt[i]);
      else hist->Draw("same");
    }

  }
    cTRDConv->Print("TRD_Conv.eps");
}

//______________________________________________________________________________
void AliTRDQATask::DrawPidESD() const
{
  // Makes a few plots

  TCanvas * cTRDPid = new TCanvas("cTRDPid", "TRD ESDPid Test", 400, 10, 600, 700) ;
  cTRDPid->Divide(9,3) ;

  gROOT->SetStyle("Plain");
  gROOT->ForceStyle();
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  
  TGaxis::SetMaxDigits(3);
  
  gStyle->SetLabelFont(52, "XYZ");
  gStyle->SetTitleFont(62, "XYZ");

  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.02);

  // draw all 
 
  const int knnames = 27;
  const char *names[knnames] = {

    "signal", "trdSigMom", "tpcSigMom",

    "trdPidEl", "trdPidMu", "trdPidPi", "trdPidK", "trdPidP", "trdPidCh",
    "trdSigMomEl", "trdSigMomMu", "trdSigMomPi", "trdSigMomK", "trdSigMomP", "trdSigMomCh",
    
    "tpcPidEl", "tpcPidMu", "tpcPidPi", "tpcPidK", "tpcPidP", "tpcPidCh",
    "tpcSigMomEl", "tpcSigMomMu", "tpcSigMomPi", "tpcSigMomK", "tpcSigMomP", "tpcSigMomCh"

  };
  
  const char *opt[knnames] = {

    "", "colz", "colz",

    "", "", "", "", "", "" ,
    "colz", "colz", "colz", "colz", "colz", "colz",

    "", "", "", "", "", "" ,
    "colz", "colz", "colz", "colz", "colz", "colz" 
  };
  
  const int klogy[knnames] = {

    0,0,0,

    1,1,1,1,1,1,
    0,0,0,0,0,0,

    1,1,1,1,1,1,
    0,0,0,0,0,0    
  };

  for(int i=0; i<knnames; i++) {
    cTRDPid->cd(i+1) ;

    TH1D *hist = dynamic_cast<TH1D*>(gDirectory->FindObject(names[i]));
    if (!hist) continue;
    
    //new TCanvas(names[i], names[i], 500, 300);
    if ( hist->GetMaximum() > 0  ) 
      gPad->SetLogy(klogy[i]);
    if (strstr(opt[i],"colz")) gPad->SetRightMargin(0.1);
    
    if (strstr(names[i],"sigMom")) gPad->SetLogz(1);
    if (strstr(names[i],"SigMom")) gPad->SetLogz(1);

    hist->Draw(opt[i]);    
    AliInfo(Form("%s\t%d", names[i], hist->GetEntries()));
  }
   cTRDPid->Print("TRD_Pid.eps");
}

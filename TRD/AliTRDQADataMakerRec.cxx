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

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Produces the data needed to calculate the quality assurance.          //
//  All data must be mergeable objects.                                   //
//                                                                        //
//  Author:                                                               //
//    Sylwester Radomski (radomski@physi.uni-heidelberg.de)               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1D.h> 
#include <TH2D.h>
#include <TH3D.h>
#include <TProfile.h>
#include <TF1.h>
#include <TCanvas.h>

// --- AliRoot header files ---
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliTRDcluster.h"
#include "AliTRDQADataMakerRec.h"
#include "AliTRDgeometry.h"
#include "AliTRDdataArrayI.h"
#include "AliTRDrawStreamTB.h"

#include "AliQAChecker.h"

ClassImp(AliTRDQADataMakerRec)

//____________________________________________________________________________ 
  AliTRDQADataMakerRec::AliTRDQADataMakerRec() : 
  AliQADataMakerRec(AliQA::GetDetName(AliQA::kTRD), "TRD Quality Assurance Data Maker")
{
  //
  // Default constructor
}

//____________________________________________________________________________ 
AliTRDQADataMakerRec::AliTRDQADataMakerRec(const AliTRDQADataMakerRec& qadm) :
  AliQADataMakerRec()
{
  //
  // Copy constructor 
  //

  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 

}

//__________________________________________________________________
AliTRDQADataMakerRec& AliTRDQADataMakerRec::operator=(const AliTRDQADataMakerRec& qadm)
{
  //
  // Equal operator.
  //

  this->~AliTRDQADataMakerRec();
  new(this) AliTRDQADataMakerRec(qadm);
  return *this;

}

//____________________________________________________________________________ 
void AliTRDQADataMakerRec::EndOfDetectorCycle(AliQA::TASKINDEX task, TObjArray * list)
{
  //
  // Detector specific actions at end of cycle
  //
  //TStopwatch watch;
  //watch.Start();
  
  AliInfo("End of TRD cycle");
  
  if (task == AliQA::kRECPOINTS) {
    
    TH1D *hist = new TH1D("fitHist", "", 200, -0.5, 199.5);
    //list->Print();
    
    // fill detector map;
    for(int i=0; i<540; i++) {
      Double_t v = ((TH1D*)list->At(0))->GetBinContent(i+1);
      Int_t sm = i/30;
      Int_t det = i%30;

      TH2D *detMap = (TH2D*)list->At(87);
      Int_t bin = detMap->FindBin(sm, det);
      detMap->SetBinContent(bin, v);
    }


    // Rec points full chambers
    for (Int_t i=0; i<540; i++) {
      
      //AliInfo(Form("I = %d", i));

      //TH1D *h = ((TH2D*)list->At(1))->ProjectionY(Form("qaTRD_recPoints_amp_%d",i), i+1, i+1);
      hist->Reset();
      for(Int_t b=1; b<hist->GetXaxis()->GetNbins()-1; b++) {
	Double_t xvalue = hist->GetBinCenter(b);
	Int_t bin = ((TH2D*)list->At(1))->FindBin(i,xvalue);
	Double_t value =  ((TH2D*)list->At(1))->GetBinContent(bin);
	hist->SetBinContent(b, value);
      }
      
      //printf("Sum = %d %f\n", i, hist->GetSum());
      if (hist->GetSum() < 100) continue; // chamber not present
      
      hist->Fit("landau", "q0", "goff", 10, 180);
      TF1 *fit = hist->GetFunction("landau");
      ((TH1D*)list->At(12))->Fill(fit->GetParameter(1));
      ((TH1D*)list->At(13))->Fill(fit->GetParameter(2));
    }
    
    // time-bin by time-bin sm by sm
    for(Int_t i=0; i<18; i++) { // loop over super-modules
      
      for(Int_t j=0; j<35; j++) { // loop over time bins
	
	//TH1D *h =  ((TH3D*)list->At(10))->ProjectionZ(Form("ampTime_%d",i), i+1, i+1, j+1, j+1);     
	hist->Reset();
	for(Int_t b=1; b<hist->GetXaxis()->GetNbins()-1; b++) {
	  Double_t xvalue = hist->GetBinCenter(b);
	  Double_t svalue = 0;
	  
	  for(Int_t det=i*30; det<(i+1)*30; det++) { // loop over detectors
	    Int_t bin = ((TH3D*)list->At(10))->FindBin(det,j,xvalue);
	    Double_t value =  ((TH3D*)list->At(10))->GetBinContent(bin);
	    svalue += value;
	  }
	  //printf("v = %f\n", value);
	  hist->SetBinContent(b, svalue);
	}
	
	if (hist->GetSum() < 100) continue;
	//printf("fitting %d %d %f\n", i, j, hist->GetSum());
	
	hist->Fit("landau", "q0", "goff", 10, 180);
	TF1 *fit = hist->GetFunction("landau");
	
	TH1D *h1 = (TH1D*)list->At(14+18+i);
	Int_t bin = h1->FindBin(j);
	// printf("%d %d %d\n", det, j, bin);
	h1->SetBinContent(bin, TMath::Abs(fit->GetParameter(1)));
      }
    }
      

    // time-bin by time-bin chamber by chamber
    for (Int_t i=0; i<540; i++) {
      
      //TH1D *test = ((TH3D*)list->At(10))->ProjectionZ(Form("ampTime_%d",i), i+1, i+1, 0, 35);     
      //if (test->GetSum() < 100) continue;
      
      //AliInfo(Form("fitting det = %d", i));
      
      for(Int_t j=0; j<35; j++) {
	
	//TH1D *h =  ((TH3D*)list->At(10))->ProjectionZ(Form("ampTime_%d",i), i+1, i+1, j+1, j+1);     
	hist->Reset();
	for(Int_t b=1; b<hist->GetXaxis()->GetNbins()-1; b++) {
	  Double_t xvalue = hist->GetBinCenter(b);
	  Int_t bin = ((TH3D*)list->At(10))->FindBin(i,j,xvalue);
	  Double_t value =  ((TH3D*)list->At(10))->GetBinContent(bin);
	  //printf("v = %f\n", value);
	  hist->SetBinContent(b, value);
	}
	
	if (hist->GetSum() < 100) continue;
	//printf("fitting %d %d %f\n", i, j, hist->GetSum());
	
	hist->Fit("landau", "q0", "goff", 10, 180);
	TF1 *fit = hist->GetFunction("landau");
	
	Int_t sm = i/30;
	Int_t det = i%30;
	TH2D *h2 = (TH2D*)list->At(14+sm);
	Int_t bin = h2->FindBin(det,j);
	// printf("%d %d %d\n", det, j, bin);
	h2->SetBinContent(bin, TMath::Abs(fit->GetParameter(1)));
	h2->SetBinError(bin,fit->GetParError(1));
      }
    }
    
    if (hist) delete hist;
  }
  
  //////////////////////////
  // const Int_t knbits = 6;
  // const char *suf[knbits] = {"TPCi", "TPCo", "TPCz", "TRDo", "TRDr", "TRDz"};
  //const char *sufRatio[4] = {"TRDrTRDo", "TRDoTPCo", "TRDrTPCo", "TRDzTPCo"};

  if (task == AliQA::kESDS) {
    
    const Int_t knRatio = 4;
    const Int_t kN[knRatio] = {4,3,4,5};
    const Int_t kD[knRatio] = {3,1,1,3}; 
    
    // create ratios
    for(Int_t type=0; type<2; type++) {
      for(Int_t i=0; i<knRatio; i++) {

	TH1D *ratio = (TH1D*)list->At(19 + 2*i + type);
	TH1D *histN = (TH1D*)list->At(3 + 2*kN[i] + type);
	TH1D *histD = (TH1D*)list->At(3 + 2*kD[i] + type);

	BuildRatio(ratio, histN, histD);
	//ratio->Reset();
	//ratio->Add(histN);
	//ratio->Divide(histD);
      }
    }
  }

  
  // call the checker
  AliQAChecker::Instance()->Run(AliQA::kTRD, task, list) ;    
}

//____________________________________________________________________________ 
void AliTRDQADataMakerRec::InitESDs()
{
  //
  // Create ESDs histograms in ESDs subdir
  //

  const Int_t kNhist = 32;

  TH1 *hist[kNhist];
  Int_t histoCounter = -1 ;

  hist[++histoCounter] = new TH1D("qaTRD_esd_ntracks", ";Number of tracks", 300, -0.5, 299.5);
  hist[++histoCounter] = new TH1D("qaTRD_esd_sector", ";Sector", 18, -0.5, 17.7);
  hist[++histoCounter] = new TH1D("qaTRD_esd_bits", ";Bits", 64, -0.5, 63.5);

  const Int_t knbits = 6;
  const char *suf[knbits] = {"TPCi", "TPCo", "TPCz", "TRDo", "TRDr", "TRDz"};

  // histo = 3
  for(Int_t i=0; i<knbits; i++) { 
    hist[++histoCounter] = new TH1D(Form("qaTRD_esd_pt%s",suf[i]), ";p_{T} (GeV/c);", 100, 0, 10);
    hist[++histoCounter] = new TH1D(Form("qaTRD_esd_trdz%s", suf[i]), ";z (cm)", 200, -400, 400); 
  }

  hist[++histoCounter] = new TH1D("qaTRD_esd_clsTRDo", "TRDo;number of clusters", 180, -0.5, 179.5);;
  hist[++histoCounter] = new TH1D("qaTRD_esd_clsTRDr", "TRDr;number of clusters", 180, -0.5, 179.5);;
  hist[++histoCounter] = new TH1D("qaTRD_esd_clsTRDz", "TRDz;number of clusters", 180, -0.5, 179.5);;
  //hist[++histoCounter] = new TH1D("qaTRD_esd_clsRatio", ";cluster ratio", 100, 0., 1.3);;

  hist[++histoCounter] = new TH2D("qaTRD_esd_sigMom", ";momentum (GeV/c);signal", 100, 0, 5, 200, 0, 1e3);

  // end of cycle plots (hist 19)
  const char *sufRatio[4] = {"TRDrTRDo", "TRDoTPCo", "TRDrTPCo", "TRDzTPCo"};

  for(int i=0; i<4; i++) {
    hist[++histoCounter] = new TH1D(Form("qaTRD_esd_pt%s",sufRatio[i]), 
				    Form("Efficiency in Pt %s;p_{T};eff", sufRatio[i]),
				    100, 0, 10);

    hist[++histoCounter] = new TH1D(Form("qaTRD_esd_trdz%s",sufRatio[i]), 
				    Form("Efficiency in Z %s;z (cm);eff", sufRatio[i]),
				    200, -400, 400);
  }

  // 27 - 31
  hist[27] = new TH1D("qaTRD_esd_quality", ";quality", 120, 0, 12);
  hist[28] = new TH1D("qaTRD_esd_budget", ";NN", 110, -1000, 100);
  hist[29] = new TH1D("qaTRD_esd_chi2", ";chi2", 200, 0, 100);
  hist[30] = new TH1D("qaTRD_esd_timeBin", ";time bin", 7, -0.5, 6.5);
  hist[31] = new TH1D("qaTRD_esd_pidQuality", "pid Quality", 7, -0.5, 6.5);

  for(Int_t i=0; i<kNhist; i++) {
    //hist[i]->Sumw2();
    Add2ESDsList(hist[i], i);
  }

}

//____________________________________________________________________________ 
void AliTRDQADataMakerRec::InitRecPoints()
{
  //
  // Create Reconstructed Points histograms in RecPoints subdir
  //

  const Int_t kNhist = 14 + 4 * 18 + 2;
  TH1 *hist[kNhist];

  hist[0] = new TH1D("qaTRD_recPoints_det", ";Detector ID of the cluster", 540, -0.5, 539.5);
  hist[1] = new TH2D("qaTRD_recPoints_amp", ";Amplitude", 540, -0.5, 539, 200, -0.5, 199.5);
  hist[2] = new TH1D("qaTRD_recPoints_npad", ";Number of Pads", 12, -0.5, 11.5);

  hist[3] = new TH1D("qaTRD_recPoints_dist2", ";residuals [2pad]", 100, -1, 1);
  hist[4] = new TH1D("qaTRD_recPoints_dist3", ";residuals [3pad]", 100, -1, 1);
  hist[5] = new TH1D("qaTRD_recPoints_dist4", ";residuals [4pad]", 100, -1, 1);
  hist[6] = new TH1D("qaTRD_recPoints_dist5", ";residuals [5pad]", 100, -1, 1);

  hist[7] = new TH2D("qaTRD_recPoints_rowCol", ";row;col", 16, -0.5, 15.5, 145, -0.5, 144.5);
  hist[8] = new TH1D("qaTRD_recPoints_time", ";time bin", 35, -0.5, 34.5);
  hist[9] = new TH1D("qaTRD_recPoints_nCls", ";number of clusters", 500, -0.5, 499.5);

  hist[10] = new TH3D("qaTRD_recPoints_sigTime", ";chamber;time bin;signal", 
		      540, -0.5, 539.5, 35, -0.5, 34.5, 200, -0.5, 199.5);
  hist[11] = new TProfile("qaTRD_recPoints_prf", ";distance;center of gravity"
                         , 120, -0.6, 0.6, -1.2, 1.2, "");

  hist[12] = new TH1D("qaTRD_recPoints_ampMPV", ";amplitude MPV", 200, 0, 200);
  hist[13] = new TH1D("qaTRD_recPoints_ampSigma", ";amplitude Sigma", 200, 0, 200); 
  
  // chamber by chamber
  for(Int_t i=0; i<18; i++) {
    hist[14+i] = new TH2D(Form("qaTRD_recPoints_sigTime_sm%d",i), Form("sm%d;det;time bin"), 
			30, -0.5, 29.5, 35, -0.5, 34.5);
    hist[14+i]->SetMinimum(20);
    hist[14+i]->SetMaximum(40);
  }
 
  // time bin by time bin sm-by-sm
  for(Int_t i=0; i<18; i++) {
    hist[14+18+i] = new TH1D(Form("qaTRD_recPoints_sigTimeShape_sm%d", i), 
			     Form("sm%d;time bin;signal"),
			     35, -0.5, 34.5);

    hist[14+18+i]->SetMaximum(120);    
  }

  // str = 50
  for(Int_t i=0; i<18; i++) {
    hist[50+i] = new TH1D(Form("qaTRD_recPoints_nCls_sm%d",i),
			  Form("sm%d;time bin;number of clusters",i),
			  35, -0.5, 34.5);
  }

  // str = 68
  for(Int_t i=0; i<18; i++) {
    hist[68+i] = new TH1D(Form("qaTRD_recPoints_totalCharge_sm%d", i),
			  Form("sm%d;time bin;total charge", i),
			  35, -0.5, 34.5);
  }

  hist[86] = new TH1D("qaTRD_recPoints_signal", ";amplitude", 200, -0.5, 199.5);
  hist[87] = new TH2D("qaTRD_recPoints_detMap", ";sm;chamber", 18, -0.5, 17.5, 30, -0.5, 29.5);


  for(Int_t i=0; i<kNhist; i++) {
    //hist[i]->Sumw2();
    Add2RecPointsList(hist[i], i);
  }
}

//____________________________________________________________________________ 
void AliTRDQADataMakerRec::InitRaws()
{
  //
  // create Raws histograms in Raws subdir
  //

  const Int_t kSM = 18;
  //const Int_t kNCh = 540;
  const Int_t kNhist = 4+kSM;
  TH1D *hist[kNhist];

  // four histograms to be published
  hist[0] = new TH1D("qaTRD_raws_det", ";detector", 540, -0.5, 539.5);
  hist[1] = new TH1D("qaTRD_raws_sig", ";signal", 100, -0.5, 99.5);
  hist[2] = new TH1D("qaTRD_raws_timeBin", ";time bin", 40, -0.5, 39.5); 
  hist[3] = new TH1D("qaTRD_raws_smId", ";supermodule", 18, -0.5, 17.5);
  //
  
  // one double per MCM (not published)
  const Int_t kNMCM = 30 * 8 * 16;
  for(Int_t i=0; i<kSM; i++)
    hist[4+i] = new TH1D(Form("qaTRD_raws_sm%d",i),"",kNMCM, -0.5, kNMCM-0.5); 
  
  // register
  for(Int_t i=0; i<kNhist; i++) {
    //hist[i]->Sumw2();
    Add2RawsList(hist[i], i);
  }

}

//____________________________________________________________________________
void AliTRDQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  //
  // Make QA data from ESDs
  //

  Int_t nTracks = esd->GetNumberOfTracks();
  GetESDsData(0)->Fill(nTracks);

  // track loop
  for (Int_t i=0; i<nTracks; i++) {

    AliESDtrack *track = esd->GetTrack(i);
    const AliExternalTrackParam *paramOut = track->GetOuterParam();
    const AliExternalTrackParam *paramIn = track->GetInnerParam();

    // long track ..
    if (!paramIn) continue;
    if (!paramOut) continue;

    // not a kink
    if (track->GetKinkIndex(0) > 0) continue; 

    Double_t extZ = GetExtZ(paramIn);
    if (TMath::Abs(extZ) > 320) continue; // acceptance cut

    // .. in the acceptance
    Int_t sector = GetSector(paramOut->GetAlpha());
  
    UInt_t u = 1;
    UInt_t status = track->GetStatus();
    for(Int_t bit=0; bit<32; bit++) 
      if (u<<bit & status) GetESDsData(2)->Fill(bit);

    const Int_t knbits = 6; 
    Int_t bit[6] = {0,0,0,0,0,0};    
    bit[0] = status & AliESDtrack::kTPCin;
    bit[1] = status & AliESDtrack::kTPCout;
    bit[2] = (status & AliESDtrack::kTPCout) && !(status & AliESDtrack::kTRDout);
    bit[3] = status & AliESDtrack::kTRDout;
    bit[4] = status & AliESDtrack::kTRDrefit;
    bit[5] = (status & AliESDtrack::kTRDout) && !(status & AliESDtrack::kTRDrefit);

    // transverse momentum
    //const Double_t *val = paramOut->GetParameter(); // parameters at the Outer plane
    Double_t pt = paramOut->Pt(); //1./TMath::Abs(val[4]);

    for(Int_t b=0; b<knbits; b++) {
      if (bit[b]) {
	GetESDsData(2*b+3)->Fill(pt); 
	GetESDsData(2*b+4)->Fill(extZ);
      }
    }

    // clusters
    for(Int_t b=0; b<3; b++) 
      if (bit[3+b]) GetESDsData(b+15)->Fill(track->GetTRDncls0());

    // refitted only
    if (!bit[4]) continue;

    //fQuality->Fill(track->GetTRDQuality());
    //fBudget->Fill(track->GetTRDBudget());
    //fSignal->Fill(track->GetTRDsignal());

    GetESDsData(1)->Fill(sector);
    GetESDsData(18)->Fill(track->GetP(), track->GetTRDsignal());

    GetESDsData(27)->Fill(track->GetTRDQuality());
    GetESDsData(28)->Fill(track->GetTRDBudget());
    GetESDsData(29)->Fill(track->GetTRDchi2());
    GetESDsData(30)->Fill(track->GetTRDTimBin(0));
    GetESDsData(31)->Fill(track->GetTRDpidQuality());
    
    /*
    hist[27] = new TH1D("qaTRD_esd_quality", ";quality", 120, 0, 12);
    hist[28] = new TH1D("qaTRD_esd_budget", ";NN", 110, -1000, 100);
    hist[29] = new TH1D("qaTRD_esd_chi2", ";chi2", 300, 0, 100);
    hist[30] = new TH1D("qaTRD_esd_timeBin", 7, -0.5, 6.5);
    hist[31] = new TH1D("qaTRD_esd_pidQuality", 7, -0.5, 6.5);
    */

    /*
    // PID only
    if (status & AliESDtrack::kTRDpid) {

      for(Int_t l=0; l<6; l++) fTime->Fill(track->GetTRDTimBin(l));

      // fill pid histograms
      Double_t trdr0 = 0; //, tpcr0 = 0;
      Int_t trdBestPid = 5; //, tpcBestPid = 5;  // charged
      const Double_t kminPidValue = 0.9;

      //Double_t pp[5];
      //track->GetTPCpid(pp); // ESD inconsequence

      for(Int_t pid=0; pid<5; pid++) {
	
	trdr0 += track->GetTRDpid(pid);
	//tpcr0 += pp[pid];
	
	fTrdPID[pid]->Fill(track->GetTRDpid(pid));
	//fTpcPID[pid]->Fill(pp[pid]);
	
	if (track->GetTRDpid(pid) > kminPidValue) trdBestPid = pid;
	//if (pp[pid] > kminPidValue) tpcBestPid = pid;
      }

      fTrdPID[5]->Fill(trdr0); // check unitarity
      fTrdSigMomPID[trdBestPid]->Fill(track->GetP(), track->GetTRDsignal());

      //fTpcPID[5]->Fill(tpcr0); // check unitarity
      //fTpcSigMomPID[tpcBestPid]->Fill(track->GetP(), track->GetTPCsignal());
    }
    */

  }

}

//______________________________________________________________________________
Int_t AliTRDQADataMakerRec::GetSector(Double_t alpha) const 
{
  //
  // Gets the sector number 
  //

  Double_t size = TMath::DegToRad() * 20.; // shall use TRDgeo
  if (alpha < 0) alpha += 2*TMath::Pi();
  Int_t sector = (Int_t)(alpha/size);
  return sector;

}

//______________________________________________________________________________
Double_t AliTRDQADataMakerRec::GetExtZ(const AliExternalTrackParam *in) const 
{
  //
  // Returns the Z position at the entry to TRD
  // using parameters from the TPC in
  //

  const Double_t kX0 = 300;

  Double_t x = in->GetX();
  const Double_t *par = in->GetParameter();
  Double_t theta = par[3];
  Double_t z = in->GetZ();

  Double_t zz = z + (kX0-x) * TMath::Tan(theta);
  return zz;

}

//____________________________________________________________________________
void AliTRDQADataMakerRec::MakeRaws(AliRawReader* rawReader)
{
  //
  // Makes QA data from raw data
  //

  // 157
  // T9 -- T10

  //const Int_t kSM = 18;
  //const Int_t kROC = 30;
  const Int_t kROB = 8;
  //const Int_t kLayer = 6;
  //const Int_t kStack = 5;
  const Int_t kMCM = 16;
  //  const Int_t kADC = 22;

  AliTRDrawStreamTB *raw = new AliTRDrawStreamTB(rawReader);

  raw->SetRawVersion(3);
  raw->Init();

  while (raw->Next()) {

    GetRawsData(0)->Fill(raw->GetDet());

    // possibly needs changes with the new reader !!
    Int_t *sig = raw->GetSignals();
    for(Int_t i=0; i<3; i++) GetRawsData(1)->Fill(sig[i]);
    // ---

    GetRawsData(2)->Fill(raw->GetTimeBin());

    // calculate the index;
    Int_t sm = raw->GetSM();
    Int_t roc = raw->GetROC();
    Int_t rob = raw->GetROB();
    Int_t mcm = raw->GetMCM();
    //Int_t adc = raw->GetADC();

    //Int_t index = roc * (kROB*kMCM*kADC) + rob * (kMCM*kADC) + mcm * kADC + adc;
    Int_t  index = roc * (kROB*kMCM) + rob * kMCM + mcm;
    GetRawsData(3)->Fill(sm);
    GetRawsData(4+sm)->Fill(index);
  }
}

//____________________________________________________________________________
void AliTRDQADataMakerRec::MakeRecPoints(TTree * clustersTree)
{
  //  
  // Makes data from RecPoints
  // 
  
  //  Info("MakeRecPoints", "making");

  Int_t nsize = Int_t(clustersTree->GetTotBytes() / (sizeof(AliTRDcluster))); 
  TObjArray *clusterArray = new TObjArray(nsize+1000); 

  TBranch *branch = clustersTree->GetBranch("TRDcluster");
  if (!branch) {
    AliError("Can't get the branch !");
    return;
  }
  branch->SetAddress(&clusterArray); 

  // Loop through all entries in the tree
  Int_t nEntries   = (Int_t) clustersTree->GetEntries();
  Int_t nbytes     = 0;
  AliTRDcluster *c = 0;
  Int_t nDet[540];
  for (Int_t i=0; i<540; i++) nDet[i] = 0;

  for (Int_t iEntry = 0; iEntry < nEntries; iEntry++) {    

    // Import the tree
    nbytes += clustersTree->GetEvent(iEntry);  

    // Get the number of points in the detector
    Int_t nCluster = clusterArray->GetEntriesFast();  

    // Loop through all TRD digits
    for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) { 
      c = (AliTRDcluster *) clusterArray->UncheckedAt(iCluster);

      Int_t iDet = c->GetDetector();
      nDet[iDet]++;
      GetRecPointsData(0)->Fill(iDet);
      GetRecPointsData(86)->Fill(c->GetQ());
      GetRecPointsData(1)->Fill(iDet, c->GetQ());
      GetRecPointsData(2)->Fill(c->GetNPads());
      if (c->GetNPads() < 6)
	GetRecPointsData(1+c->GetNPads())->Fill(c->GetCenter());

      //if (c->GetPadTime() < 5)
      ((TH2D*)GetRecPointsData(7))->Fill(c->GetPadRow(), c->GetPadCol());
      GetRecPointsData(8)->Fill(c->GetPadTime());

      ((TH3D*)GetRecPointsData(10))->Fill(iDet, c->GetPadTime(), c->GetQ());
      
      Int_t iSM = iDet / 30;
      GetRecPointsData(50+iSM)->Fill(c->GetPadTime());
      GetRecPointsData(68+iSM)->Fill(c->GetPadTime(), c->GetQ());

      // PRF for 2pad
      //if (c->GetNPads() == 2) {
      Short_t *sig = c->GetSignals();
      Double_t frac = -10;

      if (sig[0] == 0 && sig[1] == 0 && sig[2] == 0 && sig[5] == 0 && sig[6] == 0) 
	frac = 1. * sig[4] / (sig[3] + sig[4]);

      if (sig[0] == 0 && sig[1] == 0 && sig[4] == 0 && sig[5] == 0 && sig[6] == 0)
	frac = -1. * sig[2] / (sig[2] + sig[3]);

      if (frac > -10)  ((TProfile*)GetRecPointsData(11))->Fill(c->GetCenter(), frac);
	
      //}
    }
  }

  for(Int_t i=0; i<540; i++) 
    if (nDet[i] > 0) GetRecPointsData(9)->Fill(nDet[i]);

  delete clusterArray;

}

//____________________________________________________________________________ 
void AliTRDQADataMakerRec::StartOfDetectorCycle()
{
  //
  // Detector specific actions at start of cycle
  //

}

//__________________________________________________________________________
Int_t AliTRDQADataMakerRec::CheckPointer(TObject *obj, const char *name) 
{
  //
  // Checks initialization of pointers
  //

  if (!obj) AliWarning(Form("null pointer: %s", name));
  return !!obj;
}
//__________________________________________________________________________
void AliTRDQADataMakerRec::BuildRatio(TH1D *ratio, TH1D *histN, TH1D*histD) {
  //
  // Calculate the ratio of two histograms 
  // error are calculated assuming the histos have the same counts
  //

  // calclate

  Int_t nbins = histN->GetXaxis()->GetNbins();
  for(Int_t i=1; i<nbins+2; i++) {
    
    Double_t valueN = histN->GetBinContent(i);
    Double_t valueD = histD->GetBinContent(i);
    
    if (valueD < 1) {
      ratio->SetBinContent(i, 0);
      ratio->SetBinError(i, 0);
      continue;
    }

    Double_t eps = (valueN < valueD-valueN)? valueN : valueD-valueN;
    
    ratio->SetBinContent(i, valueN/valueD);
    ratio->SetBinError(i, TMath::Sqrt(eps)/valueD);
  }

  // style
  ratio->SetMinimum(-0.1);
  ratio->SetMaximum(1.1);
  ratio->SetMarkerStyle(20);
}
//__________________________________________________________________________

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

//------------------------------
// Analysis task for quality-assurance of central tarcking
// mainly based on fundamental symmetries 
//
// contact eva.sicking@cern.ch
// authors 
// Authors: Jan Fiete Grosse-Oetringhaus, Christian Klein-Boesing,
//          Andreas Morsch, Eva Sicking


#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TList.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TFile.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"

#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliESDVertex.h"
#include "AliESDInputHandler.h"
#include "AliESDtrackCuts.h"
#include "AliMultiplicity.h"


#include "AliAnalysisTaskQASym.h"
#include "AliExternalTrackParam.h"
#include "AliTrackReference.h"

#include "AliHeader.h"
#include "AliGenEventHeader.h"
#include "AliGenDPMjetEventHeader.h"

ClassImp(AliAnalysisTaskQASym)

//________________________________________________________________________
AliAnalysisTaskQASym::AliAnalysisTaskQASym(const char *name) 
  : AliAnalysisTaskSE(name) 
    ,fFieldOn(kTRUE)
    ,fHists(0)
    ,fHistRECpt(0)
    ,fEta(0)
    ,fEtaPhi(0)
    ,fEtaPt(0)
    ,fQPt(0)
    ,fDca(0)
    ,fqRec(0)
    ,fsigmaPt(0)
  
    ,fRecPtPos(0)
    ,fRecPtNeg(0)
    ,fRecPhiPos(0)
    ,fRecPhiNeg(0)
    ,fRecEtaPos(0)
    ,fRecEtaNeg(0)
    ,fRecEtaPtPos(0)
    ,fRecEtaPtNeg(0)
    ,fRecDcaPos(0)
    ,fRecDcaNeg(0)
    ,fRecDcaNegInv(0)
    ,fRecDPos(0)
    ,fRecDNeg(0)


    ,fRecQPtPosEta(0)
    ,fRecQPtNegEta(0)
    ,fRecPtPosEta(0)
    ,fRecPtNegEta(0)
    ,fRecPhiPosEta(0)
    ,fRecPhiNegEta(0)
    ,fRecDcaPosEta(0)
    ,fRecDcaNegEta(0)
    ,fRecDPosEta(0)
    ,fRecDNegEta(0)

    ,fRecPtPosVz(0)
    ,fRecPtNegVz(0)
    ,fRecEtaPosVz(0)
    ,fRecEtaNegVz(0)
    ,fRecPhiPosVz(0)
    ,fRecPhiNegVz(0)
    ,fSignedDcaPosVz(0)
    ,fSignedDcaNegVz(0)
    ,fRecQPtPosEtaVz(0)
    ,fRecQPtNegEtaVz(0)
    ,fRecEtaPtPosVz(0)
    ,fRecEtaPtNegVz(0)


    ,fDeltaPhiAll(0)
    ,fDeltaPhiLeading(0) 
    ,fDiffDcaD(0)
    ,fPhiRec(0)
    ,fThetaRec(0)
    ,fNumber(0)
    ,fVx(0)
    ,fVy(0)
    ,fVz(0)
    ,fCuts(0)
  
{
    // Constructor
    //
  for(Int_t i = 0;i<18;++i){
    fRecPtTpcSector[i] = 0;
    fRecEtaTpcSector[i] = 0;
    fSignedDcaTpcSector[i] = 0;
    fRecQPtTpcSector[i] = 0;
    fRecEtaPtTpcSector[i] = 0;
  }

  for(Int_t i = 0;i< 7;++i){
    fRecPtPosLadder[i] = 0;
    fRecPtNegLadder[i] = 0;
    fRecPhiPosLadder[i] = 0;
    fRecPhiNegLadder[i] = 0;
    fRecEtaPosLadder[i] = 0;
    fRecEtaNegLadder[i] = 0;
    fSignDcaPos[i] = 0;
    fSignDcaNeg[i] = 0;
    fSignDcaNegInv[i] = 0;
    fPtSigmaPos[i] =0;
    fPtSigmaNeg[i] =0;
    fqPtRec[i] =0;
    fDcaSigmaPos[i] =0;
    fDcaSigmaNeg[i] =0;
  }

  DefineOutput(1,  TList::Class()); 

  
  
}


//________________________________________________________________________
void AliAnalysisTaskQASym::UserCreateOutputObjects()
{
  // Create histograms
  // Called once

  Double_t range = 1.;
  Double_t pt = 20.;

  fHists = new TList();
  fHistRECpt   = new TH1F("fHistRECpt", 
			  " p_{T}",
			  100, 0., pt);
  fEta   = new TH1F("fEta", 
			  " #eta",
			  200, -2., 2.);
  fEtaPhi   = new TH2F("fEtaPhi", 
			  " #eta - #phi",
			  200, -2., 2., 128, 0., 2. * TMath::Pi());
  
  fThetaRec   = new TH1F("fThetaRec", 
			  " #theta",
			  180, 0., TMath::Pi());
  fPhiRec   = new TH1F("fPhiRec", 
			  " #phi",
			  180, 0., 2*TMath::Pi());
  fNumber   = new TH1F("fNumber", 
		       "number of tracks per event",
		       50, 0.5, 49.5);
  fVx   = new TH1F("fVx", 
		   "X of vertex",
		   100, -5., 5.);
  fVy   = new TH1F("fVy", 
		   "Y of vertex",
		   100, -5., 5.);
  fVz   = new TH1F("fVz", 
		   "Z of vertex",
		   500, -50., 50.);

  fEtaPt   = new TH1F("fEtaPt", 
			  " #eta/p_{T} ",
			  100, -1., 1.);

  fQPt   = new TH1F("fQPt", 
			  " charge/p_{T} ",
			  100, -1., 1.);

  fDca   = new TH1F("fDca", 
			  " dca ",
			  200, -range, range);


  fqRec    = new TH1F("fqRec",   
			  " charge all reconstructed particle",
			  21, -9.5, 10.5);
  
  fsigmaPt    = new TH1F("fsigmaPt",   
			  "Log_{10}(#sigma_{p_{T}})",
			  200, -2., 8.);




  //------------
  for(Int_t ITSlayer_case=0;ITSlayer_case<7;ITSlayer_case++){

    fSignDcaPos[ITSlayer_case]   = new TH1F(Form("fSignDcaPos%d", ITSlayer_case),  
				   " Signed dca", 
				   200, -range, range);
    fSignDcaPos[ITSlayer_case]->GetXaxis()->SetTitle("dca");
    fSignDcaPos[ITSlayer_case]->GetYaxis()->SetTitle("");
   
 
    fSignDcaNeg[ITSlayer_case]   = new TH1F(Form("fSignDcaNeg%d", ITSlayer_case),  
				   " Signed dcas",
				   200, -range, range);
    fSignDcaNeg[ITSlayer_case]->GetXaxis()->SetTitle("dca");
    fSignDcaNeg[ITSlayer_case]->GetYaxis()->SetTitle("");

    fSignDcaNegInv[ITSlayer_case]   = new TH1F(Form("fSignDcaNegInv%d", ITSlayer_case),  
				   " inverse Signed dca ",
				   200, -range, range);
    fSignDcaNegInv[ITSlayer_case]->GetXaxis()->SetTitle("-dca");
    fSignDcaNegInv[ITSlayer_case]->GetYaxis()->SetTitle("");




    fPtSigmaPos[ITSlayer_case]   = new TH1F(Form("fPtSigmaPos%d", ITSlayer_case),  
				   " #sigma_{pT} ",
				   208, -2., 8.);
    fPtSigmaPos[ITSlayer_case]->GetXaxis()->SetTitle("Log_{10}(#sigma_{pT})");
    fPtSigmaPos[ITSlayer_case]->GetYaxis()->SetTitle("");
    
    
    fPtSigmaNeg[ITSlayer_case]   = new TH1F(Form("fPtSigmaNeg%d",ITSlayer_case),  
				  " #sigma_{pT}",
				   208, -2., 8.);
    fPtSigmaNeg[ITSlayer_case]->GetXaxis()->SetTitle("Log_{10}(#sigma_{pT})");
    fPtSigmaNeg[ITSlayer_case]->GetYaxis()->SetTitle("");





    fqPtRec[ITSlayer_case]   = new TH1F(Form("fqPtRec%d",ITSlayer_case),  
				  "q/ p_{T}",
				   200, -100., 100.);
    fqPtRec[ITSlayer_case]->GetXaxis()->SetTitle("q_{tr}/p_{T, tr} (GeV/c)");
    fqPtRec[ITSlayer_case]->GetYaxis()->SetTitle("");

  
   


    fDcaSigmaPos[ITSlayer_case]   = new TH2F(Form("fDcaSigmaPos%d", ITSlayer_case),  
				   " p_{T} shift vs #sigma_{pT} ",
				   200, -range, range,200, -4., 4. );
    fDcaSigmaPos[ITSlayer_case]->GetXaxis()->SetTitle("signed DCA)");
    fDcaSigmaPos[ITSlayer_case]->GetYaxis()->SetTitle("log_{10}(#sigma_{pT})");
    
    
    fDcaSigmaNeg[ITSlayer_case]   = new TH2F(Form("fDcaSigmaNeg%d", ITSlayer_case),  
				   " p_{T} shift vs #sigma_{pT} ",
				   200, -range, range,200, -4., 4. );
    fDcaSigmaNeg[ITSlayer_case]->GetXaxis()->SetTitle("signed DCA");
    fDcaSigmaNeg[ITSlayer_case]->GetYaxis()->SetTitle("log_{10}(#sigma_{pT})");


    
    
 
    
    // YIELDs---------- positive and negative particles
    
    fRecPtPos   = new TH1F("fRecPtPos", 
			   " p_{T}",
			   100, 0.,pt);
    fRecPtPos->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    fRecPtNeg   = new TH1F("fRecPtNeg", 
			   " p_{T} ",
			   100, 0., pt);
    fRecPtNeg->GetXaxis()->SetTitle("p_{T} (GeV/c)");

    
    fRecPhiPos   = new TH1F("fRecPhiPos", 
			    "#phi",
			    361, 0., 360.);
    fRecPhiPos->GetXaxis()->SetTitle("#phi (deg)");
  
    fRecPhiNeg   = new TH1F("fRecPhiNeg", 
			    "#phi ",
			    361, 0., 360.);
    fRecPhiNeg->GetXaxis()->SetTitle("#phi (deg)");
    
    fRecEtaPos   = new TH1F("fRecEtaPos", 
			    "#eta",
			    200, -2., 2.);
    fRecEtaPos->GetXaxis()->SetTitle("#eta");

    fRecEtaNeg   = new TH1F("fRecEtaNeg", 
			    "#eta",
			    200, -2., 2.);
    fRecEtaNeg->GetXaxis()->SetTitle("#eta");
    
    fRecEtaPtPos   = new TH1F("fRecEtaPtPos", 
			      "#eta/p_{T}",
			      200, -0.1, .1);
    fRecEtaPtPos->GetXaxis()->SetTitle("#eta/p_{T}");

    fRecEtaPtNeg   = new TH1F("fRecEtaPtNeg", 
			      "#eta/p_{T}",
			      200, -.1, .1);
    fRecEtaPtNeg->GetXaxis()->SetTitle("#eta/p_{T}");

    fRecDcaPos   = new TH1F("fRecDcaPos", 
			 " dca",
			   100, -range, range);
    fRecDcaPos->GetXaxis()->SetTitle("dca (cm)");
    fRecDcaNeg   = new TH1F("fRecDcaNeg", 
			   " dca",
			   100, -range, range);
    fRecDcaNeg->GetXaxis()->SetTitle("dca (cm)");

    fRecDcaNegInv   = new TH1F("fRecDcaNegInv", 
			   " dca",
			   100, -range, range);
    fRecDcaNegInv->GetXaxis()->SetTitle("dca (cm)");


    fRecDPos   = new TH1F("fRecDPos", 
			 " d",
			   100, -range, range);
    fRecDPos->GetXaxis()->SetTitle("d (cm)");
    fRecDNeg   = new TH1F("fRecDNeg", 
			   "d",
			   100, -range, range);
    fRecDNeg->GetXaxis()->SetTitle("d (cm)");


    //  YIELDs ---------------- positive and negative eta
    
    
    fRecQPtPosEta   = new TH1F("fRecQPtPosEta", 
			       "q/p_{T}",
			       200, -0.5, 0.5);
    fRecQPtPosEta->GetXaxis()->SetTitle("q/p_{T} ");

    fRecQPtNegEta   = new TH1F("fRecQPtNegEta", 
			       "q/p_{T}",
			       200, -0.5, 0.5);
    fRecQPtNegEta->GetXaxis()->SetTitle("q/p_{T}");
    
    fRecPtPosEta   = new TH1F("fRecPtPosEta", 
			      " p_{T} ",
			      100, 0., pt);
    fRecPtPosEta->GetXaxis()->SetTitle("p_{T} (GeV/c)");

    fRecPtNegEta   = new TH1F("fRecPtNegEta", 
			      " p_{T}",
			      100, 0., pt);
    fRecPtNegEta->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    
    fRecPhiPosEta   = new TH1F("fRecPhiPosEta", 
			    "#phi",
			    361, 0., 360);
    fRecPhiPosEta->GetXaxis()->SetTitle("#phi (deg)");

    fRecPhiNegEta   = new TH1F("fRecPhiNegEta", 
			    "#phi ",
			    361, 0, 360);
    fRecPhiNegEta->GetXaxis()->SetTitle("#phi (deg)");

    fRecDcaPosEta   = new TH1F("fRecDcaPosEta", 
			 " dca ",
			   100, -range, range);
    fRecDcaPosEta->GetXaxis()->SetTitle("dca (cm)");
    fRecDcaNegEta   = new TH1F("fRecDcaNegEta", 
			   " dca",
			   100, -range, range);
    fRecDcaNegEta->GetXaxis()->SetTitle("dca (cm)");

    fRecDPosEta   = new TH1F("fRecDPosEta", 
			 " d",
			   100, -range, range);
    fRecDPosEta->GetXaxis()->SetTitle("d (cm)");
    fRecDNegEta   = new TH1F("fRecDNegEta", 
			   "d",
			   100, -5., 5.);
    fRecDNegEta->GetXaxis()->SetTitle("d (cm)");


    
    //  YIELDs ---------------- for TPC sectors
    for(Int_t sector=0; sector<18;sector++){
      

      fRecPtTpcSector[sector]   = new TH1F(Form("fRecPtTpcSector%02d",sector), 
					   Form("p_{T} distribution: TPC sector %d",
						sector),100, 0., pt);
      fRecPtTpcSector[sector]->GetXaxis()->SetTitle("p_{T} (GeV/c)");

      fRecEtaTpcSector[sector]   = new TH1F(Form("fRecEtaTpcSector%02d",sector), 
					   Form("#eta distribution: TPC sector %d",
						sector),200, -2., 2.);
      fRecEtaTpcSector[sector]->GetXaxis()->SetTitle("p_{T} (GeV/c)");
     

      fSignedDcaTpcSector[sector]   = new TH1F(Form("fSignedDcaTpcSector%02d",sector), 
					   Form("dca distribution: TPC sector %d",
						sector),200, -range, range );
      fSignedDcaTpcSector[sector]->GetXaxis()->SetTitle("dca");

      fRecQPtTpcSector[sector]   = new TH1F(Form("fRecQPtTpcSector%02d",sector), 
					   Form("Q/ p_{T} distribution: TPC sector %d",
						sector),100, -1., 1.);
      fRecQPtTpcSector[sector]->GetXaxis()->SetTitle("Q/p_{T} (GeV/c)");

      fRecEtaPtTpcSector[sector]   = new TH1F(Form("fRecEtaPtTpcSector%02d",sector), 
					   Form("#eta/ p_{T} distribution: TPC sector %d",
						sector),100, -1., 1.);
      fRecEtaPtTpcSector[sector]->GetXaxis()->SetTitle("#eta/p_{T} (GeV/c)");
 
    }
    // YIELDS ITS ladder
    for(Int_t i=0;i<7;i++){
      fRecPtPosLadder[i]   = new TH1F(Form("fRecPtPosLadder%d", i), 
			     " p_{T} distribution",
			     100, 0., pt);
      fRecPtPosLadder[i]->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      fRecPtNegLadder[i]   = new TH1F(Form("fRecPtNegLadder%d",i), 
			     " p_{T} distribution ",
			     100, 0., pt);
      fRecPtNegLadder[i]->GetXaxis()->SetTitle("p_{T} (GeV/c)");


      fRecPhiPosLadder[i]   = new TH1F(Form("fRecPhiPosLadder%d",i), 
				 "#phi distribution: all pos eta",
				 361, 0., 360);
      fRecPhiPosLadder[i]->GetXaxis()->SetTitle("#phi (deg)");
      
      fRecPhiNegLadder[i]   = new TH1F(Form("fRecPhiNegLadder%d", i),
				 "#phi distribution: all neg eta",
				 361, 0, 360);
      fRecPhiNegLadder[i]->GetXaxis()->SetTitle("#phi (deg)");



      fRecEtaPosLadder[i]   = new TH1F(Form("fRecEtaPosLadder%d",i), 
				       "#eta distribution",
				 200, -2., 2.);
      fRecEtaPosLadder[i]->GetXaxis()->SetTitle("#eta)");
      
      fRecEtaNegLadder[i]   = new TH1F(Form("fRecEtaNegLadder%d", i),
				 "#eta distribution",
				 200, -2., 2.);
      fRecEtaNegLadder[i]->GetXaxis()->SetTitle("#eta");
    }

    Double_t vzmax = 30;

    fRecPtPosVz = new TH2F("fRecPtPosVz", 
			   "p_{T} distribution vs Vz()",
			   100, -1., 2., 200,-vzmax,vzmax);
    fRecPtPosVz->GetXaxis()->SetTitle("log_{10}(p_{T})");
    
    fRecPtNegVz = new TH2F("fRecPtNegVz",
			   "p_{T} distribution vs Vz()",
			   100, -1., 2.,200,-vzmax,vzmax);
    fRecPtNegVz->GetXaxis()->SetTitle("Log_{10}(p_{T})");
    
   
    fRecEtaPosVz= new TH2F("fRecEtaPosVz", 
			  "#eta distribution vs Vz()",
			  100, -2., 2., 200,-vzmax,vzmax);
    fRecEtaPosVz->GetXaxis()->SetTitle("#eta");
    fRecEtaNegVz = new TH2F("fRecEtaNegVz",
			   "#eta distribution vs Vz()",
			   100, -2., 2.,200,-vzmax,vzmax);
    fRecEtaNegVz->GetXaxis()->SetTitle("#eta");

    fRecPhiPosVz= new TH2F("fRecPhiPosVz", 
			  "#eta distribution vs Vz()",
			  361, 0., 360., 200,-vzmax,vzmax);
    fRecPhiPosVz->GetXaxis()->SetTitle("#phi (deg)");
    fRecPhiNegVz = new TH2F("fRecPhiNegVz",
			   "dca vs Vz()",
			   361, 0., 360.,200,-vzmax,vzmax);
    fRecPhiNegVz->GetXaxis()->SetTitle("#phi (deg)");

    fSignedDcaPosVz= new TH2F("fSignedDcaPosVz", 
			  "#eta distribution vs Vz()",
			  200, -range, range, 200,-vzmax,vzmax);
    fSignedDcaPosVz->GetXaxis()->SetTitle("dca (cm)");
    fSignedDcaNegVz = new TH2F("fSignedDcaNegVz",
			   "dca vs Vz()",
			   200, -range, range,200,-vzmax,vzmax);
    fSignedDcaNegVz->GetXaxis()->SetTitle("dca (cm)");

    fRecQPtPosEtaVz= new TH2F("fRecQPtPosEtaVz",
                          " Q/p_{T} distribution vs Vz()",
                          100, -1., 1., 200,-vzmax,vzmax);
    fRecQPtPosEtaVz->GetXaxis()->SetTitle("Q/p_{T}");
    fRecQPtNegEtaVz = new TH2F("fRecQPtNegEtaVz",
                           " Q/p_{T} distribution vs Vz()",
                           100, -1., 1.,200,-vzmax,vzmax);
    fRecQPtNegEtaVz->GetXaxis()->SetTitle("Q/p_{T}");

 
    fRecEtaPtPosVz= new TH2F("fRecEtaPtPosVz",
                          " #eta/p_{T} distribution vs Vz()",
                          100, -1., 1., 200,-vzmax,vzmax);
    fRecEtaPtPosVz->GetXaxis()->SetTitle("#eta/p_{T");
    fRecEtaPtNegVz = new TH2F("fRecEtaPtNegVz",
                           " #eta/p_{T} distribution vs Vz()",
                           100, -1., 1.,200,-vzmax,vzmax);
    fRecEtaPtNegVz->GetXaxis()->SetTitle("#eta/p_{T}");

    //new
    fDeltaPhiAll = new TH1F("fDeltaPhiAll",
                           " #Delta #phi",200,-360,360);
    fDeltaPhiAll->GetXaxis()->SetTitle("#Delta #phi");


    fDeltaPhiLeading = new TH2F("fDeltaPhiLeading",
                           " #Delta #phi",361,-360,360, 361,0, 360);
    fDeltaPhiLeading->GetXaxis()->SetTitle("#Delta #phi (deg.)");
    fDeltaPhiLeading->GetYaxis()->SetTitle("#phi_{leading particle} (deg.)");

    fDiffDcaD    = new TH1F("fDiffDcaD",   
			    "dca-d",
			    200, -5., 5.);
    
  }

  fHists->SetOwner();

  fHists->Add(fHistRECpt);
  fHists->Add(fEta);
  fHists->Add(fEtaPhi);
  fHists->Add(fThetaRec);
  fHists->Add(fPhiRec);
  fHists->Add(fNumber);
  fHists->Add(fVx);
  fHists->Add(fVy);
  fHists->Add(fVz);

  fHists->Add(fEtaPt);
  fHists->Add(fQPt);
  fHists->Add(fDca);

  fHists->Add(fDeltaPhiAll);
  fHists->Add(fDeltaPhiLeading);
  fHists->Add(fDiffDcaD);

  fHists->Add(fqRec);
  fHists->Add(fsigmaPt);

  fHists->Add(fRecPtPos);
  fHists->Add(fRecPtNeg);
  fHists->Add(fRecPhiPos);
  fHists->Add(fRecPhiNeg);
  fHists->Add(fRecEtaPos);
  fHists->Add(fRecEtaNeg);
  fHists->Add(fRecEtaPtPos);
  fHists->Add(fRecEtaPtNeg);
  fHists->Add(fRecDcaPos);
  fHists->Add(fRecDcaNeg);
  fHists->Add(fRecDcaNegInv);
  fHists->Add(fRecDPos);
  fHists->Add(fRecDNeg);


  fHists->Add(fRecQPtPosEta);
  fHists->Add(fRecQPtNegEta);
  fHists->Add(fRecPtPosEta);
  fHists->Add(fRecPtNegEta);
  fHists->Add(fRecPhiPosEta);
  fHists->Add(fRecPhiNegEta);
  fHists->Add(fRecDcaPosEta);
  fHists->Add(fRecDcaNegEta);
  fHists->Add(fRecDPosEta);
  fHists->Add(fRecDNegEta);


  for(Int_t i=0;i<18;i++){
    fHists->Add(fRecPtTpcSector[i]);
    fHists->Add(fRecEtaTpcSector[i]);
    fHists->Add(fSignedDcaTpcSector[i]);
    fHists->Add(fRecQPtTpcSector[i]);
    fHists->Add(fRecEtaPtTpcSector[i]);
  }

  for(Int_t i=0;i<7;i++){
    fHists->Add(fRecPtPosLadder[i]);
    fHists->Add(fRecPtNegLadder[i]);
    fHists->Add(fRecPhiPosLadder[i]);
    fHists->Add(fRecPhiNegLadder[i]);
    fHists->Add(fRecEtaPosLadder[i]);
    fHists->Add(fRecEtaNegLadder[i]);
  }

  fHists->Add(fRecPtPosVz);
  fHists->Add(fRecPtNegVz);
  fHists->Add(fRecEtaPosVz);
  fHists->Add(fRecEtaNegVz);
  fHists->Add(fRecPhiPosVz);
  fHists->Add(fRecPhiNegVz);
  fHists->Add(fSignedDcaPosVz);
  fHists->Add(fSignedDcaNegVz);
  fHists->Add(fRecQPtPosEtaVz);
  fHists->Add(fRecQPtNegEtaVz);
  fHists->Add(fRecEtaPtPosVz);
  fHists->Add(fRecEtaPtNegVz);


  for(Int_t i=0;i<7;i++){
    fHists->Add(fSignDcaPos[i]);
    fHists->Add(fSignDcaNeg[i]);
    fHists->Add(fSignDcaNegInv[i]);
 
    fHists->Add(fPtSigmaPos[i]);
    fHists->Add(fPtSigmaNeg[i]);
    fHists->Add(fqPtRec[i]);
  
    fHists->Add(fDcaSigmaPos[i]);
    fHists->Add(fDcaSigmaNeg[i]);
 

  } 
  
    
    
  for (Int_t i=0; i<fHists->GetEntries(); ++i) {
    TH1 *h1 = dynamic_cast<TH1*>(fHists->At(i));
    if (h1){
      // Printf("%s ",h1->GetName());
      h1->Sumw2();
    }
  }
    // BKC


}

//__________________________________________________________

void AliAnalysisTaskQASym::UserExec(Option_t *) 
{
  printf("I'm here \n");
  AliVEvent *event = InputEvent();
  if (!event) {
     Printf("ERROR: Could not retrieve event");
     return;
  }


  if(Entry()==0){
    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
    if(esd){
      Printf("We are reading from ESD");
    }
   
  }

  Printf("There are %d tracks in this event", event->GetNumberOfTracks());

  
  Int_t   leadingTrack  =   0;
  Float_t leadingEnergy = -20.;
  Float_t leadingPhi    =   0;//TMath::Pi();


  if(event->GetNumberOfTracks()!=0) fNumber->Fill(event->GetNumberOfTracks());

  const AliVVertex* vertex = event->GetPrimaryVertex();
  Float_t vz = vertex->GetZ();
  if (TMath::Abs(vz) > 10.) return;

  for (Int_t iTrack = 0; iTrack < event->GetNumberOfTracks(); iTrack++) {
    

    AliVParticle *track = event->GetTrack(iTrack);
    AliESDtrack *esdtrack =  dynamic_cast<AliESDtrack*>(track);
    if (!track) {
      Printf("ERROR: Could not receive track %d", iTrack);
      continue;
    }
    
    //if (!fCuts->AcceptTrack(esdtrack)) continue;
    const AliExternalTrackParam * tpcPSO = esdtrack->GetTPCInnerParam();
    const AliExternalTrackParam *  tpcP = esdtrack;
    if (!tpcP) continue;
   

    if (tpcP->Pt() > 50. && tpcPSO) {
      printf("High Pt %5d %5d %13.3f %13.3f \n", event->GetPeriodNumber(), event->GetOrbitNumber(),
	     tpcPSO->Pt(), esdtrack->Pt());
      // AliFatal("Jet");
    } 
    
//    if (tpcPSO) fRecPt12->Fill(tpcPSO->Pt(), esdtrack->Pt());

    if(tpcP->E()>leadingEnergy){
      leadingTrack=iTrack;
      leadingEnergy=tpcP->E();
      leadingPhi=tpcP->Phi();
    }
   


    
    //propagate to dca
    esdtrack->PropagateToDCA(event->GetPrimaryVertex(),
			     event->GetMagneticField(), 10000.);
    
    
    // if(tpcP->Pt()<2.)continue;


    fqRec->Fill(tpcP->Charge());
  

    Double_t sigmapt = tpcP->GetSigma1Pt2();
    sigmapt= sqrt(sigmapt);
    sigmapt= sigmapt *(tpcP->Pt()*tpcP->Pt()); 

    if(sigmapt < 1.e-12) continue;
    fsigmaPt->Fill(TMath::Log10(sigmapt));
   

    // hits in ITS layer
    Int_t cas=-1;
    if(esdtrack->HasPointOnITSLayer(0)) 
      cas=0;
    else if(!esdtrack->HasPointOnITSLayer(0)
	    &&  esdtrack->HasPointOnITSLayer(1)) 
      cas=1;
    else if(!esdtrack->HasPointOnITSLayer(0)
	    && !esdtrack->HasPointOnITSLayer(1) 
	    &&  esdtrack->HasPointOnITSLayer(2)) 
      cas=2;
    else if(!esdtrack->HasPointOnITSLayer(0)
	    && !esdtrack->HasPointOnITSLayer(1) 
	    && !esdtrack->HasPointOnITSLayer(2)
	    &&  esdtrack->HasPointOnITSLayer(3)) 
      cas=3;
    else if(!esdtrack->HasPointOnITSLayer(0)
	    && !esdtrack->HasPointOnITSLayer(1) 
	    && !esdtrack->HasPointOnITSLayer(2)
	    && !esdtrack->HasPointOnITSLayer(3)
	    &&  esdtrack->HasPointOnITSLayer(4)) 
      cas=4;
    else if(   !esdtrack->HasPointOnITSLayer(0)
	    && !esdtrack->HasPointOnITSLayer(1)
	    && !esdtrack->HasPointOnITSLayer(2)
	    && !esdtrack->HasPointOnITSLayer(3)
	    && !esdtrack->HasPointOnITSLayer(4) 
	    &&  esdtrack->HasPointOnITSLayer(5)) 
      cas=5;
    else 
      cas=6;
  
   
   
    //------------------- 

    Double_t sdcatr = (tpcP->Py()*tpcP->Xv()
	       - tpcP->Px()*tpcP->Yv())/tpcP->Pt();
  


    fqPtRec[cas]->Fill(tpcP->Charge()/tpcP->Pt());
    
    fHistRECpt->Fill(tpcP->Pt());
    fEta->Fill(tpcP->Eta());
    fThetaRec->Fill(tpcP->Theta());
    fPhiRec->Fill(tpcP->Phi());
    fVx->Fill(tpcP->Xv());
    fVy->Fill(tpcP->Yv());
    fVz->Fill(tpcP->Zv());
  

    fEtaPt->Fill(tpcP->Eta()/tpcP->Pt());
    fQPt->Fill(tpcP->Charge()/tpcP->Pt());
    fDca->Fill(sdcatr);



    Float_t xy, z;
    esdtrack->GetImpactParameters(xy,z);
    fDiffDcaD->Fill(sdcatr+xy);
   
    
    //for positive particles

    if(tpcP->Charge()>0){
      fRecPtPos->Fill(tpcP->Pt());
      fRecPtPosLadder[cas]->Fill(tpcP->Pt());
      fRecPtPosVz->Fill(TMath::Log10(tpcP->Pt()),tpcP->Zv());
      fRecPhiPos->Fill(TMath::RadToDeg()*tpcP->Phi());
    
     
      fRecPhiPosLadder[cas]->Fill(TMath::RadToDeg()*tpcP->Phi());
      fRecPhiPosVz->Fill(TMath::RadToDeg()*tpcP->Phi(),tpcP->Zv());
      fSignedDcaPosVz->Fill(sdcatr,tpcP->Zv());

      fRecEtaPos->Fill(tpcP->Eta());
      fRecEtaPosLadder[cas]->Fill(tpcP->Eta());
      fRecEtaPtPos->Fill(tpcP->Eta()/tpcP->Pt());
      fRecEtaPosVz->Fill(tpcP->Eta(),tpcP->Zv());
      fRecEtaPtPosVz->Fill(tpcP->Eta()/tpcP->Pt(),tpcP->Zv());
     
      fRecDcaPos->Fill(sdcatr);
      fRecDPos->Fill(xy);
      fSignDcaPos[cas]->Fill(sdcatr);
    
     
      fDcaSigmaPos[cas]->Fill(sdcatr, TMath::Log10(sigmapt));
    
      fPtSigmaPos[cas]->Fill(TMath::Log10(sigmapt));
    }
    //and negative particles
    else {
      fRecPtNeg->Fill(tpcP->Pt());
      fRecPtNegLadder[cas]->Fill(tpcP->Pt());
      fRecPtNegVz->Fill(TMath::Log10(tpcP->Pt()),tpcP->Zv());
           
      fRecPhiNeg->Fill(TMath::RadToDeg()*tpcP->Phi());
      fRecPhiNegLadder[cas]->Fill(TMath::RadToDeg()*tpcP->Phi());
      fRecPhiNegVz->Fill(TMath::RadToDeg()*tpcP->Phi(),tpcP->Zv());
      fSignedDcaNegVz->Fill(sdcatr,tpcP->Zv());
      fRecEtaPtNegVz->Fill(tpcP->Eta()/tpcP->Pt(),tpcP->Zv());

      fRecEtaNeg->Fill(tpcP->Eta());
      fRecEtaNegLadder[cas]->Fill(tpcP->Eta());
      fRecEtaPtNeg->Fill(tpcP->Eta()/tpcP->Pt());
      fRecEtaNegVz->Fill(tpcP->Eta(),tpcP->Zv());
     
      fRecDcaNeg->Fill(sdcatr);
      fRecDcaNegInv->Fill(-sdcatr);
      fRecDNeg->Fill(xy);
      fSignDcaNeg[cas]->Fill(sdcatr);
      fSignDcaNegInv[cas]->Fill(-sdcatr);
     
     
      fDcaSigmaNeg[cas]->Fill(sdcatr,TMath::Log10(sigmapt));
   
      fPtSigmaNeg[cas]->Fill(TMath::Log10(sigmapt));
    }
    


    //all particles with positive eta
    if(tpcP->Eta()>0){
      fRecQPtPosEta->Fill(tpcP->Charge()/tpcP->Pt());
      fRecPtPosEta->Fill(tpcP->Pt());
      fRecPhiPosEta->Fill(TMath::RadToDeg()*tpcP->Phi());
      fRecQPtPosEtaVz->Fill(tpcP->Charge()/tpcP->Pt(),tpcP->Zv());
      fRecDcaPosEta->Fill(sdcatr);
      fRecDPosEta->Fill(xy);
    }
    //all particles with negative eta (and eta==0)
    else{
      fRecQPtNegEta->Fill(tpcP->Charge()/tpcP->Pt());
      fRecPtNegEta->Fill(tpcP->Pt());
      fRecPhiNegEta->Fill(TMath::RadToDeg()*tpcP->Phi());
      fRecQPtNegEtaVz->Fill(tpcP->Charge()/tpcP->Pt(),tpcP->Zv());
      fRecDcaNegEta->Fill(sdcatr);
      fRecDNegEta->Fill(xy);

    }
     

    //spectren detected by TPC sectors
    //pt cut on 1 GeV/c ?!
    // if(tpcP->Pt()<1.) continue;
    fRecPtTpcSector[Int_t(tpcP->Phi()*
			  TMath::RadToDeg()/20)]->Fill(tpcP->Pt());
    fRecEtaTpcSector[Int_t(tpcP->Phi()*
			  TMath::RadToDeg()/20)]->Fill(tpcP->Eta());
    fSignedDcaTpcSector[Int_t(tpcP->Phi()*
			  TMath::RadToDeg()/20)]->Fill(sdcatr); 
    fRecQPtTpcSector[Int_t(tpcP->Phi()*
			  TMath::RadToDeg()/20)]->Fill(tpcP->Charge()/tpcP->Pt());
    fRecEtaPtTpcSector[Int_t(tpcP->Phi()*
			  TMath::RadToDeg()/20)]->Fill(tpcP->Eta()/tpcP->Pt());
     





  // another track loop
    for (Int_t iTrack2 = 0; iTrack2 < event->GetNumberOfTracks(); iTrack2++) {
      
      if(leadingTrack==iTrack2) continue;

      AliVParticle *track2 = event->GetTrack(iTrack2);
      AliESDtrack* esdtrack2 =  dynamic_cast<AliESDtrack*>(track2);
      if (!track2) {
	Printf("ERROR: Could not receive track %d", iTrack);
	continue;
      }
      if (!fCuts->AcceptTrack(esdtrack2)) continue;
      //propagate to dca
      esdtrack2->PropagateToDCA(event->GetPrimaryVertex(),
			       event->GetMagneticField(), 10000.);
 
      fDeltaPhiLeading->Fill((leadingPhi-esdtrack2->Phi())*TMath::RadToDeg(),
			     leadingPhi*TMath::RadToDeg() );

     

    }//second track loop
  }//first track loop

  
 

  // Post output data.
  // PostData(1, fHistPt);
  PostData(1, fHists);
}      





//________________________________________________________________________
void AliAnalysisTaskQASym::Terminate(Option_t *) 
{
  // Terminate

}  






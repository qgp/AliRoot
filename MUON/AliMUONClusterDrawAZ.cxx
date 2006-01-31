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

// Cluster drawing object for AZ cluster finder 

#include <stdlib.h>
#include <Riostream.h>
//#include <TROOT.h>
#include <TCanvas.h>
#include <TLine.h>
//#include <TTree.h>
#include <TH2.h>
#include <TView.h>
#include <TStyle.h>

#include "AliMUONClusterDrawAZ.h"
#include "AliMUONClusterFinderAZ.h"
#include "AliHeader.h"
#include "AliRun.h"
#include "AliMUON.h"
//#include "AliMUONChamber.h"
#include "AliMUONDigit.h"
#include "AliMUONHit.h"
#include "AliMUONRawCluster.h"
//#include "AliMUONClusterInput.h"
#include "AliMUONPixel.h"
//#include "AliMC.h"
#include "AliMUONLoader.h"
#include "AliLog.h"

ClassImp(AliMUONClusterDrawAZ)
 
//_____________________________________________________________________________
AliMUONClusterDrawAZ::AliMUONClusterDrawAZ()
  : TObject()
{
// Default constructor
  fFind = NULL; fData = NULL;
  for (Int_t i=0; i<4; i++) fHist[i] = NULL;
}

//_____________________________________________________________________________
AliMUONClusterDrawAZ::AliMUONClusterDrawAZ(AliMUONClusterFinderAZ *clusFinder)
  : TObject()
{
// Constructor
  fFind = clusFinder;
  for (Int_t i=0; i<4; i++) fHist[i] = NULL;
  fDebug = 1; 
  fEvent = fChamber = 0;
  Init();
}

//_____________________________________________________________________________
AliMUONClusterDrawAZ::~AliMUONClusterDrawAZ()
{
  // Destructor
}

//_____________________________________________________________________________
AliMUONClusterDrawAZ::AliMUONClusterDrawAZ(const AliMUONClusterDrawAZ& rhs)
  : TObject(rhs)
{
// Protected copy constructor

  AliFatal("Not implemented.");
}


//_____________________________________________________________________________
AliMUONClusterDrawAZ&  
AliMUONClusterDrawAZ::operator=(const AliMUONClusterDrawAZ& rhs)
{
// Protected assignement operator

  if (this == &rhs) return *this;

  AliFatal("Not implemented.");
  return *this;  
}    

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::Init()
{
  // Initialization

  TCanvas *c1 = new TCanvas("c1","Clusters",0,0,600,700);
  //c1->SetFillColor(10);
  c1->Divide(1,2);
  new TCanvas("c2","Mlem",700,0,600,350);

  // Get pointer to Alice detectors
  //AliMUON *muon  = (AliMUON*) gAlice->GetModule("MUON");
  //if (!muon) return;
  //Loaders
  AliRunLoader *rl = AliRunLoader::GetRunLoader();
  AliLoader *gime = rl->GetLoader("MUONLoader");
  fData = ((AliMUONLoader*)gime)->GetMUONData();

  gime->LoadHits("READ"); 
  gime->LoadRecPoints("READ"); 
}

//_____________________________________________________________________________
Bool_t AliMUONClusterDrawAZ::FindEvCh(Int_t nev, Int_t ch)
{
  // Find requested event and chamber (skip the ones before the selected)

  if (nev < fEvent) return kFALSE;
  else if (nev == fEvent && ch < fChamber) return kFALSE;
  fEvent = nev;
  fChamber = ch;
  return kTRUE;
}

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::DrawCluster()
{
  // Draw preclusters

  TCanvas *c1 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("c1");

  cout << " nev         " << fEvent << endl;
    
  char hName[4];
  for (Int_t cath = 0; cath < 2; cath++) {
    // Build histograms
    if (fHist[cath*2]) {fHist[cath*2]->Delete(); fHist[cath*2] = 0;}
    if (fHist[cath*2+1]) {fHist[cath*2+1]->Delete(); fHist[cath*2+1] = 0;}
    if (fFind->GetNPads(cath) == 0) continue; // cluster on one cathode only
    Float_t wxMin = 999, wxMax = 0, wyMin = 999, wyMax = 0; 
    Int_t minDx = 0, maxDx = 0, minDy = 0, maxDy = 0;
    for (Int_t i = 0; i < fFind->GetNPads(0)+fFind->GetNPads(1); i++) {
      if (fFind->GetIJ(0,i) != cath) continue;
      if (fFind->GetXyq(3,i) < wxMin) { wxMin = fFind->GetXyq(3,i); minDx = i; }
      if (fFind->GetXyq(3,i) > wxMax) { wxMax = fFind->GetXyq(3,i); maxDx = i; }
      if (fFind->GetXyq(4,i) < wyMin) { wyMin = fFind->GetXyq(4,i); minDy = i; }
      if (fFind->GetXyq(4,i) > wyMax) { wyMax = fFind->GetXyq(4,i); maxDy = i; }
    }
    cout << minDx << " " << maxDx << " " << minDy << " " << maxDy << endl;
    Int_t nx, ny, padSize;
    Float_t xmin = 9999, xmax = -9999, ymin = 9999, ymax = -9999;
    if (TMath::Nint(fFind->GetXyq(3,minDx)*1000) == TMath::Nint(fFind->GetXyq(3,maxDx)*1000) &&
	TMath::Nint(fFind->GetXyq(4,minDy)*1000) == TMath::Nint(fFind->GetXyq(4,maxDy)*1000)) {
      // the same segmentation
      cout << " Same" << endl;
      cout << fFind->GetXyq(3,minDx) << " " << fFind->GetXyq(3,maxDx) << " " 
	   << fFind->GetXyq(4,minDy) << " " << fFind->GetXyq(4,maxDy) << endl;
      for (Int_t i = 0; i < fFind->GetNPads(0)+fFind->GetNPads(1); i++) {
	if (fFind->GetIJ(0,i) != cath) continue;
	if (fFind->GetXyq(0,i) < xmin) xmin = fFind->GetXyq(0,i);
	if (fFind->GetXyq(0,i) > xmax) xmax = fFind->GetXyq(0,i);
	if (fFind->GetXyq(1,i) < ymin) ymin = fFind->GetXyq(1,i);
	if (fFind->GetXyq(1,i) > ymax) ymax = fFind->GetXyq(1,i);
      }
      xmin -= fFind->GetXyq(3,minDx); xmax += fFind->GetXyq(3,minDx);
      ymin -= fFind->GetXyq(4,minDy); ymax += fFind->GetXyq(4,minDy);
      nx = TMath::Nint ((xmax-xmin)/wxMin/2);
      ny = TMath::Nint ((ymax-ymin)/wyMin/2);
      cout << xmin << " " << xmax << " " << nx << " " << ymin << " " << ymax << " " << ny << endl;
      sprintf(hName,"h%d",cath*2);
      fHist[cath*2] = new TH2D(hName,"cluster",nx,xmin,xmax,ny,ymin,ymax);
      for (Int_t i = 0; i < fFind->GetNPads(0)+fFind->GetNPads(1); i++) {
	if (fFind->GetIJ(0,i) != cath) continue;
	fHist[cath*2]->Fill(fFind->GetXyq(0,i),fFind->GetXyq(1,i),fFind->GetXyq(2,i));
	//cout << fXyq[0][i] << fXyq[1][i] << fXyq[2][i] << endl;
      }
    } else {
      // different segmentation in the cluster
      cout << " Different" << endl;
      cout << fFind->GetXyq(3,minDx) << " " << fFind->GetXyq(3,maxDx) << " " 
	   << fFind->GetXyq(4,minDy) << " " << fFind->GetXyq(4,maxDy) << endl;
      Int_t nOK = 0;
      Int_t indx, locMin, locMax;
      if (TMath::Nint(fFind->GetXyq(3,minDx)*1000) != TMath::Nint(fFind->GetXyq(3,maxDx)*1000)) {
	// different segmentation along x
	indx = 0;
	locMin = minDx;
	locMax = maxDx;
      } else {
	// different segmentation along y
	indx = 1;
	locMin = minDy;
	locMax = maxDy;
      }
      Int_t loc = locMin;
      for (Int_t i = 0; i < 2; i++) {
	// loop over different pad sizes
	if (i > 0) loc = locMax;
	padSize = TMath::Nint(fFind->GetXyq(indx+3,loc)*1000);
	xmin = 9999; xmax = -9999; ymin = 9999; ymax = -9999;
	for (Int_t j = 0; j < fFind->GetNPads(0)+fFind->GetNPads(1); j++) {
	  if (fFind->GetIJ(0,j) != cath) continue;
	  if (TMath::Nint(fFind->GetXyq(indx+3,j)*1000) != padSize) continue;
	  nOK++;
	  xmin = TMath::Min (xmin,fFind->GetXyq(0,j));
	  xmax = TMath::Max (xmax,fFind->GetXyq(0,j));
	  ymin = TMath::Min (ymin,fFind->GetXyq(1,j));
	  ymax = TMath::Max (ymax,fFind->GetXyq(1,j));
	}
	xmin -= fFind->GetXyq(3,loc); xmax += fFind->GetXyq(3,loc);
	ymin -= fFind->GetXyq(4,loc); ymax += fFind->GetXyq(4,loc);
	nx = TMath::Nint ((xmax-xmin)/fFind->GetXyq(3,loc)/2);
	ny = TMath::Nint ((ymax-ymin)/fFind->GetXyq(4,loc)/2);
	sprintf(hName,"h%d",cath*2+i);
	fHist[cath*2+i] = new TH2D(hName,"cluster",nx,xmin,xmax,ny,ymin,ymax);
	for (Int_t j = 0; j < fFind->GetNPads(0)+fFind->GetNPads(1); j++) {
	  if (fFind->GetIJ(0,j) != cath) continue;
	  if (TMath::Nint(fFind->GetXyq(indx+3,j)*1000) != padSize) continue;
	  fHist[cath*2+i]->Fill(fFind->GetXyq(0,j),fFind->GetXyq(1,j),fFind->GetXyq(2,j));
	}
      } // for (Int_t i=0;
      if (nOK != fFind->GetNPads(cath)) cout << " *** Too many segmentations: nPads, nOK " 
					     << fFind->GetNPads(cath) << " " << nOK << endl;
    } // if (TMath::Nint(fFind->GetXyq(3,minDx)*1000)
  } // for (Int_t cath = 0;
	
  // Draw histograms and coordinates
  for (Int_t cath = 0; cath < 2; cath++) {
    if (cath == 0) ModifyHistos();
    if (fFind->GetNPads(cath) == 0) continue; // cluster on one cathode only
    c1->cd(cath+1);
    gPad->SetTheta(55);
    gPad->SetPhi(30);
    Double_t x, y, x0, y0, r1 = 999, r2 = 0;
    if (fHist[cath*2+1]) {
      // 
      x0 = fHist[cath*2]->GetXaxis()->GetXmin() - 1000*TMath::Cos(30*TMath::Pi()/180);
      y0 = fHist[cath*2]->GetYaxis()->GetXmin() - 1000*TMath::Sin(30*TMath::Pi()/180);
      r1 = 0;
      Int_t ihist=cath*2;
      for (Int_t iy = 1; iy <= fHist[ihist]->GetNbinsY(); iy++) {
	y = fHist[ihist]->GetYaxis()->GetBinCenter(iy) 
	  + fHist[ihist]->GetYaxis()->GetBinWidth(iy);
	for (Int_t ix = 1; ix <= fHist[ihist]->GetNbinsX(); ix++) {
	  if (fHist[ihist]->GetCellContent(ix,iy) > 0.1) {
	    x = fHist[ihist]->GetXaxis()->GetBinCenter(ix)
	      + fHist[ihist]->GetXaxis()->GetBinWidth(ix);
	    r1 = TMath::Max (r1,TMath::Sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)));
	  }
	}
      }
      ihist = cath*2 + 1 ;
      for (Int_t iy = 1; iy <= fHist[ihist]->GetNbinsY(); iy++) {
	y = fHist[ihist]->GetYaxis()->GetBinCenter(iy)
	  + fHist[ihist]->GetYaxis()->GetBinWidth(iy);
	for (Int_t ix = 1; ix <= fHist[ihist]->GetNbinsX(); ix++) {
	  if (fHist[ihist]->GetCellContent(ix,iy) > 0.1) {
	    x = fHist[ihist]->GetXaxis()->GetBinCenter(ix)
	      + fHist[ihist]->GetXaxis()->GetBinWidth(ix);
	    r2 = TMath::Max (r2,TMath::Sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)));
	  }
	}
      }
      cout << r1 << " " << r2 << endl;
    } // if (fHist[cath*2+1])
    if (r1 > r2) {
      //fHist[cath*2]->Draw("lego1");
      fHist[cath*2]->Draw("lego1Fb");
      //if (fHist[cath*2+1]) fHist[cath*2+1]->Draw("lego1SameAxisBb");
      if (fHist[cath*2+1]) fHist[cath*2+1]->Draw("lego1SameAxisBbFb");
    } else {
      //fHist[cath*2+1]->Draw("lego1");
      fHist[cath*2+1]->Draw("lego1Fb");
      //fHist[cath*2]->Draw("lego1SameAxisBb");
      fHist[cath*2]->Draw("lego1SameAxisFbBb");
    }
    c1->Update();
  } // for (Int_t cath = 0;

  // Draw simulated and reconstructed hits 
  DrawHits();
}

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::DrawHits()
{
  // Draw simulated and reconstructed hits 

  TView *view[2] = { 0x0, 0x0 };
  Double_t p1[3]={0}, p2[3], xNDC[6];
  TLine *line[99] = {0};
  TCanvas *c1 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("c1");
  if (c1) {
    c1->cd(1);
    view[0] = c1->Pad()->GetView();
    c1->cd(2);
    view[1] = c1->Pad()->GetView();
  }
  fData->SetTreeAddress("H RC");

  TH2D *hist = fHist[0] ? fHist[0] : fHist[2];
  p2[2] = hist->GetMaximum();

  // Draw simulated hits
  cout << " *** Simulated hits *** " << endl;
  Int_t ntracks = (Int_t) fData->GetNtracks();
  fnMu = 0;
  Int_t ix, iy, iok, nLine = 0;
  TClonesArray *hits = NULL;
  for (Int_t i = 0; i < ntracks; i++) {
    fData->GetTrack(i);
    hits = fData->Hits();
    Int_t nhits = (Int_t) hits->GetEntriesFast();
    AliMUONHit* mHit;
    for (Int_t ihit = 0; ihit < nhits; ihit++) {
      mHit = (AliMUONHit*) hits->UncheckedAt(ihit);
      if (mHit->Chamber() != fChamber+1) continue;  // chamber number
      if (TMath::Abs(mHit->Z()-fFind->GetZpad()) > 1) continue; // different slat
      p2[0] = p1[0] = mHit->X();        // x-pos of hit
      p2[1] = p1[1] = mHit->Y();        // y-pos
      if (p1[0] < hist->GetXaxis()->GetXmin() || 
	  p1[0] > hist->GetXaxis()->GetXmax()) continue;
      if (p1[1] < hist->GetYaxis()->GetXmin() || 
	  p1[1] > hist->GetYaxis()->GetXmax()) continue;
      // Check if track comes thru pads with signal
      iok = 0;
      for (Int_t ihist = 0; ihist < 4; ihist++) {
	if (!fHist[ihist]) continue;
	ix = fHist[ihist]->GetXaxis()->FindBin(p1[0]);
	iy = fHist[ihist]->GetYaxis()->FindBin(p1[1]);
	if (fHist[ihist]->GetCellContent(ix,iy) > 0.5) {iok = 1; break;}
      }
      if (!iok) continue;
      gStyle->SetLineColor(1);
      if (TMath::Abs((Int_t)mHit->Particle()) == 13) {
	gStyle->SetLineColor(4);
	if (fnMu < 2) {
	  fxyMu[fnMu][0] = p1[0];
	  fxyMu[fnMu++][1] = p1[1];
	}
      }	    
      if (fDebug) printf(" X=%10.4f, Y=%10.4f, Z=%10.4f\n",p1[0],p1[1],mHit->Z());
      if (view[0] || view[1]) {
	// Take into account track angles
	p2[0] += mHit->Tlength() * TMath::Sin(mHit->Theta()/180*TMath::Pi()) 
	                         * TMath::Cos(mHit->Phi()/180*TMath::Pi()) / 2;
	p2[1] += mHit->Tlength() * TMath::Sin(mHit->Theta()/180*TMath::Pi()) 
	                         * TMath::Sin(mHit->Phi()/180*TMath::Pi()) / 2;
	for (Int_t ipad = 1; ipad < 3; ipad++) {
	  c1->cd(ipad);
	  view[ipad-1]->WCtoNDC(p1, &xNDC[0]);
	  view[ipad-1]->WCtoNDC(p2, &xNDC[3]);
	  //c1->DrawLine(xpad[0],xpad[1],xpad[3],xpad[4]);
	  line[nLine] = new TLine(xNDC[0],xNDC[1],xNDC[3],xNDC[4]);
	  line[nLine++]->Draw();
	}
      }
    } // for (Int_t ihit = 0; ihit < nhits;
  } // for (Int_t i = 0; i < ntracks;

  // Draw reconstructed coordinates
  fData->GetRawClusters();
  TClonesArray *rawclust = fData->RawClusters(fChamber);
  AliMUONRawCluster *mRaw;
  gStyle->SetLineColor(3);
  cout << " *** Reconstructed hits *** " << endl;
  if (rawclust) {
    for (Int_t i = 0; i < rawclust ->GetEntriesFast(); i++) {
      mRaw = (AliMUONRawCluster*)rawclust ->UncheckedAt(i);
      if (TMath::Abs(mRaw->GetZ(0)-fFind->GetZpad()) > 1) continue; // different slat
      p2[0] = p1[0] = mRaw->GetX(0);        // x-pos of hit
      p2[1] = p1[1] = mRaw->GetY(0);        // y-pos
      if (p1[0] < hist->GetXaxis()->GetXmin() || 
	  p1[0] > hist->GetXaxis()->GetXmax()) continue;
      if (p1[1] < hist->GetYaxis()->GetXmin() || 
	  p1[1] > hist->GetYaxis()->GetXmax()) continue;
      /*
	treeD->GetEvent(cath);
	cout << mRaw->fMultiplicity[0] << mRaw->fMultiplicity[1] << endl;
	for (Int_t j=0; j<mRaw->fMultiplicity[cath]; j++) {
	Int_t digit = mRaw->fIndexMap[j][cath];
	cout << ((AliMUONDigit*)fMuonDigits->UncheckedAt(digit))->Signal() << endl;
	}
      */
      // Check if track comes thru pads with signal
      iok = 0;
      for (Int_t ihist = 0; ihist < 4; ihist++) {
	if (!fHist[ihist]) continue;
	ix = fHist[ihist]->GetXaxis()->FindBin(p1[0]);
	iy = fHist[ihist]->GetYaxis()->FindBin(p1[1]);
	if (fHist[ihist]->GetCellContent(ix,iy) > 0.5) {iok = 1; break;}
      }
      if (!iok) continue;
      if (fDebug) printf(" X=%10.4f, Y=%10.4f, Z=%10.4f\n",p1[0],p1[1],mRaw->GetZ(0));
      if (view[0] || view[1]) {
	for (Int_t ipad = 1; ipad < 3; ipad++) {
	  c1->cd(ipad);
	  view[ipad-1]->WCtoNDC(p1, &xNDC[0]);
	  view[ipad-1]->WCtoNDC(p2, &xNDC[3]);
	  line[nLine] = new TLine(xNDC[0],xNDC[1],xNDC[3],xNDC[4]);
	  line[nLine++]->Draw();
	}
      }
    } // for (Int_t i = 0; i < rawclust ->GetEntries();
  } // if (rawclust)
  c1->Update();
}

//_____________________________________________________________________________
Int_t AliMUONClusterDrawAZ::Next()
{
  // What to do next?
  // File
  FILE *lun = 0;
  //lun = fopen("pull.dat","w");

  for (Int_t i = 0; i < fnMu; i++) {
    // Check again if muon comes thru the used pads (due to extra splitting)
    for (Int_t j = 0; j < fFind->GetNPads(0)+fFind->GetNPads(1); j++) {
      if (TMath::Abs(fxyMu[i][0]-fFind->GetXyq(0,j))<fFind->GetXyq(3,j) && 
	  TMath::Abs(fxyMu[i][1]-fFind->GetXyq(1,j))<fFind->GetXyq(4,j)) {
	if (fDebug) printf("%12.3e %12.3e %12.3e %12.3e\n",fxyMu[i][2],fxyMu[i][3],fxyMu[i][4],fxyMu[i][5]);
	if (lun) fprintf(lun,"%4d %2d %12.3e %12.3e %12.3e %12.3e\n",fEvent,fChamber,fxyMu[i][2],fxyMu[i][3],fxyMu[i][4],fxyMu[i][5]);
	break;
      }
    }
  } // for (Int_t i=0; i<fnMu;

  // What's next?
  char command[8];
  cout << " What is next? " << endl;
  command[0] = ' '; 
  gets(command);
  if (command[0] == 'n' || command[0] == 'N') { fEvent++; fChamber = 0; } // next event
  else if (command[0] == 'q' || command[0] == 'Q') { if (lun) fclose(lun); } // exit display 
  else if (command[0] == 'c' || command[0] == 'C') sscanf(command+1,"%d",&fChamber); // new chamber
  else if (command[0] == 'e' || command[0] == 'E') { sscanf(command+1,"%d",&fEvent); fChamber = 0; } // new event
  else return 1; // Next precluster
  return 0;
}


//_____________________________________________________________________________
void AliMUONClusterDrawAZ::ModifyHistos(void)
{
  // Modify histograms to bring them to (approximately) the same size
  Int_t nhist = 0;
  Float_t hlim[4][4], hbin[4][4]; // first index - xmin, xmax, ymin, ymax

  Float_t binMin[4] = {999,999,999,999};

  for (Int_t i = 0; i < 4; i++) {
    if (!fHist[i]) {
      hlim[0][i] = hlim[2][i] = 999;
      hlim[1][i] = hlim[3][i] = -999;
      continue;
    }
    hlim[0][i] = fHist[i]->GetXaxis()->GetXmin(); // xmin
    hlim[1][i] = fHist[i]->GetXaxis()->GetXmax(); // xmax
    hlim[2][i] = fHist[i]->GetYaxis()->GetXmin(); // ymin
    hlim[3][i] = fHist[i]->GetYaxis()->GetXmax(); // ymax
    hbin[0][i] = hbin[1][i] = fHist[i]->GetXaxis()->GetBinWidth(1);
    hbin[2][i] = hbin[3][i] = fHist[i]->GetYaxis()->GetBinWidth(1);
    binMin[0] = TMath::Min(binMin[0],hbin[0][i]);
    binMin[2] = TMath::Min(binMin[2],hbin[2][i]);
    nhist++;
  }
  binMin[1] = binMin[0];
  binMin[3] = binMin[2];
  cout << " Nhist: " << nhist << endl;

  // Adjust histo limits for cathode with different segmentation
  for (Int_t i = 0; i < 4; i+=2) {
    if (!fHist[i+1]) continue;
    Int_t imin, imax, i1 = i + 1;
    for (Int_t lim = 0; lim < 4; lim++) {
      while (1) {
	if (hlim[lim][i] < hlim[lim][i1]) {
	  imin = i;
	  imax = i1;
	} else {
	  imin = i1;
	  imax = i;
	}
	if (TMath::Abs(hlim[lim][imin]-hlim[lim][imax])<0.01*binMin[lim]) break;
	if (lim == 0 || lim == 2) {
	  // find lower limit
	  hlim[lim][imax] -= hbin[lim][imax];
	} else {
	  // find upper limit
	  hlim[lim][imin] += hbin[lim][imin];
	}
      } // while (1)
    }
  }
    

  Int_t imnmx = 0, nExtra = 0;
  for (Int_t lim = 0; lim < 4; lim++) {
    if (lim == 0 || lim == 2) imnmx = TMath::LocMin(4,hlim[lim]); // find lower limit
    else imnmx = TMath::LocMax(4,hlim[lim]); // find upper limit

    // Adjust histogram limit
    for (Int_t i = 0; i < 4; i++) {
      if (!fHist[i]) continue;
      nExtra = TMath::Nint ((hlim[lim][imnmx]-hlim[lim][i]) / hbin[lim][i]);
      hlim[lim][i] += nExtra * hbin[lim][i];
    }
  }
    
  // Rebuild histograms 
  TH2D *hist = 0;
  Int_t nx, ny;
  Double_t x, y, cont, cmax=0;
  char hName[4];
  for (Int_t ihist = 0; ihist < 4; ihist++) {
    if (!fHist[ihist]) continue;
    nx = TMath::Nint((hlim[1][ihist]-hlim[0][ihist])/hbin[0][ihist]);
    ny = TMath::Nint((hlim[3][ihist]-hlim[2][ihist])/hbin[2][ihist]);
    cout << ihist << " " << hlim[0][ihist] << " " << hlim[1][ihist] << " " << nx;
    cout << " " << hlim[2][ihist] << " " << hlim[3][ihist] << " " << ny << endl;
    sprintf(hName,"hh%d",ihist);
    hist =  new TH2D(hName,"hist",nx,hlim[0][ihist],hlim[1][ihist],ny,hlim[2][ihist],hlim[3][ihist]);
    for (Int_t i=1; i<=fHist[ihist]->GetNbinsX(); i++) {
      x = fHist[ihist]->GetXaxis()->GetBinCenter(i);
      for (Int_t j=1; j<=fHist[ihist]->GetNbinsY(); j++) {
	y = fHist[ihist]->GetYaxis()->GetBinCenter(j);
	cont = fHist[ihist]->GetCellContent(i,j);
	hist->Fill(x,y,cont);
      }
    }
    cmax = TMath::Max (cmax,hist->GetMaximum());
    sprintf(hName,"%s%d",fHist[ihist]->GetName(),ihist);
    fHist[ihist]->Delete();
    fHist[ihist] = new TH2D(*hist);
    fHist[ihist]->SetName(hName);
    fHist[ihist]->SetNdivisions(505,"Z");
    hist->Delete(); 
  }
  if (fDebug) printf("%f \n",cmax);

  for (Int_t ihist = 0; ihist < 4; ihist++) {
    if (!fHist[ihist]) continue;
    fHist[ihist]->SetMaximum(cmax);
    fHist[ihist]->SetMinimum(0);
  }
}

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::AdjustHist(Double_t *xylim, const AliMUONPixel *pixPtr)
{
  // Adjust histogram limits for pixel drawing

  Float_t xypads[4];
  if (fHist[0]) {
    xypads[0] = fHist[0]->GetXaxis()->GetXmin();
    xypads[1] = -fHist[0]->GetXaxis()->GetXmax();
    xypads[2] = fHist[0]->GetYaxis()->GetXmin();
    xypads[3] = -fHist[0]->GetYaxis()->GetXmax();
    for (Int_t i = 0; i < 4; i++) {
      while(1) {
	if (xylim[i] < xypads[i]) break;
	xylim[i] -= 2*pixPtr->Size(i/2);
      }
    }
  } 
}

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::DrawHist(const char* canvas, TH2D *hist)
{
  // Draw histogram in given canvas 

  Int_t ix = 0;
  //((TCanvas*)gROOT->FindObject("c2"))->cd();
  ((TCanvas*)gROOT->FindObject(canvas))->cd();
  gPad->SetTheta(55);
  gPad->SetPhi(30);
  hist->Draw("lego1Fb");
  gPad->Update();
  gets((char*)&ix);
}

//_____________________________________________________________________________
void AliMUONClusterDrawAZ::FillMuon(Int_t nfit, const Double_t *parOk, const Double_t *errOk)
{
  // Fill muon information

  Int_t indx, imax;
  Double_t cmax, rad;
  for (Int_t i = 0; i < fnMu; i++) {
    cmax = fxyMu[i][6];
    for (Int_t j = 0; j < nfit; j++) {
      indx = j<2 ? j*2 : j*2+1;  
      rad = (fxyMu[i][0]-parOk[indx])*(fxyMu[i][0]-parOk[indx]) +
            (fxyMu[i][1]-parOk[indx+1])*(fxyMu[i][1]-parOk[indx+1]);
      if (rad < cmax) {
	cmax = rad; 
	imax = indx;
	fxyMu[i][6] = cmax;
	fxyMu[i][2] = parOk[imax] - fxyMu[i][0];
	fxyMu[i][4] = parOk[imax+1] - fxyMu[i][1];
	fxyMu[i][3] = errOk[imax];
	fxyMu[i][5] = errOk[imax+1];
      }
    }      
  }
}


#include "MUONTrack.h"

#include <Alieve/EventAlieve.h>

#include <AliMagF.h>
#include <AliMagFMaps.h>
#include <AliLog.h>
#include <AliESDMuonTrack.h>
#include <AliTrackReference.h>
#include <AliESDEvent.h>
#include <AliRunLoader.h>
#include <AliRun.h>

#include <AliMUONTrack.h>
#include <AliMUONTriggerTrack.h>
#include <AliMUONTrackParam.h>
#include <AliMUONConstants.h>

#include <TClonesArray.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TParticle.h>
#include <TParticlePDG.h>

#include <Riostream.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// MUONTrack
// Produce Reve:Track from AliMUONTrack with dipole field model

ClassImp(MUONTrack)

AliMagF* MUONTrack::fFieldMap = 0;

//______________________________________________________________________
MUONTrack::MUONTrack(Reve::RecTrack* t, TrackRnrStyle* rs) :
  Reve::Track(t,rs),
  fTrack(0),
  fPart(0),
  fCount(0),
  fIsMUONTrack(kFALSE),
  fIsMUONTriggerTrack(kFALSE),
  fIsESDTrack(kFALSE),
  fIsMCTrack(kFALSE),
  fIsRefTrack(kFALSE)
{
  //
  // constructor
  //

  fFieldMap = Alieve::Event::AssertMagField();

}

//______________________________________________________________________
MUONTrack::~MUONTrack()
{
  //
  // destructor
  //

  if (fIsRefTrack || fIsESDTrack) delete fTrack;
  if (fIsMCTrack) delete fPart;

}

//______________________________________________________________________
void MUONTrack::PrintMCTrackInfo()
{
  //
  // information about the MC particle
  //

  Float_t pt, p;

  if (!fPart) {
    cout << "   ! no particle ..." << endl;
    return;
  }

  cout << endl;
  cout << "   MC track parameters at vertex" << endl;
  cout << "   -------------------------------------------------------------------------------------" << endl;
  cout << "   PDG code          Vx           Vy           Vz           Px           Py           Pz   " << endl;
    
  cout << "   " <<
    setw(8) << setprecision(0) << 
    fPart->GetPdgCode() << "    " << 
    setw(8) << setprecision(3) << 
    fPart->Vx() << "     " << 
    setw(8) << setprecision(3) << 
    fPart->Vy() << "     " << 
    setw(8) << setprecision(3) << 
    fPart->Vz() << "     " << 
    setw(8) << setprecision(3) << 
    fPart->Px() << "     " << 
    setw(8) << setprecision(3) << 
    fPart->Py() << "     " << 
    setw(8) << setprecision(4) << 
    fPart->Pz() << "     " << 
    
    endl;
  
  pt = TMath::Sqrt(fPart->Px()*fPart->Px()+fPart->Py()*fPart->Py());
  p  = TMath::Sqrt(fPart->Px()*fPart->Px()+fPart->Py()*fPart->Py()+fPart->Pz()*fPart->Pz());
  
  cout << endl;
  cout << "   Pt = " << 
    setw(8) << setprecision(3) <<
    pt << "  GeV/c" << endl;
  
  cout << "   P  = " << 
    setw(8) << setprecision(4) <<
    p  << "  GeV/c" << endl;
    
}

//______________________________________________________________________
void MUONTrack::PrintMUONTrackInfo()
{
  //
  // information about the reconstructed/reference track; at hits and at vertex
  //

  Double_t RADDEG = 180.0/TMath::Pi();

  Int_t nparam;
  Float_t pt, bc, nbc, zc;
  AliMUONTrackParam *mtp;
  TClonesArray *trackParamAtHit;

  if (!fTrack) {
    cout << "   ! no reconstructed track ..." << endl;
    return;
  }

  if (fIsMUONTrack) {
    cout << endl;
    cout << "   Track number " << fLabel << endl;
    cout << "   ---------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << "   Number of clusters       " << fTrack->GetNTrackHits() << endl;
    cout << "   Match to trigger         " << fTrack->GetMatchTrigger() << endl;
    if (fTrack->GetMatchTrigger()) {
      cout << "   Chi2 tracking-trigger    " << fTrack->GetChi2MatchTrigger() << endl;
      cout << "   Local trigger number     " << fTrack->GetLoTrgNum() << endl;
    }
  }

  if (fIsRefTrack) {
    cout << endl;
    cout << "   Track reference number " << fLabel << endl;
    cout << "   ---------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << "   Number of clusters       " << fTrack->GetNTrackHits() << endl;
  }
 
  trackParamAtHit = fTrack->GetTrackParamAtHit();
  nparam = trackParamAtHit->GetEntries();

  cout << endl;
  cout << "   TrackParamAtHit entries  " << nparam << "" << endl;
  cout << "   ---------------------------------------------------------------------------------------------------------------------------------" << endl;
  cout << "   Number   InvBendMom   BendSlope   NonBendSlope   BendCoord   NonBendCoord               Z        Px        Py        Pz         P" << endl;

  for (Int_t i = 0; i < nparam; i++) {

    mtp = (AliMUONTrackParam*)trackParamAtHit->At(i);

    cout << 
      setw(9)<< setprecision(3) << 
      i << "     " << 

      setw(8) << setprecision(3) << 
      mtp->GetInverseBendingMomentum() << "    " << 

      setw(8) << setprecision(3) <<
      mtp->GetBendingSlope()*RADDEG << "       " << 

      setw(8) << setprecision(3) <<
      mtp->GetNonBendingSlope()*RADDEG << "    " << 

      setw(8) << setprecision(4) <<
      mtp->GetBendingCoor() << "       " <<  

      setw(8) << setprecision(4) <<
      mtp->GetNonBendingCoor() << "      " <<  

      setw(10) << setprecision(6) <<
      mtp->GetZ() << "  " <<  

      setw(8) << setprecision(4) <<
      mtp->Px() << "  " <<  

      setw(8) << setprecision(4) <<
      mtp->Py() << "  " <<  

      setw(8) << setprecision(4) <<
      mtp->Pz() << "  " <<  

      setw(8) << setprecision(4) <<
      mtp->P() << "  " <<  

      endl;

  }

  cout << endl;
  cout << "   Track parameters at vertex" << endl;
  cout << "   --------------------------------------------------------------------------------------------------------------------" << endl;
  cout << "   InvBendMom   BendSlope   NonBendSlope   BendCoord   NonBendCoord           Z        Px        Py        Pz         P" << endl;

  mtp = (AliMUONTrackParam*)fTrack->GetTrackParamAtVertex();

  bc  = mtp->GetBendingCoor();
  nbc = mtp->GetNonBendingCoor();
  zc  = mtp->GetZ();
  if (bc  < 0.001) bc  = 0.0;
  if (nbc < 0.001) nbc = 0.0;
  if (zc  < 0.001) zc  = 0.0;

  cout << "     " <<
    setw(8) << setprecision(3) << 
    mtp->GetInverseBendingMomentum() << "    " << 
    
    setw(8) << setprecision(3) <<
    mtp->GetBendingSlope()*RADDEG << "       " << 
    
    setw(8) << setprecision(3) <<
    mtp->GetNonBendingSlope()*RADDEG << "    " << 
    
    setw(8) << setprecision(4) <<
    bc << "       " <<  
    
    setw(8) << setprecision(4) <<
    nbc << "  " <<  
    
    setw(10) << setprecision(6) <<
    zc << "  " <<  
    
    setw(8) << setprecision(4) <<
    mtp->Px() << "  " <<  
    
    setw(8) << setprecision(4) <<
    mtp->Py() << "  " <<  
    
    setw(8) << setprecision(4) <<
    mtp->Pz() << "  " <<  
    
    setw(8) << setprecision(4) <<
    mtp->P() << "  " <<  
    
    endl;
  
  pt = TMath::Sqrt(mtp->Px()*mtp->Px()+mtp->Py()*mtp->Py());

  cout << endl;
  cout << "   Pt = " << 
    setw(8) << setprecision(3) <<
    pt << "  GeV/c" << endl;

}

//______________________________________________________________________
void MUONTrack::PrintMUONTriggerTrackInfo()
{
  //
  // information about the trigger track
  //

  // Double_t RADDEG = 180.0/TMath::Pi();

}

//______________________________________________________________________
void MUONTrack::PrintESDTrackInfo()
{
  //
  // information about the reconstructed ESD track at vertex
  //

  Double_t RADDEG = 180.0/TMath::Pi();
  Float_t pt;

  AliMUONTrackParam *mtp = (AliMUONTrackParam*)fTrack->GetTrackParamAtVertex();

  cout << endl;
  cout << "   ESD muon track " << endl;
  cout << "   -----------------------------------------------------------------------------------------------------------" << endl;
  cout << "   InvBendMom   BendSlope   NonBendSlope    BendCoord   NonBendCoord           Z        Px        Py        Pz" << endl;
  
  cout << "     " << 
    
    setw(8) << setprecision(4) << 
    mtp->GetInverseBendingMomentum() << "    " << 
    
    setw(8) << setprecision(3) <<
    mtp->GetBendingSlope()*RADDEG << "       " << 
    
    setw(8) << setprecision(3) <<
    mtp->GetNonBendingSlope()*RADDEG << "     " << 
    
    setw(8) << setprecision(4) <<
    mtp->GetBendingCoor() << "       " <<  
    
    setw(8) << setprecision(4) <<
    mtp->GetNonBendingCoor() << "  " <<  
    
    setw(10) << setprecision(6) <<
    mtp->GetZ() << "  " <<  
    
    setw(8) << setprecision(3) <<
    mtp->Px() << "  " <<  
    
    setw(8) << setprecision(3) <<
    mtp->Py() << "  " <<  
    
    setw(8) << setprecision(3) <<
    mtp->Pz() << "  " <<  
    
    endl;
  
  pt = TMath::Sqrt(mtp->Px()*mtp->Px()+mtp->Py()*mtp->Py());
  
  cout << endl;
  cout << "   Pt = " << 
    setw(8) << setprecision(3) <<
    pt << "  GeV/c" << endl;
  
  cout << "   P  = " << 
    setw(8) << setprecision(4) <<
    mtp->P()  << "  GeV/c" << endl;
  
  AliESDEvent* esd = Alieve::Event::AssertESD();
  
  Double_t spdVertexX = 0;
  Double_t spdVertexY = 0;
  Double_t spdVertexZ = 0;
  Double_t esdVertexX = 0;
  Double_t esdVertexY = 0;
  Double_t esdVertexZ = 0;

  AliESDVertex* spdVertex = (AliESDVertex*) esd->GetVertex();
  if (spdVertex->GetNContributors()) {
    spdVertexZ = spdVertex->GetZv();
    spdVertexY = spdVertex->GetYv();
    spdVertexX = spdVertex->GetXv();
  }
  
  AliESDVertex* esdVertex = (AliESDVertex*) esd->GetPrimaryVertex();
  if (esdVertex->GetNContributors()) {
    esdVertexZ = esdVertex->GetZv();
    esdVertexY = esdVertex->GetYv();
    esdVertexX = esdVertex->GetXv();
  }
  
  Float_t t0v = esd->GetT0zVertex();
  
  cout << endl;
  cout << endl;
  cout << "External vertex SPD: " << 
    setw(3) <<
    spdVertex->GetNContributors() << "   " <<
    setw(8) << setprecision(3) <<
    spdVertexX << "   " <<
    spdVertexY << "   " <<
    spdVertexZ << "   " << endl;
  cout << "External vertex ESD: " << 
    setw(3) <<
    esdVertex->GetNContributors() << "   " <<
    setw(8) << setprecision(3) <<
    esdVertexX << "   " <<
    esdVertexY << "   " <<
    esdVertexZ << "   " << endl;
  cout << "External vertex T0: " << 
    setw(8) << setprecision(3) <<
    t0v << "   " << endl;
  
}

//______________________________________________________________________
void MUONTrack::MUONTrackInfo()
{
  //
  // MENU function
  //

  if (fIsMCTrack) {
    PrintMCTrackInfo();
  }
    
  if (fIsMUONTrack || fIsRefTrack) {
    PrintMUONTrackInfo();
  }
    
  if (fIsESDTrack) {
    PrintESDTrackInfo();
  }

  if (fIsMUONTriggerTrack) {
    PrintMUONTriggerTrackInfo();
  }
    
  cout << endl;
  cout << endl;
  cout << endl;
  cout << "   (slopes [deg], coord [cm], p [GeV/c])" << endl;

}

//______________________________________________________________________
void MUONTrack::MUONTriggerInfo()
{
  //
  // MENU function
  //

  if (fIsMUONTrack) {
    Reve::LoadMacro("MUON_trigger_info.C");
    gROOT->ProcessLine(Form("MUON_trigger_info(%d);", fLabel));
  }
  if (fIsRefTrack) {
    cout << "This is a reference track!" << endl;
  }
  if (fIsMCTrack) {
    cout << "This is a Monte-Carlo track!" << endl;
  }
  if (fIsESDTrack) {

    AliESDEvent* esd = Alieve::Event::AssertESD();
    ULong64_t triggerMask = esd->GetTriggerMask();

    cout << endl;
    cout << ">>>>>#########################################################################################################################" << endl;
    cout << endl;

    cout << "   ESD track trigger info" << endl;
    cout << "   -----------------------------------------------------" << endl;
    cout << endl;

    cout << "   Match to trigger         " << fTrack->GetMatchTrigger() << endl;
    cout << endl;
    cout << "   ESD trigger mask = " << triggerMask << endl;

    cout << endl;
    cout << "#########################################################################################################################<<<<<" << endl;
    cout << endl;
    
  }

}

//______________________________________________________________________
void MUONTrack::MakeMUONTrack(AliMUONTrack *mtrack)
{
  //
  // builds the track with dipole field
  //

  if (!fIsRefTrack) {
    fIsMUONTrack = kTRUE;
    fTrack    = mtrack;
  }

  if (fIsRefTrack) {
    fTrack = new AliMUONTrack(*mtrack);
  }

  Double_t xv, yv;
  Float_t ax, bx, ay, by;
  Float_t xr[28], yr[28], zr[28];
  Float_t xrc[28], yrc[28], zrc[28];
  char form[1000];
    
  TMatrixD smatrix(2,2);
  TMatrixD sums(2,1);
  TMatrixD res(2,1);

  Float_t xRec, xRec0;
  Float_t yRec, yRec0;
  Float_t zRec, zRec0;
  
  // middle z between the two detector planes of the trigger chambers
  Float_t zg[4] = { -1603.5, -1620.5, -1703.5, -1720.5 };

  Float_t pt    = 0.0;
  Float_t pv[3] = { 0.0 };

  if (fIsMUONTrack) {
    if (mtrack->GetMatchTrigger()) {
      sprintf(form,"MUONTrack %2d (MT)", fLabel);
    } else {
      sprintf(form,"MUONTrack %2d     ", fLabel);
    }
    SetName(form);
    SetLineStyle(1);
  }
  
  AliMUONTrackParam *trackParam = mtrack->GetTrackParamAtVertex(); 
  xRec0  = trackParam->GetNonBendingCoor();
  yRec0  = trackParam->GetBendingCoor();
  zRec0  = trackParam->GetZ();
  
  if (fIsMUONTrack) {
    SetPoint(fCount,xRec0,yRec0,zRec0);
    fCount++;
  }

  for (Int_t i = 0; i < 28; i++) xr[i]=yr[i]=zr[i]=0.0;
  
  Int_t nTrackHits = mtrack->GetNTrackHits();
  
  Bool_t hitChamber[14] = {kFALSE};
  Int_t iCha;
  TClonesArray* trackParamAtHit = mtrack->GetTrackParamAtHit();

  for (Int_t iHit = 0; iHit < nTrackHits; iHit++){

    trackParam = (AliMUONTrackParam*) trackParamAtHit->At(iHit); 
    
    if (iHit == 0) {
      if (IsMUONTrack()) {
	pt = TMath::Sqrt(trackParam->Px()*trackParam->Px()+trackParam->Py()*trackParam->Py());
	SetLineColor(ColorIndex(pt));
      }
      pv[0] = trackParam->Px();
      pv[1] = trackParam->Py();
      pv[2] = trackParam->Pz();
      fP.Set(pv);
    }

    xRec  = trackParam->GetNonBendingCoor();
    yRec  = trackParam->GetBendingCoor();
    zRec  = trackParam->GetZ();
    
    iCha = AliMUONConstants::ChamberNumber(zRec);
    
    xr[iHit] = xRec;
    yr[iHit] = yRec;
    zr[iHit] = zRec;

    hitChamber[iCha] = kTRUE;

  }

  Int_t crntCha, lastHitSt12, firstHitSt3, lastHitSt3, firstHitSt45;

  if (fIsMUONTrack) nTrackHits = 10;

  lastHitSt12  = -1;
  firstHitSt3  = -1;
  lastHitSt3   = -1;
  firstHitSt45 = -1;
  for (Int_t iHit = 0; iHit < nTrackHits; iHit++) {
    crntCha = AliMUONConstants::ChamberNumber(zr[iHit]);
    if (hitChamber[crntCha] && crntCha >= 0 && crntCha <= 3) {
      lastHitSt12 = iHit;
    }
    if (hitChamber[crntCha] && crntCha >= 4 && crntCha <= 5) {
      if (firstHitSt3 == -1) firstHitSt3 = iHit;
      lastHitSt3 = iHit;
    }
    if (hitChamber[crntCha] && crntCha >= 6 && crntCha <= 9) {
      if (firstHitSt45 == -1) firstHitSt45 = iHit;
    }
  }

  if (lastHitSt12 >= 0) {
    for (Int_t iHit = 0; iHit <= lastHitSt12; iHit++) {
      SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
      fCount++;
    }
    if (firstHitSt3 >= 0) {
      Propagate(xr,yr,zr,lastHitSt12,firstHitSt3);
      SetPoint(fCount,xr[firstHitSt3],yr[firstHitSt3],zr[firstHitSt3]);
      fCount++;
      if (lastHitSt3 >= 0) {
	SetPoint(fCount,xr[lastHitSt3],yr[lastHitSt3],zr[lastHitSt3]);
	fCount++;
	if (firstHitSt45 >= 0) {
	  Propagate(xr,yr,zr,lastHitSt3,firstHitSt45);
	  for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	    SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	    fCount++;
	  }
	} else {
	  Propagate(xr,yr,zr,lastHitSt3,9999);
	}
      } else if (firstHitSt45 >= 0) {
	Propagate(xr,yr,zr,firstHitSt3,firstHitSt45);
	for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	  SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	  fCount++;
	}
      } else {
	Propagate(xr,yr,zr,firstHitSt3,9999);
      }
    } else if (lastHitSt3 >= 0) {
      Propagate(xr,yr,zr,lastHitSt12,lastHitSt3);
      SetPoint(fCount,xr[lastHitSt3],yr[lastHitSt3],zr[lastHitSt3]);
      fCount++;
      if (firstHitSt45 >= 0) {
	Propagate(xr,yr,zr,lastHitSt3,firstHitSt45);
	for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	  SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	  fCount++;
	}
      } else {
	Propagate(xr,yr,zr,lastHitSt3,9999);
      }
    } else if (firstHitSt45 >= 0){
      Propagate(xr,yr,zr,lastHitSt12,firstHitSt45);
      for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	fCount++;
      }
    } else {
      Propagate(xr,yr,zr,lastHitSt12,9999);
    }
  } else if (firstHitSt3 >= 0) {
    SetPoint(fCount,xr[firstHitSt3],yr[firstHitSt3],zr[firstHitSt3]);
    fCount++;
    if (lastHitSt3 >= 0) {
      SetPoint(fCount,xr[lastHitSt3],yr[lastHitSt3],zr[lastHitSt3]);
      fCount++;
      if (firstHitSt45) {
	Propagate(xr,yr,zr,lastHitSt3,firstHitSt45);
	for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	  SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	  fCount++;
	}
      } else {
	Propagate(xr,yr,zr,lastHitSt3,9999);
      }
    } else if (firstHitSt45 >= 0) {
      Propagate(xr,yr,zr,firstHitSt3,firstHitSt45);
      for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	fCount++;
      }
    } else {
      Propagate(xr,yr,zr,firstHitSt3,9999);
    }
  } else if (lastHitSt3 >= 0) {
    SetPoint(fCount,xr[lastHitSt3],yr[lastHitSt3],zr[lastHitSt3]);
    fCount++;
    if (firstHitSt45 >= 0) {
      Propagate(xr,yr,zr,lastHitSt3,firstHitSt45);
      for (Int_t iHit = firstHitSt45; iHit < nTrackHits; iHit++) {
	SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
	fCount++;
      }
    } else {
      Propagate(xr,yr,zr,lastHitSt3,9999);
    }
  } else {
    for (Int_t iHit = 0; iHit < nTrackHits; iHit++) {
      SetPoint(fCount,xr[iHit],yr[iHit],zr[iHit]);
      fCount++;
    }
  }
  
  if (!fIsMUONTrack) return;

  Int_t nrc = 0;
  if (mtrack->GetMatchTrigger() && 1) {
    
    for (Int_t i = 0; i < nTrackHits; i++) {
      if (TMath::Abs(zr[i]) > 1000.0) {
	//printf("Hit %d x %f y %f z %f \n",iHit,xr[i],yr[i],zr[i]);
	xrc[nrc] = xr[i];
	yrc[nrc] = yr[i];
	zrc[nrc] = zr[i];
	nrc++;
      }
    }
    
    if (nrc < 2) return;
    
    // fit x-z
    smatrix.Zero();
    sums.Zero();
    for (Int_t i = 0; i < nrc; i++) {
      xv = (Double_t)zrc[i];
      yv = (Double_t)xrc[i];
      //printf("x-z: xv %f yv %f \n",xv,yv);
      smatrix(0,0) += 1.0;
      smatrix(1,1) += xv*xv;
      smatrix(0,1) += xv;
      smatrix(1,0) += xv;
      sums(0,0)    += yv;
      sums(1,0)    += xv*yv;
    }
    res = smatrix.Invert() * sums;
    ax = res(0,0);
    bx = res(1,0);
    
    // fit y-z
    smatrix.Zero();
    sums.Zero();
    for (Int_t i = 0; i < nrc; i++) {
      xv = (Double_t)zrc[i];
      yv = (Double_t)yrc[i];
      //printf("y-z: xv %f yv %f \n",xv,yv);
      smatrix(0,0) += 1.0;
      smatrix(1,1) += xv*xv;
      smatrix(0,1) += xv;
      smatrix(1,0) += xv;
      sums(0,0)    += yv;
      sums(1,0)    += xv*yv;
    }
    res = smatrix.Invert() * sums;
    ay = res(0,0);
    by = res(1,0);
    
    Float_t xtc, ytc, ztc;
    for (Int_t ii = 0; ii < 4; ii++) {
      
      ztc = zg[ii];
      ytc = ay+by*zg[ii];
      xtc = ax+bx*zg[ii];
      
      //printf("tc: x %f y %f z %f \n",xtc,ytc,ztc);
      
      SetPoint(fCount,xtc,ytc,ztc);
      fCount++;
      
    }
    
  }  // end match trigger

}

//______________________________________________________________________
void MUONTrack::MakeMUONTriggerTrack(AliMUONTriggerTrack *mtrack)
{
  //
  // builds the trigger track from one point and direction
  //

  Float_t x1   = mtrack->GetX11();
  Float_t y1   = mtrack->GetY11();
  Float_t thex = mtrack->GetThetax();
  Float_t they = mtrack->GetThetay();

  Float_t z11 = -1600.0;
  Float_t z22 = -1724.0;
  Float_t dz  = z22-z11;

  Float_t x2 = x1 + dz*TMath::Tan(thex);
  Float_t y2 = y1 + dz*TMath::Tan(they);

  SetPoint(fCount,x1,y1,z11); fCount++;
  SetPoint(fCount,x2,y2,z22); fCount++;

  char form[1000];

  sprintf(form,"MUONTriggerTrack %2d",mtrack->GetLoTrgNum());
  SetName(form);
  SetLineStyle(1);

}

//______________________________________________________________________
void MUONTrack::MakeESDTrack(AliESDMuonTrack *mtrack)
{
  //
  // builds the track with dipole field starting from the TParticle
  //

  fIsESDTrack = kTRUE;

  fTrack = new AliMUONTrack();
  AliMUONTrackParam trackParam;
  trackParam.GetParamFrom(*mtrack);
  fTrack->SetTrackParamAtVertex(&trackParam);
  fTrack->SetMatchTrigger(mtrack->GetMatchTrigger());

  char form[1000];
  sprintf(form,"ESDTrack %2d ", fLabel);
  SetName(form);
  SetLineStyle(3);
  SetLineColor(0);

  Double_t vect[7], vout[7];
  Double_t step = 1.0;

  Int_t charge = (Int_t)TMath::Sign(1.0,trackParam.GetInverseBendingMomentum());
  Float_t pv[3];
  pv[0] = trackParam.Px();
  pv[1] = trackParam.Py();
  pv[2] = trackParam.Pz();
  fP.Set(pv);
  
  vect[0] = trackParam.GetNonBendingCoor();
  vect[1] = trackParam.GetBendingCoor();
  vect[2] = trackParam.GetZ();
  vect[3] = trackParam.Px()/trackParam.P();
  vect[4] = trackParam.Py()/trackParam.P();
  vect[5] = trackParam.Pz()/trackParam.P();
  vect[6] = trackParam.P();

  //cout << "vertex " << vect[0] << "   " << vect[1] << "   " << vect[2] << "   " << endl;

  Double_t zMax = -1750.0;
  Double_t rMax =   350.0;
  Double_t r    =     0.0;

  Int_t nSteps = 0;
  while ((vect[2] > zMax) && (nSteps < 10000) && (r < rMax)) {
    nSteps++;
    OneStepRungekutta(charge, step, vect, vout);
    SetPoint(fCount,vout[0],vout[1],vout[2]);
    fCount++;
    for (Int_t i = 0; i < 7; i++) {
      vect[i] = vout[i];
    }
    r = TMath::Sqrt(vect[0]*vect[0]+vect[1]*vect[1]);
  }

}

//______________________________________________________________________
void MUONTrack::MakeMCTrack(TParticle *part)
{
  //
  // builds the track with dipole field starting from the TParticle
  //

  fIsMCTrack = kTRUE;

  fPart     = new TParticle(*part);

  char form[1000];
  sprintf(form,"MCTrack %2d ", fLabel);
  SetName(form);
  SetLineStyle(2);
  SetLineColor(8);

  Double_t vect[7], vout[7];
  Double_t step = 1.0;

  Float_t pv[3];
  pv[0] = fPart->Px();
  pv[1] = fPart->Py();
  pv[2] = fPart->Pz();
  fP.Set(pv);

  vect[0] = fPart->Vx();
  vect[1] = fPart->Vy();
  vect[2] = fPart->Vz();
  vect[3] = fPart->Px()/fPart->P();
  vect[4] = fPart->Py()/fPart->P();
  vect[5] = fPart->Pz()/fPart->P();
  vect[6] = fPart->P();

  TParticlePDG *ppdg = fPart->GetPDG(1);
  Int_t charge = (Int_t)(ppdg->Charge()/3.0);
  
  Double_t zMax = -1750.0;
  Double_t rMax =   350.0;
  Double_t r    =     0.0;

  Int_t nSteps = 0;
  while ((vect[2] > zMax) && (nSteps < 10000) && (r < rMax)) {
    nSteps++;
    OneStepRungekutta(charge, step, vect, vout);
    SetPoint(fCount,vout[0],vout[1],vout[2]);
    fCount++;
    for (Int_t i = 0; i < 7; i++) {
      vect[i] = vout[i];
    }
    r = TMath::Sqrt(vect[0]*vect[0]+vect[1]*vect[1]);
  }

}

//______________________________________________________________________
void MUONTrack::MakeRefTrack(AliMUONTrack *mtrack)
{
  //
  // builds the track with dipole field starting from the TParticle
  //

  fIsRefTrack = kTRUE;

  char form[1000];
  sprintf(form,"RefTrack %2d ", fLabel);
  SetName(form);
  SetLineStyle(2);
  SetLineColor(0);

  MakeMUONTrack(mtrack);

}

//______________________________________________________________________
void MUONTrack::Propagate(Float_t *xr, Float_t *yr, Float_t *zr, Int_t i1, Int_t i2)
{
  //
  // propagate in magnetic field between hits of indices i1 and i2
  //

  Double_t vect[7], vout[7];
  Double_t step = 1.0;
  Double_t zMax = 0.0;
  Int_t  charge =   0;
  AliMUONTrackParam *trackParam = 0;
  TClonesArray *trackParamAtHit = 0;

  if (i2 == 9999) {
    zMax = zr[i1]+1.5*step;
  } else {
    zMax = zr[i2]+1.5*step;
  }

  trackParamAtHit = fTrack->GetTrackParamAtHit();

  if (IsMUONTrack()) {
    trackParam = (AliMUONTrackParam*)trackParamAtHit->At(i1); 
    charge = (Int_t)TMath::Sign(1.0,trackParam->GetInverseBendingMomentum());
  }
  if (IsRefTrack()) {
    trackParam = fTrack->GetTrackParamAtVertex();
    charge = (Int_t)TMath::Sign(1.0,trackParam->GetInverseBendingMomentum());
    trackParam = (AliMUONTrackParam*)trackParamAtHit->At(i1); 
  }
  
  vect[0] = xr[i1];
  vect[1] = yr[i1];
  vect[2] = zr[i1];
  vect[3] = trackParam->Px()/trackParam->P();
  vect[4] = trackParam->Py()/trackParam->P();
  vect[5] = trackParam->Pz()/trackParam->P();
  vect[6] = trackParam->P();

  Int_t nSteps = 0;
  while ((vect[2] > zMax) && (nSteps < 10000)) {
    nSteps++;
    OneStepRungekutta(charge, step, vect, vout);
    SetPoint(fCount,vout[0],vout[1],vout[2]);
    fCount++;
    for (Int_t i = 0; i < 7; i++) {
      vect[i] = vout[i];
    }
  }
  
}

//______________________________________________________________________
void MUONTrack::GetField(Double_t *position, Double_t *field)
{
  // 
  // returns field components at position, for a give field map
  //

  /// interface for arguments in double precision (Why ? ChF)
  Float_t x[3], b[3];

  x[0] = position[0]; x[1] = position[1]; x[2] = position[2];

  if (fFieldMap) {
    fFieldMap->Field(x,b);
  }
  else {
    AliWarning("No field map");
    field[0] = field[1] = field[2] = 0.0;
    return;
  }
  
  // force components
  //b[1] = 0.0;
  //b[2] = 0.0;

  field[0] = b[0]; field[1] = b[1]; field[2] = b[2];

  return;

}

//______________________________________________________________________
void MUONTrack::OneStepRungekutta(Double_t charge, Double_t step, 
				  Double_t* vect, Double_t* vout)
{
///	******************************************************************
///	*								 *
///	*  Runge-Kutta method for tracking a particle through a magnetic *
///	*  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of	 *
///	*  Standards, procedure 25.5.20)				 *
///	*								 *
///	*  Input parameters						 *
///	*	CHARGE    Particle charge				 *
///	*	STEP	  Step size					 *
///	*	VECT	  Initial co-ords,direction cosines,momentum	 *
///	*  Output parameters						 *
///	*	VOUT	  Output co-ords,direction cosines,momentum	 *
///	*  User routine called  					 *
///	*	CALL GUFLD(X,F) 					 *
///	*								 *
///	*    ==>Called by : <USER>, GUSWIM				 *
///	*	Authors    R.Brun, M.Hansroul  *********		 *
///	*		   V.Perevoztchikov (CUT STEP implementation)	 *
///	*								 *
///	*								 *
///	******************************************************************

    Double_t h2, h4, f[4];
    Double_t xyzt[3], a, b, c, ph,ph2;
    Double_t secxs[4],secys[4],seczs[4],hxp[3];
    Double_t g1, g2, g3, g4, g5, g6, ang2, dxt, dyt, dzt;
    Double_t est, at, bt, ct, cba;
    Double_t f1, f2, f3, f4, rho, tet, hnorm, hp, rho1, sint, cost;
    
    Double_t x;
    Double_t y;
    Double_t z;
    
    Double_t xt;
    Double_t yt;
    Double_t zt;

    Double_t maxit = 1992;
    Double_t maxcut = 11;

    const Double_t kdlt   = 1e-4;
    const Double_t kdlt32 = kdlt/32.;
    const Double_t kthird = 1./3.;
    const Double_t khalf  = 0.5;
    const Double_t kec = 2.9979251e-4;

    const Double_t kpisqua = 9.86960440109;
    const Int_t kix  = 0;
    const Int_t kiy  = 1;
    const Int_t kiz  = 2;
    const Int_t kipx = 3;
    const Int_t kipy = 4;
    const Int_t kipz = 5;
  
    // *.
    // *.    ------------------------------------------------------------------
    // *.
    // *             this constant is for units cm,gev/c and kgauss
    // *
    Int_t iter = 0;
    Int_t ncut = 0;
    for(Int_t j = 0; j < 7; j++)
      vout[j] = vect[j];

    Double_t  pinv   = kec * charge / vect[6];
    Double_t tl = 0.;
    Double_t h = step;
    Double_t rest;

 
    do {
      rest  = step - tl;
      if (TMath::Abs(h) > TMath::Abs(rest)) h = rest;
      //cmodif: call gufld(vout,f) changed into:

      GetField(vout,f);

      // *
      // *             start of integration
      // *
      x      = vout[0];
      y      = vout[1];
      z      = vout[2];
      a      = vout[3];
      b      = vout[4];
      c      = vout[5];

      h2     = khalf * h;
      h4     = khalf * h2;
      ph     = pinv * h;
      ph2    = khalf * ph;
      secxs[0] = (b * f[2] - c * f[1]) * ph2;
      secys[0] = (c * f[0] - a * f[2]) * ph2;
      seczs[0] = (a * f[1] - b * f[0]) * ph2;
      ang2 = (secxs[0]*secxs[0] + secys[0]*secys[0] + seczs[0]*seczs[0]);
      if (ang2 > kpisqua) break;

      dxt    = h2 * a + h4 * secxs[0];
      dyt    = h2 * b + h4 * secys[0];
      dzt    = h2 * c + h4 * seczs[0];
      xt     = x + dxt;
      yt     = y + dyt;
      zt     = z + dzt;
      // *
      // *              second intermediate point
      // *

      est = TMath::Abs(dxt) + TMath::Abs(dyt) + TMath::Abs(dzt);
      if (est > h) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }
 
      xyzt[0] = xt;
      xyzt[1] = yt;
      xyzt[2] = zt;

      //cmodif: call gufld(xyzt,f) changed into:
      GetField(xyzt,f);

      at     = a + secxs[0];
      bt     = b + secys[0];
      ct     = c + seczs[0];

      secxs[1] = (bt * f[2] - ct * f[1]) * ph2;
      secys[1] = (ct * f[0] - at * f[2]) * ph2;
      seczs[1] = (at * f[1] - bt * f[0]) * ph2;
      at     = a + secxs[1];
      bt     = b + secys[1];
      ct     = c + seczs[1];
      secxs[2] = (bt * f[2] - ct * f[1]) * ph2;
      secys[2] = (ct * f[0] - at * f[2]) * ph2;
      seczs[2] = (at * f[1] - bt * f[0]) * ph2;
      dxt    = h * (a + secxs[2]);
      dyt    = h * (b + secys[2]);
      dzt    = h * (c + seczs[2]);
      xt     = x + dxt;
      yt     = y + dyt;
      zt     = z + dzt;
      at     = a + 2.*secxs[2];
      bt     = b + 2.*secys[2];
      ct     = c + 2.*seczs[2];

      est = TMath::Abs(dxt)+TMath::Abs(dyt)+TMath::Abs(dzt);
      if (est > 2.*TMath::Abs(h)) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }
 
      xyzt[0] = xt;
      xyzt[1] = yt;
      xyzt[2] = zt;

      //cmodif: call gufld(xyzt,f) changed into:
      GetField(xyzt,f);

      z      = z + (c + (seczs[0] + seczs[1] + seczs[2]) * kthird) * h;
      y      = y + (b + (secys[0] + secys[1] + secys[2]) * kthird) * h;
      x      = x + (a + (secxs[0] + secxs[1] + secxs[2]) * kthird) * h;

      secxs[3] = (bt*f[2] - ct*f[1])* ph2;
      secys[3] = (ct*f[0] - at*f[2])* ph2;
      seczs[3] = (at*f[1] - bt*f[0])* ph2;
      a      = a+(secxs[0]+secxs[3]+2. * (secxs[1]+secxs[2])) * kthird;
      b      = b+(secys[0]+secys[3]+2. * (secys[1]+secys[2])) * kthird;
      c      = c+(seczs[0]+seczs[3]+2. * (seczs[1]+seczs[2])) * kthird;

      est    = TMath::Abs(secxs[0]+secxs[3] - (secxs[1]+secxs[2]))
	+ TMath::Abs(secys[0]+secys[3] - (secys[1]+secys[2]))
	+ TMath::Abs(seczs[0]+seczs[3] - (seczs[1]+seczs[2]));

      if (est > kdlt && TMath::Abs(h) > 1.e-4) {
	if (ncut++ > maxcut) break;
	h *= khalf;
	continue;
      }

      ncut = 0;
      // *               if too many iterations, go to helix
      if (iter++ > maxit) break;

      tl += h;
      if (est < kdlt32) 
	h *= 2.;
      cba    = 1./ TMath::Sqrt(a*a + b*b + c*c);
      vout[0] = x;
      vout[1] = y;
      vout[2] = z;
      vout[3] = cba*a;
      vout[4] = cba*b;
      vout[5] = cba*c;
      rest = step - tl;
      if (step < 0.) rest = -rest;
      if (rest < 1.e-5*TMath::Abs(step)) return;

    } while(1);

    // angle too big, use helix

    f1  = f[0];
    f2  = f[1];
    f3  = f[2];
    f4  = TMath::Sqrt(f1*f1+f2*f2+f3*f3);
    rho = -f4*pinv;
    tet = rho * step;
 
    hnorm = 1./f4;
    f1 = f1*hnorm;
    f2 = f2*hnorm;
    f3 = f3*hnorm;

    hxp[0] = f2*vect[kipz] - f3*vect[kipy];
    hxp[1] = f3*vect[kipx] - f1*vect[kipz];
    hxp[2] = f1*vect[kipy] - f2*vect[kipx];
 
    hp = f1*vect[kipx] + f2*vect[kipy] + f3*vect[kipz];

    rho1 = 1./rho;
    sint = TMath::Sin(tet);
    cost = 2.*TMath::Sin(khalf*tet)*TMath::Sin(khalf*tet);

    g1 = sint*rho1;
    g2 = cost*rho1;
    g3 = (tet-sint) * hp*rho1;
    g4 = -cost;
    g5 = sint;
    g6 = cost * hp;
 
    vout[kix] = vect[kix] + g1*vect[kipx] + g2*hxp[0] + g3*f1;
    vout[kiy] = vect[kiy] + g1*vect[kipy] + g2*hxp[1] + g3*f2;
    vout[kiz] = vect[kiz] + g1*vect[kipz] + g2*hxp[2] + g3*f3;
 
    vout[kipx] = vect[kipx] + g4*vect[kipx] + g5*hxp[0] + g6*f1;
    vout[kipy] = vect[kipy] + g4*vect[kipy] + g5*hxp[1] + g6*f2;
    vout[kipz] = vect[kipz] + g4*vect[kipz] + g5*hxp[2] + g6*f3;

    return;
}

//______________________________________________________________________
Int_t MUONTrack::ColorIndex(Float_t val)
{
  //
  // returns color index in the palette for a give value
  //

  Float_t threshold =  0.0;
  Float_t maxVal    =  2.0;

  Float_t div  = TMath::Max(1, (Int_t)(maxVal - threshold));
  Int_t   nCol = gStyle->GetNumberOfColors();
  Int_t   cBin = (Int_t) TMath::Nint(nCol*(val - threshold)/div);

  return gStyle->GetColorPalette(TMath::Min(nCol - 1, cBin));

}

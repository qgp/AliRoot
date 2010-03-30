/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercialf purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

/* $Id: AliTRDresolution.cxx 27496 2008-07-22 08:35:45Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  TRD tracking resolution                                               //
//
// The class performs resolution and residual studies 
// of the TRD tracks for the following quantities :
//   - spatial position (y, [z])
//   - angular (phi) tracklet
//   - momentum at the track level
// 
// The class has to be used for regular detector performance checks using the official macros:
//   - $ALICE_ROOT/TRD/qaRec/run.C
//   - $ALICE_ROOT/TRD/qaRec/makeResults.C
// 
// For stand alone usage please refer to the following example: 
// {  
//   gSystem->Load("libANALYSIS.so");
//   gSystem->Load("libTRDqaRec.so");
//   AliTRDresolution *res = new AliTRDresolution();
//   //res->SetMCdata();
//   //res->SetVerbose();
//   //res->SetVisual();
//   res->Load();
//   if(!res->PostProcess()) return;
//   res->GetRefFigure(0);
// }  
//
//  Authors:                                                              //
//    Alexandru Bercuci <A.Bercuci@gsi.de>                                //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TObjArray.h>
#include <TH3.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGaxis.h>
#include <TBox.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TMatrixT.h>
#include <TVectorT.h>
#include <TTreeStream.h>
#include <TGeoManager.h>

#include "AliPID.h"

#include "AliTRDresolution.h"
#include "AliTRDgeometry.h"
#include "AliTRDpadPlane.h"
#include "AliTRDcluster.h"
#include "AliTRDseedV1.h"
#include "AliTRDtrackV1.h"
#include "AliTRDReconstructor.h"
#include "AliTRDrecoParam.h"

#include "info/AliTRDclusterInfo.h"

ClassImp(AliTRDresolution)

UChar_t const AliTRDresolution::fgNElements[kNhistos] = {
  2, 2, 5, 5,
  2, 5, 12, 2, 11
};
Char_t const * AliTRDresolution::fgPerformanceName[kNhistos] = {
     "Charge"
    ,"Cluster2Track"
    ,"Tracklet2Track"
    ,"Tracklet2TPC" 
    ,"Cluster2MC"
    ,"Tracklet2MC"
    ,"TPC2MC"
    ,"TOF/HMPID2MC"
    ,"TRD2MC"
};
Char_t const *AliTRDresolution::fgAxTitle[46][4] = {
  // Charge
  {"Impv", "x [cm]", "I_{mpv}", "x/x_{0}"}
 ,{"dI/Impv", "x/x_{0}", "#delta I/I_{mpv}", "x[cm]"}
  // Clusters to Kalman
 ,{"Pos", "tg(#phi)", "#mu_{y}^{cl} [#mum]", "#sigma_{y}^{cl} [#mum]"}
 ,{"Pulls", "tg(#phi)", "PULL: #mu_{y}^{cl}", "PULL: #sigma_{y}^{cl}"}
  // TRD tracklet to Kalman fit
 ,{"PosY", "tg(#phi)", "#mu_{y}^{trklt} [#mum]", "#sigma_{y}^{trklt} [#mum]"}
 ,{"PullsY", "tg(#phi)", "PULL: #mu_{y}^{trklt}", "PULL: #sigma_{y}^{trklt}"}
 ,{"PosZ", "tg(#theta)", "#mu_{z}^{trklt} [#mum]", "#sigma_{z}^{trklt} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "PULL: #mu_{z}^{trklt}", "PULL: #sigma_{z}^{trklt}"}
 ,{"Phi", "tg(#phi)", "#mu_{#phi}^{trklt} [mrad]", "#sigma_{#phi}^{trklt} [mrad]"}
  // TPC track 2 first TRD tracklet
 ,{"PosY", "tg(#phi)", "#mu_{y}^{TPC trklt} [#mum]", "#sigma_{y}^{TPC trklt} [#mum]"}
 ,{"PullsY", "tg(#phi)", "PULL: #mu_{y}^{TPC trklt}", "PULL: #sigma_{y}^{TPC trklt}"}
 ,{"PosZ", "tg(#theta)", "#mu_{z}^{TPC trklt} [#mum]", "#sigma_{z}^{TPC trklt} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "PULL: #mu_{z}^{TPC trklt}", "PULL: #sigma_{z}^{TPC trklt}"}
 ,{"Phi", "tg(#phi)", "#mu_{#phi}^{TPC trklt} [mrad]", "#sigma_{#phi}^{TPC trklt} [mrad]"}
  // MC cluster
 ,{"Pos", "tg(#phi)", "MC: #mu_{y}^{cl} [#mum]", "MC: #sigma_{y}^{cl} [#mum]"}
 ,{"Pulls", "tg(#phi)", "MC PULL: #mu_{y}^{cl}", "MC PULL: #sigma_{y}^{cl}"}
  // MC tracklet
 ,{"PosY", "tg(#phi)", "MC: #mu_{y}^{trklt} [#mum]", "MC: #sigma_{y}^{trklt} [#mum]"}
 ,{"PullsY", "tg(#phi)", "MC PULL: #mu_{y}^{trklt}", "MC PULL: #sigma_{y}^{trklt}"}
 ,{"PosZ", "tg(#theta)", "MC: #mu_{z}^{trklt} [#mum]", "MC: #sigma_{z}^{trklt} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "MC PULL: #mu_{z}^{trklt}", "MC PULL: #sigma_{z}^{trklt}"}
 ,{"Phi", "tg(#phi)", "MC: #mu_{#phi}^{trklt} [mrad]", "MC: #sigma_{#phi}^{trklt} [mrad]"}
  // MC track TPC
 ,{"PosY", "tg(#phi)", "MC: #mu_{y}^{TPC} [#mum]", "MC: #sigma_{y}^{TPC} [#mum]"}
 ,{"PullsY", "tg(#phi)", "MC PULL: #mu_{y}^{TPC}", "MC PULL: #sigma_{y}^{TPC}"}
 ,{"PosZ", "tg(#theta)", "MC: #mu_{z}^{TPC} [#mum]", "MC: #sigma_{z}^{TPC} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "MC PULL: #mu_{z}^{TPC}", "MC PULL: #sigma_{z}^{TPC}"}
 ,{"Phi", "tg(#phi)", "MC: #mu_{#phi}^{TPC} [mrad]", "MC: #sigma_{#phi}^{TPC} [mrad]"}
 ,{"PullsSNP", "tg(#phi)", "MC PULL: #mu_{snp}^{TPC}", "MC PULL: #sigma_{snp}^{TPC}"}
 ,{"Theta", "tg(#theta)", "MC: #mu_{#theta}^{TPC} [mrad]", "MC: #sigma_{#theta}^{TPC} [mrad]"}
 ,{"PullsTGL", "tg(#theta)", "MC PULL: #mu_{tgl}^{TPC}", "MC PULL: #sigma_{tgl}^{TPC}"}
 ,{"Pt", "p_{t}^{MC} [GeV/c]", "MC: #mu^{TPC}(#Deltap_{t}/p_{t}^{MC}) [%]", "MC: #sigma^{TPC}(#Deltap_{t}/p_{t}^{MC}) [%]"}
 ,{"Pulls1Pt", "1/p_{t}^{MC} [c/GeV]", "MC PULL: #mu_{1/p_{t}}^{TPC}", "MC PULL: #sigma_{1/p_{t}}^{TPC}"}
 ,{"P", "p^{MC} [GeV/c]", "MC: #mu^{TPC}(#Deltap/p^{MC}) [%]", "MC: #sigma^{TPC}(#Deltap/p^{MC}) [%]"}
 ,{"PullsP", "p^{MC} [GeV/c]", "MC PULL: #mu^{TPC}(#Deltap/#sigma_{p})", "MC PULL: #sigma^{TPC}(#Deltap/#sigma_{p})"}
  // MC track TOF
 ,{"PosZ", "tg(#theta)", "MC: #mu_{z}^{TOF} [#mum]", "MC: #sigma_{z}^{TOF} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "MC PULL: #mu_{z}^{TOF}", "MC PULL: #sigma_{z}^{TOF}"}
  // MC track in TRD
 ,{"PosY", "tg(#phi)", "MC: #mu_{y}^{Trk} [#mum]", "MC: #sigma_{y}^{Trk} [#mum]"}
 ,{"PullsY", "tg(#phi)", "MC PULL: #mu_{y}^{Trk}", "MC PULL: #sigma_{y}^{Trk}"}
 ,{"PosZ", "tg(#theta)", "MC: #mu_{z}^{Trk} [#mum]", "MC: #sigma_{z}^{Trk} [#mum]"}
 ,{"PullsZ", "tg(#theta)", "MC PULL: #mu_{z}^{Trk}", "MC PULL: #sigma_{z}^{Trk}"}
 ,{"Phi", "tg(#phi)", "MC: #mu_{#phi}^{Trk} [mrad]", "MC: #sigma_{#phi}^{Trk} [mrad]"}
 ,{"PullsSNP", "tg(#phi)", "MC PULL: #mu_{snp}^{Trk}", "MC PULL: #sigma_{snp}^{Trk}"}
 ,{"Theta", "tg(#theta)", "MC: #mu_{#theta}^{Trk} [mrad]", "MC: #sigma_{#theta}^{Trk} [mrad]"}
 ,{"PullsTGL", "tg(#theta)", "MC PULL: #mu_{tgl}^{Trk}", "MC PULL: #sigma_{tgl}^{Trk}"}
 ,{"Pt", "p_{t}^{MC} [GeV/c]", "MC: #mu^{Trk}(#Deltap_{t}/p_{t}^{MC}) [%]", "MC: #sigma^{Trk}(#Deltap_{t}/p_{t}^{MC}) [%]"}
 ,{"Pulls1Pt", "1/p_{t}^{MC} [c/GeV]", "MC PULL: #mu_{1/p_{t}}^{Trk}", "MC PULL: #sigma_{1/p_{t}}^{Trk}"}
 ,{"P", "p^{MC} [GeV/c]", "MC: #mu^{Trk}(#Deltap/p^{MC}) [%]", "MC: #sigma^{Trk}(#Deltap/p^{MC}) [%]"}
};

//________________________________________________________
AliTRDresolution::AliTRDresolution()
  :AliTRDrecoTask("resolution", "Spatial and momentum TRD resolution checker")
  ,fStatus(0)
  ,fIdxPlot(0)
  ,fReconstructor(NULL)
  ,fGeo(NULL)
  ,fGraphS(NULL)
  ,fGraphM(NULL)
  ,fCl(NULL)
  ,fTrklt(NULL)
  ,fMCcl(NULL)
  ,fMCtrklt(NULL)
{
  //
  // Default constructor
  //

  fReconstructor = new AliTRDReconstructor();
  fReconstructor->SetRecoParam(AliTRDrecoParam::GetLowFluxParam());
  fGeo = new AliTRDgeometry();

  InitFunctorList();

  DefineOutput(1, TObjArray::Class()); // cluster2track
  DefineOutput(2, TObjArray::Class()); // tracklet2track
  DefineOutput(3, TObjArray::Class()); // cluster2mc
  DefineOutput(4, TObjArray::Class()); // tracklet2mc
}

//________________________________________________________
AliTRDresolution::~AliTRDresolution()
{
  //
  // Destructor
  //

  if(fGraphS){fGraphS->Delete(); delete fGraphS;}
  if(fGraphM){fGraphM->Delete(); delete fGraphM;}
  delete fGeo;
  delete fReconstructor;
  if(gGeoManager) delete gGeoManager;
  if(fCl){fCl->Delete(); delete fCl;}
  if(fTrklt){fTrklt->Delete(); delete fTrklt;}
  if(fMCcl){fMCcl->Delete(); delete fMCcl;}
  if(fMCtrklt){fMCtrklt->Delete(); delete fMCtrklt;}
}


//________________________________________________________
void AliTRDresolution::CreateOutputObjects()
{
  // spatial resolution
  OpenFile(0, "RECREATE");

  fContainer = Histos();

  fCl = new TObjArray();
  fCl->SetOwner(kTRUE);
  fTrklt = new TObjArray();
  fTrklt->SetOwner(kTRUE);
  fMCcl = new TObjArray();
  fMCcl->SetOwner(kTRUE);
  fMCtrklt = new TObjArray();
  fMCtrklt->SetOwner(kTRUE);
}

//________________________________________________________
void AliTRDresolution::Exec(Option_t *opt)
{
  //
  // Execution part
  //

  fCl->Delete();
  fTrklt->Delete();
  fMCcl->Delete();
  fMCtrklt->Delete();

  AliTRDrecoTask::Exec(opt);

  PostData(1, fCl);
  PostData(2, fTrklt);
  PostData(3, fMCcl);
  PostData(4, fMCtrklt);
}

//________________________________________________________
TH1* AliTRDresolution::PlotCharge(const AliTRDtrackV1 *track)
{
  //
  // Plots the charge distribution
  //

  if(track) fkTrack = track;
  if(!fkTrack){
    AliWarning("No Track defined.");
    return NULL;
  }
  TObjArray *arr = NULL;
  if(!(arr = ((TObjArray*)fContainer->At(kCharge)))){
    AliWarning("No output container defined.");
    return NULL;
  }
  TH3S* h = NULL;

  AliTRDseedV1 *fTracklet = NULL;  
  AliTRDcluster *c = NULL;
  for(Int_t ily=0; ily<AliTRDgeometry::kNlayer; ily++){
    if(!(fTracklet = fkTrack->GetTracklet(ily))) continue;
    if(!fTracklet->IsOK()) continue;
    Float_t x0 = fTracklet->GetX0();
    Float_t dq, dl;
    for(Int_t itb=AliTRDseedV1::kNtb; itb--;){
      if(!(c = fTracklet->GetClusters(itb))){ 
        if(!(c = fTracklet->GetClusters(AliTRDseedV1::kNtb+itb))) continue;
      }
      dq = fTracklet->GetdQdl(itb, &dl);
      dl /= 0.15; // dl/dl0, dl0 = 1.5 mm for nominal vd
      (h = (TH3S*)arr->At(0))->Fill(dl, x0-c->GetX(), dq);
    }

//     if(!HasMCdata()) continue;
//     UChar_t s;
//     Float_t pt0, y0, z0, dydx0, dzdx0;
//     if(!fMC->GetDirections(x0, y0, z0, dydx0, dzdx0, pt0, s)) continue;

  }
  return h;
}


//________________________________________________________
TH1* AliTRDresolution::PlotCluster(const AliTRDtrackV1 *track)
{
  //
  // Plot the cluster distributions
  //

  if(track) fkTrack = track;
  if(!fkTrack){
    AliWarning("No Track defined.");
    return NULL;
  }
  TObjArray *arr = NULL;
  if(!(arr = ((TObjArray*)fContainer->At(kCluster)))){
    AliWarning("No output container defined.");
    return NULL;
  }
  ULong_t status = fkESD ? fkESD->GetStatus():0;

  Double_t cov[7];
  Float_t x0, y0, z0, dy, dydx, dzdx;
  AliTRDseedV1 *fTracklet = NULL;  
  for(Int_t ily=0; ily<AliTRDgeometry::kNlayer; ily++){
    if(!(fTracklet = fkTrack->GetTracklet(ily))) continue;
    if(!fTracklet->IsOK()) continue;
    x0 = fTracklet->GetX0();

    // retrive the track angle with the chamber
    y0   = fTracklet->GetYref(0);
    z0   = fTracklet->GetZref(0);
    dydx = fTracklet->GetYref(1);
    dzdx = fTracklet->GetZref(1);
    fTracklet->GetCovRef(cov);
    Float_t tilt = fTracklet->GetTilt();
    AliTRDcluster *c = NULL;
    fTracklet->ResetClusterIter(kFALSE);
    while((c = fTracklet->PrevCluster())){
      Float_t xc = c->GetX();
      Float_t yc = c->GetY();
      Float_t zc = c->GetZ();
      Float_t dx = x0 - xc; 
      Float_t yt = y0 - dx*dydx;
      Float_t zt = z0 - dx*dzdx; 
      yc -= tilt*(zc-zt); // tilt correction
      dy = yt - yc;

      //Float_t sx2 = dydx*c->GetSX(c->GetLocalTimeBin()); sx2*=sx2;
      Float_t sy2 = c->GetSigmaY2();
      if(sy2<=0.) continue;
      ((TH2I*)arr->At(0))->Fill(dydx, dy);
      ((TH2I*)arr->At(1))->Fill(dydx, dy/TMath::Sqrt(cov[0] /*+ sx2*/ + sy2));
  
      if(DebugLevel()>=2){
        // Get z-position with respect to anode wire
        Int_t istk = fGeo->GetStack(c->GetDetector());
        AliTRDpadPlane *pp = fGeo->GetPadPlane(ily, istk);
        Float_t row0 = pp->GetRow0();
        Float_t d  =  row0 - zt + pp->GetAnodeWireOffset();
        d -= ((Int_t)(2 * d)) / 2.0;
        if (d > 0.25) d  = 0.5 - d;

        AliTRDclusterInfo *clInfo = new AliTRDclusterInfo;
        fCl->Add(clInfo);
        clInfo->SetCluster(c);
        Float_t covR[] = {cov[0], cov[1], cov[2]};
        clInfo->SetGlobalPosition(yt, zt, dydx, dzdx, covR);
        clInfo->SetResolution(dy);
        clInfo->SetAnisochronity(d);
        clInfo->SetDriftLength(dx);
        (*DebugStream()) << "ClusterREC"
          <<"status="  << status
          <<"clInfo.=" << clInfo
          << "\n";
      }
    }
  }
  return (TH2I*)arr->At(0);
}


//________________________________________________________
TH1* AliTRDresolution::PlotTracklet(const AliTRDtrackV1 *track)
{
// Plot normalized residuals for tracklets to track. 
// 
// We start from the result that if X=N(|m|, |Cov|)
// BEGIN_LATEX
// (Cov^{-1})^{1/2}X = N((Cov^{-1})^{1/2}*|m|, |1|)
// END_LATEX
// in our case X=(y_trklt - y_trk z_trklt - z_trk) and |Cov| = |Cov_trklt| + |Cov_trk| at the radial 
// reference position. 
  if(track) fkTrack = track;
  if(!fkTrack){
    AliWarning("No Track defined.");
    return NULL;
  }
  TObjArray *arr = NULL;
  if(!(arr = (TObjArray*)fContainer->At(kTrackTRD ))){
    AliWarning("No output container defined.");
    return NULL;
  }

  Double_t cov[3], covR[7]/*, sqr[3], inv[3]*/;
  Float_t x, dx, dy, dz;
  AliTRDseedV1 *fTracklet = NULL;  
  for(Int_t il=AliTRDgeometry::kNlayer; il--;){
    if(!(fTracklet = fkTrack->GetTracklet(il))) continue;
    if(!fTracklet->IsOK()) continue;
    x    = fTracklet->GetX();
    dx   = fTracklet->GetX0() - x;
    // compute dy^2 and dz^2
    dy   = fTracklet->GetYref(0)-dx*fTracklet->GetYref(1) - fTracklet->GetY();
    dz   = fTracklet->GetZref(0)-dx*fTracklet->GetZref(1) - fTracklet->GetZ();
    // compute covariance matrix
    fTracklet->GetCovAt(x, cov);
    fTracklet->GetCovRef(covR);
    cov[0] += covR[0]; cov[1] += covR[1]; cov[2] += covR[2]; 
/*  // Correct PULL calculation by considering off  
    // diagonal elements in the covariance matrix
    // compute square root matrix
    if(AliTRDseedV1::GetCovInv(cov, inv)==0.) continue;
    if(AliTRDseedV1::GetCovSqrt(inv, sqr)<0.) continue;
    Double_t y = sqr[0]*dy+sqr[1]*dz;
    Double_t z = sqr[1]*dy+sqr[2]*dz;
    ((TH3*)h)->Fill(y, z, fTracklet->GetYref(1));*/

    ((TH2I*)arr->At(0))->Fill(fTracklet->GetYref(1), dy);
    ((TH2I*)arr->At(1))->Fill(fTracklet->GetYref(1), dy/TMath::Sqrt(cov[0]));
    ((TH2I*)arr->At(4))->Fill(fTracklet->GetYref(1), TMath::ATan((fTracklet->GetYref(1)-fTracklet->GetYfit(1))/(1-fTracklet->GetYref(1)*fTracklet->GetYfit(1))));
    if(!fTracklet->IsRowCross()) continue;
    ((TH2I*)arr->At(2))->Fill(fTracklet->GetZref(1), dz);
    ((TH2I*)arr->At(3))->Fill(fTracklet->GetZref(1), dz/TMath::Sqrt(cov[2]));
  }


  return (TH2I*)arr->At(0);
}


//________________________________________________________
TH1* AliTRDresolution::PlotTrackTPC(const AliTRDtrackV1 *track)
{
// Store resolution/pulls of Kalman before updating with the TRD information 
// at the radial position of the first tracklet. The following points are used 
// for comparison  
//  - the (y,z,snp) of the first TRD tracklet
//  - the (y, z, snp, tgl, pt) of the MC track reference
// 
// Additionally the momentum resolution/pulls are calculated for usage in the 
// PID calculation. 

  if(track) fkTrack = track;
  if(!fkTrack){
    AliWarning("No Track defined.");
    return NULL;
  }
  AliExternalTrackParam *tin = NULL;
  if(!(tin = fkTrack->GetTrackLow())){
    AliWarning("Track did not entered TRD fiducial volume.");
    return NULL;
  }
  TH1 *h = NULL;
  
  Double_t x = tin->GetX();
  AliTRDseedV1 *tracklet = NULL;  
  for(Int_t ily=0; ily<AliTRDgeometry::kNlayer; ily++){
    if(!(tracklet = fkTrack->GetTracklet(ily))) continue;
    break;
  }
  if(!tracklet || TMath::Abs(x-tracklet->GetX())>1.e-3){
    AliWarning("Tracklet did not match TRD entrance.");
    return NULL;
  }
  const Int_t kNPAR(5);
  Double_t parR[kNPAR]; memcpy(parR, tin->GetParameter(), kNPAR*sizeof(Double_t));
  Double_t covR[3*kNPAR]; memcpy(covR, tin->GetCovariance(), 3*kNPAR*sizeof(Double_t));
  Double_t cov[3]; tracklet->GetCovAt(x, cov);

  // define sum covariances
  TMatrixDSym COV(kNPAR); TVectorD PAR(kNPAR);
  Double_t *pc = &covR[0], *pp = &parR[0];
  for(Int_t ir=0; ir<kNPAR; ir++, pp++){
    PAR(ir) = (*pp);
    for(Int_t ic = 0; ic<=ir; ic++,pc++){ 
      COV(ir,ic) = (*pc); COV(ic,ir) = (*pc);
    }
  }
  PAR[4] = TMath::Abs(PAR[4]); // remove sign of pt !!
  //COV.Print(); PAR.Print();

  //TODO Double_t dydx =  TMath::Sqrt(1.-parR[2]*parR[2])/parR[2]; 
  Double_t dy = parR[0] - tracklet->GetY(); 
  TObjArray *arr = (TObjArray*)fContainer->At(kTrackTPC);
  ((TH2I*)arr->At(0))->Fill(tracklet->GetYref(1), dy);
  ((TH2I*)arr->At(1))->Fill(tracklet->GetYref(1), dy/TMath::Sqrt(COV(0,0)+cov[0]));
  if(tracklet->IsRowCross()){
    Double_t dz = parR[1] - tracklet->GetZ(); 
    ((TH2I*)arr->At(2))->Fill(tracklet->GetZref(1), dz);
    ((TH2I*)arr->At(3))->Fill(tracklet->GetZref(1), dz/TMath::Sqrt(COV(1,1)+cov[2]));
  }
  Double_t dphi = TMath::ASin(PAR[2])-TMath::ATan(tracklet->GetYfit(1));  ((TH2I*)arr->At(4))->Fill(tracklet->GetYref(1), dphi);


  // register reference histo for mini-task
  h = (TH2I*)arr->At(0);

  if(DebugLevel()>=1){
    (*DebugStream()) << "trackIn"
      << "x="       << x
      << "P="       << &PAR
      << "C="       << &COV
      << "\n";

    Double_t y = tracklet->GetY(); 
    Double_t z = tracklet->GetZ(); 
    (*DebugStream()) << "trackletIn"
      << "y="       << y
      << "z="       << z
      << "Vy="      << cov[0]
      << "Cyz="     << cov[1]
      << "Vz="      << cov[2]
      << "\n";
  }


  if(!HasMCdata()) return h;
  UChar_t s;
  Float_t dx, pt0, x0=tracklet->GetX0(), y0, z0, dydx0, dzdx0;
  if(!fkMC->GetDirections(x0, y0, z0, dydx0, dzdx0, pt0, s)) return h;
  // translate to reference radial position
  dx = x0 - x; y0 -= dx*dydx0; z0 -= dx*dzdx0;
  Float_t norm = 1./TMath::Sqrt(1.+dydx0*dydx0); // 1/sqrt(1+tg^2(phi))
  //Fill MC info
  TVectorD PARMC(kNPAR);
  PARMC[0]=y0; PARMC[1]=z0;
  PARMC[2]=dydx0*norm; PARMC[3]=dzdx0*norm;
  PARMC[4]=1./pt0;

//   TMatrixDSymEigen eigen(COV);
//   TVectorD evals = eigen.GetEigenValues();
//   TMatrixDSym evalsm(kNPAR);
//   for(Int_t ir=0; ir<kNPAR; ir++) for(Int_t ic=0; ic<kNPAR; ic++) evalsm(ir,ic) = (ir==ic ? evals(ir): 0.);
//   TMatrixD evecs = eigen.GetEigenVectors();
//   TMatrixD sqrcov(evecs, TMatrixD::kMult, TMatrixD(evalsm, TMatrixD::kMult, evecs.T()));
  
  // fill histos
  arr = (TObjArray*)fContainer->At(kMCtrackTPC);
  // y resolution/pulls
  ((TH2I*)arr->At(0))->Fill(dydx0, PARMC[0]-PAR[0]);
  ((TH2I*)arr->At(1))->Fill(dydx0, (PARMC[0]-PAR[0])/TMath::Sqrt(COV(0,0)));
  // z resolution/pulls
  ((TH2I*)arr->At(2))->Fill(dzdx0, PARMC[1]-PAR[1]);
  ((TH2I*)arr->At(3))->Fill(dzdx0, (PARMC[1]-PAR[1])/TMath::Sqrt(COV(1,1)));
  // phi resolution/snp pulls
  ((TH2I*)arr->At(4))->Fill(dydx0, TMath::ASin(PARMC[2])-TMath::ASin(PAR[2]));
  ((TH2I*)arr->At(5))->Fill(dydx0, (PARMC[2]-PAR[2])/TMath::Sqrt(COV(2,2)));
  // theta resolution/tgl pulls
  ((TH2I*)arr->At(6))->Fill(dzdx0, TMath::ATan((PARMC[3]-PAR[3])/(1-PARMC[3]*PAR[3])));
  ((TH2I*)arr->At(7))->Fill(dzdx0, (PARMC[3]-PAR[3])/TMath::Sqrt(COV(3,3)));
  // pt resolution\\1/pt pulls\\p resolution/pull
  for(Int_t is=AliPID::kSPECIES; is--;){
    if(TMath::Abs(fkMC->GetPDG())!=AliPID::ParticleCode(is)) continue;
    ((TH3S*)arr->At(8))->Fill(pt0, PARMC[4]/PAR[4]-1., is);
    ((TH3S*)arr->At(9))->Fill(PARMC[4], (PARMC[4]-PAR[4])/TMath::Sqrt(COV(4,4)), is);

    Double_t p0 = TMath::Sqrt(1.+ PARMC[3]*PARMC[3])*pt0, p;
    Float_t sp;
    p = tracklet->GetMomentum(&sp);
    ((TH3S*)arr->At(10))->Fill(p0, p/p0-1., is);
    ((TH3S*)arr->At(11))->Fill(p0, (p0-p)/sp, is);
    break;
  }

  // fill debug for MC 
  if(DebugLevel()>=1){
    (*DebugStream()) << "trackInMC"
      << "P="   << &PARMC
      << "\n";
  }
  return h;
}

//________________________________________________________
TH1* AliTRDresolution::PlotMC(const AliTRDtrackV1 *track)
{
  //
  // Plot MC distributions
  //

  if(!HasMCdata()){ 
    AliWarning("No MC defined. Results will not be available.");
    return NULL;
  }
  if(track) fkTrack = track;
  if(!fkTrack){
    AliWarning("No Track defined.");
    return NULL;
  }
  TObjArray *arr = NULL;
  TH1 *h = NULL;
  UChar_t s;
  Int_t pdg = fkMC->GetPDG(), det=-1;
  Int_t label = fkMC->GetLabel();
  Double_t xAnode, x, y, z, pt, dydx, dzdx, dzdl;
  Float_t pt0, x0, y0, z0, dx, dy, dz, dydx0, dzdx0;
  Double_t covR[7]/*, cov[3]*/;

  if(DebugLevel()>=1){
    Double_t dX[12], dY[12], dZ[12], dPt[12], cCOV[12][15];
    fkMC->PropagateKalman(dX, dY, dZ, dPt, cCOV);
    (*DebugStream()) << "MCkalman"
      << "pdg="  << pdg
      << "dx0="  << dX[0]
      << "dx1="  << dX[1]
      << "dx2="  << dX[2]
      << "dy0="  << dY[0]
      << "dy1="  << dY[1]
      << "dy2="  << dY[2]
      << "dz0="  << dZ[0]
      << "dz1="  << dZ[1]
      << "dz2="  << dZ[2]
      << "dpt0=" << dPt[0]
      << "dpt1=" << dPt[1]
      << "dpt2=" << dPt[2]
      << "\n";
  }

  AliTRDReconstructor rec;
  AliTRDseedV1 *fTracklet(NULL); TObjArray *clInfoArr(NULL);
  for(Int_t ily=0; ily<AliTRDgeometry::kNlayer; ily++){
    if(!(fTracklet = fkTrack->GetTracklet(ily)))/* ||
       !fTracklet->IsOK())*/ continue;

    det = fTracklet->GetDetector();
    x0  = fTracklet->GetX0();
    //radial shift with respect to the MC reference (radial position of the pad plane)
    x= fTracklet->GetX();
    if(!fkMC->GetDirections(x0, y0, z0, dydx0, dzdx0, pt0, s)) continue;
    xAnode  = fTracklet->GetX0();

    // MC track position at reference radial position
    dx  = x0 - x;
    if(DebugLevel()>=1){
      (*DebugStream()) << "MC"
        << "det="     << det
        << "pdg="     << pdg
        << "pt="      << pt0
        << "x="       << x0
        << "y="       << y0
        << "z="       << z0
        << "dydx="    << dydx0
        << "dzdx="    << dzdx0
        << "\n";
    }
    Float_t ymc = y0 - dx*dydx0;
    Float_t zmc = z0 - dx*dzdx0;
    //p = pt0*TMath::Sqrt(1.+dzdx0*dzdx0); // pt -> p

    // Kalman position at reference radial position
    dx = xAnode - x;
    dydx = fTracklet->GetYref(1);
    dzdx = fTracklet->GetZref(1);
    dzdl = fTracklet->GetTgl();
    y  = fTracklet->GetYref(0) - dx*dydx;
    dy = y - ymc;
    z  = fTracklet->GetZref(0) - dx*dzdx;
    dz = z - zmc;
    pt = TMath::Abs(fTracklet->GetPt());
    fTracklet->GetCovRef(covR);

    arr = (TObjArray*)fContainer->At(kMCtrackTRD);
    // y resolution/pulls
    ((TH2I*)arr->At(0))->Fill(dydx0, dy);
    ((TH2I*)arr->At(1))->Fill(dydx0, dy/TMath::Sqrt(covR[0]));
    // z resolution/pulls
    ((TH2I*)arr->At(2))->Fill(dzdx0, dz);
    ((TH2I*)arr->At(3))->Fill(dzdx0, dz/TMath::Sqrt(covR[2]));
    // phi resolution/ snp pulls
    Double_t dtgp = (dydx - dydx0)/(1.- dydx*dydx0);
    ((TH2I*)arr->At(4))->Fill(dydx0, TMath::ATan(dtgp));
    Double_t dsnp = dydx/TMath::Sqrt(1.+dydx*dydx) - dydx0/TMath::Sqrt(1.+dydx0*dydx0);
    ((TH2I*)arr->At(5))->Fill(dydx0, dsnp/TMath::Sqrt(covR[3]));
    // theta resolution/ tgl pulls
    Double_t dzdl0 = dzdx0/TMath::Sqrt(1.+dydx0*dydx0),
              dtgl = (dzdl - dzdl0)/(1.- dzdl*dzdl0);
    ((TH2I*)arr->At(6))->Fill(dzdl0, 
    TMath::ATan(dtgl));
    ((TH2I*)arr->At(7))->Fill(dzdl0, (dzdl - dzdl0)/TMath::Sqrt(covR[4]));
    // pt resolution  \\ 1/pt pulls \\ p resolution for PID
    for(Int_t is=AliPID::kSPECIES; is--;){
      if(TMath::Abs(pdg)!=AliPID::ParticleCode(is)) continue;
      ((TH3S*)((TObjArray*)arr->At(8))->At(ily))->Fill(pt0, pt/pt0-1., is);
      ((TH3S*)((TObjArray*)arr->At(9))->At(ily))->Fill(1./pt0, (1./pt-1./pt0)/TMath::Sqrt(covR[6]), is);
      Double_t p0 = TMath::Sqrt(1.+ dzdl0*dzdl0)*pt0,
               p  = TMath::Sqrt(1.+ dzdl*dzdl)*pt;
     ((TH3S*)((TObjArray*)arr->At(10))->At(ily))->Fill(p0, p/p0-1., is);
      break;
    }

    // Fill Debug stream for Kalman track
    if(DebugLevel()>=1){
      (*DebugStream()) << "MCtrack"
        << "pt="      << pt
        << "x="       << x
        << "y="       << y
        << "z="       << z
        << "dydx="    << dydx
        << "dzdx="    << dzdx
        << "s2y="     << covR[0]
        << "s2z="     << covR[2]
        << "\n";
    }

    // recalculate tracklet based on the MC info
    AliTRDseedV1 tt(*fTracklet);
    tt.SetZref(0, z0 - (x0-xAnode)*dzdx0);
    tt.SetZref(1, dzdx0); 
    tt.SetReconstructor(&rec);
    tt.Fit(kTRUE, kTRUE);
    x= tt.GetX();y= tt.GetY();z= tt.GetZ();
    dydx = tt.GetYfit(1);
    dx = x0 - x;
    ymc = y0 - dx*dydx0;
    zmc = z0 - dx*dzdx0;
    Bool_t rc = tt.IsRowCross(); 
    
    // add tracklet residuals for y and dydx
    arr = (TObjArray*)fContainer->At(kMCtracklet);
    if(!rc){
      dy    = y-ymc;

      Float_t dphi  = (dydx - dydx0);
      dphi /= (1.- dydx*dydx0);

      ((TH2I*)arr->At(0))->Fill(dydx0, dy);
      if(tt.GetS2Y()>0.) ((TH2I*)arr->At(1))->Fill(dydx0, dy/TMath::Sqrt(tt.GetS2Y()));
      ((TH2I*)arr->At(4))->Fill(dydx0, TMath::ATan(dphi));
    } else {
      // add tracklet residuals for z
      dz = z-zmc;
      ((TH2I*)arr->At(2))->Fill(dzdl0, dz);
      if(tt.GetS2Z()>0.) ((TH2I*)arr->At(3))->Fill(dzdl0, dz/TMath::Sqrt(tt.GetS2Z()));
    }
  
    // Fill Debug stream for tracklet
    if(DebugLevel()>=1){
      Float_t s2y = tt.GetS2Y();
      Float_t s2z = tt.GetS2Z();
      (*DebugStream()) << "MCtracklet"
        << "rc="    << rc
        << "x="     << x
        << "y="     << y
        << "z="     << z
        << "dydx="  << dydx
        << "s2y="   << s2y
        << "s2z="   << s2z
        << "\n";
    }

    Int_t istk = AliTRDgeometry::GetStack(det); 
    AliTRDpadPlane *pp = fGeo->GetPadPlane(ily, istk);
    Float_t zr0 = pp->GetRow0() + pp->GetAnodeWireOffset();
    Float_t tilt = fTracklet->GetTilt();
    //Double_t exb = AliTRDCommonParam::Instance()->GetOmegaTau(1.5);

    arr = (TObjArray*)fContainer->At(kMCcluster);
    AliTRDcluster *c = NULL;
    fTracklet->ResetClusterIter(kFALSE);
    while((c = fTracklet->PrevCluster())){
      Float_t  q = TMath::Abs(c->GetQ());
      x = c->GetX(); y = c->GetY();z = c->GetZ();
      dx = x0 - x; 
      ymc= y0 - dx*dydx0;
      zmc= z0 - dx*dzdx0;
      dy = (y - tilt*(z-zmc)) - ymc;
      // Fill Histograms
      if(q>20. && q<250.){ 
        ((TH2I*)arr->At(0))->Fill(dydx0, dy);
        ((TH2I*)arr->At(1))->Fill(dydx0, dy/TMath::Sqrt(c->GetSigmaY2()));
      }

      // Fill calibration container
      Float_t d = zr0 - zmc;
      d -= ((Int_t)(2 * d)) / 2.0;
      if (d > 0.25) d  = 0.5 - d;
      AliTRDclusterInfo *clInfo = new AliTRDclusterInfo;
      clInfo->SetCluster(c);
      clInfo->SetMC(pdg, label);
      clInfo->SetGlobalPosition(ymc, zmc, dydx0, dzdx0);
      clInfo->SetResolution(dy);
      clInfo->SetAnisochronity(d);
      clInfo->SetDriftLength(dx-.5*AliTRDgeometry::CamHght());
      clInfo->SetTilt(tilt);
      fMCcl->Add(clInfo);
      if(DebugLevel()>=2){ 
        if(!clInfoArr) clInfoArr=new TObjArray(AliTRDseedV1::kNclusters);
        clInfoArr->Add(clInfo);
      }
    }
    // Fill Debug Tree
    if(DebugLevel()>=2 && clInfoArr){
      (*DebugStream()) << "MCcluster"
        <<"clInfo.=" << clInfoArr
        << "\n";
      delete clInfoArr; clInfoArr=NULL;
    }
  }
  return h;
}


//________________________________________________________
Bool_t AliTRDresolution::GetRefFigure(Int_t ifig)
{
  //
  // Get the reference figures
  //

  Float_t xy[4] = {0., 0., 0., 0.};
  if(!gPad){
    AliWarning("Please provide a canvas to draw results.");
    return kFALSE;
  }
  TList *l = NULL; TVirtualPad *pad=NULL;
  switch(ifig){
  case kCharge:
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    ((TVirtualPad*)l->At(0))->cd();
    ((TGraphAsymmErrors*)((TObjArray*)fGraphM->At(kCharge))->At(0))->Draw("apl");
    ((TVirtualPad*)l->At(1))->cd();
    ((TGraphErrors*)((TObjArray*)fGraphS->At(kCharge))->At(0))->Draw("apl");
    break;
  case kCluster:
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = -.3; xy[1] = -200.; xy[2] = .3; xy[3] = 1000.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kCluster, 0)) break;
    xy[0] = -.3; xy[1] = -0.5; xy[2] = .3; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kCluster, 1)) break;
    return kTRUE;
  case kTrackTRD :
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = -.3; xy[1] = -500.; xy[2] = .3; xy[3] = 1500.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kTrackTRD , 0)) break;
    xy[0] = -.3; xy[1] = -0.5; xy[2] = .3; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kTrackTRD , 1)) break;
    return kTRUE;
  case 3: // kTrackTRD  z
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = -1.; xy[1] = -1000.; xy[2] = 1.; xy[3] = 4000.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kTrackTRD , 2)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kTrackTRD , 3)) break;
    return kTRUE;
  case 4: // kTrackTRD  phi
    xy[0] = -.3; xy[1] = -5.; xy[2] = .3; xy[3] = 50.;
    if(GetGraphPlot(&xy[0], kTrackTRD , 4)) return kTRUE;
    break;
  case 5: // kTrackTPC y
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = -.3; xy[1] = -500.; xy[2] = .3; xy[3] = 1500.;
    pad = ((TVirtualPad*)l->At(0)); pad->cd();
    pad->SetMargin(0.1, 0.1, 0.1, 0.01);
    if(!GetGraphPlot(&xy[0], kTrackTPC, 0)) break;
    xy[0] = -.3; xy[1] = -0.5; xy[2] = .3; xy[3] = 2.5;
    pad=((TVirtualPad*)l->At(1)); pad->cd();
    pad->SetMargin(0.1, 0.1, 0.1, 0.01);
    if(!GetGraphPlot(&xy[0], kTrackTPC, 1)) break;
    return kTRUE;
  case 6: // kTrackTPC z
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = -1.; xy[1] = -1000.; xy[2] = 1.; xy[3] = 4000.;
    pad = ((TVirtualPad*)l->At(0)); pad->cd();
    pad->SetMargin(0.1, 0.1, 0.1, 0.01);
    if(!GetGraphPlot(&xy[0], kTrackTPC, 2)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 2.5;
    pad = ((TVirtualPad*)l->At(1)); pad->cd();
    pad->SetMargin(0.1, 0.1, 0.1, 0.01);
    if(!GetGraphPlot(&xy[0], kTrackTPC, 3)) break;
    return kTRUE;
  case 7: // kTrackTPC phi
    xy[0] = -.3; xy[1] = -5.; xy[2] = .3; xy[3] = 50.;
    if(GetGraphPlot(&xy[0], kTrackTPC, 4)) return kTRUE;
    break;
  case 8: // kMCcluster
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.3; xy[1]=-50.; xy[2]=.3; xy[3]=650.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCcluster, 0)) break;
    xy[0] = -.3; xy[1] = -0.5; xy[2] = .3; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCcluster, 1)) break;
    return kTRUE;
  case 9: //kMCtracklet [y]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.2; xy[1]=-50.; xy[2]=.2; xy[3] =250.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtracklet, 0)) break;
    xy[0] = -.2; xy[1] = -0.5; xy[2] = .2; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtracklet, 1)) break;
    return kTRUE;
  case 10: //kMCtracklet [z]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-1.; xy[1]=-100.; xy[2]=1.; xy[3] =2500.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtracklet, 2)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtracklet, 3)) break;
    return kTRUE;
  case 11: //kMCtracklet [phi]
    xy[0]=-.3; xy[1]=-3.; xy[2]=.3; xy[3] =25.;
    if(!GetGraphPlot(&xy[0], kMCtracklet, 4)) break;
    return kTRUE;
  case 12: //kMCtrackTRD [y]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.2; xy[1]=-50.; xy[2]=.2; xy[3] =200.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 0)) break;
    xy[0] = -.2; xy[1] = -0.5; xy[2] = .2; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 1)) break;
    return kTRUE;
  case 13: //kMCtrackTRD [z]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-1.; xy[1]=-700.; xy[2]=1.; xy[3] =1500.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 2)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 3)) break;
    return kTRUE;
  case 14: //kMCtrackTRD [phi/snp]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.2; xy[1]=-0.2; xy[2]=.2; xy[3] =2.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 4)) break;
    xy[0] = -.2; xy[1] = -0.5; xy[2] = .2; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 5)) break;
    return kTRUE;
  case 15: //kMCtrackTRD [theta/tgl]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-1.; xy[1]=-0.5; xy[2]=1.; xy[3] =5.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 6)) break;
    xy[0] = -.2; xy[1] = -0.5; xy[2] = .2; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTRD, 7)) break;
    return kTRUE;
  case 16: //kMCtrackTRD [pt]
    xy[0] = 0.; xy[1] = -5.; xy[2] = 12.; xy[3] = 7.;
    gPad->Divide(2, 3, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
      pad = (TVirtualPad*)l->At(il); pad->cd();
      pad->SetMargin(0.07, 0.07, 0.1, 0.);
      if(!GetGraphTrack(&xy[0], 8, il)) break;
    }
    return kTRUE;
  case 17: //kMCtrackTRD [1/pt] pulls
    xy[0] = 0.; xy[1] = -1.5; xy[2] = 2.; xy[3] = 2.;
    gPad->Divide(2, 3, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
      pad = (TVirtualPad*)l->At(il); pad->cd();
      pad->SetMargin(0.07, 0.07, 0.1, 0.);
      if(!GetGraphTrack(&xy[0], 9, il)) break;
    }
    return kTRUE;
  case 18: //kMCtrackTRD [p]
    xy[0] = 0.; xy[1] = -7.5; xy[2] = 12.; xy[3] = 10.5;
    gPad->Divide(2, 3, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
      pad = (TVirtualPad*)l->At(il); pad->cd();
      pad->SetMargin(0.07, 0.07, 0.1, 0.);
      if(!GetGraphTrack(&xy[0], 10, il)) break;
    }
    return kTRUE;
  case 19: // kMCtrackTPC [y]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.25; xy[1]=-50.; xy[2]=.25; xy[3] =800.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 0)) break;
    xy[0] = -.25; xy[1] = -0.5; xy[2] = .25; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 1)) break;
    return kTRUE;
  case 20: // kMCtrackTPC [z]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-1.; xy[1]=-500.; xy[2]=1.; xy[3] =800.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 2)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 3)) break;
    return kTRUE;
  case 21: // kMCtrackTPC [phi|snp]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-.25; xy[1]=-0.5; xy[2]=.25; xy[3] =2.5;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 4)) break;
    xy[0] = -.25; xy[1] = -0.5; xy[2] = .25; xy[3] = 1.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 5)) break;
    return kTRUE;
  case 22: // kMCtrackTPC [theta|tgl]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0]=-1.; xy[1]=-1.; xy[2]=1.; xy[3] =4.;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 6)) break;
    xy[0] = -1.; xy[1] = -0.5; xy[2] = 1.; xy[3] = 1.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphPlot(&xy[0], kMCtrackTPC, 7)) break;
    return kTRUE;
  case 23: // kMCtrackTPC [pt]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = 0.; xy[1] = -.8; xy[2] = 12.; xy[3] = 2.3;
    ((TVirtualPad*)l->At(0))->cd();
    if(!GetGraphTrackTPC(xy, 8)) break;
    xy[0]=0.; xy[1]=-0.5; xy[2]=2.; xy[3] =2.5;
    ((TVirtualPad*)l->At(1))->cd();
    if(!GetGraphTrackTPC(xy, 9)) break;
    return kTRUE;
  case 24: // kMCtrackTPC [p]
    gPad->Divide(2, 1, 1.e-5, 1.e-5); l=gPad->GetListOfPrimitives(); 
    xy[0] = 0.; xy[1] = -.8; xy[2] = 12.; xy[3] = 2.3;
    pad = ((TVirtualPad*)l->At(0));pad->cd();
    pad->SetMargin(0.12, 0.12, 0.1, 0.04);
    if(!GetGraphTrackTPC(xy, 10)) break;
    xy[0]=0.; xy[1]=-1.5; xy[2]=12.; xy[3] =2.5;
    pad = ((TVirtualPad*)l->At(1)); pad->cd();
    pad->SetMargin(0.12, 0.12, 0.1, 0.04);
    if(!GetGraphTrackTPC(xy, 11)) break;
    return kTRUE;
  case 25:  // kMCtrackTOF [z]
    return kTRUE;
  }
  AliWarning(Form("Reference plot [%d] missing result", ifig));
  return kFALSE;
}


//________________________________________________________
Bool_t AliTRDresolution::PostProcess()
{
  //fContainer = dynamic_cast<TObjArray*>(GetOutputData(0));
  if (!fContainer) {
    AliError("ERROR: list not available");
    return kFALSE;
  }
  TGraph *gm= NULL, *gs= NULL;
  if(!fGraphS && !fGraphM){ 
    TObjArray *aM(NULL), *aS(NULL), *a(NULL);
    Int_t n = fContainer->GetEntriesFast();
    fGraphS = new TObjArray(n); fGraphS->SetOwner();
    fGraphM = new TObjArray(n); fGraphM->SetOwner();
    for(Int_t ig=0; ig<n; ig++){
      fGraphM->AddAt(aM = new TObjArray(fgNElements[ig]), ig);
      fGraphS->AddAt(aS = new TObjArray(fgNElements[ig]), ig);

      for(Int_t ic=0; ic<fgNElements[ig]; ic++){
        if(ig==kMCtrackTPC&&(ic>=8&&ic<=12)){ // TPC momentum plot
          aS->AddAt(a = new TObjArray(AliPID::kSPECIES), ic); 
          for(Int_t is=AliPID::kSPECIES; is--;){
            a->AddAt(gs = new TGraphErrors(), is);
            gs->SetMarkerStyle(23);
            gs->SetMarkerColor(is ? kRed : kMagenta);
            gs->SetLineStyle(is);
            gs->SetLineColor(is ? kRed : kMagenta);
            gs->SetLineWidth(is ? 1 : 3);
            gs->SetNameTitle(Form("s_%d%02d%d", ig, ic, is), "");
          }
          aM->AddAt(a = new TObjArray(AliPID::kSPECIES), ic); 
          for(Int_t is=AliPID::kSPECIES; is--;){
            a->AddAt(gm = new TGraphErrors(), is);
            gm->SetLineColor(is ? kBlack : kBlue);
            gm->SetLineStyle(is);
            gm->SetMarkerStyle(7);
            gm->SetMarkerColor(is ? kBlack : kBlue);
            gm->SetLineWidth(is ? 1 : 3);
            gm->SetNameTitle(Form("m_%d%02d%d", ig, ic, is), "");
          }
          continue;
        } else if(ig==kMCtrackTRD&&(ic==8||ic==9||ic==10)){ // TRD momentum plot
          TObjArray *aaS, *aaM;
          aS->AddAt(aaS = new TObjArray(AliTRDgeometry::kNlayer), ic); 
          aM->AddAt(aaM = new TObjArray(AliTRDgeometry::kNlayer), ic);
          for(Int_t il=AliTRDgeometry::kNlayer; il--;){
            aaS->AddAt(a = new TObjArray(AliPID::kSPECIES), il); 
            for(Int_t is=AliPID::kSPECIES; is--;){
              a->AddAt(gs = new TGraphErrors(), is);
              gs->SetMarkerStyle(23);
              gs->SetMarkerColor(is ? kRed : kMagenta);
              gs->SetLineStyle(is);
              gs->SetLineColor(is ? kRed : kMagenta);
              gs->SetLineWidth(is ? 1 : 3);
              gs->SetNameTitle(Form("s_%d%02d%d%d", ig, ic, is, il), "");
            }
            aaM->AddAt(a = new TObjArray(AliPID::kSPECIES), il);
            for(Int_t is=AliPID::kSPECIES; is--;){
              a->AddAt(gm = new TGraphErrors(), is);
              gm->SetMarkerStyle(7);
              gm->SetMarkerColor(is ? kBlack : kBlue);
              gm->SetLineStyle(is);
              gm->SetLineColor(is ? kBlack : kBlue);
              gm->SetLineWidth(is ? 1 : 3);
              gm->SetNameTitle(Form("m_%d%02d%d%d", ig, ic, is, il), "");
            }
          } 
          continue;
        }

        aS->AddAt(gs = new TGraphErrors(), ic);
        gs->SetMarkerStyle(23);
        gs->SetMarkerColor(kRed);
        gs->SetLineColor(kRed);
        gs->SetNameTitle(Form("s_%d%02d", ig, ic), "");

        aM->AddAt(gm = ig ? (TGraph*)new TGraphErrors() : (TGraph*)new TGraphAsymmErrors(), ic);
        gm->SetLineColor(kBlack);
        gm->SetMarkerStyle(7);
        gm->SetMarkerColor(kBlack);
        gm->SetNameTitle(Form("m_%d%02d", ig, ic), "");
      }
    }
  }

/*  printf("\n\n\n"); fGraphS->ls();
  printf("\n\n\n"); fGraphM->ls();*/
  

  // DEFINE MODELS
  // simple gauss
  TF1 fg("fGauss", "gaus", -.5, .5);  
  // Landau for charge resolution
  TF1 fl("fLandau", "landau", 0., 1000.);  

  //PROCESS EXPERIMENTAL DISTRIBUTIONS
  // Charge resolution
  //Process3DL(kCharge, 0, &fl); 
  // Clusters residuals
  Process2D(kCluster, 0, &fg, 1.e4); 
  Process2D(kCluster, 1, &fg); 
  fNRefFigures = 1;
  // Tracklet residual/pulls
  Process2D(kTrackTRD , 0, &fg, 1.e4); 
  Process2D(kTrackTRD , 1, &fg); 
  Process2D(kTrackTRD , 2, &fg, 1.e4); 
  Process2D(kTrackTRD , 3, &fg); 
  Process2D(kTrackTRD , 4, &fg, 1.e3); 
  fNRefFigures = 4;
  // TPC track residual/pulls
  Process2D(kTrackTPC, 0, &fg, 1.e4); 
  Process2D(kTrackTPC, 1, &fg); 
  Process2D(kTrackTPC, 2, &fg, 1.e4); 
  Process2D(kTrackTPC, 3, &fg); 
  Process2D(kTrackTPC, 4, &fg, 1.e3); 
  fNRefFigures = 7;

  if(!HasMCdata()) return kTRUE;


  //PROCESS MC RESIDUAL DISTRIBUTIONS

  // CLUSTER Y RESOLUTION/PULLS
  Process2D(kMCcluster, 0, &fg, 1.e4);
  Process2D(kMCcluster, 1, &fg);
  fNRefFigures = 8;

  // TRACKLET RESOLUTION/PULLS
  Process2D(kMCtracklet, 0, &fg, 1.e4); // y
  Process2D(kMCtracklet, 1, &fg);       // y pulls
  Process2D(kMCtracklet, 2, &fg, 1.e4); // z
  Process2D(kMCtracklet, 3, &fg);       // z pulls
  Process2D(kMCtracklet, 4, &fg, 1.e3); // phi
  fNRefFigures = 11;

  // TRACK RESOLUTION/PULLS
  Process2D(kMCtrackTRD, 0, &fg, 1.e4);   // y
  Process2D(kMCtrackTRD, 1, &fg);         // y PULL
  Process2D(kMCtrackTRD, 2, &fg, 1.e4);   // z
  Process2D(kMCtrackTRD, 3, &fg);         // z PULL
  Process2D(kMCtrackTRD, 4, &fg, 1.e3);   // phi
  Process2D(kMCtrackTRD, 5, &fg);         // snp PULL
  Process2D(kMCtrackTRD, 6, &fg, 1.e3);   // theta
  Process2D(kMCtrackTRD, 7, &fg);         // tgl PULL
  Process4D(kMCtrackTRD, 8, &fg, 1.e2);   // pt resolution
  Process4D(kMCtrackTRD, 9, &fg);         // 1/pt pulls
  Process4D(kMCtrackTRD, 10, &fg, 1.e2);  // p resolution
  fNRefFigures = 18;

  // TRACK TPC RESOLUTION/PULLS
  Process2D(kMCtrackTPC, 0, &fg, 1.e4);// y resolution
  Process2D(kMCtrackTPC, 1, &fg);      // y pulls
  Process2D(kMCtrackTPC, 2, &fg, 1.e4);// z resolution
  Process2D(kMCtrackTPC, 3, &fg);      // z pulls
  Process2D(kMCtrackTPC, 4, &fg, 1.e3);// phi resolution
  Process2D(kMCtrackTPC, 5, &fg);      // snp pulls
  Process2D(kMCtrackTPC, 6, &fg, 1.e3);// theta resolution
  Process2D(kMCtrackTPC, 7, &fg);      // tgl pulls
  Process3D(kMCtrackTPC, 8, &fg, 1.e2);// pt resolution
  Process3D(kMCtrackTPC, 9, &fg);      // 1/pt pulls
  Process3D(kMCtrackTPC, 10, &fg, 1.e2);// p resolution
  Process3D(kMCtrackTPC, 11, &fg);      // p pulls
  fNRefFigures = 24;

  // TRACK HMPID RESOLUTION/PULLS
  Process2D(kMCtrackTOF, 0, &fg, 1.e4); // z towards TOF
  Process2D(kMCtrackTOF, 1, &fg);       // z towards TOF
  fNRefFigures = 25;

  return kTRUE;
}


//________________________________________________________
void AliTRDresolution::Terminate(Option_t *opt)
{
  AliTRDrecoTask::Terminate(opt);
  if(HasPostProcess()) PostProcess();
}

//________________________________________________________
void AliTRDresolution::AdjustF1(TH1 *h, TF1 *f)
{
// Helper function to avoid duplication of code
// Make first guesses on the fit parameters

  // find the intial parameters of the fit !! (thanks George)
  Int_t nbinsy = Int_t(.5*h->GetNbinsX());
  Double_t sum = 0.;
  for(Int_t jbin=nbinsy-4; jbin<=nbinsy+4; jbin++) sum+=h->GetBinContent(jbin); sum/=9.;
  f->SetParLimits(0, 0., 3.*sum);
  f->SetParameter(0, .9*sum);
  Double_t rms = h->GetRMS();
  f->SetParLimits(1, -rms, rms);
  f->SetParameter(1, h->GetMean());

  f->SetParLimits(2, 0., 2.*rms);
  f->SetParameter(2, rms);
  if(f->GetNpar() <= 4) return;

  f->SetParLimits(3, 0., sum);
  f->SetParameter(3, .1*sum);

  f->SetParLimits(4, -.3, .3);
  f->SetParameter(4, 0.);

  f->SetParLimits(5, 0., 1.e2);
  f->SetParameter(5, 2.e-1);
}

//________________________________________________________
TObjArray* AliTRDresolution::Histos()
{
  //
  // Define histograms
  //

  if(fContainer) return fContainer;

  fContainer  = new TObjArray(kNhistos);
  //fContainer->SetOwner(kTRUE);
  TH1 *h = NULL;
  TObjArray *arr = NULL;

  const Int_t kPbins(12); // binning in momentum range should depend on the statistics analyzed

  // cluster to track residuals/pulls
  fContainer->AddAt(arr = new TObjArray(fgNElements[kCharge]), kCharge);
  arr->SetName("Charge");
  if(!(h = (TH3S*)gROOT->FindObject("hCharge"))){
    h = new TH3S("hCharge", "Charge Resolution", 20, 1., 2., 24, 0., 3.6, 100, 0., 500.);
    h->GetXaxis()->SetTitle("dx/dx_{0}");
    h->GetYaxis()->SetTitle("x_{d} [cm]");
    h->GetZaxis()->SetTitle("dq/dx [ADC/cm]");
  } else h->Reset();
  arr->AddAt(h, 0);

  // cluster to track residuals/pulls
  fContainer->AddAt(arr = new TObjArray(fgNElements[kCluster]), kCluster);
  arr->SetName("Cl");
  if(!(h = (TH2I*)gROOT->FindObject("hCl"))){
    h = new TH2I("hCl", "Cluster Residuals", 21, -.33, .33, 100, -.5, .5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH2I*)gROOT->FindObject("hClpull"))){
    h = new TH2I("hClpull", "Cluster Pulls", 21, -.33, .33, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y/#sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);

  // tracklet to track residuals/pulls in y direction
  fContainer->AddAt(arr = new TObjArray(fgNElements[kTrackTRD ]), kTrackTRD );
  arr->SetName("Trklt");
  if(!(h = (TH2I*)gROOT->FindObject("hTrkltY"))){
    h = new TH2I("hTrkltY", "Tracklet Y Residuals", 21, -.33, .33, 100, -.5, .5);
    h->GetXaxis()->SetTitle("#tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH2I*)gROOT->FindObject("hTrkltYpull"))){
    h = new TH2I("hTrkltYpull", "Tracklet Y Pulls", 21, -.33, .33, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("#tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y/#sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);
  // tracklet to track residuals/pulls in z direction
  if(!(h = (TH2I*)gROOT->FindObject("hTrkltZ"))){
    h = new TH2I("hTrkltZ", "Tracklet Z Residuals", 50, -1., 1., 100, -1.5, 1.5);
    h->GetXaxis()->SetTitle("#tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 2);
  if(!(h = (TH2I*)gROOT->FindObject("hTrkltZpull"))){
    h = new TH2I("hTrkltZpull", "Tracklet Z Pulls", 50, -1., 1., 100, -5.5, 5.5);
    h->GetXaxis()->SetTitle("#tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z/#sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 3);
  // tracklet to track phi residuals
  if(!(h = (TH2I*)gROOT->FindObject("hTrkltPhi"))){
    h = new TH2I("hTrkltPhi", "Tracklet #phi Residuals", 21, -.33, .33, 100, -.5, .5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta phi [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 4);


  // tracklet to TPC track residuals/pulls in y direction
  fContainer->AddAt(arr = new TObjArray(fgNElements[kTrackTPC]), kTrackTPC);
  arr->SetName("TrkTPC");
  if(!(h = (TH2I*)gROOT->FindObject("hTrkTPCY"))){
    h = new TH2I("hTrkTPCY", "Track[TPC] Y Residuals", 21, -.33, .33, 100, -.5, .5);
    h->GetXaxis()->SetTitle("#tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH2I*)gROOT->FindObject("hTrkTPCYpull"))){
    h = new TH2I("hTrkTPCYpull", "Track[TPC] Y Pulls", 21, -.33, .33, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("#tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y/#sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);
  // tracklet to TPC track residuals/pulls in z direction
  if(!(h = (TH2I*)gROOT->FindObject("hTrkTPCZ"))){
    h = new TH2I("hTrkTPCZ", "Track[TPC] Z Residuals", 50, -1., 1., 100, -1.5, 1.5);
    h->GetXaxis()->SetTitle("#tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 2);
  if(!(h = (TH2I*)gROOT->FindObject("hTrkTPCZpull"))){
    h = new TH2I("hTrkTPCZpull", "Track[TPC] Z Pulls", 50, -1., 1., 100, -5.5, 5.5);
    h->GetXaxis()->SetTitle("#tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z/#sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 3);
  // tracklet to TPC track phi residuals
  if(!(h = (TH2I*)gROOT->FindObject("hTrkTPCPhi"))){
    h = new TH2I("hTrkTPCPhi", "Track[TPC] #phi Residuals", 21, -.33, .33, 100, -.5, .5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta phi [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 4);


  // Resolution histos
  if(!HasMCdata()) return fContainer;

  // cluster y resolution [0]
  fContainer->AddAt(arr = new TObjArray(fgNElements[kMCcluster]), kMCcluster);
  arr->SetName("McCl");
  if(!(h = (TH2I*)gROOT->FindObject("hMcCl"))){
    h = new TH2I("hMcCl", "Cluster Resolution", 48, -.48, .48, 100, -.3, .3);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH2I*)gROOT->FindObject("hMcClPull"))){
    h = new TH2I("hMcClPull", "Cluster Pulls", 48, -.48, .48, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Deltay/#sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);


  // TRACKLET RESOLUTION
  fContainer->AddAt(arr = new TObjArray(fgNElements[kMCtracklet]), kMCtracklet);
  arr->SetName("McTrklt");
  // tracklet y resolution
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkltY"))){
    h = new TH2I("hMcTrkltY", "Tracklet Resolution (Y)", 48, -.48, .48, 100, -.2, .2);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  // tracklet y pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkltYPull"))){
    h = new TH2I("hMcTrkltYPull", "Tracklet Pulls (Y)", 48, -.48, .48, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y / #sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);
  // tracklet z resolution
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkltZ"))){
    h = new TH2I("hMcTrkltZ", "Tracklet Resolution (Z)", 100, -1., 1., 100, -1., 1.);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 2);
  // tracklet z pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkltZPull"))){
    h = new TH2I("hMcTrkltZPull", "Tracklet Pulls (Z)", 100, -1., 1., 100, -3.5, 3.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z / #sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 3);
  // tracklet phi resolution
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkltPhi"))){
    h = new TH2I("hMcTrkltPhi", "Tracklet Resolution (#Phi)", 48, -.48, .48, 100, -.15, .15);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta #phi [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 4);


  // KALMAN TRACK RESOLUTION
  fContainer->AddAt(arr = new TObjArray(fgNElements[kMCtrackTRD]), kMCtrackTRD);
  arr->SetName("McTrkTRD");
  // Kalman track y resolution
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkY"))){
    h = new TH2I("hMcTrkY", "Track Y Resolution", 48, -.48, .48, 100, -.2, .2);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  // Kalman track y pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkYPull"))){
    h = new TH2I("hMcTrkYPull", "Track Y Pulls", 48, -.48, .48, 100, -4., 4.);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y / #sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);
  // Kalman track Z
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkZ"))){
    h = new TH2I("hMcTrkZ", "Track Z Resolution", 100, -1., 1., 100, -1., 1.);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 2);
  // Kalman track Z pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkZPull"))){
    h = new TH2I("hMcTrkZPull", "Track Z Pulls", 100, -1., 1., 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z / #sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 3);
  // Kalman track SNP
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkSNP"))){
    h = new TH2I("hMcTrkSNP", "Track Phi Resolution", 60, -.3, .3, 100, -5e-3, 5e-3);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta #phi [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 4);
  // Kalman track SNP pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkSNPPull"))){
    h = new TH2I("hMcTrkSNPPull", "Track SNP Pulls", 60, -.3, .3, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta(sin(#phi)) / #sigma_{sin(#phi)}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 5);
  // Kalman track TGL
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTGL"))){
    h = new TH2I("hMcTrkTGL", "Track Theta Resolution", 100, -1., 1., 100, -5e-3, 5e-3);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta#theta [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 6);
  // Kalman track TGL pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTGLPull"))){
    h = new TH2I("hMcTrkTGLPull", "Track TGL  Pulls", 100, -1., 1., 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta(tg(#theta)) / #sigma_{tg(#theta)}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 7);
  // Kalman track Pt resolution
  const Int_t n = AliPID::kSPECIES;
  TObjArray *arr2 = NULL; TH3S* h3=NULL;
  arr->AddAt(arr2 = new TObjArray(AliTRDgeometry::kNlayer), 8);
  arr2->SetName("Track Pt Resolution");
  for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
    if(!(h3 = (TH3S*)gROOT->FindObject(Form("hMcTrkPt%d", il)))){
      h3 = new TH3S(Form("hMcTrkPt%d", il), "Track Pt Resolution", kPbins, 0., 12., 150, -.1, .2, n, -.5, n-.5);
      h3->GetXaxis()->SetTitle("p_{t} [GeV/c]");
      h3->GetYaxis()->SetTitle("#Delta p_{t}/p_{t}^{MC}");
      h3->GetZaxis()->SetTitle("SPECIES");
    } else h3->Reset();
    arr2->AddAt(h3, il);
  }
  // Kalman track Pt pulls
  arr->AddAt(arr2 = new TObjArray(AliTRDgeometry::kNlayer), 9);
  arr2->SetName("Track 1/Pt Pulls");
  for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
    if(!(h3 = (TH3S*)gROOT->FindObject(Form("hMcTrkPtPulls%d", il)))){
      h3 = new TH3S(Form("hMcTrkPtPulls%d", il), "Track 1/Pt Pulls", kPbins, 0., 2., 100, -4., 4., n, -.5, n-.5);
      h3->GetXaxis()->SetTitle("1/p_{t}^{MC} [c/GeV]");
      h3->GetYaxis()->SetTitle("#Delta(1/p_{t})/#sigma(1/p_{t}) ");
      h3->GetZaxis()->SetTitle("SPECIES");
    } else h3->Reset();
    arr2->AddAt(h3, il);
  }
  // Kalman track P resolution
  arr->AddAt(arr2 = new TObjArray(AliTRDgeometry::kNlayer), 10);
  arr2->SetName("Track P Resolution [PID]");
  for(Int_t il=0; il<AliTRDgeometry::kNlayer; il++){
    if(!(h3 = (TH3S*)gROOT->FindObject(Form("hMcTrkP%d", il)))){
      h3 = new TH3S(Form("hMcTrkP%d", il), "Track P Resolution", kPbins, 0., 12., 150, -.15, .35, n, -.5, n-.5);
      h3->GetXaxis()->SetTitle("p [GeV/c]");
      h3->GetYaxis()->SetTitle("#Delta p/p^{MC}");
      h3->GetZaxis()->SetTitle("SPECIES");
    } else h3->Reset();
    arr2->AddAt(h3, il);
  }

  // TPC TRACK RESOLUTION
  fContainer->AddAt(arr = new TObjArray(fgNElements[kMCtrackTPC]), kMCtrackTPC);
  arr->SetName("McTrkTPC");
  // Kalman track Y
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCY"))){
    h = new TH2I("hMcTrkTPCY", "Track[TPC] Y Resolution", 60, -.3, .3, 100, -.5, .5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  // Kalman track Y pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCYPull"))){
    h = new TH2I("hMcTrkTPCYPull", "Track[TPC] Y Pulls", 60, -.3, .3, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta y / #sigma_{y}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);
  // Kalman track Z
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCZ"))){
    h = new TH2I("hMcTrkTPCZ", "Track[TPC] Z Resolution", 100, -1., 1., 100, -1., 1.);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 2);
  // Kalman track Z pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCZPull"))){
    h = new TH2I("hMcTrkTPCZPull", "Track[TPC] Z Pulls", 100, -1., 1., 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z / #sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 3);
  // Kalman track SNP
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCSNP"))){
    h = new TH2I("hMcTrkTPCSNP", "Track[TPC] Phi Resolution", 60, -.3, .3, 100, -5e-3, 5e-3);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta #phi [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 4);
  // Kalman track SNP pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCSNPPull"))){
    h = new TH2I("hMcTrkTPCSNPPull", "Track[TPC] SNP Pulls", 60, -.3, .3, 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#phi)");
    h->GetYaxis()->SetTitle("#Delta(sin(#phi)) / #sigma_{sin(#phi)}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 5);
  // Kalman track TGL
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCTGL"))){
    h = new TH2I("hMcTrkTPCTGL", "Track[TPC] Theta Resolution", 100, -1., 1., 100, -5e-3, 5e-3);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta#theta [rad]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 6);
  // Kalman track TGL pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTPCTGLPull"))){
    h = new TH2I("hMcTrkTPCTGLPull", "Track[TPC] TGL  Pulls", 100, -1., 1., 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta(tg(#theta)) / #sigma_{tg(#theta)}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 7);
  // Kalman track Pt resolution
  if(!(h3 = (TH3S*)gROOT->FindObject("hMcTrkTPCPt"))){
    h3 = new TH3S("hMcTrkTPCPt", "Track[TPC] Pt Resolution", kPbins, 0., 12., 100, -.1, .2, n, -.5, n-.5);
    h3->GetXaxis()->SetTitle("p_{t} [GeV/c]");
    h3->GetYaxis()->SetTitle("#Delta p_{t}/p_{t}^{MC}");
    h3->GetZaxis()->SetTitle("SPECIES");
  } else h3->Reset();
  arr->AddAt(h3, 8);
  // Kalman track Pt pulls
  if(!(h3 = (TH3S*)gROOT->FindObject("hMcTrkTPCPtPulls"))){
    h3 = new TH3S("hMcTrkTPCPtPulls", "Track[TPC] 1/Pt Pulls", kPbins, 0., 2., 100, -4., 4., n, -.5, n-.5);
    h3->GetXaxis()->SetTitle("1/p_{t}^{MC} [c/GeV]");
    h3->GetYaxis()->SetTitle("#Delta(1/p_{t})/#sigma(1/p_{t}) ");
    h3->GetZaxis()->SetTitle("SPECIES");
  } else h3->Reset();
  arr->AddAt(h3, 9);
  // Kalman track P resolution
  if(!(h3 = (TH3S*)gROOT->FindObject("hMcTrkTPCP"))){
    h3 = new TH3S("hMcTrkTPCP", "Track[TPC] P Resolution", kPbins, 0., 12., 100, -.15, .35, n, -.5, n-.5);
    h3->GetXaxis()->SetTitle("p [GeV/c]");
    h3->GetYaxis()->SetTitle("#Delta p/p^{MC}");
    h3->GetZaxis()->SetTitle("SPECIES");
  } else h3->Reset();
  arr->AddAt(h3, 10);
  // Kalman track Pt pulls
  if(!(h3 = (TH3S*)gROOT->FindObject("hMcTrkTPCPPulls"))){
    h3 = new TH3S("hMcTrkTPCPPulls", "Track[TPC] P Pulls", kPbins, 0., 12., 100, -5., 5., n, -.5, n-.5);
    h3->GetXaxis()->SetTitle("p^{MC} [GeV/c]");
    h3->GetYaxis()->SetTitle("#Deltap/#sigma_{p}");
    h3->GetZaxis()->SetTitle("SPECIES");
  } else h3->Reset();
  arr->AddAt(h3, 11);



  // Kalman track Z resolution [TOF]
  fContainer->AddAt(arr = new TObjArray(fgNElements[kMCtrackTOF]), kMCtrackTOF);
  arr->SetName("McTrkTOF");
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTOFZ"))){
    h = new TH2I("hMcTrkTOFZ", "Track[TOF] Z Resolution", 100, -1., 1., 100, -1., 1.);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z [cm]");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 0);
  // Kalman track Z pulls
  if(!(h = (TH2I*)gROOT->FindObject("hMcTrkTOFZPull"))){
    h = new TH2I("hMcTrkTOFZPull", "Track[TOF] Z Pulls", 100, -1., 1., 100, -4.5, 4.5);
    h->GetXaxis()->SetTitle("tg(#theta)");
    h->GetYaxis()->SetTitle("#Delta z / #sigma_{z}");
    h->GetZaxis()->SetTitle("entries");
  } else h->Reset();
  arr->AddAt(h, 1);

  return fContainer;
}

//________________________________________________________
Bool_t AliTRDresolution::Process(TH2 * const h2, TF1 *f, Float_t k, TGraphErrors **g)
{
  //
  // Do the processing
  //

  Char_t pn[10]; sprintf(pn, "p%02d", fIdxPlot);
  Int_t n = 0;
  if((n=g[0]->GetN())) for(;n--;) g[0]->RemovePoint(n);
  if((n=g[1]->GetN())) for(;n--;) g[1]->RemovePoint(n);

  for(Int_t ibin = 1; ibin <= h2->GetNbinsX(); ibin++){
    Double_t x = h2->GetXaxis()->GetBinCenter(ibin);
    TH1D *h = h2->ProjectionY(pn, ibin, ibin);
    if(h->GetEntries()<100) continue;
    //AdjustF1(h, f);

    h->Fit(f, "QN");
    
    Int_t ip = g[0]->GetN();
    g[0]->SetPoint(ip, x, k*f->GetParameter(1));
    g[0]->SetPointError(ip, 0., k*f->GetParError(1));
    g[1]->SetPoint(ip, x, k*f->GetParameter(2));
    g[1]->SetPointError(ip, 0., k*f->GetParError(2));
    
/*  
    g[0]->SetPoint(ip, x, k*h->GetMean());
    g[0]->SetPointError(ip, 0., k*h->GetMeanError());
    g[1]->SetPoint(ip, x, k*h->GetRMS());
    g[1]->SetPointError(ip, 0., k*h->GetRMSError());*/
  }
  fIdxPlot++;
  return kTRUE;
}

//________________________________________________________
Bool_t AliTRDresolution::Process2D(ETRDresolutionPlot plot, Int_t idx, TF1 *f, Float_t k)
{
  //
  // Do the processing
  //

  if(!fContainer || !fGraphS || !fGraphM) return kFALSE;

  // retrive containers
  TH2I *h2 = idx<0 ? (TH2I*)(fContainer->At(plot)) : (TH2I*)((TObjArray*)(fContainer->At(plot)))->At(idx);
  if(!h2) return kFALSE;
  TGraphErrors *g[2];
  if(!(g[0] = idx<0 ? (TGraphErrors*)fGraphM->At(plot) : (TGraphErrors*)((TObjArray*)(fGraphM->At(plot)))->At(idx))) return kFALSE;

  if(!(g[1] = idx<0 ? (TGraphErrors*)fGraphS->At(plot) : (TGraphErrors*)((TObjArray*)(fGraphS->At(plot)))->At(idx))) return kFALSE;

  return Process(h2, f, k, g);
}

//________________________________________________________
Bool_t AliTRDresolution::Process3D(ETRDresolutionPlot plot, Int_t idx, TF1 *f, Float_t k)
{
  //
  // Do the processing
  //

  if(!fContainer || !fGraphS || !fGraphM) return kFALSE;

  // retrive containers
  TH3S *h3 = idx<0 ? (TH3S*)(fContainer->At(plot)) : (TH3S*)((TObjArray*)(fContainer->At(plot)))->At(idx);
  if(!h3) return kFALSE;

  TObjArray *gm, *gs;
  if(!(gm = (TObjArray*)((TObjArray*)(fGraphM->At(plot)))->At(idx))) return kFALSE;
  if(!(gs = (TObjArray*)((TObjArray*)(fGraphS->At(plot)))->At(idx))) return kFALSE;
  TGraphErrors *g[2];

  TAxis *az = h3->GetZaxis();
  for(Int_t iz=1; iz<=az->GetNbins(); iz++){
    if(!(g[0] = (TGraphErrors*)gm->At(iz-1))) return kFALSE;
    if(!(g[1] = (TGraphErrors*)gs->At(iz-1))) return kFALSE;
    az->SetRange(iz, iz);
    if(!Process((TH2*)h3->Project3D("yx"), f, k, g)) return kFALSE;
  }

  return kTRUE;
}

//________________________________________________________
Bool_t AliTRDresolution::Process3DL(ETRDresolutionPlot plot, Int_t idx, TF1 *f, Float_t k)
{
  //
  // Do the processing
  //

  if(!fContainer || !fGraphS || !fGraphM) return kFALSE;

  // retrive containers
  TH3S *h3 = (TH3S*)((TObjArray*)fContainer->At(plot))->At(idx);
  if(!h3) return kFALSE;

  TGraphAsymmErrors *gm; 
  TGraphErrors *gs;
  if(!(gm = (TGraphAsymmErrors*)((TObjArray*)fGraphM->At(plot))->At(0))) return kFALSE;
  if(!(gs = (TGraphErrors*)((TObjArray*)fGraphS->At(plot)))) return kFALSE;

  Float_t x, r, mpv, xM, xm;
  TAxis *ay = h3->GetYaxis();
  for(Int_t iy=1; iy<=h3->GetNbinsY(); iy++){
    ay->SetRange(iy, iy);
    x = ay->GetBinCenter(iy);
    TH2F *h2=(TH2F*)h3->Project3D("zx");
    TAxis *ax = h2->GetXaxis();
    for(Int_t ix=1; ix<=h2->GetNbinsX(); ix++){
      r = ax->GetBinCenter(ix);
      TH1D *h1 = h2->ProjectionY("py", ix, ix);
      if(h1->Integral()<50) continue;
      h1->Fit(f, "QN");

      GetLandauMpvFwhm(f, mpv, xm, xM);
      Int_t ip = gm->GetN();
      gm->SetPoint(ip, x, k*mpv);
      gm->SetPointError(ip, 0., 0., k*xm, k*xM);
      gs->SetPoint(ip, r, k*(xM-xm)/mpv);
      gs->SetPointError(ip, 0., 0.);
    }
  }

  return kTRUE;
}

//________________________________________________________
Bool_t AliTRDresolution::Process4D(ETRDresolutionPlot plot, Int_t idx, TF1 *f, Float_t k)
{
  //
  // Do the processing
  //

  if(!fContainer || !fGraphS || !fGraphM) return kFALSE;

  // retrive containers
  TObjArray *arr = (TObjArray*)((TObjArray*)(fContainer->At(plot)))->At(idx);
  if(!arr) return kFALSE;


  TObjArray *gm[2], *gs[2];
  if(!(gm[0] = (TObjArray*)((TObjArray*)(fGraphM->At(plot)))->At(idx))) return kFALSE;
  if(!(gs[0] = (TObjArray*)((TObjArray*)(fGraphS->At(plot)))->At(idx))) return kFALSE;

  TGraphErrors *g[2];

  TH3S *h3 = NULL;
  for(Int_t ix=0; ix<arr->GetEntriesFast(); ix++){
    if(!(h3 = (TH3S*)arr->At(ix))) return kFALSE;
    if(!(gm[1] = (TObjArray*)gm[0]->At(ix))) return kFALSE;
    if(!(gs[1] = (TObjArray*)gs[0]->At(ix))) return kFALSE;
    TAxis *az = h3->GetZaxis();
    for(Int_t iz=1; iz<=az->GetNbins(); iz++){
      if(!(g[0] = (TGraphErrors*)gm[1]->At(iz-1))) return kFALSE;
      if(!(g[1] = (TGraphErrors*)gs[1]->At(iz-1))) return kFALSE;
      az->SetRange(iz, iz);
      if(!Process((TH2*)h3->Project3D("yx"), f, k, g)) return kFALSE;
    }
  }

  return kTRUE;
}

//________________________________________________________
Bool_t AliTRDresolution::GetGraphPlot(Float_t *bb, ETRDresolutionPlot ip, Int_t idx)
{
  //
  // Get the graphs
  //

  if(!fGraphS || !fGraphM) return kFALSE;
  TGraphErrors *gm = idx<0 ? (TGraphErrors*)fGraphM->At(ip) : (TGraphErrors*)((TObjArray*)(fGraphM->At(ip)))->At(idx);
  if(!gm) return kFALSE;
  TGraphErrors *gs = idx<0 ? (TGraphErrors*)fGraphS->At(ip) : (TGraphErrors*)((TObjArray*)(fGraphS->At(ip)))->At(idx);
  if(!gs) return kFALSE;
  gs->Draw("apl"); gm->Draw("pl");

  // titles look up
  Int_t nref = 0;
  for(Int_t jp=0; jp<(Int_t)ip; jp++) nref+=fgNElements[jp];
  UChar_t jdx = idx<0?0:idx;
  for(Int_t jc=0; jc<TMath::Min(jdx,fgNElements[ip]-1); jc++) nref++;
  const Char_t **at = fgAxTitle[nref];

  Int_t n(0);
  if((n=gm->GetN())) {
    PutTrendValue(Form("%s_%s", fgPerformanceName[ip], at[0]), gm->GetMean(2));
    PutTrendValue(Form("%s_%sRMS", fgPerformanceName[ip], at[0]), gm->GetRMS(2));
  }

  if((n=gs->GetN())){
    gs->Sort(&TGraph::CompareY);
    PutTrendValue(Form("%s_%sSigMin", fgPerformanceName[ip], at[0]), gs->GetY()[0]);
    PutTrendValue(Form("%s_%sSigMax", fgPerformanceName[ip], at[0]), gs->GetY()[n-1]);
    gs->Sort(&TGraph::CompareX); 
  }

  // axis range
  TAxis *ax = NULL;
  ax = gs->GetHistogram()->GetXaxis();
  ax->SetRangeUser(bb[0], bb[2]);
  ax->SetTitle(at[1]);ax->CenterTitle();

  ax = gs->GetHistogram()->GetYaxis();
  ax->SetRangeUser(bb[1], bb[3]);
  ax->SetTitleOffset(1.1);
  ax->SetTitle(at[2]);ax->CenterTitle();

  TGaxis *gax = NULL;
  gax = new TGaxis(bb[2], bb[1], bb[2], bb[3], bb[1], bb[3], 510, "+U");
  gax->SetLineColor(kRed);gax->SetLineWidth(2);gax->SetTextColor(kRed);
  //gax->SetVertical();
  gax->CenterTitle(); gax->SetTitleOffset(.7);
  gax->SetTitle(at[3]); gax->Draw();

  // bounding box
  TBox *b = new TBox(-.15, bb[1], .15, bb[3]);
  b->SetFillStyle(3002);b->SetLineColor(0);
  b->SetFillColor(ip<=Int_t(kMCcluster)?kGreen:kBlue);
  b->Draw();
  return kTRUE;
}


//________________________________________________________
Bool_t AliTRDresolution::GetGraphTrack(Float_t *bb, Int_t idx, Int_t il)
{
  //
  // Get the graphs
  //

  if(!fGraphS || !fGraphM) return kFALSE;

  // axis titles look up
  Int_t nref = 0;
  for(Int_t jp=0; jp<Int_t(kMCtrackTRD); jp++) nref+=fgNElements[jp];
  for(Int_t jc=0; jc<idx; jc++) nref++;
  const Char_t **at = fgAxTitle[nref];

  TGraphErrors *gm = NULL, *gs = NULL;
  TObjArray *a0 = fGraphS, *a1 = NULL;
  a1 = (TObjArray*)a0->At(kMCtrackTRD); a0 = a1;
  a1 = (TObjArray*)a0->At(idx); a0 = a1;
  a1 = (TObjArray*)a0->At(il); a0 = a1;
  for(Int_t is=0; is<AliPID::kSPECIES; is++){
    if(!(gs =  (TGraphErrors*)a0->At(is))) return kFALSE;
    if(!gs->GetN()) continue;
    gs->Draw(is ? "pl" : "apl");
    gs->Sort(&TGraph::CompareY); Int_t n = gs->GetN();
    PutTrendValue(Form("%s_%sSigMin%s", fgPerformanceName[kMCtrackTRD], at[0], AliPID::ParticleShortName(is)), gs->GetY()[0]);
    PutTrendValue(Form("%s_%sSigMax%s", fgPerformanceName[kMCtrackTRD], at[0], AliPID::ParticleShortName(is)), gs->GetY()[n-1]);
    gs->Sort(&TGraph::CompareX); 
  }
  gs =  (TGraphErrors*)a0->At(0);

  // axis range
  TAxis *ax = gs->GetHistogram()->GetXaxis();
  ax->SetRangeUser(bb[0], bb[2]);
  ax->SetTitle(at[1]);ax->CenterTitle();

  ax = gs->GetHistogram()->GetYaxis();
  ax->SetRangeUser(bb[1], bb[3]);
  ax->SetTitleOffset(.5);ax->SetTitleSize(.06);
  ax->SetTitle(at[2]);ax->CenterTitle();

  TGaxis *gax = NULL;
  gax = new TGaxis(bb[2], bb[1], bb[2], bb[3], bb[1], bb[3], 510, "+U");
  gax->SetLineColor(kRed);gax->SetLineWidth(2);gax->SetTextColor(kRed);
  //gax->SetVertical();
  gax->CenterTitle(); gax->SetTitleOffset(.5);gax->SetTitleSize(.06);
  gax->SetTitle(at[3]); gax->Draw();


  a0 = fGraphM;
  a1 = (TObjArray*)a0->At(kMCtrackTRD); a0 = a1;
  a1 = (TObjArray*)a0->At(idx); a0 = a1;
  a1 = (TObjArray*)a0->At(il); a0 = a1;
  for(Int_t is=0; is<AliPID::kSPECIES; is++){
    if(!(gm =  (TGraphErrors*)a0->At(is))) return kFALSE;
    if(!gm->GetN()) continue;
    gm->Draw("pl");
    PutTrendValue(Form("%s_%s_%s", fgPerformanceName[kMCtrackTRD], at[0], AliPID::ParticleShortName(is)), gm->GetMean(2));
    PutTrendValue(Form("%s_%s_%sRMS", fgPerformanceName[kMCtrackTRD], at[0], AliPID::ParticleShortName(is)), gm->GetRMS(2));
  }

  return kTRUE;
}


//________________________________________________________
Bool_t AliTRDresolution::GetGraphTrackTPC(Float_t *bb, Int_t sel)
{
  //
  // Get the graphs
  //

  if(!fGraphS || !fGraphM) return kFALSE;

  // axis titles look up
  Int_t nref = 0;
  for(Int_t jp=0; jp<Int_t(kMCtrackTPC); jp++) nref+=fgNElements[jp];
  for(Int_t jc=0; jc<sel; jc++) nref++;
  const Char_t **at = fgAxTitle[nref];

  TGraphErrors *gm = NULL, *gs = NULL;
  TObjArray *a0 = fGraphS, *a1 = NULL;
  a1 = (TObjArray*)a0->At(kMCtrackTPC); a0 = a1;
  a1 = (TObjArray*)a0->At(sel); a0 = a1;
  for(Int_t is=AliPID::kSPECIES; is--;){
    if(!(gs =  (TGraphErrors*)a0->At(is))) return kFALSE;
    if(!gs->GetN()) continue;
    gs->Draw(is ? "pl" : "apl");
    gs->Sort(&TGraph::CompareY); Int_t n = gs->GetN();
    PutTrendValue(Form("%s_%sSigMin%s", fgPerformanceName[kMCtrackTPC], at[0], AliPID::ParticleShortName(is)), gs->GetY()[0]);
    PutTrendValue(Form("%s_%sSigMax%s", fgPerformanceName[kMCtrackTPC], at[0], AliPID::ParticleShortName(is)), gs->GetY()[n-1]);
    gs->Sort(&TGraph::CompareX); 
  }
  gs =  (TGraphErrors*)a0->At(0);
  // axis range
  TAxis *ax = gs->GetHistogram()->GetXaxis();
  ax->SetRangeUser(bb[0], bb[2]);
  ax->SetTitle(at[1]);ax->CenterTitle();

  ax = gs->GetHistogram()->GetYaxis();
  ax->SetRangeUser(bb[1], bb[3]);
  ax->SetTitleOffset(1.);ax->SetTitleSize(0.05);
  ax->SetTitle(at[2]);ax->CenterTitle();

  TGaxis *gax = NULL;
  gax = new TGaxis(bb[2], bb[1], bb[2], bb[3], bb[1], bb[3], 510, "+U");
  gax->SetLineColor(kRed);gax->SetLineWidth(2);gax->SetTextColor(kRed);
  //gax->SetVertical();
  gax->CenterTitle(); gax->SetTitleOffset(.7);gax->SetTitleSize(0.05);
  gax->SetTitle(at[3]); gax->Draw();


  a0 = fGraphM;
  a1 = (TObjArray*)a0->At(kMCtrackTPC); a0 = a1;
  a1 = (TObjArray*)a0->At(sel); a0 = a1;
  for(Int_t is=AliPID::kSPECIES; is--;){
    if(!(gm =  (TGraphErrors*)a0->At(is))) return kFALSE;
    if(!gm->GetN()) continue;
    gm->Draw("pl");
    PutTrendValue(Form("%s_%s_%s", fgPerformanceName[kMCtrackTPC], at[0], AliPID::ParticleShortName(is)), gm->GetMean(2));
    PutTrendValue(Form("%s_%s_%sRMS", fgPerformanceName[kMCtrackTPC], at[0], AliPID::ParticleShortName(is)), gm->GetRMS(2));
  }

  return kTRUE;
}

//________________________________________________________
void AliTRDresolution::GetLandauMpvFwhm(TF1 * const f, Float_t &mpv, Float_t &xm, Float_t &xM)
{
  //
  // Get the most probable value and the full width half mean 
  // of a Landau distribution
  //

  const Float_t dx = 1.;
  mpv = f->GetParameter(1);
  Float_t fx, max = f->Eval(mpv);

  xm = mpv - dx;
  while((fx = f->Eval(xm))>.5*max){
    if(fx>max){ 
      max = fx;
      mpv = xm;
    }
    xm -= dx;
  }

  xM += 2*(mpv - xm);
  while((fx = f->Eval(xM))>.5*max) xM += dx;
}


//________________________________________________________
void AliTRDresolution::SetRecoParam(AliTRDrecoParam *r)
{

  fReconstructor->SetRecoParam(r);
}

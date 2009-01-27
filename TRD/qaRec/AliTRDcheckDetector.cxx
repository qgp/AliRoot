#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TMath.h>
#include <TMap.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TObjString.h>
#include <TPad.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>

#include "AliLog.h"
#include "AliTRDcluster.h"
#include "AliESDHeader.h"
#include "AliESDRun.h"
#include "AliESDtrack.h"
#include "AliTRDgeometry.h"
#include "AliTRDpadPlane.h"
#include "AliTRDSimParam.h"
#include "AliTRDseedV1.h"
#include "AliTRDtrackV1.h"
#include "AliTRDtrackerV1.h"
#include "AliTRDReconstructor.h"
#include "AliTrackReference.h"
#include "AliTrackPointArray.h"
#include "AliTracker.h"
#include "TTreeStream.h"

#include "AliTRDtrackInfo/AliTRDtrackInfo.h"
#include "AliTRDtrackInfo/AliTRDeventInfo.h"
#include "AliTRDcheckDetector.h"

#include <cstdio>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Reconstruction QA                                                     //
//                                                                        //
//  Task doing basic checks for tracking and detector performance         //
//                                                                        //
//  Authors:                                                              //
//    Anton Andronic <A.Andronic@gsi.de>                                  //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

//_______________________________________________________
AliTRDcheckDetector::AliTRDcheckDetector():
  AliTRDrecoTask("DetChecker", "Basic Detector Checker")
  ,fEventInfo(0x0)
  ,fTriggerNames(0x0)
  ,fReconstructor(0x0)
  ,fGeo(0x0)
{
  //
  // Default constructor
  //
  DefineInput(1,AliTRDeventInfo::Class());
  fReconstructor = new AliTRDReconstructor;
  fReconstructor->SetRecoParam(AliTRDrecoParam::GetLowFluxParam());
  fGeo = new AliTRDgeometry;
  InitFunctorList();
}

//_______________________________________________________
AliTRDcheckDetector::~AliTRDcheckDetector(){
  //
  // Destructor
  // 
  if(fTriggerNames) delete fTriggerNames;
  delete fReconstructor;
  delete fGeo;
}

//_______________________________________________________
void AliTRDcheckDetector::ConnectInputData(Option_t *opt){
  //
  // Connect the Input data with the task
  //
  AliTRDrecoTask::ConnectInputData(opt);
  fEventInfo = dynamic_cast<AliTRDeventInfo *>(GetInputData(1));
}

//_______________________________________________________
void AliTRDcheckDetector::CreateOutputObjects(){
  //
  // Create Output Objects
  //
  OpenFile(0,"RECREATE");
  fContainer = Histos();
  if(!fTriggerNames) fTriggerNames = new TMap();
}

//_______________________________________________________
void AliTRDcheckDetector::Exec(Option_t *opt){
  //
  // Execution function
  // Filling TRD quality histos
  //
  if(!HasMCdata() && fEventInfo->GetEventHeader()->GetEventType() != 7) return;	// For real data we select only physical events
  AliTRDrecoTask::Exec(opt);  
  Int_t nTracks = 0;		// Count the number of tracks per event
  Int_t triggermask = fEventInfo->GetEventHeader()->GetTriggerMask();
  TString triggername =  fEventInfo->GetRunInfo()->GetFiredTriggerClasses(triggermask);
  if(fDebugLevel > 6)printf("Trigger cluster: %d, Trigger class: %s\n", triggermask, triggername.Data());
  dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTrigger))->Fill(triggermask);
  for(Int_t iti = 0; iti < fTracks->GetEntriesFast(); iti++){
    if(!fTracks->UncheckedAt(iti)) continue;
    AliTRDtrackInfo *fTrackInfo = dynamic_cast<AliTRDtrackInfo *>(fTracks->UncheckedAt(iti));
    if(!fTrackInfo->GetTrack()) continue;
    nTracks++;
  }
  if(nTracks){
    dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTriggerTracks))->Fill(triggermask);
    dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNtracksEvent))->Fill(nTracks);
  }
  if(triggermask <= 20 && !fTriggerNames->FindObject(Form("%d", triggermask))){
    fTriggerNames->Add(new TObjString(Form("%d", triggermask)), new TObjString(triggername));
    // also set the label for both histograms
    TH1 *histo = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTriggerTracks));
    histo->GetXaxis()->SetBinLabel(histo->FindBin(triggermask), triggername);
    histo = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTrigger));
    histo->GetXaxis()->SetBinLabel(histo->FindBin(triggermask), triggername);
  }
  PostData(0, fContainer);
}

//_______________________________________________________
void AliTRDcheckDetector::Terminate(Option_t *){
  //
  // Terminate function
  //
}

//_______________________________________________________
Bool_t AliTRDcheckDetector::PostProcess(){
  //
  // Do Postprocessing (for the moment set the number of Reference histograms)
  //
  
  TH1 * h = 0x0;
  h = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNtrackletsTrack));
  if(h->GetEntries()) h->Scale(100./h->Integral());

  h = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNtrackletsCross));
  if(h->GetEntries()) h->Scale(100./h->Integral());

  h = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNtracksSector));
  if(h->GetEntries()) h->Scale(100./h->Integral());
  
  // Calculate of the trigger clusters purity
  h = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTrigger));
  TH1F *h1 = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kNeventsTriggerTracks));
  h1->Divide(h);
  Float_t purities[20], val = 0;
  TString triggernames[20];
  Int_t nTriggerClasses = 0;
  for(Int_t ibin = 1; ibin <= h->GetNbinsX(); ibin++){
    if((val = h1->GetBinContent(ibin))){
      purities[nTriggerClasses] = val;
      triggernames[nTriggerClasses] = h1->GetXaxis()->GetBinLabel(ibin);
      nTriggerClasses++;
    }
  }
  h = dynamic_cast<TH1F *>(fContainer->UncheckedAt(kTriggerPurity));
  TAxis *ax = h->GetXaxis();
  for(Int_t itrg = 0; itrg < nTriggerClasses; itrg++){
    h->Fill(itrg, purities[itrg]);
    ax->SetBinLabel(itrg+1, triggernames[itrg].Data());
  }
  ax->SetRangeUser(-0.5, nTriggerClasses+.5);
  h->GetYaxis()->SetRangeUser(0,1);

  fNRefFigures = 11;

  return kTRUE;
}

//_______________________________________________________
Bool_t AliTRDcheckDetector::GetRefFigure(Int_t ifig){
  //
  // Setting Reference Figures
  //
  TObjArray *arr = 0x0;
  TH1 *h = 0x0, *h1 = 0x0, *h2 = 0x0;
  TGaxis *axis = 0x0;
  switch(ifig){
  case kNclustersTrack:
    ((TH1F*)fContainer->At(kNclustersTrack))->Draw("pl");
    return kTRUE;
  case kNclustersTracklet:
    ((TH1F*)fContainer->At(kNclustersTracklet))->Draw("pc");
    return kTRUE;
  case kNtrackletsTrack:
    h = (TH1F*)fContainer->At(kNtrackletsTrack);
    if(!h->GetEntries()) break;
    h->SetFillColor(kGreen);
    h->SetBarOffset(.2);
    h->SetBarWidth(.6);
    h->Draw("bar1");
    return kTRUE;
  case kNtrackletsCross:
    h = (TH1F*)fContainer->At(kNtrackletsCross);
    if(!h->GetEntries()) break;
    h->SetFillColor(kRed);
    h->SetBarOffset(.2);
    h->SetBarWidth(.6);
    h->Draw("bar1");
    return kTRUE;
  case kNtrackletsFindable:
    h = (TH1F*)fContainer->At(kNtrackletsFindable);
    if(!h->GetEntries()) break;
    h->Scale(100./h->Integral());
    h->SetFillColor(kGreen);
    h->SetBarOffset(.2);
    h->SetBarWidth(.6);
    h->Draw("bar1");
    return kTRUE;
  case kNtracksEvent:
    ((TH1F*)fContainer->At(kNtracksEvent))->Draw("pl");
    return kTRUE;
  case kNtracksSector:
    h = (TH1F*)fContainer->At(kNtracksSector);
    if(!h->GetEntries()) break;
    h->SetFillColor(kGreen);
    h->SetBarOffset(.2);
    h->SetBarWidth(.6);
    h->Draw("bar1");
    return kTRUE;
  case kChi2:
    ((TH1F*)((TObjArray*)fContainer->At(kChi2))->At(0))->Draw("");
    return kTRUE;
  case kPH:
    arr = (TObjArray*)fContainer->At(kPH);
    h = (TH1F*)arr->At(0);
    h->SetMarkerStyle(24);
    h->SetMarkerColor(kBlack);
    h->SetLineColor(kBlack);
    h->Draw("e1");
    // copy the second histogram in a new one with the same x-dimension as the phs with respect to time
    h1 = (TH1F *)arr->At(1);
    h2 = new TH1F("hphs1","Average PH", 31, -0.5, 30.5);
    for(Int_t ibin = h1->GetXaxis()->GetFirst(); ibin < h1->GetNbinsX(); ibin++) 
      h2->SetBinContent(ibin, h1->GetBinContent(ibin));
    h2->SetMarkerStyle(22);
    h2->SetMarkerColor(kBlue);
    h2->SetLineColor(kBlue);
    h2->Draw("e1same");
    gPad->Update();
    // create axis according to the histogram dimensions of the original second histogram
    axis = new TGaxis(gPad->GetUxmin(),
                      gPad->GetUymax(),
                      gPad->GetUxmax(),
                      gPad->GetUymax(),
                      -0.08, 4.88, 510,"-L");
    axis->SetLineColor(kBlue);
    axis->SetLabelColor(kBlue);
    axis->SetTextColor(kBlue);
    axis->SetTitle("x_{0}-x_{c} [cm]");
    axis->Draw();
    return kTRUE;
  case kChargeCluster:
    ((TH1F*)fContainer->At(kChargeCluster))->Draw("c");
    return kTRUE;
  case kChargeTracklet:
    ((TH1F*)fContainer->At(kChargeTracklet))->Draw("c");
    return kTRUE;
  case kNeventsTrigger:
    ((TH1F*)fContainer->At(kNeventsTrigger))->Draw("");
    return kTRUE;
  case kNeventsTriggerTracks:
    ((TH1F*)fContainer->At(kNeventsTriggerTracks))->Draw("");
    return kTRUE;
  case kTriggerPurity: 
    h=(TH1F*)fContainer->At(kTriggerPurity);
    h->SetBarOffset(.2);
    h->SetBarWidth(.6);
    h->SetFillColor(kGreen);
    h->Draw("bar1");
    break;
  default:
    break;
  }
  AliInfo(Form("Reference plot [%d] missing result", ifig));
  return kFALSE;
}

//_______________________________________________________
TObjArray *AliTRDcheckDetector::Histos(){
  //
  // Create QA histograms
  //
  if(fContainer) return fContainer;
  
  fContainer = new TObjArray(14);
  //fContainer->SetOwner(kTRUE);

  // Register Histograms
  TH1 * h = 0x0;
  if(!(h = (TH1F *)gROOT->FindObject("hNcls"))){
    h = new TH1F("hNcls", "N_{clusters} / track", 181, -0.5, 180.5);
    h->GetXaxis()->SetTitle("N_{clusters}");
    h->GetYaxis()->SetTitle("Entries");
  } else h->Reset();
  fContainer->AddAt(h, kNclustersTrack);

  if(!(h = (TH1F *)gROOT->FindObject("hNclTls"))){
    h = new TH1F("hNclTls","N_{clusters} / tracklet", 51, -0.5, 50.5);
    h->GetXaxis()->SetTitle("N_{clusters}");
    h->GetYaxis()->SetTitle("Entries");
  } else h->Reset();
  fContainer->AddAt(h, kNclustersTracklet);

  if(!(h = (TH1F *)gROOT->FindObject("hNtls"))){
    h = new TH1F("hNtls", "N_{tracklets} / track", AliTRDgeometry::kNlayer, 0.5, 6.5);
    h->GetXaxis()->SetTitle("N^{tracklet}");
    h->GetYaxis()->SetTitle("freq. [%]");
  } else h->Reset();
  fContainer->AddAt(h, kNtrackletsTrack);

  // 
  if(!(h = (TH1F *)gROOT->FindObject("hNtlsCross"))){
    h = new TH1F("hNtlsCross", "N_{tracklets}^{cross} / track", 7, -0.5, 6.5);
    h->GetXaxis()->SetTitle("n_{row cross}");
    h->GetYaxis()->SetTitle("freq. [%]");
  } else h->Reset();
  fContainer->AddAt(h, kNtrackletsCross);

  if(!(h = (TH1F *)gROOT->FindObject("hNtlsFindable"))){
    h = new TH1F("hNtlsFindable", "Found/Findable Tracklets" , 101, -0.005, 1.005);
    h->GetXaxis()->SetTitle("r [a.u]");
    h->GetYaxis()->SetTitle("Entries");
  } else h->Reset();
  fContainer->AddAt(h, kNtrackletsFindable);

  if(!(h = (TH1F *)gROOT->FindObject("hNtrks"))){
    h = new TH1F("hNtrks", "N_{tracks} / event", 100, 0, 100);
    h->GetXaxis()->SetTitle("N_{tracks}");
    h->GetYaxis()->SetTitle("Entries");
  } else h->Reset();
  fContainer->AddAt(h, kNtracksEvent);

  if(!(h = (TH1F *)gROOT->FindObject("hNtrksSector"))){
    h = new TH1F("hNtrksSector", "N_{tracks} / sector", AliTRDgeometry::kNsector, -0.5, 17.5);
    h->GetXaxis()->SetTitle("sector");
    h->GetYaxis()->SetTitle("freq. [%]");
  } else h->Reset();
  fContainer->AddAt(h, kNtracksSector);

  // <PH> histos
  TObjArray *arr = new TObjArray(2);
  arr->SetOwner(kTRUE);  arr->SetName("<PH>");
  fContainer->AddAt(arr, kPH);
  if(!(h = (TH1F *)gROOT->FindObject("hPHt"))){
    h = new TProfile("hPHt", "<PH>", 31, -0.5, 30.5);
    h->GetXaxis()->SetTitle("Time / 100ns");
    h->GetYaxis()->SetTitle("<PH> [a.u]");
  } else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH1F *)gROOT->FindObject("hPHx")))
    h = new TProfile("hPHx", "<PH>", 31, -0.08, 4.88);
  else h->Reset();
  arr->AddAt(h, 1);

  // Chi2 histos
  arr = new TObjArray(2);
  arr->SetOwner(kTRUE); arr->SetName("Chi2");
  fContainer->AddAt(arr, kChi2);
  if(!(h = (TH1F *)gROOT->FindObject("hChi2")))
    h = new TH1F("hChi2", "#Chi2", 200, 0, 20);
  else h->Reset();
  arr->AddAt(h, 0);
  if(!(h = (TH1F *)gROOT->FindObject("hChi2n")))
    h = new TH1F("hChi2n", "Norm. Chi2 (tracklets)", 50, 0, 5);
  else h->Reset();
  arr->AddAt(h, 1);


  if(!(h = (TH1F *)gROOT->FindObject("hQcl"))){
    h = new TH1F("hQcl", "Q_{cluster}", 200, 0, 1200);
    h->GetXaxis()->SetTitle("Q_{cluster} [a.u.]");
    h->GetYaxis()->SetTitle("Entries");
  }else h->Reset();
  fContainer->AddAt(h, kChargeCluster);

  if(!(h = (TH1F *)gROOT->FindObject("hQtrklt"))){
    h = new TH1F("hQtrklt", "Q_{tracklet}", 6000, 0, 6000);
    h->GetXaxis()->SetTitle("Q_{tracklet} [a.u.]");
    h->GetYaxis()->SetTitle("Entries");
  }else h->Reset();
  fContainer->AddAt(h, kChargeTracklet);


  if(!(h = (TH1F *)gROOT->FindObject("hEventsTrigger")))
    h = new TH1F("hEventsTrigger", "Trigger Class", 100, 0, 100);
  else h->Reset();
  fContainer->AddAt(h, kNeventsTrigger);

  if(!(h = (TH1F *)gROOT->FindObject("hEventsTriggerTracks")))
    h = new TH1F("hEventsTriggerTracks", "Trigger Class (Tracks)", 100, 0, 100);
  else h->Reset();
  fContainer->AddAt(h, kNeventsTriggerTracks);

  if(!(h = (TH1F *)gROOT->FindObject("hTriggerPurity"))){
    h = new TH1F("hTriggerPurity", "Trigger Purity", 10, -0.5, 9.5);
    h->GetXaxis()->SetTitle("Trigger Cluster");
    h->GetYaxis()->SetTitle("freq.");
  } else h->Reset();
  fContainer->AddAt(h, kTriggerPurity);

  return fContainer;
}

/*
* Plotting Functions
*/

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotNClustersTracklet(const AliTRDtrackV1 *track){
  //
  // Plot the mean number of clusters per tracklet
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNclustersTracklet)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK()) continue;
    h->Fill(tracklet->GetN2());
  }
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotNClustersTrack(const AliTRDtrackV1 *track){
  //
  // Plot the number of clusters in one track
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNclustersTrack)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  
  Int_t nclusters = 0;
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK()) continue;
    nclusters += tracklet->GetN();
    if(fDebugLevel > 2){
      Int_t crossing = Int_t(tracklet->IsRowCross());
      Int_t detector = tracklet->GetDetector();
      Float_t theta = TMath::ATan(tracklet->GetZref(1));
      Float_t phi = TMath::ATan(tracklet->GetYref(1));
      Float_t momentum = 0.;
      Int_t pdg = 0;
      Int_t kinkIndex = fESD ? fESD->GetKinkIndex() : 0;
      UShort_t TPCncls = fESD ? fESD->GetTPCncls() : 0;
      if(fMC){
        if(fMC->GetTrackRef()) momentum = fMC->GetTrackRef()->P();
        pdg = fMC->GetPDG();
      }
      (*fDebugStream) << "NClustersTrack"
        << "Detector="  << detector
        << "crossing="  << crossing
        << "momentum="	<< momentum
        << "pdg="				<< pdg
        << "theta="			<< theta
        << "phi="				<< phi
        << "kinkIndex="	<< kinkIndex
        << "TPCncls="		<< TPCncls
        << "nclusters=" << nclusters
        << "\n";
    }
  }
  h->Fill(nclusters);
  return h;
}


//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotNTrackletsTrack(const AliTRDtrackV1 *track){
  //
  // Plot the number of tracklets
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNtrackletsTrack)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  Int_t nTracklets = fTrack->GetNumberOfTracklets();
  h->Fill(nTracklets);
  if(fDebugLevel > 3){
    if(nTracklets == 1){
      // If we have one Tracklet, check in which layer this happens
      Int_t layer = -1;
      AliTRDseedV1 *tracklet = 0x0;
      for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
        if((tracklet = fTrack->GetTracklet(il)) && tracklet->IsOK()){layer =  il; break;}
      }
      (*fDebugStream) << "NTrackletsTrack"
        << "Layer=" << layer
        << "\n";
    }
  }
  return h;
}


//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotNTrackletsRowCross(const AliTRDtrackV1 *track){
  //
  // Plot the number of tracklets
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNtrackletsCross)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }

  Int_t ncross = 0;
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
    if(!(tracklet = fTrack->GetTracklet(il)) || !tracklet->IsOK()) continue;

    if(tracklet->IsRowCross()) ncross++;
  }
  h->Fill(ncross);
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotFindableTracklets(const AliTRDtrackV1 *track){
  //
  // Plots the ratio of number of tracklets vs.
  // number of findable tracklets
  //
  // Findable tracklets are defined as track prolongation
  // to layer i does not hit the dead area +- epsilon
  //
  // In order to check whether tracklet hist active area in Layer i, 
  // the track is refitted and the fitted position + an uncertainty 
  // range is compared to the chamber border (also with a different
  // uncertainty)
  //
  // For the track fit two cases are distinguished:
  // If the track is a stand alone track (defined by the status bit 
  // encoding, then the track is fitted with the tilted Rieman model
  // Otherwise the track is fitted with the Kalman fitter in two steps:
  // Since the track parameters are give at the outer point, we first 
  // fit in direction inwards. Afterwards we fit again in direction outwards
  // to extrapolate the track to layers which are not reached by the track
  // For the Kalman model, the radial track points have to be shifted by
  // a distance epsilon in the direction that we want to fit
  //
  const Float_t epsilon = 0.01;   // dead area tolerance
  const Float_t epsilon_R = 1;    // shift in radial direction of the anode wire position (Kalman filter only)
  const Float_t delta_y = 0.7;    // Tolerance in the track position in y-direction
  const Float_t delta_z = 7.0;    // Tolerance in the track position in z-direction (Padlength)
  Double_t x_anode[AliTRDgeometry::kNlayer] = {300.2, 312.8, 325.4, 338.0, 350.6, 363.2}; // Take the default X0
 
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNtrackletsFindable)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  Int_t nFound = 0, nFindable = 0;
  Int_t stack = -1;
  Double_t ymin = 0., ymax = 0., zmin = 0., zmax = 0.;
  Double_t y = 0., z = 0.;
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDpadPlane *pp;  
  for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
    if((tracklet = fTrack->GetTracklet(il)) && tracklet->IsOK()){
      tracklet->SetReconstructor(fReconstructor);
      nFound++;
    }
  }
  // 2 Different cases:
  // 1st stand alone: here we cannot propagate, but be can do a Tilted Rieman Fit
  // 2nd barrel track: here we propagate the track to the layers
  AliTrackPoint points[6];
  Float_t xyz[3];
  memset(xyz, 0, sizeof(Float_t) * 3);
  if(((fESD->GetStatus() & AliESDtrack::kTRDout) > 0) && !((fESD->GetStatus() & AliESDtrack::kTRDin) > 0)){
    // stand alone track
    for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
      xyz[0] = x_anode[il];
      points[il].SetXYZ(xyz);
    }
    AliTRDtrackerV1::FitRiemanTilt(const_cast<AliTRDtrackV1 *>(fTrack), 0x0, kTRUE, 6, points);
  } else {
    // barrel track
    //
    // 2 Steps:
    // -> Kalman inwards
    // -> Kalman outwards
    AliTRDtrackV1 copy_track(*fTrack);  // Do Kalman on a (non-constant) copy of the track
    AliTrackPoint points_inward[6], points_outward[6];
    for(Int_t il = AliTRDgeometry::kNlayer; il--;){
      // In order to avoid complications in the Kalman filter if the track points have the same radial
      // position like the tracklets, we have to shift the radial postion of the anode wire by epsilon
      // in the direction we want to go
      // The track points have to be in reverse order for the Kalman Filter inwards
      xyz[0] = x_anode[AliTRDgeometry::kNlayer - il - 1] - epsilon_R;
      points_inward[il].SetXYZ(xyz);
      xyz[0] = x_anode[il] + epsilon_R;
      points_outward[il].SetXYZ(xyz);
    }
    /*for(Int_t ipt = 0; ipt < AliTRDgeometry::kNlayer; ipt++)
      printf("%d. X = %f\n", ipt, points[ipt].GetX());*/
    // Kalman inwards
    AliTRDtrackerV1::FitKalman(&copy_track, 0x0, kFALSE, 6, points_inward);
    memcpy(points, points_inward, sizeof(AliTrackPoint) * 6); // Preliminary store the inward results in the Array points
    // Kalman outwards
    AliTRDtrackerV1::FitKalman(&copy_track, 0x0, kTRUE, 6, points_inward);
    memcpy(points, points_outward, sizeof(AliTrackPoint) * AliTRDgeometry::kNlayer);
  }
  for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
    y = points[il].GetY();
    z = points[il].GetZ();
    if((stack = fGeo->GetStack(z, il)) < 0) continue; // Not findable
    pp = fGeo->GetPadPlane(il, stack);
    ymin = pp->GetCol0() + epsilon;
    ymax = pp->GetColEnd() - epsilon; 
    zmin = pp->GetRowEnd() + epsilon; 
    zmax = pp->GetRow0() - epsilon;
    // ignore y-crossing (material)
    if((z + delta_z > zmin && z - delta_z < zmax) && (y + delta_y > ymin && y - delta_y < ymax)) nFindable++;
      if(fDebugLevel > 3){
        Double_t pos_tracklet[2] = {tracklet ? tracklet->GetYfit(0) : 0, tracklet ? tracklet->GetMeanz() : 0};
        Int_t hasTracklet = tracklet ? 1 : 0;
        (*fDebugStream)   << "FindableTracklets"
          << "layer="     << il
          << "ytracklet=" << pos_tracklet[0]
          << "ytrack="    << y
          << "ztracklet=" << pos_tracklet[1]
          << "ztrack="    << z
          << "tracklet="  << hasTracklet
          << "\n";
      }
  }
  
  h->Fill(nFindable > 0 ? TMath::Min(nFound/static_cast<Double_t>(nFindable), 1.) : 1);
  if(fDebugLevel > 2) AliInfo(Form("Findable[Found]: %d[%d|%f]", nFindable, nFound, nFound/static_cast<Float_t>(nFindable > 0 ? nFindable : 1)));
  return h;
}


//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotChi2(const AliTRDtrackV1 *track){
  //
  // Plot the chi2 of the track
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(((TObjArray*)(fContainer->At(kChi2)))->At(0)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  h->Fill(fTrack->GetChi2());
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotChi2Norm(const AliTRDtrackV1 *track){
  //
  // Plot the chi2 of the track
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(((TObjArray*)(fContainer->At(kChi2)))->At(1)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  Int_t nTracklets = 0;
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK()) continue;
    nTracklets++;
  }
  h->Fill(fTrack->GetChi2()/nTracklets);
  return h;
}


//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotPHt(const AliTRDtrackV1 *track){
  //
  // Plot the average pulse height
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TProfile *h = 0x0;
  if(!(h = dynamic_cast<TProfile *>(((TObjArray*)(fContainer->At(kPH)))->At(0)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDcluster *c = 0x0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK())continue;
    Int_t crossing = Int_t(tracklet->IsRowCross());
    Int_t detector = tracklet->GetDetector();
    for(Int_t itime = 0; itime < AliTRDtrackerV1::GetNTimeBins(); itime++){
      if(!(c = tracklet->GetClusters(itime))) continue;
      Int_t localtime        = c->GetLocalTimeBin();
      Double_t absolute_charge = TMath::Abs(c->GetQ());
      h->Fill(localtime, absolute_charge);
      if(fDebugLevel > 3){
        Double_t distance[2];
        GetDistanceToTracklet(distance, tracklet, c);
        Float_t theta = TMath::ATan(tracklet->GetZref(1));
        Float_t phi = TMath::ATan(tracklet->GetYref(1));
        Float_t momentum = 0.;
        Int_t pdg = 0;
        Int_t kinkIndex = fESD ? fESD->GetKinkIndex() : 0;
        UShort_t TPCncls = fESD ? fESD->GetTPCncls() : 0;
        if(fMC){
          if(fMC->GetTrackRef()) momentum = fMC->GetTrackRef()->P();
          pdg = fMC->GetPDG();
        }
        (*fDebugStream) << "PHt"
          << "Detector="	<< detector
          << "crossing="	<< crossing
          << "Timebin="		<< localtime
          << "Charge="		<< absolute_charge
          << "momentum="	<< momentum
          << "pdg="				<< pdg
          << "theta="			<< theta
          << "phi="				<< phi
          << "kinkIndex="	<< kinkIndex
          << "TPCncls="		<< TPCncls
          << "dy="        << distance[0]
          << "dz="        << distance[1]
          << "c.="        << c
          << "\n";
      }
    }
  }
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotPHx(const AliTRDtrackV1 *track){
  //
  // Plots the average pulse height vs the distance from the anode wire
  // (plus const anode wire offset)
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TProfile *h = 0x0;
  if(!(h = dynamic_cast<TProfile *>(((TObjArray*)(fContainer->At(kPH)))->At(1)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  Float_t offset = .5*AliTRDgeometry::CamHght();
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDcluster *c = 0x0;
  Double_t distance = 0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !(tracklet->IsOK())) continue;
    tracklet->ResetClusterIter();
    while((c = tracklet->NextCluster())){
      distance = tracklet->GetX0() - c->GetX() + offset;
      h->Fill(distance, TMath::Abs(c->GetQ()));
    }
  }  
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotChargeCluster(const AliTRDtrackV1 *track){
  //
  // Plot the cluster charge
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kChargeCluster)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDcluster *c = 0x0;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK())continue;
    for(Int_t itime = 0; itime < AliTRDtrackerV1::GetNTimeBins(); itime++){
      if(!(c = tracklet->GetClusters(itime))) continue;
      h->Fill(c->GetQ());
    }
  }
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotChargeTracklet(const AliTRDtrackV1 *track){
  //
  // Plot the charge deposit per chamber
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kChargeTracklet)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDcluster *c = 0x0;
  Double_t Qtot = 0;
  Int_t nTracklets =fTrack->GetNumberOfTracklets();
  for(Int_t itl = 0x0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK()) continue;
    Qtot = 0.;
    for(Int_t ic = AliTRDseed::knTimebins; ic--;){
      if(!(c = tracklet->GetClusters(ic))) continue;
      Qtot += TMath::Abs(c->GetQ());
    }
    h->Fill(Qtot);
    if(fDebugLevel > 3){
      Int_t crossing = (Int_t)tracklet->IsRowCross();
      Int_t detector = tracklet->GetDetector();
      Float_t theta = TMath::ATan(tracklet->GetZfit(1));
      Float_t phi = TMath::ATan(tracklet->GetYfit(1));
      Float_t momentum = 0.;
      Int_t pdg = 0;
      Int_t kinkIndex = fESD ? fESD->GetKinkIndex() : 0;
      UShort_t TPCncls = fESD ? fESD->GetTPCncls() : 0;
      if(fMC){
	      if(fMC->GetTrackRef()) momentum = fMC->GetTrackRef()->P();
        pdg = fMC->GetPDG();
      }
      (*fDebugStream) << "ChargeTracklet"
        << "Detector="  << detector
        << "crossing="  << crossing
        << "momentum="	<< momentum
        << "nTracklets="<< nTracklets
        << "pdg="				<< pdg
        << "theta="			<< theta
        << "phi="				<< phi
        << "kinkIndex="	<< kinkIndex
        << "TPCncls="		<< TPCncls
        << "QT="        << Qtot
        << "\n";
    }
  }
  return h;
}

//_______________________________________________________
TH1 *AliTRDcheckDetector::PlotNTracksSector(const AliTRDtrackV1 *track){
  //
  // Plot the number of tracks per Sector
  //
  if(track) fTrack = track;
  if(!fTrack){
    AliWarning("No Track defined.");
    return 0x0;
  }
  TH1 *h = 0x0;
  if(!(h = dynamic_cast<TH1F *>(fContainer->At(kNtracksSector)))){
    AliWarning("No Histogram defined.");
    return 0x0;
  }

  // TODO we should compare with
  // sector = Int_t(track->GetAlpha() / AliTRDgeometry::GetAlpha());

  AliTRDseedV1 *tracklet = 0x0;
  Int_t sector = -1;
  for(Int_t itl = 0; itl < AliTRDgeometry::kNlayer; itl++){
    if(!(tracklet = fTrack->GetTracklet(itl)) || !tracklet->IsOK()) continue;
    sector = static_cast<Int_t>(tracklet->GetDetector()/AliTRDgeometry::kNdets);
    break;
  }
  h->Fill(sector);
  return h;
}


//________________________________________________________
void AliTRDcheckDetector::SetRecoParam(AliTRDrecoParam *r)
{

  fReconstructor->SetRecoParam(r);
}

//________________________________________________________
void AliTRDcheckDetector::GetDistanceToTracklet(Double_t *dist, AliTRDseedV1 *tracklet, AliTRDcluster *c)
{
  Float_t x = c->GetX();
  dist[0] = c->GetY() - tracklet->GetYat(x);
  dist[1] = c->GetZ() - tracklet->GetZat(x);
}

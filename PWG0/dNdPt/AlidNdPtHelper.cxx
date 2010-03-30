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

#include <TROOT.h>
#include <TParticle.h>
#include <TParticlePDG.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TList.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TArrayI.h>
#include <TF1.h>
#include <TLorentzVector.h>

#include <AliHeader.h>
#include <AliStack.h>
#include <AliLog.h>

#include <AliESD.h>
#include <AliESDEvent.h>
#include <AliMCEvent.h>
#include <AliESDVertex.h>
#include <AliVertexerTracks.h>

#include <AliGenEventHeader.h>
#include <AliGenPythiaEventHeader.h>
#include <AliGenCocktailEventHeader.h>
#include <AliGenDPMjetEventHeader.h>

#include <AliMathBase.h>
#include <AliESDtrackCuts.h>
#include "dNdPt/AlidNdPtEventCuts.h"
#include "dNdPt/AlidNdPtAcceptanceCuts.h"
#include "dNdPt/AlidNdPtHelper.h"

//____________________________________________________________________
ClassImp(AlidNdPtHelper)

//____________________________________________________________________
const AliESDVertex* AlidNdPtHelper::GetVertex(AliESDEvent* aEsd, AlidNdPtEventCuts *evtCuts, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts, AnalysisMode analysisMode, Bool_t debug, Bool_t bRedoTPC, Bool_t bUseMeanVertex)
{
  // Get the vertex from the ESD and returns it if the vertex is valid
  //
  // Second argument decides which vertex is used (this selects
  // also the quality criteria that are applied)

  if(!aEsd) 
  { 
    ::Error("AlidNdPtHelper::GetVertex()","esd event is NULL");
    return NULL;  
  }
 
  if(!evtCuts || !accCuts || !trackCuts) 
  { 
    ::Error("AlidNdPtHelper::GetVertex()","cuts not available");
    return NULL;  
  }

  const AliESDVertex* vertex = 0;
  AliESDVertex *initVertex = 0;
  if (analysisMode == kSPD || analysisMode == kTPCITS || analysisMode == kTPCSPDvtx || analysisMode == kTPCSPDvtxUpdate)
  {
    vertex = aEsd->GetPrimaryVertexSPD();
    if (debug)
      Printf("AlidNdPtHelper::GetVertex: Returning SPD vertex");
  }
  else if (analysisMode == kTPC)
  {
    if(bRedoTPC) {

      Double_t kBz = aEsd->GetMagneticField();
      AliVertexerTracks vertexer(kBz);

      if(bUseMeanVertex) {
	 Double_t pos[3]={evtCuts->GetMeanXv(),evtCuts->GetMeanYv(),evtCuts->GetMeanZv()};
	 Double_t err[3]={evtCuts->GetSigmaMeanXv(),evtCuts->GetSigmaMeanYv(),evtCuts->GetSigmaMeanZv()};
	 initVertex = new AliESDVertex(pos,err);
	 vertexer.SetVtxStart(initVertex);
	 vertexer.SetConstraintOn();
      }

      Double_t maxDCAr = accCuts->GetMaxDCAr();
      Double_t maxDCAz = accCuts->GetMaxDCAz();
      Int_t minTPCClust = trackCuts->GetMinNClusterTPC();

      //vertexer.SetTPCMode(Double_t dcacut=0.1, Double_t dcacutIter0=1.0, Double_t maxd0z0=5.0, Int_t minCls=10, Int_t mintrks=1, Double_t nsigma=3., Double_t mindetfitter=0.1, Double_t maxtgl=1.5, Double_t fidR=3., Double_t fidZ=30., Int_t finderAlgo=1, Int_t finderAlgoIter0=4);
      vertexer.SetTPCMode(0.1,1.0,5.0,minTPCClust,1,3.,0.1,2.0,maxDCAr,maxDCAz,1,4);

      // TPC track preselection
      Int_t ntracks = aEsd->GetNumberOfTracks();
      TObjArray array(ntracks);
      UShort_t *id = new UShort_t[ntracks];

      Int_t count=0;
      for (Int_t i=0;i <ntracks; i++) {
        AliESDtrack *t = aEsd->GetTrack(i);
        if (!t) continue;
        if (t->Charge() == 0) continue;
        if (!t->GetTPCInnerParam()) continue;
        if (t->GetTPCNcls()<vertexer.GetMinClusters()) continue;
        AliExternalTrackParam  *tpcTrack  = new AliExternalTrackParam(*(t->GetTPCInnerParam()));
	if(tpcTrack) { 
	  array.AddLast(tpcTrack);
	  id[count] = (UShort_t)t->GetID();
	  count++;
	}
      } 
      AliESDVertex *vTPC = vertexer.VertexForSelectedTracks(&array,id, kTRUE, kTRUE, bUseMeanVertex);
      
      // set recreated TPC vertex
      aEsd->SetPrimaryVertexTPC(vTPC);

      for (Int_t i=0; i<aEsd->GetNumberOfTracks(); i++) {
	AliESDtrack *t = aEsd->GetTrack(i);
	t->RelateToVertexTPC(vTPC, kBz, kVeryBig);
      }
      
      delete vTPC;
      array.Delete();
      delete [] id; id=NULL;

    }
    vertex = aEsd->GetPrimaryVertexTPC();
    if (debug)
      Printf("AlidNdPtHelper::GetVertex: Returning vertex from tracks");
    }
      else
       Printf("AlidNdPtHelper::GetVertex: ERROR: Invalid second argument %d", analysisMode);

    if (!vertex) {
     if (debug)
      Printf("AlidNdPtHelper::GetVertex: No vertex found in ESD");
      return 0;
    }

  if (debug)
  {
    Printf("AlidNdPtHelper::GetVertex: Returning valid vertex: %s", vertex->GetTitle());
    vertex->Print();
  }
  
  if(initVertex) delete initVertex; initVertex=NULL;
  return vertex;
}

//____________________________________________________________________
Bool_t AlidNdPtHelper::TestRecVertex(const AliESDVertex* vertex, AnalysisMode analysisMode, Bool_t debug)
{
  // Checks if a vertex meets the needed quality criteria
  if(!vertex) return kFALSE;

  Float_t requiredZResolution = -1;
  if (analysisMode == kSPD || analysisMode == kTPCITS || analysisMode == kTPCSPDvtx || analysisMode == kTPCSPDvtxUpdate)
  {
    requiredZResolution = 0.1;
  }
  else if (analysisMode == kTPC)
    requiredZResolution = 10.;

  // check Ncontributors
  if (vertex->GetNContributors() <= 0) {
    if (debug){
      Printf("AlidNdPtHelper::GetVertex: NContributors() <= 0: %d",vertex->GetNContributors());
      Printf("AlidNdPtHelper::GetVertex: NIndices(): %d",vertex->GetNIndices());
      vertex->Print();
    }
    return kFALSE;
  }

  // check resolution
  Double_t zRes = vertex->GetZRes();
  if (zRes == 0) {
    Printf("AlidNdPtHelper::GetVertex: UNEXPECTED: resolution is 0.");
    return kFALSE;
  }

  if (zRes > requiredZResolution) {
    if (debug)
      Printf("AlidNdPtHelper::TestVertex: Resolution too poor %f (required: %f", zRes, requiredZResolution);
    return kFALSE;
  }

  return kTRUE;
}

//____________________________________________________________________
Bool_t AlidNdPtHelper::IsPrimaryParticle(AliStack* stack, Int_t idx, ParticleMode particleMode)
{
// check primary particles 
// depending on the particle mode
//
  if(!stack) return kFALSE;

  TParticle* particle = stack->Particle(idx);
  if (!particle) return  kFALSE;

  // only charged particles
  Double_t charge = particle->GetPDG()->Charge()/3.;
  if (charge == 0.0) return kFALSE;

  Int_t pdg = TMath::Abs(particle->GetPdgCode());

  // physical primary
  Bool_t prim = stack->IsPhysicalPrimary(idx);

  if(particleMode==kMCPion) {
    if(prim && pdg==kPiPlus) return kTRUE;
    else return kFALSE;
  } 

  if (particleMode==kMCKaon) {
    if(prim && pdg==kKPlus) return kTRUE;
    else return kFALSE;
  }
    
  if (particleMode==kMCProton) {
    if(prim && pdg==kProton) return kTRUE;
    else return kFALSE;
  }

return prim;
}

//____________________________________________________________________
/*
Bool_t AlidNdPtHelper::IsCosmicTrack(TObjArray *allChargedTracks, AliESDtrack *track1, Int_t trackIdx, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts)
{
//
// check cosmic tracks
//
  if(!allChargedTracks) return kFALSE;
  if(!track1) return kFALSE;
  if(!accCuts) return kFALSE;
  if(!trackCuts) return kFALSE;

  Int_t entries = allChargedTracks->GetEntries();
  for(Int_t i=0; i<entries;++i) 
  {
      //
      // exclude the same tracks
      //
      if(i == trackIdx) continue;

      AliESDtrack *track2 = (AliESDtrack*)allChargedTracks->At(i);
      if(!track2) continue;
      if(track2->Charge()==0) continue;

      if(track1->Pt() > 6. && track2->Pt() > 6. && (track1->Charge() + track2->Charge()) == 0 ) 
      {
        printf("track1->Theta() %f, track1->Eta() %f, track1->Phi() %f, track1->Charge() %d  \n", track1->Theta(), track1->Eta(), track1->Phi(), track1->Charge());
        printf("track2->Theta() %f, track2->Eta() %f, track2->Phi() %f, track2->Charge() %d  \n", track2->Theta(), track2->Eta(), track2->Phi(), track2->Charge());

        printf("deta %f, dphi %f, dq %d  \n", track1->Eta()-track2->Eta(), track1->Phi()-track2->Phi(), track1->Charge()+track2->Charge()); 

      }

      //
      // cosmic tracks in TPC
      //
      //if( TMath::Abs( track1->Theta() - track2->Theta() ) < 0.004  && 
      //  ((TMath::Abs(track1->Phi()-track2->Phi()) - TMath::Pi() )<0.004) && 
      if( (track1->Pt()-track2->Pt()) < 0.1 && track1->Pt() > 4.0 && (track1->Charge()+track2->Charge()) == 0 )
      {
        //printf("COSMIC  candidate \n");
        printf("track1->Theta() %f, track1->Eta() %f, track1->Phi() %f, track1->Charge() %d  \n", track1->Theta(), track1->Eta(), track1->Phi(), track1->Charge());
        printf("track2->Theta() %f, track2->Eta() %f, track2->Phi() %f, track2->Charge() %d  \n", track2->Theta(), track2->Eta(), track2->Phi(), track2->Charge());
        printf("dtheta %f, deta %f, dphi %f, dq %d  \n", track1->Theta()-track2->Theta(),  track1->Eta()-track2->Eta(), track1->Phi()-track2->Phi(), track1->Charge()+track2->Charge()); 
	return kTRUE;
      }
   }
     
return kFALSE; 
}
*/

//____________________________________________________________________
Bool_t AlidNdPtHelper::IsCosmicTrack(AliESDtrack *track1, AliESDtrack *track2, Int_t /*trackIdx*/, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts)
{
//
// check cosmic tracks
//
  if(!track1) return kFALSE;
  if(!track2) return kFALSE;
  if(!accCuts) return kFALSE;
  if(!trackCuts) return kFALSE;

  /*
  if(track1->Pt() > 6. && track2->Pt() > 6. && (track1->Charge() + track2->Charge()) == 0 ) 
  {
        printf("track1->Theta() %f, track1->Eta() %f, track1->Phi() %f, track1->Charge() %d  \n", track1->Theta(), track1->Eta(), track1->Phi(), track1->Charge());
        printf("track2->Theta() %f, track2->Eta() %f, track2->Phi() %f, track2->Charge() %d  \n", track2->Theta(), track2->Eta(), track2->Phi(), track2->Charge());

        printf("deta %f, dphi %f, dq %d  \n", track1->Eta()-track2->Eta(), track1->Phi()-track2->Phi(), track1->Charge()+track2->Charge()); 

      }
      */

      //
      // cosmic tracks in TPC
      //
      //if( TMath::Abs( track1->Theta() - track2->Theta() ) < 0.004  && 
      //  ((TMath::Abs(track1->Phi()-track2->Phi()) - TMath::Pi() )<0.004) && 

      if( (track1->Pt()-track2->Pt()) < 0.2 && track1->Pt() > 3.0 && 
	     (track1->Charge()+track2->Charge()) == 0 )
      {
        //printf("COSMIC  candidate \n");
	/*
        printf("track1->Theta() %f, track1->Eta() %f, track1->Phi() %f, track1->Charge() %d  \n", track1->Theta(), track1->Eta(), track1->Phi(), track1->Charge());
        printf("track2->Theta() %f, track2->Eta() %f, track2->Phi() %f, track2->Charge() %d  \n", track2->Theta(), track2->Eta(), track2->Phi(), track2->Charge());
        printf("dtheta %f, deta %f, dphi %f, dq %d  \n", track1->Theta()-track2->Theta(),  track1->Eta()-track2->Eta(), track1->Phi()-track2->Phi(), track1->Charge()+track2->Charge()); 
	*/
	return kTRUE;
      }
     
return kFALSE; 
}

//____________________________________________________________________
void AlidNdPtHelper::PrintConf(AnalysisMode analysisMode, AliTriggerAnalysis::Trigger trigger)
{
  //
  // Prints the given configuration
  //

  TString str(">>>> Running with ");

  switch (analysisMode)
  {
    case kInvalid: str += "invalid setting"; break;
    case kSPD : str += "SPD-only"; break;
    case kTPC : str += "TPC-only"; break;
    case kTPCITS : str += "Global tracking"; break;
    case kTPCSPDvtx : str += "TPC tracking + SPD event vertex"; break;
    case kTPCSPDvtxUpdate : str += "TPC tracks updated with SPD event vertex point"; break;
    case kMCRec : str += "TPC tracking + Replace rec. with MC values"; break;
  }
  str += " and trigger ";

  str += AliTriggerAnalysis::GetTriggerName(trigger);

  str += " <<<<";

  Printf("%s", str.Data());
}

//____________________________________________________________________
Int_t AlidNdPtHelper::ConvertPdgToPid(TParticle *particle) {
//
// Convert Abs(pdg) to pid 
// (0 - e, 1 - muons, 2 - pions, 3 - kaons, 4 - protons, 5 -all rest)
//
Int_t pid=-1;

  if (TMath::Abs(particle->GetPdgCode()) == kElectron)         { pid = 0; }
  else if (TMath::Abs(particle->GetPdgCode()) == kMuonMinus) { pid = 1; }
  else if (TMath::Abs(particle->GetPdgCode()) == kPiPlus)    { pid = 2; }
  else if (TMath::Abs(particle->GetPdgCode()) == kKPlus)     { pid = 3; }
  else if (TMath::Abs(particle->GetPdgCode()) == kProton)    { pid = 4; }
  else                                                       { pid = 5; }

return pid;
}

//_____________________________________________________________________________
TH1F* AlidNdPtHelper::CreateResHisto(TH2F* hRes2, TH1F **phMean, Int_t integ,  Bool_t drawBinFits, Int_t minHistEntries)
{
//
// Create mean and resolution 
// histograms
//
  TVirtualPad* currentPad = gPad;
  TAxis* axis = hRes2->GetXaxis();
  Int_t nBins = axis->GetNbins();
  Bool_t overflowBinFits = kFALSE;
  TH1F* hRes, *hMean;
  if (axis->GetXbins()->GetSize()){
    hRes = new TH1F("hRes", "", nBins, axis->GetXbins()->GetArray());
    hMean = new TH1F("hMean", "", nBins, axis->GetXbins()->GetArray());
  }
  else{
    hRes = new TH1F("hRes", "", nBins, axis->GetXmin(), axis->GetXmax());
    hMean = new TH1F("hMean", "", nBins, axis->GetXmin(), axis->GetXmax());

  }
  hRes->SetStats(false);
  hRes->SetOption("E");
  hRes->SetMinimum(0.);
  //
  hMean->SetStats(false);
  hMean->SetOption("E");
 
  // create the fit function
  TF1 * fitFunc = new TF1("G","[0]*exp(-(x-[1])*(x-[1])/(2.*[2]*[2]))",-3,3);
  
  fitFunc->SetLineWidth(2);
  fitFunc->SetFillStyle(0);
  // create canvas for fits
  TCanvas* canBinFits = NULL;
  Int_t nPads = (overflowBinFits) ? nBins+2 : nBins;
  Int_t nx = Int_t(sqrt(nPads-1.));// + 1;
  Int_t ny = (nPads-1) / nx + 1;
  if (drawBinFits) {
    canBinFits = (TCanvas*)gROOT->FindObject("canBinFits");
    if (canBinFits) delete canBinFits;
    canBinFits = new TCanvas("canBinFits", "fits of bins", 200, 100, 500, 700);
    canBinFits->Divide(nx, ny);
  }

  // loop over x bins and fit projection
  Int_t dBin = ((overflowBinFits) ? 1 : 0);
  for (Int_t bin = 1-dBin; bin <= nBins+dBin; bin++) {
    if (drawBinFits) canBinFits->cd(bin + dBin);
    Int_t bin0=TMath::Max(bin-integ,0);
    Int_t bin1=TMath::Min(bin+integ,nBins);
    TH1D* hBin = hRes2->ProjectionY("hBin", bin0, bin1);
    //    
    if (hBin->GetEntries() > minHistEntries) {
      fitFunc->SetParameters(hBin->GetMaximum(),hBin->GetMean(),hBin->GetRMS());
      hBin->Fit(fitFunc,"s");
      Double_t sigma = TMath::Abs(fitFunc->GetParameter(2));

      if (sigma > 0.){
	hRes->SetBinContent(bin, TMath::Abs(fitFunc->GetParameter(2)));
	hMean->SetBinContent(bin, fitFunc->GetParameter(1));	
      }
      else{
	hRes->SetBinContent(bin, 0.);
	hMean->SetBinContent(bin,0);
      }
      hRes->SetBinError(bin, fitFunc->GetParError(2));
      hMean->SetBinError(bin, fitFunc->GetParError(1));
      
      //
      //

    } else {
      hRes->SetBinContent(bin, 0.);
      hRes->SetBinError(bin, 0.);
      hMean->SetBinContent(bin, 0.);
      hMean->SetBinError(bin, 0.);
    }
    

    if (drawBinFits) {
      char name[256];
      if (bin == 0) {
	sprintf(name, "%s < %.4g", axis->GetTitle(), axis->GetBinUpEdge(bin));
      } else if (bin == nBins+1) {
	sprintf(name, "%.4g < %s", axis->GetBinLowEdge(bin), axis->GetTitle());
      } else {
	sprintf(name, "%.4g < %s < %.4g", axis->GetBinLowEdge(bin),
		axis->GetTitle(), axis->GetBinUpEdge(bin));
      }
      canBinFits->cd(bin + dBin);
      hBin->SetTitle(name);
      hBin->SetStats(kTRUE);
      hBin->DrawCopy("E");
      canBinFits->Update();
      canBinFits->Modified();
      canBinFits->Update();
    }
    
    delete hBin;
  }

  delete fitFunc;
  currentPad->cd();
  *phMean = hMean;
  return hRes;
}

//_____________________________________________________________________________
TH1F* AlidNdPtHelper::MakeResol(TH2F * his, Int_t integ, Bool_t type, Bool_t drawBins, Int_t minHistEntries){
// Create resolution histograms
  
     TH1F *hisr=0, *hism=0;
     if (!gPad) new TCanvas;
         hisr = CreateResHisto(his,&hism,integ,drawBins,minHistEntries);
         if (type) return hism;
         else return hisr;

return hisr;	 
}

//_____________________________________________________________________________
TObjArray* AlidNdPtHelper::GetAllChargedTracks(AliESDEvent *esdEvent, AnalysisMode analysisMode)
{
  //
  // all charged TPC particles 
  //
  TObjArray *allTracks = new TObjArray();
  if(!allTracks) return allTracks;

  AliESDtrack *track=0;
  for (Int_t iTrack = 0; iTrack < esdEvent->GetNumberOfTracks(); iTrack++) 
  { 
    if(analysisMode == AlidNdPtHelper::kTPC) { 
      // track must be deleted by user 
      track = AliESDtrackCuts::GetTPCOnlyTrack(esdEvent,iTrack);
      if(!track) continue;
    } 
    else if (analysisMode == AlidNdPtHelper::kTPCSPDvtx || AlidNdPtHelper::kTPCSPDvtxUpdate)
    {
      // track must be deleted by the user 
      track = AlidNdPtHelper::GetTPCOnlyTrackSPDvtx(esdEvent,iTrack,kFALSE);
      if(!track) continue;
    }
    else {
      track=esdEvent->GetTrack(iTrack);
    }
    if(!track) continue;

    if(track->Charge()==0) { 
      if(analysisMode == AlidNdPtHelper::kTPC || analysisMode == AlidNdPtHelper::kTPCSPDvtx || 
         analysisMode == AlidNdPtHelper::kTPCSPDvtxUpdate) 
      {
        delete track; continue; 
      } else {
        continue;
      } 
    }

    allTracks->Add(track);
  }

  if(analysisMode == AlidNdPtHelper::kTPC || analysisMode == AlidNdPtHelper::kTPCSPDvtx || 
     analysisMode == AlidNdPtHelper::kTPCSPDvtxUpdate) {
     
     allTracks->SetOwner(kTRUE);
  }

return allTracks;
}

//_____________________________________________________________________________
AliESDtrack *AlidNdPtHelper::GetTPCOnlyTrackSPDvtx(AliESDEvent* esdEvent, Int_t iTrack, Bool_t bUpdate)
{
//
// Create ESD tracks from TPCinner parameters.
// Propagte to DCA to SPD vertex.
// Update using SPD vertex point (parameter)
//
// It is user responsibility to delete these tracks
//

  if (!esdEvent) return NULL;
  if (!esdEvent->GetPrimaryVertexSPD() ) { return NULL; }
  if (!esdEvent->GetPrimaryVertexSPD()->GetStatus() ) { return  NULL; }
   
  // 
  AliESDtrack* track = esdEvent->GetTrack(iTrack);
  if (!track)
    return NULL;

  // create new ESD track
  AliESDtrack *tpcTrack = new AliESDtrack();
 
  // relate TPC-only tracks (TPCinner) to SPD vertex
  AliExternalTrackParam cParam;
  if(bUpdate) {  
    track->RelateToVertexTPC(esdEvent->GetPrimaryVertexSPD(),esdEvent->GetMagneticField(),kVeryBig,&cParam);
    track->Set(cParam.GetX(),cParam.GetAlpha(),cParam.GetParameter(),cParam.GetCovariance());

    // reject fake tracks
    if(track->Pt() > 10000.)  {
      ::Error("Exclude no physical tracks","pt>10000. GeV");
      delete tpcTrack; 
      return NULL;
    }
  }
  else {
    track->RelateToVertexTPC(esdEvent->GetPrimaryVertexSPD(), esdEvent->GetMagneticField(), kVeryBig);
  }

  // only true if we have a tpc track
  if (!track->FillTPCOnlyTrack(*tpcTrack))
  {
    delete tpcTrack;
    return NULL;
  }

return tpcTrack;
} 

//_____________________________________________________________________________
Int_t AlidNdPtHelper::GetTPCMBTrackMult(AliESDEvent *esdEvent, AlidNdPtEventCuts *evtCuts, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts)
{
  //
  // get MB event track multiplicity
  //
  if(!esdEvent) 
  { 
    ::Error("AlidNdPtHelper::GetTPCMBTrackMult()","esd event is NULL");
    return 0;  
  }
 
  if(!evtCuts || !accCuts || !trackCuts) 
  { 
    ::Error("AlidNdPtHelper::GetTPCMBTrackMult()","cuts not available");
    return 0;  
  }

  //
  Double_t pos[3]={evtCuts->GetMeanXv(),evtCuts->GetMeanYv(),evtCuts->GetMeanZv()};
  Double_t err[3]={evtCuts->GetSigmaMeanXv(),evtCuts->GetSigmaMeanYv(),evtCuts->GetSigmaMeanZv()};
  AliESDVertex vtx0(pos,err);

  //
  Float_t maxDCAr = accCuts->GetMaxDCAr();
  Float_t maxDCAz = accCuts->GetMaxDCAz();
  Float_t minTPCClust = trackCuts->GetMinNClusterTPC();
  //
  Int_t ntracks = esdEvent->GetNumberOfTracks();
  Double_t dca[2],cov[3];
  Int_t mult=0;
  for (Int_t i=0;i <ntracks; i++){
    AliESDtrack *t = esdEvent->GetTrack(i);
    if (!t) continue;
    if (t->Charge() == 0) continue;
    if (!t->GetTPCInnerParam()) continue;
    if (t->GetTPCNcls()<minTPCClust) continue;
    //
    AliExternalTrackParam  *tpcTrack  = new AliExternalTrackParam(*(t->GetTPCInnerParam()));
    if (!tpcTrack->PropagateToDCA(&vtx0,esdEvent->GetMagneticField(),100.,dca,cov)) 
    {
      if(tpcTrack) delete tpcTrack; 
      continue;
    }
    //
    if (TMath::Abs(dca[0])>maxDCAr || TMath::Abs(dca[1])>maxDCAz) {
      if(tpcTrack) delete tpcTrack; 
      continue;
    }

    mult++;    

    if(tpcTrack) delete tpcTrack; 
  }

return mult;  
}

//_____________________________________________________________________________
Int_t AlidNdPtHelper::GetTPCMBPrimTrackMult(AliESDEvent *esdEvent, AliStack * stack, AlidNdPtEventCuts *evtCuts, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts)
{
  //
  // get MB primary event track multiplicity
  //
  if(!esdEvent) 
  { 
    ::Error("AlidNdPtHelper::GetTPCMBPrimTrackMult()","esd event is NULL");
    return 0;  
  }

  if(!stack) 
  { 
    ::Error("AlidNdPtHelper::GetTPCMBPrimTrackMult()","esd event is NULL");
    return 0;  
  }
 
  if(!evtCuts || !accCuts || !trackCuts) 
  { 
    ::Error("AlidNdPtHelper::GetTPCMBPrimTrackMult()","cuts not available");
    return 0;  
  }

  //
  Double_t pos[3]={evtCuts->GetMeanXv(),evtCuts->GetMeanYv(),evtCuts->GetMeanZv()};
  Double_t err[3]={evtCuts->GetSigmaMeanXv(),evtCuts->GetSigmaMeanYv(),evtCuts->GetSigmaMeanZv()};
  AliESDVertex vtx0(pos,err);

  //
  Float_t maxDCAr = accCuts->GetMaxDCAr();
  Float_t maxDCAz = accCuts->GetMaxDCAz();
  Float_t minTPCClust = trackCuts->GetMinNClusterTPC();

  //
  Int_t ntracks = esdEvent->GetNumberOfTracks();
  Double_t dca[2],cov[3];
  Int_t mult=0;
  for (Int_t i=0;i <ntracks; i++){
    AliESDtrack *t = esdEvent->GetTrack(i);
    if (!t) continue;
    if (t->Charge() == 0) continue;
    if (!t->GetTPCInnerParam()) continue;
    if (t->GetTPCNcls()<minTPCClust) continue;
    //
    AliExternalTrackParam  *tpcTrack  = new AliExternalTrackParam(*(t->GetTPCInnerParam()));
    if (!tpcTrack->PropagateToDCA(&vtx0,esdEvent->GetMagneticField(),100.,dca,cov)) 
    {
      if(tpcTrack) delete tpcTrack; 
      continue;
    }
    //
    if (TMath::Abs(dca[0])>maxDCAr || TMath::Abs(dca[1])>maxDCAz) {
      if(tpcTrack) delete tpcTrack; 
      continue;
    }

    Int_t label = TMath::Abs(t->GetLabel());
    TParticle *part = stack->Particle(label);
    if(!part) { 
      if(tpcTrack) delete tpcTrack; 
      continue;
    }
    if(!stack->IsPhysicalPrimary(label)) 
    { 
      if(tpcTrack) delete tpcTrack; 
      continue;
    }

    mult++;    

    if(tpcTrack) delete tpcTrack; 
  }

return mult;  
}





//_____________________________________________________________________________
Int_t AlidNdPtHelper::GetMCTrueTrackMult(AliMCEvent *mcEvent, AlidNdPtEventCuts *evtCuts, AlidNdPtAcceptanceCuts *accCuts)
{
  //
  // calculate mc event true track multiplicity
  //
  if(!mcEvent) return 0;

  AliStack* stack = 0;
  Int_t mult = 0;

  // MC particle stack
  stack = mcEvent->Stack();
  if (!stack) return 0;

  Bool_t isEventOK = evtCuts->AcceptMCEvent(mcEvent);
  if(!isEventOK) return 0; 

  Int_t nPart  = stack->GetNtrack();
  for (Int_t iMc = 0; iMc < nPart; ++iMc) 
  {
     TParticle* particle = stack->Particle(iMc);
     if (!particle)
     continue;

     // only charged particles
     Double_t charge = particle->GetPDG()->Charge()/3.;
     if (charge == 0.0)
     continue;
      
     // physical primary
     Bool_t prim = stack->IsPhysicalPrimary(iMc);
     if(!prim) continue;

     // checked accepted
     if(accCuts->AcceptTrack(particle)) 
     {
       mult++;
     }
  }

return mult;  
}

//_______________________________________________________________________
void  AlidNdPtHelper::PrintMCInfo(AliStack *pStack,Int_t label)
{
// print information about particles in the stack

  if(!pStack)return;
  label = TMath::Abs(label);
  TParticle *part = pStack->Particle(label);
  Printf("########################");
  Printf("%s:%d %d UniqueID %d PDG %d P %3.3f",(char*)__FILE__,__LINE__,label,part->GetUniqueID(),part->GetPdgCode(),part->P());
  part->Print();
  TParticle* mother = part;
  Int_t imo = part->GetFirstMother();
  Int_t nprim = pStack->GetNprimary();

  while((imo >= nprim)) {
      mother =  pStack->Particle(imo);
      Printf("Mother %s:%d Label %d UniqueID %d PDG %d P %3.3f",(char*)__FILE__,__LINE__,imo,mother->GetUniqueID(),mother->GetPdgCode(),mother->P());
      mother->Print();
      imo =  mother->GetFirstMother();
 }

 Printf("########################");
}


//_____________________________________________________________________________
TH1* AlidNdPtHelper::GetContCorrHisto(TH1 *hist) 
{
//
// get contamination histogram
//
 if(!hist) return 0;

 Int_t nbins = hist->GetNbinsX();
 TH1 *h_cont = (TH1D *)hist->Clone();

 for(Int_t i=0; i<=nbins+1; i++) {
   Double_t binContent = hist->GetBinContent(i);
   Double_t binError = hist->GetBinError(i);

   h_cont->SetBinContent(i,1.-binContent);
   h_cont->SetBinError(i,binError);
 }

return h_cont;
}


//_____________________________________________________________________________
TH1* AlidNdPtHelper::ScaleByBinWidth(TH1 *hist) 
{
//
// scale by bin width
//
 if(!hist) return 0;

 TH1 *h_scale = (TH1D *)hist->Clone();
 h_scale->Scale(1.,"width");

return h_scale;
}

//_____________________________________________________________________________
TH1* AlidNdPtHelper::CalcRelativeDifference(TH1 *hist1, TH1 *hist2) 
{
//
// calculate rel. difference 
//

 if(!hist1) return 0;
 if(!hist2) return 0;

 TH1 *h1_clone = (TH1D *)hist1->Clone();
 h1_clone->Sumw2();

 // (rec-mc)/mc
 h1_clone->Add(hist2,-1);
 h1_clone->Divide(hist2);

return h1_clone;
}

//_____________________________________________________________________________
TH1* AlidNdPtHelper::CalcRelativeDifferenceFun(TH1 *hist1, TF1 *fun) 
{
//
// calculate rel. difference
// between histogram and function
//
 if(!hist1) return 0;
 if(!fun) return 0;

 TH1 *h1_clone = (TH1D *)hist1->Clone();
 h1_clone->Sumw2();

 // 
 h1_clone->Add(fun,-1);
 h1_clone->Divide(hist1);

return h1_clone;
}

//_____________________________________________________________________________
TH1* AlidNdPtHelper::NormalizeToEvent(TH2 *hist1, TH1 *hist2) 
{
// normalise to event for a given multiplicity bin
// return pt histogram 

 if(!hist1) return 0;
 if(!hist2) return 0;
 char name[256];

 Int_t nbinsX = hist1->GetNbinsX();
 //Int_t nbinsY = hist1->GetNbinsY();

 TH1D *hist_norm = 0;
 for(Int_t i=0; i<=nbinsX+1; i++) {
   sprintf(name,"mom_%d",i);
   TH1D *hist = (TH1D*)hist1->ProjectionY(name,i+1,i+1);

   sprintf(name,"mom_norm");
   if(i==0) { 
     hist_norm = (TH1D *)hist->Clone(name);
     hist_norm->Reset();
   }

   Double_t nbEvents = hist2->GetBinContent(i);
   if(!nbEvents) { nbEvents = 1.; };

   hist->Scale(1./nbEvents);
   hist_norm->Add(hist);
 }

return hist_norm;
}

//_____________________________________________________________________________
THnSparse* AlidNdPtHelper::GenerateCorrMatrix(THnSparse *hist1, THnSparse *hist2, char *name) {
// generate correction matrix
if(!hist1 || !hist2) return 0; 

THnSparse *h =(THnSparse*)hist1->Clone(name);;
h->Divide(hist1,hist2,1,1,"B");

return h;
}

//_____________________________________________________________________________
TH2* AlidNdPtHelper::GenerateCorrMatrix(TH2 *hist1, TH2 *hist2, char *name) {
// generate correction matrix
if(!hist1 || !hist2) return 0; 

TH2D *h =(TH2D*)hist1->Clone(name);;
h->Divide(hist1,hist2,1,1,"B");

return h;
}

//_____________________________________________________________________________
TH1* AlidNdPtHelper::GenerateCorrMatrix(TH1 *hist1, TH1 *hist2, char *name) {
// generate correction matrix
if(!hist1 || !hist2) return 0; 

TH1D *h =(TH1D*)hist1->Clone(name);;
h->Divide(hist1,hist2,1,1,"B");

return h;
}

//_____________________________________________________________________________
THnSparse* AlidNdPtHelper::GenerateContCorrMatrix(THnSparse *hist1, THnSparse *hist2, char *name) {
// generate contamination correction matrix
if(!hist1 || !hist2) return 0; 

THnSparse *hist =  GenerateCorrMatrix(hist1, hist2, name);
if(!hist) return 0;

// only for non ZERO bins!!!!

Int_t* coord = new Int_t[hist->GetNdimensions()];
memset(coord, 0, sizeof(Int_t) * hist->GetNdimensions());

  for (Long64_t i = 0; i < hist->GetNbins(); ++i) {
    Double_t v = hist->GetBinContent(i, coord);
    hist->SetBinContent(coord, 1.0-v);
    //printf("v %f, hist->GetBinContent(i, coord) %f \n",v,hist->GetBinContent(i, coord));
    Double_t err = hist->GetBinError(coord);
    hist->SetBinError(coord, err);
  }

delete [] coord;

return hist;
}

//_____________________________________________________________________________
TH2* AlidNdPtHelper::GenerateContCorrMatrix(TH2 *hist1, TH2 *hist2, char *name) {
// generate contamination correction matrix
if(!hist1 || !hist2) return 0; 

TH2 *hist = GenerateCorrMatrix(hist1, hist2, name);
if(!hist) return 0;

Int_t nBinsX = hist->GetNbinsX();
Int_t nBinsY = hist->GetNbinsY();

  for (Int_t i = 0; i < nBinsX+1; i++) {
  for (Int_t j = 0; j < nBinsY+1; j++) {
     Double_t cont = hist->GetBinContent(i,j);
     hist->SetBinContent(i,j,1.-cont);
     Double_t err = hist->GetBinError(i,j);
     hist->SetBinError(i,j,err);
  }
  }

return hist;
}

//_____________________________________________________________________________
TH1* AlidNdPtHelper::GenerateContCorrMatrix(TH1 *hist1, TH1 *hist2, char *name) {
// generate contamination correction matrix
if(!hist1 || !hist2) return 0; 

TH1 *hist = GenerateCorrMatrix(hist1, hist2, name);
if(!hist) return 0;

Int_t nBinsX = hist->GetNbinsX();

  for (Int_t i = 0; i < nBinsX+1; i++) {
     Double_t cont = hist->GetBinContent(i);
     hist->SetBinContent(i,1.-cont);
     Double_t err = hist->GetBinError(i);
     hist->SetBinError(i,err);
  }

return hist;
}

//_____________________________________________________________________________
const AliESDVertex* AlidNdPtHelper::GetTPCVertexZ(AliESDEvent* esdEvent, AlidNdPtEventCuts *evtCuts, AlidNdPtAcceptanceCuts *accCuts, AliESDtrackCuts *trackCuts, Float_t fraction, Int_t ntracksMin){
  //
  // TPC Z vertexer
  //
  if(!esdEvent)
  { 
    ::Error("AlidNdPtHelper::GetTPCVertexZ()","cuts not available");
    return NULL;  
  }

  if(!evtCuts || !accCuts || !trackCuts) 
  { 
    ::Error("AlidNdPtHelper::GetTPCVertexZ()","cuts not available");
    return NULL;  
  }

  Double_t vtxpos[3]={evtCuts->GetMeanXv(),evtCuts->GetMeanYv(),evtCuts->GetMeanZv()};
  Double_t vtxsigma[3]={evtCuts->GetSigmaMeanXv(),evtCuts->GetSigmaMeanYv(),evtCuts->GetSigmaMeanZv()};
  AliESDVertex vtx0(vtxpos,vtxsigma);

  Double_t maxDCAr = accCuts->GetMaxDCAr();
  Double_t maxDCAz = accCuts->GetMaxDCAz();
  Int_t minTPCClust = trackCuts->GetMinNClusterTPC();

  //
  Int_t ntracks = esdEvent->GetNumberOfTracks();
  TVectorD ztrack(ntracks);
  Double_t dca[2],cov[3];
  Int_t counter=0;
  for (Int_t i=0;i <ntracks; i++){
    AliESDtrack *t = esdEvent->GetTrack(i);
    if (!t) continue;
    if (!t->GetTPCInnerParam()) continue;
    if (t->GetTPCNcls()<minTPCClust) continue;
    //
    AliExternalTrackParam  *tpcTrack  = new AliExternalTrackParam(*(t->GetTPCInnerParam()));
    if (!tpcTrack->PropagateToDCA(&vtx0,esdEvent->GetMagneticField(),100.,dca,cov)) continue;

    //
    if (TMath::Abs(dca[0])>maxDCAr) continue;
    //if (TMath::Sqrt(cov[0])>sigmaXYcut) continue;    
    if (TMath::Abs(tpcTrack->GetZ())>maxDCAz) continue;

    ztrack[counter]=tpcTrack->GetZ();
    counter++;    

    if(tpcTrack) delete tpcTrack;
  }

  //
  // Find LTM z position
  //
  Double_t mean=0, sigma=0;
  if (counter<ntracksMin) return 0;
  //
  Int_t nused = TMath::Nint(counter*fraction);
  if (nused==counter) nused=counter-1;  
  if (nused>1){
    AliMathBase::EvaluateUni(counter, ztrack.GetMatrixArray(), mean,sigma, TMath::Nint(counter*fraction));
    sigma/=TMath::Sqrt(nused);
  }else{
    mean  = TMath::Mean(counter, ztrack.GetMatrixArray());
    sigma = TMath::RMS(counter, ztrack.GetMatrixArray());
    sigma/=TMath::Sqrt(counter-1);
  }
  vtxpos[2]=mean;
  vtxsigma[2]=sigma;
  const AliESDVertex* vertex = new AliESDVertex(vtxpos, vtxsigma);
  return vertex;
}

//_____________________________________________________________________________
Int_t  AlidNdPtHelper::GetSPDMBTrackMult(AliESDEvent* esdEvent, Float_t deltaThetaCut, Float_t deltaPhiCut) 
{
  //
  // SPD track multiplicity
  //

  // get tracklets
  const AliMultiplicity* mult = esdEvent->GetMultiplicity();
  if (!mult)
     return 0;

  // get multiplicity from SPD tracklets
  Int_t inputCount = 0; 
  for (Int_t i=0; i<mult->GetNumberOfTracklets(); ++i)
  {
    //printf("%d %f %f %f\n", i, mult->GetTheta(i), mult->GetPhi(i), mult->GetDeltaPhi(i));

     Float_t phi = mult->GetPhi(i);
     if (phi < 0)
       phi += TMath::Pi() * 2;
     Float_t deltaPhi = mult->GetDeltaPhi(i);
     Float_t deltaTheta = mult->GetDeltaTheta(i);

     if (TMath::Abs(deltaPhi) > 1)
       printf("WARNING: Very high Delta Phi: %d %f %f %f\n", i, mult->GetTheta(i), mult->GetPhi(i), deltaPhi);

     if (deltaThetaCut > 0. && TMath::Abs(deltaTheta) > deltaThetaCut)
        continue;

     if (deltaPhiCut > 0. && TMath::Abs(deltaPhi) > deltaPhiCut)
        continue;
      
     ++inputCount;
  }

return inputCount;
}

//_____________________________________________________________________________
Int_t  AlidNdPtHelper::GetSPDMBPrimTrackMult(AliESDEvent* esdEvent, AliStack* stack, Float_t deltaThetaCut, Float_t deltaPhiCut) 
{
  //
  // SPD track multiplicity
  //

  // get tracklets
  const AliMultiplicity* mult = esdEvent->GetMultiplicity();
  if (!mult)
     return 0;

  // get multiplicity from SPD tracklets
  Int_t inputCount = 0; 
  for (Int_t i=0; i<mult->GetNumberOfTracklets(); ++i)
  {
    //printf("%d %f %f %f\n", i, mult->GetTheta(i), mult->GetPhi(i), mult->GetDeltaPhi(i));

     Float_t phi = mult->GetPhi(i);
     if (phi < 0)
       phi += TMath::Pi() * 2;
     Float_t deltaPhi = mult->GetDeltaPhi(i);
     Float_t deltaTheta = mult->GetDeltaTheta(i);

     if (TMath::Abs(deltaPhi) > 1)
       printf("WARNING: Very high Delta Phi: %d %f %f %f\n", i, mult->GetTheta(i), mult->GetPhi(i), deltaPhi);

     if (deltaThetaCut > 0. && TMath::Abs(deltaTheta) > deltaThetaCut)
        continue;

     if (deltaPhiCut > 0. && TMath::Abs(deltaPhi) > deltaPhiCut)
        continue;


     if (mult->GetLabel(i, 0) < 0 || mult->GetLabel(i, 0) != mult->GetLabel(i, 1) || 
         !stack->IsPhysicalPrimary(mult->GetLabel(i, 0)))
        continue;

      
     ++inputCount;
  }

return inputCount;
}



/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Svein Lindal <slindal@fys.uio.no   >                  *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/// @file   AliHLTEvePhos.cxx
/// @author Svein Lindal <slindal@fys.uio.no>
/// @brief  HLT class for the HLT EVE display

#include "AliHLTEveHLT.h"
#include "AliHLTHOMERBlockDesc.h"
#include "AliHLTEveBase.h"
#include "AliEveHLTEventManager.h"
#include "AliHLTGlobalTriggerDecision.h"
#include "TEveManager.h"
#include "TEvePointSet.h"
#include "TEveTrack.h"
#include "TCanvas.h"
#include "AliESDEvent.h"
#include "TEveTrackPropagator.h"
#include "AliEveTrack.h"
#include "TEveVSDStructs.h"
#include "TString.h"
#include "TPCLib/tracking-ca/AliHLTTPCCATrackParam.h"
#include "TPCLib/tracking-ca/AliHLTTPCCATrackConvertor.h"
#include "AliEveMagField.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TThread.h"

ClassImp(AliHLTEveHLT)

AliHLTEveHLT::AliHLTEveHLT() : 
  AliHLTEveBase("TPC tracks"), 
  fTrueField(kFALSE),
  fUseIpOnFailedITS(kFALSE),
  fUseRkStepper(kFALSE),
  fTrackList(NULL),
  fOldTrackList(NULL),
  fPointSetVertex(NULL),
  fTrCanvas(NULL),
  fVertexCanvas(NULL),
  fHistPt(NULL), 
  fHistP(NULL), 
  fHistEta(NULL),
  fHistTheta(NULL),
  fHistPhi(NULL),
  fHistnClusters(NULL),
  fHistMult(NULL),
  fTrCount(0), 
  fVCount(0)
{
  // Constructor.
  //CreateHistograms();
}
///___________________________________________________________________
AliHLTEveHLT::~AliHLTEveHLT()
{
  //Destructor, not implemented
  if(fTrackList)
    delete fTrackList;
  fTrackList = NULL;
}
///____________________________________________________________________
void AliHLTEveHLT::ProcessEsdEvent( AliESDEvent * esd ) {
  //See header file for documentation
  if(!fTrackList) CreateTrackList();
  if(!fPointSetVertex) CreateVertexPointSet();
  ProcessEsdEvent(esd, fTrackList);
    
}
///_____________________________________________________________________
void AliHLTEveHLT::ProcessBlock(AliHLTHOMERBlockDesc * block) {
  //See header file for documentation
  if ( ! block->GetDataType().CompareTo("ALIESDV0") ) {
    if(!fTrackList) CreateTrackList();
    if(!fPointSetVertex) CreateVertexPointSet();
    ProcessEsdBlock(block, fTrackList);
  } 
  
  else if ( ! block->GetDataType().CompareTo("ROOTTOBJ") ) {
    //processROOTTOBJ( block, gHLTText );
  } 

  else if ( ! block->GetDataType().CompareTo("HLTRDLST") ) {
    cout << "ignoring hlt rdlst"<<endl;
    //processHLTRDLST( block );
  } 

  else if ( ! block->GetDataType().CompareTo("GLOBTRIG") ) {
    ProcessGlobalTrigger( block );
  } 

  else if ( !block->GetDataType().CompareTo("ROOTHIST") ) {      
    if( !fCanvas ) { 
      fCanvas = CreateCanvas("Primary Vertex", "Primary Vertex");
      fCanvas->Divide(3, 2);
    }
    ProcessHistograms( block , fCanvas);
  }  
}

///____________________________________________________________________________
void AliHLTEveHLT::UpdateElements() {
  //See header file for documentation
  //
  //DrawHistograms();
  if(fTrackList) fTrackList->ElementChanged();
  if(fPointSetVertex) fPointSetVertex->ResetBBox();
  if(fTrCanvas) fTrCanvas->Update();
  if(fVertexCanvas) fVertexCanvas->Update();
  if(fCanvas) fCanvas->Update();
  
}
///_________________________________________________________________________________
void AliHLTEveHLT::ResetElements(){
  //See header file for documentation
  
  cout << "destroy"<<endl;
  if(fTrackList) {
    fTrackList->Destroy();
    fTrackList = NULL;
    // RemoveElement(fTrackList);
    // TThread * destructor = new TThread(DestroyGarbage, (void*) this);
    // destructor->Run();
    // fTrackList = NULL;
  }
  
  if(fPointSetVertex) fPointSetVertex->Reset();
  cout<< "reset done"<<endl;
  fHistoCount = 0;

}

///_____________________________________________________________________________________
void * AliHLTEveHLT::DestroyGarbage(void * arg) {
  AliHLTEveHLT * hlt = reinterpret_cast<AliHLTEveHLT*>(arg);
  if(hlt) hlt->DestroyOldTrackList();
  return (void*)0;
}
///_____________________________________________________________________________________
void AliHLTEveHLT::DestroyOldTrackList() {
  cout << "Destroying the old tracklist's elements"<<endl;
  fOldTrackList->DestroyElements();
  cout << "Destroying the old tracklist itself"<<endl;
  fOldTrackList->Destroy();
}

///_____________________________________________________________________________________
void AliHLTEveHLT::ProcessHistograms(AliHLTHOMERBlockDesc * block, TCanvas * canvas) {
  //See header file for documentation
  if (!fTrCanvas) {
    fTrCanvas = CreateCanvas("TPC ESD QA", "TPC ESD QA");
    fTrCanvas->Divide(4, 2);
  }
  if(!fVertexCanvas) {
    fVertexCanvas = CreateCanvas("Vertex QA", "Vertex");
    fVertexCanvas->Divide(4, 2);
  }



  if ( ! block->GetClassName().CompareTo("TH1F")) {
    TH1F* histo = reinterpret_cast<TH1F*>(block->GetTObject());
    if( histo ){
      TString name(histo->GetName());
      cout << "TH1F " <<name << endl;
      if( !name.CompareTo("primVertexZ") ){
	canvas->cd(2);
	histo->Draw();
      }else if( !name.CompareTo("primVertexX") ){
	canvas->cd(3);
	histo->Draw();
      }else if( !name.CompareTo("primVertexY") ){
	canvas->cd(4);
	histo->Draw();
      } else if ( name.Contains("Track")) {
	AddHistogramToCanvas(histo, fTrCanvas, fTrCount);
      } else {
	AddHistogramToCanvas(histo, fVertexCanvas, fVCount);
      }
    }
  }  else if ( ! block->GetClassName().CompareTo("TH2F")) {
    TH2F *hista = reinterpret_cast<TH2F*>(block->GetTObject());
    if (hista ){
       TString name(hista->GetName());
       cout << "TH2F " << name << endl;
       if( !name.CompareTo("primVertexXY")) {      
	 canvas->cd(1);
	 hista->Draw();
       } else if ( name.Contains("Track")) {
	AddHistogramToCanvas(hista, fTrCanvas, fTrCount);
      } else {
	AddHistogramToCanvas(hista, fVertexCanvas, fVCount);
      }
    }
  }
  canvas->cd();
}

///_____________________________________________________________________________________
void AliHLTEveHLT::AddHistogramToCanvas(TH1* histogram, TCanvas * canvas, Int_t &cdCount) {
  canvas->cd(++cdCount);
  histogram->Draw();
}


///________________________________________________________________________________________
void AliHLTEveHLT::CreateTrackList() {
  //See header file for documentation
  fTrackList = new TEveTrackList("ESD Tracks");
  fTrackList->SetMainColor(6);
  AddElement(fTrackList);
}
// ///________________________________________________________________________________________
// void AliHLTEveHLT::CreateTrackList() {
//   //See header file for documentation
//   fTrackList = new TEveElementList("ESD tracks");
//   for (Int_t i = 0; i < 10; i++ ) {
//     TEveTrackList * trackList = new TEveTrackList(Form("Tracklist_%d", i));
//     trackList->SetMainColor(i);
//     fTrackList->AddElement(trackList);
//   }
  
//   AddElement(fTrackList);
// }

///_________________________________________________________________________________________
void AliHLTEveHLT::CreateVertexPointSet() {
  //See header file for documentation
  fPointSetVertex = new TEvePointSet("Primary Vertex");
  fPointSetVertex->SetMainColor(6);
  fPointSetVertex->SetMarkerStyle((Style_t)kFullStar);

  AddElement(fPointSetVertex);
}

///________________________________________________________________________
void AliHLTEveHLT::ProcessGlobalTrigger( AliHLTHOMERBlockDesc * block ) {
  //See header file for documentation
  AliHLTGlobalTriggerDecision * decision = dynamic_cast<AliHLTGlobalTriggerDecision*>(block->GetTObject());
  decision->Print();

}


///______________________________________________________________________
void AliHLTEveHLT::ProcessEsdBlock( AliHLTHOMERBlockDesc * block, TEveTrackList * cont ) {
  //See header file for documentation

  AliESDEvent* esd = (AliESDEvent *) (block->GetTObject());
  if (!esd) return;
  
  ProcessEsdEvent(esd, cont);
}
///___________________________________________________________________________________
void AliHLTEveHLT::ProcessEsdEvent(AliESDEvent * esd, TEveTrackList * cont) {

  esd->GetStdContent();

  cout << "ProcessESDEvent() :"<< esd->GetEventNumberInFile()<< "  " << esd->GetNumberOfCaloClusters() << " tracks : " << esd->GetNumberOfTracks() << endl;

  //fEventManager->SetRunNumber(esd->GetRunNumber());

  Double_t vertex[3];
  const AliESDVertex * esdVertex = esd->GetPrimaryVertex();
  
  if(esdVertex) {
    esdVertex->GetXYZ(vertex);
    fPointSetVertex->SetNextPoint(vertex[0], vertex[1], vertex[2]);
  }
  
  SetUpTrackPropagator(cont->GetPropagator(),-0.1*esd->GetMagneticField(), 520);

  for (Int_t iter = 0; iter < esd->GetNumberOfTracks(); ++iter) {
    AliEveTrack* track = dynamic_cast<AliEveTrack*>(MakeEsdTrack(esd->GetTrack(iter), cont));
    cont->AddElement(track);
   
    // fHistPt->Fill(esd->GetTrack(iter)->Pt());   // KK
    // fHistP->Fill(esd->GetTrack(iter)->P()*esd->GetTrack(iter)->Charge());
    // fHistEta->Fill(esd->GetTrack(iter)->Eta());
    // fHistTheta->Fill(esd->GetTrack(iter)->Theta()*TMath::RadToDeg());
    // fHistPhi->Fill(esd->GetTrack(iter)->Phi()*TMath::RadToDeg());
    // if(esd->GetTrack(iter)->GetStatus()&AliESDtrack::kTPCin || (esd->GetTrack(iter)->GetStatus()&AliESDtrack::kTPCin && esd->GetTrack(iter)->GetStatus()&AliESDtrack::kITSin)){
    //    fHistnClusters->Fill(esd->GetTrack(iter)->GetTPCNcls());  
    //}
  }
  
//fHistMult->Fill(esd->GetNumberOfTracks()); // KK
  
  
  cont->SetTitle(Form("N=%d", esd->GetNumberOfTracks()) );
  cont->MakeTracks();
  
}



void AliHLTEveHLT::DrawHistograms(){
  //See header file for documentation



}

AliEveTrack* AliHLTEveHLT::MakeEsdTrack (AliESDtrack *at, TEveTrackList* cont) {
  //See header file for documentation


  const double kCLight = 0.000299792458;
  double bz = - kCLight*10.*( cont->GetPropagator()->GetMagField(0,0,0).fZ);

  Bool_t innerTaken = kFALSE;
  if ( ! at->IsOn(AliESDtrack::kITSrefit) && fUseIpOnFailedITS)
  {
    //tp = at->GetInnerParam();
    innerTaken = kTRUE;
  }

  // Add inner/outer track parameters as path-marks.

  Double_t     pbuf[3], vbuf[3];

  AliExternalTrackParam trackParam = *at;

  // take parameters constrained to vertex (if they are)

  if( at->GetConstrainedParam() ){
    trackParam = *at->GetConstrainedParam();
  }
  else if( at->GetInnerParam() ){
    trackParam = *(at->GetInnerParam());
  }
  if( at->GetStatus()&AliESDtrack::kTRDin ){
    // transport to TRD in
    trackParam = *at;
    trackParam.PropagateTo( 290.45, -10.*( cont->GetPropagator()->GetMagField(0,0,0).fZ) );
  }

  TEveRecTrack rt;
  {
    rt.fLabel  = at->GetLabel();
    rt.fIndex  = (Int_t) at->GetID();
    rt.fStatus = (Int_t) at->GetStatus();
    rt.fSign   = (Int_t) trackParam.GetSign();  
    trackParam.GetXYZ(vbuf);
    trackParam.GetPxPyPz(pbuf);    
    rt.fV.Set(vbuf);
    rt.fP.Set(pbuf);
    Double_t ep = at->GetP(), mc = at->GetMass();
    rt.fBeta = ep/TMath::Sqrt(ep*ep + mc*mc);
  }

  AliEveTrack* track = new AliEveTrack(&rt, cont->GetPropagator());
  track->SetAttLineAttMarker(cont);
  track->SetName(Form("AliEveTrack %d", at->GetID()));
  track->SetElementTitle(CreateTrackTitle(at));
  track->SetSourceObject(at);


  // Set reference points along the trajectory
  // and the last point

  { 
    TEvePathMark startPoint(TEvePathMark::kReference);
    trackParam.GetXYZ(vbuf);
    trackParam.GetPxPyPz(pbuf);    
    startPoint.fV.Set(vbuf);
    startPoint.fP.Set(pbuf);
    rt.fV.Set(vbuf);
    rt.fP.Set(pbuf);
    Double_t ep = at->GetP(), mc = at->GetMass();
    rt.fBeta = ep/TMath::Sqrt(ep*ep + mc*mc);

    track->AddPathMark( startPoint );    
  }


  if( at->GetOuterParam() && at->GetTPCPoints(2)>80 ){
  
    //
    // use AliHLTTPCCATrackParam propagator 
    // since AliExternalTrackParam:PropagateTo()
    // has an offset at big distances
    //
    double rot = at->GetOuterParam()->GetAlpha() - trackParam.GetAlpha();
    double crot = cos(rot), srot = sin(rot);
    double xEnd = at->GetTPCPoints(2)*crot +  at->GetTPCPoints(3)*srot;
  // take parameters constrained to vertex (if they are)
 

    AliHLTTPCCATrackParam t;
    AliHLTTPCCATrackConvertor::SetExtParam( t, trackParam );
    
    Double_t x0 = trackParam.GetX();
    Double_t dx = xEnd - x0;
    
    //
    // set a reference at the half of trajectory for better drawing
    //
    
    for( double dxx=dx/2; TMath::Abs(dxx)>=1.; dxx*=.9 ){
      if( !t.TransportToX(x0+dxx, bz, .999 ) ) continue;
      AliExternalTrackParam tt;
      AliHLTTPCCATrackConvertor::GetExtParam( t, tt, trackParam.GetAlpha() ); 
      tt.GetXYZ(vbuf);
      tt.GetPxPyPz(pbuf);
      TEvePathMark midPoint(TEvePathMark::kReference);
      midPoint.fV.Set(vbuf);
      midPoint.fP.Set(pbuf);    
      track->AddPathMark( midPoint );
      break;
    }
 
   
    //
    // Set a reference at the end of the trajectory
    // and a "decay point", to let the event display know where the track ends
    //
    
    for( ; 1; dx*=.9 ){
      if( !t.TransportToX(x0+dx, bz, .999 ) ){
	if( TMath::Abs(dx)<1. ) break;
	continue;
      }
       AliExternalTrackParam tt;
      AliHLTTPCCATrackConvertor::GetExtParam( t, tt, trackParam.GetAlpha() ); 
      tt.GetXYZ(vbuf);
      tt.GetPxPyPz(pbuf);
      TEvePathMark endPoint(TEvePathMark::kReference);
      TEvePathMark decPoint(TEvePathMark::kDecay);
      endPoint.fV.Set(vbuf);
      endPoint.fP.Set(pbuf);
      decPoint.fV.Set(vbuf);
      decPoint.fP.Set(pbuf);
      track->AddPathMark( endPoint );
      track->AddPathMark( decPoint );      
      break;
    }  
  }

  if (at->IsOn(AliESDtrack::kTPCrefit))
  {
    if ( ! innerTaken)
    {
      AddTrackParamToTrack(track, at->GetInnerParam());
    }
    AddTrackParamToTrack(track, at->GetOuterParam());
  }
  return track;
}

void AliHLTEveHLT::SetUpTrackPropagator(TEveTrackPropagator* trkProp, Float_t magF, Float_t maxR) {
  //See header file for documentation

  if (fTrueField) {
    trkProp->SetMagFieldObj(new AliEveMagField);
  
  } else {
    trkProp->SetMagField(magF);
  }
 
  if (fUseRkStepper) {
    trkProp->SetStepper(TEveTrackPropagator::kRungeKutta);
  }

  trkProp->SetMaxR(maxR);
}


void AliHLTEveHLT::AddTrackParamToTrack(AliEveTrack* track, const AliExternalTrackParam* tp) {
  //See header file for documentation

  if (tp == 0)
    return;

  Double_t pbuf[3], vbuf[3];
  tp->GetXYZ(vbuf);
  tp->GetPxPyPz(pbuf);

  TEvePathMark pm(TEvePathMark::kReference);
  pm.fV.Set(vbuf);
  pm.fP.Set(pbuf);
  track->AddPathMark(pm);
}



TString AliHLTEveHLT::CreateTrackTitle(AliESDtrack* t) {
  // Add additional track parameters as a path-mark to track.

  TString s;

  Int_t label = t->GetLabel(), index = t->GetID();
  TString idx(index == kMinInt ? "<undef>" : Form("%d", index));
  TString lbl(label == kMinInt ? "<undef>" : Form("%d", label));

  Double_t p[3], v[3];
  t->GetXYZ(v);
  t->GetPxPyPz(p);
  Double_t pt    = t->Pt();
  Double_t ptsig = TMath::Sqrt(t->GetSigma1Pt2());
  Double_t ptsq  = pt*pt;
  Double_t ptm   = pt / (1.0 + pt*ptsig);
  Double_t ptM   = pt / (1.0 - pt*ptsig);

  s = Form("Index=%s, Label=%s\nChg=%d, Pdg=%d\n"
	   "pT = %.3f + %.3f - %.3f [%.3f]\n"
           "P  = (%.3f, %.3f, %.3f)\n"
           "V  = (%.3f, %.3f, %.3f)\n",
	   idx.Data(), lbl.Data(), t->Charge(), 0,
	   pt, ptM - pt, pt - ptm, ptsig*ptsq,
           p[0], p[1], p[2],
           v[0], v[1], v[2]);

  Int_t   o;
  s += "Det (in,out,refit,pid):\n";
  o  = AliESDtrack::kITSin;
  s += Form("ITS (%d,%d,%d,%d)  ",  t->IsOn(o), t->IsOn(o<<1), t->IsOn(o<<2), t->IsOn(o<<3));
  o  = AliESDtrack::kTPCin;
  s += Form("TPC(%d,%d,%d,%d)\n",   t->IsOn(o), t->IsOn(o<<1), t->IsOn(o<<2), t->IsOn(o<<3));
  o  = AliESDtrack::kTRDin;
  s += Form("TRD(%d,%d,%d,%d) ",    t->IsOn(o), t->IsOn(o<<1), t->IsOn(o<<2), t->IsOn(o<<3));
  o  = AliESDtrack::kTOFin;
  s += Form("TOF(%d,%d,%d,%d)\n",   t->IsOn(o), t->IsOn(o<<1), t->IsOn(o<<2), t->IsOn(o<<3));
  o  = AliESDtrack::kHMPIDout;
  s += Form("HMPID(out=%d,pid=%d)\n", t->IsOn(o), t->IsOn(o<<1));
  s += Form("ESD pid=%d", t->IsOn(AliESDtrack::kESDpid));

  if (t->IsOn(AliESDtrack::kESDpid))
  {
    Double_t pid[5];
    t->GetESDpid(pid);
    s += Form("\n[%.2f %.2f %.2f %.2f %.2f]", pid[0], pid[1], pid[2], pid[3], pid[4]);
  }

  return s;
}



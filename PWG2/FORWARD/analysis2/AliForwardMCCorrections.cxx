#include "AliForwardMCCorrections.h"
#include "AliTriggerAnalysis.h"
#include "AliPhysicsSelection.h"
#include "AliLog.h"
#include "AliFMDAnaParameters.h"
#include "AliESDEvent.h"
#include "AliAODHandler.h"
#include "AliMultiplicity.h"
#include "AliInputEventHandler.h"
#include <TH1.h>
#include <TDirectory.h>
#include <TTree.h>

//====================================================================
namespace {
  const char* GetEventName(Bool_t tr, Bool_t vtx) 
  {
    return Form("nEvents%s%s", (tr ? "Tr" : ""), (vtx ? "Vtx", ""));
  }
  const char* GetHitsName(UShort_t d, Char_t r) 
  {
    return Form("hitsFMD%d%c", d, r);
  }
  const char* GetStripsName(UShort_t d, Char_t r)
  {
    return Form("stripsFMD%d%c", d, r);
  }
  const char* GetPrimaryName(Char_t r, Bool_t trVtx)
  {
    return Form("primaries%s%s", 
		((r == 'I' || r == 'i') ? "Inner" : "Outer"), 
		(trVtx ? "TrVtx" : "All"));
  }
}

//====================================================================
AliForwardMCCorrections::AliForwardMCCorrections()
  : AliAnalysisTaskSE(),
    fHEventsTr(0), 
    fHEventsTrVtx(0),
    fHTriggers(0),
    fVtxAxis(),
    fEtaAxis()
{
}

//____________________________________________________________________
AliForwardMCCorrections::AliForwardMCCorrections(const char* name)
  : AliAnalysisTaskSE(name), 
    fHEventsTr(0), 
    fHEventsTrVtx(0), 
    fHTriggers(0),
    fVtxAxis(10,-10,10), 
    fEtaAxis(200,-4,6)
{
  DefineOutput(1, TList::Class());
}

//____________________________________________________________________
AliForwardMCCorrections::AliForwardMCCorrections(const AliForwardMCCorrections& o)
  : AliAnalysisTaskSE(o),
    fHEventsTr(o.fHEventsTr), 
    fHEventsTrVtx(o.fHEventsTrVtx), 
    fHTriggers(o.fHTriggers),
    fVtxAxis(o.fVtxAxis),
    fEtaAxis(o.fEtaAxis)
{
}

//____________________________________________________________________
AliForwardMCCorrections&
AliForwardMCCorrections::operator=(const AliForwardMCCorrections& o)
{
  fHEventsTr         = o.fHEventsTr;
  fHEventsTrVtx      = o.fHEventsTrVtx;
  fHTriggers         = o.fHTriggers;
  fHData             = o.fHData;
  fVtxAxis           = o.fVtxAxis;
  fEtaAxis           = o.fEtaAxis;

  return *this;
}

//____________________________________________________________________
void
AliForwardMCCorrections::Init()
{
}

//____________________________________________________________________
void
AliForwardMCCorrections::SetVertexAxis(Int_t nBin, Double_t min, Double_t max)
{
  if (max < min) max = -min;
  if (min < max) { 
    Double_t tmp = min;
    min          = max;
    max          = tmp;
  }
  if (min < -10) 
    AliWarning(Form("Minimum vertex %f < -10, make sure you want this",min));
  if (max > +10) 
    AliWarning(Form("Minimum vertex %f > +10, make sure you want this",max));
  fVtxAxis.Set(nBin, min, max);
}
//____________________________________________________________________
void
AliForwardMCCorrections::SetVertexAxis(const TAxis& axis)
{
  SetVertexAxis(axis.GetNbins(),axis.GetXmin(),axis.GetXmax());
}

//____________________________________________________________________
void
AliForwardMCCorrections::SetEtaAxis(Int_t nBin, Double_t min, Double_t max)
{
  if (max < min) max = -min;
  if (min < max) { 
    Double_t tmp = min;
    min          = max;
    max          = tmp;
  }
  if (min < -4) 
    AliWarning(Form("Minimum eta %f < -4, make sure you want this",min));
  if (max > +6) 
    AliWarning(Form("Minimum eta %f > +6, make sure you want this",max));
  fVtxAxis.Set(nBin, min, max);
}
//____________________________________________________________________
void
AliForwardMCCorrections::SetVertexAxis(const TAxis& axis)
{
  SetEtaAxis(axis.GetNbins(),axis.GetXmin(),axis.GetXmax());
}
  
//____________________________________________________________________
void
AliForwardMCCorrections::InitializeSubs()
{
}

//____________________________________________________________________
TH3D*
AliForwardMCCorrections::Make3D(const char* name, const char* title,
			       Int_t nPhi) const
{
  TH3D* ret = new TH3D(name, title,
		       fVtxAxis.GetNbins(), 
		       fVtxAxis.GetXmin(), 
		       fVtxAxis.GetXmax(), 
		       fEtaAxis.GetNbins(), 
		       fEtaAxis.GetXmin(), 
		       fEtaAxis.GetXmax(), 
		       nPhi, 0, 2*TMath::Pi());
  ret->SetXTitle("v_{z} [cm]");
  ret->SetYTitle("#eta");
  ret->SetZTitle("#varphi [radians]");
  ret->SetDirectory(0);
  ret->SetStats(0);
  ret->Sumw2();

  return ret;
}
//____________________________________________________________________
TH1D*
AliForwardMCCorrections::Make1D(const char* name, const char* title) const
{
  TH1D* ret = new TH1D(name, title,
		       fEtaAxis.GetNbins(), 
		       fEtaAxis.GetXmin(), 
		       fEtaAxis.GetXmax());
  ret->SetXTitle("#eta");
  ret->SetFillColor(kRed+1);
  ret->SetFillStyle(3001);
  ret->SetDirectory(0);
  ret->SetStats(0);
  ret->Sumw2();

  return ret;
}

//____________________________________________________________________
void
AliForwardMCCorrections::UserCreateOutputObjects()
{
  fList = new TList;
  fList->SetName(GetName());

  fHEvents = new TH1I(GetEventsName(false,false),
		      "Number of all events", 
		      fAxis.GetNbins(), 
		      fAxis.GetXmin(), 
		      fAxis.GetXmax());
  fHEvents->SetXTitle("v_{z} [cm]");
  fHEvents->SetYTitle("# of events");
  fHEvents->SetFillColor(kBlue+1);
  fHEvents->SetFillStyle(3001);
  fHEvents->SetDirectory(0);
  fList->Add(fHEvents);

  fHEventsTr = new TH1I(GetEventsName(true, false), 
			fVtxAxis.GetNbins(), 
			fVtxAxis.GetXmin(), 
			fVtxAxis.GetXmax());
  fHEventsTr->SetXTitle("v_{z} [cm]");
  fHEventsTr->SetYTitle("# of events");
  fHEventsTr->SetFillColor(kRed+1);
  fHEventsTr->SetFillStyle(3001);
  fHEventsTr->SetDirectory(0);
  fList->Add(fHEventsTr);

  fHEventsTrVtx = new TH1I(GetEventsName(true,true),
			   "Number of events w/trigger and vertex", 
			   fVtxAxis.GetNbins(), 
			   fVtxAxis.GetXmin(), 
			   fVtxAxis.GetXmax());
  fHEventsTrVtx->SetXTitle("v_{z} [cm]");
  fHEventsTrVtx->SetYTitle("# of events");
  fHEventsTrVtx->SetFillColor(kBlue+1);
  fHEventsTrVtx->SetFillStyle(3001);
  fHEventsTrVtx->SetDirectory(0);
  fList->Add(fHEventsTrVtx);

  fHEventsVtx = new TH1I(GetEventsName(false,true),
			 "Number of events w/vertex", 
			 fVtxAxis.GetNbins(), 
			 fVtxAxis.GetXmin(), 
			 fVtxAxis.GetXmax());
  fHEventsVtx->SetXTitle("v_{z} [cm]");
  fHEventsVtx->SetYTitle("# of events");
  fHEventsVtx->SetFillColor(kBlue+1);
  fHEventsVtx->SetFillStyle(3001);
  fHEventsVtx->SetDirectory(0);
  fList->Add(fHEventsVtx);

      
  fHTriggers = new TH1I("triggers", "Triggers", 10, 0, 10);
  fHTriggers->SetFillColor(kRed+1);
  fHTriggers->SetFillStyle(3001);
  fHTriggers->SetStats(0);
  fHTriggers->SetDirectory(0);
  fHTriggers->GetXaxis()->SetBinLabel(1,"INEL");
  fHTriggers->GetXaxis()->SetBinLabel(2,"INEL>0");
  fHTriggers->GetXaxis()->SetBinLabel(3,"NSD");
  fHTriggers->GetXaxis()->SetBinLabel(4,"Empty");
  fHTriggers->GetXaxis()->SetBinLabel(5,"A");
  fHTriggers->GetXaxis()->SetBinLabel(6,"B");
  fHTriggers->GetXaxis()->SetBinLabel(7,"C");
  fHTriggers->GetXaxis()->SetBinLabel(8,"E");
  fList->Add(fHTriggers);

  fPrimaryInnerAll   = Make3D(GetPrimaryName('I',false), "Primary particles, "
			      "20 #varphi bins, all events", 20);
  fPrimaryOuterAll   = Make3D(GetPrimaryName('O',false), "Primary particles, "
			      "40 #varphi bins, all events", 40);
  fPrimaryInnerTrVtx = Make3D(GetPrimaryName('I',true), "Primary particles, "
			      "20 #varphi bins, Tr+Vtx events", 20);
  fPrimaryOuterTrVtx = Make3D(GetPrimaryName('O',true), "Primary particles, "
			      "40 #varphi bins, Tr+Vtx events", 40);
  TList* primaries = new TList;
  primaries->SetName("primaries");
  primaries->Add(fPrimaryInnerAll);
  primaries->Add(fPrimaryOuterAll);
  primaries->Add(fPrimaryInnerTrVtx);
  primaries->Add(fPrimaryOuterTrVtx);
  fList->Add(primaries);


  fHitsFMD1i   = Make3D(GetHitsName(1,'i'),   "All hits in FMD1i, Tr+Vtx", 20);
  fHitsFMD2i   = Make3D(GetHitsName(2,'i'),   "All hits in FMD2i, Tr+Vtx", 20);
  fHitsFMD2o   = Make3D(GetHitsName(2,'o'),   "All hits in FMD2o, Tr+Vtx", 40);
  fHitsFMD3i   = Make3D(GetHitsName(3,'i'),   "All hits in FMD3i, Tr+Vtx", 20);
  fHitsFMD3o   = Make3D(GetHitsName(3,'o'),   "All hits in FMD3o, Tr+Vtx", 40);
  TList* hits = new TList;
  hits->SetName("hits");
  hits->Add(fHitsFMD1i);
  hits->Add(fHitsFMD2i);
  hits->Add(fHitsFMD2o);
  hits->Add(fHitsFMD3i);
  hits->Add(fHitsFMD3o);
  fList->Add(hits);

  fStripsFMD1i = Make1D(GetStripsName(1,'i'), "# hit strips in FMD1i, Tr+Vtx");
  fStripsFMD2i = Make1D(GetStripsName(2,'i'), "# hit strips in FMD2i, Tr+Vtx");
  fStripsFMD2o = Make1D(GetStripsName(2,'o'), "# hit strips in FMD2o, Tr+Vtx");
  fStripsFMD3i = Make1D(GetStripsName(3,'i'), "# hit strips in FMD3i, Tr+Vtx");
  fStripsFMD3o = Make1D(GetStripsName(3,'o'), "# hit strips in FMD3o, Tr+Vtx");
  TList* strips = new TList;
  strips->SetName("strips");
  strips->Add(fStripsFMD1i);
  strips->Add(fStripsFMD2i);
  strips->Add(fStripsFMD2o);
  strips->Add(fStripsFMD3i);
  strips->Add(fStripsFMD3o);
  fList->Add(strips);

  PostData(1, fList);
}
//____________________________________________________________________
void
AliForwardMCCorrections::UserExec(Option_t*)
{
  // Get the input data - MC event
  AliMCEvent*  mcEvent = MCEvent();
  if (mcevent) { 
    AliWarning("No MC event found");
    return;
  }

  // Get the input data - ESD event
  AliESDEvent* esd = dynamic_cast<AliESDEvent*>(InputEvent());
  if (!esd) { 
    AliWarning("No ESD event found for input event");
    return;
  }
  
  // Get the particle stack 
  AliStack* stack = mcEvent->Stack();

  // Get the event generate header 
  AliHeader*          mcHeader  = mcEvent->Header();
  AliGenEventHeader*  genHeader = mcHeader->GetCenEventHeader();
  
  // Get the generator vertex 
  TArrayF mcVertex;
  genHeader->PrimaryVertex();
  Double_t mcVtxZ = mcVertex.At(2);

  // Check the MC vertex is in range 
  Int_t mcVtxBin = fVtxAxis.FindBin(vZ);
  if (vZ < 1 || vZ > fVtxAxis.GetNbins()) {
#ifdef VERBOSE 
    AliWarning(Form("MC v_z=%f is out of rante [%,%f]", 
		    vZ, fVtxAxis.GetXmin(), fVtxAxis.GetXmax()));
#endif
    return;
  }

  // Get the triggers 
  UInt_t triggers = 0;
  Bool_t gotTriggers = AliForwardUtil::ReadTriggers(esd,triggers,fHTriggers);
  Bool_t gotInel     = triggers & AliAODForwardMult::kInel;
  
  // Get the ESD vertex 
  Double_t vZ = -1000000;
  Bool_t gotVertex = AliForwardUtil::ReadVertex(esd,vZ);
  

  // Fill the event count histograms 
  if (gotInel)              fHEventsTr->Fill(mcVtxZ);
  if (gotInel && gotVertex) fHEventsTrVtx->Fill(mcVtxZ);
  if (gotVertex)            fHEventsVtx->Fill(mcVtxZ);
  fHEvents->Fill(mcVtxZ);

  // Cache of the hits 
  AliFMDFloatMap hitsByStrip;
  AliFMDFloatMap lastByStrip;

  // Loop over all tracks 
  Int_t nTracks = mcEvent->GetNumberOfTracks();
  for (Int_t iTr = 0; iTr < nTracks; iTr++) { 
    AliMCParticle* particle = 
      static_cast<AliMCParticle*>(mcEvent->GetTrack(iTr));
    
    // Check the returned particle 
    if (!particle) continue;

    // Check if this charged and a primary 
    Bool_t isCharged = particle->Charge() != 0;
    Bool_t isPrimary = stack->IsPhysicalPrimary(iTr);

    // Fill (eta,phi) of the particle into histograsm for b
    Double_t eta = particle->Eta();
    Double_t phi = partcile->Phi();
    
    if (isCharged && isPrimary) 
      FillPrimary(gotInel, gotVertex, mcVtxZ, eta, phi);
    
    // For the rest, ignore non-collisions, and events out of vtx range 
    if (!gotInel || gotVtx) continue;
    
    Int_t nTrRef = particle->GetNumberOfTrackReferences();
    for (Int_t iTrRef = 0; iTrRef < nTrRef; iTrRef++) { 
      AliTrackReference* trackRef = particle->GetTrackReference(iTrRef);
      
      // Check existence 
      if (!ret) continue;

      // Check that we hit an FMD element 
      if (ref->DetectorId() != AliTrackReference::kFMD) 
	continue;

      // Get the detector coordinates 
      UShort_t d, s, t;
      Char_t r;
      AliFMDStripIndex::Unpack(ref->UserId(), d, r, s, t);
      
      // Check if mother (?) is charged and that we haven't dealt with it 
      // already
      Int_t lastTrack = Int_t(lastByStrip(d,r,s,t));
      if (!isCharged || iTr == lastTrack) continue;

      // Increase counter of hits per strip 
      hitsByStrip(d,r,s,t) += 1;

      Double_t trRefEta = esd->GetFMDData()->Eta(d,r,s,t);
      Double_t trRefPhi = esd->GetFMDData()->Phi(d,r,s,t);

      // Fill strip information into histograms 
      FillStrip(mcVtxZ, eta, phi, hitsByStrip(d,r,s,t) == 1);

      // Set the last processed track number - marking it as done for
      // this strip
      lastByStrip(d,r,s,t) = Float_t(iTr);
      
      // Flag neighboring strips too 
      Int_t nStrip = (r == 'I' || r == 'i' ? 512 : 256);
      if (t > 0)        lastByStrip(d,r,s,t-1) = Float_t(iTr);
      if (t < nStrip-1) lastByStrip(d,r,s,t+1) = Float_t(iTr);
    }
  }
}

//____________________________________________________________________
void
AliForwardMCCorrections::FillPrimary(Bool_t gotInel, Bool_t gotVtx, 
				    Double_t vz, Double_t eta, Double_t phi) 
{
  if (gotInel && gotVtx) {
    // This takes the place of hPrimary_FMD_<r>_vtx<v> and 
    // Analysed_FMD<r>_vtx<v>
    fPrimaryInnerTrVtx->Fill(vz,eta,phi);
    fPrimaryOuterTrVtx->Fill(vz,eta,phi);
  }
  // This takes the place of Inel_FMD<r>_vtx<v> and 
  // Analysed_FMD<r>_vtx<v>
  fPrimaryInnerAll->Fill(vz,eta,phi);
  fPrimaryOuterAll->Fill(vz,eta,phi);
}

//____________________________________________________________________
void
AliForwardMCCorrections::FillStrip(UShort_t d, Char_t r, 
				  Double_t vz, Double_t eta, Double_t phi,
				  Bool_t first) 
{
  // Number of hit strips per eta bin 
  TH1D* hits   = 0; 
  // All hits per ring, vertex in eta,phi bins.  This takes the place
  // of hHits_FMD<d><r>_vtx<v> and DoubleHits_FMD<d><r> (the later
  // being just the 2D projection over the X bins)
  TH3D* strips = 0; 
  switch (d) { 
  case 1: 
    hits   = fHitsFMD1i; 
    strips = fStripsFMD1i; 
    break;
  case 2: 
    hits   = (r == 'I' || r == 'i' ? fHitsFMD2i   : fHitsFMD2o);
    strips = (r == 'I' || r == 'i' ? fStripsFMD2i : fStripsFMD2o);
    break;
  case 3: 
    hits   = (r == 'I' || r == 'i' ? fHitsFMD3i   : fHitsFMD3o);
    strips = (r == 'I' || r == 'i' ? fStripsFMD3i : fStripsFMD3o);
    break;
  }
  if (!hits || !strips) return;
  
  if (first) strips->Fill(eta);
  
  hits->Fill(vz, eta, phi);
}

//____________________________________________________________________
TH2D*
AliForwardMCCorrections::GetVertexProj(Int_t v, TH3D* src) const
{
  Int_t nX = src->GetNbinsX();
  if (v > nX || v < 1) return 0;

  src->GetXaxis()->SetRange(v,v+1);
  
  TH2D* ret   = static_cast<TH2D*>(src->Project3D("zy"));
  ret->SetName(Form("%s_vtx%02d", src->GetName(), v));
  ret->SetDirectory(0);

  src->GetXaxis()->SetRange(0,nX+1);

  return ret;
}


//____________________________________________________________________
void
AliForwardMCCorrections::Terminate(Option_t*)
{
  TList* list = dynamic_cast<TList*>(GetOutputData(1));
  if (!list) {
    AliError("No output list defined");
    return;
  }

  TList* primaries = static_cast<TList*>(list->FindObject("primaries"));
  TList* hits      = static_cast<TList*>(list->FindObject("hits"));
  TList* strips    = static_cast<TList*>(list->FindObject("strips"));
  if (!primaries || !hits || !strips) { 
    AliError(Form("Could not find all sub-lists in %s (p=%p,h=%p,s=%p)",
		  list->GetName(), primaries, hits, strips));
    return;
  }

  TH1I* eventsAll = 
    static_cast<TH1I*>(list->FindObject(GetEventsName(false,false)));
  TH1I* eventsTr = 
    static_cast<TH1I*>(list->FindObject(GetEventsName(true,false)));
  TH1I* eventsVtx = 
    static_cast<TH1I*>(list->FindObject(GetEventsName(false,true)));
  TH1I* eventsTrVtx = 
    static_cast<TH1I*>(list->FindObject(GetEventsName(true,true)));
  if (!eventsAll || !eventsTr || !eventsVtx || !eventsTrVtx) {
    AliError(Form("Could not find all event histograms in %s "
		  "(a=%p,t=%p,v=%p,tv=%p)", list->GetName(), 
		  eventsAll, eventsTr, eventsVtx, eventsTrVtx));
    return;
  }

  TH3D* primIAll = 
    static_cast<TH3D*>(primaries->FindObject(GetPrimaryName('I',false)));
  TH3D* primOAll = 
    static_cast<TH3D*>(primaries->FindObject(GetPrimaryName('O',false)));
  TH3D* primITrVtx = 
    static_cast<TH3D*>(primaries->FindObject(GetPrimaryName('I',true)));
  TH3D* primOTrVtx = 
    static_cast<TH3D*>(primaries->FindObject(GetPrimaryName('O',true)));
  if (!primIAll || !primOAll || !primITrVtx || !primOTrVtx) {
    AliError(Form("Could not find all primary particle histograms in %s "
		  "(ai=%p,ao=%p,tvi=%p,tvo=%p)", list->GetName(), 
		  primIAll, primOAll, primITrVtx, primOTrVtx));
    return;
  }
    
  TH3D* hits1i = static_cast<TH3D*>(hits->FindObject(GetHitsName(1,'i')));
  TH3D* hits2i = static_cast<TH3D*>(hits->FindObject(GetHitsName(2,'i')));
  TH3D* hits2o = static_cast<TH3D*>(hits->FindObject(GetHitsName(2,'o')));
  TH3D* hits3i = static_cast<TH3D*>(hits->FindObject(GetHitsName(3,'i')));
  TH3D* hits3o = static_cast<TH3D*>(hits->FindObject(GetHitsName(3,'o')));
  if (!hits1i || !hits2i || !hits2o || hits3i || hits3o) {
    AliError(Form("Could not find all ring hits histograms in %s " 
		  "(1i=%p,2i=%p,2o=%p,3i=%p,3o=%p)", hits->GetName(), 
		  hits1i, hits2i, hits2o, hits3i, hits3o));
    return;
  }

  TH1D* strips1i = static_cast<TH1D*>(strips->FindObject(GetStripsName(1,'i')));
  TH1D* strips2i = static_cast<TH1D*>(strips->FindObject(GetStripsName(2,'i')));
  TH1D* strips2o = static_cast<TH1D*>(strips->FindObject(GetStripsName(2,'o')));
  TH1D* strips3i = static_cast<TH1D*>(strips->FindObject(GetStripsName(3,'i')));
  TH1D* strips3o = static_cast<TH1D*>(strips->FindObject(GetStripsName(3,'o')));
  if (!strips1i || !strips2i || !strips2o || strips3i || strips3o) {
    AliError(Form("Could not find all ring strips histograms in %s " 
		  "(1i=%p,2i=%p,2o=%p,3i=%p,3o=%p)", strips->GetName(), 
		  strips1i, strips2i, strips2o, strips3i, strips3o));
    return;
  }

  // Calculate the over-all event selection efficiency 
  TH1D* selEff = new TH1D("selEff", "Event selection efficiency", 
			  fVtxArray.GetNbins(), 
			  fVtxArray.GetXmin(),  
			  fVtxArray.GetXmax());
  selEff->Sumw2();
  selEff->SetDirectory(0);
  selEff->SetFillColor(kMagenta+1);
  selEff->SetFillStyle(3001);
  selEff->Add(eventsAll);
  selEff->Divide(eventsTrVtx);
  list->Add(selEff);

  // Loop over vertex bins and do vertex dependendt stuff 
  for (Int_t v = 1; v <= fVtxAxis.GetNbins(); v++) {
    // Make a sub-list in the output 
    TList* vl = new TList;
    vl->SetName("vtx%02d", v);
    list->Add(vl);

    // Get event counts 
    Int_t nEventsAll   = eventsAll->GetEntries(v);
    Int_t nEventsTr    = eventsTr->GetEntries(v);
    Int_t nEventsVtx   = eventsVtx->GetEntries(v);
    Int_t nEventsTrVtx = eventsTrVtx->GetEntries(v);

    // Project event histograms, set names, and store  
    TH2D* primIAllV   = GetVertexProj(v, primIAll);
    TH2D* primOAllV   = GetVertexProj(v, primOAll);
    TH2D* primITrVtxV = GetVertexProj(v, primITrVtx);
    TH2D* primOTrVtxV = GetVertexProj(v, primOTrVtx);
    vl->Add(primIAllV);
    vl->Add(primOAllV);
    vl->Add(primITrVtxV);
    vl->Add(primOTrVtxV);
    
    primIAllV->Scale(1. / nEventAll);
    primOAllV->Scale(1. / nEventAll);
    primITrVtxV->Scale(1. / nEventTr);
    primOTrVtxV->Scale(1. / nEventTr);

    // Calculate the vertex bias on the d^2N/detadphi 
    TH2D* selBiasI = static_cast<TH2D*>(primITrVtxV->Clone("selBiasI%02d",v));
    TH2D* selBiasO = static_cast<TH2D*>(primOTrVtxV->Clone("selBiasO%02d",v));
    selBiasI->SetTitle(Form("Event selection bias in vertex bin %d", v));
    selBiasO->SetTitle(Form("Event selection bias in vertex bin %d", v));
    selBiasI->Divide(primIAllV);
    selBiasO->Divide(primOAllV);
    vl->Add(selBiasI);
    vl->Add(selBiasO);

    for (UShort_t d = 1; d <= 3; d++) { 
      UShort_t nQ = (d == 1 ? 1 : 2);
      for (UShort_t q = 0; q < nQ; q++) { 
	Char_t   r  = (q == 0 ? 'I' : 'O');
	TH3D* hits3D = 0;
	TH2D* prim2D = (q == 0 ? primITrVtxV : primOTrVtxV);
	switch (d) { 
	case 1: hits3D = hits1i; break;
	case 2: hits3D = (q == 0 ? hits2i : hits2o); break;
	case 3: hits3D = (q == 0 ? hits3i : hits3o); break;
	}

	TH2D* sec = GetVertexProj(v, hits3D);
	sec->SetName(Form("secondaryFMD%d%c_vtx%02d", d, r, v));
	sec->SetTitle(Form("Secondary correction map for FMD%d%c "
			   "in vertex bin %d", d, r, v));
	sec->Divide(prim2D);
	vl->Add(sec);

	if (v > 1) continue;
	
	// Do the double hit correction (only done once per ring in
	// the vertex loop)
	TH1D* strips = 0;
	switch (d) { 
	case 1: strips = strips1i; break;
	case 2: strips = (q == 0 ? strips2i : strips2o); break;
	case 3: strips = (q == 0 ? strips3i : strips3o); break;
	}

	TH2D* hits2D    = GetVertexProj(v, hits3D);
	TH1D* doubleHit = hits2D->ProjectionX(Form("doubleHitFMD%d%c",d,r));
	doubleHit->SetTitle(Form("Double hit correction for FMD%d%c",d,r));
	doubleHit->SetDirectory(0);
	doubleHit->SetFillColor(kGreen+1);
	doubleHit->SetFillStyle(3001);
	doubleHit->Sumw2();
	doubleHit->Divide(strips);
	list->Add(doubleHit);
      }
    }
  }
}

//____________________________________________________________________
void
AliForwardMCCorrections::Print(Option_t*) const
{
}

//
// EOF
//

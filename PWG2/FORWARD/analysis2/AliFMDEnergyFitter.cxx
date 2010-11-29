//
// Class to do the energy correction of FMD ESD data
//
#include "AliFMDEnergyFitter.h"
#include <AliESDFMD.h>
#include <TAxis.h>
#include <TList.h>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include "AliFMDAnaParameters.h"
#include <AliLog.h>
#include <TClonesArray.h>
#include <TFitResult.h>
#include <THStack.h>

ClassImp(AliFMDEnergyFitter)
#if 0
; // This is for Emacs
#endif 

#define N_A(N)  (4+N-2)
#define N2_A(N) (4+(N-2)*3)
#define N2_D(N) (4+(N-2)*3+1)
#define N2_X(N) (4+(N-2)*3+2)

//____________________________________________________________________
namespace {
  Double_t 
  NLandau(Double_t* xp, Double_t* pp) 
  {
    Double_t  e        = xp[0];
    Double_t  constant = pp[0];
    Double_t  mpv      = pp[1];
    Double_t  fwhm     = pp[2];
    Int_t     n        = Int_t(pp[3]);
    Double_t  result   = 0;
    for (Int_t i = 1; i <= n; i++) {
      Double_t mpvi  =  i*(mpv+fwhm*TMath::Log(i));
      Double_t fwhmi =  i*fwhm;
      Double_t ai    =  (i == 1 ? 1 : pp[N_A(i)]);
      result         += ai * TMath::Landau(e,mpvi,fwhmi,kTRUE);
    }
    result *= constant;
    return result;
  }

  Double_t 
  NLandau2(Double_t* xp, Double_t* pp) 
  {
    Double_t  e        = xp[0];
    Double_t  constant = pp[0];
    Double_t  mpv      = pp[1];
    Double_t  fwhm     = pp[2];
    Int_t     n        = Int_t(pp[3]);
    Double_t  result   = TMath::Landau(e,mpv,fwhm,kTRUE);
    for (Int_t i = 2; i <= n; i++) {
      Double_t ai    =  pp[N2_A(i)];
      Double_t mpvi  =  pp[N2_D(i)];
      Double_t fwhmi =  pp[N2_X(i)];
      result         += ai * TMath::Landau(e,mpvi,fwhmi,kTRUE);
    }
    result *= constant;
    return result;
  }

  Double_t 
  TripleLandau(Double_t *x, Double_t *par) 
  {
    Double_t energy   = x[0];
    Double_t constant = par[0];
    Double_t mpv      = par[1];
    Double_t fwhm     = par[2];
    Double_t alpha    = par[3];
    Double_t beta     = par[4];
    Double_t mpv2     = 2*mpv+2*fwhm*TMath::Log(2);
    Double_t mpv3     = 3*mpv+3*fwhm*TMath::Log(3);

    Double_t f = constant * (TMath::Landau(energy,mpv,fwhm,kTRUE)+
			     alpha * TMath::Landau(energy,mpv2,2*fwhm,kTRUE)+
			     beta  * TMath::Landau(energy,mpv3,3*fwhm,kTRUE));
  
    return f;
  }
  Double_t 
  DoubleLandau(Double_t *x, Double_t *par) 
  {
    Double_t energy   = x[0];
    Double_t constant = par[0];
    Double_t mpv      = par[1];
    Double_t fwhm     = par[2];
    Double_t alpha    = par[3];
    Double_t mpv2     = 2*mpv+2*fwhm*TMath::Log(2);
    
    Double_t f = constant * (TMath::Landau(energy,mpv,fwhm,kTRUE)+
			     alpha * TMath::Landau(energy,mpv2,2*fwhm,kTRUE));
  
    return f;
  }
  Double_t 
  SingleLandau(Double_t *x, Double_t *par) 
  {
    Double_t energy   = x[0];
    Double_t constant = par[0];
    Double_t mpv      = par[1];
    Double_t fwhm     = par[2];
    
    Double_t f = constant * TMath::Landau(energy,mpv,fwhm,kTRUE);
  
    return f;
  }
}

//____________________________________________________________________
AliFMDEnergyFitter::AliFMDEnergyFitter()
  : TNamed(), 
    fRingHistos(),
    fLowCut(0.3),
    fNLandau(3),
    fMinEntries(100),
    fBinsToSubtract(4),
    fDoFits(false),
    fEtaAxis(),
    fDebug(0)
{}

//____________________________________________________________________
AliFMDEnergyFitter::AliFMDEnergyFitter(const char* title)
  : TNamed("fmdEnergyFitter", title), 
    fRingHistos(), 
    fLowCut(0.3),
    fNLandau(3),
    fMinEntries(100),
    fBinsToSubtract(4),
    fDoFits(false),
    fEtaAxis(100,-4,6),
    fDebug(3)
{
  fEtaAxis.SetName("etaAxis");
  fEtaAxis.SetTitle("#eta");
  fRingHistos.Add(new RingHistos(1, 'I'));
  fRingHistos.Add(new RingHistos(2, 'I'));
  fRingHistos.Add(new RingHistos(2, 'O'));
  fRingHistos.Add(new RingHistos(3, 'I'));
  fRingHistos.Add(new RingHistos(3, 'O'));
}

//____________________________________________________________________
AliFMDEnergyFitter::AliFMDEnergyFitter(const AliFMDEnergyFitter& o)
  : TNamed(o), 
    fRingHistos(), 
    fLowCut(o.fLowCut),
    fNLandau(o.fNLandau),
    fMinEntries(o.fMinEntries),
    fBinsToSubtract(o.fBinsToSubtract),
    fDoFits(o.fDoFits),
    fEtaAxis(o.fEtaAxis),
    fDebug(o.fDebug)
{
  TIter    next(&o.fRingHistos);
  TObject* obj = 0;
  while ((obj = next())) fRingHistos.Add(obj);
}

//____________________________________________________________________
AliFMDEnergyFitter::~AliFMDEnergyFitter()
{
  fRingHistos.Delete();
}

//____________________________________________________________________
AliFMDEnergyFitter&
AliFMDEnergyFitter::operator=(const AliFMDEnergyFitter& o)
{
  TNamed::operator=(o);

  fLowCut        = o.fLowCut;
  fNLandau       = o.fNLandau;
  fMinEntries    = o.fMinEntries;
  fBinsToSubtract= o.fBinsToSubtract;
  fDoFits        = o.fDoFits;
  fEtaAxis.Set(o.fEtaAxis.GetNbins(),o.fEtaAxis.GetXmin(),o.fEtaAxis.GetXmax());
  fDebug         = o.fDebug;

  fRingHistos.Delete();
  TIter    next(&o.fRingHistos);
  TObject* obj = 0;
  while ((obj = next())) fRingHistos.Add(obj);
  
  return *this;
}

//____________________________________________________________________
AliFMDEnergyFitter::RingHistos*
AliFMDEnergyFitter::GetRingHistos(UShort_t d, Char_t r) const
{
  Int_t idx = -1;
  switch (d) { 
  case 1: idx = 0; break;
  case 2: idx = 1 + (r == 'I' || r == 'i' ? 0 : 1); break;
  case 3: idx = 3 + (r == 'I' || r == 'i' ? 0 : 1); break;
  }
  if (idx < 0 || idx >= fRingHistos.GetEntries()) return 0;
  
  return static_cast<RingHistos*>(fRingHistos.At(idx));
}

//____________________________________________________________________
void
AliFMDEnergyFitter::Init(const TAxis& eAxis)
{
  fEtaAxis.Set(eAxis.GetNbins(),eAxis.GetXmin(),eAxis.GetXmax());
  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next())))
    o->Init(eAxis);
}  
//____________________________________________________________________
Bool_t
AliFMDEnergyFitter::Accumulate(const AliESDFMD& input, 
			       Bool_t           empty)
{
  for(UShort_t d = 1; d <= 3; d++) {
    Int_t nRings = (d == 1 ? 1 : 2);
    for (UShort_t q = 0; q < nRings; q++) {
      Char_t      r    = (q == 0 ? 'I' : 'O');
      UShort_t    nsec = (q == 0 ?  20 :  40);
      UShort_t    nstr = (q == 0 ? 512 : 256);

      RingHistos* histos = GetRingHistos(d, r);
      
      for(UShort_t s = 0; s < nsec;  s++) {
	for(UShort_t t = 0; t < nstr; t++) {
	  Float_t mult = input.Multiplicity(d,r,s,t);
	  
	  // Keep dead-channel information. 
	  if(mult == AliESDFMD::kInvalidMult || mult > 10 || mult <= 0) 
	    continue;

	  // Get the pseudo-rapidity 
	  Double_t eta1 = input.Eta(d,r,s,t);
	  Int_t ieta = fEtaAxis.FindBin(eta1);
	  if (ieta < 1 || ieta >  fEtaAxis.GetNbins()) continue; 

	  histos->Fill(empty, ieta-1, mult);

	} // for strip
      } // for sector
    } // for ring 
  } // for detector

  return kTRUE;
}

//____________________________________________________________________
void
AliFMDEnergyFitter::Fit(TList* dir)
{
  if (!fDoFits) return;

  TList* d = static_cast<TList*>(dir->FindObject(GetName()));
  if (!d) return;

  // +1          for chi^2
  // +3          for 1 landau 
  // +1          for N 
  // +fNLandau-1 for weights 
  Int_t nStack = 1+3+1+fNLandau-1;
  THStack* stack[nStack]; 
  stack[0] = new THStack("chi2", "#chi^{2}/#nu");
  stack[1] = new THStack("c",    "constant");
  stack[2] = new THStack("mpv",  "#Delta_{p}");
  stack[3] = new THStack("w",    "w");
  stack[4] = new THStack("n",    "# of Landaus");
  for (Int_t i = 2; i <= fNLandau; i++) 
    stack[i-1+4] = new THStack(Form("a%d", i), 
			       Form("Weight of %d signal", i));
  for (Int_t i = 0; i < nStack; i++) 
    d->Add(stack[i]);

  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next()))) {
    TObjArray* l = o->Fit(d, fEtaAxis, fLowCut, fNLandau,
			  fMinEntries, fBinsToSubtract);
    if (!l) continue;
    for (Int_t i = 0; i < l->GetEntriesFast(); i++) { 
      stack[i % nStack]->Add(static_cast<TH1*>(l->At(i))); 
    }
  }
}

//____________________________________________________________________
void
AliFMDEnergyFitter::DefineOutput(TList* dir)
{
  TList* d = new TList;
  d->SetName(GetName());
  dir->Add(d);
  d->Add(&fEtaAxis);
  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next()))) {
    o->Output(d);
  }
}
//____________________________________________________________________
void
AliFMDEnergyFitter::SetDebug(Int_t dbg)
{
  fDebug = dbg;
  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next())))
    o->fDebug = dbg;
}  
  
//====================================================================
AliFMDEnergyFitter::RingHistos::RingHistos()
  : AliForwardUtil::RingHistos(), 
    fEDist(0), 
    fEmpty(0),
    fEtaEDists(), 
    fList(0),
    fDebug(0)
{}

//____________________________________________________________________
AliFMDEnergyFitter::RingHistos::RingHistos(UShort_t d, Char_t r)
  : AliForwardUtil::RingHistos(d,r), 
    fEDist(0), 
    fEmpty(0),
    fEtaEDists(), 
    fList(0),
    fDebug(0)
{
  fEtaEDists.SetName("EDists");
}
//____________________________________________________________________
AliFMDEnergyFitter::RingHistos::RingHistos(const RingHistos& o)
  : AliForwardUtil::RingHistos(o), 
    fEDist(o.fEDist), 
    fEmpty(o.fEmpty),
    fEtaEDists(), 
    fList(0),
    fDebug(0)
{
  TIter next(&o.fEtaEDists);
  TObject* obj = 0;
  while ((obj = next())) fEtaEDists.Add(obj->Clone());
  if (o.fList) {
    fList = new TList;
    fList->SetName(fName);
    TIter next2(o.fList);
    while ((obj = next2())) fList->Add(obj->Clone());
  }
}

//____________________________________________________________________
AliFMDEnergyFitter::RingHistos&
AliFMDEnergyFitter::RingHistos::operator=(const RingHistos& o)
{
  AliForwardUtil::RingHistos::operator=(o);
  
  if (fEDist) delete fEDist;
  if (fEmpty) delete fEmpty;
  fEtaEDists.Delete();
  if (fList) fList->Delete();

  fEDist = static_cast<TH1D*>(o.fEDist->Clone());
  fEmpty = static_cast<TH1D*>(o.fEmpty->Clone());
  
  TIter next(&o.fEtaEDists);
  TObject* obj = 0;
  while ((obj = next())) fEtaEDists.Add(obj->Clone());

  if (o.fList) {
    fList = new TList;
    fList->SetName(fName);
    TIter next2(o.fList);
    while ((obj = next2())) fList->Add(obj->Clone());
  }

  return *this;
}
//____________________________________________________________________
AliFMDEnergyFitter::RingHistos::~RingHistos()
{
  if (fEDist) delete fEDist;
  fEtaEDists.Delete();
}

//____________________________________________________________________
void
AliFMDEnergyFitter::RingHistos::Fill(Bool_t empty, Int_t ieta, Double_t mult)
{
  if (empty) { 
    fEmpty->Fill(mult);
    return;
  }
  fEDist->Fill(mult);
  
  if (ieta < 0 || ieta >= fEtaEDists.GetEntries()) return;
  
  TH1D* h = static_cast<TH1D*>(fEtaEDists.At(ieta));
  if (!h) return;

  h->Fill(mult);
}

//__________________________________________________________________
TArrayD
AliFMDEnergyFitter::RingHistos::::MakeIncreasingAxis(Int_t n, Double_t min, 
						     Double_t max) 
{
  // Service function to define a logarithmic axis. 
  // Parameters: 
  //   n    Number of bins 
  //   min  Minimum of axis 
  //   max  Maximum of axis 
  TArrayD bins(n+1);
  Double_t dx = (max-min) / n;
  bins[0]     = min;
  Int_t    i  = 1;
  for (i = 1; i < n+1; i++) {
    Double_t dI   = float(i)/n;
    Double_t next = bins[i-1] + dX1 + dI * dI * dX1;
    bins[i]       = next;
    if (next > max) break;
  }
  bins.Set(i+1);
  return bins;
}

//____________________________________________________________________
void
AliFMDEnergyFitter::RingHistos::Make(Int_t ieta, Double_t emin, Double_t emax,
				     Double_t deMax, Int_t nDeBins, 
				     Bool_t incr)
{
  TH1D* h = 0;
  TString name  = Form("%s_etabin%03d", fName.Data(), ieta);
  TString title = Form("#DeltaE/#DeltaE_{mip} for %s %+5.3f#leq#eta<%+5.3f "
		       "(#eta bin %d)", fName.Data(), emin, emax, ieta);
  if (!incr) 
    h = new TH1D(name.Data(), title.Data(), nDeBins, 0, deMax);
  else { 
    TArrayD deAxis = MakeIncreasingAxis(nDeBins, 0, deMax);
    h = new TH1D(name.Data(), title.Data(), deAxis.fN-1, deAxis.fArray);
  }
    
  h->SetDirectory(0);
  h->SetXTitle("#DeltaE/#DeltaE_{mip}");	
  h->SetFillColor(Color());
  h->SetMarkerColor(Color());
  h->SetLineColor(Color());
  h->SetFillStyle(3001);
  h->Sumw2();
  
  fEtaEDists.AddAt(h, ieta-1);
}
//____________________________________________________________________
TH1D*
AliFMDEnergyFitter::RingHistos::MakePar(const char* name, 
					const char* title, 
					const TAxis& eta) const
{
  TH1D* h = new TH1D(Form("%s_%s", fName.Data(), name), 
		     Form("%s for %s", title, fName.Data()), 
		     eta.GetNbins(), eta.GetXmin(), eta.GetXmax());
  h->SetXTitle("#eta");
  h->SetYTitle(title);
  h->SetDirectory(0);
  h->SetFillColor(Color());
  h->SetMarkerColor(Color());
  h->SetLineColor(Color());
  h->SetFillStyle(3001);
  h->Sumw2();

  return h;
}
//____________________________________________________________________
TH1D*
AliFMDEnergyFitter::RingHistos::MakeTotal(const char* name, 
					  const char* title, 
					  const TAxis& eta, 
					  Int_t low, 
					  Int_t high, 
					  Double_t val, 
					  Double_t err) const
{
  Double_t xlow  = eta.GetBinLowEdge(low);
  Double_t xhigh = eta.GetBinUpEdge(high);
  TH1D* h = new TH1D(Form("%s_%s", fName.Data(), name), 
		     Form("%s for %s", title, fName.Data()), 
		     1, xlow, xhigh);
  h->SetBinContent(1, val);
  h->SetBinError(1, err);
  h->SetXTitle("#eta");
  h->SetYTitle(title);
  h->SetDirectory(0);
  h->SetFillColor(0);
  h->SetMarkerColor(Color());
  h->SetLineColor(Color());
  h->SetFillStyle(0);
  h->SetLineStyle(2);
  h->SetLineWidth(2);

  return h;
}
		     
//____________________________________________________________________
void
AliFMDEnergyFitter::RingHistos::Init(const TAxis& eAxis, 
				     Double_t maxDE, Int_t nDEbins, 
				     Bool_t useIncrBin)
{
  fEDist = new TH1D(Form("%s_edist", fName.Data()), 
		    Form("#DeltaE/#DeltaE_{mip} for %s", fName.Data()), 
		    200, 0, 6);
  fEmpty = new TH1D(Form("%s_empty", fName.Data()), 
		    Form("#DeltaE/#DeltaE_{mip} for %s (empty events)", 
			 fName.Data()), 200, 0, 6);
  fList->Add(fEDist);
  fList->Add(fEmpty);
  // fEtaEDists.Expand(eAxis.GetNbins());
  for (Int_t i = 1; i <= eAxis.GetNbins(); i++) { 
    Double_t min = eAxis.GetBinLowEdge(i);
    Double_t max = eAxis.GetBinUpEdge(i);
    Make(i, min, max, maxDE, nDEbins, useIncrBin);
  }
  fList->Add(&fEtaEDists);
}
//____________________________________________________________________
TObjArray*
AliFMDEnergyFitter::RingHistos::Fit(TList* dir, const TAxis& eta,
				    Double_t lowCut, UShort_t nLandau,
				    UShort_t minEntries,
				    UShort_t minusBins) const
{
  TList* l = GetOutputList(dir);
  if (!l) return 0; 

  TList* dists = static_cast<TList*>(l->FindObject("EDists"));
  if (!dists) { 
    AliWarning(Form("Didn't find %s_EtaEDists in %s", 
		    fName.Data(), l->GetName()));
    l->ls();
    return 0;
  }

  TObjArray* pars  = new TObjArray(3+nLandau+1);
  pars->SetName("FitResults");
  l->Add(pars);

  TH1* hChi2 = 0;
  TH1* hC    = 0;
  TH1* hMpv  = 0;
  TH1* hW    = 0;
  TH1* nS    = 0;
  TH1* hN    = 0;
  TH1* hA[nLandau-1];
  pars->Add(hChi2 = MakePar("chi2", "#chi^{2}/#nu", eta));
  pars->Add(hC    = MakePar("c",    "Constant", eta));
  pars->Add(hMpv  = MakePar("mpv",  "#Delta_{p}", eta));
  pars->Add(hW    = MakePar("w",    "#xi", eta));
  pars->Add(hS    = MakePar("s",    "#sigma", eta));
  pars->Add(hN    = MakePar("n",    "N", eta));
  for (UShort_t i = 1; i < nLandau; i++) 
    pars->Add(hA[i-1] = MakePar(Form("a%d",i+1), Form("a_{%d}",i+1), eta));

  
  Int_t nDists = dists->GetEntries();
  Int_t low    = nDists;
  Int_t high   = 0;
  for (Int_t i = 0; i < nDists; i++) { 
    TH1D* dist = static_cast<TH1D*>(dists->At(i));

    if (!dist) continue;

    TF1* res = FitHist(dist,lowCut,nLandau,minEntries,minusBins);
    if (!res) continue;
    
    low   = TMath::Min(low,i+1);
    high  = TMath::Max(high,i+1);

    Double_t chi2 = res->GetChisquare();
    Int_t    ndf  = res->GetNDF();
    hChi2->SetBinContent(i+1, ndf > 0 ? chi2 / ndf : 0);
    hC  ->SetBinContent(i+1, res->GetParameter(0));   
    hMpv->SetBinContent(i+1, res->GetParameter(1)); 
    hW  ->SetBinContent(i+1, res->GetParameter(2));   
    hN  ->SetBinContent(i+1, res->GetParameter(3));   

    hC  ->SetBinError(i+1, res->GetParError(1));
    hMpv->SetBinError(i+1, res->GetParError(2));
    hW  ->SetBinError(i+1, res->GetParError(2));
    // hN  ->SetBinError(i, res->GetParError(3));

    for (UShort_t j = 0; j < nLandau-1; j++) {
      hA[j]->SetBinContent(i+1, res->GetParameter(4+j));
      hA[j]->SetBinError(i+1, res->GetParError(4+j));
    }
  }

  TH1* total = GetOutputHist(l, Form("%s_edist", fName.Data()));
  if (total) { 
    TF1* res = FitHist(total,lowCut,nLandau,minEntries,minusBins);
    if (res) { 
      Double_t chi2 = res->GetChisquare();
      Int_t    ndf  = res->GetNDF();
      pars->Add(MakeTotal("t_chi2", "#chi^{2}/#nu", eta, low, high,
			  ndf > 0 ? chi2/ndf : 0, 0));
      pars->Add(MakeTotal("t_c",    "Constant",     eta, low, high,
			  res->GetParameter(0),res->GetParError(0)));
      pars->Add(MakeTotal("t_mpv",  "#Delta_{p}",   eta, low, high,
			  res->GetParameter(1),res->GetParError(1)));
      pars->Add(MakeTotal("t_w",    "#xi",          eta, low, high,
			  res->GetParameter(2),res->GetParError(2)));
      pars->Add(MakeTotal("t_n",    "N",            eta, low, high,
			  res->GetParameter(3),0));
      for (UShort_t j = 1; j < nLandau; j++) 
	pars->Add(MakeTotal(Form("a%d_t",j+1), 
			    Form("a_{%d}",j+1), eta, low, high,
			    res->GetParameter(3+j), res->GetParError(3+j)));
    }
  }
    
  TObjLink* lnk = dists->FirstLink();
  while (lnk) {
    TH1* h = static_cast<TH1*>(lnk->GetObject());
    if (h->GetEntries() <= 0 || 
	h->GetListOfFunctions()->GetEntries() <= 0) {
      TObjLink* keep = lnk->Next();
      dists->Remove(lnk);
      lnk = keep;
      continue;
    }
    lnk = lnk->Next();
  }

  return pars;
}

//____________________________________________________________________
TF1*
AliFMDEnergyFitter::RingHistos::FitHist(TH1* dist,Double_t lowCut, 
					UShort_t nLandau, 
					UShort_t minEntries,
					UShort_t minusBins) const
{
  Double_t maxRange = 10;

  if (dist->GetEntries() <= minEntries) return 0;

  // Find the fit range 
  dist->GetXaxis()->SetRangeUser(lowCut, maxRange);
  
  // Normalize peak to 1 
  Double_t max = dist->GetMaximum(); 
  dist->Scale(1/max);
  
  // Get the bin with maximum 
  Int_t    maxBin = dist->GetMaximumBin();
  Double_t maxE   = dist->GetBinLowEdge(maxBin);
  
  // Get the low edge 
  dist->GetXaxis()->SetRangeUser(lowCut, maxE);
  Int_t    minBin = maxBin - minusBins; // dist->GetMinimumBin();
  Double_t minE   = TMath::Max(dist->GetBinCenter(minBin),lowCut);
  Double_t maxEE  = dist->GetBinCenter(maxBin+2*minusBins);

  // Restore the range 
  dist->GetXaxis()->SetRangeUser(0, maxRange);
  
  // First do a single landau fit 
  TF1*          landau1 = new TF1("landau1", "landau", minE, maxEE);
  // landau1->SetParameters(1,1,1,1);
  landau1->SetParNames("C","#Delta_{p}","#xi");
  landau1->SetParLimits(1,minE,maxRange);
  landau1->SetParLimits(2,0,maxRange);
  landau1->SetLineColor(Color());
  // Tight fit around peak - make sure we get that right. 
  TFitResultPtr r = dist->Fit(landau1, "RQNS", "", minE, maxEE);

  return FitMore2(dist, nLandau, *r, landau1, minE, maxRange);
}

//____________________________________________________________________
TF1*
AliFMDEnergyFitter::RingHistos::FitMore(TH1*        dist,
					UShort_t    nLandau,
					TFitResult& r, 
					TF1*        landau1,
					Double_t    minE,
					Double_t    maxRange) const
{
  static TClonesArray res("TFitResult");
  static TObjArray funcs;
  res.Clear();
  funcs.SetOwner();
  funcs.Clear();
  Int_t nRes = 0;

  //r->Print();
  new(res[nRes++]) TFitResult(r);
  funcs.AddAtAndExpand(landau1, 0);

  // Now try to fit 
  for (UShort_t n = 2; n <= nLandau; n++) { 
    TFitResult* rr = static_cast<TFitResult*>(res.At(nRes-1));
    if (!rr) { 
      AliError(Form("No previous result (%p, %d) for n=%d", rr, nRes, n));
      return 0;
    }
    // Create the function object 
    Double_t mpvi  = rr->Parameter(1);
    Double_t wi    = rr->Parameter(2);
    Double_t maxEi = n*(mpvi+wi*TMath::Log(n))+2*n*wi;
    TF1* landaui = new TF1(Form("landau%d", n), &NLandau, minE, maxEi,3+n);
    landaui->SetLineStyle((n % 10)+1);
    landaui->SetLineWidth(2);
    landaui->SetLineColor(Color());
    landaui->SetParameter(0, rr->Parameter(0));
    landaui->SetParameter(1, rr->Parameter(1));
    landaui->SetParameter(2, rr->Parameter(2));
    landaui->SetParLimits(1, minE, maxRange);
    landaui->SetParLimits(2,0,maxRange);
    landaui->FixParameter(3, n);
    landaui->SetParNames("C","#Delta_{p}","#xi", "N");
    for (UShort_t i = 2; i < n; i++) {// Take parameters from last fit 
      landaui->SetParameter(N_A(i), rr->Parameter(N_A(i)));
      landaui->SetParLimits(N_A(i), 0,1);
      landaui->SetParName(i, Form("a_{%d}", i));
    }
    landaui->SetParameter(N_A(n), (n == 2 ? 0.05 : rr->Parameter(N_A(n-1))/5));
    landaui->SetParLimits(N_A(n), 0, 1);
    landaui->SetParName(N_A(n), Form("a_{%d}", n));
    // landaui->Print();

    TFitResultPtr tr = dist->Fit(landaui, "RNSQ", "", minE, maxEi);
    // tr->Print();
    if (CheckResult(*tr)) { 
      new(res[nRes++]) TFitResult(*tr);
      funcs.AddAtAndExpand(landaui,n-1);
      continue;
    }
    // Stop on bad fit 
    break;
  }
  TF1* ret = 0;
  if (funcs.At(nRes-1)) ret = static_cast<TF1*>(funcs.At(nRes-1)->Clone());
  dist->GetListOfFunctions()->Add(ret);

  res.Clear();
  funcs.Clear();

  return ret;
}

//____________________________________________________________________
TF1*
AliFMDEnergyFitter::RingHistos::FitMore2(TH1*        dist,
					 UShort_t    nLandau,
					 TFitResult& r, 
					 TF1*        landau1,
					 Double_t    minE,
					 Double_t    maxRange) const
{
  static TClonesArray res("TFitResult");
  static TObjArray funcs;
  res.Clear();
  funcs.SetOwner();
  funcs.Clear();
  Int_t nRes = 0;

  //r->Print();
  new(res[nRes++]) TFitResult(r);
  funcs.AddAtAndExpand(landau1, 0);

  // Now try to fit 
  for (UShort_t n = 2; n <= nLandau; n++) { 
    TFitResult* rr = static_cast<TFitResult*>(res.At(nRes-1));
    if (!rr) { 
      AliError(Form("No previous result (%p, %d) for n=%d", rr, nRes, n));
      return 0;
    }
    // Create the function object 
    Double_t mpvi  = rr->Parameter(1);
    Double_t wi    = rr->Parameter(2);
    Double_t maxEi = n*(mpvi+wi*TMath::Log(n))+2*n*wi;
    TF1* landaui = new TF1(Form("landau%d", n), &NLandau, minE, maxEi,
			   n*3+1);
    landaui->SetLineStyle((n % 10)+1);
    landaui->SetLineWidth(2);
    landaui->SetLineColor(Color());
    landaui->SetParameter(0, rr->Parameter(0));
    landaui->SetParameter(1, rr->Parameter(1));
    landaui->SetParameter(2, rr->Parameter(2));
    landaui->SetParLimits(1, minE, maxRange);
    landaui->SetParLimits(2,0,maxRange);
    landaui->FixParameter(3, n);
    landaui->SetParNames("C","#Delta_{p}","#xi", "N");
    for (UShort_t i = 2; i < n; i++) {// Take parameters from last fit 
      landaui->SetParameter(N2_A(i), rr->Parameter(N2_A(i)));
      landaui->SetParameter(N2_D(i), rr->Parameter(N2_D(i)));
      landaui->SetParameter(N2_X(i), rr->Parameter(N2_X(i)));
      landaui->SetParLimits(N2_A(i), 0,1);
      landaui->SetParLimits(N2_D(i), minE,maxEi);
      landaui->SetParLimits(N2_X(i), 0,maxRange);
      landaui->SetParName(N2_A(i), Form("a_{%d}", i));
      landaui->SetParName(N2_D(i), Form("#Delta_{p,%d}", i));
      landaui->SetParName(N2_X(i), Form("#xi_{%d}", i));
    }
    landaui->SetParameter(N2_A(n), n == 2 ? 0.05 : rr->Parameter(N2_A(n-1))/5);
    landaui->SetParameter(N2_D(n), mpvi);
    landaui->SetParameter(N2_X(n), wi);
    landaui->SetParLimits(N2_A(n), 0,1);
    landaui->SetParLimits(N2_D(n), minE,maxEi);
    landaui->SetParLimits(N2_X(n), 0,maxRange);
    landaui->SetParName(N2_A(n), Form("a_{%d}", n));
    landaui->SetParName(N2_D(n), Form("#Delta_{p,%d}", n));
    landaui->SetParName(N2_X(n), Form("#xi_{%d}", n));
    if (fDebug > 2) landaui->Print();

    TFitResultPtr tr = dist->Fit(landaui, "RNSQ", "", minE, maxEi);
    if (fDebug > 2)  tr->Print();
    if (CheckResult(*tr)) { 
      new(res[nRes++]) TFitResult(*tr);
      funcs.AddAtAndExpand(landaui,n-1);
      continue;
    }
    // Stop on bad fit 
    break;
  }
  TF1* ret = 0;
  if (funcs.At(nRes-1)) ret = static_cast<TF1*>(funcs.At(nRes-1)->Clone());
  dist->GetListOfFunctions()->Add(ret);

  res.Clear();
  funcs.Clear();

  return ret;
}

//____________________________________________________________________
Bool_t
AliFMDEnergyFitter::RingHistos::CheckResult(TFitResult& r) const
{
  Double_t chi2 = r.Chi2();
  Int_t    ndf  = r.Ndf();
  // Double_t prob = r.Prob();
  if (ndf <= 0 || chi2 / ndf > 5) { 
    if (fDebug > 2)
      AliWarning(Form("Fit %s has chi^2/ndf=%f/%d=%f", 
		      r.GetName(), chi2, ndf, (ndf<0 ? 0 : chi2/ndf)));
    return kFALSE;
  }
    
  UShort_t nPar = r.NPar();
  for (UShort_t i = 0; i < nPar; i++) { 
    if (i == 3) continue; 
    
    Double_t v = r.Parameter(i);
    Double_t e = r.ParError(i);
    if (v == 0) continue;
    if (v == 0 || e / v > 0.2) { 
      if (fDebug > 2)
	AliWarning(Form("Fit %s has Delta %s/%s=%f/%f=%f%%",
			r.GetName(), r.ParName(i).c_str(), 
			r.ParName(i).c_str(), e, v, 100*(v == 0 ? 0 : e/v)));
      return kFALSE;
    }
  }
  if (nPar > 4) { 
    if (r.Parameter(nPar-1) <= 1e-10) { 
      if (fDebug)
	AliWarning(Form("Last scale %s is too small %f<1e-10", 
			r.ParName(nPar-1).c_str(), r.Parameter(nPar-1)));
      return kFALSE;
    }
  }
  return kTRUE;
}


//____________________________________________________________________
void
AliFMDEnergyFitter::RingHistos::Output(TList* dir)
{
  fList = DefineOutputList(dir);
}

//____________________________________________________________________
//
// EOF
//

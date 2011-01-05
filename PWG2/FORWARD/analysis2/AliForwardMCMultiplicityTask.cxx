#include "AliForwardMCMultiplicityTask.h"
#include "AliTriggerAnalysis.h"
#include "AliPhysicsSelection.h"
#include "AliLog.h"
// #include "AliFMDAnaParameters.h"
#include "AliESDEvent.h"
#include "AliAODHandler.h"
#include "AliMultiplicity.h"
#include "AliInputEventHandler.h"
#include "AliForwardCorrectionManager.h"
#include "AliAnalysisManager.h"
#include <TH1.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TROOT.h>
#include <iostream>
#include <iomanip>

//====================================================================
AliForwardMCMultiplicityTask::AliForwardMCMultiplicityTask()
  : AliForwardMultiplicityBase(),
    fHData(0),
    fESDFMD(),
    fHistos(),
    fAODFMD(),
    fMCESDFMD(),
    fMCHistos(),
    fMCAODFMD(),
    fEventInspector(),
    fEnergyFitter(),
    fSharingFilter(),
    fDensityCalculator(),
    fCorrections(),
    fHistCollector(),
    fList(0)
{
}

//____________________________________________________________________
AliForwardMCMultiplicityTask::AliForwardMCMultiplicityTask(const char* name)
  : AliForwardMultiplicityBase(name), 
    fHData(0),
    fESDFMD(),
    fHistos(),
    fAODFMD(kFALSE),
    fMCESDFMD(),
    fMCHistos(),
    fMCAODFMD(kTRUE),
    fEventInspector("event"),
    fEnergyFitter("energy"),
    fSharingFilter("sharing"), 
    fDensityCalculator("density"),
    fCorrections("corrections"),
    fHistCollector("collector"),
    fList(0)
{
  DefineOutput(1, TList::Class());
}

//____________________________________________________________________
AliForwardMCMultiplicityTask::AliForwardMCMultiplicityTask(const AliForwardMCMultiplicityTask& o)
  : AliForwardMultiplicityBase(o),
    fHData(o.fHData),
    fESDFMD(o.fESDFMD),
    fHistos(o.fHistos),
    fAODFMD(o.fAODFMD),
    fMCESDFMD(o.fMCESDFMD),
    fMCHistos(o.fMCHistos),
    fMCAODFMD(o.fMCAODFMD),
    fEventInspector(o.fEventInspector),
    fEnergyFitter(o.fEnergyFitter),
   fSharingFilter(o.fSharingFilter),
    fDensityCalculator(o.fDensityCalculator),
    fCorrections(o.fCorrections),
    fHistCollector(o.fHistCollector),
    fList(o.fList) 
{
  DefineOutput(1, TList::Class());
}

//____________________________________________________________________
AliForwardMCMultiplicityTask&
AliForwardMCMultiplicityTask::operator=(const AliForwardMCMultiplicityTask& o)
{
  AliForwardMultiplicityBase::operator=(o);

  fHData             = o.fHData;
  fEventInspector    = o.fEventInspector;
  fEnergyFitter      = o.fEnergyFitter;
  fSharingFilter     = o.fSharingFilter;
  fDensityCalculator = o.fDensityCalculator;
  fCorrections       = o.fCorrections;
  fHistCollector     = o.fHistCollector;
  fHistos            = o.fHistos;
  fAODFMD            = o.fAODFMD;
  fMCHistos          = o.fMCHistos;
  fMCAODFMD          = o.fMCAODFMD;
  fList              = o.fList;

  return *this;
}

//____________________________________________________________________
void
AliForwardMCMultiplicityTask::SetDebug(Int_t dbg)
{
  fEventInspector.SetDebug(dbg);
  fEnergyFitter.SetDebug(dbg);
  fSharingFilter.SetDebug(dbg);
  fDensityCalculator.SetDebug(dbg);
  fCorrections.SetDebug(dbg);
  fHistCollector.SetDebug(dbg);
}
//____________________________________________________________________
void
AliForwardMCMultiplicityTask::InitializeSubs()
{
  AliForwardCorrectionManager& fcm = AliForwardCorrectionManager::Instance();
  fcm.Init(fEventInspector.GetCollisionSystem(), 
	   fEventInspector.GetEnergy(),
	   fEventInspector.GetField(), 
	   true); // Last true is for MC flag 
  // Check that we have the energy loss fits, needed by 
  //   AliFMDSharingFilter 
  //   AliFMDDensityCalculator 
  if (!fcm.GetELossFit()) { 
    AliFatal(Form("No energy loss fits"));
    return;
  }
  // Check that we have the double hit correction - (optionally) used by 
  //  AliFMDDensityCalculator 
  if (!fcm.GetDoubleHit()) {
    AliWarning("No double hit corrections"); 
  }
  // Check that we have the secondary maps, needed by 
  //   AliFMDCorrections 
  //   AliFMDHistCollector
  if (!fcm.GetSecondaryMap()) {
    AliFatal("No secondary corrections");
    return;
  }
  // Check that we have the vertex bias correction, needed by 
  //   AliFMDCorrections 
  if (!fcm.GetVertexBias()) { 
    AliFatal("No event vertex bias corrections");
    return;
  }
  // Check that we have the merging efficiencies, optionally used by 
  //   AliFMDCorrections 
  if (!fcm.GetMergingEfficiency()) {
    AliWarning("No merging efficiencies");
  }


  const TAxis* pe = fcm.GetEtaAxis();
  const TAxis* pv = fcm.GetVertexAxis();
  if (!pe) AliFatal("No eta axis defined");
  if (!pv) AliFatal("No vertex axis defined");

  fHistos.Init(*pe);
  fAODFMD.Init(*pe);
  fMCHistos.Init(*pe);
  fMCAODFMD.Init(*pe);

  fHData = static_cast<TH2D*>(fAODFMD.GetHistogram().Clone("d2Ndetadphi"));
  fHData->SetStats(0);
  fHData->SetDirectory(0);
  fList->Add(fHData);

  fEnergyFitter.Init(*pe);
  fEventInspector.Init(*pv);
  fDensityCalculator.Init(*pe);
  fCorrections.Init(*pe);
  fHistCollector.Init(*pv);

  this->Print();
}

//____________________________________________________________________
void
AliForwardMCMultiplicityTask::UserCreateOutputObjects()
{
  fList = new TList;

  AliAnalysisManager* am = AliAnalysisManager::GetAnalysisManager();
  AliAODHandler*      ah = 
    dynamic_cast<AliAODHandler*>(am->GetOutputEventHandler());
  if (!ah) AliFatal("No AOD output handler set in analysis manager");
    
    
  TObject* obj = &fAODFMD;
  ah->AddBranch("AliAODForwardMult", &obj);

  TObject* mcobj = &fMCAODFMD;
  ah->AddBranch("AliAODForwardMult", &mcobj);

  fEventInspector.DefineOutput(fList);
  fEnergyFitter.DefineOutput(fList);
  fSharingFilter.DefineOutput(fList);
  fDensityCalculator.DefineOutput(fList);
  fCorrections.DefineOutput(fList);

  PostData(1, fList);
}
//____________________________________________________________________
void
AliForwardMCMultiplicityTask::UserExec(Option_t*)
{
  // Get the input data 
  AliESDEvent* esd = dynamic_cast<AliESDEvent*>(InputEvent());
  if (!esd) { 
    AliWarning("No ESD event found for input event");
    return;
  }

  // On the first event, initialize the parameters 
  if (fFirstEvent && esd->GetESDRun()) { 
    fEventInspector.ReadRunDetails(esd);
    
    AliInfo(Form("Initializing with parameters from the ESD:\n"
		 "         AliESDEvent::GetBeamEnergy()   ->%f\n"
		 "         AliESDEvent::GetBeamType()     ->%s\n"
		 "         AliESDEvent::GetCurrentL3()    ->%f\n"
		 "         AliESDEvent::GetMagneticField()->%f\n"
		 "         AliESDEvent::GetRunNumber()    ->%d\n",
		 esd->GetBeamEnergy(), 
		 esd->GetBeamType(),
		 esd->GetCurrentL3(), 
		 esd->GetMagneticField(),
		 esd->GetRunNumber()));

    fFirstEvent = false;

    InitializeSubs();
  }
  // Clear stuff 
  fHistos.Clear();
  fESDFMD.Clear();
  fAODFMD.Clear();
  fMCHistos.Clear();
  fMCESDFMD.Clear();
  fMCAODFMD.Clear();

  Bool_t   lowFlux  = kFALSE;
  UInt_t   triggers = 0;
  UShort_t ivz      = 0;
  Double_t vz       = 0;
  UInt_t   found    = fEventInspector.Process(esd, triggers, lowFlux, ivz, vz);
  if (found & AliFMDEventInspector::kNoEvent)    return;
  if (found & AliFMDEventInspector::kNoTriggers) return;
 
  // Set trigger bits, and mark this event for storage 
  fAODFMD.SetTriggerBits(triggers);
  fMCAODFMD.SetTriggerBits(triggers);
  MarkEventForStore();

  if (found & AliFMDEventInspector::kNoSPD)     return;
  if (found & AliFMDEventInspector::kNoFMD)     return;
  if (found & AliFMDEventInspector::kNoVertex)  return;
  fAODFMD.SetIpZ(vz);
  fMCAODFMD.SetIpZ(vz);

  if (found & AliFMDEventInspector::kBadVertex) return;

  // We we do not want to use low flux specific code, we disable it here. 
  if (!fEnableLowFlux) lowFlux = false;

  // Get FMD data 
  AliESDFMD*  esdFMD  = esd->GetFMDData();
  AliMCEvent* mcEvent = MCEvent();
  // Apply the sharing filter (or hit merging or clustering if you like)
  if (!fSharingFilter.Filter(*esdFMD, lowFlux, fESDFMD)) { 
    AliWarning("Sharing filter failed!");
    return;
  }
  if (!fSharingFilter.FilterMC(*esdFMD, *mcEvent, vz, fMCESDFMD)) { 
    AliWarning("MC Sharing filter failed!");
    return;
  }
  fSharingFilter.CompareResults(fESDFMD, fMCESDFMD);

  // Do the energy stuff 
  if (!fEnergyFitter.Accumulate(*esdFMD, triggers & AliAODForwardMult::kEmpty)){
    AliWarning("Energy fitter failed");
    return;
  }

  // Calculate the inclusive charged particle density 
  if (!fDensityCalculator.Calculate(fESDFMD, fHistos, ivz, lowFlux)) { 
    AliWarning("Density calculator failed!");
    return;
  }
  if (!fDensityCalculator.CalculateMC(fMCESDFMD, fMCHistos)) { 
    AliWarning("MC Density calculator failed!");
    return;
  }
  fDensityCalculator.CompareResults(fHistos, fMCHistos);
  
  // Do the secondary and other corrections. 
  if (!fCorrections.Correct(fHistos, ivz)) { 
    AliWarning("Corrections failed");
    return;
  }
  if (!fCorrections.CorrectMC(fMCHistos, ivz)) { 
    AliWarning("MC Corrections failed");
    return;
  }
  fCorrections.CompareResults(fHistos, fMCHistos);
    
  if (!fHistCollector.Collect(fHistos, ivz, fAODFMD.GetHistogram())) {
    AliWarning("Histogram collector failed");
    return;
  }
  if (!fHistCollector.Collect(fMCHistos, ivz, fMCAODFMD.GetHistogram())) {
    AliWarning("MC Histogram collector failed");
    return;
  }

  if (fAODFMD.IsTriggerBits(AliAODForwardMult::kInel))
    fHData->Add(&(fAODFMD.GetHistogram()));

  PostData(1, fList);
}

//____________________________________________________________________
void
AliForwardMCMultiplicityTask::Terminate(Option_t*)
{
  TList* list = dynamic_cast<TList*>(GetOutputData(1));
  if (!list) {
    AliError(Form("No output list defined (%p)", GetOutputData(1)));
    if (GetOutputData(1)) GetOutputData(1)->Print();
    return;
  }
  
  // Get our histograms from the container 
  TH1I* hEventsTr    = 0;
  TH1I* hEventsTrVtx = 0;
  TH1I* hTriggers    = 0;
  if (!fEventInspector.FetchHistograms(list, hEventsTr, 
				       hEventsTrVtx, hTriggers)) { 
    AliError(Form("Didn't get histograms from event selector "
		  "(hEventsTr=%p,hEventsTrVtx=%p)", 
		  hEventsTr, hEventsTrVtx));
    list->ls();
    return;
  }

  TH2D* hData        = static_cast<TH2D*>(list->FindObject("d2Ndetadphi"));
  if (!hData) { 
    AliError(Form("Couldn't get our summed histogram from output "
		  "list %s (d2Ndetadphi=%p)", list->GetName(), hData));
    list->ls();
    return;
  }
  
  // TH1D* dNdeta = fHData->ProjectionX("dNdeta", 0, -1, "e");
  TH1D* dNdeta = hData->ProjectionX("dNdeta", 1, -1, "e");
  TH1D* norm   = hData->ProjectionX("norm",   0,  1,  "");
  dNdeta->SetTitle("dN_{ch}/d#eta in the forward regions");
  dNdeta->SetYTitle("#frac{1}{N}#frac{dN_{ch}}{d#eta}");
  dNdeta->Divide(norm);
  dNdeta->SetStats(0);
  dNdeta->Scale(Double_t(hEventsTrVtx->GetEntries())/hEventsTr->GetEntries(),
		"width");
  list->Add(dNdeta);
  list->Add(norm);

  fEnergyFitter.Fit(list);
  fSharingFilter.ScaleHistograms(list,hEventsTr->Integral());
  fDensityCalculator.ScaleHistograms(list,hEventsTrVtx->Integral());
  fCorrections.ScaleHistograms(list,hEventsTrVtx->Integral());
}

//____________________________________________________________________
void
AliForwardMCMultiplicityTask::Print(Option_t* option) const
{
  AliForwardMultiplicityBase::Print(option);
  gROOT->IncreaseDirLevel();
  fEventInspector   .Print(option);
  fEnergyFitter     .Print(option);    
  fSharingFilter    .Print(option);
  fDensityCalculator.Print(option);
  fCorrections      .Print(option);
  fHistCollector    .Print(option);
  gROOT->DecreaseDirLevel();
}

//
// EOF
//

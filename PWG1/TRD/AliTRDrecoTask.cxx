///////////////////////////////////////////////////////
//
// Basic class for Performance/Calibration TRD tasks
// 
// It performs generic tasks like :
//   - data file manegment
//   - reference container management
//   - debug container management
//   - interaction with AliAnalysisManager
//   - Plot functor loop
//
// Author: Alexandru Bercuci <A.Bercuci@gsi.de>, 10/09/2008
//
/////////////////////////////////////////////////////////

#include "TClass.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TMethodArg.h"
#include "TFile.h"
#include "TChain.h"
#include "TList.h"
#include "TMap.h"
#include "TH1.h"
#include "TF1.h"
#include "TObjArray.h"
#include "TDirectory.h"
#include "TTreeStream.h"

#include "AliLog.h"
#include "AliAnalysisTask.h"

#include "AliTRDrecoTask.h"

ClassImp(AliTRDrecoTask)

TList* AliTRDrecoTask::fgTrendPoint(0x0);
TTreeSRedirector* AliTRDrecoTask::fgDebugStream(0x0);
//_______________________________________________________

AliTRDrecoTask::AliTRDrecoTask()
  : AliAnalysisTaskSE()
  ,fNRefFigures(0)
  ,fContainer(0x0)
  ,fTracks(0x0)
  ,fkTrack(0x0)
  ,fkMC(0x0)
  ,fkESD(0x0)
  ,fDebugLevel(0)  
  ,fPlotFuncList(0x0)
{
}

AliTRDrecoTask::AliTRDrecoTask(const char *name, const char */*title*/)
  : AliAnalysisTaskSE(name)
  ,fNRefFigures(0)
  ,fContainer(0x0)
  ,fTracks(0x0)
  ,fkTrack(0x0)
  ,fkMC(0x0)
  ,fkESD(0x0)
  ,fDebugLevel(0)  
  ,fPlotFuncList(0x0)
{
  DefineInput (1, TObjArray::Class());
  DefineOutput(1, TObjArray::Class());
}

//_______________________________________________________
AliTRDrecoTask::~AliTRDrecoTask() 
{

  // Generic task destructor

  AliDebug(1, Form(" Ending %s (%s)\n", GetName(), GetTitle()));
  if(fgDebugStream){ 
    delete fgDebugStream;
    fgDebugStream = 0x0;
  }

  if(fPlotFuncList){
    fPlotFuncList->Delete();
    delete fPlotFuncList;
    fPlotFuncList = 0x0;
  }
  
  if(fContainer){
    if(fContainer->IsOwner()) fContainer->Delete();
    delete fContainer;
    fContainer = 0x0;
  }

  if(fgTrendPoint){
    TFile::Open("TRD.PerformanceTrend.root", "UPDATE");
    fgTrendPoint->Write();
    delete fgTrendPoint;
    fgTrendPoint=0x0;
    gFile->Close();
  }
}

//_______________________________________________________
void AliTRDrecoTask::ConnectInputData(Option_t *)
{
  //
  // Connect input data
  //
    AliAnalysisTaskSE::ConnectInputData();
    fTracks = dynamic_cast<TObjArray *>(GetInputData(1));
}

//_______________________________________________________
void AliTRDrecoTask::UserExec(Option_t *)
{
// Loop over Plot functors published by particular tasks

  if(!fPlotFuncList){
    AliWarning("No functor list defined for the reference plots");
    return;
  }
  if(!fTracks) return;
  if(!fTracks->GetEntriesFast()) return;
  
  AliTRDtrackInfo *trackInfo = 0x0;
  TIter plotIter(fPlotFuncList);
  TObjArrayIter trackIter(fTracks);
  while((trackInfo = dynamic_cast<AliTRDtrackInfo*>(trackIter()))){
    fkTrack = trackInfo->GetTrack();
    fkMC    = trackInfo->GetMCinfo();
    fkESD   = trackInfo->GetESDinfo();

    TMethodCall *plot = 0x0;
    plotIter.Reset();
    while((plot=dynamic_cast<TMethodCall*>(plotIter()))){
      plot->Execute(this);
    }
  }
  PostData(1, fContainer);
}

//_______________________________________________________
Bool_t AliTRDrecoTask::GetRefFigure(Int_t /*ifig*/)
{
  AliWarning("Retrieving reference figures not implemented.");
  return kFALSE;
}

//_______________________________________________________
Bool_t AliTRDrecoTask::PutTrendValue(const Char_t *name, Double_t val)
{
// Generic publisher for trend values

  if(!fgTrendPoint){
    fgTrendPoint = new TList();
    fgTrendPoint->SetOwner();
  }
  fgTrendPoint->AddLast(new TNamed(Form("%s_%s", GetName(), name), Form("%f", val)));
  return kTRUE;
}

//_______________________________________________________
void AliTRDrecoTask::InitFunctorList()
{
// Initialize list of functors

  TClass *c = this->IsA();

  TMethod *m = 0x0;
  TIter methIter(c->GetListOfMethods());
  while((m=dynamic_cast<TMethod*>(methIter()))){
    TString name(m->GetName());
    if(!name.BeginsWith("Plot")) continue;
    if(!fPlotFuncList) fPlotFuncList = new TList();
    fPlotFuncList->AddLast(new TMethodCall(c, (const char*)name, ""));
  }
}

//_______________________________________________________
Bool_t AliTRDrecoTask::Load(const Char_t *filename)
{
// Generic container loader

  if(!TFile::Open(filename)){
    AliWarning(Form("Couldn't open file %s.", filename));
    return kFALSE;
  }
  TObjArray *o = 0x0;
  if(!(o = (TObjArray*)gFile->Get(GetName()))){
    AliWarning("Missing histogram container.");
    return kFALSE;
  }
  fContainer = (TObjArray*)o->Clone(GetName());
  gFile->Close();
  return kTRUE;
}

//________________________________________________________
Bool_t AliTRDrecoTask::Save(TObjArray * const results){
  //
  // Store the output graphs in a ROOT file
  // Input TObject array will not be written as Key to the file,
  // only content itself
  //

  TDirectory *cwd = gDirectory;
  if(!TFile::Open(Form("TRD.Result%s.root", GetName()), "RECREATE")) return kFALSE;

  TIterator *iter = results->MakeIterator();
  TObject *inObject = 0x0, *outObject = 0x0;
  while((inObject = iter->Next())){
    outObject = inObject->Clone();
    outObject->Write(0x0, TObject::kSingleKey);
  }
  delete iter;
  gFile->Close(); delete gFile;
  cwd->cd(); 
  return kTRUE;
}

//_______________________________________________________
Bool_t AliTRDrecoTask::PostProcess()
{
// To be implemented by particular tasks

  AliWarning("Post processing of reference histograms not implemented.");
  return kFALSE;
}

//_______________________________________________________
void AliTRDrecoTask::SetDebugLevel(Int_t level)
{
// Generic debug handler

  fDebug = level;
  if(fDebug>=1){
    TDirectory *savedir = gDirectory;
    fgDebugStream = new TTreeSRedirector("TRD.DebugPerformance.root");
    savedir->cd();
  }
}

//____________________________________________________________________
void AliTRDrecoTask::Terminate(Option_t *)
{
  //
  // Terminate
  //

  if(fgDebugStream){ 
    delete fgDebugStream;
    fgDebugStream = 0x0;
    fDebug = 0;
  }
  if(HasPostProcess()) PostProcess();
}

//________________________________________________________
void AliTRDrecoTask::Adjust(TF1 *f, TH1 * const h)
{
// Helper function to avoid duplication of code
// Make first guesses on the fit parameters

  // find the intial parameters of the fit !! (thanks George)
  Int_t nbinsy = Int_t(.5*h->GetNbinsX());
  Double_t sum = 0.;
  for(Int_t jbin=nbinsy-4; jbin<=nbinsy+4; jbin++) sum+=h->GetBinContent(jbin); sum/=9.;
  f->SetParLimits(0, 0., 3.*sum);
  f->SetParameter(0, .9*sum);

  f->SetParLimits(1, -.2, .2);
  f->SetParameter(1, -0.1);

  f->SetParLimits(2, 0., 4.e-1);
  f->SetParameter(2, 2.e-2);
  if(f->GetNpar() <= 4) return;

  f->SetParLimits(3, 0., sum);
  f->SetParameter(3, .1*sum);

  f->SetParLimits(4, -.3, .3);
  f->SetParameter(4, 0.);

  f->SetParLimits(5, 0., 1.e2);
  f->SetParameter(5, 2.e-1);
}

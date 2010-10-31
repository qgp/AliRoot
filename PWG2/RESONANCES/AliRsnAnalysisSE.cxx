//
// Class AliRsnAnalysisSE
//
// Virtual Class derivated from AliRsnVAnalysisTaskSE which will be base class
// for all RSN SE tasks
//
// authors: Martin Vala (martin.vala@cern.ch)
//          Alberto Pulvirenti (alberto.pulvirenti@ct.infn.it)
//

#include <Riostream.h>
#include <TList.h>
#include "AliESDEvent.h"
#include "AliMCEvent.h"
#include "AliAODEvent.h"

#include "AliRsnCutSet.h"
#include "AliRsnVATProcessInfo.h"
#include "AliRsnAnalysisSE.h"

ClassImp(AliRsnAnalysisSE)

//_____________________________________________________________________________
AliRsnAnalysisSE::AliRsnAnalysisSE(const char *name, Bool_t useKine) :
  AliRsnVAnalysisTaskSE(name, useKine),
  fRsnAnalysisManager(),
  fEventCuts("eventCuts", AliRsnCut::kEvent),
  fOutList(0x0),
  fZeroEventPercentWarning(100),
  fUseZeroEventWarning(kTRUE)
{
//
// Default constructor.
//
  AliDebug(AliLog::kDebug+2, "<-");
  DefineOutput(2, TList::Class());
  AliDebug(AliLog::kDebug+2,"->");
}

//_____________________________________________________________________________
AliRsnAnalysisSE::AliRsnAnalysisSE(const AliRsnAnalysisSE& copy) :
  AliRsnVAnalysisTaskSE(copy),
  fRsnAnalysisManager(copy.fRsnAnalysisManager),
  fEventCuts(copy.fEventCuts),
  fOutList(0x0),
  fZeroEventPercentWarning(copy.fZeroEventPercentWarning),
  fUseZeroEventWarning(copy.fUseZeroEventWarning)
{
//
// Copy constructor.
//

  AliDebug(AliLog::kDebug+2,"<-");
  AliDebug(AliLog::kDebug+2,"->");
}

// //_____________________________________________________________________________
// AliRsnAnalysisSE::~AliRsnAnalysisSE()
// {
// //
// // Destructor
// //
// 
//   fOutList->Clear();
//   
// }

//_____________________________________________________________________________
void AliRsnAnalysisSE::RsnUserCreateOutputObjects()
{
//
// Creation of output objects.
// These are created through the utility methods in the analysis manager,
// which produces a list of histograms for each specified set of pairs.
// Each of these lists is added to the main list of this task.
//

  AliDebug(AliLog::kDebug+2,"<-");

  fOutList = new TList;
  fRsnAnalysisManager.InitAllPairs(fOutList);

  PostData(2, fOutList);

  AliDebug(AliLog::kDebug+2,"->");
}

//_____________________________________________________________________________
void AliRsnAnalysisSE::RsnUserExec(Option_t*)
{
//
// Execution of the analysis task.
// Recovers the input event and processes it with all included pair objects.
//

  AliDebug(AliLog::kDebug+2,"<-");
  
  fTaskInfo.SetEventUsed(kFALSE);

  if (fESDEvent) {
    AliDebug(AliLog::kDebug+1, Form("fESDEvent is %p", fESDEvent));
    AliDebug(AliLog::kDebug, Form("ESD tracks %d", fESDEvent->GetNumberOfTracks()));
  }
  if (fMCEvent) {
    AliDebug(AliLog::kDebug+1, Form("fMCEvent is %p", fMCEvent));
    AliDebug(AliLog::kDebug, Form("MC tracks %d", fMCEvent->GetNumberOfTracks()));
  }
  if (fAODEventIn) {
    AliDebug(AliLog::kDebug+1, Form("fAODEventIn is %p", fAODEventIn));
    AliDebug(AliLog::kDebug, Form("AOD(in) tracks %d", fAODEventIn->GetNumberOfTracks()));
  }
  if (fAODEventOut) {
    AliDebug(AliLog::kDebug+1, Form("fAODEventOut if %p", fAODEventOut));
    AliDebug(AliLog::kDebug, Form("AOD(out) tracks %d", fAODEventOut->GetNumberOfTracks()));
  }

  // Removing empty events
  if (fRsnEvent.GetMultiplicity()<=0) {
    AliDebug(AliLog::kDebug, "Zero event!!! Skipping ...");
    fTaskInfo.SetEventUsed(kFALSE);
    if (fUseZeroEventWarning)
    {
      TH1I *hist = (TH1I*)fInfoList->FindObject(fTaskInfo.GetEventHistogramName());
      if (!hist) return;
      Double_t zeroEventPercent = 0.0;
      if (hist->Integral() > 1) zeroEventPercent = (Double_t)hist->GetBinContent(1) / hist->Integral() * 100;
      if ((zeroEventPercent>fZeroEventPercentWarning)&&(fEntry>100))
        AliWarning(Form("%3.2f%% Events are with zero tracks (CurrentEvent=%d)!!!",zeroEventPercent,fEntry));
    }
    return;
  }

  // if general event cuts are added to the task (recommended)
  // they are checked here on the RSN event interface and,
  // if the event does not pass them, it is skipped and ProcessInfo
  // is updated accordingly
//   if (fEventCuts) {
    if (!fEventCuts.IsSelected(&fRsnEvent)) {
      fTaskInfo.SetEventUsed(kFALSE);
      return;
    }
//   }

  // if cuts are passed or not cuts were defined,
  // update the task info...
  fTaskInfo.SetEventUsed(kTRUE);

  // the virtual class has already sorted tracks in the PID index
  // so we need here just to call the execution of analysis
  if (!fMCOnly) fRsnAnalysisManager.ProcessAllPairs(&fRsnEvent, &fRsnEvent);
  else fRsnAnalysisManager.ProcessAllPairsMC(&fRsnEvent, &fRsnEvent);
  PostData(2, fOutList);

  AliDebug(AliLog::kDebug+2,"->");
}


//_____________________________________________________________________________
void AliRsnAnalysisSE::RsnTerminate(Option_t*)
{
//
// Termination.
// Could be added some monitor histograms here.
//

  AliDebug(AliLog::kDebug+2,"<-");
  AliDebug(AliLog::kDebug+2,"->");
}


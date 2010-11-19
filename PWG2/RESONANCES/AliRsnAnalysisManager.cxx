//
// Class AliRsnAnalysisManager
//
// This is the uppermost level of analysis objects collection.
// It contains a list of pair managers, which all will process
// a pool of events passed to this object, and fill their histograms.
//
// The utility of this object is to define a unique implementation
// of the whole processing, which can then be included in the different
// designs of AnalysisTask provided for SE and ME analysis.
//
// The base architecture is still AliRsnVManager, but in this case
// all the objects in the list will be AliRsnPairManager's.
//
// author     : M. Vala       [martin.vala@cern.ch]
// revised by : A. Pulvirenti [alberto.pulvirenti@ct.infn.it]
//

#include <TROOT.h>

#include "AliLog.h"
#include "AliStack.h"
#include "AliVEvent.h"
#include "AliMCEvent.h"
#include "AliRsnEvent.h"
#include "AliRsnPairFunctions.h"
#include "AliRsnPairNtuple.h"
#include "AliRsnAnalysisManager.h"


ClassImp(AliRsnAnalysisManager)

//_____________________________________________________________________________
AliRsnAnalysisManager::AliRsnAnalysisManager(const char*name) :
  TNamed(name, ""),
  fPairs(0),
  fGlobalTrackCuts()
{
//
// Default constructor
//
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::Add(AliRsnPair *pair)
{
//
// Adds a new pair manager to the list.
//

  AliDebug(AliLog::kDebug+2,"<-");

  if (!pair) 
  {
    AliWarning(Form("AliRsnPairManager is %p. Skipping ...", pair));
    return;
  }

  AliDebug(AliLog::kDebug+1, Form("Adding %s [%d]...", pair->GetName(), fPairs.GetEntries()));
  fPairs.Add(pair);

  AliDebug(AliLog::kDebug+2,"->");
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::Print(Option_t* /*dummy*/) const
{
//
// Overload of the TObject::Print() method
//

  AliInfo(Form("\t======== Analysis Manager %s ========", GetName()));
  PrintArray();
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::PrintArray() const
{
//
// Calls the "Print" method of all included pair managers
//

  AliDebug(AliLog::kDebug+2,"<-");

  AliRsnPair *pair = 0;
  TObjArrayIter next(&fPairs);
  while ((pair = (AliRsnPair*)next())) pair->Print();

  AliDebug(AliLog::kDebug+2,"->");
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::InitAllPairs(TList *list)
{
//
// Initialize all pair managers, and put all the TList of histograms
// generated by each one into a unique final output TList
//

  AliDebug(AliLog::kDebug+2,"<-");

//   TList *list = new TList();
//   list->SetName(GetName());
//   list->SetOwner();

  AliRsnPair   *pair = 0;
  TObjArrayIter next(&fPairs);
  Int_t i = 0;
  while ((pair = (AliRsnPair*)next())) 
  {
    if (!pair) continue;
    AliDebug(AliLog::kDebug+1, Form("InitAllPairs of the PairManager(%s) [%d] ...", pair->GetName(), i++));
    pair->Init("", list);
  }
  AliDebug(AliLog::kDebug+2, "->");
//   return list;
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::ProcessAllPairs(AliRsnEvent *ev0, AliRsnEvent *ev1)
{
//
// Process one or two events for all pair managers.
//

  AliDebug(AliLog::kDebug+2,"<-");
  
  if (!ev1) ev1 = ev0;
  
  Int_t nTot[2];
  nTot[0] = ev0->GetAbsoluteSum();
  nTot[1] = ev1->GetAbsoluteSum();;
  
  // external loop
  // joins the loop on tracks and v0s, by looping the indexes from 0
  // to the sum of them, and checking what to take depending of its value
  // in this step, the global cuts are checked
  Int_t          i0, i1, i, realIndex;
  AliRsnDaughter daughter0, daughter1;
  AliRsnPair    *pair = 0x0;
  TObjArrayIter  next(&fPairs);
  AliRsnDaughter::ERefType type;
  
  for (i0 = 0; i0 < nTot[0]; i0++)
  {
    // assign first track
    if (!ev0->ConvertAbsoluteIndex(i0, realIndex, type)) continue;
    ev0->SetDaughter(daughter0, realIndex, type);
    
    // check global cuts
    if (!fGlobalTrackCuts.IsSelected(&daughter0)) continue;
        
    // internal loop (same criterion)
    for (i1 = 0; i1 < nTot[1]; i1++)
    {
      // if looking same event, skip the case when the two indexes are equal
      if (ev0 == ev1 && i0 == i1) continue;
      
      // assign second track
      if (!ev1->ConvertAbsoluteIndex(i1, realIndex, type)) continue;
      ev1->SetDaughter(daughter1, realIndex, type);
      
      // check global cuts
      if (!fGlobalTrackCuts.IsSelected(&daughter1)) continue;
      
      // loop over all pairs and make computations
      next.Reset();
      i = 0;
      while ((pair = (AliRsnPair*)next())) 
      {
        AliDebug(AliLog::kDebug+1, Form("ProcessAllPairs of the AnalysisManager(%s) [%d] ...", pair->GetName(), i++));
        
        // if the pair is a like-sign, skip the case when i1 < i0,
        // in order not to double count each like-sign pair
        // (equivalent to looping from i0+1 to ntracks)
        if (pair->GetPairDef()->IsLikeSign() && i1 < i0) continue;
                
        // process the two tracks
        if (!pair->Fill(&daughter0, &daughter1, ev0, ev1)) continue;
        pair->Compute();
      }
    }
  }

  AliDebug(AliLog::kDebug+2,"->");
}

//_____________________________________________________________________________
void AliRsnAnalysisManager::ProcessAllPairsMC(AliRsnEvent *ev0, AliRsnEvent *ev1)
{
//
// Process one or two events for all pair managers.
//

  AliDebug(AliLog::kDebug+2,"<-");
  
  if (!ev1) ev1 = ev0;
  
  Int_t nTracks[2];
  nTracks[0] = ev0->GetRefMC()->GetNumberOfTracks();
  nTracks[1] = ev1->GetRefMC()->GetNumberOfTracks();
  
  // external loop
  // joins the loop on tracks and v0s, by looping the indexes from 0
  // to the sum of them, and checking what to take depending of its value
  Int_t          i0, i1, i;
  Bool_t         filled;
  AliRsnDaughter daughter0, daughter1;
  AliRsnPair    *pair = 0x0;
  TObjArrayIter  next(&fPairs);
  
  for (i0 = 0; i0 < nTracks[0]; i0++)
  {
    // skip not physical primaries
    if (!ev0->GetRefMCESD()->Stack()->IsPhysicalPrimary(i0)) continue;
    
    // assign first track
    ev0->SetDaughterMC(daughter0, i0);
        
    // internal loop (same criterion)
    for (i1 = 0; i1 < nTracks[1]; i1++)
    {
      // if looking same event, skip the case when the two indexes are equal
      if (ev0 == ev1 && i0 == i1) continue;
      
      // skip not physical primaries
      if (!ev1->GetRefMCESD()->Stack()->IsPhysicalPrimary(i1)) continue;
      
      // assign second track
      ev1->SetDaughterMC(daughter1, i1);
      
      // loop over all pairs and make computations
      next.Reset();
      i = 0;
      while ((pair = (AliRsnPair*)next())) 
      {
        AliDebug(AliLog::kDebug+1, Form("ProcessAllPairs of the AnalysisManager(%s) [%d] ...", pair->GetName(), i++));
        
        // if the pair is a like-sign, skip the case when i1 < i0,
        // in order not to double count each like-sign pair
        // (equivalent to looping from i0+1 to ntracks)
        if (pair->GetPairDef()->IsLikeSign() && i1 < i0) continue;
                
        // process the two tracks
        filled = pair->Fill(&daughter0, &daughter1, ev0, ev1);
        if (!filled) continue;
        pair->Compute();
      }
    }
  }

  AliDebug(AliLog::kDebug+2,"->");
}


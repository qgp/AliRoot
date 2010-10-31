/* $Id: AliPhysicsSelection.cxx 35782 2009-10-22 11:54:31Z jgrosseo $ */

/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

//-------------------------------------------------------------------------
//                      Implementation of   Class AliPhysicsSelection
// This class selects collision candidates from data runs, applying selection cuts on triggers 
// and background rejection based on the content of the ESD
//
// Usage:
//
// Create the object:
//   fPhysicsSelection = new AliPhysicsSelection;
//
// For MC data, call
//   fPhysicsSelection->SetAnalyzeMC()
//
// To check if an event is a collision candidate, use:
//   fPhysicsSelection->IsCollisionCandidate(fESD)
//
// After processing save the resulting histograms to a file with (a folder physics_selection 
//   will be created that contains the histograms):
//   fPhysicsSelection->SaveHistograms("physics_selection")
//
// To print statistics after processing use:
//   fPhysicsSelection->Print();
//
// The BX ids corresponding to real bunches crossings p2 are
// automatically selected. You cannot process runs with different
// filling schemes if this option is set. If you want to disable this,
// use: 
//   fPhysicsSelection->SetUseBXNumbers(0);
//
//
// If you are analizing muons and you want to keep the muon triggers
// besides the CINT1B you can set:
//   fPhysicsSelection->SetUseMuonTriggers();
//
//
// To compute the Background automatically using the control triggers
// use: 
//   fPhysicsSelection->SetComputeBG();
// this will show the value of the Beam Gas, accidentals and good
// events as additional rows in the statistic tables, but it will NOT
// subtract the background automatically.
// This option will only work for runs taken with the CINT1
// suite. This options enables automatically also the usage of BX
// numbers. You can only process one run at a time if you require this
// options, because it uses the bunch intensity estimated run by run.
// 
// The BG will usually be more important in the so-called "bin 0": the
// class can also compute the statistics table for events in this
// bin. Since the definition of bin 0 may in general change from
// analysis to analysis, the user needs to provide a callback
// implementing the definition of bin zero. The callback should be
// implemented as a method in the analysis task and should override
// the IsEventInBinZero method of AliAnalysisTaskSE, and should thus
// have the the following prototype:
//   Bool_t IsEventInBinZero(); 
// It should return true if the event is in the bin 0 and it is set by
// passing to the physics selection the NAME of the task where the
// callback is implemented: 
//   fPhysicsSelection->SetBin0Callback("MyTask").
//
//
// Usually the class selects the trigger scheme by itself depending on the run number.
// Nevertheless, you can do that manually by calling AddCollisionTriggerClass() and AddBGTriggerClass()
// Example:
// To define the class CINT1B-ABCE-NOPF-ALL as collision trigger (those will be accepted as  
// collision candidates when they pass the selection):
//   AddCollisionTriggerClass("+CINT1B-ABCE-NOPF-ALL #769 #3119");
// To select on bunch crossing IDs in addition, use:
//   AddCollisionTriggerClass("+CINT1B-ABCE-NOPF-ALL #769 #3119");
// To define the class CINT1A-ABCE-NOPF-ALL as a background trigger (those will only be counted
// for the control histograms):
//   AddBGTriggerClass("+CINT1A-ABCE-NOPF-ALL");
// You can also specify more than one trigger class in a string or you can require that some are *not*
// present. The following line would require CSMBA-ABCE-NOPF-ALL, but CSMBB-ABCE-NOPF-ALL is not allowed
// to be present:
//   AddBGTriggerClass("+CSMBA-ABCE-NOPF-ALL -CSMBB-ABCE-NOPF-ALL");
//
//   Origin: Jan Fiete Grosse-Oetringhaus, CERN 
//           Michele Floris, CERN
//-------------------------------------------------------------------------

#include <Riostream.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TList.h>
#include <TIterator.h>
#include <TDirectory.h>
#include <TObjArray.h>

#include <AliPhysicsSelection.h>

#include <AliTriggerAnalysis.h>
#include <AliLog.h>

#include <AliESDEvent.h>
#include <AliAnalysisTaskSE.h>
#include "AliAnalysisManager.h"
#include "TPRegexp.h"

ClassImp(AliPhysicsSelection)

AliPhysicsSelection::AliPhysicsSelection() :
  AliAnalysisCuts("AliPhysicsSelection", "AliPhysicsSelection"),
  fCurrentRun(-1),
  fMC(kFALSE),
  fCollTrigClasses(),
  fBGTrigClasses(),
  fTriggerAnalysis(),
  fBackgroundIdentification(0),
  fHistBunchCrossing(0),
  fHistTriggerPattern(0),
  fSkipTriggerClassSelection(0),
  fUsingCustomClasses(0),
  fSkipV0(0),
  fBIFactorA(1),
  fBIFactorC(1),
  fBIFactorAC(1), 
  fComputeBG(0),
  fBGStatOffset(0),
  fUseBXNumbers(1),
  fUseMuonTriggers(0),
  fFillingScheme(""),
  fBin0CallBack(""),
  fBin0CallBackPointer(0)
{
  // constructor
  
  fCollTrigClasses.SetOwner(1);
  fBGTrigClasses.SetOwner(1);
  fTriggerAnalysis.SetOwner(1);
  fHistStatistics[0] = 0;
  fHistStatistics[1] = 0;
  
  AliLog::SetClassDebugLevel("AliPhysicsSelection", AliLog::kWarning);
}
    
AliPhysicsSelection::~AliPhysicsSelection()
{
  // destructor

  fCollTrigClasses.Delete();
  fBGTrigClasses.Delete();
  fTriggerAnalysis.Delete();

  if (fHistStatistics[0])
  {
    delete fHistStatistics[0];
    fHistStatistics[0] = 0;
  }
  if (fHistStatistics[1])
  {
    delete fHistStatistics[1];
    fHistStatistics[1] = 0;
  }
  
  if (fHistBunchCrossing)
  {
    delete fHistBunchCrossing;
    fHistBunchCrossing = 0;
  }
  if (fHistTriggerPattern)
  {
    delete fHistTriggerPattern;
    fHistTriggerPattern = 0;
  }

}

UInt_t AliPhysicsSelection::CheckTriggerClass(const AliESDEvent* aEsd, const char* trigger) const
{
  // checks if the given trigger class(es) are found for the current event
  // format of trigger: +TRIGGER1 -TRIGGER2 [#XXX] [&YY]
  //   requires TRIGGER1 and rejects TRIGGER2
  //   in bunch crossing XXX
  //   if successful, YY is returned (for association between entry in fCollTrigClasses and AliVEvent::EOfflineTriggerTypes)
  
  Bool_t foundBCRequirement = kFALSE;
  Bool_t foundCorrectBC = kFALSE;
  
  UInt_t returnCode = AliVEvent::kUserDefined;
  
  TString str(trigger);
  TObjArray* tokens = str.Tokenize(" ");
  
  for (Int_t i=0; i < tokens->GetEntries(); i++)
  {
    TString str2(((TObjString*) tokens->At(i))->String());
    
    if (str2[0] == '+' || str2[0] == '-')
    {
      Bool_t flag = (str2[0] == '+');
      
      str2.Remove(0, 1);
      
      if (flag && !aEsd->IsTriggerClassFired(str2))
      {
        AliDebug(AliLog::kDebug, Form("Rejecting event because trigger class %s is not present", str2.Data()));
        delete tokens;
        return kFALSE;
      }
      if (!flag && aEsd->IsTriggerClassFired(str2))
      {
        AliDebug(AliLog::kDebug, Form("Rejecting event because trigger class %s is present", str2.Data()));
        delete tokens;
        return kFALSE;
      }
    }
    else if (str2[0] == '#')
    {
      foundBCRequirement = kTRUE;
    
      str2.Remove(0, 1);
      
      Int_t bcNumber = str2.Atoi();
      AliDebug(AliLog::kDebug, Form("Checking for bunch crossing number %d", bcNumber));
      
      if (aEsd->GetBunchCrossNumber() == bcNumber)
      {
        foundCorrectBC = kTRUE;
        AliDebug(AliLog::kDebug, Form("Found correct bunch crossing %d", bcNumber));
      }
    }
    else if (str2[0] == '&' && !fUsingCustomClasses)
    {
      str2.Remove(0, 1);
      
      returnCode = str2.Atoll();
    }
    else
      AliFatal(Form("Invalid trigger syntax: %s", trigger));
  }
  
  delete tokens;
  
  if (foundBCRequirement && !foundCorrectBC)
    return kFALSE;
  
  return returnCode;
}
    
UInt_t AliPhysicsSelection::IsCollisionCandidate(const AliESDEvent* aEsd)
{
  // checks if the given event is a collision candidate
  //
  // returns a bit word describing the fired offline triggers (see AliVEvent::EOfflineTriggerTypes)
  
  if (fCurrentRun != aEsd->GetRunNumber()) {
    if (!Initialize(aEsd->GetRunNumber()))
      AliFatal(Form("Could not initialize for run %d", aEsd->GetRunNumber()));
    if(fComputeBG) SetBIFactors(aEsd); // is this safe here?
  }
  const AliESDHeader* esdHeader = aEsd->GetHeader();
  if (!esdHeader)
  {
    AliError("ESD Header could not be retrieved");
    return kFALSE;
  }
  
  // check event type; should be PHYSICS = 7 for data and 0 for MC
  if (!fMC)
  {
    if (esdHeader->GetEventType() != 7)
      return kFALSE;
  }
  else
  {
    if (esdHeader->GetEventType() != 0)
      AliFatal(Form("Invalid event type for MC: %d", esdHeader->GetEventType()));
  }
  
  UInt_t accept = 0;
    
  Int_t count = fCollTrigClasses.GetEntries() + fBGTrigClasses.GetEntries();
  for (Int_t i=0; i < count; i++)
  {
    const char* triggerClass = 0;
    if (i < fCollTrigClasses.GetEntries())
      triggerClass = ((TObjString*) fCollTrigClasses.At(i))->String();
    else
      triggerClass = ((TObjString*) fBGTrigClasses.At(i - fCollTrigClasses.GetEntries()))->String();
  
    AliDebug(AliLog::kDebug, Form("Processing trigger class %s", triggerClass));
  
    AliTriggerAnalysis* triggerAnalysis = static_cast<AliTriggerAnalysis*> (fTriggerAnalysis.At(i));
  
    triggerAnalysis->FillTriggerClasses(aEsd);
    
    UInt_t singleTriggerResult = CheckTriggerClass(aEsd, triggerClass);
    if (singleTriggerResult)
    {
      triggerAnalysis->FillHistograms(aEsd);
  
      Bool_t isBin0 = kFALSE;
      if (fBin0CallBack != "") {
	AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
	if (!mgr) {
	  AliError("Cannot get the analysis manager");
	}  
	else {
	  isBin0 = ((AliAnalysisTaskSE*)mgr->GetTask(fBin0CallBack.Data()))->IsEventInBinZero();
	}
      } else if (fBin0CallBackPointer) {
	  isBin0 = (*fBin0CallBackPointer)(aEsd);
	
      }
      

      
      // hardware trigger (should only remove events for MC)
      // replay CINT1B hardware trigger
      // TODO this has to depend on the actual hardware trigger (and that depends on the run...)
      Int_t fastORHW = triggerAnalysis->SPDFiredChips(aEsd, 1); // SPD number of chips from trigger bits (!)
      Bool_t v0A       = fSkipV0 ? 0 :triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kV0A);
      Bool_t v0C       = fSkipV0 ? 0 :triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kV0C);
      Bool_t v0AHW     = fSkipV0 ? 0 :(triggerAnalysis->V0Trigger(aEsd, AliTriggerAnalysis::kASide, kTRUE) == AliTriggerAnalysis::kV0BB);// should replay hw trigger
      Bool_t v0CHW     = fSkipV0 ? 0 :(triggerAnalysis->V0Trigger(aEsd, AliTriggerAnalysis::kCSide, kTRUE) == AliTriggerAnalysis::kV0BB);// should replay hw trigger
      // offline trigger
      Int_t fastOROffline = triggerAnalysis->SPDFiredChips(aEsd, 0); // SPD number of chips from clusters (!)
      Bool_t v0ABG = fSkipV0 ? 0 :triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kV0ABG);
      Bool_t v0CBG = fSkipV0 ? 0 :triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kV0CBG);
      Bool_t v0BG = v0ABG || v0CBG;

      // fmd
      Bool_t fmdA =  triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kFMDA);
      Bool_t fmdC =  triggerAnalysis->IsOfflineTriggerFired(aEsd, AliTriggerAnalysis::kFMDC);
      Bool_t fmd  = fmdA || fmdC;
    
      // SSD
      Int_t ssdClusters = triggerAnalysis->SSDClusters(aEsd);

      // Some "macros"
      Bool_t mb1 = (fastOROffline > 0 || v0A || v0C) && (!v0BG);
      Bool_t mb1prime = (fastOROffline > 1 || (fastOROffline > 0 && (v0A || v0C)) || (v0A && v0C) ) && (!v0BG);

      // Background rejection
      Bool_t bgID = kFALSE;
      if (fBackgroundIdentification)
        bgID = ! fBackgroundIdentification->IsSelected(const_cast<AliESDEvent*> (aEsd));
      
      Int_t ntrig = fastOROffline; // any 2 hits
      if(v0A)              ntrig += 1;
      if(v0C)              ntrig += 1; //v0C alone is enough
      if(fmd)              ntrig += 1;
      if(ssdClusters>1)    ntrig += 1;


      Bool_t hwTrig = fastORHW > 0 || v0AHW || v0CHW;

      // Fill trigger pattern histo
      Int_t tpatt = 0;
      if (fastORHW>0) tpatt+=1;
      if (v0AHW)      tpatt+=2;
      if (v0CHW)      tpatt+=4;
      fHistTriggerPattern->Fill( tpatt );

      // fill statistics and return decision
      const Int_t nHistStat = 2;
      for(Int_t iHistStat = 0; iHistStat < nHistStat; iHistStat++){
	if (iHistStat == kStatIdxBin0 && !isBin0) continue; // skip the filling of bin0 stats if the event is not in the bin0
      
	fHistStatistics[iHistStat]->Fill(kStatTriggerClass, i);



	// We fill the rest only if hw trigger is ok
	if (!hwTrig)
	  {
	    AliDebug(AliLog::kDebug, "Rejecting event because hardware trigger is not fired");
	    continue;
	  } else {       
	  fHistStatistics[iHistStat]->Fill(kStatHWTrig, i);
	}
      

	// v0 BG stats
	if (v0ABG)
	  fHistStatistics[iHistStat]->Fill(kStatV0ABG, i);
	if (v0CBG)
	  fHistStatistics[iHistStat]->Fill(kStatV0CBG, i);

	// We fill the rest only if mb1 && ! v0BG
	if (mb1)
	  fHistStatistics[iHistStat]->Fill(kStatMB1, i);
	else continue;

	if (mb1prime)
	  fHistStatistics[iHistStat]->Fill(kStatMB1Prime, i);

	if (fmd)
	  fHistStatistics[iHistStat]->Fill(kStatFMD, i);

	if(ssdClusters>1)
	  fHistStatistics[iHistStat]->Fill(kStatSSD1, i);

	if(ntrig >= 2 && !v0BG) 
	  fHistStatistics[iHistStat]->Fill(kStatAny2Hits, i);

	if (fastOROffline > 0)
	  fHistStatistics[iHistStat]->Fill(kStatFO1, i);
	if (fastOROffline > 1)
	  fHistStatistics[iHistStat]->Fill(kStatFO2, i);
        
	if (v0A)
	  fHistStatistics[iHistStat]->Fill(kStatV0A, i);
	if (v0C)
	  fHistStatistics[iHistStat]->Fill(kStatV0C, i);
        
	//       if (fastOROffline > 1 && !v0BG)
	//         fHistStatistics[iHistStat]->Fill(kStatFO2NoBG, i);
            
	if (fastOROffline > 0 && (v0A || v0C) && !v0BG)
	  fHistStatistics[iHistStat]->Fill(kStatFO1AndV0, i);
  
	if (v0A && v0C && !v0BG && !bgID)
	  fHistStatistics[iHistStat]->Fill(kStatV0, i);


	if ( mb1 )
	
	  {
	    if (!v0BG || fSkipV0)
	      {
		if (!v0BG) fHistStatistics[iHistStat]->Fill(kStatOffline, i);
      
		if (fBackgroundIdentification && bgID)
		  {
		    AliDebug(AliLog::kDebug, "Rejecting event because of background identification");
		    fHistStatistics[iHistStat]->Fill(kStatBG, i);
		  }
		else
		  {
		    AliDebug(AliLog::kDebug, "Accepted event for histograms");
            
		    fHistStatistics[iHistStat]->Fill(kStatAccepted, i);
		    if(iHistStat == kStatIdxAll) fHistBunchCrossing->Fill(aEsd->GetBunchCrossNumber(), i); // Fill only for all (avoid double counting)
		    if((i < fCollTrigClasses.GetEntries() || fSkipTriggerClassSelection) && (iHistStat==kStatIdxAll))
		      accept |= singleTriggerResult; // only set for "all" (should not really matter)
		  }
	      }
	    else
	      AliDebug(AliLog::kDebug, "Rejecting event because of V0 BG flag");
	  }
	else
	  AliDebug(AliLog::kDebug, "Rejecting event because trigger condition is not fulfilled");
      }
    }
  }
 
  if (accept)
    AliDebug(AliLog::kDebug, Form("Accepted event as collision candidate with bit mask %d", accept));
  
  return accept;
}

Int_t AliPhysicsSelection::GetTriggerScheme(UInt_t runNumber) const
{
  // returns the current trigger scheme (classes that are accepted/rejected)
  
  if (fMC)
    return 0;
    
  // TODO dependent on run number
  
  switch (runNumber)
  {
    // CSMBB triggers
    case 104044:
    case 105054:
    case 105057:
      return 2;
  }
  
  // default: CINT1 suite
  return 1;
}  

const char * AliPhysicsSelection::GetFillingScheme(UInt_t runNumber)  {

  if(fMC) return "MC";

  if      (runNumber >= 104065 && runNumber <= 104160) {
    return "4x4a";
  } 
  else if (runNumber >= 104315 && runNumber <= 104321) {
    return "4x4a*";
  }
  else if (runNumber >= 104792 && runNumber <= 104803) {
    return "4x4b";
  }
  else if (runNumber >= 104824 && runNumber <= 104892) {
    return "4x4c";
  }
  else if (runNumber == 105143 || runNumber == 105160) {
    return "16x16a";
  }
  else if (runNumber >= 105256 && runNumber <= 105268) {
    return "4x4c";
  } 
  else if (runNumber >= 114786 && runNumber <= 116684) {
    return "Single_2b_1_1_1";
  }
  else if (runNumber >= 117048 && runNumber <= 117120) {
    return "Single_3b_2_2_2";
  }
  else if (runNumber >= 117220 && runNumber <= 119163) {
    return "Single_2b_1_1_1";
  }
  else if (runNumber >= 119837 && runNumber <= 119862) {
    return "Single_4b_2_2_2";
  }
  else if (runNumber >= 119902 && runNumber <= 120691) {
    return "Single_6b_3_3_3";
  }
  else if (runNumber >= 120741 && runNumber <= 122375) {
    return "Single_13b_8_8_8";
  }
  else if (runNumber >= 130148 && runNumber <= 130375) {
    return "125n_48b_36_16_36";
  } 
  else if (runNumber >= 130601 && runNumber <= 130640) {
    return "1000ns_50b_35_14_35";
  }
  else {
    AliError(Form("Unknown filling scheme (run %d)", runNumber));
  }

  return "Unknown";
}

const char * AliPhysicsSelection::GetBXIDs(UInt_t runNumber, const char * trigger)  {

  if (!fUseBXNumbers || fMC) return "";

  if      (runNumber >= 104065 && runNumber <= 104160) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #2128 #3019";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #346 #3465";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #1234 #1680";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  } 
  else if (runNumber >= 104315 && runNumber <= 104321) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #2000 #2891";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #218 #3337";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #1106 #1552";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber >= 104792 && runNumber <= 104803) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #2228 #3119";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #2554 #446";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #1334 #769";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber >= 104824 && runNumber <= 104892) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #3119 #769";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #2554 #446";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #1334 #2228";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber == 105143 || runNumber == 105160) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #1337 #1418 #2228 #2309 #3119 #3200 #446 #527";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #1580  #1742  #1904  #2066  #2630  #2792  #2954  #3362";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "  #845  #1007  #1169   #1577 #3359 #3521 #119  #281 ";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber >= 105256 && runNumber <= 105268) {
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #3019 #669";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #2454 #346";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #1234 #2128";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #790";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  } else if (runNumber >= 114786 && runNumber <= 116684) { // 7 TeV 2010, assume always the same filling scheme
    if     (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return " #346";
    else if(!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return " #2131";
    else if(!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return " #3019";
    else if(!strcmp("CINT1-E-NOPF-ALL",trigger))     return " #1238";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber >= 117048 && runNumber <= 117120) {
    //    return "Single_3b_2_2_2";
   if      (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return "   #346  #1240 ";
   else if (!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return "   #2131 ";
   else if (!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "   #3019 ";
   else if (!strcmp("CINT1-E-NOPF-ALL",trigger)) return " #1238";
   //   else AliError(Form("Unknown trigger: %s", trigger));

  }
  else if (runNumber >= 117220 && runNumber <= 119163) {
    //    return "Single_2b_1_1_1";
    if      (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return "   #346 ";
    else if (!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return "   #2131 ";
    else if (!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "   #3019 ";
    else if (!strcmp("CINT1-E-NOPF-ALL",trigger)) return " #1238 ";
    //    else AliError(Form("Unknown trigger: %s", trigger));						    
  }
  else if (runNumber >= 119837 && runNumber <= 119862) {
    //    return "Single_4b_2_2_2";
    if      (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return "   #669  #3019 ";
    else if (!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return "   #346  #2454 ";
    else if (!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "   #1234  #2128 ";
    else if (!strcmp("CINT1-E-NOPF-ALL",trigger)) return " #1681 #3463";
    //    else AliError(Form("Unknown trigger: %s", trigger));

  }
  else if (runNumber >= 119902 && runNumber <= 120691) {
    //    return "Single_6b_3_3_3";
    if      (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return "   #346  #546  #746 ";
    else if (!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return "   #2131  #2331  #2531 ";
    else if (!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "   #3019  #3219  #3419 ";
    else if (!strcmp("CINT1-E-NOPF-ALL",trigger)) return " #1296 #1670";
    //    else AliError(Form("Unknown trigger: %s", trigger));
  }
  else if (runNumber >= 120741 && runNumber <= 122375) {
    //    return "Single_13b_8_8_8";
    if      (!strcmp("CINT1B-ABCE-NOPF-ALL",trigger)) return "   #346  #446  #546  #646  #1240  #1340  #1440  #1540 ";
    else if (!strcmp("CINT1A-ABCE-NOPF-ALL",trigger)) return "   #946  #2131  #2231  #2331  #2431 ";
    else if (!strcmp("CINT1C-ABCE-NOPF-ALL",trigger)) return "   #3019  #3119  #3219  #3319  #3519 ";
    else if (!strcmp("CINT1-E-NOPF-ALL",trigger)) return " #1835 #2726";
    //    else AliError(Form("Unknown trigger: %s", trigger));
    
  } 
  else if (runNumber >= 130148 && runNumber <= 130375) {
    TString triggerString = trigger;
    static TString returnString = " ";
    returnString = "";
    if (triggerString.Contains("B")) returnString += "   #346  #396  #446  #496  #546  #596  #646  #696  #1240  #1290  #1340  #1390  #1440  #1490  #1540  #1590 ";
    if (triggerString.Contains("A")) returnString += "   #755  #805  #855  #905  #955  #1005  #1799  #1849  #1899  #2131  #2181  #2231  #2281  #2331  #2381  #2431  #2481  #2531  #2581  #2631  #2846  #3016  #3066  #3116  #3166  #3216  #3266  #3316  #3366  #3425  #3475  #3525 ";
    if (triggerString.Contains("C")) returnString += "   #3019  #3069  #3119  #3169  #3219  #3269  #3319  #3369  #14  #64  #114  #746  #796  #846  #908  #958  #1008  #1640  #1690  #1740  #2055  #2125  #2175  #2225  #2275  #2325  #2375  #2425  #2475  #2534  #2584  #2634 ";
    // Printf("0x%x",returnString.Data());
    // Printf("%s",returnString.Data());
    return returnString.Data();
  } 
  else if (runNumber >= 130601 && runNumber <= 130640) {
    TString triggerString = trigger;
    static TString returnString = " ";
    returnString = "";
    if (triggerString.Contains("B")) returnString += "  #346  #386  #426  #466  #506  #546  #586  #1240  #1280  #1320  #1360  #1400  #1440  #1480 ";
    if (triggerString.Contains("A")) returnString += "  #626  #666  #706  #746  #786  #826  #866  #1520  #1560  #1600  #1640  #1680  #1720  #1760  #2076  #2131  #2171  #2211  #2251  #2291  #2331  #2371  #2414  #2454  #2494  #2534  #2574  #2614  #2654  #2694  #2734  #2774  #2814 "; //#2854  #2894  #2934 not present in this run
    if (triggerString.Contains("C")) returnString += "  #3019  #3059  #3099  #3139  #3179  #3219  #3259  #3299  #3339  #3379  #3419  #3459  #3499  #3539  #115  #629  #669  #709  #749  #789  #829  #869  #909  #949  #989  #1029  #1069  #1109  #1149  #1523  #1563  #1603  #1643 "; //#1683  #1723  #1763 not present in this run
    return returnString.Data();
  }

  else {
    AliWarning(Form("Unknown run %d, using all BXs!",runNumber));
  }

  return "";
}
    
Bool_t AliPhysicsSelection::Initialize(Int_t runNumber)
{
  // initializes the object for the given run
  // TODO having the run number here and parameters hardcoded is clearly temporary, a way needs to be found to have a CDB-like configuration also for analysis
  
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  
  if(!fBin0CallBack) 
    AliError("Bin0 Callback not set: will not fill the statistics for the bin 0");

  if (fMC) {
    // override BX and bg options in case of MC
    fComputeBG    = kFALSE;
    fUseBXNumbers = kFALSE;
  }

  Int_t triggerScheme = GetTriggerScheme(runNumber);
  if (!fUsingCustomClasses && fCurrentRun != -1 && triggerScheme != GetTriggerScheme(fCurrentRun))
    AliFatal("Processing several runs with different trigger schemes is not supported");
  
  if(fComputeBG && fCurrentRun != -1 && fCurrentRun != runNumber) 
    AliFatal("Cannot process several runs because BG computation is requested");

  if(fComputeBG && !fUseBXNumbers) 
    AliFatal("Cannot compute BG if BX numbers are not used");
  
  if(fUseBXNumbers && fFillingScheme != "" && fFillingScheme != GetFillingScheme(runNumber))
    AliFatal("Cannot process runs with different filling scheme if usage of BX numbers is requested");

  fFillingScheme      = GetFillingScheme(runNumber);

  AliInfo(Form("Initializing for run %d", runNumber));
  
  // initialize first time?
  if (fCurrentRun == -1)
  {
    if (fUsingCustomClasses) {
      AliInfo("Using user-provided trigger classes");
    } else {
      switch (triggerScheme)
      {
      case 0:
        fCollTrigClasses.Add(new TObjString(Form("&%u", (UInt_t) AliVEvent::kMB)));
        break;
        
      case 1:
      	// trigger classes used before August 2010
        fCollTrigClasses.Add(new TObjString(Form("%s%s &%u","+CINT1B-ABCE-NOPF-ALL",  GetBXIDs(runNumber,"CINT1B-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CINT1A-ABCE-NOPF-ALL",  GetBXIDs(runNumber,"CINT1A-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CINT1C-ABCE-NOPF-ALL",  GetBXIDs(runNumber,"CINT1C-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CINT1-E-NOPF-ALL",      GetBXIDs(runNumber,"CINT1-E-NOPF-ALL"), (UInt_t) AliVEvent::kMB)));

        // Muon trigger have the same BXIDs of the corresponding CINT triggers
        fCollTrigClasses.Add(new TObjString(Form("%s%s &%u","+CMUS1B-ABCE-NOPF-MUON",  GetBXIDs(runNumber,"CINT1B-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMUON)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CMUS1A-ABCE-NOPF-MUON",  GetBXIDs(runNumber,"CINT1A-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMUON)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CMUS1C-ABCE-NOPF-MUON",  GetBXIDs(runNumber,"CINT1C-ABCE-NOPF-ALL"), (UInt_t) AliVEvent::kMUON)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u","+CMUS1-E-NOPF-MUON"    ,  GetBXIDs(runNumber,"CINT1-E-NOPF-ALL"), (UInt_t) AliVEvent::kMUON)));
        
        // triggers classes used from August 2010
        // MB
        fCollTrigClasses.Add(new TObjString(Form("%s%s &%u", "+CINT1-B-NOPF-ALLNOTRD" , GetBXIDs(runNumber, "B"),  (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CINT1-AC-NOPF-ALLNOTRD", GetBXIDs(runNumber, "AC"), (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CINT1-E-NOPF-ALLNOTRD" , GetBXIDs(runNumber, "E"),  (UInt_t) AliVEvent::kMB)));
        					  	 			      
	// MUON					  	 			      
        fCollTrigClasses.Add(new TObjString(Form("%s%s &%u", "+CMUS1-B-NOPF-ALLNOTRD" , GetBXIDs(runNumber, "B"),  (UInt_t) AliVEvent::kMUON)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CMUS1-AC-NOPF-ALLNOTRD", GetBXIDs(runNumber, "AC"), (UInt_t) AliVEvent::kMUON)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CMUS1-E-NOPF-ALLNOTRD" , GetBXIDs(runNumber, "E"),  (UInt_t) AliVEvent::kMUON)));
						  				     
	// High Multiplicity			  				     
        fCollTrigClasses.Add(new TObjString(Form("%s%s &%u", "+CSH1-B-NOPF-ALLNOTRD"  , GetBXIDs(runNumber, "B"),  (UInt_t) AliVEvent::kHighMult)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CSH1-AC-NOPF-ALLNOTRD" , GetBXIDs(runNumber, "AC"), (UInt_t) AliVEvent::kHighMult)));
        fBGTrigClasses.Add  (new TObjString(Form("%s%s &%u", "+CSH1-E-NOPF-ALLNOTRD"  , GetBXIDs(runNumber, "E"),  (UInt_t) AliVEvent::kHighMult)));

	// WARNING: IF YOU ADD MORE TRIGGER CLASSES, PLEASE CHECK THAT THE REGULAR EXPRESSION IN GetStatRow IS STILL VALID

        break;
        
      case 2:
        fCollTrigClasses.Add(new TObjString(Form("+CSMBB-ABCE-NOPF-ALL &%u", (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add(new TObjString(Form("+CSMBA-ABCE-NOPF-ALL -CSMBB-ABCE-NOPF-ALL &%u", (UInt_t) AliVEvent::kMB)));
        fBGTrigClasses.Add(new TObjString(Form("+CSMBC-ABCE-NOPF-ALL -CSMBB-ABCE-NOPF-ALL &%u", (UInt_t) AliVEvent::kMB)));
        break;
        
      case 3:
        // 
        break;
        
      default:
        AliFatal(Form("Unsupported trigger scheme %d", triggerScheme));
      }
    }
    
    Int_t count = fCollTrigClasses.GetEntries() + fBGTrigClasses.GetEntries();
    
    for (Int_t i=0; i<count; i++)
    {
      AliTriggerAnalysis* triggerAnalysis = new AliTriggerAnalysis;
      triggerAnalysis->SetAnalyzeMC(fMC);
      triggerAnalysis->EnableHistograms();
      triggerAnalysis->SetSPDGFOThreshhold(1);
      fTriggerAnalysis.Add(triggerAnalysis);
    }
      
    // TODO: shall I really delete this?
    if (fHistStatistics[0])
      delete fHistStatistics[0];
    if (fHistStatistics[1])
      delete fHistStatistics[1];
  
    fHistStatistics[kStatIdxBin0] = BookHistStatistics("_Bin0");
    fHistStatistics[kStatIdxAll]  = BookHistStatistics("");
    
    if (fHistBunchCrossing)
      delete fHistBunchCrossing;
  
    fHistBunchCrossing = new TH2F("fHistBunchCrossing", "fHistBunchCrossing;bunch crossing number;", 4000, -0.5, 3999.5,  count, -0.5, -0.5 + count);

    if (fHistTriggerPattern)
      delete fHistTriggerPattern;
    
    const int ntrig=3;
    Int_t n = 1;
    const Int_t nbinTrig = TMath::Nint(TMath::Power(2,ntrig));

    fHistTriggerPattern = new TH1F("fHistTriggerPattern", "Trigger pattern: FO + 2*v0A + 4*v0C", 
				   nbinTrig, -0.5, nbinTrig-0.5);    
    fHistTriggerPattern->GetXaxis()->SetBinLabel(1,"NO TRIG");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(2,"FO");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(3,"v0A");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(4,"FO & v0A");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(5,"v0C");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(6,"FO & v0C");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(7,"v0A & v0C");
    fHistTriggerPattern->GetXaxis()->SetBinLabel(8,"FO & v0A & v0C");

  
    n = 1;
    for (Int_t i=0; i < fCollTrigClasses.GetEntries(); i++)
    {
      fHistBunchCrossing->GetYaxis()->SetBinLabel(n, ((TObjString*) fCollTrigClasses.At(i))->String());
      n++;
    }
    for (Int_t i=0; i < fBGTrigClasses.GetEntries(); i++)
    {
      fHistBunchCrossing->GetYaxis()->SetBinLabel(n, ((TObjString*) fBGTrigClasses.At(i))->String());
      n++;
    }

    

  }
    
  Int_t count = fCollTrigClasses.GetEntries() + fBGTrigClasses.GetEntries();
  for (Int_t i=0; i<count; i++)
  {
    AliTriggerAnalysis* triggerAnalysis = static_cast<AliTriggerAnalysis*> (fTriggerAnalysis.At(i));
  
    switch (runNumber)
    {
      case 104315:
      case 104316:
      case 104320:
      case 104321:
      case 104439:
        triggerAnalysis->SetV0TimeOffset(7.5);
        break;
      default:
        triggerAnalysis->SetV0TimeOffset(0);
    }
  }
    
  fCurrentRun = runNumber;
  
  TH1::AddDirectory(oldStatus);
  
  return kTRUE;
}

TH2F * AliPhysicsSelection::BookHistStatistics(const char * tag) {
  // add 6 rows to count for the estimate of good, accidentals and
  // BG and the ratio of BG and accidentals to total +ratio goot to
  // first col + 2 for error on good.
  // TODO: Remember the the indexes of rows for the BG selection. Add new member fBGRows[] and use kStat as indexes

  Int_t count = fCollTrigClasses.GetEntries() + fBGTrigClasses.GetEntries();
#ifdef VERBOSE_STAT
  Int_t extrarows = fComputeBG != 0 ? 8 : 0;
#else
  Int_t extrarows = fComputeBG != 0 ? 3 : 0;
#endif
  TH2F * h = new TH2F(Form("fHistStatistics%s",tag), Form("fHistStatistics - %s ;;",tag), kStatAccepted, 0.5, kStatAccepted+0.5, count+extrarows, -0.5, -0.5 + count+extrarows);

  h->GetXaxis()->SetBinLabel(kStatTriggerClass,  "Trigger class");
  h->GetXaxis()->SetBinLabel(kStatHWTrig,	 "Hardware trigger");
  h->GetXaxis()->SetBinLabel(kStatFO1,	         "FO >= 1");
  h->GetXaxis()->SetBinLabel(kStatFO2,	         "FO >= 2");
  h->GetXaxis()->SetBinLabel(kStatV0A,	         "V0A");
  h->GetXaxis()->SetBinLabel(kStatV0C,	         "V0C");
  h->GetXaxis()->SetBinLabel(kStatFMD,	         "FMD");
  h->GetXaxis()->SetBinLabel(kStatSSD1,	         "SSD >= 2");
  h->GetXaxis()->SetBinLabel(kStatV0ABG,	 "V0A BG");
  h->GetXaxis()->SetBinLabel(kStatV0CBG,	 "V0C BG");
  h->GetXaxis()->SetBinLabel(kStatMB1,	         "(FO >= 1 | V0A | V0C) & !V0 BG");
  h->GetXaxis()->SetBinLabel(kStatMB1Prime,      "(FO >= 2 | (FO >= 1 & (V0A | V0C)) | (V0A &v0C) ) & !V0 BG");
  h->GetXaxis()->SetBinLabel(kStatFO1AndV0,	 "FO >= 1 & (V0A | V0C) & !V0 BG");
  h->GetXaxis()->SetBinLabel(kStatV0,	         "V0A & V0C & !V0 BG & !BG ID");
  h->GetXaxis()->SetBinLabel(kStatOffline,	 "Offline Trigger");
  h->GetXaxis()->SetBinLabel(kStatAny2Hits,	 "2 Hits & !V0 BG");
  h->GetXaxis()->SetBinLabel(kStatBG,	         "Background identification");
  h->GetXaxis()->SetBinLabel(kStatAccepted,      "Accepted");

  Int_t n = 1;
  for (Int_t i=0; i < fCollTrigClasses.GetEntries(); i++)
    {
      h->GetYaxis()->SetBinLabel(n, ((TObjString*) fCollTrigClasses.At(i))->String());
      n++;
    }
  for (Int_t i=0; i < fBGTrigClasses.GetEntries(); i++)
    {
      h->GetYaxis()->SetBinLabel(n, ((TObjString*) fBGTrigClasses.At(i))->String());
      n++;
    }

  if(fComputeBG) {
    fBGStatOffset = n;
    h->GetYaxis()->SetBinLabel(n++, Form("BG (A+C) (Mask [0x%x])", fComputeBG));
    h->GetYaxis()->SetBinLabel(n++, "ACC");
#ifdef VERBOSE_STAT
    h->GetYaxis()->SetBinLabel(n++, "BG (A+C) %  (rel. to CINT1B)");
    h->GetYaxis()->SetBinLabel(n++, "ACC % (rel. to CINT1B)");
    h->GetYaxis()->SetBinLabel(n++, "ERR GOOD %");
    h->GetYaxis()->SetBinLabel(n++, "GOOD % (rel. to 1st col)");
    h->GetYaxis()->SetBinLabel(n++, "ERR GOOD");
#endif
    h->GetYaxis()->SetBinLabel(n++, "GOOD");
  }

  return h;
}

void AliPhysicsSelection::Print(Option_t* /* option */) const
{
  // print the configuration
  
  Printf("Configuration initialized for run %d (MC: %d):", fCurrentRun, fMC);
  
  Printf("Collision trigger classes:");
  for (Int_t i=0; i < fCollTrigClasses.GetEntries(); i++)
    Printf("%s", ((TObjString*) fCollTrigClasses.At(i))->String().Data());
  
  Printf("Background trigger classes:");
  for (Int_t i=0; i < fBGTrigClasses.GetEntries(); i++)
    Printf("%s", ((TObjString*) fBGTrigClasses.At(i))->String().Data());

  AliTriggerAnalysis* triggerAnalysis = dynamic_cast<AliTriggerAnalysis*> (fTriggerAnalysis.At(0));
  
  if (triggerAnalysis)
  {
    if (triggerAnalysis->GetV0TimeOffset() > 0)
      Printf("V0 time offset active: %.2f ns", triggerAnalysis->GetV0TimeOffset());
    
    Printf("\nTotal available events:");
    
    triggerAnalysis->PrintTriggerClasses();
  }
  
  if (fHistStatistics[kStatIdxAll])
  {
    for (Int_t i=0; i<fCollTrigClasses.GetEntries(); i++)
    {
      Printf("\nSelection statistics for collision trigger %s:", ((TObjString*) fCollTrigClasses.At(i))->String().Data());
      
      Printf("Total events with correct trigger class: %d", (Int_t) fHistStatistics[kStatIdxAll]->GetBinContent(1, i+1));
      Printf("Selected collision candidates: %d", (Int_t) fHistStatistics[kStatIdxAll]->GetBinContent(fHistStatistics[kStatIdxAll]->GetXaxis()->FindBin("Accepted"), i+1));
    }
  }
  
  if (fHistBunchCrossing)
  {
    Printf("\nBunch crossing statistics:");
    
    for (Int_t i=1; i<=fHistBunchCrossing->GetNbinsY(); i++)
    {
      TString str;
      str.Form("Trigger %s has accepted events in the bunch crossings: ", fHistBunchCrossing->GetYaxis()->GetBinLabel(i));
      
      for (Int_t j=1; j<=fHistBunchCrossing->GetNbinsX(); j++)
        if (fHistBunchCrossing->GetBinContent(j, i) > 0)
          str += Form("%d, ", (Int_t) fHistBunchCrossing->GetXaxis()->GetBinCenter(j));
       
      Printf("%s", str.Data());
    }
    
    for (Int_t j=1; j<=fHistBunchCrossing->GetNbinsX(); j++)
    {
      Int_t countColl = 0;
      Int_t countBG = 0;
      for (Int_t i=1; i<=fHistBunchCrossing->GetNbinsY(); i++)
      {
        if (fHistBunchCrossing->GetBinContent(j, i) > 0)
        {
          if (fCollTrigClasses.FindObject(fHistBunchCrossing->GetYaxis()->GetBinLabel(i)))
            countColl++;
          if (fBGTrigClasses.FindObject(fHistBunchCrossing->GetYaxis()->GetBinLabel(i)))
            countBG++;
        }
      }
      if (countColl > 0 && countBG > 0)
        Printf("WARNING: Bunch crossing %d has collision and BG trigger classes active. Check BPTX functioning for this run!", (Int_t) fHistBunchCrossing->GetXaxis()->GetBinCenter(j));
    }
  }

  if (fUsingCustomClasses)        
    Printf("WARNING: Using custom trigger classes!");
  if (fSkipTriggerClassSelection) 
    Printf("WARNING: Skipping trigger class selection!");
  if (fSkipV0) 
    Printf("WARNING: Ignoring V0 information in selection");
  if(!fBin0CallBack) 
    Printf("WARNING: Callback not set: will not fill the statistics for the bin 0");

}

Long64_t AliPhysicsSelection::Merge(TCollection* list)
{
  // Merge a list of AliMultiplicityCorrection objects with this (needed for
  // PROOF).
  // Returns the number of merged objects (including this).

  if (!list)
    return 0;

  if (list->IsEmpty())
    return 1;

  TIterator* iter = list->MakeIterator();
  TObject* obj;
  
  // collections of all histograms
  const Int_t nHists = 9;
  TList collections[nHists];

  Int_t count = 0;
  while ((obj = iter->Next())) {

    AliPhysicsSelection* entry = dynamic_cast<AliPhysicsSelection*> (obj);
    if (entry == 0) 
      continue;
    // Update run number. If this one is not initialized (-1) take the one from 
    // the next physics selection to be merged with. In case of 2 different run
    // numbers issue a warning (should physics selections from different runs be 
    // merged together) A.G.
    Int_t currentRun = entry->GetCurrentRun();
    // Nothing to merge with since run number was not initialized.
    if (currentRun < 0) continue;
    if (fCurrentRun < 0) fCurrentRun = currentRun;
    if (fCurrentRun != currentRun)
       AliWarning(Form("Current run %d not matching the one to be merged with %d", fCurrentRun, currentRun));
    
    collections[0].Add(&(entry->fTriggerAnalysis));
    if (entry->fHistStatistics[0])
      collections[1].Add(entry->fHistStatistics[0]);
    if (entry->fHistStatistics[1])
      collections[2].Add(entry->fHistStatistics[1]);
    if (entry->fHistBunchCrossing)
      collections[3].Add(entry->fHistBunchCrossing);
    if (entry->fHistTriggerPattern)
      collections[4].Add(entry->fHistTriggerPattern);
    if (entry->fBackgroundIdentification)
      collections[5].Add(entry->fBackgroundIdentification);

    count++;
  }

  fTriggerAnalysis.Merge(&collections[0]);
  if (fHistStatistics[0])
    fHistStatistics[0]->Merge(&collections[1]);
  if (fHistStatistics[1])
    fHistStatistics[1]->Merge(&collections[2]);
  if (fHistBunchCrossing)
    fHistBunchCrossing->Merge(&collections[3]);
  if (fHistTriggerPattern)
    fHistTriggerPattern->Merge(&collections[4]);
  if (fBackgroundIdentification)
    fBackgroundIdentification->Merge(&collections[5]);
  
  delete iter;

  return count+1;
}

void AliPhysicsSelection::SaveHistograms(const char* folder) const
{
  // write histograms to current directory
  
  if (!fHistStatistics[0] || !fHistStatistics[1])
    return;
    
  if (folder)
  {
    gDirectory->mkdir(folder);
    gDirectory->cd(folder);
  }
  

  // Fill the last rows of fHistStatistics before saving
  if (fComputeBG) {
    Int_t triggerScheme = GetTriggerScheme(UInt_t(fCurrentRun));
    if(triggerScheme != 1){
      AliWarning("BG estimate only supported for trigger scheme \"1\" (CINT1 suite)");
    } else if (fUseMuonTriggers) {
      AliWarning("BG estimate with muon triggers to be implemented");
    } else {      
      AliInfo("BG estimate assumes that for a given run you only have A and C triggers separately or"
	      " toghether as a AC class! Make sure this assumption holds in your case"); 
      
      // use an anum for the different trigger classes, to make loops easier to read
      enum {kClassB =0 , kClassA, kClassC, kClassAC, kClassE, kNClasses};
      const char * classFlags[] = {"B", "A", "C", "AC", "E"}; // labels

      UInt_t * rows[kNClasses] = {0}; // Array of matching rows
      Int_t nrows[kNClasses] = {0};
      // Get rows matching the requested trigger bits for all trigger classes
      for(Int_t iTrigClass = 0; iTrigClass < kNClasses; iTrigClass++){
	nrows[iTrigClass] = GetStatRow(classFlags[iTrigClass],fComputeBG,&rows[iTrigClass]);	
      }

      // 0. Determine the ratios of triggers E/B, A/B, C/B from the stat histogram
      // Those are used to rescale the different classes to the same number of bx ids
      // TODO: pass names of the rows for B, CA and E and look names of the rows. How do I handle the case in which both AC are in the same row?         
      Int_t nBXIds[kNClasses] = {0};
      cout <<"Computing BG:" << endl;
      
      for(Int_t iTrigClass = 0; iTrigClass < kNClasses; iTrigClass++){
	for(Int_t irow = 0; irow < nrows[iTrigClass]; irow++) {
	  if(irow==0) cout << "- Class " << classFlags[iTrigClass] << endl;   
	  for (Int_t j=1; j<=fHistBunchCrossing->GetNbinsX(); j++) {
	    if (fHistBunchCrossing->GetBinContent(j, rows[iTrigClass][irow]) > 0) {
	      nBXIds[iTrigClass]++;	 
	    }
	  }
	  if(nBXIds[iTrigClass]>0) cout << "   Using row " << rows[iTrigClass][irow] <<  ": " 
					<< fHistBunchCrossing->GetYaxis()->GetBinLabel(rows[iTrigClass][irow]) 
					<< " (nBXID "<< nBXIds[iTrigClass] << ")"<< endl;

	}

      }

      Float_t ratioToB[kNClasses];
      ratioToB[kClassE]  = nBXIds[kClassE]  >0 ? Float_t(nBXIds[kClassB])/nBXIds[kClassE]   : 0;
      ratioToB[kClassA]  = nBXIds[kClassA]  >0 ? Float_t(nBXIds[kClassB])/nBXIds[kClassA]   : 0;
      ratioToB[kClassC]  = nBXIds[kClassC]  >0 ? Float_t(nBXIds[kClassB])/nBXIds[kClassC]   : 0;
      ratioToB[kClassAC] = nBXIds[kClassAC] >0 ? Float_t(nBXIds[kClassB])/nBXIds[kClassAC]  : 0;
      Printf("Ratio between the BX ids in the different trigger classes:");
      Printf("  B/E  = %d/%d = %f", nBXIds[kClassB],nBXIds[kClassE], ratioToB[kClassE] );
      Printf("  B/A  = %d/%d = %f", nBXIds[kClassB],nBXIds[kClassA], ratioToB[kClassA] );
      Printf("  B/C  = %d/%d = %f", nBXIds[kClassB],nBXIds[kClassC], ratioToB[kClassC] );
      Printf("  B/AC = %d/%d = %f", nBXIds[kClassB],nBXIds[kClassAC],ratioToB[kClassAC]);
      Int_t nHistStat = 2;

      // 1. loop over all cols
      for(Int_t iHistStat = 0; iHistStat < nHistStat; iHistStat++){
	Int_t ncol = fHistStatistics[iHistStat]->GetNbinsX();
	Float_t good1 = 0;      
	for(Int_t icol = 1; icol <= ncol; icol++) {
	  Int_t nEvents[kNClasses] = {0}; // number of events should be reset at every column
	  // For all trigger classes, add up over row matching trigger mask (as selected before)
	  for(Int_t iTrigClass = 0; iTrigClass < kNClasses; iTrigClass++){
	    for(Int_t irow = 0; irow < nrows[iTrigClass]; irow++) {	  
	      nEvents[iTrigClass] += (Int_t) fHistStatistics[iHistStat]->GetBinContent(icol,rows[iTrigClass][irow]);	
	    }
	    //	    cout << "Events " << classFlags[iTrigClass] << " ("<<icol<<") " << nEvents[iTrigClass] << endl;	    
	  }
	  if (nEvents[kClassB]>0) {
	    Float_t acc  = ratioToB[kClassE]*nEvents[kClassE]; 
	    Double_t acc_err = TMath::Sqrt(ratioToB[kClassE]*ratioToB[kClassE]*nEvents[kClassE]);
	    //      Int_t bg   = cint1A + cint1C - 2*acc;
	    
	     // Assuming that for a given class the triggers are either recorded as A+C or AC
	    Float_t bg   = nEvents[kClassAC] > 0 ?
	      fBIFactorAC*(ratioToB[kClassAC]*nEvents[kClassAC] - 2*acc):
	      fBIFactorA* (ratioToB[kClassA]*nEvents[kClassA]-acc) + 
	      fBIFactorC* (ratioToB[kClassC]*nEvents[kClassC]-acc) ;

	    // cout << "-----------------------" << endl;
	    // cout << "Factors: " << fBIFactorA << " " << fBIFactorC << " " << fBIFactorAC << endl;
	    // cout << "Ratios: "  << ratioToB[kClassA] << " " << ratioToB[kClassC] << " " << ratioToB[kClassAC] << endl;
	    // cout << "Evts:   "  << nEvents[kClassA] << " " << nEvents[kClassC] << " " << nEvents[kClassAC] << " " <<  nEvents[kClassB] << endl;
	    // cout << "Acc: " << acc << endl;
	    // cout << "BG: " << bg << endl;
	    // cout  << "  " <<   fBIFactorA* (ratioToB[kClassA]*nEvents[kClassA]-acc) <<endl;
	    // cout  << "  " <<   fBIFactorC* (ratioToB[kClassC]*nEvents[kClassC]-acc) <<endl;
	    // cout  << "  " <<   fBIFactorAC*(ratioToB[kClassAC]*nEvents[kClassAC] - 2*acc) << endl;
	    // cout << "-----------------------" << endl;

	    Float_t good = Float_t(nEvents[kClassB]) - bg - acc;
	    if (icol ==1) good1 = good;
	    //      Float_t errGood     = TMath::Sqrt(2*(nEvents[kClassA]+nEvents[kClassC]+nEvents[kClassE]));// Error on the number of goods assuming only bg fluctuates
	    //      DeltaG^2 = B + FA^2 A + FC^2 C + Ratio^2 (FA+FC-1)^2 E.
	    Float_t errGood     = nEvents[kClassAC] > 0 ? 
	      TMath::Sqrt( nEvents[kClassB] +
			   fBIFactorAC*fBIFactorAC*ratioToB[kClassAC]*ratioToB[kClassAC]*nEvents[kClassAC] +
			   ratioToB[kClassE] * ratioToB[kClassE] * 
			   (fBIFactorAC - 1)*(fBIFactorAC - 1)*nEvents[kClassE]) :  
	      TMath::Sqrt( nEvents[kClassB] + 
			   fBIFactorA*fBIFactorA*ratioToB[kClassA]*ratioToB[kClassA]*nEvents[kClassA] +
			   fBIFactorC*fBIFactorC*ratioToB[kClassC]*ratioToB[kClassC]*nEvents[kClassC] +
			   ratioToB[kClassE] * ratioToB[kClassE] * 
			   (fBIFactorA + fBIFactorC - 1)*(fBIFactorA + fBIFactorC - 1)*nEvents[kClassE]);

	    Float_t errBG = nEvents[kClassAC] > 0 ? 
	      TMath::Sqrt(fBIFactorAC*fBIFactorAC*ratioToB[kClassAC]*ratioToB[kClassAC]*nEvents[kClassAC]+
			  4*ratioToB[kClassE]*ratioToB[kClassE]*(fBIFactorAC*fBIFactorAC)*nEvents[kClassE]) :
	      TMath::Sqrt(fBIFactorA*fBIFactorA*ratioToB[kClassA]*ratioToB[kClassA]*nEvents[kClassA]+
			  fBIFactorC*fBIFactorC*ratioToB[kClassC]*ratioToB[kClassC]*nEvents[kClassC]+
			  ratioToB[kClassE]*ratioToB[kClassE]*(fBIFactorA+fBIFactorC)*(fBIFactorA+fBIFactorC)*nEvents[kClassE]);
	
	
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowBG,bg);	
	    fHistStatistics[iHistStat]->SetBinError  (icol,fBGStatOffset+kStatRowBG,errBG);	
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowAcc,acc);	
	    fHistStatistics[iHistStat]->SetBinError  (icol,fBGStatOffset+kStatRowAcc,acc_err);	
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowGood,good);    
	    fHistStatistics[iHistStat]->SetBinError  (icol,fBGStatOffset+kStatRowGood,errGood);    

#ifdef VERBOSE_STAT
	    //kStatRowBG=0,kStatRowAcc,kStatRowBGFrac,kStatRowAccFrac,kStatRowErrGoodFrac,kStatRowGoodFrac,kStatRowGood,kStatRowErrGood
	    Float_t accFrac   = Float_t(acc) / nEvents[kClassB]  *100;
	    Float_t errAccFrac= Float_t(acc_err) / nEvents[kClassB]  *100;
	    Float_t bgFrac    = Float_t(bg)  / nEvents[kClassB]  *100;
	    Float_t goodFrac  = Float_t(good)  / good1 *100;
	    Float_t errGoodFrac = errGood/good1 * 100;
	    Float_t errFracBG = bg > 0 ? TMath::Sqrt((errBG/bg)*(errBG/bg) + 1/nEvents[kClassB])*bgFrac : 0;
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowBGFrac,bgFrac);	
	    fHistStatistics[iHistStat]->SetBinError  (icol,fBGStatOffset+kStatRowBGFrac,errFracBG);	
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowAccFrac,accFrac);    
	    fHistStatistics[iHistStat]->SetBinError  (icol,fBGStatOffset+kStatRowAccFrac,errAccFrac);    
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowGoodFrac,goodFrac);    
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowErrGoodFrac,errGoodFrac);    
	    fHistStatistics[iHistStat]->SetBinContent(icol,fBGStatOffset+kStatRowErrGood,errGood);    
#endif
	  }
	}
      }
    }  
  }

  fHistStatistics[0]->Write();
  fHistStatistics[1]->Write();
  if(fHistBunchCrossing ) fHistBunchCrossing ->Write();
  if(fHistTriggerPattern) fHistTriggerPattern->Write();
  
  Int_t count = fCollTrigClasses.GetEntries() + fBGTrigClasses.GetEntries();
  for (Int_t i=0; i < count; i++)
  {
    TString triggerClass = "trigger_histograms_";
    if (i < fCollTrigClasses.GetEntries())
      triggerClass += ((TObjString*) fCollTrigClasses.At(i))->String();
    else
      triggerClass += ((TObjString*) fBGTrigClasses.At(i - fCollTrigClasses.GetEntries()))->String();
  
    gDirectory->mkdir(triggerClass);
    gDirectory->cd(triggerClass);
  
    static_cast<AliTriggerAnalysis*> (fTriggerAnalysis.At(i))->SaveHistograms();
    
    gDirectory->cd("..");
  }
 
  if (fBackgroundIdentification)
  {
    gDirectory->mkdir("background_identification");
    gDirectory->cd("background_identification");
      
    fBackgroundIdentification->GetOutput()->Write();
      
    gDirectory->cd("..");
  }
  
  if (folder)
    gDirectory->cd("..");
}

Int_t AliPhysicsSelection::GetStatRow(const char * triggerBXClass, UInt_t offlineTriggerType, UInt_t ** rowIDs) const {
  // Puts inside the array rowIDs the row number for a given offline
  // trigger in a given bx class. Returns the total number of lines
  // matching the selection
  // triggerBXClass can be either "A", "AC", "B" or "E"
  // offlineTriggerType is one of the types defined in AliVEvent
  // User should delete rowIDs if no longer needed

  if(!fHistStatistics[0]) {
    AliWarning("Not initialized, returning 0");
    return 0;
  }
  const Int_t nrows = fHistStatistics[0]->GetNbinsY();

  // allocate memory for at maximum nrows
  Int_t nMatches = 0;
  (*rowIDs) = new UInt_t[nrows];

  // Build regular expression. look for a +, followed by the beginning
  // of a word. Within the word, look for the class id after any
  // number of any char, but at most one dash ("[^-]*-?"), followed by
  // a - and then any char (".*") and at the class id ("\\&(\\d)")
  // The class id is stored.
  // WARNING: please check this if the trigger classes change
  TPRegexp re(Form("\\+\\b[^-]*-?%s-.*\\&(\\d)",triggerBXClass));  
  // Loop over rows and find matching ones:
  for(Int_t irow = 1; irow <= nrows; irow++){
    TObjArray * matches = re.MatchS(fHistStatistics[0]->GetYaxis()->GetBinLabel(irow));
    if (matches->GetEntries()) {
      TString s = ((TObjString*)matches->At(1))->GetString();      
      if(UInt_t(s.Atoi()) & offlineTriggerType) { // bitwise comparison with the requested mask
	//	cout << "Marching " << s.Data() << " " << offlineTriggerType << " " << fHistStatistics[0]->GetYaxis()->GetBinLabel(irow) << endl;	
	(*rowIDs)[nMatches] = irow;
	nMatches++;	
      }
    }
    delete matches;
  }
  
  return nMatches;
}

void AliPhysicsSelection::SetBIFactors(const AliESDEvent * aESD) {
  // Set factors for realtive bunch intesities
  if(!aESD) { 
    AliFatal("ESD not given");
  }
  Int_t run = aESD->GetRunNumber();
  if (run > 105268) {
    // intensities stored in the ESDs
    const AliESDRun* esdRun = aESD->GetESDRun();
    Double_t intAB = esdRun->GetMeanIntensityIntecting(0);
    Double_t intCB = esdRun->GetMeanIntensityIntecting(1);
    Double_t intAA = esdRun->GetMeanIntensityNonIntecting(0);
    Double_t intCC = esdRun->GetMeanIntensityNonIntecting(1);

    // cout << "INT " <<intAB <<endl;
    // cout << "INT " <<intCB <<endl;
    // cout << "INT " <<intAA <<endl;
    // cout << "INT " <<intCC <<endl;

    if (intAB > -1 && intAA > -1) {
      fBIFactorA = intAB/intAA;
    } else {
      AliWarning("Cannot set fBIFactorA, assuming 1");
    }
    
    if (intCB > -1 && intCC > -1) {
      fBIFactorC = intCB/intCC;
    } else {
      AliWarning("Cannot set fBIFactorC, assuming 1");
    }
      
    if (intAB > -1 && intAA > -1 &&
	intCB > -1 && intCC > -1) {
      fBIFactorAC = (intAB+intCB)/(intAA+intCC);
    } else {
      AliWarning("Cannot set fBIFactorAC, assuming 1");
    }
        
  }  
  else {
    // First runs. Intensities hardcoded
    switch(run) {
    case 104155:
      fBIFactorA = 0.961912722908;
      fBIFactorC = 1.04992336081;
      break;
    case 104157:
      fBIFactorA = 0.947312854998;
      fBIFactorC = 1.01599706417;
      break;
    case 104159:
      fBIFactorA = 0.93659320151;
      fBIFactorC = 0.98580804207;
      break;
    case 104160:
      fBIFactorA = 0.929664189926;
      fBIFactorC = 0.963467679851;
      break;
    case 104315:
      fBIFactorA = 1.08939104979;
      fBIFactorC = 0.931113921925;
      break;
    case 104316:
      fBIFactorA = 1.08351880974;
      fBIFactorC = 0.916068345845;
      break;
    case 104320:
      fBIFactorA = 1.07669281245;
      fBIFactorC = 0.876818744763;
      break;
    case 104321:
      fBIFactorA = 1.00971079602;
      fBIFactorC = 0.773781299076;
      break;
    case 104792:
      fBIFactorA = 0.787215863962;
      fBIFactorC = 0.778253173071;
      break;
    case 104793:
      fBIFactorA = 0.692211363661;
      fBIFactorC = 0.733152456667;
      break;
    case 104799:
      fBIFactorA = 1.04027825161;
      fBIFactorC = 1.00530825942;
      break;
    case 104800:
      fBIFactorA = 1.05309910671;
      fBIFactorC = 1.00376801855;
      break;
    case 104801:
      fBIFactorA = 1.0531231922;
      fBIFactorC = 0.992439666758;
      break;
    case 104802:
      fBIFactorA = 1.04191478134;
      fBIFactorC = 0.979368585208;
      break;
    case 104803:
      fBIFactorA = 1.03121314094;
      fBIFactorC = 0.973379962609;
      break;
    case 104824:
      fBIFactorA = 0.969945926722;
      fBIFactorC = 0.39549745806;
      break;
    case 104825:
      fBIFactorA = 0.968627213937;
      fBIFactorC = 0.310100412205;
      break;
    case 104841:
      fBIFactorA = 0.991601393212;
      fBIFactorC = 0.83762204722;
      break;
    case 104845:
      fBIFactorA = 0.98040863886;
      fBIFactorC = 0.694824205793;
      break;
    case 104867:
      fBIFactorA = 1.10646173412;
      fBIFactorC = 0.841407246916;
      break;
    case 104876:
      fBIFactorA = 1.12063452421;
      fBIFactorC = 0.78726542895;
      break;
    case 104890:
      fBIFactorA = 1.02346137453;
      fBIFactorC = 1.03355663595;
      break;
    case 104892:
      fBIFactorA = 1.05406025913;
      fBIFactorC = 1.00029166135;
      break;
    case 105143:
      fBIFactorA = 0.947343384349;
      fBIFactorC = 0.972637444408;
      break;
    case 105160:
      fBIFactorA = 0.908854622177;
      fBIFactorC = 0.958851103977;
      break; 
    case 105256:
      fBIFactorA = 0.810076150206;
      fBIFactorC = 0.884663561883;
      break;
    case 105257:
      fBIFactorA = 0.80974912303;
      fBIFactorC = 0.878859123479;
      break;
    case 105268:
      fBIFactorA = 0.809052110679;
      fBIFactorC = 0.87233890989;
      break;
    default:
      fBIFactorA = 1;
      fBIFactorC = 1;
    }
  }

}

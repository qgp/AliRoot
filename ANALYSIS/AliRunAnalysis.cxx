#include "AliRunAnalysis.h"
//________________________________
///////////////////////////////////////////////////////////
//
// class AliRunAnalysis
//
//
//
// Piotr.Skowronski@cern.ch
//
///////////////////////////////////////////////////////////

#include <stdlib.h>

#include <TString.h>
#include <TObjString.h>
#include <TClass.h>
#include <TFile.h>
#include <TKey.h>
#include <TObjArray.h>

#include <AliRun.h>
#include <AliRunLoader.h>
#include <AliStack.h>
#include <AliESDtrack.h>
#include <AliESD.h>

#include "AliEventCut.h"
#include "AliReader.h"
#include "AliVAODParticle.h"


ClassImp(AliRunAnalysis)
AliRunAnalysis::AliRunAnalysis():
 TTask("RunAnalysis","Alice Analysis Manager")	,
 fReader(0x0),
 fEventCut(0x0),
 fCutOnSim(kFALSE),
 fCutOnRec(kTRUE)
{
  //ctor
}
/*********************************************************/

AliRunAnalysis::~AliRunAnalysis()
{
  //dtor
  delete fEventCut;
  delete fReader;
  delete fEventCut;
}
/*********************************************************/

Int_t AliRunAnalysis::Run()
{
 //makes analysis

  if (fReader == 0x0)
   {
     Error("Run","Reader is not set");
     return 1;
   }
 /******************************/ 
 /*  Init Event                */ 
 /******************************/ 
 for (Int_t an = 0; an < fAnalysies.GetEntries(); an++)
  {
      AliAnalysis* analysis = (AliAnalysis*)fAnalysies.At(an);
      analysis->Init();
  }
 
 while (fReader->Next() == kFALSE)
  {
   AliAOD* eventsim = fReader->GetEventSim();
   AliAOD* eventrec = fReader->GetEventRec();
   
      /******************************/ 
      /*  Event Cut                 */ 
      /******************************/ 
      if ( Pass(eventrec,eventsim) )
       {
         if (AliVAODParticle::GetDebug()) Info("Run","Event rejected by Event Cut");
         continue; //Did not pass the 
       }
      /******************************/ 
      /*  Process Event             */ 
      /******************************/ 
      for (Int_t an = 0; an < fAnalysies.GetEntries(); an++)
       {
           AliAnalysis* analysis = (AliAnalysis*)fAnalysies.At(an);
           analysis->ProcessEvent(eventrec,eventsim);
       }
    
  }//end of loop over events

 /******************************/ 
 /*  Finish Event              */ 
 /******************************/ 
 for (Int_t an = 0; an < fAnalysies.GetEntries(); an++)
  {
      AliAnalysis* analysis = (AliAnalysis*)fAnalysies.At(an);
      analysis->Finish();
  }

 return 0;   
}
/*********************************************************/

void  AliRunAnalysis::Add(AliAnalysis* a)
{
  //adds a to the list of analysis
  fAnalysies.Add(a);
}
/*********************************************************/

Bool_t AliRunAnalysis::Pass(AliAOD* recevent, AliAOD* simevent)
{
  //checks the event cut
  if (fEventCut == 0x0) return kFALSE;
  
  if (fCutOnRec)
    if (fEventCut->Pass(recevent)) return kTRUE;
    
  if (fCutOnSim)
    if (fEventCut->Pass(simevent)) return kTRUE;
  
  return kFALSE;
}

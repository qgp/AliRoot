//_________________________________________________________________________
//  Utility Class for transverse energy studies; charged hadrons
//  Task for analysis
//  - reconstruction and MC output
// implementation file
//
//Created by Christine Nattrass, Rebecca Scott, Irakli Martashvili
//University of Tennessee at Knoxville
//_________________________________________________________________________
//Necessary to read config macros
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>

#include "TChain.h"
#include "TList.h"
#include "TH2F.h"

#include "AliESDEvent.h"
#include "AliMCEvent.h"
#include "AliESDtrackCuts.h"

#include "AliAnalysisTaskHadEt.h"
#include "AliAnalysisHadEtReconstructed.h"
#include "AliAnalysisHadEtMonteCarlo.h"

#include <iostream>

using namespace std;

ClassImp(AliAnalysisTaskHadEt)



//________________________________________________________________________
AliAnalysisTaskHadEt::AliAnalysisTaskHadEt(const char *name) :
        AliAnalysisTaskSE(name)
	,fHadMCConfigFile("ConfigHadEtMonteCarlo.C")
	,fHadRecoConfigFile("ConfigHadEtReconstructed.C")
        ,fOutputList(0)
        ,fRecAnalysis(0)
        ,fMCAnalysis(0)
        ,fHistEtRecvsEtMC(0)
        ,fEsdtrackCutsITSTPC(0)
        ,fEsdtrackCutsTPC(0)
        ,fEsdtrackCutsITS(0)
{
    // Constructor

  if (fHadMCConfigFile.Length()) {
    cout<<"Rereading AliAnalysisHadEtMonteCarlo configuration file..."<<endl;
    gROOT->LoadMacro(fHadMCConfigFile);
    fMCAnalysis = (AliAnalysisHadEtMonteCarlo *) gInterpreter->ProcessLine("ConfigHadEtMonteCarlo()");
  }

  if (fHadRecoConfigFile.Length()) {
    cout<<"Rereading AliAnalysisHadEtReconstructed configuration file..."<<endl;
    gROOT->LoadMacro(fHadRecoConfigFile);
    fRecAnalysis = (AliAnalysisHadEtReconstructed *) gInterpreter->ProcessLine("ConfigHadEtReconstructed()");
  }

    // Define input and output slots here
    // Input slot #0 works with a TChain
    DefineInput(0, TChain::Class());
    // Output slot #1 writes into a TH1 container

    DefineOutput(1, TList::Class());
}
AliAnalysisTaskHadEt::~AliAnalysisTaskHadEt(){//Destructor
  fOutputList->Clear();
  delete fOutputList;
  delete fRecAnalysis;
  delete fMCAnalysis;
  delete fEsdtrackCutsITSTPC;
  delete fEsdtrackCutsTPC;
  delete fEsdtrackCutsITS;
}


//________________________________________________________________________
void AliAnalysisTaskHadEt::UserCreateOutputObjects()
{
    // Create histograms
    // Called once
  fOutputList = new TList;
  fOutputList->SetOwner();
  fMCAnalysis->SetHistoList(fOutputList);
  fRecAnalysis->SetHistoList(fOutputList);
  fMCAnalysis->CreateHistograms();
  fRecAnalysis->CreateHistograms();

  if(fRecAnalysis->DataSet() != fMCAnalysis->DataSet()){
    cout<<"Warning: Reconstruction data set and Monte Carlo data set are not the same!  Setting data set to "<<fRecAnalysis->DataSet()<<endl;
  }

  Bool_t selectPrimaries=kTRUE;
  if(fRecAnalysis->DataSet()==2009){
    cout<<"Setting track cuts for the 2009 p+p collisions at 900 GeV"<<endl;
    fEsdtrackCutsITSTPC = AliESDtrackCuts::GetStandardITSTPCTrackCuts2009(selectPrimaries);
    fEsdtrackCutsITSTPC->SetName("fEsdTrackCuts");
    fEsdtrackCutsTPC = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();
    fEsdtrackCutsTPC->SetName("fEsdTrackCutsTPCOnly");
    //ITS stand alone cuts - similar to 2009 cuts but with only ITS hits required
    fEsdtrackCutsITS =  AliESDtrackCuts::GetStandardITSPureSATrackCuts2009(kTRUE,kFALSE);//we do want primaries but we do not want to require PID info
    fEsdtrackCutsITS->SetName("fEsdTrackCutsITS");
  }
  if(fRecAnalysis->DataSet()==2010){
    //cout<<"Setting track cuts for the 2010 p+p collisions at 7 GeV"<<endl;
    cout<<"Warning:  Have not set 2010 track cuts yet!!"<<endl;
    fEsdtrackCutsITSTPC = AliESDtrackCuts::GetStandardITSTPCTrackCuts2010(selectPrimaries);
    fEsdtrackCutsITSTPC->SetName("fEsdTrackCuts");
    fEsdtrackCutsTPC = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();
    fEsdtrackCutsTPC->SetName("fEsdTrackCutsTPCOnly");
    //ITS stand alone cuts - similar to 2009 cuts but with only ITS hits required
    fEsdtrackCutsITS =  AliESDtrackCuts::GetStandardITSPureSATrackCuts2009(kTRUE,kFALSE);//we do want primaries but we do not want to require PID info
    fEsdtrackCutsITS->SetName("fEsdTrackCutsITS");
  }

  fOutputList->Add(fEsdtrackCutsITSTPC);
  fOutputList->Add(fEsdtrackCutsTPC);
  fOutputList->Add(fEsdtrackCutsITS);
  if(fEsdtrackCutsITSTPC && fEsdtrackCutsTPC){
    fRecAnalysis->SetITSTrackCuts( GetITSTrackCuts());
    fMCAnalysis->SetITSTrackCuts( GetITSTrackCuts());
    fRecAnalysis->SetTPCITSTrackCuts( GetTPCITSTrackCuts());
    fMCAnalysis->SetTPCITSTrackCuts( GetTPCITSTrackCuts());
    fRecAnalysis->SetTPCOnlyTrackCuts( GetTPCOnlyTrackCuts());
    fMCAnalysis->SetTPCOnlyTrackCuts( GetTPCOnlyTrackCuts());
    //add ITS stuff!
  }
  else{
    Printf("Error: no track cuts!");
  }
}

//________________________________________________________________________
void AliAnalysisTaskHadEt::UserExec(Option_t *)
{ // execute method
  AliESDEvent *event = dynamic_cast<AliESDEvent*>(InputEvent());
if (!event) {
  Printf("ERROR: Could not retrieve event");
  return;
 }
//cout<<"AliAnalysisHadEtReconstructed 90"<<endl;

fRecAnalysis->AnalyseEvent(event);

AliMCEvent* mcEvent = MCEvent();
// if (!mcEvent) {
//   Printf("ERROR: Could not retrieve MC event");
//  }
if (mcEvent && event)
  {
    ((AliAnalysisHadEtMonteCarlo*)fMCAnalysis)->AnalyseEvent((AliVEvent*)mcEvent,(AliVEvent*)event);
    if(fMCAnalysis->Full()){
      fMCAnalysis->FillSimTotEtMinusRecoTotEtFullAcceptanceTPC( fRecAnalysis->GetCorrectedTotEtFullAcceptanceTPC() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtFullAcceptanceITS( fRecAnalysis->GetCorrectedTotEtFullAcceptanceITS() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtFullAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedTotEtFullAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtFullAcceptanceITSNoPID( fRecAnalysis->GetCorrectedTotEtFullAcceptanceITSNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtFullAcceptanceTPC( fRecAnalysis->GetCorrectedHadEtFullAcceptanceTPC() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtFullAcceptanceITS( fRecAnalysis->GetCorrectedHadEtFullAcceptanceITS() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtFullAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedHadEtFullAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtFullAcceptanceITSNoPID( fRecAnalysis->GetCorrectedHadEtFullAcceptanceITSNoPID() );
    }
    if(fMCAnalysis->EMCAL()){
      fMCAnalysis->FillSimTotEtMinusRecoTotEtEMCALAcceptanceTPC( fRecAnalysis->GetCorrectedTotEtEMCALAcceptanceTPC() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtEMCALAcceptanceITS( fRecAnalysis->GetCorrectedTotEtEMCALAcceptanceITS() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtEMCALAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedTotEtEMCALAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtEMCALAcceptanceITSNoPID( fRecAnalysis->GetCorrectedTotEtEMCALAcceptanceITSNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtEMCALAcceptanceTPC( fRecAnalysis->GetCorrectedHadEtEMCALAcceptanceTPC() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtEMCALAcceptanceITS( fRecAnalysis->GetCorrectedHadEtEMCALAcceptanceITS() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtEMCALAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedHadEtEMCALAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtEMCALAcceptanceITSNoPID( fRecAnalysis->GetCorrectedHadEtEMCALAcceptanceITSNoPID() );
    }
    if(fMCAnalysis->PHOS()){
      fMCAnalysis->FillSimTotEtMinusRecoTotEtPHOSAcceptanceTPC( fRecAnalysis->GetCorrectedTotEtPHOSAcceptanceTPC() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtPHOSAcceptanceITS( fRecAnalysis->GetCorrectedTotEtPHOSAcceptanceITS() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtPHOSAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedTotEtPHOSAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimTotEtMinusRecoTotEtPHOSAcceptanceITSNoPID( fRecAnalysis->GetCorrectedTotEtPHOSAcceptanceITSNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtPHOSAcceptanceTPC( fRecAnalysis->GetCorrectedHadEtPHOSAcceptanceTPC() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtPHOSAcceptanceITS( fRecAnalysis->GetCorrectedHadEtPHOSAcceptanceITS() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtPHOSAcceptanceTPCNoPID( fRecAnalysis->GetCorrectedHadEtPHOSAcceptanceTPCNoPID() );
      fMCAnalysis->FillSimHadEtMinusRecoHadEtPHOSAcceptanceITSNoPID( fRecAnalysis->GetCorrectedHadEtPHOSAcceptanceITSNoPID() );
    }
    if(fMCAnalysis->PiKP() && fMCAnalysis->Full()){
      fMCAnalysis->FillSimPiKPMinusRecoPiKPFullAcceptanceTPC(fRecAnalysis->GetCorrectedPiKPEtFullAcceptanceTPC());
      fMCAnalysis->FillSimPiKPMinusRecoPiKPFullAcceptanceITS(fRecAnalysis->GetCorrectedPiKPEtFullAcceptanceITS());
      fMCAnalysis->FillSimPiKPMinusRecoPiKPFullAcceptanceTPCNoPID(fRecAnalysis->GetCorrectedPiKPEtFullAcceptanceTPCNoPID());
      fMCAnalysis->FillSimPiKPMinusRecoPiKPFullAcceptanceITSNoPID(fRecAnalysis->GetCorrectedPiKPEtFullAcceptanceITSNoPID());
    }
  }

// Post output data.
PostData(1, fOutputList);
}

//________________________________________________________________________
void AliAnalysisTaskHadEt::Terminate(Option_t *)
{
    // Draw result to the screen
    // Called once at the end of the query

    fOutputList = dynamic_cast<TList*> (GetOutputData(1));
    if (!fOutputList) {
        printf("ERROR: Output list not available\n");
        return;
    }
}



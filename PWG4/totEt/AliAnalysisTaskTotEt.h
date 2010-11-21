#ifndef ALIANALYSISTASKTOTET_H 
#define ALIANALYSISTASKTOTET_H 
//_________________________________________________________________________
//  Utility Class for transverse energy studies
//  Task for analysis
//  - reconstruction and MC output
//
//*-- Authors: Oystein Djuvsland (Bergen), David Silvermyr (ORNL)
//_________________________________________________________________________

class AliAnalysisEtReconstructed;
class AliAnalysisEtMonteCarlo;
class AliESDtrackCuts;
class TH2F;
class TList;

#include "AliAnalysisTaskSE.h"

class AliAnalysisTaskTotEt : public AliAnalysisTaskSE {
  
public:
  AliAnalysisTaskTotEt(const char *name = "AliAnalysisTaskTotEt");
  virtual ~AliAnalysisTaskTotEt();
  
public:
  
  //  virtual void   ConnectInputData(Option_t *);
  virtual void   UserCreateOutputObjects();
  virtual void   UserExec(Option_t *option);
  virtual void   Terminate(Option_t *);
    

  AliESDtrackCuts* GetTPCITSTrackCuts(){return (AliESDtrackCuts*) fOutputList->FindObject("fEsdTrackCuts");}
  AliESDtrackCuts* GetTPCOnlyTrackCuts(){return (AliESDtrackCuts*) fOutputList->FindObject("fEsdTrackCutsTPCOnly");}
  AliESDtrackCuts* GetITSTrackCuts(){return (AliESDtrackCuts*) fOutputList->FindObject("fEsdTrackCutsITS");}

private:
  
  //Declare private to avoid compilation warning
  AliAnalysisTaskTotEt & operator = (const AliAnalysisTaskTotEt & g) ;//copy assignment
  AliAnalysisTaskTotEt(const AliAnalysisTaskTotEt & g) ; // copy ctor

  TString       fMCConfigFile;        // the name of the ConfigFile
  TString       fRecoConfigFile;        // the name of the ConfigFile

  TList *fOutputList; //output list
  
  AliAnalysisEtReconstructed *fRecAnalysis; // Rec 
  AliAnalysisEtMonteCarlo *fMCAnalysis; // MC
  
  TH2F *fHistEtRecvsEtMC; // Rec vs MC histo

  AliESDtrackCuts* fEsdtrackCutsITSTPC; // track cuts ITS&TPC
  AliESDtrackCuts* fEsdtrackCutsTPC; // track cuts TPC
  AliESDtrackCuts* fEsdtrackCutsITS; // track cuts ITS
  
  ClassDef(AliAnalysisTaskTotEt, 1); // example of analysis
};

#endif

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

//-----------------------------------------------------------------------
// This class compares the global reconstruction with the TPConly reconstruction
// Author : Marta Verweij - UU
//-----------------------------------------------------------------------

#ifndef ALIPWG4HIGHPTQATPCONLY_H
#define ALIPWG4HIGHPTQATPCONLY_H

#include "AliAnalysisTask.h"

class TH1F;
class TH2F;
class TH3F;
class TList;
class AliESDEvent;
class AliESDfriend;
class AliESDfriendTrack;
class AliMCEvent;
class AliVEvent;
class AliESDtrackCuts;

class AliPWG4HighPtQATPConly: public AliAnalysisTask {

 public:
  AliPWG4HighPtQATPConly();
  AliPWG4HighPtQATPConly(const char *name);
  ~AliPWG4HighPtQATPConly() {;}

  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);

  void SetCuts(AliESDtrackCuts* trackCuts) {fTrackCuts = trackCuts;}
  void SetCutsITS(AliESDtrackCuts* trackCutsITS) {fTrackCutsITS = trackCutsITS;}
  //Select the trigger
  void SelectTrigger(Int_t trig) { fTrigger = trig; } 

 protected:

 private:

  void InitHistPointers();
  AliPWG4HighPtQATPConly(const AliPWG4HighPtQATPConly&);
  AliPWG4HighPtQATPConly& operator=(const AliPWG4HighPtQATPConly&);

  AliESDEvent *fESD;              //! ESD object
  AliESDfriend *fESDfriend;       //! ESD friend object
  AliMCEvent *fMC;                //! MC event object
  AliESDtrackCuts *fTrackCuts;    // TrackCuts for global vs TPConly comparison
  AliESDtrackCuts *fTrackCutsITS; // TrackCuts including ITSrefit
  Int_t fTrigger;                 //Trigger flag as defined in AliAnalysisHelperJetTasks.h 
  
  TH1F *fNEventAll;                             //! Event counter
  TH1F *fNEventSel;                             //! Event counter: Selected events for analysis
  TH1F *fPtAll;                                 //! Pt spectrum all charged particles
  TH1F *fPtSel;                                 //! Pt spectrum all selected charged particles by fTrackCuts
  TH2F *fPtAllminPtTPCvsPtAll;                  //! Momentum resolution (global vs TPConly)
  TH3F *fPtAllminPtTPCvsPtAllNPointTPC;         //! Momentum resolution vs NPointTPC
  TH3F *fPtAllminPtTPCvsPtAllDCAR;              //! Momentum resolution vs DCAR
  TH3F *fPtAllminPtTPCvsPtAllDCAZ;              //! Momentum resolution vs DCAZ
  TH3F *fPtAllminPtTPCvsPtAllPhi;               //! Momentum resolution vs Phi
  TH3F *fPtAllminPtTPCvsPtAllNPointITS;         //! Momentum resolution vs NPointITS
  TH3F *fPtAllminPtTPCvsPtAllNSigmaToVertex;    //! Momentum resolution vs NSigmaToVertes
  TH3F *fPtAllminPtTPCvsPtAllChi2C;             //! Momentum resolution vs Chi2Constrained
  TH3F *fPtAllminPtTPCvsPtAllRel1PtUncertainty; //! Momentum resolution vs relUncertainty1Pt
  TH3F *fPtAllminPtTPCvsPtAllChi2PerNClusTPC;   //! Momentum resolution vs Chi2PerNClusTPC
  TH3F *fPtAllminPtTPCvsPtAllChi2PerNClusITS;   //! Momentum resolution vs Chi2PerNClusITS
  TH2F *fEtaPhiOutliers;                        //! Eta Phi for outliers in momentum resolution
  //    fPtAllminPtTPCvsPtAllChi2PerNClusITS'
  TH1F *fPtSelITSouter;                         //! Pt at ITS outer wall for all selected charged particles by fTrackCuts
  TH2F *fPtITSouterminPtTPCvsPtAll;                  //! Momentum resolution (global vs ITSouter-TPCinner)
  TH3F *fPtITSouterminPtTPCvsPtAllNPointTPC;         //! Momentum resolution vs NPointTPC
  TH3F *fPtITSouterminPtTPCvsPtAllDCAR;              //! Momentum resolution vs DCAR
  TH3F *fPtITSouterminPtTPCvsPtAllDCAZ;              //! Momentum resolution vs DCAZ
  TH3F *fPtITSouterminPtTPCvsPtAllPhi;               //! Momentum resolution vs Phi
  TH3F *fPtITSouterminPtTPCvsPtAllNPointITS;         //! Momentum resolution vs NPointITS
  TH3F *fPtITSouterminPtTPCvsPtAllNSigmaToVertex;    //! Momentum resolution vs NSigmaToVertes
  TH3F *fPtITSouterminPtTPCvsPtAllChi2C;             //! Momentum resolution vs Chi2Constrained
  TH3F *fPtITSouterminPtTPCvsPtAllRel1PtUncertainty; //! Momentum resolution vs relUncertainty1Pt
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusTPC;   //! Momentum resolution vs Chi2PerNClusTPC
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS;   //! Momentum resolution vs Chi2PerNClusITS
 
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer0;                  //! Track has at least 1st SPD layer
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer1;                  //! Track has at least 2nd SPD layer and not 1st SPD
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer2;                  //! Track has at least 1st SDD layer and not SPD layers
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer3;                  //! Track has at least 1st SDD layer and not SPD layers and not 1st SDD
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer4;                  //! Track has at least 1st SSD layer and not SPD or SDD layers
  TH2F *fPtITSouterminPtTPCvsPtAll_ITSLayer5;                  //! Track has at least 1st SSD layer and not SPD or SDD layers or 1st SSD

  TH2F *fPtITSouterminPtTPCvsPtAll_NoSPD;                  //! Track has no signal in SPD layers
  TH2F *fPtITSouterminPtTPCvsPtAll_NoSDD;                  //! Track has no signal in SDD layers
  TH2F *fPtITSouterminPtTPCvsPtAll_NoSSD;                  //! Track has no signal in SSD layers

  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer0;                  //! Track has at least 1st SPD layer
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer1;                  //! Track has at least 2nd SPD layer and not 1st SPD
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer2;                  //! Track has at least 1st SDD layer and not SPD layers
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer3;                  //! Track has at least 1st SDD layer and not SPD layers and not 1st SDD
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer4;                  //! Track has at least 1st SSD layer and not SPD or SDD layers
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_ITSLayer5;                  //! Track has at least 1st SSD layer and not SPD or SDD layers or 1st SSD

  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_NoSPD;                  //! Track has no signal in SPD layers
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_NoSDD;                  //! Track has no signal in SDD layers
  TH3F *fPtITSouterminPtTPCvsPtAllChi2PerNClusITS_NoSSD;                  //! Track has no signal in SSD layers

  TList *fHistList; //! List of Histograms
  
  TH1F *fPtAllTPC;     //! Pt spectrum all charged particles
  TH1F *fPtSelTPC;     //! Pt spectrum all selected charged particles by fTrackCuts
  TH1F *fPtSelTPCITS;  //! Pt spectrum all selected charged particles by fTrackCutsITS
  TList *fHistListTPC; //! List of Histograms
  
  TH1F *fPtSelITS;                              //! Pt spectrum all selected charged particles by fTrackCutsITS
  TH2F *fPtITSminPtTPCvsPtITS;                  //! Momentum resolution (global with ITSrefit vs TPConly)
  TH3F *fPtITSminPtTPCvsPtITSNPointTPC;         //! Momentum resolution vs NPointTPC 
  TH3F *fPtITSminPtTPCvsPtITSDCAR;              //! Momentum resolution vs DCAR
  TH3F *fPtITSminPtTPCvsPtITSDCAZ;              //! Momentum resolution vs DCAZ
  TH3F *fPtITSminPtTPCvsPtITSPhi;               //! Momentum resolution vs Phi
  TH3F *fPtITSminPtTPCvsPtITSNPointITS;         //! Momentum resolution vs NPointITS
  TH3F *fPtITSminPtTPCvsPtITSNSigmaToVertex;    //! Momentum resolution vs NSigmaToVertex
  TH3F *fPtITSminPtTPCvsPtITSChi2C;             //! Momentum resolution vs Chi2Constrained
  TH3F *fPtITSminPtTPCvsPtITSRel1PtUncertainty; //! Momentum resolution vs relUncertainty1Pt
  TH3F *fPtITSminPtTPCvsPtITSChi2PerNClusTPC;   //! Momentum resolution vs Chi2PerNClusTPC
  TH3F *fPtITSminPtTPCvsPtITSChi2PerNClusITS;  //! Momentum resolution vs Chi2PerNClusITS

  TH2F *fPtOuterPtInner;                        //! Momentum of global track at inner wall versus outer wall of TPC
  TH3F *fPtRel1PtUncertaintyChi2PerClusTPC;     //! Global Pt vs relUncertainty1Pt vs Chi2PerClusTPC

  TList *fHistListITS; //! List of Histograms

  ClassDef(AliPWG4HighPtQATPConly,1) 
  
};
#endif

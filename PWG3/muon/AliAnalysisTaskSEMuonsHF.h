#ifndef ALIANALYSISTASKSEMUONSHF_H
#define ALIANALYSISTASKSEMUONSHF_H

/* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//*************************************************************************
// Class AliAnalysisTaskSEMuonsHF
// AliAnalysisTaskSE for the single muon and dimuon from HF analysis
// Author: X-M. Zhang, zhang@clermont.in2p3.fr
//                     zhangxm@iopp.ccnu.edu.cn
//*************************************************************************

#include "AliAnalysisTaskSE.h"

class TString;
class TList;
class TClonesArray;
class AliMuonsHFHeader;

class AliAnalysisTaskSEMuonsHF : public AliAnalysisTaskSE {
 public:

  AliAnalysisTaskSEMuonsHF();
  AliAnalysisTaskSEMuonsHF(const char *name);
  virtual ~AliAnalysisTaskSEMuonsHF();

  virtual void Init();
  virtual void LocalInit() { Init(); }
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *opt);
  virtual void Terminate(Option_t *opt);

  void SetAnaMode(Int_t mode) { fAnaMode = (mode<3 ? mode : 0); }
  void SetIsOutputTree(Bool_t ist) { fIsOutputTree = ist; }
  void SetIsUseMC(Bool_t isMC) { fIsUseMC = isMC; }

  void SetEvsHCuts(Double_t cuts[3])  const { AliMuonsHFHeader::SetSelectionCuts(cuts);   }
  void SetMuonCuts(Double_t cuts[10]) const { AliMuonInfoStoreRD::SetSelectionCuts(cuts); }
  void SetDimuCuts(Double_t cuts[10]) const { AliDimuInfoStoreRD::SetSelectionCuts(cuts); }

 private:

  AliAnalysisTaskSEMuonsHF(const AliAnalysisTaskSEMuonsHF&);
  AliAnalysisTaskSEMuonsHF& operator=(const AliAnalysisTaskSEMuonsHF&);

  Int_t fAnaMode;        // = 0, ana both single muon and dimuon
                         // = 1, ana single muon
                         // = 2, ana dimuon
  Bool_t fIsOutputTree;  // flag used to switch on/off tree output
  Bool_t fIsUseMC;       // flag used to switch on/off MC ana

  AliMuonsHFHeader *fHeader;  // output for info at ev level
  TClonesArray *fMuonClArr;   // output clones array for single mu
  TClonesArray *fDimuClArr;   // output clones array for dimu

  TList *fListHisHeader;  // output list of histos at event level
  TList *fListHisMuon;    // output list of histos for single mu
  TList *fListHisDimu;    // output list of histos for dimuon

  ClassDef(AliAnalysisTaskSEMuonsHF, 6);
};

#endif

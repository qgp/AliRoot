#ifndef ALIANALYSISMULTPBCENTRALITYSELECTOR_H
#define ALIANALYSISMULTPBCENTRALITYSELECTOR_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                    AliAnalysisMultPbCentralitySelector
// 
// This class selects collision candidates from data runs, applying selection cuts on triggers 
// and background rejection based on the content of the ESD
//
// Author: Michele Floris, CERN
//-------------------------------------------------------------------------

#include <AliAnalysisCuts.h>
#include <AliLog.h>

#define VERBOSE_STAT

class AliESDEvent;
class TH2F;
class TH1F;
class TCollection;
class AliTriggerAnalysis;
class AliAnalysisTaskSE;
class AliESDtrackCuts;

class AliAnalysisMultPbCentralitySelector : public AliAnalysisCuts
{
public:

  AliAnalysisMultPbCentralitySelector() : fIsMC (0), fCentrEstimator(""), fCentrBin(-1), fMultMin(0), fMultMax(1000000), fFile1(""), fFile2(""), fUseMultRange(kFALSE) {;}
  virtual ~AliAnalysisMultPbCentralitySelector(){}
    
  // AliAnalysisCuts interface
  virtual UInt_t GetSelectionMask(const TObject* obj) { return (UInt_t) IsCentralityBinSelected((AliESDEvent*) obj, NULL); }
  virtual Bool_t IsSelected(TList*) { AliFatal("Not implemented"); return kFALSE; }
  virtual Bool_t IsSelected(TObject* obj)  {return (UInt_t) IsCentralityBinSelected ( (AliESDEvent*) obj, NULL);}
    
  Bool_t IsCentralityBinSelected(AliESDEvent* aEsd, AliESDtrackCuts * trackCuts);
    
  void SetAnalyzeMC(Bool_t flag = kTRUE, Double_t multMin = 0, Double_t multMax=10000) { fIsMC = flag; fMultMin = multMin; fMultMax = multMax; }
  void SetMultRange(Double_t multMin = 0, Double_t multMax=10000) { fMultMin = multMin; fMultMax = multMax; }
  void SetUseMultRange(Bool_t flag = kTRUE) {fUseMultRange = flag;}
  void SetCentralityEstimator(const char * estimator) { fCentrEstimator = estimator; }
  void SetCentralityBin(Int_t bin) { fCentrBin = bin; }
  void SetCentrTaskFiles(const char * file1, const char * file2) { fFile1 = file1; fFile2 = file2; }
  virtual void Print(Option_t* option = "") const ;
  virtual Long64_t Merge(TCollection* list){list->GetEntries();return 0;}
  
protected:
  Bool_t fIsMC;             // flag if MC is analyzed
  TString fCentrEstimator;  // Centrality estimator for AliESDCentrality
  Int_t   fCentrBin; // centrality bin to be selected
  Int_t fMultMin ; // Minimum multiplicity, because on MC we cut on tracks rather than on the estimator  
  Int_t fMultMax ; // Maximum multiplicity, because on MC we cut on tracks rather than on the estimator  
  TString fFile1; // file used by centrality task. Set here for bookkeeping
  TString fFile2; // file used by centrality task. Set here for bookkeeping
  Bool_t fUseMultRange; // if true, use track bins rather than multiplicity estimator

  ClassDef(AliAnalysisMultPbCentralitySelector, 2)
    
  private:
  AliAnalysisMultPbCentralitySelector(const AliAnalysisMultPbCentralitySelector&); // not implemented
  AliAnalysisMultPbCentralitySelector& operator=(const AliAnalysisMultPbCentralitySelector&); // not implemented
};

#endif

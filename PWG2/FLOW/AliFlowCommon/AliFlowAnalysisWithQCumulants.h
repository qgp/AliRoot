/* 
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. 
 * See cxx source for full Copyright notice 
 * $Id$ 
 */

/********************************** 
 * flow analysis with Q-cumulants * 
 *                                * 
 * author:  Ante Bilandzic        * 
 *           (anteb@nikhef.nl)    *
 *********************************/ 

#ifndef ALIFLOWANALYSISWITHQCUMULANTS_H
#define ALIFLOWANALYSISWITHQCUMULANTS_H

#include "AliFlowCommonConstants.h" // needed as include
#include "TMatrixD.h"
#include "TH2D.h"
#include "TBits.h"

class TObjArray;
class TList;
class TFile;
class TGraph;

class TH1;
class TProfile;
class TProfile2D;

class AliFlowEventSimple;
class AliFlowVector;

class AliFlowCommonHist;
class AliFlowCommonHistResults;

//================================================================================================================

class AliFlowAnalysisWithQCumulants{
 public:
  AliFlowAnalysisWithQCumulants();
  virtual ~AliFlowAnalysisWithQCumulants(); 
  // 0.) methods called in the constructor:
  virtual void InitializeArraysForIntFlow();
  virtual void InitializeArraysForDiffFlow();
  virtual void InitializeArraysForDistributions();
  virtual void InitializeArraysForNestedLoops();
  // 1.) method Init() and methods called within Init():
  virtual void Init();
    virtual void AccessConstants();
    virtual void BookAndNestAllLists();
    virtual void BookCommonHistograms();
    virtual void BookAndFillWeightsHistograms();
    virtual void BookEverythingForIntegratedFlow();
    virtual void BookEverythingForDifferentialFlow();
    virtual void BookEverythingForDistributions();  
    virtual void BookEverythingForNestedLoops();   
    virtual void StoreIntFlowFlags();
    virtual void StoreDiffFlowFlags();
    virtual void StoreHarmonic();
  // 2.) method Make() and methods called within Make():
  virtual void Make(AliFlowEventSimple *anEvent);
    // 2a.) common:
    virtual void FillAverageMultiplicities(Int_t nRP);
    virtual void FillCommonControlHistograms(AliFlowEventSimple *anEvent);
    virtual void ResetEventByEventQuantities();
    // 2b.) integrated flow:
    virtual void CalculateIntFlowCorrelations(); 
    virtual void CalculateIntFlowProductOfCorrelations();
    virtual void CalculateIntFlowSumOfEventWeights();
    virtual void CalculateIntFlowSumOfProductOfEventWeights();
    virtual void CalculateIntFlowCorrectionsForNUASinTerms();  
    virtual void CalculateIntFlowCorrectionsForNUACosTerms();
    // ...  
    virtual void CalculateIntFlowCorrelationsUsingParticleWeights();
    virtual void CalculateWeightedQProductsForIntFlow();
    virtual void EvaluateIntFlowCorrelationsWithNestedLoops(AliFlowEventSimple* const anEvent); 
    virtual void EvaluateIntFlowCorrelationsWithNestedLoopsUsingParticleWeights(AliFlowEventSimple* const anEvent); 
    virtual void EvaluateIntFlowCorrectionsForNUAWithNestedLoops(AliFlowEventSimple* const anEvent);  
    // 2c.) differential flow:
    virtual void CalculateDiffFlowCorrelations(TString type, TString ptOrEta); // type = RP or POI
    virtual void CalculateDiffFlowCorrelationsUsingParticleWeights(TString type, TString ptOrEta); // type = RP or POI 
    virtual void CalculateDiffFlowProductOfCorrelations(TString type, TString ptOrEta); // type = RP or POI
    virtual void CalculateDiffFlowSumOfEventWeights(TString type, TString ptOrEta); // type = RP or POI
    virtual void CalculateDiffFlowSumOfProductOfEventWeights(TString type, TString ptOrEta); // type = RP or POI
    virtual void CalculateDiffFlowCorrectionsForNUASinTerms(TString type, TString ptOrEta);  
    virtual void CalculateDiffFlowCorrectionsForNUACosTerms(TString type, TString ptOrEta);
    // ...
    //virtual void CalculateCorrelationsForDifferentialFlow2D(TString type); // type = RP or POI
    //virtual void CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowCosTerms(TString type); // type = RP or POI  
    //virtual void CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowSinTerms(TString type); // type = RP or POI
    virtual void EvaluateDiffFlowCorrelationsWithNestedLoops(AliFlowEventSimple* const anEvent, TString type, TString ptOrEta);
    virtual void EvaluateDiffFlowCorrelationsWithNestedLoopsUsingParticleWeights(AliFlowEventSimple* const anEvent, TString type, TString ptOrEta); 
    virtual void EvaluateDiffFlowCorrectionTermsForNUAWithNestedLoops(AliFlowEventSimple* const anEvent, TString type, TString ptOrEta);
  // 3.) method Finish() and methods called within Finish():
  virtual void Finish();
    // 3a.) integrated flow:
    virtual void FinalizeCorrelationsIntFlow();
    virtual void FinalizeCorrectionTermsForNUAIntFlow(); 
    virtual void CalculateCovariancesIntFlow();  
    virtual void CalculateCumulantsIntFlow(); 
    virtual void CalculateIntFlow(); 
    virtual void FillCommonHistResultsIntFlow();
    // nua:   
    //virtual void CalculateCorrectionsForNUAForIntQcumulants();
    virtual void CalculateQcumulantsCorrectedForNUAIntFlow(); 
    virtual void CalculateIntFlowCorrectedForNUA(); 
    //virtual void ApplyCorrectionForNonUniformAcceptanceToCumulantsForIntFlow(Bool_t useParticleWeights, TString eventWeights); 
    //virtual void PrintQuantifyingCorrectionsForNonUniformAcceptance(Bool_t useParticleWeights, TString eventWeights);
    virtual void PrintFinalResultsForIntegratedFlow(TString type);
    virtual void CrossCheckIntFlowCorrelations();
    virtual void CrossCheckIntFlowExtraCorrelations(); // extra correlations which appear only when particle weights are used
    virtual void CrossCheckIntFlowCorrectionTermsForNUA(); 
    // 3b.) differential flow:
    virtual void FinalizeReducedCorrelations(TString type, TString ptOrEta);
    virtual void CalculateDiffFlowCovariances(TString type, TString ptOrEta); 
    virtual void CalculateDiffFlowCumulants(TString type, TString ptOrEta); 
    virtual void CalculateDiffFlow(TString type, TString ptOrEta); 
    virtual void FinalizeCorrectionTermsForNUADiffFlow(TString type, TString ptOrEta); 
    virtual void CalculateDiffFlowCumulantsCorrectedForNUA(TString type, TString ptOrEta);   
    virtual void CalculateDiffFlowCorrectedForNUA(TString type, TString ptOrEta); 
    virtual void CalculateFinalResultsForRPandPOIIntegratedFlow(TString type); // to be improved (add also possibility to integrate over eta yield)
    virtual void FillCommonHistResultsDiffFlow(TString type);   
    virtual void CrossCheckDiffFlowCorrelations(TString type, TString ptOrEta); 
    virtual void CrossCheckDiffFlowCorrectionTermsForNUA(TString type, TString ptOrEta); 
        
    // to be improved (removed):
    //virtual void FinalizeCorrelationsForDiffFlow(TString type, Bool_t useParticleWeights, TString eventWeights); 
      
  // 4.)  method GetOutputHistograms() and methods called within GetOutputHistograms(): 
  virtual void GetOutputHistograms(TList *outputListHistos);
    virtual void GetPointersForCommonHistograms(TList *outputListHistos); 
    virtual void GetPointersForParticleWeightsHistograms(TList *outputListHistos);
    virtual void GetPointersForIntFlowHistograms(TList *outputListHistos); 
    virtual void GetPointersForDiffFlowHistograms(TList *outputListHistos); 
    virtual void GetPointersForNestedLoopsHistograms(TList *outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
    
  // 5.) other methods:   
  TProfile* MakePtProjection(TProfile2D *profilePtEta) const;
  TProfile* MakeEtaProjection(TProfile2D *profilePtEta) const;
  virtual void WriteHistograms(TString outputFileName);
  virtual void WriteHistograms(TDirectoryFile *outputFileName);
  
  // **** SETTERS and GETTERS ****
  
  // 0.) base:                                                                                              
  TList* GetHistList() const {return this->fHistList;} 
  
  // 1.) common:
  void SetCommonHists(AliFlowCommonHist* const ch) {this->fCommonHists = ch;};
  AliFlowCommonHist* GetCommonHists() const {return this->fCommonHists;};
  void SetCommonHists2nd(AliFlowCommonHist* const ch2nd) {this->fCommonHists2nd = ch2nd;};
  AliFlowCommonHist* GetCommonHists2nd() const {return this->fCommonHists2nd;};
  void SetCommonHists4th(AliFlowCommonHist* const ch4th) {this->fCommonHists4th = ch4th;};
  AliFlowCommonHist* GetCommonHists4th() const {return this->fCommonHists4th;};
  void SetCommonHists6th(AliFlowCommonHist* const ch6th) {this->fCommonHists6th = ch6th;};
  AliFlowCommonHist* GetCommonHists6th() const {return this->fCommonHists6th;};
  void SetCommonHists8th(AliFlowCommonHist* const ch8th) {this->fCommonHists8th = ch8th;};
  AliFlowCommonHist* GetCommonHists8th() const {return this->fCommonHists8th;};
  void SetCommonHistsResults2nd(AliFlowCommonHistResults* const chr2nd) {this->fCommonHistsResults2nd = chr2nd;};
  AliFlowCommonHistResults* GetCommonHistsResults2nd() const {return this->fCommonHistsResults2nd;};
  void SetCommonHistsResults4th(AliFlowCommonHistResults* const chr4th) {this->fCommonHistsResults4th = chr4th;};
  AliFlowCommonHistResults* GetCommonHistsResults4th() const {return this->fCommonHistsResults4th;};
  void SetCommonHistsResults6th(AliFlowCommonHistResults* const chr6th) {this->fCommonHistsResults6th = chr6th;};
  AliFlowCommonHistResults* GetCommonHistsResults6th() const {return this->fCommonHistsResults6th;};
  void SetCommonHistsResults8th(AliFlowCommonHistResults* const chr8th) {this->fCommonHistsResults8th = chr8th;};
  AliFlowCommonHistResults* GetCommonHistsResults8th() const {return this->fCommonHistsResults8th;};
  void SetHarmonic(Int_t const harmonic) {this->fHarmonic = harmonic;};
  Int_t GetHarmonic() const {return this->fHarmonic;};
  void SetAnalysisLabel(const char *aLabel) {this->fAnalysisLabel->Append(*aLabel);}; // to be improved (Append(*aLabel) changed into Append(aLabel)) 
  TString *GetAnalysisLabel() const {return this->fAnalysisLabel;};
  
  // 2a.) particle weights:
  void SetWeightsList(TList* const wlist) {this->fWeightsList = (TList*)wlist->Clone();}
  TList* GetWeightsList() const {return this->fWeightsList;}  
  void SetUsePhiWeights(Bool_t const uPhiW) {this->fUsePhiWeights = uPhiW;};
  Bool_t GetUsePhiWeights() const {return this->fUsePhiWeights;};
  void SetUsePtWeights(Bool_t const uPtW) {this->fUsePtWeights = uPtW;};
  Bool_t GetUsePtWeights() const {return this->fUsePtWeights;};
  void SetUseEtaWeights(Bool_t const uEtaW) {this->fUseEtaWeights = uEtaW;};
  Bool_t GetUseEtaWeights() const {return this->fUseEtaWeights;};
  void SetUseParticleWeights(TProfile* const uPW) {this->fUseParticleWeights = uPW;};
  TProfile* GetUseParticleWeights() const {return this->fUseParticleWeights;};
  void SetPhiWeights(TH1F* const histPhiWeights) {this->fPhiWeights = histPhiWeights;};
  TH1F* GetPhiWeights() const {return this->fPhiWeights;};
  void SetPtWeights(TH1D* const histPtWeights) {this->fPtWeights = histPtWeights;};
  TH1D* GetPtWeights() const {return this->fPtWeights;};
  void SetEtaWeights(TH1D* const histEtaWeights) {this->fEtaWeights = histEtaWeights;};
  TH1D* GetEtaWeights() const {return this->fEtaWeights;};
  
  // 2b.) event weights:
  void SetMultiplicityWeight(const char *multiplicityWeight) {*this->fMultiplicityWeight = multiplicityWeight;};
  
  // 3.) integrated flow:
  // flags:
  void SetIntFlowFlags(TProfile* const intFlowFlags) {this->fIntFlowFlags = intFlowFlags;};
  TProfile* GetIntFlowFlags() const {return this->fIntFlowFlags;};
  void SetApplyCorrectionForNUA(Bool_t const applyCorrectionForNUA) {this->fApplyCorrectionForNUA = applyCorrectionForNUA;};
  Bool_t GetApplyCorrectionForNUA() const {return this->fApplyCorrectionForNUA;};
  // integrated flow profiles:
  void SetAvMultiplicity(TProfile* const avMultiplicity) {this->fAvMultiplicity = avMultiplicity;};
  TProfile* GetAvMultiplicity() const {return this->fAvMultiplicity;};
  void SetIntFlowCorrelationsPro(TProfile* const intFlowCorrelationsPro) {this->fIntFlowCorrelationsPro = intFlowCorrelationsPro;};
  TProfile* GetIntFlowCorrelationsPro() const {return this->fIntFlowCorrelationsPro;};
  void SetIntFlowCorrelationsAllPro(TProfile* const intFlowCorrelationsAllPro) {this->fIntFlowCorrelationsAllPro = intFlowCorrelationsAllPro;};
  TProfile* GetIntFlowCorrelationsAllPro() const {return this->fIntFlowCorrelationsAllPro;};  
  void SetIntFlowExtraCorrelationsPro(TProfile* const intFlowExtraCorrelationsPro) {this->fIntFlowExtraCorrelationsPro = intFlowExtraCorrelationsPro;};
  TProfile* GetIntFlowExtraCorrelationsPro() const {return this->fIntFlowExtraCorrelationsPro;};  
  void SetIntFlowProductOfCorrelationsPro(TProfile* const intFlowProductOfCorrelationsPro) {this->fIntFlowProductOfCorrelationsPro = intFlowProductOfCorrelationsPro;};
  TProfile* GetIntFlowProductOfCorrelationsPro() const {return this->fIntFlowProductOfCorrelationsPro;};  
  void SetIntFlowCorrectionTermsForNUAPro(TProfile* const ifctfnp, Int_t const sc) {this->fIntFlowCorrectionTermsForNUAPro[sc] = ifctfnp;};
  TProfile* GetIntFlowCorrectionTermsForNUAPro(Int_t sc) const {return this->fIntFlowCorrectionTermsForNUAPro[sc];};  
  // integrated flow histograms holding all results:
  void SetIntFlowCorrelationsHist(TH1D* const intFlowCorrelationsHist) {this->fIntFlowCorrelationsHist = intFlowCorrelationsHist;};
  TH1D* GetIntFlowCorrelationsHist() const {return this->fIntFlowCorrelationsHist;};
  void SetIntFlowCorrelationsAllHist(TH1D* const intFlowCorrelationsAllHist) {this->fIntFlowCorrelationsAllHist = intFlowCorrelationsAllHist;};
  TH1D* GetIntFlowCorrelationsAllHist() const {return this->fIntFlowCorrelationsAllHist;};  
  // to be improved (removed:)
  //void SetIntFlowProductOfCorrelationsHist(TH1D* const intFlowProductOfCorrelationsHist) {this->fIntFlowProductOfCorrelationsHist = intFlowProductOfCorrelationsHist;};
  //TH1D* GetIntFlowProductOfCorrelationsHist() const {return this->fIntFlowProductOfCorrelationsHist;};  
  void SetIntFlowCorrectionTermsForNUAHist(TH1D* const ifctfnh, Int_t const sc) {this->fIntFlowCorrectionTermsForNUAHist[sc] = ifctfnh;};
  TH1D* GetIntFlowCorrectionTermsForNUAHist(Int_t sc) const {return this->fIntFlowCorrectionTermsForNUAHist[sc];};  
  void SetIntFlowCovariances(TH1D* const intFlowCovariances) {this->fIntFlowCovariances = intFlowCovariances;};
  TH1D* GetIntFlowCovariances() const {return this->fIntFlowCovariances;};
  void SetIntFlowSumOfEventWeights(TH1D* const intFlowSumOfEventWeights, Int_t const power) {this->fIntFlowSumOfEventWeights[power] = intFlowSumOfEventWeights;};
  TH1D* GetIntFlowSumOfEventWeights(Int_t power) const {return this->fIntFlowSumOfEventWeights[power];};
  void SetIntFlowSumOfProductOfEventWeights(TH1D* const intFlowSumOfProductOfEventWeights) {this->fIntFlowSumOfProductOfEventWeights = intFlowSumOfProductOfEventWeights;};
  TH1D* GetIntFlowSumOfProductOfEventWeights() const {return this->fIntFlowSumOfProductOfEventWeights;}; 
  void SetIntFlowQcumulants(TH1D* const intFlowQcumulants) {this->fIntFlowQcumulants = intFlowQcumulants;};
  TH1D* GetIntFlowQcumulants() const {return this->fIntFlowQcumulants;}; 
  void SetIntFlow(TH1D* const intFlow) {this->fIntFlow = intFlow;};
  TH1D* GetIntFlow() const {return this->fIntFlow;};
  
  // 4.) differential flow:
  // flags:
  void SetDiffFlowFlags(TProfile* const diffFlowFlags) {this->fDiffFlowFlags = diffFlowFlags;};
  TProfile* GetDiffFlowFlags() const {return this->fDiffFlowFlags;};
  void SetCalculate2DFlow(Bool_t const calculate2DFlow) {this->fCalculate2DFlow = calculate2DFlow;};
  Bool_t GetCalculate2DFlow() const {return this->fCalculate2DFlow;};
  // profiles:
  // 1D:
  void SetDiffFlowCorrelationsPro(TProfile* const diffFlowCorrelationsPro, Int_t const i, Int_t const j, Int_t const k) {this->fDiffFlowCorrelationsPro[i][j][k] = diffFlowCorrelationsPro;};
  TProfile* GetDiffFlowCorrelationsPro(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlowCorrelationsPro[i][j][k];};
  void SetDiffFlowProductOfCorrelationsPro(TProfile* const dfpocp, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowProductOfCorrelationsPro[i][j][k][l] = dfpocp;};
  TProfile* GetDiffFlowProductOfCorrelationsPro(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowProductOfCorrelationsPro[i][j][k][l];};
  void SetDiffFlowCorrectionTermsForNUAPro(TProfile* const dfctfnp, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowCorrectionTermsForNUAPro[i][j][k][l] = dfctfnp;};
  TProfile* GetDiffFlowCorrectionTermsForNUAPro(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowCorrectionTermsForNUAPro[i][j][k][l];};  
  // 2D:
  void SetCorrelationsPro(TProfile2D* const correlPro, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fCorrelationsPro[i][j][k][l] = correlPro;};
  TProfile2D* GetCorrelationsPro(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fCorrelationsPro[i][j][k][l];};
  void SetProductsOfCorrelationsPro(TProfile2D* const proOfcorrelPro, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fProductsOfCorrelationsPro[i][j][k][l] = proOfcorrelPro;};
  TProfile2D* GetProductsOfCorrelationsPro(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fProductsOfCorrelationsPro[i][j][k][l];};
  void SetCorrectionTermsPro(TProfile2D* const correctTermsPro, Int_t const i, Int_t const j, Int_t const k, Int_t const l, Int_t const m) {this->fCorrectionTermsPro[i][j][k][l][m] = correctTermsPro;};
  TProfile2D* GetCorrectionTermsPro(Int_t i, Int_t j, Int_t k, Int_t l, Int_t m) const {return this->fCorrectionTermsPro[i][j][k][l][m];};  
  // histograms:
  void SetDiffFlowCorrelationsHist(TH1D* const diffFlowCorrelationsHist, Int_t const i, Int_t const j, Int_t const k) {this->fDiffFlowCorrelationsHist[i][j][k] = diffFlowCorrelationsHist;};
  TH1D* GetDiffFlowCorrelationsHist(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlowCorrelationsHist[i][j][k];};
  void SetDiffFlowCovariances(TH1D* const diffFlowCovariances, Int_t const i, Int_t const j, Int_t const k) {this->fDiffFlowCovariances[i][j][k] = diffFlowCovariances;};
  TH1D* GetDiffFlowCovariances(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlowCovariances[i][j][k];};  
  void SetDiffFlowCumulants(TH1D* const diffFlowCumulants, Int_t const i, Int_t const j, Int_t const k) {this->fDiffFlowCumulants[i][j][k] = diffFlowCumulants;};
  TH1D* GetDiffFlowCumulants(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlowCumulants[i][j][k];};
  void SetDiffFlow(TH1D* const diffFlow, Int_t const i, Int_t const j, Int_t const k) {this->fDiffFlow[i][j][k] = diffFlow;};
  TH1D* GetDiffFlow(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlow[i][j][k];};
  void SetDiffFlowSumOfEventWeights(TH1D* const dfsoew, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowSumOfEventWeights[i][j][k][l] = dfsoew;};
  TH1D* GetDiffFlowSumOfEventWeights(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowSumOfEventWeights[i][j][k][l];};
  void SetDiffFlowSumOfProductOfEventWeights(TH1D* const dfsopoew, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowSumOfProductOfEventWeights[i][j][k][l] = dfsopoew;};
  TH1D* GetDiffFlowSumOfProductOfEventWeights(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowSumOfProductOfEventWeights[i][j][k][l];};
  void SetDiffFlowCorrectionTermsForNUAHist(TH1D* const dfctfnh, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowCorrectionTermsForNUAHist[i][j][k][l] = dfctfnh;};
  TH1D* GetDiffFlowCorrectionTermsForNUAHist(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowCorrectionTermsForNUAHist[i][j][k][l];};  
  
  // x.) debugging and cross-checking:
  void SetNestedLoopsList(TList* const nllist) {this->fNestedLoopsList = nllist;};
  TList* GetNestedLoopsList() const {return this->fNestedLoopsList;}; 
  void SetEvaluateIntFlowNestedLoops(Bool_t const eifnl) {this->fEvaluateIntFlowNestedLoops = eifnl;};
  Bool_t GetEvaluateIntFlowNestedLoops() const {return this->fEvaluateIntFlowNestedLoops;};
  void SetEvaluateDiffFlowNestedLoops(Bool_t const edfnl) {this->fEvaluateDiffFlowNestedLoops = edfnl;};
  Bool_t GetEvaluateDiffFlowNestedLoops() const {return this->fEvaluateDiffFlowNestedLoops;};  
  void SetMaxAllowedMultiplicity(Int_t const maxAllowedMultiplicity) {this->fMaxAllowedMultiplicity = maxAllowedMultiplicity;};
  Int_t GetMaxAllowedMultiplicity() const {return this->fMaxAllowedMultiplicity;};
  void SetEvaluateNestedLoops(TProfile* const enl) {this->fEvaluateNestedLoops = enl;};
  TProfile* GetEvaluateNestedLoops() const {return this->fEvaluateNestedLoops;}; 
  void SetIntFlowDirectCorrelations(TProfile* const ifdc) {this->fIntFlowDirectCorrelations = ifdc;};
  TProfile* GetIntFlowDirectCorrelations() const {return this->fIntFlowDirectCorrelations;};
  void SetIntFlowExtraDirectCorrelations(TProfile* const ifedc) {this->fIntFlowExtraDirectCorrelations = ifedc;};
  TProfile* GetIntFlowExtraDirectCorrelations() const {return this->fIntFlowExtraDirectCorrelations;};
  void SetIntFlowDirectCorrectionTermsForNUA(TProfile* const ifdctfn, Int_t const sc) {this->fIntFlowDirectCorrectionTermsForNUA[sc] = ifdctfn;};
  TProfile* GetIntFlowDirectCorrectionTermsForNUA(Int_t sc) const {return this->fIntFlowDirectCorrectionTermsForNUA[sc];};  
  void SetCrossCheckInPtBinNo(Int_t const crossCheckInPtBinNo) {this->fCrossCheckInPtBinNo = crossCheckInPtBinNo;};
  Int_t GetCrossCheckInPtBinNo() const {return this->fCrossCheckInPtBinNo;};
  void SetCrossCheckInEtaBinNo(Int_t const crossCheckInEtaBinNo) {this->fCrossCheckInEtaBinNo = crossCheckInEtaBinNo;};
  Int_t GetCrossCheckInEtaBinNo() const {return this->fCrossCheckInEtaBinNo;};
  void SetDiffFlowDirectCorrelations(TProfile* const diffFlowDirectCorrelations,Int_t const i,Int_t const j,Int_t const k){this->fDiffFlowDirectCorrelations[i][j][k]=diffFlowDirectCorrelations;};
  TProfile* GetDiffFlowDirectCorrelations(Int_t i, Int_t j, Int_t k) const {return this->fDiffFlowDirectCorrelations[i][j][k];};
  void SetDiffFlowDirectCorrectionTermsForNUA(TProfile* const dfdctfn, Int_t const i, Int_t const j, Int_t const k, Int_t const l) {this->fDiffFlowDirectCorrectionTermsForNUA[i][j][k][l] = dfdctfn;};
  TProfile* GetDiffFlowDirectCorrectionTermsForNUA(Int_t i, Int_t j, Int_t k, Int_t l) const {return this->fDiffFlowDirectCorrectionTermsForNUA[i][j][k][l];};  
        
 private:
  
  AliFlowAnalysisWithQCumulants(const AliFlowAnalysisWithQCumulants& afawQc);
  AliFlowAnalysisWithQCumulants& operator=(const AliFlowAnalysisWithQCumulants& afawQc); 
  
  // 0.) base:
  TList* fHistList; // base list to hold all output object
  
  // 1.) common:
  AliFlowCommonHist *fCommonHists; // common control histograms (taking into account ALL events) 
  AliFlowCommonHist *fCommonHists2nd; // common control histograms (taking into account only the events with 2 and more particles) 
  AliFlowCommonHist *fCommonHists4th; // common control histograms (taking into account only the events with 4 and more particles) 
  AliFlowCommonHist *fCommonHists6th; // common control histograms (taking into account only the events with 6 and more particles) 
  AliFlowCommonHist *fCommonHists8th; // common control histograms (taking into account only the events with 8 and more particles) 
  AliFlowCommonHistResults *fCommonHistsResults2nd; // final results for 2nd order int. and diff. flow for events with 2 and more particles
  AliFlowCommonHistResults *fCommonHistsResults4th; // final results for 4th order int. and diff. flow for events with 4 and more particles 
  AliFlowCommonHistResults *fCommonHistsResults6th; // final results for 6th order int. and diff. flow for events with 6 and more particles
  AliFlowCommonHistResults *fCommonHistsResults8th; // final results for 8th order int. and diff. flow for events with 8 and more particles
  Int_t fnBinsPhi; // number of phi bins
  Double_t fPhiMin; // minimum phi   
  Double_t fPhiMax; // maximum phi 
  Double_t fPhiBinWidth; // bin width for phi histograms  
  Int_t fnBinsPt; // number of pt bins
  Double_t fPtMin; // minimum pt   
  Double_t fPtMax; // maximum pt  
  Double_t fPtBinWidth; // bin width for pt histograms  
  Int_t fnBinsEta; // number of eta bins
  Double_t fEtaMin; // minimum eta   
  Double_t fEtaMax; // maximum eta
  Double_t fEtaBinWidth; // bin width for eta histograms  
  Int_t fHarmonic; // harmonic 
  TString *fAnalysisLabel; // analysis label (all histograms and output file will have this label)
  
  // 2a.) particle weights:
  TList *fWeightsList; // list to hold all histograms with particle weights: fUseParticleWeights, fPhiWeights, fPtWeights and fEtaWeights
  Bool_t fUsePhiWeights; // use phi weights
  Bool_t fUsePtWeights; // use pt weights
  Bool_t fUseEtaWeights; // use eta weights
  TProfile *fUseParticleWeights; // profile with three bins to hold values of fUsePhiWeights, fUsePtWeights and fUseEtaWeights
  TH1F *fPhiWeights; // histogram holding phi weights
  TH1D *fPtWeights; // histogram holding phi weights
  TH1D *fEtaWeights; // histogram holding phi weights 
  
  // 2b.) event weights:
  TString *fMultiplicityWeight; // event-by-event weights for multiparticle correlations
  
  // 3.) integrated flow       
  //  3a.) lists:
  TList *fIntFlowList; // list to hold all histograms and profiles relevant for integrated flow 
  TList *fIntFlowProfiles; // list to hold all profiles relevant for integrated flow
  TList *fIntFlowResults; // list to hold all histograms with final results relevant for integrated flow  
  //  3b.) flags:
  TProfile *fIntFlowFlags; // profile to hold all flags for integrated flow
  Bool_t fApplyCorrectionForNUA; // apply correction for non-uniform acceptance
  //  3c.) event-by-event quantities:
  TMatrixD *fReQ; // fReQ[m][k] = sum_{i=1}^{M} w_{i}^{k} cos(m*phi_{i})
  TMatrixD *fImQ; // fImQ[m][k] = sum_{i=1}^{M} w_{i}^{k} sin(m*phi_{i})
  TMatrixD *fSMpk; // fSM[p][k] = (sum_{i=1}^{M} w_{i}^{k})^{p}
  TH1D *fIntFlowCorrelationsEBE; // 1st bin: <2>, 2nd bin: <4>, 3rd bin: <6>, 4th bin: <8>
  TH1D *fIntFlowEventWeightsForCorrelationsEBE; // 1st bin: eW_<2>, 2nd bin: eW_<4>, 3rd bin: eW_<6>, 4th bin: eW_<8>
  TH1D *fIntFlowCorrelationsAllEBE; // to be improved (add comment)
  TH1D *fIntFlowCorrectionTermsForNUAEBE[2]; // [0=sin terms,1=cos terms], NUA = non-uniform acceptance
  //  3d.) profiles:
  TProfile *fAvMultiplicity; // profile to hold average multiplicities and number of events for events with nRP>=0, nRP>=1, ... , and nRP>=8
  TProfile *fIntFlowCorrelationsPro; // average correlations <<2>>, <<4>>, <<6>> and <<8>> (with wrong errors!) 
  TProfile *fIntFlowCorrelationsAllPro; // average all correlations for integrated flow (with wrong errors!)
  TProfile *fIntFlowExtraCorrelationsPro; // when particle weights are used some extra correlations appear 
  TProfile *fIntFlowProductOfCorrelationsPro; // average product of correlations <2>, <4>, <6> and <8>:  
  TProfile *fIntFlowCorrectionTermsForNUAPro[2]; // average correction terms for non-uniform acceptance (with wrong errors!) [0=sin terms,1=cos terms] 
  //  3e.) histograms with final results:
  TH1D *fIntFlowCorrelationsHist; // final results for average correlations <<2>>, <<4>>, <<6>> and <<8>> (with correct errors!) 
  TH1D *fIntFlowCorrelationsAllHist; // final results for all average correlations (with correct errors!) 
  TH1D *fIntFlowCorrectionTermsForNUAHist[2];// final results for correction terms for non-uniform acceptance (with correct errors!) [0=sin terms,1=cos terms]
  TH1D *fIntFlowCovariances; // final result for covariances of correlations (multiplied with weight dependent prefactor)
  TH1D *fIntFlowSumOfEventWeights[2]; // sum of linear and quadratic event weights for <2>, <4>, <6> and <8>: [0=linear 1,1=quadratic]
  TH1D *fIntFlowSumOfProductOfEventWeights; // sum of products of event weights for correlations <2>, <4>, <6> and <8>
  TH1D *fIntFlowQcumulants; // final results for integrated Q-cumulants QC{2}, QC{4}, QC{6} and QC{8}
  TH1D *fIntFlow; // final results for integrated flow estimates v_n{2,QC}, v_n{4,QC}, v_n{6,QC} and v_n{8,QC}
     
  // 4.) differential flow
  //  4a.) lists:
  TList *fDiffFlowList; // list to hold list with all histograms (fDiffFlowResults) and list with profiles (fDiffFlowProfiles) relevant for differential flow 
  TList *fDiffFlowProfiles; // list to hold all profiles relevant for differential flow
  TList *fDiffFlowResults; // list to hold all histograms with final results relevant for differential flow  
  //    4aa.) nested list in list fDiffFlowProfiles: 
  TList *fDiffFlowCorrelationsProList[2][2]; // list to hold profiles with all correlations for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowProductOfCorrelationsProList[2][2]; // list to hold profiles with products of all correlations for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowCorrectionsProList[2][2]; // list to hold profiles with correction term for NUA for differential flow [0=RP,1=POI][0=pt,1=eta] 
  //    4ab.) nested list in list fDiffFlowResults: 
  TList *fDiffFlowCorrelationsHistList[2][2]; // list to hold histograms with all correlations for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowSumOfEventWeightsHistList[2][2][2]; // list to hold histograms with sum of linear/quadratic event weights [0=RP,1=POI][0=pt,1=eta][0=linear 1,1=quadratic]
  TList *fDiffFlowSumOfProductOfEventWeightsHistList[2][2]; // list to hold histograms with sum of products of event weights [0=RP,1=POI][0=pt,1=eta]
  TList *fDiffFlowCorrectionsHistList[2][2]; // list to hold histograms with correction term for NUA for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowCovariancesHistList[2][2]; // list to hold histograms with all covariances for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowCumulantsHistList[2][2]; // list to hold histograms with all cumulants for differential flow [0=RP,1=POI][0=pt,1=eta] 
  TList *fDiffFlowHistList[2][2]; // list to hold histograms with final results for differential flow [0=RP,1=POI][0=pt,1=eta]
  //  4b.) flags:  
  TProfile *fDiffFlowFlags; // profile to hold all flags for differential flow
  Bool_t fCalculate2DFlow; // calculate differential flow in (pt,eta) (Remark: this is very expensive in terms of CPU time)
  //  4c.) event-by-event quantities:
  // 1D:
  TProfile *fReRPQ1dEBE[3][2][4][9]; // real part [0=r,1=p,2=q][0=pt,1=eta][m][k]
  TProfile *fImRPQ1dEBE[3][2][4][9]; // imaginary part [0=r,1=p,2=q][0=pt,1=eta][m][k]
  TProfile *fs1dEBE[3][2][9]; // [0=r,1=p,2=q][0=pt,1=eta][k] // to be improved
  TH1D *fDiffFlowCorrelationsEBE[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][reduced correlation index]
  TH1D *fDiffFlowEventWeightsForCorrelationsEBE[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][event weights for reduced correlation index]
  TH1D *fDiffFlowCorrectionTermsForNUAEBE[2][2][2][10]; // [0=RP,1=POI][0=pt,1=eta][0=sin terms,1=cos terms][correction term index]
  // 2D:
  TProfile2D *fReRPQ2dEBE[3][4][9]; // real part of r_{m*n,k}(pt,eta), p_{m*n,k}(pt,eta) and q_{m*n,k}(pt,eta)
  TProfile2D *fImRPQ2dEBE[3][4][9]; // imaginary part of r_{m*n,k}(pt,eta), p_{m*n,k}(pt,eta) and q_{m*n,k}(pt,eta)
  TProfile2D *fs2dEBE[3][9]; // [t][k] // to be improved
  //  4d.) profiles:
  // 1D:
  TProfile *fDiffFlowCorrelationsPro[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][correlation index]
  TProfile *fDiffFlowProductOfCorrelationsPro[2][2][8][8]; // [0=RP,1=POI][0=pt,1=eta] [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>] x 
                                                           //                          [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>]
  TProfile *fDiffFlowCorrectionTermsForNUAPro[2][2][2][10]; // [0=RP,1=POI][0=pt,1=eta][0=sin terms,1=cos terms][correction term index]
                                                              
  //  4e.) histograms holding final results:
  // 1D:
  TH1D *fDiffFlowCorrelationsHist[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][correlation index]
  TH1D *fDiffFlowCovariances[2][2][5]; // [0=RP,1=POI][0=pW not used,1=pW used][0=exact eW,1=non-exact eW][0=pt,1=eta][index of covariances] 
  TH1D *fDiffFlowCumulants[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][0=QC{2'},1=QC{4'},2=QC{6'},3=QC{8'}]
  TH1D *fDiffFlow[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][0=v'{2},1=v'{4},2=v'{6},3=v'{8}]
  TH1D *fDiffFlowSumOfEventWeights[2][2][2][4]; // [0=RP,1=POI][0=pt,1=eta][0=linear 1,1=quadratic][0=<2'>,1=<4'>,2=<6'>,3=<8'>]
  TH1D *fDiffFlowSumOfProductOfEventWeights[2][2][8][8]; // [0=RP,1=POI][0=pt,1=eta]  [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>] x 
                                                         //                           [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>]
  TH1D *fDiffFlowCorrectionTermsForNUAHist[2][2][2][10]; // [0=RP,1=POI][0=pt,1=eta][0=sin terms,1=cos terms][correction term index]
       
  // 2D:
  TProfile2D *fCorrelationsPro[2][2][2][4]; // [0=RP,1=POI][0=pWeights not used,1=pWeights used][0=exact eWeights,1=non-exact eWeights][corr.'s index]
  TProfile2D *fProductsOfCorrelationsPro[2][2][2][5]; // [0=RP,1=POI][0=pW not used,1=pW used][0=exact eWeights,1=non-exact eWeights][products' index]
  TProfile2D *fCorrectionTermsPro[2][2][2][2][2]; // [0=RP,1=POI][0=pW not used,1=pW used][0=e eW,1=ne eW][0=sin terms,1=cos terms][corr. terms' index]
        
  // 5.) distributions:
  TList *fDistributionsList; // list to hold all distributions
  TH1D *fDistributions[2][2][4]; // [0=pWeights not used,1=pWeights used][0=exact eWeights,1=non-exact eWeights][0=<2>,1=<4>,2=<6>,3=<8>]
    
  // x.) debugging and cross-checking:
  TList *fNestedLoopsList; // list to hold all profiles filled with nested loops
  Bool_t fEvaluateIntFlowNestedLoops; // evaluate nested loops relevant for integrated flow
  Bool_t fEvaluateDiffFlowNestedLoops; // evaluate nested loops relevant for differential flow
  Int_t fMaxAllowedMultiplicity; // nested loops will be evaluated only for events with multiplicity <= fMaxAllowedMultiplicity
  TProfile *fEvaluateNestedLoops; // profile with four bins: fEvaluateIntFlowNestedLoops, fEvaluateDiffFlowNestedLoops, fCrossCheckInPtBinNo and fCrossCheckInEtaBinNo 
  // integrated flow:
  TProfile *fIntFlowDirectCorrelations; // multiparticle correlations relevant for int. flow calculated with nested loops  
  TProfile *fIntFlowExtraDirectCorrelations; // when particle weights are used some extra correlations appear   
  TProfile *fIntFlowDirectCorrectionTermsForNUA[2]; // average correction terms for non-uniform acceptance evaluated with nested loops [0=sin terms,1=cos terms] 
  // differential flow:
  Int_t fCrossCheckInPtBinNo; // cross-check results for reduced correlations and corrections in this pt bin
  Int_t fCrossCheckInEtaBinNo; // cross-check results for reduced correlations and corrections in this eta bin
  TProfile *fDiffFlowDirectCorrelations[2][2][4]; // [0=RP,1=POI][0=pt,1=eta][correlation index]
  TProfile *fDiffFlowDirectCorrectionTermsForNUA[2][2][2][10]; // [0=RP,1=POI][0=pt,1=eta][0=sin terms,1=cos terms][correction term index]
                  
  ClassDef(AliFlowAnalysisWithQCumulants, 0);
};

//================================================================================================================

#endif






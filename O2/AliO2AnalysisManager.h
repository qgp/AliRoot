/// \file AliO2AnalysisManager.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2AnalysisManager class.
///
/// See implementation file for copyright details.

#ifndef ALIO2ANALYSISMANAGER_H
#define ALIO2ANALYSISMANAGER_H

// Include the base classes
#include <AliAnalysisManager.h>

#include "O2NotImplemented.h"
#include <list>
#include <string>
#include <utilities/logging.h>
/// Timeframe aware AliAnalysisManager for O2
///
/// Behaves the same as a normal AliAnalysisManager unless it detects that the
/// input is a timeframe, then it will correctly handle multiple events in a
/// timeframe

class AliO2AnalysisManager : public AliAnalysisManager {
public:
  using AliAnalysisManager::AliAnalysisManager;
  // /// Default constructor
  // AliO2AnalysisManager();
  /// Destructor
  ~AliO2AnalysisManager();
  void ExecAnalysis(Option_t *option = "");
  virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0) { return 0; }
  Bool_t initialize(const std::list<std::string> &tree);
  virtual Bool_t Notify() { return kTRUE; }
  virtual Bool_t ProcessCut(Long64_t entry) { return Process(entry); }
  virtual Bool_t Process(Long64_t entry) { return kTRUE; };
  void doAnalysis(const std::list<std::string> &files) {
    std::cout << "Starting to doAnalysis " << std::endl;
    initialize(files);
    this->AliO2AnalysisManager::ExecAnalysis("");
  }

protected:
  // protected stuff goes here

private:
  Bool_t inputIsO2 = kFALSE;

  // root specific
  ClassDef(AliO2AnalysisManager, 1);
};

#endif

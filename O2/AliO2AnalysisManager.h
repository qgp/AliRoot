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
#include <AliO2InputHandler.h>
#include <TChain.h>
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
  virtual ~AliO2AnalysisManager();
  // overwritten
  virtual void ExecAnalysis(Option_t *option = "");

  // new
  Bool_t initialize(const std::list<std::string> &tree);
  // new
  void doAnalysis(const std::list<std::string> &files) {
    if (!dynamic_cast<AliO2InputHandler *>(this->GetInputEventHandler())) {
      TChain *chain = new TChain("esdTree");
      for (auto it = files.begin(); it != files.end(); it++) {
        chain->Add(it->c_str());
      }
      // call init
      InitAnalysis();
      // and perform analysis.
      report(INFO, "start analysis");
      AliAnalysisManager::StartAnalysis("local", chain);
      return;
    }
    // call init
    initialize(files);
    // and perform analysis.
    this->AliO2AnalysisManager::ExecAnalysis("");
  }

protected:
  // protected stuff goes here

private:
  // root specific
  ClassDef(AliO2AnalysisManager, 1);
};

#endif

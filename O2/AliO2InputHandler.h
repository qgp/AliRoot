/// \file AliO2InputHandler.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2InputHandler class.
///
/// See implementation file for copyright details.

#ifndef ALIO2InputHandler_H
#define ALIO2InputHandler_H

// Include the base classes
#include <AliInputEventHandler.h>

#include "AliO2Event.h"
#include "AliO2Timeframe.h"
#include "O2NotImplemented.h"
#include "O2Timeframe.h"
#include <AliLog.h>
#include <TFile.h>
#include <list>
#include <utilities/logging.h>
/// Short AliO2InputHandler description
///
/// More detailed AliO2InputHandler description

class AliO2InputHandler : public AliInputEventHandler {
public:
  /// Default constructor
  AliO2InputHandler(){};
  /// Destructor
  ~AliO2InputHandler(){};
  Bool_t NextBranch() { return kTRUE; }
  /// Get the next event from the current timeframe, returns false if none
  /// exist.
  virtual Bool_t NextEvent() {
    while (!mCurrentFile) {
      if (0 == mFiles.size()) {
        report(PASS, "Proccessed all files");
        return kFALSE;
      }
      std::string &filename = mFiles.front();
      mCurrentFile = TFile::Open(filename.c_str(), "read");
      if (mCurrentFile) {
        report(INFO, "Opened file %s", filename.c_str());
        mCurrentEvent = 0;
        mTimeframe = dynamic_cast<O2Timeframe *>(
            mCurrentFile->GetDirectory("MyTask")->Get("O2Timeframe"));
      } else {
        report(WARN, "Couldn't open file %s", filename.c_str());
      }
      mFiles.pop_front();
    }

    if (mCurrentEvent >= mTimeframe->getNumberOfEvents()) {
      delete mCurrentFile;
      mCurrentFile = nullptr;
      return NextEvent();
    } else {
      mEvent = AliO2Event(mTimeframe->getEvent(mCurrentEvent++));
      report(PASS, "Fetched event %d/%lu  ", mCurrentEvent,
             mTimeframe->getNumberOfEvents());
      return kTRUE;
    }
  }
  virtual Bool_t Init(TTree *tree, Option_t *opt = "") { return kTRUE; }
  Bool_t initialize(const std::list<std::string> &files) {
    std::cout << "Starting to Init handler" << std::endl;
    delete mCurrentFile;
    mCurrentFile = nullptr;
    mFiles = files;
    return kTRUE;
  }

  /// Returns the current O2Event
  virtual AliVEvent *GetEvent() const {
    report(INFO, "event requested");
    return const_cast<AliO2Event *>(&mEvent);
  }
  // Reads the specified timeframe from the tree.
  virtual Bool_t BeginEvent(Long64_t entry) { return kTRUE; }
  virtual Bool_t Notify() { return AliInputEventHandler::Notify(); }
  // says stuff
  virtual Bool_t Notify(const char *path) {
    AliInfo(Form("This should be a notification on %s", path));
    return kTRUE;
  }
  // cleanup
  virtual Bool_t FinishEvent() {
    // delete mTimeframe;
    return kTRUE;
  }

  virtual Option_t *GetDataType() const { return "O2"; }
  virtual void SetInputFileName(const char *fname){};
  virtual const char *GetInputFileName() const {
    return mFiles.front().c_str();
  }
  virtual void SetOutputFileName(const char * /*fname*/) { ; }
  virtual const char *GetOutputFileName() const { return 0; }
  virtual Bool_t Init(Option_t *opt) { return kTRUE; }
  virtual Bool_t GetEntry() { return kTRUE; }
  virtual Bool_t NeedField() const { return TObject::TestBit(kNeedField); }
  virtual Bool_t Terminate() { return kTRUE; }
  virtual Bool_t TerminateIO() { return kTRUE; }
  // Setters
  virtual void SetInputTree(TTree *tree) {}
  virtual void SetEventSelection(AliVCuts *cuts) {}
  //
  void SetInactiveBranches(const char *branches) {}
  void SetActiveBranches(const char *branches) {}
  virtual TObject *GetStatistics(Option_t *option = "") const { return NULL; }
  virtual Long64_t GetReadEntry() const { return 0; }
  virtual Bool_t NewEvent() {
    Bool_t ne = fNewEvent;
    fNewEvent = kFALSE;
    return ne;
  }

  /// Sets the chi-squared probablity limit of overlap.
  ///
  /// \par threshold The lower (inclusive) overlap-probablity threshold
  /// (chi-squared value) that will associate different timestampped objects in
  /// time.
  // void SetTemporalAssociationThreshold(Double_t threshold);

protected:
  // protected stuff goes here

private:
  O2Timeframe *mTimeframe;
  AliO2Event mEvent;
  Int_t mCurrentEvent = 0;
  TFile *mCurrentFile = nullptr;
  std::list<std::string> mFiles;
  /// copy constructor prohibited
  AliO2InputHandler(const AliO2InputHandler &);
  /// assignment operator prohibited
  AliO2InputHandler &operator=(const AliO2InputHandler &);

  // root specific
  ClassDef(AliO2InputHandler, 1);
};

#endif

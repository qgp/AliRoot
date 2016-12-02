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
#include "O2Timeframe.h"
#include <AliLog.h>
/// Short AliO2InputHandler description
///
/// More detailed AliO2InputHandler description

class AliO2InputHandler : public AliInputEventHandler {
public:
  /// Default constructor
  AliO2InputHandler() { mEvent = new AliO2Event(); };
  /// Destructor
  ~AliO2InputHandler() { delete mEvent; };
  virtual Bool_t Init(TTree *tree, Option_t *opt);

  /// Returns the current O2Event
  AliO2Event *GetEvent() const { return mEvent; }
  // Read the specified entry/get new event?
  virtual Bool_t BeginEvent(Long64_t entry) {
    if (entry > mTimeframe.getNumberOfEvents()) {
      AliError(Form("Entry %lld is not in timeframe!", entry));
      return kFALSE;
    }
    O2Event event = mTimeframe.getEvent(entry);
    mEvent = new AliO2Event(&event);
    return kTRUE;
  }
  virtual Bool_t Notify() { return AliInputEventHandler::Notify(); }
  // says stuff
  virtual Bool_t Notify(const char *path) {
    AliInfo(Form("This should be a notification on %s", path));
    return kTRUE;
  }
  // cleanup
  virtual Bool_t FinishEvent() {
    delete mEvent;
    return kTRUE;
  }

  Option_t *GetDataType() const { return nullptr; }

  /// Sets the chi-squared probablity limit of overlap.
  ///
  /// \par threshold The lower (inclusive) overlap-probablity threshold
  /// (chi-squared value) that will associate different timestampped objects in
  /// time.
  // void SetTemporalAssociationThreshold(Double_t threshold);

protected:
  // protected stuff goes here

private:
  O2Timeframe mTimeframe;
  AliO2Event *mEvent = nullptr;
  /// copy constructor prohibited
  AliO2InputHandler(const AliO2InputHandler &);
  /// assignment operator prohibited
  AliO2InputHandler &operator=(const AliO2InputHandler &);

  // root specific
  ClassDef(AliO2InputHandler, 1);
};

#endif

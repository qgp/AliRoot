/// \file AliO2AnalysisTaskSE.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2AnalysisTaskSE class.
///
/// See implementation file for copyright details.

#ifndef ALIO2ANALYSISTASKSE_H
#define ALIO2ANALYSISTASKSE_H

#include <AliO2InputHandler.h>
// Include the base classes
#include <AliAnalysisTaskSE.h>

/// Short AliO2AnalysisTaskSE description
///
/// More detailed AliO2AnalysisTaskSE description
class AliO2AnalysisTaskSE : public AliAnalysisTaskSE {
public:
  using AliAnalysisTaskSE::AliAnalysisTaskSE;
  /// Default constructor
  AliO2AnalysisTaskSE();
  /// Destructor
  ~AliO2AnalysisTaskSE();
  virtual AliVEvent *InputEvent() const {
    auto handler = dynamic_cast<AliO2InputHandler *>(this->fInputHandler);
    return handler->GetEvent();
  }

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  AliO2AnalysisTaskSE(const AliO2AnalysisTaskSE &);
  /// assignment operator prohibited
  AliO2AnalysisTaskSE &operator=(const AliO2AnalysisTaskSE &);
};

#endif

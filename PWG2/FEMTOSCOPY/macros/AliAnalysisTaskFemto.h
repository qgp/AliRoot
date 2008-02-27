//------------------------------------------------------
// AliAnalysisTaskFemto - A task for the analysis framework
// from the FEMTOSCOPY analysis of PWG2. Creates the necessary
// connection between the ESD or AOD input and the femtoscopic
// code.
// Author: Adam Kisiel, OSU; Adam.Kisiel@cern.ch
//------------------------------------------------------
#ifndef ALIANALYSISTASKFEMTO_H
#define ALIANALYSISTASKFEMTO_H

#include "TH1.h"

#include "AliESDInputHandler.h"
#include "AliAODInputHandler.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisDataContainer.h"

#include "AliFemtoEventReaderESDChain.h"
#include "AliFemtoEventReaderAODChain.h"
#include "AliFemtoManager.h"

class AliAnalysisTaskFemto : public AliAnalysisTask {
 public:
  AliAnalysisTaskFemto() : AliAnalysisTask(), fESD(0), fAOD(0), fOutputList(0), fReaderESD(0x0), fReaderAOD(0x0), fManager(0x0), fAnalysisType(0) {}
  AliAnalysisTaskFemto(const char *name);
  virtual ~AliAnalysisTaskFemto() {}
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);

  void SetFemtoReaderESD(AliFemtoEventReaderESDChain *aReader);
  void SetFemtoReaderAOD(AliFemtoEventReaderAODChain *aReader);
  void SetFemtoManager(AliFemtoManager *aManager);

 private:
  AliESDEvent                 *fESD;          //! ESD object
  AliAODEvent                 *fAOD;          //! AOD object
  TList                       *fOutputList;   //  AliFemto results list
  AliFemtoEventReaderESDChain *fReaderESD;    //! AliFemto reader for ESD given by the chain
  AliFemtoEventReaderAODChain *fReaderAOD;    //! AliFemto reader for AOD given by the chain
  AliFemtoManager             *fManager;      //! AliFemto top-level manager 
  int                          fAnalysisType; //  Mark ESD of AOD analysis
 
  ClassDef(AliAnalysisTaskFemto, 2); // example of analysis
};

#endif

#ifndef AliCutTask_cxx
#define AliCutTask_cxx

// simple task that runs the esd track cuts to evaluate the basic plots created during the cuts

class TH1F;
class AliESDtrackCuts;
class AliESDEvent;
class TList;

#include "AliAnalysisTask.h"

class AliCutTask : public AliAnalysisTask {
 public:
  AliCutTask(const char *name = "AliCutTask");
  virtual ~AliCutTask() {}
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);

  void SetTrackCuts(AliESDtrackCuts* cuts) { fTrackCuts = cuts; }
  
 private:
  AliESDEvent *fESD;           //! ESD object
  AliESDtrackCuts* fTrackCuts; // track cuts

  TH1F* fVertex;   //! event z vertex distribution

  TList* fOutput;                  //! list send on output slot 0

  AliCutTask(const AliCutTask&); // not implemented
  AliCutTask& operator=(const AliCutTask&); // not implemented
  
  ClassDef(AliCutTask, 1); // example of analysis
};

#endif

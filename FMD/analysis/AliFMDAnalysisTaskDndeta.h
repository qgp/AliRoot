#ifndef ALIFMDANALYSISTASKDNDETA_H
#define ALIFMDANALYSISTASKDNDETA_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
#include "AliAnalysisTask.h"

#include "TObjArray.h"
#include "TObjString.h"
#include "TArrayI.h"
#include "TH1I.h"
#include "AliMCEvent.h"
#include "AliFMDFloatMap.h"
class AliFMDAnalysisTaskDndeta : public AliAnalysisTask
{
 public:
    AliFMDAnalysisTaskDndeta();
    AliFMDAnalysisTaskDndeta(const char* name, Bool_t SE = kTRUE);
    virtual ~AliFMDAnalysisTaskDndeta() {;}
 AliFMDAnalysisTaskDndeta(const AliFMDAnalysisTaskDndeta& o) : AliAnalysisTask(),
      fDebug(o.fDebug),
      fOutputList(0),
      fInputList(0),
      fArray(o.fArray),
      fInputArray(o.fInputArray),
      fVertexString(o.fVertexString),
      fNevents(o.fNevents),
      fNMCevents(o.fNMCevents),
      fStandalone(o.fStandalone),
      fMCevent(o.fMCevent),
      fLastTrackByStrip(o.fLastTrackByStrip),
      fPrimary(o.fPrimary),
      fRecordHits(o.fRecordHits) {}
    AliFMDAnalysisTaskDndeta& operator=(const AliFMDAnalysisTaskDndeta&) { return *this; }
    // Implementation of interface methods
    virtual void ConnectInputData(Option_t *option = "");
    virtual void CreateOutputObjects();
    virtual void Init() {}
    virtual void LocalInit() {Init();}
    virtual void Exec(Option_t *option);
    virtual void Terminate(Option_t *option);
    virtual void SetDebugLevel(Int_t level) {fDebug = level;}
    void SetInputList(TList* inputList) {fInputList = inputList;}
    void SetInputVertex(TObjString* vtxString) {fVertexString = vtxString;}
    void SetOutputList(TList* outputList) {fOutputList = outputList;}
    void SetMCEvent(AliMCEvent* mcevent) {fMCevent = mcevent;}
    void ProcessPrimary();
    TList* GetOutputList() {return fOutputList;}
    void SetAnalyzePrimary(Bool_t prim) {fPrimary = prim;}
    void SetRecordHits(Bool_t recordhits) {fRecordHits = recordhits;}
 private:
    Int_t         fDebug;        //  Debug flag
    TList*        fOutputList;
    TList*        fInputList;
    TObjArray     fArray;
    TObjArray*    fInputArray;
    TObjString*   fVertexString;
    TH1I          fNevents;
    TH1I          fNMCevents;
    Bool_t        fStandalone;
    AliMCEvent*   fMCevent;
    AliFMDFloatMap fLastTrackByStrip;
    Bool_t        fPrimary;
    Bool_t        fRecordHits;
    ClassDef(AliFMDAnalysisTaskDndeta, 0); // Analysis task for FMD analysis
};
 
#endif

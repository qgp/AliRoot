#ifndef ALIHLTVEVENTINPUTHANDLER_H
#define ALIHLTVEVENTINPUTHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//     Reconstruction-specific input handler
//     Author: Andrei Gheata, CERN
//-------------------------------------------------------------------------

#ifndef ALIVEVENTHANDLER_H
#include "AliVEventHandler.h"
#endif

class TObjArray;
class AliVfriendEvent;
class AliVEvent;

class AliHLTVEventInputHandler : public AliVEventHandler {

 public:
    AliHLTVEventInputHandler();
    AliHLTVEventInputHandler(AliHLTVEventInputHandler&);
    AliHLTVEventInputHandler(const char* name, const char* title);
    virtual ~AliHLTVEventInputHandler() {}
    AliHLTVEventInputHandler& operator=(const AliHLTVEventInputHandler&) {return *this;}
    virtual Bool_t Notify() { return kFALSE; }
    virtual Bool_t Notify(const char *) {return kTRUE;}
    virtual Bool_t Init(Option_t* /*opt*/) {return kTRUE;}
    virtual Bool_t Init(TTree* /*tree*/, Option_t* /*opt*/);
    virtual Bool_t BeginEvent(Long64_t entry);
    virtual Bool_t FinishEvent();
    virtual void  SetOutputFileName(const char* /*fname*/) {};
    virtual const char* GetOutputFileName() const {return NULL;}
    // Input
    virtual void SetInputTree(TTree* /*tree*/) {};
    // Steering 
    virtual Bool_t GetEntry() {return kTRUE;}
    virtual Bool_t Terminate() {return kTRUE;}
    virtual Bool_t TerminateIO() {return kTRUE;}

    // Especially needed for HLT
    virtual Bool_t InitTaskInputData(AliVEvent* /*esdEvent*/, AliVfriendEvent* /*friendEvent*/, TObjArray* /*arrTasks*/);

    AliVEvent* GetEvent() const {return fEvent; printf("---->AliHLTVEventInputHandler: GetEvent %p\n",fEvent);}
    //AliVEvent* GetEvent() const {return NULL;}
    void  SetEvent(AliVEvent *event) {fEvent = event;}

    AliVfriendEvent* GetVfriendEvent() const {return fFriendEvent;}
    void  SetVFriendEvent(AliVfriendEvent *friendEvent) {fFriendEvent = friendEvent;}
      
 private:
    AliHLTVEventInputHandler(const AliVEventHandler& handler);             
    AliHLTVEventInputHandler& operator=(const AliVEventHandler& handler);  
    
    AliVEvent       *fEvent;          //! Pointer to the event
    AliVfriendEvent *fFriendEvent;    //! Pointer to the friend event

    ClassDef(AliHLTVEventInputHandler, 1);
};

#endif

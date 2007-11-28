#ifndef ALIESDINPUTHANDLER_H
#define ALIESDINPUTHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     ESD Input Handler realisation of the AliVEventHandler interface
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include "AliInputEventHandler.h"
#include "AliESDEvent.h"

class AliESDInputHandler : public AliInputEventHandler {

 public:
    AliESDInputHandler();
    AliESDInputHandler(const char* name, const char* title);
    virtual ~AliESDInputHandler();
    virtual Bool_t       Init(Option_t* /*opt*/) {return kTRUE;}
    virtual Bool_t       Init(TTree* tree, Option_t* opt);
    virtual Bool_t       BeginEvent(Long64_t entry);
    virtual Bool_t       FinishEvent();
    AliESDEvent         *GetEvent() const {return fEvent;}
    //
    void SetInactiveBranches(const char* branches) {fBranches = branches;}
 private:
    void SwitchOffBranches() const;
 private:
    AliESDEvent    *fEvent;      //! Pointer to the event
    TString         fBranches;   //List of branches to be switched off (separated by space
    ClassDef(AliESDInputHandler, 2);
};

#endif

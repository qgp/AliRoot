#ifndef ALIMULTIAODINPUTHANDLER_H
#define ALIMULTIAODINPUTHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD Input Handler realisation of the AliVEventHandler interface.
//     This class handles multiple events for mixing.
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include "AliInputEventHandler.h"
class TObject;
class AliAODEvent;

class AliMultiAODInputHandler : public AliInputEventHandler {

 public:
    AliMultiAODInputHandler(Int_t size);
    AliMultiAODInputHandler(const char* name, const char* title, Int_t size);
    virtual ~AliMultiAODInputHandler();
    void   SetBufferSize(Int_t size) {fBufferSize = size;}
    void   SetEventPool(TObject* pool) {fEventPool = pool;}
    Int_t  GetBufferSize()           const {return fBufferSize;}
    Int_t  GetNBuffered()            const {return fNBuffered;}
    Bool_t IsBufferReady()           const {return (fNBuffered >= fBufferSize);}
    Bool_t IsFreshBuffer()           const {return (fIndex == (fBufferSize - 1));}
	    
    TObject*       GetEventPool()    const {return fEventPool;}
    AliAODEvent*   GetEvent(Int_t iev);
    // From the interface
    virtual Bool_t Init(TTree* tree, Option_t* /*opt*/);
    virtual Bool_t FinishEvent();
 private:
    AliMultiAODInputHandler(const AliMultiAODInputHandler& handler);             
    AliMultiAODInputHandler& operator=(const AliMultiAODInputHandler& handler);  
 private:
    Int_t          fBufferSize;   // Size of the buffer
    Int_t          fNBuffered;    // Number of events actually buffered
    Int_t          fIndex;        // Pointer to most recent event
    TTree*         fTree;         // Pointer to the tree
    TObject*       fEventPool;    // Pointer to the pool
    AliAODEvent**  fEventBuffer;  // The event buffer
    ClassDef(AliMultiAODInputHandler, 1);
};

#endif

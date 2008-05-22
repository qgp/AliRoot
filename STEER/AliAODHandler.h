#ifndef ALIAODHANDLER_H
#define ALIAODHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     Implementation of the Event Handler Interface for AOD
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include "AliVEventHandler.h"

class AliAODEvent;
class TFile;
class TTree;



class AliAODHandler : public AliVEventHandler {
    
 public:
    AliAODHandler();
    AliAODHandler(const char* name, const char* title);
    virtual ~AliAODHandler();
    virtual void         SetOutputFileName(const char* fname) {fName = fname;}
    virtual const char*  GetOutputFileName() {return fName;}
    virtual Bool_t       Init(Option_t* option);
    virtual Bool_t       Init(TTree* /*tree*/, Option_t* /*option*/)  {return kTRUE;}
    virtual Bool_t       BeginEvent(Long64_t /*entry*/)  {return kTRUE;}
    virtual Bool_t       Notify() { return AliVEventHandler::Notify(); };
    virtual Bool_t       Notify(const char * /* path */) {return kTRUE;}
    virtual Bool_t       FinishEvent();
    virtual Bool_t       Terminate();
    virtual Bool_t       TerminateIO();
    //
    virtual void         SetCreateNonStandardAOD() {fIsStandard = kFALSE;}
    //
    AliAODEvent*         GetAOD()  {return fAODEvent;}
    TTree*               GetTree() {return fTreeA;}
    void                 CreateTree(Int_t flag);
    void                 FillTree();
    void                 AddAODtoTreeUserInfo();
    void                 AddBranch(const char* cname, void* addobj);
    Bool_t               IsStandard() {return fIsStandard;}
    //
    void                 SetInputTree(TTree* /*tree*/) {;}
 private:
    AliAODHandler(const AliAODHandler&);             // Not implemented
    AliAODHandler& operator=(const AliAODHandler&);  // Not implemented
 private:
    Bool_t                   fIsStandard; //! Flag for standard aod creation 
    AliAODEvent             *fAODEvent;   //! Pointer to the AOD event
    TTree                   *fTreeA;      //! tree for AOD persistency
    TFile                   *fFileA;      //! Output file
    const char              *fName;       //! Output file name
    ClassDef(AliAODHandler, 1);
};

#endif

#ifndef ALIVEVENTHANDLER_H
#define ALIVEVENTHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     Event Handler base class
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include <TNamed.h>


class AliVEventHandler : public TNamed {

 public:
    AliVEventHandler();
    AliVEventHandler(const char* name, const char* title);
    virtual ~AliVEventHandler();
    virtual void         SetOutputFileName(char* fname)  = 0;
    virtual char*        GetOutputFileName()             = 0;
    virtual Bool_t       InitIO(Option_t* opt)           = 0;
    virtual Bool_t       BeginEvent()                    = 0;
    virtual Bool_t       Notify(const char *path)                        = 0;    
    virtual Bool_t       FinishEvent()                   = 0;
    virtual Bool_t       Terminate()                     = 0;
    virtual Bool_t       TerminateIO()                   = 0;
 private :
  ClassDef(AliVEventHandler, 0);
};

#endif

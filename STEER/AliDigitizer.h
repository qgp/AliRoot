#ifndef ALIDIGITIZER_H
#define ALIDIGITIZER_H
/* Copyright(c) 1998-2001, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$*/

////////////////////////////////////////////////////////////////////////
//
//  Base Class for Detector specific Merging/Digitization   
//                  
//  Author: Jiri Chudoba (CERN)
//
////////////////////////////////////////////////////////////////////////

#include "TTask.h"

class AliRunDigitizer;

class AliDigitizer: public TTask {

 public:
// ctor with name and title
    AliDigitizer(const Text_t* name="AliDigitizer",
                const Text_t* title="AliDigitizer");
// ctor to be used with name and title
    AliDigitizer(AliRunDigitizer *manager,
                 const Text_t* name="AliDigitizer",
                 const Text_t* title="AliDigitizer");
// Copy ctor needed because there is a pointer
    AliDigitizer(const AliDigitizer &dig);
    AliDigitizer& operator=(AliDigitizer &dig)
      {dig.Copy(*this);return *this;}
      
    virtual ~AliDigitizer();
    virtual Bool_t Init() {return kTRUE;}
//    virtual void Digitize() = 0;

protected:
    Int_t GetNInputStreams() const;
    void Copy(AliDigitizer &dig) const;

    AliRunDigitizer *fManager;
    ClassDef(AliDigitizer,1) // Base class for detector digitizers
};

#endif // ALIDIGITIZER_H


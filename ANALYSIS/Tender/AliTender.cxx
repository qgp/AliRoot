/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

#include <TChain.h>
#include <TFile.h>
 
#include "AliTender.h"
#include "AliTenderSupply.h"
#include "AliAnalysisManager.h"
#include "AliCDBManager.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliLog.h"


ClassImp(AliTender)

//______________________________________________________________________________
AliTender::AliTender():
           AliAnalysisTask(),
           fDebug(0),
           fRun(0),
           fRunChanged(kFALSE),
           fCDBkey(0),
           fDefaultStorage(),
           fCDB(NULL),
           fESDhandler(NULL),
           fESD(NULL),
           fSupplies(NULL)
{
// Dummy constructor
}

//______________________________________________________________________________
AliTender::AliTender(const char* name):
           AliAnalysisTask(name, "ESD analysis tender car"),
           fDebug(0),
           fRun(0),
           fRunChanged(kFALSE),
           fCDBkey(0),
           fDefaultStorage(),
           fCDB(NULL),
           fESDhandler(NULL),
           fESD(NULL),
           fSupplies(NULL)
{
// Default constructor
  DefineInput (0, TChain::Class());
  DefineOutput(0,  AliESDEvent::Class());
}

//______________________________________________________________________________
AliTender::~AliTender()
{
// Destructor
  if (fSupplies) {
    fSupplies->Delete();
    delete fSupplies;
  }
}

//______________________________________________________________________________
void AliTender::AddSupply(AliTenderSupply *supply)
{
// Addition of supplies.
  if (!fSupplies) fSupplies = new TObjArray();
  fSupplies->Add(supply);
}
   
//______________________________________________________________________________
void AliTender::ConnectInputData(Option_t* /*option*/)
{
// Connect the input data, create CDB manager.
  if (fDebug > 1) Printf("AliTender::ConnectInputData()\n");
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) AliFatal("No tender without an analysis manager");
  fESDhandler = dynamic_cast<AliESDInputHandler *>(mgr->GetInputEventHandler());
    
  if (fESDhandler) {
     fESD = fESDhandler->GetEvent();
  } else {
     AliFatal("No ESD input event handler connected") ; 
  }
  // Create CDB manager
  if (!fDefaultStorage.Length()) AliFatal("Default CDB storage not set.");
  fCDB = AliCDBManager::Instance();
  // SetDefault storage. Specific storages must be set by AliTenderSupply::Init()
  fCDB->SetDefaultStorage(fDefaultStorage);
  fRun = fESD->GetRunNumber();
  fCDB->SetRun(fRun);
  // Unlock CDB
  fCDBkey = fCDB->SetLock(kFALSE, fCDBkey);
  TIter next(fSupplies);
  AliTenderSupply *supply;
  while ((supply=(AliTenderSupply*)next())) supply->Init();
  // Lock CDB
  fCDBkey = fCDB->SetLock(kTRUE, fCDBkey);
}

//______________________________________________________________________________
void AliTender::CreateOutputObjects()
{
// Nothing for the moment, but we may need ESD event replication here.
  if (fDebug > 1) Printf("AliTender::CreateOutputObjects()\n");
}

//______________________________________________________________________________
void AliTender::Exec(Option_t* /*option*/)
{
//
// Execute all supplied analysis of one event. Notify run change via RunChanged().
  if (fDebug > 1) {
    Long_t entry = fESDhandler->GetReadEntry();
    Printf("AliTender::Exec() %s ==> processing event %lld\n", fESDhandler->GetTree()->GetCurrentFile()->GetName(),entry);
  }  
  fESD = fESDhandler->GetEvent();

// Call the user analysis
  // Unlock CDB
  fCDBkey = fCDB->SetLock(kFALSE, fCDBkey);
  // Intercept when the run number changed
  if (fRun != fESD->GetRunNumber()) {
    fRunChanged = kTRUE;
    fRun = fESD->GetRunNumber();
    fCDB->SetRun(fRun);
  }
  TIter next(fSupplies);
  AliTenderSupply *supply;
  while ((supply=(AliTenderSupply*)next())) supply->ProcessEvent();
  fRunChanged = kFALSE;
  // Lock CDB
  fCDBkey = fCDB->SetLock(kTRUE, fCDBkey);
  PostData(0, fESD);
}

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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TPC reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliTPCReconstructor.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliTPC.h"
#include "AliTPCclustererMI.h"
#include "AliTPCtrackerMI.h"
#include "AliTPCpidESD.h"


ClassImp(AliTPCReconstructor)


//_____________________________________________________________________________
void AliTPCReconstructor::Reconstruct(AliRunLoader* runLoader) const
{
// reconstruct clusters

  AliLoader* loader = runLoader->GetLoader("TPCLoader");
  if (!loader) {
    Error("Reconstruct", "TPC loader not found");
    return;
  }
  loader->LoadRecPoints("recreate");
  loader->LoadDigits("read");

  AliTPCParam* param = GetTPCParam(runLoader);
  if (!param) return;
  AliTPCclustererMI clusterer(param);
  Int_t nEvents = runLoader->GetNumberOfEvents();

  for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
    runLoader->GetEvent(iEvent);

    TTree* treeClusters = loader->TreeR();
    if (!treeClusters) {
      loader->MakeTree("R");
      treeClusters = loader->TreeR();
    }
    TTree* treeDigits = loader->TreeD();
    if (!treeDigits) {
      Error("Reconstruct", "Can't get digits tree !");
      return;
    }

    clusterer.SetInput(treeDigits);
    clusterer.SetOutput(treeClusters);
    clusterer.Digits2Clusters();
         
    loader->WriteRecPoints("OVERWRITE");
  }

  loader->UnloadRecPoints();
  loader->UnloadDigits();
}

//_____________________________________________________________________________
AliTracker* AliTPCReconstructor::CreateTracker(AliRunLoader* runLoader) const
{
// create a TPC tracker

  AliTPCParam* param = GetTPCParam(runLoader);
  if (!param) return NULL;
  return new AliTPCtrackerMI(param);
}

//_____________________________________________________________________________
void AliTPCReconstructor::FillESD(AliRunLoader* /*runLoader*/, 
				  AliESD* esd) const
{
// make PID

  Double_t parTPC[] = {47., 0.10, 10.};
  AliTPCpidESD tpcPID(parTPC);
  tpcPID.MakePID(esd);
}


//_____________________________________________________________________________
AliTPCParam* AliTPCReconstructor::GetTPCParam(AliRunLoader* runLoader) const
{
// get the TPC parameters

  if (!runLoader->GetAliRun()) runLoader->LoadgAlice();
  if (!runLoader->GetAliRun()) {
    Error("GetTPCParam", "couldn't get AliRun object");
    return NULL;
  }
  AliTPC* tpc = (AliTPC*) runLoader->GetAliRun()->GetDetector("TPC");
  if (!tpc) {
    Error("GetTPCParam", "couldn't get TPC detector");
    return NULL;
  }
  if (!tpc->GetParam()) {
    Error("GetTPCParam", "no TPC parameters available");
    return NULL;
  }
  return tpc->GetParam();
}

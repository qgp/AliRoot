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
 
/*
$Log$
Revision 1.3  2002/10/22 14:45:34  alibrary
Introducing Riostream.h

Revision 1.2  2002/10/14 14:57:00  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.1.2.1  2002/07/24 09:27:50  alibrary
Updating on VirtualMC

Revision 1.1  2002/06/10 17:32:17  nilsen
New Fastpoint merger added.

*/

#include <stdlib.h>
#include <Riostream.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>

#include <AliRun.h>
#include <AliRunLoader.h>
#include <AliLoader.h>
#include <AliRunDigitizer.h>

#include "AliITSFDigitizer.h"
// #include "AliITSpList.h"
#include "AliITSmodule.h"
#include "AliITSgeom.h"
#include "AliITSsimulationFastPoints.h"

ClassImp(AliITSFDigitizer)

//______________________________________________________________________
AliITSFDigitizer::AliITSFDigitizer() : AliDigitizer(){
//
// Default constructor.
//
    fITS      = 0;
    fInit     = kFALSE;
}
//______________________________________________________________________
AliITSFDigitizer::AliITSFDigitizer(AliRunDigitizer *mngr) : AliDigitizer(mngr){
//
// Standard constructor.
//
    fITS      = 0;
    fInit     = kFALSE;
}
//______________________________________________________________________
AliITSFDigitizer::~AliITSFDigitizer(){
//
// Default destructor. 
//
    fITS = 0; // don't delete fITS. Done else where.
}
//______________________________________________________________________
Bool_t AliITSFDigitizer::Init(){
//
// Initialization. 
// loads ITS and ITSgeom.
// Inputs:
//      none.
// Outputs:
//      none.

  
  fInit = kFALSE;
  if(!gAlice) {
    fITS      = 0;
    Warning("Init","gAlice not found");
    return fInit;
  }
  fITS = (AliITS *)(gAlice->GetDetector("ITS"));
  if(!fITS){
    Warning("Init","ITS not found");
    return fInit;
  } 
  if(!fITS->GetITSgeom()){
    Warning("Init","ITS geometry not found");
    return fInit;
  }
  return fInit = kTRUE;
}
////////////////////////////////////////////////////////////////////////
void AliITSFDigitizer::Exec(Option_t* opt){
//
// Main digitization function. 
// Inputs:
//      Option_t * opt  "deb" ... more verbose output 
//

  AliITSsimulationFastPoints *sim = new AliITSsimulationFastPoints();
  AliRunLoader* outrl = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (outrl == 0x0)
   {
     Error("Exec","Can not find Run Loader in output folder.");
     return;
   }

  AliLoader* outgime = outrl->GetLoader("ITSLoader");
  if (outgime == 0x0)
   {
     Error("Exec","Can not get TOF Loader from Output Run Loader.");
     return;
   }

  TTree* outputTreeR = outgime->TreeR();
  if (outputTreeR == 0x0)
   {
     outgime->MakeTree("R");
     outputTreeR = outgime->TreeR();
   }

  TClonesArray *recPoints = fITS->RecPoints();
//  TBranch *branch =
  fITS->MakeBranchInTree(outputTreeR,"ITSRecPointsF",&recPoints,4000,0);
  
  Int_t nModules;
  fITS->InitModules(-1,nModules);

// load hits into modules
  for (Int_t iFile = 0; iFile < fManager->GetNinputs(); iFile++) 
   {
     AliRunLoader* rl = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(iFile));
     if (rl == 0x0)
      {
        Error("Exec","Can not find Run Loader in input %d folder.",iFile);
        return;
      }

     AliLoader* gime = rl->GetLoader("ITSLoader");
     if (gime == 0x0)
      {
        Error("Exec","Can not get TOF Loader from Input %d Run Loader.",iFile);
        return;
      }

     gime->LoadHits();
     fITS->FillModules(gime->TreeH(),fManager->GetMask(iFile));
     gime->UnloadHits();
   }
  
// transform hits to fast rec points

  AliITSgeom *geom = fITS->GetITSgeom();
  for(Int_t moduleIndex = 0; moduleIndex < geom->GetIndexMax(); moduleIndex++){
    sim->CreateFastRecPoints(moduleIndex);
//    branch->Fill();
    outputTreeR->Fill();
    fITS->ResetRecPoints();
  }
  outrl->WriteRecPoints("OVERWRITE");
//  outputTreeR->AutoSave();
}
////////////////////////////////////////////////////////////////////////

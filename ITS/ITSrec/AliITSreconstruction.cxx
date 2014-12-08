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
/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Class for ITS RecPoint reconstruction                               //
//                                                                     //
////////////////////////////////////////////////////////////////////////

#include <TString.h>
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliITSDetTypeRec.h"
#include "AliITSLoader.h"
#include "AliITSreconstruction.h"
#include "AliITSgeom.h"

using std::endl;
using std::cout;
ClassImp(AliITSreconstruction)

//______________________________________________________________________
AliITSreconstruction::AliITSreconstruction():
 fInit(kFALSE),
 fEnt(0),
 fEnt0(0),
 fDetTypeRec(0x0),
 fDfArp(kFALSE),
 fITSgeom(0x0),
 fLoader(0x0),
 fRunLoader(0x0)
{
    // Default constructor.
    // Inputs:
    //  none.
    // Outputs:
    //   none.
    // Return:
    //    A zero-ed constructed AliITSreconstruction class.
    fDet[0] = fDet[1] = fDet[2] = kTRUE;
}
//______________________________________________________________________

AliITSreconstruction::AliITSreconstruction(AliRunLoader *rl):
 fInit(kFALSE),
 fEnt(0),
 fEnt0(0),
 fDetTypeRec(0x0),
 fDfArp(kFALSE),
 fITSgeom(0x0),
 fLoader(0x0),
 fRunLoader(rl)
{
  fDet[0] = fDet[1] = fDet[2] = kTRUE;
}
//______________________________________________________________________
AliITSreconstruction::AliITSreconstruction(const char* filename):
 fInit(kFALSE),
 fEnt(0),
 fEnt0(0),
 fDetTypeRec(0x0),
 fDfArp(kFALSE),
 fITSgeom(0x0),
 fLoader(0x0),
 fRunLoader(0x0)
{
    // Standard constructor.
    // Inputs:
    //  const char* filename    filename containing the digits to be
    //                          reconstructed. If filename = 0 (nil)
    //                          then no file is opened but a file is
    //                          assumed to already be opened. This 
    //                          already opened file will be used.
    // Outputs:
    //   none.
    // Return:
    //    A standardly constructed AliITSreconstruction class.

    fDet[0] = fDet[1] = fDet[2] = kTRUE;

    fRunLoader = AliRunLoader::Open(filename);
    if (fRunLoader == 0x0)
     {
       Error("AliITSreconstruction","Can not load the session %s \n",filename);
       return;
     }

}


//______________________________________________________________________
AliITSreconstruction::~AliITSreconstruction(){
    //    A destroyed AliITSreconstruction class.
    
    //fITS      = 0;
    delete fRunLoader;
    
}
//______________________________________________________________________
Bool_t AliITSreconstruction::Init(){
    // Class Initilizer.
    // Inputs:
    //  none.
    // Outputs:
    //   none.
    // Return:
    //    kTRUE if no errors initilizing this class occurse else kFALSE
    Info("Init"," Init ITS reconstruction");
    if (fRunLoader == 0x0)
     {
       Error("Init","Run Loader is NULL");
       return kFALSE;
     }
    //  fRunLoader->LoadgAlice();
    //   fRunLoader->LoadHeader();  

    fLoader = (AliITSLoader*) fRunLoader->GetLoader("ITSLoader");
    if(!fLoader) {
      Error("Init","ITS loader not found");
      fInit = kFALSE;
    }

    // Now ready to init.
 
    //fRunLoader->CdGAFile();
    fITSgeom = fLoader->GetITSgeom();

    fDetTypeRec = new AliITSDetTypeRec();
    fDetTypeRec->SetITSgeom(fITSgeom);
    fDetTypeRec->SetDefaults();
    fDet[0] = fDet[1] = fDet[2] = kTRUE;
    fEnt0 = 0;

    fEnt = Int_t(fRunLoader->GetNumberOfEvents());

    fLoader->LoadDigits("read");
    fLoader->LoadRecPoints("recreate");
    if (fLoader->TreeR() == 0x0) fLoader->MakeTree("R");
 
    fDetTypeRec->SetTreeAddressD(fLoader->TreeD());
    fDetTypeRec->MakeBranchR(fLoader->TreeR());
    fDetTypeRec->SetTreeAddressR(fLoader->TreeR());

    fInit = InitRec();

    Info("Init","  Done\n\n\n");

    return fInit;
}
//______________________________________________________________________
Bool_t AliITSreconstruction::InitRec(){
    // Sets up Reconstruction part of AliITSDetType..
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.


  fDetTypeRec->SetDefaultClusterFindersV2();
  Info("InitRec","    Done\n");
  return kTRUE;
}
//______________________________________________________________________ 
void AliITSreconstruction::Exec(const Option_t *opt){
    // Main reconstruction function.
    // Inputs:
    //      Option_t * opt   list of subdetector to digitize. =0 all.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Option_t *lopt;
    Int_t evnt;
    Bool_t condition =kFALSE;
    if(opt){
      if(strstr(opt,"All")||strstr(opt,"ALL")||strstr(opt,"ITS"))condition =kTRUE;
    }
    else{
      condition = kTRUE;
    }
    if(condition){
      fDet[0] = fDet[1] = fDet[2] = kTRUE;
      lopt = "All";
    }else{
      fDet[0] = fDet[1] = fDet[2] = kFALSE;
      if(strstr(opt,"SPD")) fDet[kSPD] = kTRUE;
      if(strstr(opt,"SDD")) fDet[kSDD] = kTRUE;
      if(strstr(opt,"SSD")) fDet[kSSD] = kTRUE;
      if(fDet[kSPD] && fDet[kSDD] && fDet[kSSD]) lopt = "All";
      else lopt = opt;
    } // end if strstr(opt,...)

    if(!fInit){
      cout << "Initilization Failed, Can't run Exec." << endl;
      return;
    } // end if !fInit
    for(evnt=0;evnt<fEnt;evnt++)
     {
       //      Info("Exec","");
      Info("Exec","Processing Event %d",evnt);
      //      Info("Exec","");

      fRunLoader->GetEvent(evnt);
      if (fLoader->TreeR() == 0x0) fLoader->MakeTree("R");
      fDetTypeRec->MakeBranchR(0);
      fDetTypeRec->SetTreeAddressR(fLoader->TreeR());
      fDetTypeRec->SetTreeAddressD(fLoader->TreeD());
      fDetTypeRec->DigitsToRecPoints(fLoader->TreeD(),fLoader->TreeR(),0,lopt);
    } // end for evnt
}
//______________________________________________________________________ 
void AliITSreconstruction::SetOutputFile(TString filename){
  // Set a new file name for recpoints. 
  // It must be called before Init()
  if(!fLoader)fLoader = (AliITSLoader*) fRunLoader->GetLoader("ITSLoader");
  if(fLoader){
    Info("SetOutputFile","name for rec points is %s",filename.Data());
    fLoader->SetRecPointsFileName(filename);
  }
  else {
    Error("SetOutputFile",
    "ITS loader not available. Not possible to set name: %s",filename.Data());
  }
}

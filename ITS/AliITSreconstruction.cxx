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
Revision 1.3.4.3  2002/06/25 05:58:42  barbera
Updated classes and macros to make the ITS data structures compatible with the new IO (from M. Masera)

Revision 1.3.4.2  2002/06/06 14:23:57  hristov
Merged with v3-08-02

Revision 1.3.4.1  2002/05/31 09:37:56  hristov
First set of changes done by Piotr

Revision 1.7  2002/10/14 14:57:00  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.3.6.2  2002/10/14 13:14:08  hristov
Updating VirtualMC to v3-09-02

Revision 1.6  2002/09/09 17:30:02  nilsen
Added new creator which passes a pointer to type AliRun. Can now use either
gAlice or a local instance of AliRun.

Revision 1.5  2002/05/13 14:27:57  hristov
TreeC created once per event (M.Masera)

Revision 1.4  2002/05/10 22:31:30  nilsen
Changes by Massimo Masera to allow Recpoints and Clusters to be written
to separate files.

Revision 1.3  2002/02/06 13:52:27  barbera
gAlice deletion corrected (from M. Masera)

Revision 1.2  2002/01/31 18:52:09  nilsen
Minor change to allow the use of files that are already open. grun.C macro
that also does ITS digitizationa and Reconstruction all in one go.

Revision 1.1  2002/01/30 22:20:22  nilsen
New TTask based method to do Digits To clusters. Works with files of multiple
events in the file. Macro added to show how to use. Also the changes made
in the nessesary complilation files.

*/

#include <TROOT.h>
#include <TFile.h>
#include <TSeqCollection.h>
#include <TString.h>
#include <TClonesArray.h>

#include "AliRun.h"
#include "AliRunLoader.h"

#include "AliITS.h"
#include "AliITSDetType.h"
#include "AliITSLoader.h"
#include "AliITSreconstruction.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSsegmentationSSD.h"
#include "AliITSClusterFinderSPD.h"
#include "AliITSClusterFinderSDD.h"
#include "AliITSClusterFinderSSD.h"
#include "AliITSresponseSDD.h"
#include "AliITSgeom.h"

ClassImp(AliITSreconstruction)

//______________________________________________________________________
AliITSreconstruction::AliITSreconstruction():
 fInit(kFALSE),
 fEnt(0),
 fEnt0(0),
 fITS(0x0),
 fDfArp(kFALSE),
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
 fITS(0x0),
 fDfArp(kFALSE),
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
 fITS(0x0),
 fDfArp(kFALSE),
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
       Error("AliITSreconstruction","Can not load the session",filename);
       return;
     }
    fRunLoader->LoadgAlice();
    gAlice = fRunLoader->GetAliRun();

    if(!gAlice) {
          Error("AliITSreconstruction","gAlice not found on file. Aborting.");
          fInit = kFALSE;
          return;
      } // end if !gAlice

}
//______________________________________________________________________
AliITSreconstruction::~AliITSreconstruction(){
    //    A destroyed AliITSreconstruction class.
    delete fRunLoader;
    fITS      = 0;
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
    if (fRunLoader == 0x0)
     {
       Error("Init","Run Loader is NULL");
       return kFALSE;
     }
    fRunLoader->LoadgAlice();
    fRunLoader->LoadHeader();  

    fLoader = (AliITSLoader*) fRunLoader->GetLoader("ITSLoader");
    if(!fLoader) {
      Error("Init","ITS loader not found");
      fInit = kFALSE;
    }

    //Int_t retcode;
    fITS = (AliITS*) gAlice->GetDetector("ITS");
    if(!fITS){
      cout << "ITS not found aborting. fITS=" << fITS << endl;
      fInit = kFALSE;
      return fInit;
    } // end if !fITS
    if(!(fITS->GetITSgeom())){
      cout << "ITSgeom not found aborting."<< endl;
      fInit = kFALSE;
      return fInit;
    } // end if !GetITSgeom()
    // Now ready to init.

    fDet[0] = fDet[1] = fDet[2] = kTRUE;
    fEnt0 = 0;

    fEnt  = gAlice->GetEventsPerRun();

    fLoader->LoadDigits("read");
    fLoader->LoadRecPoints("update");
    fITS->MakeTreeC();
    fITS->SetTreeAddress();
    /*
    retcode = fRunLoader->GetEvent(fEnt0);
    if(retcode != 0){
      cerr << "Unable to load event 0 - aborting\n";
      fInit = kFALSE;
      return fInit;
    }
    */
    // finished init.
    fInit = InitRec();
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
    AliITSDetType *idt;

    // SPD
    if(fDet[kSPD]){
      idt = fITS->DetType(kSPD);
      AliITSsegmentationSPD *segSPD = (AliITSsegmentationSPD*)
                                             idt->GetSegmentationModel();
      TClonesArray *digSPD = fITS->DigitsAddress(kSPD);
      TClonesArray *recpSPD = fITS->ClustersAddress(kSPD);
      AliITSClusterFinderSPD *recSPD = new AliITSClusterFinderSPD(segSPD,
                                                    digSPD,
                                                    recpSPD);
      fITS->SetReconstructionModel(kSPD,recSPD);
    } // end if fDet[kSPD].
    // SDD
    if(fDet[kSDD]){
      idt = fITS->DetType(kSDD);
      AliITSsegmentationSDD *segSDD = (AliITSsegmentationSDD*)
                                         idt->GetSegmentationModel();
      AliITSresponseSDD *resSDD = (AliITSresponseSDD*)
                                         idt->GetResponseModel();
      TClonesArray *digSDD = fITS->DigitsAddress(kSDD);
      TClonesArray *recpSDD = fITS->ClustersAddress(kSDD);
      AliITSClusterFinderSDD *recSDD =new AliITSClusterFinderSDD(segSDD,
                                                   resSDD,
                                                 digSDD,recpSDD);
      fITS->SetReconstructionModel(kSDD,recSDD);
    } // end if fDet[kSDD]
    // SSD
    if(fDet[kSSD]){
      idt = fITS->DetType(kSSD);
      AliITSsegmentationSSD *segSSD = (AliITSsegmentationSSD*)
                                       idt->GetSegmentationModel();
      TClonesArray *digSSD = fITS->DigitsAddress(kSSD);
      AliITSClusterFinderSSD *recSSD =new AliITSClusterFinderSSD(segSSD,
                                                   digSSD);
      fITS->SetReconstructionModel(kSSD,recSSD);
    } // end if fDet[kSSD]

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

    if(strstr(opt,"All")||strstr(opt,"ALL")||strstr(opt,"ITS")||opt==0){
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

      fRunLoader->GetEvent(evnt);
      fLoader->LoadDigits("read");
      fLoader->LoadRecPoints("update");
      fITS->MakeTreeC();
//      gAlice->SetEvent(evnt);
      if(!fLoader->TreeR()) fLoader->MakeTree("R");
      fITS->MakeBranch("R");
      fITS->SetTreeAddress();
      fITS->DigitsToRecPoints(evnt,0,lopt);
    } // end for evnt
}
//______________________________________________________________________ 
void AliITSreconstruction::SetOutputFile(TString filename){
  // Set a file name for recpoints. Used only if this file is not the file
  // containing digits. This obj is deleted by AliRun.
  Error("SetOutputFile","Use AliLoader::SetRecPointsFileName(TString&)instead");
}

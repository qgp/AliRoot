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
//AliConfig::Instance() added in ctor
 

/*
$Log$
Revision 1.81.2.8  2002/11/26 16:32:46  hristov
Merging NewIO with v3-09-04

Revision 1.81.2.7  2002/11/22 14:19:50  hristov
Merging NewIO-01 with v3-09-04 (part one) (P.Skowronski)

Revision 1.81.2.6  2002/10/09 09:23:55  hristov
New task hierarchy, bug corrections, new development (P.Skowronski)

Revision 1.81.2.5  2002/06/28 16:45:17  hristov
Few minor corrections

Revision 1.81.2.4  2002/06/24 16:17:09  hristov
First reset and then begin event

Revision 1.81.2.3  2002/06/18 10:18:32  hristov
Important update (P.Skowronski)

Revision 1.81.2.2  2002/06/06 14:18:33  hristov
Merged with v3-08-02

Revision 1.81.2.1  2002/05/31 09:37:59  hristov
First set of changes done by Piotr

Revision 1.91  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.90  2002/10/22 15:02:15  alibrary
Introducing Riostream.h

Revision 1.89  2002/10/17 16:26:39  hristov
Definition of additional particles moved to VMC (I.Hrivnacova)

Revision 1.88  2002/10/14 14:57:32  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.82.4.3  2002/10/14 09:45:57  hristov
Updating VirtualMC to v3-09-02

Revision 1.82.4.2  2002/06/10 17:54:06  hristov
Only one SetGenEventHeader function kept

Revision 1.82.4.1  2002/06/10 14:43:06  hristov
Merged with v3-08-02

Revision 1.86  2002/05/28 14:24:57  hristov
Correct warning messages
 
Revision 1.85  2002/05/24 13:29:58  hristov
AliTrackReference added, AliDisplay modified

Revision 1.84  2002/05/21 16:26:07  hristov
Find correctly TreeK in case CONFIG_SPLIT_FILE is set (Y.Schutz)

Revision 1.83  2002/04/04 13:16:17  jchudoba
add possibility to write sdigits, digits and rec. points into separate files

Revision 1.82  2002/03/12 11:06:03  morsch
Add particle status code to argument list of SetTrack(..).

Revision 1.81  2001/12/19 14:46:26  morsch
Add possibility to disable StepManager() for each module separately.

Revision 1.80  2001/10/21 18:22:55  hristov
BranchOld replaced by Branch. It works correctly with Root 2.02.xx

Revision 1.79  2001/10/09 18:00:35  hristov
Temporary fix to provide unique event number in the simulation (J.Chudoba)

Revision 1.78  2001/10/04 15:30:56  hristov
Changes to accommodate the set of PHOS folders and tasks (Y.Schutz)

Revision 1.77  2001/09/04 15:09:11  hristov
fTreeE->Branch replaced temporary by fTreeE->BranchOld to avoid data corruption in case of many events per file

Revision 1.76  2001/08/03 14:38:35  morsch
Use original method to access TreeH.

Revision 1.75  2001/07/28 10:52:27  hristov
Event number updated correctly (M.Ivanov)

Revision 1.74  2001/07/28 10:39:16  morsch
GetEventsPerRun() method needed by afterburners added to AliRun.h
Corresponding setters and getters have been from AliGenerator.

Revision 1.73  2001/07/27 12:34:30  jchudoba
remove the dummy argument in fRunLoader->Stack()->GetEvent call

Revision 1.72  2001/07/03 08:10:57  hristov
J.Chudoba's changes merged correctly with the HEAD

Revision 1.70  2001/06/29 08:01:36  morsch
Small correction to the previous.

Revision 1.69  2001/06/28 16:27:50  morsch
AliReco() with user control of event range.

Revision 1.68  2001/06/11 13:14:40  morsch
SetAliGenEventHeader() method added.

Revision 1.67  2001/06/07 18:24:50  buncic
Removed compilation warning in AliConfig initialisation.

Revision 1.66  2001/05/22 14:32:40  hristov
Weird inline removed

Revision 1.65  2001/05/21 17:22:51  buncic
Fixed problem with missing AliConfig while reading galice.root

Revision 1.64  2001/05/16 14:57:22  alibrary
New files for folders and Stack

Revision 1.62  2001/04/06 11:12:33  morsch
Clear fParticles after each event. (Ivana Hrivnacova)

Revision 1.61  2001/03/30 07:04:10  morsch
Call fGenerator->FinishRun() for final print-outs, cross-section and weight calculations.

Revision 1.60  2001/03/21 18:22:30  hristov
fParticleFileMap fix (I.Hrivnacova)

Revision 1.59  2001/03/12 17:47:03  hristov
Changes needed on Sun with CC 5.0

Revision 1.58  2001/03/09 14:27:26  morsch
Fix for multiple events per file: inhibit decrease of size of  fParticleFileMap.

Revision 1.57  2001/02/23 17:40:23  buncic
All trees needed for simulation created in RunMC(). TreeR and its branches
are now created in new RunReco() method.

Revision 1.56  2001/02/14 15:45:20  hristov
Algorithmic way of getting entry index in fParticleMap. Protection of fParticleFileMap (I.Hrivnacova)

Revision 1.55  2001/02/12 15:52:54  buncic
Removed OpenBaseFile().

Revision 1.54  2001/02/07 10:39:05  hristov
Remove default value for argument

Revision 1.53  2001/02/06 11:02:26  hristov
New SetTrack interface added, added check for unfilled particles in FinishEvent (I.Hrivnacova)

Revision 1.52  2001/02/05 16:22:25  buncic
Added TreeS to GetEvent().

Revision 1.51  2001/02/02 15:16:20  morsch
SetHighWaterMark method added to mark last particle in event.

Revision 1.50  2001/01/27 10:32:00  hristov
Leave the loop when primaries are filled (I.Hrivnacova)

Revision 1.49  2001/01/26 19:58:48  hristov
Major upgrade of AliRoot code

Revision 1.48  2001/01/17 10:50:50  hristov
Corrections to destructors

Revision 1.47  2000/12/18 10:44:01  morsch
Possibility to set field map by passing pointer to objet of type AliMagF via
SetField().
Example:
gAlice->SetField(new AliMagFCM("Map2", "$(ALICE_ROOT)/data/field01.dat",2,1.,10.));

Revision 1.46  2000/12/14 19:29:27  fca
galice.cuts was not read any more

Revision 1.45  2000/11/30 07:12:49  alibrary
Introducing new Rndm and QA classes

Revision 1.44  2000/10/26 13:58:59  morsch
Add possibility to choose the lego generator (of type AliGeneratorLego or derived) when running
RunLego(). Default is the base class AliGeneratorLego.

Revision 1.43  2000/10/09 09:43:17  fca
Special remapping of hits for TPC and TRD. End-of-primary action introduced

Revision 1.42  2000/10/02 21:28:14  fca
Removal of useless dependecies via forward declarations

Revision 1.41  2000/07/13 16:19:09  fca
Mainly coding conventions + some small bug fixes

Revision 1.40  2000/07/12 08:56:25  fca
Coding convention correction and warning removal

Revision 1.39  2000/07/11 18:24:59  fca
Coding convention corrections + few minor bug fixes

Revision 1.38  2000/06/20 13:05:45  fca
Writing down the TREE headers before job starts

Revision 1.37  2000/06/09 20:05:11  morsch
Introduce possibility to chose magnetic field version 3: AliMagFDM + field02.dat

Revision 1.36  2000/06/08 14:03:58  hristov
Only one initializer for a default argument

Revision 1.35  2000/06/07 10:13:14  hristov
Delete only existent objects.

Revision 1.34  2000/05/18 10:45:38  fca
Delete Particle Factory properly

Revision 1.33  2000/05/16 13:10:40  fca
New method IsNewTrack and fix for a problem in Father-Daughter relations

Revision 1.32  2000/04/27 10:38:21  fca
Correct termination of Lego Run and introduce Lego getter in AliRun

Revision 1.31  2000/04/26 10:17:32  fca
Changes in Lego for G4 compatibility

Revision 1.30  2000/04/18 19:11:40  fca
Introduce variable Config.C function signature

Revision 1.29  2000/04/07 11:12:34  fca
G4 compatibility changes

Revision 1.28  2000/04/05 06:51:06  fca
Workaround for an HP compiler problem

Revision 1.27  2000/03/22 18:08:07  fca
Rationalisation of the virtual MC interfaces

Revision 1.26  2000/03/22 13:42:26  fca
SetGenerator does not replace an existing generator, ResetGenerator does

Revision 1.25  2000/02/23 16:25:22  fca
AliVMC and AliGeant3 classes introduced
ReadEuclid moved from AliRun to AliModule

Revision 1.24  2000/01/19 17:17:20  fca
Introducing a list of lists of hits -- more hits allowed for detector now

Revision 1.23  1999/12/03 11:14:31  fca
Fixing previous wrong checking

Revision 1.21  1999/11/25 10:40:08  fca
Fixing daughters information also in primary tracks

Revision 1.20  1999/10/04 18:08:49  fca
Adding protection against inconsistent Euclid files

Revision 1.19  1999/09/29 07:50:40  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Control class for Alice C++                                              //
//  Only one single instance of this class exists.                           //
//  The object is created in main program aliroot                            //
//  and is pointed by the global gAlice.                                     //
//                                                                           //
//   -Supports the list of all Alice Detectors (fModules).                 //
//   -Supports the list of particles (fParticles).                           //
//   -Supports the Trees.                                                    //
//   -Supports the geometry.                                                 //
//   -Supports the event display.                                            //
//Begin_Html
/*
<img src="picts/AliRunClass.gif">
*/
//End_Html
//Begin_Html
/*
<img src="picts/alirun.gif">
*/
//End_Html
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Riostream.h>
 
#include <TFile.h>
#include <TRandom.h>
#include <TBRIK.h> 
#include <TCint.h> 
#include <TSystem.h>
#include <TObjectTable.h>
#include <TTree.h>
#include <TGeometry.h>
#include <TROOT.h>
#include <TBrowser.h>
#include <TFolder.h>
#include <TKey.h>
#include <TNode.h>
#include <TParticle.h>
#include <TRandom3.h>

#include "AliRun.h"
#include "AliDisplay.h"
#include "AliMC.h"
#include "AliLego.h"
#include "AliMagFC.h"
#include "AliMagFCM.h"
#include "AliMagFDM.h"
#include "AliHit.h"
#include "AliMCQA.h"
#include "AliGenerator.h"
#include "AliLegoGenerator.h"
#include "AliConfig.h"
#include "AliStack.h"
#include "AliGenEventHeader.h"
#include "AliHeader.h"

#include "AliLoader.h"
#include "AliRunLoader.h"
#include "AliDetector.h"
#include "AliPDG.h"

AliRun *gAlice;

ClassImp(AliRun)

//_______________________________________________________________________
AliRun::AliRun():
  fRun(0),
  fEvent(0),
  fEventNrInRun(0),
  fEventsPerRun(0),
  fDebug(0),
  fModules(0),
  fGeometry(0),
  fDisplay(0),
  fTimer(),
  fField(0),
  fMC(0),
  fImedia(0),
  fNdets(0),
  fTrRmax(1.e10),
  fTrZmax(1.e10),
  fGenerator(0),
  fInitDone(kFALSE),
  fLego(0),
  fPDGDB(0),  //Particle factory object
  fHitLists(0),
  fEventEnergy(0),
  fSummEnergy(0),
  fSum2Energy(0),
  fConfigFunction("\0"),
  fRandom(0),
  fMCQA(0),
  fTransParName("\0"),
  fRunLoader(0x0)
{
  //
  // Default constructor for AliRun
  //
  AliConfig::Instance();//skowron 29 Feb 2002
                        //ensures that the folder structure is build
}

//_______________________________________________________________________
AliRun::AliRun(const AliRun& arun):
  TVirtualMCApplication(arun),
  fRun(0),
  fEvent(0),
  fEventNrInRun(0),
  fEventsPerRun(0),
  fDebug(0),
  fModules(0),
  fGeometry(0),
  fDisplay(0),
  fTimer(),
  fField(0),
  fMC(0),
  fImedia(0),
  fNdets(0),
  fTrRmax(1.e10),
  fTrZmax(1.e10),
  fGenerator(0),
  fInitDone(kFALSE),
  fLego(0),
  fPDGDB(0),  //Particle factory object
  fHitLists(0),
  fEventEnergy(0),
  fSummEnergy(0),
  fSum2Energy(0),
  fConfigFunction("\0"),
  fRandom(0),
  fMCQA(0),
  fTransParName("\0"),
  fRunLoader(0x0)
{
  //
  // Copy constructor for AliRun
  //
  arun.Copy(*this);
}

//_____________________________________________________________________________
AliRun::AliRun(const char *name, const char *title):
  TVirtualMCApplication(name,title),
  fRun(0),
  fEvent(0),
  fEventNrInRun(0),
  fEventsPerRun(0),
  fDebug(0),
  fModules(new TObjArray(77)), // Support list for the Detectors
  fGeometry(0),
  fDisplay(0),
  fTimer(),
  fField(0),
  fMC(gMC),
  fImedia(new TArrayI(1000)),
  fNdets(0),
  fTrRmax(1.e10),
  fTrZmax(1.e10),
  fGenerator(0),
  fInitDone(kFALSE),
  fLego(0),
  fPDGDB(TDatabasePDG::Instance()),        //Particle factory object!
  fHitLists(new TList()),                  // Create HitLists list
  fEventEnergy(0),
  fSummEnergy(0),
  fSum2Energy(0),
  fConfigFunction("Config();"),
  fRandom(new TRandom3()),
  fMCQA(0),
  fTransParName("\0"),
  fRunLoader(0x0)
{
  //
  //  Constructor for the main processor.
  //  Creates the geometry
  //  Creates the list of Detectors.
  //  Creates the list of particles.
  //

  gAlice     = this;

  // Set random number generator
  gRandom = fRandom;

  if (gSystem->Getenv("CONFIG_SEED")) {
     gRandom->SetSeed(static_cast<UInt_t>(atoi(gSystem->Getenv("CONFIG_SEED"))));
  }

  // Add to list of browsable  
  gROOT->GetListOfBrowsables()->Add(this,name);
  // Create the TNode geometry for the event display
  BuildSimpleGeometry();
  
  // Create default mag field
  SetField();

  // Prepare the tracking medium lists
  for(Int_t i=0;i<1000;i++) (*fImedia)[i]=-99;

  // Add particle list to configuration
  AliConfig::Instance()->Add(fPDGDB); 

  // Set transport parameters
  SetTransPar();
}


//_______________________________________________________________________
AliRun::~AliRun()
{
  //
  // Default AliRun destructor
  //
  if (fRunLoader)
   {
    TFolder* evfold = fRunLoader->GetEventFolder();
    TFolder* modfold = dynamic_cast<TFolder*>(evfold->FindObjectAny(AliConfig::GetModulesFolderName()));
    TIter next(fModules);
    AliModule *mod;
    while((mod = (AliModule*)next()))
     { 
       modfold->Remove(mod);
     }
   }
   
  delete fImedia;
  delete fField;
  delete fMC;
  delete fGeometry;
  delete fDisplay;
  delete fGenerator;
  delete fLego;
  
  
  if (fModules) {
    fModules->Delete();
    delete fModules;
  }
  
  delete fHitLists;
  delete fPDGDB;
  delete fMCQA;
}

//_______________________________________________________________________
void AliRun::Copy(AliRun &) const
{
  Fatal("Copy","Not implemented!\n");
}

//_______________________________________________________________________
void AliRun::AddHit(Int_t id, Int_t track, Int_t *vol, Float_t *hits) const
{
  //
  //  Add a hit to detector id
  //
  TObjArray &dets = *fModules;
  if(dets[id]) dynamic_cast<AliModule*>(dets[id])->AddHit(track,vol,hits);
}

//_______________________________________________________________________
void AliRun::AddDigit(Int_t id, Int_t *tracks, Int_t *digits) const
{
  //
  // Add digit to detector id
  //
  TObjArray &dets = *fModules;
  if(dets[id]) dynamic_cast<AliModule*>(dets[id])->AddDigit(tracks,digits);
}

//_______________________________________________________________________
void AliRun::Browse(TBrowser *b)
{
  //
  // Called when the item "Run" is clicked on the left pane
  // of the Root browser.
  // It displays the Root Trees and all detectors.
  //
  //detectors are in folders anyway
  b->Add(fMCQA,"AliMCQA");
}

//_______________________________________________________________________
void AliRun::Build()
{
  //
  // Initialize Alice geometry
  // Dummy routine
  //
}
 
//_______________________________________________________________________
void AliRun::BuildSimpleGeometry()
{
  //
  // Create a simple TNode geometry used by Root display engine
  //
  // Initialise geometry
  //
  fGeometry = new TGeometry("AliceGeom","Galice Geometry for Hits");
  new TMaterial("void","Vacuum",0,0,0);  //Everything is void
  TBRIK *brik = new TBRIK("S_alice","alice volume","void",2000,2000,3000);
  brik->SetVisibility(0);
  new TNode("alice","alice","S_alice");
}

//_______________________________________________________________________
void AliRun::CleanDetectors()
{
  //
  // Clean Detectors at the end of event
  //
  fRunLoader->CleanDetectors();
}

//_______________________________________________________________________
Int_t AliRun::DistancetoPrimitive(Int_t, Int_t)
{
  //
  // Return the distance from the mouse to the AliRun object
  // Dummy routine
  //
  return 9999;
}

//_______________________________________________________________________
void AliRun::DumpPart (Int_t i) const
{
  //
  // Dumps particle i in the stack
  //
    fRunLoader->Stack()->DumpPart(i);
}

//_______________________________________________________________________
void AliRun::DumpPStack () const
{
  //
  // Dumps the particle stack
  //
    fRunLoader->Stack()->DumpPStack();
}

//_______________________________________________________________________
void  AliRun::SetField(AliMagF* magField)
{
    // Set Magnetic Field Map
    fField = magField;
    fField->ReadField();
}

//_______________________________________________________________________
void AliRun::SetField(Int_t type, Int_t version, Float_t scale,
		      Float_t maxField, char* filename)
{
  //
  //  Set magnetic field parameters
  //  type      Magnetic field transport flag 0=no field, 2=helix, 3=Runge Kutta
  //  version   Magnetic field map version (only 1 active now)
  //  scale     Scale factor for the magnetic field
  //  maxField  Maximum value for the magnetic field

  //
  // --- Sanity check on mag field flags
  if(fField) delete fField;
  if(version==1) {
    fField = new AliMagFC("Map1"," ",type,scale,maxField);
  } else if(version<=2) {
    fField = new AliMagFCM("Map2-3",filename,type,scale,maxField);
    fField->ReadField();
  } else if(version==3) {
    fField = new AliMagFDM("Map4",filename,type,scale,maxField);
    fField->ReadField();
  } else {
    Warning("SetField","Invalid map %d\n",version);
  }
}
//_____________________________________________________________________________
 

//_____________________________________________________________________________

void AliRun::InitLoaders()
{
  //creates list of getters
  Info("InitLoaders","");
  TIter next(fModules);
  AliModule *mod;
  while((mod = (AliModule*)next()))
   { 
     AliDetector *det = dynamic_cast<AliDetector*>(mod);
     if (det) 
      {
        Info("InitLoaders"," Adding %s ",det->GetName());
        fRunLoader->AddLoader(det);
      }
   }
  Info("InitLoaders","Done");
}
//_____________________________________________________________________________

void AliRun::FinishRun()
{
  //
  // Called at the end of the run.
  //
  
  
  if(fLego) 
   {
    Info("FinishRun"," Finish Lego");
    fRunLoader->CdGAFile();
    fLego->FinishRun();
   }
  
  // Clean detector information
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
    Info("FinishRun"," %s->FinishRun()",detector->GetName());
    detector->FinishRun();
  }
  
  //Output energy summary tables
  Info("FinishRun"," EnergySummary()");
  EnergySummary();
  
  Info("FinishRun"," fRunLoader->WriteHeader(OVERWRITE)");
  fRunLoader->WriteHeader("OVERWRITE");
  
  // Write AliRun info and all detectors parameters

  fRunLoader->CdGAFile();
  Write(0,TObject::kOverwrite);//write AliRun
  fRunLoader->Write(0,TObject::kOverwrite);//write RunLoader itself
  
  // Clean tree information
  Info("FinishRun"," fRunLoader->Stack()->FinishRun()");
  fRunLoader->Stack()->FinishRun();

  // Clean detector information
  Info("FinishRun"," fGenerator->FinishRun()");
  fGenerator->FinishRun();
}

//_______________________________________________________________________
void AliRun::FlagTrack(Int_t track)
{
  // Delegate to stack
  //
    fRunLoader->Stack()->FlagTrack(track);
}
 
//_______________________________________________________________________
void AliRun::EnergySummary()
{
  //
  // Print summary of deposited energy
  //

  Int_t ndep=0;
  Float_t edtot=0;
  Float_t ed, ed2;
  Int_t kn, i, left, j, id;
  const Float_t kzero=0;
  Int_t ievent=fRunLoader->GetHeader()->GetEvent()+1;
  //
  // Energy loss information
  if(ievent) {
    printf("***************** Energy Loss Information per event (GEV) *****************\n");
    for(kn=1;kn<fEventEnergy.GetSize();kn++) {
      ed=fSummEnergy[kn];
      if(ed>0) {
	fEventEnergy[ndep]=kn;
	if(ievent>1) {
	  ed=ed/ievent;
	  ed2=fSum2Energy[kn];
	  ed2=ed2/ievent;
	  ed2=100*TMath::Sqrt(TMath::Max(ed2-ed*ed,kzero))/ed;
	} else 
	  ed2=99;
	fSummEnergy[ndep]=ed;
	fSum2Energy[ndep]=TMath::Min(static_cast<Float_t>(99.),TMath::Max(ed2,kzero));
	edtot+=ed;
	ndep++;
      }
    }
    for(kn=0;kn<(ndep-1)/3+1;kn++) {
      left=ndep-kn*3;
      for(i=0;i<(3<left?3:left);i++) {
	j=kn*3+i;
        id=Int_t (fEventEnergy[j]+0.1);
	printf(" %s %10.3f +- %10.3f%%;",gMC->VolName(id),fSummEnergy[j],fSum2Energy[j]);
      }
      printf("\n");
    }
    //
    // Relative energy loss in different detectors
    printf("******************** Relative Energy Loss per event ********************\n");
    printf("Total energy loss per event %10.3f GeV\n",edtot);
    for(kn=0;kn<(ndep-1)/5+1;kn++) {
      left=ndep-kn*5;
      for(i=0;i<(5<left?5:left);i++) {
	j=kn*5+i;
        id=Int_t (fEventEnergy[j]+0.1);
	printf(" %s %10.3f%%;",gMC->VolName(id),100*fSummEnergy[j]/edtot);
      }
      printf("\n");
    }
    for(kn=0;kn<75;kn++) printf("*"); 
    printf("\n");
  }
  //
  // Reset the TArray's
  //  fEventEnergy.Set(0);
  //  fSummEnergy.Set(0);
  //  fSum2Energy.Set(0);
}

//_______________________________________________________________________
AliModule *AliRun::GetModule(const char *name) const
{
  //
  // Return pointer to detector from name
  //
  return dynamic_cast<AliModule*>(fModules->FindObject(name));
}
 
//_______________________________________________________________________
AliDetector *AliRun::GetDetector(const char *name) const
{
  //
  // Return pointer to detector from name
  //
  return dynamic_cast<AliDetector*>(fModules->FindObject(name));
}
 
//_______________________________________________________________________
Int_t AliRun::GetModuleID(const char *name) const
{
  //
  // Return galice internal detector identifier from name
  //
  Int_t i=-1;
  TObject *mod=fModules->FindObject(name);
  if(mod) i=fModules->IndexOf(mod);
  return i;
}
 
//_______________________________________________________________________
Int_t AliRun::GetEvent(Int_t event)
{
//
// Reloads data containers in folders # event
// Set branch addresses
//
  if (fRunLoader == 0x0)
   {
     Error("GetEvent","RunLoader is not set. Can not load data.");
     return -1;
   }
/*****************************************/ 
/****   P R E    R E L O A D I N G    ****/
/*****************************************/ 
// Reset existing structures
  ResetHits();
  ResetTrackReferences();
  ResetDigits();
  ResetSDigits();

/*****************************************/ 
/****       R  E  L  O  A  D          ****/
/*****************************************/

  fRunLoader->GetEvent(event);

/*****************************************/ 
/****  P O S T    R E L O A D I N G   ****/
/*****************************************/ 

  // Set Trees branch addresses
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) 
   {
     detector->SetTreeAddress();
   }
 
  return fRunLoader->GetHeader()->GetNtrack();
}

//_______________________________________________________________________
TGeometry *AliRun::GetGeometry()
{
  //
  // Import Alice geometry from current file
  // Return pointer to geometry object
  //
  if (!fGeometry) fGeometry = dynamic_cast<TGeometry*>(gDirectory->Get("AliceGeom"));
  //
  // Unlink and relink nodes in detectors
  // This is bad and there must be a better way...
  //
  
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
    TList *dnodes=detector->Nodes();
    Int_t j;
    TNode *node, *node1;
    for ( j=0; j<dnodes->GetSize(); j++) {
      node = dynamic_cast<TNode*>(dnodes->At(j));
      node1 = fGeometry->GetNode(node->GetName());
      dnodes->Remove(node);
      dnodes->AddAt(node1,j);
    }
  }
  return fGeometry;
}

//_______________________________________________________________________
Int_t AliRun::GetPrimary(Int_t track) const
{
  //
  // return number of primary that has generated track
  //
    return fRunLoader->Stack()->GetPrimary(track);
}
 
//_______________________________________________________________________
void AliRun::MediaTable()
{
  //
  // Built media table to get from the media number to
  // the detector id
  //

  Int_t kz, nz, idt, lz, i, k, ind;
  //  Int_t ibeg;
  TObjArray &dets = *gAlice->Detectors();
  AliModule *det;
  //
  // For all detectors
  for (kz=0;kz<fNdets;kz++) {
    // If detector is defined
    if((det=dynamic_cast<AliModule*>(dets[kz]))) {
        TArrayI &idtmed = *(det->GetIdtmed()); 
        for(nz=0;nz<100;nz++) {
	// Find max and min material number
	if((idt=idtmed[nz])) {
	  det->LoMedium() = det->LoMedium() < idt ? det->LoMedium() : idt;
	  det->HiMedium() = det->HiMedium() > idt ? det->HiMedium() : idt;
	}
      }
      if(det->LoMedium() > det->HiMedium()) {
	det->LoMedium() = 0;
	det->HiMedium() = 0;
      } else {
	if(det->HiMedium() > fImedia->GetSize()) {
	  Error("MediaTable","Increase fImedia from %d to %d",
		fImedia->GetSize(),det->HiMedium());
	  return;
	}
	// Tag all materials in rage as belonging to detector kz
	for(lz=det->LoMedium(); lz<= det->HiMedium(); lz++) {
	  (*fImedia)[lz]=kz;
	}
      }
    }
  }
  //
  // Print summary table
  printf(" Traking media ranges:\n");
  for(i=0;i<(fNdets-1)/6+1;i++) {
    for(k=0;k< (6<fNdets-i*6?6:fNdets-i*6);k++) {
      ind=i*6+k;
      det=dynamic_cast<AliModule*>(dets[ind]);
      if(det)
	printf(" %6s: %3d -> %3d;",det->GetName(),det->LoMedium(),
	       det->HiMedium());
      else
	printf(" %6s: %3d -> %3d;","NULL",0,0);
    }
    printf("\n");
  }
}

//_______________________________________________________________________
void AliRun::SetGenerator(AliGenerator *generator)
{
  //
  // Load the event generator
  //
  if(!fGenerator) fGenerator = generator;
}

//_______________________________________________________________________
void AliRun::ResetGenerator(AliGenerator *generator)
{
  //
  // Load the event generator
  //
  if(fGenerator)
    if(generator)
      Warning("ResetGenerator","Replacing generator %s with %s\n",
	      fGenerator->GetName(),generator->GetName());
    else
      Warning("ResetGenerator","Replacing generator %s with NULL\n",
	      fGenerator->GetName());
  fGenerator = generator;
}

//_______________________________________________________________________
void AliRun::SetTransPar(const char *filename)
{
  fTransParName = filename;
}

//_______________________________________________________________________
void AliRun::SetBaseFile(const char *filename)
{
  fBaseFileName = filename;
}

//_______________________________________________________________________
void AliRun::ReadTransPar()
{
  //
  // Read filename to set the transport parameters
  //


  const Int_t kncuts=10;
  const Int_t knflags=11;
  const Int_t knpars=kncuts+knflags;
  const char kpars[knpars][7] = {"CUTGAM" ,"CUTELE","CUTNEU","CUTHAD","CUTMUO",
			       "BCUTE","BCUTM","DCUTE","DCUTM","PPCUTM","ANNI",
			       "BREM","COMP","DCAY","DRAY","HADR","LOSS",
			       "MULS","PAIR","PHOT","RAYL"};
  char line[256];
  char detName[7];
  char* filtmp;
  Float_t cut[kncuts];
  Int_t flag[knflags];
  Int_t i, itmed, iret, ktmed, kz;
  FILE *lun;
  //
  // See whether the file is there
  filtmp=gSystem->ExpandPathName(fTransParName.Data());
  lun=fopen(filtmp,"r");
  delete [] filtmp;
  if(!lun) {
    Warning("ReadTransPar","File %s does not exist!\n",fTransParName.Data());
    return;
  }
  //
  if(fDebug) {
    printf(" "); for(i=0;i<60;i++) printf("*"); printf("\n");
    printf(" *%59s\n","*");
    printf(" *       Please check carefully what you are doing!%10s\n","*");
    printf(" *%59s\n","*");
  }
  //
  while(1) {
    // Initialise cuts and flags
    for(i=0;i<kncuts;i++) cut[i]=-99;
    for(i=0;i<knflags;i++) flag[i]=-99;
    itmed=0;
    for(i=0;i<256;i++) line[i]='\0';
    // Read up to the end of line excluded
    iret=fscanf(lun,"%[^\n]",line);
    if(iret<0) {
      //End of file
      fclose(lun);
      if(fDebug){
	printf(" *%59s\n","*");
	printf(" "); for(i=0;i<60;i++) printf("*"); printf("\n");
      }
      return;
    }
    // Read the end of line
    fscanf(lun,"%*c");
    if(!iret) continue;
    if(line[0]=='*') continue;
    // Read the numbers
    iret=sscanf(line,"%s %d %f %f %f %f %f %f %f %f %f %f %d %d %d %d %d %d %d %d %d %d %d",
		detName,&itmed,&cut[0],&cut[1],&cut[2],&cut[3],&cut[4],&cut[5],&cut[6],&cut[7],&cut[8],
		&cut[9],&flag[0],&flag[1],&flag[2],&flag[3],&flag[4],&flag[5],&flag[6],&flag[7],
		&flag[8],&flag[9],&flag[10]);
    if(!iret) continue;
    if(iret<0) {
      //reading error
      Warning("ReadTransPar","Error reading file %s\n",fTransParName.Data());
      continue;
    }
    // Check that the module exist
    AliModule *mod = GetModule(detName);
    if(mod) {
      // Get the array of media numbers
      TArrayI &idtmed = *mod->GetIdtmed();
      // Check that the tracking medium code is valid
      if(0<=itmed && itmed < 100) {
	ktmed=idtmed[itmed];
	if(!ktmed) {
	  Warning("ReadTransPar","Invalid tracking medium code %d for %s\n",itmed,mod->GetName());
	  continue;
	}
	// Set energy thresholds
	for(kz=0;kz<kncuts;kz++) {
	  if(cut[kz]>=0) {
	    if(fDebug) printf(" *  %-6s set to %10.3E for tracking medium code %4d for %s\n",
		   kpars[kz],cut[kz],itmed,mod->GetName());
	    gMC->Gstpar(ktmed,kpars[kz],cut[kz]);
	  }
	}
	// Set transport mechanisms
	for(kz=0;kz<knflags;kz++) {
	  if(flag[kz]>=0) {
	    if(fDebug) printf(" *  %-6s set to %10d for tracking medium code %4d for %s\n",
		   kpars[kncuts+kz],flag[kz],itmed,mod->GetName());
	    gMC->Gstpar(ktmed,kpars[kncuts+kz],Float_t(flag[kz]));
	  }
	}
      } else {
	Warning("ReadTransPar","Invalid medium code %d *\n",itmed);
	continue;
      }
    } else {
      if(fDebug) printf("%s::ReadTransParModule: %s not present\n",ClassName(),detName);
      continue;
    }
  }
}
//_____________________________________________________________________________

void AliRun::BeginEvent()
{
  //
  // Clean-up previous event
  // Energy scores  
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Info("BeginEvent","          BEGINNING EVENT               ");
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Info("BeginEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    
  /*******************************/    
  /*   Clean after eventual      */
  /*   previous event            */
  /*******************************/    

  
  //Set the next event in Run Loader -> Cleans trees (TreeK and all trees in detectors),
  fRunLoader->SetEventNumber(++fEventNrInRun);// sets new files, cleans the previous event stuff, if necessary, etc.,  
  Info("BeginEvent","EventNr is %d",fEventNrInRun);
     
  fEventEnergy.Reset();  
    // Clean detector information
  
  if (fRunLoader->Stack())
    fRunLoader->Stack()->Reset();//clean stack -> tree is unloaded
  else
    fRunLoader->MakeStack();//or make a new one
    
  Info("BeginEvent","  fRunLoader->MakeTree(K)");
  fRunLoader->MakeTree("K");
  Info("BeginEvent","  gMC->SetStack(fRunLoader->Stack())");
  gMC->SetStack(fRunLoader->Stack());//Was in InitMC - but was moved here 
                                     //because we don't have guarantee that 
                                     //stack pointer is not going to change from event to event
	                 //since it bellobgs to header and is obtained via RunLoader
  //
  //  Reset all Detectors & kinematics & make/reset trees
  //
    
  fRunLoader->GetHeader()->Reset(fRun,fEvent,fEventNrInRun);
  fRunLoader->WriteKinematics("OVERWRITE");

  Info("BeginEvent","  fRunLoader->MakeTrackRefsContainer()");
  fRunLoader->MakeTrackRefsContainer();//for insurance

  Info("BeginEvent","  ResetHits()");
  ResetHits();
  Info("BeginEvent","  fRunLoader->MakeTree(H)");
  fRunLoader->MakeTree("H");

  //
  if(fLego) 
   {
    fLego->BeginEvent();
    return;
   }

  //create new branches and SetAdresses
  TIter next(fModules);
  AliModule *detector;
  while((detector = (AliModule*)next()))
   {
    Info("BeginEvent","  %s->MakeBranch(H)",detector->GetName());
    detector->MakeBranch("H"); 
    Info("BeginEvent","  %s->MakeBranchTR()",detector->GetName());
    detector->MakeBranchTR();
    Info("BeginEvent","  %s->SetTreeAddress()",detector->GetName());
    detector->SetTreeAddress();
   }
}

//_______________________________________________________________________
TParticle* AliRun::Particle(Int_t i)
{
  if (fRunLoader)
   if (fRunLoader->Stack())
    return fRunLoader->Stack()->Particle(i);
  return 0x0;   
}

//_______________________________________________________________________
void AliRun::ResetDigits()
{
  //
  //  Reset all Detectors digits
  //
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
     detector->ResetDigits();
  }
}

//_______________________________________________________________________
void AliRun::ResetSDigits()
{
  //
  //  Reset all Detectors digits
  //
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
     detector->ResetSDigits();
  }
}

//_______________________________________________________________________
void AliRun::ResetHits()
{
  //
  //  Reset all Detectors hits
  //
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
     detector->ResetHits();
  }
}
//_______________________________________________________________________

void AliRun::ResetTrackReferences()
{
  //
  //  Reset all Detectors hits
  //
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
     detector->ResetTrackReferences();
  }
}
//_______________________________________________________________________

void AliRun::ResetPoints()
{
  //
  // Reset all Detectors points
  //
  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
     detector->ResetPoints();
  }
}
//_______________________________________________________________________

void AliRun::InitMC(const char *setup)
{
  //
  // Initialize the Alice setup
  //

  if(fInitDone) {
    Warning("Init","Cannot initialise AliRun twice!\n");
    return;
  }
    
  gROOT->LoadMacro(setup);
  gInterpreter->ProcessLine(fConfigFunction.Data());

  // Register MC in configuration 
  AliConfig::Instance()->Add(gMC);

  InitLoaders();

  fRunLoader->MakeTree("E");
  fRunLoader->LoadKinematics("RECREATE");
  fRunLoader->LoadTrackRefs("RECREATE");
  fRunLoader->LoadHits("all","RECREATE");
  
  
  fRunLoader->CdGAFile();

  gMC->DefineParticles();  //Create standard MC particles
  AliPDG::AddParticlesToPdgDataBase();  

  TObject *objfirst, *objlast;

  fNdets = fModules->GetLast()+1;

  //
  //=================Create Materials and geometry
  gMC->Init();

  // Added also after in case of interactive initialisation of modules
  fNdets = fModules->GetLast()+1;

  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) 
   {
     objlast = gDirectory->GetList()->Last();
      
     // Add Detector histograms in Detector list of histograms
     if (objlast) objfirst = gDirectory->GetList()->After(objlast);
     else         objfirst = gDirectory->GetList()->First();
     while (objfirst) 
      {
        detector->Histograms()->Add(objfirst);
        objfirst = gDirectory->GetList()->After(objfirst);
      }
   }
   ReadTransPar(); //Read the cuts for all materials
   
   MediaTable(); //Build the special IMEDIA table
   
   //Initialise geometry deposition table
   fEventEnergy.Set(gMC->NofVolumes()+1);
   fSummEnergy.Set(gMC->NofVolumes()+1);
   fSum2Energy.Set(gMC->NofVolumes()+1);
   
   //Compute cross-sections
   gMC->BuildPhysics();
   
   //Write Geometry object to current file.
   fRunLoader->WriteGeometry();
   
   fInitDone = kTRUE;

   fMCQA = new AliMCQA(fNdets);

   //
   // Save stuff at the beginning of the file to avoid file corruption
   Write();
   fEventNrInRun = -1; //important - we start Begin event from increasing current number in run
}

//_______________________________________________________________________
void AliRun::RunMC(Int_t nevent, const char *setup)
{
  //
  // Main function to be called to process a galice run
  // example
  //    Root > gAlice.Run(); 
  // a positive number of events will cause the finish routine
  // to be called
  //

  fEventsPerRun = nevent;
  // check if initialisation has been done
  if (!fInitDone) InitMC(setup);
  
  // Create the Root Tree with one branch per detector
  //Hits moved to begin event -> now we are crating separate tree for each event

  gMC->ProcessRun(nevent);

  // End of this run, close files
  if(nevent>0) FinishRun();
}

//_______________________________________________________________________
void AliRun::RunReco(const char *selected, Int_t first, Int_t last)
{
  //
  // Main function to be called to reconstruct Alice event
  // 
   Int_t nev = fRunLoader->GetNumberOfEvents();
   Info("RunReco","Found %d events",nev);
   Int_t nFirst = first;
   Int_t nLast  = (last < 0)? nev : last;
   
   for (Int_t nevent = nFirst; nevent <= nLast; nevent++) {
     Info("RunReco","Processing event %d",nevent);
     GetEvent(nevent);
     Digits2Reco(selected);
   }
}

//_______________________________________________________________________

void AliRun::Hits2Digits(const char *selected)
{

   // Convert Hits to sumable digits
   // 
   for (Int_t nevent=0; nevent<gAlice->TreeE()->GetEntries(); nevent++) {
     GetEvent(nevent);
     Hits2SDigits(selected);
     SDigits2Digits(selected);
   }  
}


//_______________________________________________________________________

void AliRun::Tree2Tree(Option_t *option, const char *selected)
{
  //
  // Function to transform the content of
  //  
  // - TreeH to TreeS (option "S")
  // - TreeS to TreeD (option "D")
  // - TreeD to TreeR (option "R")
  // 
  // If multiple options are specified ("SDR"), transformation will be done in sequence for
  // selected detector and for all detectors if none is selected (detector string 
  // can contain blank separated list of detector names). 


   const char *oS = strstr(option,"S");
   const char *oD = strstr(option,"D");
   const char *oR = strstr(option,"R");
   
   TObjArray *detectors = Detectors();

   TIter next(detectors);

   AliDetector *detector = 0;

   
   while((detector = dynamic_cast<AliDetector*>(next()))) {
     if (selected) 
       if (strcmp(detector->GetName(),selected)) continue;
     if (detector->IsActive())
      { 
       
       AliLoader* loader = detector->GetLoader();
       if (loader == 0x0) continue;
       
       if (oS) 
        {
          Info("Tree2Tree","Processing Hits2SDigits for %s ...",detector->GetName());
          loader->LoadHits("read");
          if (loader->TreeS() == 0x0) loader->MakeTree("S");
          detector->MakeBranch(option);
          detector->SetTreeAddress();
          detector->Hits2SDigits();
          loader->UnloadHits();
          loader->UnloadSDigits();
        }  
       if (oD) 
        {
          Info("Tree2Tree","Processing SDigits2Digits for %s ...",detector->GetName());
          loader->LoadSDigits("read");
          if (loader->TreeD() == 0x0) loader->MakeTree("D");
          detector->MakeBranch(option);
          detector->SetTreeAddress();
          detector->SDigits2Digits();
          loader->UnloadSDigits();
          loader->UnloadDigits();
        } 
       if (oR) 
        {
          Info("Tree2Tree","Processing Digits2Reco for %s ...",detector->GetName());
          loader->LoadDigits("read");
          if (loader->TreeR() == 0x0) loader->MakeTree("R");
          detector->MakeBranch(option);
          detector->SetTreeAddress();
          detector->Digits2Reco(); 
          loader->UnloadDigits();
          loader->UnloadRecPoints();

        }
     }   
   }
}


//_______________________________________________________________________
void AliRun::RunLego(const char *setup, Int_t nc1, Float_t c1min,
		     Float_t c1max,Int_t nc2,Float_t c2min,Float_t c2max,
		     Float_t rmin,Float_t rmax,Float_t zmax, AliLegoGenerator* gener)
{
  //
  // Generates lego plots of:
  //    - radiation length map phi vs theta
  //    - radiation length map phi vs eta
  //    - interaction length map
  //    - g/cm2 length map
  //
  //  ntheta    bins in theta, eta
  //  themin    minimum angle in theta (degrees)
  //  themax    maximum angle in theta (degrees)
  //  nphi      bins in phi
  //  phimin    minimum angle in phi (degrees)
  //  phimax    maximum angle in phi (degrees)
  //  rmin      minimum radius
  //  rmax      maximum radius
  //  
  //
  //  The number of events generated = ntheta*nphi
  //  run input parameters in macro setup (default="Config.C")
  //
  //  Use macro "lego.C" to visualize the 3 lego plots in spherical coordinates
  //Begin_Html
  /*
    <img src="picts/AliRunLego1.gif">
  */
  //End_Html
  //Begin_Html
  /*
    <img src="picts/AliRunLego2.gif">
  */
  //End_Html
  //Begin_Html
  /*
    <img src="picts/AliRunLego3.gif">
  */
  //End_Html
  //

  // check if initialisation has been done
  if (!fInitDone) InitMC(setup);
  //Save current generator
  AliGenerator *gen=Generator();

  // Set new generator
  if (!gener) gener  = new AliLegoGenerator();
  ResetGenerator(gener);
  //
  // Configure Generator
  gener->SetRadiusRange(rmin, rmax);
  gener->SetZMax(zmax);
  gener->SetCoor1Range(nc1, c1min, c1max);
  gener->SetCoor2Range(nc2, c2min, c2max);
  
  
  //Create Lego object  
  fLego = new AliLego("lego",gener);

  //Prepare MC for Lego Run
  gMC->InitLego();
  
  //Run Lego Object

  //gMC->ProcessRun(nc1*nc2+1);
  gMC->ProcessRun(nc1*nc2);
  
  // Create only the Root event Tree
  fRunLoader->MakeTree("E");
  
  // End of this run, close files
  FinishRun();
  // Restore current generator
  ResetGenerator(gen);
  // Delete Lego Object
  delete fLego; fLego=0;
}

//_______________________________________________________________________
void AliRun::SetConfigFunction(const char * config) 
{
  //
  // Set the signature of the function contained in Config.C to configure
  // the run
  //
  fConfigFunction=config;
}

//_______________________________________________________________________
void AliRun::SetCurrentTrack(Int_t track)
{ 
  //
  // Set current track number
  //
    fRunLoader->Stack()->SetCurrentTrack(track); 
}
 
//_______________________________________________________________________
void AliRun::SetTrack(Int_t done, Int_t parent, Int_t pdg, Float_t *pmom,
                      Float_t *vpos, Float_t *polar, Float_t tof,
                      AliMCProcess mech, Int_t &ntr, Float_t weight, Int_t is)
{ 
// Delegate to stack
//
    fRunLoader->Stack()->SetTrack(done, parent, pdg, pmom, vpos, polar, tof,
		     mech, ntr, weight, is);
}

//_______________________________________________________________________
void AliRun::SetTrack(Int_t done, Int_t parent, Int_t pdg,
  	  Double_t px, Double_t py, Double_t pz, Double_t e,
                      Double_t vx, Double_t vy, Double_t vz, Double_t tof,
                      Double_t polx, Double_t poly, Double_t polz,
                      AliMCProcess mech, Int_t &ntr, Float_t weight, Int_t is)
{ 
  // Delegate to stack
  //
  fRunLoader->Stack()->SetTrack(done, parent, pdg, px, py, pz, e, vx, vy, vz, tof,
                                polx, poly, polz, mech, ntr, weight, is);
}

//_______________________________________________________________________
void AliRun::SetHighWaterMark(const Int_t nt)
{
    //
    // Set high water mark for last track in event
    fRunLoader->Stack()->SetHighWaterMark(nt);
}

//_______________________________________________________________________
void AliRun::KeepTrack(const Int_t track)
{ 
  //
  // Delegate to stack
  //
    fRunLoader->Stack()->KeepTrack(track);
}
 
// 
// MC Application
// 

//_______________________________________________________________________
void  AliRun::ConstructGeometry() 
{
  //
  // Create modules, materials, geometry
  //

    TStopwatch stw;
    TIter next(fModules);
    AliModule *detector;
    Info("ConstructGeometry","Geometry creation:");
    while((detector = dynamic_cast<AliModule*>(next()))) {
      stw.Start();
      // Initialise detector materials and geometry
      detector->CreateMaterials();
      detector->CreateGeometry();
      printf("%10s R:%.2fs C:%.2fs\n",
             detector->GetName(),stw.RealTime(),stw.CpuTime());
    }
}

//_______________________________________________________________________
void  AliRun::InitGeometry()
{ 
  //
  // Initialize detectors and display geometry
  //

   printf("Initialisation:\n");
    TStopwatch stw;
    TIter next(fModules);
    AliModule *detector;
    while((detector = dynamic_cast<AliModule*>(next()))) {
      stw.Start();
      // Initialise detector and display geometry
      detector->Init();
      detector->BuildGeometry();
      printf("%10s R:%.2fs C:%.2fs\n",
	     detector->GetName(),stw.RealTime(),stw.CpuTime());
    }
 
}
//_______________________________________________________________________

void  AliRun::GeneratePrimaries()
{ 
  //
  // Generate primary particles and fill them in the stack.
  //

  Generator()->Generate();
}
//_______________________________________________________________________

void AliRun::BeginPrimary()
{
  //
  // Called  at the beginning of each primary track
  //
  
  // Reset Hits info
  gAlice->ResetHits();
  gAlice->ResetTrackReferences();

}

//_______________________________________________________________________
void AliRun::PreTrack()
{
     TObjArray &dets = *fModules;
     AliModule *module;

     for(Int_t i=0; i<=fNdets; i++)
       if((module = dynamic_cast<AliModule*>(dets[i])))
	 module->PreTrack();

     fMCQA->PreTrack();
}

//_______________________________________________________________________
void AliRun::Stepping() 
{
  //
  // Called at every step during transport
  //

  Int_t id = DetFromMate(gMC->GetMedium());
  if (id < 0) return;

  //
  // --- If lego option, do it and leave 
  if (fLego)
    fLego->StepManager();
  else {
    Int_t copy;
    //Update energy deposition tables
    AddEnergyDeposit(gMC->CurrentVolID(copy),gMC->Edep());
  
    //Call the appropriate stepping routine;
    AliModule *det = dynamic_cast<AliModule*>(fModules->At(id));
    if(det && det->StepManagerIsEnabled()) {
      fMCQA->StepManager(id);
      det->StepManager();
    }
  }
}

//_______________________________________________________________________
void AliRun::PostTrack()
{
     TObjArray &dets = *fModules;
     AliModule *module;

     for(Int_t i=0; i<=fNdets; i++)
       if((module = dynamic_cast<AliModule*>(dets[i])))
	 module->PostTrack();
}

//_______________________________________________________________________
void AliRun::FinishPrimary()
{
  //
  // Called  at the end of each primary track
  //
  
  //  static Int_t count=0;
  //  const Int_t times=10;
  // This primary is finished, purify stack
  fRunLoader->Stack()->PurifyKine();

  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
    detector->FinishPrimary();
    if(detector->GetLoader())
     {
       detector->GetLoader()->TreeH()->Fill();
     }
  }

  // Write out track references if any
  if (fRunLoader->TreeTR()) 
   {
    fRunLoader->TreeTR()->Fill();
   }
}

//_______________________________________________________________________
void AliRun::FinishEvent()
{
  //
  // Called at the end of the event.
  //
  
  //
  if(fLego) fLego->FinishEvent();

  TIter next(fModules);
  AliModule *detector;
  while((detector = dynamic_cast<AliModule*>(next()))) {
    detector->FinishEvent();
  }

  //Update the energy deposit tables
  Int_t i;
  for(i=0;i<fEventEnergy.GetSize();i++) 
   {
    fSummEnergy[i]+=fEventEnergy[i];
    fSum2Energy[i]+=fEventEnergy[i]*fEventEnergy[i];
   }

  AliHeader* header = fRunLoader->GetHeader();
  AliStack* stack = fRunLoader->Stack();
  if ( (header == 0x0) || (stack == 0x0) )
   {//check if we got header and stack. If not cry and exit aliroot
    Fatal("AliRun","Can not get the stack or header from LOADER");
    return;//never reached
   }  
  // Update Header information 
  header->SetNprimary(stack->GetNprimary());
  header->SetNtrack(stack->GetNtrack());  

  
  // Write out the kinematics
  stack->FinishEvent();
   
  // Write out the event Header information
  TTree* treeE = fRunLoader->TreeE();
  if (treeE) 
   {
      header->SetStack(stack);
      treeE->Fill();
   }
  else
   {
    Error("FinishEvent","Can not get TreeE from RL");
   }
  
  fRunLoader->WriteKinematics("OVERWRITE");
  fRunLoader->WriteTrackRefs("OVERWRITE");
  fRunLoader->WriteHits("OVERWRITE");

  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Info("FinishEvent","          FINISHING EVENT               ");
  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Info("FinishEvent","<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  
}

//_______________________________________________________________________
void AliRun::Field(const Double_t* x, Double_t *b) const
{
   Float_t xfloat[3];
   for (Int_t i=0; i<3; i++) xfloat[i] = x[i]; 

   if (Field()) {
         Float_t bfloat[3];
         Field()->Field(xfloat,bfloat);
         for (Int_t j=0; j<3; j++) b[j] = bfloat[j]; 
   } 
   else {
         printf("No mag field defined!\n");
         b[0]=b[1]=b[2]=0.;
   }

}      

// 
// End of MC Application
// 

//_______________________________________________________________________
void AliRun::Streamer(TBuffer &R__b)
{
  // Stream an object of class AliRun.

  if (R__b.IsReading()) {
    if (!gAlice) gAlice = this;
    AliRun::Class()->ReadBuffer(R__b, this);
    gROOT->GetListOfBrowsables()->Add(this,"Run");

    gRandom = fRandom;
  } else {
    AliRun::Class()->WriteBuffer(R__b, this);
  }
}


//_______________________________________________________________________
Int_t AliRun::CurrentTrack() const {
  //
  // Returns current track
  //
  return fRunLoader->Stack()->CurrentTrack();
}

//_______________________________________________________________________
Int_t AliRun::GetNtrack() const {
  //
  // Returns number of tracks in stack
  //
  return fRunLoader->Stack()->GetNtrack();
}
//_______________________________________________________________________

TObjArray* AliRun::Particles() {
  //
  // Returns pointer to Particles array
  //
  if (fRunLoader)
   if (fRunLoader->Stack())
    return fRunLoader->Stack()->Particles();
  return 0x0;
}

//___________________________________________________________________________

void AliRun::SetGenEventHeader(AliGenEventHeader* header)
{
  fRunLoader->GetHeader()->SetGenEventHeader(header);
}

//___________________________________________________________________________

Int_t AliRun::GetEvNumber() const
{ 
//Returns number of current event  
  if (fRunLoader == 0x0)
   {
     Error("GetEvent","RunLoader is not set. Can not load data.");
     return -1;
   }

  return fRunLoader->GetEventNumber();
}

void AliRun::SetRunLoader(AliRunLoader* rloader)
{
  fRunLoader = rloader;
  if (fRunLoader == 0x0) return;
  
  TString evfoldname;
  TFolder* evfold = fRunLoader->GetEventFolder();
  if (evfold) evfoldname = evfold->GetName();
  else Warning("SetRunLoader","Did not get Event Folder from Run Loader");
  
  
  TIter next(fModules);
  AliModule *module;
  while((module = (AliModule*)next())) 
   {
     if (evfold) AliConfig::Instance()->Add(module,evfoldname);
     AliDetector* detector = dynamic_cast<AliDetector*>(module);
     if (detector)
      {
        AliLoader* loader = fRunLoader->GetLoader(detector);
        if (loader == 0x0)
         {
           Error("SetRunLoader","Can not get loader for detector %s",detector->GetName());
         }
        else
         {
           Info("SetRunLoader","Setting loader for detector %s",detector->GetName());
           detector->SetLoader(loader);
         }
      }
   }
}

void AliRun::AddModule(AliModule* mod)
{
  if (mod == 0x0) return;
  if (strlen(mod->GetName()) == 0) return;
  if (GetModuleID(mod->GetName()) >= 0) return;
  
  Info("AddModule","%s",mod->GetName());
  if (fRunLoader == 0x0) AliConfig::Instance()->Add(mod);
  else AliConfig::Instance()->Add(mod,fRunLoader->GetEventFolder()->GetName());

  Modules()->Add(mod);
}

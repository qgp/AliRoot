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
Revision 1.58  2002/10/21 09:01:33  alibrary
Getting rid of unused variable

Revision 1.57  2002/10/14 14:57:29  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.56.6.2  2002/07/24 10:07:20  alibrary
Updating VirtualMC

Revision 1.56.6.1  2002/06/10 15:10:14  hristov
Merged with v3-08-02

Revision 1.56  2001/11/22 11:26:28  jchudoba
Proper deletion of arrays, deletion of unused variables (thanks to Rene Brun)

Revision 1.55  2001/09/07 08:38:30  hristov
Pointers initialised to 0 in the default constructors

Revision 1.54  2001/08/30 09:52:12  hristov
The operator[] is replaced by At() or AddAt() in case of TObjArray.

Revision 1.53  2001/07/20 10:03:13  morsch
Changes needed to work with Root 3.01 (substitute lhs [] operator). (Jiri Chudoba)

Revision 1.52  2001/06/14 13:49:22  hristov
Write a TreeD in SDigits2Digits method (needed to be compatible with alirun script)

Revision 1.51  2001/05/31 10:19:52  morsch
Fix for new AliRun::RunReco().

Revision 1.50  2001/05/16 14:57:17  alibrary
New files for folders and Stack

Revision 1.49  2001/03/12 17:45:48  hristov
Changes needed on Sun with CC 5.0

Revision 1.48  2001/03/06 00:01:36  morsch
Add  Digits2Reco() and FindClusters()
Adapt call of cluster finder to new STEER.

Revision 1.47  2001/03/05 08:38:36  morsch
Digitization related methods moved to AliMUONMerger.

Revision 1.46  2001/01/26 21:34:59  morsch
Use access functions for AliMUONHit, AliMUONDigit and AliMUONPadHit data members.

Revision 1.45  2001/01/26 20:00:49  hristov
Major upgrade of AliRoot code

Revision 1.44  2001/01/25 17:39:09  morsch
Pass size of fNdch and fNrawch to CINT.

Revision 1.43  2001/01/23 18:58:19  hristov
Initialisation of some pointers

Revision 1.42  2001/01/17 20:53:40  hristov
Destructors corrected to avoid memory leaks

Revision 1.41  2000/12/21 22:12:40  morsch
Clean-up of coding rule violations,

Revision 1.40  2000/11/29 20:32:26  gosset
Digitize:
1. correction for array index out of bounds
2. one printout commented

Revision 1.39  2000/11/12 17:17:03  pcrochet
BuildGeometry of AliMUON for trigger chambers delegated to AliMUONSegmentationTriggerX (same strategy as for tracking chambers)

Revision 1.38  2000/11/06 09:20:43  morsch
AliMUON delegates part of BuildGeometry() to AliMUONSegmentation using the
Draw() method. This avoids code and parameter replication.

Revision 1.37  2000/10/26 09:53:37  pcrochet
put back trigger chambers in the display (there was a problem in buildgeometry)

Revision 1.36  2000/10/25 19:51:18  morsch
Correct x-position of chambers.

Revision 1.35  2000/10/24 19:46:21  morsch
BuildGeometry updated for slats in station 3-4.

Revision 1.34  2000/10/18 11:42:06  morsch
- AliMUONRawCluster contains z-position.
- Some clean-up of useless print statements during initialisations.

Revision 1.33  2000/10/09 14:01:57  morsch
Unused variables removed.

Revision 1.32  2000/10/06 09:08:10  morsch
Built geometry includes slat geometry for event display.

Revision 1.31  2000/10/02 21:28:08  fca
Removal of useless dependecies via forward declarations

Revision 1.30  2000/10/02 16:58:29  egangler
Cleaning of the code :
-> coding conventions
-> void Streamers
-> some useless includes removed or replaced by "class" statement

Revision 1.29  2000/07/28 13:49:38  morsch
SetAcceptance defines inner and outer chamber radii according to angular acceptance.
Can be used for simple acceptance studies.

Revision 1.28  2000/07/22 16:43:15  morsch
Same comment as before, but now done correctly I hope (sorry it's Saturday evening)

Revision 1.27  2000/07/22 16:36:50  morsch
Change order of indices in creation (new) of xhit and yhit

Revision 1.26  2000/07/03 11:54:57  morsch
AliMUONSegmentation and AliMUONHitMap have been replaced by AliSegmentation and AliHitMap in STEER
The methods GetPadIxy and GetPadXxy of AliMUONSegmentation have changed name to GetPadI and GetPadC.

Revision 1.25  2000/06/29 12:34:09  morsch
AliMUONSegmentation class has been made independent of AliMUONChamber. This makes
it usable with any other geometry class. The link to the object to which it belongs is
established via an index. This assumes that there exists a global geometry manager
from which the pointer to the parent object can be obtained (in our case gAlice).

Revision 1.24  2000/06/28 15:16:35  morsch
(1) Client code adapted to new method signatures in AliMUONSegmentation (see comments there)
to allow development of slat-muon chamber simulation and reconstruction code in the MUON
framework. The changes should have no side effects (mostly dummy arguments).
(2) Hit disintegration uses 3-dim hit coordinates to allow simulation
of chambers with overlapping modules (MakePadHits, Disintegration).

Revision 1.23  2000/06/28 12:19:17  morsch
More consequent seperation of global input data services (AliMUONClusterInput singleton) and the
cluster and hit reconstruction algorithms in AliMUONClusterFindRawinderVS.
AliMUONClusterFinderVS becomes the base class for clustering and hit reconstruction.
It requires two cathode planes. Small modifications in the code will make it usable for
one cathode plane and, hence, more general (for test beam data).
AliMUONClusterFinder is now obsolete.

Revision 1.22  2000/06/28 08:06:10  morsch
Avoid global variables in AliMUONClusterFinderVS by seperating the input data for the fit from the
algorithmic part of the class. Input data resides inside the AliMUONClusterInput singleton.
It also naturally takes care of the TMinuit instance.

Revision 1.21  2000/06/27 08:54:41  morsch
Problems with on constant array sizes (in hitMap, nmuon, xhit, yhit) corrected.

Revision 1.20  2000/06/26 14:02:38  morsch
Add class AliMUONConstants with MUON specific constants using static memeber data and access methods.

Revision 1.19  2000/06/22 13:40:51  morsch
scope problem on HP, "i" declared once
pow changed to TMath::Power (PH, AM)

Revision 1.18  2000/06/15 07:58:48  morsch
Code from MUON-dev joined

Revision 1.14.4.17  2000/06/14 14:36:46  morsch
- add TriggerCircuit (PC)
- add GlobalTrigger and LocalTrigger and specific methods (PC)

Revision 1.14.4.16  2000/06/09 21:20:28  morsch
Most coding rule violations corrected

Revision 1.14.4.15  2000/05/02 09:54:32  morsch
RULE RN17 violations corrected

Revision 1.14.4.12  2000/04/26 12:25:02  morsch
Code revised by P. Crochet:
- Z position of TriggerChamber changed according to A.Tournaire Priv.Comm.
- ToF included in the method MakePadHits
- inner radius of flange between beam shielding and trigger corrected
- Trigger global volume updated (according to the new geometry)

Revision 1.14.4.11  2000/04/19 19:42:08  morsch
Some changes of variable names curing viols and methods concerning
correlated clusters removed.

Revision 1.14.4.10  2000/03/22 16:44:07  gosset
Memory leak suppressed in function Digitise:
p_adr->Delete() instead of Clear (I.Chevrot and A.Baldisseri)

Revision 1.14.4.9  2000/03/20 18:15:25  morsch
Positions of trigger chambers corrected (P.C.)

Revision 1.14.4.8  2000/02/21 15:38:01  morsch
Call to AddHitList introduced to make this version compatible with head.

Revision 1.14.4.7  2000/02/20 07:45:53  morsch
Bugs in Trigger part of BuildGeomemetry corrected (P.C)

Revision 1.14.4.6  2000/02/17 14:28:54  morsch
Trigger included into initialization and digitization

Revision 1.14.4.5  2000/02/15 10:02:58  morsch
Log messages of previous revisions added

Revision 1.14.4.2  2000/02/04 10:57:34  gosset
Z position of the chambers:
it was the Z position of the stations;
it is now really the Z position of the chambers.
   !!!! WARNING: THE CALLS TO "AliMUONChamber::SetZPOS"
   !!!!                   AND "AliMUONChamber::ZPosition"
   !!!! HAVE TO BE CHANGED TO "AliMUONChamber::"SetZ"
   !!!!                   AND "AliMUONChamber::Z"

Revision 1.14.4.3  2000/02/04 16:19:04  gosset
Correction for mis-spelling of NCH 

Revision 1.14.4.4  2000/02/15 09:43:38  morsch
Log message added

*/


///////////////////////////////////////////////
//  Manager and hits classes for set:MUON     //
////////////////////////////////////////////////

#include <TTUBE.h>
#include <TBRIK.h>
#include <TRotMatrix.h>
#include <TGeometry.h>
#include <TNode.h> 
#include <TTree.h> 
#include <TRandom.h> 
#include <TObject.h>
#include <TVector.h>
#include <TObjArray.h>
#include <TMinuit.h>
#include <TParticle.h>
#include <TROOT.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TDirectory.h>
#include <TObjectTable.h>
#include <AliPDG.h>
#include <TTUBE.h>

#include "AliMUON.h"
#include "AliMUONHit.h"
#include "AliMUONPadHit.h"
#include "AliMUONDigit.h"
#include "AliMUONTransientDigit.h"
#include "AliMUONRawCluster.h"
#include "AliMUONLocalTrigger.h"
#include "AliMUONGlobalTrigger.h"
#include "AliMUONTriggerCircuit.h"
#include "AliHitMap.h"
#include "AliMUONHitMapA1.h"
#include "AliMUONChamberTrigger.h"
#include "AliMUONConstants.h"
#include "AliMUONClusterFinderVS.h"
#include "AliMUONTriggerDecision.h"
#include "AliRun.h"
#include "AliHeader.h"
#include "AliMUONClusterInput.h"
#include "AliMUONMerger.h"	
#include "Riostream.h"
#include "AliConst.h" 

// Defaults parameters for Z positions of chambers
// taken from values for "stations" in AliMUON::AliMUON
//     const Float_t zch[7]={528, 690., 975., 1249., 1449., 1610, 1710.};
// and from array "dstation" in AliMUONv1::CreateGeometry
//          Float_t dstation[5]={20., 20., 20, 20., 20.};
//     for tracking chambers,
//          according to (Z1 = zch - dstation) and  (Z2 = zch + dstation)
//          for the first and second chambers in the station, respectively,
// and from "DTPLANES" in AliMUONv1::CreateGeometry
//           const Float_t DTPLANES = 15.;
//     for trigger chambers,
//          according to (Z1 = zch) and  (Z2 = zch + DTPLANES)
//          for the first and second chambers in the station, respectively

ClassImp(AliMUON)
//___________________________________________
AliMUON::AliMUON()
{
// Default Constructor
//
    fNCh             = 0;
    fNTrackingCh     = 0;
    fIshunt          = 0;
    fPadHits         = 0;
    fNPadHits        = 0;
    fChambers        = 0;
    fDchambers       = 0;
    fTriggerCircuits = 0;  
    fNdch            = 0;
    fRawClusters     = 0;
    fNrawch          = 0;
    fGlobalTrigger   = 0;
    fNLocalTrigger   = 0;
    fLocalTrigger    = 0;
    fNLocalTrigger   = 0;
    fAccMin          = 0.;
    fAccMax          = 0.;   
    fAccCut          = kFALSE;
    fMerger          = 0;
    fFileName        = 0;
}
 
//___________________________________________
AliMUON::AliMUON(const char *name, const char *title)
       : AliDetector(name,title)
{
//Begin_Html
/*
<img src="gif/alimuon.gif">
*/
//End_Html

   fHits     = new TClonesArray("AliMUONHit",1000);
   gAlice->AddHitList(fHits);
   fPadHits = new TClonesArray("AliMUONPadHit",10000);
   fNPadHits  =  0;
   fIshunt     =  0;

   fNCh             = AliMUONConstants::NCh(); 
   fNTrackingCh     = AliMUONConstants::NTrackingCh();
   fNdch            = new Int_t[fNCh];

   fDchambers = new TObjArray(AliMUONConstants::NCh());

   Int_t i;
   
   for (i=0; i<AliMUONConstants::NCh() ;i++) {
       fDchambers->AddAt(new TClonesArray("AliMUONDigit",10000),i); 
       fNdch[i]=0;
   }

   fNrawch      = new Int_t[fNTrackingCh];

   fRawClusters = new TObjArray(AliMUONConstants::NTrackingCh());

   for (i=0; i<AliMUONConstants::NTrackingCh();i++) {
       fRawClusters->AddAt(new TClonesArray("AliMUONRawCluster",10000),i); 
       fNrawch[i]=0;
   }

   fGlobalTrigger = new TClonesArray("AliMUONGlobalTrigger",1);    
   fNGlobalTrigger = 0;
   fLocalTrigger  = new TClonesArray("AliMUONLocalTrigger",234);    
   fNLocalTrigger = 0;

   SetMarkerColor(kRed);
//
//
//
//

    Int_t ch;

    fChambers = new TObjArray(AliMUONConstants::NCh());

    // Loop over stations
    for (Int_t st = 0; st < AliMUONConstants::NCh() / 2; st++) {
      // Loop over 2 chambers in the station
	for (Int_t stCH = 0; stCH < 2; stCH++) {
//
//    
//    Default Parameters for Muon Tracking Stations


	    ch = 2 * st + stCH;
//
	    if (ch < AliMUONConstants::NTrackingCh()) {
	      fChambers->AddAt(new AliMUONChamber(ch),ch);
	    } else {
	      fChambers->AddAt(new AliMUONChamberTrigger(ch),ch);
	    }
	    
        //PH	    AliMUONChamber* chamber = (AliMUONChamber*) (*fChambers)[ch];
	    AliMUONChamber* chamber = (AliMUONChamber*) fChambers->At(ch);
	    
	    chamber->SetGid(0);
	    // Default values for Z of chambers
	    chamber->SetZ(AliMUONConstants::DefaultChamberZ(ch));
//
	    chamber->InitGeo(AliMUONConstants::DefaultChamberZ(ch));
//          Set chamber inner and outer radius to default
	    chamber->SetRInner(AliMUONConstants::Dmin(st)/2);
	    chamber->SetROuter(AliMUONConstants::Dmax(st)/2);
//
	} // Chamber stCH (0, 1) in 
    }     // Station st (0...)
//    fChambers->SetLast(AliMUONConstants::NCh());
    fMaxStepGas=0.01; 
    fMaxStepAlu=0.1; 
    fMaxDestepGas=-1;
    fMaxDestepAlu=-1;
//
   fMaxIterPad   = 0;
   fCurIterPad   = 0;
//
   fAccMin          = 0.;
   fAccMax          = 0.;   
   fAccCut          = kFALSE;

   // cp new design of AliMUONTriggerDecision
   fTriggerCircuits = new TObjArray(AliMUONConstants::NTriggerCircuit());
   for (Int_t circ=0; circ<AliMUONConstants::NTriggerCircuit(); circ++) {
     fTriggerCircuits->AddAt(new AliMUONTriggerCircuit(),circ);     

   }
     fMerger = 0;
}
 
//___________________________________________
AliMUON::AliMUON(const AliMUON& rMUON)
{
// Dummy copy constructor
    ;
    
}

AliMUON::~AliMUON()
{
// Destructor
    if(fDebug) printf("%s: Calling AliMUON destructor !!!\n",ClassName());
    
    fIshunt  = 0;
 
    // Delete TObjArrays
 
    if (fChambers){
      fChambers->Delete();
      delete fChambers;
    }
 
    if (fTriggerCircuits){
      fTriggerCircuits->Delete();
      delete fTriggerCircuits;
    }
 
    if (fDchambers){
      fDchambers->Delete();
      delete fDchambers;
    }
 
    if (fRawClusters){
      fRawClusters->Delete();
      delete fRawClusters;
    }

    if (fNrawch) delete [] fNrawch;
 
    // Delete TClonesArrays
 
    if (fPadHits){
      fPadHits->Delete();
      delete fPadHits;
    }
 
    if (fGlobalTrigger){
      fGlobalTrigger->Delete();
      delete fGlobalTrigger;
    }
    fNGlobalTrigger = 0;
    
    if (fLocalTrigger){
      fLocalTrigger->Delete();
      delete fLocalTrigger;
    }
    fNLocalTrigger = 0;

    if (fHits) {
      fHits->Delete();
      delete fHits;
    }

    if (fMerger) delete fMerger;
    if (fNdch) delete [] fNdch;

}
 
//___________________________________________
void AliMUON::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliMUONHit(fIshunt,track,vol,hits);
}
//___________________________________________
void AliMUON::AddPadHit(Int_t *clhits)
{
   TClonesArray &lclusters = *fPadHits;
   new(lclusters[fNPadHits++]) AliMUONPadHit(clhits);
}
//_____________________________________________________________________________
void AliMUON::AddDigits(Int_t id, Int_t *tracks, Int_t *charges, Int_t *digits)
{
    //
    // Add a MUON digit to the list
    //

  //PH    TClonesArray &ldigits = *((TClonesArray*)(*fDchambers)[id]);
    TClonesArray &ldigits = *((TClonesArray*)fDchambers->At(id));
    new(ldigits[fNdch[id]++]) AliMUONDigit(tracks,charges,digits);
}

//_____________________________________________________________________________
void AliMUON::AddRawCluster(Int_t id, const AliMUONRawCluster& c)
{
    //
    // Add a MUON digit to the list
    //

  //PH    TClonesArray &lrawcl = *((TClonesArray*)(*fRawClusters)[id]);
    TClonesArray &lrawcl = *((TClonesArray*)fRawClusters->At(id));
    new(lrawcl[fNrawch[id]++]) AliMUONRawCluster(c);
}

//___________________________________________
void AliMUON::AddGlobalTrigger(Int_t *singlePlus, Int_t *singleMinus,
			       Int_t *singleUndef,
			       Int_t *pairUnlike, Int_t *pairLike)
{
// add a MUON Global Trigger to the list (only one GlobalTrigger per event !)
  TClonesArray &globalTrigger = *fGlobalTrigger;
  new(globalTrigger[fNGlobalTrigger++]) 
    AliMUONGlobalTrigger(singlePlus, singleMinus,  singleUndef, pairUnlike, 
			 pairLike);
}
//___________________________________________
void AliMUON::AddLocalTrigger(Int_t *localtr)
{
// add a MUON Local Trigger to the list
  TClonesArray &localTrigger = *fLocalTrigger;
  new(localTrigger[fNLocalTrigger++]) AliMUONLocalTrigger(localtr);
}

//___________________________________________
void AliMUON::BuildGeometry()
{
// Geometry for event display
  for (Int_t i=0; i<7; i++) {
    for (Int_t j=0; j<2; j++) {
      Int_t id=2*i+j+1;
      this->Chamber(id-1).SegmentationModel(1)->Draw("eventdisplay");
    }
  }
}

//___________________________________________
Int_t AliMUON::DistancetoPrimitive(Int_t , Int_t )
{
   return 9999;
}

//___________________________________________
void AliMUON::MakeBranch(Option_t* option, const char *file)
{
    //
    // Create Tree branches for the MUON.
    //
    const Int_t kBufferSize = 4000;
    char branchname[30];
    sprintf(branchname,"%sCluster",GetName());
    
    AliDetector::MakeBranch(option,file);
    
    const char *cD = strstr(option,"D");
    const char *cR = strstr(option,"R");
    const char *cH = strstr(option,"H");

    if (fPadHits   && gAlice->TreeH() && cH) {
      MakeBranchInTree(gAlice->TreeH(), 
                       branchname, &fPadHits, kBufferSize, file);
    }
    
    if (cD) {
      //
      // one branch for digits per chamber
      // 
      Int_t i;
    
      for (i=0; i<AliMUONConstants::NCh() ;i++) {
	    sprintf(branchname,"%sDigits%d",GetName(),i+1);	
	    if (fDchambers   && gAlice->TreeD()) {
            MakeBranchInTree(gAlice->TreeD(), 
                             branchname, &((*fDchambers)[i]), kBufferSize, file);
	      printf("Making Branch %s for digits in chamber %d\n",branchname,i+1);
        }
	  }	
    }
    
    if (cR) {
      //     
      // one branch for raw clusters per chamber
      //  
      printf("Make Branch - TreeR address %p\n",gAlice->TreeR());
      
      Int_t i;

      for (i=0; i<AliMUONConstants::NTrackingCh() ;i++) {
	    sprintf(branchname,"%sRawClusters%d",GetName(),i+1);	
	    if (fRawClusters   && gAlice->TreeR()) {
              MakeBranchInTree(gAlice->TreeR(), 
                               branchname, &((*fRawClusters)[i]), kBufferSize, file);
	      printf("Making Branch %s for raw clusters in chamber %d\n",branchname,i+1);
   	    }	
      }
      //
      // one branch for global trigger
      //
      sprintf(branchname,"%sGlobalTrigger",GetName());
      if (fGlobalTrigger && gAlice->TreeR()) {  
          MakeBranchInTree(gAlice->TreeR(), 
                           branchname, &fGlobalTrigger, kBufferSize, file);
	    printf("Making Branch %s for Global Trigger\n",branchname);
      }
      //
      // one branch for local trigger
      //  
      sprintf(branchname,"%sLocalTrigger",GetName());
      if (fLocalTrigger && gAlice->TreeR()) {  
          MakeBranchInTree(gAlice->TreeR(), 
                           branchname, &fLocalTrigger, kBufferSize, file);
	    printf("Making Branch %s for Local Trigger\n",branchname);
      }
   }
}

//___________________________________________
void AliMUON::SetTreeAddress()
{
  // Set branch address for the Hits and Digits Tree.
  char branchname[30];
  AliDetector::SetTreeAddress();

  TBranch *branch;
  TTree *treeH = gAlice->TreeH();
  TTree *treeD = gAlice->TreeD();
  TTree *treeR = gAlice->TreeR();

  if (treeH) {
    if (fPadHits) {
      branch = treeH->GetBranch("MUONCluster");
      if (branch) branch->SetAddress(&fPadHits);
    }
  }

  if (treeD) {
      for (int i=0; i<AliMUONConstants::NCh(); i++) {
	  sprintf(branchname,"%sDigits%d",GetName(),i+1);
	  if (fDchambers) {
	      branch = treeD->GetBranch(branchname);
	      if (branch) branch->SetAddress(&((*fDchambers)[i]));
	  }
      }
  }

  // printf("SetTreeAddress --- treeR address  %p \n",treeR);

  if (treeR) {
      for (int i=0; i<AliMUONConstants::NTrackingCh(); i++) {
	  sprintf(branchname,"%sRawClusters%d",GetName(),i+1);
	  if (fRawClusters) {
	      branch = treeR->GetBranch(branchname);
	      if (branch) branch->SetAddress(&((*fRawClusters)[i]));
	  }
      }

      if (fLocalTrigger) {
	branch = treeR->GetBranch("MUONLocalTrigger");
	if (branch) branch->SetAddress(&fLocalTrigger);
      }
      if (fGlobalTrigger) {
	branch = treeR->GetBranch("MUONGlobalTrigger");
	if (branch) branch->SetAddress(&fGlobalTrigger);
      }
  }
}
//___________________________________________
void AliMUON::ResetHits()
{
  // Reset number of clusters and the cluster array for this detector
  AliDetector::ResetHits();
  fNPadHits = 0;
  if (fPadHits) fPadHits->Clear();
}

//____________________________________________
void AliMUON::ResetDigits()
{
    //
    // Reset number of digits and the digits array for this detector
    //
    for ( int i=0;i<AliMUONConstants::NCh();i++ ) {
      //PH	if ((*fDchambers)[i])    ((TClonesArray*)(*fDchambers)[i])->Clear();
	if ((*fDchambers)[i])    ((TClonesArray*)fDchambers->At(i))->Clear();
	if (fNdch)  fNdch[i]=0;
    }
}
//____________________________________________
void AliMUON::ResetRawClusters()
{
    //
    // Reset number of raw clusters and the raw clust array for this detector
    //
    for ( int i=0;i<AliMUONConstants::NTrackingCh();i++ ) {
      //PH	if ((*fRawClusters)[i])    ((TClonesArray*)(*fRawClusters)[i])->Clear();
	if ((*fRawClusters)[i])    ((TClonesArray*)fRawClusters->At(i))->Clear();
	if (fNrawch)  fNrawch[i]=0;
    }
}

//____________________________________________
void AliMUON::ResetTrigger()
{
  //  Reset Local and Global Trigger 
  fNGlobalTrigger = 0;
  if (fGlobalTrigger) fGlobalTrigger->Clear();
  fNLocalTrigger = 0;
  if (fLocalTrigger) fLocalTrigger->Clear();
}

//____________________________________________
void AliMUON::SetPadSize(Int_t id, Int_t isec, Float_t p1, Float_t p2)
{
// Set the pad size for chamber id and cathode isec
    Int_t i=2*(id-1);
    //PH    ((AliMUONChamber*) (*fChambers)[i])  ->SetPadSize(isec,p1,p2);
    //PH    ((AliMUONChamber*) (*fChambers)[i+1])->SetPadSize(isec,p1,p2);
    ((AliMUONChamber*) fChambers->At(i))  ->SetPadSize(isec,p1,p2);
    ((AliMUONChamber*) fChambers->At(i+1))->SetPadSize(isec,p1,p2);
}

//___________________________________________
void AliMUON::SetChambersZ(const Float_t *Z)
{
  // Set Z values for all chambers (tracking and trigger)
  // from the array pointed to by "Z"
    for (Int_t ch = 0; ch < AliMUONConstants::NCh(); ch++)
      //PH	((AliMUONChamber*) ((*fChambers)[ch]))->SetZ(Z[ch]);
	((AliMUONChamber*) fChambers->At(ch))->SetZ(Z[ch]);
    return;
}

//___________________________________________
void AliMUON::SetChambersZToDefault()
{
  // Set Z values for all chambers (tracking and trigger)
  // to default values
  SetChambersZ(AliMUONConstants::DefaultChamberZ());
  return;
}

//___________________________________________
void AliMUON::SetChargeSlope(Int_t id, Float_t p1)
{
// Set the inverse charge slope for chamber id
    Int_t i=2*(id-1);
    //PH    ((AliMUONChamber*) (*fChambers)[i])->SetChargeSlope(p1);
    //PH    ((AliMUONChamber*) (*fChambers)[i+1])->SetChargeSlope(p1);
    ((AliMUONChamber*) fChambers->At(i))->SetChargeSlope(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetChargeSlope(p1);
}

//___________________________________________
void AliMUON::SetChargeSpread(Int_t id, Float_t p1, Float_t p2)
{
// Set sigma of charge spread for chamber id
    Int_t i=2*(id-1);
    //PH    ((AliMUONChamber*) fChambers->At(i))->SetChargeSpread(p1,p2);
    //PH    ((AliMUONChamber*) fChambers->Ati+1])->SetChargeSpread(p1,p2);
    ((AliMUONChamber*) fChambers->At(i))->SetChargeSpread(p1,p2);
    ((AliMUONChamber*) fChambers->At(i+1))->SetChargeSpread(p1,p2);
}

//___________________________________________
void AliMUON::SetSigmaIntegration(Int_t id, Float_t p1)
{
// Set integration limits for charge spread
    Int_t i=2*(id-1);
    //PH    ((AliMUONChamber*) (*fChambers)[i])->SetSigmaIntegration(p1);
    //PH    ((AliMUONChamber*) (*fChambers)[i+1])->SetSigmaIntegration(p1);
    ((AliMUONChamber*) fChambers->At(i))->SetSigmaIntegration(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetSigmaIntegration(p1);
}

//___________________________________________
void AliMUON::SetMaxAdc(Int_t id, Int_t p1)
{
// Set maximum number for ADCcounts (saturation)
    Int_t i=2*(id-1);
    //PH    ((AliMUONChamber*) (*fChambers)[i])->SetMaxAdc(p1);
    //PH    ((AliMUONChamber*) (*fChambers)[i+1])->SetMaxAdc(p1);
    ((AliMUONChamber*) fChambers->At(i))->SetMaxAdc(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetMaxAdc(p1);
}

//___________________________________________
void AliMUON::SetMaxStepGas(Float_t p1)
{
// Set stepsize in gas
     fMaxStepGas=p1;
}

//___________________________________________
void AliMUON::SetMaxStepAlu(Float_t p1)
{
// Set step size in Alu
    fMaxStepAlu=p1;
}

//___________________________________________
void AliMUON::SetMaxDestepGas(Float_t p1)
{
// Set maximum step size in Gas
    fMaxDestepGas=p1;
}

//___________________________________________
void AliMUON::SetMaxDestepAlu(Float_t p1)
{
// Set maximum step size in Alu
    fMaxDestepAlu=p1;
}
//___________________________________________
void AliMUON::SetAcceptance(Bool_t acc, Float_t angmin, Float_t angmax)
{
// Set acceptance cuts 
   fAccCut=acc;
   fAccMin=angmin*TMath::Pi()/180;
   fAccMax=angmax*TMath::Pi()/180;
   Int_t ch;
   if (acc) {
       for (Int_t st = 0; st < AliMUONConstants::NCh() / 2; st++) {
	   // Loop over 2 chambers in the station
	   for (Int_t stCH = 0; stCH < 2; stCH++) {
	       ch = 2 * st + stCH;
//         Set chamber inner and outer radius according to acceptance cuts
	       Chamber(ch).SetRInner(AliMUONConstants::DefaultChamberZ(ch)*TMath::Tan(fAccMin));
	       Chamber(ch).SetROuter(AliMUONConstants::DefaultChamberZ(ch)*TMath::Tan(fAccMax));
	   } // chamber loop
       } // station loop
   }
}
//___________________________________________
void   AliMUON::SetSegmentationModel(Int_t id, Int_t isec, AliSegmentation *segmentation)
{
// Set the segmentation for chamber id cathode isec
  //PH    ((AliMUONChamber*) (*fChambers)[id])->SetSegmentationModel(isec, segmentation);
    ((AliMUONChamber*) fChambers->At(id))->SetSegmentationModel(isec, segmentation);

}
//___________________________________________
void   AliMUON::SetResponseModel(Int_t id, AliMUONResponse *response)
{
// Set the response for chamber id
  //PH    ((AliMUONChamber*) (*fChambers)[id])->SetResponseModel(response);
    ((AliMUONChamber*) fChambers->At(id))->SetResponseModel(response);
}

void   AliMUON::SetReconstructionModel(Int_t id, AliMUONClusterFinderVS *reconst)
{
// Set ClusterFinder for chamber id
  //PH    ((AliMUONChamber*) (*fChambers)[id])->SetReconstructionModel(reconst);
    ((AliMUONChamber*) fChambers->At(id))->SetReconstructionModel(reconst);
}

void   AliMUON::SetNsec(Int_t id, Int_t nsec)
{
// Set number of segmented cathods for chamber id
  //PH    ((AliMUONChamber*) (*fChambers)[id])->SetNsec(nsec);
    ((AliMUONChamber*) fChambers->At(id))->SetNsec(nsec);
}

//___________________________________________
void AliMUON::SDigits2Digits()
{

// write TreeD here 

    if (!fMerger) {
      if (gAlice->GetDebug()>0) {
	cerr<<"AliMUON::SDigits2Digits: create default AliMUONMerger "<<endl;
	cerr<<" no merging, just digitization of 1 event will be done"<<endl;
      }
      fMerger = new AliMUONMerger();
    }
    fMerger->Init();
    fMerger->Digitise();
    char hname[30];
    sprintf(hname,"TreeD%d",gAlice->GetHeader()->GetEvent());
    gAlice->TreeD()->Write(hname,TObject::kOverwrite);
    gAlice->TreeD()->Reset();
}

//___________________________________________
void AliMUON::MakePadHits(Float_t xhit,Float_t yhit, Float_t zhit,
			  Float_t eloss, Float_t tof,  Int_t idvol)
{
//
//  Calls the charge disintegration method of the current chamber and adds
//  the simulated cluster to the root treee 
//
    Int_t clhits[7];
    Float_t newclust[6][500];
    Int_t nnew;
    
    
//
//  Integrated pulse height on chamber

    
    clhits[0]=fNhits+1;
//
//
//    if (idvol == 6) printf("\n ->Disintegration %f %f %f", xhit, yhit, eloss );
    

    //PH    ((AliMUONChamber*) (*fChambers)[idvol])
    ((AliMUONChamber*) fChambers->At(idvol))
	->DisIntegration(eloss, tof, xhit, yhit, zhit, nnew, newclust);
    Int_t ic=0;
//    if (idvol == 6) printf("\n nnew  %d \n", nnew);
//
//  Add new clusters
    for (Int_t i=0; i<nnew; i++) {
	if (Int_t(newclust[3][i]) > 0) {
	    ic++;
// Cathode plane
	    clhits[1] = Int_t(newclust[5][i]);
//  Cluster Charge
	    clhits[2] = Int_t(newclust[0][i]);
//  Pad: ix
	    clhits[3] = Int_t(newclust[1][i]);
//  Pad: iy 
	    clhits[4] = Int_t(newclust[2][i]);
//  Pad: charge
	    clhits[5] = Int_t(newclust[3][i]);
//  Pad: chamber sector
	    clhits[6] = Int_t(newclust[4][i]);
	    
	    AddPadHit(clhits);
	}
    }
}

//___________________________________________
void AliMUON::Trigger(Int_t nev){
// call the Trigger Algorithm and fill TreeR

  Int_t singlePlus[3]  = {0,0,0}; 
  Int_t singleMinus[3] = {0,0,0}; 
  Int_t singleUndef[3] = {0,0,0};
  Int_t pairUnlike[3]  = {0,0,0}; 
  Int_t pairLike[3]    = {0,0,0};

  ResetTrigger();
  AliMUONTriggerDecision* decision= new AliMUONTriggerDecision(1);
  decision->Trigger();   
  decision->GetGlobalTrigger(singlePlus, singleMinus, singleUndef,
			     pairUnlike, pairLike);
// add a local trigger in the list 
  AddGlobalTrigger(singlePlus, singleMinus, singleUndef, pairUnlike, pairLike);
  Int_t i;
  
  for (Int_t icirc=0; icirc<AliMUONConstants::NTriggerCircuit(); icirc++) { 
      if(decision->GetITrigger(icirc)==1) {
	  Int_t localtr[7]={0,0,0,0,0,0,0};      
	  Int_t loLpt[2]={0,0}; Int_t loHpt[2]={0,0}; Int_t loApt[2]={0,0};
	  decision->GetLutOutput(icirc, loLpt, loHpt, loApt);
	  localtr[0] = icirc;
	  localtr[1] = decision->GetStripX11(icirc);
	  localtr[2] = decision->GetDev(icirc);
	  localtr[3] = decision->GetStripY11(icirc);
	  for (i=0; i<2; i++) {    // convert the Lut output in 1 digit 
	      localtr[4] = localtr[4]+Int_t(loLpt[i]*TMath::Power(2,i));
	      localtr[5] = localtr[5]+Int_t(loHpt[i]*TMath::Power(2,i));
	      localtr[6] = localtr[6]+Int_t(loApt[i]*TMath::Power(2,i));
	  }
	  AddLocalTrigger(localtr);  // add a local trigger in the list
      }
  }
  delete decision;

  gAlice->TreeR()->Fill();
  ResetTrigger();
  char hname[30];
  sprintf(hname,"TreeR%d",nev);
  gAlice->TreeR()->Write(hname,TObject::kOverwrite);
  gAlice->TreeR()->Reset();
  printf("\n End of trigger for event %d", nev);
}


//____________________________________________
void AliMUON::Digits2Reco()
{
  FindClusters();
  Int_t nev = gAlice->GetHeader()->GetEvent();
  gAlice->TreeR()->Fill();
  char hname[30];
  sprintf(hname,"TreeR%d", nev);
  gAlice->TreeR()->Write(hname);
  gAlice->TreeR()->Reset();
  ResetRawClusters();        
  printf("\n End of cluster finding for event %d", nev);
}

void AliMUON::FindClusters()
{
//
//  Perform cluster finding
//
    TClonesArray *dig1, *dig2;
    Int_t ndig, k;
    dig1 = new TClonesArray("AliMUONDigit",1000);
    dig2 = new TClonesArray("AliMUONDigit",1000);
    AliMUONDigit *digit;
//
// Loop on chambers and on cathode planes
//
    ResetRawClusters();        
    for (Int_t ich = 0; ich < 10; ich++) {
      //PH	AliMUONChamber* iChamber = (AliMUONChamber*) (*fChambers)[ich];
	AliMUONChamber* iChamber = (AliMUONChamber*) fChambers->At(ich);
	AliMUONClusterFinderVS* rec = iChamber->ReconstructionModel();
    
	gAlice->ResetDigits();
	gAlice->TreeD()->GetEvent(0);
	TClonesArray *muonDigits = this->DigitsAddress(ich);
	ndig=muonDigits->GetEntriesFast();
	printf("\n 1 Found %d digits in %p %d", ndig, muonDigits,ich);
	TClonesArray &lhits1 = *dig1;
	Int_t n = 0;
	for (k = 0; k < ndig; k++) {
	    digit = (AliMUONDigit*) muonDigits->UncheckedAt(k);
	    if (rec->TestTrack(digit->Track(0)))
		new(lhits1[n++]) AliMUONDigit(*digit);
	}
	gAlice->ResetDigits();
	gAlice->TreeD()->GetEvent(1);
	muonDigits  = this->DigitsAddress(ich);
	ndig=muonDigits->GetEntriesFast();
	printf("\n 2 Found %d digits in %p %d", ndig, muonDigits, ich);
	TClonesArray &lhits2 = *dig2;
	n=0;
	
	for (k=0; k<ndig; k++) {
	    digit= (AliMUONDigit*) muonDigits->UncheckedAt(k);
	    if (rec->TestTrack(digit->Track(0)))
	    new(lhits2[n++]) AliMUONDigit(*digit);
	}

	if (rec) {	 
	    AliMUONClusterInput::Instance()->SetDigits(ich, dig1, dig2);
	    rec->FindRawClusters();
	}
	dig1->Delete();
	dig2->Delete();
    } // for ich
    delete dig1;
    delete dig2;
}
 
#ifdef never
void AliMUON::Streamer(TBuffer &R__b)
{
   // Stream an object of class AliMUON.
      AliMUONChamber        *iChamber;
      AliMUONTriggerCircuit *iTriggerCircuit;
      AliSegmentation       *segmentation;
      AliMUONResponse       *response;
      TClonesArray          *digitsaddress;
      TClonesArray          *rawcladdress;
      Int_t i;
      if (R__b.IsReading()) {
	  Version_t R__v = R__b.ReadVersion(); if (R__v) { }
	  AliDetector::Streamer(R__b);
	  R__b >> fNPadHits;
	  R__b >> fPadHits; // diff
	  R__b >> fNLocalTrigger;       
	  R__b >> fLocalTrigger;       
	  R__b >> fNGlobalTrigger;       
	  R__b >> fGlobalTrigger;   
	  R__b >> fDchambers;
	  R__b >> fRawClusters;
	  R__b.ReadArray(fNdch);
	  R__b.ReadArray(fNrawch);
	  R__b >> fAccCut;
	  R__b >> fAccMin;
	  R__b >> fAccMax; 
	  R__b >> fChambers;
	  R__b >> fTriggerCircuits;
	  for (i =0; i<AliMUONConstants::NTriggerCircuit(); i++) {
	      iTriggerCircuit=(AliMUONTriggerCircuit*) (*fTriggerCircuits)[i];
	      iTriggerCircuit->Streamer(R__b);
	  }
// Stream chamber related information
	  for (i =0; i<AliMUONConstants::NCh(); i++) {
	      iChamber=(AliMUONChamber*) (*fChambers)[i];
	      iChamber->Streamer(R__b);
	      if (iChamber->Nsec()==1) {
		  segmentation=iChamber->SegmentationModel(1);
		  if (segmentation)
		      segmentation->Streamer(R__b);
	      } else {
		  segmentation=iChamber->SegmentationModel(1);
		  if (segmentation)
		      segmentation->Streamer(R__b);
		  if (segmentation)
		      segmentation=iChamber->SegmentationModel(2);
		  segmentation->Streamer(R__b);
	      }
	      response=iChamber->ResponseModel();
	      if (response)
		  response->Streamer(R__b);	  
	      digitsaddress=(TClonesArray*) (*fDchambers)[i];
	      digitsaddress->Streamer(R__b);
	      if (i < AliMUONConstants::NTrackingCh()) {
		  rawcladdress=(TClonesArray*) (*fRawClusters)[i];
		  rawcladdress->Streamer(R__b);
	      }
	  }
	  
      } else {
	  R__b.WriteVersion(AliMUON::IsA());
	  AliDetector::Streamer(R__b);
	  R__b << fNPadHits;
	  R__b << fPadHits; // diff
	  R__b << fNLocalTrigger;       
	  R__b << fLocalTrigger;       
	  R__b << fNGlobalTrigger;       
	  R__b << fGlobalTrigger; 
	  R__b << fDchambers;
	  R__b << fRawClusters;
	  R__b.WriteArray(fNdch, AliMUONConstants::NCh());
	  R__b.WriteArray(fNrawch, AliMUONConstants::NTrackingCh());
	  
	  R__b << fAccCut;
	  R__b << fAccMin;
	  R__b << fAccMax; 
	  
	  R__b << fChambers;
	  R__b << fTriggerCircuits;
	  for (i =0; i<AliMUONConstants::NTriggerCircuit(); i++) {
	      iTriggerCircuit=(AliMUONTriggerCircuit*) (*fTriggerCircuits)[i];
	      iTriggerCircuit->Streamer(R__b);
	  }
	  for (i =0; i<AliMUONConstants::NCh(); i++) {
	      iChamber=(AliMUONChamber*) (*fChambers)[i];
	      iChamber->Streamer(R__b);
	      if (iChamber->Nsec()==1) {
		  segmentation=iChamber->SegmentationModel(1);
		  if (segmentation)
		      segmentation->Streamer(R__b);
	      } else {
		  segmentation=iChamber->SegmentationModel(1);
		  if (segmentation)
		      segmentation->Streamer(R__b);
		  segmentation=iChamber->SegmentationModel(2);
		  if (segmentation)
		      segmentation->Streamer(R__b);
	      }
	      response=iChamber->ResponseModel();
	      if (response)
		  response->Streamer(R__b);
	      digitsaddress=(TClonesArray*) (*fDchambers)[i];
	      digitsaddress->Streamer(R__b);
	      if (i < AliMUONConstants::NTrackingCh()) {
		  rawcladdress=(TClonesArray*) (*fRawClusters)[i];
		  rawcladdress->Streamer(R__b);
	      }
	  }
      }
}
#endif

AliMUONPadHit* AliMUON::FirstPad(AliMUONHit*  hit, TClonesArray *clusters) 
{
//
    // Initialise the pad iterator
    // Return the address of the first padhit for hit
    TClonesArray *theClusters = clusters;
    Int_t nclust = theClusters->GetEntriesFast();
    if (nclust && hit->PHlast() > 0) {
	AliMUON::fMaxIterPad=hit->PHlast();
	AliMUON::fCurIterPad=hit->PHfirst();
	return (AliMUONPadHit*) clusters->UncheckedAt(AliMUON::fCurIterPad-1);
    } else {
	return 0;
    }
}

AliMUONPadHit* AliMUON::NextPad(TClonesArray *clusters) 
{
// Get next pad (in iterator) 
//
    AliMUON::fCurIterPad++;
    if (AliMUON::fCurIterPad <= AliMUON::fMaxIterPad) {
	return (AliMUONPadHit*) clusters->UncheckedAt(AliMUON::fCurIterPad-1);
    } else {
	return 0;
    }
}


AliMUONRawCluster *AliMUON::RawCluster(Int_t ichamber, Int_t icathod, Int_t icluster)
{
//
//  Return rawcluster (icluster) for chamber ichamber and cathode icathod
//  Obsolete ??
    TClonesArray *muonRawCluster  = RawClustAddress(ichamber);
    ResetRawClusters();
    TTree *treeR = gAlice->TreeR();
    Int_t nent=(Int_t)treeR->GetEntries();
    treeR->GetEvent(nent-2+icathod-1);
    //treeR->GetEvent(icathod);
    //Int_t nrawcl = (Int_t)muonRawCluster->GetEntriesFast();

    AliMUONRawCluster * mRaw = (AliMUONRawCluster*)muonRawCluster->UncheckedAt(icluster);
    //printf("RawCluster _ nent nrawcl icluster mRaw %d %d %d%p\n",nent,nrawcl,icluster,mRaw);
    
    return  mRaw;
}
 
void   AliMUON::SetMerger(AliMUONMerger* merger)
{
// Set pointer to merger 
    fMerger = merger;
}

AliMUONMerger*  AliMUON::Merger()
{
// Return pointer to merger
    return fMerger;
}



AliMUON& AliMUON::operator = (const AliMUON& rhs)
{
// copy operator
// dummy version
    return *this;
}

////////////////////////////////////////////////////////////////////////
void AliMUON::MakeBranchInTreeD(TTree *treeD, const char *file)
{
    //
    // Create TreeD branches for the MUON.
    //

  const Int_t kBufferSize = 4000;
  char branchname[30];
    
  //
  // one branch for digits per chamber
  // 
  for (Int_t i=0; i<AliMUONConstants::NCh() ;i++) {
    sprintf(branchname,"%sDigits%d",GetName(),i+1);	
    if (fDchambers && treeD) {
      MakeBranchInTree(treeD, 
		       branchname, &((*fDchambers)[i]), kBufferSize, file);
//      printf("Making Branch %s for digits in chamber %d\n",branchname,i+1);
    }
  }
}

//___________________________________________


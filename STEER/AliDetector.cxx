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
Revision 1.16.6.5  2002/10/09 09:23:55  hristov
New task hierarchy, bug corrections, new development (P.Skowronski)

Revision 1.16.6.4  2002/06/28 16:45:17  hristov
Few minor corrections

Revision 1.16.6.3  2002/06/18 10:18:31  hristov
Important update (P.Skowronski)

Revision 1.16.6.2  2002/06/06 14:18:33  hristov
Merged with v3-08-02

Revision 1.16.6.1  2002/05/31 09:37:59  hristov
First set of changes done by Piotr

Revision 1.23  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.22  2002/10/23 07:43:00  alibrary
Introducing some effective C++ suggestions

Revision 1.21  2002/10/22 15:02:15  alibrary
Introducing Riostream.h

Revision 1.20  2002/10/14 14:57:32  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.16.8.3  2002/10/14 09:45:57  hristov
Updating VirtualMC to v3-09-02

Revision 1.19  2002/09/23 09:19:54  hristov
FirsTrackReference updated (M.Ivanov)

Revision 1.18  2002/08/26 13:51:17  hristov
Remaping of track references at the end of each primary particle (M.Ivanov)

Revision 1.17  2002/05/24 13:29:58  hristov
AliTrackReference added, AliDisplay modified

Revision 1.16  2001/10/04 15:30:56  hristov
Changes to accommodate the set of PHOS folders and tasks (Y.Schutz)

Revision 1.15  2001/07/27 13:03:13  hristov
Default Branch split level set to 99

Revision 1.14  2001/05/21 17:22:51  buncic
Fixed problem with missing AliConfig while reading galice.root

Revision 1.13  2001/05/16 14:57:22  alibrary
New files for folders and Stack

Revision 1.12  2001/03/12 17:47:03  hristov
Changes needed on Sun with CC 5.0

Revision 1.11  2001/01/26 19:58:46  hristov
Major upgrade of AliRoot code

Revision 1.10  2001/01/17 10:50:50  hristov
Corrections to destructors

Revision 1.9  2000/12/12 18:19:06  alibrary
Introduce consistency check when loading points

Revision 1.8  2000/11/30 07:12:48  alibrary
Introducing new Rndm and QA classes

Revision 1.7  2000/10/02 21:28:14  fca
Removal of useless dependecies via forward declarations

Revision 1.6  2000/07/12 08:56:25  fca
Coding convention correction and warning removal

Revision 1.5  1999/09/29 09:24:29  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Base class for ALICE modules. Both sensitive modules (detectors) and      //
// non-sensitive ones are described by this base class. This class           //
// supports the hit and digit trees produced by the simulation and also      //
// the objects produced by the reconstruction.                               //
//                                                                           //
// This class is also responsible for building the geometry of the           //
// detectors.                                                                //
//                                                                           //
//Begin_Html
/*
<img src="picts/AliDetectorClass.gif">
*/
//End_Html
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include <TBrowser.h>
#include <TFile.h>
#include <TFolder.h>
#include <TROOT.h>
#include <TTree.h>
#include <Riostream.h>

#include "AliConfig.h"
#include "AliDetector.h"
#include "AliHit.h"
#include "AliPoints.h"
#include "AliLoader.h"
#include "AliRun.h"
#include "AliTrackReference.h"

// Static variables for the hit iterator routines
static Int_t sMaxIterHit=0;
static Int_t sCurIterHit=0;


ClassImp(AliDetector)
 
//_______________________________________________________________________
AliDetector::AliDetector():
  fTimeGate(200.e-9),
  fIshunt(0),
  fNhits(0),
  fNdigits(0),
  fBufferSize(1600),
  fHits(0),
  fDigits(0),
  fPoints(0),
  fTrackReferences(0),
  fMaxIterTrackRef(0),
  fCurrentIterTrackRef(0),
  fLoader(0x0)
{
  //
  // Default constructor for the AliDetector class
  //
}
 
//_______________________________________________________________________
AliDetector::AliDetector(const AliDetector &det):
  AliModule(det),
  fTimeGate(200.e-9),
  fIshunt(0),
  fNhits(0),
  fNdigits(0),
  fBufferSize(1600),
  fHits(0),
  fDigits(0),
  fPoints(0),
  fTrackReferences(0),
  fMaxIterTrackRef(0),
  fCurrentIterTrackRef(0),
  fLoader(0x0)
{
  det.Copy(*this);
}

//_____________________________________________________________________________
AliDetector::AliDetector(const char* name,const char *title):
  AliModule(name,title),
  fTimeGate(200.e-9),
  fIshunt(0),
  fNhits(0),
  fNdigits(0),
  fBufferSize(1600),
  fHits(0),
  fDigits(0),
  fPoints(0),
  fTrackReferences(new TClonesArray("AliTrackReference", 100)),
  fMaxIterTrackRef(0),
  fCurrentIterTrackRef(0),
  fLoader(0x0)
{
  //
  // Normal constructor invoked by all Detectors.
  // Create the list for detector specific histograms
  // Add this Detector to the global list of Detectors in Run.
  //

  fActive     = kTRUE;
  AliConfig::Instance()->Add(this);
}
 
//_______________________________________________________________________
AliDetector::~AliDetector()
{
  //
  // Destructor
  //
  // Delete space point structure
  if (fPoints) {
    fPoints->Delete();
    delete fPoints;
    fPoints     = 0;
  }
  // Delete digits structure
  if (fDigits) {
    fDigits->Delete();
    delete fDigits;
    fDigits     = 0;
  }

  if (fLoader)
   {
    fLoader->GetModulesFolder()->Remove(this);
   }

}

//_______________________________________________________________________
void AliDetector::Publish(const char *dir, void *address, const char *name)
{
//
// Register pointer to detector objects. 
// 
//  TFolder *topFolder = (TFolder *)gROOT->FindObjectAny("/Folders");
  MayNotUse("Publish");
}

//_______________________________________________________________________
TBranch* AliDetector::MakeBranchInTree(TTree *tree, const char* name, 
                                       void* address, Int_t size,
                                       const char *file)
{ 
    return(MakeBranchInTree(tree,name,0,address,size,99,file));
}

//_____________________________________________________________________________
TBranch* AliDetector::
MakeBranchInTree(TTree *tree, const char* name, const char *classname, 
                 void* address,Int_t size, Int_t splitlevel, const char *file)
{ 
//
// Makes branch in given tree and diverts them to a separate file
// 
//
//
// if (GetDebug()>1)
 printf("* MakeBranch * Making Branch %s \n",name);
 if (tree == 0x0) return 0x0;
 TBranch *branch = tree->GetBranch(name);
 if (branch) return branch;
    
 if (classname) 
  {
    branch = tree->Branch(name,classname,address,size,splitlevel);
  } 
 else 
  {
    branch = tree->Branch(name,address,size);
  }
 return branch;
}

//_______________________________________________________________________
void AliDetector::Browse(TBrowser *b)
{
  //
  // Insert Detector objects in the list of objects to be browsed
  //
  char name[64];
  if( fHits == 0) return;
  TObject *obj;
  Int_t i, nobjects;
  //
  nobjects = fHits->GetEntries();
  for (i=0;i<nobjects;i++) {
    obj = fHits->At(i);
    sprintf(name,"%s_%d",obj->GetName(),i);
    b->Add(obj, &name[0]);
  }
}

//_______________________________________________________________________
void AliDetector::Copy(AliDetector &) const
{
  //
  // Copy *this onto det -- not implemented
  //
  Fatal("Copy","Not implemented\n");
}

//_______________________________________________________________________
void AliDetector::FinishRun()
{
  //
  // Procedure called at the end of a run.
  //
 if (GetLoader())
  {
    GetLoader()->CloseFiles();
  }
}
//_______________________________________________________________________

void AliDetector::RemapTrackReferencesIDs(Int_t *map)
{
  // 
  // Remapping track reference
  // Called at finish primary
  //
  if (!fTrackReferences) return;
  for (Int_t i=0;i<fTrackReferences->GetEntries();i++){
    AliTrackReference * ref = dynamic_cast<AliTrackReference*>(fTrackReferences->UncheckedAt(i));
    if (ref) {
      Int_t newID = map[ref->GetTrack()];
      if (newID>=0) ref->SetTrack(newID);
      else ref->SetTrack(-1);
      
    }
  }
}
//_______________________________________________________________________

AliHit* AliDetector::FirstHit(Int_t track)
{
  //
  // Initialise the hit iterator
  // Return the address of the first hit for track
  // If track>=0 the track is read from disk
  // while if track<0 the first hit of the current
  // track is returned
  // 
  if(track>=0) {
    gAlice->ResetHits(); //stupid = if N detector this method is called N times
    TreeH()->GetEvent(track); //skowron
  }
  //
  sMaxIterHit=fHits->GetEntriesFast();
  sCurIterHit=0;
  if(sMaxIterHit) return dynamic_cast<AliHit*>(fHits->UncheckedAt(0));
  else            return 0;
}


//_____________________________________________________________________________
AliTrackReference* AliDetector::FirstTrackReference(Int_t track)
{
  //
  // Initialise the hit iterator
  // Return the address of the first hit for track
  // If track>=0 the track is read from disk
  // while if track<0 the first hit of the current
  // track is returned
  // 
  if(track>=0) 
   {
     if (fLoader == 0x0)
      {
        Fatal("FirstTrackReference","Loader not initialized. Can not proceed");
      }
     AliRunLoader* rl = fLoader->GetRunLoader();
     rl->GetAliRun()->ResetTrackReferences();
     rl->TreeTR()->GetEvent(track);
   }
  //
  sMaxIterHit=fTrackReferences->GetEntriesFast();
  fCurrentIterTrackRef = 0;
  if(fMaxIterTrackRef) return dynamic_cast<AliTrackReference*>(fTrackReferences->UncheckedAt(0));
  else            return 0;
}



//_____________________________________________________________________________
AliHit* AliDetector::NextHit()
{
  //
  // Return the next hit for the current track
  //
  if(sMaxIterHit) {
    if(++sCurIterHit<sMaxIterHit) 
      return dynamic_cast<AliHit*>(fHits->UncheckedAt(sCurIterHit));
    else        
      return 0;
  } else {
    printf("* AliDetector::NextHit * Hit Iterator called without calling FistHit before\n");
    return 0;
  }
}

//_______________________________________________________________________
AliTrackReference* AliDetector::NextTrackReference()
{
  //
  // Return the next hit for the current track
  //
  if(fMaxIterTrackRef) {
    if(++fCurrentIterTrackRef<fMaxIterTrackRef) 
      return dynamic_cast<AliTrackReference*>(fTrackReferences->UncheckedAt(fCurrentIterTrackRef));
    else        
      return 0;
  } else {
    printf("* AliDetector::NextTrackReference * TrackReference  Iterator called without calling FistTrackReference before\n");
    return 0;
  }
}

//_______________________________________________________________________
void AliDetector::LoadPoints(Int_t)
{
  //
  // Store x, y, z of all hits in memory
  //
  if (fHits == 0) 
   {
    Error("LoadPoints","fHits == 0. Name is %s",GetName());
    return;
   }
  //
  Int_t nhits = fHits->GetEntriesFast();
  if (nhits == 0) 
   {
//    Error("LoadPoints","nhits == 0. Name is %s",GetName());
    return;
   }
  Int_t tracks = gAlice->GetNtrack();
  if (fPoints == 0) fPoints = new TObjArray(tracks);
  AliHit *ahit;
  //
  Int_t *ntrk=new Int_t[tracks];
  Int_t *limi=new Int_t[tracks];
  Float_t **coor=new Float_t*[tracks];
  for(Int_t i=0;i<tracks;i++) {
    ntrk[i]=0;
    coor[i]=0;
    limi[i]=0;
  }
  //
  AliPoints *points = 0;
  Float_t *fp=0;
  Int_t trk;
  Int_t chunk=nhits/4+1;
  //
  // Loop over all the hits and store their position
  for (Int_t hit=0;hit<nhits;hit++) {
    ahit = dynamic_cast<AliHit*>(fHits->UncheckedAt(hit));
    trk=ahit->GetTrack();
    assert(trk<=tracks);
    if(ntrk[trk]==limi[trk])
     {
      //
      // Initialise a new track
      fp=new Float_t[3*(limi[trk]+chunk)];
      if(coor[trk]) 
       {
          memcpy(fp,coor[trk],sizeof(Float_t)*3*limi[trk]);
          delete [] coor[trk];
       }
      limi[trk]+=chunk;
      coor[trk] = fp;
     } 
    else 
     {
      fp = coor[trk];
     }
    fp[3*ntrk[trk]  ] = ahit->X();
    fp[3*ntrk[trk]+1] = ahit->Y();
    fp[3*ntrk[trk]+2] = ahit->Z();
    ntrk[trk]++;
  }
  //
  for(trk=0; trk<tracks; ++trk) {
    if(ntrk[trk]) {
      points = new AliPoints();
      points->SetMarkerColor(GetMarkerColor());
      points->SetMarkerSize(GetMarkerSize());
      points->SetDetector(this);
      points->SetParticle(trk);
      points->SetPolyMarker(ntrk[trk],coor[trk],GetMarkerStyle());
      fPoints->AddAt(points,trk);
      delete [] coor[trk];
      coor[trk]=0;
    }
  }
  delete [] coor;
  delete [] ntrk;
  delete [] limi;
}

//_______________________________________________________________________
void AliDetector::MakeBranch(Option_t *option)
{
  //
  // Create a new branch for this detector in its treeH
  //

  Info("MakeBranch"," for %s",GetName());
  const char *cH = strstr(option,"H");

  if (fHits && TreeH() && cH) 
   {
     MakeBranchInTree(TreeH(), GetName(), &fHits, fBufferSize, 0);
   }	
}
//_____________________________________________________________________________
void AliDetector::MakeBranchTR(Option_t *option)
{
  //
  //
  TTree * tree = fLoader->GetRunLoader()->TreeTR();
  if (fTrackReferences && tree) 
   {
     MakeBranchInTree(tree, GetName(), &fTrackReferences, fBufferSize, 0) ;
   }	  
}

//_______________________________________________________________________
void AliDetector::ResetDigits()
{
  //
  // Reset number of digits and the digits array
  //
  fNdigits   = 0;
  if (fDigits) fDigits->Clear();
}

//_______________________________________________________________________
void AliDetector::ResetHits()
{
  //
  // Reset number of hits and the hits array
  //
  fNhits   = 0;
  if (fHits) fHits->Clear();
}

//_______________________________________________________________________
void AliDetector::ResetTrackReferences()
{
  //
  // Reset number of hits and the hits array
  //
  fMaxIterTrackRef   = 0;
  if (fTrackReferences)   fTrackReferences->Clear();
}

//_______________________________________________________________________
void AliDetector::ResetPoints()
{
  //
  // Reset array of points
  //
  if (fPoints) {
    fPoints->Delete();
    delete fPoints;
    fPoints = 0;
  }
}

//_______________________________________________________________________
void AliDetector::SetTreeAddress()
{
  //
  // Set branch address for the Hits and Digits Trees
  //
  TBranch *branch;
  //
  // Branch address for hit tree
  
  TTree *tree = TreeH();
  if (tree && fHits) {
    branch = tree->GetBranch(GetName());
    if (branch) 
     {
       Info("SetTreeAddress","(%s) Setting for Hits",GetName());
       branch->SetAddress(&fHits);
     }
    else
     {
       Warning("SetTreeAddress","(%s) Failed for Hits. Can not find branch in tree.",GetName());
     }
  }
  
  tree = TreeTR();
  if (tree && fTrackReferences) {
    branch = tree->GetBranch(GetName());
    if (branch) 
     {
       Info("SetTreeAddress","(%s) Setting for TrackRefs",GetName());
       branch->SetAddress(&fTrackReferences);
     }
    else
     {
       Warning("SetTreeAddress","(%s) Failed for Track Referneces. Can not find branch in tree.",GetName());
     }
  }
  
  //
  // Branch address for digit tree
  TTree *treeD = fLoader->TreeD();
  if (treeD && fDigits) {
    branch = treeD->GetBranch(GetName());
    if (branch) branch->SetAddress(&fDigits);
  }
}
/************************************************************/

void AliDetector::MakeTree(Option_t *option)
 {
 //makes a tree (container) for the data defined in option
 //"H" - hits
 //"D" - digits
 //"S" - summable digits
 //"R" - recontructed points and tracks
 
    AliLoader* loader = GetLoader();
    if (loader == 0x0)
     {
       Error("MakeTree","Can not get loader for %s",GetName());
       return;
     }
    loader->MakeTree(option); //delegate this job to getter
 }
/************************************************************/

AliLoader* AliDetector::MakeLoader(const char* topfoldername)
{ 
//builds standard getter (AliLoader type)
//if detector wants to use castomized getter, it must overload this method

 cout<<"AliDetector::MakeLoader: Creating standard getter for detector "<<GetName()
     <<". top folder is "<<topfoldername<<endl;
     
 fLoader = new AliLoader(GetName(),topfoldername);
 return fLoader;
 
}
/************************************************************/

TTree* AliDetector::TreeH()
{
//Get the hits container from the folder
  if (GetLoader() == 0x0) 
    {
    //sunstitude this with make getter when we can obtain the event folder name 
     Error("TreeH","Can not get the getter");
     return 0x0;
    }
 
  TTree* tree = (TTree*)GetLoader()->TreeH();
  return tree;
}

TTree* AliDetector::TreeTR()
{
//Get the hits container from the folder
  if (GetLoader() == 0x0) 
    {
    //sunstitude this with make getter when we can obtain the event folder name 
     Error("TreeTR","Can not get the loader");
     return 0x0;
    }
  AliRunLoader* rl = GetLoader()->GetRunLoader();
  if ( rl == 0x0)
   {
     Error("TreeTR","Can not get the run loader");
     return 0x0;
   }

  TTree* tree = rl->TreeTR();
  return tree;
}


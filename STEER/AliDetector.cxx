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
#include <iostream.h>

#include <TTree.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TROOT.h>
#include <TFolder.h>

#include "AliConfig.h"
#include "AliDetector.h"
#include "AliRun.h"
#include "AliHit.h"
#include "AliPoints.h"
#include "AliTrackReference.h"


// Static variables for the hit iterator routines
static Int_t sMaxIterHit=0;
static Int_t sCurIterHit=0;


ClassImp(AliDetector)
 
//_____________________________________________________________________________
AliDetector::AliDetector()
{
  //
  // Default constructor for the AliDetector class
  //
  fNhits      = 0;
  fNdigits    = 0;
  fPoints     = 0;
  fHits       = 0;
  fTrackReferences =0;
  fDigits     = 0;
  fTimeGate   = 200.e-9;
  fBufferSize = 16000;
  fDigitsFile = 0;
}
 
//_____________________________________________________________________________
AliDetector::AliDetector(const char* name,const char *title):AliModule(name,title)
{
  //
  // Normal constructor invoked by all Detectors.
  // Create the list for detector specific histograms
  // Add this Detector to the global list of Detectors in Run.
  //

  fTimeGate   = 200.e-9;
  fActive     = kTRUE;
  fNhits      = 0;
  fHits       = 0;
  fTrackReferences =0;
  fDigits     = 0;
  fNdigits    = 0;
  fPoints     = 0;
  fBufferSize = 16000;
  fDigitsFile = 0;

  AliConfig::Instance()->Add(this);

  fTrackReferences        = new TClonesArray("AliTrackReference", 100);
  //if detector to want to create another track reference - than let's be free 
  
}
 
//_____________________________________________________________________________
AliDetector::~AliDetector()
{
  //
  // Destructor
  //
  fNhits      = 0;
  fNdigits    = 0;
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
  if (fDigitsFile) delete [] fDigitsFile;
}

//_____________________________________________________________________________
void AliDetector::Publish(const char *dir, void *address, const char *name)
{
  //
  // Register pointer to detector objects. 
  // 
  TFolder *topFolder = (TFolder *)gROOT->FindObjectAny("/Folders");
  if  (topFolder) { 
    TFolder *folder = (TFolder *)topFolder->FindObjectAny(dir);
    // TFolder *folder = (TFolder *)gROOT->FindObjectAny(dir);
    if (!folder)  {
      cerr << "Cannot register: Missing folder: " << dir << endl;
    } else {
      TFolder *subfolder = (TFolder *) folder->FindObjectAny(this->GetName()); 

      if(!subfolder)
         subfolder = folder->AddFolder(this->GetName(),this->GetTitle());
      if (address) {
        TObject **obj = (TObject **) address;
        if ((*obj)->InheritsFrom(TCollection::Class())) {
           TCollection *collection = (TCollection *) (*obj); 
           if (name)
             collection->SetName(name);
        } 
        subfolder->Add(*obj);
      } 
    }  
  }
}

//_____________________________________________________________________________
TBranch* AliDetector::MakeBranchInTree(TTree *tree, const char* name, void* address, Int_t size,const char *file)
{ 
    return(MakeBranchInTree(tree,name,0,address,size,99,file));
}

//_____________________________________________________________________________
TBranch* AliDetector::MakeBranchInTree(TTree *tree, const char* name, const char *classname, void* address,Int_t size, Int_t splitlevel, const char *file)
{ 
    //
    // Makes branch in given tree and diverts them to a separate file
    //  
    if (GetDebug()>1)
      printf("* MakeBranch * Making Branch %s \n",name);
      
    TDirectory *cwd = gDirectory;
    TBranch *branch = 0;
    
    if (classname) {
      branch = tree->Branch(name,classname,address,size,splitlevel);
    } else {
      branch = tree->Branch(name,address,size);
    }
       
    if (file) {
        char * outFile = new char[strlen(gAlice->GetBaseFile())+strlen(file)+2];
        sprintf(outFile,"%s/%s",gAlice->GetBaseFile(),file);
        branch->SetFile(outFile);
        TIter next( branch->GetListOfBranches());
        while ((branch=(TBranch*)next())) {
           branch->SetFile(outFile);
        } 
       delete outFile;
        
       cwd->cd();
        
       if (GetDebug()>1)
           printf("* MakeBranch * Diverting Branch %s to file %s\n",name,file);
    }
    char *folder = 0;
    TString folderName(name);  
    
    if (!strncmp(tree->GetName(),"TreeE",5)) folder = "RunMC/Event/Data";
    if (!strncmp(tree->GetName(),"TreeK",5)) folder = "RunMC/Event/Data";
    if (!strncmp(tree->GetName(),"TreeH",5)) {
      folder     = "RunMC/Event/Data/Hits";
      folderName = "Hits" ; 
    }
    if (!strncmp(tree->GetName(),"TreeTrackReferences",5)) {
      folder     = "RunMC/Event/Data/TrackReferences";
      folderName = "TrackReferences" ; 
    }

    if (!strncmp(tree->GetName(),"TreeD",5)) {
      folder     = "Run/Event/Data";
      folderName = "Digits" ; 
    }
    if (!strncmp(tree->GetName(),"TreeS",5)) {
      folder     = "RunMC/Event/Data/SDigits";
      folderName = "SDigits" ; 
    }
    if (!strncmp(tree->GetName(),"TreeR",5)) folder = "Run/Event/RecData";

    if (folder) {
      if (GetDebug())
          printf("%15s: Publishing %s to %s\n",ClassName(),name,folder);
      Publish(folder,address, folderName.Data());
    }  
    return branch;
}

//_____________________________________________________________________________
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

//_____________________________________________________________________________
void AliDetector::Copy(AliDetector &det) const
{
  //
  // Copy *this onto det -- not implemented
  //
  Fatal("Copy","Not implemented~\n");
}

//_____________________________________________________________________________
void AliDetector::FinishRun()
{
  //
  // Procedure called at the end of a run.
  //
}



void AliDetector::RemapTrackReferencesIDs(Int_t *map)
{
  // 
  //remaping track reference
  //called at finish primary
  if (!fTrackReferences) return;
  for (Int_t i=0;i<fTrackReferences->GetEntries();i++){
    AliTrackReference * ref = (AliTrackReference*) fTrackReferences->UncheckedAt(i);
    if (ref) {
      Int_t newID = map[ref->GetTrack()];
      if (newID>=0) ref->SetTrack(newID);
      else ref->SetTrack(-1);
      
    }
  }
}

//_____________________________________________________________________________
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
    gAlice->ResetHits();
    gAlice->TreeH()->GetEvent(track);
  }
  //
  sMaxIterHit=fHits->GetEntriesFast();
  sCurIterHit=0;
  if(sMaxIterHit) return (AliHit*) fHits->UncheckedAt(0);
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
  if(track>=0) {
    gAlice->ResetTrackReferences();
    gAlice->TreeTR()->GetEvent(track);
  }
  //
  fMaxIterTrackRef     = fTrackReferences->GetEntriesFast();
  fCurrentIterTrackRef = 0;
  if(fMaxIterTrackRef) return (AliTrackReference*) fTrackReferences->UncheckedAt(0);
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
      return (AliHit*) fHits->UncheckedAt(sCurIterHit);
    else        
      return 0;
  } else {
    printf("* AliDetector::NextHit * Hit Iterator called without calling FistHit before\n");
    return 0;
  }
}
//_____________________________________________________________________________
AliTrackReference* AliDetector::NextTrackReference()
{
  //
  // Return the next hit for the current track
  //
  if(fMaxIterTrackRef) {
    if(++fCurrentIterTrackRef<fMaxIterTrackRef) 
      return (AliTrackReference*) fTrackReferences->UncheckedAt(fCurrentIterTrackRef);
    else        
      return 0;
  } else {
    printf("* AliDetector::NextTrackReference * TrackReference  Iterator called without calling FistTrackReference before\n");
    return 0;
  }
}

//_____________________________________________________________________________
void AliDetector::LoadPoints(Int_t)
{
  //
  // Store x, y, z of all hits in memory
  //
  if (fHits == 0) return;
  //
  Int_t nhits = fHits->GetEntriesFast();
  if (nhits == 0) return;
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
    ahit = (AliHit*)fHits->UncheckedAt(hit);
    trk=ahit->GetTrack();
    assert(trk<=tracks);
    if(ntrk[trk]==limi[trk]) {
      //
      // Initialise a new track
      fp=new Float_t[3*(limi[trk]+chunk)];
      if(coor[trk]) {
	memcpy(fp,coor[trk],sizeof(Float_t)*3*limi[trk]);
	delete [] coor[trk];
      }
      limi[trk]+=chunk;
      coor[trk] = fp;
    } else {
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

//_____________________________________________________________________________
void AliDetector::MakeBranch(Option_t *option, const char *file)
{
  //
  // Create a new branch in the current Root Tree
  // The branch of fHits is automatically split
  //
 
  char branchname[10];
  sprintf(branchname,"%s",GetName());
  //
  // Get the pointer to the header
  const char *cH = strstr(option,"H");
  //
  if (fHits && gAlice->TreeH() && cH) {
    MakeBranchInTree(gAlice->TreeH(), 
                     branchname, &fHits, fBufferSize, file) ;              
  }	
  
  const char *cD = strstr(option,"D");

  if (cD) {
    if (file) {
       fDigitsFile = new char[strlen (file)];
       strcpy(fDigitsFile,file);
    }
  }
}
//_____________________________________________________________________________
void AliDetector::MakeBranchTR(Option_t *option, const char *file)
{
  //
  // Create a new branch in the current Root Tree
  // The branch of fHits is automatically split
  //
 
  char branchname[10];
  sprintf(branchname,"%s",GetName());
  //
  // Get the pointer to the header
  const char *cTR = strstr(option,"T");
  //
  if (fTrackReferences && gAlice->TreeTR() && cTR) {
    MakeBranchInTree(gAlice->TreeTR(), 
                     branchname, &fTrackReferences, fBufferSize, file) ;              
  }	  
}

//_____________________________________________________________________________
void AliDetector::ResetDigits()
{
  //
  // Reset number of digits and the digits array
  //
  fNdigits   = 0;
  if (fDigits)   fDigits->Clear();
}

//_____________________________________________________________________________
void AliDetector::ResetHits()
{
  //
  // Reset number of hits and the hits array
  //
  fNhits   = 0;
  if (fHits)   fHits->Clear();
}



//_____________________________________________________________________________
void AliDetector::ResetTrackReferences()
{
  //
  // Reset number of hits and the hits array
  //
  fMaxIterTrackRef   = 0;
  if (fTrackReferences)   fTrackReferences->Clear();
}



//_____________________________________________________________________________
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

//_____________________________________________________________________________
void AliDetector::SetTreeAddress()
{
  //
  // Set branch address for the Hits and Digits Trees
  //
  TBranch *branch;
  char branchname[20];
  sprintf(branchname,"%s",GetName());
  //
  // Branch address for hit tree
  TTree *treeH = gAlice->TreeH();
  if (treeH && fHits) {
    branch = treeH->GetBranch(branchname);
    if (branch) branch->SetAddress(&fHits);
  }
  //
  // Branch address for digit tree
  TTree *treeD = gAlice->TreeD();
  if (treeD && fDigits) {
    branch = treeD->GetBranch(branchname);
    if (branch) branch->SetAddress(&fDigits);
  }

  // Branch address for tr  tree
  TTree *treeTR = gAlice->TreeTR();
  if (treeTR && fTrackReferences) {
    branch = treeTR->GetBranch(branchname);
    if (branch) branch->SetAddress(&fTrackReferences);
  }


}

 

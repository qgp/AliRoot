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
Revision 1.19.4.1  2002/06/10 14:43:06  hristov
Merged with v3-08-02

Revision 1.21  2002/05/28 14:24:57  hristov
Correct warning messages

Revision 1.20  2002/04/30 11:47:30  morsch
KeepPhysics method called by PurifyKine added (N. Carrer, A.M.)

Revision 1.19  2002/03/12 11:06:03  morsch
Add particle status code to argument list of SetTrack(..).

Revision 1.18  2002/02/20 16:14:41  hristov
fParticleBuffer points to object which doesn't belong to AliStack, do not delete it (J.Chudoba)

Revision 1.17  2001/12/05 08:51:56  hristov
The default constructor now creates no objects (thanks to r.Brun). Some corrections required by the previous changes.

Revision 1.16  2001/11/20 09:27:55  hristov
Possibility to investigate a primary of not yet loaded particle (I.Hrivnacova)

Revision 1.15  2001/09/04 15:10:37  hristov
Additional protection is included to avoid some problems using Hijing

Revision 1.14  2001/08/30 09:44:06  hristov
VertexSource_t added to avoid the warnings

Revision 1.13  2001/08/29 13:31:42  morsch
Protection against (fTreeK == 0) in destructor.

Revision 1.12  2001/07/27 13:03:13  hristov
Default Branch split level set to 99

Revision 1.11  2001/07/27 12:34:20  jchudoba
remove the dummy argument in GetEvent method

Revision 1.10  2001/07/20 10:13:54  morsch
In Particle(Int_t) use GetEntriesFast to speed up the procedure.

Revision 1.9  2001/07/03 08:10:57  hristov
J.Chudoba's changes merged correctly with the HEAD

Revision 1.6  2001/05/31 06:59:06  fca
Clean setting and deleting of fParticleBuffer

Revision 1.5  2001/05/30 12:18:46  hristov
Loop variables declared once

Revision 1.4  2001/05/25 07:25:20  hristov
AliStack destructor corrected (I.Hrivnacova)

Revision 1.3  2001/05/22 14:33:16  hristov
Minor changes

Revision 1.2  2001/05/17 05:49:39  fca
Reset pointers to daughters

Revision 1.1  2001/05/16 14:57:22  alibrary
New files for folders and Stack

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Particles stack class
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
 
#include <TObjArray.h>
#include <TParticle.h>
#include <TTree.h>
#include <TFile.h>
#include <TFolder.h>
#include <TROOT.h>

#include "AliStack.h"
#include "AliRun.h"
#include "AliModule.h"
#include "AliHit.h"

ClassImp(AliStack)

//_____________________________________________________________________________
AliStack::AliStack(Int_t size)
  : TVirtualMCStack()
{
  //
  //  Constructor
  //
  
  // Create the particles arrays 
  fParticles      = new TClonesArray("TParticle",1000);
  fParticleMap    = new TObjArray(size);
  fParticleBuffer = 0;
  fNtrack         = 0;
  fNprimary       = 0;
  fCurrent        = -1;
  fCurrentPrimary = -1;
  fTreeK          = 0;
}


//_____________________________________________________________________________
AliStack::AliStack()
  : TVirtualMCStack()
{
  //
  //  Default constructor
  //
  
  // Create the particles arrays 
  fParticles      = 0;
  fParticleMap    = 0;
  fParticleBuffer = 0;
  fNtrack         = 0;
  fCurrent        = -1;
  fNprimary       = 0;
  fCurrentPrimary = -1;
  fTreeK          = 0;
}


//_____________________________________________________________________________
AliStack::~AliStack()
{
  //
  // Destructor
  //
  
  if (fParticles) {
    fParticles->Delete();
    delete fParticles;
  }
  delete fParticleMap;
  if (fTreeK) delete fTreeK;
}

//
// public methods
//

//_____________________________________________________________________________
void AliStack::SetTrack(Int_t done, Int_t parent, Int_t pdg, Float_t *pmom,
		      Float_t *vpos, Float_t *polar, Float_t tof,
		      AliMCProcess mech, Int_t &ntr, Float_t weight, Int_t is)
{ 
  //
  // Load a track on the stack
  //
  // done     0 if the track has to be transported
  //          1 if not
  // parent   identifier of the parent track. -1 for a primary
  // pdg    particle code
  // pmom     momentum GeV/c
  // vpos     position 
  // polar    polarisation 
  // tof      time of flight in seconds
  // mecha    production mechanism
  // ntr      on output the number of the track stored
  //

  //  const Float_t tlife=0;
  
  //
  // Here we get the static mass
  // For MC is ok, but a more sophisticated method could be necessary
  // if the calculated mass is required
  // also, this method is potentially dangerous if the mass
  // used in the MC is not the same of the PDG database
  //
  Float_t mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();
  Float_t e=TMath::Sqrt(mass*mass+pmom[0]*pmom[0]+
			pmom[1]*pmom[1]+pmom[2]*pmom[2]);
  
//    printf("Loading  mass %f ene %f No %d ip %d parent %d done %d pos %f %f %f mom %f %f %f kS %d m \n",
//	   mass,e,fNtrack,pdg,parent,done,vpos[0],vpos[1],vpos[2],pmom[0],pmom[1],pmom[2],kS);
  

  SetTrack(done, parent, pdg, pmom[0], pmom[1], pmom[2], e,
           vpos[0], vpos[1], vpos[2], tof, polar[0], polar[1], polar[2],
           mech, ntr, weight, is);
}

//_____________________________________________________________________________
void AliStack::SetTrack(Int_t done, Int_t parent, Int_t pdg,
  	              Double_t px, Double_t py, Double_t pz, Double_t e,
  		      Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		      Double_t polx, Double_t poly, Double_t polz,
		      AliMCProcess mech, Int_t &ntr, Double_t weight, Int_t is)
{ 
  //
  // Load a track on the stack
  //
  // done        0 if the track has to be transported
  //             1 if not
  // parent      identifier of the parent track. -1 for a primary
  // pdg         particle code
  // kS          generation status code
  // px, py, pz  momentum GeV/c
  // vx, vy, vz  position 
  // polar       polarisation 
  // tof         time of flight in seconds
  // mech        production mechanism
  // ntr         on output the number of the track stored
  //    
  // New method interface: 
  // arguments were changed to be in correspondence with TParticle
  // constructor.
  // Note: the energy is not calculated from the static mass but
  // it is passed by argument e.


  const Int_t kFirstDaughter=-1;
  const Int_t kLastDaughter=-1;
  
  TClonesArray &particles = *fParticles;
  TParticle* particle
    = new(particles[fLoadPoint++]) 
      TParticle(pdg, is, parent, -1, kFirstDaughter, kLastDaughter,
		px, py, pz, e, vx, vy, vz, tof);
   
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(mech);

  if(!done) particle->SetBit(kDoneBit);

  //  Declare that the daughter information is valid
  particle->SetBit(kDaughtersBit);
  //  Add the particle to the stack
  fParticleMap->AddAtAndExpand(particle, fNtrack);//CHECK!!

  if(parent>=0) {
    particle = (TParticle*) fParticleMap->At(parent);
    if (particle) {
      particle->SetLastDaughter(fNtrack);
      if(particle->GetFirstDaughter()<0) particle->SetFirstDaughter(fNtrack);
    }
    else {
      printf("Error in AliStack::SetTrack: Parent %d does not exist\n",parent);
    }
  } 
  else { 
    //
    // This is a primary track. Set high water mark for this event
    fHgwmk = fNtrack;
    //
    // Set also number if primary tracks
    fNprimary = fHgwmk+1;
    fCurrentPrimary++;
  }
  ntr = fNtrack++;
}

//_____________________________________________________________________________
TParticle*  AliStack::GetNextTrack(Int_t& itrack)
{
  //
  // Returns next track from stack of particles
  //
  

  TParticle* track = GetNextParticle();

  if (track) {
    itrack = fCurrent;
    track->SetBit(kDoneBit);
  }
  else 
    itrack = -1;

  return track;
}

/*
//_____________________________________________________________________________
void  AliStack::GetNextTrack(Int_t& itrack, Int_t& pdg,     
  	                     Double_t& px, Double_t& py, Double_t& pz, Double_t& e,
  		             Double_t& vx, Double_t& vy, Double_t& vz, Double_t& tof,
		             Double_t& polx, Double_t& poly, Double_t& polz) 
{
  //
  // Return next track from stack of particles
  //
  

  TParticle* track = GetNextParticle();
//    cout << "GetNextTrack():" << fCurrent << fNprimary << endl;

  if (track) {
    itrack = fCurrent;
    pdg = track->GetPdgCode();
    px = track->Px();
    py = track->Py(); 
    pz = track->Pz();
    e  = track->Energy();
    vx = track->Vx();
    vy = track->Vy();
    vz = track->Vz();
    tof = track->T();
    TVector3 pol;
    track->GetPolarisation(pol);
    polx = pol.X();
    poly = pol.Y();
    polz = pol.Z();
    track->SetBit(kDoneBit);
//      cout << "Filled params" << endl;
  }
  else 
    itrack = -1;

  //
  // stop and start timer when we start a primary track
  Int_t nprimaries = fNprimary;
  if (fCurrent >= nprimaries) return;
  if (fCurrent < nprimaries-1) { 
    fTimer.Stop();
    track=(TParticle*) fParticleMap->At(fCurrent+1);
    //    track->SetProcessTime(fTimer.CpuTime());
  }
  fTimer.Start();
}

*/
//_____________________________________________________________________________
TParticle*  AliStack::GetPrimaryForTracking(Int_t i)
{
  //
  // Returns i-th primary particle if it is flagged to be tracked,
  // 0 otherwise
  //
  
  TParticle* particle = Particle(i);
  
  if (!particle->TestBit(kDoneBit))
    return particle;
  else
    return 0;
}      


//_____________________________________________________________________________
void AliStack::PurifyKine()
{
  //
  // Compress kinematic tree keeping only flagged particles
  // and renaming the particle id's in all the hits
  //

  TObjArray &particles = *fParticleMap;
  int nkeep=fHgwmk+1, parent, i;
  TParticle *part, *father;
  TArrayI map(particles.GetLast()+1);

  // Save in Header total number of tracks before compression

  // If no tracks generated return now
  if(fHgwmk+1 == fNtrack) return;

  // First pass, invalid Daughter information
  for(i=0; i<fNtrack; i++) {
    // Preset map, to be removed later
    if(i<=fHgwmk) map[i]=i ; 
    else {
      map[i] = -99;
      if((part=(TParticle*) particles.At(i))) {
//
//        Check of this track should be kept for physics reasons 
	  if (KeepPhysics(part)) KeepTrack(i);
//
          part->ResetBit(kDaughtersBit);
          part->SetFirstDaughter(-1);
          part->SetLastDaughter(-1);
      }
    }
  }
  // Invalid daughter information for the parent of the first particle
  // generated. This may or may not be the current primary according to
  // whether decays have been recorded among the primaries
  part = (TParticle *)particles.At(fHgwmk+1);
  particles.At(part->GetFirstMother())->ResetBit(kDaughtersBit);
  // Second pass, build map between old and new numbering
  for(i=fHgwmk+1; i<fNtrack; i++) {
    if(particles.At(i)->TestBit(kKeepBit)) {
      
      // This particle has to be kept
      map[i]=nkeep;
      // If old and new are different, have to move the pointer
      if(i!=nkeep) particles[nkeep]=particles.At(i);
      part = (TParticle*) particles.At(nkeep);
      
      // as the parent is always *before*, it must be already
      // in place. This is what we are checking anyway!
      if((parent=part->GetFirstMother())>fHgwmk) 
	if(map[parent]==-99) Fatal("PurifyKine","map[%d] = -99!\n",parent);
	else part->SetFirstMother(map[parent]);

      nkeep++;
    }
  }
  
  // Fix daughters information
  for (i=fHgwmk+1; i<nkeep; i++) {
    part = (TParticle *)particles.At(i);
    parent = part->GetFirstMother();
    if(parent>=0) {
      father = (TParticle *)particles.At(parent);
      if(father->TestBit(kDaughtersBit)) {
      
	if(i<father->GetFirstDaughter()) father->SetFirstDaughter(i);
	if(i>father->GetLastDaughter())  father->SetLastDaughter(i);
      } else {
	// Initialise daughters info for first pass
	father->SetFirstDaughter(i);
	father->SetLastDaughter(i);
	father->SetBit(kDaughtersBit);
      }
    }
  }
  
  // Now loop on all registered hit lists
  TList* hitLists = gAlice->GetHitLists();
  TIter next(hitLists);
  TCollection *hitList;
  while((hitList = (TCollection*)next())) {
    TIter nexthit(hitList);
    AliHit *hit;
    while((hit = (AliHit*)nexthit())) {
      hit->SetTrack(map[hit->GetTrack()]);
    }
  }

  // 
  // This for detectors which have a special mapping mechanism
  // for hits, such as TPC and TRD
  //

   TObjArray* modules = gAlice->Modules();
   TIter nextmod(modules);
   AliModule *detector;
   while((detector = (AliModule*)nextmod())) {
     detector->RemapTrackHitIDs(map.GetArray());
     detector->RemapTrackReferencesIDs(map.GetArray());
   }
  
   // Now the output bit, from fHgwmk to nkeep we write everything and we erase
   if(nkeep>fParticleFileMap.GetSize()) fParticleFileMap.Set(Int_t (nkeep*1.5));

   for (i=fHgwmk+1; i<nkeep; ++i) {
     fParticleBuffer = (TParticle*) particles.At(i);
     fParticleFileMap[i]=(Int_t) fTreeK->GetEntries();
     fTreeK->Fill();
     particles[i]=fParticleBuffer=0;
   }

   for (i=nkeep; i<fNtrack; ++i) particles[i]=0;

   Int_t toshrink = fNtrack-fHgwmk-1;
   fLoadPoint-=toshrink;
   for(i=fLoadPoint; i<fLoadPoint+toshrink; ++i) fParticles->RemoveAt(i);

   fNtrack=nkeep;
   fHgwmk=nkeep-1;
   //   delete [] map;
}

Bool_t AliStack::KeepPhysics(TParticle* part)
{
    //
    // Some particles have to kept on the stack for reasons motivated
    // by physics analysis. Decision is put here.
    //
    Bool_t keep = kFALSE;
    //
    // Keep first-generation daughter from primaries with heavy flavor 
    //
    Int_t parent = part->GetFirstMother();
    if (parent >= 0 && parent <= fHgwmk) {
	TParticle* father = (TParticle*) Particles()->At(parent);
	Int_t kf = father->GetPdgCode();
	kf = TMath::Abs(kf);
	Int_t kfl = kf;
	// meson ?
	if  (kfl > 10) kfl/=100;
	// baryon
	if (kfl > 10) kfl/=10;
	if (kfl > 10) kfl/=10;
	if (kfl >= 4) {
	    keep = kTRUE;
	}
    }
    return keep;
}

//_____________________________________________________________________________
void AliStack::FinishEvent()
{
  //
  // Write out the kinematics that was not yet filled
  //
  
  // Update event header


  if (!fTreeK) {
//    Fatal("FinishEvent", "No kinematics tree is defined.");
//    Don't panic this is a probably a lego run
      return;
      
  }  
  
  CleanParents();
  if(fTreeK->GetEntries() ==0) {
    // set the fParticleFileMap size for the first time
    fParticleFileMap.Set(fHgwmk+1);
  }

  Bool_t allFilled = kFALSE;
  TObject *part;
  for(Int_t i=0; i<fHgwmk+1; ++i) 
    if((part=fParticleMap->At(i))) {
      fParticleBuffer = (TParticle*) part;
      fParticleFileMap[i]= (Int_t) fTreeK->GetEntries();
      fTreeK->Fill();
      //PH      (*fParticleMap)[i]=fParticleBuffer=0;      
      fParticleBuffer=0;      
      fParticleMap->AddAt(0,i);      
      
      // When all primaries were filled no particle!=0
      // should be left => to be removed later.
      if (allFilled) printf("Why != 0 part # %d?\n",i);
    }
    else {
      // // printf("Why = 0 part # %d?\n",i); => We know.
      // break;
         // we don't break now in order to be sure there is no
	 // particle !=0 left.
	 // To be removed later and replaced with break.
      if(!allFilled) allFilled = kTRUE;
    } 
//    cout << "Nof particles: " << fNtrack << endl;
  //Reset();   
} 

//_____________________________________________________________________________
void AliStack::FlagTrack(Int_t track)
{
  //
  // Flags a track and all its family tree to be kept
  //
  
  TParticle *particle;

  Int_t curr=track;
  while(1) {
    particle=(TParticle*)fParticleMap->At(curr);
    
    // If the particle is flagged the three from here upward is saved already
    if(particle->TestBit(kKeepBit)) return;
    
    // Save this particle
    particle->SetBit(kKeepBit);
    
    // Move to father if any
    if((curr=particle->GetFirstMother())==-1) return;
  }
}
 
//_____________________________________________________________________________
void AliStack::KeepTrack(Int_t track)
{ 
  //
  // Flags a track to be kept
  //
  
  fParticleMap->At(track)->SetBit(kKeepBit);
}

//_____________________________________________________________________________
void  AliStack::Reset(Int_t size) 
{
  //
  // Resets stack
  //

  fNtrack=0;
  fNprimary=0;
  fHgwmk=0;
  fLoadPoint=0;
  fCurrent = -1;
  ResetArrays(size);
}

//_____________________________________________________________________________
void  AliStack::ResetArrays(Int_t size) 
{
  //
  // Resets stack arrays
  //

  if (fParticles) 
    fParticles->Clear();
  else
    fParticles = new TClonesArray("TParticle",1000);
  if (fParticleMap) {
    fParticleMap->Clear();
    if (size>0) fParticleMap->Expand(size);}
  else
    fParticleMap = new TObjArray(size);
}

//_____________________________________________________________________________
void AliStack::SetHighWaterMark(Int_t nt)
{
  //
  // Set high water mark for last track in event
  //
  
  fHgwmk = fNtrack-1;
  fCurrentPrimary=fHgwmk;
  
  // Set also number of primary tracks
  fNprimary = fHgwmk+1;
  fNtrack   = fHgwmk+1;      
}

//_____________________________________________________________________________
TParticle* AliStack::Particle(Int_t i)
{
  //
  // Return particle with specified ID
  
  //PH  if(!(*fParticleMap)[i]) {
  if(!fParticleMap->At(i)) {
    Int_t nentries = fParticles->GetEntriesFast();
    // algorithmic way of getting entry index
    // (primary particles are filled after secondaries)
    Int_t entry;
    if (i<fNprimary)
	entry = i+fNtrack-fNprimary;
    else 
	entry = i-fNprimary;
    // check whether algorithmic way and 
    // and the fParticleFileMap[i] give the same;
    // give the fatal error if not
    if (entry != fParticleFileMap[i]) {
      Fatal("Particle",
        "!! The algorithmic way and map are different: !!\n entry: %d map: %d",
	entry, fParticleFileMap[i]); 
    } 
      
    fTreeK->GetEntry(entry);
    new ((*fParticles)[nentries]) TParticle(*fParticleBuffer);
    fParticleMap->AddAt((*fParticles)[nentries],i);
  }
  //PH  return (TParticle *) (*fParticleMap)[i];
  return (TParticle *) fParticleMap->At(i);
}

//_____________________________________________________________________________
Int_t AliStack::GetPrimary(Int_t id)
{
  //
  // Return number of primary that has generated track
  //
  
  int current, parent;
  //
  parent=id;
  while (1) {
    current=parent;
    parent=Particle(current)->GetFirstMother();
    if(parent<0) return current;
  }
}
 
//_____________________________________________________________________________
void AliStack::DumpPart (Int_t i) const
{
  //
  // Dumps particle i in the stack
  //
  
  //PH  ((TParticle*) (*fParticleMap)[i])->Print();
  ((TParticle*) fParticleMap->At(i))->Print();
}

//_____________________________________________________________________________
void AliStack::DumpPStack ()
{
  //
  // Dumps the particle stack
  //

  Int_t i;

  printf(
	 "\n\n=======================================================================\n");
  for (i=0;i<fNtrack;i++) 
    {
      TParticle* particle = Particle(i);
      if (particle) {
        printf("-> %d ",i); particle->Print();
        printf("--------------------------------------------------------------\n");
      }
      else 
        Warning("DumpPStack", "No particle with id %d.", i); 
    }	 

  printf(
	 "\n=======================================================================\n\n");
  
  // print  particle file map
  printf("\nParticle file map: \n");
  for (i=0; i<fNtrack; i++) 
      printf("   %d th entry: %d \n",i,fParticleFileMap[i]);
}


//_____________________________________________________________________________
void AliStack::DumpLoadedStack() const
{
  //
  // Dumps the particle in the stack
  // that are loaded in memory.
  //

  TObjArray &particles = *fParticleMap;
  printf(
	 "\n\n=======================================================================\n");
  for (Int_t i=0;i<fNtrack;i++) 
    {
      TParticle* particle = (TParticle*) particles[i];
      if (particle) {
        printf("-> %d ",i); particle->Print();
        printf("--------------------------------------------------------------\n");
      }
      else { 	
        printf("-> %d  Particle not loaded.\n",i);
        printf("--------------------------------------------------------------\n");
      }	
    }
  printf(
	 "\n=======================================================================\n\n");
}

//
// protected methods
//

//_____________________________________________________________________________
void AliStack::CleanParents()
{
  //
  // Clean particles stack
  // Set parent/daughter relations
  //
  
  TObjArray &particles = *fParticleMap;
  TParticle *part;
  int i;
  for(i=0; i<fHgwmk+1; i++) {
    part = (TParticle *)particles.At(i);
    if(part) if(!part->TestBit(kDaughtersBit)) {
      part->SetFirstDaughter(-1);
      part->SetLastDaughter(-1);
    }
  }
}

//_____________________________________________________________________________
TParticle* AliStack::GetNextParticle()
{
  //
  // Return next particle from stack of particles
  //
  
  TParticle* particle = 0;
  
  // search secondaries
  //for(Int_t i=fNtrack-1; i>=0; i--) {
  for(Int_t i=fNtrack-1; i>fHgwmk; i--) {
      particle = (TParticle*) fParticleMap->At(i);
      if ((particle) && (!particle->TestBit(kDoneBit))) {
	  fCurrent=i;    
	  //cout << "GetNextParticle() - secondary " 
	  // << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
	  return particle;
      }   
  }    

  // take next primary if all secondaries were done
  while (fCurrentPrimary>=0) {
      fCurrent = fCurrentPrimary;    
      particle = (TParticle*) fParticleMap->At(fCurrentPrimary--);
      if ((particle) && (!particle->TestBit(kDoneBit))) {
	  //cout << "GetNextParticle() - primary " 
	  //   << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
	  return particle;
      } 
  }
  
  // nothing to be tracked
  fCurrent = -1;
    //cout << "GetNextParticle() - none  " 
    //   << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
  return particle;  
}

//__________________________________________________________________________________________
void AliStack::MakeTree(Int_t event, const char *file)
{
//
//  Make Kine tree and creates branch for writing particles
//  
  TBranch *branch=0;
  // Make Kinematics Tree
  char hname[30];
  if (!fTreeK) {
    sprintf(hname,"TreeK%d",event);
    fTreeK = new TTree(hname,"Kinematics");
    //  Create a branch for particles
    branch = fTreeK->Branch("Particles", "TParticle", &fParticleBuffer, 4000);          
    fTreeK->Write(0,TObject::kOverwrite);
  }
}

//_____________________________________________________________________________
void AliStack::BeginEvent(Int_t event)
{
// start a new event
//
//
    fNprimary = 0;
    fNtrack   = 0;
    
    char hname[30];
    if(fTreeK) {
	fTreeK->Reset();
	sprintf(hname,"TreeK%d",event);
	fTreeK->SetName(hname);
    }
}

//_____________________________________________________________________________
void AliStack::FinishRun()
{
// Clean TreeK information
    if (fTreeK) {
	delete fTreeK; fTreeK = 0;
    }
}

Bool_t AliStack::GetEvent(Int_t event)
{
//
// Get new event from TreeK

    // Reset/Create the particle stack
    if (fTreeK) delete fTreeK;
    
    // Get Kine Tree from file
    char treeName[20];
    sprintf(treeName,"TreeK%d",event);
    fTreeK = (TTree*)gDirectory->Get(treeName);

    if (fTreeK) 
      fTreeK->SetBranchAddress("Particles", &fParticleBuffer);
    else {
      //      Error("GetEvent","cannot find Kine Tree for event:%d\n",event);
      Warning("GetEvent","cannot find Kine Tree for event:%d\n",event);
      return kFALSE;
    }
//      printf("\n primaries %d", fNprimary);
//      printf("\n tracks    %d", fNtrack);    
      
    Int_t size = (Int_t)fTreeK->GetEntries();
    ResetArrays(size);
    return kTRUE;
}

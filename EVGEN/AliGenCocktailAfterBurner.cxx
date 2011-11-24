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

// 
// Container class for AliGenerator and AfterBurners 
// (which are AliGenerators as well) through recursion.
// The container is itself an AliGenerator a
// what is stored are not the pointers to the generators directly 
// but to objects of type
// AliGenCocktailAfterBurner entry.   
// The class provides also iterator functionality.  
// Author: andreas.morsch@cern.ch and piotr.skowronski@cern.ch
//
// 24.09.2001  Piotr Skowronski
//             debug -> gDebug,
//             fNEvents replaced with AliRunLoader::GetNumberOfEvents()
//


#include <Riostream.h>

#include <TList.h>
#include <TObjArray.h>
#include <TParticle.h>

#include "AliGenCocktailAfterBurner.h"
#include "AliGenCocktailEntry.h"
#include "AliGenCocktailEventHeader.h"
#include "AliCollisionGeometry.h"
#include "AliStack.h"
#include "AliMC.h"
#include "AliRun.h"


ClassImp(AliGenCocktailAfterBurner)
/*********************************************************************/ 
/*********************************************************************/ 

    AliGenCocktailAfterBurner::AliGenCocktailAfterBurner():
	fNAfterBurners(0),
	fAfterBurnerEntries(0),
	fGenerationDone(kFALSE),
	fInternalStacks(0),
	fCollisionGeometries(0),
	fHeaders(0),
	fCurrentEvent(0),
	fActiveStack(0),
	fActiveEvent(-1),
	fCurrentGenerator(0),
	fNBgEvents(0)
{
// Constructor
    if (gDebug > 0) 
	cout<<"AliGenCocktailAfterBurner::AliGenCocktailAfterBurner()"<<endl;
    SetName("AliGenCocktailAfterBurner");
    SetTitle("AliGenCocktailAfterBurner");
}

/*********************************************************************/ 

AliGenCocktailAfterBurner::~AliGenCocktailAfterBurner()
  {
//destructor

    if (fInternalStacks) //delete stacks
     { 
       fInternalStacks->SetOwner();
       delete fInternalStacks;
    }
    if (fAfterBurnerEntries) delete fAfterBurnerEntries; //delete entries
    Int_t numberOfEvents = AliRunLoader::Instance()->GetNumberOfEventsPerRun();
    if (fCollisionGeometries) {
      for (Int_t i = 0; i < (numberOfEvents + fNBgEvents); i++)
	if (fCollisionGeometries[i]) delete fCollisionGeometries[i];
      delete[] fCollisionGeometries;
    }
    if (fHeaders) {
      for (Int_t i = 0; i < (numberOfEvents + fNBgEvents); i++)
	if (fHeaders[i]) delete fHeaders[i];
      delete[] fHeaders;
    }
  }
/*********************************************************************/ 
/*********************************************************************/ 

void AliGenCocktailAfterBurner::
AddAfterBurner(AliGenerator *AfterBurner, char* Name, Float_t RateExp)
{
//
//  Forward parameters to the new AfterBurner
    
    if (gDebug>0)cout<<"AliGenCocktailAfterBurner::AddAfterBurner  Named "<<Name<<endl;

    if(TestBit(kPtRange) && !(AfterBurner->TestBit(kPtRange)) && !(AfterBurner->TestBit(kMomentumRange))) 
	AfterBurner->SetPtRange(fPtMin,fPtMax);
    if(TestBit(kMomentumRange) && !(AfterBurner->TestBit(kPtRange)) && !(AfterBurner->TestBit(kMomentumRange)))
	AfterBurner->SetMomentumRange(fPMin,fPMax);
    
    if (TestBit(kYRange) && !(AfterBurner->TestBit(kYRange)))
	AfterBurner->SetYRange(fYMin,fYMax);
    if (TestBit(kPhiRange) && !(AfterBurner->TestBit(kPhiRange)))
	AfterBurner->SetPhiRange(fPhiMin*180/TMath::Pi(),fPhiMax*180/TMath::Pi());
    if (TestBit(kThetaRange) && !(AfterBurner->TestBit(kThetaRange)) && !(AfterBurner->TestBit(kEtaRange)))
	AfterBurner->SetThetaRange(fThetaMin*180/TMath::Pi(),fThetaMax*180/TMath::Pi());
    if (TestBit(kVertexRange) && !(AfterBurner->TestBit(kVertexRange))) {
	AfterBurner->SetOrigin(fOrigin[0], fOrigin[1], fOrigin[2]);
	AfterBurner->SetSigma(fOsigma[0], fOsigma[1], fOsigma[2]);
	AfterBurner->SetVertexSmear(fVertexSmear);
	AfterBurner->SetVertexSource(kContainer);
	AfterBurner->SetTimeOrigin(fTimeOrigin);
    }
    AfterBurner->SetTrackingFlag(fTrackIt);
    //AfterBurner->SetContainer(this);

//
//  Add AfterBurner to list   
    
    AliGenCocktailEntry *entry = 
	new AliGenCocktailEntry(AfterBurner, Name, RateExp);
    if (!fAfterBurnerEntries) fAfterBurnerEntries = new TList();
    
    fAfterBurnerEntries->Add(entry);
    fNAfterBurners++;
//
    
}
/*********************************************************************/ 
/*********************************************************************/ 

void AliGenCocktailAfterBurner::Init()
{
// Initialisation
  Int_t numberOfEvents = AliRunLoader::Instance()->GetNumberOfEventsPerRun();
    fGenerationDone = kFALSE;
    if (fInternalStacks) //delete stacks
     { 
       fInternalStacks->SetOwner();
       fInternalStacks->Delete(); //clean after previous generation cycle
     }

    if (fCollisionGeometries) {
      for (Int_t i = 0; i < (numberOfEvents + fNBgEvents); i++)
	if (fCollisionGeometries[i]) delete fCollisionGeometries[i];
      delete[] fCollisionGeometries;
    }
    if (fHeaders) {
      for (Int_t i = 0; i < (numberOfEvents + fNBgEvents); i++)
	if (fHeaders[i]) delete fHeaders[i];
      delete[] fHeaders;
    }

    this->AliGenCocktail::Init(); 
    
    if (gDebug>0) cout<<"AliGenCocktailAfterBurner::Init"<<endl;
    TIter next(fAfterBurnerEntries);
    AliGenCocktailEntry *entry;
    //
    // Loop over generators and initialize
    while((entry = (AliGenCocktailEntry*)next())) {
	entry->Generator()->Init();
    }  
}
/*********************************************************************/ 
/*********************************************************************/ 

void AliGenCocktailAfterBurner::Generate()
{
//
// Generate event
//  Firsts runs each generator for all events
//  than after burners ones for each event
//
//  It generates and processes all events during
//  first call only.
//  In next calls it just returns already generated 
//  and processed events to the gAlice

    if (gDebug>0)
      cout<<"#####################################"<<endl
          <<"#AliGenCocktailAfterBurner::Generate#"<<endl
          <<"#####################################"<<endl;
    // Initialize header
    //
    Int_t i; //iterator
    AliStack * stack;
    
    if (fGenerationDone)
    {//if generation is done (in first call) 
     //just copy particles from the stack to the gAlice
      SetTracks(++fCurrentEvent);
      fHeader = fHeaders[fCurrentEvent];
      gAlice->SetGenEventHeader(fHeader); 
      cout<<"Returning event " << fCurrentEvent<<endl;
      return;  
    }
    else
    { //Here we are in the first call of the method
	Int_t numberOfEvents = AliRunLoader::Instance()->GetNumberOfEventsPerRun();
	cout << "Number of events per run" <<  numberOfEvents << endl;
	TArrayF eventVertex;
	eventVertex.Set(3 * (numberOfEvents + fNBgEvents));
	TArrayF eventTime;
	eventTime.Set(numberOfEvents + fNBgEvents);
	fCurrentEvent=0;
      //Create stacks
	fInternalStacks      = new TObjArray(numberOfEvents + fNBgEvents); //Create array of internal stacks
	fCollisionGeometries = new AliCollisionGeometry*[numberOfEvents + fNBgEvents]; //Create array of collision geometries
	fHeaders             = new AliGenCocktailEventHeader*[numberOfEvents + fNBgEvents]; //Create array of headers   

	for(i = 0; i < numberOfEvents + fNBgEvents; i++) 
       {	
	   stack = new AliStack(10000);
	   stack->Reset();
	   fInternalStacks->Add(stack);
	   Vertex();
	   for (Int_t j = 0; j < 3; j++) eventVertex[3 * i +  j] = fVertex[j];
	   eventTime[i] = fTime;
	   fHeaders[i] = new AliGenCocktailEventHeader();
	   fCollisionGeometries[i] = 0;
       }
/*********************************************************************/ 
      TIter next(fEntries);
      AliGenCocktailEntry *entry;
      AliGenCocktailEntry *e1;
      AliGenCocktailEntry *e2;
      const TObjArray *partArray;
  //
  // Loop over generators and generate events
      Int_t igen=0;
      while((entry = (AliGenCocktailEntry*)next())) 
      {
        igen++;
        cout<<"Generator "<<igen<<"  : "<<entry->GetName()<<endl;
/***********************************************/
//First generator for all evenets, than second for all events, etc...
        for(i = 0; i < numberOfEvents + fNBgEvents; i++) 
	{  
	    cout<<"                  EVENT "<<i << endl;
            stack = GetStack(i);
            partArray = stack->Particles();
            fCurrentGenerator = entry->Generator();
            fCurrentGenerator->SetStack(stack);
            if (igen ==1) 
	    {
                entry->SetFirst(0);
	    } 
            else 
	    {
                entry->SetFirst((partArray->GetEntriesFast())+1);
	    }
	    // Set the vertex for the generator
	    Int_t ioff = 3 * i;
	    fCurrentGenerator->SetVertex(eventVertex.At(ioff), eventVertex.At(ioff + 1), eventVertex.At(ioff + 2));
	    fCurrentGenerator->SetTime(eventTime.At(i));
	    fHeader = fHeaders[i];
	    // Set the vertex and time for the cocktail
	    TArrayF v(3);
	    for (Int_t j=0; j<3; j++) v[j] = eventVertex.At(ioff + j);
	    fHeader->SetPrimaryVertex(v);
	    fHeader->SetInteractionTime(eventTime.At(i));
	    // Generate event
	    fCurrentGenerator->Generate();
	    //
	    entry->SetLast(partArray->GetEntriesFast());
	    
	    if (fCurrentGenerator->ProvidesCollisionGeometry())  
	      fCollisionGeometries[i] = 
		new AliCollisionGeometry(*(fCurrentGenerator->CollisionGeometry()));
	} // event loop
/***********************************************/
      } // generator loop
      next.Reset();
      while((entry = (AliGenCocktailEntry*)next())) 
        {
          entry->PrintInfo();
        }
      for ( entry=FirstGenerator();entry;entry=NextGenerator() ) 
        {
          entry->PrintInfo();
        }
      for (FirstGeneratorPair(e1,e2); (e1&&e2); NextGeneratorPair(e1,e2) )
        {
          printf("\n -----------------------------");
          e1->PrintInfo();
          e2->PrintInfo();
        }
	
	
      /***********************************************/
      /*******After Burners Processing****************/
      /***********************************************/
      TIter nextAfterBurner(fAfterBurnerEntries);
      AliGenCocktailEntry *afterBurnerEntry;
      Int_t iab =0; //number of current after burner / counter
      
      cout<<"\n\nRunning After Burners"<<endl;
      while((afterBurnerEntry = (AliGenCocktailEntry*)nextAfterBurner()))
        {
          cout<<"After Burner "<<iab++<<"  :"<<afterBurnerEntry->GetName()<<endl;
          fCurrentGenerator = afterBurnerEntry->Generator();
          fCurrentGenerator->Generate();
        }
      cout<<endl<<"Finished. Processed "<<iab<<" After Burners"<<endl;

      /***********************************************/
      /***********************************************/
      /***********************************************/       
        
      fGenerationDone=kTRUE; 
      SetTracks(0); //copy event 0 to gAlice stack
	
/*********************************************************************/
      // Pass the header to gAlice
      fHeader = fHeaders[0];
      gAlice->SetGenEventHeader(fHeader); 
    } //else generated
}
/*********************************************************************/
/*********************************************************************/ 

AliStack* AliGenCocktailAfterBurner::GetStack(Int_t n) const
{
//Returns the pointer to the N'th stack (event)
  if( ( n<0 ) || ( n >= (GetNumberOfEvents()) ) )
    {
      Fatal("AliGenCocktailAfterBurner::GetStack","Asked for non existing stack (%d)",n);
      return 0; 
    }
    return ((AliStack*) fInternalStacks->At(n) );
}

/*********************************************************************/ 
/*********************************************************************/ 

AliCollisionGeometry* AliGenCocktailAfterBurner::GetCollisionGeometry(Int_t n) const
{
//Returns the pointer to the N'th stack (event)
  if( ( n<0 ) || ( n>=GetNumberOfEvents() ) )
    {
      Fatal("AliGenCocktailAfterBurner::GetCollisionGeometry","Asked for non existing stack (%d)",n);
      return 0; 
    }
    return fCollisionGeometries[n];
}

/*********************************************************************/ 
/*********************************************************************/ 

void AliGenCocktailAfterBurner::SetActiveEventNumber(Int_t actev)
{
//Set Active Events Number and Active Stack
//There is only one active event number
//Made fo convinience of work with AfterBurners (HBT processor)

    fActiveEvent = actev;
    fActiveStack = GetStack(actev);
}
/*********************************************************************/ 
/*********************************************************************/ 

void AliGenCocktailAfterBurner::SetTracks(Int_t stackno)
{
//Method which copies tracks from given stack to the
//gAlice's stack
    AliStack* instack = GetStack(stackno);
    Int_t done;
    Int_t parent; 
    Int_t pdg;
    Double_t px, py, pz, e, vx, vy, vz, tof, polx, poly, polz;
    TMCProcess mech;
    Int_t ntr;
    Float_t weight;
    TVector3 pol;
    
    TParticle * p;
    Int_t n = instack->GetNtrack();
    if (gDebug) 
    {
      cout<<"AliGenCocktailAfterBurner::SetTracks("<<stackno<<"). Number of particles is: "<<n<<"\n";
    }
    
    for(Int_t i = 0; i < n; i++)
    {
	
      p = instack->Particle(i);
      done = !p->TestBit(kDoneBit);
      parent = p->GetMother(0);
      pdg = p->GetPdgCode();
      px = p->Px();
      py = p->Py();
      pz = p->Pz();
      e  = p->Energy();
      vx = p->Vx();
      vy = p->Vy();
      vz = p->Vz();
      tof = p->T();
      p->GetPolarisation(pol);
      polx = pol.X();
      poly = pol.Y();
      polz = pol.Z();
      mech = AliGenCocktailAfterBurner::IntToMCProcess(p->GetUniqueID());
      weight = p->GetWeight();

      gAlice->GetMCApp()->PushTrack(done, parent, pdg, px, py, pz, e, vx, vy, vz, tof,polx, poly, polz, mech, ntr, weight);

      SetHighWaterMark(ntr) ; 

    }
}
/*********************************************************************/ 
/*********************************************************************/ 

TMCProcess AliGenCocktailAfterBurner::IntToMCProcess(Int_t no)
{
 //Mothod used to convert uniqueID (integer) to TMCProcess type
    const TMCProcess kMCprocesses[kMaxMCProcess] = 
    {
     kPNoProcess, kPMultipleScattering, kPEnergyLoss, kPMagneticFieldL, 
     kPDecay, kPPair, kPCompton, kPPhotoelectric, kPBrem, kPDeltaRay,
     kPAnnihilation, kPHadronic, kPNoProcess, kPEvaporation, kPNuclearFission,
     kPNuclearAbsorption, kPPbarAnnihilation, kPNCapture, kPHElastic, 
     kPHInhelastic, kPMuonNuclear, kPTOFlimit,kPPhotoFission, kPNoProcess, 
     kPRayleigh, kPNoProcess, kPNoProcess, kPNoProcess, kPNull, kPStop
    };
    
    for (Int_t i = 0;i<kMaxMCProcess;i++)
    {
      if (kMCprocesses[i] == no)
        {
          return kMCprocesses[i];
        }
    } 
    return kPNoProcess;
}


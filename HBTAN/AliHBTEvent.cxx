#include "AliHBTEvent.h"
#include "AliHBTParticle.h"
//_________________________________________________________________________
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// class AliHBTEvent                                                     //
//                                                                       //
// This class stores HBT perticles for one event                         //
// more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

ClassImp(AliHBTEvent)

const UInt_t AliHBTEvent::fgkInitEventSize = 10000;


/**************************************************************************/ 
 
AliHBTEvent::AliHBTEvent():
  fSize(fgkInitEventSize),
  fParticles(new AliHBTParticle* [fSize]),
  fNParticles(0),
  fOwner(kTRUE)
 {
 //default constructor    
 }
/**************************************************************************/ 

AliHBTEvent::~AliHBTEvent()
 {
 //destructor
   this->Reset();//delete all particles
   if(fParticles)
    { 
     delete [] fParticles; //and delete array itself
    }
   fParticles = 0x0;
 }
/**************************************************************************/ 
void  AliHBTEvent::Reset()
{
 //deletes all particles from the event
  if(fParticles && fOwner)
    {
      for(Int_t i =0; i<fNParticles; i++)
        delete fParticles[i];
    }
   fNParticles = 0;
} 
/**************************************************************************/ 

AliHBTParticle* AliHBTEvent::GetParticleSafely(Int_t n)
 {
 //returns nth particle  with range check
   if( (n<0) || (fNParticles<=n) ) return 0x0;
   else return fParticles[n];
   
 }
/**************************************************************************/ 

void  AliHBTEvent:: AddParticle(AliHBTParticle* hbtpart)
 {
 //Adds new perticle to the event
   if ( fNParticles+1 >= fSize) Expand(); //if there is no space in array, expand it
   fParticles[fNParticles++] = hbtpart; //add a pointer
 }

/**************************************************************************/ 
void  AliHBTEvent::AddParticle(TParticle* part)
 {
 //Adds TParticle to event
   AddParticle( new AliHBTParticle(*part) );
 }
/**************************************************************************/ 
void  AliHBTEvent::
AddParticle(Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t etot,
            Double_t vx, Double_t vy, Double_t vz, Double_t time)
 {
 //adds particle to event
   AddParticle(new  AliHBTParticle(pdg,px,py,pz,etot,vx,vy,vz,time) );
 }
/**************************************************************************/ 

void AliHBTEvent::Expand()
 {
 //expands the array with pointers to particles
 //about the size defined in fgkInitEventSize
 
  fSize+=fgkInitEventSize;
  AliHBTParticle** tmpParticles = new AliHBTParticle* [fSize]; //create new array of pointers
  //check if we got memory / if not Abort
  if (!tmpParticles) Fatal("AliHBTEvent::Expand()","No more space in memory");
  
  
  for(Int_t i = 0; i<fNParticles ;i++)
   //copy pointers to the new array
   {
     tmpParticles[i] = fParticles[i];
   }
  delete [] fParticles; //delete old array
   fParticles = tmpParticles; //copy new pointer to the array of pointers to particles
  
 }
 
 

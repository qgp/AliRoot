#include "AliReader.h"
//_________________________________________________________________________
///////////////////////////////////////////////////////////////////////////
//
// class AliReader
//
// Reader Base class (reads particles and tracks and
// puts it to the AliAODRun objects
//
// Provides functionality for both buffering and non-buffering reading
// This can be switched on/off via method SetEventBuffering(bool)
// The main method that inheriting classes need to implement is ReadNext()
// that read next event in queue.
// The others are:
// Bool_t  ReadsRec() const; specifies if reader is able to read simulated particles
// Bool_t  ReadsSim() const; specifies if reader is able to read reconstructed tracks
// void    Rewind(); rewind reading to the beginning
//
// reading of next event is triggered via method Next()
//
// This class provides full functionality for reading from many sources
// User can provide TObjArray of TObjString (SetDirs method or via parameter 
// in constructor) which desribes paths of directories to search data in.
// If none specified current directory is searched.
//
// Piotr.Skowronski@cern.ch
///////////////////////////////////////////////////////////////////////////

#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TClass.h>
#include <TRandom.h>
#include <TH1.h>

#include "AliAODParticleCut.h"
#include "AliAOD.h"
#include "AliAODRun.h"
 
ClassImp(AliReader)
//pure virtual
    
/*************************************************************************************/

AliReader::AliReader():
 fCuts(new TObjArray()),
 fDirs(0x0),
 fCurrentEvent(0),
 fCurrentDir(0),
 fNEventsRead(0),
 fEventRec(0x0),
 fEventSim(0x0),
 fRunSim(0x0),
 fRunRec(0x0),
 fIsRead(kFALSE),
 fBufferEvents(kFALSE),
 fBlend(kFALSE),
 fFirst(0),
 fLast(0),
 fTrackCounter(0x0)
{
//constructor
}
/*************************************************************************************/

AliReader::AliReader(TObjArray* dirs):
 fCuts(new TObjArray()),
 fDirs(dirs),
 fCurrentEvent(0),
 fCurrentDir(0),
 fNEventsRead(0),
 fEventRec(0x0),
 fEventSim(0x0),
 fRunSim(0x0),
 fRunRec(0x0),
 fIsRead(kFALSE),
 fBufferEvents(kFALSE),
 fBlend(kFALSE),
 fFirst(0),
 fLast(0),
 fTrackCounter(0x0)
{
//ctor with array of directories to read as parameter
}
/*************************************************************************************/
AliReader::AliReader(const AliReader& in):
 TNamed(in),
 fCuts((in.fCuts)?(TObjArray*)in.fCuts->Clone():0x0),
 fDirs((in.fDirs)?(TObjArray*)in.fDirs->Clone():0x0),
 fCurrentEvent(0),
 fCurrentDir(0),
 fNEventsRead(0),
 fEventRec(0x0),
 fEventSim(0x0),
 fRunSim(0x0),
 fRunRec(0x0),
 fIsRead(kFALSE),
 fBufferEvents(in.fBufferEvents),
 fBlend(in.fBlend),
 fFirst(in.fFirst),
 fLast(in.fLast),
 fTrackCounter(0x0)
{
 //cpy constructor
}

AliReader::~AliReader()
{
//destructor
 if(fCuts)
  {
   fCuts->SetOwner();
   delete fCuts;
  }
 delete fEventSim;
 delete fEventRec;
 delete fTrackCounter;
}
/*************************************************************************************/

AliReader& AliReader::operator=(const AliReader& in)
{
  //Assigment operator
 if (this == &in) return *this;  
 TNamed::operator=( (const TNamed&)in );
  
 fCuts = (in.fCuts)?(TObjArray*)in.fCuts->Clone():0x0;
 fDirs = (in.fDirs)?(TObjArray*)in.fDirs->Clone():0x0;
 fCurrentEvent = 0;
 fCurrentDir = 0;
 fNEventsRead = 0;
 fEventRec = 0x0;
 fEventSim = 0x0;
 fRunSim = 0x0;
 fRunRec = 0x0;
 fIsRead = kFALSE;
 fBufferEvents = in.fBufferEvents;
 fBlend = in.fBlend;
 fFirst = in.fFirst;
 fLast = in.fLast;
 fTrackCounter = 0x0;
 return *this;  
}
/*************************************************************************************/

Int_t AliReader::Next()
{
//moves to next event

  //if asked to read up to event nb. fLast, and it is overcome, report no more events
  if ((fNEventsRead > fLast) && (fLast > 0) ) return kTRUE;
  
  if (fTrackCounter == 0x0)//create Track Counter
   {
     fTrackCounter = new TH1I("trackcounter","Track Counter",20000,0,20000);
     fTrackCounter->SetDirectory(0x0);
   }
  
  do //if asked to read from event fFirst, rewind to it
   {
    if ( ReadNext() == kTRUE) //if no more evets, return it
      return kTRUE;
   }while (fNEventsRead < fFirst);
   
  //here we have event
  
  if (fBlend) Blend();//Mix particles order 
  
  if (fBufferEvents)//store events if buffering is on
   {
     if ( ReadsRec() && fEventRec) 
       fRunRec->SetEvent(fNEventsRead-1-fFirst,fEventRec);
     if ( ReadsSim() && fEventSim)
       fRunSim->SetEvent(fNEventsRead-1-fFirst,fEventSim);
   }
  return kFALSE;
}
/*************************************************************************************/

void AliReader::AddParticleCut(AliAODParticleCut* cut)
{
 //sets the new cut. MAKES A COPY OF THE CUT !!!!
 
  if (!cut) //if cut is NULL return with error
   {
    Error("AddParticleType","NULL pointers are not accepted any more.\nIf You want to accept all particles of this type, set an empty cut ");
    return;
   }
  AliAODParticleCut *c = (AliAODParticleCut*)cut->Clone();
  fCuts->Add(c);
}
/********************************************************************/

AliAOD* AliReader::GetEventSim(Int_t n)
 {
 //returns Nth event with simulated particles
  if (ReadsSim() == kFALSE)
   {
     Error("GetParticleEvent","This reader is not able to provide simulated particles.");
     return 0;
   } 
   
  if (!fIsRead) 
   { 
    if (ReadsSim() && (fRunSim == 0x0)) fRunSim = new AliAODRun();
    if (ReadsRec() && (fRunRec == 0x0)) fRunRec = new AliAODRun();
    
    if (Read(fRunSim,fRunRec))
     {
       Error("GetParticleEvent","Error in reading");
       return 0x0;
     }
    else fIsRead = kTRUE;
   }
  return fRunSim->GetEvent(n);
 }
/********************************************************************/

AliAOD* AliReader::GetEventRec(Int_t n)
 {
 //returns Nth event with reconstructed tracks
  if (ReadsRec() == kFALSE)
   {
     Error("GetTrackEvent","This reader is not able to provide recosntructed tracks.");
     return 0;
   } 
  if (!fIsRead) 
   {
    if (ReadsSim() && (fRunSim == 0x0)) fRunSim = new AliAODRun();
    if (ReadsRec() && (fRunRec == 0x0)) fRunRec = new AliAODRun();
    
    if(Read(fRunSim,fRunRec))
     {
       Error("GetTrackEvent","Error in reading");
       return 0x0;
     }
    else fIsRead = kTRUE;
   }
  return fRunRec->GetEvent(n);
 }
/********************************************************************/

Int_t AliReader::GetNumberOfSimEvents()
 {
 //returns number of events of particles
  if (ReadsSim() == kFALSE)
   {
     Error("GetNumberOfPartEvents","This reader is not able to provide simulated particles.");
     return 0;
   } 
   
  if (!fIsRead) 
   {
    if (ReadsSim() && (fRunSim == 0x0)) fRunSim = new AliAODRun();
    if (ReadsRec() && (fRunRec == 0x0)) fRunRec = new AliAODRun();
    
    if (Read(fRunSim,fRunRec))
     {
       Error("GetNumberOfPartEvents","Error in reading");
       return 0;
     }
    else fIsRead = kTRUE;
   }
   return fRunSim->GetNumberOfEvents();
 }
/********************************************************************/
 
Int_t AliReader::GetNumberOfRecEvents()
 {
 //returns number of events of tracks
  if (ReadsRec() == kFALSE)
   {
     Error("GetNumberOfTrackEvents","This reader is not able to provide recosntructed tracks.");
     return 0;
   } 
  if (!fIsRead)
   {
     if (ReadsSim() && (fRunSim == 0x0)) fRunSim = new AliAODRun();
     if (ReadsRec() && (fRunRec == 0x0)) fRunRec = new AliAODRun();
     
     if(Read(fRunSim,fRunRec))
      {
        Error("GetNumberOfTrackEvents","Error in reading");
        return 0;
      }
     else fIsRead = kTRUE;
   }
  return fRunRec->GetNumberOfEvents();
 }
/********************************************************************/

Int_t AliReader::Read(AliAODRun* particles, AliAODRun *tracks)
{
 //reads data and puts put to the particles and tracks objects
 //reurns 0 if everything is OK
 //
  Info("Read","");
  
  if ( ReadsSim() && (particles == 0x0) ) //check if an object is instatiated
   {
     Error("Read"," particles object must be instatiated before passing it to the reader");
     return 1;
   }
  if ( ReadsRec() && (tracks == 0x0) )  //check if an object is instatiated
   {
     Error("Read"," tracks object must be instatiated before passing it to the reader");
     return 1;
   }
   
  if (ReadsSim()) particles->Reset();//clear runs == delete all old events
  if (ReadsRec()) tracks->Reset();
  
  Rewind();
  
  Int_t i = 0;
  while(Next() == kFALSE)
   {
     if (ReadsRec()) tracks->SetEvent(i,fEventRec);
     if (ReadsSim()) particles->SetEvent(i,fEventSim);
     i++;
   }
  return 0;
}      
/*************************************************************************************/

Bool_t AliReader::Pass(AliVAODParticle* p)
{
 //Method examines whether particle meets all cut and particle type criteria
  
   if(p==0x0)//of corse we not pass NULL pointers
    {
     Warning("Pass()","No Pasaran! We never accept NULL pointers");
     return kTRUE;
    }
   //if no particle is specified, we pass all particles
   //excluding NULL pointers, of course
  if ( fCuts->GetEntriesFast() == 0 ) return kFALSE; //if no cut specified accept all particles
  for(Int_t i=0; i<fCuts->GetEntriesFast(); i++)   
   {
     AliAODParticleCut &cut = *((AliAODParticleCut*)fCuts->At(i));
     if(!cut.Pass(p)) return kFALSE;  //accepted
   }
   
  return kTRUE;//not accepted
}
/*************************************************************************************/

Bool_t  AliReader::Pass(Int_t pid)
{
//this method checks if any of existing cuts accepts this pid particles
//or any cuts accepts all particles

 if(pid == 0)
  return kTRUE;

 if ( fCuts->GetEntriesFast() == 0 ) return kFALSE; //if no cut specified accept all particles
  
 for(Int_t i=0; i<fCuts->GetEntriesFast(); i++)   
   {
     AliAODParticleCut &cut = *((AliAODParticleCut*)fCuts->At(i));
     //if some of cuts accepts all particles or some accepts particles of this type, accept
     if ( (cut.GetPID() == 0) || (cut.GetPID() == pid) ) return kFALSE; 
   }
 return kTRUE;
}
/*************************************************************************************/

TString& AliReader::GetDirName(Int_t entry)
{
//returns directory name of next one to read
  TString* retval;//return value
  if (fDirs ==  0x0)
   {
     retval = new TString(".");
     return *retval;
   }

  if ( (entry>fDirs->GetEntries()) || (entry<0))//if out of bounds return empty string
   {                                            //note that entry==0 is accepted even if array is empty (size=0)
     Error("GetDirName","Name out of bounds");
     retval = new TString();
     return *retval;
   }

  if (fDirs->GetEntries() == 0)
   { 
     retval = new TString(".");
     return *retval;
   }

  TClass *objclass = fDirs->At(entry)->IsA();
  TClass *stringclass = TObjString::Class();

  TObjString *dir = (TObjString*)objclass->DynamicCast(stringclass,fDirs->At(entry));

  if(dir == 0x0)
   {
     Error("GetDirName","Object in TObjArray is not a TObjString or its descendant");
     retval = new TString();
     return *retval;
   }
  if (gDebug > 0) Info("GetDirName","Returned ok %s",dir->String().Data());
  return dir->String();
}
/*************************************************************************************/

void AliReader::Blend()
{
  //randomly change positions of the particles after reading
  //is used to check if some distributions (of many particle properties) 
  //depend on the order of particles
  //(tracking gives particles Pt sorted)
  
  if (fEventSim == 0x0) return;
  
  for (Int_t i = 2; i < fEventSim->GetNumberOfParticles(); i++)
   {
     Int_t with = gRandom->Integer(i);
     fEventSim->SwapParticles(i,with);
     if (fEventRec) fEventRec->SwapParticles(i,with);
   }
}
/*************************************************************************************/

void AliReader::WriteTrackCounter() const
{
 //writes the counter histogram
 
 if (fTrackCounter) fTrackCounter->Write(0,TObject::kOverwrite);
 else 
  {
    Warning("WriteTrackCounter","Counter is NULL");
  }
}

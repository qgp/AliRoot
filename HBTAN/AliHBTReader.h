#ifndef ALIHBTREADER_H
#define ALIHBTREADER_H

#include <TNamed.h>
#include <TObjArray.h>

//Reader Base class (reads particles and tracks and
//puts it to the AliHBTRun objects
//Piotr.Skowronski@cern.ch

class AliHBTRun;
class AliHBTEvent;
class AliHBTParticleCut;
class AliHBTParticle;
class TString;
 
class AliHBTReader: public TNamed
{
  public:
    AliHBTReader();
    AliHBTReader(TObjArray*);
    virtual ~AliHBTReader();
    
    virtual Int_t        Next();
    virtual void         Rewind() = 0;
    
    virtual Bool_t       ReadsTracks() const = 0;
    virtual Bool_t       ReadsParticles() const = 0;
    
    void                 AddParticleCut(AliHBTParticleCut* cut);
    
    virtual Int_t        Read(AliHBTRun* particles, AliHBTRun *tracks);
    
    virtual AliHBTEvent* GetParticleEvent() {return fParticlesEvent;}//can not be const because position randomizer overloads it
    virtual AliHBTEvent* GetTrackEvent() {return fTracksEvent;}//
    
    virtual AliHBTEvent* GetParticleEvent(Int_t);
    virtual AliHBTEvent* GetTrackEvent(Int_t);
    
    virtual Int_t        GetNumberOfPartEvents();
    virtual Int_t        GetNumberOfTrackEvents();
    
    void                 SetDirs(TObjArray* dirs){fDirs = dirs;} //sets array directories names
    void                 SetEventBuffering(Bool_t flag){fBufferEvents = flag;}
    
    virtual Int_t GetNumberOfDirs() const {return (fDirs)?fDirs->GetEntries():0;}
  protected:
    
    TObjArray*    fCuts;//array with particle cuts
    TObjArray*    fDirs;//arry with directories to read data from
    
    Int_t         fCurrentEvent;//!  number of current event in current directory
    Int_t         fCurrentDir;//! number of current directory
    
    Int_t         fNEventsRead;//!total 
        
    AliHBTEvent*  fTracksEvent;    //! tracks read from current event
    AliHBTEvent*  fParticlesEvent; //! particles read from current event
    
    AliHBTRun*    fParticles; //!simulated particles
    AliHBTRun*    fTracks; //!reconstructed tracks (particles)
    
    Bool_t        fIsRead;//!flag indicating if the data are already read
    Bool_t        fBufferEvents;//flag indicating if the data should be bufferred
    
    virtual Int_t ReadNext() = 0; //this methods reads next event and put result in fTracksEvent and/or fParticlesEvent
    Bool_t Pass(AliHBTParticle*);
    Bool_t Pass(Int_t pid);

    TString& GetDirName(Int_t);
    
  private:
  
  public:
    ClassDef(AliHBTReader,2)//version 2 - TNamed as parental class
    
};

#endif

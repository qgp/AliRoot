#ifndef AliHBTReaderTOFpid_H
#define AliHBTReaderTOFpid_H

#include "TTask.h"
#include "TCutG.h"
#include "TString.h"
#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include "AliHBTReader.h"

//Multi file reader for TOF
//
//This reader reads tracks AliTOFtracks.root
//                  particles form gAlice
//Piotr.Skowronski@cern.ch
//more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html

#include <TString.h>
class TFile;

class AliHBTReaderTOFpid: public AliHBTReader
{
  public:
    AliHBTReaderTOFpid(const Char_t* trackfilename = "rhi1-20probeTrack.root");

    AliHBTReaderTOFpid(TObjArray* dirs,
                      const Char_t* trackfilename = "rhi1-20probeTrack.root");

    virtual ~AliHBTReaderTOFpid();
    
    Int_t Read(AliHBTRun* particles, AliHBTRun *tracks);//reads tracks and particles and puts them in runs
    
    AliHBTEvent* GetParticleEvent(Int_t);//returns pointer to event with particles
    AliHBTEvent* GetTrackEvent(Int_t);//returns pointer to event with particles 
    Int_t GetNumberOfPartEvents();//returns number of particle events
    Int_t GetNumberOfTrackEvents();//returns number of track events
    
  protected:
    //in the future this class is will read global tracking

    
    Int_t OpenFiles(TFile*&,Int_t);//opens files to be read for given event
    void CloseFiles(TFile*&);//close files    
    
    AliHBTRun* fParticles; //!simulated particles
    AliHBTRun* fTracks; //!reconstructed tracks (particles)
    TNtuple *fNtuple; // pointer to ntuple    

   TString fTrackFileName;//name of the file with tracks

    Bool_t fIsRead;//!flag indicating if the data are already read
  private:
  public:
    ClassDef(AliHBTReaderTOFpid,2)
};


#endif

#include "AliHBTReaderTOFpid.h"
#include <TTree.h>
#include <TFile.h>
#include <TParticle.h>

#include <AliRun.h>
#include <AliMagF.h>

#include "AliHBTRun.h"
#include "AliHBTEvent.h"
#include "AliHBTParticle.h"
#include "AliHBTParticleCut.h"

#include <TNtuple.h>
#include "TFile.h"
#include "TFolder.h"
#include "TNtuple.h"
#include "TLeaf.h"
#include <stdlib.h>
#include <Riostream.h>
#include "AliTOFPID.h" 

ClassImp(AliHBTReaderTOFpid)
/*******************************************************************/
AliHBTReaderTOFpid:: AliHBTReaderTOFpid(const Char_t* trackfilename):
 fTrackFileName(trackfilename)
{
  //constructor, 
  //Defaults:
  //  trackfilename = "rhi1-20probeTrack.root"
  
  fParticles = new AliHBTRun();
  fTracks    = new AliHBTRun();
  fIsRead = kFALSE;
}

/********************************************************************/
AliHBTReaderTOFpid::AliHBTReaderTOFpid(TObjArray* dirs,
                  const Char_t* trackfilename):
 AliHBTReader(dirs), 
 fTrackFileName(trackfilename)
{
//dirs contains strings with directories to look data in
//trackfilename - "AliHBTtrackspid.root"
 fParticles = new AliHBTRun();
 fTracks    = new AliHBTRun();
 fIsRead = kFALSE;
}
/********************************************************************/

AliHBTReaderTOFpid::~AliHBTReaderTOFpid()
 {
 //desctructor
   delete fParticles;
   delete fTracks;
 }
/********************************************************************/

AliHBTEvent* AliHBTReaderTOFpid::GetParticleEvent(Int_t n)
 {
 //returns Nth event with simulated particles
   if (!fIsRead) 
    if(Read(fParticles,fTracks))
     {
       Error("GetParticleEvent","Error in reading");
       return 0x0;
     }
   return fParticles->GetEvent(n);
 }
/********************************************************************/

AliHBTEvent* AliHBTReaderTOFpid::GetTrackEvent(Int_t n)
 {
 //returns Nth event with reconstructed tracks
   if (!fIsRead) 
    if(Read(fParticles,fTracks))
     {
       Error("GetTrackEvent","Error in reading");
       return 0x0;
     }
   return fTracks->GetEvent(n);
 }
/********************************************************************/

Int_t AliHBTReaderTOFpid::GetNumberOfPartEvents()
 {
 //returns number of events of particles
   if (!fIsRead) 
    if ( Read(fParticles,fTracks))
     {
       Error("GetNumberOfPartEvents","Error in reading");
       return 0;
     }
   return fParticles->GetNumberOfEvents();
 }

/********************************************************************/
Int_t AliHBTReaderTOFpid::GetNumberOfTrackEvents()
 {
 //returns number of events of tracks
  if (!fIsRead)
    if(Read(fParticles,fTracks))
     {
       Error("GetNumberOfTrackEvents","Error in reading");
       return 0;
     }
  return fTracks->GetNumberOfEvents();
 }

/*----------------------------------------------------------------*/
Int_t AliHBTReaderTOFpid::Read(AliHBTRun* particles, AliHBTRun *tracks)
 {
//reads data and puts to the particles and tracks objects
//returns 0 if everything is OK
//

  Info("Read","");
  Int_t i; //iterator and some temprary values
  Int_t Nevents = 0;
  Int_t totalNevents = 0;
  TFile *aTracksPIDFile;//file with tracks

  if (!particles) //check if an object is instatiated
   {
     Error("Read"," particles object must instatiated before passing it to the reader");
   }
  if (!tracks)  //check if an object is instatiated
   {
     Error("Read"," tracks object must instatiated before passing it to the reader");
   }
  particles->Reset();//clear runs == delete all old events
  tracks->Reset();
 
  Int_t currentdir = 0;
  Int_t Ndirs;

  if (fDirs) //if array with directories is supplied by user
   {
     Ndirs = fDirs->GetEntries(); //get the number if directories
   cout<<"Ndirs= "<<Ndirs<<endl;
   }
  else
   {
     Ndirs = 0; //if the array is not supplied read only from current directory
   }

  do  //do{}while; is OK even if 0 dirs specified. In that case we try to read from "./"
   {
    
    if( (i=OpenFiles(aTracksPIDFile,currentdir)) )
     {
       Error("Read","Exiting due to problems with opening files. Errorcode %d",i);
       currentdir++;
       continue;
      }      
      
    Nevents=1;
    
 fNtuple= (TNtuple*)aTracksPIDFile->Get("Ntuple"); // get ntuple from file  
 Int_t nvar = fNtuple->GetNvar(); cout <<"N of var.="<< nvar << endl;
 fNtuple->GetEvent(0);    

for (int currentEvent=0; currentEvent< Nevents; currentEvent++) {

Int_t nparticles = (Int_t)fNtuple->GetEntries();

  printf(" Number of nparticles in tof ntuple = %d ", nparticles);  

if (nparticles <= 0) return 1;

 for(Int_t i=0;i < nparticles; i++){
   fNtuple->GetEvent(i);
    Int_t pdgcode=(Int_t)(fNtuple->GetLeaf("ipart")->GetValue());
    Float_t mrec=fNtuple->GetLeaf("mext")->GetValue(0);
    Float_t leng=fNtuple->GetLeaf("leng")->GetValue(0);
    Int_t matc=(Int_t)(fNtuple->GetLeaf("matc")->GetValue(0));
    Int_t imam=(Int_t)(fNtuple->GetLeaf("imam")->GetValue(0));
    Float_t time=fNtuple->GetLeaf("text")->GetValue(0); // [ns]
    Double_t mass =0.13956995; //GetMassITS(pdgcode); //identified by dedx

    Float_t px=fNtuple->GetLeaf("pxvtx")->GetValue(0);
    Float_t py=fNtuple->GetLeaf("pyvtx")->GetValue(0);
    Float_t pz=fNtuple->GetLeaf("pzvtx")->GetValue(0);
    Double_t Eini=TMath::Sqrt(px*px+py*py+pz*pz+mass*mass);
    Float_t tpx=fNtuple->GetLeaf("pTPCx")->GetValue(0);
    Float_t tpy=fNtuple->GetLeaf("pTPCy")->GetValue(0);
    Float_t tpz=fNtuple->GetLeaf("pTPCz")->GetValue(0);
    Float_t pTPC=TMath::Sqrt(tpx*tpx+tpy*tpy+tpz*tpz);
    Double_t tEtot=TMath::Sqrt(tpx*tpx+tpy*tpy+tpz*tpz+mass*mass);
    Float_t pTOF=fNtuple->GetLeaf("pTOF")->GetValue(0);
    Int_t abspcode=0;

     if((matc==3 || matc==4)&&mrec<0.3&&mrec>0.){
     abspcode=211;
     }

     Int_t abspdgcode=TMath::Abs(pdgcode);
     Int_t pcode=abspcode*pdgcode/abspdgcode;

  AliHBTParticle* track = new AliHBTParticle(pcode,0,imam,tpx,tpy,tpz,tEtot, 0., 0., 0., 0.);
  AliHBTParticle* part = new AliHBTParticle(pdgcode,0,imam,px,py,pz,Eini, 0., 0., 0., 0.);

//         if(fpidcheck){
	 if(pcode!=211){
               delete track;
               delete part;
               continue;
             }
//    	     }

            if(Pass(track))//check if meets all criteria of any of our cuts
                         //if it does not delete it and take next good track
             { 
               delete track;
               delete part;
               continue;
             }

            particles->AddParticle(totalNevents,part);//put track and particle on the run
            tracks->AddParticle(totalNevents,track);

          } // End for tracks
	  
      totalNevents++;
      delete fNtuple;
      
       }// End for (events)

    CloseFiles(aTracksPIDFile); 
    currentdir++;
  }while(currentdir < Ndirs);

  fIsRead = kTRUE;
  return 0;
 }

/********************************************************************/
Int_t AliHBTReaderTOFpid::OpenFiles(TFile*& aTracksPIDFile,Int_t event)
{
 //opens the PID-files
    
   const TString& dirname = GetDirName(event);
   cout<<"--dirname--"<<dirname<<endl; 
   if (dirname == "")
    {
      Error("OpenFiles","Can not get directory name");
      return 4;
    }
   
   TString filename = dirname +"/"+ fTrackFileName;
   aTracksPIDFile = TFile::Open(filename.Data());
   
   if ( aTracksPIDFile  == 0x0 ) 
     {
       Error("OpenFiles 0X0","Can't open file named %s",aTracksPIDFile," ",filename.Data());
       return 1;
     }
   if (!aTracksPIDFile->IsOpen())
     {
       Error("OpenFiles is open","Can't open file named %s",filename.Data());
       return 1;
     }
  
     return 0; 
}
/********************************************************************/

/********************************************************************/
  
void AliHBTReaderTOFpid::CloseFiles(TFile*& tracksFile)
{
  //closes the files
  tracksFile->Close();
  delete tracksFile;
  tracksFile = 0x0;
}

/********************************************************************/

/********************************************************************/
/********************************************************************/
/********************************************************************/


	





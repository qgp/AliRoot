
// ROOT includes
#include "TBranch.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

// STEER includes
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliLoader.h"

// MUON includes
#include "AliMUON.h"
#include "AliMUONHit.h"
#include "AliMUONDigit.h"
#include "AliMUONRawCluster.h"

void MUONhits(char * filename="galice.root")
{
  // List of Hits per event and per track
  TClonesArray * ListOfHits = new TClonesArray("AliMUONHit",1000);

  // Creating Run Loader and openning file containing Hits
  AliRunLoader * RunLoader = AliRunLoader::Open(filename,"MUONFolder","READ");
  if (RunLoader ==0x0) {
    printf(">>> Error : Error Opening %s file \n",filename);
    return;
  }

  // Loading AliRun master
  RunLoader->LoadgAlice();
  gAlice = RunLoader->GetAliRun();

  // Loading MUON subsystem
  AliMUON * MUON = (AliMUON *) gAlice->GetDetector("MUON");
  AliLoader * MUONLoader = RunLoader->GetLoader("MUONLoader");
  MUONLoader->LoadHits("READ");

  Int_t ievent, nevents;
  nevents = RunLoader->GetNumberOfEvents();

  for(ievent=0; ievent<nevents; ievent++) {  // Event loop
    printf(">>> event %d \n",ievent);

    // Getting event ievent
    gAlice->GetEvent(ievent); 

    Int_t itrack, ntracks;
    ntracks = (Int_t) (MUONLoader->TreeH())->GetEntries();
    for (itrack=0; itrack<ntracks; itrack++) { // Track loop
      printf(">>>  track %d \n",itrack);

      //Getting List of Hits of Track itrack
      (MUONLoader->TreeH())->GetEvent(itrack); 
      ListOfHits = MUON->Hits();

      Int_t ihit, nhits;
      nhits = (Int_t) ListOfHits->GetEntriesFast();
      AliMUONHit* mHit;
      for(ihit=0; ihit<nhits; ihit++) {
	mHit = static_cast<AliMUONHit*>(ListOfHits->At(ihit));
  	Int_t Nch      = mHit->Chamber();  // chamber number
	Int_t hittrack = mHit->Track();
	Float_t x      = mHit->X();
  	Float_t y      = mHit->Y();
  	Float_t z      = mHit->Z();
  	Float_t elos   = mHit->Eloss();
  	Float_t theta  = mHit->Theta();
  	Float_t phi    = mHit->Phi();
  	Float_t momentum = mHit->Momentum();
  	printf(">>> Hit %2d Chamber %2d Track %4d x %5.3f y %5.3f z %7.3f elos %g theta %5.3f phi %5.3f momentum %5.3f\n",
	       ihit, Nch,hittrack,x,y,z,elos,theta,phi, momentum);
      }
    } // end track loop
  }  // end event loop
  MUONLoader->UnloadHits();
  RunLoader->UnloadgAlice();
}


void MUONdigits(char * filename="galice.root")
{
  TClonesArray * ListOfDigits;

  // Creating Run Loader and openning file containing Hits
  AliRunLoader * RunLoader = AliRunLoader::Open(filename,"MUONFolder","READ");
  if (RunLoader ==0x0) {
    printf(">>> Error : Error Opening %s file \n",filename);
    return;
  }

  // Loading AliRun master
  RunLoader->LoadgAlice();
  gAlice = RunLoader->GetAliRun();
 // Getting Module MUON  
  AliMUON *pMUON  = (AliMUON *) gAlice->GetDetector("MUON");

  AliLoader * MUONLoader = RunLoader->GetLoader("MUONLoader");
  MUONLoader->LoadDigits("READ");

  Int_t ievent, nevents;
  nevents = RunLoader->GetNumberOfEvents();

  AliMUONDigit * mDigit;
  

  for(ievent=0; ievent<nevents; ievent++) {
    printf(">>> event %d \n",ievent);
    RunLoader->GetEvent(ievent);
  
    // Addressing
    Int_t ichamber, nchambers;
    nchambers=14;
    pMUON->SetTreeAddress(); 
    char branchname[30];    
   //for( ichamber=0; ichamber<nchambers; ichamber++) {
    // sprintf(branchname,"MUONDigits%d",ichamber+1);
    //(MUONLoader->TreeD()->GetBranch(branchname))->SetAddress(&((*pMUON->Dchambers())[ichamber]));
    //}
   

    Int_t icathode, ncathodes;
    ncathodes=2;
    //Loop on cathodes 
    for(icathode=0; icathode<ncathodes; icathode++) {
      printf(">>>  cathode %d\n",icathode);
      MUONLoader->TreeD()->GetEvent(icathode);
      // Loop on chambers
      for( ichamber=0; ichamber<nchambers; ichamber++) {
	printf(">>>   chamber %d\n",ichamber);
	sprintf(branchname,"MUONDigits%d",ichamber+1);
	printf(">>>  branchname %s\n",branchname);
	ListOfDigits = (TClonesArray *) pMUON->Dchambers()->At(ichamber);
	
	Int_t idigit, ndigits;

	ndigits = (Int_t) ListOfDigits->GetEntriesFast();
	
	for(idigit=0; idigit<ndigits; idigit++) {
	  mDigit = static_cast<AliMUONDigit*>(ListOfDigits->At(idigit));
	  Int_t PadX   = mDigit->PadX();     // Pad X number
	  Int_t PadY   = mDigit->PadY();     // Pad Y number
	  Int_t Signal = mDigit->Signal();   // Physics Signal
	  Int_t Hit    = mDigit->Hit();      // iHit
	  Int_t Cathode= mDigit->Cathode();  // Cathode
	    
	  printf(">>> >> digit %d cathode %d hit %d PadX %d PadY %d Signal %d \n",idigit, Cathode,Hit, PadX, PadY, Signal);
	} // end digit loop
      } // end chamber loop
    } // end cathode loop
  }  // end event loop
  MUONLoader->UnloadHits();
}


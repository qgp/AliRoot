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

//_________________________________________________________________________
// Implementation version v1 of the EMCAL particle identifier 

//*-- Author: Yves Schutz (SUBATECH) 
//
// --- ROOT system ---
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TSystem.h"
#include "TBenchmark.h"
#include "TSystem.h"
  
  // --- Standard library ---
  
  // --- AliRoot header files ---
  
#include "AliGenerator.h"
#include "AliEMCAL.h"
#include "AliEMCALPIDv1.h"
#include "AliEMCALClusterizerv1.h"
#include "AliEMCALTrackSegment.h"
#include "AliEMCALTrackSegmentMakerv1.h"
#include "AliEMCALRecParticle.h"
#include "AliEMCALGeometry.h"
#include "AliEMCALGetter.h"
  
  ClassImp( AliEMCALPIDv1) 

//____________________________________________________________________________
AliEMCALPIDv1::AliEMCALPIDv1():AliEMCALPID()
{ 
  // default ctor
  
  InitParameters() ; 
  fDefaultInit = kTRUE ; 
  
}

//____________________________________________________________________________
AliEMCALPIDv1::AliEMCALPIDv1(const AliEMCALPIDv1 & pid ):AliEMCALPID(pid)
{ 
  // ctor
  InitParameters() ; 
  Init() ;
  
}

//____________________________________________________________________________
AliEMCALPIDv1::AliEMCALPIDv1(const TString alirunFileName, const TString eventFolderName):AliEMCALPID(alirunFileName, eventFolderName)
{ 
  //ctor with the indication on where to look for the track segments
 
  InitParameters() ; 
  Init() ;
  fDefaultInit = kFALSE ; 

}

//____________________________________________________________________________
AliEMCALPIDv1::~AliEMCALPIDv1()
{ 
  // dtor
}

//____________________________________________________________________________
const TString AliEMCALPIDv1::BranchName() const 
{  

  return GetName() ;
}
 
//____________________________________________________________________________
void AliEMCALPIDv1::Init()
{
  // Make all memory allocations that are not possible in default constructor
  // Add the PID task to the list of EMCAL tasks


  AliEMCALGetter * gime = AliEMCALGetter::Instance(GetTitle(), fEventFolderName.Data()) ; 

  if ( !gime->PID() ) 
    gime->PostPID(this) ;
}

//____________________________________________________________________________
void AliEMCALPIDv1::InitParameters()
{

  fRecParticlesInRun = 0 ; 
  fNEvent            = 0 ;            
  fRecParticlesInRun = 0 ;
}

//____________________________________________________________________________

void  AliEMCALPIDv1::Exec(Option_t * option) 
{
  //Steering method

  if(strstr(option,"tim"))
    gBenchmark->Start("EMCALPID");
  
  if(strstr(option,"print")) {
    Print("") ; 
    return ; 
  }
  AliEMCALGetter * gime = AliEMCALGetter::Instance() ; 

  Int_t nevents = gime->MaxEvent() ;      
  Int_t ievent ;


  for(ievent = 0; ievent < nevents; ievent++){
    gime->Event(ievent,"TR") ;
    if(gime->TrackSegments() && //Skip events, where no track segments made
       gime->TrackSegments()->GetEntriesFast()) {   
      MakeRecParticles() ;
      WriteRecParticles(ievent);
      if(strstr(option,"deb"))
	PrintRecParticles(option) ;
      //increment the total number of rec particles per run 
      fRecParticlesInRun += gime->RecParticles()->GetEntriesFast() ; 
    }
  }
  if(strstr(option,"tim")){
    gBenchmark->Stop("EMCALPID");
    Info("Exec", "took %f seconds for PID %f seconds per event", 
	 gBenchmark->GetCpuTime("EMCALPID"),  
	 gBenchmark->GetCpuTime("EMCALPID")/nevents) ;
  } 

  Unload();
}

//____________________________________________________________________________
void  AliEMCALPIDv1::MakeRecParticles(){

  // Makes a RecParticle out of a TrackSegment
  
  AliEMCALGetter * gime = AliEMCALGetter::Instance() ; 
  TObjArray * aECARecPoints = gime->ECARecPoints() ; 
  TObjArray * aPRERecPoints = gime->PRERecPoints() ; 
  TObjArray * aHCARecPoints = gime->HCARecPoints() ; 
  TClonesArray * trackSegments = gime->TrackSegments() ; 
  if ( !aECARecPoints || !aPRERecPoints || !aHCARecPoints || !trackSegments ) {
    Fatal("MakeRecParticles", "RecPoints or TrackSegments not found !") ;  
  }
  TClonesArray * recParticles  = gime->RecParticles() ; 
  recParticles->Clear();

  TIter next(trackSegments) ; 
  AliEMCALTrackSegment * ts ; 
  Int_t index = 0 ; 
  AliEMCALRecParticle * rp ; 
  while ( (ts = (AliEMCALTrackSegment *)next()) ) {
    
    new( (*recParticles)[index] ) AliEMCALRecParticle() ;
    rp = (AliEMCALRecParticle *)recParticles->At(index) ; 
    rp->SetTrackSegment(index) ;
    rp->SetIndexInList(index) ;
    	
    AliEMCALTowerRecPoint * eca = 0 ;
    if(ts->GetECAIndex()>=0)
      eca = dynamic_cast<AliEMCALTowerRecPoint *>(aECARecPoints->At(ts->GetECAIndex())) ;
    
    AliEMCALTowerRecPoint * pre = 0 ;
    if(ts->GetPREIndex()>=0)
      pre = dynamic_cast<AliEMCALTowerRecPoint *>(aPRERecPoints->At(ts->GetPREIndex())) ;
    
    AliEMCALTowerRecPoint * hca = 0 ;
    if(ts->GetHCAIndex()>=0)
      hca = dynamic_cast<AliEMCALTowerRecPoint *>(aHCARecPoints->At(ts->GetHCAIndex())) ;

    // Now set type (reconstructed) of the particle

    // Choose the cluster energy range
    
    if (!eca) {
      Fatal("MakeRecParticles", "-> emcal(%d) = %d", ts->GetECAIndex(), eca ) ;
    }

    Float_t    e = eca->GetEnergy() ;   
    
    Float_t  lambda[2] ;
    eca->GetElipsAxis(lambda) ;
    
    if((lambda[0]>0.01) && (lambda[1]>0.01)){
      // Looking PCA. Define and calculate the data (X),
      // introduce in the function X2P that gives the components (P).  

      Float_t  Spher = 0. ;
      Float_t  Emaxdtotal = 0. ; 
      
      if((lambda[0]+lambda[1])!=0) 
	Spher=fabs(lambda[0]-lambda[1])/(lambda[0]+lambda[1]); 
      
      Emaxdtotal=eca->GetMaximalEnergy()/eca->GetEnergy(); 
    }
    
    //    Float_t time = ecal->GetTime() ;
      
    //Set momentum, energy and other parameters 
    Float_t  enca = e;
    TVector3 dir(0., 0., 0.) ; 
    dir.SetMag(enca) ;
    rp->SetMomentum(dir.X(),dir.Y(),dir.Z(),enca) ;
    rp->SetCalcMass(0);
    rp->Name(); //If photon sets the particle pdg name to gamma
    rp->SetProductionVertex(0,0,0,0);
    rp->SetFirstMother(-1);
    rp->SetLastMother(-1);
    rp->SetFirstDaughter(-1);
    rp->SetLastDaughter(-1);
    rp->SetPolarisation(0,0,0);
    index++ ; 
  }
  
}

//____________________________________________________________________________
void  AliEMCALPIDv1:: Print()
{
  // Print the parameters used for the particle type identification

    Info("Print", "=============== AliEMCALPID1 ================") ;
    printf("Making PID\n");
    printf("    Pricipal analysis file from 0.5 to 100 %s\n", fFileName.Data() ) ; 
    printf("    Name of parameters file     %s\n", fFileNamePar.Data() )  ;
}

//____________________________________________________________________________
void AliEMCALPIDv1::PrintRecParticles(Option_t * option)
{
  // Print table of reconstructed particles

  AliEMCALGetter *gime = AliEMCALGetter::Instance() ; 

  TClonesArray * recParticles = gime->RecParticles() ; 

  TString message ; 
  message  = "\nevent " ;
  message += gAlice->GetEvNumber() ; 
  message += "       found " ; 
  message += recParticles->GetEntriesFast(); 
  message += " RecParticles\n" ; 

  if(strstr(option,"all")) {  // printing found TS
    message += "\n  PARTICLE         Index    \n" ; 
    
    Int_t index ;
    for (index = 0 ; index < recParticles->GetEntries() ; index++) {
      AliEMCALRecParticle * rp = (AliEMCALRecParticle * ) recParticles->At(index) ;       
      message += "\n" ;
      message += rp->Name().Data() ;  
      message += " " ;
      message += rp->GetIndexInList() ;  
      message += " " ;
      message += rp->GetType()  ;
    }
  }
  Info("Print", message.Data() ) ; 
}

//____________________________________________________________________________
void AliEMCALPIDv1::Unload() 
{
  AliEMCALGetter * gime = AliEMCALGetter::Instance() ;  
  gime->EmcalLoader()->UnloadRecPoints() ;
  gime->EmcalLoader()->UnloadTracks() ;
  gime->EmcalLoader()->UnloadRecParticles() ;
}

//____________________________________________________________________________
void  AliEMCALPIDv1::WriteRecParticles(Int_t event)
{
 
  AliEMCALGetter *gime = AliEMCALGetter::Instance() ; 

  TClonesArray * recParticles = gime->RecParticles() ; 
  recParticles->Expand(recParticles->GetEntriesFast() ) ;
  TTree * treeP =  gime->TreeP() ;

  
  
  //First rp
  Int_t bufferSize = 32000 ;    
  TBranch * rpBranch = treeP->Branch("EMCALRP",&recParticles,bufferSize);
  rpBranch->SetTitle(BranchName());

  rpBranch->Fill() ;
 
  gime->WriteRecParticles("OVERWRITE");
  gime->WritePID("OVERWRITE");

}


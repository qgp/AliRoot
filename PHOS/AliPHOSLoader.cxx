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

/* $Id:  */

/* $Log:
   29.05.2001 Yuri Kharlov:
              Everywhere reading the treese TTree->GetEvent(i)
              is replaced by reading the branches TBranch->GetEntry(0)
*/

//_________________________________________________________________________
//  A singleton. This class should be used in the analysis stage to get 
//  reconstructed objects: Digits, RecPoints, TrackSegments and RecParticles,
//  instead of directly reading them from galice.root file. This container 
//  ensures, that one reads Digits, made of these particular digits, RecPoints, 
//  made of these particular RecPoints, TrackSegments and RecParticles. 
//  This becomes non trivial if there are several identical branches, produced with
//  different set of parameters. 
//
//  An example of how to use (see also class AliPHOSAnalyser):
//  for(Int_t irecp = 0; irecp < gime->NRecParticles() ; irecp++)
//     AliPHOSRecParticle * part = gime->RecParticle(1) ;
//     ................
//  please->GetEvent(event) ;    // reads new event from galice.root
//                  
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)
//*--         Completely redesigned by Dmitri Peressounko March 2001  
//
//*-- YS June 2001 : renamed the original AliPHOSIndexToObject and make
//*--         systematic usage of TFolders without changing the interface        
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TFolder.h"
#include "TParticle.h"

// --- Standard library ---
#include <Riostream.h>

// --- AliRoot header files ---

#include "AliRun.h"
#include "AliConfig.h"
#include "AliDataLoader.h"
#include "AliPHOSLoader.h"
#include "AliPHOS.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSClusterizer.h"
#include "AliPHOSClusterizerv1.h"
#include "AliPHOSTrackSegmentMaker.h"
#include "AliPHOSTrackSegmentMakerv1.h"
#include "AliPHOSTrackSegment.h"
#include "AliPHOSPID.h" 
#include "AliPHOSPIDv1.h" 
#include "AliPHOSGeometry.h"
#include "AliPHOSCalibrationDB.h"

ClassImp(AliPHOSLoader)


const TString AliPHOSLoader::fgkHitsName("HITS");//Name for TClonesArray with hits from one event
const TString AliPHOSLoader::fgkSDigitsName("SDIGITS");//Name for TClonesArray 
const TString AliPHOSLoader::fgkDigitsName("DIGITS");//Name for TClonesArray 
const TString AliPHOSLoader::fgkEmcRecPointsName("EMCRECPOINTS");//Name for TClonesArray 
const TString AliPHOSLoader::fgkCpvRecPointsName("CPVRECPOINTS");//Name for TClonesArray 
const TString AliPHOSLoader::fgkTracksName("TRACKS");//Name for TClonesArray 
const TString AliPHOSLoader::fgkRecParticlesName("RECPARTICLES");//Name for TClonesArray

const TString AliPHOSLoader::fgkEmcRecPointsBranchName("PHOSEmcRP");//Name for branch with EMC Reconstructed Points
const TString AliPHOSLoader::fgkCpvRecPointsBranchName("PHOSCpvRP");//Name for branch with CPV Reconstructed Points
const TString AliPHOSLoader::fgkTrackSegmentsBranchName("PHOSTS");//Name for branch with TrackSegments
const TString AliPHOSLoader::fgkRecParticlesBranchName("PHOSRP");//Name for branch with Reconstructed Particles
//____________________________________________________________________________ 
AliPHOSLoader::AliPHOSLoader()
 {
  fDebug = 0;
  fRecParticlesLoaded = kFALSE;
 }
//____________________________________________________________________________ 
AliPHOSLoader::AliPHOSLoader(const Char_t *detname,const Char_t *eventfoldername):
      AliLoader(detname,eventfoldername)
{
  fDebug=0;
  fRecParticlesLoaded = kFALSE;
}
//____________________________________________________________________________ 

AliPHOSLoader::~AliPHOSLoader()
{
  //remove and delete arrays
  Clean(fgkHitsName);
  Clean(fgkSDigitsName);
  Clean(fgkDigitsName);
  Clean(fgkEmcRecPointsName);
  Clean(fgkCpvRecPointsName);
  Clean(fgkTracksName);
  Clean(fgkRecParticlesName);
}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanFolders()
 {
   CleanRecParticles();
   AliLoader::CleanFolders();
 }
//____________________________________________________________________________ 

Int_t AliPHOSLoader::SetEvent()
{
//Cleans loaded stuff and and sets Files and Directories
// do not post any data to folder/tasks


 Int_t retval = AliLoader::SetEvent();
  if (retval)
   {
     Error("SetEvent","AliLoader::SetEvent returned error");
     return retval;
   }


  if (Hits()) Hits()->Clear();
  if (SDigits()) SDigits()->Clear();
  if (Digits()) Digits()->Clear();
  if (EmcRecPoints()) EmcRecPoints()->Clear();
  if (CpvRecPoints()) CpvRecPoints()->Clear();
  if (TrackSegments()) TrackSegments()->Clear();
  if (RecParticles()) RecParticles()->Clear();
   
  return 0;
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::GetEvent()
{
//Overloads GetEvent method called by AliRunLoader::GetEvent(Int_t) method
//to add Rec Particles specific for PHOS

//First call the original method to get whatever from std. setup is needed
  Int_t retval;
  
  retval = AliLoader::GetEvent();
  if (retval)
   {
     Error("GetEvent","AliLoader::GetEvent returned error");
     return retval;
   }
  
  if (GetHitsDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadHits();
  if (GetSDigitsDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadSDigits();
  if (GetDigitsDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadDigits();
  if (GetRecPointsDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadRecPoints();
  if (GetTracksDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadTracks();
  if (GetRecParticlesDataLoader()->GetBaseDataLoader()->IsLoaded()) ReadRecParticles();


//Now, check if RecPart were loaded  
  return 0;
}
//____________________________________________________________________________ 


//____________________________________________________________________________ 
const AliPHOS * AliPHOSLoader::PHOS() 
{
  // returns the PHOS object 
  AliPHOS * phos = dynamic_cast<AliPHOS*>(GetModulesFolder()->FindObject(fDetectorName));
  if ( phos == 0x0) 
    if (fDebug)
      cout << "WARNING: AliPHOSLoader::PHOS -> PHOS module not found in Folders" << endl ; 
  return phos ; 
}  

//____________________________________________________________________________ 
const AliPHOSGeometry * AliPHOSLoader::PHOSGeometry() 
{
  AliPHOSGeometry * rv = 0 ; 
  if (PHOS() )
    rv =  PHOS()->GetGeometry();
  return rv ; 
} 


//____________________________________________________________________________ 
Int_t AliPHOSLoader::LoadHits(Option_t* opt)
{  
//------- Hits ----------------------
//Overload (extends) LoadHits implemented in AliLoader
//
  Int_t res;
  
  //First call the AliLoader's method to send the TreeH to folder
  res = AliLoader::LoadHits(opt);
  
  if (res)
   {//oops, error
     Error("LoadHits","AliLoader::LoadHits returned error");
     return res;
   }

  //read the data from tree in folder and send it to folder
  res = ReadHits();
  return 0;
}


//____________________________________________________________________________ 
Int_t AliPHOSLoader::LoadSDigits(Option_t* opt)
{  //---------- SDigits -------------------------
  Int_t res;
  //First call the AliLoader's method to send the TreeS to folder
  res = AliLoader::LoadSDigits(opt);
  if (res)
   {//oops, error
     Error("PostSDigits","AliLoader::LoadSDigits returned error");
     return res;
   }
  return ReadSDigits();
   
} 
//____________________________________________________________________________ 
Int_t AliPHOSLoader::LoadDigits(Option_t* opt)
{ 
  Int_t res;
  //First call the AliLoader's method to send the TreeS to folder
  res = AliLoader::LoadDigits(opt);
  if (res)
   {//oops, error
     Error("LoadDigits","AliLoader::LoadDigits returned error");
     return res;
   }
  return ReadDigits();
}
//____________________________________________________________________________ 
Int_t AliPHOSLoader::LoadRecPoints(Option_t* opt) 
{ // -------------- RecPoints -------------------------------------------
  Int_t res;
  //First call the AliLoader's method to send the TreeR to folder
  res = AliLoader::LoadRecPoints(opt);
  if (res)
   {//oops, error
     Error("LoadRecPoints","AliLoader::LoadRecPoints returned error");
     return res;
   }

  TFolder * phosFolder = GetDetectorDataFolder();
  if ( phosFolder  == 0x0 ) 
   {
     Error("PostDigits","Can not get detector data folder");
     return 1;
   }
  return ReadRecPoints();
}
//____________________________________________________________________________ 

Int_t  AliPHOSLoader::LoadTracks(Option_t* opt)
{
 //Loads Tracks: Open File, Reads Tree and posts, Read Data and Posts
 if (GetDebug()) Info("LoadTracks","opt = %s",opt);
 if (fTracksLoaded)
  {
    Warning("LoadTracks","Tracks are already loaded");
    return 0;
  }
 Int_t res;
  //First call the AliLoader's method to send the TreeS to folder
 if (GetTracksDataLoader()->GetBaseLoader(0)->IsLoaded() == kFALSE) 
  {//tracks can be loaded by LoadRecPoints
   res = AliLoader::LoadTracks(opt);
   if (res)
    {//oops, error
      Error("LoadTracks","AliLoader::LoadTracks returned error");
      return res;
    }
  }
 res = ReadTracks();
 if (res)
  {
    Error("LoadTracks","Error occured while reading Tracks");
    return res;
  }

 fTracksLoaded = kTRUE;
 return 0;
}

//____________________________________________________________________________ 
Int_t AliPHOSLoader::LoadRecParticles(Option_t* opt) 
{ // -------------- RecPoints -------------------------------------------
  Int_t res;
  //First call the AliLoader's method to send the TreeS to folder
  res = AliLoader::LoadRecParticles(opt);
  if (res)
   {//oops, error
     Error("LoadRecParticles","AliLoader::LoadRecParticles returned error");
     return res;
   }

  TFolder * phosFolder = GetDetectorDataFolder();
  if ( phosFolder  == 0x0 ) 
   {
     Error("PostDigits","Can not get detector data folder");
     return 1;
   }
  return ReadRecParticles();
}

//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostHits()
{
  Int_t reval = AliLoader::PostHits();
  if (reval)
   {
     Error("PostHits","AliLoader::  returned error");
     return reval;
   }
  return ReadHits();
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostSDigits()
{
  Int_t reval = AliLoader::PostSDigits();
  if (reval)
   {
     Error("PostSDigits","AliLoader::PostSDigits  returned error");
     return reval;
   }
  return ReadSDigits();
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostDigits()
{
  Int_t reval = AliLoader::PostDigits();
  if (reval)
   {
     Error("PostDigits","AliLoader::PostDigits  returned error");
     return reval;
   }
  return ReadDigits();
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostRecPoints()
{
  Int_t reval = AliLoader::PostRecPoints();
  if (reval)
   {
     Error("PostRecPoints","AliLoader::PostRecPoints  returned error");
     return reval;
   }
  return ReadRecPoints();
}

//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostRecParticles()
{
  Int_t reval = AliLoader::PostRecParticles();
  if (reval)
   {
     Error("PostRecParticles","AliLoader::PostRecParticles  returned error");
     return reval;
   }
  return ReadRecParticles();
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::PostTracks()
{
  Int_t reval = AliLoader::PostTracks();
  if (reval)
   {
     Error("PostTracks","AliLoader::PostTracks  returned error");
     return reval;
   }
  return ReadTracks();
}
//____________________________________________________________________________ 



//____________________________________________________________________________ 
Int_t AliPHOSLoader::ReadHits()
{
// If there is no Clones Array in folder creates it and sends to folder
// then tries to read
// Reads the first entry of PHOS branch in hit tree TreeH()
// Reads data from TreeH and stores it in TClonesArray that sits in DetectorDataFolder
//
  TObject** hitref = HitsRef();
  if(hitref == 0x0)
   {
     MakeHitsArray();
     hitref = HitsRef();
   }

  TClonesArray* hits = dynamic_cast<TClonesArray*>(*hitref);

  TTree* treeh = TreeH();
  
  if(treeh == 0)
   {
    Error("ReadHits"," Cannot read TreeH from folder");
    return 1;
  }
  
  TBranch * hitsbranch = treeh->GetBranch(fDetectorName);
  if (hitsbranch == 0) 
   {
    Error("ReadHits"," Cannot find branch PHOS"); 
    return 1;
  }
  
  if (GetDebug()) Info("ReadHits","Reading Hits");
  
  if (hitsbranch->GetEntries() > 1)
   {
    TClonesArray * tempo =  new TClonesArray("AliPHOSHit",1000);

    hitsbranch->SetAddress(&tempo);
    Int_t index = 0 ; 
    Int_t i = 0 ;
    for (i = 0 ; i < hitsbranch->GetEntries(); i++) 
     {
      hitsbranch->GetEntry(i) ;
      Int_t j = 0 ;
      for ( j = 0 ; j < tempo->GetEntries() ; j++) 
       {
         AliPHOSHit* hit = (AliPHOSHit*)tempo->At(j); 
         new((*hits)[index]) AliPHOSHit( *hit ) ;
         index++ ; 
       }
     }
    delete tempo;
   }
  else 
   {
    hitsbranch->SetAddress(hitref);
    hitsbranch->GetEntry(0) ;
   }

  return 0;
}
//____________________________________________________________________________ 
Int_t AliPHOSLoader::ReadSDigits()
{
// Read the summable digits tree TreeS():
// Check if TClones is in folder
// if not create and add to folder
// connect to tree if available
// Read the data

  TObject** sdref = SDigitsRef();
  if(sdref == 0x0)
   {
     MakeSDigitsArray();
     sdref = SDigitsRef();
   }
   
  TTree * treeS = TreeS();
  if(treeS==0)
   {
     //May happen if file is truncated or new in LoadSDigits
     //Error("ReadSDigits","There is no SDigit Tree");
     return 0;
   }
  
  TBranch * branch = treeS->GetBranch(fDetectorName);
  if (branch == 0) 
   {//easy, maybe just a new tree
    //Error("ReadSDigits"," Cannot find branch PHOS"); 
    return 0;
  }
    
  branch->SetAddress(SDigitsRef());
  branch->GetEntry(0);
  return 0;
}

//____________________________________________________________________________ 
Int_t AliPHOSLoader::ReadDigits()
{
// Read the summable digits tree TreeS():
// Check if TClones is in folder
// if not create and add to folder
// connect to tree if available
// Read the data
  
  TObject** dref = DigitsRef();
  if(dref == 0x0)
   {//if there is not array in folder, create it and put it there
     MakeDigitsArray();
     dref = DigitsRef();
   }

  TTree * treeD = TreeD();
  if(treeD==0)
   {
     //May happen if file is truncated or new in LoadSDigits
     //Error("ReadDigits","There is no Digit Tree");
     return 0;
   }

  TBranch * branch = treeD->GetBranch(fDetectorName);
  if (branch == 0) 
   {//easy, maybe just a new tree
    //Error("ReadDigits"," Cannot find branch ",fDetectorName.Data()); 
    return 0;
   }
  
  branch->SetAddress(dref);//connect branch to buffer sitting in folder
  branch->GetEntry(0);//get first event 

  return 0;  
}
//____________________________________________________________________________ 

void AliPHOSLoader::UnloadRecParticles()
{
  fRecParticlesLoaded = kFALSE;
  CleanRecParticles();
  if (fTracksLoaded == kFALSE) UnloadTracks();
}
//____________________________________________________________________________ 

void AliPHOSLoader::UnloadTracks()
{
 CleanTracks();//free the memory
 //in case RecPart are loaded we can not onload tree and close the file
 if (fRecParticlesLoaded == kFALSE) AliLoader::UnloadTracks();
 fTracksLoaded = kFALSE;//mark that nobody needs them
}
//____________________________________________________________________________ 

void AliPHOSLoader::Track(Int_t itrack)
{
// Read the first entry of PHOS branch in hit tree gAlice->TreeH()
  if(TreeH()== 0)
   {
     if (LoadHits())
      {
        Error("Track","Can not load hits.");
        return;
      } 
   }
  
  TBranch * hitsbranch = dynamic_cast<TBranch*>(TreeH()->GetListOfBranches()->FindObject("PHOS")) ;
  if ( !hitsbranch ) {
    if (fDebug)
      cout << "WARNING:  AliPHOSLoader::ReadTreeH -> Cannot find branch PHOS" << endl ; 
    return ;
  }  
  if(!Hits()) PostHits();

  hitsbranch->SetAddress(HitsRef());
  hitsbranch->GetEntry(itrack);

}
//____________________________________________________________________________ 
void AliPHOSLoader::ReadTreeQA()
{
  // Read the digit tree gAlice->TreeQA()
  // so far only PHOS knows about this Tree  

  if(PHOS()->TreeQA()== 0){
    cerr <<   "ERROR: AliPHOSLoader::ReadTreeQA: can not read TreeQA " << endl ;
    return ;
  }
  
  TBranch * qabranch = PHOS()->TreeQA()->GetBranch("PHOS");
  if (!qabranch) { 
    if (fDebug)
      cout << "WARNING: AliPHOSLoader::ReadTreeQA -> Cannot find QA Alarms for PHOS" << endl ;
    return ; 
  }   
  
//  if(!Alarms()) PostQA();

  qabranch->SetAddress(AlarmsRef()) ;

  qabranch->GetEntry(0) ;
  
}


//____________________________________________________________________________ 
Int_t AliPHOSLoader::ReadRecPoints()
{
//Creates and posts to folder an array container, 
//connects branch in tree (if exists), and reads data to array
  
  MakeRecPointsArray();
  
  TObjArray * cpva = 0x0 ; 
  TObjArray * emca = 0x0 ; 
  
  TTree * treeR = TreeR();
 
  if(treeR==0)
   {
     //May happen if file is truncated or new in LoadSDigits
     return 0;
   }

  Int_t retval = 0;
  TBranch * emcbranch = treeR->GetBranch(fgkEmcRecPointsBranchName);

  if (emcbranch == 0x0)
   {
     Error("ReadRecPoints","Can not get branch with EMC Rec. Points named %s",fgkEmcRecPointsBranchName.Data());
     retval = 1;
   }
  else
   {
     emcbranch->SetAddress(&emca) ;
     emcbranch->GetEntry(0) ;
   }
  TBranch * cpvbranch = treeR->GetBranch(fgkCpvRecPointsBranchName);
  if (cpvbranch == 0x0)
   {
     Error("ReadRecPoints","Can not get branch with CPV Rec. Points named %s",fgkCpvRecPointsBranchName.Data());
     retval = 2;
   }
  else
   {
     cpvbranch->SetAddress(&cpva);
     cpvbranch->GetEntry(0) ;
   }

  Int_t ii ; 
  Int_t maxemc = emca->GetEntries() ; 
  for ( ii= 0 ; ii < maxemc ; ii++ ) 
    EmcRecPoints()->Add(emca->At(ii)) ;
 
  Int_t maxcpv = cpva->GetEntries() ;
  for ( ii= 0 ; ii < maxcpv ; ii++ )
    CpvRecPoints()->Add(cpva->At(ii)) ; 

  return retval;
}

//____________________________________________________________________________ 
Int_t AliPHOSLoader::ReadTracks()
{
//Creates and posts to folder an array container, 
//connects branch in tree (if exists), and reads data to arry

  TObject** trkref = TracksRef();
  if ( trkref == 0x0 )   
   {//Create and post array
    MakeTrackSegmentsArray();
    trkref = TracksRef();
   }

  TTree * treeT = TreeT();
  if(treeT==0)
   {
     //May happen if file is truncated or new in LoadSDigits, or the file is in update mode, 
     //but tracking was not performed yet for a current event
     //Error("ReadTracks","There is no Tree with Tracks");
     return 0;
   }
  
  TBranch * branch = treeT->GetBranch(fgkTrackSegmentsBranchName);
  if (branch == 0) 
   {//easy, maybe just a new tree
    Error("ReadTracks"," Cannot find branch named %s",fgkTrackSegmentsBranchName.Data());
    return 0;
  }

  branch->SetAddress(trkref);//connect branch to buffer sitting in folder
  branch->GetEntry(0);//get first event 
  
  return 0;
}
//____________________________________________________________________________ 

Int_t AliPHOSLoader::ReadRecParticles()
{
//Reads Reconstructed  Particles from file
//Creates and posts to folder an array container, 
//connects branch in tree (if exists), and reads data to arry
  
  TObject** recpartref = RecParticlesRef();
  
  if ( recpartref == 0x0 )   
   {//Create and post array
    MakeRecParticlesArray();
    recpartref = RecParticlesRef();
   }

  TTree * treeP = TreeP();
  if(treeP==0)
   {
     //May happen if file is truncated or new in LoadSDigits, 
     //or the file is in update mode, 
     //but tracking was not performed yet for a current event
     //     Error("ReadRecParticles","There is no Tree with Tracks and Reconstructed Particles");
     return 0;
   }
  
  TBranch * branch = treeP->GetBranch(fgkRecParticlesBranchName);
  if (branch == 0) 
   {//easy, maybe just a new tree
    Error("ReadRecParticles"," Cannot find branch %s",fgkRecParticlesBranchName.Data()); 
    return 0;
  }

  branch->SetAddress(recpartref);//connect branch to buffer sitting in folder
  branch->GetEntry(0);//get first event 
  
  return 0;
}


AliPHOSGeometry* AliPHOSLoader::GetPHOSGeometry()
{
//returns PHOS geometry from gAlice 
//static Method used by some classes where it is not convienient to pass eventfoldername
 if (gAlice == 0x0)
  return 0x0;
 AliPHOS* phos=dynamic_cast<AliPHOS*>(gAlice->GetDetector("PHOS"));
 if (phos == 0x0)
  return 0x0;
 return phos->GetGeometry();
}
/***************************************************************************************/

AliPHOSLoader* AliPHOSLoader::GetPHOSLoader(const  char* eventfoldername)
{
  AliRunLoader* rn  = AliRunLoader::GetRunLoader(eventfoldername);
  if (rn == 0x0)
   {
     cerr<<"Error: <AliPHOSLoader::GetPHOSLoader>: "
         << "Can not find Run Loader in folder "<<eventfoldername<<endl;
     return 0x0;
   }
  return dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
}
/***************************************************************************************/

Bool_t AliPHOSLoader::BranchExists(const TString& recName)
 {
  if (fBranchTitle.IsNull()) return kFALSE;
  TString dataname, zername ;
  TTree* tree;
  if(recName == "SDigits")
   {
    tree = TreeS();
    dataname = GetDetectorName();
    zername = "AliPHOSSDigitizer" ;
   }
  else
    if(recName == "Digits"){
      tree = TreeD();
      dataname = GetDetectorName();
      zername = "AliPHOSDigitizer" ;
    }
    else
      if(recName == "RecPoints"){
       tree = TreeR();
       dataname = fgkEmcRecPointsBranchName;
       zername = "AliPHOSClusterizer" ;
      }
      else
       if(recName == "TrackSegments"){
         tree = TreeT();
         dataname = fgkTrackSegmentsBranchName;
         zername = "AliPHOSTrackSegmentMaker";
       }        
       else
         if(recName == "RecParticles"){
           tree = TreeP();
           dataname = fgkRecParticlesBranchName;
           zername = "AliPHOSPID";
         }
         else
           return kFALSE ;

  
  if(!tree ) 
    return kFALSE ;

  TObjArray * lob = static_cast<TObjArray*>(tree->GetListOfBranches()) ;
  TIter next(lob) ; 
  TBranch * branch = 0 ;  
  TString titleName(fBranchTitle);
  titleName+=":";

  while ((branch = (static_cast<TBranch*>(next())))) {
    TString branchName(branch->GetName() ) ; 
    TString branchTitle(branch->GetTitle() ) ;  
    if ( branchName.BeginsWith(dataname) && branchTitle.BeginsWith(fBranchTitle) ){  
      Warning("BranchExists","branch %s  with title  %s ",dataname.Data(),fBranchTitle.Data());
      return kTRUE ;
    }
    if ( branchName.BeginsWith(zername) &&  branchTitle.BeginsWith(titleName) ){
      Warning("BranchExists","branch AliPHOS... with title  %s ",branch->GetTitle());
      return kTRUE ; 
    }
  }
  return kFALSE ;

 }

void AliPHOSLoader::SetBranchTitle(const TString& btitle)
 {
  if (btitle.CompareTo(fBranchTitle) == 0) return;
  fBranchTitle = btitle;
  ReloadAll();
 }
//____________________________________________________________________________ 

void AliPHOSLoader::CleanHits()
{
  AliLoader::CleanHits();
  //Clear an array 
  TClonesArray* hits = Hits();
  if (hits) hits->Clear();
}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanSDigits()
{
  AliLoader::CleanSDigits();
  TClonesArray* sdigits = SDigits();
  if (sdigits) sdigits->Clear();
  
}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanDigits()
{
  AliLoader::CleanDigits();
  TClonesArray* digits = Digits();
  if (digits) digits->Clear();
}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanRecPoints()
{
  AliLoader::CleanRecPoints();
  TObjArray* recpoints = EmcRecPoints();
  if (recpoints) recpoints->Clear();
  recpoints = CpvRecPoints();
  if (recpoints) recpoints->Clear();
}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanTracks()
{
//Cleans Tracks stuff
  
  AliLoader::CleanTracks();//tree
  
  //and clear the array
  TClonesArray* tracks = TrackSegments();
  if (tracks) tracks->Clear();

}
//____________________________________________________________________________ 

void AliPHOSLoader::CleanRecParticles()
 {

   TClonesArray *recpar = RecParticles();
   if (recpar) recpar->Clear();
  
 
 }
//____________________________________________________________________________ 

void AliPHOSLoader::ReadCalibrationDB(const char * database,const char * filename)
{

  if(fcdb && (strcmp(database,fcdb->GetTitle())==0))
    return ;

  TFile * file = gROOT->GetFile(filename) ;
  if(!file)
    file = TFile::Open(filename);
  if(!file){
    Error ("ReadCalibrationDB", "Cannot open file %s", filename) ;
    return ;
  }
  if(fcdb)
    fcdb->Delete() ;
  fcdb = dynamic_cast<AliPHOSCalibrationDB *>(file->Get("AliPHOSCalibrationDB")) ;
  if(!fcdb)
    Error ("ReadCalibrationDB", "No database %s in file %s", database, filename) ;
}
//____________________________________________________________________________ 

// AliPHOSSDigitizer*  AliPHOSLoader::PHOSSDigitizer() 
// { 
// //return PHOS SDigitizer
//  return  dynamic_cast<AliPHOSSDigitizer*>(SDigitizer()) ;
// }

//____________________________________________________________________________ 
void AliPHOSLoader::MakeHitsArray()
{
  if (Hits()) return;
  TClonesArray* hits = new TClonesArray("AliPHOSHit",1000);
  hits->SetName(fgkHitsName);
  GetDetectorDataFolder()->Add(hits);
}

//____________________________________________________________________________ 
void AliPHOSLoader::MakeSDigitsArray()
{
  if ( SDigits()) return;
  TClonesArray* sdigits = new TClonesArray("AliPHOSDigit",1);
  sdigits->SetName(fgkSDigitsName);
  GetDetectorDataFolder()->Add(sdigits);
}

//____________________________________________________________________________ 
void AliPHOSLoader::MakeDigitsArray()
{
  if ( Digits()) return;
  TClonesArray* digits = new TClonesArray("AliPHOSDigit",1);
  digits->SetName(fgkDigitsName);
  GetDetectorDataFolder()->Add(digits);
  
}

//____________________________________________________________________________ 
void AliPHOSLoader::MakeRecPointsArray()
{
  if ( EmcRecPoints() == 0x0)
   {
    if (GetDebug()>9) Info("MakeRecPointsArray","Making array for EMC");
    TObjArray* emc = new TObjArray(100) ;
    emc->SetName(fgkEmcRecPointsName) ;
    GetDetectorDataFolder()->Add(emc);
   }

  if ( CpvRecPoints() == 0x0)
   {
    if (GetDebug()>9) Info("MakeRecPointsArray","Making array for CPV");
    TObjArray* cpv = new TObjArray(100) ;
    cpv->SetName(fgkCpvRecPointsName);
    GetDetectorDataFolder()->Add(cpv);
   }
}

//____________________________________________________________________________ 
void AliPHOSLoader::MakeTrackSegmentsArray()
{
  if ( TrackSegments()) return;
  TClonesArray * ts = new TClonesArray("AliPHOSTrackSegment",100) ;
  ts->SetName(fgkTracksName);
  GetDetectorDataFolder()->Add(ts);

}

//____________________________________________________________________________ 
void AliPHOSLoader::MakeRecParticlesArray()
{
  if ( RecParticles()) return;
  TClonesArray * rp = new TClonesArray("AliPHOSRecParticle",100) ;
  rp->SetName(fgkRecParticlesName);
  GetDetectorDataFolder()->Add(rp);
}

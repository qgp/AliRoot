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
/* $Log:
   08.2002 Dmitri Peressounko:

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
//  AliPHOSGetter * gime = AliPHOSGetter::GetInstance("galice.root","test") ;
//  for(Int_t irecp = 0; irecp < gime->NRecParticles() ; irecp++)
//     AliPHOSRecParticle * part = gime->RecParticle(1) ;
//     ................
//  gime->Event(event) ;    // reads new event from galice.root
//                  
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)
//*--         Completely redesigned by Dmitri Peressounko March 2001  
//
//*-- YS June 2001 : renamed the original AliPHOSIndexToObject and make
//*--         systematic usage of TFolders without changing the interface        
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---

#include "TSystem.h"
#include "TFile.h"
// #include "TTree.h"
#include "TROOT.h"
// #include "TObjString.h"
// #include "TFolder.h"
// #include "TParticle.h"

// --- Standard library ---

// --- AliRoot header files ---
// #include "AliRun.h"
// #include "AliConfig.h"
#include "AliPHOSGetter.h"
#include "AliRunLoader.h"
#include "AliStack.h"  
#include "AliPHOSLoader.h"
// #include "AliPHOS.h"
// #include "AliPHOSDigitizer.h"
// #include "AliPHOSSDigitizer.h"
// #include "AliPHOSClusterizerv1.h"
// #include "AliPHOSTrackSegmentMakerv1.h"
// #include "AliPHOSTrackSegment.h"
// #include "AliPHOSPIDv1.h" 
// #include "AliPHOSGeometry.h"
// #include "AliPHOSRaw2Digits.h"
//#include "AliPHOSCalibrationDB.h"
#include "AliPHOSBeamTestEvent.h"

ClassImp(AliPHOSGetter)
  
AliPHOSGetter * AliPHOSGetter::fgObjGetter = 0 ; 
AliPHOSLoader * AliPHOSGetter::fgPhosLoader = 0;
Int_t AliPHOSGetter::fgDebug = 0;

//  TFile * AliPHOSGetter::fgFile = 0 ; 

//____________________________________________________________________________ 
AliPHOSGetter::AliPHOSGetter(const char* headerFile, const char* version, Option_t * openingOption)
{
  // ctor only called by Instance()

  AliRunLoader* rl = AliRunLoader::GetRunLoader(version) ; 
  if (!rl) {
    rl = AliRunLoader::Open(headerFile, version, openingOption);
    if (!rl) {
      Fatal("AliPHOSGetter", "Could not find the Run Loader for %s - %s",headerFile, version) ; 
      return ;
    } 
    if (rl->GetAliRun() == 0x0) {
      rl->LoadgAlice();
      gAlice = rl->GetAliRun(); // should be removed
    }
  }
  fgPhosLoader = dynamic_cast<AliPHOSLoader*>(rl->GetLoader("PHOSLoader"));
  if ( !fgPhosLoader ) 
    Error("AliPHOSGetter", "Could not find PHOSLoader") ; 
  else 
    fgPhosLoader->SetTitle(version);
  
  
  // initialize data members
  SetDebug(0) ; 
  fBTE = 0 ; 
  fPrimaries = 0 ; 
  fLoadingStatus = "" ; 
}

//____________________________________________________________________________ 
AliPHOSGetter::~AliPHOSGetter()
{
  // dtor
  delete fgPhosLoader ;
  fgPhosLoader = 0 ;
  delete fBTE ; 
  fBTE = 0 ; 
  fPrimaries->Delete() ; 
  delete fPrimaries ; 
}

//____________________________________________________________________________ 
TObjArray * AliPHOSGetter::CpvRecPoints() 
{
  // asks the Loader to return the CPV RecPoints container 

  TObjArray * rv = 0 ; 
  
  rv = PhosLoader()->CpvRecPoints() ; 
  if (!rv) {
    PhosLoader()->MakeRecPointsArray() ;
    rv = PhosLoader()->CpvRecPoints() ; 
  }
  return rv ; 
}

//____________________________________________________________________________ 
TClonesArray * AliPHOSGetter::Digits() 
{
  // asks the Loader to return the Digits container 

  TClonesArray * rv = 0 ; 
  rv = PhosLoader()->Digits() ; 

  if( !rv ) {
    PhosLoader()->MakeDigitsArray() ; 
    rv = PhosLoader()->Digits() ;
  }
  return rv ; 
}

//____________________________________________________________________________ 
TObjArray * AliPHOSGetter::EmcRecPoints() 
{
  // asks the Loader to return the EMC RecPoints container 

  TObjArray * rv = 0 ; 
  
  rv = PhosLoader()->EmcRecPoints() ; 
  if (!rv) {
    PhosLoader()->MakeRecPointsArray() ;
    rv = PhosLoader()->EmcRecPoints() ; 
  }
  return rv ; 
}

//____________________________________________________________________________ 
TClonesArray * AliPHOSGetter::TrackSegments() 
{
  // asks the Loader to return the TrackSegments container 

  TClonesArray * rv = 0 ; 
  
  rv = PhosLoader()->TrackSegments() ; 
  if (!rv) {
    PhosLoader()->MakeTrackSegmentsArray() ;
    rv = PhosLoader()->TrackSegments() ; 
  }
  return rv ; 
}
//____________________________________________________________________________ 
TClonesArray * AliPHOSGetter::RecParticles() 
{
  // asks the Loader to return the TrackSegments container 

  TClonesArray * rv = 0 ; 
  
  rv = PhosLoader()->RecParticles() ; 
  if (!rv) {
    PhosLoader()->MakeRecParticlesArray() ;
    rv = PhosLoader()->RecParticles() ; 
  }
  return rv ; 
}
//____________________________________________________________________________ 
void AliPHOSGetter::Event(const Int_t event, const char* opt) 
{
  // Reads the content of all Tree's S, D and R

  if ( event >= MaxEvent() ) {
    Error("Event", "%d not found in TreeE !", event) ; 
    return ; 
  }

  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());

  // checks if we are dealing with test-beam data
  TBranch * btb = rl->TreeE()->GetBranch("AliPHOSBeamTestEvent") ;
  if(btb){
    if(!fBTE)
      fBTE = new AliPHOSBeamTestEvent() ;
    btb->SetAddress(&fBTE) ;
    btb->GetEntry(event) ;
  }
  else{
    if(fBTE){
      delete fBTE ;
      fBTE = 0 ;
    }
  }

  // Loads the type of object(s) requested
  
  rl->GetEvent(event) ;

  if( strstr(opt,"X") || (strcmp(opt,"")==0) )
    ReadPrimaries() ;

  if(strstr(opt,"H") )
    ReadTreeH();

  if(strstr(opt,"S") )
    ReadTreeS() ;

  if( strstr(opt,"D") )
    ReadTreeD() ;

  if( strstr(opt,"R") )
    ReadTreeR() ;

  if( strstr(opt,"T") )
    ReadTreeT() ;

  if( strstr(opt,"P") )
    ReadTreeP() ;

//   if( strstr(opt,"Q") )
//     ReadTreeQA() ;
 
}


//____________________________________________________________________________ 
Int_t AliPHOSGetter::EventNumber() const
  {
  // return the current event number
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  return static_cast<Int_t>(rl->GetEventNumber()) ;   
}

//____________________________________________________________________________ 
  TClonesArray * AliPHOSGetter::Hits()  
{
  // asks the loader to return  the Hits container 
  
  TClonesArray * rv = 0 ; 
  
  rv = PhosLoader()->Hits() ; 
  if ( !rv ) {
    PhosLoader()->LoadHits("read"); 
    rv = PhosLoader()->Hits() ; 
  }
  return rv ; 
}

//____________________________________________________________________________ 
AliPHOSGetter * AliPHOSGetter::Instance(const char* alirunFileName, const char* version, Option_t * openingOption) 
{
  // Creates and returns the pointer of the unique instance
  // Must be called only when the environment has changed
  
  //::Info("Instance","alirunFileName=%s version=%s openingOption=%s",alirunFileName,version,openingOption);
  
  if(!fgObjGetter){ // first time the getter is called 
    fgObjGetter = new AliPHOSGetter(alirunFileName, version, openingOption) ;
  }
  else { // the getter has been called previously
    AliRunLoader * rl = AliRunLoader::GetRunLoader(fgPhosLoader->GetTitle());
    if ( rl->GetFileName() == alirunFileName ) {// the alirunFile has the same name
      // check if the file is already open
      TFile * galiceFile = dynamic_cast<TFile *>(gROOT->FindObject(rl->GetFileName()) ) ; 
      
      if ( !galiceFile ) 
	fgObjGetter = new AliPHOSGetter(alirunFileName, version, openingOption) ;
      
      else {  // the file is already open check the version name
	TString currentVersionName = rl->GetEventFolder()->GetName() ; 
	TString newVersionName(version) ; 
	if (currentVersionName == newVersionName) 
	  if(fgDebug)
	    ::Warning( "Instance", "Files with version %s already open", currentVersionName.Data() ) ;  
	else {
	  fgObjGetter = new AliPHOSGetter(alirunFileName, version, openingOption) ;      
	}
      }
    }
    else 
      fgObjGetter = new AliPHOSGetter(alirunFileName, version, openingOption) ;      
  }
  if (!fgObjGetter) 
    ::Error("Instance", "Failed to create the PHOS Getter object") ;
  else 
    if (fgDebug)
      Print() ;
  
  return fgObjGetter ;
}

//____________________________________________________________________________ 
AliPHOSGetter *  AliPHOSGetter::Instance()
{
  // Returns the pointer of the unique instance already defined
  
  if(!fgObjGetter)
     ::Error("Instance", "Getter not initialized") ;

   return fgObjGetter ;
           
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::MaxEvent() const 
{
  // returns the number of events in the run (from TE)

  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  return static_cast<Int_t>(rl->GetNumberOfEvents()) ; 
}

//____________________________________________________________________________ 
TParticle * AliPHOSGetter::Primary(Int_t index) const
{
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  return rl->Stack()->Particle(index) ; 
} 

//____________________________________________________________________________ 
AliPHOS * AliPHOSGetter:: PHOS() const  
{
  // returns the PHOS object 
  AliPHOS * phos = dynamic_cast<AliPHOS*>(PhosLoader()->GetModulesFolder()->FindObject("PHOS")) ;  
  if (!phos) 
    if (fgDebug)
      Warning("PHOS", "PHOS module not found in module folders: %s", PhosLoader()->GetModulesFolder()->GetName() ) ; 
  return phos ; 
}  

//____________________________________________________________________________ 
AliPHOSGeometry * AliPHOSGetter::PHOSGeometry() const 
{
  // Returns PHOS geometry

  AliPHOSGeometry * rv = 0 ; 
  if (PHOS() )
    rv =  PHOS()->GetGeometry() ;
  return rv ; 
} 

//____________________________________________________________________________ 
TClonesArray * AliPHOSGetter::Primaries()  
{
  // creates the Primaries container if needed
  if ( !fPrimaries ) {
    if (fgDebug) 
      Info("Primaries", "Creating a new TClonesArray for primaries") ; 
    fPrimaries = new TClonesArray("TParticle", 1000) ;
  } 
  return fPrimaries ; 
}

//____________________________________________________________________________ 
void  AliPHOSGetter::Print() 
{
  // Print usefull information about the getter
    
  AliRunLoader * rl = AliRunLoader::GetRunLoader(fgPhosLoader->GetTitle());
  ::Info( "Print", "gAlice file is %s -- version name is %s", (rl->GetFileName()).Data(), rl->GetEventFolder()->GetName() ) ; 
}

//____________________________________________________________________________ 
void AliPHOSGetter::ReadPrimaries()  
{
  // Read Primaries from Kinematics.root
  
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  
  // gets kine tree from the root file (Kinematics.root)
  if ( ! rl->TreeK() )  // load treeK the first time
    rl->LoadKinematics() ;
  
  fNPrimaries = rl->Stack()->GetNtrack() ; 

  if (fgDebug) 
    Info( "ReadTreeK", "Found %d particles in event # %d", fNPrimaries, EventNumber() ) ; 


  // first time creates the container
  if ( Primaries() ) 
    fPrimaries->Clear() ; 
  
  Int_t index = 0 ; 
  for (index = 0 ; index < fNPrimaries; index++) { 
    new ((*fPrimaries)[index]) TParticle(*(Primary(index)));
  }
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeD()
{
  // Read the Digits
  
  
  // gets TreeD from the root file (PHOS.SDigits.root)
  if ( !IsLoaded("D") ) {
    PhosLoader()->LoadDigits("UPDATE") ;
    SetLoaded("D") ; 
  } 
  return Digits()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeH()
{
  // Read the Hits
    
  // gets TreeH from the root file (PHOS.Hit.root)
  if ( !IsLoaded("H") ) {
    PhosLoader()->LoadHits("UPDATE") ;
    SetLoaded("H") ; 
  }  
  return Hits()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeR()
{
  // Read the RecPoints
  
  
  // gets TreeR from the root file (PHOS.RecPoints.root)
  if ( !IsLoaded("R") ) {
    PhosLoader()->LoadRecPoints("UPDATE") ;
    SetLoaded("R") ; 
  }

  return EmcRecPoints()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeT()
{
  // Read the TrackSegments
  
  
  // gets TreeT from the root file (PHOS.TrackSegments.root)
  if ( !IsLoaded("T") ) {
    PhosLoader()->LoadTracks("UPDATE") ;
    SetLoaded("T") ; 
  }

  return TrackSegments()->GetEntries() ; 
}
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeP()
{
  // Read the TrackSegments
  
  
  // gets TreeT from the root file (PHOS.TrackSegments.root)
  if ( !IsLoaded("P") ) {
    PhosLoader()->LoadRecParticles("UPDATE") ;
    SetLoaded("P") ; 
  }

  return RecParticles()->GetEntries() ; 
}
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeS()
{
  // Read the SDigits
  
  
  // gets TreeS from the root file (PHOS.SDigits.root)
  if ( !IsLoaded("S") ) {
    PhosLoader()->LoadSDigits("UPDATE") ;
    SetLoaded("S") ; 
  }

  return SDigits()->GetEntries() ; 
}

//____________________________________________________________________________ 
TClonesArray * AliPHOSGetter::SDigits() 
{
  // asks the Loader to return the Digits container 

  TClonesArray * rv = 0 ; 
  
  rv = PhosLoader()->SDigits() ; 
  if (!rv) {
    PhosLoader()->MakeSDigitsArray() ;
    rv = PhosLoader()->SDigits() ; 
  }
  return rv ; 
}

//____________________________________________________________________________ 
TParticle * AliPHOSGetter::Secondary(const TParticle* p, const Int_t index) const
{
  // Return first (index=1) or second (index=2) secondary particle of primary particle p 

  if(index <= 0) 
    return 0 ;
  if(index > 2)
    return 0 ;

  if(p) {
  Int_t daughterIndex = p->GetDaughter(index-1) ; 
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  return  rl->GetAliRun()->Particle(daughterIndex) ; 
  }
  else
    return 0 ;
}

//____________________________________________________________________________ 
void AliPHOSGetter::Track(const Int_t itrack) 
{
  // Read the first entry of PHOS branch in hit tree gAlice->TreeH()
 
 AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());

  if( !TreeH() ) // load treeH the first time
    rl->LoadHits() ;

  // first time create the container
  TClonesArray * hits = Hits() ; 
  if ( hits ) 
    hits->Clear() ; 

  TBranch * phosbranch = dynamic_cast<TBranch*>(TreeH()->GetBranch("PHOS")) ; 
  phosbranch->SetAddress(&hits) ;
  phosbranch->GetEntry(itrack) ;
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeD() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeD() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("D");
    rv = PhosLoader()->TreeD() ;
  } 
  
  return rv ; 
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeH() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeH() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("H");
    rv = PhosLoader()->TreeH() ;
  } 
  
  return rv ; 
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeR() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeR() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("R");
    rv = PhosLoader()->TreeR() ;
  } 
  
  return rv ; 
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeT() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeT() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("T");
    rv = PhosLoader()->TreeT() ;
  } 
  
  return rv ; 
}
//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeP() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeP() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("P");
    rv = PhosLoader()->TreeP() ;
  } 
  
  return rv ; 
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeS() const 
{
  TTree * rv = 0 ; 
  rv = PhosLoader()->TreeS() ; 
  if ( !rv ) {
    PhosLoader()->MakeTree("S");
    rv = PhosLoader()->TreeS() ;
  } 
  
  return rv ; 
}

//____________________________________________________________________________ 
Bool_t AliPHOSGetter::VersionExists(TString & opt) const
{
  // checks if the version with the present name already exists in the same directory

  Bool_t rv = kFALSE ;
 
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  TString version( rl->GetEventFolder()->GetName() ) ; 

  opt.ToLower() ; 
  
  if ( opt == "sdigits") {
    // add the version name to the root file name
    TString fileName( PhosLoader()->GetSDigitsFileName() ) ; 
    if (version != AliConfig::fgkDefaultEventFolderName) // only if not the default folder name 
      fileName = fileName.ReplaceAll(".root", "") + "_" + version + ".root" ;
    if ( !(gSystem->AccessPathName(fileName)) ) { 
      Warning("VersionExists", "The file %s already exists", fileName.Data()) ;
      rv = kTRUE ; 
    }
    PhosLoader()->SetSDigitsFileName(fileName) ;
  }

  if ( opt == "digits") {
    // add the version name to the root file name
    TString fileName( PhosLoader()->GetDigitsFileName() ) ; 
    if (version != AliConfig::fgkDefaultEventFolderName) // only if not the default folder name 
      fileName = fileName.ReplaceAll(".root", "") + "_" + version + ".root" ;
    if ( !(gSystem->AccessPathName(fileName)) ) {
      Warning("VersionExists", "The file %s already exists", fileName.Data()) ;  
      rv = kTRUE ; 
    }
    PhosLoader()->SetDigitsFileName(fileName) ;
  }


//     else
//       if(recName == "RecPoints"){
//        tree = TreeR();
//        dataname = fgkEmcRecPointsBranchName;
//        zername = "AliPHOSClusterizer" ;
//       }
//       else
//        if(recName == "TrackSegments"){
//          tree = TreeT();
//          dataname = GetDetectorName();
//          zername = "AliPHOSTrackSegmentMaker";
//        }        
//        else
//          if(recName == "RecParticles"){
//            tree = TreeT();
//            dataname = fgkRecParticlesBranchName;
//            zername = "AliPHOSPID";
//          }
//          else
//            return kFALSE ;

  
//   if(!tree ) 
//     return kFALSE ;

//   TObjArray * lob = static_cast<TObjArray*>(tree->GetListOfBranches()) ;
//   TIter next(lob) ; 
//   TBranch * branch = 0 ;  
//   TString titleName(fBranchTitle);
//   titleName+=":";

//   while ((branch = (static_cast<TBranch*>(next())))) {
//     TString branchName(branch->GetName() ) ; 
//     TString branchTitle(branch->GetTitle() ) ;  
//     if ( branchName.BeginsWith(dataname) && branchTitle.BeginsWith(fBranchTitle) ){  
//       Warning("BranchExists","branch %s  with title  %s ",dataname.Data(),fBranchTitle.Data());
//       return kTRUE ;
//     }
//     if ( branchName.BeginsWith(zername) &&  branchTitle.BeginsWith(titleName) ){
//       Warning("BranchExists","branch AliPHOS... with title  %s ",branch->GetTitle());
//       return kTRUE ; 
//     }
//   }

  return rv ;

}


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
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TFolder.h"
#include "TParticle.h"

// --- Standard library ---
#include <iostream.h>

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliConfig.h"
#include "AliPHOSGetter.h"
#include "AliPHOS.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSClusterizerv1.h"
#include "AliPHOSTrackSegmentMakerv1.h"
#include "AliPHOSTrackSegment.h"
#include "AliPHOSPIDv1.h" 
#include "AliPHOSGeometry.h"

ClassImp(AliPHOSGetter)
  
  AliPHOSGetter * AliPHOSGetter::fgObjGetter = 0 ; 
  TFile * AliPHOSGetter::fFile = 0 ; 

//____________________________________________________________________________ 
AliPHOSGetter::AliPHOSGetter(const char* headerFile, const char* branchTitle, const Bool_t toSplit )
{
  // This is the ctor called by GetInstance and the only one that can be used 

  if( fHeaderFile.Contains("_") ) {
    cerr << "AliPHOSGetter::AliPHOSGetter -> Invalid file name (_ not allowed) " << fHeaderFile.Data() << endl ;
    abort() ; 
  }

  //Initialize  all data

  fFailed = kFALSE ;   
  fDebug  = 0 ; 

  fToSplit    = toSplit ;
  fHeaderFile = headerFile ; 

  fPrimaries = new TObjArray(1) ;

  fModuleFolder    = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/Run/Configuration/Modules")); 
  fPrimariesFolder = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/RunMC/Event/Data")); 
  fHitsFolder      = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/RunMC/Event/Data/Hits")); 
  fSDigitsFolder   = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/RunMC/Event/Data/SDigits")); 
  fDigitsFolder    = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/Run/Event/Data")); 
  fRecoFolder      = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/Run/Event/RecData")); 
  fQAFolder        = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/Run/Conditions/QA")); 
  fTasksFolder     = dynamic_cast<TFolder*>(gROOT->FindObjectAny("Folders/Tasks")) ; 

  //Set titles to branches and create PHOS specific folders
  SetTitle(branchTitle) ;
  
  if ( fHeaderFile != "aliroot"  ) { // to call the getter without a file
    //open headers file
    fFile = static_cast<TFile*>(gROOT->GetFile(fHeaderFile.Data() ) ) ;
    
    if(!fFile) {    //if file was not opened yet, read gAlice
      fFile = TFile::Open(fHeaderFile.Data(),"update") ; 
      if (!fFile->IsOpen()) {
	cerr << "ERROR : AliPHOSGetter::AliPHOSGetter -> Cannot open " << fHeaderFile.Data() << endl ; 
	fFailed = kTRUE ;
        return ;  
      }
      gAlice = static_cast<AliRun *>(fFile->Get("gAlice")) ;
    }       
  }
  
  if (!gAlice) {
    cerr << "ERROR : AliPHOSGetter::AliPHOSGetter -> Cannot find gAlice in " << fHeaderFile.Data() << endl ; 
    fFailed = kTRUE ;
    return ; 
  }
  if (!PHOS()) {
    if (fDebug)
      cout << "INFO: AliPHOSGetter:AliPHOSGetter -> Posting PHOS to Folders" << endl ; 
    if (gAlice->GetDetector("PHOS")) {
      AliConfig * conf = AliConfig::Instance() ; 
      conf->Add(static_cast<AliDetector*>(gAlice->GetDetector("PHOS"))) ; 
      conf->Add(static_cast<AliModule*>(gAlice->GetDetector("PHOS"))) ; 
    }
    else 
      cerr << "ERROR: AliPHOSGetter:AliPHOSGetter -> detector PHOS not found" << endl ;  
  }

  fDebug=0;
}

//____________________________________________________________________________ 
AliPHOSGetter::~AliPHOSGetter(){

  if (fPrimaries) {
    fPrimaries->Delete() ; 
    delete fPrimaries ; 
  }

  TFolder * phosF = dynamic_cast<TFolder *>(fSDigitsFolder->FindObject("PHOS")) ;
  TCollection * folderslist = phosF->GetListOfFolders() ; 
  TIter next(folderslist) ; 
  TFolder * folder = 0 ; 
  while ( (folder = static_cast<TFolder*>(next())) ) 
    phosF->Remove(folder) ; 
  
  if (fFile) { 
    fFile->Close() ;  
    delete fFile ;
    fFile = 0 ;
  }
  fgObjGetter = 0 ; 
  
}

//____________________________________________________________________________ 
void AliPHOSGetter::CloseFile()
{
  delete gAlice ;  
  gAlice = 0 ; 
}

//____________________________________________________________________________ 
const TFolder * AliPHOSGetter::Folder(const TString what) const {

  // returns the PHOS folder required by what
  // what = hits, sdigits, digits

  if ( what == "hits" ) 
    return dynamic_cast<const TFolder *>(fHitsFolder->FindObject("PHOS")) ; 
  else if ( what == "sdigits" ) 
    return  dynamic_cast<const TFolder *>(fSDigitsFolder->FindObject("PHOS")) ; 
  else if ( what == "digits" ) 
    return  dynamic_cast<const TFolder *>(fDigitsFolder->FindObject("PHOS")) ; 
  else {
    cerr << "ERROR: AliPHOSGetter::GetFolder -> " << what.Data() << " illegal option (hits, sdigits, digits) " << endl ; 
    return 0 ; 
  }
}

//____________________________________________________________________________ 
AliPHOSGetter * AliPHOSGetter::GetInstance()
{
  // Returns the pointer of the unique instance already defined
  
  if ( fgObjGetter ) {
    return fgObjGetter ;
  }
  else {
    //cout << "WARNING: AliPHOSGetter::GetInstance ERROR: not yet initialized" << endl ;
    return 0 ; 
  }
}

//____________________________________________________________________________ 
AliPHOSGetter * AliPHOSGetter::GetInstance(const char* headerFile,
					   const char* branchTitle,
                                           const Bool_t toSplit)
{
  // Creates and returns the pointer of the unique instance
  // Must be called only when the environment has changed

  if(!fgObjGetter){
    fgObjGetter = new AliPHOSGetter(headerFile,branchTitle,toSplit) ;
    if(fgObjGetter->fFailed)
      return 0;
    else
      return fgObjGetter ;
  }

  //First checks, if header file already opened
  if(!fgObjGetter->fFile){
     fgObjGetter = new AliPHOSGetter(headerFile,branchTitle,toSplit) ;
    if(fgObjGetter->fFailed)
      return 0;
    else
      return fgObjGetter ;
  }

  if(fgObjGetter->fHeaderFile.CompareTo(headerFile)==0){ //Opened the same header file
    if((fgObjGetter->fBranchTitle.CompareTo(branchTitle) == 0)&&   //Open the same branch title
       (toSplit==fgObjGetter->fToSplit)){                          //Nothing should be cleaned
      return fgObjGetter ;
    }
    else{ //Clean all data and AliPHOS...zers
      if(fgObjGetter->fToSplit)
	fgObjGetter->CloseSplitFiles() ;
      fgObjGetter->CleanWhiteBoard() ;
      fgObjGetter->fToSplit = toSplit ;
      fgObjGetter->SetTitle(branchTitle) ;
      return fgObjGetter ; 
    }
  }
  else{  //Close already opened files, clean memory and open new header file
    if(gAlice)
      delete gAlice ;
    gAlice= 0;
    if(fgObjGetter->fFile){
      fgObjGetter->fFile->Close() ;
      fgObjGetter->fFile=0;
    }
    if(fgObjGetter->fToSplit)
      fgObjGetter->CloseSplitFiles() ;
    fgObjGetter->CleanWhiteBoard() ;    
    fgObjGetter = new AliPHOSGetter(headerFile,branchTitle,toSplit) ;
    return fgObjGetter ; 
  }
  return fgObjGetter ; 
  
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::BranchExists(const TString recName) const
{
  //Looks in the tree Tree"name" if branch with current name olready exists

  TString filename("") ;
  TString name, dataname, zername ;
  if(recName == "SDigits"){
    filename=fSDigitsFileName ;
    name = "TreeS0" ;
    dataname = "PHOS" ;
    zername = "AliPHOSSDigitizer" ;
  }
  else
    if(recName == "Digits"){
      filename=fDigitsFileName ;
      name = "TreeD0" ;
      dataname = "PHOS" ;
      zername = "AliPHOSDigitizer" ;
    }
    else
      if(recName == "RecPoints"){
	filename=fRecPointsFileName ;
	name = "TreeR0" ;
	dataname = "PHOSEmcRP" ;
	zername = "AliPHOSClusterizer" ;
      }
      else
	if(recName == "TrackSegments"){
	  filename=fTrackSegmentsFileName ;
	  name = "TreeR0" ;
	  dataname = "PHOSTS" ;
	  zername = "AliPHOSTrackSegmentMaker" ;
	}	 
	else
	  if(recName == "RecParticles"){
	    filename= fRecParticlesFileName ;
	    name = "TreeR0" ;
	    dataname = "PHOSRP" ;
	    zername = "AliPHOSPID" ;
	  }
	  else
	    return kFALSE ;

  TFile * file ;
  TTree * tree ;
  if(fToSplit){
    file = static_cast<TFile*>(gROOT->GetFile(filename.Data() ) ) ;
    if(!file)
      file = TFile::Open(fSDigitsFileName.Data(),"update");
  }
  else
    file = fFile ;

  tree = (TTree *)file->Get(name.Data()) ;
  
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
      cerr << "WARNING: AliPHOSGetter::BranchExists -> branch " << dataname.Data() << " with title " << fBranchTitle << " already exits in "
	   << name.Data() << endl;
      return kTRUE ;
    }
    if ( branchName.BeginsWith(zername) &&  branchTitle.BeginsWith(titleName) ){
      cerr << "WARNING:  AliPHOSGetter::BranchExists -> branch AliPHOS... with title " << branch->GetTitle() << " already exits in "
	   << name.Data() << endl;     
      return kTRUE ; 
    }
  }
  //We can't delete three if gAlice points to it... To be redisigned somehow???!!!
  if(!fToSplit){
    if(name.Contains("TreeS"))
      if(tree!=gAlice->TreeS())
	tree->Delete();
    if(name.Contains("TreeD"))
      if(tree!=gAlice->TreeD())
	tree->Delete();
    if(name.Contains("TreeR"))
      if(tree!=gAlice->TreeR())
	tree->Delete();    
  }
  return kFALSE ;
  
}

//____________________________________________________________________________ 
void AliPHOSGetter::ListBranches(Int_t event) const  
{
  
  TBranch * branch = 0 ; 
  if (gAlice->GetEvent(event) == -1)
    return ; 
  
  TTree * t =  gAlice->TreeH() ; 
  if(t){
    cout << "INFO: AliPHOSGetter::ListBranches -> ****** Hits    : " << endl ; 
    TObjArray * lob = t->GetListOfBranches() ;
    TIter next(lob) ; 
    while ( (branch = static_cast<TBranch*>(next())) )
      cout << "             " << branch->GetName() << endl ; 
  } else
    cerr << "WARNING::AliPHOSGetter::ListBranches -> TreeH not found for event " << event << endl ;  
  
  t = gAlice->TreeS() ;
  if(t){
    cout << "INFO: AliPHOSGetter::ListBranches -> ****** SDigits : " << endl ; 
    TObjArray * lob = t->GetListOfBranches() ;
    TIter next(lob) ; 
    while ( (branch = static_cast<TBranch*>(next())) )
      cout << "             " << branch->GetName() << " " << branch->GetTitle() << endl ; 
  } else 
    cerr << "WARNING::AliPHOSGetter::ListBranches -> TreeS not found for event " << event << endl ;  
  
  
  t = gAlice->TreeD() ;
  if(t){
    cout << "INFO: AliPHOSGetter::ListBranches -> ****** Digits  : " << endl ; 
    TObjArray * lob = t->GetListOfBranches() ;
    TIter next(lob) ; 
    while ( (branch = static_cast<TBranch*>(next())) )
      cout << "             " << branch->GetName() << " " << branch->GetTitle() << endl ; 
  } else 
    cerr << "WARNING::AliPHOSGetter::ListBranches -> TreeD not found for event " << event << endl ;  
  

  t = gAlice->TreeR() ;
  if(t){
    cout << "INFO: AliPHOSGetter::ListBranches -> ****** Recon   : " << endl ; 
    TObjArray * lob = t->GetListOfBranches() ;
    TIter next(lob) ; 
    while ( (branch = static_cast<TBranch*>(next())) )
      cout << "             " << branch->GetName() << " " << branch->GetTitle() << endl ; 
  } else 
    cerr << "WARNING::AliPHOSGetter::ListBranches -> TreeR not found for event " << event << endl ;  
  
}

//____________________________________________________________________________ 
void AliPHOSGetter::NewBranch(TString name, Int_t event)  
{
  fBranchTitle = fSDigitsTitle = fDigitsTitle = fRecPointsTitle = fTrackSegmentsTitle = fRecParticlesTitle =  name ; 
  Event(event) ; 
}

//____________________________________________________________________________ 
Bool_t AliPHOSGetter::NewFile(TString name)  
{
  fHeaderFile = name ; 
  fFile->Close() ; 
  fFailed = kFALSE; 

  fFile = static_cast<TFile*>(gROOT->GetFile(fHeaderFile.Data() ) ) ;
  if(!fFile) {    //if file was not opened yet, read gAlice
    fFile = TFile::Open(fHeaderFile.Data(),"update") ;
    if (!fFile->IsOpen()) {
      cerr << "ERROR : AliPHOSGetter::NewFile -> Cannot open " << fHeaderFile.Data() << endl ; 
      fFailed = kTRUE ;
      return fFailed ;  
    }
    gAlice = static_cast<AliRun *>(fFile->Get("gAlice")) ;
  } 
  
  if (!gAlice) {
    cerr << "ERROR : AliPHOSGetter::AliPHOSGetter -> Cannot find gAlice in " << fHeaderFile.Data() << endl ; 
    fFailed = kTRUE ;
    return fFailed ; 
  }
  return fFailed ; 
}

//____________________________________________________________________________ 
const AliPHOS * AliPHOSGetter::PHOS() 
{
  // returns the PHOS object 
  AliPHOS * phos = dynamic_cast<AliPHOS*>(fModuleFolder->FindObject("PHOS")) ;  
  if (!phos) 
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::PHOS -> PHOS module not found in Folders" << endl ; 
  return phos ; 
}  

//____________________________________________________________________________ 
const AliPHOSGeometry * AliPHOSGetter::PHOSGeometry() 
{
  AliPHOSGeometry * rv = 0 ; 
  if (PHOS() )
    rv =  PHOS()->GetGeometry() ;
  return rv ; 
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostPrimaries(void) const 
{  //------- Primaries ----------------------

  // the hierarchy is //Folders/RunMC/Event/Data/Primaries
  
  TFolder * primariesFolder = dynamic_cast<TFolder*>(fPrimariesFolder->FindObject("Primaries")) ; 
  if ( !primariesFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post Primaries -> Folder //" << fPrimariesFolder->GetName() << "/Primaries/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Primaries -> Adding Folder //" << fPrimariesFolder->GetName() << "/Primaries/"  << endl;
    }
    primariesFolder = fPrimariesFolder->AddFolder("Primaries", "Primaries particles from TreeK") ; 
  }    
  TClonesArray *primaries=  new TClonesArray("TParticle",1000) ;
  primaries->SetName("Primaries") ;
  primariesFolder->Add(primaries) ; 
  
  return kTRUE;
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::PrimariesRef(void) const 
{  //------- Primaries ----------------------

  
  // the hierarchy is //Folders/RunMC/Event/Data/Primaries
  if ( !fPrimariesFolder ) {
    cerr << "ERROR: AliPHOSGetter::PrimariesRef -> Folder //" << fPrimariesFolder << " not found!" << endl;
    abort() ;
  }    
 
  TFolder * primariesFolder = dynamic_cast<TFolder *>(fPrimariesFolder->FindObject("Primaries")) ;
  if ( !primariesFolder ) {
    cerr << "ERROR: AliPHOSGetter::PrimariesRef -> Folder //" << fPrimariesFolder << "/Primaries/ not found!" << endl;  
    abort() ;
  }
 
  TObject * p = primariesFolder->FindObject("Primaries") ;
  if(!p) {
    cerr << "ERROR: AliPHOSGetter::PrimariesRef -> " << primariesFolder->GetName() << "/Primaries not found !" << endl ; 
    abort() ;
  }
  else
    return primariesFolder->GetListOfFolders()->GetObjectRef(p) ;
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostHits(void) const 
{  //------- Hits ----------------------

  // the hierarchy is //Folders/RunMC/Event/Data/PHOS/Hits
  
  TFolder * phosFolder = dynamic_cast<TFolder*>(fHitsFolder->FindObject("PHOS")) ; 
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post H -> Folder //" << fHitsFolder << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post H -> Adding Folder //" << fHitsFolder << "/PHOS/"  << endl;
    }
    phosFolder = fHitsFolder->AddFolder("PHOS", "Hits from PHOS") ; 
  }    
  TClonesArray *hits=  new TClonesArray("AliPHOSHit",1000) ;
  hits->SetName("Hits") ;
  phosFolder->Add(hits) ; 
  
  return kTRUE;
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::HitsRef(void) const 
{  //------- Hits ----------------------

  
  // the hierarchy is //Folders/RunMC/Event/Data/PHOS/Hits
  if ( !fHitsFolder ) {
    cerr << "ERROR: AliPHOSGetter::HitsRef -> Folder //" << fHitsFolder << " not found!" << endl;
    abort() ;
  }    
 
  TFolder * phosFolder = dynamic_cast<TFolder *>(fHitsFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::HitsRef -> Folder //" << fHitsFolder << "/PHOS/ not found!" << endl;  
    abort() ;
  }
 
  TObject * h = phosFolder->FindObject("Hits") ;
  if(!h) {
    cerr << "ERROR: AliPHOSGetter::HitsRef -> " << phosFolder->GetName() << "/Hits not found !" << endl ; 
    abort() ;
  }
  else
    return phosFolder->GetListOfFolders()->GetObjectRef(h) ;
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostSDigits(const char * name, const char * headerFile) const 
{  //---------- SDigits -------------------------

  
  // the hierarchy is //Folders/RunMC/Event/Data/PHOS/SDigits/headerFile/sdigitsname
  // because you can have sdigits from several hit files for mixing
  
  TFolder * phosFolder = dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post S -> Folder //" << fSDigitsFolder << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post S -> Adding Folder //" << fHitsFolder << "/PHOS/" << endl;
    }
    phosFolder = fSDigitsFolder->AddFolder("PHOS", "SDigits from PHOS") ; 
  }    

  TString subdir(headerFile) ;
  subdir.ReplaceAll("/","_") ; 
  TFolder * phosSubFolder = dynamic_cast<TFolder*>(phosFolder->FindObject(subdir)) ; 
  if ( !phosSubFolder ) 
    phosSubFolder = phosFolder->AddFolder(subdir, ""); 
  

  TObject * sd  = phosSubFolder->FindObject(name); 
  if ( !sd ) {
    TClonesArray * sdigits = new TClonesArray("AliPHOSDigit",1) ;
    sdigits->SetName(name) ;
    phosSubFolder->Add(sdigits) ;
  }
  
  return kTRUE;
} 
//____________________________________________________________________________ 
TObject** AliPHOSGetter::SDigitsRef(const char * name, const char * file) const 
{  //------- SDigits ----------------------
  
  // the hierarchy is //Folders/RunMC/Event/Data/PHOS/SDigits/filename/SDigits

  if ( !fSDigitsFolder ) {
    cerr << "ERROR: AliPHOSGetter::SDigitsRef -> Folder //" << fSDigitsFolder << " not found!" << endl;
    abort() ;
  }    
 
  TFolder * phosFolder = dynamic_cast<TFolder *>(fSDigitsFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::SDigitsRef -> Folder //" << fSDigitsFolder << "/PHOS/ not found!" << endl;
    abort() ;
  }

  TFolder * phosSubFolder = 0 ;
  if(file)
    phosSubFolder = dynamic_cast<TFolder *>(phosFolder->FindObject(file)) ;
  else
    phosSubFolder = dynamic_cast<TFolder *>(phosFolder->FindObject(fHeaderFile)) ;
  
  if(!phosSubFolder) {
    cerr << "ERROR: AliPHOSGetter::DigitesSRef -> Folder //Folders/RunMC/Event/Data/PHOS/" << file << "not found!" << endl;
    abort() ;
  }

  TObject * dis = phosSubFolder->FindObject(name) ;
  if(!dis){
    cerr << "ERROR: AliPHOSGetter::DigitesSRef -> object " << name << " not found! " << endl ; 
    abort()  ;
  }
  else
    return phosSubFolder->GetListOfFolders()->GetObjectRef(dis) ;

}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostSDigitizer(AliPHOSSDigitizer * sdigitizer) const 
{  //---------- SDigitizer -------------------------
    
  // the hierarchy is //Folders/Tasks/SDigitizer/PHOS/sdigitsname


  TTask * sd  = dynamic_cast<TTask*>(fTasksFolder->FindObject("SDigitizer")) ; 

  if ( !sd ) {
    cerr << "ERROR: AliPHOSGetter::Post Ser -> Task //" << fTasksFolder << "/SDigitizer not found!" << endl;
    return kFALSE ;
  }        
  TTask * phos = dynamic_cast<TTask*>(sd->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Ser ->//" << fTasksFolder << "/SDigitizer/PHOS/ not found!" << endl;  
      cout <<"INFO: AliPHOSGetter::Post Ser -> Adding //" << fTasksFolder << "/SDigitizer/PHOS/" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    sd->Add(phos) ; 
  } 
  AliPHOSSDigitizer * phossd  = dynamic_cast<AliPHOSSDigitizer *>(phos->GetListOfTasks()->FindObject( sdigitizer->GetName() )); 
  if (phossd) { 
    if (fDebug)
      cout << "INFO: AliPHOSGetter::Post Ser -> Task " << sdigitizer->GetName() << " already exists" << endl ; 
    phos->GetListOfTasks()->Remove(phossd) ;
  }
  phos->Add(sdigitizer) ;	
  return kTRUE; 
  
}

//____________________________________________________________________________ 
TObject** AliPHOSGetter::SDigitizerRef(const char * name) const 
{  

  TTask * sd  = dynamic_cast<TTask*>(fTasksFolder->FindObject("SDigitizer")) ; 
  if ( !sd ) {
    cerr << "ERROR: AliPHOSGetter::Post SerRef -> Task //" << fTasksFolder << "/SDigitizer not found!" << endl;
    abort();
  }        

  TTask * phos = dynamic_cast<TTask*>(sd->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    cerr <<"ERROR: AliPHOSGetter::Post SerRef ->  //" << fTasksFolder << "/SDigitizer/PHOS not found!" << endl;
    abort();
  }        

  TTask * task = dynamic_cast<TTask*>(phos->GetListOfTasks()->FindObject(name)) ; 

  return phos->GetListOfTasks()->GetObjectRef(task) ;

}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostSDigitizer(const char * name, const char * file) const 
{  //---------- SDigitizer -------------------------
  
 // the hierarchy is //Folders/Tasks/SDigitizer/PHOS/sdigitsname

  TTask * sd  = dynamic_cast<TTask*>(fTasksFolder->FindObject("SDigitizer")) ; 
  if ( !sd ) {
    cerr << "ERROR: AliPHOSGetter::Post Ser -> Task //" << fTasksFolder << "/SDigitizer not found!" << endl;
    return kFALSE ;
  }        

  TTask * phos = dynamic_cast<TTask*>(sd->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Ser ->  //" << fTasksFolder << "/SDigitizer/PHOS/ not found!" << endl;
      cout <<"INFO: AliPHOSGetter::Post Ser -> Adding  //" << fTasksFolder << "/SDigitizer/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    sd->Add(phos) ; 
  } 

  TString sdname(name) ;
  sdname.Append(":") ;
  sdname.Append(file);
  sdname.ReplaceAll("/","_") ; 
  AliPHOSSDigitizer * phossd  = dynamic_cast<AliPHOSSDigitizer *>(phos->GetListOfTasks()->FindObject( sdname )); 
  if (!phossd) {
    phossd = new AliPHOSSDigitizer() ;  
    //Note, we can not call constructor with parameters: it will call Getter and scew up everething
    phossd->SetName(sdname) ;
    phossd->SetTitle(file) ;
    phos->Add(phossd) ;	
  }
  return kTRUE; 
  
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostDigits(const char * name) const 
{  //---------- Digits -------------------------

  // the hierarchy is //Folders/Run/Event/Data/PHOS/SDigits/name

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fDigitsFolder->FindObject("PHOS")) ;

  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post D -> Folder //" << fDigitsFolder << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post D -> Adding Folder //" << fDigitsFolder << "/PHOS/" << endl;
    }
    phosFolder = fDigitsFolder->AddFolder("PHOS", "Digits from PHOS") ;  
  }    
 
  TObject*  dig = phosFolder->FindObject( name ) ;
  if ( !dig ) {
    TClonesArray * digits = new TClonesArray("AliPHOSDigit",1000) ;
    digits->SetName(name) ;
    phosFolder->Add(digits) ;  
  }
  return kTRUE; 
}

//____________________________________________________________________________ 
TObject** AliPHOSGetter::DigitsRef(const char * name) const 
{ //------- Digits ----------------------
  
  // the hierarchy is //Folders/Run/Event/Data/PHOS/Digits/name

  if ( !fDigitsFolder ) {
    cerr << "ERROR: AliPHOSGetter::DigitsRef -> Folder //" << fDigitsFolder << " not found!" << endl;
    abort() ;
  }    
  
  TFolder * phosFolder  = dynamic_cast<TFolder*>(fDigitsFolder->FindObject("PHOS")) ; 
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::DigitsRef -> Folder //" << fDigitsFolder << "/PHOS/ not found!" << endl;
    abort() ;
  }    

  TObject * d = phosFolder->FindObject(name) ;
  if(!d) {
    cerr << "ERROR: AliPHOSGetter::DigitsRef -> object " << name << " not found! " << endl ; 
    abort() ;
  }
  else
    return phosFolder->GetListOfFolders()->GetObjectRef(d) ;

}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostDigitizer(AliPHOSDigitizer * digitizer) const 
{  //---------- Digitizer -------------------------
  
  TTask * sd  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Digitizer")) ; 

  if ( !sd ) {
    cerr << "ERROR: AliPHOSGetter::Post Der -> Task //" << fTasksFolder << "/Digitizer not found!" << endl;
    return kFALSE ;
  }        
  TTask * phos = dynamic_cast<TTask*>(sd->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Der ->  //" << fTasksFolder << "/Digitizer/PHOS not found!" << endl;
      cout <<"INFO: AliPHOSGetter::Post Der -> Adding //" << fTasksFolder << "/Digitizer/PHOS" << endl; 
    }
    phos = new TTask("PHOS", "") ; 
    sd->Add(phos) ; 
  } 

    AliPHOSDigitizer * phosd = dynamic_cast<AliPHOSDigitizer*>(phos->GetListOfTasks()->FindObject(digitizer->GetName())) ; 
    if (phosd) { 
      phosd->Delete() ;
      phos->GetListOfTasks()->Remove(phosd) ;
    }
    phos->Add(digitizer) ; 
    return kTRUE; 
}  

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostDigitizer(const char * name) const 
{  //---------- Digitizer -------------------------
  
 // the hierarchy is //Folders/Tasks/SDigitizer/PHOS/sdigitsname

  TTask * d  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Digitizer")) ; 
  if ( !d ) {
    cerr << "ERROR: AliPHOSGetter::Post Der -> Task //" << fTasksFolder << "/Digitizer not found!" << endl;
    return kFALSE ;
  }        

  TTask * phos = dynamic_cast<TTask*>(d->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Der -> //" << fTasksFolder << "/Digitizer/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Der -> Adding //" << fTasksFolder << "/Digitizer/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    d->Add(phos) ; 
} 

  AliPHOSDigitizer * phosd = dynamic_cast<AliPHOSDigitizer*>(phos->GetListOfTasks()->FindObject(name)) ; 
  if (!phosd) { 
    phosd = new AliPHOSDigitizer() ;
    phosd->SetName(fDigitsTitle) ;
    phosd->SetTitle(fHeaderFile) ;
    phos->Add(phosd) ;
  }
  return kTRUE;  
}

//____________________________________________________________________________ 
TObject** AliPHOSGetter::DigitizerRef(const char * name) const 
{  
  TTask * sd  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Digitizer")) ; 
  if ( !sd ) {
    cerr << "ERROR: AliPHOSGetter::Post DerRef -> Task //" << fTasksFolder << "/Digitizer not found!" << endl;
    abort();
  }        

  TTask * phos = dynamic_cast<TTask*>(sd->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    cerr <<"ERROR: AliPHOSGetter::Post DerRef ->  //" << fTasksFolder << "/Digitizer/PHOS" << endl;
    abort();
  }        

  TTask * task = dynamic_cast<TTask*>(phos->GetListOfTasks()->FindObject(name)) ; 

  return phos->GetListOfTasks()->GetObjectRef(task) ;

}
 
//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostRecPoints(const char * name) const 
{ // -------------- RecPoints -------------------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/EMCARecPoints/name
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/CPVRecPoints/name

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS")) ; 
  
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post RPo -> Folder //" << fRecoFolder->GetName() << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Rpo -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/" << endl;
    }
    phosFolder = fRecoFolder->AddFolder("PHOS", "Reconstructed data from PHOS") ;  
  }    
  
  // EMCA RecPoints 
  TFolder * phosRPoEMCAFolder  = dynamic_cast<TFolder*>(phosFolder->FindObject("EMCARecPoints")) ;
  if ( !phosRPoEMCAFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post RPo -> Folder //" << fRecoFolder->GetName() << "/PHOS/EMCARecPoints/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Rpo -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/EMCARecPoints" << endl;
    }
    phosRPoEMCAFolder = phosFolder->AddFolder("EMCARecPoints", "EMCA RecPoints from PHOS") ;  
  }    
  
  TObject * erp = phosFolder->FindObject( name ) ;
  if ( !erp )   {
    TObjArray * emcrp = new TObjArray(100) ;
    emcrp->SetName(name) ;
    phosRPoEMCAFolder->Add(emcrp) ;  
  }

  // CPV RecPoints 
  TFolder * phosRPoCPVFolder  = dynamic_cast<TFolder*>(phosFolder->FindObject("CPVRecPoints")) ;
  if ( !phosRPoCPVFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post RPo -> Folder //" << fRecoFolder->GetName() << "/PHOS/CPVRecPoints/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Rpo -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/CPVRecPoints/" << endl;
    }
    phosRPoCPVFolder = phosFolder->AddFolder("CPVRecPoints", "CPV RecPoints from PHOS") ;  
  }    
  
  TObject * crp =  phosRPoCPVFolder->FindObject( name ) ;
  if ( !crp )   {
    TObjArray * cpvrp = new TObjArray(100) ;
    cpvrp->SetName(name) ;
    phosRPoCPVFolder->Add(cpvrp) ;  
  }
  return kTRUE; 
}

//____________________________________________________________________________ 
TObject** AliPHOSGetter::EmcRecPointsRef(const char * name) const 
{ // -------------- RecPoints -------------------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/EMCARecPoints/name
   
  if ( !fRecoFolder ) {
    cerr << "ERROR: AliPHOSGetter::EmcRecPointsRef -> Folder //" << fRecoFolder->GetName() << " not found!" << endl;
    abort() ; 
  }    

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/EMCARecPoints")) ; 
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::EmcRecPointsRef -> Folder //" << fRecoFolder->GetName() << "/PHOS/EMCARecPoints/ not found!" << endl;
    abort() ;
  }    


  TObject * erp = phosFolder->FindObject(name ) ;
  if ( !erp )   {
    cerr << "ERROR: AliPHOSGetter::EmcRecPointsRef -> object " << name << " not found! " << endl ; 
    abort() ;
  }
  return phosFolder->GetListOfFolders()->GetObjectRef(erp) ;
  
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::CpvRecPointsRef(const char * name) const 
{ // -------------- RecPoints -------------------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/CPVRecPoints/name
   
  if ( !fRecoFolder ) {
    cerr << "ERROR: AliPHOSGetter::CpvRecPointsRef -> Folder //" << fRecoFolder->GetName() << " not found!" << endl;
    abort() ; 
  }    

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/CPVRecPoints")) ; 
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::CpvRecPointsRef -> Folder //" << fRecoFolder->GetName() << "/PHOS/CPVRecPoints/" << endl;
    abort() ;
  }    

  TObject * crp = phosFolder->FindObject(name ) ;
  if ( !crp )   {
    cerr << "ERROR: AliPHOSGetter::CpvRecPointsRef -> object " << name << " not found " << endl ; 
    abort() ;
  }
  return phosFolder->GetListOfFolders()->GetObjectRef(crp) ;
  
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostClusterizer(AliPHOSClusterizer * clu) const 
{ // ------------------ AliPHOSClusterizer ------------------------
  
  // the hierarchy is //Folders/Tasks/Reconstructioner/PHOS/sdigitsname

  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Rer -> Task //" << fTasksFolder << "/Reconstructioner not found!" << endl;
    return kFALSE ;
  }        
        
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Rer -> //" << fTasksFolder << "/Reconstructioner/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Rer -> Adding //" << fTasksFolder << "/Reconstructioner/PHOS" << endl; 
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  AliPHOSClusterizer * phoscl = dynamic_cast<AliPHOSClusterizer*>(phos->GetListOfTasks()->FindObject(clu->GetName())) ; 
  if (phoscl) { 
    if (fDebug)
      cout << "INFO: AliPHOSGetter::Post Rer -> Task " << clu->GetName() << " already exists" << endl ; 
    phoscl->Delete() ; 
    phos->GetListOfTasks()->Remove(phoscl) ;
  }
  phos->Add(clu) ;      
  return kTRUE; 
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::ClusterizerRef(const char * name) const 
{ // ------------------ AliPHOSClusterizer ------------------------
  
  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::ClusterizerRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner not found!" << endl;
    abort() ;
  }        
        
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    cerr <<"WARNING: AliPHOSGetter::ClusterizerRef -> //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS" << endl; 
    abort() ; 
  }   

  TList * l = phos->GetListOfTasks() ; 
  TIter it(l) ;
  TTask * task ;
  TTask * clu = 0 ;
  TString cluname(name) ;
  cluname+=":clu" ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(cluname)){
      clu = task ;
      break ;
    }
  }

  if(clu) 
    return l->GetObjectRef(clu) ;
  else{
    cerr << "ERROR: AliPHOSGetter::ClusterizerRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner/clusterizer " <<  name << " not found!" << endl;
    abort() ;
  }
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostClusterizer(const char * name) const 
{ // ------------------ AliPHOSClusterizer ------------------------

  // the hierarchy is //Folders/Tasks/Reconstructioner/PHOS/sdigitsname
  
  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Rer -> Task//" << fTasksFolder << "/Reconstructioner not found!" << endl; 
    return kFALSE ;
  }        
  
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Rer -> //" << fTasksFolder << "/Reconstructioner/PHOS not found!" << endl;
      cout <<"INFO: AliPHOSGetter::Post Rer -> Adding //" << fTasksFolder << "/Reconstructioner/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  TList * l = phos->GetListOfTasks() ;   
  TIter it(l) ;
  TString clun(name) ;
  clun+=":clu" ; 
  TTask * task ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(clun))
      return kTRUE ;
  }

  AliPHOSClusterizerv1 * phoscl = new AliPHOSClusterizerv1() ;
  clun+="-v1" ; 
  phoscl->SetName(clun) ;
  phoscl->SetTitle(fHeaderFile) ;
  phos->Add(phoscl) ;
  return kTRUE; 
  
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostTrackSegments(const char * name) const 
{ // ---------------TrackSegments -----------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/TrackSegments/name

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS")) ; 
  
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post TS -> Folder //" << fRecoFolder->GetName() << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post TS -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS" << endl;
    }
    phosFolder = fRecoFolder->AddFolder("PHOS", "Reconstructed data from PHOS") ;  
  }    

  TFolder * phosTSFolder  = dynamic_cast<TFolder*>(phosFolder->FindObject("TrackSegments")) ;
  if ( !phosTSFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post TS -> Folder //" << fRecoFolder->GetName() << "/PHOS/TrackSegments/ not found!" << endl; 
      cout << "INFO:    AliPHOSGetter::Post TS -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/TrackSegments/" << endl; 
    }
    phosTSFolder = phosFolder->AddFolder("TrackSegments", "TrackSegments from PHOS") ;  
  }    
  
  TObject * tss =  phosTSFolder->FindObject( name ) ;
  if (!tss) {
    TClonesArray * ts = new TClonesArray("AliPHOSTrackSegment",100) ;
    ts->SetName(name) ;
    phosTSFolder->Add(ts) ;  
  }
  return kTRUE; 
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::TrackSegmentsRef(const char * name) const 
{ // ---------------TrackSegments -----------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/TrackSegments/name

 if ( !fRecoFolder ) {
    cerr << "ERROR: AliPHOSGetter::TrackSegmentsRef -> Folder //" << fRecoFolder->GetName() << "not found!" << endl;
    abort() ; 
  }    

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/TrackSegments")) ; 
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::TrackSegmentsRef -> Folder //" << fRecoFolder->GetName() << "/PHOS/TrackSegments/ not found!" << endl;
    abort() ;
  }    
  
  TObject * tss =  phosFolder->FindObject(name) ;
  if (!tss) {
    cerr << "ERROR: AliPHOSGetter::TrackSegmentsRef -> object " << name << " not found! " << endl ;  
    abort() ;  
  }
  return phosFolder->GetListOfFolders()->GetObjectRef(tss) ;
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostTrackSegmentMaker(AliPHOSTrackSegmentMaker * tsmaker) const 
{ //------------Track Segment Maker ------------------------------
  
  // the hierarchy is //Folders/Tasks/Reconstructioner/PHOS/sdigitsname

  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Ter -> Task //" << fTasksFolder << "/Reconstructioner not found!" << endl;
    return kFALSE ;
  }        
        
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Rer -> //" << fTasksFolder << "/Reconstructioner/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Rer -> Adding //" << fTasksFolder << "/Reconstructioner/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  AliPHOSTrackSegmentMaker * phosts = 
    dynamic_cast<AliPHOSTrackSegmentMaker*>(phos->GetListOfTasks()->FindObject(tsmaker->GetName())) ; 
  if (phosts) { 
    phosts->Delete() ;
    phos->GetListOfTasks()->Remove(phosts) ;
  }
  phos->Add(tsmaker) ;      
  return kTRUE; 
  
} 
//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostTrackSegmentMaker(const char * name) const 
{ //------------Track Segment Maker ------------------------------
  
  // the hierarchy is //Folders/Tasks/Reconstructioner/PHOS/sdigitsname
  
  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 
  
  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Ter -> Task //" << fTasksFolder->GetName() << "/Reconstructioner not found!" << endl;
    return kFALSE ;
  }        
  
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Ter -> //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Ter -> Adding //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  TList * l = phos->GetListOfTasks() ;   
  TIter it(l) ;
  TString tsn(name);
  tsn+=":tsm" ; 
  TTask * task ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(tsn))
      return kTRUE ;
  }
  
  AliPHOSTrackSegmentMakerv1 * phosts = new AliPHOSTrackSegmentMakerv1() ;
  tsn+="-v1" ;
  phosts->SetName(tsn) ;
  phosts->SetTitle(fHeaderFile) ;
  phos->Add(phosts) ;      
  return kTRUE; 
  
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::TSMakerRef(const char * name) const 
{ //------------Track Segment Maker ------------------------------
  
  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::TSMakerRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner not found!" << endl;
    abort() ;
  }        
        
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    cerr <<"WARNING: AliPHOSGetter::TSMakerRef -> //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS not found!" << endl; 
    abort()  ; 
  }   

  TList * l = phos->GetListOfTasks() ; 
  TIter it(l) ;
  TTask * task ;
  TTask * tsm = 0 ;
  TString tsmname(name) ;
  tsmname+=":tsm" ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(tsmname)){
      tsm = task ;
      break ;
    }
  }
  
  if(tsm) 
    return l->GetObjectRef(tsm) ;
  else {
    cerr << "ERROR: AliPHOSGetter::TSMakerRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS/TrackSegmentMarker/" << name << " not found!" << endl;
    abort() ;
  } 
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostRecParticles(const char * name) const 
{  // -------------------- RecParticles ------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/RecParticles/name

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS")) ; 
  
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post RPa -> Folder //" << fRecoFolder->GetName() << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Rpa -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/" << endl;
    }
    phosFolder = fRecoFolder->AddFolder("PHOS", "Reconstructed data from PHOS") ;  
  }    

 TFolder * phosRPaFolder  = dynamic_cast<TFolder*>(phosFolder->FindObject("RecParticles")) ;
  if ( !phosRPaFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post RPa -> Folder //" << fRecoFolder->GetName() << "/PHOS/RecParticles/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post RPa -> Adding Folder //" << fRecoFolder->GetName() << "/PHOS/RecParticles/" << endl;
    }
    phosRPaFolder = phosFolder->AddFolder("RecParticles", "RecParticles from PHOS") ;  
  } 

  TObject * rps = phosRPaFolder->FindObject( name )  ;
  if ( !rps ) {
    TClonesArray * rp = new TClonesArray("AliPHOSRecParticle",100) ;
    rp->SetName(name) ;    
    phosRPaFolder->Add(rp) ;  
  }
  return kTRUE; 
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::RecParticlesRef(const char * name) const 
{ // ---------------RecParticles -----------------------------------
  
  // the hierarchy is //Folders/Run/Event/RecData/PHOS/TrackSegments/name

 if ( !fRecoFolder ) {
    cerr << "ERROR: AliPHOSGetter::RecParticlesRef -> Folder//" << fRecoFolder->GetName() << " not found!" << endl; 
    abort() ; 
  }    

  TFolder * phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/RecParticles")) ; 
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::RecParticlesRef -> Folder //" << fRecoFolder->GetName() << "/PHOS/RecParticles/ not found!" << endl;
    abort() ;
  }    

  TObject * tss =  phosFolder->FindObject(name  ) ;
  if (!tss) {
    cerr << "ERROR: AliPHOSGetter::RecParticlesRef -> object " << name << " not found! " << endl ; 
    abort() ;  
  }
  return phosFolder->GetListOfFolders()->GetObjectRef(tss) ;
}

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostPID(AliPHOSPID * pid) const 
{      // ------------AliPHOS PID -----------------------------

  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Per -> Task //" << fTasksFolder << "/Reconstructioner not found!" << endl;
    return kFALSE ;
  }        
  
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Per -> //" << fTasksFolder << "/Reconstructioner/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Per -> Adding //" << fTasksFolder << "/Reconstructioner/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  AliPHOSPID * phospid = dynamic_cast<AliPHOSPID*>(phos->GetListOfTasks()->FindObject(pid->GetName())) ; 
  if (phospid) { 
    if (fDebug)
      cout << "INFO: AliPHOSGetter::Post Per -> Task " << pid->GetName()
	   << " already exists" << endl ; 
    phos->GetListOfTasks()->Remove(phospid) ;
  }
  
  phos->Add(pid) ;      
  return kTRUE; 
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostPID(const char * name) const 
{     
  // the hierarchy is //Folders/Tasks/Reconstructioner/PHOS/sdigitsname
  
  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::Post Per -> Task //" << fTasksFolder->GetName() << "/Reconstructioner not found!" << endl;
    return kFALSE ;
  }        
  
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    if (fDebug) {
      cout <<"WARNING: AliPHOSGetter::Post Per -> //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS not found!" << endl; 
      cout <<"INFO: AliPHOSGetter::Post Per -> Adding //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS" << endl;
    }
    phos = new TTask("PHOS", "") ; 
    tasks->Add(phos) ; 
  } 

  TList * l = phos->GetListOfTasks() ;   
  TIter it(l) ;
  TString pidname(name) ;
  pidname+=":pid" ;
  TTask * task ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(pidname))
      return kTRUE ;
  }
 
  AliPHOSPIDv1 * phospid = new AliPHOSPIDv1() ;
  pidname+="-v1" ;
  phospid->SetName(pidname) ; 
  phospid->SetTitle(fHeaderFile) ;
  phos->Add(phospid) ;      
  
  return kTRUE; 
} 

//____________________________________________________________________________ 
TObject** AliPHOSGetter::PIDRef(const char * name) const 
{ //------------PID ------------------------------

  TTask * tasks  = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ; 

  if ( !tasks ) {
    cerr << "ERROR: AliPHOSGetter::PIDRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner not found!" << endl;
    abort() ;
  }        
        
  TTask * phos = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if ( !phos )  {
    cerr << "ERROR: AliPHOSGetter::PIDRef -> //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS not found!" << endl; 
    abort()  ; 
  }   
  
  TList * l = phos->GetListOfTasks() ; 
  TIter it(l) ;
  TTask * task ;
  TTask * pid = 0 ;
  TString pidname(name) ;
  pidname+=":pid" ;
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(pidname)){
      pid = task ;
      break ;
    }
  }
  
  if(pid) 
    return l->GetObjectRef(pid) ;
  else {
    cerr << "ERROR: AliPHOSGetter::PIDRef -> Task //" << fTasksFolder->GetName() << "/Reconstructioner/PHOS/PID/" <<  name << " not found!" << endl;
    abort() ;
  }
  
} 

//____________________________________________________________________________ 
const Bool_t AliPHOSGetter::PostQA(void) const 
{ // ------------------ QA ---------------------------------

  // the hierarchy is //Folders/Run/Conditions/QA/PHOS/alarmsName

  TFolder * phosFolder = dynamic_cast<TFolder*>(fQAFolder->FindObject("PHOS")) ; 
  if ( !phosFolder ) {
    if (fDebug) {
      cout << "WARNING: AliPHOSGetter::Post Q -> Folder //" << fQAFolder << "/PHOS/ not found!" << endl;
      cout << "INFO:    AliPHOSGetter::Post Q -> Adding Folder //" << fQAFolder << "/PHOS/" << endl;
    }
    phosFolder = fQAFolder->AddFolder("PHOS", "QA from PHOS") ; 
  }      

  return kTRUE;
}

//____________________________________________________________________________ 
TObject** AliPHOSGetter::AlarmsRef(void) const 
{  //------- Alarms ----------------------

  
  // the hierarchy is //Folders/Run/Conditions/QA/PHOS
  if ( !fQAFolder ) {
    cerr << "ERROR: AliPHOSGetter::AlarmsRef -> Folder //" << fQAFolder << " not found!" << endl;
    abort() ;
  }    
 
  TFolder * phosFolder = dynamic_cast<TFolder *>(fQAFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) {
    cerr << "ERROR: AliPHOSGetter::AlarmsRef -> Folder //" << fQAFolder << "/PHOS/ not found!" << endl;
    abort() ;
  }
   
  return fQAFolder->GetListOfFolders()->GetObjectRef(phosFolder) ;
}


//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeK(TString filename)  
{

  // returns TreeK from file filename
  // usefull in case of split file

  if ( filename.IsNull() ) 
    filename = fHeaderFile ; 

  TFile * file = 0 ; 
  file = static_cast<TFile*>(gROOT->GetFile(filename.Data() ) ) ;
  if (!file) {  // file not yet open 
    file = TFile::Open(filename.Data(), "read") ;    
  }    
  TString treeName("TreeK") ; 
  treeName += EventNumber()  ; 
  TTree * tree = static_cast<TTree *>(file->Get(treeName.Data())) ;
  if (!tree && fDebug)  
    cout << "WARNING: AliPHOSGetter::TreeK -> " << treeName.Data() << " not found in " << filename.Data() << endl ; 
  
  return tree ; 		      
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeH(TString filename)  
{

  // returns TreeH from file filename
  // usefull in case of split file

  if ( filename.IsNull() ) 
    filename = fHeaderFile ; 

  TFile * file = 0 ; 
  file = static_cast<TFile*>(gROOT->GetFile(filename.Data() ) ) ;
  if (!file) { // file not open yet
    file = TFile::Open(filename.Data(), "read") ; 
  }
  TString treeName("TreeH") ; 
  treeName += EventNumber()  ; 
  TTree * tree = static_cast<TTree *>(file->Get(treeName.Data())) ;
  if (!tree && fDebug)  
    cout << "WARNING: AliPHOSGetter::TreeH -> " << treeName.Data() << " not found in " << filename.Data() << endl ; 
  
  return tree ; 		      
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeS(TString filename)  
{

  // returns TreeS from file filename
  // usefull in case of split file

  if ( filename.IsNull() ) 
    filename = fHeaderFile ; 

  TFile * file = 0 ; 
  file = static_cast<TFile*>(gROOT->GetFile(filename.Data() ) ) ;
  if (!file) { // file not open yet
    file = TFile::Open(filename.Data(), "read") ; 
  }
  TString treeName("TreeS") ; 
  treeName += EventNumber()  ; 
  TTree * tree = static_cast<TTree *>(file->Get(treeName.Data())) ;
  if (!tree && fDebug)  
    cout << "WARNING: AliPHOSGetter::TreeS -> " << treeName.Data() << " not found in " << filename.Data() << endl ; 
  
  return tree ; 		      
}

//____________________________________________________________________________ 
TTree * AliPHOSGetter::TreeD(TString filename)  
{

  // returns TreeD from file filename
  // usefull in case of split file

  if ( filename.IsNull() ) 
    filename = fHeaderFile ; 

  TFile * file = 0 ; 
  file = static_cast<TFile*>(gROOT->GetFile(filename.Data() ) ) ;
  if (!file) { // file not open yet
    file = TFile::Open(filename.Data(), "read") ; 
  }
  TString treeName("TreeD") ; 
  treeName += EventNumber()  ; 
  TTree * tree = static_cast<TTree *>(file->Get(treeName.Data())) ;
  if (!tree && fDebug)  
    cout << "WARNING: AliPHOSGetter::TreeD -> " << treeName.Data() << " not found in " << filename.Data() << endl ; 
  
  return tree ; 		      
}

//____________________________________________________________________________ 
const TParticle * AliPHOSGetter::Primary(Int_t index) const 
{
  // Return primary particle numbered by <index>

  if(index < 0) 
    return 0 ;
  TParticle *  p = 0 ;

  p = gAlice->Particle(index) ; 
  
  return p ; 
    
}

//____________________________________________________________________________ 
const TParticle * AliPHOSGetter::Secondary(TParticle* p, Int_t index) const
{
  // Return first (index=1) or second (index=2) secondary particle of primary particle p 

  if(index <= 0) 
    return 0 ;
  if(index > 2)
    return 0 ;

  if(p) {
  Int_t daughterIndex = p->GetDaughter(index-1) ; 
  return  gAlice->Particle(daughterIndex) ; 
  }
  else
    return 0 ;
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeD(const Int_t event)
{
  // Read the digit tree gAlice->TreeD()  
  
  TTree * treeD ;
  if(fToSplit){
    TFile * file = static_cast<TFile*>(gROOT->GetFile(fDigitsFileName)); 
    if(!file) 
      file = TFile::Open(fDigitsFileName) ;      
    // Get Digits Tree header from file
    TString treeName("TreeD") ;
    treeName += event ; 
    treeD = dynamic_cast<TTree*>(file->Get(treeName.Data()));
    if(!treeD){ // TreeD not found in header file
      if (fDebug)
	cout << "WARNING: AliPHOSGetter::ReadTreeD -> Cannot find TreeD in " << fDigitsFileName.Data() << endl;
      return 1;
    }
  }
  else
    treeD = gAlice->TreeD() ;
  
  TObjArray * lob = static_cast<TObjArray*>(treeD->GetListOfBranches()) ;
  TIter next(lob) ; 
  TBranch * branch = 0 ; 
  TBranch * digitsbranch = 0 ; 
  TBranch * digitizerbranch = 0 ; 
  Bool_t phosfound = kFALSE, digitizerfound = kFALSE ; 
  
  while ( (branch = static_cast<TBranch*>(next())) && (!phosfound || !digitizerfound) ) {
    if ( (strcmp(branch->GetName(), "PHOS")==0) && (strcmp(branch->GetTitle(), fDigitsTitle)==0) ) {
      digitsbranch = branch ; 
      phosfound = kTRUE ;
    }
    else if ( (strcmp(branch->GetName(), "AliPHOSDigitizer")==0) && (strcmp(branch->GetTitle(), fDigitsTitle)==0) ) {
      digitizerbranch = branch ; 
      digitizerfound = kTRUE ; 
    }
  }

  if ( !phosfound || !digitizerfound ) {
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeD -> Cannot find Digits and/or Digitizer with name " 
	   << fDigitsTitle << endl ;
    return 2; 
  }   
 
  //read digits
  if(!Digits(fDigitsTitle) ) 
    PostDigits(fDigitsTitle);
  digitsbranch->SetAddress(DigitsRef(fDigitsTitle)) ;
  digitsbranch->GetEntry(0) ;
  
  
  // read  the Digitizer
  RemoveTask("D", fDigitsTitle) ; // I do not understand why I need that 
  if(!Digitizer(fDigitsTitle))
    PostDigitizer(fDigitsTitle) ;
  digitizerbranch->SetAddress(DigitizerRef(fDigitsTitle)) ;
  digitizerbranch->GetEntry(0) ;
 
  //  lob  ->Delete();
  if(gAlice->TreeD()!=treeD)
    treeD->Delete();
  return 0 ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeH()
{
  // Read the first entry of PHOS branch in hit tree gAlice->TreeH()
  
  TTree * treeH = gAlice->TreeH() ;

  if(!treeH) {// TreeH not found in header file
 
    if (fDebug) 
      cout <<   "WARNING: AliPHOSGetter::ReadTreeH -> Cannot find TreeH in " << fHeaderFile << endl ;
    
    TString searchFileName("PHOS.Hits") ; 
    if((strcmp(fBranchTitle.Data(),"Default")!=0)&&(strcmp(fBranchTitle.Data(),"")!=0)){
      searchFileName+="." ;
      searchFileName += fBranchTitle ;
    }
    searchFileName+=".root" ;
    
    if ( (treeH = TreeH(searchFileName)) ) { //found TreeH in the file which contains the hits
      if (fDebug) 
	cout << "INFO: AliPHOSGetter::ReadTreeH -> TreeH found in " << searchFileName.Data() << endl ; 
      
    } else {
      cerr << "ERROR: AliPHOSGetter::ReadTreeH -> TreeH not found " << endl ; 
      return 1;
    }  
  }
  
  TBranch * hitsbranch = static_cast<TBranch*>(treeH->GetBranch("PHOS")) ;
  if ( !hitsbranch ) {
    if (fDebug)
      cout << "WARNING:  AliPHOSGetter::ReadTreeH -> Cannot find branch PHOS" << endl ; 
    return 2;
  }
  if(!Hits())
    PostHits() ;

  if (hitsbranch->GetEntries() > 1 ) {
    (dynamic_cast<TClonesArray*> (*HitsRef()))->Clear() ;
    TClonesArray * tempo =  new TClonesArray("AliPHOSHit",1000) ;
    TClonesArray * hits = dynamic_cast<TClonesArray*>(*HitsRef()) ; 
    hitsbranch->SetAddress(&tempo) ;
    Int_t index = 0 ; 
    Int_t i = 0 ;
    for (i = 0 ; i < hitsbranch->GetEntries() ; i++) {
      hitsbranch->GetEntry(i) ;
      Int_t j = 0 ; 
      for ( j = 0 ; j < tempo->GetEntries() ; j++) { 
	const AliPHOSHit * hit = static_cast<const AliPHOSHit *>(tempo->At(j)) ; 
	new((*hits)[index]) AliPHOSHit( *hit ) ;
	index++ ; 
      }
    }
    delete tempo ; 
  }
  else {
    (dynamic_cast<TClonesArray*> (*HitsRef()))->Clear() ;
    hitsbranch->SetAddress(HitsRef()) ;
    hitsbranch->GetEntry(0) ;
  }
  return 0 ; 
}

//____________________________________________________________________________ 
void AliPHOSGetter::Track(const Int_t itrack) 
{
  // Read the first entry of PHOS branch in hit tree gAlice->TreeH()

  if(gAlice->TreeH()== 0){
    cerr <<   "ERROR: AliPHOSGetter::ReadTreeH: -> Cannot read TreeH " << endl ;
    return ;
  }
  
  TBranch * hitsbranch = dynamic_cast<TBranch*>(gAlice->TreeH()->GetListOfBranches()->FindObject("PHOS")) ;
  if ( !hitsbranch ) {
    if (fDebug)
      cout << "WARNING:  AliPHOSGetter::ReadTreeH -> Cannot find branch PHOS" << endl ; 
    return ;
  }  
  if(!Hits())
    PostHits() ;

  (dynamic_cast<TClonesArray*> (*HitsRef()))->Clear() ;
  hitsbranch->SetAddress(HitsRef()) ;
  hitsbranch->GetEntry(itrack) ;

}

//____________________________________________________________________________ 
void AliPHOSGetter::ReadTreeQA()
{
  // Read the digit tree gAlice->TreeQA()
  // so far only PHOS knows about this Tree  

  if(PHOS()->TreeQA()== 0){
    cerr <<   "ERROR: AliPHOSGetter::ReadTreeQA: can not read TreeQA " << endl ;
    return ;
  }
  
  TBranch * qabranch = PHOS()->TreeQA()->GetBranch("PHOS") ; 
  if (!qabranch) { 
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeQA -> Cannot find QA Alarms for PHOS" << endl ;
    return ; 
  }   
  
  if(!Alarms())
    PostQA() ; 

  qabranch->SetAddress(AlarmsRef()) ;

  qabranch->GetEntry(0) ;
 
//   PostQA("PHOS") ; 
//   TFolder * alarmsF = Alarms() ; 
//   alarmsF->Clear() ; 
//   qabranch->SetAddress(&alarmsF) ;
//   qabranch->GetEntry(0) ;
  
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeR(const Int_t event)
{
  // Read the reconstrunction tree gAlice->TreeR()
  // A particularity has been introduced here :
  //  if gime->Event(ievent,"R") is called branches with the current title are read, the current title
  //   being for example give in AliPHOSPID(fileName, title)
  //  if gime(Event(ievent, "RA") is called the title of the branches is not checked anymore, "A" stands for any
  // This is a feature needed by PID to be able to reconstruct several times particles (each time a ther title is given)
  // from a given set of TrackSegments (with a given name)
  // This is why any is NOT used to read the branch of RecParticles
  // any migh have become obsolete : to be checked
  // See AliPHOSPIDv1    

  TTree * treeR ;
  if(fToSplit){
    TFile * file = static_cast<TFile*>(gROOT->GetFile(fRecPointsFileName)); 
    if(!file) 
      file = TFile::Open(fRecPointsFileName) ;      
    // Get Digits Tree header from file
    TString treeName("TreeR") ;
    treeName += event ; 
    treeR = dynamic_cast<TTree*>(file->Get(treeName.Data()));
    if(!treeR){ // TreeR not found in header file
      if (fDebug)
	cout << "WARNING: AliPHOSGetter::ReadTreeD -> Cannot find TreeR in " << fRecPointsFileName.Data() << endl;
      return 1;
    }
  }
  else
    treeR = gAlice->TreeR() ;
  
  // RecPoints 
  TObjArray * lob = static_cast<TObjArray*>(treeR->GetListOfBranches()) ;
  TIter next(lob) ; 
  TBranch * branch = 0 ; 
  TBranch * emcbranch = 0 ; 
  TBranch * cpvbranch = 0 ; 
  TBranch * clusterizerbranch = 0 ; 
  Bool_t phosemcrpfound = kFALSE, phoscpvrpfound = kFALSE, clusterizerfound = kFALSE ; 

  
  while ( (branch = static_cast<TBranch*>(next())) && (!phosemcrpfound || !phoscpvrpfound || !clusterizerfound) ) {
    if(strcmp(branch->GetTitle(), fRecPointsTitle)==0 ) {
      if ( strcmp(branch->GetName(), "PHOSEmcRP")==0) {
	emcbranch = branch ; 
	phosemcrpfound = kTRUE ;
      }
      else if ( strcmp(branch->GetName(), "PHOSCpvRP")==0) {
	cpvbranch = branch ; 
	phoscpvrpfound = kTRUE ;
      }
      else if(strcmp(branch->GetName(), "AliPHOSClusterizer")==0){
	clusterizerbranch = branch ; 
	clusterizerfound = kTRUE ; 
      }
    }
  }

  if ( !phosemcrpfound || !phoscpvrpfound || !clusterizerfound) {
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeR -> Cannot find RecPoints and/or Clusterizer with name " 
	   << fRecPointsTitle << endl ;
 
  } else { 
    if(!EmcRecPoints(fRecPointsTitle) ) 
      PostRecPoints(fRecPointsTitle) ;
    
    emcbranch->SetAddress(EmcRecPointsRef(fRecPointsTitle)) ;
    emcbranch->GetEntry(0) ;

    cpvbranch->SetAddress(CpvRecPointsRef(fRecPointsTitle)) ; 
    cpvbranch->GetEntry(0) ;  
    
    if(!Clusterizer(fRecPointsTitle) )
      PostClusterizer(fRecPointsTitle) ;
    
    clusterizerbranch->SetAddress(ClusterizerRef(fRecPointsTitle)) ;
    clusterizerbranch->GetEntry(0) ;
  }
  
  //------------------- TrackSegments ---------------------
  next.Reset() ; 
  TBranch * tsbranch = 0 ; 
  TBranch * tsmakerbranch = 0 ; 
  Bool_t phostsfound = kFALSE, tsmakerfound = kFALSE ; 
  while ( (branch = static_cast<TBranch*>(next())) && (!phostsfound || !tsmakerfound) ) {
    if(strcmp(branch->GetTitle(), fTrackSegmentsTitle)==0 )  {
      if ( strcmp(branch->GetName(), "PHOSTS")==0){
	tsbranch = branch ; 
	phostsfound = kTRUE ;
      }
      else if(strcmp(branch->GetName(), "AliPHOSTrackSegmentMaker")==0) {
	tsmakerbranch = branch ; 
	tsmakerfound  = kTRUE ; 
      }
    }
  }

  if ( !phostsfound || !tsmakerfound ) {
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeR -> Cannot find TrackSegments and/or TrackSegmentMaker with name "
	   << fTrackSegmentsTitle << endl ;
  } else { 
    // Read and Post the TrackSegments
    if(!TrackSegments(fTrackSegmentsTitle))
      PostTrackSegments(fTrackSegmentsTitle) ;
    tsbranch->SetAddress(TrackSegmentsRef(fTrackSegmentsTitle)) ;
    tsbranch->GetEntry(0) ;

    // Read and Post the TrackSegment Maker
    if(!TrackSegmentMaker(fTrackSegmentsTitle))
      PostTrackSegmentMaker(fTrackSegmentsTitle) ;
    tsmakerbranch->SetAddress(TSMakerRef(fTrackSegmentsTitle)) ;
    tsmakerbranch->GetEntry(0) ;
 }
  
  
  //------------ RecParticles ----------------------------
  next.Reset() ; 
  TBranch * rpabranch = 0 ; 
  TBranch * pidbranch = 0 ; 
  Bool_t phosrpafound = kFALSE, pidfound = kFALSE ; 
  
  while ( (branch = static_cast<TBranch*>(next())) && (!phosrpafound || !pidfound) ) 
    if(strcmp(branch->GetTitle(), fRecParticlesTitle)==0) {   
      if ( strcmp(branch->GetName(), "PHOSRP")==0) {   
	rpabranch = branch ; 
	phosrpafound = kTRUE ;
      }
      else if (strcmp(branch->GetName(), "AliPHOSPID")==0) {
	pidbranch = branch ; 
	pidfound  = kTRUE ; 
      }
    }
  
  if ( !phosrpafound || !pidfound ) {
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeR -> Cannot find RecParticles and/or PID with name " 
	   << fRecParticlesTitle << endl ; 
  } else { 
    // Read and Post the RecParticles
    if(!RecParticles(fRecParticlesTitle)) 
      PostRecParticles(fRecParticlesTitle) ;
    rpabranch->SetAddress(RecParticlesRef(fRecParticlesTitle)) ;
    rpabranch->GetEntry(0) ;
    // Read and Post the PID
    if(!PID(fRecParticlesTitle))
      PostPID(fRecParticlesTitle) ;
    pidbranch->SetAddress(PIDRef(fRecParticlesTitle)) ;
    pidbranch->GetEntry(0) ;
  }

  if(gAlice->TreeR()!=treeR)
    treeR->Delete();
  return 0 ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeS(const Int_t event)
{
  // Reads the SDigits treeS from all files  
  // Files, which should be opened are listed in phosF
  // So, first get list of files
  TFolder * phosF = dynamic_cast<TFolder *>(fSDigitsFolder->FindObject("PHOS")) ;
  if (!phosF) 
    phosF = fSDigitsFolder->AddFolder("PHOS", "SDigits from PHOS") ; 
  TCollection * folderslist = phosF->GetListOfFolders() ; 
  
  // Now iterate over the list of files and read TreeS into Whiteboard
  TIter next(folderslist) ; 
  TFolder * folder = 0 ; 
  TFile * file; 
  TTree * treeS = 0;
  while ( (folder = static_cast<TFolder*>(next())) ) {
    TString fileName("") ;
    if(fToSplit)
      fileName = folder->GetTitle() ;
    else
      fileName = folder->GetName() ; 
    fileName.ReplaceAll("_","/") ; 
    file = static_cast<TFile*>(gROOT->GetFile(fileName)); 
    if(!file) 
      file = TFile::Open(fileName) ;      
    // Get SDigits Tree header from file
    TString treeName("TreeS") ;
    treeName += event ; 
    treeS = dynamic_cast<TTree*>(file->Get(treeName.Data()));

    if(!treeS){ // TreeS not found in header file
      if (fDebug)
	cout << "WARNING: AliPHOSGetter::ReadTreeS -> Cannot find TreeS in " << fileName.Data() << endl;
      return 1;
    }
    
    //set address of the SDigits and SDigitizer
    TBranch   * sdigitsBranch    = 0;
    TBranch   * sdigitizerBranch = 0;
    TBranch   * branch           = 0 ;  
    TObjArray * lob = static_cast<TObjArray*>(treeS->GetListOfBranches()) ;
    TIter next(lob) ; 
    Bool_t phosfound = kFALSE, sdigitizerfound = kFALSE ; 

    while ( (branch = static_cast<TBranch*>(next())) && (!phosfound || !sdigitizerfound) ) {
      if ( (strcmp(branch->GetName(), "PHOS")==0) && (strcmp(branch->GetTitle(), fSDigitsTitle)==0) ) {
	phosfound = kTRUE ;
	sdigitsBranch = branch ; 
      }
      
      else if ( (strcmp(branch->GetName(), "AliPHOSSDigitizer")==0) && 
		(strcmp(branch->GetTitle(), fSDigitsTitle)==0) ) {
	sdigitizerfound = kTRUE ; 
	sdigitizerBranch = branch ;
      }
    }
    if ( !phosfound || !sdigitizerfound ) {
      if (fDebug)
	cout << "WARNING: AliPHOSDigitizer::ReadSDigits -> Digits and/or Digitizer branch with name " << GetName() 
	     << " not found" << endl ;
      return 2; 
    }   
    
    if ( !folder->FindObject(fSDigitsTitle) )  
      PostSDigits(fSDigitsTitle,folder->GetName()) ;

    ((TClonesArray*) (*SDigitsRef(fSDigitsTitle,folder->GetName())))->Clear() ;
    sdigitsBranch->SetAddress(SDigitsRef(fSDigitsTitle,folder->GetName())) ;
    sdigitsBranch->GetEntry(0) ;
    
    TString sdname(fSDigitsTitle) ;
    sdname+=":" ;
    sdname+=folder->GetName() ;
    if(!SDigitizer(sdname) ) 
      PostSDigitizer(fSDigitsTitle,folder->GetName()) ;
    sdigitizerBranch->SetAddress(SDigitizerRef(sdname)) ;
    sdigitizerBranch->GetEntry(0) ; 
    if(gAlice->TreeS()!=treeS)
      treeS->Delete();
  }    
  return 0 ; 
}

//____________________________________________________________________________ 
void AliPHOSGetter::ReadTreeS(TTree * treeS, Int_t input)

{  // Read the summable digits fron treeS()  


  TString filename("mergefile") ;
  filename+= input ;

  TFolder * phosFolder = dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ; 
  if ( !phosFolder ) { 
   phosFolder = fSDigitsFolder->AddFolder("PHOS", "SDigits from PHOS") ; 
  } 
  TFolder * folder=(TFolder*)phosFolder->FindObject(filename) ;
  //set address of the SDigits and SDigitizer
  TBranch   * sdigitsBranch    = 0;
  TBranch   * sdigitizerBranch = 0;
  TBranch   * branch           = 0 ;  
  TObjArray * lob = (TObjArray*)treeS->GetListOfBranches() ;
  TIter next(lob) ; 
  Bool_t phosfound = kFALSE, sdigitizerfound = kFALSE ; 
  
  while ( (branch = (TBranch*)next()) && (!phosfound || !sdigitizerfound) ) {
    if ( strcmp(branch->GetName(), "PHOS")==0) {
      phosfound = kTRUE ;
      sdigitsBranch = branch ; 
    }
    
    else if ( strcmp(branch->GetName(), "AliPHOSSDigitizer")==0) {
      sdigitizerfound = kTRUE ; 
      sdigitizerBranch = branch ;
    }
  }
  if ( !phosfound || !sdigitizerfound ) {
    if (fDebug)
      cout << "WARNING: AliPHOSGetter::ReadTreeS -> Digits and/or Digitizer branch not found" << endl ;
    return ; 
  }   
  
  if (!folder || !(folder->FindObject(sdigitsBranch->GetTitle()) ) )
    PostSDigits(sdigitsBranch->GetTitle(),filename) ;

  sdigitsBranch->SetAddress(SDigitsRef(sdigitsBranch->GetTitle(),filename)) ;
  sdigitsBranch->GetEntry(0) ;
  
  TString sdname(sdigitsBranch->GetTitle()) ;
  sdname+=":" ;
  sdname+=filename ;
  
  if(!SDigitizer(sdigitsBranch->GetTitle()) )
    PostSDigitizer(sdigitsBranch->GetTitle(),filename) ;
  sdigitizerBranch->SetAddress(SDigitizerRef(sdname)) ;
  sdigitizerBranch->GetEntry(0) ;
  if(gAlice->TreeS()!=treeS)
    treeS->Delete();
}    


//____________________________________________________________________________ 
void AliPHOSGetter::ReadPrimaries()
{
  // a lot simplified.... if 2 files are opened then we have a problem

  TClonesArray * ar = 0  ; 
  if(! (ar = Primaries()) ) { 
    PostPrimaries() ;
    ar = Primaries() ; 
  }
  ar->Delete() ; 
  
  if (TreeK(fHeaderFile)) { // treeK found in header file
    if (fDebug) 
      cout << "INFO: AliPHOSGetter::ReadPrimaries -> TreeK found in " << fHeaderFile.Data() << endl ; 
    fNPrimaries = gAlice->GetNtrack() ; 
  
  } else { // treeK not found in header file
    
    cerr << "ERROR: AliPHOSGetter::ReadPrimaries -> TreeK not  found " << endl ; 
    return ;
    
  }
  Int_t index = 0 ; 
  for (index = 0 ; index < fNPrimaries; index++) { 
    new ((*ar)[index]) TParticle(*(Primary(index)));
  }
}

//____________________________________________________________________________ 
void AliPHOSGetter::Event(const Int_t event, const char* opt)  
{
  // Reads the content of all Tree's S, D and R

  if (event >= gAlice->TreeE()->GetEntries() ) {
    cerr << "ERROR: AliPHOSGetter::Event -> " << event << " not found in TreeE!" << endl ; 
    return ; 
  }

  Bool_t any = kFALSE ; 
  if (strstr(opt,"A") ) // do not check the title of the branches
    any = kTRUE; 

  gAlice->GetEvent(event) ; 

  if( strstr(opt,"R") )
    ReadTreeR(event) ;

  if( strstr(opt,"D") )
    ReadTreeD(event) ;

  if(strstr(opt,"S") )
    ReadTreeS(event) ;

  if(strstr(opt,"H") )
    ReadTreeH() ;
   
  if( strstr(opt,"Q") )
    ReadTreeQA() ;
 
  if( strstr(opt,"P"))
    ReadPrimaries() ;
  
}

//____________________________________________________________________________ 
TObject * AliPHOSGetter::ReturnO(TString what, TString name, TString file) const 
{
  // get the object named "what" from the folder
  // folders are named like //Folders

  if ( file.IsNull() ) 
    file = fHeaderFile ; 

  TFolder * folder = 0 ;
  TObject * phosO  = 0 ; 

  if ( what.CompareTo("Primaries") == 0 ) {
    folder = dynamic_cast<TFolder *>(fPrimariesFolder->FindObject("Primaries")) ; 
    if (folder) 
      phosO  = dynamic_cast<TObject *>(folder->FindObject("Primaries")) ;  
    else 
      return 0 ; 
  }
  else if ( what.CompareTo("Hits") == 0 ) {
    folder = dynamic_cast<TFolder *>(fHitsFolder->FindObject("PHOS")) ; 
    if (folder) 
      phosO  = dynamic_cast<TObject *>(folder->FindObject("Hits")) ;  
  }
  else if ( what.CompareTo("SDigits") == 0 ) {
    file.ReplaceAll("/","_") ; 
    TString path = "PHOS/" + file  ;
    folder = dynamic_cast<TFolder *>(fSDigitsFolder->FindObject(path.Data())) ; 
    if (folder) { 
      if (name.IsNull())
	name = fSDigitsTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ; 
    }
  }
  else if ( what.CompareTo("Digits") == 0 ){
    folder = dynamic_cast<TFolder *>(fDigitsFolder->FindObject("PHOS")) ; 
    if (folder) { 
      if (name.IsNull())
	name = fDigitsTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ; 
    } 
  }
  else if ( what.CompareTo("EmcRecPoints") == 0 ) {
    folder = dynamic_cast<TFolder *>(fRecoFolder->FindObject("PHOS/EMCARecPoints")) ; 
    if (folder) { 
      if (name.IsNull())
	name = fRecPointsTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ;
    } 
  }
  else if ( what.CompareTo("CpvRecPoints") == 0 ) {
    folder = dynamic_cast<TFolder *>(fRecoFolder->FindObject("PHOS/CPVRecPoints")) ; 
    if (folder) { 
      if (name.IsNull())
	name = fRecPointsTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ; 
    }   
  }
  else if ( what.CompareTo("TrackSegments") == 0 ) {
    folder = dynamic_cast<TFolder *>(fRecoFolder->FindObject("PHOS/TrackSegments")) ; 
    if (folder) { 
      if (name.IsNull())
	name = fTrackSegmentsTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ; 
    }   
  }
  else if ( what.CompareTo("RecParticles") == 0 ) {
    folder = dynamic_cast<TFolder *>(fRecoFolder->FindObject("PHOS/RecParticles")) ; 
   if (folder) { 
      if (name.IsNull())
	name = fRecParticlesTitle ; 
      phosO  = dynamic_cast<TObject *>(folder->FindObject(name)) ;
    }   
 }
  else if ( what.CompareTo("Alarms") == 0 ){ 
    if (name.IsNull() ) 
      phosO = dynamic_cast<TObject *>(fQAFolder->FindObject("PHOS")) ;  
    else {
      folder = dynamic_cast<TFolder *>(fQAFolder->FindObject("PHOS")) ; 
      if (!folder) 
	phosO = 0 ; 
      else 
	phosO = dynamic_cast<TObject *>(folder->FindObject(name)) ;  
    }
  }
  if (!phosO) {
    if(fDebug)
      cerr << "WARNING : AliPHOSGetter::ReturnO -> Object " << what << " not found in PHOS" << endl ; 
    return 0 ;
  }

  return phosO ;
}
  
//____________________________________________________________________________ 
const TTask * AliPHOSGetter::ReturnT(TString what, TString name) const 
{
  // get the TTask named "what" from the folder
  // folders are named like //Folders/Tasks/what/PHOS/name

  TString search(what) ; 
  if ( what.CompareTo("Clusterizer") == 0 ) 
    search = "Reconstructioner" ; 
  else if ( what.CompareTo("TrackSegmentMaker") == 0 ) 
    search = "Reconstructioner" ; 
  else if ( what.CompareTo("PID") == 0 ) 
    search = "Reconstructioner" ; 
  else if ( what.CompareTo("QATasks") == 0 ) 
    search = "QA" ; 

  TTask * tasks = dynamic_cast<TTask*>(fTasksFolder->FindObject(search)) ; 

  if (!tasks) {
    cerr << "ERROR: AliPHOSGetter::ReturnT -> Task " << what << " not found!" << endl ;  
    return 0 ; 
  }

  TTask * phosT = dynamic_cast<TTask*>(tasks->GetListOfTasks()->FindObject("PHOS")) ; 
  if (!phosT) { 
    cerr << "ERROR: AliPHOSGetter::ReturnT -> Task " << what << "/PHOS not found!" << endl ;  
    return 0 ; 
  }
  
  TList * list = phosT->GetListOfTasks() ; 
 
  if (what.CompareTo("SDigitizer") == 0) {  
    if ( name.IsNull() )
      name =  fSDigitsTitle ; 
  } else  if (what.CompareTo("Digitizer") == 0){ 
    if ( name.IsNull() )
      name =  fDigitsTitle ;
  } else  if (what.CompareTo("Clusterizer") == 0){ 
    if ( name.IsNull() )
      name =  fRecPointsTitle ;
    name.Append(":clu") ;
  }
  else  if (what.CompareTo("TrackSegmentMaker") == 0){ 
    if ( name.IsNull() )
      name =  fTrackSegmentsTitle ;
    name.Append(":tsm") ;
  }
  else  if (what.CompareTo("PID") == 0){ 
    if ( name.IsNull() )
      name =  fRecParticlesTitle ;
    name.Append(":pid") ;
  }
  else  if (what.CompareTo("QATasks") == 0){ 
    if ( name.IsNull() )
      return phosT ;
  }
  
  TIter it(list) ;
  TTask * task = 0 ; 
  while((task = static_cast<TTask *>(it.Next()) )){
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(name))
      return task ;
  }
  
  if(fDebug)
    cout << "WARNING: AliPHOSGetter::ReturnT -> Task " << search << "/PHOS/" << name << " not found!" << endl ; 
  return 0 ;
}

//____________________________________________________________________________ 
void AliPHOSGetter::RemoveTask(TString opt, TString name) const 
{
  // remove a task from the folder
  // path is fTasksFolder/SDigitizer/PHOS/name
  
  TTask * task = 0 ; 
  TTask * phos = 0 ; 
  TList * lofTasks = 0 ; 

  if (opt == "S") { // SDigitizer
    task = dynamic_cast<TTask*>(fTasksFolder->FindObject("SDigitizer")) ;
    if (!task) 
      return ; 
  }
  else if (opt == "D") { // Digitizer
    task = dynamic_cast<TTask*>(fTasksFolder->FindObject("Digitizer")) ;
    if (!task) 
      return ; 
  }
  else if (opt == "C" || opt == "T" || opt == "P"  ) { // Clusterizer, TrackSegmentMaker, PID
    task = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ;
    if (!task) 
      return ; 
  }
  else {
    cerr << "WARNING: AliPHOSGetter::RemoveTask -> Unknown option " << opt.Data() << endl ; 
    return ; 
  }
  phos =  dynamic_cast<TTask*>(task->GetListOfTasks()->FindObject("PHOS")) ;
  if (!phos)
    return ; 
  lofTasks = phos->GetListOfTasks() ;
  if (!lofTasks) 
    return ; 
  TObject * obj = lofTasks->FindObject(name) ; 
  if (obj) 
    lofTasks->Remove(obj) ;
   
}

//____________________________________________________________________________ 
void AliPHOSGetter::RemoveObjects(TString opt, TString name) const 
{
  // remove SDigits from the folder
  // path is fSDigitsFolder/fHeaderFileName/name

  TFolder * phos     = 0 ; 
  TFolder * phosmain = 0 ; 

  if (opt == "H") { // Hits
    phos = dynamic_cast<TFolder*>(fHitsFolder->FindObject("PHOS")) ;
    if (!phos) 
      return ;
    name = "Hits" ; 
  }

  else if ( opt == "S") { // SDigits
    phosmain = dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ;
    if (!phosmain) 
      return ;
    phos = dynamic_cast<TFolder*>(phosmain->FindObject(fHeaderFile)) ;
    if (!phos) 
      return ;
  }
  
  else if (opt == "D") { // Digits
    phos = dynamic_cast<TFolder*>(fDigitsFolder->FindObject("PHOS")) ;
    if (!phos) 
      return ;
  }

  else if (opt == "RE") { // EMCARecPoints
    phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/EMCARecPoints")) ;
    if (!phos) 
      return ;
  }

  else if (opt == "RC") { // CPVRecPoints
    phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/CPVRecPoints")) ;
    if (!phos) 
      return ;
  }  

  else if (opt == "T") { // TrackSegments
    phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/TrackSegments")) ;
    if (!phos) 
      return ;
  }

  else if (opt == "P") { // RecParticles
    phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/RecParticles")) ;
    if (!phos) 
      return ;
  }
  
  else {
    cerr << "WARNING: AliPHOSGetter::RemoveObjects -> Unknown option " << opt.Data() << endl ; 
    return ; 
  }
  
  TObjArray * ar  = dynamic_cast<TObjArray*>(phos->FindObject(name)) ; 
  if (ar) { 
    phos->Remove(ar) ;
    ar->Delete() ; 
    delete ar ; 
  }

  if (opt == "S") 
    phosmain->Remove(phos) ; 
  
}

//____________________________________________________________________________ 
void AliPHOSGetter::RemoveSDigits() const 
{
  TFolder * phos= dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ;
  if (!phos) 
    return ;
  
  phos->SetOwner() ; 
  phos->Clear() ; 
}

//____________________________________________________________________________ 
void AliPHOSGetter::CleanWhiteBoard(void){

  TFolder * phosmain = 0 ; 
  TFolder * phos ;
  TObjArray * ar ;
  TList * lofTasks = 0 ; 
  TTask * task = 0 ; 
  TTask * phost = 0 ; 
  
  // Hits  
  phos = dynamic_cast<TFolder*>(fHitsFolder->FindObject("PHOS")) ;
  if (phos){  
    TObjArray * ar  = dynamic_cast<TObjArray*>(phos->FindObject("Hits")) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }
  
  // SDigits
  phosmain = dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ;
  if (phosmain){ 
    phos = dynamic_cast<TFolder*>(phosmain->FindObject(fHeaderFile)) ;
    if (phos) {
      ar  = dynamic_cast<TObjArray*>(phos->FindObject(fSDigitsTitle)) ; 
      if (ar) { 
	phos->Remove(ar) ;
	ar->Delete() ; 
	delete ar ; 
      }
    }
    phosmain->Remove(phos) ; 
  }

  
  // Digits
  phos = dynamic_cast<TFolder*>(fDigitsFolder->FindObject("PHOS")) ;
  if (phos){ 
    ar  = dynamic_cast<TObjArray*>(phos->FindObject(fDigitsTitle)) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }


  // EMCARecPoints
  phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/EMCARecPoints")) ;
  if (phos){ 
    ar  = dynamic_cast<TObjArray*>(phos->FindObject(fRecPointsTitle)) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }

  
  // CPVRecPoints
  phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/CPVRecPoints")) ;
  if (phos){ 
    ar  = dynamic_cast<TObjArray*>(phos->FindObject(fRecPointsTitle)) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }  

  
  // TrackSegments
  phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/TrackSegments")) ;
  if (phos) { 
    ar  = dynamic_cast<TObjArray*>(phos->FindObject(fTrackSegmentsTitle)) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }
  


  // RecParticles
  phos = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS/RecParticles")) ;
  if (phos){ 
    ar  = dynamic_cast<TObjArray*>(phos->FindObject(fRecParticlesTitle)) ; 
    if (ar) { 
      phos->Remove(ar) ;
      ar->Delete() ; 
      delete ar ; 
    }
  }


  //---- Now Tasks ----------- 

  TObject * obj ;
  TString sdname(fSDigitsTitle);
  
  // Digitizer
  task = dynamic_cast<TTask*>(fTasksFolder->FindObject("Digitizer")) ;
  if (task){ 
    phost =  dynamic_cast<TTask*>(task->GetListOfTasks()->FindObject("PHOS")) ;
    if (phost){
      lofTasks = phost->GetListOfTasks() ;
      if (lofTasks){ 
	obj = lofTasks->FindObject(sdname.Data()) ; 
	if (obj) 
	  lofTasks->Remove(obj) ;
      }
    }      
  }
  

  sdname.Append(":") ;
  // Clusterizer, TrackSegmentMaker, PID
  task = dynamic_cast<TTask*>(fTasksFolder->FindObject("Reconstructioner")) ;
  if (task){ 
    phost =  dynamic_cast<TTask*>(task->GetListOfTasks()->FindObject("PHOS")) ;
    if (phost){
      lofTasks = phost->GetListOfTasks() ;
      TIter next(lofTasks);
      while((obj=next())){ 
	TString oname(obj->GetName()) ;
	if (oname.BeginsWith(sdname)){ 
	  lofTasks->Remove(obj) ;
	}
      }
    }  
  }


  // SDigitizer
  sdname.Append(fHeaderFile) ;
  task = dynamic_cast<TTask*>(fTasksFolder->FindObject("SDigitizer")) ;
  if (task) {
    phost =  dynamic_cast<TTask*>(task->GetListOfTasks()->FindObject("PHOS")) ;
    if (phost){
      lofTasks = phost->GetListOfTasks() ;
      if (lofTasks){ 
	obj = lofTasks->FindObject(sdname.Data()) ; 
	if (obj) 
	  lofTasks->Remove(obj) ;
      }
    }
  }  

}
//____________________________________________________________________________ 
void AliPHOSGetter::SetTitle(const char * branchTitle ) 
{
  fBranchTitle        = branchTitle ;
  fSDigitsTitle       = branchTitle ; 
  fDigitsTitle        = branchTitle ; 
  fRecPointsTitle     = branchTitle ; 
  fRecParticlesTitle  = branchTitle ; 
  fTrackSegmentsTitle = branchTitle ; 
  if(fToSplit){
    //First - extract full path if necessary
    TString sFileName(fHeaderFile) ;
    Ssiz_t islash = sFileName.Last('/') ;
    if(islash<sFileName.Length())
      sFileName.Remove(islash+1,sFileName.Length()) ;
    else
      sFileName="" ;
    //Now construct file names
    fSDigitsFileName       = sFileName ;
    fDigitsFileName        = sFileName ; 
    fRecPointsFileName     = sFileName ; 
    fRecParticlesFileName  = sFileName ; 
    fTrackSegmentsFileName = sFileName ; 
    fSDigitsFileName      += "PHOS.SDigits." ;
    fDigitsFileName       += "PHOS.Digits." ; 
    fRecPointsFileName    += "PHOS.RecData." ; 
    fTrackSegmentsFileName+= "PHOS.RecData." ; 
    fRecParticlesFileName += "PHOS.RecData." ; 
    if((strcmp(fBranchTitle.Data(),"Default")!=0)&&(strcmp(fBranchTitle.Data(),"")!=0)){
      fSDigitsFileName      += fBranchTitle ;
      fSDigitsFileName      += "." ;
      fDigitsFileName       += fBranchTitle ; 
      fDigitsFileName       += "." ; 
      fRecPointsFileName    += fBranchTitle ; 
      fRecPointsFileName    += "." ; 
      fRecParticlesFileName += fBranchTitle ; 
      fRecParticlesFileName += "." ; 
      fTrackSegmentsFileName+= fBranchTitle ; 
      fTrackSegmentsFileName+= "." ; 
    }
    fSDigitsFileName      += "root" ;
    fDigitsFileName       += "root" ; 
    fRecPointsFileName    += "root" ; 
    fRecParticlesFileName += "root" ; 
    fTrackSegmentsFileName+= "root" ; 
  }else{
    fSDigitsFileName       = "" ; 
    fDigitsFileName        = "" ; 
    fRecPointsFileName     = "" ; 
    fRecParticlesFileName  = "" ; 
    fTrackSegmentsFileName = "" ; 
  }
  TFolder * phosFolder ; 
  phosFolder = dynamic_cast<TFolder*>(fHitsFolder->FindObject("PHOS")) ; 
  if ( !phosFolder ) 
    phosFolder = fHitsFolder->AddFolder("PHOS", "Hits from PHOS") ; 

  phosFolder = dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) 
    phosFolder = fSDigitsFolder->AddFolder("PHOS", "SDigits from PHOS") ; 
  
  //Make folder for SDigits
  TString subdir(fHeaderFile) ;
  subdir.ReplaceAll("/","_") ;
  phosFolder->AddFolder(subdir, fSDigitsFileName.Data());


  phosFolder  = dynamic_cast<TFolder*>(fDigitsFolder->FindObject("PHOS")) ;
  if ( !phosFolder ) 
    phosFolder = fDigitsFolder->AddFolder("PHOS", "Digits from PHOS") ;  

  phosFolder  = dynamic_cast<TFolder*>(fRecoFolder->FindObject("PHOS")) ; 
  if ( !phosFolder )
    phosFolder = fRecoFolder->AddFolder("PHOS", "Reconstructed data from PHOS") ;  
  

}
//____________________________________________________________________________ 
void AliPHOSGetter::CloseSplitFiles(void){
  TFile * file ;
  file = static_cast<TFile*>(gROOT->GetFile(fSDigitsFileName.Data() ) ) ;
  if(file)
    file->Close() ;
  file = static_cast<TFile*>(gROOT->GetFile(fDigitsFileName.Data() ) ) ;
  if(file)
    file->Close() ;
  file = static_cast<TFile*>(gROOT->GetFile(fRecPointsFileName.Data() ) ) ;
  if(file)
    file->Close() ;
  file = static_cast<TFile*>(gROOT->GetFile(fTrackSegmentsFileName.Data() ) ) ;
  if(file)
    file->Close() ;
  file = static_cast<TFile*>(gROOT->GetFile(fRecParticlesFileName.Data() ) ) ;
  if(file)
    file->Close() ;

}

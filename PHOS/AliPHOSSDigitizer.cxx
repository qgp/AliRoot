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
// This is a TTask that makes SDigits out of Hits
// The name of the TTask is also the title of the branch that will contain 
// the created SDigits
// The title of the TTAsk is the name of the file that contains the hits from
// which the SDigits are created
// A Summable Digits is the sum of all hits originating 
// from one primary in one active cell
// A threshold for assignment of the primary to SDigit is applied 
// SDigits are written to TreeS, branch "PHOS"
// AliPHOSSDigitizer with all current parameters is written 
// to TreeS branch "AliPHOSSDigitizer".
// Both branches have the same title. If necessary one can produce 
// another set of SDigits with different parameters. Two versions
// can be distunguished using titles of the branches.
// User case:
//  root [0] AliPHOSSDigitizer * s = new AliPHOSSDigitizer("galice.root")
//  Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//  root [1] s->ExecuteTask()
//             // Makes SDigitis for all events stored in galice.root
//  root [2] s->SetPedestalParameter(0.001)
//             // One can change parameters of digitization
// root [3] s->SetSDigitsBranch("Pedestal 0.001")
//             // and write them into the new branch
// root [4] s->ExecuteTask("deb all tim")
//             // available parameters:
//             deb - print # of produced SDigitis
//             deb all  - print # and list of produced SDigits
//             tim - print benchmarking information
//
//*-- Author :  Dmitri Peressounko (SUBATECH & KI) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include "TFile.h"
#include "TROOT.h"
#include "TBenchmark.h"

// --- Standard library ---

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliPHOSDigit.h"
#include "AliPHOSGetter.h"
#include "AliPHOSHit.h"
#include "AliPHOSSDigitizer.h"

ClassImp(AliPHOSSDigitizer)

           
//____________________________________________________________________________ 
  AliPHOSSDigitizer::AliPHOSSDigitizer():TTask("","")
{
  // ctor
  InitParameters() ;
  fDefaultInit = kTRUE ; 
}

//____________________________________________________________________________ 
AliPHOSSDigitizer::AliPHOSSDigitizer(const char * alirunFileName, const char * eventFolderName):
  TTask("PHOS"+AliConfig::fgkSDigitizerTaskName, alirunFileName),
  fEventFolderName(eventFolderName)
{

  // ctor
  InitParameters() ; 
  Init();
  fDefaultInit = kFALSE ; 
}

//____________________________________________________________________________ 
AliPHOSSDigitizer::AliPHOSSDigitizer(const AliPHOSSDigitizer & sd) {
  //cpy ctor 

  fA             = sd.fA ;
  fB             = sd.fB ;
  fPrimThreshold = sd.fPrimThreshold ;
  fSDigitsInRun  = sd.fSDigitsInRun ;
  SetName(sd.GetName()) ; 
  SetTitle(sd.GetTitle()) ; 
  fEventFolderName = sd.fEventFolderName;
}

//____________________________________________________________________________ 
AliPHOSSDigitizer::~AliPHOSSDigitizer()
{
  // dtor
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::Init()
{
  // Uses the getter to access the required files
  
  fInit = kTRUE ; 
  
  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName.Data());  
  if ( gime == 0 ) {
    Fatal("Init" ,"Could not obtain the Getter object for file %s and event %s !", GetTitle(), fEventFolderName.Data()) ;  
    return ;
  } 
  
  TString opt("SDigits") ; 
  if(gime->VersionExists(opt) ) { 
    Error( "Init", "Give a version name different from %s", fEventFolderName.Data() ) ;
    fInit = kFALSE ; 
  }

  gime->PostSDigitizer(this);
  gime->PhosLoader()->GetSDigitsDataLoader()->GetBaseTaskLoader()->SetDoNotReload(kTRUE);
  
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::InitParameters()
{ 
  // initializes the parameters for digitization
  fA             = 0;
  fB             = 10000000.;
  fPrimThreshold = 0.01 ;
  fSDigitsInRun  = 0 ;
}

//____________________________________________________________________________
void AliPHOSSDigitizer::Exec(Option_t *option) 
{ 
  // Collects all hits in the same active volume into digit
  
  if (strstr(option, "print") ) {
    Print("") ; 
    return ; 
  }

  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSSDigitizer");
  
  AliPHOSGetter * gime = AliPHOSGetter::Instance() ;

  //switch off reloading of this task while getting event
  if (!fInit) { // to prevent overwrite existing file
    Error( "Exec", "Give a version name different from %s", fEventFolderName.Data() ) ;
    return ;
  }


  Int_t nevents = gime->MaxEvent() ; 
  Int_t ievent ;
  for(ievent = 0; ievent < nevents; ievent++){

    gime->Event(ievent,"H") ;

    TTree * treeS = gime->TreeS(); 
    TClonesArray * hits = gime->Hits() ;
    TClonesArray * sdigits = gime->SDigits() ;
    sdigits->Clear();
    Int_t nSdigits = 0 ;
    //Now make SDigits from hits, for PHOS it is the same, so just copy    
    Int_t nPrim =  static_cast<Int_t>((gime->TreeH())->GetEntries()) ; 
    // Attention nPrim is the number of primaries tracked by Geant 
    // and this number could be different to the number of Primaries in TreeK;
    Int_t iprim ;

    for (iprim = 0 ; iprim < nPrim ; iprim ++) { 
      //=========== Get the PHOS branch from Hits Tree for the Primary iprim
      gime->Track(iprim) ;
      Int_t i;
      for ( i = 0 ; i < hits->GetEntries() ; i++ ) {
	AliPHOSHit * hit = dynamic_cast<AliPHOSHit *>(hits->At(i)) ;
	// Assign primary number only if contribution is significant
	
	if( hit->GetEnergy() > fPrimThreshold)
	  new((*sdigits)[nSdigits]) AliPHOSDigit(hit->GetPrimary(),hit->GetId(),
						 Digitize(hit->GetEnergy()), hit->GetTime()) ;
	else
	  new((*sdigits)[nSdigits]) AliPHOSDigit( -1              , hit->GetId(), 
						  Digitize(hit->GetEnergy()), hit->GetTime()) ;
	nSdigits++ ;	
	
      }
 
    } // loop over iprim

    sdigits->Sort() ;

    nSdigits = sdigits->GetEntriesFast() ;

    fSDigitsInRun += nSdigits ;  
    sdigits->Expand(nSdigits) ;

    Int_t i ;
    for (i = 0 ; i < nSdigits ; i++) { 
      AliPHOSDigit * digit = dynamic_cast<AliPHOSDigit *>(sdigits->At(i)) ; 
      digit->SetIndexInList(i) ;     
    }

    //Now write SDigits

    
    //First list of sdigits

    Int_t bufferSize = 32000 ;
    TBranch * sdigitsBranch = treeS->Branch("PHOS",&sdigits,bufferSize);

    sdigitsBranch->Fill() ;

    gime->WriteSDigits("OVERWRITE");

    //Next - SDigitizer

    Info("Exec", "name = %s", GetName()) ; 
    gime->WriteSDigitizer("OVERWRITE");

    if(strstr(option,"deb"))
      PrintSDigits(option) ;
  }
  
  Unload();

  gime->PhosLoader()->GetSDigitsDataLoader()->GetBaseTaskLoader()->SetDoNotReload(kTRUE);
  
  if(strstr(option,"tim")){
    gBenchmark->Stop("PHOSSDigitizer");
    Info("Exec","   took %f seconds for SDigitizing  %f seconds per event",
	 gBenchmark->GetCpuTime("PHOSSDigitizer"), gBenchmark->GetCpuTime("PHOSSDigitizer")/nevents) ;
  }
}

//__________________________________________________________________
void AliPHOSSDigitizer::SetSDigitsBranch(const char * title )
{
 //  // Setting title to branch SDigits 

//   TString stitle(title) ;

//   // check if branch with title already exists
//   TBranch * sdigitsBranch    = 
//     static_cast<TBranch*>(gAlice->TreeS()->GetListOfBranches()->FindObject("PHOS")) ; 
//   TBranch * sdigitizerBranch =  
//     static_cast<TBranch*>(gAlice->TreeS()->GetListOfBranches()->FindObject("AliPHOSSDigitizer")) ;
//   const char * sdigitsTitle    = sdigitsBranch ->GetTitle() ;  
//   const char * sdigitizerTitle = sdigitizerBranch ->GetTitle() ;
//   if ( stitle.CompareTo(sdigitsTitle)==0 || stitle.CompareTo(sdigitizerTitle)==0 ){
//     Error("SetSDigitsBranch", "Cannot overwrite existing branch with title %s", title) ;
//     return ;
//   }
  
//   Info("SetSDigitsBranch", "-> Changing SDigits file from %s to %s", GetName(), title) ;

//   SetName(title) ; 
    
}


//__________________________________________________________________
void AliPHOSSDigitizer::Print(Option_t* option)const
{
  // Prints parameters of SDigitizer
  TString message ; 
  message  = "\n------------------- %s -------------\n" ;  
  message += "   Writing SDigits to branch with title  %s\n" ;
  message += "   with digitization parameters  A = %f\n" ; 
  message += "                                 B = %f\n" ;
  message += "   Threshold for Primary assignment= %f\n" ; 
  message += "---------------------------------------------------\n" ;
  Info("Print", message.Data(),  GetName(),  fEventFolderName.Data(), fA, fB, fPrimThreshold ) ;
  
}

//__________________________________________________________________
Bool_t AliPHOSSDigitizer::operator==( AliPHOSSDigitizer const &sd )const
{
  // Equal operator.
  // SDititizers are equal if their pedestal, slope and threshold are equal

  if( (fA==sd.fA)&&(fB==sd.fB)&&(fPrimThreshold==sd.fPrimThreshold))
    return kTRUE ;
  else
    return kFALSE ;
}

//__________________________________________________________________
void AliPHOSSDigitizer::PrintSDigits(Option_t * option)
{
  // Prints list of digits produced in the current pass of AliPHOSDigitizer


  AliPHOSGetter * gime = AliPHOSGetter::Instance() ; 
  const TClonesArray * sdigits = gime->SDigits() ;

  TString message ; 
  message  = "\nAliPHOSSDigitiser: event " ;
  message += gAlice->GetEvNumber(); 
  message += "\n      Number of entries in SDigits list " ;  
  message += sdigits->GetEntriesFast() ; 
  char * tempo = new char[8192]; 
  
  if(strstr(option,"all")||strstr(option,"EMC")){
    
    //loop over digits
    AliPHOSDigit * digit;
    message += "\nEMC sdigits\n" ;
    message += "\n   Id  Amplitude    Time          Index Nprim: Primaries list \n" ;    
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; (index < sdigits->GetEntriesFast()) && 
	 ((dynamic_cast<AliPHOSDigit *> (sdigits->At(index)))->GetId() <= maxEmc) ; index++) {
      digit = dynamic_cast<AliPHOSDigit *>( sdigits->At(index) ) ;
      if(digit->GetNprimary() == 0) 
	continue;
      sprintf(tempo, "\n%6d  %8d    %6.5e %4d      %2d :",
	      digit->GetId(), digit->GetAmp(), digit->GetTime(), digit->GetIndexInList(), digit->GetNprimary()) ;  
      message += tempo ; 
      Int_t iprimary;
      for (iprimary=0; iprimary<digit->GetNprimary(); iprimary++) {
	sprintf(tempo, "%d ",digit->GetPrimary(iprimary+1) ) ; 
	message += tempo ; 
      }  
    }    
  }

  if(strstr(option,"all")||strstr(option,"CPV")){
    
    //loop over CPV digits
    AliPHOSDigit * digit;
    
    message += "CPV sdigits\n" ;
    message += "\n   Id  Amplitude    Index Nprim: Primaries list \n" ;    
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; index < sdigits->GetEntriesFast(); index++) {
      digit = dynamic_cast<AliPHOSDigit *>( sdigits->At(index) ) ;
      if(digit->GetId() > maxEmc){
	sprintf(tempo, "\n%6d  %8d    %4d      %2d :",
		digit->GetId(), digit->GetAmp(), digit->GetIndexInList(), digit->GetNprimary()) ;  
	message += tempo ; 
	Int_t iprimary;
	for (iprimary=0; iprimary<digit->GetNprimary(); iprimary++) {
	  sprintf(tempo, "%d ",digit->GetPrimary(iprimary+1) ) ; 
	  message += tempo ; 
	}
      }    
    }
  }
  delete []tempo ; 
  Info("PrintSDigits", message.Data() ) ;
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::Unload() const
{
  AliPHOSGetter * gime = AliPHOSGetter::Instance() ; 
  AliPHOSLoader * loader = gime->PhosLoader() ; 
  loader->UnloadHits() ; 
  loader->UnloadSDigits() ; 
}

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
#include "TTask.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TFolder.h"
#include "TBenchmark.h"
#include "TGeometry.h"

// --- Standard library ---

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliHeader.h"
#include "AliPHOSDigit.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSLoader.h"
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
AliPHOSSDigitizer::AliPHOSSDigitizer(const char *eventfoldername, const char * sDigitsTitle):
 TTask(sDigitsTitle, eventfoldername)
{
  // ctor
  InitParameters() ; 
  Init();
  fDefaultInit = kFALSE ; 
}

//____________________________________________________________________________ 
AliPHOSSDigitizer::~AliPHOSSDigitizer()
{
  // dtor
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::Init()
{
  // Initialization: open root-file, allocate arrays for hits and sdigits,
  // attach task SDigitizer to the list of PHOS tasks
  // 
  // Initialization can not be done in the default constructor
  //============================================================= YS
  //  The initialisation is now done by AliPHOSLoader
  
  if( strcmp(GetTitle(), "") == 0 )  SetTitle(AliConfig::fgkDefaultEventFolderName);
  
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::InitParameters()
{ 
  fA             = 0;
  fB             = 10000000.;
  fPrimThreshold = 0.01 ;
  fSDigitsInRun  = 0 ;
}

//____________________________________________________________________________
void AliPHOSSDigitizer::Exec(Option_t *option) 
{ 
  // Collects all hits in the same active volume into digit
  Info("Exec","                         ");
  Info("Exec","           **************");
  Info("Exec","           S DIGITIZATION");
  Info("Exec","           **************");
  Info("Exec","                         ");

  if( strcmp(GetName(), "") == 0 ) Init() ;
  
  if (strstr(option, "print") ) 
   {
     Print(""); 
     return ; 
   }

  if(strstr(option,"tim"))  gBenchmark->Start("PHOSSDigitizer");

  AliRunLoader* runget = AliRunLoader::GetRunLoader(GetTitle());
  if(runget == 0x0) 
   {
     Error("Exec","Can not find run getter in event folder \"%s\"",GetTitle());
     return;
   }
  
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(runget->GetLoader("PHOSLoader"));
  if ( gime == 0 ) 
   {
     Error("Exec","Could not obtain the Loader object !"); 
     return ;
   } 
  runget->GetEvent(0);
  Int_t retval;
  retval = gime->LoadSDigits("update");
  if (retval)
   {
     Error("Exec","Error occured while loading digits!"); 
     return ;
   }

  if(gime->BranchExists("SDigits") )
   {
     Error("Exec","SDigits branch already exists");
     return;
   } 
   

  retval = gime->LoadHits("read");//load hits to folder
  if (retval)
   {
     Error("Exec","Error occured while loading hits!"); 
     return ;
   }

  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSSDigitizer");
  
  //Check, if this branch already exits

  Int_t nevents = runget->GetNumberOfEvents(); 
  Int_t ievent ;
  for(ievent = 0; ievent < nevents; ievent++)
   {
    runget->GetEvent(ievent);
    if (gime->TreeH() == 0x0)
     {
       Error("Exec","Can not find TreeH for an event %d",ievent);
       return;
     }
    if (gime->TreeS() == 0x0) gime->MakeSDigitsContainer();//create a TreeS in folder
   
    const TClonesArray * hits = gime->Hits() ;
    TClonesArray * sdigits = gime->SDigits() ;
    sdigits->Clear();
    Int_t nSdigits = 0 ;
    
    //Now make SDigits from hits, for PHOS it is the same, so just copy    
    Int_t nPrim =  static_cast<Int_t>((gime->TreeH())->GetEntries()) ; 
    // Attention nPrim is the number of primaries tracked by Geant 
    // and this number could be different to the number of Primaries in TreeK;
    Int_t iprim ;
    cout<<"number of primaries: "<<nPrim<<endl;
    for (iprim = 0 ; iprim < nPrim ; iprim ++) { 
      //=========== Get the PHOS branch from Hits Tree for the Primary iprim
      gime->Track(iprim) ;
      Int_t i;

      for ( i = 0 ; i < hits->GetEntries() ; i++ ) 
       {
         AliPHOSHit * hit = dynamic_cast<AliPHOSHit *>(hits->At(i)) ;
         // Assign primary number only if contribution is significant
         if( hit->GetEnergy() > fPrimThreshold)
           new((*sdigits)[nSdigits]) AliPHOSDigit(hit->GetPrimary(),hit->GetId(),Digitize(hit->GetEnergy()),hit->GetTime());
         else
           new((*sdigits)[nSdigits]) AliPHOSDigit( -1, hit->GetId(), Digitize(hit->GetEnergy()), hit->GetTime());
         nSdigits++ ;
       }
     
    } // loop over iprim
    
    sdigits->Sort() ;
    
    cout<<"Number of SDigits in event "<<ievent<<" = "<<nSdigits;
    nSdigits = sdigits->GetEntriesFast() ;
    cout<<"("<<nSdigits<<")\n";
    fSDigitsInRun += nSdigits ;  

    sdigits->Expand(nSdigits) ;
    
    Int_t i ;
    for (i = 0 ; i < nSdigits ; i++) { 
      AliPHOSDigit * digit = dynamic_cast<AliPHOSDigit *>(sdigits->At(i)) ; 
      digit->SetIndexInList(i) ;     
    }

    //Now write SDigits

    Int_t bufferSize = 32000 ;
    TBranch * sdigitsBranch = gime->TreeS()->Branch("PHOS",&sdigits,bufferSize);

//    sdigitsBranch->SetTitle(sdname);

    //Next - SDigitizer
    sdigitsBranch->Fill() ;

    gime->WriteSDigitizer("OVERWRITE");
    gime->WriteSDigits("OVERWRITE");
    
    if(strstr(option,"deb"))
      PrintSDigits(option) ;
  }
  
  gime->UnloadHits();
  gime->UnloadSDigits();
  
  if(strstr(option,"tim"))
   {
    gBenchmark->Stop("PHOSSDigitizer");
    Info("Exec","   took %f seconds for SDigitizing  %f seconds per event",
	 gBenchmark->GetCpuTime("PHOSSDigitizer"), gBenchmark->GetCpuTime("PHOSSDigitizer")/nevents) ;
  }
}

//__________________________________________________________________
void AliPHOSSDigitizer::SetSDigitsBranch(const char * title )
{
  // Setting title to branch SDigits 

  AliRunLoader* runget = AliRunLoader::GetRunLoader(GetTitle());
  if(runget == 0x0) 
   {
     Error("Exec","Can not find run getter in event folder \"%s\"",GetTitle());
   }
  
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(runget->GetLoader("PHOSLoader"));
  if ( gime == 0 ) {
    cerr << "ERROR: AliPHOSSDigitizer::Init -> Could not obtain the Loader object !" << endl ; 
    return ;
  } 

  TString stitle(title) ;

  // check if branch with title already exists
  TBranch * sdigitsBranch    = 
    static_cast<TBranch*>(gime->TreeS()->GetListOfBranches()->FindObject("PHOS")) ; 
  TBranch * sdigitizerBranch =  
    static_cast<TBranch*>(gime->TreeS()->GetListOfBranches()->FindObject("AliPHOSSDigitizer"));
  
  if ( (sdigitsBranch == 0x0) || (sdigitizerBranch == 0x0) ) 
   {
    Error("SetSDigitsBranch","Can not get sigits branch or sdigitizer branch");
    return;
   }
  
  const char * sdigitsTitle    = sdigitsBranch ->GetTitle() ;  
  const char * sdigitizerTitle = sdigitizerBranch ->GetTitle() ;
  if ( stitle.CompareTo(sdigitsTitle)==0 || stitle.CompareTo(sdigitizerTitle)==0 ){
    Error("SetSDigitsBranch", "Cannot overwrite existing branch with title %s", title) ;
    return ;
  }
  
  Info("SetSDigitsBranch", "-> Changing SDigits file from %s to %s", GetName(), title) ;

//  SetName(title) ; 
  // Post to the WhiteBoard
  gime->SetBranchTitle(title);
  gime->ReloadSDigits();
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
  Info("Print", message.Data(),  GetName(),  GetName(), fA, fB, fPrimThreshold ) ;
  
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

  AliRunLoader* runget = AliRunLoader::GetRunLoader(GetTitle());
  if(runget == 0x0) 
   {
     Error("PrintSDigits","Can not find run getter in event folder \"%s\"",GetTitle());
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(runget->GetLoader("PHOSLoader"));
  if ( gime == 0 ) {
    cerr << "ERROR: AliPHOSSDigitizer::PrintSDigits -> Could not obtain the Loader object !" << endl ; 
    return ;
  } 
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
  delete tempo ; 
  Info("PrintSDigits", message.Data() );
}

//____________________________________________________________________________ 
void AliPHOSSDigitizer::UseHitsFrom(const char * filename)
{
  SetTitle(filename) ; 
  Init() ; 
}

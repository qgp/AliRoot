/*************************************************************************
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
// A Summable Digits is the sum of all hits originating 
// from one in one tower of the EMCAL 
// A threshold for assignment of the primary to SDigit is applied 
// SDigits are written to TreeS, branch "EMCAL"
// AliEMCALSDigitizer with all current parameters is written 
// to TreeS branch "AliEMCALSDigitizer".
// Both branches have the same title. If necessary one can produce 
// another set of SDigits with different parameters. Two versions
// can be distunguished using titles of the branches.
// User case:
// root [0] AliEMCALSDigitizer * s = new AliEMCALSDigitizer("galice.root")
// Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
// root [1] s->ExecuteTask()
//             // Makes SDigitis for all events stored in galice.root
// root [2] s->SetPedestalParameter(0.001)
//             // One can change parameters of digitization
// root [3] s->SetSDigitsBranch("Redestal 0.001")
//             // and write them into the new branch
// root [4] s->ExeciteTask("deb all tim")
//             // available parameters:
//             deb - print # of produced SDigitis
//             deb all  - print # and list of produced SDigits
//             tim - print benchmarking information
//
//*-- Author : Sahal Yacoob (LBL)
// based on  : AliPHOSSDigitzer 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include "TFile.h"
#include "TTask.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TFolder.h"
#include "TGeometry.h"
#include "TBenchmark.h"
// --- Standard library ---
#include <iomanip.h>

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliHeader.h"
#include "AliEMCALDigit.h"
#include "AliEMCALHit.h"
#include "AliEMCALSDigitizer.h"
#include "AliEMCALGeometry.h"
#include "AliEMCALv1.h"
#include "AliEMCALGetter.h"

ClassImp(AliEMCALSDigitizer)

           
//____________________________________________________________________________ 
  AliEMCALSDigitizer::AliEMCALSDigitizer():TTask("AliEMCALSDigitizer","") 
{
  // ctor
  InitParameters() ; 
  fDefaultInit = kTRUE ; 
}

//____________________________________________________________________________ 
AliEMCALSDigitizer::AliEMCALSDigitizer(const char* headerFile, const char *sDigitsTitle):TTask(sDigitsTitle, headerFile)
{
  // ctor
  InitParameters() ; 
  Init();
  fDefaultInit = kFALSE ; 
}

//____________________________________________________________________________ 
AliEMCALSDigitizer::~AliEMCALSDigitizer()
{
  // dtor
  // fDefaultInit = kTRUE if SDigitizer created by default ctor (to get just the parameters)
  
  if (!fDefaultInit) {
    AliEMCALGetter * gime = AliEMCALGetter::GetInstance() ; 
    
    // remove the task from the folder list
    gime->RemoveTask("S",GetName()) ;
    
    TString name(GetName()) ; 
    if (! name.IsNull() ) 
      if (name.Index(":") > 0)  
	name.Remove(name.Index(":")) ; 
    
    // remove the Hits from the folder list
    gime->RemoveObjects("H",name) ;
    
    // remove the SDigits from the folder list
    gime->RemoveObjects("S", name) ;
    
    // Close the root file
    gime->CloseFile() ; 
    
  }
  fSplitFile = 0 ; 
}

//____________________________________________________________________________ 
void AliEMCALSDigitizer::Init(){

  // Initialization: open root-file, allocate arrays for hits and sdigits,
  // attach task SDigitizer to the list of EMCAL tasks
  // 
  // Initialization can not be done in the default constructor
  //============================================================= YS
  //  The initialisation is now done by the getter

  if( strcmp(GetTitle(), "") == 0 )
    SetTitle("galice.root") ;
    
  AliEMCALGetter * gime = AliEMCALGetter::GetInstance(GetTitle(), GetName(), "update") ;     
  if ( gime == 0 ) {
    cerr << "ERROR: AliEMCALSDigitizer::Init -> Could not obtain the Getter object !" << endl ; 
    return ;
  } 
  
  gime->PostSDigits( GetName(), GetTitle() ) ; 

  TString sdname(GetName() );
  sdname.Append(":") ;
  sdname.Append(GetTitle() ) ;
  SetName(sdname.Data()) ;
  gime->PostSDigitizer(this) ;
 
 
}

//____________________________________________________________________________ 
void AliEMCALSDigitizer::InitParameters(){
  fA = 0;
  fB = 10000000.;
  fTowerPrimThreshold = 0.01 ;
  fPreShowerPrimThreshold = 0.0001 ; 
  fNevents = 0 ; 
  fPhotonElectronFactor = 5000. ; // photoelectrons per GeV 
  fSplitFile = 0 ; 

}

//____________________________________________________________________________
void AliEMCALSDigitizer::Exec(Option_t *option) { 

// Collects all hits in the same active volume into digit
 
  if( strcmp(GetName(), "") == 0 )
    Init() ;
  
  if (strstr(option, "print") ) {
    Print("") ; 
    return ; 
  }
  
  if(strstr(option,"tim"))
    gBenchmark->Start("EMCALSDigitizer");


  //Check, if this branch already exits
  gAlice->GetEvent(0) ;
 
  TString sdname(GetName()) ;
  sdname.Remove(sdname.Index(GetTitle())-1) ;

  if(gAlice->TreeS() ) {
    TObjArray * lob = static_cast<TObjArray*>(gAlice->TreeS()->GetListOfBranches()) ;
    TIter next(lob) ; 
    TBranch * branch = 0 ;  
    Bool_t emcalfound = kFALSE, sdigitizerfound = kFALSE ; 
    
     while ( (branch = (static_cast<TBranch*>(next()))) && (!emcalfound || !sdigitizerfound) ) {
      TString thisName( GetName() ) ; 
      TString branchName( branch->GetTitle() ) ; 
      branchName.Append(":") ; 
      if ( (strcmp(branch->GetName(), "EMCAL")==0) && thisName.BeginsWith(branchName) )  
	emcalfound = kTRUE ;
      
      else if ( (strcmp(branch->GetName(), "AliEMCALSDigitizer")==0) && thisName.BeginsWith(branchName) )  
	sdigitizerfound = kTRUE ; 
    }
    
    if ( emcalfound || sdigitizerfound ) {
      cerr << "WARNING: AliEMCALSDigitizer::Exec -> SDigits and/or SDigitizer branch with name " << GetName() 
	   << " already exits" << endl ;
      return ; 
    }   
  }  

  AliEMCALGetter * gime = AliEMCALGetter::GetInstance() ;  
  Int_t nevents = (Int_t) gAlice->TreeE()->GetEntries() ; 
  
  Int_t ievent ;
  for(ievent = 0; ievent < nevents; ievent++){
    gime->Event(ievent,"H") ;
    const TClonesArray * fHits = gime->Hits() ;
    TClonesArray * sdigits = gime->SDigits(sdname.Data()) ;
    sdigits->Clear();
    Int_t nSdigits = 0 ;

  //Collects all hits in the same active volume into digit
  
  
    
    
    //Now made SDigits from hits, for EMCAL it is the same, so just copy    
    //  Int_t itrack ;
    // for (itrack=0; itrack < gAlice->GetNtrack(); itrack++){
    //gime->Track(itrack);  
      //=========== Get the EMCAL branch from Hits Tree for the Primary track itrack
     
      Int_t i;
      for ( i = 0 ; i < fHits->GetEntries() ; i++ ) { // loop over all hits (hit = deposited energy/layer/entering particle)
	AliEMCALHit * hit = dynamic_cast<AliEMCALHit*>(fHits->At(i)) ;
        AliEMCALDigit * curSDigit = 0 ;
        AliEMCALDigit * sdigit = 0 ;
        Bool_t newsdigit = kTRUE; 

	

	// Assign primary number only if deposited energy is significant
	  
	if( (!hit->IsInPreShower() && hit->GetEnergy() > fTowerPrimThreshold) || 
	    (hit->IsInPreShower() && hit->GetEnergy() > fPreShowerPrimThreshold)) {
	  curSDigit =  new AliEMCALDigit( hit->GetPrimary(),
					  hit->GetIparent(),Layer2TowerID(hit->GetId(),hit->IsInPreShower()), 
					  Digitize(hit->GetEnergy()), 
					  hit->GetTime()) ;
	} else {
	  curSDigit =  new AliEMCALDigit( -1               , 
					  -1               ,
					  Layer2TowerID(hit->GetId(),hit->IsInPreShower()), 
					  Digitize(hit->GetEnergy()), 
					  hit->GetTime() ) ;	
	}
	Int_t check = 0 ;
	for(check= 0; check < nSdigits ; check++) {
          sdigit = (AliEMCALDigit *)sdigits->At(check);
          if( sdigit->GetId() == curSDigit->GetId()) { // Are we in the same tower or the same preshower ?              
	      *sdigit = *sdigit + *curSDigit;
   
	      newsdigit = kFALSE;
	  }
	}
	if (newsdigit) { 
	  new((*sdigits)[nSdigits])  AliEMCALDigit(*curSDigit);
	  nSdigits++ ;  
	}
      }  // loop over all hits (hit = deposited energy/layer/entering particle)
      //    } // loop over tracks
      
      sdigits->Sort() ;
      
      nSdigits = sdigits->GetEntriesFast() ;
      if (nSdigits > 0) { 
	sdigits->Expand(nSdigits) ;
	
	//  Int_t i ;
	const AliEMCALGeometry * geom = gime->EMCALGeometry() ; 
	
	Int_t lastPreShowerIndex = nSdigits - 1 ;
	if (!(dynamic_cast<AliEMCALDigit *>(sdigits->At(lastPreShowerIndex))->IsInPreShower()))
	  lastPreShowerIndex = -2; 
	Int_t firstPreShowerIndex = 100000 ; 
	Int_t index ; 
	AliEMCALDigit * sdigit = 0 ;
	for ( index = 0; index < nSdigits ; index++) {
	  sdigit = dynamic_cast<AliEMCALDigit *>(sdigits->At(index) ) ;
	  if (sdigit->IsInPreShower() ){ 
	    firstPreShowerIndex = index ;
	    break ;
	  }
	}
	
	AliEMCALDigit * preshower ;
	AliEMCALDigit * tower ;
	Int_t lastIndex = lastPreShowerIndex +1 ; 
	
	
	for (index = firstPreShowerIndex ; index <= lastPreShowerIndex; index++) {
	  preshower = dynamic_cast<AliEMCALDigit *>(sdigits->At(index) ); 
	  Bool_t towerFound = kFALSE ;
	  Int_t jndex ; 
	  for (jndex = 0; jndex < firstPreShowerIndex; jndex++) {
	    tower  = dynamic_cast<AliEMCALDigit *>(sdigits->At(jndex) ); 
	    if ( (preshower->GetId() - (geom->GetNZ() * geom->GetNPhi()) ) == tower->GetId() ) {	  
	      Float_t towerEnergy  = static_cast<Float_t>(tower->GetAmp()) ; 
	      Float_t preshoEnergy = static_cast<Float_t>(preshower->GetAmp()) ; 
	      towerEnergy +=preshoEnergy ; 
	      *tower = *tower + *preshower    ; // and add preshower multiplied by layer ratio to tower
	      tower->SetAmp(static_cast<Int_t>(TMath::Ceil(towerEnergy))) ; 
	      towerFound = kTRUE ;
	    }
	  }
	  if ( !towerFound ) { 
	    
	    new((*sdigits)[lastIndex])  AliEMCALDigit(*preshower);
	    AliEMCALDigit * temp = dynamic_cast<AliEMCALDigit *>(sdigits->At(lastIndex)) ;
	    temp->SetId(temp->GetId() - (geom->GetNZ() * geom->GetNPhi()) ) ;
	    lastIndex++ ; 
	  }
	}
	
	sdigits->Sort() ;
	Int_t NPrimarymax = -1 ; 
	for (i = 0 ; i < sdigits->GetEntriesFast() ; i++) { 
	  sdigit = dynamic_cast<AliEMCALDigit *>(sdigits->At(i)) ;
	  sdigit->SetIndexInList(i) ;
	}
	
	for (i = 0 ; i < sdigits->GetEntriesFast() ; i++) { 
	  if (((dynamic_cast<AliEMCALDigit *>(sdigits->At(i)))->GetNprimary()) > NPrimarymax)
	    NPrimarymax = ((dynamic_cast<AliEMCALDigit *>(sdigits->At(i)))->GetNprimary()) ;
	}
      }
	if(gAlice->TreeS() == 0)
	  gAlice->MakeTree("S",fSplitFile);

      //First list of sdigits
      Int_t bufferSize = 32000 ;    
      TBranch * sdigitsBranch = gAlice->TreeS()->Branch("EMCAL",&sdigits,bufferSize);
      sdigitsBranch->SetTitle(sdname);
      
      //second - SDigitizer
      Int_t splitlevel = 0 ;
      AliEMCALSDigitizer * sd = this ;
      TBranch * sdigitizerBranch = gAlice->TreeS()->Branch("AliEMCALSDigitizer","AliEMCALSDigitizer",
							   &sd,bufferSize,splitlevel); 
      sdigitizerBranch->SetTitle(sdname);
      
      sdigitsBranch->Fill() ; 
      sdigitizerBranch->Fill() ; 
      gAlice->TreeS()->AutoSave() ;
           
      if(strstr(option,"deb"))
	PrintSDigits(option) ;
      
  }
  
  if(strstr(option,"tim")){
    gBenchmark->Stop("EMCALSDigitizer");
    cout << "AliEMCALSDigitizer:" << endl ;
    cout << "   took " << gBenchmark->GetCpuTime("EMCALSDigitizer") << " seconds for SDigitizing " 
	 <<  gBenchmark->GetCpuTime("EMCALSDigitizer")/fNevents << " seconds per event " << endl ;
    cout << endl ;
  }
  
  
}
//__________________________________________________________________
void AliEMCALSDigitizer::SetSDigitsBranch(const char * title ){
 
  // Setting title to branch SDigits 

  TString stitle(title) ;

  // check if branch with title already exists
  TBranch * sdigitsBranch    = 
    static_cast<TBranch*>(gAlice->TreeS()->GetListOfBranches()->FindObject("EMCAL")) ; 
  TBranch * sdigitizerBranch =  
    static_cast<TBranch*>(gAlice->TreeS()->GetListOfBranches()->FindObject("AliEMCALSDigitizer")) ;
  const char * sdigitsTitle    = sdigitsBranch ->GetTitle() ;  
  const char * sdigitizerTitle = sdigitizerBranch ->GetTitle() ;
  if ( stitle.CompareTo(sdigitsTitle)==0 || stitle.CompareTo(sdigitizerTitle)==0 ){
    cerr << "ERROR: AliEMCALSdigitizer::SetSDigitsBranch -> Cannot overwrite existing branch with title " << title << endl ;
    return ;
  }
  
  cout << "AliEMCALSdigitizer::SetSDigitsBranch -> Changing SDigits file from " << GetName() << " to " << title << endl ;

  SetName(title) ; 
    
  // Post to the WhiteBoard
  AliEMCALGetter * gime = AliEMCALGetter::GetInstance() ; 
  gime->PostSDigits( title, GetTitle()) ; 


}

//__________________________________________________________________
void AliEMCALSDigitizer::SetSplitFile(const TString splitFileName) 
{
  // Diverts the SDigits in a file separate from the hits file
  
  TDirectory * cwd = gDirectory ;

  if ( !(gAlice->GetTreeSFileName() == splitFileName) ) {
    if (gAlice->GetTreeSFile() )  
      gAlice->GetTreeSFile()->Close() ; 
  }

  fSplitFile = gAlice->InitTreeFile("S",splitFileName.Data());
  fSplitFile->cd() ; 
  gAlice->Write(0, TObject::kOverwrite);
 
  TTree *treeE  = gAlice->TreeE();
  if (!treeE) {
    cerr << "ERROR: AliEMCALSDigitizer::SetSPlitFile -> No TreeE found "<<endl;
   abort() ;
  }      
  
  // copy TreeE
  
  AliHeader *header = new AliHeader();
  treeE->SetBranchAddress("Header", &header);
  treeE->SetBranchStatus("*",1);
  TTree *treeENew =  treeE->CloneTree();
  treeENew->Write(0, TObject::kOverwrite);
  
  // copy AliceGeom
  TGeometry *AliceGeom = static_cast<TGeometry*>(cwd->Get("AliceGeom"));
  if (!AliceGeom) {
    cerr << "ERROR: AliEMCALSDigitizer::SetSPlitFile -> AliceGeom was not found in the input file "<<endl;
    abort() ;
  }
  AliceGeom->Write(0, TObject::kOverwrite) ;
  
  gAlice->MakeTree("S",fSplitFile);
  cwd->cd() ; 
  cout << "INFO: AliEMCALSDigitizer::SetSPlitFile -> SDigits will be stored in " << splitFileName.Data() << endl ; 
}

//__________________________________________________________________
void AliEMCALSDigitizer::Print(Option_t* option)const{
  cout << "------------------- "<< GetName() << " -------------" << endl ;
  cout << "   Writing SDigitis to branch with title  " << GetName() << endl ;
  cout << "   with digitization parameters  A               = " << fA << endl ;
  cout << "                                 B               = " << fB << endl ;
  cout << "   Threshold for Primary assignment in Tower     = " << fTowerPrimThreshold << endl ; 
  cout << "   Threshold for Primary assignment in PreShower = " << fPreShowerPrimThreshold << endl ; 
  cout << "---------------------------------------------------"<<endl ;
  
}
//__________________________________________________________________
Bool_t AliEMCALSDigitizer::operator==( AliEMCALSDigitizer const &sd )const{
  if( (fA==sd.fA)&&(fB==sd.fB)&&
      (fTowerPrimThreshold==sd.fTowerPrimThreshold) &&
      (fPreShowerPrimThreshold==sd.fPreShowerPrimThreshold))
    return kTRUE ;
  else
    return kFALSE ;
}
//__________________________________________________________________
void AliEMCALSDigitizer::PrintSDigits(Option_t * option){
  //Prints list of digits produced at the current pass of AliEMCALDigitizer
  
  AliEMCALGetter * gime = AliEMCALGetter::GetInstance() ; 
  TString sdname(GetName()) ;
  sdname.Remove(sdname.Index(GetTitle())-1) ;
  const TClonesArray * sdigits = gime->SDigits(sdname.Data()) ; 
  
  cout << "AliEMCALSDigitiser: event " << gAlice->GetEvNumber() << endl ;
  cout << "      Number of entries in SDigits list " << sdigits->GetEntriesFast() << endl ;
  cout << endl ;
  if(strstr(option,"all")||strstr(option,"EMC")){

    //loop over digits
    AliEMCALDigit * digit;
    cout << "SDigit Id " << " Amplitude " <<  "     Time " << "     Index "  <<  " Nprim " << " Primaries list " <<  endl;    
    Int_t index ;
    for (index = 0 ; index < sdigits->GetEntries() ; index++) {
      digit = (AliEMCALDigit * )  sdigits->At(index) ;
      cout << setw(6)  <<  digit->GetId() << "   "  << 	setw(10)  <<  digit->GetAmp() <<   "    "  << digit->GetTime()
	   << setw(6)  <<  digit->GetIndexInList() << "    "   
	   << setw(5)  <<  digit->GetNprimary() <<"    ";
      
      Int_t iprimary;
      for (iprimary=0; iprimary<digit->GetNprimary(); iprimary++)
	cout << " "  <<  digit->GetPrimary(iprimary+1) << "  ";
      cout << endl;  	 
    }
    cout <<endl;
  }
}
//________________________________________________________________________
Int_t AliEMCALSDigitizer::Layer2TowerID(Int_t ihit, Bool_t preshower){
  // Method to Transform from Hit Id to Digit Id
  // This function should be one to one
  AliEMCALGetter * gime = AliEMCALGetter::GetInstance() ;
  const AliEMCALGeometry * geom = gime->EMCALGeometry();
  Int_t ieta  = ((ihit-1)/geom->GetNPhi())%geom->GetNZ(); // eta Tower Index
  Int_t iphi = (ihit-1)%(geom->GetNPhi())+1; //phi Tower Index
  Int_t it = -10;
  Int_t ipre = 0;

  if (preshower)ipre = 1;
  if (iphi > 0 && ieta >= 0){
    it = iphi+ieta*geom->GetNPhi() + ipre*geom->GetNPhi()*geom->GetNZ();
    return it;
  }else{
    cerr << " AliEMCALSDigitizer::Layer2TowerID() -- there is an error "<< endl << "Eta number = "
	 << ieta << "Phi number = " << iphi << endl ;
    return it;
  } // end if iphi>0 && ieta>0
}
//_______________________________________________________________________________________
// void AliEMCALSDigitizer::TestTowerID(void)
// {
//   Int_t j;

//   Bool_t preshower = kFALSE;
//   for (j = 0 ; j < 10 ; j++){  // loop over hit id
//     Int_t i;
//    for (i = 0 ; i <= 2 ; i++){  // loop over 
//      Int_t k = i*96*144+j*144+1;
//       cout << " Hit Index = " << k << "   " << j*10 << "   TOWERID = " <<  Layer2TowerID(k, preshower) << endl ;
//     }
//   }
// }

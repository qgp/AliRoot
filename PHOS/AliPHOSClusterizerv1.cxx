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

/* $Log:
   1 October 2000. Yuri Kharlov:
     AreNeighbours()
     PPSD upper layer is considered if number of layers>1

   18 October 2000. Yuri Kharlov:
     AliPHOSClusterizerv1()
     CPV clusterizing parameters added

     MakeClusters()
     After first PPSD digit remove EMC digits only once
*/
//*-- Author: Yves Schutz (SUBATECH)  & Dmitri Peressounko (SUBATECH & Kurchatov Institute)
//////////////////////////////////////////////////////////////////////////////
//  Clusterization class. Performs clusterization (collects neighbouring active cells) and 
//  unfolds the clusters having several local maxima.  
//  Results are stored in TreeR#, branches PHOSEmcRP (EMC recPoints),
//  PHOSCpvRP (CPV RecPoints) and AliPHOSClusterizer (Clusterizer with all 
//  parameters including input digits branch title, thresholds etc.)
//  This TTask is normally called from Reconstructioner, but can as well be used in 
//  standalone mode.
// Use Case:
//  root [0] AliPHOSClusterizerv1 * cl = new AliPHOSClusterizerv1("galice.root", "recpointsname", "digitsname")  
//  Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//               // reads gAlice from header file "galice.root", uses digits stored in the branch names "digitsname" (default = "Default")
//               // and saves recpoints in branch named "recpointsname" (default = "digitsname")                       
//  root [1] cl->ExecuteTask()  
//               //finds RecPoints in all events stored in galice.root
//  root [2] cl->SetDigitsBranch("digits2") 
//               //sets another title for Digitis (input) branch
//  root [3] cl->SetRecPointsBranch("recp2")  
//               //sets another title four output branches
//  root [4] cl->SetEmcLocalMaxCut(0.03)  
//               //set clusterization parameters
//  root [5] cl->ExecuteTask("deb all time")  
//               //once more finds RecPoints options are 
//               // deb - print number of found rec points
//               // deb all - print number of found RecPoints and some their characteristics 
//               // time - print benchmarking results

// --- ROOT system ---

#include "TROOT.h" 
#include "TFile.h" 
#include "TFolder.h" 
#include "TMath.h" 
#include "TMinuit.h"
#include "TTree.h" 
#include "TSystem.h" 
#include "TBenchmark.h"

// --- Standard library ---

#include <iostream.h>
#include <iomanip.h>

// --- AliRoot header files ---

#include "AliPHOSClusterizerv1.h"
#include "AliPHOSCpvRecPoint.h"
#include "AliPHOSDigit.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSEmcRecPoint.h"
#include "AliPHOS.h"
#include "AliPHOSGetter.h"
#include "AliRun.h"

ClassImp(AliPHOSClusterizerv1)
  
//____________________________________________________________________________
  AliPHOSClusterizerv1::AliPHOSClusterizerv1() : AliPHOSClusterizer()
{
  // default ctor (to be used mainly by Streamer)
  
  InitParameters() ; 
  fDefaultInit = kTRUE ; 
}

//____________________________________________________________________________
AliPHOSClusterizerv1::AliPHOSClusterizerv1(const char* headerFile,const char* name, const Bool_t toSplit)
:AliPHOSClusterizer(headerFile, name, toSplit)
{
  // ctor with the indication of the file where header Tree and digits Tree are stored
  
  InitParameters() ;
  Init() ;
  fDefaultInit = kFALSE ; 

}

//____________________________________________________________________________
  AliPHOSClusterizerv1::~AliPHOSClusterizerv1()
{
  // dtor
  fSplitFile = 0 ; 

}

//____________________________________________________________________________
const TString AliPHOSClusterizerv1::BranchName() const 
{  
  TString branchName(GetName() ) ;
  branchName.Remove(branchName.Index(Version())-1) ;
  return branchName ;
}
 
//____________________________________________________________________________
Float_t  AliPHOSClusterizerv1::Calibrate(Int_t amp, Int_t absId) const
{ //To be replased later by the method, reading individual parameters from the database

  if(absId <= fEmcCrystals) //calibrate as EMC 
    return fADCpedestalEmc + amp*fADCchanelEmc ;    
   
  else //Digitize as CPV
    return fADCpedestalCpv+ amp*fADCchanelCpv ;       
}

//____________________________________________________________________________
void AliPHOSClusterizerv1::Exec(Option_t * option)
{
  // Steering method

  if( strcmp(GetName(), "")== 0 ) 
    Init() ;

  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSClusterizer"); 
  
  if(strstr(option,"print"))
    Print("") ; 

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ;
  if(gime->BranchExists("RecPoints"))
    return ;
  Int_t nevents = gime->MaxEvent() ;
  Int_t ievent ;
  
  for(ievent = 0; ievent < nevents; ievent++){
    
    gime->Event(ievent,"D") ;
    
    if(ievent == 0)
      GetCalibrationParameters() ;
    
    fNumberOfEmcClusters  = fNumberOfCpvClusters  = 0 ;
            
    MakeClusters() ;
    
    if(fToUnfold)             
      MakeUnfolding() ;

    WriteRecPoints(ievent) ;

    if(strstr(option,"deb"))  
      PrintRecPoints(option) ;

    //increment the total number of digits per run 
    fRecPointsInRun += gime->EmcRecPoints()->GetEntriesFast() ;  
    fRecPointsInRun += gime->CpvRecPoints()->GetEntriesFast() ;  
 }
  
  if(strstr(option,"tim")){
    gBenchmark->Stop("PHOSClusterizer");
    cout << "AliPHOSClusterizer:" << endl ;
    cout << "  took " << gBenchmark->GetCpuTime("PHOSClusterizer") << " seconds for Clusterizing " 
	 <<  gBenchmark->GetCpuTime("PHOSClusterizer")/nevents << " seconds per event " << endl ;
    cout << endl ;
  }
  
}

//____________________________________________________________________________
Bool_t AliPHOSClusterizerv1::FindFit(AliPHOSEmcRecPoint * emcRP, AliPHOSDigit ** maxAt, Float_t * maxAtEnergy,
				    Int_t nPar, Float_t * fitparameters) const
{ 
  // Calls TMinuit to fit the energy distribution of a cluster with several maxima 
  // The initial values for fitting procedure are set equal to the positions of local maxima.
  // Cluster will be fitted as a superposition of nPar/3 electromagnetic showers

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ; 
  TClonesArray * digits = gime->Digits() ; 
  

  gMinuit->mncler();                     // Reset Minuit's list of paramters
  gMinuit->SetPrintLevel(-1) ;           // No Printout
  gMinuit->SetFCN(AliPHOSClusterizerv1::UnfoldingChiSquare) ;  
                                         // To set the address of the minimization function 

  TList * toMinuit = new TList();
  toMinuit->AddAt(emcRP,0) ;
  toMinuit->AddAt(digits,1) ;
  
  gMinuit->SetObjectFit(toMinuit) ;         // To tranfer pointer to UnfoldingChiSquare

  // filling initial values for fit parameters
  AliPHOSDigit * digit ;

  Int_t ierflg  = 0; 
  Int_t index   = 0 ;
  Int_t nDigits = (Int_t) nPar / 3 ;

  Int_t iDigit ;

  const AliPHOSGeometry * geom = gime->PHOSGeometry() ; 

  for(iDigit = 0; iDigit < nDigits; iDigit++){
    digit = maxAt[iDigit]; 

    Int_t relid[4] ;
    Float_t x = 0.;
    Float_t z = 0.;
    geom->AbsToRelNumbering(digit->GetId(), relid) ;
    geom->RelPosInModule(relid, x, z) ;

    Float_t energy = maxAtEnergy[iDigit] ;

    gMinuit->mnparm(index, "x",  x, 0.1, 0, 0, ierflg) ;
    index++ ;   
    if(ierflg != 0){ 
      cout << "PHOS Unfolding>  Unable to set initial value for fit procedure : x = " << x << endl ;
      return kFALSE;
    }
    gMinuit->mnparm(index, "z",  z, 0.1, 0, 0, ierflg) ;
    index++ ;   
    if(ierflg != 0){
      cout << "PHOS Unfolding>  Unable to set initial value for fit procedure : z = " << z << endl ;
      return kFALSE;
    }
    gMinuit->mnparm(index, "Energy",  energy , 0.05*energy, 0., 4.*energy, ierflg) ;
    index++ ;   
    if(ierflg != 0){
      cout << "PHOS Unfolding>  Unable to set initial value for fit procedure : energy = " << energy << endl ;      
      return kFALSE;
    }
  }

  Double_t p0 = 0.1 ; // "Tolerance" Evaluation stops when EDM = 0.0001*p0 ; The number of function call slightly
                      //  depends on it. 
  Double_t p1 = 1.0 ;
  Double_t p2 = 0.0 ;

  gMinuit->mnexcm("SET STR", &p2, 0, ierflg) ;   // force TMinuit to reduce function calls  
  gMinuit->mnexcm("SET GRA", &p1, 1, ierflg) ;   // force TMinuit to use my gradient  
  gMinuit->SetMaxIterations(5);
  gMinuit->mnexcm("SET NOW", &p2 , 0, ierflg) ;  // No Warnings

  gMinuit->mnexcm("MIGRAD", &p0, 0, ierflg) ;    // minimize 

  if(ierflg == 4){  // Minimum not found   
    cout << "PHOS Unfolding>  Fit not converged, cluster abandoned "<< endl ;      
    return kFALSE ;
  }            
  for(index = 0; index < nPar; index++){
    Double_t err ;
    Double_t val ;
    gMinuit->GetParameter(index, val, err) ;    // Returns value and error of parameter index
    fitparameters[index] = val ;
   }

  delete toMinuit ;
  return kTRUE;

}

//____________________________________________________________________________
void AliPHOSClusterizerv1::GetCalibrationParameters() 
{
  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ;
  const AliPHOSDigitizer * dig = gime->Digitizer(BranchName()) ;

  fADCchanelEmc   = dig->GetEMCchannel() ;
  fADCpedestalEmc = dig->GetEMCpedestal();
  
  fADCchanelCpv   = dig->GetCPVchannel() ;
  fADCpedestalCpv = dig->GetCPVpedestal() ; 
  
}

//____________________________________________________________________________
void AliPHOSClusterizerv1::Init()
{
  // Make all memory allocations which can not be done in default constructor.
  // Attach the Clusterizer task to the list of PHOS tasks
  
  if ( strcmp(GetTitle(), "") == 0 )
    SetTitle("galice.root") ;

  TString branchname = GetName() ;
  branchname.Remove(branchname.Index(Version())-1) ;

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance(GetTitle(),branchname.Data(), fToSplit ) ; 
  if ( gime == 0 ) {
    cerr << "ERROR: AliPHOSClusterizerv1::Init -> Could not obtain the Getter object !" << endl ; 
    return ;
  } 

  fSplitFile = 0 ;
  if(fToSplit){
    // construct the name of the file as /path/EMCAL.SDigits.root
    //First - extract full path if necessary
    TString fileName(GetTitle()) ;
    Ssiz_t islash = fileName.Last('/') ;
    if(islash<fileName.Length())
      fileName.Remove(islash+1,fileName.Length()) ;
    else
      fileName="" ;
    // Next - append the file name 
    fileName+="PHOS.RecData." ;
    if((strcmp(branchname.Data(),"Default")!=0)&&(strcmp(branchname.Data(),"")!=0)){
      fileName+=branchname ;
      fileName+="." ;
    }
    fileName+="root" ;
    // Finally - check if the file already opened or open the file
    fSplitFile = static_cast<TFile*>(gROOT->GetFile(fileName.Data()));   
    if(!fSplitFile)
      fSplitFile =  TFile::Open(fileName.Data(),"update") ;
  }


    
  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
  fEmcCrystals = geom->GetNModules() *  geom->GetNCristalsInModule() ;

  if(!gMinuit) 
    gMinuit = new TMinuit(100) ;

  gime->PostClusterizer(this) ;
  gime->PostRecPoints(branchname) ;
  
}

//____________________________________________________________________________
void AliPHOSClusterizerv1::InitParameters()
{

  fNumberOfCpvClusters     = 0 ; 
  fNumberOfEmcClusters     = 0 ; 
  
  fCpvClusteringThreshold  = 0.0;
  fEmcClusteringThreshold  = 0.2;   
  
  fEmcLocMaxCut            = 0.03 ;
  fCpvLocMaxCut            = 0.03 ;
  
  fW0                      = 4.5 ;
  fW0CPV                   = 4.0 ;

  fEmcTimeGate             = 1.e-8 ; 
  
  fToUnfold                = kTRUE ;
    
  TString clusterizerName( GetName()) ;
  if (clusterizerName.IsNull() ) 
    clusterizerName = "Default" ; 
  clusterizerName.Append(":") ; 
  clusterizerName.Append(Version()) ; 
  SetName(clusterizerName) ;
  fRecPointsInRun          = 0 ;

}

//____________________________________________________________________________
Int_t AliPHOSClusterizerv1::AreNeighbours(AliPHOSDigit * d1, AliPHOSDigit * d2)const
{
  // Gives the neighbourness of two digits = 0 are not neighbour but continue searching 
  //                                       = 1 are neighbour
  //                                       = 2 are not neighbour but do not continue searching
  // neighbours are defined as digits having at least a common vertex 
  // The order of d1 and d2 is important: first (d1) should be a digit already in a cluster 
  //                                      which is compared to a digit (d2)  not yet in a cluster  

  const AliPHOSGeometry * geom = AliPHOSGetter::GetInstance()->PHOSGeometry() ;

  Int_t rv = 0 ; 

  Int_t relid1[4] ; 
  geom->AbsToRelNumbering(d1->GetId(), relid1) ; 

  Int_t relid2[4] ; 
  geom->AbsToRelNumbering(d2->GetId(), relid2) ; 
 
  if ( (relid1[0] == relid2[0]) && (relid1[1]==relid2[1]) ) { // inside the same PHOS module 
    Int_t rowdiff = TMath::Abs( relid1[2] - relid2[2] ) ;  
    Int_t coldiff = TMath::Abs( relid1[3] - relid2[3] ) ;  
    
    if (( coldiff <= 1 )  && ( rowdiff <= 1 )){
      if((relid1[1] != 0) || (TMath::Abs(d1->GetTime() - d2->GetTime() ) < fEmcTimeGate))
      rv = 1 ; 
    }
    else {
      if((relid2[2] > relid1[2]) && (relid2[3] > relid1[3]+1)) 
	rv = 2; //  Difference in row numbers is too large to look further 
    }

  } 
  else {
    
    if( (relid1[0] < relid2[0]) || (relid1[1] != relid2[1]) )  
      rv=2 ;

  }

  return rv ; 
}


//____________________________________________________________________________
Bool_t AliPHOSClusterizerv1::IsInEmc(AliPHOSDigit * digit) const
{
  // Tells if (true) or not (false) the digit is in a PHOS-EMC module
 
  Bool_t rv = kFALSE ; 
  const AliPHOSGeometry * geom = AliPHOSGetter::GetInstance()->PHOSGeometry() ;

  Int_t nEMC = geom->GetNModules()*geom->GetNPhi()*geom->GetNZ();  

  if(digit->GetId() <= nEMC )   rv = kTRUE; 

  return rv ; 
}

//____________________________________________________________________________
Bool_t AliPHOSClusterizerv1::IsInCpv(AliPHOSDigit * digit) const
{
  // Tells if (true) or not (false) the digit is in a PHOS-CPV module
 
  Bool_t rv = kFALSE ; 
  const AliPHOSGeometry * geom = AliPHOSGetter::GetInstance()->PHOSGeometry() ;

  Int_t nEMC = geom->GetNModules()*geom->GetNPhi()*geom->GetNZ();

  if(digit->GetId() > nEMC )   rv = kTRUE;

  return rv ; 
}

//____________________________________________________________________________
void AliPHOSClusterizerv1::WriteRecPoints(Int_t event)
{

  // Creates new branches with given title
  // fills and writes into TreeR.
  
  
  AliPHOSGetter *gime = AliPHOSGetter::GetInstance() ; 
  TObjArray * emcRecPoints = gime->EmcRecPoints() ; 
  TObjArray * cpvRecPoints = gime->CpvRecPoints() ; 
  TClonesArray * digits = gime->Digits() ; 
  TTree * treeR ;
  
  if(fToSplit){
    if(!fSplitFile)
      return ;
    fSplitFile->cd() ;
    TString name("TreeR") ;
    name += event ; 
    treeR = dynamic_cast<TTree*>(fSplitFile->Get(name)); 
  }
  else{
    treeR = gAlice->TreeR();
  }
  
  if(!treeR){
    gAlice->MakeTree("R", fSplitFile);
    treeR = gAlice->TreeR() ;
  }

  Int_t index ;
  //Evaluate position, dispersion and other RecPoint properties...
  for(index = 0; index < emcRecPoints->GetEntries(); index++)
    ((AliPHOSEmcRecPoint *)emcRecPoints->At(index))->EvalAll(fW0,digits) ;
  
  emcRecPoints->Sort() ;
  for(index = 0; index < emcRecPoints->GetEntries(); index++)
    ((AliPHOSEmcRecPoint *)emcRecPoints->At(index))->SetIndexInList(index) ;
  
  emcRecPoints->Expand(emcRecPoints->GetEntriesFast()) ; 
  
  //Now the same for CPV
  for(index = 0; index < cpvRecPoints->GetEntries(); index++)
    ((AliPHOSRecPoint *)cpvRecPoints->At(index))->EvalAll(fW0CPV,digits)  ;
  
  cpvRecPoints->Sort() ;
  
  for(index = 0; index < cpvRecPoints->GetEntries(); index++)
    ((AliPHOSRecPoint *)cpvRecPoints->At(index))->SetIndexInList(index) ;
  
  cpvRecPoints->Expand(cpvRecPoints->GetEntriesFast()) ;
  
  Int_t bufferSize = 32000 ;    
  Int_t splitlevel = 0 ;
 
  //First EMC
  TBranch * emcBranch = treeR->Branch("PHOSEmcRP","TObjArray",&emcRecPoints,bufferSize,splitlevel);
  emcBranch->SetTitle(BranchName());
    
  //Now CPV branch
  TBranch * cpvBranch = treeR->Branch("PHOSCpvRP","TObjArray",&cpvRecPoints,bufferSize,splitlevel);
  cpvBranch->SetTitle(BranchName());
    
  //And Finally  clusterizer branch
  AliPHOSClusterizerv1 * cl = this ;
  TBranch * clusterizerBranch = treeR->Branch("AliPHOSClusterizer","AliPHOSClusterizerv1",
					      &cl,bufferSize,splitlevel);
  clusterizerBranch->SetTitle(BranchName());

  emcBranch        ->Fill() ;
  cpvBranch        ->Fill() ;
  clusterizerBranch->Fill() ;

  treeR->AutoSave() ; 
  if(gAlice->TreeR()!=treeR)
    treeR->Delete();
}

//____________________________________________________________________________
void AliPHOSClusterizerv1::MakeClusters()
{
  // Steering method to construct the clusters stored in a list of Reconstructed Points
  // A cluster is defined as a list of neighbour digits

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ; 

  TObjArray * emcRecPoints = gime->EmcRecPoints(BranchName()) ; 
  TObjArray * cpvRecPoints = gime->CpvRecPoints(BranchName()) ; 
  emcRecPoints->Delete() ;
  cpvRecPoints->Delete() ;
  
  TClonesArray * digits = gime->Digits() ; 
   if ( !digits ) {
    cerr << "ERROR:  AliPHOSClusterizerv1::MakeClusters -> Digits with name " 
	 << GetName() << " not found ! " << endl ; 
    abort() ; 
  } 
  TClonesArray * digitsC =  (TClonesArray*)digits->Clone() ;
  
 
  // Clusterization starts  

  TIter nextdigit(digitsC) ; 
  AliPHOSDigit * digit ; 
  Bool_t notremoved = kTRUE ;

  while ( (digit = (AliPHOSDigit *)nextdigit()) ) { // scan over the list of digitsC
    AliPHOSRecPoint * clu = 0 ; 

    TArrayI clusterdigitslist(1500) ;   
    Int_t index ;

    if (( IsInEmc (digit) && Calibrate(digit->GetAmp(),digit->GetId()) > fEmcClusteringThreshold  ) || 
        ( IsInCpv (digit) && Calibrate(digit->GetAmp(),digit->GetId()) > fCpvClusteringThreshold  ) ) {
         
      Int_t iDigitInCluster = 0 ; 
      
      if  ( IsInEmc(digit) ) {   
	// start a new EMC RecPoint
	if(fNumberOfEmcClusters >= emcRecPoints->GetSize()) 
	  emcRecPoints->Expand(2*fNumberOfEmcClusters+1) ;
  	
	emcRecPoints->AddAt(new  AliPHOSEmcRecPoint(""), fNumberOfEmcClusters) ;
	clu = (AliPHOSEmcRecPoint *) emcRecPoints->At(fNumberOfEmcClusters) ; 
  	fNumberOfEmcClusters++ ; 
	clu->AddDigit(*digit, Calibrate(digit->GetAmp(),digit->GetId())) ; 
	clusterdigitslist[iDigitInCluster] = digit->GetIndexInList() ;	
	iDigitInCluster++ ; 
	digitsC->Remove(digit) ; 

      } else { 
	
	// start a new CPV cluster
	if(fNumberOfCpvClusters >= cpvRecPoints->GetSize()) 
	  cpvRecPoints->Expand(2*fNumberOfCpvClusters+1);

	cpvRecPoints->AddAt(new AliPHOSCpvRecPoint(""), fNumberOfCpvClusters) ;

	clu =  (AliPHOSCpvRecPoint *) cpvRecPoints->At(fNumberOfCpvClusters)  ;  
	fNumberOfCpvClusters++ ; 
	clu->AddDigit(*digit, Calibrate(digit->GetAmp(),digit->GetId()) ) ;	
	clusterdigitslist[iDigitInCluster] = digit->GetIndexInList()  ;	
	iDigitInCluster++ ; 
	digitsC->Remove(digit) ; 
        nextdigit.Reset() ;
	
	// Here we remove remaining EMC digits, which cannot make a cluster
	
        if( notremoved ) { 
	  while( ( digit = (AliPHOSDigit *)nextdigit() ) ) {
            if( IsInEmc(digit) ) 
	      digitsC->Remove(digit) ;
            else 
	      break ;
	  }
	  notremoved = kFALSE ;
	}
	
      } // else        
      
      nextdigit.Reset() ;
      
      AliPHOSDigit * digitN ; 
      index = 0 ;
      while (index < iDigitInCluster){ // scan over digits already in cluster 
	digit =  (AliPHOSDigit*)digits->At(clusterdigitslist[index])  ;      
	index++ ; 
        while ( (digitN = (AliPHOSDigit *)nextdigit()) ) { // scan over the reduced list of digits 
	  Int_t ineb = AreNeighbours(digit, digitN);       // call (digit,digitN) in THAT oder !!!!!
          switch (ineb ) {
          case 0 :   // not a neighbour
	    break ;
	  case 1 :   // are neighbours 
	    clu->AddDigit(*digitN, Calibrate( digitN->GetAmp(), digitN->GetId() ) ) ;
	    clusterdigitslist[iDigitInCluster] = digitN->GetIndexInList() ; 
	    iDigitInCluster++ ; 
	    digitsC->Remove(digitN) ;
	    break ;
          case 2 :   // too far from each other
	    goto endofloop;   
	  } // switch
	  
	} // while digitN
	
      endofloop: ;
	nextdigit.Reset() ; 
	
      } // loop over cluster     

    } // energy theshold  

    
  } // while digit

  delete digitsC ;

}

//____________________________________________________________________________
void AliPHOSClusterizerv1::MakeUnfolding()
{
  // Unfolds clusters using the shape of an ElectroMagnetic shower
  // Performs unfolding of all EMC/CPV clusters

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ; 
  

  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
  TObjArray * emcRecPoints = gime->EmcRecPoints(BranchName()) ; 
  TObjArray * cpvRecPoints = gime->CpvRecPoints(BranchName()) ; 
  TClonesArray * digits = gime->Digits() ; 
  
  // Unfold first EMC clusters 
  if(fNumberOfEmcClusters > 0){

    Int_t nModulesToUnfold = geom->GetNModules() ; 

    Int_t numberofNotUnfolded = fNumberOfEmcClusters ; 
    Int_t index ;   
    for(index = 0 ; index < numberofNotUnfolded ; index++){
      
      AliPHOSEmcRecPoint * emcRecPoint = (AliPHOSEmcRecPoint *) emcRecPoints->At(index) ;
      if(emcRecPoint->GetPHOSMod()> nModulesToUnfold)
	break ;
      
      Int_t nMultipl = emcRecPoint->GetMultiplicity() ; 
      AliPHOSDigit ** maxAt = new AliPHOSDigit*[nMultipl] ;
      Float_t * maxAtEnergy = new Float_t[nMultipl] ;
      Int_t nMax = emcRecPoint->GetNumberOfLocalMax(maxAt, maxAtEnergy,fEmcLocMaxCut,digits) ;
      
      if( nMax > 1 ) {     // if cluster is very flat (no pronounced maximum) then nMax = 0       
	UnfoldCluster(emcRecPoint, nMax, maxAt, maxAtEnergy) ;
	emcRecPoints->Remove(emcRecPoint); 
	emcRecPoints->Compress() ;
	index-- ;
	fNumberOfEmcClusters -- ;
	numberofNotUnfolded-- ;
      }
      
      delete[] maxAt ; 
      delete[] maxAtEnergy ; 
    }
  } 
  // Unfolding of EMC clusters finished


  // Unfold now CPV clusters
  if(fNumberOfCpvClusters > 0){
    
    Int_t nModulesToUnfold = geom->GetNModules() ;

    Int_t numberofCpvNotUnfolded = fNumberOfCpvClusters ;     
    Int_t index ;   
    for(index = 0 ; index < numberofCpvNotUnfolded ; index++){
      
      AliPHOSRecPoint * recPoint = (AliPHOSRecPoint *) cpvRecPoints->At(index) ;

      if(recPoint->GetPHOSMod()> nModulesToUnfold)
	break ;
      
      AliPHOSEmcRecPoint * emcRecPoint = (AliPHOSEmcRecPoint*) recPoint ; 
      
      Int_t nMultipl = emcRecPoint->GetMultiplicity() ; 
      AliPHOSDigit ** maxAt = new AliPHOSDigit*[nMultipl] ;
      Float_t * maxAtEnergy = new Float_t[nMultipl] ;
      Int_t nMax = emcRecPoint->GetNumberOfLocalMax(maxAt, maxAtEnergy,fCpvLocMaxCut,digits) ;
      
      if( nMax > 1 ) {     // if cluster is very flat (no pronounced maximum) then nMax = 0       
	UnfoldCluster(emcRecPoint, nMax, maxAt, maxAtEnergy) ;
	cpvRecPoints->Remove(emcRecPoint); 
	cpvRecPoints->Compress() ;
	index-- ;
	numberofCpvNotUnfolded-- ;
	fNumberOfCpvClusters-- ;
      }
      
      delete[] maxAt ; 
      delete[] maxAtEnergy ; 
    } 
  }
  //Unfolding of Cpv clusters finished
  
}

//____________________________________________________________________________
Double_t  AliPHOSClusterizerv1::ShowerShape(Double_t r)
{ 
  // Shape of the shower (see PHOS TDR)
  // If you change this function, change also the gradient evaluation in ChiSquare()

  Double_t r4    = r*r*r*r ;
  Double_t r295  = TMath::Power(r, 2.95) ;
  Double_t shape = TMath::Exp( -r4 * (1. / (2.32 + 0.26 * r4) + 0.0316 / (1 + 0.0652 * r295) ) ) ;
  return shape ;
}

//____________________________________________________________________________
void  AliPHOSClusterizerv1::UnfoldCluster(AliPHOSEmcRecPoint * iniEmc, 
						 Int_t nMax, 
						 AliPHOSDigit ** maxAt, 
						 Float_t * maxAtEnergy)
{ 
  // Performs the unfolding of a cluster with nMax overlapping showers 

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ; 

  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
  const TClonesArray * digits = gime->Digits() ; 
  TObjArray * emcRecPoints = gime->EmcRecPoints() ; 
  TObjArray * cpvRecPoints = gime->CpvRecPoints() ; 

  Int_t nPar = 3 * nMax ;
  Float_t * fitparameters = new Float_t[nPar] ;

  Bool_t rv = FindFit(iniEmc, maxAt, maxAtEnergy, nPar, fitparameters) ;
  if( !rv ) {
    // Fit failed, return and remove cluster
    delete[] fitparameters ; 
    return ;
  }

  // create ufolded rec points and fill them with new energy lists
  // First calculate energy deposited in each sell in accordance with fit (without fluctuations): efit[]
  // and later correct this number in acordance with actual energy deposition

  Int_t nDigits = iniEmc->GetMultiplicity() ;  
  Float_t * efit = new Float_t[nDigits] ;
  Float_t xDigit=0.,zDigit=0.,distance=0. ;
  Float_t xpar=0.,zpar=0.,epar=0.  ;
  Int_t relid[4] ;
  AliPHOSDigit * digit = 0 ;
  Int_t * emcDigits = iniEmc->GetDigitsList() ;

  Int_t iparam ;
  Int_t iDigit ;
  for(iDigit = 0 ; iDigit < nDigits ; iDigit ++){
    digit = (AliPHOSDigit*) digits->At(emcDigits[iDigit] ) ;   
    geom->AbsToRelNumbering(digit->GetId(), relid) ;
    geom->RelPosInModule(relid, xDigit, zDigit) ;
    efit[iDigit] = 0;

    iparam = 0 ;    
    while(iparam < nPar ){
      xpar = fitparameters[iparam] ;
      zpar = fitparameters[iparam+1] ;
      epar = fitparameters[iparam+2] ;
      iparam += 3 ;
      distance = (xDigit - xpar) * (xDigit - xpar) + (zDigit - zpar) * (zDigit - zpar)  ;
      distance =  TMath::Sqrt(distance) ;
      efit[iDigit] += epar * ShowerShape(distance) ;
    }
  }
  

  // Now create new RecPoints and fill energy lists with efit corrected to fluctuations
  // so that energy deposited in each cell is distributed betwin new clusters proportionally
  // to its contribution to efit

  Float_t * emcEnergies = iniEmc->GetEnergiesList() ;
  Float_t ratio ;

  iparam = 0 ;
  while(iparam < nPar ){
    xpar = fitparameters[iparam] ;
    zpar = fitparameters[iparam+1] ;
    epar = fitparameters[iparam+2] ;
    iparam += 3 ;    
    
    AliPHOSEmcRecPoint * emcRP = 0 ;  

    if(iniEmc->IsEmc()){ //create new entries in fEmcRecPoints...
      
      if(fNumberOfEmcClusters >= emcRecPoints->GetSize())
	emcRecPoints->Expand(2*fNumberOfEmcClusters) ;
      
      (*emcRecPoints)[fNumberOfEmcClusters] = new AliPHOSEmcRecPoint("") ;
      emcRP = (AliPHOSEmcRecPoint *) emcRecPoints->At(fNumberOfEmcClusters);
      fNumberOfEmcClusters++ ;
    }
    else{//create new entries in fCpvRecPoints
      if(fNumberOfCpvClusters >= cpvRecPoints->GetSize())
	cpvRecPoints->Expand(2*fNumberOfCpvClusters) ;
      
      (*cpvRecPoints)[fNumberOfCpvClusters] = new AliPHOSCpvRecPoint("") ;
      emcRP = (AliPHOSEmcRecPoint *) cpvRecPoints->At(fNumberOfCpvClusters);
      fNumberOfCpvClusters++ ;
    }
    
    Float_t eDigit ;
    for(iDigit = 0 ; iDigit < nDigits ; iDigit ++){
      digit = (AliPHOSDigit*) digits->At( emcDigits[iDigit] ) ; 
      geom->AbsToRelNumbering(digit->GetId(), relid) ;
      geom->RelPosInModule(relid, xDigit, zDigit) ;
      distance = (xDigit - xpar) * (xDigit - xpar) + (zDigit - zpar) * (zDigit - zpar)  ;
      distance =  TMath::Sqrt(distance) ;
      ratio = epar * ShowerShape(distance) / efit[iDigit] ; 
      eDigit = emcEnergies[iDigit] * ratio ;
      emcRP->AddDigit( *digit, eDigit ) ;
    }	
  }
 
  delete[] fitparameters ; 
  delete[] efit ; 
  
}

//_____________________________________________________________________________
void AliPHOSClusterizerv1::UnfoldingChiSquare(Int_t & nPar, Double_t * Grad, Double_t & fret, Double_t * x, Int_t iflag)
{
  // Calculates the Chi square for the cluster unfolding minimization
  // Number of parameters, Gradient, Chi squared, parameters, what to do

  TList * toMinuit = (TList*) gMinuit->GetObjectFit() ;

  AliPHOSEmcRecPoint * emcRP = (AliPHOSEmcRecPoint*) toMinuit->At(0)  ;
  TClonesArray * digits = (TClonesArray*)toMinuit->At(1)  ;


  
  //  AliPHOSEmcRecPoint * emcRP = (AliPHOSEmcRecPoint *) gMinuit->GetObjectFit() ; // EmcRecPoint to fit

  Int_t * emcDigits     = emcRP->GetDigitsList() ;

  Int_t nOdigits = emcRP->GetDigitsMultiplicity() ; 

  Float_t * emcEnergies = emcRP->GetEnergiesList() ;

  const AliPHOSGeometry * geom = AliPHOSGetter::GetInstance()->PHOSGeometry() ; 
  fret = 0. ;     
  Int_t iparam ;

  if(iflag == 2)
    for(iparam = 0 ; iparam < nPar ; iparam++)    
      Grad[iparam] = 0 ; // Will evaluate gradient
  
  Double_t efit ;    

  AliPHOSDigit * digit ;
  Int_t iDigit ;

  for( iDigit = 0 ; iDigit < nOdigits ; iDigit++) {

    digit = (AliPHOSDigit*) digits->At( emcDigits[iDigit] ) ; 

    Int_t relid[4] ;
    Float_t xDigit ;
    Float_t zDigit ;

    geom->AbsToRelNumbering(digit->GetId(), relid) ;

    geom->RelPosInModule(relid, xDigit, zDigit) ;

     if(iflag == 2){  // calculate gradient
       Int_t iParam = 0 ;
       efit = 0 ;
       while(iParam < nPar ){
	 Double_t distance = (xDigit - x[iParam]) * (xDigit - x[iParam]) ;
	 iParam++ ; 
	 distance += (zDigit - x[iParam]) * (zDigit - x[iParam]) ; 
	 distance = TMath::Sqrt( distance ) ; 
	 iParam++ ; 	 
	 efit += x[iParam] * ShowerShape(distance) ;
	 iParam++ ;
       }
       Double_t sum = 2. * (efit - emcEnergies[iDigit]) / emcEnergies[iDigit] ; // Here we assume, that sigma = sqrt(E) 
       iParam = 0 ;
       while(iParam < nPar ){
	 Double_t xpar = x[iParam] ;
	 Double_t zpar = x[iParam+1] ;
	 Double_t epar = x[iParam+2] ;
	 Double_t dr = TMath::Sqrt( (xDigit - xpar) * (xDigit - xpar) + (zDigit - zpar) * (zDigit - zpar) );
	 Double_t shape = sum * ShowerShape(dr) ;
	 Double_t r4 = dr*dr*dr*dr ;
	 Double_t r295 = TMath::Power(dr,2.95) ;
	 Double_t deriv =-4. * dr*dr * ( 2.32 / ( (2.32 + 0.26 * r4) * (2.32 + 0.26 * r4) ) +
					 0.0316 * (1. + 0.0171 * r295) / ( ( 1. + 0.0652 * r295) * (1. + 0.0652 * r295) ) ) ;
	 
	 Grad[iParam] += epar * shape * deriv * (xpar - xDigit) ;  // Derivative over x    
	 iParam++ ; 
	 Grad[iParam] += epar * shape * deriv * (zpar - zDigit) ;  // Derivative over z         
	 iParam++ ; 
	 Grad[iParam] += shape ;                                  // Derivative over energy     	
	 iParam++ ; 
       }
     }
     efit = 0;
     iparam = 0 ;

     while(iparam < nPar ){
       Double_t xpar = x[iparam] ;
       Double_t zpar = x[iparam+1] ;
       Double_t epar = x[iparam+2] ;
       iparam += 3 ;
       Double_t distance = (xDigit - xpar) * (xDigit - xpar) + (zDigit - zpar) * (zDigit - zpar)  ;
       distance =  TMath::Sqrt(distance) ;
       efit += epar * ShowerShape(distance) ;
     }

     fret += (efit-emcEnergies[iDigit])*(efit-emcEnergies[iDigit])/emcEnergies[iDigit] ; 
     // Here we assume, that sigma = sqrt(E)
  }

}

//____________________________________________________________________________
void AliPHOSClusterizerv1::Print(Option_t * option)const
{
  // Print clusterizer parameters

  if( strcmp(GetName(), "") !=0 ){
    
    // Print parameters
 
    TString taskName(GetName()) ; 
    taskName.ReplaceAll(Version(), "") ;

    cout << "---------------"<< taskName.Data() << " " << GetTitle()<< "-----------" << endl 
	 << "Clusterizing digits from the file: " << taskName.Data() << endl 
	 << "                           Branch: " << GetName() << endl 
	 << endl 
	 << "                       EMC Clustering threshold = " << fEmcClusteringThreshold << endl
	 << "                       EMC Local Maximum cut    = " << fEmcLocMaxCut << endl
	 << "                       EMC Logarothmic weight   = " << fW0 << endl
	 << endl
	 << "                       CPV Clustering threshold = " << fCpvClusteringThreshold << endl
	 << "                       CPV Local Maximum cut    = " << fCpvLocMaxCut << endl
       << "                       CPV Logarothmic weight   = " << fW0CPV << endl
	 << endl ;
    if(fToUnfold)
      cout << " Unfolding on " << endl ;
    else
      cout << " Unfolding off " << endl ;
    
    cout << "------------------------------------------------------------------" <<endl ;
  }
  else
    cout << " AliPHOSClusterizerv1 not initialized " << endl ;
}
//____________________________________________________________________________
void AliPHOSClusterizerv1::PrintRecPoints(Option_t * option)
{
  // Prints list of RecPoints produced at the current pass of AliPHOSClusterizer

  TObjArray * emcRecPoints = AliPHOSGetter::GetInstance()->EmcRecPoints(BranchName()) ; 
  TObjArray * cpvRecPoints = AliPHOSGetter::GetInstance()->CpvRecPoints(BranchName()) ; 

  cout << "AliPHOSClusterizerv1: : event "<<gAlice->GetEvNumber() << endl ;
  cout << "       Found "<< emcRecPoints->GetEntriesFast() << " EMC Rec Points and " 
	   << cpvRecPoints->GetEntriesFast() << " CPV RecPoints" << endl ;

  fRecPointsInRun +=  emcRecPoints->GetEntriesFast() ; 
  fRecPointsInRun +=  cpvRecPoints->GetEntriesFast() ; 

  if(strstr(option,"all")) {
    cout << "EMC clusters " << endl ;
    cout << " Index  Ene(MeV)   Multi  Module     X      Y      Z    Lambda 1   Lambda 2  # of prim  Primaries list "      <<  endl;      
    
    Int_t index ;
    for (index = 0 ; index < emcRecPoints->GetEntries() ; index++) {
      AliPHOSEmcRecPoint * rp = (AliPHOSEmcRecPoint * )emcRecPoints->At(index) ; 
      TVector3  locpos;  
      rp->GetLocalPosition(locpos);
      Float_t lambda[2]; 
      rp->GetElipsAxis(lambda);
      Int_t * primaries; 
      Int_t nprimaries;
      primaries = rp->GetPrimaries(nprimaries);

      cout << setw(4) << rp->GetIndexInList() << "   " 
	   << setw(7) << setprecision(3) << rp->GetEnergy() << "           " 
	   << setw(3) <<         rp->GetMultiplicity() << "  " 
	   << setw(1) <<              rp->GetPHOSMod() << "  " 
	   << setw(6) << setprecision(2) << locpos.X() << "  " 
	   << setw(6) << setprecision(2) << locpos.Y() << "  " 
	   << setw(6) << setprecision(2) << locpos.Z() << "  "
	   << setw(4) << setprecision(2) << lambda[0]  << "  "
	   << setw(4) << setprecision(2) << lambda[1]  << "  "
	   << setw(2) << nprimaries << "  " ;
     
      for (Int_t iprimary=0; iprimary<nprimaries; iprimary++)
	cout << setw(4) <<   primaries[iprimary] << "  "  ;
      cout << endl ;  	 
    }

    //Now plot CPV recPoints
    cout << "EMC clusters " << endl ;
    cout << "  Index    " 
	 << "  Multi    "
 	 << "  Module   "  
	 << "    X      "
	 << "    Y      "
	 << "    Z      "
	 << " # of prim "
	 << " Primaries list "      <<  endl;      
    
    for (index = 0 ; index < cpvRecPoints->GetEntries() ; index++) {
      AliPHOSRecPoint * rp = (AliPHOSRecPoint * )cpvRecPoints->At(index) ; 
      cout << setw(6) << rp->GetIndexInList() << "     ";
      cout << setw(6) << rp->GetPHOSMod()     << "        CPV     ";
      
      TVector3  locpos;  
      rp->GetLocalPosition(locpos);
      cout << setw(6) <<  locpos.X()          << "     ";
      cout << setw(6) <<  locpos.Y()          << "     ";
      cout << setw(6) <<  locpos.Z()          << "     ";
      
      Int_t * primaries; 
      Int_t nprimaries ; 
      primaries = rp->GetPrimaries(nprimaries);
      cout << setw(6) <<    nprimaries         << "     ";

      for (Int_t iprimary=0; iprimary<nprimaries; iprimary++)
	cout << setw(4)  <<  primaries[iprimary] << " ";
      cout << endl;  	 
    }


    cout << "-----------------------------------------------------------------------"<<endl ;
  }
}


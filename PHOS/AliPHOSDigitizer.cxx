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
//*-- Author :  Dmitri Peressounko (SUBATECH & Kurchatov Institute) 
//////////////////////////////////////////////////////////////////////////////
// This TTask performs digitization of Summable digits (in the PHOS case it is just
// the sum of contributions from all primary particles into a given cell). 
// In addition it performs mixing of summable digits from different events.
// The name of the TTask is also the title of the branch that will contain 
// the created SDigits
// The title of the TTAsk is the name of the file that contains the hits from
// which the SDigits are created
//
// For each event two branches are created in TreeD:
//   "PHOS" - list of digits
//   "AliPHOSDigitizer" - AliPHOSDigitizer with all parameters used in digitization
//
// Note, that one can set a title for new digits branch, and repeat digitization with
// another set of parameters.
//
// Use case:
// root[0] AliPHOSDigitizer * d = new AliPHOSDigitizer() ;
// root[1] d->ExecuteTask()             
// Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//                       //Digitizes SDigitis in all events found in file galice.root 
//
// root[2] AliPHOSDigitizer * d1 = new AliPHOSDigitizer("galice1.root") ;  
//                       // Will read sdigits from galice1.root
// root[3] d1->MixWith("galice2.root")       
// Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//                       // Reads another set of sdigits from galice2.root
// root[3] d1->MixWith("galice3.root")       
//                       // Reads another set of sdigits from galice3.root
// root[4] d->ExecuteTask("deb timing")    
//                       // Reads SDigits from files galice1.root, galice2.root ....
//                       // mixes them and stores produced Digits in file galice1.root          
//                       // deb - prints number of produced digits
//                       // deb all - prints list of produced digits
//                       // timing  - prints time used for digitization
//

// --- ROOT system ---
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TFolder.h"
#include "TObjString.h"
#include "TGeometry.h"
#include "TBenchmark.h"

// --- Standard library ---

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliHeader.h"
#include "AliStream.h"
#include "AliRunDigitizer.h"
#include "AliPHOSDigit.h"
#include "AliPHOS.h"
#include "AliPHOSGetter.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSTick.h"

ClassImp(AliPHOSDigitizer)


//____________________________________________________________________________ 
  AliPHOSDigitizer::AliPHOSDigitizer():AliDigitizer("","") 
{
  // ctor
  InitParameters() ; 
  fDefaultInit = kTRUE ;
  fManager = 0 ;                     // We work in the standalong mode
  fInputFileNames = 0 ; 
  fEventNames = 0 ; 
  fEventFolderName = "" ; 
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(const TString alirunFileName, const TString eventFolderName):
  AliDigitizer("PHOS"+AliConfig::fgkDigitizerTaskName, alirunFileName),
  fInputFileNames(0), fEventNames(0), fEventFolderName(eventFolderName)
{
  // ctor

  InitParameters() ; 
  Init() ;
  fDefaultInit = kFALSE ; 
  fManager = 0 ;                     // We work in the standalong mode
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(const AliPHOSDigitizer & d)
{
  // copyy ctor 

  SetName(d.GetName()) ; 
  SetTitle(d.GetTitle()) ; 
  fPinNoise = d.fPinNoise ; 
  fEMCDigitThreshold = d.fEMCDigitThreshold ; 
  fCPVNoise          = d.fCPVNoise ; 
  fCPVDigitThreshold = d.fCPVDigitThreshold ; 
  fTimeResolution    = d.fTimeResolution ; 
  fTimeThreshold     = d.fTimeThreshold ; 
  fTimeSignalLength  = d.fTimeSignalLength ; 
  fADCchanelEmc      = d.fADCchanelEmc ; 
  fADCpedestalEmc    = d.fADCpedestalEmc ; 
  fNADCemc           = d.fNADCemc ;
  fADCchanelCpv      = d.fADCchanelCpv ;
  fADCpedestalCpv    = d.fADCpedestalCpv ;
  fNADCcpv           = d.fNADCcpv ; 
  fEventFolderName   = d.fEventFolderName;
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(AliRunDigitizer * rd):
 AliDigitizer(rd,"PHOS"+AliConfig::fgkDigitizerTaskName),
 fEventFolderName(0)
{
  // ctor
  fManager = rd ; 
  SetName(fManager->GetInputFolderName(0)) ;
  // take title as name of stream 0
  SetTitle(dynamic_cast<AliStream*>(fManager->GetInputStream(0))->GetFileName(0));
  InitParameters() ; 
  Init() ; 
  fDefaultInit = kFALSE ; 
}

//____________________________________________________________________________ 
  AliPHOSDigitizer::~AliPHOSDigitizer()
{
  // dtor
  delete [] fInputFileNames ; 
  delete [] fEventNames ; 
 
}

//____________________________________________________________________________
void AliPHOSDigitizer::Digitize(const Int_t event) 
{ 
  
  // Makes the digitization of the collected summable digits.
  //  It first creates the array of all PHOS modules
  //  filled with noise (different for EMC, CPV and PPSD) and
  //  then adds contributions from SDigits. 
  // This design avoids scanning over the list of digits to add 
  // contribution to new SDigits only.

  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 

  
  //TTree * treeD = gime->TreeD();
  TClonesArray * digits = gime->Digits() ; 
  digits->Clear() ;

  const AliPHOSGeometry *geom = gime->PHOSGeometry() ; 
  //Making digits with noise, first EMC
  Int_t nEMC = geom->GetNModules()*geom->GetNPhi()*geom->GetNZ();
  
  Int_t nCPV ;
  Int_t absID ;
  
  nCPV = nEMC + geom->GetNumberOfCPVPadsZ() * geom->GetNumberOfCPVPadsPhi() * geom->GetNModules() ;
  
  digits->Expand(nCPV) ;

  // get first the sdigitizer from the tasks list 
  if ( !gime->SDigitizer() ) 
    gime->LoadSDigitizer();
  AliPHOSSDigitizer * sDigitizer = gime->SDigitizer(); 

  if ( !sDigitizer )
    Fatal("Digitize", "SDigitizer with name %s %s not found", fEventFolderName.Data(), GetTitle() ) ; 

  //take all the inputs to add together and load the SDigits
  TObjArray * sdigArray = new TObjArray(fInput) ;
  //gime->Event(event,"S");
  sdigArray->AddAt(gime->SDigits(), 0) ;
  Int_t i ;
  for(i = 1 ; i < fInput ; i++){
    TString tempo(fEventNames[i]) ; 
    tempo += i ;
    AliPHOSGetter * gime = AliPHOSGetter::Instance(fInputFileNames[i], tempo) ; 
    gime->Event(event,"S");
    sdigArray->AddAt(gime->SDigits(), i) ;
  }

  //Find the first crystall with signal
  Int_t nextSig = 200000 ; 
  TClonesArray * sdigits ;  
  for(i = 0 ; i < fInput ; i++){
    sdigits = dynamic_cast<TClonesArray *>(sdigArray->At(i)) ;
    if ( !sdigits->GetEntriesFast() )
      continue ; 
    Int_t curNext = dynamic_cast<AliPHOSDigit *>(sdigits->At(0))->GetId() ;
    if(curNext < nextSig) 
      nextSig = curNext ;
  }
  
  TArrayI index(fInput) ;
  index.Reset() ;  //Set all indexes to zero
  
  AliPHOSDigit * digit ;
  AliPHOSDigit * curSDigit ;
  
  TClonesArray * ticks = new TClonesArray("AliPHOSTick",1000) ;
  
  //Put Noise contribution
  for(absID = 1 ; absID <= nEMC ; absID++){
    Float_t noise = gRandom->Gaus(0., fPinNoise) ; 
    new((*digits)[absID-1]) AliPHOSDigit( -1,absID,sDigitizer->Digitize(noise), TimeOfNoise() ) ;
    //look if we have to add signal?
    digit = dynamic_cast<AliPHOSDigit *>(digits->At(absID-1)) ;
    
    if(absID==nextSig){
      //Add SDigits from all inputs 
      ticks->Clear() ;
      Int_t contrib = 0 ;
      Float_t a = digit->GetAmp() ;
      Float_t b = TMath::Abs( a / fTimeSignalLength) ;
      //Mark the beginnign of the signal
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime(),0, b);  
      //Mark the end of the ignal     
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime()+fTimeSignalLength, -a, -b); 
      
      //loop over inputs
      for(i = 0 ; i < fInput ; i++){
	if( dynamic_cast<TClonesArray *>(sdigArray->At(i))->GetEntriesFast() > index[i] )
	  curSDigit = dynamic_cast<AliPHOSDigit*>(dynamic_cast<TClonesArray *>(sdigArray->At(i))->At(index[i])) ; 	
	else
	  curSDigit = 0 ;
	//May be several digits will contribute from the same input
	while(curSDigit && curSDigit->GetId() == absID){	   
	  //Shift primary to separate primaries belonging different inputs
	  Int_t primaryoffset ;
	  if(fManager)
	    primaryoffset = fManager->GetMask(i) ; 
	  else
	    primaryoffset = 10000000*i ;
	  curSDigit->ShiftPrimary(primaryoffset) ;
	  
	  a = curSDigit->GetAmp() ;
	  b = a /fTimeSignalLength ;
	  new((*ticks)[contrib++]) AliPHOSTick(curSDigit->GetTime(),0, b);  
	  new((*ticks)[contrib++]) AliPHOSTick(curSDigit->GetTime()+fTimeSignalLength, -a, -b); 
	  
	  *digit = *digit + *curSDigit ;  //add energies
	  
	  index[i]++ ;
	  if( dynamic_cast<TClonesArray *>(sdigArray->At(i))->GetEntriesFast() > index[i] )
	    curSDigit = dynamic_cast<AliPHOSDigit*>(dynamic_cast<TClonesArray *>(sdigArray->At(i))->At(index[i])) ; 	
	  else
	    curSDigit = 0 ;
	}
      }
      
      //calculate and set time
      Float_t time = FrontEdgeTime(ticks) ;
      digit->SetTime(time) ;
      
      //Find next signal module
      nextSig = 200000 ;
      for(i = 0 ; i < fInput ; i++){
	sdigits = dynamic_cast<TClonesArray *>(sdigArray->At(i)) ;
	Int_t curNext = nextSig ;
	if(sdigits->GetEntriesFast() > index[i] ){
	  curNext = dynamic_cast<AliPHOSDigit *>(sdigits->At(index[i]))->GetId() ;
	}
	if(curNext < nextSig) nextSig = curNext ;
      }
    }
  }
  
  ticks->Delete() ;
  delete ticks ;
  
  
  //Now CPV digits (different noise and no timing)
  for(absID = nEMC+1; absID <= nCPV; absID++){
    Float_t noise = gRandom->Gaus(0., fCPVNoise) ; 
    new((*digits)[absID-1]) AliPHOSDigit( -1,absID,sDigitizer->Digitize(noise), TimeOfNoise() ) ;
    //look if we have to add signal?
    if(absID==nextSig){
      digit = dynamic_cast<AliPHOSDigit *>(digits->At(absID-1)) ;
      //Add SDigits from all inputs
      for(i = 0 ; i < fInput ; i++){
	if( dynamic_cast<TClonesArray *>(sdigArray->At(i))->GetEntriesFast() > index[i] )
	  curSDigit = dynamic_cast<AliPHOSDigit*>( dynamic_cast<TClonesArray *>(sdigArray->At(i))->At(index[i])) ; 	
	else
	  curSDigit = 0 ;

	//May be several digits will contribute from the same input
	while(curSDigit && curSDigit->GetId() == absID){	   
	  //Shift primary to separate primaries belonging different inputs
	  Int_t primaryoffset ;
	  if(fManager)
	    primaryoffset = fManager->GetMask(i) ; 
	  else
	    primaryoffset = 10000000*i ;
	  curSDigit->ShiftPrimary(primaryoffset) ;

	  //add energies
	  *digit = *digit + *curSDigit ;  
	  index[i]++ ;
	  if( dynamic_cast<TClonesArray *>(sdigArray->At(i))->GetEntriesFast() > index[i] )
	    curSDigit = dynamic_cast<AliPHOSDigit*>( dynamic_cast<TClonesArray *>(sdigArray->At(i))->At(index[i]) ) ; 	
	  else
	    curSDigit = 0 ;
	}
      }

      //Find next signal module
      nextSig = 200000 ;
      for(i = 0 ; i < fInput ; i++){
	sdigits = dynamic_cast<TClonesArray *>(sdigArray->At(i)) ;
	Int_t curNext = nextSig ;
	if(sdigits->GetEntriesFast() > index[i] )
	  curNext = dynamic_cast<AliPHOSDigit *>( sdigits->At(index[i]) )->GetId() ;
	if(curNext < nextSig) nextSig = curNext ;
      }
      
    }
  }
  delete sdigArray ; //We should not delete its contents
  
  
  
  //remove digits below thresholds
  for(i = 0; i < nEMC ; i++){
    digit = dynamic_cast<AliPHOSDigit*>( digits->At(i) ) ;
    if(sDigitizer->Calibrate( digit->GetAmp() ) < fEMCDigitThreshold)
      digits->RemoveAt(i) ;
    else
      digit->SetTime(gRandom->Gaus(digit->GetTime(),fTimeResolution) ) ;
  }


  for(i = nEMC; i < nCPV ; i++)
    if( sDigitizer->Calibrate( dynamic_cast<AliPHOSDigit*>(digits->At(i))->GetAmp() ) < fCPVDigitThreshold )
      digits->RemoveAt(i) ;
    
  digits->Compress() ;  
  
  Int_t ndigits = digits->GetEntriesFast() ;
  digits->Expand(ndigits) ;

  //Set indexes in list of digits and make true digitization of the energy
  for (i = 0 ; i < ndigits ; i++) { 
    digit = dynamic_cast<AliPHOSDigit*>( digits->At(i) ) ; 
    digit->SetIndexInList(i) ;     
    Float_t energy = sDigitizer->Calibrate(digit->GetAmp()) ;
    digit->SetAmp(DigitizeEnergy(energy,digit->GetId()) ) ;
  }

}

//____________________________________________________________________________
Int_t AliPHOSDigitizer::DigitizeEnergy(Float_t energy, Int_t absId)
{
  // Returns digitized value of the energy in a cell absId

  Int_t chanel ;
  if(absId <= fEmcCrystals){ //digitize as EMC 
    chanel = (Int_t) TMath::Ceil((energy - fADCpedestalEmc)/fADCchanelEmc) ;       
    if(chanel > fNADCemc ) chanel =  fNADCemc ;
  }
  else{ //Digitize as CPV
    chanel = (Int_t) TMath::Ceil((energy - fADCpedestalCpv)/fADCchanelCpv) ;       
    if(chanel > fNADCcpv ) chanel =  fNADCcpv ;
  }
  return chanel ;
}

//____________________________________________________________________________
void AliPHOSDigitizer::Exec(Option_t *option) 
{ 
  // Does the job

  if (!fInit) { // to prevent overwrite existing file
    Error( "Exec", "Give a version name different from %s", fEventFolderName.Data() ) ;
    return ;
  }   

  if (strstr(option,"print")) {
    Print();
    return ; 
  }
  
  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSDigitizer");
  
  if (fManager) 
    fInput = fManager->GetNinputs() ;
  
  Int_t nevents ;
  
  AliPHOSGetter * gime = AliPHOSGetter::Instance() ;
  
  nevents = gime->MaxEvent() ;
  
  Int_t ievent ;

  for(ievent = 0; ievent < nevents; ievent++){
 
    gime->Event(ievent,"S") ;

    Digitize(ievent) ; //Add prepared SDigits to digits and add the noise

    WriteDigits(ievent) ;

    if(strstr(option,"deb"))
      PrintDigits(option);
    
    //increment the total number of Digits per run 
    fDigitsInRun += gime->Digits()->GetEntriesFast() ;  
 }
  
  if(strstr(option,"tim")){
    gBenchmark->Stop("PHOSDigitizer");
    TString message ; 
    message = "  took %f seconds for Digitizing %f seconds per event\n" ; 
    Info("Exec", message.Data(), 
	 gBenchmark->GetCpuTime("PHOSDigitizer"), 
	 gBenchmark->GetCpuTime("PHOSDigitizer")/nevents ); 
  } 
}

//____________________________________________________________________________ 
Float_t AliPHOSDigitizer::FrontEdgeTime(TClonesArray * ticks) const
{
  // Returns the shortest time among all time ticks

  ticks->Sort() ; //Sort in accordance with times of ticks
  TIter it(ticks) ;
  AliPHOSTick * ctick = (AliPHOSTick *) it.Next() ;
  Float_t time = ctick->CrossingTime(fTimeThreshold) ;    

  AliPHOSTick * t ;  
  while((t=(AliPHOSTick*) it.Next())){
    if(t->GetTime() < time)  //This tick starts before crossing
      *ctick+=*t ;
    else
      return time ;

    time = ctick->CrossingTime(fTimeThreshold) ;    
  }
  return time ;
}

//____________________________________________________________________________ 
Bool_t AliPHOSDigitizer::Init()
{
  // Makes all memory allocations
  fInit = kTRUE ; 
  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 
  if ( gime == 0 ) {
    Fatal("Init" ,"Could not obtain the Getter object for file %s and event %s !", GetTitle(), fEventFolderName.Data()) ;  
    return kFALSE;
  } 
  
  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;

  fEmcCrystals = geom->GetNModules() * geom->GetNCristalsInModule() ;
  
  TString opt("Digits") ; 
  if(gime->VersionExists(opt) ) { 
    Error( "Init", "Give a version name different from %s", fEventFolderName.Data() ) ;
    fInit = kFALSE ; 
  }
  //else 
  //  Info("Init", "name = %s\n", gime->GetDigitsFileName().Data()) ; 

  // Post Digitizer to the white board
  gime->PostDigitizer(this) ;
  
  if (fManager) {
    fInput = fManager->GetNinputs() ; 
  } else {
    fInput           = 1 ;
  }
  fInputFileNames  = new TString[fInput] ; 
  fEventNames      = new TString[fInput] ; 
  fInputFileNames[0] = GetTitle() ; 
  fEventNames[0]     = fEventFolderName.Data() ; 
  Int_t index ; 
  for (index = 1 ; index < fInput ; index++) {
    fInputFileNames[index] = dynamic_cast<AliStream*>(fManager->GetInputStream(index))->GetFileName(0); 
    TString tempo = fManager->GetInputFolderName(index) ;
    fEventNames[index] = tempo.Remove(tempo.Length()-1) ; // strip of the stream number added bt fManager 
  }

  //to prevent cleaning of this object while GetEvent is called
  gime->PhosLoader()->GetDigitsDataLoader()->GetBaseTaskLoader()->SetDoNotReload(kTRUE);

  return fInit ; 
}

//____________________________________________________________________________ 
void AliPHOSDigitizer::InitParameters()
{
  // Set initial parameters for Digitizer

  fPinNoise           = 0.004 ;
  fEMCDigitThreshold  = 0.012 ;
  fCPVNoise           = 0.01;
  fCPVDigitThreshold  = 0.09 ;
  fTimeResolution     = 0.5e-9 ;
  fTimeSignalLength   = 1.0e-9 ;
  fDigitsInRun  = 0 ; 
  fADCchanelEmc = 0.0015;        // width of one ADC channel in GeV
  fADCpedestalEmc = 0.005 ;      //
  fNADCemc = (Int_t) TMath::Power(2,16) ;  // number of channels in EMC ADC

  fADCchanelCpv = 0.0012 ;          // width of one ADC channel in CPV 'popugais'
  fADCpedestalCpv = 0.012 ;         // 
  fNADCcpv = (Int_t) TMath::Power(2,12);      // number of channels in CPV ADC

  fTimeThreshold = 0.001*10000000 ; //Means 1 MeV in terms of SDigits amplitude
    
}

//__________________________________________________________________
void AliPHOSDigitizer::MixWith(const TString alirunFileName, const TString eventFolderName)
{
  // Allows to produce digits by superimposing background and signal event.
  // It is assumed, that headers file with SIGNAL events is opened in 
  // the constructor. 
  // Sets the BACKGROUND event, with which the SIGNAL event is to be mixed 
  // Thus we avoid writing (changing) huge and expensive 
  // backgound files: all output will be writen into SIGNAL, i.e. 
  // opened in constructor file. 
  //
  // One can open as many files to mix with as one needs.
  // However only Sdigits with the same name (i.e. constructed with the same SDigitizer)
  // can be mixed.

  if( strcmp(fEventFolderName, "") == 0 )
    Init() ;

  if(fManager){
    Warning("MixWith", "Cannot use this method with AliRunDigitizer\n" ) ;
    return ;
  }
  // looking for file which contains AliRun
  if (gSystem->AccessPathName(alirunFileName)) {// file does not exist
    Error("MixWith", "File %s does not exist!", alirunFileName.Data()) ;
    return ; 
  }
  // looking for the file which contains SDigits
  AliPHOSGetter * gime = AliPHOSGetter::Instance() ; 
  TString fileName( gime->GetSDigitsFileName() ) ; 
    if ( eventFolderName != AliConfig::fgkDefaultEventFolderName) // only if not the default folder name 
      fileName = fileName.ReplaceAll(".root", "") + "_" + eventFolderName + ".root" ;
    if ( (gSystem->AccessPathName(fileName)) ) { 
      Error("MixWith", "The file %s does not exist!", fileName.Data()) ;
      return ;
    }
    // need to increase the arrays
    TString tempo = fInputFileNames[fInput-1] ; 
    delete [] fInputFileNames ; 
    fInputFileNames = new TString[fInput+1] ; 
    fInputFileNames[fInput-1] = tempo ; 
 
    tempo = fEventNames[fInput-1] ; 
    delete [] fEventNames ; 
    fEventNames = new TString[fInput+1] ; 
    fEventNames[fInput-1] = tempo ; 

    fInputFileNames[fInput] = alirunFileName ; 
    fEventNames[fInput]     = eventFolderName ;
    fInput++ ;
}

//__________________________________________________________________
void AliPHOSDigitizer::Print()const 
{
  // Print Digitizer's parameters
  Info("Print", "\n------------------- %s -------------", GetName() ) ; 
  if( strcmp(fEventFolderName.Data(), "") != 0 ){
    printf(" Writing Digits to branch with title  %s\n", fEventFolderName.Data()) ;
    
    Int_t nStreams ; 
    if (fManager) 
      nStreams =  GetNInputStreams() ;
    else 
      nStreams = fInput ; 
    
    Int_t index = 0 ;  
    for (index = 0 ; index < nStreams ; index++) {  
	TString tempo(fEventNames[index]) ; 
	tempo += index ;
	AliPHOSGetter * gime = AliPHOSGetter::Instance(fInputFileNames[index], tempo) ; 
	TString fileName( gime->GetSDigitsFileName() ) ; 
	if ( fEventNames[index] != AliConfig::fgkDefaultEventFolderName) // only if not the default folder name 
	  fileName = fileName.ReplaceAll(".root", "") + "_" + fEventNames[index]  + ".root" ;
	printf ("Adding SDigits from %s %s\n", fInputFileNames[index].Data(), fileName.Data()) ; 
    }
    AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 
    printf("\nWriting digits to %s", gime->GetDigitsFileName().Data()) ;
    
    printf("\nWith following parameters:\n") ;
    printf("  Electronics noise in EMC (fPinNoise) = %f\n", fPinNoise ) ; 
    printf("  Threshold  in EMC  (fEMCDigitThreshold) = %f\n", fEMCDigitThreshold ) ;  
    printf("  Noise in CPV (fCPVNoise) = %f\n", fCPVNoise ) ; 
    printf("  Threshold in CPV (fCPVDigitThreshold) = %f\n",fCPVDigitThreshold ) ;  
    printf(" ---------------------------------------------------\n") ;   
  }
  else
    Info("Print", "AliPHOSDigitizer not initialized\n" ) ;
  
}

//__________________________________________________________________
 void AliPHOSDigitizer::PrintDigits(Option_t * option)
{
  // Print a table of digits
  
  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 
  TClonesArray * digits = gime->Digits() ; 
  
  TString message ; 
  message  = "\nevent " ;
  message += gAlice->GetEvNumber() ;
  message += "\n       Number of entries in Digits list " ;  
  message += digits->GetEntriesFast()  ;  
  char * tempo = new char[8192]; 
 
  if(strstr(option,"all")||strstr(option,"EMC")){  
    //loop over digits
    AliPHOSDigit * digit;
    message += "\nEMC digits (with primaries):\n"  ;
    message += "\n   Id  Amplitude    Time          Index Nprim: Primaries list \n" ;    
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; (index < digits->GetEntriesFast()) && 
	   (((AliPHOSDigit * )  digits->At(index))->GetId() <= maxEmc) ; index++) {
      digit = (AliPHOSDigit * )  digits->At(index) ;
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
    message += "\nCPV digits:\n" ;
    message += "\n   Id  Amplitude    Index Nprim: Primaries list \n" ;    

    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; index < digits->GetEntriesFast(); index++) {
      digit = (AliPHOSDigit * )  digits->At(index) ;
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
  Info("Print", message.Data() ) ; 
}

//__________________________________________________________________
Float_t AliPHOSDigitizer::TimeOfNoise(void) const
{  // Calculates the time signal generated by noise
  //to be rewritten, now returns just big number
  return 1. ;

}

//__________________________________________________________________
void AliPHOSDigitizer::Unload() 
{  
  
  Int_t i ; 
  for(i = 1 ; i < fInput ; i++){
    TString tempo(fEventNames[i]) ; 
    tempo += i ;
    AliPHOSGetter * gime = AliPHOSGetter::Instance(fInputFileNames[i], tempo) ; 
    gime->PhosLoader()->UnloadSDigits() ; 
  }
  
  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 
  gime->PhosLoader()->UnloadDigits() ; 
}

//____________________________________________________________________________
void AliPHOSDigitizer::WriteDigits(Int_t event)
{

  // Makes TreeD in the output file. 
  // Check if branch already exists: 
  //   if yes, exit without writing: ROOT TTree does not support overwriting/updating of 
  //      already existing branches. 
  //   else creates branch with Digits, named "PHOS", title "...",
  //      and branch "AliPHOSDigitizer", with the same title to keep all the parameters
  //      and names of files, from which digits are made.

  AliPHOSGetter * gime = AliPHOSGetter::Instance(GetTitle(), fEventFolderName) ; 
  const TClonesArray * digits = gime->Digits() ; 
  TTree * treeD = gime->TreeD();

  // -- create Digits branch
  Int_t bufferSize = 32000 ;    
  TBranch * digitsBranch = treeD->Branch("PHOS",&digits,bufferSize);
  digitsBranch->SetTitle(fEventFolderName);
  digitsBranch->Fill() ;
  
  gime->WriteDigits("OVERWRITE");
  gime->WriteDigitizer("OVERWRITE");

  Unload() ; 

}

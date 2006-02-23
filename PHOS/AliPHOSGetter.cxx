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

#include <TFile.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TParticle.h>
#include <TF1.h>
#include <TGraph.h>
//#include <TCanvas.h>
//#include <TFrame.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliESD.h"
#include "AliHeader.h"  
#include "AliMC.h"
#include "AliPHOS.h"
#include "AliPHOSBeamTestEvent.h"
#include "AliPHOSGetter.h"
#include "AliPHOSLoader.h"
#include "AliRunLoader.h"
#include "AliStack.h"  
#include "AliPHOSRawStream.h"
#include "AliRawReaderFile.h"
#include "AliLog.h"
#include "AliCDBLocal.h"
#include "AliCDBStorage.h"
#include "AliCDBManager.h"

ClassImp(AliPHOSGetter)
  
AliPHOSGetter * AliPHOSGetter::fgObjGetter = 0 ; 
AliPHOSLoader * AliPHOSGetter::fgPhosLoader = 0;
Int_t AliPHOSGetter::fgDebug = 0;
AliPHOSCalibData* AliPHOSGetter::fCalibData = 0;
AliPHOSAlignData* AliPHOSGetter::fAlignData = 0;

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
      rl->LoadHeader();
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
 
  fESDFileName = rl->GetFileName()  ; // this should be the galice.root file
  fESDFileName.ReplaceAll("galice.root", "AliESDs.root") ;  
  fESDFile = 0 ; 
  fESD = 0 ; 
  fESDTree = 0 ; 
  fRawDigits = kFALSE ;

}

//____________________________________________________________________________ 
AliPHOSGetter::~AliPHOSGetter()
{
  // dtor
  if(fgPhosLoader){
    delete fgPhosLoader ;
    fgPhosLoader = 0 ;
  }
  if(fBTE){
    delete fBTE ; 
    fBTE = 0 ;
  } 
  if(fPrimaries){
    fPrimaries->Delete() ; 
    delete fPrimaries ;
  } 
  if (fESD) 
    delete fESD ; 
  if (fESDTree) 
    delete fESDTree ;
 
  fgObjGetter = 0;
}

//____________________________________________________________________________ 
void AliPHOSGetter::Reset()
{
  // resets things in case the getter is called consecutively with different files
  // the PHOS Loader is already deleted by the Run Loader

  if (fPrimaries) { 
    fPrimaries->Delete() ; 
    delete fPrimaries ;
  } 
  fgPhosLoader = 0; 
  fgObjGetter = 0; 
}

//____________________________________________________________________________ 
AliPHOSClusterizer * AliPHOSGetter::Clusterizer()
{ 
  // Returns pointer to the Clusterizer task 
  AliPHOSClusterizer * rv ; 
  rv =  dynamic_cast<AliPHOSClusterizer *>(PhosLoader()->Reconstructioner()) ;
  if (!rv) {
    Event(0, "R") ; 
    rv =  dynamic_cast<AliPHOSClusterizer*>(PhosLoader()->Reconstructioner()) ;
  }
  return rv ; 
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
AliPHOSDigitizer * AliPHOSGetter::Digitizer() 
{ 
  // Returns pointer to the Digitizer task 
  AliPHOSDigitizer * rv ; 
  rv =  dynamic_cast<AliPHOSDigitizer *>(PhosLoader()->Digitizer()) ;
  if (!rv) {
    Event(0, "D") ; 
    rv =  dynamic_cast<AliPHOSDigitizer *>(PhosLoader()->Digitizer()) ;
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
AliPHOSTrackSegmentMaker * AliPHOSGetter::TrackSegmentMaker()
{ 
  // Returns pointer to the TrackSegmentMaker task 
  AliPHOSTrackSegmentMaker * rv ; 
  rv =  dynamic_cast<AliPHOSTrackSegmentMaker *>(PhosLoader()->TrackSegmentMaker()) ;
  if (!rv) {
    Event(0, "T") ; 
    rv =  dynamic_cast<AliPHOSTrackSegmentMaker *>(PhosLoader()->TrackSegmentMaker()) ;
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
void AliPHOSGetter::Event(Int_t event, const char* opt) 
{
  // Reads the content of all Tree's S, D and R

//   if ( event >= MaxEvent() ) {
//     Error("Event", "%d not found in TreeE !", event) ; 
//     return ; 
//   }

  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());

//   // checks if we are dealing with test-beam data
//   TBranch * btb = rl->TreeE()->GetBranch("AliPHOSBeamTestEvent") ;
//   if(btb){
//     if(!fBTE)
//       fBTE = new AliPHOSBeamTestEvent() ;
//     btb->SetAddress(&fBTE) ;
//     btb->GetEntry(event) ;
//   }
//   else{
//     if(fBTE){
//       delete fBTE ;
//       fBTE = 0 ;
//     }
//   }

  // Loads the type of object(s) requested
  
  rl->GetEvent(event) ;

  if(strstr(opt,"X") || (strcmp(opt,"")==0)){
    ReadPrimaries() ;
  }
  
  if(strstr(opt,"H")  ){
    ReadTreeH();
  }
  
  if(strstr(opt,"S")  ){
    ReadTreeS() ;
  }
  
  if(strstr(opt,"D") ){
    ReadTreeD() ;
  }
  
  if(strstr(opt,"R") ){
    ReadTreeR() ;
  }

  if( strstr(opt,"T") ){
    ReadTreeT() ;
  }    

  if( strstr(opt,"P") ){
    ReadTreeP() ;
  }    

  if( strstr(opt,"E") ){
    ReadTreeE(event) ;
  }

}


//____________________________________________________________________________ 
void AliPHOSGetter::Event(AliRawReader *rawReader, const char* opt) 
{
  // Reads the raw event from rawReader

  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  
  if( strstr(opt,"W")  ){
    rawReader->NextEvent(); 
    Int_t iEvent = rl->GetEventNumber();
    rl->GetEvent(iEvent);
    ReadRaw(rawReader) ;
    rl->SetEventNumber(++iEvent);
  }    
 
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
    else {
      AliRunLoader * rl = AliRunLoader::GetRunLoader(fgPhosLoader->GetTitle()) ; 
      if ( strstr(version, AliConfig::GetDefaultEventFolderName()) ) // false in case of merging
	delete rl ; 
      fgObjGetter = new AliPHOSGetter(alirunFileName, version, openingOption) ;      
    }
  }
  if (!fgObjGetter) 
    ::Error("AliPHOSGetter::Instance", "Failed to create the PHOS Getter object") ;
  else 
    if (fgDebug)
      Print() ;
  
  return fgObjGetter ;
}

//____________________________________________________________________________ 
AliPHOSGetter *  AliPHOSGetter::Instance()
{
  // Returns the pointer of the unique instance already defined
  
  if(!fgObjGetter && fgDebug)
     ::Warning("AliPHOSGetter::Instance", "Getter not initialized") ;

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
  AliPHOSLoader *    loader = 0;
  static AliPHOSLoader * oldloader = 0;
  static AliPHOS * phos = 0;

  loader = PhosLoader();

  if ( loader != oldloader) {
    phos = dynamic_cast<AliPHOS*>(loader->GetModulesFolder()->FindObject("PHOS")) ;
    oldloader = loader;
  }
  if (!phos) 
    if (fgDebug)
      Warning("PHOS", "PHOS module not found in module folders: %s", PhosLoader()->GetModulesFolder()->GetName() ) ; 
  return phos ; 
}  



//____________________________________________________________________________ 
AliPHOSPID * AliPHOSGetter::PID()
{ 
  // Returns pointer to the PID task 
  AliPHOSPID * rv ; 
  rv =  dynamic_cast<AliPHOSPID *>(PhosLoader()->PIDTask()) ;
  if (!rv) {
    Event(0, "P") ; 
    rv =  dynamic_cast<AliPHOSPID *>(PhosLoader()->PIDTask()) ;
  }
  return rv ; 
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
  if ( ! rl->TreeK() ) { // load treeK the first time
    rl->LoadKinematics() ;
  }
  
  fNPrimaries = (rl->GetHeader())->GetNtrack(); 
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
Bool_t AliPHOSGetter::OpenESDFile() 
{
  //Open the ESD file    
  Bool_t rv = kTRUE ; 
  if (!fESDFile) {
    fESDFile = TFile::Open(fESDFileName) ;
    if (!fESDFile ) 
      return kFALSE ; 
  }
  else if (fESDFile->IsOpen()) {
    fESDFile->Close() ; 
    fESDFile = TFile::Open(fESDFileName) ;
  }
  if (!fESDFile->IsOpen())
    rv = kFALSE ; 
  return rv ; 
}

//____________________________________________________________________________ 
void AliPHOSGetter::FitRaw(Bool_t lowGainFlag, TGraph * gLowGain, TGraph * gHighGain, TF1* signalF, Double_t & energy, Double_t & time)
{
  // Fits the raw signal time distribution 

  const Int_t kNoiseThreshold = 0 ;
  Double_t timezero1 = 0., timezero2 = 0., timemax = 0. ;
  Double_t signal = 0., signalmax = 0. ;       
  time   = 0. ; 
  energy = 0. ; 

  if (lowGainFlag) {
    timezero1 = timezero2 = signalmax = timemax = 0. ;
    signalF->FixParameter(0, PHOS()->GetRawFormatLowCharge()) ; 
    signalF->FixParameter(1, PHOS()->GetRawFormatLowGain()) ; 
    Int_t index ; 
    for (index = 0; index < PHOS()->GetRawFormatTimeBins(); index++) {
      gLowGain->GetPoint(index, time, signal) ; 
      if (signal > kNoiseThreshold && timezero1 == 0.) 
	timezero1 = time ;
      if (signal <= kNoiseThreshold && timezero1 > 0. && timezero2 == 0.)
	timezero2 = time ; 
      if (signal > signalmax) {
	signalmax = signal ; 
	timemax   = time ; 
      }
    }
    signalmax /= PHOS()->RawResponseFunctionMax(PHOS()->GetRawFormatLowCharge(), 
						PHOS()->GetRawFormatLowGain()) ;
    if ( timezero1 + PHOS()->GetRawFormatTimePeak() < PHOS()->GetRawFormatTimeMax() * 0.4 ) { // else its noise 
      signalF->SetParameter(2, signalmax) ; 
      signalF->SetParameter(3, timezero1) ;    	    
      gLowGain->Fit(signalF, "QRON", "", 0., timezero2); //, "QRON") ; 
      energy = signalF->GetParameter(2) ; 
      time   = signalF->GetMaximumX() - PHOS()->GetRawFormatTimePeak() - PHOS()->GetRawFormatTimeTrigger() ;
    }
  } else {
    timezero1 = timezero2 = signalmax = timemax = 0. ;
    signalF->FixParameter(0, PHOS()->GetRawFormatHighCharge()) ; 
    signalF->FixParameter(1, PHOS()->GetRawFormatHighGain()) ; 
    Int_t index ; 
    for (index = 0; index < PHOS()->GetRawFormatTimeBins(); index++) {
      gHighGain->GetPoint(index, time, signal) ;               
      if (signal > kNoiseThreshold && timezero1 == 0.) 
	timezero1 = time ;
      if (signal <= kNoiseThreshold && timezero1 > 0. && timezero2 == 0.)
	timezero2 = time ; 
      if (signal > signalmax) {
	signalmax = signal ;   
	timemax   = time ; 
      }
    }
    signalmax /= PHOS()->RawResponseFunctionMax(PHOS()->GetRawFormatHighCharge(), 
						PHOS()->GetRawFormatHighGain()) ;;
    if ( timezero1 + PHOS()->GetRawFormatTimePeak() < PHOS()->GetRawFormatTimeMax() * 0.4 ) { // else its noise  
      signalF->SetParameter(2, signalmax) ; 
      signalF->SetParameter(3, timezero1) ;               
      gHighGain->Fit(signalF, "QRON", "", 0., timezero2) ; 
      energy = signalF->GetParameter(2) ; 
      time   = signalF->GetMaximumX() - PHOS()->GetRawFormatTimePeak() - PHOS()->GetRawFormatTimeTrigger() ;
    }
  }
  if (time == 0) energy = 0 ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::CalibrateRaw(Double_t energy, Int_t *relId)
{
  // Convert energy into digitized amplitude for a cell relId
  // It is a user responsilibity to open CDB and set
  // AliPHOSCalibData object by the following operators:
  // 
  // AliCDBLocal *loc = new AliCDBLocal("deCalibDB");
  // AliPHOSCalibData* clb = (AliPHOSCalibData*)AliCDBStorage::Instance()
  //    ->Get(path_to_calibdata,run_number);
  // AliPHOSGetter* gime = AliPHOSGetter::Instance("galice.root");
  // gime->SetCalibData(clb);

  if (CalibData() == 0)
    Warning("CalibrateRaw","Calibration DB is not initiated!");

  Int_t   module = relId[0];
  Int_t   column = relId[3];
  Int_t   row    = relId[2];

  Float_t gainFactor = 0.0015; // width of one Emc ADC channel in GeV
  Float_t pedestal   = 0.005;  // Emc pedestals

  if(CalibData()) {
    gainFactor = CalibData()->GetADCchannelEmc (module,column,row);
    pedestal   = CalibData()->GetADCpedestalEmc(module,column,row);
  }
  
  Int_t   amp = static_cast<Int_t>( (energy - pedestal) / gainFactor + 0.5 ) ; 
  return amp;
}
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadRaw(AliRawReader *rawReader)
{
  // reads the raw format data, converts it into digits format and store digits in Digits()
  // container.

  AliPHOSRawStream in(rawReader);
 
  Bool_t first = kTRUE ;
  
  TF1 * signalF = new TF1("signal", AliPHOS::RawResponseFunction, 0, PHOS()->GetRawFormatTimeMax(), 4);
  signalF->SetParNames("Charge", "Gain", "Amplitude", "TimeZero") ; 

  Int_t relId[4], id =0;
  Bool_t lowGainFlag = kFALSE ; 
 
  TClonesArray * digits = Digits() ;
  digits->Clear() ; 
  Int_t idigit = 0 ; 
  Int_t amp    = 0 ; 
  Double_t energy = 0. ; 
  Double_t time   = 0. ; 

  TGraph * gLowGain = new TGraph(PHOS()->GetRawFormatTimeBins()) ; 
  TGraph * gHighGain= new TGraph(PHOS()->GetRawFormatTimeBins()) ;  

  while ( in.Next() ) { // PHOS entries loop 
    if ( (in.IsNewRow() || in.IsNewColumn() || in.IsNewModule()) ) {
      relId[0] = in.GetModule() ;
      if ( relId[0] >= PHOS()->GetRawFormatLowGainOffset() ) { 
	relId[0] -=  PHOS()->GetRawFormatLowGainOffset() ;
	lowGainFlag = kTRUE ;
      } else
	lowGainFlag = kFALSE ;
      relId[1] = 0 ;
      relId[2] = in.GetRow() ;
      relId[3] = in.GetColumn() ;
      PHOSGeometry()->RelToAbsNumbering(relId, id) ;
      if (!first) {
	FitRaw(lowGainFlag, gLowGain, gHighGain, signalF, energy, time) ; 
 	amp = CalibrateRaw(energy,relId);
	if (amp > 0) {
	  new((*digits)[idigit]) AliPHOSDigit( -1, id, amp, time) ;	
	  idigit++ ; 
	}
	Int_t index ; 
	for (index = 0; index < PHOS()->GetRawFormatTimeBins(); index++) {
	  gLowGain->SetPoint(index, index * PHOS()->GetRawFormatTimeMax() / PHOS()->GetRawFormatTimeBins(), 0) ;  
	  gHighGain->SetPoint(index, index * PHOS()->GetRawFormatTimeMax() / PHOS()->GetRawFormatTimeBins(), 0) ; 
	} 
      }
      first = kFALSE ; 
    }
    if (lowGainFlag)
      gLowGain->SetPoint(in.GetTime(), 
		     in.GetTime()* PHOS()->GetRawFormatTimeMax() / PHOS()->GetRawFormatTimeBins(), 
		     in.GetSignal()) ;
    else 
      gHighGain->SetPoint(in.GetTime(), 
			 in.GetTime() * PHOS()->GetRawFormatTimeMax() / PHOS()->GetRawFormatTimeBins(), 
			 in.GetSignal() ) ;

  } // PHOS entries loop

  FitRaw(lowGainFlag, gLowGain, gHighGain, signalF, energy, time) ; 
  amp = CalibrateRaw(energy,relId);
  if (amp > 0 ) {
    new((*digits)[idigit]) AliPHOSDigit( -1, id, amp, time) ;
    idigit++ ; 
  }
  digits->Sort() ; 

  delete signalF ; 
  delete gLowGain;
  delete gHighGain ; 
  
//   AliInfo(Form("Event %d, digits: %d\n", EventNumber(),digits->GetEntries())); //bvp
  return digits->GetEntriesFast() ; 
}

//   TClonesArray * digits = Digits() ;
//   digits->Clear() ; 
//   Int_t idigit = 0 ; 

//   while ( in.Next() ) { // PHOS entries loop 
 
//     Int_t amp = in.GetSignal() ; 
//     Double_t time = in.GetTime() ; 
//     Int_t relId[4], id ;

//     relId[0] = in.GetModule() ;
//     if ( relId[0] >= PHOS()->GetRawFormatLowGainOffset() ) { 
//       relId[0] -=  PHOS()->GetRawFormatLowGainOffset() ; 
//     }
//     relId[1] = 0 ; 
//     relId[2] = in.GetRow() ; 
//     relId[3] = in.GetColumn() ; 
//     PHOSGeometry()->RelToAbsNumbering(relId, id) ;	
    
//     if (amp > 0 && id >=0 ) {
//       new((*digits)[idigit]) AliPHOSDigit( -1, id, amp, time) ;	
//       idigit++ ; 
//     }
    
//   } // PHOS entries loop
  
//   digits->Sort() ; 
  
//   return digits->GetEntriesFast() ; 
// }
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeD()
{
  // Read the Digits
  
  PhosLoader()->CleanDigits() ;    
  PhosLoader()->LoadDigits("UPDATE") ;
  PhosLoader()->LoadDigitizer("UPDATE") ;
  return Digits()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeH()
{
  // Read the Hits
  PhosLoader()->CleanHits() ;
  // gets TreeH from the root file (PHOS.Hit.root)
  //if ( !IsLoaded("H") ) {
    PhosLoader()->LoadHits("UPDATE") ;
  //  SetLoaded("H") ; 
  //}  
  return Hits()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeR()
{
  // Read the RecPoints
  
  PhosLoader()->CleanRecPoints() ;
  // gets TreeR from the root file (PHOS.RecPoints.root)
  //if ( !IsLoaded("R") ) {
    PhosLoader()->LoadRecPoints("UPDATE") ;
    PhosLoader()->LoadClusterizer("UPDATE") ;
    //  SetLoaded("R") ; 
    //}

  return EmcRecPoints()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeT()
{
  // Read the TrackSegments
  
  PhosLoader()->CleanTracks() ; 
  // gets TreeT from the root file (PHOS.TrackSegments.root)
  //if ( !IsLoaded("T") ) {
    PhosLoader()->LoadTracks("UPDATE") ;
    PhosLoader()->LoadTrackSegmentMaker("UPDATE") ;
    //    SetLoaded("T") ; 
    //}

  return TrackSegments()->GetEntries() ; 
}
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeP()
{
  // Read the RecParticles
  
  PhosLoader()->CleanRecParticles() ; 

  // gets TreeT from the root file (PHOS.TrackSegments.root)
  //  if ( !IsLoaded("P") ) {
    PhosLoader()->LoadRecParticles("UPDATE") ;
    PhosLoader()->LoadPID("UPDATE") ;
    //  SetLoaded("P") ; 
    //}

  return RecParticles()->GetEntries() ; 
}
//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeS()
{
  // Read the SDigits
  
  PhosLoader()->CleanSDigits() ; 
  // gets TreeS from the root file (PHOS.SDigits.root)
  //if ( !IsLoaded("S") ) {
    PhosLoader()->LoadSDigits("READ") ;
    PhosLoader()->LoadSDigitizer("READ") ;
    //  SetLoaded("S") ; 
    //}

  return SDigits()->GetEntries() ; 
}

//____________________________________________________________________________ 
Int_t AliPHOSGetter::ReadTreeE(Int_t event)
{
  // Read the ESD
  
  // gets esdTree from the root file (AliESDs.root)
  if (!fESDFile)
    if ( !OpenESDFile() ) 
      return -1 ; 

  fESDTree = static_cast<TTree*>(fESDFile->Get("esdTree")) ; 
  fESD = new AliESD;
   if (!fESDTree) {

     Error("ReadTreeE", "no ESD tree found");
     return -1;
   }
   fESDTree->SetBranchAddress("ESD", &fESD);
   fESDTree->GetEvent(event);

   return event ; 
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
AliPHOSSDigitizer * AliPHOSGetter::SDigitizer()
{ 
  // Returns pointer to the SDigitizer task 
  AliPHOSSDigitizer * rv ; 
  rv =  dynamic_cast<AliPHOSSDigitizer *>(PhosLoader()->SDigitizer()) ;
  if (!rv) {
    Event(0, "S") ; 
    rv =  dynamic_cast<AliPHOSSDigitizer *>(PhosLoader()->SDigitizer()) ;
  }
  return rv ; 
}

//____________________________________________________________________________ 
TParticle * AliPHOSGetter::Secondary(const TParticle* p, Int_t index) const
{
  // Return first (index=1) or second (index=2) secondary particle of primary particle p 

  if(index <= 0) 
    return 0 ;
  if(index > 2)
    return 0 ;

  if(p) {
  Int_t daughterIndex = p->GetDaughter(index-1) ; 
  AliRunLoader * rl = AliRunLoader::GetRunLoader(PhosLoader()->GetTitle());
  return  rl->GetAliRun()->GetMCApp()->Particle(daughterIndex) ; 
  }
  else
    return 0 ;
}

//____________________________________________________________________________ 
void AliPHOSGetter::Track(Int_t itrack) 
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
  // Returns pointer to the Digits Tree
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
  // Returns pointer to the Hits Tree
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
  // Returns pointer to the RecPoints Tree
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
  // Returns pointer to the TrackSegments Tree
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
  // Returns pointer to the RecParticles  Tree
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
 // Returns pointer to the SDigits Tree
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
    if (version != AliConfig::GetDefaultEventFolderName()) // only if not the default folder name 
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
    if (version != AliConfig::GetDefaultEventFolderName()) // only if not the default folder name 
      fileName = fileName.ReplaceAll(".root", "") + "_" + version + ".root" ;
    if ( !(gSystem->AccessPathName(fileName)) ) {
      Warning("VersionExists", "The file %s already exists", fileName.Data()) ;  
      rv = kTRUE ; 
    }
  }

  return rv ;

}

//____________________________________________________________________________ 
UShort_t AliPHOSGetter::EventPattern(void) const
{
  // Return the pattern (trigger bit register) of the beam-test event
  if(fBTE)
    return fBTE->GetPattern() ;
  else
    return 0 ;
}
//____________________________________________________________________________ 
Float_t AliPHOSGetter::BeamEnergy(void) const
{
  // Return the beam energy of the beam-test event
  if(fBTE)
    return fBTE->GetBeamEnergy() ;
  else
    return 0 ;
}
//____________________________________________________________________________ 

AliPHOSCalibData* AliPHOSGetter::CalibData()
{ 
  // Check if the instance of AliPHOSCalibData exists, and return it

  if( !(AliCDBManager::Instance()->IsDefaultStorageSet()) )
    fCalibData=0x0;
  return fCalibData;
}
//____________________________________________________________________________ 

AliPHOSAlignData* AliPHOSGetter::AlignData()
{ 
  // Check if the instance of AliPHOSAlignData exists, and return it

  if( !(AliCDBManager::Instance()->IsDefaultStorageSet()) )
    fAlignData=0x0;
  return fAlignData;
}

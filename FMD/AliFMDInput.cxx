/**************************************************************************
 * Copyright(c) 2004, ALICE Experiment at CERN, All rights reserved. *
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
/** @file    AliFMDInput.cxx
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 12:42:40 2006
    @brief   FMD utility classes for reading FMD data
*/
//___________________________________________________________________
//
// The classes defined here, are utility classes for reading in data
// for the FMD.  They are  put in a seperate library to not polute the
// normal libraries.  The classes are intended to be used as base
// classes for customized class that do some sort of analysis on the
// various types of data produced by the FMD. 
//
// Latest changes by Christian Holm Christensen
//
#include "AliFMDInput.h"	// ALIFMDHIT_H
#include "AliFMDDebug.h"	// ALIFMDDEBUG_H ALILOG_H
#include "AliLoader.h"          // ALILOADER_H
#include "AliRunLoader.h"       // ALIRUNLOADER_H
#include "AliRun.h"             // ALIRUN_H
#include "AliStack.h"           // ALISTACK_H
#include "AliRawReaderFile.h"   // ALIRAWREADERFILE_H
#include "AliFMD.h"             // ALIFMD_H
#include "AliFMDHit.h"		// ALIFMDHIT_H
#include "AliFMDDigit.h"	// ALIFMDDigit_H
#include "AliFMDSDigit.h"	// ALIFMDDigit_H
#include "AliFMDRecPoint.h"	// ALIFMDRECPOINT_H
#include "AliFMDRawReader.h"    // ALIFMDRAWREADER_H
#include <AliESD.h>
#include <AliESDFMD.h>
#include <AliESDEvent.h>
#include <AliCDBManager.h>
#include <AliCDBEntry.h>
#include <AliAlignObjParams.h>
#include <TTree.h>              // ROOT_TTree
#include <TChain.h>             // ROOT_TChain
#include <TParticle.h>          // ROOT_TParticle
#include <TString.h>            // ROOT_TString
#include <TDatabasePDG.h>       // ROOT_TDatabasePDG
#include <TMath.h>              // ROOT_TMath
#include <TGeoManager.h>        // ROOT_TGeoManager 
#include <TSystemDirectory.h>   // ROOT_TSystemDirectory
#include <Riostream.h>		// ROOT_Riostream
#include <TFile.h>              // ROOT_TFile
#include <TStreamerInfo.h>
#include <TArrayF.h>

//____________________________________________________________________
ClassImp(AliFMDInput)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif


//____________________________________________________________________
AliFMDInput::AliFMDInput()
  : fGAliceFile(""), 
    fLoader(0),
    fRun(0), 
    fStack(0),
    fFMDLoader(0), 
    fReader(0),
    fFMD(0),
    fESD(0),
    fESDEvent(0),
    fTreeE(0),
    fTreeH(0),
    fTreeD(0),
    fTreeS(0),
    fTreeR(0), 
    fTreeA(0), 
    fChainE(0),
    fArrayE(0),
    fArrayH(0),
    fArrayD(0),
    fArrayS(0), 
    fArrayR(0), 
    fArrayA(0), 
    fGeoManager(0),
    fTreeMask(0), 
    fIsInit(kFALSE)
{

  // Constructor of an FMD input object.  Specify what data to read in
  // using the AddLoad member function.   Sub-classes should at a
  // minimum overload the member function Event.   A full job can be
  // executed using the member function Run. 
}

  

//____________________________________________________________________
AliFMDInput::AliFMDInput(const char* gAliceFile)
  : fGAliceFile(gAliceFile),
    fLoader(0),
    fRun(0), 
    fStack(0),
    fFMDLoader(0), 
    fReader(0),
    fFMD(0),
    fESD(0),
    fESDEvent(0),
    fTreeE(0),
    fTreeH(0),
    fTreeD(0),
    fTreeS(0),
    fTreeR(0), 
    fTreeA(0), 
    fChainE(0),
    fArrayE(0),
    fArrayH(0),
    fArrayD(0),
    fArrayS(0), 
    fArrayR(0), 
    fArrayA(0), 
    fGeoManager(0),
    fTreeMask(0), 
    fIsInit(kFALSE)
{
  
  // Constructor of an FMD input object.  Specify what data to read in
  // using the AddLoad member function.   Sub-classes should at a
  // minimum overload the member function Event.   A full job can be
  // executed using the member function Run. 
}

//____________________________________________________________________
Int_t
AliFMDInput::NEvents() const 
{
  // Get number of events
  if (fTreeE) return fTreeE->GetEntries();
  return -1;
}

//____________________________________________________________________
Bool_t
AliFMDInput::Init()
{
  // Initialize the object.  Get the needed loaders, and such. 

  // Check if we have been initialized
  if (fIsInit) { 
    AliWarning("Already initialized");
    return fIsInit;
  }
  if (fGAliceFile.IsNull()) fGAliceFile = "galice.root";
  // Get the loader
  fLoader = AliRunLoader::Open(fGAliceFile.Data(), "Alice", "read");
  if (!fLoader) {
    AliError(Form("Coulnd't read the file %s", fGAliceFile.Data()));
    return kFALSE;
  }
  
  // Get the run 
  if  (fLoader->LoadgAlice()) return kFALSE;
  fRun = fLoader->GetAliRun();
  
  // Get the FMD 
  fFMD = static_cast<AliFMD*>(fRun->GetDetector("FMD"));
  if (!fFMD) {
    AliError("Failed to get detector FMD from loader");
    return kFALSE;
  }
  
  // Get the FMD loader
  fFMDLoader = fLoader->GetLoader("FMDLoader");
  if (!fFMDLoader) {
    AliError("Failed to get detector FMD loader from loader");
    return kFALSE;
  }
  if (fLoader->LoadHeader()) { 
    AliError("Failed to get event header information from loader");
    return kFALSE;
  }
  fTreeE = fLoader->TreeE();

  // Optionally, get the ESD files
  if (TESTBIT(fTreeMask, kESD)) {
    fChainE = new TChain("esdTree");
    TSystemDirectory dir(".",".");
    TList*           files = dir.GetListOfFiles();
    TSystemFile*     file = 0;
    if (!files) {
      AliError("No files");
      return kFALSE;
    }
    files->Sort();
    TIter            next(files);
    while ((file = static_cast<TSystemFile*>(next()))) {
      TString fname(file->GetName());
      if (fname.Contains("AliESDs")) fChainE->AddFile(fname.Data());
    }
    fESDEvent = new AliESDEvent();
    fESDEvent->ReadFromTree(fChainE);
    //    fChainE->SetBranchAddress("ESD", &fMainESD);
    
  }
    
  if (TESTBIT(fTreeMask, kRaw)) {
    AliInfo("Getting FMD raw data digits");
    fArrayA = new TClonesArray("AliFMDDigit");
    fReader = new AliRawReaderFile(-1);
  }
  
  // Optionally, get the geometry 
  if (TESTBIT(fTreeMask, kGeometry)) {
    TString fname(fRun->GetGeometryFileName());
    if (fname.IsNull()) {
      Warning("Init", "No file name for the geometry from AliRun");
      fname = gSystem->DirName(fGAliceFile);
      fname.Append("/geometry.root");
    }
    fGeoManager = TGeoManager::Import(fname.Data());
    if (!fGeoManager) {
      Fatal("Init", "No geometry manager found");
      return kFALSE;
    }
    AliCDBManager* cdb   = AliCDBManager::Instance();
    AliCDBEntry*   align = cdb->Get("FMD/Align/Data");
    if (align) {
      AliInfo("Got alignment data from CDB");
      TClonesArray* array = dynamic_cast<TClonesArray*>(align->GetObject());
      if (!array) {
	AliWarning("Invalid align data from CDB");
      }
      else {
	Int_t nAlign = array->GetEntries();
	for (Int_t i = 0; i < nAlign; i++) {
	  AliAlignObjParams* a = static_cast<AliAlignObjParams*>(array->At(i));
	  if (!a->ApplyToGeometry()) {
	    AliWarning(Form("Failed to apply alignment to %s", 
			    a->GetSymName()));
	  }
	}
      }
    }
  }

  fEventCount = 0;
  fIsInit = kTRUE;
  return fIsInit;
}

//____________________________________________________________________
Bool_t
AliFMDInput::Begin(Int_t event)
{
  // Called at the begining of each event.  Per default, it gets the
  // data trees and gets pointers to the output arrays.   Users can
  // overload this, but should call this member function in the
  // overloaded member function of the derived class. 

  // Check if we have been initialized
  if (!fIsInit) { 
    AliError("Not initialized");
    return fIsInit;
  }

  // Get the event 
  if (fLoader->GetEvent(event)) return kFALSE;
  AliInfo(Form("Now in event %8d/%8d", event, NEvents()));

  // Possibly load global kinematics information 
  if (TESTBIT(fTreeMask, kKinematics) || TESTBIT(fTreeMask, kTracks)) {
    // AliInfo("Getting kinematics");
    if (fLoader->LoadKinematics()) return kFALSE;
    fStack = fLoader->Stack();
  }

  // Possibly load FMD Hit information 
  if (TESTBIT(fTreeMask, kHits) || TESTBIT(fTreeMask, kTracks)) {
    // AliInfo("Getting FMD hits");
    if (!fFMDLoader || fFMDLoader->LoadHits()) return kFALSE;
    fTreeH = fFMDLoader->TreeH();
    if (!fArrayH) fArrayH = fFMD->Hits(); 
  }

  // Possibly load FMD Digit information 
  if (TESTBIT(fTreeMask, kDigits)) {
    // AliInfo("Getting FMD digits");
    if (!fFMDLoader || fFMDLoader->LoadDigits()) return kFALSE;
    fTreeD = fFMDLoader->TreeD();
    if (fTreeD) {
      if (!fArrayD) fArrayD = fFMD->Digits();
    }
    else {
      fArrayD = 0;
      AliWarning(Form("Failed to load FMD Digits"));
    } 
  }

  // Possibly load FMD Sdigit information 
  if (TESTBIT(fTreeMask, kSDigits)) {
    // AliInfo("Getting FMD summable digits");
    if (!fFMDLoader || fFMDLoader->LoadSDigits()) return kFALSE;
    fTreeS = fFMDLoader->TreeS();
    if (!fArrayS) fArrayS = fFMD->SDigits();
  }

  // Possibly load FMD RecPoints information 
  if (TESTBIT(fTreeMask, kRecPoints)) {
    // AliInfo("Getting FMD reconstructed points");
    if (!fFMDLoader || fFMDLoader->LoadRecPoints()) return kFALSE;
    fTreeR = fFMDLoader->TreeR();
    if (!fArrayR) fArrayR = new TClonesArray("AliFMDRecPoint");
    fTreeR->SetBranchAddress("FMD",  &fArrayR);
  }  

  // Possibly load FMD ESD information 
  if (TESTBIT(fTreeMask, kESD)) {
    // AliInfo("Getting FMD event summary data");
    Int_t read = fChainE->GetEntry(event);
    if (read <= 0) return kFALSE;
    fESD = fESDEvent->GetFMDData();
    if (!fESD) return kFALSE;
#if 0
    TFile* f = fChainE->GetFile();
    if (f) {
      TObject* o = f->GetStreamerInfoList()->FindObject("AliFMDMap");
      if (o) {
	TStreamerInfo* info = static_cast<TStreamerInfo*>(o);
	std::cout << "AliFMDMap class version read is " 
		  <<  info->GetClassVersion() << std::endl;
      }
    }
    // fESD->CheckNeedUShort(fChainE->GetFile());
#endif
  }

  // Possibly load FMD Digit information 
  if (TESTBIT(fTreeMask, kRaw)) {
    // AliInfo("Getting FMD raw data digits");
    if (!fReader->NextEvent()) return kFALSE;
    AliFMDRawReader r(fReader, 0);
    fArrayA->Clear();
    r.ReadAdcs(fArrayA);
  }
  fEventCount++;
  return kTRUE;
}


//____________________________________________________________________
Bool_t 
AliFMDInput::Event()
{
  // Process one event.  The default implementation one or more of 
  //
  //   -  ProcessHits       if the hits are loaded. 
  //   -  ProcessDigits     if the digits are loaded. 
  //   -  ProcessSDigits    if the sumbable digits are loaded. 
  //   -  ProcessRecPoints  if the reconstructed points are loaded. 
  //   -  ProcessESD        if the event summary data is loaded
  // 
  if (TESTBIT(fTreeMask, kHits)) 
    if (!ProcessHits()) return kFALSE; 
  if (TESTBIT(fTreeMask, kTracks)) 
    if (!ProcessTracks()) return kFALSE; 
  if (TESTBIT(fTreeMask, kDigits)) 
    if (!ProcessDigits()) return kFALSE;
  if (TESTBIT(fTreeMask, kSDigits)) 
    if (!ProcessSDigits()) return kFALSE;
  if (TESTBIT(fTreeMask, kRaw)) 
    if (!ProcessRawDigits()) return kFALSE;
  if (TESTBIT(fTreeMask, kRecPoints)) 
    if (!ProcessRecPoints()) return kFALSE;
  if (TESTBIT(fTreeMask, kESD))
    if (!ProcessESDs()) return kFALSE;
  
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessHits()
{
  // Read the hit tree, and pass each hit to the member function
  // ProcessHit.
  if (!fTreeH) {
    AliError("No hit tree defined");
    return kFALSE;
  }
  Int_t nTracks = fTreeH->GetEntries();
  for (Int_t i = 0; i < nTracks; i++) {
    Int_t hitRead  = fTreeH->GetEntry(i);
    if (hitRead <= 0) continue;
    if (!fArrayH) {
      AliError("No hit array defined");
      return kFALSE;
    }
    Int_t nHit = fArrayH->GetEntries();
    if (nHit <= 0) continue;
    for (Int_t j = 0; j < nHit; j++) {
      AliFMDHit* hit = static_cast<AliFMDHit*>(fArrayH->At(j));
      if (!hit) continue;
      TParticle* track = 0;
      if (TESTBIT(fTreeMask, kKinematics) && fStack) {
	Int_t trackno = hit->Track();
	track = fStack->Particle(trackno);
      }
      if (!ProcessHit(hit, track)) return kFALSE;
    }    
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessTracks()
{
  // Read the hit tree, and pass each hit to the member function
  // ProcessHit.
  if (!fStack) {
    AliError("No track tree defined");
    return kFALSE;
  }
  if (!fTreeH) {
    AliError("No hit tree defined");
    return kFALSE;
  }
  Int_t nTracks = fTreeH->GetEntries();
  for (Int_t i = 0; i < nTracks; i++) {
    TParticle* track = fStack->Particle(i);
    if (!track) continue;
    Int_t hitRead  = fTreeH->GetEntry(i);
    if (hitRead <= 0) continue;
    if (!fArrayH) {
      AliError("No hit array defined");
      return kFALSE;
    }
    Int_t nHit = fArrayH->GetEntries();
    if (nHit <= 0) continue;

    for (Int_t j = 0; j < nHit; j++) {
      AliFMDHit* hit = static_cast<AliFMDHit*>(fArrayH->At(j));
      if (!hit) continue;
      if (!ProcessTrack(i, track, hit)) return kFALSE;
    }    
    // if (!ProcessTrack(i, track, fArrayH)) return kFALSE;
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessDigits()
{
  // Read the digit tree, and pass each digit to the member function
  // ProcessDigit.
  Int_t nEv = fTreeD->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t digitRead  = fTreeD->GetEntry(i);
    if (digitRead <= 0) continue;
    Int_t nDigit = fArrayD->GetEntries();
    if (nDigit <= 0) continue;
    for (Int_t j = 0; j < nDigit; j++) {
      AliFMDDigit* digit = static_cast<AliFMDDigit*>(fArrayD->At(j));
      if (!digit) continue;
      if (!ProcessDigit(digit)) return kFALSE;
    }    
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessSDigits()
{
  // Read the summable digit tree, and pass each sumable digit to the
  // member function ProcessSdigit.
  Int_t nEv = fTreeD->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t sdigitRead  = fTreeS->GetEntry(i);
    if (sdigitRead <= 0) continue;
    Int_t nSdigit = fArrayS->GetEntries();
    if (nSdigit <= 0) continue;
    for (Int_t j = 0; j < nSdigit; j++) {
      AliFMDSDigit* sdigit = static_cast<AliFMDSDigit*>(fArrayS->At(j));
      if (!sdigit) continue;
      if (!ProcessSDigit(sdigit)) return kFALSE;
    }    
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessRawDigits()
{
  // Read the digit tree, and pass each digit to the member function
  // ProcessDigit.
  Int_t nDigit = fArrayA->GetEntries();
  if (nDigit <= 0) return kTRUE;
  for (Int_t j = 0; j < nDigit; j++) {
    AliFMDDigit* digit = static_cast<AliFMDDigit*>(fArrayA->At(j));
    if (!digit) continue;
    if (!ProcessRawDigit(digit)) return kFALSE;
  }    
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessRecPoints()
{
  // Read the reconstrcted points tree, and pass each reconstruction
  // object (AliFMDRecPoint) to either ProcessRecPoint.
  Int_t nEv = fTreeR->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t recRead  = fTreeR->GetEntry(i);
    if (recRead <= 0) continue;
    Int_t nRecPoint = fArrayR->GetEntries();
    for (Int_t j = 0; j < nRecPoint; j++) {
      AliFMDRecPoint* recPoint = static_cast<AliFMDRecPoint*>(fArrayR->At(j));
      if (!recPoint) continue;
      if (!ProcessRecPoint(recPoint)) return kFALSE;
    }    
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessESDs()
{
  // Process event summary data
  if (!fESD) return kFALSE;
  for (UShort_t det = 1; det <= 3; det++) {
    Char_t rings[] = { 'I', (det == 1 ? '\0' : 'O'), '\0' };
    for (Char_t* rng = rings; *rng != '\0'; rng++) {
      UShort_t nsec = (*rng == 'I' ?  20 :  40);
      UShort_t nstr = (*rng == 'I' ? 512 : 256);
      for (UShort_t sec = 0; sec < nsec; sec++) {
	for (UShort_t str = 0; str < nstr; str++) {
	  Float_t eta  = fESD->Eta(det,*rng,sec,str);
	  Float_t mult = fESD->Multiplicity(det,*rng,sec,str);
	  if (!fESD->IsAngleCorrected()) 
	    mult *= TMath::Abs(TMath::Cos(2.*TMath::ATan(TMath::Exp(-eta))));
	  if (!ProcessESD(det, *rng, sec, str, eta, mult)) continue;
	}
      }
    }
  }
  return kTRUE;
}

//____________________________________________________________________
Bool_t
AliFMDInput::End()
{
  // Called at the end of each event.  Per default, it unloads the
  // data trees and resets the pointers to the output arrays.   Users
  // can overload this, but should call this member function in the
  // overloaded member function of the derived class. 

  // Check if we have been initialized
  if (!fIsInit) { 
    AliError("Not initialized");
    return fIsInit;
  }
  // Possibly unload global kinematics information 
  if (TESTBIT(fTreeMask, kKinematics) || TESTBIT(fTreeMask, kTracks)) {
    fLoader->UnloadKinematics();
    // fTreeK = 0;
    fStack = 0;
  }
  // Possibly unload FMD Hit information 
  if (TESTBIT(fTreeMask, kHits) || TESTBIT(fTreeMask, kTracks)) {
    fFMDLoader->UnloadHits();
    fTreeH = 0;
  }
  // Possibly unload FMD Digit information 
  if (TESTBIT(fTreeMask, kDigits)) {
    fFMDLoader->UnloadDigits();
    fTreeD = 0;
  }
  // Possibly unload FMD Sdigit information 
  if (TESTBIT(fTreeMask, kSDigits)) {
    fFMDLoader->UnloadSDigits();
    fTreeS = 0;
  }
  // Possibly unload FMD RecPoints information 
  if (TESTBIT(fTreeMask, kRecPoints)) {
    fFMDLoader->UnloadRecPoints();
    fTreeR = 0;
  }
  // AliInfo("Now out event");
  return kTRUE;
}

//____________________________________________________________________
Bool_t
AliFMDInput::Run()
{
  // Run over all events and files references in galice.root 

  Bool_t retval;
  if (!(retval = Init())) return retval;

  Int_t nEvents = NEvents();
  for (Int_t event = 0; event < nEvents; event++) {
    if (!(retval = Begin(event))) break;
    if (!(retval = Event())) break;
    if (!(retval = End())) break;
  }
  if (!retval) return retval;
  retval = Finish();
  return retval;
}

//__________________________________________________________________
TArrayF 
AliFMDInput::MakeLogScale(Int_t n, Double_t min, Double_t max) 
{
  // Service function to define a logarithmic axis. 
  // Parameters: 
  //   n    Number of bins 
  //   min  Minimum of axis 
  //   max  Maximum of axis 
  TArrayF bins(n+1);
  bins[0]      = min;
  if (n <= 20) {
    for (Int_t i = 1; i < n+1; i++) bins[i] = bins[i-1] + (max-min)/n;
    return bins;
  }
  Float_t dp   = n / TMath::Log10(max / min);
  Float_t pmin = TMath::Log10(min);
  for (Int_t i = 1; i < n+1; i++) {
    Float_t p = pmin + i / dp;
    bins[i]   = TMath::Power(10, p);
  }
  return bins;
}



//____________________________________________________________________
//
// EOF
//

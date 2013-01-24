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
#include "AliRawReaderRoot.h"   // ALIRAWREADERROOT_H
#include "AliRawReaderDate.h"   // ALIRAWREADERDATE_H
#include "AliRawEventHeaderBase.h" 
#include "AliFMD.h"             // ALIFMD_H
#include "AliFMDHit.h"		// ALIFMDHIT_H
#include "AliFMDDigit.h"	// ALIFMDDigit_H
#include "AliFMDSDigit.h"	// ALIFMDDigit_H
#include "AliFMDRecPoint.h"	// ALIFMDRECPOINT_H
#include "AliFMDRawReader.h"    // ALIFMDRAWREADER_H
#include "AliFMDGeometry.h"
#include <AliESD.h>
#include <AliESDFMD.h>
#include <AliESDEvent.h>
#include <AliCDBManager.h>
#include <AliCDBEntry.h>
#include <AliAlignObjParams.h>
#include <AliTrackReference.h>
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
const AliFMDInput::ETrees AliFMDInput::fgkAllLoads[] = { kHits,       
							 kKinematics,
							 kDigits,    
							 kSDigits,   
							 kHeader,    
							 kRecPoints, 
							 kESD,       
							 kRaw,       
							 kGeometry,  
							 kTrackRefs, 
							 kRawCalib,  
							 kUser };

//____________________________________________________________________
AliFMDInput::AliFMDInput()
  : TNamed("AliFMDInput", "Input handler for various FMD data"), 
    fGAliceFile(""), 
    fLoader(0),
    fRun(0), 
    fStack(0),
    fFMDLoader(0), 
    fReader(0),
    fFMDReader(0),
    fFMD(0),
    fESD(0),
    fESDEvent(0),
    fTreeE(0),
    fTreeH(0),
    fTreeTR(0),
    fTreeD(0),
    fTreeS(0),
    fTreeR(0), 
    fTreeA(0), 
    fChainE(0),
    fArrayE(0),
    fArrayH(0),
    fArrayTR(0), 
    fArrayD(0),
    fArrayS(0), 
    fArrayR(0), 
    fArrayA(0), 
    fHeader(0),
    fGeoManager(0),
    fTreeMask(0), 
    fRawFile(""),
    fInputDir("."),
    fIsInit(kFALSE),
    fEventCount(0), 
    fNEvents(-1)
{

  // Constructor of an FMD input object.  Specify what data to read in
  // using the AddLoad member function.   Sub-classes should at a
  // minimum overload the member function Event.   A full job can be
  // executed using the member function Run. 
}

  

//____________________________________________________________________
AliFMDInput::AliFMDInput(const char* gAliceFile)
  : TNamed("AliFMDInput", "Input handler for various FMD data"), 
    fGAliceFile(gAliceFile),
    fLoader(0),
    fRun(0), 
    fStack(0),
    fFMDLoader(0), 
    fReader(0),
    fFMDReader(0),
    fFMD(0),
    fESD(0),
    fESDEvent(0),
    fTreeE(0),
    fTreeH(0),
    fTreeTR(0),
    fTreeD(0),
    fTreeS(0),
    fTreeR(0), 
    fTreeA(0), 
    fChainE(0),
    fArrayE(0),
    fArrayH(0),
    fArrayTR(0), 
    fArrayD(0),
    fArrayS(0), 
    fArrayR(0), 
    fArrayA(0), 
    fHeader(0),
    fGeoManager(0),
    fTreeMask(0), 
    fRawFile(""),
    fInputDir("."),
    fIsInit(kFALSE),
    fEventCount(0),
    fNEvents(-1)
{
  
  // Constructor of an FMD input object.  Specify what data to read in
  // using the AddLoad member function.   Sub-classes should at a
  // minimum overload the member function Event.   A full job can be
  // executed using the member function Run. 
}

//____________________________________________________________________
void
AliFMDInput::SetLoads(UInt_t mask)
{
  for (UInt_t i = 0; i < sizeof(mask); i++) { 
    if (!(mask & (1 << i))) continue;
    const ETrees *ptype = fgkAllLoads;
    do { 
      ETrees type = *ptype;
      if (i != UInt_t(type)) continue;
      AddLoad(type);
      break;
    } while (*ptype++ != kUser);
  }
}
     
//____________________________________________________________________
void
AliFMDInput::SetLoads(const char* what) 
{
  TString    l(what);
  TObjArray* ll = l.Tokenize(", ");
  TIter      next(ll);
  TObject*   os = 0;
  while ((os = next())) { 
    ETrees type = ParseLoad(os->GetName());
    AddLoad(type);
  }
  delete ll;
}
    

//____________________________________________________________________
AliFMDInput::ETrees
AliFMDInput::ParseLoad(const char* what)
{
  TString opt(what);
  opt.ToLower();
  const ETrees* ptype = fgkAllLoads;
  do { 
    ETrees  type = *ptype;
    if (opt.Contains(TreeName(type,true), TString::kIgnoreCase)) 
      return type;
  } while (*ptype++ != kUser);
  return kUser;
}
//____________________________________________________________________
const char*
AliFMDInput::LoadedString(Bool_t dataOnly) const
{
  static TString ret;
  if (!ret.IsNull()) return ret.Data();

  const ETrees* ptype = fgkAllLoads;
  do { 
    ETrees type = *ptype;
    if (dataOnly && 
	(type == kKinematics || 
	 type == kHeader     || 
	 type == kGeometry   || 
	 type == kTrackRefs)) continue;
    if (!IsLoaded(*ptype)) continue;
    
    if (!ret.IsNull()) ret.Append(",");
    ret.Append(TreeName(type));
  } while (*ptype++ != kUser);
  return ret.Data();
}

//____________________________________________________________________
const char* 
AliFMDInput::TreeName(ETrees tree, Bool_t shortest) 
{
  if (shortest) {
    switch (tree) {
    case kHits:		return "hit";        
    case kKinematics:	return "kin";
    case kDigits:	return "dig";     
    case kSDigits:	return "sdig";    
    case kHeader:	return "hea";     
    case kRecPoints:	return "recp";  
    case kESD:		return "esd";        
    case kRaw:		return "raw";        
    case kGeometry:	return "geo";   
    case kTrackRefs:	return "trackr";  
    case kRawCalib:	return "rawc";   
    case kUser:		return "user"; 
    }
    return 0;
  }
  switch (tree) {
  case kHits:		return "Hits";        
  case kKinematics:	return "Kinematics"; 
  case kDigits:		return "Digits";     
  case kSDigits:	return "SDigits";    
  case kHeader:		return "Header";     
  case kRecPoints:	return "RecPoints";  
  case kESD:		return "ESD";        
  case kRaw:		return "Raw";        
  case kGeometry:	return "Geometry";   
  case kTrackRefs:	return "TrackRefs";  
  case kRawCalib:	return "RawCalib";   
  case kUser:		return "User"; 
  }
  return 0;
}

//____________________________________________________________________
Int_t
AliFMDInput::NEvents() const 
{
  // Get number of events
  if (IsLoaded(kRaw) || 
      IsLoaded(kRawCalib)) return fReader->GetNumberOfEvents();
  if (fChainE) return fChainE->GetEntriesFast();
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
  TString what;
  const ETrees* ptype = fgkAllLoads;
  do { 
    ETrees type = *ptype;
    what.Append(Form("\n\t%-20s: %s", TreeName(type), 
		     IsLoaded(type) ? "yes" : "no"));
  } while (*ptype++ != kUser);
  
  Info("Init","Initialising w/mask 0x%04x%s", fTreeMask, what.Data());
  // Get the run 
  if (IsLoaded(kDigits)     ||
      IsLoaded(kSDigits)    || 
      IsLoaded(kKinematics) || 
      IsLoaded(kTrackRefs)  || 
      IsLoaded(kHeader)) {
    if (!gSystem->FindFile(".:/", fGAliceFile)) {
      AliWarning(Form("Cannot find file %s in .:/", fGAliceFile.Data()));
    }
    else {
      fLoader = AliRunLoader::Open(fGAliceFile.Data(), "Alice", "read");
      if (!fLoader) {
	AliError(Form("Coulnd't read the file %s", fGAliceFile.Data()));
	return kFALSE;
      }
      AliInfo(Form("Opened GAlice file %s", fGAliceFile.Data()));

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
    }
  }

  // Optionally, get the ESD files
  if (IsLoaded(kESD)) {
    fChainE = MakeChain("ESD", fInputDir, true);
    fESDEvent = new AliESDEvent();
    fESDEvent->ReadFromTree(fChainE);
    //    fChainE->SetBranchAddress("ESD", &fMainESD);
    
  }
    
  if (IsLoaded(kRaw) || 
      IsLoaded(kRawCalib)) {
    AliInfo("Getting FMD raw data digits");
    fArrayA = new TClonesArray("AliFMDDigit");
#if 0
    if (!fRawFile.IsNull() && fRawFile.EndsWith(".root"))
      fReader = new AliRawReaderRoot(fRawFile.Data());
    else if (!fRawFile.IsNull() && fRawFile.EndsWith(".raw"))
      fReader = new AliRawReaderDate(fRawFile.Data());
    else
      fReader = new AliRawReaderFile(-1);
#else
    if(!fRawFile.IsNull()) 
      fReader = AliRawReader::Create(fRawFile.Data());
    else 
      fReader = new AliRawReaderFile(-1);
#endif
    fFMDReader = new AliFMDRawReader(fReader, 0);
  }
  
  // Optionally, get the geometry 
  if (IsLoaded(kGeometry)) {
    TString fname;
    if (fRun) {
      fname = gSystem->DirName(fGAliceFile);
      fname.Append("/geometry.root");
    }
    if (!gSystem->AccessPathName(fname.Data())) 
      fname = "";
    AliCDBManager* cdb   = AliCDBManager::Instance();
    if (!cdb->IsDefaultStorageSet()) {
      cdb->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
      cdb->SetRun(0);
    }

    AliGeomManager::LoadGeometry(fname.IsNull() ? 0 : fname.Data());

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
    AliFMDGeometry* geom = AliFMDGeometry::Instance();
    geom->Init();
    geom->InitTransformations();
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
  if (fLoader && fLoader->GetEvent(event)) return kFALSE;

  // Possibly load global kinematics information 
  if (IsLoaded(kKinematics)) {
    // AliInfo("Getting kinematics");
    if (fLoader->LoadKinematics("READ")) return kFALSE;
    fStack = fLoader->Stack();
  }

  // Possibly load FMD Hit information 
  if (IsLoaded(kHits)) {
    // AliInfo("Getting FMD hits");
    if (!fFMDLoader || fFMDLoader->LoadHits("READ")) return kFALSE;
    fTreeH = fFMDLoader->TreeH();
    if (!fArrayH) fArrayH = fFMD->Hits(); 
  }
  
  // Possibly load FMD TrackReference information 
  if (IsLoaded(kTrackRefs)) {
    // AliInfo("Getting FMD hits");
    if (!fLoader || fLoader->LoadTrackRefs("READ")) return kFALSE;
    fTreeTR = fLoader->TreeTR();
    if (!fArrayTR) fArrayTR = new TClonesArray("AliTrackReference");
    fTreeTR->SetBranchAddress("TrackReferences",  &fArrayTR);
  }
  
  // Possibly load heaedr information 
  if (IsLoaded(kHeader)) {
    // AliInfo("Getting FMD hits");
    if (!fLoader /* || fLoader->LoadHeader()*/) return kFALSE;
    fHeader = fLoader->GetHeader();
  }

  // Possibly load FMD Digit information 
  if (IsLoaded(kDigits)) {
    // AliInfo("Getting FMD digits");
    if (!fFMDLoader || fFMDLoader->LoadDigits("READ")) return kFALSE;
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
  if (IsLoaded(kSDigits)) {
    // AliInfo("Getting FMD summable digits");
    if (!fFMDLoader || fFMDLoader->LoadSDigits("READ")) { 
      AliWarning("Failed to load SDigits!");
      return kFALSE;
    }
    fTreeS = fFMDLoader->TreeS();
    if (!fArrayS) fArrayS = fFMD->SDigits();
  }

  // Possibly load FMD RecPoints information 
  if (IsLoaded(kRecPoints)) {
    // AliInfo("Getting FMD reconstructed points");
    if (!fFMDLoader || fFMDLoader->LoadRecPoints("READ")) return kFALSE;
    fTreeR = fFMDLoader->TreeR();
    if (!fArrayR) fArrayR = new TClonesArray("AliFMDRecPoint");
    fTreeR->SetBranchAddress("FMD",  &fArrayR);
  }  

  // Possibly load FMD ESD information 
  if (IsLoaded(kESD)) {
    // AliInfo("Getting FMD event summary data");
    Int_t read = fChainE->GetEntry(event);
    if (read <= 0) return kFALSE;
    fESD = fESDEvent->GetFMDData();
    if (!fESD) return kFALSE;
  }

  // Possibly load FMD Digit information 
  if (IsLoaded(kRaw) || IsLoaded(kRawCalib)) {
    Bool_t mon = fRawFile.Contains("mem://");
    // AliInfo("Getting FMD raw data digits");
    if (mon) std::cout << "Waiting for event ..." << std::flush;
    do { 
      if (!fReader->NextEvent()) { 
	if (mon) { 
	  gSystem->Sleep(3);
	  continue;
	}
	return kFALSE;
      }
      UInt_t eventType = fReader->GetType();
      if(eventType == AliRawEventHeaderBase::kPhysicsEvent ||
	 eventType == AliRawEventHeaderBase::kCalibrationEvent) 
	break;
    } while (true);
    if (mon) std::cout << "got it" << std::endl;
    // AliFMDRawReader r(fReader, 0);
    fArrayA->Clear();
    fFMDReader->ReadAdcs(fArrayA);
    AliFMDDebug(1, ("Got a total of %d digits", fArrayA->GetEntriesFast()));
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
  if (IsLoaded(kHits))     if (!ProcessHits())      return kFALSE; 
  if (IsLoaded(kTrackRefs))if (!ProcessTrackRefs()) return kFALSE; 
  if (IsLoaded(kKinematics) && 
      IsLoaded(kHits))     if (!ProcessTracks())    return kFALSE; 
  if (IsLoaded(kKinematics))if (!ProcessStack())     return kFALSE; 
  if (IsLoaded(kSDigits))  if (!ProcessSDigits())   return kFALSE;
  if (IsLoaded(kDigits))   if (!ProcessDigits())    return kFALSE;
  if (IsLoaded(kRaw))      if (!ProcessRawDigits()) return kFALSE;
  if (IsLoaded(kRawCalib)) if (!ProcessRawCalibDigits())return kFALSE;
  if (IsLoaded(kRecPoints))if (!ProcessRecPoints()) return kFALSE;
  if (IsLoaded(kESD))      if (!ProcessESDs())      return kFALSE;
  if (IsLoaded(kUser))     if (!ProcessUsers())     return kFALSE;
  
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
  if (!fArrayH) {
    AliError("No hit array defined");
    return kFALSE;
  }

  Int_t nTracks = fTreeH->GetEntries();
  for (Int_t i = 0; i < nTracks; i++) {
    Int_t hitRead  = fTreeH->GetEntry(i);
    if (hitRead <= 0) continue;

    Int_t nHit = fArrayH->GetEntries();
    if (nHit <= 0) continue;
  
    for (Int_t j = 0; j < nHit; j++) {
      AliFMDHit* hit = static_cast<AliFMDHit*>(fArrayH->At(j));
      if (!hit) continue;

      TParticle* track = 0;
      if (IsLoaded(kKinematics) && fStack) {
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
AliFMDInput::ProcessTrackRefs()
{
  // Read the reconstrcted points tree, and pass each reconstruction
  // object (AliFMDRecPoint) to either ProcessRecPoint.
  if (!fTreeTR) {
    AliError("No track reference tree defined");
    return kFALSE;
  }
  if (!fArrayTR) {
    AliError("No track reference array defined");
    return kFALSE;
  }

  Int_t nEv = fTreeTR->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t trRead  = fTreeTR->GetEntry(i);
    if (trRead <= 0) continue;
    Int_t nTrackRefs = fArrayTR->GetEntries();
    for (Int_t j = 0; j < nTrackRefs; j++) {
      AliTrackReference* trackRef = 
	static_cast<AliTrackReference*>(fArrayTR->At(j));
      if (!trackRef) continue;
      // if (trackRef->DetectorId() != AliTrackReference::kFMD) continue;
      TParticle* track = 0;
      if (IsLoaded(kKinematics) && fStack) {
	Int_t trackno = trackRef->GetTrack();
	track = fStack->Particle(trackno);
      }
      if (!ProcessTrackRef(trackRef,track)) return kFALSE;
    }    
  }
  return kTRUE;
}
//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessTracks()
{
  // Read the hit tree, and pass each hit to the member function
  // ProcessTrack.
  if (!fStack) {
    AliError("No track tree defined");
    return kFALSE;
  }
  if (!fTreeH) {
    AliError("No hit tree defined");
    return kFALSE;
  }
  if (!fArrayH) {
    AliError("No hit array defined");
    return kFALSE;
  }

  // Int_t nTracks = fStack->GetNtrack();
  Int_t nTracks = fTreeH->GetEntries();
  for (Int_t i = 0; i < nTracks; i++) {
    Int_t      trackno = nTracks - i - 1;
    TParticle* track   = fStack->Particle(trackno);
    if (!track) continue;

    // Get the hits for this track. 
    Int_t hitRead  = fTreeH->GetEntry(i);
    Int_t nHit     = fArrayH->GetEntries();
    if (nHit == 0 || hitRead <= 0) { 
      // Let user code see the track, even if there's no hits. 
      if (!ProcessTrack(trackno, track, 0)) return kFALSE;
      continue;
    }

    // Loop over the hits corresponding to this track.
    for (Int_t j = 0; j < nHit; j++) {
      AliFMDHit* hit = static_cast<AliFMDHit*>(fArrayH->At(j));
      if (!ProcessTrack(trackno, track, hit)) return kFALSE;
    }   
  }
  return kTRUE;
}
//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessStack()
{
  // Read the hit tree, and pass each hit to the member function
  // ProcessTrack.
  if (!fStack) {
    AliError("No track tree defined");
    return kFALSE;
  }
  Int_t nTracks = fStack->GetNtrack();
  for (Int_t i = 0; i < nTracks; i++) {
    Int_t      trackno = nTracks - i - 1;
    TParticle* track   = fStack->Particle(trackno);
    if (!track) continue;

    if (!ProcessParticle(trackno, track)) return kFALSE;
  }
  return kTRUE;
}
//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessDigits()
{
  // Read the digit tree, and pass each digit to the member function
  // ProcessDigit.
  if (!fTreeD) {
    AliError("No digit tree defined");
    return kFALSE;
  }
  if (!fArrayD) {
    AliError("No digit array defined");
    return kFALSE;
  }

  Int_t nEv = fTreeD->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t digitRead  = fTreeD->GetEntry(i);
    if (digitRead <= 0) continue;
    Int_t nDigit = fArrayD->GetEntries();
    AliFMDDebug(0, ("Got %5d digits for this event", nDigit));
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
  if (!fTreeS) {
    AliWarning("No sdigit tree defined");
    return kTRUE; // Empty SDigits is fine
  }
  if (!fArrayS) {
    AliWarning("No sdigit array defined");
    return kTRUE; // Empty SDigits is fine
  }

  Int_t nEv = fTreeS->GetEntries();
  for (Int_t i = 0; i < nEv; i++) {
    Int_t sdigitRead  = fTreeS->GetEntry(i);
    if (sdigitRead <= 0) { 
      AliInfo(Form("Read nothing from tree"));
      continue;
    }
    Int_t nSdigit = fArrayS->GetEntriesFast();
    AliFMDDebug(0, ("Got %5d digits for this event", nSdigit));
    AliInfo(Form("Got %5d digits for this event", nSdigit));
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
  if (!fArrayA) {
    AliError("No raw digit array defined");
    return kFALSE;
  }

  Int_t nDigit = fArrayA->GetEntries();
  if (nDigit <= 0) return kTRUE;
  for (Int_t j = 0; j < nDigit; j++) {
    AliFMDDigit* digit = static_cast<AliFMDDigit*>(fArrayA->At(j));
    if (!digit) continue;
    if (AliLog::GetDebugLevel("FMD","") >= 40 && j < 30) 
      digit->Print();
    if (!ProcessRawDigit(digit)) return kFALSE;
  }    
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessRawCalibDigits()
{
  // Read the digit tree, and pass each digit to the member function
  // ProcessDigit.
  if (!fArrayA) {
    AliError("No raw digit array defined");
    return kFALSE;
  }

  Int_t nDigit = fArrayA->GetEntries();
  if (nDigit <= 0) return kTRUE;
  for (Int_t j = 0; j < nDigit; j++) {
    AliFMDDigit* digit = static_cast<AliFMDDigit*>(fArrayA->At(j));
    if (!digit) continue;
    if (AliLog::GetDebugLevel("FMD","") >= 40 && j < 30) 
      digit->Print();
    if (!ProcessRawCalibDigit(digit)) return kFALSE;
  }    
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDInput::ProcessRecPoints()
{
  // Read the reconstrcted points tree, and pass each reconstruction
  // object (AliFMDRecPoint) to either ProcessRecPoint.
  if (!fTreeR) {
    AliError("No recpoint tree defined");
    return kFALSE;
  }
  if (!fArrayR) {
    AliError("No recpoints array defined");
    return kFALSE;
  }

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
AliFMDInput::ProcessUsers()
{
  // Process event summary data
  for (UShort_t det = 1; det <= 3; det++) {
    Char_t rings[] = { 'I', (det == 1 ? '\0' : 'O'), '\0' };
    for (Char_t* rng = rings; *rng != '\0'; rng++) {
      UShort_t nsec = (*rng == 'I' ?  20 :  40);
      UShort_t nstr = (*rng == 'I' ? 512 : 256);
      for (UShort_t sec = 0; sec < nsec; sec++) {
	for (UShort_t str = 0; str < nstr; str++) {
	  Float_t v  = GetSignal(det,*rng,sec,str);
	  if (!ProcessUser(det, *rng, sec, str, v)) continue;
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
  if (IsLoaded(kKinematics)) {
    fLoader->UnloadKinematics();
    // fTreeK = 0;
    fStack = 0;
  }
  // Possibly unload FMD Hit information 
  if (IsLoaded(kHits)) {
    fFMDLoader->UnloadHits();
    fTreeH = 0;
  }
  // Possibly unload FMD Digit information 
  if (IsLoaded(kDigits)) {
    fFMDLoader->UnloadDigits();
    fTreeD = 0;
  }
  // Possibly unload FMD Sdigit information 
  if (IsLoaded(kSDigits)) {
    fFMDLoader->UnloadSDigits();
    fTreeS = 0;
  }
  // Possibly unload FMD RecPoints information 
  if (IsLoaded(kRecPoints)) {
    fFMDLoader->UnloadRecPoints();
    fTreeR = 0;
  }
  // AliInfo("Now out event");
  return kTRUE;
}

//____________________________________________________________________
Bool_t
AliFMDInput::Run(UInt_t maxEvents)
{
  // Run over all events and files references in galice.root 

  Bool_t retval;
  if (!(retval = Init())) return retval;

  fNEvents = NEvents();
  if (fNEvents < 0)       fNEvents = maxEvents;
  else if (maxEvents > 0) fNEvents = TMath::Min(fNEvents,Int_t(maxEvents));

  Int_t event = 0;
  for (; fNEvents < 0 || event < fNEvents; event++) {
    printf("\rEvent %8d/%8d ...", event, fNEvents);
    if (!(retval = Begin(event))) break;
    if (!(retval = Event())) break;
    if (!(retval = End())) break;
  }
  printf("Looped over %8d events\n", event+1);
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
void
AliFMDInput::ScanDirectory(TSystemDirectory* dir, 
			   const TString& olddir, 
			   TChain* chain, 
			   const char* pattern, bool recursive)
{
  // Get list of files, and go back to old working directory
  TString oldDir(gSystem->WorkingDirectory());
  TList* files = dir->GetListOfFiles();
  gSystem->ChangeDirectory(oldDir);
  
  // Sort list of files and check if we should add it 
  if (!files) return;
  files->Sort();
  TIter next(files);
  TSystemFile* file = 0;
  while ((file = static_cast<TSystemFile*>(next()))) {
    TString name(file->GetName());
    
    // Ignore special links 
    if (name == "." || name == "..") continue;

    // Check if this is a directory 
    if (file->IsDirectory()) { 
      if (recursive) 
	ScanDirectory(static_cast<TSystemDirectory*>(file),
		      olddir, chain,
		      pattern,recursive);
      continue;
    }
    
    // If this is not a root file, ignore 
    if (!name.EndsWith(".root")) continue;

    // If this file does not contain the pattern, ignore 
    if (!name.Contains(pattern)) continue;
    if (name.Contains("friends")) continue;
    
    // Get the path 
    TString data(Form("%s/%s", file->GetTitle(), name.Data()));

    TFile* test = TFile::Open(data.Data(), "READ");
    if (!test || test->IsZombie()) { 
      ::Warning("ScanDirectory", "Failed to open file %s", data.Data());
      continue;
    }
    test->Close();
    chain->Add(data);
  }
}

//____________________________________________________________________
TChain*
AliFMDInput::MakeChain(const char* what, const char* datadir, bool recursive)
{
  TString w(what);
  w.ToUpper();
  const char* treeName = 0;
  const char* pattern  = 0;
  if      (w.Contains("ESD")) { treeName = "esdTree"; pattern = "AliESD"; }
  else if (w.Contains("MC"))  { treeName = "TE";      pattern = "galice"; }
  else {
    ::Error("MakeChain", "Unknown mode '%s' (not one of ESD, or MC)", what);
    return 0;
  }
    
  // --- Our data chain ----------------------------------------------
  TChain* chain = new TChain(treeName);

  // --- Get list of ESDs --------------------------------------------
  // Open source directory, and make sure we go back to were we were 
  TString oldDir(gSystem->WorkingDirectory());
  TSystemDirectory d(datadir, datadir);
  ScanDirectory(&d, oldDir, chain, pattern, recursive);

  return chain;
}


//____________________________________________________________________
//
// EOF
//

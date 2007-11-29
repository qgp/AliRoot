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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for running the reconstruction                                      //
//                                                                           //
// Clusters and tracks are created for all detectors and all events by       //
// typing:                                                                   //
//                                                                           //
//   AliReconstruction rec;                                                  //
//   rec.Run();                                                              //
//                                                                           //
// The Run method returns kTRUE in case of successful execution.             //
//                                                                           //
// If the input to the reconstruction are not simulated digits but raw data, //
// this can be specified by an argument of the Run method or by the method   //
//                                                                           //
//   rec.SetInput("...");                                                    //
//                                                                           //
// The input formats and the corresponding argument are:                     //
// - DDL raw data files: directory name, ends with "/"                       //
// - raw data root file: root file name, extension ".root"                   //
// - raw data DATE file: DATE file name, any other non-empty string          //
// - MC root files     : empty string, default                               //
//                                                                           //
// By default all events are reconstructed. The reconstruction can be        //
// limited to a range of events by giving the index of the first and the     //
// last event as an argument to the Run method or by calling                 //
//                                                                           //
//   rec.SetEventRange(..., ...);                                            //
//                                                                           //
// The index -1 (default) can be used for the last event to indicate no      //
// upper limit of the event range.                                           //
//                                                                           //
// In case of raw-data reconstruction the user can modify the default        //
// number of events per digits/clusters/tracks file. In case the option      //
// is not used the number is set 1. In case the user provides 0, than        //
// the number of events is equal to the number of events inside the          //
// raw-data file (i.e. one digits/clusters/tracks file):                     //
//                                                                           //
//   rec.SetNumberOfEventsPerFile(...);                                      //
//                                                                           //
//                                                                           //
// The name of the galice file can be changed from the default               //
// "galice.root" by passing it as argument to the AliReconstruction          //
// constructor or by                                                         //
//                                                                           //
//   rec.SetGAliceFile("...");                                               //
//                                                                           //
// The local reconstruction can be switched on or off for individual         //
// detectors by                                                              //
//                                                                           //
//   rec.SetRunLocalReconstruction("...");                                   //
//                                                                           //
// The argument is a (case sensitive) string with the names of the           //
// detectors separated by a space. The special string "ALL" selects all      //
// available detectors. This is the default.                                 //
//                                                                           //
// The reconstruction of the primary vertex position can be switched off by  //
//                                                                           //
//   rec.SetRunVertexFinder(kFALSE);                                         //
//                                                                           //
// The tracking and the creation of ESD tracks can be switched on for        //
// selected detectors by                                                     //
//                                                                           //
//   rec.SetRunTracking("...");                                              //
//                                                                           //
// Uniform/nonuniform field tracking switches (default: uniform field)       //
//                                                                           //
//   rec.SetUniformFieldTracking(); ( rec.SetUniformFieldTracking(kFALSE); ) //
//                                                                           //
// The filling of additional ESD information can be steered by               //
//                                                                           //
//   rec.SetFillESD("...");                                                  //
//                                                                           //
// Again, for both methods the string specifies the list of detectors.       //
// The default is "ALL".                                                     //
//                                                                           //
// The call of the shortcut method                                           //
//                                                                           //
//   rec.SetRunReconstruction("...");                                        //
//                                                                           //
// is equivalent to calling SetRunLocalReconstruction, SetRunTracking and    //
// SetFillESD with the same detector selecting string as argument.           //
//                                                                           //
// The reconstruction requires digits or raw data as input. For the creation //
// of digits and raw data have a look at the class AliSimulation.            //
//                                                                           //
// For debug purposes the method SetCheckPointLevel can be used. If the      //
// argument is greater than 0, files with ESD events will be written after   //
// selected steps of the reconstruction for each event:                      //
//   level 1: after tracking and after filling of ESD (final)                //
//   level 2: in addition after each tracking step                           //
//   level 3: in addition after the filling of ESD for each detector         //
// If a final check point file exists for an event, this event will be       //
// skipped in the reconstruction. The tracking and the filling of ESD for    //
// a detector will be skipped as well, if the corresponding check point      //
// file exists. The ESD event will then be loaded from the file instead.     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TArrayF.h>
#include <TFile.h>
#include <TList.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TPluginManager.h>
#include <TGeoManager.h>
#include <TLorentzVector.h>
#include <TArrayS.h>
#include <TArrayD.h>

#include "AliReconstruction.h"
#include "AliCodeTimer.h"
#include "AliReconstructor.h"
#include "AliLog.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliRawReaderFile.h"
#include "AliRawReaderDate.h"
#include "AliRawReaderRoot.h"
#include "AliRawEventHeaderBase.h"
#include "AliESDEvent.h"
#include "AliESDMuonTrack.h"
#include "AliESDfriend.h"
#include "AliESDVertex.h"
#include "AliESDcascade.h"
#include "AliESDkink.h"
#include "AliESDtrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDCaloCells.h"
#include "AliMultiplicity.h"
#include "AliTracker.h"
#include "AliVertexer.h"
#include "AliVertexerTracks.h"
#include "AliV0vertexer.h"
#include "AliCascadeVertexer.h"
#include "AliHeader.h"
#include "AliGenEventHeader.h"
#include "AliPID.h"
#include "AliESDpid.h"
#include "AliESDtrack.h"
#include "AliESDPmdTrack.h"

#include "AliESDTagCreator.h"
#include "AliAODTagCreator.h"

#include "AliGeomManager.h"
#include "AliTrackPointArray.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliAlignObj.h"

#include "AliCentralTrigger.h"
#include "AliCTPRawStream.h"

#include "AliAODEvent.h"
#include "AliAODHeader.h"
#include "AliAODTrack.h"
#include "AliAODVertex.h"
#include "AliAODv0.h"
#include "AliAODJet.h"
#include "AliAODCaloCells.h"
#include "AliAODCaloCluster.h"
#include "AliAODPmdCluster.h"
#include "AliAODFmdCluster.h"
#include "AliAODTracklets.h"

#include "AliQADataMaker.h" 
#include "AliQA.h"
#include "AliQADataMakerSteer.h"

#include "AliSysInfo.h" // memory snapshots


ClassImp(AliReconstruction)


//_____________________________________________________________________________
const char* AliReconstruction::fgkDetectorName[AliReconstruction::fgkNDetectors] = {"ITS", "TPC", "TRD", "TOF", "PHOS", "HMPID", "EMCAL", "MUON", "FMD", "ZDC", "PMD", "T0", "VZERO", "ACORDE", "HLT"};

//_____________________________________________________________________________
AliReconstruction::AliReconstruction(const char* gAliceFilename,
				     const char* name, const char* title) :
  TNamed(name, title),

  fUniformField(kTRUE),
  fRunVertexFinder(kTRUE),
  fRunHLTTracking(kFALSE),
  fRunMuonTracking(kFALSE),
  fRunV0Finder(kTRUE),
  fRunCascadeFinder(kTRUE),
  fStopOnError(kFALSE),
  fWriteAlignmentData(kFALSE),
  fWriteESDfriend(kFALSE),
  fWriteAOD(kFALSE),
  fFillTriggerESD(kTRUE),

  fCleanESD(kTRUE),
  fV0DCAmax(3.),
  fV0CsPmin(0.),
  fDmax(50.),
  fZmax(50.),

  fRunLocalReconstruction("ALL"),
  fRunTracking("ALL"),
  fFillESD("ALL"),
  fUseTrackingErrorsForAlignment(""),
  fGAliceFileName(gAliceFilename),
  fInput(""),
  fEquipIdMap(""),
  fFirstEvent(0),
  fLastEvent(-1),
  fNumberOfEventsPerFile(1),
  fCheckPointLevel(0),
  fOptions(),
  fLoadAlignFromCDB(kTRUE),
  fLoadAlignData("ALL"),
  fESDPar(""),

  fRunLoader(NULL),
  fRawReader(NULL),

  fVertexer(NULL),
  fDiamondProfile(NULL),
  fMeanVertexConstraint(kTRUE),

  fGRPList(NULL),

  fAlignObjArray(NULL),
  fCDBUri(),
  fSpecCDBUri(), 
  fInitCDBCalled(kFALSE),
  fSetRunNumberFromDataCalled(kFALSE),
  fRunQA(kTRUE),  
  fQAInLoop(kFALSE) 

{
// create reconstruction object with default parameters
  
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    fReconstructor[iDet] = NULL;
    fLoader[iDet] = NULL;
    fTracker[iDet] = NULL;
    fQADataMaker[iDet] = NULL;
	fQACycles[iDet] = 999999;	
  }
  AliPID pid;
}

//_____________________________________________________________________________
AliReconstruction::AliReconstruction(const AliReconstruction& rec) :
  TNamed(rec),

  fUniformField(rec.fUniformField),
  fRunVertexFinder(rec.fRunVertexFinder),
  fRunHLTTracking(rec.fRunHLTTracking),
  fRunMuonTracking(rec.fRunMuonTracking),
  fRunV0Finder(rec.fRunV0Finder),
  fRunCascadeFinder(rec.fRunCascadeFinder),
  fStopOnError(rec.fStopOnError),
  fWriteAlignmentData(rec.fWriteAlignmentData),
  fWriteESDfriend(rec.fWriteESDfriend),
  fWriteAOD(rec.fWriteAOD),
  fFillTriggerESD(rec.fFillTriggerESD),

  fCleanESD(rec.fCleanESD),
  fV0DCAmax(rec.fV0DCAmax),
  fV0CsPmin(rec.fV0CsPmin),
  fDmax(rec.fDmax),
  fZmax(rec.fZmax),

  fRunLocalReconstruction(rec.fRunLocalReconstruction),
  fRunTracking(rec.fRunTracking),
  fFillESD(rec.fFillESD),
  fUseTrackingErrorsForAlignment(rec.fUseTrackingErrorsForAlignment),
  fGAliceFileName(rec.fGAliceFileName),
  fInput(rec.fInput),
  fEquipIdMap(rec.fEquipIdMap),
  fFirstEvent(rec.fFirstEvent),
  fLastEvent(rec.fLastEvent),
  fNumberOfEventsPerFile(rec.fNumberOfEventsPerFile),
  fCheckPointLevel(0),
  fOptions(),
  fLoadAlignFromCDB(rec.fLoadAlignFromCDB),
  fLoadAlignData(rec.fLoadAlignData),
  fESDPar(rec.fESDPar),

  fRunLoader(NULL),
  fRawReader(NULL),

  fVertexer(NULL),
  fDiamondProfile(NULL),
  fMeanVertexConstraint(rec.fMeanVertexConstraint),

  fGRPList(NULL),

  fAlignObjArray(rec.fAlignObjArray),
  fCDBUri(rec.fCDBUri),
  fSpecCDBUri(), 
  fInitCDBCalled(rec.fInitCDBCalled),
  fSetRunNumberFromDataCalled(rec.fSetRunNumberFromDataCalled),
  fRunQA(kTRUE),  
  fQAInLoop(kFALSE) 

{
// copy constructor

  for (Int_t i = 0; i < rec.fOptions.GetEntriesFast(); i++) {
    if (rec.fOptions[i]) fOptions.Add(rec.fOptions[i]->Clone());
  }
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    fReconstructor[iDet] = NULL;
    fLoader[iDet] = NULL;
    fTracker[iDet] = NULL;
    fQADataMaker[iDet] = NULL;
	fQACycles[iDet] = rec.fQACycles[iDet];	
  }
  for (Int_t i = 0; i < rec.fSpecCDBUri.GetEntriesFast(); i++) {
    if (rec.fSpecCDBUri[i]) fSpecCDBUri.Add(rec.fSpecCDBUri[i]->Clone());
  }
}

//_____________________________________________________________________________
AliReconstruction& AliReconstruction::operator = (const AliReconstruction& rec)
{
// assignment operator

  this->~AliReconstruction();
  new(this) AliReconstruction(rec);
  return *this;
}

//_____________________________________________________________________________
AliReconstruction::~AliReconstruction()
{
// clean up

  CleanUp();
  fOptions.Delete();
  fSpecCDBUri.Delete();

  AliCodeTimer::Instance()->Print();
}

//_____________________________________________________________________________
void AliReconstruction::InitCDB()
{
// activate a default CDB storage
// First check if we have any CDB storage set, because it is used 
// to retrieve the calibration and alignment constants

  if (fInitCDBCalled) return;
  fInitCDBCalled = kTRUE;

  AliCDBManager* man = AliCDBManager::Instance();
  if (man->IsDefaultStorageSet())
  {
    AliWarning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    AliWarning("Default CDB storage has been already set !");
    AliWarning(Form("Ignoring the default storage declared in AliReconstruction: %s",fCDBUri.Data()));
    AliWarning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    fCDBUri = man->GetDefaultStorage()->GetURI();
  }
  else {
    if (fCDBUri.Length() > 0) 
    {
    	AliDebug(2,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    	AliDebug(2, Form("Default CDB storage is set to: %s", fCDBUri.Data()));
    	AliDebug(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    } else {
    	fCDBUri="local://$ALICE_ROOT";
    	AliWarning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    	AliWarning("Default CDB storage not yet set !!!!");
    	AliWarning(Form("Setting it now to: %s", fCDBUri.Data()));
    	AliWarning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    		
    }
    man->SetDefaultStorage(fCDBUri);
  }

  // Now activate the detector specific CDB storage locations
  for (Int_t i = 0; i < fSpecCDBUri.GetEntriesFast(); i++) {
    TObject* obj = fSpecCDBUri[i];
    if (!obj) continue;
    AliDebug(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    AliDebug(2, Form("Specific CDB storage for %s is set to: %s",obj->GetName(),obj->GetTitle()));
    AliDebug(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    man->SetSpecificStorage(obj->GetName(), obj->GetTitle());
  }
  
}

//_____________________________________________________________________________
void AliReconstruction::SetDefaultStorage(const char* uri) {
// Store the desired default CDB storage location
// Activate it later within the Run() method

  fCDBUri = uri;

}

//_____________________________________________________________________________
void AliReconstruction::SetSpecificStorage(const char* calibType, const char* uri) {
// Store a detector-specific CDB storage location
// Activate it later within the Run() method

  AliCDBPath aPath(calibType);
  if(!aPath.IsValid()){
	// if calibType is not wildcard but it is a valid detector, add "/*" to make it a valid path
	for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
		if(!strcmp(calibType, fgkDetectorName[iDet])) {
			aPath.SetPath(Form("%s/*", calibType));
			AliInfo(Form("Path for specific storage set to %s", aPath.GetPath().Data()));
			break;
		}
        }
	if(!aPath.IsValid()){
  		AliError(Form("Not a valid path or detector: %s", calibType));
  		return;
	}
  }

//  // check that calibType refers to a "valid" detector name
//  Bool_t isDetector = kFALSE;
//  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
//    TString detName = fgkDetectorName[iDet];
//    if(aPath.GetLevel0() == detName) {
//    	isDetector = kTRUE;
//	break;
//    }
//  }
//
//  if(!isDetector) {
//	AliError(Form("Not a valid detector: %s", aPath.GetLevel0().Data()));
//	return;
//  }

  TObject* obj = fSpecCDBUri.FindObject(aPath.GetPath().Data());
  if (obj) fSpecCDBUri.Remove(obj);
  fSpecCDBUri.Add(new TNamed(aPath.GetPath().Data(), uri));

}

//_____________________________________________________________________________
Bool_t AliReconstruction::SetRunNumberFromData()
{
  // The method is called in Run() in order
  // to set a correct run number.
  // In case of raw data reconstruction the
  // run number is taken from the raw data header

  if (fSetRunNumberFromDataCalled) return kTRUE;
  fSetRunNumberFromDataCalled = kTRUE;
  
  AliCDBManager* man = AliCDBManager::Instance();
  
  if(man->GetRun() > 0) {
  	AliWarning("Run number is taken from event header! Ignoring settings in AliCDBManager!");
  } 
  
  if (!fRunLoader) {
      AliError("No run loader is found !"); 
      return kFALSE;
    }
    // read run number from gAlice
    if(fRunLoader->GetAliRun())
      AliCDBManager::Instance()->SetRun(fRunLoader->GetHeader()->GetRun());
    else {
      if(fRawReader) {
	if(fRawReader->NextEvent()) {
	  AliCDBManager::Instance()->SetRun(fRawReader->GetRunNumber());
	  fRawReader->RewindEvents();
	}
	else {
	  AliError("No raw-data events found !");
	  return kFALSE;
	}
      }
      else {
	AliError("Neither gAlice nor RawReader objects are found !");
	return kFALSE;
      }
  }

  man->Print();  
  
  return kTRUE;
}

//_____________________________________________________________________________
void AliReconstruction::SetCDBLock() {
  // Set CDB lock: from now on it is forbidden to reset the run number
  // or the default storage or to activate any further storage!
  
  AliCDBManager::Instance()->SetLock(1);
}

//_____________________________________________________________________________
Bool_t AliReconstruction::MisalignGeometry(const TString& detectors)
{
  // Read the alignment objects from CDB.
  // Each detector is supposed to have the
  // alignment objects in DET/Align/Data CDB path.
  // All the detector objects are then collected,
  // sorted by geometry level (starting from ALIC) and
  // then applied to the TGeo geometry.
  // Finally an overlaps check is performed.

  // Load alignment data from CDB and fill fAlignObjArray 
  if(fLoadAlignFromCDB){
  	
    TString detStr = detectors;
    TString loadAlObjsListOfDets = "";
    
    for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
      if (!IsSelected(fgkDetectorName[iDet], detStr)) continue;
      loadAlObjsListOfDets += fgkDetectorName[iDet];
      loadAlObjsListOfDets += " ";
    } // end loop over detectors
    loadAlObjsListOfDets.Prepend("GRP "); //add alignment objects for non-sensitive modules
    AliGeomManager::ApplyAlignObjsFromCDB(loadAlObjsListOfDets.Data());
  }else{
    // Check if the array with alignment objects was
    // provided by the user. If yes, apply the objects
    // to the present TGeo geometry
    if (fAlignObjArray) {
      if (gGeoManager && gGeoManager->IsClosed()) {
	if (AliGeomManager::ApplyAlignObjsToGeom(*fAlignObjArray) == kFALSE) {
	  AliError("The misalignment of one or more volumes failed!"
		   "Compare the list of simulated detectors and the list of detector alignment data!");
	  return kFALSE;
	}
      }
      else {
	AliError("Can't apply the misalignment! gGeoManager doesn't exist or it is still opened!");
	return kFALSE;
      }
    }
  }
  
  delete fAlignObjArray; fAlignObjArray=0;

  return kTRUE;
}

//_____________________________________________________________________________
void AliReconstruction::SetGAliceFile(const char* fileName)
{
// set the name of the galice file

  fGAliceFileName = fileName;
}

//_____________________________________________________________________________
void AliReconstruction::SetOption(const char* detector, const char* option)
{
// set options for the reconstruction of a detector

  TObject* obj = fOptions.FindObject(detector);
  if (obj) fOptions.Remove(obj);
  fOptions.Add(new TNamed(detector, option));
}


//_____________________________________________________________________________
Bool_t AliReconstruction::Run(const char* input)
{
// run the reconstruction

  AliCodeTimerAuto("")
  
  // set the input
  if (!input) input = fInput.Data();
  TString fileName(input);
  if (fileName.EndsWith("/")) {
    fRawReader = new AliRawReaderFile(fileName);
  } else if (fileName.EndsWith(".root")) {
    fRawReader = new AliRawReaderRoot(fileName);
  } else if (!fileName.IsNull()) {
    fRawReader = new AliRawReaderDate(fileName);
    fRawReader->SelectEvents(7);
  }
  if (!fEquipIdMap.IsNull() && fRawReader)
    fRawReader->LoadEquipmentIdsMap(fEquipIdMap);

   AliSysInfo::AddStamp("Start");
  // get the run loader
  if (!InitRunLoader()) return kFALSE;
   AliSysInfo::AddStamp("LoadLoader");

  // Initialize the CDB storage
  InitCDB();
  
  AliSysInfo::AddStamp("LoadCDB");

  // Set run number in CDBManager (if it is not already set by the user)
  if (!SetRunNumberFromData()) if (fStopOnError) return kFALSE;
  
  // Set CDB lock: from now on it is forbidden to reset the run number
  // or the default storage or to activate any further storage!
  SetCDBLock();
  
  // Import ideal TGeo geometry and apply misalignment
  if (!gGeoManager) {
    TString geom(gSystem->DirName(fGAliceFileName));
    geom += "/geometry.root";
    AliGeomManager::LoadGeometry(geom.Data());
    if (!gGeoManager) if (fStopOnError) return kFALSE;
  }

  if (!MisalignGeometry(fLoadAlignData)) if (fStopOnError) return kFALSE;
   AliSysInfo::AddStamp("LoadGeom");

  //QA 
  AliQADataMakerSteer qas ; 
  if ( fRunQA && fRawReader) 
		qas.Run("ALL", fRawReader) ; 
 
 // checking the QA of previous steps
  CheckQA() ; 
 
  // local reconstruction
  if (!fRunLocalReconstruction.IsNull()) {
    if (!RunLocalReconstruction(fRunLocalReconstruction)) {
      if (fStopOnError) {CleanUp(); return kFALSE;}
    }
  }
//  if (!fRunVertexFinder && fRunTracking.IsNull() && 
//      fFillESD.IsNull()) return kTRUE;

  // get vertexer
  if (fRunVertexFinder && !CreateVertexer()) {
    if (fStopOnError) {
      CleanUp(); 
      return kFALSE;
    }
  }
   AliSysInfo::AddStamp("Vertexer");

  // get trackers
  if (!fRunTracking.IsNull() && !CreateTrackers(fRunTracking)) {
    if (fStopOnError) {
      CleanUp(); 
      return kFALSE;
    }      
  }
   AliSysInfo::AddStamp("LoadTrackers");

  // get the possibly already existing ESD file and tree
  AliESDEvent* esd = new AliESDEvent(); AliESDEvent* hltesd = new AliESDEvent();
  TFile* fileOld = NULL;
  TTree* treeOld = NULL; TTree *hlttreeOld = NULL;
  if (!gSystem->AccessPathName("AliESDs.root")){
    gSystem->CopyFile("AliESDs.root", "AliESDs.old.root", kTRUE);
    fileOld = TFile::Open("AliESDs.old.root");
    if (fileOld && fileOld->IsOpen()) {
      treeOld = (TTree*) fileOld->Get("esdTree");
      if (treeOld)esd->ReadFromTree(treeOld);
      hlttreeOld = (TTree*) fileOld->Get("HLTesdTree");
      if (hlttreeOld)	hltesd->ReadFromTree(hlttreeOld);
    }
  }

  // create the ESD output file and tree
  TFile* file = TFile::Open("AliESDs.root", "RECREATE");
  file->SetCompressionLevel(2);
  if (!file->IsOpen()) {
    AliError("opening AliESDs.root failed");
    if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}    
  }

  TTree* tree = new TTree("esdTree", "Tree with ESD objects");
  esd = new AliESDEvent();
  esd->CreateStdContent();
  esd->WriteToTree(tree);

  TTree* hlttree = new TTree("HLTesdTree", "Tree with HLT ESD objects");
  hltesd = new AliESDEvent();
  hltesd->CreateStdContent();
  hltesd->WriteToTree(hlttree);

  /* CKB Why?
  delete esd; delete hltesd;
  esd = NULL; hltesd = NULL;
  */
  // create the branch with ESD additions



  AliESDfriend *esdf = 0; 
  if (fWriteESDfriend) {
    esdf = new AliESDfriend();
    TBranch *br=tree->Branch("ESDfriend.","AliESDfriend", &esdf);
    br->SetFile("AliESDfriends.root");
    esd->AddObject(esdf);
  }

  
  // Get the GRP CDB entry
  AliCDBEntry* entryGRP = AliCDBManager::Instance()->Get("GRP/GRP/Data");
	
  if(entryGRP) {
  	fGRPList = dynamic_cast<TList*> (entryGRP->GetObject());  
  } else {
  	AliError("No GRP entry found in OCDB!");
  }

  // Get the diamond profile from OCDB
  AliCDBEntry* entry = AliCDBManager::Instance()
  	->Get("GRP/Calib/MeanVertex");
	
  if(entry) {
  	fDiamondProfile = dynamic_cast<AliESDVertex*> (entry->GetObject());  
  } else {
  	AliError("No diamond profile found in OCDB!");
  }

  AliVertexerTracks tVertexer(AliTracker::GetBz());
  if(fDiamondProfile && fMeanVertexConstraint) tVertexer.SetVtxStart(fDiamondProfile);

  // loop over events
 
  if (fRawReader) fRawReader->RewindEvents();
   TString detStr(fFillESD) ; 

  ProcInfo_t ProcInfo;
  gSystem->GetProcInfo(&ProcInfo);
  AliInfo(Form("Current memory usage %d %d", ProcInfo.fMemResident, ProcInfo.fMemVirtual));
  
  for (Int_t iEvent = 0; iEvent < fRunLoader->GetNumberOfEvents(); iEvent++) {
    if (fRawReader) fRawReader->NextEvent();
    if ((iEvent < fFirstEvent) || ((fLastEvent >= 0) && (iEvent > fLastEvent))) {
      // copy old ESD to the new one
      if (treeOld) {
	esd->ReadFromTree(treeOld);
	treeOld->GetEntry(iEvent);
      }
      tree->Fill();
      if (hlttreeOld) {
	esd->ReadFromTree(hlttreeOld);
	hlttreeOld->GetEntry(iEvent);
      }
      hlttree->Fill();
      continue;
    }
    
    AliInfo(Form("processing event %d", iEvent));
    fRunLoader->GetEvent(iEvent);

    char aFileName[256];
    sprintf(aFileName, "ESD_%d.%d_final.root", 
	    fRunLoader->GetHeader()->GetRun(), 
	    fRunLoader->GetHeader()->GetEventNrInRun());
    if (!gSystem->AccessPathName(aFileName)) continue;

    // local reconstruction
    if (!fRunLocalReconstruction.IsNull()) {
      if (!RunLocalEventReconstruction(fRunLocalReconstruction)) {
	if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
      }
    }

  
    esd->SetRunNumber(fRunLoader->GetHeader()->GetRun());
    hltesd->SetRunNumber(fRunLoader->GetHeader()->GetRun());
    esd->SetEventNumberInFile(fRunLoader->GetHeader()->GetEventNrInRun());
    hltesd->SetEventNumberInFile(fRunLoader->GetHeader()->GetEventNrInRun());
    
    // Set magnetic field from the tracker
    esd->SetMagneticField(AliTracker::GetBz());
    hltesd->SetMagneticField(AliTracker::GetBz());

    
    
    // Fill raw-data error log into the ESD
    if (fRawReader) FillRawDataErrorLog(iEvent,esd);

    // vertex finder
    if (fRunVertexFinder) {
      if (!ReadESD(esd, "vertex")) {
	if (!RunVertexFinder(esd)) {
	  if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
	}
	if (fCheckPointLevel > 0) WriteESD(esd, "vertex");
      }
    }

    // HLT tracking
    if (!fRunTracking.IsNull()) {
      if (fRunHLTTracking) {
	hltesd->SetVertex(esd->GetVertex());
	if (!RunHLTTracking(hltesd)) {
	  if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
	}
      }
    }

    // Muon tracking
    if (!fRunTracking.IsNull()) {
      if (fRunMuonTracking) {
	if (!RunMuonTracking(esd)) {
	  if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
	}
      }
    }

    // barrel tracking
    if (!fRunTracking.IsNull()) {
      if (!ReadESD(esd, "tracking")) {
	if (!RunTracking(esd)) {
	  if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
	}
	if (fCheckPointLevel > 0) WriteESD(esd, "tracking");
      }
    }

    // fill ESD
    if (!fFillESD.IsNull()) {
      if (!FillESD(esd, fFillESD)) {
	if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
      }
    }
  
//    if (!fFillESD.IsNull()) 

	  // do QA in the event loop if requested
	  if (fQAInLoop) 
		  RunQA(fFillESD.Data(), esd);

    // fill Event header information from the RawEventHeader
    if (fRawReader){FillRawEventHeaderESD(esd);}

    // combined PID
    AliESDpid::MakePID(esd);
    if (fCheckPointLevel > 1) WriteESD(esd, "PID");

    if (fFillTriggerESD) {
      if (!ReadESD(esd, "trigger")) {
	if (!FillTriggerESD(esd)) {
	  if (fStopOnError) {CleanUp(file, fileOld); return kFALSE;}
	}
	if (fCheckPointLevel > 1) WriteESD(esd, "trigger");
      }
    }

    file->cd();

    //Try to improve the reconstructed primary vertex position using the tracks
    AliESDVertex *pvtx=0;
    Bool_t dovertex=kTRUE;
    TObject* obj = fOptions.FindObject("ITS");
    if (obj) {
      TString optITS = obj->GetTitle();
      if (optITS.Contains("cosmics") || optITS.Contains("COSMICS")) 
	dovertex=kFALSE;
    }
    if(dovertex) pvtx=tVertexer.FindPrimaryVertex(esd);
    if(fDiamondProfile) esd->SetDiamond(fDiamondProfile);
    
    if (pvtx)
    if (pvtx->GetStatus()) {
       // Store the improved primary vertex
       esd->SetPrimaryVertex(pvtx);
       // Propagate the tracks to the DCA to the improved primary vertex
       Double_t somethingbig = 777.;
       Double_t bz = esd->GetMagneticField();
       Int_t nt=esd->GetNumberOfTracks();
       while (nt--) {
	 AliESDtrack *t = esd->GetTrack(nt);
         t->RelateToVertex(pvtx, bz, somethingbig);
       } 
    }

    if (fRunV0Finder) {
       // V0 finding
       AliV0vertexer vtxer;
       vtxer.Tracks2V0vertices(esd);

       if (fRunCascadeFinder) {
          // Cascade finding
          AliCascadeVertexer cvtxer;
          cvtxer.V0sTracks2CascadeVertices(esd);
       }
    }
 
    // write ESD
    if (fCleanESD) CleanESD(esd);
    if (fWriteESDfriend) {
      esdf->~AliESDfriend();
      new (esdf) AliESDfriend(); // Reset...
      esd->GetESDfriend(esdf);
    }
    tree->Fill();

    // write HLT ESD
    hlttree->Fill();

    if (fCheckPointLevel > 0)  WriteESD(esd, "final"); 
    esd->Reset();
    hltesd->Reset();
    if (fWriteESDfriend) {
      esdf->~AliESDfriend();
      new (esdf) AliESDfriend(); // Reset...
    }
    // esdf->Reset();
    // delete esdf; esdf = 0;
  // ESD QA 
 
    gSystem->GetProcInfo(&ProcInfo);
    AliInfo(Form("Event %d -> Current memory usage %d %d",iEvent, ProcInfo.fMemResident, ProcInfo.fMemVirtual));
  }
  
  detStr = fFillESD ; 

	
  // write quality assurance ESDs data (one entry for all events)
  if (fQAInLoop) {
	  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
		  if (!IsSelected(fgkDetectorName[iDet], detStr)) 
			  continue;
		  AliQADataMaker * qadm = GetQADataMaker(iDet);
		  if (!qadm) continue;
		  qadm->EndOfCycle(AliQA::kRECPOINTS);
		  qadm->EndOfCycle(AliQA::kESDS);
		  qadm->Finish();
	  }
  }

  tree->GetUserInfo()->Add(esd);
  hlttree->GetUserInfo()->Add(hltesd);
  
  const TMap *cdbMap = AliCDBManager::Instance()->GetStorageMap();	 
  const TList *cdbList = AliCDBManager::Instance()->GetRetrievedIds();	 
	 	 
   TMap *cdbMapCopy = new TMap(cdbMap->GetEntries());	 
   cdbMapCopy->SetOwner(1);	 
   cdbMapCopy->SetName("cdbMap");	 
   TIter iter(cdbMap->GetTable());	 
 	 
   TPair* pair = 0;	 
   while((pair = dynamic_cast<TPair*> (iter.Next()))){	 
         TObjString* keyStr = dynamic_cast<TObjString*> (pair->Key());	 
         TObjString* valStr = dynamic_cast<TObjString*> (pair->Value());	 
         cdbMapCopy->Add(new TObjString(keyStr->GetName()), new TObjString(valStr->GetName()));	 
   }	 
 	 
   TList *cdbListCopy = new TList();	 
   cdbListCopy->SetOwner(1);	 
   cdbListCopy->SetName("cdbList");	 
 	 
   TIter iter2(cdbList);	 
 	 
   AliCDBId* id=0;	 
   while((id = dynamic_cast<AliCDBId*> (iter2.Next()))){	 
         cdbListCopy->Add(new TObjString(id->ToString().Data()));	 
   }	 
 	 
   tree->GetUserInfo()->Add(cdbMapCopy);	 
   tree->GetUserInfo()->Add(cdbListCopy);


  if(fESDPar.Contains("ESD.par")){
    AliInfo("Attaching ESD.par to Tree");
    TNamed *fn = CopyFileToTNamed(fESDPar.Data(),"ESD.par");
    tree->GetUserInfo()->Add(fn);
  }


  file->cd();
  if (fWriteESDfriend)
    tree->SetBranchStatus("ESDfriend*",0);
  // we want to have only one tree version number
  tree->Write(tree->GetName(),TObject::kOverwrite);
  hlttree->Write();

  if (fWriteAOD) {
    TFile *aodFile = TFile::Open("AliAOD.root", "RECREATE");
    ESDFile2AODFile(file, aodFile);
    aodFile->Close();
  }

  gROOT->cd();
  CleanUp(file, fileOld);
    
  // Create tags for the events in the ESD tree (the ESD tree is always present)
  // In case of empty events the tags will contain dummy values
  AliESDTagCreator *esdtagCreator = new AliESDTagCreator();
  esdtagCreator->CreateESDTags(fFirstEvent,fLastEvent,fGRPList);
  if (fWriteAOD) {
    AliAODTagCreator *aodtagCreator = new AliAODTagCreator();
    aodtagCreator->CreateAODTags(fFirstEvent,fLastEvent,fGRPList);
  }

  //QA fone outside the event loop
  if (!fQAInLoop) {
	  if ( fRunQA ) {
		  qas.Run(fRunLocalReconstruction.Data(), AliQA::kRECPOINTS) ;
		  //	qas.Reset() ;
		  qas.Run(fRunTracking.Data(), AliQA::kESDS) ;
	  }
  }
  
  // Cleanup of CDB manager: cache and active storages!
  AliCDBManager::Instance()->ClearCache();
  
  
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::RunLocalReconstruction(const TString& detectors)
{
// run the local reconstruction
  static Int_t eventNr=0;
  AliCodeTimerAuto("")

 //  AliCDBManager* man = AliCDBManager::Instance();
//   Bool_t origCache = man->GetCacheFlag();

//   TString detStr = detectors;
//   for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
//     if (!IsSelected(fgkDetectorName[iDet], detStr)) continue;
//     AliReconstructor* reconstructor = GetReconstructor(iDet);
//     if (!reconstructor) continue;
//     if (reconstructor->HasLocalReconstruction()) continue;

//     AliCodeTimerStart(Form("running reconstruction for %s", fgkDetectorName[iDet]));
//     AliInfo(Form("running reconstruction for %s", fgkDetectorName[iDet]));
    
//     AliCodeTimerStart(Form("Loading calibration data from OCDB for %s", fgkDetectorName[iDet]));                          
//     AliInfo(Form("Loading calibration data from OCDB for %s", fgkDetectorName[iDet]));

//     man->SetCacheFlag(kTRUE);
//     TString calibPath = Form("%s/Calib/*", fgkDetectorName[iDet]);
//     man->GetAll(calibPath); // entries are cached!

//     AliCodeTimerStop(Form("Loading calibration data from OCDB for %s", fgkDetectorName[iDet]));
     
//     if (fRawReader) {
//       fRawReader->RewindEvents();
//       reconstructor->Reconstruct(fRunLoader, fRawReader);
//     } else {
//       reconstructor->Reconstruct(fRunLoader);
//     }
     
//      AliCodeTimerStop(Form("running reconstruction for %s", fgkDetectorName[iDet]));
    // AliSysInfo::AddStamp(Form("LRec%s_%d",fgkDetectorName[iDet],eventNr));

//     // unload calibration data
//     man->UnloadFromCache(calibPath);
//     //man->ClearCache();
//   }

//   man->SetCacheFlag(origCache);

//   if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
//     AliError(Form("the following detectors were not found: %s",
//                   detStr.Data()));
//     if (fStopOnError) return kFALSE;
//   }

	  eventNr++;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunLocalEventReconstruction(const TString& detectors)
{
// run the local reconstruction
  static Int_t eventNr=0;
  AliCodeTimerAuto("")

  TString detStr = detectors;
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    if (!IsSelected(fgkDetectorName[iDet], detStr)) continue;
    AliReconstructor* reconstructor = GetReconstructor(iDet);
    if (!reconstructor) continue;
    AliLoader* loader = fLoader[iDet];
    if (!loader) {
      AliWarning(Form("No loader is defined for %s!",fgkDetectorName[iDet]));
      continue;
    }

    // conversion of digits
    if (fRawReader && reconstructor->HasDigitConversion()) {
      AliInfo(Form("converting raw data digits into root objects for %s", 
		   fgkDetectorName[iDet]));
      AliCodeTimerAuto(Form("converting raw data digits into root objects for %s", 
                            fgkDetectorName[iDet]));
      loader->LoadDigits("update");
      loader->CleanDigits();
      loader->MakeDigitsContainer();
      TTree* digitsTree = loader->TreeD();
      reconstructor->ConvertDigits(fRawReader, digitsTree);
      loader->WriteDigits("OVERWRITE");
      loader->UnloadDigits();
    }

    // local reconstruction
    AliInfo(Form("running reconstruction for %s", fgkDetectorName[iDet]));
    AliCodeTimerAuto(Form("running reconstruction for %s", fgkDetectorName[iDet]));
    loader->LoadRecPoints("update");
    loader->CleanRecPoints();
    loader->MakeRecPointsContainer();
    TTree* clustersTree = loader->TreeR();
    if (fRawReader && !reconstructor->HasDigitConversion()) {
      reconstructor->Reconstruct(fRawReader, clustersTree);
    } else {
      loader->LoadDigits("read");
      TTree* digitsTree = loader->TreeD();
      if (!digitsTree) {
	AliError(Form("Can't get the %s digits tree", fgkDetectorName[iDet]));
	if (fStopOnError) return kFALSE;
      } else {
	reconstructor->Reconstruct(digitsTree, clustersTree);
      }
      loader->UnloadDigits();
    }

	// do QA in the event loop if requested
	if (fQAInLoop) {
		AliQADataMaker * qadm = GetQADataMaker(iDet);
		if (qadm) {
			AliCodeTimerStart(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
			AliInfo(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
			if (qadm->IsCycleDone() ) {
				qadm->EndOfCycle(AliQA::kRECPOINTS) ; 
				qadm->EndOfCycle(AliQA::kESDS) ; 
				qadm->StartOfCycle(AliQA::kRECPOINTS) ; 
				qadm->StartOfCycle(AliQA::kESDS, "same") ; 
			}
			qadm->Exec(AliQA::kRECPOINTS, clustersTree) ; 
			AliCodeTimerStop(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
		}
	}

	loader->WriteRecPoints("OVERWRITE");
    loader->UnloadRecPoints();
    AliSysInfo::AddStamp(Form("LRec%s_%d",fgkDetectorName[iDet],eventNr), iDet,1,eventNr);
  }

  if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
    AliError(Form("the following detectors were not found: %s",
                  detStr.Data()));
    if (fStopOnError) return kFALSE;
  }
  eventNr++;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunVertexFinder(AliESDEvent*& esd)
{
// run the barrel tracking

  AliCodeTimerAuto("")

  AliESDVertex* vertex = NULL;
  Double_t vtxPos[3] = {0, 0, 0};
  Double_t vtxErr[3] = {0.07, 0.07, 0.1};
  TArrayF mcVertex(3); 
  if (fRunLoader->GetHeader() && fRunLoader->GetHeader()->GenEventHeader()) {
    fRunLoader->GetHeader()->GenEventHeader()->PrimaryVertex(mcVertex);
    for (Int_t i = 0; i < 3; i++) vtxPos[i] = mcVertex[i];
  }

  if (fVertexer) {
    if(fDiamondProfile) fVertexer->SetVtxStart(fDiamondProfile);
    AliInfo("running the ITS vertex finder");
    if (fLoader[0]) fLoader[0]->LoadRecPoints();
    vertex = fVertexer->FindVertexForCurrentEvent(fRunLoader->GetEventNumber());
    if (fLoader[0]) fLoader[0]->UnloadRecPoints();
    if(!vertex){
      AliWarning("Vertex not found");
      vertex = new AliESDVertex();
      vertex->SetName("default");
    }
    else {
      vertex->SetName("reconstructed");
    }

  } else {
    AliInfo("getting the primary vertex from MC");
    vertex = new AliESDVertex(vtxPos, vtxErr);
  }

  if (vertex) {
    vertex->GetXYZ(vtxPos);
    vertex->GetSigmaXYZ(vtxErr);
  } else {
    AliWarning("no vertex reconstructed");
    vertex = new AliESDVertex(vtxPos, vtxErr);
  }
  esd->SetVertex(vertex);
  // if SPD multiplicity has been determined, it is stored in the ESD
  AliMultiplicity *mult = fVertexer->GetMultiplicity();
  if(mult)esd->SetMultiplicity(mult);

  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    if (fTracker[iDet]) fTracker[iDet]->SetVertex(vtxPos, vtxErr);
  }  
  delete vertex;

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunHLTTracking(AliESDEvent*& esd)
{
// run the HLT barrel tracking

  AliCodeTimerAuto("")

  if (!fRunLoader) {
    AliError("Missing runLoader!");
    return kFALSE;
  }

  AliInfo("running HLT tracking");

  // Get a pointer to the HLT reconstructor
  AliReconstructor *reconstructor = GetReconstructor(fgkNDetectors-1);
  if (!reconstructor) return kFALSE;

  // TPC + ITS
  for (Int_t iDet = 1; iDet >= 0; iDet--) {
    TString detName = fgkDetectorName[iDet];
    AliDebug(1, Form("%s HLT tracking", detName.Data()));
    reconstructor->SetOption(detName.Data());
    AliTracker *tracker = reconstructor->CreateTracker();
    if (!tracker) {
      AliWarning(Form("couldn't create a HLT tracker for %s", detName.Data()));
      if (fStopOnError) return kFALSE;
      continue;
    }
    Double_t vtxPos[3];
    Double_t vtxErr[3]={0.005,0.005,0.010};
    const AliESDVertex *vertex = esd->GetVertex();
    vertex->GetXYZ(vtxPos);
    tracker->SetVertex(vtxPos,vtxErr);
    if(iDet != 1) {
      fLoader[iDet]->LoadRecPoints("read");
      TTree* tree = fLoader[iDet]->TreeR();
      if (!tree) {
	AliError(Form("Can't get the %s cluster tree", detName.Data()));
	return kFALSE;
      }
      tracker->LoadClusters(tree);
    }
    if (tracker->Clusters2Tracks(esd) != 0) {
      AliError(Form("HLT %s Clusters2Tracks failed", fgkDetectorName[iDet]));
      return kFALSE;
    }
    if(iDet != 1) {
      tracker->UnloadClusters();
    }
    delete tracker;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunMuonTracking(AliESDEvent*& esd)
{
// run the muon spectrometer tracking

  AliCodeTimerAuto("")

  if (!fRunLoader) {
    AliError("Missing runLoader!");
    return kFALSE;
  }
  Int_t iDet = 7; // for MUON

  AliInfo("is running...");

  // Get a pointer to the MUON reconstructor
  AliReconstructor *reconstructor = GetReconstructor(iDet);
  if (!reconstructor) return kFALSE;

  
  TString detName = fgkDetectorName[iDet];
  AliDebug(1, Form("%s tracking", detName.Data()));
  AliTracker *tracker =  reconstructor->CreateTracker();
  if (!tracker) {
    AliWarning(Form("couldn't create a tracker for %s", detName.Data()));
    return kFALSE;
  }
     
  // read RecPoints
  fLoader[iDet]->LoadRecPoints("read");  

  tracker->LoadClusters(fLoader[iDet]->TreeR());
  
  Int_t rv = tracker->Clusters2Tracks(esd);
  
  if ( rv )
  {
    AliError(Form("%s Clusters2Tracks failed", fgkDetectorName[iDet]));
    return kFALSE;
  }
  
  fLoader[iDet]->UnloadRecPoints();

  tracker->UnloadClusters();
  
  delete tracker;
  
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::RunTracking(AliESDEvent*& esd)
{
// run the barrel tracking
  static Int_t eventNr=0;
  AliCodeTimerAuto("")

  AliInfo("running tracking");

  //Fill the ESD with the T0 info (will be used by the TOF) 
  if (fReconstructor[11] && fLoader[11]) {
    fLoader[11]->LoadRecPoints("READ");
    TTree *treeR = fLoader[11]->TreeR();
    GetReconstructor(11)->FillESD((TTree *)NULL,treeR,esd);
  }

  // pass 1: TPC + ITS inwards
  for (Int_t iDet = 1; iDet >= 0; iDet--) {
    if (!fTracker[iDet]) continue;
    AliDebug(1, Form("%s tracking", fgkDetectorName[iDet]));

    // load clusters
    fLoader[iDet]->LoadRecPoints("read");
    AliSysInfo::AddStamp(Form("RLoadCluster%s_%d",fgkDetectorName[iDet],eventNr),iDet,1, eventNr);
    TTree* tree = fLoader[iDet]->TreeR();
    if (!tree) {
      AliError(Form("Can't get the %s cluster tree", fgkDetectorName[iDet]));
      return kFALSE;
    }
    fTracker[iDet]->LoadClusters(tree);
    AliSysInfo::AddStamp(Form("TLoadCluster%s_%d",fgkDetectorName[iDet],eventNr), iDet,2, eventNr);
    // run tracking
    if (fTracker[iDet]->Clusters2Tracks(esd) != 0) {
      AliError(Form("%s Clusters2Tracks failed", fgkDetectorName[iDet]));
      return kFALSE;
    }
    if (fCheckPointLevel > 1) {
      WriteESD(esd, Form("%s.tracking", fgkDetectorName[iDet]));
    }
    // preliminary PID in TPC needed by the ITS tracker
    if (iDet == 1) {
      GetReconstructor(1)->FillESD((TTree*)NULL, (TTree*)NULL, esd);
      AliESDpid::MakePID(esd);
    } 
    AliSysInfo::AddStamp(Form("Tracking0%s_%d",fgkDetectorName[iDet],eventNr), iDet,3,eventNr);
  }

  // pass 2: ALL backwards
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    if (!fTracker[iDet]) continue;
    AliDebug(1, Form("%s back propagation", fgkDetectorName[iDet]));

    // load clusters
    if (iDet > 1) {     // all except ITS, TPC
      TTree* tree = NULL;
      fLoader[iDet]->LoadRecPoints("read");
      AliSysInfo::AddStamp(Form("RLoadCluster0%s_%d",fgkDetectorName[iDet],eventNr), iDet,1, eventNr);
      tree = fLoader[iDet]->TreeR();
      if (!tree) {
	AliError(Form("Can't get the %s cluster tree", fgkDetectorName[iDet]));
	return kFALSE;
      }
      fTracker[iDet]->LoadClusters(tree); 
      AliSysInfo::AddStamp(Form("TLoadCluster0%s_%d",fgkDetectorName[iDet],eventNr), iDet,2, eventNr);
    }

    // run tracking
    if (fTracker[iDet]->PropagateBack(esd) != 0) {
      AliError(Form("%s backward propagation failed", fgkDetectorName[iDet]));
      //      return kFALSE;
    }
    if (fCheckPointLevel > 1) {
      WriteESD(esd, Form("%s.back", fgkDetectorName[iDet]));
    }

    // unload clusters
    if (iDet > 2) {     // all except ITS, TPC, TRD
      fTracker[iDet]->UnloadClusters();
      fLoader[iDet]->UnloadRecPoints();
    }
    // updated PID in TPC needed by the ITS tracker -MI
    if (iDet == 1) {
      GetReconstructor(1)->FillESD((TTree*)NULL, (TTree*)NULL, esd);
      AliESDpid::MakePID(esd);
    }
    AliSysInfo::AddStamp(Form("Tracking1%s_%d",fgkDetectorName[iDet],eventNr), iDet,3, eventNr);
  }

  // write space-points to the ESD in case alignment data output
  // is switched on
  if (fWriteAlignmentData)
    WriteAlignmentData(esd);

  // pass 3: TRD + TPC + ITS refit inwards
  for (Int_t iDet = 2; iDet >= 0; iDet--) {
    if (!fTracker[iDet]) continue;
    AliDebug(1, Form("%s inward refit", fgkDetectorName[iDet]));

    // run tracking
    if (fTracker[iDet]->RefitInward(esd) != 0) {
      AliError(Form("%s inward refit failed", fgkDetectorName[iDet]));
      //      return kFALSE;
    }
    if (fCheckPointLevel > 1) {
      WriteESD(esd, Form("%s.refit", fgkDetectorName[iDet]));
    }
    AliSysInfo::AddStamp(Form("Tracking2%s_%d",fgkDetectorName[iDet],eventNr), iDet,3, eventNr);
    // unload clusters
    fTracker[iDet]->UnloadClusters();
    AliSysInfo::AddStamp(Form("TUnloadCluster%s_%d",fgkDetectorName[iDet],eventNr), iDet,4, eventNr);
    fLoader[iDet]->UnloadRecPoints();
    AliSysInfo::AddStamp(Form("RUnloadCluster%s_%d",fgkDetectorName[iDet],eventNr), iDet,5, eventNr);
  }
  //
  // Propagate track to the vertex - if not done by ITS
  //
  Int_t ntracks = esd->GetNumberOfTracks();
  for (Int_t itrack=0; itrack<ntracks; itrack++){
    const Double_t kRadius  = 3;   // beam pipe radius
    const Double_t kMaxStep = 5;   // max step
    const Double_t kMaxD    = 123456;  // max distance to prim vertex
    Double_t       fieldZ   = AliTracker::GetBz();  //
    AliESDtrack * track = esd->GetTrack(itrack);
    if (!track) continue;
    if (track->IsOn(AliESDtrack::kITSrefit)) continue;
   AliTracker::PropagateTrackTo(track,kRadius,track->GetMass(),kMaxStep,kTRUE);
    track->RelateToVertex(esd->GetVertex(),fieldZ, kMaxD);
  }
  eventNr++;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::CleanESD(AliESDEvent *esd){
  //
  // Remove the data which are not needed for the physics analysis.
  //

  Int_t nTracks=esd->GetNumberOfTracks();
  Int_t nV0s=esd->GetNumberOfV0s();
  AliInfo
  (Form("Number of ESD tracks and V0s before cleaning: %d %d",nTracks,nV0s));

  Float_t cleanPars[]={fV0DCAmax,fV0CsPmin,fDmax,fZmax};
  Bool_t rc=esd->Clean(cleanPars);

  nTracks=esd->GetNumberOfTracks();
  nV0s=esd->GetNumberOfV0s();
  AliInfo
  (Form("Number of ESD tracks and V0s after cleaning %d",nTracks,nV0s));

  return rc;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::FillESD(AliESDEvent*& esd, const TString& detectors)
{
// fill the event summary data

  AliCodeTimerAuto("")
    static Int_t eventNr=0; 
  TString detStr = detectors;
  
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
  if (!IsSelected(fgkDetectorName[iDet], detStr)) continue;
    AliReconstructor* reconstructor = GetReconstructor(iDet);
    if (!reconstructor) continue;
    if (!ReadESD(esd, fgkDetectorName[iDet])) {
      AliDebug(1, Form("filling ESD for %s", fgkDetectorName[iDet]));
      TTree* clustersTree = NULL;
      if (fLoader[iDet]) {
	fLoader[iDet]->LoadRecPoints("read");
	clustersTree = fLoader[iDet]->TreeR();
	if (!clustersTree) {
	  AliError(Form("Can't get the %s clusters tree", 
			fgkDetectorName[iDet]));
	  if (fStopOnError) return kFALSE;
	}
      }
      if (fRawReader && !reconstructor->HasDigitConversion()) {
        reconstructor->FillESD(fRawReader, clustersTree, esd);
      } else {
	TTree* digitsTree = NULL;
	if (fLoader[iDet]) {
	  fLoader[iDet]->LoadDigits("read");
	  digitsTree = fLoader[iDet]->TreeD();
	  if (!digitsTree) {
	    AliError(Form("Can't get the %s digits tree", 
			  fgkDetectorName[iDet]));
	    if (fStopOnError) return kFALSE;
	  }
	}
	reconstructor->FillESD(digitsTree, clustersTree, esd);
	if (fLoader[iDet]) fLoader[iDet]->UnloadDigits();
      }
      if (fLoader[iDet]) {
	fLoader[iDet]->UnloadRecPoints();
      }

      if (fCheckPointLevel > 2) WriteESD(esd, fgkDetectorName[iDet]);
    }
  }

  if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
    AliError(Form("the following detectors were not found: %s", 
                  detStr.Data()));
    if (fStopOnError) return kFALSE;
  }
  AliSysInfo::AddStamp(Form("FillESD%d",eventNr), 0,1, eventNr);
  eventNr++;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::FillTriggerESD(AliESDEvent*& esd)
{
  // Reads the trigger decision which is
  // stored in Trigger.root file and fills
  // the corresponding esd entries

  AliCodeTimerAuto("")
  
  AliInfo("Filling trigger information into the ESD");

  if (fRawReader) {
    AliCTPRawStream input(fRawReader);
    if (!input.Next()) {
      AliError("No valid CTP (trigger) DDL raw data is found ! The trigger information is not stored in the ESD !");
      return kFALSE;
    }
    esd->SetTriggerMask(input.GetClassMask());
    esd->SetTriggerCluster(input.GetClusterMask());
  }
  else {
    AliRunLoader *runloader = AliRunLoader::GetRunLoader();
    if (runloader) {
      if (!runloader->LoadTrigger()) {
	AliCentralTrigger *aCTP = runloader->GetTrigger();
	esd->SetTriggerMask(aCTP->GetClassMask());
	esd->SetTriggerCluster(aCTP->GetClusterMask());
      }
      else {
	AliWarning("No trigger can be loaded! The trigger information is not stored in the ESD !");
	return kFALSE;
      }
    }
    else {
      AliError("No run loader is available! The trigger information is not stored in the ESD !");
      return kFALSE;
    }
  }

  return kTRUE;
}





//_____________________________________________________________________________
Bool_t AliReconstruction::FillRawEventHeaderESD(AliESDEvent*& esd)
{
  // 
  // Filling information from RawReader Header
  // 

  AliInfo("Filling information from RawReader Header");
  esd->SetBunchCrossNumber(0);
  esd->SetOrbitNumber(0);
  esd->SetPeriodNumber(0);
  esd->SetTimeStamp(0);
  esd->SetEventType(0);
  const AliRawEventHeaderBase * eventHeader = fRawReader->GetEventHeader();
  if (eventHeader){

    const UInt_t *id = eventHeader->GetP("Id");
    esd->SetBunchCrossNumber((id)[1]&0x00000fff);
    esd->SetOrbitNumber((((id)[0]<<20)&0xf00000)|(((id)[1]>>12)&0xfffff));
    esd->SetPeriodNumber(((id)[0]>>4)&0x0fffffff);

    esd->SetTimeStamp((eventHeader->Get("Timestamp")));  
    esd->SetEventType((eventHeader->Get("Type")));
  }

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::IsSelected(TString detName, TString& detectors) const
{
// check whether detName is contained in detectors
// if yes, it is removed from detectors

  // check if all detectors are selected
  if ((detectors.CompareTo("ALL") == 0) ||
      detectors.BeginsWith("ALL ") ||
      detectors.EndsWith(" ALL") ||
      detectors.Contains(" ALL ")) {
    detectors = "ALL";
    return kTRUE;
  }

  // search for the given detector
  Bool_t result = kFALSE;
  if ((detectors.CompareTo(detName) == 0) ||
      detectors.BeginsWith(detName+" ") ||
      detectors.EndsWith(" "+detName) ||
      detectors.Contains(" "+detName+" ")) {
    detectors.ReplaceAll(detName, "");
    result = kTRUE;
  }

  // clean up the detectors string
  while (detectors.Contains("  ")) detectors.ReplaceAll("  ", " ");
  while (detectors.BeginsWith(" ")) detectors.Remove(0, 1);
  while (detectors.EndsWith(" ")) detectors.Remove(detectors.Length()-1, 1);

  return result;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::InitRunLoader()
{
// get or create the run loader

  if (gAlice) delete gAlice;
  gAlice = NULL;

  if (!gSystem->AccessPathName(fGAliceFileName.Data())) { // galice.root exists
    // load all base libraries to get the loader classes
    TString libs = gSystem->GetLibraries();
    for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
      TString detName = fgkDetectorName[iDet];
      if (detName == "HLT") continue;
      if (libs.Contains("lib" + detName + "base.so")) continue;
      gSystem->Load("lib" + detName + "base.so");
    }
    fRunLoader = AliRunLoader::Open(fGAliceFileName.Data());
    if (!fRunLoader) {
      AliError(Form("no run loader found in file %s", fGAliceFileName.Data()));
      CleanUp();
      return kFALSE;
    }
    fRunLoader->CdGAFile();
    if (gFile->GetKey(AliRunLoader::GetGAliceName())) {
      if (fRunLoader->LoadgAlice() == 0) {
	gAlice = fRunLoader->GetAliRun();
	AliTracker::SetFieldMap(gAlice->Field(),fUniformField);
      }
    }
    if (!gAlice && !fRawReader) {
      AliError(Form("no gAlice object found in file %s",
		    fGAliceFileName.Data()));
      CleanUp();
      return kFALSE;
    }

    //PH This is a temporary fix to give access to the kinematics
    //PH that is needed for the labels of ITS clusters
    fRunLoader->LoadHeader();
    fRunLoader->LoadKinematics();

  } else {               // galice.root does not exist
    if (!fRawReader) {
      AliError(Form("the file %s does not exist", fGAliceFileName.Data()));
      CleanUp();
      return kFALSE;
    }
    fRunLoader = AliRunLoader::Open(fGAliceFileName.Data(),
				    AliConfig::GetDefaultEventFolderName(),
				    "recreate");
    if (!fRunLoader) {
      AliError(Form("could not create run loader in file %s", 
		    fGAliceFileName.Data()));
      CleanUp();
      return kFALSE;
    }
    fRunLoader->MakeTree("E");
    Int_t iEvent = 0;
    while (fRawReader->NextEvent()) {
      fRunLoader->SetEventNumber(iEvent);
      fRunLoader->GetHeader()->Reset(fRawReader->GetRunNumber(), 
				     iEvent, iEvent);
      fRunLoader->MakeTree("H");
      fRunLoader->TreeE()->Fill();
      iEvent++;
    }
    fRawReader->RewindEvents();
    if (fNumberOfEventsPerFile > 0)
      fRunLoader->SetNumberOfEventsPerFile(fNumberOfEventsPerFile);
    else
      fRunLoader->SetNumberOfEventsPerFile(iEvent);
    fRunLoader->WriteHeader("OVERWRITE");
    fRunLoader->CdGAFile();
    fRunLoader->Write(0, TObject::kOverwrite);
//    AliTracker::SetFieldMap(???);
  }

  return kTRUE;
}

//_____________________________________________________________________________
AliReconstructor* AliReconstruction::GetReconstructor(Int_t iDet)
{
// get the reconstructor object and the loader for a detector

  if (fReconstructor[iDet]) return fReconstructor[iDet];

  // load the reconstructor object
  TPluginManager* pluginManager = gROOT->GetPluginManager();
  TString detName = fgkDetectorName[iDet];
  TString recName = "Ali" + detName + "Reconstructor";
  if (gAlice && !gAlice->GetDetector(detName) && (detName != "HLT")) return NULL;

  AliReconstructor* reconstructor = NULL;
  // first check if a plugin is defined for the reconstructor
  TPluginHandler* pluginHandler = 
    pluginManager->FindHandler("AliReconstructor", detName);
  // if not, add a plugin for it
  if (!pluginHandler) {
    AliDebug(1, Form("defining plugin for %s", recName.Data()));
    TString libs = gSystem->GetLibraries();
    if (libs.Contains("lib" + detName + "base.so") ||
	(gSystem->Load("lib" + detName + "base.so") >= 0)) {
      pluginManager->AddHandler("AliReconstructor", detName, 
				recName, detName + "rec", recName + "()");
    } else {
      pluginManager->AddHandler("AliReconstructor", detName, 
				recName, detName, recName + "()");
    }
    pluginHandler = pluginManager->FindHandler("AliReconstructor", detName);
  }
  if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) {
    reconstructor = (AliReconstructor*) pluginHandler->ExecPlugin(0);
  }
  if (reconstructor) {
    TObject* obj = fOptions.FindObject(detName.Data());
    if (obj) reconstructor->SetOption(obj->GetTitle());
    reconstructor->Init();
    fReconstructor[iDet] = reconstructor;
  }

  // get or create the loader
  if (detName != "HLT") {
    fLoader[iDet] = fRunLoader->GetLoader(detName + "Loader");
    if (!fLoader[iDet]) {
      AliConfig::Instance()
	->CreateDetectorFolders(fRunLoader->GetEventFolder(), 
				detName, detName);
      // first check if a plugin is defined for the loader
      pluginHandler = 
	pluginManager->FindHandler("AliLoader", detName);
      // if not, add a plugin for it
      if (!pluginHandler) {
	TString loaderName = "Ali" + detName + "Loader";
	AliDebug(1, Form("defining plugin for %s", loaderName.Data()));
	pluginManager->AddHandler("AliLoader", detName, 
				  loaderName, detName + "base", 
				  loaderName + "(const char*, TFolder*)");
	pluginHandler = pluginManager->FindHandler("AliLoader", detName);
      }
      if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) {
	fLoader[iDet] = 
	  (AliLoader*) pluginHandler->ExecPlugin(2, detName.Data(), 
						 fRunLoader->GetEventFolder());
      }
      if (!fLoader[iDet]) {   // use default loader
	fLoader[iDet] = new AliLoader(detName, fRunLoader->GetEventFolder());
      }
      if (!fLoader[iDet]) {
	AliWarning(Form("couldn't get loader for %s", detName.Data()));
	if (fStopOnError) return NULL;
      } else {
	fRunLoader->AddLoader(fLoader[iDet]);
	fRunLoader->CdGAFile();
	if (gFile && !gFile->IsWritable()) gFile->ReOpen("UPDATE");
	fRunLoader->Write(0, TObject::kOverwrite);
      }
    }
  }
      
  return reconstructor;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::CreateVertexer()
{
// create the vertexer

  fVertexer = NULL;
  AliReconstructor* itsReconstructor = GetReconstructor(0);
  if (itsReconstructor) {
    fVertexer = itsReconstructor->CreateVertexer();
  }
  if (!fVertexer) {
    AliWarning("couldn't create a vertexer for ITS");
    if (fStopOnError) return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::CreateTrackers(const TString& detectors)
{
// create the trackers

  TString detStr = detectors;
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    if (!IsSelected(fgkDetectorName[iDet], detStr)) continue;
    AliReconstructor* reconstructor = GetReconstructor(iDet);
    if (!reconstructor) continue;
    TString detName = fgkDetectorName[iDet];
    if (detName == "HLT") {
      fRunHLTTracking = kTRUE;
      continue;
    }
    if (detName == "MUON") {
      fRunMuonTracking = kTRUE;
      continue;
    }


    fTracker[iDet] = reconstructor->CreateTracker();
    if (!fTracker[iDet] && (iDet < 7)) {
      AliWarning(Form("couldn't create a tracker for %s", detName.Data()));
      if (fStopOnError) return kFALSE;
    }
    AliSysInfo::AddStamp(Form("LTracker%s",fgkDetectorName[iDet]), iDet,0);
  }

  return kTRUE;
}

//_____________________________________________________________________________
void AliReconstruction::CleanUp(TFile* file, TFile* fileOld)
{
// delete trackers and the run loader and close and delete the file

  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
    delete fReconstructor[iDet];
    fReconstructor[iDet] = NULL;
    fLoader[iDet] = NULL;
    delete fTracker[iDet];
    fTracker[iDet] = NULL;
//    delete fQADataMaker[iDet];
//    fQADataMaker[iDet] = NULL;
  }
  delete fVertexer;
  fVertexer = NULL;
  
  if(!(AliCDBManager::Instance()->GetCacheFlag())) {
  	delete fDiamondProfile;
  	fDiamondProfile = NULL;
  }

  delete fGRPList;
  fGRPList = NULL;

  delete fRunLoader;
  fRunLoader = NULL;
  delete fRawReader;
  fRawReader = NULL;

  if (file) {
    file->Close();
    delete file;
  }

  if (fileOld) {
    fileOld->Close();
    delete fileOld;
    gSystem->Unlink("AliESDs.old.root");
  }
}

//_____________________________________________________________________________

Bool_t AliReconstruction::ReadESD(AliESDEvent*& esd, const char* recStep) const
{
// read the ESD event from a file

  if (!esd) return kFALSE;
  char fileName[256];
  sprintf(fileName, "ESD_%d.%d_%s.root", 
	  esd->GetRunNumber(), esd->GetEventNumberInFile(), recStep);
  if (gSystem->AccessPathName(fileName)) return kFALSE;

  AliInfo(Form("reading ESD from file %s", fileName));
  AliDebug(1, Form("reading ESD from file %s", fileName));
  TFile* file = TFile::Open(fileName);
  if (!file || !file->IsOpen()) {
    AliError(Form("opening %s failed", fileName));
    delete file;
    return kFALSE;
  }

  gROOT->cd();
  delete esd;
  esd = (AliESDEvent*) file->Get("ESD");
  file->Close();
  delete file;
  return kTRUE;

}



//_____________________________________________________________________________
void AliReconstruction::WriteESD(AliESDEvent* esd, const char* recStep) const
{
// write the ESD event to a file

  if (!esd) return;
  char fileName[256];
  sprintf(fileName, "ESD_%d.%d_%s.root", 
	  esd->GetRunNumber(), esd->GetEventNumberInFile(), recStep);

  AliDebug(1, Form("writing ESD to file %s", fileName));
  TFile* file = TFile::Open(fileName, "recreate");
  if (!file || !file->IsOpen()) {
    AliError(Form("opening %s failed", fileName));
  } else {
    esd->Write("ESD");
    file->Close();
  }
  delete file;
}





//_____________________________________________________________________________
void AliReconstruction::ESDFile2AODFile(TFile* esdFile, TFile* aodFile)
{
  // write all files from the given esd file to an aod file

  // create an AliAOD object 
  AliAODEvent *aod = new AliAODEvent();
  aod->CreateStdContent();
  
  // go to the file
  aodFile->cd();
  
  // create the tree
  TTree *aodTree = new TTree("aodTree", "AliAOD tree");
  aodTree->Branch(aod->GetList());

  // connect to ESD
  TTree *t = (TTree*) esdFile->Get("esdTree");
  AliESDEvent *esd = new AliESDEvent();
  esd->ReadFromTree(t);

  Int_t nEvents = t->GetEntries();

  // set arrays and pointers
  Float_t posF[3];
  Double_t pos[3];
  Double_t p[3];
  Double_t p_pos[3];
  Double_t p_neg[3];
  Double_t covVtx[6];
  Double_t covTr[21];
  Double_t pid[10];

  // loop over events and fill them
  for (Int_t iEvent = 0; iEvent < nEvents; ++iEvent) {
    //cout << "event: " << iEvent << endl;
    t->GetEntry(iEvent);

    // Multiplicity information needed by the header (to be revised!)
    Int_t nTracks   = esd->GetNumberOfTracks();
    Int_t nPosTracks = 0;
    for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) 
      if (esd->GetTrack(iTrack)->Charge()> 0) nPosTracks++;

    // Access the header
    AliAODHeader *header = aod->GetHeader();

    // fill the header
    header->SetRunNumber       (esd->GetRunNumber()       );
    header->SetBunchCrossNumber(esd->GetBunchCrossNumber());
    header->SetOrbitNumber     (esd->GetOrbitNumber()     );
    header->SetPeriodNumber    (esd->GetPeriodNumber()    );
    header->SetTriggerMask     (esd->GetTriggerMask()     ); 
    header->SetTriggerCluster  (esd->GetTriggerCluster()  );
    header->SetEventType       (esd->GetEventType()       );
    header->SetMagneticField   (esd->GetMagneticField()   );
    header->SetZDCN1Energy     (esd->GetZDCN1Energy()     );
    header->SetZDCP1Energy     (esd->GetZDCP1Energy()     );
    header->SetZDCN2Energy     (esd->GetZDCN2Energy()     );
    header->SetZDCP2Energy     (esd->GetZDCP2Energy()     );
    header->SetZDCEMEnergy     (esd->GetZDCEMEnergy(0),esd->GetZDCEMEnergy(1));
    header->SetRefMultiplicity   (nTracks);
    header->SetRefMultiplicityPos(nPosTracks);
    header->SetRefMultiplicityNeg(nTracks - nPosTracks);
    header->SetMuonMagFieldScale(-999.); // FIXME
    header->SetCentrality(-999.);        // FIXME

    Int_t nV0s      = esd->GetNumberOfV0s();
    Int_t nCascades = esd->GetNumberOfCascades();
    Int_t nKinks    = esd->GetNumberOfKinks();
	Int_t nVertices = nV0s + 2*nCascades /*could lead to two vertices, one V0 and the Xi */+ nKinks + 1 /* = prim. vtx*/;    
	Int_t nJets     = 0;
    Int_t nCaloClus = esd->GetNumberOfCaloClusters();
    Int_t nFmdClus  = 0;
    Int_t nPmdClus  = esd->GetNumberOfPmdTracks();
   
    aod->ResetStd(nTracks, nVertices, nV0s+nCascades, nJets, nCaloClus, nFmdClus, nPmdClus);
    
    // Array to take into account the tracks already added to the AOD
    Bool_t * usedTrack = NULL;
    if (nTracks>0) {
      usedTrack = new Bool_t[nTracks];
      for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) usedTrack[iTrack]=kFALSE;
    }
    // Array to take into account the V0s already added to the AOD
    Bool_t * usedV0 = NULL;
    if (nV0s>0) {
      usedV0 = new Bool_t[nV0s];
      for (Int_t iV0=0; iV0<nV0s; ++iV0) usedV0[iV0]=kFALSE;
    }
    // Array to take into account the kinks already added to the AOD
    Bool_t * usedKink = NULL;
    if (nKinks>0) {
      usedKink = new Bool_t[nKinks];
      for (Int_t iKink=0; iKink<nKinks; ++iKink) usedKink[iKink]=kFALSE;
    }
    
    // Access to the AOD container of vertices
    TClonesArray &vertices = *(aod->GetVertices());
    Int_t jVertices=0;

    // Access to the AOD container of tracks
    TClonesArray &tracks = *(aod->GetTracks());
    Int_t jTracks=0; 
   
    // Access to the AOD container of V0s
    TClonesArray &V0s = *(aod->GetV0s());
    Int_t jV0s=0;
    
    // Add primary vertex. The primary tracks will be defined
    // after the loops on the composite objects (V0, cascades, kinks)
    const AliESDVertex *vtx = esd->GetPrimaryVertex();
      
    vtx->GetXYZ(pos); // position
    vtx->GetCovMatrix(covVtx); //covariance matrix

    AliAODVertex * primary = new(vertices[jVertices++])
      AliAODVertex(pos, covVtx, vtx->GetChi2toNDF(), NULL, -1, AliAODVertex::kPrimary);
         

    AliAODTrack *aodTrack = 0x0;
    
    // Create vertices starting from the most complex objects

    // Cascades
    for (Int_t nCascade = 0; nCascade < nCascades; ++nCascade) {
      AliESDcascade *cascade = esd->GetCascade(nCascade);
      
      cascade->GetXYZ(pos[0], pos[1], pos[2]);
      cascade->GetPosCovXi(covVtx);
     
      // Add the cascade vertex
      AliAODVertex * vcascade = new(vertices[jVertices++]) AliAODVertex(pos,
									covVtx,
									cascade->GetChi2Xi(), // = chi2/NDF since NDF = 2*2-3
									primary,
									nCascade,
									AliAODVertex::kCascade);

      primary->AddDaughter(vcascade); // the cascade 'particle' (represented by a vertex) is added as a daughter to the primary vertex

      // Add the V0 from the cascade. The ESD class have to be optimized...
      // Now we have to search for the corresponding V0 in the list of V0s
      // using the indeces of the positive and negative tracks

      Int_t posFromV0 = cascade->GetPindex();
      Int_t negFromV0 = cascade->GetNindex();


      AliESDv0 * v0 = 0x0;
      Int_t indV0 = -1;

      for (Int_t iV0=0; iV0<nV0s; ++iV0) {

	v0 = esd->GetV0(iV0);
	Int_t posV0 = v0->GetPindex();
	Int_t negV0 = v0->GetNindex();

	if (posV0==posFromV0 && negV0==negFromV0) {
	  indV0 = iV0;
	  break;
	}
      }

      AliAODVertex * vV0FromCascade = 0x0;

      if (indV0>-1 && !usedV0[indV0]) {
	
	// the V0 exists in the array of V0s and is not used

	usedV0[indV0] = kTRUE;
	
	v0->GetXYZ(pos[0], pos[1], pos[2]);
	v0->GetPosCov(covVtx);
	
	vV0FromCascade = new(vertices[jVertices++]) AliAODVertex(pos,
								 covVtx,
								 v0->GetChi2V0(), // = chi2/NDF since NDF = 2*2-3
								 vcascade,
								 indV0,
								 AliAODVertex::kV0);
      } else {

	// the V0 doesn't exist in the array of V0s or was used
	cerr << "Error: event " << iEvent << " cascade " << nCascade
	     << " The V0 " << indV0 
	     << " doesn't exist in the array of V0s or was used!" << endl;

	cascade->GetXYZ(pos[0], pos[1], pos[2]);
	cascade->GetPosCov(covVtx);
      
	vV0FromCascade = new(vertices[jVertices++]) AliAODVertex(pos,
								 covVtx,
								 v0->GetChi2V0(), // = chi2/NDF since NDF = 2*2-3
								 vcascade,
								 indV0,
								 AliAODVertex::kV0);
	vcascade->AddDaughter(vV0FromCascade);

      }

      // Add the positive tracks from the V0

      if (! usedTrack[posFromV0]) {

	usedTrack[posFromV0] = kTRUE;

	AliESDtrack *esdTrack = esd->GetTrack(posFromV0);
	esdTrack->GetPxPyPz(p_pos);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);
	
	vV0FromCascade->AddDaughter(aodTrack =
				    new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(), 
					   p_pos, 
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vV0FromCascade,
					   kTRUE,  // check if this is right
					   kFALSE, // check if this is right
					   AliAODTrack::kSecondary)
		);
	aodTrack->ConvertAliPIDtoAODPID();
      }
      else {
	cerr << "Error: event " << iEvent << " cascade " << nCascade
	     << " track " << posFromV0 << " has already been used!" << endl;
      }

      // Add the negative tracks from the V0

      if (!usedTrack[negFromV0]) {
	
	usedTrack[negFromV0] = kTRUE;
	
	AliESDtrack *esdTrack = esd->GetTrack(negFromV0);
	esdTrack->GetPxPyPz(p_neg);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);
	
	vV0FromCascade->AddDaughter(aodTrack =
                new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(),
					   p_neg,
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vV0FromCascade,
					   kTRUE,  // check if this is right
					   kFALSE, // check if this is right
					   AliAODTrack::kSecondary)
		);
	aodTrack->ConvertAliPIDtoAODPID();
      }
      else {
	cerr << "Error: event " << iEvent << " cascade " << nCascade
	     << " track " << negFromV0 << " has already been used!" << endl;
      }

      // add it to the V0 array as well
      Double_t d0[2] = { -999., -99.};
      // counting is probably wrong
      new(V0s[jV0s++]) AliAODv0(vV0FromCascade, -999., -99., p_pos, p_neg, d0); // to be refined

      // Add the bachelor track from the cascade

      Int_t bachelor = cascade->GetBindex();
      
      if(!usedTrack[bachelor]) {
      
	usedTrack[bachelor] = kTRUE;
	
	AliESDtrack *esdTrack = esd->GetTrack(bachelor);
	esdTrack->GetPxPyPz(p);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);

	vcascade->AddDaughter(aodTrack =
        	new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(),
					   p,
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vcascade,
					   kTRUE,  // check if this is right
					   kFALSE, // check if this is right
					   AliAODTrack::kSecondary)
		);
 	aodTrack->ConvertAliPIDtoAODPID();
     }
      else {
	cerr << "Error: event " << iEvent << " cascade " << nCascade
	     << " track " << bachelor << " has already been used!" << endl;
      }
      
      // Add the primary track of the cascade (if any)
      
    } // end of the loop on cascades
 
    // V0s
        
    for (Int_t nV0 = 0; nV0 < nV0s; ++nV0) {

      if (usedV0[nV0]) continue; // skip if aready added to the AOD

      AliESDv0 *v0 = esd->GetV0(nV0); 
     
      v0->GetXYZ(pos[0], pos[1], pos[2]);
      v0->GetPosCov(covVtx);

      AliAODVertex * vV0 = 
	new(vertices[jVertices++]) AliAODVertex(pos,
						covVtx,
						v0->GetChi2V0(), // = chi2/NDF since NDF = 2*2-3
						primary,
						nV0,
						AliAODVertex::kV0);
      primary->AddDaughter(vV0);

      Int_t posFromV0 = v0->GetPindex();
      Int_t negFromV0 = v0->GetNindex();
      
      // Add the positive tracks from the V0

      if (!usedTrack[posFromV0]) {
	
	usedTrack[posFromV0] = kTRUE;

	AliESDtrack *esdTrack = esd->GetTrack(posFromV0);
	esdTrack->GetPxPyPz(p_pos);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);
	
	vV0->AddDaughter(aodTrack =
        	new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(), 
					   p_pos, 
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vV0,
					   kTRUE,  // check if this is right
					   kFALSE, // check if this is right
					   AliAODTrack::kSecondary)
		);
	aodTrack->ConvertAliPIDtoAODPID();
      }
      else {
	cerr << "Error: event " << iEvent << " V0 " << nV0
	     << " track " << posFromV0 << " has already been used!" << endl;
      }

      // Add the negative tracks from the V0

      if (!usedTrack[negFromV0]) {

	usedTrack[negFromV0] = kTRUE;

	AliESDtrack *esdTrack = esd->GetTrack(negFromV0);
	esdTrack->GetPxPyPz(p_neg);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);

	vV0->AddDaughter(aodTrack =
                new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(),
					   p_neg,
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vV0,
					   kTRUE,  // check if this is right
					   kFALSE, // check if this is right
					   AliAODTrack::kSecondary)
		);
	aodTrack->ConvertAliPIDtoAODPID();
      }
      else {
	cerr << "Error: event " << iEvent << " V0 " << nV0
	     << " track " << negFromV0 << " has already been used!" << endl;
      }

      // add it to the V0 array as well
      Double_t d0[2] = { 999., 99.};
      new(V0s[jV0s++]) AliAODv0(vV0, 999., 99., p_pos, p_neg, d0); // to be refined
    }
	V0s.Expand(jV0s);	 
    // end of the loop on V0s
    
    // Kinks: it is a big mess the access to the information in the kinks
    // The loop is on the tracks in order to find the mother and daugther of each kink


    for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) {

      AliESDtrack * esdTrack = esd->GetTrack(iTrack);

      Int_t ikink = esdTrack->GetKinkIndex(0);

      if (ikink) {
	// Negative kink index: mother, positive: daughter

	// Search for the second track of the kink

	for (Int_t jTrack = iTrack+1; jTrack<nTracks; ++jTrack) {

	  AliESDtrack * esdTrack1 = esd->GetTrack(jTrack);

	  Int_t jkink = esdTrack1->GetKinkIndex(0);

	  if ( TMath::Abs(ikink)==TMath::Abs(jkink) ) {

	    // The two tracks are from the same kink
	  
	    if (usedKink[TMath::Abs(ikink)-1]) continue; // skip used kinks

	    Int_t imother = -1;
	    Int_t idaughter = -1;

	    if (ikink<0 && jkink>0) {

	      imother = iTrack;
	      idaughter = jTrack;
	    }
	    else if (ikink>0 && jkink<0) {

	      imother = jTrack;
	      idaughter = iTrack;
	    }
	    else {
	      cerr << "Error: Wrong combination of kink indexes: "
	      << ikink << " " << jkink << endl;
	      continue;
	    }

	    // Add the mother track

	    AliAODTrack * mother = NULL;

	    if (!usedTrack[imother]) {
	
	      usedTrack[imother] = kTRUE;
	
	      AliESDtrack *esdTrack = esd->GetTrack(imother);
	      esdTrack->GetPxPyPz(p);
	      esdTrack->GetXYZ(pos);
	      esdTrack->GetCovarianceXYZPxPyPz(covTr);
	      esdTrack->GetESDpid(pid);

	      mother = 
		new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(),
					   p,
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   primary,
					   kTRUE, // check if this is right
					   kTRUE, // check if this is right
					   AliAODTrack::kPrimary);
	      primary->AddDaughter(mother);
	      mother->ConvertAliPIDtoAODPID();
	    }
	    else {
	      cerr << "Error: event " << iEvent << " kink " << TMath::Abs(ikink)-1
	      << " track " << imother << " has already been used!" << endl;
	    }

	    // Add the kink vertex
	    AliESDkink * kink = esd->GetKink(TMath::Abs(ikink)-1);

	    AliAODVertex * vkink = 
	    new(vertices[jVertices++]) AliAODVertex(kink->GetPosition(),
						    NULL,
						    0.,
						    mother,
						    esdTrack->GetID(), // This is the track ID of the mother's track!
						    AliAODVertex::kKink);
	    // Add the daughter track

	    AliAODTrack * daughter = NULL;

	    if (!usedTrack[idaughter]) {
	
	      usedTrack[idaughter] = kTRUE;
	
	      AliESDtrack *esdTrack = esd->GetTrack(idaughter);
	      esdTrack->GetPxPyPz(p);
	      esdTrack->GetXYZ(pos);
	      esdTrack->GetCovarianceXYZPxPyPz(covTr);
	      esdTrack->GetESDpid(pid);

	      daughter = 
		new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					   esdTrack->GetLabel(),
					   p,
					   kTRUE,
					   pos,
					   kFALSE,
					   covTr, 
					   (Short_t)esdTrack->Charge(),
					   esdTrack->GetITSClusterMap(), 
					   pid,
					   vkink,
					   kTRUE, // check if this is right
					   kTRUE, // check if this is right
					   AliAODTrack::kPrimary);
	      vkink->AddDaughter(daughter);
	      daughter->ConvertAliPIDtoAODPID();
	    }
	    else {
	      cerr << "Error: event " << iEvent << " kink " << TMath::Abs(ikink)-1
	      << " track " << idaughter << " has already been used!" << endl;
	    }
	  }
	}
      }
    }
    vertices.Expand(jVertices);

    // Tracks (primary and orphan)
    for (Int_t nTrack = 0; nTrack < nTracks; ++nTrack) {

      if (usedTrack[nTrack]) continue;

      AliESDtrack *esdTrack = esd->GetTrack(nTrack);
      esdTrack->GetPxPyPz(p);
      esdTrack->GetXYZ(pos);
      esdTrack->GetCovarianceXYZPxPyPz(covTr);
      esdTrack->GetESDpid(pid);

      Float_t impactXY, impactZ;

      esdTrack->GetImpactParameters(impactXY,impactZ);

      if (impactXY<3.) {
	// track inside the beam pipe
      
	primary->AddDaughter(aodTrack =
	    new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					 esdTrack->GetLabel(),
					 p,
					 kTRUE,
					 pos,
					 kFALSE,
					 covTr, 
					 (Short_t)esdTrack->Charge(),
					 esdTrack->GetITSClusterMap(), 
					 pid,
					 primary,
					 kTRUE, // check if this is right
					 kTRUE, // check if this is right
					 AliAODTrack::kPrimary)
	    );
	aodTrack->ConvertAliPIDtoAODPID();
      }
      else {
	// outside the beam pipe: orphan track
	// Don't write them anymore!
	continue;
      }	
    } // end of loop on tracks
    
    // muon tracks
    Int_t nMuTracks = esd->GetNumberOfMuonTracks();
    for (Int_t nMuTrack = 0; nMuTrack < nMuTracks; ++nMuTrack) {
      
      AliESDMuonTrack *esdMuTrack = esd->GetMuonTrack(nMuTrack);     
      p[0] = esdMuTrack->Px(); 
      p[1] = esdMuTrack->Py(); 
      p[2] = esdMuTrack->Pz();
      pos[0] = primary->GetX(); 
      pos[1] = primary->GetY(); 
      pos[2] = primary->GetZ();
      
      // has to be changed once the muon pid is provided by the ESD
      for (Int_t i = 0; i < 10; pid[i++] = 0.); pid[AliAODTrack::kMuon]=1.;
      
      primary->AddDaughter(aodTrack =
	  new(tracks[jTracks++]) AliAODTrack(0, // no ID provided
					     0, // no label provided
					     p,
					     kTRUE,
					     pos,
					     kFALSE,
					     NULL, // no covariance matrix provided
					     esdMuTrack->Charge(),
					     0, // ITSClusterMap is set below
					     pid,
					     primary,
 					     kFALSE,  // muon tracks are not used to fit the primary vtx
					     kFALSE,  // not used for vertex fit
					     AliAODTrack::kPrimary)
	  );

      aodTrack->SetHitsPatternInTrigCh(esdMuTrack->GetHitsPatternInTrigCh());
      Int_t track2Trigger = esdMuTrack->GetMatchTrigger();
      aodTrack->SetMatchTrigger(track2Trigger);
      if (track2Trigger) 
	aodTrack->SetChi2MatchTrigger(esdMuTrack->GetChi2MatchTrigger());
      else 
	aodTrack->SetChi2MatchTrigger(0.);
    }
    tracks.Expand(jTracks); // remove 'empty slots' due to unwritten tracks
	
    // Access to the AOD container of PMD clusters
    TClonesArray &pmdClusters = *(aod->GetPmdClusters());
    Int_t jPmdClusters=0;
  
    for (Int_t iPmd = 0; iPmd < nPmdClus; ++iPmd) {
      // file pmd clusters, to be revised!
      AliESDPmdTrack *pmdTrack = esd->GetPmdTrack(iPmd);
      Int_t nLabel = 0;
      Int_t *label = 0x0;
      Double_t pos[3] = { pmdTrack->GetClusterX(), pmdTrack->GetClusterY(), pmdTrack->GetClusterZ() };
      Double_t pid[9] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. }; // to be revised!
      // type not set!
      // assoc cluster not set
      new(pmdClusters[jPmdClusters++]) AliAODPmdCluster(iPmd, nLabel, label, pmdTrack->GetClusterADC(), pos, pid);
    }

    // Access to the AOD container of clusters
    TClonesArray &caloClusters = *(aod->GetCaloClusters());
    Int_t jClusters=0;
 
    for (Int_t iClust=0; iClust<nCaloClus; ++iClust) {

      AliESDCaloCluster * cluster = esd->GetCaloCluster(iClust);

      Int_t id = cluster->GetID();
      Int_t nLabel = 0;
      Int_t *label = 0x0;
      Float_t energy = cluster->E();
      cluster->GetPosition(posF);
      Char_t ttype=AliAODCluster::kUndef;

      if (cluster->GetClusterType() == AliESDCaloCluster::kPHOSCluster) {
	ttype=AliAODCluster::kPHOSNeutral;
      } 
      else if (cluster->GetClusterType() == AliESDCaloCluster::kEMCALClusterv1) {
	ttype = AliAODCluster::kEMCALClusterv1;
      }

      
      AliAODCaloCluster *caloCluster = new(caloClusters[jClusters++]) AliAODCaloCluster(id,
											nLabel,
											label,
											energy,
											pos,
											NULL,
											ttype);
      
      caloCluster->SetCaloCluster(); // to be refined!

    } 
    caloClusters.Expand(jClusters); // resize TObjArray to 'remove' slots for pseudo clusters	 
    // end of loop on calo clusters

    // fill PHOS cell info

    AliESDCaloCells &esdEMcells = *(esd->GetEMCALCells());
    Int_t nEMcell = esdEMcells.GetNumberOfCells() ;

    AliAODCaloCells &aodEMcells = *(aod->GetEMCALCells());
    aodEMcells.CreateContainer(nEMcell);
    aodEMcells.SetType(AliAODCaloCells::kEMCAL);
    for (Int_t iCell = 0; iCell < nEMcell; iCell++) {      
      aodEMcells.SetCell(iCell,esdEMcells.GetCellNumber(iCell),esdEMcells.GetAmplitude(iCell));
    }
    aodEMcells.Sort();

    // fill PHOS cell info

    AliESDCaloCells &esdPHcells = *(esd->GetPHOSCells());
    Int_t nPHcell = esdPHcells.GetNumberOfCells() ;

    AliAODCaloCells &aodPHcells = *(aod->GetPHOSCells());
    aodPHcells.CreateContainer(nPHcell);
    aodPHcells.SetType(AliAODCaloCells::kPHOS);
    for (Int_t iCell = 0; iCell < nPHcell; iCell++) {      
      aodPHcells.SetCell(iCell,esdPHcells.GetCellNumber(iCell),esdPHcells.GetAmplitude(iCell));
    }
    aodPHcells.Sort();

    // tracklets    
    AliAODTracklets &SPDTracklets = *(aod->GetTracklets());
    const AliMultiplicity *mult = esd->GetMultiplicity();
    if (mult) {
      if (mult->GetNumberOfTracklets()>0) {
	SPDTracklets.CreateContainer(mult->GetNumberOfTracklets());

	for (Int_t n=0; n<mult->GetNumberOfTracklets(); n++) {
	  SPDTracklets.SetTracklet(n, mult->GetTheta(n), mult->GetPhi(n), mult->GetDeltaPhi(n), mult->GetLabel(n));
	}
      }
    } else {
      Printf("ERROR: AliMultiplicity could not be retrieved from ESD");
    }

    delete [] usedTrack;
    delete [] usedV0;
    delete [] usedKink;

    // fill the tree for this event
    aodTree->Fill();
  } // end of event loop

  aodTree->GetUserInfo()->Add(aod);

  // write the tree to the specified file
  aodFile = aodTree->GetCurrentFile();
  aodFile->cd();
  aodTree->Write();

  return;
}

void AliReconstruction::WriteAlignmentData(AliESDEvent* esd)
{
  // Write space-points which are then used in the alignment procedures
  // For the moment only ITS, TRD and TPC

  // Load TOF clusters
  if (fTracker[3]){
    fLoader[3]->LoadRecPoints("read");
    TTree* tree = fLoader[3]->TreeR();
    if (!tree) {
      AliError(Form("Can't get the %s cluster tree", fgkDetectorName[3]));
      return;
    }
    fTracker[3]->LoadClusters(tree);
  }
  Int_t ntracks = esd->GetNumberOfTracks();
  for (Int_t itrack = 0; itrack < ntracks; itrack++)
    {
      AliESDtrack *track = esd->GetTrack(itrack);
      Int_t nsp = 0;
      Int_t idx[200];
      for (Int_t iDet = 3; iDet >= 0; iDet--)
	nsp += track->GetNcls(iDet);
      if (nsp) {
	AliTrackPointArray *sp = new AliTrackPointArray(nsp);
	track->SetTrackPointArray(sp);
	Int_t isptrack = 0;
	for (Int_t iDet = 3; iDet >= 0; iDet--) {
	  AliTracker *tracker = fTracker[iDet];
	  if (!tracker) continue;
	  Int_t nspdet = track->GetNcls(iDet);
	  if (nspdet <= 0) continue;
	  track->GetClusters(iDet,idx);
	  AliTrackPoint p;
	  Int_t isp = 0;
	  Int_t isp2 = 0;
	  while (isp < nspdet) {
	    Bool_t isvalid;
	    if(IsSelected(fgkDetectorName[iDet],fUseTrackingErrorsForAlignment)) {
	      isvalid = tracker->GetTrackPointTrackingError(idx[isp2],p,track);
	    } else {
	      isvalid = tracker->GetTrackPoint(idx[isp2],p); 
	    } 
	    isp2++;
	    const Int_t kNTPCmax = 159;
	    if (iDet==1 && isp2>kNTPCmax) break;   // to be fixed
	    if (!isvalid) continue;
	    sp->AddPoint(isptrack,&p); isptrack++; isp++;
	  }
	}	
      }
    }
  if (fTracker[3]){
    fTracker[3]->UnloadClusters();
    fLoader[3]->UnloadRecPoints();
  }
}

//_____________________________________________________________________________
void AliReconstruction::FillRawDataErrorLog(Int_t iEvent, AliESDEvent* esd)
{
  // The method reads the raw-data error log
  // accumulated within the rawReader.
  // It extracts the raw-data errors related to
  // the current event and stores them into
  // a TClonesArray inside the esd object.

  if (!fRawReader) return;

  for(Int_t i = 0; i < fRawReader->GetNumberOfErrorLogs(); i++) {

    AliRawDataErrorLog *log = fRawReader->GetErrorLog(i);
    if (!log) continue;
    if (iEvent != log->GetEventNumber()) continue;

    esd->AddRawDataErrorLog(log);
  }

}

TNamed* AliReconstruction::CopyFileToTNamed(TString fPath,TString fName){
  // Dump a file content into a char in TNamed
  ifstream in;
  in.open(fPath.Data(),ios::in | ios::binary|ios::ate);
  Int_t kBytes = (Int_t)in.tellg();
  printf("Size: %d \n",kBytes);
  TNamed *fn = 0;
  if(in.good()){
    char* memblock = new char [kBytes];
    in.seekg (0, ios::beg);
    in.read (memblock, kBytes);
    in.close();
    TString fData(memblock,kBytes);
    fn = new TNamed(fName,fData);
    printf("fData Size: %d \n",fData.Sizeof());
    printf("fName Size: %d \n",fName.Sizeof());
    printf("fn    Size: %d \n",fn->Sizeof());
    delete[] memblock;
  }
  else{
    AliInfo(Form("Could not Open %s\n",fPath.Data()));
  }

  return fn;
}

void AliReconstruction::TNamedToFile(TTree* fTree, TString fName){
  // This is not really needed in AliReconstruction at the moment
  // but can serve as a template

  TList *fList = fTree->GetUserInfo();
  TNamed *fn = (TNamed*)fList->FindObject(fName.Data());
  printf("fn Size: %d \n",fn->Sizeof());

  TString fTmp(fn->GetName()); // to be 100% sure in principle fName also works
  const char* cdata = fn->GetTitle();
  printf("fTmp Size %d\n",fTmp.Sizeof());

  int size = fn->Sizeof()-fTmp.Sizeof()-sizeof(UChar_t)-sizeof(Int_t); // see dfinition of TString::SizeOf()...
  printf("calculated size %d\n",size);
  ofstream out(fName.Data(),ios::out | ios::binary);
  out.write(cdata,size);
  out.close();

}
  
//_____________________________________________________________________________
AliQADataMaker * AliReconstruction::GetQADataMaker(Int_t iDet)
{
 // get the quality assurance data maker object and the loader for a detector

  if (fQADataMaker[iDet]) 
    return fQADataMaker[iDet];

  // load the QA data maker object
  TPluginManager* pluginManager = gROOT->GetPluginManager();
  TString detName = fgkDetectorName[iDet];
  TString qadmName = "Ali" + detName + "QADataMaker";
  if (gAlice && !gAlice->GetDetector(detName) && (detName != "HLT")) 
    return NULL;

  AliQADataMaker * qadm = NULL;
  // first check if a plugin is defined for the quality assurance data maker
  TPluginHandler* pluginHandler = pluginManager->FindHandler("AliQADataMaker", detName);
  // if not, add a plugin for it
  if (!pluginHandler) {
    AliDebug(1, Form("defining plugin for %s", qadmName.Data()));
    TString libs = gSystem->GetLibraries();
    if (libs.Contains("lib" + detName + "base.so") ||
	(gSystem->Load("lib" + detName + "base.so") >= 0)) {
      pluginManager->AddHandler("AliQADataMaker", detName, 
				qadmName, detName + "qadm", qadmName + "()");
    } else {
      pluginManager->AddHandler("AliQADataMaker", detName, 
				qadmName, detName, qadmName + "()");
    }
    pluginHandler = pluginManager->FindHandler("AliQADataMaker", detName);
  }
  if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) {
    qadm = (AliQADataMaker *) pluginHandler->ExecPlugin(0);
  }
  if (qadm) {
    AliInfo(Form("Initializing quality assurance data maker for %s", fgkDetectorName[iDet]));
    qadm->Init(AliQA::kRECPOINTS, AliCDBManager::Instance()->GetRun(), GetQACycles(fgkDetectorName[iDet]));
    qadm->StartOfCycle(AliQA::kRECPOINTS);
    qadm->Init(AliQA::kESDS, AliCDBManager::Instance()->GetRun());
    qadm->StartOfCycle(AliQA::kESDS, "same") ; 	
    fQADataMaker[iDet] = qadm;
  }

  return qadm;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunQA(const char* detectors, AliESDEvent *& esd)
{
  // run the Quality Assurance data producer

  AliCodeTimerAuto("")
  TString detStr = detectors;
  for (Int_t iDet = 0; iDet < fgkNDetectors; iDet++) {
   if (!IsSelected(fgkDetectorName[iDet], detStr)) 
     continue;
   AliQADataMaker * qadm = GetQADataMaker(iDet);
   if (!qadm) 
     continue;
   AliCodeTimerStart(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
   AliInfo(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
    
   qadm->Exec(AliQA::kESDS, esd) ; 
   qadm->Increment() ; 

   AliCodeTimerStop(Form("running quality assurance data maker for %s", fgkDetectorName[iDet]));
 }
 if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
   AliError(Form("the following detectors were not found: %s",
		 detStr.Data()));
   if (fStopOnError) 
     return kFALSE;
 }
 
 return kTRUE;
  
}

//_____________________________________________________________________________
void AliReconstruction::CheckQA()
{
// check the QA of SIM for this run and remove the detectors 
// with status Fatal
  
	TString newRunLocalReconstruction ; 
	TString newRunTracking ;
	TString newFillESD ;
	 
	for (Int_t iDet = 0; iDet < AliQA::kNDET; iDet++) {
		TString detName(AliQA::GetDetName(iDet)) ;
		AliQA * qa = AliQA::Instance(AliQA::DETECTORINDEX(iDet)) ; 
		if ( qa->IsSet(AliQA::DETECTORINDEX(iDet), AliQA::kSIM, AliQA::kFATAL)) {
				AliInfo(Form("QA status for %s in Hits and/or SDIGITS  and/or Digits was Fatal; No reconstruction performed", detName.Data())) ;
		} else {
			if ( fRunLocalReconstruction.Contains(AliQA::GetDetName(iDet)) || 
					fRunLocalReconstruction.Contains("ALL") )  {
				newRunLocalReconstruction += detName ; 
				newRunLocalReconstruction += " " ; 			
			}
			if ( fRunTracking.Contains(AliQA::GetDetName(iDet)) || 
					fRunTracking.Contains("ALL") )  {
				newRunTracking += detName ; 
				newRunTracking += " " ; 			
			}
			if ( fFillESD.Contains(AliQA::GetDetName(iDet)) || 
					fFillESD.Contains("ALL") )  {
				newFillESD += detName ; 
				newFillESD += " " ; 			
			}
		}
	}
	fRunLocalReconstruction = newRunLocalReconstruction ; 
	fRunTracking            = newRunTracking ; 
	fFillESD                = newFillESD ; 
}

//_____________________________________________________________________________
Int_t AliReconstruction::GetDetIndex(const char* detector)
{
  // return the detector index corresponding to detector
  Int_t index = -1 ; 
  for (index = 0; index < fgkNDetectors ; index++) {
    if ( strcmp(detector, fgkDetectorName[index]) == 0 )
	break ; 
  }	
  return index ; 
}

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

/* $Id: */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for running the Quality Assurance Checker
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliCDBEntry.h"
#include "AliQAManager.h"
#include "AliCDBStorage.h"
#include "AliRunInfo.h" 
#include "AliLog.h"
#include "AliModule.h" 
#include "AliQAv1.h"
#include "AliQAChecker.h"
#include "AliQACheckerBase.h"
#include "AliCorrQAChecker.h"
#include "AliGlobalQAChecker.h"
#include "AliGRPObject.h"

#include <TKey.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TPluginManager.h> 
#include <TROOT.h>
#include <TStopwatch.h> 
#include <TString.h> 
#include <TSystem.h> 
#include <TList.h>
#include <TNtupleD.h>

ClassImp(AliQAChecker)
  AliQAChecker * AliQAChecker::fgQAChecker = 0x0 ;

//_____________________________________________________________________________
AliQAChecker::AliQAChecker(const char* name, const char* title) :
  TNamed(name, title),
  fDataFile(0x0), 
  fRunInfo(0x0), 
  fRunInfoOwner(kFALSE), 
  fRefFile(0x0), 
  fFoundDetectors("."), 
  fEventSpecie(AliRecoParam::kDefault) 
{
  // ctor: initialise checkers and open the data file   
  for (Int_t det = 0 ; det < AliQAv1::kNDET ; det++) 
    fCheckers[det] = NULL ; 
}

//_____________________________________________________________________________
AliQAChecker::AliQAChecker(const AliQAChecker& qac) :
  TNamed(qac),
  fDataFile(qac.fDataFile), 
  fRunInfo(qac.fRunInfo), 
  fRunInfoOwner(kFALSE),   
  fRefFile(qac.fRefFile), 
  fFoundDetectors(qac.fFoundDetectors),
  fEventSpecie(qac.fEventSpecie)
{
  // copy constructor
  
  for (Int_t det = 0 ; det < AliQAv1::kNDET ; det++) 
    fCheckers[det] = NULL ; 
}

//_____________________________________________________________________________
AliQAChecker& AliQAChecker::operator = (const AliQAChecker& qac)
{
// assignment operator

  this->~AliQAChecker();
  new(this) AliQAChecker(qac);
  return *this;
}

//_____________________________________________________________________________
AliQAChecker::~AliQAChecker()
{
// clean up
  if (fRunInfo)
    delete fRunInfo ; 
  delete [] fCheckers ; 
  AliQAv1::Close() ; 
}

//_____________________________________________________________________________
  AliQACheckerBase * AliQAChecker::GetDetQAChecker(Int_t det)
{
	// Gets the Quality Assurance checker for the detector specified by its name
	
	if (fCheckers[det]) 
    return fCheckers[det];

	AliQACheckerBase * qac = NULL ;

	TString detName(AliQAv1::GetDetName(det)) ; 
	
	if (det == AliQAv1::kGLOBAL) {
		qac = new AliGlobalQAChecker() ; 
	} else if (det == AliQAv1::kCORR) {
		qac = new AliCorrQAChecker() ; 
	} else {
		AliDebug(AliQAv1::GetQADebugLevel(), Form("Retrieving QA checker for %s", detName.Data())) ; 
		TPluginManager* pluginManager = gROOT->GetPluginManager() ;
		TString qacName = "Ali" + detName + "QAChecker" ;

		// first check if a plugin is defined for the quality assurance checker
		TPluginHandler* pluginHandler = pluginManager->FindHandler("AliQAChecker", detName.Data());
		// if not, add a plugin for it
		if (!pluginHandler) {
			//AliDebug(AliQAv1::GetQADebugLevel(), Form("defining plugin for %s", qacName.Data()));
			TString libs = gSystem->GetLibraries();
		
			if (libs.Contains("lib" + detName + "base.so") || (gSystem->Load("lib" + detName + "base.so") >= 0))
				pluginManager->AddHandler("AliQAChecker", detName, qacName, detName + "qac", qacName + "()");
			else 
				pluginManager->AddHandler("AliQAChecker", detName, qacName, detName, qacName + "()");

			pluginHandler = pluginManager->FindHandler("AliQAChecker", detName);	

			if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) 
				qac = (AliQACheckerBase *) pluginHandler->ExecPlugin(0);
  
		}
	}
	if (qac) 
		fCheckers[det] = qac ;
	
	return qac ; 
}
 
//_____________________________________________________________________________
void AliQAChecker::GetRefSubDir(const char * det, const char * task, TDirectory *& dirFile, TObjArray **& dirOCDB)     
{ 
  // Opens and returns the file with the reference data 
  dirFile = NULL ; 
  TString refStorage(AliQAv1::GetQARefStorage()) ;
//  if (refStorage.Contains(AliQAv1::GetLabLocalFile())) {	
//    refStorage.ReplaceAll(AliQAv1::GetLabLocalFile(), "") ; 
//    refStorage += AliQAv1::GetQARefFileName() ;
//    if ( fRefFile ) 
//      if ( fRefFile->IsOpen() ) 
//					fRefFile->Close() ; 
//    fRefFile = TFile::Open(refStorage.Data()) ; 
//    if (!fRefFile) { 
//      AliError(Form("Cannot find reference file %s", refStorage.Data())) ; 
//      dirFile = NULL ; 
//    }
//    dirFile = fRefFile->GetDirectory(det) ; 
//    if (!dirFile) {
//      AliWarning(Form("Directory %s not found in %d", det, refStorage.Data())) ; 
//    } else {
//			dirFile = dirFile->GetDirectory(task) ; 
//      if (!dirFile) 
//				AliWarning(Form("Directory %s/%s not found in %s", det, task, refStorage.Data())) ; 
//    }  
//  } else 
  if (!refStorage.Contains(AliQAv1::GetLabLocalOCDB()) && !refStorage.Contains(AliQAv1::GetLabAliEnOCDB())) {
    AliError(Form("%s is not a valid location for reference data", refStorage.Data())) ; 
    return ; 
  } else {
    AliQAManager* manQA = AliQAManager::QAManager() ;
    dirOCDB = new TObjArray*[AliRecoParam::kNSpecies] ;	
    for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
      dirOCDB[specie] = 0x0;
      if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) 
        continue ; 
      //if ( strcmp(AliQAv1::GetRefDataDirName(), "") == 0 ) { // the name of the last level of the directory is not set (EventSpecie)
        // Get it from RunInfo
        //if (!fRunInfo)  // not yet set, get the info from GRP
        //  LoadRunInfoFromGRP() ; 
      AliQAv1::SetQARefDataDirName(specie) ;
      //}
      if ( ! manQA->GetLock() ) { 
        manQA->SetDefaultStorage(AliQAv1::GetQARefStorage()) ; 
        manQA->SetSpecificStorage("*", AliQAv1::GetQARefStorage()) ;
        manQA->SetRun(AliCDBManager::Instance()->GetRun()) ; 
        manQA->SetLock() ; 
      }
      char * detOCDBDir = Form("%s/%s/%s", det, AliQAv1::GetRefOCDBDirName(), AliQAv1::GetRefDataDirName()) ; 
      AliCDBEntry * entry = manQA->Get(detOCDBDir, manQA->GetRun()) ;
      if (entry) {
        TList * listDetQAD =static_cast<TList *>(entry->GetObject()) ;
        if ( strcmp(listDetQAD->ClassName(), "TList") != 0 ) {
          AliError(Form("Expected a Tlist and found a %s for detector %s", listDetQAD->ClassName(), det)) ; 
          continue ; 
        }       
        TIter next(listDetQAD) ;
        TObjArray * ar ; 
        while ( (ar = (TObjArray*)next()) )
          if ( listDetQAD ) 
          dirOCDB[specie] = static_cast<TObjArray *>(listDetQAD->FindObject(Form("%s/%s", task, AliRecoParam::GetEventSpecieName(specie)))) ; 
      }
    }
  }
}

//_____________________________________________________________________________
AliQAChecker * AliQAChecker::Instance()
{
	// returns unique instance of the checker
  if ( ! fgQAChecker ) 
   fgQAChecker = new AliQAChecker() ; 
  return fgQAChecker ;  
}

//_____________________________________________________________________________
void AliQAChecker::LoadRunInfoFromGRP() 
{
  AliCDBManager* man = AliCDBManager::Instance() ;
  AliCDBEntry* entry = man->Get(AliQAv1::GetGRPPath().Data());
  AliGRPObject* grpObject = 0x0;
  if (entry) {

	  TMap* m = static_cast<TMap*>(entry->GetObject());  // old GRP entry

	  if (m) {
	    AliDebug(AliQAv1::GetQADebugLevel(), "It is a map");
	    //m->Print();
	    grpObject = new AliGRPObject();
	         grpObject->ReadValuesFromMap(m);
    }

    else {
	    AliDebug(AliQAv1::GetQADebugLevel(), "It is a new GRP object");
        grpObject = static_cast<AliGRPObject*>(entry->GetObject());  // new GRP entry
    }

    entry->SetOwner(0);
    AliCDBManager::Instance()->UnloadFromCache("GRP/GRP/Data");
  }

  if (!grpObject) {
     AliFatal("No GRP entry found in OCDB!");
  }

  TString lhcState = grpObject->GetLHCState();
  if (lhcState==AliGRPObject::GetInvalidString()) {
    AliError("GRP/GRP/Data entry:  missing value for the LHC state ! Using UNKNOWN");
    lhcState = "UNKNOWN";
  }

  TString beamType = grpObject->GetBeamType();
  if (beamType==AliGRPObject::GetInvalidString()) {
    AliError("GRP/GRP/Data entry:  missing value for the beam type ! Using UNKNOWN");
    beamType = "UNKNOWN";
  }

  Float_t beamEnergy = grpObject->GetBeamEnergy();
  if (beamEnergy==AliGRPObject::GetInvalidFloat()) {
    AliError("GRP/GRP/Data entry:  missing value for the beam energy ! Using 0");
    beamEnergy = 0;
  }

  TString runType = grpObject->GetRunType();
  if (runType==AliGRPObject::GetInvalidString()) {
    AliError("GRP/GRP/Data entry:  missing value for the run type ! Using UNKNOWN");
    runType = "UNKNOWN";
  }

  Int_t activeDetectors = grpObject->GetDetectorMask();
  if (activeDetectors==AliGRPObject::GetInvalidInt()) {
    AliError("GRP/GRP/Data entry:  missing value for the detector mask ! Using 1074790399");
    activeDetectors = 1074790399;
  }

  fRunInfo = new AliRunInfo(lhcState, beamType, beamEnergy, runType, activeDetectors);

  fRunInfoOwner = kTRUE ; 

  // set the event specie
  fEventSpecie = AliRecoParam::kDefault ;
  if (strcmp(runType,"PHYSICS")) {
    // Not a physics run, the event specie is set to kCalib
    fEventSpecie = AliRecoParam::kCalib ;
    return;
  }
  if (strcmp(lhcState,"STABLE_BEAMS") == 0) {
    // Heavy ion run (any beam tha is not pp, the event specie is set to kHighMult
    fEventSpecie = AliRecoParam::kHighMult ;
    if ((strcmp(beamType,"p-p") == 0) ||
        (strcmp(beamType,"p-")  == 0) ||
        (strcmp(beamType,"-p")  == 0) ||
        (strcmp(beamType,"P-P") == 0) ||
        (strcmp(beamType,"P-")  == 0) ||
        (strcmp(beamType,"-P")  == 0)) {
      // Proton run, the event specie is set to kLowMult
      fEventSpecie = AliRecoParam::kLowMult ;
    }
    else if (strcmp(beamType,"-") == 0) {
      // No beams, we assume cosmic data
      fEventSpecie = AliRecoParam::kCosmic ;
    }
    else if (strcmp(beamType,"UNKNOWN") == 0) {
      // No LHC beam information is available, we use the default event specie
      fEventSpecie = AliRecoParam::kDefault ;
    }
  }
}

//_____________________________________________________________________________
Bool_t AliQAChecker::Run(const char * fileName)
{
  // run the Quality Assurance Checker for all tasks Hits, SDigits, Digits, DigitsR, RecPoints, TrackSegments, RecParticles and ESDs
  // starting from data in file  
  TStopwatch stopwatch;
  stopwatch.Start();

  //search for all detectors QA directories
  TList * detKeyList = AliQAv1::GetQADataFile(fileName)->GetListOfKeys() ; 
  TIter nextd(detKeyList) ; 
  TKey * detKey ; 
  while ( (detKey = static_cast<TKey *>(nextd()) ) ) {
    AliDebug(AliQAv1::GetQADebugLevel(), Form("Found %s", detKey->GetName())) ;
    //Check which detector
    TString detName ; 
    TString detNameQA(detKey->GetName()) ; 
    Int_t det ; 
    for ( det = 0; det < AliQAv1::kNDET ; det++) {
      detName = AliQAv1::GetDetName(det) ; 
      if (detNameQA.Contains(detName)) {
        fFoundDetectors+=detName ; 
        fFoundDetectors+="." ;		
        break ; 
      }
    } 
    TDirectory * detDir = AliQAv1::GetQADataFile(fileName)->GetDirectory(detKey->GetName()) ; 
    TList * taskKeyList = detDir->GetListOfKeys() ;
    TIter nextt(taskKeyList) ; 
    TKey * taskKey ; 
    // now search for the tasks dir
    while ( (taskKey = static_cast<TKey *>(nextt()) ) ) {
      TString taskName( taskKey->GetName() ) ; 
      AliDebug(AliQAv1::GetQADebugLevel(), Form("Found %s", taskName.Data())) ;
      TDirectory * taskDir = detDir->GetDirectory(taskName.Data()) ; 
      taskDir->cd() ; 
      AliQACheckerBase * qac = GetDetQAChecker(det) ; 
      if (qac)
        AliDebug(AliQAv1::GetQADebugLevel(), Form("QA checker found for %s", detName.Data())) ; 
      if (!qac)
        AliFatal(Form("QA checker not found for %s", detName.Data())) ; 
      AliQAv1::ALITASK_t index = AliQAv1::kNULLTASK ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kHITS) ) 
        index = AliQAv1::kSIM ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kSDIGITS) ) 
        index = AliQAv1::kSIM ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kDIGITS) ) 
        index = AliQAv1::kSIM ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kDIGITSR) ) 
        index = AliQAv1::kREC ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kRECPOINTS) ) 
        index = AliQAv1::kREC ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kTRACKSEGMENTS) ) 
        index = AliQAv1::kREC ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kRECPARTICLES) ) 
        index = AliQAv1::kREC ; 
      if ( taskName == AliQAv1::GetTaskName(AliQAv1::kESDS) ) 
        index = AliQAv1::kESD ; 
      qac->Init(AliQAv1::DETECTORINDEX_t(det)) ; 
      
      TDirectory * refDir     = NULL ; 
      TObjArray ** refOCDBDir = NULL ;	
      GetRefSubDir(detNameQA.Data(), taskName.Data(), refDir, refOCDBDir) ;
		  qac->SetRefandData(refDir, refOCDBDir, taskDir) ;
		  qac->Run(index) ; 
    }
  }
  AliInfo("QA performed for following detectors:") ; 
  for ( Int_t det = 0; det < AliQAv1::kNDET; det++) {
    if (fFoundDetectors.Contains(AliQAv1::GetDetName(det))) {
      AliInfoClass(Form("%s, ",AliQAv1::GetDetName(det))) ; 
      fFoundDetectors.ReplaceAll(AliQAv1::GetDetName(det), "") ; 
    }	
  }
  printf("\n") ; 
  return kTRUE ; 
}

//_____________________________________________________________________________
Bool_t AliQAChecker::Run(AliQAv1::DETECTORINDEX_t det, AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
	// run the Quality Assurance Checker for detector det, for task task starting from data in list

	AliQACheckerBase * qac = GetDetQAChecker(det) ; 
	if (qac)
		AliDebug(AliQAv1::GetQADebugLevel(), Form("QA checker found for %s", AliQAv1::GetDetName(det).Data())) ;
	if (!qac)
		AliError(Form("QA checker not found for %s", AliQAv1::GetDetName(det).Data())) ; 
  
	AliQAv1::ALITASK_t index = AliQAv1::kNULLTASK ; 
	if ( task == AliQAv1::kRAWS ) 
		index = AliQAv1::kRAW ; 
	else if ( task == AliQAv1::kHITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kSDIGITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kDIGITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kDIGITSR ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kRECPOINTS ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kTRACKSEGMENTS ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kRECPARTICLES ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kESDS ) 
		index = AliQAv1::kESD ; 

	TDirectory * refDir     = NULL ; 
	TObjArray ** refOCDBDir = NULL  ;	
  qac->Init(det) ; 
  GetRefSubDir(AliQAv1::GetDetName(det), AliQAv1::GetTaskName(task), refDir, refOCDBDir) ;
  qac->SetRefandData(refDir, refOCDBDir) ; 
  qac->Run(index, list) ; 
	return kTRUE ; 
}

//_____________________________________________________________________________
Bool_t AliQAChecker::Run(AliQAv1::DETECTORINDEX_t det, AliQAv1::TASKINDEX_t task, TNtupleD ** list)
{
	// run the Quality Assurance Checker for detector det, for task task starting from data in list
  
	AliQACheckerBase * qac = GetDetQAChecker(det) ; 
	if (qac)
		AliDebug(AliQAv1::GetQADebugLevel(), Form("QA checker found for %s", AliQAv1::GetDetName(det).Data())) ;
	if (!qac)
		AliError(Form("QA checker not found for %s", AliQAv1::GetDetName(det).Data())) ; 
  
	AliQAv1::ALITASK_t index = AliQAv1::kNULLTASK ; 
	if ( task == AliQAv1::kRAWS ) 
		index = AliQAv1::kRAW ; 
	else if ( task == AliQAv1::kHITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kSDIGITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kDIGITS ) 
		index = AliQAv1::kSIM ; 
	else if ( task == AliQAv1::kDIGITSR ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kRECPOINTS ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kTRACKSEGMENTS ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kRECPARTICLES ) 
		index = AliQAv1::kREC ; 
	else if ( task == AliQAv1::kESDS ) 
		index = AliQAv1::kESD ; 
  
	TDirectory * refDir     = NULL ; 
	TObjArray ** refOCDBDir = NULL ;	
  qac->Init(det) ; 
  GetRefSubDir(AliQAv1::GetDetName(det), AliQAv1::GetTaskName(task), refDir, refOCDBDir) ;
  qac->SetRefandData(refDir, refOCDBDir) ; 
  qac->Run(index, list) ; 
  return kTRUE ; 
}

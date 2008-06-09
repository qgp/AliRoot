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
// class for running the QA makers                                           //
//                                                                           //
//   AliQADataMakerSteer qas;                                                //
//   qas.Run(AliQA::kRAWS, rawROOTFileName);                                 //
//   qas.Run(AliQA::kHITS);                                                  //
//   qas.Run(AliQA::kSDIGITS);                                               //
//   qas.Run(AliQA::kDIGITS);                                                //
//   qas.Run(AliQA::kRECPOINTS);                                             //
//   qas.Run(AliQA::kESDS);                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TKey.h>
#include <TFile.h>
#include <TFileMerger.h>
#include <TPluginManager.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>

#include "AliCDBManager.h"
#include "AliCDBEntry.h"
#include "AliCDBId.h"
#include "AliCDBMetaData.h"
#include "AliESDEvent.h"
#include "AliGeomManager.h"
#include "AliHeader.h"
#include "AliLog.h"
#include "AliModule.h"
#include "AliQA.h"
#include "AliQADataMakerRec.h"
#include "AliQADataMakerSim.h"
#include "AliQADataMakerSteer.h" 
#include "AliRawReaderDate.h"
#include "AliRawReaderFile.h"
#include "AliRawReaderRoot.h"
#include "AliRun.h"
#include "AliRunLoader.h"

ClassImp(AliQADataMakerSteer) 

//_____________________________________________________________________________
AliQADataMakerSteer::AliQADataMakerSteer(const char* gAliceFilename, const char * name, const char * title) :
	TNamed(name, title), 
	fCurrentEvent(0),  
	fCycleSame(kFALSE),
	fDetectors("ALL"), 
	fDetectorsW("ALL"), 
	fESD(NULL), 
	fESDTree(NULL),
	fFirst(kTRUE),  
	fGAliceFileName(gAliceFilename), 
	fFirstEvent(0),        
	fMaxEvents(0),        
	fNumberOfEvents(999999), 
    fRunNumber(0), 
	fRawReader(NULL), 
	fRawReaderDelete(kTRUE), 
	fRunLoader(NULL),
	fQADataMakers()

{
	// default ctor
	fMaxEvents = fNumberOfEvents ; 
	for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
		if (IsSelected(AliQA::GetDetName(iDet))) {
			fLoader[iDet]      = NULL ;
			fQADataMaker[iDet] = NULL ;
			fQACycles[iDet]    = 999999 ;
		}
	}	
}

//_____________________________________________________________________________
AliQADataMakerSteer::AliQADataMakerSteer(const AliQADataMakerSteer & qas) : 
	TNamed(qas), 
	fCurrentEvent(qas.fCurrentEvent),  
	fCycleSame(kFALSE),
	fDetectors(qas.fDetectors), 
	fDetectorsW(qas.fDetectorsW), 
	fESD(NULL), 
	fESDTree(NULL), 
	fFirst(qas.fFirst),  
	fGAliceFileName(qas.fGAliceFileName), 
	fFirstEvent(qas.fFirstEvent),        
	fMaxEvents(qas.fMaxEvents),        
	fNumberOfEvents(qas.fNumberOfEvents), 
    fRunNumber(qas.fRunNumber), 
	fRawReader(NULL), 
	fRawReaderDelete(kTRUE), 
	fRunLoader(NULL),
	fQADataMakers()
{
	// cpy ctor
	for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
		fLoader[iDet]      = qas.fLoader[iDet] ;
		fQADataMaker[iDet] = qas.fQADataMaker[iDet] ;
		fQACycles[iDet]    = qas.fQACycles[iDet] ;	
	}
}

//_____________________________________________________________________________
AliQADataMakerSteer & AliQADataMakerSteer::operator = (const AliQADataMakerSteer & qas) 
{
	// assignment operator
  this->~AliQADataMakerSteer() ;
  new(this) AliQADataMakerSteer(qas) ;
  return *this ;
}

//_____________________________________________________________________________
AliQADataMakerSteer::~AliQADataMakerSteer() 
{
	// dtor
  for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
	  if (IsSelected(AliQA::GetDetName(iDet))) {
		  fLoader[iDet] = NULL;
		  if (fQADataMaker[iDet]) {
			  (fQADataMaker[iDet])->Finish() ; 
			  delete fQADataMaker[iDet] ;
			  fQADataMaker[iDet] = NULL ;
		  }
	  }
  }

  if (fRawReaderDelete) { 
	fRunLoader = NULL ;
	delete fRawReader ;
	fRawReader = NULL ;
  }
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::DoIt(const AliQA::TASKINDEX_t taskIndex, const char * mode)
{
	// Runs all the QA data Maker for every detector
		
	Bool_t rv = kFALSE ;
    // Fill QA data in event loop 
	for (UInt_t iEvent = fFirstEvent ; iEvent < (UInt_t)fMaxEvents ; iEvent++) {
		fCurrentEvent++ ; 
		// Get the event
		if ( iEvent%10 == 0  ) 
			AliInfo(Form("processing event %d", iEvent));
		if ( taskIndex == AliQA::kRAWS ) {
			if ( !fRawReader->NextEvent() )
				break ;
		} else if ( taskIndex == AliQA::kESDS ) {
			if ( fESDTree->GetEntry(iEvent) == 0 )
				break ;
		} else {
			if ( fRunLoader->GetEvent(iEvent) != 0 )
				break ;
		}
		// loop  over active loaders
		for (Int_t i = 0; i < fQADataMakers.GetEntriesFast() ; i++) {
			AliQADataMaker * qadm = static_cast<AliQADataMaker *>(fQADataMakers.At(i)) ;
			if ( qadm->IsCycleDone() ) {
				qadm->EndOfCycle(AliQA::kRAWS) ;
				qadm->StartOfCycle(AliQA::kRAWS) ;
			}
			TTree * data ; 
			Int_t iDet = qadm->GetUniqueID();
			AliLoader* loader = GetLoader(iDet);
			switch (taskIndex) {
				case AliQA::kRAWS :
					qadm->Exec(taskIndex, fRawReader) ; 
					break ; 
				case AliQA::kHITS :
					loader->LoadHits() ; 
					data = loader->TreeH() ; 
					if ( ! data ) {
						AliWarning(Form(" Hit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
					} else {
						qadm->Exec(taskIndex, data) ;
					} 
					break ;
					case AliQA::kSDIGITS :
					loader->LoadSDigits() ; 
					data = loader->TreeS() ; 
					if ( ! data ) {
						AliWarning(Form(" SDigit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
					} else {
						qadm->Exec(taskIndex, data) ; 
					}
					break; 
					case AliQA::kDIGITS :
					loader->LoadDigits() ; 
					data = loader->TreeD() ; 
					if ( ! data ) {
						AliWarning(Form(" Digit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
					} else {
						qadm->Exec(taskIndex, data) ;
					}
					break; 
					case AliQA::kRECPOINTS :
					loader->LoadRecPoints() ; 
					data = loader->TreeR() ; 
					if (!data) {
						AliWarning(Form("RecPoints not found for %s", AliQA::GetDetName(iDet))) ; 
					} else {
						qadm->Exec(taskIndex, data) ; 
					}
					break; 
					case AliQA::kTRACKSEGMENTS :
					break; 
					case AliQA::kRECPARTICLES :
					break; 
					case AliQA::kESDS :
					qadm->Exec(taskIndex, fESD) ;
					break; 
					case AliQA::kNTASKINDEX :
					break; 
			} //task switch
			//qadm->Increment() ; 
		} // detector loop
	} // event loop	
//	// Save QA data for all detectors
	rv = Finish(taskIndex, mode) ;
	
	if ( taskIndex == AliQA::kRAWS ) 
		fRawReader->RewindEvents() ;

	return rv ; 
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Finish(const AliQA::TASKINDEX_t taskIndex, const char * /*mode*/) 
{
	// write output to file for all detectors
	for (Int_t i = 0; i < fQADataMakers.GetEntriesFast() ; i++) {
		AliQADataMaker * qadm = static_cast<AliQADataMaker *>(fQADataMakers.At(i));
		qadm->EndOfCycle(taskIndex) ; 
	}    
	return kTRUE ; 
}

//_____________________________________________________________________________
TObjArray * AliQADataMakerSteer::GetFromOCDB(AliQA::DETECTORINDEX_t det, AliQA::TASKINDEX_t task, const char * year) const 
{
	// Retrieve the list of QA data for a given detector and a given task 
	TObjArray * rv = NULL ;
	TString tmp(AliQA::GetQARefStorage()) ; 
	if ( tmp.IsNull() ) { 
		AliError("No storage defined, use AliQA::SetQARefStorage") ; 
		return NULL ; 
	}	
	AliCDBManager* man = AliCDBManager::Instance() ; 
	if ( ! man->IsDefaultStorageSet() ) {
		TString tmp(AliQA::GetQARefDefaultStorage()) ; 
		tmp.Append(year) ; 
		tmp.Append("/") ; 
		man->SetDefaultStorage(tmp.Data()) ; 		
		man->SetSpecificStorage(Form("%s/*", AliQA::GetQAName()), AliQA::GetQARefStorage()) ;
	}
	TString detOCDBDir(Form("%s/%s/%s", AliQA::GetQAName(), AliQA::GetDetName((Int_t)det), AliQA::GetRefOCDBDirName())) ; 
	AliInfo(Form("Retrieving reference data from %s/%s for %s", AliQA::GetQARefStorage(), detOCDBDir.Data(), AliQA::GetTaskName(task).Data())) ; 
	AliCDBEntry* entry = man->Get(detOCDBDir.Data(), 0) ; //FIXME 0 --> Run Number
	TList * listDetQAD = dynamic_cast<TList *>(entry->GetObject()) ;
	if ( listDetQAD ) 
		rv = dynamic_cast<TObjArray *>(listDetQAD->FindObject(AliQA::GetTaskName(task))) ; 
	return rv ; 
}

//_____________________________________________________________________________
AliLoader * AliQADataMakerSteer::GetLoader(Int_t iDet)
{
	// get the loader for a detector

	if ( !fRunLoader ) 
		return NULL ; 
	
	TString detName = AliQA::GetDetName(iDet) ;
    fLoader[iDet] = fRunLoader->GetLoader(detName + "Loader");
	if (fLoader[iDet]) 
		return fLoader[iDet] ;
	
	// load the QA data maker object
	TPluginManager* pluginManager = gROOT->GetPluginManager() ;
	TString loaderName = "Ali" + detName + "Loader" ;

	AliLoader * loader = NULL ;
	// first check if a plugin is defined for the quality assurance data maker
	TPluginHandler* pluginHandler = pluginManager->FindHandler("AliLoader", detName) ;
	// if not, add a plugin for it
	if (!pluginHandler) {
		AliDebug(1, Form("defining plugin for %s", loaderName.Data())) ;
		TString libs = gSystem->GetLibraries() ;
		if (libs.Contains("lib" + detName + "base.so") || (gSystem->Load("lib" + detName + "base.so") >= 0)) {
			pluginManager->AddHandler("AliQADataMaker", detName, loaderName, detName + "loader", loaderName + "()") ;
		} else {
			pluginManager->AddHandler("AliLoader", detName, loaderName, detName, loaderName + "()") ;
		}
		pluginHandler = pluginManager->FindHandler("AliLoader", detName) ;
	}
	if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) {
		loader = (AliLoader *) pluginHandler->ExecPlugin(0) ;
	}
	if (loader) 
		fLoader[iDet] = loader ;
	return loader ;
}

//_____________________________________________________________________________
AliQADataMaker * AliQADataMakerSteer::GetQADataMaker(const Int_t iDet, const char * mode )
{
	// get the quality assurance data maker for a detector
	
	if (fQADataMaker[iDet]) 
		return fQADataMaker[iDet] ;
	
	AliQADataMaker * qadm = NULL ;
	
	if (fFirst) {
		// load the QA data maker object
		TPluginManager* pluginManager = gROOT->GetPluginManager() ;
		TString detName = AliQA::GetDetName(iDet) ;
		TString tmp(mode) ; 
		if (tmp.Contains("sim")) 
			tmp.ReplaceAll("s", "S") ; 
		else if (tmp.Contains("rec")) 
			tmp.ReplaceAll("r", "R") ; 
		TString qadmName = "Ali" + detName + "QADataMaker" + tmp ;

		// first check if a plugin is defined for the quality assurance data maker
		TPluginHandler* pluginHandler = pluginManager->FindHandler("AliQADataMaker", detName) ;
		// if not, add a plugin for it
		if (!pluginHandler) {
			AliDebug(1, Form("defining plugin for %s", qadmName.Data())) ;
			TString libs = gSystem->GetLibraries() ;
			if (libs.Contains("lib" + detName + mode + ".so") || (gSystem->Load("lib" + detName + mode + ".so") >= 0)) {
				pluginManager->AddHandler("AliQADataMaker", detName, qadmName, detName + "qadm", qadmName + "()") ;
			} else {
				pluginManager->AddHandler("AliQADataMaker", detName, qadmName, detName, qadmName + "()") ;
			}
			pluginHandler = pluginManager->FindHandler("AliQADataMaker", detName) ;
		}
		if (pluginHandler && (pluginHandler->LoadPlugin() == 0)) {
			qadm = (AliQADataMaker *) pluginHandler->ExecPlugin(0) ;
		}
		if (qadm) 
			fQADataMaker[iDet] = qadm ;
	}
	return qadm ;
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Init(const AliQA::TASKINDEX_t taskIndex, const char * mode, const  char * input )
{
	// Initialize the event source and QA data makers
	
	if (taskIndex == AliQA::kRAWS) { 
		if (!fRawReader) {
		        fRawReader = AliRawReader::Create(input);
		}
	    if ( ! fRawReader ) 
			return kFALSE ; 
		fRawReaderDelete = kTRUE ; 
		fRawReader->NextEvent() ; 
		fRunNumber = fRawReader->GetRunNumber() ; 
		AliCDBManager::Instance()->SetRun(fRunNumber) ; 
		fRawReader->RewindEvents();
		fNumberOfEvents = 999999 ;
		if ( fMaxEvents < 0 ) 
			fMaxEvents = fNumberOfEvents ; 
		} else if (taskIndex == AliQA::kESDS) {
		if (!gSystem->AccessPathName("AliESDs.root")) { // AliESDs.root exists
			TFile * esdFile = TFile::Open("AliESDs.root") ;
			fESDTree = dynamic_cast<TTree *> (esdFile->Get("esdTree")) ; 
			if ( !fESDTree ) {
				AliError("esdTree not found") ; 
				return kFALSE ; 
			} else {
				fESD     = new AliESDEvent() ;
				fESD->ReadFromTree(fESDTree) ;
				fESDTree->GetEntry(0) ; 
				fRunNumber = fESD->GetRunNumber() ; 
				fNumberOfEvents = fESDTree->GetEntries() ;
				if ( fMaxEvents < 0 ) 
					fMaxEvents = fNumberOfEvents ; 
			}
		} else {
			AliError("AliESDs.root not found") ; 
			return kFALSE ; 
		}			
		} else {
		if ( !InitRunLoader() ) { 
			AliWarning("No Run Loader not found") ; 
		} else {
			fNumberOfEvents = fRunLoader->GetNumberOfEvents() ;
			if ( fMaxEvents < 0 ) 
				fMaxEvents = fNumberOfEvents ; 

		}
	}

	// Get Detectors 
	TObjArray* detArray = NULL ; 
	if (fRunLoader) // check if RunLoader exists 
		if ( fRunLoader->GetAliRun() ) { // check if AliRun exists in gAlice.root
			detArray = fRunLoader->GetAliRun()->Detectors() ;
			fRunNumber = fRunLoader->GetHeader()->GetRun() ; 
		}
	// Build array of QA data makers for all detectors
	fQADataMakers.Clear();
	for (UInt_t iDet = 0; iDet < fgkNDetectors ; iDet++) {
		if (IsSelected(AliQA::GetDetName(iDet))) {
			AliQADataMaker * qadm = GetQADataMaker(iDet, mode) ;
			if (!qadm) {
				AliError(Form("AliQADataMaker not found for %s", AliQA::GetDetName(iDet))) ; 
				fDetectorsW.ReplaceAll(AliQA::GetDetName(iDet), "") ; 
			} else {
				AliDebug(1, Form("Data Maker found for %s", qadm->GetName())) ; 
				// skip non active detectors
				if (detArray) {
					AliModule* det = static_cast<AliModule*>(detArray->FindObject(AliQA::GetDetName(iDet))) ;
					if (!det || !det->IsActive())  
						continue ;
				}
				qadm->SetName(AliQA::GetDetName(iDet));
				qadm->SetUniqueID(iDet);
				fQADataMakers.Add(qadm);
			}
		}
	} 
	// Initialize all QA data makers for all detectors
	fRunNumber = AliCDBManager::Instance()->GetRun() ; 
	if ( !  AliGeomManager::GetGeometry() ) 
		AliGeomManager::LoadGeometry() ; 
	for (Int_t i = 0; i < fQADataMakers.GetEntriesFast() ; i++) {
		AliQADataMaker * qadm = static_cast<AliQADataMaker *>(fQADataMakers.At(i));
		qadm->Init(taskIndex, fRunNumber, GetQACycles(qadm->GetUniqueID())) ;
		qadm->StartOfCycle(taskIndex, fCycleSame) ;
	} 
	fFirst = kFALSE ;
	return kTRUE ; 
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::InitRunLoader()
{
	// get or create the run loader
	if (fRunLoader) {
		fCycleSame = kTRUE ; 
	} else {
		if (!gSystem->AccessPathName(fGAliceFileName.Data())) { // galice.root exists
			// load all base libraries to get the loader classes
			TString libs = gSystem->GetLibraries() ;
			for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
				if (!IsSelected(AliQA::GetDetName(iDet))) 
					continue ; 
				TString detName = AliQA::GetDetName(iDet) ;
				if (detName == "HLT") 
					continue;
				if (libs.Contains("lib" + detName + "base.so")) 
					continue;
				gSystem->Load("lib" + detName + "base.so");
			}
			fRunLoader = AliRunLoader::Open(fGAliceFileName.Data());
			if (!fRunLoader) {
				AliError(Form("no run loader found in file %s", fGAliceFileName.Data()));
				return kFALSE;
			}
			fRunLoader->CdGAFile();
			if (fRunLoader->LoadgAlice() == 0) {
				gAlice = fRunLoader->GetAliRun();
			}

			if (!gAlice) {
				AliError(Form("no gAlice object found in file %s", fGAliceFileName.Data()));
				return kFALSE;
			}

		} else {               // galice.root does not exist
			AliError(Form("the file %s does not exist", fGAliceFileName.Data()));
			return kFALSE;
		}
	}

	if (!fRunNumber) { 
		fRunLoader->LoadHeader();
		fRunNumber = fRunLoader->GetHeader()->GetRun() ; 
	}
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::IsSelected(const char * det) 
{
	// check whether detName is contained in detectors
	// if yes, it is removed from detectors
	
	Bool_t rv = kFALSE;
	const TString detName(det) ;
	// check if all detectors are selected
	if (fDetectors.Contains("ALL")) {
		fDetectors = "ALL";
		rv = kTRUE;
	} else if ((fDetectors.CompareTo(detName) == 0) ||
			   fDetectors.BeginsWith(detName+" ") ||
			   fDetectors.EndsWith(" "+detName) ||
			   fDetectors.Contains(" "+detName+" ")) {
		//		fDetectors.ReplaceAll(detName, "");
		rv = kTRUE;
	}
	
	// clean up the detectors string
	//	while (fDetectors.Contains("  ")) 
	//		fDetectors.ReplaceAll("  ", " ");
	//	while (fDetectors.BeginsWith(" ")) 
	//		fDetectors.Remove(0, 1);
	//	while (fDetectors.EndsWith(" ")) 
	//		fDetectors.Remove(fDetectors.Length()-1, 1);
	
	return rv ;
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Merge(const Int_t runNumber) const
{
	// Merge all the cycles from all detectors in one single file per run
	TString cmd ;
	if ( runNumber == -1 )
		cmd = Form(".! ls *%s*.*.*.root > tempo.txt", AliQA::GetQADataFileName()) ; 
	else 
		cmd = Form(".! ls *%s*.%d.*.root > tempo.txt", AliQA::GetQADataFileName(), runNumber) ; 
	gROOT->ProcessLine(cmd.Data()) ;
	ifstream in("tempo.txt") ; 
	const Int_t runMax = 10 ;  
	TString file[AliQA::kNDET*runMax] ;
	Int_t run[AliQA::kNDET*runMax] = {-1} ;
	
	Int_t index = 0 ; 
	while ( 1 ) {
		in >> file[index] ; 
		if ( !in.good() ) 
			break ; 
		AliInfo(Form("index = %d file = %s", index, (file[index]).Data())) ; 
		index++ ;
	}
	
	if ( index == 0 ) { 
		AliError(Form("run number %d not found", runNumber)) ; 
		return kFALSE ; 
	}
	
	Int_t runIndex    = 0 ;  
	Int_t runIndexMax = 0 ; 
	TString stmp(Form(".%s.", AliQA::GetQADataFileName())) ; 
	for (Int_t ifile = 0 ; ifile < index ; ifile++) {
		TString tmp(file[ifile]) ; 
		tmp.ReplaceAll(".root", "") ; 
		TString det = tmp(0, tmp.Index(".")) ; 
		tmp.Remove(0, tmp.Index(stmp)+4) ; 
		TString ttmp = tmp(0, tmp.Index(".")) ; 
		Int_t newRun = ttmp.Atoi() ;
		for (Int_t irun = 0; irun <= runIndexMax; irun++) {
			if (newRun == run[irun]) 
				break ; 
			run[runIndex] = newRun ; 
			runIndex++ ; 
		}
		runIndexMax = runIndex ; 
		ttmp = tmp(tmp.Index(".")+1, tmp.Length()) ; 
		Int_t cycle = ttmp.Atoi() ;  
		AliDebug(1, Form("%s : det = %s run = %d cycle = %d \n", file[ifile].Data(), det.Data(), newRun, cycle)) ; 
	}
	for (Int_t irun = 0 ; irun < runIndexMax ; irun++) {
		TFileMerger merger ; 
		TString outFileName(Form("Merged.%s.%d.root",AliQA::GetQADataFileName(),run[irun]));		
		merger.OutputFile(outFileName.Data()) ; 
		for (Int_t ifile = 0 ; ifile < index-1 ; ifile++) {
			TString pattern(Form("%s.%d.", AliQA::GetQADataFileName(), run[irun])) ; 
			TString tmp(file[ifile]) ; 
			if (tmp.Contains(pattern)) {
				merger.AddFile(tmp) ; 
			}
		}
		merger.Merge() ; 
	}
	
	return kTRUE ; 
}

//_____________________________________________________________________________
void AliQADataMakerSteer::Reset(const Bool_t sameCycle)
{
	// Reset the default data members

	for (Int_t i = 0; i < fQADataMakers.GetEntriesFast() ; i++) {
		AliQADataMaker * qadm = static_cast<AliQADataMaker *>(fQADataMakers.At(i));
		qadm->Reset(sameCycle);
	} 
	if (fRawReaderDelete) { 
		delete fRawReader ;
		fRawReader      = NULL ;
	}

	fCycleSame      = sameCycle ; 
	fESD            = NULL ; 
	fESDTree        = NULL ; 
	fFirst          = kTRUE ;   
	fNumberOfEvents = 999999 ;  
}

//_____________________________________________________________________________
TString AliQADataMakerSteer::Run(const char * detectors, AliRawReader * rawReader, const Bool_t sameCycle) 
{
	//Runs all the QA data Maker for Raws only
	
	fCycleSame       = sameCycle ;
	fRawReader       = rawReader ;
	fDetectors       = detectors ; 
	fDetectorsW      = detectors ; 	
	
	AliCDBManager* man = AliCDBManager::Instance() ; 

	if ( man->GetRun() == -1 ) {// check if run number not set previously and set it from raw data
		rawReader->NextEvent() ; 
		man->SetRun(fRawReader->GetRunNumber()) ;
		rawReader->RewindEvents() ;
	}	
	
	if ( !Init(AliQA::kRAWS, "rec") ) 
		return kFALSE ; 
	fRawReaderDelete = kFALSE ; 

	DoIt(AliQA::kRAWS, "rec") ; 
	return 	fDetectorsW ;
}

//_____________________________________________________________________________
TString AliQADataMakerSteer::Run(const char * detectors, const char * fileName, const Bool_t sameCycle) 
{
	//Runs all the QA data Maker for Raws only

	fCycleSame       = sameCycle ;
	fDetectors       = detectors ; 
	fDetectorsW      = detectors ; 	
	
	AliCDBManager* man = AliCDBManager::Instance() ; 
	if ( man->GetRun() == -1 ) { // check if run number not set previously and set it from AliRun
		AliRunLoader * rl = AliRunLoader::Open("galice.root") ;
		if ( ! rl ) {
			AliFatal("galice.root file not found in current directory") ; 
		} else {
			rl->CdGAFile() ; 
			rl->LoadgAlice() ;
			if ( ! rl->GetAliRun() ) {
				AliFatal("AliRun not found in galice.root") ;
			} else {
				rl->LoadHeader() ;
				man->SetRun(rl->GetHeader()->GetRun());
			}
		}
	}
	
	if ( !Init(AliQA::kRAWS, "rec", fileName) ) 
		return kFALSE ; 
	
	DoIt(AliQA::kRAWS, "rec") ; 
	return 	fDetectorsW ;
}

//_____________________________________________________________________________
TString AliQADataMakerSteer::Run(const char * detectors, const AliQA::TASKINDEX_t taskIndex, Bool_t const sameCycle, const  char * fileName ) 
{
	// Runs all the QA data Maker for every detector
	
	fCycleSame       = sameCycle ;
	fDetectors       = detectors ; 
	fDetectorsW      = detectors ; 		
	
	TString mode ; 
	if ( (taskIndex == AliQA::kHITS) || (taskIndex == AliQA::kSDIGITS) || (taskIndex == AliQA::kDIGITS) ) 
		mode = "sim" ; 
	else if ( (taskIndex == AliQA::kRAWS) || (taskIndex == AliQA::kRECPOINTS) || (taskIndex == AliQA::kESDS) )
		mode = "rec" ; 
	else {
		AliError(Form("%s not implemented", AliQA::GetTaskName(taskIndex).Data())) ; 
		return "" ;
	}

	AliCDBManager* man = AliCDBManager::Instance() ; 	
	if ( man->GetRun() == -1 ) { // check if run number not set previously and set it from AliRun
		AliRunLoader * rl = AliRunLoader::Open("galice.root") ;
		if ( ! rl ) {
			AliFatal("galice.root file not found in current directory") ; 
		} else {
			rl->CdGAFile() ; 
			rl->LoadgAlice() ;
			if ( ! rl->GetAliRun() ) {
				AliFatal("AliRun not found in galice.root") ;
			} else {
				rl->LoadHeader() ;
				man->SetRun(rl->GetHeader()->GetRun()) ;
			}
		}
	}
	
	if ( !Init(taskIndex, mode.Data(), fileName) ) 
		return kFALSE ; 

	DoIt(taskIndex, mode.Data()) ;
	
	return fDetectorsW ;

}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Save2OCDB(const Int_t runNumber, const char * year, const Int_t cycleNumber, const char * detectors) const
{
	// take the locasl QA data merge into a single file and save in OCDB 
	Bool_t rv = kTRUE ; 
	TString tmp(AliQA::GetQARefStorage()) ; 
	if ( tmp.IsNull() ) { 
		AliError("No storage defined, use AliQA::SetQARefStorage") ; 
		return kFALSE ; 
	}
	if ( !(tmp.Contains(AliQA::GetLabLocalOCDB()) || tmp.Contains(AliQA::GetLabAliEnOCDB())) ) {
		AliError(Form("%s is a wrong storage, use %s or %s", AliQA::GetQARefStorage(), AliQA::GetLabLocalOCDB().Data(), AliQA::GetLabAliEnOCDB().Data())) ; 
		return kFALSE ; 
	}
	TString sdet(detectors) ; 
	sdet.ToUpper() ;
	TFile * inputFile ; 
	if ( sdet.Contains("ALL") ) {
		rv = Merge(runNumber) ; 
		if ( ! rv )
			return kFALSE ; 
		TString inputFileName(Form("Merged.%s.%d.root", AliQA::GetQADataFileName(), runNumber)) ; 
		inputFile = TFile::Open(inputFileName.Data()) ; 
		rv = SaveIt2OCDB(runNumber, inputFile, year) ; 
	} else {
		for (Int_t index = 0; index < AliQA::kNDET; index++) {
			if (sdet.Contains(AliQA::GetDetName(index))) {
				TString inputFileName(Form("%s.%s.%d.%d.root", AliQA::GetDetName(index), AliQA::GetQADataFileName(), runNumber, cycleNumber)) ; 
				inputFile = TFile::Open(inputFileName.Data()) ; 			
				rv *= SaveIt2OCDB(runNumber, inputFile, year) ; 
			}
		}
	}
	return rv ; 
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::SaveIt2OCDB(const Int_t runNumber, TFile * inputFile, const char * year) const
{
	// reads the TH1 from file and adds it to appropriate list before saving to OCDB
	Bool_t rv = kTRUE ;
	AliInfo(Form("Saving TH1s in %s to %s", inputFile->GetName(), AliQA::GetQARefStorage())) ; 
	AliCDBManager* man = AliCDBManager::Instance() ; 
	if ( ! man->IsDefaultStorageSet() ) {
		TString tmp( AliQA::GetQARefStorage() ) ; 
		if ( tmp.Contains(AliQA::GetLabLocalOCDB()) ) 
			man->SetDefaultStorage(AliQA::GetQARefStorage()) ;
		else {
			TString tmp(AliQA::GetQARefDefaultStorage()) ; 
			tmp.Append(year) ; 
			tmp.Append("?user=alidaq") ; 
			man->SetDefaultStorage(tmp.Data()) ; 
		}
	}
	man->SetSpecificStorage("*", AliQA::GetQARefStorage()) ; 
	if(man->GetRun() < 0) 
		man->SetRun(runNumber);

	AliCDBMetaData mdr ;
	mdr.SetResponsible("yves schutz");

	for ( Int_t detIndex = 0 ; detIndex < AliQA::kNDET ; detIndex++) {
		TDirectory * detDir = inputFile->GetDirectory(AliQA::GetDetName(detIndex)) ; 
		if ( detDir ) {
			AliInfo(Form("Entering %s", detDir->GetName())) ;
			TString detOCDBDir(Form("%s/%s/%s", AliQA::GetDetName(detIndex), AliQA::GetRefOCDBDirName(), AliQA::GetRefDataDirName())) ; 
			AliCDBId idr(detOCDBDir.Data(), runNumber, AliCDBRunRange::Infinity())  ;
			TList * listDetQAD = new TList() ;
			TString listName(Form("%s QA data Reference", AliQA::GetDetName(detIndex))) ; 
			mdr.SetComment("HMPID QA stuff");
			listDetQAD->SetName(listName) ; 
			TList * taskList = detDir->GetListOfKeys() ; 
			TIter nextTask(taskList) ; 
			TKey * taskKey ; 
			while ( (taskKey = dynamic_cast<TKey*>(nextTask())) ) {
				TDirectory * taskDir = detDir->GetDirectory(taskKey->GetName()) ; 
				AliInfo(Form("Saving %s", taskDir->GetName())) ; 
				TObjArray * listTaskQAD = new TObjArray(100) ; 
				listTaskQAD->SetName(taskKey->GetName()) ;
				listDetQAD->Add(listTaskQAD) ; 
				TList * histList = taskDir->GetListOfKeys() ; 
				TIter nextHist(histList) ; 
				TKey * histKey ; 
				while ( (histKey = dynamic_cast<TKey*>(nextHist())) ) {
					TObject * odata = taskDir->Get(histKey->GetName()) ; 
					if ( !odata ) {
						AliError(Form("%s in %s/%s returns a NULL pointer !!", histKey->GetName(), detDir->GetName(), taskDir->GetName())) ;
					} else {
						AliInfo(Form("Adding %s", histKey->GetName())) ;
						if ( odata->IsA()->InheritsFrom("TH1") ) {
							AliInfo(Form("Adding %s", histKey->GetName())) ;
							TH1 * hdata = static_cast<TH1*>(odata) ; 
							listTaskQAD->Add(hdata) ; 
						}
					}
				}
			}
			man->Put(listDetQAD, idr, &mdr) ;
		}
	}
	return rv ; 
}	


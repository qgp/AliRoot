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

#include <TFile.h>
#include <TPluginManager.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>

#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliQA.h"
#include "AliQADataMaker.h"
#include "AliQADataMakerSteer.h" 
#include "AliRawReaderRoot.h"
#include "AliRun.h"
#include "AliRunLoader.h"

ClassImp(AliQADataMakerSteer) 

//_____________________________________________________________________________
AliQADataMakerSteer::AliQADataMakerSteer(const char* gAliceFilename, const char * name, const char * title) :
	TNamed(name, title), 
	fCycleOption("new"), 
	fESD(NULL), 
	fESDTree(NULL),
	fFirst(kTRUE),  
	fGAliceFileName(gAliceFilename), 
	fRunNumber(0), 
	fNumberOfEvents(0), 
	fRawReader(NULL), 
	fRunLoader(NULL)  
{
	// default ctor
	for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
		fLoader[iDet]      = NULL ;
		fQADataMaker[iDet] = NULL ;
		fQACycles[iDet]    = 999999 ;	
	}	
}

//_____________________________________________________________________________
AliQADataMakerSteer::AliQADataMakerSteer(const AliQADataMakerSteer & qas) : 
	TNamed(qas), 
	fCycleOption("new"), 
	fESD(NULL), 
	fESDTree(NULL), 
	fFirst(qas.fFirst),  
	fGAliceFileName(qas.fGAliceFileName), 
	fRunNumber(qas.fRunNumber), 
	fNumberOfEvents(qas.fNumberOfEvents), 
	fRawReader(NULL), 
	fRunLoader(NULL)  
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
    fLoader[iDet] = NULL;
    delete fQADataMaker[iDet];
    fQADataMaker[iDet] = NULL;
  }

  delete fRunLoader;
  fRunLoader = NULL;
  delete fRawReader;
  fRawReader = NULL;
}

//_____________________________________________________________________________
AliLoader * AliQADataMakerSteer::GetLoader(Int_t iDet)
{
	// get the loader for a detector
	
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
AliQADataMaker * AliQADataMakerSteer::GetQADataMaker(Int_t iDet)
{
	// get the quality assurance data maker for a detector
	
	if (fQADataMaker[iDet]) 
		return fQADataMaker[iDet] ;
	
	AliQADataMaker * qadm = NULL ;
	
	if (fFirst) {
		// load the QA data maker object
		TPluginManager* pluginManager = gROOT->GetPluginManager() ;
		TString detName = AliQA::GetDetName(iDet) ;
		TString qadmName = "Ali" + detName + "QADataMaker" ;

		// first check if a plugin is defined for the quality assurance data maker
		TPluginHandler* pluginHandler = pluginManager->FindHandler("AliQADataMaker", detName) ;
		// if not, add a plugin for it
		if (!pluginHandler) {
			AliDebug(1, Form("defining plugin for %s", qadmName.Data())) ;
			TString libs = gSystem->GetLibraries() ;
			if (libs.Contains("lib" + detName + "base.so") || (gSystem->Load("lib" + detName + "base.so") >= 0)) {
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
Bool_t AliQADataMakerSteer::Init(const AliQA::TASKINDEX taskIndex, const  char * fileName )
{
	// Initialize the event source and QA data makers
	
	if (taskIndex == AliQA::kRAWS) {
		fRawReader = new AliRawReaderRoot(fileName) ; 
	    if ( ! fRawReader ) 
			return kFALSE ; 
		fRawReader->NextEvent() ; 
		fRunNumber = fRawReader->GetRunNumber() ; 
		fRawReader->RewindEvents();
		fNumberOfEvents = 999999 ;
		
	} else if (taskIndex == AliQA::kESDS) {
		if (!gSystem->AccessPathName("AliESDs.root")) { // AliESDs.root exists
			TFile * esdFile = TFile::Open("AliESDs.root") ;
			fESDTree = dynamic_cast<TTree *> (esdFile->Get("esdTree")) ; 
	        fESD     = new AliESDEvent() ;
            fESD->ReadFromTree(fESDTree) ;
			fESDTree->GetEntry(0) ; 
			fRunNumber = fESD->GetRunNumber() ; 
			fNumberOfEvents = fESDTree->GetEntries() ;
		} else {
			AliError("AliESDs.root not found") ; 
			return kFALSE ; 
		}
		
	} else {
		if ( !InitRunLoader() ) {
			AliError("Problems in getting the Run Loader") ; 
			return kFALSE ; 
		} else {
			fRunNumber      = fRunLoader->GetAliRun()->GetRunNumber() ; 
			fNumberOfEvents = fRunLoader->GetNumberOfEvents() ;
		}
	}
		
	// Initialize all QA data makers for all detectors
	for (UInt_t iDet = 0; iDet < fgkNDetectors ; iDet++) {
		AliQADataMaker * qadm = GetQADataMaker(iDet) ;
		if (!qadm) {
			AliWarning(Form("AliQADataMaker not found for %s", AliQA::GetDetName(iDet))) ; 
		} else {
			AliInfo(Form("Data Maker found for %s", qadm->GetName())) ; 
			qadm->Init(taskIndex, fRunNumber, GetQACycles(iDet)) ;
			qadm->StartOfCycle(taskIndex, fCycleOption) ;
		}
	} 
	fFirst = kFALSE ;
	return kTRUE ; 
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::InitRunLoader()
{
	// get or create the run loader
	if (fRunLoader) {
		fCycleOption = "same" ; 
		return kTRUE ;
	} 
		
	if (!gSystem->AccessPathName(fGAliceFileName.Data())) { // galice.root exists
    // load all base libraries to get the loader classes
		TString libs = gSystem->GetLibraries() ;
		for (UInt_t iDet = 0; iDet < fgkNDetectors; iDet++) {
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

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Finish(const AliQA::TASKINDEX taskIndex) 
{
	// write output to file for all detectors
	for (UInt_t iDet = 0; iDet < fgkNDetectors ; iDet++) {
		AliQADataMaker * qadm = GetQADataMaker(iDet) ;
		if (qadm) {
			qadm->EndOfCycle(taskIndex) ; 
			qadm->Finish(taskIndex) ; 
		}
	}
	return kTRUE ; 
}

//_____________________________________________________________________________
Bool_t AliQADataMakerSteer::Run(const AliQA::TASKINDEX taskIndex, const  char * fileName )
{
	// Runs all the QA data Maker for every detector
	Bool_t rv = kFALSE ;
	
	if ( !Init(taskIndex, fileName) ) 
		return kFALSE ; 
		
    // Fill QA data in event loop 
	for (UInt_t iEvent = 0 ; iEvent < fNumberOfEvents ; iEvent++) {
		// Get the event
		AliInfo(Form("processing event %d", iEvent));
		if ( taskIndex == AliQA::kRAWS ) {
			if ( !fRawReader->NextEvent() )
				break ;
		} else if ( taskIndex == AliQA::kESDS ) {
			fESDTree->GetEntry(iEvent) ;
		} else {
			fRunLoader->GetEvent(iEvent);
		}
		// loop over detectors
		for (UInt_t iDet = 0 ; iDet < fgkNDetectors ; iDet++) {
			AliQADataMaker * qadm = GetQADataMaker(iDet) ;
			if (!qadm) {
				rv = kFALSE ;
			} else {
				if ( qadm->IsCycleDone() ) {
					qadm->EndOfCycle(AliQA::kRAWS) ;
					qadm->StartOfCycle(AliQA::kRAWS) ;
				}
				TTree * data ; 
				switch (taskIndex) {
					case AliQA::kRAWS :
						qadm->Exec(taskIndex, fRawReader) ; 
					break ; 
					case AliQA::kHITS :
						GetLoader(iDet)->LoadHits() ; 
						data = GetLoader(iDet)->TreeH() ; 
						if ( ! data ) {
							AliWarning(Form(" Hit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
						} else {
							qadm->Exec(taskIndex, data) ;
						} 
					break ;
					case AliQA::kSDIGITS :
						GetLoader(iDet)->LoadSDigits() ; 
						data = GetLoader(iDet)->TreeS() ; 
						if ( ! data ) {
							AliWarning(Form(" SDigit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
						} else {
							qadm->Exec(taskIndex, data) ; 
						}
					break; 
					case AliQA::kDIGITS :
						GetLoader(iDet)->LoadDigits() ; 
						data = GetLoader(iDet)->TreeD() ; 
						if ( ! data ) {
							AliWarning(Form(" Digit Tree not found for  %s", AliQA::GetDetName(iDet))) ; 
						} else {
							qadm->Exec(taskIndex, data) ;
						}
					break; 
					case AliQA::kRECPOINTS :
						GetLoader(iDet)->LoadRecPoints() ; 
						data = GetLoader(iDet)->TreeR() ; 
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
				} //task switch
				qadm->Increment() ; 
			} //data maker exist
		} // detector loop
	} // event loop	
	// Save QA data for all detectors
	rv = Finish(taskIndex) ;
	return rv ; 
}

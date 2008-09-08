// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Sebastian Bablok <Sebastian.Bablok@ift.uib.no>        *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/** @file   AliHLTPendolino.cxx
    @author Sebastian Bablok
    @date   
    @brief  
*/

#include "AliHLTPendolino.h"

#include "AliHLTPredictionProcessorInterface.h"
#include "AliHLTPendolinoLogger.h"
#include "AliHLTPendolinoLoggerOStream.h"

#include <AliCDBPath.h>
#include <AliCDBEntry.h>
#include <AliCDBManager.h>
#include <AliCDBStorage.h>
#include <AliPreprocessor.h>
#include <AliCDBId.h>

#include <TObjString.h>
#include <TTimeStamp.h>

#include <fstream>
#include <stdexcept>
#include <cstdlib>


using namespace std;


ClassImp(AliHLTPendolino)
		

/** Static string to define a local storage for the OCDB contact. */
const TString AliHLTPendolino::kLOCAL_STORAGE_DEFINE = "local://";

const char* AliHLTPendolino::kHLTInterfaceModule = "Pendolino-Core";

const TString AliHLTPendolino::kTaxiListBaseFolder = getenv("ALIHLT_T_HCDBDIR"); 
//"/opt/T-HCDB/lists/lists-taxi/";

const TString AliHLTPendolino::kTaxiListFolderName = "lists/lists-taxi/";

const TString AliHLTPendolino::kTaxiListPendolino = "Pendolino.list";

const Int_t AliHLTPendolino::kMAX_LINE_LENGTH = 256;

const Int_t AliHLTPendolino::kHLTPendolinoException = -10;

const Int_t AliHLTPendolino::kHLTPendolinoBadCast = -9;

const Int_t AliHLTPendolino::kHLTPendolinoNotPredictProc = -8;

const Int_t AliHLTPendolino::kHLTPendolinoModuleNotExisting = -7;

const Int_t AliHLTPendolino::kHLTPendolinoNoDCS = -6;

//const Int_t AliHLTPendolino::



AliHLTPendolino::AliHLTPendolino(Int_t run, TString HCDBbase, 
			TString runType, AliHLTPendolinoLogger* logger) :
			fRunType(runType), fRunNumber(run), 
			fHCDBPath(""), fPredictionProcessorMap(),
			fpLogger(0), fOwnLogger(kFALSE) {
	// C-tor of AliHLTPendolino
	fHCDBPath = kLOCAL_STORAGE_DEFINE + HCDBbase;
	if (logger == 0) {
		fpLogger = new AliHLTPendolinoLoggerOStream();
		fOwnLogger = kTRUE;
	} else {
		fpLogger = logger;
		fOwnLogger = kFALSE;
	}
}


AliHLTPendolino::~AliHLTPendolino() {
	// D-tor of AliHLTPendolino
	// clean up registered PredicitonProcs
    TMapIter iter(&fPredictionProcessorMap, kIterForward);
    AliHLTPredictionProcessorInterface* aPredict;
    TObject* key = 0;

    // get each key inside the map
    while ((key = iter.Next())) {
        TString detector = key->GetName();

        try {
            // get value for the key
            aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*>
                    (fPredictionProcessorMap.GetValue(key));

            if (aPredict == 0) {
                Log(kHLTInterfaceModule, 
						" *** ERROR, cannot delete registered processor '"
					   	+ detector + "'; does not seem to be a PredictionProcessor.");
                continue;
            }
			Log(kHLTInterfaceModule, " ### [DEBUG] deleting PredictProc '" + 
					detector + "'.");
			delete aPredict;
        } catch (std::bad_cast) {
            // failed -> is not a AliHLTPredictionProcessorInterface implementation
            // -> discarding call
            Log(kHLTInterfaceModule, " *** ERROR, cannot delete registered processor '"
                    + detector + "'; does not seem to be a PredictionProcessor..");
            continue;

        } catch (std::exception& e) {
            Log(kHLTInterfaceModule, 
					" *** Exception in call for deleting PrecitionProcessor '"
                    + detector + "'.");
            continue;
        }
    }
	Log(kHLTInterfaceModule, " ### [DEBUG] Deleting of PredictionProcessors finished.");

	// clean up logger
	if ((fOwnLogger) && (fpLogger != 0)) {
		delete fpLogger;
	}
	
}


// inherited virtual functions, maybe use them from base class
Bool_t AliHLTPendolino::Store(const AliCDBPath& path, TObject* object,
			AliCDBMetaData* metaData, Int_t validityStart, 
			Bool_t validityInfinite) {
	// stores a entry in HCDB
	Bool_t retVal = kFALSE;
	Int_t startNumber = 0;
	Int_t endNumber = 0;
	AliCDBManager* man = 0; 
	AliCDBStorage* local_hcdb = 0;	

	startNumber = ((fRunNumber - validityStart) <= 0) ? 0 : (fRunNumber - validityStart);
	endNumber = (validityInfinite) ? AliCDBRunRange::Infinity() : fRunNumber;

	man = AliCDBManager::Instance();
    if (man == 0) {
		Log(kHLTInterfaceModule, " *** ERROR cannot obtain a CDB Manager reference.");
		return kFALSE;
	}

    // contact local storage (HCDB)
    local_hcdb = man->GetStorage(fHCDBPath.Data());
    if (local_hcdb == 0) {
		TString msg(" *** ERROR in initiating HCDB: ");
		msg += fHCDBPath;
        Log(kHLTInterfaceModule, msg.Data());
        man->DestroyActiveStorages();
        return kFALSE;
    }

	// taken from AliShuttle
	if (! dynamic_cast<TObjString*> (metaData->GetProperty("RunUsed(TObjString)"))) {
        TObjString runUsed = Form("%d", fRunNumber);
        metaData->SetProperty("RunUsed(TObjString)", runUsed.Clone());
    }


    // Version is set to current run, it will be used later to transfer data to Grid
    // Why using current run number as version number ???
	AliCDBId entryID(path, startNumber, endNumber, fRunNumber, -1);
	
	if (local_hcdb->Put(object, entryID, metaData)) {
		retVal = kTRUE;
	} else {
		TString msg(" *** Unable to store DCS data to HCDB: ");
		msg += entryID.ToString();
    	Log(kHLTInterfaceModule, msg.Data());
    }

	man->DestroyActiveStorages();

	return retVal;
}


Bool_t AliHLTPendolino::StoreReferenceData(const AliCDBPath& path,
					   TObject* /*object*/, AliCDBMetaData* /*metaData*/) {
	// Disabled Function inherited from interface
	TString msg(" ~~~ PredictProc tries to store reference data to '" 
			+ path.GetPath() + "'. Discarding call in Pendolino.");
	Log(kHLTInterfaceModule, msg.Data());

	return kFALSE;
}


Bool_t AliHLTPendolino::StoreReferenceFile(const char* detector,
			const char* localFile, const char* gridFileName) {
    // Disabled Function inherited from interface
    TString msg;
	TString det(detector);
	TString filename(localFile);
	TString gridname(gridFileName);
	msg = " ~~~ PredictProc (" + det + ") tries to store reference file (" +
			filename + ") to '" + gridname + 
			"'. Discarding call in Pendolino.";
    Log(kHLTInterfaceModule, msg.Data());

    return kFALSE;
}


Bool_t AliHLTPendolino::StoreRunMetadataFile(const char* localFile,
			const char* gridFileName) {
    // Disabled Function inherited from interface
    TString msg;

    TString filename(localFile);
    TString gridname(gridFileName);
    msg = " ~~~ PredictProc tries to store 'run meta data' file (" +
            filename + ") to '" + gridname + "'. Discarding call in Pendolino.";
    Log(kHLTInterfaceModule, msg.Data());

    return kFALSE;
}


const char* AliHLTPendolino::GetFile(Int_t system, const char* detector,
			const char* id, const char* source) {
    // Disabled Function inherited from interface
    TString msg;
	TString det(detector);
	TString filename(id);
	TString src(source);
	TString from(GetSystemName(system));
	msg = " ~~~ PredictProc (" + det + ") requests file (" + filename + ") from '" 
			+ src + "' at " + from + ". Discarding call in Pendolino.";
	Log(kHLTInterfaceModule, msg.Data());

	return NULL;
}

const char* AliHLTPendolino::GetTriggerConfiguration() {
    // Disabled Function inherited from interface
    TString msg;
    msg = " ~~~ PredictProc tries to request Trigger configuration, this is disabled. Discarding call in Pendolino.";
	Log(kHLTInterfaceModule, msg.Data());

	return NULL;
}


TList* AliHLTPendolino::GetFileSources(Int_t system, const char* detector,
			const char* id) {
	// Disabled Function inherited from interface
    TString msg;
    TString det(detector);
    TString filename(id);
    TString from(GetSystemName(system));
    msg = " ~~~ PredictProc (" + det + ") requests file sources for (" + filename 
			+ ") from '" + from + ". Discarding call in Pendolino.";
    Log(kHLTInterfaceModule, msg.Data());

    return NULL;
}


TList* AliHLTPendolino::GetFileIDs(Int_t system, const char* detector,
			const char* source) {
    // Disabled Function inherited from interface
    TString msg;
    TString det(detector);
    TString filename(source);
    TString from(GetSystemName(system));
    msg = " ~~~ PredictProc (" + det + ") requests file IDs for (" + filename
            + ") from '" + from + ". Discarding call in Pendolino.";
    Log(kHLTInterfaceModule, msg.Data());

    return NULL;
}


const char* AliHLTPendolino::GetRunParameter(const char* /*lbEntry*/) {
	// getter for run parameter
		
// TODO
// maybe using a parameter file, where these settings are stored at start up by
// the starting script and a dedicated class read and stores its content.

	Log(kHLTInterfaceModule, 
			" ### GetRunParameter are not defined, yet. Feature will be available soon.");
	return NULL;
}


Bool_t AliHLTPendolino::GetHLTStatus() {
	// getter for HLT status
	// since this is the Pendolino -> always true
	return kTRUE;
}


AliCDBEntry* AliHLTPendolino::GetFromOCDB(const char* detector,
			const AliCDBPath& path) {
	// fetches entry from HCDB
	AliCDBManager *man = AliCDBManager::Instance();
	AliCDBEntry* entry = 0;
	
	if (man == 0) {
		TString msg(" *** ERROR, cannot obtain a CDB Manager reference for: ");
		msg += detector;
		Log(kHLTInterfaceModule, msg.Data());
		return NULL;
	}

	AliCDBStorage *hcdb = man->GetStorage(fHCDBPath.Data());
	if (hcdb == 0) {
		TString msg(" *** ERROR, cannot acquire HCDB storage (");
		msg += fHCDBPath + ") for fetching data for Pendolino.";
		Log(kHLTInterfaceModule, msg.Data());
		return NULL;
	}
	
	entry = hcdb->Get(path, fRunNumber);

	if (entry == 0) {
		TString msg(" ~~~ WARNING: no valid entry for '");
		msg += path.GetPath() + "' in HCDB for run number ";
		msg += fRunNumber;
		Log(kHLTInterfaceModule, msg.Data());
	}

	return entry;
	
/*
	AliCDBEntry* entry = 0;
	try {
		entry = dynamic_cast<AliCDBEntry*> (hcdb->Get(path, fRunNumber));
	} catch (std::bad_cast) {
		TString msg(" *** ERROR, bad cast of HCDB entry (");
		msg += path.GetPath() + ") after fetching from HCDB.";
		Log(kHLTInterfaceModule, msg.Data());
		return NULL;
	}
	return entry;
*/

}


Bool_t AliHLTPendolino::includeAliCDBEntryInList(const TString& entryPath) {
	// includes entry in Taxi list (objects to be fetched from OCDB)
	Bool_t bRet = kFALSE;
	ifstream infile;
	ofstream outfile;
	TString filename;
	TTimeStamp ts;

	filename = kTaxiListBaseFolder + "/" + kTaxiListFolderName + 
			kTaxiListPendolino;
	Log(kHLTInterfaceModule, filename + " [DEBUG] filename");

	infile.open(filename, ios_base::in);
	if (infile.is_open()) {
		char line[kMAX_LINE_LENGTH];

		while (!infile.eof()) {
			infile.getline(line, kMAX_LINE_LENGTH);
			if (strncmp(line, entryPath.Data(), entryPath.Length()) == 0) {
					// entry already exists, leave function after proper clean up
				TString msg(" --- Entry '");
				msg += entryPath + "' is already included in Taxi list file.";
				Log(kHLTInterfaceModule, msg.Data());	
				infile.close();
				return kTRUE;
			}
		}
		infile.close();
		
		// include entry to list
		outfile.open(filename, ios_base::out | ios_base::app);
		if (!outfile.is_open()) {
			TString msg(" *** Unable to create Pendolino list file '");
			msg += filename + "' for Taxi. Continueing without list update...";
			Log(kHLTInterfaceModule, msg.Data());
			return kFALSE;
		}
//		outfile.seekp(-1, ios::end);
		outfile << endl;
		outfile << "#HLT (Pendolino) - Run: " << fRunNumber << ", Time: " <<
				ts.AsString() << endl;
		outfile << entryPath.Data() << endl;
		outfile.close();

		TString msg(" +++ Included missing entry '");
		msg += entryPath + "' in Taxi list file.";
		Log(kHLTInterfaceModule, msg.Data());
		bRet = kTRUE;
		
	} else {
		TString msg(" ~~~ Unable to open Pendolino list file '");
	   	msg += filename + "' for Taxi. Creating new one.";
		Log(kHLTInterfaceModule, msg.Data());
		outfile.open(filename, ios_base::out);
		
		if (outfile.is_open()) {
			outfile << "# Automatic generated Taxi list." << endl;
		   	outfile << "# It contains the OCDB entries required by the Pendolino." 
					<< endl << "#" << endl;
			outfile << "#    !!! DON'T EDIT THIS FILE (if you don't know what you are doing) !!!"
					<< endl << endl;
			outfile << "#HLT (Pendolino) - Run: " << fRunNumber << ", Time: " << 
					ts.AsString() << endl;
			outfile << entryPath.Data() << endl;
			outfile.close();
			bRet = kTRUE;
		
		} else {
			msg=" *** Unable to create Pendolino list file '";
			msg += filename + "' for Taxi. Continueing without list update...";
			Log(kHLTInterfaceModule, msg.Data());
		}	
	}
	
	return bRet;
}


void AliHLTPendolino::Log(const char* detector, const char* message) {
	// logging function
	fpLogger->log(detector, message);
	// refer data to a Pendolino Logger, which can take care of it
}


void AliHLTPendolino::RegisterPreprocessor(AliPreprocessor* preprocessor) {
	// registers a PredictionProcessor
	if (preprocessor == 0) {
		Log(kHLTInterfaceModule, 
				" *** ERROR: Cannot register NULL pointer as PredictionProcessor.");
		return;	
	}

    TString detector(preprocessor->GetName());

    if (fPredictionProcessorMap.GetValue(detector.Data())) {
        Log(kHLTInterfaceModule, " ~~~ Already registered PredictionProcessor '" +
                detector + "'. Ignoring call.");
        return;
    }
	// store as AliPreprocessor* and make cast to AliHLTPredictionProcessorInterface*
	// later, when accesing them.
    fPredictionProcessorMap.Add(new TObjString(detector), preprocessor);

/*	
	TString detector(preprocessor->GetName());
	AliHLTPredictionProcessorInterface* predictProc = 0;
//	UInt_t retVal = 0;

	// TODO move this in seperated call outside RegisterPreprocessor(..)
	// safety reason, since preprocessor is not completely generated yet
	if (!preprocessor->ProcessDCS()) {
		Log(kHLTInterfaceModule, " *** PredictionProcessor '" + detector +
				"' not registered, because it will not process DCS values.");
		return;
	}
	Log(kHLTInterfaceModule, "Module Processes DCS values, Registering PredictionProc " 
			+ detector);
	
	// don't use this check, if there are several PreProcs from one detector
	// they will have all have different names
	//if (GetDetPos(detector.Data()) < 0) {
	//	Log(kHLTInterfaceModule, "   *** Invalid detector name: " + detector);
	//}

	// Check if preprocessor is actually PredictionProcessor
	try {

		predictProc = reinterpret_cast<AliHLTPredictionProcessorInterface*> 
				(preprocessor);
// Don't use dynamic_cast or C-style cast, they only rename the pointer/object, 
// but don't import the extended members -> use reinterpret_cast. Maybe perform 
// dynamic_cast check in  other function, which is not called inside a C-tor 
// of AliHLTPredictionProcessorInterface.
		
// ATTENTION: Don't call any functions of AliHLTPredictionProcessorInterface here
// the object has not been completely generated yet, only AliPreprocessor part
// is available. Call of these function should be performed in seperate call outside
// RegisterPreprocessor(..).)

	} catch (std::bad_cast) { 
		// failed -> is not a AliHLTPredictionProcessorInterface implementation
		// -> discarding call
		Log(kHLTInterfaceModule, " *** Cannot register PredictionProcessor '" + detector +
				"'. Does not implement the AliHLTPredictionProcessorInterface.");
		return;
	} catch (std::exception& e) {
		Log(kHLTInterfaceModule, " *** Exception in the registering of the PredictProc.");		
	}

	if (fPredictionProcessorMap.GetValue(detector.Data())) {
		Log(kHLTInterfaceModule, " ~~~ Already registered PredictionProcessor '" +
				detector + "'. Ignoring call.");
		return;
	}

	fPredictionProcessorMap.Add(new TObjString(detector), predictProc);
*/
}


UInt_t AliHLTPendolino::setToPredictMaking() {
	// switches prdiction making on in all registered PredictioProcessors
	UInt_t retVal = 0;

	// get an iterator for the map
	TMapIter iter(&fPredictionProcessorMap, kIterForward);
	AliHLTPredictionProcessorInterface* aPredict;
	TObject* key = 0;	
	
	// get each key inside the map
	while ((key = iter.Next())) {
		TString detector = key->GetName();
		
		try {
			// get value for the key
			aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*>
					(fPredictionProcessorMap.GetValue(key));
		
			if (aPredict == 0) {
				Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
						+ detector +
						"'. Does not implement the AliHLTPredictionProcessorInterface.");
				continue;
			}
//			detector = aPredict->GetName();
			
			if ((aPredict->ProcessDCS()) && (aPredict->makePrediction() == 0)) {
				retVal++;
			} else {
				Log(kHLTInterfaceModule, " *** PredictionProcessor '" + detector
						+ "' does not allow DCS processing or failed to init prediction making.");
			}
		} catch (std::bad_cast) {
	        // failed -> is not a AliHLTPredictionProcessorInterface implementation
    	    // -> discarding call
        	Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '" 
					+ detector +
            	    "'. Does not implement the AliHLTPredictionProcessorInterface.");
        	continue;

		} catch (std::exception& e) {
			Log(kHLTInterfaceModule, " *** Exception in call for makePrediction of " 
					+ detector + ".");
			continue;
		}
	}
	return retVal;
}


Int_t AliHLTPendolino::setToPredictMaking(TString detector) {
	// switches prediction making on in chosen PreditionProcessor
	Int_t retVal = 0;
	AliHLTPredictionProcessorInterface* aPredict = 0;
	
	try {
		// get the value for the key
		TObject* object = fPredictionProcessorMap.GetValue(detector.Data());

		if (object == 0) {
			Log(kHLTInterfaceModule, " *** No PredictionProcessor for '" +
					detector + "' registered.");
			return kHLTPendolinoModuleNotExisting;
		}
				
		aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*> (object);

		if (aPredict == 0) {
			Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
					+ detector +
					"'. Does not implement the AliHLTPredictionProcessorInterface.");
			return kHLTPendolinoNotPredictProc;
		}
//            detector = aPredict->GetName();

		if (!((aPredict->ProcessDCS()) && (aPredict->makePrediction() == 0))) {
			Log(kHLTInterfaceModule, " *** PredictionProcessor '" + detector +
					"' does not allow DCS processing or failed to init prediction making.");
			retVal = kHLTPendolinoNoDCS;
		}
		
	} catch (std::bad_cast) {
		// failed -> is not a AliHLTPredictionProcessorInterface implementation
		// -> discarding call
		Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
				+ detector +
				"'. Does not implement the AliHLTPredictionProcessorInterface.");
		retVal = kHLTPendolinoBadCast;

	} catch (std::exception& e) {
		Log(kHLTInterfaceModule, " *** Exception in call for makePrediction of "
				+ detector + ".");
		retVal = kHLTPendolinoException;
    }
	
    return retVal;
}


Int_t AliHLTPendolino::prepareDCSValues(TString detector, TMap* DCSValues) {
	// function to prepare retrieved DCS values
	Int_t retVal = 0;
	AliHLTPredictionProcessorInterface* aPredict = 0;

	try {
		// get the value for the key
		TObject* object = fPredictionProcessorMap.GetValue(detector.Data());
		
		if (object == 0) {
			Log(kHLTInterfaceModule, " *** No PredictionProcessor for '" +
					detector + "' registered.");
			return kHLTPendolinoModuleNotExisting;
		}
				
		aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*> (object);

		if (aPredict == 0) {
			Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
					+ detector +
					"'. Does not implement the AliHLTPredictionProcessorInterface.");
			return kHLTPendolinoNotPredictProc;
		}

		retVal = aPredict->Process(DCSValues);


	} catch (std::bad_cast) {
		// failed -> is not a AliHLTPredictionProcessorInterface implementation
		// -> discarding call
		Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
				+ detector +
				"'. Does not implement the AliHLTPredictionProcessorInterface.");
		retVal = kHLTPendolinoBadCast;

	} catch (std::exception& e) {
		Log(kHLTInterfaceModule, " *** Exception in call prepareDCSValues of "
				+ detector + ".");
		retVal = kHLTPendolinoException;
	}
	
	return retVal;	
}

TMap* AliHLTPendolino::emulateDCSMap(TString detector, TString aliasName) {
	// function to generate test data of given PredictionProcessor
	TMap* result = NULL;
	AliHLTPredictionProcessorInterface* aPredict = 0;

    try {
        // get the value for the key
        TObject* object = fPredictionProcessorMap.GetValue(detector.Data());

        if (object == 0) {
            Log(kHLTInterfaceModule, " *** No PredictionProcessor for '" +
                    detector + "' registered.");
            return result;
        }

        aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*> (object);

        if (aPredict == 0) {
            Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
                    + detector +
                    "'. Does not implement the AliHLTPredictionProcessorInterface.");
            return result;
        }

        result = aPredict->produceTestData(aliasName);


    } catch (std::bad_cast) {
        // failed -> is not a AliHLTPredictionProcessorInterface implementation
        // -> discarding call
        Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
                + detector +
                "'. Does not implement the AliHLTPredictionProcessorInterface.");

    } catch (std::exception& e) {
        Log(kHLTInterfaceModule, " *** Exception in call emulateDCSMap of "
                + detector + ".");
    }
	return result;
}


Int_t AliHLTPendolino::initPredictProc(TString detector, Int_t run, 
			UInt_t startTime, UInt_t endTime) {
	// initializes given PredictionProcessor (defined by detector name)
    Int_t retVal = 0;
    AliHLTPredictionProcessorInterface* aPredict = 0;

    try {
        // get the value for the key
        TObject* object = fPredictionProcessorMap.GetValue(detector.Data());

        if (object == 0) {
            Log(kHLTInterfaceModule, " *** No PredictionProcessor for '" +
                    detector + "' registered.");
            return kHLTPendolinoModuleNotExisting;
        }

        aPredict = dynamic_cast<AliHLTPredictionProcessorInterface*> (object);

        if (aPredict == 0) {
            Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
                    + detector +
                    "'. Does not implement the AliHLTPredictionProcessorInterface.");
            return kHLTPendolinoNotPredictProc;
        }
		
		// Initialize Prediction Processor
		aPredict->Initialize(run, startTime, endTime);

    } catch (std::bad_cast) {
        // failed -> is not a AliHLTPredictionProcessorInterface implementation
        // -> discarding call
        Log(kHLTInterfaceModule, " *** Cannot use PredictionProcessor '"
                + detector +
                "'. Does not implement the AliHLTPredictionProcessorInterface.");
        retVal = kHLTPendolinoBadCast;

    } catch (std::exception& e) {
        Log(kHLTInterfaceModule, " *** Exception in call prepareDCSValues of "
                + detector + ".");
        retVal = kHLTPendolinoException;
    }

    return retVal;
}



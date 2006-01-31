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

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
// AliCDBLocal										       //
// access class to a DataBase in a local storage  			                       //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TFile.h>
#include <TKey.h>

#include "AliCDBLocal.h"
#include "AliCDBEntry.h"
#include "AliLog.h"

ClassImp(AliCDBLocal)

//_____________________________________________________________________________
AliCDBLocal::AliCDBLocal(const char* baseDir):
fBaseDirectory(baseDir) 
{
// constructor

	// check baseDire: trying to cd to baseDir; if it does not exist, create it
	void* dir = gSystem->OpenDirectory(baseDir);
	if (dir == NULL) {
		if (gSystem->mkdir(baseDir, kTRUE)) {
			AliError(Form("Can't open directory <%s>!", baseDir));
		}

	} else {
		AliDebug(2,Form("Folder <%s> found",fBaseDirectory.Data()));
		gSystem->FreeDirectory(dir);
	}
}

//_____________________________________________________________________________
AliCDBLocal::~AliCDBLocal() {
// destructor

}


//_____________________________________________________________________________
Bool_t AliCDBLocal::FilenameToId(const char* filename, AliCDBRunRange& runRange,
// build AliCDBId from filename numbers

	Int_t& version, Int_t& subVersion) {

        Ssiz_t mSize;

	// valid filename: Run#firstRun_#lastRun_v#version_s#subVersion.root
        TRegexp keyPattern("^Run[0-9]+_[0-9]+_v[0-9]+_s[0-9]+.root$");
        keyPattern.Index(filename, &mSize);
        if (!mSize) {
		AliDebug(2, Form("Bad filename <%s>.", filename));
                return kFALSE;
        }

	TString idString(filename);
	idString.Resize(idString.Length() - sizeof(".root") + 1);

        TObjArray* strArray = (TObjArray*) idString.Tokenize("_");

	TString firstRunString(((TObjString*) strArray->At(0))->GetString());
	runRange.SetFirstRun(atoi(firstRunString.Data() + 3));
	runRange.SetLastRun(atoi(((TObjString*) strArray->At(1))->GetString()));
	
	TString verString(((TObjString*) strArray->At(2))->GetString());
	version = atoi(verString.Data() + 1);

	TString subVerString(((TObjString*) strArray->At(3))->GetString());
        subVersion = atoi(subVerString.Data() + 1);

        delete strArray;

        return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliCDBLocal::IdToFilename(const AliCDBRunRange& runRange, Int_t version,
	Int_t subVersion, TString& filename) {
// build file name from AliCDBId data (run range, version, subVersion)

	if (!runRange.IsValid()) {
		AliWarning(Form("Invalid run range <%d, %d>.", 
			runRange.GetFirstRun(), runRange.GetLastRun()));
		return kFALSE;
	}

	if (version < 0) {
		AliWarning(Form("Invalid version <%d>.", version));
                return kFALSE;
	}

	if (subVersion < 0) {
		AliWarning(Form("Invalid subversion <%s>.", subVersion));
		return kFALSE;
	}
 
        filename += "Run";
	filename += runRange.GetFirstRun();
        filename += "_";
        filename += runRange.GetLastRun();
	filename += "_v";
	filename += version;
	filename += "_s";
	filename += subVersion;
	filename += ".root";

        return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliCDBLocal::PrepareId(AliCDBId& id) {
// prepare id (version, subVersion) of the object that will be stored (called by PutEntry)

	TString dirName;
	dirName += fBaseDirectory;
	dirName += '/';
	dirName += id.GetPath();

	// go to the path; if directory does not exist, create it
	void* dirPtr = gSystem->OpenDirectory(dirName);
	if (!dirPtr) {
		gSystem->mkdir(dirName, kTRUE);
                dirPtr = gSystem->OpenDirectory(dirName);

                if (!dirPtr) {
                        AliError(Form("Can't create directory <%s>!", 
					dirName.Data()));
                        return kFALSE;
                }
	}

	const char* filename;
	AliCDBRunRange aRunRange; // the runRange got from filename 
 	AliCDBRunRange lastRunRange(-1,-1); // highest runRange found
	Int_t aVersion, aSubVersion; // the version subVersion got from filename
	Int_t lastVersion = 0, lastSubVersion = -1; // highest version and subVersion found

	if (!id.HasVersion()) { // version not specified: look for highest version & subVersion
				
		while ((filename = gSystem->GetDirEntry(dirPtr))) { // loop on the files

			TString aString(filename);
			if (aString == "." || aString == "..") continue;
	
			if (!FilenameToId(filename, aRunRange, aVersion, 
				aSubVersion)) {
				AliWarning(Form(
					"Bad filename <%s>! I'll skip it.", 
					filename));
				continue;
			}
			
			if (!aRunRange.Overlaps(id.GetAliCDBRunRange())) continue;
			if(aVersion < lastVersion) continue;
			if(aVersion > lastVersion) lastSubVersion = -1;
			if(aSubVersion < lastSubVersion) continue;
			lastVersion = aVersion;
			lastSubVersion = aSubVersion;
			lastRunRange = aRunRange;
		}

		id.SetVersion(lastVersion);
		id.SetSubVersion(lastSubVersion + 1);

	} else { // version specified, look for highest subVersion only
		
		while ((filename = gSystem->GetDirEntry(dirPtr))) { // loop on the files
			
			TString aString(filename);
			if (aString == "." || aString == "..") {
				continue;
			}

			if (!FilenameToId(filename, aRunRange, aVersion, 
				aSubVersion)) {
				AliWarning(Form(
					"Bad filename <%s>!I'll skip it.",
					filename));	
				continue;
			}

			if (aRunRange.Overlaps(id.GetAliCDBRunRange()) 
				&& aVersion == id.GetVersion()
				&& aSubVersion > lastSubVersion) {
				lastSubVersion = aSubVersion;
				lastRunRange = aRunRange;
			}
	
		}
		
		id.SetSubVersion(lastSubVersion + 1);
	}

	gSystem->FreeDirectory(dirPtr);

	TString lastStorage = id.GetLastStorage();
	if(lastStorage.Contains(TString("grid"), TString::kIgnoreCase) &&
	   id.GetSubVersion() > 0 ){
		AliError(Form("Grid to Local Storage error! local object with version v%d_s%d found:",id.GetVersion(), id.GetSubVersion()-1));
		AliError(Form("This object has been already transferred from Grid (check v%d_s0)!",id.GetVersion()));
		return kFALSE;
	}

	if(lastStorage.Contains(TString("new"), TString::kIgnoreCase) &&
	   id.GetSubVersion() > 0 ){
		AliWarning(Form("*** WARNING! a NEW object is being stored with version v%d_s%d",
					id.GetVersion(),id.GetSubVersion()));
		AliWarning(Form("and it will hide previously stored object with v%d_s%d!",
					id.GetVersion(),id.GetSubVersion()-1));
	}

 	if(!lastRunRange.IsAnyRange() && !(lastRunRange.IsEqual(& id.GetAliCDBRunRange()))) 
    		AliWarning(Form("Run range modified w.r.t. previous version (Run%d_%d_v%d_s%d)",
    		     	lastRunRange.GetFirstRun(), lastRunRange.GetLastRun(), 
			id.GetVersion(), id.GetSubVersion()-1));

	return kTRUE;
}

//_____________________________________________________________________________
AliCDBId AliCDBLocal::GetId(const AliCDBId& query) {
// look for filename matching query (called by GetEntry)

	AliCDBId result(query.GetAliCDBPath(), -1, -1, -1, -1);

	TString dirName;
	dirName += fBaseDirectory;
	dirName += '/';
	dirName += query.GetPath(); // dirName = fDBPath/idPath

	void* dirPtr = gSystem->OpenDirectory(dirName); 
	if (!dirPtr) {
   	 	AliError(Form("Directory <%s> not found", (query.GetPath()).Data()));
   	 	AliError(Form("in DB folder %s", fBaseDirectory.Data()));
		return result;
	}

	const char* filename;	
	
	AliCDBRunRange aRunRange; // the runRange got from filename
	Int_t aVersion, aSubVersion; // the version and subVersion got from filename

	if (!query.HasVersion()) { // neither version and subversion specified -> look for highest version and subVersion
		
		while ((filename = gSystem->GetDirEntry(dirPtr))) { // loop on files
			
			TString aString(filename);
			if (aString == "." || aString == "..") continue;

			if (!FilenameToId(filename, aRunRange, aVersion, aSubVersion)) continue;
                        // aRunRange, aVersion, aSubVersion filled from filename

			if (!aRunRange.Comprises(query.GetAliCDBRunRange())) continue;
			// aRunRange contains requested run!
			
			if (result.GetVersion() < aVersion) {
				result.SetVersion(aVersion);
				result.SetSubVersion(aSubVersion);

				result.SetFirstRun(
					aRunRange.GetFirstRun());
				result.SetLastRun(
					aRunRange.GetLastRun());

			} else if (result.GetVersion() == aVersion 
				&& result.GetSubVersion() 
					< aSubVersion) {

				result.SetSubVersion(aSubVersion);

                        	result.SetFirstRun(
					aRunRange.GetFirstRun());
                        	result.SetLastRun(
					aRunRange.GetLastRun());
			} else if (result.GetVersion() == aVersion
				&& result.GetSubVersion() == aSubVersion){
              			AliError(Form("More than one object valid for run %d, version %d_%d!", 
		       			query.GetFirstRun(), aVersion, aSubVersion));
	      			result.SetRunRange(-1,-1); result.SetVersion(-1); result.SetSubVersion(-1);
	      			return result; 
				}
		}
		
	} else if (!query.HasSubVersion()) { // version specified but not subversion -> look for highest subVersion

		result.SetVersion(query.GetVersion());

		while ((filename = gSystem->GetDirEntry(dirPtr))) { // loop on files

                        TString aString(filename);
                        if (aString == "." || aString == "..") continue;

			if (!FilenameToId(filename, aRunRange, aVersion, aSubVersion)) continue;       
                        // aRunRange, aVersion, aSubVersion filled from filename

                        if (!aRunRange.Comprises(query.GetAliCDBRunRange())) continue; 
			// aRunRange contains requested run!
			
			if(query.GetVersion() != aVersion) continue;
			// aVersion is requested version!
			
	 		if(result.GetSubVersion() == aSubVersion){
              			AliError(Form("More than one object valid for run %d, version %d_%d!", 
		       			query.GetFirstRun(), aVersion, aSubVersion));
	     			result.SetRunRange(-1,-1); result.SetVersion(-1); result.SetSubVersion(-1);
	     			return result; 
	 		}
			if( result.GetSubVersion() < aSubVersion) {

                                result.SetSubVersion(aSubVersion);

                                result.SetFirstRun(
					aRunRange.GetFirstRun());
                                result.SetLastRun(
					aRunRange.GetLastRun());
			} 
                }

	} else { // both version and subversion specified

		while ((filename = gSystem->GetDirEntry(dirPtr))) { // loop on files

                        TString aString(filename);
                        if (aString == "." || aString == "..") continue;

                        if (!FilenameToId(filename, aRunRange, aVersion, aSubVersion)) continue;
                        // aRunRange, aVersion, aSubVersion filled from filename
 			
			if (!aRunRange.Comprises(query.GetAliCDBRunRange())) continue;
			// aRunRange contains requested run!

			if(query.GetVersion() != aVersion || query.GetSubVersion() != aSubVersion) continue; 
			// aVersion and aSubVersion are requested version and subVersion!
			
			if(result.GetVersion() == aVersion && result.GetSubVersion() == aSubVersion){
              			AliError(Form("More than one object valid for run %d, version %d_%d!", 
		       			query.GetFirstRun(), aVersion, aSubVersion));
	     			result.SetRunRange(-1,-1); result.SetVersion(-1); result.SetSubVersion(-1);
	     			return result; 
			}
			result.SetVersion(aVersion);
		        result.SetSubVersion(aSubVersion);
			result.SetFirstRun(aRunRange.GetFirstRun());
			result.SetLastRun(aRunRange.GetLastRun());
			
		}
	}

	gSystem->FreeDirectory(dirPtr);

	return result;
}

//_____________________________________________________________________________
AliCDBEntry* AliCDBLocal::GetEntry(const AliCDBId& queryId) {
// get AliCDBEntry from the database

	AliCDBId dataId;
	
	// look for a filename matching query requests (path, runRange, version, subVersion)
	if (!queryId.HasVersion()) {
		// if version is not specified, first check the selection criteria list
		dataId = GetId(GetSelection(queryId));
	} else {
		dataId = GetId(queryId);
	}

	if (!dataId.IsSpecified()) return NULL;

	TString filename;
	if (!IdToFilename(dataId.GetAliCDBRunRange(), dataId.GetVersion(),
		 dataId.GetSubVersion(), filename)) {

		AliError("Bad data ID encountered! Subnormal error!");
                return NULL;
	}
	
	filename.Prepend((fBaseDirectory +'/' + queryId.GetPath() + '/'));

	TFile file(filename, "READ"); // open file
	if (!file.IsOpen()) {
		AliError(Form("Can't open file <%s>!", filename.Data()));
                return NULL;
	}

	// get the only AliCDBEntry object from the file
	// the object in the file is an AliCDBEntry entry named "AliCDBEntry"
	
	TObject* anObject = file.Get("AliCDBEntry");
	if (!anObject) {
		AliError("Bad storage data: NULL entry object!");
		return NULL;
	}

	if (AliCDBEntry::Class() != anObject->IsA()) {
		AliError("Bad storage data: Invalid entry object!");
		return NULL;
	}
	
 	AliCDBId entryId = ((AliCDBEntry* ) anObject)->GetId();
 
	// The object's Id are not reset during storage
	// If object's Id runRange or version do not match with filename,
	// it means that someone renamed file by hand. In this case a warning msg is issued.
	
	((AliCDBEntry*) anObject)-> SetLastStorage("local");
 
 	if(!((entryId.GetAliCDBRunRange()).IsEqual(& dataId.GetAliCDBRunRange())) || 
    		(entryId.GetVersion() != dataId.GetVersion()) || (entryId.GetSubVersion() != dataId.GetSubVersion())){
    		AliWarning(Form("Either object Id's RunRange or version do noth match with file name:"));
    		AliWarning("someone renamed file by hand!");
        }
	
	// close file, return retieved entry
	file.Close();
	return (AliCDBEntry*) anObject;
}

//_____________________________________________________________________________
void AliCDBLocal::GetEntriesForLevel0(const char* level0,
	const AliCDBId& queryId, TList* result) {
// multiple request (AliCDBStorage::GetAll)

	TString level0Dir;
	level0Dir += fBaseDirectory;
	level0Dir += '/';
	level0Dir += level0;
	
	void* level0DirPtr = gSystem->OpenDirectory(level0Dir);
	if (!level0DirPtr) {
		AliError(Form("Can't open level0 directory <%s>!", 
			level0Dir.Data()));
                return;
	} 

	const char* level1;
	while ((level1 = gSystem->GetDirEntry(level0DirPtr))) {

		TString level1Str(level1);
		if (level1Str == "." || level1Str == "..") {
			continue;
		}

                if (queryId.GetAliCDBPath().Level1Comprises(level1)) {
			GetEntriesForLevel1(level0, level1, queryId, result);
        	}
	}

	gSystem->FreeDirectory(level0DirPtr);
}

//_____________________________________________________________________________
void AliCDBLocal::GetEntriesForLevel1(const char* level0, const char* level1,
	const AliCDBId& queryId, TList* result) {
// multiple request (AliCDBStorage::GetAll)

	TString level1Dir;
	level1Dir += fBaseDirectory;
	level1Dir += '/';
	level1Dir += level0;
	level1Dir += '/';
	level1Dir += level1;

	void* level1DirPtr = gSystem->OpenDirectory(level1Dir);
	if (!level1DirPtr) {
		AliError(Form("Can't open level1 directory <%s>!", 
			level1Dir.Data()));
                return;
	}

	const char* level2;
	while ((level2 = gSystem->GetDirEntry(level1DirPtr))) {

		TString level2Str(level2);
		if (level2Str == "." || level2Str == "..") {
			continue;
		}

		if (queryId.GetAliCDBPath().Level2Comprises(level2)) {
			
			AliCDBPath entryPath(level0, level1, level2);
        		AliCDBId entryId(entryPath, queryId.GetAliCDBRunRange(),
		                queryId.GetVersion(), queryId.GetSubVersion());

		        AliCDBEntry* anEntry = GetEntry(entryId);
	        	if (anEntry) {
        		        result->Add(anEntry);
		        }
		}
	}

	gSystem->FreeDirectory(level1DirPtr);
}

//_____________________________________________________________________________
TList* AliCDBLocal::GetEntries(const AliCDBId& queryId) {
// multiple request (AliCDBStorage::GetAll)

	void* storageDirPtr = gSystem->OpenDirectory(fBaseDirectory);
	if (!storageDirPtr) {
		AliError(Form("Can't open storage directory <%s>",
			fBaseDirectory.Data()));
		return NULL;
	}	

	TList* result = new TList();
	result->SetOwner();

	const char* level0;
	while ((level0 = gSystem->GetDirEntry(storageDirPtr))) {
		
		TString level0Str(level0);
		if (level0Str == "." || level0Str == "..") {
			continue;
		}

		if (queryId.GetAliCDBPath().Level0Comprises(level0)) {
			GetEntriesForLevel0(level0, queryId, result);	
		}
	}	
	
	gSystem->FreeDirectory(storageDirPtr);

	return result;	
}

//_____________________________________________________________________________
Bool_t AliCDBLocal::PutEntry(AliCDBEntry* entry) {
// put an AliCDBEntry object into the database

	AliCDBId& id = entry->GetId();

	// set version and subVersion for the entry to be stored
	if (!PrepareId(id)) return kFALSE;		

	
	// build filename from entry's id
	TString filename;
	if (!IdToFilename(id.GetAliCDBRunRange(), id.GetVersion(),
		id.GetSubVersion(), filename)) {

		AliError("Bad ID encountered! Subnormal error!");
		return kFALSE;
	}
	
	filename.Prepend(fBaseDirectory +'/' + id.GetPath() + '/');
	
	// open file
	TFile file(filename, "CREATE");
	if (!file.IsOpen()) {
		AliError(Form("Can't open file <%s>!", filename.Data()));
		return kFALSE;
	}
	
	entry->SetVersion(id.GetVersion());
	entry->SetSubVersion(id.GetSubVersion());
	
	// write object (key name: "AliCDBEntry")
	Bool_t result = file.WriteTObject(entry, "AliCDBEntry");
	if (!result) AliError(Form("Can't write entry to file: %s", filename.Data()));

	file.Close();
        if(result) AliInfo(Form("AliCDBEntry stored into file %s",filename.Data()));

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
// AliCDBLocal factory  			                                               //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(AliCDBLocalFactory)

//_____________________________________________________________________________
Bool_t AliCDBLocalFactory::Validate(const char* dbString) {
// check if the string is valid local URI

        TRegexp dbPattern("^local://.+$");

        return TString(dbString).Contains(dbPattern);
}

//_____________________________________________________________________________
AliCDBParam* AliCDBLocalFactory::CreateParameter(const char* dbString) {
// create AliCDBLocalParam class from the URI string

	if (!Validate(dbString)) {
		return NULL;
	}

	TString pathname(dbString + sizeof("local://") - 1);
	
	gSystem->ExpandPathName(pathname);

	if (pathname[0] != '/') {
		pathname.Prepend(TString(gSystem->WorkingDirectory()) + '/');
	}

	AliInfo(pathname);

	return new AliCDBLocalParam(pathname);       
}

//_____________________________________________________________________________
AliCDBStorage* AliCDBLocalFactory::Create(const AliCDBParam* param) {
// create AliCDBLocal storage instance from parameters
	
	if (AliCDBLocalParam::Class() == param->IsA()) {
		
		const AliCDBLocalParam* localParam = 
			(const AliCDBLocalParam*) param;
		
		return new AliCDBLocal(localParam->GetPath());
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
// AliCDBLocal Parameter class  			                                       //                                          //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(AliCDBLocalParam)

//_____________________________________________________________________________
AliCDBLocalParam::AliCDBLocalParam() {
// default constructor

}

//_____________________________________________________________________________
AliCDBLocalParam::AliCDBLocalParam(const char* dbPath):
fDBPath(dbPath)
{	
// constructor

	SetType("local");
	SetURI(TString("local://") + dbPath);
}

//_____________________________________________________________________________
AliCDBLocalParam::~AliCDBLocalParam() {
// destructor

}

//_____________________________________________________________________________
AliCDBParam* AliCDBLocalParam::CloneParam() const {
// clone parameter

        return new AliCDBLocalParam(fDBPath);
}

//_____________________________________________________________________________
ULong_t AliCDBLocalParam::Hash() const {
// return Hash function

       return fDBPath.Hash();
}

//_____________________________________________________________________________
Bool_t AliCDBLocalParam::IsEqual(const TObject* obj) const {
// check if this object is equal to AliCDBParam obj

        if (this == obj) {
                return kTRUE;
        }

        if (AliCDBLocalParam::Class() != obj->IsA()) {
                return kFALSE;
        }

        AliCDBLocalParam* other = (AliCDBLocalParam*) obj;

        return fDBPath == other->fDBPath;
}


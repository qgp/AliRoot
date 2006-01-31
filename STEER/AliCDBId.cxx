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

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBId						   //
//  Identity of an object stored into a database:  		   //
//  path, run validity range, version, subVersion 		   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include "AliCDBId.h"

ClassImp(AliCDBId)

//_____________________________________________________________________________
AliCDBId::AliCDBId():
fPath(), 
fRunRange(-1,-1), 
fVersion(-1), 
fSubVersion(-1),
fLastStorage("new")
{
// constructor

}

//_____________________________________________________________________________
AliCDBId::AliCDBId(const AliCDBId& other):
TObject(),
fPath(other.fPath), 
fRunRange(other.fRunRange),
fVersion(other.fVersion), 
fSubVersion(other.fSubVersion),
fLastStorage(other.fLastStorage)
{
// constructor

}

//_____________________________________________________________________________
AliCDBId::AliCDBId(const AliCDBPath& path, Int_t firstRun, Int_t lastRun, 
	Int_t version, Int_t subVersion):
fPath(path), 
fRunRange(firstRun, lastRun), 
fVersion(version), 
fSubVersion(subVersion),
fLastStorage("new")
{
// constructor

} 

//_____________________________________________________________________________
AliCDBId::AliCDBId(const AliCDBPath& path, const AliCDBRunRange& runRange, 
	Int_t version, Int_t subVersion):
fPath(path), 
fRunRange(runRange), 
fVersion(version),
fSubVersion(subVersion),
fLastStorage("new")
{
// constructor

}

//_____________________________________________________________________________
AliCDBId::~AliCDBId() {
//destructor

}

//_____________________________________________________________________________
Bool_t AliCDBId::IsValid() const {
// validity check
	
	if (!(fPath.IsValid() && fRunRange.IsValid())) {
		return kFALSE;
	}
	
	// FALSE if doesn't have version but has subVersion
	return !(!HasVersion() && HasSubVersion());
}

//_____________________________________________________________________________
TString AliCDBId::ToString() const {
// returns a string of Id data

	TString result;
	result += "Path \"";
	result += GetPath();
	result += "\"; RunRange [";
	result += GetFirstRun();
	result += ",";
	result += GetLastRun();
	result += "]; Version v";
	result += GetVersion();
	result += "_s";
	result += GetSubVersion();
	result += "; Previous storage ";
	result += fLastStorage;
	
	return result;	
}

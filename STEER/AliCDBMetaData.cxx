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
//  class AliCDBMetaData					   //
//  Set of data describing the object  				   //
//  but not used to identify the object 			   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include "AliCDBMetaData.h"
#include "AliLog.h"

#include <TObjString.h>

ClassImp(AliCDBMetaData)

//_____________________________________________________________________________
AliCDBMetaData::AliCDBMetaData() {
// default constructor

	fProperties.SetOwner(1);
}

//_____________________________________________________________________________
AliCDBMetaData::~AliCDBMetaData() {
// destructor

}

//_____________________________________________________________________________
void AliCDBMetaData::SetProperty(const char* property, TObject* object) {
// add something to the list of properties

	fProperties.Add(new TObjString(property), object);
}

//_____________________________________________________________________________
TObject* AliCDBMetaData::GetProperty(const char* property) const {
// get a property specified by its name (property)

	return fProperties.GetValue(property);
}

//_____________________________________________________________________________
Bool_t AliCDBMetaData::RemoveProperty(const char* property) {
// removes a property

	TObjString objStrProperty(property);
	TObjString* aKey = (TObjString*) fProperties.Remove(&objStrProperty);	

	if (aKey) {
		delete aKey;
		return kTRUE;
	} else {
		return kFALSE;
	}
}

//_____________________________________________________________________________
void AliCDBMetaData::PrintMetaData() {
// print the object's metaData

	AliInfo("**** Object's MetaData set ****");
	AliInfo(Form("  Object's class name:	%s", fObjectClassName.Data()));
	AliInfo(Form("  Responsible:		%s", fResponsible.Data()));
	AliInfo(Form("  Beam period:		%d", fBeamPeriod));
	AliInfo(Form("  AliRoot version:	%s", fAliRootVersion.Data()));
	AliInfo(Form("  Comment:		%s", fComment.Data()));
	AliInfo("  Properties key names:");

	TIter iter(fProperties.GetTable());	
	TPair* aPair;
	while ((aPair = (TPair*) iter.Next())) {
		AliInfo(Form(" 			%s",((TObjString* )aPair->Key())->String().Data()));
	}
	
}

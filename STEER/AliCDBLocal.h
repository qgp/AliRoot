#ifndef ALI_CDB_LOCAL_H
#define ALI_CDB_LOCAL_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBLocal						   //
//  access class to a DataBase in a local storage                  //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include "AliCDBStorage.h"
#include "AliCDBManager.h"

class AliCDBLocal: public AliCDBStorage {
	friend class AliCDBLocalFactory;

public:

	virtual Bool_t IsReadOnly() const {return kFALSE;};
	virtual Bool_t HasSubVersion() const {return kTRUE;};

protected:

	virtual AliCDBEntry* GetEntry(const AliCDBId& queryId);
        virtual TList* GetEntries(const AliCDBId& queryId);
        virtual Bool_t PutEntry(AliCDBEntry* entry);

private:

	AliCDBLocal(const char* baseDir);
	virtual ~AliCDBLocal();
	
	Bool_t FilenameToId(const char* filename, AliCDBRunRange& runRange, 
			Int_t& version, Int_t& subVersion);
	Bool_t IdToFilename(const AliCDBRunRange& runRange, Int_t version, 
			Int_t subVersion, TString& filename);

	Bool_t PrepareId(AliCDBId& id);
	AliCDBId GetId(const AliCDBId& query);

	void GetEntriesForLevel0(const char* level0, const AliCDBId& query, TList* result);
	void GetEntriesForLevel1(const char* level0, const char* Level1,
			const AliCDBId& query, TList* result);

	TString fBaseDirectory; // path of the DB folder

	ClassDef(AliCDBLocal, 0); // access class to a DataBase in a local storage
};

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBLocalFactory					   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliCDBLocalFactory: public AliCDBStorageFactory {

public:

	virtual Bool_t Validate(const char* dbString);
        virtual AliCDBParam* CreateParameter(const char* dbString);

protected:
        virtual AliCDBStorage* Create(const AliCDBParam* param);

        ClassDef(AliCDBLocalFactory, 0);
};

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBLocalParam					   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliCDBLocalParam: public AliCDBParam {
	
public:
	AliCDBLocalParam();
	AliCDBLocalParam(const char* dbPath);
	
	virtual ~AliCDBLocalParam();

	const TString& GetPath() const {return fDBPath;};

	virtual AliCDBParam* CloneParam() const;

        virtual ULong_t Hash() const;
        virtual Bool_t IsEqual(const TObject* obj) const;

private:

	TString fDBPath; // path of the DB folder

	ClassDef(AliCDBLocalParam, 0);
};

#endif

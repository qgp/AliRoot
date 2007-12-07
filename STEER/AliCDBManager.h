#ifndef ALI_CDB_MANAGER_H
#define ALI_CDB_MANAGER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBManager                                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TList.h>
#include <TMap.h>

class AliCDBEntry;
class AliCDBId;
class AliCDBPath;
class AliCDBRunRange;
class AliCDBMetaData;
class AliCDBStorage;
class AliCDBStorageFactory;
class AliCDBParam;

class AliCDBManager: public TObject {

 public:
	enum DataType {kCondition=0, kReference, kPrivate};

	void RegisterFactory(AliCDBStorageFactory* factory);

	Bool_t HasStorage(const char* dbString) const;

	AliCDBParam* CreateParameter(const char* dbString) const;
	AliCDBParam* GetCondParam() const {return fCondParam;}
	AliCDBParam* GetRefParam() const {return fRefParam;}
	static const char* GetDataTypeName(DataType type);

	AliCDBStorage* GetStorage(const char* dbString);
	AliCDBStorage* GetStorage(const AliCDBParam* param);

	TList* GetActiveStorages();

	const TMap* GetStorageMap() const {return fStorageMap;}
	const TList* GetRetrievedIds() const {return fIds;}

	void SetDefaultStorage(const char* dbString);
	void SetDefaultStorage(const AliCDBParam* param);
	void SetDefaultStorage(AliCDBStorage *storage);

	Bool_t IsDefaultStorageSet() const {return fDefaultStorage != 0;}
	AliCDBStorage* GetDefaultStorage() const {return fDefaultStorage;}
	void UnsetDefaultStorage();

	void SetSpecificStorage(const char* calibType, const char* dbString);
	void SetSpecificStorage(const char* calibType, AliCDBParam* param);

	AliCDBStorage* GetSpecificStorage(const char* calibType);

	void SetDrain(const char* dbString);
	void SetDrain(const AliCDBParam* param);
	void SetDrain(AliCDBStorage *storage);

	Bool_t IsDrainSet() const {return fDrainStorage != 0;}

	Bool_t Drain(AliCDBEntry* entry);

	void UnsetDrain(){fDrainStorage = 0x0;}

	AliCDBEntry* Get(const AliCDBId& query);
	AliCDBEntry* Get(const AliCDBPath& path, Int_t runNumber=-1,
				Int_t version = -1, Int_t subVersion = -1);
	AliCDBEntry* Get(const AliCDBPath& path, const AliCDBRunRange& runRange,
				 Int_t version = -1, Int_t subVersion = -1);

	const char* GetURI(const char* path);				 
				 
	AliCDBId* GetId(const AliCDBId& query);
	AliCDBId* GetId(const AliCDBPath& path, Int_t runNumber=-1,
				Int_t version = -1, Int_t subVersion = -1);
	AliCDBId* GetId(const AliCDBPath& path, const AliCDBRunRange& runRange,
				 Int_t version = -1, Int_t subVersion = -1);

	TList* GetAll(const AliCDBId& query);
	TList* GetAll(const AliCDBPath& path, Int_t runNumber=-1,
				Int_t version = -1, Int_t subVersion = -1);
	TList* GetAll(const AliCDBPath& path, const AliCDBRunRange& runRange,
				 Int_t version = -1, Int_t subVersion = -1); 

	Bool_t Put(TObject* object, AliCDBId& id,
			AliCDBMetaData* metaData, DataType type=kPrivate);
	Bool_t Put(AliCDBEntry* entry, DataType type=kPrivate);

	void SetCacheFlag(Bool_t cacheFlag) {fCache=cacheFlag;}
	Bool_t GetCacheFlag() const {return fCache;}
	
	void SetLock(Bool_t lockFlag=kTRUE);
	Bool_t GetLock() const {return fLock;}

	void SetRun(Int_t run);
	Int_t GetRun() const {return fRun;}

	void DestroyActiveStorages();
	void DestroyActiveStorage(AliCDBStorage* storage);

	void QueryCDB();

	void Print(Option_t* option="") const;

	static void Destroy();
	~AliCDBManager();

  	void ClearCache();
  	void UnloadFromCache(const char* path);

	Bool_t IsShortLived(const char* path);

	static AliCDBManager* Instance();

 private:

	static TString fgkCondUri;	// URI of the Conditions data base folder
	static TString fgkRefUri;	// URI of the Reference data base folder

	AliCDBManager();
	AliCDBManager(const AliCDBManager & source);
	AliCDBManager & operator=(const AliCDBManager & source);

	static AliCDBManager* fgInstance; // AliCDBManager instance
	
	AliCDBStorage* GetActiveStorage(const AliCDBParam* param);
	void PutActiveStorage(AliCDBParam* param, AliCDBStorage* storage);

  	void CacheEntry(const char* path, AliCDBEntry* entry);

	AliCDBParam* SelectSpecificStorage(const TString& path);
	

	void Init();
	void InitShortLived();


	TList fFactories; 		//! list of registered storage factories
	TMap fActiveStorages;		//! list of active storages
	TMap fSpecificStorages;         //! list of detector-specific storages
	TMap fEntryCache;    	  	//! cache of the retrieved objects

	TList* fIds;           	//! List of the retrieved object Id's (to be streamed to file)
	TMap* fStorageMap;      //! list of storages (to be streamed to file)
	TList* fShortLived; 	//! List of short lived objects

	AliCDBStorage *fDefaultStorage;	//! pointer to default storage
	AliCDBStorage *fDrainStorage;	//! pointer to drain storage

	AliCDBParam* fCondParam; 	// Conditions data storage parameters
	AliCDBParam* fRefParam;		// Reference data storage parameters

	Int_t fRun;			//! The run number
	Bool_t fCache;			//! The cache flag
	Bool_t fLock; 	//! Lock flag, if ON default storage and run number cannot be reset


	ClassDef(AliCDBManager, 0);
};


/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBStorageFactory                                     //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliCDBParam;
class AliCDBStorageFactory: public TObject {
	friend class AliCDBManager;
	
public:
	virtual ~AliCDBStorageFactory(){}
	virtual Bool_t Validate(const char* dbString) = 0;
	virtual AliCDBParam* CreateParameter(const char* dbString) = 0;	

protected:
	virtual AliCDBStorage* Create(const AliCDBParam* param) = 0;

	ClassDef(AliCDBStorageFactory, 0);
};

/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliCDBParam                                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliCDBParam: public TObject {

public:

	AliCDBParam();
	virtual ~AliCDBParam();

	const TString& GetType() const {return fType;};
	const TString& GetURI() const {return fURI;};

	virtual AliCDBParam* CloneParam() const = 0;

protected:

	void SetType(const char* type) {fType = type;};
	void SetURI(const char* uri) {fURI = uri;};

private:

	TString fType; //! CDB type
	TString fURI;  //! CDB URI

	ClassDef(AliCDBParam, 0);
};

#endif

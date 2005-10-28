#ifndef ALI_SHUTTLE_CONFIG_H
#define ALI_SHUTTLE_CONFIG_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//
// This class keeps the AliShuttle configuration.
// It reads the configuration for LDAP server.
//

#include <TObject.h>
#include <TString.h>
#include <TList.h>
#include <TMap.h>
#include <TLDAPServer.h>

class AliShuttleConfig: public TObject {
public:
	AliShuttleConfig(const char* host, Int_t port = LDAP_PORT, 
			const char* binddn = 0, const char* password = 0, 
			const char* basedn = "dc=alice,dc=cern,dc=ch"); 
	virtual ~AliShuttleConfig();

	Bool_t IsValid() const {return fIsValid;};

	const TList* GetDetectors() const;

	Bool_t HasDetector(const char* detector) const;
	const char* GetHost(const char* detector) const;
	Int_t GetPort(const char* detector) const;
	const TList* GetAliases(const char* detector) const;

	virtual void Print(Option_t* option = NULL) const;

private:
	
	class ConfigHolder: public TObject {
		TString fDetector;
		TString fHost;
		Int_t fPort;
		TList fAliases;		
		Bool_t fIsValid;

	public:
		ConfigHolder(const TLDAPEntry* entry);
		~ConfigHolder();

		const char* GetDetector() const {return fDetector.Data();};
		const char* GetHost() const {return fHost.Data();};
		Int_t GetPort() const {return fPort;};
		const TList* GetAliases() const {return &fAliases;};

		Bool_t IsValid() const {return fIsValid;};

		ClassDef(ConfigHolder, 0);
	};


	Bool_t fIsValid;

	TMap fDetectorMap;
	TList fDetectorList;	

	ClassDef(AliShuttleConfig, 0);
};

#endif


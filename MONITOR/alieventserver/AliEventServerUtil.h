#ifndef __ALIEVENTSERVER_UTIL_H__
#define __ALIEVENTSERVER_UTIL_H__

// Default configuration file
#define ALIEVENTSERVER_CONF "alieventserver.rootrc"

//______________DEFAULT SETTINGS________________
#define DEFAULT_SERVER_HOST "tcp://*"
#define DEFAULT_SERVER_PORT 5024
#define DEFAULT_SERVER_SAVE_RECO_DIR "/local/reco"
#define DEFAULT_CDB_STORAGE "local:///local/OCDB/2013"
#define DEFAULT_CDB_SPEC_STORAGE_PATH1 "/local/reco/GRP/GRP/Data"
#define DEFAULT_CDB_SPEC_STORAGE_VALUE1 ""
#define DEFAULT_CDB_SPEC_STORAGE_PATH2 "/local/reco/GRP/CTP/Config"
#define DEFAULT_CDB_SPEC_STORAGE_VALUE2 ""
#define DEFAULT_CDB_SPEC_STORAGE_PATH3 ""
#define DEFAULT_CDB_SPEC_STORAGE_VALUE3 ""
#define DEFAULT_QA_RUN ":"
#define DEFAULT_QAREF_STORAGE "local://$ALICE_ROOT/QAref"
#define DEFAULT_QA_RUN_GLOBAL 1
#define DEFAULT_RECO_RUN_PLANE_EFF 1
#define DEFAULT_RECO_WRITE_ESDF 0
#define DEFAULT_RECO_WRITE_ALIGN 1
#define DEFAULT_RECO_CLEAN_ESD 0
#define DEFAULT_RECO_DETECTORS "ALL -PHOS -EMCAL"
#define DEFAULT_LOGBOOK_HOST "localhost"
#define DEFAULT_LOGBOOK_PORT 3306
#define DEFAULT_LOGBOOK_DB "logbook"
#define DEFAULT_LOGBOOK_USER "dqm"
#define DEFAULT_LOGBOOK_PASS "dqm123"

#include <TSystem.h>
#include <TString.h>

namespace AliEventServerUtil
{
// return full path to the server configuration file
	inline const char* GetPathToServerConf()
	{
		return Form("%s/MONITOR/%s",
			    gSystem->Getenv("ALICE_ROOT"),
			    ALIEVENTSERVER_CONF);
	}
}

#endif /* __ALIEVENTSERVER_UTIL_H__ */

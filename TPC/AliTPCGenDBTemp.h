/////////////////////////////////////////////////////////////////
// Class to generate temperature sensor data base entries.
//
// Existing data base structure read at start of processsing.
// 20/12-2006 HH.
// Modification log:
/////////////////////////////////////////////////////////////////

#ifndef AliTPCGenDBTemp_h
#define AliTPCGenDBTemp_h

#include <TROOT.h>
#include <TFile.h>
#include <TObjArray.h>

#include "AliTPCSensorTempArray.h"
#include "AliLog.h"
#include "AliDCSGenDB.h"


class AliTPCGenDBTemp : public AliDCSGenDB {

public:

// constructors

  AliTPCGenDBTemp();
  AliTPCGenDBTemp(const char *defaultStorage, const char *specificStorage);
  ~AliTPCGenDBTemp();
  void            MakeCalib(const char *file, const char *fMap,
                            const TTimeStamp& startTime,
			    const TTimeStamp& endTime, Int_t run,
			    const TString& amandaString=0);
  void            MakeConfig(const char *file, Int_t firstRun, Int_t lastRun, 
                            const char *confDir, const TString& amandaString=0);

// functionality

  static TClonesArray* ReadList(const char* fname, const char *title="tempConf", 
                                 const TString& amandaString=0);
  static TTree*   ReadListTree(const char* fname, const char *title="tempConf",
                                 const TString& amandaString=0);

// getters/setters
 

private:
  AliTPCGenDBTemp(const AliTPCGenDBTemp& org);
  AliTPCGenDBTemp& operator= (const AliTPCGenDBTemp& org);

   ClassDef(AliTPCGenDBTemp,1)
};
#endif

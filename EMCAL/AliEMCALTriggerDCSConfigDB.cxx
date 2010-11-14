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

/*




Adapted from TRD
Author: R. GUERNANE LPSC Grenoble CNRS/IN2P3
*/

#include <TClonesArray.h>
#include <TObjArray.h>

#include "AliCDBManager.h"
#include "AliCDBEntry.h"
#include "AliLog.h"

#include "AliEMCALTriggerDCSConfigDB.h"
#include "AliEMCALTriggerDCSConfig.h"
#include "AliEMCALTriggerSTUDCSConfig.h"
#include "AliEMCALTriggerTRUDCSConfig.h"

ClassImp(AliEMCALTriggerDCSConfigDB)

AliEMCALTriggerDCSConfigDB* AliEMCALTriggerDCSConfigDB::fgInstance   = 0;
Bool_t                      AliEMCALTriggerDCSConfigDB::fgTerminated = kFALSE;

//_____________________________________________________________________________
AliEMCALTriggerDCSConfigDB* AliEMCALTriggerDCSConfigDB::Instance()
{
	//
	// Singleton implementation
	// Returns an instance of this class, it is created if neccessary
	//
  
	if (fgTerminated != kFALSE) 
	{
		return 0;
	}

	if (fgInstance == 0) 
	{
		fgInstance = new AliEMCALTriggerDCSConfigDB();
	}

	return fgInstance;
}

//_____________________________________________________________________________
void AliEMCALTriggerDCSConfigDB::Terminate()
{
	//
	// Singleton implementation
	// Deletes the instance of this class and sets the terminated flag,
	// instances cannot be requested anymore
	// This function can be called several times.
	//
  
	fgTerminated = kTRUE;
  
	if (fgInstance != 0) 
	{
		delete fgInstance;
		fgInstance = 0;
	}
}

//_____________________________________________________________________________
AliEMCALTriggerDCSConfigDB::AliEMCALTriggerDCSConfigDB() : TObject()
,fRun(-1)
{
	//
	// Default constructor
	//
	// TODO Default runnumber is set to 0, this should be changed later
	//      to an invalid value (e.g. -1) to prevent
	// TODO invalid calibration data to be used.
	//

	for (Int_t i = 0; i < kCDBCacheSize; ++i) 
	{
		fCDBCache[i]   = 0;
		fCDBEntries[i] = 0;
	}
}

//_____________________________________________________________________________
AliEMCALTriggerDCSConfigDB::AliEMCALTriggerDCSConfigDB(const AliEMCALTriggerDCSConfigDB &c) : TObject(c)
,fRun(-1)
{
	//
	// Copy constructor (not that it make any sense for a singleton...)
	//

	for (Int_t i = 0; i < kCDBCacheSize; ++i) 
	{
		fCDBCache[i]   = 0;
		fCDBEntries[i] = 0;
	}
}

//_____________________________________________________________________________
AliEMCALTriggerDCSConfigDB &AliEMCALTriggerDCSConfigDB::operator=(const AliEMCALTriggerDCSConfigDB &c) 
{
	//
	// Assignment operator (same as above ...)
	//
	if (this != &c) 
	{
		AliFatal("No assignment operator defined");
	}

	return *this;
}

//_____________________________________________________________________________
AliEMCALTriggerDCSConfigDB::~AliEMCALTriggerDCSConfigDB() 
{
	//
	// destructor
	//
	Invalidate();
}

//_____________________________________________________________________________
const TObject *AliEMCALTriggerDCSConfigDB::GetCachedCDBObject(Int_t id)
{
	//
	// Retrieves a cdb object with the given id. The objects are cached as
	// long as the run number is not changed.
	//
	switch (id) 
	{
		// Parameters defined per pad and chamber
		case kIDTriggerConfig : 
			return CacheCDBEntry(kIDTriggerConfig, "EMCAL/Calib/Trigger"); 
			break;
		default:			
			AliError("Object not found!");
			break;
	}

	return 0x0;
}

//_____________________________________________________________________________
AliCDBEntry* AliEMCALTriggerDCSConfigDB::GetCDBEntry(const char *cdbPath)
{
	// 
	// Retrieves an entry with path <cdbPath> from the CDB.
	//
	AliCDBEntry *entry = AliCDBManager::Instance()->Get(cdbPath,fRun);
	
	if (!entry) 
	{ 
		AliError(Form("Failed to get entry: %s",cdbPath));
		return 0; 
	}
  
	return entry;
}

//_____________________________________________________________________________
const TObject *AliEMCALTriggerDCSConfigDB::CacheCDBEntry(Int_t id, const char *cdbPath)
{
	//
	// Caches the entry <id> with cdb path <cdbPath>
	//
  
	if (!fCDBCache[id]) 
	{
		fCDBEntries[id] = GetCDBEntry(cdbPath);
		
		if (fCDBEntries[id]) fCDBCache[id] = fCDBEntries[id]->GetObject();
	}

	return fCDBCache[id];
}

//_____________________________________________________________________________
void AliEMCALTriggerDCSConfigDB::SetRun(Long64_t run)
{
  //
  // Sets current run number. Calibration data is read from the corresponding file.
  // When the run number changes the caching is invalidated.
  //

  if (fRun == run) return;

  fRun = run;

  Invalidate();
}

//_____________________________________________________________________________
void AliEMCALTriggerDCSConfigDB::Invalidate()
{
	//
	// Invalidates cache (when run number is changed).
	//
	for (Int_t i = 0; i < kCDBCacheSize; ++i) 
	{
		if (fCDBEntries[i]) 
		{
			if (AliCDBManager::Instance()->GetCacheFlag() == kFALSE) 
			{
				if ((fCDBEntries[i]->IsOwner() == kFALSE) && (fCDBCache[i])) delete fCDBCache[i];
				
				delete fCDBEntries[i];
			}
			
			fCDBEntries[i] = 0;
			fCDBCache[i]   = 0;
		}
	}
}

//_____________________________________________________________________________
const AliEMCALTriggerDCSConfig* AliEMCALTriggerDCSConfigDB::GetTriggerDCSConfig()
{
	//
	//
	//
	const AliEMCALTriggerDCSConfig* dcsConf = dynamic_cast<const AliEMCALTriggerDCSConfig*>(GetCachedCDBObject(kIDTriggerConfig));
	
	if (!dcsConf) 
	{
		AliError("Trigger DCS configuration not found!");
		return 0x0;
	}
	else
		return dcsConf;
}

//_____________________________________________________________________________
void AliEMCALTriggerDCSConfigDB::GetSTUSegmentation(Int_t ssg[], Int_t spg[], Int_t ssj[], Int_t spj[])
{
	//
	//
	//
	const AliEMCALTriggerDCSConfig* dcsConf = dynamic_cast<const AliEMCALTriggerDCSConfig*>(GetCachedCDBObject(kIDTriggerConfig));
  if(dcsConf){
    AliEMCALTriggerSTUDCSConfig* stuConf = dcsConf->GetSTUDCSConfig();
    if(stuConf){
      Int_t fw = stuConf->GetFw();
      
      switch ( fw )
      {
        case 2223:
          ssg[0] = 1;
          ssg[1] = 1;
          spg[0] = 2;
          spg[1] = 2;
          
          ssj[0] = 4;
          ssj[1] = 4;
          spj[0] = 2;
          spj[1] = 2;
          break;
        default:
          AliError("Firmware version do not match!");
          break;
      }
    }
    else {
      AliError("STUDCSConfig is null!");
    }
  }
  else {
    AliError("DCSConfig is null!");
  }
  
}

//_____________________________________________________________________________
Int_t AliEMCALTriggerDCSConfigDB::GetTRUSegmentation(Int_t iTRU)
{
	//
	const AliEMCALTriggerDCSConfig* dcsConf = dynamic_cast<const AliEMCALTriggerDCSConfig*>(GetCachedCDBObject(kIDTriggerConfig));
  if(dcsConf){	
    AliEMCALTriggerTRUDCSConfig* truConf = dcsConf->GetTRUDCSConfig(iTRU);
    if(truConf){
      Int_t sel = truConf->GetL0SEL();
	
      if (sel & 0x0001)
        return 2;
      else
        return 1;
    } else AliError("TRUDCSConf Null!") ;
  }else AliError("TriggerDCSConf Null!") ;
  
  return -1;
}

//_____________________________________________________________________________
Int_t AliEMCALTriggerDCSConfigDB::GetTRUGTHRL0(Int_t iTRU)
{
	//
	//
	//
	const AliEMCALTriggerDCSConfig* dcsConf = dynamic_cast<const AliEMCALTriggerDCSConfig*>(GetCachedCDBObject(kIDTriggerConfig));
  if(dcsConf){	
    AliEMCALTriggerTRUDCSConfig* truConf = dcsConf->GetTRUDCSConfig(iTRU);
    if(truConf){
      return truConf->GetGTHRL0();
    } else AliError("TRUDCSConf Null!") ;
  }else AliError("TriggerDCSConf Null!") ;
  
  return -1;
}

#include "AliITSPreprocessorSSD.h"

#include "AliCDBMetaData.h"
#include "AliLog.h"
#include "TFile.h"

#include <TTimeStamp.h>
#include <TObjString.h>

#include "AliITSRawStreamSSD.h"
#include "AliITSNoiseSSD.h"
#include "AliITSPedestalSSD.h"
#include "AliITSBadChannelsSSD.h"
#include <Riostream.h>


//
// Author: Enrico Fragiacomo
// Date: 13/10/2006
// 
// SHUTTLE preprocessing class for SSD calibration files

/* $Id$ */

const Int_t AliITSPreprocessorSSD::fgkNumberOfSSD = 1698;

ClassImp(AliITSPreprocessorSSD)

//______________________________________________________________________________________________
void AliITSPreprocessorSSD::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
 
  AliPreprocessor::Initialize(run, startTime, endTime);
  
  Log(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run,
	       TTimeStamp(startTime).AsString(),
	       TTimeStamp(endTime).AsString()));
  
}

//______________________________________________________________________________________________
UInt_t AliITSPreprocessorSSD::Process(TMap* /*dcsAliasMap*/)
{

  // Note. To be modified: dcsAliasMap is not needed but I can not get rid
  // of it unless the base class AliPreprocessor is modified accordingly.

  TObjArray calib_array(fgkNumberOfSSD); 
  TObjArray badch_array(fgkNumberOfSSD); 
  TObjArray ped_array(fgkNumberOfSSD); 
  //Float_t noise=0, gain=0;

  TString runType = GetRunType();
 if(runType == "ELECTRONICS_CALIBRATION_RUN") {

  }
  else if(runType == "PEDESTAL") {

    TList* list = GetFileSources(kDAQ, "CALIBRATION");
    if (list && list->GetEntries() > 0)
      {
	Log("The following sources produced files with the id CALIBRATION");
	list->Print();
	
	// create iterator over list of LDCs (provides list of TObjString)
	TIter next(list);
	TObjString *ok;
	
	// expect to iterate 3 times (LDC0, LDC1, LDC2)
	while ( (ok = (TObjString*) next()) ) {                               
	  
	  TString key = ok->String();
	  
	  TString fileName = GetFile(kDAQ, "CALIBRATION", key.Data());
	  if (fileName.Length() > 0) {
	    
	    Log(Form("Got the file %s, now we can extract some values.", fileName.Data()));
	    
	    TFile *f = new TFile(fileName.Data());
	    if(!f || !f->IsOpen()){
	    	Log("Error opening file!");
		delete list;
		return 2;
	    }
	    
	    TObjArray *cal; 
	    f->GetObject("Noise;1", cal); 
	    if(!cal) {
	    	Log("File does not contain expected data for the noise!");
		delete list;
	    }	    
	    Int_t nmod = cal->GetEntries();
	    for(Int_t mod=0; mod<nmod; mod++) {
	      AliITSNoiseSSD *calib = (AliITSNoiseSSD*) cal->At(mod);
	      if((calib->GetMod()<500)||(calib->GetMod()>2198)) continue;
	      calib_array.AddAt(calib,calib->GetMod()-500);
	    }

	    TObjArray *bad; 
	    f->GetObject("BadChannels;1", bad); 
	    if(!bad) {
	    	Log("File does not contain expected data for bad channels  !");
		delete list;
	    }	    
	    nmod = bad->GetEntries();
	    for(Int_t mod=0; mod<nmod; mod++) {
	      AliITSBadChannelsSSD *badch = (AliITSBadChannelsSSD*) bad->At(mod);
	      if((badch->GetMod()<500)||(badch->GetMod()>2198)) continue;
	      badch_array.AddAt(badch,badch->GetMod()-500);
	    }

	    TObjArray *ped; 
	    f->GetObject("Pedestal;1", ped); 
	    if(!ped) {
	    	Log("File does not contain expected data for the pedestals!");
		delete list;
	    }	    
	    nmod = ped->GetEntries();
	    for(Int_t mod=0; mod<nmod; mod++) {
	      AliITSPedestalSSD *pedel = (AliITSPedestalSSD*) ped->At(mod);
	      if((pedel->GetMod()<500)||(pedel->GetMod()>2198)) continue;
	      ped_array.AddAt(pedel,pedel->GetMod()-500);
	    }

	    f->Close(); delete f;	    
		
	  } else {
	  	Log("GetFile error!");
		delete list;
		return 3;
	  } // if filename
	} // end iteration over LDCs
	
	delete list;
      } else {
      	  Log("GetFileSources error!");
	  if(list) delete list;
	  return 4;
      } // if list
    
      //Now we have to store the final CDB file
      AliCDBMetaData metaData;
      metaData.SetBeamPeriod(0);
      metaData.SetResponsible("Enrico Fragiacomo");
      metaData.SetComment("Fills noise, pedestal and bad channels TObjArray");
  
      if(!Store("Calib", "NoiseSSD", &calib_array, &metaData, 0, 1)) {
	Log("no store");
        return 1;
      }  
      
      if(!Store("Calib", "BadChannelsSSD", &badch_array, &metaData, 0, 1)) {
	Log("no store");
        return 1;
      }  
      
      if(!StoreReferenceData("Calib","PedestalSSD", &ped_array, &metaData)) {
	Log("no store");
	return 1;
      }
	 
  } // end if noise_run
  else {
    Log("Nothing to do");
    return 0;
  }
  
  Log("Database updated");
  return 0; // 0 means success

}


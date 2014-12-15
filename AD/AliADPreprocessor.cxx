#include "AliADPreprocessor.h"
#include "AliADCalibData.h"
#include "AliCDBMetaData.h"
#include "AliCDBEntry.h"
#include "AliDCSValue.h"
#include "AliLog.h"
#include "AliShuttleInterface.h"
#include "AliADDataDCS.h"

#include <TFile.h>
#include <TTimeStamp.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TH1F.h>


class Tlist;

//
//  This class is  a simple preprocessor for AD detector.
//
//  It gets High Voltage values for a given run from DCS and Pedestal values from DAQ 
//  and writes them as Calibration MetaData into OCDB/AD/Calib/Data
//  It also retrieves FEE parameters from DCS archive   
//  and writes them as Trigger MetaData into OCDB/AD/Trigger/Data 
//  (to be used for trigger simulation)
//

ClassImp(AliADPreprocessor)

//______________________________________________________________________________________________
AliADPreprocessor::AliADPreprocessor(AliShuttleInterface* shuttle) :
	AliPreprocessor("AD0", shuttle),
	fDCSData(0)
 
{
  // constructor  
  
  AddRunType("STANDALONE_PULSER");
  AddRunType("STANDALONE_BC");
  AddRunType("PHYSICS");
    
}

//______________________________________________________________________________________________
AliADPreprocessor::~AliADPreprocessor()
{
  // destructor
	delete fDCSData;
	
}

//______________________________________________________________________________________________
void AliADPreprocessor::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
  // Creates AliADDataDCS object

   AliPreprocessor::Initialize(run, startTime, endTime);
  
   Log(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run,
		TTimeStamp(startTime).AsString(),
		TTimeStamp(endTime).AsString()));

   fRun       = run;
   // fStartTime = startTime;
   // fEndTime   = endTime;
   fStartTime = GetStartTimeDCSQuery ();
   fEndTime   = GetEndTimeDCSQuery ();
   time_t daqStart = (time_t) (((TString)GetRunParameter("DAQ_time_start")).Atoi());
   time_t daqEnd   = (time_t) (((TString)GetRunParameter("DAQ_time_end")).Atoi());
   time_t ctpStart = (time_t) (((TString)GetRunParameter("TRGTimeStart")).Atoi());
   time_t ctpEnd   = (time_t) (((TString)GetRunParameter("TRGTimeEnd")).Atoi());
   
	fDCSData      = new AliADDataDCS(fRun, fStartTime, fEndTime,(UInt_t)daqStart, (UInt_t)daqEnd,(UInt_t)ctpStart, (UInt_t)ctpEnd);	
   
}

//______________________________________________________________________________________________
UInt_t AliADPreprocessor::Process(TMap* dcsAliasMap)
{
  // Fills data retrieved from DCS and DAQ into a AliADCalibData object and 
  // stores it into CalibrationDB


  // *** GET RUN TYPE ***
  TString runType = GetRunType();


  // *** REFERENCE DATA *** 
  
  TString fileName; 
  AliADCalibData *calibData = new AliADCalibData();
  
  // *************** HV From DCS ******************
  // Fills data into a AliADDataDCS object
  if(!dcsAliasMap) return 1;

 	// The Processing of the DCS input data is forwarded to AliADDataDCS
  if (!fDCSData->ProcessData(*dcsAliasMap)) return 1;

	// Writes AD PMs HV values into AD calibration object and Timing resolution parameters
  	calibData->FillDCSData(fDCSData);
	   
   // *************** From DAQ ******************
   
	TString sourcesId = "AD0da_results";

	TList* sourceList = GetFileSources(kDAQ, sourcesId.Data());
  	if (!sourceList)  {
		Log(Form("No sources found for id %s", sourcesId.Data()));      		
      		return 1; }
	Log(Form("The following sources produced files with the id %s",sourcesId.Data()));
	sourceList->Print();    

  	TIter iter(sourceList);
  	TObjString *source;
		
	while((source=dynamic_cast<TObjString*> (iter.Next()))){
  		fileName = GetFile(kDAQ, sourcesId.Data(), source->GetName());
  		if (fileName.Length() > 0)
    		Log(Form("Got the file %s, now we can extract some values.", fileName.Data()));
		FILE *file;
		if((file = fopen(fileName.Data(),"r")) == NULL){
            	                   Log(Form("Cannot open file %s",fileName.Data()));
	    	  	           return 1;}
		Float_t pedMean[32], pedSigma[32], adcMean[32], adcSigma[32] ;
		for(Int_t j=0; j<32; j++) {
		  Int_t resScan = fscanf(file,"%f %f %f %f",
					 &pedMean[j], &pedSigma[j], &adcMean[j], &adcSigma[j]);
		  if (resScan != 4) Log(Form("Bad data in file %s !",fileName.Data()));
		}
		fclose(file);
	    	
		calibData->SetPedestal(pedMean);
		calibData->SetSigma(pedSigma);
		calibData->SetADCmean(adcMean);			
		calibData->SetADCsigma(adcSigma);
		}				

	delete source;      
  
  // Check that everything was properly transmitted

//   for(Int_t j=0; j<128; j++){printf("Pedestal[%d] -> %f \n",j,calibData->GetPedestal(j));}
//   for(Int_t j=0; j<128; j++){printf("pedSigma[%d] -> %f \n",j,calibData->GetSigma(j));}
//   for(Int_t j=0; j<128; j++){printf("Gain[%d] -> %f \n",j,calibData->GetGain(j));}
//   for(Int_t j=0; j<128; j++){printf("adcSigma[%d] -> %f \n",j,calibData->GetADCsigma(j));}
//   for(Int_t j=0; j<64; j++){printf("MeanHV[%d] -> %f \n",j,calibData->GetMeanHV(j));}
//   for(Int_t j=0; j<64; j++){printf("WidthHV[%d] -> %f \n",j,calibData->GetWidthHV(j));}
  
  // Now we store the AD Calibration Object into CalibrationDB

  Bool_t resECal=kTRUE;
  
  Bool_t result = 0;
//  if(sourceList && sourceList->GetEntries()>0)
//  {
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Michal Broz");
  metaData.SetComment("This preprocessor fills an AliADCalibData object");

  resECal = Store("Calib", "Data", calibData, &metaData, 0, kTRUE);
//  }
  if(resECal==kFALSE ) result = 1;
  

  delete calibData;
  delete sourceList; 

 	
  return result;
}


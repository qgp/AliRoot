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

/* $Id$ */


//#include <Riostream.h>
//#include <stdio.h>
//#include <stdlib.h>

#include <TFile.h>
#include <TH2S.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TTimeStamp.h>

#include "AliCDBMetaData.h"
#include "AliCDBEntry.h"
#include "AliLog.h"
#include "AliTOFChannelOnlineArray.h"
#include "AliTOFChannelOnlineStatusArray.h"
#include "AliTOFDataDCS.h"
#include "AliTOFLvHvDataPoints.h"
#include "AliTOFGeometry.h"
#include "AliTOFPreprocessor.h"
#include "AliTOFFEEReader.h"
#include "AliTOFRawStream.h"
#include "AliTOFCableLengthMap.h"
#include "AliTOFcalibHisto.h"
#include "AliTOFFEEDump.h"
#include "TChain.h"
#include "AliTOFDeltaBCOffset.h"
#include "AliTOFCTPLatency.h"
#include "AliTOFT0Fill.h"
#include "AliTOFT0FillOnlineCalib.h"
#include "AliTOFHitField.h"
#include "AliTOFChannelOffline.h"
#include "TF1.h"
#include "TGeoManager.h"
#include "AliGeomManager.h"

// TOF preprocessor class.
// It takes data from DCS and passes them to the class AliTOFDataDCS, which
// processes them. The result is then written to the CDB.
// Analogously, it takes data form DAQ (both at Run level and inclusive - 
// of all the runs - level, processes them, and stores both Reference Data
// and Online Calibration files in the CDB. 
// Processing of Pulser/Noise Run data and of TOF FEE DCS map included also.

// return codes:
// return=0 : all ok
// return=1 : no DCS input data Map
// return=2 : no DCS input data processing
// return=3 : no DCS processed data was stored in Ref Data
// return=4 : no DAQ input for Ref Data
// return=5 : failed to store DAQ Ref Data
// return=6 : failed to retrieve DAQ data for calibration 
// return=7 : problems in processing histos in the input DAQ file 
// return=8 : failed to store Online Delays
// return=9 : failed to store Reference Data for Pulser
// return=10: failed to retrieve Pulser data 
// return=11: failed to store Pulser map in OCDB
// return=12: failed to store Reference Data for Noise
// return=13: failed to retrieve Noise data 
// return=14: failed to store Noise map in OCDB
// return=15: failed to retrieve FEE data from FXS
// return=16: failed to retrieve FEE data from OCDB
// return=17: failed to store FEE data in OCDB
// return=18: failed to store FEE reference data in OCDB
// return=20: failed in retrieving status variable

ClassImp(AliTOFPreprocessor)

const Int_t    AliTOFPreprocessor::fgkBinRangeAve = 13;    // number of bins where to calculate the mean 
const Double_t AliTOFPreprocessor::fgkIntegralThr = 100;   // min number of entries to perform computation of delay per channel 
const Double_t AliTOFPreprocessor::fgkThrPar      = 0.013; // parameter used to trigger the calculation of the delay

//_____________________________________________________________________________

AliTOFPreprocessor::AliTOFPreprocessor(AliShuttleInterface* shuttle) :
  AliPreprocessor("TOF", shuttle),
  fData(0),
  fHVLVmaps(0),
  fCal(0),
  fNChannels(0),
  fStoreRefData(kTRUE),
  fFDRFlag(kFALSE),
  fStatus(0),
  fMatchingWindow(0),
  fLatencyWindow(0),
  fIsStatusMapChanged(0)
{
  // constructor
  AddRunType("PHYSICS");
  AddRunType("PULSER");
  AddRunType("NOISE");

}

//_____________________________________________________________________________

AliTOFPreprocessor::~AliTOFPreprocessor()
{
  // destructor
}

//______________________________________________________________________________
void AliTOFPreprocessor::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
  // Creates AliTOFDataDCS object

  AliPreprocessor::Initialize(run, startTime, endTime);

	AliInfo(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s \n\tStartTime DCS Query %s \n\tEndTime DCS Query %s", run,
		TTimeStamp(startTime).AsString(),
		TTimeStamp(endTime).AsString(), ((TTimeStamp)GetStartTimeDCSQuery()).AsString(), ((TTimeStamp)GetEndTimeDCSQuery()).AsString()));

	fData = new AliTOFDataDCS(fRun, fStartTime, fEndTime, GetStartTimeDCSQuery(), GetEndTimeDCSQuery());
	fHVLVmaps = new AliTOFLvHvDataPoints(fRun, fStartTime, fEndTime, GetStartTimeDCSQuery(), GetEndTimeDCSQuery());
	fNChannels = AliTOFGeometry::NSectors()*(2*(AliTOFGeometry::NStripC()+AliTOFGeometry::NStripB())+AliTOFGeometry::NStripA())*AliTOFGeometry::NpadZ()*AliTOFGeometry::NpadX();
}
//_____________________________________________________________________________
Bool_t AliTOFPreprocessor::ProcessDCS(){

  // check whether DCS should be processed or not...

  TString runType = GetRunType();
  Log(Form("RunType %s",runType.Data()));

  if (runType != "PHYSICS"){
    return kFALSE;
  }

  return kTRUE;
}
//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessDCSDataPoints(TMap *dcsAliasMap)
{
  // Fills data into a AliTOFDataDCS object


  Log("Processing DCS DP");
  TH1::AddDirectory(0);

  Bool_t resultDCSMap=kFALSE;
  Bool_t resultDCSStore=kFALSE;

  // processing DCS

  fData->SetFDRFlag(fFDRFlag);
  
  if (!dcsAliasMap){
    Log("No DCS map found: TOF exiting from Shuttle");
    if (fData){
	    delete fData;
	    fData = 0;
    }
    return 1;// return error Code for DCS input data not found 
  }
  else {

  // The processing of the DCS input data is forwarded to AliTOFDataDCS
    resultDCSMap=fData->ProcessData(*dcsAliasMap);
    if(!resultDCSMap){
      Log("Some problems occurred while processing DCS data, TOF exiting from Shuttle");
      if (fData){
	      delete fData;
	      fData = 0;
      }
      return 2;// return error Code for processed DCS data not stored 
    }
    else{
      AliCDBMetaData metaDataDCS;
      metaDataDCS.SetBeamPeriod(0);
      metaDataDCS.SetResponsible("Chiara Zampolli");
      metaDataDCS.SetComment("This preprocessor fills an AliTOFDataDCS object.");
      AliInfo("Storing DCS Data");
      resultDCSStore = StoreReferenceData("Calib","DCSData",fData, &metaDataDCS);
      if (!resultDCSStore){
	Log("Some problems occurred while storing DCS data results in Reference Data, TOF exiting from Shuttle");
	if (fData){
		delete fData;
		fData = 0;
	}
	return 3;// return error Code for processed DCS data not stored 
	         // in reference data
	
      }
    }
  }
  if (fData){
	  delete fData;
	  fData = 0;
  }
  
  return 0;
}
//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessHVandLVdps(TMap *dcsAliasMap)
{
  //
  //Fills data into a AliTOFLvHvDataPoints object
  // Merges fStatus object with LV and HV status at SOR
  // Updates fStatus object with LV and HV status
  //    at EOR in case of correct end of run
  //    at last but two value in case of end-of-run caused by TOF detector.
  //

  Log("Processing HV and LV DCS DPs");
  TH1::AddDirectory(0);

  Bool_t resultDCSMap=kFALSE;

  // processing DCS

  fHVLVmaps->SetFDRFlag(fFDRFlag);
  
  if (!dcsAliasMap){
    Log("No DCS map found: TOF exiting from Shuttle");
    if (fHVLVmaps){
      delete fHVLVmaps;
      fHVLVmaps = 0;
    }
    return 1;// return error Code for DCS input data not found 
  }
  else {

    // The processing of the DCS input data is forwarded to AliTOFDataDCS
    resultDCSMap = fHVLVmaps->ProcessData(*dcsAliasMap);
    if (!resultDCSMap) {
      Log("Some problems occurred while processing DCS data, TOF exiting from Shuttle");
      if (fHVLVmaps) {
	delete fHVLVmaps;
	fHVLVmaps = 0;
      }
      return 2;// return error Code for processed DCS data not stored 
    }
    else {

      // check with plots. Start...
      /*
      TH1F *hROsor = new TH1F("hROsor","RO status map at SOR",91*96*18,-0.5,91*96*18-0.5);
      for (Int_t ii=1; ii<=91*96*18; ii++) hROsor->SetBinContent(ii,-1);
      for (Int_t ii=0; ii<91*96*18; ii++) {
	if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWBad)
	  hROsor->SetBinContent(ii+1,0);
	else if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWOk)
	  hROsor->SetBinContent(ii+1,1);
      }

      TH1F *hROandHVandLVsor = new TH1F("hROandHVandLVsor","RO.and.HV.andLV status map at SOR",91*96*18,-0.5,91*96*18-0.5);
      for (Int_t ii=1; ii<=91*96*18; ii++) hROandHVandLVsor->SetBinContent(ii,-1);
      TH1F *hROandHVandLVeor = new TH1F("hROandHVandLVeor","RO.and.HV.andLV status map at EOR",91*96*18,-0.5,91*96*18-0.5);
      for (Int_t ii=1; ii<=91*96*18; ii++) hROandHVandLVeor->SetBinContent(ii,-1);
      */

      AliTOFDCSmaps * lvANDhvMap = (AliTOFDCSmaps*)fHVLVmaps->GetHVandLVmapAtSOR(); // Get LV.and.HV status map at SOR
      for (Int_t index=0; index<fNChannels; index++) {
	if ( ( lvANDhvMap->GetCellValue(index)==0 &&
	       fStatus->GetHWStatus(index) != AliTOFChannelOnlineStatusArray::kTOFHWBad ) ||
	     ( lvANDhvMap->GetCellValue(index)==1 &&
	       fStatus->GetHWStatus(index) != AliTOFChannelOnlineStatusArray::kTOFHWOk ) ) {
	  fStatus->SetHWStatus(index, AliTOFChannelOnlineStatusArray::kTOFHWBad);
	  fIsStatusMapChanged=kTRUE;
	}
      }
      
      // check with plots. Start...
      /*
      for (Int_t ii=0; ii<91*96*18; ii++) {
	if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWBad)
	  hROandHVandLVsor->SetBinContent(ii+1,0);
	else if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWOk)
	  hROandHVandLVsor->SetBinContent(ii+1,1);
      }
      */

      lvANDhvMap = (AliTOFDCSmaps*)fHVLVmaps->GetHVandLVmapAtEOR(); // Get LV.and.HV status map at EOR
      for (Int_t index=0; index<fNChannels; index++) {
	if ( ( lvANDhvMap->GetCellValue(index)==0 &&
	       fStatus->GetHWStatus(index)!=AliTOFChannelOnlineStatusArray::kTOFHWBad ) ||
	     ( lvANDhvMap->GetCellValue(index)==1 &&
	       fStatus->GetHWStatus(index) != AliTOFChannelOnlineStatusArray::kTOFHWOk ) ) {
	  fStatus->SetHWStatus(index, AliTOFChannelOnlineStatusArray::kTOFHWBad);
	  fIsStatusMapChanged=kTRUE;
	}
      }

      // check with plots. Start...
      /*
      for (Int_t ii=0; ii<91*96*18; ii++) {
	if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWBad)
	  hROandHVandLVeor->SetBinContent(ii+1,0);
	else if (fStatus->GetHWStatus(ii)==AliTOFChannelOnlineStatusArray::kTOFHWOk)
	  hROandHVandLVeor->SetBinContent(ii+1,1);
      }

      TCanvas *canvas = new TCanvas("canvas","",10,10,1000,1000);
      canvas->SetFillColor(0);
      canvas->Divide(2,2);
      canvas->cd(1);
      hROsor->SetLineWidth(2);
      hROsor->Draw();
      canvas->cd(2);
      hROandHVandLVsor->SetLineWidth(2);
      hROandHVandLVsor->Draw();
      canvas->cd(3);
      hROandHVandLVeor->SetLineWidth(2);
      hROandHVandLVeor->Draw();
      canvas->cd();
      */

    }
  }


  /* check whether we don't need to update OCDB.
   * in this case we can return without errors. */

  if (!fIsStatusMapChanged) {
    AliInfo("TOF FEE config has not changed. Do not overwrite stored file.");
    return 0; // return ok
  }

  TString runType = GetRunType();
  if (runType != "PHYSICS") {
    AliInfo(Form("Run Type = %s, waiting to store status map",GetRunType()));
    return 0; // return ok
  }

  // update the OCDB with the current FEE.and.HV.and.LV
  // since even a little difference has been detected.

  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Roberto Preghenella");
  metaData.SetComment("This preprocessor fills an AliTOFChannelOnlineStatusArray object from FEE.and.HV.and.LV data.");
  AliInfo("Storing Status data from current run. Collected RO.and.HV.and.LV infos @ EOR");
  // store FEE data
  if (!Store("Calib", "Status", fStatus, &metaData, 0, kTRUE)) {
    // failed
    Log("problems while storing RO.and.HV.and.LV Status data object");
    if (fStatus){
      delete fStatus;
      fStatus = 0;
    }
    if (fHVLVmaps) {
      delete fHVLVmaps;
      fHVLVmaps = 0;
    }
    return 17; // return error code for problems  while storing FEE data
  }

  // everything fine. return

  if (fStatus){
    delete fStatus;
    fStatus = 0;
  }

  if (fHVLVmaps) {
    delete fHVLVmaps;
    fHVLVmaps = 0;
  }
  
  return 0;
}

//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessOnlineDelays()
{
  // Processing data from DAQ for online calibration 

  Bool_t updateOCDB = kFALSE;
  Log("Processing DAQ delays");

  // reading configuration map 
  TString compDelays = "kFALSE";
  Int_t deltaStartingRun = fRun;
  Int_t startingRun = fRun-deltaStartingRun;
  Int_t binRangeAve = fgkBinRangeAve;
  Double_t integralThr = fgkIntegralThr;
  Double_t thrPar = fgkThrPar;

  AliCDBEntry *cdbEntry = GetFromOCDB("Calib","Config");
  if (!cdbEntry) {
	  Log(Form("No Configuration entry found in CDB, using default values: ComputingDelays = %s, StartingRun = %i",compDelays.Data(), startingRun));
  }
  else {
	  TMap *configMap = (TMap*)cdbEntry->GetObject();
	  if (!configMap){
		  Log(Form("No map found in Config entry in CDB, using default values: ComputingDelays = %s, StartingRun = %i",compDelays.Data(), startingRun));
	  }
	  else{
		  TObjString *strDelays = (TObjString*)configMap->GetValue("ComputingDelays");
		  if (strDelays) {
			  compDelays = (TString) strDelays->GetString();
		  }
		  else {
			  Log(Form("No ComputingDelays value found in Map from Config entry in CDB, using default value: ComputingDelays =  %s",compDelays.Data()));
		  }
		  TObjString *strRun = (TObjString*)configMap->GetValue("StartingRun");
		  if (strRun) {
			  TString tmpstr = strRun->GetString();
			  startingRun = tmpstr.Atoi();
			  deltaStartingRun = fRun - startingRun;
		  }
		  else {
			  Log(Form("No StartingRun value found in Map from Config entry in CDB, using default value: StartingRun = %i",startingRun));
		  }
		  TObjString *strBinRangeAve = (TObjString*)configMap->GetValue("BinRangeAve");
		  if (strBinRangeAve) {
			  TString tmpstr = strBinRangeAve->GetString();
			  binRangeAve = tmpstr.Atoi();
		  }
		  else {
			  Log(Form("No BinRangeAve value found in Map from Config entry in CDB, using default value: BinRangeAve = %i",binRangeAve));
		  }
		  TObjString *strIntegralThr = (TObjString*)configMap->GetValue("IntegralThr");
		  if (strIntegralThr) {
			  TString tmpstr = strIntegralThr->GetString();
			  integralThr = tmpstr.Atof();
		  }
		  else {
			  Log(Form("No IntegralThr value found in Map from Config entry in CDB, using default value: IntegralThr = %i",integralThr));
		  }
		  TObjString *strThrPar = (TObjString*)configMap->GetValue("ThrPar");
		  if (strThrPar) {
			  TString tmpstr = strThrPar->GetString();
			  thrPar = tmpstr.Atof();
		  }
		  else {
			  Log(Form("No ThrPar value found in Map from Config entry in CDB, using default value: ThrPar = %i",thrPar));
		  }
	  }
  }
  if (compDelays == "kTRUE") fFDRFlag = kFALSE;
  else fFDRFlag = kTRUE;

  delete cdbEntry;
  cdbEntry = 0x0;

  Log(Form("ComputingDelays = %s, StartingRun = %i",compDelays.Data(),startingRun));

  /* init array with current calibration, if any */
  fCal = new AliTOFChannelOnlineArray(fNChannels);  
  AliTOFChannelOnlineArray *curCal = NULL;

  AliCDBEntry *cdbEntry2 = GetFromOCDB("Calib","ParOnlineDelay");
  if (!cdbEntry2 || !cdbEntry2->GetObject()) {
    /* no CDB entry found. set update flag */
    Log("     ************ WARNING ************");
    Log("No CDB ParOnlineDelay entry found, creating a new one!");
    Log("     *********************************");
    updateOCDB = kTRUE;
  }
  else {
    Log("Found previous ParOnlineDelay entry. Using it to init calibration");
    curCal = (AliTOFChannelOnlineArray *)cdbEntry2->GetObject();
    for (Int_t i = 0; i < fNChannels; i++)
      fCal->SetDelay(i, curCal->GetDelay(i));
  }
 

  TH1::AddDirectory(0);

  Bool_t resultDAQRef=kFALSE;
  Bool_t resultTOFPP=kFALSE;
  TH2S *h2 = 0x0;
  // processing DAQ
  
  TFile * daqFile=0x0;
  
  if(fStoreRefData){
    //retrieving data at Run level
	  TList* list = GetFileSources(kDAQ, "RUNLevel");
	  if (list !=0x0 && list->GetEntries()!=0)
		  {
			  AliInfo("The following sources produced files with the id RUNLevel");
			  list->Print();
			  for (Int_t jj=0;jj<list->GetEntries();jj++){
				  TObjString * str = dynamic_cast<TObjString*> (list->At(jj));
				  AliInfo(Form("found source %s", str->String().Data()));
				  // file to be stored run per run
				  TString fileNameRun = GetFile(kDAQ, "RUNLevel", str->GetName());
				  if (fileNameRun.Length()>0){
					  AliInfo(Form("Got the file %s, now we can store the Reference Data for the current Run.", fileNameRun.Data()));
					  daqFile = new TFile(fileNameRun.Data(),"READ");
					  h2 = (TH2S*) daqFile->Get("htof");
					  AliCDBMetaData metaDataHisto;
					  metaDataHisto.SetBeamPeriod(0);
					  metaDataHisto.SetResponsible("Chiara Zampolli");
					  metaDataHisto.SetComment("This preprocessor stores the array of histos object as Reference Data.");
					  AliInfo("Storing Reference Data");
					  resultDAQRef = StoreReferenceData("Calib","DAQData",h2, &metaDataHisto);
					  if (!resultDAQRef){
						  Log("some problems occurred::No Reference Data stored, TOF exiting from Shuttle");
						  delete h2;
						  delete list;
						  delete fCal;
						  fCal=0x0;
						  return 5;//return error code for failure in storing Ref Data 
					  }
					  daqFile->Close();
					  delete daqFile;
				  }
				  
				  else{
					  Log("The input data file from DAQ (run-level) was not found, TOF exiting from Shuttle "); 
					  delete list;
					  delete fCal;
					  fCal=0x0;
					  return 4;//return error code for failure in retrieving Ref Data 
				  }
			  }
			  delete list;
		  }
	  else{
		  Log("The input data file list from DAQ (run-level) was not found, TOF exiting from Shuttle "); 
		  delete fCal;
		  fCal=0x0;
		  return 4;//return error code for failure in retrieving Ref Data 
	  }	
  }


  //Total files, with cumulative histos
  
  TList* listTot = GetFileSources(kDAQ, "DELAYS");
  if (listTot !=0x0 && listTot->GetEntries()!=0)
	  {
		  AliInfo("The following sources produced files with the id DELAYS");
		  listTot->Print();
		  for (Int_t jj=0;jj<listTot->GetEntries();jj++){
			  TObjString * str = dynamic_cast<TObjString*> (listTot->At(jj));
			  AliInfo(Form("found source %s", str->String().Data()));
			  
			  // file with summed histos, to extract calib params
			  TString fileName = GetFile(kDAQ, "DELAYS", str->GetName());
			  if (fileName.Length()>0){
				  AliInfo(Form("Got the file %s, now we can extract some values.", fileName.Data()));
				  
				  daqFile = new TFile(fileName.Data(),"READ");
				  if (h2) delete h2;
				  h2 = (TH2S*) daqFile->Get("htoftot");
				  if (!h2){
					  Log("some problems occurred:: No histo retrieved, TOF exiting from Shuttle");
					  delete listTot;
					  delete daqFile;
					  delete fCal;
					  fCal=0x0;
					  return 7; //return error code for histograms not existing/junky
				  }
				  else {
					  static const Int_t kSize=h2->GetNbinsX();
					  static const Int_t kNBins=h2->GetNbinsY();
					  static const Double_t kXBinmin=h2->GetYaxis()->GetBinLowEdge(1);
					  if (kSize != fNChannels){
						  Log(" number of bins along x different from number of pads, found only a subset of the histograms, TOF exiting from Shuttle");
						  delete listTot;
						  delete h2;
						  delete daqFile;
						  delete fCal;
						  fCal=0x0;
						  return 7; //return error code for histograms not existing/junky
					  }
					  Int_t nNotStatistics = 0; // number of channel with not enough statistics

					  /* FDR flag set. do not compute delays, use nominal cable delays */
					  if (fFDRFlag) {

					    Log(" Not computing delays according to flag set in Config entry in OCDB!");
					    FillWithCosmicCalibration(fCal);

					    /* check whether the new calibration is different from the previous one */
					    if (curCal) { /* well, check also whether we have a previous calibration */
					      for (Int_t i = 0; i < fNChannels; i++) {
						if (fCal->GetDelay(i) != curCal->GetDelay(i)) {
						  updateOCDB = kTRUE;
						  break;
						}
					      }
					    }
					    else /* otherwise update OCDB */
					      updateOCDB = kTRUE;

					  }

					  else {  // computing delays if not in FDR runs

					    updateOCDB = kTRUE; /* always update OCDB when computing delays */

						  for (Int_t ich=0;ich<kSize;ich++){
							  /* check whether channel has been read out during current run.
							   * if the status is bad it means it has not been read out.
							   * in this case skip channel in order to not affect the mean */ 
							  if (fStatus->GetHWStatus(ich) == AliTOFChannelOnlineStatusArray::kTOFHWBad){
								  AliDebug(2,Form(" Channel %i found bad according to FEEmap, (HW status = %i), skipping from delay computing",ich, (Int_t)fStatus->GetHWStatus(ich)));
								  continue;
							  }
							  AliDebug(2,Form(" Channel %i found ok according to FEEmap, starting delay computing",ich));
							  TH1S *h1 = new TH1S("h1","h1",kNBins,kXBinmin-0.5,kNBins*1.+kXBinmin-0.5);
							  for (Int_t ibin=0;ibin<kNBins;ibin++){
								  h1->SetBinContent(ibin+1,h2->GetBinContent(ich+1,ibin+1));
							  }
							  if(h1->Integral()<integralThr) {
								  nNotStatistics++;
								  Log(Form(" Not enough statistics for bin %i, skipping this channel",ich));  // printing message only if not in FDR runs
								  delete h1;
								  h1=0x0;
								  continue;
							  }
							  Bool_t found=kFALSE; 
							  Float_t minContent=h1->Integral()*thrPar; 
							  Int_t nbinsX = h1->GetNbinsX();
							  Int_t startBin=1;
							  for (Int_t j=1; j<=nbinsX; j++){
								  if ((
								       h1->GetBinContent(j) +     
								       h1->GetBinContent(j+1)+
								       h1->GetBinContent(j+2)+ 
								       h1->GetBinContent(j+3))>minContent){
									  found=kTRUE;
									  startBin=j;
									  break;
								  }
							  }
							  if(!found) AliInfo(Form("WARNING!!! no start of fit found for histo # %i",ich));
							  // Now calculate the mean over the interval. 
							  Double_t mean = 0;
							  Double_t sumw2 = 0;
							  Double_t nent = 0;
							  for(Int_t k=0;k<binRangeAve;k++){
								  mean=mean+h1->GetBinCenter(startBin+k)*h1->GetBinContent(startBin+k);                 
								  nent=nent+h1->GetBinContent(startBin+k);                 
								  sumw2=sumw2+(h1->GetBinCenter(startBin+k))*(h1->GetBinCenter(startBin+k))*(h1->GetBinContent(startBin+k));
							  }
							  mean= mean/nent; //<x>
							  sumw2=sumw2/nent; //<x^2>
							  Double_t rmsmean= 0;
							  rmsmean = TMath::Sqrt((sumw2-mean*mean)/nent);
							  if (ich<fNChannels) {
								  Float_t delay = mean*AliTOFGeometry::TdcBinWidth()*1.E-3; // delay in ns
								  fCal->SetDelay(ich,delay);  // delay in ns
								  AliDebug(2,Form("Setting delay %f (ns) for channel %i",delay,ich));
							  }
							  delete h1;
							  h1=0x0;
						  }
					  }
					  if (nNotStatistics!=0) Log(Form("Too little statistics for %d channels!",nNotStatistics)); 
				  }
				  delete h2;
				  daqFile->Close();
				  delete daqFile;
			  }
			  else{
				  Log("The Cumulative data file from DAQ does not exist, TOF exiting from Shuttle"); 
				  delete listTot;
				  delete fCal;
				  fCal=0x0;
				  return 6;//return error code for problems in retrieving DAQ data 
			  }
		  }
		  delete listTot;
	  }
  else{
    Log("Problem: no list for Cumulative data file from DAQ was found, TOF exiting from Shuttle");
    delete fCal;
    fCal=0x0;
    return 6; //return error code for problems in retrieving DAQ data 
  }

  /* check whether we don't need to update OCDB.
   * in this case we can return without errors and
   * the current FEE is stored in the fStatus object. */
  if (!updateOCDB) {
    AliInfo("update OCDB flag not set. Do not overwrite stored file.");
    return 0; /* return ok */
  }
  
  daqFile=0;
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Chiara Zampolli");
  metaData.SetComment("This preprocessor fills an AliTOFChannelOnlineArray object for online calibration - delays.");
  AliInfo("Storing Calibration Data");
  resultTOFPP = Store("Calib","ParOnlineDelay",fCal, &metaData,deltaStartingRun,kTRUE);
  if(!resultTOFPP){
    Log("Some problems occurred while storing online object resulting from DAQ data processing");
    delete fCal;
    fCal=0x0;
    return 8;//return error code for problems in storing DAQ data 
  }

  if (fCal){
    delete fCal;
    fCal = 0;
  }

  return 0;
}

//_____________________________________________________________________________

UInt_t 
AliTOFPreprocessor::ProcessT0Fill()
{
  // Processing data from DAQ for T0-fill measurement 

  Log("Processing T0-fill");

#if 0
  /* instance and setup CDB manager */
  AliCDBManager *cdb = AliCDBManager::Instance();
  /* load geometry */
  if (!gGeoManager) AliGeomManager::LoadGeometry();
#endif

  /* get params from OCDB */
  AliCDBEntry *cdbe = NULL;

  /* get T0-fill calibration params */
  cdbe = GetFromOCDB("Calib", "T0FillOnlineCalib");
  if (!cdbe) {
    Log("cannot get \"T0FillOnlineCalib\" entry from OCDB");
    return 21;
  }
  AliTOFT0FillOnlineCalib *t0FillOnlineCalibObject = (AliTOFT0FillOnlineCalib *)cdbe->GetObject();
  if (!t0FillOnlineCalibObject) {
    Log("cannot get \"T0FillOnlineCalib\" object from CDB entry");
    return 21;
  }
  Float_t t0FillCalibOffset = t0FillOnlineCalibObject->GetOffset();
  Float_t t0FillCalibCoefficient = t0FillOnlineCalibObject->GetCoefficient();
  Log(Form("got \"T0FillOnlineCalib\" object: offset=%f coeff=%f", t0FillCalibOffset, t0FillCalibCoefficient));

  /* get online status from OCDB */
  cdbe = GetFromOCDB("Calib", "Status");
  if (!cdbe) {
    Log("cannot get \"Status\" entry from OCDB");
    return 21;
  }
  AliTOFChannelOnlineStatusArray *statusArray = (AliTOFChannelOnlineStatusArray *)cdbe->GetObject();
  if (!statusArray) {
    Log("cannot get \"Status\" object from CDB entry");
    return 21;
  }
  Log("got \"Status\" object");

  /* get offline calibration from OCDB */
  cdbe = GetFromOCDB("Calib", "ParOffline");
  if (!cdbe) {
    Log("cannot get \"ParOffline\" entry from OCDB");
    return 21;
  }
  TObjArray *offlineArray = (TObjArray *)cdbe->GetObject();
  AliTOFChannelOffline *channelOffline;
  if (!offlineArray) {
    Log("cannot get \"ParOffline\" object from CDB entry");
    return 21;
  }
  Log("got \"ParOffline\" object");

  /* get deltaBC offset from OCDB */
  cdbe = GetFromOCDB("Calib", "DeltaBCOffset");
  if (!cdbe) {
    Log("cannot get \"DeltaBCOffset\" entry from OCDB");
    return 21;
  }
  AliTOFDeltaBCOffset *deltaBCOffsetObject = (AliTOFDeltaBCOffset *)cdbe->GetObject();
  if (!deltaBCOffsetObject) {
    Log("cannot get \"DeltaBCOffset\" object from CDB entry");
    return 21;
  }
  Int_t deltaBCOffset = deltaBCOffsetObject->GetDeltaBCOffset();
  Log(Form("got \"DeltaBCOffset\" object: deltaBCOffset=%d (BC bins)", deltaBCOffset));

  /* get CTP latency from OCDB */
  cdbe = GetFromOCDB("Calib", "CTPLatency");
  if (!cdbe) {
    Log("cannot get \"CTPLatency\" entry from OCDB");
    return 21;
  }
  AliTOFCTPLatency *ctpLatencyObject = (AliTOFCTPLatency *)cdbe->GetObject();
  if (!ctpLatencyObject) {
    Log("cannot get \"CTPLatency\" object from CDB entry");
    return 21;
  }
  Float_t ctpLatency = ctpLatencyObject->GetCTPLatency();
  Log(Form("got \"CTPLatency\" object: ctpLatency=%f (ps)", ctpLatency));
  
  /* get file sources from FXS */
  TList *fileList = GetFileSources(kDAQ, "HITS");
  if (!fileList || fileList->GetEntries() == 0) {
    Log("cannot get DAQ source file list or empty list");
    return 21;
  }
  Log(Form("got DAQ source file list: %d files", fileList->GetEntries()));
  fileList->Print();
  
  /* create tree chain using file sources */
  TChain chain("hitTree");
  for (Int_t ifile = 0; ifile < fileList->GetEntries(); ifile++) {
    TObjString *str = (TObjString *)fileList->At(ifile);
    TString filename = GetFile(kDAQ, "HITS", str->GetName());
    chain.Add(filename);
    Log(Form("file added to input chain: source=%s, filename=%s", str->String().Data(), filename.Data()));
  }
  Int_t nhits = chain.GetEntries();
  Log(Form("input chain ready: %d hits", nhits));

  /* setup input chain */
  AliTOFHitField *hit = new AliTOFHitField();
  chain.SetBranchAddress("hit", &hit);

  /* create calib histo and geometry */
  AliTOFcalibHisto calibHisto;
  calibHisto.LoadCalibHisto();
  AliTOFGeometry tofGeo;

  /* constants */
  Float_t c = TMath::C() * 1.e2 / 1.e12; /* cm/ps */
  Float_t c_1 = 1. / c;
  /* variables */
  Int_t index, timebin, totbin, deltaBC, l0l1latency, det[5];
  Float_t timeps, totns, corrps, length, timeexp, timezero, pos[3], latencyWindow;

  /* histos */
  TH1F *hT0Fill = new TH1F("hT0Fill", "T0 fill;t - t_{exp}^{(c)} (ps);", 2000, -24400., 24400.);

  /* loop over hits */
  for (Int_t ihit = 0; ihit < nhits; ihit++) {

    /* get entry */
   chain.GetEntry(ihit);
    
    /* get hit info */
    index = hit->GetIndex();
    timebin = hit->GetTimeBin();
    totbin = hit->GetTOTBin();
    deltaBC = hit->GetDeltaBC();
    l0l1latency = hit->GetL0L1Latency();
    latencyWindow = statusArray->GetLatencyWindow(index) * 1.e3;
    
    /* convert time in ps and tot in ns */
    timeps = timebin * AliTOFGeometry::TdcBinWidth();
    totns = totbin * AliTOFGeometry::ToTBinWidth() * 1.e-3;
    /* get calibration correction in ps */
    channelOffline = (AliTOFChannelOffline *)offlineArray->At(index);
    if (totns < AliTOFGeometry::SlewTOTMin()) totns = AliTOFGeometry::SlewTOTMin();
    if (totns > AliTOFGeometry::SlewTOTMax()) totns = AliTOFGeometry::SlewTOTMax();
    corrps = 0.;
    for (Int_t ipar = 0; ipar < 6; ipar++) corrps += channelOffline->GetSlewPar(ipar) * TMath::Power(totns, ipar);
    corrps *= 1.e3;
    /* perform time correction */
    timeps = timeps + (deltaBC - deltaBCOffset) * AliTOFGeometry::BunchCrossingBinWidth() + l0l1latency * AliTOFGeometry::BunchCrossingBinWidth() + ctpLatency - latencyWindow - corrps;
    /* compute length and expected time */
    tofGeo.GetVolumeIndices(index, det);
    tofGeo.GetPosPar(det, pos);
    length = 0.;
    for (Int_t i = 0; i < 3; i++) length += pos[i] * pos[i];
    length = TMath::Sqrt(length);
    timeexp = length * c_1;
    /* compute time zero */
    timezero = timeps - timeexp;
    
    /* fill histos */
    hT0Fill->Fill(timezero);
  }

  /* rebin until maximum bin has required minimum entries */
  Int_t maxBin = hT0Fill->GetMaximumBin();
  Int_t maxBinContent = hT0Fill->GetBinContent(maxBin);
  Float_t binWidth = hT0Fill->GetBinWidth(maxBin);
  while (maxBinContent < 400 && binWidth < 90.) {
    hT0Fill->Rebin(2);
    maxBin = hT0Fill->GetMaximumBin();
    maxBinContent = hT0Fill->GetBinContent(maxBin);
    binWidth = hT0Fill->GetBinWidth(maxBin);
  }
  Float_t maxBinCenter = hT0Fill->GetBinCenter(maxBin);

  /* rough landau fit of the edge */
  TF1 *landau = (TF1 *)gROOT->GetFunction("landau");
  landau->SetParameter(1, maxBinCenter);
  Float_t fitMin = maxBinCenter - 1000.; /* fit from 10 ns before max */
  Float_t fitMax = maxBinCenter + binWidth; /* fit until a bin width above max */
  hT0Fill->Fit("landau", "q0", "", fitMin, fitMax);
  /* get rough landau mean and sigma to set a better fit range */
  Float_t mean = landau->GetParameter(1);
  Float_t sigma = landau->GetParameter(2);
  /* better landau fit of the edge */
  fitMin = maxBinCenter - 3. * sigma;
  fitMax = mean;
  hT0Fill->Fit("landau", "q0", "", fitMin, fitMax);
  /* print params */
  mean = landau->GetParameter(1);
  sigma = landau->GetParameter(2);
  Float_t meane = landau->GetParError(1);
  Float_t sigmae = landau->GetParError(2);
  Log(Form("edge fit: mean  = %f +- %f ps", mean, meane));
  Log(Form("edge fit: sigma = %f +- %f ps", sigma, sigmae));
  Float_t edge = mean - 3. * sigma;
  Float_t edgee = TMath::Sqrt(meane * meane + 3. * sigmae * 3. * sigmae);
  Log(Form("edge fit: edge = %f +- %f ps", edge, edgee));
  /* apply calibration to get T0-fill from egde */
  Float_t t0Fill = edge * t0FillCalibCoefficient + t0FillCalibOffset;
  Log(Form("estimated T0-fill: %f ps", t0Fill));

  /* create T0-fill object */
  AliTOFT0Fill *t0FillObject = new AliTOFT0Fill();
  t0FillObject->SetT0Fill(t0Fill);

  /* store reference data */
  if(fStoreRefData){
    AliCDBMetaData metaDataHisto;
    metaDataHisto.SetBeamPeriod(0);
    metaDataHisto.SetResponsible("Roberto Preghenella");
    metaDataHisto.SetComment("online T0-fill histogram");
    if (!StoreReferenceData("Calib","T0Fill", hT0Fill, &metaDataHisto)) {
      Log("error while storing reference data");
      delete hT0Fill;
      delete hit;
      delete t0FillObject;
      return 21;
    }
    Log("reference data successfully stored");
  }
  
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Roberto Preghenella");
  metaData.SetComment("online T0-fill measurement");
  if (!Store("Calib", "T0Fill", t0FillObject, &metaData, 0, kFALSE)) {
    Log("error while storing T0-fill object");
    delete hT0Fill;
    delete hit;
    delete t0FillObject;
    return 21;
  }
  Log("T0-fill object successfully stored");

  delete hT0Fill;
  delete hit;
  delete t0FillObject;
  return 0;

}
 

//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessPulserData()
{
  // Processing Pulser Run data for TOF channel status

  Log("Processing Pulser");

  if (fStatus==0x0){
	  AliError("No valid fStatus found, some errors must have occurred!!");
	  return 20;
  }

  TH1::AddDirectory(0);
  
  Bool_t resultPulserRef=kFALSE;
  Bool_t resultPulser=kFALSE;
  
  static const Int_t kSize = AliTOFGeometry::NPadXSector()*AliTOFGeometry::NSectors();
  TH1S * htofPulser = new TH1S("hTOFpulser","histo with signals on TOF during pulser", kSize,-0.5,kSize-0.5);
  for (Int_t ibin =1;ibin<=kSize;ibin++){
	  htofPulser->SetBinContent(ibin,-1);
  }
  
  // processing pulser
  
  TFile * daqFile=0x0;
  TH1S *h1=0x0;
  
  //retrieving Pulser data 
  TList* listPulser = GetFileSources(kDAQ, "PULSER");
  if (listPulser !=0x0 && listPulser->GetEntries()!=0)
	  {
		  AliInfo("The following sources produced files with the id PULSER");
		  listPulser->Print();
		  Int_t nPulser = 0;
		  for (Int_t jj=0;jj<listPulser->GetEntries();jj++){
			  Int_t nPulserSource = 0;
			  TObjString * str = dynamic_cast<TObjString*> (listPulser->At(jj));
			  AliInfo(Form("found source %s", str->String().Data()));
			  // file to be stored run per run
			  TString fileNamePulser = GetFile(kDAQ, "PULSER", str->GetName());
			  if (fileNamePulser.Length()>0){
				  // storing refernce data
				  AliInfo(Form("Got the file %s, now we can process pulser data.", fileNamePulser.Data()));
				  daqFile = new TFile(fileNamePulser.Data(),"READ");
				  h1 = (TH1S*) daqFile->Get("hTOFpulser");
				  for (Int_t ibin=0;ibin<kSize;ibin++){
					  if ((h1->GetBinContent(ibin+1))!=-1){
						  if ((htofPulser->GetBinContent(ibin+1))==-1){
							  htofPulser->SetBinContent(ibin+1,h1->GetBinContent(ibin+1));
						  }
						  else {
							  Log(Form("Something strange occurred during Pulser run, channel %i already read by another LDC, please check!",ibin));
						  }
					  }
				  }
				  
				  // elaborating infos
				  Double_t mean =0;
				  Int_t nread=0;
				  Int_t nreadNotEmpty=0;
				  for (Int_t ientry=1;ientry<=h1->GetNbinsX();ientry++){
					  
					  AliDebug(3,Form(" channel %i pulser status before pulser = %i, with global status = %i",ientry,(Int_t)fStatus->GetPulserStatus(ientry),(Int_t)fStatus->GetStatus(ientry)));
					  /* check whether channel has been read out during current run.
					   * if the status is bad it means it has not been read out.
					   * in this case skip channel in order to not affect the mean */ 
					  if (fStatus->GetHWStatus(ientry-1) == AliTOFChannelOnlineStatusArray::kTOFHWBad)
						  continue;
					  nPulser++;
					  nPulserSource++;
					  if (h1->GetBinContent(ientry)==-1) continue;
					  else {
						  if (h1->GetBinContent(ientry)>0) {
							  nreadNotEmpty++;
							  AliDebug(2,Form(" channel %i is ok with entry = %f; so far %i channels added ",ientry-1,h1->GetBinContent(ientry),nreadNotEmpty));
						  }
						  mean+=h1->GetBinContent(ientry);
						  nread++;
					  }
				  }
				  if (nread!=0) {
					  mean/=nread;
					  AliDebug(2,Form(" nread =  %i , nreadNotEmpty = %i, mean = %f",nread,nreadNotEmpty,mean));
					  for (Int_t ich =0;ich<fNChannels;ich++){
						  if (h1->GetBinContent(ich+1)==-1) continue;
						  AliDebug(3,Form(" channel %i pulser status before pulser = %i",ich,(Int_t)fStatus->GetPulserStatus(ich)));
						  
						  /* check whether channel has been read out during current run.
						   * if the status is bad it means it has not been read out.
						   * in this case skip channel in order to leave its status 
						   * unchanged */
						  if (fStatus->GetHWStatus(ich) == AliTOFChannelOnlineStatusArray::kTOFHWBad)
							  continue;
						  
						  if (h1->GetBinContent(ich+1)<0.05*mean){
							  fStatus->SetPulserStatus(ich,AliTOFChannelOnlineStatusArray::kTOFPulserBad);  // bad status for pulser
							  AliDebug(2,Form( " channel %i pulser status after pulser = %i (bad, content = %f), with global status = %i",ich,(Int_t)fStatus->GetPulserStatus(ich),h1->GetBinContent(ich+1),(Int_t)fStatus->GetStatus(ich)));
						  }
						  else {
							  fStatus->SetPulserStatus(ich,AliTOFChannelOnlineStatusArray::kTOFPulserOk);  // good status for pulser
							  AliDebug(2,Form( " channel %i pulser status after pulser = %i (good), with global status = %i",ich,(Int_t)fStatus->GetPulserStatus(ich),(Int_t)fStatus->GetStatus(ich)));
						  }
					  }
				  }
				  else {
					  Log("No channels read!! No action taken, keeping old status");
				  }
				  
				  daqFile->Close();
				  delete daqFile;
				  delete h1;
			  }
			  
			  else{
				  Log("The input data file from DAQ (pulser) was not found, TOF exiting from Shuttle "); 
				  delete listPulser;
				  delete htofPulser;
				  htofPulser = 0x0;
				  if (fStatus){
					  delete fStatus;
					  fStatus = 0;
				  }
				  return 10;//return error code for failure in retrieving Ref Data 
			  }
			  AliDebug(2,Form(" Number of channels processed during pulser run from source %i = %i",jj, nPulserSource));		 
		  }
		  AliDebug(2,Form(" Number of channels processed during pulser run = %i",nPulser));
		  delete listPulser;
	  }
  
  else{
	  Log("The input data file list from DAQ (pulser) was not found, TOF exiting from Shuttle "); 
	  delete htofPulser;
	  htofPulser = 0x0;
	  if (fStatus){
		  delete fStatus;
		  fStatus = 0;
	  }
	  return 10;//return error code for failure in retrieving Ref Data 
  }	
  
  //storing in OCDB  
  
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Chiara Zampolli");
  metaData.SetComment("This preprocessor fills an AliTOFChannelOnlineStatusArray object after a Pulser run.");
  AliInfo("Storing Calibration Data from Pulser Run");
  resultPulser = Store("Calib","Status",fStatus, &metaData,0,kTRUE);
  if(!resultPulser){
    Log("Some problems occurred while storing online object resulting from Pulser data processing");
    delete htofPulser;
    htofPulser = 0x0;
    if (fStatus){
	    delete fStatus;
	    fStatus = 0;
    }
    return 11;//return error code for problems in storing Pulser data 
  }

  if(fStoreRefData){
    
    AliCDBMetaData metaDataHisto;
    metaDataHisto.SetBeamPeriod(0);
    metaDataHisto.SetResponsible("Chiara Zampolli");
    char comment[200];
    sprintf(comment,"This preprocessor stores the Ref data from a pulser run.");
    metaDataHisto.SetComment(comment);
    AliInfo("Storing Reference Data");
    resultPulserRef = StoreReferenceData("Calib","PulserData",htofPulser, &metaDataHisto);
    if (!resultPulserRef){
      Log("some problems occurred::No Reference Data for pulser stored, TOF exiting from Shuttle");
      delete htofPulser;
      htofPulser = 0x0;
      if (fStatus){
	      delete fStatus;
	      fStatus = 0;
      }
      return 9;//return error code for failure in storing Ref Data 
    }
  }
  
  daqFile=0;

  delete htofPulser;
  htofPulser = 0x0;

  if (fStatus){
    delete fStatus;
    fStatus = 0;
  }

  return 0;
}
//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessNoiseData()
{

  // Processing Noise Run data for TOF channel status

  Log("Processing Noise");

  if (fStatus==0x0){
	  AliError("No valid fStatus found, some errors must have occurred!!");
	  return 20;
  }

  Float_t noiseThr = 1;   // setting default threshold for noise to 1 Hz
  // reading config map
  AliCDBEntry *cdbEntry = GetFromOCDB("Calib","ConfigNoise");
  if (!cdbEntry) {
	  Log(Form("No Configuration entry found in CDB, using default values: NoiseThr = %d",noiseThr));
  }
  else {
	  TMap *configMap = (TMap*)cdbEntry->GetObject();
	  if (!configMap){
		  Log(Form("No map found in Config entry in CDB, using default values: NoiseThr = %d", noiseThr));
	  }
	  else{
		  TObjString *strNoiseThr = (TObjString*)configMap->GetValue("NoiseThr");
		  if (strNoiseThr) {
			  TString tmpstr = strNoiseThr->GetString();
			  noiseThr = tmpstr.Atoi();
		  }
		  else {
			  Log(Form("No NoiseThr value found in Map from ConfigNoise entry in CDB, using default value: NoiseThr = %i",noiseThr));
		  }
	  }
  }

  delete cdbEntry;
  cdbEntry = 0x0;

  TH1::AddDirectory(0);

  Bool_t resultNoiseRef=kFALSE;
  Bool_t resultNoise=kFALSE;

  static const Int_t kSize = AliTOFGeometry::NPadXSector()*AliTOFGeometry::NSectors();
  TH1F * htofNoise = new TH1F("hTOFnoise","histo with signals on TOF during noise", kSize,-0.5,kSize-0.5);
  for (Int_t ibin =1;ibin<=kSize;ibin++){
	  htofNoise->SetBinContent(ibin,-1);
  }
  
  // processing noise
  
  TFile * daqFile=0x0;
  TH1F * h1=0x0;
  
  // useful counters
  Int_t nNoise = 0;
  Int_t nNoisyChannels = 0;
  Int_t nNotNoisyChannels = 0;
  Int_t nChannelsFromDA = 0;
  Int_t nMatchingWindowNullNonZero = 0;
  Int_t nMatchingWindowNullEqualZero = 0;

  // retrieving Noise data 
  TList* listNoise = GetFileSources(kDAQ, "NOISE");
  if (listNoise !=0x0 && listNoise->GetEntries()!=0)
	  {
		  AliInfo("The following sources produced files with the id NOISE");
		  listNoise->Print();
		  for (Int_t jj=0;jj<listNoise->GetEntries();jj++){
			  Int_t nNoiseSource = 0;
			  TObjString * str = dynamic_cast<TObjString*> (listNoise->At(jj));
			  AliInfo(Form("found source %s", str->String().Data()));
			  // file to be stored run per run
			  TString fileNameNoise = GetFile(kDAQ, "NOISE", str->GetName());
			  if (fileNameNoise.Length()>0){
				  // storing reference data
				  AliInfo(Form("Got the file %s, now we can process noise data.", fileNameNoise.Data()));
				  daqFile = new TFile(fileNameNoise.Data(),"READ");
				  h1 = (TH1F*) daqFile->Get("hTOFnoise");
				  for (Int_t ibin=0;ibin<kSize;ibin++){
					  if ((h1->GetBinContent(ibin+1))!=-1){
						  nNoiseSource++;
						  // checking the matching window for current channel
						  if (fMatchingWindow[ibin] == 0){
							  Log(Form("Matching window for channel %i null, but the channel was read by the LDC! skipping channel, BUT Please check!",ibin));
							  if ((h1->GetBinContent(ibin+1))!=0) nMatchingWindowNullNonZero++;						
							  if ((h1->GetBinContent(ibin+1))==0) nMatchingWindowNullEqualZero++;						
							  continue;
						  }
						  if ((htofNoise->GetBinContent(ibin+1))==-1){
							  htofNoise->SetBinContent(ibin+1,h1->GetBinContent(ibin+1)/(fMatchingWindow[ibin]*1.E-9));
							  if ((h1->GetBinContent(ibin+1))!= 0) AliDebug(2,Form("Channel = %i, Matching window = %i, Content = %f", ibin, fMatchingWindow[ibin], htofNoise->GetBinContent(ibin+1)));
						  }
						  else {
							  Log(Form("Something strange occurred during Noise run, channel %i already read by another LDC, please check!",ibin));
						  }
					  }
				  }

				  Log(Form(" Number of channels processed during noise run from source %i = %i",jj, nNoiseSource));
				  daqFile->Close();
				  delete daqFile;
				  delete h1;
				  daqFile = 0x0;
				  h1 = 0x0;

			  }
			  else{
				  Log("The input data file from DAQ (noise) was not found, TOF exiting from Shuttle "); 
				  delete listNoise;
				  listNoise = 0x0;
				  delete htofNoise;
				  htofNoise = 0x0;
				  if (fStatus){
					  delete fStatus;
					  fStatus = 0;
				  }
				  if (fMatchingWindow){
					  delete [] fMatchingWindow;
					  fMatchingWindow = 0;
				  }
				  return 13;//return error code for failure in retrieving Ref Data 
			  }
		  }		  
	  }
			  
  else{
	  Log("The input data file list from DAQ (noise) was not found, TOF exiting from Shuttle "); 
	  delete htofNoise;
	  htofNoise = 0x0;
	  if (fStatus){
		  delete fStatus;
		  fStatus = 0;
	  }
	  if (fMatchingWindow){
		  delete [] fMatchingWindow;
		  fMatchingWindow = 0;
	  }
	  return 13;//return error code for failure in retrieving Ref Data 
  }	
  
  // elaborating infos to set NOISE status
  for (Int_t ich =0;ich<fNChannels;ich++){
	  if (htofNoise->GetBinContent(ich+1)== -1) continue;

	  nChannelsFromDA++;

	  AliDebug(3,Form(" channel %i noise status before noise = %i, with global status = %i",ich,(Int_t)fStatus->GetNoiseStatus(ich),(Int_t)fStatus->GetStatus(ich)));
	  //AliDebug(2,Form( " channel %i status before noise = %i",ich,(Int_t)fStatus->GetNoiseStatus(ich)));
	  
	  /* check whether channel has been read out during current run.
	   * if the status is bad it means it has not been read out.
	   * in this case skip channel in order to leave its status 
	   * unchanged */

	  if ((fStatus->GetHWStatus(ich)) == AliTOFChannelOnlineStatusArray::kTOFHWBad)
		  continue;
	  
	  nNoise++;
	  if (htofNoise->GetBinContent(ich+1) >= noiseThr){
		  fStatus->SetNoiseStatus(ich,AliTOFChannelOnlineStatusArray::kTOFNoiseBad); // bad status for noise
		  AliDebug(3,Form( " channel %i noise status after noise = %i, with global status = %i",ich,(Int_t)fStatus->GetNoiseStatus(ich),(Int_t)fStatus->GetStatus(ich)));
		  nNoisyChannels++;
	  }
	  else {
		  fStatus->SetNoiseStatus(ich,AliTOFChannelOnlineStatusArray::kTOFNoiseOk); // good status for noise
		  AliDebug(3,Form(" channel %i noise status after noise = %i, with global status = %i",ich,(Int_t)fStatus->GetNoiseStatus(ich),(Int_t)fStatus->GetStatus(ich)));
		  nNotNoisyChannels++;
	  }
  }
  
  Log(Form(" Number of channels processed by DA during noise run, independetly from TOFFEE = %i",nChannelsFromDA));
  Log(Form(" Number of channels processed during noise run (that were ON according to TOFFEE) = %i",nNoise));
  Log(Form(" Number of noisy channels found during noise run = %i",nNoisyChannels));
  Log(Form(" Number of not noisy channels found during noise run = %i",nNotNoisyChannels));
  Log(Form(" Number of channels with matching window NULL (so skipped), but Non Zero content = %i",nMatchingWindowNullNonZero));
  Log(Form(" Number of channels with matching window NULL (so skipped), and Zero content = %i",nMatchingWindowNullEqualZero));

  delete listNoise;
  
  //daqFile=0;
  
  //storing in OCDB
  
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Chiara Zampolli");
  metaData.SetComment("This preprocessor fills an AliTOFChannelOnlineStatusArray object after a Noise run.");
  AliInfo("Storing Calibration Data from Noise Run");
  resultNoise = Store("Calib","Status",fStatus, &metaData,0,kTRUE);
  if(!resultNoise){
    Log("Some problems occurred while storing online object resulting from Noise data processing");
    delete htofNoise;
    htofNoise = 0x0;
    if (fStatus){
	    delete fStatus;
	    fStatus = 0;
    }
    if (fMatchingWindow){
	    delete [] fMatchingWindow;
	    fMatchingWindow = 0;
    }
    return 14;//return error code for problems in storing Noise data 
  }

  if(fStoreRefData){
    
    AliCDBMetaData metaDataHisto;
    metaDataHisto.SetBeamPeriod(0);
    metaDataHisto.SetResponsible("Chiara Zampolli");
    char comment[200];
    sprintf(comment,"This preprocessor stores the Ref data from a noise run. ");
    metaDataHisto.SetComment(comment);
    AliInfo("Storing Reference Data");
    resultNoiseRef = StoreReferenceData("Calib","NoiseData",htofNoise, &metaDataHisto);
    if (!resultNoiseRef){
      Log("some problems occurred::No Reference Data for noise stored");
      delete htofNoise;
      htofNoise = 0x0;
      if (fStatus){
	      delete fStatus;
	      fStatus = 0;
      }
      if (fMatchingWindow){
	      delete [] fMatchingWindow;
	      fMatchingWindow = 0;
      }
      return 12;//return error code for failure in storing Ref Data 
    }
  }

  delete htofNoise;
  htofNoise = 0x0;

  if (fStatus){
    delete fStatus;
    fStatus = 0;
  }

  if (fMatchingWindow){
	  delete [] fMatchingWindow;
	  fMatchingWindow = 0;
  }

  return 0;
}
//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::ProcessFEEData()
{
  // Processing Pulser Run data for TOF channel status
  // dummy for the time being

  Log("Processing FEE");

  //Bool_t updateOCDB = kFALSE;
  AliTOFFEEReader feeReader;

  TH1C hCurrentFEE("hCurrentFEE","histo with current FEE channel status", fNChannels, 0, fNChannels);
  
  /* load current TOF FEE(dump) from DCS FXS, 
   * setup TOFFEEdump object */

  const char * toffeeFileName = GetFile(kDCS,"TofFeeMap",""); 
  AliInfo(Form("toffee file name = %s", toffeeFileName));
  if (toffeeFileName == NULL) {
    return 15;
  } 
  AliTOFFEEDump feedump;
  feedump.ReadFromFile(toffeeFileName);
  
  /* load current TOF FEE(light) config from DCS FXS, parse, 
   * fill current FEE histogram and set FEE status */
  
  const char * nameFile = GetFile(kDCS,"TofFeeLightMap",""); 
  AliInfo(Form("toffeeLight file name = %s",nameFile));
  if (nameFile == NULL) {
	  return 15;
  } 
  feeReader.LoadFEElightConfig(nameFile);
  Int_t parseFee = feeReader.ParseFEElightConfig();
  AliDebug(2,Form("%i enabled channels found in FEElight configuration",parseFee));
  /* load stored TOF FEE from OCDB and compare it with current FEE.
   * if stored FEE is different from current FEE set update flag.
   * if there is no stored FEE in OCDB set update flag */
  
  fMatchingWindow = new Int_t[fNChannels];
  fLatencyWindow = new Int_t[fNChannels];
  
  AliCDBEntry *cdbEntry = GetFromOCDB("Calib","Status");
  if (!cdbEntry) {
	  /* no CDB entry found. set update flag */
	  Log("     ************ WARNING ************");
	  Log("No CDB Status entry found, creating a new one!");
	  Log("     *********************************");
	  fStatus = new AliTOFChannelOnlineStatusArray(fNChannels);
	  //updateOCDB = kTRUE;
	  fIsStatusMapChanged = kTRUE;
  }
  else {
	  if (cdbEntry) cdbEntry->SetOwner(kFALSE);
	  /* CDB entry OK. loop over channels */
	  fStatus = (AliTOFChannelOnlineStatusArray*) cdbEntry->GetObject();
	  delete cdbEntry;
	  cdbEntry = 0x0;
	  /* cehck whether status object has latency window data */
	  if (!fStatus->HasLatencyWindow()) {
	    /* create new status object and update OCDB */
	    Log("     ************ WARNING ************");
	    Log("CDB Status entry found but has no latency window data, creating a new one!");
	    Log("     *********************************");
	    delete fStatus;
	    fStatus = new AliTOFChannelOnlineStatusArray(fNChannels);
	    //updateOCDB = kTRUE;
	    fIsStatusMapChanged = kTRUE;
	  }
  }
  for (Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
	  //AliDebug(2,Form("********** channel %i",iChannel));
	  /* compare current FEE channel status with stored one 
	   * if different set update flag and break loop */
	  //AliDebug(2,Form( " channel %i status before FEE = %i",iChannel,(Int_t)fStatus->GetHWStatus(iChannel)));
	  fMatchingWindow[iChannel] = feeReader.GetMatchingWindow(iChannel);
	  fLatencyWindow[iChannel] = feeReader.GetLatencyWindow(iChannel);
	  if (feeReader.IsChannelEnabled(iChannel)) {
		  hCurrentFEE.SetBinContent(iChannel + 1, 1);
		  if (fStatus->GetHWStatus(iChannel)!=AliTOFChannelOnlineStatusArray::kTOFHWOk){
		          //updateOCDB = kTRUE;
			  fIsStatusMapChanged = kTRUE;
			  fStatus->SetHWStatus(iChannel,AliTOFChannelOnlineStatusArray::kTOFHWOk);
			  AliDebug(3,Form( " changed into enabled: channel %i status after FEE = %i",iChannel,(Int_t)fStatus->GetHWStatus(iChannel)));
		  }
		  if (fStatus->GetLatencyWindow(iChannel)!=fLatencyWindow[iChannel]){
		          //updateOCDB = kTRUE;
			  fIsStatusMapChanged = kTRUE;
			  fStatus->SetLatencyWindow(iChannel,fLatencyWindow[iChannel]);
			  AliDebug(3,Form( " changed latency window: channel %i latency window after FEE = %i",iChannel,fStatus->GetLatencyWindow(iChannel)));
		  }
	  }
	  else {
		  if (fStatus->GetHWStatus(iChannel)!=AliTOFChannelOnlineStatusArray::kTOFHWBad){
		          //updateOCDB = kTRUE;
			  fIsStatusMapChanged = kTRUE;
			  fStatus->SetHWStatus(iChannel,AliTOFChannelOnlineStatusArray::kTOFHWBad);
			  AliDebug(3,Form( " changed into disabled: channel %i status after FEE = %i",iChannel,(Int_t)fStatus->GetHWStatus(iChannel)));
		  }
	  }
  }


  /* check whether we don't have to store reference data.
   * in this case we return without errors. */
  if (fStoreRefData) {
    /* store reference data */
    AliCDBMetaData metaDataHisto;
    metaDataHisto.SetBeamPeriod(0);
    metaDataHisto.SetResponsible("Roberto Preghenella");
    metaDataHisto.SetComment("This preprocessor stores the FEE Ref data of the current run.");
    AliInfo("Storing FEE reference data");
    /* store FEE reference data */
    if (!StoreReferenceData("Calib", "FEEData", &hCurrentFEE, &metaDataHisto)) {
      /* failed */
      Log("problems while storing FEE reference data");
      if (fStatus){
	delete fStatus;
	fStatus = 0;
      }
      return 18; /* error return code for problems while storing FEE reference data */
    }
    
    /* store TOF FEE dump reference data */
    AliCDBMetaData metaDatadump;
    metaDatadump.SetBeamPeriod(0);
    metaDatadump.SetResponsible("Roberto Preghenella");
    metaDatadump.SetComment("This preprocessor stores the TOF FEE dump Ref data of the current run.");
    AliInfo("Storing TOF FEE dump reference data");
    /* store FEE reference data */
    if (!StoreReferenceData("Calib", "FEEDump", &feedump, &metaDatadump)) {
      /* failed */
      Log("problems while storing TOF FEE dump reference data");
      return 18; /* error return code for problems while storing FEE reference data */
    }
  }

  return 0;

}

//_____________________________________________________________________________

UInt_t AliTOFPreprocessor::Process(TMap *dcsAliasMap)
{
  //
  // Main AliTOFPreprocessor method called by SHUTTLE
  //

  TString runType = GetRunType();
  Log(Form("RunType %s",runType.Data()));
  
  // processing 

  /* always process FEE data */
  Int_t iresultFEE = ProcessFEEData();
  if (iresultFEE != 0)
    return iresultFEE;

  if (runType == "PULSER") {
    Int_t iresultPulser = ProcessPulserData();
    return iresultPulser; 
  }

  if (runType == "NOISE") { // for the time being associating noise runs with pedestal runs; proper run type to be defined 
    Int_t iresultNoise = ProcessNoiseData();
    return iresultNoise; 
  }
 
  if (runType == "PHYSICS") {
    //    Int_t iresultDAQ = ProcessOnlineDelays();
    Int_t iresultDAQ = ProcessT0Fill();
    if (iresultDAQ != 0) {
      return iresultDAQ;
    }
    else {
      Int_t iresultDCS = ProcessDCSDataPoints(dcsAliasMap);
      Int_t iResultHVandLVdps = ProcessHVandLVdps(dcsAliasMap);
      return iresultDCS+iResultHVandLVdps;
    }
  }

  // storing
  return 0;
}


//_____________________________________________________________________________

void
AliTOFPreprocessor::FillWithCosmicCalibration(AliTOFChannelOnlineArray *cal)
{
  /*
   * fill with cosmic calibration 
   */

  Log(" Using cosmic-ray calibration.");
  
  AliTOFcalibHisto calibHisto;
  calibHisto.SetFullCorrectionFlag(AliTOFcalibHisto::kTimeSlewingCorr, kFALSE);
  Log(Form(" loading calibration histograms from %s", calibHisto.GetCalibHistoFileName()));
  Log(Form(" loading calibration parameters from %s", calibHisto.GetCalibParFileName()));
  calibHisto.LoadCalibPar();
  
  /* loop over channel index */
  for (Int_t iIndex = 0; iIndex < fNChannels; iIndex++) {
    cal->SetDelay(iIndex, calibHisto.GetFullCorrection(iIndex));
  }
  
}

//_____________________________________________________________________________

void
AliTOFPreprocessor::FillWithCableLengthMap(AliTOFChannelOnlineArray *cal)
{
  /*
   * fill with cosmic calibration 
   */
  
  Log(" Using cable-length map.");
  AliTOFRawStream tofrs;
  Int_t det[5], dummy, index;
  Float_t cableTimeShift;
  
  /* temporarly disable warnings */
  AliLog::EType_t logLevel = (AliLog::EType_t)AliLog::GetGlobalLogLevel();
  AliLog::SetGlobalLogLevel(AliLog::kError);
  
  /* loop over EO indeces */
  for (Int_t iddl = 0; iddl < 72; iddl++)
    for (Int_t islot = 3; islot <= 12; islot++)
      for (Int_t ichain = 0; ichain < 2; ichain++)
	for (Int_t itdc = 0; itdc < 15; itdc++)
	  for (Int_t ichannel = 0; ichannel < 8; ichannel++) {
	    
	    /* get DO index */
	    tofrs.EquipmentId2VolumeId(iddl, islot, ichain, itdc, ichannel, det);
	    
	    /* swap det[3] and det[4] indeces (needed to obtain correct channel index) */
	    dummy = det[3];
	    det[3] = det[4];
	    det[4] = dummy;
	    
	    /* check DO index */
	    if (det[0] < 0 || det[0] > 17 ||
		det[1] < 0 || det[1] > 4 ||
		det[2] < 0 || det[2] > 18 ||
		det[3] < 0 || det[3] > 1 ||
		det[4] < 0 || det[4] > 47)
	      continue;
	    
	    /* get channel index */
	    index = AliTOFGeometry::GetIndex(det);
	    
	    /* get cable time shift */
	    cableTimeShift = AliTOFCableLengthMap::GetCableTimeShift(iddl, islot, ichain, itdc);
	    
	    /* set delay */
	    if (index<fNChannels) {
	      cal->SetDelay(index,cableTimeShift);  // delay in ns
	      AliDebug(2,Form("Setting delay %f (ns) for channel %i",cableTimeShift,index));
	    }
	    
	  } /* loop over EO indeces */
  
  /* re-enable warnings */
  AliLog::SetGlobalLogLevel(logLevel);
  
}



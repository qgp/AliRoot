/**************************************************************************
 * Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved.      *
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


#include "AliTPCPreprocessor.h"
#include "AliShuttleInterface.h"

#include "AliCDBMetaData.h"
#include "AliDCSValue.h"
#include "AliLog.h"
#include "AliTPCSensorTempArray.h"
#include "AliTPCROC.h"
#include "AliTPCCalROC.h"
#include "AliTPCCalPad.h"
#include "AliTPCCalibPedestal.h"
#include "AliTPCCalibPulser.h"
#include "AliTPCCalibCE.h"
#include "TFile.h"
#include "TTree.h"
#include "TEnv.h"

#include <TTimeStamp.h>

const Int_t kValCutTemp = 100;               // discard temperatures > 100 degrees
const Int_t kDiffCutTemp = 5;	             // discard temperature differences > 5 degrees
const TString kPedestalRunType = "PEDESTAL_RUN";  // pedestal run identifier
const TString kPulserRunType = "PULSER_RUN";   // pulser run identifier
const TString kPhysicsRunType = "PHYSICS";   // physics run identifier
const TString kStandAloneRunType = "STANDALONE"; // standalone run identifier
const TString kDaqRunType = "DAQ"; // DAQ run identifier
const TString kAmandaTemp = "tpc_PT_%d.Temperature"; // Amanda string for temperature entries
//const Double_t kFitFraction = 0.7;                 // Fraction of DCS sensor fits required              
const Double_t kFitFraction = -1.0;          // Don't require minimum number of fits in commissioning run 

//
// This class is the SHUTTLE preprocessor for the TPC detector.
//

ClassImp(AliTPCPreprocessor)

//______________________________________________________________________________________________
AliTPCPreprocessor::AliTPCPreprocessor(AliShuttleInterface* shuttle) :
  AliPreprocessor("TPC",shuttle),
  fConfEnv(0), fTemp(0), fHighVoltage(0), fConfigOK(kTRUE), fROC(0)
{
  // constructor
  fROC = AliTPCROC::Instance();
}
//______________________________________________________________________________________________
// AliTPCPreprocessor::AliTPCPreprocessor(const AliTPCPreprocessor& org) :
//   AliPreprocessor(org),
//   fConfEnv(0), fTemp(0), fHighVoltage(0), fConfigOK(kTRUE)
// {
//   // copy constructor not implemented
//   //   -- missing underlying copy constructor in AliPreprocessor
//
//   Fatal("AliTPCPreprocessor", "copy constructor not implemented");
//
// //  fTemp = new AliTPCSensorTempArray(*(org.fTemp));
// }

//______________________________________________________________________________________________
AliTPCPreprocessor::~AliTPCPreprocessor()
{
  // destructor

  delete fTemp;
  delete fHighVoltage;
}
//______________________________________________________________________________________________
AliTPCPreprocessor& AliTPCPreprocessor::operator = (const AliTPCPreprocessor& )
{
  Fatal("operator =", "assignment operator not implemented");
  return *this;
}


//______________________________________________________________________________________________
void AliTPCPreprocessor::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
  // Creates AliTestDataDCS object

  AliPreprocessor::Initialize(run, startTime, endTime);

	AliInfo(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run,
		TTimeStamp(startTime).AsString(),
		TTimeStamp(endTime).AsString()));

  // Preprocessor configuration

	AliCDBEntry* entry = GetFromOCDB("Config", "Preprocessor");
        if (entry) fConfEnv = (TEnv*) entry->GetObject();
        if ( fConfEnv==0 ) {
           Log("AliTPCPreprocsessor: Preprocessor Config OCDB entry missing.\n");
	   fConfigOK = kFALSE;
           return;
        }

  // Temperature sensors

        TTree *confTree = 0;
	entry = GetFromOCDB("Config", "Temperature");
        if (entry) confTree = (TTree*) entry->GetObject();
        if ( confTree==0 ) {
           Log("AliTPCPreprocsessor: Temperature Config OCDB entry missing.\n");
	   fConfigOK = kFALSE;
	   return;
        }
        fTemp = new AliTPCSensorTempArray(fStartTime, fEndTime, confTree, kAmandaTemp);
	fTemp->SetValCut(kValCutTemp);
	fTemp->SetDiffCut(kDiffCutTemp);


  // High voltage measurements

      if (false) {    // high voltage maps not yet implemented...
        confTree=0;
        entry=0;
        entry = GetFromOCDB("Config", "HighVoltage");
        if (entry) confTree = (TTree*) entry->GetObject();
        if ( confTree==0 ) {
           Log("AliTPCPreprocsessor: High Voltage Config OCDB entry missing.\n");
           fConfigOK = kFALSE;
           return;
        }
        fHighVoltage = new AliDCSSensorArray(fStartTime, fEndTime, confTree);
      }
}

//______________________________________________________________________________________________
UInt_t AliTPCPreprocessor::Process(TMap* dcsAliasMap)
{
  // Fills data into TPC calibrations objects

  // Amanda servers provide information directly through dcsAliasMap

  if (!dcsAliasMap) return 9;
  if (dcsAliasMap->GetEntries() == 0 ) return 9;
  if (!fConfigOK) return 9;

  TString runType = GetRunType();

  // Temperature sensors are processed by AliTPCCalTemp


  UInt_t tempResult = MapTemperature(dcsAliasMap);
  UInt_t result=tempResult;

  // High Voltage recordings


 if (false) {   // High Voltage maps not yet implemented..
 
  UInt_t hvResult = MapHighVoltage(dcsAliasMap);
  result+=hvResult;

 }

  // Other calibration information will be retrieved through FXS files
  //  examples:
  //    TList* fileSourcesDAQ = GetFile(AliShuttleInterface::kDAQ, "pedestals");
  //    const char* fileNamePed = GetFile(AliShuttleInterface::kDAQ, "pedestals", "LDC1");
  //
  //    TList* fileSourcesHLT = GetFile(AliShuttleInterface::kHLT, "calib");
  //    const char* fileNameHLT = GetFile(AliShuttleInterface::kHLT, "calib", "LDC1");

  // pedestal entries

  if(runType == kPedestalRunType) {
    Int_t numSources = 1;
    Int_t pedestalSource[2] = {AliShuttleInterface::kDAQ,AliShuttleInterface::kHLT} ;
    TString source = fConfEnv->GetValue("Pedestal","DAQ");
    source.ToUpper();
    if ( source == "HLT") pedestalSource[0] = AliShuttleInterface::kHLT;
    if (!GetHLTStatus()) pedestalSource[0] = AliShuttleInterface::kDAQ;
    if (source == "HLTDAQ" ) {
        numSources=2;
	pedestalSource[0] = AliShuttleInterface::kHLT;
	pedestalSource[1] = AliShuttleInterface::kDAQ;
    }
    if (source == "DAQHLT" ) numSources=2;
    UInt_t pedestalResult=0;
    for (Int_t i=0; i<numSources; i++ ) {	
      UInt_t pedestalResult = ExtractPedestals(pedestalSource[i]);
      if ( pedestalResult == 0 ) break;
    }
    result += pedestalResult;

  }

  // pulser trigger processing

  if(runType == kPulserRunType) {
    Int_t numSources = 1;
    Int_t pulserSource[2] = {AliShuttleInterface::kDAQ,AliShuttleInterface::kHLT} ;
    TString source = fConfEnv->GetValue("Pulser","DAQ");
    source.ToUpper();
    if ( source == "HLT") pulserSource[0] = AliShuttleInterface::kHLT;
    if (!GetHLTStatus()) pulserSource[0] = AliShuttleInterface::kDAQ;
    if (source == "HLTDAQ" ) {
        numSources=2;
	pulserSource[0] = AliShuttleInterface::kHLT;
	pulserSource[1] = AliShuttleInterface::kDAQ;
    }
    if (source == "DAQHLT" ) numSources=2;
    UInt_t pulserResult=0;
    for (Int_t i=0; i<numSources; i++ ) {	
      pulserResult = ExtractPulser(pulserSource[i]);
      if ( pulserResult == 0 ) break;
    }
    result += pulserResult;

  }



  // Central Electrode processing

  if( runType == kPhysicsRunType || runType == kStandAloneRunType || 
      runType == kDaqRunType ) {    

    Int_t numSources = 1;
    Int_t ceSource[2] = {AliShuttleInterface::kDAQ,AliShuttleInterface::kHLT} ;
    TString source = fConfEnv->GetValue("CE","DAQ");
    source.ToUpper();
    if ( source == "HLT") ceSource[0] = AliShuttleInterface::kHLT;
    if (!GetHLTStatus()) ceSource[0] = AliShuttleInterface::kDAQ;
    if (source == "HLTDAQ" ) {
        numSources=2;
	ceSource[0] = AliShuttleInterface::kHLT;
	ceSource[1] = AliShuttleInterface::kDAQ;
    }
    if (source == "DAQHLT" ) numSources=2;
    UInt_t ceResult=0;
    for (Int_t i=0; i<numSources; i++ ) {	
      ceResult = ExtractCE(ceSource[i]);
      if ( ceResult == 0 ) break;
    }
    result += ceResult;

  }

  return result;
}
//______________________________________________________________________________________________
UInt_t AliTPCPreprocessor::MapTemperature(TMap* dcsAliasMap)
{

   // extract DCS temperature maps. Perform fits to save space

  UInt_t result=0;
  TMap *map = fTemp->ExtractDCS(dcsAliasMap);
  if (map) {
    fTemp->MakeSplineFit(map);
    Double_t fitFraction = 1.0*fTemp->NumFits()/fTemp->NumSensors(); 
    if (fitFraction > kFitFraction ) {
      AliInfo(Form("Temperature values extracted, fits performed.\n"));
    } else { 
      Log ("Too few temperature maps fitted. \n");
      result = 9;
    }
  } else {
    Log("No temperature map extracted. \n");
    result=9;
  }
  delete map;
  // Now store the final CDB file

  if ( result == 0 ) {
        AliCDBMetaData metaData;
	metaData.SetBeamPeriod(0);
	metaData.SetResponsible("Haavard Helstrup");
	metaData.SetComment("Preprocessor AliTPC data base entries.");

	Bool_t storeOK = Store("Calib", "Temperature", fTemp, &metaData, 0, kFALSE);
        if ( !storeOK )  result=1;

   }

   return result;

}

//______________________________________________________________________________________________
UInt_t AliTPCPreprocessor::MapHighVoltage(TMap* dcsAliasMap)
{

   // extract DCS HV maps. Perform fits to save space

  UInt_t result=0;
  TMap *map = fHighVoltage->ExtractDCS(dcsAliasMap);
  if (map) {
    fHighVoltage->MakeSplineFit(map);
    Double_t fitFraction = 1.0*fHighVoltage->NumFits()/fHighVoltage->NumSensors(); 
    if (fitFraction > kFitFraction ) {
      AliInfo(Form("High volatge recordings extracted, fits performed.\n"));
    } else { 
      Log ("Too few high voltage recordings fitted. \n");
      result = 9;
    }
  } else {
    Log("No high voltage recordings extracted. \n");
    result=9;
  }
  delete map;
  // Now store the final CDB file

  if ( result == 0 ) {
        AliCDBMetaData metaData;
	metaData.SetBeamPeriod(0);
	metaData.SetResponsible("Haavard Helstrup");
	metaData.SetComment("Preprocessor AliTPC data base entries.");

	Bool_t storeOK = Store("Calib", "HighVoltage", fHighVoltage, &metaData, 0, kFALSE);
        if ( !storeOK )  result=1;

   }

   return result;

}


//______________________________________________________________________________________________

UInt_t AliTPCPreprocessor::ExtractPedestals(Int_t sourceFXS)
{
 //
 //  Read pedestal file from file exchage server
 //  Keep original entry from OCDB in case no new pedestals are available
 //
 AliTPCCalPad *calPadPed=0;
 AliCDBEntry* entry = GetFromOCDB("Calib", "Pedestals");
 if (entry) calPadPed = (AliTPCCalPad*)entry->GetObject();
 if ( calPadPed==NULL ) {
     Log("AliTPCPreprocsessor: No previous TPC pedestal entry available.\n");
     calPadPed = new AliTPCCalPad("PedestalsMean","PedestalsMean");
 }

 AliTPCCalPad *calPadRMS=0;
 entry = GetFromOCDB("Calib", "Noise");
 if (entry) calPadRMS = (AliTPCCalPad*)entry->GetObject();
 if ( calPadRMS==NULL ) {
     Log("AliTPCPreprocsessor: No previous TPC noise entry available.\n");
     calPadRMS = new AliTPCCalPad("PedestalsRMS","PedestalsRMS");
 }


 UInt_t result=0;

 Int_t nSectors = fROC->GetNSectors();
 TList* list = GetFileSources(sourceFXS,"pedestals");
 
 if (list && list->GetEntries()>0) {

//  loop through all files from LDCs

    Bool_t changed=false;
    UInt_t index = 0;
    while (list->At(index)!=NULL) {
     TObjString* fileNameEntry = (TObjString*) list->At(index);
     if (fileNameEntry!=NULL) {
        TString fileName = GetFile(sourceFXS, "pedestals",
	                                 fileNameEntry->GetString().Data());
        TFile *f = TFile::Open(fileName);
        if (!f) {
	  Log ("Error opening pedestal file.");
	  result =2;
	  break;
	}
        AliTPCCalibPedestal *calPed;
	f->GetObject("AliTPCCalibPedestal",calPed);
        if ( !calPed ) {
	  Log ("No pedestal calibration object in file.");
	  result = 2;
	  break;
	}

        //  replace entries for the sectors available in the present file

        changed=true;
        for (Int_t sector=0; sector<nSectors; sector++) {
           AliTPCCalROC *rocPed=calPed->GetCalRocPedestal(sector, kFALSE);
           if ( rocPed )  calPadPed->SetCalROC(rocPed,sector);
           AliTPCCalROC *rocRMS=calPed->GetCalRocRMS(sector, kFALSE);
           if ( rocRMS )  calPadRMS->SetCalROC(rocRMS,sector);
        }
      }
     ++index;
    }  // while(list)
//
//  Store updated pedestal entry to OCDB
//
    if (changed) {
     AliCDBMetaData metaData;
     metaData.SetBeamPeriod(0);
     metaData.SetResponsible("Haavard Helstrup");
     metaData.SetComment("Preprocessor AliTPC data base entries."); 
 
     Bool_t storeOK = Store("Calib", "Pedestals", calPadPed, &metaData, 0, kTRUE);
     if ( !storeOK ) ++result;
     storeOK = Store("Calib", "PadNoise", calPadRMS, &metaData, 0, kTRUE);
     if ( !storeOK ) ++result;
    }
  } else {
    Log ("Error: no entries in input file list!");
    result = 1;
  }

  return result;
}

//______________________________________________________________________________________________


UInt_t AliTPCPreprocessor::ExtractPulser(Int_t sourceFXS)
{
 //
 //  Read pulser calibration file from file exchage server
 //  Keep original entry from OCDB in case no new pulser calibration is available
 //
 TObjArray    *pulserObjects=0;
 AliTPCCalPad *pulserTmean=0;
 AliTPCCalPad *pulserTrms=0;
 AliTPCCalPad *pulserQmean=0;
 AliCDBEntry* entry = GetFromOCDB("Calib", "Pulser");
 if (entry) pulserObjects = (TObjArray*)entry->GetObject();
 if ( pulserObjects==NULL ) {
     Log("AliTPCPreprocsessor: No previous TPC pulser entry available.\n");
     pulserObjects = new TObjArray;    
 }

 pulserTmean = (AliTPCCalPad*)pulserObjects->FindObject("PulserTmean");
 if ( !pulserTmean ) {
    pulserTmean = new AliTPCCalPad("PulserTmean","PulserTmean");
    pulserObjects->Add(pulserTmean);
 }
 pulserTrms = (AliTPCCalPad*)pulserObjects->FindObject("PulserTrms");
 if ( !pulserTrms )  { 
    pulserTrms = new AliTPCCalPad("PulserTrms","PulserTrms");
    pulserObjects->Add(pulserTrms);
 }
 pulserQmean = (AliTPCCalPad*)pulserObjects->FindObject("PulserQmean");
 if ( !pulserQmean )  { 
    pulserQmean = new AliTPCCalPad("PulserQmean","PulserQmean");
    pulserObjects->Add(pulserQmean);
 }


 UInt_t result=0;

 Int_t nSectors = fROC->GetNSectors();
 TList* list = GetFileSources(sourceFXS,"pulser");
 
 if (list && list->GetEntries()>0) {

//  loop through all files from LDCs

    Bool_t changed=false;
    UInt_t index = 0;
    while (list->At(index)!=NULL) {
     TObjString* fileNameEntry = (TObjString*) list->At(index);
     if (fileNameEntry!=NULL) {
        TString fileName = GetFile(sourceFXS, "pulser",
	                                 fileNameEntry->GetString().Data());
        TFile *f = TFile::Open(fileName);
        if (!f) {
	  Log ("Error opening pulser file.");
	  result =2;
	  break;
	}
        AliTPCCalibPulser *calPulser;
	f->GetObject("AliTPCCalibPulser",calPulser);
        if ( !calPulser ) {
	  Log ("No pulser calibration object in file.");
	  result = 2;
	  break;
	}

        //  replace entries for the sectors available in the present file

        changed=true;
        for (Int_t sector=0; sector<nSectors; sector++) {
           AliTPCCalROC *rocTmean=calPulser->GetCalRocT0(sector);
           if ( rocTmean )  pulserTmean->SetCalROC(rocTmean,sector);
           AliTPCCalROC *rocTrms=calPulser->GetCalRocRMS(sector);
           if ( rocTrms )  pulserTrms->SetCalROC(rocTrms,sector);
           AliTPCCalROC *rocQmean=calPulser->GetCalRocQ(sector);
           if ( rocQmean )  pulserQmean->SetCalROC(rocQmean,sector);
        }
      }
     ++index;
    }  // while(list)
//
//  Store updated pedestal entry to OCDB
//
    if (changed) {
     AliCDBMetaData metaData;
     metaData.SetBeamPeriod(0);
     metaData.SetResponsible("Haavard Helstrup");
     metaData.SetComment("Preprocessor AliTPC data base entries.");

     Bool_t storeOK = Store("Calib", "Pulser", pulserObjects, &metaData, 0, kTRUE);
     if ( !storeOK ) ++result;
    }  
  } else {
    Log ("Error: no entries in input file list!");
    result = 1;
  }

  return result;
}

UInt_t AliTPCPreprocessor::ExtractCE(Int_t sourceFXS)
{
 //
 //  Read Central Electrode file from file exchage server
 //  Keep original entry from OCDB in case no new CE calibration is available
 //
 TObjArray    *ceObjects=0;
 AliTPCCalPad *ceTmean=0;
 AliTPCCalPad *ceTrms=0;
 AliTPCCalPad *ceQmean=0;
 AliCDBEntry* entry = GetFromOCDB("Calib", "CE");
 if (entry) ceObjects = (TObjArray*)entry->GetObject();
 if ( ceObjects==NULL ) {
     Log("AliTPCPreprocsessor: No previous TPC central electrode entry available.\n");
     ceObjects = new TObjArray;    
 }

 ceTmean = (AliTPCCalPad*)ceObjects->FindObject("CETmean");
 if ( !ceTmean ) {
    ceTmean = new AliTPCCalPad("CETmean","CETmean");
    ceObjects->Add(ceTmean);
 }
 ceTrms = (AliTPCCalPad*)ceObjects->FindObject("CETrms");
 if ( !ceTrms )  { 
    ceTrms = new AliTPCCalPad("CETrms","CETrms");
    ceObjects->Add(ceTrms);
 }
 ceQmean = (AliTPCCalPad*)ceObjects->FindObject("CEQmean");
 if ( !ceQmean )  { 
    ceQmean = new AliTPCCalPad("CEQmean","CEQmean");
    ceObjects->Add(ceQmean);
 }


 UInt_t result=0;

 Int_t nSectors = fROC->GetNSectors();
 TList* list = GetFileSources(sourceFXS,"CE");
 
 if (list && list->GetEntries()>0) {

//  loop through all files from LDCs

    UInt_t index = 0;
    while (list->At(index)!=NULL) {
     TObjString* fileNameEntry = (TObjString*) list->At(index);
     if (fileNameEntry!=NULL) {
        TString fileName = GetFile(sourceFXS, "CE",
	                                 fileNameEntry->GetString().Data());
        TFile *f = TFile::Open(fileName);
        if (!f) {
	  Log ("Error opening central electrode file.");
	  result =2;
	  break;
	}
        AliTPCCalibCE *calCE;
	f->GetObject("AliTPCCalibCE",calCE);

        //  replace entries for the sectors available in the present file

        for (Int_t sector=0; sector<nSectors; sector++) {
           AliTPCCalROC *rocTmean=calCE->GetCalRocT0(sector);
           if ( rocTmean )  ceTmean->SetCalROC(rocTmean,sector);
           AliTPCCalROC *rocTrms=calCE->GetCalRocRMS(sector);
           if ( rocTrms )  ceTrms->SetCalROC(rocTrms,sector);
           AliTPCCalROC *rocQmean=calCE->GetCalRocQ(sector);
           if ( rocQmean )  ceQmean->SetCalROC(rocQmean,sector);
        }
      }
     ++index;
    }  // while(list)
//
//  Store updated pedestal entry to OCDB
//
    AliCDBMetaData metaData;
    metaData.SetBeamPeriod(0);
    metaData.SetResponsible("Haavard Helstrup");
    metaData.SetComment("Preprocessor AliTPC data base entries.");

    Bool_t storeOK = Store("Calib", "CE", ceObjects, &metaData, 0, kTRUE);
    if ( !storeOK ) ++result;
    
  } else {
    Log ("Error: no entries!");
    result = 1;
  }

  return result;
}

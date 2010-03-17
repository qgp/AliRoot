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

//-------------------------------------------------------------------------
//                          Class AliGRPPreprocessor
//                  Global Run Parameters (GRP) preprocessor
//    Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//    Modified: Ernesto.Lopez.Torres@cern.ch  CEADEN-CERN
//    Modified: Chiara.Zampolli@cern.ch  CERN
//-------------------------------------------------------------------------

#include <TChain.h>
#include <TList.h>
#include <TMap.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TGraph.h>
#include <TString.h>
#include <TFile.h>

#include <float.h>

#include "AliGRPPreprocessor.h"
#include "AliGRPObject.h"
#include "AliDCSSensor.h"
#include "AliSplineFit.h"
#include "AliDCSSensorArray.h"
#include "AliRawEventHeaderVersions.h"

#include "AliTriggerConfiguration.h"
#include "AliTriggerRunScalers.h"

#include "AliCDBMetaData.h"
#include "AliLog.h"
#include "AliESDVertex.h"
#include "AliLHCReader.h"
#include "AliLHCData.h"
#include "AliDCSArray.h"

class AliDCSValue;
class AliShuttleInterface;

// needed for ReceivePromptRecoParameters

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <AliCDBManager.h>
#include <AliCDBMetaData.h>
#include <AliCDBId.h>
#include <AliTriggerConfiguration.h>
#include <AliCTPTimeParams.h>

const Double_t kFitFraction = -1.;                 // Fraction of DCS sensor fits required

ClassImp(AliGRPPreprocessor)

//_______________________________________________________________

  const Int_t AliGRPPreprocessor::fgknDAQLbPar = 8; // num parameters in the logbook for PHYSICS runs, when beamType from DAQ logbook == NULL
  const Int_t AliGRPPreprocessor::fgknDAQLbParReduced = 7; // num parameters in the logbook for the other cases
  const Int_t AliGRPPreprocessor::fgknDCSDP = 48;   // number of dcs dps
  const Int_t AliGRPPreprocessor::fgknDCSDPHallProbes = 40;   // number of dcs dps
  const Int_t AliGRPPreprocessor::fgknLHCDP = 5;   // number of dcs dps from LHC data
  const Int_t AliGRPPreprocessor::fgkDCSDPHallTopShift = 4;   // shift from the top to get tp the Hall Probes names in the list of DCS DPs
  const Int_t AliGRPPreprocessor::fgkDCSDPNonWorking = 5; // number of non working DCS DPs
  const char* AliGRPPreprocessor::fgkDCSDataPoints[AliGRPPreprocessor::fgknDCSDP] = {
                   "L3Polarity",
                   "DipolePolarity",
                   "L3Current",
                   "DipoleCurrent",
		   "L3_BSF17_H1",
		   "L3_BSF17_H2",
		   "L3_BSF17_H3",
		   "L3_BSF17_Temperature",
		   "L3_BSF4_H1",
		   "L3_BSF4_H2",
		   "L3_BSF4_H3",
		   "L3_BSF4_Temperature",
		   "L3_BKF17_H1",
		   "L3_BKF17_H2",
		   "L3_BKF17_H3",
		   "L3_BKF17_Temperature",
		   "L3_BKF4_H1",
		   "L3_BKF4_H2",
		   "L3_BKF4_H3",
		   "L3_BKF4_Temperature",
		   "L3_BSF13_H1",
		   "L3_BSF13_H2",
		   "L3_BSF13_H3",
		   "L3_BSF13_Temperature",
		   "L3_BSF8_H1",
		   "L3_BSF8_H2",
		   "L3_BSF8_H3",
		   "L3_BSF8_Temperature",
		   "L3_BKF13_H1",
		   "L3_BKF13_H2",
		   "L3_BKF13_H3",
		   "L3_BKF13_Temperature",
		   "L3_BKF8_H1",
		   "L3_BKF8_H2",
		   "L3_BKF8_H3",
		   "L3_BKF8_Temperature",
		   "Dipole_Inside_H1",
		   "Dipole_Inside_H2",
		   "Dipole_Inside_H3",
		   "Dipole_Inside_Temperature",
		   "Dipole_Outside_H1",
		   "Dipole_Outside_H2",
		   "Dipole_Outside_H3",
		   "Dipole_Outside_Temperature",
                   "CavernTemperature",
                   "CavernAtmosPressure",
                   "SurfaceAtmosPressure",
                   "CavernAtmosPressure2"
                 };

  const char* AliGRPPreprocessor::fgkDCSDataPointsHallProbes[AliGRPPreprocessor::fgknDCSDPHallProbes] = {
		   "L3_BSF17_H1",
		   "L3_BSF17_H2",
		   "L3_BSF17_H3",
		   "L3_BSF17_Temperature",
		   "L3_BSF4_H1",
		   "L3_BSF4_H2",
		   "L3_BSF4_H3",
		   "L3_BSF4_Temperature",
		   "L3_BKF17_H1",
		   "L3_BKF17_H2",
		   "L3_BKF17_H3",
		   "L3_BKF17_Temperature",
		   "L3_BKF4_H1",
		   "L3_BKF4_H2",
		   "L3_BKF4_H3",
		   "L3_BKF4_Temperature",
		   "L3_BSF13_H1",
		   "L3_BSF13_H2",
		   "L3_BSF13_H3",
		   "L3_BSF13_Temperature",
		   "L3_BSF8_H1",
		   "L3_BSF8_H2",
		   "L3_BSF8_H3",
		   "L3_BSF8_Temperature",
		   "L3_BKF13_H1",
		   "L3_BKF13_H2",
		   "L3_BKF13_H3",
		   "L3_BKF13_Temperature",
		   "L3_BKF8_H1",
		   "L3_BKF8_H2",
		   "L3_BKF8_H3",
		   "L3_BKF8_Temperature",
		   "Dipole_Inside_H1",
		   "Dipole_Inside_H2",
		   "Dipole_Inside_H3",
		   "Dipole_Inside_Temperature",
		   "Dipole_Outside_H1",
		   "Dipole_Outside_H2",
		   "Dipole_Outside_H3",
		   "Dipole_Outside_Temperature"
                 };
                 
  const Short_t kSensors = 45; // start index position of sensor in DCS DPs
  const Short_t kNumSensors = 3; // Number of sensors in DCS DPs (CavernAtmosPressure, SurfaceAtmosPressure, CavernAtmosPressure2)


  const char* AliGRPPreprocessor::fgkLHCDataPoints[AliGRPPreprocessor::fgknLHCDP] = {
	  "lhcMon_LHCBeam.Energy",
	  "lhcMon_LHCMachineMode.value",
	  "lhcMon_LHCBeamMode.value",
          "dip/acc/LHC/RunControl/BeamType/Beam1.payload",
          "dip/acc/LHC/RunControl/BeamType/Beam2.payload"
  };
  const char* kppError[] = {
                   "",
                   "(DAQ logbook ERROR)",
                   "(DAQ FXS ERROR)",
                   "(Trigger Scalers not found in DCS FXS - ERROR)",
                   "(DCS data points ERROR)",
                   "(Trigger Configuration ERROR)",
                   "(DAQ logbook ERROR determining partition of the run)",
                   "(CTP timing ERROR)",
		   "(SPD Mean Vertex ERROR)",
		   "(DCS FXS Error for LHC Data)",
		   "(LHC Data Error)"
  };

//_______________________________________________________________

AliGRPPreprocessor::AliGRPPreprocessor(AliShuttleInterface* shuttle):
	AliPreprocessor("GRP",shuttle),  fPressure(0), fmaxFloat(0), fminFloat(0),fmaxDouble(0), fminDouble(0), fmaxInt(0), fminInt(0), fmaxUInt(0), fminUInt(0),fdaqStartEndTimeOk(kTRUE),ffailedDPs(new TObjArray(fgknDCSDP))
{
	// constructor - shuttle must be instantiated!

	AddRunType("COSMIC");
	AddRunType("LASER");
	AddRunType("PHYSICS");
	AddRunType("CALIBRATION_BC");
	AddRunType("CALIBRATION_CENTRAL");
	AddRunType("CALIBRATION_EMD");
	AddRunType("CALIBRATION_MB");
	AddRunType("CALIBRATION_SEMICENTRAL");
	AddRunType("CALIBRATION");
	AddRunType("PEDESTAL");
	AddRunType("STANDALONE");
	AddRunType("GAIN");
	AddRunType("NOISE");
	AddRunType("PULSER");
        AddRunType("STANDALONE_PULSER");
        AddRunType("STANDALONE_BC");

	fmaxFloat = FLT_MAX;
	fminFloat = -FLT_MAX;
	fmaxDouble = DBL_MAX;
	fminDouble = -DBL_MAX;
	fmaxInt = kMaxInt;
	fminInt = kMinInt;
	fmaxUInt = kMaxUInt;
	fminUInt = 0;

	AliInfo(Form("Max allowed float = %6.5e",fmaxFloat));
	AliInfo(Form("Min allowed float = %6.5e",fminFloat));
	AliInfo(Form("Max allowed double = %6.5e",fmaxDouble));
	AliInfo(Form("Min allowed double = %6.5e",fminDouble));
	AliInfo(Form("Max allowed integer = %d",fmaxInt));
	AliInfo(Form("Min allowed integer = %d",fminInt));
	AliInfo(Form("Max allowed unsigned integer = %u",(Int_t)fmaxUInt));
	AliInfo(Form("Min allowed unsigned integer = %u",(Int_t)fminUInt));

	ffailedDPs->SetOwner(kTRUE);
}

//_______________________________________________________________

AliGRPPreprocessor::~AliGRPPreprocessor()
{
	//destructor
	
	delete fPressure;
	delete ffailedDPs;

}

//_______________________________________________________________

void AliGRPPreprocessor::Initialize(Int_t run, UInt_t startTime, UInt_t endTime)
{
	// Initialize preprocessor

	AliPreprocessor::Initialize(run, startTime, endTime);
	
	AliInfo("Initialization of the GRP preprocessor.");
	AliInfo(Form("Start Time DCS = %d",GetStartTimeDCSQuery()));
	AliInfo(Form("End Time DCS = %d",GetEndTimeDCSQuery()));
	TClonesArray * array = new TClonesArray("AliDCSSensor",kNumSensors); 
	for(Int_t j = 0; j < kNumSensors; j++) {
		AliDCSSensor * sens = new ((*array)[j])AliDCSSensor;
		sens->SetStringID(fgkDCSDataPoints[j+kSensors]);
	}
	AliInfo(Form("Pressure Entries: %d",array->GetEntries()));
	
	fPressure = new AliDCSSensorArray(GetStartTimeDCSQuery(), GetEndTimeDCSQuery(), array);

	for (Int_t iDP=0; iDP < fgknDCSDP; iDP++){
		TObjString* dp = new TObjString(fgkDCSDataPoints[iDP]);
		ffailedDPs->AddAt(dp,iDP);
	}

}

//_______________________________________________________________

UInt_t AliGRPPreprocessor::Process(TMap* valueMap)
{
	// process data retrieved by the Shuttle
	
	// retrieving "partition" and "detector" fields from DAQ logbook to 
	// determine the partition in which the run was taken
	// the partition is used to decide how to react in case of errors for CTP

	TString partition = (TString)GetRunParameter("partition");  
	TString detector = (TString)GetRunParameter("detector");   

	AliGRPObject *grpobj = new AliGRPObject();  // object to store data
	grpobj->SetBeamEnergyIsSqrtSHalfGeV(); // new format

	//=================//
	// DAQ logbook     //
	//=================//
	UInt_t error = 0;
	
	Int_t iDaqLB = ProcessDaqLB(grpobj);
	TString runType = (TString)GetRunType();
	TString beamType = (TString)GetRunParameter("beamType");
	//if((runType == "PHYSICS" && iDaqLB == fgknDAQLbPar && beamType!="Cosmics") ||  (runType == "PHYSICS" && iDaqLB == fgknDAQLbParReduced && beamType=="Cosmics") || (runType != "PHYSICS" && iDaqLB == fgknDAQLbParReduced)) {
	if((runType == "PHYSICS" && iDaqLB == fgknDAQLbPar && !beamType.IsNull()) ||  (runType == "PHYSICS" && iDaqLB == fgknDAQLbParReduced && beamType.IsNull()) || (runType != "PHYSICS" && iDaqLB == fgknDAQLbParReduced)) {
		Log(Form("DAQ Logbook, successful!"));
	} else {
		Log(Form("DAQ Logbook, could not get all expected entries!!!"));
		error |= 1;
	}

	//=================//
	// DAQ FXS         //
	//=================//
	UInt_t iDaqFxs = ProcessDaqFxs();
	if( iDaqFxs == 0 ) {
		Log(Form("DAQ FXS, successful!"));
	} else {
		Log(Form("DAQ FXS, could not store run raw tag file!!!"));
		error |= 2;
	}
	
	//=================//
	// DCS FXS         //
	//=================//
	UInt_t iDcsFxs = ProcessDcsFxs(partition, detector);
	if( iDcsFxs == 0 ) {
		Log(Form("DCS FXS, successful!"));
	} else  if (iDcsFxs ==1) {
		Log(Form("DCS FXS, Could not store CTP scalers!!!"));
		error |= 4;
	} else{
		Log(Form("Incorrect field in DAQ logbook for partition = %s and detector = %s, going into error without CTP scalers...",partition.Data(),detector.Data()));
		error |= 32;
	}
	
	//=================//
	// DCS data points //
	//=================//
	Log(Form("Starting DCS Query at %d and finishing at %d",GetStartTimeDCSQuery(),GetEndTimeDCSQuery()));
	Int_t entries = ProcessDcsDPs( valueMap, grpobj );
	Log(Form("entries found = %d (should be %d)",entries, fgknDCSDP-fgkDCSDPNonWorking));
	if (fdaqStartEndTimeOk){
		if( entries < fgknDCSDP - fgkDCSDPNonWorking ) { // L3_BSF4_H3, L3_BSF17_H1, L3_BSF17_H2, L3_BSF17_H3, L3_BSF17_Temperature are not working yet...  
			Log(Form("Possible problem with the DCS data points!!! Only %d/%d entries found - Please read further for more details",entries,fgknDCSDP-fgkDCSDPNonWorking));
			Log(Form("The DPs giving problems were:"));
			for (Int_t iDP = 0; iDP < fgknDCSDP; iDP++){
				TObjString *dpString = (TObjString*)ffailedDPs->At(iDP);
				if (dpString){
					TString name = dpString->String();
					if (name != "L3_BSF4_H3" && name != "L3_BSF17_H1" && name != "L3_BSF17_H2" && name != "L3_BSF17_H3" && name != "L3_BSF17_Temperature" ){
						Log(Form("******** %s ******** not present, but foreseen --> causing an ERROR",name.Data()));
					}
					else {
						Log(Form(" %s is not present, but was not generating any error since it is not ready in DCS - check the other DPs in this list!",name.Data()));
					}
				}
			}
			error |= 8;
		}
		else  Log(Form("DCS data points, successful!"));
	}
	else Log(Form("Statistical values for DCS DPs could not be computed due to missing DAQ_time_start and DAQ_time_end fields in DAQ logbook")); 
	
	//=======================//
	// Trigger Configuration //
	//=======================//

	const char * triggerConf = GetTriggerConfiguration();

	if (partition.IsNull() && !detector.IsNull()){ // standalone partition
		Log("STANDALONE partition for current run, using Trigger Configuration dummy value");
		AliCDBEntry *cdbEntry = GetFromOCDB("CTP","DummyConfig");
		if (!cdbEntry) {
			Log(Form("No dummy CTP configuration entry found, going into error..."));
			error |= 16;
		}
		else{
			AliTriggerConfiguration *runcfg = (AliTriggerConfiguration*)cdbEntry->GetObject();
			if (!runcfg){
				Log(Form("dummy CTP config not found in OCDB entry, going into error..."));
				error |= 16;
			}
			else {
				TString titleCTPcfg = Form("CTP cfg for run %i from Dummy entry in OCDB",fRun);
				runcfg->SetTitle(titleCTPcfg);
				AliCDBMetaData metaData;
				metaData.SetResponsible("Roman Lietava");
				metaData.SetComment("CTP run configuration from dummy entry in OCDB");
				if (!Store("CTP","Config", runcfg, &metaData, 0, 0)) {
					Log("Unable to store the dummy CTP run configuration object to OCDB!");
					error |= 16;
				}
			}
		}
	}

	else if (!partition.IsNull() && detector.IsNull()){ // global partition
		Log("GLOBAL partition for current run, using Trigger Configuration from DAQ Logbook");
		if (triggerConf!= NULL) {
			Log("Found trigger configuration in DAQ logbook");
			AliTriggerConfiguration *runcfg = AliTriggerConfiguration::LoadConfigurationFromString(triggerConf);	  
			if (!runcfg) {
				Log("Bad CTP run configuration file from DAQ logbook! The corresponding CDB entry will not be filled!");
				error |= 16;
			}
			else {
				TString titleCTPcfg = Form("CTP cfg for run %i from DAQ",fRun);
				runcfg->SetTitle(titleCTPcfg);
				AliCDBMetaData metaData;
				metaData.SetBeamPeriod(0);
				metaData.SetResponsible("Roman Lietava");
				metaData.SetComment("CTP run configuration from DAQ logbook");
				if (!Store("CTP","Config", runcfg, &metaData, 0, 0)) {
					Log("Unable to store the CTP run configuration object to OCDB!");
					error |= 16;
				}
			}
		}

		else {
			Log("Trigger configuration NULL in DAQ logbook");
			error |= 16;
		}
	}

	else {
		Log(Form("Incorrect field in DAQ logbook for partition = %s and detector = %s, going into error without trigger configuration...",partition.Data(),detector.Data()));
		error |= 32;
	}

        //===========================//
	// Trigger Timing Parameters //
        //===========================//

	
	const char * triggerCTPtiming = GetCTPTimeParams();

	if (partition.IsNull() && !detector.IsNull()){ // standalone partition
		Log("STANDALONE partition for current run, using CTP timing params dummy value");
		AliCDBEntry *cdbEntry = GetFromOCDB("CTP","DummyCTPtime");
		if (!cdbEntry) {
			Log(Form("No dummy CTP timing parameters entry found, going into error..."));
			error |= 64;
		}
		else{
			AliCTPTimeParams *runCTPtiming = (AliCTPTimeParams*)cdbEntry->GetObject();
			if (!runCTPtiming){
				Log(Form("dummy CTP timing parameters not found in OCDB entry, going into error..."));
				error |= 64;
			}
			else {
				TString titleCTPtiming = Form("CTP timing params for run %i from Dummy entry in OCDB",fRun);
				runCTPtiming->SetTitle(titleCTPtiming);
				AliCDBMetaData metadata;
				metadata.SetResponsible("Roman Lietava");
				metadata.SetComment("CTP run timing parameters from dummy entry in OCDB");
				if (!Store("CTP","CTPtiming", runCTPtiming, &metadata, 0, 0)) {
					Log("Unable to store the dummy CTP timing params object to OCDB!");
					error |= 64;
				}
			}
		}
	}

	else if (!partition.IsNull() && detector.IsNull()){ // global partition
		Log("GLOBAL partition for current run, using Trigger Timing Parameters from DAQ Logbook");
		if (triggerCTPtiming!= NULL) {
			Log("Found trigger timing params in DAQ logbook");
			AliDebug(2,Form("%s",triggerCTPtiming));
			AliCTPTimeParams *runCTPtiming = AliCTPTimeParams::LoadCTPTimeParamsFromString(triggerCTPtiming);	  
			if (!runCTPtiming) {
				Log("Bad CTP trigger timing params file from DAQ logbook! The corresponding CDB entry will not be filled!");
				error |= 64;
			}
			else {
				TString titleCTPtiming = Form("CTP timing params for run %i from DAQ",fRun);
				runCTPtiming->SetTitle(titleCTPtiming);
				AliCDBMetaData metadata;
				metadata.SetBeamPeriod(0);
				metadata.SetResponsible("Roman Lietava");
				metadata.SetComment("CTP timing params from DAQ logbook");
				if (!Store("CTP","CTPtiming", runCTPtiming, &metadata, 0, 0)) {
					Log("Unable to store the CTP timing params object to OCDB!");
					error |= 64;
				}
			}
		}

		else {
			Log("Trigger timing params NULL in DAQ logbook");
			error |= 64;
		}
	}

	else {
		Log(Form("Incorrect field in DAQ logbook for partition = %s and detector = %s, going into error without trigger timing parameters...",partition.Data(),detector.Data()));
		error |= 32;
	}


	//=================//
	// LHC Data        //
	//=================//

	UInt_t iLHCData = ProcessLHCData(grpobj);

	if( iLHCData == 0 ) {
		Log(Form("LHC Data from DCS FXS, successful!"));
	} else  if (iLHCData == 1) {
		Log(Form("LHC Data, problems with DCS FXS!"));
		error |= 256;
	} else if (iLHCData ==3){
		Log(Form("Problems in storing LHC Data - but not going into Error"));
	} else if (iLHCData ==4){
		Log(Form("Problems with LHC Data to be put in /GRP/GRP/LHCData - but not going into Error"));
	} else{
		Log(Form("LHC Data problems"));
		error |= 512;
	}
	
	//==================//
	// SPD Mean Vertex  //
	//==================//
	if (runType == "PHYSICS"){
		UInt_t iSPDMeanVertex = ProcessSPDMeanVertex();
		if( iSPDMeanVertex == 1 ) {
			Log(Form("SPD Mean Vertex, successful!"));
		} else {
			Log(Form("SPD Mean Vertex failed!!!"));
			error |= 128; 
		}
	}
	else {
		Log("SPD Mean Vertex not processed since runType != PHYSICS");
	}

	// storing AliGRPObject in OCDB

	AliCDBMetaData md;
	md.SetResponsible("Chiara Zampolli");
	md.SetComment("Output parameters from the GRP preprocessor.");
	
	Bool_t result = kTRUE;
	result = Store("GRP", "Data", grpobj, &md); 
	delete grpobj;

	if (result && !error ) {
		Log("GRP Preprocessor Success");
		return 0;
	} else {
		Log( Form("GRP Preprocessor FAILS!!! %s%s%s%s%s%s%s%s%s%s",
			  kppError[(error&1)?1:0],
			  kppError[(error&2)?2:0],
			  kppError[(error&4)?3:0],
			  kppError[(error&8)?4:0],
			  kppError[(error&16)?5:0],
			  kppError[(error&32)?6:0],
			  kppError[(error&64)?7:0],
			  kppError[(error&128)?8:0],
			  kppError[(error&256)?9:0],
			  kppError[(error&512)?10:0]
			  ));
		return error;
	}


}

//_______________________________________________________________

UInt_t AliGRPPreprocessor::ProcessLHCData(AliGRPObject *grpobj)
{
	//
	//Getting the LHC Data from DCS FXS
	//

	TString timeStartString = (TString)GetRunParameter("DAQ_time_start");
	TString timeEndString = (TString)GetRunParameter("DAQ_time_end");
	if (timeStartString.IsNull() || timeEndString.IsNull()){
		if (timeStartString.IsNull()){ 
			AliError("DAQ_time_start not set in logbook! Setting statistical values for current DP to invalid");
		}
		else if (timeEndString.IsNull()){
			AliError("DAQ_time_end not set in logbook! Setting statistical values for current DP to invalid");
		}
		return 2;
	}  

	Double_t timeStart = timeStartString.Atof();
	Double_t timeEnd = timeEndString.Atof();

	TString fileName = GetFile(kDCS, "LHCData","");
	if (fileName.Length()>0){
		AliInfo("Got The LHC Data file");
		AliLHCReader lhcReader;
		TMap* lhcMap = (TMap*)lhcReader.ReadLHCDP(fileName.Data());
		if (lhcMap) {
			Log(Form("LHCData map entries = %d",lhcMap->GetEntries()));
			
			// Processing data to be put in AliGRPObject
			// Energy
			TObjArray* energyArray = (TObjArray*)lhcMap->GetValue(fgkLHCDataPoints[0]);
			if (energyArray){			
				Float_t energy = ProcessEnergy(energyArray,timeStart,timeEnd);
				if (energy != -1) {
					grpobj->SetBeamEnergy(energy);
					grpobj->SetBeamEnergyIsSqrtSHalfGeV(kTRUE);
				}
			}
			else {
				AliError("Energy not found in LHC Data file!!!");
			}	
			Double_t timeBeamMode = 0;
			Double_t timeMachineMode = 0;
			// BeamMode
			TObjArray* beamModeArray = (TObjArray*)lhcMap->GetValue(fgkLHCDataPoints[2]);
			if (beamModeArray){		
				if (beamModeArray->GetEntries()==0){
					AliInfo("No Beam Mode found, setting it to UNKNOWN");
					grpobj->SetLHCState("UNKNOWN");
				}
				else{
					AliDCSArray* beamMode = (AliDCSArray*)beamModeArray->At(0);
					TObjString* beamModeString = beamMode->GetStringArray(0);
					if (beamModeArray->GetEntries()>1){
						AliDCSArray* beamMode1 = (AliDCSArray*)beamModeArray->At(1);
						timeBeamMode = beamMode1->GetTimeStamp();
						AliWarning(Form("The beam mode changed at timestamp %f! Setting it to the first value found and setting MaxTimeLHCValidity",timeBeamMode));
					}
					AliInfo(Form("LHC State (corresponding to BeamMode) = %s",(beamModeString->String()).Data()));
					grpobj->SetLHCState(beamModeString->String());
				}
				
			}
			else{
				AliError("Beam mode array not found in LHC Data file!!!");
			}
			// MachineMode
			TObjArray* machineModeArray = (TObjArray*)lhcMap->GetValue(fgkLHCDataPoints[1]);
			if (machineModeArray){
				if (machineModeArray->GetEntries()==0){
					AliInfo("No Machine Mode found, setting it to UNKNOWN");
					grpobj->SetMachineMode("UNKNOWN");
				}
				else{
					AliDCSArray* machineMode = (AliDCSArray*)machineModeArray->At(0);
					TObjString* machineModeString = machineMode->GetStringArray(0);
					if (machineModeArray->GetEntries()>1){
						AliDCSArray* machineMode1 = (AliDCSArray*)machineModeArray->At(1);
						timeMachineMode = machineMode1->GetTimeStamp();
						AliWarning(Form("The Machine Mode changed at timestamp %f! Setting it to the first value found and setting MaxTimeLHCValidity",timeMachineMode));
					}
					AliInfo(Form("Machine Mode = %s",(machineModeString->String()).Data()));
					grpobj->SetMachineMode(machineModeString->String());
				}
			}
			else {
				AliError("Machine mode array not found in LHC Data file!!!");
			}	
			if (timeBeamMode!=0 || timeMachineMode!=0){
				Double_t minTimeLHCValidity;
 				if (timeBeamMode == 0){
					minTimeLHCValidity = timeMachineMode;
				}
				else if (timeMachineMode == 0){
					minTimeLHCValidity = timeBeamMode;
				}
				else {
					minTimeLHCValidity= TMath::Min(timeBeamMode,timeMachineMode);
				}
				AliWarning(Form("Setting MaxTimeLHCValidity to %f",minTimeLHCValidity));
				grpobj->SetMaxTimeLHCValidity(minTimeLHCValidity);
			}
			
			// BeamType1 and BeamType2 
			TObjArray* beam1Array = (TObjArray*)lhcMap->GetValue(fgkLHCDataPoints[3]);
			if (beam1Array){			
				AliInfo(Form("%d entries for Beam1",beam1Array->GetEntries()));
			}
			else{
				AliError("Beam1 array not found in LHC data file!!!");
			}			
			TObjArray* beam2Array = 
				(TObjArray*)lhcMap->GetValue(fgkLHCDataPoints[4]);
			if (beam2Array){
				AliInfo(Form("%d entries for Beam2",beam2Array->GetEntries()));
			}
			else{
				AliError("Beam2 array not found in LHC data file!!!");
			}	
			// Processing data to go to AliLHCData object
			AliLHCData* dt = new AliLHCData(lhcMap,timeStart,timeEnd);
							
			// storing AliLHCData in OCDB
			if (dt){			
				AliCDBMetaData md;
				md.SetResponsible("Ruben Shahoyan");
				md.SetComment("LHC data from the GRP preprocessor.");
				
				Bool_t result = kTRUE;
				result = Store("GRP", "LHCData", dt, &md); 
				delete dt;
				if (result) return 0;
				else return 3;
			}
			else return 4;
			delete lhcMap; 
		}
		else {
			AliError("Cannot read correctly LHCData file");
			return 2;
		}
	}
  
	else {
		AliError("No LHCData file found in DCS FXS");
		return 1;
	}

}

//_______________________________________________________________

UInt_t AliGRPPreprocessor::ProcessSPDMeanVertex()
{
	//
	//Getting the SPD Mean Vertex
	//

	TList* list = GetForeignFileSources("SPD", kDAQ, "VertexDiamond");
	Bool_t storeResult = kTRUE;
	if (list !=0x0 && list->GetEntries()!=0)
		{
			AliInfo("The following sources produced files with the id VertexDiamond from SPD");
			list->Print();
			for (Int_t jj=0;jj<list->GetEntries();jj++){
				TObjString * str = dynamic_cast<TObjString*> (list->At(jj));
				AliInfo(Form("found source %s", str->String().Data()));
				TString fileNameRun = GetForeignFile("SPD", kDAQ, "VertexDiamond", str->GetName());
				if (fileNameRun.Length()>0){
					AliInfo(Form("Got the file %s", fileNameRun.Data()));
					TFile daqFile(fileNameRun.Data(),"READ");
					if (daqFile.IsOpen()) {
						AliESDVertex* meanVtx = dynamic_cast<AliESDVertex*>(daqFile.Get("MeanVertexPos"));
						if (meanVtx){
							meanVtx->Print();	
							// storing in the OCDB 
							AliCDBMetaData md;
							md.SetResponsible("Cvetan Cheshkov");
							md.SetComment("SPD Mean Vertex");					
							storeResult = Store("Calib", "MeanVertexSPD", meanVtx, &md, 0, kTRUE); 
						}
						else{
							AliWarning("No SPD Mean Vertex object found in file");
						} 
					}
					else {
						AliError("Can't open file");
						storeResult = kFALSE;
					}
				}
				else{
					AliWarning("No file found for current source for SPD Mean Vertex");
				}
			}
		}
	else {
		AliWarning("No list found for SPD Mean Vertex");
	}

	if (list) delete list;

	return storeResult;
}


//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessDaqLB(AliGRPObject* grpObj)
{
	//Getting the DAQ lb information
	
	time_t timeStart = (time_t)(((TString)GetRunParameter("DAQ_time_start")).Atoi());
	time_t timeEnd = (time_t)(((TString)GetRunParameter("DAQ_time_end")).Atoi());
	Float_t beamEnergy = (Float_t)(((TString)GetRunParameter("beamEnergy")).Atof());
	TString beamType = (TString)GetRunParameter("beamType");
	Char_t numberOfDetectors = (Char_t)(((TString)GetRunParameter("numberOfDetectors")).Atoi());
	UInt_t  detectorMask = (UInt_t)(((TString)GetRunParameter("detectorMask")).Atoi());
	TString lhcPeriod = (TString)GetRunParameter("LHCperiod");
	TString runType = (TString)GetRunType();

	UInt_t nparameter = 0;
	if (timeStart != 0){
		grpObj->SetTimeStart(timeStart);
		Log(Form("Start time for run %d: %d",fRun, (Int_t)timeStart));
		nparameter++;
	} 
	else {
		Log(Form("Start time not put in logbook, setting to invalid in GRP entry!"));
	}

	if (timeEnd != 0){
		grpObj->SetTimeEnd(timeEnd);
		Log(Form("End time for run %d: %i",fRun, (Int_t)timeEnd));
		nparameter++;
	} 
	else {
		Log(Form("End time not put in logbook, setting to invalid in GRP entry!"));
	}

	if (beamEnergy != 0){
		grpObj->SetBeamEnergy(beamEnergy);
		Log(Form("Beam Energy for run %d: %f",fRun, beamEnergy));
		//if ((runType == "PHYSICS" && beamType!="Cosmics")){
		if ((runType == "PHYSICS" && !beamType.IsNull())){   // if beamType is NOT Null, then we're not in a Cosmics run
			nparameter++; // increasing nparameters only in case we're in PHYSICS runs with beamType != NULL
		}
	} 
	else {
		//if ((runType == "PHYSICS" && beamType!="Cosmics")){
		if ((runType == "PHYSICS" && !beamType.IsNull())){ // if beamType is NOT Null, then we're not in a Cosmics run
			Log(Form("Beam Energy not put in logbook, setting to invalid in GRP entry, and producing an error (beamType = %s, runType = %s)",beamType.Data(), runType.Data()));
		}
		else{
			Log(Form("Beam Energy not put in logbook, setting to invalid in GRP entry, but not producing any error (beamType = NULL, runType = %s)", runType.Data()));
		}
	}

		
	if (beamType.Length() != 0){
		grpObj->SetBeamType(beamType);
		Log(Form("Beam Type for run %d: %s",fRun, beamType.Data()));
		nparameter++; 
	} 
	else {
		Log(Form("Beam Type not put in logbook, setting to invalid in GRP entry! Not producing any error, considering this as a Cosmics run"));
		nparameter++;
	}
		
	if (numberOfDetectors != 0){
		grpObj->SetNumberOfDetectors(numberOfDetectors);
		Log(Form("Number Of Detectors for run %d: %d",fRun, (Int_t)numberOfDetectors));
		nparameter++;
	} 
	else {
		Log(Form("Number Of Detectors not put in logbook, setting to invalid in GRP entry!"));
	}

	if (detectorMask != 0){
		grpObj->SetDetectorMask(detectorMask);
		Log(Form("Detector Mask for run %d: %d",fRun, detectorMask));
		nparameter++;
	} 
	else {
		Log(Form("Detector Mask not put in logbook, setting to invalid in GRP entry!"));
	}

	if (lhcPeriod.Length() != 0) {
		grpObj->SetLHCPeriod(lhcPeriod);
		Log(Form("LHC period (DAQ) for run %d: %s",fRun, lhcPeriod.Data()));
		nparameter++;
	} 
	else {
		Log(Form("LHCperiod not put in logbook, setting to invalid in GRP entry!"));
	}
	if (runType.Length() != 0) {
		grpObj->SetRunType(runType);
		Log(Form("Run Type (DAQ) for run %d: %s",fRun, runType.Data()));
		nparameter++;
	} 
	else {
		Log(Form("Run Type not put in logbook, setting to invalid in GRP entry!"));
	}

	return nparameter;
}

//_______________________________________________________________

UInt_t AliGRPPreprocessor::ProcessDaqFxs()
{
	//======DAQ FXS======//
	
	AliRawEventHeaderV3_9::Class()->IgnoreTObjectStreamer(); // to avoid trying reading TObject store in AliRawEventHeaderV3_9 - temporary fix 
	TList* list = GetFileSources(kDAQ);  
	if (!list) {
		Log("No raw data tag list: connection problems with DAQ FXS logbook!");
		return 1;
	}
	
	if (list->GetEntries() == 0) {
		Log("no raw data tags in this run: nothing to merge!");
		delete  list; list=0;
		return 0;
	}
	
	TChain *fRawTagChain = new TChain("T");
	Int_t nFiles=0;
	TIterator* iter = list->MakeIterator();
	TObject* obj = 0;
	while ((obj = iter->Next())) {
		TObjString* objStr = dynamic_cast<TObjString*> (obj);
		if (objStr) {
			Log(Form("Found source %s", objStr->String().Data()));
			TList* list2 = GetFileIDs(kDAQ, objStr->String());
			if (!list2) {
				Log("No list with ids from DAQ was found: connection problems with DAQ FXS logbook!");
				delete fRawTagChain; fRawTagChain=0;
				return 1;
			}
			Log(Form("Number of ids: %d",list2->GetEntries()));
			for(Int_t i = 0; i < list2->GetEntries(); i++) {
				TObjString *idStr = (TObjString *)list2->At(i);
				TString fileName = GetFile(kDAQ,idStr->String().Data(),objStr->String().Data());
				if (fileName.Length() > 0) {
					Log(Form("Adding file in the chain: %s",fileName.Data()));
					fRawTagChain->Add(fileName.Data());
					nFiles++;
				} else {
					Log(Form("Could not retrieve file with id %s from source %s: "
						 "connection problems with DAQ FXS!",
						 idStr->String().Data(), objStr->String().Data()));
					delete list; list=0;
					delete list2; list2=0;
					delete fRawTagChain; fRawTagChain=0;
					return 2;
				}
			}
			delete list2;
		}
	}
	
	TString fRawDataFileName = "GRP_Merged.tag.root";
	Log(Form("Merging %d raw data tags into file: %s", nFiles, fRawDataFileName.Data()));
	
	if( fRawTagChain->Merge(fRawDataFileName) < 1 ) {
		Log("Error merging raw data files!!!");
		return 3;
	}
	
	TString outputfile = Form("Run%d.Merged.RAW.tag.root", fRun);
	Bool_t result = StoreRunMetadataFile(fRawDataFileName.Data(),outputfile.Data());
	
	if (!result) {
		Log("Problem storing raw data tags in local file!!!");
	} else {
		Log("Raw data tags merged successfully!!");
	}
	
	delete iter;
	delete list;
	delete fRawTagChain; fRawTagChain=0;
	
	if (result == kFALSE) {
		return 4;
	}
	
	return 0;
	
}

//_______________________________________________________________
UInt_t AliGRPPreprocessor::ProcessDcsFxs(TString partition, TString detector)
{

	// processing the info
	// stored in the DCS FXS
	// coming from the trigger

	// Get the CTP counters information

	if (partition.IsNull() && !detector.IsNull()){ // standalone partition
		Log("STANDALONE partition for current run, using Trigger Scalers dummy value");
		AliCDBEntry *cdbEntry = GetFromOCDB("CTP","DummyScalers");
		if (!cdbEntry) {
			Log(Form("No dummy CTP scalers entry found, going into error..."));
			return 1;
		}
		else{
			AliTriggerRunScalers *scalers = (AliTriggerRunScalers*)cdbEntry->GetObject();
			if (!scalers){
				Log(Form("CTP dummy scalers not found in OCDB entry, going into error..."));
				return 1;
			}
			else {
				AliCDBMetaData metaData;
				metaData.SetResponsible("Roman Lietava");
				metaData.SetComment("CTP scalers from dummy entry in OCDB");
				if (!Store("CTP","Scalers", scalers, &metaData, 0, 0)) {
					Log("Unable to store the dummy CTP scalers object to OCDB!");
					return 1;
				}
			}
		}
	}

	else if (!partition.IsNull() && detector.IsNull()){ // global partition
		Log("GLOBAL partition for current run, using CTP scalers from DCS FXS");
		TString countersfile = GetFile(kDCS, "CTP_xcounters","");
		if (countersfile.IsNull()) {
			Log("No CTP counters files has been found: empty source!");
			return 1;
		}
		else {
			Log(Form("File with Id CTP_xcounters found in DCS FXS! Copied to %s",countersfile.Data()));
			AliTriggerRunScalers *scalers = AliTriggerRunScalers::ReadScalers(countersfile);
			if (!scalers) {
				Log("Bad CTP counters file! The corresponding CDB entry will not be filled!");
				return 1;
			}
			else {
				AliCDBMetaData metaData;
				metaData.SetBeamPeriod(0);
				metaData.SetResponsible("Roman Lietava");
				metaData.SetComment("CTP scalers");
				if (!Store("CTP","Scalers", scalers, &metaData, 0, 0)) {
					Log("Unable to store the CTP scalers object to OCDB!");
					delete scalers;					
					return 1;
				}
			}
			delete scalers;
		}
	}
	

	else{	
		Log(Form("Incorrect field in DAQ logbook for partition = %s and detector = %s, going into error...",partition.Data(),detector.Data()));
		return 2;
	}

	return 0;

}
//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessDcsDPs(TMap* valueMap, AliGRPObject* grpObj)
{

	//
	// processing DCS DPs
	//

	Int_t entries = 0;  // counting the entries that are in the DCS DB, not taking care whether they have values or not
	Int_t nL3Entries = 0;
	Int_t nDipoleEntries = 0;
	Int_t nEnvEntries = 0;
	Int_t nHallProbesEntries = 0;
	nL3Entries = ProcessL3DPs(valueMap, grpObj);
	nDipoleEntries = ProcessDipoleDPs(valueMap, grpObj);
	nEnvEntries = ProcessEnvDPs(valueMap, grpObj);
	nHallProbesEntries = ProcessHPDPs(valueMap, grpObj);
	grpObj->SetPolarityConventionLHC();  // after the dipole cables swap we comply with LHC convention
	Log(Form("L3Entries = %d, nDipoleEntries =%d, nEnvEntries = %d, nHallProbesEntries = %d", nL3Entries, nDipoleEntries, nEnvEntries, nHallProbesEntries));
	entries = nL3Entries + nDipoleEntries + nEnvEntries + nHallProbesEntries;
	return entries;

}

//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessL3DPs(const TMap* valueMap, AliGRPObject* grpObj)
{

	// processing DPs
	// related to 
	// L3 info

	Int_t nL3Entries = 0;

	TObjArray *array = 0x0;
	Int_t indexDP = -1;
	Bool_t isZero = kTRUE; // flag to monitor L3Current. If set to true, the magnet is OFF, and the polarity can change

	AliInfo(Form("==========L3Current==========="));
	Bool_t outOfRange = kFALSE;  // flag to monitor if any value collected by DCS is out of range
	indexDP = kL3Current;
	array = (TObjArray *)valueMap->GetValue(fgkDCSDataPoints[indexDP]);
	if(!array) {
		Log(Form("%s not found in the map!!!",fgkDCSDataPoints[indexDP]));
	} 
	else {
		if (array->GetEntries() == 0){
			AliError(Form("No entries found in array! setting %s to invalid...",fgkDCSDataPoints[indexDP]));
		}
		else {
			Float_t *floatDCS = ProcessFloatAllMagnet(array, indexDP, isZero);
			if (floatDCS != NULL){
				grpObj->SetL3Current(floatDCS);
			}
			else{
				outOfRange = kTRUE;
			}	
			if (floatDCS){
				delete[] floatDCS;
				floatDCS = 0x0;
			}
		}
		if (!outOfRange) {
			nL3Entries++;
			ffailedDPs->RemoveAt(indexDP);
		}
	}

	if (array) array = 0x0;

	AliInfo(Form("==========L3Polarity==========="));
	indexDP = kL3Polarity;
	array = (TObjArray *)valueMap->GetValue(fgkDCSDataPoints[indexDP]);
	if(!array) {
		Log(Form("%s not found in the map!!!",fgkDCSDataPoints[indexDP]));
	} 
	else {
		if (array->GetEntries() == 0){
			AliError(Form("No entries found in array! setting %s Polarity to invalid...",fgkDCSDataPoints[indexDP]));
		}
		else {
			Bool_t change = kFALSE;
			Char_t charDCS = ProcessBool(array,change);
			if (change == kFALSE){
				grpObj->SetL3Polarity(charDCS);
				AliInfo(Form("%s set to %d",fgkDCSDataPoints[indexDP],(Int_t)(grpObj->GetL3Polarity())));
				ffailedDPs->RemoveAt(indexDP);
				nL3Entries++;
			}
			else if (isZero){
				AliInfo(Form("%s set to invalid, but magnet was OFF (according to the current), DP not considered wrong",fgkDCSDataPoints[indexDP]));
				ffailedDPs->RemoveAt(indexDP);
				nL3Entries++;
			}
			else {
				AliError(Form("%s value changed within the run, while the magnet was ON (according to the current), setting it to invalid and considering the DP as wrong",fgkDCSDataPoints[indexDP]));
			}
		}
	}

	return nL3Entries;

}
//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessDipoleDPs(const TMap* valueMap, AliGRPObject* grpObj)
{
	// processing DPs
	// related to 
	// the Dipole info

	Int_t nDipoleEntries = 0;
	TObjArray *array = 0x0;
	Int_t indexDP = -1;
	Bool_t isZero = kTRUE; // flag to monitor L3Current. If set to true, the magnet is OFF, and the polarity can change

	AliInfo(Form("==========DipoleCurrent==========="));
	Bool_t outOfRange = kFALSE;  // flag to monitor if any value collected by DCS is out of range
	indexDP = kDipoleCurrent;
	array = (TObjArray *)valueMap->GetValue(fgkDCSDataPoints[indexDP]);
	if(!array) {
		Log(Form("%s not found in the map!!!",fgkDCSDataPoints[indexDP]));
	} 
	else {
		if (array->GetEntries() == 0){
			AliError(Form("No entries found in array! setting %s to invalid...",fgkDCSDataPoints[indexDP]));
		}
		else {
			Float_t *floatDCS = ProcessFloatAllMagnet(array, indexDP, isZero);
			if (floatDCS != NULL){
				grpObj->SetDipoleCurrent(floatDCS);
			} 
			else{
				outOfRange=kTRUE;
			}
			if (floatDCS){
				delete[] floatDCS;
				floatDCS = 0x0;
			}
		}
		if (!outOfRange) {
			nDipoleEntries++;
			ffailedDPs->RemoveAt(indexDP);
		}
	}

	if (array) array = 0x0;

	AliInfo(Form("==========DipolePolarity==========="));
	indexDP = kDipolePolarity;
	array = (TObjArray *)valueMap->GetValue(fgkDCSDataPoints[indexDP]);
	if(!array) {
		Log(Form("%s not found in the map!!!",fgkDCSDataPoints[indexDP]));
	} 
	else {
		if (array->GetEntries() == 0){
			AliError(Form("No entries found in array! setting %s to invalid...",fgkDCSDataPoints[indexDP]));
		}
		else {
			Bool_t change = kFALSE;
			Char_t charDCS = ProcessBool(array,change);
			if (!change){
				grpObj->SetDipolePolarity(charDCS);
				AliInfo(Form("%s set to %d",fgkDCSDataPoints[indexDP],(Int_t)(grpObj->GetDipolePolarity())));
				ffailedDPs->RemoveAt(indexDP);
				nDipoleEntries++;
			}
			else if (isZero){
				AliInfo(Form("%s set to invalid, but magnet was OFF (according to the current), DP not considered wrong",fgkDCSDataPoints[indexDP]));
				ffailedDPs->RemoveAt(indexDP);
				nDipoleEntries++;
			}
			else{
				AliError(Form("%s value changed within the run while the magnet was ON (according to the current), setting it to invalid and considering the DP as wrong", fgkDCSDataPoints[indexDP]));
			}
		}
	}

	return nDipoleEntries;

}
//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessEnvDPs(TMap* valueMap, AliGRPObject* grpObj)
{
	// processing DPs
	// related to 
	// evironment conditions (temperature, pressure) info

	Int_t nEnvEntries = 0;
	TObjArray *array = 0x0;
	Int_t indexDP = -1;

	AliInfo(Form("==========CavernTemperature==========="));
	Bool_t outOfRange = kFALSE;  // flag to monitor if any value collected by DCS is out of range
	indexDP = kCavernTemperature;
	array = (TObjArray *)valueMap->GetValue(fgkDCSDataPoints[indexDP]);
	if(!array) {
		Log(Form("%s not found in the map!!!",fgkDCSDataPoints[indexDP]));
	} 
	else {
		if (array->GetEntries() == 0){
			AliError(Form("No entries found in array! setting %s to invalid...",fgkDCSDataPoints[indexDP]));
		}
		else {
			Float_t *floatDCS = ProcessFloatAll(array);
			if (floatDCS != NULL){
				grpObj->SetCavernTemperature(floatDCS);
			}
			else{
				outOfRange = kTRUE;
			}
			if (floatDCS){
				delete[] floatDCS;
				floatDCS = 0x0;
			}
		}
		if (!outOfRange) {
			ffailedDPs->RemoveAt(indexDP);
			nEnvEntries++;
		}
	}

	if (array) array = 0x0;

	AliInfo(Form("========== AtmosPressures (Cavern + Surface + Cavern2) ==========="));
	AliDCSSensorArray *dcsSensorArray = GetPressureMap(valueMap);
	//dcsSensorArray->Print();
	if( fPressure->NumFits()<kNumSensors ) {
		Log(Form("Check the pressure sensor values! Not all the %d pressure sensors have been fit",kNumSensors));
	} 
	Log(Form("Number of fits performed = %d",fPressure->NumFits()));

	AliInfo(Form("==========CavernAtmosPressure==========="));
	indexDP = kCavernAtmosPressure;
	AliDCSSensor* sensorCavernP2 = dcsSensorArray->GetSensor(fgkDCSDataPoints[indexDP]);
	TGraph* graph = sensorCavernP2->GetGraph();
	AliDebug(3,Form("index = %d",indexDP));
	AliDebug(3,Form("name = %s",fgkDCSDataPoints[indexDP]));
	AliDebug(2,Form("graph = %p",graph));
	AliDebug(3,Form("sensorCavernP2 = %p", sensorCavernP2));
	if(sensorCavernP2->GetFit() || graph) {
		if (sensorCavernP2->GetFit()){
			Log(Form("Fit for sensor %s found",fgkDCSDataPoints[indexDP]));
		}
		else {
			Log(Form("Fit for sensor %s not found, but the graph is there - NOT going into error",fgkDCSDataPoints[indexDP]));
		}
		grpObj->SetCavernAtmosPressure(sensorCavernP2);
		ffailedDPs->RemoveAt(indexDP);
		nEnvEntries++;
	} 
	//if (sensorP2) delete sensorP2;
	else {
		Log(Form("ERROR!!! Neither graph nor fit found for sensor %s - this will not increase the number of found DCS DPs and will cause an error", fgkDCSDataPoints[indexDP] ));
	}
	
	AliInfo(Form("==========SurfaceAtmosPressure==========="));
	indexDP = kSurfaceAtmosPressure;
	AliDCSSensor* sensorP2 = dcsSensorArray->GetSensor(fgkDCSDataPoints[indexDP]);
	graph = sensorP2->GetGraph();
	AliDebug(3,Form("index = %d",indexDP));
	AliDebug(3,Form("name = %s",fgkDCSDataPoints[indexDP]));
	AliDebug(2,Form("graph = %p",graph));	
	AliDebug(3,Form("sensorP2 = %p", sensorP2));
	if(sensorP2->GetFit() || graph) {
		if (sensorP2->GetFit()){
			Log(Form("Fit for sensor %s found",fgkDCSDataPoints[indexDP]));
		}
		else {
			Log(Form("Fit for sensor %s not found, but the graph is there - NOT going into error",fgkDCSDataPoints[indexDP]));
		}
		grpObj->SetSurfaceAtmosPressure(sensorP2);
		ffailedDPs->RemoveAt(indexDP);
		nEnvEntries++;
	} 
	//if (sensorP2) delete sensorP2;
	else {
		Log(Form("ERROR!!! Neither graph nor fit found for sensor %s - this will not increase the number of found DCS DPs and will cause an error", fgkDCSDataPoints[indexDP] ));
	}

	AliInfo(Form("==========CavernAtmosPressure2==========="));
	indexDP = kCavernAtmosPressure2;
	AliDCSSensor* sensorCavernP22 = dcsSensorArray->GetSensor(fgkDCSDataPoints[indexDP]);
	graph = sensorCavernP22->GetGraph();
	AliDebug(3,Form("index = %d",indexDP));
	AliDebug(3,Form("name = %s",fgkDCSDataPoints[indexDP]));
	AliDebug(2,Form("graph = %p",graph));	
	AliDebug(3,Form("sensorCavernP2_2 = %p", sensorCavernP22));
	if(sensorCavernP22->GetFit() || graph) {
		if (sensorCavernP22->GetFit()){
			Log(Form("Fit for sensor %s found",fgkDCSDataPoints[indexDP]));
		}
		else {
			Log(Form("Fit for sensor %s not found, but the graph is there - NOT going into error",fgkDCSDataPoints[indexDP]));
		}
		grpObj->SetCavernAtmosPressure2(sensorCavernP22);
		ffailedDPs->RemoveAt(indexDP);
		nEnvEntries++;
	} 
	//if (sensorP2) delete sensorP2;
	else {
		Log(Form("ERROR!!! Neither graph nor fit found for sensor %s - this will not increase the number of found DCS DPs and will cause an error", fgkDCSDataPoints[indexDP] ));
	}
	
	
	return nEnvEntries;
}
//_______________________________________________________________

Int_t AliGRPPreprocessor::ProcessHPDPs(const TMap* valueMap, AliGRPObject* grpObj)
{
	// processing DPs
	// related to 
	// Hall Probes info

	Int_t nHPEntries = 0;
	TObjArray *array = 0x0;
	Int_t indexDP = -1;
	Bool_t outOfRange; // flag to monitor if any value collected by DCS is out of range

	if (fgknDCSDPHallProbes != AliGRPObject::GetNumberOfHP()){
		AliError(Form("Number of Hall probes expected in GRP Preprocessor (i.e. %d) different from number of Hall Probes foreseen in GRP object (i.e. %d). Looping on entries from GRP object anyway.", fgknDCSDPHallProbes, AliGRPObject::GetNumberOfHP()));
	}
	for (indexDP = 0; indexDP < AliGRPObject::GetNumberOfHP(); indexDP++){
		outOfRange = kFALSE; // resetting outOfRange flag at each HP
		AliInfo(Form("==========%s===========",AliGRPObject::GetHPDP(indexDP)));
		array = (TObjArray *)valueMap->GetValue(AliGRPObject::GetHPDP(indexDP));
		if(!array) {
			Log(Form("%s not found in the map!!!",AliGRPObject::GetHPDP(indexDP)));
		} 
		else {
			if (array->GetEntries() == 0){
				AliError(Form("No entries found in array! setting %s to invalid...",AliGRPObject::GetHPDP(indexDP)));
			}
			else {
				Float_t *floatDCS = ProcessFloatAll(array);
				if (floatDCS != NULL){
					AliDebug(2,Form("value[0] = %f, value[1] = %f, value[2] = %f, value[3] = %f, value[4] = %f",floatDCS[0],floatDCS[1],floatDCS[2],floatDCS[3],floatDCS[4])); 
					grpObj->SetHallProbes((AliGRPObject::DP_HallProbes)indexDP,floatDCS);
					for (Int_t kk = 0 ; kk< 5; kk++){
						AliDebug(2,Form("HallProbe[%d][%d]=%f",indexDP,kk,grpObj->GetHallProbes((AliGRPObject::DP_HallProbes)indexDP,(AliGRPObject::Stats)kk)));
					}
				}
				else{
					outOfRange = kTRUE;
				}
				if (floatDCS){
					delete[] floatDCS;
					floatDCS = 0x0;
				}
			}
			if (!outOfRange) {
				ffailedDPs->RemoveAt(indexDP + fgkDCSDPHallTopShift);  // 7 = shift in the complete list of DPs to get to the Hall Probes
				nHPEntries++;
			}
		}
	}
		
	Log(Form("Hall Probes = %d ", nHPEntries));
	return nHPEntries;
}

//_________________________________________________________________________

AliSplineFit* AliGRPPreprocessor::GetSplineFit(const TObjArray *array, const TString& stringID){


	// 
	// returning Spline Fit 
	// 

	Int_t entriesarray = array->GetEntries();
	Float_t* value = new Float_t[entriesarray];
	Float_t* time = new Float_t[entriesarray];
	AliDCSValue* v = 0x0;
	for (Int_t iarray = 0; iarray < entriesarray; iarray++){
		v = (AliDCSValue*)array->At(iarray);
		value[iarray] = v->GetFloat();
		time[iarray] = v->GetTimeStamp();
		AliDebug(2,Form("iarray = %d, value = %f, time = %f",iarray,value[iarray],time[iarray]));
	}
	TGraph* gr = new TGraph(entriesarray,value,time);
	if (!gr ) {
		AliWarning(Form("%s: no input graph to compute SplineFit",stringID.Data()));
		return NULL;
	}
	AliSplineFit *fit = new AliSplineFit();
	fit->SetMinPoints(10);
	fit->InitKnots(gr,10,10,0.0);
	fit->SplineFit(2);
	fit->Cleanup();
	if (!fit) {
		AliWarning(Form("%s: no fit performed",stringID.Data()));
		return NULL;
	} 
	return fit;
}

//_________________________________________________________________________

TString AliGRPPreprocessor::ProcessChar(const TObjArray *array)
{

	// 
	// processing char
	//

	TString aDCSString="";
	
	AliDCSValue *v = 0x0;
	for(Int_t iCount = 0; iCount < array->GetEntries(); iCount++) {
		v = (AliDCSValue *)array->At(iCount);
		if (((Int_t)(v->GetTimeStamp()) < (Int_t)GetStartTimeDCSQuery()) || ((Int_t)(v->GetTimeStamp()) > (Int_t)GetEndTimeDCSQuery())) {
			AliError(Form("DCS values for the parameter outside the queried interval"));
			continue;
		}
		if (iCount > 0) {
			if (aDCSString != v->GetChar())
			AliError(Form("DCS values for the parameter changed from %s to %c within the queried interval", aDCSString.Data(), (Char_t)v->GetChar()));
		}
		aDCSString = (TString)v->GetChar();  // keeping always last value in the array
	}
	return aDCSString;
}

//__________________________________________________________________________________________________________________

Float_t* AliGRPPreprocessor::ProcessFloatAll(const TObjArray* array)
{
	// 
	// processing Float values using Mean, Median, Standard Deviation wrt Mean, Standar Deviation wrt Median 
	//
	// parameters[0] = mean
	// parameters[1] = truncated mean (calculated excluding points outside +/- 3RMS from mean
	// parameters[2] = median
	// parameters[3] = standard deviation wrt mean
	// parameters[4] = standard deviation wrt median
	//

	TString timeStartString = (TString)GetRunParameter("DAQ_time_start");
	TString timeEndString = (TString)GetRunParameter("DAQ_time_end");
	if (timeStartString.IsNull() || timeStartString.IsNull()){
		if (timeStartString.IsNull()){ 
			AliError("DAQ_time_start not set in logbook! Setting statistical values for current DP to invalid");
		}
		else if (timeStartString.IsNull()){
			AliError("DAQ_time_end not set in logbook! Setting statistical values for current DP to invalid");
		}
		fdaqStartEndTimeOk = kFALSE;
		return 0;
	}  

	Int_t timeStart = (Int_t)(timeStartString.Atoi());
	Int_t timeEnd = (Int_t)(timeEndString.Atoi());
	Float_t* parameters = new Float_t[5];
	Int_t iCounts = 0;
	Int_t iCountsRun = 0;
	Int_t nCounts = array->GetEntries();
	Float_t valueBeforeSOR = 0;
	Float_t valueAfterEOR = 0;
	Int_t timestampBeforeSOR = -1;
	Int_t timestampAfterEOR = -1;
	Int_t ientrySOR = -1;
	Int_t ientryEOR = -1;
	Float_t* arrayValues = 0x0; 
	Double_t* arrayWeights = 0x0; 
	Bool_t truncMeanFlag = kTRUE;  // flag to indicate whether Truncated Mean should be calculated or not
	Bool_t sdFlag = kTRUE;  // flag to indicate whether SD (wrt Mean/Median) should be calculated or not

	for(Int_t i = 0; i < nCounts; i++) {
		AliDCSValue *v = (AliDCSValue *)array->At(i);
		if ((v->GetFloat() <= fminFloat) || (v->GetFloat() >= fmaxFloat)) {
			AliError(Form("Error! Float value found in DCS map at %d-th entry is OUT OF RANGE: value = %6.5e",i,v->GetFloat()));
			if (v->GetFloat() < fminFloat) AliInfo(Form("The value is smaller than %6.5e",fminFloat));
			if (v->GetFloat() > fmaxFloat) AliInfo(Form("The value is greater than %6.5e",fmaxFloat));
			return NULL;
		}
		if(((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) &&((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery())) {
			AliDebug(2,Form("%d-th entry = %f at timestamp %i",i,v->GetFloat(),v->GetTimeStamp()));
			iCounts += 1;
			// look for the last value before SOR and the first value before EOR
			if (((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) && (Int_t)(v->GetTimeStamp()) < timeStart) {
				timestampBeforeSOR = (Int_t)(v->GetTimeStamp());
				AliDebug(2,Form("timestamp of last value before SOR = %d, with DAQ_time_start = %d",timestampBeforeSOR,timeStart));
				valueBeforeSOR = v->GetFloat();
			}
			else if ((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery() && (Int_t)(v->GetTimeStamp()) > timeEnd && timestampAfterEOR == -1){
				timestampAfterEOR = (Int_t)(v->GetTimeStamp());
				valueAfterEOR = v->GetFloat();
				AliDebug(2,Form("timestamp of first value after EOR = %d, with DAQ_time_end = %d",timestampAfterEOR,timeEnd));
			}
			// check if there are DPs between DAQ_time_start and DAQ_time_end
			if(((Int_t)(v->GetTimeStamp()) >= timeStart) &&((Int_t)(v->GetTimeStamp()) <= timeEnd)) {
				if (ientrySOR == -1) ientrySOR = i;  // first entry after SOR
				if (ientryEOR < i) ientryEOR = i;  // last entry before EOR
				AliDebug(2,Form("entry between SOR and EOR"));
				iCountsRun += 1;
			}
		}
		else {
			AliError(Form("DCS values for the parameter outside the queried interval: timestamp = %d",v->GetTimeStamp()));
		}
	}

	if (timestampBeforeSOR == -1){
		AliWarning("No value found before SOR");
	}
	if (timestampAfterEOR == -1){
		AliWarning("No value found after EOR");
	}

	AliDebug(2,Form("Number of valid entries (within DCS query interval) = %i, from a total amount of %i entries",iCounts,nCounts));
	AliDebug(2,Form("Last value before DAQ_time_start (SOR) = %f at timestamp = %d",valueBeforeSOR,timestampBeforeSOR));
	AliDebug(2,Form("First value after DAQ_time_end (EOR)   = %f at timestamp = %d",valueAfterEOR,timestampAfterEOR));
	AliInfo(Form("Found %d entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)",iCountsRun));
	AliDebug(2,Form("Index of first entry after DAQ_time_start (SOR) = %d ",ientrySOR));
	AliDebug(2,Form("Index of first entry before DAQ_time_end (EOR) = %d ",ientryEOR));

	Int_t nentriesUsed = 0;
	if (iCountsRun > 1){
		AliInfo("Using entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)");
		AliDebug(2,"Calculating (weighted) Mean and Median");
		arrayValues = new Float_t[iCountsRun]; 
		arrayWeights = new Double_t[iCountsRun]; 
		nentriesUsed = iCountsRun;
		for (Int_t i = ientrySOR; i <= ientryEOR; i++){
			AliDCSValue *v = (AliDCSValue *)array->At(i);
			Int_t timestamp2 = 0;
			if (i < ientryEOR){
				AliDCSValue *v1 = (AliDCSValue *)array->At(i+1);
				timestamp2 = (Int_t)v1->GetTimeStamp();
			}
			else {
				timestamp2 = timeEnd+1;
			}
			arrayWeights[i-ientrySOR] = (Double_t)(timestamp2 - (Int_t)v->GetTimeStamp());
			arrayValues[i-ientrySOR] = v->GetFloat();
		}
		parameters[0] = TMath::Mean(iCountsRun,arrayValues,arrayWeights);
		parameters[2] = TMath::Median(iCountsRun,arrayValues,arrayWeights);
	}
	else if (iCountsRun == 1){
		AliDCSValue* v = (AliDCSValue *)array->At(ientrySOR);
		nentriesUsed = 2;
		if (timestampBeforeSOR != -1 && timestampBeforeSOR != (Int_t)v->GetTimeStamp()){
			AliWarning("Using single entry between DAQ_time_start (SOR) and DAQ_time_end (EOR) and last entry before SOR. Truncated mean won't be calculated.");
			arrayValues = new Float_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)((Int_t)v->GetTimeStamp()-timestampBeforeSOR);
			arrayValues[1] = v->GetFloat();
			arrayWeights[1] = (Double_t)(timeEnd+1-(Int_t)v->GetTimeStamp());
			AliDebug(2, Form("value0 = %f, with weight = %f",arrayValues[0],arrayWeights[0])); 
			AliDebug(2, Form("value1 = %f, with weight = %f",arrayValues[1],arrayWeights[1])); 
			parameters[0] = TMath::Mean(2,arrayValues,arrayWeights);
			parameters[2] = TMath::Median(2,arrayValues,arrayWeights);
			truncMeanFlag = kFALSE;
		}
		else{
			AliError("Cannot calculate mean, truncated mean, median, SD wrt mean, SD wrt median for current DP - only one value collected during the run, but no value before with which to calculate the statistical quantities");
			parameters[0] = AliGRPObject::GetInvalidFloat();
			parameters[1] = AliGRPObject::GetInvalidFloat();
			parameters[2] = AliGRPObject::GetInvalidFloat();
			parameters[3] = AliGRPObject::GetInvalidFloat();
			parameters[4] = AliGRPObject::GetInvalidFloat();
			return parameters;
		}
	}
	else { // iCountsRun == 0, using only the point immediately before SOR
		if (timestampBeforeSOR == -1){
			AliError("Cannot set mean, truncated mean, median, SD wrt mean, SD wrt median for current DP - no points during the run collected, and point before SOR missing");
			parameters[0] = AliGRPObject::GetInvalidFloat();
			parameters[1] = AliGRPObject::GetInvalidFloat();
			parameters[2] = AliGRPObject::GetInvalidFloat();
			parameters[3] = AliGRPObject::GetInvalidFloat();
			parameters[4] = AliGRPObject::GetInvalidFloat();
			return parameters;
		}
		else {
			AliWarning("Using only last entry before SOR. Truncated mean and Standard deviations (wrt mean/median) won't be calculated.");
			AliDebug(2,Form("value = %f",valueBeforeSOR)); 
			parameters[0] = valueBeforeSOR;
			parameters[2] = valueBeforeSOR;
			truncMeanFlag = kFALSE;
			sdFlag = kFALSE;
		}
	}

	Float_t temp = 0;
	Float_t temp1 = 0;
	Float_t sumweights = 0; 
	Int_t entriesTruncMean = 0;
	Float_t* arrayValuesTruncMean = new Float_t[nentriesUsed]; 
	Double_t* arrayWeightsTruncMean = new Double_t[nentriesUsed]; 

	// calculating SD wrt Mean and Median
	AliDebug(2,"Calculating SD wrt Mean and SD wrt Median");
	if (sdFlag){
		for (Int_t i =0; i< nentriesUsed; i++){
			AliDebug(2,Form("Entry %d: value = %f, weight = %f",i,arrayValues[i],arrayWeights[i]));
			temp += (arrayValues[i]-parameters[2])*(arrayValues[i]-parameters[2]);
			temp1 += arrayWeights[i]*(arrayValues[i]-parameters[0])*(arrayValues[i]-parameters[0]);
			sumweights += arrayWeights[i];
		}
		// setting SD wrt Mean 
		if (sumweights != 0 ){
			parameters[3] = TMath::Sqrt(temp1/sumweights);
		}
		else {
			AliError("Sum of weights to calculate Standard Deviation (wrt mean) <= 0, setting the SD to invalid");
			parameters[3] = AliGRPObject::GetInvalidFloat();
		}
		// setting SD wrt Median
		if (nentriesUsed != 0){
			parameters[4] = TMath::Sqrt(temp/nentriesUsed);
		}
		else{
			AliError("Number of entries used to calculate Standard Deviation (wrt median) <= 0, setting the SD to invalid");
			parameters[4] = AliGRPObject::GetInvalidFloat();
		}
	}
	else {
		parameters[3] = AliGRPObject::GetInvalidFloat();
		parameters[4] = AliGRPObject::GetInvalidFloat();
	}		

	// calculating truncated mean (this comes afterwards since you need the SD wrt Mean)
	if (truncMeanFlag){
		AliDebug(2,"Calculating Truncated Mean");
		for (Int_t i =0; i< nentriesUsed; i++){
			AliDebug(2,Form("Entry %d: value = %f, weight = %f",i,arrayValues[i],arrayWeights[i]));
			if ((arrayValues[i]<=parameters[0]+3*parameters[3]) && (arrayValues[i]>=parameters[0]-3*parameters[3])){
				arrayValuesTruncMean[entriesTruncMean]=arrayValues[i];
				arrayWeightsTruncMean[entriesTruncMean]=arrayWeights[i];
				AliDebug(2,Form("For Truncated Mean: Entry %d: value = %f, weight = %f",entriesTruncMean,arrayValuesTruncMean[entriesTruncMean],arrayWeightsTruncMean[entriesTruncMean]));
				entriesTruncMean++;			
			}
			else{
				AliDebug(2,"Discarding entry");
			}
		}
		// setting truncated mean 
		if (entriesTruncMean >1){
			AliDebug(2,Form("%d entries used for truncated mean",entriesTruncMean));
			parameters[1] = TMath::Mean(entriesTruncMean,arrayValuesTruncMean,arrayWeightsTruncMean);
		}
		else{	
			AliDebug(2,Form("Too few entries (%d) to calculate truncated mean",entriesTruncMean));
			parameters[1] = AliGRPObject::GetInvalidFloat();
		}
	}
	else{
			parameters[1] = AliGRPObject::GetInvalidFloat();
	}

	AliInfo(Form("(weighted) mean = %f ",parameters[0]));
	AliInfo(Form("(weighted) truncated mean = %f ",parameters[1]));
	AliInfo(Form("median = %f ",parameters[2]));
	AliInfo(Form("(weighted) standard deviation with (weighted) mean = %f ",parameters[3]));
	AliInfo(Form("standard deviation with median = %f ",parameters[4]));
	
	return parameters;
}

//__________________________________________________________________________________________________________________

Float_t* AliGRPPreprocessor::ProcessFloatAllMagnet(const TObjArray* array, Int_t indexDP, Bool_t &isZero)
{
	// 
	// processing Float values using Mean, Median, Standard Deviation wrt Mean, Standar Deviation wrt Median 
	// used for L3 and Dipole magnets, using isZero flag to decide whther the magnet was OFF/ON
	// the flag is set according to the L3/Dipole current value
	// current threshold for L3 = 350 A (value provided by DCS)
	// current threshold for Dipole = 450 A (value provided by DCS)
	//
	// parameters[0] = mean
	// parameters[1] = truncated mean (calculated excluding points outside +/- 3RMS from mean
	// parameters[2] = median
	// parameters[3] = standard deviation wrt mean
	// parameters[4] = standard deviation wrt median
	//

	AliInfo(Form("indexDP = %d",indexDP)); 

	Int_t nCounts = array->GetEntries();
	for(Int_t i = 0; i < nCounts; i++) {
		AliDCSValue *v = (AliDCSValue *)array->At(i);
		if ((v->GetFloat() <= fminFloat) || (v->GetFloat() >= fmaxFloat)) {
			AliError(Form("Error! Float value found in DCS map at %d-th entry is OUT OF RANGE: value = %6.5e",i,v->GetFloat()));
			if (v->GetFloat() < fminFloat) AliInfo(Form("The value is smaller than %6.5e",fminFloat));
			if (v->GetFloat() > fmaxFloat) AliInfo(Form("The value is greater than %6.5e",fmaxFloat));
			return NULL;
		}
		if(((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) &&((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery())) {
			AliDebug(2,Form("%d-th entry = %f",i,v->GetFloat()));
			if (indexDP == kL3Current && v->GetFloat() > 350 && isZero == kTRUE) isZero=kFALSE; 
			if (indexDP == kDipoleCurrent && v->GetFloat() > 450 && isZero == kTRUE) isZero=kFALSE; 
		}
		else {
			AliError(Form("DCS values for the parameter outside the queried interval"));
		}
	}

	return ProcessFloatAll(array);
}


//_______________________________________________________________

Char_t AliGRPPreprocessor::ProcessBool(const TObjArray* array, Bool_t &change)
{
	// 
	// processing Boolean values
	//

	Bool_t aDCSBool = kTRUE;

	AliDCSValue *v = 0x0;

	for(Int_t iCount = 0; iCount < array->GetEntries(); iCount++) {
		v = (AliDCSValue *)array->At(iCount);
		if (((Int_t)(v->GetTimeStamp()) < (Int_t)GetStartTimeDCSQuery()) || ((Int_t)(v->GetTimeStamp()) > (Int_t)GetEndTimeDCSQuery())) {
			AliError(Form("DCS values for the parameter outside the queried interval"));
			continue;
		}
		if (iCount > 0) {
			if (aDCSBool != v->GetBool()) {
				AliError(Form("DCS values for the parameter changed from %d to %d within the queried interval", (UInt_t)aDCSBool, (UInt_t)v->GetBool()));
				change = kTRUE;
			}
		}
		aDCSBool = v->GetBool(); // always keeping last value
		AliDebug(2,Form("Bool = %d",(Int_t)aDCSBool));
	}
	
	Char_t caDCSBool = (Char_t) aDCSBool;
	return caDCSBool;
	
}

//_______________________________________________________________

Float_t AliGRPPreprocessor::ProcessInt(const TObjArray* array)
{
	// 
	// processing Int values, returning mean
	// AliGRPObject::GetInvalidFloat() is returned if any of the DCS values
	// are outside the queried time interval or their value is out of range
	//

	TString timeStartString = (TString)GetRunParameter("DAQ_time_start");
	TString timeEndString = (TString)GetRunParameter("DAQ_time_end");
	if (timeStartString.IsNull() || timeStartString.IsNull()){
		if (timeStartString.IsNull()){ 
			AliError("DAQ_time_start not set in logbook! Setting statistical values for current DP to invalid");
		}
		else if (timeStartString.IsNull()){
			AliError("DAQ_time_end not set in logbook! Setting statistical values for current DP to invalid");
		}
		return 0;
	}  

	Int_t timeStart = (Int_t)(timeStartString.Atoi());
	Int_t timeEnd = (Int_t)(timeEndString.Atoi());
	Float_t aDCSArrayMean = 0.0;
	Int_t iCounts = 0;
	Float_t valueBeforeSOR = 0;
	Float_t valueAfterEOR = 0;
	Int_t timestampBeforeSOR = -1;
	Int_t timestampAfterEOR = -1;
	Int_t ientrySOR = -1;
	Int_t ientryEOR = -1;
	Float_t* arrayValues = 0x0; 
	Double_t* arrayWeights = 0x0; 
	Int_t iCountsRun = 0;
	Int_t nCounts = array->GetEntries();

	for(Int_t i = 0; i < nCounts; i++) {
		AliDCSValue* v = (AliDCSValue *)array->At(i);
		if ((v->GetInt() < fminInt) || (v->GetInt() > fmaxInt)) {
			AliError(Form("Error! Int value found in DCS map at %d-th entry is OUT OF RANGE: value = %d",i, v->GetInt()));
			return AliGRPObject::GetInvalidFloat();
		}
		if(((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) &&((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery())) {
			AliDebug(2,Form("%d-th entry = %d at timestamp %i",i,v->GetInt(),v->GetTimeStamp()));
			iCounts += 1;
			// look for the last value before SOR and the first value before EOR
			if (((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) && (Int_t)(v->GetTimeStamp()) < timeStart) {
				timestampBeforeSOR = (Int_t)(v->GetTimeStamp());
				AliDebug(2,Form("timestamp of last entry before SOR = %d, with DAQ_time_start = %d",timestampBeforeSOR,timeStart));
				valueBeforeSOR = (Float_t) v->GetInt();
			}
			else if ((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery() && (Int_t)(v->GetTimeStamp()) > timeEnd && timestampAfterEOR == -1){
				timestampAfterEOR = (Int_t)(v->GetTimeStamp());
				valueAfterEOR = (Float_t) v->GetInt();
				AliDebug(2,Form("timestamp of first entry after EOR = %d, with DAQ_time_end = %d",timestampAfterEOR,timeEnd));
			}
			// check if there are DPs between DAQ_time_start and DAQ_time_end
			if(((Int_t)(v->GetTimeStamp()) >= timeStart) &&((Int_t)(v->GetTimeStamp()) <= timeEnd)) {
				if (ientrySOR == -1) ientrySOR = i;  // first entry after SOR
				if (ientryEOR < i) ientryEOR = i;  // last entry before EOR
				AliDebug(2,Form("entry between SOR and EOR"));
				iCountsRun += 1;
			}
		}
		else {
			AliError(Form("DCS values for the parameter outside the queried interval: timestamp = %d",v->GetTimeStamp()));
		}
	}

	if (timestampBeforeSOR == -1){
		AliWarning("No value found before SOR!");
	}
	if (timestampAfterEOR == -1){
		AliWarning("No value found after EOR!");
	}

	AliDebug(2,Form("Number of valid entries (within query interval) = %i, starting from %i entries",iCounts,nCounts));
	AliDebug(2,Form("Last value before DAQ_time_start (SOR) = %f at timestamp = %d",valueBeforeSOR,timestampBeforeSOR));
	AliDebug(2,Form("First value after DAQ_time_end (EOR)   = %f at timestamp = %d",valueAfterEOR,timestampAfterEOR));
	AliInfo(Form("Found %d entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)",iCountsRun));
	AliDebug(2,Form("Index of first entry after DAQ_time_start (SOR) = %d ",ientrySOR));
	AliDebug(2,Form("Index of first entry before DAQ_time_end (EOR) = %d ",ientryEOR));

	Int_t nentriesUsed = 0;
	if (iCountsRun > 1){
		AliInfo("Using entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)");
		AliDebug(2,"Calculating (weighted) Mean");
		arrayValues = new Float_t[iCountsRun]; 
		arrayWeights = new Double_t[iCountsRun]; 
		nentriesUsed = iCountsRun;
		for (Int_t i = ientrySOR; i <= ientryEOR; i++){
			AliDCSValue *v = (AliDCSValue *)array->At(i);
			Int_t timestamp2 = 0;
			if (i < ientryEOR){
				AliDCSValue *v1 = (AliDCSValue *)array->At(i+1);
				timestamp2 = (Int_t)v1->GetTimeStamp();
			}
			else {
				timestamp2 = timeEnd+1;
			}
			arrayWeights[i-ientrySOR] = (Double_t)(timestamp2 - (Int_t)v->GetTimeStamp());
			arrayValues[i-ientrySOR] = (Float_t)v->GetInt();
		}
		aDCSArrayMean = TMath::Mean(iCountsRun,arrayValues,arrayWeights);
	}
	else if (iCountsRun == 1){
		AliDCSValue* v = (AliDCSValue *)array->At(ientrySOR);
		nentriesUsed = 2;
		if (timestampBeforeSOR != -1 && timestampBeforeSOR != (Int_t)v->GetTimeStamp()){
			AliWarning("Using single entry between DAQ_time_start (SOR) and DAQ_time_end (EOR) and last entry before SOR.");
			arrayValues = new Float_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)((Int_t)v->GetTimeStamp()-timestampBeforeSOR);
			arrayValues[1] = (Float_t)v->GetInt();
			arrayWeights[1] = (Double_t)(timeEnd+1-(Int_t)v->GetTimeStamp());
			AliDebug(2,Form("value0 = %f, with weight = %f",arrayValues[0],arrayWeights[0])); 
			AliDebug(2,Form("value1 = %f, with weight = %f",arrayValues[1],arrayWeights[1])); 
			aDCSArrayMean = TMath::Mean(2,arrayValues,arrayWeights);
		}
		else{
			AliError("Cannot calculate mean - only one value collected during the run, but no value before with which to calculate the statistical quantities");
			return AliGRPObject::GetInvalidFloat();
		}
	}
	else { // iCountsRun == 0, using the point immediately before SOR and the one immediately after EOR
		if (timestampBeforeSOR == -1 || timestampAfterEOR == -1){
			if (timestampBeforeSOR == -1){
				AliError("Cannot calculate mean - no points during the run collected, and point before SOR missing");
			}
			if (timestampAfterEOR == -1){
				AliError("Cannot calculate maen - no points during the run collected, and point after EOR missing");
			}
			return AliGRPObject::GetInvalidFloat();
		}
		else {
			AliWarning("Using last entry before SOR and first entry after EOR.");
			nentriesUsed = 2;
			arrayValues = new Float_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)(timestampAfterEOR - timestampBeforeSOR);
			arrayValues[1] = valueAfterEOR;
			arrayWeights[1] = 1.;
			AliDebug(2,Form("value0 = %f, with weight = %f",arrayValues[0],arrayWeights[0])); 
			AliDebug(2,Form("value1 = %f, with weight = %f",arrayValues[1],arrayWeights[1])); 
			aDCSArrayMean = TMath::Mean(1,arrayValues,arrayWeights);
		}
	}

	AliInfo(Form("mean = %f ", aDCSArrayMean));
	return aDCSArrayMean;

}
//_______________________________________________________________

Float_t AliGRPPreprocessor::ProcessUInt(const TObjArray* array)
{
	// 
	// processing Int values, returning mean 
	// AliGRPObject::GetInvalidFloat() is returned if any of the DCS values
	// are outside the queried time interval or their value is out of range
	//

	TString timeStartString = (TString)GetRunParameter("DAQ_time_start");
	TString timeEndString = (TString)GetRunParameter("DAQ_time_end");
	if (timeStartString.IsNull() || timeStartString.IsNull()){
		if (timeStartString.IsNull()){ 
			AliError("DAQ_time_start not set in logbook! Setting statistical values for current DP to invalid");
		}
		else if (timeStartString.IsNull()){
			AliError("DAQ_time_end not set in logbook! Setting statistical values for current DP to invalid");
		}
		return 0;
	}  

	Int_t timeStart = (Int_t)(timeStartString.Atoi());
	Int_t timeEnd = (Int_t)(timeEndString.Atoi());
	Float_t aDCSArrayMean = 0.0;
	Int_t iCounts = 0;
	Float_t valueBeforeSOR = 0;
	Float_t valueAfterEOR = 0;
	Int_t timestampBeforeSOR = -1;
	Int_t timestampAfterEOR = -1;
	Int_t ientrySOR = -1;
	Int_t ientryEOR = -1;
	Float_t* arrayValues = 0x0; 
	Double_t* arrayWeights = 0x0; 
	Int_t iCountsRun = 0;
	Int_t nCounts = array->GetEntries();

	for(Int_t i = 0; i < nCounts; i++) {
		AliDCSValue* v = (AliDCSValue *)array->At(i);
		if ((v->GetUInt() < fminUInt) || (v->GetUInt() > fmaxUInt)) {
			AliError(Form("Error! UInt value found in DCS map at %d-th entry is OUT OF RANGE: value = %u",i,v->GetUInt()));
			return AliGRPObject::GetInvalidFloat();
		}
		if(((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) &&((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery())) {
			AliDebug(2,Form("%d-th entry = %d at timestamp %i",i,v->GetUInt(),v->GetTimeStamp()));
			iCounts += 1;
			// look for the last value before SOR and the first value before EOR
			if (((Int_t)(v->GetTimeStamp()) >= (Int_t)GetStartTimeDCSQuery()) && (Int_t)(v->GetTimeStamp()) < timeStart) {
				timestampBeforeSOR = (Int_t)(v->GetTimeStamp());
				AliDebug(2,Form("timestamp of last entry before SOR = %d, with DAQ_time_start = %d",timestampBeforeSOR,timeStart));
				valueBeforeSOR = (Float_t)v->GetUInt();
			}
			else if ((Int_t)(v->GetTimeStamp()) <= (Int_t)GetEndTimeDCSQuery() && (Int_t)(v->GetTimeStamp()) > timeEnd && timestampAfterEOR == -1){
				timestampAfterEOR = (Int_t)(v->GetTimeStamp());
				valueAfterEOR = (Float_t)v->GetUInt();
				AliDebug(2,Form("timestamp of first entry after EOR = %d, with DAQ_time_end = %d",timestampAfterEOR,timeEnd));
			}
			// check if there are DPs between DAQ_time_start and DAQ_time_end
			if(((Int_t)(v->GetTimeStamp()) >= timeStart) &&((Int_t)(v->GetTimeStamp()) <= timeEnd)) {
				if (ientrySOR == -1) ientrySOR = i;  // first entry after SOR
				if (ientryEOR < i) ientryEOR = i;  // last entry before EOR
				AliDebug(2,Form("entry between SOR and EOR"));
				iCountsRun += 1;
			}
		}
		else {
			AliError(Form("DCS values for the parameter outside the queried interval: timestamp = %d",v->GetTimeStamp()));
		}
	}

	if (timestampBeforeSOR == -1){
		AliWarning("No value found before SOR!");
	}
	if (timestampAfterEOR == -1){
		AliWarning("No value found after EOR!");
	}

	AliDebug(2,Form("Number of valid entries (within query interval) = %i, starting from %i entries",iCounts,nCounts));
	AliDebug(2,Form("Last value before DAQ_time_start (SOR) = %f at timestamp = %d",valueBeforeSOR,timestampBeforeSOR));
	AliDebug(2,Form("First value after DAQ_time_end (EOR)   = %f at timestamp = %d",valueAfterEOR,timestampAfterEOR));
	AliInfo(Form("Found %d entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)",iCountsRun));
	AliDebug(2,Form("Index of first entry after DAQ_time_start (SOR) = %d ",ientrySOR));
	AliDebug(2,Form("Index of first entry before DAQ_time_end (EOR) = %d ",ientryEOR));

	Int_t nentriesUsed = 0;
	if (iCountsRun > 1){
		AliInfo("Using entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)");
		AliDebug(2,"Calculating (weighted) Mean");
		arrayValues = new Float_t[iCountsRun]; 
		arrayWeights = new Double_t[iCountsRun]; 
		nentriesUsed = iCountsRun;
		for (Int_t i = ientrySOR; i <= ientryEOR; i++){
			AliDCSValue *v = (AliDCSValue *)array->At(i);
			Int_t timestamp2 = 0;
			if (i < ientryEOR){
				AliDCSValue *v1 = (AliDCSValue *)array->At(i+1);
				timestamp2 = (Int_t)v1->GetTimeStamp();
			}
			else {
				timestamp2 = timeEnd+1;
			}
			arrayWeights[i-ientrySOR] = (Double_t)(timestamp2 - (Int_t)v->GetTimeStamp());
			arrayValues[i-ientrySOR] = (Float_t)v->GetUInt();
		}
		aDCSArrayMean = TMath::Mean(iCountsRun,arrayValues,arrayWeights);
	}
	else if (iCountsRun == 1){
		AliDCSValue* v = (AliDCSValue *)array->At(ientrySOR);
		nentriesUsed = 2;
		if (timestampBeforeSOR != -1 && timestampBeforeSOR != (Int_t)v->GetTimeStamp()){
			AliWarning("Using single entry between DAQ_time_start (SOR) and DAQ_time_end (EOR) and last entry before SOR.");
			arrayValues = new Float_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)((Int_t)v->GetTimeStamp()-timestampBeforeSOR);
			arrayValues[1] = (Float_t)v->GetUInt();
			arrayWeights[1] = (Double_t)(timeEnd+1-(Int_t)v->GetTimeStamp());
			AliDebug(2,Form("value0 = %f, with weight = %f",arrayValues[0],arrayWeights[0])); 
			AliDebug(2,Form("value1 = %f, with weight = %f",arrayValues[1],arrayWeights[1])); 
			aDCSArrayMean = TMath::Mean(2,arrayValues,arrayWeights);
		}
		else{
			AliError("Cannot calculate mean - only one value collected during the run, but no value before with which to calculate the statistical quantities");
			return AliGRPObject::GetInvalidFloat();
		}
	}
	else { // iCountsRun == 0, using the point immediately before SOR and the one immediately after EOR
		if (timestampBeforeSOR == -1 || timestampAfterEOR == -1){
			if (timestampBeforeSOR == -1){
				AliError("Cannot calculate mean - no points during the run collected, and point before SOR missing");
			}
			if (timestampAfterEOR == -1){
				AliError("Cannot calculate maen - no points during the run collected, and point after EOR missing");
			}
			return AliGRPObject::GetInvalidFloat();
		}
		else {
			AliWarning("Using last entry before SOR and first entry after EOR.");
			nentriesUsed = 2;
			arrayValues = new Float_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)(timestampAfterEOR - timestampBeforeSOR);
			arrayValues[1] = valueAfterEOR;
			arrayWeights[1] = 1.;
			AliDebug(2,Form("value0 = %f, with weight = %f",arrayValues[0],arrayWeights[0])); 
			AliDebug(2,Form("value1 = %f, with weight = %f",arrayValues[1],arrayWeights[1])); 
			aDCSArrayMean = TMath::Mean(1,arrayValues,arrayWeights);
		}
	}

	AliInfo(Form("mean = %f ",aDCSArrayMean));
	return aDCSArrayMean;

}


//_______________________________________________________________

AliDCSSensorArray *AliGRPPreprocessor::GetPressureMap(TMap* dcsAliasMap)
{
	// extract DCS pressure maps. Perform fits to save space
	
	TMap *map = fPressure->ExtractDCS(dcsAliasMap);
	if (map) {
		AliDebug(2,Form("Map has %d entries",map->GetEntries()));
		fPressure->MakeSplineFit(map);
		Double_t fitFraction = fPressure->NumFits()/fPressure->NumSensors(); 
		if (fitFraction > kFitFraction ) {
			AliInfo(Form("Pressure values extracted, %d fits performed for %d sensors.", fPressure->NumFits(),fPressure->NumSensors()));
		} else { 
			AliInfo("Too few pressure maps fitted!!!");
		}
	} else {
		AliInfo("no atmospheric pressure map extracted!!!");
	}
	delete map;
	
	return fPressure;
}


  
//_______________________________________________________________
Int_t AliGRPPreprocessor::ReceivePromptRecoParameters(UInt_t run, const char* dbHost, Int_t dbPort, const char* dbName, const char* user, const char* password, const char *cdbRoot, TString &gdc)
{
	//
	// Retrieves logbook and trigger information from the online logbook
	// This information is needed for prompt reconstruction
	//
	// Parameters are:
	// Run number
	// DAQ params: dbHost, dbPort, dbName, user, password, logbookTable, triggerTable
	// cdbRoot
	//
	// returns:
	//         positive on success: the return code is the run number of last run processed of the same run type already processed by the SHUTTLE
	//         0 on success and no run was found
	//         negative on error
	//
	// This function is NOT called during the preprocessor run in the Shuttle!
	//
	
	// defaults
	if (dbPort == 0)
		dbPort = 3306;
	
	// CDB connection
	AliCDBManager* cdb = AliCDBManager::Instance();
	cdb->SetDefaultStorage(cdbRoot);
	
	// SQL connection
	TSQLServer* server = TSQLServer::Connect(Form("mysql://%s:%d/%s", dbHost, dbPort, dbName), user, password);
	
	if (!server)
		{
			Printf("ERROR: Could not connect to DAQ LB");
			return -1;
		}
	
	// main logbook
	TString sqlQuery;
	sqlQuery.Form("SELECT DAQ_time_start, run_type, detectorMask, L3_magnetCurrent, Dipole_magnetCurrent FROM logbook WHERE run = %d", run);
	TSQLResult* result = server->Query(sqlQuery);
	if (!result)
		{
			Printf("ERROR: Can't execute query <%s>!", sqlQuery.Data());
			return -2;
		}
	
	if (result->GetRowCount() == 0)
		{
			Printf("ERROR: Run %d not found", run);
			delete result;
			return -3;
		}
	
	TSQLRow* row = result->Next();
	if (!row)
		{
			Printf("ERROR: Could not receive data from run %d", run);
			delete result;
			return -4;
		}
	
	TString timeStartString(row->GetField(0));
	TString runType(row->GetField(1));
	TString detectorMaskString(row->GetField(2));
	TString l3CurrentString(row->GetField(3));
	TString dipoleCurrentString(row->GetField(4));
	time_t timeStart = (time_t)(timeStartString.Atoi());
	UInt_t detectorMask = (UInt_t)(detectorMaskString.Atoi());
	Float_t l3Current = (Float_t)(TMath::Abs(l3CurrentString.Atof()));
	Float_t dipoleCurrent = (Float_t)(TMath::Abs(dipoleCurrentString.Atof()));
	Char_t l3Polarity = (l3CurrentString.Atof() < 0) ? 1 : 0;
	Char_t dipolePolarity = (dipoleCurrentString.Atof() < 0) ? 1 : 0;
	
	AliGRPObject * grpObj = new AliGRPObject();
	grpObj->SetTimeStart(timeStart); 
	grpObj->SetRunType((TString)(row->GetField(1)));
	grpObj->SetDetectorMask(detectorMask);
	grpObj->SetL3Current(l3Current,(AliGRPObject::Stats)0);
	grpObj->SetDipoleCurrent(dipoleCurrent,(AliGRPObject::Stats)0);
	grpObj->SetL3Polarity(l3Polarity);
	grpObj->SetDipolePolarity(dipolePolarity);
	grpObj->SetPolarityConventionLHC();  // after the dipole cables swap we comply with LHC convention

	delete row;
	row = 0;
	
	delete result;
	result = 0;
	
	Printf("Storing GRP/GRP/Data object with the following content");
	grpObj->Dump();
	
	AliCDBMetaData metadata;
	metadata.SetResponsible("Jan Fiete Grosse-Oetringhaus & Chiara Zampolli");
	metadata.SetComment("GRP Output parameters received during online running");
	
	AliCDBId id("GRP/GRP/Data", run, run);
	Bool_t success = cdb->Put(grpObj, id, &metadata);
	
	delete grpObj;
	
	if (!success)
		{
			Printf("ERROR: Could not store GRP/GRP/Data into OCDB");
			return -5;
		}
	
	// Receive trigger information
	sqlQuery.Form("SELECT configFile FROM logbook_trigger_config WHERE run = %d", run);
	result = server->Query(sqlQuery);
	if (!result)
		{
			Printf("ERROR: Can't execute query <%s>!", sqlQuery.Data());
			return -11;
		}
	
	if (result->GetRowCount() == 0)
		{
			Printf("ERROR: Run %d not found in logbook_trigger_config", run);
			delete result;
			return -12;
		}
	
	row = result->Next();
	if (!row)
		{
			Printf("ERROR: Could not receive logbook_trigger_config data from run %d", run);
			delete result;
			return -13;
		}
	
	TString triggerConfig(row->GetField(0));
	
	delete row;
	row = 0;
	
	delete result;
	result = 0;
	
	Printf("Found trigger configuration: %s", triggerConfig.Data());
	
	AliTriggerConfiguration *runcfg = AliTriggerConfiguration::LoadConfigurationFromString(triggerConfig);
	if (!runcfg)
		{
			Printf("ERROR: Could not create CTP configuration object");
			return -14;
		}
	
	metadata.SetComment("CTP run configuration received during online running");
	
	AliCDBId id2("GRP/CTP/Config", run, run);
	success = cdb->Put(runcfg, id2, &metadata);
	
	delete runcfg;
	runcfg = 0;
	
	if (!success)
		{
			Printf("ERROR: Could not store GRP/CTP/Config into OCDB");
			return -15;
		}
	

	// Receive list of GDCs for this run
	sqlQuery.Form("SELECT GDC FROM logbook_stats_GDC WHERE run = %d", run);
	result = server->Query(sqlQuery);
	if (!result)
		{
			Printf("ERROR: Can't execute query <%s>!", sqlQuery.Data());
			return -24;
		}
	
	if (result->GetRowCount() == 0)
		{
			Printf("ERROR: Run %d not found in logbook_stats_GDC", run);
			delete result;
			return -25;
		}

	gdc = "";
	for (Int_t iGDC = 0; iGDC < result->GetRowCount(); iGDC++) {
	  row = result->Next();
	  if (!row)
	    {
	      Printf("ERROR: Could not receive logbook_stats_GDC data from run %d", run);
	      delete result;
	      return -26;
	    }
	  gdc += row->GetField(0);
	  gdc += " ";
	}

	delete row;
	row = 0;
	
	delete result;
	result = 0;
	
	Printf("Found GDC: %s", gdc.Data());

	// get last run with same run type that was already processed by the SHUTTLE
	
	sqlQuery.Form("SELECT max(logbook.run) FROM logbook LEFT JOIN logbook_shuttle ON logbook_shuttle.run = logbook.run WHERE run_type = '%s' AND shuttle_done = 1", runType.Data());
	result = server->Query(sqlQuery);
	if (!result)
		{
			Printf("ERROR: Can't execute query <%s>!", sqlQuery.Data());
			return -21;
		}
	
	if (result->GetRowCount() == 0)
		{
			Printf("ERROR: No result with query <%s>", sqlQuery.Data());
			delete result;
			return -22;
		}
	
	row = result->Next();
	if (!row)
		{
			Printf("ERROR: Could not receive data for query <%s>", sqlQuery.Data());
			delete result;
			return -23;
		}
	
	TString lastRunStr(row->GetField(0));
	Int_t lastRun = lastRunStr.Atoi();
	
	Printf("Last run with same run type %s is %d", runType.Data(), lastRun);
	
	delete row;
	row = 0;
	
	delete result;
	result = 0;
	
	server->Close();
	delete server;
	server = 0;
	
	return lastRun;
}
//-----------------------------------------------------------------
Double_t AliGRPPreprocessor::CalculateMean(TObjArray* array){

	//
	// Calculating mean over TObjArray from LHC Data
	//

	TString timeStartString = (TString)GetRunParameter("DAQ_time_start");
	TString timeEndString = (TString)GetRunParameter("DAQ_time_end");
	if (timeStartString.IsNull() || timeStartString.IsNull()){
		if (timeStartString.IsNull()){ 
			AliError("DAQ_time_start not set in logbook! Setting statistical values for current DP to invalid");
		}
		else if (timeStartString.IsNull()){
			AliError("DAQ_time_end not set in logbook! Setting statistical values for current DP to invalid");
		}
		return 0;
	}  

	Int_t timeStart = (Int_t)(timeStartString.Atoi());
	Int_t timeEnd = (Int_t)(timeEndString.Atoi());
	timeStart = 1260646960;
	timeEnd = 1260652740;
	Double_t* parameters = new Double_t[5];
	parameters[0] = -1.;
	parameters[1] = -1.;
	parameters[2] = -1.;
	parameters[3] = -1.;
	parameters[4] = -1.;
	Int_t iCounts = 0;
	Int_t iCountsRun = 0;
	Int_t nCounts = array->GetEntries();
	printf("ncounts = %d\n",nCounts);
	Double_t valueBeforeSOR = 0;
	Double_t valueAfterEOR = 0;
	Double_t timestampBeforeSOR = -1.;
	Double_t timestampAfterEOR = -1.;
	Int_t ientrySOR = -1;
	Int_t ientryEOR = -1;
	Double_t* arrayValues = 0x0; 
	Double_t* arrayWeights = 0x0; 
	Bool_t truncMeanFlag = kTRUE;  // flag to indicate whether Truncated Mean should be calculated or not
	Bool_t sdFlag = kTRUE;  // flag to indicate whether SD (wrt Mean/Median) should be calculated or not

	for(Int_t i = 0; i < nCounts; i++) {
		AliDCSArray *dcs = (AliDCSArray*)array->At(i);
		if((dcs->GetTimeStamp() >= timeStart) &&(dcs->GetTimeStamp() <= timeEnd)) {
			AliDebug(2,Form("%d-th entry = %f at timestamp %f\n",i,(Double_t)(dcs->GetInt(0)),dcs->GetTimeStamp()));
			iCounts += 1;
			// look for the last value before SOR and the first value before EOR
			if ((dcs->GetTimeStamp() >= timeStart) && (dcs->GetTimeStamp() < timeStart)) {
				timestampBeforeSOR = dcs->GetTimeStamp();
				AliDebug(2,Form("timestamp of last value before SOR = %f, with DAQ_time_start = %d\n",timestampBeforeSOR,timeStart));
				valueBeforeSOR = (Double_t)(dcs->GetInt(0));
			}
			else if ((dcs->GetTimeStamp() <= timeEnd) && (dcs->GetTimeStamp() > timeEnd) && timestampAfterEOR == -1){
				timestampAfterEOR = dcs->GetTimeStamp();
				valueAfterEOR = (Double_t)(dcs->GetInt(0));
				AliDebug(2,Form("timestamp of first value after EOR = %f, with DAQ_time_end = %d\n",timestampAfterEOR,timeEnd));
			}
			// check if there are DPs between DAQ_time_start and DAQ_time_end
			if((dcs->GetTimeStamp() >= timeStart) &&(dcs->GetTimeStamp() <= timeEnd)) {
				if (ientrySOR == -1) ientrySOR = i;  // first entry after SOR
				if (ientryEOR < i) ientryEOR = i;  // last entry before EOR
				AliDebug(2,Form("entry between SOR and EOR\n"));
				iCountsRun += 1;
			}
		}
		else {
			printf("DCS values for the parameter outside the queried interval: timestamp = %f\n",dcs->GetTimeStamp());
		}
	}

	if (timestampBeforeSOR == -1.){
		printf("No value found before SOR\n");
	}
	if (timestampAfterEOR == -1.){
		printf("No value found after EOR\n");
	}

	printf("Number of valid entries (within DCS query interval) = %i, from a total amount of %i entries\n",iCounts,nCounts);
	printf("Last value before DAQ_time_start (SOR) = %f at timestamp = %f\n",valueBeforeSOR,timestampBeforeSOR);
	printf("First value after DAQ_time_end (EOR)   = %f at timestamp = %f\n",valueAfterEOR,timestampAfterEOR);
	printf("Found %d entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)\n",iCountsRun);
	printf("Index of first entry after DAQ_time_start (SOR) = %d\n ",ientrySOR);
	printf("Index of first entry before DAQ_time_end (EOR) = %d\n ",ientryEOR);

	Int_t nentriesUsed = 0;
	if (iCountsRun > 1){
		printf("Using entries between DAQ_time_start (SOR) and DAQ_time_end (EOR)\n");
		printf("Calculating (weighted) Mean and Median\n" );
		arrayValues = new Double_t[iCountsRun]; 
		arrayWeights = new Double_t[iCountsRun]; 
		nentriesUsed = iCountsRun;
		for (Int_t i = ientrySOR; i <= ientryEOR; i++){
			AliDCSArray *dcs = (AliDCSArray *)array->At(i);
			Double_t timestamp2 = 0;
			if (i < ientryEOR){
				AliDCSArray *dcs1 = (AliDCSArray *)array->At(i+1);
				timestamp2 = dcs1->GetTimeStamp();
			}
			else {
				timestamp2 = (Double_t)timeEnd+1;
			}
			arrayWeights[i-ientrySOR] = (Double_t)((Double_t)timestamp2 - dcs->GetTimeStamp());
			arrayValues[i-ientrySOR] = (Double_t)(dcs->GetInt(0));
			printf("Entry %d: value = %f, weight = %f\n",i-ientrySOR,arrayValues[i-ientrySOR],arrayWeights[i-ientrySOR]);
		}
		parameters[0] = TMath::Mean(iCountsRun,arrayValues,arrayWeights);
		parameters[2] = TMath::Median(iCountsRun,arrayValues,arrayWeights);
	}
	else if (iCountsRun == 1){
		AliDCSArray* dcs = (AliDCSArray *)array->At(ientrySOR);
		nentriesUsed = 2;
		if (timestampBeforeSOR != -1 && timestampBeforeSOR != (Int_t)dcs->GetTimeStamp()){
			printf("Using single entry between DAQ_time_start (SOR) and DAQ_time_end (EOR) and last entry before SOR. Truncated mean won't be calculated.\n");
			arrayValues = new Double_t[2];
			arrayWeights = new Double_t[2];
			arrayValues[0] = valueBeforeSOR;
			arrayWeights[0] = (Double_t)(dcs->GetTimeStamp()-(Double_t)timestampBeforeSOR);
			arrayValues[1] = (Double_t)(dcs->GetInt(0));
			arrayWeights[1] = (Double_t)((Double_t)timeEnd+1-dcs->GetTimeStamp());
			printf("value0 = %f, with weight = %f\n",arrayValues[0],arrayWeights[0]); 
			printf("value1 = %f, with weight = %f\n",arrayValues[1],arrayWeights[1]); 
			parameters[0] = TMath::Mean(2,arrayValues,arrayWeights);
			parameters[2] = TMath::Median(2,arrayValues,arrayWeights);
			truncMeanFlag = kFALSE;
		}
		else{
			printf("Cannot calculate mean, truncated mean, median, SD wrt mean, SD wrt median for current DP - only one value collected during the run, but no value before with which to calculate the statistical quantities\n");
			parameters[0] = -1;
			parameters[1] = -1;
			parameters[2] = -1;
			parameters[3] = -1;
			parameters[4] = -1;
			return parameters[0];
		}
	}
	else { // iCountsRun == 0, using only the point immediately before SOR
		if (timestampBeforeSOR == -1.){
			printf("Cannot set mean, truncated mean, median, SD wrt mean, SD wrt median for current DP - no points during the run collected, and point before SOR missing\n");
			parameters[0] = -1;
			parameters[1] = -1;
			parameters[2] = -1;
			parameters[3] = -1;
			parameters[4] = -1;
			return parameters[0];
		}
		else {
			printf("Using only last entry before SOR. Truncated mean and Standard deviations (wrt mean/median) won't be calculated.\n");
			printf("value = %f\n",valueBeforeSOR); 
			parameters[0] = valueBeforeSOR;
			parameters[2] = valueBeforeSOR;
			truncMeanFlag = kFALSE;
			sdFlag = kFALSE;
		}
	}

	Double_t temp = 0;
	Double_t temp1 = 0;
	Double_t sumweights = 0; 
	Int_t entriesTruncMean = 0;
	Double_t* arrayValuesTruncMean = new Double_t[nentriesUsed]; 
	Double_t* arrayWeightsTruncMean = new Double_t[nentriesUsed]; 

	// calculating SD wrt Mean and Median
	printf("Calculating SD wrt Mean and SD wrt Median\n");
	if (sdFlag){
		for (Int_t i =0; i< nentriesUsed; i++){
			//printf("Entry %d: value = %f, weight = %f\n",i,arrayValues[i],arrayWeights[i]);
			temp += (arrayValues[i]-parameters[2])*(arrayValues[i]-parameters[2]);
			temp1 += arrayWeights[i]*(arrayValues[i]-parameters[0])*(arrayValues[i]-parameters[0]);
			sumweights += arrayWeights[i];
		}
		// setting SD wrt Mean 
		if (sumweights != 0 ){
			parameters[3] = TMath::Sqrt(temp1/sumweights);
		}
		else {
			printf("Sum of weights to calculate Standard Deviation (wrt mean) <= 0, setting the SD to invalid\n");
			parameters[3] = -1;
		}
		// setting SD wrt Median
		if (nentriesUsed != 0){
			parameters[4] = TMath::Sqrt(temp/nentriesUsed);
		}
		else{
			printf("Number of entries used to calculate Standard Deviation (wrt median) <= 0, setting the SD to invalid\n");
			parameters[4] = -1;
		}
	}
	else {
		parameters[3] = -1;
		parameters[4] = -1;
	}		

	// calculating truncated mean (this comes afterwards since you need the SD wrt Mean)
	if (truncMeanFlag){
		printf("Calculating Truncated Mean\n");
		for (Int_t i =0; i< nentriesUsed; i++){
			//printf("Entry %d: value = %f, weight = %f\n",i,arrayValues[i],arrayWeights[i]);
			if ((arrayValues[i]<=parameters[0]+3*parameters[3]) && (arrayValues[i]>=parameters[0]-3*parameters[3])){
				arrayValuesTruncMean[entriesTruncMean]=arrayValues[i];
				arrayWeightsTruncMean[entriesTruncMean]=arrayWeights[i];
				printf("For Truncated Mean: Entry %d: value = %f, weight = %f\n",entriesTruncMean,arrayValuesTruncMean[entriesTruncMean],arrayWeightsTruncMean[entriesTruncMean]);
				entriesTruncMean++;			
			}
			else{
				printf("Discarding entry\n");
			}
		}
		// setting truncated mean 
		if (entriesTruncMean >1){
			printf("%d entries used for truncated mean\n",entriesTruncMean);
			parameters[1] = TMath::Mean(entriesTruncMean,arrayValuesTruncMean,arrayWeightsTruncMean);
		}
		else{	
			printf("Too few entries (%d) to calculate truncated mean\n",entriesTruncMean);
			parameters[1] = -1;
		}
	}
	else{
			parameters[1] = -1;
	}
	
	printf("(weighted) mean = %f \n",parameters[0]);
	printf("(weighted) truncated mean = %f \n",parameters[1]);
	printf("median = %f \n",parameters[2]);
	printf("(weighted) standard deviation with (weighted) mean = %f \n",parameters[3]);
	printf("standard deviation with median = %f \n",parameters[4]);
	
	return (parameters[0]);
}
//------------------------------------------------------------------------------------------------------
Float_t AliGRPPreprocessor::ProcessEnergy(TObjArray* array, Double_t timeStart, Double_t timeEnd){

	//
	// Method to processo LHC Energy information
	// Only the first value is returned, provided that it is withing DAQ_time_start and DAQ_time_end
	//

	Int_t nCounts = array->GetEntries();
	Float_t energy = -1;
	Bool_t inRange = kFALSE;
	AliDebug(2,Form("Energy measurements = %d\n",nCounts));
	if (nCounts ==0){
		AliWarning("No Energy values found! Beam Energy remaining invalid!");
	}
	else{
		for(Int_t i = 0; i < nCounts; i++) {
			AliDCSArray *dcs = (AliDCSArray*)array->At(i);
			if((dcs->GetTimeStamp() >= timeStart) &&(dcs->GetTimeStamp() <= timeEnd)) {
				energy = (Float_t)(TMath::Nint(((Double_t)(dcs->GetInt(0)))*120/1000)); // sqrt(s)/2 energy in GeV
				AliInfo(Form("Energy value found = %d, converting --> sqrt(s)/2 = %f (GeV)", dcs->GetInt(0),energy));
				inRange = kTRUE;
				break;
			}
		}
		if (inRange == kFALSE){
				AliInfo("No Energy value found between DAQ_time_start and DAQ_time_end - energy will remain invalid!");
		}
	
	}

	return energy;
}

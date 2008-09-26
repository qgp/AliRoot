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

// Class containing the GRP data that have to be stored in the OCDB.
// Data come either from DAQ logbook or from DCS DB.
// Processing of the data can also be performed here.

#include "AliGRPObject.h"
#include "AliSplineFit.h"
#include "AliDCSSensor.h"
#include "AliLog.h"
#include <TObject.h>

ClassImp(AliGRPObject)
	
const Float_t AliGRPObject::fgkInvalidFloat = 0xffffffff; // value to identify invalid data - float
const TString AliGRPObject::fgkInvalidString = "";  // value to identify invalid data - string
const Char_t AliGRPObject::fgkInvalidChar = -1;         // value to identify invalid data - uchar
const Int_t AliGRPObject::fgkInvalidInt = -1;  // value to identify invalid data - uint
const Int_t AliGRPObject::fgkInvalidUInt = 0;  // value to identify invalid data - uint
const Int_t AliGRPObject::fgknDCSDP_HallProbes = 40;   // number of dcs dps
const char* AliGRPObject::fgkDCSDataPoints_HallProbes[AliGRPObject::fgknDCSDP_HallProbes] = {
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
                 };

//-----------------------------------------------------------------------------
AliGRPObject::AliGRPObject():
	TObject(),
	fPoints(5),
	fDimension(0),
	fTimeStart((time_t)fgkInvalidFloat),
	fTimeEnd((time_t)fgkInvalidFloat),
	fBeamEnergy(fgkInvalidFloat),
	fBeamType(fgkInvalidString),
	fNumberOfDetectors(fgkInvalidChar),
	fDetectorMask(fgkInvalidUInt),
	fLHCPeriod(fgkInvalidString),
	fRunType(fgkInvalidString),
	fLHCState(fgkInvalidString),
	fLHCLuminosity(new Float_t[fPoints]),
	fLHCLuminositySplineFit(0x0),
	fBeamIntensity(new Float_t[fPoints]),
	fBeamIntensitySplineFit(0x0),
	fL3Polarity(fgkInvalidChar),
	fDipolePolarity(fgkInvalidChar),
	fL3Current(new Float_t[fPoints]),
	fDipoleCurrent(new Float_t[fPoints]),
	fCavernTemperature(new Float_t[fPoints]),
	//fCavernAtmosPressure(new Float_t[fPoints]),
	fCavernAtmosPressure(0x0),
	fSurfaceAtmosPressure(0x0),
	fHallProbes(0x0)
{

	//
	// AliGRPObject default ctor
	//

	fDimension = fgknDCSDP_HallProbes*fPoints;
	fHallProbes = new Float_t[fDimension];

	for (Int_t nhp=0; nhp< fDimension; nhp++){
		fHallProbes[nhp] = fgkInvalidFloat;
	}

	for (Int_t i = 0; i < fPoints; i++){

		fLHCLuminosity[i] = fgkInvalidFloat;
		fBeamIntensity[i] = fgkInvalidFloat;
		fL3Current[i] = fgkInvalidFloat;
		fDipoleCurrent[i] = fgkInvalidFloat;
		fCavernTemperature[i] = fgkInvalidFloat;
		//		fCavernAtmosPressure[i] = fgkInvalidFloat;
	}
}

//-----------------------------------------------------------------------------

AliGRPObject::AliGRPObject(const AliGRPObject &obj):
	TObject(),
	fPoints(obj.fPoints),
	fDimension(obj.fDimension),
	fTimeStart(obj.fTimeStart),
	fTimeEnd(obj.fTimeEnd),
	fBeamEnergy(obj.fBeamEnergy),
	fBeamType(obj.fBeamType),
	fNumberOfDetectors(obj.fNumberOfDetectors),
	fDetectorMask(obj.fDetectorMask),
	fLHCPeriod(obj.fLHCPeriod),
	fRunType(obj.fRunType),
	fLHCState(obj.fLHCState),
	fLHCLuminosity(new Float_t[fPoints]),
	fLHCLuminositySplineFit(obj.fLHCLuminositySplineFit),
	fBeamIntensity(new Float_t[fPoints]),
	fBeamIntensitySplineFit(obj.fBeamIntensitySplineFit),
	fL3Polarity(obj.fL3Polarity),
	fDipolePolarity(obj.fDipolePolarity),
	fL3Current(new Float_t[fPoints]),
	fDipoleCurrent(new Float_t[fPoints]),
	fCavernTemperature(new Float_t[fPoints]),
	//fCavernAtmosPressure(new Float_t[fPoints]),
	fCavernAtmosPressure(obj.fCavernAtmosPressure),
	fSurfaceAtmosPressure(obj.fSurfaceAtmosPressure),
	fHallProbes(0x0)

{

	//
	// AliGRPObject copy ctor
	//

	for (Int_t nhp=0; nhp< fDimension; nhp++){
		fHallProbes[nhp] = obj.fHallProbes[nhp];
	}

	for (Int_t i = 0; i < fPoints; i++){

		fLHCLuminosity[i] = obj.fLHCLuminosity[i];
		fBeamIntensity[i] = obj.fBeamIntensity[i];
		fL3Current[i] = obj.fL3Current[i];
		fDipoleCurrent[i] = obj.fDipoleCurrent[i];
		//fCavernTemperature[i] = obj.fCavernTemperature[i];
		fCavernAtmosPressure[i] = obj.fCavernAtmosPressure[i];
	}
}

//-----------------------------------------------------------------------------

AliGRPObject& AliGRPObject:: operator=(const AliGRPObject & obj) 
{

	//
	// AliGRPObject assignment operator
	//

	this->fTimeStart = obj.GetTimeStart();
	this->fTimeEnd = obj.GetTimeEnd();
	this->fBeamEnergy = obj.GetBeamEnergy();
	this->fBeamType = obj.GetBeamType();
	this->fNumberOfDetectors = obj.GetNumberOfDetectors();
	this->fDetectorMask = obj.GetDetectorMask();
	this->fLHCPeriod = obj.GetLHCPeriod();
	this->fRunType = obj.GetRunType();
	this->fLHCState = obj.GetLHCState();
	this->fLHCLuminositySplineFit = obj.GetLHCLuminositySplineFit();
	this->fBeamIntensitySplineFit = obj.GetBeamIntensitySplineFit();
	this->fL3Polarity = obj.GetL3Polarity();
	this->fDipolePolarity = obj.GetDipolePolarity();
	this->fCavernAtmosPressure = obj.GetCavernAtmosPressure();
	this->fSurfaceAtmosPressure = obj.GetSurfaceAtmosPressure();
	this->fPoints = obj.GetPoints();
	this->fDimension = obj.GetDimension();

	this->fLHCLuminosity = new Float_t[fPoints];
	this->fBeamIntensity = new Float_t[fPoints];
	this->fL3Current = new Float_t[fPoints];
	this->fDipoleCurrent = new Float_t[fPoints];
	this->fCavernTemperature = new Float_t[fPoints];
	//this->fCavernAtmosPressure = new Float_t[fPoints];
	
	for (Int_t nhp=0; nhp< fDimension; nhp++){
		this->fHallProbes[nhp] = obj.GetHallProbes(nhp);

	}
	for (Int_t i = 0; i < fPoints; i++){

		//		this->fBeamEnergy[i] = obj.GetBeamEnergy((Stats)i);
		this->fLHCLuminosity[i] = obj.GetLHCLuminosity((Stats)i);
		this->fBeamIntensity[i] = obj.GetBeamIntensity((Stats)i);
		this->fL3Current[i] = obj.GetL3Current((Stats)i);
		this->fDipoleCurrent[i] = obj.GetDipoleCurrent((Stats)i);
		this->fCavernTemperature[i] = obj.GetCavernTemperature((Stats)i);
		//		this->fCavernAtmosPressure[i] = obj.GetCavernAtmosPressure((Stats)i);
	}

	return *this;
}

//-----------------------------------------------------------------------------

AliGRPObject::~AliGRPObject() {

	//
	// dtor
	//

	
	delete [] fHallProbes;
	delete [] fLHCLuminosity;
	delete [] fBeamIntensity;
	delete [] fL3Current;
	delete [] fDipoleCurrent;
	delete [] fCavernTemperature;

	if (fLHCLuminositySplineFit){
		delete fLHCLuminositySplineFit;
		fLHCLuminositySplineFit = 0x0;
	}
	if (fBeamIntensitySplineFit){
		delete fBeamIntensitySplineFit;
		fBeamIntensitySplineFit = 0x0;
	}
	if (fCavernAtmosPressure){
		delete fCavernAtmosPressure;
		fCavernAtmosPressure = 0x0;
	}
	if (fSurfaceAtmosPressure){
		delete fSurfaceAtmosPressure;
		fSurfaceAtmosPressure = 0x0;
	}
}

//-----------------------------------------------------------------------------

Float_t* AliGRPObject::GetHallProbes(DP_HallProbes hp) {

	//
	// method to return array of statistical
        // variables for Hall Probe hp
	//

	Float_t * array = new Float_t[fPoints];
	Int_t shift = fPoints*(Int_t)hp; 
	for (Int_t i=0;i<fPoints; i++){

		array[i] = fHallProbes[shift+i];

	}

	return array;
}

//-------------------------------------------------------------------------------

void AliGRPObject::SetHallProbes(DP_HallProbes hp, Float_t* hall_probe){

	//
	// method to set hall probe hp 
	// from a given array
	//

	Int_t shift = fPoints*hp;
	for (Int_t i = 0; i< fPoints; i++){

		fHallProbes[i+shift] =  hall_probe[i];
	}
	return;
}

//-------------------------------------------------------------------------------

void AliGRPObject::ReadValuesFromMap(TMap* mapGRP){

	//
	// method to set the values of the GRP parameters 
	// reading them from the old format of the GRP 
	// object, i.e. a TMap
	//

	if (mapGRP->GetValue("fAliceStartTime")){
		SetTimeStart((time_t)(((TObjString*)(mapGRP->GetValue("fAliceStartTime")))->GetString()).Atoi());
	}
	else {
		AliError(Form("No fAliceStartTime value found in GRP map!"));
	}
	if (mapGRP->GetValue("fAliceStopTime")){
		SetTimeEnd((time_t)(((TObjString*)(mapGRP->GetValue("fAliceStopTime")))->GetString()).Atoi());
	}
	
	else { 
		AliError(Form("No fAliceStopTime value found in GRP map!"));
	}

	if(mapGRP->GetValue("fAliceBeamEnergy")){
		SetBeamEnergy((((TObjString*)(mapGRP->GetValue("fAliceBeamEnergy")))->GetString()).Atof());
	}
	else { 
		AliError(Form("No fAliceBeamEnergy value found in GRP map!"));
	}
	if(mapGRP->GetValue("fAliceBeamType")){
		SetBeamType(((TObjString*)(mapGRP->GetValue("fAliceBeamType")))->GetString());
	}
	else { 
		AliError(Form("No fAliceBeamType value found in GRP map!"));
	}
	if(mapGRP->GetValue("fNumberOfDetectors")){
		SetNumberOfDetectors((Char_t)(((TObjString*)(mapGRP->GetValue("fNumberOfDetectors")))->GetString()).Atoi()); 
	}
	else { 
		AliError(Form("No fNumberOfDetectors value found in GRP map!"));
	}
	if(mapGRP->GetValue("fDetectorMask")){
		SetDetectorMask((UInt_t)(((TObjString*)(mapGRP->GetValue("fDetectorMask")))->GetString()).Atoi());  
	}
	else { 
		AliError(Form("No fDetectorMask value found in GRP map!"));
	}
	if(mapGRP->GetValue("fLHCPeriod")){
		SetLHCPeriod(((TObjString*)(mapGRP->GetValue("fLHCPeriod")))->GetString());
	}
	else { 
		AliError(Form("No fLHCPeriod value found in GRP map!"));
	}
	if(mapGRP->GetValue("fRunType")){
		SetRunType(((TObjString*)(mapGRP->GetValue("fRunType")))->GetString());
	}
	else { 
		AliError(Form("No fRunType value found in GRP map!"));
	}
	if(mapGRP->GetValue("fLHCState")){
		SetLHCState(((TObjString*)(mapGRP->GetValue("fLHCState")))->GetString());
	}
	else { 
		AliError(Form("No fLHCState value found in GRP map!"));
	}
	if(mapGRP->GetValue("fLHCluminosity")){
		AliInfo(Form("fLHCLuminosity found, but porting only average to the new object, since the other values are not available in the old object"));
		SetLHCLuminosity((Float_t)(((TObjString*)(mapGRP->GetValue("fLHCLuminosity")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fLHCLuminosity value found in GRP map!"));
	}
	if(mapGRP->GetValue("fBeamIntensity")){
		AliInfo(Form("fBeamIntensity found, but porting only average to the new object, since the other values are not available in the old object"));
		SetBeamIntensity((Float_t)(((TObjString*)(mapGRP->GetValue("fBeamIntensity")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fBeamIntensity value found in GRP map!"));
	}
	if(mapGRP->GetValue("fL3Polarity")){
		SetL3Polarity((Char_t)(((TObjString*)(mapGRP->GetValue("fL3Polarity")))->GetString()).Atoi());
	}	
	else { 
		AliError(Form("No fL3Polarity value found in GRP map!"));
	}
	if(mapGRP->GetValue("fDipolePolarity")){
		SetDipolePolarity((Char_t)(((TObjString*)(mapGRP->GetValue("fDipolePolarity")))->GetString()).Atoi());  
	}	
	else { 
		AliError(Form("No fDipolePolarity value found in GRP map!"));
	}
	if(mapGRP->GetValue("fL3Current")){
		AliInfo(Form("fL3Current found, but porting only average to the new object, since the other values are not available in the old object"));
		SetL3Current((Float_t)(((TObjString*)(mapGRP->GetValue("fL3Current")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fL3Current value found in GRP map!"));
	}
	if(mapGRP->GetValue("fDipoleCurrent")){
		AliInfo(Form("fDipoleCurrent found, but porting only average to the new object, since the other values are not available in the old object"));
		SetDipoleCurrent((Float_t)(((TObjString*)(mapGRP->GetValue("fDipoleCurrent")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fDipoleCurrent value found in GRP map!"));
	}
	if(mapGRP->GetValue("fCavernTemperature")){
		AliInfo(Form("fCaverntemperature found, but porting only average to the new object, since the other values are not available in the old object"));
		SetCavernTemperature((Float_t)(((TObjString*)(mapGRP->GetValue("fCavernTemperature")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fCavernTemperature value found in GRP map!"));
	}
	/*
	if(mapGRP->GetValue("fCavernAtmosPressure")){
		SetCavernAtmosPressure((Float_t)(((TObjString*)(mapGRP->GetValue("fCavernAtmosPressure")))->GetString()).Atof(),(Stats)0);
	}	
	else { 
		AliError(Form("No fCavernAtmosPressure value found in GRP map!"));
	}
	*/
	if(mapGRP->GetValue("fCavernAtmosPressure")){
		AliInfo(Form("fCavernAtmosPressure found, but not ported to the new object since of a different type"));
	}	
	else { 
		AliError(Form("No fCavernAtmosPressure value found in GRP map!"));
	}
	if(mapGRP->GetValue("fP2Pressure")){
		SetSurfaceAtmosPressure((AliDCSSensor*)((TObjString*)(mapGRP->GetValue("fP2Pressure"))));
	}
	else { 
		AliError(Form("No fP2Pressure value found in GRP map!"));
	}
	
	return;

}

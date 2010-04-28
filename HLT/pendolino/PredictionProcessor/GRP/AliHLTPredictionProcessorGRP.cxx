// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Gaute Ovrebekk                                        *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

//  @file   AliHLTPredictionProcessorGRP.cxx
//  @author Matthias Richter
//  @date   2010-04-12
//  @brief  Prediction processor for the GRP entry
// 

#include "AliHLTPredictionProcessorGRP.h"
#include <AliCDBMetaData.h>
#include <AliCDBEntry.h>

#include "AliGRPObject.h"
#include "AliCDBMetaData.h"

//#include <TObjArray.h>
//#include <AliDCSValue.h>
//#include <TTimeStamp.h>


ClassImp(AliHLTPredictionProcessorGRP)

AliHLTPredictionProcessorGRP::AliHLTPredictionProcessorGRP(AliHLTPendolino* pendolino)
  : AliHLTPredictionProcessorInterface("GRP", pendolino)
{
  // constructor
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}


AliHLTPredictionProcessorGRP::~AliHLTPredictionProcessorGRP()
{
  // destructor
}


UInt_t AliHLTPredictionProcessorGRP::makePrediction(Bool_t /*doPrediction*/)
{
  // switch for prediction making

  // not exactly clear what to do here, have to check the interface
  return 0;
}


void AliHLTPredictionProcessorGRP::Initialize(Int_t run, UInt_t startTime, 
					      UInt_t endTime)
{
  // initializes AliHLTPredictionProcessorGRP
  AliHLTPredictionProcessorInterface::Initialize(run, startTime, endTime);

  TString msg("Initialized GRP PredictProc. Run: ");
  msg += GetRunNumber();
  msg += ", start time: ";
  msg += StartTime();
  msg += ", end time: ";
  msg += EndTime();
  msg += ".";	
  Log(msg.Data());
}


UInt_t AliHLTPredictionProcessorGRP::Process(TMap* dcsAliasMap)
{
  // processes the DCS value map
  
  if (!dcsAliasMap) return 9;
  if (dcsAliasMap->GetEntries() == 0 ) return 9;

  Float_t l3Current=0.0;
  Float_t l3Polarity=0.0;
  Float_t dipoleCurrent=0.0;
  Float_t dipolePolarity=0.0;
  Float_t cavernAtmosPressure=0.0;
  Float_t cavernAtmosPressure2=0.0;
  Float_t surfaceAtmosPressure=0.0;
  
  Bool_t bRet = kTRUE;
  const char* key="";

  key="L3Current";
  if (!GetSensorValue(dcsAliasMap, key, &l3Current)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="L3Polarity";
  if (!GetSensorValue(dcsAliasMap, key, &l3Polarity)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="DipoleCurrent";
  if (!GetSensorValue(dcsAliasMap, key, &dipoleCurrent)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="DipolePolarity";
  if (!GetSensorValue(dcsAliasMap, key, &dipolePolarity)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="CavernAtmosPressure";
  if (!GetSensorValue(dcsAliasMap, key, &cavernAtmosPressure)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="CavernAtmosPressure2";
  if (!GetSensorValue(dcsAliasMap, key, &cavernAtmosPressure2)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  key="SurfaceAtmosPressure2";
  if (!GetSensorValue(dcsAliasMap, key, &surfaceAtmosPressure)) {
    Log(Form("failed to extract %s from alias map", key));
    bRet=kFALSE;
  }

  // generate GRP object
  AliGRPObject* grpObj=new AliGRPObject;
  float cmsEnergy=14000;
  grpObj->SetBeamEnergy(cmsEnergy/0.120); // LHC convention
  grpObj->SetBeamType("p-p");
  grpObj->SetL3Current(l3Current,(AliGRPObject::Stats)0);
  grpObj->SetDipoleCurrent(dipoleCurrent,(AliGRPObject::Stats)0);  
  grpObj->SetL3Polarity(l3Polarity);  
  grpObj->SetDipolePolarity(dipolePolarity);
  grpObj->SetPolarityConventionLHC();                    // LHC convention +/+ current -> -/- field main components
  // TODO: set also the pressure sensors, but first need to clarify how to use the AliDCSSensor

  UInt_t start=0;
  AliCDBMetaData cdbMetaData;
  cdbMetaData.SetResponsible("ALICE HLT");
  cdbMetaData.SetComment(Form("GRP entry for the magnetic field initialization of HLT components, produced by %s", ClassName()));
  
  if (Store("GRP", "Data", grpObj, &cdbMetaData, start, kTRUE)) {
  } else {
    Log(" *** Failed to store GRP object");
    return 7;
  }
  
  return 0;
}

TMap* AliHLTPredictionProcessorGRP::produceTestData(TString /*aliasName*/)
{
  // produces test data for AliHLTPredictionProcessorGRP
  TMap* resultMap = 0;

  return resultMap;
}



#ifndef ALIPHOSCALIBDATA_H
#define ALIPHOSCALIBDATA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//  class for PHOS calibration                //
////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"

class AliPHOSEmcCalibData;
class AliPHOSCpvCalibData;
class AliPHOSEmcBadChannelsMap;
class AliCDBMetaData;

class AliPHOSCalibData: public TNamed {

 public:
  AliPHOSCalibData();
  AliPHOSCalibData(Int_t runNumber);
  AliPHOSCalibData(AliPHOSCalibData & phosCDB);
  virtual ~AliPHOSCalibData();

  AliPHOSCalibData & operator = (const AliPHOSCalibData & rhs);

  void Reset();
  virtual void Print(Option_t *option = "") const; 
  
  void CreateNew();
  void RandomEmc(Float_t ccMin=0.5   , Float_t ccMax=1.5);
  void RandomCpv(Float_t ccMin=0.0009, Float_t ccMax=0.0015);

  Float_t GetADCchannelEmc(Int_t module, Int_t column, Int_t row) const;
  Float_t GetADCpedestalEmc(Int_t module, Int_t column, Int_t row) const;
  
  void SetADCchannelEmc(Int_t module, Int_t column, Int_t row, Float_t value);
  void SetADCpedestalEmc(Int_t module, Int_t column, Int_t row, Float_t value);

  Float_t GetADCchannelCpv(Int_t module, Int_t column, Int_t row) const;
  Float_t GetADCpedestalCpv(Int_t module, Int_t column, Int_t row) const;
  
  void SetADCchannelCpv(Int_t module, Int_t column, Int_t row, Float_t value);
  void SetADCpedestalCpv(Int_t module, Int_t column, Int_t row, Float_t value);

  Int_t  GetNumOfEmcBadChannels() const;
  Bool_t IsBadChannelEmc(Int_t module, Int_t col, Int_t row) const; 
  void   EmcBadChannelIds(Int_t *badIds=0); 

  void SetEmcDataPath(const char* emcPath) {fEmcDataPath=emcPath;}
  void SetCpvDataPath(const char* cpvPath) {fCpvDataPath=cpvPath;}

  Bool_t WriteEmc(Int_t firstRun, Int_t lastRun, AliCDBMetaData *md);
  Bool_t WriteCpv(Int_t firstRun, Int_t lastRun, AliCDBMetaData *md);
  Bool_t WriteEmcBadChannelsMap(Int_t firstRun, Int_t lastRun, AliCDBMetaData *md);

 private:

  AliPHOSEmcCalibData* fCalibDataEmc; // EMC calibration data
  AliPHOSCpvCalibData* fCalibDataCpv; // CPV calibration data
  AliPHOSEmcBadChannelsMap* fEmcBadChannelsMap; // EMC bad channels map
  
  TString fEmcDataPath; // path to EMC calibration data
  TString fCpvDataPath; // path to CPV calibration data
  TString fEmcBadChannelsMapPath; // path to bad channels map

  ClassDef(AliPHOSCalibData,4)    // PHOS Calibration data
};

#endif

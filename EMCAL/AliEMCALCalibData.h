#ifndef ALIEMCALCALIBDATA_H
#define ALIEMCALCALIBDATA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//
//  class for EMCAL calibration               //
//
////////////////////////////////////////////////

#include "TNamed.h"
#include "AliEMCALGeoParams.h"

class AliEMCALCalibData: public TNamed {

 public:
  AliEMCALCalibData();
  AliEMCALCalibData(const char* name);
  AliEMCALCalibData(const AliEMCALCalibData &calibda);
  AliEMCALCalibData& operator= (const AliEMCALCalibData &calibda);
  virtual ~AliEMCALCalibData();
  void Reset();
  virtual void Print(Option_t *option = "") const;
  
  // All indexes start from 0!
  Float_t GetADCchannel      (Int_t module, Int_t column, Int_t row) const;
  Float_t GetADCchannelDecal (Int_t module, Int_t column, Int_t row) const;
  Float_t GetADCpedestal     (Int_t module, Int_t column, Int_t row) const;
  Float_t GetTimeChannel     (Int_t module, Int_t column, Int_t row) const;
  Float_t GetTimeChannelDecal(Int_t module, Int_t column, Int_t row) const;
	
  Float_t GetADCchannelRef   () {return fADCchannelRef;}

  //
  void SetADCchannel      (Int_t module, Int_t column, Int_t row, Float_t value);
  void SetADCchannelDecal (Int_t module, Int_t column, Int_t row, Float_t value);
  void SetADCpedestal     (Int_t module, Int_t column, Int_t row, Float_t value);
  void SetTimeChannel     (Int_t module, Int_t column, Int_t row, Float_t value);
  void SetTimeChannelDecal(Int_t module, Int_t column, Int_t row, Float_t value);

  void SetADCchannelRef   (Float_t value) {fADCchannelRef = value;}

  // Fill for (relative) recalibration (undo 1, apply 2)
  void Fill(const AliEMCALCalibData *cd1, const AliEMCALCalibData *cd2, Bool_t print=0);

 protected:
  Float_t  fADCchannel      [AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows] ; // width of one ADC channel in GeV ([mod][col][row])
  Float_t  fADCchannelDecal [AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows] ; // decalibrate width of one ADC channel in GeV ([mod][col][row])
  Float_t  fADCpedestal     [AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows] ; // value of the  ADC pedestal ([mod][col][row])
  Float_t  fTimeChannel     [AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows] ; // time width of one ADC channel ([mod][col][row])
  Float_t  fTimeChannelDecal[AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows] ; // time width of one ADC channel ([mod][col][row])

  Float_t  fADCchannelRef;

  //
  ClassDef(AliEMCALCalibData,3)    // EMCAL Calibration data
};

#endif

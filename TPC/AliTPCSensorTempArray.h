#ifndef AliTPCSensorTempArray_H
#define AliTPCSensorTempArray_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TPC calibration class for temperature sensors                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"

#include "AliDCSSensorArray.h"
#include "AliTPCSensorTemp.h"

class TTimeStamp;
class TMap;
class TGraph;
class TObjString;
class AliSplineFit;
class AliDCSSensor;

#include "TString.h"


class AliTPCSensorTempArray : public AliDCSSensorArray {
 public:
  AliTPCSensorTempArray();
  AliTPCSensorTempArray(Int_t prevRun); 
  AliTPCSensorTempArray(const char *fname);
  AliTPCSensorTempArray (UInt_t startTime, UInt_t endTime, const char *filepath=".");
  AliTPCSensorTempArray(const AliTPCSensorTempArray &c);   
  virtual ~AliTPCSensorTempArray();
  AliTPCSensorTempArray &operator=(const AliTPCSensorTempArray &c);
  virtual void Copy (TObject &c) const;
  void SetGraph     (TMap *map);
  void MakeSplineFit(TMap *map);
  void ReadSensors  (const char *fname);
  const char* GetAmandaString() { return fAmandaString.Data(); }
  void SetAmandaString(const char* string) {fAmandaString=string;}
  TMap* ExtractDCS  (TMap *dcsMap);
  AliTPCSensorTemp* GetSensor (Int_t type, Int_t side, Int_t sector, Int_t num);
  AliTPCSensorTemp* GetSensor (Int_t IdDCS);
  AliTPCSensorTemp* GetSensor (Double_t x, Double_t y, Double_t z);

 protected:

  TString fAmandaString;        //! Amanda string to identify temperature entries
  ClassDef(AliTPCSensorTempArray,1)       //  TPC calibration class for parameters which are saved per pad

};

#endif

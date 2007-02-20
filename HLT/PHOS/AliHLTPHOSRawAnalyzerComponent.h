#ifndef ALIHLTPHOSRAWANALYZERCOMPONENT_H
#define ALIHLTPHOSRAWANALYZERCOMPONENT_H

/* Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                          */

#include "AliHLTProcessor.h"
#include "AliHLTPHOSRawAnalyzer.h"
#include "AliRawReaderMemory.h"
#include "AliCaloRawStream.h"
#include "AliHLTPHOSDefinitions.h"

class AliHLTPHOSRcuCellEnergyDataStruct;


class AliHLTPHOSRawAnalyzerComponent: public AliHLTProcessor
{
 public:

  AliHLTPHOSRawAnalyzerComponent();
  ~AliHLTPHOSRawAnalyzerComponent();
  AliHLTPHOSRawAnalyzerComponent(const AliHLTPHOSRawAnalyzerComponent & );
  AliHLTPHOSRawAnalyzerComponent & operator = (const AliHLTPHOSRawAnalyzerComponent &)
   {
      return *this;
   };

  virtual int DoInit( int argc, const char** argv );
  virtual int Deinit();
  virtual int DoDeinit();
  void DumpData();
  void DumpChannelData(Double_t *data); 
  void SetEquippmentID(AliHLTUInt32_t id);
  int GetEquippmentID();
  void SetCoordinates( AliHLTUInt32_t equippmentID);
  virtual const char* GetComponentID() = 0;
  virtual void GetInputDataTypes(std::vector<AliHLTComponentDataType, std::allocator<AliHLTComponentDataType> >&);
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
  virtual AliHLTComponent* Spawn() = 0;
  virtual int DoEvent(const AliHLTComponentEventData&, const AliHLTComponentBlockData*, AliHLTComponentTriggerData&, AliHLTUInt8_t*, AliHLTUInt32_t&, std::vector<AliHLTComponentBlockData, std::allocator<AliHLTComponentBlockData> >&);

 protected:
  AliHLTPHOSRawAnalyzer *analyzerPtr; 
  void Reset();
  void ResetDataPtr();

 private:
  static int fEventCount;
  AliHLTUInt32_t fEquippmentID;
  AliHLTUInt16_t fRcuX;
  AliHLTUInt16_t fRcuZ;
  AliHLTUInt16_t fRcuRowOffeset;
  AliHLTUInt16_t fRcuColOffeset;
  AliHLTUInt16_t fModuleID;
  Double_t fTmpChannelData[1008];
  Double_t fMaxValues[5][64][56][2];
  AliCaloRawStream *fPHOSRawStream;
  AliRawReaderMemory *fRawMemoryReader;
  AliHLTPHOSRcuCellEnergyDataStruct* outPtr;
  static const AliHLTComponentDataType inputDataTypes[];
};
#endif

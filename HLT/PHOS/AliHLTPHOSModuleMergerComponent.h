#ifndef ALIHLTPHOSMODULEMERGERCOMPONENT_H
#define ALIHLTPHOSMODULEMERGERCOMPONENT_H

/* Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice  */ 

#include "AliHLTProcessor.h"
#include "AliHLTPHOSDefinitions.h"
#include "AliHLTPHOSCommonDefs.h"

class AliHLTPHOSModuleMergerComponent:public AliHLTProcessor
{
 public:
  AliHLTPHOSModuleMergerComponent();
  virtual ~AliHLTPHOSModuleMergerComponent();
  AliHLTPHOSModuleMergerComponent(const AliHLTPHOSModuleMergerComponent & );
  AliHLTPHOSModuleMergerComponent & operator = (const AliHLTPHOSModuleMergerComponent &)
    {
      return *this;
   };
  virtual int DoInit(int argc =0, const char** argv = 0);
  virtual int Deinit();
  virtual int DoDeinit();
  virtual int DoEvent(const AliHLTComponentEventData&, const AliHLTComponentBlockData*, AliHLTComponentTriggerData&, AliHLTUInt8_t*, AliHLTUInt32_t&, std::vector<AliHLTComponentBlockData, std::allocator<AliHLTComponentBlockData> >&);
  void DumpData(int gain =0);
  int GetEquippmentId();
  virtual const char* GetComponentID();
  virtual void GetInputDataTypes(std::vector<AliHLTComponentDataType, std::allocator<AliHLTComponentDataType> >&);
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
  void SetEquippmentId(int id = 0);
  virtual AliHLTComponent* Spawn();

 protected:
  void Reset();
  void ResetDataPtr();

 private:
  int fEventCount;                                                    /**<event counter*/
  AliHLTUInt32_t fEquippmentID;                                       /**<Eguippment ID as given by ALICE*/
  Double_t fTmpChannelData[ALTRO_MAX_SAMPLES];                        /**<Array to tmporarily store samples from a single ALTRO*/ 
  Double_t fMaxValues[N_MODULES][N_ROWS_MOD][N_COLUMNS_MOD][N_GAINS]; /**<Tower/Crystal energies*/
  static const AliHLTComponentDataType fgkInputDataTypes[];           /**<The input datatypes the component can recieve (obsolete ?)*/
  static const AliHLTComponentDataType fgkOutputDataType;             /**<The type of date the compnent writes to shared memory */
};

#endif

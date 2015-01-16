#ifndef ALIHLTMUONDIGITREADERCOMPONENT_H
#define ALIHLTMUONDIGITREADERCOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
///  @file   AliHLTMUONDigitReaderComponent.h
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   24 October 2014
///  @brief  digit reader source component for the dimuon HLT.
///

#include "AliHLTDataSource.h"
#include "AliHLTMUONDataBlockWriter.h"

class TFile;
class TTree;
class AliMUONVDigit;
class AliMUONVDigitStore;

/**
 * @class AliHLTMUONDigitReaderComponent
 * @brief A source component for the dHLT digit reading from root file.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONDigitReader <br>
 * Library: \b libAliHLTMUON.so  <br>
 * Input Data Types: none <br>
 * Output Data Types: ('DIGSTORE', 'MUON') <br>
 *
 * @ingroup alihlt_muon_components
 */
class AliHLTMUONDigitReaderComponent : public AliHLTDataSource
{
public:
  AliHLTMUONDigitReaderComponent();
  virtual ~AliHLTMUONDigitReaderComponent();
  
  // Public functions to implement AliHLTDataSource's interface.
  // These functions are required for the registration process
  
  virtual const char* GetComponentID();
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
  virtual AliHLTComponent* Spawn();
  
protected:
  
  // Protected functions to implement AliHLTDataSource's interface.
  // These functions provide initialization as well as the actual processing
  // capabilities of the component.
  
  virtual int DoInit(int argc, const char** argv);
  virtual int DoDeinit();
  virtual int GetEvent(const AliHLTComponentEventData& evtData,
                       AliHLTComponentTriggerData& trigData,
                       AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
                       AliHLTComponentBlockDataList& outputBlocks);
  using AliHLTDataSource::GetEvent;
  
private:
  
  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONDigitReaderComponent(const AliHLTMUONDigitReaderComponent& /*obj*/);
  /// Not implemented.
  AliHLTMUONDigitReaderComponent& operator = (const AliHLTMUONDigitReaderComponent& /*obj*/);
  
  void CleanMemory();
  
  int CreateDigitBlock(AliHLTUInt8_t *outputPtr, AliHLTUInt32_t size,
                       AliHLTMUONDigitsBlockWriter *&digitblock);
  
  int AddDigit(AliMUONVDigit &digit, AliHLTMUONDigitsBlockWriter &digitblock);
  
  TFile* fFile; ///< input file
  TTree* fTree; ///< input tree
  AliMUONVDigitStore *fDigitStore; ///< list of digits in an AliMUONVDigit format
  Bool_t fPerDE; ///< register digits per DE
  
  ClassDef(AliHLTMUONDigitReaderComponent, 0)
};

#endif // ALIHLTMUONDIGITREADERCOMPONENT_H


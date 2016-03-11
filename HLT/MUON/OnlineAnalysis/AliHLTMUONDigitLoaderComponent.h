#ifndef ALIHLTMUONDIGITLOADERCOMPONENT_H
#define ALIHLTMUONDIGITLOADERCOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
///  @file   AliHLTMUONDigitLoaderComponent.h
///  @author Hugo Pereira Da Costa <pereira@hep.saclay.cea.fr>
///  @date   March 08 2016
///  @brief  load digits from raw data
///

#include "AliHLTProcessor.h"
#include "AliHLTMUONDigitsBlockStruct.h"

#include "AliMUONTrackerDDLDecoderEventHandler.h"
#include "AliMUONTrackerDDLDecoder.h"
#include "AliHLTMUONDataBlockWriter.h"

#include <TExMap.h>

/**
 * @class AliHLTMUONDigitLoaderComponent
 * @brief A processing component for the dHLT tracker DDL reconstruction.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONDigitLoader <br>
 * Library: \b libAliHLTMUON.so  <br>
 * Input Data Types: ('DDL_RAW ', 'MUON') <br>
 * Output Data Types: ('Digit', 'MUON') <br>
 *
 * @ingroup alihlt_muon_components
 */
class AliHLTMUONDigitLoaderComponent : public AliHLTProcessor
{

  public:
  AliHLTMUONDigitLoaderComponent();
  virtual ~AliHLTMUONDigitLoaderComponent();

  // Public functions to implement AliHLTProcessor's interface.
  // These functions are required for the registration process

  virtual const char* GetComponentID();
  virtual void GetInputDataTypes(AliHLTComponentDataTypeList& );
  virtual AliHLTComponentDataType GetOutputDataType();
  virtual void GetOutputDataSize(unsigned long&, double& );
  virtual AliHLTComponent* Spawn();

  protected:
  // Protected functions to implement AliHLTProcessor's interface.
  // These functions provide initialization as well as the actual processing
  // capabilities of the component.

  virtual int DoInit(int , const char** );
  virtual int DoDeinit();
  virtual int DoEvent(
    const AliHLTComponentEventData&,
    const AliHLTComponentBlockData*,
    AliHLTComponentTriggerData&,
    AliHLTUInt8_t*,
    AliHLTUInt32_t&,
    AliHLTComponentBlockDataList&);

  using AliHLTProcessor::DoEvent;

  private:

  // create mapping
  void CreateMapping( void );

  // create digit block
  int CreateDigitsBlock(
    AliHLTUInt8_t*, AliHLTUInt32_t ,
    AliHLTMUONDigitsBlockWriter*& ) const;

  // internal class to interface the raw data decoder
  class RawDecoderHandler : public AliMUONTrackerDDLDecoderEventHandler
  {

    public:
    void OnNewBusPatch(const AliMUONBusPatchHeaderStruct* header, const void* data);
    void OnData(UInt_t data, bool parityError);
    void OnError(ErrorCode error, const void* location);
    void SetParent(AliHLTMUONDigitLoaderComponent* parent) { fParent = parent; }

    private:
    AliHLTMUONDigitLoaderComponent* fParent;
    Int_t fDetElemId;

  };

  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONDigitLoaderComponent(const AliHLTMUONDigitLoaderComponent& /*obj*/);

  /// Not implemented.
  AliHLTMUONDigitLoaderComponent& operator = (const AliHLTMUONDigitLoaderComponent& /*obj*/);

  /// maps detector id to the plane type of cathode 0
  /** this is needed to get the right unique id for decoded digits */
  TExMap fDEPlaneType;

  /// raw data decoder
  AliMUONTrackerDDLDecoder<RawDecoderHandler> fRawDecoder;

  /// true if event could not be properly decoded
  Bool_t fBadEvent;

  // digit block writer
  AliHLTMUONDigitsBlockWriter* fDigitsBlock;

  friend class AliMUONTrackerDDLDecoder<RawDecoderHandler>;

  ClassDef(AliHLTMUONDigitLoaderComponent, 0)
};

#endif

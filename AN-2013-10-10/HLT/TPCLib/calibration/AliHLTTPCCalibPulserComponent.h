//-*- Mode: C++ -*-
#ifndef ALIHLTTPCCALIBPULSERCOMPONENT_H
#define ALIHLTTPCCALIBPULSERCOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTTPCCalibPulserComponent.h
    @author Jochen Thaeder
    @date   
    @brief  A pulser calibration component for the TPC.
*/

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt   

#include "AliHLTCalibrationProcessor.h"
#include "AliHLTTPCDefinitions.h"

class AliTPCRawStream;
class AliRawReaderMemory;
class AliTPCCalibPulser;

/**
 * @class AliHLTTPCCalibPulserComponent
 * 
 * This class is the calibration component for the AliTPCCalibPulser class 
 * used for pulser calibration of the TPC. 
 * 
 * It inherits from the AliHLTCalibrationProcessor and uses the high-level 
 * interface. The output is the class AliTPCCalibPulser as a TObject.
 *
 * The component has the following component arguments:
 *   -enableanalysis       : Wether to enable analyis before shipping data to FXS
 *
 * @ingroup alihlt_tpc_components
 */
class AliHLTTPCCalibPulserComponent : public AliHLTCalibrationProcessor
    {
    public:
      /** constructor */
      AliHLTTPCCalibPulserComponent();
      /** destructor */
      virtual ~AliHLTTPCCalibPulserComponent();
      
      // Public functions to implement AliHLTComponent's interface.
      // These functions are required for the registration process

      const char* GetComponentID();
      void GetInputDataTypes( vector<AliHLTComponentDataType>& list);
      AliHLTComponentDataType GetOutputDataType();
      virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );
      AliHLTComponent* Spawn();

    protected:

      using AliHLTCalibrationProcessor::ProcessCalibration;
      using AliHLTCalibrationProcessor::ShipDataToFXS;
      
      // Protected functions to implement AliHLTComponent's interface.
      // These functions provide initialization as well as the actual processing
      // capabilities of the component. 
      
      /** Initialize the calibration component. */
      Int_t InitCalibration();

      /** Scan commandline arguments of the calibration component. */
      Int_t ScanArgument( Int_t argc, const char** argv );

      /** DeInitialize the calibration component. */
      Int_t DeinitCalibration();

      /** Process the data in the calibration component. */
      Int_t ProcessCalibration( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData );

      /** Ship the data to the FXS at end of run or eventmodulo. */
      Int_t ShipDataToFXS( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData );

    private:
      /** copy constructor prohibited */
      AliHLTTPCCalibPulserComponent(const AliHLTTPCCalibPulserComponent&);
      /** assignment operator prohibited */
      AliHLTTPCCalibPulserComponent& operator=(const AliHLTTPCCalibPulserComponent&);

      /** The reader object for reading from memory */
      AliRawReaderMemory* fRawReader;                                              //!transient

      /** The reader object for reading TPC raw data */  
      AliTPCRawStream* fRawStream;                                                 //!transient

      /** Pulser Calibration class */
      AliTPCCalibPulser * fCalibPulser;                                        //!transient
      
      /** Minimum patch specifcation for this component */
      AliHLTUInt8_t fMinPatch;                                                     // see above

      /** Minimum patch specifcation for this component */
      AliHLTUInt8_t fMaxPatch;                                                     // see above

      /** The Specification for this component */
      AliHLTUInt32_t fSpecification;                                               // see above

      /** Analysze calibration data before shipping to FXS */
      Bool_t fEnableAnalysis;                                                      // see above

      ClassDef(AliHLTTPCCalibPulserComponent, 0)

    };

#endif

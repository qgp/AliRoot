//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTTPCCALIBCECOMPONENT_H
#define ALIHLTTPCCALIBCECOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTTPCCalibCEComponent.h
    @author Jochen Thaeder
    @date   
    @brief  A pedestal calibration component for the TPC.
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt   

#include "AliHLTCalibrationProcessor.h"

class AliTPCRawStream;
class AliRawReaderMemory;
class AliTPCCalibCE;

/**
 * @class AliHLTTPCCalibCEComponent
 * 
 * This class is the calibration component for the AliTPCCalibCE class 
 * used for central electrode calibration of the TPC. 
 * 
 * It inherits from the AliHLTCalibrationProcessor and uses the high-level 
 * interface. The output is the class AliTPCCalibCE as a TObject.
 *
 * The component has the following component arguments:
 *   -enableanalysis       : Whether to enable analyis before shipping data to FXS
 *
 * @ingroup alihlt_tpc_components
 */
class AliHLTTPCCalibCEComponent : public AliHLTCalibrationProcessor
    {
    public:
      /** constructor */
      AliHLTTPCCalibCEComponent();
      /** destructor */
      virtual ~AliHLTTPCCalibCEComponent();
      
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
      AliHLTTPCCalibCEComponent(const AliHLTTPCCalibCEComponent&);
      /** assignment operator prohibited */
      AliHLTTPCCalibCEComponent& operator=(const AliHLTTPCCalibCEComponent&);

      /** The reader object for reading from memory */
      AliRawReaderMemory* fRawReader;                                              //!transient

      /** The reader object for reading TPC raw data */  
      AliTPCRawStream* fRawStream;                                                 //!transient

      /** Pedestal Calibration class */
      AliTPCCalibCE * fCalibCE;                                                    //!transient
      
      /** Minimum patch specifcation for this component */
      AliHLTUInt8_t fMinPatch;                                                     // see above

      /** Minimum patch specifcation for this component */
      AliHLTUInt8_t fMaxPatch;                                                     // see above

      /** The Specification for this component */
      AliHLTUInt32_t fSpecification;                                               // see above

      /** Analysze calibration data before shipping to FXS */
      Bool_t fEnableAnalysis;                                                      // see above

      ClassDef(AliHLTTPCCalibCEComponent, 2)

    };
#endif

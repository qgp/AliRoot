// -*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTTPCNOISEMAPCOMPONENT_H
#define ALIHLTTPCNOISEMAPCOMPONENT_H

//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTTPCNoiseMapComponent.h
    @author Kalliopi Kanaki
    @date   
    @brief  Component for Noise Map
*/

#include "AliHLTProcessor.h"

//forward declarations
class AliHLTTPCDigitReader;
class TH2;

/**
 * @class AliHLTTPCNoiseMapComponent
 * Implementation of the component to fill histograms with TPC noise and read
 * the noise map from OCDB by request.
 * The component implements the interface methods of the @ref AliHLTProcessor.
 * It reads the data pad by pad and fills histograms. The output is unpacked and 
 * sent to the clulsterfinder.
 * 
 * The component has the following component arguments:
 * - apply-noise-map  Read the noise map from OCDB and apply it on the online data
 *
 * -plot-side-a   Histograms the TPC side A
 *          
 * -plot-side-c   Histograms the TPC side C
 *
 * -apply-noisemap  Reads a noise map from a file and subtracts the value contained in every pad from the data
 *
 * @ingroup alihlt_tpc
 */
class AliHLTTPCNoiseMapComponent : public AliHLTProcessor {
    
   public:
   
   /** standard constructor */    
   AliHLTTPCNoiseMapComponent();           
   /** destructor */
   virtual ~AliHLTTPCNoiseMapComponent();

      // Public functions to implement AliHLTComponent's interface.
      // These functions are required for the registration process
      
      /** interface function, see @ref AliHLTComponent for description */
      const char* GetComponentID();							     
      /** interface function, see @ref AliHLTComponent for description */
      void GetInputDataTypes( vector<AliHLTComponentDataType>& list);			     
      /** interface function, see @ref AliHLTComponent for description */
      AliHLTComponentDataType GetOutputDataType();					     
      /** interface function, see @ref AliHLTComponent for description */
      int GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList);			   
      /** interface function, see @ref AliHLTComponent for description */
      virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier ); 
      /** interface function, see @ref AliHLTComponent for description */
      AliHLTComponent* Spawn(); 							   
      /** function for acting on the saving and cleaning histograms, after they are filled */
      void MakeHistosPublic();
  
   protected:
	
      // Protected functions to implement AliHLTComponent's interface.
      // These functions provide initialization as well as the actual processing capabilities of the component. 

      int DoInit( int argc, const char** argv );
      int DoDeinit();
//       int DoEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
// 		   AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
// 		   AliHLTUInt32_t& size, vector<AliHLTComponentBlockData>& outputBlocks );
      int DoEvent( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData );
      int Reconfigure(const char* cdbEntry, const char* chainId);

      using AliHLTProcessor::DoEvent;

   private:
   
      int Configure(const char* arguments);
          
      /** copy constructor prohibited */
      AliHLTTPCNoiseMapComponent(const AliHLTTPCNoiseMapComponent&);

      /** assignment operator prohibited */
      AliHLTTPCNoiseMapComponent& operator=(const AliHLTTPCNoiseMapComponent&);

      /** the reader object for data decoding */
      AliHLTUInt32_t fSpecification;  //!transient
      //AliHLTUInt8_t fMinPartition;    //!transient
      //AliHLTUInt8_t fMaxPartition;    //!transient
      
      //AliHLTTPCDigitReader *pDigitReader;

      Bool_t fPlotSideA;      //!transient
      Bool_t fPlotSideC;      //!transient
      Bool_t fApplyNoiseMap;  //!transient
      Bool_t fIsPacked;       //!transient   
      Bool_t fIsUnpacked;     //!transient
      
      Int_t fSlice;  //!transient
      
      Int_t  fCurrentPartition; //!transient
      Int_t  fCurrentRow;       //!transient
      
      TH2 *fHistSideA;     //!transient    
      TH2 *fHistSideC;     //!transient  
      TH2 *fHistCDBMap;    //!transient 
            
      ClassDef(AliHLTTPCNoiseMapComponent, 1)
    };

#endif

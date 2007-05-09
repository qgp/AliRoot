// @(#) $Id$

#ifndef ALIHLTTPCCLUSTERFINDERCOMPONENT_H
#define ALIHLTTPCCLUSTERFINDERCOMPONENT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTTPCClusterFinderComponent.h
    @author Timm Steinbeck, Matthias Richter, Jochen Thaeder
    @date   
    @brief  The TPC cluster finder component.
*/

#include "AliHLTProcessor.h"
#include "AliHLTTPCDefinitions.h"
#include "AliHLTTPCDigitReaderPacked.h"
#include "AliHLTTPCDigitReaderUnpacked.h"
#include "AliHLTTPCDigitReaderRaw.h"

class AliHLTTPCClusterFinder;

/**
 * @class AliHLTTPCClusterFinderComponent
 * Implementation of the cluster finder component.
 * The component implements the interface methods of the @ref AliHLTProcessor.
 * The actual cluster finding algorithm is implemented in @ref AliHLTTPCClusterFinder.
 * The component can handle unpacked and packed data of different formats via the
 * AliHLTTPCDigitReader implementations. Two components are registered, the
 * TPCClusterFinderUnpacked and the TPCClusterFinderPacked. The latter one can
 * instantiate different digit readers depending on the arguments.
 * 
 * The component has the following component arguments:
 * - rawreadermode   the mode for the @ref AliHLTTPCDigitReaderRaw
 * - adc-threshold   ADC count threshold for zero suppression, if <0 the base line
 *                   calculation and subtraction is switched off
 * - pp-run          set parameters specific to a pp run; currently this switches
 *                   cluster deconvolution off for pp runs
 *
 * @ingroup alihlt_tpc
 */
class AliHLTTPCClusterFinderComponent : public AliHLTProcessor
    {
    public:
        /**
         * constructor 
         * @param packed    whether to use the packed or unpacked reader 
         */
	AliHLTTPCClusterFinderComponent(bool packed);
	/** not a valid copy constructor, defined according to effective C++ style */
	AliHLTTPCClusterFinderComponent(const AliHLTTPCClusterFinderComponent&);
	/** not a valid assignment op, but defined according to effective C++ style */
	AliHLTTPCClusterFinderComponent& operator=(const AliHLTTPCClusterFinderComponent&);
	/** destructor */
	virtual ~AliHLTTPCClusterFinderComponent();

	// Public functions to implement AliHLTComponent's interface.
	// These functions are required for the registration process

	const char* GetComponentID();
	void GetInputDataTypes( vector<AliHLTComponentDataType>& list);
	AliHLTComponentDataType GetOutputDataType();
	virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );
	AliHLTComponent* Spawn();

    protected:
	
	// Protected functions to implement AliHLTComponent's interface.
	// These functions provide initialization as well as the actual processing
	// capabilities of the component. 

	int DoInit( int argc, const char** argv );
	int DoDeinit();
	int DoEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
		     AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
		     AliHLTUInt32_t& size, vector<AliHLTComponentBlockData>& outputBlocks );
	
    private:
	/** the cluster finder object */
	AliHLTTPCClusterFinder* fClusterFinder;                                      //!transient
	/** the reader object for data decoding */
	AliHLTTPCDigitReader* fReader;                                               //!transient

      bool fClusterDeconv;
      float fXYClusterError;
      float fZClusterError;
      /**
       * switch to indicated the reader
       * use fPackedSwitch = true for packed inputtype "gkDDLPackedRawDataType"
       * use fPackedSwitch = false for unpacked inputtype "gkUnpackedRawDataType"
       */
      Int_t fPackedSwitch;                                                           // see above
      
      ClassDef(AliHLTTPCClusterFinderComponent, 0)

    };
#endif

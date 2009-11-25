#ifndef ALIHLTMUONEMPTYEVENTFILTERCOMPONENT_H
#define ALIHLTMUONEMPTYEVENTFILTERCOMPONENT_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
/// @file   AliHLTMUONEmptyEventFilterComponent.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   2007-12-12
/// @brief  Declaration of the empty event filter component.
///

#include "AliHLTMUONProcessor.h"

#if __GNUC__ && __GNUC__ < 3
#define std
#endif

/**
 * @class AliHLTMUONEmptyEventFilterComponent
 * \brief Component for filtering on empty events.
 * This component class is a utility component for debugging. It is used to filter
 * empty dHLT events. (Specifically built for the Dec 2007 Cosmic tests where the
 * muon spectrometer should not see any hits. Therefor we would be interested to
 * analysis the raw data for those cases where the hit reconstructor actuall found
 * something, because this would be strange/abnormal.)
 * The component will look for dHLT data blocks containing results like dHLT
 * reconstructed hits, trigger records and dHLT tracks,
 * if any of the found blocks are not empty then every data block that it received
 * will be forwarded to the output.
 * This component would normally subscribe to all the front end DDL processing
 * components like MUONHitReconstructor and MUONTriggerReconstructor, the DDL
 * RORCPublishers for the DDL data and also the MUONMansoTrackerFSM component.
 * A dump subscriber can then connect to the empty event filter component to
 * make sure it only receives events that are not empty.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONEmptyEventFilter <br>
 * Library: \b libAliHLTMUON.so <br>
 * Input Data Types:  kAliHLTAnyDataType = "*******:***" <br>
 * Output Data Types: kAliHLTAnyDataType|kAliHLTDataOriginMUON = "*******:MUON" <br>
 *
 * <h2>Mandatory arguments:</h2>
 * None.
 *
 * <h2>Optional arguments:</h2>
 * \li -sendempty <br>
 *      This parameter causes the component to behave like an anti-filter
 *      meaning that it will send all events for which the dHLT results data
 *      blocks were empty. This is useful for collecting those events where dHLT
 *      is not finding anything but perhaps it should. <br>
 * \li -dumponerror <br>
 *      This flag will cause the component to dump the data blocks it received if
 *      an error occurs during the processing of an event. <br>
 * \li -dumppath <i>path</i> <br>
 *      Allows one to specify the path in which to dump the received data blocks
 *      if an error occurs. <br>
 *
 * <h2>Standard configuration:</h2>
 * There is no special configuration for this component.
 *
 * <h2>Default CDB entries:</h2>
 * None.
 *
 * <h2>Performance:</h2>
 * Less than a milliseconds per event.
 *
 * <h2>Memory consumption:</h2>
 * Minimal, under 1 MBytes.
 *
 * <h2>Output size:</h2>
 * The maximum is the same size as the input data size.
 *
 * @ingroup alihlt_dimuon_component
 */
class AliHLTMUONEmptyEventFilterComponent : public AliHLTMUONProcessor
{
public:
	AliHLTMUONEmptyEventFilterComponent();
	virtual ~AliHLTMUONEmptyEventFilterComponent();

	// Public functions to implement AliHLTComponent's interface.
	// These functions are required for the registration process.

	virtual const char* GetComponentID();
	virtual void GetInputDataTypes(AliHLTComponentDataTypeList& list);
	virtual AliHLTComponentDataType GetOutputDataType();
	virtual void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
	virtual AliHLTComponent* Spawn();
	
protected:
	
	// Protected functions to implement AliHLTComponent's interface.
	// These functions provide initialization as well as the actual processing
	// capabilities of the component. 

	virtual int DoInit(int argc, const char** argv);
	virtual int DoDeinit();
	virtual int DoEvent(
			const AliHLTComponentEventData& evtData,
			const AliHLTComponentBlockData* blocks,
			AliHLTComponentTriggerData& trigData,
			AliHLTUInt8_t* outputPtr, 
			AliHLTUInt32_t& size,
			AliHLTComponentBlockDataList& outputBlocks
		);
	virtual bool IgnoreArgument(const char* arg) const;
	
	using AliHLTProcessor::DoEvent;
	
private:

	// Do not allow copying of this class.
	AliHLTMUONEmptyEventFilterComponent(const AliHLTMUONEmptyEventFilterComponent& /*obj*/);
	AliHLTMUONEmptyEventFilterComponent& operator = (const AliHLTMUONEmptyEventFilterComponent& /*obj*/);
	
	bool fSendOnEmpty; //! Flag indicating if we should implement the inverse filter and only send everything if dHLT internal data blocks are empty.

	ClassDef(AliHLTMUONEmptyEventFilterComponent, 0)  // Filter component for empty dHLT events.
};

#endif // ALIHLTMUONEMPTYEVENTFILTERCOMPONENT_H


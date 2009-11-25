#ifndef ALIHLTMUONCHANNELSBLOCKSTRUCT_H
#define ALIHLTMUONCHANNELSBLOCKSTRUCT_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

///
/// @file   AliHLTMUONChannelsBlockStruct.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   18 May 2007
/// @brief  Definition of internal dHLT channels block structure corresponding to clusters.
///
/// The structures are defined with C linkage since C generally gives us more
/// binary compatibility between compilers.
///

#include "AliHLTMUONDataTypes.h"
#include <ostream>

extern "C"
{

/**
 * Gives the fired channel/pad information which was considered during
 * hit reconstruction and correlated into a common cluster.
 */
struct AliHLTMUONChannelStruct
{
	AliHLTInt32_t fClusterId;   // ID corresponding to the cluster this
	                            // channel is part of. -1 == invalid.

	AliHLTUInt16_t fBusPatch;    // Bus patch to which this is connected.
	AliHLTUInt16_t fManu;        // The MANU address on electronics.
	AliHLTUInt16_t fChannelAddress; // The channel address on electronics.
	AliHLTUInt16_t fSignal;      // ADC value of signal.
	AliHLTUInt32_t fRawDataWord; // The raw data word as found in the DDL stream.
};

/**
 * AliHLTMUONChannelBlockStruct defines the format of the internal channel
 * data block corresponding to clusters on the tracking chambers.
 */
struct AliHLTMUONChannelsBlockStruct
{
	AliHLTMUONDataBlockHeader fHeader; // Common data block header

	// Array of cluster channels/pads.
	//AliHLTMUONChannelStruct fChannel[/*fHeader.fNrecords*/];
};

} // extern "C"


/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONChannelStruct in the following format:
 *   {fClusterId = xx, fBusPatch = yy, fManu = zz, fChannelAddress = uu, fSignal = ww,
 *    fRawDataWord = 0xXXXXXXXX}
 */
std::ostream& operator << (
		std::ostream& stream, const AliHLTMUONChannelStruct& channel
	);

/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONChannelsBlockStruct in the following format:
 *   {fHeader = xx, fChannel[] = [{..}, {..}, ...]}
 */
std::ostream& operator << (
		std::ostream& stream, const AliHLTMUONChannelsBlockStruct& block
	);


inline bool operator == (
		const AliHLTMUONChannelStruct& a,
		const AliHLTMUONChannelStruct& b
	)
{
	return a.fClusterId == b.fClusterId and a.fBusPatch == b.fBusPatch and
		a.fManu == b.fManu and a.fChannelAddress == b.fChannelAddress and
		a.fSignal == b.fSignal and a.fRawDataWord == b.fRawDataWord;
}

inline bool operator != (
		const AliHLTMUONChannelStruct& a,
		const AliHLTMUONChannelStruct& b
	)
{
	return not operator == (a, b);
}


bool operator == (
		const AliHLTMUONChannelsBlockStruct& a,
		const AliHLTMUONChannelsBlockStruct& b
	);

inline bool operator != (
		const AliHLTMUONChannelsBlockStruct& a,
		const AliHLTMUONChannelsBlockStruct& b
	)
{
	return not operator == (a, b);
}

#endif // ALIHLTMUONCHANNELSBLOCKSTRUCT_H

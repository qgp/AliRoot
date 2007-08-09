#ifndef ALIHLTMUONCLUSTERSBLOCKSTRUCT_H
#define ALIHLTMUONCLUSTERSBLOCKSTRUCT_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/**
 * @file   AliHLTMUONClustersBlockStruct.h
 * @author Artur Szostak <artursz@iafrica.com>
 * @date   
 * @brief  Definition of internal dimuon HLT block structure containing
 *         debugging information about clusters and their corresponding
 *         reconstructed hits.
 * 
 * The structures are defined with C linkage since C generally gives us more
 * binary compatibility between compilers.
 */

#include "AliHLTMUONDataTypes.h"
#include "AliHLTMUONRecHitsBlockStruct.h"
#include <ostream>

extern "C"
{

/**
 * Debugging information about a cluster and its reconstructed hit.
 */
struct AliHLTMUONClusterStruct
{
	AliHLTInt32_t fId; // Unique ID for the cluster. It must be at
	                   // least unique for any given event. -1 == invalid.

	AliHLTMUONRecHitStruct fHit; // Corresponding reconstructed hit.

	AliHLTInt32_t fDetElemId;  // Detector ID number from AliRoot geometry
	                           // on which the cluster was found.

	AliHLTUInt32_t fNchannels; // Number of channels/pads in the cluster.
};

/**
 * AliHLTMUONClusterBlockStruct defines the format of the internal cluster
 * data block.
 */
struct AliHLTMUONClustersBlockStruct
{
	AliHLTMUONDataBlockHeader fHeader; // Common data block header

	// Array of clusters.
	AliHLTMUONClusterStruct fCluster[/*fHeader.fNrecords*/];
};

} // extern "C"


/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONClusterStruct in the following format:
 *   {fId = xx, fHit = {...}, fDetElemId = yy, fNchannels = zz}
 */
std::ostream& operator << (
		std::ostream& stream, const AliHLTMUONClusterStruct& cluster
	);

/**
 * Stream operator for usage with std::ostream classes which prints the
 * AliHLTMUONClustersBlockStruct in the following format:
 *   {fHeader = xx, fCluster[] = [{..}, {..}, ...]}
 */
std::ostream& operator << (
		std::ostream& stream, const AliHLTMUONClustersBlockStruct& block
	);


inline bool operator == (
		const AliHLTMUONClusterStruct& a,
		const AliHLTMUONClusterStruct& b
	)
{
	return	a.fId == b.fId and a.fHit == b.fHit and
		a.fDetElemId == b.fDetElemId and a.fNchannels == b.fNchannels;
}

inline bool operator != (
		const AliHLTMUONClusterStruct& a,
		const AliHLTMUONClusterStruct& b
	)
{
	return not operator == (a, b);
}


bool operator == (
		const AliHLTMUONClustersBlockStruct& a,
		const AliHLTMUONClustersBlockStruct& b
	);

inline bool operator != (
		const AliHLTMUONClustersBlockStruct& a,
		const AliHLTMUONClustersBlockStruct& b
	)
{
	return not operator == (a, b);
}

#endif // ALIHLTMUONCLUSTERSBLOCKSTRUCT_H

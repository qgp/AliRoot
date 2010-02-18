/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Artur Szostak <artursz@iafrica.com>                                  *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id: AliHLTMUONTracksBlockStruct.cxx 36627 2009-11-10 19:21:49Z aszostak $

///
/// @file   AliHLTMUONTracksBlockStruct.cxx
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   10 Feb 2010
/// @brief  Implementation of useful stream and comparison operators.
///
/// The tracks data block is an internal dimuon HLT data block structure
/// generated by the tracker components.
///

#include "AliHLTMUONTracksBlockStruct.h"
#include "AliHLTMUONUtils.h"
#include <cassert>


std::ostream& operator << (
		std::ostream& stream, const AliHLTMUONTrackStruct& track
	)
{
	stream	<< "{fId = " << track.fFlags
		<< ", fTrigRec = " << track.fTrigRec
		<< ", fFlags = " << std::showbase << std::hex
		<< track.fFlags << std::dec
		<< ", fPx = " << track.fPx
		<< ", fPy = " << track.fPy
		<< ", fPz = " << track.fPz
		<< ", fInverseBendingMomentum = " << track.fInverseBendingMomentum
		<< ", fThetaX = " << track.fThetaX
		<< ", fThetaX = " << track.fThetaY
		<< ", fX = " << track.fX
		<< ", fY = " << track.fY
		<< ", fZ = " << track.fZ
		<< ", fChi2 = " << track.fChi2;
	for (AliHLTUInt32_t i = 0; i < 16; i++)
	{
		stream << ", fHit[" << i << "] = " << track.fHit[0];
	}
	stream << "}";
	return stream;
}


std::ostream& operator << (
		std::ostream& stream,
		const AliHLTMUONTracksBlockStruct& block
	)
{
	assert( AliHLTMUONUtils::IntegrityOk(block) );

	const AliHLTMUONTrackStruct* track =
		reinterpret_cast<const AliHLTMUONTrackStruct*>(&block + 1);
	stream 	<< "{fHeader = " << block.fHeader << ", fTrack[] = [";
	if (block.fHeader.fNrecords > 0) stream << track[0];
	for (AliHLTUInt32_t i = 1; i < block.fHeader.fNrecords; i++)
		stream << ", " << track[i];
	stream << "]}";
	return stream;
}


bool operator == (
		const AliHLTMUONTrackStruct& a,
		const AliHLTMUONTrackStruct& b
	)
{
	bool result = a.fId == b.fId and a.fTrigRec == b.fTrigRec
		and a.fFlags == b.fFlags and a.fPx == b.fPx and a.fPy == b.fPy
		and a.fPz == b.fPz and a.fInverseBendingMomentum == b.fInverseBendingMomentum
		and a.fThetaX == b.fThetaX and a.fThetaY == b.fThetaY
		and a.fX == b.fX and a.fY == b.fY and a.fZ == b.fZ and a.fChi2 == b.fChi2;
	for (AliHLTUInt32_t i = 0; i < 16; i++)
	{
		result &= (a.fHit[i] == b.fHit[i]);
	}
	return result;
}


bool operator == (
		const AliHLTMUONTracksBlockStruct& a,
		const AliHLTMUONTracksBlockStruct& b
	)
{
	assert( AliHLTMUONUtils::IntegrityOk(a) );
	assert( AliHLTMUONUtils::IntegrityOk(b) );

	const AliHLTMUONTrackStruct* trackA =
		reinterpret_cast<const AliHLTMUONTrackStruct*>(&a + 1);
	const AliHLTMUONTrackStruct* trackB =
		reinterpret_cast<const AliHLTMUONTrackStruct*>(&b + 1);
	
	// First check if the blocks have the same header. If they do then check
	// if every track is the same. In either case if we find a difference
	// return false.
	if (a.fHeader != b.fHeader) return false;
	for (AliHLTUInt32_t i = 0; i < a.fHeader.fNrecords; i++)
		if (trackA[i] != trackB[i]) return false;
	return true;
}

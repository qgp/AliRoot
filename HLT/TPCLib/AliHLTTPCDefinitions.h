// XEmacs -*-C++-*-
// @(#) $Id$

#ifndef ALIHLTTPCDEFINITIONS_H
#define ALIHLTTPCDEFINITIONS_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

#include "AliHLTDataTypes.h"
#include "Rtypes.h"

/**
 * @class AliHLTTPCDefinitions
 * Data type definitions for the libAliHLTTPC library.
 * 
 * @ingroup alihlt_tpc
 */
class AliHLTTPCDefinitions
{
public:

	static AliHLTUInt8_t GetMinSliceNr( const AliHLTComponentBlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x00FF0000) >> 16 );
		}
	static AliHLTUInt8_t GetMinSliceNr( ULong_t spec )
		{
		return (AliHLTUInt8_t)( (spec & 0x00FF0000) >> 16 );
		}
	static AliHLTUInt8_t GetMaxSliceNr( const AliHLTComponentBlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0xFF000000) >> 24 );
		}
	static AliHLTUInt8_t GetMaxSliceNr( ULong_t spec )
		{
		return (AliHLTUInt8_t)( (spec & 0xFF000000) >> 24 );
		}
	static AliHLTUInt8_t GetMinPatchNr( const AliHLTComponentBlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x000000FF) );
		}
	static AliHLTUInt8_t GetMinPatchNr( ULong_t spec )
		{
		return (AliHLTUInt8_t)( (spec & 0x000000FF) );
		}
	static AliHLTUInt8_t GetMaxPatchNr( const AliHLTComponentBlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x0000FF00) >> 8 );
		}
	static AliHLTUInt8_t GetMaxPatchNr( ULong_t spec )
		{
		return (AliHLTUInt8_t)( (spec & 0x0000FF00) >> 8 );
		}
	
	static AliHLTUInt32_t EncodeDataSpecification( AliHLTUInt8_t minSliceNr, 
						AliHLTUInt8_t maxSliceNr,
						AliHLTUInt8_t minPatchNr,
						AliHLTUInt8_t maxPatchNr )
		{
		return ((maxSliceNr & 0xFF) << 24) | ((minSliceNr & 0xFF) << 16) | ((maxPatchNr & 0xFF) << 8) | ((minPatchNr & 0xFF));
		}
	
	/**
	 * Converts a slice and patch number to a DDL ID number for TPC.
	 * \param slice  The slice number in the range [0..35] (0..17 for A side and 18..35 for C side).
	 * \param patch  The patch number in the range [0..5].
	 * \returns the DDL ID number of TPC or -1 if the slice or patch was invalid.
	 * \note A side is in the -z axis direction (same side as the muon spectrometer)
	 *       and C side is in the +z axis direction.
	 */
	static AliHLTInt32_t SlicePatchToDDLId(AliHLTUInt8_t slice, AliHLTUInt8_t patch)
		{
		if (slice > 35 or patch > 5) return -1;
		return 768 + (patch > 1 ? 72 + 4*slice + patch - 2 : 2*slice + patch);
		}
		
	/**
	 * Converts a DDL ID number for the TPC to a slice and patch number.
	 * [in] \param ddlid  The DDL ID number to convert.
	 * [out] \param slice  The resultant slice number in the range [0..35].
	 * [out] \param patch  The resultant patch number in the range [0..5].
	 * \returns true if the DDL ID number was valid and slice and patch were set,
	 *     otherwise false for an invalid DDL ID.
	 */
	static bool DDLIdToSlicePatch(AliHLTInt32_t ddlid, AliHLTUInt8_t& slice, AliHLTUInt8_t& patch);

  /** DDL packed RAW data */
  static const AliHLTComponentDataType fgkDDLPackedRawDataType;         // see above
  /** DDL entropy encoded data */
  static const AliHLTComponentDataType fgkDDLEncodedEntropyRawDataType; // see above
  /** packed RAW data */
  static const AliHLTComponentDataType fgkPackedRawDataType;            // see above
  /** unpacked RAW data */
  static const AliHLTComponentDataType fgkUnpackedRawDataType;          // see above
  /** cluster data */
  static const AliHLTComponentDataType fgkClustersDataType;             // see above
  /** raw cluster data (not transformed) */
  static const AliHLTComponentDataType fgkRawClustersDataType;          // see above
  /** HW cluster data */
  static const AliHLTComponentDataType fgkHWClustersDataType;           // see above
  /** HW alternative output cluster data */
  static const AliHLTComponentDataType fgkAlterClustersDataType;        // see above
  /** track segments in local coordinates */
  static const AliHLTComponentDataType fgkTrackSegmentsDataType;        // see above
  /** tracks in global koordinates */
  static const AliHLTComponentDataType fgkTracksDataType;               // see above
  /** vertex data structure */
  static const AliHLTComponentDataType fgkVertexDataType;               // see above

  // Cluster & Tracks model data
  /** cluster tracks model data type */
  static const AliHLTComponentDataType fgkClusterTracksModelDataType;          // see above
  /** remaining clusters model data type */
  static const AliHLTComponentDataType fgkRemainingClustersModelDataType;      // see above
  /** cluster tracks compressed data type */
  static const AliHLTComponentDataType fgkClusterTracksCompressedDataType;     // see above
  /** remaining clusters compressed data type */
  static const AliHLTComponentDataType fgkRemainingClustersCompressedDataType; // see above

  // Calibration data
  /** pedestal calibration data */
  static const AliHLTComponentDataType fgkCalibPedestalDataType;   // see above
  /** signal calibration data */
  static const AliHLTComponentDataType fgkCalibPulserDataType;     // see above
  /** central electrode calibration data */
  static const AliHLTComponentDataType fgkCalibCEDataType;         // see above

  // offline calbration components

  /** alignment calibration data */
  static const AliHLTComponentDataType fgkOfflineCalibAlignDataType;         // see above
  /** track calibration data */
  static const AliHLTComponentDataType fgkOfflineCalibTracksDataType;        // see above
  /** gain calibration data */
  static const AliHLTComponentDataType fgkOfflineCalibTracksGainDataType;    // see above
  /** cluster monte carlo information */
  static const AliHLTComponentDataType fgkAliHLTDataTypeClusterMCInfo;    // see above

private:

  /// Do not allow creation of this class since everything is static.
  AliHLTTPCDefinitions();
  virtual ~AliHLTTPCDefinitions();

  ClassDef(AliHLTTPCDefinitions, 0)  // Useful static definitions and methods for TPC
};

#endif

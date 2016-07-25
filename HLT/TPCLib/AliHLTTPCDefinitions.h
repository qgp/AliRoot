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
	static Int_t GetSingleSliceNr( const AliHLTComponentBlockData& block )
		{
		return GetSingleSliceNr(block.fSpecification);
		}
	static Int_t GetSingleSliceNr( ULong_t spec );
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
	static Int_t GetSinglePatchNr( const AliHLTComponentBlockData& block )
		{
		return GetSinglePatchNr(block.fSpecification);
		}
	static Int_t GetSinglePatchNr( ULong_t spec );
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
	 * \param [in] ddlid  The DDL ID number to convert.
	 * \param [out] slice  The resultant slice number in the range [0..35].
	 * \param [out] patch  The resultant patch number in the range [0..5].
	 * \returns true if the DDL ID number was valid and slice and patch were set,
	 *     otherwise false for an invalid DDL ID.
	 */
	static bool DDLIdToSlicePatch(AliHLTInt32_t ddlid, AliHLTUInt8_t& slice, AliHLTUInt8_t& patch);

  /** DDL entropy encoded data */
  static const AliHLTComponentDataType fgkDDLEncodedEntropyRawDataType; // see above
  static const AliHLTComponentDataType& DDLEncodedEntropyRawDataType();
  /** packed RAW data */
  static const AliHLTComponentDataType fgkPackedRawDataType;            // see above
  static const AliHLTComponentDataType& PackedRawDataType();
  /** unpacked RAW data */
  static const AliHLTComponentDataType fgkUnpackedRawDataType;          // see above
  static const AliHLTComponentDataType& UnpackedRawDataType();
  /** cluster data */
  static const AliHLTComponentDataType fgkClustersDataType;             // see above
  static const AliHLTComponentDataType& ClustersDataType();
  /** xyz cluster coordinates */
  static const AliHLTComponentDataType fgkClustersXYZDataType;          // see above
  static const AliHLTComponentDataType& ClustersXYZDataType();
  /** raw cluster data (not transformed) */
  static const AliHLTComponentDataType fgkRawClustersDataType;          // see above
  static const AliHLTComponentDataType fgkRawClustersDataTypeNotCompressed;          // A copy of the fgkRawClustersDataType, which is used if the DataCompressor failed to compress clusters
  static const AliHLTComponentDataType& RawClustersDataType();
  static const AliHLTComponentDataType& RawClustersDataTypeNotCompressed();
  /** raw cluster data descriptor*/
  static const AliHLTComponentDataType fgkRawClustersDescriptorDataType;          // see above
  static const AliHLTComponentDataType& RawClustersDescriptorDataType();
  /** HW cluster data */
  static const AliHLTComponentDataType fgkHWClustersDataType;           // see above
  static const AliHLTComponentDataType& HWClustersDataType(); 
  /** track segments in local coordinates */
  static const AliHLTComponentDataType fgkTrackSegmentsDataType;        // see above
  static const AliHLTComponentDataType& TrackSegmentsDataType();
  /** tracks in global koordinates */
  static const AliHLTComponentDataType fgkTracksDataType;               // see above
  static const AliHLTComponentDataType& TracksDataType();
  /** vertex data structure */
  static const AliHLTComponentDataType fgkVertexDataType;               // see above
  static const AliHLTComponentDataType& VertexDataType();
  
  //Initialized Fast transform data object
  static const AliHLTComponentDataType fgkTPCFastTransformDataObjectDataType;	//see above
  static const AliHLTComponentDataType &TPCFastTransformDataObjectDataType(){ return fgkTPCFastTransformDataObjectDataType; }	//see above
  static const AliHLTComponentDataType fgkTPCReverseTransformInfoDataType; //
  static const AliHLTComponentDataType &TPCReverseTransformInfoDataType(){ return fgkTPCReverseTransformInfoDataType; }	//see above

  // Cluster & Tracks model data
  /** data compression descriptor*/
  static const AliHLTComponentDataType fgkDataCompressionDescriptorDataType;          // see above
  static const AliHLTComponentDataType& DataCompressionDescriptorDataType();
  /** cluster tracks model data type */
  static const AliHLTComponentDataType fgkClusterTracksModelDataType;          // see above
  static const AliHLTComponentDataType& ClusterTracksModelDataType();
  /** remaining clusters model data type */
  static const AliHLTComponentDataType fgkRemainingClustersModelDataType;      // see above
  static const AliHLTComponentDataType& RemainingClustersModelDataType();
  /** track clusters compressed data type */
  static const AliHLTComponentDataType fgkClusterTracksCompressedDataType;     // see above
  static const AliHLTComponentDataType& ClusterTracksCompressedDataType();
  /** track cluster ids data type */
  static const AliHLTComponentDataType& ClusterIdTracksDataType();
  /** compressed cluster ids data type */
  static const AliHLTComponentDataType& CompressedClusterIdDataType();
  /** remaining clusters compressed data type */
  static const AliHLTComponentDataType fgkRemainingClustersCompressedDataType; // see above
  static const AliHLTComponentDataType& RemainingClustersCompressedDataType();
  /** remaining clusters ids data type */
  static const AliHLTComponentDataType& RemainingClusterIdsDataType();
  /** Data type to transport optional flags, like split cluster flag */
  static const AliHLTComponentDataType fgkClustersFlagsDataType;          // see above
  static const AliHLTComponentDataType& ClustersFlagsDataType();

  // Calibration data
  /** pedestal calibration data */
  static const AliHLTComponentDataType& CalibPedestalDataType();
  /** signal calibration data */
  static const AliHLTComponentDataType& CalibPulserDataType();
  /** central electrode calibration data */
  static const AliHLTComponentDataType& CalibCEDataType();

  // offline calbration components

  /** alignment calibration data */
  static const AliHLTComponentDataType& OfflineCalibAlignDataType();
  /** track calibration data */
  static const AliHLTComponentDataType& OfflineCalibTracksDataType();
  /** gain calibration data */
  static const AliHLTComponentDataType& OfflineCalibTracksGainDataType();
  /** cluster monte carlo information */
  static const AliHLTComponentDataType fgkAliHLTDataTypeClusterMCInfo;    // see above
  static const AliHLTComponentDataType& AliHLTDataTypeClusterMCInfo();

  // ids for the different parameters of a cluster
  enum AliClusterParameterId_t {
    kPadRow = 0,
    kPad,
    kTime,
    kSigmaY2,
    kSigmaZ2,
    kCharge,
    kQMax,
    kResidualPad,
    kResidualTime,
    kClusterCount,
    kLast = kQMax
  };

  // helper struct for the definition of cluster parameters
  struct AliClusterParameter {
    AliClusterParameterId_t fId; //! id of the parameter
    const char* fName;           //! name of the parameter
    int fBitLength;              //! bit length
    int fOptional;               //! optional parameter
    int fScale;                  //! scale for conversion to int number
  };

  static const AliClusterParameter fgkClusterParameterDefinitions[];
  static unsigned GetNumberOfClusterParameterDefinitions();
  static const unsigned fgkMaxClusterDeltaPad;
  static unsigned GetMaxClusterDeltaPad() {return fgkMaxClusterDeltaPad;}
  static const unsigned fgkMaxClusterDeltaTime;
  static unsigned GetMaxClusterDeltaTime() {return fgkMaxClusterDeltaTime;}

private:

  /// Do not allow creation of this class since everything is static.
  AliHLTTPCDefinitions();
  virtual ~AliHLTTPCDefinitions();

  ClassDef(AliHLTTPCDefinitions, 0)  // Useful static definitions and methods for TPC
};

#endif

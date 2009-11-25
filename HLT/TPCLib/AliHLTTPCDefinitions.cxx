// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  Timm Steinbeck <timm@kip.uni-heidelberg.de>           *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Definitions for the HLT TPC components                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliHLTTPCDefinitions.h"


/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCDefinitions)

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkDDLPackedRawDataType = AliHLTComponentDataTypeInitializer("DDL_RWPK", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkDDLEncodedEntropyRawDataType = AliHLTComponentDataTypeInitializer("DDLENCEN", kAliHLTDataOriginTPC);
										      								  	      
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkPackedRawDataType = AliHLTComponentDataTypeInitializer("RAWPAKED", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkUnpackedRawDataType = AliHLTComponentDataTypeInitializer("RAWUNPAK", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkClustersDataType = AliHLTComponentDataTypeInitializer("CLUSTERS", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkHWClustersDataType = AliHLTComponentDataTypeInitializer("HWCLUST1", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkAlterClustersDataType = AliHLTComponentDataTypeInitializer("HWCL_ALT", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkVertexDataType = AliHLTComponentDataTypeInitializer("VERTEX  ", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkTrackSegmentsDataType = AliHLTComponentDataTypeInitializer("TRAKSEGS", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkTracksDataType = AliHLTComponentDataTypeInitializer("TRACKS  ", kAliHLTDataOriginTPC);

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkClusterTracksModelDataType = AliHLTComponentDataTypeInitializer("CLSTRKMD", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkRemainingClustersModelDataType = AliHLTComponentDataTypeInitializer("REMCLSMD", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkClusterTracksCompressedDataType = AliHLTComponentDataTypeInitializer("CLSTRKCM", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkRemainingClustersCompressedDataType = AliHLTComponentDataTypeInitializer("REMCLSCM", kAliHLTDataOriginTPC);
										      								  	      
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkCalibPedestalDataType = AliHLTComponentDataTypeInitializer("CAL_PED ", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkCalibPulserDataType = AliHLTComponentDataTypeInitializer("CAL_PULS", kAliHLTDataOriginTPC);
const AliHLTComponentDataType AliHLTTPCDefinitions::fgkCalibCEDataType = AliHLTComponentDataTypeInitializer("CAL_CE  ", kAliHLTDataOriginTPC);

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkOfflineCalibAlignDataType = AliHLTComponentDataTypeInitializer("CALALIGN", kAliHLTDataOriginTPC);

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkOfflineCalibTracksDataType = AliHLTComponentDataTypeInitializer("CALTRACK", kAliHLTDataOriginTPC);

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkOfflineCalibTracksGainDataType = AliHLTComponentDataTypeInitializer("CALGAIN ", kAliHLTDataOriginTPC);

const AliHLTComponentDataType AliHLTTPCDefinitions::fgkAliHLTDataTypeClusterMCInfo = AliHLTComponentDataTypeInitializer("CLMCINFO", kAliHLTDataOriginTPC);


AliHLTTPCDefinitions::AliHLTTPCDefinitions()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCDefinitions::~AliHLTTPCDefinitions()
{
  // see header file for class documentation
}

    

// $Id$

/**************************************************************************
 * Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Authors: Per Thomas Hille <perthi@fys.uio.no>, after                   * 
 *          Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          for the ALICE Offline Project.                                *
 *	                                                                  *
 * Contributors are mentioned in the code where appropriate.              *
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
// Definitions for the HLT PHOS components                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliHLTPHOSDefinitions.h"


const AliHLTComponentDataType AliHLTPHOSDefinitions::gkDDLPackedRawDataType          = { sizeof(AliHLTComponentDataType),       {'D','D','L','_','R','W','P','K'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellEnergyDataType            = { sizeof(AliHLTComponentDataType),       {'C','E','L','L','E','N','E','R'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellEnergyHistogramDataType   = { sizeof(AliHLTComponentDataType),       {'E','N','E','R','H','I','S','T'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellAverageEnergyDataType     = { sizeof(AliHLTComponentDataType),       {'E','N','E','R','A','V','E','R'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellAccumulatedEnergyDataType = { sizeof(AliHLTComponentDataType),       {'E','N','E','R','A','C','C','U'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellTimingHistogramDataType   = { sizeof(AliHLTComponentDataType),       {'T','I','M','E','H','I','S','T'},{'P','H','O','S'}};;    
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellTimingAverageDataType     = { sizeof(AliHLTComponentDataType),       {'T','I','M','E','A','V','E','R'},{'P','H','O','S'}};;
const AliHLTComponentDataType AliHLTPHOSDefinitions::gkCellChannelDataDataType       = { sizeof(AliHLTComponentDataType),       {'C','H','A','N','D','A','T','A'},{'P','H','O','S'}};;

// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          for The ALICE Off-line Project.                               *
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
// Definitions for the HLT TRD components                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliHLTTRDDefinitions.h"


ClassImp(AliHLTTRDDefinitions)

const AliHLTComponentDataType AliHLTTRDDefinitions::gkDDLRawDataType = { sizeof(AliHLTComponentDataType), {'D','D','L','_','R','A','W',' '},{'T','R','D',' '}};;

const AliHLTComponentDataType AliHLTTRDDefinitions::gkClusterDataType = { sizeof(AliHLTComponentDataType), {'C','L','U','S','T','E','R','S'},{'T','R','D',' '}};;

const AliHLTComponentDataType AliHLTTRDDefinitions::gkTRDSATracksDataType = { sizeof(AliHLTComponentDataType), {'T','R','A','C','K','S','S','A'},{'T','R','D',' '}};;

const AliHLTComponentDataType AliHLTTRDDefinitions::gkMCMtrackletDataType = { sizeof(AliHLTComponentDataType), {'M','C','M','T','R','L','E','T'},{'T','R','D',' '}};;

const AliHLTComponentDataType AliHLTTRDDefinitions::gkMCMcalibrationDataType = { sizeof(AliHLTComponentDataType), {'M','C','M','C','A','L','I','H'},{'T','R','D',' '}};;


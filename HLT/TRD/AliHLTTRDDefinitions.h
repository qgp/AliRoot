//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTTRDDEFINITIONS_H
#define ALIHLTTRDDEFINITIONS_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/* AliHLTTRDDefinitions
 */

#include "AliHLTDataTypes.h"
#include "Rtypes.h"

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  The HLT definitions for TRD                                              //  
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


class AliHLTTRDDefinitions
{

public:
  AliHLTTRDDefinitions();
  virtual ~AliHLTTRDDefinitions();
  
  static const AliHLTComponentDataType fgkDigitsDataType;  // TRD digits 
  static const AliHLTComponentDataType fgkClusterDataType; // Cluster
  static const AliHLTComponentDataType fgkHiLvlClusterDataType; // Cluster for offline comparation
  static const AliHLTComponentDataType fgkTracksDataType; // Stand Alone tracks
  static const AliHLTComponentDataType fgkHiLvlTracksDataType; // Stand Alone tracks for offline comparation
  static const AliHLTComponentDataType fgkMCMtrackletDataType; // MCM tracklet Data
  static const AliHLTComponentDataType fgkMCMcalibrationDataType; // MCM Calibration data
  static const AliHLTComponentDataType fgkCalibrationDataType; // Calibration with TRDtracks
  static const AliHLTComponentDataType fgkEORCalibrationDataType;//Calibration end of run 

  ClassDef(AliHLTTRDDefinitions, 0)
    
};

#endif


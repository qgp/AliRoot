// XEmacs -*-C++-*-
#ifndef ALIHLTEMCALDEFINITIONS_H
#define ALIHLTEMCALDEFINITIONS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* AliHLTEMCALDefinitions
 */

#include "AliHLTDataTypes.h"
#include "Rtypes.h"

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  The HLT definitions for EMCAL                                            //  
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


class AliHLTEMCALDefinitions
{

public:
  AliHLTEMCALDefinitions();
  virtual ~AliHLTEMCALDefinitions();
  
  static const AliHLTComponentDataType fgkDDLRawDataType; // Raw Data
  static const AliHLTComponentDataType fgkDigitDataType; // EMCAL Digits
  static const AliHLTComponentDataType fgkClusterDataType; // EMCAL Clusters
  static const AliHLTComponentDataType fgkESDDataType; // global ESD data type - may change!!!
  static const AliHLTComponentDataType fgkEMCALESDDataType; // ESD data type after emcal processing
  static const AliHLTComponentDataType fgkCalibrationDataType; // Calibration
  static const AliHLTComponentDataType fgkChannelDataType;
  ClassDef(AliHLTEMCALDefinitions, 0)
    
};

#endif

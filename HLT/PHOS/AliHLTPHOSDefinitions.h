
// XEmacs -*-C++-*-
// @(#) $Id$

#ifndef ALIHLTPHOSDEFINITIONS_H
#define ALIHLTPHOSDEFINITIONS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* AliHLTPHOSDefinitions
 */

#include "AliHLTDataTypes.h"
//#include "Rtypes.h"

class AliHLTPHOSDefinitions
    {
    public:
      static const AliHLTComponentDataType fgkCellEnergyDataType;    /**<Reconstructed cell/crystal energies*/
      static const AliHLTComponentDataType fgkDDLPackedRawDataType;  /**<DDL raw data on the RCU data format*/
      static const AliHLTComponentDataType fgkCellEnergyHistogramDataType;  /**<Histogram for per cell/gain energy distribution*/
      static const AliHLTComponentDataType fgkCellAverageEnergyDataType;  /**<Histogram for per cell/gain energy distribution*/
      static const AliHLTComponentDataType fgkCellAccumulatedEnergyDataType;  /**<Histogram for per cell/gain energy distribution*/
      static const AliHLTComponentDataType fgkCellTimingHistogramDataType;  /**<Histogram for per cell/gain time distribution*/      
      static const AliHLTComponentDataType fgkCellTimingAverageDataType;  /**<Histogram for per cell/gain time distribution*/  
      static const AliHLTComponentDataType fgkCellChannelDataDataType;  /**<Time dependent signal from the readout channels*/  
    };

#endif

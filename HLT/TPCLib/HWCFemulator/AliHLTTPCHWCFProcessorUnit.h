//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTTPCHWCFPROCESSORUNIT_H
#define ALIHLTTPCHWCFPROCESSORUNIT_H

//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *


#include "AliHLTDataTypes.h"
#include "AliHLTTPCHWCFDataTypes.h"


//  @class AliHLTTPCHWCFProcessorUnit
//  @author Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de>
//  @author Torsten Alt <talt@cern.ch> 
//  @brief  Channel Processor unit of FPGA ClusterFinder Emulator for TPC
//  @brief  ( see AliHLTTPCHWCFEmulator class )
//  @note
//
class AliHLTTPCHWCFProcessorUnit
{
 public:

  /** standard constructor */
  AliHLTTPCHWCFProcessorUnit();
  
  /** destructor */
  ~AliHLTTPCHWCFProcessorUnit();

  /** set debug level */
  void SetDebugLevel( int val ){ fDebug = val; }

  /** do cluster deconvolution in time direction */
  void SetDeconvolution( bool val ){ fDeconvolute = val; }

  /** lower charge limit for isolated signals
   */
  void SetSingleSeqLimit( AliHLTUInt32_t val ){ 
    fSingleSeqLimit = val << AliHLTTPCHWCFDefinitions::kFixedPoint; 
  }

  /** max. size of the cluster in time bins
   */
  void SetTimeBinWindow( AliHLTUInt32_t val ){ 
    fTimeBinWindow = val>=1 ?val/2*2+1 :1;
  }


  /** initialise */
  int Init();
  
  /** input stream of data */
  int InputStream( const AliHLTTPCHWCFBunch *bunch );

  /** output stream of data */
  const AliHLTTPCHWCFClusterFragment *OutputStream();
  
 private: 

  /** copy constructor prohibited */
  AliHLTTPCHWCFProcessorUnit(const AliHLTTPCHWCFProcessorUnit&);
  /** assignment operator prohibited */
  AliHLTTPCHWCFProcessorUnit& operator=(const AliHLTTPCHWCFProcessorUnit&);  
  

  AliHLTTPCHWCFClusterFragment fOutput; // current output
  const AliHLTTPCHWCFBunch *fkBunch; // current input
  AliHLTUInt32_t fBunchIndex; // index in bunch
  bool fDeconvolute;    // do deconvolution in time direction
  AliHLTUInt64_t fSingleSeqLimit; // lower charge limit for isolated signals
  AliHLTUInt32_t fTimeBinWindow; // max. size of the cluster in time bins 
  int fDebug; // debug level
};

#endif

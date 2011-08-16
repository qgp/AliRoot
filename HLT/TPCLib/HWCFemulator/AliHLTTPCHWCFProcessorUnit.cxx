// $Id$
//****************************************************************************
//* This file is property of and copyright by the ALICE HLT Project          * 
//* ALICE Experiment at CERN, All rights reserved.                           *
//*                                                                          *
//* Primary Authors: Sergey Gorbunov, Torsten Alt                            *
//* Developers:      Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de> *
//*                  Torsten Alt <talt@cern.ch>                              *
//*                  for The ALICE HLT Project.                              *
//*                                                                          *
//* Permission to use, copy, modify and distribute this software and its     *
//* documentation strictly for non-commercial purposes is hereby granted     *
//* without fee, provided that the above copyright notice appears in all     *
//* copies and that both the copyright notice and this permission notice     *
//* appear in the supporting documentation. The authors make no claims       *
//* about the suitability of this software for any purpose. It is            *
//* provided "as is" without express or implied warranty.                    *
//****************************************************************************

//  @file   AliHLTTPCHWCFProcessorUnit.cxx
//  @author Sergey Gorbunov <sergey.gorbunov@fias.uni-frankfurt.de>
//  @author Torsten Alt <talt@cern.ch> 
//  @date   
//  @brief  Channel Processor unit of FPGA ClusterFinder Emulator for TPC
//  @brief  ( see AliHLTTPCHWCFEmulator class )
//  @note 

#include "AliHLTTPCHWCFProcessorUnit.h"
#include <iostream>
#include <cstdio>


AliHLTTPCHWCFProcessorUnit::AliHLTTPCHWCFProcessorUnit()
  :
  fOutput(),
  fkBunch(0),
  fBunchIndex(0),
  fDeconvolute(0),
  fSingleSeqLimit(0),
  fTimeBinWindow(5),
  fDebug(0)
{
  //constructor 
  Init();
}


AliHLTTPCHWCFProcessorUnit::~AliHLTTPCHWCFProcessorUnit()
{   
  //destructor 
}

AliHLTTPCHWCFProcessorUnit::AliHLTTPCHWCFProcessorUnit(const AliHLTTPCHWCFProcessorUnit&)
  :
  fOutput(),
  fkBunch(0),
  fBunchIndex(0),
  fDeconvolute(0),
  fSingleSeqLimit(0),
  fTimeBinWindow(5),
  fDebug(0)
{
  // dummy
  Init();
}

AliHLTTPCHWCFProcessorUnit& AliHLTTPCHWCFProcessorUnit::operator=(const AliHLTTPCHWCFProcessorUnit&)
{
  // dummy  
  return *this;
}

int AliHLTTPCHWCFProcessorUnit::Init()
{
  // initialise  

  fkBunch = 0;
  return 0;
}

int AliHLTTPCHWCFProcessorUnit::InputStream( const AliHLTTPCHWCFBunch *bunch )
{
  // input stream of data 
  
  if( bunch && fDebug ){
    printf("\nHWCF Processor: input bunch F %1d R %3d P %3d T %3d NS %2ld:\n",
	   bunch->fFlag, bunch->fRow, bunch->fPad, bunch->fTime, bunch->fData.size());
    for( unsigned int i=0; i<bunch->fData.size(); i++ ){
      printf("   %2d ", bunch->fData[i]);
      if( i < bunch->fMC.size() ){
	printf("(");
	for( int j=0; j<3; j++ ) printf(" {%d,%2.0f}",bunch->fMC[i].fClusterID[j].fMCID, bunch->fMC[i].fClusterID[j].fWeight );
	printf(" )\n");
      }
    }
  }

  fkBunch = bunch;
  fBunchIndex = 0;
  return 0;
}

const AliHLTTPCHWCFClusterFragment *AliHLTTPCHWCFProcessorUnit::OutputStream()
{ 
  // output stream of data 

  if( !fkBunch ) return 0;
  
  fOutput.fFlag = fkBunch->fFlag;
  fOutput.fRow = fkBunch->fRow;
  fOutput.fPad = fkBunch->fPad;
  fOutput.fBranch = fkBunch->fBranch;
  fOutput.fBorder = fkBunch->fBorder;
  fOutput.fQmax = 0;
  fOutput.fQ = 0;
  fOutput.fT = 0;
  fOutput.fT2 = 0;
  fOutput.fP = 0;
  fOutput.fP2 = 0;
  fOutput.fTMean =  fkBunch->fTime;  
  fOutput.fMC.clear();
  
  if( fkBunch->fFlag==2 && fkBunch->fData.size()==1 ){ // rcu trailer word, forward it 
    fOutput.fRow = fkBunch->fData[0];
  }	
  
  if( fkBunch->fFlag >1 ){
    fkBunch = 0;
    fBunchIndex = 0;
    return &fOutput;
  }

  if( fkBunch->fFlag < 1 ) return 0;

  if( fBunchIndex >= fkBunch->fData.size() || fkBunch->fTime < fBunchIndex ) return 0;  

  AliHLTInt32_t bunchTime0 = fkBunch->fTime - fBunchIndex;
  AliHLTInt32_t bunchTime = bunchTime0;

  AliHLTUInt32_t qArrStart = fBunchIndex, qArrEnd = qArrStart;
  AliHLTUInt32_t qLast = 0;
  bool slope = 0;
  AliHLTUInt32_t length = 0;

  for( ; qArrEnd<fkBunch->fData.size() && bunchTime>=0; qArrEnd++, bunchTime--, length++ ){
    AliHLTUInt32_t q = fkBunch->fData[qArrEnd];
    if( ( slope && length+1 >= fTimeBinWindow ) ||
	( fDeconvolute && slope && q>qLast )     ){ // split the cluster
      qArrEnd++;
      break;
    }
    
    if( q<qLast && !slope ){ // local maximum
      if( length > fTimeBinWindow/2 ){
	length = fTimeBinWindow/2;
	qArrStart = qArrEnd - length;
      }      
      slope = 1;     
    }
    qLast = q;
  }

  fBunchIndex = qArrStart;
  bunchTime0 = fkBunch->fTime - fBunchIndex; 
  bunchTime = bunchTime0;
  length = 0;
  
  for( ; fBunchIndex<qArrEnd; fBunchIndex++, bunchTime--, length++ ){
    AliHLTUInt64_t q = fkBunch->fData[fBunchIndex]*fkBunch->fGain;
    if (fOutput.fQmax < q) fOutput.fQmax = q;
    fOutput.fQ += q;
    fOutput.fT += q*bunchTime;
    fOutput.fT2+= q*bunchTime*bunchTime;
    fOutput.fP += q*fkBunch->fPad;
    fOutput.fP2+= q*fkBunch->fPad*fkBunch->fPad;
    if( fBunchIndex<fkBunch->fMC.size() ){
      fOutput.fMC.push_back(fkBunch->fMC[fBunchIndex]);
    }
  }  
 
  fOutput.fTMean = (AliHLTUInt64_t)( (bunchTime0 + bunchTime + 1)/2 );

  if( length==1 && fOutput.fQ < fSingleSeqLimit ) return 0;

  return &fOutput;
}

//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTTRDOFFLINECLUSTERIZERCOMPONENT_H
#define ALIHLTTRDOFFLINECLUSTERIZERCOMPONENT_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTTRDOfflineClusterizerComponent.h
    @author 
    @date   2009-08-31
    @brief  
*/

#include "AliHLTTRDClusterizerComponent.h"

class AliHLTTRDOfflineClusterizerComponent : public AliHLTTRDClusterizerComponent
{
public:
  AliHLTTRDOfflineClusterizerComponent();
  virtual ~AliHLTTRDOfflineClusterizerComponent();

  const char* GetComponentID();
  AliHLTComponent* Spawn();

  int DoInit( int argc, const char** argv );
  int DoDeinit();
  int DoEvent( const AliHLTComponent_EventData& evtData, const AliHLTComponent_BlockData* blocks, 
	       AliHLTComponent_TriggerData& trigData, AliHLTUInt8_t* outputPtr, 
	       AliHLTUInt32_t& size, vector<AliHLTComponent_BlockData>& outputBlocks );

protected:
  void SetOfflineParams();

  ClassDef(AliHLTTRDOfflineClusterizerComponent, 1)

};
#endif

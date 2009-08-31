//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTTRDOFFLINETRACKERV1COMPONENT_H
#define ALIHLTTRDOFFLINETRACKERV1COMPONENT_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTTRDOfflineTrackerV1Component
    @author 
    @date   2009-08-31
    @brief  
*/

#include "AliHLTTRDTrackerV1Component.h"

class AliHLTTRDOfflineTrackerV1Component : public AliHLTTRDTrackerV1Component
{
public:
  AliHLTTRDOfflineTrackerV1Component();
  virtual ~AliHLTTRDOfflineTrackerV1Component();

  const char* GetComponentID();
  AliHLTComponent* Spawn();

protected:
  int DoInit( int argc, const char** argv );

  void SetOfflineParams();

  ClassDef(AliHLTTRDOfflineTrackerV1Component, 1)

};
#endif

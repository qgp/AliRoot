#ifndef ALIMUONCHAMBERTRIGGER_H
#define ALIMUONCHAMBERTRIGGER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004
//
/// \ingroup sim
/// \class AliMUONChamberTrigger
/// \brief Muon trigger chamber class

#include "AliMUONChamber.h"

class AliMUONClusterFinder;
class AliMUONResponseTrigger;
class AliMUONResponseTriggerV1;
class AliMUONGeometryTransformer;
class AliMUONHit;

class AliMUONChamberTrigger : public AliMUONChamber 
{
  public:
    AliMUONChamberTrigger();
    AliMUONChamberTrigger(Int_t id, const AliMUONGeometryTransformer* kGeometry);
    virtual ~AliMUONChamberTrigger();
    
  protected:   
    /// Not implemented
    AliMUONChamberTrigger(const AliMUONChamberTrigger& right);
    /// Not implemented
    AliMUONChamberTrigger&  operator = (const AliMUONChamberTrigger& right);

    const AliMUONGeometryTransformer* fkGeomTransformer;///< geometry transformations

  ClassDef(AliMUONChamberTrigger,2) // Muon trigger chamber class
};
#endif













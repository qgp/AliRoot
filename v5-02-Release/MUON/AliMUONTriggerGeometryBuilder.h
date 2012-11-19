/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// Revision of includes 07/05/2004
//
/// \ingroup sim
/// \class AliMUONTriggerGeometryBuilder
/// \brief MUON Trigger stations geometry construction class
///
/// \author Philippe Crochet (LPCCFd)

#ifndef ALI_MUON_TRIGGER_GEOMETRY_BUILDER_H
#define ALI_MUON_TRIGGER_GEOMETRY_BUILDER_H

#include "AliMUONVGeometryBuilder.h"

class AliMUON;

class AliMUONTriggerGeometryBuilder : public AliMUONVGeometryBuilder
{
  public:
    AliMUONTriggerGeometryBuilder(AliMUON* muon);
    AliMUONTriggerGeometryBuilder();
    virtual ~AliMUONTriggerGeometryBuilder();
  
    // methods
    virtual void CreateGeometry();
    virtual void SetVolumes();
    virtual void SetTransformations();
    virtual void SetSensitiveVolumes();
    
    /// Do not apply global transformation (geometry is defined in the new ALICE reference frame)
    virtual bool ApplyGlobalTransformation() { return false; }
    
  protected:  
    
  private:
    /// Not implemented
    AliMUONTriggerGeometryBuilder(const AliMUONTriggerGeometryBuilder& rhs);
    /// Not implemented
    AliMUONTriggerGeometryBuilder& operator = (const AliMUONTriggerGeometryBuilder& rhs);

    AliMUON*  fMUON; ///< the MUON detector class 
        
  ClassDef(AliMUONTriggerGeometryBuilder,1) // MUON Trigger stations geometry construction class
};

#endif //ALI_MUON_TRIGGER_GEOMETRY_BUILDER_H

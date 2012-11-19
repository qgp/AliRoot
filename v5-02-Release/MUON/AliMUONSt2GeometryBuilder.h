/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// Revision of includes 07/05/2004
//
/// \ingroup sim
/// \class AliMUONSt2GeometryBuilder
/// \brief MUON Station2 coarse geometry construction class
///
/// Extracted from AliMUONv1
/// by Ivana Hrivnacova, IPN Orsay

#ifndef ALI_MUON_ST2_GEOMETRY_BUILDER_H
#define ALI_MUON_ST2_GEOMETRY_BUILDER_H

#include "AliMUONVGeometryBuilder.h"

class AliMUON;

class AliMUONSt2GeometryBuilder : public AliMUONVGeometryBuilder
{
  public:
    AliMUONSt2GeometryBuilder(AliMUON* muon);
    AliMUONSt2GeometryBuilder();
    virtual ~AliMUONSt2GeometryBuilder();
  
    // methods
    virtual void CreateGeometry();
    virtual void SetVolumes();
    virtual void SetTransformations();
    virtual void SetSensitiveVolumes();
    
  private:
    /// Not implemented
    AliMUONSt2GeometryBuilder(const AliMUONSt2GeometryBuilder& rhs);
    /// Not implemented
    AliMUONSt2GeometryBuilder& operator = (const AliMUONSt2GeometryBuilder& rhs);

    AliMUON*  fMUON; ///< the MUON detector class 
        
  ClassDef(AliMUONSt2GeometryBuilder,1) // MUON Station2 coarse geometry construction class
};

#endif //ALI_MUON_ST2_GEOMETRY_BUILDER_H

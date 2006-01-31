/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// Revision of includes 07/05/2004

/// \ingroup geometry
/// \class AliMUONVGeometryBuilder
/// \brief Abstract base class for geometry construction per module(s)
///
/// Author: Ivana Hrivnacova, IPN Orsay

#ifndef ALI_MUON_V_GEOMETRY_BUILDER_H
#define ALI_MUON_V_GEOMETRY_BUILDER_H

#include <fstream>

#include <TObject.h>
#include <TObjArray.h>
#include <TGeoMatrix.h>

class TGeoTranslation;
class TGeoRotation;
class TGeoCombiTrans;

class AliMUONGeometryModule;
class AliMUONGeometryEnvelopeStore;
class AliMUONGeometryStore;
class AliMUONGeometrySVMap;

class AliMUONVGeometryBuilder : public TObject
{
  public:
    AliMUONVGeometryBuilder(Int_t geometryModuleId1,
                            Int_t geometryModuleId2 = -1,
                            Int_t geometryModuleId3 = -1,
                            Int_t geometryModuleId4 = -1,
                            Int_t geometryModuleId5 = -1,
                            Int_t geometryModuleId6 = -1);
    AliMUONVGeometryBuilder();
    virtual ~AliMUONVGeometryBuilder();
  
    // methods
    void  SetReferenceFrame(const TGeoCombiTrans& referenceFrame);
    void  RebuildSVMaps() const;
    void  FillTransformations() const;

    virtual void CreateMaterials() {}  // make = 0; ?
                  // Function to be overriden in a concrete chamber/station
		  // geometry builder class.
		  // Only materials that are not defined in the common
		  // functions should be defined here.

    virtual void CreateGeometry() = 0;
                  // Function to be overriden in a concrete chamber/station
		  // geometry builder class.
		  // The geometry built there should not be placed
		  // in ALIC; but all volumes going to ALIC
		  // have to be added as envelopes to the chamber
		  // geometries
		  // (They will be then placed automatically 
		  // usind the provided transformation.

    virtual void SetTransformations() = 0;
                  // Function to be overriden in a concrete chamber/station
		  // geometry class.
		  // The transformation of each chamber(s) wrt ALICE
		  // should be defined and set to its geometry class. 

    virtual void SetSensitiveVolumes() = 0;
                  // Function to be overriden in a concrete chamber/station
		  // geometry class.
		  // The sensitive volumes Ids for each chamber
		  // should be defined and set to its geometry class. 

    virtual bool ApplyGlobalTransformation() { return true; }
                  // Function to be overriden (and return false) 
		  // in the concrete geometry builder classes 
		  // which are already defined in the new ALICE
		  // coordinate frame

    // access to module geometries
    Int_t  NofGeometries() const;
    AliMUONGeometryModule* Geometry(Int_t i) const;
                  // In difference from protected GetGeometry()
		  // this function access geometry via index and not
		  // via moduleId

  protected:
    AliMUONVGeometryBuilder(const AliMUONVGeometryBuilder& rhs);

    // operators  
    AliMUONVGeometryBuilder& operator = (const AliMUONVGeometryBuilder& rhs);

    // methods
    AliMUONGeometryModule*         GetGeometry(Int_t moduleId) const;
    AliMUONGeometryEnvelopeStore*  GetEnvelopes(Int_t moduleId) const;
    AliMUONGeometrySVMap*          GetSVMap(Int_t moduleId) const;
    
    // set module transformation
    void SetTranslation(Int_t moduleId, 
                        const TGeoTranslation& translation);
    void SetTransformation(Int_t moduleId, 
                        const TGeoTranslation& translation,
			const TGeoRotation& rotation);
    
  private:
    //methods
    TGeoHMatrix ConvertTransform(const TGeoHMatrix& transform) const;
    TString     ComposePath(const TString& volName, Int_t copyNo) const; 
    void        MapSV(const TString& path0, 
                      const TString& volName, Int_t detElemId) const;

    // data members
    TObjArray*  fGeometryModules;   // the modules geometries that will be built
                                    // by this builder				    
    TGeoCombiTrans fReferenceFrame; // the transformation from the builder 
                                    // reference frame to that of the transform 
				    // data files
				        
  ClassDef(AliMUONVGeometryBuilder,4) // MUON chamber geometry base class
};

// inline functions

inline Int_t  AliMUONVGeometryBuilder::NofGeometries() const
{ return fGeometryModules->GetEntriesFast(); }

inline AliMUONGeometryModule* AliMUONVGeometryBuilder::Geometry(Int_t i) const
{ return (AliMUONGeometryModule*)fGeometryModules->At(i); }

#endif //ALI_MUON_V_GEOMETRY_BUILDER_H

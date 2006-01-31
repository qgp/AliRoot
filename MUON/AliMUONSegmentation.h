/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup geometry
/// \class AliMUONSegmentation
/// \brief Container class for modules segmentations
///
/// Author: Ivana Hrivnacova, IPN Orsay

#ifndef ALI_MUON_SEGMENTATION_H
#define ALI_MUON_SEGMENTATION_H

#include <TObject.h>
#include <TGeoMatrix.h>

class TObjArray;

class AliMpVSegmentation;

class AliMUONGeometrySegmentation;
class AliMUONVGeometryDESegmentation;

class AliMUONSegmentation : public TObject
{
  public:
    AliMUONSegmentation(Int_t nofModules);
    AliMUONSegmentation();
    virtual  ~AliMUONSegmentation();
    
    // methods
    void  AddDESegmentation(AliMUONVGeometryDESegmentation* segmentation);

    void  AddModuleSegmentation(Int_t moduleId, Int_t cathod,
                            AliMUONGeometrySegmentation* segmentation);
    void  Init();			    

    //
    // get methods
    //
    
    // Geometry segmentations
    //
    AliMUONGeometrySegmentation* GetModuleSegmentation(
                     Int_t moduleId, Int_t cathod, Bool_t warn = true) const;

    AliMUONGeometrySegmentation* GetModuleSegmentationByDEId(
                     Int_t detElemId, Int_t cathod, Bool_t warn = true) const;

    // DE segmentations
    //
    const AliMUONVGeometryDESegmentation* GetDESegmentation(
                     Int_t detElemId, Int_t cathod, Bool_t warn = true) const;

    // Mapping segmentations
    //
    const AliMpVSegmentation* GetMpSegmentation(
                     Int_t detElemId, Int_t cathod, Bool_t warn = true) const;
		     
    // DE properties
    //
    Bool_t   HasDE(Int_t detElemId, Int_t cathod = 0) const;
    TString  GetDEName(Int_t detElemId, Int_t cathod = 0) const;

  protected:
    AliMUONSegmentation(const AliMUONSegmentation& right);
    AliMUONSegmentation&  operator = (const AliMUONSegmentation& right);
 
  private:
    // data members
    TObjArray*  fDESegmentations;        // array of DE segmentations
    TObjArray*  fModuleSegmentations[2]; // array of module segmentations
                                         // for two cathods         

  ClassDef(AliMUONSegmentation,1)  // Container class for module segmentations
};

#endif //ALI_MUON_SEGMENTATION_H








/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$
//
// Class AliMUONVGeometryBuilder
// -----------------------------
// Abstract base class for geometry construction per geometry module(s).
// Author: Ivana Hrivnacova, IPN Orsay
// 23/01/2004

#include <Riostream.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TGeoMatrix.h>
#include <TVirtualMC.h>

#include "AliMUONVGeometryBuilder.h"
#include "AliMUONGeometryModule.h"
#include "AliMUONGeometryDetElement.h"
#include "AliMUONGeometryStore.h"
#include "AliMUONGeometrySVMap.h"
#include "AliMUONGeometryEnvelopeStore.h"
#include "AliMUONGeometryEnvelope.h"
#include "AliMUONGeometryConstituent.h"
#include "AliMUONGeometryDEIndexing.h"
#include "AliMUONGeometryBuilder.h"
#include "AliLog.h"

ClassImp(AliMUONVGeometryBuilder)

//______________________________________________________________________________
AliMUONVGeometryBuilder::AliMUONVGeometryBuilder(
                            Int_t moduleId1, Int_t moduleId2,
                            Int_t moduleId3, Int_t moduleId4,
                            Int_t moduleId5, Int_t moduleId6)
 : TObject(),
   fGeometryModules(0),
   fReferenceFrame()
 {
// Standard constructor

  // Create the module geometries array
  fGeometryModules = new TObjArray();
  
  if ( moduleId1 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId1));
 
  if ( moduleId2 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId2));

  if ( moduleId3 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId3));

  if ( moduleId4 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId4));

  if ( moduleId5 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId5));

  if ( moduleId6 >= 0 ) 
    fGeometryModules->Add(new AliMUONGeometryModule(moduleId6));
}

//______________________________________________________________________________
AliMUONVGeometryBuilder::AliMUONVGeometryBuilder()
 : TObject(),
   fGeometryModules(0),
   fReferenceFrame()
{
// Default constructor
}


//______________________________________________________________________________
AliMUONVGeometryBuilder::AliMUONVGeometryBuilder(const AliMUONVGeometryBuilder& rhs)
  : TObject(rhs)
{
// Protected copy constructor

  AliFatal("Copy constructor is not implemented.");
}

//______________________________________________________________________________
AliMUONVGeometryBuilder::~AliMUONVGeometryBuilder() {
//
  if (fGeometryModules) {
    fGeometryModules->Clear(); // Sets pointers to 0 since it is not the owner
    delete fGeometryModules;
  }
}

//______________________________________________________________________________
AliMUONVGeometryBuilder& 
AliMUONVGeometryBuilder::operator = (const AliMUONVGeometryBuilder& rhs) 
{
// Protected assignement operator

  // check assignement to self
  if (this == &rhs) return *this;

  AliFatal("Assignment operator is not implemented.");
    
  return *this;  
}

//
// private methods
//

//______________________________________________________________________________
TGeoHMatrix 
AliMUONVGeometryBuilder::ConvertTransform(const TGeoHMatrix& transform) const
{
// Convert transformation into the reference frame

  if ( fReferenceFrame.IsIdentity() )
    return transform;
  else  {
    return AliMUONGeometryBuilder::Multiply( fReferenceFrame.Inverse(),
  				  	     transform,
    					     fReferenceFrame );  
  }			    
}

//______________________________________________________________________________
TString  AliMUONVGeometryBuilder::ComposePath(const TString& volName, 
                                               Int_t copyNo) const
{
// Compose path from given volName and copyNo
// ---

  TString path(volName);
  path += ".";
  path += copyNo;
  
  return path;
}  

//______________________________________________________________________________
void AliMUONVGeometryBuilder::MapSV(const TString& path0, 
                                    const TString& volName, Int_t detElemId) const
{
// Update the path with all daughters volumes recursively
// and map it to the detection element Id if it is a sensitive volume
// ---

  // Get module sensitive volumes map
  Int_t moduleId = AliMUONGeometryDEIndexing::GetModuleId(detElemId);
  AliMUONGeometrySVMap* svMap = GetSVMap(moduleId);     

  Int_t nofDaughters = gMC->NofVolDaughters(volName);
  if (nofDaughters == 0) {

    // Get the name of the last volume in the path
    Ssiz_t npos1 = path0.Last('/')+1; 
    Ssiz_t npos2 = path0.Last('.');
    TString volName(path0(npos1, npos2-npos1));  
    
    // Check if it is sensitive volume
    Int_t moduleId = AliMUONGeometryDEIndexing::GetModuleId(detElemId);
    AliMUONGeometryModule* geometry = GetGeometry(moduleId);
    if (geometry->IsSensitiveVolume(volName)) {
      //cout << ".. adding to the map  " 
      //     <<  path0 << "  "  << detElemId << endl;
    
      // Map the sensitive volume to detection element
      svMap->Add(path0, detElemId); 
    }  
    return; 
  }  

  for (Int_t i=0; i<nofDaughters; i++) {
    Int_t copyNo = gMC->VolDaughterCopyNo(volName, i);
    TString newName =  gMC->VolDaughterName(volName, i);
            
    TString path = path0;
    path += "/";
    path += ComposePath(newName, copyNo);

    MapSV(path, newName, detElemId);
  }
}     

//
// protected methods
//

//______________________________________________________________________________
AliMUONGeometryModule*  
AliMUONVGeometryBuilder::GetGeometry(Int_t moduleId) const
{
// Returns the module geometry specified by moduleId
// ---

  for (Int_t i=0; i<fGeometryModules->GetEntriesFast(); i++) {

    AliMUONGeometryModule* geometry 
      = (AliMUONGeometryModule*)fGeometryModules->At(i);

    if ( geometry->GetModuleId() == moduleId) return geometry;
  }   
  
  return 0;
}  

//______________________________________________________________________________
AliMUONGeometryEnvelopeStore*  
AliMUONVGeometryBuilder::GetEnvelopes(Int_t moduleId) const
{
// Returns the envelope store of the module geometry specified by moduleId
// ---

  AliMUONGeometryModule* geometry = GetGeometry(moduleId);
  
  if (!geometry) {
    AliFatal(Form("Module geometry %d is not defined", moduleId)); 
    return 0;
  }
  
  return geometry->GetEnvelopeStore();
}  

//______________________________________________________________________________
AliMUONGeometrySVMap*  
AliMUONVGeometryBuilder::GetSVMap(Int_t moduleId) const
{
// Returns the transformation store of the module geometry specified by moduleId
// ---

  AliMUONGeometryModule* geometry = GetGeometry(moduleId);
  
  if (!geometry) {
    AliFatal(Form("Geometry %d is not defined", moduleId)); 
    return 0;
  }
  
  return geometry->GetSVMap();
}  

//______________________________________________________________________________
void AliMUONVGeometryBuilder::SetTranslation(Int_t moduleId, 
                                  const TGeoTranslation& translation)
{
// Sets the translation to the geometry module given by moduleId,
// applies reference frame transformation 
// ---

  AliMUONGeometryModule* geometry = GetGeometry(moduleId);
  
  if (!geometry) {
    AliFatal(Form("Geometry %d is not defined", moduleId)); 
    return;
  }
  
  // Apply frame transform
  TGeoHMatrix newTransform = ConvertTransform(translation);

  // Set new transformation
  geometry->SetTransformation(newTransform);
}  


//______________________________________________________________________________
void AliMUONVGeometryBuilder::SetTransformation(Int_t moduleId, 
                                  const TGeoTranslation& translation,
				  const TGeoRotation& rotation)
{
// Sets the translation to the geometry module given by moduleId,
// applies reference frame transformation 
// ---

  AliMUONGeometryModule* geometry = GetGeometry(moduleId);
  
  if (!geometry) {
    AliFatal(Form("Geometry %d is not defined", moduleId)); 
    return;
  }
  
  TGeoCombiTrans transformation 
    = TGeoCombiTrans(translation, rotation);

  // Apply frame transform
  TGeoHMatrix newTransform = ConvertTransform(translation);

  // Set new transformation
  geometry->SetTransformation(newTransform);
}  

//
// public functions
//

//______________________________________________________________________________
void  AliMUONVGeometryBuilder::SetReferenceFrame(
                                  const TGeoCombiTrans& referenceFrame)
{ 
  fReferenceFrame = referenceFrame; 

  for (Int_t i=0; i<fGeometryModules->GetEntriesFast(); i++) {
    AliMUONGeometryModule* geometry 
      = (AliMUONGeometryModule*)fGeometryModules->At(i);
    AliMUONGeometryEnvelopeStore* envelopeStore 
      = geometry->GetEnvelopeStore();
      
    envelopeStore->SetReferenceFrame(referenceFrame);
  }          
}


//______________________________________________________________________________
void  AliMUONVGeometryBuilder::FillTransformations() const
{
// Fills transformations store from defined geometry.
// ---

  for (Int_t i=0; i<fGeometryModules->GetEntriesFast(); i++) {
    AliMUONGeometryModule* geometry 
      = (AliMUONGeometryModule*)fGeometryModules->At(i);
    const TObjArray* envelopes 
      = geometry->GetEnvelopeStore()->GetEnvelopes();    
    
    AliMUONGeometryStore* detElements 
      = geometry->GetTransformer()->GetDetElementStore(); 
      
    for (Int_t j=0; j<envelopes->GetEntriesFast(); j++) {
      AliMUONGeometryEnvelope* envelope
        = (AliMUONGeometryEnvelope*)envelopes->At(j);

      // skip envelope not corresponding to detection element
      if(envelope->GetUniqueID() == 0) continue;
       
      // Get envelope data 
      Int_t detElemId = envelope->GetUniqueID();	
      TString path = ComposePath(envelope->GetName(), 
                                 envelope->GetCopyNo());
      const TGeoCombiTrans* transform = envelope->GetTransformation(); 
      
      // Apply frame transform
      TGeoHMatrix newTransform = ConvertTransform(*transform);

      // Add detection element transformation 
      detElements->Add(detElemId,
        new AliMUONGeometryDetElement(detElemId, path, newTransform)); 
    }  
  }
}

//_____ _________________________________________________________________________
void  AliMUONVGeometryBuilder::RebuildSVMaps() const
{
// Clear the SV maps in memory and fill them from defined geometry.
// ---

  for (Int_t i=0; i<fGeometryModules->GetEntriesFast(); i++) {
    AliMUONGeometryModule* geometry 
      = (AliMUONGeometryModule*)fGeometryModules->At(i);
    
    // Clear the map   
    geometry->GetSVMap()->Clear();
     
    // Fill the map from geometry
    const TObjArray* envelopes 
      = geometry->GetEnvelopeStore()->GetEnvelopes();    

    for (Int_t j=0; j<envelopes->GetEntriesFast(); j++) {
      AliMUONGeometryEnvelope* envelope
        = (AliMUONGeometryEnvelope*)envelopes->At(j);

      // skip envelope not corresponding to detection element
      if(envelope->GetUniqueID() == 0) continue;
       
      TString path0("/ALIC.1");
      if (geometry->GetMotherVolume() != "ALIC") {
        path0 += "/";
	path0 += ComposePath(geometry->GetMotherVolume(), 1);
      }  
      if (! geometry->IsVirtual() ) {
        path0 += "/";
	path0 += ComposePath(geometry->GetVolume(), 1);
      }  
       
      if (!envelope->IsVirtual()) {
         TString path = path0;
         path += "/";
	 path += ComposePath(envelope->GetName(), envelope->GetCopyNo());
	 MapSV(path, envelope->GetName(), envelope->GetUniqueID());
      }
      else {	 
        for  (Int_t k=0; k<envelope->GetConstituents()->GetEntriesFast(); k++) {
          AliMUONGeometryConstituent* constituent
            = (AliMUONGeometryConstituent*)envelope->GetConstituents()->At(k);
         TString path = path0;
         path += "/";
	 path += ComposePath(constituent->GetName(), constituent->GetCopyNo());
	 MapSV(path, constituent->GetName(), envelope->GetUniqueID());
        }
      }
    }  
  } 	             
}


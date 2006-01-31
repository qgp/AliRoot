/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup geometry
/// \class AliMUONGeometryEnvelopeStore
/// \brief Store for temporary volumes envelopes
///
/// Class for definititon of the temporary volume envelopes
/// used in geometry construction
///
/// Author: Ivana Hrivnacova, IPN Orsay

#ifndef ALI_MUON_GEOMETRY_ENVELOPE_STORE_H
#define ALI_MUON_GEOMETRY_ENVELOPE_STORE_H

#include <TObject.h>
#include <TString.h>
#include <TGeoMatrix.h>

class TGeoTranslation;
class TGeoRotation;
class TGeoCombiTrans;
class TObjArray;
class TArrayI;

class AliMUONChamber;
class AliMUONGeometryEnvelope;
class AliMUONGeometryStore;

class AliMUONGeometryEnvelopeStore : public TObject
{
  public:
    AliMUONGeometryEnvelopeStore(AliMUONGeometryStore* detElements);
    AliMUONGeometryEnvelopeStore();
    AliMUONGeometryEnvelopeStore(const AliMUONGeometryEnvelopeStore& rhs);
    virtual ~AliMUONGeometryEnvelopeStore();

    // operators  
    AliMUONGeometryEnvelopeStore& 
    operator = (const AliMUONGeometryEnvelopeStore& rhs);
  
    // methods
        
          // adding virtual envelopes 	
          // (not placed in MC geometry, only logical assembly of volumes,
	  //  cannot have more copies)	
    void  AddEnvelope(const TString& name, Int_t id, 
                      Bool_t isVirtual, const char* only="ONLY"); 
    void  AddEnvelope(const TString& name, Int_t id,  
                      Bool_t isVirtual,
                      const TGeoTranslation& translation, 
		      const char* only="ONLY"); 
    void  AddEnvelope(const TString& name, Int_t id, 
                      Bool_t isVirtual, 
                      const TGeoTranslation& translation, 
		      const TGeoRotation& rotation,
		      const char* only="ONLY");
    void  AddEnvelope(const TString& name, Int_t id,  
                      Bool_t isVirtual,
                      const TGeoCombiTrans& transform,
		      const char* only="ONLY");
		      
          // adding non-virtual envelopes 	
          // (placed in MC geometry with transformation composed
	  //  of transformation of chamber and their transformation, 
	  //  can have more copies )	
    void  AddEnvelope(const TString& name, Int_t id, 
                      Int_t copyNo, const char* only="ONLY"); 
    void  AddEnvelope(const TString& name, Int_t id, 
                      Int_t copyNo, 
                      const TGeoTranslation& translation,
		      const char* only="ONLY"); 
    void  AddEnvelope(const TString& name,  Int_t id, 
                      Int_t copyNo,
                      const TGeoTranslation& translation, 
		      const TGeoRotation& rotation,
		      const char* only="ONLY");
    void  AddEnvelope(const TString& name,  Int_t id, 
                      Int_t copyNo, 
                      const TGeoCombiTrans& transform,
		      const char* only="ONLY");

          // adding constituents to virtual envelopes 	
          // (placed in MC geometry with transformation composed
	  //  of transformation of chamber, envelope and their own
	  //  transformation )	
    void  AddEnvelopeConstituent(const TString& name, const TString& envName, 
                      Int_t copyNo); 
    void  AddEnvelopeConstituent(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoTranslation& translation); 
    void  AddEnvelopeConstituent(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoTranslation& translation, 
		      const TGeoRotation& rotation);
    void  AddEnvelopeConstituent(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoCombiTrans& transform);
		      		      
          // adding constituents to virtual envelopes with specified shape
	  // parameters
          // (placed in MC geometry with transformation composed
	  //  of transformation of chamber, envelope and their own
	  //  transformation )	
    void  AddEnvelopeConstituentParam(const TString& name, const TString& envName, 
                      Int_t copyNo, Int_t npar, Double_t* param); 
    void  AddEnvelopeConstituentParam(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoTranslation& translation,
		      Int_t npar, Double_t* param); 
    void  AddEnvelopeConstituentParam(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoTranslation& translation, 
		      const TGeoRotation& rotation, Int_t npar, Double_t* param);
    void  AddEnvelopeConstituentParam(const TString& name, const TString& envName, 
                      Int_t copyNo, const TGeoCombiTrans& transform,
		      Int_t npar, Double_t* param);
		      		      
    void  SetDebug(Bool_t debug);
    void  SetReferenceFrame(const TGeoCombiTrans& referenceFrame);

    // Alignement
    virtual Bool_t  GetAlign() const;
    virtual void    SetAlign(Bool_t align);
 
    // get methods
    const TObjArray*  GetEnvelopes() const;
    Int_t             GetNofDetElements() const;

  private:
    // methods
    TGeoHMatrix ConvertTransform(const TGeoHMatrix& transform) const;
    AliMUONGeometryEnvelope* FindEnvelope(const TString& name) const;
    Bool_t AlignEnvelope(AliMUONGeometryEnvelope* envelope) const;
 
    // data members
    TObjArray*  fEnvelopes; // the envelopes names and transformations
		            // wrt to the chamber position in mother volume                                 
    AliMUONGeometryStore*  fDetElements; // detection elements
                            // used for alignement of enevelopes
    TGeoCombiTrans fReferenceFrame; // the transformation from the builder 
                                    // reference frame to that of the transform 
				    // data files
    Bool_t      fDebug;     // Switch for debugging  
    Bool_t      fAlign;     // option to read transformations from a file
 
  ClassDef(AliMUONGeometryEnvelopeStore,2) // Geometry envelope store
};

// inline functions

inline void  AliMUONGeometryEnvelopeStore::SetDebug(Bool_t debug)
{ fDebug = debug; }

inline Bool_t  AliMUONGeometryEnvelopeStore::GetAlign() const
{ return fAlign; }

inline void AliMUONGeometryEnvelopeStore::SetAlign(Bool_t align)
{ fAlign = align; }

inline const TObjArray* AliMUONGeometryEnvelopeStore::GetEnvelopes() const
{ return fEnvelopes; }

inline void 
AliMUONGeometryEnvelopeStore::SetReferenceFrame(const TGeoCombiTrans& referenceFrame)
{ fReferenceFrame = referenceFrame; }

#endif //ALI_MUON_CHAMBER_ENVELOPE_STORE_H

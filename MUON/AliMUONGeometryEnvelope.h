// $Id$
//
// Class AliMUONGeometryEnvelope
// -----------------------------
// Helper class for definititon an assembly of volumes.
//
// Author: Ivana Hrivnacova, IPN Orsay

#ifndef ALI_MUON_GEOMETRY_ENVELOPE_H
#define ALI_MUON_GEOMETRY_ENVELOPE_H

#include <TNamed.h>

class TGeoTranslation;
class TGeoRotation;
class TGeoCombiTrans;
class TObjArray;

class AliMUONGeometryEnvelope : public TNamed
{
  public:
    AliMUONGeometryEnvelope(const TString& name, Bool_t isVirtual);
    AliMUONGeometryEnvelope(const TString& name, Int_t copyNo);
    AliMUONGeometryEnvelope();
    AliMUONGeometryEnvelope(const AliMUONGeometryEnvelope& rhs);
    virtual ~AliMUONGeometryEnvelope();

    // operators  
    AliMUONGeometryEnvelope& operator = (const AliMUONGeometryEnvelope& rhs);
  
    // methods
    void  AddConstituent(const TString& name, Int_t copyNo); 
    void  AddConstituent(const TString& name, Int_t copyNo, 
                         const TGeoTranslation& translation); 
    void  AddConstituent(const TString& name, Int_t copyNo,
                         const TGeoTranslation& translation, 
	  	         const TGeoRotation& rotation);

    void  AddConstituentParam(const TString& name, Int_t copyNo, 
                         Int_t npar, Double_t* param);
    void  AddConstituentParam(const TString& name, Int_t copyNo, 
                         const TGeoTranslation& translation,
			 Int_t npar, Double_t* param);
    void  AddConstituentParam(const TString& name, Int_t copyNo, 
                         const TGeoTranslation& translation, 
	  	         const TGeoRotation& rotation,
			 Int_t npar, Double_t* param);

    void  SetTranslation(const TGeoTranslation& translation);
    void  SetRotation(const TGeoRotation& rotation);

    // get methods
    Bool_t                 IsVirtual() const;  
    Int_t                  GetCopyNo() const;  
    const TGeoCombiTrans*  GetTransformation() const;
    const TObjArray*       GetConstituents() const;

  private:
    Bool_t           fIsVirtual;     // true if envelope is not represented
                                     // by a real volume
    Int_t            fCopyNo;        // copy number 
                                     // (only non virtual envelope can have 
				     //  more than one copy)
    TGeoCombiTrans*  fTransformation;// the envelope transformation wrt to the
                                     // mother frame (the chamber position)
    TObjArray*       fConstituents;  // the constituents names and transformations
		                     // wrt to the envelope position                                 
 
  ClassDef(AliMUONGeometryEnvelope,1) // MUON chamber geometry base class
};

// inline functions

inline Bool_t AliMUONGeometryEnvelope::IsVirtual() const
{ return fIsVirtual; }  

inline Int_t AliMUONGeometryEnvelope::GetCopyNo() const
{ return fCopyNo; }  

inline const TGeoCombiTrans* AliMUONGeometryEnvelope::GetTransformation() const 
{ return fTransformation; }

inline const TObjArray* AliMUONGeometryEnvelope::GetConstituents() const
{ return fConstituents; }

#endif //ALI_MUON_GEOMETRY_ENVELOPE_H

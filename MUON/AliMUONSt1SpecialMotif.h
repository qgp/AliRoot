#ifndef ALI_MUON_ST1_SPECIAL_MOTIF_H
#define ALI_MUON_ST1_SPECIAL_MOTIF_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Authors: David Guez, Ivana Hrivnacova, Marion MacCormick; IPN Orsay
//
// Class AliMUONSt1SpecialMotif
// ----------------------------
// Encapsulate the distance between the center of a given daughter card
// and the pad/kapton connector.

#include <TVector2.h>

class AliMUONSt1SpecialMotif  
{
  public:
    AliMUONSt1SpecialMotif();
    AliMUONSt1SpecialMotif(const TVector2& delta,Double_t rotAngle=0.);
    AliMUONSt1SpecialMotif(const AliMUONSt1SpecialMotif& src);
    virtual ~AliMUONSt1SpecialMotif();
    
    TVector2 GetDelta()    const {return fDelta;}
    Double_t GetRotAngle() const {return fRotAngle;}

  private:
    TVector2  fDelta;   // offset of this motif
    Double_t  fRotAngle;// rotation angle in degrees (0� = vertical) 
};

#endif //ALI_MUON_ST1_SPECIAL_MOTIF_H

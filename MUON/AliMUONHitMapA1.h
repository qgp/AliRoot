#ifndef ALIMUONHITMAPA1_H
#define ALIMUONHITMAPA1_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004

/// \ingroup sim
/// \class AliMUONHitMapA1
/// \brief Implements AliHitMap as a 1-dim array

#include "AliHitMap.h"

class TObjArray;

class AliMUONHitMapA1 : public AliHitMap 
{
 public:
    AliMUONHitMapA1();
    //AliMUONHitMapA1(Int_t idDE, AliMUONGeometrySegmentation* seg, TObjArray* dig);
    AliMUONHitMapA1(Int_t npx, Int_t npy, TObjArray* dig);
    virtual ~AliMUONHitMapA1();

    // Fill hits from list of digits into hit map
    virtual  void  FillHits();
    // Clear the hit map
    virtual  void  Clear(const char *opt = "");
    // Set a single hit
    virtual  void  SetHit(Int_t ix, Int_t iy, Int_t idigit);
    // Delete a single hit
    virtual  void  DeleteHit(Int_t ix, Int_t iy);
    // Get index of hit in the list of digits
    virtual Int_t  GetHitIndex(Int_t ix, Int_t iy) const;
    // Get pointer to digit
    virtual TObject*  GetHit(Int_t ix, Int_t iy) const;
    // Flag a hit as used
    virtual  void  FlagHit(Int_t ix, Int_t iy);
    // Validate
    virtual  Bool_t ValidateHit(Int_t ix, Int_t iy);
    // Test hit status
    virtual FlagType TestHit(Int_t ix, Int_t iy);

 private:
    /// Not implemented
    AliMUONHitMapA1(const AliMUONHitMapA1 & hitMap);
    /// Not implemented
    AliMUONHitMapA1& operator = (const AliMUONHitMapA1& rhs);

    // Check index
    Int_t CheckedIndex(Int_t ix, Int_t iy) const;
 private:
    Int_t fNpx;                           ///< Maximum number of pads in x
    Int_t fNpy;                           ///< Maximum number of pads in y
    TObjArray *fDigits;                   ///< Pointer to digits
    Int_t fMaxIndex;                      ///< maximum index in hit map

    /// The array of digits
    Int_t *fHitMap;                       // ! [fMaxIndex]         

    ClassDef(AliMUONHitMapA1,0) // Implements HitMap as a 1-dim array
};
#endif	

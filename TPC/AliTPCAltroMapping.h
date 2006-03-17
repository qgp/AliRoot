#ifndef ALITPCALTROMAPPING_H
#define ALITPCALTROMAPPING_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//////////////////////////////////////////////////////////
// Class used to setup the mapping of hardware adresses //
// in ALTRO to pad-rows and pad indeces.                //
// The mapping is defined in an external mapping files  //
// separately. The class derives from the base altro    //
// mapping class defined in the RAW package.            //
//////////////////////////////////////////////////////////

#include "AliAltroMapping.h"

class AliTPCAltroMapping: public AliAltroMapping {
 public:
  AliTPCAltroMapping(const char *mappingFile);
  virtual ~AliTPCAltroMapping();

  AliTPCAltroMapping(const AliTPCAltroMapping& mapping);
  AliTPCAltroMapping& operator = (const AliTPCAltroMapping& mapping);

  virtual Int_t GetHWAdress(Int_t padrow, Int_t pad, Int_t sector) const;
  virtual Int_t GetPadRow(Int_t hwAdress) const;
  virtual Int_t GetPad(Int_t hwAdress) const;
  virtual Int_t GetSector(Int_t hwAdress) const;

 protected:
  virtual Bool_t ReadMapping();
  virtual void   DeleteMappingArrays();

  Int_t     fMinPadRow;        // Minimum Index of pad-row
  Int_t     fMaxPadRow;        // Maximum Index of pad-row
  Int_t     fMaxPad;           // Maximum Index of pad inside row
  Short_t **fMapping;          // Array which connects hardware adresses to pad and pad-row indeces
  Short_t **fInvMapping;       // Inverse of fMapping

  ClassDef(AliTPCAltroMapping,0)  // Altro mapping handler class
};

#endif

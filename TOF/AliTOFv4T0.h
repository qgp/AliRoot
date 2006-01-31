//_________________________________________________________________________
// Implementation version v4 of TOF Manager class
// FULL COVERAGE VERSION i.e. NO HOLES FOR PHOS AND HMPID (RICH) ARE DEFINED
//   
//*-- Authors: Pierella, Seganti, Vicinanza (Bologna and Salerno University)

#ifndef ALITOFv4T0_H
#define ALITOFv4T0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "AliTOF.h"
 
 
class AliTOFv4T0 : public AliTOF {

public:
  AliTOFv4T0();
  AliTOFv4T0(const char *name, const char *title);
  virtual ~AliTOFv4T0() {   if (fTOFGeometry) {delete fTOFGeometry; 
    fTOFGeometry  = 0;  }
}
  virtual void   BuildGeometry();
  virtual void   CreateGeometry();
  virtual void   CreateMaterials();
  virtual void   Init();
  virtual Int_t  IsVersion() const {return 6;}
  virtual void   TOFpc(Float_t xtof,Float_t ytof,Float_t zlenC,Float_t zlenB,
                       Float_t zlenA,Float_t ztof0);
  virtual void   StepManager();
  virtual void   DrawModule() const;
  virtual void   DrawDetectorModules();
  virtual void   DrawDetectorStrips();
 
private:
  Int_t fIdFTOA; // FTOA volume identifier (outer plate A)
  Int_t fIdFTOB; // FTOB volume identifier (outer plate B)
  Int_t fIdFTOC; // FTOC volume identifier (outer plate C)
  Int_t fIdFLTA; // FLTA volume identifier (inner plate A)
  Int_t fIdFLTB; // FLTB volume identifier (inner plate B)
  Int_t fIdFLTC; // FLTC volume identifier (inner plate C)
  Bool_t fTOFHoles; // Selecting Geometry with and w/o holes
 
  ClassDef(AliTOFv4T0,1)  //Time Of Flight version 4
};
 
#endif /* ALITOFv4T0_H */

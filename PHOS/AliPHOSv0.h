#ifndef ALIPHOSV0_H
#define ALIPHOSV0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//_________________________________________________________________________
// Implementation version v0 of PHOS Manager class 
// Layout EMC + CPV  has name IHEP
//*--                  
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---

class TFile;
class TFolder;

// --- AliRoot header files ---
#include "AliPHOS.h"

class AliPHOSv0 : public AliPHOS {

 public:

  AliPHOSv0() {}
  AliPHOSv0(const char *name, const char *title="") ;
  AliPHOSv0(AliPHOSv0 & phos) : AliPHOS(phos) {
    phos.Copy(*this) ; 
  } 
  virtual ~AliPHOSv0(void){
    // dtor
  } 
  virtual void Copy(AliPHOSv0 & phos) ; 

//    virtual void   AddHit( Int_t shunt, Int_t primary, Int_t track, Int_t id, Float_t *hits ) {
  virtual void   AddHit( Int_t, Int_t, Int_t, Int_t, Float_t*) {
    // useless since there are no hits
    Fatal("AddHit", "not to be used with v0") ;
  }
  virtual void   BuildGeometry(void) ;             // creates the geometry for the ROOT display
  void           BuildGeometryforEMC(void) ;      // creates the PHOS geometry for the ROOT display
  //  void           BuildGeometryforPPSD(void) ;      // creates the PPSD geometry for the ROOT display
  void           BuildGeometryforCPV(void) ;       // creates the CPV  geometry for the ROOT display
  virtual void   CreateGeometry(void) ;            // creates the geometry for GEANT
  void           CreateGeometryforEMC(void) ;     // creates the PHOS geometry for GEANT
  //  void           CreateGeometryforPPSD(void) ;     // creates the PPSD geometry for GEANT
  void           CreateGeometryforCPV(void) ;      // creates the CPV  geometry for GEANT
  void           CreateGeometryforSupport(void) ;  // creates the Support geometry for GEANT
  virtual Float_t ZMin() const;                    // overall dimension of the module (min)
  virtual Float_t ZMax() const;                    // overall dimension of the module (max)

  virtual void   Init(void) ;                      // does nothing
  virtual Int_t  IsVersion(void) const { 
    // Gives the version number 
    return 0 ; 
  }
  virtual const TString Version(void)const { 
    // As above
    return TString("v0") ; 
  }
  
  AliPHOSv0 & operator = (const AliPHOSv0 & /*rvalue*/)  {
    // assignement operator requested by coding convention but not needed
    Fatal("operator =", "not implemented") ;
    return *this ; 
  }
  
 protected:
  

  ClassDef(AliPHOSv0,1)  // Implementation of PHOS manager class for layout EMC+PPSD
    
    };
    
#endif // AliPHOSV0_H

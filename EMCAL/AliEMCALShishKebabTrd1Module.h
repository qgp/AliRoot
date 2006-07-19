#ifndef ALIEMCALSHISHKEBABTRD1MODULE_H
#define ALIEMCALSHISHKEBABTRD1MODULE_H

/* Copyright(c) 1998-2004, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
//_________________________________________________________________________
// Main class for TRD1 geometry of Shish-Kebab case.
// Author: Aleksei Pavlinov(WSU).
// Nov 2004; Feb 2006

#include "TNamed.h"
#include "TVector2.h"

class AliEMCALGeometry;

class AliEMCALShishKebabTrd1Module : public TNamed {
 public:
  AliEMCALShishKebabTrd1Module(Double_t theta=0.0, AliEMCALGeometry *g=0);
  AliEMCALShishKebabTrd1Module(AliEMCALShishKebabTrd1Module &leftNeighbor);
  void Init(Double_t A, Double_t B);
  AliEMCALShishKebabTrd1Module(const AliEMCALShishKebabTrd1Module& mod) : TNamed(mod.GetName(),mod.GetTitle()){
    // cpy ctor: no implementation yet; requested by the Coding Convention
    Fatal("cpy ctor", "not implemented") ;  
  }
  AliEMCALShishKebabTrd1Module & operator = (const AliEMCALShishKebabTrd1Module& /*rvalue*/)  {
    Fatal("operator =", "not implemented") ;  
    return *this ; 
  }

  virtual ~AliEMCALShishKebabTrd1Module(void) {}
  Bool_t GetParameters();
  void DefineName(Double_t theta);
  void DefineFirstModule();

  Double_t GetTheta() const {return fTheta;}
  TVector2& GetCenterOfModule() {return fOK;}

  Double_t  GetPosX() const {return fOK.Y();}
  Double_t  GetPosZ() const {return fOK.X();}
  Double_t  GetPosXfromR() const {return fOK.Y() - fgr;}
  Double_t  GetA() const {return fA;}
  Double_t  GetB() const {return fB;}
  //  Additional offline staff 
  //  ieta=0 or 1 - Jun 02, 2006
  TVector2& GetCenterOfCellInLocalCoordinateofSM(Int_t ieta)
  { if(ieta<=0) return fOK2;
    else        return fOK1;}
  void GetCenterOfCellInLocalCoordinateofSM(Int_t ieta, Double_t &xr, Double_t &zr)
  { 
    if(ieta<=0) {xr = fOK2.Y(); zr = fOK2.X();
    } else      {xr = fOK1.Y(); zr = fOK1.X();
    }
  }
  // 15-may-06
  TVector2& GetCenterOfModuleFace() {return fOB;}  
  TVector2& GetCenterOfModuleFace(Int_t ieta) {
    if(ieta<=0) return fOB2;
    else        return fOB1;
  }  
  // 
  Double_t GetTanBetta() const {return fgtanBetta;}
  Double_t Getb()        const {return fgb;}
  // service methods
  void PrintShish(Int_t pri=1) const;  // *MENU*
  Double_t GetThetaInDegree() const;
  Double_t  GetEtaOfCenterOfModule() const;

 protected:
  // geometry info
  static AliEMCALGeometry *fgGeometry; //!
  static Double_t fga;        // 2*dx1=2*dy1
  static Double_t fga2;       // 2*dx2
  static Double_t fgb;        // 2*dz1
  static Double_t fgangle;    // ~1 degree
  static Double_t fgtanBetta; // tan(fgangle/2.)
  static Double_t fgr;        // radius to IP

  TVector2 fOK;     // position the module center in ALICE system; x->y; z->x;
  Double_t fA;      // parameters of right line : y = A*z + B
  Double_t fB;      // system where zero point is IP.
  Double_t fThetaA; // angle coresponding fA - for convinience
  Double_t fTheta;  // theta angle of perependicular to SK module
  // position of towers(cells) with differents ieta (1 or 2) in local coordinate of SM
  // Nov 04,2004; Feb 19,2006 
  TVector2 fOK1; // ieta=1
  TVector2 fOK2; // ieta=0
  // May 13, 2006; local position of module (cells) face  
  TVector2 fOB;  // module
  TVector2 fOB1; // ieta=1
  TVector2 fOB2; // ieta=0

  // public:
  ClassDef(AliEMCALShishKebabTrd1Module,0) // TRD1 Shish-Kebab module 
};

#endif

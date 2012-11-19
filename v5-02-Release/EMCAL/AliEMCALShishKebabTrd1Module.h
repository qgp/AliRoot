#ifndef ALIEMCALSHISHKEBABTRD1MODULE_H
#define ALIEMCALSHISHKEBABTRD1MODULE_H

/* Copyright(c) 1998-2010, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
//_________________________________________________________________________
// Main class for TRD1 geometry of Shish-Kebab case.
// Author: Alexei Pavlinov(WSU).
// Nov 2004; Feb 2006; Apr 2010; Oct 23, 2010

#include <TNamed.h>
#include <TMath.h>
#include <TVector2.h>

class AliEMCALEMCGeometry;

class AliEMCALShishKebabTrd1Module : public TNamed {
 public:
  AliEMCALShishKebabTrd1Module(Double_t theta=0.0, AliEMCALEMCGeometry *g=0);
  AliEMCALShishKebabTrd1Module(AliEMCALShishKebabTrd1Module &leftNeighbor);
  void Init(Double_t A, Double_t B);
  void DefineAllStaff();
  AliEMCALShishKebabTrd1Module(const AliEMCALShishKebabTrd1Module& mod);

  AliEMCALShishKebabTrd1Module & operator = (const AliEMCALShishKebabTrd1Module& /*rvalue*/)  {
    Fatal("operator =", "not implemented") ;  
    return *this ; 
  }

  virtual ~AliEMCALShishKebabTrd1Module(void) {}
  Bool_t GetParameters();
  void DefineName(Double_t theta);
  void DefineFirstModule(const Int_t key=0); // key=0-zero tilt of first module

  Double_t GetTheta() const {return fTheta;}
  TVector2& GetCenterOfModule() {return fOK;}

  Double_t  GetPosX() const {return fOK.Y();}
  Double_t  GetPosZ() const {return fOK.X();}
  Double_t  GetPosXfromR() const {return fOK.Y() - fgr;}
  Double_t  GetA() const {return fA;}
  Double_t  GetB() const {return fB;}
  Double_t  GetRadius() const {return fgr;}
  TVector2  GetORB() const {return fORB;}
  TVector2  GetORT() const {return fORT;}
  //  Additional offline staff 
  //  ieta=0 or 1 - Jun 02, 2006
  TVector2& GetCenterOfCellInLocalCoordinateofSM(Int_t ieta)
  { 
    if(ieta<=0) return fOK2;
    else        return fOK1;
  }
  void GetCenterOfCellInLocalCoordinateofSM(Int_t ieta, Double_t &xr, Double_t &zr, Int_t pri=0) const
  { 
    if(ieta<=0) {xr = fOK2.Y(); zr = fOK2.X();
    } else      {xr = fOK1.Y(); zr = fOK1.X();
    }
    if(pri>0) printf(" %s ieta %2.2i xr %8.4f  zr %8.4f \n", GetName(), ieta, xr, zr);  
  }
  void GetCenterOfCellInLocalCoordinateofSM3X3(Int_t ieta, Double_t &xr, Double_t &zr) const
  { // 3X3 case - Nov 9,2006
    if(ieta < 0) ieta = 0; //ieta = ieta<0? ieta=0 : ieta; // check index
    if(ieta > 2) ieta = 2; //ieta = ieta>2? ieta=2 : ieta;
    xr   = fOK3X3[2-ieta].Y(); zr   = fOK3X3[2-ieta].X();
  }
  void GetCenterOfCellInLocalCoordinateofSM1X1(Double_t &xr, Double_t &zr) const
  { // 1X1 case - Nov 27,2006 // Center of cell is center of module
    xr   = fOK.Y() - fgr;
    zr   = fOK.X();
  }
  // 15-may-06
  TVector2& GetCenterOfModuleFace() {return fOB;}  
  TVector2& GetCenterOfModuleFace(Int_t ieta) {
    if(ieta<=0) return fOB2;
    else        return fOB1;
  }  
  // Jul 30, 2007
  void GetPositionAtCenterCellLine(Int_t ieta, Double_t dist, TVector2 &v);
  // 
  Double_t GetTanBetta() const {return fgtanBetta;}
  Double_t Getb()        const {return fgb;}
  // service methods
  void PrintShish(Int_t pri=1) const;  // *MENU*
  Double_t  GetThetaInDegree() const;
  Double_t  GetEtaOfCenterOfModule() const;
  Double_t  GetMaxEtaOfModule(int pri=0) const;
  static Double_t ThetaToEta(Double_t theta) 
  {return -TMath::Log(TMath::Tan(theta/2.));}

 protected:
  // geometry info
  AliEMCALEMCGeometry *fGeometry; //!
  static Double_t fga;        // 2*dx1=2*dy1
  static Double_t fga2;       // 2*dx2
  static Double_t fgb;        // 2*dz1
  static Double_t fgangle;    // in rad (1.5 degree)
  static Double_t fgtanBetta; // tan(fgangle/2.)
  static Double_t fgr;        // radius to IP

  TVector2 fOK;     // position the module center in ALICE system; x->y; z->x;
  Double_t fA;      // parameters of right line : y = A*z + B
  Double_t fB;      // system where zero point is IP.
  Double_t fThetaA; // angle coresponding fA - for convinience
  Double_t fTheta;  // theta angle of perpendicular to SK module
  // position of towers(cells) with differents ieta (1 or 2) in local coordinate of SM
  // Nov 04,2004; Feb 19,2006 
  TVector2 fOK1; // ieta=1
  TVector2 fOK2; // ieta=0
  // May 13, 2006; local position of module (cells) center face  
  TVector2 fOB;   // module
  TVector2 fOB1;  // ieta=1
  TVector2 fOB2;  // ieta=0
  // Jul 30, 2007
  Double_t fThetaOB1; // theta of cell center line (go through OB1)
  Double_t fThetaOB2;  // theta of cell center line (go through OB2)
  // 3X3 case - Nov 9,2006
  TVector2 fOK3X3[3];
  // Apr 14, 2010 - checking of geometry
  TVector2 fORB; // position of right/bottom point of module
  TVector2 fORT; // position of right/top    point of module
  // public:
  ClassDef(AliEMCALShishKebabTrd1Module, 5) // TRD1 Shish-Kebab module 
};

#endif

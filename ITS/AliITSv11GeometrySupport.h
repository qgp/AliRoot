#ifndef ALIITSV11GEOMETRYSUPPORT_H
#define ALIITSV11GEOMETRYSUPPORT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


// This class Defines the Geometry for the ITS services and support cones
// outside of the ceneteral volume (except for the Ceneteral support 
// cylinders. Other classes define the rest of the ITS. Specificaly the ITS
// The SSD support cone,SSD Support centeral cylinder, SDD support cone,
// The SDD cupport centeral cylinder, the SPD Thermal Sheald, The supports
// and cable trays on both the RB26 (muon dump) and RB24 sides, and all of
// the cabling from the ladders/stave ends out past the TPC. 


/*
  $Id$
 */
#include "AliITSv11Geometry.h"
#include <TGeoManager.h>

class TGeoVolume;

class AliITSv11GeometrySupport : public AliITSv11Geometry {
  public:
    AliITSv11GeometrySupport(){};
    AliITSv11GeometrySupport(Int_t debug):AliITSv11Geometry(debug){};
    virtual ~AliITSv11GeometrySupport(){};
    //
    virtual void SPDCone(TGeoVolume *moth,TGeoManager *mgr=gGeoManager);
    virtual void SDDCone(TGeoVolume *moth,TGeoManager *mgr=gGeoManager);
    virtual void SSDCone(TGeoVolume *moth,TGeoManager *mgr=gGeoManager);
    virtual void ServicesCableSupport(TGeoVolume *moth,
                                      TGeoManager *mgr=gGeoManager);

  private:
    void CreateSPDThermalShape(Double_t ina, Double_t inb, Double_t inr,
			       Double_t oua, Double_t oub, Double_t our,
			       Double_t   t, Double_t *x , Double_t *y );
    void CreateSPDOmegaShape(Double_t *xin, Double_t *yin, Double_t  t,
			     Double_t    d, Double_t   *x, Double_t *y);
    void FillSPDXtruShape(Double_t a, Double_t  b, Double_t  r,
			  Double_t t, Double_t *x, Double_t *y);

    ClassDef(AliITSv11GeometrySupport,1) // ITS v11 Support geometry
};

#endif

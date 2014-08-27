#ifndef ALIITSUV1_H
#define ALIITSUV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//========================================================================
//
//        Geometry for the Upgrade of the Inner Tracking System
//
// Mario Sitta (sitta@to.infn.it)
//
//========================================================================


// $Id: AliITSUv1.h 

#include "AliITSU.h"

class  AliITSUv1Layer;
//class  AliITSv11GeomBeamPipe;
class  TGeoVolume;
class  TGeoVolumeAssembly;

class AliITSUv1 : public AliITSU {

 public:


  typedef enum {
    kIBModelDummy=0,
    kIBModel0=1,
    kIBModel1=2, 
    kIBModel21=3,
    kIBModel22=4,
    kIBModel3=5,
    kOBModelDummy=6,
    kOBModel0=7,
    kOBModel1=8 
  } AliITSUModel_t;
  

  AliITSUv1();
  AliITSUv1(const char *title, Int_t nlay);
  virtual       ~AliITSUv1() ;
  virtual void   SetNWrapVolumes(Int_t n);
  virtual void   AddAlignableVolumes() const;
  void           AddAlignableVolumesLayer(int lr, TString& parent,Int_t &lastUID) const;
  void           AddAlignableVolumesStave(int lr, int st, TString& parent,Int_t &lastUID) const;
  void           AddAlignableVolumesHalfStave(int lr, int st, int sst, TString& parent,Int_t &lastUID) const;
  void           AddAlignableVolumesModule(int lr, int st, int sst, int md, TString& parent,Int_t &lastUID) const;
  void           AddAlignableVolumesChip(int lr, int st, int sst, int md, int ch, TString& parent,Int_t &lastUID) const;

  virtual void   CreateGeometry();
  	  void   CreateSuppCyl(const Bool_t innerBarrel,TGeoVolume *dest,const TGeoManager *mgr=gGeoManager);
  virtual void   CreateMaterials();
  virtual void   DefineLayer(Int_t nlay,Double_t phi0,Double_t r,Double_t zlen,Int_t nstav,
			     Int_t nunit, Double_t lthick=0.,Double_t dthick=0.,UInt_t detType=0, Int_t buildFlag=0);
  virtual void   DefineLayerTurbo(Int_t nlay,Double_t phi0,Double_t r,Double_t zlen,Int_t nstav,
				  Int_t nunit,Double_t width,Double_t tilt,
				  Double_t lthick = 0.,Double_t dthick = 0.,UInt_t detType=0, Int_t buildFlag=0);
  virtual void   GetLayerParameters(Int_t nlay, Double_t &phi0,Double_t &r, Double_t &zlen,
				    Int_t &nstav, Int_t &nmod,
				    Double_t &width, Double_t &tilt,
				    Double_t &lthick, Double_t &mthick,
				    UInt_t &dettype) const;
  virtual void   DefineWrapVolume(Int_t id, Double_t rmin,Double_t rmax, Double_t zspan);
  virtual void   Init(); 
  virtual Bool_t IsLayerTurbo(Int_t nlay);
  virtual Int_t  IsVersion()                 const { return 20;}  // vUpgrade ? do we need this
  virtual void   SetDefaults();
  virtual void   StepManager();
  virtual void   SetLayerChipTypeID(Int_t lr, UInt_t id);
  virtual Int_t  GetLayerChipTypeID(Int_t lr);
  virtual void   SetStaveModelIB(AliITSUModel_t model) {fStaveModelIB=model;}
  virtual void   SetStaveModelOB(AliITSUModel_t model) {fStaveModelOB=model;}
  virtual AliITSUModel_t GetStaveModelIB() const {return fStaveModelIB;}
  virtual AliITSUModel_t GetStaveModelOB() const {return fStaveModelOB;}
  //
 private:
  AliITSUv1(const AliITSUv1 &source); // copy constructor
  AliITSUv1& operator=(const AliITSUv1 &source); // assignment operator

  TGeoVolume* CreateWrapperVolume(Int_t nLay);

  //
  Int_t     fNWrapVol;       // number of wrapper volumes
  Double_t* fWrapRMin;       // min radius of wrapper volume
  Double_t* fWrapRMax;       // max radius of wrapper volume
  Double_t* fWrapZSpan;      // Z span of wrapper volume
  Int_t*    fLay2WrapV;      // id of wrapper layer to which layer belongs (-1 if not wrapped)
  Bool_t   *fLayTurbo;       // True for "turbo" layers
  Double_t *fLayPhi0;        // Vector of layer's 1st stave phi in lab
  Double_t *fLayRadii;       // Vector of layer radii
  Double_t *fLayZLength;     // Vector of layer length along Z
  Int_t    *fStavPerLay;     // Vector of number of staves per layer
  Int_t    *fUnitPerStave;   // Vector of number of "units" per stave
  Double_t *fStaveThick;     // Vector of stave thicknesses
  Double_t *fStaveWidth;     // Vector of stave width (only used for turbo)
  Double_t *fStaveTilt;      // Vector of stave tilt (only used for turbo)
  Double_t *fDetThick;       // Vector of detector thicknesses
  UInt_t   *fChipTypeID;     // Vector of detector type id
  Int_t    *fBuildLevel;     // Vector of Material Budget Studies
  //  
  AliITSUv1Layer **fUpGeom; //! Geometry
  AliITSUModel_t fStaveModelIB; // The stave model for the Inner Barrel
  AliITSUModel_t fStaveModelOB; // The stave model for the Outer Barrel
  
  // Parameters for the Upgrade geometry
  
  ClassDef(AliITSUv1,0)                          
};
 
#endif

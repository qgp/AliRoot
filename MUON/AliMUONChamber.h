#ifndef ALIMUONCHAMBER_H
#define ALIMUONCHAMBER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "TObjArray.h"
#include "AliMUONSegmentation.h"
#include "AliMUONResponse.h"

class AliMUONClusterFinder;
//class AliMUONResponse ;
//class AliMUONSegmentation ;

class AliMUONChamber:
public TObject
{
 public:
    AliMUONChamber();
    AliMUONChamber(const AliMUONChamber & rChamber);
    virtual ~AliMUONChamber();
    
//
// Get GEANT id of sensitive volume
  virtual Int_t   GetGid() {return fGid;}
// Set GEANT id of sensitive volume
  virtual void    SetGid(Int_t id) {fGid=id;}
//  
// Initialisation
  virtual void    Init();
// Set z-position of chamber  
  virtual void    SetZ(Float_t Z) {fZ = Z;}
// Get z-position of chamber  
  virtual Float_t Z(){return fZ;}
// Set inner radius of sensitive volume 
  virtual void SetRInner(Float_t rmin) {frMin=rmin;}
// Set outer radius of sensitive volum  
  virtual void SetROuter(Float_t rmax) {frMax=rmax;}  

// Return inner radius of sensitive volume 
  virtual  Float_t RInner()            {return frMin;}
// Return outer radius of sensitive volum  
  virtual Float_t ROuter()            {return frMax;}  
//  
// Set response model
  virtual void    SetResponseModel(AliMUONResponse* thisResponse) {fResponse=thisResponse;}
//  
// Set segmentation model
  virtual void    SetSegmentationModel(Int_t i, AliMUONSegmentation* thisSegmentation) {
      (*fSegmentation)[i-1] = thisSegmentation;
  }
// Set Cluster reconstruction model  
  virtual void    SetReconstructionModel(AliMUONClusterFinder *thisReconstruction) {
      fReconstruction = thisReconstruction;
  }
//  
//  Get pointer to response model
  virtual AliMUONResponse* &ResponseModel(){return fResponse;}
//  
//  Get reference to segmentation model
  virtual AliMUONSegmentation*  SegmentationModel(Int_t isec) {
      return (AliMUONSegmentation *) (*fSegmentation)[isec-1];
  }
  virtual TObjArray* ChamberSegmentation() {return fSegmentation;}
//  Get pointer to cluster reconstruction model
  virtual AliMUONClusterFinder* &ReconstructionModel(){return fReconstruction;}
// Get number of segmentation sectors  
  virtual Int_t Nsec()              {return fnsec;}
// Set number of segmented cathodes (1 or 2)  
  virtual void  SetNsec(Int_t nsec) {fnsec=nsec;}
//
// Member function forwarding to the segmentation and response models
//
// Calculate pulse height from energy loss  
  virtual Float_t IntPH(Float_t eloss) {return fResponse->IntPH(eloss);}
//  
// Ask segmentation if signal should be generated  
  virtual Int_t   SigGenCond(Float_t x, Float_t y, Float_t z);
//
// Initialisation of segmentation for hit  
  virtual void    SigGenInit(Float_t x, Float_t y, Float_t z);
// Configuration forwarding
//
// Define signal distribution region
// by number of sigmas of the distribution function
  virtual void   SetSigmaIntegration(Float_t p1)
      {fResponse->SetSigmaIntegration(p1);}
// Set the single electron pulse-height (ADCchan/e)  
  virtual void   SetChargeSlope(Float_t p1)              {fResponse->SetChargeSlope(p1);}
// Set width of charge distribution function  
  virtual void   SetChargeSpread(Float_t p1, Float_t p2) {fResponse->SetChargeSpread(p1,p2);}
// Set maximum ADC count value
  virtual void   SetMaxAdc(Int_t p1)                   {fResponse->SetMaxAdc(p1);}
// Set Pad size
  virtual void   SetPadSize(Int_t isec, Float_t p1, Float_t p2) {
      ((AliMUONSegmentation*) (*fSegmentation)[isec-1])->SetPadSize(p1,p2);
  }
//  
// Cluster formation method (charge disintegration)
  virtual void   DisIntegration(Float_t eloss, Float_t tof,
				Float_t xhit, Float_t yhit,
				Int_t& x, Float_t newclust[6][500]);
// Initialize geometry related parameters  
  virtual void    InitGeo(Float_t z);
//
  virtual Float_t DGas() {return fdGas;}
  virtual Float_t DAlu() {return fdAlu;}  
// assignment operator  
  virtual AliMUONChamber& operator =(const AliMUONChamber& rhs);
  
 protected:

  Float_t fdGas; // half gaz gap
  Float_t fdAlu; // half Alu width  
  Int_t   fGid;  // GEANT volume if for sensitive volume of this chamber
  Float_t fZ;    // Z position (cm)
  Int_t   fnsec; // number of semented cathode planes
  Float_t frMin; // innermost sensitive radius
  Float_t frMax; // outermost sensitive radius

  TObjArray            *fSegmentation;    // pointer to segmentation
  AliMUONClusterFinder *fReconstruction;  // pointer to reconstruction
  AliMUONResponse      *fResponse;        // pointer to response
  ClassDef(AliMUONChamber,1) // Muon tracking and trigger chamber class
};

#endif

#ifndef ALIZDCV3_H
#define ALIZDCV3_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//  Manager and hits classes for set: ZDC     //
////////////////////////////////////////////////

#include "AliZDC.h"

//____________________________________________________________________________ 
class AliZDCv3 : public AliZDC {

public:
  AliZDCv3();
  AliZDCv3(const char *name, const char *title);
  virtual  ~AliZDCv3() {}
  virtual void  CreateGeometry();
  virtual void  CreateBeamLine();
  virtual void  CreateZDC();
  virtual void  CreateMaterials();
  virtual Int_t IsVersion() const {return 1;}
  virtual void  DrawModule() const;
  virtual void  AddAlignableVolumes() const;
  virtual void  Init();
  virtual void  InitTables();
  virtual void  StepManager();
  
 
protected:

  // Sensitive media
  Int_t   fMedSensF1;         // Sensitive medium F1
  Int_t   fMedSensF2;         // Sensitive medium F2
  Int_t   fMedSensZP;         // Sensitive medium for ZP
  Int_t   fMedSensZN;         // Sensitive medium for ZN
  Int_t   fMedSensZEM;        // Sensitive medium for EM ZDC
  Int_t   fMedSensGR;         // Other sensitive medium
  Int_t   fMedSensPI;         // Beam pipe and magnet coils
  Int_t   fMedSensTDI;        // Cu materials along beam pipe
  
  // Parameters for light tables
  Int_t   fNalfan;	      // Number of Alfa (neutrons)
  Int_t   fNalfap;	      // Number of Alfa (protons)
  Int_t   fNben;	      // Number of beta (neutrons)
  Int_t   fNbep;	      // Number of beta (protons)
  Float_t fTablen[4][90][18]; // Neutrons light table
  Float_t fTablep[4][90][28]; // Protons light table

  // Parameters for hadronic calorimeters geometry
  // NB -> parameters used in CreateZDC() and in StepManager()
  // (other parameters are defined in CreateZDC())
  Float_t fDimZN[3];	// Dimensions of proton detector
  Float_t fDimZP[3];	// Dimensions of proton detector
  Float_t fPosZNC[3];   // Position of neutron detector side C
  Float_t fPosZNA[3];   // Position of neutron detector side A  
  Float_t fPosZPC[3]; 	// Position of proton detector side C
  Float_t fPosZPA[3]; 	// Position of proton detector side A
  Float_t fFibZN[3]; 	// Fibers for neutron detector
  Float_t fFibZP[3];  	// Fibers for proton detector

  // Parameters for EM calorimeter geometry
  // NB -> parameters used in CreateZDC() and in StepManager()
  // (other parameters are defined in CreateZDC())
  Float_t fPosZEM[3]; // Position of EM detector
  Float_t fZEMLength; // ZEM length
  
  // Parameters for proton accepancy studies
  Int_t fpLostITC, fpLostD1C, fpDetectedC, fnDetectedC; // Side C
  Int_t fpLostITA, fpLostD1A, fpLostTDI, fpDetectedA, fnDetectedA; // Side A
  
  
   ClassDef(AliZDCv3,2)  // Zero Degree Calorimeter version 1
}; 
 
#endif

// -*- mode: C++ -*- 
#ifndef ALIESDRUN_H
#define ALIESDRUN_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                     Implementation Class AliESDRun
//   Run by run data
//   for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>
#include <TObjArray.h>
#include <TString.h>

class TGeoHMatrix;
class AliESDVertex;

class AliESDRun: public TObject {
public:

  enum StatusBits {kBInfoStored = BIT(14), kUniformBMap = BIT(15), kConvSqrtSHalfGeV = BIT(16)};

  AliESDRun();
  AliESDRun(const AliESDRun& esd);
  AliESDRun& operator=(const AliESDRun& esd);
  virtual void Copy(TObject &obj) const; // Interface for using TOBject::Copy()
  virtual ~AliESDRun();

  Bool_t  InitMagneticField() const;
  Int_t   GetRunNumber() const {return fRunNumber;}
  void    SetRunNumber(Int_t n) {fRunNumber=n;}
  void    SetMagneticField(Float_t mf){fMagneticField = mf;}
  Double_t GetMagneticField() const {return fMagneticField;}
  UInt_t   GetPeriodNumber() const {return fPeriodNumber;}
  void    SetPeriodNumber(Int_t n) {fPeriodNumber=n;}
  void    Reset();
  void    Print(const Option_t *opt=0) const;
  void    SetDiamond(const AliESDVertex *vertex);
  void    SetTriggerClass(const char*name, Int_t index);
  void    SetCurrentL3(Float_t cur)    {fCurrentL3 = cur;}
  void    SetCurrentDip(Float_t cur)   {fCurrentDip = cur;}
  void    SetBeamEnergy(Float_t be)    {fBeamEnergy = be;}
  void    SetBeamType(const char* bt)  {fBeamType = bt;}
  void    SetBeamEnergyIsSqrtSHalfGeV(Bool_t v=kTRUE) {SetBit(kConvSqrtSHalfGeV,v);}

  Bool_t  IsBeamEnergyIsSqrtSHalfGeV() const {return TestBit(kConvSqrtSHalfGeV);}  
  Double_t GetDiamondX() const {return fDiamondXY[0];}
  Double_t GetDiamondY() const {return fDiamondXY[1];}
  Double_t GetSigma2DiamondX() const {return fDiamondCovXY[0];}
  Double_t GetSigma2DiamondY() const {return fDiamondCovXY[2];}
  void GetDiamondCovXY(Float_t cov[3]) const {
    for(Int_t i=0;i<3;i++) cov[i]=fDiamondCovXY[i]; return;
  }
  const char* GetTriggerClass(Int_t index) const;
  TString     GetActiveTriggerClasses() const;
  TString     GetFiredTriggerClasses(ULong64_t mask) const;
  Bool_t      IsTriggerClassFired(ULong64_t mask, const char *name) const;
  Float_t     GetCurrentL3()               const {return fCurrentL3;}
  Float_t     GetCurrentDip()              const {return fCurrentDip;}
  Float_t     GetBeamEnergy()              const {return IsBeamEnergyIsSqrtSHalfGeV() ? fBeamEnergy : fBeamEnergy/2;}
  const char* GetBeamType()                const {return fBeamType.Data();}

  void    SetPHOSMatrix(TGeoHMatrix*matrix, Int_t i) {
    if ((i >= 0) && (i < kNPHOSMatrix)) fPHOSMatrix[i] = matrix;
  }
  const TGeoHMatrix* GetPHOSMatrix(Int_t i) const {
    return ((i >= 0) && (i < kNPHOSMatrix)) ? fPHOSMatrix[i] : NULL;
  }
	
  void    SetEMCALMatrix(TGeoHMatrix*matrix, Int_t i) {
	if ((i >= 0) && (i < kNEMCALMatrix)) fEMCALMatrix[i] = matrix;
  }
  const TGeoHMatrix* GetEMCALMatrix(Int_t i) const {
	return ((i >= 0) && (i < kNEMCALMatrix)) ? fEMCALMatrix[i] : NULL;
  }
	
  enum {kNTriggerClasses = 50};
  enum {kNPHOSMatrix = 5};
  enum {kNEMCALMatrix = 12};

private:
  Float_t         fCurrentL3;       // signed current in the L3     (LHC convention: +current -> +Bz)
  Float_t         fCurrentDip;      // signed current in the Dipole (LHC convention: +current -> -Bx)
  Float_t         fBeamEnergy;      // beamEnergy entry from GRP
  Double32_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF
  Double32_t      fDiamondXY[2];    // Interaction diamond (x,y) in RUN
  Double32_t      fDiamondCovXY[3]; // Interaction diamond covariance (x,y) in RUN
  UInt_t          fPeriodNumber;    // PeriodNumber
  Int_t           fRunNumber;       // Run Number
  Int_t           fRecoVersion;     // Version of reconstruction
  TString         fBeamType;        // beam type from GRP
  TObjArray       fTriggerClasses;  // array of TNamed containing the names of the active trigger classes
  TGeoHMatrix*    fPHOSMatrix[kNPHOSMatrix]; //PHOS module position and orientation matrices
  TGeoHMatrix*    fEMCALMatrix[kNEMCALMatrix]; //EMCAL supermodule position and orientation matrices

  ClassDef(AliESDRun,6)
};

#endif 

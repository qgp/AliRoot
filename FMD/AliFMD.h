#ifndef ALIFMD_H
#define ALIFMD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//  Manager and hits classes for set:Si-FMD     //
////////////////////////////////////////////////
 
#include "AliDetector.h"
#include "TString.h"
#include "TBranch.h"
//#include "AliFMDMerger.h" 
#include "AliFMDSDigitizer.h" 
 class TFile;
 class TTree;
// class AliFMDMerger;
 class AliFMD : public AliDetector {
 
public:
  AliFMD();
  AliFMD(const char *name, const char *title);
  virtual       ~AliFMD(); 
  virtual void   AddHit(Int_t, Int_t*, Float_t*);
  virtual void   AddDigit(Int_t*);
  virtual void   AddSDigit(Int_t*);
  virtual void   BuildGeometry();
  virtual void   CreateGeometry() {}
  virtual void   CreateMaterials()=0; 
  virtual Int_t  DistanceToPrimitive(Int_t px, Int_t py);
  virtual Int_t  IsVersion() const =0;
  virtual void   Init();
  virtual void   MakeBranch(Option_t *opt=" ");
  virtual void   MakeBranchInTreeD(TTree *treeD, const char *file=0);
  virtual void   SetTreeAddress();
  virtual void   ResetHits();
  virtual void   ResetDigits();
  virtual void   DrawDetector()=0;
  virtual void   StepManager() {}
   // Granularity
  virtual void SetRingsSi1(Int_t ringsSi1=256);
  virtual void SetSectorsSi1(Int_t sectorsSi1=20);
  virtual void SetRingsSi2(Int_t ringsSi2=128);
  virtual void SetSectorsSi2(Int_t sectorsSi2=40);
   
  void SetEventNumber(Int_t i)     {fEvNrSig = i;}
  void  Eta2Radius(Float_t, Float_t, Float_t*);
  void Hits2SDigits();//
  void Digits2Reco(); 
  virtual void SetHitsAddressBranch(TBranch *b){b->SetAddress(&fHits);}
  
   // Digitisation
  TClonesArray *SDigits() const {return fSDigits;}
  TClonesArray *ReconParticles() const {return fReconParticles;}   

 protected:
  Int_t fIdSens1;     //Si sensetive volume
  Int_t fIdSens2;     //Si sensetive volume
  Int_t fIdSens3;     //Si sensetive volume
  Int_t fIdSens4;     //Si sensetive volume
  Int_t fIdSens5;     //Si sensetive volume
 //Granularity
  Int_t fRingsSi1;       // Number of rings
  Int_t fSectorsSi1;    // Number of sectors
  Int_t fRingsSi2;       // Number of rings
  Int_t fSectorsSi2;    // Number of sectors

  Int_t   fNevents ;        // Number of events to digitize
  Int_t fEvNrSig;                 // signal     event number

  //  AliFMDMerger *fMerger;   // ! pointer to merger
  TClonesArray *fSDigits      ; // List of summable digits
  TClonesArray *fReconParticles;

 ClassDef(AliFMD,4)  //Class for the FMD detector
};
#endif // AliFMD_H



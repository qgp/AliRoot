#ifndef ALITRDDIGITIZER_H
#define ALITRDDIGITIZER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliDigitizer.h"

class TFile;
class TF1;

class AliRunDigitizer;
class AliRunLoader;

class AliTRD;
class AliTRDdigitsManager;
class AliTRDgeometry;
class AliTRDparameter;

///////////////////////////////////////////////////////
//  Produces digits from the hits information        //
///////////////////////////////////////////////////////

class AliTRDdigitizer : public AliDigitizer {

 public:

  AliTRDdigitizer();
  AliTRDdigitizer(const Text_t* name, const Text_t* title);
  AliTRDdigitizer(AliRunDigitizer *manager, const Text_t* name, const Text_t* title);
  AliTRDdigitizer(AliRunDigitizer *manager);
  AliTRDdigitizer(const AliTRDdigitizer &d);
  virtual ~AliTRDdigitizer();
  AliTRDdigitizer &operator=(const AliTRDdigitizer &d);

  virtual void         Copy(TObject &d) const;
  virtual Bool_t       InitDetector();
  virtual void         Exec(Option_t* option = 0);  
  virtual Bool_t       Open(const Char_t *file, Int_t nEvent = 0);
  virtual Bool_t       MakeBranch(TTree* tree) const;
  virtual Bool_t       MakeDigits();
  virtual void         AddSDigitsManager(AliTRDdigitsManager *manager);
  virtual void         DeleteSDigitsManager();
  virtual Bool_t       ConvertSDigits();
  virtual Bool_t       MergeSDigits();
  virtual Bool_t       SDigits2Digits();
  virtual Bool_t       WriteDigits() const;

          void         InitOutput(Int_t iEvent);
 
  virtual void         SetCompress(Int_t c = 1)             { fCompress        = c;   };
  virtual void         SetDebug(Int_t v = 1)                { fDebug           = v;   };
  virtual void         SetSDigits(Int_t v = 1)              { fSDigits         = v;   };
  virtual void         SetSDigitsScale(Float_t s)           { fSDigitsScale    = s;   };
  virtual void         SetEvent(Int_t v = 0)                { fEvent           = v;   };
  virtual void         SetManager(AliTRDdigitsManager *man) { fDigitsManager   = man; };   
  virtual void         SetGeometry(AliTRDgeometry *geo)     { fGeo             = geo; };
  virtual void         SetParameter(AliTRDparameter *par)   { fPar             = par; };
  virtual void         SetMergeSignalOnly(Bool_t m = kTRUE) { fMergeSignalOnly = m;   };
  virtual void         SetSimple(Int_t v = 1)               { fSimpleSim       = v;
                                                              fSimpleDet       = 12;
                                                              fCompress        = kFALSE; };

  AliTRDdigitsManager *Digits()                       const { return fDigitsManager; };

  Bool_t       GetCompress()                  const { return fCompress;      };
  Bool_t       GetSDigits()                   const { return fSDigits;       };
  Float_t      GetSDigitsScale()              const { return fSDigitsScale;  };
  AliTRDparameter     *GetParameter()                 const { return fPar;           };
  Bool_t       GetSimple()                    const { return fSimpleSim;     };

  virtual Double_t TimeStruct(Float_t vdrift, Double_t time, Double_t z);  

  Float_t GetDiffusionT(Float_t vdrift);
  Float_t GetDiffusionL(Float_t vdrift);
  virtual Int_t Diffusion(Float_t vdrift, Double_t driftlength, Double_t *xyz);

  Float_t GetLorentzFactor(Float_t vdrift);
  virtual Int_t ExB(Float_t vdrift, Double_t driftlength, Double_t *xyz);  
  
  protected:

  //TFile               *fInputFile;          //! ALIROOT-file
  AliRunLoader        *fRunLoader;          //! Local pointer
  AliTRDdigitsManager *fDigitsManager;      //! Manager for the output digits
  AliTRDdigitsManager *fSDigitsManager;     //! Manager for the summed input s-digits
  TList               *fSDigitsManagerList; //! List of managers of input s-digits
  AliTRD              *fTRD;                //! TRD detector class
  AliTRDgeometry      *fGeo;                //! TRD geometry
  AliTRDparameter     *fPar;          //  TRD common parameter object
  Int_t                fEvent;              //! Event number
  Int_t               *fMasks;              //! Masks for the merging
  Bool_t               fCompress;           //  Switch to keep only compressed data in memory
  Int_t                fDebug;              //  Sets the debug level
  Bool_t               fSDigits;            //  Switch for the summable digits
  Float_t              fSDigitsScale;       //  Scale factor for the summable digits 
  Bool_t               fMergeSignalOnly;    //  Merge only detectors that contain a signal
  Bool_t               fSimpleSim;          //  Switch for the simplified simulation
  Int_t                fSimpleDet;          //  Detecttor number used in the simplified simulation
 
  void SampleTimeStruct(Float_t vdrift);
  void RecalcDiffusion(Float_t vdrift);
  
  struct
  {
    Float_t fLastVdrift;                                      //  the structures are valid for fLastVdrift (caching)
    Float_t fDiffusionT;
    Float_t fDiffusionL;
    Float_t              fOmegaTau;                           //  Tangens of the Lorentz angle
    Float_t              fLorentzFactor;                      //  Factor due to Lorentz force
  } fDiffusionInfo;
    
  struct 
  {
    Float_t fLastVdrift;                                      //  the structures are valid for fLastVdrift (caching)
    Float_t             *fTimeStruct1;                        //! Time Structure of Drift Cells
    Float_t             *fTimeStruct2;                        //! Time Structure of Drift Cells
    Float_t              fVDlo;                               //  Lower drift velocity, for interpolation
    Float_t              fVDhi;                               //  Higher drift velocity, for interpolation
  } fTimeStructInfo;
  
  private:
    
  virtual Bool_t Init();
    
  virtual void         DeConvExp(Double_t *source, Double_t *target, Int_t n, Int_t nexp);
  virtual Bool_t       CheckDetector(Int_t plane, Int_t chamber, Int_t sector);

  ClassDef(AliTRDdigitizer,8)               //  Produces TRD-Digits

};

#endif

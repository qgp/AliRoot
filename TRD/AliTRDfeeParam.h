#ifndef ALITRDFEEPARAM_H
#define ALITRDFEEPARAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//////////////////////////////////////////////////
//                                              //
//  TRD front end electronics parameters class  //
//  Contains all FEE (MCM, TRAP, PASA) related  //
//  parameters, constants, and mapping.         //
//                                              //
//////////////////////////////////////////////////

#include <TObject.h>

class AliTRDCommonParam;
class AliTRDpadPlane;
class AliTRDgeometry;

//_____________________________________________________________________________
class AliTRDfeeParam : public TObject
{

 public:

  AliTRDfeeParam(const AliTRDfeeParam &p);
  virtual           ~AliTRDfeeParam();
  AliTRDfeeParam    &operator=(const AliTRDfeeParam &p);
  virtual void       Copy(TObject &p) const;

  static AliTRDfeeParam *Instance();  // Singleton
  static void            Terminate();

  // Translation from MCM to Pad and vice versa
  virtual Int_t    GetPadRowFromMCM(Int_t irob, Int_t imcm) const;
  virtual Int_t    GetPadColFromADC(Int_t irob, Int_t imcm, Int_t iadc) const;
  virtual Int_t    GetMCMfromPad(Int_t irow, Int_t icol) const;
  virtual Int_t    GetROBfromPad(Int_t irow, Int_t icol) const;
  virtual Int_t    GetRobSide(Int_t irob) const;
  virtual Int_t    GetColSide(Int_t icol) const;

  static  Float_t  GetSamplingFrequency() { return (Float_t)fgkLHCfrequency / 4000000.0; }
  static  Int_t    GetNmcmRob()           { return fgkNmcmRob; }
  static  Int_t    GetNmcmRobInRow()      { return fgkNmcmRobInRow; }
  static  Int_t    GetNmcmRobInCol()      { return fgkNmcmRobInCol; }
  static  Int_t    GetNrobC0()            { return fgkNrobC0; }
  static  Int_t    GetNrobC1()            { return fgkNrobC1; }
  static  Int_t    GetNadcMcm()           { return fgkNadcMcm; }
  static  Int_t    GetNtimebin()          { return fgkNtimebin; }
  static  Int_t    GetNcol()              { return fgkNcol; }
  static  Int_t    GetNcolMcm()           { return fgkNcolMcm; }
  static  Int_t    GetNrowC0()            { return fgkNrowC0; }
  static  Int_t    GetNrowC1()            { return fgkNrowC1; }

  //          Float_t  GetClusThr()           { return fClusThr; };
  //        Float_t  GetPadThr() const { return fPadThr; };
  //        Int_t    GetTailCancelation() const { return fTCOn; };
  //        Int_t    GetNexponential() const { return fTCnexp; };
  //virtual void     GetFilterParam(Float_t &r1, Float_t &r2, Float_t &c1, Float_t &c2, Float_t &ped) const;
  //        Int_t    GetFilterType() const { return fFilterType; };

  static  Float_t  GetTFattenuationParam() { return ((Float_t)fgkTFattenuationParameter1) / ((Float_t)fgkTFattenuationParameter2) ; }
  static  Float_t  GetTFf0()               { return 1 + fgkTFon*(-1+GetTFattenuationParam()); }   // 1 if TC off

 protected:

  static AliTRDfeeParam *fgInstance;
  static Bool_t          fgTerminated;       //  Defines if this class has already been terminated                                                        

  //  AliTRDgeometry    *fGeo;     // TRD geometry class
  AliTRDCommonParam *fCP;      // TRD common parameters class

  // Remark: ISO C++ allows initialization of static const values only for integer.

  // Basic Geometrical numbers
  static const Int_t    fgkLHCfrequency      = 40079000 ; // [Hz] LHC clock (should be moved to STEER?)
  static const Int_t    fgkNmcmRob           = 16;        // Number of MCMs per ROB         (old fgkMCMmax)
  static const Int_t    fgkNmcmRobInRow      = 4;         // Number of MCMs per ROB in row dir. (old fgkMCMrow)
  static const Int_t    fgkNmcmRobInCol      = 4;         // Number of MCMs per ROB in col dir. (old fgkMCMrow)
  static const Int_t    fgkNrobC0            = 6;         // Number of ROBs per C0 chamber  (old fgkROBmaxC0)
  static const Int_t    fgkNrobC1            = 8;         // Number of ROBs per C1 chamber  (old fgkROBmaxC1)
  static const Int_t    fgkNadcMcm           = 21;        // Number of ADC channels per MCM (old fgkADCmax)
  static const Int_t    fgkNtimebin          = 30;        // Number of Time bins            (old fgkTBmax)
  static const Int_t    fgkNcol              = 144;       // Number of pads per padplane row(old fgkColmax)
  static const Int_t    fgkNcolMcm           = 18;        // Number of pads per MCM         (old fgkPadmax)
  static const Int_t    fgkNrowC0            = 12;        // Number of Rows per C0 chamber  (old fgkRowmaxC0)
  static const Int_t    fgkNrowC1            = 16;        // Number of Rows per C1 chamber  (old fgkRowmaxC1)

  // ADC intrinsic parameters
  static const Int_t    fgkADCpedestal       = 100000;    // ADC baseline * 100 (old name fPedestal)
  static const Int_t    fgkADCnoise          = 10;        // ADC noise    * 100 (not contained in the digitizer) [in ADC] 
  static const Int_t    fgkADCDAC            = 0;         // 5 bit ADC gain parameter

  // TRAP filter global setup
  static const Bool_t   fgkPFon              = kTRUE;     // Pedestal Filter enable/disable flag.
  static const Bool_t   fgkGFon              = kFALSE;    // Gain correction Filter enable/disable flag
  static const Bool_t   fgkTFon              = kTRUE;     // Tail cancelation Filter enable/disable flag (old name fTCOn)

  // PF setup
  static const Int_t    fgkPFtimeConstant    =  0;        // 0 for fastest, 3 for slowest (no effect, probably)
  static const Int_t    fgkPFeffectPedestal  = 10;        // [in ADC units] the desired baseline (Additive)

  // GF setup
  static const Int_t    fgkGFnoise           =  0;        // Noise level increased by gain filter x 100 [in ADC] (to be measured)

  // TF setup
  static const Int_t    fgkTFtype                 = 2;    // TC type (0=analog, 1=digital, 2=TRAPsim) (old name fFilterType)
  static const Int_t    fgkTFlongDecayWeight      = 270;  // 0 to 1024 corresponds to 0 to 0.5
  static const Int_t    fgkTFlongDecayParameter   = 348;  // 0 to 511 corresponds to 0.75 to 1
  static const Int_t    fgkTFshortDecayParameter  = 449;  // 0 to 511 correponds to 0.25 to 0.5
  static const Int_t    fgkTFattenuationParameter1= 45;   // attenuationParameter = fgkTFattenuationParameter1/fgkTFattenuationParameter2
  static const Int_t    fgkTFattenuationParameter2= 14;   //                      = -alphaL/ln(lambdaL)-(1-alphaL)/ln(lambdaS)

  // OLD TF setup (calculated from above)  (valid only for fgkTFtype = 0 or 1)
  static const Int_t    fgkTFaNExp                 = 1;    // Number of exponential
               Float_t  fTFaR1;                        // Time constant [microseconds] long (old name fR1)
               Float_t  fTFaR2;                        // Time constant [microseconds] short(old name fR2)
               Float_t  fTFaC1;                        // Weight long  (old name fC1)
               Float_t  fTFaC2;                        // Weight short (old name fC2)

  // Zero suppression parameters
  static const Int_t    fgkEBsingleIndicatorThreshold = 3;    // used in EBIS, in ADC units above the pedestal
  static const Int_t    fgkEBsumIndicatorThreshold    = 4;    // used in EBIT, in ADC units above the pedestal
  static const Int_t    fgkEBindicatorLookupTable     = 0xF0; // see the TRAP user manual, used in EBIL
  static const Int_t    fgkEBmarkIgnoreNeighbour      = 1;    // used in EBIN

  // Charge accumulators
  static const Int_t    fgkPREPqAcc0Start             =  0;   // Preprocessor Charge Accumulator 0 Start
  static const Int_t    fgkPREPqAcc0End               = 10;   // Preprocessor Charge Accumulator 0 End
  static const Int_t    fgkPREPqAcc1Start             = 11;   // Preprocessor Charge Accumulator 1 Start
  static const Int_t    fgkPREPqAcc1End               = 20;   // Preprocessor Charge Accumulator 1 End
  static const Int_t    fgkMinClusterCharge           = 20;   // Hit detection [in ADC units]

  // OLD TRAP processing parameters calculated from above
  //static const Float_t  fClusThr;                     // Cluster threshold
  //static const Float_t  fPadThr;                      // Pad threshold

  // For raw production
  static const Int_t    fgkRAWversion            = 1;         // Raw data production version
  static const Bool_t   fgkRAWstoreRaw           = kTRUE;     // Store unfiltered data for raw data stream

 private:

  AliTRDfeeParam();

  ClassDef(AliTRDfeeParam,1)  //
};

#endif

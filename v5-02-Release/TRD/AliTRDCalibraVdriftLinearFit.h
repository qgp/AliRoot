#ifndef ALITRDCALIBRAVDRIFTLINEARFIT_H
#define ALITRDCALIBRAVDRIFTLINEARFIT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for online calibration                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//#include "TObjArray.h"
#include "TObject.h"

//class TVectorD;
class TObjArray;
class TH2S;
class TH2;
class TTreeSRedirector;
class TString;
class TGraphErrors;
class TLinearFitter;
class TTreeSRedirector;

class AliTRDCalibraVdriftLinearFit : public TObject {

 public:

  AliTRDCalibraVdriftLinearFit();
  AliTRDCalibraVdriftLinearFit(const AliTRDCalibraVdriftLinearFit &ped);
  AliTRDCalibraVdriftLinearFit(const TObjArray &obja);
  virtual ~AliTRDCalibraVdriftLinearFit();
  virtual Long64_t Merge(const TCollection* list);
  virtual void Copy(TObject &c) const;

  AliTRDCalibraVdriftLinearFit& operator = (const  AliTRDCalibraVdriftLinearFit &source);

  void            Update(Int_t detector, Float_t tnp, Float_t pars1);
  void            FillPEArray();
  void            FillPEArray2();
  void            SetNameCalibUsed(const char*name)          { fNameCalibUsed = name;};
  const char*     GetNameCalibUsed() const                   { return fNameCalibUsed;};
  void            Add(const AliTRDCalibraVdriftLinearFit *ped);
  TH2S           *GetLinearFitterHisto(Int_t detector, Bool_t force=kFALSE);
  TH2S           *GetLinearFitterHistoForce(Int_t detector);
  TH2S           *GetLinearFitterHistoNoForce(Int_t detector) const   { return (TH2S*)fLinearFitterHistoArray.UncheckedAt(detector);};
  Bool_t          GetParam(Int_t detector, TVectorD *param);
  Bool_t          GetError(Int_t detector, TVectorD *error);
  Int_t           GetMinNumberOfPointsForFit() const         { return fMinNpointsFit;};

  TObjArray      *GetPArray()                    { return &fLinearFitterPArray;       };
  TObjArray      *GetEArray()                    { return &fLinearFitterEArray;       };
  TObjArray       GetHistoArray() const          { return fLinearFitterHistoArray;    };
  void            SetRobustFit(Bool_t robustFit) { fRobustFit = robustFit;            };
  void            SetMinNumberOfPointsForFit(Int_t minNpointsFit) { fMinNpointsFit = minNpointsFit;};

  // Debug
  void     SetDebugLevel(Short_t level)          { fDebugLevel = level;               };
  void     SetSeeDetector(Int_t seeDetector)     { fSeeDetector = seeDetector;        };

private:
  TGraphErrors   *DrawMS(const TH2 *const h2, Int_t &nEntries);

  Int_t           fVersion;                 // Version of the object
  TString         fNameCalibUsed;           // Info of the version, subversion, firstrun of the calib used

  TObjArray       fLinearFitterHistoArray;  // TObjArray of histo2D for debugging Linear Fitters
  TObjArray       fLinearFitterPArray;      // Array of result parameters from linear fitters for the detectors
  TObjArray       fLinearFitterEArray;      // Array of result errors from linear fitters for the detectors
  Bool_t          fRobustFit;               // Robust fit or not
  Int_t           fMinNpointsFit;           // Min number of points for the fit  

 //For debugging
  TTreeSRedirector *fDebugStreamer;         //!Debug streamer
  Short_t           fDebugLevel;            // Flag for debugging
  Int_t             fSeeDetector;           // Detector to see
 
  ClassDef(AliTRDCalibraVdriftLinearFit,4)  // Online Vdrift calibration

};



#endif


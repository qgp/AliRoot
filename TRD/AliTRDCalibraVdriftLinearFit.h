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
  void            Add(const AliTRDCalibraVdriftLinearFit *ped);
  TH2S           *GetLinearFitterHisto(Int_t detector, Bool_t force=kFALSE);
  TH2S           *GetLinearFitterHistoForce(Int_t detector);
  TH2S           *GetLinearFitterHistoNoForce(Int_t detector) const   { return (TH2S*)fLinearFitterHistoArray.UncheckedAt(detector);};
  Bool_t          GetParam(Int_t detector, TVectorD *param);
  Bool_t          GetError(Int_t detector, TVectorD *error);

  TObjArray      *GetPArray()                    { return &fLinearFitterPArray;       };
  TObjArray      *GetEArray()                    { return &fLinearFitterEArray;       };
  TObjArray       GetHistoArray() const          { return fLinearFitterHistoArray;    };

 private:
   
  Int_t           fVersion;                 // Version of the object

  TObjArray       fLinearFitterHistoArray;  // TObjArray of histo2D for debugging Linear Fitters
  TObjArray       fLinearFitterPArray;      // Array of result parameters from linear fitters for the detectors
  TObjArray       fLinearFitterEArray;      // Array of result errors from linear fitters for the detectors

  
  ClassDef(AliTRDCalibraVdriftLinearFit,1)  // Online Vdrift calibration

};



#endif


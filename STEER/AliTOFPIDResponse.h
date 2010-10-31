#ifndef ALITOFPIDRESPONSE_H
#define ALITOFPIDRESPONSE_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------
//                    TOF PID class
//   Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------

#include "TObject.h"
#include "AliPID.h"

class AliTOFPIDResponse : public TObject {
public:

  AliTOFPIDResponse();
  AliTOFPIDResponse(Double_t *param);
  ~AliTOFPIDResponse(){}

  void     SetTimeResolution(Float_t res) { fSigma = res; }
  void     SetTimeZero(Double_t t0) { fTime0=t0; }
  Double_t GetTimeZero() const { return fTime0; }

  void     SetMaxMismatchProbability(Double_t p) {fPmax=p;}
  Double_t GetMaxMismatchProbability() const {return fPmax;}

  Double_t GetExpectedSigma(Float_t mom, Float_t tof, Float_t mass) const;


  Double_t GetMismatchProbability(Double_t p,Double_t mass) const;

  void     SetT0event(Float_t *t0event){for(Int_t i=0;i < fNmomBins;i++) fT0event[i] = t0event[i];};
  void     SetT0resolution(Float_t *t0resolution){for(Int_t i=0;i < fNmomBins;i++) fT0resolution[i] = t0resolution[i];};
  void     ResetT0info(){ for(Int_t i=0;i < fNmomBins;i++){ fT0event[i] = 0.0; fT0resolution[i] = 0.0;} };
  void     SetMomBoundary();
  Int_t    GetMomBin(Float_t p) const;
  Int_t    GetNmomBins(){return fNmomBins;};
  Float_t  GetMinMom(Int_t ibin) const {if(ibin >=0 && ibin <= fNmomBins) return fPCutMin[ibin]; else return 0.0;};
  Float_t  GetMaxMom(Int_t ibin)const {if(ibin >=0 && ibin <= fNmomBins) return fPCutMin[ibin+1]; else return 0.0;};
  void     SetT0bin(Int_t ibin,Float_t t0bin){if(ibin >=0 && ibin <= fNmomBins) fT0event[ibin] = t0bin;};
  void     SetT0binRes(Int_t ibin,Float_t t0binRes){if(ibin >=0 && ibin <= fNmomBins) fT0resolution[ibin] = t0binRes;};
  Float_t  GetT0bin(Int_t ibin) const {if(ibin >=0 && ibin <= fNmomBins) return fT0event[ibin]; else return 0.0;};
  Float_t  GetT0binRes(Int_t ibin) const {if(ibin >=0 && ibin <= fNmomBins) return fT0resolution[ibin]; else return 0.0;};

  // Get Start Time for a track
  Float_t  GetStartTime(Float_t mom);
  Float_t  GetStartTimeRes(Float_t mom);

 private:
  Double_t fSigma;        // intrinsic TOF resolution

  // obsolete
  Double_t fPmax;         // "maximal" probability of mismathing (at ~0.5 GeV/c)
  Double_t fTime0;        // time zero
  //--------------

  // About event time (t0) info
  static const Int_t fNmomBins = 10; // number of momentum bin 
  Float_t fT0event[fNmomBins];    // t0 (best, T0, T0-TOF, ...) of the event as a function of p 
  Float_t fT0resolution[fNmomBins]; // t0 (best, T0, T0-TOF, ...) resolution as a function of p 
  Float_t fPCutMin[fNmomBins+1]; // min values for p bins
  
  ClassDef(AliTOFPIDResponse,2)   // TOF PID class
};

#endif

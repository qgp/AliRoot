#ifndef ALIKALMANTRACK_H
#define ALIKALMANTRACK_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliKalmanTrack
//
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>

class AliCluster;

class AliKalmanTrack : public TObject {
public:
  AliKalmanTrack();
  AliKalmanTrack(const AliKalmanTrack &t);

  virtual ~AliKalmanTrack(){};
  void SetLabel(Int_t lab) {fLab=lab;}

  Bool_t   IsSortable() const {return kTRUE;}
  Int_t    GetLabel()   const {return fLab;}
  Double_t GetChi2()    const {return fChi2;}
  Double_t GetMass()    const {return fMass;}
  Int_t    GetNumberOfClusters() const {return fN;}
  virtual Int_t GetClusterIndex(Int_t) const { //reserved for AliTracker
    Warning("GetClusterIndex(Int_t)","Method must be overloaded !\n");
    return 0;
  } 
  virtual Double_t GetPIDsignal() const {
    Warning("GetPIDsignal()","Method must be overloaded !\n");
    return 0.;
  }

  virtual Double_t GetDCA(const AliKalmanTrack *,Double_t &,Double_t &) const; 
  virtual 
  Double_t PropagateToDCA(AliKalmanTrack *, Double_t d=0., Double_t x0=0.); 
  virtual Double_t GetAlpha() const {
    Warning("GetAlpha()","Method must be overloaded !\n");
    return 0.;
  }
  virtual Double_t GetSigmaY2() const {
    Warning("GetSigmaY2()","Method must be overloaded !\n");
    return 0.;
  }
  virtual Double_t GetSigmaZ2() const {
    Warning("GetSigmaZ2()","Method must be overloaded !\n");
    return 0.;
  }

  virtual Int_t Compare(const TObject *) const {return 0;} 

  virtual void GetExternalParameters(Double_t &/*xr*/, Double_t /*x*/[5]) const {}
  virtual void GetExternalCovariance(Double_t /*cov*/[15]) const {}

  virtual Double_t GetX() const;
  virtual Double_t GetdEdx() const;

  virtual Double_t GetY() const;
  virtual Double_t GetZ() const;
  virtual Double_t GetSnp() const;
  virtual Double_t GetTgl() const;
  virtual Double_t Get1Pt() const;

  virtual Double_t Phi() const;
  virtual Double_t SigmaPhi() const;
  virtual Double_t Theta() const;
  virtual Double_t SigmaTheta() const;
  virtual Double_t Px() const;
  virtual Double_t Py() const;
  virtual Double_t Pz() const;
  virtual Double_t Pt() const;
  virtual Double_t SigmaPt() const;
  virtual Double_t P() const;

  virtual Double_t GetPredictedChi2(const AliCluster *) const {return 0.;}
  virtual 
    Int_t PropagateTo(Double_t /*xr*/, Double_t /*x0*/, Double_t /*rho*/) {return 0;}
  virtual Int_t Update(const AliCluster*, Double_t /*chi2*/, UInt_t) {return 0;}

  static void SetConvConst(Double_t cc) {fgConvConst=cc;}
  Double_t GetConvConst() const {return fgConvConst;}

  static void SetMagneticField(Double_t f) {// f - Magnetic field in T
    fgConvConst=100/0.299792458/f;
  }
  Double_t GetMagneticField() const {return 100/0.299792458/fgConvConst;}

  // Time integration (S.Radomski@gsi.de)
  void   StartTimeIntegral();
  void SetIntegratedLength(Double_t l) {fIntegratedLength=l;}
  void SetIntegratedTimes(const Double_t *times);

  Bool_t IsStartedTimeIntegral() const {return fStartTimeIntegral;}
  void     AddTimeStep(Double_t length);
  void GetIntegratedTimes(Double_t *times) const;
  Double_t GetIntegratedTime(Int_t pdg) const;
  Double_t GetIntegratedLength() const {return fIntegratedLength;}
  void PrintTime() const;
  

protected:
  void SetChi2(Double_t chi2) {fChi2=chi2;} 
  void SetMass(Double_t mass) {fMass=mass;}
  void SetNumberOfClusters(Int_t n) {fN=n;} 

 private:
  Int_t fLab;             // track label
  Double_t fChi2;         // total chi2 value for this track
  Double_t fMass;         // mass hypothesis
  Int_t fN;               // number of associated clusters

  static Double_t fgConvConst; //conversion constant cm -> GeV/c

  // variables for time integration (S.Radomski@gsi.de)
  static const Int_t fgkTypes = 5;  // Number of track types (e,mu,pi,k,p)
  Bool_t  fStartTimeIntegral;       // indicator wether integrate time
  Double_t fIntegratedTime[5];       // integrated time
  Double_t fIntegratedLength;        // integrated length
  
  ClassDef(AliKalmanTrack,2)    // Reconstructed track
};

#endif



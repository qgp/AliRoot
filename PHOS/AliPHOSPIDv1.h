#ifndef ALIPHOSPIDV1_H
#define ALIPHOSPIDV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 * Revision 1.56  2005/05/28 14:19:04  schutz
 * Compilation warnings fixed by T.P.
 *
 */

//_________________________________________________________________________
// Implementation version v1 of the PHOS particle identifier 
// Identification is based on information from CPV and EMC
// Oh yeah                 
//*-- Author: Yves Schutz (SUBATECH), Gustavo Conesa.

// --- ROOT system ---
#include <TMatrixFfwd.h>

class TVector3 ;
class TPrincipal ;
class TROOT ;
class TTree ;
class TCanvas ;
class TFolder ;
class TFormula;
// --- Standard library ---
// --- AliRoot header files ---
class AliPHOSEmcRecPoint ;
class AliPHOSCpvRecPoint ;
class AliPHOSClusterizerv1 ;
class AliPHOSTrackSegmentMakerv1 ;

#include "AliPHOSPID.h"
#include "AliPID.h"
class  AliPHOSPIDv1 : public AliPHOSPID {
  
public:
  
  AliPHOSPIDv1() ;          // ctor   
  AliPHOSPIDv1(const TString alirunFileNameFile, const TString eventFolderName = AliConfig::GetDefaultEventFolderName()) ;
  AliPHOSPIDv1(const AliPHOSPIDv1 & pid) ;          // cpy ctor            
  
  virtual ~AliPHOSPIDv1() ; // dtor
  
  virtual void Exec(Option_t *option);  // Does the job

  //Get file name that contain the PCA
  const TString GetFileNamePrincipal(TString particle) const;

  //Get file name that contain PID parameters
  const TString GetFileNameParameters()      const {return fFileNameParameters ;}

  // Get number of rec.particles in this run
  virtual Int_t GetRecParticlesInRun() const {return fRecParticlesInRun ;}  


  // Get PID parameters as they are defined in fParameters
  Float_t GetParameterCalibration    (Int_t i)               const;
  Float_t GetParameterCpv2Emc        (Int_t i, TString axis) const;
  Float_t GetParameterTimeGate       (Int_t i)               const;
  Float_t GetParameterToCalculateEllipse(TString particle, TString param, Int_t i) const  ;     
  Float_t GetParameterPhotonBoundary (Int_t i)               const;
  Float_t GetParameterPi0Boundary    (Int_t i)               const;

  // Get energy-dependent PID parameters
  Float_t GetCalibratedEnergy    (Float_t e)                 const;
  Float_t GetCpv2EmcDistanceCut  (TString axis, Float_t e)   const ;
  Float_t GetEllipseParameter    (TString particle, TString param, Float_t e) const;

  Double_t GetThresholdChargedNeutral () const {return  fChargedNeutralThreshold;}
  Float_t GetTOFEnergyThreshold () const {return  fTOFEnThreshold;}
  Float_t GetDispersionEnergyThreshold () const {return  fDispEnThreshold;}
  Int_t   GetDispersionMultiplicityThreshold () const {return  fDispMultThreshold;}

  //Do bayesian PID
  void SetBayesianPID(Bool_t set){ fBayesian = set ;}

  // Set PID parameters to change appropriate element of fParameters
  void SetParameterCalibration   (Int_t i, Float_t param);
  void SetParameterCpv2Emc       (Int_t i, TString axis, Float_t cut)  ; 
  void SetParameterTimeGate      (Int_t i, Float_t gate)  ; 
  void SetParameterToCalculateEllipse(TString particle, TString param, Int_t i, Float_t value) ;
  void SetParameterPhotonBoundary(Int_t i, Float_t param);
  void SetParameterPi0Boundary   (Int_t i, Float_t param);

  void SetThresholdChargedNeutral (Double_t th) {fChargedNeutralThreshold = th;}
  void SetTOFEnergyThreshold (Float_t th)  {fTOFEnThreshold = th;}
  void SetDispersionEnergyThreshold (Float_t th) {fDispEnThreshold = th;}
  void SetDispersionMultiplicityThreshold (Int_t th)  {fDispMultThreshold = th;}

  //Switch to "on flyght" mode, without writing to TreeR and file  
  void SetWriting(Bool_t toWrite = kFALSE){fWrite = toWrite;} 
  void Print(const Option_t * = "") const ; 

  virtual const char * Version() const { return "pid-v1" ; }  

  AliPHOSPIDv1 & operator = (const AliPHOSPIDv1 & /*pid*/) { return *this ;} 
  
private:
  
  const TString BranchName() const ; 
  virtual void  Init() ;
  virtual void  InitParameters() ;
  void          MakeRecParticles(void ) ;
  void          MakePID(void) ;

  //Functions to calculate the PID probability 
  //  Double_t ChargedHadronDistProb(Double_t  x, Double_t y, Double_t * parg, Double_t * parl) ;
  Double_t GausF   (Double_t x, Double_t y, Double_t *par) ; //gaussian probability, parameter dependence a+b/(x*x)+c/x
  Double_t GausPol2(Double_t x, Double_t y, Double_t *par) ; //gaussian probability, parameter dependence a+b*x+c*x*x
  Double_t LandauF(Double_t x, Double_t y, Double_t *par) ; //gaussian probability, parameter dependence  a+b/(x*x)+c/x
  Double_t LandauPol2(Double_t x, Double_t y, Double_t *par) ; //gaussian probability, parameter dependence a+b*x+c*x*x
 // Relative Distance CPV-EMC
  Float_t GetDistance     (AliPHOSEmcRecPoint * emc, AliPHOSCpvRecPoint * cpv, Option_t * axis)const ; 
  Int_t   GetCPVBit       (AliPHOSEmcRecPoint * emc, AliPHOSCpvRecPoint * cpv, Int_t EffPur, Float_t e) const;
  Int_t   GetPrincipalBit (TString particle, const Double_t* P, Int_t EffPur, Float_t e)const ; //Principal cut
  Int_t   GetHardPhotonBit(AliPHOSEmcRecPoint * emc) const;
  Int_t   GetHardPi0Bit   (AliPHOSEmcRecPoint * emc) const;
  TVector3      GetMomentumDirection(AliPHOSEmcRecPoint * emc, AliPHOSCpvRecPoint * cpv)const ;
  void          PrintRecParticles(Option_t * option) ;
  virtual void  WriteRecParticles() ; 
  void          SetParameters() ; //Fills the matrix of parameters
  void          Unload(); 

  //PID population
  void SetInitPID(const Double_t * pid) ;
  void GetInitPID(Double_t * pid) const ;

private:
  Bool_t      fBayesian ;                 //  Do PID bayesian
  Bool_t      fDefaultInit;              //! kTRUE if the task was created by defaut ctor (only parameters are initialized)
  Bool_t      fWrite ;                   //! To write result to file 
  Int_t       fNEvent ;                  //! current event number
  TString     fFileNamePrincipalPhoton ; //  File name of the photon principals
  TString     fFileNamePrincipalPi0 ;    //  File name of the pi0 principals
  TString     fFileNameParameters ;      //  File name with PID parameters
  TPrincipal *fPrincipalPhoton ;         //! TPrincipal from photon pca file 
  TPrincipal *fPrincipalPi0 ;            //! TPrincipal from pi0 pca file 
  Double_t   *fX ;                       //! Shower shape for the principal data 
  Double_t   *fPPhoton ;                 //! Principal photon eigenvalues
  Double_t   *fPPi0 ;                    //! Principal pi0 eigenvalues
  Int_t       fRecParticlesInRun ;       //! Total number of recparticles in one run
  TMatrixF    *fParameters;               //! Matrix of identification Parameters

  //Initial pid population
  Double_t fInitPID[AliPID::kSPECIESN] ; // Initial population to do bayesian PID
  // pid probability function parameters
  // ToF
  Double_t fTphoton[3] ;       // gaussian tof response for photon
  TFormula * fTFphoton ;       // the formula   
  Double_t fTpiong[3] ;        // gaussian tof response for pions
  TFormula * fTFpiong ;        // the formula
  Double_t fTkaong[3] ;        // landau tof response for kaons
  TFormula * fTFkaong ;        // the formula
  Double_t fTkaonl[3] ;        // landau tof response for kaons
  TFormula * fTFkaonl ;        // the formula
  Double_t fThhadrong[3] ;     // gaus   tof response for heavy hadrons
  TFormula * fTFhhadrong ;     // the formula
  Double_t fThhadronl[3] ;     // landau   tof response for heavy hadrons
  TFormula * fTFhhadronl ;     // the formula

  //Shower dispersion
  Double_t fDmuon[3]    ;     // gaussian ss response for muon 
  TFormula * fDFmuon    ;     // the formula 
  Double_t fDphoton[10] ;     // gaussian ss response for EM
  Double_t fDpi0[10]    ;     // gaussian ss response for pi0
  Double_t fDhadron[10] ;     // gaussian ss response for hadrons

  Double_t fXelectron[10] ;   // gaussian emc-cpv distance response for electron
  Double_t fXcharged[10]  ;   // landau emc-cpv distance response for charged part (no elect) */
  Double_t fZelectron[10] ;   // gaussian emc-cpv distance response for electron
  Double_t fZcharged[10]  ;   // landau emc-cpv distance response for charged part (no elect) */


  Double_t fERecWeightPar[4] ;  // gaussian tof response for photon
  TFormula * fERecWeight ;      // the formula   
  Double_t fChargedNeutralThreshold ; //Threshold to differentiate between charged and neutral
  Float_t  fTOFEnThreshold;           //Maximum energy to use TOF
  Float_t  fDispEnThreshold;          //Minimum energy to use shower shape
  Int_t    fDispMultThreshold ;       //Minimum multiplicity to use shower shape

  ClassDef( AliPHOSPIDv1,12)  // Particle identifier implementation version 1

};

#endif // AliPHOSPIDV1_H

#ifndef ALIPHOSCLUSTERIZERV1_H
#define ALIPHOSCLUSTERIZERV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  Implementation version 1 of the clusterization algorithm                     
//  Performs clusterization (collects neighbouring active cells) and 
//  unfolding of the clusters with several local maxima.  
//  results are stored in TreeR#, branches PHOSEmcRP (EMC recPoints),
//  PHOSCpvRP (CPV RecPoints) and AliPHOSClusterizer
//
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliPHOSClusterizer.h"
class AliPHOSEmcRecPoint ; 
class AliPHOSDigit ;
class AliPHOSDigitizer ;
class AliPHOSGeometry ;
class AliPHOSCalibrationDB ;

class AliPHOSClusterizerv1 : public AliPHOSClusterizer {
  
public:
  
  AliPHOSClusterizerv1() ;         
  AliPHOSClusterizerv1(const TString alirunFileNameFile, const TString eventFolderName = AliConfig::GetDefaultEventFolderName());
  AliPHOSClusterizerv1(const AliPHOSClusterizerv1 & clu) : AliPHOSClusterizer(clu) {
    // cpy ctor: no implementation yet
    // requested by the Coding Convention
    Fatal("cpy ctor", "not implemented") ;
  }
  virtual ~AliPHOSClusterizerv1()  ;
  
  virtual Int_t   AreNeighbours(AliPHOSDigit * d1, AliPHOSDigit * d2)const ; 
                               // Checks if digits are in neighbour cells 

  virtual Float_t Calibrate(Int_t amp, Int_t absId)const ;  // Tranforms Amp to energy 

  virtual void    GetNumberOfClustersFound(int * numb )const{  numb[0] = fNumberOfEmcClusters ; 
                                                               numb[1] = fNumberOfCpvClusters ; }

  virtual Float_t GetEmcClusteringThreshold()const{ return fEmcClusteringThreshold;}
  virtual Float_t GetEmcLocalMaxCut()const        { return fEmcLocMaxCut;} 
  virtual Float_t GetEmcLogWeight()const          { return fW0;}  
  virtual Float_t GetEmcTimeGate() const          { return fEmcTimeGate ; }
  virtual Float_t GetCpvClusteringThreshold()const{ return fCpvClusteringThreshold;  } 
  virtual Float_t GetCpvLocalMaxCut()const        { return fCpvLocMaxCut;} 
  virtual Float_t GetCpvLogWeight()const          { return fW0CPV;}  
  virtual const char *  GetRecPointsBranch() const{ return GetName() ;}
  virtual Int_t   GetRecPointsInRun() const       {return fRecPointsInRun ;} 

  virtual void    Exec(Option_t *option);   // Does the job

  void Print()const ;

  virtual void SetEmcClusteringThreshold(Float_t cluth)  { fEmcClusteringThreshold = cluth ; }
  virtual void SetEmcLocalMaxCut(Float_t cut)            { fEmcLocMaxCut = cut ; }
  virtual void SetEmcLogWeight(Float_t w)                { fW0 = w ; }
  virtual void SetEmcTimeGate(Float_t gate)              { fEmcTimeGate = gate ;}
  virtual void SetCpvClusteringThreshold(Float_t cluth)  { fCpvClusteringThreshold = cluth ; }
  virtual void SetCpvLocalMaxCut(Float_t cut)            { fCpvLocMaxCut = cut ; }
  virtual void SetCpvLogWeight(Float_t w)                { fW0CPV = w ; }
  virtual void SetUnfolding(Bool_t toUnfold = kTRUE )    { fToUnfold = toUnfold ;}  
  static Double_t ShowerShape(Double_t r) ; // Shape of EM shower used in unfolding; 
                                            //class member function (not object member function)
  static void UnfoldingChiSquare(Int_t & nPar, Double_t * Grad, Double_t & fret, Double_t * x, Int_t iflag)  ;
                                            // Chi^2 of the fit. Should be static to be passes to MINUIT
  void Unload() ; 
  virtual const char * Version() const { return "clu-v1"; }  

protected:

  void           WriteRecPoints() ;
  virtual void   MakeClusters( ) ;            
  virtual Bool_t IsInEmc (AliPHOSDigit * digit)const ;     // Tells if id digit is in EMC
  virtual Bool_t IsInCpv (AliPHOSDigit * digit)const ;     // Tells if id digit is in CPV

  
private:

  const   TString BranchName() const ; 
  void    GetCalibrationParameters(void) ;
  
  Bool_t  FindFit(AliPHOSEmcRecPoint * emcRP, AliPHOSDigit ** MaxAt, Float_t * maxAtEnergy, 
		  Int_t NPar, Float_t * FitParametres) const; //Used in UnfoldClusters, calls TMinuit
  void    Init() ;
  void    InitParameters() ;

  virtual void   MakeUnfolding() ;
  void           UnfoldCluster(AliPHOSEmcRecPoint * iniEmc,Int_t Nmax, 
		       AliPHOSDigit ** maxAt,Float_t * maxAtEnergy ) ; //Unfolds cluster using TMinuit package
  void           PrintRecPoints(Option_t * option) ;

private:

  Bool_t  fDefaultInit;              //! Says if the task was created by defaut ctor (only parameters are initialized)

  Int_t   fEmcCrystals ;             // number of EMC cristalls in PHOS

  Bool_t  fToUnfold ;                // To perform unfolding 

  Int_t   fNumberOfEmcClusters ;     // number of EMC clusters found 
  Int_t   fNumberOfCpvClusters ;     // number of CPV clusters found
 
  //Calibration parameters
  AliPHOSCalibrationDB * fCalibrationDB ; //! Calibration database if aval
  Float_t fADCchanelEmc ;           // width of one ADC channel in GeV
  Float_t fADCpedestalEmc ;         //
  Float_t fADCchanelCpv ;           // width of one ADC channel in CPV 'popugais'
  Float_t fADCpedestalCpv ;         // 

  Float_t fEmcClusteringThreshold ;  // minimum energy to include a EMC digit in a cluster
  Float_t fCpvClusteringThreshold ;  // minimum energy to include a CPV digit in a cluster
  Float_t fEmcLocMaxCut ;            // minimum energy difference to distinguish local maxima in a cluster
  Float_t fW0 ;                      // logarithmic weight for the cluster center of gravity calculation
  Float_t fCpvLocMaxCut ;            // minimum energy difference to distinguish local maxima in a CPV cluster
  Float_t fW0CPV ;                   // logarithmic weight for the CPV cluster center of gravity calculation
  Int_t fRecPointsInRun ;            //! Total number of recpoints in one run
  Float_t fEmcTimeGate ;             // Maximum time difference between the digits in ont EMC cluster
    
  ClassDef(AliPHOSClusterizerv1,3)   // Clusterizer implementation version 1

};

#endif // AliPHOSCLUSTERIZERV1_H

#ifndef ALIPHOSCLUSTERIZER_H
#define ALIPHOSCLUSTERIZER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
                            
/* $Id$ */

//_________________________________________________________________________
//  Base class for the clusterization algorithm 
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (SUBATECH & Kurchatov Institute)
// --- ROOT system ---

#include "TTask.h" 
#include "AliConfig.h"

class TFile ; 

// --- Standard library ---

// --- AliRoot header files ---

class AliPHOSClusterizer : public TTask {

public:

  AliPHOSClusterizer() ;        // default ctor
  AliPHOSClusterizer(const TString alirunFileName, const TString eventFolderName = AliConfig::fgkDefaultEventFolderName) ;
  AliPHOSClusterizer(const AliPHOSClusterizer & clusterizer) { ; }
  virtual ~AliPHOSClusterizer() ; // dtor
  virtual Float_t GetEmcClusteringThreshold()const {Warning("GetEmcClusteringThreshold", "Not Defined" ) ; return 0. ; }  
  virtual Float_t GetEmcLocalMaxCut()const {Warning("GetEmcLocalMaxCut", "Not Defined" ) ; return 0. ; } 
  virtual Float_t GetEmcLogWeight()const {Warning("GetEmcLogWeight", "Not Defined" ) ; return 0. ; } 
  virtual Float_t GetEmcTimeGate() const {Warning("GetEmcTimeGate", "Not Defined" ) ; return 0. ; }  ;
  virtual Float_t GetCpvClusteringThreshold()const {Warning("GetCpvClusteringThreshold", "Not Defined" ) ; return 0. ; } 
  virtual Float_t GetCpvLocalMaxCut()const {Warning("GetCpvLocalMaxCut", "Not Defined" ) ; return 0. ; } 
  virtual Float_t GetCpvLogWeight()const {Warning("GetCpvLogWeight", "Not Defined" ) ; return 0. ; } 
  virtual const char *  GetRecPointsBranch() const {Warning("GetRecPointsBranch", "Not Defined" ) ; return 0 ; }  ;
  virtual const Int_t GetRecPointsInRun()  const {Warning("GetRecPointsInRun", "Not Defined" ) ; return 0 ; } 
  virtual const char *  GetDigitsBranch() const{Warning("GetDigitsBranch", "Not Defined" ) ; return 0 ; }   ;

  virtual void MakeClusters() {Warning("MakeClusters", "Not Defined" ) ; } 
  virtual void Print()const {Warning("Print", "Not Defined" ) ; } 

  virtual void SetEmcClusteringThreshold(Float_t cluth) {Warning("SetEmcClusteringThreshold", "Not Defined" ) ; } 
  virtual void SetEmcLocalMaxCut(Float_t cut) {Warning("SetEmcLocalMaxCut", "Not Defined" ) ; } 
    
  virtual void SetEmcLogWeight(Float_t w) {Warning("SetEmcLogWeight", "Not Defined" ) ; } 
  virtual void SetEmcTimeGate(Float_t gate) {Warning("SetEmcTimeGate", "Not Defined" ) ; } 
  virtual void SetCpvClusteringThreshold(Float_t cluth) {Warning("SetCpvClusteringThreshold", "Not Defined" ) ; } 
  virtual void SetCpvLocalMaxCut(Float_t cut) {Warning("SetCpvLocalMaxCut", "Not Defined" ) ; } 
  virtual void SetCpvLogWeight(Float_t w) {Warning("SetCpvLogWeight", "Not Defined" ) ;  } 
  virtual void SetDigitsBranch(const char * title) {Warning("SetDigitsBranch", "Not Defined" ) ; }  
  virtual void SetRecPointsBranch(const char *title) {Warning("SetRecPointsBranch", "Not Defined" ) ; } 
  virtual void SetUnfolding(Bool_t toUnfold ){Warning("SetUnfolding", "Not Defined" ) ;}
  void   SetEventFolderName(TString name) { fEventFolderName = name ; }

  AliPHOSClusterizer & operator = (const AliPHOSClusterizer & rvalue)  {return *this ;} 
 
  virtual const char * Version() const {Warning("Version", "Not Defined" ) ; return 0 ; }  

protected:
  TString fEventFolderName ;  // event folder name


  ClassDef(AliPHOSClusterizer,3)  // Clusterization algorithm class 

} ;

#endif // AliPHOSCLUSTERIZER_H

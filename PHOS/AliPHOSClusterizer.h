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

// --- Standard library ---
#include <iostream.h> 

// --- AliRoot header files ---

//#include "AliPHOSDigit.h"

class AliPHOSClusterizer : public TTask {

public:

  AliPHOSClusterizer() ;        // default ctor
  AliPHOSClusterizer(const char * headerFile, const char * name) ;
  virtual ~AliPHOSClusterizer() ; // dtor

  const TString GetHitsFileName() const { return fHitsFileName ; }
  const TString GetSDigitsFileName() const { return fSDigitsFileName ; }
  const TString GetDigitsFileName() const { return fDigitsFileName ; }

  virtual Float_t GetEmcClusteringThreshold()const {cout << "Not Defined" << endl ; return 0. ; }  
  virtual Float_t GetEmcLocalMaxCut()const {cout << "Not Defined" << endl ; return 0. ; } 
  virtual Float_t GetEmcLogWeight()const {cout << "Not Defined" << endl ; return 0. ; } 
  virtual Float_t GetEmcTimeGate() const {cout << "Not Defined" << endl ; return 0. ; }  ;
  virtual Float_t GetCpvClusteringThreshold()const {cout << "Not Defined" << endl ; return 0. ; } 
  virtual Float_t GetCpvLocalMaxCut()const {cout << "Not Defined" << endl ; return 0. ; } 
  virtual Float_t GetCpvLogWeight()const {cout << "Not Defined" << endl ; return 0. ; } 
  virtual char *  GetRecPointsBranch() const {cout << "Not Defined" << endl ; return 0 ; }  ;
  virtual const Int_t GetRecPointsInRun()  const {cout << "Not Defined" << endl ; return 0 ; } 
  virtual char *  GetDigitsBranch() const{cout << "Not Defined" << endl ; return 0 ; }   ;

  virtual void MakeClusters() {cout << "Not Defined" << endl ; } 
  virtual void Print(Option_t * option)const {cout << "Not Defined" << endl ; } 

  virtual void SetEmcClusteringThreshold(Float_t cluth) {cout << "Not Defined" << endl ; } 
  virtual void SetEmcLocalMaxCut(Float_t cut) {cout << "Not Defined" << endl ; } 
  virtual void SetEmcLogWeight(Float_t w) {cout << "Not Defined" << endl ; } 
  virtual void SetEmcTimeGate(Float_t gate) {cout << "Not Defined" << endl ; } 
  virtual void SetCpvClusteringThreshold(Float_t cluth) {cout << "Not Defined" << endl ; } 
  virtual void SetCpvLocalMaxCut(Float_t cut) {cout << "Not Defined" << endl ; } 
  virtual void SetCpvLogWeight(Float_t w) {cout << "Not Defined" << endl ;  } 
  virtual void SetDigitsBranch(const char * title) {cout << "Not Defined" << endl ; }  
  virtual void SetRecPointsBranch(const char *title) {cout << "Not Defined" << endl ; } 
  virtual void SetSplitFile(const TString splitFileName = "PHOS.RecData.root") ; 
  virtual void SetUnfolding(Bool_t toUnfold ){cout << "Not Defined" << endl ;}  
  virtual const char * Version() const {cout << "Not Defined" << endl ; return 0 ; }  

protected:

  TString fHitsFileName ;          // file name that contains the original hits
  TString fSDigitsFileName ;       // file name that contains the original SDigits
  TString fDigitsFileName ;        // file name that contains the original Digits
  TFile * fSplitFile ;             //! file in which RecPoints will eventually be stored


  ClassDef(AliPHOSClusterizer,2)  // Clusterization algorithm class 

} ;

#endif // AliPHOSCLUSTERIZER_H

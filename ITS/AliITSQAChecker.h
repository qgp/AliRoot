#ifndef ALIITSQACHECKER_H
#define ALIITSQACHECKER_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
//  Checks the quality assurance. 
//  By comparing with reference data
//  INFN Torino
//  W.Ferrarese  P.Cerello  Mag 2008
//


// --- ROOT system ---
class TFile ; 
class TH2F ;  

// --- AliRoot header files ---
#include "AliQAv1.h"
#include "AliQACheckerBase.h"

class AliITSQASPDChecker;
class AliITSQASDDChecker;
class AliITSQASSDChecker;
class AliITSLoader ; 

class AliITSQAChecker: public AliQACheckerBase {

public:
  AliITSQAChecker(Bool_t kMode = kFALSE, Short_t subDet = 0, Short_t ldc = 0) ;         // ctor
  virtual ~AliITSQAChecker();// dtor
  void SetMode(Bool_t kMode) { fkOnline = kMode; }
  void SetSubDet(Short_t subdet) { fDet = subdet; }
  void SetLDC(Short_t ldc) { fLDC = ldc; }
  Bool_t GetMode() const { return fkOnline; };
  Short_t GetSubDet() const { return fDet; } ;
  Short_t GetLDC() const { return fLDC; }  ;
  virtual void SetTaskOffset(Int_t SPDOffset, Int_t SDDOffset, Int_t SSDOffset);
  virtual void SetHisto(Int_t SPDhisto, Int_t SDDhisto, Int_t SSDhisto);
  virtual void SetDetTaskOffset(Int_t subdet=0,Int_t offset=0);
  virtual void InitQACheckerLimits();
  virtual void CreateStepForBit(Double_t histonumb,Double_t *steprange);
  virtual void SetQA(AliQAv1::ALITASK_t index, Double_t * value) const;
  virtual void SetDetHisto(Int_t subdet=0,Int_t histo=0);

  virtual Int_t GetSPDHisto(){return fSPDHisto;} ;
  virtual Int_t GetSDDHisto(){return fSDDHisto;} ;
  virtual Int_t GetSSDHisto(){return fSSDHisto;} ;

  virtual  void   MakeImage( TObjArray ** list, AliQAv1::TASKINDEX_t task, AliQAv1::MODE_t mode) ; 
  virtual  void   MakeITSImage( TObjArray ** list, AliQAv1::TASKINDEX_t task, AliQAv1::MODE_t mode) { AliQACheckerBase::MakeImage(list,task, mode);} 

protected:
  virtual void Check(Double_t * test, AliQAv1::ALITASK_t index, TObjArray ** list, const AliDetectorRecoParam * recoParam) ;
  virtual void SetSPDTaskOffset(Int_t SPDOffset){fSPDOffset = SPDOffset;} ;
  virtual void SetSDDTaskOffset(Int_t SDDOffset){fSDDOffset = SDDOffset;} ;
  virtual void SetSSDTaskOffset(Int_t SSDOffset){fSSDOffset = SSDOffset;} ;

  virtual void SetSPDHisto(Int_t SPDhisto){fSPDHisto = SPDhisto;} ;
  virtual void SetSDDHisto(Int_t SDDhisto){fSDDHisto = SDDhisto;} ;
  virtual void SetSSDHisto(Int_t SSDhisto){fSSDHisto = SSDhisto;} ;

private:
  AliITSQAChecker(const AliITSQAChecker& qac);
  AliITSQAChecker& operator=(const AliITSQAChecker& qac);  

  Bool_t  fkOnline; //online=kTRUE offline=kFALSE
  Short_t fDet;   //0=all 1=SPD 2=SDD 3=SSD
  Short_t fLDC; //LDC number

  Int_t fSPDOffset; //starting point for the QACheck list SPD
  Int_t fSDDOffset; //starting point for the QACheck list SDD
  Int_t fSSDOffset; //starting point for the QACheck list SSD

  Int_t fSPDHisto; //number of histograms for SPD
  Int_t fSDDHisto; //number of histograms for SDD
  Int_t fSSDHisto; //number of histograms for SSD

  AliITSQASPDChecker *fSPDChecker;  // SPD Checker
  AliITSQASDDChecker *fSDDChecker;  // SDD Checker
  AliITSQASSDChecker *fSSDChecker;  // SSD Checker

  ClassDef(AliITSQAChecker,3)  // description 

};

#endif // AliITSQAChecker_H

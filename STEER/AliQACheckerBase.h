#ifndef ALIQACHECKERBASE_H
#define ALIQACHECKERBASE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
//  Base class for detectors quality assurance checkers 
//  Compares Data made by QADataMakers with reference data
//  Y. Schutz CERN August 2007
//


// --- ROOT system ---
#include <TNamed.h>
#include "AliQA.h"
class TFile ; 
class TH1 ; 
class TObjArray ; 
class TDirectory ; 
class TNtupleD ;

// --- Standard library ---

// --- AliRoot header files ---

class AliQACheckerBase: public TNamed {

public:
  AliQACheckerBase(const char * name = "", const char * title = "") ;          // ctor
  AliQACheckerBase(const AliQACheckerBase& qac) ;   
  AliQACheckerBase& operator = (const AliQACheckerBase& qac) ;
  virtual ~AliQACheckerBase() {;} // dtor

  void   Init(const AliQA::DETECTORINDEX_t det) ; 
  void   Run(AliQA::ALITASK_t tsk, TObject * obj = NULL); 
  void   SetRefandData(TDirectory * ref, TObjArray * refOCDB, TDirectory * data=NULL) { fRefSubDir = ref ;  fRefOCDBSubDir = refOCDB, fDataSubDir = data ; }

protected:
  virtual const Double_t Check(AliQA::ALITASK_t index) ;
  virtual const Double_t Check(AliQA::ALITASK_t, TObjArray *) ; 
  virtual const Double_t Check(AliQA::ALITASK_t, TNtupleD *) {;} 
  const Double_t DiffC(const TH1 * href, const TH1 * hin) const ;   
  const Double_t DiffK(const TH1 * href, const TH1 * hin) const ;   
  void           Finish() const ; 
  virtual void   SetQA(AliQA::ALITASK_t index, Double_t value) const ;	

  TDirectory  * fDataSubDir    ; //! directory for the current task directory in the current detector directory in the data file
  TDirectory  * fRefSubDir     ; //! directory for the current task directory in the current detector directory in the reference file
  TObjArray   * fRefOCDBSubDir ; //! Entry in OCDB for the current detector 

  ClassDef(AliQACheckerBase,1)  // description 

};

#endif // AliQUALASSCHECKERBASE_H

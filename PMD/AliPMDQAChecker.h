#ifndef ALIPMDQACHECKER_H
#define ALIPMDQACHECKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
  Checks the quality assurance. 
  By comparing with reference data
  B.K. Nandi
*/


// --- ROOT system ---
class TFile ; 
class TH1F ; 
class TH1I ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliQACheckerBase.h"
class AliPMDLoader ; 

class AliPMDQAChecker: public AliQACheckerBase {

public:
  AliPMDQAChecker() : AliQACheckerBase("PMD","PMD Quality Assurance Data Maker") {;}          // ctor
  AliPMDQAChecker(const AliPMDQAChecker& qac) : AliQACheckerBase(qac.GetName(), qac.GetTitle()) {;} // cpy ctor   
  AliPMDQAChecker& operator = (const AliPMDQAChecker& qac) ;
  virtual ~AliPMDQAChecker() {;} // dtor

private:
  
  ClassDef(AliPMDQAChecker,1)  // description 

};

#endif // AliPMDQAChecker_H

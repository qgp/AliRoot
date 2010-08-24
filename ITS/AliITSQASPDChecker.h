#ifndef ALIITSQASPDCHECKER_H
#define ALIITSQASPDCHECKER_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
//  Checks the quality assurance. 
//  By comparing with reference data
//  INFN Torino
//  P. Cerello - apr 2008
//

#include "AliQAv1.h"
#include "TPaveText.h"

class AliITSQASPDChecker: public TObject {

public:
  AliITSQASPDChecker();
  AliITSQASPDChecker& operator = (const AliITSQASPDChecker& qac) ; //operator =
  virtual ~AliITSQASPDChecker(); // dtor
  virtual Double_t Check(AliQAv1::ALITASK_t index, TObjArray * list, const AliDetectorRecoParam * recoParam);
  Double_t CheckRawData(const TObjArray *list);
  void SetTaskOffset(Int_t TaskOffset);
  
  void SetStepBit(const Double_t *steprange);
  Double_t *GetStepBit() const {return fStepBitSPD;};
  void SetSPDLimits(const Float_t *lowvalue, const Float_t * highvalue);
private:
  
  AliITSQASPDChecker(const AliITSQASPDChecker& qac):TObject(),fSubDetOffset(qac.fSubDetOffset),fStepBitSPD(qac.fStepBitSPD),fLowSPDValue(qac.fLowSPDValue),fHighSPDValue(qac.fHighSPDValue){;}  // cpy ctor   
  Int_t fSubDetOffset;           // checking operation starting point
  Double_t *fStepBitSPD;         // parameter interface for ITS final QA
  Float_t *fLowSPDValue;         // lower limits for QA bit settings
  Float_t *fHighSPDValue;        // lower limits for QA bit settings
  
  TPaveText * fDisplayStatus[6]; //Online Comments for the DQM-shifter histos 

  ClassDef(AliITSQASPDChecker,3)  // description 

};

#endif // AliITSQASPDChecker_H


#ifndef ALIHMPIDQATASK_H
#define ALIHMPIDQATASK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
//______________________________________________________________________________
// An analysis task to check the HMPID data in simulated data
//
//*-- Annalisa Mastroserio
//////////////////////////////////////////////////////////////////////////////

#include <TTree.h> 
#include "AliAnalysisTask.h"  

class AliESD ; 
class TH1F ; 
class TH2F ; 

class AliHMPIDQATask : public AliAnalysisTask {

public:
  AliHMPIDQATask(const char *name) ;
  virtual ~AliHMPIDQATask() ;
   
  virtual void Exec(Option_t * opt = "") ;
  virtual void ConnectInputData(Option_t *);
  virtual void CreateOutputObjects();
  virtual void Terminate(Option_t * opt = "") ;

private:
  TTree   * fChain ;            //!pointer to the analyzed TTree or TChain
  AliESD  * fESD ;              //! Declaration of leave types

  TObjArray * fOutputContainer ; //! output data container

  // Histograms
 
  TH2F * fhHMPIDCkovP ;  //!
  TH2F * fhHMPIDMipXY ;  //!
  TH2F * fhHMPIDDifXY ;  //!
  TH2F * fhHMPIDSigP ;   //!
  TH1F * fhHMPIDProb[5] ;//!

  AliHMPIDQATask(const AliHMPIDQATask&); // Not implemented
  AliHMPIDQATask& operator=(const AliHMPIDQATask&); // Not implemented
  
  ClassDef(AliHMPIDQATask, 0); // a HMPID photon analysis task 
};
#endif // ALIHMPIDQATASK_H

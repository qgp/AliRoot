#ifndef ALIANALYSISGOODIES_H
#define ALIANALYSISGOODIES_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
//______________________________________________________________________________
// Various utilities usefull for analysis
//
//*-- Yves Schutz 
//////////////////////////////////////////////////////////////////////////////

#include <TObject.h> 
#include <TStopwatch.h> 
#include <TString.h> 

#include "AliLog.h"

class AliAnalysisTask ; 
class TChain ; 
class TClass ; 
class AliEventTagCuts ;  
class AliRunTagCuts ;  
class AliLHCTagCuts ;  
class AliDetectorTagCuts ;  

class AliAnalysisGoodies : public TObject {

public:
  AliAnalysisGoodies() ; 
  AliAnalysisGoodies(const AliAnalysisGoodies& ag) ; 
  ~AliAnalysisGoodies() {;} 

  virtual void Help() const; 
  Bool_t Alien2Local(const TString collectionNameIn, const TString localDir) ; 
  Bool_t Make(AliRunTagCuts *runCuts, AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts *evtCuts, const char * in, const char * out) const ;
  Bool_t Merge(const char * collection, const char * subFile = 0, const char * outFile = 0) ; 
  Bool_t Register( const char * lfndir, const char * pfndir, const char * file)  ;   
  Bool_t Process(TChain * chain) ;  
  Bool_t Process(const char * esdFile)  ;  
  Bool_t Process(const char * inFile, AliRunTagCuts *runCuts, AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts * evtCuts ) ;
  Bool_t Process(const char * inFile, const char * runCuts, const char * lhcCuts, const char * detCuts, const char * evtCuts) ;  
  void   SetAODIndex(const Int_t taskIndex, const Int_t outputIndex) { fAODIndex = new Int_t[2] ; fAODIndex[0] = taskIndex ; fAODIndex[1] = outputIndex ; }
  void   SetESDTreeName(const char * name) { fESDTreeName = name ; }
  void   SetTasks(TList * taskList, TList ** inputType, TList ** outputType) ;
  Bool_t MakeEsdCollectionFromTagFile(AliRunTagCuts *runCuts, AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts *evtCuts, const char * in, const char * out) const ; 

private:
  const Int_t * GetAODIndex() const { return fAODIndex ; }
  Bool_t MakeEsdCollectionFromTagFile(const char * , const char * , const char * , const char *) const ;
  Bool_t MakeEsdCollectionFromTagCollection(AliRunTagCuts *runCuts, AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts *evtCuts, const char * in, const char * out) const ;
  Bool_t MakeEsdCollectionFromTagCollection(const char * runCuts, const char *lhcCuts, const char *detCuts, const char * evtCuts, const char * in, const char * out) const ;
  Bool_t ProcessChain(TChain * chain) const ; 
  Bool_t ProcessEsdFile(const char * esdFile) const ;
  Bool_t ProcessEsdXmlCollection(const char * xmlFile) const ;
  Bool_t ProcessTagFile(const char * tagFile, AliRunTagCuts *runCuts,  AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts *evtCuts) const ;  
  Bool_t ProcessTagFile(const char * tagFile, const char * runCuts, const char * evtCuts) const ;
  Bool_t ProcessTagFile(const char * tagFile, const char * runCuts, const char * lhcCuts, const char * detCuts, const char * evtCuts) const ;   
  Bool_t ProcessTagXmlCollection(const char * xmlFile, AliRunTagCuts *runCuts, AliLHCTagCuts *lhcCuts, AliDetectorTagCuts *detCuts, AliEventTagCuts * evtCuts) const ;   
  Bool_t ProcessTagXmlCollection(const char * xmlFile, const char * runCuts, const char * lhcCuts, const char * detCuts, const char * evtCuts) const ; 

  TStopwatch        fTimer         ;  //! stopwatch
  TString           fESDTreeName   ;  //! name of the ESD TTree
  UShort_t          fnumberOfTasks ;  //! number of tasks
  TList           * fTaskList      ;  //! list of tasks
  TList          ** fTaskInType    ;  //! list of tasks input
  TList          ** fTaskOuType    ;  //! list of tasks output
  Int_t           * fAODIndex      ;  //! index of task and output that is related to AOD

  ClassDef(AliAnalysisGoodies, 0); // an analysis utilities class
};
#endif // ALIANALYSISGOODIES_H

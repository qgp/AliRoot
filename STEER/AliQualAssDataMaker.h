#ifndef ALIQUALASSDATAMAKER_H
#define ALIQUALASSDATAMAKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

/*
  Base Class:
  Produces the data needed to calculate the quality assurance. 
  All data must be mergeable objects.
  Y. Schutz CERN July 2007
*/


// --- ROOT system ---
#include <TH1.h>
#include <TList.h>
#include <TNamed.h>  
class TFile;  
class TDirectory;
class TObject; 
class TTree; 
class AliESDEvent;
class AliRawReader;
class TClonesArray;

// --- Standard library ---

// --- AliRoot header files ---
#include "AliQualAss.h"

class AliQualAssDataMaker: public TNamed {
  
public:
  
  AliQualAssDataMaker(const char * name="", const char * title="") ;          // ctor
  AliQualAssDataMaker(const AliQualAssDataMaker& qadm) ;   
  AliQualAssDataMaker& operator = (const AliQualAssDataMaker& qadm) ;
  virtual ~AliQualAssDataMaker() {;} // dtor
  
  virtual void        Exec(AliQualAss::TASKINDEX, TObject * data) ;
  void                EndOfCycle(AliQualAss::TASKINDEX) ;
  void                Finish(AliQualAss::TASKINDEX task) const ; 
  static const char * GetDetectorDirName() { return fDetectorDirName.Data() ; }
  const Int_t         Increment() { return ++fCycleCounter ; } 
  TList *             Init(AliQualAss::TASKINDEX, Int_t run, Int_t cycles = -1) ;
  const Bool_t        IsCycleDone() const { return fCycleCounter > fCycle ? kTRUE : kFALSE ; }
  const Int_t         Add2DigitsList(TH1 * hist, Int_t index)    { return Add2List(hist, index, fDigitsQAList) ; }
  const Int_t         Add2ESDsList(TH1 * hist, Int_t index)      { return Add2List(hist, index, fESDsQAList) ; }
  const Int_t         Add2HitsList(TH1 * hist, Int_t index)      { return Add2List(hist, index, fHitsQAList) ; }
  const Int_t         Add2RecPointsList(TH1 * hist, Int_t index) { return Add2List(hist, index, fRecPointsQAList) ; }
  const Int_t         Add2RawsList(TH1 * hist, Int_t index)      { return Add2List(hist, index, fRawsQAList) ; }
  const Int_t         Add2SDigitsList(TH1 * hist, Int_t index)   { return Add2List(hist, index, fSDigitsQAList) ; }
  TH1 *               GetDigitsData(Int_t index)    { return dynamic_cast<TH1 *>(GetData(fDigitsQAList, index)) ; }
  TH1 *               GetESDsData(Int_t index)      { return dynamic_cast<TH1 *>(GetData(fESDsQAList, index)) ; }
  TH1 *               GetHitsData(Int_t index)      { return dynamic_cast<TH1 *>(GetData(fHitsQAList, index)) ; }
  TH1 *               GetRecPointsData(Int_t index) { return dynamic_cast<TH1 *>(GetData(fRecPointsQAList, index)) ; }
  TH1 *               GetRawsData(Int_t index)      { return dynamic_cast<TH1 *>(GetData(fRawsQAList, index)) ; }
  TH1 *               GetSDigitsData(Int_t index)   { return dynamic_cast<TH1 *>(GetData(fSDigitsQAList, index)) ; }
  void                SetCycle(Int_t nevts) { fCycle = nevts ; } 
  void                StartOfCycle(AliQualAss::TASKINDEX, Option_t * sameCycle = "") ;

protected: 

  Int_t          Add2List(TH1 * hist, Int_t index, TList * list) { list->AddAt(hist, index) ; return list->LastIndex() ; }
  virtual void   EndOfDetectorCycle() {AliInfo("To be implemented by detectors");} 
  TObject *      GetData(TList * list, Int_t index)  { return list->At(index) ; } 
  virtual void   InitDigits()        {AliInfo("To be implemented by detectors");}
  virtual void   InitESDs()          {AliInfo("To be implemented by detectors");}
  virtual void   InitHits()          {AliInfo("To be implemented by detectors");}
  //virtual void   InitRecParticles()  {AliInfo("To be implemented by detectors");}
  virtual void   InitRecPoints()     {AliInfo("To be implemented by detectors");}
  virtual void   InitRaws()          {AliInfo("To be implemented by detectors");}
  virtual void   InitSDigits()       {AliInfo("To be implemented by detectors");}
  //virtual void   InitTrackSegments() {AliInfo("To ne implemented by detectors");}
  virtual void   MakeESDs(AliESDEvent * )          {AliInfo("To be implemented by detectors");} 
  virtual void   MakeHits(TClonesArray * )         {AliInfo("To be implemented by detectors");} 
  virtual void   MakeDigits(TClonesArray * )       {AliInfo("To be implemented by detectors");} 
  //  virtual void   MakeRecParticles(TClonesArray * ) {AliInfo("To be implemented by detectors");} 
  virtual void   MakeRaws(AliRawReader *)          {AliInfo("To be implemented by detectors");} 
  virtual void   MakeRecPoints(TTree * )           {AliInfo("To be implemented by detectors");} 
  virtual void   MakeSDigits(TClonesArray * )      {AliInfo("To be implemented by detectors");} 
  //virtual void   MakeTrackSegments(TTree * )       {AliInfo("To be implemented by detectors");} 
  void           ResetCycle() { fCycleCounter = 0 ; } 
  virtual void   StartOfDetectorCycle() {AliInfo("To be implemented by detectors");} 

  TFile *        fOutput ;          //! output root file
  TDirectory *   fDetectorDir ;     //! directory for the given detector in the file
  static TString fDetectorDirName ; //! detector directory name in the quality assurance data file
  TList *        fDigitsQAList ;    //! list of the digits QA data objects
  TList *        fESDsQAList ;      //! list of the ESDs QA data objects
  TList *        fHitsQAList ;      //! list of the hits QA data objects
  TList *        fRawsQAList ;      //! list of the raws QA data objects
  TList *        fRecPointsQAList ; //! list of the recpoints QA data objects
  TList *        fSDigitsQAList ;   //! list of the sdigits QA data objects
  Int_t          fCurrentCycle ;    //! current cycle number
  Int_t          fCycle ;           //! length (# events) of the QA data acquisition cycle  
  Int_t          fCycleCounter ;    //! cycle counter
  Int_t          fRun ;             //! run number
 ClassDef(AliQualAssDataMaker,1)  // description 

};

#endif // AliQualAssDataMaker_H

#ifndef ALIRUNDIGITIZER_H
#define ALIRUNDIGITIZER_H
/* Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////
//
//  Manager Class for Merging/Digitization   
//                  
//  Author: Jiri Chudoba (CERN)
//
////////////////////////////////////////////////////////////////////////

// --- ROOT system ---

#include "TArrayI.h"
#include "TTask.h"
class TClonesArray;
class TFile;
class TParticle;
class TTree;

// --- AliRoot header files ---

class AliStream;
class AliDigitizer;
class AliMergeCombi;
class AliRunLoader;

const Int_t kMaxStreamsToMerge = 4;

class AliRunDigitizer: public TTask {

public:
  AliRunDigitizer();
  AliRunDigitizer(Int_t nInputStreams, Int_t sperb=1);
  AliRunDigitizer(const AliRunDigitizer& dig);
  AliRunDigitizer& operator=(const AliRunDigitizer& dig)
    {dig.Copy(*this); return (*this);}
  virtual ~AliRunDigitizer();

  void      ExecuteTask(Option_t* option = 0);
  void      Exec(Option_t *option) {this->Digitize(option);}
  void      Digitize(Option_t* option = 0);
  void      AddDigitizer(AliDigitizer *digitizer);

  void      SetOutputFile(TString fn);
  TString   GetOutputFile() const {return fOutputFileName;}
  
  void      SetOutputDir(TString dn) {fOutputDirName = dn;}
  TString   GetOutputDir() const {return fOutputDirName;}
  
  void      SetInputStream(Int_t stream, const char *inputName);
  
  void      SetFirstOutputEventNr(Int_t i) {fEvent = i;}
  void      SetNrOfEventsToWrite(Int_t i) {fNrOfEventsToWrite = i;}
  void      SetCopyTreesFromInput(Int_t i) {fCopyTreesFromInput = i;}
  Int_t     GetCopyTreesFromInput() {return fCopyTreesFromInput;}
  Int_t     GetOutputEventNr() const {return fEvent;}
  void      SetCombinationFileName(TString fn) {fCombinationFileName = fn;} 
  TString   GetCombinationFileName() const {return fCombinationFileName;}
  Int_t     GetMask(Int_t i) const {return fkMASK[i];}


  Int_t     GetNinputs() const {return fNinputs;}
  const TString& GetInputFolderName(Int_t i) const;
  const char* GetOutputFolderName();


    
// Nr of particles in all input files for a given event
//     (as numbered in the output file)
  Int_t GetNParticles(Int_t event) const;

// Nr of particles in input file input for a given event
//     (as numbered in this input file)
  Int_t GetNParticles(Int_t event, Int_t input) const;

// return pointer to an int array with input event numbers which were
// merged in the output event event
  Int_t* GetInputEventNumbers(Int_t event) const;

// return an event number of an eventInput from input file input
// which was merged to create output event event
  Int_t GetInputEventNumber(Int_t event, Int_t input) const;
  
// return pointer to particle with index i (index with mask)
  TParticle* GetParticle(Int_t i, Int_t event) const;

// return pointer to particle with index i in the input file input
// (index without mask)
  TParticle* GetParticle(Int_t i, Int_t input, Int_t event) const;

// return TString with input file name  
  TString GetInputFileName(const Int_t input, const Int_t order) const;
  
  Int_t     GetDebug() const {return fDebug;}
  void      SetDebug(Int_t level) {fDebug = level;}

private:
  void Copy(AliRunDigitizer& dig) const;
  Bool_t            ConnectInputTrees();
  Bool_t            InitGlobal();
  Bool_t            InitOutputGlobal();
  void              InitEvent();
  void              FinishEvent();
  void              FinishGlobal();

  Int_t             fkMASK[kMaxStreamsToMerge];  //! masks for track ids from
                                              //  different source files
  Int_t             fkMASKSTEP;           // step to increase MASK for
                                          // each input file
  TString           fOutputFileName;      // output file name
  TString           fOutputDirName;       // output dir name

  Int_t             fEvent;               // output event nr.
  Int_t             fNrOfEventsToWrite;   // Nr of events to write
  Int_t             fNrOfEventsWritten;   // Nr of events written
  Int_t             fCopyTreesFromInput;  // from which input file the trees
                                          // should be copied, -1 for no copies
  Int_t             fNinputs;             // nr of input streams - can be taken from the TClonesArray dimension
  Int_t             fNinputsGiven;        // nr of input streams given by user
  TClonesArray *    fInputStreams;        // input signal streams

//  AliStream*        fOutputStream;
  AliRunLoader*     fOutRunLoader;        //!
  
  AliMergeCombi *   fCombi;               // pointer to the combination object
  TArrayI           fCombination;         //! combination of events from
  TString           fCombinationFileName; // fn with combinations (used
                                          // with type 2 of comb.)
  Int_t             fDebug;                //! specifies debug level, 0 is min

  AliRunLoader*     GetOutRunLoader();
  
  static const TString fgkDefOutFolderName;
  static const TString fgkBaseInFolderName;
  
  ClassDef(AliRunDigitizer,4)
};

#endif // ALIRUNDIGITIZER_H

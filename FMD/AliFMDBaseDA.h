// -*- mode: C++ -*- 
#ifndef ALIFMDBASEDA_H
#define ALIFMDBASEDA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * See cxx source for full Copyright notice                               
 */
//
// This class provides a base interface for the Detector Algorithms
// (DA) of the FMD.  At least three implementations are needed:
// AliFMDPedestalDA, AliFMDGainDA and AliFMDPhysicsDA .  These classes
// will provide the calibration data for the AliFMDPreprocessor to be
// used in the shuttle.  The input for this class are raw data
// (AliRawReader) and the output is a comma-separated file
// (std::ofstream) that contains the values defined in the
// implementations of this class.
//
// Author: Hans Hjersing Dalsgaard, hans.dalsgaard@cern.ch
//

#include "TNamed.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "iostream"
#include "fstream"
#include "TString.h"
#include "AliRawReader.h"
#include "AliFMDDigit.h"
#include "AliFMDParameters.h"
#include "TArrayS.h"
class TDirectory;
class AliFMDRawReader;

class AliFMDBaseDA: public TNamed {
  
public:
  AliFMDBaseDA() ;
  AliFMDBaseDA(const AliFMDBaseDA & baseDA) ;
  //  AliFMDBaseDA& operator = (const AliFMDBaseDA & baseDA) ; 
  
  ~AliFMDBaseDA() ;
  
  void Run(AliRawReader* fmdReader);
  void SetSaveDiagnostics(Bool_t save) {fSaveHistograms = save;}
  void SetRequiredEvents(Int_t nEvents) {fRequiredEvents = nEvents;}
  Int_t GetRequiredEvents() const {return fRequiredEvents ;}
protected:
  
  virtual void Init()  {};
  virtual void FillChannels(AliFMDDigit* )  {};
  virtual void Analyse(UShort_t, Char_t, UShort_t, UShort_t )  {};
  virtual void WriteHeaderToFile()  {};
  virtual void AddChannelContainer(TObjArray*, UShort_t, Char_t, UShort_t, UShort_t )  {};
  virtual void FinishEvent()  {};
  virtual void Terminate(TFile* ) {};
  
  Int_t GetCurrentEvent() const {return fCurrentEvent;}
  
  static const UInt_t fgkBaseDDL = 3072;   // base FMD ddl
  //Char_t* fDiagnosticsFilename;
  TString fDiagnosticsFilename;            // name of diagnostics file
  std::ofstream fOutputFile;               // output file
  std::ofstream fConditionsFile;           // conditions file
  Bool_t fSaveHistograms;                  // save hists or not
  TObjArray fDetectorArray;                // array indiced by detector

  Int_t GetHalfringIndex(UShort_t, Char_t, UShort_t) const;
  Int_t GetPulseSize(UShort_t det , 
		     Char_t ring, 
		     UShort_t board) {return fPulseSize.At(GetHalfringIndex(det,ring,board));}
  Int_t GetPulseLength(UShort_t det, 
		       Char_t ring, 
		       UShort_t board) {return fPulseLength.At(GetHalfringIndex(det,ring,board));}

  
  const char* GetDetectorPath(UShort_t det, Bool_t full=kTRUE) const;
  const char* GetRingPath(UShort_t det, Char_t ring, Bool_t full=kTRUE) const;
  const char* GetSectorPath(UShort_t det, Char_t ring, UShort_t sec, 
			    Bool_t full=kTRUE) const;
  const char* GetStripPath(UShort_t det, Char_t ring, UShort_t sec, 
			   UShort_t str, Bool_t full=kTRUE) const;
  

  
  TArrayS fPulseSize;                     // Pulse size for gain calib
  TArrayS fPulseLength;                   // Pulse length for gain calib

  Bool_t  fSeenDetectors[3];              // Detectors seen so far
private:
 
  void WriteConditionsData(AliFMDRawReader* fmdReader);
  void SetCurrentEvent(Int_t currentEvent) {fCurrentEvent = currentEvent; }
  void InitContainer(TDirectory* dir);
  Int_t fRequiredEvents;            // number of events required for this calib
  Int_t fCurrentEvent;              // the current event       
  
  
  
  ClassDef(AliFMDBaseDA,0)

};
#endif


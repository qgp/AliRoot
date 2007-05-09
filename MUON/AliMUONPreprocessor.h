#ifndef ALIMUONPREPROCESSOR_H
#define ALIMUONPREPROCESSOR_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup shuttle
/// \class AliMUONPreprocessor
/// \brief Shuttle preprocessor for MUON subsystems (TRK and TRG)
/// 
//  Author Laurent Aphecetche

#ifndef ALI_PREPROCESSOR_H
#  include "AliPreprocessor.h"
#endif

class AliMUONVSubprocessor;
class TObjArray;

class AliMUONPreprocessor : public AliPreprocessor
{
public:
  virtual void Initialize(Int_t run, UInt_t startTime, UInt_t endTime);
  virtual UInt_t Process(TMap* dcsAliasMap);
  virtual void Print(Option_t* opt="") const;
  
  virtual Bool_t ProcessDCS() { return fProcessDCS; }

  /// Publish AliPreprocessor::Log function
  void Log(const char* message) { AliPreprocessor::Log(message); }
  
  /// Publish AliPreprocessor::GetFileSources function
  TList* GetFileSources(Int_t system, const char* id) 
  { return AliPreprocessor::GetFileSources(system,id); }

  /// Publish AliPreprocessor::Store function
  Bool_t Store(const char* pathLevel2, const char* pathLevel3, TObject* object,
               AliCDBMetaData* metaData, 
               Int_t validityStart = 0, Bool_t validityInfinite = kFALSE)
  {
    return AliPreprocessor::Store(pathLevel2,pathLevel3,object,metaData,
                                  validityStart,validityInfinite);
  }
  
  /// Publish AliPreprocessor::GetFile function
  const char* GetFile(Int_t system, const char* id, const char* source)
  {
    return AliPreprocessor::GetFile(system,id,source);
  }  
  
protected:
  AliMUONPreprocessor(const char* detName, AliShuttleInterface* shuttle);
  virtual ~AliMUONPreprocessor();
  
  void Add(AliMUONVSubprocessor* subProcessor, Bool_t processDCS=kFALSE); 
  void ClearSubprocessors();
  
private:
  /// Not implemented
  AliMUONPreprocessor(const AliMUONPreprocessor& rhs);
  /// Not implemented
  AliMUONPreprocessor& operator=(const AliMUONPreprocessor& rhs);
  
  AliMUONVSubprocessor* Subprocessor(Int_t i) const;
  
private:

  TObjArray* fSubprocessors; //!< sub processors to execute
  Bool_t fProcessDCS; //!< whether the current subprocessor(s) needs DCS or not
  
  ClassDef(AliMUONPreprocessor,2) // MUON Shuttle preprocessor
};

#endif

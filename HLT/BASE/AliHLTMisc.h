//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTMISC_H
#define ALIHLTMISC_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               */

/// @file   AliHLTMisc.h
/// @author Matthias Richter
/// @date   
/// @brief  Definition of various glue functions implemented in dynamically
///         loaded libraries

#include "TObject.h"
#include "AliHLTStdIncludes.h"
#include "AliHLTDataTypes.h"
#include "AliHLTLogging.h"
#include "TClass.h"
#include "TSystem.h"

class AliCDBManager;
class AliCDBEntry;
class AliRawReader;
class AliHLTComponentDataType;
class AliHLTGlobalTriggerDecision;

class AliHLTMisc : public TObject {
 public:
  AliHLTMisc();
  ~AliHLTMisc();

  template<class T>
  static T* LoadInstance(const T* dummy, const char* classname, const char* library);

  static AliHLTMisc& Instance();

  virtual int InitCDB(const char* cdbpath);

  virtual int SetCDBRunNo(int runNo);
  virtual int GetCDBRunNo();

  virtual AliCDBEntry* LoadOCDBEntry(const char* path, int runNo=-1, int version = -1, int subVersion = -1);

  virtual TObject* ExtractObject(AliCDBEntry* entry);

  virtual int InitMagneticField() const;

  virtual AliHLTUInt64_t GetTriggerMask(AliRawReader* rawReader) const;

  virtual Double_t GetBz();
  virtual Double_t GetBz(const Double_t *r);
  virtual void GetBxByBz(const Double_t r[3], Double_t b[3]);

  virtual const TClass* IsAliESDHLTDecision() const;
  virtual int Copy(const AliHLTGlobalTriggerDecision* pDecision, TObject* pESDHLTDecision) const;

 private:
  static AliHLTMisc* fgInstance;

  ClassDef(AliHLTMisc, 0)
};

#define ALIHLTMISC_LIBRARY "libHLTrec.so"
#define ALIHLTMISC_INIT_CDB "AliHLTMiscInitCDB"
#define ALIHLTMISC_SET_CDB_RUNNO "AliHLTMiscSetCDBRunNo"

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * Init the CDB access for the running instance.
   * The method is used from the C wrapper interface utilized by the  on-line
   * framework. The path of the (H)CDB is set to the specified path.<br>
   * When running from AliRoot, the CDB path is set in the startup of the
   * reconstruction.<br>
   * If cdbpath is nil or empty and the CDB is not already initialized, the
   * CDB storage is set to local://$ALICE_ROOT/OCDB and the run no to 0.
   * @param cdbpath     path to the CDB
   * @return neg. error code if failed
   * @note function implemented in libHLTrec
   */
  int AliHLTMiscInitCDB(const char* cdbpath);
  typedef int (*AliHLTMiscInitCDB_t)(const char* cdbpath);

  /**
   * Init the Run no for the CDB access.
   * @param runNo       the run no
   * @return neg. error code if failed
   * @note function implemented in libHLTrec
   */
  int AliHLTMiscSetCDBRunNo(int runNo);
  typedef int (*AliHLTMiscSetCDBRunNo_t)(int runNo);

#ifdef __cplusplus
}
#endif

template<class T>
T* AliHLTMisc::LoadInstance(const T* /*t*/, const char* classname, const char* library)
{
  // see header file for function documentation
  int iLibResult=0;
  T* pInstance=NULL;
  AliHLTLogging log;
  TClass* pCl=NULL;
  ROOT::NewFunc_t pNewFunc=NULL;
  do {
    pCl=TClass::GetClass(classname);
  } while (!pCl && (iLibResult=gSystem->Load(library))==0);
  if (iLibResult>=0) {
    if (pCl && (pNewFunc=pCl->GetNew())!=NULL) {
      void* p=(*pNewFunc)(NULL);
      if (p) {
	pInstance=reinterpret_cast<T*>(p);
	if (!pInstance) {
	  log.Logging(kHLTLogError, "AliHLTMisc::LoadInstance", "HLT Analysis", "type cast (%s) to instance failed", classname);
	}
      } else {
	log.Logging(kHLTLogError, "AliHLTMisc::LoadInstance", "HLT Analysis", "can not create instance of type %s from class descriptor", classname);
      }
    } else {
      log.Logging(kHLTLogError, "AliHLTMisc::LoadInstance", "HLT Analysis", "can not find class descriptor %s", classname);
    }
  } else {
    log.Logging(kHLTLogError, "AliHLTMisc::LoadInstance", "HLT Analysis", "can not load %s library in order to find class descriptor %s", library, classname);
  }
  return pInstance;
}

// direct printout of data type struct
ostream  &operator<<(ostream &str, const AliHLTComponentDataType&);

#endif //ALIHLTMISC_H

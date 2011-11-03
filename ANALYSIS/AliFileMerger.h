#ifndef ALIFILEMERGER_H
#define ALIFILEMERGER_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//////////////////////////////////////////////////////////////////////////
//
//  Utilities for file merging
//
//////////////////////////////////////////////////////////////////////////

class TObjString;

#include "TNamed.h"

class AliFileMerger : public TNamed
{

 public:
  AliFileMerger();
  AliFileMerger(const char* name);
  void Merge(TFile* fileIn, TObjArray * array);

  void IterTXT( const char * fileList,  const char* outputFileName,Bool_t dontOverwrite=kFALSE);
  void IterAlien(const char* outputDir, const char* outputFileName = "CalibObjects.root" , const char* pattern = "AliESDfriends_v1.root", Bool_t dontOverwrite=kFALSE);
  //
  void StoreResults(TObjArray * array, const char* outputFileName);
  void StoreSeparateResults(TObjArray * array, const char* outputFileName);
  //
  Bool_t IsAccepted(TString name);
  void AddReject(const char *reject);
  void AddAccept(const char *accept);
  void SetNoTrees(Bool_t v=kTRUE)        {fNoTrees = v;}
  Bool_t IsNoTrees()               const {return fNoTrees;}

protected:
  int AddFile(TList* sourcelist, std::string entry);
  int MergeRootfile( TDirectory *target, TList *sourceNames);

protected:
  TObjArray * fRejectMask;  // mask of the objects to be rejected
  TObjArray * fAcceptMask;    // mask of the objects to be accepted
  Bool_t      fNoTrees;       // do we merge trees

private:
  AliFileMerger(const AliFileMerger&);
  AliFileMerger& operator=(const AliFileMerger& other);
  
  ClassDef(AliFileMerger, 1); // File merger utilities for AliRoot
};

#endif

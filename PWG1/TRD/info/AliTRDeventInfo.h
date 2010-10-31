#ifndef ALITRDEVENTINFO_H
#define ALITRDEVENTINFO_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Event info for TRD performance train                                  //
//                                                                        //
//  Authors:                                                              //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TObject.h>

class AliESDHeader;
class AliESDRun;

class AliTRDeventInfo : public TObject{
public:
  AliTRDeventInfo();
  AliTRDeventInfo(AliESDHeader *header, AliESDRun *run);
  AliTRDeventInfo(const AliTRDeventInfo &info);
  AliTRDeventInfo& operator=(const AliTRDeventInfo &info);
  virtual ~AliTRDeventInfo();
  virtual void Delete(const Option_t *);

  AliESDHeader *GetEventHeader() const { return fHeader; }
  AliESDRun *GetRunInfo() const { return fRun; }
  Bool_t IsOwner() const { return TestBit(kOwner); }
  void SetEventHeader(AliESDHeader *evHeader){ fHeader = evHeader; }
  void SetRunInfo(AliESDRun *evRun) { fRun = evRun; }
  void SetOwner();

private:
  enum{
    kOwner = BIT(14)
  };
  AliESDHeader *fHeader;		//! The ESD Header
  AliESDRun *fRun;		      //! The ESD Run Info

  ClassDef(AliTRDeventInfo, 1)
};
#endif

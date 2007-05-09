#ifndef ALITRDRECONSTRUCTOR_H
#define ALITRDRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class for TRD reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliReconstructor.h"

class AliRawReader;

class AliTRDReconstructor: public AliReconstructor {

 public:

  AliTRDReconstructor(): AliReconstructor()                             { };
  virtual ~AliTRDReconstructor()                                        { };

  virtual void        ConvertDigits(AliRawReader *rawReader, TTree *digitsTree) const;

  virtual void        Reconstruct(AliRunLoader *runLoader, AliRawReader *rawReader) const;
  virtual void        Reconstruct(AliRawReader *rawReader, TTree *clusterTree) const;
  virtual void        Reconstruct(TTree *digitsTree, TTree *clusterTree) const;
  virtual void        Reconstruct(AliRunLoader *runLoader) const;

  virtual AliTracker *CreateTracker(AliRunLoader *runLoader) const;

  virtual void        FillESD(AliRunLoader *runLoader, AliRawReader *rawReader, AliESD *esd) const;
  virtual void        FillESD(AliRawReader *rawReader, TTree *clusterTree, AliESD *esd) const;
  virtual void        FillESD(TTree *digitsTree, TTree *clusterTree, AliESD *esd) const;
  virtual void        FillESD(AliRunLoader *runLoader, AliESD *esd) const;

  static  void        SetSeedingOn(Bool_t seeding)               { fgkSeedingOn  = seeding; }  
  static  void        SetStreamLevel(Int_t level)                { fgStreamLevel = level;   }

  static  Bool_t      SeedingOn()                                { return fgkSeedingOn;     }
  static  Int_t       StreamLevel()                              { return fgStreamLevel;    }

 private:

  static  Bool_t   fgkSeedingOn;  //  Set flag for seeding during reconstruction
  static  Int_t    fgStreamLevel; //  Flag for streaming

  ClassDef(AliTRDReconstructor,0) //  Class for the TRD reconstruction

};

#endif

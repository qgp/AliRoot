//   Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved.
//  See cxx source for full Copyright notice                               
//  AliFMDReconstruction.h 
//  Task Class for making TreeR in FMD                        
//-- Authors: Evgeny Karpechev (INR) and Alla Maevskaia (INR)
/*
    Reconstruct nember of particles 
    in given group of pads for given FMDvolume
    determine by numberOfVolume , 
    numberOfMinSector,numberOfMaxSector,
    numberOfMinRing, numberOfMaxRing
    Reconstruction method choose dependence on number of empty pads  
  */


#ifndef AliFMDReconstruction_h
#define AliFMDReconstruction_h

#include "TTask.h"
class TString;
class AliFMD;

class AliRunLoader;

class AliFMDReconstruction: public TTask 
{
 public:
  AliFMDReconstruction() ; 
  AliFMDReconstruction(AliRunLoader* rl) ; 
  virtual ~AliFMDReconstruction();
  virtual void  Exec(Option_t *option); 
  void SetNEvents(Int_t Nevents){fNevents = Nevents;}
  Stat_t GetNEvents()  {return fNevents;}
  TClonesArray *Digits() {return fDigits;}
  Int_t DeterminationByThresholds(Int_t a[10][50][800], 
				  Int_t volume, Int_t Rmin, Int_t Rmax, 
				  Int_t Smin, Int_t Smax);
  Int_t DeterminationByPoisson (Int_t PadADC[10][50][800], Int_t volume,
				Int_t Rmin, Int_t Rmax, 
				Int_t Smin, Int_t Smax);
  
 private:
  TClonesArray *fDigits;               // ! array with digits
  Int_t   fNevents ;                         // Number of events

  AliRunLoader* fRunLoader;  //!Run Loader of that event

  ClassDef(AliFMDReconstruction,2) 


}; 
#endif










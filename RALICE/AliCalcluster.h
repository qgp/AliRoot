#ifndef ALICALCLUSTER_H
#define ALICALCLUSTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

#include "Riostream.h"
#include <math.h>
 
#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
 
#include "AliCalmodule.h"
#include "AliMath.h"
 
class AliCalcluster : public TObject,public AliPosition
{
 public:
  AliCalcluster();                   // Default constructor, all data initialised to 0
  ~AliCalcluster();                  // Default destructor
  AliCalcluster(AliCalmodule& m);    // Create new cluster starting at module m
  Int_t GetRow();                    // Return row number of cluster center
  Int_t GetColumn();                 // Return column number of cluster center
  Float_t GetSignal(Int_t n=0);      // Return signal of nxn matrix around the center
  Int_t GetNmodules();               // Return number of modules in cluster
  Float_t GetRowDispersion();        // Return normalised row dispersion of cluster
  Float_t GetColumnDispersion();     // Return normalised column dispersion of cluster
  void Start(AliCalmodule& m);       // Reset cluster data to start with module m
  void Add(AliCalmodule& m);         // Add module data to cluster
  void AddVetoSignal(AliSignal& s,Int_t extr=1); // Associate (extrapolated) signal
  void AddVetoSignal(AliSignal* s,Int_t extr=1) { AddVetoSignal(*s,extr); }
  AliSignal* GetVetoSignal(Int_t j); // Access to veto signal number j
  Int_t GetNvetos();                 // Provide the number of veto signals
  Float_t GetVetoLevel();            // Provide confidence level of best associated veto hit
  Int_t HasVetoHit(Double_t cl);     // Check for ass. veto hit with conf. level > cl
 
 protected:
  AliCalmodule* fCenter; // Pointer to the central module of the cluster
  Float_t fSig;          // The total signal value of the cluster
  Int_t fNmods;          // The number of modules in the cluster
  Float_t fSig11;        // Cluster signal of the central module
  Float_t fSig33;        // Cluster signal in 3x3 matrix around the center
  Float_t fSig55;        // Cluster signal in 5x5 matrix around the center
  Float_t fRowdisp;      // Row dispersion of cluster (not normalised)
  Float_t fColdisp;      // Column dispersion of cluster (not normalised)
  Int_t fNvetos;         // The number of associated veto signals
  TObjArray* fVetos;     // The array of associated veto signals
 
 ClassDef(AliCalcluster,2) // Description of a cluster of calorimeter modules.
};
#endif

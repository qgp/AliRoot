#ifndef ALIMUONCLUSTERRECONSTRUCTOR_H
#define ALIMUONCLUSTERRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

////////////////////////////////////
// MUON event reconstructor in ALICE
////////////////////////////////////
#include "TObjArray.h"
#include "AliDetector.h"

class AliLoader;
class AliMUON;
class AliMUONChamber;
class AliMUONRawCluster;
class AliMUONClusterFinderVS;
class AliMUONData;


class AliMUONClusterReconstructor : public TObject {

 public:
  AliMUONClusterReconstructor(AliLoader* ); // Constructor
  virtual ~AliMUONClusterReconstructor(void); // Destructor
  AliMUONClusterReconstructor (const AliMUONClusterReconstructor& ); // copy constructor
  AliMUONClusterReconstructor& operator=(const AliMUONClusterReconstructor& ); // assignment operator

  // Interface with AliMUONData
  virtual void       SetTreeAddress(){};
    
  // Cluster Finding & Trigger
  virtual void   Digits2Clusters();


  // void EventDump(void);  // dump reconstructed event
  
  // Set Reconstruction Model
  virtual void   SetReconstructionModel(Int_t id, AliMUONClusterFinderVS* );
 
  AliMUONData*   GetMUONData() {return fMUONData;}

  Int_t GetPrintLevel(void) const {return fPrintLevel;}
  void SetPrintLevel(Int_t PrintLevel) {fPrintLevel = PrintLevel;}

 protected:

 private:

  Int_t                   fNCh;                // Number of chambers   
  Int_t                   fNTrackingCh;        // Number of tracking chambers*
  AliMUONData*            fMUONData;           //! Data container for MUON subsystem 
  AliMUON*                fMUON;               //! pointer to MUON  
  TObjArray*              fChambers;           //! List of Tracking Chambers

 // print level
  Int_t fPrintLevel;

  // debug
  Int_t fDebug;
  
  // alice loader
  AliLoader* fLoader;


  ClassDef(AliMUONClusterReconstructor,0) // MUON cluster reconstructor in ALICE
};
	
#endif

#ifndef ALIADRECONSTRUCTOR_H
#define ALIADRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved.*/
/* See cxx source for full Copyright notice                              */
/* $Id: AliADReconstructor.h 20956 2007-09-26 14:22:18Z cvetan $  */

///////////////////////////////////////////////////////////////////////////
///                                                                      //
/// class for AD reconstruction                                       //
///                                                                      //
///////////////////////////////////////////////////////////////////////////

#include "AliReconstructor.h"
#include "AliLog.h"
#include "AliADConst.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliADRecoParam.h"

class AliESDAD;
class AliESDADfriend;
class AliESDEvent;
class AliADCalibData;

class AliADReconstructor: public AliReconstructor {
public:
  AliADReconstructor();
  virtual ~AliADReconstructor();
  virtual void   Init();
  
  virtual void   Reconstruct(AliRawReader* /*rawReader*/, 
		             TTree* /*clustersTree*/) const {
    AliError("Method not implemented"); return;};
  virtual void   Reconstruct(TTree*, TTree*) const {return;};
  
  virtual void   FillESD(TTree* digitsTree, TTree* /*clustersTree*/, 
			 AliESDEvent* esd) const;

  virtual void   FillESD(AliRawReader* /*rawReader*/, TTree* /*clustersTree*/, 
			 AliESDEvent* /*esd*/) const { 
    AliError("Method not implemented"); return;};
  
  virtual Bool_t HasDigitConversion() const { return kTRUE; }
  virtual void ConvertDigits(AliRawReader* rawReader, TTree* digitsTree) const;

  static const AliADRecoParam* GetRecoParam() { return dynamic_cast<const AliADRecoParam*>(AliReconstructor::GetRecoParam(14)); }

  AliCDBStorage     *SetStorage(const char* uri);
  void GetCollisionMode();

  AliADCalibData *GetCalibData() const; 

  enum {kInvalidADC   =  -1024,
        kInvalidTime  =  -1024};

  AliESDAD*    GetESDAD() { return fESDAD; }

protected:

  AliESDAD*        fESDAD;       // AD ESD object 
  AliESDEvent*     fESD;         // ESD object
  AliESDADfriend*  fESDADfriend; // AD ESDfriend object  

private:
  AliADReconstructor(const AliADReconstructor &reconstructor); //Not implemented
  AliADReconstructor& operator = (const AliADReconstructor &reconstructor); //Not implemented
  
  AliADCalibData* fCalibData;      //! calibration data
  mutable TClonesArray *fDigitsArray;  // clones-array for ConvertDigits() and FillESD()
  
  Int_t              fCollisionMode;  // =0->p-p, =1->A-A
  Float_t            fBeamEnergy;     // beam energy

  ClassDef(AliADReconstructor, 1)  // class for the AD reconstruction
};

#endif

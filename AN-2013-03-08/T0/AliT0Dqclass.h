#ifndef DAQ
#define DAQ

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//  class for T0 DAQ output                   //
////////////////////////////////////////////////

#include "TNamed.h"


class AliT0Dqclass: public TNamed {

 public:
  AliT0Dqclass( );
  AliT0Dqclass(const char* name);
  virtual ~AliT0Dqclass();
  void Reset();
  virtual void  Print(Option_t *) const; 
  //

  Float_t  GetTime(Int_t channel)   	   const {return fTime[channel];}
  Float_t* GetTime()   		   const {return (float*)fTime;}
  Float_t  GetAmplitude(Int_t channel)     const {return fAmplitude[channel];}
  Float_t* GetAmplitude()   		   const {return (float*)fAmplitude;}

  void  SetTime(Int_t channel, Float_t val) {fTime[channel]=val;}
  void  SetAmplitude(Int_t channel, Float_t val) {fAmplitude[channel]=val;}


 protected:
  // --- Pedestals
  Float_t  fTime[24];	          // Mean pedestal values 
  Float_t  fAmplitude[24];	 // Mean pedestal values 
  Float_t  fTotTime[24];

  ClassDef(AliT0Dqclass,1)    	 // DAQ data
};

typedef AliT0Dqclass AliSTARTDqclass; // for backward compatibility

#endif

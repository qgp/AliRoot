#ifndef ALIGENEVTGEN_H
#define ALIGENEVTGEN_H

/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  AliGenEvtGen class to performs decays of particles generated by a    //
//  previous generator. It inherits from AliGenerator.                   //
//                                                                       //  
//  Origin: Giuseppe.Bruno@ba.infn.it  &  Fiorella.Fionda@ba.infn.it     //
///////////////////////////////////////////////////////////////////////////

#include "AliGenerator.h"
#include "AliDecayerEvtGen.h"

class TParticle;

class AliGenEvtGen : public AliGenerator {

  public:

  typedef enum {kAllPart, kBeautyPart, kCharmPart} DecayOff_t;

  AliGenEvtGen();
 
  ~AliGenEvtGen();

  virtual void Init();
  virtual void Generate(); 
  AliDecayer* GetDecayer() {return fDecayer;}
  void SetForceDecay(Decay_t decay = kAll) {fForceDecay = decay;} //set a decay mode
  Bool_t SetUserDecayTable(Char_t *path);
  void SetParticleSwitchedOff(DecayOff_t decay) {fSwitchOff = decay;} //set particles to be decayed by EvtGen
  
  protected:
  Int_t GetFlavour(Int_t pdgCode); 
  
 
  private:
  AliStack *fStack;		   //!pointer to AliStack
  AliDecayerEvtGen  *fDecayer;     //!pointer to AliDecayerEvtGen
  Decay_t  fForceDecay;            //!decay case
  DecayOff_t fSwitchOff;           //!particle decay  
  Bool_t fUserDecay;		   //!TRUE if a user decay table is set	
  Char_t    *fUserDecayTablePath;  //!pointer to path of user decay table

  AliGenEvtGen(const AliGenEvtGen &EvtGen);
  AliGenEvtGen & operator=(const AliGenEvtGen & rhs);
  
  

  ClassDef(AliGenEvtGen,0) //AliGenerator interface to EvtGen
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif

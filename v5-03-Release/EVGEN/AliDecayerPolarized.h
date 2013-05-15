#ifndef ALIDECAYERPOLARIZED_H
#define ALIDECAYERPOLARIZED_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Class to generate decay products for polarized heavy quarkonia

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TF1.h>

#include "AliDecayer.h"


class AliDecayerPolarized : public AliDecayer
{
 public:
    typedef enum { kNoPol = 0, kColSop = 1, kHelicity = 2} Polar_t;
    typedef enum { kElectron = 1, kMuon = 2} FinState_t;
  AliDecayerPolarized();
  AliDecayerPolarized(Double_t alpha, Polar_t systref, FinState_t decprod);
  AliDecayerPolarized(const AliDecayerPolarized &decayer);
  virtual ~AliDecayerPolarized();
  void SetPolDec(Double_t alpha=0) {fAlpha=alpha;}
  void SetPolRefSys(Polar_t systref=kColSop) {fSystRef=systref;}
  void SetDecProd(FinState_t decprod=kMuon) {fDecProd=decprod;}
  virtual void  Init(){;}
  virtual void  Decay(Int_t ipart, TLorentzVector *p);
  virtual Int_t ImportParticles(TClonesArray *part);
  // The following methods are dummy
  virtual void    SetForceDecay(Int_t type);
  virtual void    ForceDecay();
  virtual Float_t GetPartialBranchingRatio(Int_t ipart);
  virtual Float_t GetLifetime(Int_t kf);
  virtual void    ReadDecayTable();
    
 protected:
  Double_t fAlpha;       // Polarization parameter
  Polar_t fSystRef;      // Reference system for polarization
  FinState_t fDecProd;   // Choice of decay products
  TF1 *fPol;             // ! Angular distribution for decay products
  TParticle *fMother;    // ! Particle that has to be decayed
  TParticle *fDaughter1; // ! Decay product no. 1
  TParticle *fDaughter2; // ! Decay product no. 2

 private:
  void  Copy(TObject &decayer) const;
  AliDecayerPolarized &operator=(const AliDecayerPolarized &decayer) 
      {decayer.Copy(*this);return(*this);}
  
  ClassDef(AliDecayerPolarized,1) // Polarized 2-body quarkonium decay
};
#endif

 

#ifndef ALIEMCALPARTON_H
#define ALIEMCALPARTON_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */

#include <TObject.h>
//*-- Author: Renan Cabrera (Creighton U.)


class AliEMCALParton : public TObject {
public:
  AliEMCALParton();
  AliEMCALParton(Float_t energy, Float_t phi, Float_t eta);
  virtual ~AliEMCALParton();
  void SetEnergy(Float_t val) {fEnergy = val;}
  void SetPhi(Float_t val)    {fPhi    = val;}
  void SetEta(Float_t val)    {fEta    = val;}
  void SetTrackList(Int_t,Float_t*,Float_t*,Float_t*,Int_t*);
  void GetTrackList(Float_t*,Float_t*,Float_t*,Int_t*);
  
  Int_t GetNTracks(){return fNTracks;}
  Float_t Energy()  {return fEnergy;}
  Float_t Phi()     {return fPhi;}
  Float_t Eta()     {return fEta;}
  
protected:
  Float_t   fEnergy;   // Jet Energy
  Float_t   fEta;      // Jet Phi
  Float_t   fPhi;      // Jet Eta
  Int_t     fNTracks;      // Number of tracks 
  Float_t  fTrackEnergy[1000];  // Jet Tracks Energy
  Float_t  fTrackEta[1000];     // Jet Tracks Eta
  Float_t  fTrackPhi[1000];     // Jet Tracks Phi
  Int_t    fTrackPDG[1000];     // Jet Tracks PDG code
  ClassDef(AliEMCALParton,4) // Jet for EMCAL
    
} ;

#endif // ALIEMCALParton_H

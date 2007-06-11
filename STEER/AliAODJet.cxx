/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD class for jets
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include <TLorentzVector.h>
#include "AliAODJet.h"

ClassImp(AliAODJet)


//______________________________________________________________________________
AliAODJet::AliAODJet() :
    AliVirtualParticle(),
    fMomentum(0)
{
  // constructor
    fBackgEnergy[0]   = 0.;     
    fBackgEnergy[1]   = 0.;
    fEffectiveArea[0] = 0.;   
    fEffectiveArea[1] = 0.;   
}

AliAODJet::AliAODJet(Double_t px, Double_t py, Double_t pz, Double_t e):
    AliVirtualParticle(),
    fMomentum(0)
{
  // constructor
    fMomentum = new TLorentzVector(px, py, pz, e);
    fBackgEnergy[0]   = 0.;     
    fBackgEnergy[1]   = 0.;
    fEffectiveArea[0] = 0.;   
    fEffectiveArea[1] = 0.;   
}

AliAODJet::AliAODJet(TLorentzVector & p):
    AliVirtualParticle(),
    fMomentum(0)
{
  // constructor
    fMomentum = new TLorentzVector(p);
    fBackgEnergy[0]   = 0.;     
    fBackgEnergy[1]   = 0.;
    fEffectiveArea[0] = 0.;   
    fEffectiveArea[1] = 0.;   
}


//______________________________________________________________________________
AliAODJet::~AliAODJet() 
{
  // destructor
    delete fMomentum;
}

//______________________________________________________________________________
AliAODJet::AliAODJet(const AliAODJet& jet) :
    AliVirtualParticle(jet),
    fMomentum(0)
{
  // Copy constructor
    fMomentum = new TLorentzVector(*jet.fMomentum);
    
}

//______________________________________________________________________________
AliAODJet& AliAODJet::operator=(const AliAODJet& jet)
{
  // Assignment operator
  if(this!=&jet) {
  }

  return *this;
}

void AliAODJet::Print(Option_t* /*option*/) const 
{
  // Print information of all data members
  printf("Jet 4-vector:\n");
  printf("     E  = %13.3f\n", E() );
  printf("     Px = %13.3f\n", Px());
  printf("     Py = %13.3f\n", Py());
  printf("     Pz = %13.3f\n", Pz());
  printf("Background Energy:\n");
  printf("Charged:  %13.3f\n", ChargedBgEnergy());
  printf("Neutral:  %13.3f\n", NeutralBgEnergy());
  printf("Total:    %13.3f\n", TotalBgEnergy());
  printf("Effective Area: \n");
  printf("Charged:  %13.3f\n", EffectiveAreaCharged());
  printf("Neutral:  %13.3f\n", EffectiveAreaNeutral());
}

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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


//-------------------------------------------------------------------------
//                      Implementation of   Class AliESDZDC
//   This is a class that summarizes the ZDC data
//   for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------



#include "AliESDZDC.h"

ClassImp(AliESDZDC)

//______________________________________________________________________________
AliESDZDC::AliESDZDC() :
  TObject(),
  fZDCN1Energy(0),
  fZDCP1Energy(0),
  fZDCN2Energy(0),
  fZDCP2Energy(0),
  fZDCEMEnergy(0),
  fZDCEMEnergy1(0),
  fZDCParticipants(0)
{
  for(int i=0; i<5; i++){
    fZN1TowerEnergy[i] = fZN2TowerEnergy[i] = 0.;
    fZP1TowerEnergy[i] = fZP2TowerEnergy[i] = 0.;
    fZN1TowerEnergyLR[i] = fZN2TowerEnergyLR[i] = 0.;
    fZP1TowerEnergyLR[i] = fZP2TowerEnergyLR[i] = 0.;
  }
}

AliESDZDC::AliESDZDC(const AliESDZDC& zdc) :
  TObject(zdc),
  fZDCN1Energy(zdc.fZDCN1Energy),
  fZDCP1Energy(zdc.fZDCP1Energy),
  fZDCN2Energy(zdc.fZDCN2Energy),
  fZDCP2Energy(zdc.fZDCP2Energy),
  fZDCEMEnergy(zdc.fZDCEMEnergy),
  fZDCEMEnergy1(zdc.fZDCEMEnergy1),
  fZDCParticipants(zdc.fZDCParticipants)
{
  // copy constructor
  for(int i=0; i<5; i++){
     fZN1TowerEnergy[i] = zdc.fZN1TowerEnergy[i];
     fZN2TowerEnergy[i] = zdc.fZN2TowerEnergy[i];
     fZP1TowerEnergy[i] = zdc.fZP1TowerEnergy[i];
     fZP2TowerEnergy[i] = zdc.fZP2TowerEnergy[i];
     fZN1TowerEnergyLR[i] = zdc.fZN1TowerEnergyLR[i];
     fZN2TowerEnergyLR[i] = zdc.fZN2TowerEnergyLR[i];
     fZP1TowerEnergyLR[i] = zdc.fZP1TowerEnergyLR[i];
     fZP2TowerEnergyLR[i] = zdc.fZP2TowerEnergyLR[i];
  }
}

AliESDZDC& AliESDZDC::operator=(const AliESDZDC&zdc)
{
  // assigment operator
  if(this!=&zdc) {
    TObject::operator=(zdc);
    fZDCN1Energy = zdc.fZDCN1Energy;
    fZDCP1Energy = zdc.fZDCP1Energy;
    fZDCN2Energy = zdc.fZDCN2Energy;
    fZDCP2Energy = zdc.fZDCP2Energy;
    fZDCParticipants = zdc.fZDCParticipants;
    fZDCEMEnergy = zdc.fZDCEMEnergy;
    fZDCEMEnergy1 = zdc.fZDCEMEnergy1;
    for(Int_t i=0; i<5; i++){
       fZN1TowerEnergy[i] = zdc.fZN1TowerEnergy[i];
       fZN2TowerEnergy[i] = zdc.fZN2TowerEnergy[i];
       fZP1TowerEnergy[i] = zdc.fZP1TowerEnergy[i];
       fZP2TowerEnergy[i] = zdc.fZP2TowerEnergy[i];
       fZN1TowerEnergyLR[i] = zdc.fZN1TowerEnergyLR[i];
       fZN2TowerEnergyLR[i] = zdc.fZN2TowerEnergyLR[i];
       fZP1TowerEnergyLR[i] = zdc.fZP1TowerEnergyLR[i];
       fZP2TowerEnergyLR[i] = zdc.fZP2TowerEnergyLR[i];
    }
  } 
  return *this;
}


//______________________________________________________________________________
void AliESDZDC::Reset()
{
  // reset all data members
  fZDCN1Energy=0;
  fZDCP1Energy=0;
  fZDCN2Energy=0;
  fZDCP2Energy=0;
  fZDCParticipants=0;  
  fZDCEMEnergy=0;
  fZDCEMEnergy1=0;
  for(int i=0; i<5; i++){
    fZN1TowerEnergy[i] = fZN2TowerEnergy[i] = 0.;
    fZP1TowerEnergy[i] = fZP2TowerEnergy[i] = 0.;
    fZN1TowerEnergyLR[i] = fZN2TowerEnergyLR[i] = 0.;
    fZP1TowerEnergyLR[i] = fZP2TowerEnergyLR[i] = 0.;
  }
}

//______________________________________________________________________________
void AliESDZDC::Print(const Option_t *) const
{
  //  Print ESD for the ZDC
  printf("\n \t ZN1Energy = %f TeV, ZP1Energy = %f TeV, ZN2Energy = %f TeV,"
  " ZP2Energy = %f, Nparticipants = %d\n",
  fZDCN1Energy,fZDCP1Energy,fZDCN2Energy,fZDCP2Energy,fZDCParticipants);
}



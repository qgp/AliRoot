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

/*
$Log$
Revision 1.3  2001/07/27 17:24:28  morsch
Some bugs corrected. ( Piotr Skowronski)

Revision 1.2  2001/03/28 07:31:48  hristov
Loop variables declared only once (HP,Sun)

Revision 1.1  2001/03/24 10:04:44  morsch
MevSim interfaced through AliGenerator, first commit (Sylwester Radomski et al.)
//Piotr Skowronski Line 104: fThetaMin-->fThetaMax
*/

//
// Wrapper for MEVSIM generator.
// It is using TMevSim to comunicate with fortarn code
// 
//	
// Sylwester Radomski <radomski@if.pw.edu.pl>
//

//#include "TSystem.h"
//#include "TUnixSystem.h"
#include "TParticle.h"
#include "TMevSim.h"

#include "AliGenMevSim.h"
#include "AliRun.h"


ClassImp(AliGenMevSim)

//____________________________________________________________________________
AliGenMevSim::AliGenMevSim() : AliGenerator(-1) 
{
  //
  // Standard creator
  //
  
  fConfig = new AliMevSimConfig();
  fMevSim = new TMevSim();
  sRandom = fRandom;
  
}
//____________________________________________________________________________
AliGenMevSim::AliGenMevSim(AliMevSimConfig *config): AliGenerator(-1) {

  fConfig = config;
  fMevSim = new TMevSim(); 
  sRandom = fRandom;
}

//____________________________________________________________________________
AliGenMevSim::~AliGenMevSim() 
{
  //
  // Standard destructor
  //
  if (fMevSim) delete fMevSim;
}
//____________________________________________________________________________
void AliGenMevSim::SetConfig(AliMevSimConfig *config) {
  
  fConfig = config;
}
//____________________________________________________________________________
void AliGenMevSim::AddParticleType(AliMevSimParticle *type) {

  fMevSim->AddPartTypeParams((TMevSimPartTypeParams*)type);
}
//____________________________________________________________________________
void AliGenMevSim::Init() 
{
  //
  // Generator initialisation method
  //

  // fill data from AliMevSimConfig;

  TMevSim *mevsim = fMevSim;

  // geometry & momentum cut

  if (TestBit(kPtRange))  mevsim->SetPtCutRange(fPtMin, fPtMax);
  
  if (TestBit(kPhiRange)) // from radians to 0 - 360 deg
    mevsim->SetPhiCutRange( fPhiMin*180/((Float_t)TMath::Pi()) + 180.0,//cast to float is because if fPhiMin=-Pi
                            fPhiMax*180/((Float_t)TMath::Pi()) + 180.0 );//passed value is non 0 and negative (but smaall)
  
  if (TestBit(kThetaRange)) // from theta to eta
  {
    mevsim->SetEtaCutRange( -TMath::Log( TMath::Tan(fThetaMax/2)) , -TMath::Log( TMath::Tan(fThetaMin/2)) );
  }  

  // mevsim specyfic parameters
  
  mevsim->SetModelType(fConfig->fModelType);
  mevsim->SetReacPlaneCntrl(fConfig->fReacPlaneCntrl);
  mevsim->SetPsiRParams(fConfig->fPsiRMean, fConfig->fPsiRStDev);
  mevsim->SetMultFacParams(fConfig->fMultFacMean, fConfig->fMultFacStDev);
  
  // mevsim initialisation

  mevsim->Initialize();
}

//____________________________________________________________________________
void AliGenMevSim::Generate() 
{
  //
  // Read the formatted output file and load the particles
  // Temporary solution
  //

  Int_t i;

  PDG_t pdg;
  Float_t orgin[3] = {0,0,0};
  Float_t polar[3] = {0,0,0};
  Float_t p[3] = {1,1,1};
  Float_t time = 0;
  
  const Int_t parent = -1;
  Int_t id;

  // vertexing 

  VertexInternal();

  orgin[0] = fVertex[0];
  orgin[1] = fVertex[1];
  orgin[2] = fVertex[2];

  cout << "Vertex ";
  for (i =0; i<3; i++)
    cout << orgin[i] << "\t";
  cout << endl;

  Int_t nParticles = 0;

  TClonesArray *particles = new TClonesArray("TParticle");
  TParticle *particle;

  fMevSim->GenerateEvent();
  fNpart= fMevSim->ImportParticles(particles,"");

  cout << "Found " << fNpart << " particles ..." << endl;
  nParticles = fNpart;

  for (i=0; i<nParticles; i++) {
    
    particle = (TParticle*) (*particles)[i];

    pdg = (PDG_t) particle->GetPdgCode();
    p[0] = particle->Px();
    p[1] = particle->Py();
    p[2] = particle->Pz();
    
    SetTrack(fTrackIt, parent, pdg, p, orgin, polar, time, kPPrimary, id);

  }  
 
  particles->Clear();
  if (particles) delete particles;
}
//____________________________________________________________________________
//____________________________________________________________________________


#ifndef WIN32
# define ran ran_
# define type_of_call
#else
# define ran RAN
# define type_of_call _stdcall
#endif

extern "C" Float_t type_of_call ran(Int_t &)
{
  return sRandom->Rndm(); 
}

//____________________________________________________________________________

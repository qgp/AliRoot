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

/* $Id: AliGenEMCocktail.cxx 40702 2010-04-26 13:09:52Z morsch $ */

// Class to create the cocktail for physics with electrons, di-electrons,
// and photons from the decay of the following sources:
// pizero, eta, rho, omega, etaprime, phi
// Kinematic distributions of the sources are taken from AliGenEMlib.
// Decay channels can be selected via the method SetDecayMode.
// Particles can be generated flat in pT with weights according to the
// chosen pT distributions from AliGenEMlib (weighting mode: kNonAnalog),
// or they are generated according to the pT distributions themselves
// (weighting mode: kAnalog)  
 
 
#include <TObjArray.h>
#include <TParticle.h>
#include <TF1.h>
#include <TVirtualMC.h>
#include <TPDGCode.h>
#include <TDatabasePDG.h>
#include "AliGenCocktailEventHeader.h"

#include "AliGenCocktailEntry.h"
#include "AliGenEMCocktail.h"
#include "AliGenEMlib.h"
#include "AliGenBox.h"
#include "AliGenParam.h"
#include "AliMC.h"
#include "AliRun.h"
#include "AliStack.h"
#include "AliDecayer.h"
#include "AliDecayerPythia.h"
#include "AliLog.h"
#include "AliGenCorrHF.h"

ClassImp(AliGenEMCocktail)  
  
//________________________________________________________________________
AliGenEMCocktail::AliGenEMCocktail()
:AliGenCocktail(),
   fDecayer(0),
   fDecayMode(kAll),
   fWeightingMode(kNonAnalog),
   fNPart(1000),
  fYieldArray(),
  fPtSelect(0),
  fCentrality(0),
  fV2Systematic(0),
  fForceConv(kFALSE),
  fSelectedParticles(kGenHadrons)
{
  // Constructor

}

//_________________________________________________________________________
AliGenEMCocktail::~AliGenEMCocktail()
{
  // Destructor

}

//_________________________________________________________________________
void AliGenEMCocktail::CreateCocktail()
{
  // create and add sources to the cocktail

  fDecayer->SetForceDecay(fDecayMode);
  fDecayer->ForceDecay();

  // Set kinematic limits
  Double_t ptMin  = fPtMin;
  Double_t ptMax  = fPtMax;
  Double_t yMin   = fYMin;;
  Double_t yMax   = fYMax;;
  Double_t phiMin = fPhiMin*180./TMath::Pi();
  Double_t phiMax = fPhiMax*180./TMath::Pi();
  AliInfo(Form("Ranges pT:%4.1f : %4.1f GeV/c, y:%4.2f : %4.2f, Phi:%5.1f : %5.1f degres",ptMin,ptMax,yMin,yMax,phiMin,phiMax));
  AliInfo(Form("the parametrised sources uses the decay mode %d",fDecayMode));

  //Initialize user selection for Pt Parameterization and centrality:
  AliGenEMlib::SelectParams(fPtSelect,fCentrality,fV2Systematic);

  // Create and add electron sources to the generator
  // pizero
  if(fSelectedParticles&kGenPizero){
  AliGenParam *genpizero=0;
  Char_t namePizero[10];    
  snprintf(namePizero,10,"Pizero");    
  genpizero = new AliGenParam(fNPart/0.925, new AliGenEMlib(), AliGenEMlib::kPizero, "DUMMY");
  genpizero->SetYRange(fYMin/0.925, fYMax/0.925);
  AddSource2Generator(namePizero,genpizero);
  TF1 *fPtPizero = genpizero->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kPizero] = fPtPizero->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kPizero] = fPtPizero->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // eta  
  if(fSelectedParticles&kGenEta){
  AliGenParam *geneta=0;
  Char_t nameEta[10];    
  snprintf(nameEta,10,"Eta");    
  geneta = new AliGenParam(fNPart/0.825, new AliGenEMlib(), AliGenEMlib::kEta, "DUMMY");
  geneta->SetYRange(fYMin/0.825, fYMax/0.825);
  AddSource2Generator(nameEta,geneta);
  TF1 *fPtEta = geneta->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kEta] = fPtEta->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kEta] = fPtEta->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // rho  
  if(fSelectedParticles&kGenRho){
  AliGenParam *genrho=0;
  Char_t nameRho[10];    
  snprintf(nameRho,10,"Rho");    
  genrho = new AliGenParam(fNPart/0.775, new AliGenEMlib(), AliGenEMlib::kRho, "DUMMY");
  genrho->SetYRange(fYMin/0.775, fYMax/0.775);
  AddSource2Generator(nameRho,genrho);
  TF1 *fPtRho = genrho->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kRho] = fPtRho->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kRho] = fPtRho->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }
  
  // omega
  if(fSelectedParticles&kGenOmega){
  AliGenParam *genomega=0;
  Char_t nameOmega[10];    
  snprintf(nameOmega,10,"Omega");    
  genomega = new AliGenParam(fNPart/0.775, new AliGenEMlib(), AliGenEMlib::kOmega, "DUMMY");
  genomega->SetYRange(fYMin/0.775, fYMax/0.775);
  AddSource2Generator(nameOmega,genomega);
  TF1 *fPtOmega = genomega->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kOmega] = fPtOmega->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kOmega] = fPtOmega->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // etaprime
  if(fSelectedParticles&kGenEtaprime){
  AliGenParam *genetaprime=0;
  Char_t nameEtaprime[10];    
  snprintf(nameEtaprime,10,"Etaprime");    
  genetaprime = new AliGenParam(fNPart/0.725, new AliGenEMlib(), AliGenEMlib::kEtaprime, "DUMMY");
  genetaprime->SetYRange(fYMin/0.725, fYMax/0.725);
  AddSource2Generator(nameEtaprime,genetaprime);
  TF1 *fPtEtaprime = genetaprime->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kEtaprime] = fPtEtaprime->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kEtaprime] = fPtEtaprime->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // phi  
  if(fSelectedParticles&kGenPhi){
  AliGenParam *genphi=0;
  Char_t namePhi[10];    
  snprintf(namePhi,10,"Phi");    
  genphi = new AliGenParam(fNPart/0.725, new AliGenEMlib(), AliGenEMlib::kPhi, "DUMMY");
  genphi->SetYRange(fYMin/0.725, fYMax/0.725);
  AddSource2Generator(namePhi,genphi);
  TF1 *fPtPhi = genphi->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kPhi] = fPtPhi->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kPhi] = fPtPhi->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // jpsi  
  if(fSelectedParticles&kGenJpsi){
  AliGenParam *genjpsi=0;
  Char_t nameJpsi[10];    
  snprintf(nameJpsi,10,"Jpsi");    
  genjpsi = new AliGenParam(fNPart/0.525, new AliGenEMlib(), AliGenEMlib::kJpsi, "DUMMY");
  genjpsi->SetYRange(fYMin/0.525, fYMax/0.525);
  AddSource2Generator(nameJpsi,genjpsi);
  TF1 *fPtJpsi = genjpsi->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kJpsi] = fPtJpsi->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kJpsi] = fPtJpsi->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  // direct gamma
  if(fDecayMode!=kGammaEM) return;
  if(fSelectedParticles&kGenDirectRealGamma){
    AliGenParam *genDirectRealG=0;
    Char_t nameDirectRealG[10];    
    snprintf(nameDirectRealG,10,"DirectRealGamma");    
    genDirectRealG = new AliGenParam(fNPart, new AliGenEMlib(), AliGenEMlib::kDirectRealGamma, "DUMMY");
    genDirectRealG->SetYRange(fYMin, fYMax);
    AddSource2Generator(nameDirectRealG,genDirectRealG);
    TF1 *fPtDirectRealG = genDirectRealG->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kDirectRealGamma] = fPtDirectRealG->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kDirectRealGamma] = fPtDirectRealG->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }

  if(fSelectedParticles&kGenDirectVirtGamma){
    TDatabasePDG::Instance()->AddParticle("DirectVirtGamma","DirectVirtGamma",0,true,0,0,"GaugeBoson",220000);
    AliGenParam *genDirectVirtG=0;
    Char_t nameDirectVirtG[10];    
    snprintf(nameDirectVirtG,10,"DirectVirtGamma");    
    genDirectVirtG = new AliGenParam(fNPart/0.775, new AliGenEMlib(), AliGenEMlib::kDirectVirtGamma, "DUMMY");
    genDirectVirtG->SetYRange(fYMin/0.775, fYMax/0.775);
    AddSource2Generator(nameDirectVirtG,genDirectVirtG);
    TF1 *fPtDirectVirtG = genDirectVirtG->GetPt();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
    fYieldArray[kDirectVirtGamma] = fPtDirectVirtG->Integral(fPtMin,fPtMax,1.e-6);
#else
    fYieldArray[kDirectVirtGamma] = fPtDirectVirtG->Integral(fPtMin,fPtMax,(Double_t *)0,1.e-6);
#endif
  }
}

//-------------------------------------------------------------------
void AliGenEMCocktail::AddSource2Generator(Char_t* nameSource, 
					 AliGenParam* const genSource)
{
// add sources to the cocktail
  Double_t phiMin = fPhiMin*180./TMath::Pi();
  Double_t phiMax = fPhiMax*180./TMath::Pi();

  genSource->SetPtRange(fPtMin, fPtMax);  
  genSource->SetPhiRange(phiMin, phiMax);
  genSource->SetWeighting(fWeightingMode);
  genSource->SetForceGammaConversion(fForceConv);
  if (!TVirtualMC::GetMC()) genSource->SetDecayer(fDecayer);  
  genSource->Init();
    
  AddGenerator(genSource,nameSource,1.); // Adding Generator    
}

//-------------------------------------------------------------------
void AliGenEMCocktail::Init()
{
  // Initialisation
  TIter next(fEntries);
  AliGenCocktailEntry *entry;
  if (fStack) {
    while((entry = (AliGenCocktailEntry*)next())) {
      entry->Generator()->SetStack(fStack);
    }
  }
}

//_________________________________________________________________________
void AliGenEMCocktail::Generate()
{
  // Generate event 
  TIter next(fEntries);
  AliGenCocktailEntry *entry = 0;
  AliGenCocktailEntry *preventry = 0;
  AliGenerator* gen = 0;

  if (fHeader) delete fHeader;
  fHeader = new AliGenCocktailEventHeader("Electromagnetic Cocktail Header");

  const TObjArray *partArray = gAlice->GetMCApp()->Particles();
    
  // Generate the vertex position used by all generators    
  if(fVertexSmear == kPerEvent) Vertex();

  //Reseting stack
  AliRunLoader * runloader = AliRunLoader::Instance();
  if (runloader)
    if (runloader->Stack())
      runloader->Stack()->Clean();
  
  // Loop over generators and generate events
  Int_t igen = 0;
  Float_t evPlane;
  Rndm(&evPlane,1);
  evPlane*=TMath::Pi()*2;
  while((entry = (AliGenCocktailEntry*)next())) {
    gen = entry->Generator();
    gen->SetVertex(fVertex.At(0), fVertex.At(1), fVertex.At(2));
    
    if (fNPart > 0) {
      igen++;	
      if (igen == 1) entry->SetFirst(0);		
      else  entry->SetFirst((partArray->GetEntriesFast())+1);
      gen->SetEventPlane(evPlane);
      gen->Generate();
      entry->SetLast(partArray->GetEntriesFast());
      preventry = entry;
    }
  }  
  next.Reset();

  // Setting weights for proper absolute normalization
  Int_t iPart, iMother;
  Int_t pdgMother = 0;
  Double_t weight = 0.;
  Double_t dNdy = 0.;
  Int_t maxPart = partArray->GetEntriesFast();
  for(iPart=0; iPart<maxPart; iPart++){      
    TParticle *part = gAlice->GetMCApp()->Particle(iPart);
    iMother = part->GetFirstMother();
    TParticle *mother = 0;
    if (iMother>=0){
      mother = gAlice->GetMCApp()->Particle(iMother);
      pdgMother = mother->GetPdgCode();
    }
    else
      pdgMother = part->GetPdgCode();

    switch (pdgMother){
    case 111:
      dNdy = fYieldArray[kPizero];
      break;
    case 221:
      dNdy = fYieldArray[kEta];
      break;
    case 113:
      dNdy = fYieldArray[kRho];
      break;
    case 223:
      dNdy = fYieldArray[kOmega];
      break;
    case 331:
      dNdy = fYieldArray[kEtaprime];
      break;
    case 333:
      dNdy = fYieldArray[kPhi];
      break;
    case 443:
      dNdy = fYieldArray[kJpsi];
      break;
    case 22:
      dNdy = fYieldArray[kDirectRealGamma];
      break;
    case 220000:
      dNdy = fYieldArray[kDirectVirtGamma];
      break;
      
    default:
      dNdy = 0.;
    }

    weight = dNdy*part->GetWeight();
    part->SetWeight(weight);
  }	
  
  fHeader->SetNProduced(maxPart);


  TArrayF eventVertex;
  eventVertex.Set(3);
  for (Int_t j=0; j < 3; j++) eventVertex[j] = fVertex[j];
  
  fHeader->SetPrimaryVertex(eventVertex);

  gAlice->SetGenEventHeader(fHeader);
}

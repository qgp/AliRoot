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

/* $Id$ */

//
// Classe to create the coktail for physics with muons for pp at 14 TeV
// The followoing sources: 
// jpsi,psiP, upsilon, upsilonP, upsilonPP are added to Pythia
// The free parameeters are :
//      pp reaction cross-section
//      production cross-sections in pp collisions 
// July 07:added heavy quark production from AliGenCorrHF and heavy quark 
//         production switched off in Pythia 
// Aug. 07: added trigger cut on total momentum

#include <TObjArray.h>
#include <TParticle.h>
#include <TF1.h>
#include <TVirtualMC.h>

#include "AliGenCocktailEventHeader.h"

#include "AliGenCocktailEntry.h"
#include "AliGenMUONCocktailpp.h"
#include "AliGenMUONlib.h"
#include "AliGenParam.h"
#include "AliMC.h"
#include "AliRun.h"
#include "AliStack.h"
#include "AliDecayer.h"
#include "AliLog.h"
#include "AliGenCorrHF.h"

ClassImp(AliGenMUONCocktailpp)  
  
//________________________________________________________________________
AliGenMUONCocktailpp::AliGenMUONCocktailpp()
    :AliGenCocktail(),
     fDecayer(0),
     fDecayModeResonance(kAll),
     fDecayModePythia(kAll),
     fMuonMultiplicity(0),
     fMuonPtCut(0.),
     fMuonPCut(0.),
     fMuonThetaMinCut(0.),
     fMuonThetaMaxCut(180.),
     fMuonOriginCut(-999.),
     fNSucceded(0),
     fNGenerated(0),
     fSigmaJPsi(49.44e-6),
     fSigmaPsiP(7.67e-6),
     fSigmaUpsilon(0.989e-6),
     fSigmaUpsilonP(0.502e-6),
     fSigmaUpsilonPP(0.228e-6),
     fSigmaCCbar(11.2e-3),
     fSigmaBBbar(0.51e-3),
     fSigmaSilent(kFALSE)
{
// Constructor
}

//_________________________________________________________________________
AliGenMUONCocktailpp::~AliGenMUONCocktailpp()
// Destructor
{
    
}

//_________________________________________________________________________
void AliGenMUONCocktailpp::CreateCocktail()
{
// MinBias NN cross section @ pp 14 TeV -PR  Vol II p:473
    Double_t sigmaReaction =   0.070; 
    
// These limits are only used for renormalization of quarkonia cross section
// Pythia events are generated in 4pi  
    Double_t ptMin  = fPtMin;
    Double_t ptMax  = fPtMax;
    Double_t yMin   = fYMin;;
    Double_t yMax   = fYMax;;
    Double_t phiMin = fPhiMin*180./TMath::Pi();
    Double_t phiMax = fPhiMax*180./TMath::Pi();
    AliInfo(Form("Ranges pT:%4.1f : %4.1f GeV/c, y:%4.2f : %4.2f, Phi:%5.1f : %5.1f degres",ptMin,ptMax,yMin,yMax,phiMin,phiMax));
    
// Ratios with respect to the reaction cross-section in the 
// kinematics limit of the MUONCocktail
    Double_t ratiojpsi;
    Double_t ratiopsiP;
    Double_t ratioupsilon;
    Double_t ratioupsilonP;
    Double_t ratioupsilonPP;
    Double_t ratioccbar;
    Double_t ratiobbbar;

// Cross sections in barns (from PPR Vol. II p: 552) pp - 14 TeV and 
// corrected from feed down of higher resonances 

    Double_t sigmajpsi      = fSigmaJPsi;  
    Double_t sigmapsiP      = fSigmaPsiP;  
    Double_t sigmaupsilon   = fSigmaUpsilon;  
    Double_t sigmaupsilonP  = fSigmaUpsilonP;  
    Double_t sigmaupsilonPP = fSigmaUpsilonPP;
    Double_t sigmaccbar     = fSigmaCCbar;
    Double_t sigmabbbar     = fSigmaBBbar;

    AliInfo(Form("the parametrised resonances uses the decay mode %d",fDecayModeResonance));

// Generation using CDF scaled distribution for pp@14TeV (from D. Stocco)
//----------------------------------------------------------------------
// Jpsi generator
    AliGenParam * genjpsi = new AliGenParam(1, AliGenMUONlib::kJpsi, "CDF pp", "Jpsi");
// first step: generation in 4pi
    genjpsi->SetPtRange(0.,100.);
    genjpsi->SetYRange(-8.,8.);
    genjpsi->SetPhiRange(0.,360.);
    genjpsi->SetForceDecay(fDecayModeResonance);
    if (!gMC) genjpsi->SetDecayer(fDecayer);

    genjpsi->Init();  // generation in 4pi
    ratiojpsi = sigmajpsi / sigmaReaction * genjpsi->GetRelativeArea(ptMin,ptMax,yMin,yMax,phiMin,phiMax); // get weight
    if (!fSigmaSilent) {
      AliInfo(Form("jpsi prod. cross-section in pp(14 TeV) %5.3g b",sigmajpsi));
      AliInfo(Form("jpsi prod. probability per collision in acceptance %5.3g",ratiojpsi));
    }
// second step: generation in selected kinematical range
    genjpsi->SetPtRange(ptMin, ptMax);  
    genjpsi->SetYRange(yMin, yMax);
    genjpsi->SetPhiRange(phiMin, phiMax);
    genjpsi->Init(); // generation in selected kinematic range
    AddGenerator(genjpsi, "Jpsi", ratiojpsi); // Adding Generator
//------------------------------------------------------------------
// Psi prime generator
    AliGenParam * genpsiP = new AliGenParam(1, AliGenMUONlib::kPsiP, "CDF pp", "PsiP");
// first step: generation in 4pi
    genpsiP->SetPtRange(0.,100.);
    genpsiP->SetYRange(-8.,8.);
    genpsiP->SetPhiRange(0.,360.);
    genpsiP->SetForceDecay(fDecayModeResonance);
    if (!gMC) genpsiP->SetDecayer(fDecayer);

    genpsiP->Init();  // generation in 4pi
    ratiopsiP = sigmapsiP / sigmaReaction * genpsiP->GetRelativeArea(ptMin,ptMax,yMin,yMax,phiMin,phiMax);
    if (!fSigmaSilent) {
      AliInfo(Form("psiP prod. cross-section in pp(14 TeV) %5.3g b",sigmapsiP));
      AliInfo(Form("psiP prod. probability per collision in acceptance %5.3g",ratiopsiP));
    }
// second step: generation in selected kinematical range
    genpsiP->SetPtRange(ptMin, ptMax);  
    genpsiP->SetYRange(yMin, yMax);
    genpsiP->SetPhiRange(phiMin, phiMax);
    genpsiP->Init(); // generation in selected kinematic range
    AddGenerator(genpsiP, "PsiP", ratiopsiP); // Adding Generator
//------------------------------------------------------------------
// Upsilon 1S generator
    AliGenParam * genupsilon = new AliGenParam(1, AliGenMUONlib::kUpsilon, "CDF pp", "Upsilon");
// first step: generation in 4pi
    genupsilon->SetPtRange(0.,100.); 
    genupsilon->SetYRange(-8.,8.);
    genupsilon->SetPhiRange(0.,360.);
    genupsilon->SetForceDecay(fDecayModeResonance);
    if (!gMC) genupsilon->SetDecayer(fDecayer);
    genupsilon->Init();  // generation in 4pi
    ratioupsilon = sigmaupsilon / sigmaReaction * genupsilon->GetRelativeArea(ptMin,ptMax,yMin,yMax,phiMin,phiMax);
    if (!fSigmaSilent) {
      AliInfo(Form("Upsilon 1S prod. cross-section in pp(14 TeV) %5.3g b",sigmaupsilon));
      AliInfo(Form("Upsilon 1S prod. probability per collision in acceptance %5.3g",ratioupsilon));
    }
// second step: generation in selected kinematical range
    genupsilon->SetPtRange(ptMin, ptMax);  
    genupsilon->SetYRange(yMin, yMax);
    genupsilon->SetPhiRange(phiMin, phiMax);
    genupsilon->Init(); // generation in selected kinematical range
    AddGenerator(genupsilon,"Upsilon", ratioupsilon); // Adding Generator
//------------------------------------------------------------------
// Upsilon 2S generator
    AliGenParam * genupsilonP = new AliGenParam(1, AliGenMUONlib::kUpsilonP, "CDF pp", "UpsilonP");
// first step: generation in 4pi
    genupsilonP->SetPtRange(0.,100.);
    genupsilonP->SetYRange(-8.,8.);
    genupsilonP->SetPhiRange(0.,360.);
    genupsilonP->SetForceDecay(fDecayModeResonance);
    if (!gMC) genupsilonP->SetDecayer(fDecayer);  
    genupsilonP->Init();  // generation in 4pi
    ratioupsilonP = sigmaupsilonP / sigmaReaction * genupsilonP->GetRelativeArea(ptMin,ptMax,yMin,yMax,phiMin,phiMax);
    if (!fSigmaSilent) {
      AliInfo(Form("Upsilon 2S prod. cross-section in pp(14 TeV) %5.3g b",sigmaupsilonP));
      AliInfo(Form("Upsilon 2S prod. probability per collision in acceptance %5.3g",ratioupsilonP));
    }
// second step: generation in the kinematical range
    genupsilonP->SetPtRange(ptMin, ptMax);  
    genupsilonP->SetYRange(yMin, yMax);
    genupsilonP->SetPhiRange(phiMin, phiMax);
    genupsilonP->Init(); // generation in selected kinematical range
    AddGenerator(genupsilonP,"UpsilonP", ratioupsilonP); // Adding Generator
    
//------------------------------------------------------------------
// Upsilon 3S generator
    AliGenParam * genupsilonPP = new AliGenParam(1, AliGenMUONlib::kUpsilonPP, "CDF pp", "UpsilonPP");
// first step: generation in 4pi
    genupsilonPP->SetPtRange(0.,100.); 
    genupsilonPP->SetYRange(-8.,8.);
    genupsilonPP->SetPhiRange(0.,360.);
    genupsilonPP->SetForceDecay(fDecayModeResonance);
    if (!gMC) genupsilonPP->SetDecayer(fDecayer);  
    genupsilonPP->Init();  // generation in 4pi
    ratioupsilonPP = sigmaupsilonPP / sigmaReaction * genupsilonPP->GetRelativeArea(ptMin,ptMax,yMin,yMax,phiMin,phiMax);
    if (!fSigmaSilent) {
      AliInfo(Form("Upsilon 3S prod. cross-section in pp(14 TeV) %5.3g b",sigmaupsilonPP));
      AliInfo(Form("Upsilon 3S prod. probability per collision in acceptance %5.3g",ratioupsilonPP));
    }
// second step: generation in selected kinematical range
    genupsilonPP->SetPtRange(ptMin, ptMax);  
    genupsilonPP->SetYRange(yMin, yMax);
    genupsilonPP->SetPhiRange(phiMin, phiMax);
    genupsilonPP->Init(); // generation in selected kinematical range
    AddGenerator(genupsilonPP,"UpsilonPP", ratioupsilonPP); // Adding Generator

//------------------------------------------------------------------
// Generator of charm
    
    AliGenCorrHF *gencharm = new AliGenCorrHF(1, 4);  
    gencharm->SetMomentumRange(0,9999);
    gencharm->SetForceDecay(kAll);
    ratioccbar = sigmaccbar/sigmaReaction;
    if (!gMC) gencharm->SetDecayer(fDecayer);  
    gencharm->Init();
    if (!fSigmaSilent) {
      AliInfo(Form("c-cbar prod. cross-section in pp(14 TeV) %5.3g b",sigmaccbar));
      AliInfo(Form("c-cbar prod. probability per collision in acceptance %5.3g",ratioccbar));
    }
    AddGenerator(gencharm,"CorrHFCharm",ratioccbar);

//------------------------------------------------------------------
// Generator of beauty

    AliGenCorrHF *genbeauty = new AliGenCorrHF(1, 5);  
    genbeauty->SetMomentumRange(0,9999);
    genbeauty->SetForceDecay(kAll);
    ratiobbbar = sigmabbbar/sigmaReaction;
    if (!gMC) genbeauty->SetDecayer(fDecayer);  
    genbeauty->Init();
    if (!fSigmaSilent) {
      AliInfo(Form("b-bbar prod. cross-section in pp(14 TeV) %5.3g b",sigmabbbar));
      AliInfo(Form("b-bbar prod. probability per collision in acceptance %5.3g",ratiobbbar));
    }
    AddGenerator(genbeauty,"CorrHFBeauty",ratiobbbar); 

//-------------------------------------------------------------------
// Pythia generator
//
// This has to go into the Config.C
//
//    AliGenPythia *pythia = new AliGenPythia(1);
//    pythia->SetProcess(kPyMbMSEL1);
//    pythia->SetStrucFunc(kCTEQ5L);
//    pythia->SetEnergyCMS(14000.);
//    AliInfo(Form("\n\npythia uses the decay mode %d", GetDecayModePythia()));
//    Decay_t dt = gener->GetDecayModePythia();
//    pythia->SetForceDecay(dt);
//    pythia->SetPtRange(0.,100.);
//    pythia->SetYRange(-8.,8.);
//    pythia->SetPhiRange(0.,360.);
//    pythia->SetPtHard(2.76,-1.0);
//    pythia->SwitchHFOff();
//    pythia->Init(); 
//    AddGenerator(pythia,"Pythia",1);
}

void AliGenMUONCocktailpp::Init()
{
    //
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
void AliGenMUONCocktailpp::Generate()
{
// Generate event 
    TIter next(fEntries);
    AliGenCocktailEntry *entry = 0;
    AliGenCocktailEntry *preventry = 0;
    AliGenerator* gen = 0;

    const TObjArray *partArray = gAlice->GetMCApp()->Particles();
    
// Generate the vertex position used by all generators    
    if(fVertexSmear == kPerEvent) Vertex();

// Loop on primordialTrigger: 
// minimum muon multiplicity above a pt cut in a theta acceptance region

    Bool_t primordialTrigger = kFALSE;
    while(!primordialTrigger) {
	//Reseting stack
	AliRunLoader * runloader = gAlice->GetRunLoader();
	if (runloader)
	    if (runloader->Stack())
		runloader->Stack()->Clean();
	// Loop over generators and generate events
	Int_t igen = 0;
	Int_t npart = 0;	
	const char* genName = 0;
	while((entry = (AliGenCocktailEntry*)next())) {
	    gen = entry->Generator();
	    genName = entry->GetName();
	    gen->SetVertex(fVertex.At(0), fVertex.At(1), fVertex.At(2));

	    npart = (strcmp(genName,"Pythia") == 0) ? 1 :
		gRandom->Poisson(entry->Rate());

	    if (npart > 0) {
		igen++;	
		if (igen == 1) entry->SetFirst(0);		
		else  entry->SetFirst((partArray->GetEntriesFast())+1);
		
		gen->SetNumberParticles(npart);		
		gen->Generate();
		entry->SetLast(partArray->GetEntriesFast());
		preventry = entry;
	    }
	}  
	next.Reset();

// Testing primordial trigger: Single muons or dimuons with Pt above a Pt cut 
// in the muon spectrometer acceptance
	Int_t iPart;
	fNGenerated++;
	Int_t numberOfMuons=0;Int_t maxPart = partArray->GetEntriesFast();
	for(iPart=0; iPart<maxPart; iPart++){      
	    
	  TParticle *part = gAlice->GetMCApp()->Particle(iPart);
	  if ( TMath::Abs(part->GetPdgCode()) == 13 ){  
	    if((part->Vz() > fMuonOriginCut) && //take only the muons that decayed before the abs + 1 int. length in C abs
	       (part->Theta()*180./TMath::Pi()>fMuonThetaMinCut) &&
	       (part->Theta()*180./TMath::Pi()<fMuonThetaMaxCut) &&
	       (part->Pt()>fMuonPtCut) &&
	       (part->P()>fMuonPCut)) {
	      numberOfMuons++;
	    }
	  }
	}	
	if (numberOfMuons >= fMuonMultiplicity) primordialTrigger = kTRUE;
    }
    fNSucceded++;

//     AliInfo(Form("Generated Events are %d and Succeeded Events are %d",fNGenerated,fNSucceded));
    AliDebug(5,Form("Generated Events are %d and Succeeded Events are %d",fNGenerated,fNSucceded));
}

    

#ifndef ALIEMCALJETFINDEROUTPUT_H
#define ALIEMCALJETFINDEROUTPUT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *  *  * See cxx source for full Copyright notice     */

/* $Id$ */

//_________________________________________________________________________
//  Output object for jetfinder
//
//*-- Author: 	Renan Cabrera (LBL)
//		Mark Horner (LBL/UCT)
//


#include "TObject.h"
#include "TParticle.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "AliEMCALParton.h"
#include "AliEMCALJet.h"
#include "AliEMCALJetFinderTypes.h"

class AliEMCALJetFinderOutput : public TObject
{
		
	public:
		AliEMCALJetFinderOutput();
		~AliEMCALJetFinderOutput();
		void Reset(AliEMCALJetFinderResetType_t resettype);
		void AddJet(AliEMCALJet *jet); 
		void AddParton(AliEMCALParton *parton);
		void AddParticle(TParticle *particle);
		void SetDebug(Int_t debug){fDebug = debug;}
		AliEMCALJet* GetJet(Int_t jetID);
		Int_t GetNJets(){return fNJets;}
		AliEMCALParton* GetParton(Int_t partonID);
		Int_t GetNPartons(){return fNPartons;}
		TParticle* GetParticle(Int_t particleID);
		Int_t GetNParticles(){return fNParticles;}

	private:
		void InitArrays();
		AliEMCALJet	fJetsArray[10];     	// 
		AliEMCALParton	fPartonsArray[4];  	// 
		Int_t		fNPartons;		// Number of Partons actually stored
		Int_t		fNJets; 		// Number of jets actually stored
		TParticle   fParticlesArray[2000];	//
		Int_t		fNParticles;		// Number of particles actually stored
                Int_t           fNMaxJets;      	// Maximum number of jets 
                Int_t           fNMaxParticles; 	// Maximum number of primary particles
                Int_t           fNMaxPartons;   	// Maximum number of primary particles
                Int_t           fDebug;			// Debug level
		Bool_t 		fInitialised;		// stores whether or not the arrays have been initialised
		
	ClassDef(AliEMCALJetFinderOutput,2)
};
#endif

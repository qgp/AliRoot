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
Revision 1.1.1.1  2003/05/29 18:56:53  horner
Initial import - Mark


*/

//_________________________________________________________________________
//  Output object for jetfinder
//
//*-- Author: 	Renan Cabrera (LBL)
//		Mark Horner (LBL/UCT)
//



#include <stdio.h>
#include <TClonesArray.h>
#include <TParticle.h>
#include <TTree.h>

//.....................
#include "AliEMCALJet.h"
#include "AliEMCALParton.h"
#include "AliEMCALJetFinderOutput.h"
#include "AliEMCALJetFinderInput.h"

ClassImp(AliEMCALJetFinderOutput)

//________________________________________________________________________
AliEMCALJetFinderOutput::AliEMCALJetFinderOutput()
{
if (fDebug>0) Info("AliEMCALJetFinderOutput","Beginning Constructor");

        fNMaxJets=10;
        fNMaxParticles=2000;
        fNMaxPartons=4;
	fInitialised = kFALSE;
	InitArrays();
} 
void AliEMCALJetFinderOutput::InitArrays()
{
	
if (fDebug>1) Info("AliEMCALJetFinderOutput","Beginning InitArrays");

	fJetsArray 	= new TClonesArray("AliEMCALJet",fNMaxJets);
	fNJets = 0;
	fPartonsArray 	= new TClonesArray("AliEMCALParton",fNMaxPartons);
	fNPartons = 0;
	fParticlesArray = new TClonesArray("TParticle",fNMaxParticles); 
	fNParticles =0;
	fInitialised = kTRUE;

}

//_______________________________________________________________________
AliEMCALJetFinderOutput::~AliEMCALJetFinderOutput()
{
if (fDebug>0) Info("~AliEMCALJetFinderOutput","Beginning Destructor");
if (fInitialised)
{
 fJetsArray->Delete();
 fPartonsArray->Delete();
 fParticlesArray->Delete();

 delete fJetsArray;
 delete fPartonsArray;
 delete fParticlesArray;
}
}

//_______________________________________________________________________
void AliEMCALJetFinderOutput::Reset(AliEMCALJetFinderResetType_t resettype)
{
if (fDebug>1) Info("Reset","Beginning Reset");
if (!fInitialised) InitArrays();
 if (	resettype == kResetAll ||
	resettype == kResetJets||
	resettype == kResetData ){
	 fJetsArray->Delete();
	 fNJets = 0;
 }
 if (   resettype == kResetAll ||
        resettype == kResetPartons||              
        resettype == kResetData ){
	 fPartonsArray->Delete();
	 fNPartons = 0;
 }
 if (   resettype == kResetAll ||    
        resettype == kResetParticles||              
        resettype == kResetData ){
	 fParticlesArray->Delete();
	 fNParticles = 0;
 }
}
//________________________________________________________________________
void AliEMCALJetFinderOutput::AddJet(AliEMCALJet* jet)
{
if (fDebug>1) Info("AddJet","Beginning AddJet");
if (!fInitialised) InitArrays();

	if (fNJets < fNMaxJets){
		new( (*fJetsArray)[fNJets] )  AliEMCALJet( *jet );
		fNJets++;
	}else
	{
		Error("AddJet","Cannot AddJet - maximum exceeded");
	}
}


//_______________________________________________________________________
void AliEMCALJetFinderOutput::AddParton(AliEMCALParton* parton)
{
if (fDebug>1) Info("AddParton","Beginning AddParton");
if (!fInitialised) InitArrays();

	if (fNPartons < fNMaxPartons){
		new( (*fPartonsArray)[fNPartons] )  AliEMCALParton( *parton );
		fNPartons++;
	}else
	{
                Error("AddParton","Cannot AddParton - maximum exceeded");
	}

}

//_______________________________________________________________________
void AliEMCALJetFinderOutput::AddParticle(TParticle* particle)
{
if (fDebug>1) Info("AddParticle","Beginning AddParticle");
if (!fInitialised) InitArrays();

	if (fNParticles < fNMaxParticles){
		new( (*fParticlesArray)[fNParticles] )  TParticle( *particle );
		fNParticles++;
	}else
	{
		Error("AddParticle","Cannot AddParticle - maximum exceeded");
	}

}

//______________________________________________________________________
AliEMCALJet* AliEMCALJetFinderOutput::GetJet(Int_t jetID)
{
if (fDebug>1) Info("GetJet","Beginning GetJet");
	
  if (jetID >= fNJets) return 0;
  return (AliEMCALJet*)((*fJetsArray)[jetID]);
  

}

//______________________________________________________________________
AliEMCALParton* AliEMCALJetFinderOutput::GetParton(Int_t partonID)
{
if (fDebug>1) Info("GetParton","Beginning GetParton");

  if (partonID >= fNPartons) return 0;
  return (AliEMCALParton*)((*fPartonsArray)[partonID]);


}

//______________________________________________________________________
TParticle* AliEMCALJetFinderOutput::GetParticle(Int_t particleID)
{
if (fDebug>1) Info("GetParticle","Beginning GetParticle");

  if (particleID >= fNParticles) return 0;
  return (TParticle*)((*fParticlesArray)[particleID]);


}


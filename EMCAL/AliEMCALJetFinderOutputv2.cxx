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

*/

//_________________________________________________________________________
//  Output object for jetfinder
//
//*-- Author: 	Renan Cabrera (LBL)
//		Mark Horner (LBL/UCT)
//


#include <stdio.h>
#include <TObjArray.h>
#include <TParticle.h>
#include <TTree.h>

//.....................
#include "AliEMCALJet.h"
#include "AliEMCALParton.h"
#include "AliEMCALJetFinderOutputv2.h"
#include "AliEMCALJetFinderInput.h"

ClassImp(AliEMCALJetFinderOutputv2)

//________________________________________________________________________
AliEMCALJetFinderOutputv2::AliEMCALJetFinderOutputv2(){ 
	fNMaxJets=10;
	fNMaxParticles=2000;
	fNMaxPartons=4;
	fInitialised=kFALSE;
	fNPartons=0;
	fNJets=0;    
	fNParticles=0;
								

if (fDebug>0) Info("AliEMCALJetFinderOutputv2","Beginning Constructor");

} //________________________________________________________________________
void AliEMCALJetFinderOutputv2::InitArrays()
{
	
if (fDebug>1) Info("AliEMCALJetFinderOutputv2","Beginning InitArrays");

}

//_______________________________________________________________________
AliEMCALJetFinderOutputv2::~AliEMCALJetFinderOutputv2()
{
if (fDebug>0) Info("~AliEMCALJetFinderOutputv2","Beginning Destructor");
}

//_______________________________________________________________________
void AliEMCALJetFinderOutputv2::Reset(AliEMCALJetFinderResetType_t resettype)
{
if (fDebug>1) Info("Reset","Beginning Reset");
if (!fInitialised) InitArrays();
 if (	resettype == kResetAll ||
	resettype == kResetJets||
	resettype == kResetData ){
	 fNJets = 0;
 }
 if (   resettype == kResetAll ||
        resettype == kResetPartons||              
        resettype == kResetData ){
	 fNPartons = 0;
 }
 if (   resettype == kResetAll ||    
        resettype == kResetParticles||              
        resettype == kResetData ){
	 fNParticles = 0;
 }
}
//________________________________________________________________________
void AliEMCALJetFinderOutputv2::AddJet(AliEMCALJet* jet)
{
if (fDebug>1) Info("AddJet","Beginning AddJet");
if (!fInitialised) InitArrays();


  	if (fNJets < fNMaxJets){
		new( &fJetsArray[fNJets])   AliEMCALJet( *jet );
		fNJets++;
	}else
	{
		Error("AddJet","Cannot AddJet - maximum exceeded");
                }
   
}


//_______________________________________________________________________
void AliEMCALJetFinderOutputv2::AddParton(AliEMCALParton* parton)
{
if (fDebug>1) Info("AddParton","Beginning AddParton");
if (!fInitialised) InitArrays();

	if (fNPartons < fNMaxPartons){
		new( &fPartonsArray[fNPartons] )  AliEMCALParton( *parton );
		fNPartons++;
	}else
	{
                Error("AddParton","Cannot AddParton - maximum exceeded");
	}
 
}

//_______________________________________________________________________
void AliEMCALJetFinderOutputv2::AddParticle(TParticle* particle)
{
if (fDebug>1) Info("AddParticle","Beginning AddParticle");
if (!fInitialised) InitArrays();

	if (fNParticles < fNMaxParticles){
		new( &fParticlesArray[fNParticles] )  TParticle( *particle );
		fNParticles++;
	}else
	{
		Error("AddParticle","Cannot AddParticle - maximum exceeded");
                }
}

//______________________________________________________________________
AliEMCALJet* AliEMCALJetFinderOutputv2::GetJet(Int_t jetID)
{
if (fDebug>1) Info("GetJet","Beginning GetJet");
	
  if (jetID >= fNJets) return 0;
  return &(fJetsArray[jetID]);
  
}

//______________________________________________________________________
AliEMCALParton* AliEMCALJetFinderOutputv2::GetParton(Int_t partonID)
{
if (fDebug>1) Info("GetParton","Beginning GetParton");

  if (partonID >= fNPartons) return 0;
  return &(fPartonsArray[partonID]);
}

//______________________________________________________________________
TParticle* AliEMCALJetFinderOutputv2::GetParticle(Int_t particleID)
{
if (fDebug>1) Info("GetParticle","Beginning GetParticle");

  if (particleID >= fNParticles) return 0;
return &(fParticlesArray[particleID]);

}


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

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Base class for prompt gamma and correlation analysis
//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---

#include <TParticle.h>
#include <TH2.h>

//---- AliRoot system ---- 
#include "AliAnaGamma.h" 
#include "AliGammaReader.h" 
#include "AliAnaGammaDirect.h" 
#include "AliAnaGammaCorrelation.h" 
#include "AliNeutralMesonSelection.h"
#include "Riostream.h"
#include "AliLog.h"

ClassImp(AliAnaGamma)


//____________________________________________________________________________
  AliAnaGamma::AliAnaGamma() : 
    TObject(),
    fOutputContainer(0x0), 
    fAnaType(0),  fCalorimeter(0), fData(0x0), fKine(0x0), 
    fReader(0x0), fGammaDirect(0x0), fGammaCorrelation(0x0),
    fNeutralMesonSelection(0x0)
{
  //Default Ctor

  //Initialize parameters, pointers and histograms
  if(!fReader)
    fReader = new AliGammaReader();
  if(!fGammaDirect)
    fGammaDirect = new AliAnaGammaDirect();
  if(!fGammaCorrelation)
    fGammaCorrelation = new AliAnaGammaCorrelation();
  if(!fNeutralMesonSelection)
    fNeutralMesonSelection = new AliNeutralMesonSelection();

  InitParameters();
  
}

//____________________________________________________________________________
AliAnaGamma::AliAnaGamma(const AliAnaGamma & g) :   
  TObject(),
  fOutputContainer(g. fOutputContainer), 
  fAnaType(g.fAnaType),  fCalorimeter(g.fCalorimeter), 
  fData(g.fData), fKine(g.fKine),fReader(g.fReader),
  fGammaDirect(g.fGammaDirect), fGammaCorrelation(g.fGammaCorrelation),
  fNeutralMesonSelection(g.fNeutralMesonSelection)
{
  // cpy ctor
  
}

//_________________________________________________________________________
AliAnaGamma & AliAnaGamma::operator = (const AliAnaGamma & source)
{
  // assignment operator

  if(this == &source)return *this;
  ((TObject *)this)->operator=(source);

  fOutputContainer = source.fOutputContainer ;
  fAnaType = source.fAnaType;
  fCalorimeter = source.fCalorimeter ;
  fData = source.fData ; 
  fKine = source.fKine ;
  fReader = source.fReader ;
  fGammaDirect = source.fGammaDirect ;
  fGammaCorrelation = source.fGammaCorrelation ;
  fNeutralMesonSelection = source.fNeutralMesonSelection ;

  return *this;

}

//____________________________________________________________________________
AliAnaGamma::~AliAnaGamma() 
{
  // Remove all pointers

  fOutputContainer->Clear() ; 
  delete fOutputContainer ;

  delete fData ; 
  delete fKine ;
  delete fReader ;
  delete fGammaDirect ;
  delete fGammaCorrelation ;
  delete fNeutralMesonSelection ;

}

//________________________________________________________________________
void AliAnaGamma::Init()
{  

  //Init container histograms and other common variables

  //Histograms
  fOutputContainer = new TList ;
  

  
  //Fill container with appropriate histograms
  if(fAnaType==kIsolationCut)
    fGammaDirect->SetICMethod(AliAnaGammaDirect::kSeveralIC);
  TList * promptcontainer =  fGammaDirect->GetCreateOutputObjects(); 
  for(Int_t i = 0; i < promptcontainer->GetEntries(); i++)
    fOutputContainer->Add(promptcontainer->At(i)) ;
  
  TList * neutralmesoncontainer =  fNeutralMesonSelection->GetCreateOutputObjects();
  if(fNeutralMesonSelection->AreNeutralMesonSelectionHistosKept()){
    for(Int_t i = 0; i < neutralmesoncontainer->GetEntries(); i++)
      fOutputContainer->Add(neutralmesoncontainer->At(i)) ;
  }
  
  if(fAnaType == kCorrelation){
    
    if(fGammaCorrelation->GetCorrelationType() == AliAnaGammaCorrelation::kParton && 
       fReader->GetDataType() != AliGammaReader::kMC)
      AliFatal("kParton must be analyzed with data kMC");
    
    //Set the parameters for the neutral pair selection depending on the analysis
    fNeutralMesonSelection->SetDeltaPhiCutRange(fGammaCorrelation->GetDeltaPhiMinCut(), 
						fGammaCorrelation->GetDeltaPhiMaxCut());  
    if(fGammaCorrelation->GetCorrelationType() == AliAnaGammaCorrelation::kHadron){
      fNeutralMesonSelection->SetPhiPtSelection(AliNeutralMesonSelection::kSelectPhiMinPt);
      fNeutralMesonSelection->SetMinPt(fGammaCorrelation->GetMinPtHadron());
      
    }
    
    if(fGammaCorrelation->GetCorrelationType() == AliAnaGammaCorrelation::kJetLeadCone){
      fNeutralMesonSelection->SetPhiPtSelection(AliNeutralMesonSelection::kSelectPhiPtRatio);
      fNeutralMesonSelection->SetRatioCutRange(fGammaCorrelation->GetRatioMinCut(), 
					       fGammaCorrelation->GetRatioMaxCut());
    }

    TList * correlationcontainer =  fGammaCorrelation->GetCreateOutputObjects();
    for(Int_t i = 0; i < correlationcontainer->GetEntries(); i++)
      fOutputContainer->Add(correlationcontainer->At(i)) ;
    fGammaCorrelation->SetOutputContainer(fOutputContainer);
    fGammaCorrelation->SetNeutralMesonSelection(fNeutralMesonSelection);
  }  
  
}

//____________________________________________________________________________
void AliAnaGamma::InitParameters()
{

  //Init data members
 
  fAnaType = kPrompt;
  fCalorimeter = "EMCAL";

}

//__________________________________________________________________
void AliAnaGamma::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;
  
  Info("Print", "%s %s", GetName(), GetTitle() ) ;
  printf("Analysis type           =     %d\n", fAnaType) ;
  printf("Calorimeter           =     %s\n", fCalorimeter.Data()) ;

  switch(fAnaType)
    {
    case kPrompt:
      {
	fGammaDirect->Print("");
      }// case kIsolationCut
      break;
    case kIsolationCut:
      {
	fGammaDirect->Print("");
      }// case kIsolationCut
      break;
   
    case kCorrelation:
      {
	fGammaCorrelation->Print("");
      }//  case kCorrelation
      break;
      
    }//switch
} 


//____________________________________________________________________________
Bool_t AliAnaGamma::ProcessEvent(Long64_t entry){

  AliDebug(1,Form("Entry %d",entry));

  if(!fOutputContainer)
    AliFatal("Histograms not initialized");

  //CreateTLists with arrays of TParticles. Filled with particles only relevant for the analysis.

  TClonesArray * plCTS      = new TClonesArray("TParticle",1000); // All particles refitted in Central Tracking System (ITS+TPC)
  TClonesArray * plEMCAL    = new TClonesArray("TParticle",1000);   // All particles measured in Jet Calorimeter (EMCAL)
  TClonesArray * plPHOS     = new TClonesArray("TParticle",1000);  // All particles measured  Gamma calorimeter
  TClonesArray * plParton   = new TClonesArray("TParticle",1000);  // All partons
  //Fill lists with photons, neutral particles and charged particles
  //look for the highest energy photon in the event inside fCalorimeter
    
  //Fill particle lists 
  if(fReader->GetDataType() == AliGammaReader::kData){
    AliDebug(1,"Data analysis %s");
    fReader->CreateParticleList(fData, NULL,plCTS,plEMCAL,plPHOS,NULL); 
  }
  else if( fReader->GetDataType()== AliGammaReader::kMC){
    AliDebug(1,"Kinematics analysis");
    fReader->CreateParticleList(fKine, NULL,plCTS,plEMCAL,plPHOS,plParton); 
  }
  else if(fReader->GetDataType() == AliGammaReader::kMCData) {
   AliDebug(1,"Data + Kinematics analysis");
   fReader->CreateParticleList(fData, fKine,plCTS,plEMCAL,plPHOS,NULL); 
  }
  else
    AliError("Option not implemented");
  
  //Search highest energy prompt gamma in calorimeter
  if(fCalorimeter == "PHOS")
    MakeAnalysis(plPHOS, plEMCAL, plCTS, plParton) ; 
  else if (fCalorimeter == "EMCAL")
    MakeAnalysis(plEMCAL, plPHOS, plCTS,plParton) ; 
  else
    AliFatal("Wrong calorimeter name");

  plCTS->Clear() ;
  plEMCAL->Clear() ;
  plPHOS->Clear() ;
  plParton->Clear() ;

  delete plCTS ;
  delete plPHOS ;
  delete plEMCAL ;
  delete plParton ;

  return kTRUE;

}

//____________________________________________________________________________
void AliAnaGamma::MakeAnalysis(TClonesArray * plCalo, TClonesArray * plNe, TClonesArray * plCTS, TClonesArray * plParton)  {
  
  TParticle * pGamma = new TParticle ;
  Bool_t isInCalo = kFALSE ;
  
  switch(fAnaType)
    {
      
    case kPrompt:
      {
	AliDebug(1,"kPrompt analysis");
	fGammaDirect->GetPromptGamma(plCalo, plCTS,pGamma,isInCalo);
	if(!isInCalo)
	  AliDebug(2,"Prompt gamma not found");
      }// case kPromptGamma:
      break;
      
    case kIsolationCut:
      {	
	AliDebug(1,"kIsolationCut analysis");
	fGammaDirect->MakeICAnalysis(plCalo, plCTS);
      }// case kIsolationCut
      break;
      
    case kCorrelation:
      {
	AliDebug(1,"kCorrelation analysis");	
	fGammaDirect->GetPromptGamma(plCalo, plCTS,pGamma,isInCalo);
	if(isInCalo){
	  
	  switch(fGammaCorrelation->GetCorrelationType())
	    {
	    case AliAnaGammaCorrelation::kParton:
	      {
		AliDebug(1,"kParton correlation");
		fGammaCorrelation->MakeGammaCorrelation(pGamma, plParton, NULL);
	      }//  case kParton
	      break;
      
	    case AliAnaGammaCorrelation::kHadron:
	      {
		AliDebug(1,"kHadron correlation");
		fGammaCorrelation->MakeGammaCorrelation(pGamma, plCTS, plNe);
	      }//  case kHadron
	      break;

	    case AliAnaGammaCorrelation::kJetLeadCone:
	      {		
		AliDebug(1,"kJetLeadCone correlation");
		fGammaCorrelation->MakeGammaCorrelation(pGamma, plCTS, plNe);
	      }//  case kJetLeadCone
	      break;
	      
	    case AliAnaGammaCorrelation::kJetFinder:
	      {	
		AliDebug(1,"kJetFinder correlation");
		printf("Analysis not implemented \n");
	      }//  case kJetFinder
	      break;
	    }// switch correlation
	}// is in calo
	else  AliDebug(2,"Prompt gamma not found");
      }// case kCorrelation
      break;

    } //switch(fAnaType)

  delete pGamma ; 
  
}

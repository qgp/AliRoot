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
 * Revision 1.4.4.1  2007/06/18 12:26:49  hristov
 * Updates from the CVS HEAD
 *
 * Revision 1.5  2007/06/18 12:06:03  schutz
 * new methods of AliESDCaloCluster
 *
 * Revision 1.4  2007/03/15 11:47:39  schutz
 * Methods added
 *
 * Revision 1.3  2007/03/08 10:24:32  schutz
 * Coding convention
 *
 * Revision 1.2  2007/02/09 18:40:40  schutz
 * New version from Gustavo
 *
 * Revision 1.1  2007/01/23 17:17:29  schutz
 * New Gamma package
 *
 *
 */

//_________________________________________________________________________
// Class for the analysis of prompt direct photons with the isolation cut analysis
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include <TFile.h>
#include <TParticle.h>
#include <TH2.h>
#include <TChain.h>
#include "AliAnaGammaDirect.h" 
#include "AliESD.h"
#include "AliESDtrack.h"
#include "AliESDCaloCluster.h"
#include "Riostream.h"
#include "AliLog.h"

ClassImp(AliAnaGammaDirect)

//____________________________________________________________________________
  AliAnaGammaDirect::AliAnaGammaDirect(const char *name) : 
    AliAnalysisTask(name,""), fCaloList(0x0),  fCTSList(0x0),
    fOutputContainer(0x0),  fDirectGamma(0x0),
    fMinGammaPt(0.),
    fConeSize(0.),fPtThreshold(0.),fPtSumThreshold(0), 
    fMakeICMethod(0),fhNGamma(0),fhPhiGamma(0),fhEtaGamma(0)
{
  //Ctor
        
  //Initialize parameters
  InitParameters();

  // Input slot #0 works with an Ntuple
  DefineInput(0, TClonesArray::Class());
  DefineInput(1, TClonesArray::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(0,  TParticle::Class()) ; 
  DefineOutput(1,  TList::Class()) ; 
}


//____________________________________________________________________________
AliAnaGammaDirect::AliAnaGammaDirect(const AliAnaGammaDirect & g) : 
  AliAnalysisTask(g),  fCaloList(g.fCaloList),  fCTSList(g.fCTSList),
  fOutputContainer(g.fOutputContainer),  fDirectGamma(g.fDirectGamma),
  fMinGammaPt(g.fMinGammaPt), 
  fConeSize(g.fConeSize),
  fPtThreshold(g.fPtThreshold),
  fPtSumThreshold(g.fPtSumThreshold), 
  fMakeICMethod(g.fMakeICMethod),
  fhNGamma(g.fhNGamma),fhPhiGamma(g.fhPhiGamma),fhEtaGamma(g.fhEtaGamma)
{
  // cpy ctor
  SetName (g.GetName()) ; 
  SetTitle(g.GetTitle()) ; 
}

//_________________________________________________________________________
AliAnaGammaDirect & AliAnaGammaDirect::operator = (const AliAnaGammaDirect & source)
{
  // assignment operator

  if(&source == this) return *this;

  fCaloList = source.fCaloList;
  fCTSList = source.fCTSList;
  fOutputContainer = source.fOutputContainer ;
  fDirectGamma = source.fDirectGamma ;
  fMinGammaPt = source.fMinGammaPt ;   
  fConeSize = source.fConeSize ;
  fPtThreshold = source.fPtThreshold ;
  fPtSumThreshold = source.fPtSumThreshold ; 
  fMakeICMethod = source.fMakeICMethod ;
  fhNGamma = source.fhNGamma ; 
  fhPhiGamma = source.fhPhiGamma ;
  fhEtaGamma = source.fhEtaGamma ;

  return *this;

}

//____________________________________________________________________________
AliAnaGammaDirect::~AliAnaGammaDirect() 
{
  // Remove all pointers
  fOutputContainer->Clear() ; 
  delete fOutputContainer ;

  fCaloList->Clear() ; 
  delete fCaloList ;

  fCTSList->Clear() ; 
  delete fCTSList ;

  delete fDirectGamma ;

  delete fhNGamma    ;  
  delete fhPhiGamma  ; 
  delete fhEtaGamma   ;  

}

//______________________________________________________________________________
void AliAnaGammaDirect::ConnectInputData(const Option_t*)
{
  // Initialisation of branch container and histograms 
    
  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  
  // Get input data
  fCaloList = dynamic_cast<TClonesArray *>(GetInputData(0)) ;
  if (!fCaloList) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }
  
  fCTSList = dynamic_cast<TClonesArray *>(GetInputData(1)) ;
  if (!fCTSList) {
    AliError(Form("Input 1 for %s not found\n", GetName()));
    return ;
  }

}

//________________________________________________________________________
void AliAnaGammaDirect::CreateOutputObjects()
{  

  // Create histograms to be saved in output file and 
  // store them in fOutputContainer
  OpenFile(0) ; 
  fOutputContainer = new TList() ; 
  fOutputContainer->SetName("DirectGammaHistos") ; 
  
  //Histograms of highest gamma identified in Event
  fhNGamma  = new TH1F("NGamma","Number of #gamma over PHOS",240,0,120); 
  fhNGamma->SetYTitle("N");
  fhNGamma->SetXTitle("p_{T #gamma}(GeV/c)");
  fOutputContainer->Add(fhNGamma) ; 
  
  fhPhiGamma  = new TH2F
    ("PhiGamma","#phi_{#gamma}",200,0,120,200,0,7); 
  fhPhiGamma->SetYTitle("#phi");
  fhPhiGamma->SetXTitle("p_{T #gamma} (GeV/c)");
  fOutputContainer->Add(fhPhiGamma) ; 
  
  fhEtaGamma  = new TH2F
    ("EtaGamma","#phi_{#gamma}",200,0,120,200,-0.8,0.8); 
  fhEtaGamma->SetYTitle("#eta");
  fhEtaGamma->SetXTitle("p_{T #gamma} (GeV/c)");
  fOutputContainer->Add(fhEtaGamma) ;
  
  fDirectGamma = new TParticle;

}

//____________________________________________________________________________
void AliAnaGammaDirect::Exec(Option_t *) 
{
  
  // Processing of one event  
  //AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), entry)) ; 
  
  Bool_t iIsInPHOSorEMCAL = kFALSE ; //To check if Gamma was in any calorimeter
  //Search highest energy prompt gamma in calorimeter
  GetPromptGamma(fCaloList,  fCTSList, fDirectGamma, iIsInPHOSorEMCAL) ; 
    
  //If there is any photon candidate in calorimeter
  if(iIsInPHOSorEMCAL)
    AliInfo(Form("Prompt Gamma: pt %f, phi %f, eta %f", fDirectGamma->Pt(),fDirectGamma->Phi(),fDirectGamma->Eta())) ;
    
  PostData(0, fDirectGamma) ; 
  PostData(1, fOutputContainer);

  AliDebug(2, "End of analysis");
}    


//____________________________________________________________________________
void AliAnaGammaDirect::GetPromptGamma(TClonesArray * pl, TClonesArray * plCTS, TParticle *pGamma, Bool_t &Is) const 
{
  //Search for the prompt photon in Calorimeter with pt > fMinGammaPt

  Double_t pt = 0;
  Int_t index = -1; 
  for(Int_t ipr = 0;ipr < pl->GetEntries() ; ipr ++ ){
    TParticle * particle = dynamic_cast<TParticle *>(pl->At(ipr)) ;

    if((particle->Pt() > fMinGammaPt) && (particle->Pt() > pt)){
      index = ipr ;
      pt = particle->Pt();
      pGamma->SetMomentum(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
      AliDebug(4,Form("Cluster in calo: pt %f, phi %f, eta %f", pGamma->Pt(),pGamma->Phi(),pGamma->Eta())) ;
      Is  = kTRUE;
    }
  }

  //Do Isolation?
  if(fMakeICMethod && Is)
    {
      Float_t coneptsum = 0 ;
      Bool_t  icPtThres   = kFALSE;
      Bool_t  icPtSum     = kFALSE;
      MakeIsolationCut(plCTS,pl, pGamma, index, 
		       icPtThres, icPtSum,coneptsum);
      if(fMakeICMethod == 1) //Pt thres method
	Is = icPtThres ;
      if(fMakeICMethod == 2) //Pt cone sum method
	Is = icPtSum ;
    }
  
  if(Is){
    AliDebug(3,Form("Cluster with p_{T} larger than %f found in calorimeter ", fMinGammaPt)) ;
    AliDebug(3,Form("Gamma: pt %f, phi %f, eta %f", pGamma->Pt(),pGamma->Phi(),pGamma->Eta())) ;
    //Fill prompt gamma histograms
    fhNGamma  ->Fill(pGamma->Pt());
    fhPhiGamma->Fill( pGamma->Pt(),pGamma->Phi());
    fhEtaGamma->Fill(pGamma->Pt(),pGamma->Eta());
  }
  else
    AliDebug(1,Form("NO Cluster with pT larger than %f found in calorimeter ", fMinGammaPt)) ;
}

  //____________________________________________________________________________
void AliAnaGammaDirect::InitParameters()
{
 
  //Initialize the parameters of the analysis.
  fMinGammaPt  = 5. ;

  //Fill particle lists when PID is ok
  fConeSize             = 0.2 ; 
  fPtThreshold         = 2.0; 
  fPtSumThreshold  = 1.; 

  fMakeICMethod = 1; // 0 don't isolate, 1 pt thresh method, 2 cone pt sum method
}

//__________________________________________________________________
void  AliAnaGammaDirect::MakeIsolationCut(TClonesArray * plCTS, 
					   TClonesArray * plNe, 
					   TParticle * pCandidate, 
					   Int_t index, 
					   Bool_t  &icmpt,  Bool_t  &icms, 
					   Float_t &coneptsum) const 
{  
  //Search in cone around a candidate particle if it is isolated 
  Float_t phiC  = pCandidate->Phi() ;
  Float_t etaC = pCandidate->Eta() ;
  Float_t pt     = -100. ;
  Float_t eta   = -100.  ;
  Float_t phi    = -100.  ;
  Float_t rad   = -100 ;
  Int_t    n        = 0 ;
  TParticle * particle  = new TParticle;

  coneptsum = 0; 
  icmpt = kFALSE;
  icms  = kFALSE;

  //Check charged particles in cone.
  for(Int_t ipr = 0;ipr < plCTS->GetEntries() ; ipr ++ ){
    particle = dynamic_cast<TParticle *>(plCTS->At(ipr)) ;
    pt    = particle->Pt();
    eta  = particle->Eta();
    phi  = particle->Phi() ;
    
    //Check if there is any particle inside cone with pt larger than  fPtThreshold
    rad = TMath::Sqrt(TMath::Power((eta-etaC),2) +
		      TMath::Power((phi-phiC),2));
    if(rad<fConeSize){
      AliDebug(3,Form("charged in cone pt %f, phi %f, eta %f, R %f ",pt,phi,eta,rad));
      coneptsum+=pt;
      if(pt > fPtThreshold ) n++;
    }
  }// charged particle loop
  
  //Check neutral particles in cone.
  for(Int_t ipr = 0;ipr < plNe->GetEntries() ; ipr ++ ){
    if(ipr != index){//Do not count the candidate
      particle = dynamic_cast<TParticle *>(plNe->At(ipr)) ;
      pt    = particle->Pt();
      eta  = particle->Eta();
      phi  = particle->Phi() ;
      
      //Check if there is any particle inside cone with pt larger than  fPtThreshold
      rad = TMath::Sqrt(TMath::Power((eta-etaC),2) +
			TMath::Power((phi-phiC),2));
      if(rad<fConeSize){
	AliDebug(3,Form("charged in cone pt %f, phi %f, eta %f, R %f ",pt,phi,eta,rad));
	coneptsum+=pt;
	if(pt > fPtThreshold ) n++;
      }
    }
  }// neutral particle loop
  
  if(n == 0) 
    icmpt =  kTRUE ;
  if(coneptsum < fPtSumThreshold)
    icms  =  kTRUE ;

}

void AliAnaGammaDirect::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  Info("Print", "%s %s", GetName(), GetTitle() ) ;
  printf("IC method               =     %d\n", fMakeICMethod) ; 
  printf("Cone Size               =     %f\n", fConeSize) ; 
  printf("pT threshold           =     %f\n", fPtThreshold) ;
  printf("pT sum threshold   =     %f\n", fPtSumThreshold) ; 
  printf("Min Gamma pT      =     %f\n",  fMinGammaPt) ; 
} 

void AliAnaGammaDirect::Terminate(Option_t *)
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
    

}

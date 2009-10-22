/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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

/********************************** 
 * flow analysis with Q-cumulants * 
 *                                * 
 * author:  Ante Bilandzic        * 
 *           (anteb@nikhef.nl)    *
 *********************************/ 

#define AliFlowAnalysisWithQCumulants_cxx

#include "Riostream.h"
#include "AliFlowCommonConstants.h"
#include "AliFlowCommonHist.h"
#include "AliFlowCommonHistResults.h"
#include "TChain.h"
#include "TFile.h"
#include "TList.h"
#include "TGraph.h"
#include "TParticle.h"
#include "TRandom3.h"
#include "TStyle.h"
#include "TProfile.h"
#include "TProfile2D.h" 
#include "TProfile3D.h"
#include "TMath.h"
#include "TArrow.h"
#include "TPaveLabel.h"
#include "TCanvas.h"
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "AliFlowAnalysisWithQCumulants.h"
#include "TArrayD.h"
#include "TRandom.h"
#include "TF1.h"

class TH1;
class TH2;
class TGraph;
class TPave;
class TLatex;
class TMarker;
class TRandom3;
class TObjArray;
class TList;
class TCanvas;
class TSystem;
class TROOT;
class AliFlowVector;
class TVector;


//================================================================================================================


ClassImp(AliFlowAnalysisWithQCumulants)

AliFlowAnalysisWithQCumulants::AliFlowAnalysisWithQCumulants(): 
 // 0.) base:
 fHistList(NULL),
 // 1.) common:
 fCommonHists(NULL),
 fCommonHists2nd(NULL), 
 fCommonHists4th(NULL),
 fCommonHists6th(NULL),
 fCommonHists8th(NULL),
 fCommonHistsResults2nd(NULL),
 fCommonHistsResults4th(NULL),
 fCommonHistsResults6th(NULL),
 fCommonHistsResults8th(NULL),
 fnBinsPhi(0),
 fPhiMin(0),
 fPhiMax(0),
 fPhiBinWidth(0),
 fnBinsPt(0),
 fPtMin(0),
 fPtMax(0),
 fPtBinWidth(0),
 fnBinsEta(0),
 fEtaMin(0),
 fEtaMax(0),
 fEtaBinWidth(0),
 fHarmonic(2),
 fAnalysisLabel(NULL),
 // 2.) weights:
 fWeightsList(NULL),
 fUsePhiWeights(kFALSE),
 fUsePtWeights(kFALSE),
 fUseEtaWeights(kFALSE),
 fUseParticleWeights(NULL),
 fPhiWeights(NULL),
 fPtWeights(NULL),
 fEtaWeights(NULL),
 // 3.) integrated flow:
 fIntFlowList(NULL), 
 fIntFlowProfiles(NULL),
 fIntFlowResults(NULL),
 fIntFlowFlags(NULL),
 fApplyCorrectionForNUA(kTRUE), 
 fReQ(NULL),
 fImQ(NULL),
 fSMpk(NULL),
 fIntFlowCorrelationsEBE(NULL),
 fIntFlowCorrelationsAllEBE(NULL),
 fAvMultiplicity(NULL),
 fIntFlowCorrelationsPro(NULL),
 fIntFlowCorrelationsAllPro(NULL),
 fIntFlowProductOfCorrelationsPro(NULL),
 fIntFlowCorrelationsHist(NULL),
 fIntFlowCorrelationsAllHist(NULL),
 fIntFlowCovariances(NULL),
 fIntFlowSumOfProductOfEventWeights(NULL),
 fIntFlowQcumulants(NULL),
 fIntFlow(NULL),
 // 4.) differential flow:
 fDiffFlowList(NULL),
 fDiffFlowProfiles(NULL),
 fDiffFlowResults(NULL),
 fDiffFlowFlags(NULL),
 fCalculate2DFlow(kFALSE),
 // 5.) distributions:
 fDistributionsList(NULL),
 // x.) debugging and cross-checking:
 fNestedLoopsList(NULL),
 fEvaluateNestedLoopsForIntFlow(kFALSE),
 fEvaluateNestedLoopsForDiffFlow(kFALSE),  
 fEvaluateNestedLoops(NULL),
 fDirectCorrelations(NULL),
 fDirectCorrectionsCos(NULL),
 fDirectCorrectionsSin(NULL),
 fDirectCorrelationsDiffFlow(NULL),
 fDirectCorrectionsDiffFlowCos(NULL),
 fDirectCorrectionsDiffFlowSin(NULL),
 fDirectCorrelationsW(NULL),
 fDirectCorrectionsCosW(NULL),
 fDirectCorrectionsSinW(NULL),
 fDirectCorrelationsDiffFlowW(NULL),
 fDirectCorrectionsDiffFlowCosW(NULL),
 fDirectCorrectionsDiffFlowSinW(NULL)
 {
  // constructor  
  
  // base list to hold all output objects:
  fHistList = new TList();
  fHistList->SetName("cobjQC");
  fHistList->SetOwner(kTRUE);
  
  // list to hold histograms with phi, pt and eta weights:      
  fWeightsList = new TList();
    
  // analysis label;
  fAnalysisLabel = new TString();
      
  // initialize all arrays:  
  this->InitializeArraysForIntFlow();
  this->InitializeArraysForDiffFlow();
  this->InitializeArraysForDistributions();
  
 } // end of constructor
 

//================================================================================================================  


AliFlowAnalysisWithQCumulants::~AliFlowAnalysisWithQCumulants()
{
 // destructor
 
 delete fHistList;

} // end of AliFlowAnalysisWithQCumulants::~AliFlowAnalysisWithQCumulants()


//================================================================================================================


void AliFlowAnalysisWithQCumulants::Init()
{
 // a) Access all common constants;
 // b) Book all objects;
 // c) Store flags for integrated and differential flow;
 // d) Store harmonic which will be estimated.
  
 // a) Access all common constants:
 this->AccessConstants();
 
 // b) Book all objects:
 this->BookAndFillWeightsHistograms();
 this->BookAndNestAllLists();
 this->BookCommonHistograms();
 this->BookEverythingForIntegratedFlow(); 
 this->BookEverythingForDifferentialFlow(); 
 this->BookEverythingForDistributions();
 this->BookEverythingForNestedLoops();
 
 // c) Store flags for integrated and differential flow:
 this->StoreIntFlowFlags();
 this->StoreDiffFlowFlags();

 // d) Store harmonic which will be estimated:
 this->StoreHarmonic();
 
} // end of void AliFlowAnalysisWithQCumulants::Init()


//================================================================================================================


void AliFlowAnalysisWithQCumulants::Make(AliFlowEventSimple* anEvent)
{
 // running over data only in this method
 
 // a) fill the common control histograms and call method to fill fAvMultiplicity;
 // b) loop over data to calculate e-b-e quantities;
 // c) call the methods;
 // d) debugging and cross-checking (evaluate nested loops);
 // e) reset e-b-e quantities.
 
 Double_t dPhi = 0.; // azimuthal angle in the laboratory frame
 Double_t dPt  = 0.; // transverse momentum
 Double_t dEta = 0.; // pseudorapidity

 Double_t wPhi = 1.; // phi weight
 Double_t wPt  = 1.; // pt weight
 Double_t wEta = 1.; // eta weight
 
 // ********************************************
 // **** FILL THE COMMON CONTROL HISTOGRAMS ****
 // ********************************************
                                         
 Int_t nRP = anEvent->GetEventNSelTracksRP(); // number of RPs (i.e. number of particles used to determine the reaction plane)
 
 fCommonHists->FillControlHistograms(anEvent); 
 
 if(nRP>1)
 {
  fCommonHists2nd->FillControlHistograms(anEvent);                                        
  if(nRP>3)
  {
   fCommonHists4th->FillControlHistograms(anEvent);                                        
   if(nRP>5)
   {
    fCommonHists6th->FillControlHistograms(anEvent);                                        
    if(nRP>7)
    {
     fCommonHists8th->FillControlHistograms(anEvent);                                        
    } // end of if(nRP>7)  
   } // end of if(nRP>5) 
  } // end of if(nRP>3)                                                                                                                      
 } // end of if(nRP>1) 
                                                                        
 this->FillAverageMultiplicities(nRP);                                                                  
                                                                                                                                                                                                                                                                                        
 // *******************************************************
 // **** LOOP OVER DATA AND CALCULATE E-B-E QUANTITIES ****
 // *******************************************************
 
 Int_t nPrim = anEvent->NumberOfTracks();  // nPrim = total number of primary tracks, i.e. nPrim = nRP + nPOI + rest, where:
                                           // nRP   = # of particles used to determine the reaction plane;
                                           // nPOI  = # of particles of interest for a detailed flow analysis;
                                           // rest  = # of particles which are not niether RPs nor POIs.  
 
 AliFlowTrackSimple *aftsTrack = NULL;
 
 for(Int_t i=0;i<nPrim;i++) 
 { 
  aftsTrack=anEvent->GetTrack(i);
  if(aftsTrack)
  {
   if(!(aftsTrack->InRPSelection() || aftsTrack->InPOISelection())) continue; // consider only tracks which are RPs or POIs
   Int_t n = fHarmonic; // shortcut for the harmonic
   if(aftsTrack->InRPSelection()) // RP condition:
   {    
    dPhi = aftsTrack->Phi();
    dPt  = aftsTrack->Pt();
    dEta = aftsTrack->Eta();
    if(fUsePhiWeights && fPhiWeights && fnBinsPhi) // determine phi weight for this particle:
    {
     wPhi = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(dPhi*fnBinsPhi/TMath::TwoPi())));
    }
    if(fUsePtWeights && fPtWeights && fnBinsPt) // determine pt weight for this particle:
    {
     wPt = fPtWeights->GetBinContent(1+(Int_t)(TMath::Floor((dPt-fPtMin)/fPtBinWidth))); 
    }              
    if(fUseEtaWeights && fEtaWeights && fEtaBinWidth) // determine eta weight for this particle: 
    {
     wEta = fEtaWeights->GetBinContent(1+(Int_t)(TMath::Floor((dEta-fEtaMin)/fEtaBinWidth))); 
    } 
      
    // integrated flow: 
    // calculate Re[Q_{m*n,k}] and Im[Q_{m*n,k}], m = 1,2,3,4, for this event:
    for(Int_t m=0;m<4;m++)
    {
     for(Int_t k=0;k<9;k++)
     {
      (*fReQ)(m,k)+=pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1)*n*dPhi); 
      (*fImQ)(m,k)+=pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1)*n*dPhi); 
     } 
    }
    // calculate S^{M}_{p,k} for this event 
    // Remark: final calculation of S^{M}_{p,k} follows after the loop over data bellow:
    for(Int_t p=0;p<8;p++)
    {
     for(Int_t k=0;k<9;k++)
     {     
      (*fSMpk)(p,k)+=pow(wPhi*wPt*wEta,k);
     }
    } 
    
    // differential flow:
    // 1D (pt):
    // (r_{m*m,k}(pt)): 
    for(Int_t m=0;m<4;m++)
    {
     for(Int_t k=0;k<9;k++)
     {
      fReRPQ1dEBE[0][0][m][k]->Fill(dPt,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
      fImRPQ1dEBE[0][0][m][k]->Fill(dPt,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
     }
    }
           
    // s_{k}(pt) for RPs // to be improved (clarified)
    // Remark: final calculation of s_{p,k}(pt) follows after the loop over data bellow:
    for(Int_t k=0;k<9;k++)
    {
     fs1dEBE[0][0][k]->Fill(dPt,pow(wPhi*wPt*wEta,k),1.);
    }
    // 1D (eta):
    // (r_{m*m,k}(eta)): 
    for(Int_t m=0;m<4;m++)
    {
     for(Int_t k=0;k<9;k++)
     {
      fReRPQ1dEBE[0][1][m][k]->Fill(dEta,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
      fImRPQ1dEBE[0][1][m][k]->Fill(dEta,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
     }
    }   
    // s_{k}(eta) for RPs // to be improved (clarified)
    // Remark: final calculation of s_{p,k}(eta) follows after the loop over data bellow:
    for(Int_t k=0;k<9;k++)
    {
     fs1dEBE[0][1][k]->Fill(dEta,pow(wPhi*wPt*wEta,k),1.);
    }
    
    
    
    /*
    // 2D (pt,eta):
    if(fCalculate2DFlow)
    {
     // (r_{m*m,k}(pt,eta)): 
     for(Int_t m=0;m<4;m++)
     {
      for(Int_t k=0;k<9;k++)
      {
       fReRPQ2dEBE[0][m][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
       fImRPQ2dEBE[0][m][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
      }
     }    
     // s_{k}(pt,eta) for RPs // to be improved (clarified)
     // Remark: final calculation of s_{p,k}(pt,eta) follows after the loop over data bellow:
     for(Int_t k=0;k<9;k++)
     {
      fs2dEBE[0][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k),1.);
     }
    } // end of if(fCalculate2DFlow)  
    */ 
    
      
     
    if(aftsTrack->InPOISelection())
    {
     // 1D (pt): 
     // (q_{m*m,k}(pt)): 
     for(Int_t m=0;m<4;m++)
     {
      for(Int_t k=0;k<9;k++)
      {
       fReRPQ1dEBE[2][0][m][k]->Fill(dPt,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
       fImRPQ1dEBE[2][0][m][k]->Fill(dPt,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
      }
     } 
     // s_{k}(pt) for RP&&POIs // to be improved (clarified)
     // Remark: final calculation of s_{p,k}(pt,eta) follows after the loop over data bellow:
     for(Int_t k=0;k<9;k++)
     {
      fs1dEBE[2][0][k]->Fill(dPt,pow(wPhi*wPt*wEta,k),1.);
     }
     // 1D (eta): 
     // (q_{m*m,k}(eta)): 
     for(Int_t m=0;m<4;m++)
     {
      for(Int_t k=0;k<9;k++)
      {
       fReRPQ1dEBE[2][1][m][k]->Fill(dEta,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
       fImRPQ1dEBE[2][1][m][k]->Fill(dEta,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
      }
     } 
     // s_{k}(eta) for RP&&POIs // to be improved (clarified)
     // Remark: final calculation of s_{p,k}(pt,eta) follows after the loop over data bellow:
     for(Int_t k=0;k<9;k++)
     {
      fs1dEBE[2][1][k]->Fill(dEta,pow(wPhi*wPt*wEta,k),1.);
     }
     
     /*
     // 2D (pt,eta) 
     if(fCalculate2DFlow)
     {
      // (q_{m*m,k}(pt,eta)): 
      for(Int_t m=0;m<4;m++)
      {
       for(Int_t k=0;k<9;k++)
       {
        fReRPQ2dEBE[2][m][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k)*TMath::Cos((m+1.)*n*dPhi),1.);
        fImRPQ2dEBE[2][m][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k)*TMath::Sin((m+1.)*n*dPhi),1.);
       }
      } 
      // s_{k}(pt,eta) for RP&&POIs // to be improved (clarified)
      // Remark: final calculation of s_{p,k}(pt,eta) follows after the loop over data bellow:
      for(Int_t k=0;k<9;k++)
      {
       fs2dEBE[2][k]->Fill(dPt,dEta,pow(wPhi*wPt*wEta,k),1.);
      }
     } // end of if(fCalculate2DFlow) 
     */
      
    } // end of if(aftsTrack->InPOISelection())
    

     
   } // end of if(pTrack->InRPSelection())

  
  
   if(aftsTrack->InPOISelection())
   {
    dPhi = aftsTrack->Phi();
    dPt  = aftsTrack->Pt();
    dEta = aftsTrack->Eta();
    
    // 1D (pt)
    // p_n(m*n,0):   
    for(Int_t m=0;m<4;m++)
    {
     fReRPQ1dEBE[1][0][m][0]->Fill(dPt,TMath::Cos((m+1.)*n*dPhi),1.);
     fImRPQ1dEBE[1][0][m][0]->Fill(dPt,TMath::Sin((m+1.)*n*dPhi),1.);
    }
    // 1D (eta)
    // p_n(m*n,0):   
    for(Int_t m=0;m<4;m++)
    {
     fReRPQ1dEBE[1][1][m][0]->Fill(dEta,TMath::Cos((m+1.)*n*dPhi),1.);
     fImRPQ1dEBE[1][1][m][0]->Fill(dEta,TMath::Sin((m+1.)*n*dPhi),1.);
    }
    
    
    /*
    // 2D (pt,eta):
    if(fCalculate2DFlow)
    {      
     // p_n(m*n,0):   
     for(Int_t m=0;m<4;m++)
     {
      fReRPQ2dEBE[1][m][0]->Fill(dPt,dEta,TMath::Cos((m+1.)*n*dPhi),1.);
      fImRPQ2dEBE[1][m][0]->Fill(dPt,dEta,TMath::Sin((m+1.)*n*dPhi),1.);
     }
    } // end of if(fCalculate2DFlow)  
    */
    
    
   } // end of if(pTrack->InPOISelection() )   
 
  
  } else // to if(aftsTrack)
    {
     cout<<endl;
     cout<<" WARNING: no particle! (i.e. aftsTrack is a NULL pointer in AFAWQC::Make().)"<<endl;
     cout<<endl;       
    }
 } // end of for(Int_t i=0;i<nPrim;i++) 

 // calculate the final expressions for S^{M}_{p,k}:
 for(Int_t p=0;p<8;p++)
 {
  for(Int_t k=0;k<9;k++)
  {
   (*fSMpk)(p,k)=pow((*fSMpk)(p,k),p+1);
  }  
 } 
 
 // *****************************
 // **** CALL THE METHODS *******
 // *****************************
 // integrated flow:
 if(!fEvaluateNestedLoopsForIntFlow)
 {
  // without weights:
  if(nRP>1) this->CalculateIntFlowCorrelations();
  if(nRP>3) this->CalculateIntFlowProductOfCorrelations();
  if(nRP>1) this->CalculateIntFlowSumOfEventWeights();
  if(nRP>1) this->CalculateIntFlowSumOfProductOfEventWeights();
  if(fApplyCorrectionForNUA)
  {
   if(nRP>0) this->CalculateIntFlowCorrectionsForNUASinTerms();
   if(nRP>0) this->CalculateIntFlowCorrectionsForNUACosTerms();
  }
  // with weights:
  if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
  {
   //if(nRP>1) this->CalculateWeightedCorrelationsForIntegratedFlow();
   //if(nRP>3) this->CalculateWeightedQProductsForIntFlow();
  } 
 }

 // differential flow:
 if(!fEvaluateNestedLoopsForDiffFlow)
 {
  // 1D differential flow (without usage of particle weights):
  if(nRP>1)
  {
   this->CalculateReducedCorrelations1D("RP","Pt");
   this->CalculateReducedCorrelations1D("RP","Eta");
   this->CalculateReducedCorrelations1D("POI","Pt");
   this->CalculateReducedCorrelations1D("POI","Eta");
   this->CalculateDiffFlowProductOfCorrelations("RP","Pt");
   this->CalculateDiffFlowProductOfCorrelations("RP","Eta");
   this->CalculateDiffFlowProductOfCorrelations("POI","Pt");
   this->CalculateDiffFlowProductOfCorrelations("POI","Eta");
   this->CalculateDiffFlowSumOfEventWeights("RP","Pt");
   this->CalculateDiffFlowSumOfEventWeights("RP","Eta");
   this->CalculateDiffFlowSumOfEventWeights("POI","Pt");
   this->CalculateDiffFlowSumOfEventWeights("POI","Eta");
   this->CalculateDiffFlowSumOfProductOfEventWeights("RP","Pt");
   this->CalculateDiffFlowSumOfProductOfEventWeights("RP","Eta");
   this->CalculateDiffFlowSumOfProductOfEventWeights("POI","Pt");
   this->CalculateDiffFlowSumOfProductOfEventWeights("POI","Eta");
  }
  
  // with weights:
  // ... 
  // 1D differential flow (eta)  
  if(nRP>1)
  {
   /*
   this->CalculateReducedCorrelations1D("RP","Eta");
   this->CalculateReducedCorrelations1D("POI","Eta");
   this->CalculateQProductsForDiffFlow("RP","Eta");
   this->CalculateQProductsForDiffFlow("POI","Eta");
   this->CalculateSumOfEventWeightsForDiffFlow("RP","Eta");
   this->CalculateProductOfEventWeightsForDiffFlow("RP","Eta");
   this->CalculateSumOfEventWeightsForDiffFlow("POI","Eta");
   this->CalculateProductOfEventWeightsForDiffFlow("POI","Eta");
   */
  }
  
  /*
  // 2D differential flow
  if(fCalculate2DFlow)
  {
   // without weights:
   if(nRP>1) this->CalculateCorrelationsForDifferentialFlow2D("RP");
   if(nRP>1) this->CalculateCorrelationsForDifferentialFlow2D("POI");
  
   // with weights:
   if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
   {
    if(nRP>1) this->CalculateWeightedCorrelationsForDifferentialFlow2D("RP");
    if(nRP>1) this->CalculateWeightedCorrelationsForDifferentialFlow2D("POI");
   } 
  } // end of if(fCalculate2DFlow)
  */
  
 }
 
 // **************************************************************
 // **** DEBUGGING AND CROSS-CHECKING (EVALUATE NESTED LOOPS) ****
 // **************************************************************

 if(fEvaluateNestedLoopsForIntFlow)
 {
  if(nPrim>0 && nPrim<15) // only for these multiplicities it is feasible to evaluate 8 nested loops in short time 
  {
   // without weights:
   if(!(fUsePhiWeights||fUsePtWeights||fUseEtaWeights))
   {
    this->CalculateIntFlowCorrelations();
    this->CalculateIntFlowCorrectionsForNUASinTerms();
    this->CalculateIntFlowCorrectionsForNUACosTerms();
   }
   // with weights:
   if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
   {
    this->CalculateWeightedCorrelationsForIntegratedFlow();
   }
    
   this->EvaluateNestedLoopsForIntegratedFlow(anEvent);  
  }
 } 
 
 if(fEvaluateNestedLoopsForDiffFlow)
 {
  if(nPrim>0 && nPrim<15) // only for these multiplicities it is feasible to evaluate 8 nested loops in short time 
  {
   // without weights:
   if(!(fUsePhiWeights||fUsePtWeights||fUseEtaWeights))
   {
    this->CalculateCorrelationsForDifferentialFlow2D("RP");
    this->CalculateCorrelationsForDifferentialFlow2D("POI");
   }
   // with weights:
   if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
   {
    this->CalculateWeightedCorrelationsForDifferentialFlow2D("RP");
    this->CalculateWeightedCorrelationsForDifferentialFlow2D("POI");
   }
    
   this->EvaluateNestedLoopsForDifferentialFlow(anEvent);  
  }
 } 
 
 // ********************************
 // **** RESET E-B-E QUANTITIES ****
 // ********************************
 
 // integrated flow:
 fReQ->Zero();
 fImQ->Zero();
 fSMpk->Zero();
 fIntFlowCorrelationsEBE->Reset();
 fIntFlowCorrelationsAllEBE->Reset();
 
 if(fApplyCorrectionForNUA)  
 {
  for(Int_t sc=0;sc<2;sc++)
  {
   fIntFlowCorrectionTermsForNUAEBE[sc]->Reset();
  } 
 }
    
 // differential flow:
 // 1D:
 for(Int_t t=0;t<3;t++) // type (RP, POI, POI&&RP)
 {
  for(Int_t pe=0;pe<2;pe++) // 1D in pt or eta
  {
   for(Int_t m=0;m<4;m++) // multiple of harmonic
   {
    for(Int_t k=0;k<9;k++) // power of weight
    {
     if(fReRPQ1dEBE[t][pe][m][k]) fReRPQ1dEBE[t][pe][m][k]->Reset();
     if(fImRPQ1dEBE[t][pe][m][k]) fImRPQ1dEBE[t][pe][m][k]->Reset();
    }   
   }
  }
 }
  
 for(Int_t t=0;t<3;t++) // type (0 = RP, 1 = POI, 2 = RP&&POI )
 { 
  for(Int_t pe=0;pe<2;pe++) // 1D in pt or eta
  {
   for(Int_t k=0;k<9;k++)
   {
    if(fs1dEBE[t][pe][k]) fs1dEBE[t][pe][k]->Reset();
   }
  }
 }

 // e-b-e reduced correlations:
 for(Int_t t=0;t<2;t++) // type (0 = RP, 1 = POI)
 {  
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t rci=0;rci<4;rci++) // reduced correlation index
   {
    if(fDiffFlowCorrelationsEBE[t][pe][rci]) fDiffFlowCorrelationsEBE[t][pe][rci]->Reset();
   }
  }
 }
    
 // 2D (pt,eta)
 if(fCalculate2DFlow)
 {
  for(Int_t t=0;t<3;t++) // type (RP, POI, POI&&RP)
  {
   for(Int_t m=0;m<4;m++) // multiple of harmonic
   {
    for(Int_t k=0;k<9;k++) // power of weight
    {
     if(fReRPQ2dEBE[t][m][k]) fReRPQ2dEBE[t][m][k]->Reset();
     if(fImRPQ2dEBE[t][m][k]) fImRPQ2dEBE[t][m][k]->Reset();
    }   
   }
  }
  for(Int_t t=0;t<3;t++) // type (0 = RP, 1 = POI, 2 = RP&&POI )
  { 
   for(Int_t k=0;k<9;k++)
   {
    if(fs2dEBE[t][k]) fs2dEBE[t][k]->Reset();
   }
  }  
 } // end of if(fCalculate2DFlow) 
 
} // end of AliFlowAnalysisWithQCumulants::Make(AliFlowEventSimple* anEvent)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::Finish()
{
 // Calculate the final results.
 //  a) acces the constants;
 //  b) access the flags;
 //  c) calculate the final results for integrated flow (without and with weights);
 //  d) store in AliFlowCommonHistResults and print the final results for integrated flow;
 //  e) calculate the final results for differential flow (without and with weights);
 //  f) print the final results for integrated flow obtained from differential flow (to be improved (terminology));
 //  g) COMPARE RESULTS FROM NESTED LOOPS vs RESULTS FROM Q-VECTORS FOR INTEGRATED FLOW
 
 // ******************************
 // **** ACCESS THE CONSTANTS ****
 // ******************************
 
 this->AccessConstants();          
 
 if(fCommonHists && fCommonHists->GetHarmonic())
 {
  fHarmonic = (Int_t)(fCommonHists->GetHarmonic())->GetBinContent(1); // to be improved (moved somewhere else)
 } 

 // **************************
 // **** ACCESS THE FLAGS ****
 // **************************    
 fUsePhiWeights = (Int_t)fUseParticleWeights->GetBinContent(1); 
 fUsePtWeights = (Int_t)fUseParticleWeights->GetBinContent(2); 
 fUseEtaWeights = (Int_t)fUseParticleWeights->GetBinContent(3);  
 fApplyCorrectionForNUA = (Int_t)fIntFlowFlags->GetBinContent(3); 
 fEvaluateNestedLoopsForIntFlow = (Int_t)fEvaluateNestedLoops->GetBinContent(1);
 fEvaluateNestedLoopsForDiffFlow = (Int_t)fEvaluateNestedLoops->GetBinContent(2); 
    
 // *********************************************************
 // **** CALCULATE THE FINAL RESULTS FOR INTEGRATED FLOW ****
 // *********************************************************    
 
 // without weights:
 this->FinalizeCorrelationsIntFlow();
 this->CalculateCovariancesIntFlow();
 this->CalculateCumulantsIntFlow();
 this->CalculateIntFlow(); 

 if(fApplyCorrectionForNUA) // to be improved (reorganized)
 {
  this->FinalizeCorrectionTermsForNUAIntFlow();
  this->CalculateQcumulantsCorrectedForNUAIntFlow();   
  this->CalculateIntFlowCorrectedForNUA(); 
 }
 
 //this->ApplyCorrectionForNonUniformAcceptanceToCumulantsForIntFlow(kFALSE,"exact");
 
 // with weights:
 if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
 {
  //this->FinalizeCorrelationsIntFlow(); 
  // this->CalculateCorrectionsForNUAForIntQcumulants(kTRUE,"exact");
  //this->CalculateCovariancesIntFlow(kTRUE,"exact");
  //this->CalculateCumulantsIntFlow();
  // this->ApplyCorrectionForNonUniformAcceptanceToCumulantsForIntFlow(kTRUE,"exact");
  //this->CalculateIntFlow(); 
  // this->CalculateIntFlow(kTRUE,"exact",kTRUE); // weighted and corrected for non-uniform acceptance
 }
 
 // ***************************************************************
 // **** STORE AND PRINT THE FINAL RESULTS FOR INTEGRATED FLOW ****
 // ***************************************************************
 
 this->FillCommonHistResultsIntFlow();  
  
 this->PrintFinalResultsForIntegratedFlow("NONAME"); // to be improved (name)
 
 // ***********************************************************
 // **** CALCULATE THE FINAL RESULTS FOR DIFFERENTIAL FLOW ****
 // ***********************************************************    
 
 // without weights:
 this->FinalizeReducedCorrelations("RP","Pt"); 
 this->FinalizeReducedCorrelations("RP","Eta"); 
 this->FinalizeReducedCorrelations("POI","Pt"); 
 this->FinalizeReducedCorrelations("POI","Eta");  
 this->CalculateDiffFlowCovariances("RP","Pt");
 this->CalculateDiffFlowCovariances("RP","Eta");
 this->CalculateDiffFlowCovariances("POI","Pt");
 this->CalculateDiffFlowCovariances("POI","Eta");
 this->CalculateDiffFlowCumulants("RP","Pt");
 this->CalculateDiffFlowCumulants("RP","Eta");
 this->CalculateDiffFlowCumulants("POI","Pt");
 this->CalculateDiffFlowCumulants("POI","Eta");
 this->CalculateDiffFlow("RP","Pt");
 this->CalculateDiffFlow("RP","Eta");
 this->CalculateDiffFlow("POI","Pt");
 this->CalculateDiffFlow("POI","Eta");
 this->CalculateFinalResultsForRPandPOIIntegratedFlow("RP");
 this->CalculateFinalResultsForRPandPOIIntegratedFlow("POI");
 //this->FinalizeCorrelationsForDiffFlow("RP",kFALSE,"exact");
 //this->FinalizeCorrelationsForDiffFlow("POI",kFALSE,"exact");
 
 // with weights:
 if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
 {
  //this->FinalizeCorrelationsForDiffFlow("RP",kTRUE,"exact");
  //this->FinalizeCorrelationsForDiffFlow("POI",kTRUE,"exact");
  //this->CalculateDiffFlowCumulants("RP",kTRUE,"exact");
  //this->CalculateDiffFlowCumulants("POI",kTRUE,"exact");
  //this->CalculateDiffFlow("RP",kTRUE,"exact");
  //this->CalculateDiffFlow("POI",kTRUE,"exact");
  //this->CalculateFinalResultsForRPandPOIIntegratedFlow("RP",kTRUE,"exact");
  //this->CalculateFinalResultsForRPandPOIIntegratedFlow("POI",kTRUE,"exact");
 }
 
   
  //this->CalculateFinalCorrectionsForNonUniformAcceptanceForDifferentialFlow(kFALSE,"POI"); // to be improved (to calculate also when weights are used) 
  //this->CalculateFinalCorrectionsForNonUniformAcceptanceForDifferentialFlow(kFALSE,"RP"); // to be improved (to calculate also when weights are used)

 
 // *****************************************************************
 // **** STORE AND PRINT THE FINAL RESULTS FOR DIFFERENTIAL FLOW ****
 // *****************************************************************
 if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights)
 {
  //this->FillCommonHistResultsDiffFlow("RP",kTRUE,"exact",kFALSE);
  //this->FillCommonHistResultsDiffFlow("POI",kTRUE,"exact",kFALSE);
 } else
   {
    this->FillCommonHistResultsDiffFlow("RP");
    this->FillCommonHistResultsDiffFlow("POI");
   }
 
 this->PrintFinalResultsForIntegratedFlow("RP"); 
 this->PrintFinalResultsForIntegratedFlow("POI"); 
  
 // *****************************************************************************************
 // **** COMPARE RESULTS FROM NESTED LOOPS vs RESULTS FROM Q-VECTORS FOR INTEGRATED FLOW ****
 // *****************************************************************************************    
 
 if(fEvaluateNestedLoopsForIntFlow) 
 {
  this->CompareResultsFromNestedLoopsAndFromQVectorsForIntFlow(fUsePhiWeights||fUsePtWeights||fUseEtaWeights);
 } 
 
 if(fEvaluateNestedLoopsForDiffFlow) 
 {
  this->CompareResultsFromNestedLoopsAndFromQVectorsForDiffFlow(fUsePhiWeights||fUsePtWeights||fUseEtaWeights);
 } 
                                                                                                                                                                                                                                                                                                                                   
} // end of AliFlowAnalysisWithQCumulants::Finish()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectionsForNUACosTerms()
{
 // calculate corrections for non-uniform acceptance of the detector for no-name integrated flow (cos terms)
 
 // multiplicity:
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n = (*fReQ)(0,0);
 Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 Double_t dImQ1n = (*fImQ)(0,0);
 Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);
        
 //                                  *************************************************************
 //                                  **** corrections for non-uniform acceptance (cos terms): ****
 //                                  *************************************************************
 //
 // Remark 1: corrections for non-uniform acceptance (cos terms) calculated with non-weighted Q-vectors 
 //           are stored in 1D profile fQCorrectionsCos.
 // Remark 2: binning of fQCorrectionsCos is organized as follows:
 // --------------------------------------------------------------------------------------------------------------------
 // 1st bin: <<cos(n*(phi1))>> = cosP1n
 // 2nd bin: <<cos(n*(phi1+phi2))>> = cosP1nP1n
 // 3rd bin: <<cos(n*(phi1-phi2-phi3))>> = cosP1nM1nM1n
 // ...
 // --------------------------------------------------------------------------------------------------------------------
  
 // 1-particle:
 Double_t cosP1n = 0.; // <<cos(n*(phi1))>>
   
 if(dMult>0)
 {
  cosP1n = dReQ1n/dMult; 
  
  // average non-weighted 1-particle correction (cos terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[1]->SetBinContent(1,cosP1n);
  
  // final average non-weighted 1-particle correction (cos terms) for non-uniform acceptance for all events:
  fIntFlowCorrectionTermsForNUAPro[1]->Fill(0.5,cosP1n,dMult);  
 } 
 
 // 2-particle:
 Double_t cosP1nP1n = 0.; // <<cos(n*(phi1+phi2))>>
 
 if(dMult>1)
 {
  cosP1nP1n = (pow(dReQ1n,2)-pow(dImQ1n,2)-dReQ2n)/(dMult*(dMult-1)); 
  
  // average non-weighted 2-particle correction (cos terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[1]->SetBinContent(2,cosP1nP1n);
  
  // final average non-weighted 2-particle correction (cos terms) for non-uniform acceptance for all events:
  fIntFlowCorrectionTermsForNUAPro[1]->Fill(1.5,cosP1nP1n,dMult*(dMult-1));  
 } 
 
 // 3-particle:
 Double_t cosP1nM1nM1n = 0.; // <<cos(n*(phi1-phi2-phi3))>>
 
 if(dMult>2)
 {
  cosP1nM1nM1n = (dReQ1n*(pow(dReQ1n,2)+pow(dImQ1n,2))-dReQ1n*dReQ2n-dImQ1n*dImQ2n-2.*(dMult-1)*dReQ1n)
               / (dMult*(dMult-1)*(dMult-2)); 
  
  // average non-weighted 3-particle correction (cos terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[1]->SetBinContent(3,cosP1nM1nM1n);
  
  // final average non-weighted 3-particle correction (cos terms) for non-uniform acceptance for all events:
  fIntFlowCorrectionTermsForNUAPro[1]->Fill(2.5,cosP1nM1nM1n,dMult*(dMult-1)*(dMult-2));  
 } 
 
} // end of AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectionsForNUACosTerms()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectionsForNUASinTerms()
{
 // calculate corrections for non-uniform acceptance of the detector for no-name integrated flow (sin terms)
 
 // multiplicity:
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n = (*fReQ)(0,0);
 Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 Double_t dImQ1n = (*fImQ)(0,0);
 Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);
        
 //                                  *************************************************************
 //                                  **** corrections for non-uniform acceptance (sin terms): ****
 //                                  *************************************************************
 //
 // Remark 1: corrections for non-uniform acceptance (sin terms) calculated with non-weighted Q-vectors 
 //           are stored in 1D profile fQCorrectionsSin.
 // Remark 2: binning of fQCorrectionsSin is organized as follows:
 // --------------------------------------------------------------------------------------------------------------------
 // 1st bin: <<sin(n*(phi1))>> = sinP1n
 // 2nd bin: <<sin(n*(phi1+phi2))>> = sinP1nP1n
 // 3rd bin: <<sin(n*(phi1-phi2-phi3))>> = sinP1nM1nM1n
 // ...
 // --------------------------------------------------------------------------------------------------------------------
 
 // 1-particle:
 Double_t sinP1n = 0.; // <sin(n*(phi1))>
 
 if(dMult>0)
 {
  sinP1n = dImQ1n/dMult; 
     
  // average non-weighted 1-particle correction (sin terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[0]->SetBinContent(1,sinP1n);
  
  // final average non-weighted 1-particle correction (sin terms) for non-uniform acceptance for all events:   
  fIntFlowCorrectionTermsForNUAPro[0]->Fill(0.5,sinP1n,dMult);  
 } 
 
 // 2-particle:
 Double_t sinP1nP1n = 0.; // <<sin(n*(phi1+phi2))>>
 
 if(dMult>1)
 {
  sinP1nP1n = (2.*dReQ1n*dImQ1n-dImQ2n)/(dMult*(dMult-1)); 
     
  // average non-weighted 2-particle correction (sin terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[0]->SetBinContent(2,sinP1nP1n);
  
  // final average non-weighted 1-particle correction (sin terms) for non-uniform acceptance for all events:      
  fIntFlowCorrectionTermsForNUAPro[0]->Fill(1.5,sinP1nP1n,dMult*(dMult-1));  
 } 
 
 // 3-particle:
 Double_t sinP1nM1nM1n = 0.; // <<sin(n*(phi1-phi2-phi3))>>
 
 if(dMult>2)
 {
  sinP1nM1nM1n = (-dImQ1n*(pow(dReQ1n,2)+pow(dImQ1n,2))+dReQ1n*dImQ2n-dImQ1n*dReQ2n+2.*(dMult-1)*dImQ1n)
               / (dMult*(dMult-1)*(dMult-2)); 
  
  // average non-weighted 3-particle correction (sin terms) for non-uniform acceptance for single event:
  fIntFlowCorrectionTermsForNUAEBE[0]->SetBinContent(3,sinP1nM1nM1n);
  
  // final average non-weighted 3-particle correction (sin terms) for non-uniform acceptance for all events:  
  fIntFlowCorrectionTermsForNUAPro[0]->Fill(2.5,sinP1nM1nM1n,dMult*(dMult-1)*(dMult-2));  
 } 
 
} // end of AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectionsForNUASinTerms()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowCosTerms(TString type)
{
 // calculate corrections for non-uniform acceptance of the detector for differential flow (cos terms)
 
 // multiplicity:
 //Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 //Double_t dReQ1n = (*fReQ)(0,0);
 //Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 //Double_t dImQ1n = (*fImQ)(0,0);
 //Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);

 // looping over all (pt,eta) bins and calculating correlations needed for differential flow: 
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   // real and imaginary parts of q_n (non-weighted Q-vector evaluated only for POIs in harmonic n for each (pt,eta) bin): 
   //Double_t dReqnPtEta = 0.;
   //Double_t dImqnPtEta = 0.;

   // number of POIs in each (pt,eta) bin:
   Double_t dmPtEta = 0.;

   // real and imaginary parts of q''_{n}, q''_{2n}, ... 
   // (non-weighted Q-vectors evaluated only for particles which are both RPs and POIs in harmonic n, 2n, ... for each (pt,eta) bin): 
   //Double_t dReqPrimePrime1nPtEta = 0.;
   //Double_t dImqPrimePrime1nPtEta = 0.;
   //Double_t dReqPrimePrime2nPtEta = 0.;
   //Double_t dImqPrimePrime2nPtEta = 0.;

   // number of particles which are both RPs and POIs in each (pt,eta) bin:
   //Double_t dmPrimePrimePtEta = 0.;
   
   if(type == "POI")
   {
    // q''_{n}, q''_{2n}:
    //...............................................................................................
    //dReqPrimePrime1nPtEta = fReqPrimePrime1nPtEta->GetBinContent(fReqPrimePrime1nPtEta->GetBin(p,e));
    //dImqPrimePrime1nPtEta = fImqPrimePrime1nPtEta->GetBinContent(fImqPrimePrime1nPtEta->GetBin(p,e));
    //dReqPrimePrime2nPtEta = fReqPrimePrime2nPtEta->GetBinContent(fReqPrimePrime2nPtEta->GetBin(p,e));
    //dImqPrimePrime2nPtEta = fImqPrimePrime2nPtEta->GetBinContent(fImqPrimePrime2nPtEta->GetBin(p,e));
    //...............................................................................................
   
    // m'':
    //dmPrimePrimePtEta = fmPrimePrimePtEta->GetBinContent(fmPrimePrimePtEta->GetBin(p,e));
   
    // q'_{n}: 
    //dReqnPtEta = fReqnPtEta->GetBinContent(fReqnPtEta->GetBin(p,e));
    //dImqnPtEta = fImqnPtEta->GetBinContent(fImqnPtEta->GetBin(p,e));
    //dmPtEta    = fmPtEta->GetBinContent(fmPtEta->GetBin(p,e));
   }
   else if(type == "RP")
   {
    // q_RP{n}, q_RP{2n}:
    //...............................................................................................
    //dReqPrimePrime1nPtEta = fReqRP1nPtEta->GetBinContent(fReqRP1nPtEta->GetBin(p,e));
    //dImqPrimePrime1nPtEta = fImqRP1nPtEta->GetBinContent(fImqRP1nPtEta->GetBin(p,e));
    //dReqPrimePrime2nPtEta = fReqRP2nPtEta->GetBinContent(fReqRP2nPtEta->GetBin(p,e));
    //dImqPrimePrime2nPtEta = fImqRP2nPtEta->GetBinContent(fImqRP2nPtEta->GetBin(p,e));
    //...............................................................................................
   
    // m'':
    //dmPrimePrimePtEta = fmRPPtEta->GetBinContent(fmRPPtEta->GetBin(p,e));
   
    //dReqnPtEta = fReqRP1nPtEta->GetBinContent(fReqRP1nPtEta->GetBin(p,e)); // not a bug ;-)
    //dImqnPtEta = fImqRP1nPtEta->GetBinContent(fImqRP1nPtEta->GetBin(p,e)); // not a bug ;-)
    //dmPtEta    = fmRPPtEta->GetBinContent(fmRPPtEta->GetBin(p,e));         // not a bug ;-) 
   }
   
   // 1'-p correction:
   //Double_t oneCosP1nPsiPtEta = 0.;
   
   if(dmPtEta)
   {
    //oneCosP1nPsiPtEta = dReqnPtEta/dmPtEta;
   
    // fill the 2D profile to get the average 1'-p correction for each (pt, eta) bin:
    if(type == "POI")
    { 
     //fCorrectionsCosP1nPsiPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,
     //                                    oneCosP1nPsiPtEta,dmPtEta);
    }
    else if(type == "RP")
    {
     //fCorrectionsCosP1nPsiPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,
     //                                    oneCosP1nPsiPtEta,dmPtEta);
    }
   } // end of if(dmPtEta*dMult-dmPrimePrimePtEta)
   
   /*
   
   // 4'-particle correlation:
   Double_t four1n1n1n1nPtEta = 0.;
   if((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
       + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.)) // to be improved (introduce a new variable for this expression)
   {
    four1n1n1n1nPtEta = ((pow(dReQ1n,2.)+pow(dImQ1n,2.))*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)
                      - dReqPrimePrime2nPtEta*(pow(dReQ1n,2.)-pow(dImQ1n,2.))
                      - 2.*dImqPrimePrime2nPtEta*dReQ1n*dImQ1n
                      - dReqnPtEta*(dReQ1n*dReQ2n+dImQ1n*dImQ2n)
                      + dImqnPtEta*(dImQ1n*dReQ2n-dReQ1n*dImQ2n)
                      - 2.*dMult*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)
                      - 2.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*dmPrimePrimePtEta                      
                      + 6.*(dReqPrimePrime1nPtEta*dReQ1n+dImqPrimePrime1nPtEta*dImQ1n)                                            
                      + 1.*(dReqPrimePrime2nPtEta*dReQ2n+dImqPrimePrime2nPtEta*dImQ2n)                      
                      + 2.*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)                       
                      + 2.*dmPrimePrimePtEta*dMult                      
                      - 6.*dmPrimePrimePtEta)        
                      / ((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                          + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
    
    // fill the 2D profile to get the average correlation for each (pt, eta) bin:
    if(type == "POI")
    {
     f4pPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                       (dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                        + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.));
    }
    else if(type == "RP")
    {
     f4pPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                      (dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                       + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.));   
    }
   } // end of if((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
     //            +dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.))
   
  */
   
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 
} // end of AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowCosTerms(TString type)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowSinTerms(TString type)
{
 // calculate corrections for non-uniform acceptance of the detector for differential flow (sin terms)
 
 // multiplicity:
 //Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 //Double_t dReQ1n = (*fReQ)(0,0);
 //Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 //Double_t dImQ1n = (*fImQ)(0,0);
 //Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);

 // looping over all (pt,eta) bins and calculating correlations needed for differential flow: 
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   // real and imaginary parts of q_n (non-weighted Q-vector evaluated only for POIs in harmonic n for each (pt,eta) bin): 
   //Double_t dReqnPtEta = 0.;
   //Double_t dImqnPtEta = 0.;

   // number of POIs in each (pt,eta) bin:
   Double_t dmPtEta = 0.;

   // real and imaginary parts of q''_{n}, q''_{2n}, ... 
   // (non-weighted Q-vectors evaluated only for particles which are both RPs and POIs in harmonic n, 2n, ... for each (pt,eta) bin): 
   //Double_t dReqPrimePrime1nPtEta = 0.;
   //Double_t dImqPrimePrime1nPtEta = 0.;
   //Double_t dReqPrimePrime2nPtEta = 0.;
   //Double_t dImqPrimePrime2nPtEta = 0.;

   // number of particles which are both RPs and POIs in each (pt,eta) bin:
   //Double_t dmPrimePrimePtEta = 0.;
   
   if(type == "POI")
   {
    // q''_{n}, q''_{2n}:
    //...............................................................................................
    //dReqPrimePrime1nPtEta = fReqPrimePrime1nPtEta->GetBinContent(fReqPrimePrime1nPtEta->GetBin(p,e));
    //dImqPrimePrime1nPtEta = fImqPrimePrime1nPtEta->GetBinContent(fImqPrimePrime1nPtEta->GetBin(p,e));
    //dReqPrimePrime2nPtEta = fReqPrimePrime2nPtEta->GetBinContent(fReqPrimePrime2nPtEta->GetBin(p,e));
    //dImqPrimePrime2nPtEta = fImqPrimePrime2nPtEta->GetBinContent(fImqPrimePrime2nPtEta->GetBin(p,e));
    //...............................................................................................
   
    // m'':
    //dmPrimePrimePtEta = fmPrimePrimePtEta->GetBinContent(fmPrimePrimePtEta->GetBin(p,e));
   
    // q'_{n}: 
    //dReqnPtEta = fReqnPtEta->GetBinContent(fReqnPtEta->GetBin(p,e));
    //dImqnPtEta = fImqnPtEta->GetBinContent(fImqnPtEta->GetBin(p,e));
    //dmPtEta    = fmPtEta->GetBinContent(fmPtEta->GetBin(p,e));
   }
   else if(type == "RP")
   {
    // q_RP{n}, q_RP{2n}:
    //...............................................................................................
    //dReqPrimePrime1nPtEta = fReqRP1nPtEta->GetBinContent(fReqRP1nPtEta->GetBin(p,e));
    //dImqPrimePrime1nPtEta = fImqRP1nPtEta->GetBinContent(fImqRP1nPtEta->GetBin(p,e));
    //dReqPrimePrime2nPtEta = fReqRP2nPtEta->GetBinContent(fReqRP2nPtEta->GetBin(p,e));
    //dImqPrimePrime2nPtEta = fImqRP2nPtEta->GetBinContent(fImqRP2nPtEta->GetBin(p,e));
    //...............................................................................................
   
    // m'':
    //dmPrimePrimePtEta = fmRPPtEta->GetBinContent(fmRPPtEta->GetBin(p,e));
   
    //dReqnPtEta = fReqRP1nPtEta->GetBinContent(fReqRP1nPtEta->GetBin(p,e)); // not a bug ;-)
    //dImqnPtEta = fImqRP1nPtEta->GetBinContent(fImqRP1nPtEta->GetBin(p,e)); // not a bug ;-)
    //dmPtEta    = fmRPPtEta->GetBinContent(fmRPPtEta->GetBin(p,e));         // not a bug ;-) 
   }
   
   // 1'-p correction:
   //Double_t oneSinP1nPsiPtEta = 0.;
   
   if(dmPtEta)
   {
    //oneSinP1nPsiPtEta = dImqnPtEta/dmPtEta;
   
    // fill the 2D profile to get the average 1'-p correction for each (pt, eta) bin:
    if(type == "POI")
    { 
     //fCorrectionsSinP1nPsiPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,
     //                                    oneSinP1nPsiPtEta,dmPtEta);
    }
    else if(type == "RP")
    {
     //fCorrectionsSinP1nPsiPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,
     //                                    oneSinP1nPsiPtEta,dmPtEta);
    }
   } // end of if(dmPtEta*dMult-dmPrimePrimePtEta)
   
   /*
   
   // 4'-particle correlation:
   Double_t four1n1n1n1nPtEta = 0.;
   if((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
       + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.)) // to be improved (introduce a new variable for this expression)
   {
    four1n1n1n1nPtEta = ((pow(dReQ1n,2.)+pow(dImQ1n,2.))*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)
                      - dReqPrimePrime2nPtEta*(pow(dReQ1n,2.)-pow(dImQ1n,2.))
                      - 2.*dImqPrimePrime2nPtEta*dReQ1n*dImQ1n
                      - dReqnPtEta*(dReQ1n*dReQ2n+dImQ1n*dImQ2n)
                      + dImqnPtEta*(dImQ1n*dReQ2n-dReQ1n*dImQ2n)
                      - 2.*dMult*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)
                      - 2.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*dmPrimePrimePtEta                      
                      + 6.*(dReqPrimePrime1nPtEta*dReQ1n+dImqPrimePrime1nPtEta*dImQ1n)                                            
                      + 1.*(dReqPrimePrime2nPtEta*dReQ2n+dImqPrimePrime2nPtEta*dImQ2n)                      
                      + 2.*(dReqnPtEta*dReQ1n+dImqnPtEta*dImQ1n)                       
                      + 2.*dmPrimePrimePtEta*dMult                      
                      - 6.*dmPrimePrimePtEta)        
                      / ((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                          + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
    
    // fill the 2D profile to get the average correlation for each (pt, eta) bin:
    if(type == "POI")
    {
     f4pPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                       (dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                        + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.));
    }
    else if(type == "RP")
    {
     f4pPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                      (dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
                       + dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.));   
    }
   } // end of if((dmPtEta-dmPrimePrimePtEta)*dMult*(dMult-1.)*(dMult-2.)
     //            +dmPrimePrimePtEta*(dMult-1.)*(dMult-2.)*(dMult-3.))
   
  */
   
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 
} // end of AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNonUniformAcceptanceForDifferentialFlowSinTerms(TString type)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::EvaluateNestedLoopsForDifferentialFlow(AliFlowEventSimple* anEvent)
{
 // evaluate the nested loops relevant for differential flow (needed for cross-checking the results)
 
 Int_t nPrim = anEvent->NumberOfTracks(); 
 AliFlowTrackSimple *aftsTrack = NULL;
 
 Double_t psi1=0., phi2=0., phi3=0., phi4=0.;// phi5=0., phi6=0., phi7=0., phi8=0.;
 Double_t wPhi1=1., wPhi2=1., wPhi3=1., wPhi4=1.;// wPhi5=1., wPhi6=1., wPhi7=1., wPhi8=1.;
 
 Int_t n = fHarmonic; // to be improved
 
 //                                          ********************************************
 //                                          **** NESTED LOOPS FOR DIFFERENTIAL FLOW ****
 //                                          ******************************************** 
 
 // Remark 1: (pt,eta) bin in which the cross-checking will be performed is given by 1.1 < pt < 1.2 GeV and -0.55 < eta < -0.525 
 
 // Remark 2: multi-particle correlations needed for diff. flow calculated with nested loops without weights are stored in 1D profile  
 //           fDirectCorrelationsDiffFlow
 
 // Remark 3: multi-particle correlations needed for diff. flow calculated with nested loops with weights are stored in 1D profile  
 //           fDirectCorrelationsDiffFlowW;
 
 // Remark 4: binning of fDirectCorrelationsDiffFlow is organized as follows:
 //......................................................................................
 //       ---- bins 1-20: 2-particle correlations ----
 //  1st bin: <2'>_{1n|1n} = twoPrime1n1n = <cos(n*(psi1-phi2))>
 //       ---- bins 21-40: 3-particle correlations ----
 //       ---- bins 41-60: 4-particle correlations ----
 // 41st bin: <4'>_{1n,1n|1n,1n} = fourPrime1n1n1n1n  = <cos(n*(psi1+phi2-phi3-phi4))>
 //......................................................................................
 
 // Remark 5: binning of fDirectCorrelationsDiffFlow is organized as follows:
 //......................................................................................
 //       ---- bins 1-20: 2-particle correlations ----
 //  1st bin: twoPrime1n1nW0W1 = <w2 cos(n*(psi1-phi2))>
 //       ---- bins 21-40: 3-particle correlations ----
 //       ---- bins 41-60: 4-particle correlations ----
 // 41st bin: fourPrime1n1n1n1nW0W1W1W1 = <w2 w3 w4 cos(n*(psi1+phi2-phi3-phi4))>
 //......................................................................................
 
 // 2'-particle:
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  // POI condition (first particle in the correlator must be POI): 
  if(!((aftsTrack->Pt()>=1.1 && aftsTrack->Pt()<1.2) && (aftsTrack->Eta()>=-0.55 && aftsTrack->Eta()<-0.525) && (aftsTrack->InPOISelection())))continue;
  psi1=aftsTrack->Phi(); 
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(psi1*fnBinsPhi/TMath::TwoPi())));
  
  fDirectCorrectionsDiffFlowCos->Fill(0.,cos(1.*n*(psi1)),1.); // <<cos(n*(psi1))>>
  fDirectCorrectionsDiffFlowSin->Fill(0.,sin(1.*n*(psi1)),1.); // <<sin(n*(psi1))>>
  
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   // RP condition (!(first) particle in the correlator must be RP):
   if(!(aftsTrack->InRPSelection()))continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
    
   // non-weighted: 
   //.....................................................................................  
   fDirectCorrelationsDiffFlow->Fill(0.,cos(1.*n*(psi1-phi2)),1.); // <cos(n*(psi1-phi2))>
   //.....................................................................................  
   // weighted:
   //.....................................................................................   
   if(fUsePhiWeights) fDirectCorrelationsDiffFlowW->Fill(0.,cos(1.*n*(psi1-phi2)),wPhi2); // <w2 cos(n*(psi1-phi2))>
   //.....................................................................................  
   
   //fDirectCorrelations->Fill(103.,cos(1.*n*(phi1-phi2)),pow(wPhi1,2)*wPhi2);//<2'>_{n,n}
   //fDirectCorrelations->Fill(104.,cos(2.*n*(phi1-phi2)),wPhi1*pow(wPhi2,2));//<2'>_{n,n}
   //fDirectCorrelations->Fill(105.,cos(1.*n*(phi1-phi2)),pow(wPhi2,3));//<2'>_{n,n}  
   //fDirectCorrelations->Fill(41.,cos(2.*n*(phi1-phi2)),1);//<2'>_{2n,2n}
   //fDirectCorrelations->Fill(42.,cos(3.*n*(phi1-phi2)),1);//<2'>_{3n,3n}
   //fDirectCorrelations->Fill(43.,cos(4.*n*(phi1-phi2)),1);//<2'>_{4n,4n}   
    
  }//end of for(Int_t i2=0;i2<nPrim;i2++)
 }//end of for(Int_t i1=0;i1<nPrim;i1++)
 
 
 
 /*
 
 //<3'>_{2n|n,n}
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!((aftsTrack->Pt()>=0.5&&aftsTrack->Pt()<0.6)&&(aftsTrack->InPOISelection())))continue;//POI condition
  psi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(psi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection()))continue;//RP condition
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection()))continue;//RP condition
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    //fill the fDirectCorrelations:     
    
    // 2-p
    //fDirectCorrelations->Fill(101.,cos(n*(phi2-phi3)),wPhi1*wPhi2*wPhi3); // <w1 w2 w3 cos(n(phi2-phi3))>
    //fDirectCorrelations->Fill(102.,cos(n*(phi1-phi3)),pow(wPhi2,2.)*wPhi3); // <w2^2 w3 cos(n(psi1-phi2))>
    
    // 3-p            
    //fDirectCorrelations->Fill(110.,cos(n*(2.*phi1-phi2-phi3)),wPhi1*wPhi2*wPhi3); // <w1 w2 w3 cos(n(2psi1-phi2-phi3))>
    //fDirectCorrelations->Fill(111.,cos(n*(phi1+phi2-2.*phi3)),wPhi2*pow(wPhi3,2.)); // <w2 w3^2 cos(n(psi1+phi2-2.*phi3))>
    
    
    //fDirectCorrelations->Fill(46.,cos(n*(phi1+phi2-2.*phi3)),1);//<3'>_{n,n|2n}    
   }//end of for(Int_t i3=0;i3<nPrim;i3++)  
  }//end of for(Int_t i2=0;i2<nPrim;i2++)  
 }//end of for(Int_t i1=0;i1<nPrim;i1++)
 */
 
 // 4'-particle:
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  // POI condition (first particle in the correlator must be POI): 
  if(!((aftsTrack->Pt()>=1.1 && aftsTrack->Pt()<1.2) && (aftsTrack->Eta()>=-0.55 && aftsTrack->Eta()<-0.525) && (aftsTrack->InPOISelection())))continue;
  psi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(psi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   // RP condition (!(first) particle in the correlator must be RP): 
   if(!(aftsTrack->InRPSelection()))continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   { 
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    // RP condition (!(first) particle in the correlator must be RP):
    if(!(aftsTrack->InRPSelection()))continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     // RP condition (!(first) particle in the correlator must be RP):
     if(!(aftsTrack->InRPSelection()))continue;  
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     
     // non-weighted:
     //.........................................................................................................................
     fDirectCorrelationsDiffFlow->Fill(40.,cos(n*(psi1+phi2-phi3-phi4)),1.); // <cos(n(psi1+phi1-phi2-phi3))> 
     //.........................................................................................................................     
     // weighted:
     //...............................................................................................................................
     if(fUsePhiWeights) fDirectCorrelationsDiffFlowW->Fill(40.,cos(n*(psi1+phi2-phi3-phi4)),wPhi2*wPhi3*wPhi4); // <w2 w3 w4 cos(n(psi1+phi2-phi3-phi4))> 
     //...............................................................................................................................     
          
    }//end of for(Int_t i4=0;i4<nPrim;i4++)
   }//end of for(Int_t i3=0;i3<nPrim;i3++)
  }//end of for(Int_t i2=0;i2<nPrim;i2++) 
 }//end of for(Int_t i1=0;i1<nPrim;i1++)
 
 /*                
 //<5'>_{2n,n|n,n,n}
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!((aftsTrack->Pt()>=0.5&&aftsTrack->Pt()<0.6)&&(aftsTrack->InPOISelection())))continue;//POI condition
  phi1=aftsTrack->Phi();
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection()))continue;//RP condition   
   phi2=aftsTrack->Phi();
   for(Int_t i3=0;i3<nPrim;i3++)
   { 
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection()))continue;//RP condition   
    phi3=aftsTrack->Phi();
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection()))continue;//RP condition  
     phi4=aftsTrack->Phi();//
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection()))continue;//RP condition  
      phi5=aftsTrack->Phi();    
      //fill the fDirectCorrelations:if(bNestedLoops)
      //fDirectCorrelations->Fill(55.,cos(2.*n*phi1+n*phi2-n*phi3-n*phi4-n*phi5),1);//<5'>_{2n,n|n,n,n}
     }//end of for(Int_t i5=0;i5<nPrim;i5++)  
    }//end of for(Int_t i4=0;i4<nPrim;i4++)
   }//end of for(Int_t i3=0;i3<nPrim;i3++)
  }//end of for(Int_t i2=0;i2<nPrim;i2++) 
 }//end of for(Int_t i1=0;i1<nPrim;i1++)
 

  
 */
 /*
 
 
 
 //<6'>_{n,n,n|n,n,n}
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!((aftsTrack->Pt()>=0.5&&aftsTrack->Pt()<0.6)&&(aftsTrack->InPOISelection())))continue;//POI condition
  phi1=aftsTrack->Phi();
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection()))continue;//RP condition   
   phi2=aftsTrack->Phi();
   for(Int_t i3=0;i3<nPrim;i3++)
   { 
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection()))continue;//RP condition   
    phi3=aftsTrack->Phi();
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection()))continue;//RP condition  
     phi4=aftsTrack->Phi();
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection()))continue;//RP condition  
      phi5=aftsTrack->Phi();    
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection()))continue;//RP condition  
       phi6=aftsTrack->Phi();  
       //fill the fDirectCorrelations:
       //fDirectCorrelations->Fill(60.,cos(n*(phi1+phi2+phi3-phi4-phi5-phi6)),1);//<6'>_{n,n,n|n,n,n}
      }//end of for(Int_t i6=0;i6<nPrim;i6++)   
     }//end of for(Int_t i5=0;i5<nPrim;i5++)  
    }//end of for(Int_t i4=0;i4<nPrim;i4++)
   }//end of for(Int_t i3=0;i3<nPrim;i3++)
  }//end of for(Int_t i2=0;i2<nPrim;i2++) 
 }//end of for(Int_t i1=0;i1<nPrim;i1++)

 
 */
 /* 
   
     
 //<7'>_{2n,n,n|n,n,n,n}
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!((aftsTrack->Pt()>=0.5&&aftsTrack->Pt()<0.6)&&(aftsTrack->InPOISelection())))continue;//POI condition
  phi1=aftsTrack->Phi();
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection()))continue;//RP condition   
   phi2=aftsTrack->Phi();
   for(Int_t i3=0;i3<nPrim;i3++)
   { 
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection()))continue;//RP condition   
    phi3=aftsTrack->Phi();
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection()))continue;//RP condition  
     phi4=aftsTrack->Phi();
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection()))continue;//RP condition  
      phi5=aftsTrack->Phi();    
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection()))continue;//RP condition  
       phi6=aftsTrack->Phi();
       for(Int_t i7=0;i7<nPrim;i7++)
       {
        if(i7==i1||i7==i2||i7==i3||i7==i4||i7==i5||i7==i6)continue;
        aftsTrack=anEvent->GetTrack(i7);
        if(!(aftsTrack->InRPSelection()))continue;//RP condition  
        phi7=aftsTrack->Phi();   
        //fill the fDirectCorrelations:
        //fDirectCorrelations->Fill(65.,cos(2.*n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6-n*phi7),1);//<7'>_{2n,n,n|n,n,n,n}
       }//end of for(Int_t i7=0;i7<nPrim;i7++)  
      }//end of for(Int_t i6=0;i6<nPrim;i6++)   
     }//end of for(Int_t i5=0;i5<nPrim;i5++)  
    }//end of for(Int_t i4=0;i4<nPrim;i4++)
   }//end of for(Int_t i3=0;i3<nPrim;i3++)
  }//end of for(Int_t i2=0;i2<nPrim;i2++) 
 }//end of for(Int_t i1=0;i1<nPrim;i1++)

 
  
 */
 /*  
    
     
       
 //<8'>_{n,n,n,n|n,n,n,n}
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!((aftsTrack->Pt()>=0.5&&aftsTrack->Pt()<0.6)&&(aftsTrack->InPOISelection())))continue;//POI condition
  phi1=aftsTrack->Phi();
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection()))continue;//RP condition   
   phi2=aftsTrack->Phi();
   for(Int_t i3=0;i3<nPrim;i3++)
   { 
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection()))continue;//RP condition   
    phi3=aftsTrack->Phi();
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection()))continue;//RP condition  
     phi4=aftsTrack->Phi();
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection()))continue;//RP condition  
      phi5=aftsTrack->Phi();    
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection()))continue;//RP condition  
       phi6=aftsTrack->Phi();
       for(Int_t i7=0;i7<nPrim;i7++)
       {
        if(i7==i1||i7==i2||i7==i3||i7==i4||i7==i5||i7==i6)continue;
        aftsTrack=anEvent->GetTrack(i7);
        if(!(aftsTrack->InRPSelection()))continue;//RP condition  
        phi7=aftsTrack->Phi();
        for(Int_t i8=0;i8<nPrim;i8++)
        {
         if(i8==i1||i8==i2||i8==i3||i8==i4||i8==i5||i8==i6||i8==i7)continue;
         aftsTrack=anEvent->GetTrack(i8);
         if(!(aftsTrack->InRPSelection()))continue;//RP condition  
         phi8=aftsTrack->Phi();           
         //fill the fDirectCorrelations:
         //fDirectCorrelations->Fill(70.,cos(n*(phi1+phi2+phi3+phi4-phi5-phi6-phi7-phi8)),1);//<8'>_{n,n,n,n|n,n,n,n}
        }//end of for(Int_t i8=0;i8<nPrim;i8++) 
       }//end of for(Int_t i7=0;i7<nPrim;i7++)  
      }//end of for(Int_t i6=0;i6<nPrim;i6++)   
     }//end of for(Int_t i5=0;i5<nPrim;i5++)  
    }//end of for(Int_t i4=0;i4<nPrim;i4++)
   }//end of for(Int_t i3=0;i3<nPrim;i3++)
  }//end of for(Int_t i2=0;i2<nPrim;i2++) 
 }//end of for(Int_t i1=0;i1<nPrim;i1++)
 
 
 
 */ 
 
 
 
 
} // end of AliFlowAnalysisWithQCumulants::EvaluateNestedLoopsForDifferentialFlow(AliFlowEventSimple* anEvent)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetOutputHistograms(TList *outputListHistos)
{
 // a) Get pointers for common control and common result histograms and profiles.
 // b) Get pointers for histograms with particle weights.
 // c) Get pointers for histograms and profiles relevant for integrated flow.
 // d) Get pointers for histograms and profiles relevant for differental flow.
 // e) Get pointers for histograms and profiles holding results obtained with nested loops.
 
 if(outputListHistos)
 {	
  this->GetPointersForCommonHistograms(outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
  this->GetPointersForParticleWeightsHistograms(outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
  this->GetPointersForIntFlowHistograms(outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
  this->GetPointersForDiffFlowHistograms(outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
  this->GetPointersForNestedLoopsHistograms(outputListHistos); // to be improved (no need to pass here argument, use setter for base list instead)
 }
   
  
   /*
   
   
   // profiles and results: 
   diffFlowListProfiles = dynamic_cast<TList*>(diffFlowList->FindObject("Profiles"));
   diffFlowListResults = dynamic_cast<TList*>(diffFlowList->FindObject("Results"));
  } else
    {
     cout<<"WARNING: diffFlowList is NULL in AFAWQC::GOH() !!!!"<<endl;
    }      
  
  // nested list in the list of profiles fDiffFlowListProfiles "Profiles":
  TList *dfpType[2] = {NULL};
  TList *dfpParticleWeights[2][2] = {{NULL}};
  TList *dfpEventWeights[2][2][2] = {{{NULL}}};
  TList *diffFlowCorrelations[2][2][2] = {{{NULL}}};
  TList *diffFlowProductsOfCorrelations[2][2][2] = {{{NULL}}};
  TList *diffFlowCorrectionTerms[2][2][2][2] = {{{{NULL}}}};
  
  if(diffFlowListProfiles)
  {
   for(Int_t t=0;t<2;t++)
   {
    dfpType[t] = dynamic_cast<TList*>(diffFlowListProfiles->FindObject(typeFlag[t].Data()));
    if(dfpType[t])
    {
     for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
     {
      dfpParticleWeights[t][pW] = dynamic_cast<TList*>(dfpType[t]->FindObject(Form("%s, pWeights %s",typeFlag[t].Data(),pWeightsFlag[pW].Data())));
      if(dfpParticleWeights[t][pW])
      {
       for(Int_t eW=0;eW<2;eW++)
       {
        dfpEventWeights[t][pW][eW] = dynamic_cast<TList*>(dfpParticleWeights[t][pW]->FindObject(Form("%s, pWeights %s, eWeights %s",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
        if(dfpEventWeights[t][pW][eW])
        {
         // correlations: 
         diffFlowCorrelations[t][pW][eW] = dynamic_cast<TList*>(dfpEventWeights[t][pW][eW]->FindObject(Form("Correlations (%s, pWeights %s, eWeights %s)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
         // products of correlations:
         diffFlowProductsOfCorrelations[t][pW][eW] = dynamic_cast<TList*>(dfpEventWeights[t][pW][eW]->FindObject(Form("Products of correlations (%s, pWeights %s, eWeights %s)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
         // correction terms:
         for(Int_t sc=0;sc<2;sc++)
         {
          diffFlowCorrectionTerms[t][pW][eW][sc] = dynamic_cast<TList*>(dfpEventWeights[t][pW][eW]->FindObject(Form("Corrections for NUA (%s, pWeights %s, eWeights %s, %s terms)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),sinCosFlag[sc].Data())));
          //this->SetDiffFlowCorrectionTerms(diffFlowCorrectionTerms[t][pW][sc],t,pW,sc);   
         }
        } else // to if(dfpEventWeights[t][pW][eW])
          {
           cout<<"WARNING: dfpEventWeights[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
           cout<<"t = "<<t<<endl;
           cout<<"pW = "<<pW<<endl;
           cout<<"eW = "<<eW<<endl;
          }
       } // end of for(Int_t eW=0;eW<2;eW++)   
      } else // to if(dfpParticleWeights[t][pW])
        {
         cout<<"WARNING: dfpParticleWeights[t][pW] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
        }
     } // end of for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++) 
     } else // if(dfpType[t]) 
      {
       cout<<"WARNING: dfpType[t] is NULL in AFAWQC::GOH() !!!!"<<endl;
       cout<<"t = "<<t<<endl;
      }
   } // end of for(Int_t t=0;t<2;t++)
  } else // to if(diffFlowListProfiles)
    {
     cout<<"WARNING: diffFlowListProfiles is NULL in AFAWQC::GOH() !!!!"<<endl;
    }  
  
  TProfile2D *correlationsPro[2][2][2][4] = {{{{NULL}}}};
  TProfile2D *productsOfCorrelationsPro[2][2][2][5] = {{{{NULL}}}};
  TProfile2D *correctionTermsPro[2][2][2][2][2] = {{{{{NULL}}}}};
  
  TString correlationName[4] = {"<2'>","<4'>","<6'>","<8'>"};
  TString cumulantName[4] = {"QC{2'}","QC{4'}","QC{6'}","QC{8'}"};
  TString productOfCorrelationsName[5] = {"<2><2'>","<2><4'>","<4><2'>","<4><4'>","<2'><4'>"};
  TString correctionsSinTermsName[2] = {"sin(1)","sin(2)"};
  TString correctionsCosTermsName[2] = {"cos(1)","cos(2)"};
  TString correctionName[4] = {"corr. to QC{2'}","corr. to QC{4'}","corr. to QC{6'}","corr. to QC{8'}"};
  TString covarianceName[5] = {"Cov(<2>,<2'>)","Cov(<2>,<4'>)","Cov(<4>,<2'>)","Cov(<4>,<4'>)","Cov(<2'>,<4'>)"}; 
  TString flowName[4] = {"v'{2}","v'{4}","v'{6}","v'{8}"}; 
  
  for(Int_t t=0;t<2;t++)
  {
   for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
   {
    for(Int_t eW=0;eW<2;eW++)
    {
     // correlations:
     if(diffFlowCorrelations[t][pW][eW])
     {
      for(Int_t correlIndex=0;correlIndex<4;correlIndex++)
      {
       correlationsPro[t][pW][eW][correlIndex] = dynamic_cast<TProfile2D*>(diffFlowCorrelations[t][pW][eW]->FindObject(correlationName[correlIndex].Data()));
       if(correlationsPro[t][pW][eW][correlIndex])
       {
        this->SetCorrelationsPro(correlationsPro[t][pW][eW][correlIndex],t,pW,eW,correlIndex);
       } else // to if(correlationsPro[t][pW][ew][correlIndex])
         {
          cout<<"WARNING: correlationsPro[t][pW][eW][correlIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"ci = "<<correlIndex<<endl;
         } 
      }
     } else // to if(diffFlowCorrelations[t][pW][eW])
       {
        cout<<"WARNING: diffFlowCorrelations[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
        cout<<"t = "<<t<<endl;
        cout<<"pW = "<<pW<<endl;
        cout<<"eW = "<<eW<<endl;
       } 
     // products of correlations:
     if(diffFlowProductsOfCorrelations[t][pW][eW])
     {
      for(Int_t productOfCorrelationsIndex=0;productOfCorrelationsIndex<5;productOfCorrelationsIndex++)
      {
       productsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex] = dynamic_cast<TProfile2D*>(diffFlowProductsOfCorrelations[t][pW][eW]->FindObject(productOfCorrelationsName[productOfCorrelationsIndex].Data()));
       if(productsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex])
       {
        this->SetProductsOfCorrelationsPro(productsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex],t,pW,eW,productOfCorrelationsIndex);
       } else // to if(productsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex])
         {
          cout<<"WARNING: productsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"ci = "<<productOfCorrelationsIndex<<endl;
         } 
      }
     } else // to if(diffFlowProductsOfCorrelations[t][pW][eW])
       {
        cout<<"WARNING: diffFlowProductsOfCorrelations[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
        cout<<"t = "<<t<<endl;
        cout<<"pW = "<<pW<<endl;
        cout<<"eW = "<<eW<<endl;
       }
     // correction terms:
     for(Int_t sc=0;sc<2;sc++)
     {
      if(diffFlowCorrectionTerms[t][pW][eW][sc])
      {
       for(Int_t correctionIndex=0;correctionIndex<2;correctionIndex++)
       {
        if(sc==0)
        {
         correctionTermsPro[t][pW][eW][sc][correctionIndex] = dynamic_cast<TProfile2D*>(diffFlowCorrectionTerms[t][pW][eW][sc]->FindObject(correctionsSinTermsName[correctionIndex].Data())); 
         if(correctionTermsPro[t][pW][eW][sc][correctionIndex])
         {
          this->SetCorrectionTermsPro(correctionTermsPro[t][pW][eW][sc][correctionIndex],t,pW,eW,sc,correctionIndex);
         } else 
           {
            cout<<"WARNING: correctionTermsPro[t][pW][eW][sc][correctionIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
            cout<<"t = "<<t<<endl;
            cout<<"pW = "<<pW<<endl;
            cout<<"eW = "<<eW<<endl;
            cout<<"sc = "<<sc<<endl;
            cout<<"ci = "<<correctionIndex<<endl;
           }
        } 
        if(sc==1)
        {
         correctionTermsPro[t][pW][eW][sc][correctionIndex] = dynamic_cast<TProfile2D*>(diffFlowCorrectionTerms[t][pW][eW][sc]->FindObject(correctionsCosTermsName[correctionIndex].Data())); 
         if(correctionTermsPro[t][pW][eW][sc][correctionIndex])
         {
          this->SetCorrectionTermsPro(correctionTermsPro[t][pW][eW][sc][correctionIndex],t,pW,eW,sc,correctionIndex);
         } else 
           {
            cout<<"WARNING: correctionTermsPro[t][pW][eW][sc][correctionIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
            cout<<"t = "<<t<<endl;
            cout<<"pW = "<<pW<<endl;
            cout<<"eW = "<<eW<<endl;
            cout<<"sc = "<<sc<<endl;
            cout<<"ci = "<<correctionIndex<<endl;
           }         
        }  
       } // end of for(Int_t correctionIndex=0;correctionIndex<2;correctionIndex++)
      } else // to if(diffFlowCorrectionTerms[t][pW][eW][sc])
        {
         cout<<"WARNING: diffFlowCorrectionTerms[t][pW][eW][sc] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
         cout<<"eW = "<<eW<<endl;
         cout<<"sc = "<<sc<<endl;
        }
     } // end of for(Int_t sc=0;sc<2;sc++)  
    } // end of for(Int_t eW=0;eW<2;eW++)
   }  // end of for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
  } // end of for(Int_t t=0;t<2;t++)
  
  // nested list in the list of results fDiffFlowListResults "Results":
  TList *dfrType[2] = {NULL};
  TList *dfrParticleWeights[2][2] = {{NULL}};
  TList *dfrEventWeights[2][2][2] = {{{NULL}}};
  TList *dfrCorrections[2][2][2][2] = {{{{NULL}}}}; 
  TList *diffFlowFinalCorrelations[2][2][2] = {{{NULL}}}; 
  TList *diffFlowFinalCorrections[2][2][2] = {{{NULL}}}; 
  TList *diffFlowFinalCovariances[2][2][2] = {{{NULL}}};   
  TList *diffFlowFinalCumulants[2][2][2][2] = {{{{NULL}}}};  
  TList *diffFlowFinalFlow[2][2][2][2] = {{{{NULL}}}}; 
 
  if(diffFlowListResults)
  {
   for(Int_t t=0;t<2;t++)
   {
    dfrType[t] = dynamic_cast<TList*>(diffFlowListResults->FindObject(typeFlag[t].Data()));
    if(dfrType[t])
    {
     for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
     {
      dfrParticleWeights[t][pW] = dynamic_cast<TList*>(dfrType[t]->FindObject(Form("%s, pWeights %s",typeFlag[t].Data(),pWeightsFlag[pW].Data())));
      if(dfrParticleWeights[t][pW])
      {
       for(Int_t eW=0;eW<2;eW++)
       {
        dfrEventWeights[t][pW][eW] = dynamic_cast<TList*>(dfrParticleWeights[t][pW]->FindObject(Form("%s, pWeights %s, eWeights %s",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
        if(dfrEventWeights[t][pW][eW])
        {
         diffFlowFinalCorrelations[t][pW][eW] = dynamic_cast<TList*>(dfrEventWeights[t][pW][eW]->FindObject(Form("Correlations (%s, pWeights %s, eWeights %s)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
         diffFlowFinalCorrections[t][pW][eW] = dynamic_cast<TList*>(dfrEventWeights[t][pW][eW]->FindObject(Form("Corrections (%s, pWeights %s, eWeights %s)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));
         diffFlowFinalCovariances[t][pW][eW] = dynamic_cast<TList*>(dfrEventWeights[t][pW][eW]->FindObject(Form("Covariances (%s, pWeights %s, eWeights %s)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())));      
         for(Int_t nua=0;nua<2;nua++)
         {
          dfrCorrections[t][pW][eW][nua] = dynamic_cast<TList*>(dfrEventWeights[t][pW][eW]->FindObject(Form("%s, pWeights %s, eWeights %s, %s for NUA",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())));
          if(dfrCorrections[t][pW][eW][nua])
          {
           diffFlowFinalCumulants[t][pW][eW][nua] = dynamic_cast<TList*>(dfrCorrections[t][pW][eW][nua]->FindObject(Form("Cumulants (%s, pWeights %s, eWeights %s, %s for NUA)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())));
           diffFlowFinalFlow[t][pW][eW][nua] = dynamic_cast<TList*>(dfrCorrections[t][pW][eW][nua]->FindObject(Form("Differential Flow (%s, pWeights %s, eWeights %s, %s for NUA)",typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())));
          } else // to if(dfrCorrections[t][pW][eW][nua])
            {
             cout<<"WARNING: dfrCorrections[t][pW][eW][nua] is NULL in AFAWQC::GOH() !!!!"<<endl;
             cout<<"t = "<<t<<endl;
             cout<<"pW = "<<pW<<endl;
             cout<<"eW = "<<eW<<endl;
             cout<<"nua = "<<nua<<endl;
            }
         } // end of for(Int_t nua=0;nua<2;nua++)
        } else // to if(dfrEventWeights[t][pW][eW])
          {
           cout<<"WARNING: dfrEventWeights[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
           cout<<"t = "<<t<<endl;
           cout<<"pW = "<<pW<<endl;
           cout<<"eW = "<<eW<<endl;
          }
       } // end of for(Int_t eW=0;eW<2;eW++)
      } else // to if(dfrParticleWeights[t][pW])
        {
         cout<<"WARNING: dfrParticleWeights[t][pW] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
        }
     } // end of for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
    } else // to if(dfrType[t])
      {
       cout<<"WARNING: dfrType[t] is NULL in AFAWQC::GOH() !!!!"<<endl;
       cout<<"t = "<<t<<endl;
      }
   } // end of for(Int_t t=0;t<2;t++)
  } else // to if(diffFlowListResults)
    {
     cout<<"WARNING: diffFlowListResults is NULL in AFAWQC::GOH() !!!!"<<endl;
    }
 
 TH2D *finalCorrelations2D[2][2][2][4] = {{{{NULL}}}};
 TH1D *finalCorrelations1D[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH2D *finalCumulants2D[2][2][2][2][4] = {{{{{NULL}}}}};
 TH1D *finalCumulantsPt[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH1D *finalCumulantsEta[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH2D *finalCorrections2D[2][2][2][4] = {{{{NULL}}}};
 TH1D *finalCorrections1D[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH2D *finalCovariances2D[2][2][2][4] = {{{{NULL}}}};
 TH1D *finalCovariances1D[2][2][2][2][5] = {{{{{NULL}}}}}; 
 TH2D *finalFlow2D[2][2][2][2][4] = {{{{{NULL}}}}};
 TH1D *finalFlowPt[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH1D *finalFlowEta[2][2][2][2][4] = {{{{{NULL}}}}}; 
 TH2D *nonEmptyBins2D[2] = {NULL};
 //TH1D *nonEmptyBins1D[2][2] = {{NULL}};
 
 for(Int_t t=0;t<2;t++)
 {
  // 2D:
  nonEmptyBins2D[t] = dynamic_cast<TH2D*>(dfrType[t]->FindObject(Form("%s, (p_{t},#eta)",typeFlag[t].Data())));
  if(nonEmptyBins2D[t])
  {
   this->SetNonEmptyBins2D(nonEmptyBins2D[t],t);
  } else
    { 
     cout<<"WARNING: nonEmptyBins2D[t] is NULL in AFAWQC::GOH() !!!!"<<endl;
     cout<<"t = "<<t<<endl;
    }
  // 1D:
  
  
  
  
  
  */
  
  
  
  
  
  /*
  for(Int_t pe=0;pe<2;pe++)
  {
   nonEmptyBins1D[t][pe] = dynamic_cast<TH1D*>(dfrType[t]->FindObject(Form("%s, %s",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(nonEmptyBins1D[t][pe])
   {
    this->SetNonEmptyBins1D(nonEmptyBins1D[t][pe],t,pe);
   } else
     { 
      cout<<"WARNING: nonEmptyBins1D[t][pe] is NULL in AFAWQC::GOH() !!!!"<<endl;
      cout<<"t = "<<t<<endl;
      cout<<"pe = "<<pe<<endl;
     }
  } 
  */ 
  
  
  
  
  
  /*
  
  
  
  
  
  for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
  {
   for(Int_t eW=0;eW<2;eW++)
   {
    // correlations:
    if(diffFlowFinalCorrelations[t][pW][eW])
    {
     for(Int_t correlIndex=0;correlIndex<4;correlIndex++)
     {
      // 2D:
      finalCorrelations2D[t][pW][eW][correlIndex] = dynamic_cast<TH2D*>(diffFlowFinalCorrelations[t][pW][eW]->FindObject(Form("%s, (p_{t},#eta)",correlationName[correlIndex].Data())));
      if(finalCorrelations2D[t][pW][eW][correlIndex])
      {
       this->SetFinalCorrelations2D(finalCorrelations2D[t][pW][eW][correlIndex],t,pW,eW,correlIndex);
      } else
        {
         cout<<"WARNING: finalCorrelations2D[t][pW][eW][correlIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
         cout<<"eW = "<<eW<<endl;
         cout<<"ci = "<<correlIndex<<endl;
        }
       // 1D
       for(Int_t pe=0;pe<2;pe++)
       {
        if(pe==0)
        {
         finalCorrelations1D[t][pW][eW][pe][correlIndex] = dynamic_cast<TH1D*>(diffFlowFinalCorrelations[t][pW][eW]->FindObject(Form("%s, (p_{t})",correlationName[correlIndex].Data())));
        }
        if(pe==1)
        {
         finalCorrelations1D[t][pW][eW][pe][correlIndex] = dynamic_cast<TH1D*>(diffFlowFinalCorrelations[t][pW][eW]->FindObject(Form("%s, (#eta)",correlationName[correlIndex].Data())));
        }          
        if(finalCorrelations1D[t][pW][eW][pe][correlIndex])
        {
         this->SetFinalCorrelations1D(finalCorrelations1D[t][pW][eW][pe][correlIndex],t,pW,eW,pe,correlIndex);
        } else
          {
           cout<<"WARNING: finalCorrelations1D[t][pW][eW][pe][correlIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
           cout<<"t = "<<t<<endl;
           cout<<"pW = "<<pW<<endl;
           cout<<"eW = "<<eW<<endl;
           cout<<"pe = "<<pe<<endl;
           cout<<"ci = "<<correlIndex<<endl;
          }   
       } // end of for(Int_t pe=0;pe<2;pe++)  
     } // end of for(Int_t correlIndex=0;correlIndex<4;correlIndex++)
    } else // to if(diffFlowFinalCorrelations[t][pW][eW])
      { 
       cout<<"WARNING: diffFlowFinalCorrelations[t][pW] is NULL in AFAWQC::GOH() !!!!"<<endl;
       cout<<"t = "<<t<<endl;
       cout<<"pW = "<<pW<<endl;
       cout<<"eW = "<<eW<<endl;
      }
    // corrections:
    if(diffFlowFinalCorrections[t][pW][eW])
    {
     for(Int_t correctionIndex=0;correctionIndex<4;correctionIndex++)
     {
      // 2D:
      finalCorrections2D[t][pW][eW][correctionIndex] = dynamic_cast<TH2D*>(diffFlowFinalCorrections[t][pW][eW]->FindObject(Form("%s, (p_{t},#eta)",correctionName[correctionIndex].Data())));
      if(finalCorrections2D[t][pW][eW][correctionIndex])
      {
       this->SetFinalCorrections2D(finalCorrections2D[t][pW][eW][correctionIndex],t,pW,eW,correctionIndex);
      } else
        {
         cout<<"WARNING: finalCorrections2D[t][pW][eW][correctionIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
         cout<<"eW = "<<eW<<endl;
         cout<<"ci = "<<correctionIndex<<endl;
        }
      // 1D
      for(Int_t pe=0;pe<2;pe++)
      {
       if(pe==0)
       {
        finalCorrections1D[t][pW][eW][pe][correctionIndex] = dynamic_cast<TH1D*>(diffFlowFinalCorrections[t][pW][eW]->FindObject(Form("%s, (p_{t})",correctionName[correctionIndex].Data())));
       }
       if(pe==1)
       {
        finalCorrections1D[t][pW][eW][pe][correctionIndex] = dynamic_cast<TH1D*>(diffFlowFinalCorrections[t][pW][eW]->FindObject(Form("%s, (#eta)",correctionName[correctionIndex].Data())));
       }          
       if(finalCorrections1D[t][pW][eW][pe][correctionIndex])
       {
        this->SetFinalCorrections1D(finalCorrections1D[t][pW][eW][pe][correctionIndex],t,pW,eW,pe,correctionIndex);
       } else
         {
          cout<<"WARNING: finalCorrections1D[t][pW][eW][pe][correctionIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"pe = "<<pe<<endl;
          cout<<"ci = "<<correctionIndex<<endl;
         }   
      } // end of for(Int_t pe=0;pe<2;pe++)  
     } // end of for(Int_t correctionIndex=0;correctionIndex<4;correctionIndex++)
    } else // to if(diffFlowFinalCorrections[t][pW][eW])
      { 
       cout<<"WARNING: diffFlowFinalCorrections[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
       cout<<"t = "<<t<<endl;
       cout<<"pW = "<<pW<<endl;
       cout<<"eW = "<<eW<<endl;
      }     
    // covariances:
    if(diffFlowFinalCovariances[t][pW][eW])
    {
     for(Int_t covarianceIndex=0;covarianceIndex<5;covarianceIndex++)
     {
      // 2D:
      finalCovariances2D[t][pW][eW][covarianceIndex] = dynamic_cast<TH2D*>(diffFlowFinalCovariances[t][pW][eW]->FindObject(Form("%s, (p_{t},#eta)",covarianceName[covarianceIndex].Data())));
      if(finalCovariances2D[t][pW][eW][covarianceIndex])
      {
       this->SetFinalCovariances2D(finalCovariances2D[t][pW][eW][covarianceIndex],t,pW,eW,covarianceIndex);
      } else
        {
         cout<<"WARNING: finalCovariances2D[t][pW][eW][nua][covarianceIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
         cout<<"t = "<<t<<endl;
         cout<<"pW = "<<pW<<endl;
         cout<<"eW = "<<eW<<endl;
         cout<<"ci = "<<covarianceIndex<<endl;
        }
      // 1D:
      for(Int_t pe=0;pe<2;pe++)
      {
       if(pe==0)
       {
        finalCovariances1D[t][pW][eW][pe][covarianceIndex] = dynamic_cast<TH1D*>(diffFlowFinalCovariances[t][pW][eW]->FindObject(Form("%s, (p_{t})",covarianceName[covarianceIndex].Data())));
       }
       if(pe==1)
       {
        finalCovariances1D[t][pW][eW][pe][covarianceIndex] = dynamic_cast<TH1D*>(diffFlowFinalCovariances[t][pW][eW]->FindObject(Form("%s, (#eta)",covarianceName[covarianceIndex].Data())));
       }          
       if(finalCovariances1D[t][pW][eW][pe][covarianceIndex])
       {
        this->SetFinalCovariances1D(finalCovariances1D[t][pW][eW][pe][covarianceIndex],t,pW,eW,pe,covarianceIndex);
       } else
         {
          cout<<"WARNING: finalCovariances1D[t][pW][eW][pe][covarianceIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"pe = "<<pe<<endl;
          cout<<"ci = "<<covarianceIndex<<endl;
         }   
      } // end of for(Int_t pe=0;pe<2;pe++)  
     } // end of for(Int_t covarianceIndex=0;covarianceIndex<4;covarianceIndex++)
    } else // to if(diffFlowFinalCovariances[t][pW][eW])
      { 
       cout<<"WARNING: diffFlowFinalCovariances[t][pW][eW] is NULL in AFAWQC::GOH() !!!!"<<endl;
       cout<<"t = "<<t<<endl;
       cout<<"pW = "<<pW<<endl;
       cout<<"eW = "<<eW<<endl;
      }        
       
    for(Int_t nua=0;nua<2;nua++)
    {  
     // cumulants:
     if(diffFlowFinalCumulants[t][pW][eW][nua])
     {
      for(Int_t cumulantIndex=0;cumulantIndex<4;cumulantIndex++)
      {
       // 2D:
       finalCumulants2D[t][pW][eW][nua][cumulantIndex] = dynamic_cast<TH2D*>(diffFlowFinalCumulants[t][pW][eW][nua]->FindObject(Form("%s, (p_{t},#eta)",cumulantName[cumulantIndex].Data())));
       if(finalCumulants2D[t][pW][eW][nua][cumulantIndex])
       {
        this->SetFinalCumulants2D(finalCumulants2D[t][pW][eW][nua][cumulantIndex],t,pW,eW,nua,cumulantIndex);
       } else
         {
          cout<<"WARNING: finalCumulants2D[t][pW][eW][nua][cumulantIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<cumulantIndex<<endl;
         }
       // pt:  
       finalCumulantsPt[t][pW][eW][nua][cumulantIndex] = dynamic_cast<TH1D*>(diffFlowFinalCumulants[t][pW][eW][nua]->FindObject(Form("%s, (p_{t})",cumulantName[cumulantIndex].Data())));
       if(finalCumulantsPt[t][pW][eW][nua][cumulantIndex])
       {
        this->SetFinalCumulantsPt(finalCumulantsPt[t][pW][eW][nua][cumulantIndex],t,pW,eW,nua,cumulantIndex);
       } else
         {
          cout<<"WARNING: finalCumulantsPt[t][pW][eW][nua][cumulantIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<cumulantIndex<<endl;
         } 
       // eta:
       finalCumulantsEta[t][pW][eW][nua][cumulantIndex] = dynamic_cast<TH1D*>(diffFlowFinalCumulants[t][pW][eW][nua]->FindObject(Form("%s, (#eta)",cumulantName[cumulantIndex].Data())));
       if(finalCumulantsEta[t][pW][eW][nua][cumulantIndex])
       {
        this->SetFinalCumulantsEta(finalCumulantsEta[t][pW][eW][nua][cumulantIndex],t,pW,eW,nua,cumulantIndex);
       } else
         {
          cout<<"WARNING: finalCumulantsEta[t][pW][eW][nua][cumulantIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<cumulantIndex<<endl;
         }   
      } // end of for(Int_t cumulantIndex=0;cumulantIndex<4;cumulantIndex++)
     } else // to if(diffFlowFinalCumulants[t][pW][eW][nua])
       { 
        cout<<"WARNING: diffFlowFinalCumulants[t][pW][eW][nua] is NULL in AFAWQC::GOH() !!!!"<<endl;
        cout<<"t = "<<t<<endl;
        cout<<"pW = "<<pW<<endl;
        cout<<"eW = "<<eW<<endl;
        cout<<"nua = "<<nua<<endl;
       }      
     // flow:
     if(diffFlowFinalFlow[t][pW][eW][nua])
     {
      for(Int_t flowIndex=0;flowIndex<4;flowIndex++)
      {
       // 2D:
       finalFlow2D[t][pW][eW][nua][flowIndex] = dynamic_cast<TH2D*>(diffFlowFinalFlow[t][pW][eW][nua]->FindObject(Form("%s, (p_{t},#eta)",flowName[flowIndex].Data())));
       if(finalFlow2D[t][pW][eW][nua][flowIndex])
       {
        this->SetFinalFlow2D(finalFlow2D[t][pW][eW][nua][flowIndex],t,pW,eW,nua,flowIndex);
       } else
         {
          cout<<"WARNING: finalFlow2D[t][pW][eW][nua][flowIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<flowIndex<<endl;
         }
       // pt:
       finalFlowPt[t][pW][eW][nua][flowIndex] = dynamic_cast<TH1D*>(diffFlowFinalFlow[t][pW][eW][nua]->FindObject(Form("%s, (p_{t})",flowName[flowIndex].Data())));
       if(finalFlowPt[t][pW][eW][nua][flowIndex])
       {
        this->SetFinalFlowPt(finalFlowPt[t][pW][eW][nua][flowIndex],t,pW,eW,nua,flowIndex);
       } else
         {
          cout<<"WARNING: finalFlow1D[t][pW][nua][flowIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<flowIndex<<endl;
         }   
       // eta: 
       finalFlowEta[t][pW][eW][nua][flowIndex] = dynamic_cast<TH1D*>(diffFlowFinalFlow[t][pW][eW][nua]->FindObject(Form("%s, (#eta)",flowName[flowIndex].Data())));          
       if(finalFlowEta[t][pW][eW][nua][flowIndex])
       {
        this->SetFinalFlowEta(finalFlowEta[t][pW][eW][nua][flowIndex],t,pW,eW,nua,flowIndex);
       } else
         {
          cout<<"WARNING: finalFlow1D[t][pW][nua][flowIndex] is NULL in AFAWQC::GOH() !!!!"<<endl;
          cout<<"t = "<<t<<endl;
          cout<<"pW = "<<pW<<endl;
          cout<<"eW = "<<eW<<endl;
          cout<<"nua = "<<nua<<endl;
          cout<<"ci = "<<flowIndex<<endl;
         }   
      } // end of for(Int_t flowIndex=0;flowIndex<4;flowIndex++)
     } else // to if(diffFlowFinalFlow[t][pW][eW][nua])
       { 
        cout<<"WARNING: diffFlowFinalFlow[t][pW][eW][nua] is NULL in AFAWQC::GOH() !!!!"<<endl;
        cout<<"t = "<<t<<endl;
        cout<<"pW = "<<pW<<endl;
        cout<<"eW = "<<eW<<endl;
        cout<<"nua = "<<nua<<endl;
       }               
    } // end of for(Int_t nua=0;nua<2;nua++)
   } // end of for(Int_t eW=0;eW<2;eW++) 
  } // end of for(Int_t pW=0;pW<(1+(Int_t)(bUsePhiWeights||bUsePtWeights||bUseEtaWeights));pW++)
 } // end of for(Int_t t=0;t<2;t++)
 
  // x.) nested loops:
  TList *nestedLoopsList = dynamic_cast<TList*>(outputListHistos->FindObject("Nested Loops"));
  if(nestedLoopsList) this->SetNestedLoopsList(nestedLoopsList);
  TProfile *evaluateNestedLoops = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fEvaluateNestedLoops"));
  Bool_t bEvaluateNestedLoopsForIntFlow = kFALSE;
  Bool_t bEvaluateNestedLoopsForDiffFlow = kFALSE;
  if(evaluateNestedLoops)
  {
   this->SetEvaluateNestedLoops(evaluateNestedLoops);
   bEvaluateNestedLoopsForIntFlow = (Int_t)evaluateNestedLoops->GetBinContent(1);
   bEvaluateNestedLoopsForDiffFlow = (Int_t)evaluateNestedLoops->GetBinContent(2);
  }
  
  if(bEvaluateNestedLoopsForIntFlow)
  {
   TProfile *directCorrelations = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelation"));
   if(directCorrelations) this->SetDirectCorrelations(directCorrelations);
   TProfile *directCorrectionsCos = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsCos"));
   if(directCorrectionsCos) this->SetDirectCorrectionsCos(directCorrectionsCos);
   TProfile *directCorrectionsSin = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsSin"));
   if(directCorrectionsSin) this->SetDirectCorrectionsSin(directCorrectionsSin);
   if(bUsePhiWeights||bUsePtWeights||bUseEtaWeights)
   {
    TProfile *directCorrelationsW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationW"));
    if(directCorrelationsW) this->SetDirectCorrelationsW(directCorrelationsW);
    TProfile *directCorrectionsCosW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsCosW"));
    if(directCorrectionsCosW) this->SetDirectCorrectionsCosW(directCorrectionsCosW);
    TProfile *directCorrectionsSinW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsSinW")); 
    if(directCorrectionsSinW) this->SetDirectCorrectionsSinW(directCorrectionsSinW);
   }
  }
  
  if(bEvaluateNestedLoopsForDiffFlow)
  {
   TProfile *directCorrelationsDiffFlow = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationsDiffFlow"));
   if(directCorrelationsDiffFlow) this->SetDirectCorrelationsDiffFlow(directCorrelationsDiffFlow);
   TProfile *directCorrectionsDiffFlowCos = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowCos"));
   if(directCorrectionsDiffFlowCos) this->SetDirectCorrectionsDiffFlowCos(directCorrectionsDiffFlowCos);
   TProfile *directCorrectionsDiffFlowSin = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowSin"));
   if(directCorrectionsDiffFlowSin) this->SetDirectCorrectionsDiffFlowSin(directCorrectionsDiffFlowSin);
   if(bUsePhiWeights||bUsePtWeights||bUseEtaWeights)
   {
    TProfile *directCorrelationsDiffFlowW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationsDiffFlowW")); 
    if(directCorrelationsDiffFlowW) this->SetDirectCorrelationsDiffFlowW(directCorrelationsDiffFlowW);
    TProfile *directCorrectionsDiffFlowCosW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowCosW"));
    if(directCorrectionsDiffFlowCosW) this->SetDirectCorrectionsDiffFlowCosW(directCorrectionsDiffFlowCosW);
    TProfile *directCorrectionsDiffFlowSinW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowSinW"));
    if(directCorrectionsDiffFlowSinW) this->SetDirectCorrectionsDiffFlowSinW(directCorrectionsDiffFlowSinW);
   }
  }
  
 } 
 
 */
 

 
}


//================================================================================================================================


TProfile* AliFlowAnalysisWithQCumulants::MakePtProjection(TProfile2D *profilePtEta) const
{
 // project 2D profile onto pt axis to get 1D profile
 
 Int_t nBinsPt   = profilePtEta->GetNbinsX();
 Double_t dPtMin = (profilePtEta->GetXaxis())->GetXmin();
 Double_t dPtMax = (profilePtEta->GetXaxis())->GetXmax();
 
 Int_t nBinsEta   = profilePtEta->GetNbinsY();
 
 TProfile *profilePt = new TProfile("","",nBinsPt,dPtMin,dPtMax); 
 
 for(Int_t p=1;p<=nBinsPt;p++)
 {
  Double_t contentPt = 0.;
  Double_t entryPt = 0.;
  Double_t spreadPt = 0.;
  Double_t sum1 = 0.;
  Double_t sum2 = 0.;
  Double_t sum3 = 0.;
  for(Int_t e=1;e<=nBinsEta;e++)
  {
   contentPt += (profilePtEta->GetBinContent(profilePtEta->GetBin(p,e)))
              * (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)));
   entryPt   += (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)));
   
   sum1 += (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)))
         * (pow(profilePtEta->GetBinError(profilePtEta->GetBin(p,e)),2.)
            + pow(profilePtEta->GetBinContent(profilePtEta->GetBin(p,e)),2.)); 
   sum2 += (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)));
   sum3 += (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)))
         * (profilePtEta->GetBinContent(profilePtEta->GetBin(p,e)));            
  }
  if(sum2>0. && sum1/sum2-pow(sum3/sum2,2.) > 0.)
  {
   spreadPt = pow(sum1/sum2-pow(sum3/sum2,2.),0.5);
  }
  profilePt->SetBinContent(p,contentPt);
  profilePt->SetBinEntries(p,entryPt);
  {
   profilePt->SetBinError(p,spreadPt);
  }
  
 }
 
 return profilePt;
 
} // end of TProfile* AliFlowAnalysisWithQCumulants::MakePtProjection(TProfile2D *profilePtEta)


//================================================================================================================================


TProfile* AliFlowAnalysisWithQCumulants::MakeEtaProjection(TProfile2D *profilePtEta) const
{
 // project 2D profile onto eta axis to get 1D profile
 
 Int_t nBinsEta   = profilePtEta->GetNbinsY();
 Double_t dEtaMin = (profilePtEta->GetYaxis())->GetXmin();
 Double_t dEtaMax = (profilePtEta->GetYaxis())->GetXmax();
 
 Int_t nBinsPt = profilePtEta->GetNbinsX();
 
 TProfile *profileEta = new TProfile("","",nBinsEta,dEtaMin,dEtaMax); 
 
 for(Int_t e=1;e<=nBinsEta;e++)
 {
  Double_t contentEta = 0.;
  Double_t entryEta = 0.;
  for(Int_t p=1;p<=nBinsPt;p++)
  {
   contentEta += (profilePtEta->GetBinContent(profilePtEta->GetBin(p,e)))
              * (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)));
   entryEta   += (profilePtEta->GetBinEntries(profilePtEta->GetBin(p,e)));
  }
  profileEta->SetBinContent(e,contentEta);
  profileEta->SetBinEntries(e,entryEta);
 }
 
 return profileEta;
 
} // end of TProfile* AliFlowAnalysisWithQCumulants::MakeEtaProjection(TProfile2D *profilePtEta)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateFinalCorrectionsForNonUniformAcceptanceForDifferentialFlow(Bool_t useParticleWeights,TString type)
{
 
 useParticleWeights=kFALSE;
 type="ac";
  
 // calculate final corrections due to non-uniform acceptance of the detector to reduced multi-particle correlations
 /*
 if(!(useParticleWeights))
 {
  if(type == "POI")
  { 
   // **** corrections for non-uniform acceptance for 2nd order QC' for POI's ****
   
   // 1st term: <<cos(n*psi)>><<cos(n*phi)>>:
   if(fCorrectionsCosP1nPsiPtEtaPOI && fQCorrectionsCos)
   {
    // pt,eta: 
    if(f2pFinalCorrectionsForNUAPtEtaPOI) f2pFinalCorrectionsForNUAPtEtaPOI->Reset(); // to be improved
    TH2D *correctionPtEta1stTerm = new TH2D(*(fCorrectionsCosP1nPsiPtEtaPOI->ProjectionXY("","e")));
    correctionPtEta1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here?   
    if(f2pFinalCorrectionsForNUAPtEtaPOI) f2pFinalCorrectionsForNUAPtEtaPOI->Add(correctionPtEta1stTerm); // to be improved (if condition goes somewhere else)
    delete correctionPtEta1stTerm;
    // pt:
    if(f2pFinalCorrectionsForNUAPtPOI) f2pFinalCorrectionsForNUAPtPOI->Reset(); // to be improved
    TH1D *correctionPt1stTerm = new TH1D(*((this->MakePtProjection(fCorrectionsCosP1nPsiPtEtaPOI))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
    correctionPt1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
    if(f2pFinalCorrectionsForNUAPtPOI) f2pFinalCorrectionsForNUAPtPOI->Add(correctionPt1stTerm); // to be improved (if condition goes somewhere else)
    delete correctionPt1stTerm;
    // eta:
    if(f2pFinalCorrectionsForNUAEtaPOI) f2pFinalCorrectionsForNUAEtaPOI->Reset(); // to be improved    
    TH1D *correctionEta1stTerm = new TH1D(*((this->MakeEtaProjection(fCorrectionsCosP1nPsiPtEtaPOI))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
    correctionEta1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
    if(f2pFinalCorrectionsForNUAEtaPOI) f2pFinalCorrectionsForNUAEtaPOI->Add(correctionEta1stTerm); // to be improved (if condition goes somewhere else)
    delete correctionEta1stTerm;    
   } else
     { 
      cout<<"WARNING: (fCorrectionsCosP1nPsiPtEtaPOI && fQCorrectionsCos && f2pFinalCorrectionsForNUAPtEtaPOI) is NULL in QC::CFCFNUAFDF() !!!!  "<<endl;
      cout<<"         Corrections for non-uniform acceptance for differential flow are not correct."<<endl;
     } 
     
   // 2nd term: <<sin(n*psi)>><<sin(n*phi)>>:  
   if(fCorrectionsSinP1nPsiPtEtaPOI && fQCorrectionsSin)
   {
    // pt,eta:
    TH2D *correctionPtEta2ndTerm = new TH2D(*(fCorrectionsSinP1nPsiPtEtaPOI->ProjectionXY("","e")));
    correctionPtEta2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here?    
    if(f2pFinalCorrectionsForNUAPtEtaPOI) f2pFinalCorrectionsForNUAPtEtaPOI->Add(correctionPtEta2ndTerm); // to be improved (if condition goes somewhere else)
    delete correctionPtEta2ndTerm;
    // pt:
    TH1D *correctionPt2ndTerm = new TH1D(*((this->MakePtProjection(fCorrectionsSinP1nPsiPtEtaPOI))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
    correctionPt2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
    if(f2pFinalCorrectionsForNUAPtPOI) f2pFinalCorrectionsForNUAPtPOI->Add(correctionPt2ndTerm); // to be improved (if condition goes somewhere else)
    delete correctionPt2ndTerm;
    // eta:
    TH1D *correctionEta2ndTerm = new TH1D(*((this->MakeEtaProjection(fCorrectionsSinP1nPsiPtEtaPOI))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
    correctionEta2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
    if(f2pFinalCorrectionsForNUAEtaPOI) f2pFinalCorrectionsForNUAEtaPOI->Add(correctionEta2ndTerm); // to be improved (if condition goes somewhere else)
    delete correctionEta2ndTerm; 
   } else
     { 
      cout<<"WARNING: (fCorrectionsSinP1nPsiPtEtaPOI && fQCorrectionsSin) is NULL in QC::CFCFNUAFDF() !!!!  "<<endl;
      cout<<"         Corrections for non-uniform acceptance for differential flow are not correct."<<endl;
     } 
  } else if(type == "RP")
    {
     // **** corrections for non-uniform acceptance for 2nd order QC' for RP's ****
   
     // 1st term: <<cos(n*psi)>><<cos(n*phi)>>:
     if(fCorrectionsCosP1nPsiPtEtaRP && fQCorrectionsCos)
     {
      // pt,eta: 
      if(f2pFinalCorrectionsForNUAPtEtaRP) f2pFinalCorrectionsForNUAPtEtaRP->Reset(); // to be improved
      TH2D *correctionPtEta1stTerm = new TH2D(*(fCorrectionsCosP1nPsiPtEtaRP->ProjectionXY("","e")));
      correctionPtEta1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here?    
      if(f2pFinalCorrectionsForNUAPtEtaRP) f2pFinalCorrectionsForNUAPtEtaRP->Add(correctionPtEta1stTerm); // to be improved (if condition goes somewhere else)
      delete correctionPtEta1stTerm;
      // pt:
      if(f2pFinalCorrectionsForNUAPtRP) f2pFinalCorrectionsForNUAPtRP->Reset(); // to be improved
      TH1D *correctionPt1stTerm = new TH1D(*((this->MakePtProjection(fCorrectionsCosP1nPsiPtEtaRP))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
      correctionPt1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
      if(f2pFinalCorrectionsForNUAPtRP) f2pFinalCorrectionsForNUAPtRP->Add(correctionPt1stTerm); // to be improved (if condition goes somewhere else)
      delete correctionPt1stTerm;
      // eta:
      if(f2pFinalCorrectionsForNUAEtaRP) f2pFinalCorrectionsForNUAEtaRP->Reset(); // to be improved
      TH1D *correctionEta1stTerm = new TH1D(*((this->MakeEtaProjection(fCorrectionsCosP1nPsiPtEtaRP))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
      correctionEta1stTerm->Scale(fQCorrectionsCos->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
      if(f2pFinalCorrectionsForNUAEtaRP) f2pFinalCorrectionsForNUAEtaRP->Add(correctionEta1stTerm); // to be improved (if condition goes somewhere else)
      delete correctionEta1stTerm;    
     } else
       { 
        cout<<"WARNING: (fCorrectionsCosP1nPsiPtEtaRP && fQCorrectionsCos) is NULL in QC::CFCFNUAFDF() !!!!  "<<endl;
        cout<<"         Corrections for non-uniform acceptance for differential flow are not correct."<<endl;
       } 
     // 2nd term: <<sin(n*psi)>><<sin(n*phi)>>:  
     if(fCorrectionsSinP1nPsiPtEtaRP && fQCorrectionsSin)
     {
      // pt,eta: 
      TH2D *correctionPtEta2ndTerm = new TH2D(*(fCorrectionsSinP1nPsiPtEtaRP->ProjectionXY("","e")));
      correctionPtEta2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here?    
      if(f2pFinalCorrectionsForNUAPtEtaRP) f2pFinalCorrectionsForNUAPtEtaRP->Add(correctionPtEta2ndTerm); // to be improved (if condition goes somewhere else)
      delete correctionPtEta2ndTerm;
      // pt:
      TH1D *correctionPt2ndTerm = new TH1D(*((this->MakePtProjection(fCorrectionsSinP1nPsiPtEtaRP))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
      correctionPt2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
      if(f2pFinalCorrectionsForNUAPtRP) f2pFinalCorrectionsForNUAPtRP->Add(correctionPt2ndTerm); // to be improved (if condition goes somewhere else)
      delete correctionPt2ndTerm;
      // eta:
      TH1D *correctionEta2ndTerm = new TH1D(*((this->MakeEtaProjection(fCorrectionsSinP1nPsiPtEtaRP))->ProjectionX("","e"))); // to be improved: are errors propagated correctly here? 
      correctionEta2ndTerm->Scale(fQCorrectionsSin->GetBinContent(1)); // to be improved: are errors propagated correctly here? 
      if(f2pFinalCorrectionsForNUAEtaRP) f2pFinalCorrectionsForNUAEtaRP->Add(correctionEta2ndTerm); // to be improved (if condition goes somewhere else)
      delete correctionEta2ndTerm; 
     } else
       { 
        cout<<"WARNING: (fCorrectionsSinP1nPsiPtEtaRP && fQCorrectionsSin) is NULL in QC::CFCFNUAFDF() !!!!  "<<endl;
        cout<<"         Corrections for non-uniform acceptance for differential flow are not correct."<<endl;
       }              
    } else // to else if(type == "RP")
      {
       cout<<"WARNING: Type must be either POI or RP in QC::CFCFNUAFDF() !!!!                           "<<endl;
       cout<<"         Corrections for non-uniform acceptance for differential flow were not calculated."<<endl;
      }  
 } else // to if(!(useParticleWeights))
   {
    // ...
   }
 */
} // end of AliFlowAnalysisWithQCumulants::CalculateFinalCorrectionsForNonUniformAcceptanceForDifferentialFlow(Bool_t useParticleWeights,TString type)


//==================================================================================================================================

/*
void AliFlowAnalysisWithQCumulants::CalculateFinalResultsForDifferentialFlow(
                                                        TH2D *flowPtEta, TH1D *flowPt, TH1D *flowEta, 
                                                        TProfile2D *profile2ndPtEta, TProfile2D *profile4thPtEta, 
                                                        TProfile2D *profile6thPtEta, TProfile2D *profile8thPtEta)
{
 // calculate and store the final results for integrated flow
 
 TString *namePtEta = new TString();
 TString *type = new TString();
 TString *order2nd = new TString();
 TString *order4th = new TString();
 TString *order6th = new TString();
 TString *order8th = new TString(); 
 TString *pW = new TString();

 if(profile2ndPtEta) *namePtEta = profile2ndPtEta->GetName();
 if(namePtEta->Contains("POI")) *type = "POI";
 if(namePtEta->Contains("RP")) *type  = "RP";
 if(namePtEta->Contains("W")) *pW      = "W";
 if(namePtEta->Contains("2")) *order2nd  = "2";
 if(profile4thPtEta) *namePtEta = profile4thPtEta->GetName();
 if(namePtEta->Contains("4")) *order4th = "4";

 if(profile6thPtEta) *namePtEta = profile6thPtEta->GetName();
 if(namePtEta->Contains("6")) *order6th = "6";
 
 if(profile8thPtEta) *namePtEta = profile8thPtEta->GetName();
 if(namePtEta->Contains("8")) *order8th = "8";

 TProfile *profile2ndPt = NULL;
 TProfile *profile4thPt = NULL;
 TProfile *profile6thPt = NULL;
 TProfile *profile8thPt = NULL;

 TProfile *profile2ndEta = NULL;
 TProfile *profile4thEta = NULL;
 TProfile *profile6thEta = NULL;
 TProfile *profile8thEta = NULL;
  
 if(*order2nd == "2")
 {
  profile2ndPt  = new TProfile(*(this->MakePtProjection(profile2ndPtEta))); 
  profile2ndEta = new TProfile(*(this->MakeEtaProjection(profile2ndPtEta))); 
  if(*order4th == "4")
  {
   profile4thPt  = new TProfile(*(this->MakePtProjection(profile4thPtEta))); 
   profile4thEta = new TProfile(*(this->MakeEtaProjection(profile4thPtEta))); 
   if(*order6th == "6")
   {
    profile6thPt  = new TProfile(*(this->MakePtProjection(profile6thPtEta))); 
    profile6thEta = new TProfile(*(this->MakeEtaProjection(profile6thPtEta))); 
    if(*order8th == "8")
    {
     profile8thPt  = new TProfile(*(this->MakePtProjection(profile8thPtEta))); 
     profile8thEta = new TProfile(*(this->MakeEtaProjection(profile8thPtEta))); 
    }     
   }    
  } 
 }
 
 Int_t nBinsPt  = profile2ndPt->GetNbinsX();
 Int_t nBinsEta = profile2ndEta->GetNbinsX();
 
 Double_t dV2 = 0.;
 Double_t dV4 = 0.;
 Double_t dV6 = 0.;
 Double_t dV8 = 0.; 
 
 if(!(*pW == "W"))
 {
  dV2 = fIntFlowResultsQC->GetBinContent(1); 
  dV4 = fIntFlowResultsQC->GetBinContent(2); 
  dV6 = fIntFlowResultsQC->GetBinContent(3); 
  dV8 = fIntFlowResultsQC->GetBinContent(4); 
 } 
 else if(*pW == "W")
 {
  dV2 = fIntFlowResultsQCW->GetBinContent(1);  
  dV4 = fIntFlowResultsQCW->GetBinContent(2); 
  dV6 = fIntFlowResultsQCW->GetBinContent(3); 
  dV8 = fIntFlowResultsQCW->GetBinContent(4); 
 }    
 
 // 3D (pt,eta): 
 Double_t twoPrimePtEta   = 0.; // <<2'>> (pt,eta) 
 Double_t fourPrimePtEta  = 0.; // <<4'>> (pt,eta)  
 //Double_t sixPrimePtEta   = 0.; // <<6'>> (pt,eta) 
 //Double_t eightPrimePtEta = 0.; // <<8'>> (pt,eta) 
 Double_t secondOrderDiffFlowCumulantPtEta = 0.; // d_n{2,Q} (pt,eta)
 Double_t fourthOrderDiffFlowCumulantPtEta = 0.; // d_n{4,Q} (pt,eta) 
 //Double_t sixthOrderDiffFlowCumulantPtEta = 0.; // d_n{6,Q} (pt,eta)
 //Double_t eightOrderDiffFlowCumulantPtEta = 0.; // d_n{8,Q} (pt,eta)2nd
 Double_t dv2PtEta = 0.; // v'_n{2} (pt,eta) 
 Double_t dv4PtEta = 0.; // v'_n{4} (pt,eta) 
 //Double_t dv6PtEta = 0.; // v'_n{6} (pt,eta) 
 //Double_t dv8PtEta = 0.; // v'_n{8} (pt,eta)  

 // 2D (pt):   
 Double_t twoPrimePt   = 0.; // <<2'>> (pt)  
 Double_t fourPrimePt  = 0.; // <<4'>> (pt) 
 //Double_t sixPrimePt   = 0.; // <<6'>> (pt) 
 //Double_t eightPrimePt = 0.; // <<8'>> (pt)          
 Double_t secondOrderDiffFlowCumulantPt = 0.; // d_n{2,Q} (pt) 
 Double_t fourthOrderDiffFlowCumulantPt = 0.; // d_n{4,Q} (pt)  
 //Double_t sixthOrderDiffFlowCumulantPt = 0.; // d_n{6,Q} (pt)
 //Double_t eightOrderDiffFlowCumulantPt = 0.; // d_n{8,Q} (pt)
 Double_t dv2Pt = 0.; // v'_n{2} (pt)
 Double_t dv4Pt = 0.; // v'_n{4} (pt)
 //Double_t dv6Pt = 0.; // v'_n{6} (pt) 
 //Double_t dv8Pt = 0.; // v'_n{8} (pt)  

 // 2D (eta):           
 Double_t twoPrimeEta   = 0.; // <<2'>> (eta)  
 Double_t fourPrimeEta  = 0.; // <<4>> (eta) 
 //Double_t sixPrimeEta   = 0.; // <<6>> (eta) 
 //Double_t eightPrimeEta = 0.; // <<8'>> (eta)  
 Double_t secondOrderDiffFlowCumulantEta = 0.; // d_n{2,Q} (eta)
 Double_t fourthOrderDiffFlowCumulantEta = 0.; // d_n{4,Q} (eta) 
 //Double_t sixthOrderDiffFlowCumulantEta = 0.; // d_n{6,Q} (eta) 
 //Double_t eightOrderDiffFlowCumulantEta = 0.; // d_n{8,Q} (eta) 
 Double_t dv2Eta = 0.; // v'_n{2} (eta)
 Double_t dv4Eta = 0.; // v'_n{4} (eta)
 //Double_t dv6Eta = 0.; // v'_n{6} (eta) 
 //Double_t dv8Eta = 0.; // v'_n{8} (eta)
 

 // looping over (pt,eta) bins to calculate v'(pt,eta) 
 for(Int_t p=1;p<nBinsPt+1;p++)
 {
  for(Int_t e=1;e<nBinsEta+1;e++)
  {
  
   // 2nd order: 
   twoPrimePtEta = profile2ndPtEta->GetBinContent(profile2ndPtEta->GetBin(p,e));
   secondOrderDiffFlowCumulantPtEta = twoPrimePtEta;
   

   //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   // to be improved (applying correction for NUA):
   if(namePtEta->Contains("POI"))
   {
    if(f2pFinalCorrectionsForNUAPtEtaPOI) secondOrderDiffFlowCumulantPtEta = twoPrimePtEta 
                                     - f2pFinalCorrectionsForNUAPtEtaPOI->GetBinContent(f2pFinalCorrectionsForNUAPtEtaPOI->GetBin(p,e)) ;
   } else if (namePtEta->Contains("RP"))
     {  
      if(f2pFinalCorrectionsForNUAPtEtaRP) secondOrderDiffFlowCumulantPtEta = twoPrimePtEta 
                                       - f2pFinalCorrectionsForNUAPtEtaRP->GetBinContent(f2pFinalCorrectionsForNUAPtEtaRP->GetBin(p,e));
     }
   //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   
   
   if(dV2)
   {
    dv2PtEta = secondOrderDiffFlowCumulantPtEta/dV2;
    if(*order2nd == "2") 
    {
     flowPtEta->SetBinContent(p,e,dv2PtEta);   
    } 
   }
   
   // 4th order: 
   if(*order4th == "4" || *order6th == "6" || *order8th == "8")
   {
    fourPrimePtEta = profile4thPtEta->GetBinContent(profile4thPtEta->GetBin(p,e));
    fourthOrderDiffFlowCumulantPtEta = fourPrimePtEta - 2.*twoPrimePtEta*pow(dV2,2.); // to be improved (correlations instead of pow(dV2,2.))
    if(dV4)
    {
     dv4PtEta = -fourthOrderDiffFlowCumulantPtEta/pow(dV4,3);
     if(*order4th == "4")
     {
      flowPtEta->SetBinContent(p,e,dv4PtEta);
     } 
    }
   }    
   
  } // end of for(Int_t e=1;e<nBinsEta+1;e++)
 } // end of for(Int_t p=1;p<nBinsPt+1;p++) 
   
   
 // looping over (pt) bins to calcualate v'(pt)
 for(Int_t p=1;p<nBinsPt+1;p++)
 {
 
  // 2nd order: 
  twoPrimePt = profile2ndPt->GetBinContent(p);
  secondOrderDiffFlowCumulantPt = twoPrimePt;
  
  
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  // to be improved (applying correction for NUA):
  if(namePtEta->Contains("POI"))
  {
   if(f2pFinalCorrectionsForNUAPtPOI) secondOrderDiffFlowCumulantPt = twoPrimePt
                                    - f2pFinalCorrectionsForNUAPtPOI->GetBinContent(p) ;
  } else if (namePtEta->Contains("RP"))
    {
     if(f2pFinalCorrectionsForNUAPtRP) secondOrderDiffFlowCumulantPt = twoPrimePt
                                      - f2pFinalCorrectionsForNUAPtRP->GetBinContent(p);
    }
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  
  
  if(dV2)
  {
   dv2Pt = secondOrderDiffFlowCumulantPt/dV2;
   if(*order2nd == "2") 
   {
    flowPt->SetBinContent(p,dv2Pt);
   }
   
   // common control histos: (to be improved fill only once. now they are filled first without weights and then with weights):
   if(namePtEta->Contains("POI") && *order2nd == "2")
   {
    fCommonHistsResults2nd->FillDifferentialFlowPtPOI(p,dv2Pt,0.); //to be improved (errors && bb or bb+1 ?)
   } 
   else if(namePtEta->Contains("RP") && *order2nd == "2")
   {
    fCommonHistsResults2nd->FillDifferentialFlowPtRP(p,dv2Pt,0.); //to be improved (errors && bb or bb+1 ?)
   }
   
  }
  
  // 4th order: 
  if(*order4th == "4" || *order6th == "6" || *order8th == "8")
  {
   fourPrimePt = profile4thPt->GetBinContent(profile4thPt->GetBin(p));
   fourthOrderDiffFlowCumulantPt = fourPrimePt - 2.*twoPrimePt*pow(dV2,2.); // to be improved (correlations instead of pow(dV2,2.))
   if(dV4)
   {
    dv4Pt = -fourthOrderDiffFlowCumulantPt/pow(dV4,3);
    if(*order4th == "4") 
    {
     flowPt->SetBinContent(p,dv4Pt);
    }
    
    // common control histos: (to be improved):
    if(namePtEta->Contains("POI") && *order4th == "4")
    {
     fCommonHistsResults4th->FillDifferentialFlowPtPOI(p,dv4Pt,0.); //to be improved (errors && bb or bb+1 ?)
    } 
    else if(namePtEta->Contains("RP") && *order4th == "4" )
    {
     fCommonHistsResults4th->FillDifferentialFlowPtRP(p,dv4Pt,0.); //to be improved (errors && bb or bb+1 ?)
    }
        
   }
  }    
  
 } // end of for(Int_t p=1;p<nBinsPt+1;p++)  
 
 
 // looping over (eta) bins to calcualate v'(eta)
 for(Int_t e=1;e<nBinsEta+1;e++)
 {
 
  // 2nd order: 
  twoPrimeEta = profile2ndEta->GetBinContent(e);
  secondOrderDiffFlowCumulantEta = twoPrimeEta;
  
  
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  // to be improved (applying correction for NUA):
  if(namePtEta->Contains("POI"))
  {
   if(f2pFinalCorrectionsForNUAEtaPOI) secondOrderDiffFlowCumulantEta = twoPrimeEta
                                    - f2pFinalCorrectionsForNUAEtaPOI->GetBinContent(e) ;
  } else if (namePtEta->Contains("RP"))
    {
     if(f2pFinalCorrectionsForNUAEtaRP) secondOrderDiffFlowCumulantEta = twoPrimeEta
                                      - f2pFinalCorrectionsForNUAEtaRP->GetBinContent(e);
    }
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  
  
  if(dV2)
  {
   dv2Eta = secondOrderDiffFlowCumulantEta/dV2;
   if(*order2nd == "2") 
   {
    flowEta->SetBinContent(e,dv2Eta);
   }
   
   // common control histos: (to be improved):
   if(namePtEta->Contains("POI") && *order2nd == "2")
   {
    fCommonHistsResults2nd->FillDifferentialFlowEtaPOI(e,dv2Eta,0.); //to be improved (errors && bb or bb+1 ?)
   } 
   else if(namePtEta->Contains("RP") && *order2nd == "2")
   {
    fCommonHistsResults2nd->FillDifferentialFlowEtaRP(e,dv2Eta,0.); //to be improved (errors && bb or bb+1 ?)
   }
     

  }
  
  // 4th order: 
  if(*order4th == "4" || *order6th == "6" || *order8th == "8")
  {
   fourPrimeEta = profile4thEta->GetBinContent(profile4thEta->GetBin(e));
   fourthOrderDiffFlowCumulantEta = fourPrimeEta - 2.*twoPrimeEta*pow(dV2,2.); // to be improved (correlations instead of pow(dV2,2.))
   if(dV4)
   {
    dv4Eta = -fourthOrderDiffFlowCumulantEta/pow(dV4,3);
    if(*order4th == "4")
    {
     flowEta->SetBinContent(e,dv4Eta);
    }
    
    // common control histos: (to be improved):
    if(namePtEta->Contains("POI") && *order4th == "4")
    {
     fCommonHistsResults4th->FillDifferentialFlowEtaPOI(e,dv4Eta,0.); //to be improved (errors && bb or bb+1 ?)
    } 
    else if(namePtEta->Contains("RP") && *order4th == "4")
    {
     fCommonHistsResults4th->FillDifferentialFlowEtaRP(e,dv4Eta,0.); //to be improved (errors && bb or bb+1 ?)
    }
   
   }
  }    
  
 } // end of for(Int_t e=1;e<nBinsEta+1;e++)    
    
 delete namePtEta;
 delete type;
 delete order2nd;
 delete order4th;
 delete order6th;
 delete order8th;
 delete pW;
 delete profile2ndPt;
 delete profile4thPt;
 delete profile6thPt;
 delete profile8thPt;
 delete profile2ndEta;
 delete profile4thEta;
 delete profile6thEta;
 delete profile8thEta;

} // end of AliFlowAnalysisWithQCumulants::CalculateFinalResultsForDifferentialFlow(Bool_t useParticleWeights, TString type)
*/

//================================================================================================================================


void AliFlowAnalysisWithQCumulants::PrintFinalResultsForIntegratedFlow(TString type)
{
 // printing on the screen the final results for integrated flow (NONAME, POI and RP) // to be improved (NONAME) 
 
 Int_t n = fHarmonic; 
 
 if(type == "NONAME" || type == "RP" || type == "POI")
 {
  if(!(fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th))
  {
   cout<<"WARNING: fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th"<<endl;
   cout<<"         is NULL in AFAWQC::PFRFIF() !!!!"<<endl;
  }
 } else
   {
    cout<<"WARNING: type in not from {NONAME, RP, POI} in AFAWQC::PFRFIF() !!!!"<<endl;
    exit(0);
   }
 
 Double_t dVn[4] = {0.}; // array to hold Vn{2}, Vn{4}, Vn{6} and Vn{8}   
 Double_t dVnErr[4] = {0.}; // array to hold errors of Vn{2}, Vn{4}, Vn{6} and Vn{8}   
 
 if(type == "NONAME")
 {
  dVn[0] = (fCommonHistsResults2nd->GetHistIntFlow())->GetBinContent(1); 
  dVnErr[0] = (fCommonHistsResults2nd->GetHistIntFlow())->GetBinError(1); 
  dVn[1] = (fCommonHistsResults4th->GetHistIntFlow())->GetBinContent(1); 
  dVnErr[1] = (fCommonHistsResults4th->GetHistIntFlow())->GetBinError(1); 
  dVn[2] = (fCommonHistsResults6th->GetHistIntFlow())->GetBinContent(1); 
  dVnErr[2] = (fCommonHistsResults6th->GetHistIntFlow())->GetBinError(1); 
  dVn[3] = (fCommonHistsResults8th->GetHistIntFlow())->GetBinContent(1); 
  dVnErr[3] = (fCommonHistsResults8th->GetHistIntFlow())->GetBinError(1); 
 } else if(type == "RP")
   {
    dVn[0] = (fCommonHistsResults2nd->GetHistIntFlowRP())->GetBinContent(1); 
    dVnErr[0] = (fCommonHistsResults2nd->GetHistIntFlowRP())->GetBinError(1); 
    dVn[1] = (fCommonHistsResults4th->GetHistIntFlowRP())->GetBinContent(1); 
    dVnErr[1] = (fCommonHistsResults4th->GetHistIntFlowRP())->GetBinError(1); 
    dVn[2] = (fCommonHistsResults6th->GetHistIntFlowRP())->GetBinContent(1); 
    dVnErr[2] = (fCommonHistsResults6th->GetHistIntFlowRP())->GetBinError(1); 
    dVn[3] = (fCommonHistsResults8th->GetHistIntFlowRP())->GetBinContent(1); 
    dVnErr[3] = (fCommonHistsResults8th->GetHistIntFlowRP())->GetBinError(1); 
   } else if(type == "POI")
     {
      dVn[0] = (fCommonHistsResults2nd->GetHistIntFlowPOI())->GetBinContent(1); 
      dVnErr[0] = (fCommonHistsResults2nd->GetHistIntFlowPOI())->GetBinError(1); 
      dVn[1] = (fCommonHistsResults4th->GetHistIntFlowPOI())->GetBinContent(1); 
      dVnErr[1] = (fCommonHistsResults4th->GetHistIntFlowPOI())->GetBinError(1); 
      dVn[2] = (fCommonHistsResults6th->GetHistIntFlowPOI())->GetBinContent(1); 
      dVnErr[2] = (fCommonHistsResults6th->GetHistIntFlowPOI())->GetBinError(1); 
      dVn[3] = (fCommonHistsResults8th->GetHistIntFlowPOI())->GetBinContent(1); 
      dVnErr[3] = (fCommonHistsResults8th->GetHistIntFlowPOI())->GetBinError(1); 
     }
 
 TString title = " flow estimates from Q-cumulants"; 
 TString subtitle = "    ("; 
 
 if(!(fUsePhiWeights||fUsePtWeights||fUseEtaWeights))
 {
  subtitle.Append(type);
  subtitle.Append(", without weights)");
 } else  
   {
    subtitle.Append(type);
    subtitle.Append(", with weights)");
   }
  
 cout<<endl;
 cout<<"*************************************"<<endl;
 cout<<"*************************************"<<endl;
 cout<<title.Data()<<endl; 
 cout<<subtitle.Data()<<endl; 
 cout<<endl;
  
 for(Int_t i=0;i<4;i++)
 {
  if(dVn[i]>=0.)
  {
   cout<<"  v_"<<n<<"{"<<2*(i+1)<<"} = "<<dVn[i]<<" +/- "<<dVnErr[i]<<endl;
  }
  else
  {
   cout<<"  v_"<<n<<"{"<<2*(i+1)<<"} = Im"<<endl;
  }  
 }

 cout<<endl;
 /*
 if(type == "NONAME")
 {
  cout<<"     nEvts = "<<nEvtsNoName<<", AvM = "<<dMultNoName<<endl; // to be improved
 }
 else if (type == "RP")
 {
  cout<<"     nEvts = "<<nEvtsRP<<", AvM = "<<dMultRP<<endl; // to be improved  
 } 
 else if (type == "POI")
 {
  cout<<"     nEvts = "<<nEvtsPOI<<", AvM = "<<dMultPOI<<endl; // to be improved  
 } 
 */
 cout<<"*************************************"<<endl;
 cout<<"*************************************"<<endl;
 cout<<endl; 
  
}// end of AliFlowAnalysisWithQCumulants::PrintFinalResultsForIntegratedFlow(TString type="NONAME");


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CompareResultsFromNestedLoopsAndFromQVectorsForDiffFlow(Bool_t useParticleWeights)
{
 // compare correlations needed for diff. flow calculated with nested loops and those calculated from Q-vectors

 cout<<endl;
 cout<<"   *************************************"<<endl;
 cout<<"   **** cross-checking the formulas ****"<<endl;
 cout<<"   ****    for differential flow    ****"<<endl;
 cout<<"   ****                             ****"<<endl;
 cout<<"   ****        (pt,eta) bin:        ****"<<endl; 
 cout<<"   ****    1.1  < pt  <  1.2  GeV   ****"<<endl;  
 cout<<"   ****   -0.55 < eta < -0.525      ****"<<endl; 
 cout<<"   *************************************"<<endl;                             
 cout<<endl;
 
 if(!useParticleWeights)
 {                                        
  cout<<"<cos(n(psi1-phi2))> from Q-vectors    = "<<fCorrelationsPro[0][0][0][0]->GetBinContent(fCorrelationsPro[0][0][0][0]->GetBin(12,19))<<endl;
  cout<<"<cos(n(psi1-phi2))> from nested loops = "<<fDirectCorrelationsDiffFlow->GetBinContent(1)<<endl;
  cout<<endl;  
  cout<<"<cos(n(psi1+phi2-phi3-phi4))> from Q-vectors    = "<<fCorrelationsPro[0][0][0][1]->GetBinContent(fCorrelationsPro[0][0][0][1]->GetBin(12,19))<<endl;
  cout<<"<cos(n(psi1+phi2-phi3-phi4))> from nested loops = "<<fDirectCorrelationsDiffFlow->GetBinContent(41)<<endl;
  cout<<endl;   
  cout<<"****************************************************"<<endl;
  cout<<"****************************************************"<<endl;
  cout<<endl;
  /*
  cout<<"<cos(n(psi1))> from Q-vectors    = "<<fCorrectionsCosP1nPsiPtEtaPOI->GetBinContent(fCorrectionsCosP1nPsiPtEtaPOI->GetBin(12,19))<<endl;
  cout<<"<cos(n(psi1))> from nested loops = "<<fDirectCorrectionsDiffFlowCos->GetBinContent(1)<<endl;
  cout<<endl;  
  cout<<"<sin(n(psi1))> from Q-vectors    = "<<fCorrectionsSinP1nPsiPtEtaPOI->GetBinContent(fCorrectionsSinP1nPsiPtEtaPOI->GetBin(12,19))<<endl;
  cout<<"<sin(n(psi1))> from nested loops = "<<fDirectCorrectionsDiffFlowSin->GetBinContent(1)<<endl;
  cout<<endl;
  */
 }
 
 if(useParticleWeights)
 {
  cout<<"<w2 cos(n(psi1-phi2))> from Q-vectors (RP)   = "<<fCorrelationsPro[0][1][0][0]->GetBinContent(fCorrelationsPro[0][1][0][0]->GetBin(12,19))<<endl;
  cout<<"<w2 cos(n(psi1-phi2))> from Q-vectors (POI)  = "<<fCorrelationsPro[1][1][0][0]->GetBinContent(fCorrelationsPro[1][1][0][0]->GetBin(12,19))<<endl;
  cout<<"<w2 cos(n(psi1-phi2))> from nested loops     = "<<fDirectCorrelationsDiffFlowW->GetBinContent(1)<<endl;
  cout<<endl;  
  cout<<"<w2 w3 w4 cos(n(psi1+phi2-phi3-phi4))> from Q-vectors (RP)  = "<<fCorrelationsPro[0][1][0][1]->GetBinContent(fCorrelationsPro[0][1][0][1]->GetBin(12,19))<<endl;
  cout<<"<w2 w3 w4 cos(n(psi1+phi2-phi3-phi4))> from Q-vectors (POI) = "<<fCorrelationsPro[1][1][0][1]->GetBinContent(fCorrelationsPro[1][1][0][1]->GetBin(12,19))<<endl;
  cout<<"<w2 w3 w4 cos(n(psi1+phi2-phi3-phi4))> from nested loops    = "<<fDirectCorrelationsDiffFlowW->GetBinContent(41)<<endl;
  cout<<endl;  
 }
 
} // end of void AliFlowAnalysisWithQCumulants::CompareResultsFromNestedLoopsAndFromQVectorsForDiffFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::WriteHistograms(TString outputFileName)
{
 //store the final results in output .root file
 TFile *output = new TFile(outputFileName.Data(),"RECREATE");
 //output->WriteObject(fHistList, "cobjQC","SingleKey");
 fHistList->Write(fHistList->GetName(), TObject::kSingleKey);
 delete output;
}


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookCommonHistograms()
{
 // Book common control histograms and common histograms for final results.
 // common control histogram (ALL events)
 TString commonHistsName = "AliFlowCommonHistQC";
 commonHistsName += fAnalysisLabel->Data();
 fCommonHists = new AliFlowCommonHist(commonHistsName.Data());
 fHistList->Add(fCommonHists);  
 // common control histogram (for events with 2 and more particles)
 TString commonHists2ndOrderName = "AliFlowCommonHist2ndOrderQC";
 commonHists2ndOrderName += fAnalysisLabel->Data();
 fCommonHists2nd = new AliFlowCommonHist(commonHists2ndOrderName.Data());
 fHistList->Add(fCommonHists2nd);  
 // common control histogram (for events with 4 and more particles)
 TString commonHists4thOrderName = "AliFlowCommonHist4thOrderQC";
 commonHists4thOrderName += fAnalysisLabel->Data();
 fCommonHists4th = new AliFlowCommonHist(commonHists4thOrderName.Data());
 fHistList->Add(fCommonHists4th);  
 // common control histogram (for events with 6 and more particles)
 TString commonHists6thOrderName = "AliFlowCommonHist6thOrderQC";
 commonHists6thOrderName += fAnalysisLabel->Data();
 fCommonHists6th = new AliFlowCommonHist(commonHists6thOrderName.Data());
 fHistList->Add(fCommonHists6th);  
 // common control histogram (for events with 8 and more particles)
 TString commonHists8thOrderName = "AliFlowCommonHist8thOrderQC";
 commonHists8thOrderName += fAnalysisLabel->Data();
 fCommonHists8th = new AliFlowCommonHist(commonHists8thOrderName.Data());
 fHistList->Add(fCommonHists8th);    
 // common histograms for final results (calculated for events with 2 and more particles)
 TString commonHistResults2ndOrderName = "AliFlowCommonHistResults2ndOrderQC";
 commonHistResults2ndOrderName += fAnalysisLabel->Data();
 fCommonHistsResults2nd = new AliFlowCommonHistResults(commonHistResults2ndOrderName.Data());
 fHistList->Add(fCommonHistsResults2nd);  
 // common histograms for final results (calculated for events with 4 and more particles)
 TString commonHistResults4thOrderName = "AliFlowCommonHistResults4thOrderQC";
 commonHistResults4thOrderName += fAnalysisLabel->Data();
 fCommonHistsResults4th = new AliFlowCommonHistResults(commonHistResults4thOrderName.Data());
 fHistList->Add(fCommonHistsResults4th); 
 // common histograms for final results (calculated for events with 6 and more particles)
 TString commonHistResults6thOrderName = "AliFlowCommonHistResults6thOrderQC";
 commonHistResults6thOrderName += fAnalysisLabel->Data();
 fCommonHistsResults6th = new AliFlowCommonHistResults(commonHistResults6thOrderName.Data());
 fHistList->Add(fCommonHistsResults6th);  
 // common histograms for final results (calculated for events with 8 and more particles)
 TString commonHistResults8thOrderName = "AliFlowCommonHistResults8thOrderQC";
 commonHistResults8thOrderName += fAnalysisLabel->Data();
 fCommonHistsResults8th = new AliFlowCommonHistResults(commonHistResults8thOrderName.Data());
 fHistList->Add(fCommonHistsResults8th); 
 
} // end of void AliFlowAnalysisWithQCumulants::BookCommonHistograms()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookAndFillWeightsHistograms()
{
 // book and fill histograms which hold phi, pt and eta weights

 if(!fWeightsList)
 {
  cout<<"WARNING: fWeightsList is NULL in AFAWQC::BAFWH() !!!!"<<endl;
  exit(0);  
 }
    
 TString fUseParticleWeightsName = "fUseParticleWeightsQC";
 fUseParticleWeightsName += fAnalysisLabel->Data();
 fUseParticleWeights = new TProfile(fUseParticleWeightsName.Data(),"0 = particle weight not used, 1 = particle weight used ",3,0,3);
 fUseParticleWeights->SetLabelSize(0.06);
 (fUseParticleWeights->GetXaxis())->SetBinLabel(1,"w_{#phi}");
 (fUseParticleWeights->GetXaxis())->SetBinLabel(2,"w_{p_{T}}");
 (fUseParticleWeights->GetXaxis())->SetBinLabel(3,"w_{#eta}");
 fUseParticleWeights->Fill(0.5,(Int_t)fUsePhiWeights);
 fUseParticleWeights->Fill(1.5,(Int_t)fUsePtWeights);
 fUseParticleWeights->Fill(2.5,(Int_t)fUseEtaWeights);
 fWeightsList->Add(fUseParticleWeights); 
  
 if(fUsePhiWeights)
 {
  if(fWeightsList->FindObject("phi_weights"))
  {
   fPhiWeights = dynamic_cast<TH1F*>(fWeightsList->FindObject("phi_weights"));
   if(fPhiWeights->GetBinWidth(1) != fPhiBinWidth)
   {
    cout<<"WARNING: fPhiWeights->GetBinWidth(1) != fPhiBinWidth in AFAWQC::BAFWH() !!!!        "<<endl;
    cout<<"         This indicates inconsistent binning in phi histograms throughout the code."<<endl;
    exit(0);
   }
  } else 
    {
     cout<<"WARNING: fWeightsList->FindObject(\"phi_weights\") is NULL in AFAWQC::BAFWH() !!!!"<<endl;
     exit(0);
    }
 } // end of if(fUsePhiWeights)
 
 if(fUsePtWeights) 
 {
  if(fWeightsList->FindObject("pt_weights"))
  {
   fPtWeights = dynamic_cast<TH1D*>(fWeightsList->FindObject("pt_weights"));
   if(fPtWeights->GetBinWidth(1) != fPtBinWidth)
   {
    cout<<"WARNING: fPtWeights->GetBinWidth(1) != fPtBinWidth in AFAWQC::BAFWH() !!!!         "<<endl;
    cout<<"         This indicates insconsistent binning in pt histograms throughout the code."<<endl;
    exit(0);
   }
  } else 
    {
     cout<<"WARNING: fWeightsList->FindObject(\"pt_weights\") is NULL in AFAWQC::BAFWH() !!!!"<<endl;
     exit(0);
    }
 } // end of if(fUsePtWeights)    

 if(fUseEtaWeights) 
 {
  if(fWeightsList->FindObject("eta_weights"))
  {
   fEtaWeights = dynamic_cast<TH1D*>(fWeightsList->FindObject("eta_weights"));
   if(fEtaWeights->GetBinWidth(1) != fEtaBinWidth)
   {
    cout<<"WARNING: fEtaWeights->GetBinWidth(1) != fEtaBinWidth in AFAWQC::BAFWH() !!!!        "<<endl;
    cout<<"         This indicates insconsistent binning in eta histograms throughout the code."<<endl;
    exit(0);
   }
  } else 
    {
     cout<<"WARNING: fUseEtaWeights && fWeightsList->FindObject(\"eta_weights\") is NULL in AFAWQC::BAFWH() !!!!"<<endl;
     exit(0);
    }
 } // end of if(fUseEtaWeights)
 
} // end of AliFlowAnalysisWithQCumulants::BookAndFillWeightsHistograms()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookEverythingForIntegratedFlow()
{
 // Book all objects for integrated flow:
 //  a) Book profile to hold all flags for integrated flow.
 //  b) Book event-by-event quantities.
 //  c) Book profiles. // to be improved (comment)
 //  d) Book histograms holding the final results.
 
 TString sinCosFlag[2] = {"sin","cos"}; // to be improved (should I promote this to data members?)
 TString powerFlag[2] = {"linear","quadratic"}; // to be improved (should I promote this to data members?)
 
 // a) Book profile to hold all flags for integrated flow:
 TString intFlowFlagsName = "fIntFlowFlags";
 intFlowFlagsName += fAnalysisLabel->Data();
 fIntFlowFlags = new TProfile(intFlowFlagsName.Data(),"Flags for Integrated Flow",3,0,3);
 fIntFlowFlags->SetTickLength(-0.01,"Y");
 fIntFlowFlags->SetMarkerStyle(25);
 fIntFlowFlags->SetLabelSize(0.05);
 fIntFlowFlags->SetLabelOffset(0.02,"Y");
 (fIntFlowFlags->GetXaxis())->SetBinLabel(1,"Particle Weights");
 (fIntFlowFlags->GetXaxis())->SetBinLabel(2,"Event Weights");
 (fIntFlowFlags->GetXaxis())->SetBinLabel(3,"Corrected for NUA?");
 fIntFlowList->Add(fIntFlowFlags);

 // b) Book event-by-event quantities:
 // Re[Q_{m*n,k}], Im[Q_{m*n,k}] and S_{p,k}^M: 
 fReQ  = new TMatrixD(4,9);
 fImQ  = new TMatrixD(4,9);
 fSMpk = new TMatrixD(8,9);
 // average correlations <2>, <4>, <6> and <8> for single event (bining is the same as in fIntFlowCorrelationsPro and fIntFlowCorrelationsHist):
 TString intFlowCorrelationsEBEName = "fIntFlowCorrelationsEBE";
 intFlowCorrelationsEBEName += fAnalysisLabel->Data();
 fIntFlowCorrelationsEBE = new TH1D(intFlowCorrelationsEBEName.Data(),intFlowCorrelationsEBEName.Data(),4,0,4);
 // average all correlations for single event (bining is the same as in fIntFlowCorrelationsAllPro and fIntFlowCorrelationsAllHist):
 TString intFlowCorrelationsAllEBEName = "fIntFlowCorrelationsAllEBE";
 intFlowCorrelationsAllEBEName += fAnalysisLabel->Data();
 fIntFlowCorrelationsAllEBE = new TH1D(intFlowCorrelationsAllEBEName.Data(),intFlowCorrelationsAllEBEName.Data(),32,0,32);
 // average correction terms for non-uniform acceptance for single event 
 // (binning is the same as in fIntFlowCorrectionTermsForNUAPro[2] and fIntFlowCorrectionTermsForNUAHist[2]):
 TString fIntFlowCorrectionTermsForNUAEBEName = "fIntFlowCorrectionTermsForNUAEBE";
 fIntFlowCorrectionTermsForNUAEBEName += fAnalysisLabel->Data();
 for(Int_t sc=0;sc<2;sc++) // sin or cos terms
 {
  fIntFlowCorrectionTermsForNUAEBE[sc] = new TH1D(Form("%s: %s terms",fIntFlowCorrectionTermsForNUAEBEName.Data(),sinCosFlag[sc].Data()),Form("Correction terms for non-uniform acceptance (%s terms)",sinCosFlag[sc].Data()),10,0,10);  
 }
 
 // c) Book profiles: // to be improved (comment)
 // profile to hold average multiplicities and number of events for events with nRP>=0, nRP>=1, ... , and nRP>=8:
 TString avMultiplicityName = "fAvMultiplicity";
 avMultiplicityName += fAnalysisLabel->Data();
 fAvMultiplicity = new TProfile(avMultiplicityName.Data(),"Average Multiplicities of RPs",9,0,9);
 fAvMultiplicity->SetTickLength(-0.01,"Y");
 fAvMultiplicity->SetMarkerStyle(25);
 fAvMultiplicity->SetLabelSize(0.05);
 fAvMultiplicity->SetLabelOffset(0.02,"Y");
 fAvMultiplicity->SetYTitle("Average Multiplicity");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(1,"all evts");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(2,"n_{RP} #geq 1");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(3,"n_{RP} #geq 2");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(4,"n_{RP} #geq 3");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(5,"n_{RP} #geq 4");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(6,"n_{RP} #geq 5");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(7,"n_{RP} #geq 6");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(8,"n_{RP} #geq 7");
 (fAvMultiplicity->GetXaxis())->SetBinLabel(9,"n_{RP} #geq 8");
 fIntFlowProfiles->Add(fAvMultiplicity);
 // average correlations <<2>>, <<4>>, <<6>> and <<8>> for all events (with wrong errors!):
 TString intFlowCorrelationsProName = "fIntFlowCorrelationsPro";
 intFlowCorrelationsProName += fAnalysisLabel->Data();
 fIntFlowCorrelationsPro = new TProfile(intFlowCorrelationsProName.Data(),"Average correlations for all events",4,0,4,"s");
 fIntFlowCorrelationsPro->SetTickLength(-0.01,"Y");
 fIntFlowCorrelationsPro->SetMarkerStyle(25);
 fIntFlowCorrelationsPro->SetLabelSize(0.06);
 fIntFlowCorrelationsPro->SetLabelOffset(0.01,"Y");
 (fIntFlowCorrelationsPro->GetXaxis())->SetBinLabel(1,"<<2>>");
 (fIntFlowCorrelationsPro->GetXaxis())->SetBinLabel(2,"<<4>>");
 (fIntFlowCorrelationsPro->GetXaxis())->SetBinLabel(3,"<<6>>");
 (fIntFlowCorrelationsPro->GetXaxis())->SetBinLabel(4,"<<8>>");
 fIntFlowProfiles->Add(fIntFlowCorrelationsPro);
 // average all correlations for all events (with wrong errors!):
 TString intFlowCorrelationsAllProName = "fIntFlowCorrelationsAllPro";
 intFlowCorrelationsAllProName += fAnalysisLabel->Data();
 fIntFlowCorrelationsAllPro = new TProfile(intFlowCorrelationsAllProName.Data(),"Average correlations for all events",32,0,32,"s");
 fIntFlowCorrelationsAllPro->SetTickLength(-0.01,"Y");
 fIntFlowCorrelationsAllPro->SetMarkerStyle(25);
 fIntFlowCorrelationsAllPro->SetLabelSize(0.03);
 fIntFlowCorrelationsAllPro->SetLabelOffset(0.01,"Y");
 // 2-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(1,"<<2>>_{n|n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(2,"<<2>>_{2n|2n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(3,"<<2>>_{3n|3n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(4,"<<2>>_{4n|4n}");
 // 3-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(6,"<<3>>_{2n|n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(7,"<<3>>_{3n|2n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(8,"<<3>>_{4n|2n,2n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(9,"<<3>>_{4n|3n,n}");
 // 4-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(11,"<<4>>_{n,n|n,n}"); 
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(12,"<<4>>_{2n,n|2n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(13,"<<4>>_{2n,2n|2n,2n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(14,"<<4>>_{3n|n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(15,"<<4>>_{3n,n|3n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(16,"<<4>>_{3n,n|2n,2n}"); 
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(17,"<<4>>_{4n|2n,n,n}");
 // 5-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(19,"<<5>>_{2n|n,n,n,n}"); 
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(20,"<<5>>_{2n,2n|2n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(21,"<<5>>_{3n,n|2n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(22,"<<5>>_{4n|n,n,n,n}");
 // 6-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(24,"<<6>>_{n,n,n|n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(25,"<<6>>_{2n,n,n|2n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(26,"<<6>>_{2n,2n|n,n,n,n}");
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(27,"<<6>>_{3n,n|n,n,n,n}");
 // 7-p correlations:  
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(29,"<<7>>_{2n,n,n|n,n,n,n}");
 // 8-p correlations:
 (fIntFlowCorrelationsAllPro->GetXaxis())->SetBinLabel(31,"<<8>>_{n,n,n,n|n,n,n,n}");
 fIntFlowProfiles->Add(fIntFlowCorrelationsAllPro);
 // average product of correlations <2>, <4>, <6> and <8>:  
 TString intFlowProductOfCorrelationsProName = "fIntFlowProductOfCorrelationsPro";
 intFlowProductOfCorrelationsProName += fAnalysisLabel->Data();
 fIntFlowProductOfCorrelationsPro = new TProfile(intFlowProductOfCorrelationsProName.Data(),"Average products of correlations",6,0,6);
 fIntFlowProductOfCorrelationsPro->SetTickLength(-0.01,"Y");
 fIntFlowProductOfCorrelationsPro->SetMarkerStyle(25); 
 fIntFlowProductOfCorrelationsPro->SetLabelSize(0.05);
 fIntFlowProductOfCorrelationsPro->SetLabelOffset(0.01,"Y");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(1,"<<2><4>>");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(2,"<<2><6>>");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(3,"<<2><8>>");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(4,"<<4><6>>");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(5,"<<4><8>>");
 (fIntFlowProductOfCorrelationsPro->GetXaxis())->SetBinLabel(6,"<<6><8>>");
 fIntFlowProfiles->Add(fIntFlowProductOfCorrelationsPro);
 // average correction terms for non-uniform acceptance (with wrong errors!):
 for(Int_t sc=0;sc<2;sc++) // sin or cos terms
 {
  TString intFlowCorrectionTermsForNUAProName = "fIntFlowCorrectionTermsForNUAPro";
  intFlowCorrectionTermsForNUAProName += fAnalysisLabel->Data();
  fIntFlowCorrectionTermsForNUAPro[sc] = new TProfile(Form("%s: %s terms",intFlowCorrectionTermsForNUAProName.Data(),sinCosFlag[sc].Data()),Form("Correction terms for non-uniform acceptance (%s terms)",sinCosFlag[sc].Data()),10,0,10,"s");
  fIntFlowCorrectionTermsForNUAPro[sc]->SetTickLength(-0.01,"Y");
  fIntFlowCorrectionTermsForNUAPro[sc]->SetMarkerStyle(25);
  fIntFlowCorrectionTermsForNUAPro[sc]->SetLabelSize(0.03);
  fIntFlowCorrectionTermsForNUAPro[sc]->SetLabelOffset(0.01,"Y");
  // ......................................................................... 
  // 1-p terms:
  (fIntFlowCorrectionTermsForNUAPro[sc]->GetXaxis())->SetBinLabel(1,Form("%s(n(#phi_{1}))>",sinCosFlag[sc].Data()));
  // 2-p terms:
  // 3-p terms:
  // ...
  // ......................................................................... 
  fIntFlowProfiles->Add(fIntFlowCorrectionTermsForNUAPro[sc]);
 } // end of for(Int_t sc=0;sc<2;sc++) 
 
 // d) Book histograms holding the final results:
 // average correlations <<2>>, <<4>>, <<6>> and <<8>> for all events (with correct errors!):
 TString intFlowCorrelationsHistName = "fIntFlowCorrelationsHist";
 intFlowCorrelationsHistName += fAnalysisLabel->Data();
 fIntFlowCorrelationsHist = new TH1D(intFlowCorrelationsHistName.Data(),"Average correlations for all events",4,0,4);
 fIntFlowCorrelationsHist->SetTickLength(-0.01,"Y");
 fIntFlowCorrelationsHist->SetMarkerStyle(25);
 fIntFlowCorrelationsHist->SetLabelSize(0.06);
 fIntFlowCorrelationsHist->SetLabelOffset(0.01,"Y");
 (fIntFlowCorrelationsHist->GetXaxis())->SetBinLabel(1,"<<2>>");
 (fIntFlowCorrelationsHist->GetXaxis())->SetBinLabel(2,"<<4>>");
 (fIntFlowCorrelationsHist->GetXaxis())->SetBinLabel(3,"<<6>>");
 (fIntFlowCorrelationsHist->GetXaxis())->SetBinLabel(4,"<<8>>");
 fIntFlowResults->Add(fIntFlowCorrelationsHist);
 // average all correlations for all events (with correct errors!):
 TString intFlowCorrelationsAllHistName = "fIntFlowCorrelationsAllHist";
 intFlowCorrelationsAllHistName += fAnalysisLabel->Data();
 fIntFlowCorrelationsAllHist = new TH1D(intFlowCorrelationsAllHistName.Data(),"Average correlations for all events",32,0,32);
 fIntFlowCorrelationsAllHist->SetTickLength(-0.01,"Y");
 fIntFlowCorrelationsAllHist->SetMarkerStyle(25);
 fIntFlowCorrelationsAllHist->SetLabelSize(0.03);
 fIntFlowCorrelationsAllHist->SetLabelOffset(0.01,"Y");
 // 2-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(1,"<<2>>_{n|n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(2,"<<2>>_{2n|2n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(3,"<<2>>_{3n|3n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(4,"<<2>>_{4n|4n}");
 // 3-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(6,"<<3>>_{2n|n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(7,"<<3>>_{3n|2n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(8,"<<3>>_{4n|2n,2n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(9,"<<3>>_{4n|3n,n}");
 // 4-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(11,"<<4>>_{n,n|n,n}"); 
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(12,"<<4>>_{2n,n|2n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(13,"<<4>>_{2n,2n|2n,2n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(14,"<<4>>_{3n|n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(15,"<<4>>_{3n,n|3n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(16,"<<4>>_{3n,n|2n,2n}"); 
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(17,"<<4>>_{4n|2n,n,n}");
 // 5-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(19,"<<5>>_{2n|n,n,n,n}"); 
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(20,"<<5>>_{2n,2n|2n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(21,"<<5>>_{3n,n|2n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(22,"<<5>>_{4n|n,n,n,n}");
 // 6-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(24,"<<6>>_{n,n,n|n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(25,"<<6>>_{2n,n,n|2n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(26,"<<6>>_{2n,2n|n,n,n,n}");
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(27,"<<6>>_{3n,n|n,n,n,n}");
 // 7-p correlations:  
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(29,"<<7>>_{2n,n,n|n,n,n,n}");
 // 8-p correlations:
 (fIntFlowCorrelationsAllHist->GetXaxis())->SetBinLabel(31,"<<8>>_{n,n,n,n|n,n,n,n}");
 fIntFlowResults->Add(fIntFlowCorrelationsAllHist);
 // average correction terms for non-uniform acceptance (with correct errors!):
 for(Int_t sc=0;sc<2;sc++) // sin or cos terms
 {
  TString intFlowCorrectionTermsForNUAHistName = "fIntFlowCorrectionTermsForNUAHist";
  intFlowCorrectionTermsForNUAHistName += fAnalysisLabel->Data();
  fIntFlowCorrectionTermsForNUAHist[sc] = new TH1D(Form("%s: %s terms",intFlowCorrectionTermsForNUAHistName.Data(),sinCosFlag[sc].Data()),Form("Correction terms for non-uniform acceptance (%s terms)",sinCosFlag[sc].Data()),10,0,10);
  fIntFlowCorrectionTermsForNUAHist[sc]->SetTickLength(-0.01,"Y");
  fIntFlowCorrectionTermsForNUAHist[sc]->SetMarkerStyle(25);
  fIntFlowCorrectionTermsForNUAHist[sc]->SetLabelSize(0.03);
  fIntFlowCorrectionTermsForNUAHist[sc]->SetLabelOffset(0.01,"Y");
  // ......................................................................... 
  // 1-p terms:
  (fIntFlowCorrectionTermsForNUAHist[sc]->GetXaxis())->SetBinLabel(1,Form("%s(n(#phi_{1}))>",sinCosFlag[sc].Data()));
  // 2-p terms:
  // 3-p terms:
  // ...
  // ......................................................................... 
  fIntFlowResults->Add(fIntFlowCorrectionTermsForNUAHist[sc]);
 } // end of for(Int_t sc=0;sc<2;sc++) 
 // covariances (multiplied with weight dependent prefactor):
 TString intFlowCovariancesName = "fIntFlowCovariances";
 intFlowCovariancesName += fAnalysisLabel->Data();
 fIntFlowCovariances = new TH1D(intFlowCovariancesName.Data(),"Covariances (multiplied with weight dependent prefactor)",6,0,6);
 fIntFlowCovariances->SetLabelSize(0.04);
 fIntFlowCovariances->SetMarkerStyle(25);
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(1,"Cov(<2>,<4>)");
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(2,"Cov(<2>,<6>)");
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(3,"Cov(<2>,<8>)");
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(4,"Cov(<4>,<6>)");
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(5,"Cov(<4>,<8>)");
 (fIntFlowCovariances->GetXaxis())->SetBinLabel(6,"Cov(<6>,<8>)");  
 fIntFlowResults->Add(fIntFlowCovariances);
 // sum of linear and quadratic event weights for <2>, <4>, <6> and <8>:
 TString intFlowSumOfEventWeightsName = "fIntFlowSumOfEventWeights";
 intFlowSumOfEventWeightsName += fAnalysisLabel->Data();
 for(Int_t power=0;power<2;power++)
 {
  fIntFlowSumOfEventWeights[power] = new TH1D(Form("%s: %s",intFlowSumOfEventWeightsName.Data(),powerFlag[power].Data()),Form("Sum of %s event weights for correlations",powerFlag[power].Data()),4,0,4);
  fIntFlowSumOfEventWeights[power]->SetLabelSize(0.05);
  fIntFlowSumOfEventWeights[power]->SetMarkerStyle(25);
  if(power == 0)
  {
   (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(1,"#sum_{i=1}^{N} w_{<2>}");
   (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(2,"#sum_{i=1}^{N} w_{<4>}");
   (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(3,"#sum_{i=1}^{N} w_{<6>}");
   (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(4,"#sum_{i=1}^{N} w_{<8>}");
  } else if (power == 1) 
    {
     (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(1,"#sum_{i=1}^{N} w_{<2>}^{2}");
     (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(2,"#sum_{i=1}^{N} w_{<4>}^{2}");
     (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(3,"#sum_{i=1}^{N} w_{<6>}^{2}");
     (fIntFlowSumOfEventWeights[power]->GetXaxis())->SetBinLabel(4,"#sum_{i=1}^{N} w_{<8>}^{2}");
    }
  fIntFlowResults->Add(fIntFlowSumOfEventWeights[power]);
 } 
 // sum of products of event weights for correlations <2>, <4>, <6> and <8>:  
 TString intFlowSumOfProductOfEventWeightsName = "fIntFlowSumOfProductOfEventWeights";
 intFlowSumOfProductOfEventWeightsName += fAnalysisLabel->Data();
 fIntFlowSumOfProductOfEventWeights = new TH1D(intFlowSumOfProductOfEventWeightsName.Data(),"Sum of product of event weights for correlations",6,0,6);
 fIntFlowSumOfProductOfEventWeights->SetLabelSize(0.05);
 fIntFlowSumOfProductOfEventWeights->SetMarkerStyle(25);
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(1,"#sum_{i=1}^{N} w_{<2>} w_{<4>}");
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(2,"#sum_{i=1}^{N} w_{<2>} w_{<6>}");
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(3,"#sum_{i=1}^{N} w_{<2>} w_{<8>}");
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(4,"#sum_{i=1}^{N} w_{<4>} w_{<6>}");
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(5,"#sum_{i=1}^{N} w_{<4>} w_{<8>}");
 (fIntFlowSumOfProductOfEventWeights->GetXaxis())->SetBinLabel(6,"#sum_{i=1}^{N} w_{<6>} w_{<8>}");
 fIntFlowResults->Add(fIntFlowSumOfProductOfEventWeights);
 // final results for integrated Q-cumulants:
 TString intFlowQcumulantsName = "fIntFlowQcumulants";
 intFlowQcumulantsName += fAnalysisLabel->Data();
 fIntFlowQcumulants = new TH1D(intFlowQcumulantsName.Data(),"Integrated Q-cumulants",4,0,4);
 fIntFlowQcumulants->SetLabelSize(0.05);
 fIntFlowQcumulants->SetMarkerStyle(25);
 (fIntFlowQcumulants->GetXaxis())->SetBinLabel(1,"QC{2}");
 (fIntFlowQcumulants->GetXaxis())->SetBinLabel(2,"QC{4}");
 (fIntFlowQcumulants->GetXaxis())->SetBinLabel(3,"QC{6}");
 (fIntFlowQcumulants->GetXaxis())->SetBinLabel(4,"QC{8}");
 fIntFlowResults->Add(fIntFlowQcumulants);
 // final integrated flow estimates from Q-cumulants:
 TString intFlowName = "fIntFlow";
 intFlowName += fAnalysisLabel->Data();  
 // integrated flow from Q-cumulants:
 fIntFlow = new TH1D(intFlowName.Data(),"Integrated flow estimates from Q-cumulants",4,0,4);
 fIntFlow->SetLabelSize(0.05);
 fIntFlow->SetMarkerStyle(25);
 (fIntFlow->GetXaxis())->SetBinLabel(1,"v_{2}{2,QC}");
 (fIntFlow->GetXaxis())->SetBinLabel(2,"v_{2}{4,QC}");
 (fIntFlow->GetXaxis())->SetBinLabel(3,"v_{2}{6,QC}");
 (fIntFlow->GetXaxis())->SetBinLabel(4,"v_{2}{8,QC}");
 fIntFlowResults->Add(fIntFlow);

 /* // to be improved (removed):
  // final average weighted multi-particle correlations for all events calculated from Q-vectors
  fQCorrelations[1] = new TProfile("Weighted correlations","final average multi-particle correlations from weighted Q-vectors",200,0,200,"s");
  fQCorrelations[1]->SetTickLength(-0.01,"Y");
  fQCorrelations[1]->SetMarkerStyle(25);
  fQCorrelations[1]->SetLabelSize(0.03);
  fQCorrelations[1]->SetLabelOffset(0.01,"Y");
  // 2-particle correlations:
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(1,"<w_{1}w_{2}cos(n(#phi_{1}-#phi_{2}))>");
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(2,"<w_{1}^{2}w_{2}^{2}cos(2n(#phi_{1}-#phi_{2}))>");
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(3,"<w_{1}^{3}w_{2}^{3}cos(3n(#phi_{1}-#phi_{2}))>");
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(4,"<w_{1}^{4}w_{2}^{4}cos(4n(#phi_{1}-#phi_{2}))>");
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(5,"<w_{1}^{3}w_{2}cos(n(#phi_{1}-#phi_{2}))>");
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(6,"<w_{1}^{2}w_{2}w_{3}cos(n(#phi_{1}-#phi_{2}))>");
  // 3-particle correlations:
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(21,"<w_{1}w_{2}w_{3}^{2}cos(n(2#phi_{1}-#phi_{2}-#phi_{3}))>");
  // 4-particle correlations:
  (fQCorrelations[1]->GetXaxis())->SetBinLabel(41,"<w_{1}w_{2}w_{3}w_{4}cos(n(#phi_{1}+#phi_{2}-#phi_{3}-#phi_{4}))>");
  // add fQCorrelations[1] to the list fIntFlowList:
  fIntFlowList->Add(fQCorrelations[1]); 
 */
  
} // end of AliFlowAnalysisWithQCumulants::BookEverythingForIntegratedFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookEverythingForNestedLoops()
{
 // Book all objects relevant for calculations with nested loops.

 // to be improved: hardwired names for some objects here
 
 TString evaluateNestedLoopsName = "fEvaluateNestedLoops";
 evaluateNestedLoopsName += fAnalysisLabel->Data();
 fEvaluateNestedLoops = new TProfile(evaluateNestedLoopsName.Data(),"1 = evaluate, 0 = do not evaluate",2,0,2);
 fEvaluateNestedLoops->SetLabelSize(0.05);
 (fEvaluateNestedLoops->GetXaxis())->SetBinLabel(1,"Nested Loops (Int. Flow)");
 (fEvaluateNestedLoops->GetXaxis())->SetBinLabel(2,"Nested Loops (Diff. Flow)");
 fEvaluateNestedLoops->Fill(0.5,(Int_t)fEvaluateNestedLoopsForIntFlow);
 fEvaluateNestedLoops->Fill(1.5,(Int_t)fEvaluateNestedLoopsForDiffFlow);
 fNestedLoopsList->Add(fEvaluateNestedLoops);
 // nested loops for integrated flow:
 if(fEvaluateNestedLoopsForIntFlow)
 {
  fDirectCorrelations = new TProfile("fDirectCorrelations","multi-particle correlations with nested loops",100,0,100,"s");
  fNestedLoopsList->Add(fDirectCorrelations);
  fDirectCorrectionsCos = new TProfile("fDirectCorrectionsCos"," corrections for non-uniform acceptance (cos terms)",100,0,100,"s");
  fNestedLoopsList->Add(fDirectCorrectionsCos);
  fDirectCorrectionsSin = new TProfile("fDirectCorrectionsSin"," corrections for non-uniform acceptance (sin terms)",100,0,100,"s");
  fNestedLoopsList->Add(fDirectCorrectionsSin); 
  if(fUsePhiWeights) // Remark: cross-checking performed only with phi-weights (this is sufficient)
  {
   fDirectCorrelationsW = new TProfile("fDirectCorrelationsW","multi-particle correlations with nested loops",200,0,200,"s"); 
   fNestedLoopsList->Add(fDirectCorrelationsW);
   fDirectCorrectionsCosW = new TProfile("fDirectCorrectionsCosW"," corrections for non-uniform acceptance (cos terms)",100,0,100,"s");
   fNestedLoopsList->Add(fDirectCorrectionsCosW);
   fDirectCorrectionsSinW = new TProfile("fDirectCorrectionsSinW"," corrections for non-uniform acceptance (sin terms)",100,0,100,"s");
   fNestedLoopsList->Add(fDirectCorrectionsSinW); 
  }
 }
 // nested loops for differential flow: 
 if(fEvaluateNestedLoopsForDiffFlow)
 {
  fDirectCorrelationsDiffFlow = new TProfile("fDirectCorrelationsDiffFlow","multi-particle correlations with nested loops",200,0,200,"s");
  fNestedLoopsList->Add(fDirectCorrelationsDiffFlow); 
  fDirectCorrectionsDiffFlowCos = new TProfile("fDirectCorrectionsDiffFlowCos",
                                               "corrections for non-uniform acceptance (cos terms) with nested loops",200,0,200,"s");
  fNestedLoopsList->Add(fDirectCorrectionsDiffFlowCos);   
  fDirectCorrectionsDiffFlowSin = new TProfile("fDirectCorrectionsDiffFlowSin",
                                               "corrections for non-uniform acceptance (sin terms) with nested loops",200,0,200,"s");
  fNestedLoopsList->Add(fDirectCorrectionsDiffFlowSin);           
  if(fUsePhiWeights) // Remark: cross-checking performed only with phi-weights (this is sufficient)
  {
   fDirectCorrelationsDiffFlowW = new TProfile("fDirectCorrelationsDiffFlowW","multi-particle correlations with nested loops",200,0,200,"s");
   fNestedLoopsList->Add(fDirectCorrelationsDiffFlowW);
   fDirectCorrectionsDiffFlowCosW = new TProfile("fDirectCorrectionsDiffFlowCosW",
                                               "corrections for non-uniform acceptance (cos terms) with nested loops",200,0,200,"s");
   fNestedLoopsList->Add(fDirectCorrectionsDiffFlowCosW);   
   fDirectCorrectionsDiffFlowSinW = new TProfile("fDirectCorrectionsDiffFlowSinW",
                                               "corrections for non-uniform acceptance (sin terms) with nested loops",200,0,200,"s");
   fNestedLoopsList->Add(fDirectCorrectionsDiffFlowSinW);   
  }
 }
 
} // end of AliFlowAnalysisWithQCumulants::BookEverythingForNestedLoops()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrelations()
{
 // calculate all correlations needed for integrated flow
 
 // multiplicity:
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n = (*fReQ)(0,0);
 Double_t dReQ2n = (*fReQ)(1,0);
 Double_t dReQ3n = (*fReQ)(2,0);
 Double_t dReQ4n = (*fReQ)(3,0);
 Double_t dImQ1n = (*fImQ)(0,0);
 Double_t dImQ2n = (*fImQ)(1,0);
 Double_t dImQ3n = (*fImQ)(2,0);
 Double_t dImQ4n = (*fImQ)(3,0);
  
 // real and imaginary parts of some expressions involving various combinations of Q-vectors evaluated in harmonics n, 2n, 3n and 4n:
 // (these expression appear in the Eqs. for the multi-particle correlations bellow)
 
 // Re[Q_{2n} Q_{n}^* Q_{n}^*]
 Double_t reQ2nQ1nstarQ1nstar = pow(dReQ1n,2.)*dReQ2n + 2.*dReQ1n*dImQ1n*dImQ2n - pow(dImQ1n,2.)*dReQ2n; 
 
 // Im[Q_{2n} Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ1nstarQ1nstar = pow(dReQ1n,2.)*dImQ2n-2.*dReQ1n*dImQ1n*dReQ2n-pow(dImQ1n,2.)*dImQ2n; 
 
 // Re[Q_{n} Q_{n} Q_{2n}^*] = Re[Q_{2n} Q_{n}^* Q_{n}^*]
 Double_t reQ1nQ1nQ2nstar = reQ2nQ1nstarQ1nstar; 
 
 // Re[Q_{3n} Q_{n} Q_{2n}^* Q_{2n}^*]
 Double_t reQ3nQ1nQ2nstarQ2nstar = (pow(dReQ2n,2.)-pow(dImQ2n,2.))*(dReQ3n*dReQ1n-dImQ3n*dImQ1n) 
                                 + 2.*dReQ2n*dImQ2n*(dReQ3n*dImQ1n+dImQ3n*dReQ1n);

 // Im[Q_{3n} Q_{n} Q_{2n}^* Q_{2n}^*]                                                                  
 //Double_t imQ3nQ1nQ2nstarQ2nstar = calculate and implement this (deleteMe)
  
 // Re[Q_{2n} Q_{2n} Q_{3n}^* Q_{1n}^*] = Re[Q_{3n} Q_{n} Q_{2n}^* Q_{2n}^*]
 Double_t reQ2nQ2nQ3nstarQ1nstar = reQ3nQ1nQ2nstarQ2nstar;
  
 // Re[Q_{4n} Q_{2n}^* Q_{2n}^*]
 Double_t reQ4nQ2nstarQ2nstar = pow(dReQ2n,2.)*dReQ4n+2.*dReQ2n*dImQ2n*dImQ4n-pow(dImQ2n,2.)*dReQ4n;

 // Im[Q_{4n} Q_{2n}^* Q_{2n}^*]
 //Double_t imQ4nQ2nstarQ2nstar = calculate and implement this (deleteMe)
 
 // Re[Q_{2n} Q_{2n} Q_{4n}^*] =  Re[Q_{4n} Q_{2n}^* Q_{2n}^*]
 Double_t reQ2nQ2nQ4nstar = reQ4nQ2nstarQ2nstar;
 
 // Re[Q_{4n} Q_{3n}^* Q_{n}^*]
 Double_t reQ4nQ3nstarQ1nstar = dReQ4n*(dReQ3n*dReQ1n-dImQ3n*dImQ1n)+dImQ4n*(dReQ3n*dImQ1n+dImQ3n*dReQ1n);
 
 // Re[Q_{3n} Q_{n} Q_{4n}^*] = Re[Q_{4n} Q_{3n}^* Q_{n}^*]
 Double_t reQ3nQ1nQ4nstar = reQ4nQ3nstarQ1nstar;
 
 // Im[Q_{4n} Q_{3n}^* Q_{n}^*]
 //Double_t imQ4nQ3nstarQ1nstar = calculate and implement this (deleteMe)

 // Re[Q_{3n} Q_{2n}^* Q_{n}^*]
 Double_t reQ3nQ2nstarQ1nstar = dReQ3n*dReQ2n*dReQ1n-dReQ3n*dImQ2n*dImQ1n+dImQ3n*dReQ2n*dImQ1n
                              + dImQ3n*dImQ2n*dReQ1n;
                              
 // Re[Q_{2n} Q_{n} Q_{3n}^*] = Re[Q_{3n} Q_{2n}^* Q_{n}^*]
 Double_t reQ2nQ1nQ3nstar = reQ3nQ2nstarQ1nstar;
 
 // Im[Q_{3n} Q_{2n}^* Q_{n}^*]
 //Double_t imQ3nQ2nstarQ1nstar; //calculate and implement this (deleteMe)
 
 // Re[Q_{3n} Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ3nQ1nstarQ1nstarQ1nstar = dReQ3n*pow(dReQ1n,3)-3.*dReQ1n*dReQ3n*pow(dImQ1n,2)
                                     + 3.*dImQ1n*dImQ3n*pow(dReQ1n,2)-dImQ3n*pow(dImQ1n,3);

 // Im[Q_{3n} Q_{n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ3nQ1nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 
 // |Q_{2n}|^2 |Q_{n}|^2
 Double_t dQ2nQ1nQ2nstarQ1nstar = (pow(dReQ2n,2.)+pow(dImQ2n,2.))*(pow(dReQ1n,2.)+pow(dImQ1n,2.));
 
 // Re[Q_{4n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ4nQ2nstarQ1nstarQ1nstar = (dReQ4n*dReQ2n+dImQ4n*dImQ2n)*(pow(dReQ1n,2)-pow(dImQ1n,2))
                                     + 2.*dReQ1n*dImQ1n*(dImQ4n*dReQ2n-dReQ4n*dImQ2n); 
 
 // Im[Q_{4n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ4nQ2nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 
 // Re[Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ2nQ1nQ1nstarQ1nstarQ1nstar = (dReQ2n*dReQ1n-dImQ2n*dImQ1n)*(pow(dReQ1n,3)-3.*dReQ1n*pow(dImQ1n,2))
                                        + (dReQ2n*dImQ1n+dReQ1n*dImQ2n)*(3.*dImQ1n*pow(dReQ1n,2)-pow(dImQ1n,3));

 // Im[Q_{2n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^*] 
 //Double_t imQ2nQ1nQ1nstarQ1nstarQ1nstar; //calculate and implement this (deleteMe)
 
 // Re[Q_{2n} Q_{2n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ2nQ2nQ2nstarQ1nstarQ1nstar = (pow(dReQ2n,2.)+pow(dImQ2n,2.))
                                        * (dReQ2n*(pow(dReQ1n,2.)-pow(dImQ1n,2.)) + 2.*dImQ2n*dReQ1n*dImQ1n);

 // Im[Q_{2n} Q_{2n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ2nQ2nstarQ1nstarQ1nstar = (pow(dReQ2n,2.)+pow(dImQ2n,2.))
 //                                       * (dImQ2n*(pow(dReQ1n,2.)-pow(dImQ1n,2.)) - 2.*dReQ2n*dReQ1n*dImQ1n);
 
 // Re[Q_{4n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ4nQ1nstarQ1nstarQ1nstarQ1nstar = pow(dReQ1n,4.)*dReQ4n-6.*pow(dReQ1n,2.)*dReQ4n*pow(dImQ1n,2.)
                                            + pow(dImQ1n,4.)*dReQ4n+4.*pow(dReQ1n,3.)*dImQ1n*dImQ4n
                                            - 4.*pow(dImQ1n,3.)*dReQ1n*dImQ4n;
                                            
 // Im[Q_{4n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ4nQ1nstarQ1nstarQ1nstarQ1nstar = pow(dReQ1n,4.)*dImQ4n-6.*pow(dReQ1n,2.)*dImQ4n*pow(dImQ1n,2.)
 //                                           + pow(dImQ1n,4.)*dImQ4n+4.*pow(dImQ1n,3.)*dReQ1n*dReQ4n
 //                                           - 4.*pow(dReQ1n,3.)*dImQ1n*dReQ4n;
 
 // Re[Q_{3n} Q_{n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ3nQ1nQ2nstarQ1nstarQ1nstar = (pow(dReQ1n,2.)+pow(dImQ1n,2.))
                                        * (dReQ1n*dReQ2n*dReQ3n-dReQ3n*dImQ1n*dImQ2n+dReQ2n*dImQ1n*dImQ3n+dReQ1n*dImQ2n*dImQ3n);
 
 // Im[Q_{3n} Q_{n} Q_{2n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ3nQ1nQ2nstarQ1nstarQ1nstar = (pow(dReQ1n,2.)+pow(dImQ1n,2.))
 //                                       * (-dReQ2n*dReQ3n*dImQ1n-dReQ1n*dReQ3n*dImQ2n+dReQ1n*dReQ2n*dImQ3n-dImQ1n*dImQ2n*dImQ3n);
 
 
 // Re[Q_{2n} Q_{2n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ2nQ2nQ1nstarQ1nstarQ1nstarQ1nstar = (pow(dReQ1n,2.)*dReQ2n-2.*dReQ1n*dReQ2n*dImQ1n-dReQ2n*pow(dImQ1n,2.)
                                               + dImQ2n*pow(dReQ1n,2.)+2.*dReQ1n*dImQ1n*dImQ2n-pow(dImQ1n,2.)*dImQ2n)
                                               * (pow(dReQ1n,2.)*dReQ2n+2.*dReQ1n*dReQ2n*dImQ1n-dReQ2n*pow(dImQ1n,2.)
                                               - dImQ2n*pow(dReQ1n,2.)+2.*dReQ1n*dImQ1n*dImQ2n+pow(dImQ1n,2.)*dImQ2n);
 
 // Im[Q_{2n} Q_{2n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 //Double_t imQ2nQ2nQ1nstarQ1nstarQ1nstarQ1nstar = 2.*(pow(dReQ1n,2.)*dReQ2n-dReQ2n*pow(dImQ1n,2.)
 //                                              + 2.*dReQ1n*dImQ1n*dImQ2n)*(pow(dReQ1n,2.)*dImQ2n
 //                                              - 2.*dReQ1n*dImQ1n*dReQ2n-pow(dImQ1n,2.)*dImQ2n);
 
 // Re[Q_{3n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ3nQ1nQ1nstarQ1nstarQ1nstarQ1nstar = (pow(dReQ1n,2.)+pow(dImQ1n,2.))
                                               * (pow(dReQ1n,3.)*dReQ3n-3.*dReQ1n*dReQ3n*pow(dImQ1n,2.)
                                               + 3.*pow(dReQ1n,2.)*dImQ1n*dImQ3n-pow(dImQ1n,3.)*dImQ3n);
  
 // Im[Q_{3n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]                                                                                           
 //Double_t imQ3nQ1nQ1nstarQ1nstarQ1nstarQ1nstar = (pow(dReQ1n,2.)+pow(dImQ1n,2.))
 //                                              * (pow(dImQ1n,3.)*dReQ3n-3.*dImQ1n*dReQ3n*pow(dReQ1n,2.)
 //                                              - 3.*pow(dImQ1n,2.)*dReQ1n*dImQ3n+pow(dReQ1n,3.)*dImQ3n);
 
 // |Q_{2n}|^2 |Q_{n}|^4
 Double_t dQ2nQ1nQ1nQ2nstarQ1nstarQ1nstar = (pow(dReQ2n,2.)+pow(dImQ2n,2.))*pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.);
 
 // Re[Q_{2n} Q_{n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]
 Double_t reQ2nQ1nQ1nQ1nstarQ1nstarQ1nstarQ1nstar = pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)
                                                  * (pow(dReQ1n,2.)*dReQ2n-dReQ2n*pow(dImQ1n,2.)
                                                  + 2.*dReQ1n*dImQ1n*dImQ2n);
                                                  
 // Im[Q_{2n} Q_{n} Q_{n} Q_{n}^* Q_{n}^* Q_{n}^* Q_{n}^*]                                                  
 //Double_t imQ2nQ1nQ1nQ1nstarQ1nstarQ1nstarQ1nstar = pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)
 //                                                 * (pow(dReQ1n,2.)*dImQ2n-dImQ2n*pow(dImQ1n,2.)
 //                                                 - 2.*dReQ1n*dReQ2n*dImQ1n);
 
  
 
       
 //                                        **************************************
 //                                        **** multi-particle correlations: ****
 //                                        **************************************
 //
 // Remark 1: multi-particle correlations calculated with non-weighted Q-vectors are stored in 1D profile fQCorrelations[0].
 // Remark 2: binning of fQCorrelations[0] is organized as follows:
 // --------------------------------------------------------------------------------------------------------------------
 //  1st bin: <2>_{1n|1n} = two1n1n = cos(n*(phi1-phi2))>
 //  2nd bin: <2>_{2n|2n} = two2n2n = cos(2n*(phi1-phi2))>
 //  3rd bin: <2>_{3n|3n} = two3n3n = cos(3n*(phi1-phi2))> 
 //  4th bin: <2>_{4n|4n} = two4n4n = cos(4n*(phi1-phi2))>
 //  5th bin:           ----  EMPTY ----
 //  6th bin: <3>_{2n|1n,1n} = three2n1n1n = <cos(n*(2.*phi1-phi2-phi3))>
 //  7th bin: <3>_{3n|2n,1n} = three3n2n1n = <cos(n*(3.*phi1-2.*phi2-phi3))>
 //  8th bin: <3>_{4n|2n,2n} = three4n2n2n = <cos(n*(4.*phi1-2.*phi2-2.*phi3))>
 //  9th bin: <3>_{4n|3n,1n} = three4n3n1n = <cos(n*(4.*phi1-3.*phi2-phi3))>
 // 10th bin:           ----  EMPTY ----
 // 11th bin: <4>_{1n,1n|1n,1n} = four1n1n1n1n = <cos(n*(phi1+phi2-phi3-phi4))>
 // 12th bin: <4>_{2n,1n|2n,1n} = four2n1n2n1n = <cos(2.*n*(phi1+phi2-phi3-phi4))>
 // 13th bin: <4>_{2n,2n|2n,2n} = four2n2n2n2n = <cos(n*(2.*phi1+phi2-2.*phi3-phi4))>
 // 14th bin: <4>_{3n|1n,1n,1n} = four3n1n1n1n = <cos(n*(3.*phi1-phi2-phi3-phi4))> 
 // 15th bin: <4>_{3n,1n|3n,1n} = four3n1n3n1n = <cos(n*(4.*phi1-2.*phi2-phi3-phi4))>
 // 16th bin: <4>_{3n,1n|2n,2n} = four3n1n2n2n = <cos(n*(3.*phi1+phi2-2.*phi3-2.*phi4))>
 // 17th bin: <4>_{4n|2n,1n,1n} = four4n2n1n1n = <cos(n*(3.*phi1+phi2-3.*phi3-phi4))> 
 // 18th bin:           ----  EMPTY ----
 // 19th bin: <5>_{2n|1n,1n,1n,1n} = five2n1n1n1n1n = <cos(n*(2.*phi1+phi2-phi3-phi4-phi5))>
 // 20th bin: <5>_{2n,2n|2n,1n,1n} = five2n2n2n1n1n = <cos(n*(2.*phi1+2.*phi2-2.*phi3-phi4-phi5))>
 // 21st bin: <5>_{3n,1n|2n,1n,1n} = five3n1n2n1n1n = <cos(n*(3.*phi1+phi2-2.*phi3-phi4-phi5))>
 // 22nd bin: <5>_{4n|1n,1n,1n,1n} = five4n1n1n1n1n = <cos(n*(4.*phi1-phi2-phi3-phi4-phi5))>
 // 23rd bin:           ----  EMPTY ----
 // 24th bin: <6>_{1n,1n,1n|1n,1n,1n} = six1n1n1n1n1n1n = <cos(n*(phi1+phi2+phi3-phi4-phi5-phi6))>
 // 25th bin: <6>_{2n,1n,1n|2n,1n,1n} = six2n1n1n2n1n1n = <cos(n*(2.*phi1+2.*phi2-phi3-phi4-phi5-phi6))>
 // 26th bin: <6>_{2n,2n|1n,1n,1n,1n} = six2n2n1n1n1n1n = <cos(n*(3.*phi1+phi2-phi3-phi4-phi5-phi6))>
 // 27th bin: <6>_{3n,1n|1n,1n,1n,1n} = six3n1n1n1n1n1n = <cos(n*(2.*phi1+phi2+phi3-2.*phi4-phi5-phi6))>
 // 28th bin:           ----  EMPTY ----
 // 29th bin: <7>_{2n,1n,1n|1n,1n,1n,1n} = seven2n1n1n1n1n1n1n =  <cos(n*(2.*phi1+phi2+phi3-phi4-phi5-phi6-phi7))>
 // 30th bin:           ----  EMPTY ----
 // 31st bin: <8>_{1n,1n,1n,1n|1n,1n,1n,1n} = eight1n1n1n1n1n1n1n1n = <cos(n*(phi1+phi2+phi3+phi4-phi5-phi6-phi7-phi8))>
 // --------------------------------------------------------------------------------------------------------------------
    
 // 2-particle:
 Double_t two1n1n = 0.; // <cos(n*(phi1-phi2))>
 Double_t two2n2n = 0.; // <cos(2n*(phi1-phi2))>
 Double_t two3n3n = 0.; // <cos(3n*(phi1-phi2))>
 Double_t two4n4n = 0.; // <cos(4n*(phi1-phi2))>
 
 if(dMult>1)
 {
  two1n1n = (pow(dReQ1n,2.)+pow(dImQ1n,2.)-dMult)/(dMult*(dMult-1.)); 
  two2n2n = (pow(dReQ2n,2.)+pow(dImQ2n,2.)-dMult)/(dMult*(dMult-1.)); 
  two3n3n = (pow(dReQ3n,2.)+pow(dImQ3n,2.)-dMult)/(dMult*(dMult-1.)); 
  two4n4n = (pow(dReQ4n,2.)+pow(dImQ4n,2.)-dMult)/(dMult*(dMult-1.)); 
  
  // average 2-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(1,two1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(2,two2n2n);
  fIntFlowCorrelationsAllEBE->SetBinContent(3,two3n3n);
  fIntFlowCorrelationsAllEBE->SetBinContent(4,two4n4n);
          
  // average 2-particle correlations for all events:      
  fIntFlowCorrelationsAllPro->Fill(0.5,two1n1n,dMult*(dMult-1.));  
  fIntFlowCorrelationsAllPro->Fill(1.5,two2n2n,dMult*(dMult-1.)); 
  fIntFlowCorrelationsAllPro->Fill(2.5,two3n3n,dMult*(dMult-1.)); 
  fIntFlowCorrelationsAllPro->Fill(3.5,two4n4n,dMult*(dMult-1.)); 
  
  // store separetately <2> (to be improved: do I really need this?)
  fIntFlowCorrelationsEBE->SetBinContent(1,two1n1n);
  fIntFlowCorrelationsPro->Fill(0.5,two1n1n,dMult*(dMult-1.));
  
  // distribution of <cos(n*(phi1-phi2))>:
  //f2pDistribution->Fill(two1n1n,dMult*(dMult-1.)); 
 } // end of if(dMult>1)
 
 // 3-particle:
 Double_t three2n1n1n = 0.; // <cos(n*(2.*phi1-phi2-phi3))>
 Double_t three3n2n1n = 0.; // <cos(n*(3.*phi1-2.*phi2-phi3))>
 Double_t three4n2n2n = 0.; // <cos(n*(4.*phi1-2.*phi2-2.*phi3))>
 Double_t three4n3n1n = 0.; // <cos(n*(4.*phi1-3.*phi2-phi3))>
 
 if(dMult>2)
 {
  three2n1n1n = (reQ2nQ1nstarQ1nstar-2.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))
              - (pow(dReQ2n,2.)+pow(dImQ2n,2.))+2.*dMult)
              / (dMult*(dMult-1.)*(dMult-2.));              
  three3n2n1n = (reQ3nQ2nstarQ1nstar-(pow(dReQ3n,2.)+pow(dImQ3n,2.))
              - (pow(dReQ2n,2.)+pow(dImQ2n,2.))
              - (pow(dReQ1n,2.)+pow(dImQ1n,2.))+2.*dMult)
              / (dMult*(dMult-1.)*(dMult-2.));
  three4n2n2n = (reQ4nQ2nstarQ2nstar-2.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))
              - (pow(dReQ4n,2.)+pow(dImQ4n,2.))+2.*dMult)
              / (dMult*(dMult-1.)*(dMult-2.)); 
  three4n3n1n = (reQ4nQ3nstarQ1nstar-(pow(dReQ4n,2.)+pow(dImQ4n,2.))
              - (pow(dReQ3n,2.)+pow(dImQ3n,2.))
              - (pow(dReQ1n,2.)+pow(dImQ1n,2.))+2.*dMult)
              / (dMult*(dMult-1.)*(dMult-2.)); 
              
  // average 3-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(6,three2n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(7,three3n2n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(8,three4n2n2n);
  fIntFlowCorrelationsAllEBE->SetBinContent(9,three4n3n1n);
        
  // average 3-particle correlations for all events:                
  fIntFlowCorrelationsAllPro->Fill(5.5,three2n1n1n,dMult*(dMult-1.)*(dMult-2.)); 
  fIntFlowCorrelationsAllPro->Fill(6.5,three3n2n1n,dMult*(dMult-1.)*(dMult-2.));
  fIntFlowCorrelationsAllPro->Fill(7.5,three4n2n2n,dMult*(dMult-1.)*(dMult-2.)); 
  fIntFlowCorrelationsAllPro->Fill(8.5,three4n3n1n,dMult*(dMult-1.)*(dMult-2.));    
 } // end of if(dMult>2)
 
 // 4-particle:
 Double_t four1n1n1n1n = 0.; // <cos(n*(phi1+phi2-phi3-phi4))>
 Double_t four2n2n2n2n = 0.; // <cos(2.*n*(phi1+phi2-phi3-phi4))>
 Double_t four2n1n2n1n = 0.; // <cos(n*(2.*phi1+phi2-2.*phi3-phi4))> 
 Double_t four3n1n1n1n = 0.; // <cos(n*(3.*phi1-phi2-phi3-phi4))> 
 Double_t four4n2n1n1n = 0.; // <cos(n*(4.*phi1-2.*phi2-phi3-phi4))> 
 Double_t four3n1n2n2n = 0.; // <cos(n*(3.*phi1+phi2-2.*phi3-2.*phi4))> 
 Double_t four3n1n3n1n = 0.; // <cos(n*(3.*phi1+phi2-3.*phi3-phi4))>   
 
 if(dMult>3)
 {
  four1n1n1n1n = (2.*dMult*(dMult-3.)+pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)-4.*(dMult-2.)*(pow(dReQ1n,2.)
               + pow(dImQ1n,2.))-2.*reQ2nQ1nstarQ1nstar+(pow(dReQ2n,2.)+pow(dImQ2n,2.)))
               / (dMult*(dMult-1)*(dMult-2.)*(dMult-3.));     
  four2n2n2n2n = (2.*dMult*(dMult-3.)+pow((pow(dReQ2n,2.)+pow(dImQ2n,2.)),2.)-4.*(dMult-2.)*(pow(dReQ2n,2.)
               + pow(dImQ2n,2.))-2.*reQ4nQ2nstarQ2nstar+(pow(dReQ4n,2.)+pow(dImQ4n,2.)))
               / (dMult*(dMult-1)*(dMult-2.)*(dMult-3.));
  four2n1n2n1n = (dQ2nQ1nQ2nstarQ1nstar-2.*reQ3nQ2nstarQ1nstar-2.*reQ2nQ1nstarQ1nstar)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               - ((dMult-5.)*(pow(dReQ1n,2.)+pow(dImQ1n,2.))
               + (dMult-4.)*(pow(dReQ2n,2.)+pow(dImQ2n,2.))-(pow(dReQ3n,2.)+pow(dImQ3n,2.)))
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               + (dMult-6.)/((dMult-1.)*(dMult-2.)*(dMult-3.));
  four3n1n1n1n = (reQ3nQ1nstarQ1nstarQ1nstar-3.*reQ3nQ2nstarQ1nstar-3.*reQ2nQ1nstarQ1nstar)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               + (2.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))+3.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))
               + 6.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))-6.*dMult)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  four4n2n1n1n = (reQ4nQ2nstarQ1nstarQ1nstar-2.*reQ4nQ3nstarQ1nstar-reQ4nQ2nstarQ2nstar-2.*reQ3nQ2nstarQ1nstar)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               - (reQ2nQ1nstarQ1nstar-2.*(pow(dReQ4n,2.)+pow(dImQ4n,2.))-2.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
               - 3.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))-4.*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               - 6./((dMult-1.)*(dMult-2.)*(dMult-3.));
  four3n1n2n2n = (reQ3nQ1nQ2nstarQ2nstar-reQ4nQ2nstarQ2nstar-reQ3nQ1nQ4nstar-2.*reQ3nQ2nstarQ1nstar)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               - (2.*reQ1nQ1nQ2nstar-(pow(dReQ4n,2.)+pow(dImQ4n,2.))-2.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
               - 4.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))-4.*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               - 6./((dMult-1.)*(dMult-2.)*(dMult-3.)); 
  four3n1n3n1n = ((pow(dReQ3n,2.)+pow(dImQ3n,2.))*(pow(dReQ1n,2.)+pow(dImQ1n,2.))
               - 2.*reQ4nQ3nstarQ1nstar-2.*reQ3nQ2nstarQ1nstar)
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               + ((pow(dReQ4n,2.)+pow(dImQ4n,2.))-(dMult-4.)*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
               + (pow(dReQ2n,2.)+pow(dImQ2n,2.))-(dMult-4.)*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
               / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.))
               + (dMult-6.)/((dMult-1.)*(dMult-2.)*(dMult-3.));
               
  // average 4-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(11,four1n1n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(12,four2n1n2n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(13,four2n2n2n2n);
  fIntFlowCorrelationsAllEBE->SetBinContent(14,four3n1n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(15,four3n1n3n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(16,four3n1n2n2n);
  fIntFlowCorrelationsAllEBE->SetBinContent(17,four4n2n1n1n);
        
  // average 4-particle correlations for all events:                
  fIntFlowCorrelationsAllPro->Fill(10.5,four1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  fIntFlowCorrelationsAllPro->Fill(11.5,four2n1n2n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  fIntFlowCorrelationsAllPro->Fill(12.5,four2n2n2n2n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  fIntFlowCorrelationsAllPro->Fill(13.5,four3n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  fIntFlowCorrelationsAllPro->Fill(14.5,four3n1n3n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  fIntFlowCorrelationsAllPro->Fill(15.5,four3n1n2n2n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));  
  fIntFlowCorrelationsAllPro->Fill(16.5,four4n2n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
  
  // store separetately <4> (to be improved: do I really need this?)
  fIntFlowCorrelationsEBE->SetBinContent(2,four1n1n1n1n);
  fIntFlowCorrelationsPro->Fill(1.5,four1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  
  // distribution of <cos(n*(phi1+phi2-phi3-phi4))>
  //f4pDistribution->Fill(four1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.));
  
 } // end of if(dMult>3)

 // 5-particle:
 Double_t five2n1n1n1n1n = 0.; // <cos(n*(2.*phi1+phi2-phi3-phi4-phi5))>
 Double_t five2n2n2n1n1n = 0.; // <cos(n*(2.*phi1+2.*phi2-2.*phi3-phi4-phi5))>
 Double_t five3n1n2n1n1n = 0.; // <cos(n*(3.*phi1+phi2-2.*phi3-phi4-phi5))>
 Double_t five4n1n1n1n1n = 0.; // <cos(n*(4.*phi1-phi2-phi3-phi4-phi5))>
 
 if(dMult>4)
 {
  five2n1n1n1n1n = (reQ2nQ1nQ1nstarQ1nstarQ1nstar-reQ3nQ1nstarQ1nstarQ1nstar+6.*reQ3nQ2nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (reQ2nQ1nQ3nstar+3.*(dMult-6.)*reQ2nQ1nstarQ1nstar+3.*reQ1nQ1nQ2nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (2.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
                 + 3.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*(pow(dReQ2n,2.)+pow(dImQ2n,2.))     
                 - 3.*(dMult-4.)*(pow(dReQ2n,2.)+pow(dImQ2n,2.)))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - 3.*(pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)
                 - 2.*(2*dMult-5.)*(pow(dReQ1n,2.)+pow(dImQ1n,2.))+2.*dMult*(dMult-4.))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
                 
  five2n2n2n1n1n = (reQ2nQ2nQ2nstarQ1nstarQ1nstar-reQ4nQ2nstarQ1nstarQ1nstar-2.*reQ2nQ2nQ3nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 + 2.*(reQ4nQ2nstarQ2nstar+4.*reQ3nQ2nstarQ1nstar+reQ3nQ1nQ4nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 + (reQ2nQ2nQ4nstar-2.*(dMult-5.)*reQ2nQ1nstarQ1nstar+2.*reQ1nQ1nQ2nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (2.*(pow(dReQ4n,2.)+pow(dImQ4n,2.))+4.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
                 + 1.*pow((pow(dReQ2n,2.)+pow(dImQ2n,2.)),2.)
                 - 2.*(3.*dMult-10.)*(pow(dReQ2n,2.)+pow(dImQ2n,2.)))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (4.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*(pow(dReQ2n,2.)+pow(dImQ2n,2.))
                 - 4.*(dMult-5.)*(pow(dReQ1n,2.)+pow(dImQ1n,2.))+4.*dMult*(dMult-6.))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)); 

  five4n1n1n1n1n = (reQ4nQ1nstarQ1nstarQ1nstarQ1nstar-6.*reQ4nQ2nstarQ1nstarQ1nstar-4.*reQ3nQ1nstarQ1nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 + (8.*reQ4nQ3nstarQ1nstar+3.*reQ4nQ2nstarQ2nstar+12.*reQ3nQ2nstarQ1nstar+12.*reQ2nQ1nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (6.*(pow(dReQ4n,2.)+pow(dImQ4n,2.))+8.*(pow(dReQ3n,2.)+pow(dImQ3n,2.))
                 + 12.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))+24.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))-24.*dMult)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
  
  five3n1n2n1n1n = (reQ3nQ1nQ2nstarQ1nstarQ1nstar-reQ4nQ2nstarQ1nstarQ1nstar-reQ3nQ1nstarQ1nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (reQ3nQ1nQ2nstarQ2nstar-3.*reQ4nQ3nstarQ1nstar-reQ4nQ2nstarQ2nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - ((2.*dMult-13.)*reQ3nQ2nstarQ1nstar-reQ3nQ1nQ4nstar-9.*reQ2nQ1nstarQ1nstar)
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - (2.*reQ1nQ1nQ2nstar+2.*(pow(dReQ4n,2.)+pow(dImQ4n,2.))
                 - 2.*(dMult-5.)*(pow(dReQ3n,2.)+pow(dImQ3n,2.))+2.*(pow(dReQ3n,2.)
                 + pow(dImQ3n,2.))*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 + (2.*(dMult-6.)*(pow(dReQ2n,2.)+pow(dImQ2n,2.))
                 - 2.*(pow(dReQ2n,2.)+pow(dImQ2n,2.))*(pow(dReQ1n,2.)+pow(dImQ1n,2.))
                 - pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)
                 + 2.*(3.*dMult-11.)*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
                 / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.))
                 - 4.*(dMult-6.)/((dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
                 
  // average 5-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(19,five2n1n1n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(20,five2n2n2n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(21,five3n1n2n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(22,five4n1n1n1n1n);
        
  // average 5-particle correlations for all events:                         
  fIntFlowCorrelationsAllPro->Fill(18.5,five2n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)); 
  fIntFlowCorrelationsAllPro->Fill(19.5,five2n2n2n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
  fIntFlowCorrelationsAllPro->Fill(20.5,five3n1n2n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
  fIntFlowCorrelationsAllPro->Fill(21.5,five4n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.));
 } // end of if(dMult>4)
    
 // 6-particle:
 Double_t six1n1n1n1n1n1n = 0.; // <cos(n*(phi1+phi2+phi3-phi4-phi5-phi6))>
 Double_t six2n2n1n1n1n1n = 0.; // <cos(n*(2.*phi1+2.*phi2-phi3-phi4-phi5-phi6))>
 Double_t six3n1n1n1n1n1n = 0.; // <cos(n*(3.*phi1+phi2-phi3-phi4-phi5-phi6))>
 Double_t six2n1n1n2n1n1n = 0.; // <cos(n*(2.*phi1+phi2+phi3-2.*phi4-phi5-phi6))>
 
 if(dMult>5)
 {
  six1n1n1n1n1n1n = (pow(pow(dReQ1n,2.)+pow(dImQ1n,2.),3.)+9.*dQ2nQ1nQ2nstarQ1nstar-6.*reQ2nQ1nQ1nstarQ1nstarQ1nstar)
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.))
                  + 4.*(reQ3nQ1nstarQ1nstarQ1nstar-3.*reQ3nQ2nstarQ1nstar)
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.))
                  + 2.*(9.*(dMult-4.)*reQ2nQ1nstarQ1nstar+2.*(pow(dReQ3n,2.)+pow(dImQ3n,2.)))
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.))
                  - 9.*(pow((pow(dReQ1n,2.)+pow(dImQ1n,2.)),2.)+(pow(dReQ2n,2.)+pow(dImQ2n,2.)))
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-5.))
                  + (18.*(pow(dReQ1n,2.)+pow(dImQ1n,2.)))
                  / (dMult*(dMult-1)*(dMult-3)*(dMult-4))
                  - 6./((dMult-1.)*(dMult-2.)*(dMult-3.));
                  
  six2n1n1n2n1n1n = (dQ2nQ1nQ1nQ2nstarQ1nstarQ1nstar-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)
                  * (2.*five2n2n2n1n1n+4.*five2n1n1n1n1n+4.*five3n1n2n1n1n+4.*four2n1n2n1n+1.*four1n1n1n1n)
                  - dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(4.*four1n1n1n1n+4.*two1n1n
                  + 2.*three2n1n1n+2.*three2n1n1n+4.*four3n1n1n1n+8.*three2n1n1n+2.*four4n2n1n1n
                  + 4.*four2n1n2n1n+2.*two2n2n+8.*four2n1n2n1n+4.*four3n1n3n1n+8.*three3n2n1n
                  + 4.*four3n1n2n2n+4.*four1n1n1n1n+4.*four2n1n2n1n+1.*four2n2n2n2n)
                  - dMult*(dMult-1.)*(dMult-2.)*(2.*three2n1n1n+8.*two1n1n+4.*two1n1n+2.
                  + 4.*two1n1n+4.*three2n1n1n+2.*two2n2n+4.*three2n1n1n+8.*three3n2n1n
                  + 8.*two2n2n+4.*three4n3n1n+4.*two3n3n+4.*three3n2n1n+4.*two1n1n
                  + 8.*three2n1n1n+4.*two1n1n+4.*three3n2n1n+4.*three2n1n1n+2.*two2n2n
                  + 4.*three3n2n1n+2.*three4n2n2n)-dMult*(dMult-1.)
                  * (4.*two1n1n+4.+4.*two1n1n+2.*two2n2n+1.+4.*two1n1n+4.*two2n2n+4.*two3n3n
                  + 1.+2.*two2n2n+1.*two4n4n)-dMult)
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); // to be improved (direct formula needed)
 
  six2n2n1n1n1n1n = (reQ2nQ2nQ1nstarQ1nstarQ1nstarQ1nstar-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)
                  * (five4n1n1n1n1n+8.*five2n1n1n1n1n+6.*five2n2n2n1n1n)-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)
                  * (4.*four3n1n1n1n+6.*four4n2n1n1n+12.*three2n1n1n+12.*four1n1n1n1n+24.*four2n1n2n1n
                  + 4.*four3n1n2n2n+3.*four2n2n2n2n)-dMult*(dMult-1.)*(dMult-2.)*(6.*three2n1n1n+12.*three3n2n1n
                  + 4.*three4n3n1n+3.*three4n2n2n+8.*three2n1n1n+24.*two1n1n+12.*two2n2n+12.*three2n1n1n+8.*three3n2n1n
                  + 1.*three4n2n2n)-dMult*(dMult-1.)*(4.*two1n1n+6.*two2n2n+4.*two3n3n+1.*two4n4n+2.*two2n2n+8.*two1n1n+6.)-dMult)
                  / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); // to be improved (direct formula needed)
   
  six3n1n1n1n1n1n = (reQ3nQ1nQ1nstarQ1nstarQ1nstarQ1nstar-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)
                  * (five4n1n1n1n1n+4.*five2n1n1n1n1n+6.*five3n1n2n1n1n+4.*four3n1n1n1n)
                  - dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(4.*four3n1n1n1n+6.*four4n2n1n1n+6.*four1n1n1n1n
                  + 12.*three2n1n1n+12.*four2n1n2n1n+6.*four3n1n1n1n+12.*three3n2n1n+4.*four3n1n3n1n+3.*four3n1n2n2n)
                  - dMult*(dMult-1.)*(dMult-2.)*(6.*three2n1n1n+12.*three3n2n1n+4.*three4n3n1n+3.*three4n2n2n+4.*two1n1n
                  + 12.*two1n1n+6.*three2n1n1n+12.*three2n1n1n+4.*three3n2n1n+12.*two2n2n+4.*three3n2n1n+4.*two3n3n+1.*three4n3n1n
                  + 6.*three3n2n1n)-dMult*(dMult-1.)*(4.*two1n1n+6.*two2n2n+4.*two3n3n+1.*two4n4n+1.*two1n1n+4.+6.*two1n1n+4.*two2n2n
                  + 1.*two3n3n)-dMult)/(dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); // to be improved (direct formula needed)
                                 
  // average 6-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(24,six1n1n1n1n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(25,six2n1n1n2n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(26,six2n2n1n1n1n1n);
  fIntFlowCorrelationsAllEBE->SetBinContent(27,six3n1n1n1n1n1n);
        
  // average 6-particle correlations for all events:         
  fIntFlowCorrelationsAllPro->Fill(23.5,six1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); 
  fIntFlowCorrelationsAllPro->Fill(24.5,six2n1n1n2n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); 
  fIntFlowCorrelationsAllPro->Fill(25.5,six2n2n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.));
  fIntFlowCorrelationsAllPro->Fill(26.5,six3n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); 

  // store separetately <6> (to be improved: do I really need this?)
  fIntFlowCorrelationsEBE->SetBinContent(3,six1n1n1n1n1n1n);
  fIntFlowCorrelationsPro->Fill(2.5,six1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.));
 
  // distribution of <cos(n*(phi1+phi2+phi3-phi4-phi5-phi6))>
  //f6pDistribution->Fill(six1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)); 
 } // end of if(dMult>5)
 
 // 7-particle:
 Double_t seven2n1n1n1n1n1n1n = 0.; // <cos(n*(2.*phi1+phi2+phi3-phi4-phi5-phi6-phi7))>
 
 if(dMult>6)
 {
  seven2n1n1n1n1n1n1n = (reQ2nQ1nQ1nQ1nstarQ1nstarQ1nstarQ1nstar-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)
                      * (2.*six3n1n1n1n1n1n+4.*six1n1n1n1n1n1n+1.*six2n2n1n1n1n1n+6.*six2n1n1n2n1n1n+8.*five2n1n1n1n1n)
                      - dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(1.*five4n1n1n1n1n +8.*five2n1n1n1n1n+8.*four3n1n1n1n
                      + 12.*five3n1n2n1n1n+4.*five2n1n1n1n1n+3.*five2n2n2n1n1n+6.*five2n2n2n1n1n+6.*four1n1n1n1n+24.*four1n1n1n1n
                      + 12.*five2n1n1n1n1n+12.*five2n1n1n1n1n+12.*three2n1n1n+24.*four2n1n2n1n+4.*five3n1n2n1n1n+4.*five2n1n1n1n1n)
                      - dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(4.*four3n1n1n1n+6.*four4n2n1n1n+12.*four1n1n1n1n+24.*three2n1n1n
                      + 24.*four2n1n2n1n+12.*four3n1n1n1n+24.*three3n2n1n+8.*four3n1n3n1n+6.*four3n1n2n2n+6.*three2n1n1n+12.*four1n1n1n1n
                      + 12.*four2n1n2n1n+6.*three2n1n1n+12.*four2n1n2n1n+4.*four3n1n2n2n+3.*four2n2n2n2n+4.*four1n1n1n1n+6.*three2n1n1n
                      + 24.*two1n1n+24.*four1n1n1n1n+4.*four3n1n1n1n+24.*two1n1n+24.*three2n1n1n+12.*two2n2n+24.*three2n1n1n+12.*four2n1n2n1n
                      + 8.*three3n2n1n+8.*four2n1n2n1n+1.*four4n2n1n1n)-dMult*(dMult-1.)*(dMult-2.)*(6.*three2n1n1n+1.*three2n1n1n+8.*two1n1n
                      + 12.*three3n2n1n+24.*two1n1n+12.*three2n1n1n+4.*three2n1n1n+8.*two1n1n+4.*three4n3n1n+24.*three2n1n1n+8.*three3n2n1n
                      + 12.*two1n1n+12.*two1n1n+3.*three4n2n2n+24.*two2n2n+6.*two2n2n+12.+12.*three3n2n1n+8.*two3n3n+12.*three2n1n1n+24.*two1n1n
                      + 4.*three3n2n1n+8.*three3n2n1n+2.*three4n3n1n+12.*two1n1n+8.*three2n1n1n+4.*three2n1n1n+2.*three3n2n1n+6.*two2n2n+8.*two2n2n
                      + 1.*three4n2n2n+4.*three3n2n1n+6.*three2n1n1n)-dMult*(dMult-1.)*(4.*two1n1n+2.*two1n1n+6.*two2n2n+8.+1.*two2n2n+4.*two3n3n
                      + 12.*two1n1n+4.*two1n1n+1.*two4n4n+8.*two2n2n+6.+2.*two3n3n+4.*two1n1n+1.*two2n2n)-dMult)
                      / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)); // to be improved (direct formula needed)
        
  // average 7-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(29,seven2n1n1n1n1n1n1n);
       
  // average 7-particle correlations for all events:                      
  fIntFlowCorrelationsAllPro->Fill(28.5,seven2n1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.));
 } // end of if(dMult>6)
 
 // 8-particle:
 Double_t eight1n1n1n1n1n1n1n1n = 0.; // <cos(n*(phi1+phi2+phi3+phi4-phi5-phi6-phi7-phi8))>
 if(dMult>7)
 {
  eight1n1n1n1n1n1n1n1n = (pow(pow(dReQ1n,2.)+pow(dImQ1n,2.),4.)-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)
                        * (12.*seven2n1n1n1n1n1n1n+16.*six1n1n1n1n1n1n)-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)
                        * (8.*six3n1n1n1n1n1n+48.*six1n1n1n1n1n1n+6.*six2n2n1n1n1n1n+96.*five2n1n1n1n1n+72.*four1n1n1n1n+36.*six2n1n1n2n1n1n)
                        - dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(2.*five4n1n1n1n1n+32.*five2n1n1n1n1n+36.*four1n1n1n1n
                        + 32.*four3n1n1n1n+48.*five2n1n1n1n1n+48.*five3n1n2n1n1n+144.*five2n1n1n1n1n+288.*four1n1n1n1n+36.*five2n2n2n1n1n
                        + 144.*three2n1n1n+96.*two1n1n+144.*four2n1n2n1n)-dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)
                        * (8.*four3n1n1n1n+48.*four1n1n1n1n+12.*four4n2n1n1n+96.*four2n1n2n1n+96.*three2n1n1n+72.*three2n1n1n+144.*two1n1n
                        + 16.*four3n1n3n1n+48.*four3n1n1n1n+144.*four1n1n1n1n+72.*four1n1n1n1n+96.*three3n2n1n+24.*four3n1n2n2n+144.*four2n1n2n1n
                        + 288.*two1n1n+288.*three2n1n1n+9.*four2n2n2n2n+72.*two2n2n+24.)-dMult*(dMult-1.)*(dMult-2.)*(12.*three2n1n1n+16.*two1n1n
                        + 24.*three3n2n1n+48.*three2n1n1n+96.*two1n1n+8.*three4n3n1n+32.*three3n2n1n+96.*three2n1n1n+144.*two1n1n+6.*three4n2n2n
                        + 96.*two2n2n+36.*two2n2n+72.+48.*three3n2n1n+16.*two3n3n+72.*three2n1n1n+144.*two1n1n)-dMult*(dMult-1.)*(8.*two1n1n
                        + 12.*two2n2n+16.+8.*two3n3n+48.*two1n1n+1.*two4n4n+16.*two2n2n+18.)-dMult)
                        / (dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)*(dMult-7.)); // to be improved (direct formula needed)
  
  // average 8-particle correlations for single event: 
  fIntFlowCorrelationsAllEBE->SetBinContent(31,eight1n1n1n1n1n1n1n1n);
       
  // average 8-particle correlations for all events:                       
  fIntFlowCorrelationsAllPro->Fill(30.5,eight1n1n1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)*(dMult-7.));
 
  // store separetately <8> (to be improved: do I really need this?)
  fIntFlowCorrelationsEBE->SetBinContent(4,eight1n1n1n1n1n1n1n1n);
  fIntFlowCorrelationsPro->Fill(3.5,eight1n1n1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)*(dMult-7.));
  
  // distribution of <cos(n*(phi1+phi2+phi3+phi4-phi5-phi6-phi7-phi8))>
  //f8pDistribution->Fill(eight1n1n1n1n1n1n1n1n,dMult*(dMult-1.)*(dMult-2.)*(dMult-3.)*(dMult-4.)*(dMult-5.)*(dMult-6.)*(dMult-7.));
 } // end of if(dMult>7) 
 
} // end of AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrelations()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::EvaluateNestedLoopsForIntegratedFlow(AliFlowEventSimple* anEvent)
{
 // 1.) Evaluate with nested loops the relevant correlations for integrated flow without and with using the particle weights. 
 //     Results are stored in profiles fDirectCorrelations and fDirectCorrelationsW, respectively.
 
 // 2.) Evaluate with nested loops corrections for non-uniform acceptance relevant for integrated flow, 
 //     without and with using the particle weights.
 //     Without weights: cos terms are stored in profile fDirectCorrectionsCos, and sin terms in profile fDirectCorrectionsSin.
 //     With weights: cos terms are stored in profile fDirectCorrectionsCosW, and sin terms in profile fDirectCorrectionsSinW.
 
 // 3.) Binning of fDirectCorrelations is organized as follows:
 // 
 //  1st bin: <2>_{1n|1n} = two1n1n = cos(n*(phi1-phi2))>
 //  2nd bin: <2>_{2n|2n} = two2n2n = cos(2n*(phi1-phi2))>
 //  3rd bin: <2>_{3n|3n} = two3n3n = cos(3n*(phi1-phi2))> 
 //  4th bin: <2>_{4n|4n} = two4n4n = cos(4n*(phi1-phi2))>
 //  5th bin:           ----  EMPTY ----
 //  6th bin: <3>_{2n|1n,1n} = three2n1n1n = <cos(n*(2.*phi1-phi2-phi3))>
 //  7th bin: <3>_{3n|2n,1n} = three3n2n1n = <cos(n*(3.*phi1-2.*phi2-phi3))>
 //  8th bin: <3>_{4n|2n,2n} = three4n2n2n = <cos(n*(4.*phi1-2.*phi2-2.*phi3))>
 //  9th bin: <3>_{4n|3n,1n} = three4n3n1n = <cos(n*(4.*phi1-3.*phi2-phi3))>
 // 10th bin:           ----  EMPTY ----
 // 11th bin: <4>_{1n,1n|1n,1n} = four1n1n1n1n = <cos(n*(phi1+phi2-phi3-phi4))>
 // 12th bin: <4>_{2n,1n|2n,1n} = four2n1n2n1n = <cos(2.*n*(phi1+phi2-phi3-phi4))>
 // 13th bin: <4>_{2n,2n|2n,2n} = four2n2n2n2n = <cos(n*(2.*phi1+phi2-2.*phi3-phi4))>
 // 14th bin: <4>_{3n|1n,1n,1n} = four3n1n1n1n = <cos(n*(3.*phi1-phi2-phi3-phi4))> 
 // 15th bin: <4>_{3n,1n|3n,1n} = four3n1n3n1n = <cos(n*(4.*phi1-2.*phi2-phi3-phi4))>
 // 16th bin: <4>_{3n,1n|2n,2n} = four3n1n2n2n = <cos(n*(3.*phi1+phi2-2.*phi3-2.*phi4))>
 // 17th bin: <4>_{4n|2n,1n,1n} = four4n2n1n1n = <cos(n*(3.*phi1+phi2-3.*phi3-phi4))> 
 // 18th bin:           ----  EMPTY ----
 // 19th bin: <5>_{2n|1n,1n,1n,1n} = five2n1n1n1n1n = <cos(n*(2.*phi1+phi2-phi3-phi4-phi5))>
 // 20th bin: <5>_{2n,2n|2n,1n,1n} = five2n2n2n1n1n = <cos(n*(2.*phi1+2.*phi2-2.*phi3-phi4-phi5))>
 // 21st bin: <5>_{3n,1n|2n,1n,1n} = five3n1n2n1n1n = <cos(n*(3.*phi1+phi2-2.*phi3-phi4-phi5))>
 // 22nd bin: <5>_{4n|1n,1n,1n,1n} = five4n1n1n1n1n = <cos(n*(4.*phi1-phi2-phi3-phi4-phi5))>
 // 23rd bin:           ----  EMPTY ----
 // 24th bin: <6>_{1n,1n,1n|1n,1n,1n} = six1n1n1n1n1n1n = <cos(n*(phi1+phi2+phi3-phi4-phi5-phi6))>
 // 25th bin: <6>_{2n,1n,1n|2n,1n,1n} = six2n1n1n2n1n1n = <cos(n*(2.*phi1+2.*phi2-phi3-phi4-phi5-phi6))>
 // 26th bin: <6>_{2n,2n|1n,1n,1n,1n} = six2n2n1n1n1n1n = <cos(n*(3.*phi1+phi2-phi3-phi4-phi5-phi6))>
 // 27th bin: <6>_{3n,1n|1n,1n,1n,1n} = six3n1n1n1n1n1n = <cos(n*(2.*phi1+phi2+phi3-2.*phi4-phi5-phi6))>
 // 28th bin:           ----  EMPTY ----
 // 29th bin: <7>_{2n,1n,1n|1n,1n,1n,1n} = seven2n1n1n1n1n1n1n =  <cos(n*(2.*phi1+phi2+phi3-phi4-phi5-phi6-phi7))>
 // 30th bin:           ----  EMPTY ----
 // 31st bin: <8>_{1n,1n,1n,1n|1n,1n,1n,1n} = eight1n1n1n1n1n1n1n1n = <cos(n*(phi1+phi2+phi3+phi4-phi5-phi6-phi7-phi8))>
 
 // 4.) Binning of fDirectCorrelationsW is organized as follows:
 // ..............................................................................................
 //       ---- bins 1-20: 2-particle correlations ----
 // 1st bin: two1n1nW1W1 = <w1 w2 cos(n*(phi1-phi2))>
 // 2nd bin: two2n2nW2W2 = <w1^2 w2^2 cos(2n*(phi1-phi2))>
 // 3rd bin: two3n3nW3W3 = <w1^3 w2^3 cos(3n*(phi1-phi2))>
 // 4th bin: two4n4nW4W4 = <w1^4 w2^4 cos(4n*(phi1-phi2))>
 // 5th bin: two1n1nW3W1 = <w1^3 w2 cos(n*(phi1-phi2))>
 // 6th bin: two1n1nW1W1W2 = <w1 w2 w3^2 cos(n*(phi1-phi2))>  
 //       ---- bins 21-40: 3-particle correlations ----
 // 21st bin: three2n1n1nW2W1W1 = <w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))> 
 //       ---- bins 41-60: 4-particle correlations ----
 // 41st bin: four1n1n1n1nW1W1W1W1 = <w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))>
 //       ---- bins 61-80: 5-particle correlations ---- 
 //       ---- bins 81-100: 6-particle correlations ----
 //       ---- bins 101-120: 7-particle correlations ----
 //       ---- bins 121-140: 8-particle correlations ----
 // ..............................................................................................
 
 // 5.) Binning of fDirectCorrectionsCos is organized as follows:
 // ..............................................................................................
 // 1st bin: <<cos(n*(phi1))>> = cosP1n
 // 2nd bin: <<cos(n*(phi1+phi2))>> = cosP1nP1n
 // 3rd bin: <<cos(n*(phi1-phi2-phi3))>> = cosP1nM1nM1n
 // ...
 // ..............................................................................................
              
 // 6.) Binning of fDirectCorrectionsSin is organized as follows:
 // ..............................................................................................
 // 1st bin: <<sin(n*(phi1))>> = sinP1n
 // 2nd bin: <<sin(n*(phi1+phi2))>> = sinP1nP1n
 // 3rd bin: <<sin(n*(phi1-phi2-phi3))>> = sinP1nM1nM1n
 // ...
 // ..............................................................................................
       
 // 7.) Binning of fDirectCorrectionsCosW is organized as follows:
 // ..............................................................................................     
 // ...
 // ..............................................................................................     
 
 // 8.) Binning of fDirectCorrectionsSinW is organized as follows:
 // ..............................................................................................     
 // ...
 // ..............................................................................................     
 
 Int_t nPrim = anEvent->NumberOfTracks(); 
 AliFlowTrackSimple *aftsTrack = NULL;
 
 Double_t phi1=0., phi2=0., phi3=0., phi4=0., phi5=0., phi6=0., phi7=0., phi8=0.;
 Double_t wPhi1=1., wPhi2=1., wPhi3=1., wPhi4=1., wPhi5=1., wPhi6=1., wPhi7=1., wPhi8=1.;
 
 Int_t n = fHarmonic; 
 
 // 2-particle correlations and 1- and 2-particle correction terms:       
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  
  // corrections for non-uniform acceptance:
  // non-weighted: 
  fDirectCorrectionsCos->Fill(0.5,cos(n*phi1),1.); // <cos(n*phi1)>
  fDirectCorrectionsSin->Fill(0.5,sin(n*phi1),1.); // <sin(n*phi1)>  
  // weighted:
  // fDirectCorrectionsCosW->Fill(0.5,???,1); // to be improved (continued)
  // fDirectCorrectionsSinW->Fill(0.5,???,1); // to be improved (continued)
  
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
    
   // non-weighted correlations: 
   fDirectCorrelations->Fill(0.5,cos(n*(phi1-phi2)),1.);    // <cos(n*(phi1-phi2))>
   fDirectCorrelations->Fill(1.5,cos(2.*n*(phi1-phi2)),1.); // <cos(2n*(phi1-phi2))>
   fDirectCorrelations->Fill(2.5,cos(3.*n*(phi1-phi2)),1.); // <cos(3n*(phi1-phi2))>
   fDirectCorrelations->Fill(3.5,cos(4.*n*(phi1-phi2)),1.); // <cos(4n*(phi1-phi2))> 
   
   // weighted correlations:
   // ................................................................................................................
   if(fUsePhiWeights) fDirectCorrelationsW->Fill(0.5,cos(n*(phi1-phi2)),wPhi1*wPhi2);                  // <w1   w2   cos( n*(phi1-phi2))>
   if(fUsePhiWeights) fDirectCorrelationsW->Fill(1.5,cos(2.*n*(phi1-phi2)),pow(wPhi1,2)*pow(wPhi2,2)); // <w1^2 w2^2 cos(2n*(phi1-phi2))>
   if(fUsePhiWeights) fDirectCorrelationsW->Fill(2.5,cos(3.*n*(phi1-phi2)),pow(wPhi1,3)*pow(wPhi2,3)); // <w1^3 w2^3 cos(3n*(phi1-phi2))>
   if(fUsePhiWeights) fDirectCorrelationsW->Fill(3.5,cos(4.*n*(phi1-phi2)),pow(wPhi1,4)*pow(wPhi2,4)); // <w1^4 w2^4 cos(4n*(phi1-phi2))> 
   if(fUsePhiWeights) fDirectCorrelationsW->Fill(4.5,cos(n*(phi1-phi2)),pow(wPhi1,3)*wPhi2);           // <w1^3 w2 cos(n*(phi1-phi2))>
   // ...
   // ................................................................................................................
 
   // non-weighted corrections for non-uniform acceptance (cos terms)
   // ................................................................................................................
   fDirectCorrectionsCos->Fill(1.5,cos(n*(phi1+phi2)),1.); // <<cos(n*(phi1+phi2))>>
   // ...
   // ................................................................................................................
  
   // non-weighted corrections for non-uniform acceptance (sin terms)
   // ................................................................................................................
   fDirectCorrectionsSin->Fill(1.5,sin(n*(phi1+phi2)),1.); // <<sin(n*(phi1+phi2))>>
   // ...
   // ................................................................................................................
   
   // weighted corrections for non-uniform acceptance (cos terms)
   // ................................................................................................................
   // fDirectCorrectionsCosW->Fill(1.5,???,1.); // to be improved (continued)
   // ...
   // ................................................................................................................
  
   // non-weighted corrections for non-uniform acceptance (sin terms)
   // ................................................................................................................
   // fDirectCorrectionsSinW->Fill(1.5,???,1.); // to be improved (continued)
   // ...
   // ................................................................................................................

  } // end of for(Int_t i2=0;i2<nPrim;i2++)
 } // end of for(Int_t i1=0;i1<nPrim;i1++)
 
 // 3-particle correlations:         
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    
    // non-weighted correlations:
    fDirectCorrelations->Fill(5.,cos(2.*n*phi1-n*(phi2+phi3)),1.);       //<3>_{2n|nn,n}
    fDirectCorrelations->Fill(6.,cos(3.*n*phi1-2.*n*phi2-n*phi3),1.);    //<3>_{3n|2n,n}
    fDirectCorrelations->Fill(7.,cos(4.*n*phi1-2.*n*phi2-2.*n*phi3),1.); //<3>_{4n|2n,2n}
    fDirectCorrelations->Fill(8.,cos(4.*n*phi1-3.*n*phi2-n*phi3),1.);    //<3>_{4n|3n,n}
    
    // weighted correlations:
    // ..............................................................................................................................
    // 2-p:
    if(fUsePhiWeights) fDirectCorrelationsW->Fill(5.,cos(n*(phi1-phi2)),wPhi1*wPhi2*pow(wPhi3,2)); // <w1 w2 w3^2 cos(n*(phi1-phi2))>
    // 3-p:
    if(fUsePhiWeights) fDirectCorrelationsW->Fill(20.,cos(2.*n*phi1-n*(phi2+phi3)),pow(wPhi1,2)*wPhi2*wPhi3); // <w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))>
    // ...
    // ..............................................................................................................................
    
    // non-weighted corrections for non-uniform acceptance (cos terms)
    // ................................................................................................................
    fDirectCorrectionsCos->Fill(2.,cos(n*(phi1-phi2-phi3)),1.); // <<cos(n*(phi1-phi2-phi3))>>
    // ...
    // ................................................................................................................
  
    // non-weighted corrections for non-uniform acceptance (sin terms)
    // ................................................................................................................
    fDirectCorrectionsSin->Fill(2.,sin(n*(phi1-phi2-phi3)),1.); // <<sin(n*(phi1-phi2-phi3))>>
    // ...
    // ................................................................................................................
    
    // weighted corrections for non-uniform acceptance (cos terms)
    // ................................................................................................................
    // ...
    // ................................................................................................................
    
    // weighted corrections for non-uniform acceptance (sin terms)
    // ................................................................................................................
    // ...
    // ................................................................................................................

   } // end of for(Int_t i3=0;i3<nPrim;i3++)
  } // end of for(Int_t i2=0;i2<nPrim;i2++)
 } // end of for(Int_t i1=0;i1<nPrim;i1++)

 // 4-particle correlations:       
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection())) continue;
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     
     // non-weighted:
     fDirectCorrelations->Fill(10.,cos(n*phi1+n*phi2-n*phi3-n*phi4),1.);            // <4>_{n,n|n,n} 
     fDirectCorrelations->Fill(11.,cos(2.*n*phi1+n*phi2-2.*n*phi3-n*phi4),1.);      // <4>_{2n,n|2n,n}
     fDirectCorrelations->Fill(12.,cos(2.*n*phi1+2*n*phi2-2.*n*phi3-2.*n*phi4),1.); // <4>_{2n,2n|2n,2n}
     fDirectCorrelations->Fill(13.,cos(3.*n*phi1-n*phi2-n*phi3-n*phi4),1.);         // <4>_{3n|n,n,n}
     fDirectCorrelations->Fill(14.,cos(3.*n*phi1+n*phi2-3.*n*phi3-n*phi4),1.);      // <4>_{3n,n|3n,n}   
     fDirectCorrelations->Fill(15.,cos(3.*n*phi1+n*phi2-2.*n*phi3-2.*n*phi4),1.);   // <4>_{3n,n|2n,2n}
     fDirectCorrelations->Fill(16.,cos(4.*n*phi1-2.*n*phi2-n*phi3-n*phi4),1.);      // <4>_{4n|2n,n,n}
     
     // weighted:
     //.......................................................................................
     // 4-p:
     if(fUsePhiWeights) fDirectCorrelationsW->Fill(40.,cos(n*phi1+n*phi2-n*phi3-n*phi4),wPhi1*wPhi2*wPhi3*wPhi4); 
     // ...             
     //.......................................................................................
     
    }  
   }
  }
 }

 // 5-particle correlations:      
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  //cout<<"i1 = "<<i1<<endl;
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;  
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection())) continue;
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection())) continue;
      phi5=aftsTrack->Phi();
      if(fUsePhiWeights && fPhiWeights) wPhi5 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi5*fnBinsPhi/TMath::TwoPi())));
      
      // non-weighted:
      //------------------------------------------------------------------------------------------------------
      fDirectCorrelations->Fill(18.,cos(2.*n*phi1+n*phi2-n*phi3-n*phi4-n*phi5),1.);       //<5>_{2n,n|n,n,n}
      fDirectCorrelations->Fill(19.,cos(2.*n*phi1+2.*n*phi2-2.*n*phi3-n*phi4-n*phi5),1.); //<5>_{2n,2n|2n,n,n}
      fDirectCorrelations->Fill(20.,cos(3.*n*phi1+n*phi2-2.*n*phi3-n*phi4-n*phi5),1.);    //<5>_{3n,n|2n,n,n}
      fDirectCorrelations->Fill(21.,cos(4.*n*phi1-n*phi2-n*phi3-n*phi4-n*phi5),1.);       //<5>_{4n|n,n,n,n}
      //------------------------------------------------------------------------------------------------------
      
      // weighted:
      //..............................................................................................................
      // 5-p:
      if(fUsePhiWeights) fDirectCorrelationsW->Fill(60.,cos(2.*n*phi1+n*phi2-n*phi3-n*phi4-n*phi5),pow(wPhi1,2)*wPhi2*wPhi3*wPhi4*wPhi5);     
      //..............................................................................................................
      
     }
    }  
   }
  }
 }
 
 // 6-particle correlations:
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  //cout<<"i1 = "<<i1<<endl;
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection())) continue;
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection())) continue;
      phi5=aftsTrack->Phi();
      if(fUsePhiWeights && fPhiWeights) wPhi5 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi5*fnBinsPhi/TMath::TwoPi())));
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection())) continue;
       phi6=aftsTrack->Phi(); 
       if(fUsePhiWeights && fPhiWeights) wPhi6 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi6*fnBinsPhi/TMath::TwoPi())));
       
       // non-weighted:
       //-----------------------------------------------------------------------------------------------------------
       fDirectCorrelations->Fill(23.,cos(n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6),1.);       //<6>_{n,n,n|n,n,n}
       fDirectCorrelations->Fill(24.,cos(2.*n*phi1+n*phi2+n*phi3-2.*n*phi4-n*phi5-n*phi6),1.); //<6>_{2n,n,n|2n,n,n}
       fDirectCorrelations->Fill(25.,cos(2.*n*phi1+2.*n*phi2-n*phi3-n*phi4-n*phi5-n*phi6),1.); //<6>_{2n,2n|n,n,n,n}
       fDirectCorrelations->Fill(26.,cos(3.*n*phi1+n*phi2-n*phi3-n*phi4-n*phi5-n*phi6),1.);    //<6>_{3n,n|n,n,n,n}  
       //-----------------------------------------------------------------------------------------------------------

       // weighted:
       //.................................................................................................................
       // 6-p:
       if(fUsePhiWeights) fDirectCorrelationsW->Fill(80.,cos(n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6),wPhi1*wPhi2*wPhi3*wPhi4*wPhi5*wPhi6);
       //.................................................................................................................       
          
      } 
     }
    }  
   }
  }
 }
 
 // 7-particle correlations:
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  //cout<<"i1 = "<<i1<<endl;
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection())) continue;
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection())) continue;
      phi5=aftsTrack->Phi();
      if(fUsePhiWeights && fPhiWeights) wPhi5 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi5*fnBinsPhi/TMath::TwoPi())));
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection())) continue;
       phi6=aftsTrack->Phi(); 
       if(fUsePhiWeights && fPhiWeights) wPhi6 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi6*fnBinsPhi/TMath::TwoPi())));
       for(Int_t i7=0;i7<nPrim;i7++)
       {
        if(i7==i1||i7==i2||i7==i3||i7==i4||i7==i5||i7==i6)continue;
        aftsTrack=anEvent->GetTrack(i7);
        if(!(aftsTrack->InRPSelection())) continue;
        phi7=aftsTrack->Phi(); 
        if(fUsePhiWeights && fPhiWeights) wPhi7 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi7*fnBinsPhi/TMath::TwoPi())));
        
        // non-weighted:
        //---------------------------------------------------------------------------------------------------------------
        fDirectCorrelations->Fill(28.,cos(2.*n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6-n*phi7),1.);//<7>_{2n,n,n|n,n,n,n}
        //---------------------------------------------------------------------------------------------------------------
        
        // weighted:
        //..........................................................................................................................................
        if(fUsePhiWeights) fDirectCorrelationsW->Fill(100.,cos(2.*n*phi1+n*phi2+n*phi3-n*phi4-n*phi5-n*phi6-n*phi7),
                                                           pow(wPhi1,2.)*wPhi2*wPhi3*wPhi4*wPhi5*wPhi6*wPhi7);
        //..........................................................................................................................................
        
       } 
      } 
     }
    }  
   }
  }
 }
 
 cout<<endl;
 
 // 8-particle correlations:
 for(Int_t i1=0;i1<nPrim;i1++)
 {
  cout<<"i1 = "<<i1<<endl;
  aftsTrack=anEvent->GetTrack(i1);
  if(!(aftsTrack->InRPSelection())) continue;
  phi1=aftsTrack->Phi();
  if(fUsePhiWeights && fPhiWeights) wPhi1 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi1*fnBinsPhi/TMath::TwoPi())));
  for(Int_t i2=0;i2<nPrim;i2++)
  {
   if(i2==i1)continue;
   aftsTrack=anEvent->GetTrack(i2);
   if(!(aftsTrack->InRPSelection())) continue;
   phi2=aftsTrack->Phi();
   if(fUsePhiWeights && fPhiWeights) wPhi2 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi2*fnBinsPhi/TMath::TwoPi())));
   for(Int_t i3=0;i3<nPrim;i3++)
   {
    if(i3==i1||i3==i2)continue;
    aftsTrack=anEvent->GetTrack(i3);
    if(!(aftsTrack->InRPSelection())) continue;
    phi3=aftsTrack->Phi();
    if(fUsePhiWeights && fPhiWeights) wPhi3 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi3*fnBinsPhi/TMath::TwoPi())));
    for(Int_t i4=0;i4<nPrim;i4++)
    {
     if(i4==i1||i4==i2||i4==i3)continue;
     aftsTrack=anEvent->GetTrack(i4);
     if(!(aftsTrack->InRPSelection())) continue;
     phi4=aftsTrack->Phi();
     if(fUsePhiWeights && fPhiWeights) wPhi4 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi4*fnBinsPhi/TMath::TwoPi())));
     for(Int_t i5=0;i5<nPrim;i5++)
     {
      if(i5==i1||i5==i2||i5==i3||i5==i4)continue;
      aftsTrack=anEvent->GetTrack(i5);
      if(!(aftsTrack->InRPSelection())) continue;
      phi5=aftsTrack->Phi();
      if(fUsePhiWeights && fPhiWeights) wPhi5 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi5*fnBinsPhi/TMath::TwoPi())));
      for(Int_t i6=0;i6<nPrim;i6++)
      {
       if(i6==i1||i6==i2||i6==i3||i6==i4||i6==i5)continue;
       aftsTrack=anEvent->GetTrack(i6);
       if(!(aftsTrack->InRPSelection())) continue;
       phi6=aftsTrack->Phi();
       if(fUsePhiWeights && fPhiWeights) wPhi6 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi6*fnBinsPhi/TMath::TwoPi()))); 
       for(Int_t i7=0;i7<nPrim;i7++)
       {
        if(i7==i1||i7==i2||i7==i3||i7==i4||i7==i5||i7==i6)continue;
        aftsTrack=anEvent->GetTrack(i7);
        if(!(aftsTrack->InRPSelection())) continue;
        phi7=aftsTrack->Phi();
        if(fUsePhiWeights && fPhiWeights) wPhi7 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi7*fnBinsPhi/TMath::TwoPi()))); 
        for(Int_t i8=0;i8<nPrim;i8++)
        {
         if(i8==i1||i8==i2||i8==i3||i8==i4||i8==i5||i8==i6||i8==i7)continue;
         aftsTrack=anEvent->GetTrack(i8);
         if(!(aftsTrack->InRPSelection())) continue;
         phi8=aftsTrack->Phi();
         if(fUsePhiWeights && fPhiWeights) wPhi8 = fPhiWeights->GetBinContent(1+(Int_t)(TMath::Floor(phi8*fnBinsPhi/TMath::TwoPi()))); 
          
         // non-weighted: 
         //--------------------------------------------------------------------------------------------------------------------
         fDirectCorrelations->Fill(30.,cos(n*phi1+n*phi2+n*phi3+n*phi4-n*phi5-n*phi6-n*phi7-n*phi8),1.);//<8>_{n,n,n,n|n,n,n,n}
         //--------------------------------------------------------------------------------------------------------------------
         
         // weighted: 
         //...........................................................................................................................................
         if(fUsePhiWeights) fDirectCorrelationsW->Fill(120.,cos(n*phi1+n*phi2+n*phi3+n*phi4-n*phi5-n*phi6-n*phi7-n*phi8),
                                                           wPhi1*wPhi2*wPhi3*wPhi4*wPhi5*wPhi6*wPhi7*wPhi8);
         //...........................................................................................................................................
     
        } 
       } 
      } 
     }
    }  
   }
  }
 }
 
} // end of AliFlowAnalysisWithQCumulants::EvaluateNestedLoopsForIntegratedFlow(AliFlowEventSimple* anEvent)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CompareResultsFromNestedLoopsAndFromQVectorsForIntFlow(Bool_t useParticleWeights)
{
 // compare results needed for int. flow calculated with nested loops and with those calculated from Q-vectors

 
 /*


 cout<<endl;
 cout<<endl;
 cout<<"   *************************************"<<endl;
 cout<<"   **** cross-checking the formulas ****"<<endl;
 cout<<"   ****     for integrated flow     ****"<<endl;
 cout<<"   *************************************"<<endl;
 cout<<endl;
 cout<<endl;

  
 */  
    
 if(!(useParticleWeights))
 {
  
  /*
  
  cout<<"   **** results for non-weighted correlations: ****"<<endl;
  cout<<endl;
  cout<<"<2>_{1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(1)<<endl;
  cout<<"<2>_{1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(1)<<endl;
  cout<<endl;
  cout<<"<2>_{2n,2n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(2)<<endl;
  cout<<"<2>_{2n,2n} from nested loops = "<<fDirectCorrelations->GetBinContent(2)<<endl;
  cout<<endl;
  cout<<"<2>_{3n,3n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(3)<<endl;
  cout<<"<2>_{3n,3n} from nested loops = "<<fDirectCorrelations->GetBinContent(3)<<endl;
  cout<<endl;
  cout<<"<2>_{4n,4n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(4)<<endl;
  cout<<"<2>_{4n,4n} from nested loops = "<<fDirectCorrelations->GetBinContent(4)<<endl;
  cout<<endl; 
  cout<<"<3>_{2n|1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(6)<<endl;
  cout<<"<3>_{2n|1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(6)<<endl;
  cout<<endl;
  cout<<"<3>_{3n|2n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(7)<<endl;
  cout<<"<3>_{3n|2n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(7)<<endl;
  cout<<endl;
  cout<<"<3>_{4n,2n,2n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(8)<<endl;
  cout<<"<3>_{4n,2n,2n} from nested loops = "<<fDirectCorrelations->GetBinContent(8)<<endl;
  cout<<endl;
  cout<<"<3>_{4n,3n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(9)<<endl;
  cout<<"<3>_{4n,3n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(9)<<endl;
  cout<<endl; 
  cout<<"<4>_{1n,1n|1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(11)<<endl;
  cout<<"<4>_{1n,1n|1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(11)<<endl;
  cout<<endl;
  cout<<"<4>_{2n,1n|2n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(12)<<endl;
  cout<<"<4>_{2n,1n|2n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(12)<<endl;
  cout<<endl;
  cout<<"<4>_{2n,2n|2n,2n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(13)<<endl;
  cout<<"<4>_{2n,2n|2n,2n} from nested loops = "<<fDirectCorrelations->GetBinContent(13)<<endl;
  cout<<endl;
  cout<<"<4>_{3n|1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(14)<<endl;
  cout<<"<4>_{3n|1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(14)<<endl;
  cout<<endl;
  cout<<"<4>_{3n,1n|3n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(15)<<endl;
  cout<<"<4>_{3n,1n|3n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(15)<<endl;
  cout<<endl;
  cout<<"<4>_{3n,1n|2n,2n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(16)<<endl;
  cout<<"<4>_{3n,1n|2n,2n} from nested loops = "<<fDirectCorrelations->GetBinContent(16)<<endl;
  cout<<endl; 
  cout<<"<4>_{4n|2n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(17)<<endl;
  cout<<"<4>_{4n|2n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(17)<<endl;
  cout<<endl;
  cout<<"<5>_{2n,1n|1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(19)<<endl;
  cout<<"<5>_{2n,1n|1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(19)<<endl;
  cout<<endl;
  cout<<"<5>_{2n,2n|2n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(20)<<endl;
  cout<<"<5>_{2n,2n|2n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(20)<<endl;
  cout<<endl;
  cout<<"<5>_{3n,1n|2n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(21)<<endl;
  cout<<"<5>_{3n,1n|2n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(21)<<endl;
  cout<<endl;
  cout<<"<5>_{4n|1n,1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(22)<<endl;
  cout<<"<5>_{4n|1n,1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(22)<<endl;
  cout<<endl;
  cout<<"<6>_{1n,1n,1n|1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(24)<<endl;
  cout<<"<6>_{1n,1n,1n|1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(24)<<endl;
  cout<<endl; 
  cout<<"<6>_{2n,1n,1n|2n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(25)<<endl;
  cout<<"<6>_{2n,1n,1n|2n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(25)<<endl;
  cout<<endl;
  cout<<"<6>_{2n,2n|1n,1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(26)<<endl;
  cout<<"<6>_{2n,2n|1n,1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(26)<<endl;
  cout<<endl; 
  cout<<"<6>_{3n,1n|1n,1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(27)<<endl;
  cout<<"<6>_{3n,1n|1n,1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(27)<<endl;
  cout<<endl; 
  cout<<"<7>_{2n,1n,1n|1n,1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(29)<<endl;
  cout<<"<7>_{2n,1n,1n|1n,1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(29)<<endl;
  cout<<endl; 
  cout<<"<8>_{1n,1n,1n,1n|1n,1n,1n,1n} from Q-vectors    = "<<fQCorrelations[0][0]->GetBinContent(31)<<endl;
  cout<<"<8>_{1n,1n,1n,1n|1n,1n,1n,1n} from nested loops = "<<fDirectCorrelations->GetBinContent(31)<<endl;
  cout<<endl; 
  cout<<endl; 
  cout<<"   **** results for non-weighted correction terms: ****"<<endl;
  cout<<endl;
  //.........................................................................................
  cout<<"<cos(n*phi1)> from Q-vectors    = "<<fQCorrections[0][0][1]->GetBinContent(1)<<endl;
  cout<<"<cos(n*phi1)> from nested loops = "<<fDirectCorrectionsCos->GetBinContent(1)<<endl;
  cout<<endl;
  cout<<"<sin(n*phi1)> from Q-vectors    = "<<fQCorrections[0][0][0]->GetBinContent(1)<<endl;
  cout<<"<sin(n*phi1)> from nested loops = "<<fDirectCorrectionsSin->GetBinContent(1)<<endl;
  cout<<endl;  
  cout<<"<cos(n*(phi1+phi2))> from Q-vectors    = "<<fQCorrections[0][0][1]->GetBinContent(2)<<endl;
  cout<<"<cos(n*(phi1+phi2))> from nested loops = "<<fDirectCorrectionsCos->GetBinContent(2)<<endl;
  cout<<endl;
  cout<<"<sin(n*(phi1+phi2))> from Q-vectors    = "<<fQCorrections[0][0][0]->GetBinContent(2)<<endl;
  cout<<"<sin(n*(phi1+phi2))> from nested loops = "<<fDirectCorrectionsSin->GetBinContent(2)<<endl;
  cout<<endl; 
  cout<<"<cos(n*(phi1-phi2-phi3))> from Q-vectors    = "<<fQCorrections[0][0][1]->GetBinContent(3)<<endl;
  cout<<"<cos(n*(phi1-phi2-phi3))> from nested loops = "<<fDirectCorrectionsCos->GetBinContent(3)<<endl;
  cout<<endl;
  cout<<"<sin(n*(phi1-phi2-phi3))> from Q-vectors    = "<<fQCorrections[0][0][0]->GetBinContent(3)<<endl;
  cout<<"<sin(n*(phi1-phi2-phi3))> from nested loops = "<<fDirectCorrectionsSin->GetBinContent(3)<<endl;
  cout<<endl;  
  //.........................................................................................
 
 */
 
 }
 
 if(useParticleWeights)
 {
 
  /* 
      
  cout<<"   **** results for weighted correlations: ****"<<endl;
  cout<<endl;
  //.........................................................................................
  cout<<"<w1 w2 cos(n*(phi1-phi2))> from Q-vectors         = "<<fQCorrelations[1][0]->GetBinContent(1)<<endl;
  cout<<"<<w1 w2 cos(n*(phi1-phi2))> from nested loops     = "<<fDirectCorrelationsW->GetBinContent(1)<<endl;
  cout<<endl;
  cout<<"<w1^2 w2^2 cos(2n*(phi1-phi2))> from Q-vectors    = "<<fQCorrelations[1][0]->GetBinContent(2)<<endl;
  cout<<"<w1^2 w2^2 cos(2n*(phi1-phi2))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(2)<<endl;
  cout<<endl;
  cout<<"<w1^3 w2^3 cos(3n*(phi1-phi2))> from Q-vectors    = "<<fQCorrelations[1][0]->GetBinContent(3)<<endl;
  cout<<"<w1^3 w2^3 cos(3n*(phi1-phi2))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(3)<<endl;
  cout<<endl;
  cout<<"<w1^4 w2^4 cos(4n*(phi1-phi2))> from Q-vectors    = "<<fQCorrelations[1][0]->GetBinContent(4)<<endl;
  cout<<"<w1^4 w2^4 cos(4n*(phi1-phi2))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(4)<<endl;
  cout<<endl;  
  
  
  */
  
  
  /*
  cout<<"<w1^3 w2 cos(n*(phi1-phi2))> from Q-vectors       = "<<fQCorrelationsW->GetBinContent(5)<<endl;
  cout<<"<w1^3 w2 cos(n*(phi1-phi2))> from nested loops    = "<<fDirectCorrelationsW->GetBinContent(5)<<endl;
  cout<<endl;
  cout<<"<w1 w2 w3^2 cos(n*(phi1-phi2))> from Q-vectors    = "<<fQCorrelationsW->GetBinContent(6)<<endl;
  cout<<"<w1 w2 w3^2 cos(n*(phi1-phi2))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(6)<<endl;
  cout<<endl;
  cout<<"<w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))> from Q-vectors    = "<<fQCorrelationsW->GetBinContent(21)<<endl;
  cout<<"<w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(21)<<endl;
  cout<<endl;
  */ 
  
  
  /*
  
  
  cout<<"<w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))> from Q-vectors    = "<<fQCorrelations[1][0]->GetBinContent(11)<<endl;
  cout<<"<w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))> from nested loops = "<<fDirectCorrelationsW->GetBinContent(41)<<endl;
  cout<<endl;
  //.........................................................................................
  //cout<<endl; 
  //cout<<endl; 
  //cout<<"   **** results for weighted correction terms: ****"<<endl;
  //cout<<endl;
  //.........................................................................................

 */
 
 } 
 
} // end of AliFlowAnalysisWithQCumulants::CompareResultsFromNestedLoopsAndFromQVectorsForIntFlow(Bool_t useParticleWeights)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowProductOfCorrelations()
{
 // Calculate averages of products of correlations for integrated flow
 
 // a) Binning of fIntFlowProductOfCorrelationsPro is organized as follows:
 //     1st bin: <<2><4>> 
 //     2nd bin: <<2><6>>
 //     3rd bin: <<2><8>>
 //     4th bin: <<4><6>>
 //     5th bin: <<4><8>>
 //     6th bin: <<6><8>>

 Double_t dMult = (*fSMpk)(0,0); // multiplicity 

 Double_t twoEBE = 0.; // <2>
 Double_t fourEBE = 0.; // <4>
 Double_t sixEBE = 0.; // <6>
 Double_t eightEBE = 0.; // <8>
 
 // to be improved (this can be implemented better)
 if(dMult>1)
 {
  twoEBE = fIntFlowCorrelationsEBE->GetBinContent(1);
  if(dMult>3)
  { 
   fourEBE = fIntFlowCorrelationsEBE->GetBinContent(2);
   if(dMult>5) 
   {
    sixEBE = fIntFlowCorrelationsEBE->GetBinContent(3);
    if(dMult>7) 
    { 
     eightEBE = fIntFlowCorrelationsEBE->GetBinContent(4);
    }
   }
  }
 }  
 
 // <<2><4>>
 if(dMult>3)
 {
  fIntFlowProductOfCorrelationsPro->Fill(0.5,twoEBE*fourEBE,dMult*(dMult-1)*dMult*(dMult-1)*(dMult-2)*(dMult-3));
 }
 
 // <<2><6>>
 if(dMult>5)
 {
  fIntFlowProductOfCorrelationsPro->Fill(1.5,twoEBE*sixEBE,dMult*(dMult-1)*dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5));
 }
 
 // <<2><8>>
 if(dMult>7)
 {
  fIntFlowProductOfCorrelationsPro->Fill(2.5,twoEBE*eightEBE,dMult*(dMult-1)*dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7));
 }
 
 // <<4><6>>
 if(dMult>5)
 {
  fIntFlowProductOfCorrelationsPro->Fill(3.5,fourEBE*sixEBE,dMult*(dMult-1)*(dMult-2)*(dMult-3)*dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5));
 }
 
 // <<4><8>>
 if(dMult>7)
 {
  fIntFlowProductOfCorrelationsPro->Fill(4.5,fourEBE*eightEBE,dMult*(dMult-1)*(dMult-2)*(dMult-3)*
                                 dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7));
 }
 
 // <<6><8>>
 if(dMult>7)
 {
  fIntFlowProductOfCorrelationsPro->Fill(5.5,sixEBE*eightEBE,dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*
                                dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7));
 }
 
} // end of AliFlowAnalysisWithQCumulants::CalculateIntFlowProductOfCorrelations()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateCovariancesIntFlow()
{
 // a) Calculate unbiased estimators Cov(<2>,<4>), Cov(<2>,<6>), Cov(<2>,<8>), Cov(<4>,<6>), Cov(<4>,<8>) and Cov(<6>,<8>)
 //    for covariances V_(<2>,<4>), V_(<2>,<6>), V_(<2>,<8>), V_(<4>,<6>), V_(<4>,<8>) and V_(<6>,<8>).
 // b) Store in histogram fIntFlowCovariances for instance the following: 
 //
 //             Cov(<2>,<4>) * (sum_{i=1}^{N} w_{<2>}_i w_{<4>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<4>}_j)]
 // 
 //    where N is the number of events, w_{<2>} is event weight for <2> and w_{<4>} is event weight for <4>.
 // c) Binning of fIntFlowCovariances is organized as follows:
 // 
 //     1st bin: Cov(<2>,<4>) * (sum_{i=1}^{N} w_{<2>}_i w_{<4>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<4>}_j)] 
 //     2nd bin: Cov(<2>,<6>) * (sum_{i=1}^{N} w_{<2>}_i w_{<6>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<6>}_j)]
 //     3rd bin: Cov(<2>,<8>) * (sum_{i=1}^{N} w_{<2>}_i w_{<8>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<8>}_j)]
 //     4th bin: Cov(<4>,<6>) * (sum_{i=1}^{N} w_{<4>}_i w_{<6>}_i )/[(sum_{i=1}^{N} w_{<4>}_i) * (sum_{j=1}^{N} w_{<6>}_j)]
 //     5th bin: Cov(<4>,<8>) * (sum_{i=1}^{N} w_{<4>}_i w_{<8>}_i )/[(sum_{i=1}^{N} w_{<4>}_i) * (sum_{j=1}^{N} w_{<8>}_j)]
 //     6th bin: Cov(<6>,<8>) * (sum_{i=1}^{N} w_{<6>}_i w_{<8>}_i )/[(sum_{i=1}^{N} w_{<6>}_i) * (sum_{j=1}^{N} w_{<8>}_j)]
    
 for(Int_t power=0;power<2;power++)
 { 
  if(!(fIntFlowCorrelationsPro && fIntFlowProductOfCorrelationsPro 
       && fIntFlowSumOfEventWeights[power] && fIntFlowSumOfProductOfEventWeights
       && fIntFlowCovariances)) 
  {
   cout<<"WARNING: fIntFlowCorrelationsPro && fIntFlowProductOfCorrelationsPro "<<endl;
   cout<<"         && fIntFlowSumOfEventWeights[power] && fIntFlowSumOfProductOfEventWeights"<<endl;
   cout<<"         && fIntFlowCovariances is NULL in AFAWQC::FCIF() !!!!"<<endl;
   cout<<"power = "<<power<<endl;
   exit(0);
  }
 }
   
 // average 2-, 4-, 6- and 8-particle correlations for all events:
 Double_t correlation[4] = {0.};
 for(Int_t ci=0;ci<4;ci++)
 {
  correlation[ci] = fIntFlowCorrelationsPro->GetBinContent(ci+1);
 } 
 // average products of 2-, 4-, 6- and 8-particle correlations: 
 Double_t productOfCorrelations[4][4] = {{0.}};
 Int_t productOfCorrelationsLabel = 1;
 // denominators in the expressions for the unbiased estimator for covariance:
 Double_t denominator[4][4] = {{0.}};
 Int_t sumOfProductOfEventWeightsLabel1 = 1;
 // weight dependent prefactor which multiply unbiased estimators for covariances:
 Double_t wPrefactor[4][4] = {{0.}}; 
 Int_t sumOfProductOfEventWeightsLabel2 = 1;
 for(Int_t c1=0;c1<4;c1++)
 {
  for(Int_t c2=c1+1;c2<4;c2++)
  {
   productOfCorrelations[c1][c2] = fIntFlowProductOfCorrelationsPro->GetBinContent(productOfCorrelationsLabel);
   if(fIntFlowSumOfEventWeights[0]->GetBinContent(c1+1) && fIntFlowSumOfEventWeights[0]->GetBinContent(c2+1))
   {
    denominator[c1][c2] = 1.-(fIntFlowSumOfProductOfEventWeights->GetBinContent(sumOfProductOfEventWeightsLabel1))/
                             (fIntFlowSumOfEventWeights[0]->GetBinContent(c1+1) 
                              * fIntFlowSumOfEventWeights[0]->GetBinContent(c2+1));
                              
    wPrefactor[c1][c2] =  fIntFlowSumOfProductOfEventWeights->GetBinContent(sumOfProductOfEventWeightsLabel2)/ 
                          (fIntFlowSumOfEventWeights[0]->GetBinContent(c1+1)
                            * fIntFlowSumOfEventWeights[0]->GetBinContent(c2+1));
                          
                              
   }
   productOfCorrelationsLabel++;
   sumOfProductOfEventWeightsLabel1++;
   sumOfProductOfEventWeightsLabel2++;  
  }
 }
 
 // covariance label:
 Int_t covarianceLabel = 1;
 for(Int_t c1=0;c1<4;c1++)
 {
  for(Int_t c2=c1+1;c2<4;c2++)
  {
   if(denominator[c1][c2])
   {
    // covariances:
    Double_t cov = (productOfCorrelations[c1][c2]-correlation[c1]*correlation[c2])/denominator[c1][c2]; 
    // covarianced multiplied with weight dependent prefactor:
    Double_t wCov = cov * wPrefactor[c1][c2];
    fIntFlowCovariances->SetBinContent(covarianceLabel,wCov);
   }
   covarianceLabel++;
  }
 }
 
} // end of AliFlowAnalysisWithQCumulants::CalculateCovariancesIntFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::FinalizeCorrelationsIntFlow() 
{
 // From profile fIntFlowCorrelationsPro access measured correlations and spread, 
 // correctly calculate the statistical errors and store the final results and 
 // statistical errors for correlations in histogram fIntFlowCorrelationsHist.
 //
 // Remark: Statistical error of correlation is calculated as:
 //
 //          statistical error = termA * spread * termB:
 //          termA = sqrt{sum_{i=1}^{N} w^2}/(sum_{i=1}^{N} w)
 //          termB = 1/sqrt(1-termA^2)   
 
 for(Int_t power=0;power<2;power++)
 { 
  if(!(fIntFlowCorrelationsHist && fIntFlowCorrelationsPro && fIntFlowSumOfEventWeights[power])) 
  {
   cout<<"WARNING: fIntFlowCorrelationsHist && fIntFlowCorrelationsPro && fIntFlowSumOfEventWeights[power] is NULL in AFAWQC::FCIF() !!!!"<<endl;
   cout<<"power = "<<power<<endl;
   exit(0);
  }
 }
  
 for(Int_t ci=1;ci<=4;ci++) // correlation index
 {
  Double_t correlation = fIntFlowCorrelationsPro->GetBinContent(ci);
  Double_t spread = fIntFlowCorrelationsPro->GetBinError(ci);
  Double_t sumOfLinearEventWeights = fIntFlowSumOfEventWeights[0]->GetBinContent(ci);
  Double_t sumOfQuadraticEventWeights = fIntFlowSumOfEventWeights[1]->GetBinContent(ci);
  Double_t termA = 0.;
  Double_t termB = 0.;
  if(sumOfLinearEventWeights)
  {
   termA = pow(sumOfQuadraticEventWeights,0.5)/sumOfLinearEventWeights;
  } else
    {
     cout<<"WARNING: sumOfLinearEventWeights == 0 in AFAWQC::FCIF() !!!!"<<endl;
     cout<<"         (for "<<2*ci<<"-particle correlation)"<<endl;
    }
  if(1.-pow(termA,2.) > 0.)
  {
   termB = 1./pow(1-pow(termA,2.),0.5);
  } else
    {
     cout<<"WARNING: 1.-pow(termA,2.) <= 0 in AFAWQC::FCIF() !!!!"<<endl;   
     cout<<"         (for "<<2*ci<<"-particle correlation)"<<endl;
    }     
  Double_t statisticalError = termA * spread * termB;
  fIntFlowCorrelationsHist->SetBinContent(ci,correlation);
  fIntFlowCorrelationsHist->SetBinError(ci,statisticalError);
 } // end of for(Int_t ci=1;ci<=4;ci++) // correlation index                                                                
                                                                                                                              
} // end of AliFlowAnalysisWithQCumulants::FinalizeCorrelationsIntFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::FillAverageMultiplicities(Int_t nRP)
{
 // Fill profile fAverageMultiplicity to hold average multiplicities and number of events for events with nRP>=0, nRP>=1, ... , and nRP>=8
 
 // Binning of fAverageMultiplicity is organized as follows:
 //  1st bin: all events (including the empty ones)
 //  2nd bin: event with # of RPs greater or equal to 1
 //  3rd bin: event with # of RPs greater or equal to 2
 //  4th bin: event with # of RPs greater or equal to 3
 //  5th bin: event with # of RPs greater or equal to 4
 //  6th bin: event with # of RPs greater or equal to 5
 //  7th bin: event with # of RPs greater or equal to 6
 //  8th bin: event with # of RPs greater or equal to 7
 //  9th bin: event with # of RPs greater or equal to 8
 
 if(!fAvMultiplicity)
 {
  cout<<"WARNING: fAvMultiplicity is NULL in AFAWQC::FAM() !!!!"<<endl;
  exit(0);
 }
 
 if(nRP<0)
 {
  cout<<"WARNING: nRP<0 in in AFAWQC::FAM() !!!!"<<endl;
  exit(0);
 }
 
 for(Int_t i=0;i<9;i++)
 {
  if(nRP>=i) fAvMultiplicity->Fill(i+0.5,nRP,1);
 }
 
} // end of AliFlowAnalysisWithQCumulants::FillAverageMultiplicities(nRP)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateCumulantsIntFlow()
{
 // a) Calculate Q-cumulants from the measured multiparticle correlations.
 // b) Propagate the statistical errors of measured multiparticle correlations to statistical errors of Q-cumulants.  
 // c) REMARK: Q-cumulants calculated in this method are biased by non-uniform acceptance of detector !!!! 
 //            Method ApplyCorrectionForNonUniformAcceptance* (to be improved: finalize the name here)
 //            is called afterwards to correct for this bias.   
 // d) Store the results and statistical error of Q-cumulants in histogram fCumulants.
 //    Binning of fCumulants is organized as follows:
 //
 //     1st bin: QC{2}
 //     2nd bin: QC{4}
 //     3rd bin: QC{6}
 //     4th bin: QC{8}
 
 if(!(fIntFlowCorrelationsHist && fIntFlowCovariances && fIntFlowQcumulants))
 {
  cout<<"WARNING: fIntFlowCorrelationsHist && fIntFlowCovariances && fIntFlowQcumulants is NULL in AFAWQC::CCIF() !!!!"<<endl;
  exit(0);
 }
 
 // correlations:
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1); // <<2>> 
 Double_t four = fIntFlowCorrelationsHist->GetBinContent(2); // <<4>>  
 Double_t six = fIntFlowCorrelationsHist->GetBinContent(3); // <<6>> 
 Double_t eight = fIntFlowCorrelationsHist->GetBinContent(4); // <<8>>  
 
 // statistical errors of average 2-, 4-, 6- and 8-particle azimuthal correlations:
 Double_t twoError = fIntFlowCorrelationsHist->GetBinError(1); // statistical error of <2>  
 Double_t fourError = fIntFlowCorrelationsHist->GetBinError(2); // statistical error of <4>   
 Double_t sixError = fIntFlowCorrelationsHist->GetBinError(3); // statistical error of <6> 
 Double_t eightError = fIntFlowCorrelationsHist->GetBinError(4); // statistical error of <8> 
 
 // covariances (multiplied by prefactor depending on weights - see comments in CalculateCovariancesIntFlow()):
 Double_t wCov24 = fIntFlowCovariances->GetBinContent(1); // Cov(<2>,<4>) * prefactor(w_<2>,w_<4>)
 Double_t wCov26 = fIntFlowCovariances->GetBinContent(2); // Cov(<2>,<6>) * prefactor(w_<2>,w_<6>)
 Double_t wCov28 = fIntFlowCovariances->GetBinContent(3); // Cov(<2>,<8>) * prefactor(w_<2>,w_<8>)
 Double_t wCov46 = fIntFlowCovariances->GetBinContent(4); // Cov(<4>,<6>) * prefactor(w_<4>,w_<6>)
 Double_t wCov48 = fIntFlowCovariances->GetBinContent(5); // Cov(<4>,<8>) * prefactor(w_<4>,w_<8>)
 Double_t wCov68 = fIntFlowCovariances->GetBinContent(6); // Cov(<6>,<8>) * prefactor(w_<6>,w_<8>)
 
 // Q-cumulants: 
 Double_t qc2 = 0.; // QC{2}
 Double_t qc4 = 0.; // QC{4}
 Double_t qc6 = 0.; // QC{6}
 Double_t qc8 = 0.; // QC{8}
 if(two) qc2 = two; 
 if(four) qc4 = four-2.*pow(two,2.); 
 if(six) qc6 = six-9.*two*four+12.*pow(two,3.); 
 if(eight) qc8 = eight-16.*two*six-18.*pow(four,2.)+144.*pow(two,2.)*four-144.*pow(two,4.); 
 
 // statistical errors of Q-cumulants:       
 Double_t qc2Error = 0.;
 Double_t qc4Error = 0.;
 Double_t qc6Error = 0.;
 Double_t qc8Error = 0.;
 
 // squared statistical errors of Q-cumulants:       
 //Double_t qc2ErrorSquared = 0.;
 Double_t qc4ErrorSquared = 0.;
 Double_t qc6ErrorSquared = 0.;
 Double_t qc8ErrorSquared = 0.;
        
 // statistical error of QC{2}:              
 qc2Error = twoError;                     
                             
 // statistical error of QC{4}:              
 qc4ErrorSquared = 16.*pow(two,2.)*pow(twoError,2)+pow(fourError,2.)
                 - 8.*two*wCov24;                     
 if(qc4ErrorSquared>=0.)
 {
  qc4Error = pow(qc4ErrorSquared,0.5);
 } else 
   {
    cout<<"WARNING: Statistical error of QC{4} is imaginary !!!!"<<endl;
   }
                                           
 // statistical error of QC{6}:              
 qc6ErrorSquared = 81.*pow(4.*pow(two,2.)-four,2.)*pow(twoError,2.)
                 + 81.*pow(two,2.)*pow(fourError,2.)
                 + pow(sixError,2.)
                 - 162.*two*(4.*pow(two,2.)-four)*wCov24
                 + 18.*(4.*pow(two,2.)-four)*wCov26
                 - 18.*two*wCov46; 
                    
 if(qc6ErrorSquared>=0.)
 {
  qc6Error = pow(qc6ErrorSquared,0.5);
 } else 
   {
    cout<<"WARNING: Statistical error of QC{6} is imaginary !!!!"<<endl;
   }
                            
 // statistical error of QC{8}:              
 qc8ErrorSquared = 256.*pow(36.*pow(two,3.)-18.*four*two+six,2.)*pow(twoError,2.)
                 + 1296.*pow(4.*pow(two,2.)-four,2.)*pow(fourError,2.)
                 + 256.*pow(two,2.)*pow(sixError,2.)
                 + pow(eightError,2.)
                 - 1152.*(36.*pow(two,3.)-18.*four*two+six)*(4.*pow(two,2.)-four)*wCov24
                 + 512.*two*(36.*pow(two,3.)-18.*four*two+six)*wCov26
                 - 32.*(36.*pow(two,3.)-18.*four*two+six)*wCov28
                 - 1152.*two*(4.*pow(two,2.)-four)*wCov46
                 + 72.*(4.*pow(two,2.)-four)*wCov48
                 - 32.*two*wCov68;      
 if(qc8ErrorSquared>=0.)
 {
  qc8Error = pow(qc8ErrorSquared,0.5);
 } else 
   {
    cout<<"WARNING: Statistical error of QC{8} is imaginary !!!!"<<endl;
   }

 // store the results and statistical errors for Q-cumulants:
 fIntFlowQcumulants->SetBinContent(1,qc2);
 fIntFlowQcumulants->SetBinError(1,qc2Error);
 fIntFlowQcumulants->SetBinContent(2,qc4);
 fIntFlowQcumulants->SetBinError(2,qc4Error);
 fIntFlowQcumulants->SetBinContent(3,qc6);
 fIntFlowQcumulants->SetBinError(3,qc6Error);
 fIntFlowQcumulants->SetBinContent(4,qc8); 
 fIntFlowQcumulants->SetBinError(4,qc8Error); 
  
} // end of AliFlowAnalysisWithQCumulants::CalculateCumulantsIntFlow()


//================================================================================================================================ 


void AliFlowAnalysisWithQCumulants::CalculateIntFlow()
{
 // a) Calculate the final results for integrated flow estimates from Q-cumulants.
 // b) Propagate the statistical errors of measured multiparticle correlations to statistical errors of integrated flow estimates.  
 // c) Store the results and statistical errors of integrated flow estimates in histogram fIntFlow.
 //    Binning of fIntFlow is organized as follows:
 //
 //     1st bin: v{2,QC}
 //     2nd bin: v{4,QC}
 //     3rd bin: v{6,QC}
 //     4th bin: v{8,QC}
 
 if(!(fIntFlowCorrelationsHist && fIntFlowCovariances && fIntFlowQcumulants && fIntFlow))
 {
  cout<<"WARNING: fIntFlowCorrelationsHist && fIntFlowCovariances && fIntFlowQcumulants && fIntFlow is NULL in AFAWQC::CCIF() !!!!"<<endl;
  exit(0);
 }
   
 // Q-cumulants:
 Double_t qc2 = fIntFlowQcumulants->GetBinContent(1); // QC{2}  
 Double_t qc4 = fIntFlowQcumulants->GetBinContent(2); // QC{4}  
 Double_t qc6 = fIntFlowQcumulants->GetBinContent(3); // QC{6}  
 Double_t qc8 = fIntFlowQcumulants->GetBinContent(4); // QC{8}
  
 // correlations:
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1); // <<2>> 
 Double_t four = fIntFlowCorrelationsHist->GetBinContent(2); // <<4>>  
 Double_t six = fIntFlowCorrelationsHist->GetBinContent(3); // <<6>> 
 Double_t eight = fIntFlowCorrelationsHist->GetBinContent(4); // <<8>>  
 
 // statistical errors of average 2-, 4-, 6- and 8-particle azimuthal correlations:
 Double_t twoError = fIntFlowCorrelationsHist->GetBinError(1); // statistical error of <2>  
 Double_t fourError = fIntFlowCorrelationsHist->GetBinError(2); // statistical error of <4>   
 Double_t sixError = fIntFlowCorrelationsHist->GetBinError(3); // statistical error of <6> 
 Double_t eightError = fIntFlowCorrelationsHist->GetBinError(4); // statistical error of <8> 
 
 // covariances (multiplied by prefactor depending on weights - see comments in CalculateCovariancesIntFlow()):
 Double_t wCov24 = fIntFlowCovariances->GetBinContent(1); // Cov(<2>,<4>) * prefactor(w_<2>,w_<4>)
 Double_t wCov26 = fIntFlowCovariances->GetBinContent(2); // Cov(<2>,<6>) * prefactor(w_<2>,w_<6>)
 Double_t wCov28 = fIntFlowCovariances->GetBinContent(3); // Cov(<2>,<8>) * prefactor(w_<2>,w_<8>)
 Double_t wCov46 = fIntFlowCovariances->GetBinContent(4); // Cov(<4>,<6>) * prefactor(w_<4>,w_<6>)
 Double_t wCov48 = fIntFlowCovariances->GetBinContent(5); // Cov(<4>,<8>) * prefactor(w_<4>,w_<8>)
 Double_t wCov68 = fIntFlowCovariances->GetBinContent(6); // Cov(<6>,<8>) * prefactor(w_<6>,w_<8>)
  
 // integrated flow estimates:
 Double_t v2 = 0.; // v{2,QC}  
 Double_t v4 = 0.; // v{4,QC}  
 Double_t v6 = 0.; // v{6,QC}  
 Double_t v8 = 0.; // v{8,QC}
 
 // calculate integrated flow estimates from Q-cumulants: 
 if(qc2>=0.) v2 = pow(qc2,1./2.); 
 if(qc4<=0.) v4 = pow(-1.*qc4,1./4.); 
 if(qc6>=0.) v6 = pow((1./4.)*qc6,1./6.); 
 if(qc8<=0.) v8 = pow((-1./33.)*qc8,1./8.); 
   
 // statistical errors of integrated flow estimates:
 Double_t v2Error = 0.; // statistical error of v{2,QC}  
 Double_t v4Error = 0.; // statistical error of v{4,QC}  
 Double_t v6Error = 0.; // statistical error of v{6,QC}  
 Double_t v8Error = 0.; // statistical error of v{8,QC}
   
 // squares of statistical errors of integrated flow estimates:
 Double_t v2ErrorSquared = 0.; // squared statistical error of v{2,QC} 
 Double_t v4ErrorSquared = 0.; // squared statistical error of v{4,QC}   
 Double_t v6ErrorSquared = 0.; // squared statistical error of v{6,QC}   
 Double_t v8ErrorSquared = 0.; // squared statistical error of v{8,QC} 
 
 // calculate squared statistical errors of integrated flow estimates:
 if(two != 0.) 
 { 
  v2ErrorSquared = (1./(4.*two))*pow(twoError,2.);
 } 
 if(2.*pow(two,2.)-four > 0.)
 {
  v4ErrorSquared = (1./pow(2.*pow(two,2.)-four,3./2.))*
                   (pow(two,2.)*pow(twoError,2.)+(1./16.)*pow(fourError,2.)-(1./2.)*two*wCov24);
 }
 if(six-9.*four*two+12.*pow(two,3.) > 0.) 
 {
  v6ErrorSquared = ((1./2.)*(1./pow(2.,2./3.))*(1./pow(six-9.*four*two+12.*pow(two,3.),5./3.)))*
                   ((9./2.)*pow(4.*pow(two,2.)-four,2.)*pow(twoError,2.) 
                    + (9./2.)*pow(two,2.)*pow(fourError,2.)+(1./18.)*pow(sixError,2.)
                    - 9.*two*(4.*pow(two,2.)-four)*wCov24+(4.*pow(two,2.)-four)*wCov26-two*wCov46); 
 }
 if(-1.*eight+16.*six*two+18.*pow(four,2.)-144.*four*pow(two,2.)+144.*pow(two,4.) > 0.) 
 {
  v8ErrorSquared = (4./pow(33,1./4.))*(1./pow(-1.*eight+16.*six*two+18.*pow(four,2.)-144.*four*pow(two,2.)+144.*pow(two,4.),7./4.))*
                   (pow(36.*pow(two,3.)-18.*four*two+six,2.)*pow(twoError,2.)
                    + (81./16.)*pow(4.*pow(two,2.)-four,2.)*pow(fourError,2.)
                    + pow(two,2.)*pow(sixError,2.)
                    + (1./256.)*pow(eightError,2.)
                    - (9./2.)*(36.*pow(two,3.)-18.*four*two+six)*(4.*pow(two,2.)-four)*wCov24
                    + 2.*two*(36.*pow(two,3.)-18.*four*two+six)*wCov26
                    - (1./8.)*(36.*pow(two,3.)-18.*four*two+six)*wCov28                    
                    - (9./2.)*two*(4.*pow(two,2.)-four)*wCov46                   
                    + (9./32.)*(4.*pow(two,2.)-four)*wCov48                    
                    - (1./8.)*two*wCov68);
 } 

 // calculate statistical errors of integrated flow estimates: 
 if(v2ErrorSquared >= 0.)
 {
  v2Error = pow(v2ErrorSquared,0.5);
 } else
   {
    cout<<"WARNING: Statistical error of v{2,QC} is imaginary !!!!"<<endl;
   }    
 if(v4ErrorSquared >= 0.)
 {
  v4Error = pow(v4ErrorSquared,0.5);
 } else
   {
    cout<<"WARNING: Statistical error of v{4,QC} is imaginary !!!!"<<endl;
   }     
 if(v6ErrorSquared >= 0.)
 {
  v6Error = pow(v6ErrorSquared,0.5);
 } else
   {
    cout<<"WARNING: Statistical error of v{6,QC} is imaginary !!!!"<<endl;
   }     
 if(v8ErrorSquared >= 0.)
 {
  v8Error = pow(v8ErrorSquared,0.5);
 } else
   {
    cout<<"WARNING: Statistical error of v{8,QC} is imaginary !!!!"<<endl;
   }    
                     
 // store the results and statistical errors of integrated flow estimates:
 fIntFlow->SetBinContent(1,v2);
 fIntFlow->SetBinError(1,v2Error);
 fIntFlow->SetBinContent(2,v4);
 fIntFlow->SetBinError(2,v4Error);
 fIntFlow->SetBinContent(3,v6);
 fIntFlow->SetBinError(3,v6Error);
 fIntFlow->SetBinContent(4,v8);
 fIntFlow->SetBinError(4,v8Error);
       
} // end of AliFlowAnalysisWithQCumulants::CalculateIntFlow()


//================================================================================================================================ 


void AliFlowAnalysisWithQCumulants::FillCommonHistResultsIntFlow()
{
 // Fill in AliFlowCommonHistResults histograms relevant for 'NONAME' integrated flow (to be improved (name))
 
 if(!fIntFlow)
 {
  cout<<"WARNING: fIntFlow is NULL in AFAWQC::FCHRIF() !!!!"<<endl;
  exit(0); 
 }  
    
 if(!(fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th))
 {
  cout<<"WARNING: fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th"<<endl; 
  cout<<"         is NULL in AFAWQC::FCHRIF() !!!!"<<endl;
  exit(0);
 }
 
 Double_t v2 = fIntFlow->GetBinContent(1);
 Double_t v4 = fIntFlow->GetBinContent(2);
 Double_t v6 = fIntFlow->GetBinContent(3);
 Double_t v8 = fIntFlow->GetBinContent(4);
  
 Double_t v2Error = fIntFlow->GetBinError(1);
 Double_t v4Error = fIntFlow->GetBinError(2);
 Double_t v6Error = fIntFlow->GetBinError(3);
 Double_t v8Error = fIntFlow->GetBinError(4);
 
 fCommonHistsResults2nd->FillIntegratedFlow(v2,v2Error); // to be improved (hardwired 2nd in the name)  
 fCommonHistsResults4th->FillIntegratedFlow(v4,v4Error); // to be improved (hardwired 4th in the name)
 fCommonHistsResults6th->FillIntegratedFlow(v6,v6Error); // to be improved (hardwired 6th in the name)
 fCommonHistsResults8th->FillIntegratedFlow(v8,v8Error); // to be improved (hardwired 8th in the name) 

} // end of AliFlowAnalysisWithQCumulants::FillCommonHistResultsIntFlow()


//================================================================================================================================ 


/*
void AliFlowAnalysisWithQCumulants::ApplyCorrectionForNonUniformAcceptanceToCumulantsForIntFlow(Bool_t useParticleWeights, TString eventWeights)
{
 // apply correction for non-uniform acceptance to cumulants for integrated flow 
 // (Remark: non-corrected cumulants are accessed from fCumulants[pW][0], corrected cumulants are stored in fCumulants[pW][1])
 
 // shortcuts for the flags:
 Int_t pW = (Int_t)(useParticleWeights); // 0=pWeights not used, 1=pWeights used
 Int_t eW = -1;
 
 if(eventWeights == "exact")
 {
  eW = 0;
 }
 
 if(!(fCumulants[pW][eW][0] && fCumulants[pW][eW][1] && fCorrections[pW][eW]))
 {
  cout<<"WARNING: fCumulants[pW][eW][0] && fCumulants[pW][eW][1] && fCorrections[pW][eW] is NULL in AFAWQC::ACFNUATCFIF() !!!!"<<endl;
  cout<<"pW = "<<pW<<endl;
  cout<<"eW = "<<eW<<endl;
  exit(0);
 } 
  
 // non-corrected cumulants:
 Double_t qc2 = fCumulants[pW][eW][0]->GetBinContent(1); 
 Double_t qc4 = fCumulants[pW][eW][0]->GetBinContent(2); 
 Double_t qc6 = fCumulants[pW][eW][0]->GetBinContent(3); 
 Double_t qc8 = fCumulants[pW][eW][0]->GetBinContent(4); 
 // statistical error of non-corrected cumulants:  
 Double_t qc2Error = fCumulants[pW][eW][0]->GetBinError(1); 
 Double_t qc4Error = fCumulants[pW][eW][0]->GetBinError(2); 
 Double_t qc6Error = fCumulants[pW][eW][0]->GetBinError(3); 
 Double_t qc8Error = fCumulants[pW][eW][0]->GetBinError(4); 
 // corrections for non-uniform acceptance:
 Double_t qc2Correction = fCorrections[pW][eW]->GetBinContent(1); 
 Double_t qc4Correction = fCorrections[pW][eW]->GetBinContent(2); 
 Double_t qc6Correction = fCorrections[pW][eW]->GetBinContent(3); 
 Double_t qc8Correction = fCorrections[pW][eW]->GetBinContent(4); 
 // corrected cumulants:
 Double_t qc2Corrected = qc2 + qc2Correction;
 Double_t qc4Corrected = qc4 + qc4Correction;
 Double_t qc6Corrected = qc6 + qc6Correction;
 Double_t qc8Corrected = qc8 + qc8Correction;
  
 // ... to be improved (I need here also to correct error of QCs for NUA. 
 // For simplicity sake I assume at the moment that this correction is negliglible, but it will be added eventually...)
 
 // store corrected results and statistical errors for cumulants:   
 fCumulants[pW][eW][1]->SetBinContent(1,qc2Corrected);
 fCumulants[pW][eW][1]->SetBinContent(2,qc4Corrected);
 fCumulants[pW][eW][1]->SetBinContent(3,qc6Corrected);
 fCumulants[pW][eW][1]->SetBinContent(4,qc8Corrected);
 fCumulants[pW][eW][1]->SetBinError(1,qc2Error); // to be improved (correct also qc2Error for NUA)
 fCumulants[pW][eW][1]->SetBinError(2,qc4Error); // to be improved (correct also qc4Error for NUA)
 fCumulants[pW][eW][1]->SetBinError(3,qc6Error); // to be improved (correct also qc6Error for NUA)
 fCumulants[pW][eW][1]->SetBinError(4,qc8Error); // to be improved (correct also qc8Error for NUA)  
  
} // end of AliFlowAnalysisWithQCumulants::ApplyCorrectionForNonUniformAcceptanceToCumulantsForIntFlow(Bool_t useParticleWeights, TString eventWeights)
*/


//================================================================================================================================


/*  
void AliFlowAnalysisWithQCumulants::PrintQuantifyingCorrectionsForNonUniformAcceptance(Bool_t useParticleWeights, TString eventWeights)
{
 // print on the screen QC{n,biased}/QC{n,corrected}
 
 // shortcuts for the flags:
 Int_t pW = (Int_t)(useParticleWeights); // 0=pWeights not used, 1=pWeights used
 
 Int_t eW = -1;
 
 if(eventWeights == "exact")
 {
  eW = 0;
 } 
 
 if(!(fCumulants[pW][eW][0] && fCumulants[pW][eW][1]))
 {
  cout<<"WARNING: fCumulants[pW][eW][0] && fCumulants[pW][eW][1] is NULL in AFAWQC::PQCFNUA() !!!!"<<endl;
  cout<<"pW = "<<pW<<endl;
  cout<<"eW = "<<eW<<endl;
  exit(0);
 }
   
 cout<<endl;
 cout<<" Quantifying the bias to Q-cumulants from"<<endl;
 cout<<"  non-uniform acceptance of the detector:"<<endl;
 cout<<endl;
  
 if(fCumulants[pW][eW][1]->GetBinContent(1)) 
 { 
  cout<<"  QC{2,biased}/QC{2,corrected} = "<<(fCumulants[pW][eW][0]->GetBinContent(1))/(fCumulants[pW][eW][1]->GetBinContent(1))<<endl;   
 }
 if(fCumulants[pW][eW][1]->GetBinContent(2)) 
 { 
  cout<<"  QC{4,biased}/QC{4,corrected} = "<<fCumulants[pW][eW][0]->GetBinContent(2)/fCumulants[pW][eW][1]->GetBinContent(2)<<endl;   
 }
 
 cout<<endl;
            
} // end of AliFlowAnalysisWithQCumulants::PrintQuantifyingCorrectionsForNonUniformAcceptance(Bool_t useParticleWeights, TString eventWeights)
*/


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateWeightedCorrelationsForIntegratedFlow()
{
 // calculate all weighted correlations needed for integrated flow and store them in 1D profile fQCorrelations[1][eW] and fQExtraCorrelations[1][eW]
 
 /*
 
 for(Int_t eW=0;eW<2;eW++)
 {
  if(!(fQCorrelationsEBE[1] && fQCorrelations[1][eW]))
  {
   cout<<"WARNING: fQCorrelationsEBE[1] && fQCorrelations[1][eW] is NULL in AFAWQC::CWCFIF() !!!!"<<endl;
   cout<<"eW = "<<eW<<endl;
   exit(0);
  }
 }
 
 // Remark 1: binning of fQCorrelations[W] is organized as follows:
 //..............................................................................................
 //       ---- bins 1-20: 2-particle correlations ----
 // 1st bin: two1n1nW1W1 = <w1 w2 cos(n*(phi1-phi2))>
 // 2nd bin: two2n2nW2W2 = <w1^2 w2^2 cos(2n*(phi1-phi2))>
 // 3rd bin: two3n3nW3W3 = <w1^3 w2^3 cos(3n*(phi1-phi2))>
 // 4th bin: two4n4nW4W4 = <w1^4 w2^4 cos(4n*(phi1-phi2))>
 // 5th bin: two1n1nW3W1 = <w1^3 w2 cos(n*(phi1-phi2))>
 // 6th bin: two1n1nW1W1W2 = <w1 w2 w3^2 cos(n*(phi1-phi2))>  
 //       ---- bins 21-40: 3-particle correlations ----
 // 21st bin: three2n1n1nW2W1W1 = <w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))> 
 //       ---- bins 41-60: 4-particle correlations ----
 // 41st bin: four1n1n1n1nW1W1W1W1 = <w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))>
 //       ---- bins 61-80: 5-particle correlations ---- 
 //       ---- bins 81-100: 6-particle correlations ----
 //       ---- bins 101-120: 7-particle correlations ----
 //       ---- bins 121-140: 8-particle correlations ----
 //..............................................................................................
 
 // multiplicity (number of particles used to determine the reaction plane)
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n1k = (*fReQ)(0,1);
 Double_t dReQ2n2k = (*fReQ)(1,2);
 Double_t dReQ3n3k = (*fReQ)(2,3);
 Double_t dReQ4n4k = (*fReQ)(3,4);
 Double_t dReQ1n3k = (*fReQ)(0,3);
 Double_t dImQ1n1k = (*fImQ)(0,1);
 Double_t dImQ2n2k = (*fImQ)(1,2);
 Double_t dImQ3n3k = (*fImQ)(2,3);
 Double_t dImQ4n4k = (*fImQ)(3,4);
 Double_t dImQ1n3k = (*fImQ)(0,3);

 // dMs are variables introduced in order to simplify some Eqs. bellow:
 //..............................................................................................
 Double_t dM11 = (*fSMpk)(1,1)-(*fSMpk)(0,2); // dM11 = sum_{i,j=1,i!=j}^M w_i w_j
 Double_t dM22 = (*fSMpk)(1,2)-(*fSMpk)(0,4); // dM22 = sum_{i,j=1,i!=j}^M w_i^2 w_j^2
 Double_t dM33 = (*fSMpk)(1,3)-(*fSMpk)(0,6); // dM33 = sum_{i,j=1,i!=j}^M w_i^3 w_j^3
 Double_t dM44 = (*fSMpk)(1,4)-(*fSMpk)(0,8); // dM44 = sum_{i,j=1,i!=j}^M w_i^4 w_j^4
 Double_t dM31 = (*fSMpk)(0,3)*(*fSMpk)(0,1)-(*fSMpk)(0,4); // dM31 = sum_{i,j=1,i!=j}^M w_i^3 w_j
 Double_t dM211 = (*fSMpk)(0,2)*(*fSMpk)(1,1)-2.*(*fSMpk)(0,3)*(*fSMpk)(0,1)
                - (*fSMpk)(1,2)+2.*(*fSMpk)(0,4); // dM211 = sum_{i,j,k=1,i!=j!=k}^M w_i^2 w_j w_k
 Double_t dM1111 = (*fSMpk)(3,1)-6.*(*fSMpk)(0,2)*(*fSMpk)(1,1)  
                 + 8.*(*fSMpk)(0,3)*(*fSMpk)(0,1)
                 + 3.*(*fSMpk)(1,2)-6.*(*fSMpk)(0,4); // dM1111 = sum_{i,j,k,l=1,i!=j!=k!=l}^M w_i w_j w_k w_l
 //..............................................................................................

 //  ***********************************************
 //  **** weighted multi-particle correlations: ****
 //  ***********************************************
 //.............................................................................................. 
 // weighted 2-particle correlations:
 Double_t two1n1nW1W1 = 0.; // <w1 w2 cos(n*(phi1-phi2))>
 Double_t two2n2nW2W2 = 0.; // <w1^2 w2^2 cos(2n*(phi1-phi2))>
 Double_t two3n3nW3W3 = 0.; // <w1^3 w2^3 cos(3n*(phi1-phi2))>
 Double_t two4n4nW4W4 = 0.; // <w1^4 w2^4 cos(4n*(phi1-phi2))>
 Double_t two1n1nW3W1 = 0.; // <w1^3 w2 cos(n*(phi1-phi2))>
 Double_t two1n1nW1W1W2 = 0.; // <w1 w2 w3^2 cos(n*(phi1-phi2))> 
 
 if(dMult>1) 
 { 
  if(dM11)
  {
   two1n1nW1W1 = (pow(dReQ1n1k,2)+pow(dImQ1n1k,2)-(*fSMpk)(0,2))/dM11; 
   
   // average weighted correlation <w1 w2 cos(n*(phi1-phi2))> for single event: 
   fQCorrelationsEBE[1]->SetBinContent(1,two1n1nW1W1);

   // average weighted correlation <w1 w2 cos(n*(phi1-phi2))> for all events:
   //fQCorrelationsW->Fill(0.,two1n1nW1W1,dM11);
   fQCorrelations[1][0]->Fill(0.5,two1n1nW1W1,dM11);
  }
  if(dM22)
  {
   two2n2nW2W2 = (pow(dReQ2n2k,2)+pow(dImQ2n2k,2)-(*fSMpk)(0,4))/dM22; 
   //fQCorrelationsW->Fill(1.,two2n2nW2W2,dM22); 
   fQCorrelations[1][0]->Fill(1.5,two2n2nW2W2,dM22);
  }
  if(dM33)
  {
   two3n3nW3W3 = (pow(dReQ3n3k,2)+pow(dImQ3n3k,2)-(*fSMpk)(0,6))/dM33;
   //fQCorrelationsW->Fill(2.,two3n3nW3W3,dM33);
   fQCorrelations[1][0]->Fill(2.5,two3n3nW3W3,dM33);
  }
  if(dM44)
  {
   two4n4nW4W4 = (pow(dReQ4n4k,2)+pow(dImQ4n4k,2)-(*fSMpk)(0,8))/dM44; 
   //fQCorrelationsW->Fill(3.,two4n4nW4W4,dM44); 
   fQCorrelations[1][0]->Fill(3.5,two4n4nW4W4,dM44); 
  } 
  if(dM31)
  {
   two1n1nW3W1 = (dReQ1n3k*dReQ1n1k+dImQ1n3k*dImQ1n1k-(*fSMpk)(0,4))/dM31; 
   //fQCorrelationsW->Fill(4.,two1n1nW3W1,dM31);  
  } 
  if(dM211)
  {
   two1n1nW1W1W2 = ((*fSMpk)(0,2)*(pow(dReQ1n1k,2)+pow(dImQ1n1k,2)-(*fSMpk)(0,2))
                 - 2.*(dReQ1n3k*dReQ1n1k+dImQ1n3k*dImQ1n1k
                 - (*fSMpk)(0,4)))/dM211;
   //fQCorrelationsW->Fill(5.,two1n1nW1W1W2,dM211);  
  }  
 } // end of if(dMult>1)
 //..............................................................................................
 
 //..............................................................................................
 // weighted 3-particle correlations:
 Double_t three2n1n1nW2W1W1 = 0.; // <w1^2 w2 w3 cos(n*(2phi1-phi2-phi3))>
 
 if(dMult>2) 
 { 
  if(dM211)
  {                                                       
   three2n1n1nW2W1W1 = (pow(dReQ1n1k,2.)*dReQ2n2k+2.*dReQ1n1k*dImQ1n1k*dImQ2n2k-pow(dImQ1n1k,2.)*dReQ2n2k
                     - 2.*(dReQ1n3k*dReQ1n1k+dImQ1n3k*dImQ1n1k)
                     - pow(dReQ2n2k,2)-pow(dImQ2n2k,2)
                     + 2.*(*fSMpk)(0,4))/dM211;                                                                               
   //fQCorrelationsW->Fill(20.,three2n1n1nW2W1W1,dM211);
  } 
 } // end of if(dMult>2) 
 //..............................................................................................
 
 //..............................................................................................
 // weighted 4-particle correlations:
 Double_t four1n1n1n1nW1W1W1W1 = 0.; // <w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))>
 if(dMult>3) 
 { 
  if(dM1111)
  {      
   four1n1n1n1nW1W1W1W1 = (pow(pow(dReQ1n1k,2.)+pow(dImQ1n1k,2.),2)
                        - 2.*(pow(dReQ1n1k,2.)*dReQ2n2k+2.*dReQ1n1k*dImQ1n1k*dImQ2n2k-pow(dImQ1n1k,2.)*dReQ2n2k)
                        + 8.*(dReQ1n3k*dReQ1n1k+dImQ1n3k*dImQ1n1k)
                        + (pow(dReQ2n2k,2)+pow(dImQ2n2k,2))
                        - 4.*(*fSMpk)(0,2)*(pow(dReQ1n1k,2)+pow(dImQ1n1k,2))
                        - 6.*(*fSMpk)(0,4)+2.*(*fSMpk)(1,2))/dM1111;  
                        
   // average weighted correlation <w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))> for single event: 
   fQCorrelationsEBE[1]->SetBinContent(11,four1n1n1n1nW1W1W1W1);

   // average weighted correlation <w1 w2 w3 w4 cos(n*(phi1+phi2-phi3-phi4))> for all events:                        
   //fQCorrelationsW->Fill(40.,four1n1n1n1nW1W1W1W1,dM1111);
   
   fQCorrelations[1][0]->Fill(10.5,four1n1n1n1nW1W1W1W1,dM1111);
  } 
 } // end of if(dMult>3) 
 //..............................................................................................
 
 */
 
} // end of AliFlowAnalysisWithQCumulants::CalculateWeightedCorrelationsForIntegratedFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateWeightedQProductsForIntFlow() // to be improved (completed)
{
 // calculate averages like <<2><4>>, <<2><6>>, <<4><6>>, etc. which are needed to calculate covariances 
 // Remark: here we take weighted correlations!
 
 /*
 
 // binning of fQProductsW is organized as follows:
 // 
 // 1st bin: <2><4> 
 // 2nd bin: <2><6>
 // 3rd bin: <2><8>
 // 4th bin: <4><6>
 // 5th bin: <4><8>
 // 6th bin: <6><8>
 
 Double_t dMult = (*fSMpk)(0,0); // multiplicity (number of particles used to determine the reaction plane)

 Double_t dM11 = (*fSMpk)(1,1)-(*fSMpk)(0,2); // dM11 = sum_{i,j=1,i!=j}^M w_i w_j
 Double_t dM1111 = (*fSMpk)(3,1)-6.*(*fSMpk)(0,2)*(*fSMpk)(1,1)  
                 + 8.*(*fSMpk)(0,3)*(*fSMpk)(0,1)
                 + 3.*(*fSMpk)(1,2)-6.*(*fSMpk)(0,4); // dM1111 = sum_{i,j,k,l=1,i!=j!=k!=l}^M w_i w_j w_k w_l

 Double_t twoEBEW = 0.; // <2>
 Double_t fourEBEW = 0.; // <4>
 
 twoEBEW = fQCorrelationsEBE[1]->GetBinContent(1);
 fourEBEW = fQCorrelationsEBE[1]->GetBinContent(11);
 
 // <2><4>
 if(dMult>3)
 {
  fQProducts[1][0]->Fill(0.5,twoEBEW*fourEBEW,dM11*dM1111);
 }
 
 */
 
} // end of AliFlowAnalysisWithQCumulants::CalculateWeightedQProductsForIntFlow()  


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::InitializeArraysForIntFlow()
{
 // Initialize all arrays used to calculate integrated flow.
 
 for(Int_t sc=0;sc<2;sc++) // sin or cos terms
 {
  fIntFlowCorrectionTermsForNUAEBE[sc] = NULL;
  fIntFlowCorrectionTermsForNUAPro[sc] = NULL;
  fIntFlowCorrectionTermsForNUAHist[sc] = NULL;
 }
   
 for(Int_t power=0;power<2;power++) // linear or quadratic 
 {
  fIntFlowSumOfEventWeights[power] = NULL;    
 }
 
} // end of void AliFlowAnalysisWithQCumulants::InitializeArraysForIntFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::InitializeArraysForDiffFlow()
{
 // Initialize all arrays needed to calculate differential flow.
 //  a) Initialize lists holding profiles;
 //  b) Initialize lists holding histograms;
 //  c) Initialize event-by-event quantities;
 //  d) Initialize profiles;
 //  e) Initialize histograms holding final results.
 
 // a) Initialize lists holding profiles;
 for(Int_t t=0;t<2;t++) // type (RP, POI)
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   fDiffFlowCorrelationsProList[t][pe] = NULL;
   fDiffFlowProductOfCorrelationsProList[t][pe] = NULL;
   fDiffFlowCorrectionsProList[t][pe] = NULL;
  }
 }  
 
 // b) Initialize lists holding histograms;
 for(Int_t t=0;t<2;t++) // type (RP, POI)
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   fDiffFlowCorrelationsHistList[t][pe] = NULL;
   for(Int_t power=0;power<2;power++)
   {
    fDiffFlowSumOfEventWeightsHistList[t][pe][power] = NULL;
   } // end of for(Int_t power=0;power<2;power++)  
   fDiffFlowSumOfProductOfEventWeightsHistList[t][pe] = NULL;
   fDiffFlowCovariancesHistList[t][pe] = NULL;
   fDiffFlowCumulantsHistList[t][pe] = NULL;
   fDiffFlowHistList[t][pe] = NULL;
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // enf of for(Int_t t=0;t<2;t++) // type (RP, POI) 
 
 // c) Initialize event-by-event quantities:
 // 1D:
 for(Int_t t=0;t<3;t++) // type (RP, POI, POI&&RP)
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  { 
   for(Int_t m=0;m<4;m++) // multiple of harmonic
   {
    for(Int_t k=0;k<9;k++) // power of weight
    {
     fReRPQ1dEBE[t][pe][m][k] = NULL;
     fImRPQ1dEBE[t][pe][m][k] = NULL;
     fs1dEBE[t][pe][k] = NULL; // to be improved (this doesn't need to be within loop over m)
    }   
   }
  }
 }
 // 2D:  
 for(Int_t t=0;t<3;t++) // type (RP, POI, POI&&RP)
 {
  for(Int_t m=0;m<4;m++) // multiple of harmonic
  {
   for(Int_t k=0;k<9;k++) // power of weight
   {
    fReRPQ2dEBE[t][m][k] = NULL;
    fImRPQ2dEBE[t][m][k] = NULL;
    fs2dEBE[t][k] = NULL; // to be improved (this doesn't need to be within loop over m)
   }   
  }
 }
 
 // d) Initialize profiles:
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t ci=0;ci<4;ci++) // correlation index
   {
    fDiffFlowCorrelationsPro[t][pe][ci] = NULL;
   } // end of for(Int_t ci=0;ci<4;ci++)   
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=0;mci2<8;mci2++) // mixed correlation index
    {
     fDiffFlowProductOfCorrelationsPro[t][pe][mci1][mci2] = NULL;
    } // end of for(Int_t mci2=0;mci2<8;mci2++) // mixed correlation index
   } // end of for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index  
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI
  
 // e) Initialize histograms holding final results.
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t ci=0;ci<4;ci++) // correlation index
   {
    fDiffFlowCorrelationsHist[t][pe][ci] = NULL;
    fDiffFlowCumulants[t][pe][ci] = NULL;
    fDiffFlow[t][pe][ci] = NULL;
   } // end of for(Int_t ci=0;ci<4;ci++)    
   for(Int_t covarianceIndex=0;covarianceIndex<5;covarianceIndex++) 
   {
    fDiffFlowCovariances[t][pe][covarianceIndex] = NULL;     
   } // end of for(Int_t covarianceIndex=0;covarianceIndex<5;covarianceIndex++) 
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI

 
 // sum of event weights for reduced correlations:
 for(Int_t t=0;t<2;t++) // type = RP or POI
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t p=0;p<2;p++) // power of weight is 1 or 2
   {
    for(Int_t ew=0;ew<4;ew++) // event weight index for reduced correlations
    {
     fDiffFlowSumOfEventWeights[t][pe][p][ew] = NULL;
    } 
   }   
  }
 }
 // product of event weights for both types of correlations:
 for(Int_t t=0;t<2;t++) // type = RP or POI
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=0;mci2<8;mci2++) // mixed correlation index
    {
     fDiffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2] = NULL;
    } 
   }   
  }
 }

 
 
 
 /*
 
 // nested lists in fDiffFlowProfiles:
 for(Int_t t=0;t<2;t++)
 {
  fDFPType[t] = NULL;
  for(Int_t pW=0;pW<2;pW++) // particle weights not used (0) or used (1)
  {
   fDFPParticleWeights[t][pW] = NULL;
   for(Int_t eW=0;eW<2;eW++)
   {   
    fDFPEventWeights[t][pW][eW] = NULL;
    fDiffFlowCorrelations[t][pW][eW] = NULL;
    fDiffFlowProductsOfCorrelations[t][pW][eW] = NULL;
    for(Int_t sc=0;sc<2;sc++)
    {
     fDiffFlowCorrectionTerms[t][pW][eW][sc] = NULL;
    }
   } 
  }
 }  
 
 
 */
 
  
  
  /*
  for(Int_t pW=0;pW<2;pW++) // particle weights not used (0) or used (1)
  {
   for(Int_t eW=0;eW<2;eW++)
   {
    // correlations:
    for(Int_t correlationIndex=0;correlationIndex<4;correlationIndex++)
    {
     fCorrelationsPro[t][pW][eW][correlationIndex] = NULL;
    }
    // products of correlations:
    for(Int_t productOfCorrelationsIndex=0;productOfCorrelationsIndex<6;productOfCorrelationsIndex++)
    {
     fProductsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex] = NULL;
    }
    // correction terms:
    for(Int_t sc=0;sc<2;sc++)
    {
     for(Int_t correctionsIndex=0;correctionsIndex<2;correctionsIndex++)
     {
      fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex] = NULL;
     } 
    } 
   }
  } 
  */
  

 
 
 
 /*
 
 
 
 // nested lists in fDiffFlowResults:
 for(Int_t t=0;t<2;t++)
 {
  fDFRType[t] = NULL;
  for(Int_t pW=0;pW<2;pW++) // particle weights not used (0) or used (1)
  {
   fDFRParticleWeights[t][pW] = NULL;
   for(Int_t eW=0;eW<2;eW++)
   {    
    fDFREventWeights[t][pW][eW] = NULL;
    fDiffFlowFinalCorrelations[t][pW][eW] = NULL;
    fDiffFlowFinalCorrections[t][pW][eW] = NULL;
    fDiffFlowFinalCovariances[t][pW][eW] = NULL;
    for(Int_t nua=0;nua<2;nua++)
    {
     fDFRCorrections[t][pW][eW][nua] = NULL;   
     fDiffFlowFinalCumulants[t][pW][eW][nua] = NULL;   
     fDiffFlowFinalFlow[t][pW][eW][nua] = NULL;
    }
   } 
  }
 }  
 
 // 2D and 1D histograms in nested lists in fDiffFlowResults:
 for(Int_t t=0;t<2;t++) 
 {
  fNonEmptyBins2D[t] = NULL;
  for(Int_t pe=0;pe<2;pe++)
  {
   fNonEmptyBins1D[t][pe] = NULL;
  }
  for(Int_t pW=0;pW<2;pW++) // particle weights not used (0) or used (1)
  {
   for(Int_t eW=0;eW<2;eW++)
   {
    // correlations:
    for(Int_t correlationIndex=0;correlationIndex<4;correlationIndex++) 
    {
     fFinalCorrelations2D[t][pW][eW][correlationIndex] = NULL;
     for(Int_t pe=0;pe<2;pe++)
     {
      fFinalCorrelations1D[t][pW][eW][pe][correlationIndex] = NULL;   
     }
    }
    // corrections:
    for(Int_t correctionIndex=0;correctionIndex<4;correctionIndex++) 
    {
     fFinalCorrections2D[t][pW][eW][correctionIndex] = NULL;
     for(Int_t pe=0;pe<2;pe++)
     {
      fFinalCorrections1D[t][pW][eW][pe][correctionIndex] = NULL;     
     }
    }
    // covariances:
    for(Int_t covarianceIndex=0;covarianceIndex<5;covarianceIndex++) 
    {
     fFinalCovariances2D[t][pW][eW][covarianceIndex] = NULL;
     for(Int_t pe=0;pe<2;pe++)
     {
      fFinalCovariances1D[t][pW][eW][pe][covarianceIndex] = NULL;     
     }
    }
    for(Int_t nua=0;nua<2;nua++) 
    {  
     // cumulants:
     for(Int_t cumulantIndex=0;cumulantIndex<4;cumulantIndex++) 
     {
      fFinalCumulants2D[t][pW][eW][nua][cumulantIndex] = NULL;
      fFinalCumulantsPt[t][pW][eW][nua][cumulantIndex] = NULL;     
      fFinalCumulantsEta[t][pW][eW][nua][cumulantIndex] = NULL;       
     }
     // final flow:
     for(Int_t finalFlowIndex=0;finalFlowIndex<4;finalFlowIndex++) 
     {
      fFinalFlow2D[t][pW][eW][nua][finalFlowIndex] = NULL;
      fFinalFlowPt[t][pW][eW][nua][finalFlowIndex] = NULL;   
      fFinalFlowEta[t][pW][eW][nua][finalFlowIndex] = NULL;   
     }
    } 
   }  
  }
 }
  
 
 */
  
  
} // end of AliFlowAnalysisWithQCumulants::InitializeArraysForDiffFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateCorrelationsForDifferentialFlow2D(TString type)
{
 // calculate all reduced correlations needed for differential flow for each (pt,eta) bin: 
 
 if(type == "RP") // to be improved (removed)
 {
  cout<<endl;
 }
 // ... 
 
 /*
 
 Int_t typeFlag = -1; 
  
 // reduced correlations ares stored in fCorrelationsPro[t][pW][index] and are indexed as follows:
 // index:
 // 0: <2'>
 // 1: <4'>

 // multiplicity:
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n = (*fReQ)(0,0);
 Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 Double_t dImQ1n = (*fImQ)(0,0);
 Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);

 // looping over all (pt,eta) bins and calculating correlations needed for differential flow: 
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   // real and imaginary parts of p_{m*n,0} (non-weighted Q-vector evaluated for POIs in particular (pt,eta) bin): 
   Double_t p1n0kRe = 0.;
   Double_t p1n0kIm = 0.;

   // number of POIs in particular (pt,eta) bin:
   Double_t mp = 0.;

   // real and imaginary parts of q_{m*n,0} (non-weighted Q-vector evaluated for particles which are both RPs and POIs in particular (pt,eta) bin):
   Double_t q1n0kRe = 0.;
   Double_t q1n0kIm = 0.;
   Double_t q2n0kRe = 0.;
   Double_t q2n0kIm = 0.;

   // number of particles which are both RPs and POIs in particular (pt,eta) bin:
   Double_t mq = 0.;
   
   // q_{m*n,0}:
   q1n0kRe = fReEBE2D[2][0][0]->GetBinContent(fReEBE2D[2][0][0]->GetBin(p,e))
           * fReEBE2D[2][0][0]->GetBinEntries(fReEBE2D[2][0][0]->GetBin(p,e));
   q1n0kIm = fImEBE2D[2][0][0]->GetBinContent(fImEBE2D[2][0][0]->GetBin(p,e))
           * fImEBE2D[2][0][0]->GetBinEntries(fImEBE2D[2][0][0]->GetBin(p,e));
   q2n0kRe = fReEBE2D[2][1][0]->GetBinContent(fReEBE2D[2][1][0]->GetBin(p,e))
           * fReEBE2D[2][1][0]->GetBinEntries(fReEBE2D[2][1][0]->GetBin(p,e));
   q2n0kIm = fImEBE2D[2][1][0]->GetBinContent(fImEBE2D[2][1][0]->GetBin(p,e))
           * fImEBE2D[2][1][0]->GetBinEntries(fImEBE2D[2][1][0]->GetBin(p,e));
           
   mq = fReEBE2D[2][0][0]->GetBinEntries(fReEBE2D[2][0][0]->GetBin(p,e)); // to be improved (cross-checked by accessing other profiles here)
   
   if(type == "POI")
   {
    // p_{m*n,0}:
    p1n0kRe = fReEBE2D[1][0][0]->GetBinContent(fReEBE2D[1][0][0]->GetBin(p,e))
            * fReEBE2D[1][0][0]->GetBinEntries(fReEBE2D[1][0][0]->GetBin(p,e));
    p1n0kIm = fImEBE2D[1][0][0]->GetBinContent(fImEBE2D[1][0][0]->GetBin(p,e))  
            * fImEBE2D[1][0][0]->GetBinEntries(fImEBE2D[1][0][0]->GetBin(p,e));
            
    mp = fReEBE2D[1][0][0]->GetBinEntries(fReEBE2D[1][0][0]->GetBin(p,e)); // to be improved (cross-checked by accessing other profiles here)
    
    typeFlag = 1;
   }
   else if(type == "RP")
   {
    // p_{m*n,0} = q_{m*n,0}:
    p1n0kRe = q1n0kRe; 
    p1n0kIm = q1n0kIm; 
    mp = mq; 
    
    typeFlag = 0;
   }
   
   // count events with non-empty (pt,eta) bin:
   if(mp>0)
   {
    fNonEmptyBins2D[typeFlag]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,1);
   }
   
   // 2'-particle correlation for particular (pt,eta) bin:
   Double_t two1n1nPtEta = 0.;
   if(mp*dMult-mq)
   {
    two1n1nPtEta = (p1n0kRe*dReQ1n+p1n0kIm*dImQ1n-mq)
                 / (mp*dMult-mq);
   
    // fill the 2D profile to get the average correlation for each (pt,eta) bin:
    if(type == "POI")
    { 
     //f2pPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nPtEta,mp*dMult-mq);
     
     fCorrelationsPro[1][0][0][0]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nPtEta,mp*dMult-mq);
    }
    else if(type == "RP")
    {
     //f2pPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nPtEta,mp*dMult-mq);   
     fCorrelationsPro[0][0][0][0]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nPtEta,mp*dMult-mq);
    }
   } // end of if(mp*dMult-mq)
  
   // 4'-particle correlation:
   Double_t four1n1n1n1nPtEta = 0.;
   if((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
       + mq*(dMult-1.)*(dMult-2.)*(dMult-3.)) // to be improved (introduce a new variable for this expression)
   {
    four1n1n1n1nPtEta = ((pow(dReQ1n,2.)+pow(dImQ1n,2.))*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)
                      - q2n0kRe*(pow(dReQ1n,2.)-pow(dImQ1n,2.))
                      - 2.*q2n0kIm*dReQ1n*dImQ1n
                      - p1n0kRe*(dReQ1n*dReQ2n+dImQ1n*dImQ2n)
                      + p1n0kIm*(dImQ1n*dReQ2n-dReQ1n*dImQ2n)
                      - 2.*dMult*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)
                      - 2.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*mq                      
                      + 6.*(q1n0kRe*dReQ1n+q1n0kIm*dImQ1n)                                            
                      + 1.*(q2n0kRe*dReQ2n+q2n0kIm*dImQ2n)                      
                      + 2.*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)                       
                      + 2.*mq*dMult                      
                      - 6.*mq)        
                      / ((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                          + mq*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
    
    // fill the 2D profile to get the average correlation for each (pt, eta) bin:
    if(type == "POI")
    {
     //f4pPtEtaPOI->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
     //                  (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     //                   + mq*(dMult-1.)*(dMult-2.)*(dMult-3.));
     
     fCorrelationsPro[1][0][0][1]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                                     (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                                     + mq*(dMult-1.)*(dMult-2.)*(dMult-3.));
    }
    else if(type == "RP")
    {
     //f4pPtEtaRP->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
     //                 (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     //                  + mq*(dMult-1.)*(dMult-2.)*(dMult-3.));   
                       
     fCorrelationsPro[0][0][0][1]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nPtEta,
                                       (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                                       + mq*(dMult-1.)*(dMult-2.)*(dMult-3.));                   
    }
   } // end of if((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     //            +mq*(dMult-1.)*(dMult-2.)*(dMult-3.))
   
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)

 
  
 */
   
    
      
} // end of AliFlowAnalysisWithQCumulants::CalculateCorrelationsForDifferentialFlow2D()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateWeightedCorrelationsForDifferentialFlow2D(TString type)
{
 // calculate all weighted correlations needed for differential flow 
 
  if(type == "RP") // to be improved (removed)
 {
  cout<<endl;
 }
 // ... 
 
 /*
 
 
 
 // real and imaginary parts of weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n1k = (*fReQ)(0,1);
 Double_t dReQ2n2k = (*fReQ)(1,2);
 Double_t dReQ1n3k = (*fReQ)(0,3);
 //Double_t dReQ4n4k = (*fReQ)(3,4);
 Double_t dImQ1n1k = (*fImQ)(0,1);
 Double_t dImQ2n2k = (*fImQ)(1,2);
 Double_t dImQ1n3k = (*fImQ)(0,3);
 //Double_t dImQ4n4k = (*fImQ)(3,4);
 
 // S^M_{p,k} (see .h file for the definition of fSMpk):
 Double_t dSM1p1k = (*fSMpk)(0,1);
 Double_t dSM1p2k = (*fSMpk)(0,2);
 Double_t dSM1p3k = (*fSMpk)(0,3);
 Double_t dSM2p1k = (*fSMpk)(1,1);
 Double_t dSM3p1k = (*fSMpk)(2,1);
 
 // looping over all (pt,eta) bins and calculating weighted correlations needed for differential flow: 
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   // real and imaginary parts of p_{m*n,0} (non-weighted Q-vector evaluated for POIs in particular (pt,eta) bin):  
   Double_t p1n0kRe = 0.;
   Double_t p1n0kIm = 0.;

   // number of POIs in particular (pt,eta) bin):
   Double_t mp = 0.;

   // real and imaginary parts of q_{m*n,k}: 
   // (weighted Q-vector evaluated for particles which are both RPs and POIs in particular (pt,eta) bin)
   Double_t q1n2kRe = 0.;
   Double_t q1n2kIm = 0.;
   Double_t q2n1kRe = 0.;
   Double_t q2n1kIm = 0.;

   // s_{1,1}, s_{1,2} and s_{1,3} // to be improved (add explanation)  
   Double_t s1p1k = 0.; 
   Double_t s1p2k = 0.; 
   Double_t s1p3k = 0.; 
   
   // M0111 from Eq. (118) in QC2c (to be improved (notation))
   Double_t dM0111 = 0.;
 
   if(type == "POI")
   {
    // p_{m*n,0}:
    p1n0kRe = fReEBE2D[1][0][0]->GetBinContent(fReEBE2D[1][0][0]->GetBin(p,e))
            * fReEBE2D[1][0][0]->GetBinEntries(fReEBE2D[1][0][0]->GetBin(p,e));
    p1n0kIm = fImEBE2D[1][0][0]->GetBinContent(fImEBE2D[1][0][0]->GetBin(p,e))
            * fImEBE2D[1][0][0]->GetBinEntries(fImEBE2D[1][0][0]->GetBin(p,e)); 
            
    mp = fReEBE2D[1][0][0]->GetBinEntries(fReEBE2D[1][0][0]->GetBin(p,e));
    
    // q_{m*n,k}: 
    q1n2kRe = fReEBE2D[2][0][2]->GetBinContent(fReEBE2D[2][0][2]->GetBin(p,e))
            * fReEBE2D[2][0][2]->GetBinEntries(fReEBE2D[2][0][2]->GetBin(p,e));
    q1n2kIm = fImEBE2D[2][0][2]->GetBinContent(fImEBE2D[2][0][2]->GetBin(p,e))
            * fImEBE2D[2][0][2]->GetBinEntries(fImEBE2D[2][0][2]->GetBin(p,e));
    q2n1kRe = fReEBE2D[2][1][1]->GetBinContent(fReEBE2D[2][1][1]->GetBin(p,e))
            * fReEBE2D[2][1][1]->GetBinEntries(fReEBE2D[2][1][1]->GetBin(p,e)); 
    q2n1kIm = fImEBE2D[2][1][1]->GetBinContent(fImEBE2D[2][1][1]->GetBin(p,e))
            * fImEBE2D[2][1][1]->GetBinEntries(fImEBE2D[2][1][1]->GetBin(p,e));
       
    // s_{1,1}, s_{1,2} and s_{1,3} // to be improved (add explanation)  
    s1p1k = pow(fs2D[2][1]->GetBinContent(fs2D[2][1]->GetBin(p,e)),1.); 
    s1p2k = pow(fs2D[2][2]->GetBinContent(fs2D[2][2]->GetBin(p,e)),1.); 
    s1p3k = pow(fs2D[2][3]->GetBinContent(fs2D[2][3]->GetBin(p,e)),1.); 
   
    // M0111 from Eq. (118) in QC2c (to be improved (notation)):
    dM0111 = mp*(dSM3p1k-3.*dSM1p1k*dSM1p2k+2.*dSM1p3k)
           - 3.*(s1p1k*(dSM2p1k-dSM1p2k)
           + 2.*(s1p3k-s1p2k*dSM1p1k));
   }
   else if(type == "RP")
   {
    p1n0kRe = fReEBE2D[0][0][0]->GetBinContent(fReEBE2D[0][0][0]->GetBin(p,e))
            * fReEBE2D[0][0][0]->GetBinEntries(fReEBE2D[0][0][0]->GetBin(p,e));
    p1n0kIm = fImEBE2D[0][0][0]->GetBinContent(fImEBE2D[0][0][0]->GetBin(p,e))
            * fImEBE2D[0][0][0]->GetBinEntries(fImEBE2D[0][0][0]->GetBin(p,e));
            
    mp = fReEBE2D[0][0][0]->GetBinEntries(fReEBE2D[0][0][0]->GetBin(p,e));
    
    // q_{m*n,k}: 
    q1n2kRe = fReEBE2D[0][0][2]->GetBinContent(fReEBE2D[0][0][2]->GetBin(p,e))
            * fReEBE2D[0][0][2]->GetBinEntries(fReEBE2D[0][0][2]->GetBin(p,e));
    q1n2kIm = fImEBE2D[0][0][2]->GetBinContent(fImEBE2D[0][0][2]->GetBin(p,e))
            * fImEBE2D[0][0][2]->GetBinEntries(fImEBE2D[0][0][2]->GetBin(p,e));
    q2n1kRe = fReEBE2D[0][1][1]->GetBinContent(fReEBE2D[0][1][1]->GetBin(p,e))
            * fReEBE2D[0][1][1]->GetBinEntries(fReEBE2D[0][1][1]->GetBin(p,e));
    q2n1kIm = fImEBE2D[0][1][1]->GetBinContent(fImEBE2D[0][1][1]->GetBin(p,e))
            * fImEBE2D[0][1][1]->GetBinEntries(fImEBE2D[0][1][1]->GetBin(p,e));
   
    // s_{1,1}, s_{1,2} and s_{1,3} // to be improved (add explanation)  
    s1p1k = pow(fs2D[0][1]->GetBinContent(fs2D[0][1]->GetBin(p,e)),1.); 
    s1p2k = pow(fs2D[0][2]->GetBinContent(fs2D[0][2]->GetBin(p,e)),1.); 
    s1p3k = pow(fs2D[0][3]->GetBinContent(fs2D[0][3]->GetBin(p,e)),1.); 
   
    // M0111 from Eq. (118) in QC2c (to be improved (notation)):
    dM0111 = mp*(dSM3p1k-3.*dSM1p1k*dSM1p2k+2.*dSM1p3k)
           - 3.*(s1p1k*(dSM2p1k-dSM1p2k)
           + 2.*(s1p3k-s1p2k*dSM1p1k));
    //...............................................................................................   
   }
   
   // 2'-particle correlation:
   Double_t two1n1nW0W1PtEta = 0.;
   if(mp*dSM1p1k-s1p1k)
   {
    two1n1nW0W1PtEta = (p1n0kRe*dReQ1n1k+p1n0kIm*dImQ1n1k-s1p1k)
                 / (mp*dSM1p1k-s1p1k);
   
    // fill the 2D profile to get the average correlation for each (pt, eta) bin:
    if(type == "POI")
    {
     //f2pPtEtaPOIW->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nW0W1PtEta,
     //                   mp*dSM1p1k-s1p1k);
     fCorrelationsPro[1][1][0][0]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nW0W1PtEta,mp*dSM1p1k-s1p1k);
    }
    else if(type == "RP")
    {
     //f2pPtEtaRPW->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nW0W1PtEta,
     //                  mp*dSM1p1k-s1p1k); 
     fCorrelationsPro[0][1][0][0]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,two1n1nW0W1PtEta,mp*dSM1p1k-s1p1k);  
    }
   } // end of if(mp*dMult-dmPrimePrimePtEta)
   
   // 4'-particle correlation:
   Double_t four1n1n1n1nW0W1W1W1PtEta = 0.;
   if(dM0111)
   {
    four1n1n1n1nW0W1W1W1PtEta = ((pow(dReQ1n1k,2.)+pow(dImQ1n1k,2.))*(p1n0kRe*dReQ1n1k+p1n0kIm*dImQ1n1k)
                      - q2n1kRe*(pow(dReQ1n1k,2.)-pow(dImQ1n1k,2.))
                      - 2.*q2n1kIm*dReQ1n1k*dImQ1n1k
                      - p1n0kRe*(dReQ1n1k*dReQ2n2k+dImQ1n1k*dImQ2n2k)
                      + p1n0kIm*(dImQ1n1k*dReQ2n2k-dReQ1n1k*dImQ2n2k)
                      - 2.*dSM1p2k*(p1n0kRe*dReQ1n1k+p1n0kIm*dImQ1n1k)
                      - 2.*(pow(dReQ1n1k,2.)+pow(dImQ1n1k,2.))*s1p1k                                            
                      + 6.*(q1n2kRe*dReQ1n1k+q1n2kIm*dImQ1n1k)                                           
                      + 1.*(q2n1kRe*dReQ2n2k+q2n1kIm*dImQ2n2k)                         
                      + 2.*(p1n0kRe*dReQ1n3k+p1n0kIm*dImQ1n3k)                      
                      + 2.*s1p1k*dSM1p2k                                      
                      - 6.*s1p3k)        
                      / dM0111; // to be imropoved (notation of dM0111)
   
    // fill the 2D profile to get the average correlation for each (pt, eta) bin:
    if(type == "POI")
    {
     //f4pPtEtaPOIW->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nW0W1W1W1PtEta,dM0111);
     fCorrelationsPro[1][1][0][1]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nW0W1W1W1PtEta,dM0111);
    }
    else if(type == "RP")
    {
     //f4pPtEtaRPW->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nW0W1W1W1PtEta,dM0111); 
     fCorrelationsPro[0][1][0][1]->Fill(fPtMin+(p-1)*fPtBinWidth,fEtaMin+(e-1)*fEtaBinWidth,four1n1n1n1nW0W1W1W1PtEta,dM0111); 
    }
   } // end of if(dM0111)
  
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 
  
 */  
    
      
} // end of AliFlowAnalysisWithQCumulants::CalculateWeightedCorrelationsForDifferentialFlow2D(TString type)


//================================================================================================================================


/*
void AliFlowAnalysisWithQCumulants::FinalizeCorrelationsForDiffFlow(TString type, Bool_t useParticleWeights, TString eventWeights)
{
 // 1.) Access average for 2D correlations from profiles and store them in 2D final results histograms;
 // 2.) Access spread for 2D correlations from profiles, calculate error and store it in 2D final results histograms;
 // 3.) Make projections along pt and eta axis and store results and errors in 1D final results histograms. 
 
 Int_t typeFlag = -1;
 Int_t pWeightsFlag = -1;
 Int_t eWeightsFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } else 
     {
      cout<<"WARNING: type must be either RP or POI in AFAWQC::FCFDF() !!!!"<<endl;
      exit(0);
     }
     
 if(!useParticleWeights)
 {
  pWeightsFlag = 0;
 } else 
   {
    pWeightsFlag = 1;   
   }   
   
 if(eventWeights == "exact")
 {
  eWeightsFlag = 0;
 }          
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pW = pWeightsFlag;
 Int_t eW = eWeightsFlag;
 
 // from 2D histogram fNonEmptyBins2D make two 1D histograms fNonEmptyBins1D in pt and eta (to be improved (i.e. moved somewhere else))  
 // pt:
 for(Int_t p=1;p<fnBinsPt;p++)
 {
  Double_t contentPt = 0.;
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   contentPt += (fNonEmptyBins2D[t]->GetBinContent(fNonEmptyBins2D[t]->GetBin(p,e)));          
  }
  fNonEmptyBins1D[t][0]->SetBinContent(p,contentPt);
 }
 // eta:
 for(Int_t e=1;e<fnBinsEta;e++)
 {
  Double_t contentEta = 0.;
  for(Int_t p=1;p<=fnBinsPt;p++)
  {
   contentEta += (fNonEmptyBins2D[t]->GetBinContent(fNonEmptyBins2D[t]->GetBin(p,e)));          
  }
  fNonEmptyBins1D[t][1]->SetBinContent(e,contentEta);
 }
 
 // from 2D profile in (pt,eta) make two 1D profiles in (pt) and (eta):
 TProfile *profile[2][4]; // [0=pt,1=eta][correlation index] // to be improved (do not hardwire the correlation index)
 
 for(Int_t pe=0;pe<2;pe++) // pt or eta
 {
  for(Int_t ci=0;ci<4;ci++) // correlation index
  {
   if(pe==0) profile[pe][ci] = this->MakePtProjection(fCorrelationsPro[t][pW][eW][ci]);
   if(pe==1) profile[pe][ci] = this->MakeEtaProjection(fCorrelationsPro[t][pW][eW][ci]);
  }
 }
  
 // transfer 2D profile into 2D histogram:
 // to be improved (see in documentation if there is a method to transfer values from 2D profile into 2D histogram)    
 for(Int_t ci=0;ci<4;ci++)
 {
  for(Int_t p=1;p<=fnBinsPt;p++)
  {
   for(Int_t e=1;e<=fnBinsEta;e++)
   {
    Double_t correlation = fCorrelationsPro[t][pW][eW][ci]->GetBinContent(fCorrelationsPro[t][pW][eW][ci]->GetBin(p,e)); 
    Double_t spread = fCorrelationsPro[t][pW][eW][ci]->GetBinError(fCorrelationsPro[t][pW][eW][ci]->GetBin(p,e));
    Double_t nEvts = fNonEmptyBins2D[t]->GetBinContent(fNonEmptyBins2D[t]->GetBin(p,e));
    Double_t error = 0.;
    fFinalCorrelations2D[t][pW][eW][ci]->SetBinContent(fFinalCorrelations2D[t][pW][eW][ci]->GetBin(p,e),correlation);          
    if(nEvts>0)
    {
     error = spread/pow(nEvts,0.5);
     fFinalCorrelations2D[t][pW][eW][ci]->SetBinError(fFinalCorrelations2D[t][pW][eW][ci]->GetBin(p,e),error);
    }
   } // end of for(Int_t e=1;e<=fnBinsEta;e++)
  } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 } // end of for(Int_t ci=0;ci<4;ci++)
 
 // transfer 1D profile into 1D histogram (pt):
 // to be improved (see in documentation if there is a method to transfer values from 1D profile into 1D histogram)    
 for(Int_t ci=0;ci<4;ci++)
 {
  for(Int_t p=1;p<=fnBinsPt;p++)
  {
   if(profile[0][ci])
   {
    Double_t correlation = profile[0][ci]->GetBinContent(p); 
    Double_t spread = profile[0][ci]->GetBinError(p);
    Double_t nEvts = fNonEmptyBins1D[t][0]->GetBinContent(p);
    Double_t error = 0.;
    fFinalCorrelations1D[t][pW][eW][0][ci]->SetBinContent(p,correlation); 
    if(nEvts>0)
    {
     error = spread/pow(nEvts,0.5);
     fFinalCorrelations1D[t][pW][eW][0][ci]->SetBinError(p,error);
    }  
   }   
  } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 } // end of for(Int_t ci=0;ci<4;ci++)
 
 // transfer 1D profile into 1D histogram (eta):
 // to be improved (see in documentation if there is a method to transfer values from 1D profile into 1D histogram)    
 for(Int_t ci=0;ci<4;ci++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++)
  {
   if(profile[1][ci])
   {
    Double_t correlation = profile[1][ci]->GetBinContent(e); 
    fFinalCorrelations1D[t][pW][eW][1][ci]->SetBinContent(e,correlation);      
   }    
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t ci=0;ci<4;ci++)
        
} // end of void AliFlowAnalysisWithQCumulants::FinalizeCorrelationsForDiffFlow(TString type, Bool_t useParticleWeights, TString eventWeights)
*/


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateDiffFlowCumulants(TString type, TString ptOrEta)
{
 // calcualate cumulants for differential flow from measured correlations
 // Remark: cumulants calculated here are NOT corrected for non-uniform acceptance. This correction is applied in the method ...
 // to be improved (description) 
 
 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
     
 // common:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 
 // correlation <<2>>: 
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1);
 
 // 1D:
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  // reduced correlations:   
  Double_t twoPrime = fDiffFlowCorrelationsHist[t][pe][0]->GetBinContent(b); // <<2'>>(pt)
  Double_t fourPrime = fDiffFlowCorrelationsHist[t][pe][1]->GetBinContent(b); // <<4'>>(pt)
  // final statistical error of reduced correlations:
  //Double_t twoPrimeError = fFinalCorrelations1D[t][pW][eW][0][0]->GetBinError(p); 
  // QC{2'}:
  Double_t qc2Prime = twoPrime; // QC{2'}
  //Double_t qc2PrimeError = twoPrimeError; // final stat. error of QC{2'}
  fDiffFlowCumulants[t][pe][0]->SetBinContent(b,qc2Prime); 
  //fFinalCumulantsPt[t][pW][eW][nua][0]->SetBinError(p,qc2PrimeError);   
  // QC{4'}:
  Double_t qc4Prime = fourPrime - 2.*twoPrime*two; // QC{4'} = <<4'>> - 2*<<2'>><<2>>
  fDiffFlowCumulants[t][pe][1]->SetBinContent(b,qc4Prime); 
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 
 
 /*
 // 1D (eta):
 // to be improved (see documentation if I can do all this without looping)
 // to be improved (treat pt and eta in one go)
 // to be improved (combine loops over nua for 2D and 1D)
 for(Int_t e=1;e<=fnBinsEta;e++)
 {
  // reduced correlations:   
  Double_t twoPrime = fFinalCorrelations1D[t][pW][eW][1][0]->GetBinContent(e); // <<2'>>(eta)
  Double_t fourPrime = fFinalCorrelations1D[t][pW][eW][1][1]->GetBinContent(e); // <<4'>>(eta)
  // final statistical error of reduced correlations:
  Double_t twoPrimeError = fFinalCorrelations1D[t][pW][eW][1][0]->GetBinError(e); 
  for(Int_t nua=0;nua<2;nua++)
  {
   // QC{2'}:
   Double_t qc2Prime = twoPrime; // QC{2'}
   Double_t qc2PrimeError = twoPrimeError; // final stat. error of QC{2'}
   fFinalCumulantsEta[t][pW][eW][nua][0]->SetBinContent(e,qc2Prime);    
   fFinalCumulantsEta[t][pW][eW][nua][0]->SetBinError(e,qc2PrimeError);   
   // QC{4'}:
   Double_t qc4Prime = fourPrime - 2.*twoPrime*two; // QC{4'} = <<4'>> - 2*<<2'>><<2>>
   fFinalCumulantsEta[t][pW][eW][nua][1]->SetBinContent(e,qc4Prime);
  } // end of for(Int_t nua=0;nua<2;nua++)   
 } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 */ 

 
 
 
 
 
 
 
 
 
 
 
    
 /* 
 // 2D (pt,eta):
 // to be improved (see documentation if I can do all this without looping)
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  for(Int_t e=1;e<=fnBinsEta;e++) 
  {  
   // reduced correlations:   
   Double_t twoPrime = fFinalCorrelations2D[t][pW][eW][0]->GetBinContent(fFinalCorrelations2D[t][pW][eW][0]->GetBin(p,e)); // <<2'>>(pt,eta)
   Double_t fourPrime = fFinalCorrelations2D[t][pW][eW][1]->GetBinContent(fFinalCorrelations2D[t][pW][eW][1]->GetBin(p,e)); // <<4'>>(pt,eta)
   for(Int_t nua=0;nua<2;nua++)
   {
    // QC{2'}:
    Double_t qc2Prime = twoPrime; // QC{2'} = <<2'>>
    fFinalCumulants2D[t][pW][eW][nua][0]->SetBinContent(fFinalCumulants2D[t][pW][eW][nua][0]->GetBin(p,e),qc2Prime);    
    // QC{4'}:
    Double_t qc4Prime = fourPrime - 2.*twoPrime*two; // QC{4'} = <<4'>> - 2*<<2'>><<2>>
    fFinalCumulants2D[t][pW][eW][nua][1]->SetBinContent(fFinalCumulants2D[t][pW][eW][nua][1]->GetBin(p,e),qc4Prime);   
   } // end of for(Int_t nua=0;nua<2;nua++)   
  } // end of for(Int_t e=1;e<=fnBinsEta;e++)
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 */
 
 
 /*
 
 
 
 */
 
  
} // end of void AliFlowAnalysisWithQCumulants::CalculateDiffFlowCumulants(TString type, Bool_t useParticleWeights, TString eventWeights); 


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateFinalResultsForRPandPOIIntegratedFlow(TString type)
{
 // calculate final results for integrated flow of RPs and POIs 
  
 Int_t typeFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } else 
     {
      cout<<"WARNING: type must be either RP or POI in AFAWQC::CDF() !!!!"<<endl;
      exit(0);
     }
     
 // shortcuts:
 Int_t t = typeFlag;
  
 // pt yield:    
 TH1F *yield2ndPt = NULL;
 TH1F *yield4thPt = NULL;
 TH1F *yield6thPt = NULL;
 TH1F *yield8thPt = NULL;
 
 if(type == "POI")
 {
  yield2ndPt = (TH1F*)(fCommonHists2nd->GetHistPtPOI())->Clone();
  yield4thPt = (TH1F*)(fCommonHists4th->GetHistPtPOI())->Clone();
  yield6thPt = (TH1F*)(fCommonHists6th->GetHistPtPOI())->Clone();
  yield8thPt = (TH1F*)(fCommonHists8th->GetHistPtPOI())->Clone();  
 } 
 else if(type == "RP")
 {
  yield2ndPt = (TH1F*)(fCommonHists2nd->GetHistPtRP())->Clone();
  yield4thPt = (TH1F*)(fCommonHists4th->GetHistPtRP())->Clone();
  yield6thPt = (TH1F*)(fCommonHists6th->GetHistPtRP())->Clone();
  yield8thPt = (TH1F*)(fCommonHists8th->GetHistPtRP())->Clone();  
 } 
 
 Int_t nBinsPt = yield2ndPt->GetNbinsX();
 
 TH1D *flow2ndPt = NULL;
 TH1D *flow4thPt = NULL;
 TH1D *flow6thPt = NULL;
 TH1D *flow8thPt = NULL;
 
 // to be improved (hardwired pt index)
 flow2ndPt = (TH1D*)fDiffFlow[t][0][0]->Clone();
 flow4thPt = (TH1D*)fDiffFlow[t][0][1]->Clone();
 flow6thPt = (TH1D*)fDiffFlow[t][0][2]->Clone();
 flow8thPt = (TH1D*)fDiffFlow[t][0][3]->Clone(); 
   
 Double_t dvn2nd = 0., dvn4th = 0., dvn6th = 0., dvn8th = 0.; // differential flow
 Double_t dErrvn2nd = 0., dErrvn4th = 0., dErrvn6th = 0., dErrvn8th = 0.; // error on differential flow
 
 Double_t dVn2nd = 0., dVn4th = 0., dVn6th = 0., dVn8th = 0.; // integrated flow 
 Double_t dErrVn2nd = 0., dErrVn4th = 0., dErrVn6th = 0., dErrVn8th = 0.; // error on integrated flow

 Double_t dYield2nd = 0., dYield4th = 0., dYield6th = 0., dYield8th = 0.; // pt yield 
 Double_t dSum2nd = 0., dSum4th = 0., dSum6th = 0., dSum8th = 0.; // needed for normalizing integrated flow
 
 // looping over pt bins:
 for(Int_t p=1;p<nBinsPt+1;p++)
 {
  dvn2nd = flow2ndPt->GetBinContent(p);
  dvn4th = flow4thPt->GetBinContent(p);
  dvn6th = flow6thPt->GetBinContent(p);
  dvn8th = flow8thPt->GetBinContent(p);
  
  dErrvn2nd = flow2ndPt->GetBinError(p);
  dErrvn4th = flow4thPt->GetBinError(p);
  dErrvn6th = flow6thPt->GetBinError(p);
  dErrvn8th = flow8thPt->GetBinError(p);

  dYield2nd = yield2ndPt->GetBinContent(p);  
  dYield4th = yield4thPt->GetBinContent(p);
  dYield6th = yield6thPt->GetBinContent(p);
  dYield8th = yield8thPt->GetBinContent(p);
  
  dVn2nd += dvn2nd*dYield2nd;
  dVn4th += dvn4th*dYield4th;
  dVn6th += dvn6th*dYield6th;
  dVn8th += dvn8th*dYield8th;
  
  dSum2nd += dYield2nd;
  dSum4th += dYield4th;
  dSum6th += dYield6th;
  dSum8th += dYield8th;
  
  dErrVn2nd += dYield2nd*dYield2nd*dErrvn2nd*dErrvn2nd; // ro be improved (check this relation)
  dErrVn4th += dYield4th*dYield4th*dErrvn4th*dErrvn4th;
  dErrVn6th += dYield6th*dYield6th*dErrvn6th*dErrvn6th;
  dErrVn8th += dYield8th*dYield8th*dErrvn8th*dErrvn8th;
    
 } // end of for(Int_t p=1;p<nBinsPt+1;p++)

 // normalizing the results for integrated flow:
 if(dSum2nd) 
 {
  dVn2nd /= dSum2nd;
  dErrVn2nd /= (dSum2nd*dSum2nd);
  dErrVn2nd = TMath::Sqrt(dErrVn2nd);
 } 
 if(dSum4th) 
 {
  dVn4th /= dSum4th;
  dErrVn4th /= (dSum4th*dSum4th);
  dErrVn4th = TMath::Sqrt(dErrVn4th);
 } 
 //if(dSum6th) dVn6th/=dSum6th;
 //if(dSum8th) dVn8th/=dSum8th;
 
  
 /* 
 // storing the results for integrated flow: // to be improved (make a new method for this!)
 if(!(useParticleWeights))
 {
  if(type == "POI")
  {
   // 2nd:
   fIntFlowResultsPOIQC->SetBinContent(1,dVn2nd);
   fIntFlowResultsPOIQC->SetBinError(1,dSd2nd);
   // 4th:
   fIntFlowResultsPOIQC->SetBinContent(2,dVn4th);
   fIntFlowResultsPOIQC->SetBinError(2,dSd4th);
   // 6th:
   fIntFlowResultsPOIQC->SetBinContent(3,dVn6th);
   fIntFlowResultsPOIQC->SetBinError(3,dSd6th);
   // 8th:
   fIntFlowResultsPOIQC->SetBinContent(4,dVn8th);
   fIntFlowResultsPOIQC->SetBinError(4,dSd8th);
  }
  else if (type == "RP")
  {
   // 2nd:
   fIntFlowResultsRPQC->SetBinContent(1,dVn2nd);
   fIntFlowResultsRPQC->SetBinError(1,dSd2nd);
   // 4th:
   fIntFlowResultsRPQC->SetBinContent(2,dVn4th);
   fIntFlowResultsRPQC->SetBinError(2,dSd4th);
   // 6th:
   fIntFlowResultsRPQC->SetBinContent(3,dVn6th);
   fIntFlowResultsRPQC->SetBinError(3,dSd6th);
   // 8th:
   fIntFlowResultsRPQC->SetBinContent(4,dVn8th);
   fIntFlowResultsRPQC->SetBinError(4,dSd8th);
  }
 } 
 else if (useParticleWeights)
 {
  if(type == "POI")
  {
   // 2nd:
   fIntFlowResultsPOIQCW->SetBinContent(1,dVn2nd);
   fIntFlowResultsPOIQCW->SetBinError(1,dSd2nd);
   // 4th:
   fIntFlowResultsPOIQCW->SetBinContent(2,dVn4th);
   fIntFlowResultsPOIQCW->SetBinError(2,dSd4th);
   // 6th:
   fIntFlowResultsPOIQCW->SetBinContent(3,dVn6th);
   fIntFlowResultsPOIQCW->SetBinError(3,dSd6th);
   // 8th:
   fIntFlowResultsPOIQCW->SetBinContent(4,dVn8th);
   fIntFlowResultsPOIQCW->SetBinError(4,dSd8th);
  }
  else if (type == "RP")
  {
   // 2nd:
   fIntFlowResultsRPQCW->SetBinContent(1,dVn2nd);
   fIntFlowResultsRPQCW->SetBinError(1,dSd2nd);
   // 4th:
   fIntFlowResultsRPQCW->SetBinContent(2,dVn4th);
   fIntFlowResultsRPQCW->SetBinError(2,dSd4th);
   // 6th:
   fIntFlowResultsRPQCW->SetBinContent(3,dVn6th);
   fIntFlowResultsRPQCW->SetBinError(3,dSd6th);
   // 8th:
   fIntFlowResultsRPQCW->SetBinContent(4,dVn8th);
   fIntFlowResultsRPQCW->SetBinError(4,dSd8th);
  }
 }
 */
 
 
 
 // storing the results for integrated flow in common histos: (to be improved: new method for this?)
 if(type == "POI")
 {
  fCommonHistsResults2nd->FillIntegratedFlowPOI(dVn2nd,dErrVn2nd); 
  fCommonHistsResults4th->FillIntegratedFlowPOI(dVn4th,dErrVn4th); 
  fCommonHistsResults6th->FillIntegratedFlowPOI(dVn6th,0.); // to be improved (errors)
  fCommonHistsResults8th->FillIntegratedFlowPOI(dVn8th,0.); // to be improved (errors)
 }
 else if (type == "RP")
 {
  fCommonHistsResults2nd->FillIntegratedFlowRP(dVn2nd,dErrVn2nd); 
  fCommonHistsResults4th->FillIntegratedFlowRP(dVn4th,dErrVn4th);
  fCommonHistsResults6th->FillIntegratedFlowRP(dVn6th,0.); // to be improved (errors)
  fCommonHistsResults8th->FillIntegratedFlowRP(dVn8th,0.); // to be improved (errors)
 }
 
 delete flow2ndPt;
 delete flow4thPt;
 //delete flow6thPt;
 //delete flow8thPt;
 
 delete yield2ndPt;
 delete yield4thPt;
 delete yield6thPt;
 delete yield8thPt;
      
     
} // end of AliFlowAnalysisWithQCumulants::CalculateFinalResultsForRPandPOIIntegratedFlow(TString type)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::InitializeArraysForDistributions()
{
 // initialize arrays used for distributions:
 
 /*
 
 for(Int_t pW=0;pW<2;pW++) // particle weights not used (0) or used (1)
 {
  for(Int_t eW=0;eW<2;eW++)
  {
   for(Int_t di=0;di<4;di++) // distribution index
   {
    fDistributions[pW][eW][di] = NULL;
   }
  } 
 }
 
 */
 
} // end of void AliFlowAnalysisWithQCumulants::InitializeArraysForDistributions()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookEverythingForDistributions()
{
 // book all histograms for distributions
 
 /*
 //weighted <2>_{n|n} distribution
 f2pDistribution = new TH1D("f2pDistribution","<2>_{n|n} distribution",100000,-0.02,0.1);
 f2pDistribution->SetXTitle("<2>_{n|n}");
 f2pDistribution->SetYTitle("Counts");
 fHistList->Add(f2pDistribution);

 //weighted <4>_{n,n|n,n} distribution
 f4pDistribution = new TH1D("f4pDistribution","<4>_{n,n|n,n} distribution",100000,-0.00025,0.002);
 f4pDistribution->SetXTitle("<4>_{n,n|n,n}");
 f4pDistribution->SetYTitle("Counts");
 fHistList->Add(f4pDistribution); 
 
 //weighted <6>_{n,n,n|n,n,n} distribution
 f6pDistribution = new TH1D("f6pDistribution","<6>_{n,n,n|n,n,n} distribution",100000,-0.000005,0.000025);
 f6pDistribution->SetXTitle("<6>_{n,n,n|n,n,n}");
 f6pDistribution->SetYTitle("Counts");
 fHistList->Add(f6pDistribution);
 
 //weighted <8>_{n,n,n,n|n,n,n,n} distribution
 f8pDistribution = new TH1D("f8pDistribution","<8>_{n,n,n,n|n,n,n,n} distribution",100000,-0.000000001,0.00000001);
 f8pDistribution->SetXTitle("<8>_{n,n,n,n|n,n,n,n}");
 f8pDistribution->SetYTitle("Counts");
 fHistList->Add(f8pDistribution);
 */
 
} // end of void AliFlowAnalysisWithQCumulants::BookEverythingForDistributions()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookAndNestAllLists()
{
 // Book and nest all lists nested in the base list fHistList.
 //  a) Book and nest lists for integrated flow;
 //  b) Book and nest lists for differential flow;
 //  c) Book and nest list for particle weights;
 //  d) Book and nest list for distributions;
 //  e) Book and nest list for nested loops;
 
 // a) Book and nest all lists for integrated flow:
 // base list for integrated flow:
 fIntFlowList = new TList();
 fIntFlowList->SetName("Integrated Flow");
 fIntFlowList->SetOwner(kTRUE);
 fHistList->Add(fIntFlowList);
 // list holding profiles: 
 fIntFlowProfiles = new TList();
 fIntFlowProfiles->SetName("Profiles");
 fIntFlowProfiles->SetOwner(kTRUE);
 fIntFlowList->Add(fIntFlowProfiles);
 // list holding histograms with results:
 fIntFlowResults = new TList();
 fIntFlowResults->SetName("Results");
 fIntFlowResults->SetOwner(kTRUE);
 fIntFlowList->Add(fIntFlowResults);
 
 // b) Book and nest lists for differential flow;
 fDiffFlowList = new TList();
 fDiffFlowList->SetName("Differential Flow");
 fDiffFlowList->SetOwner(kTRUE); 
 fHistList->Add(fDiffFlowList);
 // list holding profiles: 
 fDiffFlowProfiles = new TList(); 
 fDiffFlowProfiles->SetName("Profiles");
 fDiffFlowProfiles->SetOwner(kTRUE);
 fDiffFlowList->Add(fDiffFlowProfiles);
 // list holding histograms with results: 
 fDiffFlowResults = new TList();
 fDiffFlowResults->SetName("Results");
 fDiffFlowResults->SetOwner(kTRUE);
 fDiffFlowList->Add(fDiffFlowResults);
 // flags used for naming nested lists in list fDiffFlowProfiles and fDiffFlowResults:  
 TList list;
 list.SetOwner(kTRUE);
 TString typeFlag[2] = {"RP","POI"};  
 TString ptEtaFlag[2] = {"p_{T}","#eta"}; 
 TString powerFlag[2] = {"linear","quadratic"};   
 // nested lists in fDiffFlowProfiles (~/Differential Flow/Profiles):
 for(Int_t t=0;t<2;t++) // type: RP or POI
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   // list holding profiles with correlations:
   fDiffFlowCorrelationsProList[t][pe] = (TList*)list.Clone();
   fDiffFlowCorrelationsProList[t][pe]->SetName(Form("Profiles with correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowProfiles->Add(fDiffFlowCorrelationsProList[t][pe]);
   // list holding profiles with products of correlations:
   fDiffFlowProductOfCorrelationsProList[t][pe] = (TList*)list.Clone();
   fDiffFlowProductOfCorrelationsProList[t][pe]->SetName(Form("Profiles with products of correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowProfiles->Add(fDiffFlowProductOfCorrelationsProList[t][pe]);
   // list holding profiles with correlations:
   fDiffFlowCorrectionsProList[t][pe] = (TList*)list.Clone();
   fDiffFlowCorrectionsProList[t][pe]->SetName(Form("Profiles with correction terms for NUA (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowProfiles->Add(fDiffFlowCorrectionsProList[t][pe]);   
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta 
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI   
 // nested lists in fDiffFlowResults (~/Differential Flow/Results):
 for(Int_t t=0;t<2;t++) // type: RP or POI
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   // list holding histograms with correlations:
   fDiffFlowCorrelationsHistList[t][pe] = (TList*)list.Clone();
   fDiffFlowCorrelationsHistList[t][pe]->SetName(Form("Correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowResults->Add(fDiffFlowCorrelationsHistList[t][pe]);
   for(Int_t power=0;power<2;power++)
   {
    // list holding histograms with sums of event weights:
    fDiffFlowSumOfEventWeightsHistList[t][pe][power] = (TList*)list.Clone();
    fDiffFlowSumOfEventWeightsHistList[t][pe][power]->SetName(Form("Sum of %s event weights (%s, %s)",powerFlag[power].Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data()));
    fDiffFlowResults->Add(fDiffFlowSumOfEventWeightsHistList[t][pe][power]);    
   } // end of for(Int_t power=0;power<2;power++)
   // list holding histograms with sums of products of event weights:
   fDiffFlowSumOfProductOfEventWeightsHistList[t][pe] = (TList*)list.Clone();
   fDiffFlowSumOfProductOfEventWeightsHistList[t][pe]->SetName(Form("Sum of products of event weights (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowResults->Add(fDiffFlowSumOfProductOfEventWeightsHistList[t][pe]);
   // list holding histograms with covariances of correlations:
   fDiffFlowCovariancesHistList[t][pe] = (TList*)list.Clone();
   fDiffFlowCovariancesHistList[t][pe]->SetName(Form("Covariances of correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowResults->Add(fDiffFlowCovariancesHistList[t][pe]);
   // list holding histograms with differential Q-cumulants:
   fDiffFlowCumulantsHistList[t][pe] = (TList*)list.Clone();
   fDiffFlowCumulantsHistList[t][pe]->SetName(Form("Differential Q-cumulants (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowResults->Add(fDiffFlowCumulantsHistList[t][pe]);   
   // list holding histograms with differential flow estimates from Q-cumulants:
   fDiffFlowHistList[t][pe] = (TList*)list.Clone();
   fDiffFlowHistList[t][pe]->SetName(Form("Differential flow (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()));
   fDiffFlowResults->Add(fDiffFlowHistList[t][pe]);      
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI
  
 // c) Book and nest list for particle weights:
 fWeightsList->SetName("Weights");
 fWeightsList->SetOwner(kTRUE);   
 fHistList->Add(fWeightsList); 

 // d) Book and nest list for distributions:
 fDistributionsList = new TList();
 fDistributionsList->SetName("Distributions");
 fDistributionsList->SetOwner(kTRUE);
 fHistList->Add(fDistributionsList);
 
 // e) Book and nest list for nested loops:
 fNestedLoopsList = new TList();
 fNestedLoopsList->SetName("Nested Loops");
 fNestedLoopsList->SetOwner(kTRUE);
 fHistList->Add(fNestedLoopsList);
 
} // end of void AliFlowAnalysisWithQCumulants::BookAndNestAllLists()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::FillCommonHistResultsDiffFlow(TString type)
{
 // fill common result histograms for differential flow
 
 Int_t typeFlag = -1;
 //Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 //Int_t pe = ptEtaFlag;

 /* // to be improved (implement protection here)
 for(Int_t o=0;o<4;o++) // order
 {
  if(!fFinalFlowPt[t][pW][eW][nua][o])
  {
    cout<<"WARNING: fFinalFlowPt[t][pW][eW][nua][o] is NULL in AFAWQC::FCHRIF() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pW = "<<pW<<endl;
    cout<<"eW = "<<eW<<endl;
    cout<<"nua = "<<nua<<endl;
    cout<<"o = "<<o<<endl;
    exit(0); 
  }
  if(!fFinalFlowEta[t][pW][eW][nua][o])
  {
    cout<<"WARNING: fFinalFlowEta[t][pW][eW][nua][o] is NULL in AFAWQC::FCHRIF() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pW = "<<pW<<endl;
    cout<<"eW = "<<eW<<endl;
    cout<<"nua = "<<nua<<endl;
    cout<<"o = "<<o<<endl;
    exit(0); 
  }  
 } 
 */
     
 if(!(fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th))
 {
  cout<<"WARNING: fCommonHistsResults2nd && fCommonHistsResults4th && fCommonHistsResults6th && fCommonHistsResults8th"<<endl; 
  cout<<"         is NULL in AFAWQC::FCHRIF() !!!!"<<endl;
  exit(0);
 }
 
 // pt:
 for(Int_t p=1;p<=fnBinsPt;p++)
 {
  Double_t v2 = fDiffFlow[t][0][0]->GetBinContent(p);
  Double_t v4 = fDiffFlow[t][0][1]->GetBinContent(p);
  Double_t v6 = fDiffFlow[t][0][2]->GetBinContent(p);
  Double_t v8 = fDiffFlow[t][0][3]->GetBinContent(p);
  
  Double_t v2Error = fDiffFlow[t][0][0]->GetBinError(p);
  Double_t v4Error = fDiffFlow[t][0][1]->GetBinError(p);
  //Double_t v6Error = fFinalFlow1D[t][pW][nua][0][2]->GetBinError(p);
  //Double_t v8Error = fFinalFlow1D[t][pW][nua][0][3]->GetBinError(p);
 
  if(type == "RP")
  {
   fCommonHistsResults2nd->FillDifferentialFlowPtRP(p,v2,v2Error);
   fCommonHistsResults4th->FillDifferentialFlowPtRP(p,v4,v4Error);
   fCommonHistsResults6th->FillDifferentialFlowPtRP(p,v6,0.);
   fCommonHistsResults8th->FillDifferentialFlowPtRP(p,v8,0.);
  } else if(type == "POI")
    {
     fCommonHistsResults2nd->FillDifferentialFlowPtPOI(p,v2,v2Error);
     fCommonHistsResults4th->FillDifferentialFlowPtPOI(p,v4,v4Error);
     fCommonHistsResults6th->FillDifferentialFlowPtPOI(p,v6,0.);
     fCommonHistsResults8th->FillDifferentialFlowPtPOI(p,v8,0.);
    }
 } // end of for(Int_t p=1;p<=fnBinsPt;p++)   
 
 // eta:
 for(Int_t e=1;e<=fnBinsEta;e++)
 {
  Double_t v2 = fDiffFlow[t][1][0]->GetBinContent(e);
  Double_t v4 = fDiffFlow[t][1][1]->GetBinContent(e);
  Double_t v6 = fDiffFlow[t][1][2]->GetBinContent(e);
  Double_t v8 = fDiffFlow[t][1][3]->GetBinContent(e);
  
  Double_t v2Error = fDiffFlow[t][1][0]->GetBinError(e);
  Double_t v4Error = fDiffFlow[t][1][1]->GetBinError(e);
  //Double_t v6Error = fDiffFlow[t][1][2]->GetBinError(e);
  //Double_t v8Error = fDiffFlow[t][1][3]->GetBinError(e);
 
  if(type == "RP")
  {
   fCommonHistsResults2nd->FillDifferentialFlowEtaRP(e,v2,v2Error);
   fCommonHistsResults4th->FillDifferentialFlowEtaRP(e,v4,v4Error);
   fCommonHistsResults6th->FillDifferentialFlowEtaRP(e,v6,0.);
   fCommonHistsResults8th->FillDifferentialFlowEtaRP(e,v8,0.);
  } else if(type == "POI")
    {
     fCommonHistsResults2nd->FillDifferentialFlowEtaPOI(e,v2,v2Error);
     fCommonHistsResults4th->FillDifferentialFlowEtaPOI(e,v4,v4Error);
     fCommonHistsResults6th->FillDifferentialFlowEtaPOI(e,v6,0.);
     fCommonHistsResults8th->FillDifferentialFlowEtaPOI(e,v8,0.);
    }
 } // end of for(Int_t e=1;e<=fnBinsEta;e++)    
 
} // end of void AliFlowAnalysisWithQCumulants::FillCommonHistResultsDiffFlow(TString type, Bool_t useParticleWeights, TString eventWeights, Bool_t correctedForNUA)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::AccessConstants()
{
 // access needed common constants from AliFlowCommonConstants
 
 fnBinsPhi = AliFlowCommonConstants::GetNbinsPhi();
 fPhiMin = AliFlowCommonConstants::GetPhiMin();	     
 fPhiMax = AliFlowCommonConstants::GetPhiMax();
 if(fnBinsPhi) fPhiBinWidth = (fPhiMax-fPhiMin)/fnBinsPhi;  
 fnBinsPt = AliFlowCommonConstants::GetNbinsPt();
 fPtMin = AliFlowCommonConstants::GetPtMin();	     
 fPtMax = AliFlowCommonConstants::GetPtMax();
 if(fnBinsPt) fPtBinWidth = (fPtMax-fPtMin)/fnBinsPt;  
 fnBinsEta = AliFlowCommonConstants::GetNbinsEta();
 fEtaMin = AliFlowCommonConstants::GetEtaMin();	     
 fEtaMax = AliFlowCommonConstants::GetEtaMax();
 if(fnBinsEta) fEtaBinWidth = (fEtaMax-fEtaMin)/fnBinsEta;  
 
} // end of void AliFlowAnalysisWithQCumulants::AccessConstants()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowSumOfEventWeights()
{
 // Calculate sum of linear and quadratic event weights for correlations
 
 Double_t dMult = (*fSMpk)(0,0); // multiplicity 

 Double_t eventWeight[4] = {0}; 
 eventWeight[0] = dMult*(dMult-1); // event weight for <2> 
 eventWeight[1] = dMult*(dMult-1)*(dMult-2)*(dMult-3); // event weight for <4> 
 eventWeight[2] = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5); // event weight for <6> 
 eventWeight[3] = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7); // event weight for <8> 
 
 for(Int_t p=0;p<2;p++) // power-1
 {
  for(Int_t ci=0;ci<4;ci++) // correlation index
  { 
   fIntFlowSumOfEventWeights[p]->Fill(ci+0.5,pow(eventWeight[ci],p+1)); 
  }
 }
  
} // end of void AliFlowAnalysisWithQCumulants::CalculateIntFlowSumOfEventWeights()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowSumOfProductOfEventWeights()
{
 // Calculate sum of product of event weights for correlations

 Double_t dMult = (*fSMpk)(0,0); // multiplicity 

 Double_t eventWeight[4] = {0}; 
 eventWeight[0] = dMult*(dMult-1); // event weight for <2> 
 eventWeight[1] = dMult*(dMult-1)*(dMult-2)*(dMult-3); // event weight for <4> 
 eventWeight[2] = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5); // event weight for <6> 
 eventWeight[3] = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7); // event weight for <8> 

 fIntFlowSumOfProductOfEventWeights->Fill(0.5,eventWeight[0]*eventWeight[1]); 
 fIntFlowSumOfProductOfEventWeights->Fill(1.5,eventWeight[0]*eventWeight[2]); 
 fIntFlowSumOfProductOfEventWeights->Fill(2.5,eventWeight[0]*eventWeight[3]); 
 fIntFlowSumOfProductOfEventWeights->Fill(3.5,eventWeight[1]*eventWeight[2]); 
 fIntFlowSumOfProductOfEventWeights->Fill(4.5,eventWeight[1]*eventWeight[3]); 
 fIntFlowSumOfProductOfEventWeights->Fill(5.5,eventWeight[2]*eventWeight[3]); 

} // end of void AliFlowAnalysisWithQCumulants::CalculateIntFlowIntFlowSumOfProductOfEventWeights()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateReducedCorrelations1D(TString type, TString ptOrEta)
{
 // calculate reduced correlations for RPs or POIs in pt or eta bins

 // multiplicity:
 Double_t dMult = (*fSMpk)(0,0);
 
 // real and imaginary parts of non-weighted Q-vectors evaluated in harmonics n, 2n, 3n and 4n: 
 Double_t dReQ1n = (*fReQ)(0,0);
 Double_t dReQ2n = (*fReQ)(1,0);
 //Double_t dReQ3n = (*fReQ)(2,0);
 //Double_t dReQ4n = (*fReQ)(3,0);
 Double_t dImQ1n = (*fImQ)(0,0);
 Double_t dImQ2n = (*fImQ)(1,0);
 //Double_t dImQ3n = (*fImQ)(2,0);
 //Double_t dImQ4n = (*fImQ)(3,0);

 // reduced correlations are stored in fDiffFlowCorrelationsPro[0=RP,1=POI][0=pt,1=eta][correlation index]. Correlation index runs as follows:
 // 
 // 0: <<2'>>
 // 1: <<4'>>
 // 2: <<6'>>
 // 3: <<8'>>
 
 Int_t t = -1; // type flag 
 Int_t pe = -1; // ptEta flag
 
 if(type == "RP")
 {
  t = 0;
 } else if(type == "POI")
   {
    t = 1;
   }

 if(ptOrEta == "Pt")
 {
  pe = 0;
 } else if(ptOrEta == "Eta")
   {
    pe = 1;
   }
    
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 Double_t minPtEta[2] = {fPtMin,fEtaMin};
 //Double_t maxPtEta[2] = {fPtMax,fEtaMax};
 Double_t binWidthPtEta[2] = {fPtBinWidth,fEtaBinWidth};

 // looping over all bins and calculating reduced correlations: 
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  // real and imaginary parts of p_{m*n,0} (non-weighted Q-vector evaluated for POIs in particular pt or eta bin): 
  Double_t p1n0kRe = 0.;
  Double_t p1n0kIm = 0.;

  // number of POIs in particular pt or eta bin:
  Double_t mp = 0.;

  // real and imaginary parts of q_{m*n,0} (non-weighted Q-vector evaluated for particles which are both RPs and POIs in particular pt or eta bin):
  Double_t q1n0kRe = 0.;
  Double_t q1n0kIm = 0.;
  Double_t q2n0kRe = 0.;
  Double_t q2n0kIm = 0.;

  // number of particles which are both RPs and POIs in particular pt or eta bin:
  Double_t mq = 0.;
   
  // q_{m*n,0}:
  q1n0kRe = fReRPQ1dEBE[2][pe][0][0]->GetBinContent(fReRPQ1dEBE[2][pe][0][0]->GetBin(b))
          * fReRPQ1dEBE[2][pe][0][0]->GetBinEntries(fReRPQ1dEBE[2][pe][0][0]->GetBin(b));
  q1n0kIm = fImRPQ1dEBE[2][pe][0][0]->GetBinContent(fImRPQ1dEBE[2][pe][0][0]->GetBin(b))
          * fImRPQ1dEBE[2][pe][0][0]->GetBinEntries(fImRPQ1dEBE[2][pe][0][0]->GetBin(b));
  q2n0kRe = fReRPQ1dEBE[2][pe][1][0]->GetBinContent(fReRPQ1dEBE[2][pe][1][0]->GetBin(b))
          * fReRPQ1dEBE[2][pe][1][0]->GetBinEntries(fReRPQ1dEBE[2][pe][1][0]->GetBin(b));
  q2n0kIm = fImRPQ1dEBE[2][pe][1][0]->GetBinContent(fImRPQ1dEBE[2][pe][1][0]->GetBin(b))
          * fImRPQ1dEBE[2][pe][1][0]->GetBinEntries(fImRPQ1dEBE[2][pe][1][0]->GetBin(b));         
               
   mq = fReRPQ1dEBE[2][pe][0][0]->GetBinEntries(fReRPQ1dEBE[2][pe][0][0]->GetBin(b)); // to be improved (cross-checked by accessing other profiles here)
   
   if(type == "POI")
   {
    // p_{m*n,0}:
    p1n0kRe = fReRPQ1dEBE[1][pe][0][0]->GetBinContent(fReRPQ1dEBE[1][pe][0][0]->GetBin(b))
            * fReRPQ1dEBE[1][pe][0][0]->GetBinEntries(fReRPQ1dEBE[1][pe][0][0]->GetBin(b));
    p1n0kIm = fImRPQ1dEBE[1][pe][0][0]->GetBinContent(fImRPQ1dEBE[1][pe][0][0]->GetBin(b))  
            * fImRPQ1dEBE[1][pe][0][0]->GetBinEntries(fImRPQ1dEBE[1][pe][0][0]->GetBin(b));
            
    mp = fReRPQ1dEBE[1][pe][0][0]->GetBinEntries(fReRPQ1dEBE[1][pe][0][0]->GetBin(b)); // to be improved (cross-checked by accessing other profiles here)
    
    t = 1; // typeFlag = RP or POI
   }
   else if(type == "RP")
   {
    // p_{m*n,0} = q_{m*n,0}:
    p1n0kRe = q1n0kRe; 
    p1n0kIm = q1n0kIm; 
    mp = mq; 
    
    t = 0; // typeFlag = RP or POI
   }
      
   // 2'-particle correlation for particular (pt,eta) bin:
   Double_t two1n1nPtEta = 0.;
   if(mp*dMult-mq)
   {
    two1n1nPtEta = (p1n0kRe*dReQ1n+p1n0kIm*dImQ1n-mq)
                 / (mp*dMult-mq);
   
    if(type == "POI")
    { 
     // fill profile to get <<2'>> for POIs
     fDiffFlowCorrelationsPro[1][pe][0]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],two1n1nPtEta,mp*dMult-mq);
     // histogram to store <2'> for POIs e-b-e (needed in some other methods):
     fDiffFlowCorrelationsEBE[1][pe][0]->SetBinContent(b,two1n1nPtEta);      
    }
    else if(type == "RP")
    {
     // profile to get <<2'>> for RPs:
     fDiffFlowCorrelationsPro[0][pe][0]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],two1n1nPtEta,mp*dMult-mq);
     // histogram to store <2'> for RPs e-b-e (needed in some other methods):
     fDiffFlowCorrelationsEBE[0][pe][0]->SetBinContent(b,two1n1nPtEta); 
    }
   } // end of if(mp*dMult-mq)
  
   // 4'-particle correlation:
   Double_t four1n1n1n1nPtEta = 0.;
   if((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
       + mq*(dMult-1.)*(dMult-2.)*(dMult-3.)) // to be improved (introduce a new variable for this expression)
   {
    four1n1n1n1nPtEta = ((pow(dReQ1n,2.)+pow(dImQ1n,2.))*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)
                      - q2n0kRe*(pow(dReQ1n,2.)-pow(dImQ1n,2.))
                      - 2.*q2n0kIm*dReQ1n*dImQ1n
                      - p1n0kRe*(dReQ1n*dReQ2n+dImQ1n*dImQ2n)
                      + p1n0kIm*(dImQ1n*dReQ2n-dReQ1n*dImQ2n)
                      - 2.*dMult*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)
                      - 2.*(pow(dReQ1n,2.)+pow(dImQ1n,2.))*mq                      
                      + 6.*(q1n0kRe*dReQ1n+q1n0kIm*dImQ1n)                                            
                      + 1.*(q2n0kRe*dReQ2n+q2n0kIm*dImQ2n)                      
                      + 2.*(p1n0kRe*dReQ1n+p1n0kIm*dImQ1n)                       
                      + 2.*mq*dMult                      
                      - 6.*mq)        
                      / ((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                          + mq*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
    
    if(type == "POI")
    {
     // profile to get <<4'>> for POIs:
     fDiffFlowCorrelationsPro[1][pe][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],four1n1n1n1nPtEta,
                                     (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                                     + mq*(dMult-1.)*(dMult-2.)*(dMult-3.)); 
     // histogram to store <4'> for POIs e-b-e (needed in some other methods):
     fDiffFlowCorrelationsEBE[1][pe][1]->SetBinContent(b,four1n1n1n1nPtEta);                               
    }
    else if(type == "RP")
    {
     // profile to get <<4'>> for RPs:
     fDiffFlowCorrelationsPro[0][pe][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],four1n1n1n1nPtEta,
                                       (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
                                       + mq*(dMult-1.)*(dMult-2.)*(dMult-3.));                   
     // histogram to store <4'> for RPs e-b-e (needed in some other methods):
     fDiffFlowCorrelationsEBE[0][pe][1]->SetBinContent(b,four1n1n1n1nPtEta);                   
    }
   } // end of if((mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     //            +mq*(dMult-1.)*(dMult-2.)*(dMult-3.))
   
 } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 
   
} // end of void AliFlowAnalysisWithQCumulants::CalculateReducedCorrelations1D(TString type, TString ptOrEta);


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateDiffFlowSumOfEventWeights(TString type, TString ptOrEta)
{
 // Calculate sums of various event weights for reduced correlations. 
 // (These quantitites are needed in expressions for unbiased estimators relevant for the statistical errors.)

 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
   
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
 
 // binning:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 Double_t minPtEta[2] = {fPtMin,fEtaMin};
 //Double_t maxPtEta[2] = {fPtMax,fEtaMax};
 Double_t binWidthPtEta[2] = {fPtBinWidth,fEtaBinWidth};
 
 for(Int_t rpq=0;rpq<3;rpq++)
 {
  for(Int_t m=0;m<4;m++)
  {
   for(Int_t k=0;k<9;k++)
   {
    if(!fReRPQ1dEBE[rpq][pe][m][k])
    {
     cout<<"WARNING: fReRPQ1dEBE[rpq][pe][m][k] is NULL in AFAWQC::CSAPOEWFDF() !!!!"<<endl;
     cout<<"pe  = "<<pe<<endl;
     cout<<"rpq = "<<rpq<<endl;
     cout<<"m   = "<<m<<endl;
     cout<<"k   = "<<k<<endl;
     exit(0); 
    }
   }
  }
 }  

 // multiplicities:
 Double_t dMult = (*fSMpk)(0,0); // total event multiplicity
 Double_t mr = 0.; // number of RPs in particular pt or eta bin
 Double_t mp = 0.; // number of POIs in particular pt or eta bin 
 Double_t mq = 0.; // number of particles which are both RPs and POIs in particular pt or eta bin
 
 // event weights for reduced correlations:
 Double_t dw2 = 0.; // event weight for <2'>
 Double_t dw4 = 0.; // event weight for <4'>
 //Double_t dw6 = 0.; // event weight for <6'>
 //Double_t dw8 = 0.; // event weight for <8'>

 // looping over bins:
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  if(type == "RP")
  {
   mr = fReRPQ1dEBE[0][pe][0][0]->GetBinEntries(b);
   mp = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
   mq = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
  } else if(type == "POI")
    {
     mp = fReRPQ1dEBE[1][pe][0][0]->GetBinEntries(b);
     mq = fReRPQ1dEBE[2][pe][0][0]->GetBinEntries(b);    
    }
  
  // event weight for <2'>:
  dw2 = mp*dMult-mq;  
  fDiffFlowSumOfEventWeights[t][pe][0][0]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2);
  fDiffFlowSumOfEventWeights[t][pe][1][0]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],pow(dw2,2.));
  
  // event weight for <4'>:
  dw4 = (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     + mq*(dMult-1.)*(dMult-2.)*(dMult-3.);  
  fDiffFlowSumOfEventWeights[t][pe][0][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw4);
  fDiffFlowSumOfEventWeights[t][pe][1][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],pow(dw4,2.));
  
  // event weight for <6'>:
  //dw6 = ...;  
  //fDiffFlowSumOfEventWeights[t][pe][0][2]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw6);
  //fDiffFlowSumOfEventWeights[t][pe][t][1][2]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],pow(dw6,2.));
  
  // event weight for <8'>:
  //dw8 = ...;  
  //fDiffFlowSumOfEventWeights[t][pe][0][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw8);
  //fDiffFlowSumOfEventWeights[t][pe][1][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],pow(dw8,2.));   
 } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++) 
 
} // end of void AliFlowAnalysisWithQCumulants::CalculateDiffFlowSumOfEventWeights()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateDiffFlowSumOfProductOfEventWeights(TString type, TString ptOrEta)
{
 // Calculate sum of products of various event weights for both types of correlations (the ones for int. and diff. flow). 
 // (These quantitites are needed in expressions for unbiased estimators relevant for the statistical errors.)
 //
 // Important: To fill fDiffFlowSumOfProductOfEventWeights[][][][] use bellow table (i,j) with following constraints: 
 // 1.) i<j  
 // 2.) do not store terms which DO NOT include reduced correlations;
 // Table:
 // [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>] x [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>]
  
 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
     
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
  
 // binning:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 Double_t minPtEta[2] = {fPtMin,fEtaMin};
 //Double_t maxPtEta[2] = {fPtMax,fEtaMax};
 Double_t binWidthPtEta[2] = {fPtBinWidth,fEtaBinWidth};
 
 // protection:
 for(Int_t rpq=0;rpq<3;rpq++)
 {
  for(Int_t m=0;m<4;m++)
  {
   for(Int_t k=0;k<9;k++)
   {
    if(!fReRPQ1dEBE[rpq][pe][m][k])
    {
     cout<<"WARNING: fReRPQ1dEBE[rpq][pe][m][k] is NULL in AFAWQC::CSAPOEWFDF() !!!!"<<endl;
     cout<<"pe  = "<<pe<<endl;
     cout<<"rpq = "<<rpq<<endl;
     cout<<"m   = "<<m<<endl;
     cout<<"k   = "<<k<<endl;
     exit(0); 
    }
   }
  }
 }  
 
 // multiplicities:
 Double_t dMult = (*fSMpk)(0,0); // total event multiplicity
 Double_t mr = 0.; // number of RPs in particular pt or eta bin
 Double_t mp = 0.; // number of POIs in particular pt or eta bin 
 Double_t mq = 0.; // number of particles which are both RPs and POIs in particular pt or eta bin
 
 // event weights for correlations:
 Double_t dW2 = dMult*(dMult-1); // event weight for <2> 
 Double_t dW4 = dMult*(dMult-1)*(dMult-2)*(dMult-3); // event weight for <4> 
 Double_t dW6 = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5); // event weight for <6> 
 Double_t dW8 = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7); // event weight for <8> 

 // event weights for reduced correlations:
 Double_t dw2 = 0.; // event weight for <2'>
 Double_t dw4 = 0.; // event weight for <4'>
 //Double_t dw6 = 0.; // event weight for <6'>
 //Double_t dw8 = 0.; // event weight for <8'>
 
 // looping over bins:
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  if(type == "RP")
  {
   mr = fReRPQ1dEBE[0][pe][0][0]->GetBinEntries(b);
   mp = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
   mq = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
  } else if(type == "POI")
    {
     mp = fReRPQ1dEBE[1][pe][0][0]->GetBinEntries(b);
     mq = fReRPQ1dEBE[2][pe][0][0]->GetBinEntries(b);    
    }
  
  // event weight for <2'>:
  dw2 = mp*dMult-mq;  
  fDiffFlowSumOfProductOfEventWeights[t][pe][0][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW2*dw2); // storing product of even weights for <2> and <2'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][1][2]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dW4); // storing product of even weights for <4> and <2'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][1][4]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dW6); // storing product of even weights for <6> and <2'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][1][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dW8); // storing product of even weights for <8> and <2'>
  
  // event weight for <4'>:
  dw4 = (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     + mq*(dMult-1.)*(dMult-2.)*(dMult-3.);  
  fDiffFlowSumOfProductOfEventWeights[t][pe][0][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW2*dw4); // storing product of even weights for <2> and <4'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][1][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dw4); // storing product of even weights for <2'> and <4'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][2][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW4*dw4); // storing product of even weights for <4> and <4'>
  fDiffFlowSumOfProductOfEventWeights[t][pe][3][4]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw4*dW6); // storing product of even weights for <6> and <4'> 
  fDiffFlowSumOfProductOfEventWeights[t][pe][3][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw4*dW8); // storing product of even weights for <8> and <4'>

  // event weight for <6'>:
  //dw6 = ...;  
  //fDiffFlowSumOfProductOfEventWeights[t][pe][0][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW2*dw6); // storing product of even weights for <2> and <6'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][1][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dw6); // storing product of even weights for <2'> and <6'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][2][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW4*dw6); // storing product of even weights for <4> and <6'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][3][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw4*dw6); // storing product of even weights for <4'> and <6'> 
  //fDiffFlowSumOfProductOfEventWeights[t][pe][4][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW6*dw6); // storing product of even weights for <6> and <6'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][5][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw6*dW8); // storing product of even weights for <6'> and <8>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][5][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw6*dw8); // storing product of even weights for <6'> and <8'>

  // event weight for <8'>:
  //dw8 = ...;  
  //fDiffFlowSumOfProductOfEventWeights[t][pe][0][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW2*dw8); // storing product of even weights for <2> and <8'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][1][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw2*dw8); // storing product of even weights for <2'> and <8'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][2][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW4*dw8); // storing product of even weights for <4> and <8'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][3][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw4*dw8); // storing product of even weights for <4'> and <8'> 
  //fDiffFlowSumOfProductOfEventWeights[t][pe][4][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW6*dw8); // storing product of even weights for <6> and <8'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][5][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dw6*dw8); // storing product of even weights for <6'> and <8'>
  //fDiffFlowSumOfProductOfEventWeights[t][pe][6][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],dW8*dw8); // storing product of even weights for <8> and <8'>
  
  // Table:
  // [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>] x [0=<2>,1=<2'>,2=<4>,3=<4'>,4=<6>,5=<6'>,6=<8>,7=<8'>]
   
 } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 


} // end of void AliFlowAnalysisWithQCumulants::CalculateDiffFlowSumOfProductOfEventWeights(TString type, TString ptOrEta)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::FinalizeReducedCorrelations(TString type, TString ptOrEta)
{
 // Transfer profiles into histograms and calculate statistical errors correctly.

 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
             
 for(Int_t rci=0;rci<4;rci++)
 {
  if(!fDiffFlowCorrelationsPro[t][pe][rci])
  {
   cout<<"WARNING: fDiffFlowCorrelationsPro[t][pe][rci] is NULL in AFAWQC::FRC() !!!!"<<endl;
   cout<<"t   = "<<t<<endl; 
   cout<<"pe  = "<<pe<<endl; 
   cout<<"rci = "<<rci<<endl;
   exit(0); 
  }
  for(Int_t power=0;power<2;power++)
  {
   if(!fDiffFlowSumOfEventWeights[t][pe][power][rci])
   {
    cout<<"WARNING: fDiffFlowSumOfEventWeights[t][pe][power][rci] is NULL in AFAWQC::FRC() !!!!"<<endl;
    cout<<"t     = "<<t<<endl; 
    cout<<"pe    = "<<pe<<endl;
    cout<<"power = "<<power<<endl; 
    cout<<"rci   = "<<rci<<endl;
    exit(0); 
   }   
  } // end of for(Int_t power=0;power<2;power++)
 } // end of for(Int_t rci=0;rci<4;rci++)
    
 // common:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 
 // transfer 1D profile into 1D histogram:
 Double_t correlation = 0.;
 Double_t spread = 0.;
 Double_t sumOfWeights = 0.; // sum of weights for particular reduced correlations for particular pt or eta bin
 Double_t sumOfSquaredWeights = 0.; // sum of squared weights for particular reduced correlations for particular pt or eta bin
 Double_t error = 0.; // error = termA * spread * termB
                      // termA = (sqrt(sumOfSquaredWeights)/sumOfWeights) 
                      // termB = 1/pow(1-termA^2,0.5)
 Double_t termA = 0.;                      
 Double_t termB = 0.;                      
 for(Int_t rci=0;rci<4;rci++) // index of reduced correlation
 {
  for(Int_t b=1;b<=nBinsPtEta[pe];b++) // number of pt or eta bins
  {
   correlation = fDiffFlowCorrelationsPro[t][pe][rci]->GetBinContent(b); 
   spread = fDiffFlowCorrelationsPro[t][pe][rci]->GetBinError(b);
   sumOfWeights = fDiffFlowSumOfEventWeights[t][pe][0][rci]->GetBinContent(b);
   sumOfSquaredWeights = fDiffFlowSumOfEventWeights[t][pe][1][rci]->GetBinContent(b);
   if(sumOfWeights) termA = (pow(sumOfSquaredWeights,0.5)/sumOfWeights);
   if(1.-pow(termA,2.)>0.) termB = 1./pow(1.-pow(termA,2.),0.5); 
   error = termA*spread*termB; // final error (unbiased estimator for standard deviation)
   fDiffFlowCorrelationsHist[t][pe][rci]->SetBinContent(b,correlation); 
   fDiffFlowCorrelationsHist[t][pe][rci]->SetBinError(b,error); 
  } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 } // end of for(Int_t rci=0;rci<4;rci++)
 
} // end of void AliFlowAnalysisWithQCumulants::FinalizeReducedCorrelations(TString type, TString ptOrEta)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateDiffFlowProductOfCorrelations(TString type, TString ptOrEta)
{
 // store products: <2><2'>, <2><4'>, <2><6'>, <2><8'>, <2'><4>, 
 //                 <2'><4'>, <2'><6>, <2'><6'>, <2'><8>, <2'><8'>,
 //                 <4><4'>, <4><6'>, <4><8'>, <4'><6>, <4'><6'>, 
 //                 <4'><8>, <4'><8'>, <6><6'>, <6><8'>, <6'><8>, 
 //                 <6'><8'>, <8><8'>.
  
 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
     
 // common:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 Double_t minPtEta[2] = {fPtMin,fEtaMin};
 Double_t binWidthPtEta[2] = {fPtBinWidth,fEtaBinWidth};
   
 // protections // to be improved (add protection for all pointers in this method)
 if(!fIntFlowCorrelationsEBE)
 {
  cout<<"WARNING: fIntFlowCorrelationsEBE is NULL in AFAWQC::CDFPOC() !!!!"<<endl;
  exit(0);
 } 
   
 Double_t dMult = (*fSMpk)(0,0); // multiplicity (number of particles used to determine the reaction plane)
 Double_t mr = 0.; // number of RPs in particular pt or eta bin
 Double_t mp = 0.; // number of POIs in particular pt or eta bin 
 Double_t mq = 0.; // number of particles which are both RPs and POIs in particular pt or eta bin

 // e-b-e correlations:
 Double_t twoEBE = 0.; // <2>
 Double_t fourEBE = 0.; // <4>
 Double_t sixEBE = 0.; // <6>
 Double_t eightEBE = 0.; // <8>
 
 if(dMult>1)
 {  
  twoEBE = fIntFlowCorrelationsEBE->GetBinContent(1);
  if(dMult>3)
  {
   fourEBE = fIntFlowCorrelationsEBE->GetBinContent(2);
   if(dMult>5) 
   {
    sixEBE = fIntFlowCorrelationsEBE->GetBinContent(3);
    if(dMult>7) 
    { 
     eightEBE = fIntFlowCorrelationsEBE->GetBinContent(4);
    }
   }
  }
 }  
 
 // event weights for correlations:
 Double_t dW2 = dMult*(dMult-1); // event weight for <2> 
 Double_t dW4 = dMult*(dMult-1)*(dMult-2)*(dMult-3); // event weight for <4> 
 Double_t dW6 = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5); // event weight for <6> 
 Double_t dW8 = dMult*(dMult-1)*(dMult-2)*(dMult-3)*(dMult-4)*(dMult-5)*(dMult-6)*(dMult-7); // event weight for <8> 
  
 // e-b-e reduced correlations:
 Double_t twoReducedEBE = 0.; // <2'>
 Double_t fourReducedEBE = 0.; // <4'>
 Double_t sixReducedEBE = 0.; // <6'>
 Double_t eightReducedEBE = 0.; // <8'> 
 
 // event weights for reduced correlations:
 Double_t dw2 = 0.; // event weight for <2'>
 Double_t dw4 = 0.; // event weight for <4'>
 //Double_t dw6 = 0.; // event weight for <6'>
 //Double_t dw8 = 0.; // event weight for <8'>

 // looping over bins:
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  // e-b-e reduced correlations:
  twoReducedEBE = fDiffFlowCorrelationsEBE[t][pe][0]->GetBinContent(b);
  fourReducedEBE = fDiffFlowCorrelationsEBE[t][pe][1]->GetBinContent(b);
  sixReducedEBE = fDiffFlowCorrelationsEBE[t][pe][2]->GetBinContent(b);
  eightReducedEBE = fDiffFlowCorrelationsEBE[t][pe][3]->GetBinContent(b);

  // to be improved (I should not do this here again)
  if(type == "RP")
  {
   mr = fReRPQ1dEBE[0][pe][0][0]->GetBinEntries(b);
   mp = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
   mq = mr; // trick to use the very same Eqs. bellow both for RP's and POI's diff. flow
  } else if(type == "POI")
    {
     mp = fReRPQ1dEBE[1][pe][0][0]->GetBinEntries(b);
     mq = fReRPQ1dEBE[2][pe][0][0]->GetBinEntries(b);    
    }
  
  // event weights for reduced correlations:
  dw2 = mp*dMult-mq; // weight for <2'> 
  dw4 = (mp-mq)*dMult*(dMult-1.)*(dMult-2.)
     + mq*(dMult-1.)*(dMult-2.)*(dMult-3.); // weight for <4'>
  //dw6 = ...     
  //dw8 = ...     
 
  // storing all products:
  fDiffFlowProductOfCorrelationsPro[t][pe][0][1]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoEBE*twoReducedEBE,dW2*dw2); // storing <2><2'>
  fDiffFlowProductOfCorrelationsPro[t][pe][1][2]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourEBE*twoReducedEBE,dW4*dw2); // storing <4><2'>
  fDiffFlowProductOfCorrelationsPro[t][pe][1][4]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixEBE*twoReducedEBE,dW6*dw2); // storing <6><2'>
  fDiffFlowProductOfCorrelationsPro[t][pe][1][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],eightEBE*twoReducedEBE,dW8*dw2); // storing <8><2'>
  
  // event weight for <4'>:
  fDiffFlowProductOfCorrelationsPro[t][pe][0][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoEBE*fourReducedEBE,dW2*dw4); // storing <2><4'>
  fDiffFlowProductOfCorrelationsPro[t][pe][1][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoReducedEBE*fourReducedEBE,dw2*dw4); // storing <2'><4'>
  fDiffFlowProductOfCorrelationsPro[t][pe][2][3]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourEBE*fourReducedEBE,dW4*dw4); // storing <4><4'>
  fDiffFlowProductOfCorrelationsPro[t][pe][3][4]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixEBE*fourReducedEBE,dW6*dw4); // storing <6><4'> 
  fDiffFlowProductOfCorrelationsPro[t][pe][3][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],eightEBE*fourReducedEBE,dW8*dw4); // storing <8><4'>

  // event weight for <6'>:
  //dw6 = ...;  
  //fDiffFlowProductOfCorrelationsPro[t][pe][0][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoEBE*sixReducedEBE,dW2*dw6); // storing <2><6'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][1][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoReducedEBE*sixReducedEBE,dw2*dw6); // storing <2'><6'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][2][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourEBE*sixReducedEBE,dW4*dw6); // storing <4><6'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][3][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourReducedEBE*sixReducedEBE,dw4*dw6); // storing <4'><6'> 
  //fDiffFlowProductOfCorrelationsPro[t][pe][4][5]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixEBE*sixReducedEBE,dW6*dw6); // storing <6><6'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][5][6]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixReducedEBE*eightEBE,dw6*dW8); // storing <6'><8>
  //fDiffFlowProductOfCorrelationsPro[t][pe][5][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixReducedEBE*eightReducedEBE,dw6*dw8); // storing <6'><8'>

  // event weight for <8'>:
  //dw8 = ...;  
  //fDiffFlowProductOfCorrelationsPro[t][pe][0][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoEBE*eightReducedEBE,dW2*dw8); // storing <2><8'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][1][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],twoReducedEBE*eightReducedEBE,dw2*dw8); // storing <2'><8'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][2][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourEBE*eightReducedEBE,dW4*dw8); // storing <4><8'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][3][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],fourReducedEBE*eightReducedEBE,dw4*dw8); // storing <4'><8'> 
  //fDiffFlowProductOfCorrelationsPro[t][pe][4][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixEBE*eightReducedEBE,dW6*dw8); // storing <6><8'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][5][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],sixReducedEBE*eightReducedEBE,dw6*dw8); // storing <6'><8'>
  //fDiffFlowProductOfCorrelationsPro[t][pe][6][7]->Fill(minPtEta[pe]+(b-1)*binWidthPtEta[pe],eightEBE*eightReducedEBE,dW8*dw8); // storing <8><8'> 
 } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++       
     
} // end of void AliFlowAnalysisWithQCumulants::CalculateDiffFlowProductOfCorrelations(TString type, TString ptOrEta)


//================================================================================================================================
    
    
void AliFlowAnalysisWithQCumulants::CalculateDiffFlowCovariances(TString type, TString ptOrEta) // to be improved (reimplemented)
{
 // a) Calculate unbiased estimators Cov(<2>,<2'>), Cov(<2>,<4'>), Cov(<4>,<2'>), Cov(<4>,<4'>) and Cov(<2'>,<4'>)
 //    for covariances V(<2>,<2'>), V(<2>,<4'>), V(<4>,<2'>), V(<4>,<4'>) and V(<2'>,<4'>).  
 // b) Store in histogram fDiffFlowCovariances[t][pe][index] for instance the following: 
 //
 //             Cov(<2>,<2'>) * (sum_{i=1}^{N} w_{<2>}_i w_{<2'>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<2'>}_j)]
 // 
 //     where N is the number of events, w_{<2>} is event weight for <2> and w_{<2'>} is event weight for <2'>.
 // c) Binning of fDiffFlowCovariances[t][pe][index] is organized as follows:
 // 
 //     1st bin: Cov(<2>,<2'>) * (sum_{i=1}^{N} w_{<2>}_i w_{<2'>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<2'>}_j)] 
 //     2nd bin: Cov(<2>,<4'>) * (sum_{i=1}^{N} w_{<2>}_i w_{<4'>}_i )/[(sum_{i=1}^{N} w_{<2>}_i) * (sum_{j=1}^{N} w_{<4'>}_j)] 
 //     3rd bin: Cov(<4>,<2'>) * (sum_{i=1}^{N} w_{<4>}_i w_{<2'>}_i )/[(sum_{i=1}^{N} w_{<4>}_i) * (sum_{j=1}^{N} w_{<2'>}_j)] 
 //     4th bin: Cov(<4>,<4'>) * (sum_{i=1}^{N} w_{<4>}_i w_{<4'>}_i )/[(sum_{i=1}^{N} w_{<4>}_i) * (sum_{j=1}^{N} w_{<4'>}_j)] 
 //     5th bin: Cov(<2'>,<4'>) * (sum_{i=1}^{N} w_{<2'>}_i w_{<4'>}_i )/[(sum_{i=1}^{N} w_{<2'>}_i) * (sum_{j=1}^{N} w_{<4'>}_j)] 
 //     ...
  
 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
     
 // common:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 //Double_t minPtEta[2] = {fPtMin,fEtaMin};
 //Double_t maxPtEta[2] = {fPtMax,fEtaMax};
 //Double_t binWidthPtEta[2] = {fPtBinWidth,fEtaBinWidth};
 
 // average correlations:
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1); // <<2>>
 Double_t four = fIntFlowCorrelationsHist->GetBinContent(2); // <<4>>
 //Double_t six = fIntFlowCorrelationsHist->GetBinContent(3); // <<6>>
 //Double_t eight = fIntFlowCorrelationsHist->GetBinContent(4); // <<8>>
 
 // sum of weights for correlation:
 Double_t sumOfWeightsForTwo = fIntFlowSumOfEventWeights[0]->GetBinContent(1); // sum_{i=1}^{N} w_{<2>}
 Double_t sumOfWeightsForFour = fIntFlowSumOfEventWeights[0]->GetBinContent(2); // sum_{i=1}^{N} w_{<4>}
 //Double_t sumOfWeightsForSix = fIntFlowSumOfEventWeights[0]->GetBinContent(3); // sum_{i=1}^{N} w_{<6>}
 //Double_t sumOfWeightsForEight = fIntFlowSumOfEventWeights[0]->GetBinContent(4); // sum_{i=1}^{N} w_{<8>}
 
 // average reduced correlations:
 Double_t twoReduced = 0.; // <<2'>> 
 Double_t fourReduced = 0.; // <<4'>>
 //Double_t sixReduced = 0.; // <<6'>>
 //Double_t eightReduced = 0.; // <<8'>>

 // sum of weights for reduced correlation:
 Double_t sumOfWeightsForTwoReduced = 0.; // sum_{i=1}^{N} w_{<2'>}
 Double_t sumOfWeightsForFourReduced = 0.; // sum_{i=1}^{N} w_{<4'>}
 //Double_t sumOfWeightsForSixReduced = 0.; // sum_{i=1}^{N} w_{<6'>}
 //Double_t sumOfWeightsForEightReduced = 0.; // sum_{i=1}^{N} w_{<8'>}
  
 // product of weights for reduced correlation:
 Double_t productOfWeightsForTwoTwoReduced = 0.; // sum_{i=1}^{N} w_{<2>}w_{<2'>}
 Double_t productOfWeightsForTwoFourReduced = 0.; // sum_{i=1}^{N} w_{<2>}w_{<4'>}
 Double_t productOfWeightsForFourTwoReduced = 0.; // sum_{i=1}^{N} w_{<4>}w_{<2'>}
 Double_t productOfWeightsForFourFourReduced = 0.; // sum_{i=1}^{N} w_{<4>}w_{<4'>}
 Double_t productOfWeightsForTwoReducedFourReduced = 0.; // sum_{i=1}^{N} w_{<2'>}w_{<4'>}
 // ...
 
 // products for differential flow:
 Double_t twoTwoReduced = 0; // <<2><2'>> 
 Double_t twoFourReduced = 0; // <<2><4'>> 
 Double_t fourTwoReduced = 0; // <<4><2'>> 
 Double_t fourFourReduced = 0; // <<4><4'>> 
 Double_t twoReducedFourReduced = 0; // <<2'><4'>> 

 // denominators in the expressions for the unbiased estimators for covariances:
 // denominator = 1 - term1/(term2*term3)
 // prefactor = term1/(term2*term3)
 Double_t denominator = 0.; 
 Double_t prefactor = 0.;
 Double_t term1 = 0.; 
 Double_t term2 = 0.; 
 Double_t term3 = 0.; 
 
 // unbiased estimators for covariances for differential flow:
 Double_t covTwoTwoReduced = 0.; // Cov(<2>,<2'>)
 Double_t wCovTwoTwoReduced = 0.; // Cov(<2>,<2'>) * prefactor(w_{<2>},w_{<2'>})
 Double_t covTwoFourReduced = 0.; // Cov(<2>,<4'>)
 Double_t wCovTwoFourReduced = 0.; // Cov(<2>,<4'>) * prefactor(w_{<2>},w_{<4'>})
 Double_t covFourTwoReduced = 0.; // Cov(<4>,<2'>)
 Double_t wCovFourTwoReduced = 0.; // Cov(<4>,<2'>) * prefactor(w_{<4>},w_{<2'>})
 Double_t covFourFourReduced = 0.; // Cov(<4>,<4'>)
 Double_t wCovFourFourReduced = 0.; // Cov(<4>,<4'>) * prefactor(w_{<4>},w_{<4'>})
 Double_t covTwoReducedFourReduced = 0.; // Cov(<2'>,<4'>)
 Double_t wCovTwoReducedFourReduced = 0.; // Cov(<2'>,<4'>) * prefactor(w_{<2'>},w_{<4'>})
 
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  // average reduced corelations:
  twoReduced = fDiffFlowCorrelationsHist[t][pe][0]->GetBinContent(b);
  fourReduced = fDiffFlowCorrelationsHist[t][pe][1]->GetBinContent(b);
  // average products:
  twoTwoReduced = fDiffFlowProductOfCorrelationsPro[t][pe][0][1]->GetBinContent(b);
  twoFourReduced = fDiffFlowProductOfCorrelationsPro[t][pe][0][3]->GetBinContent(b);
  fourTwoReduced = fDiffFlowProductOfCorrelationsPro[t][pe][1][2]->GetBinContent(b);
  fourFourReduced = fDiffFlowProductOfCorrelationsPro[t][pe][2][3]->GetBinContent(b);
  twoReducedFourReduced = fDiffFlowProductOfCorrelationsPro[t][pe][1][3]->GetBinContent(b);  
  // sum of weights for reduced correlations:
  sumOfWeightsForTwoReduced = fDiffFlowSumOfEventWeights[t][pe][0][0]->GetBinContent(b);
  sumOfWeightsForFourReduced = fDiffFlowSumOfEventWeights[t][pe][0][1]->GetBinContent(b);
  // products of weights for correlations:
  productOfWeightsForTwoTwoReduced = fDiffFlowSumOfProductOfEventWeights[t][pe][0][1]->GetBinContent(b); 
  productOfWeightsForTwoFourReduced = fDiffFlowSumOfProductOfEventWeights[t][pe][0][3]->GetBinContent(b);
  productOfWeightsForFourTwoReduced = fDiffFlowSumOfProductOfEventWeights[t][pe][1][2]->GetBinContent(b);
  productOfWeightsForFourFourReduced = fDiffFlowSumOfProductOfEventWeights[t][pe][2][3]->GetBinContent(b);
  productOfWeightsForTwoReducedFourReduced = fDiffFlowSumOfProductOfEventWeights[t][pe][1][3]->GetBinContent(b);
  // denominator for the unbiased estimator for covariances: 1 - term1/(term2*term3) 
  // prefactor (multiplies Cov's) = term1/(term2*term3)       
  // <2>,<2'>:
  term1 = productOfWeightsForTwoTwoReduced;      
  term2 = sumOfWeightsForTwo;
  term3 = sumOfWeightsForTwoReduced;        
  if(term2*term3>0.)
  {
   denominator = 1.-term1/(term2*term3);
   prefactor = term1/(term2*term3);
   if(denominator!=0.)
   {
    covTwoTwoReduced = (twoTwoReduced-two*twoReduced)/denominator;            
    wCovTwoTwoReduced = covTwoTwoReduced*prefactor; 
    fDiffFlowCovariances[t][pe][0]->SetBinContent(b,wCovTwoTwoReduced);
   }
  }
  // <2>,<4'>:
  term1 = productOfWeightsForTwoFourReduced;      
  term2 = sumOfWeightsForTwo;
  term3 = sumOfWeightsForFourReduced;        
  if(term2*term3>0.)
  {
   denominator = 1.-term1/(term2*term3);
   prefactor = term1/(term2*term3);
   if(denominator!=0.)
   {
    covTwoFourReduced = (twoFourReduced-two*fourReduced)/denominator;            
    wCovTwoFourReduced = covTwoFourReduced*prefactor; 
    fDiffFlowCovariances[t][pe][1]->SetBinContent(b,wCovTwoFourReduced);
   }
  }
  // <4>,<2'>:
  term1 = productOfWeightsForFourTwoReduced;      
  term2 = sumOfWeightsForFour;
  term3 = sumOfWeightsForTwoReduced;        
  if(term2*term3>0.)
  {
   denominator = 1.-term1/(term2*term3);
   prefactor = term1/(term2*term3);
   if(denominator!=0.)
   {
    covFourTwoReduced = (fourTwoReduced-four*twoReduced)/denominator;            
    wCovFourTwoReduced = covFourTwoReduced*prefactor; 
    fDiffFlowCovariances[t][pe][2]->SetBinContent(b,wCovFourTwoReduced);
   }
  }
  // <4>,<4'>:
  term1 = productOfWeightsForFourFourReduced;      
  term2 = sumOfWeightsForFour;
  term3 = sumOfWeightsForFourReduced;        
  if(term2*term3>0.)
  {
   denominator = 1.-term1/(term2*term3);
   prefactor = term1/(term2*term3);
   if(denominator!=0.)
   {
    covFourFourReduced = (fourFourReduced-four*fourReduced)/denominator;            
    wCovFourFourReduced = covFourFourReduced*prefactor; 
    fDiffFlowCovariances[t][pe][3]->SetBinContent(b,wCovFourFourReduced);
   }
  }
  // <2'>,<4'>:
  term1 = productOfWeightsForTwoReducedFourReduced;      
  term2 = sumOfWeightsForTwoReduced;
  term3 = sumOfWeightsForFourReduced;        
  if(term2*term3>0.)
  {
   denominator = 1.-term1/(term2*term3);
   prefactor = term1/(term2*term3);
   if(denominator!=0.)
   {
    covTwoReducedFourReduced = (twoReducedFourReduced-twoReduced*fourReduced)/denominator;            
    wCovTwoReducedFourReduced = covTwoReducedFourReduced*prefactor; 
    fDiffFlowCovariances[t][pe][4]->SetBinContent(b,wCovTwoReducedFourReduced);
   }
  }
   
 } // end of for(Int_t b=1;b<=nBinsPtEta[pe];b++)
  
 
 
} // end of void AliFlowAnalysisWithQCumulants::CalculateDiffFlowCovariances(TString type, TString ptOrEta)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateDiffFlow(TString type, TString ptOrEta)
{
 // calculate differential flow from differential cumulants and previously obtained integrated flow: (to be improved: description)
 
 Int_t typeFlag = -1;
 Int_t ptEtaFlag = -1;

 if(type == "RP")
 {
  typeFlag = 0;
 } else if(type == "POI")
   {
    typeFlag = 1;
   } 
     
 if(ptOrEta == "Pt")
 {
  ptEtaFlag = 0;
 } else if(ptOrEta == "Eta")
   {
    ptEtaFlag = 1;
   } 
  
 // shortcuts:
 Int_t t = typeFlag;
 Int_t pe = ptEtaFlag;
     
 // common:
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
   
 // correlations:
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1); // <<2>>
 Double_t four = fIntFlowCorrelationsHist->GetBinContent(2); // <<4>>
 
 // statistical errors of correlations:
 Double_t twoError = fIntFlowCorrelationsHist->GetBinError(1);
 Double_t fourError = fIntFlowCorrelationsHist->GetBinError(2);   
    
 // reduced correlations:
 Double_t twoReduced = 0.; // <<2'>>
 Double_t fourReduced = 0.; // <<4'>>
 
 // statistical errors of reduced correlations:
 Double_t twoReducedError = 0.; 
 Double_t fourReducedError = 0.; 

 // covariances:
 Double_t wCovTwoFour = fIntFlowCovariances->GetBinContent(1);// // Cov(<2>,<4>) * prefactor(<2>,<4>)
 Double_t wCovTwoTwoReduced = 0.; // Cov(<2>,<2'>) * prefactor(<2>,<2'>)
 Double_t wCovTwoFourReduced = 0.; // Cov(<2>,<4'>) * prefactor(<2>,<4'>)
 Double_t wCovFourTwoReduced = 0.; // Cov(<4>,<2'>) * prefactor(<4>,<2'>)
 Double_t wCovFourFourReduced = 0.; // Cov(<4>,<4'>) * prefactor(<4>,<4'>)
 Double_t wCovTwoReducedFourReduced = 0.; // Cov(<2'>,<4'>) * prefactor(<2'>,<4'>)
 
 // differential flow:
 Double_t v2Prime = 0.; // v'{2}                   
 Double_t v4Prime = 0.; // v'{4}
 
 // statistical error of differential flow:
 Double_t v2PrimeError = 0.;                    
 Double_t v4PrimeError = 0.; 
 
 // squared statistical error of differential flow:
 Double_t v2PrimeErrorSquared = 0.;                    
 Double_t v4PrimeErrorSquared = 0.; 
 
 // loop over pt or eta bins:
 for(Int_t b=1;b<=nBinsPtEta[pe];b++)
 {
  // reduced correlations and statistical errors:
  twoReduced = fDiffFlowCorrelationsHist[t][pe][0]->GetBinContent(b);
  twoReducedError = fDiffFlowCorrelationsHist[t][pe][0]->GetBinError(b);
  fourReduced = fDiffFlowCorrelationsHist[t][pe][1]->GetBinContent(b);
  fourReducedError = fDiffFlowCorrelationsHist[t][pe][1]->GetBinError(b);
  // covariances:
  wCovTwoTwoReduced = fDiffFlowCovariances[t][pe][0]->GetBinContent(b);
  wCovTwoFourReduced = fDiffFlowCovariances[t][pe][1]->GetBinContent(b);
  wCovFourTwoReduced = fDiffFlowCovariances[t][pe][2]->GetBinContent(b);
  wCovFourFourReduced = fDiffFlowCovariances[t][pe][3]->GetBinContent(b);
  wCovTwoReducedFourReduced = fDiffFlowCovariances[t][pe][4]->GetBinContent(b);
  // differential flow:
  // v'{2}:
  if(two>0.) 
  {
   v2Prime = twoReduced/pow(two,0.5);
   v2PrimeErrorSquared = (1./4.)*pow(two,-3.)*
                         (pow(twoReduced,2.)*pow(twoError,2.)
                          + 4.*pow(two,2.)*pow(twoReducedError,2.)
                          - 4.*two*twoReduced*wCovTwoTwoReduced);
     
                                                            
   if(v2PrimeErrorSquared>0.) v2PrimeError = pow(v2PrimeErrorSquared,0.5);
   fDiffFlow[t][pe][0]->SetBinContent(b,v2Prime); 
   fDiffFlow[t][pe][0]->SetBinError(b,v2PrimeError);     
  }
  // differential flow:
  // v'{4}
  if(2.*pow(two,2.)-four > 0.) 
  {
   v4Prime = (2.*two*twoReduced-fourReduced)/pow(2.*pow(two,2.)-four,3./4.);
   v4PrimeErrorSquared = pow(2.*pow(two,2.)-four,-7./2.)*
                         (pow(2.*pow(two,2.)*twoReduced-3.*two*fourReduced+2.*four*twoReduced,2.)*pow(twoError,2.)
                          + (9./16.)*pow(2.*two*twoReduced-fourReduced,2.)*pow(fourError,2.)
                          + 4.*pow(two,2.)*pow(2.*pow(two,2.)-four,2.)*pow(twoReducedError,2.)
                          + pow(2.*pow(two,2.)-four,2.)*pow(fourReducedError,2.)                          
                          - (3./2.)*(2.*two*twoReduced-fourReduced)
                          * (2.*pow(two,2.)*twoReduced-3.*two*fourReduced+2.*four*twoReduced)*wCovTwoFour
                          - 4.*two*(2.*pow(two,2.)-four)
                          * (2.*pow(two,2.)*twoReduced-3.*two*fourReduced+2.*four*twoReduced)*wCovTwoTwoReduced
                          + 2.*(2.*pow(two,2.)-four)
                          * (2.*pow(two,2.)*twoReduced-3.*two*fourReduced+2.*four*twoReduced)*wCovTwoFourReduced
                          + 3.*two*(2.*pow(two,2.)-four)*(2.*two*twoReduced-fourReduced)*wCovFourTwoReduced
                          - (3./2.)*(2.*pow(two,2.)-four)*(2.*two*twoReduced-fourReduced)*wCovFourFourReduced 
                          - 4.*two*pow(2.*pow(two,2.)-four,2.)*wCovTwoReducedFourReduced);  
   if(v4PrimeErrorSquared>0.) v4PrimeError = pow(v4PrimeErrorSquared,0.5);        
   fDiffFlow[t][pe][1]->SetBinContent(b,v4Prime);
   fDiffFlow[t][pe][1]->SetBinError(b,v4PrimeError);     
  }
  
 } // end of for(Int_t b=1;b<=fnBinsPtEta[pe];b++)
 
   
 
 
 /*
 // 2D:
 for(Int_t nua=0;nua<2;nua++)
 {
  for(Int_t p=1;p<=fnBinsPt;p++)
  {
   for(Int_t e=1;e<=fnBinsEta;e++) 
   { 
    // differential cumulants:
    Double_t qc2Prime = fFinalCumulants2D[t][pW][eW][nua][0]->GetBinContent(fFinalCumulants2D[t][pW][eW][nua][0]->GetBin(p,e)); // QC{2'}                    
    Double_t qc4Prime = fFinalCumulants2D[t][pW][eW][nua][1]->GetBinContent(fFinalCumulants2D[t][pW][eW][nua][1]->GetBin(p,e)); // QC{4'}
    // differential flow:
    Double_t v2Prime = 0.;                    
    Double_t v4Prime = 0.; 
    if(v2) 
    {
     v2Prime = qc2Prime/v2;
     fFinalFlow2D[t][pW][eW][nua][0]->SetBinContent(fFinalFlow2D[t][pW][eW][nua][0]->GetBin(p,e),v2Prime);  
    }                   
    if(v4)
    {
     v4Prime = -qc4Prime/pow(v4,3.); 
     fFinalFlow2D[t][pW][eW][nua][1]->SetBinContent(fFinalFlow2D[t][pW][eW][nua][1]->GetBin(p,e),v4Prime);  
    }                    
   } // end of for(Int_t e=1;e<=fnBinsEta;e++)
  } // end of for(Int_t p=1;p<=fnBinsPt;p++)
 } // end of for(Int_t nua=0;nua<2;nua++)
 */
 
 

} // end of AliFlowAnalysisWithQCumulants::CalculateDiffFlow(TString type, Bool_t useParticleWeights)



//================================================================================================================================


void AliFlowAnalysisWithQCumulants::StoreIntFlowFlags()
{
 // a) Store all flags for integrated flow in profile fIntFlowFlags.
 
 if(!fIntFlowFlags)
 {
  cout<<"WARNING: fIntFlowFlags is NULL in AFAWQC::SFFIF() !!!!"<<endl;
  exit(0);
 } 

 fIntFlowFlags->Fill(0.5,(Int_t)fUsePhiWeights||fUsePtWeights||fUseEtaWeights); // particle weights used or not
 //fIntFlowFlags->Fill(1.5,""); // which event weight was used? // to be improved
 fIntFlowFlags->Fill(2.5,(Int_t)fApplyCorrectionForNUA); // corrected for non-uniform acceptance or not
  
} // end of void AliFlowAnalysisWithQCumulants::StoreIntFlowFlags()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::StoreDiffFlowFlags()
{
 // Store all flags for differential flow in the profile fDiffFlowFlags.
  
 if(!fDiffFlowFlags)
 {
  cout<<"WARNING: fDiffFlowFlags is NULL in AFAWQC::SFFDF() !!!!"<<endl;
  exit(0);
 } 
 
 fDiffFlowFlags->Fill(0.5,fUsePhiWeights||fUsePtWeights||fUseEtaWeights); // particle weights used or not
 //fDiffFlowFlags->Fill(1.5,""); // which event weight was used? // to be improved
 fDiffFlowFlags->Fill(2.5,fApplyCorrectionForNUA); // corrected for non-uniform acceptance or not
 fDiffFlowFlags->Fill(3.5,fCalculate2DFlow); // calculate also 2D differential flow in (pt,eta) or not
    
} // end of void AliFlowAnalysisWithQCumulants::StoreDiffFlowFlags()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetPointersForCommonHistograms(TList *outputListHistos) 
{
 // Access all pointers to common control and common result histograms and profiles.
 
 if(outputListHistos)  
 {
  TString commonHistsName = "AliFlowCommonHistQC";
  commonHistsName += fAnalysisLabel->Data();
  AliFlowCommonHist *commonHist = dynamic_cast<AliFlowCommonHist*>(outputListHistos->FindObject(commonHistsName.Data()));
  if(commonHist) this->SetCommonHists(commonHist); 
  TString commonHists2ndOrderName = "AliFlowCommonHist2ndOrderQC";
  commonHists2ndOrderName += fAnalysisLabel->Data();
  AliFlowCommonHist *commonHist2nd = dynamic_cast<AliFlowCommonHist*>(outputListHistos->FindObject(commonHists2ndOrderName.Data()));
  if(commonHist2nd) this->SetCommonHists2nd(commonHist2nd);   
  TString commonHists4thOrderName = "AliFlowCommonHist4thOrderQC";
  commonHists4thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHist *commonHist4th = dynamic_cast<AliFlowCommonHist*>(outputListHistos->FindObject(commonHists4thOrderName.Data()));
  if(commonHist4th) this->SetCommonHists4th(commonHist4th);  
  TString commonHists6thOrderName = "AliFlowCommonHist6thOrderQC";
  commonHists6thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHist *commonHist6th = dynamic_cast<AliFlowCommonHist*>(outputListHistos->FindObject(commonHists6thOrderName.Data()));
  if(commonHist6th) this->SetCommonHists6th(commonHist6th);  
  TString commonHists8thOrderName = "AliFlowCommonHist8thOrderQC";
  commonHists8thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHist *commonHist8th = dynamic_cast<AliFlowCommonHist*>(outputListHistos->FindObject(commonHists8thOrderName.Data()));
  if(commonHist8th) this->SetCommonHists8th(commonHist8th);  
  TString commonHistResults2ndOrderName = "AliFlowCommonHistResults2ndOrderQC"; 
  commonHistResults2ndOrderName += fAnalysisLabel->Data(); 
  AliFlowCommonHistResults *commonHistRes2nd = dynamic_cast<AliFlowCommonHistResults*>
                                               (outputListHistos->FindObject(commonHistResults2ndOrderName.Data()));
  if(commonHistRes2nd) this->SetCommonHistsResults2nd(commonHistRes2nd);   
  TString commonHistResults4thOrderName = "AliFlowCommonHistResults4thOrderQC";
  commonHistResults4thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHistResults *commonHistRes4th = dynamic_cast<AliFlowCommonHistResults*>
                                               (outputListHistos->FindObject(commonHistResults4thOrderName.Data()));
  if(commonHistRes4th) this->SetCommonHistsResults4th(commonHistRes4th);  
  TString commonHistResults6thOrderName = "AliFlowCommonHistResults6thOrderQC";
  commonHistResults6thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHistResults *commonHistRes6th = dynamic_cast<AliFlowCommonHistResults*>
                                               (outputListHistos->FindObject(commonHistResults6thOrderName.Data()));
  if(commonHistRes6th) this->SetCommonHistsResults6th(commonHistRes6th);  
  TString commonHistResults8thOrderName = "AliFlowCommonHistResults8thOrderQC";
  commonHistResults8thOrderName += fAnalysisLabel->Data();
  AliFlowCommonHistResults *commonHistRes8th = dynamic_cast<AliFlowCommonHistResults*>
                                               (outputListHistos->FindObject(commonHistResults8thOrderName.Data()));  
  if(commonHistRes8th) this->SetCommonHistsResults8th(commonHistRes8th);
 } else
   {
    cout<<"WARNING: outputListHistos is NULL in AFAWQC::GPFCH() !!!!"<<endl;
    exit(0);
   }
        
} // end of void AliFlowAnalysisWithQCumulants::GetPointersForCommonHistograms(TList *outputListHistos) 


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetPointersForParticleWeightsHistograms(TList *outputListHistos) 
{
 // Get pointers for histograms with particle weights.

 if(outputListHistos)
 {
  TList *weightsList = dynamic_cast<TList*>(outputListHistos->FindObject("Weights"));
  if(weightsList) this->SetWeightsList(weightsList);
  Bool_t bUsePhiWeights = kFALSE;
  Bool_t bUsePtWeights = kFALSE;
  Bool_t bUseEtaWeights = kFALSE;
  TString fUseParticleWeightsName = "fUseParticleWeightsQC"; // to be improved (hirdwired label QC)
  fUseParticleWeightsName += fAnalysisLabel->Data();
  TProfile *useParticleWeights = dynamic_cast<TProfile*>(weightsList->FindObject(fUseParticleWeightsName.Data()));
  if(useParticleWeights)
  {
   this->SetUseParticleWeights(useParticleWeights);  
   bUsePhiWeights = (Int_t)useParticleWeights->GetBinContent(1);
   bUsePtWeights = (Int_t)useParticleWeights->GetBinContent(2);
   bUseEtaWeights = (Int_t)useParticleWeights->GetBinContent(3);
  }
 } else
   {
    cout<<"WARNING: outputListHistos is NULL in AFAWQC::GPFPWH() !!!!"<<endl;
    exit(0);
   }

} // end of void AliFlowAnalysisWithQCumulants::GetPointersForParticleWeightsHistograms(TList *outputListHistos); 


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetPointersForIntFlowHistograms(TList *outputListHistos) 
{
 // Get pointers for histograms and profiles relevant for integrated flow:
 //  a) Get pointer to base list for integrated flow holding profile fIntFlowFlags and lists fIntFlowProfiles and fIntFlowResults.
 //  b) Get pointer to profile fIntFlowFlags holding all flags for integrated flow.
 //  c) Get pointer to list fIntFlowProfiles and pointers to all objects that she holds. 
 //  d) Get pointer to list fIntFlowResults and pointers to all objects that she holds. 
  
 TString sinCosFlag[2] = {"sin","cos"}; // to be improved (should I promote this to data member?)
 TString powerFlag[2] = {"linear","quadratic"}; // to be improved (should I promote this to data member?)
 
 if(outputListHistos)
 {
  // a) Get pointer to base list for integrated flow holding profile fIntFlowFlags and lists fIntFlowProfiles and fIntFlowResults:
  TList *intFlowList = NULL;
  intFlowList = dynamic_cast<TList*>(outputListHistos->FindObject("Integrated Flow"));
  if(!intFlowList) 
  {
   cout<<"WARNING: intFlowList is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
   exit(0); 
  }  
  
  // b) Get pointer to profile fIntFlowFlags holding all flags for integrated flow:
  TString intFlowFlagsName = "fIntFlowFlags";
  intFlowFlagsName += fAnalysisLabel->Data();
  TProfile *intFlowFlags = dynamic_cast<TProfile*>(intFlowList->FindObject(intFlowFlagsName.Data()));
  Bool_t bApplyCorrectionForNUA = kFALSE;
  if(intFlowFlags)
  {
   this->SetIntFlowFlags(intFlowFlags);  
   bApplyCorrectionForNUA = (Int_t)intFlowFlags->GetBinContent(3); 
   this->SetApplyCorrectionForNUA(bApplyCorrectionForNUA);      
  } else 
    {
     cout<<"WARNING: intFlowFlags is NULL in FAWQC::GPFIFH() !!!!"<<endl;
    }
  
  // c) Get pointer to list fIntFlowProfiles and pointers to all objects that she holds:
  TList *intFlowProfiles = NULL;
  intFlowProfiles = dynamic_cast<TList*>(intFlowList->FindObject("Profiles"));
  if(intFlowProfiles)  
  {
   // average multiplicities:
   TString avMultiplicityName = "fAvMultiplicity";
   avMultiplicityName += fAnalysisLabel->Data();
   TProfile *avMultiplicity = dynamic_cast<TProfile*>(intFlowProfiles->FindObject(avMultiplicityName.Data()));
   if(avMultiplicity) 
   {
    this->SetAvMultiplicity(avMultiplicity);
   } else 
     {
      cout<<"WARNING: avMultiplicity is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     }
   // average correlations <<2>>, <<4>>, <<6>> and <<8>> (with wrong errors!):
   TString intFlowCorrelationsProName = "fIntFlowCorrelationsPro";
   intFlowCorrelationsProName += fAnalysisLabel->Data();
   TProfile *intFlowCorrelationsPro = dynamic_cast<TProfile*>(intFlowProfiles->FindObject(intFlowCorrelationsProName.Data()));
   if(intFlowCorrelationsPro) 
   {
    this->SetIntFlowCorrelationsPro(intFlowCorrelationsPro);
   } else 
     {
      cout<<"WARNING: intFlowCorrelationsPro is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     } 
   // average all correlations for integrated flow (with wrong errors!):
   TString intFlowCorrelationsAllProName = "fIntFlowCorrelationsAllPro";
   intFlowCorrelationsAllProName += fAnalysisLabel->Data();
   TProfile *intFlowCorrelationsAllPro = dynamic_cast<TProfile*>(intFlowProfiles->FindObject(intFlowCorrelationsAllProName.Data()));
   if(intFlowCorrelationsAllPro) 
   {
    this->SetIntFlowCorrelationsAllPro(intFlowCorrelationsAllPro);
   } else 
     {
      cout<<"WARNING: intFlowCorrelationsAllPro is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     }     
   // average products of correlations <2>, <4>, <6> and <8>:  
   TString intFlowProductOfCorrelationsProName = "fIntFlowProductOfCorrelationsPro";
   intFlowProductOfCorrelationsProName += fAnalysisLabel->Data();
   TProfile *intFlowProductOfCorrelationsPro = dynamic_cast<TProfile*>(intFlowProfiles->FindObject(intFlowProductOfCorrelationsProName.Data()));
   if(intFlowProductOfCorrelationsPro) 
   {
    this->SetIntFlowProductOfCorrelationsPro(intFlowProductOfCorrelationsPro);
   } else 
     {
      cout<<"WARNING: intFlowProductOfCorrelationsPro is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     }     
   // average correction terms for non-uniform acceptance (with wrong errors!):
   for(Int_t sc=0;sc<2;sc++)
   {
    TString intFlowCorrectionTermsForNUAProName = "fIntFlowCorrectionTermsForNUAPro";
    intFlowCorrectionTermsForNUAProName += fAnalysisLabel->Data();
    TProfile *intFlowCorrectionTermsForNUAPro = dynamic_cast<TProfile*>(intFlowProfiles->FindObject((Form("%s: %s terms",intFlowCorrectionTermsForNUAProName.Data(),sinCosFlag[sc].Data()))));
    if(intFlowCorrectionTermsForNUAPro) 
    {
     this->SetIntFlowCorrectionTermsForNUAPro(intFlowCorrectionTermsForNUAPro,sc);
    } else 
      {
       cout<<"WARNING: intFlowCorrectionTermsForNUAPro is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
       cout<<"sc = "<<sc<<endl;
      } 
   } // end of for(Int_t sc=0;sc<2;sc++)           
  } else // to if(intFlowProfiles)  
    {
     cout<<"WARNING: intFlowProfiles is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
    }
   
  //  d) Get pointer to list fIntFlowResults and pointers to all objects that she holds. 
  TList *intFlowResults = NULL;
  intFlowResults = dynamic_cast<TList*>(intFlowList->FindObject("Results"));
  if(intFlowResults)
  {
   // average correlations <<2>>, <<4>>, <<6>> and <<8>> (with correct errors!):
   TString intFlowCorrelationsHistName = "fIntFlowCorrelationsHist";
   intFlowCorrelationsHistName += fAnalysisLabel->Data();
   TH1D *intFlowCorrelationsHist = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowCorrelationsHistName.Data()));
   if(intFlowCorrelationsHist) 
   {
    this->SetIntFlowCorrelationsHist(intFlowCorrelationsHist);
   } else 
     {
      cout<<"WARNING: intFlowCorrelationsHist is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     } 
   // average all correlations for integrated flow (with correct errors!):
   TString intFlowCorrelationsAllHistName = "fIntFlowCorrelationsAllHist";
   intFlowCorrelationsAllHistName += fAnalysisLabel->Data();
   TH1D *intFlowCorrelationsAllHist = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowCorrelationsAllHistName.Data()));
   if(intFlowCorrelationsAllHist) 
   {
    this->SetIntFlowCorrelationsAllHist(intFlowCorrelationsAllHist);
   } else 
     {
      cout<<"WARNING: intFlowCorrelationsAllHist is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     }  
   // average correction terms for non-uniform acceptance (with correct errors!):
   TString intFlowCorrectionTermsForNUAHistName = "fIntFlowCorrectionTermsForNUAHist";
   intFlowCorrectionTermsForNUAHistName += fAnalysisLabel->Data();
   for(Int_t sc=0;sc<2;sc++)
   {
    TH1D *intFlowCorrectionTermsForNUAHist = dynamic_cast<TH1D*>(intFlowResults->FindObject((Form("%s: %s terms",intFlowCorrectionTermsForNUAHistName.Data(),sinCosFlag[sc].Data()))));
    if(intFlowCorrectionTermsForNUAHist) 
    {
     this->SetIntFlowCorrectionTermsForNUAHist(intFlowCorrectionTermsForNUAHist,sc);
    } else 
      {
       cout<<"WARNING: intFlowCorrectionTermsForNUAHist is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
       cout<<"sc = "<<sc<<endl;
      } 
   } // end of for(Int_t sc=0;sc<2;sc++)           
   // covariances (multiplied with weight dependent prefactor):
   TString intFlowCovariancesName = "fIntFlowCovariances";
   intFlowCovariancesName += fAnalysisLabel->Data();
   TH1D *intFlowCovariances = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowCovariancesName.Data()));
   if(intFlowCovariances) 
   {
    this->SetIntFlowCovariances(intFlowCovariances); 
   } else 
     {
      cout<<"WARNING: intFlowCovariances is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     } 
   // sum of linear and quadratic event weights for <2>, <4>, <6> and <8>:
   TString intFlowSumOfEventWeightsName = "fIntFlowSumOfEventWeights";
   intFlowSumOfEventWeightsName += fAnalysisLabel->Data();
   for(Int_t power=0;power<2;power++)
   {
    TH1D *intFlowSumOfEventWeights = dynamic_cast<TH1D*>(intFlowResults->FindObject(Form("%s: %s",intFlowSumOfEventWeightsName.Data(),powerFlag[power].Data())));
    if(intFlowSumOfEventWeights) 
    {
     this->SetIntFlowSumOfEventWeights(intFlowSumOfEventWeights,power);
    } else 
      {
       cout<<"WARNING: intFlowSumOfEventWeights is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
       cout<<"power = "<<power<<endl;
      }                                   
   } // end of for(Int_t power=0;power<2;power++)                                                                  
   // sum of products of event weights for correlations <2>, <4>, <6> and <8>:  
   TString intFlowSumOfProductOfEventWeightsName = "fIntFlowSumOfProductOfEventWeights";
   intFlowSumOfProductOfEventWeightsName += fAnalysisLabel->Data();
   TH1D *intFlowSumOfProductOfEventWeights = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowSumOfProductOfEventWeightsName.Data()));
   if(intFlowSumOfProductOfEventWeights) 
   {
    this->SetIntFlowSumOfProductOfEventWeights(intFlowSumOfProductOfEventWeights);
   } else 
     {
      cout<<"WARNING: intFlowSumOfProductOfEventWeights is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     } 
   // final results for integrated Q-cumulants:
   TString intFlowQcumulantsName = "fIntFlowQcumulants";
   intFlowQcumulantsName += fAnalysisLabel->Data();
   TH1D *intFlowQcumulants = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowQcumulantsName.Data()));
   if(intFlowQcumulants) 
   {
    this->SetIntFlowQcumulants(intFlowQcumulants);
   } else 
     {
      cout<<"WARNING: intFlowQcumulants is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
     }  
   // final integrated flow estimates from Q-cumulants:
   TString intFlowName = "fIntFlow";
   intFlowName += fAnalysisLabel->Data();
   TH1D *intFlow = dynamic_cast<TH1D*>(intFlowResults->FindObject(intFlowName.Data()));
   if(intFlow) 
   {
    this->SetIntFlow(intFlow);
   } else 
     {
      cout<<"WARNING: intFlow is NULL in AFAWQC::GPFIFH() !!!!"<<endl; 
     }   
  } else // to if(intFlowResults)
    {
     cout<<"WARNING: intFlowResults is NULL in AFAWQC::GPFIFH() !!!!"<<endl;
    }
 } // end of if(outputListHistos)

} // end of void AliFlowAnalysisWithQCumulants::GetPointersForIntFlowHistograms(TList *outputListHistos)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetPointersForDiffFlowHistograms(TList *outputListHistos)
{
 // Get pointer to all objects relevant for differential flow.
 //  a) Define flags locally (to be improved: should I promote flags to data members?);
 //  b) Get pointer to base list for differential flow fDiffFlowList and nested lists fDiffFlowListProfiles and fDiffFlowListResults;
 //  c) Get pointer to profile fDiffFlowFlags holding all flags for differential flow;
 //  d) Get pointers to all nested lists in fDiffFlowListProfiles and to profiles which they hold;
 //  e) Get pointers to all nested lists in fDiffFlowListResults and to histograms which they hold.
 
 // a) Define flags locally (to be improved: should I promote flags to data members?): 
 TString typeFlag[2] = {"RP","POI"}; 
 TString ptEtaFlag[2] = {"p_{T}","#eta"};
 TString powerFlag[2] = {"linear","quadratic"};
 TString differentialCumulantIndex[4] = {"QC{2'}","QC{4'}","QC{6'}","QC{8'}"};  
 TString differentialFlowIndex[4] = {"v'{2}","v'{4}","v'{6}","v'{8}"};  
 TString reducedCorrelationIndex[4] = {"<2'>","<4'>","<6'>","<8'>"};
 TString mixedCorrelationIndex[8] = {"<2>","<2'>","<4>","<4'>","<6>","<6'>","<8>","<8'>"};
 TString covarianceName[5] = {"Cov(<2>,<2'>)","Cov(<2>,<4'>)","Cov(<4>,<2'>)","Cov(<4>,<4'>)","Cov(<2'>,<4'>)"}; 
  
 // b) Get pointer to base list for differential flow fDiffFlowList and nested lists fDiffFlowListProfiles and fDiffFlowListResults:
 TList *diffFlowList = NULL;
 diffFlowList = dynamic_cast<TList*>(outputListHistos->FindObject("Differential Flow"));  
 if(!diffFlowList)
 { 
  cout<<"WARNING: diffFlowList is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
  exit(0);
 }
 // list holding nested lists containing profiles:
 TList *diffFlowListProfiles = NULL;
 diffFlowListProfiles = dynamic_cast<TList*>(diffFlowList->FindObject("Profiles"));
 if(!diffFlowListProfiles)
 { 
  cout<<"WARNING: diffFlowListProfiles is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
  exit(0);
 }
 // list holding nested lists containing 2D and 1D histograms with final results:
 TList *diffFlowListResults = NULL;
 diffFlowListResults = dynamic_cast<TList*>(diffFlowList->FindObject("Results"));
 if(!diffFlowListResults)
 { 
  cout<<"WARNING: diffFlowListResults is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
  exit(0);
 }
 
 // c) Get pointer to profile holding all flags for differential flow;
 TString diffFlowFlagsName = "fDiffFlowFlags";
 diffFlowFlagsName += fAnalysisLabel->Data();
 TProfile *diffFlowFlags = dynamic_cast<TProfile*>(diffFlowList->FindObject(diffFlowFlagsName.Data()));
 Bool_t bCalculate2DFlow = kFALSE;
 if(diffFlowFlags)
 {
  this->SetDiffFlowFlags(diffFlowFlags);  
  bCalculate2DFlow = (Int_t)diffFlowFlags->GetBinContent(4);
  this->SetCalculate2DFlow(bCalculate2DFlow); // to be improved (shoul I call this setter somewhere else?)     
 }
  
 // d) Get pointers to all nested lists in fDiffFlowListProfiles and to profiles which they hold;
 // correlations:
 TList *diffFlowCorrelationsProList[2][2] = {{NULL}};
 TString diffFlowCorrelationsProName = "fDiffFlowCorrelationsPro";
 diffFlowCorrelationsProName += fAnalysisLabel->Data();
 TProfile *diffFlowCorrelationsPro[2][2][4] = {{{NULL}}};   
 // products of correlations:
 TList *diffFlowProductOfCorrelationsProList[2][2] = {{NULL}};
 TString diffFlowProductOfCorrelationsProName = "fDiffFlowProductOfCorrelationsPro";
 diffFlowProductOfCorrelationsProName += fAnalysisLabel->Data();  
 TProfile *diffFlowProductOfCorrelationsPro[2][2][8][8] = {{{{NULL}}}};   
 // corrections:
 TList *diffFlowCorrectionsProList[2][2] = {{NULL}};
 // ...
 for(Int_t t=0;t<2;t++)
 {
  for(Int_t pe=0;pe<2;pe++)
  {
   diffFlowCorrelationsProList[t][pe] = dynamic_cast<TList*>(diffFlowListProfiles->FindObject(Form("Profiles with correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowCorrelationsProList[t][pe])
   { 
    cout<<"WARNING: diffFlowCorrelationsProList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t ci=0;ci<4;ci++) // correlation index
   {
    diffFlowCorrelationsPro[t][pe][ci] = dynamic_cast<TProfile*>(diffFlowCorrelationsProList[t][pe]->FindObject(Form("%s, %s, %s, %s",diffFlowCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[ci].Data())));
    if(diffFlowCorrelationsPro[t][pe][ci])
    {
     this->SetDiffFlowCorrelationsPro(diffFlowCorrelationsPro[t][pe][ci],t,pe,ci);
    } else
      {
       cout<<"WARNING: diffFlowCorrelationsPro[t][pe][ci] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
       cout<<"t  = "<<t<<endl;
       cout<<"pe = "<<pe<<endl;   
       cout<<"ci = "<<ci<<endl;
      }     
   } // end of for(Int_t ci=0;ci<4;ci++) // correlation index  
   // products of correlations:    
   diffFlowProductOfCorrelationsProList[t][pe] = dynamic_cast<TList*>(diffFlowListProfiles->FindObject(Form("Profiles with products of correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data()))); 
   if(!diffFlowProductOfCorrelationsProList[t][pe])
   { 
    cout<<"WARNING: ddiffFlowProductOfCorrelationsProList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
    {
     diffFlowProductOfCorrelationsPro[t][pe][mci1][mci2] = dynamic_cast<TProfile*>(diffFlowProductOfCorrelationsProList[t][pe]->FindObject(Form("%s, %s, %s, %s, %s",diffFlowProductOfCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data())));
     if(diffFlowProductOfCorrelationsPro[t][pe][mci1][mci2])
     {
      this->SetDiffFlowProductOfCorrelationsPro(diffFlowProductOfCorrelationsPro[t][pe][mci1][mci2],t,pe,mci1,mci2);
     } else
       {
        cout<<"WARNING: diffFlowCorrelationsPro[t][pe][ci] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
        cout<<"t    = "<<t<<endl;
        cout<<"pe   = "<<pe<<endl;   
        cout<<"mci1 = "<<mci1<<endl;
        cout<<"mci2 = "<<mci2<<endl;
       }
     if(mci1%2 == 0) mci2++; // products which DO NOT include reduced correlations are not stored here
    } // end of for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
   } // end of for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index    
   // corrections:
   diffFlowCorrectionsProList[t][pe] = dynamic_cast<TList*>(diffFlowListProfiles->FindObject(Form("Profiles with correction terms for NUA (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowCorrectionsProList[t][pe])
   { 
    cout<<"WARNING: diffFlowCorrectionsProList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   // ...      
  } // end of for(Int_t pe=0;pe<2;pe++)
 } // end of for(Int_t t=0;t<2;t++)
  
 // e) Get pointers to all nested lists in fDiffFlowListResults and to histograms which they hold.
 // reduced correlations:
 TList *diffFlowCorrelationsHistList[2][2] = {{NULL}};
 TString diffFlowCorrelationsHistName = "fDiffFlowCorrelationsHist";
 diffFlowCorrelationsHistName += fAnalysisLabel->Data();  
 TH1D *diffFlowCorrelationsHist[2][2][4] = {{{NULL}}};
 // differential Q-cumulants:
 TList *diffFlowCumulantsHistList[2][2] = {{NULL}};
 TString diffFlowCumulantsName = "fDiffFlowCumulants";
 diffFlowCumulantsName += fAnalysisLabel->Data();  
 TH1D *diffFlowCumulants[2][2][4] = {{{NULL}}};
 // differential flow estimates from Q-cumulants:
 TList *diffFlowHistList[2][2] = {{NULL}};
 TString diffFlowName = "fDiffFlow";
 diffFlowName += fAnalysisLabel->Data();  
 TH1D *diffFlow[2][2][4] = {{{NULL}}};
 // differential covariances:
 TList *diffFlowCovariancesHistList[2][2] = {{NULL}};
 TString diffFlowCovariancesName = "fDiffFlowCovariances";
 diffFlowCovariancesName += fAnalysisLabel->Data();  
 TH1D *diffFlowCovariances[2][2][5] = {{{NULL}}};
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   // reduced correlations:
   diffFlowCorrelationsHistList[t][pe] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowCorrelationsHistList[t][pe])
   { 
    cout<<"WARNING: diffFlowCorrelationsHistList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t index=0;index<4;index++) 
   {
    diffFlowCorrelationsHist[t][pe][index] = dynamic_cast<TH1D*>(diffFlowCorrelationsHistList[t][pe]->FindObject(Form("%s, %s, %s, %s",diffFlowCorrelationsHistName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[index].Data())));
    if(diffFlowCorrelationsHist[t][pe][index])
    {
     this->SetDiffFlowCorrelationsHist(diffFlowCorrelationsHist[t][pe][index],t,pe,index);
    } else 
      {
       cout<<"WARNING: diffFlowCorrelationsHist[t][pe][index] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
       cout<<"t     = "<<t<<endl;
       cout<<"pe    = "<<pe<<endl;
       cout<<"index = "<<index<<endl;
       exit(0);       
      } 
   } // end of for(Int_t index=0;index<4;index++)
   // differential Q-cumulants:
   diffFlowCumulantsHistList[t][pe] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Differential Q-cumulants (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowCumulantsHistList[t][pe])
   { 
    cout<<"WARNING: diffFlowCumulantsHistList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t  = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t index=0;index<4;index++) 
   {
    diffFlowCumulants[t][pe][index] = dynamic_cast<TH1D*>(diffFlowCumulantsHistList[t][pe]->FindObject(Form("%s, %s, %s, %s",diffFlowCumulantsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialCumulantIndex[index].Data())));
    if(diffFlowCumulants[t][pe][index])
    {
     this->SetDiffFlowCumulants(diffFlowCumulants[t][pe][index],t,pe,index);
    } else 
      {
       cout<<"WARNING: diffFlowCumulants[t][pe][index] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
       cout<<"t     = "<<t<<endl;
       cout<<"pe    = "<<pe<<endl;
       cout<<"index = "<<index<<endl;
       exit(0);       
      } 
   } // end of for(Int_t index=0;index<4;index++)
   // differential flow estimates from Q-cumulants:
   diffFlowHistList[t][pe] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Differential flow (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowHistList[t][pe])
   { 
    cout<<"WARNING: diffFlowHistList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t  = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t index=0;index<4;index++) 
   {
    diffFlow[t][pe][index] = dynamic_cast<TH1D*>(diffFlowHistList[t][pe]->FindObject(Form("%s, %s, %s, %s",diffFlowName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialFlowIndex[index].Data())));
    if(diffFlow[t][pe][index])
    {
     this->SetDiffFlow(diffFlow[t][pe][index],t,pe,index);
    } else 
      {
       cout<<"WARNING: diffFlow[t][pe][index] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
       cout<<"t     = "<<t<<endl;
       cout<<"pe    = "<<pe<<endl;
       cout<<"index = "<<index<<endl;
       exit(0);       
      } 
   } // end of for(Int_t index=0;index<4;index++)
   // differential covariances:
   diffFlowCovariancesHistList[t][pe] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Covariances of correlations (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowCovariancesHistList[t][pe])
   { 
    cout<<"WARNING: diffFlowCovariancesHistList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t  = "<<t<<endl;
    cout<<"pe = "<<pe<<endl;
    exit(0);
   }
   for(Int_t covIndex=0;covIndex<5;covIndex++) 
   {
    diffFlowCovariances[t][pe][covIndex] = dynamic_cast<TH1D*>(diffFlowCovariancesHistList[t][pe]->FindObject(Form("%s, %s, %s, %s",diffFlowCovariancesName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),covarianceName[covIndex].Data())));
    if(diffFlowCovariances[t][pe][covIndex])
    {
     this->SetDiffFlowCovariances(diffFlowCovariances[t][pe][covIndex],t,pe,covIndex);
    } else 
      {
       cout<<"WARNING: diffFlowCovariances[t][pe][covIndex] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
       cout<<"t        = "<<t<<endl;
       cout<<"pe       = "<<pe<<endl;
       cout<<"covIndex = "<<covIndex<<endl;
       exit(0);       
      } 
   } // end of for(Int_t covIndex=0;covIndex<5;covIndex++) // covariance index    
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI 
 // sum of event weights for reduced correlations:
 TList *diffFlowSumOfEventWeightsHistList[2][2][2] = {{{NULL}}};
 TString diffFlowSumOfEventWeightsName = "fDiffFlowSumOfEventWeights";
 diffFlowSumOfEventWeightsName += fAnalysisLabel->Data();  
 TH1D *diffFlowSumOfEventWeights[2][2][2][4] = {{{{NULL}}}};
 for(Int_t t=0;t<2;t++) // type is RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  { 
   for(Int_t p=0;p<2;p++) // power of event weights is either 1 or 2
   {
    diffFlowSumOfEventWeightsHistList[t][pe][p] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Sum of %s event weights (%s, %s)",powerFlag[p].Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data())));
    if(!diffFlowSumOfEventWeightsHistList[t][pe][p])
    { 
     cout<<"WARNING: diffFlowSumOfEventWeightsHistList[t][pe][p] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
     cout<<"t     = "<<t<<endl;
     cout<<"pe    = "<<pe<<endl;
     cout<<"power = "<<p<<endl;
     exit(0);
    }
    for(Int_t ew=0;ew<4;ew++) // index of reduced correlation
    {
     diffFlowSumOfEventWeights[t][pe][p][ew] = dynamic_cast<TH1D*>(diffFlowSumOfEventWeightsHistList[t][pe][p]->FindObject(Form("%s, %s, %s, %s, %s",diffFlowSumOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),powerFlag[p].Data(),reducedCorrelationIndex[ew].Data())));    
     if(diffFlowSumOfEventWeights[t][pe][p][ew])
     {
      this->SetDiffFlowSumOfEventWeights(diffFlowSumOfEventWeights[t][pe][p][ew],t,pe,p,ew);
     } else 
       {
        cout<<"WARNING: diffFlowSumOfEventWeights[t][pe][p][ew] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
        cout<<"t     = "<<t<<endl;
        cout<<"pe    = "<<pe<<endl;
        cout<<"power = "<<p<<endl;
        cout<<"ew    = "<<ew<<endl;
        exit(0);       
       } 
    }
   } // end of for(Int_t p=0;p<2;p++) // power of event weights is either 1 or 2
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type is RP or POI
 //  
 TList *diffFlowSumOfProductOfEventWeightsHistList[2][2] = {{NULL}};
 TString diffFlowSumOfProductOfEventWeightsName = "fDiffFlowSumOfProductOfEventWeights";
 diffFlowSumOfProductOfEventWeightsName += fAnalysisLabel->Data();  
 TH1D *diffFlowSumOfProductOfEventWeights[2][2][8][8] = {{{{NULL}}}};
 for(Int_t t=0;t<2;t++) // type is RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  { 
   diffFlowSumOfProductOfEventWeightsHistList[t][pe] = dynamic_cast<TList*>(diffFlowListResults->FindObject(Form("Sum of products of event weights (%s, %s)",typeFlag[t].Data(),ptEtaFlag[pe].Data())));
   if(!diffFlowSumOfProductOfEventWeightsHistList[t][pe])
   { 
    cout<<"WARNING: diffFlowSumOfProductOfEventWeightsHistList[t][pe] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
    cout<<"t     = "<<t<<endl;
    cout<<"pe    = "<<pe<<endl;
    exit(0);
   }
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
    {
     diffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2] = dynamic_cast<TH1D*>(diffFlowSumOfProductOfEventWeightsHistList[t][pe]->FindObject(Form("%s, %s, %s, %s, %s",diffFlowSumOfProductOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data())));    
      if(diffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2])
      {
       this->SetDiffFlowSumOfProductOfEventWeights(diffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2],t,pe,mci1,mci2);
      } else 
        {
         cout<<"WARNING: diffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2] is NULL in AFAWQC::GPFDFH() !!!!"<<endl;
         cout<<"t    = "<<t<<endl;
         cout<<"pe   = "<<pe<<endl;
         cout<<"mci1 = "<<mci1<<endl;
         cout<<"mci2 = "<<mci2<<endl;
         exit(0);       
        } 
     if(mci1%2 == 0) mci2++; // products which DO NOT include reduced correlations are not stored here
    } // end of for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
   } // end of for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta
 } // end of for(Int_t t=0;t<2;t++) // type is RP or POI

} // end void AliFlowAnalysisWithQCumulants::GetPointersForDiffFlowHistograms(TList *outputListHistos)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::BookEverythingForDifferentialFlow()
{
 // Book all histograms and profiles needed for differential flow.
 //  a) Define flags locally (to be improved: should I promote flags to data members?);
 //  b) Book profile to hold all flags for differential flow;
 //  c) Book e-b-e quantities;
 //  d) Book profiles;
 //  e) Book histograms holding final results. 
 
 // a) Define flags locally (to be improved: should I promote flags to data members?): 
 TString typeFlag[2] = {"RP","POI"}; 
 TString ptEtaFlag[2] = {"p_{T}","#eta"};
 TString powerFlag[2] = {"linear","quadratic"};
 TString differentialCumulantIndex[4] = {"QC{2'}","QC{4'}","QC{6'}","QC{8'}"};  
 TString differentialFlowIndex[4] = {"v'{2}","v'{4}","v'{6}","v'{8}"};  
 TString reducedCorrelationIndex[4] = {"<2'>","<4'>","<6'>","<8'>"};
 TString mixedCorrelationIndex[8] = {"<2>","<2'>","<4>","<4'>","<6>","<6'>","<8>","<8'>"};
 TString covarianceName[5] = {"Cov(<2>,<2'>)","Cov(<2>,<4'>)","Cov(<4>,<2'>)","Cov(<4>,<4'>)","Cov(<2'>,<4'>)"}; 
 Int_t nBinsPtEta[2] = {fnBinsPt,fnBinsEta};
 Double_t minPtEta[2] = {fPtMin,fEtaMin};
 Double_t maxPtEta[2] = {fPtMax,fEtaMax};
  
 // b) Book profile to hold all flags for differential flow:
 TString diffFlowFlagsName = "fDiffFlowFlags";
 diffFlowFlagsName += fAnalysisLabel->Data();
 fDiffFlowFlags = new TProfile(diffFlowFlagsName.Data(),"Flags for Differential Flow",4,0,4);
 fDiffFlowFlags->SetTickLength(-0.01,"Y");
 fDiffFlowFlags->SetMarkerStyle(25);
 fDiffFlowFlags->SetLabelSize(0.05);
 fDiffFlowFlags->SetLabelOffset(0.02,"Y");
 (fDiffFlowFlags->GetXaxis())->SetBinLabel(1,"Particle Weights");
 (fDiffFlowFlags->GetXaxis())->SetBinLabel(2,"Event Weights");
 (fDiffFlowFlags->GetXaxis())->SetBinLabel(3,"Corrected for NUA?");
 (fDiffFlowFlags->GetXaxis())->SetBinLabel(4,"Calculated 2D flow?");
 fDiffFlowList->Add(fDiffFlowFlags);

 // c) Book e-b-e quantities:
 // Event-by-event r_{m*n,k}(pt,eta), p_{m*n,k}(pt,eta) and q_{m*n,k}(pt,eta)
 // Explanantion of notation:
 //  1.) n is harmonic, m is multiple of harmonic;
 //  2.) k is power of particle weight;
 //  3.) r_{m*n,k}(pt,eta) = Q-vector evaluated in harmonic m*n for RPs in particular (pt,eta) bin (i-th RP is weighted with w_i^k);   
 //  4.) p_{m*n,k}(pt,eta) = Q-vector evaluated in harmonic m*n for POIs in particular (pt,eta) bin 
 //                          (if i-th POI is also RP, than it is weighted with w_i^k);   
 //  5.) q_{m*n,k}(pt,eta) = Q-vector evaluated in harmonic m*n for particles which are both RPs and POIs in particular (pt,eta) bin 
 //                          (i-th RP&&POI is weighted with w_i^k)            
  
 // 1D:
 for(Int_t t=0;t<3;t++) // typeFlag (0 = RP, 1 = POI, 2 = RP && POI )
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t m=0;m<4;m++) // multiple of harmonic
   {
    for(Int_t k=0;k<9;k++) // power of particle weight
    {
     fReRPQ1dEBE[t][pe][m][k] = new TProfile(Form("TypeFlag%dpteta%dmultiple%dpower%dRe",t,pe,m,k),
                                             Form("TypeFlag%dpteta%dmultiple%dpower%dRe",t,pe,m,k),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
     fImRPQ1dEBE[t][pe][m][k] = new TProfile(Form("TypeFlag%dpteta%dmultiple%dpower%dIm",t,pe,m,k),
                                             Form("TypeFlag%dpteta%dmultiple%dpower%dIm",t,pe,m,k),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
    }
   }
  }
 } 
 // to be improved (add explanation of fs1dEBE[t][pe][k]):   
 for(Int_t t=0;t<3;t++) // typeFlag (0 = RP, 1 = POI, 2 = RP&&POI )
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t k=0;k<9;k++) // power of particle weight
   {
    fs1dEBE[t][pe][k] = new TProfile(Form("TypeFlag%dpteta%dmultiple%d",t,pe,k),
                                     Form("TypeFlag%dpteta%dmultiple%d",t,pe,k),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
   }
  }
 }    
 // 2D:
 TProfile2D styleRe("typeMultiplePowerRe","typeMultiplePowerRe",fnBinsPt,fPtMin,fPtMax,fnBinsEta,fEtaMin,fEtaMax);
 TProfile2D styleIm("typeMultiplePowerIm","typeMultiplePowerIm",fnBinsPt,fPtMin,fPtMax,fnBinsEta,fEtaMin,fEtaMax);
 for(Int_t t=0;t<3;t++) // typeFlag (0 = RP, 1 = POI, 2 = RP&&POI )
 { 
  for(Int_t m=0;m<4;m++)
  {
   for(Int_t k=0;k<9;k++)
   {
    fReRPQ2dEBE[t][m][k] = (TProfile2D*)styleRe.Clone(Form("typeFlag%dmultiple%dpower%dRe",t,m,k)); 
    fImRPQ2dEBE[t][m][k] = (TProfile2D*)styleIm.Clone(Form("typeFlag%dmultiple%dpower%dIm",t,m,k));
   }
  } 
 } 
 TProfile2D styleS("typePower","typePower",fnBinsPt,fPtMin,fPtMax,fnBinsEta,fEtaMin,fEtaMax);
 for(Int_t t=0;t<3;t++) // typeFlag (0 = RP, 1 = POI, 2 = RP&&POI )
 { 
  for(Int_t k=0;k<9;k++)
  {
   fs2dEBE[t][k] = (TProfile2D*)styleS.Clone(Form("typeFlag%dpower%d",t,k));
  }
 }
 // reduced correlations e-b-e:
 TString diffFlowCorrelationsEBEName = "fDiffFlowCorrelationsEBE";
 diffFlowCorrelationsEBEName += fAnalysisLabel->Data();
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t rci=0;rci<4;rci++) // reduced correlation index
   {
    fDiffFlowCorrelationsEBE[t][pe][rci] = new TH1D(Form("%s, %s, %s, %s",diffFlowCorrelationsEBEName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[rci].Data()),Form("%s, %s, %s, %s",diffFlowCorrelationsEBEName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[rci].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]);
   } // end of for(Int_t ci=0;ci<4;ci++) // correlation index
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta 
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI
      
 // d) Book profiles;
 // reduced correlations:
 TString diffFlowCorrelationsProName = "fDiffFlowCorrelationsPro";
 diffFlowCorrelationsProName += fAnalysisLabel->Data();
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t rci=0;rci<4;rci++) // reduced correlation index
   {
    // reduced correlations:
    fDiffFlowCorrelationsPro[t][pe][rci] = new TProfile(Form("%s, %s, %s, %s",diffFlowCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[rci].Data()),Form("%s, %s, %s, %s",diffFlowCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[rci].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe],"s");
    fDiffFlowCorrelationsPro[t][pe][rci]->SetXTitle(ptEtaFlag[pe].Data());
    fDiffFlowCorrelationsProList[t][pe]->Add(fDiffFlowCorrelationsPro[t][pe][rci]); // to be improved (add dedicated list to hold reduced correlations)
   } // end of for(Int_t rci=0;rci<4;rci++) // correlation index
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta 
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI
 
 // e) Book histograms holding final results. 
 // reduced correlations:
 TString diffFlowCorrelationsHistName = "fDiffFlowCorrelationsHist";
 diffFlowCorrelationsHistName += fAnalysisLabel->Data();
 // differential covariances:
 TString diffFlowCovariancesName = "fDiffFlowCovariances";
 diffFlowCovariancesName += fAnalysisLabel->Data();
 // differential Q-cumulants:
 TString diffFlowCumulantsName = "fDiffFlowCumulants";
 diffFlowCumulantsName += fAnalysisLabel->Data();
 // differential flow:
 TString diffFlowName = "fDiffFlow";
 diffFlowName += fAnalysisLabel->Data();
 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  {
   for(Int_t index=0;index<4;index++) 
   {
    // reduced correlations:
    fDiffFlowCorrelationsHist[t][pe][index] = new TH1D(Form("%s, %s, %s, %s",diffFlowCorrelationsHistName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[index].Data()),Form("%s, %s, %s, %s",diffFlowCorrelationsHistName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),reducedCorrelationIndex[index].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]);
    fDiffFlowCorrelationsHist[t][pe][index]->SetXTitle(ptEtaFlag[pe].Data());
    fDiffFlowCorrelationsHistList[t][pe]->Add(fDiffFlowCorrelationsHist[t][pe][index]); 
    // differential Q-cumulants:
    fDiffFlowCumulants[t][pe][index] = new TH1D(Form("%s, %s, %s, %s",diffFlowCumulantsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialCumulantIndex[index].Data()),Form("%s, %s, %s, %s",diffFlowCumulantsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialCumulantIndex[index].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]);
    fDiffFlowCumulants[t][pe][index]->SetXTitle(ptEtaFlag[pe].Data());
    fDiffFlowCumulantsHistList[t][pe]->Add(fDiffFlowCumulants[t][pe][index]); 
    // differential flow estimates from Q-cumulants:
    fDiffFlow[t][pe][index] = new TH1D(Form("%s, %s, %s, %s",diffFlowName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialFlowIndex[index].Data()),Form("%s, %s, %s, %s",diffFlowName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),differentialFlowIndex[index].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]);
    fDiffFlow[t][pe][index]->SetXTitle(ptEtaFlag[pe].Data());
    fDiffFlowHistList[t][pe]->Add(fDiffFlow[t][pe][index]); 
   } // end of for(Int_t index=0;index<4;index++) 
   for(Int_t covIndex=0;covIndex<5;covIndex++) // covariance index 
   {
    // differential covariances:
    fDiffFlowCovariances[t][pe][covIndex] = new TH1D(Form("%s, %s, %s, %s",diffFlowCovariancesName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),covarianceName[covIndex].Data()),Form("%s, %s, %s, %s",diffFlowCovariancesName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),covarianceName[covIndex].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]);
    fDiffFlowCovariances[t][pe][covIndex]->SetXTitle(ptEtaFlag[pe].Data());
    fDiffFlowCovariancesHistList[t][pe]->Add(fDiffFlowCovariances[t][pe][covIndex]); 
   } // end of for(Int_t covIndex=0;covIndex<5;covIndex++) // covariance index
   // products of both types of correlations: 
   TString diffFlowProductOfCorrelationsProName = "fDiffFlowProductOfCorrelationsPro";
   diffFlowProductOfCorrelationsProName += fAnalysisLabel->Data();  
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
    {
     fDiffFlowProductOfCorrelationsPro[t][pe][mci1][mci2] = new TProfile(Form("%s, %s, %s, %s, %s",diffFlowProductOfCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data()),Form("%s, %s, %s, %s #times %s",diffFlowProductOfCorrelationsProName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
     fDiffFlowProductOfCorrelationsPro[t][pe][mci1][mci2]->SetXTitle(ptEtaFlag[pe].Data());
     fDiffFlowProductOfCorrelationsProList[t][pe]->Add(fDiffFlowProductOfCorrelationsPro[t][pe][mci1][mci2]); 
     if(mci1%2 == 0) mci2++; // products which DO NOT include reduced correlations are not stored here
    } // end of for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
   } // end of for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index    
  } // end of for(Int_t pe=0;pe<2;pe++) // pt or eta 
 } // end of for(Int_t t=0;t<2;t++) // type: RP or POI
 // sums of event weights for reduced correlations: 
 TString diffFlowSumOfEventWeightsName = "fDiffFlowSumOfEventWeights";
 diffFlowSumOfEventWeightsName += fAnalysisLabel->Data();  
 for(Int_t t=0;t<2;t++) // type is RP or POI
 { 
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  { 
   for(Int_t p=0;p<2;p++) // power of weights is either 1 or 2
   {
    for(Int_t ew=0;ew<4;ew++) // index of reduced correlation
    {
     fDiffFlowSumOfEventWeights[t][pe][p][ew] = new TH1D(Form("%s, %s, %s, %s, %s",diffFlowSumOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),powerFlag[p].Data(),reducedCorrelationIndex[ew].Data()),Form("%s, %s, %s, power = %s, %s",diffFlowSumOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),powerFlag[p].Data(),reducedCorrelationIndex[ew].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
     fDiffFlowSumOfEventWeights[t][pe][p][ew]->SetXTitle(ptEtaFlag[pe].Data());
     fDiffFlowSumOfEventWeightsHistList[t][pe][p]->Add(fDiffFlowSumOfEventWeights[t][pe][p][ew]); // to be improved (add dedicated list to hold all this)
    }
   }
  }
 } 
 // sum of products of event weights for both types of correlations: 
 TString diffFlowSumOfProductOfEventWeightsName = "fDiffFlowSumOfProductOfEventWeights";
 diffFlowSumOfProductOfEventWeightsName += fAnalysisLabel->Data();  
 for(Int_t t=0;t<2;t++) // type is RP or POI
 {
  for(Int_t pe=0;pe<2;pe++) // pt or eta
  { 
   for(Int_t mci1=0;mci1<8;mci1++) // mixed correlation index
   {
    for(Int_t mci2=mci1+1;mci2<8;mci2++) // mixed correlation index
    {
     fDiffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2] = new TH1D(Form("%s, %s, %s, %s, %s",diffFlowSumOfProductOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data()),Form("%s, %s, %s, %s #times %s",diffFlowSumOfProductOfEventWeightsName.Data(),typeFlag[t].Data(),ptEtaFlag[pe].Data(),mixedCorrelationIndex[mci1].Data(),mixedCorrelationIndex[mci2].Data()),nBinsPtEta[pe],minPtEta[pe],maxPtEta[pe]); 
     fDiffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2]->SetXTitle(ptEtaFlag[pe].Data());
     fDiffFlowSumOfProductOfEventWeightsHistList[t][pe]->Add(fDiffFlowSumOfProductOfEventWeights[t][pe][mci1][mci2]); 
     if(mci1%2 == 0) mci2++; // products which DO NOT include reduced correlations are not stored here
    }
   }
  }
 } 
  
  
  
 /*
 
  

  
 TString pWeightsFlag[2] = {"not used","used"};  // to be improved (do I need this here?)
 TString eWeightsFlag[2] = {"exact","non-exact"};  // to be improved (do I need this here?)
 //TString sinCosFlag[2] = {"sin","cos"}; // to be improved (do I need this here?)
 TString nuaFlag[2] = {"not corrected","corrected"}; // nua = non-uniform acceptance // to be improved (do I need this here?)
 
 // book all 1D profiles:
 // pt:
 TProfile styleProfilePt("","",fnBinsPt,fPtMin,fPtMax);
 styleProfilePt.SetXTitle("p_{t}");
 // eta:
 TProfile styleProfileEta("","",fnBinsEta,fEtaMin,fEtaMax);
 styleProfileEta.SetXTitle("#eta");


 
 // book all 2D profiles and add to appropriate lists:
 TProfile2D styleProfile("","",fnBinsPt,fPtMin,fPtMax,fnBinsEta,fEtaMin,fEtaMax);
 styleProfile.SetXTitle("p_{t}");
 styleProfile.SetYTitle("#eta");
 TString correlationName[4] = {"<2'>","<4'>","<6'>","<8'>"};
 TString cumulantName[4] = {"QC{2'}","QC{4'}","QC{6'}","QC{8'}"};
 TString productOfCorrelationsName[5] = {"<2><2'>","<2><4'>","<4><2'>","<4><4'>","<2'><4'>"};
 TString correctionsSinTermsName[2] = {"sin(1)","sin(2)"};
 TString correctionsCosTermsName[2] = {"cos(1)","cos(2)"};
 TString correctionName[4] = {"corr. to QC{2'}","corr. to QC{4'}","corr. to QC{6'}","corr. to QC{8'}"};
 TString covarianceName[5] = {"Cov(<2>,<2'>)","Cov(<2>,<4'>)","Cov(<4>,<2'>)","Cov(<4>,<4'>)","Cov(<2'>,<4'>)"}; 
 TString flowName[4] = {"v'{2}","v'{4}","v'{6}","v'{8}"}; 
 
 // profile to hold flags for diffrential flow:
 TString fFlagsForDiffFlowName = "fFlagsForDiffFlowQC";
 fFlagsForDiffFlowName += fAnalysisLabel->Data();
 fFlagsForDiffFlow = new TProfile(fFlagsForDiffFlowName.Data(),"Flags for differential flow",10,0,10);
 fFlagsForDiffFlow->SetLabelSize(0.06);
 (fFlagsForDiffFlow->GetXaxis())->SetBinLabel(1,"2D Flow");
 fFlagsForDiffFlow->Fill(0.5,(Int_t)fCalculate2DFlow);
 fDiffFlowList->Add(fFlagsForDiffFlow); 

 for(Int_t t=0;t<2;t++) // type: RP or POI
 { 
  for(Int_t pW=0;pW<1+(Int_t)(fUsePhiWeights||fUsePtWeights||fUseEtaWeights);pW++) // weights: not used or used 
  {
   for(Int_t eW=0;eW<2;eW++)
   {
    // correlations:
    for(Int_t correlIndex=0;correlIndex<4;correlIndex++)
    {
     fCorrelationsPro[t][pW][eW][correlIndex] = (TProfile2D*)styleProfile.Clone(correlationName[correlIndex].Data());
     fCorrelationsPro[t][pW][eW][correlIndex]->SetTitle(correlationName[correlIndex].Data()); 
     fDiffFlowCorrelations[t][pW][eW]->Add(fCorrelationsPro[t][pW][eW][correlIndex]);
    }
    // products of correlations:
    for(Int_t productOfCorrelationsIndex=0;productOfCorrelationsIndex<5;productOfCorrelationsIndex++)
    {
     fProductsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex] = (TProfile2D*)styleProfile.Clone(productOfCorrelationsName[productOfCorrelationsIndex].Data());
     fProductsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex]->SetTitle(productOfCorrelationsName[productOfCorrelationsIndex].Data()); 
     fDiffFlowProductsOfCorrelations[t][pW][eW]->Add(fProductsOfCorrelationsPro[t][pW][eW][productOfCorrelationsIndex]);
    }
    // correction terms:
    for(Int_t sc=0;sc<2;sc++)
    {
     for(Int_t correctionsIndex=0;correctionsIndex<2;correctionsIndex++)
     {
      if(sc==0)
      {
       fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex] = (TProfile2D*)styleProfile.Clone(correctionsSinTermsName[correctionsIndex].Data());
       fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex]->SetTitle(correctionsSinTermsName[correctionsIndex].Data());
       fDiffFlowCorrectionTerms[t][pW][eW][sc]->Add(fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex]);
      }
      if(sc==1)
      {
       fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex] = (TProfile2D*)styleProfile.Clone(correctionsCosTermsName[correctionsIndex].Data());
       fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex]->SetTitle(correctionsCosTermsName[correctionsIndex].Data());
       fDiffFlowCorrectionTerms[t][pW][eW][sc]->Add(fCorrectionTermsPro[t][pW][eW][sc][correctionsIndex]);
      }    
     }
    }
   }   
  }  
 }
   
 // book all 2D and 1D histograms for final results and store them in appropriate lists:
 TH2D styleHistPtEta("","",fnBinsPt,fPtMin,fPtMax,fnBinsEta,fEtaMin,fEtaMax);
 styleHistPtEta.SetXTitle("p_{t}");
 styleHistPtEta.SetYTitle("#eta");
 TH1D styleHistPt("","",fnBinsPt,fPtMin,fPtMax);
 styleHistPt.SetXTitle("p_{t}");
 TH1D styleHistEta("","",fnBinsEta,fEtaMin,fEtaMax);
 styleHistEta.SetXTitle("#eta");
 for(Int_t t=0;t<2;t++) // RP or POI
 {
  // 2D:
  fNonEmptyBins2D[t] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",typeFlag[t].Data()));
  fNonEmptyBins2D[t]->SetTitle(Form("Non-empty bins: %s (p_{t},#eta)",typeFlag[t].Data())); 
  fDFRType[t]->Add(fNonEmptyBins2D[t]); 
  // 1D:
  fNonEmptyBins1D[t][0] = (TH1D*)styleHistPt.Clone(Form("%s, p_{t}",typeFlag[t].Data()));
  fNonEmptyBins1D[t][0]->SetTitle(Form("Non-empty bins: %s (p_{t})",typeFlag[t].Data())); 
  fDFRType[t]->Add(fNonEmptyBins1D[t][0]); 
  fNonEmptyBins1D[t][1] = (TH1D*)styleHistEta.Clone(Form("%s, #eta",typeFlag[t].Data()));
  fNonEmptyBins1D[t][1]->SetTitle(Form("Non-empty bins: %s (#eta)",typeFlag[t].Data())); 
  fDFRType[t]->Add(fNonEmptyBins1D[t][1]); 
  
  for(Int_t pW=0;pW<1+(Int_t)(fUsePhiWeights||fUsePtWeights||fUseEtaWeights);pW++) // particle weights not used or used
  {
   for(Int_t eW=0;eW<2;eW++)
   {
    // correlations:
    for(Int_t correlIndex=0;correlIndex<4;correlIndex++)
    {
     // 2D:
     fFinalCorrelations2D[t][pW][eW][correlIndex] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",correlationName[correlIndex].Data()));
     fFinalCorrelations2D[t][pW][eW][correlIndex]->SetTitle(Form("%s (p_{t},#eta): %s, pWeights %s, eWeights %s",correlationName[correlIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
     fDiffFlowFinalCorrelations[t][pW][eW]->Add(fFinalCorrelations2D[t][pW][eW][correlIndex]);   
     // 1D:
     for(Int_t pe=0;pe<2;pe++) // pt or eta:
     {
      if(pe==0) 
      { 
       fFinalCorrelations1D[t][pW][eW][pe][correlIndex] = (TH1D*)styleHistPt.Clone(Form("%s, (p_{t})",correlationName[correlIndex].Data()));
       fFinalCorrelations1D[t][pW][eW][pe][correlIndex]->SetTitle(Form("%s (p_{t}): %s, pWeights %s, eWeights %s",correlationName[correlIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCorrelations[t][pW][eW]->Add(fFinalCorrelations1D[t][pW][eW][pe][correlIndex]);   
      }
      if(pe==1)
      {
       fFinalCorrelations1D[t][pW][eW][pe][correlIndex] = (TH1D*)styleHistEta.Clone(Form("%s, (#eta)",correlationName[correlIndex].Data()));
       fFinalCorrelations1D[t][pW][eW][pe][correlIndex]->SetTitle(Form("%s (#eta): %s, pWeights %s, eWeights %s",correlationName[correlIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCorrelations[t][pW][eW]->Add(fFinalCorrelations1D[t][pW][eW][pe][correlIndex]); 
      } 
     } 
    }    
    // corrections:
    for(Int_t correctionIndex=0;correctionIndex<4;correctionIndex++)
    {
     // 2D:
     fFinalCorrections2D[t][pW][eW][correctionIndex] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",correctionName[correctionIndex].Data()));
     fFinalCorrections2D[t][pW][eW][correctionIndex]->SetTitle(Form("%s (p_{t},#eta): %s, pWeights %s, eWeights %s",correctionName[correctionIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
     fDiffFlowFinalCorrections[t][pW][eW]->Add(fFinalCorrections2D[t][pW][eW][correctionIndex]);   
     // 1D:
     for(Int_t pe=0;pe<2;pe++) // pt or eta:
     {
      if(pe==0) 
      { 
       fFinalCorrections1D[t][pW][eW][pe][correctionIndex] = (TH1D*)styleHistPt.Clone(Form("%s, (p_{t})",correctionName[correctionIndex].Data()));
       fFinalCorrections1D[t][pW][eW][pe][correctionIndex]->SetTitle(Form("%s (p_{t}): %s, pWeights %s, eWeights %s",correctionName[correctionIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCorrections[t][pW][eW]->Add(fFinalCorrections1D[t][pW][eW][pe][correctionIndex]);   
      }
      if(pe==1)
      {
       fFinalCorrections1D[t][pW][eW][pe][correctionIndex] = (TH1D*)styleHistEta.Clone(Form("%s, (#eta)",correctionName[correctionIndex].Data()));
       fFinalCorrections1D[t][pW][eW][pe][correctionIndex]->SetTitle(Form("%s (#eta): %s, pWeights %s, eWeights %s",correctionName[correctionIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCorrections[t][pW][eW]->Add(fFinalCorrections1D[t][pW][eW][pe][correctionIndex]); 
      } 
     } 
    }
    // covariances:
    for(Int_t covarianceIndex=0;covarianceIndex<5;covarianceIndex++)
    {
     // 2D:
     fFinalCovariances2D[t][pW][eW][covarianceIndex] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",covarianceName[covarianceIndex].Data()));
     fFinalCovariances2D[t][pW][eW][covarianceIndex]->SetTitle(Form("%s (p_{t},#eta): %s, pWeights %s, eWeights %s",covarianceName[covarianceIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
     fDiffFlowFinalCovariances[t][pW][eW]->Add(fFinalCovariances2D[t][pW][eW][covarianceIndex]);  
     // 1D:
     for(Int_t pe=0;pe<2;pe++) // pt or eta:
     {
      if(pe==0) 
      { 
       fFinalCovariances1D[t][pW][eW][pe][covarianceIndex] = (TH1D*)styleHistPt.Clone(Form("%s, (p_{t})",covarianceName[covarianceIndex].Data()));
       fFinalCovariances1D[t][pW][eW][pe][covarianceIndex]->SetTitle(Form("%s (p_{t}): %s, pWeights %s, eWeights %s",covarianceName[covarianceIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCovariances[t][pW][eW]->Add(fFinalCovariances1D[t][pW][eW][pe][covarianceIndex]);   
      }
      if(pe==1)
      {
       fFinalCovariances1D[t][pW][eW][pe][covarianceIndex] = (TH1D*)styleHistEta.Clone(Form("%s, (#eta)",covarianceName[covarianceIndex].Data()));
       fFinalCovariances1D[t][pW][eW][pe][covarianceIndex]->SetTitle(Form("%s (#eta): %s, pWeights %s, eWeights %s",covarianceName[covarianceIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data())); 
       fDiffFlowFinalCovariances[t][pW][eW]->Add(fFinalCovariances1D[t][pW][eW][pe][covarianceIndex]); 
      } 
     } 
    }
    for(Int_t nua=0;nua<2;nua++) // corrected or not 
    {
     // cumulants:
     for(Int_t cumulantIndex=0;cumulantIndex<4;cumulantIndex++)
     {
      // 2D:
      fFinalCumulants2D[t][pW][eW][nua][cumulantIndex] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",cumulantName[cumulantIndex].Data()));
      fFinalCumulants2D[t][pW][eW][nua][cumulantIndex]->SetTitle(Form("%s (p_{t},#eta): %s, pWeights %s, eWeights %s, %s for NUA",cumulantName[cumulantIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalCumulants[t][pW][eW][nua]->Add(fFinalCumulants2D[t][pW][eW][nua][cumulantIndex]);   
      // pt:
      fFinalCumulantsPt[t][pW][eW][nua][cumulantIndex] = (TH1D*)styleHistPt.Clone(Form("%s, (p_{t})",cumulantName[cumulantIndex].Data()));
      fFinalCumulantsPt[t][pW][eW][nua][cumulantIndex]->SetTitle(Form("%s (p_{t}): %s, pWeights %s, eWeights %s, %s for NUA",cumulantName[cumulantIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalCumulants[t][pW][eW][nua]->Add(fFinalCumulantsPt[t][pW][eW][nua][cumulantIndex]);   
      // eta:     
      fFinalCumulantsEta[t][pW][eW][nua][cumulantIndex] = (TH1D*)styleHistEta.Clone(Form("%s, (#eta)",cumulantName[cumulantIndex].Data()));
      fFinalCumulantsEta[t][pW][eW][nua][cumulantIndex]->SetTitle(Form("%s (#eta): %s, pWeights %s, eWeights %s, %s for NUA",cumulantName[cumulantIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalCumulants[t][pW][eW][nua]->Add(fFinalCumulantsEta[t][pW][eW][nua][cumulantIndex]);  
     }       
     // flow:
     for(Int_t flowIndex=0;flowIndex<4;flowIndex++)
     {
      // 2D:
      fFinalFlow2D[t][pW][eW][nua][flowIndex] = (TH2D*)styleHistPtEta.Clone(Form("%s, (p_{t},#eta)",flowName[flowIndex].Data()));
      fFinalFlow2D[t][pW][eW][nua][flowIndex]->SetTitle(Form("%s (p_{t},#eta): %s, pWeights %s, eWeights %s, %s for NUA",flowName[flowIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalFlow[t][pW][eW][nua]->Add(fFinalFlow2D[t][pW][eW][nua][flowIndex]);   
      // pt: 
      fFinalFlowPt[t][pW][eW][nua][flowIndex] = (TH1D*)styleHistPt.Clone(Form("%s, (p_{t})",flowName[flowIndex].Data()));
      fFinalFlowPt[t][pW][eW][nua][flowIndex]->SetTitle(Form("%s (p_{t}): %s, pWeights %s, eWeights %s, %s for NUA",flowName[flowIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalFlow[t][pW][eW][nua]->Add(fFinalFlowPt[t][pW][eW][nua][flowIndex]);   
      // eta:
      fFinalFlowEta[t][pW][eW][nua][flowIndex] = (TH1D*)styleHistEta.Clone(Form("%s, (#eta)",flowName[flowIndex].Data()));
      fFinalFlowEta[t][pW][eW][nua][flowIndex]->SetTitle(Form("%s (#eta): %s, pWeights %s, eWeights %s, %s for NUA",flowName[flowIndex].Data(),typeFlag[t].Data(),pWeightsFlag[pW].Data(),eWeightsFlag[eW].Data(),nuaFlag[nua].Data())); 
      fDiffFlowFinalFlow[t][pW][eW][nua]->Add(fFinalFlowEta[t][pW][eW][nua][flowIndex]);   
     }
    }   
   } 
  } 
 } 
   
   

 */  
   
   

         
} // end of AliFlowAnalysisWithQCumulants::BookEverythingForDifferentialFlow()


//================================================================================================================================

/*
void AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNUAForIntQcumulants() // to be improved (do I really need this method?)
{
 // Calculate final corrections for non-uniform acceptance for Q-cumulants.
  
 // Corrections for non-uniform acceptance are stored in histogram fCorrectionsForNUA,
 // binning of fCorrectionsForNUA is organized as follows:
 //
 // 1st bin: correction to QC{2}
 // 2nd bin: correction to QC{4}
 // 3rd bin: correction to QC{6}
 // 4th bin: correction to QC{8}
  
 // shortcuts flags:
 Int_t pW = (Int_t)(useParticleWeights);
 
 Int_t eW = -1;
 
 if(eventWeights == "exact")
 {
  eW = 0;
 }

 for(Int_t sc=0;sc<2;sc++) // sin or cos terms flag
 {
  if(!(fQCorrelations[pW][eW] && fQCorrections[pW][eW][sc] && fCorrections[pW][eW]))
  {
   cout<<"WARNING: fQCorrelations[pW][eW] && fQCorrections[pW][eW][sc] && fCorrections[pW][eW] is NULL in AFAWQC::CFCFNUAFIF() !!!!"<<endl;
   cout<<"pW = "<<pW<<endl;
   cout<<"eW = "<<eW<<endl;
   cout<<"sc = "<<sc<<endl;
   exit(0);
  }
 }  

 // measured 2-, 4-, 6- and 8-particle azimuthal correlations (biased with non-uniform acceptance!):
 Double_t two = fQCorrelations[pW][eW]->GetBinContent(1); // <<2>>
 //Double_t four = fQCorrelations[pW][eW]->GetBinContent(11); // <<4>>
 //Double_t six = fQCorrelations[pW][eW]->GetBinContent(24); // <<6>>
 //Double_t eight = fQCorrelations[pW][eW]->GetBinContent(31); // <<8>>
 
 // correction terms to QC{2}:
 // <<cos(n*phi1)>>^2
 Double_t two1stTerm = pow(fQCorrections[pW][eW][1]->GetBinContent(1),2); 
 // <<sin(n*phi1)>>^2
 Double_t two2ndTerm = pow(fQCorrections[pW][eW][0]->GetBinContent(1),2); 
 // final corrections for non-uniform acceptance to QC{2}:
 Double_t correctionQC2 = -1.*two1stTerm-1.*two2ndTerm;
 fCorrections[pW][eW]->SetBinContent(1,correctionQC2); 
 
 // correction terms to QC{4}:
 // <<cos(n*phi1)>> <<cos(n*(phi1-phi2-phi3))>>
 Double_t four1stTerm = fQCorrections[pW][eW][1]->GetBinContent(1)*fQCorrections[pW][eW][1]->GetBinContent(3);  
 // <<sin(n*phi1)>> <<sin(n*(phi1-phi2-phi3))>>
 Double_t four2ndTerm = fQCorrections[pW][eW][0]->GetBinContent(1)*fQCorrections[pW][eW][0]->GetBinContent(3);  
 // <<cos(n*(phi1+phi2))>>^2
 Double_t four3rdTerm = pow(fQCorrections[pW][eW][1]->GetBinContent(2),2); 
 // <<sin(n*(phi1+phi2))>>^2
 Double_t four4thTerm = pow(fQCorrections[pW][eW][0]->GetBinContent(2),2); 
 // <<cos(n*(phi1+phi2))>> (<<cos(n*phi1)>>^2 - <<sin(n*phi1)>>^2)
 Double_t four5thTerm = fQCorrections[pW][eW][1]->GetBinContent(2)
                      * (pow(fQCorrections[pW][eW][1]->GetBinContent(1),2)-pow(fQCorrections[pW][eW][0]->GetBinContent(1),2));
 // <<sin(n*(phi1+phi2))>> <<cos(n*phi1)>> <<sin(n*phi1)>>
 Double_t four6thTerm = fQCorrections[pW][eW][0]->GetBinContent(2)
                      * fQCorrections[pW][eW][1]->GetBinContent(1)
                      * fQCorrections[pW][eW][0]->GetBinContent(1);         
 // <<cos(n*(phi1-phi2))>> (<<cos(n*phi1)>>^2 + <<sin(n*phi1)>>^2)
 Double_t four7thTerm = two*(pow(fQCorrections[pW][eW][1]->GetBinContent(1),2)+pow(fQCorrections[pW][eW][0]->GetBinContent(1),2));  
 // (<<cos(n*phi1)>>^2 + <<sin(n*phi1)>>^2)^2
 Double_t four8thTerm = pow(pow(fQCorrections[pW][eW][1]->GetBinContent(1),2)+pow(fQCorrections[pW][eW][0]->GetBinContent(1),2),2);      
 // final correction to QC{4}:
 Double_t correctionQC4 = -4.*four1stTerm+4.*four2ndTerm-four3rdTerm-four4thTerm
                        + 4.*four5thTerm+8.*four6thTerm+8.*four7thTerm-6.*four8thTerm;                            
 fCorrections[pW][eW]->SetBinContent(2,correctionQC4);   

 // ... to be improved (continued for 6th and 8th order)                                                    


} // end of AliFlowAnalysisWithQCumulants::CalculateCorrectionsForNUAForIntQcumulants()
*/

//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateQcumulantsCorrectedForNUAIntFlow()
{
 // Calculate generalized Q-cumulants (cumulants corrected for non-unifom acceptance).
 
 // measured 2-, 4-, 6- and 8-particle correlations (biased by non-uniform acceptance!):
 Double_t two = fIntFlowCorrelationsHist->GetBinContent(1); // <<2>>
 Double_t four = fIntFlowCorrelationsHist->GetBinContent(2); // <<4>>
 //Double_t six = fIntFlowCorrelationsHist->GetBinContent(3); // <<6>>
 //Double_t eight = fIntFlowCorrelationsHist->GetBinContent(4); // <<8>>
 
 // statistical error of measured 2-, 4-, 6- and 8-particle correlations:
 //Double_t twoError = fIntFlowCorrelationsHist->GetBinError(1); // statistical error of <<2>>
 //Double_t fourError = fIntFlowCorrelationsHist->GetBinError(2); // statistical error of <<4>>
 //Double_t sixError = fIntFlowCorrelationsHist->GetBinError(3); // statistical error of <<6>>
 //Double_t eightError = fIntFlowCorrelationsHist->GetBinError(4); // statistical error of <<8>>

 // QC{2}:
 // <<cos(n*phi1)>>^2
 Double_t two1stTerm = pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1),2); 
 //Double_t two1stTermErrorSquared = pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinError(1),2); 
 // <<sin(n*phi1)>>^2
 Double_t two2ndTerm = pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1),2); 
 //Double_t two2ndTermErrorSquared = pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinError(1),2); 
 // generalized QC{2}:
 Double_t gQC2 = two - two1stTerm - two2ndTerm; // to be improved (terminology, notation)
 fIntFlowQcumulants->SetBinContent(1,gQC2); 
 //fIntFlowQcumulants->SetBinError(1,0.); // to be improved (propagate error) 
 
 // QC{4}:
 // <<cos(n*phi1)>> <<cos(n*(phi1-phi2-phi3))>>
 Double_t four1stTerm = fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1)
                      * fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(3);  
 // <<sin(n*phi1)>> <<sin(n*(phi1-phi2-phi3))>>
 Double_t four2ndTerm = fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1)
                      * fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(3);  
 // <<cos(n*(phi1+phi2))>>^2
 Double_t four3rdTerm = pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(2),2); 
 // <<sin(n*(phi1+phi2))>>^2
 Double_t four4thTerm = pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(2),2); 
 // <<cos(n*(phi1+phi2))>> (<<cos(n*phi1)>>^2 - <<sin(n*phi1)>>^2)
 Double_t four5thTerm = fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(2)
                      * (pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1),2)
                      - pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1),2));
 // <<sin(n*(phi1+phi2))>> <<cos(n*phi1)>> <<sin(n*phi1)>>
 Double_t four6thTerm = fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(2)
                      * fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1)
                      * fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1);         
 // <<cos(n*(phi1-phi2))>> (<<cos(n*phi1)>>^2 + <<sin(n*phi1)>>^2)
 Double_t four7thTerm = two*(pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1),2)
                      + pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1),2));  
 // (<<cos(n*phi1)>>^2 + <<sin(n*phi1)>>^2)^2
 Double_t four8thTerm = pow(pow(fIntFlowCorrectionTermsForNUAHist[1]->GetBinContent(1),2)
                      + pow(fIntFlowCorrectionTermsForNUAHist[0]->GetBinContent(1),2),2);      
 // generalized QC{4}:
 Double_t gQC4 = four-2.*pow(two,2.)-4.*four1stTerm+4.*four2ndTerm-four3rdTerm
               - four4thTerm+4.*four5thTerm+8.*four6thTerm+8.*four7thTerm-6.*four8thTerm;                            
 fIntFlowQcumulants->SetBinContent(2,gQC4);   
 //fIntFlowQcumulants->SetBinError(2,0.); // to be improved (propagate error) 

 // ... to be improved (continued for 6th and 8th order)                                                    
    
} // end of void AliFlowAnalysisWithQCumulants::CalculateQcumulantsCorrectedForNUAIntFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectedForNUA()
{
 // Calculate integrated flow from generalized Q-cumulants (corrected for non-uniform acceptance).
 
 // to be improved: add protection for NULL pointers, propagate statistical errors from 
 // measured correlations and correction terms
 
 // generalized Q-cumulants:
 Double_t qc2 = fIntFlowQcumulants->GetBinContent(1); // QC{2}  
 Double_t qc4 = fIntFlowQcumulants->GetBinContent(2); // QC{4}  
 //Double_t qc6 = fIntFlowQcumulants->GetBinContent(3); // QC{6}  
 //Double_t qc8 = fIntFlowQcumulants->GetBinContent(4); // QC{8}
 
 // integrated flow estimates:
 Double_t v2 = 0.; // v{2,QC}  
 Double_t v4 = 0.; // v{4,QC}  
 //Double_t v6 = 0.; // v{6,QC}  
 //Double_t v8 = 0.; // v{8,QC}

 // calculate integrated flow estimates from generalized Q-cumulants: 
 if(qc2>=0.) v2 = pow(qc2,1./2.); 
 if(qc4<=0.) v4 = pow(-1.*qc4,1./4.); 
 //if(qc6>=0.) v6 = pow((1./4.)*qc6,1./6.); 
 //if(qc8<=0.) v8 = pow((-1./33.)*qc8,1./8.); 

 // store integrated flow estimates from generalized Q-cumulants:
 fIntFlow->SetBinContent(1,v2);
 fIntFlow->SetBinContent(2,v4);
 //fIntFlow->SetBinContent(3,v6);
 //fIntFlow->SetBinContent(4,v8);

} // end of void AliFlowAnalysisWithQCumulants::CalculateIntFlowCorrectedForNUA()

   
//================================================================================================================================


void AliFlowAnalysisWithQCumulants::FinalizeCorrectionTermsForNUAIntFlow() 
{
 // From profile fIntFlowCorrectionTermsForNUAPro[2] access measured corretion terms
 // and their spread, correctly calculate the statistical errors and store the final 
 // results and statistical errors for correction terms in histogram fIntFlowCorrectionTermsForNUAHist[2].
 //
 // Remark: Statistical error of correction temrs is calculated as:
 //
 //          statistical error = termA * spread * termB:
 //          termA = sqrt{sum_{i=1}^{N} w^2}/(sum_{i=1}^{N} w)
 //          termB = 1/sqrt(1-termA^2)   
 
 /* // to be improved (implement protection here)
 for(Int_t power=0;power<2;power++)
 { 
  if(!(fIntFlowCorrelationsHist && fIntFlowCorrelationsPro && fIntFlowSumOfEventWeights[power])) 
  {
   cout<<"WARNING: fIntFlowCorrelationsHist && fIntFlowCorrelationsPro && fIntFlowSumOfEventWeights[power] is NULL in AFAWQC::FCIF() !!!!"<<endl;
   cout<<"power = "<<power<<endl;
   exit(0);
  }
 }
 */
  
 for(Int_t sc=0;sc<2;sc++) // sin or cos correction terms 
 {
  for(Int_t ci=1;ci<=10;ci++) // correction term index
  {
   Double_t correction = fIntFlowCorrectionTermsForNUAPro[sc]->GetBinContent(ci);
   //Double_t spread = fIntFlowCorrectionTermsForNUAPro[sc]->GetBinError(ci);
   //Double_t sumOfLinearEventWeights = fIntFlowSumOfEventWeights[0]->GetBinContent(ci);
   //Double_t sumOfQuadraticEventWeights = fIntFlowSumOfEventWeights[1]->GetBinContent(ci);
   //Double_t termA = 0.;
   //Double_t termB = 0.;
   //if(sumOfLinearEventWeights)
   //{
   // termA = pow(sumOfQuadraticEventWeights,0.5)/sumOfLinearEventWeights;
   //} else
   // {
   //  cout<<"WARNING: sumOfLinearEventWeights == 0 in AFAWQC::FCIF() !!!!"<<endl;
   //  cout<<"         (for "<<2*ci<<"-particle correlation)"<<endl;
   // }
   /*
   if(1.-pow(termA,2.) > 0.)
   {
    termB = 1./pow(1-pow(termA,2.),0.5);
   } else
     {
      cout<<"WARNING: 1.-pow(termA,2.) <= 0 in AFAWQC::FCIF() !!!!"<<endl;   
      cout<<"         (for "<<2*ci<<"-particle correlation)"<<endl;
     }     
   Double_t statisticalError = termA * spread * termB;
   */
   fIntFlowCorrectionTermsForNUAHist[sc]->SetBinContent(ci,correction);
   //fIntFlowCorrectionTermsForNUAHist[sc]->SetBinError(ci,statisticalError);
  } // end of for(Int_t ci=1;ci<=10;ci++) // correction term index
 } // end of for(Int sc=0;sc<2;sc++) // sin or cos correction terms 
                                                                                                                                                                                               
} // end of void AliFlowAnalysisWithQCumulants::FinalizeCorrectionTermsForNUAIntFlow()


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::GetPointersForNestedLoopsHistograms(TList *outputListHistos)
{
 // Get pointers to all objects relevant for calculations with nested loops.

 // to be improved: harwired names for some objects here (fix this in paralel with method BookEverythingForNestedLoops())
   
 if(outputListHistos)
 {
  TList *nestedLoopsList = dynamic_cast<TList*>(outputListHistos->FindObject("Nested Loops"));
  if(nestedLoopsList) 
  {
   this->SetNestedLoopsList(nestedLoopsList);
  } else
    {
     cout<<"WARNING: nestedLoopsList is NULL in AFAWQC::GPFNLH() !!!!"<<endl;
     exit(0);
    }
  TString evaluateNestedLoopsName = "fEvaluateNestedLoops";
  evaluateNestedLoopsName += fAnalysisLabel->Data();  
  TProfile *evaluateNestedLoops = dynamic_cast<TProfile*>(nestedLoopsList->FindObject(evaluateNestedLoopsName.Data()));
  Bool_t bEvaluateNestedLoopsForIntFlow = kFALSE;
  Bool_t bEvaluateNestedLoopsForDiffFlow = kFALSE;
  if(evaluateNestedLoops)
  {
   this->SetEvaluateNestedLoops(evaluateNestedLoops);
   bEvaluateNestedLoopsForIntFlow = (Int_t)evaluateNestedLoops->GetBinContent(1);
   bEvaluateNestedLoopsForDiffFlow = (Int_t)evaluateNestedLoops->GetBinContent(2);
  }
  // nested loops relevant for integrated flow:  
  if(bEvaluateNestedLoopsForIntFlow)
  {
   TProfile *directCorrelations = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelation"));
   if(directCorrelations) this->SetDirectCorrelations(directCorrelations);
   TProfile *directCorrectionsCos = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsCos"));
   if(directCorrectionsCos) this->SetDirectCorrectionsCos(directCorrectionsCos);
   TProfile *directCorrectionsSin = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsSin"));
   if(directCorrectionsSin) this->SetDirectCorrectionsSin(directCorrectionsSin);
   if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights) // to be improved (this is a weak point, because now this method MUST be called after GPFWG where these booleans are set)
   {
    TProfile *directCorrelationsW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationW"));
    if(directCorrelationsW) this->SetDirectCorrelationsW(directCorrelationsW);
    TProfile *directCorrectionsCosW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsCosW"));
    if(directCorrectionsCosW) this->SetDirectCorrectionsCosW(directCorrectionsCosW);
    TProfile *directCorrectionsSinW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsSinW")); 
    if(directCorrectionsSinW) this->SetDirectCorrectionsSinW(directCorrectionsSinW);
   }
  }
  // nested loops relevant for differential flow:  
  if(bEvaluateNestedLoopsForDiffFlow)
  {
   TProfile *directCorrelationsDiffFlow = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationsDiffFlow"));
   if(directCorrelationsDiffFlow) this->SetDirectCorrelationsDiffFlow(directCorrelationsDiffFlow);
   TProfile *directCorrectionsDiffFlowCos = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowCos"));
   if(directCorrectionsDiffFlowCos) this->SetDirectCorrectionsDiffFlowCos(directCorrectionsDiffFlowCos);
   TProfile *directCorrectionsDiffFlowSin = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowSin"));
   if(directCorrectionsDiffFlowSin) this->SetDirectCorrectionsDiffFlowSin(directCorrectionsDiffFlowSin);
   if(fUsePhiWeights||fUsePtWeights||fUseEtaWeights) // to be improved (this is a weak point, because now this method MUST be called after GPFWG where these booleans are set)
   {
    TProfile *directCorrelationsDiffFlowW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrelationsDiffFlowW")); 
    if(directCorrelationsDiffFlowW) this->SetDirectCorrelationsDiffFlowW(directCorrelationsDiffFlowW);
    TProfile *directCorrectionsDiffFlowCosW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowCosW"));
    if(directCorrectionsDiffFlowCosW) this->SetDirectCorrectionsDiffFlowCosW(directCorrectionsDiffFlowCosW);
    TProfile *directCorrectionsDiffFlowSinW = dynamic_cast<TProfile*>(nestedLoopsList->FindObject("fDirectCorrectionsDiffFlowSinW"));
    if(directCorrectionsDiffFlowSinW) this->SetDirectCorrectionsDiffFlowSinW(directCorrectionsDiffFlowSinW);
   }
  } // end of if(bEvaluateNestedLoopsForDiffFlow)
 } else
   {
    cout<<"WARNING: outputListHistos is NULL in AFAWQC::GPFNLH() !!!!"<<endl;
    exit(0);
   }

} // end of void AliFlowAnalysisWithQCumulants::GetPointersForNestedLoopsHistograms(TList *outputListHistos)


//================================================================================================================================


void AliFlowAnalysisWithQCumulants::StoreHarmonic()
{
 // Store flow harmonic in common control histograms.

 (fCommonHists->GetHarmonic())->Fill(0.5,fHarmonic);
 (fCommonHists2nd->GetHarmonic())->Fill(0.5,fHarmonic);
 (fCommonHists4th->GetHarmonic())->Fill(0.5,fHarmonic);
 (fCommonHists6th->GetHarmonic())->Fill(0.5,fHarmonic);
 (fCommonHists8th->GetHarmonic())->Fill(0.5,fHarmonic);

} // end of void AliFlowAnalysisWithQCumulants::StoreHarmonic()




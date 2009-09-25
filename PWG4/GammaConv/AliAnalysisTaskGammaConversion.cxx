/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: Ana Marin, Kathrin Koch, Kenneth Aamodt                        *
 * Version 1.1                                                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

////////////////////////////////////////////////
//--------------------------------------------- 
// Class used to do analysis on conversion pairs
//---------------------------------------------
////////////////////////////////////////////////

// root
#include <TChain.h>

// analysis
#include "AliAnalysisTaskGammaConversion.h"
#include "AliStack.h"
#include "AliLog.h"
#include "AliESDtrackCuts.h"
#include "TNtuple.h"
//#include "AliCFManager.h"  // for CF
//#include "AliCFContainer.h"   // for CF
#include "AliGammaConversionAODObject.h"

class AliCFContainer;
class AliCFManager;
class AliKFVertex;
class AliAODHandler;
class AliAODEvent;
class ALiESDEvent;
class AliMCEvent;
class AliMCEventHandler;
class AliESDInputHandler;
class AliAnalysisManager;
class Riostream;
class TFile;
class TInterpreter;
class TSystem;
class TROOT;

ClassImp(AliAnalysisTaskGammaConversion)


AliAnalysisTaskGammaConversion::AliAnalysisTaskGammaConversion():
AliAnalysisTaskSE(),
  fV0Reader(NULL),
  fStack(NULL),
  fMCTruth(NULL),    // for CF
  fGCMCEvent(NULL),    // for CF
  fESDEvent(NULL),	
  fOutputContainer(NULL),
  fCFManager(0x0),   // for CF
  fHistograms(NULL),
  fDoMCTruth(kFALSE),
  fDoNeutralMeson(kFALSE),
  fDoJet(kFALSE),
  fDoChic(kFALSE),
  fKFReconstructedGammasTClone(NULL),
  fCurrentEventPosElectronTClone(NULL),
  fCurrentEventNegElectronTClone(NULL),
  fKFReconstructedGammasCutTClone(NULL),
  fPreviousEventTLVNegElectronTClone(NULL),
  fPreviousEventTLVPosElectronTClone(NULL),	
//  fKFReconstructedGammas(),
  fElectronv1(),
  fElectronv2(),
//  fCurrentEventPosElectron(),
//  fCurrentEventNegElectron(),
//  fKFReconstructedGammasCut(),		 	
//  fPreviousEventTLVNegElectron(),
//  fPreviousEventTLVPosElectron(),	
  fElectronMass(-1),
  fGammaMass(-1),
  fPi0Mass(-1),
  fEtaMass(-1),
  fGammaWidth(-1),
  fPi0Width(-1),
  fEtaWidth(-1),
  fMinOpeningAngleGhostCut(0.),
  fEsdTrackCuts(0),
  fCalculateBackground(kFALSE),
  fWriteNtuple(kFALSE),
  fGammaNtuple(NULL),
  fNeutralMesonNtuple(NULL),
  fTotalNumberOfAddedNtupleEntries(0),
  fChargedParticles(NULL),
  fChargedParticlesId(),
  fGammaPtHighest(0.),
  fMinPtForGammaJet(1.),
  fMinIsoConeSize(0.2),
  fMinPtIsoCone(0.7),
  fMinPtGamChargedCorr(0.5),
  fMinPtJetCone(0.5),
  fLeadingChargedIndex(-1),
  fAODBranch(NULL),
  fAODBranchName("GammaConv")//,
  //  fAODObjects(NULL)
{
  // Default constructor
  // Common I/O in slot 0
  DefineInput (0, TChain::Class());
  DefineOutput(0, TTree::Class());
	
  // Your private output
  DefineOutput(1, TList::Class());
	
  // Define standard ESD track cuts for Gamma-hadron correlation 
  SetESDtrackCuts();
}

AliAnalysisTaskGammaConversion::AliAnalysisTaskGammaConversion(const char* name):
  AliAnalysisTaskSE(name),
  fV0Reader(NULL),
  fStack(NULL),
  fMCTruth(NULL),    // for CF
  fGCMCEvent(NULL),    // for CF
  fESDEvent(NULL),	
  fOutputContainer(0x0),
  fCFManager(0x0),   // for CF
  fHistograms(NULL),
  fDoMCTruth(kFALSE),
  fDoNeutralMeson(kFALSE),
  fDoJet(kFALSE),
  fDoChic(kFALSE),
  fKFReconstructedGammasTClone(NULL),
  fCurrentEventPosElectronTClone(NULL),
  fCurrentEventNegElectronTClone(NULL),
  fKFReconstructedGammasCutTClone(NULL),
  fPreviousEventTLVNegElectronTClone(NULL),
  fPreviousEventTLVPosElectronTClone(NULL),	
  //  fKFReconstructedGammas(),
  fElectronv1(),
  fElectronv2(),
  //  fCurrentEventPosElectron(),
  //  fCurrentEventNegElectron(),
  //  fKFReconstructedGammasCut(),	
  //  fPreviousEventTLVNegElectron(),
  //  fPreviousEventTLVPosElectron(),
  fElectronMass(-1),
  fGammaMass(-1),
  fPi0Mass(-1),
  fEtaMass(-1),
  fGammaWidth(-1),
  fPi0Width(-1),
  fEtaWidth(-1),
  fMinOpeningAngleGhostCut(0.),
  fEsdTrackCuts(0),
  fCalculateBackground(kFALSE),
  fWriteNtuple(kFALSE),
  fGammaNtuple(NULL),
  fNeutralMesonNtuple(NULL),
  fTotalNumberOfAddedNtupleEntries(0),
  fChargedParticles(NULL),
  fChargedParticlesId(),
  fGammaPtHighest(0.),
  fMinPtForGammaJet(1.),
  fMinIsoConeSize(0.2),
  fMinPtIsoCone(0.7),
  fMinPtGamChargedCorr(0.5),
  fMinPtJetCone(0.5),
  fLeadingChargedIndex(-1),
  fAODBranch(NULL),
  fAODBranchName("GammaConv")//,
  // fAODObjects(NULL)
{
  // Common I/O in slot 0
  DefineInput (0, TChain::Class());
  DefineOutput(0, TTree::Class());
	
  // Your private output
  DefineOutput(1, TList::Class());
  DefineOutput(2, AliCFContainer::Class());  // for CF
	
	
  // Define standard ESD track cuts for Gamma-hadron correlation 
  SetESDtrackCuts();
}

AliAnalysisTaskGammaConversion::~AliAnalysisTaskGammaConversion() 
{
  // Remove all pointers
	
  if(fOutputContainer){
    fOutputContainer->Clear() ; 
    delete fOutputContainer ;
  }
  if(fHistograms){
    delete fHistograms;
  }
  if(fV0Reader){
    delete fV0Reader;
  }
	
  // for CF
  if(fCFManager){
    delete fCFManager;
  }
	
  if (fAODBranch) {
    fAODBranch->Clear();
    delete fAODBranch ;
  }
}


void AliAnalysisTaskGammaConversion::Init()
{
  // Initialization
  // AliLog::SetGlobalLogLevel(AliLog::kError);
}
void AliAnalysisTaskGammaConversion::SetESDtrackCuts()
{
  // SetESDtrackCuts
	
  fEsdTrackCuts = new AliESDtrackCuts("AliESDtrackCuts");
  //standard cuts from:
  //http://aliceinfo.cern.ch/alicvs/viewvc/PWG0/dNdEta/CreateCuts.C?revision=1.4&view=markup
  //fEsdTrackCuts->SetMinNClustersTPC(50);
  //fEsdTrackCuts->SetMaxChi2PerClusterTPC(3.5);
  //fEsdTrackCuts->SetMaxCovDiagonalElements(2,2,0.5,0.5,2);
  fEsdTrackCuts->SetRequireTPCRefit(kTRUE);
  fEsdTrackCuts->SetRequireITSRefit(kTRUE);
  fEsdTrackCuts->SetMaxNsigmaToVertex(3);
  fEsdTrackCuts->SetRequireSigmaToVertex(kTRUE);
  //  fEsdTrackCuts->SetAcceptKinkDaughters(kFALSE);
	
}

void AliAnalysisTaskGammaConversion::Exec(Option_t */*option*/)
{
  // Execute analysis for current event
	
  ConnectInputData("");
	
  //Each event needs an empty branch
  fAODBranch->Clear();
	
  if(fKFReconstructedGammasTClone == NULL){
    fKFReconstructedGammasTClone = new TClonesArray("AliKFParticle",0);
  }
  if(fCurrentEventPosElectronTClone == NULL){
    fCurrentEventPosElectronTClone = new TClonesArray("AliESDtrack",0);
  }
  if(fCurrentEventNegElectronTClone == NULL){
    fCurrentEventNegElectronTClone = new TClonesArray("AliESDtrack",0);
  }
  if(fKFReconstructedGammasCutTClone == NULL){
    fKFReconstructedGammasCutTClone = new TClonesArray("AliKFParticle",0);
  }
  if(fPreviousEventTLVNegElectronTClone == NULL){
    fPreviousEventTLVNegElectronTClone = new TClonesArray("TLorentzVector",0);
  }
  if(fPreviousEventTLVPosElectronTClone == NULL){
    fPreviousEventTLVPosElectronTClone  = new TClonesArray("TLorentzVector",0);
  }
  if(fChargedParticles == NULL){
    fChargedParticles = new TClonesArray("AliESDtrack",0);
  }
	
  //clear TClones
  fKFReconstructedGammasTClone->Clear();
  fCurrentEventPosElectronTClone->Clear();
  fCurrentEventNegElectronTClone->Clear();
  fKFReconstructedGammasCutTClone->Clear();
  fPreviousEventTLVNegElectronTClone->Clear();
  fPreviousEventTLVPosElectronTClone->Clear();
	
  //clear vectors
  //  fKFReconstructedGammas.clear();
  fElectronv1.clear();
  fElectronv2.clear();
  //  fCurrentEventPosElectron.clear();
  //  fCurrentEventNegElectron.clear();	
  //  fKFReconstructedGammasCut.clear(); 
	
  fChargedParticles->Clear();	
  fChargedParticlesId.clear();	
	
  //Clear the data in the v0Reader
  fV0Reader->UpdateEventByEventData();
	
	
  // Process the MC information
  if(fDoMCTruth){
    ProcessMCData();
  }
	
  //Process the v0 information with no cuts
  ProcessV0sNoCut();
	
  // Process the v0 information
  ProcessV0s();
	
  //Fill Gamma AOD
  FillAODWithConversionGammas() ; 
	
  //calculate background if flag is set
  if(fCalculateBackground){
    CalculateBackground();
  }
	
  // Process reconstructed gammas
  if(fDoNeutralMeson == kTRUE){
    ProcessGammasForNeutralMesonAnalysis();
  }
	
  CheckV0Efficiency();
	
  //Process reconstructed gammas electrons for Chi_c Analysis
  if(fDoChic == kTRUE){
    ProcessGammaElectronsForChicAnalysis();
  }
  // Process reconstructed gammas for gamma Jet/hadron correlations
  if(fDoJet == kTRUE){
    ProcessGammasForGammaJetAnalysis();
  }
	
  PostData(1, fOutputContainer);
  PostData(2, fCFManager->GetParticleContainer());  // for CF
	
}

void AliAnalysisTaskGammaConversion::ConnectInputData(Option_t */*option*/){
  // see header file for documentation
	
  if(fV0Reader == NULL){
    // Write warning here cuts and so on are default if this ever happens
  }
  fV0Reader->Initialize();
}



void AliAnalysisTaskGammaConversion::ProcessMCData(){
  // see header file for documentation
	
  fStack = fV0Reader->GetMCStack();
  fMCTruth = fV0Reader->GetMCTruth();  // for CF
  fGCMCEvent = fV0Reader->GetMCEvent();  // for CF
	
	
  // for CF
  if(!fGCMCEvent) cout << "NO MC INFO FOUND" << endl;
  fCFManager->SetEventInfo(fGCMCEvent);
  Double_t containerInput[3]; 
  // end for CF
	
	
  if(fV0Reader->CheckForPrimaryVertex() == kFALSE){
    return; // aborts if the primary vertex does not have contributors.
  }
	
  for (Int_t iTracks = 0; iTracks < fStack->GetNtrack(); iTracks++) {
    TParticle* particle = (TParticle *)fStack->Particle(iTracks);
		
    if (!particle) {
      //print warning here
      continue;
    }
		
    ///////////////////////Begin Chic Analysis/////////////////////////////
		
    if(particle->GetPdgCode() == 443){//Is JPsi	
      if(particle->GetNDaughters()==2){
	if(TMath::Abs(fStack->Particle(particle->GetFirstDaughter())->GetPdgCode()) == 11 &&
	   TMath::Abs(fStack->Particle(particle->GetLastDaughter())->GetPdgCode()) == 11){
	  TParticle* daug0 = fStack->Particle(particle->GetFirstDaughter());
	  TParticle* daug1 = fStack->Particle(particle->GetLastDaughter());
	  if(TMath::Abs(daug0->Eta()) < 0.9 && TMath::Abs(daug1->Eta()) < 0.9)
	    fHistograms->FillTable("Table_Electrons",3);//e+ e-  from J/Psi inside acceptance
					
	  if( TMath::Abs(daug0->Eta()) < 0.9){
	    if(daug0->GetPdgCode() == -11)
	      fHistograms->FillTable("Table_Electrons",1);//e+  from J/Psi inside acceptance
	    else
	      fHistograms->FillTable("Table_Electrons",2);//e-   from J/Psi inside acceptance
						
	  }
	  if(TMath::Abs(daug1->Eta()) < 0.9){
	    if(daug1->GetPdgCode() == -11)
	      fHistograms->FillTable("Table_Electrons",1);//e+  from J/Psi inside acceptance
	    else
	      fHistograms->FillTable("Table_Electrons",2);//e-   from J/Psi inside acceptance
	  }
	}
      }
    }
    //              const int CHI_C0   = 10441;
    //              const int CHI_C1   = 20443;
    //              const int CHI_C2   = 445
    if(particle->GetPdgCode() == 22){//gamma from JPsi
      if(particle->GetMother(0) > -1){
	if(fStack->Particle(particle->GetMother(0))->GetPdgCode() == 10441 ||
	   fStack->Particle(particle->GetMother(0))->GetPdgCode() == 20443 ||
	   fStack->Particle(particle->GetMother(0))->GetPdgCode() == 445){
	  if(TMath::Abs(particle->Eta()) < 1.2)
	    fHistograms->FillTable("Table_Electrons",17);// gamma from chic inside accptance
	}
      }
    }
    if(particle->GetPdgCode() == 10441 || particle->GetPdgCode() == 20443 || particle->GetPdgCode() == 445){
      if( particle->GetNDaughters() == 2){
	TParticle* daug0 = fStack->Particle(particle->GetFirstDaughter());
	TParticle* daug1 = fStack->Particle(particle->GetLastDaughter());
				
	if( (daug0->GetPdgCode() == 443 || daug0->GetPdgCode() == 22) && (daug1->GetPdgCode() == 443 || daug1->GetPdgCode() == 22) ){
	  if( daug0->GetPdgCode() == 443){
	    TParticle* daugE0 = fStack->Particle(daug0->GetFirstDaughter());
	    TParticle* daugE1 = fStack->Particle(daug0->GetLastDaughter());
	    if( TMath::Abs(daug1->Eta()) < 1.2 && TMath::Abs(daugE0->Eta()) < 0.9 && TMath::Abs(daugE1->Eta()) < 0.9 )
	      fHistograms->FillTable("Table_Electrons",18);
						
	  }//if
	  else if (daug1->GetPdgCode() == 443){
	    TParticle* daugE0 = fStack->Particle(daug1->GetFirstDaughter());
	    TParticle* daugE1 = fStack->Particle(daug1->GetLastDaughter());
	    if( TMath::Abs(daug0->Eta()) < 1.2 && TMath::Abs(daugE0->Eta()) < 0.9 && TMath::Abs(daugE1->Eta()) < 0.9 )
	      fHistograms->FillTable("Table_Electrons",18);
	  }//else if
	}//gamma o Jpsi
      }//GetNDaughters
    }
		
		
    /////////////////////End Chic Analysis////////////////////////////
		
		
    if(TMath::Abs(particle->Eta())> fV0Reader->GetEtaCut() )	continue;
		
    if(particle->R()>fV0Reader->GetMaxRCut())	continue; // cuts on distance from collision point
		
    Double_t tmpPhi=particle->Phi();
		
    if(particle->Phi()> TMath::Pi()){
      tmpPhi = particle->Phi()-(2*TMath::Pi());
    }
		
    Double_t rapidity;
    if(particle->Energy() - particle->Pz() == 0 || particle->Energy() + particle->Pz() == 0){
      rapidity=0;
    }
    else{
      rapidity = 0.5*(TMath::Log((particle->Energy()+particle->Pz()) / (particle->Energy()-particle->Pz())));
    }	
		
    //process the gammas
    if (particle->GetPdgCode() == 22){
      

      if(particle->GetMother(0) >-1 && fStack->Particle(particle->GetMother(0))->GetPdgCode() == 22){
	continue; // no photon as mothers!
      }
      
      if(particle->GetMother(0) >= fStack->GetNprimary()){
	continue; // the gamma has a mother, and it is not a primary particle
      }
			
      fHistograms->FillHistogram("MC_allGamma_Energy", particle->Energy());
      fHistograms->FillHistogram("MC_allGamma_Pt", particle->Pt());
      fHistograms->FillHistogram("MC_allGamma_Eta", particle->Eta());
      fHistograms->FillHistogram("MC_allGamma_Phi", tmpPhi);
      fHistograms->FillHistogram("MC_allGamma_Rapid", rapidity);
			
      // for CF
      containerInput[0] = particle->Pt();
      containerInput[1] = particle->Eta();
      if(particle->GetMother(0) >=0){
	containerInput[2] = fStack->Particle(particle->GetMother(0))->GetMass();
      }
      else{
	containerInput[2]=-1;
      }
      
      fCFManager->GetParticleContainer()->Fill(containerInput,kStepGenerated);					// generated gamma
			
      if(particle->GetMother(0) < 0){   // direct gamma
	fHistograms->FillHistogram("MC_allDirectGamma_Energy",particle->Energy());
	fHistograms->FillHistogram("MC_allDirectGamma_Pt", particle->Pt());
	fHistograms->FillHistogram("MC_allDirectGamma_Eta", particle->Eta());
	fHistograms->FillHistogram("MC_allDirectGamma_Phi", tmpPhi);
	fHistograms->FillHistogram("MC_allDirectGamma_Rapid", rapidity);				
      }
			
      // looking for conversion (electron + positron from pairbuilding (= 5) )
      TParticle* ePos = NULL;
      TParticle* eNeg = NULL;
			
      if(particle->GetNDaughters() >= 2){
	for(Int_t daughterIndex=particle->GetFirstDaughter();daughterIndex<=particle->GetLastDaughter();daughterIndex++){
	  TParticle *tmpDaughter = fStack->Particle(daughterIndex);
	  if(tmpDaughter->GetUniqueID() == 5){
	    if(tmpDaughter->GetPdgCode() == 11){
	      eNeg = tmpDaughter;
	    }
	    else if(tmpDaughter->GetPdgCode() == -11){
	      ePos = tmpDaughter;
	    }
	  }
	}
      }
			
			
      if(ePos == NULL || eNeg == NULL){ // means we do not have two daughters from pair production
	continue;
      }
			
			
      Double_t ePosPhi = ePos->Phi();
      if(ePos->Phi()> TMath::Pi()) ePosPhi = ePos->Phi()-(2*TMath::Pi());
			
      Double_t eNegPhi = eNeg->Phi();
      if(eNeg->Phi()> TMath::Pi()) eNegPhi = eNeg->Phi()-(2*TMath::Pi());
			
			
      if(ePos->Pt()<fV0Reader->GetPtCut() || eNeg->Pt()<fV0Reader->GetPtCut()){
	continue; // no reconstruction below the Pt cut
      }
			
      if(TMath::Abs(ePos->Eta())> fV0Reader->GetEtaCut() || TMath::Abs(eNeg->Eta())> fV0Reader->GetEtaCut()){
	continue;
      }	
			
      if(ePos->R()>fV0Reader->GetMaxRCut()){
	continue; // cuts on distance from collision point
      }

      if(TMath::Abs(ePos->Vz()) > fV0Reader->GetMaxZCut()){
	continue;   // outside material
      }
			
			
      if((TMath::Abs(ePos->Vz()) * fV0Reader->GetLineCutZRSlope()) - fV0Reader->GetLineCutZValue()  > ePos->R()){
	continue;               // line cut to exclude regions where we do not reconstruct
      }		
      		
			
      // for CF
      fCFManager->GetParticleContainer()->Fill(containerInput,kStepReconstructable);	// reconstructable gamma	
			
      fHistograms->FillHistogram("MC_ConvGamma_Energy", particle->Energy());
      fHistograms->FillHistogram("MC_ConvGamma_Pt", particle->Pt());
      fHistograms->FillHistogram("MC_ConvGamma_Eta", particle->Eta());
      fHistograms->FillHistogram("MC_ConvGamma_Phi", tmpPhi);
      fHistograms->FillHistogram("MC_ConvGamma_Rapid", rapidity);
      fHistograms->FillHistogram("MC_ConvGamma_Pt_Eta", particle->Pt(),particle->Eta());
			
      fHistograms->FillHistogram("MC_E_Energy", eNeg->Energy());
      fHistograms->FillHistogram("MC_E_Pt", eNeg->Pt());
      fHistograms->FillHistogram("MC_E_Eta", eNeg->Eta());
      fHistograms->FillHistogram("MC_E_Phi", eNegPhi);
			
      fHistograms->FillHistogram("MC_P_Energy", ePos->Energy());
      fHistograms->FillHistogram("MC_P_Pt", ePos->Pt());
      fHistograms->FillHistogram("MC_P_Eta", ePos->Eta());
      fHistograms->FillHistogram("MC_P_Phi", ePosPhi);
			
			
      // begin Mapping 
      Int_t rBin    = fHistograms->GetRBin(ePos->R());
      Int_t phiBin  = fHistograms->GetPhiBin(particle->Phi());
			
      TString nameMCMappingPhiR="";
      nameMCMappingPhiR.Form("MC_Conversion_Mapping_Phi%02d_R%02d",phiBin,rBin);
      fHistograms->FillHistogram(nameMCMappingPhiR, ePos->Vz(), particle->Eta());
			
      TString nameMCMappingPhi="";
      nameMCMappingPhi.Form("MC_Conversion_Mapping_Phi%02d",phiBin);
      fHistograms->FillHistogram(nameMCMappingPhi, particle->Eta());
			
      TString nameMCMappingR="";
      nameMCMappingR.Form("MC_Conversion_Mapping_R%02d",rBin);
      fHistograms->FillHistogram(nameMCMappingR, particle->Eta());
			
      TString nameMCMappingPhiInR="";
      nameMCMappingPhiInR.Form("MC_Conversion_Mapping_Phi_in_R_%02d",rBin);
      fHistograms->FillHistogram(nameMCMappingPhiInR, tmpPhi);
      //end mapping
			
      fHistograms->FillHistogram("MC_Conversion_R",ePos->R());
      fHistograms->FillHistogram("MC_Conversion_ZR",ePos->Vz(),ePos->R());
      fHistograms->FillHistogram("MC_Conversion_XY",ePos->Vx(),ePos->Vy());
      fHistograms->FillHistogram("MC_Conversion_OpeningAngle",GetMCOpeningAngle(ePos, eNeg));
			
      if(particle->GetMother(0) < 0){ // no mother = direct gamma, still inside converted
	fHistograms->FillHistogram("MC_ConvDirectGamma_Energy",particle->Energy());
	fHistograms->FillHistogram("MC_ConvDirectGamma_Pt", particle->Pt());
	fHistograms->FillHistogram("MC_ConvDirectGamma_Eta", particle->Eta());
	fHistograms->FillHistogram("MC_ConvDirectGamma_Phi", tmpPhi);
	fHistograms->FillHistogram("MC_ConvDirectGamma_Rapid", rapidity);
				
      } // end direct gamma
      else{   // mother exits 
	/*	if( fStack->Particle(particle->GetMother(0))->GetPdgCode()==10441 ||//chic0 
		fStack->Particle(particle->GetMother(0))->GetPdgCode()==20443 ||//psi2S
		fStack->Particle(particle->GetMother(0))->GetPdgCode()==445  //chic2
		){ 
		fMCGammaChic.push_back(particle);
		}
	*/
      }  // end if mother exits
    } // end if particle is a photon
		
		
		
    // process motherparticles (2 gammas as daughters)
    // the motherparticle had already to pass the R and the eta cut, but no line cut.
    // the line cut is just valid for the conversions!
		
    if(particle->GetNDaughters() == 2){
			
      TParticle* daughter0 = (TParticle*)fStack->Particle(particle->GetFirstDaughter());
      TParticle* daughter1 = (TParticle*)fStack->Particle(particle->GetLastDaughter());
			
      if(daughter0->GetPdgCode() != 22 || daughter1->GetPdgCode() != 22) continue; //check for gamma gamma daughters
			
      // Check the acceptance for both gammas
      Bool_t gammaEtaCut = kTRUE;
      if(TMath::Abs(daughter0->Eta()) > fV0Reader->GetEtaCut() || TMath::Abs(daughter1->Eta()) > fV0Reader->GetEtaCut()  ) gammaEtaCut = kFALSE;
      Bool_t gammaRCut = kTRUE;
      if(daughter0->R() > fV0Reader->GetMaxRCut() || daughter1->R() > fV0Reader->GetMaxRCut()  ) gammaRCut = kFALSE;
			
			
			
      // check for conversions now -> have to pass eta, R and line cut!
      Bool_t daughter0Electron = kFALSE;
      Bool_t daughter0Positron = kFALSE;
      Bool_t daughter1Electron = kFALSE;
      Bool_t daughter1Positron = kFALSE;
			
      if(daughter0->GetNDaughters() >= 2){  // first gamma
	for(Int_t TrackIndex=daughter0->GetFirstDaughter();TrackIndex<=daughter0->GetLastDaughter();TrackIndex++){
	  TParticle *tmpDaughter = fStack->Particle(TrackIndex);
	  if(tmpDaughter->GetUniqueID() == 5){
	    if(tmpDaughter->GetPdgCode() == 11){
	      if( TMath::Abs(tmpDaughter->Eta()) <= fV0Reader->GetEtaCut() ){
		if( ( TMath::Abs(tmpDaughter->Vz()) * fV0Reader->GetLineCutZRSlope()) - fV0Reader->GetLineCutZValue()  < tmpDaughter->R() ){
		  if(tmpDaughter->R()< fV0Reader->GetMaxRCut()){
		    daughter0Electron = kTRUE;
		  }
		}
								
	      }
	    }
	    else if(tmpDaughter->GetPdgCode() == -11){
	      if( TMath::Abs(tmpDaughter->Eta()) <= fV0Reader->GetEtaCut() ){
		if( ( TMath::Abs(tmpDaughter->Vz()) * fV0Reader->GetLineCutZRSlope()) - fV0Reader->GetLineCutZValue()  < tmpDaughter->R() ){
		  if(tmpDaughter->R()< fV0Reader->GetMaxRCut()){
		    daughter0Positron = kTRUE;
		  }
		}						
	      }					
	    }
	  }
	}
      }
			
			
      if(daughter1->GetNDaughters() >= 2){   // second gamma
	for(Int_t TrackIndex=daughter1->GetFirstDaughter();TrackIndex<=daughter1->GetLastDaughter();TrackIndex++){
	  TParticle *tmpDaughter = fStack->Particle(TrackIndex);
	  if(tmpDaughter->GetUniqueID() == 5){
	    if(tmpDaughter->GetPdgCode() == 11){
	      if( TMath::Abs(tmpDaughter->Eta()) <= fV0Reader->GetEtaCut() ){
		if( ( TMath::Abs(tmpDaughter->Vz()) * fV0Reader->GetLineCutZRSlope()) - fV0Reader->GetLineCutZValue()  < tmpDaughter->R() ){
		  if(tmpDaughter->R()< fV0Reader->GetMaxRCut()){
		    daughter1Electron = kTRUE;
		  }
		}
								
	      }
	    }
	    else if(tmpDaughter->GetPdgCode() == -11){
	      if( TMath::Abs(tmpDaughter->Eta()) <= fV0Reader->GetEtaCut() ){
		if( ( TMath::Abs(tmpDaughter->Vz()) * fV0Reader->GetLineCutZRSlope()) - fV0Reader->GetLineCutZValue()  < tmpDaughter->R() ){
		  if(tmpDaughter->R()< fV0Reader->GetMaxRCut()){
		    daughter1Positron = kTRUE;
		  }
		}
								
	      }
							
	    }
	  }
	}
      }
			
			
			
			
      if(particle->GetPdgCode()==111){     //Pi0
	if( iTracks >= fStack->GetNprimary()){
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_Eta", particle->Eta());
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_Rapid", rapidity);
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_Phi", tmpPhi);
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_Pt", particle->Pt());
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_Energy", particle->Energy());
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_R", particle->R());
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_ZR", particle->Vz(),particle->R());
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_GammaDaughter_OpeningAngle", GetMCOpeningAngle(daughter0,daughter1));
	  fHistograms->FillHistogram("MC_Pi0_Secondaries_XY", particle->Vx(),particle->Vy());//only fill from one daughter to avoid multiple filling
					
	  if(gammaEtaCut && gammaRCut){  
	    //if(TMath::Abs(daughter0->Eta()) <= fV0Reader->GetEtaCut() && TMath::Abs(daughter1->Eta()) <= fV0Reader->GetEtaCut() ){
	    fHistograms->FillHistogram("MC_Pi0_Secondaries_Pt_Eta_withinAcceptance", particle->Pt(),particle->Eta());
	    fHistograms->FillHistogram("MC_Pi0_Secondaries_Pt_Rapid_withinAcceptance", particle->Pt(),rapidity);
	    if(daughter0Electron && daughter0Positron && daughter1Electron && daughter1Positron){
	      fHistograms->FillHistogram("MC_Pi0_Secondaries_Pt_Eta_ConvGamma_withinAcceptance", particle->Pt(),particle->Eta());
	      fHistograms->FillHistogram("MC_Pi0_Secondaries_Pt_Rapid_ConvGamma_withinAcceptance", particle->Pt(),rapidity);
	    }
	  }
	}
	else{
	  fHistograms->FillHistogram("MC_Pi0_Eta", particle->Eta());	
	  fHistograms->FillHistogram("MC_Pi0_Rapid", rapidity);
	  fHistograms->FillHistogram("MC_Pi0_Phi", tmpPhi);
	  fHistograms->FillHistogram("MC_Pi0_Pt", particle->Pt());
	  fHistograms->FillHistogram("MC_Pi0_Energy", particle->Energy());
	  fHistograms->FillHistogram("MC_Pi0_R", particle->R());
	  fHistograms->FillHistogram("MC_Pi0_ZR", particle->Vz(),particle->R());
	  fHistograms->FillHistogram("MC_Pi0_GammaDaughter_OpeningAngle", GetMCOpeningAngle(daughter0,daughter1));
	  fHistograms->FillHistogram("MC_Pi0_XY", particle->Vx(), particle->Vy());//only fill from one daughter to avoid multiple filling
					
	  if(gammaEtaCut && gammaRCut){
	    //	  if(TMath::Abs(daughter0->Eta()) <= fV0Reader->GetEtaCut() && TMath::Abs(daughter1->Eta()) <= fV0Reader->GetEtaCut() ){
	    fHistograms->FillHistogram("MC_Pi0_Pt_Eta_withinAcceptance", particle->Pt(),particle->Eta());
	    fHistograms->FillHistogram("MC_Pi0_Pt_Rapid_withinAcceptance", particle->Pt(),rapidity);
	    if(daughter0Electron && daughter0Positron && daughter1Electron && daughter1Positron){
	      fHistograms->FillHistogram("MC_Pi0_Pt_Eta_ConvGamma_withinAcceptance", particle->Pt(),particle->Eta());
	      fHistograms->FillHistogram("MC_Pi0_Pt_Rapid_ConvGamma_withinAcceptance", particle->Pt(),rapidity);
	      fHistograms->FillHistogram("MC_Pi0_ZR_ConvGamma_withinAcceptance", particle->Vz(),particle->R());
	    }
	  }
	}
      }
			
      if(particle->GetPdgCode()==221){   //Eta
	fHistograms->FillHistogram("MC_Eta_Eta", particle->Eta());
	fHistograms->FillHistogram("MC_Eta_Rapid", rapidity);
	fHistograms->FillHistogram("MC_Eta_Phi",tmpPhi);
	fHistograms->FillHistogram("MC_Eta_Pt", particle->Pt());
	fHistograms->FillHistogram("MC_Eta_Energy", particle->Energy());
	fHistograms->FillHistogram("MC_Eta_R", particle->R());
	fHistograms->FillHistogram("MC_Eta_ZR", particle->Vz(),particle->R());
	fHistograms->FillHistogram("MC_Eta_GammaDaughter_OpeningAngle", GetMCOpeningAngle(daughter0,daughter1));
	fHistograms->FillHistogram("MC_Eta_XY", particle->Vx(), particle->Vy());//only fill from one daughter to avoid multiple filling
				
	if(gammaEtaCut && gammaRCut){  
	  //	if(TMath::Abs(daughter0->Eta()) <= fV0Reader->GetEtaCut() && TMath::Abs(daughter1->Eta()) <= fV0Reader->GetEtaCut() ){
	  fHistograms->FillHistogram("MC_Eta_Pt_Eta_withinAcceptance", particle->Pt(),particle->Eta());
	  fHistograms->FillHistogram("MC_Eta_Pt_Rapid_withinAcceptance", particle->Pt(),rapidity);
	  if(daughter0Electron && daughter0Positron && daughter1Electron && daughter1Positron){
	    fHistograms->FillHistogram("MC_Eta_Pt_Eta_ConvGamma_withinAcceptance", particle->Pt(),particle->Eta());
	    fHistograms->FillHistogram("MC_Eta_Pt_Rapid_ConvGamma_withinAcceptance", particle->Pt(),rapidity);
	    fHistograms->FillHistogram("MC_Eta_ZR_ConvGamma_withinAcceptance", particle->Vz(),particle->R());
	  }
					
	}
				
      }
			
      // all motherparticles with 2 gammas as daughters
      fHistograms->FillHistogram("MC_Mother_R", particle->R());
      fHistograms->FillHistogram("MC_Mother_ZR", particle->Vz(),particle->R());
      fHistograms->FillHistogram("MC_Mother_XY", particle->Vx(),particle->Vy());
      fHistograms->FillHistogram("MC_Mother_Mass", particle->GetCalcMass());
      fHistograms->FillHistogram("MC_Mother_GammaDaughter_OpeningAngle", GetMCOpeningAngle(daughter0,daughter1));
      fHistograms->FillHistogram("MC_Mother_Energy", particle->Energy());
      fHistograms->FillHistogram("MC_Mother_Pt", particle->Pt());
      fHistograms->FillHistogram("MC_Mother_Eta", particle->Eta());
      fHistograms->FillHistogram("MC_Mother_Rapid", rapidity);
      fHistograms->FillHistogram("MC_Mother_Phi",tmpPhi);
      fHistograms->FillHistogram("MC_Mother_InvMass_vs_Pt",particle->GetMass(),particle->Pt());			
			
      if(gammaEtaCut && gammaRCut){  
	//      if(TMath::Abs(daughter0->Eta()) <= fV0Reader->GetEtaCut() && TMath::Abs(daughter1->Eta()) <= fV0Reader->GetEtaCut() ){
	fHistograms->FillHistogram("MC_Mother_Pt_Eta_withinAcceptance", particle->Pt(),particle->Eta());
	fHistograms->FillHistogram("MC_Mother_Pt_Rapid_withinAcceptance", particle->Pt(),rapidity);
	fHistograms->FillHistogram("MC_Mother_InvMass_vs_Pt_withinAcceptance",particle->GetMass(),particle->Pt());			
	if(daughter0Electron && daughter0Positron && daughter1Electron && daughter1Positron){
	  fHistograms->FillHistogram("MC_Mother_Pt_Eta_ConvGamma_withinAcceptance", particle->Pt(),particle->Eta());
	  fHistograms->FillHistogram("MC_Mother_Pt_Rapid_ConvGamma_withinAcceptance", particle->Pt(),rapidity);
	  fHistograms->FillHistogram("MC_Mother_InvMass_vs_Pt_ConvGamma_withinAcceptance",particle->GetMass(),particle->Pt());			
					
	}
				
				
      }  // end passed R and eta cut
			
    } // end if(particle->GetNDaughters() == 2)
		
  }// end for (Int_t iTracks = 0; iTracks < fStack->GetNtrack(); iTracks++)

} // end ProcessMCData



void AliAnalysisTaskGammaConversion::FillNtuple(){
  //Fills the ntuple with the different values
	
  if(fGammaNtuple == NULL){
    return;
  }
  Int_t numberOfV0s = fV0Reader->GetNumberOfV0s();
  for(Int_t i=0;i<numberOfV0s;i++){
    Float_t values[27] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    AliESDv0 * cV0 = fV0Reader->GetV0(i);
    Double_t negPID=0;
    Double_t posPID=0;
    fV0Reader->GetPIDProbability(negPID,posPID);
    values[0]=cV0->GetOnFlyStatus();
    values[1]=fV0Reader->CheckForPrimaryVertex();
    values[2]=negPID;
    values[3]=posPID;
    values[4]=fV0Reader->GetX();
    values[5]=fV0Reader->GetY();
    values[6]=fV0Reader->GetZ();
    values[7]=fV0Reader->GetXYRadius();
    values[8]=fV0Reader->GetMotherCandidateNDF();
    values[9]=fV0Reader->GetMotherCandidateChi2();
    values[10]=fV0Reader->GetMotherCandidateEnergy();
    values[11]=fV0Reader->GetMotherCandidateEta();
    values[12]=fV0Reader->GetMotherCandidatePt();
    values[13]=fV0Reader->GetMotherCandidateMass();
    values[14]=fV0Reader->GetMotherCandidateWidth();
    //    values[15]=fV0Reader->GetMotherMCParticle()->Pt();   MOVED TO THE END, HAS TO BE CALLED AFTER HasSameMother NB: still has the same entry in the array
    values[16]=fV0Reader->GetOpeningAngle();
    values[17]=fV0Reader->GetNegativeTrackEnergy();
    values[18]=fV0Reader->GetNegativeTrackPt();
    values[19]=fV0Reader->GetNegativeTrackEta();
    values[20]=fV0Reader->GetNegativeTrackPhi();
    values[21]=fV0Reader->GetPositiveTrackEnergy();
    values[22]=fV0Reader->GetPositiveTrackPt();
    values[23]=fV0Reader->GetPositiveTrackEta();
    values[24]=fV0Reader->GetPositiveTrackPhi();
    values[25]=fV0Reader->HasSameMCMother();
    if(values[25] != 0){
      values[26]=fV0Reader->GetMotherMCParticlePDGCode();
      values[15]=fV0Reader->GetMotherMCParticle()->Pt();
    }
    fTotalNumberOfAddedNtupleEntries++;
    fGammaNtuple->Fill(values);
  }
  fV0Reader->ResetV0IndexNumber();
	
}

void AliAnalysisTaskGammaConversion::ProcessV0sNoCut(){
  // Process all the V0's without applying any cuts to it
	
  Int_t numberOfV0s = fV0Reader->GetNumberOfV0s();
  for(Int_t i=0;i<numberOfV0s;i++){
    /*AliESDv0 * cV0 = */fV0Reader->GetV0(i);
		
    if(fV0Reader->CheckForPrimaryVertex() == kFALSE){
      return;
    }
		
    if(fDoMCTruth){
			
      if(fV0Reader->HasSameMCMother() == kFALSE){
	continue;
      }
			
      TParticle * negativeMC = (TParticle*)fV0Reader->GetNegativeMCParticle();
      TParticle * positiveMC = (TParticle*)fV0Reader->GetPositiveMCParticle();
			
      if(TMath::Abs(negativeMC->GetPdgCode())!=11 || TMath::Abs(positiveMC->GetPdgCode())!=11){
	continue;
      }
      if(negativeMC->GetPdgCode()==positiveMC->GetPdgCode()){
	continue;
      }

      if(negativeMC->GetUniqueID() != 5 || positiveMC->GetUniqueID() !=5){
	continue;
      }
			
      if(fV0Reader->GetMotherMCParticle()->GetPdgCode() == 22){
				
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Pt", fV0Reader->GetMotherCandidatePt());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Energy", fV0Reader->GetMotherCandidateEnergy());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Eta", fV0Reader->GetMotherCandidateEta());				
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Phi", fV0Reader->GetMotherCandidatePhi());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Mass", fV0Reader->GetMotherCandidateMass());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Width", fV0Reader->GetMotherCandidateWidth());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Chi2", fV0Reader->GetMotherCandidateChi2());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_NDF", fV0Reader->GetMotherCandidateNDF());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Rapid", fV0Reader->GetMotherCandidateRapidity());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Pt_Eta", fV0Reader->GetMotherCandidatePt(),fV0Reader->GetMotherCandidateEta());
				
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Pt_Chi2", fV0Reader->GetMotherCandidatePt(), fV0Reader->GetMotherCandidateChi2());
	fHistograms->FillHistogram("ESD_NoCutConvGamma_Eta_Chi2", fV0Reader->GetMotherCandidateEta(), fV0Reader->GetMotherCandidateChi2());
				
	fHistograms->FillHistogram("ESD_NoCutConversion_XY", fV0Reader->GetX(),fV0Reader->GetY());
	fHistograms->FillHistogram("ESD_NoCutConversion_R", fV0Reader->GetXYRadius());
	fHistograms->FillHistogram("ESD_NoCutConversion_ZR", fV0Reader->GetZ(),fV0Reader->GetXYRadius());
	fHistograms->FillHistogram("ESD_NoCutConversion_OpeningAngle", fV0Reader->GetOpeningAngle());
				
	//store MCTruth properties
	fHistograms->FillHistogram("ESD_NoCutConvGamma_MC_Pt_Eta", fV0Reader->GetMotherMCParticle()->Pt(),fV0Reader->GetMotherMCParticle()->Eta());
	fHistograms->FillHistogram("ESD_NoCutConversion_MC_ZR", negativeMC->Vz(),negativeMC->R());
	fHistograms->FillHistogram("ESD_NoCutConversion_MC_XY", negativeMC->Vx(),negativeMC->Vy());
      }
    }
  }
  fV0Reader->ResetV0IndexNumber();
}

void AliAnalysisTaskGammaConversion::ProcessV0s(){
  // see header file for documentation
	
  if(fWriteNtuple == kTRUE){
    FillNtuple();
  }
	
  Int_t nSurvivingV0s=0;
  while(fV0Reader->NextV0()){
    nSurvivingV0s++;
		
		
    //-------------------------- filling v0 information -------------------------------------
    fHistograms->FillHistogram("ESD_Conversion_R", fV0Reader->GetXYRadius());
    fHistograms->FillHistogram("ESD_Conversion_ZR", fV0Reader->GetZ(),fV0Reader->GetXYRadius());
    fHistograms->FillHistogram("ESD_Conversion_XY", fV0Reader->GetX(),fV0Reader->GetY());
    fHistograms->FillHistogram("ESD_Conversion_OpeningAngle", fV0Reader->GetOpeningAngle());    
		
    fHistograms->FillHistogram("ESD_E_Energy", fV0Reader->GetNegativeTrackEnergy());
    fHistograms->FillHistogram("ESD_E_Pt", fV0Reader->GetNegativeTrackPt());
    fHistograms->FillHistogram("ESD_E_Eta", fV0Reader->GetNegativeTrackEta());
    fHistograms->FillHistogram("ESD_E_Phi", fV0Reader->GetNegativeTrackPhi());
		
    fHistograms->FillHistogram("ESD_P_Energy", fV0Reader->GetPositiveTrackEnergy());
    fHistograms->FillHistogram("ESD_P_Pt", fV0Reader->GetPositiveTrackPt());
    fHistograms->FillHistogram("ESD_P_Eta", fV0Reader->GetPositiveTrackEta());
    fHistograms->FillHistogram("ESD_P_Phi", fV0Reader->GetPositiveTrackPhi());
		
    fHistograms->FillHistogram("ESD_ConvGamma_Energy", fV0Reader->GetMotherCandidateEnergy());
    fHistograms->FillHistogram("ESD_ConvGamma_Pt", fV0Reader->GetMotherCandidatePt());
    fHistograms->FillHistogram("ESD_ConvGamma_Eta", fV0Reader->GetMotherCandidateEta());
    fHistograms->FillHistogram("ESD_ConvGamma_Phi", fV0Reader->GetMotherCandidatePhi());
    fHistograms->FillHistogram("ESD_ConvGamma_Mass", fV0Reader->GetMotherCandidateMass());
    fHistograms->FillHistogram("ESD_ConvGamma_Width", fV0Reader->GetMotherCandidateWidth());
    fHistograms->FillHistogram("ESD_ConvGamma_Chi2", fV0Reader->GetMotherCandidateChi2());
    fHistograms->FillHistogram("ESD_ConvGamma_NDF", fV0Reader->GetMotherCandidateNDF());
    fHistograms->FillHistogram("ESD_ConvGamma_Rapid", fV0Reader->GetMotherCandidateRapidity());
    fHistograms->FillHistogram("ESD_ConvGamma_Pt_Eta", fV0Reader->GetMotherCandidatePt(),fV0Reader->GetMotherCandidateEta());
		
    fHistograms->FillHistogram("ESD_ConvGamma_Pt_Chi2", fV0Reader->GetMotherCandidatePt(), fV0Reader->GetMotherCandidateChi2());
    fHistograms->FillHistogram("ESD_ConvGamma_Eta_Chi2", fV0Reader->GetMotherCandidateEta(), fV0Reader->GetMotherCandidateChi2());
		
		
    // begin mapping
    Int_t rBin    = fHistograms->GetRBin(fV0Reader->GetXYRadius());
    Int_t phiBin  = fHistograms->GetPhiBin(fV0Reader->GetNegativeTrackPhi());
    Double_t motherCandidateEta= fV0Reader->GetMotherCandidateEta();
		
    TString nameESDMappingPhiR="";
    nameESDMappingPhiR.Form("ESD_Conversion_Mapping_Phi%02d_R%02d",phiBin,rBin);
    fHistograms->FillHistogram(nameESDMappingPhiR, fV0Reader->GetZ(), motherCandidateEta);
		
    TString nameESDMappingPhi="";
    nameESDMappingPhi.Form("ESD_Conversion_Mapping_Phi%02d",phiBin);
    fHistograms->FillHistogram(nameESDMappingPhi, fV0Reader->GetZ(), motherCandidateEta);
		
    TString nameESDMappingR="";
    nameESDMappingR.Form("ESD_Conversion_Mapping_R%02d",rBin);
    fHistograms->FillHistogram(nameESDMappingR, fV0Reader->GetZ(), motherCandidateEta);  
		
    TString nameESDMappingPhiInR="";
    nameESDMappingPhiInR.Form("ESD_Conversion_Mapping_Phi_in_R_%02d",rBin);
    fHistograms->FillHistogram(nameESDMappingPhiInR, fV0Reader->GetMotherCandidatePhi());
    // end mapping
		
    new((*fKFReconstructedGammasTClone)[fKFReconstructedGammasTClone->GetEntriesFast()])  AliKFParticle(*fV0Reader->GetMotherCandidateKFCombination());
		
    //    fKFReconstructedGammas.push_back(*fV0Reader->GetMotherCandidateKFCombination());
    fElectronv1.push_back(fV0Reader->GetCurrentV0()->GetPindex());
    fElectronv2.push_back(fV0Reader->GetCurrentV0()->GetNindex());
		
		
    //----------------------------------- checking for "real" conversions (MC match) --------------------------------------
    if(fDoMCTruth){
			
      if(fV0Reader->HasSameMCMother() == kFALSE){
	continue;
      }
      TParticle * negativeMC = (TParticle*)fV0Reader->GetNegativeMCParticle();
      TParticle * positiveMC = (TParticle*)fV0Reader->GetPositiveMCParticle();

      if(TMath::Abs(negativeMC->GetPdgCode())!=11 || TMath::Abs(positiveMC->GetPdgCode())!=11){
	continue;
      }

      if(negativeMC->GetPdgCode()==positiveMC->GetPdgCode()){
	continue;
      }

      if(negativeMC->GetUniqueID() != 5 || positiveMC->GetUniqueID() !=5){// check if the daughters come from a conversion
	continue;
      }
			
      if(fV0Reader->GetMotherMCParticle()->GetPdgCode() == 22){

	fHistograms->FillHistogram("ESD_TrueConvGamma_Pt", fV0Reader->GetMotherCandidatePt());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Energy", fV0Reader->GetMotherCandidateEnergy());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Eta", fV0Reader->GetMotherCandidateEta());				
	fHistograms->FillHistogram("ESD_TrueConvGamma_Phi", fV0Reader->GetMotherCandidatePhi());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Mass", fV0Reader->GetMotherCandidateMass());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Width", fV0Reader->GetMotherCandidateWidth());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Chi2", fV0Reader->GetMotherCandidateChi2());
	fHistograms->FillHistogram("ESD_TrueConvGamma_NDF", fV0Reader->GetMotherCandidateNDF());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Pt_Eta", fV0Reader->GetMotherCandidatePt(),fV0Reader->GetMotherCandidateEta());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Rapid", fV0Reader->GetMotherCandidateRapidity());
	fHistograms->FillHistogram("ESD_TrueConvGamma_TrackLength", /*fV0Reader->GetNegativeTrackLength()*/fV0Reader->GetNegativeNTPCClusters());
	fHistograms->FillHistogram("ESD_TrueConvGamma_TrackLength", /*fV0Reader->GetPositiveTrackLength()*/fV0Reader->GetPositiveNTPCClusters());
	fHistograms->FillHistogram("ESD_TrueConvGamma_TrackLengthVSInvMass",/*fV0Reader->GetNegativeTrackLength()*/fV0Reader->GetNegativeNTPCClusters(),fV0Reader->GetMotherCandidateMass());
	fHistograms->FillHistogram("ESD_TrueConvGamma_TrackLengthVSInvMass",/*fV0Reader->GetPositiveTrackLength()*/fV0Reader->GetPositiveNTPCClusters(),fV0Reader->GetMotherCandidateMass());
				
	fHistograms->FillHistogram("ESD_TrueConvGamma_Pt_Chi2", fV0Reader->GetMotherCandidatePt(), fV0Reader->GetMotherCandidateChi2());
	fHistograms->FillHistogram("ESD_TrueConvGamma_Eta_Chi2", fV0Reader->GetMotherCandidateEta(), fV0Reader->GetMotherCandidateChi2());
				
				
	fHistograms->FillHistogram("ESD_TrueConversion_XY", fV0Reader->GetX(),fV0Reader->GetY());
	fHistograms->FillHistogram("ESD_TrueConversion_R", fV0Reader->GetXYRadius());
	fHistograms->FillHistogram("ESD_TrueConversion_ZR", fV0Reader->GetZ(),fV0Reader->GetXYRadius());
	fHistograms->FillHistogram("ESD_TrueConversion_OpeningAngle", fV0Reader->GetOpeningAngle());
				
	//store MCTruth properties
	fHistograms->FillHistogram("ESD_TrueConvGamma_MC_Pt_Eta", fV0Reader->GetMotherMCParticle()->Pt(),fV0Reader->GetMotherMCParticle()->Eta());
	fHistograms->FillHistogram("ESD_TrueConversion_MC_ZR", negativeMC->Vz(),negativeMC->R());
	fHistograms->FillHistogram("ESD_TrueConversion_MC_XY", negativeMC->Vx(),negativeMC->Vy());
				
	//resolution
	Double_t mcpt   = fV0Reader->GetMotherMCParticle()->Pt();
	Double_t esdpt  = fV0Reader->GetMotherCandidatePt();
	Double_t resdPt = 0;
	if(mcpt > 0){
	  resdPt = ((esdpt - mcpt)/mcpt)*100;
	}
	else if(mcpt < 0){
	  cout<<"Pt of MC particle is negative, this will cause wrong calculation of resPt"<<endl; 
	}
				
	fHistograms->FillHistogram("Resolution_dPt", mcpt, resdPt);
	fHistograms->FillHistogram("Resolution_MC_Pt", mcpt);
	fHistograms->FillHistogram("Resolution_ESD_Pt", esdpt);
				
	Double_t resdZ = 0;
	if(fV0Reader->GetNegativeMCParticle()->Vz() != 0){
	  resdZ = ((fV0Reader->GetZ() -fV0Reader->GetNegativeMCParticle()->Vz())/fV0Reader->GetNegativeMCParticle()->Vz())*100;
	}
				
	fHistograms->FillHistogram("Resolution_dZ", fV0Reader->GetNegativeMCParticle()->Vz(), resdZ);
	fHistograms->FillHistogram("Resolution_MC_Z", fV0Reader->GetNegativeMCParticle()->Vz());
	fHistograms->FillHistogram("Resolution_ESD_Z", fV0Reader->GetZ());
				
	Double_t resdR = 0;
	if(fV0Reader->GetNegativeMCParticle()->R() != 0){
	  resdR = ((fV0Reader->GetXYRadius() - fV0Reader->GetNegativeMCParticle()->R())/fV0Reader->GetNegativeMCParticle()->R())*100;
	}
				
	fHistograms->FillHistogram("Resolution_dR", fV0Reader->GetNegativeMCParticle()->R(), resdR);
	fHistograms->FillHistogram("Resolution_MC_R", fV0Reader->GetNegativeMCParticle()->R());
	fHistograms->FillHistogram("Resolution_ESD_R", fV0Reader->GetXYRadius());
	fHistograms->FillHistogram("Resolution_dR_dPt", resdR, resdPt);
      }//if(fV0Reader->GetMotherMCParticle()->GetPdgCode() == 22)
    }//if(fDoMCTruth)
  }//while(fV0Reader->NextV0)
  fHistograms->FillHistogram("ESD_NumberOfSurvivingV0s", nSurvivingV0s);
  fHistograms->FillHistogram("ESD_NumberOfV0s", fV0Reader->GetNumberOfV0s());
}

void AliAnalysisTaskGammaConversion::FillAODWithConversionGammas(){
  // Fill AOD with reconstructed Gamma
	
  for(Int_t gammaIndex=0;gammaIndex<fKFReconstructedGammasTClone->GetEntriesFast();gammaIndex++){
    //  for(UInt_t gammaIndex=0;gammaIndex<fKFReconstructedGammas.size();gammaIndex++){
    //Create AOD particle object from AliKFParticle
		
    /*    AliKFParticle * gammakf = &fKFReconstructedGammas[gammaIndex];
    //You could add directly AliKFParticle objects to the AOD, avoiding dependences with PartCorr
    //but this means that I have to work a little bit more in my side.
    //AODPWG4Particle objects are simpler and lighter, I think
    AliAODPWG4Particle gamma = AliAODPWG4Particle(gammakf->Px(),gammakf->Py(),gammakf->Pz(), gammakf->E());
    gamma.SetLabel(-1);//How to get the MC label of the reconstructed gamma?
    gamma.SetCaloLabel(-1,-1); //How to get the MC label of the 2 electrons that form the gamma?
    gamma.SetDetector("CTS"); //tag the gamma as reconstructed in the central barrel
    gamma.SetPdg(AliCaloPID::kPhotonConv); //photon id
    gamma.SetTag(-1); //Here I usually put a flag saying that montecarlo says it is prompt, decay fragmentation photon, or hadrons or whatever
		 
    //Add it to the aod list
    Int_t i = fAODBranch->GetEntriesFast();
    new((*fAODBranch)[i])  AliAODPWG4Particle(gamma);
    */
    //    AliKFParticle * gammakf = &fKFReconstructedGammas[gammaIndex];
    AliKFParticle * gammakf = (AliKFParticle *)fKFReconstructedGammasTClone->At(gammaIndex);
    AliGammaConversionAODObject aodObject;
    aodObject.SetPx(gammakf->GetPx());
    aodObject.SetPy(gammakf->GetPy());
    aodObject.SetPz(gammakf->GetPz());
    aodObject.SetLabel1(fElectronv1[gammaIndex]);
    aodObject.SetLabel2(fElectronv2[gammaIndex]);
    Int_t i = fAODBranch->GetEntriesFast();
    new((*fAODBranch)[i])  AliGammaConversionAODObject(aodObject);
  }
	
}


void AliAnalysisTaskGammaConversion::ProcessGammasForNeutralMesonAnalysis(){
  // see header file for documentation
	
  //  for(UInt_t firstGammaIndex=0;firstGammaIndex<fKFReconstructedGammas.size();firstGammaIndex++){
  //    for(UInt_t secondGammaIndex=firstGammaIndex+1;secondGammaIndex<fKFReconstructedGammas.size();secondGammaIndex++){
  for(Int_t firstGammaIndex=0;firstGammaIndex<fKFReconstructedGammasTClone->GetEntriesFast();firstGammaIndex++){
    for(Int_t secondGammaIndex=firstGammaIndex+1;secondGammaIndex<fKFReconstructedGammasTClone->GetEntriesFast();secondGammaIndex++){
			
      //      AliKFParticle * twoGammaDecayCandidateDaughter0 = &fKFReconstructedGammas[firstGammaIndex];
      //      AliKFParticle * twoGammaDecayCandidateDaughter1 = &fKFReconstructedGammas[secondGammaIndex];
			
      AliKFParticle * twoGammaDecayCandidateDaughter0 = (AliKFParticle *)fKFReconstructedGammasTClone->At(firstGammaIndex);
      AliKFParticle * twoGammaDecayCandidateDaughter1 = (AliKFParticle *)fKFReconstructedGammasTClone->At(secondGammaIndex);
			
      if(fElectronv1[firstGammaIndex]==fElectronv1[secondGammaIndex] || fElectronv1[firstGammaIndex]==fElectronv2[secondGammaIndex]){
	continue;
      }
      if(fElectronv2[firstGammaIndex]==fElectronv1[secondGammaIndex] || fElectronv2[firstGammaIndex]==fElectronv2[secondGammaIndex]){
	continue;
      }
			
      AliKFParticle *twoGammaCandidate = new AliKFParticle(*twoGammaDecayCandidateDaughter0,*twoGammaDecayCandidateDaughter1);
			
      Double_t massTwoGammaCandidate = 0.;
      Double_t widthTwoGammaCandidate = 0.;
      Double_t chi2TwoGammaCandidate =10000.;	
      twoGammaCandidate->GetMass(massTwoGammaCandidate,widthTwoGammaCandidate);
      if(twoGammaCandidate->GetNDF()>0){
	chi2TwoGammaCandidate = twoGammaCandidate->GetChi2()/twoGammaCandidate->GetNDF();
				
	if(chi2TwoGammaCandidate>0 && chi2TwoGammaCandidate<fV0Reader->GetChi2CutMeson()){
					
	  TVector3 momentumVectorTwoGammaCandidate(twoGammaCandidate->GetPx(),twoGammaCandidate->GetPy(),twoGammaCandidate->GetPz());
	  TVector3 spaceVectorTwoGammaCandidate(twoGammaCandidate->GetX(),twoGammaCandidate->GetY(),twoGammaCandidate->GetZ());
					
	  Double_t openingAngleTwoGammaCandidate = twoGammaDecayCandidateDaughter0->GetAngle(*twoGammaDecayCandidateDaughter1);					
	  Double_t rapidity;
	  if(twoGammaCandidate->GetE() - twoGammaCandidate->GetPz() == 0 || twoGammaCandidate->GetE() + twoGammaCandidate->GetPz() == 0){
	    rapidity=0;
	  }
	  else{
	    rapidity = 0.5*(TMath::Log((twoGammaCandidate->GetE() +twoGammaCandidate->GetPz()) / (twoGammaCandidate->GetE()-twoGammaCandidate->GetPz())));
	  }
					
	  if(openingAngleTwoGammaCandidate < fMinOpeningAngleGhostCut) continue;   // minimum opening angle to avoid using ghosttracks
					
	  fHistograms->FillHistogram("ESD_Mother_GammaDaughter_OpeningAngle", openingAngleTwoGammaCandidate);
	  fHistograms->FillHistogram("ESD_Mother_Energy", twoGammaCandidate->GetE());
	  fHistograms->FillHistogram("ESD_Mother_Pt", momentumVectorTwoGammaCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Mother_Eta", momentumVectorTwoGammaCandidate.Eta());
	  fHistograms->FillHistogram("ESD_Mother_Rapid", rapidity);					
	  fHistograms->FillHistogram("ESD_Mother_Phi", spaceVectorTwoGammaCandidate.Phi());
	  fHistograms->FillHistogram("ESD_Mother_Mass", massTwoGammaCandidate);
	  fHistograms->FillHistogram("ESD_Mother_R", spaceVectorTwoGammaCandidate.Pt());    // Pt in Space == R!!!
	  fHistograms->FillHistogram("ESD_Mother_ZR", twoGammaCandidate->GetZ(), spaceVectorTwoGammaCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Mother_XY", twoGammaCandidate->GetX(), twoGammaCandidate->GetY());
	  fHistograms->FillHistogram("ESD_Mother_InvMass_vs_Pt",massTwoGammaCandidate ,momentumVectorTwoGammaCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Mother_InvMass",massTwoGammaCandidate);
	}
      }
      delete twoGammaCandidate;
    }
  }
}

void AliAnalysisTaskGammaConversion::CalculateBackground(){
  // see header file for documentation
	
  vector<AliKFParticle> vectorCurrentEventGoodV0s = fV0Reader->GetCurrentEventGoodV0s();
  vector<AliKFParticle> vectorPreviousEventGoodV0s = fV0Reader->GetPreviousEventGoodV0s();
	
  for(UInt_t iCurrent=0;iCurrent<vectorCurrentEventGoodV0s.size();iCurrent++){
    AliKFParticle * currentEventGoodV0 = &vectorCurrentEventGoodV0s.at(iCurrent);
    for(UInt_t iPrevious=0;iPrevious<vectorPreviousEventGoodV0s.size();iPrevious++){
      AliKFParticle * previousGoodV0 = &vectorPreviousEventGoodV0s.at(iPrevious);
			
      AliKFParticle *backgroundCandidate = new AliKFParticle(*currentEventGoodV0,*previousGoodV0);
			
      Double_t massBG =0.;
      Double_t widthBG = 0.;
      Double_t chi2BG =10000.;	
      backgroundCandidate->GetMass(massBG,widthBG);
      if(backgroundCandidate->GetNDF()>0){
	chi2BG = backgroundCandidate->GetChi2()/backgroundCandidate->GetNDF();
	if(chi2BG>0 && chi2BG<fV0Reader->GetChi2CutMeson()){
					
	  TVector3 momentumVectorbackgroundCandidate(backgroundCandidate->GetPx(),backgroundCandidate->GetPy(),backgroundCandidate->GetPz());
	  TVector3 spaceVectorbackgroundCandidate(backgroundCandidate->GetX(),backgroundCandidate->GetY(),backgroundCandidate->GetZ());
					
	  Double_t openingAngleBG = currentEventGoodV0->GetAngle(*previousGoodV0);
					
	  Double_t rapidity;
	  if(backgroundCandidate->GetE() - backgroundCandidate->GetPz() == 0 || backgroundCandidate->GetE() + backgroundCandidate->GetPz() == 0) rapidity=0;
	  else rapidity = 0.5*(TMath::Log((backgroundCandidate->GetE() +backgroundCandidate->GetPz()) / (backgroundCandidate->GetE()-backgroundCandidate->GetPz())));
					
					
					
					
	  if(openingAngleBG < fMinOpeningAngleGhostCut ) continue;   // minimum opening angle to avoid using ghosttracks
					
					
	  fHistograms->FillHistogram("ESD_Background_GammaDaughter_OpeningAngle", openingAngleBG);
	  fHistograms->FillHistogram("ESD_Background_Energy", backgroundCandidate->GetE());
	  fHistograms->FillHistogram("ESD_Background_Pt",  momentumVectorbackgroundCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Background_Eta", momentumVectorbackgroundCandidate.Eta());
	  fHistograms->FillHistogram("ESD_Background_Rapidity", rapidity);
	  fHistograms->FillHistogram("ESD_Background_Phi", spaceVectorbackgroundCandidate.Phi());
	  fHistograms->FillHistogram("ESD_Background_Mass", massBG);
	  fHistograms->FillHistogram("ESD_Background_R", spaceVectorbackgroundCandidate.Pt());  // Pt in Space == R!!!!
	  fHistograms->FillHistogram("ESD_Background_ZR", backgroundCandidate->GetZ(), spaceVectorbackgroundCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Background_XY", backgroundCandidate->GetX(), backgroundCandidate->GetY());
	  fHistograms->FillHistogram("ESD_Background_InvMass_vs_Pt",massBG,momentumVectorbackgroundCandidate.Pt());
	  fHistograms->FillHistogram("ESD_Background_InvMass",massBG);
	}
      }
      delete backgroundCandidate;   
    }
  }
}



void AliAnalysisTaskGammaConversion::ProcessGammasForGammaJetAnalysis(){
  //ProcessGammasForGammaJetAnalysis
	
  Double_t distIsoMin;
	
  CreateListOfChargedParticles();
	
	
  //  for(UInt_t gammaIndex=0;gammaIndex<fKFReconstructedGammas.size();gammaIndex++){
  for(Int_t gammaIndex=0;gammaIndex<fKFReconstructedGammasTClone->GetEntriesFast();gammaIndex++){
    AliKFParticle * currentGamma = (AliKFParticle*)fKFReconstructedGammasTClone->At(gammaIndex);
    TVector3 momentumVectorCurrentGamma(currentGamma->GetPx(),currentGamma->GetPy(),currentGamma->GetPz());
		
    if( momentumVectorCurrentGamma.Pt()> fMinPtForGammaJet){
      distIsoMin=GetMinimumDistanceToCharge(gammaIndex);
			
      if (distIsoMin > fMinIsoConeSize && fLeadingChargedIndex>=0){
	CalculateJetCone(gammaIndex);
      }
    }
  }
}

void AliAnalysisTaskGammaConversion::CreateListOfChargedParticles(){
  // CreateListOfChargedParticles
	
  fESDEvent = fV0Reader->GetESDEvent();
  for(Int_t iTracks = 0; iTracks < fESDEvent->GetNumberOfTracks(); iTracks++){
    AliESDtrack* curTrack = fESDEvent->GetTrack(iTracks);
		
    if(!curTrack){
      continue;
    }
		
    if(fEsdTrackCuts->AcceptTrack(curTrack) ){
      new((*fChargedParticles)[fChargedParticles->GetEntriesFast()])  AliESDtrack(*curTrack);
      //      fChargedParticles.push_back(curTrack);
      fChargedParticlesId.push_back(iTracks);
    }
  }
}
void AliAnalysisTaskGammaConversion::CalculateJetCone(Int_t gammaIndex){
  // CaculateJetCone
	
  Double_t cone;
  Double_t coneSize=0.3;
  Double_t ptJet=0;
	
  //  AliKFParticle * currentGamma = &fKFReconstructedGammas[gammaIndex];
  AliKFParticle * currentGamma = (AliKFParticle*)fKFReconstructedGammasTClone->At(gammaIndex);

  TVector3 momentumVectorCurrentGamma(currentGamma->GetPx(),currentGamma->GetPy(),currentGamma->GetPz());
	
  AliESDtrack* leadingCharged = (AliESDtrack*)(fChargedParticles->At(fLeadingChargedIndex));

  Double_t momLeadingCharged[3];
  leadingCharged->GetConstrainedPxPyPz(momLeadingCharged);
	
  TVector3 momentumVectorLeadingCharged(momLeadingCharged[0],momLeadingCharged[1],momLeadingCharged[2]);
	
  Double_t phi1=momentumVectorLeadingCharged.Phi();
  Double_t eta1=momentumVectorLeadingCharged.Eta();
  Double_t phi3=momentumVectorCurrentGamma.Phi();
	
  for(Int_t iCh=0;iCh<fChargedParticles->GetEntriesFast();iCh++){
    AliESDtrack* curTrack = (AliESDtrack*)(fChargedParticles->At(iCh));
    Int_t chId = fChargedParticlesId[iCh];
    if(fLeadingChargedIndex==chId || fLeadingChargedIndex==chId) continue;
    Double_t mom[3];
    curTrack->GetConstrainedPxPyPz(mom);
    TVector3 momentumVectorChargedParticle(mom[0],mom[1],mom[2]);
    Double_t phi2=momentumVectorChargedParticle.Phi();
    Double_t eta2=momentumVectorChargedParticle.Eta();
		
		
    cone=100.;
    if( TMath::Abs(phi2 - phi1) <= ( TMath::TwoPi()-coneSize) ){
      cone = TMath::Sqrt(  TMath::Power((eta2-eta1),2)+ TMath::Power((phi2-phi1),2) );
    }else{
      if( (phi2 - phi1)> TMath::TwoPi()-coneSize ){
	cone = TMath::Sqrt(  TMath::Power((eta2-eta1),2)+ TMath::Power((phi2-TMath::TwoPi()-phi1),2) );
      }
      if( (phi2 - phi1)< -(TMath::TwoPi()-coneSize) ){
	cone = TMath::Sqrt(  TMath::Power((eta2-eta1),2)+ TMath::Power((phi2+TMath::TwoPi()-phi1),2) );
      }
    }
		
    if(cone <coneSize&& momentumVectorChargedParticle.Pt()>fMinPtJetCone ){
      ptJet+= momentumVectorChargedParticle.Pt();
      Double_t ffzHdrGam = momentumVectorChargedParticle.Pt()/momentumVectorCurrentGamma.Pt();
      Double_t imbalanceHdrGam=-momentumVectorChargedParticle.Dot(momentumVectorCurrentGamma)/momentumVectorCurrentGamma.Mag2();
      fHistograms->FillHistogram("ESD_FFzHdrGam",ffzHdrGam);
      fHistograms->FillHistogram("ESD_ImbalanceHdrGam",imbalanceHdrGam);
			
    }
		
    Double_t dphiHdrGam=phi3-phi2;
    if ( dphiHdrGam < (-TMath::PiOver2())){
      dphiHdrGam+=(TMath::TwoPi());
    }
		
    if ( dphiHdrGam > (3.*TMath::PiOver2()) ){
      dphiHdrGam-=(TMath::TwoPi());
    }
		
    if (momentumVectorChargedParticle.Pt()>fMinPtGamChargedCorr){
      fHistograms->FillHistogram("ESD_dphiHdrGamIsolated",dphiHdrGam);
    }
  }//track loop
	
	
}

Double_t AliAnalysisTaskGammaConversion::GetMinimumDistanceToCharge(Int_t indexHighestPtGamma){
  // GetMinimumDistanceToCharge
	
  Double_t fIsoMin=100.;
  Double_t ptLeadingCharged=-1.;

  fLeadingChargedIndex=-1;
	
  AliKFParticle * gammaHighestPt = (AliKFParticle*)fKFReconstructedGammasTClone->At(indexHighestPtGamma);
  TVector3 momentumVectorgammaHighestPt(gammaHighestPt->GetPx(),gammaHighestPt->GetPy(),gammaHighestPt->GetPz());
	
  Double_t phi1=momentumVectorgammaHighestPt.Phi();
  Double_t eta1=momentumVectorgammaHighestPt.Eta();
	
  for(Int_t iCh=0;iCh<fChargedParticles->GetEntriesFast();iCh++){
    AliESDtrack* curTrack = (AliESDtrack*)(fChargedParticles->At(iCh));
    Int_t chId = fChargedParticlesId[iCh];
    if(fElectronv1[indexHighestPtGamma]==chId || fElectronv2[indexHighestPtGamma]==chId) continue;
    Double_t mom[3];
    curTrack->GetConstrainedPxPyPz(mom);
    TVector3 momentumVectorChargedParticle(mom[0],mom[1],mom[2]);
    Double_t phi2=momentumVectorChargedParticle.Phi();
    Double_t eta2=momentumVectorChargedParticle.Eta();
    Double_t iso=pow(  (pow( (eta1-eta2),2)+ pow((phi1-phi2),2)),0.5 );
		
    if(momentumVectorChargedParticle.Pt()>fMinPtIsoCone ){
      if (iso<fIsoMin){
	fIsoMin=iso;
      }
    }
		
    Double_t dphiHdrGam=phi1-phi2;
    if ( dphiHdrGam < (-TMath::PiOver2())){
      dphiHdrGam+=(TMath::TwoPi());
    }
		
    if ( dphiHdrGam > (3.*TMath::PiOver2()) ){
      dphiHdrGam-=(TMath::TwoPi());
    }
    if (momentumVectorChargedParticle.Pt()>fMinPtGamChargedCorr){
      fHistograms->FillHistogram("ESD_dphiHdrGam",dphiHdrGam);
    }
		
    if (dphiHdrGam>0.9*TMath::Pi() && dphiHdrGam<1.1*TMath::Pi()){
      if (momentumVectorChargedParticle.Pt()> ptLeadingCharged && momentumVectorChargedParticle.Pt()>0.1*momentumVectorgammaHighestPt.Pt()){
	ptLeadingCharged=momentumVectorChargedParticle.Pt();
	fLeadingChargedIndex=iCh;
      }
    }
		
  }//track loop
  fHistograms->FillHistogram("ESD_MinimumIsoDistance",fIsoMin);
  return fIsoMin;
	
}

Int_t  AliAnalysisTaskGammaConversion::GetIndexHighestPtGamma(){
  //GetIndexHighestPtGamma
	
  Int_t indexHighestPtGamma=-1;
  //Double_t 
  fGammaPtHighest = -100.;
	
  for(Int_t firstGammaIndex=0;firstGammaIndex<fKFReconstructedGammasTClone->GetEntriesFast();firstGammaIndex++){
    AliKFParticle * gammaHighestPtCandidate = (AliKFParticle*)fKFReconstructedGammasTClone->At(firstGammaIndex);
    TVector3 momentumVectorgammaHighestPtCandidate(gammaHighestPtCandidate->GetPx(),gammaHighestPtCandidate->GetPy(),gammaHighestPtCandidate->GetPz());
    if (momentumVectorgammaHighestPtCandidate.Pt() > fGammaPtHighest){
      fGammaPtHighest=momentumVectorgammaHighestPtCandidate.Pt();
      //gammaHighestPt = gammaHighestPtCandidate;
      indexHighestPtGamma=firstGammaIndex;
    }
  }
	
  return indexHighestPtGamma;
	
}


void AliAnalysisTaskGammaConversion::Terminate(Option_t */*option*/)
{
  // Terminate analysis
  //
  AliDebug(1,"Do nothing in Terminate");
}

void AliAnalysisTaskGammaConversion::UserCreateOutputObjects()
{
  //AOD
  fAODBranch = new TClonesArray("AliGammaConversionAODObject", 0);
  fAODBranch->SetName(fAODBranchName); 
  AddAODBranch("TClonesArray", &fAODBranch);
	
  // Create the output container
  if(fOutputContainer != NULL){
    delete fOutputContainer;
    fOutputContainer = NULL;
  }
  if(fOutputContainer == NULL){
    fOutputContainer = new TList();
  }
	
  //Adding the histograms to the output container
  fHistograms->GetOutputContainer(fOutputContainer);
	
	
  if(fWriteNtuple){
    if(fGammaNtuple == NULL){
      fGammaNtuple = new TNtuple("V0ntuple","V0ntuple","OnTheFly:HasVertex:NegPIDProb:PosPIDProb:X:Y:Z:R:MotherCandidateNDF:MotherCandidateChi2:MotherCandidateEnergy:MotherCandidateEta:MotherCandidatePt:MotherCandidateMass:MotherCandidateWidth:MCMotherCandidatePT:EPOpeningAngle:ElectronEnergy:ElectronPt:ElectronEta:ElectronPhi:PositronEnergy:PositronPt:PositronEta:PositronPhi:HasSameMCMother:MotherMCParticlePIDCode",50000);
    }
    if(fNeutralMesonNtuple == NULL){
      fNeutralMesonNtuple = new TNtuple("NeutralMesonNtuple","NeutralMesonNtuple","test");
    }
    TList * ntupleTList = new TList();
    ntupleTList->SetName("Ntuple");
    ntupleTList->Add((TNtuple*)fGammaNtuple);
    fOutputContainer->Add(ntupleTList);
  }
	
  fOutputContainer->SetName(GetName());
}

Double_t AliAnalysisTaskGammaConversion::GetMCOpeningAngle(TParticle* const daughter0, TParticle* const daughter1) const{
  //helper function
  TVector3 v3D0(daughter0->Px(),daughter0->Py(),daughter0->Pz());
  TVector3 v3D1(daughter1->Px(),daughter1->Py(),daughter1->Pz());
  return v3D0.Angle(v3D1);
}

void AliAnalysisTaskGammaConversion::CheckV0Efficiency(){
  // see header file for documentation
	
  vector<Int_t> indexOfGammaParticle;
	
  fStack = fV0Reader->GetMCStack();
	
  if(fV0Reader->CheckForPrimaryVertex() == kFALSE){
    return; // aborts if the primary vertex does not have contributors.
  }
	
  for (Int_t iTracks = 0; iTracks < fStack->GetNprimary(); iTracks++) {
    TParticle* particle = (TParticle *)fStack->Particle(iTracks);
    if(particle->GetPdgCode()==22){     //Gamma
      if(particle->GetNDaughters() >= 2){
	TParticle* electron=NULL;
	TParticle* positron=NULL; 
	for(Int_t daughterIndex=particle->GetFirstDaughter();daughterIndex<=particle->GetLastDaughter();daughterIndex++){
	  TParticle *tmpDaughter = fStack->Particle(daughterIndex);
	  if(tmpDaughter->GetUniqueID() == 5){
	    if(tmpDaughter->GetPdgCode() == 11){
	      electron = tmpDaughter;
	    }
	    else if(tmpDaughter->GetPdgCode() == -11){
	      positron = tmpDaughter;
	    }
	  }
	}
	if(electron!=NULL && positron!=0){
	  if(electron->R()<160){
	    indexOfGammaParticle.push_back(iTracks);
	  }
	}
      }
    }
  }
	
  Int_t nFoundGammas=0;
  Int_t nNotFoundGammas=0;
	
  Int_t numberOfV0s = fV0Reader->GetNumberOfV0s();
  for(Int_t i=0;i<numberOfV0s;i++){
    fV0Reader->GetV0(i);
		
    if(fV0Reader->HasSameMCMother() == kFALSE){
      continue;
    }
		
    TParticle * negativeMC = (TParticle*)fV0Reader->GetNegativeMCParticle();
    TParticle * positiveMC = (TParticle*)fV0Reader->GetPositiveMCParticle();
		
    if(TMath::Abs(negativeMC->GetPdgCode())!=11 || TMath::Abs(positiveMC->GetPdgCode())!=11){
      continue;
    }
    if(negativeMC->GetPdgCode()==positiveMC->GetPdgCode()){
      continue;
    }
		
    if(fV0Reader->GetMotherMCParticle()->GetPdgCode() == 22){
      //TParticle * v0Gamma = fV0Reader->GetMotherMCParticle();
      for(UInt_t mcIndex=0;mcIndex<indexOfGammaParticle.size();mcIndex++){
	if(negativeMC->GetFirstMother()==indexOfGammaParticle[mcIndex]){
	  nFoundGammas++;
	}
	else{
	  nNotFoundGammas++;
	}
      }
    }
  }
}


void AliAnalysisTaskGammaConversion::ProcessGammaElectronsForChicAnalysis(){
  // see header file for documantation
	
  fESDEvent = fV0Reader->GetESDEvent();
	
	
  TClonesArray * vESDeNegTemp = new TClonesArray("AliESDtrack",0);
  TClonesArray * vESDePosTemp = new TClonesArray("AliESDtrack",0);
  TClonesArray * vESDxNegTemp = new TClonesArray("AliESDtrack",0);
  TClonesArray * vESDxPosTemp = new TClonesArray("AliESDtrack",0);
  TClonesArray * vESDeNegNoJPsi = new TClonesArray("AliESDtrack",0);
  TClonesArray * vESDePosNoJPsi = new TClonesArray("AliESDtrack",0);
	
  /*
    vector <AliESDtrack*> vESDeNegTemp(0);
    vector <AliESDtrack*> vESDePosTemp(0);
    vector <AliESDtrack*> vESDxNegTemp(0);
    vector <AliESDtrack*> vESDxPosTemp(0);
    vector <AliESDtrack*> vESDeNegNoJPsi(0);
    vector <AliESDtrack*> vESDePosNoJPsi(0); 
  */
	
	
  fHistograms->FillTable("Table_Electrons",0);//Count number of Events
	
  for(Int_t iTracks = 0; iTracks < fESDEvent->GetNumberOfTracks(); iTracks++){
    AliESDtrack* curTrack = fESDEvent->GetTrack(iTracks);
		
    if(!curTrack){
      //print warning here
      continue;
    }
		
    double p[3];if(!curTrack->GetConstrainedPxPyPz(p))continue;
    double r[3];curTrack->GetConstrainedXYZ(r);
		
    TVector3 rXYZ(r);
		
    fHistograms->FillTable("Table_Electrons",4);//Count number of ESD tracks
		
    Bool_t flagKink       =  kTRUE;
    Bool_t flagTPCrefit   =  kTRUE;
    Bool_t flagTRDrefit   =  kTRUE;
    Bool_t flagITSrefit   =  kTRUE;
    Bool_t flagTRDout     =  kTRUE;
    Bool_t flagVertex     =  kTRUE;
		
		
    //Cuts ---------------------------------------------------------------
		
    if(curTrack->GetKinkIndex(0) > 0){
      fHistograms->FillHistogram("Table_Electrons",5);//Count kink
      flagKink = kFALSE;
    }
		
    ULong_t trkStatus = curTrack->GetStatus();
		
    ULong_t tpcRefit = (trkStatus & AliESDtrack::kTPCrefit);
		
    if(!tpcRefit){
      fHistograms->FillHistogram("Table_Electrons",9);//Count not TPCrefit
      flagTPCrefit = kFALSE;
    }
		
    ULong_t itsRefit = (trkStatus & AliESDtrack::kITSrefit);
    if(!itsRefit){
      fHistograms->FillHistogram("Table_Electrons",10);//Count not ITSrefit
      flagITSrefit = kFALSE;
    }
		
    ULong_t trdRefit = (trkStatus & AliESDtrack::kTRDrefit);
		
    if(!trdRefit){
      fHistograms->FillHistogram("Table_Electrons",8); //Count not TRDrefit
      flagTRDrefit = kFALSE;
    }
		
    ULong_t trdOut = (trkStatus & AliESDtrack::kTRDout);
		
    if(!trdOut) {
      fHistograms->FillHistogram("Table_Electrons",7); //Count not TRDout
      flagTRDout = kFALSE;
    }
		
    double nsigmaToVxt = GetSigmaToVertex(curTrack);
		
    if(nsigmaToVxt > 3){
      fHistograms->FillHistogram("Table_Electrons",6); //Count Tracks with number of sigmas > 3
      flagVertex = kFALSE;
    }
		
    if(! (flagKink && flagTPCrefit && flagITSrefit && flagTRDrefit && flagTRDout && flagVertex ) ) continue;
    fHistograms->FillHistogram("Table_Electrons",11);//Count Tracks passed Cuts
		
		
    Stat_t pid, weight;
    GetPID(curTrack, pid, weight);
		
    if(pid!=0){
      fHistograms->FillHistogram("Table_Electrons",12); //Count Tracks with pid != 0
    }
		
    if(pid == 0){
      fHistograms->FillHistogram("Table_Electrons",13); //Count Tracks with pid != 0
    }
		
		
		
		
    Int_t labelMC = TMath::Abs(curTrack->GetLabel());
    TParticle* curParticle = fStack->Particle(labelMC);
		
		
		
		
    TLorentzVector curElec;
    curElec.SetXYZM(p[0],p[1],p[2],fElectronMass);
		
		
		
		
    if(curTrack->GetSign() > 0){
			
      //     vESDxPosTemp.push_back(curTrack);
      new((*vESDxPosTemp)[vESDxPosTemp->GetEntriesFast()])  AliESDtrack(*curTrack);
			
      if( pid == 0){
				
	fHistograms->FillHistogram("ESD_ElectronPosNegPt",curElec.Pt());
	fHistograms->FillHistogram("ESD_ElectronPosPt",curElec.Pt());
	fHistograms->FillHistogram("MC_ElectronPosNegPt",curParticle->Pt());
	fHistograms->FillHistogram("ESD_ElectronPosNegEta",curElec.Eta());
	fHistograms->FillHistogram("MC_ElectronPosNegEta",curParticle->Eta());
	//	vESDePosTemp.push_back(curTrack);
	new((*vESDePosTemp)[vESDePosTemp->GetEntriesFast()])  AliESDtrack(*curTrack);
				
      }
			
    }
    else {
      //      vESDxNegTemp.push_back(curTrack);
      /*			if(vESDxNegTemp == NULL){
				cout<<"TCloes is zero"<<endl;
				}
				if(curTrack == NULL){
				cout<<"curTrack is zero"<<endl;
				}
      */	
      new((*vESDxNegTemp)[vESDxNegTemp->GetEntriesFast()])  AliESDtrack(*curTrack);
			
      if( pid == 0){
				
	fHistograms->FillHistogram("ESD_ElectronPosNegPt",curElec.Pt());
	fHistograms->FillHistogram("ESD_ElectronNegPt",curElec.Pt());
	fHistograms->FillHistogram("MC_ElectronPosNegPt",curParticle->Pt());
	fHistograms->FillHistogram("ESD_ElectronPosNegEta",curElec.Eta());
	fHistograms->FillHistogram("MC_ElectronPosNegEta",curParticle->Eta());
	//vESDeNegTemp.push_back(curTrack);
	new((*vESDeNegTemp)[vESDeNegTemp->GetEntriesFast()])  AliESDtrack(*curTrack);
				
      }
			
    }
		
  }
	
	
  Bool_t ePosJPsi = kFALSE;
  Bool_t eNegJPsi = kFALSE;		
  Bool_t ePosPi0  = kFALSE;
  Bool_t eNegPi0  = kFALSE;
	
  UInt_t iePosJPsi=0,ieNegJPsi=0,iePosPi0=0,ieNegPi0=0;
	
  for(Int_t iNeg=0; iNeg < vESDeNegTemp->GetEntriesFast(); iNeg++){
    if(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDeNegTemp->At(iNeg)))->GetLabel()))->GetPdgCode() == 11)
      if(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDeNegTemp->At(iNeg)))->GetLabel()))->GetMother(0) > -1){
	Int_t labelMother = fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDeNegTemp->At(iNeg)))->GetLabel()))->GetMother(0);
	TParticle* partMother = fStack ->Particle(labelMother);
	if (partMother->GetPdgCode() == 111){
	  ieNegPi0 = iNeg;
	  eNegPi0 = kTRUE;
	}
	if(partMother->GetPdgCode() == 443){ //Mother JPsi
	  fHistograms->FillTable("Table_Electrons",14);
	  ieNegJPsi = iNeg;
	  eNegJPsi = kTRUE;
	}
	else{	
	  //	  vESDeNegNoJPsi.push_back(vESDeNegTemp[iNeg]);
	  new((*vESDeNegNoJPsi)[vESDeNegNoJPsi->GetEntriesFast()])  AliESDtrack(*(AliESDtrack*)(vESDeNegTemp->At(iNeg)));
	  //		cout<<"ESD No Positivo JPsi "<<endl;
	}
				
      }
  }	
	
  for(Int_t iPos=0; iPos < vESDePosTemp->GetEntriesFast(); iPos++){
    if(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDePosTemp->At(iPos)))->GetLabel()))->GetPdgCode() == -11)
      if(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDePosTemp->At(iPos)))->GetLabel()))->GetMother(0) > -1){
	Int_t labelMother = fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDePosTemp->At(iPos)))->GetLabel()))->GetMother(0);
	TParticle* partMother = fStack ->Particle(labelMother);
	if (partMother->GetPdgCode() == 111){
	  iePosPi0 = iPos;
	  ePosPi0 = kTRUE;
	}
	if(partMother->GetPdgCode() == 443){ //Mother JPsi
	  fHistograms->FillTable("Table_Electrons",15);
	  iePosJPsi = iPos;
	  ePosJPsi = kTRUE;
	}
	else{
	  //	  vESDePosNoJPsi.push_back(vESDePosTemp[iPos]);
	  new((*vESDePosNoJPsi)[vESDePosNoJPsi->GetEntriesFast()])  AliESDtrack(*(AliESDtrack*)(vESDePosTemp->At(iPos)));	  
	  //		cout<<"ESD No Negativo JPsi "<<endl;
	}
				
      }
  }
	
  if( eNegJPsi && ePosJPsi ){
    TVector3 tempeNegV,tempePosV;
    tempeNegV.SetXYZ(((AliESDtrack*)(vESDeNegTemp->At(ieNegJPsi)))->Px(),((AliESDtrack*)(vESDeNegTemp->At(ieNegJPsi)))->Py(),((AliESDtrack*)(vESDeNegTemp->At(ieNegJPsi)))->Pz());			
    tempePosV.SetXYZ(((AliESDtrack*)(vESDePosTemp->At(iePosJPsi)))->Px(),((AliESDtrack*)(vESDePosTemp->At(iePosJPsi)))->Py(),((AliESDtrack*)(vESDePosTemp->At(iePosJPsi)))->Pz());
    fHistograms->FillTable("Table_Electrons",16);
    fHistograms->FillHistogram("ESD_ElectronPosNegJPsiAngle",tempeNegV.Angle(tempePosV));	
    fHistograms->FillHistogram("MC_ElectronPosNegJPsiAngle",GetMCOpeningAngle(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDeNegTemp->At(ieNegJPsi)))->GetLabel())),
									      fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDePosTemp->At(iePosJPsi)))->GetLabel()))));	
  }
	
  if( eNegPi0 && ePosPi0 ){
    TVector3 tempeNegV,tempePosV;
    tempeNegV.SetXYZ(((AliESDtrack*)(vESDeNegTemp->At(ieNegPi0)))->Px(),((AliESDtrack*)(vESDeNegTemp->At(ieNegPi0)))->Py(),((AliESDtrack*)(vESDeNegTemp->At(ieNegPi0)))->Pz());
    tempePosV.SetXYZ(((AliESDtrack*)(vESDePosTemp->At(iePosPi0)))->Px(),((AliESDtrack*)(vESDePosTemp->At(iePosPi0)))->Py(),((AliESDtrack*)(vESDePosTemp->At(iePosPi0)))->Pz());
    fHistograms->FillHistogram("ESD_ElectronPosNegPi0Angle",tempeNegV.Angle(tempePosV));
    fHistograms->FillHistogram("MC_ElectronPosNegPi0Angle",GetMCOpeningAngle(fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDeNegTemp->At(ieNegPi0)))->GetLabel())),
									     fStack->Particle(TMath::Abs(((AliESDtrack*)(vESDePosTemp->At(iePosPi0)))->GetLabel()))));   
  }
	
	
  FillAngle("ESD_eNegePosAngleBeforeCut",GetTLorentzVector(vESDeNegTemp),GetTLorentzVector(vESDePosTemp));
	
  CleanWithAngleCuts(*vESDeNegTemp,*vESDePosTemp,*fKFReconstructedGammasTClone);
	
  //  vector <TLorentzVector> vCurrentTLVeNeg = GetTLorentzVector(fCurrentEventNegElectron);
  //  vector <TLorentzVector> vCurrentTLVePos = GetTLorentzVector(fCurrentEventPosElectron);
	
  TClonesArray vCurrentTLVeNeg = GetTLorentzVector(fCurrentEventNegElectronTClone);
  TClonesArray vCurrentTLVePos = GetTLorentzVector(fCurrentEventPosElectronTClone);
	
	
  FillAngle("ESD_eNegePosAngleAfterCut",vCurrentTLVeNeg,vCurrentTLVePos);
	
	
	
	
  //FillAngle("ESD_eNegePosAngleAfterCut",CurrentTLVeNeg,CurrentTLVePos);
	
	
  FillElectronInvMass("ESD_InvMass_ePluseMinus",vCurrentTLVeNeg,vCurrentTLVePos);
  FillElectronInvMass("ESD_InvMass_xPlusxMinus",GetTLorentzVector(vESDxNegTemp),GetTLorentzVector(vESDxPosTemp));
	
	
	
  FillGammaElectronInvMass("ESD_InvMass_GammaePluseMinusChiC","ESD_InvMass_GammaePluseMinusChiCDiff",*fKFReconstructedGammasCutTClone,vCurrentTLVeNeg,vCurrentTLVePos);
	
  FillGammaElectronInvMass("ESD_InvMass_GammaePluseMinusPi0","ESD_InvMass_GammaePluseMinusPi0Diff",
			   *fKFReconstructedGammasCutTClone,vCurrentTLVeNeg,vCurrentTLVePos);
	
  //BackGround
	
  //Like Sign e+e-
  ElectronBackground("ESD_ENegBackground",vCurrentTLVeNeg);
  ElectronBackground("ESD_EPosBackground",vCurrentTLVePos);
  ElectronBackground("ESD_EPosENegBackground",vCurrentTLVeNeg);
  ElectronBackground("ESD_EPosENegBackground",vCurrentTLVePos);
	
  //        Like Sign e+e- no JPsi
  ElectronBackground("ESD_EPosENegNoJPsiBG",GetTLorentzVector(vESDeNegNoJPsi));
  ElectronBackground("ESD_EPosENegNoJPsiBG",GetTLorentzVector(vESDePosNoJPsi));
	
  //Mixed Event
	
  if( fCurrentEventPosElectronTClone->GetEntriesFast() > 0 && fCurrentEventNegElectronTClone->GetEntriesFast() > 0 && fKFReconstructedGammasCutTClone->GetEntriesFast() > 0 ){
    FillGammaElectronInvMass("ESD_EPosENegGammaBackgroundMX","ESD_EPosENegGammaBackgroundMXDiff",
			     *fKFReconstructedGammasCutTClone,*fPreviousEventTLVNegElectronTClone,*fPreviousEventTLVPosElectronTClone);
    *fPreviousEventTLVNegElectronTClone = vCurrentTLVeNeg;
    *fPreviousEventTLVPosElectronTClone = vCurrentTLVePos;
		
  }
	
  /*
  //Photons P
  Double_t vtx[3];
  vtx[0]=0;vtx[1]=0;vtx[2]=0;
  for(UInt_t i=0;i<fKFReconstructedGammasChic.size();i++){
	 
  //      if(fMCGammaChicTempCut[i]->GetMother(0) < 0) continue;
	 
	 
	 
  Int_t tempLabel = fStack->Particle(fMCGammaChicTempCut[i]->GetMother(0))->GetPdgCode();
  //      cout<<" Label Pedro Gonzalez " <<tempLabel <<endl;
	 
  //      cout<<" Label Distance"<<fKFReconstructedGammasChic[i].GetDistanceFromVertex(vtx)<<endl;
	 
  if( tempLabel == 10441 || tempLabel == 20443 || tempLabel == 445 )
	 
  fHistograms->FillHistogram("ESD_PhotonsMomentum",fKFReconstructedGammasChic[i].GetMomentum());
	 
	 
  }
	 
	 
  */
	
	
}

/*
  void AliAnalysisTaskGammaConversion::FillAngle(TString histoName,vector <TLorentzVector> tlVeNeg, vector <TLorentzVector> tlVePos){
  //see header file for documentation
  for( UInt_t iNeg=0; iNeg < tlVeNeg.size(); iNeg++){
  for (UInt_t iPos=0; iPos < tlVePos.size(); iPos++){
  fHistograms->FillHistogram(histoName.Data(),tlVeNeg[iNeg].Vect().Angle(tlVePos[iPos].Vect()));
  }
  }
  }
*/
void AliAnalysisTaskGammaConversion::FillAngle(TString histoName,TClonesArray const tlVeNeg, TClonesArray const tlVePos){
  //see header file for documentation
  for( Int_t iNeg=0; iNeg < tlVeNeg.GetEntriesFast(); iNeg++){
    for (Int_t iPos=0; iPos < tlVePos.GetEntriesFast(); iPos++){
      fHistograms->FillHistogram(histoName.Data(),((TLorentzVector*)(tlVeNeg.At(iNeg)))->Vect().Angle(((TLorentzVector*)(tlVePos.At(iPos)))->Vect()));
    }
  }
}
void AliAnalysisTaskGammaConversion::FillElectronInvMass(TString histoName, TClonesArray const eNeg, TClonesArray const ePos){
  //see header file for documentation
  for( Int_t n=0; n < eNeg.GetEntriesFast(); n++){
    TLorentzVector en = (*(TLorentzVector*)(eNeg.At(n)));
    for (Int_t p=0; p < ePos.GetEntriesFast(); p++){
      TLorentzVector ep = (*(TLorentzVector*)(ePos.At(p)));
      TLorentzVector np = ep + en;
      fHistograms->FillHistogram(histoName.Data(),np.M());
    }
  }
}

void AliAnalysisTaskGammaConversion::FillGammaElectronInvMass(TString histoMass,TString histoDiff,TClonesArray const fKFGammas,
							      TClonesArray const tlVeNeg,TClonesArray const tlVePos)
{
  //see header file for documentation
	
  for( Int_t iNeg=0; iNeg < tlVeNeg.GetEntriesFast(); iNeg++ ){
		
    for (Int_t iPos=0; iPos < tlVePos.GetEntriesFast(); iPos++){
			
      TLorentzVector xy = *((TLorentzVector *)(tlVePos.At(iPos))) + *((TLorentzVector *)(tlVeNeg.At(iNeg)));
			
      for (Int_t iGam=0; iGam < fKFGammas.GetEntriesFast(); iGam++){
				
	//	AliKFParticle * gammaCandidate = &fKFGammas[iGam];
	AliKFParticle * gammaCandidate = (AliKFParticle *)(fKFGammas.At(iGam));
	TLorentzVector g;
				
	g.SetXYZM(gammaCandidate->GetPx(),gammaCandidate->GetPy(),gammaCandidate->GetPz(),fGammaMass);
	TLorentzVector xyg = xy + g;
	fHistograms->FillHistogram(histoMass.Data(),xyg.M());
	fHistograms->FillHistogram(histoDiff.Data(),(xyg.M()-xy.M()));
      }
    }
  }
	
}
void AliAnalysisTaskGammaConversion::ElectronBackground(TString hBg, TClonesArray e)
{
  // see header file for documentation
  for(Int_t i=0; i < e.GetEntriesFast(); i++)
    {
      for (Int_t j=i+1; j < e.GetEntriesFast(); j++)
	{
	  TLorentzVector ee = (*(TLorentzVector*)(e.At(i))) + (*(TLorentzVector*)(e.At(j)));
			
	  fHistograms->FillHistogram(hBg.Data(),ee.M());
	}
    }
}


void AliAnalysisTaskGammaConversion::CleanWithAngleCuts(TClonesArray const negativeElectrons,
							TClonesArray const positiveElectrons, 
							TClonesArray const gammas){
  // see header file for documentation
	
  UInt_t  sizeN = negativeElectrons.GetEntriesFast();
  UInt_t  sizeP = positiveElectrons.GetEntriesFast();
  UInt_t  sizeG = gammas.GetEntriesFast();
	
	
	
  vector <Bool_t> xNegBand(sizeN);
  vector <Bool_t> xPosBand(sizeP);
  vector <Bool_t> gammaBand(sizeG);
	
	
  for(UInt_t iNeg=0; iNeg < sizeN; iNeg++) xNegBand[iNeg]=kTRUE;
  for(UInt_t iPos=0; iPos < sizeP; iPos++) xPosBand[iPos]=kTRUE;
  for(UInt_t iGam=0; iGam < sizeG; iGam++) gammaBand[iGam]=kTRUE;
	
	
  for(UInt_t iPos=0; iPos < sizeP; iPos++){
		
    Double_t aP[3]; 
    ((AliESDtrack*)(positiveElectrons.At(iPos)))->GetConstrainedPxPyPz(aP); 
		
    TVector3 ePosV(aP[0],aP[1],aP[2]);
		
    for(UInt_t iNeg=0; iNeg < sizeN; iNeg++){
			
      Double_t aN[3]; 
      ((AliESDtrack*)(negativeElectrons.At(iNeg)))->GetConstrainedPxPyPz(aN); 
      TVector3 eNegV(aN[0],aN[1],aN[2]);
			
      if(ePosV.Angle(eNegV) < 0.05){ //e+e- from gamma
	xPosBand[iPos]=kFALSE;
	xNegBand[iNeg]=kFALSE;
      }
			
      for(UInt_t iGam=0; iGam < sizeG; iGam++){
	AliKFParticle* gammaCandidate = (AliKFParticle*)gammas.At(iGam);
	TVector3 gammaCandidateVector(gammaCandidate->Px(),gammaCandidate->Py(),gammaCandidate->Pz());
	if(ePosV.Angle(gammaCandidateVector) < 0.05 || eNegV.Angle(gammaCandidateVector) < 0.05)
	  gammaBand[iGam]=kFALSE;
      }
    }
  }
	
	
	
	
  for(UInt_t iPos=0; iPos < sizeP; iPos++){
    if(xPosBand[iPos]){
      new((*fCurrentEventPosElectronTClone)[fCurrentEventPosElectronTClone->GetEntriesFast()]) AliESDtrack((*(AliESDtrack*)(positiveElectrons.At(iPos))));
      //      fCurrentEventPosElectron.push_back(positiveElectrons[iPos]);
    }
  }
  for(UInt_t iNeg=0;iNeg < sizeN; iNeg++){
    if(xNegBand[iNeg]){
      new((*fCurrentEventNegElectronTClone)[fCurrentEventNegElectronTClone->GetEntriesFast()]) AliESDtrack((*(AliESDtrack*)(negativeElectrons.At(iNeg))));
      //      fCurrentEventNegElectron.push_back(negativeElectrons[iNeg]);
    }
  }
  for(UInt_t iGam=0; iGam < sizeG; iGam++){
    if(gammaBand[iGam]){
      new((*fKFReconstructedGammasCutTClone)[fKFReconstructedGammasCutTClone->GetEntriesFast()]) AliKFParticle((*(AliKFParticle*)(gammas.At(iGam))));
      //fKFReconstructedGammasCut.push_back(*(AliKFParticle*)gammas->At(iGam));
    }
  }
}


void  AliAnalysisTaskGammaConversion::GetPID(AliESDtrack *track, Stat_t &pid, Stat_t &weight)
{
  // see header file for documentation
  pid = -1;
  weight = -1;
	
  double wpart[5];
  double wpartbayes[5];
	
  //get probability of the diffenrent particle types
  track->GetESDpid(wpart);
	
  // Tentative particle type "concentrations"
  double c[5]={0.01, 0.01, 0.85, 0.10, 0.05};
	
  //Bayes' formula
  double rcc = 0.;
  for (int i = 0; i < 5; i++)
    {
      rcc+=(c[i] * wpart[i]);
    }
	
	
	
  for (int i=0; i<5; i++) {
    if( rcc>0 || rcc<0){//Kenneth: not sure if the rcc<0 should be there, this is from fixing a coding violation where we are not allowed to say: rcc!=0 (RC19)  
      wpartbayes[i] = c[i] * wpart[i] / rcc;
    }
  }
	
	
	
  Float_t max=0.;
  int ipid=-1;
  //find most probable particle in ESD pid
  //0:Electron - 1:Muon - 2:Pion - 3:Kaon - 4:Proton
  for (int i = 0; i < 5; i++)
    {
      if (wpartbayes[i] > max)
        {
	  ipid = i;
	  max = wpartbayes[i];
        }
    }
	
  pid = ipid;
  weight = max;
}
double AliAnalysisTaskGammaConversion::GetSigmaToVertex(AliESDtrack* t)
{
  // Calculates the number of sigma to the vertex.
	
  Float_t b[2];
  Float_t bRes[2];
  Float_t bCov[3];
  t->GetImpactParameters(b,bCov);
  if (bCov[0]<=0 || bCov[2]<=0) {
    AliDebug(1, "Estimated b resolution lower or equal zero!");
    bCov[0]=0; bCov[2]=0;
  }
  bRes[0] = TMath::Sqrt(bCov[0]);
  bRes[1] = TMath::Sqrt(bCov[2]);
	
  // -----------------------------------
  // How to get to a n-sigma cut?
  //
  // The accumulated statistics from 0 to d is
  //
  // ->  Erf(d/Sqrt(2)) for a 1-dim gauss (d = n_sigma)
  // ->  1 - Exp(-d**2) for a 2-dim gauss (d*d = dx*dx + dy*dy != n_sigma)
  //
  // It means that for a 2-dim gauss: n_sigma(d) = Sqrt(2)*ErfInv(1 - Exp((-x**2)/2)
  // Can this be expressed in a different way?
	
  if (bRes[0] == 0 || bRes[1] ==0)
    return -1;
	
  double d = TMath::Sqrt(TMath::Power(b[0]/bRes[0],2) + TMath::Power(b[1]/bRes[1],2));
	
  // stupid rounding problem screws up everything:
  // if d is too big, TMath::Exp(...) gets 0, and TMath::ErfInverse(1) that should be infinite, gets 0 :(
  if (TMath::Exp(-d * d / 2) < 1e-10)
    return 1000;
	
	
  d = TMath::ErfInverse(1 - TMath::Exp(-d * d / 2)) * TMath::Sqrt(2);
  return d;
}

//vector <TLorentzVector> AliAnalysisTaskGammaConversion::GetTLorentzVector(vector <AliESDtrack*> esdTrack){
TClonesArray AliAnalysisTaskGammaConversion::GetTLorentzVector(TClonesArray *const esdTrack){
  //Return TLoresntz vector of track?
  //  vector <TLorentzVector> tlVtrack(0);
  TClonesArray array("TLorentzVector",0); 
	
  for(Int_t itrack=0; itrack < esdTrack->GetEntriesFast(); itrack++){
    double p[3]; 
    //esdTrack[itrack]->GetConstrainedPxPyPz(p);
    ((AliESDtrack*)(esdTrack->At(itrack)))->GetConstrainedPxPyPz(p);
    TLorentzVector currentTrack;
    currentTrack.SetXYZM(p[0],p[1],p[2],fElectronMass);
    new((array)[array.GetEntriesFast()])  TLorentzVector(currentTrack);
    //    tlVtrack.push_back(currentTrack);
  }
	
  return array;
	
  //  return tlVtrack;
}


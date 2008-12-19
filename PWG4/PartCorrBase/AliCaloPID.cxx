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
/* $Id: AliCaloPID.cxx 21839 2007-10-29 13:49:42Z gustavo $ */

//_________________________________________________________________________
// Class for track/cluster acceptance selection
// Selection in Central barrel, EMCAL and PHOS
//                
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include <TMath.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TFormula.h>

//---- ANALYSIS system ----
#include "AliLog.h"
#include "AliCaloPID.h"
#include "AliAODCaloCluster.h"
#include "AliAODPWG4Particle.h"
#include "AliStack.h"
#include "TParticle.h"

ClassImp(AliCaloPID)


//________________________________________________
AliCaloPID::AliCaloPID() : 
TObject(), fEMCALPhotonWeight(0.), fEMCALPi0Weight(0.),  
fEMCALElectronWeight(0.),  fEMCALChargeWeight(0.),
fEMCALNeutralWeight(0.),
fPHOSPhotonWeight(0.), fPHOSPi0Weight(0.),  
fPHOSElectronWeight(0.), fPHOSChargeWeight(0.) , 
fPHOSNeutralWeight(0.), fPHOSWeightFormula(0), 
fPHOSPhotonWeightFormula(0x0), fPHOSPi0WeightFormula(0x0),
fDispCut(0.),fTOFCut(0.), fDebug(-1), fMCGenerator("")
{
	//Ctor
	
	//Initialize parameters
	InitParameters();
}

//____________________________________________________________________________
AliCaloPID::AliCaloPID(const AliCaloPID & pid) :   
TObject(pid), fEMCALPhotonWeight(pid.fEMCALPhotonWeight), 
fEMCALPi0Weight(pid.fEMCALPi0Weight), 
fEMCALElectronWeight(pid.fEMCALElectronWeight), 
fEMCALChargeWeight(pid.fEMCALChargeWeight), 
fEMCALNeutralWeight(pid.fEMCALNeutralWeight), 
fPHOSPhotonWeight(pid.fPHOSPhotonWeight),
fPHOSPi0Weight(pid.fPHOSPi0Weight),
fPHOSElectronWeight(pid.fPHOSElectronWeight), 
fPHOSChargeWeight(pid.fPHOSChargeWeight),
fPHOSNeutralWeight(pid.fPHOSNeutralWeight),
fPHOSWeightFormula(pid.fPHOSWeightFormula), 
fPHOSPhotonWeightFormula(pid.fPHOSPhotonWeightFormula), 
fPHOSPi0WeightFormula(pid.fPHOSPi0WeightFormula), 
fDispCut(pid.fDispCut),fTOFCut(pid.fTOFCut),
fDebug(pid.fDebug),fMCGenerator(pid.fMCGenerator)
{
	// cpy ctor
	
}

//_________________________________________________________________________
AliCaloPID & AliCaloPID::operator = (const AliCaloPID & pid)
{
	// assignment operator
	
	if(&pid == this) return *this;
	
	fEMCALPhotonWeight = pid. fEMCALPhotonWeight ;
	fEMCALPi0Weight = pid.fEMCALPi0Weight ;
	fEMCALElectronWeight = pid.fEMCALElectronWeight; 
	fEMCALChargeWeight = pid.fEMCALChargeWeight;
	fEMCALNeutralWeight = pid.fEMCALNeutralWeight;
	
	fPHOSPhotonWeight = pid.fPHOSPhotonWeight ;
	fPHOSPi0Weight = pid.fPHOSPi0Weight ;
	fPHOSElectronWeight = pid.fPHOSElectronWeight; 
	fPHOSChargeWeight = pid.fPHOSChargeWeight;
	fPHOSNeutralWeight = pid.fPHOSNeutralWeight;
	
	fPHOSWeightFormula       = pid.fPHOSWeightFormula; 
	fPHOSPhotonWeightFormula = pid.fPHOSPhotonWeightFormula; 
	fPHOSPi0WeightFormula    = pid.fPHOSPi0WeightFormula;
	
	fDispCut  = pid.fDispCut;
	fTOFCut   = pid.fTOFCut;
	fDebug    = pid.fDebug;
	fMCGenerator = pid.fMCGenerator;

	return *this;
	
}

//_________________________________
AliCaloPID::~AliCaloPID() {
	//Dtor
	
	if(fPHOSPhotonWeightFormula) delete  fPHOSPhotonWeightFormula ;
	if(fPHOSPi0WeightFormula) delete  fPHOSPi0WeightFormula ;
	
}


//_________________________________________________________________________
Int_t AliCaloPID::CheckOrigin(const Int_t label, AliStack * stack) const {
  //Play with the MC stack if available
  //Check origin of the candidates, good for PYTHIA
  
  if(!stack) AliFatal("Stack is not available, check analysis settings in configuration file, STOP!!");
  
  if(label >= 0 && label <  stack->GetNtrack()){
    //Mother
    TParticle * mom = stack->Particle(label);
    Int_t mPdg = TMath::Abs(mom->GetPdgCode());
    Int_t mStatus =  mom->GetStatusCode() ;
    Int_t iParent =  mom->GetFirstMother() ;
    if(fDebug > 0 && label < 8 ) printf("AliCaloPID::CheckOrigin: Mother is parton %d\n",iParent);
    
    //GrandParent
    TParticle * parent = new TParticle ;
    Int_t pPdg = -1;
    Int_t pStatus =-1;
    if(iParent > 0){
      parent = stack->Particle(iParent);
      pPdg = TMath::Abs(parent->GetPdgCode());
      pStatus = parent->GetStatusCode();  
    }
    else if(fDebug > 0 ) printf("AliCaloPID::CheckOrigin: Parent with label %d\n",iParent);
    
    //return tag
    if(mPdg == 22){
      if(mStatus == 1){
	if(fMCGenerator == "PYTHIA"){
	  if(iParent < 8 && iParent > 5) {//outgoing partons
	    if(pPdg == 22) return kMCPrompt;
	    else  return kMCFragmentation;
	  }//Outgoing partons
	  else if(pStatus == 11){//Decay
	    if(pPdg == 111) return kMCPi0Decay ;
	    else if (pPdg == 321)  return kMCEtaDecay ;
	    else  return kMCOtherDecay ;
	  }//Decay
	  else return kMCISR; //Initial state radiation
	}//PYTHIA

	else if(fMCGenerator == "HERWIG"){	  
	  if(pStatus < 197){//Not decay
 	    while(1){
	      if(parent->GetFirstMother()<=5) break;
	      iParent = parent->GetFirstMother();
	      parent=stack->Particle(iParent);
	      pStatus= parent->GetStatusCode();
	      pPdg = parent->GetPdgCode();
	    }//Look for the parton
	    
	    if(iParent < 8 && iParent > 5) {
	      if(pPdg == 22) return kMCPrompt;
	      else  return kMCFragmentation;
	    }
	    return kMCISR;//Initial state radiation
	  }//Not decay
	  else{//Decay
	    if(pPdg == 111) return kMCPi0Decay ;
	    else if (pPdg == 321)  return kMCEtaDecay ;
	    else  return kMCOtherDecay ;
	  }//Decay
	}//HERWIG
	else return  kMCUnknown;
      }//Status 1 : Pythia generated
      else if(mStatus == 0){
	if(pPdg ==22 || pPdg ==11) return kMCConversion ;
	if(pPdg == 111) return kMCPi0Decay ;
	else if (pPdg == 221)  return kMCEtaDecay ;
	else  return kMCOtherDecay ;
      }//status 0 : geant generated
    }//Mother Photon
    else if(mPdg == 111)  return kMCPi0 ;
    else if(mPdg == 221)  return kMCEta ;
    else if(mPdg ==11){
      printf("Origin electron, pT %f\n",mom->Pt());

      if(mStatus == 0) return kMCConversion ;
      else return kMCElectron ;
    }
    else return kMCUnknown;
  }//Good label value
  else{
    if(label < 0 ) printf("AliCaloPID::CheckOrigin: *** bad label or no stack ***:  label %d \n", label);
    if(label >=  stack->GetNtrack()) printf("AliCaloPID::CheckOrigin: *** large label ***:  label %d, n tracks %d \n", label, stack->GetNtrack());
    return kMCUnknown;
  }//Bad label
	
  return kMCUnknown;
  
}

//_______________________________________________________________
void AliCaloPID::InitParameters()
{
	//Initialize the parameters of the PID.
	
	fEMCALPhotonWeight   = 0.8 ;
	fEMCALPi0Weight      = 0.5 ;
	fEMCALElectronWeight = 0.8 ;
	fEMCALChargeWeight   = 0.5 ;
	fEMCALNeutralWeight  = 0.5 ;
	
	fPHOSPhotonWeight    = 0.75 ;
	fPHOSPi0Weight       = 0.8 ;
	fPHOSElectronWeight  = 0.5 ;
	fPHOSChargeWeight    = 0.5 ;
	fPHOSNeutralWeight   = 0.5 ;
	
	//Formula to set the PID weight threshold for photon or pi0
	fPHOSWeightFormula = kTRUE;
	fPHOSPhotonWeightFormula = 
    new TFormula("photonWeight","0.98*(x<40)+ 0.68*(x>=100)+(x>=40 && x<100)*(0.98+x*(6e-3)-x*x*(2e-04)+x*x*x*(1.1e-06))");
	fPHOSPi0WeightFormula = 
    new TFormula("pi0Weight","0.98*(x<65)+ 0.915*(x>=100)+(x>=65 && x-x*(1.95e-3)-x*x*(4.31e-05)+x*x*x*(3.61e-07))");
	
    fDispCut  = 1.5;
	fTOFCut   = 5.e-9;
	fDebug = -1;
	fMCGenerator = "PYTHIA";
}

//_______________________________________________________________
Int_t AliCaloPID::GetPdg(const TString calo, const Double_t * pid, const Float_t energy) const {
	//Return most probable identity of the particle.
	
	if(!pid) AliFatal("pid pointer not initialized!!!");
	
	Float_t wPh =  fPHOSPhotonWeight ;
	Float_t wPi0 =  fPHOSPi0Weight ;
	Float_t wE =  fPHOSElectronWeight ;
	Float_t wCh =  fPHOSChargeWeight ;
	Float_t wNe =  fPHOSNeutralWeight ;
	
	
	if(calo == "PHOS" && fPHOSWeightFormula){
		wPh  = fPHOSPhotonWeightFormula->Eval(energy) ;
		wPi0 = fPHOSPi0WeightFormula->Eval(energy);
	}
	
	if(calo == "EMCAL"){
		
		wPh  =  fEMCALPhotonWeight ;
		wPi0 =  fEMCALPi0Weight ;
		wE   =  fEMCALElectronWeight ;
		wCh  =  fEMCALChargeWeight ;
		wNe  =  fEMCALNeutralWeight ;
		
	}
	
	if(fDebug > 0)  printf("AliCaloPID::GetPdg: calo %s, ph %0.2f, pi0 %0.2f, el %0.2f, conv el %0.2f, hadrons: pion %0.2f, kaon %0.2f, proton %0.2f , neutron %0.2f, kaon %0.2f \n",
						   calo.Data(),pid[AliAODCluster::kPhoton], pid[AliAODCluster::kPi0],
						   pid[AliAODCluster::kElectron], pid[AliAODCluster::kEleCon],
						   pid[AliAODCluster::kPion], pid[AliAODCluster::kKaon], pid[AliAODCluster::kProton],
						   pid[AliAODCluster::kNeutron], pid[AliAODCluster::kKaon0]);
	
	Int_t pdg = kNeutralUnknown ;
	Float_t chargedHadronWeight = pid[AliAODCluster::kProton]+pid[AliAODCluster::kKaon]+
    pid[AliAODCluster::kPion]+pid[AliAODCluster::kMuon];
	Float_t neutralHadronWeight = pid[AliAODCluster::kNeutron]+pid[AliAODCluster::kKaon0];
	Float_t allChargedWeight    = pid[AliAODCluster::kElectron]+pid[AliAODCluster::kEleCon]+ chargedHadronWeight;
	Float_t allNeutralWeight    = pid[AliAODCluster::kPhoton]+pid[AliAODCluster::kPi0]+ neutralHadronWeight;
	
	//Select most probable ID
	if(calo=="PHOS"){
		if(pid[AliAODCluster::kPhoton] > wPh) pdg = kPhoton ;
		else if(pid[AliAODCluster::kPi0] > wPi0) pdg = kPi0 ; 
		else if(pid[AliAODCluster::kElectron] > wE)  pdg = kElectron ;
		else if(pid[AliAODCluster::kEleCon] >  wE) pdg = kEleCon ;
		else if(chargedHadronWeight > wCh) pdg = kChargedHadron ;  
		else if(neutralHadronWeight > wNe) pdg = kNeutralHadron ; 
		else if(allChargedWeight >  allNeutralWeight)
			pdg = kChargedUnknown ; 
		else 
			pdg = kNeutralUnknown ;
	}
	else{//EMCAL
		if(pid[AliAODCluster::kPhoton]  > wPh) pdg = kPhoton ;
		else if(pid[AliAODCluster::kPi0] > wPi0) pdg = kPi0 ; 
		else if(pid[AliAODCluster::kElectron]  > wE) pdg = kElectron ;
		else if(chargedHadronWeight + neutralHadronWeight > wCh) pdg = kChargedHadron ;  
		else if(neutralHadronWeight + chargedHadronWeight > wNe) pdg = kNeutralHadron ; 
		else pdg =  kNeutralUnknown ;
		
	}
	
	
	if(fDebug > 0)printf("AliCaloPID::GetPdg:Final Pdg: %d \n", pdg);
	
	
	
	return pdg ;
	
}

//_______________________________________________________________
Int_t AliCaloPID::GetPdg(const TString calo,const TLorentzVector mom, const AliAODCaloCluster * cluster) const {
	//Recalculated PID with all parameters
	if(fDebug > 0)printf("AliCaloPID::GetPdg: Calorimeter %s, E %3.2f, l0 %3.2f, l1 %3.2f, disp %3.2f, tof %1.11f, distCPV %3.2f, distToBC %1.1f, NMax %d\n",
						 calo.Data(),mom.E(),cluster->GetM02(),cluster->GetM20(),cluster->GetDispersion(),cluster->GetTOF(), 
						 cluster->GetEmcCpvDistance(), cluster->GetDistToBadChannel(),cluster->GetNExMax());
	
	if(calo == "EMCAL") {
		if(cluster->GetM02()< 0.25) return kPhoton ;
		else return  kNeutralHadron ; 
	}
	
	//   if(calo == "PHOS") {
	//    if(cluster->GetM02()< 0.25) return kPhoton ;
	//    else return  kNeutralHadron ; 
	//  }
	
	return  kNeutralHadron ; 
	
}

//__________________________________________________
TString  AliCaloPID::GetPIDParametersList()  {
	//Put data member values in string to keep in output container
	
	TString parList ; //this will be list of parameters used for this analysis.
	char onePar[255] ;
	sprintf(onePar,"--- AliCaloPID ---\n") ;
	parList+=onePar ;	
	sprintf(onePar,"fDispCut =%2.2f (Cut on dispersion, used in PID evaluation) \n",fDispCut) ;
	parList+=onePar ;
	sprintf(onePar,"fTOFCut  =%e (Cut on TOF, used in PID evaluation) \n",fTOFCut) ;
	parList+=onePar ;
	sprintf(onePar,"fEMCALPhotonWeight =%2.2f (EMCAL bayesian weight for photons)\n",fEMCALPhotonWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fEMCALPi0Weight =%2.2f (EMCAL bayesian weight for pi0)\n",fEMCALPi0Weight) ;
	parList+=onePar ;
	sprintf(onePar,"fEMCALElectronWeight =%2.2f(EMCAL bayesian weight for electrons)\n",fEMCALElectronWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fEMCALChargeWeight =%2.2f (EMCAL bayesian weight for charged hadrons)\n",fEMCALChargeWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fEMCALNeutralWeight =%2.2f (EMCAL bayesian weight for neutral hadrons)\n",fEMCALNeutralWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fPHOSPhotonWeight =%2.2f (PHOS bayesian weight for photons)\n",fPHOSPhotonWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fPHOSPi0Weight =%2.2f (PHOS bayesian weight for pi0)\n",fPHOSPi0Weight) ;
	parList+=onePar ;
	sprintf(onePar,"fPHOSElectronWeight =%2.2f(PHOS bayesian weight for electrons)\n",fPHOSElectronWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fPHOSChargeWeight =%2.2f (PHOS bayesian weight for charged hadrons)\n",fPHOSChargeWeight) ;
	parList+=onePar ;
	sprintf(onePar,"fPHOSNeutralWeight =%2.2f (PHOS bayesian weight for neutral hadrons)\n",fPHOSNeutralWeight) ;
	parList+=onePar ;
	
	if(fPHOSWeightFormula){
		parList+="PHOS Photon Weight Formula: "+(fPHOSPhotonWeightFormula->GetExpFormula("p"));
		parList+="PHOS Pi0    Weight Formula: "+(fPHOSPi0WeightFormula->GetExpFormula("p"));
	}
	
	return parList; 
	
}

//________________________________________________________________
void AliCaloPID::Print(const Option_t * opt) const
{
	
	//Print some relevant parameters set for the analysis
	if(! opt)
		return;
	
	printf("***** Print: %s %s ******\n", GetName(), GetTitle() ) ;
	
	printf("PHOS PID weight , photon %0.2f, pi0 %0.2f, e %0.2f, charge %0.2f, neutral %0.2f \n",  
		   fPHOSPhotonWeight,  fPHOSPi0Weight, 
		   fPHOSElectronWeight,  fPHOSChargeWeight,   fPHOSNeutralWeight) ; 
	printf("EMCAL PID weight, photon %0.2f, pi0 %0.2f, e %0.2f, charge %0.2f, neutral %0.2f\n",   
		   fEMCALPhotonWeight,  fEMCALPi0Weight, 
		   fEMCALElectronWeight,  fEMCALChargeWeight,  fEMCALNeutralWeight) ; 
	
	printf("PHOS Parametrized weight on?  =     %d\n",  fPHOSWeightFormula) ; 
	if(fPHOSWeightFormula){
		printf("Photon weight formula = %s\n", (fPHOSPhotonWeightFormula->GetExpFormula("p")).Data());
		printf("Pi0    weight formula = %s\n", (fPHOSPhotonWeightFormula->GetExpFormula("p")).Data());
    }
	
	printf("TOF cut        = %e\n",fTOFCut);
	printf("Dispersion cut = %2.2f\n",fDispCut);
	printf("Debug level    = %d\n",fDebug);
	printf("MC Generator   = %s\n",fMCGenerator.Data());

	printf(" \n");
	
} 

//_______________________________________________________________
void AliCaloPID::SetPIDBits(const TString calo, const AliAODCaloCluster * cluster, AliAODPWG4Particle * ph) {
	//Set Bits for PID selection
	
	//Dispersion/lambdas
    Double_t disp=cluster->GetDispersion()  ;
	//    Double_t m20=calo->GetM20() ;
	//    Double_t m02=calo->GetM02() ; 
    ph->SetDispBit(disp<fDispCut) ;  
	
    //TOF
    Double_t tof=cluster->GetTOF()  ;
    ph->SetTOFBit(TMath::Abs(tof)<fTOFCut) ; 
	
    //Charged veto
	//    Double_t cpvR=calo->GetEmcCpvDistance() ; 
    Int_t ntr=cluster->GetNTracksMatched();  //number of track matched
    ph->SetChargedBit(ntr>0) ;  //Temporary cut, should we evaluate distance?
	
    //Set PID pdg
	ph->SetPdg(GetPdg(calo,cluster->PID(),ph->E()));
	
	if(fDebug > 0){ 
		printf("AliCaloPID::SetPIDBits: TOF %e, Dispersion %2.2f, NTracks %d\n",tof , disp, ntr); 	
		printf("AliCaloPID::SetPIDBits: pdg %d, bits: TOF %d, Dispersion %d, Charge %d\n",
			   ph->GetPdg(), ph->GetTOFBit() , ph->GetDispBit() , ph->GetChargedBit()); 
	}
}



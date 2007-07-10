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
// Class to select ESD and AOD data for the analysis gamma isolation and correlations
// 
//  Class created from old AliPHOSGammaJet 
//  (see AliRoot versions previous Release 4-09)
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include <TFile.h>
#include <TParticle.h>
#include <TH2.h>
#include <TChain.h>

//---- ANALYSIS system ----
#include "AliAnaGammaData.h" 
#include "AliAnalysisManager.h"
#include "AliESD.h"
#include "AliESDtrack.h"
#include "AliESDCaloCluster.h"
#include "Riostream.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliAODPhoton.h"
#include "AliLog.h"

ClassImp(AliAnaGammaData)

//____________________________________________________________________________
  AliAnaGammaData::AliAnaGammaData(const char *name) : 
     AliAnalysisTask(name,"AnalysisDataReder"),  
     fChain(0x0), 
     fESD(0x0), 
     fCTSList(0x0),  fEMCALList(0x0), fPHOSList(0x0),  
     fCalorimeter(""), fEMCALPID(0),fPHOSPID(0),
     fEMCALPhotonWeight(0.), fEMCALPi0Weight(0.), fPHOSPhotonWeight(0.)
 
{
  //Default Ctor
  
  //Initialize parameters
  InitParameters();
  
  // Input slot #0 
  DefineInput(0, TChain::Class());
  // Output slots
  DefineOutput(0,  TClonesArray::Class()) ;  
  DefineOutput(1,  TClonesArray::Class()) ;
  DefineOutput(2,  TClonesArray::Class()) ;
}

//____________________________________________________________________________
AliAnaGammaData::AliAnaGammaData(const AliAnaGammaData & g) :   
  AliAnalysisTask(g), 
  fChain(g.fChain), 
  fESD(g.fESD), 
  fCTSList(g.fCTSList),  fEMCALList(g.fEMCALList), fPHOSList(g.fPHOSList),  
  fCalorimeter(g.fCalorimeter),
  fEMCALPID(g.fEMCALPID), 
  fPHOSPID(g.fPHOSPID),
  fEMCALPhotonWeight(g.fEMCALPhotonWeight), 
  fEMCALPi0Weight(g.fEMCALPi0Weight), 
  fPHOSPhotonWeight(g.fPHOSPhotonWeight)
{
  // cpy ctor
  SetName (g.GetName()) ; 
  SetTitle(g.GetTitle()) ; 
}

//_________________________________________________________________________
AliAnaGammaData & AliAnaGammaData::operator = (const AliAnaGammaData & source)
{
  // assignment operator

  if(&source == this) return *this;

  fChain = source.fChain ; 
  fESD = source.fESD ;
  fCTSList = source.fCTSList;  
  fEMCALList = source.fEMCALList;
  fPHOSList = source.fPHOSList;  
  fCalorimeter = source. fCalorimeter ;  
  fEMCALPID = source.fEMCALPID ;
  fPHOSPID = source.fPHOSPID ;
  fEMCALPhotonWeight = source. fEMCALPhotonWeight ;
  fEMCALPi0Weight = source.fEMCALPi0Weight ;
  fPHOSPhotonWeight = source.fPHOSPhotonWeight ;

  return *this;

}

//____________________________________________________________________________
AliAnaGammaData::~AliAnaGammaData() 
{
  // Remove all pointers
  fCTSList->Clear() ; 
  delete fCTSList ;
  fEMCALList->Clear() ; 
  delete fEMCALList ;
  fPHOSList->Clear() ; 
  delete fPHOSList ;
}

//______________________________________________________________________________
void AliAnaGammaData::ConnectInputData(const Option_t*)
{
  // Initialisation of branch container and histograms 
    
  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  
  // Get input data
  fChain = dynamic_cast<TChain *>(GetInputData(0)) ;
  if (!fChain) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }
  
  // One should first check if the branch address was taken by some other task
  char ** address = (char **)GetBranchAddress(0, "ESD");
  if (address) {
    fESD = (AliESD*)(*address);
  } else {
    fESD = new AliESD();
    SetBranchAddress(0, "ESD", &fESD);
  }

}

//________________________________________________________________________
void AliAnaGammaData::CreateOutputObjects()
{  
  //
  // Create the output container
  //

  // store particle lists in output containers
    
  fCTSList = new TClonesArray("TParticle") ; 
  fEMCALList = new TClonesArray("TParticle") ; 
  fPHOSList = new TClonesArray("TParticle") ; 
}

//____________________________________________________________________________
void AliAnaGammaData::CreateParticleList(){
  
  //Create a list of particles from the ESD. These particles have been measured 
  //by the Central Tracking system (TPC+ITS), PHOS and EMCAL 
   
  Int_t npar  = 0 ;
  Float_t *pid = new Float_t[AliPID::kSPECIESN];  
  AliDebug(3,"Fill particle lists");

  //AliInfo(Form("fCalorimeter %s",fCalorimeter.Data()));

  Double_t v[3] ; //vertex ;
  fESD->GetVertex()->GetXYZ(v) ; 

  //########### PHOS ##############
  if(fCalorimeter == "PHOS"){

    Int_t begphos = fESD->GetFirstPHOSCluster();  
    Int_t endphos = fESD->GetFirstPHOSCluster() + 
      fESD->GetNumberOfPHOSClusters() ;  
    Int_t indexNePHOS = 0 ;
    AliDebug(3,Form("First PHOS particle %d, last particle %d", begphos,endphos));

    if(fCalorimeter == "PHOS"){
      for (npar =  begphos; npar <  endphos; npar++) {//////////////PHOS track loop
	AliESDCaloCluster * clus = fESD->GetCaloCluster(npar) ; // retrieve track from esd

	//Create a TParticle to fill the particle list
	TLorentzVector momentum ;
	clus->GetMomentum(momentum, v);
	TParticle * particle = new TParticle() ;
	particle->SetMomentum(momentum) ;
	
	//Select only photons
	pid=clus->GetPid();
	AliDebug(1,Form("PHOS clusters: pt %f, phi %f, eta %f, \n PID:   ph %0.01f, pi0  %0.01f, e  %0.01f", 
			particle->Pt(),particle->Phi(),particle->Eta(),
			pid[AliPID::kPhoton] ,pid[AliPID::kPhoton], pid[AliPID::kElectron]));
	if( !fPHOSPID)
	  new((*fPHOSList)[indexNePHOS++])   TParticle(*particle) ;
	else if( pid[AliPID::kPhoton] > fPHOSPhotonWeight)
	  new((*fPHOSList)[indexNePHOS++])   TParticle(*particle) ;
      }
    }
  }
  
  //########### CTS (TPC+ITS) #####################
  Int_t begtpc   = 0 ;  
  Int_t endtpc   = fESD->GetNumberOfTracks() ;
  Int_t indexCh  = 0  ;
  AliDebug(3,Form("First CTS particle %d, last particle %d", begtpc,endtpc));

  for (npar =  begtpc; npar <  endtpc; npar++) {////////////// track loop
    AliESDtrack * track = fESD->GetTrack(npar) ; // retrieve track from esd

    //We want tracks fitted in the detectors:
    ULong_t status=AliESDtrack::kTPCrefit;
    status|=AliESDtrack::kITSrefit;
   
    //We want tracks whose PID bit is set:
    //     ULong_t status =AliESDtrack::kITSpid;
    //     status|=AliESDtrack::kTPCpid;
  
    if ( (track->GetStatus() & status) == status) {//Check if the bits we want are set
      // Do something with the tracks which were successfully
      // re-fitted 
    
      Double_t en = 0; //track ->GetTPCsignal() ;
      Double_t mom[3];
      track->GetPxPyPz(mom) ;
      Double_t px = mom[0];
      Double_t py = mom[1];
      Double_t pz = mom[2]; //Check with TPC people if this is correct.
      Int_t pdg = 11; //Give any charged PDG code, in this case electron.
      //I just want to tag the particle as charged

       TParticle * particle = new TParticle(pdg, 1, -1, -1, -1, -1, 
					    px, py, pz, en, v[0], v[1], v[2], 0);
       AliDebug(1,Form("Tracks: pt %f, phi %f, eta %f", 
		       particle->Pt(),particle->Phi(),particle->Eta()));
      new((*fCTSList)[indexCh++])       TParticle(*particle) ;    
    }
  }
  
  //################ EMCAL ##############
  Int_t begem = fESD->GetFirstEMCALCluster();  
  Int_t endem = fESD->GetFirstEMCALCluster() + 
      fESD->GetNumberOfEMCALClusters() ;  
  Int_t indexNe  = 0 ; 
  
  AliDebug(1,Form("First EMCAL particle %d, last particle %d",begem,endem));
    
    for (npar =  begem; npar <  endem; npar++) {//////////////EMCAL track loop
      AliESDCaloCluster * clus = fESD->GetCaloCluster(npar) ; // retrieve track from esd
      Int_t clustertype = clus->GetClusterType();
      Int_t trackmatch = clus->GetTrackMatched();
      if(clustertype == AliESDCaloCluster::kClusterv1 &&  trackmatch == -1){
	TLorentzVector momentum ;
	clus->GetMomentum(momentum, v);
	TParticle * particle = new TParticle() ;
	particle->SetMomentum(momentum) ;

	pid=clus->GetPid();
	AliDebug(1,Form("EMCAL clusters: pt %f, phi %f, eta %f, \n PID:   ph %0.01f, pi0  %0.01f, e  %0.01f", 
			particle->Pt(),particle->Phi(),particle->Eta(),
			pid[AliPID::kPhoton] ,pid[AliPID::kPhoton], pid[AliPID::kElectron]));

	if(fCalorimeter == "EMCAL")
	  {
	    if(!fEMCALPID) //Only identified particles
	      new((*fEMCALList)[indexNe++])       TParticle(*particle) ;
	    else if(pid[AliPID::kPhoton] > fEMCALPhotonWeight)
	      new((*fEMCALList)[indexNe++])       TParticle(*particle) ;	    
	  }
	else
	  {
	      Int_t pdg = 0;
	      if(fEMCALPID) 
		{
		  if( pid[AliPID::kPhoton] > fEMCALPhotonWeight) 
		    pdg = 22;
		  else if( pid[AliPID::kPi0] > fEMCALPi0Weight)
		    pdg = 111;
		}
	      else
		pdg = 22; //No PID, assume all photons
	      
	      TParticle * particle = new TParticle(pdg, 1, -1, -1, -1, -1, 
						   momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E(), v[0], v[1], v[2], 0);	      
	      new((*fEMCALList)[indexNe++])       TParticle(*particle) ;
	  }
      }
    }
    
    AliDebug(3,"Particle lists filled");
    
}



//____________________________________________________________________________
void AliAnaGammaData::Exec(Option_t *) 
{
  
  // Processing of one event
  //Get ESDs
  Long64_t entry = fChain->GetReadEntry() ;
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  }
  
  if ( !((entry)%100) ) 
     AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), entry)) ; 

  //Fill particle lists 
  CreateParticleList(); 

  PostData(0, fCTSList);
  PostData(1, fEMCALList);
  PostData(2, fPHOSList);
  
  AliDebug(2, "End of analysis");
}    

  //____________________________________________________________________________
void AliAnaGammaData::InitParameters()
{
 
  //Initialize the parameters of the analysis.
  fCalorimeter="PHOS";

  //Fill particle lists when PID is ok
  fEMCALPID = kFALSE;
  fPHOSPID = kFALSE;
  fEMCALPhotonWeight = 0.5 ;
  fEMCALPi0Weight = 0.5 ;
  fPHOSPhotonWeight = 0.8 ;

}


void AliAnaGammaData::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  Info("Print", "%s %s", GetName(), GetTitle() ) ;
  printf("PHOS PID on?               =     %d\n",  fPHOSPID) ; 
  printf("EMCAL PID  on?         =     %d\n",  fEMCALPID) ;
  printf("PHOS PID weight , photon  %f\n",  fPHOSPhotonWeight) ; 
  printf("EMCAL PID weight, photon %f, pi0 %f\n",   fEMCALPhotonWeight,  fEMCALPi0Weight) ; 
  printf("Prompt gamma Calorimeter            =     %s\n", fCalorimeter.Data()) ; 

} 

void AliAnaGammaData::Terminate(Option_t *)
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
    

}

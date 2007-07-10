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
// Class to select MC kinematics data for the analysis gamma isolation and correlations
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
#include <TRandom.h>

//---- ANALYSIS system ----
#include "AliAnaGammaMC.h" 
#include "AliAnalysisManager.h"
#include "AliESD.h"
#include "AliESDtrack.h"
#include "AliESDCaloCluster.h"
#include "Riostream.h"
#include "AliLog.h"

ClassImp(AliAnaGammaMC)

//____________________________________________________________________________
  AliAnaGammaMC::AliAnaGammaMC(const char *name) : 
     AliAnalysisTaskRL(name,"AnalysisGammaMC"),  
     fChain(0x0), 
     fESD(0x0), 
     fCTSList(0x0),  fEMCALList(0x0), fPHOSList(0x0),   fPartonList(0x0),  
     fCalorimeter(""),  
     fCTSEtaCut(0.), fEMCALEtaCut(0.), fPHOSEtaCut(0.),
     fNeutralPtCut(0.),
     fChargedPtCut(0.), 
     fEMCALIPDistance(0.),  fPHOSIPDistance(0.), 
     fEMCALMinDistance(0.),  fPHOSMinDistance(0.), 
     fDecayPi0(kFALSE)
       
{
  //Default Ctor

  fPhiEMCALCut[0]=0.;
  fPhiEMCALCut[1]=0.;
  fPhiPHOSCut[0]=0.;
  fPhiPHOSCut[1]=0.;
 
  //Initialize parameters
  InitParameters();
  
  // Input slot #0 
  DefineInput(0, TChain::Class());
  // Output slots
  DefineOutput(0,  TClonesArray::Class()) ;  
  DefineOutput(1,  TClonesArray::Class()) ; 
  DefineOutput(2,  TClonesArray::Class()) ; 
  DefineOutput(3,  TClonesArray::Class()) ; 
}

//____________________________________________________________________________
AliAnaGammaMC::AliAnaGammaMC(const AliAnaGammaMC & g) :   
  fChain(g.fChain), 
  fESD(g.fESD),    
  fCTSList(g.fCTSList),  fEMCALList(g.fEMCALList), fPHOSList(g.fPHOSList),   fPartonList(g.fPartonList),  
  fCalorimeter(g.fCalorimeter),
  fCTSEtaCut(g.fCTSEtaCut),  fEMCALEtaCut(g.fEMCALEtaCut),  fPHOSEtaCut(g.fPHOSEtaCut),
  fNeutralPtCut(g.fNeutralPtCut),
  fChargedPtCut(g.fChargedPtCut),
  fEMCALIPDistance(g.fEMCALIPDistance),  fPHOSIPDistance(g.fPHOSIPDistance),  
  fEMCALMinDistance(g.fEMCALMinDistance),  fPHOSMinDistance(g.fPHOSMinDistance), 
  fDecayPi0(g.fDecayPi0)
{
  // cpy ctor
  SetName (g.GetName()) ; 
  SetTitle(g.GetTitle()) ; 

  fPhiEMCALCut[0]=g.fPhiEMCALCut[0];
  fPhiEMCALCut[1]=g.fPhiEMCALCut[1];
  fPhiPHOSCut[0]=g.fPhiPHOSCut[0];
  fPhiPHOSCut[1]=g.fPhiPHOSCut[1];
}

//_________________________________________________________________________
AliAnaGammaMC & AliAnaGammaMC::operator = (const AliAnaGammaMC & source)
{
  // assignment operator

  if(&source == this) return *this;

  fChain = source.fChain ; 
  fESD = source.fESD ;
  fCTSList = source.fCTSList;  
  fEMCALList = source.fEMCALList;
  fPHOSList = source.fPHOSList;  
  fPartonList = source.fPartonList;  
  fCalorimeter = source. fCalorimeter ;  
  fCTSEtaCut = source.fCTSEtaCut;  
  fEMCALEtaCut = source.fEMCALEtaCut;  
  fPHOSEtaCut = source.fPHOSEtaCut;
  fNeutralPtCut = source.fNeutralPtCut;
  fChargedPtCut = source.fChargedPtCut; 
  fEMCALIPDistance = source.fEMCALIPDistance; 
  fPHOSIPDistance = source.fPHOSIPDistance; 
  fEMCALMinDistance = source.fEMCALMinDistance; 
  fPHOSMinDistance = source.fPHOSMinDistance; 
  fDecayPi0 = source.fDecayPi0;
  fPhiEMCALCut[0]=source.fPhiEMCALCut[0];
  fPhiEMCALCut[1]=source.fPhiEMCALCut[1];
  fPhiPHOSCut[0]=source.fPhiPHOSCut[0];
  fPhiPHOSCut[1]=source.fPhiPHOSCut[1];

  return *this;

}

//____________________________________________________________________________
AliAnaGammaMC::~AliAnaGammaMC() 
{
  // Remove all pointers
  fCTSList->Clear() ; 
  delete fCTSList ;
  fEMCALList->Clear() ; 
  delete fEMCALList ;
  fPHOSList->Clear() ; 
  delete fPHOSList ;
  fPartonList->Clear() ; 
  delete fPartonList ;

}

//______________________________________________________________________________
void AliAnaGammaMC::ConnectInputData(const Option_t*)
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
void AliAnaGammaMC::CreateOutputObjects()
{  
  //
  // Create the output container
  //
     
  fCTSList = new TClonesArray("TParticle") ; 
  fEMCALList = new TClonesArray("TParticle") ; 
  fPHOSList = new TClonesArray("TParticle") ;
  fPartonList = new TClonesArray("TParticle") ;
}

//____________________________________________________________________________
void AliAnaGammaMC::CreateParticleList(AliStack * stack)
{


  Int_t indexCh     = fCTSList->GetEntries() ;
  Int_t indexEMCAL = fEMCALList->GetEntries() ;
  Int_t indexPHOS = fPHOSList->GetEntries() ;
  Int_t indexParton = fPartonList->GetEntries() ;
  Int_t iParticle = 0 ;
  Double_t charge = 0.;
    
    for (iParticle=0 ; iParticle <  stack->GetNprimary() ; iParticle++) {
     TParticle * particle = stack->Particle(iParticle); 
         //Keep partons
      if(particle->GetStatusCode() == 21 && iParticle>5) {
	//Fill lists
	new((*fPartonList)[indexParton++])       TParticle(*particle) ;
	AliDebug(1,Form("parton %d, e= %f, name = %s",iParticle,particle->Energy(),particle->GetName()));
      }

      //Keep Stable particles 
      if((particle->GetStatusCode() == 1) && (particle->Pt() > 0)){

	charge = TDatabasePDG::Instance()->GetParticle(particle->GetPdgCode())->Charge();

	//---------- Charged particles ----------------------
	if((charge != 0) && (particle->Pt() > fChargedPtCut)){
	  //Particles in CTS acceptance
	  if(TMath::Abs(particle->Eta())<fCTSEtaCut){  
	    //Fill lists
	    new((*fCTSList)[indexCh++])       TParticle(*particle) ;
	    AliDebug(1,Form("charged %d, e= %f, name = %s",iParticle,particle->Energy(),particle->GetName()));
	  }
	}
	//-------------Neutral particles ----------------------
	else if((charge == 0) && particle->Pt() > fNeutralPtCut &&  
		TMath::Abs(particle->GetPdgCode())>16){//Avoid neutrinos

	  if((particle->Phi()>fPhiPHOSCut[0]) && 
	     (particle->Phi()<fPhiPHOSCut[1]) &&
	     (TMath::Abs(particle->Eta())<fPHOSEtaCut))
	    {//Is in PHOS
	      if(particle->GetPdgCode()!=111){
		new((*fPHOSList)[indexPHOS++])       TParticle(*particle) ;
		AliDebug(1,Form("neutral PHOS %d, e= %f, name = %s",iParticle,particle->Energy(),particle->GetName()));
	      }
	      else
		MakePi0Decay("PHOS",particle,fPHOSList,indexPHOS);
	    }//is in PHOS

	  if((particle->Phi()>fPhiEMCALCut[0]) && 
	     (particle->Phi()<fPhiEMCALCut[1]) &&
	     (TMath::Abs(particle->Eta())<fEMCALEtaCut))
	    {//Is in EMCAL 
	      if(particle->GetPdgCode()!=111){
		new((*fEMCALList)[indexEMCAL++])       TParticle(*particle) ;
		AliDebug(1,Form("neutral EMCAL %d, e= %f, name = %s",iParticle,particle->Energy(),particle->GetName()));
	      }
	      else
		MakePi0Decay("EMCAL",particle,fEMCALList,indexEMCAL);
	    }//is in EMCAL
	}//neutral particle
      }//stable particle
    }//particle loop
}

//___________________________________________________________________________
void AliAnaGammaMC::MakePi0Decay(TString calo,TParticle * particle, 
				 TClonesArray * plCalo, Int_t &indexCalo){
  
  //Decays pi0, see if aperture angle is small and then add the pi0 or the 2 gamma
  
  TLorentzVector pPi0, pGamma1, pGamma2 ;
  Double_t angle = 0, cellDistance = 0., mindist=0, minphi=0, maxphi=0, etacut=0;
  
  pPi0.SetPxPyPzE(particle->Px(),particle->Py(),particle->Pz(),particle->Energy());
  
  //Decay
  Pi0Decay(pPi0,pGamma1,pGamma2,angle);
  
  if(calo == "PHOS"){
    cellDistance = angle*fPHOSIPDistance;
    mindist=fPHOSMinDistance;
    minphi= fPhiPHOSCut[0];
    maxphi= fPhiPHOSCut[1];
    etacut =fPHOSEtaCut;
  } 
  else if(calo == "EMCAL"){
    cellDistance = angle*fEMCALIPDistance;
    mindist=fEMCALMinDistance;
    minphi= fPhiEMCALCut[0];
    maxphi= fPhiEMCALCut[1];
    etacut =fEMCALEtaCut;
  } 
  else
    AliFatal("Wrong Calorimeter");
  
  if (cellDistance < mindist) {
    //Pi0 overlapped, add it to the list
    new((*plCalo)[indexCalo++])       TParticle(*particle) ;
    AliDebug(1,Form("neutral %s, e= %f, name = pi0",calo.Data(), particle->Energy()));
  }//overlapped
  else{
    //Not overlapped
    TParticle * photon1 = new TParticle(22,1,0,0,0,0,pGamma1.Px(),pGamma1.Py(),
					pGamma1.Pz(),pGamma1.E(),
					0,0,0,0);    
    TParticle * photon2 = new TParticle(22,1,0,0,0,0,pGamma2.Px(),pGamma2.Py(),
					pGamma2.Pz(),pGamma2.E(),0,0,0,0);
    if(photon1->Pt() > fNeutralPtCut && 
       TMath::Abs(photon1->Eta())<etacut &&
       photon1->Phi()>minphi && photon1->Phi()<maxphi){
      
      new((*plCalo)[indexCalo++])       TParticle(*photon1) ;
      AliDebug(1,Form("neutral %s, e= %f, name = gammadecay1",calo.Data(), particle->Energy()));
    }// photon 1 of pi0 in acceptance
    
    if(photon2->Pt() > fNeutralPtCut && 
       TMath::Abs(photon2->Eta())<etacut &&
       photon2->Phi()>minphi && photon2->Phi()<maxphi){
      
      new((*plCalo)[indexCalo++])       TParticle(*photon2) ;
      AliDebug(1,Form("neutral %s, e= %f, name = gammadecay2",calo.Data(), particle->Energy()));
    }// photon 2 of pi0 in acceptance	
    
  }//Not overlapped gamma
}

//____________________________________________________________________________
void AliAnaGammaMC::Exec(Option_t *) 
{
  
  // Processing of one event
  //Get ESDs
  Long64_t ientry = fChain->GetReadEntry() ;
  if (AliAnalysisTaskRL::GetEntry(ientry) == kFALSE) {
    printf("Couldn't get event from the runLoader\n");
    return;
  } 
 
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  }
  
  AliStack* stack = GetStack();
  if (!stack) {
  AliDebug(AliLog::kError, "Stack not available");
    //return kFALSE;
  }

  if ( !((ientry)%100) ) 
     AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), ientry)) ; 

  //Fill particle lists 
  CreateParticleList(stack); 
  
  AliDebug(2, "End of analysis");
  
}    

//_______________________________________________________________
void AliAnaGammaMC::InitParameters()
{
 
  //Initialize the parameters of the analysis.
  fCalorimeter="PHOS";

  //Fill particle lists when PID is ok
  fEMCALMinDistance    = 10. ;  
  fPHOSMinDistance    = 3.6 ;
  fEMCALIPDistance    = 450. ;//cm  
  fPHOSIPDistance    = 460. ;//cm
  fCTSEtaCut         = 0.7 ;  
  fEMCALEtaCut         = 0.7 ;  
  fPHOSEtaCut         = 0.12 ;
  fPhiEMCALCut[0] = 80. *TMath::Pi()/180.;
  fPhiEMCALCut[1] = 190.*TMath::Pi()/180.;
  fPhiPHOSCut[0] = 220. *TMath::Pi()/180.;
  fPhiPHOSCut[1] = 320.*TMath::Pi()/180.;
  fNeutralPtCut   = 0.5 ;
  fChargedPtCut   = 0.5 ;
  fDecayPi0 = kFALSE;
}

//____________________________________________________________________________
void AliAnaGammaMC::Pi0Decay(TLorentzVector &p0, TLorentzVector &p1, 
			     TLorentzVector &p2, Double_t &angle) {
  // Perform isotropic decay pi0 -> 2 photons
  // p0 is pi0 4-momentum (inut)
  // p1 and p2 are photon 4-momenta (output)
  //  cout<<"Boost vector"<<endl;
  Double_t mPi0 = TDatabasePDG::Instance()->GetParticle(111)->Mass();
  TVector3 b = p0.BoostVector();
  //cout<<"Parameters"<<endl;
  //Double_t mPi0   = p0.M();
  Double_t phi    = TMath::TwoPi() * gRandom->Rndm();
  Double_t cosThe = 2 * gRandom->Rndm() - 1;
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t sinThe = TMath::Sqrt(1-cosThe*cosThe);
  Double_t ePi0   = mPi0/2.;
  //cout<<"ePi0 "<<ePi0<<endl;
  //cout<<"Components"<<endl;
  p1.SetPx(+ePi0*cosPhi*sinThe);
  p1.SetPy(+ePi0*sinPhi*sinThe);
  p1.SetPz(+ePi0*cosThe);
  p1.SetE(ePi0);
  //cout<<"p1: "<<p1.Px()<<" "<<p1.Py()<<" "<<p1.Pz()<<" "<<p1.E()<<endl;
  //cout<<"p1 Mass: "<<p1.Px()*p1.Px()+p1.Py()*p1.Py()+p1.Pz()*p1.Pz()-p1.E()*p1.E()<<endl;
  p2.SetPx(-ePi0*cosPhi*sinThe);
  p2.SetPy(-ePi0*sinPhi*sinThe);
  p2.SetPz(-ePi0*cosThe);
  p2.SetE(ePi0);
  //cout<<"p2: "<<p2.Px()<<" "<<p2.Py()<<" "<<p2.Pz()<<" "<<p2.E()<<endl;
  //cout<<"p2 Mass: "<<p2.Px()*p2.Px()+p2.Py()*p2.Py()+p2.Pz()*p2.Pz()-p2.E()*p2.E()<<endl;
  //cout<<"Boost "<<b.X()<<" "<<b.Y()<<" "<<b.Z()<<endl;
  p1.Boost(b);
  //cout<<"p1: "<<p1.Px()<<" "<<p1.Py()<<" "<<p1.Pz()<<" "<<p1.E()<<endl;
  p2.Boost(b);
  //cout<<"p2: "<<p2.Px()<<" "<<p2.Py()<<" "<<p2.Pz()<<" "<<p2.E()<<endl;
  //cout<<"angle"<<endl;
  angle = p1.Angle(p2.Vect());
  //cout<<angle<<endl;
}

//________________________________________________________________
void AliAnaGammaMC::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  Info("Print", "%s %s", GetName(), GetTitle() ) ;
  printf("Prompt gamma Calorimeter            =     %s\n", fCalorimeter.Data()) ; 
  printf("CTS Eta cut           : %f\n", fCTSEtaCut) ;
  printf("EMCAL Eta cut           : %f\n", fEMCALEtaCut) ;
  printf("PHOS Eta cut           : %f\n", fPHOSEtaCut) ;
  printf("Phi EMCAL cut           : [%f, %f]\n", fPhiEMCALCut[0],fPhiEMCALCut[1]) ;
  printf("Phi PHOS cut           : [%f, %f]\n", fPhiPHOSCut[0],fPhiPHOSCut[1]) ;
  printf("pT neutral cut           : %f GeV/c\n", fNeutralPtCut) ;
  printf("pT charged cut           : %f GeV/c\n", fChargedPtCut) ;
  printf("IP distance to PHOS         : %f\n", fPHOSIPDistance) ;
  printf("IP distance to EMCAL         : %f\n", fEMCALIPDistance) ;
  printf("Min gamma decay distance in PHOS         : %f\n", fPHOSMinDistance) ;
  printf("Min gamma decay distance in EMCAL         : %f\n", fEMCALMinDistance) ;
  printf("Decay Pi0?          : %d\n", fDecayPi0) ;

} 

void AliAnaGammaMC::Terminate(Option_t *)
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
    

}

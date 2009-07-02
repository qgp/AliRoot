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
 * about the suitability of this software for any purpose. It is         *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id:  $ */

//_________________________________________________________________________
// Class for reading data (Kinematics) in order to do prompt gamma 
// or other particle identification and correlations
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include <TParticle.h>
#include <TH2.h>
#include <TChain.h>
#include <TRandom.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>

//---- ANALYSIS system ----
#include "AliCaloTrackMCReader.h" 
#include "Riostream.h"
#include "AliLog.h"
#include "AliGenEventHeader.h"
#include "AliStack.h"
#include "AliAODCaloCluster.h"
#include "AliAODTrack.h"

ClassImp(AliCaloTrackMCReader)

//____________________________________________________________________________
AliCaloTrackMCReader::AliCaloTrackMCReader() : 
  AliCaloTrackReader(), fDecayPi0(0), 
  fNeutralParticlesArray(0x0),    fChargedParticlesArray(0x0), 
  fStatusArray(0x0), fKeepAllStatus(0), fClonesArrayType(0)
{
  //Ctor
  
  //Initialize parameters
  InitParameters();
  fDataType = kMC;  
  
}

//____________________________________________________________________________
AliCaloTrackMCReader::AliCaloTrackMCReader(const AliCaloTrackMCReader & g) :   
  AliCaloTrackReader(g), fDecayPi0(g.fDecayPi0), 
  fNeutralParticlesArray(g.fNeutralParticlesArray?new TArrayI(*g.fNeutralParticlesArray):0x0),
  fChargedParticlesArray(g.fChargedParticlesArray?new TArrayI(*g.fChargedParticlesArray):0x0),
  fStatusArray(g.fStatusArray?new TArrayI(*g.fStatusArray):0x0),
  fKeepAllStatus(g.fKeepAllStatus), fClonesArrayType(g.fClonesArrayType)
{
  // cpy ctor
}

//_________________________________________________________________________
AliCaloTrackMCReader & AliCaloTrackMCReader::operator = (const AliCaloTrackMCReader & source)
{
  // assignment operator

  if(&source == this) return *this;

  fDecayPi0 = source.fDecayPi0; 

  delete fChargedParticlesArray;
  fChargedParticlesArray = source.fChargedParticlesArray?new TArrayI(*source.fChargedParticlesArray):0x0;

  delete fNeutralParticlesArray;
  fNeutralParticlesArray = source.fNeutralParticlesArray?new TArrayI(*source.fNeutralParticlesArray):0x0;

  delete fStatusArray;
  fStatusArray = source.fStatusArray?new TArrayI(*source.fStatusArray):0x0;
 
  fKeepAllStatus = source.fKeepAllStatus ;
  fClonesArrayType = source.fClonesArrayType ;

  return *this;

}

//_________________________________
AliCaloTrackMCReader::~AliCaloTrackMCReader() {
  //Dtor

  if(fChargedParticlesArray) delete fChargedParticlesArray ;
  if(fNeutralParticlesArray) delete fNeutralParticlesArray ;
  if(fStatusArray) delete fStatusArray ;

}

//____________________________________________________________________________
void AliCaloTrackMCReader::GetVertex(Double_t  v[3]) {
  //Return vertex position

  TArrayF pv;
  GetGenEventHeader()->PrimaryVertex(pv);
  v[0]=pv.At(0);
  v[1]=pv.At(1);
  v[2]=pv.At(2);

}


//_______________________________________________________________
void AliCaloTrackMCReader::InitParameters()
{
  
  //Initialize the parameters of the analysis.

  fDecayPi0 = kFALSE;

  fChargedParticlesArray = new TArrayI(1);
  fChargedParticlesArray->SetAt(11,0);  
  //Int_t pdgarray[]={12,14,16};// skip neutrinos
  //fNeutralParticlesArray = new TArrayI(3, pdgarray);
  fNeutralParticlesArray = new TArrayI(3);
  fNeutralParticlesArray->SetAt(12,0); fNeutralParticlesArray->SetAt(14,1); fNeutralParticlesArray->SetAt(16,2); 
  fStatusArray = new TArrayI(1);
  fStatusArray->SetAt(1,0); 
 
  fKeepAllStatus = kTRUE;
  fClonesArrayType = kAliAOD ;

}

//____________________________________________________________________________
void  AliCaloTrackMCReader::FillCalorimeters(const Int_t iParticle, TParticle* particle, TLorentzVector momentum,
					     Int_t &indexPHOS, Int_t &indexEMCAL){
  //Fill AODCaloClusters or TParticles lists of PHOS or EMCAL

  //In PHOS
  if(fFillPHOS && fFidutialCut->IsInFidutialCut(momentum,"PHOS") && momentum.Pt() > fPHOSPtMin){
    
    if(fClonesArrayType == kTParticle) new((*fAODPHOS)[indexPHOS++])       TParticle(*particle) ;
    else{
      
      Char_t ttype= AliAODCluster::kPHOSNeutral;
      Int_t labels[] = {iParticle};
      Float_t x[] = {momentum.X(), momentum.Y(), momentum.Z()};
      AliAODCaloCluster *calo = new((*fAODPHOS)[indexPHOS++]) 
	AliAODCaloCluster(iParticle,1,labels,momentum.E(), x, NULL, ttype, 0);
      SetCaloClusterPID(particle->GetPdgCode(),calo) ;
    }
  }
  //In EMCAL
  else if(fFillEMCAL && fFidutialCut->IsInFidutialCut(momentum,"EMCAL") && momentum.Pt() > fEMCALPtMin){
    //cout<<"Reader : E "<<momentum.E()<<" ; Phi"<<momentum.Phi()<< " ; Eta "<<momentum.Eta()<<endl;
    if(fClonesArrayType == kTParticle) new((*fAODEMCAL)[indexEMCAL++])       TParticle(*particle) ;
    else{
      Char_t ttype= AliAODCluster::kEMCALClusterv1;
      Int_t labels[] = {iParticle};
      Float_t x[] = {momentum.X(), momentum.Y(), momentum.Z()};
      AliAODCaloCluster *calo = new((*fAODEMCAL)[indexEMCAL++]) 
	AliAODCaloCluster(iParticle,1,labels,momentum.E(), x, NULL, ttype, 0);
      SetCaloClusterPID(particle->GetPdgCode(),calo) ;  
    }
  }
}

//____________________________________________________________________________
void AliCaloTrackMCReader::FillInputEvent()
{
  //Create list of particles from EMCAL, PHOS and CTS. 

  if(fClonesArrayType == kTParticle){
    fAODCTS = new TClonesArray("TParticle",0);
    fAODEMCAL = new TClonesArray("TParticle",0);
    fAODPHOS = new TClonesArray("TParticle",0);
  }
  else if(fClonesArrayType == kAliAOD){
   fAODCTS = new TClonesArray("AliAODTrack",0);
   fAODPHOS = new TClonesArray("AliAODCaloCluster",0);
   fAODEMCAL = new TClonesArray("AliAODCaloCluster",0);
  }
  else {AliFatal("Wrong clones type");}


  Int_t indexCh      = 0 ;
  Int_t indexEMCAL = 0 ;
  Int_t indexPHOS = 0 ;
  
  Int_t iParticle = 0 ;
  Double_t charge = 0.;
  
  for (iParticle=0 ; iParticle <  GetStack()->GetNprimary() ; iParticle++) {
    TParticle * particle = GetStack()->Particle(iParticle); 
    TLorentzVector momentum;
    Float_t p[3];
    Float_t x[3];
    Int_t pdg = particle->GetPdgCode();

    //Keep particles with a given status 
    if(KeepParticleWithStatus(particle->GetStatusCode()) && (particle->Pt() > 0) ){
      
      charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
      particle->Momentum(momentum);
      //---------- Charged particles ----------------------
      if((charge != 0) && (momentum.Pt() > fCTSPtMin) && (fFidutialCut->IsInFidutialCut(momentum,"CTS"))){
	if(fFillCTS){
	  //Particles in CTS acceptance
	  if(fDebug > 3 && momentum.Pt() > 0.2)printf("Fill MC CTS :: Selected tracks E %3.2f, pt %3.2f, phi %3.2f, eta %3.2f\n",
						      momentum.E(),momentum.Pt(),momentum.Phi()*TMath::RadToDeg(),momentum.Eta());
	  
	  if(fClonesArrayType == kTParticle) new((*fAODCTS)[indexCh++])       TParticle(*particle) ;
	  else{
	    x[0] = particle->Vx(); x[1] = particle->Vy(); x[2] = particle->Vz();
	    p[0] = particle->Px(); p[1] = particle->Py(); p[2] = particle->Pz();
	    AliAODTrack *aodTrack = new((*fAODCTS)[indexCh++]) 
	      AliAODTrack(0, iParticle, p, kTRUE, x, kFALSE,NULL, 0, 0, 
			  NULL,
			  0x0,//primary,
			kFALSE, // No fit performed
			  kFALSE, // No fit performed
			  AliAODTrack::kPrimary, 
			  0);
	    SetTrackChargeAndPID(pdg, aodTrack);
	  }
	}
	//Keep some charged particles in calorimeters lists
	if((fFillPHOS || fFillEMCAL) && KeepChargedParticles(pdg)) FillCalorimeters(iParticle, particle, momentum, indexPHOS, indexEMCAL);

      }//Charged
      
       //-------------Neutral particles ----------------------
      else if(charge == 0 && (fFillPHOS || fFillEMCAL)){
	//Skip neutrinos or other neutral particles
	if(SkipNeutralParticles(pdg) || particle->IsPrimary()) continue ; // protection added (MG)
	
	//Fill particle/calocluster arrays
	if(!fDecayPi0) FillCalorimeters(iParticle, particle, momentum, indexPHOS, indexEMCAL);
	else {
	  //Sometimes pi0 are stable for the generator, if needed decay it by hand
	  if(pdg == 111 ){
	    if(momentum.Pt() >  fPHOSPtMin || momentum.Pt() >  fEMCALPtMin){
	      TLorentzVector lvGamma1, lvGamma2 ;
	      //Double_t angle = 0;
	      
	      //Decay
	      MakePi0Decay(momentum,lvGamma1,lvGamma2);//,angle);
	      
	      //Check if Pi0 is in the acceptance of the calorimeters, if aperture angle is small, keep it
	      TParticle * pPhoton1 = new TParticle(22,1,iParticle,0,0,0,lvGamma1.Px(),lvGamma1.Py(),
						   lvGamma1.Pz(),lvGamma1.E(),0,0,0,0);   
	      TParticle * pPhoton2 = new TParticle(22,1,iParticle,0,0,0,lvGamma2.Px(),lvGamma2.Py(),
						   lvGamma2.Pz(),lvGamma2.E(),0,0,0,0);
	      //Fill particle/calocluster arrays
	      FillCalorimeters(iParticle,pPhoton1,lvGamma1, indexPHOS, indexEMCAL);
	      FillCalorimeters(iParticle,pPhoton2,lvGamma2, indexPHOS, indexEMCAL);
	    }//pt cut
	  }//pi0
	  else FillCalorimeters(iParticle,particle, momentum, indexPHOS, indexEMCAL); //Add the rest
	}
      }//neutral particles
     } //particle with correct status
   }//particle loop

}

//________________________________________________________________
Bool_t AliCaloTrackMCReader::KeepParticleWithStatus(Int_t status) const {
  //Check if status is equal to one of the  list
  //These particles will be used in analysis.
  if(!fKeepAllStatus){
    for(Int_t i= 0; i < fStatusArray->GetSize(); i++)
      if(status ==  fStatusArray->At(i)) return kTRUE ;

    return kFALSE; 
    
  }
  else
    return kTRUE ;  
}

//________________________________________________________________
Bool_t AliCaloTrackMCReader::KeepChargedParticles(Int_t pdg) const {
  //Check if pdg is equal to one of the charged particles list
  //These particles will be added to the calorimeters lists.

  for(Int_t i= 0; i < fChargedParticlesArray->GetSize(); i++)
    if(TMath::Abs(pdg) ==  fChargedParticlesArray->At(i)) return kTRUE ;
  
  return kFALSE; 
  
}

//________________________________________________________________
void AliCaloTrackMCReader::Print(const Option_t * opt) const
{
  //Print some relevant parameters set for the analysis
  if(! opt)
    return;
  
  Info("**** Print **** ", "%s %s", GetName(), GetTitle() ) ;
  
  printf("Decay Pi0?          : %d\n", fDecayPi0) ;
  printf("TClonesArray type   : %d\n", fClonesArrayType) ;
  printf("Keep all status?    : %d\n", fKeepAllStatus) ;
  
  if(!fKeepAllStatus) printf("Keep particles with status : ");
  for(Int_t i= 0; i < fStatusArray->GetSize(); i++)
    printf(" %d ; ", fStatusArray->At(i));
  printf("\n");
  
  printf("Skip neutral particles in calo : ");
  for(Int_t i= 0; i < fNeutralParticlesArray->GetSize(); i++)
    printf(" %d ; ", fNeutralParticlesArray->At(i));
  printf("\n");
  
  printf("Keep charged particles in calo : ");
  for(Int_t i= 0; i < fChargedParticlesArray->GetSize(); i++)
    printf(" %d ; ", fChargedParticlesArray->At(i));
  printf("\n");

}

//____________________________________________________________________________
void AliCaloTrackMCReader::MakePi0Decay(TLorentzVector &p0, TLorentzVector &p1, 
				TLorentzVector &p2){//, Double_t &angle) {
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
  //angle = p1.Angle(p2.Vect());
  //cout<<angle<<endl;
}

//____________________________________________________________________________
void AliCaloTrackMCReader::SetInputEvent(TObject* /*esd*/, TObject* /*aod*/, TObject* mc) {
  // Connect the data pointer
  SetMC((AliMCEvent*) mc);
}


//________________________________________________________________
Bool_t AliCaloTrackMCReader::SkipNeutralParticles(Int_t pdg) const {
  //Check if pdg is equal to one of the neutral particles list
  //These particles will be skipped from analysis.

  for(Int_t i= 0; i < fNeutralParticlesArray->GetSize(); i++)
    if(TMath::Abs(pdg) ==  fNeutralParticlesArray->At(i)) return kTRUE ;
  
  return kFALSE; 
  
}


//____________________________________________________________________
void AliCaloTrackMCReader::SetTrackChargeAndPID(const Int_t pdgCode, AliAODTrack *track) {

  Float_t PID[10] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

  switch (pdgCode) {

  case 22: // gamma
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 11: // e- 
    track->SetCharge(-1);
    PID[AliAODTrack::kElectron] = 1.;
    track->SetPID(PID);
    break;
    
  case -11: // e+
    track->SetCharge(+1);
    PID[AliAODTrack::kElectron] = 1.;
    track->SetPID(PID);
    break;
    
  case 13: // mu- 
    track->SetCharge(-1);
    PID[AliAODTrack::kMuon] = 1.;
    track->SetPID(PID);
    break;
    
  case -13: // mu+
    track->SetCharge(+1);
    PID[AliAODTrack::kMuon] = 1.;
    track->SetPID(PID);
    break;
    
  case 111: // pi0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case 211: // pi+
    track->SetCharge(+1);
    PID[AliAODTrack::kPion] = 1.;
    track->SetPID(PID);
    break;
    
  case -211: // pi-
    track->SetCharge(-1);
    PID[AliAODTrack::kPion] = 1.;
    track->SetPID(PID);
    break;
    
  case 130: // K0L
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case 321: // K+
    track->SetCharge(+1);
    PID[AliAODTrack::kKaon] = 1.;
    track->SetPID(PID);
    break;
    
  case -321: // K- 
    track->SetCharge(-1);
    PID[AliAODTrack::kKaon] = 1.;
    track->SetPID(PID);
    break;
    
  case 2112: // n
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case 2212: // p
    track->SetCharge(+1);
    PID[AliAODTrack::kProton] = 1.;
    track->SetPID(PID);
    break;
    
  case -2212: // anti-p
    track->SetCharge(-1);
    PID[AliAODTrack::kProton] = 1.;
    track->SetPID(PID);
    break;

  case 310: // K0S
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case 311: // K0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case -311: // anti-K0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;
    
  case 221: // eta
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3122: // lambda
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3222: // Sigma+
    track->SetCharge(+1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3212: // Sigma0
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3112: // Sigma-
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3322: // Xi0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3312: // Xi-
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 3334: // Omega-
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -2112: // n-bar
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3122: // anti-Lambda
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3222: // anti-Sigma-
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3212: // anti-Sigma0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3112: // anti-Sigma+
    track->SetCharge(+1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3322: // anti-Xi0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -3312: // anti-Xi+
    track->SetCharge(+1);
    break;

  case -3334: // anti-Omega+
    track->SetCharge(+1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 411: // D+
    track->SetCharge(+1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -411: // D- 
    track->SetCharge(-1);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case 421: // D0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  case -421: // anti-D0
    track->SetCharge(0);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
    break;

  default : // unknown
    track->SetCharge(-99);
    PID[AliAODTrack::kUnknown] = 1.;
    track->SetPID(PID);
 }

  return;
}

//____________________________________________________________________
void AliCaloTrackMCReader::SetCaloClusterPID(const Int_t pdgCode, AliAODCaloCluster *calo) {

  Float_t PID[9] = {0., 0., 0., 0., 0., 0., 0., 0., 0.};

  switch (pdgCode) {

  case 22: // gamma
    PID[AliAODCaloCluster::kPhoton] = 1.;
    calo->SetPID(PID);
    break;

  case 11: // e- 
    PID[AliAODCaloCluster::kElectron] = 1.;
    calo->SetPID(PID);
    break;
    
  case -11: // e+
    PID[AliAODCaloCluster::kElectron] = 1.;
    calo->SetPID(PID);
    break;
    
  case 13: // mu- 
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case -13: // mu+
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case 111: // pi0
    PID[AliAODCaloCluster::kPi0] = 1.;
    calo->SetPID(PID);
    break;
    
  case 211: // pi+
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case -211: // pi-
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case 130: // K0L
    PID[AliAODCaloCluster::kKaon0] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1;
    calo->SetPID(PID);
    break;
    
  case 321: // K+
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case -321: // K- 
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case 2112: // n
    PID[AliAODCaloCluster::kNeutron] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;
    
  case 2212: // p
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;
    
  case -2212: // anti-p
    PID[AliAODCaloCluster::kCharged] = 1.;
    calo->SetPID(PID);
    break;

  case 310: // K0S
    PID[AliAODCaloCluster::kKaon0] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;
    
  case 311: // K0
    PID[AliAODCaloCluster::kKaon0] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;
    
  case -311: // anti-K0
    PID[AliAODCaloCluster::kKaon0] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;
    
  case 221: // eta
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;

  case 3122: // lambda
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3222: // Sigma+
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3212: // Sigma0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3112: // Sigma-
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3322: // Xi0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3312: // Xi-
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 3334: // Omega-
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -2112: // n-bar
    PID[AliAODCaloCluster::kNeutron] = 1.;
    PID[AliAODCaloCluster::kNeutral] = 1.;
    calo->SetPID(PID);
    break;

  case -3122: // anti-Lambda
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3222: // anti-Sigma-
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3212: // anti-Sigma0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3112: // anti-Sigma+
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3322: // anti-Xi0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3312: // anti-Xi+
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -3334: // anti-Omega+
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 411: // D+
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -411: // D- 
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case 421: // D0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  case -421: // anti-D0
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
    break;

  default : // unknown
    PID[AliAODCaloCluster::kUnknown] = 1.;
    calo->SetPID(PID);
 }

  return;
}

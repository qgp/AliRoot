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

//_________________________________________________________________________
// Class for the analysis of gamma-jet correlations 
//   
// 
//*-- Author: Gustavo Conesa & Yves Schutz (IFIC, CERN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

#include "TString.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TList.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TParticle.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TPad.h"
#include "TArrayC.h"
#include "AliRun.h"
#include "AliPHOSGammaJet.h" 
#include "AliPHOSGetter.h" 
#include "../PYTHIA6/AliGenPythia.h" 
ClassImp(AliPHOSGammaJet)

//____________________________________________________________________________
AliPHOSGammaJet::AliPHOSGammaJet() : TTask() 
{
  // ctor
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.Reset(0.);
  fEtaCut           = 0 ;
  fFastRec          = 0 ;
  fInvMassMaxCut    = 0 ;
  fInvMassMinCut    = 0 ;
  fJetJet           = 0 ;
  fJetQuenching     = 0 ;
  fJetRatioMaxCut   = 0 ;
  fJetRatioMinCut   = 0 ;
  fNEvent           = 0 ;
  fNIter            = 0 ;
  fMinDistance      = 0 ;
  fOnlyCharged      = kFALSE ;
  fOptFast          = kFALSE ;
  fPhiMaxCut        = 0 ;
  fPhiMinCut        = 0 ;
  fRatioMaxCut      = 0 ;
  fRatioMinCut      = 0 ;
  fScalarProductCut = 0;
              
  fOutputFile     = new TFile(gDirectory->GetName()) ;
  fInputFileName  = gDirectory->GetName() ;
  fOutputFileName = gDirectory->GetName() ;
  fHIJINGFileName = gDirectory->GetName() ;
  fJetQuenchingFileName = gDirectory->GetName() ;
  fHIJING        = 0 ;
  fPosParaA      = 0. ;                      
  fPosParaB      = 0. ;
  //fPyth1         = new AliGenPythia();
  //fPyth2         = new AliGenPythia();
  fRan           = 0 ;                            
  fResPara1      = 0. ;                       
  fResPara2      = 0. ;                        
  fResPara3      = 0. ;  
        
  fListHistos     = new TObjArray(100) ;
  TList * list = gDirectory->GetListOfKeys() ; 
  TIter next(list) ; 
  TH2F * h = 0 ;
  Int_t index ; 
  for (index = 0 ; index < list->GetSize()-1 ; index++) { //-1 to avoid GammaJet Task
    h = dynamic_cast<TH2F*>(gDirectory->Get(list->At(index)->GetName())) ; 
    fListHistos->Add(h) ; 
  }
  List() ; 
}

//____________________________________________________________________________
AliPHOSGammaJet::AliPHOSGammaJet(const TString inputfilename) : 
  TTask("GammaJet","Analysis of gamma-jet correlations")
{
  // ctor 
  fInputFileName = inputfilename;
  fJetQuenchingFileName = inputfilename;
  fFastRec = new AliPHOSFastGlobalReconstruction(fInputFileName);  
  AliPHOSGetter *  gime = AliPHOSGetter::GetInstance(fInputFileName) ;
  fNEvent = gime->MaxEvent();
  InitParameters();
  fListHistos = 0 ;
  //InitJetQuenching(fInputFileName,fJetQuenchingFileName);
  //InitJetQuenching(fInputFileName, fPyth1);
//    AliGenPythia* fPyth1 = (AliGenPythia*) gAlice->Generator();
//    fPyth1->Init();
//    fPyth2 = fPyth2;

//    TFile *f1=new TFile(fInputFileName,"read");
//    AliRun * alice1 = (AliRun*)(f1->Get("gAlice"));
//    fPyth1 = (AliGenPythia*) alice1->Generator();
//    fPyth1->Init();
  
//    TFile *f2=new TFile(fJetQuenchingFileName,"read");
//    AliRun * alice2 = (AliRun*)(f2->Get("gAlice"));
//    fPyth2 = (AliGenPythia*) alice2->Generator();
//    fPyth2->Init();


}

//____________________________________________________________________________
AliPHOSGammaJet::AliPHOSGammaJet(const AliPHOSGammaJet & gj) 
{
  // cpy ctor
  fAngleMaxParam    = gj.fAngleMaxParam;
  fScalarProductCut = gj.fScalarProductCut;
  fEtaCut           = gj.fEtaCut ;
  fInvMassMaxCut    = gj.fInvMassMaxCut ;
  fInvMassMinCut    = gj.fInvMassMinCut ;
  fFastRec          = gj.fFastRec ;
  fMinDistance      = gj.fMinDistance ;
  fOptFast          = gj.fOptFast ;
  fOnlyCharged      = gj.fOnlyCharged ;
  fOutputFile       = gj.fOutputFile ;
  fInputFileName    = gj.fInputFileName ;
  fOutputFileName   = gj.fOutputFileName ;
  fHIJINGFileName   = gj.fHIJINGFileName ;
  fHIJING           = gj.fHIJING ;
  fJetQuenchingFileName = gj.fJetQuenchingFileName;
  fRatioMaxCut      = gj.fRatioMaxCut ;
  fRatioMinCut      = gj.fRatioMinCut ;
  fJetJet           = gj.fJetJet;
  fJetQuenching     = gj.fJetQuenching ;
  fJetRatioMaxCut   = gj.fJetRatioMaxCut ;
  fJetRatioMinCut   = gj.fJetRatioMinCut ;
  fNEvent           = gj.fNEvent ;  
  fNIter            = gj.fNIter ;
  fResPara1         = gj.fResPara1 ;    
  fResPara2         = gj.fResPara2 ; 
  fResPara3         = gj.fResPara3 ; 
  fPhiMaxCut        = gj.fPhiMaxCut  ;
  fPhiMinCut        = gj.fPhiMinCut ;
  fPosParaA         = gj.fPosParaA ;    
  fPosParaB         = gj.fPosParaB ;  
  //fPyth1             = gj.fPyth1 ;
  //fPyth2             = gj.fPyth2 ;
  SetName(gj.GetName()) ; 
  SetTitle(gj.GetTitle()) ; 
}

//____________________________________________________________________________
AliPHOSGammaJet::~AliPHOSGammaJet() 
{
  fOutputFile->Close() ;

}

//____________________________________________________________________________
void AliPHOSGammaJet::AddHIJINGToList(TList & particleList, const Int_t iEvent, const TLorentzVector gamma) 
{
  TParticle * particle = new TParticle();
  
  TLorentzVector pPi0, pGamma1, pGamma2 ;
  Double_t angle = 0;
  
  Int_t n = -1; 
  
  AliPHOSFastGlobalReconstruction *fastRec = new AliPHOSFastGlobalReconstruction(fHIJINGFileName);  
  AliPHOSGetter * gime = AliPHOSGetter::GetInstance(fHIJINGFileName) ; 
  
  Int_t nEvent = gime->MaxEvent() ;
  Int_t iiEvent =   iEvent%nEvent;
  
  gime->Event(iiEvent, "P") ;
  
  if(fOptFast)
    fastRec->FastReconstruction(iiEvent,kTRUE,fEtaCut,gamma.Phi()+fPhiMinCut,gamma.Phi()+fPhiMaxCut);
  
  //cout<<nparticles<<endl;
  Int_t  nparticles = gime->NPrimaries() ; 
  Int_t iParticle=0 ;
  if(fOptFast){
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      //Keep original partons
      particle = new TParticle(*(gime->Primary(iParticle))) ;
      //particle = (TParticle*)fAlice->Particle(iParticle) ;
      if((particle->GetStatusCode() == 0)){
	// Keep particles different from Pi0
	if(particle->Pt()>0.)
	  if((particle->GetPdgCode() != 111)
	     &&(TMath::Abs(particle->Eta())<fEtaCut)
	     &&(particle->Phi()>gamma.Phi()+fPhiMinCut)
	     &&(particle->Phi()<gamma.Phi()+fPhiMaxCut)){
	    if(particle->GetPdgCode()==22){
	      //Info("AddHIJINGToList","Photon0 before: %f", particle->Energy());
	      MakePhoton(particle);
	      //Info("AddHIJINGToList","Photon0 after: %f", particle->Energy());
	    }
	    particleList.Add(particle);
	    n++;
	  } 
	//Decay Pi0 and keep it with different status name
	//Keep decay photons
	if(particle->GetPdgCode() == 111) {
	  //cout<<"Pi0 "<<endl;
	  n += 3 ; 
	  particle->Momentum(pPi0);
	  Pi0Decay(particle->GetMass(),pPi0,pGamma1,pGamma2,angle);
	  dynamic_cast<TH2F*>(fListHistos->FindObject("AnglePair"))->Fill(pPi0.E(),angle);
	  TParticle * photon1 = new TParticle(22,1,0,0,0,0,pGamma1.Px(),pGamma1.Py(),
					      pGamma1.Pz(),pGamma1.E(),0,0,0,0);
	  TParticle * photon2 = new TParticle(22,1,0,0,0,0,pGamma2.Px(),pGamma2.Py(),
					      pGamma2.Pz(),pGamma2.E(),0,0,0,0);
	  //photon1->SetWeight(1);
	  //photon2->SetWeight(2);
	  particle->SetStatusCode(2);
	  
	  //Info("AddHIJINGToList","Photon 1 and 2 before: %f  %f", photon1->Energy(), photon2->Energy());
	  MakePhoton(photon1);
	  MakePhoton(photon2);
	  //Info("AddHIJINGToList","Photon 1 and 2 after: %f  %f", photon1->Energy(), photon2->Energy());
	  //particleList.Add(particle);
	  if((TMath::Abs(photon1->Eta())<fEtaCut)
	     &&(photon1->Phi()>(gamma.Phi()+fPhiMinCut))
	     &&(photon1->Phi()<(gamma.Phi()+fPhiMaxCut)))
	    particleList.Add(photon1);
	  if((TMath::Abs(photon2->Eta())<fEtaCut)
	     &&(photon2->Phi()>(gamma.Phi()+fPhiMinCut))
	     &&(photon2->Phi()<(gamma.Phi()+fPhiMaxCut)))
	    particleList.Add(photon2);
	  //photon1->Print();
	  //photon2->Print();  
	}//if pi0
      }//final particle, etacut
    }//for (iParticle<nParticle)
  }//optfast
  else{
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      
      //Keep Stable particles within eta range
      particle = new TParticle(*(gime->Primary(iParticle))) ;
      //particle = (TParticle*)fAlice->Particle(iParticle) ;
      //cout<<"Inside loop "<<particle<<endl;
     
      if(particle->Pt()>0.)
	if((particle->GetStatusCode() == 0)
	   &&(TMath::Abs(particle->Eta())<fEtaCut) 
	   &&(particle->Phi()>(gamma.Phi()+fPhiMinCut))
	   &&(particle->Phi()<(gamma.Phi()+fPhiMaxCut))){
	  
	  //cout<<" HIJ code "<<particle->GetStatusCode()<<"  "<<particle->GetName()<<" pt  "<<particle->Energy()
	  //   <<" phi  "<<particle->Phi()<<endl;
	   
	   //cout<<"Inside if"<<endl;
	   particleList.Add(particle);
	   n++;
	  //cout<<"Inside loop, end"<<endl;
	}//final particle, etacut
    }//for (iParticle<nParticle)
  }// no optfast
  //Info("AddHIJINGList","N %d",n);
  
}


//____________________________________________________________________________
void AliPHOSGammaJet::CreateParticleList(AliPHOSGetter * gime, TList & particleList, const Int_t idg) 
{
  //Info("CreateParticleList","Inside");
  TParticle * particle = new TParticle();
  TLorentzVector pPi0, pGamma1, pGamma2 ;
  Double_t angle = 0, cellDistance = 0.;
  
  Int_t n = -1; 
  Int_t  nparticles = gime->NPrimaries() ; 
  Int_t iParticle=0 ;
  if(fOptFast){
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      particle = new TParticle(*(gime->Primary(iParticle))) ;

      //Info("CreateParticleList","after parton");
      //Keep Stable particles within eta range
      
      if((particle->GetStatusCode() == 1)&&(iParticle !=idg)){

	// Keep particles different from Pi0
	if((particle->GetPdgCode() != 111)&&(TMath::Abs(particle->Eta())<fEtaCut)){
	  if(particle->GetPdgCode()==22){
	    //Info("CreateParticleList","Photon0 before: %f", particle->Energy());
	    MakePhoton(particle);
	    //Info("CreateParticleList","Photon0 after: %f", particle->Energy());
	  }
	  particleList.Add(particle);
	  n++;
	} 
	//Decay Pi0 and keep it with different status name
	//Keep decay photons
	if((particle->GetPdgCode() == 111) && (TMath::Abs(particle->Eta())<fEtaCut+2.)) {
	  //cout<<"Pi0 "<<endl;
	  
	  particle->Momentum(pPi0);
	  Pi0Decay(particle->GetMass(),pPi0,pGamma1,pGamma2,angle);

	  cellDistance = angle*460; //cm
	  if (cellDistance < fMinDistance) {
	    n ++ ; 
	    particleList.Add(particle);
	  }// if cell<distance	
	  else {
	    n += 3 ; 
	    dynamic_cast<TH2F*>(fListHistos->FindObject("AnglePair"))->Fill(pPi0.E(),angle);
	    TParticle * photon1 = new TParticle(22,1,0,0,0,0,pGamma1.Px(),pGamma1.Py(),
						pGamma1.Pz(),pGamma1.E(),0,0,0,0);
	    TParticle * photon2 = new TParticle(22,1,0,0,0,0,pGamma2.Px(),pGamma2.Py(),
						pGamma2.Pz(),pGamma2.E(),0,0,0,0);
	    //photon1->SetWeight(1);
	    //photon2->SetWeight(2);
	    particle->SetStatusCode(2);
	    
	    //Info("CreateParticleList","Photon 1 and 2 before: %f  %f", photon1->Energy(), photon2->Energy());
	    MakePhoton(particle);
	    MakePhoton(photon1);
	    MakePhoton(photon2);
	    //Info("CreateParticleList","Photon 1 and 2 after: %f  %f", photon1->Energy(), photon2->Energy());
	    particleList.Add(particle);
	    particleList.Add(photon1);
	    particleList.Add(photon2);
	    //photon1->Print();
	    //photon2->Print();  
	  }//if angle > mindist
	}//if pi0
      }//final particle, etacut
    }//for (iParticle<nParticle)
  }//optfast
  else{
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      particle = new TParticle(*(gime->Primary(iParticle))) ;
      if(particle->GetStatusCode()==1)
	
      //Keep Stable particles within eta range 
      if((particle->GetStatusCode() == 1)&&(TMath::Abs(particle->Eta())<fEtaCut)&&(iParticle !=idg)){
	//cout<<" Creat code "<<particle->GetStatusCode()<<"  "<<particle->GetName()<<"  "<<particle->Energy()<<endl;
	particleList.Add(particle);
	n++;

      }//final particle, etacut
    }//for (iParticle<nParticle)
  }// no optfast
  //Info("CreateParticleList","N %d",n);
  
}
//____________________________________________________________________________
void AliPHOSGammaJet::Exec(Option_t *option) 
{
  // does the job
  
  MakeHistos() ;  

  AliPHOSGetter * gime = AliPHOSGetter::GetInstance() ; 

  //Int_t nEvent1 = gime->MaxEvent() ;   
  //nEvent1=10 ;
  Int_t iEvent = 0 ; 
  for ( iEvent = 0 ; iEvent < fNEvent ; iEvent++) {
    //if (iEvent <= 100 || iEvent%10 == 0)
    Info("Exec", "Event %d", iEvent) ;

    fFastRec = new AliPHOSFastGlobalReconstruction(fInputFileName);
    gime     = AliPHOSGetter::GetInstance(fInputFileName) ; 

    gime->Event(iEvent, "P") ; 

    Int_t idg = -1 ;
    Bool_t isInPHOS = kFALSE;
   
    TLorentzVector gamma = GetGammaJet(gime, isInPHOS, idg); 

    if(isInPHOS){

      //Make fast reconstruction of charged
      if(fOptFast)
	fFastRec->FastReconstruction(iEvent, kFALSE,0.,0.,0.);
     
      //Fill particle list
      TList particleList ; 
 
      //Info("Exec","Begin quenching");    
      if(fJetQuenching)
	MakeJetQuenching(gime,particleList,iEvent, gamma,idg);
      //Info("Exec","End quenching") ;  
      else 
	CreateParticleList(gime, particleList, idg);

      //cout<<"Gamma ?: phi "<<gamma.Phi()<<" theta "<<gamma.Theta()
      //  <<" eta "<<gamma.Eta()<<" E "<<gamma.Energy()<<endl;

      Double_t ptg = gamma.Pt();
      dynamic_cast<TH1F*>(fListHistos->FindObject("NGamma"))->Fill(ptg);
      if(strstr(option,"deb"))
	Info("Exec", "Gamma: %f", ptg) ; 
      
      if(fHIJING)
	AddHIJINGToList(particleList, iEvent, gamma);
     
      if(fOnlyCharged){
	TLorentzVector charge  = GetLeadingCharge(particleList,gamma);
	Double_t ptch = charge.Pt();

	if(ptch>0.){
	  if(strstr(option,"deb"))
	    Info("Exec", "Charge: %f", ptch) ;
	  TString type = "Charge";

	  dynamic_cast<TH2F*>(fListHistos->FindObject("ChargeRatio"))
	    ->Fill(ptg,ptch/ptg);
	  dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiCharge"))
	    ->Fill(ptg,charge.Phi()-gamma.Phi());
	  dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaEtaCharge"))
	    ->Fill(ptg,charge.Eta()-gamma.Eta());
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingCharge"))
	    ->Fill(ptg,ptch);

	  MakeJet(particleList, gamma, charge,type);
	}
      }//only charged
      else{
	TLorentzVector any = GetAnyLeading(particleList,gamma);
	Double_t ptany = any.Pt();
	if(strstr(option,"deb"))
	  Info("Exec", "Any Leading 1: Pt = %f, Phi = %f", ptany,any.Phi()) ;
	
	dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeading"))
	  ->Fill(ptg,ptany);
	if(ptany>0.){
	  
	  TLorentzVector charge  = GetLeadingCharge(particleList,gamma);
	  Double_t ptch = charge.Pt();
	  if(strstr(option,"deb"))
	    Info("Exec", "Charge: %f", ptch) ;
	  
	  
	  TLorentzVector pi0     = GetLeadingPi0(particleList,gamma);  
	  Double_t ptpi = pi0.Pt();
	  if(strstr(option,"deb"))
	    Info("Exec", "Pi0: %f", ptpi) ;
	  
	  
	  //TLorentzVector neutral = GetLeadingNeutral(particleList, gamma);
	  //Double_t ptne = neutral.Pt();	
	  //if(strstr(option,"deb"))
	  //  Info("Exec", "Neutral: %f", ptne) ;
	  
	  TString type = "Any";
	  MakeJet(particleList, gamma, any,type);
	  
	  //if((ptne>ptch)&&(ptne>ptpi))
	  //dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingNeutral"))
	  //->Fill(ptg,neutral.Pt());
	  
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0LeadingBefore"))
	    ->Fill(ptch,ptpi);
	  
	  //Select leading and make jet
	  if(ptch> ptpi){
	    if(strstr(option,"deb"))
	      Info("Exec", "Leading Charge: %f", ptch) ;
	    dynamic_cast<TNtuple*>(fListHistos->FindObject("PtLeadingNT"))
	      ->Fill(ptg, ptch,ptpi, ptany) ;
	    dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0Leading"))
	      ->Fill(ptch,ptpi);
	    dynamic_cast<TH2F*>(fListHistos->FindObject("ChargeRatio"))
	      ->Fill(ptg,ptch/ptg);
	    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiCharge"))
	      ->Fill(ptg,charge.Phi()-gamma.Phi());
	    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaEtaCharge"))
	      ->Fill(ptg,charge.Eta()-gamma.Eta());
	    dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingCharge"))
	      ->Fill(ptg,ptch);
	    type = "Charge";
	    MakeJet(particleList, gamma,charge,type);
	  }
	  else if(ptpi> ptch){
	    if(strstr(option,"deb"))
	      Info("Exec", "Leading Pi0: %f", ptpi) ;
	    dynamic_cast<TNtuple*>(fListHistos->FindObject("PtLeadingNT"))
	      ->Fill(ptg, ptch,ptpi, ptany) ;
	    dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0Leading"))
	      ->Fill(ptch,ptpi);
	    dynamic_cast<TH2F*>(fListHistos->FindObject("Pi0Ratio"))
	      ->Fill(ptg,ptpi/ptg);
	    dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingPi0"))
	      ->Fill(ptg,ptch);
	    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiPi0"))
	      ->Fill(ptg,pi0.Phi()-gamma.Phi());
	    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaEtaPi0"))
	      ->Fill(ptg,pi0.Eta()-gamma.Eta());
	    TString type = "Pi0";
	    MakeJet(particleList, gamma, pi0,type);
	  }//pi0
	  // 	else{
	  // 	  if(strstr(option,"deb"))
	  // 	    Info("Exec", "Leading Neutral: %f", neutral.Pt()) ;
	  // 	}//else
	  
	}	
	else{
	  //<<"  "<<gamma.Phi()<<endl;
	  dynamic_cast<TH2F*>(fListHistos->FindObject("EtaPhiGammaNoLeading"))
	    ->Fill(gamma.Eta(),gamma.Phi()); 
	}//any = 0
      }// not only charged
    }// is in PHOS
  }//loop: events
  
  fOutputFile->Write() ; 
  fOutputFile->cd();
  this->Write();
}    
//  //______________________________________________________________________________________
//  void AliPHOSGammaJet::MakeJetQuenching(TList &particleList, const Int_t iEvent, TLorentzVector & gamma){

//    TLorentzVector jet1(0,0,0,0) ;
//    TLorentzVector jet2(0,0,0,0) ;

//    AliPHOSGetter * gime = AliPHOSGetter::GetInstance(fInputFileName) ; 
//    //jet1 = GetPythiaJet(fPyth1, gime, iEvent, gamma);
//    cout<<"Jet   1: phi "<<jet1.Phi()<<" theta "<<jet1.Theta()<<" eta "<<jet1.Eta()<<" Pt "<<jet1.Pt()<<endl;
//    cout<<"Gamma 1: Pt "<<gamma.Pt()<<" Phi "<<gamma.Phi()<<" jet - gamma "<<jet1.Phi()-gamma.Phi()<<endl;

//    AliPHOSFastGlobalReconstruction *fastRec = new AliPHOSFastGlobalReconstruction(fJetQuenchingFileName);  
//    AliPHOSGetter * gime2 = AliPHOSGetter::GetInstance(fJetQuenchingFileName) ; 
 
//    Int_t iiEvent = iEvent;
//    //Choose next event. If we are in the same file and in  last event, take the first one
//    if(fJetQuenchingFileName == fInputFileName){
//      iiEvent = iEvent + 1;
//      if(iEvent == fNEvent -1){
//        iiEvent = 0;
//      }
//    }

//    //Set Fast Reconstruction
//     if(fOptFast)
//      fastRec->FastReconstruction(iiEvent, kFALSE,0.,0.,0.);

//     Int_t idg = -1 ;
//     Bool_t isInPHOS = kFALSE;
//     TLorentzVector gamma2 = GetGammaJet(gime2, isInPHOS, idg); 
   
//     //Find second jet
//     jet2 = GetPythiaJet(fPyth2,gime2,iiEvent, gamma2);
//     //   Int_t i = -1;
//  //    while(jet2.Phi()== 0 && jet2.Theta()==0 && jet2.Pt()==0){
//  //      i++;
//  //      jet2 = GetPythiaJet(fJetQuenchingFileName, i, gamma2);
//  //    }
//     cout<<"Jet   2: phi "<<jet2.Phi()<<" theta "<<jet2.Theta()<<" eta "<<jet2.Eta()<<" Pt "<<jet2.Pt()<<endl;
//     cout<<"Gamma 2: Pt "<<gamma2.Pt()<<" Phi "<<gamma2.Phi()<<" jet - gamma "<<jet2.Phi()-gamma2.Phi()<<endl;

//     //Set new prompt gamma (gamma+ gamma2nd event)

//     Double_t pGamma1 = gamma.P();
//     Double_t pGamma2 = jet2.P();
//     Double_t px = gamma.Px()*(pGamma1+pGamma2)/pGamma1;
//     Double_t py = gamma.Py()*(pGamma1+pGamma2)/pGamma1;
//     Double_t pz = gamma.Pz()*(pGamma1+pGamma2)/pGamma1;
//     Double_t e  = gamma.Energy()+jet2.Energy();
//     gamma.SetPxPyPzE(px,py,pz,e);
//     //cout<<"Gamma12: phi "<<gamma.Phi()<<" theta "<<gamma.Theta()<<" eta "<<gamma.Eta()<<" E "<<gamma.Energy()<<endl;

//     //Transform second event and add it to the list
//     TransformAddJetToList(gime2,particleList,jet1,jet2, gamma, idg);

//  }
//______________________________________________________________________________________
void AliPHOSGammaJet::MakeJetQuenching(AliPHOSGetter * gime,
				       TList &particleList, 
				       const Int_t iEvent, 
				       TLorentzVector & gamma, 
				       const Int_t idg){
  

  //Open cuts, now we don't wnat to reject nothing
  Double_t etacut = GetEtaCut();
  Double_t maxrat = GetRatioMaxCut();
  Double_t minrat = GetRatioMinCut();
  Double_t maxphi = GetPhiMaxCut();
  Double_t minphi = GetPhiMinCut();
  SetEtaCut(2.0);	
  SetRatioCutRange(0.,2.);
  SetPhiCutRange(2.5,4.);

  TLorentzVector jet1(0,0,0,0);
  TList newpl;
  CreateParticleList(gime, particleList, idg);
  TLorentzVector leading = GetAnyLeading(particleList,gamma);
  SetJetParticleListInCone(particleList, newpl, jet1,leading,0.7, 0);

  //cout<<"Lead   1: phi "<<leading.Phi()<<" theta "<<leading.Theta()
  //    <<" eta "<<leading.Eta()<<" Pt "<<leading.Pt()<<endl;	
  //cout<<"Jet   1: phi "<<jet1.Phi()<<" theta "<<jet1.Theta()
  //    <<" eta "<<jet1.Eta()<<" Pt "<<jet1.Pt()<<endl;
  //cout<<"Gamma 1: Pt "<<gamma.Pt()<<" Phi "<<gamma.Phi()
  //    <<" Eta "<<gamma.Eta()<<" jet - gamma "<<jet1.Phi()-gamma.Phi()<<endl;

  AliPHOSFastGlobalReconstruction *fastRec = new 
    AliPHOSFastGlobalReconstruction(fJetQuenchingFileName);  
  AliPHOSGetter * gime2 = AliPHOSGetter::GetInstance(fJetQuenchingFileName) ; 
 
  Int_t iiEvent = iEvent;
  //Choose next event. If we are in the same file and in  last event, 
  //take the first one
  if(fJetQuenchingFileName == fInputFileName){
    iiEvent = iEvent + 1;
    if(iEvent == fNEvent -1){
      iiEvent = 0;
    }
  }

  gime2->Event(iiEvent,"P");
  
  //Set Fast Reconstruction
  if(fOptFast)
    fastRec->FastReconstruction(iiEvent, kFALSE,0.,0.,0.);
  
  //Find second jet
  
  TList pl2;
  Int_t idg2 = -1 ;
  Bool_t isInPHOS = kFALSE;
  TLorentzVector gamma2 = GetGammaJet(gime2, isInPHOS, idg2); 
  CreateParticleList(gime2, pl2, idg2);
  TLorentzVector leading2 = GetAnyLeading(pl2,gamma2);
  TLorentzVector jet2(0,0,0,0);
  TList newpl2;
  SetJetParticleListInCone(pl2, newpl2, jet2,leading2,2.0, 0);

  //cout<<"Lead   2: phi "<<leading2.Phi()<<" theta "<<leading2.Theta()
  //    <<" eta "<<leading2.Eta()<<" Pt  "<<leading2.Pt()<<endl;
  //cout<<"Jet   2: phi "<<jet2.Phi()<<" theta "<<jet2.Theta()
  //    <<" eta "<<jet2.Eta()<<" Pt "<<jet2.Pt()<<endl;
  //cout<<"Gamma 2: Pt "<<gamma2.Pt()<<" Phi "<<gamma2.Phi()
  //    <<" Eta "<<gamma2.Eta()
  //    <<" jet - gamma "<<jet2.Phi()-gamma2.Phi()<<endl;

  //Set again the cuts

  SetEtaCut(etacut);
  SetRatioCutRange(minrat,maxrat);
  SetPhiCutRange(minphi,maxphi);

  //Set new prompt gamma (gamma+ gamma2nd event)
  
  Double_t pGamma1 = gamma.P();
  Double_t pGamma2 = gamma2.P();
  Double_t px = gamma.Px()*(pGamma1+pGamma2)/pGamma1;
  Double_t py = gamma.Py()*(pGamma1+pGamma2)/pGamma1;
  Double_t pz = gamma.Pz()*(pGamma1+pGamma2)/pGamma1;
  Double_t e  = gamma.Energy()+gamma2.Energy();
  gamma.SetPxPyPzE(px,py,pz,e);

  //cout<<"Gamma12: phi "<<gamma.Phi()<<" theta "<<gamma.Theta()
  //    <<" eta "<<gamma.Eta()<<" E "<<gamma.Energy()<<endl;
  
  //Transform second event and add it to the list
  TransformAddJetToList(gime2,particleList,jet1,jet2);

}

//_____________________________________________________________________
void AliPHOSGammaJet::EraseParticlesAwayFromJet(TList &particleList, 
						TLorentzVector &jet){
 
  TLorentzVector newjet,lv;
  Int_t iPrimary=-1;
  TVector3 jetvect(0,0,0);
  TVector3 ivect(0,0,0);

  TParticle *particle = new TParticle();
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    particle->Momentum(lv);
    jetvect = jet.Vect();
    ivect = lv.Vect();
    Double_t scalar = jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag());
    if(scalar>fScalarProductCut)
	newjet+=lv;
    else
      particleList.Remove(particle);
  }// while 
  //cout<<"Create  end"<<cone<<" "<<ptcut<<endl;
  jet = newjet;
}


//____________________________________________________________________________
void AliPHOSGammaJet::FillJetParticleHistos(TList &particleList, 
					    TLorentzVector &jet, 
					    const TLorentzVector gamma,
					    const TString cone, 
					    const TString ptcut, 
					    const TString type){

  TParticle *particle = new TParticle();
  Int_t iPrimary=-1;
  TLorentzVector lv;
  TVector3 jetvect(0,0,0);
  TVector3 ivect(0,0,0);

  Double_t phig = gamma.Phi();
  Double_t ptg = gamma.Pt();
  Double_t etag = gamma.Eta();

  Double_t ptjet  = jet.Pt() ;
  

  if(ptjet>0.){
   
    Double_t phijet = jet.Phi() ;
    Double_t etajet = jet.Eta() ;
  
    //cout<<" phi gamma "<<phig<<" phi jet "<<phijet
    //    <<" difference "<<phijet-phig<<endl;
  
    dynamic_cast<TH2F*>(fListHistos->FindObject("Jet"+type+"RatioCone"+cone+"Pt"+ptcut))
      ->Fill(ptg,ptjet/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("Jet"+type+"PhiCone"+cone+"Pt"+ptcut))
      ->Fill(ptg,phijet-phig);
    dynamic_cast<TH2F*>(fListHistos->FindObject("Jet"+type+"EtaCone"+cone+"Pt"+ptcut))
      ->Fill(ptg,etajet-etag);
    //cout<<" pt jet / pt g = "<<ptjet/ptg<<" pt jet = "<<ptjet<<" pt g = "<<ptg<<endl;
    if((ptjet/ptg<fJetRatioMaxCut)&&(ptjet/ptg>fJetRatioMinCut)){
      dynamic_cast<TH1F*>(fListHistos->FindObject("N"+type+"JetCone"+cone+"Pt"+ptcut))
	->Fill(ptg);
      
      TIter next(&particleList);
      while ( (particle = (TParticle*)next()) ) {
	iPrimary++;  
	particle->Momentum(lv);
	
	jetvect = jet.Vect();
	ivect = lv.Vect();

	dynamic_cast<TH2F*>(fListHistos->FindObject("JetFragmentCone"+cone+"Pt"+ptcut))->
	  Fill(ptg,lv.Pt()/ptg);
	if(particle->GetPDG(0)->Charge()!= 0){
	  dynamic_cast<TH2F*>(fListHistos->FindObject("JetFragmentChCone"+cone+"Pt"+ptcut))->
	    Fill(ptg,lv.Pt()/ptg);
	  
	  dynamic_cast<TH1F*>(fListHistos->FindObject("JetScalarChCone"+cone+"Pt"+ptcut))-> 
	    Fill(jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag()));
	}
	else
	  dynamic_cast<TH1F*>(fListHistos->FindObject("JetScalarCone"+cone+"Pt"+ptcut))-> 
	    Fill(jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag()));
	
      }// while 
    }//in jet ratio
  }//ptjet >0.
  //cout<<"end fill"<<endl;
}


//____________________________________________________________________________
TLorentzVector AliPHOSGammaJet::GetAnyLeading(TList &particleList, const TLorentzVector gamma) 
{
  TParticle *particle = new TParticle();
  TLorentzVector any(0,0,0,0);

  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg  = gamma.Pt();
  Double_t phig = gamma.Phi();
  Double_t etag = gamma.Eta();

  Int_t iPrimary=-1, type = -1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    pti = particle->Pt(); 
  
    //cout<<pti<<"  "<<ptmax<<"  "<<iPrimary<<endl;   
    //cout<<phi<<"  "<<phig<<endl;
    
//     if(particle->GetStatusCode()==1){
    //cout<<" name "<<particle->GetName()<<"  "<<particle->Pt()
    //   <<"  "<<ptmax<<endl;
       //cout<<" phi gamma "<<phig<<" phi any "<<particle->Phi()
       //   <<" difference "<<phig-particle->Phi()<<endl;
//     }
    if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
      phi = particle->Phi();
      if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut))
	{
	  ptmax = pti;
	  type = particle->GetPdgCode();
	  particle->Momentum(any) ;
	  //cout<<any<<endl;
	}//phi cut  
    }//ptmax 
  }// while
  if(any.Pt() > 0.){
    dynamic_cast<TH2F*>(fListHistos->FindObject("AnyRatio"))->Fill(ptg,any.Pt()/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiAny"))->Fill(ptg,any.Phi()-phig);
    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaEtaAny"))->Fill(ptg,any.Eta()-etag);
    //Info("GetAnyLeading","type %d, E  %f", type,neutral->Pt());
  }
  //Info("GetAnyLeading","N %d",iPrimary);
  return any;
}


//____________________________________________________________________________
void AliPHOSGammaJet::MakeJet(TList &particleList, const TLorentzVector gamma, 
			      const TLorentzVector leading, 
			      TString & type) {

  TList pl;
  TLorentzVector jet0(0,0,0,0);  
  //Info("MakeJet","Before 1st SetJetParticlesListInCone");
  SetJetParticleListInCone(particleList, pl, jet0,leading ,0.6, 0.);
  //Info("MakeJet","After 1st SetJetParticlesListInCone");
  Float_t cone[]  = {0.3,0.4,0.5,0.6};
  Float_t ptcut[] = {0.0,0.5,1.0};
  TString scone[]  = {"03","04","05","06"};
  TString sptcut[] = {"0","05","1"};

  for(Int_t icone = 0; icone<4; icone++){
    for(Int_t ipt = 0; ipt<3;ipt++){
      TList newpl;
      TLorentzVector jet(0,0,0,0);
      //  Info("MakeJet","Before 2nd SetJetParticlesListInCone");
      SetJetParticleListInCone(pl, newpl, jet,leading,cone[icone], ptcut[ipt]);
      // Info("MakeJet","After 2nd SetJetParticlesListInCone");
      //Info("MakeJet","Before Erase");
      for(Int_t i = 0; i<fNIter;i++)
	EraseParticlesAwayFromJet(newpl,jet);
      //Info("MakeJet","After Erase");
      //Info("MakeJet","Before Fill");
      FillJetParticleHistos(newpl, jet, gamma,scone[icone],sptcut[ipt], type);
      //Info("MakeJet","After Fill");
      //newpl.Delete();
    }
  }
  //pl.Delete();
}

//____________________________________________________________________________
TLorentzVector AliPHOSGammaJet::GetGammaJet(AliPHOSGetter *gime, Bool_t & is, Int_t & idg) 
{
  TParticle *particle = new TParticle();
  TLorentzVector gamma(0,0,0,0);
  Int_t module = -1;
  Double_t x = 0., z = 0.;
  Int_t idmotherg = -1;
  idg = -1;
  is = kFALSE;
  TClonesArray * pr = gime->Primaries() ;
  
  if(!fJetJet){   
    for(Int_t ipr = 0;ipr < pr->GetEntries() ; ipr ++ ){
      particle = (TParticle *) pr->At(ipr) ;
      Int_t ksCode = particle->GetStatusCode();
      Int_t iMother= particle->GetMother(0);
      
      if (ksCode == 21 && iMother == -1){
	if(particle->GetPdgCode()==22){
	  idmotherg = ipr;
	  //cout<<"idmother "<<idmotherg<<endl;
// 	  cout<<"phi "<<particle->Phi()<<" theta "<<particle->Theta()<<" eta "<<particle->Eta()
// 	      <<" E "<<particle->Energy()<<endl;
	}
      }
      if(ksCode == 1){
	if(idmotherg == particle->GetMother(0)){
	  
	  idg = ipr;
	  if(fOptFast)
	    MakePhoton(particle);
	  particle->Momentum(gamma) ;
	  //cout<<"particle "<<particle->Phi()<<" lv "<<gamma.Phi()<<endl;
	  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
	  geom->ImpactOnEmc(static_cast<double>(particle->Theta()),static_cast<double>(particle->Phi()), 
			    module,z,x);
	  if(module != 0)
	    is = kTRUE;
	  
	  break;
	}
      }// kscode == 1
    }// for
  }
  else{

    TLorentzVector pPi0, pGamma1, pGamma2 ;
    Double_t angle = 0, cellDistance = 0.;

    for(Int_t ipr = 0;ipr < pr->GetEntries() ; ipr ++ ){
      particle = (TParticle *) pr->At(ipr) ;
      Int_t ksCode = particle->GetStatusCode();
      Int_t ksPdg  = particle->GetPdgCode();
      Double_t ksPt = particle->Pt();
      Double_t ptmax = 0;
      if((ksPt > 0.)&&(ksCode == 1)&&(ksPdg==111)){
	if(TMath::Abs(particle->Eta())<fEtaCut+1.){
	
	  particle->Momentum(pPi0);
	  //cout<<"Pi0 decay 1"<<endl;
	  Pi0Decay(particle->GetMass(),pPi0,pGamma1,pGamma2,angle);
	  //cout<<"Pi0 decay 2"<<endl;
	  cellDistance = angle*460; //cm
	  //cout<<"angle "<<angle<<" i "<<ipr<<endl;
	  if (cellDistance < fMinDistance) {
	    if(ksPt > ptmax){
	      //cout<< "Pi0 "<<ipr<<endl;
	      
	      if(fOptFast){
		//cout<<"Pi0 1 "<<endl;
		MakePhoton(particle);
		//cout<<"Pi0 2 "<<endl;
	      }
	      const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
	      geom->ImpactOnEmc(static_cast<double>(particle->Theta()),
				static_cast<double>(particle->Phi()), 
				module,z,x);
	      if(module != 0) {
		idg = ipr;
		ptmax = ksPt;   
		is = kTRUE;
		particle->Momentum(gamma) ;
	      }
	    }//ksPt > ptmax
	  }// if cell<distance
	  else{
	    //cout<< "Gamma "<<ipr<<endl;
	    TParticle * photon1 = new TParticle(22,1,0,0,0,0,pGamma1.Px(),pGamma1.Py(),
						pGamma1.Pz(),pGamma1.E(),0,0,0,0);
	    TParticle * photon2 = new TParticle(22,1,0,0,0,0,pGamma2.Px(),pGamma2.Py(),
						pGamma2.Pz(),pGamma2.E(),0,0,0,0);  
	    if(fOptFast){
	      //cout<<"Gamma 1 "<<endl;
	      MakePhoton(photon1);
	      //cout<<"Gamma 2 "<<endl;
	      MakePhoton(photon2);
	      //cout<<"Gamma 12 "<<endl;
	    }
	    //Photon1
	    if(photon1->Pt() > ptmax){
	      const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
	      geom->ImpactOnEmc(static_cast<double>(photon1->Theta()),
				static_cast<double>(photon1->Phi()), 
				module,z,x);
	      if(module != 0) {
		idg = ipr;
		ptmax = photon1->Pt();   
		is = kTRUE;
		photon1->Momentum(gamma) ;
	      }
	    }//phton1 > ptmax
	    
	    //Photon2
	    if(photon2->Pt() > ptmax){
	      const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
	      geom->ImpactOnEmc(static_cast<double>(photon2->Theta()),
				static_cast<double>(photon2->Phi()), 
				module,z,x);
	      if(module != 0) {
		idg = ipr;
		ptmax = photon2->Pt();   
		is = kTRUE;
		photon2->Momentum(gamma) ;
	      }
	    }//phton2 > ptmax
	  }//cell<distance	
	}//eta
      }// kscode == 1
    }// while
  }
  return gamma;
}

//  //____________________________________________________________________________
//  TLorentzVector AliPHOSGammaJet::GetPythiaJet(AliGenPythia * pyth, AliPHOSGetter * gime, 
//  					     const Int_t iEvent, const TLorentzVector gamma){

//  //   TFile *f=new TFile(filename,"read");
//  //   AliRun* gAlice = (AliRun*)(f->Get("gAlice"));
//  //   AliGenPythia* pyth = (AliGenPythia*) gAlice->Generator();
//  //   pyth->Init();


//    cout<<"Get Event"<<endl;
//    gime->Event(iEvent, "P");
//    Int_t nJ = -1, nJT = -1;
//    Float_t jets[4][10];
//    cout<<"Rec Jet"<<endl;
//    pyth->SetJetReconstructionMode(1);
//    //cout<<"recons mode" <<endl; 
//    cout<<"Load Jet"<<endl;
//    pyth->LoadEvent();
//    //cout<<"load event" <<endl;
//    cout<<"Get Jet"<<endl;
//    pyth->GetJets(nJ, nJT, jets);
//    cout<<"NJets "<<nJT<<" nj "<<nJ<<endl;
//    Int_t index = -1 ;
 
//    for (Int_t i = 0; i<=nJT;i++){
//      TLorentzVector jet(0,0,0,0);
//      jet.SetPxPyPzE(jets[0][i],jets[1][i],jets[2][i],jets[3][i]);

//      Double_t diff = jet.Phi()-gamma.Phi() ;
//      Double_t rat  = jet.Pt()/gamma.Pt() ;
//      //cout<<"Jet  pyth"<<i<< ": phi "<<jet.Phi()<<" theta "<<jet.Theta()<<" eta "<<jet.Eta()<<" Pt "<<jet.Pt()<<endl;
//      //cout<<"Gamma: Pt "<<gamma.Pt()<<" Phi "<<gamma.Phi()<<" jet - gamma "<<diff<<" jet pt/ gamma pt "<<rat<<endl;
    
//      if((diff>fPhiMinCut) && (diff<fPhiMaxCut)){
//        //cout<<"pasa phi "<<i<<"diff  "<<diff<<" > "<<fPhiMinCut<<" < "<<fPhiMaxCut<<endl;
//        if((rat>fJetRatioMinCut) && (rat<fJetRatioMaxCut)){
//  	//cout<<"pasa pt "<<i<<"rat  "<<rat<<" > "<<fJetRatioMinCut<<" < "<<fJetRatioMaxCut<<endl;
//  	index = i;
//        }
//      }
//    }
//    //cout<<"INDEX "<<index<<endl;
//    TLorentzVector jet(0,0,0,0);
//    if(index != -1)
//      jet.SetPxPyPzE(jets[0][index],jets[1][index],jets[2][index],jets[3][index]);
    
//    return jet;

//  }
//____________________________________________________________________________
TLorentzVector AliPHOSGammaJet::GetLeadingCharge(TList &particleList, 
						 const TLorentzVector gamma) 
{
  TParticle * particle = new TParticle();
  TLorentzVector charge(0,0,0,0) ;
 
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    
    if(((ksCode == 1)||(ksCode == 0))&&(particle->GetPDG(0)->Charge()!=0)){
      pti = particle->Pt();
//        cout<<"pti "<<pti<<" ptg "<<ptg<<" pti/ptg "<<pti/ptg<<endl;
//        cout<<"fRatioMinCut "<<fRatioMinCut
//  	  <<" fRatioMaxCut "<<fRatioMaxCut<<endl;
//        cout<<"fPhiMinCut "<<fPhiMinCut
//  	  <<" fPhiMaxCut "<<fPhiMaxCut<<endl;
      if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	phi = particle->Phi();
	//  cout<<"charge : phi "<<particle->Phi()<<" theta "<<particle->Theta()
//  	    <<" eta "<<particle->Eta()<<" Pt "<<particle->Pt()<<endl;
	if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	  ptmax = pti;
	  particle -> Momentum(charge);
	}//phi cut  
      }//ptmax 
    }// kscode == 1
  }// while
  if((charge.Pt() > 0.0)){
    dynamic_cast<TH2F*>(fListHistos->FindObject("ChargeRatioAll"))->Fill(ptg,charge.Pt()/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiChargeAll"))->Fill(ptg,charge.Phi()-phig);
  }
  //Info("GetLeadingCharge","N %d",iPrimary);
  return charge ;

}


//____________________________________________________________________________
TLorentzVector AliPHOSGammaJet::GetLeadingNeutral(TList &particleList,const TLorentzVector gamma) 
{
  TParticle *particle = new TParticle();
  TLorentzVector neutral(0,0,0,0);
 
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1, type = -1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    pti = particle->Pt(); 
    if(((ksCode == 1)||(ksCode == 0))&&(particle->GetPDG(0)->Charge()==0)
       &&(particle->GetPdgCode()!=111)&&(pti>0.)){
      pti = particle->Pt(); 
      if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	phi = particle->Phi();
	if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	  ptmax = pti;
	  type = particle->GetPdgCode();
	  particle->Momentum(neutral);
	}//phi cut  
      }//ptmax 
    }// kscode == 1
  }// while
  if(neutral.Pt() > 0.){
    //dynamic_cast<TH2F*>(fListHistos->FindObject("NeutralRatioAll"))->Fill(ptg,neutral.Pt()/ptg);
    //dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiNeutral"))->Fill(ptg,neutral.Phi()-phig);
    //Info("GetLeadingNeutral","type %d, E  %f", type,neutral.Pt());
  }

  return neutral;

}

//____________________________________________________________________________
TLorentzVector AliPHOSGammaJet::GetLeadingPi0(TList &particleList, const TLorentzVector gamma) 
{
  const Float_t mpi0 = 0.1349766;
  TParticle * particle = new TParticle();
  TLorentzVector pi0(0,0,0,0) ;
  
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1;
  TIter next(&particleList);

  if(!fOptFast){
    while ( (particle = (TParticle*)next()) ) {
      iPrimary++;  
      Int_t ksCode = particle->GetStatusCode();
      
      if(((ksCode == 1)||(ksCode == 0))&&(particle->GetPdgCode()==111)){
	pti = particle->Pt(); 
	if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	  phi = particle->Phi();
	  if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	    ptmax = pti;
	    particle -> Momentum(pi0);
	  }//phi cut  
	}//ptmax 
      }// kscode == 1
    }// while
  }//no opt fast

  else{

    TLorentzVector gammai,gammaj;
    Double_t arg = 0., angle = 0., e = 0., invmass = 0., anglecutmax = 0.0, anglecutmin = 0.0;
    TIter next(&particleList);
    while ( (particle = (TParticle*)next()) ) {
      iPrimary++;	  
      Int_t ksCode = particle->GetStatusCode();
      Int_t ksPdg = particle->GetPdgCode();
      if(((ksCode == 1)||(ksCode == 0))) { 
	if(ksPdg == 111){
	  pti = particle->Pt();
	  if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	    phi = particle->Phi();
	    if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	      ptmax = pti;
	      particle -> Momentum(pi0);
	    }//phi cut
	  }//ratio cut
	}// pdg = 111
	if(ksPdg == 22){
	  particle->Momentum(gammai);
	  
	  Int_t jPrimary=-1;
	  TIter next2(&particleList);
	  while ( (particle = (TParticle*)next2()) ) {
	    jPrimary++;
	    if(jPrimary>iPrimary){
	      ksCode = particle->GetStatusCode();
	      ksPdg = particle->GetPdgCode();
	      if(((ksCode == 1)||(ksCode == 0))){
		if(ksPdg == 22){
		  particle->Momentum(gammaj);
		  //Info("GetLeadingPi0","Egammai %f, Egammaj %f", gammai.Pt(),gammaj.Pt());
		  angle = gammaj.Angle(gammai.Vect());
		  //Info("GetLeadingPi0","Angle %f", angle);
		  e = (gammai+gammaj).E();
		  anglecutmax = fAngleMaxParam.At(0)*TMath::Exp(fAngleMaxParam.At(1)*e)
		    +fAngleMaxParam.At(2)+fAngleMaxParam.At(3)*e;
		  //Info("GetLeadingPi0","Angle %f, max %f", angle,anglecutmax);
		  arg = (e*e-2*mpi0*mpi0)/(e*e);
		  //Info("GetLeadingPi0","Angle %f, cos min %f", angle,arg);
		  if(arg>0.)
		    anglecutmin = TMath::ACos(arg);
		  else
		    anglecutmin= 100.;
		  //Info("GetLeadingPi0","Angle %f, min %f", angle,anglecutmin);
		  dynamic_cast<TH2F*>(fListHistos->FindObject("AngleAllPair")) ->Fill(e,angle);
		  if((angle<anglecutmax)&&(angle>=anglecutmin)){
		    dynamic_cast<TH2F*>(fListHistos->FindObject("AngleAllPairCut")) ->Fill(e,angle);
		    invmass = (gammai+gammaj).M();
		    //Info("GetLeadingPi0","InvMass %f", invmass);
		    if((invmass>fInvMassMinCut) && (invmass<fInvMassMaxCut)){ 
		      pti = (gammai+gammaj).Pt();
		      phi = (gammai+gammaj).Phi();
		      if(pti>ptmax && (pti/ptg>fRatioMinCut) && (pti/ptg<fRatioMaxCut)){
			if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
			  //Info("GetLeadingPi0","Delta Phi %f", phi-phig);
			  ptmax = pti;
			  pi0 = gammai+gammaj;
			}//phi cut
		      }//e12>el && (e12/eg>0.2 && e12/eg<1.)
		    }//(invmass>0.125) && (invmass<0.145)
		  }//gammaj.Angle(gammai.Vect())<0.04
		}//if pdg = 22
	      }//(ksCode == 1)
	    }
	  }//while
	  //	    cout<<"jPrimary "<<jPrimary<<endl;
	}// if pdg = 22
      }//if kscode 1
    }//while		
  }//foptfast
  
  if((pi0.Pt() > 0.0)){
    dynamic_cast<TH2F*>(fListHistos->FindObject("InvMassPair")) ->Fill(ptg,pi0.M());
    dynamic_cast<TH2F*>(fListHistos->FindObject("Pi0RatioAll"))->Fill(ptg,pi0.Pt()/ptg);
	dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiPi0All"))->Fill(ptg,pi0.Phi()-phig);
  }
      
  return pi0 ;
  
}

//____________________________________________________________________________
void AliPHOSGammaJet::InitParameters()
{
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.AddAt(0.4,0);//={0.4,-0.25,0.025,-2e-4};
  fAngleMaxParam.AddAt(-0.25,1) ;
  fAngleMaxParam.AddAt(0.025,2) ;
  fAngleMaxParam.AddAt(-2e-4,3) ;
  fOutputFileName = "GammaJet.root" ;
  fHIJINGFileName = "galice.root" ;
  fHIJING         = kFALSE ;
  fNIter          = 1 ;
  fMinDistance    = 3.6 ;
  fScalarProductCut=0.95 ;
  fEtaCut         = 0.7 ;
  fInvMassMaxCut  = 0.14 ;
  fInvMassMinCut  = 0.13 ;
  fOnlyCharged      = kFALSE ;
  fOptFast        = kFALSE ;
  fPhiMaxCut      = 3.4 ;
  fPhiMinCut      = 2.9 ;
  fRatioMaxCut    = 1.0 ;
  fRatioMinCut    = 0.05 ; // Optimum cut on pp is 0.13. In case of quenching
                         // it will diminish to 0.5
  fJetRatioMaxCut = 1.2 ;// To be changed with hijing
  fJetRatioMinCut = 0.8 ; 
  fJetJet         = kFALSE ;
  fJetQuenching   = kFALSE ;
  fResPara1       = 0.0255 ;    // GeV
  fResPara2       = 0.0272 ; 
  fResPara3       = 0.0129 ; 
  
  fPosParaA      = 0.096 ;    // cm
  fPosParaB      = 0.229 ;  
  
 
}

//____________________________________________________________________________
void AliPHOSGammaJet::List() const
{
  // List the histos

  Info("List", "%d histograms found", fListHistos->GetEntries() ) ; 
  TIter next(fListHistos) ; 
  TH2F * h = 0 ; 
  while ( (h = dynamic_cast<TH2F*>(next())) )
    Info("List", "%s", h->GetName()) ; 
}

//____________________________________________________________________________
Double_t AliPHOSGammaJet::MakeEnergy(const Double_t energy)
{  
  // Smears the energy according to the energy dependent energy resolution.
  // A gaussian distribution is assumed
  
  Double_t sigma  = SigmaE(energy) ; 
  return  fRan.Gaus(energy, sigma) ;   


}
//____________________________________________________________________________
void AliPHOSGammaJet::MakeHistos()
{
  // Create histograms to be saved in output file ans sores them in an TObjectArray

  fOutputFile = new TFile(fOutputFileName, "recreate") ;

  fListHistos = new TObjArray(100) ;
 
  // Histos gamma pt vs leading pt

  TNtuple *nt = new TNtuple("PtLeadingNT", "Pt", "ptg:ptc:ptp:pta") ; 
  fListHistos->Add(nt) ;

  TH2F * hPtChargePi0Leading  = new TH2F
    ("PtChargePi0Leading","Pt_{#leading charge} vs Pt_{pi0}",120,0,120,120,0,120); 
  hPtChargePi0Leading->SetYTitle("Pt_{charge}");
  hPtChargePi0Leading->SetXTitle("Pt_{pi0} (GeV/c)");
  fListHistos->Add(hPtChargePi0Leading) ; 
  
  TH2F * hPtChargePi0LeadingBefore  = new TH2F
    ("PtChargePi0LeadingBefore","Pt_{#leading charge} vs Pt_{pi0}",120,0,120,120,0,120); 
  hPtChargePi0LeadingBefore->SetYTitle("Pt_{charge}");
  hPtChargePi0LeadingBefore->SetXTitle("Pt_{pi0} (GeV/c)");
  fListHistos->Add(hPtChargePi0LeadingBefore) ;

  TH2F * hPtGammaLeading  = new TH2F
    ("PtGammaLeading","Pt_{#leading} vs Pt_{#gamma}",120,0,120,120,0,120); 
  hPtGammaLeading->SetYTitle("Pt_{lead}");
  hPtGammaLeading->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeading) ; 
  
  TH2F * hEtaPhiGammaNoLeading  = new TH2F
    ("EtaPhiGammaNoLeading","#eta_{#gamma} vs #phi_{#gamma}",100,-1.3,1.3,100,-4,0); 
  hEtaPhiGammaNoLeading->SetXTitle("#eta");
  hEtaPhiGammaNoLeading->SetYTitle("#phi");
  fListHistos->Add(hEtaPhiGammaNoLeading) ; 

  TH2F * hPtGammaLeadingCharge  = new TH2F
    ("PtGammaLeadingCharge","Pt_{#lead charge} vs Pt_{#gamma}",120,0,120,120,0,120); 
  hPtGammaLeadingCharge->SetYTitle("Pt_{lead charge}");
  hPtGammaLeadingCharge->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingCharge) ; 

  TH2F * hPtGammaLeadingPi0  = new TH2F
    ("PtGammaLeadingPi0","Pt_{#lead pi0} vs Pt_{#gamma}",120,0,120,120,0,120); 
  hPtGammaLeadingPi0->SetYTitle("Pt_{lead pi0}");
  hPtGammaLeadingPi0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingPi0) ;
 
 TH2F * hPtGammaLeadingNeutral  = new TH2F
    ("PtGammaLeadingNeutral","Pt_{#lead neutral} vs Pt_{#gamma}",120,0,120,120,0,120); 
  hPtGammaLeadingNeutral->SetYTitle("Pt_{lead neutral}");
  hPtGammaLeadingNeutral->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingNeutral) ;

  //Histos ratio charged leading pt / gamma pt vs pt 
 
  TH2F * hChargeRatio  = new TH2F
    ("ChargeRatio","Pt_{leading charge} /Pt_{#gamma} vs Pt_{#gamma}",120,0,120,120,0,1); 
  hChargeRatio->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hChargeRatio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hChargeRatio) ; 

  TH2F * hAnyRatio  = new TH2F
    ("AnyRatio","Pt_{leading} /Pt_{#gamma} vs Pt_{#gamma}",120,0,120,120,0,1); 
  hAnyRatio->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hAnyRatio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hAnyRatio) ; 

  TH2F * hPi0Ratio  = new TH2F
    ("Pi0Ratio","Pt_{leading  #pi^{0}} /Pt_{#gamma} vs Pt_{#gamma}",120,0,120,120,0,1); 
  hPi0Ratio->SetYTitle("Pt_{lead  #pi^{0}} /Pt_{#gamma}");
  hPi0Ratio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPi0Ratio) ;
 
  //Before chosing the leading 

  TH2F * hChargeRatioAll  = new TH2F
    ("ChargeRatioAll","Pt_{leading charge} /Pt_{#gamma} vs Pt_{#gamma} No Cut",120,0,120,120,0,1); 
  hChargeRatioAll->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hChargeRatioAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hChargeRatioAll) ;

  TH2F * hPi0RatioAll  = new TH2F
    ("Pi0RatioAll","Pt_{leading  #pi^{0}} /Pt_{#gamma} vs Pt_{#gamma} No Cut",120,0,120,120,0,1); 
  hPi0RatioAll->SetYTitle("Pt_{lead  #pi^{0}} /Pt_{#gamma}");
  hPi0RatioAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPi0RatioAll) ; 


  TH2F * hDeltaPhiChargeAll  = new TH2F
    ("DeltaPhiChargeAll","#phi_{#gamma} - #phi_{charge} vs Pt_{#gamma}",200,0,120,200,0,6.4); 
  hDeltaPhiChargeAll->SetYTitle("#Delta #phi");
  hDeltaPhiChargeAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiChargeAll) ; 
		 
  TH2F * hDeltaPhiPi0All  = new TH2F
    ("DeltaPhiPi0All","#phi_{#gamma} - #phi_{ #pi^{0}} vs Pt_{#gamma}",200,0,120,200,0,6.4); 
  hDeltaPhiPi0All->SetYTitle("#Delta #phi");
  hDeltaPhiPi0All->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiPi0All) ; 

  TH2F * hDeltaPhiAny  = new TH2F
    ("DeltaPhiAny","#phi_{#gamma} - #phi_{any} vs Pt_{#gamma}",200,0,120,200,0,6.4); 
  hDeltaPhiAny->SetYTitle("#Delta #phi");
  hDeltaPhiAny->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiAny) ; 

  TH2F * hDeltaPhiCharge  = new TH2F
    ("DeltaPhiCharge","#phi_{#gamma} - #phi_{charge} vs Pt_{#gamma}",200,0,120,200,0,6.4); 
  hDeltaPhiCharge->SetYTitle("#Delta #phi");
  hDeltaPhiCharge->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiCharge) ; 
		 
  TH2F * hDeltaPhiPi0  = new TH2F
    ("DeltaPhiPi0","#phi_{#gamma} - #phi_{ #pi^{0}} vs Pt_{#gamma}",200,0,120,200,0,6.4); 
  hDeltaPhiPi0->SetYTitle("#Delta #phi");
  hDeltaPhiPi0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiPi0) ; 

  TH2F * hDeltaEtaAny  = new TH2F
    ("DeltaEtaAny","#eta_{#gamma} - #eta_{any} vs Pt_{#gamma}",200,0,120,200,-2,2); 
  hDeltaEtaAny->SetYTitle("#Delta #eta");
  hDeltaEtaAny->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaEtaAny) ; 

  TH2F * hDeltaEtaCharge  = new TH2F
    ("DeltaEtaCharge","#eta_{#gamma} - #eta_{charge} vs Pt_{#gamma}",200,0,120,200,-2,2); 
  hDeltaEtaCharge->SetYTitle("#Delta #eta");
  hDeltaEtaCharge->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaEtaCharge) ; 
		 
  TH2F * hDeltaEtaPi0  = new TH2F
    ("DeltaEtaPi0","#eta_{#gamma} - #eta_{ #pi^{0}} vs Pt_{#gamma}",200,0,120,200,-2,2); 
  hDeltaEtaPi0->SetYTitle("#Delta #eta");
  hDeltaEtaPi0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaEtaPi0) ; 
  if(fOptFast){
    TH2F * hPtmodPtnomod  = new TH2F
      ("PtmodPtnomod","Ptmod(Ptnomod)",200,0,120,200,0,120); 
    hPtmodPtnomod ->SetYTitle("Pt mod (GeV/c)");
    hPtmodPtnomod ->SetXTitle("Pt no mod (GeV/c)");
    fListHistos->Add(hPtmodPtnomod ) ;
    
    TH2F * hPhimodPhinomod  = new TH2F
      ("PhimodPhinomod","Phimod(Phinomod)",200,0,6,200,0,6); 
    hPhimodPhinomod ->SetYTitle("Phi mod");
    hPhimodPhinomod ->SetXTitle("Phi no mod");
    fListHistos->Add(hPhimodPhinomod ) ;

    TH2F * hThmodThnomod  = new TH2F
      ("ThmodThnomod","Thmod(Thnomod)",200,0,3,200,0,3); 
    hThmodThnomod ->SetYTitle("Th mod ");
    hThmodThnomod ->SetXTitle("Th no mod ");
    fListHistos->Add(hThmodThnomod ) ;
    
    TH2F * hAnglePair  = new TH2F
      ("AnglePair","Angle between #pi^{0} #gamma pair vs Pt_{ #pi^{0}}",300,0,50,300,0,0.2); 
    hAnglePair->SetYTitle("Angle (rad)");
    hAnglePair->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    fListHistos->Add(hAnglePair) ; 

    TH2F * hAngleAllPair  = new TH2F
      ("AngleAllPair","Angle between all #gamma pair vs Pt_{ #pi^{0}}",200,0,50,200,0,0.2); 
    hAngleAllPair->SetYTitle("Angle (rad)");
    hAngleAllPair->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    fListHistos->Add(hAngleAllPair) ; 

    TH2F * hAngleAllPairCut  = new TH2F
      ("AngleAllPairCut","Angle between all #gamma pair cut vs Pt_{ #pi^{0}}",200,0,50,200,0,0.2); 
    hAngleAllPairCut->SetYTitle("Angle (rad)");
    hAngleAllPairCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    fListHistos->Add(hAngleAllPairCut) ; 

  }

  TH2F * hInvMassPair  = new TH2F
    ("InvMassPair","Invariant Mass of #gamma pair vs Pt_{#gamma}",120,0,120,120,0.1,0.16); 
  hInvMassPair->SetYTitle("Invariant Mass (GeV/c^{2})");
  hInvMassPair->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hInvMassPair) ; 


 //Count

  TH1F * hNGamma  = new TH1F
    ("NGamma","Number of #gamma over PHOS",120,0,120); 
  hNGamma->SetYTitle("N");
  hNGamma->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNGamma->Sumw2();
  fListHistos->Add(hNGamma) ; 

  TH2F * hJetAnyRatio[4][3];
  TH2F * hJetPi0Ratio[4][3];
  TH2F * hJetChargeRatio[4][3];
  TH2F * hJetAnyPhi[4][3];
  TH2F * hJetPi0Phi[4][3];
  TH2F * hJetChargePhi[4][3];
  TH2F * hJetAnyEta[4][3];
  TH2F * hJetPi0Eta[4][3];
  TH2F * hJetChargeEta[4][3];

  TH1F * hNCharge[4][3];
  TH1F * hNAny[4][3];
  TH1F * hNPi0[4][3];
  TH2F * hJetFragment[4][3];
  TH2F * hJetFragmentCh[4][3];
  TH1F * hJetScalar[4][3];
  TH1F * hJetScalarCh[4][3];



  TString cone[]  = {"0.3","0.4","0.5","0.6"};
  TString ptcut[] = {"0.0","0.5","1.0"};
  TString scone[]  = {"03","04","05","06"};
  TString sptcut[] = {"0","05","1"};

  for(Int_t icone = 0; icone<4; icone++){
    for(Int_t ipt = 0; ipt<3;ipt++){
      
      //Jet Pt / Gamma Pt 
      
      
      hJetPi0Ratio[icone][ipt]  = new TH2F
	("JetPi0RatioCone"+scone[icone]+"Pt"+sptcut[ipt], 
	 "Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,0,2);
      hJetPi0Ratio[icone][ipt]->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
      hJetPi0Ratio[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetPi0Ratio[icone][ipt]) ; 
      
      hJetChargeRatio[icone][ipt]  = new TH2F
	("JetChargeRatioCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,0,2);
      hJetChargeRatio[icone][ipt]->SetYTitle("Pt_{jet lead charge}/Pt_{#gamma}");
      hJetChargeRatio[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetChargeRatio[icone][ipt]) ; 
      
      hJetAnyRatio[icone][ipt]  = new TH2F
      ("JetAnyRatioCone"+scone[icone]+"Pt"+sptcut[ipt],
       "Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,0,2);
      hJetAnyRatio[icone][ipt]->SetYTitle("Pt_{jet lead any}/Pt_{#gamma}");
      hJetAnyRatio[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetAnyRatio[icone][ipt]) ; 
      
      //Phi gamma - Phi jet
      
      
      hJetPi0Phi[icone][ipt]  = new TH2F
	("JetPi0PhiCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#phi_{jet lead pi0}/#phi_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,1,4);
      hJetPi0Phi[icone][ipt]->SetYTitle("#Delta #phi");
      hJetPi0Phi[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetPi0Phi[icone][ipt]) ; 
      
      hJetChargePhi[icone][ipt]  = new TH2F
	("JetChargePhiCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#phi_{jet lead pi0}-#phi_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,1,4);
      hJetChargePhi[icone][ipt]->SetYTitle("#Delta #phi");
      hJetChargePhi[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetChargePhi[icone][ipt]) ; 
      
      hJetAnyPhi[icone][ipt]  = new TH2F
	("JetAnyPhiCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#phi_{jet lead any}-#phi_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120,120,1,4);
      hJetAnyPhi[icone][ipt]->SetYTitle("Pt_{jet lead any}/Pt_{#gamma}");
      hJetAnyPhi[icone][ipt]->SetXTitle("#Delta #phi");
      fListHistos->Add(hJetAnyPhi[icone][ipt]) ;
      
      //Eta jet
      
      hJetPi0Eta[icone][ipt]  = new TH2F
	("JetPi0EtaCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#eta_{jet lead pi0}/#eta_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",200,0,120,200,-2,2);
      hJetPi0Eta[icone][ipt]->SetYTitle("#Delta #eta");
      hJetPi0Eta[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetPi0Eta[icone][ipt]) ; 
      
      hJetChargeEta[icone][ipt]  = new TH2F
	("JetChargeEtaCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#eta_{jet lead pi0}-#eta_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",200,0,120,200,-2,2);
      hJetChargeEta[icone][ipt]->SetYTitle("#Delta #eta");
      hJetChargeEta[icone][ipt]->SetXTitle("Pt_{#gamma} (GeV/c)");
      fListHistos->Add(hJetChargeEta[icone][ipt]) ; 
      
      hJetAnyEta[icone][ipt]  = new TH2F
	("JetAnyEtaCone"+scone[icone]+"Pt"+sptcut[ipt],
	 "#eta_{jet lead any}-#eta_{#gamma} vs Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",200,0,100,200,-2,2);
      hJetAnyEta[icone][ipt]->SetYTitle("Pt_{jet lead any}/Pt_{#gamma}");
      hJetAnyEta[icone][ipt]->SetXTitle("#Delta #eta");
      fListHistos->Add(hJetAnyEta[icone][ipt]) ;
      
      //Counts
      
      hNCharge[icone][ipt]  = new TH1F
	("NChargeJetCone"+scone[icone]+"Pt"+sptcut[ipt],"Number of charged jets, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120); 
      hNCharge[icone][ipt]->SetYTitle("N");
      hNCharge[icone][ipt]->SetXTitle("Pt_{#gamma}(GeV/c)");
      hNCharge[icone][ipt]->Sumw2();
      fListHistos->Add(hNCharge[icone][ipt]) ; 
      
      hNPi0[icone][ipt]  = new TH1F
	("NPi0JetCone"+scone[icone]+"Pt"+sptcut[ipt],"Number of neutral jets, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120); 
      hNPi0[icone][ipt]->SetYTitle("N");
      hNPi0[icone][ipt]->SetXTitle("Pt_{#gamma}(GeV/c)");
      hNPi0[icone][ipt]->Sumw2();
      fListHistos->Add(hNPi0[icone][ipt]) ; 
      
      hNAny[icone][ipt]  = new TH1F
	("NAnyJetCone"+scone[icone]+"Pt"+sptcut[ipt],"Number of jets, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",120,0,120); 
      hNAny[icone][ipt]->SetYTitle("N");
      hNAny[icone][ipt]->SetXTitle("Pt_{#gamma}(GeV/c)");
      hNAny[icone][ipt]->Sumw2();
      fListHistos->Add(hNAny[icone][ipt]) ; 
      
      hJetFragment[icone][ipt]  = new TH2F
	("JetFragmentCone"+scone[icone]+"Pt"+sptcut[ipt],"x = Pt_{i}/Pt_{#gamma}, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",
	 120,0.,120.,120,0.,1.); 
      hJetFragment[icone][ipt]->SetYTitle("dN / dx");
      hJetFragment[icone][ipt]->SetXTitle("x");
      hJetFragment[icone][ipt]->Sumw2();
      fListHistos->Add(hJetFragment[icone][ipt]) ; 
      
      hJetFragmentCh[icone][ipt]  = new TH2F
	("JetFragmentChCone"+scone[icone]+"Pt"+sptcut[ipt],"x = Pt_{i charged}/Pt_{#gamma}, cone ="+cone[icone]+", pt>" +
	 ptcut[ipt]+" GeV/c",120,0.,120.,120,0.,1.); 
      hJetFragmentCh[icone][ipt]->SetYTitle("dN / dx");
      hJetFragmentCh[icone][ipt]->SetXTitle("x");
      hJetFragmentCh[icone][ipt]->Sumw2();
      fListHistos->Add(hJetFragmentCh[icone][ipt]) ;
      
      hJetScalar[icone][ipt]  = new TH1F
	("JetScalarCone"+scone[icone]+"Pt"+sptcut[ipt],"Scalar product jet*particle, cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",100,0,1); 
      hJetScalar[icone][ipt]->SetYTitle("N");
      hJetScalar[icone][ipt]->SetXTitle("Product");
      hJetScalar[icone][ipt]->Sumw2();
      fListHistos->Add(hJetScalar[icone][ipt]) ; 
      
 hJetScalarCh[icone][ipt]  = new TH1F
	("JetScalarChCone"+scone[icone]+"Pt"+sptcut[ipt],"Scalar product jet*particle(charged), cone ="+cone[icone]+", pt>" +ptcut[ipt]+" GeV/c",100,0,1); 
      hJetScalarCh[icone][ipt]->SetYTitle("N");
      hJetScalarCh[icone][ipt]->SetXTitle("Product");
      hJetScalarCh[icone][ipt]->Sumw2();
      fListHistos->Add(hJetScalarCh[icone][ipt]) ; 
      
    }//ipt
  } //icone
}

//____________________________________________________________________________
void  AliPHOSGammaJet::MakePhoton(TParticle * particle)
{
  //if(PhotonDetected(particle)){
  //modify energy
  TLorentzVector lv;
  particle->Momentum(lv);
  Float_t pbefore = particle->Pt();  
  Double_t energy = particle->Energy()  ;
  Double_t modenergy = MakeEnergy(energy) ;
  //Info("MakePhoton","Energy %f, Modif %f",energy,modenergy);
  //modenergy = energy;
  
  
  //Mod theta phi
  //Double_t modtheta = 0.,modphi = 0.; 
  //MakeThetaPhi(energy,lv,modtheta,modphi);
  //modtheta = particle->Theta();
  //modphi = particle->Phi();
  
  // get the detected direction
  TVector3 pos = lv.Vect(); 
  pos*=460./energy;
  TVector3 modpos = MakePosition(energy, pos) ;
  modpos *= modenergy / 460.;

  Float_t modtheta = modpos.Theta();
  Float_t modphi   = modpos.Phi(); 
  Float_t theta = particle->Theta();
  Float_t phi   = particle->Phi(); 
  
  // Set the modified 4-momentum of the reconstructed particle
  Float_t py = modenergy*TMath::Sin(modphi)*TMath::Sin(modtheta);
  Float_t px = modenergy*TMath::Cos(modphi)*TMath::Sin(modtheta);
  Float_t pz = modenergy*TMath::Cos(modtheta); 
  
  particle->SetMomentum(px, py, pz, modenergy) ;
  
  Float_t pafter = particle->Pt();
  //Info("MakePhoton","Theta dif %f",theta-particle->Theta());
  //Info("MakePhoton","Phi   dif %f",phi-particle->Phi());
  dynamic_cast<TH2F*>(fListHistos->FindObject("PtmodPtnomod"))->Fill(pbefore,pafter);
  dynamic_cast<TH2F*>(fListHistos->FindObject("PhimodPhinomod"))->Fill(phi,particle->Phi());
  dynamic_cast<TH2F*>(fListHistos->FindObject("ThmodThnomod"))->Fill(theta,particle->Theta());  
  //}
//else
//  particle = new TParticle();
}

//____________________________________________________________________________
TVector3 AliPHOSGammaJet::MakePosition(const Double_t energy, const TVector3 pos)
{
  // Smears the impact position according to the energy dependent position resolution
  // A gaussian position distribution is assumed

  TVector3 newpos ;

  Double_t sigma = SigmaP(energy) ;
  Double_t x = fRan.Gaus( pos.X(), sigma ) ;
  Double_t z = fRan.Gaus( pos.Z(), sigma ) ;
  Double_t y = pos.Y() ; 
  
  newpos.SetX(x) ; 
  newpos.SetY(y) ; 
  newpos.SetZ(z) ; 

  // Info("MakePosition","Theta dif %f",pos.Theta()-newpos.Theta());
//   Info("MakePosition","Phi   dif %f",pos.Phi()-newpos.Phi());	      
  return newpos ; 
}

//____________________________________________________________________________
void AliPHOSGammaJet::Pi0Decay(Double_t mPi0, TLorentzVector &p0, 
			       TLorentzVector &p1, TLorentzVector &p2, Double_t &angle) {
  // Perform isotropic decay pi0 -> 2 photons
  // p0 is pi0 4-momentum (inut)
  // p1 and p2 are photon 4-momenta (output)
  //  cout<<"Boost vector"<<endl;
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
// //____________________________________________________________________________
// Bool_t AliPHOSGammaJet::PhotonDetected(TParticle *particle)
// {
//   // Returns kTRUE is a particle is reconstructed, kFALSE otherwise.
//   // A particle is reconstructed if it is a photon and accepted with the
//   // probability Efficiency(pt,eta) depending on pt and eta.

//   Bool_t detected = kFALSE;
//   if (particle->GetPdgCode()== 22) {
//     Float_t pt  = particle->Pt();
//     if (gRandom->Rndm() < PhotonEfficiency(pt)) detected = kTRUE;
//   }
//   return detected;
// }
// //____________________________________________________________________________
// Float_t AliPHOSGammaJet::PhotonEfficiency(Float_t pt)
// {
//   // Detection probability vs. pt and eta, i.e. a probability to detect
//   // a particle with transverse momentum pt and speudorapidity eta.
//   // For the moment assume that photons  are detected with
//   // 95% efficiency within |eta|<0.7 and pt>0.5 GeV/c, and with 0% efficiency
//   // beyond that acceptance

//   const Float_t kPtLimit  = 0.5;
//   Float_t efficiency = 0.0;
//   if (pt > kPtLimit) efficiency = 0.95;
//   return efficiency;
// }

//____________________________________________________________________________
void AliPHOSGammaJet::Plot(TString what, Option_t * option) const
{
  TH2F * h = dynamic_cast<TH2F*>(fOutputFile->Get(what));
  if(h){
    h->Draw();
  }
  else if (what == "all") {
  TCanvas * can = new TCanvas("GammaJet", "Gamma-Jet Study",10,40,1600,1200);
  can->cd() ; 
  can->Range(0,0,22,20);
  TPaveLabel *pl1 = new TPaveLabel(1,18,20,19.5,"Titre","br");
  pl1->SetFillColor(18);
  pl1->SetTextFont(32);
  pl1->SetTextColor(49);
  pl1->Draw();
  Int_t index ; 
  TPad * pad = 0; 
  Float_t begx = -0.29, begy = 0., endx = 0., endy = 0.30 ; 
  for (index = 0 ; index < fListHistos->GetEntries() ; index++) {
    TString name("pad") ; 
    name += index ; 
    begx += 0.30 ;
    endx += 0.30 ;
    if (begx >= 1.0 || endx >= 1.0) {
      begx = 0.01 ; 
      endx = 0.30 ; 
      begy += 0.30 ;
      endy += 0.30 ; 
    } 
    printf("%f %f %f %f \n", begx, begy, endx, endy) ; 
    pad = new TPad(name,"This is a pad",begx,begy,endx,endy,33);
    pad->Draw();
    pad->cd() ; 
    fListHistos->At(index)->Draw(option) ; 
    pad->Modified() ; 
    pad->Update() ; 
    can->cd() ; 
  }

  }
  else{
    Info("Draw", "Histogram %s does not exist or unknown option", what.Data()) ;
     fOutputFile->ls();
  }
}

//____________________________________________________________________________
void AliPHOSGammaJet::Print(char * opt) 
{
  // Print 
  Info("Print", "%s %s", GetName(), GetTitle() ) ;
 
  printf("Scalar Product cut: %f\n", fScalarProductCut) ;
  printf("Eta cut           : %f\n", fEtaCut) ;
  printf("D phi max cut     : %f\n", fPhiMaxCut) ; 
  printf("D phi min cut     : %f\n", fPhiMinCut) ;
  printf("Leading Ratio max cut     : %f\n", fRatioMaxCut) ; 
  printf("Leading Ratio min cut     : %f\n", fRatioMinCut) ;
  printf("Jet Ratio max cut     : %f\n", fJetRatioMaxCut) ; 
  printf("Jet Ratio min cut     : %f\n", fJetRatioMinCut) ;
  printf("Fast recons       : %d\n", fOptFast);
  printf("Inv Mass max cut  : %f\n", fInvMassMaxCut) ; 
  printf("Inv Mass min cut  : %f\n", fInvMassMinCut) ; 
 
} 


//___________________________________________________________________
void AliPHOSGammaJet::SetJetParticleListInCone(TList &particleList, 
					       TList &newparticlelist, 
					       TLorentzVector & jet,
					       const TLorentzVector leading,
					       const Double_t cone, 
					       const Double_t ptcut){
  //cout<<"Create  "<<cone<<" "<<ptcut<<endl;
  Double_t rad = 0;
  Double_t phi = leading.Phi();
  Double_t eta = leading.Eta();
  Int_t iPrimary=-1;
  TLorentzVector lv;
  TParticle *particle = new TParticle();
  TIter next(&particleList);

  if(!fOnlyCharged){
    while ( (particle = (TParticle*)next()) ) {
      iPrimary++;  
      Int_t ksCode = particle->GetStatusCode();
      if((ksCode == 1)||(ksCode == 0)){
	//1 pythia
	//0 HIJING
	
	rad = TMath::Sqrt(TMath::Power(particle->Eta()-eta,2)+
			  TMath::Power(particle->Phi()-phi,2));     
	
	if((rad<cone) && (ptcut<particle->Pt())){
	  //Info("SetJetParticleListInCone","particle %d",iPrimary);
	  newparticlelist.Add(new TParticle(*particle));
	  particle->Momentum(lv);
	  jet+=lv;
	}
      }// status code 1, no gamma
    }// while 
  }//Not only charge
  else{
    while ( (particle = (TParticle*)next()) ) {
      iPrimary++;  
      Int_t ksCode = particle->GetStatusCode();
      Double_t ksSign  = particle->GetPDG(0)->Charge();
      if((ksSign!=0)&&((ksCode == 1)||(ksCode == 0))){
	//1 pythia
	//0 HIJING
	
	rad = TMath::Sqrt(TMath::Power(particle->Eta()-eta,2)+
			  TMath::Power(particle->Phi()-phi,2));     
	
	if((rad<cone) && (ptcut<particle->Pt())){
	  //Info("SetJetParticleListInCone","particle %d",iPrimary);
	  newparticlelist.Add(new TParticle(*particle));
	  particle->Momentum(lv);
	  jet+=lv;
	}
      }// status code 1, no gamma
    }// while 
  }
  //cout<<"Create  end "<<cone<<" "<<ptcut<<endl;
}


//  //_______________________________________________________________-
//  void InitJetQuenching(const TString input, const TString quenching){
  
//    TFile *f1=new TFile(input,"read");
//    AliRun* fAlice1 = (AliRun*)(f1->Get("gAlice"));
//    AliGenPythia * fPyth1 = (AliGenPythia*) fAlice1->Generator();
//    fPyth1->Init();
  
//    TFile *f2=new TFile(quenching,"read");
//    AliRun* fAlice2 = (AliRun*)(f2->Get("gAlice"));
//    AliGenPythia * fPyth2 = (AliGenPythia*) fAlice2->Generator();
//    fPyth2->Init();

//  }

//____________________________________________________________________________
Double_t AliPHOSGammaJet::SigmaE(Double_t energy)
{
  // Calculates the energy dependent energy resolution
  
  Double_t rv = -1 ; 
  
  rv = TMath::Sqrt( TMath::Power(fResPara1/energy, 2) 
	       + TMath::Power(fResPara2/TMath::Sqrt(energy), 2) 
	       + TMath::Power(fResPara3, 2) ) ;  

  return rv * energy ; 
}

//____________________________________________________________________________
Double_t AliPHOSGammaJet::SigmaP(Double_t energy)
{
  // Calculates the energy dependent position resolution 

  Double_t sigma = TMath::Sqrt(TMath::Power(fPosParaA,2) + TMath::Power(fPosParaB,2) / energy) ; 


  return sigma   ; // in cm  
}


//____________________________________________________________________________
void AliPHOSGammaJet::TransformAddJetToList(AliPHOSGetter * gime, 
					    TList & particleList, 
					    const TLorentzVector jet1, 
					    const TLorentzVector jet2) 
{
  TParticle * particle = new TParticle();
  Int_t n = -1;
  Int_t  nparticles = gime->NPrimaries() ; 
  Int_t iParticle=0 ;
  if(fOptFast){

    TLorentzVector pPi0, pGamma1, pGamma2 ;
    Double_t angle = 0, cellDistance = 0.;

    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {

      particle = new TParticle(*(gime->Primary(iParticle))) ;
      
      if(particle->GetStatusCode() == 1){

	// Keep particles different from Pi0
	if((particle->GetPdgCode() != 111)&&(TMath::Abs(particle->Eta())<fEtaCut)){
	  if(particle->GetPdgCode()==22){
	    //Info("TransformAddJetToList","Photon0 before: %f", particle->Energy());
	    MakePhoton(particle);
	    //Info("TransformAddJetToList","Photon0 after: %f", particle->Energy());
	  }
	  TransformParticle(particle,jet1,jet2);
	  particleList.Add(particle);
	  n++;
	} 

	//Decay Pi0 and keep it with different status name
	//Keep decay photons
	if((particle->GetPdgCode() == 111) && (TMath::Abs(particle->Eta())<fEtaCut+2.)) {

	  particle->Momentum(pPi0);
	  Pi0Decay(particle->GetMass(),pPi0,pGamma1,pGamma2,angle);

	  cellDistance = angle*460; //cm
	  if (cellDistance < fMinDistance) {
	    TransformParticle(particle,jet1,jet2);
	    particleList.Add(particle);
	    n ++ ; 
	  }// if cell<distance	
	  else {
	    n += 3 ; 
	    dynamic_cast<TH2F*>(fListHistos->FindObject("AnglePair"))->Fill(pPi0.E(),angle);

	    TParticle * photon1 = new TParticle(22,1,0,0,0,0,pGamma1.Px(),pGamma1.Py(),
						pGamma1.Pz(),pGamma1.E(),0,0,0,0);
	    TParticle * photon2 = new TParticle(22,1,0,0,0,0,pGamma2.Px(),pGamma2.Py(),
						pGamma2.Pz(),pGamma2.E(),0,0,0,0);
	    //photon1->SetWeight(1);
	    //photon2->SetWeight(2);
	    particle->SetStatusCode(2);
	    
	    //Info("TransformAddJetToList","Photon 1 and 2 before: %f  %f", photon1->Energy(), photon2->Energy());
	    MakePhoton(photon1);
	    MakePhoton(photon2);
	    //Info("TransformAddJetToList","Photon 1 and 2 after: %f  %f", photon1->Energy(), photon2->Energy());
	    //particleList.Add(particle);
	    if((TMath::Abs(photon1->Eta())<fEtaCut)){
	      TransformParticle(photon1,jet1,jet2);
	      particleList.Add(photon1);
	    }
	    if((TMath::Abs(photon2->Eta())<fEtaCut)){
	      TransformParticle(photon2,jet1,jet2);
	      particleList.Add(photon2);
	    }
	    //photon1->Print();
	    //photon2->Print();
	  }//if cell>distance
	}//if pi0
      }//final particle, etacut
    }//for (iParticle<nParticle)
  }//optfast
  else{

     //Test to Transform one jet into another
     cout<<"Jet 1: phi "<<jet1.Phi()<<" theta "<<jet1.Theta()
	 <<" eta "<<jet1.Eta()<<" Pt "<<jet1.Pt()<<" E "<<jet1.E()<<" Pz "<<jet1.Pz()<<endl;
     cout<<"Jet 2: phi "<<jet2.Phi()<<" theta "<<jet2.Theta()
	 <<" eta "<<jet2.Eta()<<" Pt "<<jet2.Pt()<<" E "<<jet2.E()<<" Pz "<<jet2.Pz()<<endl;
     TParticle * jet22 = new TParticle();
     jet22->SetMomentum(jet2);
     TransformParticle(jet22,jet1,jet2);
     cout<<"Jet2->1: phi "<<jet22->Phi()<<" theta "<<jet22->Theta()
	 <<" eta "<<jet22->Eta()<<" Pt "<<jet22->Pt()<<" E "<<jet22->Energy()<<" Pz "<<jet22->Pz()<<endl;

    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
   
      particle = new TParticle(*(gime->Primary(iParticle))) ;
     
      //cout<<"Inside loop "<<iParticle<<endl;
      //Keep Stable particles within eta range
      if((particle->GetStatusCode() == 1)&&(TMath::Abs(particle->Eta())<fEtaCut)){

	//TransformParticle(particle,jet1,jet2);
	//cout<<"Inside if"<<endl;
	particleList.Add(particle);
	n++;  
	//cout<<"Inside loop, end"<<endl;
      }//final particle
    }//for (iParticle<nParticle)
  }// no optfast
  //Info("TransformAddJetToList","N %d E %f",n,e);

}

//____________________________________________________________________________
void  AliPHOSGammaJet::TransformParticle(TParticle *particle, const TLorentzVector jet1, 
					 const TLorentzVector jet2)
{
  //
  // Perform Lorentz Transformation and Rotation
  // Copy paste from AliGenPythiaJet 

  //cout<<"Inside Transform "<<endl;
  
  Float_t px   =  particle->Px();
  Float_t py   =  particle->Py();
  Float_t pz   =  particle->Pz();
  Float_t e    =  particle->Energy();
  //
  // 1 st Lorentz Transform
  //
  // Pz --> Pz' = 0
  
  Float_t betalorentz =jet2.Pz()/jet2.Energy();
  //cout<<" b "<<betalorentz<<endl;
  Float_t gammalorentz = 1./TMath::Sqrt(1. - betalorentz * betalorentz);
  //cout<<"g "<<gammalorentz<<endl;
  Float_t pzt =  gammalorentz * pz        - gammalorentz * betalorentz * e;
  Float_t et  = -gammalorentz * betalorentz * pz + gammalorentz        * e;

  cout<<"Pzt 1 "<<pzt<<" et 1"<<et<<endl;
 
  //
  // Phi Rotation
  //
  Float_t phi = jet1.Phi() - jet2.Phi();
  //cout<<" phi12 "<<phi<<endl;
  if(phi<0)
    phi+=TMath::TwoPi();

  Float_t pxt =   TMath::Cos(phi) * px -  TMath::Sin(phi) * py;
  Float_t pyt =   TMath::Sin(phi) * px +  TMath::Cos(phi) * py;
  //
  //
 
  particle->SetMomentum(pxt,pyt,pzt,et);

  //
  // 2 nd Lorentz Transform
  //
  // pz' = 0 --> pz''

  Float_t px2   =  particle->Px();
  Float_t py2   =  particle->Py();
  Float_t pz2   =  particle->Pz();
  Float_t e2    =  particle->Energy();

  Float_t betalorentz2  =-jet2.Pz()/jet2.E();
  //cout<<" b2 "<<betalorentz2<<endl;
  Float_t gammalorentz2 = 1./TMath::Sqrt(1. - betalorentz2 * betalorentz2);
  //cout<<" g2 "<< gammalorentz2<<endl;
  Float_t pzt2 =  gammalorentz2 * pz2        - gammalorentz2 * betalorentz2 * e2;
  Float_t et2  = -gammalorentz2 * betalorentz2 * pz2 + gammalorentz2        * e2;
 
 particle->SetMomentum(px2,py2,pzt2,et2);
 cout<<"Pzt 2 "<<pzt2<<" et 2 "<<et2<<endl;
} 






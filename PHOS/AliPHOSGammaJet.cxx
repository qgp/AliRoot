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
#include "TMatrixD.h"
#include "TList.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TParticle.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TPad.h"
#include "AliRun.h"
#include "AliPHOSGammaJet.h" 
#include "AliPHOSGetter.h" 

ClassImp(AliPHOSGammaJet)

//____________________________________________________________________________
AliPHOSGammaJet::AliPHOSGammaJet() : TTask() 
{
  // ctor
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.Reset(0.);
  fMatrixParam    = 0x0;
  fScalarProductCut = 0;
  fEtaCut         = 0 ;
  fPhiMaxCut      = 0 ;
  fPhiMinCut      = 0 ;
  fPtEffCut       = 0 ;
  fInvMassMaxCut  = 0 ;
  fInvMassMinCut  = 0 ;
  fRatioMaxCut    = 0 ;
  fRatioMinCut    = 0 ;
  fFastRec        = 0 ;
  fOptFast        = kFALSE ;
              
  fOutputFile     = new TFile(gDirectory->GetName()) ;
  fOutputFileName = gDirectory->GetName() ;
  fPosParaA      = 0. ;                      
  fPosParaB      = 0. ;
  fRan            = 0 ;                            
  fResPara1       = 0. ;                       
  fResPara2       = 0. ;                        
  fResPara3       = 0. ;  
        
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
AliPHOSGammaJet::AliPHOSGammaJet(const TString inputfilename) : TTask("GammaJet","Analysis of gamma-jet correlations")
{
  // ctor 
  fFastRec = new AliPHOSFastGlobalReconstruction(inputfilename);  
  AliPHOSGetter::GetInstance(inputfilename) ;
  InitParameters();
  fListHistos = 0 ; 
}

//____________________________________________________________________________
AliPHOSGammaJet::AliPHOSGammaJet(const AliPHOSGammaJet & gj) 
{
  // cpy ctor
  fAngleMaxParam  = gj.fAngleMaxParam;
  fMatrixParam    = gj.fMatrixParam;
  fScalarProductCut= gj.fScalarProductCut;
  fEtaCut         = gj.fEtaCut ;
  fInvMassMaxCut  = gj.fInvMassMaxCut ;
  fInvMassMinCut  = gj.fInvMassMinCut ;
  fPtEffCut       = gj.fPtEffCut ;
  fFastRec        = gj.fFastRec ;
  fOptFast        = gj.fOptFast ;
  fOutputFile     = gj.fOutputFile ;
  fOutputFileName = gj.fOutputFileName ;
  fRatioMaxCut    = gj.fRatioMaxCut ;
  fRatioMinCut    = gj.fRatioMinCut ;  
  fResPara1       = gj.fResPara1 ;    
  fResPara2       = gj.fResPara2 ; 
  fResPara3       = gj.fResPara3 ; 
  fPhiMaxCut      = gj.fPhiMaxCut  ;
  fPhiMinCut      = gj.fPhiMinCut ;
  fPosParaA       = gj.fPosParaA ;    
  fPosParaB       = gj.fPosParaB ;  

  SetName(gj.GetName()) ; 
  SetTitle(gj.GetTitle()) ; 
}

//____________________________________________________________________________
AliPHOSGammaJet::~AliPHOSGammaJet() 
{
  fOutputFile->Close() ;

}

//____________________________________________________________________________
void AliPHOSGammaJet::CreateParticleList(AliPHOSGetter * gime, TList & particleList) 
{
  TParticle * particle = 0x0;
  TLorentzVector pPi0, pGamma1, pGamma2 ;
  Double_t angle = 0;
  
  Int_t n = -1; 
  Int_t  nparticles = gime->NPrimaries() ; 
  Int_t iParticle=0 ;
  if(fOptFast){
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      //Keep original partons
      particle = new TParticle(*(gime->Primary(iParticle))) ;
      if((particle->GetStatusCode()== 21)){
	particleList.Add(particle);
	n++;
      }
      //Info("CreateParticleList","after parton");
      //Keep Stable particles within eta range
      
      if((particle->GetStatusCode() == 1)&&(TMath::Abs(particle->Eta())<fEtaCut)){
	// Keep particles different from Pi0
	if(particle->GetPdgCode() != 111){
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
	  
	  //Info("CreateParticleList","Photon 1 and 2 before: %f  %f", photon1->Energy(), photon2->Energy());
	  MakePhoton(photon1);
	  MakePhoton(photon2);
	  //Info("CreateParticleList","Photon 1 and 2 after: %f  %f", photon1->Energy(), photon2->Energy());
	  particleList.Add(particle);
	  particleList.Add(photon1);
	  particleList.Add(photon2);
	  //photon1->Print();
	  //photon2->Print();  
	}//if pi0
      }//final particle, etacut
    }//for (iParticle<nParticle)
  }//optfast
  else{
    for (iParticle=0 ; iParticle < nparticles ; iParticle++) {
      //Keep original partons
      particle = new TParticle(*(gime->Primary(iParticle))) ;
      if((particle->GetStatusCode()== 21)){
	particleList.Add(particle);
	n++;
      }
      //Keep Stable particles within eta range
      
      if((particle->GetStatusCode() == 1)&&(TMath::Abs(particle->Eta())<fEtaCut)){
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
  Int_t nEvent1 = gime->MaxEvent() ;   
  //nEvent1=10 ;
  Int_t iEvent = 0 ; 
  for ( iEvent = 0 ; iEvent < nEvent1 ; iEvent++) {
    if (iEvent <= 100 || iEvent%10 == 0)
      Info("Exec", "Event %d", iEvent) ;
    
    gime->Event(iEvent, "P") ; 
    if(fOptFast)
      fFastRec->FastReconstruction(iEvent);
    
    //Fill particle list       
    TList particleList ; 
    CreateParticleList(gime, particleList);
    
    TLorentzVector gamma,charge,pi0,neutral,any;
    Int_t idg = -1;
    
    GetGammaJet(gime,particleList,gamma, idg);
   
    Double_t ptg = gamma.Pt();
        
    if(ptg){
      Info("Exec", "Gamma: %f", ptg) ;

      dynamic_cast<TH1F*>(fListHistos->FindObject("NGamma"))->Fill(ptg);

        
      GetAnyLeading(particleList,any, idg,gamma);
      if(any.Pt()>0.){
	dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeading"))->Fill(ptg,any.Pt());

	Info("Exec", "Any Leading : %f", any.Pt()) ;
	GetLeadingCharge(particleList,charge, idg,gamma);
	Info("Exec", "Charge: %f", charge.Pt()) ;
	GetLeadingPi0(particleList,pi0, idg,gamma);   
	Info("Exec", "Pi0: %f", pi0.Pt()) ;
	GetLeadingNeutral(particleList,neutral, idg,gamma);
	TString type = "Any";
	GetJet(particleList, gamma, any,type,idg);
	Info("Exec", "Leading Any2 : %f", any.Pt()) ;
	if((neutral.Pt()>charge.Pt())&&(neutral.Pt()>pi0.Pt()))
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingNeutral"))->Fill(ptg,neutral.Pt());

	dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0LeadingBefore"))->Fill(charge.Pt(),pi0.Pt());

	if(charge.Pt()> pi0.Pt()){
	  Info("Exec", "Leading Charge: %f", charge.Pt()) ;
	  dynamic_cast<TNtuple*>(fListHistos->FindObject("PtLeadingNT"))->Fill(ptg, charge.Pt(),pi0.Pt(), any.Pt()) ;
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0Leading"))->Fill(charge.Pt(),pi0.Pt());
	  dynamic_cast<TH2F*>(fListHistos->FindObject("ChargeRatio"))->Fill(ptg,charge.Pt()/ptg);
	  dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiCharge"))->Fill(ptg,charge.Phi()-gamma.Phi());
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingCharge"))->Fill(ptg,charge.Pt());
	  type = "Charge";
	  GetJet(particleList, gamma,charge,type,idg);
	}
	else if(pi0.Pt()> charge.Pt()){
	  dynamic_cast<TNtuple*>(fListHistos->FindObject("PtLeadingNT"))->Fill(ptg, charge.Pt(),pi0.Pt(), any.Pt()) ;
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtChargePi0Leading"))->Fill(charge.Pt(),pi0.Pt());
	  Info("Exec", "Leading Neutral (pi0): %f", pi0.Pt()) ;
	  dynamic_cast<TH2F*>(fListHistos->FindObject("Pi0Ratio"))->Fill(ptg,pi0.Pt()/ptg);
	  dynamic_cast<TH2F*>(fListHistos->FindObject("PtGammaLeadingPi0"))->Fill(ptg,pi0.Pt());
	  dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiPi0"))->Fill(ptg,pi0.Phi()-gamma.Phi());
	  TString type = "Pi0";
	  GetJet(particleList, gamma, pi0,type,idg);
	}//pi0
	else{
	  Info("Exec", "Leading Neutral: %f", neutral.Pt()) ;
	}//else
	
      }	
      else{
	//cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>EtaPhiGammaNoLeading"<<gamma.Eta()<<"  "<<gamma.Phi()<<endl;
	dynamic_cast<TH2F*>(fListHistos->FindObject("EtaPhiGammaNoLeading"))->Fill(gamma.Eta(),gamma.Phi()); 
      }
      
    }//if ptg
  }//loop: events
  
  fOutputFile->Write() ; 
  fOutputFile->cd();
  this->Write();
}    


//______________________________________________________________________________________
void AliPHOSGammaJet::GetJetParticleListInCone(TList &particleList, TList &newparticlelist, TLorentzVector &jet,
					       Double_t eta, Double_t phi ,Double_t cone, 
					       Double_t ptcut, Int_t idg){
  //cout<<"Create  "<<cone<<" "<<ptcut<<endl;
  Double_t rad = 0;
  Int_t iPrimary=-1;
  TLorentzVector lv;
  TParticle *particle = 0x0;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    //Info("GetJetParticleListInCone","particle %d",iPrimary);
    
    Int_t ksCode = particle->GetStatusCode();
    if((ksCode == 1) &&  idg!= iPrimary){
      
      
      rad = TMath::Sqrt(TMath::Power(particle->Eta()-eta,2)+
			TMath::Power(particle->Phi()-phi,2));     
      
      if((rad<cone) && (ptcut<particle->Pt())){
	newparticlelist.Add(particle);
	particle->Momentum(lv);
	jet+=lv;
      }
    }// status code 1, no gamma
  }// while 
  //cout<<"Create  end"<<cone<<" "<<ptcut<<endl;
}

//______________________________________________________________________________________
void AliPHOSGammaJet::EraseParticlesAwayFromJet(TList &particleList, TLorentzVector &jet){
 
  TLorentzVector newjet,lv;
  Int_t iPrimary=-1;
  TVector3 jetvect(0,0,0);
  TVector3 ivect(0,0,0);

  TParticle *particle = 0x0;
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
void AliPHOSGammaJet::FillJetParticleHistos(TList &particleList, TLorentzVector &jet, Double_t ptg, Double_t phig,
					    TString cone, TString ptcut, TString type, Double_t baseline){

  TParticle *particle = 0x0;
  Int_t iPrimary=-1;
  TLorentzVector lv;
  TVector3 jetvect(0,0,0);
  TVector3 ivect(0,0,0);
 
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    particle->Momentum(lv);
    
    jetvect = jet.Vect();
    ivect = lv.Vect();
    //Double_t scalar = jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag());
    //cout<<scalar<<" "<<ivect.Mag()<<" "<<jetvect.Mag()<< " "<<jetvect.Dot(ivect)<<endl;
    dynamic_cast<TH1F*>(fListHistos->FindObject("JetPartitionCone"+cone+"Pt"+ptcut))->
      Fill(lv.Pt()/ptg);
    if(particle->GetPDG(0)->Charge()!= 0){
      dynamic_cast<TH1F*>(fListHistos->FindObject("JetPartitionChCone"+cone+"Pt"+ptcut))->
	Fill(lv.Pt()/ptg);
      
      dynamic_cast<TH1F*>(fListHistos->FindObject("JetScalarChCone"+cone+"Pt"+ptcut))-> 
	Fill(jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag()));
    }
    else
      dynamic_cast<TH1F*>(fListHistos->FindObject("JetScalarCone"+cone+"Pt"+ptcut))-> 
	Fill(jetvect.Dot(ivect)/(ivect.Mag()*jetvect.Mag()));
    
  }// while 

  Double_t ptjet = jet.Pt();
  Double_t phijet = jet.Phi();
  if(ptjet>0.){
    //cout<<"Jet"+type+"RatioCone"+cone+"Pt"+ptcut<<endl;
    dynamic_cast<TH2F*>(fListHistos->FindObject("Jet"+type+"RatioCone"+cone+"Pt"+ptcut))->Fill(ptg,ptjet/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("Jet"+type+"PhiCone"+cone+"Pt"+ptcut))->Fill(ptg,phijet-phig);
    if(TMath::Abs(ptjet/ptg-baseline)<fPtEffCut){
      //cout<<"NAnyJetCone"+cone+"Pt"+ptcut<<endl;
      dynamic_cast<TH1F*>(fListHistos->FindObject("N"+type+"JetCone"+cone+"Pt"+ptcut))->Fill(ptg);
    }
  }
  //cout<<"end fill"<<endl;
 }
//____________________________________________________________________________
void AliPHOSGammaJet::GetJet(TList &particleList, TLorentzVector & gamma, TLorentzVector & leading, 
			     TString & type,Int_t & idg) 

{
  // TLorentzVector cone03pt05, cone04pt05, cone05pt05, cone06pt05, lv;
  //TLorentzVector cone03pt1, cone04pt1, cone05pt1, cone06pt1;
  //TLorentzVector cone03pt0, cone04pt0, cone05pt0, cone06pt0;

  //TParticle * particle = 0x0;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();
  //Double_t etag = gamma.Eta();
  Double_t phi = leading.Phi();
  Double_t eta = leading.Eta();
  //Double_t  rad = 0.;

  TList pl;
  TLorentzVector jet0(0,0,0,0);

  GetJetParticleListInCone(particleList, pl, jet0,eta, phi ,0.6, 0., idg);
  Float_t cone[]  = {0.3,0.4,0.5,0.6};
  Float_t ptcut[] = {0.0,0.5,1.0};
  TString scone[]  = {"03","04","05","06"};
  TString sptcut[] = {"0","05","1"};
  // Double_t baseline[3][4];
  
  //baseline[0][0] = 0.950;  baseline[0][1] = 0.947; baseline[0][2] = 0.937;//cone 0.3
  //baseline[1][0] = 0.965;  baseline[1][1] = 0.960; baseline[1][2] = 0.946;//cone 0.4
  //baseline[2][0] = 0.974;  baseline[2][1] = 0.968; baseline[2][2] = 0.951;//cone 0.5
  //baseline[3][0] = 0.981;  baseline[3][1] = 0.974; baseline[3][2] = 0.954;//cone 0.6

  for(Int_t icone = 0; icone<4; icone++){
    for(Int_t ipt = 0; ipt<3;ipt++){
      TList newpl;
      TLorentzVector jet(0,0,0,0);

      GetJetParticleListInCone(pl, newpl, jet,eta, phi ,cone[icone], ptcut[ipt], idg);
      EraseParticlesAwayFromJet(newpl,jet);
     //  Double_t base = (*fMatrixParam)(icone,ipt);
//       cout<<base<<endl;
      FillJetParticleHistos(newpl, jet, ptg, phig,scone[icone],sptcut[ipt], type, (*fMatrixParam)(icone,ipt));
     
    }
  }
 
  //cout<<"End get jet"<<endl;
}
//____________________________________________________________________________
void AliPHOSGammaJet::GetGammaJet(AliPHOSGetter *gime, TList &particleList, TLorentzVector &gamma, 
				  Int_t & id) 
{
  TParticle *particle = 0x0;
  Int_t module = -1;
  Double_t x = 0., z = 0.;

  Int_t iPrimary=-1, idmotherg = -1;
  
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    Int_t iMother= particle->GetMother(0);
    
    if (ksCode == 21 && iMother == -1)
      if(particle->GetPdgCode()==22){
	idmotherg = iPrimary;
	//cout<<"idmother "<<idmotherg<<endl;
      }
    if(ksCode == 1){
      
      if(idmotherg == particle->GetMother(0)){
	const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
	geom->ImpactOnEmc(static_cast<double>(particle->Theta()),static_cast<double>(particle->Phi()), 
			  module,z,x);
	if(module != 0){
	  particle->Momentum(gamma);
	  id = iPrimary;
	}
 	break;
      }
    }// kscode == 1
  }// while
}
//____________________________________________________________________________
// void AliPHOSGammaJet::GetGammaJet(AliPHOSGetter *gime, TLorentzVector &gamma, Int_t & idg, Int_t idparton) 
// {
//   TParticle *particle = 0x0;
//   //Int_t module = -1;
//   //Double_t x = 0., z = 0.;
//   TParticle *hardParton1 = 0 , *hardParton2 = 0, *mothergamma = 0, *parton = 0;
//   Float_t ptmax1=0, ptmax2=0;
//   Int_t idmotherg = -1, id1 = -1, id2 = -2;

//   for (Int_t iPrimary = 0 ; iPrimary < nparticles ; iPrimary++) {
//     particle = gAlice->Particle(iPrimary) ;
//     Int_t ksCode = particle->GetStatusCode();
//     Int_t iMother= particle->GetMother(0);
//     Float_t pt   = particle->Pt();
//     if (ksCode == 21 && iMother == -1) {
     
//       if      (pt > ptmax1) {
// 	ptmax2 = ptmax1;
// 	hardParton2 = hardParton1;
// 	ptmax1 = pt;
// 	hardParton1 = particle;
// 	id1 = iPrimary;
//       }
//       else if (pt > ptmax2) {
// 	ptmax2 = pt;
// 	hardParton2 = particle;
// 	id2 = iPrimary;
//       }
//     }
//     if(hardParton1->GetPdgCode()==22){
//       mothergamma = hardparton1;
//       idmotherg = id1;
//       parton = hardparton2;
//     }
//     else{
//       mothergamma = hardparton2;
//       idmotherg = id2;
//       parton = hardparton1;
//       if(hardParton2->GetPdgCode()!=22)
// 	Info("GetGammaJet","No mother photon !!!!")
//     }
//     // if (ksCode == 21 && iMother == -1)
// //       if(particle->GetPdgCode()==22){
// // 	idmotherg = iPrimary;
// // 	//cout<<"idmother "<<idmotherg<<endl;
// //       }
//     if(ksCode == 1){
      
//       if(idmotherg == particle->GetMother(0)){
// 	Info("GetGammaJet","Mother %f, dougther %f",gammamother->Energy(),particle->Energy());
// 	const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
// 	geom->ImpactOnEmc(static_cast<double>(particle->Theta()),static_cast<double>(particle->Phi()), 
// 			  module,z,x);
// 	if(module != 0){
// 	  particle->Momentum(gamma);
// 	  id = iPrimary;
// 	}
//  	//break;
//       }
//     }// kscode == 1
//   }// while
// }
 
//____________________________________________________________________________
void AliPHOSGammaJet::GetLeadingCharge(TList &particleList, TLorentzVector &charge, Int_t & id,
				       TLorentzVector & gamma) 
{
  TParticle *particle = 0x0;
  
 
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    
    if((ksCode == 1)&&(id != iPrimary)
       &&(particle->GetPDG(0)->Charge()!=0)){
      pti = particle->Pt(); 
      if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	phi = particle->Phi();
	if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	  ptmax = pti;
	  particle->Momentum(charge);
	}//phi cut  
      }//ptmax 
    }// kscode == 1
  }// while
  if((ptmax != 0.0)){
  dynamic_cast<TH2F*>(fListHistos->FindObject("ChargeRatioAll"))->Fill(ptg,charge.Pt()/ptg);
  dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiChargeAll"))->Fill(ptg,charge.Phi()-phig);
  }
}


//____________________________________________________________________________
void AliPHOSGammaJet::GetLeadingNeutral(TList &particleList, TLorentzVector &neutral, Int_t & id,
					TLorentzVector & gamma) 
{
  TParticle *particle = 0x0;
  
 
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1, type = -1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    pti = particle->Pt(); 
    if((ksCode == 1)&&(id != iPrimary)&&(particle->GetPDG(0)->Charge()==0)&&(particle->GetPdgCode()!=111)&&(pti>0.)){
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
  if(ptmax > 0.){
    //dynamic_cast<TH2F*>(fListHistos->FindObject("NeutralRatioAll"))->Fill(ptg,neutral.Pt()/ptg);
    //dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiNeutral"))->Fill(ptg,neutral.Phi()-phig);
    Info("GetLeadingNeutral","type %d, E  %f", type,neutral.Pt());
  }
}

//____________________________________________________________________________
void AliPHOSGammaJet::GetAnyLeading(TList &particleList, TLorentzVector &any, Int_t & id,
					TLorentzVector & gamma) 
{
  TParticle *particle = 0x0;
  
 
  Double_t ptmax = 0., pti = 0., phi = 0.;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();

  Int_t iPrimary=-1, type = -1;
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    pti = particle->Pt(); 
   
    if((ksCode == 1)&&(id != iPrimary)){
      //cout<<pti<<"  "<<ptmax<<endl;   
      if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	phi = particle->Phi();
	if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut))
	{
	  ptmax = pti;
	  type = particle->GetPdgCode();
	  particle->Momentum(any);
	  }//phi cut  
      }//ptmax 
    }// kscode == 1
  }// while
  if(ptmax > 0.){
    dynamic_cast<TH2F*>(fListHistos->FindObject("AnyRatio"))->Fill(ptg,any.Pt()/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiAny"))->Fill(ptg,any.Phi()-phig);
    //Info("GetAnyLeading","type %d, E  %f", type,neutral.Pt());
  }
  //cout<<">>>>>>>>>>>>>>>>>ptmax   "<<ptmax<< " any.pt  "<<any.Pt()<<endl;
}
//____________________________________________________________________________
void AliPHOSGammaJet::GetLeadingPi0(TList &particleList, TLorentzVector &pi0, Int_t & id, 
				    TLorentzVector & gamma) 
{
  TParticle *particle = 0x0;
  
  Double_t ptmax = 0., pti = 0., phi = 0., arg = 0;
  Double_t ptg = gamma.Pt();
  Double_t phig = gamma.Phi();
  const Float_t mpi0 = 0.1349766;
  Int_t iPrimary=-1;
  if(!fOptFast){
  TIter next(&particleList);
  while ( (particle = (TParticle*)next()) ) {
    iPrimary++;  
    Int_t ksCode = particle->GetStatusCode();
    
    //if((ksCode == 2))
    if((ksCode == 1)&&(id != iPrimary)&&(particle->GetPdgCode() == 111))
      {
	pti = particle->Pt();
	if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
	  phi = particle->Phi();
	  if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
	    ptmax = pti;
	    particle->Momentum(pi0);
	  }// phi cut
	}// pt max
      }// kscode == 1
  }// while
  }//!optfast
  else{
    TLorentzVector gammai,gammaj;
    Double_t angle = 0., e = 0., invmass = 0., anglecutmax = 0.0, anglecutmin = 0.0;
    TIter next(&particleList);
    while ( (particle = (TParticle*)next()) ) {
      iPrimary++;	  
      Int_t ksCode = particle->GetStatusCode();
      Int_t ksPdg = particle->GetPdgCode();
      if((ksCode == 1) && (iPrimary != id) && (ksPdg == 22)){
	particle->Momentum(gammai);
	
	Int_t jPrimary=-1;
	TIter next2(&particleList);
	while ( (particle = (TParticle*)next2()) ) {
	  jPrimary++;
	  if(jPrimary>iPrimary){
	    ksCode = particle->GetStatusCode();
	    ksPdg = particle->GetPdgCode();
	    if((ksCode == 1) && (iPrimary != id) && (ksPdg == 22)){
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
	    }//(ksCode == 1)
	  }
	}//while
	//	    cout<<"jPrimary "<<jPrimary<<endl;
      }// if kscode 1
    }//while		 
  }//foptfast
  if(ptmax != 0.0){
    dynamic_cast<TH2F*>(fListHistos->FindObject("InvMassPair")) ->Fill(ptg,pi0.M());
    dynamic_cast<TH2F*>(fListHistos->FindObject("Pi0RatioAll"))->Fill(ptg,pi0.Pt()/ptg);
    dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiPi0All"))->Fill(ptg,pi0.Phi()-phig);
  }
}

//____________________________________________________________________________
// void AliPHOSGammaJet::GetLeadingPi0(TList &particleList, TLorentzVector &pi0, Int_t & id, 
// 				    TLorentzVector & gamma) 
// {
//   TParticle *particle = 0x0;
  
//   Double_t ptmax = 0., pti = 0., phi = 0.;
//   Double_t ptg = gamma.Pt();
//   Double_t phig = gamma.Phi();
//   Int_t iPrimary=-1;
//   TIter next(&particleList);

//   if(!fOptFast){
//     while ( (particle = (TParticle*)next()) ) {
//       iPrimary++;  
//       Int_t ksCode = particle->GetStatusCode();
      
//       //if((ksCode == 2))
//       if((ksCode == 1)&&(id != iPrimary)&&(particle->GetPdgCode() == 111))
// 	{
// 	  pti = particle->Pt();
// 	  if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
// 	    phi = particle->Phi();
// 	    if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
// 	      ptmax = pti;
// 	      particle->Momentum(pi0);
// 	    }// phi cut
// 	  }// pt max
// 	}// kscode == 1
//     }// while
//   }//!foptfast
//   else {
//     TLorentzVector gammai;
//     while ( (particle = (TParticle*)next()) ) {
//       iPrimary++;  
//       Int_t ksCode = particle->GetStatusCode();
      
//       //if((ksCode == 2))
//       if((ksCode == 1)&&(id != iPrimary)&&(particle->GetPdgCode() == 22))
// 	{
// 	  GetPi0FromGamma(particleList, pi0, id, iPrimary,particle);
// 	  pti = pi0.Pt();
// 	  if((pti> ptmax)&&(pti/ptg>fRatioMinCut)&&(pti/ptg<fRatioMaxCut)){
// 	    phi = particle->Phi();
// 	    if(((phi-phig)>fPhiMinCut)&&((phi-phig)<fPhiMaxCut)){
// 	      ptmax = pti;
// 	      particle->Momentum(pi0);
// 	    }// phi
// 	  }//ptmax
// 	}// kscode == 1
//     }// while
//   }//foptfast

//   if(ptmax != 0.0){
//     dynamic_cast<TH2F*>(fListHistos->FindObject("InvMassPair")) ->Fill(ptg,pi0.M());
//     dynamic_cast<TH2F*>(fListHistos->FindObject("Pi0RatioAll"))->Fill(ptg,pi0.Pt()/ptg);
//     dynamic_cast<TH2F*>(fListHistos->FindObject("DeltaPhiPi0All"))->Fill(ptg,pi0.Phi()-phig);
//   }
// }


//____________________________________________________________________________
// void AliPHOSGammaJet::GetPi0FromGamma(TList &particleList, TLorentzVector &pi0, const Int_t id, 
// 				      const Int_t iPrimary, TParticle * particle) 
// { 
//   const Double_t mpi0 = 0.1349766;
//   Double_t arg = 0., angle = 0., anglecutmax = 0., anglecutmin = 0., invmass = 0., e = 0.;
//   TLorentzVector gammaj, gammai;
//   Int_t jPrimary=-1;

//   particle->Momentum(gammai);

//   TIter next2(&particleList);
//   while ( (particle = (TParticle*)next2()) ) {
//     jPrimary++;
//     if(jPrimary>iPrimary){
//       Int_t ksCode = particle->GetStatusCode();
//       Int_t ksPdg = particle->GetPdgCode();
//       if((ksCode == 1) && (iPrimary != id) && (ksPdg == 22)){
// 	particle->Momentum(gammaj);
// 	//Info("GetLeadingPi0","Egammai %f, Egammaj %f", gammai.Pt(),gammaj.Pt());
// 	angle = gammaj.Angle(gammai.Vect());
// 	//Info("GetLeadingPi0","Angle %f", angle);
// 	e = (gammai+gammaj).E();
// 	anglecutmax = fAngleMaxParam.At(0)*TMath::Exp(fAngleMaxParam.At(1)*e)
// 	  +fAngleMaxParam.At(2)+fAngleMaxParam.At(3)*e;
// 	//Info("GetLeadingPi0","Angle %f, max %f", angle,anglecutmax);
// 	arg = (e*e-2*mpi0*mpi0)/(e*e);
// 	//Info("GetLeadingPi0","Angle %f, cos min %f", angle,arg);
// 	if(arg>0.)
// 	  anglecutmin = TMath::ACos(arg);
// 	else
// 	  anglecutmin= 100.;
// 	//Info("GetLeadingPi0","Angle %f, min %f", angle,anglecutmin);
// 	dynamic_cast<TH2F*>(fListHistos->FindObject("AngleAllPair")) ->Fill(e,angle);
// 	if((angle<anglecutmax)&&(angle>=anglecutmin)){
// 	  dynamic_cast<TH2F*>(fListHistos->FindObject("AngleAllPairCut")) ->Fill(e,angle);
// 	  invmass = (gammai+gammaj).M();
// 	  //Info("GetLeadingPi0","InvMass %f", invmass);
// 	  if((invmass>fInvMassMinCut) && (invmass<fInvMassMaxCut)){ 
// 	    pi0 = gammai+gammaj;
// 	  }//(invmass>0.125) && (invmass<0.145)
// 	}//gammaj.Angle(gammai.Vect())<0.04
//       }//(ksCode == 1)
//     }
//   }//while
// }
//____________________________________________________________________________
void AliPHOSGammaJet::InitParameters()
{
  fAngleMaxParam.Set(4) ;
  fAngleMaxParam.AddAt(0.4,0);//={0.4,-0.25,0.025,-2e-4};
  fAngleMaxParam.AddAt(-0.25,1);
  fAngleMaxParam.AddAt(0.025,2);
  fAngleMaxParam.AddAt(-2e-4,3);
  fOutputFileName = "GammaJet.root";
  fScalarProductCut=0.95;
  fEtaCut         = 0.7 ;
  fPhiMaxCut      = 3.4 ;
  fPhiMinCut      = 2.9 ;
  fInvMassMaxCut  = 0.14 ;
  fInvMassMinCut  = 0.13 ;
  fRatioMaxCut    = 1.0;
  fRatioMinCut    = 0.2;
  fPtEffCut       = 0.15;
  fResPara1       = 0.0255 ;    // GeV
  fResPara2       = 0.0272 ; 
  fResPara3       = 0.0129 ; 
  
  fPosParaA      = 0.096 ;    // cm
  fPosParaB      = 0.229 ;  
 
  fOptFast        = kFALSE;
 
  fMatrixParam = new TMatrix(4,3) ;

  (*fMatrixParam)(0,0) = 0.950;(*fMatrixParam)(0,1) = 0.947;(*fMatrixParam)(0,2) = 0.937;
  (*fMatrixParam)(1,0) = 0.965;(*fMatrixParam)(1,1) = 0.960;(*fMatrixParam)(1,2) = 0.946;
  (*fMatrixParam)(2,0) = 0.974;(*fMatrixParam)(2,1) = 0.968;(*fMatrixParam)(2,2) = 0.951;
  (*fMatrixParam)(3,0) = 0.981;(*fMatrixParam)(3,1) = 0.974;(*fMatrixParam)(3,2) = 0.954;
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
    ("PtChargePi0Leading","Pt_{#leading charge} vs Pt_{pi0}",100,0,100,100,0,100); 
  hPtChargePi0Leading->SetYTitle("Pt_{charge}");
  hPtChargePi0Leading->SetXTitle("Pt_{pi0} (GeV/c)");
  fListHistos->Add(hPtChargePi0Leading) ; 
  
  TH2F * hPtChargePi0LeadingBefore  = new TH2F
    ("PtChargePi0LeadingBefore","Pt_{#leading charge} vs Pt_{pi0}",100,0,100,100,0,100); 
  hPtChargePi0LeadingBefore->SetYTitle("Pt_{charge}");
  hPtChargePi0LeadingBefore->SetXTitle("Pt_{pi0} (GeV/c)");
  fListHistos->Add(hPtChargePi0LeadingBefore) ;

  TH2F * hPtGammaLeading  = new TH2F
    ("PtGammaLeading","Pt_{#leading} vs Pt_{#gamma}",100,0,100,100,0,100); 
  hPtGammaLeading->SetYTitle("Pt_{lead}");
  hPtGammaLeading->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeading) ; 
  
  TH2F * hEtaPhiGammaNoLeading  = new TH2F
    ("EtaPhiGammaNoLeading","#eta_{#gamma} vs #phi_{#gamma}",100,-1.3,1.3,100,-4,0); 
  hEtaPhiGammaNoLeading->SetXTitle("#eta");
  hEtaPhiGammaNoLeading->SetYTitle("#phi");
  fListHistos->Add(hEtaPhiGammaNoLeading) ; 

  TH2F * hPtGammaLeadingCharge  = new TH2F
    ("PtGammaLeadingCharge","Pt_{#lead charge} vs Pt_{#gamma}",100,0,100,100,0,100); 
  hPtGammaLeadingCharge->SetYTitle("Pt_{lead charge}");
  hPtGammaLeadingCharge->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingCharge) ; 

  TH2F * hPtGammaLeadingPi0  = new TH2F
    ("PtGammaLeadingPi0","Pt_{#lead pi0} vs Pt_{#gamma}",100,0,100,100,0,100); 
  hPtGammaLeadingPi0->SetYTitle("Pt_{lead pi0}");
  hPtGammaLeadingPi0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingPi0) ;
 
 TH2F * hPtGammaLeadingNeutral  = new TH2F
    ("PtGammaLeadingNeutral","Pt_{#lead neutral} vs Pt_{#gamma}",100,0,100,100,0,100); 
  hPtGammaLeadingNeutral->SetYTitle("Pt_{lead neutral}");
  hPtGammaLeadingNeutral->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPtGammaLeadingNeutral) ;

  //Histos ratio charged leading pt / gamma pt vs pt 

  TH2F * hChargeRatio  = new TH2F
    ("ChargeRatio","Pt_{leading charge} /Pt_{#gamma} vs Pt_{#gamma}",100,0,100,100,0,1); 
  hChargeRatio->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hChargeRatio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hChargeRatio) ; 

  TH2F * hAnyRatio  = new TH2F
    ("AnyRatio","Pt_{leading} /Pt_{#gamma} vs Pt_{#gamma}",100,0,100,100,0,1); 
  hAnyRatio->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hAnyRatio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hAnyRatio) ; 

  TH2F * hPi0Ratio  = new TH2F
    ("Pi0Ratio","Pt_{leading  #pi^{0}} /Pt_{#gamma} vs Pt_{#gamma}",100,0,100,100,0,1); 
  hPi0Ratio->SetYTitle("Pt_{lead  #pi^{0}} /Pt_{#gamma}");
  hPi0Ratio->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPi0Ratio) ;
 
  //Before chosing the leading 

  TH2F * hChargeRatioAll  = new TH2F
    ("ChargeRatioAll","Pt_{leading charge} /Pt_{#gamma} vs Pt_{#gamma} No Cut",100,0,100,100,0,1); 
  hChargeRatioAll->SetYTitle("Pt_{lead charge} /Pt_{#gamma}");
  hChargeRatioAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hChargeRatioAll) ;

  TH2F * hPi0RatioAll  = new TH2F
    ("Pi0RatioAll","Pt_{leading  #pi^{0}} /Pt_{#gamma} vs Pt_{#gamma} No Cut",100,0,100,100,0,1); 
  hPi0RatioAll->SetYTitle("Pt_{lead  #pi^{0}} /Pt_{#gamma}");
  hPi0RatioAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hPi0RatioAll) ; 


  TH2F * hDeltaPhiChargeAll  = new TH2F
    ("DeltaPhiChargeAll","#phi_{#gamma} - #phi_{charge} vs Pt_{#gamma}",200,0,100,200,0,6.4); 
  hDeltaPhiChargeAll->SetYTitle("#Delta #phi");
  hDeltaPhiChargeAll->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiChargeAll) ; 
		 
  TH2F * hDeltaPhiPi0All  = new TH2F
    ("DeltaPhiPi0All","#phi_{#gamma} - #phi_{ #pi^{0}} vs Pt_{#gamma}",200,0,100,200,0,6.4); 
  hDeltaPhiPi0All->SetYTitle("#Delta #phi");
  hDeltaPhiPi0All->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiPi0All) ; 

  TH2F * hDeltaPhiAny  = new TH2F
    ("DeltaPhiAny","#phi_{#gamma} - #phi_{any} vs Pt_{#gamma}",200,0,100,200,0,6.4); 
  hDeltaPhiAny->SetYTitle("#Delta #phi");
  hDeltaPhiAny->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiAny) ; 

  TH2F * hDeltaPhiCharge  = new TH2F
    ("DeltaPhiCharge","#phi_{#gamma} - #phi_{charge} vs Pt_{#gamma}",200,0,100,200,0,6.4); 
  hDeltaPhiCharge->SetYTitle("#Delta #phi");
  hDeltaPhiCharge->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiCharge) ; 
		 
  TH2F * hDeltaPhiPi0  = new TH2F
    ("DeltaPhiPi0","#phi_{#gamma} - #phi_{ #pi^{0}} vs Pt_{#gamma}",200,0,100,200,0,6.4); 
  hDeltaPhiPi0->SetYTitle("#Delta #phi");
  hDeltaPhiPi0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hDeltaPhiPi0) ; 

  if(fOptFast){
    TH2F * hPtmodPtnomod  = new TH2F
      ("PtmodPtnomod","Ptmod(Ptnomod)",200,0,100,200,0,100); 
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
      ("AngleAllPair","Angle between all #gamma pair vs Pt_{ #pi^{0}}",100,0,50,200,0,0.2); 
    hAngleAllPair->SetYTitle("Angle (rad)");
    hAngleAllPair->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    fListHistos->Add(hAngleAllPair) ; 

    TH2F * hAngleAllPairCut  = new TH2F
      ("AngleAllPairCut","Angle between all #gamma pair cut vs Pt_{ #pi^{0}}",100,0,50,200,0,0.2); 
    hAngleAllPairCut->SetYTitle("Angle (rad)");
    hAngleAllPairCut->SetXTitle("E_{ #pi^{0}} (GeV/c)");
    fListHistos->Add(hAngleAllPairCut) ; 

  }

  TH2F * hInvMassPair  = new TH2F
    ("InvMassPair","Invariant Mass of #gamma pair vs Pt_{#gamma}",100,0,100,100,0.1,0.16); 
  hInvMassPair->SetYTitle("Invariant Mass (GeV/c^{2})");
  hInvMassPair->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hInvMassPair) ; 

  //Jet ratio

  TH2F * hJetPi0RatioCone06Pt0  = new TH2F
    ("JetPi0RatioCone06Pt0","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone06Pt0->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone06Pt0) ; 
 
  TH2F * hJetPi0RatioCone05Pt0  = new TH2F
    ("JetPi0RatioCone05Pt0","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0., pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone05Pt0->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone05Pt0) ; 
  
  TH2F * hJetPi0RatioCone04Pt0  = new TH2F
    ("JetPi0RatioCone04Pt0","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone04Pt0->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone04Pt0) ; 
  
  TH2F * hJetPi0RatioCone03Pt0  = new TH2F
    ("JetPi0RatioCone03Pt0","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone03Pt0->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone03Pt0) ; 


  TH2F * hJetPi0RatioCone06Pt05  = new TH2F
    ("JetPi0RatioCone06Pt05","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone06Pt05->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone06Pt05) ; 
 
  TH2F * hJetPi0RatioCone05Pt05  = new TH2F
    ("JetPi0RatioCone05Pt05","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone05Pt05->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone05Pt05) ; 
  
  TH2F * hJetPi0RatioCone04Pt05  = new TH2F
    ("JetPi0RatioCone04Pt05","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone04Pt05->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone04Pt05) ; 
  
  TH2F * hJetPi0RatioCone03Pt05  = new TH2F
    ("JetPi0RatioCone03Pt05","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone03Pt05->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone03Pt05) ; 

  TH2F * hJetPi0RatioCone06Pt1  = new TH2F
    ("JetPi0RatioCone06Pt1","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone06Pt1->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone06Pt1) ; 
 
  TH2F * hJetPi0RatioCone05Pt1  = new TH2F
    ("JetPi0RatioCone05Pt1","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone05Pt1->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone05Pt1) ; 
 
  TH2F * hJetPi0RatioCone04Pt1  = new TH2F
    ("JetPi0RatioCone04Pt1","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone04Pt1->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone04Pt1) ; 
  
  TH2F * hJetPi0RatioCone03Pt1  = new TH2F
    ("JetPi0RatioCone03Pt1","Pt_{jet lead #pi^{0}}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetPi0RatioCone03Pt1->SetYTitle("Pt_{jet lead #pi^{0}}/Pt_{#gamma}");
  hJetPi0RatioCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0RatioCone03Pt1) ;



  TH2F * hJetAnyRatioCone06Pt0  = new TH2F
    ("JetAnyRatioCone06Pt0","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone06Pt0->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone06Pt0) ; 
 
  TH2F * hJetAnyRatioCone05Pt0  = new TH2F
    ("JetAnyRatioCone05Pt0","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0., pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone05Pt0->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone05Pt0) ; 
  
  TH2F * hJetAnyRatioCone04Pt0  = new TH2F
    ("JetAnyRatioCone04Pt0","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone04Pt0->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone04Pt0) ; 
  
  TH2F * hJetAnyRatioCone03Pt0  = new TH2F
    ("JetAnyRatioCone03Pt0","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone03Pt0->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone03Pt0) ; 


  TH2F * hJetAnyRatioCone06Pt05  = new TH2F
    ("JetAnyRatioCone06Pt05","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone06Pt05->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone06Pt05) ; 
 
  TH2F * hJetAnyRatioCone05Pt05  = new TH2F
    ("JetAnyRatioCone05Pt05","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone05Pt05->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone05Pt05) ; 
  
  TH2F * hJetAnyRatioCone04Pt05  = new TH2F
    ("JetAnyRatioCone04Pt05","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone04Pt05->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone04Pt05) ; 
  
  TH2F * hJetAnyRatioCone03Pt05  = new TH2F
    ("JetAnyRatioCone03Pt05","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone03Pt05->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone03Pt05) ; 

  TH2F * hJetAnyRatioCone06Pt1  = new TH2F
    ("JetAnyRatioCone06Pt1","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone06Pt1->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone06Pt1) ; 
 
  TH2F * hJetAnyRatioCone05Pt1  = new TH2F
    ("JetAnyRatioCone05Pt1","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone05Pt1->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone05Pt1) ; 
 
  TH2F * hJetAnyRatioCone04Pt1  = new TH2F
    ("JetAnyRatioCone04Pt1","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone04Pt1->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone04Pt1) ; 
  
  TH2F * hJetAnyRatioCone03Pt1  = new TH2F
    ("JetAnyRatioCone03Pt1","Pt_{jet any lead}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetAnyRatioCone03Pt1->SetYTitle("Pt_{jet any lead}/Pt_{#gamma}");
  hJetAnyRatioCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyRatioCone03Pt1) ;


  TH2F * hJetChargeRatioCone06Pt0  = new TH2F
    ("JetChargeRatioCone06Pt0","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone06Pt0->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone06Pt0) ; 
 
  TH2F * hJetChargeRatioCone05Pt0  = new TH2F
    ("JetChargeRatioCone05Pt0","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0., pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone05Pt0->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone05Pt0) ; 
 
  TH2F * hJetChargeRatioCone04Pt0  = new TH2F
    ("JetChargeRatioCone04Pt0","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone04Pt0->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone04Pt0) ; 
  
  TH2F * hJetChargeRatioCone03Pt0  = new TH2F
    ("JetChargeRatioCone03Pt0","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0. GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone03Pt0->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone03Pt0) ; 


  TH2F * hJetChargeRatioCone06Pt05  = new TH2F
    ("JetChargeRatioCone06Pt05","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone06Pt05->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone06Pt05) ; 
 
  TH2F * hJetChargeRatioCone05Pt05  = new TH2F
    ("JetChargeRatioCone05Pt05","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone05Pt05->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone05Pt05) ; 
 
  TH2F * hJetChargeRatioCone04Pt05  = new TH2F
    ("JetChargeRatioCone04Pt05","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone04Pt05->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone04Pt05) ; 
  
  TH2F * hJetChargeRatioCone03Pt05  = new TH2F
    ("JetChargeRatioCone03Pt05","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone03Pt05->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone03Pt05) ; 

  TH2F * hJetChargeRatioCone06Pt1  = new TH2F
    ("JetChargeRatioCone06Pt1","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone06Pt1->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone06Pt1) ; 
 
  TH2F * hJetChargeRatioCone05Pt1  = new TH2F
    ("JetChargeRatioCone05Pt1","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone05Pt1->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone05Pt1) ; 
 
  TH2F * hJetChargeRatioCone04Pt1  = new TH2F
    ("JetChargeRatioCone04Pt1","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone04Pt1->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone04Pt1) ; 
  
  TH2F * hJetChargeRatioCone03Pt1  = new TH2F
    ("JetChargeRatioCone03Pt1","Pt_{jet lead charge}/Pt_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",100,0,100,100,0,2); 
  hJetChargeRatioCone03Pt1->SetYTitle("Pt_{jet}/Pt_{#gamma}");
  hJetChargeRatioCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargeRatioCone03Pt1) ;

  //Jet delta phi

  TH2F * hJetPi0PhiCone06Pt0  = new TH2F
    ("JetPi0PhiCone06Pt0","#phi_{jet lead pi0}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone06Pt0->SetYTitle("#Delta #phi_{jet lead pi0}");
  hJetPi0PhiCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone06Pt0) ; 
 
  TH2F * hJetPi0PhiCone05Pt0  = new TH2F
    ("JetPi0PhiCone05Pt0","#phi_{jet lead pi0}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone05Pt0->SetYTitle("#Delta #phi_{jet lead pi0}");
  hJetPi0PhiCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone05Pt0) ; 
 
  TH2F * hJetPi0PhiCone04Pt0  = new TH2F
    ("JetPi0PhiCone04Pt0","#phi_{jet lead pi0}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone04Pt0->SetYTitle("#Delta #phi_{jet lead pi0}");
  hJetPi0PhiCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone04Pt0) ; 
  
  TH2F * hJetPi0PhiCone03Pt0  = new TH2F
    ("JetPi0PhiCone03Pt0","#phi_{jet lead pi0}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone03Pt0->SetYTitle("#Delta #phi_{jet lead pi0}");
  hJetPi0PhiCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone03Pt0) ;

  TH2F * hJetPi0PhiCone06Pt05  = new TH2F
    ("JetPi0PhiCone06Pt05","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone06Pt05->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone06Pt05) ; 
 
  TH2F * hJetPi0PhiCone05Pt05  = new TH2F
    ("JetPi0PhiCone05Pt05","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone05Pt05->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone05Pt05) ; 
 
  TH2F * hJetPi0PhiCone04Pt05  = new TH2F
    ("JetPi0PhiCone04Pt05","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone04Pt05->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone04Pt05) ; 
  
  TH2F * hJetPi0PhiCone03Pt05  = new TH2F
    ("JetPi0PhiCone03Pt05","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone03Pt05->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone03Pt05) ; 

  TH2F * hJetPi0PhiCone06Pt1  = new TH2F
    ("JetPi0PhiCone06Pt1","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone06Pt1->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone06Pt1) ; 
 
  TH2F * hJetPi0PhiCone05Pt1  = new TH2F
    ("JetPi0PhiCone05Pt1","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone05Pt1->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone05Pt1) ; 
 
  TH2F * hJetPi0PhiCone04Pt1  = new TH2F
    ("JetPi0PhiCone04Pt1","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone04Pt1->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone04Pt1) ; 
  
  TH2F * hJetPi0PhiCone03Pt1  = new TH2F
    ("JetPi0PhiCone03Pt1","#phi_{jet lead #pi^{0}}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetPi0PhiCone03Pt1->SetYTitle("#Delta #phi_{jet lead #pi^{0}}");
  hJetPi0PhiCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetPi0PhiCone03Pt1) ;

  TH2F * hJetChargePhiCone06Pt0  = new TH2F
    ("JetChargePhiCone06Pt0","#phi_{jet lead ch}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone06Pt0->SetYTitle("#Delta #phi_{jet lead ch}");
  hJetChargePhiCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone06Pt0) ; 
 
  TH2F * hJetChargePhiCone05Pt0  = new TH2F
    ("JetChargePhiCone05Pt0","#phi_{jet lead ch}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone05Pt0->SetYTitle("#Delta #phi_{jet lead ch}");
  hJetChargePhiCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone05Pt0) ; 
 
  TH2F * hJetChargePhiCone04Pt0  = new TH2F
    ("JetChargePhiCone04Pt0","#phi_{jet lead ch}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone04Pt0->SetYTitle("#Delta #phi_{jet lead ch}");
  hJetChargePhiCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone04Pt0) ; 
  
  TH2F * hJetChargePhiCone03Pt0  = new TH2F
    ("JetChargePhiCone03Pt0","#phi_{jet lead ch}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone03Pt0->SetYTitle("#Delta #phi_{jet lead ch}");
  hJetChargePhiCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone03Pt0) ;

  TH2F * hJetChargePhiCone06Pt05  = new TH2F
    ("JetChargePhiCone06Pt05","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone06Pt05->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone06Pt05) ; 
 
  TH2F * hJetChargePhiCone05Pt05  = new TH2F
    ("JetChargePhiCone05Pt05","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone05Pt05->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone05Pt05) ; 
 
  TH2F * hJetChargePhiCone04Pt05  = new TH2F
    ("JetChargePhiCone04Pt05","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone04Pt05->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone04Pt05) ; 
  
  TH2F * hJetChargePhiCone03Pt05  = new TH2F
    ("JetChargePhiCone03Pt05","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone03Pt05->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone03Pt05) ; 

  TH2F * hJetChargePhiCone06Pt1  = new TH2F
    ("JetChargePhiCone06Pt1","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone06Pt1->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone06Pt1) ; 
 
  TH2F * hJetChargePhiCone05Pt1  = new TH2F
    ("JetChargePhiCone05Pt1","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone05Pt1->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone05Pt1) ; 
 
  TH2F * hJetChargePhiCone04Pt1  = new TH2F
    ("JetChargePhiCone04Pt1","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone04Pt1->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone04Pt1) ; 
  
  TH2F * hJetChargePhiCone03Pt1  = new TH2F
    ("JetChargePhiCone03Pt1","#phi_{jet lead charge}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetChargePhiCone03Pt1->SetYTitle("#Delta #phi_{jet lead charge}");
  hJetChargePhiCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetChargePhiCone03Pt1) ;



  TH2F * hJetAnyPhiCone06Pt05  = new TH2F
    ("JetAnyPhiCone06Pt05","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone06Pt05->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone06Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone06Pt05) ; 
 
  TH2F * hJetAnyPhiCone05Pt05  = new TH2F
    ("JetAnyPhiCone05Pt05","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone05Pt05->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone05Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone05Pt05) ; 
 
  TH2F * hJetAnyPhiCone04Pt05  = new TH2F
    ("JetAnyPhiCone04Pt05","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone04Pt05->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone04Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone04Pt05) ; 
  
  TH2F * hJetAnyPhiCone03Pt05  = new TH2F
    ("JetAnyPhiCone03Pt05","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone03Pt05->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone03Pt05->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone03Pt05) ; 

  TH2F * hJetAnyPhiCone06Pt0  = new TH2F
    ("JetAnyPhiCone06Pt0","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone06Pt0->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone06Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone06Pt0) ; 
 
  TH2F * hJetAnyPhiCone05Pt0  = new TH2F
    ("JetAnyPhiCone05Pt0","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone05Pt0->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone05Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone05Pt0) ; 
 
  TH2F * hJetAnyPhiCone04Pt0  = new TH2F
    ("JetAnyPhiCone04Pt0","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone04Pt0->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone04Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone04Pt0) ; 
  
  TH2F * hJetAnyPhiCone03Pt0  = new TH2F
    ("JetAnyPhiCone03Pt0","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 0.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone03Pt0->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone03Pt0->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone03Pt0) ;


  TH2F * hJetAnyPhiCone06Pt1  = new TH2F
    ("JetAnyPhiCone06Pt1","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone06Pt1->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone06Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone06Pt1) ; 
 
  TH2F * hJetAnyPhiCone05Pt1  = new TH2F
    ("JetAnyPhiCone05Pt1","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone05Pt1->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone05Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone05Pt1) ; 
 
  TH2F * hJetAnyPhiCone04Pt1  = new TH2F
    ("JetAnyPhiCone04Pt1","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone04Pt1->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone04Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone04Pt1) ; 
  
  TH2F * hJetAnyPhiCone03Pt1  = new TH2F
    ("JetAnyPhiCone03Pt1","#phi_{jet lead any}/#phi_{#gamma} vs Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",200,0,100,200,0,6.4); 
  hJetAnyPhiCone03Pt1->SetYTitle("#Delta #phi_{jet lead any}");
  hJetAnyPhiCone03Pt1->SetXTitle("Pt_{#gamma} (GeV/c)");
  fListHistos->Add(hJetAnyPhiCone03Pt1) ;



  //Count

  TH1F * hNGamma  = new TH1F
    ("NGamma","Number of #gamma over PHOS",100,0,100); 
  hNGamma->SetYTitle("N");
  hNGamma->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNGamma->Sumw2();
  fListHistos->Add(hNGamma) ; 


  TH1F * hNChargeJetCone06Pt0  = new TH1F
    ("NChargeJetCone06Pt0","Number of jets, cone = 0.6, pt> 0.  GeV/c",100,0,100); 
  hNChargeJetCone06Pt0->SetYTitle("N");
  hNChargeJetCone06Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone06Pt0->Sumw2();
  fListHistos->Add(hNChargeJetCone06Pt0) ; 
 
  TH1F * hNChargeJetCone05Pt0  = new TH1F
    ("NChargeJetCone05Pt0","Number of jets, cone = 0.5, pt> 0.  GeV/c",100,0,100); 
  hNChargeJetCone05Pt0->SetYTitle("N");
  hNChargeJetCone05Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone05Pt0->Sumw2();
  fListHistos->Add(hNChargeJetCone05Pt0) ; 
 
  TH1F * hNChargeJetCone04Pt0  = new TH1F
    ("NChargeJetCone04Pt0","Number of jets, cone = 0.4, pt> 0.  GeV/c",100,0,100); 
  hNChargeJetCone04Pt0->SetYTitle("N");
  hNChargeJetCone04Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
hNChargeJetCone04Pt0->Sumw2();
  fListHistos->Add(hNChargeJetCone04Pt0) ; 
  
  TH1F * hNChargeJetCone03Pt0  = new TH1F
    ("NChargeJetCone03Pt0","Number of jets, cone = 0.3, pt> 0.  GeV/c",100,0,100); 
  hNChargeJetCone03Pt0->SetYTitle("N");
  hNChargeJetCone03Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone03Pt0->Sumw2();
  fListHistos->Add(hNChargeJetCone03Pt0) ; 



  TH1F * hNChargeJetCone06Pt05  = new TH1F
    ("NChargeJetCone06Pt05","Number of jets, cone = 0.6, pt> 0.5 GeV/c",100,0,100); 
  hNChargeJetCone06Pt05->SetYTitle("N");
  hNChargeJetCone06Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone06Pt05->Sumw2();
  fListHistos->Add(hNChargeJetCone06Pt05) ; 
 
  TH1F * hNChargeJetCone05Pt05  = new TH1F
    ("NChargeJetCone05Pt05","Number of jets, cone = 0.5, pt> 0.5 GeV/c",100,0,100); 
  hNChargeJetCone05Pt05->SetYTitle("N");
  hNChargeJetCone05Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone05Pt05->Sumw2();
  fListHistos->Add(hNChargeJetCone05Pt05) ; 
 
  TH1F * hNChargeJetCone04Pt05  = new TH1F
    ("NChargeJetCone04Pt05","Number of jets, cone = 0.4, pt> 0.5 GeV/c",100,0,100); 
  hNChargeJetCone04Pt05->SetYTitle("N");
  hNChargeJetCone04Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
hNChargeJetCone04Pt05->Sumw2();
  fListHistos->Add(hNChargeJetCone04Pt05) ; 
  
  TH1F * hNChargeJetCone03Pt05  = new TH1F
    ("NChargeJetCone03Pt05","Number of jets, cone = 0.3, pt> 0.5 GeV/c",100,0,100); 
  hNChargeJetCone03Pt05->SetYTitle("N");
  hNChargeJetCone03Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone03Pt05->Sumw2();
  fListHistos->Add(hNChargeJetCone03Pt05) ; 

  TH1F * hNChargeJetCone06Pt1  = new TH1F
    ("NChargeJetCone06Pt1","Number of jets, cone = 0.6, pt> 1.0 GeV/c",100,0,100); 
  hNChargeJetCone06Pt1->SetYTitle("N");
  hNChargeJetCone06Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone06Pt1->Sumw2();
  fListHistos->Add(hNChargeJetCone06Pt1) ; 
 
  TH1F * hNChargeJetCone05Pt1  = new TH1F
    ("NChargeJetCone05Pt1","Number of jets, cone = 0.5, pt> 1.0 GeV/c",100,0,100); 
  hNChargeJetCone05Pt1->SetYTitle("N");
  hNChargeJetCone05Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone05Pt1->Sumw2();
  fListHistos->Add(hNChargeJetCone05Pt1) ; 
 
  TH1F * hNChargeJetCone04Pt1  = new TH1F
    ("NChargeJetCone04Pt1","Number of jets, cone = 0.4, pt> 1.0 GeV/c",100,0,100); 
  hNChargeJetCone04Pt1->SetYTitle("N");
  hNChargeJetCone04Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone04Pt1->Sumw2();
  fListHistos->Add(hNChargeJetCone04Pt1) ; 
  
  TH1F * hNChargeJetCone03Pt1  = new TH1F
    ("NChargeJetCone03Pt1","Number of jets, cone = 0.3, pt> 1.0 GeV/c",100,0,100); 
  hNChargeJetCone03Pt1->SetYTitle("N");
  hNChargeJetCone03Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNChargeJetCone03Pt1->Sumw2();
  fListHistos->Add(hNChargeJetCone03Pt1) ;


  //Neutral

  TH1F * hNPi0JetCone06Pt0  = new TH1F
    ("NPi0JetCone06Pt0","Number of jets, cone = 0.6, pt> 0.  GeV/c",100,0,100); 
  hNPi0JetCone06Pt0->SetYTitle("N");
  hNPi0JetCone06Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone06Pt0->Sumw2();
  fListHistos->Add(hNPi0JetCone06Pt0) ; 
 
  TH1F * hNPi0JetCone05Pt0  = new TH1F
    ("NPi0JetCone05Pt0","Number of jets, cone = 0.5, pt> 0.  GeV/c",100,0,100); 
  hNPi0JetCone05Pt0->SetYTitle("N");
  hNPi0JetCone05Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone05Pt0->Sumw2();
  fListHistos->Add(hNPi0JetCone05Pt0) ; 
 
  TH1F * hNPi0JetCone04Pt0  = new TH1F
    ("NPi0JetCone04Pt0","Number of jets, cone = 0.4, pt> 0.  GeV/c",100,0,100); 
  hNPi0JetCone04Pt0->SetYTitle("N");
  hNPi0JetCone04Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
hNPi0JetCone04Pt0->Sumw2();
  fListHistos->Add(hNPi0JetCone04Pt0) ; 
  
  TH1F * hNPi0JetCone03Pt0  = new TH1F
    ("NPi0JetCone03Pt0","Number of jets, cone = 0.3, pt> 0.  GeV/c",100,0,100); 
  hNPi0JetCone03Pt0->SetYTitle("N");
  hNPi0JetCone03Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone03Pt0->Sumw2();
  fListHistos->Add(hNPi0JetCone03Pt0) ; 


  TH1F * hNPi0JetCone06Pt05  = new TH1F
    ("NPi0JetCone06Pt05","Number of jets, cone = 0.6, pt> 0.5 GeV/c",100,0,100); 
  hNPi0JetCone06Pt05->SetYTitle("N");
  hNPi0JetCone06Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone06Pt05->Sumw2();
  fListHistos->Add(hNPi0JetCone06Pt05) ; 
 
  TH1F * hNPi0JetCone05Pt05  = new TH1F
    ("NPi0JetCone05Pt05","Number of jets, cone = 0.5, pt> 0.5 GeV/c",100,0,100); 
  hNPi0JetCone05Pt05->SetYTitle("N");
  hNPi0JetCone05Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone05Pt05->Sumw2();
  fListHistos->Add(hNPi0JetCone05Pt05) ; 
 
  TH1F * hNPi0JetCone04Pt05  = new TH1F
    ("NPi0JetCone04Pt05","Number of jets, cone = 0.4, pt> 0.5 GeV/c",100,0,100); 
  hNPi0JetCone04Pt05->SetYTitle("N");
  hNPi0JetCone04Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone04Pt05->Sumw2();
  fListHistos->Add(hNPi0JetCone04Pt05) ; 
  
  TH1F * hNPi0JetCone03Pt05  = new TH1F
    ("NPi0JetCone03Pt05","Number of jets, cone = 0.3, pt> 0.5 GeV/c",100,0,100); 
  hNPi0JetCone03Pt05->SetYTitle("N");
  hNPi0JetCone03Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone03Pt05->Sumw2();
  fListHistos->Add(hNPi0JetCone03Pt05) ; 

  TH1F * hNPi0JetCone06Pt1  = new TH1F
    ("NPi0JetCone06Pt1","Number of jets, cone = 0.6, pt> 1.0 GeV/c",100,0,100); 
  hNPi0JetCone06Pt1->SetYTitle("N");
  hNPi0JetCone06Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone06Pt1->Sumw2();
  fListHistos->Add(hNPi0JetCone06Pt1) ; 
 
  TH1F * hNPi0JetCone05Pt1  = new TH1F
    ("NPi0JetCone05Pt1","Number of jets, cone = 0.5, pt> 1.0 GeV/c",100,0,100); 
  hNPi0JetCone05Pt1->SetYTitle("N");
  hNPi0JetCone05Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone05Pt1->Sumw2();
  fListHistos->Add(hNPi0JetCone05Pt1) ; 
 
  TH1F * hNPi0JetCone04Pt1  = new TH1F
    ("NPi0JetCone04Pt1","Number of jets, cone = 0.4, pt> 1.0 GeV/c",100,0,100); 
  hNPi0JetCone04Pt1->SetYTitle("N");
  hNPi0JetCone04Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone04Pt1->Sumw2();
  fListHistos->Add(hNPi0JetCone04Pt1) ; 
  
  TH1F * hNPi0JetCone03Pt1  = new TH1F
    ("NPi0JetCone03Pt1","Number of jets, cone = 0.3, pt> 1.0 GeV/c",100,0,100); 
  hNPi0JetCone03Pt1->SetYTitle("N");
  hNPi0JetCone03Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNPi0JetCone03Pt1->Sumw2();
  fListHistos->Add(hNPi0JetCone03Pt1) ;


  //Any 

  TH1F * hNAnyJetCone06Pt0  = new TH1F
    ("NAnyJetCone06Pt0","Number of jets, cone = 0.6, pt> 0.  GeV/c",100,0,100); 
  hNAnyJetCone06Pt0->SetYTitle("N");
  hNAnyJetCone06Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone06Pt0->Sumw2();
  fListHistos->Add(hNAnyJetCone06Pt0) ; 
 
  TH1F * hNAnyJetCone05Pt0  = new TH1F
    ("NAnyJetCone05Pt0","Number of jets, cone = 0.5, pt> 0.  GeV/c",100,0,100); 
  hNAnyJetCone05Pt0->SetYTitle("N");
  hNAnyJetCone05Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone05Pt0->Sumw2();
  fListHistos->Add(hNAnyJetCone05Pt0) ; 
 
  TH1F * hNAnyJetCone04Pt0  = new TH1F
    ("NAnyJetCone04Pt0","Number of jets, cone = 0.4, pt> 0.  GeV/c",100,0,100); 
  hNAnyJetCone04Pt0->SetYTitle("N");
  hNAnyJetCone04Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
hNAnyJetCone04Pt0->Sumw2();
  fListHistos->Add(hNAnyJetCone04Pt0) ; 
  
  TH1F * hNAnyJetCone03Pt0  = new TH1F
    ("NAnyJetCone03Pt0","Number of jets, cone = 0.3, pt> 0.  GeV/c",100,0,100); 
  hNAnyJetCone03Pt0->SetYTitle("N");
  hNAnyJetCone03Pt0->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone03Pt0->Sumw2();
  fListHistos->Add(hNAnyJetCone03Pt0) ; 


  TH1F * hNAnyJetCone06Pt05  = new TH1F
    ("NAnyJetCone06Pt05","Number of jets, cone = 0.6, pt> 0.5 GeV/c",100,0,100); 
  hNAnyJetCone06Pt05->SetYTitle("N");
  hNAnyJetCone06Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone06Pt05->Sumw2();
  fListHistos->Add(hNAnyJetCone06Pt05) ; 
 
  TH1F * hNAnyJetCone05Pt05  = new TH1F
    ("NAnyJetCone05Pt05","Number of jets, cone = 0.5, pt> 0.5 GeV/c",100,0,100); 
  hNAnyJetCone05Pt05->SetYTitle("N");
  hNAnyJetCone05Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone05Pt05->Sumw2();
  fListHistos->Add(hNAnyJetCone05Pt05) ; 
 
  TH1F * hNAnyJetCone04Pt05  = new TH1F
    ("NAnyJetCone04Pt05","Number of jets, cone = 0.4, pt> 0.5 GeV/c",100,0,100); 
  hNAnyJetCone04Pt05->SetYTitle("N");
  hNAnyJetCone04Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone04Pt05->Sumw2();
  fListHistos->Add(hNAnyJetCone04Pt05) ; 
  
  TH1F * hNAnyJetCone03Pt05  = new TH1F
    ("NAnyJetCone03Pt05","Number of jets, cone = 0.3, pt> 0.5 GeV/c",100,0,100); 
  hNAnyJetCone03Pt05->SetYTitle("N");
  hNAnyJetCone03Pt05->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone03Pt05->Sumw2();
  fListHistos->Add(hNAnyJetCone03Pt05) ; 

  TH1F * hNAnyJetCone06Pt1  = new TH1F
    ("NAnyJetCone06Pt1","Number of jets, cone = 0.6, pt> 1.0 GeV/c",100,0,100); 
  hNAnyJetCone06Pt1->SetYTitle("N");
  hNAnyJetCone06Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone06Pt1->Sumw2();
  fListHistos->Add(hNAnyJetCone06Pt1) ; 
 
  TH1F * hNAnyJetCone05Pt1  = new TH1F
    ("NAnyJetCone05Pt1","Number of jets, cone = 0.5, pt> 1.0 GeV/c",100,0,100); 
  hNAnyJetCone05Pt1->SetYTitle("N");
  hNAnyJetCone05Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone05Pt1->Sumw2();
  fListHistos->Add(hNAnyJetCone05Pt1) ; 
 
  TH1F * hNAnyJetCone04Pt1  = new TH1F
    ("NAnyJetCone04Pt1","Number of jets, cone = 0.4, pt> 1.0 GeV/c",100,0,100); 
  hNAnyJetCone04Pt1->SetYTitle("N");
  hNAnyJetCone04Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone04Pt1->Sumw2();
  fListHistos->Add(hNAnyJetCone04Pt1) ; 
  
  TH1F * hNAnyJetCone03Pt1  = new TH1F
    ("NAnyJetCone03Pt1","Number of jets, cone = 0.3, pt> 1.0 GeV/c",100,0,100); 
  hNAnyJetCone03Pt1->SetYTitle("N");
  hNAnyJetCone03Pt1->SetXTitle("Pt_{#gamma}(GeV/c)");
  hNAnyJetCone03Pt1->Sumw2();
  fListHistos->Add(hNAnyJetCone03Pt1) ;


  //Partition
  TH1F * hJetPartitionCone06Pt05  = new TH1F
    ("JetPartitionCone06Pt05","x = Pt_{i}/Pt_{#gamma}, cone = 0.6, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionCone06Pt05->SetYTitle("dN / dx");
  hJetPartitionCone06Pt05->SetXTitle("x");
  hJetPartitionCone06Pt05->Sumw2();
  fListHistos->Add(hJetPartitionCone06Pt05) ; 
 
  TH1F * hJetPartitionCone05Pt05  = new TH1F
    ("JetPartitionCone05Pt05","x = Pt_{i}/Pt_{#gamma}, cone = 0.5, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionCone05Pt05->SetYTitle("dN / dx");
  hJetPartitionCone05Pt05->SetXTitle("x");
  hJetPartitionCone05Pt05->Sumw2();
  fListHistos->Add(hJetPartitionCone05Pt05) ; 
 
  TH1F * hJetPartitionCone04Pt05  = new TH1F
    ("JetPartitionCone04Pt05","x = Pt_{i}/Pt_{#gamma}, cone = 0.4, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionCone04Pt05->SetYTitle("dN / dx");
  hJetPartitionCone04Pt05->SetXTitle("x");
  hJetPartitionCone04Pt05->Sumw2();
  fListHistos->Add(hJetPartitionCone04Pt05) ; 
  
 TH1F * hJetPartitionCone03Pt05  = new TH1F
    ("JetPartitionCone03Pt05","x = Pt_{i}/Pt_{#gamma}, cone = 0.3, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionCone03Pt05->SetYTitle("dN / dx");
  hJetPartitionCone03Pt05->SetXTitle("x");
  hJetPartitionCone03Pt05->Sumw2();
  fListHistos->Add(hJetPartitionCone03Pt05) ; 


  TH1F * hJetPartitionCone06Pt0  = new TH1F
    ("JetPartitionCone06Pt0","x = Pt_{i}/Pt_{#gamma}, cone = 0.6, pt> 0. GeV/c",100,0,1); 
  hJetPartitionCone06Pt0->SetYTitle("dN / dx");
  hJetPartitionCone06Pt0->SetXTitle("x");
  hJetPartitionCone06Pt0->Sumw2();
  fListHistos->Add(hJetPartitionCone06Pt0) ; 
 
  TH1F * hJetPartitionCone05Pt0  = new TH1F
    ("JetPartitionCone05Pt0","x = Pt_{i}/Pt_{#gamma}, cone = 0.5, pt> 0. GeV/c",100,0,1); 
  hJetPartitionCone05Pt0->SetYTitle("dN / dx");
  hJetPartitionCone05Pt0->SetXTitle("x");
  hJetPartitionCone05Pt0->Sumw2();
  fListHistos->Add(hJetPartitionCone05Pt0) ; 
 
  TH1F * hJetPartitionCone04Pt0  = new TH1F
    ("JetPartitionCone04Pt0","x = Pt_{i}/Pt_{#gamma}, cone = 0.4, pt> 0. GeV/c",100,0,1); 
  hJetPartitionCone04Pt0->SetYTitle("dN / dx");
  hJetPartitionCone04Pt0->SetXTitle("x");
  hJetPartitionCone04Pt0->Sumw2();
  fListHistos->Add(hJetPartitionCone04Pt0) ; 
  
  TH1F * hJetPartitionCone03Pt0  = new TH1F
    ("JetPartitionCone03Pt0","x = Pt_{i}/Pt_{#gamma}, cone = 0.3, pt> 0. GeV/c",100,0,1); 
  hJetPartitionCone03Pt0->SetYTitle("dN / dx");
  hJetPartitionCone03Pt0->SetXTitle("x");
  hJetPartitionCone03Pt0->Sumw2();
  fListHistos->Add(hJetPartitionCone03Pt0) ; 


  TH1F * hJetPartitionCone06Pt1  = new TH1F
    ("JetPartitionCone06Pt1","x = Pt_{i}/Pt_{#gamma}, cone = 0.6, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionCone06Pt1->SetYTitle("dN / dx");
  hJetPartitionCone06Pt1->SetXTitle("x");
  hJetPartitionCone06Pt1->Sumw2();
  fListHistos->Add(hJetPartitionCone06Pt1) ; 
 
  TH1F * hJetPartitionCone05Pt1  = new TH1F
    ("JetPartitionCone05Pt1","x = Pt_{i}/Pt_{#gamma}, cone = 0.5, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionCone05Pt1->SetYTitle("dN / dx");
  hJetPartitionCone05Pt1->SetXTitle("x");
  hJetPartitionCone05Pt1->Sumw2();
  fListHistos->Add(hJetPartitionCone05Pt1) ; 
 
  TH1F * hJetPartitionCone04Pt1  = new TH1F
    ("JetPartitionCone04Pt1","x = Pt_{i}/Pt_{#gamma}, cone = 0.4, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionCone04Pt1->SetYTitle("dN / dx");
  hJetPartitionCone04Pt1->SetXTitle("x");
  hJetPartitionCone04Pt1->Sumw2();
  fListHistos->Add(hJetPartitionCone04Pt1) ; 
  
  TH1F * hJetPartitionCone03Pt1  = new TH1F
    ("JetPartitionCone03Pt1","x = Pt_{i}/Pt_{#gamma}, cone = 0.3, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionCone03Pt1->SetYTitle("dN / dx");
  hJetPartitionCone03Pt1->SetXTitle("x");
  hJetPartitionCone03Pt1->Sumw2();
  fListHistos->Add(hJetPartitionCone03Pt1) ;


  //Partition, only charged
  TH1F * hJetPartitionChCone06Pt05  = new TH1F
    ("JetPartitionChCone06Pt05","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.6, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionChCone06Pt05->SetYTitle("dN / dx");
  hJetPartitionChCone06Pt05->SetXTitle("x");
  hJetPartitionChCone06Pt05->Sumw2();
  fListHistos->Add(hJetPartitionChCone06Pt05) ; 
  
  TH1F * hJetPartitionChCone05Pt05  = new TH1F
    ("JetPartitionChCone05Pt05","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.5, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionChCone05Pt05->SetYTitle("dN / dx");
  hJetPartitionChCone05Pt05->SetXTitle("x");
  hJetPartitionChCone05Pt05->Sumw2();
  fListHistos->Add(hJetPartitionChCone05Pt05) ; 
  
  TH1F * hJetPartitionChCone04Pt05  = new TH1F
    ("JetPartitionChCone04Pt05","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.4, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionChCone04Pt05->SetYTitle("dN / dx");
  hJetPartitionChCone04Pt05->SetXTitle("x");
  hJetPartitionChCone04Pt05->Sumw2();
  fListHistos->Add(hJetPartitionChCone04Pt05) ; 
  
  TH1F * hJetPartitionChCone03Pt05  = new TH1F
    ("JetPartitionChCone03Pt05","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.3, pt> 0.5 GeV/c",100,0,1); 
  hJetPartitionChCone03Pt05->SetYTitle("dN / dx");
  hJetPartitionChCone03Pt05->SetXTitle("x");
  hJetPartitionChCone03Pt05->Sumw2();
  fListHistos->Add(hJetPartitionChCone03Pt05) ; 
  

  TH1F * hJetPartitionChCone06Pt0  = new TH1F
    ("JetPartitionChCone06Pt0","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.6, pt> 0. GeV/c",100,0,1); 
  hJetPartitionChCone06Pt0->SetYTitle("dN / dx");
  hJetPartitionChCone06Pt0->SetXTitle("x");
  hJetPartitionChCone06Pt0->Sumw2();
  fListHistos->Add(hJetPartitionChCone06Pt0) ; 
  
  TH1F * hJetPartitionChCone05Pt0  = new TH1F
    ("JetPartitionChCone05Pt0","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.5, pt> 0. GeV/c",100,0,1); 
  hJetPartitionChCone05Pt0->SetYTitle("dN / dx");
  hJetPartitionChCone05Pt0->SetXTitle("x");
  hJetPartitionChCone05Pt0->Sumw2();
  fListHistos->Add(hJetPartitionChCone05Pt0) ; 
  
  TH1F * hJetPartitionChCone04Pt0  = new TH1F
    ("JetPartitionChCone04Pt0","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.4, pt> 0. GeV/c",100,0,1); 
  hJetPartitionChCone04Pt0->SetYTitle("dN / dx");
  hJetPartitionChCone04Pt0->SetXTitle("x");
  hJetPartitionChCone04Pt0->Sumw2();
  fListHistos->Add(hJetPartitionChCone04Pt0) ; 
  
  TH1F * hJetPartitionChCone03Pt0  = new TH1F
    ("JetPartitionChCone03Pt0","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.3, pt> 0. GeV/c",100,0,1); 
  hJetPartitionChCone03Pt0->SetYTitle("dN / dx");
  hJetPartitionChCone03Pt0->SetXTitle("x");
  hJetPartitionChCone03Pt0->Sumw2();
  fListHistos->Add(hJetPartitionChCone03Pt0) ; 


  TH1F * hJetPartitionChCone06Pt1  = new TH1F
    ("JetPartitionChCone06Pt1","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.6, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionChCone06Pt1->SetYTitle("dN / dx");
  hJetPartitionChCone06Pt1->SetXTitle("x");
  hJetPartitionChCone06Pt1->Sumw2();
  fListHistos->Add(hJetPartitionChCone06Pt1) ; 
  
  TH1F * hJetPartitionChCone05Pt1  = new TH1F
    ("JetPartitionChCone05Pt1","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.5, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionChCone05Pt1->SetYTitle("dN / dx");
  hJetPartitionChCone05Pt1->SetXTitle("x");
  hJetPartitionChCone05Pt1->Sumw2();
  fListHistos->Add(hJetPartitionChCone05Pt1) ; 
 
  TH1F * hJetPartitionChCone04Pt1  = new TH1F
    ("JetPartitionChCone04Pt1","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.4, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionChCone04Pt1->SetYTitle("dN / dx");
  hJetPartitionChCone04Pt1->SetXTitle("x");
  hJetPartitionChCone04Pt1->Sumw2();
  fListHistos->Add(hJetPartitionChCone04Pt1) ; 
  
  TH1F * hJetPartitionChCone03Pt1  = new TH1F
    ("JetPartitionChCone03Pt1","x = Pt_{i}/Pt_{#gamma} charged, cone = 0.3, pt> 1.0 GeV/c",100,0,1); 
  hJetPartitionChCone03Pt1->SetYTitle("dN / dx");
  hJetPartitionChCone03Pt1->SetXTitle("x");
  hJetPartitionChCone03Pt1->Sumw2();
  fListHistos->Add(hJetPartitionChCone03Pt1) ;



  //Scalar product jet vs particle of jet

 
  TH1F * hJetScalarChCone06Pt05  = new TH1F
    ("JetScalarChCone06Pt05","Scalar product jet*particle, charged, cone = 0.6, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarChCone06Pt05->SetYTitle("N");
  hJetScalarChCone06Pt05->SetXTitle("Product");
  hJetScalarChCone06Pt05->Sumw2();
  fListHistos->Add(hJetScalarChCone06Pt05) ; 
  
  TH1F * hJetScalarChCone05Pt05  = new TH1F
    ("JetScalarChCone05Pt05","Scalar product jet*particle, charged, cone = 0.5, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarChCone05Pt05->SetYTitle("N");
  hJetScalarChCone05Pt05->SetXTitle("Product");
  hJetScalarChCone05Pt05->Sumw2();
  fListHistos->Add(hJetScalarChCone05Pt05) ; 
  
  TH1F * hJetScalarChCone04Pt05  = new TH1F
    ("JetScalarChCone04Pt05","Scalar product jet*particle, charged, cone = 0.4, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarChCone04Pt05->SetYTitle("N");
  hJetScalarChCone04Pt05->SetXTitle("Product");
  hJetScalarChCone04Pt05->Sumw2();
  fListHistos->Add(hJetScalarChCone04Pt05) ; 
  
  TH1F * hJetScalarChCone03Pt05  = new TH1F
    ("JetScalarChCone03Pt05","Scalar product jet*particle, charged, cone = 0.3, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarChCone03Pt05->SetYTitle("N");
  hJetScalarChCone03Pt05->SetXTitle("Product");
  hJetScalarChCone03Pt05->Sumw2();
  fListHistos->Add(hJetScalarChCone03Pt05) ; 
  

  TH1F * hJetScalarChCone06Pt0  = new TH1F
    ("JetScalarChCone06Pt0","Scalar product jet*particle, charged, cone = 0.6, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarChCone06Pt0->SetYTitle("N");
  hJetScalarChCone06Pt0->SetXTitle("Product");
  hJetScalarChCone06Pt0->Sumw2();
  fListHistos->Add(hJetScalarChCone06Pt0) ; 
  
  TH1F * hJetScalarChCone05Pt0  = new TH1F
    ("JetScalarChCone05Pt0","Scalar product jet*particle, charged, cone = 0.5, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarChCone05Pt0->SetYTitle("N");
  hJetScalarChCone05Pt0->SetXTitle("Product");
  hJetScalarChCone05Pt0->Sumw2();
  fListHistos->Add(hJetScalarChCone05Pt0) ; 
  
  TH1F * hJetScalarChCone04Pt0  = new TH1F
    ("JetScalarChCone04Pt0","Scalar product jet*particle, charged, cone = 0.4, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarChCone04Pt0->SetYTitle("N");
  hJetScalarChCone04Pt0->SetXTitle("Product");
  hJetScalarChCone04Pt0->Sumw2();
  fListHistos->Add(hJetScalarChCone04Pt0) ; 
  
  TH1F * hJetScalarChCone03Pt0  = new TH1F
    ("JetScalarChCone03Pt0","Scalar product jet*particle, charged, cone = 0.3, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarChCone03Pt0->SetYTitle("N");
  hJetScalarChCone03Pt0->SetXTitle("Product");
  hJetScalarChCone03Pt0->Sumw2();
  fListHistos->Add(hJetScalarChCone03Pt0) ; 


  TH1F * hJetScalarChCone06Pt1  = new TH1F
    ("JetScalarChCone06Pt1","Scalar product jet*particle, charged, cone = 0.6, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarChCone06Pt1->SetYTitle("N");
  hJetScalarChCone06Pt1->SetXTitle("Product");
  hJetScalarChCone06Pt1->Sumw2();
  fListHistos->Add(hJetScalarChCone06Pt1) ; 
  
  TH1F * hJetScalarChCone05Pt1  = new TH1F
    ("JetScalarChCone05Pt1","Scalar product jet*particle, charged, cone = 0.5, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarChCone05Pt1->SetYTitle("N");
  hJetScalarChCone05Pt1->SetXTitle("Product");
  hJetScalarChCone05Pt1->Sumw2();
  fListHistos->Add(hJetScalarChCone05Pt1) ; 
 
  TH1F * hJetScalarChCone04Pt1  = new TH1F
    ("JetScalarChCone04Pt1","Scalar product jet*particle, charged, cone = 0.4, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarChCone04Pt1->SetYTitle("N");
  hJetScalarChCone04Pt1->SetXTitle("Product");
  hJetScalarChCone04Pt1->Sumw2();
  fListHistos->Add(hJetScalarChCone04Pt1) ; 
  
  TH1F * hJetScalarChCone03Pt1  = new TH1F
    ("JetScalarChCone03Pt1","Scalar product jet*particle, charged, cone = 0.3, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarChCone03Pt1->SetYTitle("N");
  hJetScalarChCone03Pt1->SetXTitle("Product");
  hJetScalarChCone03Pt1->Sumw2();
  fListHistos->Add(hJetScalarChCone03Pt1) ;
  
  //Scalar, Not only charged
  TH1F * hJetScalarCone06Pt05  = new TH1F
    ("JetScalarCone06Pt05","Scalar product jet*particle, cone = 0.6, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarCone06Pt05->SetYTitle("N");
  hJetScalarCone06Pt05->SetXTitle("Product");
  hJetScalarCone06Pt05->Sumw2();
  fListHistos->Add(hJetScalarCone06Pt05) ; 
  
  TH1F * hJetScalarCone05Pt05  = new TH1F
    ("JetScalarCone05Pt05","Scalar product jet*particle, cone = 0.5, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarCone05Pt05->SetYTitle("N");
  hJetScalarCone05Pt05->SetXTitle("Product");
  hJetScalarCone05Pt05->Sumw2();
  fListHistos->Add(hJetScalarCone05Pt05) ; 
  
  TH1F * hJetScalarCone04Pt05  = new TH1F
    ("JetScalarCone04Pt05","Scalar product jet*particle, cone = 0.4, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarCone04Pt05->SetYTitle("N");
  hJetScalarCone04Pt05->SetXTitle("Product");
  hJetScalarCone04Pt05->Sumw2();
  fListHistos->Add(hJetScalarCone04Pt05) ; 
  
  TH1F * hJetScalarCone03Pt05  = new TH1F
    ("JetScalarCone03Pt05","Scalar product jet*particle, cone = 0.3, pt> 0.5 GeV/c",100,0.8,1.); 
  hJetScalarCone03Pt05->SetYTitle("N");
  hJetScalarCone03Pt05->SetXTitle("Product");
  hJetScalarCone03Pt05->Sumw2();
  fListHistos->Add(hJetScalarCone03Pt05) ; 
  

  TH1F * hJetScalarCone06Pt0  = new TH1F
    ("JetScalarCone06Pt0","Scalar product jet*particle, cone = 0.6, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarCone06Pt0->SetYTitle("N");
  hJetScalarCone06Pt0->SetXTitle("Product");
  hJetScalarCone06Pt0->Sumw2();
  fListHistos->Add(hJetScalarCone06Pt0) ; 
  
  TH1F * hJetScalarCone05Pt0  = new TH1F
    ("JetScalarCone05Pt0","Scalar product jet*particle, cone = 0.5, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarCone05Pt0->SetYTitle("N");
  hJetScalarCone05Pt0->SetXTitle("Product");
  hJetScalarCone05Pt0->Sumw2();
  fListHistos->Add(hJetScalarCone05Pt0) ; 
  
  TH1F * hJetScalarCone04Pt0  = new TH1F
    ("JetScalarCone04Pt0","Scalar product jet*particle, cone = 0.4, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarCone04Pt0->SetYTitle("N");
  hJetScalarCone04Pt0->SetXTitle("Product");
  hJetScalarCone04Pt0->Sumw2();
  fListHistos->Add(hJetScalarCone04Pt0) ; 
  
  TH1F * hJetScalarCone03Pt0  = new TH1F
    ("JetScalarCone03Pt0","Scalar product jet*particle, cone = 0.3, pt> 0. GeV/c",100,0.8,1.); 
  hJetScalarCone03Pt0->SetYTitle("N");
  hJetScalarCone03Pt0->SetXTitle("Product");
  hJetScalarCone03Pt0->Sumw2();
  fListHistos->Add(hJetScalarCone03Pt0) ; 


  TH1F * hJetScalarCone06Pt1  = new TH1F
    ("JetScalarCone06Pt1","Scalar product jet*particle, cone = 0.6, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarCone06Pt1->SetYTitle("N");
  hJetScalarCone06Pt1->SetXTitle("Product");
  hJetScalarCone06Pt1->Sumw2();
  fListHistos->Add(hJetScalarCone06Pt1) ; 
  
  TH1F * hJetScalarCone05Pt1  = new TH1F
    ("JetScalarCone05Pt1","Scalar product jet*particle, cone = 0.5, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarCone05Pt1->SetYTitle("N");
  hJetScalarCone05Pt1->SetXTitle("Product");
  hJetScalarCone05Pt1->Sumw2();
  fListHistos->Add(hJetScalarCone05Pt1) ; 
 
  TH1F * hJetScalarCone04Pt1  = new TH1F
    ("JetScalarCone04Pt1","Scalar product jet*particle, cone = 0.4, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarCone04Pt1->SetYTitle("N");
  hJetScalarCone04Pt1->SetXTitle("Product");
  hJetScalarCone04Pt1->Sumw2();
  fListHistos->Add(hJetScalarCone04Pt1) ; 
  
  TH1F * hJetScalarCone03Pt1  = new TH1F
    ("JetScalarCone03Pt1","Scalar product jet*particle, cone = 0.3, pt> 1.0 GeV/c",100,0.8,1.); 
  hJetScalarCone03Pt1->SetYTitle("N");
  hJetScalarCone03Pt1->SetXTitle("Product");
  hJetScalarCone03Pt1->Sumw2();
  fListHistos->Add(hJetScalarCone03Pt1) ;
  
  
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
//  particle = 0x0;
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
  printf("Pt jet cut        : %f\n", fPtEffCut) ;
  printf("Scalar Product cut: %f\n", fScalarProductCut) ;
  printf("Eta cut           : %f\n", fEtaCut) ;
  printf("D phi max cut     : %f\n", fPhiMaxCut) ; 
  printf("D phi min cut     : %f\n", fPhiMinCut) ;
  printf("Ratio max cut     : %f\n", fRatioMaxCut) ; 
  printf("Ratio min cut     : %f\n", fRatioMinCut) ;
  printf("Fast recons       : %d\n", fOptFast);
  printf("Inv Mass max cut  : %f\n", fInvMassMaxCut) ; 
  printf("Inv Mass min cut  : %f\n", fInvMassMinCut) ; 
 
} 

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

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
//_________________________________________________________________________
// An analysis task to check the TOF in simulated data
//
//*-- Silvia Arcelli
//-Distributions of the matching performance
//-TOF Time info and (TOF - expected time) plots
//-Summary Plots on TOF Pid
//////////////////////////////////////////////////////////////////////////////
#include <TChain.h>
#include <TFile.h> 
#include <TObject.h> 
#include <TCanvas.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TLatex.h>
#include <TPad.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TROOT.h>

#include "AliTOFQATask.h" 
#include "AliESD.h" 
#include "AliESDtrack.h" 
#include "AliLog.h"

//______________________________________________________________________________
AliTOFQATask::AliTOFQATask(const char *name) : 
  AliAnalysisTask(name,""),  
  fChain(0),
  fESD(0), 
  fOutputContainer(0), 
  fhTOFMatch(0),
  fhESDeffPhi(0),
  fhESDeffTheta(0),
  fhESDeffMom(0),
  fhTOFeffPhi(0),
  fhTOFeffTheta(0),
  fhTOFeffMom(0),
  fhTOFeffPhiMT(0),
  fhTOFeffThetaMT(0),
  fhTOFeffMomMT(0),
  fhTOFsector(0),
  fhTOFsectorMT(0),
  fhTOFTime(0),
  fhTOFDeltaTime(0),
  fhTOFDeltaTimeMT(0),
  fhTOFIDSpecies(0),
  fhTOFMassVsMom(0),
  fhTOFMass(0),
  fmatchFracMin(0.15),
  fmatchEffMin(0.28),
  ftimePeakMax(0.05),
  fmassPeakMax(0.05)
{
  // Constructor.
  // Input slot #0 works with an Ntuple
  DefineInput(0, TChain::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(0,  TObjArray::Class()) ; 
}

//______________________________________________________________________________
AliTOFQATask::AliTOFQATask(const AliTOFQATask &qatask) : 
  AliAnalysisTask("AliTOFQATask",""),  
  fChain(0),
  fESD(0), 
  fOutputContainer(0), 
  fhTOFMatch(0),
  fhESDeffPhi(0),
  fhESDeffTheta(0),
  fhESDeffMom(0),
  fhTOFeffPhi(0),
  fhTOFeffTheta(0),
  fhTOFeffMom(0),
  fhTOFeffPhiMT(0),
  fhTOFeffThetaMT(0),
  fhTOFeffMomMT(0),
  fhTOFsector(0),
  fhTOFsectorMT(0),
  fhTOFTime(0),
  fhTOFDeltaTime(0),
  fhTOFDeltaTimeMT(0),
  fhTOFIDSpecies(0),
  fhTOFMassVsMom(0),
  fhTOFMass(0),
  fmatchFracMin(0.15),
  fmatchEffMin(0.28),
  ftimePeakMax(0.05),
  fmassPeakMax(0.05)
{
  // Copy Constructor.
  fChain=qatask.fChain;
  fESD=qatask.fESD; 
  fOutputContainer=qatask.fOutputContainer; 
  fhTOFMatch=qatask.fhTOFMatch;
  fhESDeffPhi=qatask.fhESDeffPhi;
  fhESDeffTheta=qatask.fhESDeffTheta;
  fhESDeffMom=qatask.fhESDeffMom;
  fhTOFeffPhi=qatask.fhTOFeffPhi;
  fhTOFeffTheta=qatask.fhTOFeffTheta;
  fhTOFeffMom=qatask.fhTOFeffMom;
  fhTOFeffPhiMT=qatask.fhTOFeffPhiMT;
  fhTOFeffThetaMT=qatask.fhTOFeffThetaMT;
  fhTOFeffMomMT=qatask.fhTOFeffMomMT;
  fhTOFsector=qatask.fhTOFsector;
  fhTOFsectorMT=qatask.fhTOFsectorMT;
  fhTOFTime=qatask.fhTOFTime;
  fhTOFDeltaTime=qatask.fhTOFDeltaTime;
  fhTOFDeltaTimeMT=qatask.fhTOFDeltaTimeMT;
  fhTOFIDSpecies=qatask.fhTOFIDSpecies;
  fhTOFMassVsMom=qatask.fhTOFMassVsMom;
  fhTOFMass=qatask.fhTOFMass;
  fmatchFracMin=qatask.fmatchFracMin;
  fmatchEffMin=qatask.fmatchEffMin;
  ftimePeakMax=qatask.ftimePeakMax;
  fmassPeakMax=qatask.fmassPeakMax;
}
//______________________________________________________________________________
AliTOFQATask:: ~AliTOFQATask() 
{
  delete fOutputContainer;
  delete fhTOFMatch;
  delete fhESDeffPhi;
  delete fhESDeffTheta;
  delete fhESDeffMom;
  delete fhTOFeffPhi;
  delete fhTOFeffTheta;
  delete fhTOFeffMom;
  delete fhTOFeffPhiMT;
  delete fhTOFeffThetaMT;
  delete fhTOFeffMomMT;
  delete fhTOFsector;
  delete fhTOFsectorMT;
  delete fhTOFTime;
  delete fhTOFDeltaTime;
  delete fhTOFDeltaTimeMT;
  delete fhTOFIDSpecies;
  delete fhTOFMassVsMom;
  delete fhTOFMass;
  }
//______________________________________________________________________________
AliTOFQATask& AliTOFQATask::operator=(const AliTOFQATask &qatask)  
{ 
   //assignment operator
  this->fChain=qatask.fChain;
  this->fESD=qatask.fESD; 
  this->fOutputContainer=qatask.fOutputContainer; 
  this->fhTOFMatch=qatask.fhTOFMatch;
  this->fhESDeffPhi=qatask.fhESDeffPhi;
  this->fhESDeffTheta=qatask.fhESDeffTheta;
  this->fhESDeffMom=qatask.fhESDeffMom;
  this->fhTOFeffPhi=qatask.fhTOFeffPhi;
  this->fhTOFeffTheta=qatask.fhTOFeffTheta;
  this->fhTOFeffMom=qatask.fhTOFeffMom;
  this->fhTOFeffPhiMT=qatask.fhTOFeffPhiMT;
  this->fhTOFeffThetaMT=qatask.fhTOFeffThetaMT;
  this->fhTOFeffMomMT=qatask.fhTOFeffMomMT;
  this->fhTOFsector=qatask.fhTOFsector;
  this->fhTOFsectorMT=qatask.fhTOFsectorMT;
  this->fhTOFTime=qatask.fhTOFTime;
  this->fhTOFDeltaTime=qatask.fhTOFDeltaTime;
  this->fhTOFDeltaTimeMT=qatask.fhTOFDeltaTimeMT;
  this->fhTOFIDSpecies=qatask.fhTOFIDSpecies;
  this->fhTOFMassVsMom=qatask.fhTOFMassVsMom;
  this->fhTOFMass=qatask.fhTOFMass;
  this->fmatchFracMin=qatask.fmatchFracMin;
  this->fmatchEffMin=qatask.fmatchEffMin;
  this->ftimePeakMax=qatask.ftimePeakMax;
  this->fmassPeakMax=qatask.fmassPeakMax;
  return *this;
}
//______________________________________________________________________________
void AliTOFQATask::Init(const Option_t*)
{
  // Initialisation of branch container and histograms 
    
  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  
  // Get input data
  fChain = dynamic_cast<TChain *>(GetInputData(0)) ;
  if (!fChain) {
    AliError(Form("Input 0 for %s not found\n", GetName()));
    return ;
  }
  
  if (!fESD) {
    // One should first check if the branch address was taken by some other task
    char ** address = (char **)GetBranchAddress(0, "ESD") ;
    if (address) 
      fESD = (AliESD *)(*address) ; 
    if (!fESD) 
      fChain->SetBranchAddress("ESD", &fESD) ;  
  }
  
  BookHistos();
}

//______________________________________________________________________________
void AliTOFQATask::Exec(Option_t *) 
{

//******* The loop over events --------------------------------------------------

  Int_t nselESD=0;
  Int_t nmatchTOF=0;
  Int_t npidok=0;
  Int_t pisel=0,kasel=0,prsel=0,elsel=0,musel=0;
  //Set equal a-priori weights (just charged hadrions)
  const Int_t nCalinSec=8736;
  Double_t c[5]={0, 0, 1, 1, 1};
  // Processing of one event
  Long64_t entry = fChain->GetReadEntry() ;  
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  }
  
  if ( !((entry-1)%100) ) 
    AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), entry)) ; 
  
  // ************************  TOF *************************************


  Int_t ntrk = fESD->GetNumberOfTracks() ;

  while ( ntrk-- ) {
    
    AliESDtrack * t = fESD->GetTrack(ntrk) ;
    if ( (t->GetStatus() & AliESDtrack::kTIME)==0 )continue;    

    nselESD++;

    Double_t mom   = t->GetP() ; 
    Double_t phi   = TMath::ATan2(t->GetX(),t->GetY()) ; 
    Double_t theta = TMath::ACos(t->GetZ()/
    TMath::Sqrt(t->GetX()*t->GetX()+t->GetY()*t->GetY()+t->GetZ()*t->GetZ())); 
    phi*=180/TMath::Pi();
    theta*=180/TMath::Pi();

    fhESDeffPhi->Fill(phi);
    fhESDeffTheta->Fill(theta);
    fhESDeffMom->Fill(mom);


    if(t->GetTOFsignal()<0)continue;

    nmatchTOF++;

    Double_t time=t->GetTOFsignal();//TOF time in ps	
    Int_t detid=t->GetTOFCalChannel();//which pad was hit
    Int_t sector = detid/nCalinSec;
    fhTOFTime->Fill(time*1.E-3);
    fhTOFeffPhi->Fill(phi);
    fhTOFeffTheta->Fill(theta);
    fhTOFeffMom->Fill(mom);
    fhTOFsector->Fill(sector);
    //Well matched

    Int_t label=TMath::Abs(t->GetLabel());
    Int_t clab[3]; t->GetTOFLabel(clab);    
    if(label==clab[0] || label==clab[1] || label==clab[2]) {
      fhTOFeffPhiMT->Fill(phi);
      fhTOFeffThetaMT->Fill(theta);
      fhTOFeffMomMT->Fill(mom);
      fhTOFsectorMT->Fill(sector);
    }      	

    //Look at TOF PID 

    UInt_t status=AliESDtrack::kESDpid;status|=AliESDtrack::kTOFpid; 
    if (!((t->GetStatus()&status) == status))continue;
    npidok++;
    Double_t times[10];
    t->GetIntegratedTimes(times);//ps
    Double_t l   =t->GetIntegratedLength()/100.; // (m)
    Double_t mass= -1.;
    Double_t invBetaGamma= (0.299*time*1.E-3/l)*(0.299*time*1.E-3/l) -1.;
    if(invBetaGamma<0){mass = -mom*TMath::Sqrt(-invBetaGamma);}
    else{mass = mom*TMath::Sqrt(invBetaGamma);}

    //The Mass/ vs Momentum Plot:
    fhTOFMassVsMom->Fill(mass,mom);
    fhTOFMass->Fill(mass);
    
    //PID weights 
    Double_t r[10]; t->GetTOFpid(r);
    Double_t rcc=0.;
    Int_t i;
    for (i=0; i<AliPID::kSPECIES; i++) rcc+=(c[i]*r[i]);
    if (rcc==0.) continue;
    Double_t w[10];
    for (i=0; i<AliPID::kSPECIES; i++) w[i]=c[i]*r[i]/rcc;
    
    fhTOFDeltaTime->Fill((time-times[2])*1.E-3);

    if(label==clab[0] || label==clab[1] || label==clab[2]) {
      fhTOFDeltaTimeMT->Fill((time-times[2])*1.E-3);
    }      	
    
    if (w[4]>w[0] && w[4]>w[1] && w[4]>w[2] && w[4]>w[3]){
      prsel++;
      fhTOFIDSpecies->Fill(4);
    }
    if (w[3]>w[0] && w[3]>w[1] && w[3]>w[2] && w[3]>w[4]){
      kasel++;
      fhTOFIDSpecies->Fill(3);
    }
    if (w[2]>w[0] && w[2]>w[1] && w[2]>w[3] && w[2]>w[4]){
      pisel++;
      fhTOFIDSpecies->Fill(2);
    }
    if (w[1]>w[0] && w[1]>w[2] && w[1]>w[3] && w[1]>w[4]){
      musel++;
      fhTOFIDSpecies->Fill(1);
    }
    if (w[0]>w[1] && w[0]>w[2] && w[0]>w[3] && w[0]>w[4]){
      elsel++;	 
      fhTOFIDSpecies->Fill(0);
    }
  }

  Float_t fracM= -1;
  if(nselESD>10)fracM=((Float_t) nmatchTOF)/((Float_t) nselESD);
  fhTOFMatch->Fill(fracM);

  PostData(0, fOutputContainer);  
  AliInfo("Finishing event processing...") ; 

}
//______________________________________________________________________________
void AliTOFQATask::BookHistos()
{  
  // Construct histograms:
  
  fhTOFMatch= 
    new TH1F("hTOFMatch","Fraction of Matched TOF tracks",101,-0.005,1.005);
  fhESDeffPhi= 
    new TH1F("hESDeffPhi","ESD tracks Phi(vtx)",    180, -180., 180.) ;
  fhESDeffTheta= 
    new TH1F("hESDeffTheta","ESD tracks Theta (vtx)",90, 45., 135.) ;
  fhESDeffMom= 
    new TH1F("hESDeffMom","ESD tracks Momentum (vtx)",40, 0., 6.) ;
  fhTOFeffPhi = 
    new TH1F("hTOFeffPhi","TOF, Matched vs Phi(vtx)", 180,-180, 180.);
  fhTOFeffPhiMT= 
    new TH1F("hTOFeffPhiMT","TOF, Well Matched vs Phi(vtx)",180,-180,180.);
  fhTOFeffTheta= 
    new TH1F("hTOFeffTheta","TOF, Matched vs Theta(vtx)",90,45.,135.);  
  fhTOFeffThetaMT= 
    new TH1F("hTOFeffThetaMT","TOF, Well Matched vs Theta(vtx)",90,45.,135.);
  fhTOFeffMom = 
    new TH1F("hTOFeffMom","TOF, Matched vs Momentum ", 40, 0.,6.);
  fhTOFeffMomMT = 
    new TH1F("hTOFeffMomMT","TOF, Well Matched vs Momentum ", 40, 0.,6.); 
  fhTOFsector = 
    new TH1F("hTOFsector","TOF, Matched vs Sector ", 18,0.,18.); 
  fhTOFsectorMT = 
    new TH1F("hTOFsectorMT","TOF, Well Matched vs Sector", 18, 0.,18.); 

  fhESDeffMom->Sumw2(); fhTOFeffMom->Sumw2();  fhTOFeffMomMT->Sumw2();
  fhESDeffTheta->Sumw2();  fhTOFeffTheta->Sumw2();  fhTOFeffThetaMT->Sumw2();
  fhESDeffPhi->Sumw2();  fhTOFeffPhi->Sumw2();  fhTOFeffPhiMT->Sumw2();
  fhTOFsector->Sumw2(); fhTOFsectorMT->Sumw2();

  fhTOFTime = 
    new TH1F("hTOFTime","TOF, t(TOF)in ns ",1000,0,100.); 
  fhTOFDeltaTime = 
    new TH1F("hTOFDeltaTime","TOF,t(TOF)-t(exp,pion), ns ",1000,-4.4,20.);   
  fhTOFDeltaTimeMT = 
    new TH1F("hTOFDeltaTimeMT","TOF, t(TOF)-t(exp,pion) for Well Matched, ns ",1000,-4.4,20.); 
  fhTOFIDSpecies = 
    new TH1F("hTOFIDSpecies","TOF, ID Sample Composition ",5,-0.5,4.5);
  fhTOFMassVsMom = 
    new TH2F("hTOFMassVsMom","TOF, Mass Vs Momentum ",280,-0.2,1.2,600,0.,6.);
  fhTOFMass = 
    new TH1F("hTOFMass","TOF, reconstructed mass ",280,-0.2,1.2);

  
  // create the output container
  
  fOutputContainer = new TObjArray(18) ; 
  fOutputContainer->SetName(GetName()) ; 

  fOutputContainer->AddAt(fhTOFMatch,             0) ; 
  fOutputContainer->AddAt(fhESDeffPhi,            1) ; 
  fOutputContainer->AddAt(fhESDeffTheta,          2) ; 
  fOutputContainer->AddAt(fhESDeffMom,            3) ; 
  fOutputContainer->AddAt(fhTOFeffPhi,            4) ; 
  fOutputContainer->AddAt(fhTOFeffPhiMT,          5) ; 
  fOutputContainer->AddAt(fhTOFeffTheta,          6) ; 
  fOutputContainer->AddAt(fhTOFeffThetaMT,        7) ; 
  fOutputContainer->AddAt(fhTOFeffMom,            8) ; 
  fOutputContainer->AddAt(fhTOFeffMomMT,          9) ; 
  fOutputContainer->AddAt(fhTOFsector,           10) ; 
  fOutputContainer->AddAt(fhTOFsectorMT,         11) ; 
  fOutputContainer->AddAt(fhTOFTime,             12) ; 
  fOutputContainer->AddAt(fhTOFDeltaTime ,       13) ; 
  fOutputContainer->AddAt(fhTOFDeltaTimeMT,      14) ; 
  fOutputContainer->AddAt(fhTOFIDSpecies,        15) ; 
  fOutputContainer->AddAt(fhTOFMassVsMom,        16) ; 
  fOutputContainer->AddAt(fhTOFMass,             17) ; 
  
}
//______________________________________________________________________________
void AliTOFQATask::GetEfficiency() 
{
  // calculates the efficiency
  
  
  fhTOFeffPhiMT->Divide(fhTOFeffPhiMT,fhTOFeffPhi,1,1,"B");
  fhTOFeffThetaMT->Divide(fhTOFeffThetaMT,fhTOFeffTheta,1,1,"B");
  fhTOFeffMomMT->Divide(fhTOFeffMomMT,fhTOFeffMom,1,1,"B");

  fhTOFeffPhi->Divide(fhTOFeffPhi,fhESDeffPhi,1,1,"B");
  fhTOFeffTheta->Divide(fhTOFeffTheta,fhESDeffTheta,1,1,"B");
  fhTOFeffMom->Divide(fhTOFeffMom,fhESDeffMom,1,1,"B");
  
}

//______________________________________________________________________________
void AliTOFQATask::DrawHistos() 
{


  Int_t TOFsectors[18]={0,1,1,0,0,0,1,1,0,1,1,1,1,0,0,1,1,1};//TOF sectors which are supposed to be present
  char* report=" ";
  const char* part[3]={"pions","kaons","protons"};// pi,ka,pr
  const Float_t masses[3]={0.1396,0.494,0.938};// particle masses pi,ka,pr
  // Makes a few plots

  AliInfo("Plotting....") ; 

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(111110);
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetFrameFillColor(0);
  //
  TGaxis::SetMaxDigits(3);  
  gStyle->SetLabelFont(52, "XYZ");
  gStyle->SetTitleFont(62, "XYZ");
  gStyle->SetPadRightMargin(0.02);

  
  TCanvas * cTOFeff = new TCanvas("cTOFeff", "TOF ESD General", 400, 30, 550, 630) ;

  cTOFeff->Divide(1,2) ;
  cTOFeff->cd(1);
  fhTOFMatch->GetXaxis()->SetTitle("Fraction of matched ESD tracks/event");
  fhTOFMatch->GetYaxis()->SetTitle("Events");
  fhTOFMatch->SetFillColor(4);
  fhTOFMatch->Draw();  
  cTOFeff->cd(2);
  TPad *b = (TPad*)gPad;
  b->Divide(2,1);
  b->cd(1) ;
  fhTOFsector->GetXaxis()->SetTitle("Sector index of matched TOF Cluster");
  fhTOFsector->GetYaxis()->SetTitle("Entries");
  fhTOFsector->SetFillColor(4);
  fhTOFsector->SetMinimum(0.);
  fhTOFsector->GetYaxis()->SetNdivisions(205);
  fhTOFsector->GetYaxis()->SetTitleOffset(1.2);
  fhTOFsector->Draw("histo");
  b->cd(2) ;
  fhTOFeffMom->SetMaximum(1.2);
  fhTOFeffMom->GetXaxis()->SetTitle("Track momentum (GeV/c)");
  fhTOFeffMom->GetYaxis()->SetNdivisions(205);
  fhTOFeffMom->GetYaxis()->SetTitleOffset(1.2);
  fhTOFeffMom->GetYaxis()->SetTitle("Fraction of matched ESD tracks");
  fhTOFeffMom->SetFillColor(4);
  fhTOFeffMom->Draw();
  fhTOFeffMom->Fit("pol0","Q","",0.5,3.);
  TF1 *fitMom = fhTOFeffMom->GetFunction("pol0");
  fhTOFeffMom->Draw("histo,same");
  
  cTOFeff->Print("TOF_eff.gif");



  TCanvas * cTOFtime = new TCanvas("cTOFtime", "TOF measured Times ", 400, 30, 550, 630) ;  
  cTOFtime->Divide(1,2) ;
  cTOFtime->cd(1);
  cTOFtime->GetPad(1)->SetLogy(1);
  fhTOFTime->GetXaxis()->SetTitle("TOF time (ns)");
  fhTOFTime->GetYaxis()->SetTitle("Entries");
  fhTOFTime->SetFillColor(4);
  fhTOFTime->Draw();
  cTOFtime->cd(2);
  cTOFtime->GetPad(2)->SetLogy(1);
  fhTOFDeltaTime->GetXaxis()->SetTitle("t^{TOF}-t^{exp}_{#pi} (ns)");
  fhTOFDeltaTime->GetYaxis()->SetTitle("Entries");
  fhTOFDeltaTime->SetFillColor(4);
  Int_t ntimepeak=1;
  TSpectrum *timeDiff = new TSpectrum(2*ntimepeak);
  Int_t ntime = timeDiff->Search(fhTOFDeltaTime,ntimepeak," ",0.1);
  fhTOFDeltaTime->Draw();

  cTOFtime->Print("TOF_time.gif");


  TCanvas * cTOFpid = new TCanvas("cTOFpid", "TOF PID ", 400, 30, 550, 630) ;  
  cTOFpid->Divide(1,3) ;
  cTOFpid->cd(1);
  cTOFpid->SetLogy(1);
  fhTOFMass->GetXaxis()->SetTitle("Reconstructed Mass (GeV/c^{2})");
  fhTOFMass->GetYaxis()->SetTitle("Entries");
  fhTOFMass->SetFillColor(4);
  Int_t npmass=1;
  if(fhTOFMass->GetEntries()>1000)npmass=3;
  TSpectrum *mass = new TSpectrum(2*npmass);
  Int_t nmass = mass->Search(fhTOFMass,npmass," ",0.02);
  fhTOFMass->Draw();
  cTOFpid->SetLogy(0);
  cTOFpid->cd(2);
  fhTOFMassVsMom->GetYaxis()->SetRange(0,400);
  fhTOFMassVsMom->GetXaxis()->SetTitle("Reconstructed Mass (GeV/c^{2})");
  fhTOFMassVsMom->GetYaxis()->SetTitle("Track Momentum (GeV/c)");
  fhTOFMassVsMom->GetXaxis()->SetTitleSize(0.05);
  fhTOFMassVsMom->GetYaxis()->SetTitleSize(0.05);
  fhTOFMassVsMom->SetMarkerStyle(20);
  fhTOFMassVsMom->SetMarkerSize(0.05);
  fhTOFMassVsMom->SetMarkerColor(2);
  fhTOFMassVsMom->Draw();
  cTOFpid->cd(3);

  TLatex *   tex = new TLatex(1., 1.25, "Bayesian PID: a-priori concentrations: [0,0,1,1,1]");
  tex->SetTextColor(1);
  tex->SetTextSize(0.045);
  tex->SetLineWidth(2);

  Float_t norm=1./fhTOFIDSpecies->GetEntries();
  fhTOFIDSpecies->Scale(norm);
  fhTOFIDSpecies->SetMaximum(1.2);
  fhTOFIDSpecies->GetXaxis()->SetTitle("Particle Type");
  fhTOFIDSpecies->GetYaxis()->SetTitle("ID Fractions");
  fhTOFIDSpecies->GetXaxis()->SetTitleSize(0.05);
  fhTOFIDSpecies->GetYaxis()->SetTitleSize(0.05);
  fhTOFIDSpecies->SetFillColor(4);
  fhTOFIDSpecies->Draw();
  tex->Draw();
 
  char ch[10];

  Float_t pifrac=fhTOFIDSpecies->GetBinContent(3);
  Float_t kafrac=fhTOFIDSpecies->GetBinContent(4);
  Float_t prfrac=fhTOFIDSpecies->GetBinContent(5);

  sprintf(ch,"[2]:pion fraction   = %5.3f",pifrac);    
  TLatex *   texpi = new TLatex(-0.3, 0.9, ch);
  texpi->SetTextColor(1);
  texpi->SetTextSize(0.05);
  texpi->SetLineWidth(2);
  texpi->Draw();
  sprintf(ch,"[3]:kaon fraction   = %5.3f",kafrac);    
  TLatex *   texka = new TLatex(-0.3, 0.8, ch);
  texka->SetTextColor(1);
  texka->SetTextSize(0.05);
  texka->SetLineWidth(2);
  texka->Draw();
  sprintf(ch,"[4]:proton fraction = %5.3f",prfrac);    
  TLatex *   texpr = new TLatex(-0.3, 0.7, ch);
  texpr->SetTextColor(1);
  texpr->SetTextSize(0.05);
  texpr->SetLineWidth(2);
  texpr->Draw();


  cTOFpid->Print("TOF_pid.gif");

  char line[1024] ; 
  sprintf(line, ".!tar -zcvf %s.tar.gz *.gif", GetName()) ; 
  gROOT->ProcessLine(line);
  sprintf(line, ".!rm -fR *.gif"); 
  gROOT->ProcessLine(line);
  AliInfo(Form("*** TOF QA plots saved in %s.tar.gz...\n", GetName())) ;

  AliInfo("****-------- Start of TOF Data Quality report:------") ; 

  Bool_t problem=kFALSE;
  //------------------------------Matching Efficiency

  //Overall Fraction:
  Float_t matchFrac= fhTOFMatch->GetMean();
  if(matchFrac<fmatchFracMin){
    AliWarning(Form("*** Overall Fraction of matched tracks too low! Fraction = %f", matchFrac)) ;
    problem=kTRUE;
  }else{
    AliInfo(Form("*** Fraction of matched tracks  = %f", matchFrac)) ; 
  } 

  if(fhTOFeffMom->GetEntries()<1.){
    AliWarning(Form("*** No tracks matching with TOF! Fraction is = %f", matchFrac)) ; 
    problem=kTRUE;
  }

  
  //The efficiency as a function of momentum:
  Float_t eff=  fitMom->GetParameter(0);
  Float_t deff=  fitMom->GetParError(0);
  if(eff+3*deff<fmatchEffMin){
    AliWarning(Form("*** Fraction of matched tracks vs Momentum is too low! Fraction= %f", eff)) ;
    problem=kTRUE;
  }else{
    AliInfo(Form("*** Fraction of matched tracks for p>0.5 GeV is = %f", eff)) ;
  } 

  //Matched tracks vs TOF Sector:
  for(Int_t isec=1;isec<=18;isec++){
    if(fhTOFsector->GetBinContent(isec)<1 && TOFsectors[isec-1]>0){
      AliWarning(Form("*** Missing Entries in sector %i", isec)); 
      problem=kTRUE;
    }
    if(fhTOFsector->GetBinContent(isec)>0 && TOFsectors[isec-1]==0){
      AliWarning(Form("*** Unexpected Entries in sector %i", isec)); 
      problem=kTRUE;
    }
  }

  //-----------------------------Pid Quality

  // Look at the time - expected time: 
  if(ntime==0)AliWarning("*** No peak was found in time difference spectrum!");
  Float_t *timePos = timeDiff->GetPositionX();
  if(TMath::Abs(timePos[0])>ftimePeakMax){
    AliWarning(Form("*** Main Peak position in tTOF-TEXP spectrum is sitting far from where expected! Tpeak = %f ps",timePos[0]*1.E3));  
    problem=kTRUE;
  }else{
    AliInfo(Form("*** Main Peak position in tTOF-TEXP found at = %f ps",timePos[0]*1.E3));  
  }
  // Look at the Mass Spectrum: 
  if(nmass==0)AliWarning("*** No peak was found in Mass difference spectrum!");
  Float_t *massPos = mass->GetPositionX();
  for(Int_t imass=0;imass<nmass;imass++){   
    AliInfo(Form("*** the Mass peak for %s found at  = %f GeV/c^2",part[imass],massPos[imass]));
    if(TMath::Abs( massPos[imass]-masses[imass])> fmassPeakMax){
      AliWarning(Form("*** the Mass peak position for %s is not in the right place, found at  = %f GeV/c^2",part[imass],massPos[imass]));
      problem=kTRUE;
    }
  }

  // Look at the ID Species: 

  if(fhTOFIDSpecies->GetEntries()>1000){
    if(pifrac<0.8 || (kafrac<0.01 || kafrac>0.2) || (prfrac<0.01 || prfrac>0.2)){
      AliWarning(Form("*** Unexpected Id fractions: pions = %f, kaons = %f, protons %f", pifrac,kafrac,prfrac));
      problem=kTRUE;
    }
  }

  if(problem){ report="Problems found, please check!!!";  
  AliError(Form("****-------- End of TOF QA report, %s -",report)) ;} 

 else {report="Everything ok.";
   AliInfo(Form("****-------- End of TOF QA report, %s -",report)) ; 
 }  

  delete mass;
  delete timeDiff;
}

//______________________________________________________________________________
void AliTOFQATask::Terminate(Option_t *)
{
  // Processing when the event loop is ended, some plots+checks
  
  AliInfo("TOF QA Task: End of events loop");
  GetEfficiency();
  DrawHistos() ; 
}

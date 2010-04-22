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
// An analysis task to check the EMCAL photon data in simulated data
// An analysis task to check the EMCAL photon data in simulated data
// An analysis task to check the EMCAL photon data in simulated data
// An analysis task to check the EMCAL photon data in simulated data
// An analysis task to check the EMCAL photon data in simulated data
//
//*-- Yves Schutz 
//////////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h> 
#include <TH1.h>
#include <TNtuple.h>
#include <TROOT.h>
#include <TVector3.h> 
#include <TString.h> 

#include "AliEMCALQATask.h" 
#include "AliESD.h" 
#include "AliLog.h"

//______________________________________________________________________________
AliEMCALQATask::AliEMCALQATask(const char *name) : 
  AliAnalysisTask(name,""),  
  fChain(0),
  fESD(0), 
  fOutputContainer(0), 
  fhEMCALPos(0),
  fhEMCAL(0),
  fhEMCALEnergy(0),
  fhEMCALDigits(0),
  fhEMCALRecParticles(0),
  fhEMCALPhotons(0),
  fhEMCALInvariantMass(0),
  fhEMCALDigitsEvent(0)
{
  // Constructor.
  // Input slot #0 works with an Ntuple
  DefineInput(0, TChain::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(0,  TObjArray::Class()) ; 
}

//____________________________________________________________________________
AliEMCALQATask::AliEMCALQATask(const AliEMCALQATask& ta) :
  AliAnalysisTask(ta.GetName(),""),  
  fChain(ta.fChain),
  fESD(ta.fESD), 
  fOutputContainer(ta.fOutputContainer), 
  fhEMCALPos(ta.fhEMCALPos),
  fhEMCAL(ta.fhEMCAL),
  fhEMCALEnergy(ta.fhEMCALEnergy),
  fhEMCALDigits(ta.fhEMCALDigits),
  fhEMCALRecParticles(ta.fhEMCALRecParticles),
  fhEMCALPhotons(ta.fhEMCALPhotons),
  fhEMCALInvariantMass(ta.fhEMCALInvariantMass),
  fhEMCALDigitsEvent(ta.fhEMCALDigitsEvent)
{ 
  // cpy ctor
}

//_____________________________________________________________________________
AliEMCALQATask& AliEMCALQATask::operator = (const AliEMCALQATask& ap)
{
// assignment operator

  this->~AliEMCALQATask();
  new(this) AliEMCALQATask(ap);
  return *this;
}

//______________________________________________________________________________
AliEMCALQATask::~AliEMCALQATask()
{
  // dtor
  fOutputContainer->Clear() ; 
  delete fOutputContainer ; 

  delete fhEMCALPos ;
  delete fhEMCAL ;
  delete fhEMCALEnergy ;
  delete fhEMCALDigits ;
  delete fhEMCALRecParticles ;
  delete fhEMCALPhotons ;
  delete fhEMCALInvariantMass ;
  delete fhEMCALDigitsEvent ;
}

//______________________________________________________________________________
void AliEMCALQATask::ConnectInputData(const Option_t*)
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
  
//______________________________________________________________________________
void AliEMCALQATask::CreateOutputObjects()  
{
// create histograms  

  OpenFile(0) ; 

  fhEMCALPos           = new TNtuple("EMCALPos"        , "Position in EMCAL" , "x:y:z");
  fhEMCAL              = new TNtuple("EMCAL"           , "EMCAL" , "event:digits:clusters:photons");
  fhEMCALEnergy        = new TH1D("EMCALEnergy"        , "EMCALEnergy"       , 1000, 0., 10. ) ;
  fhEMCALDigits        = new TH1I("EMCALDigitsCluster" , "EMCALDigits"       , 20 , 0 , 20  ) ;
  fhEMCALRecParticles  = new TH1D("EMCALRecParticles"  , "EMCALRecParticles", 20 , 0., 20. ) ;
  fhEMCALPhotons       = new TH1I("EMCALPhotons"       , "EMCALPhotons"      , 20 , 0 , 20  ) ;
  fhEMCALInvariantMass = new TH1D("EMCALInvariantMass" , "EMCALInvariantMass", 400, 0., 400.) ;
  fhEMCALDigitsEvent   = new TH1I("EMCALDigitsEvent"   , "EMCALDigitsEvent"  , 30 , 0 , 30  ) ;
  
  // create output container
  
  fOutputContainer = new TObjArray(8) ; 
  fOutputContainer->SetName(GetName()) ; 

  fOutputContainer->AddAt(fhEMCALPos,            0) ; 
  fOutputContainer->AddAt(fhEMCAL,               1) ; 
  fOutputContainer->AddAt(fhEMCALEnergy,         2) ; 
  fOutputContainer->AddAt(fhEMCALDigits,         3) ; 
  fOutputContainer->AddAt(fhEMCALRecParticles,   4) ; 
  fOutputContainer->AddAt(fhEMCALPhotons,        5) ; 
  fOutputContainer->AddAt(fhEMCALInvariantMass,  6) ; 
  fOutputContainer->AddAt(fhEMCALDigitsEvent,    7) ; 
}

//______________________________________________________________________________
void AliEMCALQATask::Exec(Option_t *) 
{
  // Processing of one event
 
  Long64_t entry = fChain->GetReadEntry() ;
 
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  }
  
  if ( !((entry-1)%100) ) 
    AliInfo(Form("%s ----> Processing event # %lld",  (dynamic_cast<TChain *>(fChain))->GetFile()->GetName(), entry)) ; 
  
  //************************  EMCAL *************************************
  Int_t       firstEmcalCluster       = fESD->GetFirstEMCALCluster() ;
  const Int_t kNumberOfEmcalClusters   = fESD->GetNumberOfEMCALClusters() ;

  TVector3 ** emcalVector        = new TVector3*[kNumberOfEmcalClusters] ;
  Float_t   * emcalPhotonsEnergy = new Float_t[kNumberOfEmcalClusters] ;
  Int_t      emcalCluster ; 
  Int_t      numberOfEmcalClustersv1 = 0 ; 
  Int_t      numberOfPhotonsInEmcal  = 0 ;
  Int_t      numberOfDigitsInEmcal   = 0 ;  


  // loop over all the EMCAL Cluster
  for(emcalCluster = firstEmcalCluster ; emcalCluster < firstEmcalCluster + kNumberOfEmcalClusters ; emcalCluster++) {
    AliESDCaloCluster * caloCluster = fESD->GetCaloCluster(emcalCluster) ;
    if (caloCluster) {
      Float_t pos[3] ;
      if(caloCluster->GetClusterType() == AliESDCaloCluster::kEMCALClusterv1) {  
	caloCluster->GetPosition(pos) ;
	fhEMCALPos->Fill(pos[0],pos[1],pos[2]) ;
	fhEMCALEnergy->Fill(caloCluster->E()) ;
	fhEMCALDigits->Fill(entry, caloCluster->GetNCells()) ;
	numberOfEmcalClustersv1++ ;
	numberOfDigitsInEmcal += caloCluster->GetNCells() ;    
	// Float_t * pid = clus->GetPid() ;
	// if(pid[AliPID::kPhoton]>0.9){
	emcalVector[numberOfPhotonsInEmcal] = new TVector3(pos[0],pos[1],pos[2]) ;
	emcalPhotonsEnergy[numberOfPhotonsInEmcal] = caloCluster->E() ;
	numberOfPhotonsInEmcal++ ; 
      }
    }
  } // EMCAL clusters loop

  fhEMCALRecParticles->Fill(numberOfEmcalClustersv1);
  fhEMCALPhotons->Fill(numberOfPhotonsInEmcal);
  fhEMCALDigitsEvent->Fill(numberOfDigitsInEmcal);
  fhEMCAL->Fill(entry, numberOfDigitsInEmcal, numberOfEmcalClustersv1, numberOfPhotonsInEmcal) ; 
  
  // invariant Mass
  if (numberOfPhotonsInEmcal > 1 ) {
    Int_t emcalPhoton1, emcalPhoton2 ; 
    for(emcalPhoton1 = 0 ; emcalPhoton1 < numberOfPhotonsInEmcal ; emcalPhoton1++) {
      for(emcalPhoton2 = emcalPhoton1 + 1 ; emcalPhoton2 < numberOfPhotonsInEmcal ; emcalPhoton2++) {
	Float_t tempMass = TMath::Sqrt( 2 * emcalPhotonsEnergy[emcalPhoton1] * emcalPhotonsEnergy[emcalPhoton2] * 
					( 1 - TMath::Cos( emcalVector[emcalPhoton1]->Angle(*emcalVector[emcalPhoton2])) 
					  )
					);
	fhEMCALInvariantMass->Fill(tempMass*1000.);
      }
    }    
  }
  
  PostData(0, fOutputContainer);
  
  delete [] emcalVector ; 
  delete [] emcalPhotonsEnergy ;
}

//______________________________________________________________________________
void AliEMCALQATask::Terminate(Option_t *)
{
  // Processing when the event loop is ended
  fOutputContainer = (TObjArray*)GetOutputData(0);
  fhEMCALEnergy = (TH1D*)fOutputContainer->At(2);
  fhEMCALDigits = (TH1I*)fOutputContainer->At(3);
  fhEMCALRecParticles = (TH1D*)fOutputContainer->At(4);
  fhEMCALPhotons = (TH1I*)fOutputContainer->At(5);
  fhEMCALInvariantMass = (TH1D*)fOutputContainer->At(6);
  fhEMCALDigitsEvent = (TH1I*)fOutputContainer->At(7);
 
  Bool_t problem = kFALSE ; 
  AliInfo(Form(" *** %s Report:", GetName())) ; 
  printf("        EMCALEnergy Mean        : %5.3f , RMS : %5.3f \n", fhEMCALEnergy->GetMean(),        fhEMCALEnergy->GetRMS()        ) ;
  printf("        EMCALDigits Mean        : %5.3f , RMS : %5.3f \n", fhEMCALDigits->GetMean(),        fhEMCALDigits->GetRMS()        ) ;
  printf("        EMCALRecParticles Mean  : %5.3f , RMS : %5.3f \n", fhEMCALRecParticles->GetMean(),  fhEMCALRecParticles->GetRMS()  ) ;
  printf("        EMCALPhotons Mean       : %5.3f , RMS : %5.3f \n", fhEMCALPhotons->GetMean(),       fhEMCALPhotons->GetRMS()       ) ;
  printf("        EMCALInvariantMass Mean : %5.3f , RMS : %5.3f \n", fhEMCALInvariantMass->GetMean(), fhEMCALInvariantMass->GetRMS() ) ;
  printf("        EMCALDigitsEvent Mean   : %5.3f , RMS : %5.3f \n", fhEMCALDigitsEvent->GetMean(),   fhEMCALDigitsEvent->GetRMS()   ) ;

  TCanvas  * cEMCAL = new TCanvas("EMCAL", "EMCAL ESD Test", 400, 10, 600, 700);
  cEMCAL->Divide(3, 2) ; 

  cEMCAL->cd(1) ; 
  if ( fhEMCALEnergy->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhEMCALEnergy->SetAxisRange(0, 25.);
  fhEMCALEnergy->SetXTitle("Energy (GeV)");
  fhEMCALEnergy->Draw();
  
  cEMCAL->cd(2) ; 
  if ( fhEMCALDigits->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhEMCALDigits->SetAxisRange(0, 25.);
  fhEMCALDigits->SetXTitle("DigitsPerCluster");
  fhEMCALDigits->Draw();
 
  cEMCAL->cd(3) ; 
  if ( fhEMCALRecParticles->GetMaximum() > 0. ) 
     gPad->SetLogy();
  fhEMCALRecParticles->SetAxisRange(0, 25.);
  fhEMCALRecParticles->SetXTitle("RecParticles");
  fhEMCALRecParticles->Draw();
 
  cEMCAL->cd(4) ; 
  if ( fhEMCALPhotons->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhEMCALPhotons->SetAxisRange(0, 25.);
  fhEMCALPhotons->SetXTitle("Photons");
  fhEMCALPhotons->Draw();
 
  cEMCAL->cd(5) ; 
  fhEMCALInvariantMass->SetXTitle("InvariantMass (MeV/c²)");
  fhEMCALInvariantMass->Draw();
 
  cEMCAL->cd(6) ; 
  if ( fhEMCALDigitsEvent->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhEMCALDigitsEvent->SetAxisRange(0, 40.);
  fhEMCALDigitsEvent->SetXTitle("DigitsPerEvent");
  fhEMCALDigitsEvent->Draw();
 
  cEMCAL->Print("EMCAL.eps");
 
  char line[1024] ; 
  sprintf(line, ".!tar -zcf %s.tar.gz *.eps", GetName()) ; 
  gROOT->ProcessLine(line);
  sprintf(line, ".!rm -fR *.eps"); 
  gROOT->ProcessLine(line);
 
  AliInfo(Form("!!! All the eps files are in %s.tar.gz !!!", GetName())) ;

  TString report ; 
  if(problem)
    report="Problems found, please check!!!";  
  else 
    report="OK";

  AliInfo(Form("*** %s Summary Report: %s \n",GetName(), report.Data())) ; 
}

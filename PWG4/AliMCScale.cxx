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

/* $Id: */

//_________________________________________________________________________
// A basic analysis task to analyse photon detected by PHOS
//
//*-- Yves Schutz ,modified by Yaxian(Add some histograms) 
//////////////////////////////////////////////////////////////////////////////

#include <TFile.h> 
#include <TH1.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TCanvas.h> 

#include "AliMCScale.h" 
#include "AliAnalysisManager.h"
#include "AliLog.h"

//______________________________________________________________________________
AliMCScale::AliMCScale() : 
  fDebug(0),
  fScale(1.0),
  fInputList(0x0), 
  fOutputList(0x0), 
  fhInPhotonEnergy(0),
  fhOuPhotonEnergy(0),
  fhInPhotonPt(0),
  fhOuPhotonPt(0),
  fhInPhotonEta(0),
  fhOuPhotonEta(0),
  fhInPhotonPhi(0),
  fhOuPhotonPhi(0)

{
  //Default constructor
}
//______________________________________________________________________________
AliMCScale::AliMCScale(const char *name) : 
  AliAnalysisTask(name,""),  
  fDebug(0),
  fScale(1.0), 
  fInputList(0x0), 
  fOutputList(0x0), 
  fhInPhotonEnergy(0),
  fhOuPhotonEnergy(0),
  fhInPhotonPt(0),
  fhOuPhotonPt(0),
  fhInPhotonEta(0),
  fhOuPhotonEta(0),
  fhInPhotonPhi(0),
  fhOuPhotonPhi(0)

{
  // Constructor.
  // Called only after the event loop
  SetPostEventLoop(kTRUE);
  // Input slot #0 
  DefineInput(0,  TList::Class()) ; 
  // Output slot 
  DefineOutput(0,  TList::Class()) ; 
}

//______________________________________________________________________________
AliMCScale::~AliMCScale()
{
  // dtor
  
}


//______________________________________________________________________________
void AliMCScale::ConnectInputData(const Option_t*)
{
  // Initialisation of branch container and histograms 
    
  AliInfo(Form("*** Initialization of %s", GetName())) ; 
  fInputList     = dynamic_cast<TList*>(GetInputData(0)) ;  
  fhInPhotonEnergy = dynamic_cast<TH1D*>(fInputList->At(0));
  fhInPhotonPt = dynamic_cast<TH1D*>(fInputList->At(1));
  fhInPhotonEta = dynamic_cast<TH1D*>(fInputList->At(2));
  fhInPhotonPhi = dynamic_cast<TH1D*>(fInputList->At(3));

}
//________________________________________________________________________
void AliMCScale::CreateOutputObjects()
{  
  // Create the outputs containers
  // Is created in Exec(), because the input must be available

}

//______________________________________________________________________________
void AliMCScale::Exec(Option_t *) 
{
  // Do the Scaling
    
  fhOuPhotonEnergy = static_cast<TH1D*>(fhInPhotonEnergy->Clone("PhotonEnergyScaled")) ;
  fhOuPhotonPt = static_cast<TH1D*>(fhInPhotonPt->Clone("PhotonPtScaled")) ;
  fhOuPhotonEta = static_cast<TH1D*>(fhInPhotonEta->Clone("PhotonEtaScaled")) ;
  fhOuPhotonPhi = static_cast<TH1D*>(fhInPhotonPhi->Clone("PhotonPhiScaled")) ;

  // create output container
  
  fOutputList = new TList() ; 
  fOutputList->SetName(GetName()) ; 

  fOutputList->AddAt(fhOuPhotonEnergy,        0) ;
  fhOuPhotonEnergy->Scale(fScale) ;
  fOutputList->AddAt(fhOuPhotonPt,            1) ;
  fhOuPhotonPt->Scale(fScale) ;
  fOutputList->AddAt(fhOuPhotonEta,           2) ;
  fhOuPhotonEta->Scale(fScale) ;
  fOutputList->AddAt(fhOuPhotonPhi,           3) ;
  fhOuPhotonPhi->Scale(fScale) ;

  PostData(0, fOutputList);
}


//______________________________________________________________________________
void AliMCScale::Init()
{
  // Intialisation of parameters
  AliInfo("Doing initialisation") ;
  // nothing to be done
}

//______________________________________________________________________________
void AliMCScale::Terminate(Option_t *)
{
  // Processing when the event loop is ended
  
  AliInfo(Form(" *** %s Report:", GetName())) ; 
  printf("        Photon Energy Integral In       : %5.3e \n", fhInPhotonEnergy->Integral() ) ;
  printf("        Photon Energy Integral Ou       : %5.3e \n", fhOuPhotonEnergy->Integral() ) ;
  printf("        Photon Pt Integral In           : %5.3e \n", fhInPhotonPt->Integral() ) ;
  printf("        Photon Pt Integral Ou           : %5.3e \n", fhOuPhotonPt->Integral() ) ;
  printf("        Photon Eta Integral In          : %5.3e \n", fhInPhotonEta->Integral() ) ;
  printf("        Photon Eta Integral Ou          : %5.3e \n", fhOuPhotonEta->Integral() ) ;
  printf("        Photon Phi Integral In          : %5.3e \n", fhInPhotonPhi->Integral() ) ;
  printf("        Photon Phi Integral Ou          : %5.3e \n", fhOuPhotonPhi->Integral() ) ;

  TCanvas  * cPHOS = new TCanvas("cPHOS", "PHOS ESD Test", 400, 10, 600, 700) ;

  cPHOS->Divide(2, 1);
  cPHOS->cd(1) ; 
  if ( fhInPhotonEnergy->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhInPhotonEnergy->SetAxisRange(0, 25.);
  fhInPhotonEnergy->SetLineColor(2);
  fhInPhotonEnergy->Draw();

  cPHOS->cd(2) ; 
  if ( fhOuPhotonEnergy->GetMaximum() > 0. ) 
    gPad->SetLogy();
  fhOuPhotonEnergy->SetAxisRange(0, 25.);
  fhOuPhotonEnergy->SetLineColor(2);
  fhOuPhotonEnergy->Draw();
}

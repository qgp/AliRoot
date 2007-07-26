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
 
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <Riostream.h>

#include "AliAnalysisTaskGamma.h"
#include "AliAnaGamma.h"
#include "AliGammaReader.h"
#include "AliESD.h"
#include "AliStack.h"
#include "AliLog.h"

ClassImp(AliAnalysisTaskGamma)

////////////////////////////////////////////////////////////////////////

AliAnalysisTaskGamma::AliAnalysisTaskGamma():
    fAna(0x0),
    fChain(0x0),
    fESD(0x0),
    fTreeG(0x0),
    fOutputContainer(0x0)
{
  // Default constructor
}

//_____________________________________________________
AliAnalysisTaskGamma::AliAnalysisTaskGamma(const char* name):
    AliAnalysisTaskRL(name, "AnalysisTaskGamma"),
    fAna(0x0),
    fChain(0x0),
    fESD(0x0),
    fTreeG(0x0),
    fOutputContainer(0x0)
{
  // Default constructor
  
  Init();
 
  DefineInput (0, TChain::Class());
  //DefineOutput(0, TTree::Class());// to create AODs, to be done
  DefineOutput(0, TList::Class());

}

//_____________________________________________________
AliAnalysisTaskGamma::~AliAnalysisTaskGamma() 
{

  // Remove all pointers
  fOutputContainer->Clear() ; 
  delete fOutputContainer ;

  delete fTreeG ; 
}

//_____________________________________________________
void AliAnalysisTaskGamma::CreateOutputObjects()
{
  // Create the output container
  //OpenFile(0);
  //fTreeG = new TTree ; // fAna->MakeTreeG("TreeG");// to create AODs, to be done
  
  OpenFile(0);
 
  fOutputContainer = fAna->GetOutputContainer();

}

//_____________________________________________________
void AliAnalysisTaskGamma::Init()
{
  // Initialization
  AliDebug(1,"Begin");
  
  // Call configuration file
  gROOT->LoadMacro("ConfigGammaAnalysis.C");
  fAna = (AliAnaGamma*) gInterpreter->ProcessLine("ConfigGammaAnalysis()");
  
  if(!fAna)
    AliFatal("Analysis pointer not initialized, abort analysis!");
  
  // Initialise Jet Analysis
  fAna->Init();
  
  AliDebug(1,"End");

}

//_____________________________________________________
void AliAnalysisTaskGamma::ConnectInputData(Option_t */*option*/)
{
  // Connect the input data
  //
  AliDebug(1,"ConnectInputData() ");
  fChain = (TChain*)GetInputData(0);
  
  char ** address = (char **)GetBranchAddress(0, "ESD");
  if (address)     {
    
    // Branch has been already connected
    fESD = (AliESD*)(*address);
  }
  else     {
    // First task taking the branch enables it
    fESD = new AliESD();
    SetBranchAddress(0, "ESD", &fESD);
  }
  
}

//_____________________________________________________
void AliAnalysisTaskGamma::Exec(Option_t */*option*/)
{
  // Execute analysis for current event
  //
  
  Long64_t ientry = fChain->GetReadEntry();
  if ( !((ientry)%100) ) 
    AliInfo(Form("Analysing event # %5d\n", (Int_t) ientry));
  
  if (!fESD) {
    AliError("fESD is not connected to the input!") ; 
    return ; 
  } 

  //Get the type of data, check if type is correct
  Int_t  datatype = fAna->GetReader()->GetDataType();
  if(datatype != AliGammaReader::kData && 
     datatype != AliGammaReader::kMC && 
     datatype != AliGammaReader::kMCData){
    AliFatal("Wrong type of data");
    return ;
  }
  
  //Pass ESD pointer to analysis    
  fAna->SetData(fESD);
  
  //In case of montecarlo analysis, pass the stack also.
  if(datatype == AliGammaReader::kMC || datatype == AliGammaReader::kMCData){
    if (AliAnalysisTaskRL::GetEntry(ientry) == kFALSE) {
      AliError("Couldn't get event from the runLoader\n");
      return;
    } 
    
    AliStack* stack = GetStack();
    if (!stack) {
      AliError("Stack not available");
    }
    
    //Pass Stack pointer to analysis    
    fAna -> SetKine(stack);
  }
  
  
  fAna->ProcessEvent(ientry);
  
  //PostData(0, fTreeG); // Create AODs, to be done.
  PostData(0, fOutputContainer);
  
}

//_____________________________________________________
void AliAnalysisTaskGamma::Terminate(Option_t */*option*/)
{
  // Terminate analysis
  //
  AliDebug(1,"Do nothing in Terminate");
 
}


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
//*--
//*-- Author: Gines Martinez & Yves Schutz (SUBATECH) 
//*-- Compleetely redesigned by Dmitri Peressounko (SUBATECH & RRC KI) March 2001
/////////////////////////////////////////////////////////////////////////////////////
//  Wrapping class for reconstruction. Allows to produce reconstruction from 
//  different steps: from previously produced hits,sdigits, etc. Each new reconstruction
//  flow (e.g. digits, made from them RecPoints, subsequently made TrackSegments, 
//  subsequently made RecParticles) are distinguished by the title of created branches. One can 
//  use this title as a comment, see use case below. 
//  Thanks to getters, one can set 
//  parameters to reconstruction briks. The full set of parameters is saved in the 
//  corresponding branch: e.g. parameters of clusterizer are stored in branch 
//  TreeR::AliPHOSClusterizer with the same title as the branch containing the RecPoints. //  TTree does not support overwriting, therefore one can not produce several 
//  branches with the same names and titles - use different titles.
//
//  Use case: 
//
//  root [0] AliPHOSReconstructioner * r = new AliPHOSReconstructioner("galice.root")
//              //  Set the header file
//  root [1] r->ExecuteTask() 
//              //  Make full chain of reconstruction
//
//              // One can specify the title for each branch 
//  root [2] r->SetBranchFileName("RecPoints","RecPoints1") ;
//      
//             // One can change parameters of reconstruction algorithms
//  root [3] r->GetClusterizer()->SetEmcLocalMaxCut(0.02)
//
//             // One can specify the starting point of the reconstruction and title of all 
//             // branches produced in this pass
//  root [4] r->StartFrom("AliPHOSClusterizer","Local max cut 0.02") 
//             // means that will use already generated Digits and produce only RecPoints, 
//             // TS and RecParticles 
//
//             // And finally one can call ExecuteTask() with the following options
//  root [5] r->ExecuteTask("debug all timing")
//             // deb     - prints the numbers of produced SDigits, Digits etc.
//             // deb all - prints in addition list of made SDigits, digits etc.
//             // timing  - prints benchmarking results
///////////////////////////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---

#include "TClonesArray.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

// --- Standard library ---

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliPHOSReconstructioner.h"
#include "AliPHOSClusterizerv1.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSTrackSegmentMakerv1.h"
#include "AliPHOSPIDv1.h"
#include "AliPHOSFastRecParticle.h"
#include "AliPHOSCpvRecPoint.h"
#include "AliPHOSLoader.h"

ClassImp(AliPHOSReconstructioner)

//____________________________________________________________________________
  AliPHOSReconstructioner::AliPHOSReconstructioner():TTask("AliPHOSReconstructioner","")
{
  // ctor
  fDigitizer   = 0 ;
  fClusterizer = 0 ;
  fTSMaker     = 0 ;
  fPID         = 0 ; 
  fSDigitizer  = 0 ;

  fIsInitialized = kFALSE ;

} 

//____________________________________________________________________________
AliPHOSReconstructioner::AliPHOSReconstructioner(const char* evFoldName,const char * branchName):
TTask("AliPHOSReconstructioner",evFoldName)
{
  // ctor
  AliRunLoader* rl = AliRunLoader::GetRunLoader(evFoldName);
  if (rl == 0x0)
   {
     Fatal("AliPHOSReconstructioner","Can not get Run Loader from folder %s.",evFoldName);
   } 
  if (rl->GetAliRun() == 0x0)
   {
     delete gAlice;
     gAlice = 0x0;
     rl->LoadgAlice();
     gAlice = rl->GetAliRun();
   }

  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rl->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("AliPHOSReconstructioner","Can not get PHOS Loader");
     return;  
   }
  
  TString method("AliPHOSReconstructioner::AliPHOSReconstructioner(");
  method = (((method + evFoldName)+",")+branchName)+"): ";
  
  fSDigitsBranch= branchName; 
  
  cout<<"\n\n\n";
  cout<<method<<"\n\nCreating SDigitizer\n";
  fSDigitizer  = new AliPHOSSDigitizer(GetTitle(),"PHOSSDigitizer");
  Add(fSDigitizer);
  gime->PostSDigitizer(fSDigitizer);

  fDigitsBranch=branchName ;
  cout<<"\n\n\n";
  cout<<method<<"\n\nCreating Digitizer\n";
  fDigitizer   = new AliPHOSDigitizer(GetTitle(),"PHOSDigitizer") ;
  Add(fDigitizer) ;
  gime->PostDigitizer(fDigitizer);

  fRecPointBranch=branchName ; 
  cout<<"\n\n\n";
  cout<<method<<"Creating Clusterizer\n";
  fClusterizer = new AliPHOSClusterizerv1(GetTitle(),"PHOSReconstructioner");
  Add(fClusterizer);
  gime->PostReconstructioner(fClusterizer);
  
  fTSBranch=branchName ; 
  fTSMaker     = new AliPHOSTrackSegmentMakerv1(GetTitle(),"PHOSTracker");
  Add(fTSMaker) ;
  gime->PostTracker(fTSMaker);

  
  fRecPartBranch=branchName ; 
  cout<<"\n\n\n";
  cout<<method<<"Creating PID\n";
  fPID         = new AliPHOSPIDv1(GetTitle(),"PHOSPID");
  Add(fPID);
  cout<<"\nFINISHED \n\n"<<method;
  
  fIsInitialized = kTRUE ;
} 
//____________________________________________________________________________
void AliPHOSReconstructioner::Exec(Option_t *option)
{
  //chesk, if the names of branches, which should be made conicide with already
  //existing
  if(!fIsInitialized)
    Init() ;
}
//____________________________________________________________________________
 void AliPHOSReconstructioner::Init()
{
  // initiliaze Reconstructioner if necessary: we can not do this in default constructor

  if(!fIsInitialized){
    // Initialisation

    fSDigitsBranch="Default" ; 
    fSDigitizer  = new AliPHOSSDigitizer(GetTitle(),fSDigitsBranch.Data()) ; 
    Add(fSDigitizer) ;

    fDigitsBranch="Default" ; 
    fDigitizer   = new AliPHOSDigitizer(GetTitle(),fDigitsBranch.Data());
    Add(fDigitizer) ;

    fRecPointBranch="Default" ; 
    fClusterizer = new AliPHOSClusterizerv1(GetTitle(),fRecPointBranch.Data());
    Add(fClusterizer) ;

    fTSBranch="Default" ; 
    fTSMaker     = new AliPHOSTrackSegmentMakerv1(GetTitle(),fTSBranch.Data());
    Add(fTSMaker) ;


    fRecPartBranch="Default"; 
    fPID         = new AliPHOSPIDv1(GetTitle(),fRecPartBranch.Data()) ;
    Add(fPID) ;
    
    fIsInitialized = kTRUE ;
  }
} 
//____________________________________________________________________________
AliPHOSReconstructioner::~AliPHOSReconstructioner()
{
  // Delete data members if any
} 

void AliPHOSReconstructioner::Print(Option_t * option)const {
  // Print reconstructioner data  

  TString message ; 
  message  = "-----------------AliPHOSReconstructioner---------------\n" ;
  message += " Reconstruction of the header file %s\n" ;
  message += " with the following modules:\n" ;

  if(fSDigitizer->IsActive()){
    message += "   (+)   %s to branch %s\n" ; 
  }
  if(fDigitizer->IsActive()){
    message += "   (+)   %s to branch %s\n" ; 
  }
  
  if(fClusterizer->IsActive()){
    message += "   (+)   %s to branch %s\n" ;
  }

  if(fTSMaker->IsActive()){
    message += "   (+)   %s to branch %s\n" ; 
  }

  if(fPID->IsActive()){
    message += "   (+)   %s to branch %s\n" ;  
  }
  Info("Print", message.Data(), 
       GetTitle(), 
       fSDigitizer->GetName(), fSDigitsBranch.Data(), 
       fDigitizer->GetName(), fDigitsBranch.Data() , 
       fClusterizer->GetName(), fRecPointBranch.Data(), 
       fTSMaker->GetName(), fTSBranch.Data() , 
       fPID->GetName(), fRecPartBranch.Data() ) ; 
}

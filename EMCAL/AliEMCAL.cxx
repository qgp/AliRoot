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
// Base Class for EMCAL description:
// This class contains material definitions    
// for the EMCAL - It does not place the detector in Alice
//*-- Author: Yves Schutz (SUBATECH) 
//
//*-- Additional Contributions: Sahal Yacoob (LBNL/UCT)
//
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---
class TFile;
#include <TFolder.h> 
#include <TTree.h>
#include <TVirtualMC.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliMagF.h"
#include "AliESDCaloTrack.h" 
#include "AliESD.h"
#include "AliEMCAL.h"
#include "AliEMCALGetter.h"
#include "AliRun.h"
#include "AliEMCALSDigitizer.h"
#include "AliEMCALDigitizer.h"
#include "AliEMCALClusterizerv1.h"
#include "AliEMCALTrackSegmentMakerv1.h"
#include "AliEMCALPIDv1.h"

ClassImp(AliEMCAL)
//____________________________________________________________________________
AliEMCAL::AliEMCAL():AliDetector()
{
  // Default ctor 
  fName="EMCAL";
  fGeom = 0 ; 
}

//____________________________________________________________________________
AliEMCAL::AliEMCAL(const char* name, const char* title): AliDetector(name,title)
{
  //   ctor : title is used to identify the layout
  fGeom = 0;
}

//____________________________________________________________________________
AliEMCAL::~AliEMCAL()
{

}

//____________________________________________________________________________
void AliEMCAL::Copy(AliEMCAL & emcal)
{
  TObject::Copy(emcal) ; 
}

//____________________________________________________________________________
AliDigitizer* AliEMCAL::CreateDigitizer(AliRunDigitizer* manager) const
{
  return new AliEMCALDigitizer(manager);
}

//____________________________________________________________________________
void AliEMCAL::CreateMaterials()
{
  // Definitions of materials to build EMCAL and associated tracking media.
  // media number in idtmed are 1599 to 1698.

  // --- Air ---
  AliMaterial(0, "Air$", 14.61, 7.3, 0.001205, 30420., 67500., 0, 0) ;


  // --- Lead ---                                                                     
  AliMaterial(1, "Pb$", 207.2, 82, 11.35, 0.56, 0., 0, 0) ;


  // --- The polysterene scintillator (CH) ---
  Float_t aP[2] = {12.011, 1.00794} ;
  Float_t zP[2] = {6.0, 1.0} ;
  Float_t wP[2] = {1.0, 1.0} ;
  Float_t dP = 1.032 ;

  AliMixture(2, "Polystyrene$", aP, zP, dP, -2, wP) ;

  // --- Aluminium ---
  AliMaterial(3, "Al$", 26.98, 13., 2.7, 8.9, 999., 0, 0) ;
  // ---         Absorption length is ignored ^

  // DEFINITION OF THE TRACKING MEDIA

  // for EMCAL: idtmed[1599->1698] equivalent to fIdtmed[0->100]
  Int_t * idtmed = fIdtmed->GetArray() - 1599 ; 
  Int_t   isxfld = gAlice->Field()->Integ() ;
  Float_t sxmgmx = gAlice->Field()->Max() ;
 


   // Air                                                                           -> idtmed[1599] 
  AliMedium(0, "Air          $", 0, 0,
	     isxfld, sxmgmx, 10.0, 1.0, 0.1, 0.1, 10.0, 0, 0) ;

  // The Lead                                                                      -> idtmed[1600]
 
  AliMedium(1, "Lead      $", 1, 0,
	     isxfld, sxmgmx, 10.0, 0.1, 0.1, 0.1, 0.1, 0, 0) ;

 // The scintillator of the CPV made of Polystyrene scintillator                   -> idtmed[1601]
  AliMedium(2, "CPV scint.   $", 2, 1,
            isxfld, sxmgmx, 10.0, 0.001, 0.1, 0.001, 0.001, 0, 0) ;

  // Various Aluminium parts made of Al                                            -> idtmed[1602]
  AliMedium(3, "Al parts     $", 3, 0,
             isxfld, sxmgmx, 10.0, 0.1, 0.1, 0.001, 0.001, 0, 0) ;


// --- Set decent energy thresholds for gamma and electron tracking

  // Tracking threshold for photons and electrons in Lead 
  gMC->Gstpar(idtmed[1600],"CUTGAM",0.00008) ;
  gMC->Gstpar(idtmed[1600],"CUTELE",0.001) ;
  gMC->Gstpar(idtmed[1600],"BCUTE",0.0001) ;

  // --- Generate explicitly delta rays in Lead ---
  gMC->Gstpar(idtmed[1600], "LOSS",3.) ;
  gMC->Gstpar(idtmed[1600], "DRAY",1.) ;
  gMC->Gstpar(idtmed[1600], "DCUTE",0.00001) ;
  gMC->Gstpar(idtmed[1600], "DCUTM",0.00001) ;

// --- in aluminium parts ---
  gMC->Gstpar(idtmed[1602], "LOSS",3.) ;
  gMC->Gstpar(idtmed[1602], "DRAY",1.) ;
  gMC->Gstpar(idtmed[1602], "DCUTE",0.00001) ;
  gMC->Gstpar(idtmed[1602], "DCUTM",0.00001) ;

// --- and finally thresholds for photons and electrons in the scintillator ---
  gMC->Gstpar(idtmed[1601],"CUTGAM",0.00008) ;
  gMC->Gstpar(idtmed[1601],"CUTELE",0.001) ;
  gMC->Gstpar(idtmed[1601],"BCUTE",0.0001) ;

}

//____________________________________________________________________________
void AliEMCAL::FillESD(AliESD* esd) const 
{

  // Called by AliReconstruct after Reconstruct() and global tracking and vertxing 
  //Creates the tracksegments and Recparticles
    
  AliRunLoader * runLoader = AliRunLoader::GetRunLoader() ; 
  Int_t eventNumber = runLoader->GetEventNumber() ;

  TString headerFile(runLoader->GetFileName()) ; 
  TString branchName(runLoader->GetEventFolder()->GetName()) ;  

  AliEMCALPIDv1 pid(headerFile, branchName);

  // do current event; the loop over events is done by AliReconstruction::Run()
  pid.SetEventRange(eventNumber, eventNumber) ; 
  if ( GetDebug() ) 
   pid.ExecuteTask("deb all") ;
  else 
    pid.ExecuteTask("") ;
  
  // Creates AliESDtrack from AliEMCALRecParticles 
  AliEMCALGetter::Instance()->Event(eventNumber, "P") ; 
  TClonesArray *recParticles = AliEMCALGetter::Instance()->RecParticles();
  Int_t nOfRecParticles = recParticles->GetEntries();
  for (Int_t recpart = 0 ; recpart < nOfRecParticles ; recpart++) {
    AliEMCALRecParticle * rp = dynamic_cast<AliEMCALRecParticle*>(recParticles->At(recpart));
    if (GetDebug()) 
      rp->Print();
    AliESDtrack * et = new AliESDtrack() ; 
    // fills the ESDtrack
    Double_t xyz[3];
    for (Int_t ixyz=0; ixyz<3; ixyz++) 
      xyz[ixyz] = rp->GetPos()[ixyz];
    //et->SetEMCALposition(xyz) ; 
    //et->SetEMCALsignal  (rp->Energy()) ; 
    //et->SetEMCALpid     (rp->GetPID()) ;
    // add the track to the esd object
    esd->AddTrack(et);
    delete et;
  }
}

//____________________________________________________________________________
void AliEMCAL::Hits2SDigits()  
{ 
// create summable digits

  AliEMCALSDigitizer* emcalDigitizer = 
    new AliEMCALSDigitizer(fLoader->GetRunLoader()->GetFileName().Data()) ;
  emcalDigitizer->SetEventRange(0, -1) ; // do all the events
  emcalDigitizer->ExecuteTask() ;
}

//____________________________________________________________________________
void AliEMCAL::Reconstruct() const 
{ 
   // method called by AliReconstruction; 
  // Only the clusterization is performed,; the rest of the reconstruction is done in FillESD because the track
  // segment maker needs access to the AliESD object to retrieve the tracks reconstructed by 
  // the global tracking.
 
  AliRunLoader * runLoader = AliRunLoader::GetRunLoader() ; 
  TString headerFile(runLoader->GetFileName()) ; 
  TString branchName(runLoader->GetEventFolder()->GetName()) ;  
  
  AliEMCALClusterizerv1 clu(headerFile, branchName);
  clu.SetEventRange(0, -1) ; // do all the events
  if ( GetDebug() ) 
    clu.ExecuteTask("deb all") ; 
  else 
    clu.ExecuteTask("") ;  

}

//____________________________________________________________________________
AliLoader* AliEMCAL::MakeLoader(const char* topfoldername)
{
//different behaviour than standard (singleton getter)
// --> to be discussed and made eventually coherent
 fLoader = new AliEMCALLoader(GetName(),topfoldername);
 return fLoader;
}

//____________________________________________________________________________
void AliEMCAL::SetTreeAddress()
{ 
  // Linking Hits in Tree to Hits array
  TBranch *branch;
  char branchname[20];
  sprintf(branchname,"%s",GetName());
  
  // Branch address for hit tree
  TTree *treeH = TreeH();
  if (treeH) {
    branch = treeH->GetBranch(branchname);
    if (branch) 
      { 
	if (fHits == 0x0) 
	  fHits= new TClonesArray("AliEMCALHit",1000);
	branch->SetAddress(&fHits);
      }
    else
      {
	Warning("SetTreeAddress","<%s> Failed",GetName());
      }
  }
}






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
// Implementation version v0 of PHOS Manager class 
// Layout EMC + PPSD has name GPS2  
// The main goal of this version of AliPHOS is to calculte the 
//  induced charged in the PIN diode, taking into account light
//  tracking in the PbWO4 crystal, induced signal in the 
//  PIN due to MIPS particle and electronic noise.
// This is done in the StepManager 
//                  
//*-- Author:  Odd Harald Odland & Gines Martinez (SUBATECH)


// --- ROOT system ---
#include "TRandom.h"

// --- Standard library ---

#include <string.h>
#include <stdlib.h>
#include <strstream.h>

// --- AliRoot header files ---

#include "AliPHOSv3.h"
#include "AliPHOSHit.h"
#include "AliPHOSCPVDigit.h"
#include "AliRun.h"
#include "AliConst.h"
#include "AliMC.h"
#include "AliPHOSGeometry.h"

ClassImp(AliPHOSv3)

//____________________________________________________________________________
  AliPHOSv3::AliPHOSv3(void) : AliPHOSv1() 
{
  // default ctor: initialize daa members

  fLightYieldMean         = 0. ;         
  fIntrinsicPINEfficiency = 0. ; 
  fLightYieldAttenuation  = 0. ;  
  fRecalibrationFactor    = 0. ;    
  fElectronsPerGeV        = 0. ;
  fAPDGain                = 0. ;    
  
}

//____________________________________________________________________________
  AliPHOSv3::AliPHOSv3(const char *name, const char *title):
AliPHOSv1(name,title)
{
  // ctor 

  // The light yield is a poissonian distribution of the number of
  // photons created in the PbWo4 crystal, calculated using following formula
  // NumberOfPhotons = EnergyLost * LightYieldMean* APDEfficiency * 
  //              exp (-LightYieldAttenuation * DistanceToPINdiodeFromTheHit);
  // LightYieldMean is parameter calculated to be over 47000 photons per GeV  
  // APDEfficiency is 0.02655
  // k_0 is 0.0045 from Valery Antonenko 
  // The number of electrons created in the APD is 
  // NumberOfElectrons = APDGain * LightYield
  // The APD Gain is 300

  fLightYieldMean = 47000;
  fIntrinsicPINEfficiency = 0.02655 ; //APD= 0.1875/0.1271 * 0.018 (PIN)
  fLightYieldAttenuation = 0.0045 ; 
  fRecalibrationFactor = 13.418/ fLightYieldMean ;
  fElectronsPerGeV = 2.77e+8 ;
  fAPDGain= 300. ;
}

// //____________________________________________________________________________
// AliPHOSv3::AliPHOSv3(AliPHOSReconstructioner * Reconstructioner, const char *name, const char *title):
//   AliPHOSv1(Reconstructioner,name,title)
// {
//   // ctor 

//   // Number of electrons created in the PIN due to light collected in the PbWo4 crystal is calculated using 
//   // following formula
//   // NumberOfElectrons = EnergyLost * LightYield * PINEfficiency * 
//   //                     exp (-LightYieldAttenuation * DistanceToPINdiodeFromTheHit) *
//   //                     RecalibrationFactor ;
//   // LightYield is obtained as a Poissonian distribution with a mean at 700000 photons per GeV fromValery Antonenko
//   // PINEfficiency is 0.1875 
//   // k_0 is 0.0045 from Valery Antonenko 

//   fLightYieldMean = 700000.;
//   fIntrinsicPINEfficiency = 0.1875 ;
//   fLightYieldAttenuation = 0.0045 ;
//   fRecalibrationFactor = 6.2 / fLightYieldMean ;
//   fRecalibrationFactor = 5.67 /fLightYieldMean ;//25.04.2001 OHO
//   fElectronsPerGeV = 2.77e+8 ;
// }
//____________________________________________________________________________

void AliPHOSv3::StepManager(void)
{
  // Accumulates hits as long as the track stays in a single crystal or PPSD gas Cell

//    if (gMC->IsTrackEntering())
//      Info("StepManager", "Track enters the volume %d", gMC->CurrentVolName()) ;
//    if (gMC->IsTrackExiting())
//      Info("StepManager", "Track leaves the volume %d", gMC->CurrentVolName()) ;

  Int_t          relid[4] ;        // (box, layer, row, column) indices
  Int_t          absid    ;        // absolute cell ID number
  Float_t        xyze[4]={0,0,0,0}  ; // position wrt MRS and energy deposited

  TLorentzVector pos      ;     // Lorentz vector of the track current position
  Int_t          copy     ;
  Float_t        fLightYield ;   // Light Yield per GeV

  Int_t tracknumber =  gAlice->CurrentTrack() ; 
  Int_t primary     =  gAlice->GetPrimary( gAlice->CurrentTrack() ); 
  TString name      =  GetGeometry()->GetName() ; 
  Float_t        lostenergy ;
  Float_t        global[3] ;
  Float_t        local[3] ;


  if ( name == "GPS2" || name == "MIXT" ) {            // ======> CPV is a GPS' PPSD

    if( gMC->CurrentVolID(copy) == gMC->VolId("PPCE") ) // We are inside a gas cell 
    {
      gMC->TrackPosition(pos) ;
      xyze[0] = pos[0] ;
      xyze[1] = pos[1] ;
      xyze[2] = pos[2] ;
      xyze[3] = gMC->Edep() ; 

      if ( xyze[3] != 0) { // there is deposited energy 
       	gMC->CurrentVolOffID(5, relid[0]) ;  // get the PHOS Module number
	if ( name == "MIXT" && strcmp(gMC->CurrentVolOffName(5),"PHO1") == 0 ){
	  relid[0] += GetGeometry()->GetNModules() - GetGeometry()->GetNPPSDModules();
	}
       	gMC->CurrentVolOffID(3, relid[1]) ;  // get the Micromegas Module number 
      // 1-> GetGeometry()->GetNumberOfModulesPhi() * GetGeometry()->GetNumberOfModulesZ() upper
      //   > GetGeometry()->GetNumberOfModulesPhi() * GetGeometry()->GetNumberOfModulesZ() lower
       	gMC->CurrentVolOffID(1, relid[2]) ;  // get the row number of the cell
        gMC->CurrentVolID(relid[3]) ;        // get the column number 

	// get the absolute Id number

       	GetGeometry()->RelToAbsNumbering(relid, absid) ; 

	// add current hit to the hit list      
	  AddHit(fIshunt, primary, tracknumber, absid, xyze);


      } // there is deposited energy 
    } // We are inside the gas of the CPV  
  } // GPS2 configuration

  if ( name == "IHEP" || name == "MIXT" ) {       // ======> CPV is a IHEP's one

    // Yuri Kharlov, 28 September 2000

    if( gMC->CurrentVolID(copy) == gMC->VolId("PCPQ") &&
	gMC->IsTrackEntering()  &&
	gMC->TrackCharge() != 0) {      

      gMC -> TrackPosition(pos);
      Float_t xyzm[3], xyzd[3] ;
      Int_t i;
      for (i=0; i<3; i++) xyzm[i] = pos[i];
      gMC -> Gmtod (xyzm, xyzd, 1);    // transform coordinate from master to daughter system

      Float_t        xyd[3]={0,0,0}   ;   //local posiiton of the entering
      xyd[0]  = xyzd[0];
      xyd[1]  =-xyzd[1];
      xyd[2]  =-xyzd[2];

      
      // Current momentum of the hit's track in the local ref. system
        TLorentzVector pmom     ;        //momentum of the particle initiated hit
      gMC -> TrackMomentum(pmom);
      Float_t pm[3], pd[3];
      for (i=0; i<3; i++) pm[i]   = pmom[i];
      gMC -> Gmtod (pm, pd, 2);        // transform 3-momentum from master to daughter system
      pmom[0] = pd[0];
      pmom[1] =-pd[1];
      pmom[2] =-pd[2];
      
      // Digitize the current CPV hit:

      // 1. find pad response and
      
      Int_t moduleNumber;
      gMC->CurrentVolOffID(3,moduleNumber);
      moduleNumber--;


      TClonesArray *cpvDigits = new TClonesArray("AliPHOSCPVDigit",0);   // array of digits for current hit
      CPVDigitize(pmom,xyd,moduleNumber,cpvDigits);
      
      Float_t xmean = 0;
      Float_t zmean = 0;
      Float_t qsum  = 0;
      Int_t   idigit,ndigits;

      // 2. go through the current digit list and sum digits in pads

      ndigits = cpvDigits->GetEntriesFast();
      for (idigit=0; idigit<ndigits-1; idigit++) {
	AliPHOSCPVDigit  *cpvDigit1 = dynamic_cast<AliPHOSCPVDigit*>(cpvDigits->UncheckedAt(idigit));
	Float_t x1 = cpvDigit1->GetXpad() ;
	Float_t z1 = cpvDigit1->GetYpad() ;
	for (Int_t jdigit=idigit+1; jdigit<ndigits; jdigit++) {
	  AliPHOSCPVDigit  *cpvDigit2 = dynamic_cast<AliPHOSCPVDigit*>(cpvDigits->UncheckedAt(jdigit));
	  Float_t x2 = cpvDigit2->GetXpad() ;
	  Float_t z2 = cpvDigit2->GetYpad() ;
	  if (x1==x2 && z1==z2) {
	    Float_t qsum = cpvDigit1->GetQpad() + cpvDigit2->GetQpad() ;
	    cpvDigit2->SetQpad(qsum) ;
	    cpvDigits->RemoveAt(idigit) ;
	  }
	}
      }
      cpvDigits->Compress() ;

      // 3. add digits to temporary hit list fTmpHits

      ndigits = cpvDigits->GetEntriesFast();
      for (idigit=0; idigit<ndigits; idigit++) {
	AliPHOSCPVDigit  *cpvDigit = dynamic_cast<AliPHOSCPVDigit*>(cpvDigits->UncheckedAt(idigit));
	relid[0] = moduleNumber + 1 ;                             // CPV (or PHOS) module number
	relid[1] =-1 ;                                            // means CPV
	relid[2] = cpvDigit->GetXpad() ;                          // column number of a pad
	relid[3] = cpvDigit->GetYpad() ;                          // row    number of a pad
	
	// get the absolute Id number
	GetGeometry()->RelToAbsNumbering(relid, absid) ; 

	// add current digit to the temporary hit list
	xyze[0] = 0. ;
	xyze[1] = 0. ;
	xyze[2] = 0. ;
	xyze[3] = cpvDigit->GetQpad() ;                           // amplitude in a pad
	primary = -1;                                             // No need in primary for CPV
	AddHit(fIshunt, primary, tracknumber, absid, xyze);

	if (cpvDigit->GetQpad() > 0.02) {
	  xmean += cpvDigit->GetQpad() * (cpvDigit->GetXpad() + 0.5);
	  zmean += cpvDigit->GetQpad() * (cpvDigit->GetYpad() + 0.5);
	  qsum  += cpvDigit->GetQpad();
	}
      }
      delete cpvDigits;
    }
  } // end of IHEP configuration
  

if(gMC->CurrentVolID(copy)==gMC->VolId("PXTL")){// We are inside a PBWO4 crystal
    gMC->TrackPosition(pos) ;
    xyze[0] = pos[0] ;
    xyze[1] = pos[1] ;
    xyze[2] = pos[2] ;
    global[0] = pos[0] ;
    global[1] = pos[1] ;
    global[2] = pos[2] ;
    lostenergy = gMC->Edep(); 
    xyze[3] = gMC->Edep() ;

  
  if ( (xyze[3] != 0) ){//Track is inside the crystal and deposits some energy

      gMC->CurrentVolOffID(10, relid[0]) ; // get the PHOS module number ;

      if ( name == "MIXT" && strcmp(gMC->CurrentVolOffName(10),"PHO1") == 0 )
	relid[0] += GetGeometry()->GetNModules() - GetGeometry()->GetNPPSDModules();      

      relid[1] = 0   ;                    // means PBW04
   gMC->CurrentVolOffID(4, relid[2]) ; // get the row number inside the module
   gMC->CurrentVolOffID(3, relid[3]) ; // get the cell number inside the module
      
      // get the absolute Id number

   GetGeometry()->RelToAbsNumbering(relid, absid) ; 

   gMC->Gmtod(global, local, 1) ;

   //Calculates the light yield, the number of photns produced in the
   //crystal 
   fLightYield = gRandom->Poisson(
				  fLightYieldMean * lostenergy *
				  fIntrinsicPINEfficiency * 
				  exp(-fLightYieldAttenuation *
				      (local[1]+GetGeometry()->GetCrystalSize(1)/2.0 ))
				  ) ;
   //Calculates de energy deposited in the crystal  
   xyze[3] = (fRecalibrationFactor/100.) * fAPDGain * fLightYield  ;
  
    
   // Info("StepManager", "xyze[3]: %f", xyze[3]) ;
   // Info("StepManager", "lostenergy: %f", lostenergy) ; 


        

      // add current hit to the hit list
 

   if (xyze[3] != 0.) AddHit(fIshunt, primary,tracknumber, absid, xyze);
  

    } // there is deposited energy
  } // we are inside a PHOS Xtal

//  if(gMC->CurrentVolID(copy) == gMC->VolId("PPIN"))//We are inside the PIN diode 
//     {
//       Info("StepManager", "Inside PIN";
//       gMC->TrackPosition(pos) ;
//       global[0] = pos[0] ;
//       global[1] = pos[1] ;
//       global[2] = pos[2] ;
//       xyze[0] = pos[0] ;
//       xyze[1] = pos[1] ;
//       xyze[2] = pos[2] ;
//       lostenergy = gMC->Edep() ;
//       xyze[3] = gMC->Edep() ;
      
//       if ( xyze[3] != 0 ) {
// 	gMC->CurrentVolOffID(11, relid[0]) ; // get the PHOS module number ;
// 	relid[1] = 0   ;                    // means PW04 and PIN
// 	gMC->CurrentVolOffID(5, relid[2]) ; // get the row number inside the module
// 	gMC->CurrentVolOffID(4, relid[3]) ; // get the cell number inside the module
	
// 	// get the absolute Id number
	
// 	Int_t absid ; 
// 	GetGeometry()->RelToAbsNumbering(relid,absid) ;
// 	gMC->Gmtod(global, local, 1) ;

// // calculating number of electrons in the PIN diode asociated to this hit

// 	  //nElectrons = lostenergy * fElectronsPerGeV ;
//        //  xyze[3] = nElectrons * fRecalibrationFactor ;
//           apdgain = gRandom->Poisson(300.) ;
//           // apdgain = 300.;
//    //if(local[1]<-0.0045) xyze[3] = apdgain * nElectrons * fRecalibrationFactor/10000.;

//     if(local[1]<-0.0045) xyze[3] = apdgain * lostenergy * fElectronsPerGeV* (fRecalibrationFactor/100.);

//    //if((local[1]>-0.0045)&&(gMC->TrackPid()==-11)) xyze[3] = apdgain * nElectrons * fRecalibrationFactor/10000.;

//      if((local[1]>-0.0045)&&(gMC->TrackPid()==-11)) xyze[3] = apdgain * lostenergy * fElectronsPerGeV * (fRecalibrationFactor/100.);

//    //if(local[1]>-0.0045) xyze[3] = nElectrons * fRecalibrationFactor/10000.;

//      if(local[1]>-0.0045) xyze[3] = lostenergy * fElectronsPerGeV * (fRecalibrationFactor/100.);
	  
// 	  // add current hit to the hit list

// 	  AddHit(fIshunt, primary, tracknumber, absid, xyze);

// 	  //printf("PIN volume is  %d, %d, %d, %d \n",relid[0],relid[1],relid[2],relid[3]);
// 	  printf("Lost energy in the PIN is %f \n",lostenergy) ;
//       } // there is deposited energy
//     } // we are inside a PHOS XtalPHOS PIN diode
}

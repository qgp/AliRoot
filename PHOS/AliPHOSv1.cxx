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
// Implementation version v1 of PHOS Manager class 
//---
// Layout EMC + PPSD has name GPS2:
// Produces cumulated hits
//---
// Layout EMC + CPV  has name IHEP:
// Produces hits for CPV, cumulated hits
//---
// Layout EMC + CPV + PPSD has name GPS:
// Produces hits for CPV, cumulated hits
//---
//*-- Author: Yves Schutz (SUBATECH)


// --- ROOT system ---

#include "TBRIK.h"
#include "TNode.h"
#include "TRandom.h"
#include "TTree.h"
#include "TParticle.h"

// --- Standard library ---

#include <string.h>
#include <stdlib.h>
#include <strstream.h>

// --- AliRoot header files ---

#include "AliPHOSv1.h"
#include "AliPHOSHit.h"
#include "AliPHOSCPVDigit.h"
#include "AliRun.h"
#include "AliConst.h"
#include "AliMC.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSQAIntCheckable.h"
#include "AliPHOSQAFloatCheckable.h"
#include "AliPHOSQAMeanChecker.h"

ClassImp(AliPHOSv1)

//____________________________________________________________________________
AliPHOSv1::AliPHOSv1():
AliPHOSv0()
{
  // default ctor: initialze data memebers
  fQAHitsMul  = 0 ;
  fQAHitsMulB = 0 ; 
  fQATotEner  = 0 ; 
  fQATotEnerB = 0 ; 

  fLightYieldMean         = 0. ;         
  fIntrinsicPINEfficiency = 0. ; 
  fLightYieldAttenuation  = 0. ;  
  fRecalibrationFactor    = 0. ;    
  fElectronsPerGeV        = 0. ;
  fAPDGain                = 0. ;  
  fLightFactor            = 0. ; 
  fAPDFactor              = 0. ; 

}

//____________________________________________________________________________
AliPHOSv1::AliPHOSv1(const char *name, const char *title):
 AliPHOSv0(name,title) 
{
  //
  // We store hits :
  //   - fHits (the "normal" one), which retains the hits associated with
  //     the current primary particle being tracked
  //     (this array is reset after each primary has been tracked).
  //



  // We do not want to save in TreeH the raw hits
  // But save the cumulated hits instead (need to create the branch myself)
  // It is put in the Digit Tree because the TreeH is filled after each primary
  // and the TreeD at the end of the event (branch is set in FinishEvent() ). 
  
  fHits= new TClonesArray("AliPHOSHit",1000) ;
  gAlice->AddHitList(fHits) ; 

  fNhits = 0 ;

  fIshunt     =  2 ; // All hits are associated with primary particles

  //Photoelectron statistics:
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
  fLightYieldAttenuation  = 0.0045 ; 
  fRecalibrationFactor    = 13.418/ fLightYieldMean ;
  fElectronsPerGeV        = 2.77e+8 ;
  fAPDGain                = 300. ;
  fLightFactor            = fLightYieldMean * fIntrinsicPINEfficiency ; 
  fAPDFactor              = (fRecalibrationFactor/100.) * fAPDGain ; 


  Int_t nb   = GetGeometry()->GetNModules() ; 
  
  // create checkables 
  fQAHitsMul   = new AliPHOSQAIntCheckable("HitsM") ; 
  fQATotEner   = new AliPHOSQAFloatCheckable("TotEn") ; 
  fQAHitsMulB  = new TClonesArray("AliPHOSQAIntCheckable",nb) ;
  fQAHitsMulB->SetOwner() ; 
  fQATotEnerB  = new TClonesArray("AliPHOSQAFloatCheckable", nb); 
  fQATotEnerB->SetOwner() ; 
  char tempo[20]  ; 
  Int_t i ; 
  for ( i = 0 ; i < nb ; i++ ) {
    sprintf(tempo, "HitsMB%d", i+1) ; 
    new( (*fQAHitsMulB)[i]) AliPHOSQAIntCheckable(tempo) ; 
    sprintf(tempo, "TotEnB%d", i+1) ; 
    new( (*fQATotEnerB)[i] ) AliPHOSQAFloatCheckable(tempo) ;
  }

  AliPHOSQAMeanChecker * hmc  = new AliPHOSQAMeanChecker("HitsMul", 100. ,25.) ; 
  AliPHOSQAMeanChecker * emc  = new AliPHOSQAMeanChecker("TotEner", 10. ,5.) ; 
  AliPHOSQAMeanChecker * bhmc = new AliPHOSQAMeanChecker("HitsMulB", 100. ,5.) ; 
  AliPHOSQAMeanChecker * bemc = new AliPHOSQAMeanChecker("TotEnerB", 2. ,.5) ; 

  // associate checkables and checkers 
  fQAHitsMul->AddChecker(hmc) ; 
  fQATotEner->AddChecker(emc) ; 
  for ( i = 0 ; i < nb ; i++ ) {
    (static_cast<AliPHOSQAIntCheckable*>((*fQAHitsMulB)[i]))->AddChecker(bhmc) ;
    (static_cast<AliPHOSQAFloatCheckable*>((*fQATotEnerB)[i]))->AddChecker(bemc) ; 
  }

}

//____________________________________________________________________________
AliPHOSv1::~AliPHOSv1()
{
  // dtor

  if ( fHits) {
    fHits->Delete() ; 
    delete fHits ;
    fHits = 0 ; 
  }
  
  if ( fQAHitsMulB ) {
    fQAHitsMulB->Delete() ;
    delete fQAHitsMulB ; 
  }

  if ( fQATotEnerB ) {
    fQATotEnerB->Delete() ;
    delete fQATotEnerB ; 
  }
 
}

//____________________________________________________________________________
void AliPHOSv1::AddHit(Int_t shunt, Int_t primary, Int_t tracknumber, Int_t Id, Float_t * hits)
{
  // Add a hit to the hit list.
  // A PHOS hit is the sum of all hits in a single crystal from one primary and within some time gate

  Int_t hitCounter ;
  AliPHOSHit *newHit ;
  AliPHOSHit *curHit ;
  Bool_t deja = kFALSE ;
  AliPHOSGeometry * geom = GetGeometry() ; 

  newHit = new AliPHOSHit(shunt, primary, tracknumber, Id, hits) ;

  for ( hitCounter = fNhits-1 ; hitCounter >= 0 && !deja ; hitCounter-- ) {
    curHit = dynamic_cast<AliPHOSHit*>((*fHits)[hitCounter]) ;
    if(curHit->GetPrimary() != primary) break ; 
           // We add hits with the same primary, while GEANT treats primaries succesively 
    if( *curHit == *newHit ) {
      *curHit + *newHit ;
      deja = kTRUE ;
    }
  }
         
  if ( !deja ) {
    new((*fHits)[fNhits]) AliPHOSHit(*newHit) ;
    // get the block Id number
    Int_t relid[4] ;
    geom->AbsToRelNumbering(Id, relid) ;
    // and fill the relevant QA checkable (only if in PbW04)
    if ( relid[1] == 0 ) {
      fQAHitsMul->Update(1) ; 
      (static_cast<AliPHOSQAIntCheckable*>((*fQAHitsMulB)[relid[0]-1]))->Update(1) ;
    } 
    fNhits++ ;
  }

  delete newHit;
}

//____________________________________________________________________________
void AliPHOSv1::FinishPrimary() 
{
  // called at the end of each track (primary) by AliRun
  // hits are reset for each new track
  // accumulate the total hit-multiplicity
//   if ( fQAHitsMul ) 
//     fQAHitsMul->Update( fHits->GetEntriesFast() ) ; 

}

//____________________________________________________________________________
void AliPHOSv1::FinishEvent() 
{
  // called at the end of each event by AliRun
  // accumulate the hit-multiplicity and total energy per block 
  // if the values have been updated check it

  if ( fQATotEner ) { 
    if ( fQATotEner->HasChanged() ) {
      fQATotEner->CheckMe() ; 
      fQATotEner->Reset() ; 
    }
  }
  
  Int_t i ; 
  if ( fQAHitsMulB && fQATotEnerB ) {
    for (i = 0 ; i < GetGeometry()->GetNModules() ; i++) {
      AliPHOSQAIntCheckable * ci = static_cast<AliPHOSQAIntCheckable*>((*fQAHitsMulB)[i]) ;  
      AliPHOSQAFloatCheckable* cf = static_cast<AliPHOSQAFloatCheckable*>((*fQATotEnerB)[i]) ; 
      if ( ci->HasChanged() ) { 
	ci->CheckMe() ;  
	ci->Reset() ;
      } 
      if ( cf->HasChanged() ) { 
	cf->CheckMe() ; 
	cf->Reset() ;
      }
    } 
  }
  
  // check the total multiplicity 
  
  if ( fQAHitsMul ) {
    if ( fQAHitsMul->HasChanged() ) { 
      fQAHitsMul->CheckMe() ; 
      fQAHitsMul->Reset() ; 
    }
  } 
}
//____________________________________________________________________________
void AliPHOSv1::StepManager(void)
{
   // Accumulates hits as long as the track stays in a single crystal or CPV gas Cell

  Int_t          relid[4] ;           // (box, layer, row, column) indices
  Int_t          absid    ;           // absolute cell ID number
  Float_t        xyzte[5]={-1000.,-1000.,-1000.,0.,0.}  ; // position wrt MRS, time and energy deposited
  TLorentzVector pos      ;           // Lorentz vector of the track current position
  Int_t          copy     ;

  Int_t tracknumber =  gAlice->CurrentTrack() ; 
  Int_t primary     =  gAlice->GetPrimary( gAlice->CurrentTrack() ); 
  TString name      =  GetGeometry()->GetName() ; 

  Int_t moduleNumber ;
  
  if( gMC->CurrentVolID(copy) == gMC->VolId("PCPQ") &&
      (gMC->IsTrackEntering() ) &&
      gMC->TrackCharge() != 0) {      
    
    gMC -> TrackPosition(pos);
    
    Float_t xyzm[3], xyzd[3] ;
    Int_t i;
    for (i=0; i<3; i++) xyzm[i] = pos[i];
    gMC -> Gmtod (xyzm, xyzd, 1);    // transform coordinate from master to daughter system
    
    Float_t        xyd[3]={0,0,0}   ;   //local position of the entering
    xyd[0]  = xyzd[0];
    xyd[1]  =-xyzd[2];
    xyd[2]  =-xyzd[1];
    
    // Current momentum of the hit's track in the local ref. system
    TLorentzVector pmom     ;        //momentum of the particle initiated hit
    gMC -> TrackMomentum(pmom);
    Float_t pm[3], pd[3];
    for (i=0; i<3; i++)  
      pm[i]   = pmom[i];
    
    gMC -> Gmtod (pm, pd, 2);        // transform 3-momentum from master to daughter system
    pmom[0] = pd[0];
    pmom[1] =-pd[1];
    pmom[2] =-pd[2];

    // Digitize the current CPV hit:
    
    // 1. find pad response and    
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

      xyzte[3] = gMC->TrackTime() ;
      xyzte[4] = cpvDigit->GetQpad() ;                          // amplitude in a pad
      primary = -1;                                             // No need in primary for CPV
      AddHit(fIshunt, primary, tracknumber, absid, xyzte);
      
      if (cpvDigit->GetQpad() > 0.02) {
	xmean += cpvDigit->GetQpad() * (cpvDigit->GetXpad() + 0.5);
	zmean += cpvDigit->GetQpad() * (cpvDigit->GetYpad() + 0.5);
	qsum  += cpvDigit->GetQpad();
      }
    }
    if (cpvDigits) {
      cpvDigits->Delete();
      delete cpvDigits;
      cpvDigits=0;
    }
  }

 
  
  if(gMC->CurrentVolID(copy) == gMC->VolId("PXTL") ) { //  We are inside a PBWO crystal

    gMC->TrackPosition(pos) ;
    xyzte[0] = pos[0] ;
    xyzte[1] = pos[1] ;
    xyzte[2] = pos[2] ;

    Float_t global[3], local[3] ;
    global[0] = pos[0] ;
    global[1] = pos[1] ;
    global[2] = pos[2] ;
    Float_t lostenergy = gMC->Edep(); 
    
    //Put in the TreeK particle entering PHOS and all its parents
    if ( gMC->IsTrackEntering() ){
      Float_t xyzd[3] ;
      gMC -> Gmtod (xyzte, xyzd, 1);    // transform coordinate from master to daughter system    
      if (xyzd[1] >  GetGeometry()->GetCrystalSize(1)/2-0.002 ||
	  xyzd[1] < -GetGeometry()->GetCrystalSize(1)/2+0.002) {
	TParticle * part = 0 ; 
	Int_t parent = gAlice->CurrentTrack() ; 
	while ( parent != -1 ) {
	  part = gAlice->Particle(parent) ; 
	  part->SetBit(kKeepBit);
	  parent = part->GetFirstMother() ; 
	}
      }
    }
    if ( lostenergy != 0 ) {  // Track is inside the crystal and deposits some energy 
      xyzte[3] = gMC->TrackTime() ;     
      
      gMC->CurrentVolOffID(10, moduleNumber) ; // get the PHOS module number ;
      
      Int_t strip ;
      gMC->CurrentVolOffID(3, strip);
      Int_t cell ;
      gMC->CurrentVolOffID(2, cell);
      
      Int_t row = 1 + GetGeometry()->GetNZ() - strip % GetGeometry()->GetNZ() ;
      Int_t col = (Int_t) TMath::Ceil((Double_t) strip/GetGeometry()->GetNZ()) -1 ;
      
      absid = (moduleNumber-1)*GetGeometry()->GetNCristalsInModule() + 
	row + (col*GetGeometry()->GetEMCAGeometry()->GetNCellsInStrip() + cell-1)*GetGeometry()->GetNZ() ;
      
      gMC->Gmtod(global, local, 1) ;
      
      //Calculates the light yield, the number of photons produced in the
      //crystal 
      Float_t lightYield = gRandom->Poisson(fLightFactor * lostenergy *
					    exp(-fLightYieldAttenuation *
						(local[1]+GetGeometry()->GetCrystalSize(1)/2.0 ))
					    ) ;

      //Calculates de energy deposited in the crystal  
      xyzte[4] = fAPDFactor * lightYield  ;
      
      // add current hit to the hit list
      //cout << "AliPHOSv1::StepManager " << primary << " " << tracknumber << endl ; 
      AddHit(fIshunt, primary,tracknumber, absid, xyzte);
      
      // fill the relevant QA Checkables
      fQATotEner->Update( xyzte[4] ) ;                                             // total energy in PHOS
      (static_cast<AliPHOSQAFloatCheckable*>((*fQATotEnerB)[moduleNumber-1]))->Update( xyzte[4] ) ; // energy in this block  
      
    } // there is deposited energy
  } // we are inside a PHOS Xtal
  
}

//____________________________________________________________________________
void AliPHOSv1::CPVDigitize (TLorentzVector p, Float_t *zxhit, Int_t moduleNumber, TClonesArray *cpvDigits)
{
  // ------------------------------------------------------------------------
  // Digitize one CPV hit:
  // On input take exact 4-momentum p and position zxhit of the hit,
  // find the pad response around this hit and
  // put the amplitudes in the pads into array digits
  //
  // Author: Yuri Kharlov (after Serguei Sadovsky)
  // 2 October 2000
  // ------------------------------------------------------------------------

  const Float_t kCelWr  = GetGeometry()->GetPadSizePhi()/2;  // Distance between wires (2 wires above 1 pad)
  const Float_t kDetR   = 0.1;     // Relative energy fluctuation in track for 100 e-
  const Float_t kdEdx   = 4.0;     // Average energy loss in CPV;
  const Int_t   kNgamz  = 5;       // Ionization size in Z
  const Int_t   kNgamx  = 9;       // Ionization size in Phi
  const Float_t kNoise = 0.03;    // charge noise in one pad

  Float_t rnor1,rnor2;

  // Just a reminder on axes notation in the CPV module:
  // axis Z goes along the beam
  // axis X goes across the beam in the module plane
  // axis Y is a normal to the module plane showing from the IP

  Float_t hitX  = zxhit[0];
  Float_t hitZ  =-zxhit[1];
  Float_t pX    = p.Px();
  Float_t pZ    =-p.Pz();
  Float_t pNorm = p.Py();
  Float_t eloss = kdEdx;

//    cout << "CPVDigitize: YVK : "<<hitX<<" "<<hitZ<<" | "<<pX<<" "<<pZ<<" "<<pNorm<<endl;

  Float_t dZY   = pZ/pNorm * GetGeometry()->GetCPVGasThickness();
  Float_t dXY   = pX/pNorm * GetGeometry()->GetCPVGasThickness();
  gRandom->Rannor(rnor1,rnor2);
  eloss *= (1 + kDetR*rnor1) *
           TMath::Sqrt((1 + ( pow(dZY,2) + pow(dXY,2) ) / pow(GetGeometry()->GetCPVGasThickness(),2)));
  Float_t zhit1 = hitZ + GetGeometry()->GetCPVActiveSize(1)/2 - dZY/2;
  Float_t xhit1 = hitX + GetGeometry()->GetCPVActiveSize(0)/2 - dXY/2;
  Float_t zhit2 = zhit1 + dZY;
  Float_t xhit2 = xhit1 + dXY;

  Int_t   iwht1 = (Int_t) (xhit1 / kCelWr);           // wire (x) coordinate "in"
  Int_t   iwht2 = (Int_t) (xhit2 / kCelWr);           // wire (x) coordinate "out"

  Int_t   nIter;
  Float_t zxe[3][5];
  if (iwht1==iwht2) {                      // incline 1-wire hit
    nIter = 2;
    zxe[0][0] = (zhit1 + zhit2 - dZY*0.57735) / 2;
    zxe[1][0] = (iwht1 + 0.5) * kCelWr;
    zxe[2][0] =  eloss/2;
    zxe[0][1] = (zhit1 + zhit2 + dZY*0.57735) / 2;
    zxe[1][1] = (iwht1 + 0.5) * kCelWr;
    zxe[2][1] =  eloss/2;
  }
  else if (TMath::Abs(iwht1-iwht2) != 1) { // incline 3-wire hit
    nIter = 3;
    Int_t iwht3 = (iwht1 + iwht2) / 2;
    Float_t xwht1 = (iwht1 + 0.5) * kCelWr; // wire 1
    Float_t xwht2 = (iwht2 + 0.5) * kCelWr; // wire 2
    Float_t xwht3 = (iwht3 + 0.5) * kCelWr; // wire 3
    Float_t xwr13 = (xwht1 + xwht3) / 2;   // center 13
    Float_t xwr23 = (xwht2 + xwht3) / 2;   // center 23
    Float_t dxw1  = xhit1 - xwr13;
    Float_t dxw2  = xhit2 - xwr23;
    Float_t egm1  = TMath::Abs(dxw1) / ( TMath::Abs(dxw1) + TMath::Abs(dxw2) + kCelWr );
    Float_t egm2  = TMath::Abs(dxw2) / ( TMath::Abs(dxw1) + TMath::Abs(dxw2) + kCelWr );
    Float_t egm3  =           kCelWr / ( TMath::Abs(dxw1) + TMath::Abs(dxw2) + kCelWr );
    zxe[0][0] = (dXY*(xwr13-xwht1)/dXY + zhit1 + zhit1) / 2;
    zxe[1][0] =  xwht1;
    zxe[2][0] =  eloss * egm1;
    zxe[0][1] = (dXY*(xwr23-xwht1)/dXY + zhit1 + zhit2) / 2;
    zxe[1][1] =  xwht2;
    zxe[2][1] =  eloss * egm2;
    zxe[0][2] =  dXY*(xwht3-xwht1)/dXY + zhit1;
    zxe[1][2] =  xwht3;
    zxe[2][2] =  eloss * egm3;
  }
  else {                                   // incline 2-wire hit
    nIter = 2;
    Float_t xwht1 = (iwht1 + 0.5) * kCelWr;
    Float_t xwht2 = (iwht2 + 0.5) * kCelWr;
    Float_t xwr12 = (xwht1 + xwht2) / 2;
    Float_t dxw1  = xhit1 - xwr12;
    Float_t dxw2  = xhit2 - xwr12;
    Float_t egm1  = TMath::Abs(dxw1) / ( TMath::Abs(dxw1) + TMath::Abs(dxw2) );
    Float_t egm2  = TMath::Abs(dxw2) / ( TMath::Abs(dxw1) + TMath::Abs(dxw2) );
    zxe[0][0] = (zhit1 + zhit2 - dZY*egm1) / 2;
    zxe[1][0] =  xwht1;
    zxe[2][0] =  eloss * egm1;
    zxe[0][1] = (zhit1 + zhit2 + dZY*egm2) / 2;
    zxe[1][1] =  xwht2;
    zxe[2][1] =  eloss * egm2;
  }

  // Finite size of ionization region

  Int_t nCellZ  = GetGeometry()->GetNumberOfCPVPadsZ();
  Int_t nCellX  = GetGeometry()->GetNumberOfCPVPadsPhi();
  Int_t nz3     = (kNgamz+1)/2;
  Int_t nx3     = (kNgamx+1)/2;
  cpvDigits->Expand(nIter*kNgamx*kNgamz);
  TClonesArray &ldigits = *(static_cast<TClonesArray *>(cpvDigits));

  for (Int_t iter=0; iter<nIter; iter++) {

    Float_t zhit = zxe[0][iter];
    Float_t xhit = zxe[1][iter];
    Float_t qhit = zxe[2][iter];
    Float_t zcell = zhit / GetGeometry()->GetPadSizeZ();
    Float_t xcell = xhit / GetGeometry()->GetPadSizePhi();
    if ( zcell<=0      || xcell<=0 ||
	 zcell>=nCellZ || xcell>=nCellX) return;
    Int_t izcell = (Int_t) zcell;
    Int_t ixcell = (Int_t) xcell;
    Float_t zc = zcell - izcell - 0.5;
    Float_t xc = xcell - ixcell - 0.5;
    for (Int_t iz=1; iz<=kNgamz; iz++) {
      Int_t kzg = izcell + iz - nz3;
      if (kzg<=0 || kzg>nCellZ) continue;
      Float_t zg = (Float_t)(iz-nz3) - zc;
      for (Int_t ix=1; ix<=kNgamx; ix++) {
	Int_t kxg = ixcell + ix - nx3;
	if (kxg<=0 || kxg>nCellX) continue;
	Float_t xg = (Float_t)(ix-nx3) - xc;
	
	// Now calculate pad response
	Float_t qpad = CPVPadResponseFunction(qhit,zg,xg);
	qpad += kNoise*rnor2;
	if (qpad<0) continue;
	
	// Fill the array with pad response ID and amplitude
	new(ldigits[cpvDigits->GetEntriesFast()]) AliPHOSCPVDigit(kxg,kzg,qpad);
      }
    }
  }
}

//____________________________________________________________________________
Float_t AliPHOSv1::CPVPadResponseFunction(Float_t qhit, Float_t zhit, Float_t xhit) {
  // ------------------------------------------------------------------------
  // Calculate the amplitude in one CPV pad using the
  // cumulative pad response function
  // Author: Yuri Kharlov (after Serguei Sadovski)
  // 3 October 2000
  // ------------------------------------------------------------------------

  Double_t dz = GetGeometry()->GetPadSizeZ()   / 2;
  Double_t dx = GetGeometry()->GetPadSizePhi() / 2;
  Double_t z  = zhit * GetGeometry()->GetPadSizeZ();
  Double_t x  = xhit * GetGeometry()->GetPadSizePhi();
  Double_t amplitude = qhit *
    (CPVCumulPadResponse(z+dz,x+dx) - CPVCumulPadResponse(z+dz,x-dx) -
     CPVCumulPadResponse(z-dz,x+dx) + CPVCumulPadResponse(z-dz,x-dx));
  return (Float_t)amplitude;
}

//____________________________________________________________________________
Double_t AliPHOSv1::CPVCumulPadResponse(Double_t x, Double_t y) {
  // ------------------------------------------------------------------------
  // Cumulative pad response function
  // It includes several terms from the CF decomposition in electrostatics
  // Note: this cumulative function is wrong since omits some terms
  //       but the cell amplitude obtained with it is correct because
  //       these omitting terms cancel
  // Author: Yuri Kharlov (after Serguei Sadovski)
  // 3 October 2000
  // ------------------------------------------------------------------------

  const Double_t kA=1.0;
  const Double_t kB=0.7;

  Double_t r2       = x*x + y*y;
  Double_t xy       = x*y;
  Double_t cumulPRF = 0;
  for (Int_t i=0; i<=4; i++) {
    Double_t b1 = (2*i + 1) * kB;
    cumulPRF += TMath::Power(-1,i) * TMath::ATan( xy / (b1*TMath::Sqrt(b1*b1 + r2)) );
  }
  cumulPRF *= kA/(2*TMath::Pi());
  return cumulPRF;
}


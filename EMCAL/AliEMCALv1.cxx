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
// Implementation version v1 of EMCAL Manager class 
// An object of this class does not produce digits
// It is the one to use if you do want to produce outputs in TREEH 
//                  
//*-- Author: Sahal Yacoob (LBL /UCT)
//*--       : Jennifer Klay (LBL)

// This Class not stores information on all particles prior to EMCAL entry - in order to facilitate analysis.
// This is done by setting fIShunt =2, and flagging all parents of particles entering the EMCAL.

// 15/02/2002 .... Yves Schutz
//  1. fSamplingFraction and fLayerToPreshowerRatio have been removed
//  2. Timing signal is collected and added to hit

// --- ROOT system ---
#include "TPGON.h"
#include "TTUBS.h"
#include "TNode.h"
#include "TRandom.h"
#include "TTree.h"
#include "TGeometry.h"
#include "TParticle.h"

// --- Standard library ---

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Rstrstream.h>
#include <Riostream.h>
#include <math.h>

// --- AliRoot header files ---

#include "AliEMCALv1.h"
#include "AliEMCALHit.h"
#include "AliEMCALGeometry.h"
#include "AliConst.h"
#include "AliRun.h"

ClassImp(AliEMCALv1)


//______________________________________________________________________
AliEMCALv1::AliEMCALv1():AliEMCALv0(){
  // ctor

}

//______________________________________________________________________
AliEMCALv1::AliEMCALv1(const char *name, const char *title):
    AliEMCALv0(name,title){
    // Standard Creator.

    fHits= new TClonesArray("AliEMCALHit",1000);
    gAlice->AddHitList(fHits);

    fNhits = 0;
    fIshunt     =  2; // All hits are associated with particles entering the calorimeter
}

//______________________________________________________________________
AliEMCALv1::~AliEMCALv1(){
    // dtor

    if ( fHits) {
	fHits->Delete();
	delete fHits;
	fHits = 0;
    }
}
//______________________________________________________________________
void AliEMCALv1::AddHit(Int_t shunt, Int_t primary, Int_t tracknumber, Int_t iparent, Float_t ienergy, 
			Int_t id, Float_t * hits,Float_t * p){
    // Add a hit to the hit list.
    // An EMCAL hit is the sum of all hits in a tower section (PRE, ECAL, HCAL) 
    //   originating from the same entering particle 
    Int_t hitCounter;
    
    AliEMCALHit *newHit;
    AliEMCALHit *curHit;
    Bool_t deja = kFALSE;

    newHit = new AliEMCALHit(shunt, primary, tracknumber, iparent, ienergy, id, hits, p);
    for ( hitCounter = fNhits-1; hitCounter >= 0 && !deja; hitCounter-- ) {
	curHit = (AliEMCALHit*) (*fHits)[hitCounter];
	// We add hits with the same tracknumber, while GEANT treats
	// primaries succesively
	if(curHit->GetPrimary() != primary) 
	  break;
	if( *curHit == *newHit ) {
	    *curHit = *curHit + *newHit;
	    deja = kTRUE;
	} // end if
    } // end for hitCounter

    if ( !deja ) {
	new((*fHits)[fNhits]) AliEMCALHit(*newHit);
	fNhits++;
    } // end if

    delete newHit;
}
//______________________________________________________________________
void AliEMCALv1::StepManager(void){
  // Accumulates hits as long as the track stays in a single
  // crystal or PPSD gas Cell

  Int_t          id[2];           // (layer, phi, Eta) indices
  // position wrt MRS and energy deposited
  Float_t        xyzte[5]={0.,0.,0.,0.,0.};// position wrt MRS, time and energy deposited
  Float_t        pmom[4]={0.,0.,0.,0.};
  TLorentzVector pos; // Lorentz vector of the track current position.
  TLorentzVector mom; // Lorentz vector of the track current momentum.
  Int_t tracknumber =  gAlice->CurrentTrack();
  Int_t primary = 0;
  static Int_t iparent = 0;
  static Float_t ienergy = 0;
  Int_t copy = 0;
  
  AliEMCALGeometry * geom = GetGeometry() ; 

  if(gMC->IsTrackEntering() && (strcmp(gMC->CurrentVolName(),"XALU") == 0)){ // This Particle in enterring the Calorimeter
    gMC->TrackPosition(pos) ;
    xyzte[0] = pos[0] ;
    xyzte[1] = pos[1] ;
    xyzte[2] = pos[2] ;
    if ( (xyzte[0]*xyzte[0] + xyzte[1]*xyzte[1])  
	 <  (geom->GetEnvelop(0)+geom->GetGap2Active()+1.5 )*(geom->GetEnvelop(0)+geom->GetGap2Active()+1.5 ) ) {
      iparent = tracknumber;
      gMC->TrackMomentum(mom);
      ienergy = mom[3]; 
      TParticle * part = 0 ;
      Int_t parent = iparent ;
      while ( parent != -1 ) { // <------------- flags this particle to be kept and
	//all the ancestors of this particle
	part = gAlice->Particle(parent) ;
	part->SetBit(kKeepBit);
	parent = part->GetFirstMother() ;
      }
    }
  }
  if(gMC->CurrentVolID(copy) == gMC->VolId("XPHI") ) { // We are in a Scintillator Layer 
    
    Float_t depositedEnergy ; 
    
    if( (depositedEnergy = gMC->Edep()) > 0.){// Track is inside a scintillator and deposits some energy
     
      gMC->TrackPosition(pos);
      xyzte[0] = pos[0];
      xyzte[1] = pos[1];
      xyzte[2] = pos[2];
      xyzte[3] = gMC->TrackTime() ;       
      
      gMC->TrackMomentum(mom);
      pmom[0] = mom[0];
      pmom[1] = mom[1];
      pmom[2] = mom[2];
      pmom[3] = mom[3];
      
      gMC->CurrentVolOffID(1, id[0]); // get the POLY copy number;
      gMC->CurrentVolID(id[1]); // get the phi number inside the layer
      
      Int_t tower = (id[0]-1) % geom->GetNZ() + 1 + (id[1] - 1) * geom->GetNZ() ;  
      Int_t layer = static_cast<Int_t>((id[0]-1)/(geom->GetNZ())) + 1 ; 
      Int_t absid = tower ; 
      if (layer <= geom->GetNPRLayers() )
	absid += geom->GetNZ() * geom->GetNPhi() ;
      else if (layer > geom->GetNECLayers() )
	absid += 2 * geom->GetNZ() * geom->GetNPhi() ;
      else {
	Int_t nlayers = geom->GetNPRLayers()+ geom->GetNECLayers()+ geom->GetNHCLayers() ;
	if (layer > nlayers) 
	  Fatal("StepManager", "Wrong calculation of layer number: layer = %d > %d\n", layer, nlayers) ;
      }
	
      Float_t lightYield =  depositedEnergy ;
					     ;
      xyzte[4] = lightYield  ;
   
      primary = gAlice->GetPrimary(tracknumber);

      if (gDebug == 2) 
	Info("StepManager", "id0 = %d, id1 = %d, absid = %d tower = %d layer = %d energy = %f\n", id[0], id[1], absid, tower, layer, xyzte[4]) ;

      AddHit(fIshunt, primary,tracknumber, iparent, ienergy, absid, xyzte, pmom);
    } // there is deposited energy
  }
}

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
//  Hits class for EMCAL    
//  A hit in EMCAL is the sum of all hits in a single segment
//  from a single enterring particle             
//*-- Author: Sahal Yacoob (LBL / UCT)
// Based on AliPHOSHit

// --- Standard library ---
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Rstrstream.h>
#include <Riostream.h>

// --- ROOT system ---
#include <TLorentzVector.h>

// --- AliRoot header files ---
#include "AliEMCALHit.h"
#include "AliRun.h"
#include "AliConst.h"
#include "AliEMCALGeometry.h"
#include "AliEMCALGetter.h"

ClassImp(AliEMCALHit)

//______________________________________________________________________
AliEMCALHit::AliEMCALHit(){
    // Default ctor
   
    fId      = 0;
    fELOS    = 0.0;
    fTime    = 0.0;
    fPrimary = 0;
    fTrack   = 0;
    fX       = 0.0;
    fY       = 0.0;
    fZ       = 0.0;
    fPx       = 0.0;
    fPy       = 0.0;
    fPz       = 0.0;
    fPe       = 0.0;
    fIparent = 0;
    fIenergy = 0.0;
}
//______________________________________________________________________
AliEMCALHit::AliEMCALHit(const AliEMCALHit & hit){
    // copy ctor
   
    fId      = hit.fId ; 
    fELOS    = hit.fELOS ;
    fPrimary = hit.fPrimary ; 
    fTrack   = hit.fTrack ; 
    fX       = hit.fX;
    fY       = hit.fY;
    fZ       = hit.fZ;
    fPx       = hit.fPx;
    fPy       = hit.fPy;
    fPz       = hit.fPz;
    fPe       = hit.fPe;
    fIparent = hit.fIparent;
    fIenergy = hit.fIenergy;
    fTime    = hit.fTime  ;
}
//______________________________________________________________________
AliEMCALHit::AliEMCALHit(Int_t shunt, Int_t primary, Int_t track,Int_t iparent, Float_t ienergy, Int_t id,
			 Float_t *hits,Float_t *p):AliHit(shunt, track){
    //
    // Create an EMCAL  hit object
    //
    fX          = hits[0];
    fY          = hits[1];
    fZ          = hits[2];
    fTime       = hits[3] ;
    fId         = id;
    fELOS       = hits[4];
    fPrimary    = primary;
    fPx          = p[0];
    fPy          = p[1];
    fPz          = p[2];
    fPe          = p[3];
    fIparent    = iparent;
    fIenergy    = ienergy;
}

//______________________________________________________________________
const Bool_t AliEMCALHit::IsInPreShower() const 
{
  Bool_t rv = kFALSE ;
  
  const AliEMCALGeometry * geom = AliEMCALGetter::GetInstance()->EMCALGeometry() ;
  if((GetId()/geom->GetNPhi()) < (2*geom->GetNZ())) 
    rv = kTRUE; 
  return rv; 
} 

//______________________________________________________________________
Bool_t AliEMCALHit::operator==(AliEMCALHit const &rValue) const{ 
    // Two hits are identical if they have the same Id and originat
    // from the same enterring Particle 
    Bool_t rv = kFALSE;

    if ( (fId == rValue.GetId()) && ( fIparent == rValue.GetIparent()) )
	rv = kTRUE;

    return rv;
}
//______________________________________________________________________
AliEMCALHit AliEMCALHit::operator+(const AliEMCALHit &rValue){
    // Add the energy of the hit

    fELOS += rValue.GetEnergy() ;
 
    if(rValue.GetTime() < fTime)
      fTime = rValue.GetTime() ;
 
    return *this;

}
//______________________________________________________________________
ostream& operator << (ostream& out,AliEMCALHit& hit){
    // Print out Id and energy

    out << "AliEMCALHit:";
    out << "id=" <<  hit.GetId();
    out << ", Eloss=" <<  hit.GetEnergy();
    out << ", Time=" << hit.GetTime();
    out << "GeV , Track no.=" << hit.GetPrimary();
    out << ", (xyz)=(" << hit.X()<< ","<< hit.Y()<< ","<<hit.Z()<<") cm";
    out << ", fTrack=" << hit.GetTrack();
    out << ", P=(" << hit.GetPx() << "," << hit.GetPy() << "," << hit.GetPz()
                  << "," <<hit.GetPe() << ") GeV"  ;
    out << ", Enterring particle ID" << hit.GetIparent();
    out << ", Enterring particle initial energy = " << hit.GetIenergy() << " GeV" ;
    out << endl;

    return out;
}

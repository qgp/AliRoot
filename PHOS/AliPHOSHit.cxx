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
//  Hits class for PHOS     
//  A hit in PHOS is the sum of all hits in a single crystal
//*--
//*-- Author: Maxime Volkov (RRC KI) & Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)

// --- ROOT system ---

// --- Standard library ---
#include <string.h>
#include <stdlib.h>

// --- AliRoot header files ---
#include "AliPHOSHit.h"
#include "AliRun.h"
#include "AliConst.h"
#include "AliPHOSLoader.h"
#include "AliPHOSGeometry.h"
#include "AliPHOS.h"

ClassImp(AliPHOSHit)
  
  //____________________________________________________________________________
  AliPHOSHit::AliPHOSHit(const AliPHOSHit & hit) 
{
  // copy ctor
  fX       = hit.fX ; 
  fY       = hit.fY ; 
  fZ       = hit.fZ ; 
  fId      = hit.fId ; 
  fELOS    = hit.fELOS ;
  fPrimary = hit.fPrimary ; 
  fTrack   = hit.fTrack ; 
  fTime    = hit.fTime  ;
  
} 

//____________________________________________________________________________
AliPHOSHit::AliPHOSHit(Int_t shunt, Int_t primary, Int_t track, Int_t id, Float_t *hits): AliHit(shunt, track)
{
  //
  // Create a CPV hit object
  //
  
  fX          = hits[0] ;
  fY          = hits[1] ;
  fZ          = hits[2] ;
  fTime       = hits[3] ;
  fId         = id ;
  fELOS       = hits[4] ;
  fPrimary    = primary ;
}
//____________________________________________________________________________
Float_t AliPHOSHit::X() const
{
  //  if(fX < -1000.){
    TVector3  pos ;
    AliPHOS * phos = static_cast<AliPHOS*> (gAlice->GetDetector("PHOS")) ;
    phos->GetGeometry() ->RelPosInAlice(GetId(),  pos) ;
    return pos.X() ;
    //    fX = pos.X() ;
    //    fY = pos.Y() ;
    //    fZ = pos.Z() ;
    //  }
    //  return fX;
}
//____________________________________________________________________________
Float_t AliPHOSHit::Y() const
{
  //  if(fY < -1000.){
    TVector3  pos ;
    AliPHOS * phos = static_cast<AliPHOS*> (gAlice->GetDetector("PHOS")) ;
    phos->GetGeometry() ->RelPosInAlice(GetId(),  pos) ;
    return pos.Y(); 
    //    fX = pos.X() ;
    //    fY = pos.Y() ;
    //    fZ = pos.Z() ;
    //  }
    //  return fY;
}
//____________________________________________________________________________
Float_t AliPHOSHit::Z() const
{
  //  if(fY < -1000.){
    TVector3  pos ;
    AliPHOS * phos = static_cast<AliPHOS*> (gAlice->GetDetector("PHOS")) ;
    phos->GetGeometry() ->RelPosInAlice(GetId(),  pos) ;
    return pos.Z() ;
    //    fX = pos.X() ;
    //    fY = pos.Y() ;
    //    fZ = pos.Z() ;
    //  }
    //  return fZ;
}
//____________________________________________________________________________
Bool_t AliPHOSHit::operator==(AliPHOSHit const &rValue) const
{ 
  // Two hits are identical if they have the same Id and originate from the same primary 

  Bool_t rv = kFALSE ; 

  if ( (fId == rValue.GetId()) && ( fPrimary == rValue.GetPrimary() ) )
    rv = kTRUE;
  
  return rv;
}

//____________________________________________________________________________
AliPHOSHit AliPHOSHit::operator+(const AliPHOSHit &rValue)
{
  // Add the energy of the hit
  
  fELOS += rValue.GetEnergy() ;

  if(rValue.GetTime() < fTime)
    fTime = rValue.GetTime() ;
    
   return *this;

}

//____________________________________________________________________________
ostream& operator << (ostream& out, const AliPHOSHit& hit) 
{
  // Print out Id and energy 
  
  //out << "AliPHOSHit = " << hit.GetId() << " " << hit.GetEnergy() << "  " << hit.GetTime() << endl ;
  Warning("operator <<", "Implement differently") ; 
  return out ;
}




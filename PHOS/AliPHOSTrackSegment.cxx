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

/* History of cvs commits:
 *
 * $Log$
 * Revision 1.29  2005/05/28 14:19:05  schutz
 * Compilation warnings fixed by T.P.
 *
 */

//_________________________________________________________________________
//  Track segment in PHOS
//  Can be : 1 EmcRecPoint
//           1 EmcRecPoint + 1 CPV
//           1 EmcRecPoint + 1 CPV + 1 charged track
//                  
//*-- Author:  Dmitri Peressounko (RRC KI & SUBATECH)

// --- ROOT system ---
 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliPHOSEmcRecPoint.h" 
#include "AliPHOSTrackSegment.h" 
#include "AliESDtrack.h" 

ClassImp(AliPHOSTrackSegment)

//____________________________________________________________________________
AliPHOSTrackSegment::AliPHOSTrackSegment()
			: fEmcRecPoint(0),
			  fIndexInList(0),
			  fCpvRecPoint(0),
			  fTrack(0)
{
  //def ctor
}

//____________________________________________________________________________
AliPHOSTrackSegment::AliPHOSTrackSegment( AliPHOSEmcRecPoint * emc , 
					  AliPHOSRecPoint * cpvrp1)
			: fEmcRecPoint(0),
			  fIndexInList(0),
			  fCpvRecPoint(0),
			  fTrack(0)
{
  // ctor

  if( emc )   
    fEmcRecPoint =  emc->GetIndexInList() ;
  else 
    fEmcRecPoint = -1 ;

  if( cpvrp1 )  
    fCpvRecPoint = cpvrp1->GetIndexInList() ;
 else 
    fCpvRecPoint = -1 ;

  fTrack = -1 ; 

  fIndexInList = -1 ;
}

//____________________________________________________________________________
AliPHOSTrackSegment::AliPHOSTrackSegment( AliPHOSEmcRecPoint * emc , 
					  AliPHOSRecPoint * cpvrp1, 
					  Int_t track)
			: fEmcRecPoint(0),
			  fIndexInList(0),
			  fCpvRecPoint(0),
			  fTrack(0)
{
  // ctor

  if( emc )   
    fEmcRecPoint =  emc->GetIndexInList() ;
  else 
    fEmcRecPoint = -1 ;

  if( cpvrp1 )  
    fCpvRecPoint = cpvrp1->GetIndexInList() ;
 else 
    fCpvRecPoint = -1 ;
  
  fTrack = track ; 

  fIndexInList = -1 ;
}

//____________________________________________________________________________
AliPHOSTrackSegment::AliPHOSTrackSegment( const AliPHOSTrackSegment & ts) 
  : TObject(ts),
    fEmcRecPoint(0),
    fIndexInList(0),
    fCpvRecPoint(0),
    fTrack(0)
{
  // Copy ctor

  ( (AliPHOSTrackSegment &)ts ).Copy(*this) ; 
}


//____________________________________________________________________________
void AliPHOSTrackSegment::Copy(TObject & obj) const
{
  // Copy of a track segment into another track segment

   TObject::Copy(obj) ;
   ( (AliPHOSTrackSegment &)obj ).fEmcRecPoint     = fEmcRecPoint ; 
   ( (AliPHOSTrackSegment &)obj ).fCpvRecPoint     = fCpvRecPoint ; 
   ( (AliPHOSTrackSegment &)obj ).fIndexInList     = fIndexInList ; 
   ( (AliPHOSTrackSegment &)obj ).fTrack           = fTrack ;
} 


//____________________________________________________________________________
void AliPHOSTrackSegment::Print(const Option_t *) const
{
  // Print all information on this track Segment
  

  Info("Print", "");
  printf("Stored at position %d\n", fIndexInList) ;
  printf(" Emc RecPoint #     %d\n", fEmcRecPoint) ;
  if(fCpvRecPoint >= 0)
    printf(" CPV RecPoint #     %d\n", fCpvRecPoint) ;
  else
    printf(" No CPV RecPoint\n");
  if (fTrack >= 0) 
    printf(" Charged track #     %d\n", fTrack) ;
  else
    printf(" No Charged track\n");
}

//____________________________________________________________________________
void AliPHOSTrackSegment::SetCpvRecPoint(AliPHOSRecPoint * cpvRecPoint) 
{
  // gives an id from its position in the list
  if( cpvRecPoint )  
    fCpvRecPoint = cpvRecPoint->GetIndexInList() ;
 else 
    fCpvRecPoint = -1 ;
}


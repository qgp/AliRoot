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

/*
$Log$
Revision 1.10  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.9  2001/12/05 14:36:47  hristov
The default constructor now creates no objects; destructor corrected (thanks to R.Brun).

Revision 1.8  2001/10/21 18:38:43  hristov
Several pointers were set to zero in the default constructors to avoid memory management problems

Revision 1.7  2000/11/30 07:12:49  alibrary
Introducing new Rndm and QA classes

Revision 1.6  2000/10/02 21:28:14  fca
Removal of useless dependecies via forward declarations

Revision 1.5  2000/07/11 18:24:59  fca
Coding convention corrections + few minor bug fixes

Revision 1.4  2000/05/16 08:30:02  fca
Using automatic streamer for c arrays

Revision 1.3  2000/03/20 14:22:25  fca
New version to support new PHOS code

Revision 1.2  2000/02/15 09:43:54  fca
Corrections
- a bug in the streamer (wrong size of the arrays)
- replace Read/WriteArray by Read/WriteFastArray (suggestion R.Brun)

Revision 1.1  1999/12/17 09:01:14  fca
Y.Schutz new classes for reconstruction

*/

//-*-C++-*-
//_________________________________________________________________________
// Base Class for reconstructed space points 
// usually coming from the clusterisation algorithms
// run on the digits
//
//*-- Author : Yves Schutz  SUBATECH 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliRecPoint.h"
#include "AliGeometry.h"
#include "AliDigitNew.h"

ClassImp(AliRecPoint)


//_______________________________________________________________________
AliRecPoint::AliRecPoint():
  fAmp(0),
  fGeom(0),
  fIndexInList(-1), // to be set when the point is already stored
  fLocPos(0,0,0),
  fLocPosM(0),
  fMaxDigit(100),
  fMulDigit(0),
  fMaxTrack(5),
  fMulTrack(0),
  fDigitsList(0),
  fTracksList(0)
{
  //
  // default ctor  
  //
}

//_______________________________________________________________________
AliRecPoint::AliRecPoint(const char * ):
  fAmp(0),
  fGeom(0),
  fIndexInList(-1), // to be set when the point is already stored
  fLocPos(0,0,0),
  fLocPosM(new TMatrix(3,3)),
  fMaxDigit(100),
  fMulDigit(0),
  fMaxTrack(5),
  fMulTrack(0),
  fDigitsList(new int[fMaxDigit]),
  fTracksList(new int[fMaxTrack])
{
  //
  // Standard ctor  
  //
}

//_______________________________________________________________________
AliRecPoint::AliRecPoint(const AliRecPoint& recp):
  TObject(recp),
  fAmp(0),
  fGeom(0),
  fIndexInList(-1), // to be set when the point is already stored
  fLocPos(0,0,0),
  fLocPosM(0),
  fMaxDigit(100),
  fMulDigit(0),
  fMaxTrack(5),
  fMulTrack(0),
  fDigitsList(0),
  fTracksList(0)
{
  //
  // Copy constructor
  //
  recp.Copy(*this);
}

//_______________________________________________________________________
AliRecPoint::~AliRecPoint()
{
  // dtor
  
  delete fLocPosM ; 
  delete [] fDigitsList ; 
  delete [] fTracksList ;  
  
}
  
//_______________________________________________________________________
void AliRecPoint::AddDigit(AliDigitNew & digit)
{
  // adds a digit to the digits list
  // and accumulates the total amplitude and the multiplicity 
  
  
  if ( fMulDigit >= fMaxDigit ) { // increase the size of the list 
    int * tempo = new ( int[fMaxDigit*=2] ) ; 
    
    Int_t index ; 
    
    for ( index = 0 ; index < fMulDigit ; index++ )
      tempo[index] = fDigitsList[index] ; 
    
    delete fDigitsList ; 
    fDigitsList = tempo ; 
  }
  
  fDigitsList[fMulDigit] = digit.GetIndexInList()  ; 
  fMulDigit++ ; 
  fAmp += digit.GetAmp() ; 
}

//_______________________________________________________________________
// void AliRecPoint::AddTrack(AliTrack & track)
// {
//   // adds a digit to the digits list
//   // and accumulates the total amplitude and the multiplicity 


//   if ( fMulTrack >= fMaxTrack ) { // increase the size of the list 
//     int * tempo = new int[fMaxTrack*=2] ; 
//     Int_t index ; 
//     for ( index = 0 ; index < fMulTrack ; index++ )
//       tempo[index] = fTracksList[index] ; 
//     delete fTracksList ; 
//     fTracksList = tempo ; 
//   }

//   fTracksList[fMulTrack++]=  (int) &Track  ; 
// }

//_______________________________________________________________________
void AliRecPoint::Copy(AliRecPoint& recp) const
{
  //
  // Copy *this onto pts
  //
  // Copy all first
  if(this != &recp) {
    ((TObject*) this)->Copy(dynamic_cast<TObject&>(recp));
    recp.fAmp = fAmp;
    recp.fGeom = fGeom;
    recp.fIndexInList = fIndexInList;
    recp.fLocPos = fLocPos;
    recp.fLocPosM = new TMatrix(*fLocPosM);
    recp.fMaxDigit = fMaxDigit;
    recp.fMulDigit = fMulDigit;
    recp.fMaxTrack = fMaxTrack;
    recp.fMulTrack = fMulTrack;
    
    // Duplicate pointed objects
    recp.fDigitsList = new Int_t[fMulDigit];
    memcpy(recp.fDigitsList,fDigitsList,fMulDigit*sizeof(Int_t));
    recp.fTracksList = new Int_t[fMulTrack];
    memcpy(recp.fTracksList,fTracksList,fMulTrack*sizeof(Int_t));
  }
}

//_______________________________________________________________________
void AliRecPoint::GetCovarianceMatrix(TMatrix & mat) const
{
  // returns the covariant matrix for the local position
  
  mat = *fLocPosM ; 

}

//____________________________________________________________________________
void AliRecPoint::GetLocalPosition(TVector3 & pos) const
{
  // returns the position of the cluster in the local reference system of the sub-detector

  pos = fLocPos;

 
}

//____________________________________________________________________________
void AliRecPoint::GetGlobalPosition(TVector3 & gpos, TMatrix & gmat) const
{
  // returns the position of the cluster in the global reference system of ALICE
  // and the uncertainty on this position
  

  fGeom->GetGlobal(this, gpos, gmat) ;
 
}



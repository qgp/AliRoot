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
Revision 1.7.4.1  2002/06/03 09:55:04  hristov
Merged with v3-08-02

Revision 1.10  2002/10/14 14:57:44  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.7.6.2  2002/07/24 10:09:31  alibrary
Updating VirtualMC

Revision 1.7.6.1  2002/06/10 15:28:58  hristov
Merged with v3-08-02

Revision 1.8  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.9  2002/06/12 09:54:35  cblume
Update of tracking code provided by Sergei

Revision 1.8  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.7  2001/12/05 15:04:34  hristov
Changes related to the corrections of AliRecPoint

Revision 1.6  2001/02/14 18:22:26  cblume
Change in the geometry of the padplane

Revision 1.5  2000/11/14 14:40:27  cblume
Correction for the Sun compiler (kTRUE and kFALSE)

Revision 1.4  2000/11/01 14:53:21  cblume
Merge with TRD-develop

Revision 1.1.4.2  2000/10/04 16:34:58  cblume
Replace include files by forward declarations

Revision 1.1.4.1  2000/09/22 14:50:39  cblume
Adapted to tracking code

Revision 1.3  2000/06/09 11:10:07  cblume
Compiler warnings and coding conventions, next round

Revision 1.2  2000/06/08 18:32:58  cblume
Make code compliant to coding conventions

Revision 1.1  2000/02/28 19:02:07  cblume
Add new TRD classes

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD reconstructed point                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliRun.h"

#include "AliTRDgeometry.h"
#include "AliTRDrecPoint.h"
#include "AliTRD.h"

ClassImp(AliTRDrecPoint)

//_____________________________________________________________________________
AliTRDrecPoint::AliTRDrecPoint():AliRecPoint()
{
  //
  // Standard constructor
  //

  fDetector = 0;

  AliTRD *trd;
  if ((gAlice) &&
      (trd = ((AliTRD*) gAlice->GetDetector("TRD")))) {
    fGeom = trd->GetGeometry();
  }
  else {
    fGeom = NULL;
  }

}

//_____________________________________________________________________________
AliTRDrecPoint::AliTRDrecPoint(const char * opt):AliRecPoint(opt)
{
  //
  // Standard constructor
  //

  fDetector = 0;

  AliTRD *trd;
  if ((gAlice) &&
      (trd = ((AliTRD*) gAlice->GetDetector("TRD")))) {
    fGeom = trd->GetGeometry();
  }
  else {
    fGeom = NULL;
  }

}

//_____________________________________________________________________________
AliTRDrecPoint::~AliTRDrecPoint()
{
  //
  // AliTRDrecPoint destructor
  //

}

//_____________________________________________________________________________
void AliTRDrecPoint::AddDigit(Int_t digit)
{
  //
  // Adds the index of a digit to the digits list
  //

  // First resize the list 
  // (no clusters with more than 3 digits for the TRD
  if ((fMulDigit == 0) && (fMaxDigit >= 5)) {
    fMaxDigit = 5;
    delete fDigitsList;
    fDigitsList = new int[fMaxDigit];
  }

  // Increase the size of the list if necessary
  if (fMulDigit >= fMaxDigit) { 
    fMaxDigit *= 2;
    int *tempo = new (int[fMaxDigit]); 
    Int_t index; 
    for (index = 0; index < fMulDigit; index++)
      tempo[index] = fDigitsList[index]; 
    delete fDigitsList; 
    fDigitsList = tempo; 
  }
  
  fDigitsList[fMulDigit++] = digit;

}

//_____________________________________________________________________________
void AliTRDrecPoint::SetLocalPosition(TVector3 &pos)
{
  //
  // Sets the position of the point in the local coordinate system
  // (row,col,time) and calculates the error matrix in the same
  // system.
  //

  //const Float_t kSq12 = 3.464101615;

  // Set the position
  //fLocPos = pos;

  // Set the error matrix
  // row:  pad-size / sqrt(12)
  // col:  not defined yet
  // time: bin-size / sqrt(12)
  //Int_t plane   = ((AliTRDgeometry *) fGeom)->GetPlane(fDetector);
  //Int_t chamber = ((AliTRDgeometry *) fGeom)->GetChamber(fDetector);
  //Int_t sector  = ((AliTRDgeometry *) fGeom)->GetSector(fDetector);
  //fLocPosM->operator()(0,0) = ((AliTRDgeometry *) fGeom)->GetRowPadSize(plane
  //                                                                     ,chamber
  //                                                                     ,sector) 
  //                          / kSq12;
  //fLocPosM->operator()(1,1) = 0.0;
  //fLocPosM->operator()(2,2) = ((AliTRDgeometry *) fGeom)->GetTimeBinSize() 
  //                          / kSq12;

  //  printf("rec. point: row = %f, col = %f, time = %f \n",
  //           fLocPos[0],fLocPos[1],fLocPos[2]); 

}

//_____________________________________________________________________________
void AliTRDrecPoint::SetTrackingYZ(Float_t sigmaY, Float_t sigmaZ)
{
 //
 // Sets the position of the point in the local coordinate system
 // of tracking sector
 //

  //Int_t plane = ((AliTRDgeometry *) fGeom)->GetPlane(fDetector);
  //Int_t chamber = ((AliTRDgeometry *) fGeom)->GetChamber(fDetector);
  //Int_t sector = ((AliTRDgeometry *) fGeom)->GetSector(fDetector);


 // Set the position

  //Float_t   padRow    = fLocPos[0];             // Pad Row position
  //Float_t   padCol    = fLocPos[1];             // Pad Column position

  //Float_t   col0 = ((AliTRDgeometry *) fGeom)->GetCol0(plane);
  //Float_t   row0 = ((AliTRDgeometry *) fGeom)->GetRow0(plane,chamber,sector);

  //  Float_t   offset = 0.5 * ((AliTRDgeometry *) fGeom)->GetChamberWidth(plane);

  //fY = - (col0 + padCol * ((AliTRDgeometry *) fGeom)->GetColPadSize(plane));
  //fZ =    row0 + padRow * ((AliTRDgeometry *) fGeom)->GetRowPadSize(plane
  //                                                                   ,chamber
  //                                                                 ,sector);

  //  fSigmaY = sigmaY * sigmaY;
  //  fSigmaZ = sigmaZ * sigmaZ;

//fSigmaY2 = 0.05 * 0.05;

//fSigmaZ2 = ((AliTRDgeometry *) fGeom)->GetRowPadSize(plane,chamber,sector)
//         * ((AliTRDgeometry *) fGeom)->GetRowPadSize(plane,chamber,sector) 
//         / 12.;

}                                    

//_____________________________________________________________________________
void AliTRDrecPoint::AddTrackIndex(Int_t *track)
{
 // Adds track index. Currently assumed that track is an array of
 // size 9, and up to 3 track indexes are stored in fTracks[3].
 // Indexes are sorted according to:
 //  1) index of max number of appearances is stored first
 //  2) if two or more indexes appear equal number of times, the lowest
 //     ones are stored first;

  const Int_t kSize = 9;

  Int_t entries[kSize][2], i, j, index;

  Bool_t indexAdded;

  for (i=0; i<kSize; i++) {
    entries[i][0]=-1;
    entries[i][1]=0;
  }


  for (Int_t k=0; k<kSize; k++) {
    index=track[k];
    indexAdded=kFALSE; j=0;
    if (index >= 0) {
      while ( (!indexAdded) && ( j < kSize ) ) {
        if ((entries[j][0]==index) || (entries[j][1]==0)) {
          entries[j][0]=index;
          entries[j][1]=entries[j][1]+1;
          indexAdded=kTRUE;
        }
        j++;
      }
    }
  }                   

  // sort by number of appearances and index value
  Int_t swap=1, tmp0, tmp1;
  while ( swap > 0) {
    swap=0;
    for(i=0; i<(kSize-1); i++) {
      if ((entries[i][0] >= 0) && (entries[i+1][0] >= 0)) {
        if ((entries[i][1] < entries[i+1][1]) ||
            ((entries[i][1] == entries[i+1][1]) &&
             (entries[i][0] > entries[i+1][0]))) {
               tmp0=entries[i][0];
               tmp1=entries[i][1];
               entries[i][0]=entries[i+1][0];
               entries[i][1]=entries[i+1][1];
               entries[i+1][0]=tmp0;
               entries[i+1][1]=tmp1;
               swap++;
        }
      }
    }
  }

  // set track indexes

  for(i=0; i<3; i++) {
    fTracks[i] = entries[i][0];
  }

  return;

}                    








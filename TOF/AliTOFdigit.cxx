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
//  TOF digit: member variables 
//  fSector  : TOF sector
//  fPlate   : TOF plate
//  fStrip   : strips number
//  fPadx    : pad number along x
//  fPadz    : pad number along z
//  fTdc     : TDC
//  fAdc     : ADC
//              
//  Getters, setters and member functions  defined here
//
//*-- Authors: F. Pierella, A. Seganti, D. Vicinanza

 
#include <Riostream.h>

#include <TVirtualMC.h>

#include "AliRun.h"
#include "AliTOF.h"
#include "AliTOFdigit.h"

ClassImp(AliTOFdigit)

//______________________________________________________________________________
AliTOFdigit::AliTOFdigit(Int_t *tracks, Int_t *vol,Float_t *digit)
:AliDigit(tracks)
{
//
// Constructor of digit object
//
  fSector = vol[0];
  fPlate  = vol[1];
  fStrip  = vol[2];
  fPadx  = vol[3];
  fPadz  = vol[4];
  fTdc    = digit[0];
  fAdc    = digit[1];
}

//____________________________________________________________________________
AliTOFdigit::AliTOFdigit(const AliTOFdigit & digit)
{
  // 
  // copy ctor for AliTOFdigit object
  //

  Int_t i ;
  for ( i = 0; i < 3 ; i++)
    fTracks[i]  = digit.fTracks[i] ;
  fSector = digit.fSector;
  fPlate  = digit.fPlate;
  fStrip  = digit.fStrip;
  fPadx   = digit.fPadx;
  fPadz   = digit.fPadz;
  fTdc    = digit.fTdc;
  fAdc    = digit.fAdc;

}

//______________________________________________________________________________
AliTOFdigit::AliTOFdigit(Int_t sector, Int_t plate, Int_t strip, Int_t padx,
Int_t padz, Float_t tdc, Float_t adc)
{
//
// Constructor for sdigit
//
  fSector = sector;
  fPlate  = plate;
  fStrip  = strip;
  fPadx   = padx;
  fPadz   = padz;  
  fTdc    = tdc;   
  fAdc    = adc;     
}
   
//______________________________________________________________________________
void AliTOFdigit::GetLocation(Int_t *Loc) const
{
//
// Get the cohordinates of the digit
// in terms of Sector - Plate - Strip - Pad
//

   Loc[0]=fSector;
   Loc[1]=fPlate;
   Loc[2]=fStrip;
   Loc[3]=fPadx;
   Loc[4]=fPadz;
}

//______________________________________________________________________________
Int_t AliTOFdigit::GetTotPad() const
{
//
// Get the "total" index of the pad inside a Sector
// starting from the digits data.
//

  AliTOF* tof;
  
  if(gAlice){
     tof =(AliTOF*) gAlice->GetDetector("TOF");
  }else{
     printf("AliTOFdigit::GetTotPad - No AliRun object present, exiting");
     return 0;
  }
  
  Int_t pad = fPadx+tof->GetNpadX()*(fPadz-1);
  Int_t before=0;

  switch(fPlate){ 
  case 1: before = 0;
          break;
  case 2: before = tof->GetNStripC();
          break;
  case 3: before = tof->GetNStripB() + tof->GetNStripC();
          break;
  case 4: before = tof->GetNStripA() + tof->GetNStripB() + tof->GetNStripC();
          break;
  case 5: before = tof->GetNStripA() + 2*tof->GetNStripB() + tof->GetNStripC();
          break;
  }
  
  Int_t strip = fStrip+before;
  Int_t padTot = tof->GetPadXStr()*(strip-1)+pad;
  return padTot;
}

//______________________________________________________________________________
void AliTOFdigit::AddTrack(Int_t track)
{
//
// Add a new and different track to the digit 
//
  if (track==fTracks[0] || track==fTracks[1] || track==fTracks[2]) return;
   if (fTracks[1]==0){
      fTracks[1] = track;
   }else if (fTracks[2]==0){
      fTracks[2] = track;
   }else{
   // printf("AliTOFdigit::AddTrack ERROR: Too many Tracks (>3) \n");
   }
}

// Overloading of Streaming, Sum and Comparison operators

//______________________________________________________________________________
Bool_t AliTOFdigit::operator==(AliTOFdigit const &digit) const
{
//
// Overloading of Comparison operator
//   
 if (fSector==digit.fSector &&
     fPlate==digit.fPlate &&
     fStrip==digit.fStrip &&
     fPadx==digit.fPadx &&
     fPadz==digit.fPadz &&
     fTdc==digit.fTdc &&
     fAdc==digit.fAdc) return kTRUE;
     else return kFALSE;
}

//______________________________________________________________________________
AliTOFdigit& AliTOFdigit::operator+(AliTOFdigit const &digit)
{
//
// Overloading of Sum operator
// Note: Some convolution 
// between the two digit variables has to be inserted
//
if  (fSector==digit.fSector &&
     fPlate==digit.fPlate &&
     fStrip==digit.fStrip &&
     fPadx==digit.fPadx &&
     fPadz==digit.fPadz) {
                            // convolution to be inserted here
                             fTdc+=digit.fTdc;
                             fAdc+=digit.fAdc;
                           } else
                AliTOFdigit(fSector,fPlate,fStrip,fPadx,fPadz,fTdc,fAdc);
  return *this;
}

//______________________________________________________________________________
ostream& operator << (ostream& out, const AliTOFdigit &digit)
{
//
// Output streamer: output of the digit data
//
out << "Sector " << digit.fSector << ", Plate " << digit.fPlate << ", Strip " << digit.fStrip << endl;
out << "Padx" << digit.fPadx << ", Padz " << digit.fPadz << endl;
out << "TDC " << digit.fTdc << ", ADC "<< digit.fAdc << endl;
return out;
}


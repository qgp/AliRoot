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
//  TOF sdigit: member variables 
//  fSector  : TOF sector
//  fPlate   : TOF plate
//  fStrip   : strips number
//  fPadx    : pad number along x
//  fPadz    : pad number along z
//  fTdc     : TArrayF of TDC values
//  fAdc     : TArrayF of ADC values
//              
//  Getters, setters and member functions  defined here
//
//*-- Authors: F. Pierella, A. Seganti, D. Vicinanza

#include <Riostream.h>
#include "TArrayF.h"
#include "TArrayI.h"

#include "AliRun.h"
#include "AliTOF.h"
#include "AliTOFConstants.h"
#include "AliTOFSDigit.h"

ClassImp(AliTOFSDigit)

////////////////////////////////////////////////////////////////////////
  AliTOFSDigit::AliTOFSDigit()
{
  //
  // default ctor
  //
  fNDigits = 0;
  fTdc = 0;
  fAdc = 0;
  fTracks = 0;
}

////////////////////////////////////////////////////////////////////////
AliTOFSDigit::AliTOFSDigit(Int_t tracknum, Int_t *vol,Float_t *digit)
{
  //
  // Constructor of digit object
  //
  fSector = vol[0];
  fPlate  = vol[1];
  fStrip  = vol[2];
  fPadx   = vol[3];
  fPadz   = vol[4];
  fNDigits = 1;
  fTdc = new TArrayF(fNDigits);
  (*fTdc)[0] = digit[0];
  fAdc = new TArrayF(fNDigits);
  (*fAdc)[0] = digit[1];
  fTracks = new TArrayI(kMAXDIGITS*fNDigits);
  (*fTracks)[0] = tracknum;
  for (Int_t i = 1; i <kMAXDIGITS*fNDigits; i++) {
    (*fTracks)[i] = -1;
  }
}

////////////////////////////////////////////////////////////////////////
AliTOFSDigit::AliTOFSDigit(const AliTOFSDigit & digit)
{
  // 
  // copy ctor for AliTOFSDigit object
  //
  fSector = digit.fSector;
  fPlate  = digit.fPlate;
  fStrip  = digit.fStrip;
  fPadx   = digit.fPadx;
  fPadz   = digit.fPadz;
  fNDigits = digit.fNDigits;
  fTdc = new TArrayF(*digit.fTdc);  
  fAdc = new TArrayF(*digit.fAdc);
  fTracks = new TArrayI(*digit.fTracks);
}

////////////////////////////////////////////////////////////////////////
AliTOFSDigit::AliTOFSDigit(Int_t sector, Int_t plate, Int_t strip, Int_t padx,
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
  fNDigits = 1;
  fTdc = new TArrayF(fNDigits);
  (*fTdc)[0] = tdc;   
  fAdc = new TArrayF(fNDigits);
  (*fAdc)[0] = adc;   
  // no tracks were specified, set them to -1
  fTracks = new TArrayI(kMAXDIGITS*fNDigits);
  for (Int_t i = 0; i <kMAXDIGITS*fNDigits; i++) {
    (*fTracks)[i] = -1;
  }
}

////////////////////////////////////////////////////////////////////////
void AliTOFSDigit::GetLocation(Int_t *Loc) const
{
  //
  // Get the coordinates of the digit
  // in terms of Sector - Plate - Strip - Pad
  //
  
  Loc[0]=fSector;
  Loc[1]=fPlate;
  Loc[2]=fStrip;
  Loc[3]=fPadx;
  Loc[4]=fPadz;
}

////////////////////////////////////////////////////////////////////////
void AliTOFSDigit::Update(Float_t tdcbin, Int_t tdc, Int_t adc, Int_t track)
{
  //
  // Add charge and track
  //
  
  Int_t sameTime = -1;
  Float_t tdcwindow=((Float_t)AliTOFConstants::fgkTimeDiff)/tdcbin;
  for (Int_t i = 0; i < fNDigits; i++) {
    if (TMath::Abs(tdc-fTdc->At(i)) < tdcwindow) {
      sameTime = i;
      break;
    }
  }
  
  if (sameTime >= 0) {
    (*fAdc)[sameTime] += static_cast<Float_t>(adc);
    // update track - find the first -1  value and replace it by the
    // track number
    for (Int_t iTrack=0; iTrack<kMAXDIGITS; iTrack++) {
      if ((*fTracks)[sameTime*kMAXDIGITS+iTrack] == -1) {
	(*fTracks)[sameTime*kMAXDIGITS+iTrack] = track;
	break;
      }
      // write warning about many tracks going to this pad
      if (iTrack == kMAXDIGITS) {
	cerr<<"WARNING: AliTOFSDigit::Update  Many hits in the padhit"<<endl;
	cerr<<"         ";
	//	PrintPad();
      }
    }
  } else {
    // add new time slot
    fNDigits++;
    fTdc->Set(fNDigits);
    (*fTdc)[fNDigits-1] = tdc;
    fAdc->Set(fNDigits);
    (*fAdc)[fNDigits-1] = adc;
    fTracks->Set(fNDigits*kMAXDIGITS);
    (*fTracks)[(fNDigits-1)*kMAXDIGITS] = track;
    for (Int_t i = 1; i <kMAXDIGITS; i++) {
      (*fTracks)[(fNDigits-1)*kMAXDIGITS+i] = -1;
    }
  }
  
}

////////////////////////////////////////////////////////////////////////
void AliTOFSDigit::Update(AliTOFSDigit* sdig)
{

  //
  // Perform the sum with sdig
  //

  // start loop on all sdig locations
  Int_t nlocations=sdig->GetNDigits();

  for (Int_t j = 0; j < nlocations; j++) {
    Float_t tdcbin=50.; // [ps] hardwired for the time being
    Int_t tdc=(Int_t)sdig->GetTdc(j);
    Int_t adc=(Int_t)sdig->GetAdc(j);
    // getting here only the first track number
    Int_t track=GetTrack(j,0);
    
    
    Int_t sameTime = -1;
    Float_t tdcwindow=((Float_t)AliTOFConstants::fgkTimeDiff)/tdcbin;
    for (Int_t i = 0; i < fNDigits; i++) {
      if (TMath::Abs(tdc-fTdc->At(i)) < tdcwindow) {
	sameTime = i;
	break;
      }
    }
    
    if (sameTime >= 0) {
      (*fAdc)[sameTime] += static_cast<Float_t>(adc);
      // update track - find the first -1  value and replace it by the
      // track number
      for (Int_t iTrack=0; iTrack<kMAXDIGITS; iTrack++) {
	if ((*fTracks)[sameTime*kMAXDIGITS+iTrack] == -1) {
	  (*fTracks)[sameTime*kMAXDIGITS+iTrack] = track;
	  break;
	}
	// write warning about many tracks going to this pad
	if (iTrack == kMAXDIGITS) {
	  cerr<<"WARNING: AliTOFSDigit::Update  Many hits in the padhit"<<endl;
	  cerr<<"         ";
	  //	PrintPad();
	}
      }
    } else {
      // add new time slot
      fNDigits++;
      fTdc->Set(fNDigits);
      (*fTdc)[fNDigits-1] = tdc;
      fAdc->Set(fNDigits);
      (*fAdc)[fNDigits-1] = adc;
      fTracks->Set(fNDigits*kMAXDIGITS);
      (*fTracks)[(fNDigits-1)*kMAXDIGITS] = track;
      for (Int_t i = 1; i <kMAXDIGITS; i++) {
	(*fTracks)[(fNDigits-1)*kMAXDIGITS+i] = -1;
      } // for (Int_t i = 1; i <kMAXDIGITS; i++)
    } // if (sameTime >= 0)
  } // end loop on sdig locations
}

////////////////////////////////////////////////////////////////////////
AliTOFSDigit::~AliTOFSDigit()
{
  //
  // dtor
  //
  delete fTdc;
  delete fAdc;
  delete fTracks;
}

////////////////////////////////////////////////////////////////////////

Int_t AliTOFSDigit::GetTotPad() const
{
  //
  // Get the "total" index of the pad inside a Sector
  // starting from the digits data.
  //
  
  Int_t pad = fPadx+AliTOFConstants::fgkNpadX*(fPadz-1);
  Int_t before=0;
  
  switch(fPlate){ 
  case 1: before = 0;
    break;
  case 2: before = AliTOFConstants::fgkNStripC;
    break;
  case 3: before = AliTOFConstants::fgkNStripB + AliTOFConstants::fgkNStripC;
    break;
  case 4: before = AliTOFConstants::fgkNStripA + AliTOFConstants::fgkNStripB + AliTOFConstants::fgkNStripC;
    break;
  case 5: before = AliTOFConstants::fgkNStripA + 2*AliTOFConstants::fgkNStripB + AliTOFConstants::fgkNStripC;
    break;
  }
  
  Int_t strip = fStrip+before;
  Int_t padTot = AliTOFConstants::fgkPadXStrip*(strip-1)+pad;
  return padTot;
}


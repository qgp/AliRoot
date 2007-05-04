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

//-----------------------------------------------------------------
//           Implementation of the DetectorTag class
//   This is the class to deal with the tags in the detector level
//   Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//-----------------------------------------------------------------

#include "AliDetectorTag.h"
#include "AliLog.h"
#include "TObjString.h"

ClassImp(AliDetectorTag)

//___________________________________________________________________________
AliDetectorTag::AliDetectorTag() :
  TObject(),
  fDetectorArray(new TObjArray()),
  fMask(0),
  fITSSPD(kFALSE),
  fITSSDD(kFALSE),
  fITSSSD(kFALSE),
  fTPC(kFALSE),
  fTRD(kFALSE),
  fTOF(kFALSE),
  fHMPID(kFALSE),
  fPHOS(kFALSE),
  fPMD(kFALSE),
  fMUON(kFALSE),
  fFMD(kFALSE),
  fTZERO(kFALSE),
  fVZERO(kFALSE),
  fZDC(kFALSE),
  fEMCAL(kFALSE)
{
  // Default constructor
  for(Int_t k = 0; k < 20; k++) fDetectors[k] = 0;
}

//___________________________________________________________________________
AliDetectorTag::AliDetectorTag(const AliDetectorTag & detTag) :
  TObject(detTag),
  fDetectorArray(detTag.fDetectorArray),
  fMask(detTag.fMask),
  fITSSPD(detTag.fITSSPD),
  fITSSDD(detTag.fITSSDD),
  fITSSSD(detTag.fITSSSD),
  fTPC(detTag.fTPC),
  fTRD(detTag.fTRD),
  fTOF(detTag.fTOF),
  fHMPID(detTag.fHMPID),
  fPHOS(detTag.fPHOS),
  fPMD(detTag.fPMD),
  fMUON(detTag.fMUON),
  fFMD(detTag.fFMD),
  fTZERO(detTag.fTZERO),
  fVZERO(detTag.fVZERO),
  fZDC(detTag.fZDC),
  fEMCAL(detTag.fEMCAL)
 {
  // DetectorTag copy constructor
  for(Int_t k = 0; k < 20; k++) fDetectors[k] = detTag.fDetectors[k];
}

//___________________________________________________________________________
AliDetectorTag & AliDetectorTag::operator=(const AliDetectorTag &detTag) {
  //DetectorTag assignment operator
  if (this != &detTag) {
    TObject::operator=(detTag);
    
    fDetectorArray = detTag.fDetectorArray;
    fMask = detTag.fMask;   
    fITSSPD = detTag.fITSSPD;
    fITSSDD = detTag.fITSSDD;
    fITSSSD = detTag.fITSSSD;
    fTPC = detTag.fTPC;
    fTRD = detTag.fTRD;
    fTOF = detTag.fTOF;
    fHMPID = detTag.fHMPID;
    fPHOS = detTag.fPHOS;
    fPMD = detTag.fPMD;
    fMUON = detTag.fMUON;
    fFMD = detTag.fFMD;
    fTZERO = detTag.fTZERO;
    fVZERO = detTag.fVZERO;
    fZDC = detTag.fZDC;
    fEMCAL = detTag.fEMCAL;
    for(Int_t k = 0; k < 20; k++) fDetectors[k] = detTag.fDetectors[k];
  }
  return *this;
}

//___________________________________________________________________________
AliDetectorTag::~AliDetectorTag() {
  // Destructor
  delete fDetectorArray;
}

//___________________________________________________________________________
void AliDetectorTag::Int2Bin() {
  // Convert the integer into binary
  Int_t j=0; 
  UInt_t mask = fMask;
  for(Int_t k = 0; k < 20; k++) fDetectors[k] = 0;
  while(mask > 0) {
   fDetectors[j] = mask%2;
   mask = mask/2;
   j++; 
  }
  SetDetectorConfiguration();
}

//___________________________________________________________________________
void AliDetectorTag::SetDetectorConfiguration() {
  //sets the detector configuration
  if(fDetectors[0] == 1) {
    SetITSSPD(); fDetectorArray->Add(new TObjString("SPD"));}
  if(fDetectors[1] == 1) {
    SetITSSDD(); fDetectorArray->Add(new TObjString("SDD"));}
  if(fDetectors[2] == 1) {
    SetITSSSD(); fDetectorArray->Add(new TObjString("SSD"));}
  if(fDetectors[3] == 1) {
    SetTPC(); fDetectorArray->Add(new TObjString("TPC"));}
  if(fDetectors[4] == 1) {
    SetTRD(); fDetectorArray->Add(new TObjString("TRD"));}
  if(fDetectors[5] == 1) {
    SetTOF(); fDetectorArray->Add(new TObjString("TOF"));}
  if(fDetectors[6] == 1) {
    SetHMPID();fDetectorArray->Add(new TObjString("HMPID"));}
  if(fDetectors[7] == 1) {
    SetPHOS(); fDetectorArray->Add(new TObjString("PHOS"));}
  if(fDetectors[9] == 1) {
    SetPMD(); fDetectorArray->Add(new TObjString("PMD"));}
  if(fDetectors[10] == 1) {
    SetMUON(); fDetectorArray->Add(new TObjString("MUON"));}
  if(fDetectors[12] == 1) {
    SetFMD(); fDetectorArray->Add(new TObjString("FMD"));}
  if(fDetectors[13] == 1) {
    SetTZERO(); fDetectorArray->Add(new TObjString("T0"));}
  if(fDetectors[14] == 1) {
    SetVZERO(); fDetectorArray->Add(new TObjString("VZERO"));}
  if(fDetectors[15] == 1) {
    SetZDC(); fDetectorArray->Add(new TObjString("ZDC"));}
  if(fDetectors[18] == 1) {
    SetEMCAL(); fDetectorArray->Add(new TObjString("EMCAL"));}
}

//___________________________________________________________________________
void AliDetectorTag::PrintDetectorMask() {
  //prints the detector mask
  AliInfo( Form( "ITS-SPD: %d", GetITSSPD()) );
  AliInfo( Form( "ITS-SDD: %d", GetITSSDD()) );
  AliInfo( Form( "ITS-SSD: %d", GetITSSSD()) );
  AliInfo( Form( "TPC: %d", GetTPC()) );
  AliInfo( Form( "TRD: %d", GetTRD()) );
  AliInfo( Form( "TOF: %d", GetTOF()) );
  AliInfo( Form( "HMPID: %d", GetHMPID()) );
  AliInfo( Form( "PHOS: %d", GetPHOS()) );
  AliInfo( Form( "PMD: %d", GetPMD()) );
  AliInfo( Form( "MUON: %d", GetMUON()) );
  AliInfo( Form( "FMD: %d", GetFMD()) );
  AliInfo( Form( "TZERO: %d", GetTZERO()) );
  AliInfo( Form( "VZERO: %d", GetVZERO()) );
  AliInfo( Form( "ZDC: %d", GetZDC()) );
  AliInfo( Form( "EMCAL: %d", GetEMCAL()) );
}

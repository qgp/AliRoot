/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

//-------------------------------------------------------------------------
//     AOD event base class
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include "AliAODHeader.h"

ClassImp(AliAODHeader)

//______________________________________________________________________________
AliAODHeader::AliAODHeader() : 
  AliVHeader(),
  fMagneticField(-999.),
  fMuonMagFieldScale(-999.),
  fCentrality(-999.),
  fZDCN1Energy(-999.),
  fZDCP1Energy(-999.),
  fZDCN2Energy(-999.),
  fZDCP2Energy(-999.),
  fNQTheta(0),
  fQTheta(0x0),
  fTriggerMask(0),
  fRunNumber(-999),  
  fRefMult(-999),
  fRefMultPos(-999),
  fRefMultNeg(-999),
  fEventType(0),
  fOrbitNumber(0),
  fPeriodNumber(0),
  fBunchCrossNumber(0),
  fTriggerCluster(0)
{
  // default constructor

  SetName("header");
  for(int j=0; j<2; j++) fZDCEMEnergy[j] = -999.;
}

//______________________________________________________________________________
AliAODHeader::AliAODHeader(Int_t nRun, 
			   UShort_t nBunchX,
			   UInt_t nOrbit,
			   UInt_t nPeriod,
			   Char_t *title) :
  AliVHeader(),
  fMagneticField(-999.),
  fMuonMagFieldScale(-999.),
  fCentrality(-999.),
  fZDCN1Energy(-999.),
  fZDCP1Energy(-999.),
  fZDCN2Energy(-999.),
  fZDCP2Energy(-999.),
  fNQTheta(0),
  fQTheta(0x0),
  fTriggerMask(0),
  fRunNumber(nRun),
  fRefMult(-999),
  fRefMultPos(-999),
  fRefMultNeg(-999),
  fEventType(0),
  fOrbitNumber(nOrbit),
  fPeriodNumber(nPeriod),
  fBunchCrossNumber(nBunchX),
  fTriggerCluster(0)
{
  // constructor

  SetName("header");
  SetTitle(title);
  for(int j=0; j<2; j++) fZDCEMEnergy[j] = -999.;
}

//______________________________________________________________________________
AliAODHeader::AliAODHeader(Int_t nRun, 
			   UShort_t nBunchX,
			   UInt_t nOrbit,
			   UInt_t nPeriod,
			   Int_t refMult,
			   Int_t refMultPos,
			   Int_t refMultNeg,
			   Double_t magField,
			   Double_t muonMagFieldScale,
			   Double_t cent,
			   Double_t n1Energy,
			   Double_t p1Energy,
			   Double_t n2Energy,
			   Double_t p2Energy,
			   Double_t *emEnergy,
 			   ULong64_t trigMask,
			   UChar_t trigClus,
			   UInt_t evttype,
			   Char_t *title) :
  AliVHeader(),
  fMagneticField(magField),
  fMuonMagFieldScale(muonMagFieldScale),
  fCentrality(cent),
  fZDCN1Energy(n1Energy),
  fZDCP1Energy(p1Energy),
  fZDCN2Energy(n2Energy),
  fZDCP2Energy(p2Energy),
  fNQTheta(0),
  fQTheta(0x0),
  fTriggerMask(trigMask),
  fRunNumber(nRun),  
  fRefMult(refMult),
  fRefMultPos(refMultPos),
  fRefMultNeg(refMultNeg),
  fEventType(evttype),
  fOrbitNumber(nOrbit),
  fPeriodNumber(nPeriod),
  fBunchCrossNumber(nBunchX),
  fTriggerCluster(trigClus)
{
  // constructor

  SetName("header");
  SetTitle(title);
  for(int j=0; j<2; j++) fZDCEMEnergy[j] = emEnergy[j];
}

//______________________________________________________________________________
AliAODHeader::~AliAODHeader() 
{
  // destructor
  
  RemoveQTheta();
}

//______________________________________________________________________________
AliAODHeader::AliAODHeader(const AliAODHeader& hdr) :
  AliVHeader(hdr),
  fMagneticField(hdr.fMagneticField),
  fMuonMagFieldScale(hdr.fMuonMagFieldScale),
  fCentrality(hdr.fCentrality),
  fZDCN1Energy(hdr.fZDCN1Energy),
  fZDCP1Energy(hdr.fZDCP1Energy),
  fZDCN2Energy(hdr.fZDCN2Energy),
  fZDCP2Energy(hdr.fZDCP2Energy),
  fNQTheta(0),
  fQTheta(0x0),
  fTriggerMask(hdr.fTriggerMask),
  fRunNumber(hdr.fRunNumber),  
  fRefMult(hdr.fRefMult), 
  fRefMultPos(hdr.fRefMultPos), 
  fRefMultNeg(hdr.fRefMultNeg),
  fEventType(hdr.fEventType),
  fOrbitNumber(hdr.fOrbitNumber),
  fPeriodNumber(hdr.fPeriodNumber),
  fBunchCrossNumber(hdr.fBunchCrossNumber),
  fTriggerCluster(hdr.fTriggerCluster)
{
  // Copy constructor.
  
  SetName(hdr.fName);
  SetTitle(hdr.fTitle);
  SetQTheta(hdr.fQTheta, hdr.fNQTheta);
  SetZDCEMEnergy(hdr.fZDCEMEnergy[0], hdr.fZDCEMEnergy[1]);
}

//______________________________________________________________________________
AliAODHeader& AliAODHeader::operator=(const AliAODHeader& hdr)
{
  // Assignment operator
  if(this!=&hdr) {
    
     AliVHeader::operator=(hdr);
    
    fMagneticField    = hdr.fMagneticField;
    fMuonMagFieldScale= hdr.fMuonMagFieldScale;
    fCentrality       = hdr.fCentrality;
    fZDCN1Energy      = hdr.fZDCN1Energy;
    fZDCP1Energy      = hdr.fZDCP1Energy;
    fZDCN2Energy      = hdr.fZDCN2Energy;
    fZDCP2Energy      = hdr.fZDCP2Energy;
    fTriggerMask      = hdr.fTriggerMask;
    fRunNumber        = hdr.fRunNumber;
    fRefMult          = hdr.fRefMult;
    fRefMultPos       = hdr.fRefMultPos;
    fRefMultNeg       = hdr.fRefMultNeg;
    fEventType        = hdr.fEventType;
    fOrbitNumber      = hdr.fOrbitNumber;
    fPeriodNumber     = hdr.fPeriodNumber;
    fBunchCrossNumber = hdr.fBunchCrossNumber;
    fTriggerCluster   = hdr.fTriggerCluster;

    SetName(hdr.fName);
    SetTitle(hdr.fTitle);
    SetQTheta(hdr.fQTheta, hdr.fNQTheta);
    SetZDCEMEnergy(hdr.fZDCEMEnergy[0], hdr.fZDCEMEnergy[1]);
  }


  return *this;
}

//______________________________________________________________________________
void AliAODHeader::SetQTheta(Double_t *QTheta, UInt_t size) 
{
  if (QTheta && size>0) {
    if (size != (UInt_t)fNQTheta) {
      RemoveQTheta();
      fNQTheta = size;
      fQTheta = new Double_t[fNQTheta];
    }
    
    for (Int_t i = 0; i < fNQTheta; i++) {
      fQTheta[i] = QTheta[i];
    }
  } else {
    RemoveQTheta();
  }

  return;
}

//______________________________________________________________________________
Double_t AliAODHeader::GetQTheta(UInt_t i) const
{
  if (fQTheta && i < (UInt_t)fNQTheta) {
    return fQTheta[i];
  } else {
    return -999.;
  }
}

//______________________________________________________________________________
void AliAODHeader::RemoveQTheta()
{
  delete[] fQTheta;
  fQTheta = 0x0;
  fNQTheta = 0;

  return;
}

//______________________________________________________________________________
void AliAODHeader::Print(Option_t* /*option*/) const 
{
  // prints event information

  printf("Run #                   : %d\n", fRunNumber);
  printf("Bunch Crossing  #       : %d\n", fBunchCrossNumber);
  printf("Orbit Number #          : %d\n", fOrbitNumber);
  printf("Period Number #         : %d\n", fPeriodNumber);
  printf("Trigger mask            : %lld\n", fTriggerMask);
  printf("Trigger cluster         : %d\n", fTriggerCluster);
  printf("Event Type              : %d\n", fEventType);
  printf("Magnetic field          : %f\n", fMagneticField);
  printf("Muon mag. field scale   : %f\n", fMuonMagFieldScale);
  
  printf("Centrality              : %f\n", fCentrality);
  printf("ZDC N1 Energy           : %f\n", fZDCN1Energy);
  printf("ZDC P1 Energy           : %f\n", fZDCP1Energy);
  printf("ZDC N2 Energy           : %f\n", fZDCN2Energy);
  printf("ZDC P2 Energy           : %f\n", fZDCP2Energy);
  printf("ZDC EM1 Energy          : %f\n", fZDCEMEnergy[0]);
  printf("ZDC EM2 Energy          : %f\n", fZDCEMEnergy[1]);
  printf("ref. Multiplicity       : %d\n", fRefMult);
  printf("ref. Multiplicity (pos) : %d\n", fRefMultPos);
  printf("ref. Multiplicity (neg) : %d\n", fRefMultNeg);

  if (fQTheta) {
    for (UInt_t i = 0; i<(UInt_t)fNQTheta; i++) {
      printf("QTheta[%d]              : %d\n", i, GetQTheta(i));
    }
  }

  return;
}

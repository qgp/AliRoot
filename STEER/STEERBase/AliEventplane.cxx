/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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

//*****************************************************
//   Class AliEventplane
//   author: Alberica Toia, Johanna Gramling
//*****************************************************
/// A container for the event plane stored in AOD in ESD
 
#include "AliLog.h"
#include "AliEventplane.h"
#include "TVector2.h"
#include "AliVTrack.h"
#include "TObjArray.h"
#include "TArrayF.h"
#include "AliVEvent.h"
#include "AliVVZERO.h"

ClassImp(AliEventplane)

AliEventplane::AliEventplane() : TNamed("Eventplane", "Eventplane"),
  fQVector(0),
  fQContributionX(0),
  fQContributionY(0),
  fQContributionXsub1(0),
  fQContributionYsub1(0),
  fQContributionXsub2(0),
  fQContributionYsub2(0),
  fEventplaneQ(-1),
  fQsub1(0),
  fQsub2(0),
  fQsubRes(0)
{
  /// constructor
  fQContributionX = new TArrayF(0);
  fQContributionY = new TArrayF(0);
  fQContributionXsub1 = new TArrayF(0);
  fQContributionYsub1 = new TArrayF(0);
  fQContributionXsub2 = new TArrayF(0);
  fQContributionYsub2 = new TArrayF(0);
  for(Int_t i = 0; i < 8; ++i) {
    fMeanX2[i]=fMeanY2[i]=0.;
    fAPlus[i]=fAMinus[i]=1.;
    fLambdaPlus[i]=fLambdaMinus[i]=0.;
    fCos8Psi[i]=0.;
  }
}

AliEventplane::AliEventplane(const AliEventplane& ep) : 
  TNamed(),
  fQVector(0),
  fQContributionX(0),
  fQContributionY(0),
  fQContributionXsub1(0),
  fQContributionYsub1(0),
  fQContributionXsub2(0),
  fQContributionYsub2(0),
  fEventplaneQ(0),
  fQsub1(0),
  fQsub2(0),
  fQsubRes(0)
{
  /// Copy constructor
  ((AliEventplane &) ep).CopyEP(*this);
}

AliEventplane& AliEventplane::operator=(const AliEventplane& ep)
{
  /// Assignment operator
  if (this!=&ep)
    ((AliEventplane &) ep).CopyEP(*this);

  return *this;
}

void AliEventplane::CopyEP(AliEventplane& ep) const
{ // copy function

  AliEventplane& target = (AliEventplane &) ep;
  if (fQContributionX)
      target.fQContributionX = fQContributionX;
  if (fQContributionY)
      target.fQContributionY = fQContributionY;
  if (fQContributionXsub1)
      target.fQContributionXsub1 = fQContributionXsub1;
  if (fQContributionYsub1)
      target.fQContributionYsub1 = fQContributionYsub1;
  if (fQContributionXsub2)
      target.fQContributionXsub2 = fQContributionXsub2;
  if (fQContributionYsub2)
      target.fQContributionYsub2 = fQContributionYsub2;
  if (fEventplaneQ)
      target.fEventplaneQ = fEventplaneQ;
  if (fQVector)
      target.fQVector = dynamic_cast<TVector2*> (fQVector->Clone());
  if (fQsub1)
      target.fQsub1 = dynamic_cast<TVector2*> (fQsub1->Clone());
  if (fQsub2)
      target.fQsub2 = dynamic_cast<TVector2*> (fQsub2->Clone());
  if (fQsubRes)
      target.fQsubRes = fQsubRes;

  for(Int_t i = 0; i < 8; ++i) {
    target.fMeanX2[i]=fMeanX2[i];
    target.fMeanY2[i]=fMeanY2[i];
    target.fAPlus[i]=fAPlus[i];
    target.fAMinus[i]=fAMinus[i];
    target.fLambdaPlus[i]=fLambdaPlus[i];
    target.fLambdaMinus[i]=fLambdaMinus[i];
    target.fCos8Psi[i]=fCos8Psi[i];
  }
}

AliEventplane::~AliEventplane()
{
  /// destructor
  if (fQContributionX){
      delete fQContributionX;
      fQContributionX = 0;
  }
  if (fQContributionY){
      delete fQContributionY;
      fQContributionY = 0;
  }
  if (fQContributionXsub1){
      delete fQContributionXsub1;
      fQContributionXsub1 = 0;
  }
  if (fQContributionYsub1){
      delete fQContributionYsub1;
      fQContributionYsub1 = 0;
  }
  if (fQContributionXsub2){
      delete fQContributionXsub2;
      fQContributionXsub2 = 0;
  }
  if (fQContributionYsub2){
      delete fQContributionYsub2;
      fQContributionYsub2 = 0;
  }
  if (fQVector){
      delete fQVector;
      fQVector = 0;
  }
  if (fQsub1){
      delete fQsub1;
      fQsub1 = 0;
  }
    if (fQsub2){
      delete fQsub2;
      fQsub2 = 0;
  }
}

TVector2* AliEventplane::GetQVector()
{
  return fQVector;
}

Double_t AliEventplane::GetEventplane(const char *x, const AliVEvent *event, Int_t harmonic) const
{
  TString method = x;
  if(method.CompareTo("Q")==0)      return fEventplaneQ;
  else if(method.CompareTo("V0A")==0) return CalculateVZEROEventPlane(event, 4, 7, harmonic);
  else if(method.CompareTo("V0C")==0) return CalculateVZEROEventPlane(event, 0, 3, harmonic);
  else if(method.CompareTo("V0")==0)  return CalculateVZEROEventPlane(event, 0, 7, harmonic);

  return -1000.;
}

Double_t AliEventplane::CalculateVZEROEventPlane(const AliVEvent *  event, Int_t firstRing, Int_t lastRing, Int_t harmonic) const
{
  if(!event) {
    AliError("No Event received");
    return -1000.;
  }
  AliVVZERO *vzeroData = event->GetVZEROData();
  if(!vzeroData) {
    AliError("Enable to get VZERO Data");
    return -1000.;
  }
  if(harmonic <= 0) {
    AliError("Required harmonic is less or equal to 0");
    return -1000.;
  }

  Double_t qxTot=0., qyTot=0.;
  Double_t qx, qy;
  Double_t totMult = 0.;
  for(Int_t ring = firstRing; ring <=lastRing; ++ring) {
    qx=qy=0.;
    Double_t trans[2][2];
    trans[0][0] = 1./(1.-fLambdaPlus[ring]*fLambdaMinus[ring]);
    trans[0][1] = -fLambdaMinus[ring]/(1.-fLambdaPlus[ring]*fLambdaMinus[ring]);
    trans[1][0] = -fLambdaPlus[ring]/(1.-fLambdaPlus[ring]*fLambdaMinus[ring]);
    trans[1][1] = 1./(1.-fLambdaPlus[ring]*fLambdaMinus[ring]);
    Double_t multRing = 0.;
    for(Int_t iCh = ring*8; iCh < (ring+1)*8; ++iCh) {
      Double_t phi = TMath::Pi()/8. + TMath::Pi()/4.*(iCh%8);
      Double_t mult = event->GetVZEROEqMultiplicity(iCh);
      qx += mult*TMath::Cos(harmonic*phi);
      qy += mult*TMath::Sin(harmonic*phi);
      multRing += mult;
    }

    if (multRing < 1e-6) continue;
    totMult += multRing;

    if (harmonic == 2) {
      // Recentering
      Double_t qxPrime = qx - fMeanX2[ring];
      Double_t qyPrime = qy - fMeanY2[ring];
      // Twist
      Double_t qxSeconde = trans[0][0]*qxPrime + trans[0][1]*qyPrime;
      Double_t qySeconde = trans[1][0]*qxPrime + trans[1][1]*qyPrime;
      // Rescaling
      Double_t qxTierce = qxSeconde/fAPlus[ring];
      Double_t qyTierce = qySeconde/fAMinus[ring];
      qxTot += qxTierce;
      qyTot += qyTierce;
    }
    else {
      qxTot += qx;
      qyTot += qy;
    }
  }

  // In case of no hits return an invalid event-plane angle
  if (totMult<1e-6) return -999;

  // 4th Fourier momenta in order to flatten the EP within a sector
  Double_t psi = TMath::ATan2(qyTot,qxTot)/harmonic;
  if ((harmonic == 2) && (firstRing == lastRing)) {
    psi += (fCos8Psi[firstRing]*TMath::Sin(2.*4.*psi))/2.;
    if (psi > TMath::Pi()/2) psi -= TMath::Pi();
    if (psi <-TMath::Pi()/2) psi += TMath::Pi();
  }

  return psi;
}

void AliEventplane::SetVZEROEPParams(Int_t ring,
				     Double_t meanX2, Double_t meanY2,
				     Double_t aPlus, Double_t aMinus,
				     Double_t lambdaPlus, Double_t lambdaMinus,
				     Double_t cos8Psi)
{
  // Set the VZERO event-plane
  // flatenning parameters
  if (ring < 0 || ring >=8) AliFatal(Form("Bad ring index (%d)",ring));

  fMeanX2[ring] = meanX2;
  fMeanY2[ring] = meanY2;
  fAPlus[ring] = aPlus;
  fAMinus[ring] = aMinus;
  fLambdaPlus[ring] = lambdaPlus;
  fLambdaMinus[ring] = lambdaMinus;
  fCos8Psi[ring] = cos8Psi;
}

TVector2* AliEventplane::GetQsub1()
{
  return fQsub1;
}

TVector2* AliEventplane::GetQsub2()
{
  return fQsub2;
}

Double_t AliEventplane::GetQsubRes()
{
  return fQsubRes;
}

Bool_t AliEventplane::IsEventInEventplaneClass(Double_t a, Double_t b, const char *x)
{
  TString method = x;
  if ((method.CompareTo("Q")==0) && (fEventplaneQ >=a && fEventplaneQ < b)) return kTRUE;
  else return kFALSE;
}

Double_t AliEventplane::GetQContributionX(AliVTrack* track)
{ 
  return fQContributionX->GetAt(track->GetID());
}

Double_t AliEventplane::GetQContributionY(AliVTrack* track)
{ 
  return fQContributionY->GetAt(track->GetID());
}

Double_t AliEventplane::GetQContributionXsub1(AliVTrack* track)
{ 
  return fQContributionXsub1->GetAt(track->GetID());
}

Double_t AliEventplane::GetQContributionYsub1(AliVTrack* track)
{ 
  return fQContributionYsub1->GetAt(track->GetID());
}

Double_t AliEventplane::GetQContributionXsub2(AliVTrack* track)
{ 
  return fQContributionXsub2->GetAt(track->GetID());
}

Double_t AliEventplane::GetQContributionYsub2(AliVTrack* track)
{ 
  return fQContributionYsub2->GetAt(track->GetID());
}

void AliEventplane::Reset()
{ 
  delete fQVector; fQVector=0;
  fQContributionX->Reset();
  fQContributionY->Reset();
  fQContributionXsub1->Reset();
  fQContributionYsub1->Reset();
  fQContributionXsub2->Reset();
  fQContributionYsub2->Reset();
  fEventplaneQ = -1;
  delete fQsub1; fQsub1=0;
  delete fQsub2; fQsub2=0;
  fQsubRes = 0;
}

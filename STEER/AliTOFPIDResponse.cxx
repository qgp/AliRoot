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

//-----------------------------------------------------------------//
//                                                                 //
//           Implementation of the TOF PID class                   //
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch         //
//                                                                 //
//-----------------------------------------------------------------//

#include "TMath.h"
#include "AliLog.h"

#include "AliTOFPIDResponse.h"

ClassImp(AliTOFPIDResponse)

//_________________________________________________________________________
AliTOFPIDResponse::AliTOFPIDResponse(): 
  fSigma(0),
  fPmax(0),         // zero at 0.5 GeV/c for pp
  fTime0(0)
{
  // Reset T0 info
  ResetT0info();
  SetMomBoundary();
}
//_________________________________________________________________________
AliTOFPIDResponse::AliTOFPIDResponse(Double_t *param):
  fSigma(param[0]),
  fPmax(0),          // zero at 0.5 GeV/c for pp
  fTime0(0)
{
  //
  //  The main constructor
  //
  //

  //fPmax=TMath::Exp(-0.5*3*3)/fSigma; // ~3 sigma at 0.5 GeV/c for PbPb 

  // Reset T0 info
  ResetT0info();
  SetMomBoundary();
}
//_________________________________________________________________________
Double_t 
AliTOFPIDResponse::GetMismatchProbability(Double_t p, Double_t mass) const {
  //
  // Returns the probability of mismatching 
  // assuming 1/(p*beta)^2 scaling
  //
  const Double_t km=0.5;                   // "reference" momentum (GeV/c)

  Double_t ref2=km*km*km*km/(km*km + mass*mass);// "reference" (p*beta)^2
  Double_t p2beta2=p*p*p*p/(p*p + mass*mass);

  return fPmax*ref2/p2beta2;
}
//_________________________________________________________________________
Double_t AliTOFPIDResponse::GetExpectedSigma(Float_t mom, Float_t time, Float_t mass) const {
  //
  // Return the expected sigma of the PID signal for the specified
  // particle type.
  // If the operation is not possible, return a negative value.
  //

  Double_t dpp=0.018*mass/mom;      //mean relative pt resolution;

 
  Double_t sigma = dpp*time/(1.+ mom*mom/(mass*mass));
  
  Int_t index = GetMomBin(mom);

  Double_t t0res = fT0resolution[index];

  return TMath::Sqrt(sigma*sigma + 50.0*50.0/mom/mom + fSigma*fSigma + t0res*t0res);

}
//_________________________________________________________________________
Int_t AliTOFPIDResponse::GetMomBin(Float_t p) const{
  //
  // Returns the momentum bin index
  //

  Int_t i=0;
  while(p > fPCutMin[i] && i < fNmomBins) i++;
  if(i > 0) i--;

  return i;
}
//_________________________________________________________________________
void AliTOFPIDResponse::SetMomBoundary(){
  //
  // Set boundaries for momentum bins
  //

  fPCutMin[0] = 0.3;
  fPCutMin[1] = 0.5;
  fPCutMin[2] = 0.6;
  fPCutMin[3] = 0.7;
  fPCutMin[4] = 0.8;
  fPCutMin[5] = 0.9;
  fPCutMin[6] = 1;
  fPCutMin[7] = 1.2;
  fPCutMin[8] = 1.5;
  fPCutMin[9] = 2;
  fPCutMin[10] = 3;  
}
//_________________________________________________________________________
Float_t AliTOFPIDResponse::GetStartTime(Float_t mom) const {
  //
  // Returns event_time value as estimated by TOF combinatorial algorithm
  //

  Int_t ibin = GetMomBin(mom);
  return GetT0bin(ibin);

}
//_________________________________________________________________________
Float_t AliTOFPIDResponse::GetStartTimeRes(Float_t mom) const {
  //
  // Returns event_time resolution as estimated by TOF combinatorial algorithm
  //

  Int_t ibin = GetMomBin(mom);
  return GetT0binRes(ibin);

}

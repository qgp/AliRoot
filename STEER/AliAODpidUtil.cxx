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

/* $Id: AliAODpidUtil.cxx 38329 2010-01-17 19:17:24Z hristov $ */

//-----------------------------------------------------------------
//           Implementation of the combined PID class
//           For the AOD Class
//           containing information on the particle identification
//      Origin: Rosa Romita, GSI, r.romita@gsi.de
//-----------------------------------------------------------------

#include "AliLog.h"
#include "AliPID.h"
#include "AliAODpidUtil.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODPid.h"
#include "AliTRDPIDResponse.h"
#include "AliESDtrack.h"

ClassImp(AliAODpidUtil)

  Int_t AliAODpidUtil::MakePID(AliAODTrack *track,Double_t *p) const {
  //
  //  Calculate probabilities for all detectors, except if TPConly==kTRUE
  //  and combine PID
  //  
  //   Option TPConly==kTRUE is used during reconstruction, 
  //  because ITS tracking uses TPC pid
  //  HMPID and TRD pid are done in detector reconstructors
  //

  /*
    Float_t TimeZeroTOF = 0;
    if (subtractT0) 
    TimeZeroTOF = event->GetT0();
  */
  Int_t ns=AliPID::kSPECIES;
  Double_t tpcPid[AliPID::kSPECIES];
  MakeTPCPID(track,tpcPid);
  Double_t itsPid[AliPID::kSPECIES];
  Double_t tofPid[AliPID::kSPECIES];
  Double_t trdPid[AliPID::kSPECIES];
  MakeITSPID(track,itsPid);
  MakeTOFPID(track,tofPid);
  //MakeHMPIDPID(track);
  MakeTRDPID(track,trdPid);
  for (Int_t j=0; j<ns; j++) {
    p[j]=tpcPid[j]*itsPid[j]*tofPid[j]*trdPid[j];
  }

  return 0;
}
//_________________________________________________________________________
void AliAODpidUtil::MakeTPCPID(AliAODTrack *track,Double_t *p) const
{
  //
  //  TPC pid using bethe-bloch and gaussian response
  //

  if ((track->GetStatus()&AliESDtrack::kTPCin )==0) return;

  Double_t mom = track->P();
  AliAODPid *pidObj = track->GetDetPid();
  if (pidObj) mom = pidObj->GetTPCmomentum();
  UShort_t nTPCClus=pidObj->GetTPCsignalN();
   
  Double_t dedx=pidObj->GetTPCsignal(); 
  Bool_t mismatch=kTRUE;

  for (Int_t j=0; j<AliPID::kSPECIES; j++) {
    AliPID::EParticleType type=AliPID::EParticleType(j);
    Double_t bethe=fTPCResponse.GetExpectedSignal(mom,type); 
    Double_t sigma=fTPCResponse.GetExpectedSigma(mom,nTPCClus,type);
    if (TMath::Abs(dedx-bethe) > fRange*sigma) {
      p[j]=TMath::Exp(-0.5*fRange*fRange)/sigma;
    } else {
      p[j]=TMath::Exp(-0.5*(dedx-bethe)*(dedx-bethe)/(sigma*sigma))/sigma;
      mismatch=kFALSE;
    }

  }

  if (mismatch)
    for (Int_t j=0; j<AliPID::kSPECIES; j++) p[j]=1./AliPID::kSPECIES;


  return;
}
//_________________________________________________________________________
void AliAODpidUtil::MakeITSPID(AliAODTrack *track,Double_t *p) const
{
  //
  // ITS PID
  //  1) Truncated mean method
  //


  if ((track->GetStatus()&AliESDtrack::kITSin)==0) return;
  UChar_t clumap=track->GetITSClusterMap();
  Int_t nPointsForPid=0;
  for(Int_t i=2; i<6; i++){
   if(clumap&(1<<i)) ++nPointsForPid;
  }
  if(nPointsForPid<3) { // track not to be used for combined PID purposes
    for (Int_t j=0; j<AliPID::kSPECIES; j++) 
      p[j] = 1./AliPID::kSPECIES;
    return;
  }
  Double_t mom=track->P();  
  AliAODPid *pidObj = track->GetDetPid();

  Double_t dedx = pidObj->GetITSsignal();
  Bool_t mismatch = kTRUE;
  Bool_t isSA = kTRUE;
  if(track->GetStatus() & AliESDtrack::kTPCin){
    isSA = kFALSE;
    if (pidObj)
      mom = pidObj->GetTPCmomentum();
  }
  for (Int_t j=0; j<AliPID::kSPECIES; j++) {
    Double_t mass = AliPID::ParticleMass(j);//GeV/c^2
    Double_t bethe = fITSResponse.Bethe(mom,mass);
    Double_t sigma = fITSResponse.GetResolution(bethe,nPointsForPid,isSA);
    if (TMath::Abs(dedx-bethe) > fRange*sigma) {
      p[j]=TMath::Exp(-0.5*fRange*fRange)/sigma;
    } else {
      p[j]=TMath::Exp(-0.5*(dedx-bethe)*(dedx-bethe)/(sigma*sigma))/sigma;
      mismatch=kFALSE;
    }

    // Check for particles heavier than (AliPID::kSPECIES - 1)

  }

  if (mismatch)
    for (Int_t j=0; j<AliPID::kSPECIES; j++) p[j]=1./AliPID::kSPECIES;

  return;

}
//_________________________________________________________________________
void AliAODpidUtil::MakeTOFPID(AliAODTrack *track, Double_t *p) const
{
  //
  //   TOF PID using gaussian response
  //
  if ((track->GetStatus()&AliESDtrack::kTOFout )==0)    return;
  if ((track->GetStatus()&AliESDtrack::kTIME )==0)     return;
  if ((track->GetStatus()&AliESDtrack::kTOFpid )==0)   return;

  Double_t time[AliPID::kSPECIESN];
  Double_t sigma[AliPID::kSPECIESN];
  AliAODPid *pidObj = track->GetDetPid();
  pidObj->GetIntegratedTimes(time);

  for (Int_t iPart = 0; iPart < AliPID::kSPECIES; iPart++) {
    sigma[iPart] = fTOFResponse.GetExpectedSigma(track->P(),time[iPart],AliPID::ParticleMass(iPart));
  }

  AliDebugGeneral("AliESDpid::MakeTOFPID",2,
		  Form("Expected TOF signals [ps]: %f %f %f %f %f",
		       time[AliPID::kElectron],
		       time[AliPID::kMuon],
		       time[AliPID::kPion],
		       time[AliPID::kKaon],
		       time[AliPID::kProton]));

  AliDebugGeneral("AliESDpid::MakeTOFPID",2,
		  Form("Expected TOF std deviations [ps]: %f %f %f %f %f",
		       sigma[AliPID::kElectron],
		       sigma[AliPID::kMuon],
		       sigma[AliPID::kPion],
		       sigma[AliPID::kKaon],
		       sigma[AliPID::kProton]
		       ));

  Double_t tof = pidObj->GetTOFsignal();

  Bool_t mismatch = kTRUE;
  for (Int_t j=0; j<AliPID::kSPECIES; j++) {
    Double_t sig = sigma[j];
    if (TMath::Abs(tof-time[j]) > fRange*sig) {
      p[j] = TMath::Exp(-0.5*fRange*fRange)/sig;
    } else
      p[j] = TMath::Exp(-0.5*(tof-time[j])*(tof-time[j])/(sig*sig))/sig;

    // Check the mismatching
    Double_t mass = AliPID::ParticleMass(j);
    Double_t pm = fTOFResponse.GetMismatchProbability(track->P(),mass);
    if (p[j]>pm) mismatch = kFALSE;

    // Check for particles heavier than (AliPID::kSPECIES - 1)

  }

  if (mismatch)
    for (Int_t j=0; j<AliPID::kSPECIES; j++) p[j]=1./AliPID::kSPECIES;

  return;
}
//_________________________________________________________________________
void AliAODpidUtil::MakeTRDPID(AliAODTrack *track,Double_t *p) const
{
  
  // Method to recalculate the TRD PID probabilities
  if ((track->GetStatus()&AliESDtrack::kTRDout )==0)   return;

  AliAODPid *pidObj = track->GetDetPid();
  Float_t *mom=pidObj->GetTRDmomentum();
  Int_t ntracklets=0;
  for(Int_t iPl=0;iPl<6;iPl++){
   if(mom[iPl]>0.) ntracklets++;
  }
   if(ntracklets<4) return;

  Double_t* dedx=pidObj->GetTRDsignal();
  Bool_t norm=kTRUE;
  fTRDResponse.GetResponse(pidObj->GetTRDnSlices(),dedx,mom,p,norm);
  return;
}

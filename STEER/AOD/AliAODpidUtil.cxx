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

#include "TRandom.h"
#include "AliLog.h"
#include "AliPID.h"
#include "AliAODpidUtil.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODPid.h"
#include "AliTRDPIDResponse.h"
#include "AliESDtrack.h"
#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"
#include "AliTOFPIDParams.h"

#include <AliDetectorPID.h>

ClassImp(AliAODpidUtil)

//_________________________________________________________________________
Float_t AliAODpidUtil::GetITSsignalTunedOnData(const AliVTrack *t) const {
  AliAODTrack *trk = (AliAODTrack *) t;
  Float_t dedx = trk->GetITSsignalTunedOnData();
  if(dedx > 0) return dedx;

  Int_t nITSpid = 0;
  for(Int_t i=2; i<6; i++){
    if(trk->HasPointOnITSLayer(i)) nITSpid++;
  }

  Bool_t isSA = kFALSE;
  if(trk->TestFilterBit(AliAODTrack::kTrkITSsa)==kTRUE) isSA=kTRUE;

  AliPID::EParticleType type = AliPID::kPion;

  AliAODMCHeader *mcHeader = dynamic_cast<AliAODMCHeader*>(trk->GetAODEvent()->GetList()->FindObject(AliAODMCHeader::StdBranchName()));
  if(mcHeader){

    TClonesArray *mcArray = (TClonesArray*)trk->GetAODEvent()->GetList()->FindObject(AliAODMCParticle::StdBranchName());

    Bool_t kGood = kTRUE;
    if ( mcArray->At(TMath::Abs(t->GetLabel())) != NULL ) {  // protects against label-0 tracks e.g. the initial proton for Phythia events
      Int_t part = TMath::Abs(((AliAODMCParticle*)mcArray->At(TMath::Abs(t->GetLabel())))->GetPdgCode());
      if(part==AliPID::ParticleCode(AliPID::kElectron)){ // e
	type = AliPID::kElectron;
      }
      else if(part==AliPID::ParticleCode(AliPID::kPion)){ // pi
	type = AliPID::kPion;
      }
      else if(part==AliPID::ParticleCode(AliPID::kKaon)){ // K
	type = AliPID::kKaon;
      }
      else if(part==AliPID::ParticleCode(AliPID::kProton)){ // p 
	type = AliPID::kProton;
      }
      else if(part==AliPID::ParticleCode(AliPID::kDeuteron)){ // d
	type = AliPID::kDeuteron;
      }
      else if(part==AliPID::ParticleCode(AliPID::kTriton)){ // t
	type = AliPID::kTriton;
      }
      else
	kGood = kFALSE;
    } else kGood = kFALSE;

    if(kGood){
      Double_t bethe = fITSResponse.Bethe(trk->P(),type,isSA);
      Double_t sigma = fITSResponse.GetResolution(bethe,nITSpid,isSA,trk->P(),type);
      dedx = gRandom->Gaus(bethe,sigma);
    }
    
  }
    trk->SetITSsignalTunedOnData(dedx);
    return dedx;
}


//_________________________________________________________________________
Float_t AliAODpidUtil::GetSignalDeltaTOFold(const AliVParticle *vtrack, AliPID::EParticleType type, Bool_t ratio/*=kFALSE*/) const
{
  //
  // Number of sigma implementation for the TOF
  //

  AliAODTrack *track=(AliAODTrack*)vtrack;
  AliAODPid *pidObj = track->GetDetPid();
  if (!pidObj) return -9999.;
  Double_t tofTime = 99999;
  if (fTuneMConData && ((fTuneMConDataMask & kDetTOF) == kDetTOF) ) tofTime = (Double_t)this->GetTOFsignalTunedOnData((AliVTrack*)vtrack);
  else tofTime=pidObj->GetTOFsignal();
  const Double_t expTime=fTOFResponse.GetExpectedSignal((AliVTrack*)vtrack,type);
  Double_t sigmaTOFPid[AliPID::kSPECIES];
  pidObj->GetTOFpidResolution(sigmaTOFPid);
  AliAODEvent *event=(AliAODEvent*)track->GetAODEvent();
  if (event) {  // protection if the user didn't call GetTrack, which sets the internal pointer
    AliTOFHeader* tofH=(AliTOFHeader*)event->GetTOFHeader();
    if (tofH && (TMath::Abs(sigmaTOFPid[0]) <= 1.E-16) ) { // new AOD
        tofTime -= fTOFResponse.GetStartTime(vtrack->P());
    }
  } else {
    AliError("pointer to AliAODEvent not found, please call GetTrack to set it");
    return -9999.;
  }

  Double_t delta=-9999.;

  if (!ratio) delta=tofTime-expTime;
  else if (expTime>1.e-20) delta=tofTime/expTime;

  return delta;
}

//_________________________________________________________________________
Float_t AliAODpidUtil::GetNumberOfSigmasTOFold(const AliVParticle *vtrack, AliPID::EParticleType type) const
{
  //
  // Number of sigma implementation for the TOF
  //

  AliAODTrack *track=(AliAODTrack*)vtrack;

  Bool_t oldAod=kTRUE;
  Double_t sigTOF=0.;
  AliAODPid *pidObj = track->GetDetPid();
  if (!pidObj) return -999.;
  Double_t tofTime = 99999;
  if (fTuneMConData && ((fTuneMConDataMask & kDetTOF) == kDetTOF) ) tofTime = (Double_t)this->GetTOFsignalTunedOnData((AliVTrack*)vtrack);
  else tofTime=pidObj->GetTOFsignal();
  Double_t expTime=fTOFResponse.GetExpectedSignal((AliVTrack*)vtrack,type);
  Double_t sigmaTOFPid[AliPID::kSPECIES];
  pidObj->GetTOFpidResolution(sigmaTOFPid);
  AliAODEvent *event=(AliAODEvent*)track->GetAODEvent();
  if (event) {  // protection if the user didn't call GetTrack, which sets the internal pointer
    AliTOFHeader* tofH=(AliTOFHeader*)event->GetTOFHeader();
    if (tofH && (TMath::Abs(sigmaTOFPid[0]) <= 1.E-16) ) { // new AOD
        sigTOF=fTOFResponse.GetExpectedSigma(track->P(),expTime,AliPID::ParticleMassZ(type)); //fTOFResponse is set in InitialiseEvent
        tofTime -= fTOFResponse.GetStartTime(vtrack->P());
        oldAod=kFALSE;
    }
  } else {
    AliError("pointer to AliAODEvent not found, please call GetTrack to set it");
    return -996.;
  }
  if (oldAod) { // old AOD
    if (type <= AliPID::kProton) {
      sigTOF=sigmaTOFPid[type];
    } else return -998.;  // light nuclei cannot be supported on old AOD because we don't have timeZero resolution
  }
  if (sigTOF>0) return (tofTime - expTime)/sigTOF;
  else return -997.;
}

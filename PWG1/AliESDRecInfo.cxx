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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Time Projection Chamber                                                  //
//  Comparison macro for reconstructed tracks - ESDs V0s                                     //
//  responsible: 
//  marian.ivanov@cern.ch                                                    //
//
//

 



//ROOT includes
#include "Rtypes.h"
//
//ALIROOT includes
//
#include "AliESDtrack.h"
#include "AliTPCParam.h"
#include "AliTrackReference.h"
#include "AliTPCParamSR.h"
#include "AliESDfriend.h"
#include "AliESDtrack.h"
#include "AliTPCseed.h"
#include "AliITStrackMI.h"
#include "AliTRDtrack.h"
#include "AliMCInfo.h"
#include "AliESDRecInfo.h"



ClassImp(AliESDRecInfo)




AliTPCParam * GetTPCParam(){
  AliTPCParamSR * par = new AliTPCParamSR;
  par->Update();
  return par;
}




AliESDRecInfo::AliESDRecInfo(): 
  fITSOn(0),           // ITS refitted inward
  fTRDOn(0),           // ITS refitted inward
  fDeltaP(0),          //delta of momenta
  fSign(0),           // sign
  fReconstructed(0),         //flag if track was reconstructed
  fFake(0),             // fake track
  fMultiple(0),         // number of reconstructions
  fTPCOn(0),           // TPC refitted inward
  fBestTOFmatch(0),        //best matching between times
  fESDtrack(0),        // esd track
  fTrackF(0),      // friend track
  fTPCtrack(0),        // tpc track
  fITStrack(0),        // its track
  fTRDtrack(0)        // trd track  
{
  //
  //  default constructor
  //
}


AliESDRecInfo::AliESDRecInfo(const AliESDRecInfo& recinfo):
  TObject()
{
  //
  //
  //
  memcpy(this,&recinfo, sizeof(recinfo));
  fESDtrack=0; fTrackF=0; fTPCtrack=0;fITStrack=0;fTRDtrack=0;
  SetESDtrack(recinfo.GetESDtrack());
}


AliESDRecInfo::~AliESDRecInfo()

{
  //
  //  destructor
  //
  if (fESDtrack) { delete fESDtrack; fESDtrack=0;}
  if (fTrackF)   { delete fTrackF;   fTrackF=0;}
  if (fTPCtrack) { delete fTPCtrack; fTPCtrack=0;}
  if (fITStrack) { delete fITStrack; fITStrack=0;}
  if (fTRDtrack) { delete fTRDtrack; fTRDtrack=0;}

}



void AliESDRecInfo::Reset()
{
  //
  // reset info
  //
  fMultiple =0; 
  fFake     =0;
  fReconstructed=0;
  if (fESDtrack) { delete fESDtrack; fESDtrack=0;}
  if (fTrackF)   { delete fTrackF;   fTrackF=0;}
  if (fTPCtrack) { delete fTPCtrack; fTPCtrack=0;}
  if (fITStrack) { delete fITStrack; fITStrack=0;}
  if (fTRDtrack) { delete fTRDtrack; fTRDtrack=0;}
} 

void AliESDRecInfo::SetESDtrack(const AliESDtrack *track){
  //
  // 
  //
  if (fESDtrack) delete fESDtrack;
  fESDtrack = (AliESDtrack*)track->Clone();
  if (0 &&track->GetFriendTrack()){
    if (fTrackF) delete fTrackF;
    fTrackF = (AliESDfriendTrack*)track->GetFriendTrack()->Clone();
    if (fTrackF->GetCalibObject(0)){
      if (fTPCtrack) delete fTPCtrack;
      fTPCtrack = (AliTPCseed*)fTrackF->GetCalibObject(0)->Clone();
    }
  }
  
}

void  AliESDRecInfo::UpdatePoints(AliESDtrack*track)
{
  //
  //
  Int_t iclusters[200];
  Float_t density[160];
  for (Int_t i=0;i<160;i++) density[i]=-1.;
  fTPCPoints[0]= 160;
  fTPCPoints[1] = -1;
  //
  if (fTPCPoints[0]<fTPCPoints[1]) return;
  //  Int_t nclusters=track->GetTPCclusters(iclusters);

  Int_t ngood=0;
  Int_t undeff=0;
  Int_t nall =0;
  Int_t range=20;
  for (Int_t i=0;i<160;i++){
    Int_t last = i-range;
    if (nall<range) nall++;
    if (last>=0){
      if (iclusters[last]>0&& (iclusters[last]&0x8000)==0) ngood--;
      if (iclusters[last]==-1) undeff--;
    }
    if (iclusters[i]>0&& (iclusters[i]&0x8000)==0)   ngood++;
    if (iclusters[i]==-1) undeff++;
    if (nall==range &&undeff<range/2) density[i-range/2] = Float_t(ngood)/Float_t(nall-undeff);
  }
  Float_t maxdens=0;
  Int_t indexmax =0;
  for (Int_t i=0;i<160;i++){
    if (density[i]<0) continue;
    if (density[i]>maxdens){
      maxdens=density[i];
      indexmax=i;
    }
  }
  //
  //max dens point
  fTPCPoints[3] = maxdens;
  fTPCPoints[1] = indexmax;
  //
  // last point
  for (Int_t i=indexmax;i<160;i++){
    if (density[i]<0) continue;
    if (density[i]<maxdens/2.) {
      break;
    }
    fTPCPoints[2]=i;
  }
  //
  // first point
  for (Int_t i=indexmax;i>0;i--){
    if (density[i]<0) continue;
    if (density[i]<maxdens/2.) {
      break;
    }
    fTPCPoints[0]=i;
  }
  //
  // Density at the last 30 padrows
  //
  // 
  nall  = 0;
  ngood = 0;
  for (Int_t i=159;i>0;i--){
    if (iclusters[i]==-1) continue; //dead zone
    nall++;
    if (iclusters[i]>0)   ngood++;
    if (nall>20) break;
  }
  fTPCPoints[4] = Float_t(ngood)/Float_t(nall);
  //
  if ((track->GetStatus()&AliESDtrack::kITSrefit)>0) fTPCPoints[0]=-1;


}

//
//
void AliESDRecInfo::Update(AliMCInfo* info,AliTPCParam * /*par*/, Bool_t reconstructed)
{
  //
  //
  //calculates derived variables
  //  
  //
  UpdatePoints(fESDtrack);
  fBestTOFmatch=1000;
  AliTrackReference * ref = &(info->fTrackRef);
  fTPCinR0[0] = info->fTrackRef.X();	
  fTPCinR0[1] = info->fTrackRef.Y();	
  fTPCinR0[2] = info->fTrackRef.Z();
  fTPCinR0[3] = TMath::Sqrt(fTPCinR0[0]*fTPCinR0[0]+fTPCinR0[1]*fTPCinR0[1]);
  fTPCinR0[4] = TMath::ATan2(fTPCinR0[1],fTPCinR0[0]);
  //
  fTPCinP0[0] = ref->Px();
  fTPCinP0[1] = ref->Py();
  fTPCinP0[2] = ref->Pz();
  fTPCinP0[3] = ref->Pt();
  fTPCinP0[4] = ref->P();
  fDeltaP     = (ref->P()-info->fParticle.P())/info->fParticle.P();
  //
  //
  if (fTPCinP0[3]>0.0000001){
    //
    fTPCAngle0[0] = TMath::ATan2(fTPCinP0[1],fTPCinP0[0]);
    fTPCAngle0[1] = TMath::ATan(fTPCinP0[2]/fTPCinP0[3]);
  }
  //
  //
  fITSinP0[0]=info->fParticle.Px();
  fITSinP0[1]=info->fParticle.Py();
  fITSinP0[2]=info->fParticle.Pz();
  fITSinP0[3]=info->fParticle.Pt();    
  //
  fITSinR0[0]=info->fParticle.Vx();
  fITSinR0[1]=info->fParticle.Vy();
  fITSinR0[2]=info->fParticle.Vz();
  fITSinR0[3] = TMath::Sqrt(fITSinR0[0]*fITSinR0[0]+fITSinR0[1]*fITSinR0[1]);
  fITSinR0[4] = TMath::ATan2(fITSinR0[1],fITSinR0[0]);
  //
  //
  if (fITSinP0[3]>0.0000001){
    fITSAngle0[0] = TMath::ATan2(fITSinP0[1],fITSinP0[0]);
    fITSAngle0[1] = TMath::ATan(fITSinP0[2]/fITSinP0[3]);
  }
  //
  for (Int_t i=0;i<4;i++) fStatus[i] =0;
  fReconstructed = kFALSE;
  fTPCOn = kFALSE;
  fITSOn = kFALSE;
  fTRDOn = kFALSE;  
  if (reconstructed==kFALSE) return;

  fLabels[0] = info->fLabel;
  fLabels[1] = info->fPrimPart;
  fReconstructed = kTRUE;
  fTPCOn = ((fESDtrack->GetStatus()&AliESDtrack::kTPCrefit)>0) ? kTRUE : kFALSE;
  fITSOn = ((fESDtrack->GetStatus()&AliESDtrack::kITSrefit)>0) ? kTRUE : kFALSE;
  fTRDOn = ((fESDtrack->GetStatus()&AliESDtrack::kTRDrefit)>0) ? kTRUE : kFALSE;
  //
  //  
  if ((fESDtrack->GetStatus()&AliESDtrack::kTPCrefit)>0){
    fStatus[1] =3;
  }
  else{
    if ((fESDtrack->GetStatus()&AliESDtrack::kTPCout)>0){
      fStatus[1] =2;
    }
    else{
      if ((fESDtrack->GetStatus()&AliESDtrack::kTPCin)>0)
	fStatus[1]=1;
    }      
  }
  //
  if ((fESDtrack->GetStatus()&AliESDtrack::kITSout)>0){
    fStatus[0] =2;
  }
  else{
    if ((fESDtrack->GetStatus()&AliESDtrack::kITSrefit)>0){
      fStatus[0] =1;
    }
    else{
      fStatus[0]=0;
    }      
  }

  //
  //
  if ((fESDtrack->GetStatus()&AliESDtrack::kTRDrefit)>0){
    fStatus[2] =2;
  }
  else{
    if ((fESDtrack->GetStatus()&AliESDtrack::kTRDout)>0){
      fStatus[2] =1;
    }
  }
  if ((fESDtrack->GetStatus()&AliESDtrack::kTRDStop)>0){
    fStatus[2] =10;
  }

  //
  //TOF 
  // 
  if (((fESDtrack->GetStatus()&AliESDtrack::kTOFout)>0)){
    //
    // best tof match
    Double_t times[5];
    fESDtrack->GetIntegratedTimes(times);    
    for (Int_t i=0;i<5;i++){
      if ( TMath::Abs(fESDtrack->GetTOFsignal()-times[i]) <TMath::Abs(fBestTOFmatch) ){
	fBestTOFmatch = fESDtrack->GetTOFsignal()-times[i];
      }
    }
    Int_t toflabel[3];
    fESDtrack->GetTOFLabel(toflabel);
    Bool_t toffake=kTRUE;
    Bool_t tofdaughter=kFALSE;
    for (Int_t i=0;i<3;i++){
      if (toflabel[i]<0) continue;      
      if (toflabel[i]== TMath::Abs(fESDtrack->GetLabel()))  toffake=kFALSE;	
      if (toflabel[i]==info->fParticle.GetDaughter(0) || (toflabel[i]==info->fParticle.GetDaughter(1))) tofdaughter=kTRUE;  // decay product of original particle
      fStatus[3]=1;
    }
    if (toffake) fStatus[3] =3;       //total fake
    if (tofdaughter) fStatus[3]=2;    //fake because of decay
  }else{
    fStatus[3]=0;
  }


  if (fStatus[1]>0 &&info->fNTPCRef>0&&TMath::Abs(fTPCinP0[3])>0.0001){
    //TPC
    fESDtrack->GetInnerXYZ(fTPCinR1);
    fTPCinR1[3] = TMath::Sqrt(fTPCinR1[0]*fTPCinR1[0]+fTPCinR1[1]*fTPCinR1[1]);
    fTPCinR1[4] = TMath::ATan2(fTPCinR1[1],fTPCinR1[0]);	
    fESDtrack->GetInnerPxPyPz(fTPCinP1);
    fTPCinP1[3] = TMath::Sqrt(fTPCinP1[0]*fTPCinP1[0]+fTPCinP1[1]*fTPCinP1[1]);
    fTPCinP1[4] = TMath::Sqrt(fTPCinP1[3]*fTPCinP1[3]+fTPCinP1[2]*fTPCinP1[2]);
    //
    //
    if (fTPCinP1[3]>0.000000000000001){
      fTPCAngle1[0] = TMath::ATan2(fTPCinP1[1],fTPCinP1[0]);
      fTPCAngle1[1] = TMath::ATan(fTPCinP1[2]/fTPCinP1[3]);  
    }    
    Double_t cov[15], param[5],x, alpha;
    fESDtrack->GetInnerExternalCovariance(cov);
    fESDtrack->GetInnerExternalParameters(alpha, x,param);
    if (x<50) return ;
    //
    fTPCDelta[0] = (fTPCinR0[4]-fTPCinR1[4])*fTPCinR1[3];  //delta rfi
    fTPCPools[0] = fTPCDelta[0]/TMath::Sqrt(cov[0]);
    fTPCDelta[1] = (fTPCinR0[2]-fTPCinR1[2]);              //delta z
    fTPCPools[1] = fTPCDelta[1]/TMath::Sqrt(cov[2]);
    fTPCDelta[2] = (fTPCAngle0[0]-fTPCAngle1[0]);
    fTPCPools[2] = fTPCDelta[2]/TMath::Sqrt(cov[5]);
    fTPCDelta[3] = (TMath::Tan(fTPCAngle0[1])-TMath::Tan(fTPCAngle1[1]));
    fTPCPools[3] = fTPCDelta[3]/TMath::Sqrt(cov[9]);
    fTPCDelta[4] = (fTPCinP0[3]-fTPCinP1[3]);
    Double_t sign = (param[4]>0)? 1.:-1; 
    fSign =sign;
    fTPCPools[4] = sign*(1./fTPCinP0[3]-1./fTPCinP1[3])/TMath::Sqrt(TMath::Abs(cov[14]));
  }
  if (fITSOn){
    // ITS 
    Double_t param[5],x;
    fESDtrack->GetExternalParameters(x,param);   
    //    fESDtrack->GetConstrainedExternalParameters(x,param);   
    Double_t cov[15];
    fESDtrack->GetExternalCovariance(cov);
    //fESDtrack->GetConstrainedExternalCovariance(cov);
    if (TMath::Abs(param[4])<0.0000000001) return;

    fESDtrack->GetXYZ(fITSinR1);
    fESDtrack->GetPxPyPz(fITSinP1);
    fITSinP1[3] = TMath::Sqrt(fITSinP1[0]*fITSinP1[0]+fITSinP1[1]*fITSinP1[1]);
    //
    fITSinR1[3] = TMath::Sqrt(fITSinR1[0]*fITSinR1[0]+fITSinR1[1]*fITSinR1[1]);
    fITSinR1[4] = TMath::ATan2(fITSinR1[1],fITSinR1[0]);
    //
    //
    if (fITSinP1[3]>0.0000001){
      fITSAngle1[0] = TMath::ATan2(fITSinP1[1],fITSinP1[0]);
      fITSAngle1[1] = TMath::ATan(fITSinP1[2]/fITSinP1[3]);  
    }
    //
    //
    fITSDelta[0] = (fITSinR0[4]-fITSinR1[4])*fITSinR1[3];  //delta rfi
    fITSPools[0] = fITSDelta[0]/TMath::Sqrt(cov[0]);
    fITSDelta[1] = (fITSinR0[2]-fITSinR1[2]);              //delta z
    fITSPools[1] = fITSDelta[1]/TMath::Sqrt(cov[2]);
    fITSDelta[2] = (fITSAngle0[0]-fITSAngle1[0]);
    fITSPools[2] = fITSDelta[2]/TMath::Sqrt(cov[5]);
    fITSDelta[3] = (TMath::Tan(fITSAngle0[1])-TMath::Tan(fITSAngle1[1]));
    fITSPools[3] = fITSDelta[3]/TMath::Sqrt(cov[9]);
    fITSDelta[4] = (fITSinP0[3]-fITSinP1[3]);    
    Double_t sign = (param[4]>0) ? 1:-1; 
    fSign = sign;
    fITSPools[4] = sign*(1./fITSinP0[3]-1./fITSinP1[3])/TMath::Sqrt(cov[14]);    
  }
  
}



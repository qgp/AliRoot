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

//--------------------------------------------------------------------//
//                                                                    //
// AliTOFtracker Class                                                //
// Task: Perform association of the ESD tracks to TOF Clusters        //
// and Update ESD track with associated TOF Cluster parameters        //
//                                                                    //
// -- Authors : S. Arcelli, C. Zampolli (Bologna University and INFN) //
// -- Contacts: Annalisa.De.Caro@cern.ch                              //
// --         : Chiara.Zampolli@bo.infn.it                            //
// --         : Silvia.Arcelli@bo.infn.it                             //
//                                                                    //
//--------------------------------------------------------------------//

#include <Rtypes.h>
#include <TROOT.h>

#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TTree.h>
#include <TFile.h>
#include <TH2F.h>

#include "AliAlignObj.h"
#include "AliESDtrack.h"
#include "AliESD.h"
#include "AliLog.h"
#include "AliTrackPointArray.h"

#include "AliTOFcalib.h"
#include "AliTOFRecoParam.h"
#include "AliTOFcluster.h"
#include "AliTOFGeometryV5.h"
#include "AliTOFtracker.h"
#include "AliTOFtrack.h"

extern TGeoManager *gGeoManager;
extern TROOT *gROOT;

ClassImp(AliTOFtracker)

//_____________________________________________________________________________
AliTOFtracker::AliTOFtracker():
  fRecoParam(0x0),
  fGeom(0x0),
  fPid(0x0),
  fN(0),
  fNseeds(0),
  fNseedsTOF(0),
  fngoodmatch(0),
  fnbadmatch(0),
  fnunmatch(0),
  fnmatch(0),
  fTracks(0x0),
  fSeeds(0x0),
  fHDigClusMap(0x0),
  fHDigNClus(0x0),
  fHDigClusTime(0x0),
  fHDigClusToT(0x0),
  fHRecNClus(0x0),
  fHRecDist(0x0),
  fHRecSigYVsP(0x0),
  fHRecSigZVsP(0x0),
  fHRecSigYVsPWin(0x0),
  fHRecSigZVsPWin(0x0),
  fCalTree(0x0),
  fIch(-1),
  fToT(-1.),
  fTime(-1.),
  fExpTimePi(-1.),
  fExpTimeKa(-1.),
  fExpTimePr(-1.)
 { 
  //AliTOFtracker main Ctor
   
   // Gettimg the geometry 
   fGeom=new AliTOFGeometryV5();
   // Read the reconstruction parameters from the OCDB
   AliTOFcalib *calib = new AliTOFcalib(fGeom);
   fRecoParam = (AliTOFRecoParam*)calib->ReadRecParFromCDB("TOF/Calib",-1);
   if(fRecoParam->GetApplyPbPbCuts())fRecoParam=fRecoParam->GetPbPbparam();
   Double_t parPID[2];   
   parPID[0]=fRecoParam->GetTimeResolution();
   parPID[1]=fRecoParam->GetTimeNSigma();
   fPid=new AliTOFpidESD(parPID);
   InitCheckHists();

}
//_____________________________________________________________________________
AliTOFtracker::AliTOFtracker(const AliTOFtracker &t):
  AliTracker(),
  fRecoParam(0x0),
  fGeom(0x0),
  fPid(0x0),
  fN(0),
  fNseeds(0),
  fNseedsTOF(0),
  fngoodmatch(0),
  fnbadmatch(0),
  fnunmatch(0),
  fnmatch(0),
  fTracks(0x0),
  fSeeds(0x0),
  fHDigClusMap(0x0),
  fHDigNClus(0x0),
  fHDigClusTime(0x0),
  fHDigClusToT(0x0),
  fHRecNClus(0x0),
  fHRecDist(0x0),
  fHRecSigYVsP(0x0),
  fHRecSigZVsP(0x0),
  fHRecSigYVsPWin(0x0),
  fHRecSigZVsPWin(0x0),
  fCalTree(0x0),
  fIch(-1),
  fToT(-1.),
  fTime(-1.),
  fExpTimePi(-1.),
  fExpTimeKa(-1.),
  fExpTimePr(-1.)
 { 
  //AliTOFtracker copy Ctor

  fNseeds=t.fNseeds;
  fNseeds=t.fNseeds;
  fNseedsTOF=t.fNseedsTOF;
  fngoodmatch=t.fngoodmatch;
  fnbadmatch=t.fnbadmatch;
  fnunmatch=t.fnunmatch;
  fnmatch=t.fnmatch;
  fRecoParam=t.fRecoParam;
  fGeom=t.fGeom;
  fPid=t.fPid;
  fSeeds=t.fSeeds;
  fTracks=t.fTracks;
  fN=t.fN;
}

//_____________________________________________________________________________
AliTOFtracker& AliTOFtracker::operator=(const AliTOFtracker &t)
{ 
  //AliTOFtracker assignment operator

  this->fNseeds=t.fNseeds;
  this->fNseedsTOF=t.fNseedsTOF;
  this->fngoodmatch=t.fngoodmatch;
  this->fnbadmatch=t.fnbadmatch;
  this->fnunmatch=t.fnunmatch;
  this->fnmatch=t.fnmatch;
  this->fRecoParam = t.fRecoParam;
  this->fGeom = t.fGeom;
  this->fPid = t.fPid;
  this->fSeeds=t.fSeeds;
  this->fTracks=t.fTracks;
  this->fN=t.fN;
  return *this;

}
//_____________________________________________________________________________
AliTOFtracker::~AliTOFtracker() {
  //
  // Dtor
  //

  SaveCheckHists();

  delete fRecoParam; 
  delete fGeom; 
  delete fPid; 
  delete fHDigClusMap;
  delete fHDigNClus;
  delete fHDigClusTime;
  delete fHDigClusToT;
  delete fHRecNClus;
  delete fHRecDist;
  delete fHRecSigYVsP;
  delete fHRecSigZVsP;
  delete fHRecSigYVsPWin;
  delete fHRecSigZVsPWin;
  delete fCalTree;
}
//_____________________________________________________________________________
Int_t AliTOFtracker::PropagateBack(AliESD* event) {
  //
  // Gets seeds from ESD event and Match with TOF Clusters
  //


  //Initialise some counters

  fNseeds=0;
  fNseedsTOF=0;
  fngoodmatch=0;
  fnbadmatch=0;
  fnunmatch=0;
  fnmatch=0;

  Int_t ntrk=event->GetNumberOfTracks();
  fNseeds = ntrk;
  fSeeds= new TClonesArray("AliESDtrack",ntrk);
  TClonesArray &aESDTrack = *fSeeds;


  //Load ESD tracks into a local Array of ESD Seeds

  for (Int_t i=0; i<fNseeds; i++) {
    AliESDtrack *t=event->GetTrack(i);
    new(aESDTrack[i]) AliESDtrack(*t);
  }

  //Prepare ESD tracks candidates for TOF Matching
  CollectESD();

  //First Step with Strict Matching Criterion
  MatchTracks(kFALSE);

  //Second Step with Looser Matching Criterion
  MatchTracks(kTRUE);

  AliInfo(Form("Number of matched tracks: %d",fnmatch));
  AliInfo(Form("Number of good matched tracks: %d",fngoodmatch));
  AliInfo(Form("Number of bad  matched tracks: %d",fnbadmatch));

  //Update the matched ESD tracks

  for (Int_t i=0; i<ntrk; i++) {
    AliESDtrack *t=event->GetTrack(i);
    AliESDtrack *seed =(AliESDtrack*)fSeeds->UncheckedAt(i);
    if(seed->GetTOFsignal()>0){
      t->SetTOFsignal(seed->GetTOFsignal());
      t->SetTOFcluster(seed->GetTOFcluster());
      t->SetTOFsignalToT(seed->GetTOFsignalToT());
      t->SetTOFCalChannel(seed->GetTOFCalChannel());
      Int_t tlab[3]; seed->GetTOFLabel(tlab);    
      t->SetTOFLabel(tlab);
      AliTOFtrack *track = new AliTOFtrack(*seed); 
      t->UpdateTrackParams(track,AliESDtrack::kTOFout);   
      delete track;
    }
  }

  //Handle Time Zero information

  Double_t timeZero=0.;
  Double_t timeZeroMax=99999.;
  Bool_t usetimeZero     = fRecoParam->UseTimeZero();
  Bool_t timeZeroFromT0  = fRecoParam->GetTimeZerofromT0();
  Bool_t timeZeroFromTOF = fRecoParam->GetTimeZerofromTOF();

  AliDebug(1,Form("Use Time Zero?: %d",usetimeZero));
  AliDebug(1,Form("Time Zero from T0? : %d",timeZeroFromT0));
  AliDebug(1,Form("Time Zero From TOF? : %d",timeZeroFromTOF));

  if(usetimeZero){
    if(timeZeroFromT0){
      timeZero=GetTimeZerofromT0(event); 
    }
    if(timeZeroFromTOF && (timeZero>timeZeroMax || !timeZeroFromT0)){
      timeZero=GetTimeZerofromTOF(event); 
    }
  }
  AliDebug(2,Form("time Zero used in PID: %f",timeZero));
  //Make TOF PID
  fPid->MakePID(event,timeZero);

  if (fSeeds) {
    fSeeds->Delete();
    delete fSeeds;
    fSeeds = 0x0;
  }
  if (fTracks) {
    fTracks->Delete();
    delete fTracks;
    fTracks = 0x0;
  }
  return 0;
  
}
//_________________________________________________________________________
void AliTOFtracker::CollectESD() {
   //prepare the set of ESD tracks to be matched to clusters in TOF
 
  fTracks= new TClonesArray("AliTOFtrack");
  TClonesArray &aTOFTrack = *fTracks;
  for (Int_t i=0; i<fNseeds; i++) {

    AliESDtrack *t =(AliESDtrack*)fSeeds->UncheckedAt(i);
    if ((t->GetStatus()&AliESDtrack::kTPCout)==0)continue;

    // TRD good tracks, already propagated at 371 cm

    AliTOFtrack *track = new AliTOFtrack(*t); // New
    Double_t x = track->GetX(); //New

    if (((t->GetStatus()&AliESDtrack::kTRDout)!=0 ) && 
	 ( x >= fGeom->RinTOF()) ){
      track->SetSeedIndex(i);
      t->UpdateTrackParams(track,AliESDtrack::kTOFout);    
      new(aTOFTrack[fNseedsTOF]) AliTOFtrack(*track);
      fNseedsTOF++;
      delete track;
    }

    // Propagate the rest of TPCbp  

    else {
      if(track->PropagateToInnerTOF()){ 
      	track->SetSeedIndex(i);
	t->UpdateTrackParams(track,AliESDtrack::kTOFout);    
 	new(aTOFTrack[fNseedsTOF]) AliTOFtrack(*track);
	fNseedsTOF++;
      }
      delete track;
    }
  }

  AliInfo(Form("Number of TOF seedds %i",fNseedsTOF));

  // Sort according uncertainties on track position 
  fTracks->Sort();

}
//_________________________________________________________________________
void AliTOFtracker::MatchTracks( Bool_t mLastStep){


  // Parameters used/regulating the reconstruction

  static Float_t corrLen=0.75;
  static Float_t detDepth=15.3;

  Float_t dY=fGeom->XPad(); 
  Float_t dZ=fGeom->ZPad(); 

  Float_t sensRadius = fRecoParam->GetSensRadius();
  Float_t stepSize   = fRecoParam->GetStepSize();
  Float_t scaleFact   = fRecoParam->GetWindowScaleFact();
  Float_t dyMax=fRecoParam->GetWindowSizeMaxY(); 
  Float_t dzMax=fRecoParam->GetWindowSizeMaxZ();
  Float_t dCut=fRecoParam->GetDistanceCut();
  Double_t maxChi2=fRecoParam->GetMaxChi2();
  Bool_t timeWalkCorr    = fRecoParam->GetTimeWalkCorr();
  if(!mLastStep){
    AliDebug(1,"++++++++++++++TOF Reconstruction Parameters:++++++++++++ \n");
    AliDebug(1,Form("TOF sens radius: %f",sensRadius));
    AliDebug(1,Form("TOF step size: %f",stepSize));
    AliDebug(1,Form("TOF Window scale factor: %f",scaleFact));
    AliDebug(1,Form("TOF Window max dy: %f",dyMax));
    AliDebug(1,Form("TOF Window max dz: %f",dzMax));
    AliDebug(1,Form("TOF distance Cut: %f",dCut));
    AliDebug(1,Form("TOF Max Chi2: %f",maxChi2));
    AliDebug(1,Form("Time Walk Correction? : %d",timeWalkCorr));   
  }
  //Match ESD tracks to clusters in TOF


  // Get the number of propagation steps

  Int_t nSteps=(Int_t)(detDepth/stepSize);

  AliTOFcalib *calib = new AliTOFcalib(fGeom);

  //PH Arrays (moved outside of the loop)

  Float_t * trackPos[4];
  for (Int_t ii=0; ii<4; ii++) trackPos[ii] = new Float_t[nSteps];
  Int_t * clind[6];
  for (Int_t ii=0;ii<6;ii++) clind[ii] = new Int_t[fN];
  
  for (Int_t iseed=0; iseed<fNseedsTOF; iseed++) {

    AliTOFtrack *track =(AliTOFtrack*)fTracks->UncheckedAt(iseed);
    AliESDtrack *t =(AliESDtrack*)fSeeds->UncheckedAt(track->GetSeedIndex());
    if(t->GetTOFsignal()>0. ) continue;
    AliTOFtrack *trackTOFin =new AliTOFtrack(*track);

    // Some init

    Int_t         index[10000];
    Float_t        dist[10000];
    Float_t       distZ[10000];
    Float_t       cxpos[10000];
    Float_t       crecL[10000];
    TGeoHMatrix   global[1000];
     
    // Determine a window around the track

    Double_t x,par[5]; 
    trackTOFin->GetExternalParameters(x,par);
    Double_t cov[15]; 
    trackTOFin->GetExternalCovariance(cov);

    Double_t dphi=
      scaleFact*
      ((5*TMath::Sqrt(cov[0]) + 0.5*dY + 2.5*TMath::Abs(par[2]))/sensRadius); 
    Double_t dz=
       scaleFact*
       (5*TMath::Sqrt(cov[2]) + 0.5*dZ + 2.5*TMath::Abs(par[3]));

    Double_t phi=TMath::ATan2(par[0],x) + trackTOFin->GetAlpha();
    if (phi<-TMath::Pi())phi+=2*TMath::Pi();
    if (phi>=TMath::Pi())phi-=2*TMath::Pi();
    Double_t z=par[1];   

    //upper limit on window's size.

    if(dz> dzMax) dz=dzMax;
    if(dphi*sensRadius> dyMax) dphi=dyMax/sensRadius;


    Int_t nc=0;

    // find the clusters in the window of the track

    for (Int_t k=FindClusterIndex(z-dz); k<fN; k++) {

      AliTOFcluster *c=fClusters[k];
      if (c->GetZ() > z+dz) break;
      if (c->IsUsed()) continue;

      if (!c->GetStatus()) continue; // skip bad channels as declared in OCDB

      Double_t dph=TMath::Abs(c->GetPhi()-phi);
      if (dph>TMath::Pi()) dph-=2.*TMath::Pi();
      if (TMath::Abs(dph)>dphi) continue;

      Double_t yc=(c->GetPhi() - trackTOFin->GetAlpha())*c->GetR();
      Double_t p[2]={yc, c->GetZ()};
      Double_t cov[3]= {dY*dY/12., 0., dZ*dZ/12.};
      if (trackTOFin->AliExternalTrackParam::GetPredictedChi2(p,cov) > maxChi2)continue;

      clind[0][nc] = c->GetDetInd(0);
      clind[1][nc] = c->GetDetInd(1);
      clind[2][nc] = c->GetDetInd(2);
      clind[3][nc] = c->GetDetInd(3);
      clind[4][nc] = c->GetDetInd(4);
      clind[5][nc] = k;      
      Char_t path[100];
      Int_t ind[5];
      ind[0]=clind[0][nc];
      ind[1]=clind[1][nc];
      ind[2]=clind[2][nc];
      ind[3]=clind[3][nc];
      ind[4]=clind[4][nc];
      fGeom->GetVolumePath(ind,path);
      gGeoManager->cd(path);
      global[nc] = *gGeoManager->GetCurrentMatrix();
      nc++;
    }

    //start fine propagation 

    Int_t nStepsDone = 0;
    for( Int_t istep=0; istep<nSteps; istep++){ 

      Float_t xs=fGeom->RinTOF()+istep*0.1;
      Double_t ymax=xs*TMath::Tan(0.5*fGeom->GetAlpha());

      Bool_t skip=kFALSE;
      Double_t ysect=trackTOFin->GetYat(xs,skip);
      if (skip) break;
      if (ysect > ymax) {
	if (!trackTOFin->Rotate(fGeom->GetAlpha())) {
	  break;
	}
      } else if (ysect <-ymax) {
	if (!trackTOFin->Rotate(fGeom->GetAlpha())) {
	  break;
	}
      }

      if(!trackTOFin->PropagateTo(xs)) {
	break;
      }

      nStepsDone++;

      // store the running point (Globalrf) - fine propagation     

      Double_t r[3];
      trackTOFin->GetXYZ(r);
      trackPos[0][istep]= (Float_t) r[0];
      trackPos[1][istep]= (Float_t) r[1];
      trackPos[2][istep]= (Float_t) r[2];   
      trackPos[3][istep]= trackTOFin->GetIntegratedLength();
    }


    Int_t nfound = 0;
    for (Int_t istep=0; istep<nStepsDone; istep++) {

      Bool_t isInside =kFALSE;
      Float_t ctrackPos[3];	

      ctrackPos[0]= trackPos[0][istep];
      ctrackPos[1]= trackPos[1][istep];
      ctrackPos[2]= trackPos[2][istep];

      //now see whether the track matches any of the TOF clusters            

      for (Int_t i=0; i<nc; i++){
	Int_t cind[5];
	cind[0]= clind[0][i];
	cind[1]= clind[1][i];
	cind[2]= clind[2][i];
	cind[3]= clind[3][i];
	cind[4]= clind[4][i];
        Bool_t accept = kFALSE;
	if( mLastStep)accept = (fGeom->DistanceToPad(cind,global[i],ctrackPos)<dCut);
	if(!mLastStep)accept = (fGeom->IsInsideThePad(cind,global[i],ctrackPos));
	if(accept){
	  if(!mLastStep)isInside=kTRUE;
          Float_t dist3d[3];
	  dist[nfound]=fGeom->DistanceToPad(cind,global[i],ctrackPos,dist3d);
	  distZ[nfound]=dist3d[2];
	  crecL[nfound]=trackPos[3][istep];
	  index[nfound]=clind[5][i]; // store cluster id 	    
	  cxpos[nfound]=fGeom->RinTOF()+istep*0.1; //store prop.radius
	  nfound++;
	  if(isInside)break;
	}//end if accept
      } //end for on the clusters


      if(isInside)break;
    } //end for on the steps     



    if (nfound == 0 ) {
      fnunmatch++;
      delete trackTOFin;
      continue;
    }
    
    fnmatch++;

    // now choose the cluster to be matched with the track.

    Int_t idclus=0;
    Float_t  recL = 0.;
    Float_t  xpos=0.;
    Float_t  mindist=1000.;
    Float_t  mindistZ=0.;
    for (Int_t iclus= 0; iclus<nfound;iclus++){
      if (dist[iclus]< mindist){
      	mindist = dist[iclus];
      	mindistZ = distZ[iclus];
      	xpos = cxpos[iclus];
        idclus =index[iclus]; 
        recL=crecL[iclus]+corrLen*0.5;
      }
    }

    AliTOFcluster *c=fClusters[idclus];
    c->Use(); 

    // Track length correction for matching Step 2 

    if(mLastStep){
      Float_t rc=TMath::Sqrt(c->GetR()*c->GetR() + c->GetZ()*c->GetZ());
      Float_t rt=TMath::Sqrt(trackPos[0][70]*trackPos[0][70]
			     +trackPos[1][70]*trackPos[1][70]
			     +trackPos[2][70]*trackPos[2][70]);
      Float_t dlt=rc-rt;      
      recL=trackPos[3][70]+dlt;
    }    

    if (
	(c->GetLabel(0)==TMath::Abs(trackTOFin->GetLabel()))
	||
	(c->GetLabel(1)==TMath::Abs(trackTOFin->GetLabel()))
	||
	(c->GetLabel(2)==TMath::Abs(trackTOFin->GetLabel()))
	) {
      fngoodmatch++;

       AliDebug(2,Form(" track label good %5i",trackTOFin->GetLabel()));

    }
    else{
      fnbadmatch++;

      AliDebug(2,Form(" track label  bad %5i",trackTOFin->GetLabel()));

    }

    delete trackTOFin;

    //  Store quantities to be used in the TOF Calibration
    Float_t tToT=fGeom->ToTBinWidth()*c->GetToT()*1E-3; // in ns
    t->SetTOFsignalToT(tToT);
    Int_t ind[5];
    ind[0]=c->GetDetInd(0);
    ind[1]=c->GetDetInd(1);
    ind[2]=c->GetDetInd(2);
    ind[3]=c->GetDetInd(3);
    ind[4]=c->GetDetInd(4);
    Int_t calindex = calib->GetIndex(ind);
    t->SetTOFCalChannel(calindex);

    // keep track of the track labels in the matched cluster
    Int_t tlab[3];
    tlab[0]=c->GetLabel(0);
    tlab[1]=c->GetLabel(1);
    tlab[2]=c->GetLabel(2);
    
    Double_t tof=fGeom->TdcBinWidth()*c->GetTDC()+32; // in ps
    if(timeWalkCorr)tof=CorrectTimeWalk(mindistZ,tof);
    t->SetTOFsignal(tof);
    t->SetTOFcluster(idclus); // pointing to the recPoints tree
    Double_t time[AliPID::kSPECIES]; t->GetIntegratedTimes(time);
    Double_t mom=t->GetP();
    for(Int_t j=0;j<AliPID::kSPECIES;j++){
      Double_t mass=AliPID::ParticleMass(j);
      time[j]+=(recL-trackPos[3][0])/3e-2*TMath::Sqrt(mom*mom+mass*mass)/mom;
    }

    AliTOFtrack *trackTOFout = new AliTOFtrack(*t); 
    trackTOFout->PropagateTo(xpos);
    t->UpdateTrackParams(trackTOFout,AliESDtrack::kTOFout);    
    t->SetIntegratedLength(recL);
    t->SetIntegratedTimes(time);
    t->SetTOFLabel(tlab);
    // Fill Reco-QA histos for Reconstruction
    fHRecNClus->Fill(nc);
    fHRecDist->Fill(mindist);
    fHRecSigYVsP->Fill(mom,TMath::Sqrt(cov[0]));
    fHRecSigZVsP->Fill(mom,TMath::Sqrt(cov[2]));
    fHRecSigYVsPWin->Fill(mom,dphi*sensRadius);
    fHRecSigZVsPWin->Fill(mom,dz);

    // Fill Tree for on-the-fly offline Calibration

    if ( !((t->GetStatus() & AliESDtrack::kTIME)==0 )){    
      Float_t rawtime=fGeom->TdcBinWidth()*c->GetTDCRAW()+32; // RAW time,in ps
      fIch=calindex;
      fToT=tToT;
      fTime=rawtime;
      fExpTimePi=time[2];
      fExpTimeKa=time[3];
      fExpTimePr=time[4];
      fCalTree->Fill();
    }
    delete trackTOFout;
  }
  for (Int_t ii=0; ii<4; ii++) delete [] trackPos[ii];
  for (Int_t ii=0;ii<6;ii++) delete [] clind[ii];
  delete calib;
}
//_________________________________________________________________________
Int_t AliTOFtracker::LoadClusters(TTree *cTree) {
  //--------------------------------------------------------------------
  //This function loads the TOF clusters
  //--------------------------------------------------------------------

  Int_t npadX = fGeom->NpadX();
  Int_t npadZ = fGeom->NpadZ();
  Int_t nStripA = fGeom->NStripA();
  Int_t nStripB = fGeom->NStripB();
  Int_t nStripC = fGeom->NStripC();

  TBranch *branch=cTree->GetBranch("TOF");
  if (!branch) { 
    AliError("can't get the branch with the TOF clusters !");
    return 1;
  }

  TClonesArray dummy("AliTOFcluster",10000), *clusters=&dummy;
  branch->SetAddress(&clusters);

  cTree->GetEvent(0);
  Int_t nc=clusters->GetEntriesFast();
  fHDigNClus->Fill(nc);

  AliInfo(Form("Number of clusters: %d",nc));

  for (Int_t i=0; i<nc; i++) {
    AliTOFcluster *c=(AliTOFcluster*)clusters->UncheckedAt(i);
    fClusters[i]=new AliTOFcluster(*c); fN++;

  // Fill Digits QA histos
 
    Int_t isector = c->GetDetInd(0);
    Int_t iplate = c->GetDetInd(1);
    Int_t istrip = c->GetDetInd(2);
    Int_t ipadX = c->GetDetInd(4);
    Int_t ipadZ = c->GetDetInd(3);

    Float_t time =(fGeom->TdcBinWidth()*c->GetTDC())*1E-3; // in ns
    Float_t tot = (fGeom->TdcBinWidth()*c->GetToT())*1E-3;//in ns
 
    Int_t stripOffset = 0;
    switch (iplate) {
    case 0:
      stripOffset = 0;
      break;
    case 1:
      stripOffset = nStripC;
      break;
    case 2:
      stripOffset = nStripC+nStripB;
      break;
    case 3:
      stripOffset = nStripC+nStripB+nStripA;
      break;
    case 4:
      stripOffset = nStripC+nStripB+nStripA+nStripB;
      break;
    default:
      AliError(Form("Wrong plate number in TOF (%d) !",iplate));
      break;
    };
    Int_t zindex=npadZ*(istrip+stripOffset)+(ipadZ+1);
    Int_t phiindex=npadX*isector+ipadX+1;
    fHDigClusMap->Fill(zindex,phiindex);
    fHDigClusTime->Fill(time);
    fHDigClusToT->Fill(tot);

  }


  return 0;
}
//_________________________________________________________________________
void AliTOFtracker::UnloadClusters() {
  //--------------------------------------------------------------------
  //This function unloads TOF clusters
  //--------------------------------------------------------------------
  for (Int_t i=0; i<fN; i++) {
    delete fClusters[i];
    fClusters[i] = 0x0;
  }
  fN=0;
}

//_________________________________________________________________________
Int_t AliTOFtracker::FindClusterIndex(Double_t z) const {
  //--------------------------------------------------------------------
  // This function returns the index of the nearest cluster 
  //--------------------------------------------------------------------
  if (fN==0) return 0;
  if (z <= fClusters[0]->GetZ()) return 0;
  if (z > fClusters[fN-1]->GetZ()) return fN;
  Int_t b=0, e=fN-1, m=(b+e)/2;
  for (; b<e; m=(b+e)/2) {
    if (z > fClusters[m]->GetZ()) b=m+1;
    else e=m; 
  }
  return m;
}

//_________________________________________________________________________
Bool_t AliTOFtracker::GetTrackPoint(Int_t index, AliTrackPoint& p) const
{
  // Get track space point with index i
  // Coordinates are in the global system
  AliTOFcluster *cl = fClusters[index];
  Float_t xyz[3];
  xyz[0] = cl->GetR()*TMath::Cos(cl->GetPhi());
  xyz[1] = cl->GetR()*TMath::Sin(cl->GetPhi());
  xyz[2] = cl->GetZ();
  Float_t phiangle = (Int_t(cl->GetPhi()*TMath::RadToDeg()/20.)+0.5)*20.*TMath::DegToRad();
  Float_t sinphi = TMath::Sin(phiangle), cosphi = TMath::Cos(phiangle);
  Float_t tiltangle = fGeom->GetAngles(cl->GetDetInd(1),cl->GetDetInd(2))*TMath::DegToRad();
  Float_t sinth = TMath::Sin(tiltangle), costh = TMath::Cos(tiltangle);
  Float_t sigmay2 = fGeom->XPad()*fGeom->XPad()/12.;
  Float_t sigmaz2 = fGeom->ZPad()*fGeom->ZPad()/12.;
  Float_t cov[6];
  cov[0] = sinphi*sinphi*sigmay2 + cosphi*cosphi*sinth*sinth*sigmaz2;
  cov[1] = -sinphi*cosphi*sigmay2 + sinphi*cosphi*sinth*sinth*sigmaz2;
  cov[2] = -cosphi*sinth*costh*sigmaz2;
  cov[3] = cosphi*cosphi*sigmay2 + sinphi*sinphi*sinth*sinth*sigmaz2;
  cov[4] = -sinphi*sinth*costh*sigmaz2;
  cov[5] = costh*costh*sigmaz2;
  p.SetXYZ(xyz[0],xyz[1],xyz[2],cov);

  // Detector numbering scheme
  Int_t nSector = fGeom->NSectors();
  Int_t nPlate  = fGeom->NPlates();
  Int_t nStripA = fGeom->NStripA();
  Int_t nStripB = fGeom->NStripB();
  Int_t nStripC = fGeom->NStripC();

  Int_t isector = cl->GetDetInd(0);
  if (isector >= nSector)
    AliError(Form("Wrong sector number in TOF (%d) !",isector));
  Int_t iplate = cl->GetDetInd(1);
  if (iplate >= nPlate)
    AliError(Form("Wrong plate number in TOF (%d) !",iplate));
  Int_t istrip = cl->GetDetInd(2);

  Int_t stripOffset = 0;
  switch (iplate) {
  case 0:
    stripOffset = 0;
    break;
  case 1:
    stripOffset = nStripC;
    break;
  case 2:
    stripOffset = nStripC+nStripB;
    break;
  case 3:
    stripOffset = nStripC+nStripB+nStripA;
    break;
  case 4:
    stripOffset = nStripC+nStripB+nStripA+nStripB;
    break;
  default:
    AliError(Form("Wrong plate number in TOF (%d) !",iplate));
    break;
  };

  Int_t idet = (2*(nStripC+nStripB)+nStripA)*isector +
               stripOffset +
               istrip;
  UShort_t volid = AliAlignObj::LayerToVolUID(AliAlignObj::kTOF,idet);
  p.SetVolumeID((UShort_t)volid);
  return kTRUE;
}
//_________________________________________________________________________
void AliTOFtracker::InitCheckHists() {

  //Init histos for Digits/Reco QA and Calibration


  fCalTree = new TTree("CalTree", "Tree for TOF calibration");
  fCalTree->Branch("TOFchannelindex",&fIch,"iTOFch/I");
  fCalTree->Branch("ToT",&fToT,"TOFToT/F");
  fCalTree->Branch("TOFtime",&fTime,"TOFtime/F");
  fCalTree->Branch("PionExpTime",&fExpTimePi,"PiExpTime/F");
  fCalTree->Branch("KaonExpTime",&fExpTimeKa,"KaExpTime/F");
  fCalTree->Branch("ProtonExpTime",&fExpTimePr,"PrExpTime/F");

  //Digits "QA" 
  fHDigClusMap = new TH2F("TOFDig_ClusMap", "",182,0.5,182.5,864, 0.5,864.5);  
  fHDigNClus = new TH1F("TOFDig_NClus", "",200,0.5,200.5);  
  fHDigClusTime = new TH1F("TOFDig_ClusTime", "",2000,0.,200.);  
  fHDigClusToT = new TH1F("TOFDig_ClusToT", "",500,0.,100);  

  //Reco "QA"
  fHRecNClus =new TH1F("TOFRec_NClusW", "",50,0.5,50.5);
  fHRecDist=new TH1F("TOFRec_Dist", "",50,0.5,10.5);
  fHRecSigYVsP=new TH2F("TOFDig_SigYVsP", "",40,0.,4.,100, 0.,5.);
  fHRecSigZVsP=new TH2F("TOFDig_SigZVsP", "",40,0.,4.,100, 0.,5.);
  fHRecSigYVsPWin=new TH2F("TOFDig_SigYVsPWin", "",40,0.,4.,100, 0.,50.);
  fHRecSigZVsPWin=new TH2F("TOFDig_SigZVsPWin", "",40,0.,4.,100, 0.,50.);
}

//_________________________________________________________________________
void AliTOFtracker::SaveCheckHists() {

  //write histos for Digits/Reco QA and Calibration

  TDirectory *dir = gDirectory;
  TFile *logFile = 0;
  TFile *logFileTOF = 0;

  TSeqCollection *list = gROOT->GetListOfFiles();
  int n = list->GetEntries();
  for(int i=0; i<n; i++) {
    logFile = (TFile*)list->At(i);
    if (strstr(logFile->GetName(), "AliESDs.root")) break;
  }

  Bool_t isThere=kFALSE;
  for(int i=0; i<n; i++) {
    logFileTOF = (TFile*)list->At(i);
    if (strstr(logFileTOF->GetName(), "TOFQA.root")){
      isThere=kTRUE;
      break;
    } 
  }
   
  logFile->cd();
  fHDigClusMap->Write(fHDigClusMap->GetName(), TObject::kOverwrite);
  fHDigNClus->Write(fHDigNClus->GetName(), TObject::kOverwrite);
  fHDigClusTime->Write(fHDigClusTime->GetName(), TObject::kOverwrite);
  fHDigClusToT->Write(fHDigClusToT->GetName(), TObject::kOverwrite);
  fHRecNClus->Write(fHRecNClus->GetName(), TObject::kOverwrite);
  fHRecDist->Write(fHRecDist->GetName(), TObject::kOverwrite);
  fHRecSigYVsP->Write(fHRecSigYVsP->GetName(), TObject::kOverwrite);
  fHRecSigZVsP->Write(fHRecSigZVsP->GetName(), TObject::kOverwrite);
  fHRecSigYVsPWin->Write(fHRecSigYVsPWin->GetName(), TObject::kOverwrite);
  fHRecSigZVsPWin->Write(fHRecSigZVsPWin->GetName(), TObject::kOverwrite);
  fCalTree->Write(fCalTree->GetName(),TObject::kOverwrite);
  logFile->Flush();  

  if(!isThere)logFileTOF = new TFile( "TOFQA.root","RECREATE");
  logFileTOF->cd(); 
  fHDigClusMap->Write(fHDigClusMap->GetName(), TObject::kOverwrite);
  fHDigNClus->Write(fHDigNClus->GetName(), TObject::kOverwrite);
  fHDigClusTime->Write(fHDigClusTime->GetName(), TObject::kOverwrite);
  fHDigClusToT->Write(fHDigClusToT->GetName(), TObject::kOverwrite);
  fHRecNClus->Write(fHRecNClus->GetName(), TObject::kOverwrite);
  fHRecDist->Write(fHRecDist->GetName(), TObject::kOverwrite);
  fHRecSigYVsP->Write(fHRecSigYVsP->GetName(), TObject::kOverwrite);
  fHRecSigZVsP->Write(fHRecSigZVsP->GetName(), TObject::kOverwrite);
  fHRecSigYVsPWin->Write(fHRecSigYVsPWin->GetName(), TObject::kOverwrite);
  fHRecSigZVsPWin->Write(fHRecSigZVsPWin->GetName(), TObject::kOverwrite);
  fCalTree->Write(fCalTree->GetName(),TObject::kOverwrite);
  logFileTOF->Flush();  

  dir->cd();
  }
//_________________________________________________________________________
Float_t AliTOFtracker::CorrectTimeWalk( Float_t dist, Float_t tof) {

  //dummy, for the moment
  Float_t tofcorr=0.;
  if(dist<fGeom->ZPad()*0.5){
    tofcorr=tof;
    //place here the actual correction
  }else{
    tofcorr=tof; 
  } 
  return tofcorr;
}
//_________________________________________________________________________
Float_t AliTOFtracker::GetTimeZerofromT0(AliESD *event) {

  //Returns TimeZero as measured by T0 detector

  return event->GetT0();
}
//_________________________________________________________________________
Float_t AliTOFtracker::GetTimeZerofromTOF(AliESD *event) {

  //dummy, for the moment. T0 algorithm using tracks on TOF
  {
    //place T0 algo here...
  }
  return 0.;
}

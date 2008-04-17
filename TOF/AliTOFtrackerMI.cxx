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
//   AliTOFtracker Class                                           //
//   Task: Perform association of the ESD tracks to TOF Clusters   //
//   and Update ESD track with associated TOF Cluster parameters   //
//                                                                 //
//-----------------------------------------------------------------//

#include <Rtypes.h>

#include "TClonesArray.h"
//#include "TGeoManager.h"
#include "TTree.h"
#include "TTreeStream.h"

//#include "AliRun.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"

#include "AliTOFRecoParam.h"
#include "AliTOFcalib.h"
#include "AliTOFcluster.h"
#include "AliTOFGeometry.h"
#include "AliTOFtrackerMI.h"
#include "AliTOFtrack.h"
#include "AliTOFpidESD.h"

class TGeoManager;

extern TGeoManager *gGeoManager;

ClassImp(AliTOFtrackerMI)

//_____________________________________________________________________________
AliTOFtrackerMI::AliTOFtrackerMI():
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
  fR(379.), 
  fTOFHeigth(15.3),  
  fdCut(3.), 
  fDx(1.5), 
  fDy(0), 
  fDz(0), 
  fTracks(0x0),
  fSeeds(0x0),
  fDebugStreamer(0x0)
 { 
  //AliTOFtrackerMI main Ctor

   fRecoParam=new AliTOFRecoParam();
   fGeom=new AliTOFGeometry();
   Double_t parPID[2];   
   parPID[0]=fRecoParam->GetTimeResolution();
   parPID[1]=fRecoParam->GetTimeNSigma();
   fPid=new AliTOFpidESD(parPID);
   fDy=AliTOFGeometry::XPad(); 
   fDz=AliTOFGeometry::ZPad(); 
   fDebugStreamer = new TTreeSRedirector("TOFdebug.root");   
}
//_____________________________________________________________________________
AliTOFtrackerMI::AliTOFtrackerMI(const AliTOFtrackerMI &t):
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
  fR(379.), 
  fTOFHeigth(15.3),  
  fdCut(3.), 
  fDx(1.5), 
  fDy(0), 
  fDz(0), 
  fTracks(0x0),
  fSeeds(0x0),
  fDebugStreamer(0x0)
 { 
  //AliTOFtrackerMI copy Ctor

  fNseeds=t.fNseeds;
  fNseedsTOF=t.fNseedsTOF;
  fngoodmatch=t.fngoodmatch;
  fnbadmatch=t.fnbadmatch;
  fnunmatch=t.fnunmatch;
  fnmatch=t.fnmatch;
  fRecoParam = t.fRecoParam;
  fGeom=t.fGeom;
  fPid = t.fPid;
  fR=t.fR; 
  fTOFHeigth=t.fTOFHeigth;  
  fdCut=t.fdCut; 
  fDy=t.fDy; 
  fDz=t.fDz; 
  fDx=1.5; 
  fSeeds=t.fSeeds;
  fTracks=t.fTracks;
  fN=t.fN;
}

//_________________________________________________________________________________
AliTOFtrackerMI& AliTOFtrackerMI::operator=(const AliTOFtrackerMI &t)
{ 
  //AliTOFtrackerMI assignment operator

  this->fNseeds=t.fNseeds;
  this->fNseedsTOF=t.fNseedsTOF;
  this->fngoodmatch=t.fngoodmatch;
  this->fnbadmatch=t.fnbadmatch;
  this->fnunmatch=t.fnunmatch;
  this->fnmatch=t.fnmatch;
  this->fRecoParam = t.fRecoParam;
  this->fGeom = t.fGeom;
  this->fPid = t.fPid;
  this->fR=t.fR; 
  this->fTOFHeigth=t.fTOFHeigth;  
  this->fdCut=t.fdCut; 
  this->fDy=t.fDy;
  this->fDz=t.fDz;
  this->fDx=t.fDx;
  this->fSeeds=t.fSeeds;
  this->fTracks=t.fTracks;
  this->fN=t.fN;
  return *this;

}

//_____________________________________________________________________________
AliTOFtrackerMI::~AliTOFtrackerMI(){
  //
  //
  //
  if (fDebugStreamer) {    
    //fDebugStreamer->Close();
    delete fDebugStreamer;
  }
  delete fRecoParam;
  delete fGeom;
  delete fPid;
}

//_____________________________________________________________________________
Int_t AliTOFtrackerMI::PropagateBack(AliESDEvent* event) {
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
  fSeeds= new TClonesArray("AliESDtrack");
  TClonesArray &aESDTrack = *fSeeds;


  //Load ESD tracks into a local Array of ESD Seeds

  for (Int_t i=0; i<fNseeds; i++) {
    AliESDtrack *t=event->GetTrack(i);
    new(aESDTrack[i]) AliESDtrack(*t);
  }

  //Prepare ESD tracks candidates for TOF Matching
  CollectESD();

  //First Step with Strict Matching Criterion
  //MatchTracks(kFALSE);

  //Second Step with Looser Matching Criterion
  //MatchTracks(kTRUE);
  MatchTracksMI(kFALSE);  // assign track to clusters
  MatchTracksMI(kTRUE);   // assign clusters to esd
  
  Info("PropagateBack","Number of matched tracks: %d",fnmatch);
  Info("PropagateBack","Number of good matched tracks: %d",fngoodmatch);
  Info("PropagateBack","Number of bad  matched tracks: %d",fnbadmatch);

  //Update the matched ESD tracks

  for (Int_t i=0; i<ntrk; i++) {
    AliESDtrack *t=event->GetTrack(i);
    AliESDtrack *seed =(AliESDtrack*)fSeeds->UncheckedAt(i);
    if(seed->GetTOFsignal()>0){
      t->SetTOFsignal(seed->GetTOFsignal());
      t->SetTOFcluster(seed->GetTOFcluster());
      Int_t tlab[3];
      seed->GetTOFLabel(tlab);    
      t->SetTOFLabel(tlab);
      AliTOFtrack *track = new AliTOFtrack(*seed);
      Float_t info[10];
      Double_t times[10];
      seed->GetTOFInfo(info);
      seed->GetIntegratedTimes(times);
      t->UpdateTrackParams(track,AliESDtrack::kTOFout);    
      t->SetIntegratedLength(seed->GetIntegratedLength());
      t->SetIntegratedTimes(times);
      t->SetTOFsignalToT(seed->GetTOFsignalToT());
      t->SetTOFCalChannel(seed->GetTOFCalChannel());
      //
      t->SetTOFInfo(info);
      delete track;
    }
  }


  //Make TOF PID
  fPid->MakePID(event);

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
void AliTOFtrackerMI::CollectESD() {
   //prepare the set of ESD tracks to be matched to clusters in TOF
 
  fTracks= new TClonesArray("AliTOFtrack");
  TClonesArray &aTOFTrack = *fTracks;
  Int_t c0=0;
  Int_t c1=0;
  for (Int_t i=0; i<fNseeds; i++) {

    AliESDtrack *t =(AliESDtrack*)fSeeds->UncheckedAt(i);
    if ((t->GetStatus()&AliESDtrack::kTPCout)==0)continue;

    // TRD good tracks, already propagated at 372 cm

    AliTOFtrack *track = new AliTOFtrack(*t); // New
    Double_t x = track->GetX(); //New

    if (((t->GetStatus()&AliESDtrack::kTRDout)!=0 ) && 
	 ( x >= AliTOFGeometry::RinTOF()) ){
      track->SetSeedIndex(i);
      t->UpdateTrackParams(track,AliESDtrack::kTOFout);    
      new(aTOFTrack[fNseedsTOF]) AliTOFtrack(*track);
      fNseedsTOF++;
      c0++;
      delete track;
    }

    // Propagate the rest of TPCbp  

    else {
      if(track->PropagateToInnerTOF()){ // temporary solution
      	track->SetSeedIndex(i);
	t->UpdateTrackParams(track,AliESDtrack::kTOFout);    
 	new(aTOFTrack[fNseedsTOF]) AliTOFtrack(*track);
	fNseedsTOF++;
	c1++;
      }
      delete track;
    }
  }
  //
  //
  printf("TRD\tOn\t%d\tOff\t%d\n",c0,c1);

  // Sort according uncertainties on track position 
  fTracks->Sort();

}







//
//
//_________________________________________________________________________
void AliTOFtrackerMI::MatchTracks( Bool_t /*mLastStep*/){
  return;
}
//
//
//_________________________________________________________________________
void AliTOFtrackerMI::MatchTracksMI(Bool_t mLastStep){

  //Match ESD tracks to clusters in TOF
  const Float_t kTofOffset = 26;  // time offset
  const Float_t kMinQuality        = -6.; // minimal quality
  const Float_t kMaxQualityD       = 1.;  // max delta quality if cluster used
  const Float_t kForbiddenR        = 0.1;  // minimal PID according TPC

  static const Double_t kMasses[]={
    0.000511, 0.105658, 0.139570, 0.493677, 0.938272, 1.875613
  };
  
  Int_t nSteps=(Int_t)(fTOFHeigth/0.1);

  AliTOFcalib *calib = new AliTOFcalib();

  //PH Arrays (moved outside of the loop)
  Float_t * trackPos[4];
  for (Int_t ii=0; ii<4; ii++) trackPos[ii] = new Float_t[nSteps];
  Int_t * clind[6];
  for (Int_t ii=0;ii<6;ii++) clind[ii] = new Int_t[fN];

  // Some init 
  
  Int_t         index[1000];
  Float_t       dist3D[1000][6];
  Double_t      times[1000][6];
  Float_t       mintimedist[1000];
  Float_t       likelihood[1000];
  Float_t       length[1000];
  AliTOFcluster *clusters[1000];
  Double_t       tpcpid[5];
  dist3D[0][0]=1;
  
  for (Int_t i=0; i<fNseedsTOF; i++) {

    AliTOFtrack *track =(AliTOFtrack*)fTracks->UncheckedAt(i);
    AliESDtrack *t =(AliESDtrack*)fSeeds->UncheckedAt(track->GetSeedIndex());
    Bool_t hasTime = ( (t->GetStatus()& AliESDtrack::kTIME)>0) ? kTRUE:kFALSE;   // did we integrate time
    Float_t trdquality = t->GetTRDQuality();
    //
    // Normalize tpc pid
    //
    t->GetTPCpid(tpcpid);
    Double_t sumpid=0;
    for (Int_t ipid=0;ipid<5;ipid++){
      sumpid+=tpcpid[ipid];
    }
    for (Int_t ipid=0;ipid<5;ipid++){
      if (sumpid>0) tpcpid[ipid]/=sumpid;
      else{
	tpcpid[ipid]=0.2;
      }
    }

    if (trdquality<0) continue; // no chance 
    //
    AliTOFtrack *trackTOFin =new AliTOFtrack(*track);     
    //
    //propagat track to the middle of TOF
    //
    Float_t xs = 379.2;  // should be defined in the TOF geometry
    Double_t ymax=xs*TMath::Tan(0.5*AliTOFGeometry::GetAlpha());  
    Bool_t skip=kFALSE;
    Double_t ysect=trackTOFin->GetYat(xs,skip);
    if (skip){
      xs = 373.;
      ymax=xs*TMath::Tan(0.5*AliTOFGeometry::GetAlpha());
      ysect=trackTOFin->GetYat(xs,skip);
    }
    if (ysect > ymax) {
      if (!trackTOFin->Rotate(AliTOFGeometry::GetAlpha())) {
	continue;
      }
    } else if (ysect <-ymax) {
      if (!trackTOFin->Rotate(-AliTOFGeometry::GetAlpha())) {
	continue;
      }
    }    
    if(!trackTOFin->PropagateTo(xs)) {
      continue;
    }
    //
    // Determine a window around the track
    //
    Double_t x,par[5]; 
    trackTOFin->GetExternalParameters(x,par);
    Double_t cov[15]; 
    trackTOFin->GetExternalCovariance(cov);
    Float_t scalefact=3.;    
    Double_t dphi=
      scalefact*
      ((5*TMath::Sqrt(cov[0]) + 3.*fDy +10.*TMath::Abs(par[2]))/fR); 
    Double_t dz=
      scalefact*
      (5*TMath::Sqrt(cov[2]) + 3.*fDz  +10.*TMath::Abs(par[3]));
    
    Double_t phi=TMath::ATan2(par[0],x) + trackTOFin->GetAlpha();
    if (phi<-TMath::Pi())phi+=2*TMath::Pi();
    if (phi>=TMath::Pi())phi-=2*TMath::Pi();
    Double_t z=par[1];   

    Int_t nc     =0;
    Int_t nfound =0;
    // find the clusters in the window of the track

    for (Int_t k=FindClusterIndex(z-dz); k<fN; k++) {
      AliTOFcluster *c=fClusters[k];
      if (c->GetZ() > z+dz) break;
      //      if (c->IsUsed()) continue;
      
      Double_t dph=TMath::Abs(c->GetPhi()-phi);
      if (dph>TMath::Pi()) dph-=2.*TMath::Pi();
      if (TMath::Abs(dph)>dphi) continue;
    
      clind[0][nc] = c->GetDetInd(0);
      clind[1][nc] = c->GetDetInd(1);
      clind[2][nc] = c->GetDetInd(2);
      clind[3][nc] = c->GetDetInd(3);
      clind[4][nc] = c->GetDetInd(4);
      clind[5][nc] = k;      
      nc++;
    }

    //
    // select close clusters
    //
    Double_t mom=t->GetP();
    //    Bool_t dump = kTRUE;
    for (Int_t icl=0; icl<nc; icl++){
      Float_t distances[5];
      if (nfound>=1000) break;
      index[nfound]=clind[5][icl];
      AliTOFcluster *cluster = fClusters[clind[5][icl]];
      GetLinearDistances(trackTOFin,cluster, distances);
      dist3D[nfound][0] = distances[4];
      dist3D[nfound][1] = distances[1];
      dist3D[nfound][2] = distances[2];
      // cut on distance
      if (TMath::Abs(dist3D[nfound][1])>20 || TMath::Abs(dist3D[nfound][2])>20) continue;
      //
      GetLikelihood(distances[1],distances[2],cov,trackTOFin, dist3D[nfound][3], dist3D[nfound][4]);   
      //
      // cut on likelihood      
      if (dist3D[nfound][3]*dist3D[nfound][4]<0.00000000000001) continue;  // log likelihood
      if (TMath::Log(dist3D[nfound][3]*dist3D[nfound][4])<-9) continue;  // log likelihood
      //
      clusters[nfound] = cluster;
      //
      //length  and TOF updates 
      trackTOFin->GetIntegratedTimes(times[nfound]);
      length[nfound] = trackTOFin->GetIntegratedLength();
      length[nfound]+=distances[4];
      mintimedist[nfound]=1000; 
      Double_t tof2=AliTOFGeometry::TdcBinWidth()*cluster->GetTDC()+kTofOffset; // in ps
      // Float_t tgamma = TMath::Sqrt(cluster->GetR()*cluster->GetR()+cluster->GetZ()*cluster->GetZ())/0.03;  //time for "primary" gamma
      //if (trackTOFin->GetPt()<0.7 && TMath::Abs(tgamma-tof2)<350) continue;  // gamma conversion candidate - TEMPORARY
      for(Int_t j=0;j<=5;j++){
	Double_t mass=kMasses[j];
	times[nfound][j]+=distances[4]/3e-2*TMath::Sqrt(mom*mom+mass*mass)/mom;   // add time distance
	if ( TMath::Abs(times[nfound][j]-tof2)<mintimedist[nfound] && tpcpid[j]>kForbiddenR){
	  mintimedist[nfound]=TMath::Abs(times[nfound][j]-tof2);
	}
      }
      //
      Float_t   liketime =  TMath::Exp(-mintimedist[nfound]/90.)+0.25*TMath::Exp(-mintimedist[nfound]/180.);
      if (!hasTime)  liketime=0.2;
      likelihood[nfound] = TMath::Log(dist3D[nfound][3]*dist3D[nfound][4]*liketime);
      
      if (TMath::Log(dist3D[nfound][3]*dist3D[nfound][4])<-1){
	if (likelihood[nfound]<-9.) continue;
      }
      //
      nfound++;
    }   
    if (nfound == 0 ) {
      fnunmatch++;
      delete trackTOFin;
      continue;
    } 
    //
    //choose the best cluster
    //
    Float_t quality[1000];
    Int_t   index[1000];
    //
    AliTOFcluster * cgold=0;
    Int_t igold =-1;
    for (Int_t icl=0;icl<nfound;icl++){
      quality[icl] = dist3D[icl][3]*dist3D[icl][4];
    }
    TMath::Sort(nfound,quality,index,kTRUE);
    igold =  index[0];
    cgold =  clusters[igold];
    if (nfound>1 &&likelihood[index[1]]>likelihood[index[0]]){
      if ( quality[index[0]]<quality[index[1]]+0.5){
	igold = index[1];
	cgold =  clusters[igold];
      }
    }
    //
    //
    Float_t qualityGold = TMath::Log(0.0000001+(quality[igold]*(0.1+TMath::Exp(-mintimedist[igold]/80.))*(0.2+trdquality)));
    if (!mLastStep){
      if (cgold->GetQuality()<qualityGold) cgold->SetQuality(qualityGold);
      continue;
    }
    //
    if (mLastStep){
      //signed better cluster
      if (cgold->GetQuality()>qualityGold+kMaxQualityD) continue;
      if (2.*qualityGold-cgold->GetQuality()<kMinQuality) continue;
    }
 
    Int_t inonfake=-1;
    
    for (Int_t icl=0;icl<nfound;icl++){
      if (
	  (clusters[index[icl]]->GetLabel(0)==TMath::Abs(trackTOFin->GetLabel()))
	  ||
	  (clusters[index[icl]]->GetLabel(1)==TMath::Abs(trackTOFin->GetLabel()))
	  ||
	  (clusters[index[icl]]->GetLabel(2)==TMath::Abs(trackTOFin->GetLabel()))
	  ) {
	inonfake = icl;
	break;
      }
    }    
    fnmatch++;;
    if (inonfake==0) fngoodmatch++;
    else{
      fnbadmatch++;
    }

    Int_t tlab[3];
    tlab[0]=cgold->GetLabel(0);
    tlab[1]=cgold->GetLabel(1);
    tlab[2]=cgold->GetLabel(2);
    //    Double_t tof2=25.*cgold->GetTDC()-350; // in ps
    Double_t tof2=AliTOFGeometry::TdcBinWidth()*cgold->GetTDC()+kTofOffset; // in ps
    Float_t tgamma = TMath::Sqrt(cgold->GetR()*cgold->GetR()+cgold->GetZ()*cgold->GetZ())/0.03;
    Float_t info[11]={dist3D[igold][0],dist3D[igold][1],dist3D[igold][2],dist3D[igold][3],dist3D[igold][4],mintimedist[igold],
		      -1,tgamma, qualityGold,cgold->GetQuality(),0};
    //    GetLinearDistances(trackTOFin,cgold,&info[6]);
    if (inonfake>=0){
      info[6] = inonfake;
      //      info[7] = mintimedist[index[inonfake]];
    }
    //
    //  Store quantities to be used for TOF Calibration
    Float_t tToT=cgold->GetToT(); // in ps
    t->SetTOFsignalToT(tToT);
    Int_t ind[5];
    ind[0]=cgold->GetDetInd(0);
    ind[1]=cgold->GetDetInd(1);
    ind[2]=cgold->GetDetInd(2);
    ind[3]=cgold->GetDetInd(3);
    ind[4]=cgold->GetDetInd(4);
    Int_t calindex = AliTOFGeometry::GetIndex(ind);
    t->SetTOFCalChannel(calindex);

    t->SetTOFInfo(info);
    t->SetTOFsignal(tof2);
    t->SetTOFcluster(cgold->GetIndex());  
    AliTOFtrack *trackTOFout = new AliTOFtrack(*t); 
    trackTOFout->PropagateTo(379.);
    t->UpdateTrackParams(trackTOFout,AliESDtrack::kTOFout);    
    t->SetIntegratedLength(length[igold]);
    t->SetIntegratedTimes(times[igold]);
    t->SetTOFLabel(tlab);
    //
    delete trackTOFin;
    delete trackTOFout;
    //
  }
  //
  //
  //
  for (Int_t ii=0; ii<4; ii++) delete [] trackPos[ii];
  for (Int_t ii=0;ii<6;ii++) delete [] clind[ii];
  delete calib;
}




// //_________________________________________________________________________
// Int_t AliTOFtrackerMI::LoadClusters(TTree *dTree) {
//   //--------------------------------------------------------------------
//   //This function loads the TOF clusters
//   //--------------------------------------------------------------------

//   TBranch *branch=dTree->GetBranch("TOF");
//   if (!branch) { 
//     AliError(" can't get the branch with the TOF digits !");
//     return 1;
//   }

//   TClonesArray dummy("AliTOFdigit",10000), *digits=&dummy;
//   branch->SetAddress(&digits);

//   dTree->GetEvent(0);
//   Int_t nd=digits->GetEntriesFast();
//   Info("LoadClusters","number of digits: %d",nd);

//   for (Int_t i=0; i<nd; i++) {
//     AliTOFdigit *d=(AliTOFdigit*)digits->UncheckedAt(i);
//     Int_t dig[5]; Float_t g[3];
//     dig[0]=d->GetSector();
//     dig[1]=d->GetPlate();
//     dig[2]=d->GetStrip();
//     dig[3]=d->GetPadz();
//     dig[4]=d->GetPadx();

//     fGeom->GetPos(dig,g);

//     Double_t h[5];
//     h[0]=TMath::Sqrt(g[0]*g[0]+g[1]*g[1]);
//     h[1]=TMath::ATan2(g[1],g[0]); h[2]=g[2]; 
//     h[3]=d->GetTdc(); h[4]=d->GetAdc();

//     AliTOFcluster *cl=new AliTOFcluster(h,d->GetTracks(),dig,i);
//     InsertCluster(cl);
//   }  

//   return 0;
// }
// //_________________________________________________________________________
// void AliTOFtrackerMI::UnloadClusters() {
//   //--------------------------------------------------------------------
//   //This function unloads TOF clusters
//   //--------------------------------------------------------------------
//   for (Int_t i=0; i<fN; i++) delete fClusters[i];
//   fN=0;
// }



//_________________________________________________________________________
Int_t AliTOFtrackerMI::LoadClusters(TTree *cTree) {
  //--------------------------------------------------------------------
  //This function loads the TOF clusters
  //--------------------------------------------------------------------

  TBranch *branch=cTree->GetBranch("TOF");
  if (!branch) { 
    AliError("can't get the branch with the TOF clusters !");
    return 1;
  }

  TClonesArray dummy("AliTOFcluster",10000), *clusters=&dummy;
  branch->SetAddress(&clusters);

  cTree->GetEvent(0);
  Int_t nc=clusters->GetEntriesFast();
  AliInfo(Form("Number of clusters: %d",nc));

  for (Int_t i=0; i<nc; i++) {
    AliTOFcluster *c=(AliTOFcluster*)clusters->UncheckedAt(i);
    /*
    for (Int_t jj=0; jj<5; jj++) dig[jj]=c->GetDetInd(jj);

    Double_t h[5];
    h[0]=c->GetR();
    h[1]=c->GetPhi();
    h[2]=c->GetZ();
    h[3]=c->GetTDC();
    h[4]=c->GetADC();

    Int_t indexDig[3];
    for (Int_t jj=0; jj<3; jj++) indexDig[jj] = c->GetLabel(jj);

    AliTOFcluster *cl=new AliTOFcluster(h,c->GetTracks(),dig,i);
    */

    //    fClusters[i]=c; fN++;
    fClusters[i]=new AliTOFcluster(*c); fN++;

    //AliInfo(Form("%4i %4i  %f %f %f  %f %f   %2i %1i %2i %1i %2i",i, fClusters[i]->GetIndex(),fClusters[i]->GetZ(),fClusters[i]->GetR(),fClusters[i]->GetPhi(), fClusters[i]->GetTDC(),fClusters[i]->GetADC(),fClusters[i]->GetDetInd(0),fClusters[i]->GetDetInd(1),fClusters[i]->GetDetInd(2),fClusters[i]->GetDetInd(3),fClusters[i]->GetDetInd(4)));
    //AliInfo(Form("%i %f",i, fClusters[i]->GetZ()));
  }

  //AliInfo(Form("Number of clusters: %d",fN));

  return 0;
}
//_________________________________________________________________________
void AliTOFtrackerMI::UnloadClusters() {
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
Int_t AliTOFtrackerMI::InsertCluster(AliTOFcluster *c) {
  //--------------------------------------------------------------------
  //This function adds a cluster to the array of clusters sorted in Z
  //--------------------------------------------------------------------
  if (fN==kMaxCluster) {
    AliError("Too many clusters !");
    return 1;
  }

  if (fN==0) {fClusters[fN++]=c; return 0;}
  Int_t i=FindClusterIndex(c->GetZ());
  memmove(fClusters+i+1 ,fClusters+i,(fN-i)*sizeof(AliTOFcluster*));
  fClusters[i]=c; fN++;

  return 0;
}

//_________________________________________________________________________
Int_t AliTOFtrackerMI::FindClusterIndex(Double_t z) const {
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



Float_t AliTOFtrackerMI::GetLinearDistances(AliTOFtrack * track, AliTOFcluster *cluster, Float_t distances[5])
{
  //
  // calclates distance between cluster and track
  // use linear aproximation
  //
  const Float_t kRaddeg = 180/3.14159265358979312;
  //
  //  Float_t tiltangle  = fGeom->GetAngles(cluster->fdetIndex[1],cluster->fdetIndex[2])/kRaddeg;  //tiltangle  
  Int_t cind[5];
  cind[0]= cluster->GetDetInd(0);
  cind[1]= cluster->GetDetInd(1);
  cind[2]= cluster->GetDetInd(2);
  cind[3]= cluster->GetDetInd(3);
  cind[4]= cluster->GetDetInd(4);
  Float_t tiltangle  = AliTOFGeometry::GetAngles(cluster->GetDetInd(1),cluster->GetDetInd(2))/kRaddeg;  //tiltangle  

  Float_t cpos[3];  //cluster position
  Float_t cpos0[3];  //cluster position
  //  fGeom->GetPos(cluster->fdetIndex,cpos);  
  //fGeom->GetPos(cluster->fdetIndex,cpos0);  
  //
  fGeom->GetPos(cind,cpos);  
  fGeom->GetPos(cind,cpos0);  

  Float_t phi = TMath::ATan2(cpos[1],cpos[0]);	
  if(phi<0) phi=2.*TMath::Pi()+phi;
  //  Get the local angle in the sector philoc
  Float_t phiangle   = (Int_t (phi*kRaddeg/20.) + 0.5)*20./kRaddeg;
  //
  Double_t v0[3];
  Double_t dir[3];
  track->GetXYZ(v0);
  track->GetPxPyPz(dir);
  dir[0]/=track->GetP();
  dir[1]/=track->GetP();
  dir[2]/=track->GetP();
  //
  //
  //rotate 0
  Float_t sinphi = TMath::Sin(phiangle);
  Float_t cosphi = TMath::Cos(phiangle);
  Float_t sinth  = TMath::Sin(tiltangle);
  Float_t costh  = TMath::Cos(tiltangle);
  //
  Float_t temp;
  temp    =  cpos[0]*cosphi+cpos[1]*sinphi;
  cpos[1] = -cpos[0]*sinphi+cpos[1]*cosphi;
  cpos[0] = temp;
  temp    =  v0[0]*cosphi+v0[1]*sinphi;
  v0[1] = -v0[0]*sinphi+v0[1]*cosphi;
  v0[0] = temp;
  //  
  temp    =  cpos[0]*costh+cpos[2]*sinth;
  cpos[2] = -cpos[0]*sinth+cpos[2]*costh;
  cpos[0] = temp;
  temp    =  v0[0]*costh+v0[2]*sinth;
  v0[2]   = -v0[0]*sinth+v0[2]*costh;
  v0[0]   = temp;
  //
  //
  //rotate direction vector
  //
  temp    =  dir[0]*cosphi+dir[1]*sinphi;
  dir[1] = -dir[0]*sinphi+dir[1]*cosphi;
  dir[0] = temp;
  //
  temp    =  dir[0]*costh+dir[2]*sinth;
  dir[2]  = -dir[0]*sinth+dir[2]*costh;
  dir[0]  = temp;
  //
  Float_t v3[3];
  Float_t k = (cpos[0]-v0[0])/dir[0];
  v3[0] = v0[0]+k*dir[0];
  v3[1] = v0[1]+k*dir[1];
  v3[2] = v0[2]+k*dir[2];
  //
  distances[0] = v3[0]-cpos[0];
  distances[1] = v3[1]-cpos[1];
  distances[2] = v3[2]-cpos[2];
  distances[3] = TMath::Sqrt( distances[0]*distances[0]+distances[1]*distances[1]+distances[2]*distances[2]); //distance
  distances[4] = k;  //length

  //
  // Debuging part of the matching
  //
  if (track->GetLabel()==cluster->GetLabel(0) ||track->GetLabel()==cluster->GetLabel(1) ){
    TTreeSRedirector& cstream = *fDebugStreamer;
    Float_t tdc = cluster->GetTDC();
    cstream<<"Tracks"<<
      "TOF.="<<track<<
      "Cx="<<cpos0[0]<<
      "Cy="<<cpos0[1]<<
      "Cz="<<cpos0[2]<<
      "Dist="<<k<<
      "Dist0="<<distances[0]<<
      "Dist1="<<distances[1]<<
      "Dist2="<<distances[2]<<
      "TDC="<<tdc<<
      "\n";
  }
  return distances[3];
}



void    AliTOFtrackerMI::GetLikelihood(Float_t dy, Float_t dz, const Double_t *cov, AliTOFtrack * /*track*/, Float_t & py, Float_t &pz)
{
  //
  //  get likelihood - track covariance taken
  //  75 % of gauss with expected sigma
  //  25 % of gauss with extended sigma
  
  Double_t kMaxSigmaY  = 0.6;  // ~ 90% of TRD tracks  
  Double_t kMaxSigmaZ  = 1.2;  // ~ 90% of TRD tracks 
  Double_t kMeanSigmaY = 0.25; // mean TRD sigma  
  Double_t kMeanSigmaZ = 0.5;  // mean TRD sigma 

  
  Float_t normwidth, normd, p0,p1;  
  Float_t sigmay = TMath::Max(TMath::Sqrt(cov[0]+kMeanSigmaY*kMeanSigmaY),kMaxSigmaY);
  Float_t sigmaz = TMath::Max(TMath::Sqrt(cov[2]+kMeanSigmaZ*kMeanSigmaZ),kMaxSigmaZ);

  py=0;
  pz=0;  
  // 
  // py calculation   - 75% admixture of original sigma - 25% tails 
  //
  normwidth = fDy/sigmay;
  normd     = dy/sigmay;
  p0 = 0.5*(1+TMath::Erf(normd-normwidth*0.5));
  p1 = 0.5*(1+TMath::Erf(normd+normwidth*0.5));  
  py+= 0.75*(p1-p0);
  //
  normwidth = fDy/(3.*sigmay);
  normd     = dy/(3.*sigmay);
  p0 = 0.5*(1+TMath::Erf(normd-normwidth*0.5));
  p1 = 0.5*(1+TMath::Erf(normd+normwidth*0.5));  
  py+= 0.25*(p1-p0);
  // 
  // pz calculation   - 75% admixture of original sigma - 25% tails 
  //
  normwidth = fDz/sigmaz;
  normd     = dz/sigmaz;
  p0 = 0.5*(1+TMath::Erf(normd-normwidth*0.5));
  p1 = 0.5*(1+TMath::Erf(normd+normwidth*0.5));  
  pz+= 0.75*(p1-p0);
  //
  normwidth = fDz/(3.*sigmaz);
  normd     = dz/(3.*sigmaz);
  p0 = 0.5*(1+TMath::Erf(normd-normwidth*0.5));
  p1 = 0.5*(1+TMath::Erf(normd+normwidth*0.5));  
  pz+= 0.25*(p1-p0);
}

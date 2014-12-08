//-------------------------------------------------------------------------
//               Implementation of the ITS tracker class
//    The pattern recongintion based on the "cooked covariance" approach
//-------------------------------------------------------------------------

#include <TTree.h>
#include <TClonesArray.h>

#include "AliLog.h"
#include "AliESDEvent.h"
#include "AliITSUClusterPix.h"
#include "AliITSUTrackerCooked.h"
#include "AliITSUTrackCooked.h" 
#include "AliITSUReconstructor.h" 

ClassImp(AliITSUTrackerCooked)

//************************************************
// Constants hardcoded for the moment:
//************************************************
// seed "windows" in z and phi: MakeSeeds
const Double_t kzWin=0.33;
const Double_t kminPt=0.05;
// Maximal accepted impact parameters for the seeds 
const Double_t kmaxDCAxy=3.;
const Double_t kmaxDCAz= 3.;
// Layers for the seeding
const Int_t kSeedingLayer1=6, kSeedingLayer2=4, kSeedingLayer3=5;
// Space point resolution
const Double_t kSigma2=0.0005*0.0005;
// Max accepted chi2
const Double_t kmaxChi2PerCluster=20.;
const Double_t kmaxChi2PerTrack=30.;
// Tracking "road" from layer to layer
const Double_t kRoadY=0.7;
const Double_t kRoadZ=0.7;
// Minimal number of attached clusters
const Int_t kminNumberOfClusters=4;

//************************************************
// TODO:
//************************************************
// Seeding:
// Precalculate cylidnrical (r,phi) for the clusters;
// use exact r's for the clusters


AliITSUTrackerCooked::AliITSUlayer
              AliITSUTrackerCooked::fgLayers[AliITSUTrackerCooked::kNLayers];

AliITSUTrackerCooked::AliITSUTrackerCooked(AliITSUReconstructor *rec): 
AliITSUTrackerGlo(rec),
fSeeds(0),
fI(kNLayers-1),
fBestTrack(0), 
fTrackToFollow(0),
fSAonly(kTRUE) 
{
  //--------------------------------------------------------------------
  // This default constructor needs to be provided
  //--------------------------------------------------------------------
  const Double_t 
  klRadius[7]={2.34, 3.15, 3.93, 19.61, 24.55, 34.39, 39.34}; //tdr6

  for (Int_t i=0; i<kNLayers; i++) fgLayers[i].SetR(klRadius[i]);

  // Some default primary vertex
  Double_t xyz[]={0.,0.,0.};
  Double_t ers[]={2.,2.,2.};

  SetVertex(xyz,ers);

}

void AliITSUTrackerCooked::ResetTrackToFollow(const AliITSUTrackCooked &t) {
  //--------------------------------------------------------------------
  // Prepare to follow a new track seed
  //--------------------------------------------------------------------
     delete fTrackToFollow;
     fTrackToFollow = new AliITSUTrackCooked(t);
}
  
void AliITSUTrackerCooked::ResetBestTrack() {
  //--------------------------------------------------------------------
  // Replace the best track branch
  //--------------------------------------------------------------------
     delete fBestTrack;
     fBestTrack = new AliITSUTrackCooked(*fTrackToFollow);
}
  
AliITSUTrackerCooked::~AliITSUTrackerCooked() 
{
  //--------------------------------------------------------------------
  // Virtual destructor
  //--------------------------------------------------------------------

  if (fSeeds) fSeeds->Delete(); delete fSeeds; 
  delete fBestTrack;
  delete fTrackToFollow;

}

static Double_t 
f1(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x3, Double_t y3)
{
    //-----------------------------------------------------------------
    // Initial approximation of the track curvature
    //-----------------------------------------------------------------
    Double_t d=(x2-x1)*(y3-y2)-(x3-x2)*(y2-y1);
    Double_t a=0.5*((y3-y2)*(y2*y2-y1*y1+x2*x2-x1*x1)-
                    (y2-y1)*(y3*y3-y2*y2+x3*x3-x2*x2));
    Double_t b=0.5*((x2-x1)*(y3*y3-y2*y2+x3*x3-x2*x2)-
                    (x3-x2)*(y2*y2-y1*y1+x2*x2-x1*x1));
    
    Double_t xr=TMath::Abs(d/(d*x1-a)), yr=TMath::Abs(d/(d*y1-b));
    
    Double_t crv=xr*yr/sqrt(xr*xr+yr*yr);
    if (d>0) crv=-crv;

    return crv;
}

static Double_t 
f2(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x3, Double_t y3)
{
    //-----------------------------------------------------------------
    // Initial approximation of the x-coordinate of the center of curvature 
    //-----------------------------------------------------------------

  Double_t k1=(y2-y1)/(x2-x1), k2=(y3-y2)/(x3-x2);
  Double_t x0=0.5*(k1*k2*(y1-y3) + k2*(x1+x2) - k1*(x2+x3))/(k2-k1);

  return x0;
}

static Double_t 
f3(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t z1, Double_t z2)
{
    //-----------------------------------------------------------------
    // Initial approximation of the tangent of the track dip angle
    //-----------------------------------------------------------------
    return (z1 - z2)/sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

Bool_t AliITSUTrackerCooked::
AddCookedSeed(const Float_t r1[3], Int_t l1, Int_t i1, 
              const Float_t r2[3], Int_t l2, Int_t i2,
              const AliCluster *c3,Int_t l3, Int_t i3) 
{
    //--------------------------------------------------------------------
    // This is the main cooking function.
    // Creates seed parameters out of provided clusters.
    //--------------------------------------------------------------------
    Float_t x,a;
    if (!c3->GetXAlphaRefPlane(x,a)) return kFALSE;

    Double_t ca=TMath::Cos(a), sa=TMath::Sin(a);
    Double_t x1 = r1[0]*ca + r1[1]*sa,
             y1 =-r1[0]*sa + r1[1]*ca, z1 = r1[2];
    Double_t x2 = r2[0]*ca + r2[1]*sa,
             y2 =-r2[0]*sa + r2[1]*ca, z2 = r2[2];
    Double_t x3 = x,  y3 = c3->GetY(), z3 = c3->GetZ();

    Double_t par[5];
    par[0]=y3;
    par[1]=z3;
    Double_t crv=f1(x1, y1, x2, y2, x3, y3); //curvature
    Double_t x0 =f2(x1, y1, x2, y2, x3, y3); //x-coordinate of the center
    Double_t tgl12=f3(x1, y1, x2, y2, z1, z2);
    Double_t tgl23=f3(x2, y2, x3, y3, z2, z3);

    Double_t sf=crv*(x-x0);
    if (TMath::Abs(sf) >= kAlmost1) return kFALSE;
    par[2]=sf;

    par[3]=0.5*(tgl12 + tgl23);
    Double_t bz=GetBz();
    par[4]=(TMath::Abs(bz) < kAlmost0Field) ? kAlmost0 : crv/(bz*kB2C);

    Double_t cov[15];
    /*
    for (Int_t i=0; i<15; i++) cov[i]=0.;
    cov[0] =kSigma2*10;
    cov[2] =kSigma2*10;
    cov[5] =0.007*0.007*10;   //FIXME all these lines
    cov[9] =0.007*0.007*10;
    cov[14]=0.1*0.1*10;
    */
    const Double_t dlt=0.0005;
    Double_t
    fy=1./(fgLayers[kSeedingLayer3].GetR() - fgLayers[kSeedingLayer2].GetR());
    Double_t tz=fy;
    Double_t cy=(f1(x1, y1, x2, y2+dlt, x3, y3) - crv)/dlt/bz/kB2C;
    cy*=20; //FIXME: MS contribution to the cov[14]
    Double_t s2=kSigma2;

    cov[0]=s2;
    cov[1]=0.;     cov[2]=s2;
    cov[3]=s2*fy;  cov[4]=0.;    cov[5]=s2*fy*fy;
    cov[6]=0.;     cov[7]=s2*tz; cov[8]=0.;        cov[9]=s2*tz*tz;
    cov[10]=s2*cy; cov[11]=0.;   cov[12]=s2*fy*cy; cov[13]=0.; cov[14]=s2*cy*cy;

    AliITSUTrackCooked *seed=new AliITSUTrackCooked();
    seed->Set(Double_t(x), Double_t(a), par, cov);

    Float_t dz[2]; 
    seed->GetDZ(GetX(),GetY(),GetZ(),GetBz(),dz);
    if (TMath::Abs(dz[0]) > kmaxDCAxy) {delete seed; return kFALSE;} 
    if (TMath::Abs(dz[1]) > kmaxDCAz ) {delete seed; return kFALSE;} 

    Double_t xx0 = 0.008; // Rough layer thickness
    Double_t radl= 9.36;  // Radiation length of Si [cm]
    Double_t rho = 2.33;  // Density of Si [g/cm^3] 
    Double_t mass= 0.139;// Pion
    if (!seed->CorrectForMeanMaterial(xx0, xx0*radl*rho, mass, kTRUE)) {
       delete seed; return kFALSE;
    }

    seed->SetClusterIndex(l1,i1);
    seed->SetClusterIndex(l2,i2);
    seed->SetClusterIndex(l3,i3);

    fSeeds->AddLast(seed);

    return kTRUE;
}

Int_t AliITSUTrackerCooked::MakeSeeds() {
  //--------------------------------------------------------------------
  // This is the main pattern recongition function.
  // Creates seeds out of two clusters and another point.
  //--------------------------------------------------------------------
   const Double_t zv=GetZ();

   AliITSUlayer &layer1=fgLayers[kSeedingLayer1];
   AliITSUlayer &layer2=fgLayers[kSeedingLayer2];
   AliITSUlayer &layer3=fgLayers[kSeedingLayer3];
   Double_t r1=layer1.GetR();
   Double_t r2=layer2.GetR();
   Double_t r3=layer3.GetR();

   const Double_t maxC  = TMath::Abs(GetBz()*kB2C/kminPt);
   const Double_t kpWin = TMath::ASin(0.5*maxC*r1) - TMath::ASin(0.5*maxC*r2);

   Int_t nClusters1=layer1.GetNumberOfClusters();
   Int_t nClusters2=layer2.GetNumberOfClusters();
   Int_t nClusters3=layer3.GetNumberOfClusters();
   for (Int_t n1=0; n1<nClusters1; n1++) {
     AliCluster *c1=layer1.GetCluster(n1);
     //
     //Int_t lab=c1->GetLabel(0);
     //
     Double_t z1=c1->GetZ();
     Float_t xyz1[3]; c1->GetGlobalXYZ(xyz1);
     Double_t phi1=layer1.GetClusterPhi(n1);
     Double_t zr2=zv + r2/r1*(z1-zv);
     Int_t start2=layer2.FindClusterIndex(zr2-kzWin);
     for (Int_t n2=start2; n2<nClusters2; n2++) {
         AliCluster *c2=layer2.GetCluster(n2);
         //
         //if (c2->GetLabel(0)!=lab) continue;
	 //
         Double_t z2=c2->GetZ();
         if (z2 > (zr2+kzWin)) break;  //check in Z

         Float_t xyz2[3]; c2->GetGlobalXYZ(xyz2);
         Double_t phi2=layer2.GetClusterPhi(n2);
         if (TMath::Abs(phi2-phi1) > kpWin) continue;  //check in Phi

         Double_t zr3=z1 + (r3-r1)/(r2-r1)*(z2-z1);
         Double_t crv=f1(xyz1[0], xyz1[1], xyz2[0], xyz2[1], GetX(), GetY());
         Double_t phir3 = phi1 + 0.5*crv*(r3 - r1); 

         Int_t start3=layer3.FindClusterIndex(zr3-kzWin/2);
         for (Int_t n3=start3; n3<nClusters3; n3++) {
             AliCluster *c3=layer3.GetCluster(n3);
             //
             //if (c3->GetLabel(0)!=lab) continue;
             //
             Double_t z3=c3->GetZ();
             if (z3 > (zr3+kzWin/2)) break;  //check in Z

             Float_t xyz3[3]; c3->GetGlobalXYZ(xyz3);
             Double_t phi3=layer3.GetClusterPhi(n3);
             if (TMath::Abs(phir3-phi3) > kpWin/100) continue;  //check in Phi

             AliITSUClusterPix cc(*((AliITSUClusterPix*)c2));
             cc.GoToFrameTrk();
             AddCookedSeed(xyz1, kSeedingLayer1, n1,
                           xyz3, kSeedingLayer3, n3, 
                           &cc,  kSeedingLayer2, n2);

	 }
     }
   }

   for (Int_t n1=0; n1<nClusters1; n1++) {
     AliCluster *c1=layer1.GetCluster(n1);
     ((AliITSUClusterPix*)c1)->GoToFrameTrk();
   }
   for (Int_t n2=0; n2<nClusters2; n2++) {
     AliCluster *c2=layer2.GetCluster(n2);
     ((AliITSUClusterPix*)c2)->GoToFrameTrk();
   }
   for (Int_t n3=0; n3<nClusters3; n3++) {
     AliCluster *c3=layer3.GetCluster(n3);
     ((AliITSUClusterPix*)c3)->GoToFrameTrk();
   }

   return fSeeds->GetEntriesFast();
}

Int_t AliITSUTrackerCooked::Clusters2Tracks(AliESDEvent *event) {
  //--------------------------------------------------------------------
  // This is the main tracking function
  // The clusters must already be loaded
  //--------------------------------------------------------------------

  if (!fSAonly) AliITSUTrackerGlo::Clusters2Tracks(event);

  if (fSeeds) {fSeeds->Delete(); delete fSeeds;}
  fSeeds=new TObjArray(77777);

  //Seeding with the triggered primary vertex
  Double_t xyz[3];
  const AliESDVertex *vtx=0;
  vtx=event->GetPrimaryVertexSPD();
  if (vtx->GetStatus()) {
     xyz[0]=vtx->GetX(); xyz[1]=vtx->GetY(); xyz[2]=vtx->GetZ();
     SetVertex(xyz);
     MakeSeeds();
  }
  //Seeding with the pileup primary vertices
  TClonesArray *verticesSPD=event->GetPileupVerticesSPD();
  Int_t nfoundSPD=verticesSPD->GetEntries(); 
  for (Int_t v=0; v<nfoundSPD; v++) {
      vtx=(AliESDVertex *)verticesSPD->UncheckedAt(v);
      if (!vtx->GetStatus()) continue;
      xyz[0]=vtx->GetX(); xyz[1]=vtx->GetY(); xyz[2]=vtx->GetZ();
      SetVertex(xyz);
      MakeSeeds();
  }
  fSeeds->Sort();
  Int_t nSeeds=fSeeds->GetEntriesFast();

  // Possibly, icrement the seeds with additional clusters (Kalman)

  // Possibly, (re)fit the found tracks 

  Int_t ngood=0;
  for (Int_t s=0; s<nSeeds; s++) {
      const AliITSUTrackCooked *track=(AliITSUTrackCooked*)fSeeds->At(s);
 
      Double_t x=track->GetX();
      Double_t y=track->GetY();
      Double_t phi=track->GetAlpha() + TMath::ATan2(y,x);
      const Float_t pi2 = 2.*TMath::Pi();
      if (phi<0.) phi+=pi2;
      else if (phi >= pi2) phi-=pi2;
      for (Int_t n=0; n<kNLayers-3; n++) {
        Double_t z;
        track->GetZAt(fgLayers[n].GetR(),GetBz(),z);
        fgLayers[n].SelectClusters(phi,kRoadY,z,kRoadZ);
      }
      
      ResetTrackToFollow(*track);
      ResetBestTrack();
      fI=kSeedingLayer2;
      fgLayers[fI].ResetTrack(*track);

      for (FollowProlongation(); fI<kSeedingLayer2; fI++) {
          while (TakeNextProlongation()) FollowProlongation();
      }

      if (fBestTrack->GetNumberOfClusters() < kminNumberOfClusters) continue;

      CookLabel(fBestTrack,0.); //For comparison only
      Int_t label=fBestTrack->GetLabel();
      if (label>0) ngood++;

      AliESDtrack iotrack;
      iotrack.UpdateTrackParams(fBestTrack,AliESDtrack::kITSin);
      iotrack.SetLabel(label);
      event->AddTrack(&iotrack);
      UseClusters(fBestTrack);
  }

  Info("Clusters2Tracks","Seeds: %d",nSeeds);
  if (nSeeds)
  Info("Clusters2Tracks","Good tracks/seeds: %f",Float_t(ngood)/nSeeds);

  if (fSeeds) {fSeeds->Delete(); delete fSeeds;}
  fSeeds=0;
    
  return 0;
}

void AliITSUTrackerCooked::FollowProlongation() {
  //--------------------------------------------------------------------
  // Push this track tree branch towards the primary vertex
  //--------------------------------------------------------------------
  while (fI) {
    fI--;
    fgLayers[fI].ResetSelectedClusters();  
    if (!TakeNextProlongation()) return;
  } 

  //deal with the best track
  Int_t ncl=fTrackToFollow->GetNumberOfClusters();
  Int_t nclb=fBestTrack->GetNumberOfClusters();
  if (ncl >= nclb) {
     Double_t chi2=fTrackToFollow->GetChi2();
     if (chi2 < kmaxChi2PerTrack) {        
        if (ncl > nclb || chi2 < fBestTrack->GetChi2()) {
	   ResetBestTrack();
        }
     }
  }

}

Int_t AliITSUTrackerCooked::TakeNextProlongation() {
  //--------------------------------------------------------------------
  // Switch to the next track tree branch
  //--------------------------------------------------------------------
  AliITSUlayer &layer=fgLayers[fI];

  const AliCluster *c=0; Int_t ci=-1;
  const AliCluster *cc=0; Int_t cci=-1;
  UShort_t volId=-1;
  Double_t z=0., dz=0., y=0., dy=0., chi2=0.; 
  while ((c=layer.GetNextCluster(ci))!=0) {
    Int_t id=c->GetVolumeId();
    if (id != volId) {
       volId=id;
       const AliITSUTrackCooked *t = fgLayers[fI+1].GetTrack();
       ResetTrackToFollow(*t);
       Double_t x=layer.GetXRef(ci);
       Double_t alpha=layer.GetAlphaRef(ci);
       if (!fTrackToFollow->Propagate(alpha, x, GetBz())) {
         //Warning("TakeNextProlongation","propagation failed !\n");
          continue;
       }
       dz=7*TMath::Sqrt(fTrackToFollow->GetSigmaZ2() + kSigma2);
       dy=7*TMath::Sqrt(fTrackToFollow->GetSigmaY2() + kSigma2);
       z=fTrackToFollow->GetZ();
       y=fTrackToFollow->GetY();
    }

    //if (TMath::Abs(fTrackToFollow.GetZ()-GetZ())>layer.GetR()+dz) continue;

    if (TMath::Abs(z - c->GetZ()) > dz) continue;
    if (TMath::Abs(y - c->GetY()) > dy) continue;

    Double_t ch2=fTrackToFollow->GetPredictedChi2(c); 
    if (ch2 > kmaxChi2PerCluster) continue;
    chi2=ch2;
    cc=c; cci=ci;
    break;
  }

  if (!cc) return 0;

  if (!fTrackToFollow->Update(cc,chi2,(fI<<28)+cci)) {
     //Warning("TakeNextProlongation","filtering failed !\n");
     return 0;
  }
  Double_t xx0 = (fI > 2) ? 0.008 : 0.003;  // Rough layer thickness
  Double_t x0  = 9.36; // Radiation length of Si [cm]
  Double_t rho = 2.33; // Density of Si [g/cm^3] 
  Double_t mass = fTrackToFollow->GetMass();
  fTrackToFollow->CorrectForMeanMaterial(xx0, xx0*x0*rho, mass, kTRUE);
  layer.ResetTrack(*fTrackToFollow); 


  return 1;
}

Int_t AliITSUTrackerCooked::PropagateBack(AliESDEvent *event) {
  //--------------------------------------------------------------------
  // Here, we implement the Kalman smoother ?
  // The clusters must already be loaded
  //--------------------------------------------------------------------
  Int_t n=event->GetNumberOfTracks();
  Int_t ntrk=0;
  Int_t ngood=0;
  for (Int_t i=0; i<n; i++) {
      AliESDtrack *esdTrack=event->GetTrack(i);

      if (!esdTrack->IsOn(AliESDtrack::kITSin)) continue;
      if ( esdTrack->IsOn(AliESDtrack::kTPCin)) continue;//skip a TPC+ITS track

      AliITSUTrackCooked track(*esdTrack);

      ResetTrackToFollow(track);

      fTrackToFollow->ResetCovariance(10.); fTrackToFollow->ResetClusters();
      if (RefitAt(40., fTrackToFollow, &track)) {

         CookLabel(fTrackToFollow, 0.); //For comparison only
         Int_t label=fTrackToFollow->GetLabel();
         if (label>0) ngood++;

         esdTrack->UpdateTrackParams(fTrackToFollow,AliESDtrack::kITSout);
         //UseClusters(fTrackToFollow);
         ntrk++;
      }
  }

  Info("PropagateBack","Back propagated tracks: %d",ntrk);
  if (ntrk)
  Info("PropagateBack","Good tracks/back propagated: %f",Float_t(ngood)/ntrk);
  
  if (!fSAonly) AliITSUTrackerGlo::PropagateBack(event);
  
  return 0;
}

Bool_t AliITSUTrackerCooked::
RefitAt(Double_t xx, AliITSUTrackCooked *t, const AliITSUTrackCooked *c) {
  //--------------------------------------------------------------------
  // This function refits the track "t" at the position "x" using
  // the clusters from "c"
  //--------------------------------------------------------------------
  Int_t index[kNLayers];
  Int_t k;
  for (k=0; k<kNLayers; k++) index[k]=-1;
  Int_t nc=c->GetNumberOfClusters();
  for (k=0; k<nc; k++) {
    Int_t idx=c->GetClusterIndex(k), nl=(idx&0xf0000000)>>28;
    index[nl]=idx;
  }

  Int_t from, to, step;
  if (xx > t->GetX()) {
      from=0; to=kNLayers;
      step=+1;
  } else {
      from=kNLayers-1; to=-1;
      step=-1;
  }

  for (Int_t i=from; i != to; i += step) {
     Int_t idx=index[i];
     if (idx>=0) {
        const AliCluster *cl=GetCluster(idx);
        Float_t xr,ar; cl->GetXAlphaRefPlane(xr, ar);
        if (!t->Propagate(Double_t(ar), Double_t(xr), GetBz())) {
           //Warning("RefitAt","propagation failed !\n");
           return kFALSE;
        }
        Double_t chi2=t->GetPredictedChi2(cl);
        if (chi2 < kmaxChi2PerCluster) t->Update(cl, chi2, idx);
     } else {
        Double_t r=fgLayers[i].GetR();
        Double_t phi,z;
        if (!t->GetPhiZat(r,phi,z)) {
           //Warning("RefitAt","failed to estimate track !\n");
           return kFALSE;
        }
        if (!t->Propagate(phi, r, GetBz())) {
           //Warning("RefitAt","propagation failed !\n");
           return kFALSE;
        }
     }
     Double_t xx0 = (i > 2) ? 0.008 : 0.003;  // Rough layer thickness
     Double_t x0  = 9.36; // Radiation length of Si [cm]
     Double_t rho = 2.33; // Density of Si [g/cm^3]
     Double_t mass = t->GetMass();
     t->CorrectForMeanMaterial(xx0, -step*xx0*x0*rho, mass, kTRUE);
  }

  if (!t->PropagateTo(xx,0.,0.)) return kFALSE;
  return kTRUE;
}

Int_t AliITSUTrackerCooked::RefitInward(AliESDEvent *event) {
  //--------------------------------------------------------------------
  // Some final refit, after the outliers get removed by the smoother ?  
  // The clusters must be loaded
  //--------------------------------------------------------------------
  Int_t n=event->GetNumberOfTracks();
  Int_t ntrk=0;
  Int_t ngood=0;
  for (Int_t i=0; i<n; i++) {
      AliESDtrack *esdTrack=event->GetTrack(i);

      if (!esdTrack->IsOn(AliESDtrack::kITSout)) continue;
      if ( esdTrack->IsOn(AliESDtrack::kTPCin)) continue;//skip a TPC+ITS track

      AliITSUTrackCooked track(*esdTrack);
      ResetTrackToFollow(track);

      fTrackToFollow->ResetCovariance(10.); fTrackToFollow->ResetClusters();
      if (!RefitAt(2.1, fTrackToFollow, &track)) continue;
      //Cross the beam pipe
      if (!fTrackToFollow->PropagateTo(1.8, 2.27e-3, 35.28*1.848)) continue;

      CookLabel(fTrackToFollow, 0.); //For comparison only
      Int_t label=fTrackToFollow->GetLabel();
      if (label>0) ngood++;

      esdTrack->UpdateTrackParams(fTrackToFollow,AliESDtrack::kITSrefit);
      //esdTrack->RelateToVertex(event->GetVertex(),GetBz(),33.);
      //UseClusters(fTrackToFollow);
      ntrk++;
  }

  Info("RefitInward","Refitted tracks: %d",ntrk);
  if (ntrk)
  Info("RefitInward","Good tracks/refitted: %f",Float_t(ngood)/ntrk);
    
  if (!fSAonly) AliITSUTrackerGlo::RefitInward(event);

  return 0;
}

Int_t AliITSUTrackerCooked::LoadClusters(TTree *cTree) {
  //--------------------------------------------------------------------
  // This function reads the ITSU clusters from the tree,
  // sort them, distribute over the internal tracker arrays, etc
  //--------------------------------------------------------------------
  if (!cTree) {
     AliFatal("No cluster tree !");
     return 1;
  }

  AliITSUTrackerGlo::LoadClusters(cTree);

  for (Int_t i=0; i<kNLayers; i++) {
      TClonesArray *clusters=fReconstructor->GetClusters(i);
      switch (i) {
      case kSeedingLayer1: 
      case kSeedingLayer2: 
      case kSeedingLayer3: 
	 fgLayers[i].InsertClusters(clusters,kTRUE,fSAonly);
         break;
      default:
	 fgLayers[i].InsertClusters(clusters,kFALSE,fSAonly);
         break;
      }
  }

  return 0;
}

void AliITSUTrackerCooked::UnloadClusters() {
  //--------------------------------------------------------------------
  // This function unloads ITSU clusters from the RAM
  //--------------------------------------------------------------------
  AliITSUTrackerGlo::UnloadClusters();
  for (Int_t i=0; i<kNLayers; i++) fgLayers[i].DeleteClusters();
}

AliCluster *AliITSUTrackerCooked::GetCluster(Int_t index) const {
  //--------------------------------------------------------------------
  //       Return pointer to a given cluster
  //--------------------------------------------------------------------
    Int_t l=(index & 0xf0000000) >> 28;
    Int_t c=(index & 0x0fffffff) >> 00;
    return fgLayers[l].GetCluster(c);
}

AliITSUTrackerCooked::AliITSUlayer::AliITSUlayer():
  fR(0),
  fN(0),
  fNsel(0),
  fI(0),
  fTrack(0) 
{
  //--------------------------------------------------------------------
  // This default constructor needs to be provided
  //--------------------------------------------------------------------
  for (Int_t i=0; i<kMaxClusterPerLayer; i++) fClusters[i]=0;
  for (Int_t i=0; i<kMaxSelected; i++) fIndex[i]=-1;
}

AliITSUTrackerCooked::AliITSUlayer::~AliITSUlayer()
{
  //--------------------------------------------------------------------
  // Simple destructor
  //--------------------------------------------------------------------
  delete fTrack;
}

void 
AliITSUTrackerCooked::AliITSUlayer::ResetTrack(const AliITSUTrackCooked &t) {
  //--------------------------------------------------------------------
  // Replace the track estimate at this layer
  //--------------------------------------------------------------------
   delete fTrack;
   fTrack=new AliITSUTrackCooked(t);
}

void AliITSUTrackerCooked::AliITSUlayer::
InsertClusters(TClonesArray *clusters, Bool_t seedingLayer, Bool_t saOnly)
{
  //--------------------------------------------------------------------
  // Load clusters to this layer
  //--------------------------------------------------------------------
  Int_t ncl=clusters->GetEntriesFast();
  Double_t r=0.;
  for (Int_t i=0; i<ncl; i++) {
     AliITSUClusterPix *c=(AliITSUClusterPix*)clusters->UncheckedAt(i);
     if (!saOnly) if (c->IsClusterUsed()) continue;
     c->GoToFrameGlo();
     Double_t x=c->GetX(), y=c->GetY();
     r += TMath::Sqrt(x*x + y*y);
     if (!seedingLayer) c->GoToFrameTrk();
     //if (!c->Misalign()) AliWarning("Can't misalign this cluster !");
     if (InsertCluster(c)) break;
  }
  if (fN) fR = r/fN;
  const Float_t pi2 = 2.*TMath::Pi();
  for (Int_t i=0; i<fN; i++) {
      AliCluster *c=fClusters[i];
      c->GetXAlphaRefPlane(fXRef[i],fAlphaRef[i]);
      Float_t xyz[3]; c->GetGlobalXYZ(xyz);
      Float_t phi=TMath::ATan2(xyz[1],xyz[0]);
      if (phi<0.) phi+=pi2;
      else if (phi >= pi2) phi-=pi2;
      fPhi[i]=phi;
  }
}

void AliITSUTrackerCooked::AliITSUlayer::DeleteClusters()
{
  //--------------------------------------------------------------------
  // Load clusters to this layer
  //--------------------------------------------------------------------
  //for (Int_t i=0; i<fN; i++) {delete fClusters[i]; fClusters[i]=0;}
  fN=0;
}

Int_t 
AliITSUTrackerCooked::AliITSUlayer::InsertCluster(AliCluster *c) {
  //--------------------------------------------------------------------
  // This function inserts a cluster to this layer in increasing
  // order of the cluster's fZ
  //--------------------------------------------------------------------
  if (fN>=kMaxClusterPerLayer) {
     ::Error("InsertCluster","Too many clusters !\n");
     return 1;
  }
  if (fN==0) fClusters[0]=c;
  else {
     Int_t i=FindClusterIndex(c->GetZ());
     Int_t k=fN-i;
     memmove(fClusters+i+1 ,fClusters+i,k*sizeof(AliCluster*));
     fClusters[i]=c;
  }
  fN++;
  return 0;
}

Int_t 
AliITSUTrackerCooked::AliITSUlayer::FindClusterIndex(Double_t z) const {
  //--------------------------------------------------------------------
  // This function returns the index of the first 
  // with its fZ >= "z". 
  //--------------------------------------------------------------------
  if (fN==0) return 0;

  Int_t b=0;
  if (z <= fClusters[b]->GetZ()) return b;

  Int_t e=b+fN-1;
  if (z > fClusters[e]->GetZ()) return e+1;

  Int_t m=(b+e)/2;
  for (; b<e; m=(b+e)/2) {
    if (z > fClusters[m]->GetZ()) b=m+1;
    else e=m; 
  }
  return m;
}

void AliITSUTrackerCooked::AliITSUlayer::
SelectClusters(Float_t phi, Float_t dy, Float_t z, Float_t dz) {
  //--------------------------------------------------------------------
  // This function selects clusters within the "road"
  //--------------------------------------------------------------------
  fNsel=0;

  Float_t dphi=dy/fR; 
  Float_t phiMin=phi-dphi;
  Float_t phiMax=phi+dphi;
  Float_t zMin=z-dz;
  Float_t zMax=z+dz;
 
  Int_t i=FindClusterIndex(zMin), imax=FindClusterIndex(zMax);
  for (; i<imax; i++) {
      Float_t cphi=fPhi[i];
      if (cphi <= phiMin) continue;
      if (cphi >  phiMax) continue;
      AliCluster *c=fClusters[i];
      if (c->IsClusterUsed()) continue;

      fIndex[fNsel++]=i;
      if (fNsel>=kMaxSelected) break;
  } 
}

const AliCluster *AliITSUTrackerCooked::AliITSUlayer::GetNextCluster(Int_t &ci){
  //--------------------------------------------------------------------
  // This function returns clusters within the "road" 
  //--------------------------------------------------------------------
  if (fI<fNsel) {
      ci=fIndex[fI++];
      return fClusters[ci];       
  }
  ci=-1;
  return 0; 
}


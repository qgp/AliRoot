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

/*
$Log$
Revision 1.9.4.1  2003/06/19 06:59:58  hristov
Updated version of parallel tracking (M.Ivanov)

Revision 1.9  2003/03/19 17:14:11  hristov
Load/UnloadClusters added to the base class and the derived classes changed correspondingly. Possibility to give 2 input files for ITS and TPC tracks in PropagateBack. TRD tracker uses fEventN from the base class (T.Kuhr)

Revision 1.8  2003/03/05 11:16:15  kowal2
Logs added

*/







/*
  AliTPC parallel tracker - 
  How to use?  - 
  run AliTPCFindClusters.C macro - clusters neccessary for tracker are founded
  run AliTPCFindTracksMI.C macro - to find tracks
  tracks are written to AliTPCtracks.root file
  for comparison also seeds are written to the same file - to special branch
*/

//-------------------------------------------------------
//          Implementation of the TPC tracker
//
//   Origin: Marian Ivanov   Marian.Ivanov@cern.ch
// 
//-------------------------------------------------------

#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>
#include "Riostream.h"

#include "AliTPCtrackerMI.h"
#include "AliTPCclusterMI.h"
#include "AliTPCParam.h"
#include "AliTPCClustersRow.h"
#include "AliComplexCluster.h"
#include "AliTPCpolyTrack.h"
#include "TStopwatch.h"


ClassImp(AliTPCseed)



AliTPCclusterTracks::AliTPCclusterTracks(){
  // class for storing overlaping info
  fTrackIndex[0]=-1;
  fTrackIndex[1]=-1;
  fTrackIndex[2]=-1;
  fDistance[0]=1000;
  fDistance[1]=1000;
  fDistance[2]=1000;
}

Int_t AliTPCtrackerMI::UpdateTrack(AliTPCseed * track, Bool_t accept, Bool_t debug){

  AliTPCclusterMI* c =track->fCurrentCluster;
  if (!accept) track->fCurrentClusterIndex1 |=0x8000;  //sign not accepted clusters

  UInt_t i = track->fCurrentClusterIndex1;

  Int_t sec=(i&0xff000000)>>24; 
  Int_t row = (i&0x00ff0000)>>16;
  track->fRow=(i&0x00ff0000)>>16;
  track->fSector = sec;
  //  Int_t index = i&0xFFFF;
  if (sec>=fParam->GetNInnerSector()) track->fRow += fParam->GetNRowLow(); 
  track->SetClusterIndex2(track->fRow, i);
  
  track->fFirstPoint = row;
  if ( track->fLastPoint<row) track->fLastPoint =row;
  track->fClusterPointer[track->fRow] = c;
  
  //

  Float_t angle2 = track->GetSnp()*track->GetSnp();
  angle2 = TMath::Sqrt(angle2/(1-angle2)); 
  //
  //SET NEW Track Point
  //
  //  if (debug)
  {
    AliTPCTrackPoint   *trpoint =track->GetTrackPoint(track->fRow);
    /*
    if (c!=0){
      //if we have a cluster
      trpoint->GetCPoint().SetY(c->GetY());
      trpoint->GetCPoint().SetZ(c->GetZ());    
      //
      //
      trpoint->GetCPoint().SetType(c->GetType());
      trpoint->GetCPoint().SetQ(c->GetQ());
      trpoint->GetCPoint().SetMax(c->GetMax());
      //  
      trpoint->GetCPoint().SetErrY(TMath::Sqrt(track->fErrorY2));
      trpoint->GetCPoint().SetErrZ(TMath::Sqrt(track->fErrorZ2));
      //
    }
    */
    trpoint->GetTPoint().SetSigmaY(c->GetSigmaY2()/(track->fCurrentSigmaY*track->fCurrentSigmaY));
    trpoint->GetTPoint().SetSigmaZ(c->GetSigmaZ2()/(track->fCurrentSigmaZ*track->fCurrentSigmaZ));

    trpoint->GetTPoint().SetX(track->GetX());
    trpoint->GetTPoint().SetY(track->GetY());
    trpoint->GetTPoint().SetZ(track->GetZ());
    //
    trpoint->GetTPoint().SetAngleY(angle2);
    trpoint->GetTPoint().SetAngleZ(track->GetTgl());
  }  

  Double_t chi2 = track->GetPredictedChi2(track->fCurrentCluster);

  //  if (track->fIsSeeding){ 
  
  track->fErrorY2 *= 1.3;
  track->fErrorY2 += 0.01;    
  track->fErrorZ2 *= 1.3;   
  track->fErrorZ2 += 0.005;      
    //}
  if (!accept) return 0;
  return track->Update(c,chi2,i);

}
//_____________________________________________________________________________
AliTPCtrackerMI::AliTPCtrackerMI(const AliTPCParam *par, Int_t eventn): 
AliTracker(), fkNIS(par->GetNInnerSector()/2), fkNOS(par->GetNOuterSector()/2)
{
  //---------------------------------------------------------------------
  // The main TPC tracker constructor
  //---------------------------------------------------------------------
  fInnerSec=new AliTPCSector[fkNIS];         
  fOuterSec=new AliTPCSector[fkNOS];

  Int_t i;
  for (i=0; i<fkNIS; i++) fInnerSec[i].Setup(par,0);
  for (i=0; i<fkNOS; i++) fOuterSec[i].Setup(par,1);

  fN=0;  fSectors=0;

  fClustersArray.Setup(par);
  fClustersArray.SetClusterType("AliTPCclusterMI");

  char   cname[100];
  if (eventn==-1) {
    sprintf(cname,"TreeC_TPC");
  }
  else {
    sprintf(cname,"TreeC_TPC_%d",eventn);
  }

  fClustersArray.ConnectTree(cname);

  fEventN = eventn;
  fSeeds=0;
  fNtracks = 0;
  fParam = par;
  //fSeedPool = 0;
}

//_____________________________________________________________________________
AliTPCtrackerMI::~AliTPCtrackerMI() {
  //------------------------------------------------------------------
  // TPC tracker destructor
  //------------------------------------------------------------------
  delete[] fInnerSec;
  delete[] fOuterSec;
  if (fSeeds) {
    fSeeds->Delete(); 
    delete fSeeds;
  }
}
/*
AliTPCseed * AliTPCtrackerMI::NewSeed()
{
  //return seed from the pool or create new
  if (fSeedPool==0) fSeedPool = new TObjArray(1000);
  AliTPCseed * seed =0;
  if (fSeedPool->GetEntriesFast()<1) {   
    seed = new AliTPCseed;
    //fSeedPool->AddLast(seed);
    return seed;
  }
  seed = (AliTPCseed*)(fSeedPool->Last());
  fSeedPool->RemoveLast();
  return seed;
}

void AliTPCtrackerMI::DeleteSeed(TObject * seed)
{
  // add seed to the list of available
  if (fSeedPool)
    fSeedPool->AddLast(seed);
}

void AliTPCtrackerMI::CompressSeed(Int_t max)
{
  // compress pool with seeds
  if (!fSeedPool)
    return;
  if (max>fSeedPool->GetEntriesFast()) return;
  for (Int_t i=fSeedPool->GetEntriesFast();i>max;i--){
    delete fSeedPool->RemoveAt(i);
  }
  fSeedPool->Compress();
}
*/

Double_t AliTPCtrackerMI::ErrY2(AliTPCseed* seed, AliTPCclusterMI * cl){
  //
  //
  Float_t snoise2;
  Float_t z = TMath::Abs(fParam->GetZLength()-TMath::Abs(seed->GetZ()));

  //cluster "quality"
  Float_t rsigmay = 1;
  Int_t ctype = 0;

  //standard if we don't have cluster - take MIP
  const Float_t chmip = 50.; 
  Float_t amp = chmip/0.3;  
  Float_t nel;
  Float_t nprim;
  if (cl){
    amp = cl->GetQ();
    rsigmay = cl->GetSigmaY2()/(seed->fCurrentSigmaY*seed->fCurrentSigmaY);
    ctype = cl->GetType();
  }
  

  Float_t landau=2 ;    //landau fluctuation part
  Float_t gg=2;         // gg fluctuation part
  Float_t padlength= fSectors->GetPadPitchLength(seed->GetX());

  if (fSectors==fInnerSec){
    snoise2 = 0.0004/padlength;
    nel     = 0.268*amp;
    nprim   = 0.155*amp;
    gg      = (2+0.001*nel/(padlength*padlength))/nel;
    landau  = (2.+0.12*nprim)*0.5*(2.+nprim*nprim*0.001/(padlength*padlength))/nprim;
    if (landau>1) landau=1;
  }
  else {
    snoise2 = 0.0004/padlength;
    nel     = 0.3*amp;
    nprim   = 0.133*amp;

    gg      = (2+0.0008*nel/(padlength*padlength))/nel;
    landau  = (2.+0.12*nprim)*0.5*(2.+nprim*nprim*0.001/(padlength*padlength))/nprim;
    if (landau>1) landau=1;
  }


  Float_t sdiff = gg*fParam->GetDiffT()*fParam->GetDiffT()*z;
  Float_t angle2 = seed->GetSnp()*seed->GetSnp();
  angle2 = angle2/(1-angle2); 
  Float_t angular = landau*angle2*padlength*padlength/12.;
  Float_t res = sdiff + angular;
  
    
  if ((ctype==0) && (fSectors ==fOuterSec))
    res *= 0.78 +TMath::Exp(7.4*(rsigmay-1.2));

  if ((ctype==0) && (fSectors ==fInnerSec))
    res *= 0.72 +TMath::Exp(3.36*(rsigmay-1.2));
  

  if ((ctype>0))
    res*= TMath::Power((rsigmay+0.5),1.5)+0.0064;
  
  if (ctype<0)
    res*=2.4;  // overestimate error 2 times
  
  res+= snoise2;
 
  if (res<2*snoise2)
    res = 2*snoise2;
  
  seed->SetErrorY2(res);
  return res;
}




Double_t AliTPCtrackerMI::ErrZ2(AliTPCseed* seed, AliTPCclusterMI * cl){
  //
  //
  Float_t snoise2;
  Float_t z = TMath::Abs(fParam->GetZLength()-TMath::Abs(seed->GetZ()));
  //signal quality
  Float_t rsigmaz=1;
  Int_t ctype =0;

  const Float_t chmip = 50.;
  Float_t amp = chmip/0.3;  
  Float_t nel;
  Float_t nprim;
  if (cl){
    amp = cl->GetQ();
    rsigmaz = cl->GetSigmaZ2()/(seed->fCurrentSigmaZ*seed->fCurrentSigmaZ);
    ctype = cl->GetType();
  }

  //
  Float_t landau=2 ;    //landau fluctuation part
  Float_t gg=2;         // gg fluctuation part
  Float_t padlength= fSectors->GetPadPitchLength(seed->GetX());
 
  if (fSectors==fInnerSec){
    snoise2 = 0.0004/padlength;
    nel     = 0.268*amp;
    nprim   = 0.155*amp;
    gg      = (2+0.001*nel/(padlength*padlength))/nel;
    landau  = (2.+0.12*nprim)*0.5*(2.+nprim*nprim*0.001/(padlength*padlength))/nprim;
    if (landau>1) landau=1;
  }
  else {
    snoise2 = 0.0004/padlength;
    nel     = 0.3*amp;
    nprim   = 0.133*amp;
    gg      = (2+0.0008*nel/(padlength*padlength))/nel;
    landau  = (2.+0.12*nprim)*0.5*(2.+nprim*nprim*0.001/(padlength*padlength))/nprim;
    if (landau>1) landau=1;
  }
  Float_t sdiff = gg*fParam->GetDiffT()*fParam->GetDiffT()*z;

  //
  Float_t angle2 = seed->GetSnp()*seed->GetSnp();
  angle2 = TMath::Sqrt((1-angle2));
  if (angle2<0.6) angle2 = 0.6;
  //angle2 = 1;

  Float_t angle = seed->GetTgl()/angle2;
  Float_t angular = landau*angle*angle*padlength*padlength/12.;
  Float_t res = sdiff + angular;

  
  if ((ctype==0) && (fSectors ==fOuterSec))
    res *= 0.81 +TMath::Exp(6.8*(rsigmaz-1.2));

  if ((ctype==0) && (fSectors ==fInnerSec))
    res *= 0.72 +TMath::Exp(2.04*(rsigmaz-1.2));
  
  if ((ctype>0))
    res*= TMath::Power(rsigmaz+0.5,1.5)+0.0064;  //0.31+0.147*ctype;
  if (ctype<0)
    res*=1.3;
  if ((ctype<0) &&amp<70)
    res*=1.3;  

  res += snoise2;
  if (res<2*snoise2)
     res = 2*snoise2;

  seed->SetErrorZ2(res);
  return res;
}




void AliTPCseed::Reset(Bool_t all)
{
  //
  //
  SetNumberOfClusters(0);
  fNFoundable = 0;
  SetChi2(0);
  ResetCovariance();
  if (fTrackPoints){
    for (Int_t i=0;i<8;i++){
      delete [] fTrackPoints[i];
    }
    delete fTrackPoints;
    fTrackPoints =0;
  }


  if (all){   
    for (Int_t i=0;i<200;i++) SetClusterIndex2(i,-3);
    for (Int_t i=0;i<160;i++) fClusterPointer[i]=0;
  }

}





Int_t  AliTPCseed::GetProlongation(Double_t xk, Double_t &y, Double_t & z) const
{
  //-----------------------------------------------------------------
  // This function find proloncation of a track to a reference plane x=xk.
  // doesn't change internal state of the track
  //-----------------------------------------------------------------
  
  Double_t x1=fX, x2=x1+(xk-x1), dx=x2-x1;
  //  Double_t y1=fP0, z1=fP1;
  Double_t c1=fP4*x1 - fP2, r1=sqrt(1.- c1*c1);
  Double_t c2=fP4*x2 - fP2, r2=sqrt(1.- c2*c2);
  
  y = fP0;
  z = fP1;
  y += dx*(c1+c2)/(r1+r2);
  z += dx*(c1+c2)/(c1*r2 + c2*r1)*fP3;
  return 0;  
}


//_____________________________________________________________________________
Double_t AliTPCseed::GetPredictedChi2(const AliTPCclusterMI *c) const 
{
  //-----------------------------------------------------------------
  // This function calculates a predicted chi2 increment.
  //-----------------------------------------------------------------
  //Double_t r00=c->GetSigmaY2(), r01=0., r11=c->GetSigmaZ2();
  Double_t r00=fErrorY2, r01=0., r11=fErrorZ2;
  r00+=fC00; r01+=fC10; r11+=fC11;

  Double_t det=r00*r11 - r01*r01;
  if (TMath::Abs(det) < 1.e-10) {
    Int_t n=GetNumberOfClusters();
    if (n>4) cerr<<n<<" AliKalmanTrack warning: Singular matrix !\n";
    return 1e10;
  }
  Double_t tmp=r00; r00=r11; r11=tmp; r01=-r01;
  
  Double_t dy=c->GetY() - fP0, dz=c->GetZ() - fP1;
  
  return (dy*r00*dy + 2*r01*dy*dz + dz*r11*dz)/det;
}


//_________________________________________________________________________________________


Int_t AliTPCseed::Compare(const TObject *o) const {
  //-----------------------------------------------------------------
  // This function compares tracks according to the sector - for given sector according z
  //-----------------------------------------------------------------
  AliTPCseed *t=(AliTPCseed*)o;

  if (fSort == 0){
    if (t->fRelativeSector>fRelativeSector) return -1;
    if (t->fRelativeSector<fRelativeSector) return 1;
    Double_t z2 = t->GetZ();
    Double_t z1 = GetZ();
    if (z2>z1) return 1;
    if (z2<z1) return -1;
    return 0;
  }
  else {
    if (t->GetNumberOfClusters() <GetNumberOfClusters()) return -1;
    else return +1;
  }
}

void AliTPCtrackerMI::RotateToLocal(AliTPCseed *seed)
{
  //rotate to track "local coordinata
  Float_t x = seed->GetX();
  Float_t y = seed->GetY();
  Float_t ymax = x*TMath::Tan(0.5*fSectors->GetAlpha());
  if (y > ymax) {
    seed->fRelativeSector= (seed->fRelativeSector+1) % fN;
    if (!seed->Rotate(fSectors->GetAlpha())) 
      return;
  } else if (y <-ymax) {
    seed->fRelativeSector= (seed->fRelativeSector-1+fN) % fN;
    if (!seed->Rotate(-fSectors->GetAlpha())) 
      return;
  }   

}




//_____________________________________________________________________________
Int_t AliTPCseed::Update(const AliTPCclusterMI *c, Double_t chisq, UInt_t index) {
  //-----------------------------------------------------------------
  // This function associates a cluster with this track.
  //-----------------------------------------------------------------
  //  Double_t r00=c->GetSigmaY2(), r01=0., r11=c->GetSigmaZ2();
  //Double_t r00=sigmay2, r01=0., r11=sigmaz2;
  Double_t r00=fErrorY2, r01=0., r11=fErrorZ2;

  r00+=fC00; r01+=fC10; r11+=fC11;
  Double_t det=r00*r11 - r01*r01;
  Double_t tmp=r00; r00=r11/det; r11=tmp/det; r01=-r01/det;

  Double_t k00=fC00*r00+fC10*r01, k01=fC00*r01+fC10*r11;
  Double_t k10=fC10*r00+fC11*r01, k11=fC10*r01+fC11*r11;
  Double_t k20=fC20*r00+fC21*r01, k21=fC20*r01+fC21*r11;
  Double_t k30=fC30*r00+fC31*r01, k31=fC30*r01+fC31*r11;
  Double_t k40=fC40*r00+fC41*r01, k41=fC40*r01+fC41*r11;

  Double_t dy=c->GetY() - fP0, dz=c->GetZ() - fP1;
  Double_t cur=fP4 + k40*dy + k41*dz, eta=fP2 + k20*dy + k21*dz;
  if (TMath::Abs(cur*fX-eta) >= 0.9) {
    //    Int_t n=GetNumberOfClusters();
    //if (n>4) cerr<<n<<" AliTPCtrack warning: Filtering failed !\n";
    return 0;
  }

  fP0 += k00*dy + k01*dz;
  fP1 += k10*dy + k11*dz;
  fP2  = eta;
  fP3 += k30*dy + k31*dz;
  fP4  = cur;

  Double_t c01=fC10, c02=fC20, c03=fC30, c04=fC40;
  Double_t c12=fC21, c13=fC31, c14=fC41;

  fC00-=k00*fC00+k01*fC10; fC10-=k00*c01+k01*fC11;
  fC20-=k00*c02+k01*c12;   fC30-=k00*c03+k01*c13;
  fC40-=k00*c04+k01*c14; 

  fC11-=k10*c01+k11*fC11;
  fC21-=k10*c02+k11*c12;   fC31-=k10*c03+k11*c13;
  fC41-=k10*c04+k11*c14; 

  fC22-=k20*c02+k21*c12;   fC32-=k20*c03+k21*c13;
  fC42-=k20*c04+k21*c14; 

  fC33-=k30*c03+k31*c13;
  fC43-=k40*c03+k41*c13; 

  fC44-=k40*c04+k41*c14; 

  Int_t n=GetNumberOfClusters();
  //  fIndex[n]=index;
  SetNumberOfClusters(n+1);
  SetChi2(GetChi2()+chisq);

  return 1;
}



//_____________________________________________________________________________
Double_t AliTPCtrackerMI::f1(Double_t x1,Double_t y1,
                   Double_t x2,Double_t y2,
                   Double_t x3,Double_t y3) 
{
  //-----------------------------------------------------------------
  // Initial approximation of the track curvature
  //-----------------------------------------------------------------
  Double_t d=(x2-x1)*(y3-y2)-(x3-x2)*(y2-y1);
  Double_t a=0.5*((y3-y2)*(y2*y2-y1*y1+x2*x2-x1*x1)-
                  (y2-y1)*(y3*y3-y2*y2+x3*x3-x2*x2));
  Double_t b=0.5*((x2-x1)*(y3*y3-y2*y2+x3*x3-x2*x2)-
                  (x3-x2)*(y2*y2-y1*y1+x2*x2-x1*x1));

  Double_t xr=TMath::Abs(d/(d*x1-a)), yr=d/(d*y1-b);
  if ( xr*xr+yr*yr<=0.00000000000001) return 100;
  return -xr*yr/sqrt(xr*xr+yr*yr); 
}


//_____________________________________________________________________________
Double_t AliTPCtrackerMI::f2(Double_t x1,Double_t y1,
                   Double_t x2,Double_t y2,
                   Double_t x3,Double_t y3) 
{
  //-----------------------------------------------------------------
  // Initial approximation of the track curvature times center of curvature
  //-----------------------------------------------------------------
  Double_t d=(x2-x1)*(y3-y2)-(x3-x2)*(y2-y1);
  Double_t a=0.5*((y3-y2)*(y2*y2-y1*y1+x2*x2-x1*x1)-
                  (y2-y1)*(y3*y3-y2*y2+x3*x3-x2*x2));
  Double_t b=0.5*((x2-x1)*(y3*y3-y2*y2+x3*x3-x2*x2)-
                  (x3-x2)*(y2*y2-y1*y1+x2*x2-x1*x1));

  Double_t xr=TMath::Abs(d/(d*x1-a)), yr=d/(d*y1-b);
  
  return -a/(d*y1-b)*xr/sqrt(xr*xr+yr*yr);
}

//_____________________________________________________________________________
Double_t AliTPCtrackerMI::f3(Double_t x1,Double_t y1, 
                   Double_t x2,Double_t y2,
                   Double_t z1,Double_t z2) 
{
  //-----------------------------------------------------------------
  // Initial approximation of the tangent of the track dip angle
  //-----------------------------------------------------------------
  return (z1 - z2)/sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

Int_t  AliTPCtrackerMI::LoadClusters()
{
  //
  // load clusters to the memory
  AliTPCClustersRow *clrow= new AliTPCClustersRow;
  clrow->SetClass("AliTPCclusterMI");
  clrow->SetArray(0);
  clrow->GetArray()->ExpandCreateFast(10000);
  //
  TTree * tree = fClustersArray.GetTree();
  TBranch * br = tree->GetBranch("Segment");
  br->SetAddress(&clrow);
  //
  Int_t j=Int_t(fClustersArray.GetTree()->GetEntries());
  for (Int_t i=0; i<j; i++) {
    br->GetEntry(i);
    //  
    Int_t sec,row;
    fParam->AdjustSectorRow(clrow->GetID(),sec,row);
    //
    AliTPCRow * tpcrow=0;
    Int_t left=0;
    if (sec<fkNIS*2){
      tpcrow = &(fInnerSec[sec%fkNIS][row]);    
      left = sec/fkNIS;
    }
    else{
      tpcrow = &(fOuterSec[(sec-fkNIS*2)%fkNOS][row]);
      left = (sec-fkNIS*2)/fkNOS;
    }
    if (left ==0){
      tpcrow->fN1 = clrow->GetArray()->GetEntriesFast();
      tpcrow->fClusters1 = new AliTPCclusterMI[tpcrow->fN1];
      for (Int_t i=0;i<tpcrow->fN1;i++) 
	tpcrow->fClusters1[i] = *(AliTPCclusterMI*)(clrow->GetArray()->At(i));
    }
    if (left ==1){
      tpcrow->fN2 = clrow->GetArray()->GetEntriesFast();
      tpcrow->fClusters2 = new AliTPCclusterMI[tpcrow->fN2];
      for (Int_t i=0;i<tpcrow->fN2;i++) 
	tpcrow->fClusters2[i] = *(AliTPCclusterMI*)(clrow->GetArray()->At(i));
    }
  }
  //
  delete clrow;
  return 0;
}


void AliTPCtrackerMI::UnloadClusters()
{
  //
  // unload clusters from the memory
  //
  Int_t nrows = fOuterSec->GetNRows();
  for (Int_t sec = 0;sec<fkNOS;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCRow*  tpcrow = &(fOuterSec[sec%fkNOS][row]);
      if (tpcrow){
	if (tpcrow->fClusters1) delete []tpcrow->fClusters1; 
	if (tpcrow->fClusters2) delete []tpcrow->fClusters2; 
      }
    }
  //
  nrows = fInnerSec->GetNRows();
  for (Int_t sec = 0;sec<fkNIS;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCRow*  tpcrow = &(fInnerSec[sec%fkNIS][row]);
      if (tpcrow){
	if (tpcrow->fClusters1) delete []tpcrow->fClusters1; 
	if (tpcrow->fClusters2) delete []tpcrow->fClusters2; 
      }
    }

  return ;
}


//_____________________________________________________________________________
Int_t AliTPCtrackerMI::LoadOuterSectors() {
  //-----------------------------------------------------------------
  // This function fills outer TPC sectors with clusters.
  //-----------------------------------------------------------------
  Int_t nrows = fOuterSec->GetNRows();
  UInt_t index=0;
  for (Int_t sec = 0;sec<fkNOS;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCRow*  tpcrow = &(fOuterSec[sec%fkNOS][row]);  
      Int_t sec2 = sec+2*fkNIS;
      //left
      Int_t ncl = tpcrow->fN1;
      while (ncl--) {
	AliTPCclusterMI *c= &(tpcrow->fClusters1[ncl]);
	index=(((sec2<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //right
      ncl = tpcrow->fN2;
      while (ncl--) {
	AliTPCclusterMI *c= &(tpcrow->fClusters2[ncl]);
	index=((((sec2+fkNOS)<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
    }  
  fN=fkNOS;
  fSectors=fOuterSec;
  return 0;
}


//_____________________________________________________________________________
Int_t  AliTPCtrackerMI::LoadInnerSectors() {
  //-----------------------------------------------------------------
  // This function fills inner TPC sectors with clusters.
  //-----------------------------------------------------------------
  Int_t nrows = fInnerSec->GetNRows();
  UInt_t index=0;
  for (Int_t sec = 0;sec<fkNIS;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCRow*  tpcrow = &(fInnerSec[sec%fkNIS][row]);
      //
      //left
      Int_t ncl = tpcrow->fN1;
      while (ncl--) {
	AliTPCclusterMI *c= &(tpcrow->fClusters1[ncl]);
	index=(((sec<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //right
      ncl = tpcrow->fN2;
      while (ncl--) {
	AliTPCclusterMI *c= &(tpcrow->fClusters2[ncl]);
	index=((((sec+fkNIS)<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
    }  
  
  fN=fkNIS;
  fSectors=fInnerSec;
  return 0;
}


//_________________________________________________________________________
AliTPCclusterMI *AliTPCtrackerMI::GetClusterMI2(Int_t index) const {
  //--------------------------------------------------------------------
  //       Return pointer to a given cluster
  //--------------------------------------------------------------------
  Int_t sec=(index&0xff000000)>>24; 
  Int_t row=(index&0x00ff0000)>>16; 
  Int_t ncl=(index&0x0000ffff)>>00;

  AliTPCClustersRow *clrow=((AliTPCtrackerMI *) this)->fClustersArray.GetRow(sec,row);
  if (!clrow) return 0;
  if (clrow->GetArray()->GetEntriesFast()<=ncl) return 0;
  //  AliTPCclusterMI * cl = (AliTPCclusterMI*)(*clrow)[ncl];
  //AliTPCclusterMI * cl2 = GetClusterMI2(index);

  return (AliTPCclusterMI*)(*clrow)[ncl];      
}


//_________________________________________________________________________
AliTPCclusterMI *AliTPCtrackerMI::GetClusterMI(Int_t index) const {
  //--------------------------------------------------------------------
  //       Return pointer to a given cluster
  //--------------------------------------------------------------------
  Int_t sec=(index&0xff000000)>>24; 
  Int_t row=(index&0x00ff0000)>>16; 
  Int_t ncl=(index&0x0000ffff)>>00;

  const AliTPCRow * tpcrow=0;
  AliTPCclusterMI * clrow =0;
  if (sec<fkNIS*2){
    tpcrow = &(fInnerSec[sec%fkNIS][row]);
    if (sec<fkNIS) 
      clrow = tpcrow->fClusters1;
    else
      clrow = tpcrow->fClusters2;
  }
  else{
    tpcrow = &(fOuterSec[(sec-fkNIS*2)%fkNOS][row]);
    if (sec-2*fkNIS<fkNOS)
      clrow = tpcrow->fClusters1;
    else
      clrow = tpcrow->fClusters2;
  }
  if (tpcrow==0) return 0;
  if (tpcrow->GetN()<=ncl) return 0;
  //  return (AliTPCclusterMI*)(*tpcrow)[ncl];      
  return &(clrow[ncl]);      
  
}



Int_t AliTPCtrackerMI::FollowToNext(AliTPCseed& t, Int_t nr) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation to next pad row
  //-----------------------------------------------------------------
  //
  Double_t  x=fSectors->GetX(nr), ymax=fSectors->GetMaxY(nr);
  //  if (t.GetRadius()>x+10 ) return 0;
  if (!t.PropagateTo(x)) {
    t.fStopped = kTRUE;
    return 0;
  }
  //
  Double_t  y=t.GetY(), z=t.GetZ();
  if (y > ymax) {
    t.fRelativeSector= (t.fRelativeSector+1) % fN;
    if (!t.Rotate(fSectors->GetAlpha())) 
      return 0;
  } else if (y <-ymax) {
    t.fRelativeSector= (t.fRelativeSector-1+fN) % fN;
    if (!t.Rotate(-fSectors->GetAlpha())) 
      return 0;
  }   
  //
  // update current shape info every 3 pad-row
  if ( (nr%3==0) || t.GetNumberOfClusters()<2){
    t.fCurrentSigmaY = GetSigmaY(&t);
    t.fCurrentSigmaZ = GetSigmaZ(&t);
  }
  //  
  AliTPCclusterMI *cl=0;
  UInt_t index=0;
  
  if (t.GetClusterIndex2(nr)>0){ 
    //
    //cl = GetClusterMI(t.GetClusterIndex2(nr));
    cl = t.fClusterPointer[nr];
    index = t.GetClusterIndex2(nr);
    t.fCurrentClusterIndex1 = index; 
  }

  const AliTPCRow &krow=fSectors[t.fRelativeSector][nr];
  if ( (t.GetSigmaY2()<0) || t.GetSigmaZ2()<0) return 0;
  Double_t  roady  =1.;
  Double_t  roadz = 1.;
  //
  if (TMath::Abs(TMath::Abs(y)-ymax)<krow.fDeadZone){
    t.fInDead = kTRUE;
    Int_t row = nr;
    if (fSectors==fOuterSec) row += fParam->GetNRowLow();
    t.SetClusterIndex2(row,-1); 
    return 0;
  } 
  else
    {
      if (TMath::Abs(z)<(1.05*x+10)) t.fNFoundable++;
      else
	return 0;
    }   
  //calculate 
  
  if ((cl==0)&&(krow)) {
    cl = krow.FindNearest2(y,z,roady,roadz,index);    
    t.fCurrentClusterIndex1 = krow.GetIndex(index);       
  }  


  if (cl) {
    t.fCurrentCluster = cl; 
    //t.fCurrentClusterIndex1 = krow.GetIndex(index);   
    t.fCurrentClusterIndex2 = index;   
    Double_t sy2=ErrY2(&t,t.fCurrentCluster);
    Double_t sz2=ErrZ2(&t,t.fCurrentCluster);

    Double_t sdistancey = TMath::Sqrt(sy2+t.GetSigmaY2());
    Double_t sdistancez = TMath::Sqrt(sz2+t.GetSigmaZ2());

    Double_t rdistancey = TMath::Abs(t.fCurrentCluster->GetY()-t.GetY());
    Double_t rdistancez = TMath::Abs(t.fCurrentCluster->GetZ()-t.GetZ());
    
    Double_t rdistance  = rdistancey/sdistancey+rdistancez/sdistancez;
    Bool_t accept =kTRUE;

    //    printf("\t%f\t%f\t%f\n",rdistancey/sdistancey,rdistancez/sdistancez,rdistance);
    if ( (rdistancey>1) || (rdistancez>1)) return 0;
    if (rdistance>6) return kFALSE;
        
    
    if ((rdistancey/sdistancey>3 || rdistancez/sdistancez>3) && t.fCurrentCluster->GetType()==0)  
	accept = kFALSE;  //suspisiouce - will be changed

    if ((rdistancey/sdistancey>2.5 || rdistancez/sdistancez>2.5) && t.fCurrentCluster->GetType()>0)  
	// strict cut on overlaped cluster
	accept = kFALSE;  //suspisiouce - will be changed

    if ( (rdistancey/sdistancey>1 || rdistancez/sdistancez>2.5 ) 
	 && t.fCurrentCluster->GetType()<0){
      t.fNFoundable--;
      accept = kFALSE ;
    }
    if (t.fCurrentCluster->IsUsed()){
      //
      //
      t.fErrorZ2*=2;
      t.fErrorY2*=2;
      t.fNShared++;
    }
    UpdateTrack(&t,accept);

  } else {    
    
  }
  return 1;
}

Int_t AliTPCtrackerMI::FollowToNext(AliTPCseed& t, Int_t nr, Int_t step) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation to next pad row
  //-----------------------------------------------------------------
  //
  Int_t middle = nr+step/2;
  Double_t  x=fSectors->GetX(middle), ymax=fSectors->GetMaxY(middle);

  AliTPCclusterMI * clusters[200]; //pointers to clusters
  Double_t ty[200],tz[200];          //track local position
  UInt_t cindex[200];                //indexes of the clusters
  if (!t.PropagateTo(x)) {
    t.fStopped = kTRUE;
    return 0;
  }
  //
  // update current information about cluster estimates 
  if ( (nr%3==0) || t.GetNumberOfClusters()<2){
    t.fCurrentSigmaY = GetSigmaY(&t);
    t.fCurrentSigmaZ = GetSigmaZ(&t);
  }
  Double_t sy2=ErrY2(&t)*2;
  Double_t sz2=ErrZ2(&t)*2;
  if ( (t.GetSigmaY2()<0) || t.GetSigmaZ2()<0) return 0;
  
  Double_t  roady  =3.*sqrt(t.GetSigmaY2() + sy2);
  Double_t  roadz = 3.*sqrt(t.GetSigmaZ2() + sz2);
  Double_t  y=t.GetY(), z=t.GetZ();
  
  //
  //  
  const AliTPCRow &krow=fSectors[t.fRelativeSector][middle];
  
  if (TMath::Abs(TMath::Abs(y)-ymax)<krow.fDeadZone){
    t.fInDead = kTRUE;
    Int_t row = nr;
    if (fSectors==fOuterSec) row += fParam->GetNRowLow();
    t.SetClusterIndex2(row,-1); 
    return 0;
  } 
  else
    {
      if (TMath::Abs(z)<(1.05*x+10)) t.fNFoundable+=step;
      else
	return 0;
    }   



  //
  // find prolongations  
  for (Int_t i=0;i<step;i++){
    Double_t x = fSectors->GetX(nr+i);
    t.GetProlongation(x,ty[i],tz[i]);   // get local track position
  }
  //rotate if necessary
  if (ty[step-1] > ymax || ty[0] > ymax) {
    t.fRelativeSector= (t.fRelativeSector+1) % fN;
    if (!t.Rotate(fSectors->GetAlpha())) 
      return 1;
  } else if (ty[step-1] <-ymax || ty[0] <-ymax) {
    t.fRelativeSector= (t.fRelativeSector-1+fN) % fN;
    if (!t.Rotate(-fSectors->GetAlpha())) 
      return 1;
  }   
  //find nerest clusters
  for (Int_t i=0;i<step;i++){
    const AliTPCRow &row=fSectors[t.fRelativeSector][nr+i];
    clusters[i] = row.FindNearest2(ty[i],tz[i],roady,roadz,cindex[i]);
  }

  /*  

  if (cl) {
    t.fCurrentCluster = cl; 
    t.fCurrentClusterIndex1 = krow.GetIndex(index);   
    t.fCurrentClusterIndex2 = index;   
    Double_t sy2=ErrY2(&t,t.fCurrentCluster);
    Double_t sz2=ErrZ2(&t,t.fCurrentCluster);

    Double_t sdistancey = TMath::Sqrt(sy2+t.GetSigmaY2());
    Double_t sdistancez = TMath::Sqrt(sz2+t.GetSigmaZ2());

    Double_t rdistancey = TMath::Abs(t.fCurrentCluster->GetY()-t.GetY());
    Double_t rdistancez = TMath::Abs(t.fCurrentCluster->GetZ()-t.GetZ());
    
    Double_t rdistance  = TMath::Sqrt(TMath::Power(rdistancey/sdistancey,2)+TMath::Power(rdistancez/sdistancez,2));

    //    printf("\t%f\t%f\t%f\n",rdistancey/sdistancey,rdistancez/sdistancez,rdistance);
    if ( (rdistancey>1) || (rdistancez>1)) return 0;

    if (rdistance>3.5) return 0;
        
    if ((rdistancey/sdistancey>3 || rdistancez/sdistancez>3) && t.fCurrentCluster->GetType()==0)  
	return 0;  //suspisiouce - will be changed

    if ((rdistancey/sdistancey>2.5 || rdistancez/sdistancez>2.5) && t.fCurrentCluster->GetType()>0)  
	// strict cut on overlaped cluster
	return 0;  //suspisiouce - will be changed

    if ( (rdistancey/sdistancey>1 || rdistancez/sdistancez>2.5 ) 
	 && t.fCurrentCluster->GetType()<0){
      t.fNFoundable--;
      return 0;
    }
    UpdateTrack(&t);

  } 

 */
  

  return 1;
}


Int_t AliTPCtrackerMI::UpdateClusters(AliTPCseed& t,  Int_t nr) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation to next pad row
  //-----------------------------------------------------------------
  t.fCurrentCluster  = 0;
  t.fCurrentClusterIndex1 = 0;   
  t.fCurrentClusterIndex2 = 0;
   
  Double_t xt=t.GetX();
  Int_t row = fSectors->GetRowNumber(xt)-1; 
  Double_t ymax=fSectors->GetMaxY(nr);

  if (row < nr) return 1; // don't prolongate if not information until now -

  Double_t x=fSectors->GetX(nr);
  if (!t.PropagateTo(x)){
    t.fStopped =kTRUE;
    return 0;
  }
  //
  Double_t  y=t.GetY(), z=t.GetZ();
  if (y > ymax) {
    t.fRelativeSector= (t.fRelativeSector+1) % fN;
    if (!t.Rotate(fSectors->GetAlpha())) 
      return 0;
  } else if (y <-ymax) {
    t.fRelativeSector= (t.fRelativeSector-1+fN) % fN;
    if (!t.Rotate(-fSectors->GetAlpha())) 
      return 0;
  }
  //

  AliTPCRow &krow=fSectors[t.fRelativeSector][nr];
  if (TMath::Abs(TMath::Abs(y)-ymax)<krow.fDeadZone){
    t.fInDead = kTRUE;
    Int_t row = nr;
    if (fSectors==fOuterSec) row += fParam->GetNRowLow();
    t.SetClusterIndex2(row,-1); 
    return 0;
  } 
  else
    {
      if (TMath::Abs(t.GetZ())<(1.05*t.GetX()+10)) t.fNFoundable++;
      else
	return 0;      
    }

  // update current
  if ( (nr%3==0) || t.GetNumberOfClusters()<2){
    t.fCurrentSigmaY = GetSigmaY(&t);
    t.fCurrentSigmaZ = GetSigmaZ(&t);
  }
    
  AliTPCclusterMI *cl=0;
  UInt_t index=0;
  //
  Double_t roady = 1.;
  Double_t roadz = 1.;
  //
  if (krow) {    
    cl = krow.FindNearest2(y,z,roady,roadz,index);      
  }
  t.fCurrentCluster  = cl;
  t.fCurrentClusterIndex1 = krow.GetIndex(index);   
  t.fCurrentClusterIndex2 = index;   
  return 1;
}


Int_t AliTPCtrackerMI::FollowToNextCluster(AliTPCseed & t, Int_t nr) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation to next pad row
  //-----------------------------------------------------------------

  //update error according neighborhoud
  Float_t meanz =0;
  Float_t meany =0;
  Int_t accept=0;
  for (Int_t di=1;di<4;di++){
    AliTPCclusterMI * cl = t.fClusterPointer[t.fRow];
    if (cl) accept++;
    AliTPCTrackPoint * point = t.GetTrackPoint(di);
    meany+= point->GetTPoint().GetSigmaY();
    meanz+= point->GetTPoint().GetSigmaZ();
  }
  if (accept>0){
    meany/=accept;
    meanz/=accept;    
  }
  Float_t corz=TMath::Max(1.,meanz);
  Float_t cory=TMath::Max(1.,meany);
  //  


  if (t.fCurrentCluster) {
    Double_t sy2=ErrY2(&t,t.fCurrentCluster);
    Double_t sz2=ErrZ2(&t,t.fCurrentCluster);
    sy2*=cory;
    sz2*=corz;
    //
    Double_t rdistancey = TMath::Abs(t.fCurrentCluster->GetY()-t.GetY());
    Double_t rdistancez = TMath::Abs(t.fCurrentCluster->GetZ()-t.GetZ());
    if ( (rdistancey>1) || (rdistancez>1)) return 0;

    Double_t sdistancey = TMath::Sqrt(sy2+t.GetSigmaY2());
    Double_t sdistancez = TMath::Sqrt(sz2+t.GetSigmaZ2());    
    Double_t rdistance  = rdistancey/sdistancey+rdistancez/sdistancez;
    //    printf("\t%f\t%f\t%f\n",rdistancey/sdistancey,rdistancez/sdistancez,rdistance);
    Bool_t accept = kTRUE;
    
    if (rdistance>5) return kFALSE;
    
    if ((rdistancey/sdistancey>3 || rdistancez/sdistancez>3) && t.fCurrentCluster->GetType()==0)  
	accept = kFALSE;  //suspisiouce - will be changed

    if ((rdistancey/sdistancey>2.5 || rdistancez/sdistancez>2.5) && t.fCurrentCluster->GetType()>0)  
	// strict cut on overlaped cluster
	accept = kFALSE;  //suspisiouce - will be changed

    if ( (rdistancey/sdistancey>2 || rdistancez/sdistancez>2.5 ||t.fCurrentCluster->GetQ()<70 ) 
	 && t.fCurrentCluster->GetType()<0){
      t.fNFoundable--;    
      accept = kFALSE;
    }
    if (t.fCurrentCluster->IsUsed()){
      //
      //
      t.fErrorZ2*=2;
      t.fErrorY2*=2;
      t.fNShared++;
    }
    UpdateTrack(&t,accept);
   
  } else {
  }
  return 1;
}



//_____________________________________________________________________________
Int_t AliTPCtrackerMI::FollowProlongation(AliTPCseed& t, Int_t rf, Int_t step) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation.
  //-----------------------------------------------------------------
  Double_t xt=t.GetX();
  //
  Double_t alpha=t.GetAlpha() - fSectors->GetAlphaShift();
  if (alpha > 2.*TMath::Pi()) alpha -= 2.*TMath::Pi();  
  if (alpha < 0.            ) alpha += 2.*TMath::Pi();  
  t.fRelativeSector = Int_t(alpha/fSectors->GetAlpha())%fN;
    
  for (Int_t nr=fSectors->GetRowNumber(xt)-1; nr>=rf; nr-=step) {
   
    if (FollowToNext(t,nr)==0) {
    }
  }   
  return 1;
}



Int_t AliTPCtrackerMI::FollowBackProlongation(AliTPCseed& t, Int_t rf) {
  //-----------------------------------------------------------------
  // This function tries to find a track prolongation.
  //-----------------------------------------------------------------
  //  Double_t xt=t.GetX();  
  //
  Double_t alpha=t.GetAlpha() - fSectors->GetAlphaShift();
  if (alpha > 2.*TMath::Pi()) alpha -= 2.*TMath::Pi();  
  if (alpha < 0.            ) alpha += 2.*TMath::Pi();  
  t.fRelativeSector = Int_t(alpha/fSectors->GetAlpha())%fN;
  
  Int_t first = 0;
  //
  if (fSectors == fOuterSec){
    first = t.fFirstPoint-fInnerSec->GetNRows();
  }
  else
    first = t.fFirstPoint;
  //
  if (first<0) first=0;
  for (Int_t nr=first+1; nr<=rf; nr++) {
    if ( (t.GetSnp()<0.9))
      FollowToNext(t,nr);                                                       
      
  }   
  return 1;
}




   
Float_t AliTPCtrackerMI::OverlapFactor(AliTPCseed * s1, AliTPCseed * s2, Int_t &sum1, Int_t & sum2)
{
  //
  //
  sum1=0;
  sum2=0;
  Int_t sum=0;
  //
  Float_t dz2 =(s1->GetZ() - s2->GetZ());
  dz2*=dz2;  

  Float_t dy2 =TMath::Abs((s1->GetY() - s2->GetY()));
  dy2*=dy2;
  Float_t distance = TMath::Sqrt(dz2+dy2);
  if (distance>4.) return 0; // if there are far away  - not overlap - to reduce combinatorics
 
  //  Int_t offset =0;
  //if (fSectors==fOuterSec) offset = fParam->GetNRowLow();
  Int_t firstpoint = TMath::Min(s1->fFirstPoint,s2->fFirstPoint);
  Int_t lastpoint = TMath::Max(s1->fLastPoint,s2->fLastPoint);
  //Int_t firstpoint = TMath::Max(s1->fFirstPoint,s2->fFirstPoint);
  //Int_t lastpoint = TMath::Min(s1->fLastPoint,s2->fLastPoint);
  //lastpoint +=offset;
  //firstpoint+=offset;
  if (lastpoint>160) 
    lastpoint =160;
  if (firstpoint<0) 
    firstpoint = 0;
  if (firstpoint>lastpoint) {
    firstpoint =lastpoint;
    //    lastpoint  =160;
  }
    
  
  for (Int_t i=firstpoint-1;i<lastpoint+1;i++){
    if (s1->GetClusterIndex2(i)>0) sum1++;
    if (s2->GetClusterIndex2(i)>0) sum2++;
    if (s1->GetClusterIndex2(i)==s2->GetClusterIndex2(i) && s1->GetClusterIndex2(i)>0) {
      sum++;
    }
  }
  if (sum<5) return 0;

  Float_t summin = TMath::Min(sum1+1,sum2+1);
  Float_t ratio = (sum+1)/Float_t(summin);
  return ratio;
}

void  AliTPCtrackerMI::SignShared(AliTPCseed * s1, AliTPCseed * s2)
{
  //
  //
  if (s1->fSector!=s2->fSector) return;
  //
  Float_t dz2 =(s1->GetZ() - s2->GetZ());
  dz2*=dz2;
  Float_t dy2 =(s1->GetY() - s2->GetY());

  dy2*=dy2;
  Float_t distance = TMath::Sqrt(dz2+dy2);
  if (distance>15.) return ; // if there are far away  - not overlap - to reduce combinatorics
  //trpoint = new (pointarray[track->fRow]) AliTPCTrackPoint;
  //  TClonesArray &pointarray1 = *(s1->fPoints);
  //TClonesArray &pointarray2 = *(s2->fPoints);
  //
  for (Int_t i=0;i<160;i++){
    if (s1->GetClusterIndex2(i)==s2->GetClusterIndex2(i) && s1->GetClusterIndex2(i)>0) {
      //  AliTPCTrackPoint *p1  = (AliTPCTrackPoint *)(pointarray1.UncheckedAt(i));
      //AliTPCTrackPoint *p2  = (AliTPCTrackPoint *)(pointarray2.UncheckedAt(i)); 
      AliTPCTrackPoint *p1  = s1->GetTrackPoint(i);
      AliTPCTrackPoint *p2  = s2->GetTrackPoint(i);; 
      p1->fIsShared = kTRUE;
      p2->fIsShared = kTRUE;
    }
  } 
}




void  AliTPCtrackerMI::RemoveOverlap(TObjArray * arr, Float_t factor, Int_t removalindex , Bool_t shared){

  

  //
  // remove overlap - used removal factor - removal index stored in the track
  for (Int_t i=0; i<arr->GetEntriesFast(); i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    //if (pt) RotateToLocal(pt);
    pt->fSort = 0;
  }
  arr->UnSort();
  arr->Sort();  // sorting according z
  arr->Expand(arr->GetEntries());
  Int_t nseed=arr->GetEntriesFast();
  //  printf("seeds \t%p \t%d\n",arr, nseed);
  //  arr->Expand(arr->GetEntries());  //remove 0 pointers
  nseed = arr->GetEntriesFast();
  Int_t removed = 0;
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    if (!(pt->IsActive())) continue;
    for (Int_t j=i+1; j<nseed; j++){
      AliTPCseed *pt2=(AliTPCseed*)arr->UncheckedAt(j);
      //
      if (!pt2) continue; 
      if (! ((pt2->IsActive()) || (pt2->fRemoval==10))) continue;
      if (TMath::Abs(pt->fRelativeSector-pt2->fRelativeSector)>0) break;
      if (TMath::Abs(pt2->GetZ()-pt->GetZ())<4){

	Int_t sum1,sum2;
	Float_t ratio = OverlapFactor(pt,pt2,sum1,sum2);
	//if (sum1==0) {
	//  pt->Desactivate(removalindex); // arr->RemoveAt(i); 
	//  break;
	//}
	if (ratio>factor){
	  if (pt->fBConstrain) sum1+=10;
	  if (pt2->fBConstrain) sum2+=10;
	  //	  if (pt->GetChi2()<pt2->GetChi2()) pt2->Desactivate(removalindex);  // arr->RemoveAt(j);	
	  if (pt2->fRemoval==10) {
	    pt2->Desactivate(removalindex);
	    if (removalindex<0) delete arr->RemoveAt(j);
	    //if (removalindex<0) DeleteSeed(arr->RemoveAt(j));
	    continue;
	  }
	  Float_t ratio2 = (pt->GetChi2()*pt2->GetNumberOfClusters())/
	    (pt2->GetChi2()*pt->GetNumberOfClusters());
	  Float_t ratio3 = Float_t(sum1+pt->GetNumberOfClusters()-sum2-pt2->GetNumberOfClusters())/
	    Float_t(sum1+sum2+pt->GetNumberOfClusters()+pt2->GetNumberOfClusters());
	  removed++;
	  if (TMath::Abs(ratio3)>0.025){  // if much more points  
	    if (sum1>sum2) pt2->Desactivate(removalindex);
	    else {
	      pt->Desactivate(removalindex); // arr->RemoveAt(i); 
	      if (removalindex<0) delete arr->RemoveAt(i);
	      //if (removalindex<0) DeleteSeed(arr->RemoveAt(i));
	      break;
	    }
	  }
	  else{
	    /*  decide on error
	      Float_t sigma1 = TMath::Sqrt(pt->GetSigmaY2() +pt->GetSigmaZ2());
	      Float_t sigma2 = TMath::Sqrt(pt2->GetSigmaY2()+pt2->GetSigmaZ2());
	      if (sigma1<sigma2)  
	      pt2->Desactivate(removalindex);
	      else {
	      pt->Desactivate(removalindex); // arr->RemoveAt(i); 
	      break;
	      }
	    */
	     
	    //decide on mean chi2
	    if (ratio2<1) {
	      pt2->Desactivate(removalindex);
	      if (removalindex<0) delete arr->RemoveAt(j);
	      //if (removalindex<0) DeleteSeed(arr->RemoveAt(j));

	    }
	    else {
	      pt->Desactivate(removalindex); // arr->RemoveAt(i); 
	      if (removalindex<0) delete arr->RemoveAt(i);
	      //if (removalindex<0) DeleteSeed(arr->RemoveAt(i));
	      break;
	    }	    	    
	  }  	  
	}  // if suspicious ratio
      } 
      else
	break;
    }
  }
  //  printf("removed\t%d\n",removed);
  Int_t good =0; 
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) continue;
    if ((pt->GetNumberOfClusters() < pt->fNFoundable*0.5) && pt->IsActive()) {
      //desactivate tracks with small number of points
      //      printf("%d\t%d\t%f\n", pt->GetNumberOfClusters(), pt->fNFoundable,pt->GetNumberOfClusters()/Float_t(pt->fNFoundable));
      pt->Desactivate(10);  //desactivate  - small muber of points
    }
    if ((pt->IsActive())){
      good++;
      continue;
    }
  }
  
  
  if (shared)
    for (Int_t i=0; i<nseed; i++) {
      AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
      if (!pt) continue;
      if (!(pt->IsActive())) continue;
      for (Int_t j=i+1; j<nseed; j++){
	AliTPCseed *pt2=(AliTPCseed*)arr->UncheckedAt(j);
	if ((pt2) && pt2->IsActive()) {
	  if ( TMath::Abs(pt->fRelativeSector-pt2->fRelativeSector)>1) break;
	  SignShared(pt,pt2);
	}
      }
    }
  fNtracks = good;
  printf("\n*****\nNumber of good tracks after overlap removal\t%d\n",fNtracks);
  //  CompressSeed(500);

}

void AliTPCtrackerMI::RemoveUsed(TObjArray * arr, Float_t factor1,  Float_t factor2, Int_t removalindex)
{

  //Loop over all tracks and remove "overlaps"
  //
  //
  Int_t nseed = arr->GetEntriesFast();  
  Int_t good =0;

  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    pt->fSort =1;
  }
  arr->UnSort();
  arr->Sort();
  //
  //unsign used
  
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    
    //    Int_t noc=pt->GetNumberOfClusters();
    //    Int_t shared =0;
    BuildDensity(pt);
    for (Int_t i=0; i<160; i++) {
      Int_t index=pt->GetClusterIndex2(i);
      if (index<0 || index&0x8000) continue;
      //      AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index); 
      AliTPCclusterMI *c= pt->fClusterPointer[i]; 
      if (!c) continue;
      if (c->IsUsed()) c->Use();
    }
  }
  //
  // loop over "primaries"
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    
    //if (!(pt->IsActive())) continue;
    if (!(pt->fBConstrain)) continue;
    Int_t noc=pt->GetNumberOfClusters();
    Int_t shared =0;
    BuildDensity(pt);
    for (Int_t i=0; i<160; i++) {
      Int_t index=pt->GetClusterIndex2(i);
      if (index<0 || index&0x8000) continue;
      //      AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index); 
      AliTPCclusterMI *c= pt->fClusterPointer[i]; 
      if (!c) continue;
      if (c->IsUsed()) shared++;
    }
    if ((Float_t(shared)/Float_t(noc))>factor1)
      pt->Desactivate(removalindex);
    else{
      good++;
      for (Int_t i=0; i<160; i++) {
	Int_t index=pt->GetClusterIndex2(i);
	if (index<0 || index&0x8000 ) continue;
	//	AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index);  
	AliTPCclusterMI *c= pt->fClusterPointer[i];  

	if (!c) continue;
	if (!c->IsUsed()) c->Use();  
      }
    }
  }
  fNtracks = good;

  // loop over "secondaries"
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    //if (!(pt->IsActive())) continue;
    if (pt->fBConstrain) continue;

    Int_t noc=pt->GetNumberOfClusters();
    Int_t shared =0;
    BuildDensity(pt);
    for (Int_t i=0; i<160; i++) {
      Int_t index=pt->GetClusterIndex2(i);
      if (index<0 || index&0x8000) continue;
      //      AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index);
      AliTPCclusterMI *c= pt->fClusterPointer[i];
      if (!c) continue;
      if (c->IsUsed()) shared++;
    }
    if ((Float_t(shared)/Float_t(noc))>factor2)
      pt->Desactivate(removalindex);
    else{
      good++;
      for (Int_t i=0; i<160; i++) {
	//Int_t index=pt->GetClusterIndex2(i);
	//	AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index);  
	AliTPCclusterMI *c= pt->fClusterPointer[i];
	if (!c) continue;
	if (!c->IsUsed()) c->Use();  
      }
    }
  }
  fNtracks = good;

  printf("\n*****\nNumber of good tracks after shared removal\t%d\n",fNtracks);
}

void AliTPCtrackerMI::SignClusters(TObjArray * arr, Float_t fraction)
{
  //
  //sign clusters to be "used"
  // loop over "primaries"
  static int gLastCheck =0;
  Float_t sumdens=0;
  Float_t sumdens2=0;
  Float_t sumn   =0;
  Float_t sumn2  =0;

  Float_t sum    =0;
  Int_t nseed = arr->GetEntriesFast();
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    //if (!pt) {
    //  continue;
    //}    
    //    if (!(pt->IsActive())) continue;
    //    if (!(pt->fBConstrain)) continue;
    Float_t dens = pt->GetNumberOfClusters()/Float_t(pt->fNFoundable);
    if (dens>0.7){
      sumdens += dens;
      sumdens2+= dens*dens;
      sumn    += pt->GetNumberOfClusters();
      sumn2   += pt->GetNumberOfClusters()*pt->GetNumberOfClusters();
      sum++;
    }
  }

  Float_t mdensity = 0.9;
  Float_t meann    = 130;
  Float_t sdensity = 0.1;
  Float_t smeann    = 10;

  if (sum>20){
    mdensity = sumdens/sum;
    meann    = sumn/sum;
    sdensity = sumdens2/sum-mdensity*mdensity;
    sdensity = TMath::Sqrt(sdensity);
    //
    smeann   = sumn2/sum-meann*meann;
    smeann   = TMath::Sqrt(smeann);
  }

  //
  for (Int_t i=gLastCheck; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }    
    if (!(pt->IsActive())) continue;
    //    if (!(pt->fBConstrain)) continue;
    Float_t dens = pt->GetNumberOfClusters()/Float_t(pt->fNFoundable);
    if (dens>(mdensity-4.*sdensity) && pt->GetNumberOfClusters()>(meann-4.*smeann)){
      //Int_t noc=pt->GetNumberOfClusters();
      for (Int_t i=0; i<160; i++) {

	Int_t index=pt->GetClusterIndex2(i);
	if (index<0) continue;
	//AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index);  	
	AliTPCclusterMI *c= pt->fClusterPointer[i];
	if (!c) continue;
	if (!(c->IsUsed())) c->Use();  
      }
    }
  }
  gLastCheck = nseed;
}


void  AliTPCtrackerMI::StopNotActive(TObjArray * arr, Int_t row0, Float_t th1, Float_t th2)
{
  // stop not active tracks
  // take th1 as threshold for number of founded to number of foundable on last 10 active rows
  // take th2 as threshold for number of founded to number of foundable on last 20 active rows 
  Int_t nseed = arr->GetEntriesFast();  
  //
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i);    
    if (!pt) {
      continue;
    }
    if (!(pt->IsActive())) continue;
    StopNotActive(pt,row0,th1,th2);
  }
}



void  AliTPCtrackerMI::StopNotActive(AliTPCseed * seed, Int_t row0, Float_t th1,
 Float_t th2)
{
  // stop not active tracks
  // take th1 as threshold for number of founded to number of foundable on last 10 active rows
  // take th2 as threshold for number of founded to number of foundable on last 20 active rows 
  Int_t sumgood1  = 0;
  Int_t sumgood2  = 0;
  Int_t foundable = 0;
  Int_t maxindex = seed->fLastPoint;  //last foundable row
  
  for (Int_t i=row0; i++;i<maxindex){
    Int_t index = seed->GetClusterIndex2(i);
    if (index!=-1) foundable++;
    //AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index); 
    //if (!c) continue;
    if (foundable<=30) sumgood1++;
    if (foundable<=50) {
      sumgood2++;
    }
    else{ 
      break;
    }        
  }
  if (foundable>=30.){ 
     if (sumgood1<(th1*30.)) seed->Desactivate(10);
  }
  if (foundable>=50)
    if (sumgood2<(th2*50.)) seed->Desactivate(10);
}


//_____________________________________________________________________________
void AliTPCtrackerMI::MakeSeeds(TObjArray * arr, Int_t sec, Int_t i1, Int_t i2) {
  //-----------------------------------------------------------------
  // This function creates track seeds.
  //-----------------------------------------------------------------
  //  if (fSeeds==0) fSeeds=new TObjArray(15000);

  Double_t x[5], c[15];

  Double_t alpha=fOuterSec->GetAlpha(), shift=fOuterSec->GetAlphaShift();
  Double_t cs=cos(alpha), sn=sin(alpha);

  Double_t x1 =fOuterSec->GetX(i1);
  Double_t xx2=fOuterSec->GetX(i2);
  AliTPCseed * seed = new AliTPCseed;
  //
  //  for (Int_t ns=0; ns<fkNOS; ns++) 
  Int_t ns =sec;
    {
    Int_t nl=fOuterSec[(ns-1+fkNOS)%fkNOS][i2];
    Int_t nm=fOuterSec[ns][i2];
    Int_t nu=fOuterSec[(ns+1)%fkNOS][i2];
    const AliTPCRow& kr1=fOuterSec[ns][i1];
    AliTPCRow&  kr21 = fOuterSec[(ns-1+fkNOS)%fkNOS][i2];
    AliTPCRow&  kr22 = fOuterSec[(ns)%fkNOS][i2];
    AliTPCRow&  kr23 = fOuterSec[(ns+1)%fkNOS][i2];

    for (Int_t is=0; is < kr1; is++) {
      Double_t y1=kr1[is]->GetY(), z1=kr1[is]->GetZ();
      Double_t x3=GetX(), y3=GetY(), z3=GetZ();

      Float_t anglez = (z1-z3)/(x1-x3); 
      Float_t extraz = z1 - anglez*(x1-xx2);  // extrapolated z

      for (Int_t js=0; js < nl+nm+nu; js++) {
	const AliTPCclusterMI *kcl;
        Double_t x2,   y2,   z2;
	if (js<nl) {	 
	  if (js==0) {
	    js = kr21.Find(extraz-10.);
	    if (js>=nl) continue;
	  }	  
	  kcl=kr21[js];
	  z2=kcl->GetZ();
	  if ((extraz-z2)>10) continue;	  
	  if ((extraz-z2)<-10) {
	    js = nl-1;
	    continue;
	  }
          y2=kcl->GetY(); 
          x2= xx2*cs+y2*sn;
          y2=-xx2*sn+y2*cs;
	} else 
	  if (js<nl+nm) {
	    if (js==nl) {
	      js = nl+kr22.Find(extraz-10.);
	      if (js>=nl+nm) continue;
	    }	  	  
	    kcl=kr22[js-nl];
	    z2=kcl->GetZ();
	    if ((extraz-z2)>10) continue;
	    if ((extraz-z2)<-10) {
	      js = nl+nm-1;
	      continue;
	    }
            x2=xx2; y2=kcl->GetY(); 
	  } else {
	    //const AliTPCRow& kr2=fOuterSec[(ns+1)%fkNOS][i2];	  
	    if (js==nl+nm) {
	      js = nl+nm+kr23.Find(extraz-10.);
	      if (js>=nl+nm+nu) break;
	    }	  
	    kcl=kr23[js-nl-nm];
	    z2=kcl->GetZ(); 
	    if ((extraz-z2)>10) continue;
	    if ((extraz-z2)<-10) {
	      break;	    
	    }
            y2=kcl->GetY();
            x2=xx2*cs-y2*sn;
            y2=xx2*sn+y2*cs;
	  }

        Double_t zz=z1 - anglez*(x1-x2); 
        if (TMath::Abs(zz-z2)>10.) continue;

        Double_t d=(x2-x1)*(0.-y2)-(0.-x2)*(y2-y1);
        if (d==0.) {cerr<<"MakeSeeds warning: Straight seed !\n"; continue;}

	x[0]=y1;
	x[1]=z1;
	x[4]=f1(x1,y1,x2,y2,x3,y3);
	if (TMath::Abs(x[4]) >= 0.0066) continue;
	x[2]=f2(x1,y1,x2,y2,x3,y3);
	//if (TMath::Abs(x[4]*x1-x[2]) >= 0.99999) continue;
	x[3]=f3(x1,y1,x2,y2,z1,z2);
	if (TMath::Abs(x[3]) > 1.2) continue;
	Double_t a=asin(x[2]);
	Double_t zv=z1 - x[3]/x[4]*(a+asin(x[4]*x1-x[2]));
	if (TMath::Abs(zv-z3)>10.) continue; 

        Double_t sy1=kr1[is]->GetSigmaY2()*2, sz1=kr1[is]->GetSigmaZ2()*4;
        Double_t sy2=kcl->GetSigmaY2()*2,     sz2=kcl->GetSigmaZ2()*4;
	//Double_t sy3=400*3./12., sy=0.1, sz=0.1;
	Double_t sy3=25000*x[4]*x[4]+0.1, sy=0.1, sz=0.1;
	//Double_t sy3=25000*x[4]*x[4]*60+0.5, sy=0.1, sz=0.1;

	Double_t f40=(f1(x1,y1+sy,x2,y2,x3,y3)-x[4])/sy;
	Double_t f42=(f1(x1,y1,x2,y2+sy,x3,y3)-x[4])/sy;
	Double_t f43=(f1(x1,y1,x2,y2,x3,y3+sy)-x[4])/sy;
	Double_t f20=(f2(x1,y1+sy,x2,y2,x3,y3)-x[2])/sy;
	Double_t f22=(f2(x1,y1,x2,y2+sy,x3,y3)-x[2])/sy;
	Double_t f23=(f2(x1,y1,x2,y2,x3,y3+sy)-x[2])/sy;
	Double_t f30=(f3(x1,y1+sy,x2,y2,z1,z2)-x[3])/sy;
	Double_t f31=(f3(x1,y1,x2,y2,z1+sz,z2)-x[3])/sz;
	Double_t f32=(f3(x1,y1,x2,y2+sy,z1,z2)-x[3])/sy;
	Double_t f34=(f3(x1,y1,x2,y2,z1,z2+sz)-x[3])/sz;

        c[0]=sy1;
        c[1]=0.;       c[2]=sz1;
        c[3]=f20*sy1;  c[4]=0.;       c[5]=f20*sy1*f20+f22*sy2*f22+f23*sy3*f23;
        c[6]=f30*sy1;  c[7]=f31*sz1;  c[8]=f30*sy1*f20+f32*sy2*f22;
                       c[9]=f30*sy1*f30+f31*sz1*f31+f32*sy2*f32+f34*sz2*f34;
        c[10]=f40*sy1; c[11]=0.; c[12]=f40*sy1*f20+f42*sy2*f22+f43*sy3*f23;
        c[13]=f30*sy1*f40+f32*sy2*f42;
        c[14]=f40*sy1*f40+f42*sy2*f42+f43*sy3*f43;

        UInt_t index=kr1.GetIndex(is);
	AliTPCseed *track=new(seed) AliTPCseed(index, x, c, x1, ns*alpha+shift);
	track->fIsSeeding = kTRUE;
	//Int_t rc=FollowProlongation(*track, i2);
	Int_t delta4 = Int_t((i2-i1)/4.);

	FollowProlongation(*track, i1-delta4);
	if (track->GetNumberOfClusters() < track->fNFoundable/2.) {
	  //delete track;
	  seed->~AliTPCseed();
	  continue;
	}
	FollowProlongation(*track, i1-2*delta4);
	if (track->GetNumberOfClusters() < track->fNFoundable/2.) {
	  //delete track;
	  seed->~AliTPCseed();
	  continue;
	}
	FollowProlongation(*track, i1-3*delta4);
	if (track->GetNumberOfClusters() < track->fNFoundable/2.) {
	  //	  delete track;
	  seed->~AliTPCseed();
	  continue;
	}
	FollowProlongation(*track, i2);
	//Int_t rc = 1;
	track->fBConstrain =1;
	track->fLastPoint = i1+fInnerSec->GetNRows();  // first cluster in track position
	if (track->GetNumberOfClusters()<(i1-i2)/4 || track->GetNumberOfClusters() < track->fNFoundable/2. ) 
	  //  seed = seed;
	  //delete track;
	  seed->~AliTPCseed();
        else {
	  arr->AddLast(track); 
	  seed = new AliTPCseed; 
	  //seed = NewSeed(); 
	}	
      }
    }
    }
    delete seed;
    
}

//_____________________________________________________________________________
void AliTPCtrackerMI::MakeSeeds3(TObjArray * arr, Int_t sec, Int_t i1, Int_t i2, Float_t deltay, Float_t ccut) {
  //-----------------------------------------------------------------
  // This function creates track seeds.
  //-----------------------------------------------------------------
  //  if (fSeeds==0) fSeeds=new TObjArray(15000);

  Double_t x[5], c[15];
  Int_t di = i1-i2;
  i2       = i1-di;

  //AliTPCseed * seed = NewSeed();
  AliTPCseed * seed = new AliTPCseed;
  Double_t alpha=fOuterSec->GetAlpha(), shift=fOuterSec->GetAlphaShift();
  Double_t cs=cos(alpha), sn=sin(alpha);
  //
  Double_t x1 =fOuterSec->GetX(i1);
  Double_t xx2=fOuterSec->GetX(i2);
  Double_t x3=GetX(), y3=GetY(), z3=GetZ();

  Int_t im = (i2+i1)/2;    //middle pad row index
  Double_t xm = fOuterSec->GetX(im); // radius of middle pad-row
  const AliTPCRow& krm=fOuterSec[sec][im]; //middle pad -row
  //
  Int_t ns =sec;   

  const AliTPCRow& kr1=fOuterSec[ns][i1];
  Double_t ymax = fSectors->GetMaxY(i1)-kr1.fDeadZone-1.5;  
  Int_t ddsec = 1;
  if (deltay>0) ddsec = 0; 
  // loop over clusters  
  for (Int_t is=0; is < kr1; is++) {
    //
    Double_t y1=kr1[is]->GetY(), z1=kr1[is]->GetZ();    
    if (kr1[is]->IsUsed()) continue;
    if (deltay>0 && TMath::Abs(ymax-TMath::Abs(y1))> deltay ) continue;  // seed only at the edge

    // find possible directions    
    Float_t anglez = (z1-z3)/(x1-x3); 
    Float_t extraz = z1 - anglez*(x1-xx2);  // extrapolated z      
    //
    // loop over 2 sectors
    Int_t dsec1=-ddsec;
    Int_t dsec2= 0;
    if (y1<0)  dsec2= 0;
    if (y1>0)  dsec1= 0;
    //
    for (Int_t dsec = dsec1; dsec<=dsec2;dsec++){
      Int_t sec2 = sec + dsec;
      //
      AliTPCRow&  kr2 = fOuterSec[(sec2+fkNOS)%fkNOS][i2];
      Int_t  index1 = TMath::Max(kr2.Find(extraz-30.)-1,0);
      Int_t  index2 = TMath::Min(kr2.Find(extraz+30.)+1,kr2);
      Double_t x2,   y2,   z2;
      
      for (Int_t js=index1; js < index2; js++) {
	const AliTPCclusterMI *kcl = kr2[js];
	if (kcl->IsUsed()) continue;
	x2 = xx2; 
	z2 = kcl->GetZ();
	y2 = kcl->GetY(); 
	if (dsec!=0){
	  // rotation	  
	  x2= xx2*cs-y2*sn*dsec;
	  y2=+xx2*sn*dsec+y2*cs;	  
	}

	// Double_t zz=z1 - anglez*(x1-x2); 
        //if (TMath::Abs(zz-z2)>10.) continue;

        Double_t d=(x2-x1)*(0.-y2)-(0.-x2)*(y2-y1);
        if (d==0.) {cerr<<"MakeSeeds warning: Straight seed !\n"; continue;}

	x[3]=f3(x1,y1,x2,y2,z1,z2);
	if (TMath::Abs(x[3]) > 1.2) continue;

	x[0]=y1;
	x[1]=z1;
	x[4]=f1(x1,y1,x2,y2,x3,y3);
	if (TMath::Abs(x[4]) >= ccut) continue;
	x[2]=f2(x1,y1,x2,y2,x3,y3);
	//if (TMath::Abs(x[4]*x1-x[2]) >= 0.99999) continue;

	Double_t a=asin(x[2]);
	Double_t zv=z1 - x[3]/x[4]*(a+asin(x[4]*x1-x[2]));
	if (TMath::Abs(zv-z3)>10.) continue; 
	//
        Double_t sy1=kr1[is]->GetSigmaY2()*2., sz1=kr1[is]->GetSigmaZ2()*2.;
        Double_t sy2=kcl->GetSigmaY2()*2.,     sz2=kcl->GetSigmaZ2()*2.;
	//Double_t sy3=400*3./12., sy=0.1, sz=0.1;
	Double_t sy3=25000*x[4]*x[4]+0.1, sy=0.1, sz=0.1;
	//Double_t sy3=25000*x[4]*x[4]*60+0.5, sy=0.1, sz=0.1;

	Double_t f40=(f1(x1,y1+sy,x2,y2,x3,y3)-x[4])/sy;
	Double_t f42=(f1(x1,y1,x2,y2+sy,x3,y3)-x[4])/sy;
	Double_t f43=(f1(x1,y1,x2,y2,x3,y3+sy)-x[4])/sy;
	Double_t f20=(f2(x1,y1+sy,x2,y2,x3,y3)-x[2])/sy;
	Double_t f22=(f2(x1,y1,x2,y2+sy,x3,y3)-x[2])/sy;
	Double_t f23=(f2(x1,y1,x2,y2,x3,y3+sy)-x[2])/sy;
	Double_t f30=(f3(x1,y1+sy,x2,y2,z1,z2)-x[3])/sy;
	Double_t f31=(f3(x1,y1,x2,y2,z1+sz,z2)-x[3])/sz;
	Double_t f32=(f3(x1,y1,x2,y2+sy,z1,z2)-x[3])/sy;
	Double_t f34=(f3(x1,y1,x2,y2,z1,z2+sz)-x[3])/sz;
	
        c[0]=sy1;
        c[1]=0.;       c[2]=sz1;
        c[3]=f20*sy1;  c[4]=0.;       c[5]=f20*sy1*f20+f22*sy2*f22+f23*sy3*f23;
        c[6]=f30*sy1;  c[7]=f31*sz1;  c[8]=f30*sy1*f20+f32*sy2*f22;
                       c[9]=f30*sy1*f30+f31*sz1*f31+f32*sy2*f32+f34*sz2*f34;
        c[10]=f40*sy1; c[11]=0.; c[12]=f40*sy1*f20+f42*sy2*f22+f43*sy3*f23;
        c[13]=f30*sy1*f40+f32*sy2*f42;
        c[14]=f40*sy1*f40+f42*sy2*f42+f43*sy3*f43;
	
	//	if (!BuildSeed(kr1[is],kcl,0,x1,x2,x3,x,c)) continue;

        UInt_t index=kr1.GetIndex(is);
	AliTPCseed *track=new(seed) AliTPCseed(index, x, c, x1, ns*alpha+shift);

	Double_t ym,zm;
	track->GetProlongation(xm,ym,zm);
	UInt_t dummy;
	AliTPCclusterMI * cm = krm.FindNearest2(ym,zm,0.3,0.3,dummy);
	if (!cm) {	  
	  seed->Reset();
	  //delete track;
	  seed->~AliTPCseed();
	  continue;
	}
	track->fIsSeeding = kTRUE;

	FollowProlongation(*track, i2,1);
	//
        zv = track->GetZ()+track->GetTgl()/track->GetC()*
	  ( asin(-track->GetEta()) - asin(track->GetX()*track->GetC()-track->GetEta()));
	if (TMath::Abs(zv-z3)>3) {
	  seed->Reset();
	  //delete track;
	  seed->~AliTPCseed();
	  continue;
	}
	
	//Int_t rc = 1;
	track->fBConstrain =1;
	track->fLastPoint = i1+fInnerSec->GetNRows();  // first cluster in track position
	if (track->GetNumberOfClusters()<(i1-i2)*0.5 || 
	    track->GetNumberOfClusters() < track->fNFoundable*0.6 || 
	    track->fNShared>0.4*track->GetNumberOfClusters() ) {
	  //seed = seed;
	  seed->Reset();
	  //delete track;
	  seed->~AliTPCseed();
	}
        else {
	  arr->AddLast(track); 
	  seed = new AliTPCseed; 
	  //seed = NewSeed(); 
	}
	
      }
    }
  }
  delete seed;
}


//_____________________________________________________________________________
void AliTPCtrackerMI::MakeSeeds2(TObjArray * arr, Int_t sec, Int_t i1, Int_t i2, 
				 Float_t deltay, Bool_t bconstrain) {
  //-----------------------------------------------------------------
  // This function creates track seeds - without vertex constraint
  //-----------------------------------------------------------------

  Double_t alpha=fOuterSec->GetAlpha(), shift=fOuterSec->GetAlphaShift();
  //  Double_t cs=cos(alpha), sn=sin(alpha);
  Int_t row0 = (i1+i2)/2;
  Int_t drow = (i1-i2)/2;
  const AliTPCRow& kr0=fSectors[sec][row0];
  AliTPCRow * kr=0;

  AliTPCpolyTrack polytrack;
  Int_t nclusters=fSectors[sec][row0];
  AliTPCseed * seed = new AliTPCseed;
  //AliTPCseed * seed = NewSeed();

  //  Int_t nfound =0;
  //Int_t nfoundable =0;
  Int_t sumused=0;
  Int_t cused=0;
  Int_t cnused=0;
  for (Int_t is=0; is < nclusters; is++) {  //LOOP over clusters
    Int_t nfound =0;
    Int_t nfoundable =0;
    for (Int_t iter =1; iter<2; iter++){   //iterations
      const AliTPCRow& krm=fSectors[sec][row0-iter];
      const AliTPCRow& krp=fSectors[sec][row0+iter];      
      const AliTPCclusterMI * cl= kr0[is];
      
      if (cl->IsUsed()) {
	cused++;
      }
      else{
	cnused++;
      }
      Double_t x = kr0.GetX();
      // Initialization of the polytrack
      nfound =0;
      nfoundable =0;
      polytrack.Reset();
      //
      Double_t y0= cl->GetY();
      Double_t z0= cl->GetZ();
      Float_t erry = 0;
      Float_t errz = 0;
      
      Double_t ymax = fSectors->GetMaxY(row0)-kr0.fDeadZone-1.5;
      if (deltay>0 && TMath::Abs(ymax-TMath::Abs(y0))> deltay ) continue;  // seed only at the edge
      
      erry = (0.5)*cl->GetSigmaY2()/TMath::Sqrt(cl->GetQ())*6;	    
      errz = (0.5)*cl->GetSigmaZ2()/TMath::Sqrt(cl->GetQ())*6;      
      polytrack.AddPoint(x,y0,z0,erry, errz);

      sumused=0;
      if (cl->IsUsed()) sumused++;


      Float_t roady = (5*TMath::Sqrt(cl->GetSigmaY2()+0.2)+1.)*iter;
      Float_t roadz = (5*TMath::Sqrt(cl->GetSigmaZ2()+0.2)+1.)*iter;
      //
      x = krm.GetX();
      AliTPCclusterMI * cl1 = krm.FindNearest(y0,z0,roady,roadz);
      if (cl1 && TMath::Abs(ymax-TMath::Abs(y0))) {
	erry = (0.5)*cl1->GetSigmaY2()/TMath::Sqrt(cl1->GetQ())*3;	    
	errz = (0.5)*cl1->GetSigmaZ2()/TMath::Sqrt(cl1->GetQ())*3;
	if (cl1->IsUsed())  sumused++;
	polytrack.AddPoint(x,cl1->GetY(),cl1->GetZ(),erry,errz);
      }
      //
      x = krp.GetX();
      AliTPCclusterMI * cl2 = krp.FindNearest(y0,z0,roady,roadz);
      if (cl2) {
	erry = (0.5)*cl2->GetSigmaY2()/TMath::Sqrt(cl2->GetQ())*3;	    
	errz = (0.5)*cl2->GetSigmaZ2()/TMath::Sqrt(cl2->GetQ())*3;
	if (cl2->IsUsed()) sumused++;	 
	polytrack.AddPoint(x,cl2->GetY(),cl2->GetZ(),erry,errz);
      }
      //
      if (sumused>1) continue;
      polytrack.UpdateParameters();
      // follow polytrack
      roadz = 1.2;
      roady = 1.2;
      //
      Double_t yn,zn;
      nfoundable = polytrack.GetN();
      nfound     = nfoundable; 
      //
      for (Int_t ddrow = iter+1; ddrow<drow;ddrow++){
	Float_t maxdist = 0.8*(1.+3./(ddrow));
	for (Int_t delta = -1;delta<=1;delta+=2){
	  Int_t row = row0+ddrow*delta;
	  kr = &(fSectors[sec][row]);
	  Double_t xn = kr->GetX();
	  Double_t ymax = fSectors->GetMaxY(row)-kr->fDeadZone-1.5;
	  polytrack.GetFitPoint(xn,yn,zn);
	  if (TMath::Abs(yn)>ymax) continue;
	  nfoundable++;
	  AliTPCclusterMI * cln = kr->FindNearest(yn,zn,roady,roadz);
	  if (cln) {
	    Float_t dist =  TMath::Sqrt(  (yn-cln->GetY())*(yn-cln->GetY())+(zn-cln->GetZ())*(zn-cln->GetZ()));
	    if (dist<maxdist){
	      
	      erry = (dist+0.3)*cln->GetSigmaY2()/TMath::Sqrt(cln->GetQ())*(1.+1./(ddrow));	    
	      errz = (dist+0.3)*cln->GetSigmaZ2()/TMath::Sqrt(cln->GetQ())*(1.+1./(ddrow));
	      if (cln->IsUsed()) {
		//	printf("used\n");
		sumused++;
		erry*=2;
		errz*=2;
	      }
	      polytrack.AddPoint(xn,cln->GetY(),cln->GetZ(),erry, errz);
	      nfound++;
	    }
	  }
	}
	polytrack.UpdateParameters();
	if (nfound<0.45*nfoundable) break;
      }      
      if (sumused>3) break;
      //      if (nfound>0.7*nfoundable)  break; 
      if (sumused>0.5*nfoundable) break;
    }
    if (sumused>3) {
      //printf("sumused   %d\n",sumused);
      continue;
    }

    if ((nfound>0.5*nfoundable) &&( nfoundable>0.4*(i1-i2))) {
      //
      // test seed with and without constrain
      for (Int_t constrain=0; constrain<=0;constrain++){
	// add polytrack candidate

	Double_t x[5], c[15];
	Double_t x1,x2,x3,y1,y2,y3,z1,z2,z3;
	polytrack.GetBoundaries(x3,x1);	
	x2 = (x1+x3)/2.;
	polytrack.GetFitPoint(x1,y1,z1);
	polytrack.GetFitPoint(x2,y2,z2);
	polytrack.GetFitPoint(x3,y3,z3);
	//
	//is track pointing to the vertex ?
	Double_t x0,y0,z0;
	x0=0;
	polytrack.GetFitPoint(x0,y0,z0);

	if (constrain) {
	  x2 = x3;
	  y2 = y3;
	  z2 = z3;
	  
	  x3 = 0;
	  y3 = 0;
	  z3 = 0;
	}
	x[0]=y1;
	x[1]=z1;
	x[4]=f1(x1,y1,x2,y2,x3,y3);
	if (TMath::Abs(x[4]) >= 0.05) continue;  //MI change
	x[2]=f2(x1,y1,x2,y2,x3,y3);
	//if (TMath::Abs(x[4]*x1-x[2]) >= 0.99999) continue;
	//x[3]=f3(x1,y1,x2,y2,z1,z2);
	x[3]=f3(x1,y1,x3,y3,z1,z3);
	if (TMath::Abs(x[3]) > 2.2) continue;
	if (TMath::Abs(x[2]) > 1.99) continue;
	//      Double_t a=asin(x[2]);
	
	
	Double_t sy =0.1, sz =0.1;
	Double_t sy1=0.02, sz1=0.02;
	Double_t sy2=0.02, sz2=0.02;
	Double_t sy3=0.02;

	if (constrain){
	  sy3=25000*x[4]*x[4]+0.1, sy=0.1, sz=0.1;
	}
	
	Double_t f40=(f1(x1,y1+sy,x2,y2,x3,y3)-x[4])/sy;
	Double_t f42=(f1(x1,y1,x2,y2+sy,x3,y3)-x[4])/sy;
	Double_t f43=(f1(x1,y1,x2,y2,x3,y3+sy)-x[4])/sy;
	Double_t f20=(f2(x1,y1+sy,x2,y2,x3,y3)-x[2])/sy;
	Double_t f22=(f2(x1,y1,x2,y2+sy,x3,y3)-x[2])/sy;
	Double_t f23=(f2(x1,y1,x2,y2,x3,y3+sy)-x[2])/sy;

	Double_t f30=(f3(x1,y1+sy,x3,y3,z1,z3)-x[3])/sy;
	Double_t f31=(f3(x1,y1,x3,y3,z1+sz,z3)-x[3])/sz;
	Double_t f32=(f3(x1,y1,x3,y3+sy,z1,z3)-x[3])/sy;
	Double_t f34=(f3(x1,y1,x3,y3,z1,z3+sz)-x[3])/sz;

	
	c[0]=sy1;
	c[1]=0.;       c[2]=sz1;
	c[3]=f20*sy1;  c[4]=0.;       c[5]=f20*sy1*f20+f22*sy2*f22+f23*sy3*f23;
	c[6]=f30*sy1;  c[7]=f31*sz1;  c[8]=f30*sy1*f20+f32*sy2*f22;
	c[9]=f30*sy1*f30+f31*sz1*f31+f32*sy2*f32+f34*sz2*f34;
	c[10]=f40*sy1; c[11]=0.; c[12]=f40*sy1*f20+f42*sy2*f22+f43*sy3*f23;
	c[13]=f30*sy1*f40+f32*sy2*f42;
	c[14]=f40*sy1*f40+f42*sy2*f42+f43*sy3*f43;
	
	Int_t row1 = fSectors->GetRowNumber(x1);
	UInt_t index=0;
	//kr0.GetIndex(is);
	AliTPCseed *track=new (seed) AliTPCseed(index, x, c, x1, sec*alpha+shift);
	track->fStopped =kFALSE;
	track->fIsSeeding = kTRUE;
	Int_t rc=FollowProlongation(*track, i2);	
	if (constrain) track->fBConstrain =1;
	else
	  track->fBConstrain =0;
	track->fLastPoint = row1+fInnerSec->GetNRows();  // first cluster in track position
	if (rc==0 || track->GetNumberOfClusters()<(i1-i2)*0.5 || 
	    track->GetNumberOfClusters() < track->fNFoundable*0.6 || 
	    track->fNShared>0.4*track->GetNumberOfClusters()) {
	  //delete track;
	  seed->Reset();
	  seed->~AliTPCseed();
	}
	else {
	  arr->AddLast(track);
	  seed = new AliTPCseed;
	  //seed = NewSeed(); 
	}
      }
    }  // if accepted seed
  }
  delete seed;
}


void AliTPCtrackerMI::ReSeed(AliTPCseed *t)
{
  //
  // reseed - refit -  track
  //
  Int_t first = 0;
  Int_t last  = fSectors->GetNRows()-1;
  //
  if (fSectors == fOuterSec){
    first = TMath::Max(first, t->fFirstPoint-fInnerSec->GetNRows());
    //last  = 
  }
  else
    first = t->fFirstPoint;
  //
  FollowBackProlongation(*t,fSectors->GetNRows()-1);
  t->Reset(kFALSE);
  FollowProlongation(*t,first);
}







//_____________________________________________________________________________
Int_t AliTPCtrackerMI::ReadSeeds(const TFile *inp) {
  //-----------------------------------------------------------------
  // This function reades track seeds.
  //-----------------------------------------------------------------
  TDirectory *savedir=gDirectory; 

  TFile *in=(TFile*)inp;
  if (!in->IsOpen()) {
     cerr<<"AliTPCtrackerMI::ReadSeeds(): input file is not open !\n";
     return 1;
  }

  in->cd();
  TTree *seedTree=(TTree*)in->Get("Seeds");
  if (!seedTree) {
     cerr<<"AliTPCtrackerMI::ReadSeeds(): ";
     cerr<<"can't get a tree with track seeds !\n";
     return 2;
  }
  AliTPCtrack *seed=new AliTPCtrack; 
  seedTree->SetBranchAddress("tracks",&seed);
  
  if (fSeeds==0) fSeeds=new TObjArray(15000);

  Int_t n=(Int_t)seedTree->GetEntries();
  for (Int_t i=0; i<n; i++) {
     seedTree->GetEvent(i);
     fSeeds->AddLast(new AliTPCseed(*seed,seed->GetAlpha()));
  }
  
  delete seed;
  delete seedTree; 
  savedir->cd();
  return 0;
}

//_____________________________________________________________________________
Int_t AliTPCtrackerMI::Clusters2Tracks(const TFile *inp, TFile *out) {
  //-----------------------------------------------------------------
  // This is a track finder.
  //-----------------------------------------------------------------
  TDirectory *savedir=gDirectory; 

  if (inp) {
     TFile *in=(TFile*)inp;
     if (!in->IsOpen()) {
        cerr<<"AliTPCtrackerMI::Clusters2Tracks(): input file is not open !\n";
        return 1;
     }
  }

  if (!out->IsOpen()) {
     cerr<<"AliTPCtrackerMI::Clusters2Tracks(): output file is not open !\n";
     return 2;
  }

  out->cd();

  char   tname[100];
  sprintf(tname,"TreeT_TPC_%d",fEventN);
  TTree tracktree(tname,"Tree with TPC tracks");
  TTree seedtree("Seeds","Seeds");
  AliTPCtrack *iotrack=0;
  AliTPCseed  *ioseed=0;
  tracktree.Branch("tracks","AliTPCtrack",&iotrack,32000,100);
  TStopwatch timer;
  
  printf("Loading clusters \n");
  LoadClusters();
  printf("Time for loading clusters: \t");timer.Print();timer.Start();

  //LoadClusters();
  //printf("Time for loading clusters: \t");timer.Print();timer.Start();

  printf("Loading outer sectors\n");
  LoadOuterSectors();
  printf("Time for loading outer sectors: \t");timer.Print();timer.Start();
  //LoadOuterSectors();
  //printf("Time for loading outer sectors: \t");timer.Print();timer.Start();

  printf("Loading inner sectors\n");
  LoadInnerSectors();
  printf("Time for loading inner sectors: \t");timer.Print();timer.Start();
  //LoadInnerSectors();
  //printf("Time for loading inner sectors: \t");timer.Print();timer.Start();


  fSeeds = Tracking();
  RemoveUsed(fSeeds,0.4,0.4,6);
  RemoveOverlap(fSeeds,0.99,7,kTRUE);
  //  CompressSeed(0);
  //
  //
  //activate again some tracks
  for (Int_t i=0; i<fSeeds->GetEntriesFast(); i++) {
    AliTPCseed *pt=(AliTPCseed*)fSeeds->UncheckedAt(i), &t=*pt;    
    if (!pt) continue;    
    Int_t nc=t.GetNumberOfClusters();
    if (nc<15) continue;
    if (pt->fRemoval==10) {
      //BuildDensity(pt);
      if ((pt->DensityBigger(0.9)>0) || (pt->DensityBigger(0.8)>1) || (pt->DensityBigger(0.7)>2))  
	pt->Desactivate(10);  // make track again active
      else
	pt->Desactivate(20); 	
    } 
  }
  
  ioseed  = (AliTPCseed*)(fSeeds->UncheckedAt(0));
  AliTPCseed * vseed = new AliTPCseed;
  vseed->fPoints = new TClonesArray("AliTPCTrackPoint",1);
  vseed->fEPoints = new TClonesArray("AliTPCExactPoint",1);
  vseed->fPoints->ExpandCreateFast(2);
  
  //TBranch * seedbranch =   
  seedtree.Branch("seeds","AliTPCseed",&vseed,32000,99);
  //delete vseed;
  Int_t nseed=fSeeds->GetEntriesFast();

  Int_t found = 0;
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)fSeeds->UncheckedAt(i), &t=*pt;    
    if (!pt) continue;    
    Int_t nc=t.GetNumberOfClusters();
    if (nc<15) {
      delete fSeeds->RemoveAt(i);
      continue;
    }
    t.CookdEdx(0.02,0.6);
    CookLabel(pt,0.1); //For comparison only
    //if ((pt->IsActive() || (pt->fRemoval==10) )&& nc>50 &&pt->GetNumberOfClusters()>0.4*pt->fNFoundable){
    if ((pt->IsActive() || (pt->fRemoval==10) )){
      iotrack=pt;
      //iotrack->PropagateToVertex();
      tracktree.Fill();
      cerr<<found++<<'\r';      
    }   
    /* 
      pt->RebuildSeed();
      seedbranch->SetAddress(&pt);
      
      seedtree.Fill();        
      for (Int_t j=0;j<160;j++){
      delete pt->fPoints->RemoveAt(j);
      }
      delete pt->fPoints;
      pt->fPoints =0;
    */
    delete fSeeds->RemoveAt(i);
  }
  //  fNTracks = found;
  printf("Time for track writing and dedx cooking: \t"); timer.Print();timer.Start();
  delete vseed;

  tracktree.Write();
  seedtree.Write();
  cerr<<"Number of found tracks : "<<"\t"<<found<<endl;  
  savedir->cd();

  UnloadClusters();
  printf("Time for unloading cluster: \t"); timer.Print();timer.Start();
  
  return 0;
}

void AliTPCtrackerMI::Tracking(TObjArray * arr)
{
  //
  // tracking of the seeds
  fSectors = fOuterSec;
  ParallelTracking(arr,fSectors->GetNRows()-1,0);
  //StopNotActive(arr,fInnerSec->GetNRows(), 0.2,0.3);
  //
  fSectors = fInnerSec;
  ParallelTracking(arr,fSectors->GetNRows()-1,fSectors->GetNRows()/2);
  //StopNotActive(arr,fInnerSec->GetNRows(), 0.2,0.3);
  ParallelTracking(arr,fSectors->GetNRows()/2-1,0);
  fSectors = fOuterSec;
  //CompressSeed(500);
}

TObjArray * AliTPCtrackerMI::Tracking(Int_t seedtype, Int_t i1, Int_t i2, Float_t ccut, Float_t dy)
{
  //
  //
  //tracking routine
  TObjArray * arr = new TObjArray;
  // 
  fSectors = fOuterSec;
  for (Int_t sec=0;sec<fkNOS;sec++){
    if (seedtype==3) MakeSeeds3(arr,sec,i1,i2,dy,ccut);
    if (seedtype==2) MakeSeeds2(arr,sec,i1,i2,dy);
  }
  /*
  Int_t nseed = arr->GetEntriesFast();
  for (Int_t i=0;i<nseed;i++){
    AliTPCseed * seed = (AliTPCseed*)arr->UncheckedAt(i);
    if (seed->fBConstrain==0)
      ReSeed(seed);
  }
  */
  Tracking(arr);  
  return arr;
}

TObjArray * AliTPCtrackerMI::Tracking()
{
  //
  //
  Int_t nup=fOuterSec->GetNRows();

  TObjArray * seeds = new TObjArray;
  TObjArray * arr=0;
  
  Int_t gap =20;
  Float_t ccut=0.0055;


  // first seedings in outer layers
  arr = Tracking(3,nup-1,nup-1-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //return seeds;
  //
  arr = Tracking(3,nup-3,nup-3-gap,ccut,-1);   
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);

  //  
  ccut+=0.0011;
  arr = Tracking(3,nup-6,nup-6-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  ccut+=0.0011;
  arr = Tracking(3,nup-9,nup-9-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  arr = Tracking(2,nup-1,nup-1-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8); 
  //
  arr = Tracking(2,nup-3,nup-3-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  arr = Tracking(3,nup-12,nup-11-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  // 
  arr = Tracking(2,nup-6,nup-6-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  ccut+=0.0011;
  arr = Tracking(3,nup-16,nup-16-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  arr = Tracking(2,nup-9,nup-9-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  arr = Tracking(2,nup-12,nup-12-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  arr = Tracking(3,nup-21,nup-21-gap,ccut,-1);
  SumTracks(seeds,arr);
  SignClusters(seeds,0.8);
  //
  //  arr = Tracking(2,nup-16,nup-16-gap,ccut,-1);
  //SumTracks(seeds,arr);
  //SignClusters(seeds,0.8);
  //
  //  RemoveOverlap(seeds,0.95,1);
  //
  //
  for (Int_t delta = 20;delta<nup-gap-10; delta+=10){
    arr = Tracking(3,nup-1-delta,nup-1-delta-gap,ccut,-1);
    SumTracks(seeds,arr);   
    SignClusters(seeds,0.8);
    ccut+=0.0011;
  }   

  for (Int_t delta = 20;delta<nup-gap-10; delta+=10){
    arr = Tracking(2,nup-5-delta,nup-5-delta-gap,0.0066,-1); 
    SumTracks(seeds,arr);
    SignClusters(seeds,0.8);
    ccut+=0.0022;
  }   
  arr = Tracking(3,gap+3,3,ccut,-1);
  SumTracks(seeds,arr);
  //
  arr = Tracking(3,10,0,ccut,-1);
  SumTracks(seeds,arr);
  //
  arr = Tracking(2,20,0,ccut,-1);
  SumTracks(seeds,arr);
  //
  arr = Tracking(2,7,0,ccut,-1);
  SumTracks(seeds,arr);
  //

  return seeds;
}

void AliTPCtrackerMI::SumTracks(TObjArray *arr1,TObjArray *arr2)
{
  //
  //sum tracks to common container
  Int_t nseed = arr2->GetEntriesFast();
  for (Int_t i=0;i<nseed;i++){
    if (arr2->At(i)){
      if ( ((AliTPCseed*)(arr2->At(i)))->GetNumberOfClusters()<20) 
	delete arr2->RemoveAt(i);
      else
	arr1->AddLast(arr2->RemoveAt(i));
    }
  }
  delete arr2;  
}



void  AliTPCtrackerMI::ParallelTracking(TObjArray * arr, Int_t rfirst, Int_t rlast)
{
  //
  // try to track in parralel

  Int_t nseed=arr->GetEntriesFast();
  //prepare seeds for tracking
  for (Int_t i=0; i<nseed; i++) {
    AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i), &t=*pt; 
    if (!pt) continue;
    if (!t.IsActive()) continue;
    // follow prolongation to the first layer
    if ( (fSectors ==fInnerSec) || (t.fFirstPoint>rfirst+1))  
      FollowProlongation(t, rfirst+1);
  }


  //
  for (Int_t nr=rfirst; nr>=rlast; nr--){      
    // make indexes with the cluster tracks for given       
    //    for (Int_t i = 0;i<fN;i++)
    //  fSectors[i][nr].MakeClusterTracks();

    // find nearest cluster
    for (Int_t i=0; i<nseed; i++) {
      AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i), &t=*pt;       
      if (!pt) continue;
      if (!pt->IsActive()) continue;
      if ( (fSectors ==fOuterSec) && pt->fFirstPoint<nr) continue;
      if (pt->fRelativeSector>17) {
	continue;
      }
      UpdateClusters(t,nr);
    }
    // prolonagate to the nearest cluster - if founded
    for (Int_t i=0; i<nseed; i++) {
      AliTPCseed *pt=(AliTPCseed*)arr->UncheckedAt(i); 
      if (!pt) continue;
      if (!pt->IsActive()) continue; 
      if ((fSectors ==fOuterSec) &&pt->fFirstPoint<nr) continue;
      if (pt->fRelativeSector>17) {
	continue;
      }
      FollowToNextCluster(*pt,nr);
    }
    //    for (Int_t i= 0;i<fN;i++)
    //  fSectors[i][nr].ClearClusterTracks();
  }    
}

void AliTPCtrackerMI::PrepareForBackProlongation(TObjArray * arr)
{
  //
  //
  // if we use TPC track itself we have to "update" covariance
  //
  

}

Int_t AliTPCtrackerMI::PropagateBack(TObjArray * arr)
{
  //
  //
  // if we use TPC track itself we have to "update" covariance
  //
  
  return 0;

}




Float_t  AliTPCtrackerMI::GetSigmaY(AliTPCseed * seed)
{
  //
  //  
  Float_t sd2 = TMath::Abs((fParam->GetZLength()-TMath::Abs(seed->GetZ())))*fParam->GetDiffL()*fParam->GetDiffL();
  Float_t padlength =  fParam->GetPadPitchLength(seed->fSector);
  Float_t sres = (seed->fSector < fParam->GetNSector()/2) ? 0.2 :0.3;
  Float_t angular  = seed->GetSnp();
  angular = angular*angular/(1-angular*angular);
  //  angular*=angular;
  //angular  = TMath::Sqrt(angular/(1-angular));
  Float_t res = TMath::Sqrt(sd2+padlength*padlength*angular/12.+sres*sres);
  return res;
}
Float_t  AliTPCtrackerMI::GetSigmaZ(AliTPCseed * seed)
{
  //
  //
  Float_t sd2 = TMath::Abs((fParam->GetZLength()-TMath::Abs(seed->GetZ())))*fParam->GetDiffL()*fParam->GetDiffL();
  Float_t padlength =  fParam->GetPadPitchLength(seed->fSector);
  Float_t sres = fParam->GetZSigma();
  Float_t angular  = seed->GetTgl();
  Float_t res = TMath::Sqrt(sd2+padlength*padlength*angular*angular/12.+sres*sres);
  return res;
}




//__________________________________________________________________________
void AliTPCtrackerMI::CookLabel(AliTPCseed *t, Float_t wrong) const {
  //--------------------------------------------------------------------
  //This function "cooks" a track label. If label<0, this track is fake.
  //--------------------------------------------------------------------
  Int_t noc=t->GetNumberOfClusters();
  Int_t *lb=new Int_t[noc];
  Int_t *mx=new Int_t[noc];
  AliTPCclusterMI **clusters=new AliTPCclusterMI*[noc];
  //
  for (Int_t i=0;i<noc;i++) {
    clusters[i]=0;
    lb[i]=mx[i]=0;
  }

  Int_t i;
  Int_t current=0;
  for (i=0; i<160,current<noc; i++) {
     
     Int_t index=t->GetClusterIndex2(i);
     if (index<=0) continue; 
     if (index&0x8000) continue;
     //     
     //clusters[current]=GetClusterMI(index);
     clusters[current]=t->fClusterPointer[i];     
     current++;
  }
  noc = current-1;

  Int_t lab=123456789;
  for (i=0; i<noc; i++) {
    AliTPCclusterMI *c=clusters[i];
    if (!c) continue;
    lab=TMath::Abs(c->GetLabel(0));
    Int_t j;
    for (j=0; j<noc; j++) if (lb[j]==lab || mx[j]==0) break;
    lb[j]=lab;
    (mx[j])++;
  }

  Int_t max=0;
  for (i=0; i<noc; i++) if (mx[i]>max) {max=mx[i]; lab=lb[i];}
    
  for (i=0; i<noc; i++) {
    AliTPCclusterMI *c=clusters[i]; 
    if (!c) continue;
    if (TMath::Abs(c->GetLabel(1)) == lab ||
        TMath::Abs(c->GetLabel(2)) == lab ) max++;
  }

  if ((1.- Float_t(max)/noc) > wrong) lab=-lab;

  else {
     Int_t tail=Int_t(0.10*noc);
     max=0;
     Int_t ind=0;
     for (i=1; i<=160&&ind<tail; i++) {
       //       AliTPCclusterMI *c=clusters[noc-i];
       AliTPCclusterMI *c=clusters[i];
       if (!c) continue;
       if (lab == TMath::Abs(c->GetLabel(0)) ||
           lab == TMath::Abs(c->GetLabel(1)) ||
           lab == TMath::Abs(c->GetLabel(2))) max++;
       ind++;
     }
     if (max < Int_t(0.5*tail)) lab=-lab;
  }

  t->SetLabel(lab);

  delete[] lb;
  delete[] mx;
  delete[] clusters;
}

//_________________________________________________________________________
void AliTPCtrackerMI::AliTPCSector::Setup(const AliTPCParam *par, Int_t f) {
  //-----------------------------------------------------------------------
  // Setup inner sector
  //-----------------------------------------------------------------------
  if (f==0) {
     fAlpha=par->GetInnerAngle();
     fAlphaShift=par->GetInnerAngleShift();
     fPadPitchWidth=par->GetInnerPadPitchWidth();
     fPadPitchLength=par->GetInnerPadPitchLength();
     fN=par->GetNRowLow();
     fRow=new AliTPCRow[fN];
     for (Int_t i=0; i<fN; i++) {
       fRow[i].SetX(par->GetPadRowRadiiLow(i));
       fRow[i].fDeadZone =1.5;  //1.5 cm of dead zone
     }
  } else {
     fAlpha=par->GetOuterAngle();
     fAlphaShift=par->GetOuterAngleShift();
     fPadPitchWidth  = par->GetOuterPadPitchWidth();
     fPadPitchLength = par->GetOuter1PadPitchLength();
     f1PadPitchLength = par->GetOuter1PadPitchLength();
     f2PadPitchLength = par->GetOuter2PadPitchLength();

     fN=par->GetNRowUp();
     fRow=new AliTPCRow[fN];
     for (Int_t i=0; i<fN; i++) {
       fRow[i].SetX(par->GetPadRowRadiiUp(i)); 
       fRow[i].fDeadZone =1.5;  // 1.5 cm of dead zone
     }
  } 
}


AliTPCtrackerMI::AliTPCRow::~AliTPCRow(){
  //
  if (fClusterTracks) delete [] fClusterTracks;
  fClusterTracks = 0;
}

void AliTPCtrackerMI::AliTPCRow::MakeClusterTracks(){
  //create cluster tracks
  if (fN>0) 
    fClusterTracks = new AliTPCclusterTracks[fN];
}

void AliTPCtrackerMI::AliTPCRow::ClearClusterTracks(){
  if (fClusterTracks) delete[] fClusterTracks;
  fClusterTracks =0;
}



void AliTPCtrackerMI::AliTPCRow::UpdateClusterTrack(Int_t clindex, Int_t trindex, AliTPCseed * seed){
  //
  //
  // update information of the cluster tracks - if track is nearer then other tracks to the 
  // given track
  const AliTPCclusterMI * cl = (*this)[clindex];
  AliTPCclusterTracks * cltracks = GetClusterTracks(clindex);
  // find the distance of the cluster to the track
  Float_t dy2 = (cl->GetY()- seed->GetY());
  dy2*=dy2;
  Float_t dz2 = (cl->GetZ()- seed->GetZ());
  dz2*=dz2;
  //
  Float_t distance = TMath::Sqrt(dy2+dz2);
  if (distance> 3) 
    return;  // MI - to be changed - AliTPCtrackerParam
  
  if ( distance < cltracks->fDistance[0]){
    cltracks->fDistance[2] =cltracks->fDistance[1];
    cltracks->fDistance[1] =cltracks->fDistance[0];
    cltracks->fDistance[0] =distance;
    cltracks->fTrackIndex[2] =cltracks->fTrackIndex[1];
    cltracks->fTrackIndex[1] =cltracks->fTrackIndex[0];
    cltracks->fTrackIndex[0] =trindex; 
  }
  else
    if ( distance < cltracks->fDistance[1]){
      cltracks->fDistance[2] =cltracks->fDistance[1];  
      cltracks->fDistance[1] =distance;
      cltracks->fTrackIndex[2] =cltracks->fTrackIndex[1];
      cltracks->fTrackIndex[1] =trindex; 
    } else
      if (distance < cltracks->fDistance[2]){
	cltracks->fDistance[2] =distance;
	cltracks->fTrackIndex[2] =trindex; 
      }  
}


//_________________________________________________________________________
void 
AliTPCtrackerMI::AliTPCRow::InsertCluster(const AliTPCclusterMI* c, UInt_t index) {
  //-----------------------------------------------------------------------
  // Insert a cluster into this pad row in accordence with its y-coordinate
  //-----------------------------------------------------------------------
  if (fN==kMaxClusterPerRow) {
    cerr<<"AliTPCRow::InsertCluster(): Too many clusters !\n"; return;
  }
  if (fN==0) {fIndex[0]=index; fClusters[fN++]=c; return;}
  Int_t i=Find(c->GetZ());
  memmove(fClusters+i+1 ,fClusters+i,(fN-i)*sizeof(AliTPCclusterMI*));
  memmove(fIndex   +i+1 ,fIndex   +i,(fN-i)*sizeof(UInt_t));
  fIndex[i]=index; fClusters[i]=c; fN++;
}


//___________________________________________________________________
Int_t AliTPCtrackerMI::AliTPCRow::Find(Double_t z) const {
  //-----------------------------------------------------------------------
  // Return the index of the nearest cluster 
  //-----------------------------------------------------------------------
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



//___________________________________________________________________
AliTPCclusterMI * AliTPCtrackerMI::AliTPCRow::FindNearest(Double_t y, Double_t z, Double_t roady, Double_t roadz) const {
  //-----------------------------------------------------------------------
  // Return the index of the nearest cluster in z y 
  //-----------------------------------------------------------------------
  Float_t maxdistance = roady*roady + roadz*roadz;

  AliTPCclusterMI *cl =0;
  for (Int_t i=Find(z-roadz); i<fN; i++) {
      AliTPCclusterMI *c=(AliTPCclusterMI*)(fClusters[i]);
      if (c->GetZ() > z+roadz) break;
      if ( (c->GetY()-y) >  roady ) continue;
      Float_t distance = (c->GetZ()-z)*(c->GetZ()-z)+(c->GetY()-y)*(c->GetY()-y);
      if (maxdistance>distance) {
	maxdistance = distance;
	cl=c;       
      }
  }
  return cl;      
}

AliTPCclusterMI * AliTPCtrackerMI::AliTPCRow::FindNearest2(Double_t y, Double_t z, Double_t roady, Double_t roadz,UInt_t & index) const 
{
  //-----------------------------------------------------------------------
  // Return the index of the nearest cluster in z y 
  //-----------------------------------------------------------------------
  Float_t maxdistance = roady*roady + roadz*roadz;

  AliTPCclusterMI *cl =0;
  for (Int_t i=Find(z-roadz); i<fN; i++) {
      AliTPCclusterMI *c=(AliTPCclusterMI*)(fClusters[i]);
      if (c->GetZ() > z+roadz) break;
      if ( c->GetY()-y >  roady ) continue;
      if ( y-c->GetY() >  roady ) continue;
      Float_t distance = (c->GetZ()-z)*(c->GetZ()-z)+(c->GetY()-y)*(c->GetY()-y);
      if (maxdistance>distance) {
	maxdistance = distance;
	cl=c;       
	index =i;
	//roady = TMath::Sqrt(maxdistance);
      }
  }
  return cl;      
}





AliTPCseed::AliTPCseed():AliTPCtrack(){
  //
  fRow=0; 
  fRemoval =0; 
  for (Int_t i=0;i<200;i++) SetClusterIndex2(i,-3);
  for (Int_t i=0;i<160;i++) fClusterPointer[i]=0;

  fPoints = 0;
  fEPoints = 0;
  fNFoundable =0;
  fNShared  =0;
  fTrackPoints =0;
  fRemoval = 0;
  fSort =0;
}

AliTPCseed::AliTPCseed(const AliTPCtrack &t):AliTPCtrack(t){
  fPoints = 0;
  fEPoints = 0;
  fNShared  =0; 
  fTrackPoints =0;
  fRemoval =0;
  fSort =0;
  for (Int_t i=0;i<200;i++) SetClusterIndex2(i,-3); 
  for (Int_t i=0;i<160;i++) fClusterPointer[i]=0;
}

AliTPCseed::AliTPCseed(const AliKalmanTrack &t, Double_t a):AliTPCtrack(t,a){
  fRow=0;
  for (Int_t i=0;i<200;i++) SetClusterIndex2(i,-3);
  for (Int_t i=0;i<160;i++) fClusterPointer[i]=0;
  fPoints = 0;
  fEPoints = 0;
  fNFoundable =0; 
  fNShared  =0; 
  fTrackPoints =0;
  fRemoval =0;
  fSort = 0;
}

AliTPCseed::AliTPCseed(UInt_t index, const Double_t xx[5], const Double_t cc[15], 
					Double_t xr, Double_t alpha):      
  AliTPCtrack(index, xx, cc, xr, alpha) {
  //
  //
  fRow =0;
  for (Int_t i=0;i<200;i++) SetClusterIndex2(i,-3);
  for (Int_t i=0;i<160;i++) fClusterPointer[i]=0;
  fPoints = 0;
  fEPoints = 0;
  fNFoundable =0;
  fNShared  = 0;
  fTrackPoints =0;
  fRemoval =0;
  fSort =0;
}

AliTPCseed::~AliTPCseed(){
  if (fPoints) delete fPoints;
  fPoints =0;
  if (fEPoints) delete fEPoints;
  fEPoints = 0;
  if (fTrackPoints){
    for (Int_t i=0;i<8;i++){
      delete [] fTrackPoints[i];
    }
    delete fTrackPoints;
    fTrackPoints =0;
  }

}

AliTPCTrackPoint * AliTPCseed::GetTrackPoint(Int_t i)
{
  //
  // 
  if (!fTrackPoints) {
    fTrackPoints = new AliTPCTrackPoint*[8];
    for ( Int_t i=0;i<8;i++)
      fTrackPoints[i]=0;
  }
  Int_t index1 = i/20;
  if (!fTrackPoints[index1]) fTrackPoints[index1] = new AliTPCTrackPoint[20];
  return &(fTrackPoints[index1][i%20]);
}

void AliTPCseed::RebuildSeed()
{
  //
  // rebuild seed to be ready for storing
  fPoints = new TClonesArray("AliTPCTrackPoint",160);
  fPoints->ExpandCreateFast(160);
  fEPoints = new TClonesArray("AliTPCExactPoint",1);
  for (Int_t i=0;i<160;i++){
    AliTPCTrackPoint *trpoint = (AliTPCTrackPoint*)fPoints->UncheckedAt(i);
    *trpoint = *(GetTrackPoint(i));
  }

}

void  AliTPCtrackerMI::BuildDensity(AliTPCseed *seed)
{
  //build table of densities
  //
  //
  Int_t maxindex = seed->fLastPoint;  //last foundable row
  
  for (Int_t ig=0;ig<15;ig++){
    Int_t foundable =0;
    Int_t found =0;
    for (Int_t j=0;j<20;j++){
      if (ig*10+j > maxindex ) continue;
      Int_t index = seed->GetClusterIndex2(ig*10+j);
      if (index!=-1) foundable++;
      //
      //      AliTPCclusterMI *c=(AliTPCclusterMI*)GetClusterMI(index);
      AliTPCclusterMI *c=seed->fClusterPointer[ig*10+j];
 
      if (!c) continue;
      if (!c->IsUsed()) found++;
    }
    if (foundable<10) seed->fClusterDensity[ig]=-1.;
    else{
      Float_t mean = (foundable+20.)/2.;
      seed->fClusterDensity[ig] = Float_t(found)/mean;
    }
  }
}

Int_t  AliTPCseed::DensityBigger(Float_t th)
{
  //
  //return number of trackletes with bigger densitie then th
  Int_t n=0;
  for (Int_t i=0;i<15;i++){
    if (fClusterDensity[i]>th) n++;
  }
  return n;
}


//_____________________________________________________________________________
void AliTPCseed::CookdEdx(Double_t low, Double_t up) {
  //-----------------------------------------------------------------
  // This funtion calculates dE/dX within the "low" and "up" cuts.
  //-----------------------------------------------------------------

  Float_t amp[200];
  Float_t angular[200];
  Float_t weight[200];
  Int_t index[200];
  //Int_t nc = 0;
  //  TClonesArray & arr = *fPoints; 
  Float_t meanlog = 100.;
  
  Float_t mean[4]  = {0,0,0,0};
  Float_t sigma[4] = {1000,1000,1000,1000};
  Int_t nc[4]      = {0,0,0,0};
  Float_t norm[4]    = {1000,1000,1000,1000};
  //
  //
  fNShared =0;

  for (Int_t of =0; of<4; of++){    
    for (Int_t i=of;i<160;i+=4)
      {
	Int_t index = fIndex[i];
	if (index<0||index&0x8000) continue;

	//AliTPCTrackPoint * point = (AliTPCTrackPoint *) arr.At(i);
	AliTPCTrackPoint * point = GetTrackPoint(i);
	AliTPCTrackPoint * pointm = GetTrackPoint(i-1);
	AliTPCTrackPoint * pointp = 0;
	if (i<159) pointp = GetTrackPoint(i+1);

	if (point==0) continue;
	AliTPCclusterMI * cl = fClusterPointer[i];
	if (cl==0) continue;
	Int_t   type   = cl->GetType();
	if (point->fIsShared){
	  fNShared++;
	  continue;
	}
	if (pointm) 
	  if (pointm->fIsShared) continue;
	if (pointp) 
	  if (pointp->fIsShared) continue;

	if (type<0) continue;
	//	if (point->GetCPoint().GetMax()<5) continue;
	if (cl->GetMax()<5) continue;
	Float_t angley = point->GetTPoint().GetAngleY();
	Float_t anglez = point->GetTPoint().GetAngleZ();

	Float_t rsigmay =  point->GetTPoint().GetSigmaY();
	Float_t rsigmaz =  point->GetTPoint().GetSigmaZ();
	/*
	Float_t ns = 1.;
	if (pointm){
	  rsigmay +=  pointm->GetTPoint().GetSigmaY();
	  rsigmaz +=  pointm->GetTPoint().GetSigmaZ();
	  ns+=1.;
	}
	if (pointp){
	  rsigmay +=  pointp->GetTPoint().GetSigmaY();
	  rsigmaz +=  pointp->GetTPoint().GetSigmaZ();
	  ns+=1.;
	}
	rsigmay/=ns;
	rsigmaz/=ns;
	*/

	Float_t rsigma = TMath::Sqrt(rsigmay*rsigmaz);

	Float_t ampc   = 0;     // normalization to the number of electrons
	if (i>64){
	  //	  ampc = 1.*point->GetCPoint().GetMax();
	  ampc = 1.*cl->GetMax();
	  //ampc = 1.*point->GetCPoint().GetQ();	  
	  //	  AliTPCClusterPoint & p = point->GetCPoint();
	  //	  Float_t dy = TMath::Abs(Int_t( TMath::Abs(p.GetY()/0.6)) - TMath::Abs(p.GetY()/0.6)+0.5);
	  // Float_t iz =  (250.0-TMath::Abs(p.GetZ())+0.11)/0.566;
	  //Float_t dz = 
	  //  TMath::Abs( Int_t(iz) - iz + 0.5);
	  //ampc *= 1.15*(1-0.3*dy);
	  //ampc *= 1.15*(1-0.3*dz);
	  //	  Float_t zfactor = (1.05-0.0004*TMath::Abs(point->GetCPoint().GetZ()));
	  //ampc               *=zfactor; 
	}
	else{ 
	  //ampc = 1.0*point->GetCPoint().GetMax(); 
	  ampc = 1.0*cl->GetMax(); 
	  //ampc = 1.0*point->GetCPoint().GetQ(); 
	  //AliTPCClusterPoint & p = point->GetCPoint();
	  // Float_t dy = TMath::Abs(Int_t( TMath::Abs(p.GetY()/0.4)) - TMath::Abs(p.GetY()/0.4)+0.5);
	  //Float_t iz =  (250.0-TMath::Abs(p.GetZ())+0.11)/0.566;
	  //Float_t dz = 
	  //  TMath::Abs( Int_t(iz) - iz + 0.5);

	  //ampc *= 1.15*(1-0.3*dy);
	  //ampc *= 1.15*(1-0.3*dz);
	  //	Float_t zfactor = (1.02-0.000*TMath::Abs(point->GetCPoint().GetZ()));
	  //ampc               *=zfactor; 

	}
	ampc *= 2.0;     // put mean value to channel 50
	//ampc *= 0.58;     // put mean value to channel 50
	Float_t w      =  1.;
	//	if (type>0)  w =  1./(type/2.-0.5); 
	Float_t z = TMath::Abs(cl->GetZ());
	if (i<64) {
	  ampc /= 0.6;
	  //ampc /= (1+0.0008*z);
	} else
	  if (i>128){
	    ampc /=1.5;
	    //ampc /= (1+0.0008*z);
	  }else{
	    //ampc /= (1+0.0008*z);
	  }
	
	if (type<0) {  //amp at the border - lower weight
	  // w*= 2.;
	  
	  continue;
	}
	if (rsigma>1.5) ampc/=1.3;  // if big backround
	amp[nc[of]]        = ampc;
	angular[nc[of]]    = TMath::Sqrt(1.+angley*angley+anglez*anglez);
	weight[nc[of]]     = w;
	nc[of]++;
      }
    
    TMath::Sort(nc[of],amp,index,kFALSE);
    Float_t sumamp=0;
    Float_t sumamp2=0;
    Float_t sumw=0;
    //meanlog = amp[index[Int_t(nc[of]*0.33)]];
    meanlog = 50;
    for (Int_t i=int(nc[of]*low+0.5);i<int(nc[of]*up+0.5);i++){
      Float_t ampl      = amp[index[i]]/angular[index[i]];
      ampl              = meanlog*TMath::Log(1.+ampl/meanlog);
      //
      sumw    += weight[index[i]]; 
      sumamp  += weight[index[i]]*ampl;
      sumamp2 += weight[index[i]]*ampl*ampl;
      norm[of]    += angular[index[i]]*weight[index[i]];
    }
    if (sumw<1){ 
      SetdEdx(0);  
    }
    else {
      norm[of] /= sumw;
      mean[of]  = sumamp/sumw;
      sigma[of] = sumamp2/sumw-mean[of]*mean[of];
      if (sigma[of]>0.1) 
	sigma[of] = TMath::Sqrt(sigma[of]);
      else
	sigma[of] = 1000;
      
    mean[of] = (TMath::Exp(mean[of]/meanlog)-1)*meanlog;
    //mean  *=(1-0.02*(sigma/(mean*0.17)-1.));
    //mean *=(1-0.1*(norm-1.));
    }
  }

  Float_t dedx =0;
  fSdEdx =0;
  fMAngular =0;
  //  mean[0]*= (1-0.05*(sigma[0]/(0.01+mean[1]*0.18)-1));
  //  mean[1]*= (1-0.05*(sigma[1]/(0.01+mean[0]*0.18)-1));

  
  //  dedx = (mean[0]* TMath::Sqrt((1.+nc[0]))+ mean[1]* TMath::Sqrt((1.+nc[1])) )/ 
  //  (  TMath::Sqrt((1.+nc[0]))+TMath::Sqrt((1.+nc[1])));

  Int_t norm2 = 0;
  Int_t norm3 = 0;
  for (Int_t i =0;i<4;i++){
    if (nc[i]>2&&nc[i]<1000){
      dedx      += mean[i] *nc[i];
      fSdEdx    += sigma[i]*(nc[i]-2);
      fMAngular += norm[i] *nc[i];    
      norm2     += nc[i];
      norm3     += nc[i]-2;
    }
    fDEDX[i]  = mean[i];             
    fSDEDX[i] = sigma[i];            
    fNCDEDX[i]= nc[i]; 
  }

  if (norm3>0){
    dedx   /=norm2;
    fSdEdx /=norm3;
    fMAngular/=norm2;
  }
  else{
    SetdEdx(0);
    return;
  }
  //  Float_t dedx1 =dedx;
  /*
  dedx =0;
  for (Int_t i =0;i<4;i++){
    if (nc[i]>2&&nc[i]<1000){
      mean[i]   = mean[i]*(1-0.12*(sigma[i]/(fSdEdx)-1.));
      dedx      += mean[i] *nc[i];
    }
    fDEDX[i]  = mean[i];                
  }
  dedx /= norm2;
  */

  
  SetdEdx(dedx);
    
  //mi deDX



  //Very rough PID
  Double_t p=TMath::Sqrt((1.+ GetTgl()*GetTgl())/(Get1Pt()*Get1Pt()));

  if (p<0.6) {
    if (dedx < 39.+ 12./(p+0.25)/(p+0.25)) { SetMass(0.13957); return;}
    if (dedx < 39.+ 12./p/p) { SetMass(0.49368); return;}
    SetMass(0.93827); return;
  }

  if (p<1.2) {
    if (dedx < 39.+ 12./(p+0.25)/(p+0.25)) { SetMass(0.13957); return;}
    SetMass(0.93827); return;
  }

  SetMass(0.13957); return;

}



/*



void AliTPCseed::CookdEdx2(Double_t low, Double_t up) {
  //-----------------------------------------------------------------
  // This funtion calculates dE/dX within the "low" and "up" cuts.
  //-----------------------------------------------------------------

  Float_t amp[200];
  Float_t angular[200];
  Float_t weight[200];
  Int_t index[200];
  Bool_t inlimit[200];
  for (Int_t i=0;i<200;i++) inlimit[i]=kFALSE;
  for (Int_t i=0;i<200;i++) amp[i]=10000;
  for (Int_t i=0;i<200;i++) angular[i]= 1;;
  

  //
  Float_t meanlog = 100.;
  Int_t indexde[4]={0,64,128,160};

  Float_t amean     =0;
  Float_t asigma    =0;
  Float_t anc       =0;
  Float_t anorm     =0;

  Float_t mean[4]  = {0,0,0,0};
  Float_t sigma[4] = {1000,1000,1000,1000};
  Int_t nc[4]      = {0,0,0,0};
  Float_t norm[4]    = {1000,1000,1000,1000};
  //
  //
  fNShared =0;

  //  for (Int_t of =0; of<3; of++){    
  //  for (Int_t i=indexde[of];i<indexde[of+1];i++)
  for (Int_t i =0; i<160;i++)
    {
	AliTPCTrackPoint * point = GetTrackPoint(i);
	if (point==0) continue;
	if (point->fIsShared){
	  fNShared++;	  
	  continue;
	}
	Int_t   type   = point->GetCPoint().GetType();
	if (type<0) continue;
	if (point->GetCPoint().GetMax()<5) continue;
	Float_t angley = point->GetTPoint().GetAngleY();
	Float_t anglez = point->GetTPoint().GetAngleZ();
	Float_t rsigmay =  point->GetCPoint().GetSigmaY();
	Float_t rsigmaz =  point->GetCPoint().GetSigmaZ();
	Float_t rsigma = TMath::Sqrt(rsigmay*rsigmaz);

	Float_t ampc   = 0;     // normalization to the number of electrons
	if (i>64){
	  ampc =  point->GetCPoint().GetMax();
	}
	else{ 
	  ampc = point->GetCPoint().GetMax(); 
	}
	ampc *= 2.0;     // put mean value to channel 50
	//	ampc *= 0.565;     // put mean value to channel 50

	Float_t w      =  1.;
	Float_t z = TMath::Abs(point->GetCPoint().GetZ());
	if (i<64) {
	  ampc /= 0.63;
	} else
	  if (i>128){
	    ampc /=1.51;
	  }	  	
	if (type<0) {  //amp at the border - lower weight	  	  
	  continue;
	}
	if (rsigma>1.5) ampc/=1.3;  // if big backround
	angular[i]    = TMath::Sqrt(1.+angley*angley+anglez*anglez);
	amp[i]        = ampc/angular[i];
	weight[i]     = w;
	anc++;
    }

  TMath::Sort(159,amp,index,kFALSE);
  for (Int_t i=int(anc*low+0.5);i<int(anc*up+0.5);i++){      
    inlimit[index[i]] = kTRUE;  // take all clusters
  }
  
  //  meanlog = amp[index[Int_t(anc*0.3)]];
  meanlog =10000.;
  for (Int_t of =0; of<3; of++){    
    Float_t sumamp=0;
    Float_t sumamp2=0;
    Float_t sumw=0;    
   for (Int_t i=indexde[of];i<indexde[of+1];i++)
      {
	if (inlimit[i]==kFALSE) continue;
	Float_t ampl      = amp[i];
	///angular[i];
	ampl              = meanlog*TMath::Log(1.+ampl/meanlog);
	//
	sumw    += weight[i]; 
	sumamp  += weight[i]*ampl;
	sumamp2 += weight[i]*ampl*ampl;
	norm[of]    += angular[i]*weight[i];
	nc[of]++;
      }
   if (sumw<1){ 
     SetdEdx(0);  
   }
   else {
     norm[of] /= sumw;
     mean[of]  = sumamp/sumw;
     sigma[of] = sumamp2/sumw-mean[of]*mean[of];
     if (sigma[of]>0.1) 
       sigma[of] = TMath::Sqrt(sigma[of]);
     else
       sigma[of] = 1000;      
     mean[of] = (TMath::Exp(mean[of]/meanlog)-1)*meanlog;
   }
  }
    
  Float_t dedx =0;
  fSdEdx =0;
  fMAngular =0;
  //
  Int_t norm2 = 0;
  Int_t norm3 = 0;
  Float_t www[3] = {12.,14.,17.};
  //Float_t www[3] = {1.,1.,1.};

  for (Int_t i =0;i<3;i++){
    if (nc[i]>2&&nc[i]<1000){
      dedx      += mean[i] *nc[i]*www[i]/sigma[i];
      fSdEdx    += sigma[i]*(nc[i]-2)*www[i]/sigma[i];
      fMAngular += norm[i] *nc[i];    
      norm2     += nc[i]*www[i]/sigma[i];
      norm3     += (nc[i]-2)*www[i]/sigma[i];
    }
    fDEDX[i]  = mean[i];             
    fSDEDX[i] = sigma[i];            
    fNCDEDX[i]= nc[i]; 
  }

  if (norm3>0){
    dedx   /=norm2;
    fSdEdx /=norm3;
    fMAngular/=norm2;
  }
  else{
    SetdEdx(0);
    return;
  }
  //  Float_t dedx1 =dedx;
  
  dedx =0;
  Float_t norm4 = 0;
  for (Int_t i =0;i<3;i++){
    if (nc[i]>2&&nc[i]<1000&&sigma[i]>3){
      //mean[i]   = mean[i]*(1+0.08*(sigma[i]/(fSdEdx)-1.));
      dedx      += mean[i] *(nc[i])/(sigma[i]);
      norm4     += (nc[i])/(sigma[i]);
    }
    fDEDX[i]  = mean[i];                
  }
  if (norm4>0) dedx /= norm4;
  

  
  SetdEdx(dedx);
    
  //mi deDX

}

*/

//-------------------------------------------------------------------------
//            Implementation of the ITS clusterer V2 class
//
//          Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------
//uncomment the line below for running with V1 cluster finder classes 
//#define V1

//PH 19/05/2003 This class hast to be adapted to NewIO

#include <stdlib.h>

#include "AliRun.h"

#include "AliITSclustererV2.h"
#include "AliITSclusterV2.h"

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include "AliITSgeom.h"
#include "AliITSdigit.h"

ClassImp(AliITSclustererV2)

extern AliRun *gAlice;

AliITSclustererV2::AliITSclustererV2(const AliITSgeom *geom) {
  //------------------------------------------------------------
  // Standard constructor
  //------------------------------------------------------------
  AliITSgeom *g=(AliITSgeom*)geom;

  fEvent=0;
  fI=0;

  Int_t mmax=geom->GetIndexMax();
  if (mmax>2200) {cerr<<"Too many ITS subdetectors !\n"; exit(1);}
  Int_t m;
  for (m=0; m<mmax; m++) {
     Int_t lay,lad,det; g->GetModuleId(m,lay,lad,det);
     Float_t x,y,z;     g->GetTrans(lay,lad,det,x,y,z); 
     Double_t rot[9];   g->GetRotMatrix(lay,lad,det,rot);
     fYshift[m] = x*rot[0] + y*rot[1];
     fZshift[m] = (Double_t)z;
     fNdet[m] = (lad-1)*g->GetNdetectors(lay) + (det-1);
  }

  //SPD geometry  
  fLastSPD1=g->GetModuleIndex(2,1,1)-1;
  fNySPD=256; fNzSPD=160;
  fYpitchSPD=0.0050;
  fZ1pitchSPD=0.0425; fZ2pitchSPD=0.0625;
  fHwSPD=0.64; fHlSPD=3.48;
  fYSPD[0]=0.5*fYpitchSPD;
  for (m=1; m<fNySPD; m++) fYSPD[m]=fYSPD[m-1]+fYpitchSPD; 
  fZSPD[0]=fZ1pitchSPD;
  for (m=1; m<fNzSPD; m++) {
    Double_t dz=fZ1pitchSPD;
    if (m==31 || m==32 || m==63  || m==64  || m==95 || m==96 || 
        m==127 || m==128) dz=fZ2pitchSPD; 
    fZSPD[m]=fZSPD[m-1]+dz;
  }
  for (m=0; m<fNzSPD; m++) {
    Double_t dz=0.5*fZ1pitchSPD;
    if (m==31 || m==32 || m==63  || m==64  || m==95 || m==96 || 
        m==127 || m==128) dz=0.5*fZ2pitchSPD; 
    fZSPD[m]-=dz;
  }

  //SDD geometry 
  fNySDD=256; fNzSDD=256;
  fYpitchSDD=0.01825;
  fZpitchSDD=0.02940;
  fHwSDD=3.5085; fHlSDD=3.7632;
  fYoffSDD=0.0425;

  //SSD geometry
  fLastSSD1=g->GetModuleIndex(6,1,1)-1;
  fYpitchSSD=0.0095;
  fHwSSD=3.65;
  fHlSSD=2.00;
  fTanP=0.0275;
  fTanN=0.0075;
}

void AliITSclustererV2::Digits2Clusters(const TFile *in, TFile *out) {
  //------------------------------------------------------------
  // This function creates ITS clusters
  //------------------------------------------------------------
  Int_t ncl=0;
  TDirectory *savedir=gDirectory;

  if (!out->IsOpen()) {
    cerr<<"AliITSclustererV2::Digits2Clusters(): output file not open !\n";
    return;
  }

  Char_t name[100];
  sprintf(name,"TreeD%d",fEvent);

  //TTree *dTree=(TTree*)((TFile*)in)->Get(name);
  TTree *dTree=gAlice->TreeD();
  if (!dTree) {
    cerr<<"Input tree "<<name<<" not found !\n";
    return;
  }

  TClonesArray *digitsSPD=new TClonesArray("AliITSdigitSPD",3000);
  dTree->SetBranchAddress("ITSDigitsSPD",&digitsSPD);
  TClonesArray *digitsSDD=new TClonesArray("AliITSdigitSDD",3000);
  dTree->SetBranchAddress("ITSDigitsSDD",&digitsSDD);
  TClonesArray *digitsSSD=new TClonesArray("AliITSdigitSSD",3000);
  dTree->SetBranchAddress("ITSDigitsSSD",&digitsSSD);

  Int_t mmax=(Int_t)dTree->GetEntries();

  out->cd();

  sprintf(name,"TreeC_ITS_%d",fEvent);
  TTree cTree(name,"ITS clusters V2");
  TClonesArray *clusters=new TClonesArray("AliITSclusterV2",1000);
  cTree.Branch("Clusters",&clusters);

  for (fI=0; fI<mmax; fI++) {
    dTree->GetEvent(fI);

    if     (digitsSPD->GetEntriesFast()!=0) 
                          FindClustersSPD(digitsSPD,clusters);
    else if(digitsSDD->GetEntriesFast()!=0) 
                          FindClustersSDD(digitsSDD,clusters);
    else if(digitsSSD->GetEntriesFast()!=0) 
                          FindClustersSSD(digitsSSD,clusters);

    ncl+=clusters->GetEntriesFast();

    cTree.Fill();

    digitsSPD->Clear();
    digitsSDD->Clear();
    digitsSSD->Clear();
    clusters->Clear();
  }
  cTree.Write();

  delete clusters;

  delete digitsSPD;
  delete digitsSDD;
  delete digitsSSD;

  //delete dTree;

  cerr<<"Number of found clusters : "<<ncl<<endl;

  savedir->cd();
}

//**** Fast clusters *******************************
#include "TParticle.h"

#include "AliITS.h"
#include "AliITSmodule.h"
#include "AliITSRecPoint.h"
#include "AliITSsimulationFastPoints.h"
#include "AliITSRecPoint.h"

static void CheckLabels(Int_t lab[3]) {
  //------------------------------------------------------------
  // Tries to find mother's labels
  //------------------------------------------------------------
    Int_t label=lab[0];
    if (label>=0) {
       TParticle *part=(TParticle*)gAlice->Particle(label);
       label=-3;
       while (part->P() < 0.005) {
          Int_t m=part->GetFirstMother();
          if (m<0) {cerr<<"Primary momentum: "<<part->P()<<endl; break;}
          if (part->GetStatusCode()>0) {
             cerr<<"Primary momentum: "<<part->P()<<endl; break;
          }
          label=m;
          part=(TParticle*)gAlice->Particle(label);
        }
        if      (lab[1]<0) lab[1]=label;
        else if (lab[2]<0) lab[2]=label;
        else ;//cerr<<"CheckLabels : No empty labels !\n";
    }
}

void AliITSclustererV2::RecPoints2Clusters
(const TClonesArray *points, Int_t idx, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Conversion AliITSRecPoint -> AliITSclusterV2 for the ITS 
  // subdetector indexed by idx 
  //------------------------------------------------------------
  TClonesArray &cl=*clusters;
  Int_t ncl=points->GetEntriesFast();
  for (Int_t i=0; i<ncl; i++) {
    AliITSRecPoint *p = (AliITSRecPoint *)points->UncheckedAt(i);
    Float_t lp[5];
    lp[0]=-p->GetX()-fYshift[idx]; if (idx<=fLastSPD1) lp[0]*=-1; //SPD1
    lp[1]=p->GetZ()+fZshift[idx];
    lp[2]=p->GetSigmaX2();
    lp[3]=p->GetSigmaZ2();
    lp[4]=p->GetQ();
    Int_t lab[4]; 
    lab[0]=p->GetLabel(0); lab[1]=p->GetLabel(1); lab[2]=p->GetLabel(2);
    lab[3]=fNdet[idx];
    CheckLabels(lab);
    new (cl[i]) AliITSclusterV2(lab,lp);
  }  
} 

void AliITSclustererV2::Hits2Clusters(const TFile *in, TFile *out) {
  //------------------------------------------------------------
  // This function creates ITS clusters
  //------------------------------------------------------------
  TDirectory *savedir=gDirectory;

  if (!out->IsOpen()) {
    cerr<<"AliITSclustererV2::Hits2Clusters: output file not open !\n";
    return;
  }

  if (!gAlice) {
     cerr<<"AliITSclustererV2::Hits2Clusters : gAlice==0 !\n";
     return;
  }

  AliITS *its  = (AliITS*)gAlice->GetModule("ITS");
  if (!its) { 
     cerr<<"AliITSclustererV2::Hits2Clusters : Can't find the ITS !\n"; 
     return; 
  }
  AliITSgeom *geom=its->GetITSgeom();
  Int_t mmax=geom->GetIndexMax();

  its->InitModules(-1,mmax);
  its->FillModules(gAlice->TreeH(),0);

  out->cd();

  Char_t name[100];
  sprintf(name,"TreeC_ITS_%d",fEvent);
  TTree cTree(name,"ITS clusters V2");
  TClonesArray *clusters=new TClonesArray("AliITSclusterV2",1000);
  cTree.Branch("Clusters",&clusters);

  static TClonesArray *points=its->RecPoints();
  AliITSsimulationFastPoints sim;
  Int_t ncl=0;
  for (Int_t m=0; m<mmax; m++) {
    AliITSmodule *mod=its->GetModule(m);      
    sim.CreateFastRecPoints(mod,m,gRandom);      

    RecPoints2Clusters(points, m, clusters);
    its->ResetRecPoints();

    ncl+=clusters->GetEntriesFast();
    cTree.Fill();
    clusters->Clear();
  }
  cTree.Write();

  cerr<<"Number of found fast clusters : "<<ncl<<endl;

  delete clusters;

  savedir->cd();
}

//***********************************

#ifndef V1

void AliITSclustererV2:: 
FindCluster(Int_t k,Int_t maxz,AliBin *bins,Int_t &n,Int_t *idx) {
  //------------------------------------------------------------
  // returns an array of indices of digits belonging to the cluster
  // (needed when the segmentation is not regular) 
  //------------------------------------------------------------
  if (n<200) idx[n++]=bins[k].GetIndex();
  bins[k].Use();

  if (bins[k-maxz].IsNotUsed()) FindCluster(k-maxz,maxz,bins,n,idx);
  if (bins[k-1   ].IsNotUsed()) FindCluster(k-1   ,maxz,bins,n,idx);
  if (bins[k+maxz].IsNotUsed()) FindCluster(k+maxz,maxz,bins,n,idx);
  if (bins[k+1   ].IsNotUsed()) FindCluster(k+1   ,maxz,bins,n,idx);
  /*
  if (bins[k-maxz-1].IsNotUsed()) FindCluster(k-maxz-1,maxz,bins,n,idx);
  if (bins[k-maxz+1].IsNotUsed()) FindCluster(k-maxz+1,maxz,bins,n,idx);
  if (bins[k+maxz-1].IsNotUsed()) FindCluster(k+maxz-1,maxz,bins,n,idx);
  if (bins[k+maxz+1].IsNotUsed()) FindCluster(k+maxz+1,maxz,bins,n,idx);
  */
}

void AliITSclustererV2::
FindClustersSPD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SPD cluster finder
  //------------------------------------------------------------
  const Int_t kMAXBIN=(fNzSPD+2)*(fNySPD+2);

  Int_t ndigits=digits->GetEntriesFast();
  AliBin *bins=new AliBin[kMAXBIN];

  Int_t k;
  AliITSdigitSPD *d=0;
  for (k=0; k<ndigits; k++) {
     d=(AliITSdigitSPD*)digits->UncheckedAt(k);
     Int_t i=d->GetCoord2()+1;   //y
     Int_t j=d->GetCoord1()+1;
     bins[i*fNzSPD+j].SetIndex(k);
     bins[i*fNzSPD+j].SetMask(1);
  }
   
  Int_t n=0; TClonesArray &cl=*clusters;
  for (k=0; k<kMAXBIN; k++) {
     if (!bins[k].IsNotUsed()) continue;
     Int_t ni=0, idx[200];
     FindCluster(k,fNzSPD,bins,ni,idx);
     if (ni==200) {cerr<<"SPD: Too big cluster !\n"; continue;}

     Int_t lab[4]; 
     lab[0]=-2;
     lab[1]=-2;
     lab[2]=-2;
     lab[3]=fNdet[fI];

     d=(AliITSdigitSPD*)digits->UncheckedAt(idx[0]);
     Int_t ymin=d->GetCoord2(),ymax=ymin;
     Int_t zmin=d->GetCoord1(),zmax=zmin;
     Float_t y=0.,z=0.,q=0.;
     for (Int_t l=0; l<ni; l++) {
        d=(AliITSdigitSPD*)digits->UncheckedAt(idx[l]);

        if (ymin > d->GetCoord2()) ymin=d->GetCoord2();
        if (ymax < d->GetCoord2()) ymax=d->GetCoord2();
        if (zmin > d->GetCoord1()) zmin=d->GetCoord1();
        if (zmax < d->GetCoord1()) zmax=d->GetCoord1();

        Int_t lab0=(d->GetTracks())[0];      
        if (lab0>=0) {
	  if (lab[0]<0) {
             lab[0]=lab0;
          } else if (lab[1]<0) {
            if (lab0!=lab[0]) lab[1]=lab0;
	  } else if (lab[2]<0) {
            if (lab0!=lab[0])
            if (lab0!=lab[1]) lab[2]=lab0;
          }
        }
        Float_t qq=d->GetSignal();
        y+=qq*fYSPD[d->GetCoord2()]; z+=qq*fZSPD[d->GetCoord1()]; q+=qq;   
     }
     y/=q; z/=q;
     y-=fHwSPD; z-=fHlSPD;

     Float_t lp[5];
     lp[0]=-y-fYshift[fI]; if (fI<=fLastSPD1) lp[0]=-lp[0];
     lp[1]= z+fZshift[fI];
     lp[2]= fYpitchSPD*fYpitchSPD/12.;
     lp[3]= fZ1pitchSPD*fZ1pitchSPD/12.;
     //lp[4]= q;
     lp[4]= (zmax-zmin+1)*100 + (ymax-ymin+1);

     //CheckLabels(lab);
     new (cl[n]) AliITSclusterV2(lab,lp); n++; 
  }

  delete bins;
}

Bool_t AliITSclustererV2::IsMaximum(Int_t k,Int_t max,const AliBin *bins) {
  //------------------------------------------------------------
  //is this a local maximum ?
  //------------------------------------------------------------
  UShort_t q=bins[k].GetQ();
  if (q==1023) return kFALSE;
  if (bins[k-max].GetQ() > q) return kFALSE;
  if (bins[k-1  ].GetQ() > q) return kFALSE; 
  if (bins[k+max].GetQ() > q) return kFALSE; 
  if (bins[k+1  ].GetQ() > q) return kFALSE; 
  if (bins[k-max-1].GetQ() > q) return kFALSE;
  if (bins[k+max-1].GetQ() > q) return kFALSE; 
  if (bins[k+max+1].GetQ() > q) return kFALSE; 
  if (bins[k-max+1].GetQ() > q) return kFALSE;
  return kTRUE; 
}

void AliITSclustererV2::
FindPeaks(Int_t k,Int_t max,AliBin *b,Int_t *idx,UInt_t *msk,Int_t& n) {
  //------------------------------------------------------------
  //find local maxima
  //------------------------------------------------------------
  if (n<31)
  if (IsMaximum(k,max,b)) {
    idx[n]=k; msk[n]=(2<<n);
    n++;
  }
  b[k].SetMask(0);
  if (b[k-max].GetMask()&1) FindPeaks(k-max,max,b,idx,msk,n);
  if (b[k-1  ].GetMask()&1) FindPeaks(k-1  ,max,b,idx,msk,n);
  if (b[k+max].GetMask()&1) FindPeaks(k+max,max,b,idx,msk,n);
  if (b[k+1  ].GetMask()&1) FindPeaks(k+1  ,max,b,idx,msk,n);
}

void AliITSclustererV2::
MarkPeak(Int_t k, Int_t max, AliBin *bins, UInt_t m) {
  //------------------------------------------------------------
  //mark this peak
  //------------------------------------------------------------
  UShort_t q=bins[k].GetQ();

  bins[k].SetMask(bins[k].GetMask()|m); 

  if (bins[k-max].GetQ() <= q)
     if ((bins[k-max].GetMask()&m) == 0) MarkPeak(k-max,max,bins,m);
  if (bins[k-1  ].GetQ() <= q)
     if ((bins[k-1  ].GetMask()&m) == 0) MarkPeak(k-1  ,max,bins,m);
  if (bins[k+max].GetQ() <= q)
     if ((bins[k+max].GetMask()&m) == 0) MarkPeak(k+max,max,bins,m);
  if (bins[k+1  ].GetQ() <= q)
     if ((bins[k+1  ].GetMask()&m) == 0) MarkPeak(k+1  ,max,bins,m);
}

void AliITSclustererV2::
MakeCluster(Int_t k,Int_t max,AliBin *bins,UInt_t m,AliITSclusterV2 &c) {
  //------------------------------------------------------------
  //make cluster using digits of this peak
  //------------------------------------------------------------
  Float_t q=(Float_t)bins[k].GetQ();
  Int_t i=k/max, j=k-i*max;

  c.SetQ(c.GetQ()+q);
  c.SetY(c.GetY()+i*q); 
  c.SetZ(c.GetZ()+j*q); 
  c.SetSigmaY2(c.GetSigmaY2()+i*i*q);
  c.SetSigmaZ2(c.GetSigmaZ2()+j*j*q);

  bins[k].SetMask(0xFFFFFFFE);
  
  if (bins[k-max].GetMask() == m) MakeCluster(k-max,max,bins,m,c);
  if (bins[k-1  ].GetMask() == m) MakeCluster(k-1  ,max,bins,m,c);
  if (bins[k+max].GetMask() == m) MakeCluster(k+max,max,bins,m,c);
  if (bins[k+1  ].GetMask() == m) MakeCluster(k+1  ,max,bins,m,c);
}

void AliITSclustererV2::
FindClustersSDD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SDD cluster finder
  //------------------------------------------------------------
  const Int_t kMAXBIN=(fNzSDD+2)*(fNySDD+2);

  AliBin *bins[2];
  bins[0]=new AliBin[kMAXBIN];
  bins[1]=new AliBin[kMAXBIN];

  AliITSdigitSDD *d=0;
  Int_t i, ndigits=digits->GetEntriesFast();
  for (i=0; i<ndigits; i++) {
     d=(AliITSdigitSDD*)digits->UncheckedAt(i);
     Int_t y=d->GetCoord2()+1;   //y
     Int_t z=d->GetCoord1()+1;   //z
     Int_t q=d->GetSignal();
     if (z <= fNzSDD) {
       bins[0][y*fNzSDD+z].SetQ(q);
       bins[0][y*fNzSDD+z].SetMask(1);
       bins[0][y*fNzSDD+z].SetIndex(i);
     } else {
       z-=fNzSDD; 
       bins[1][y*fNzSDD+z].SetQ(q);
       bins[1][y*fNzSDD+z].SetMask(1);
       bins[1][y*fNzSDD+z].SetIndex(i);
     }
  }
  
  Int_t ncl=0; TClonesArray &cl=*clusters;
  for (Int_t s=0; s<2; s++)
    for (i=0; i<kMAXBIN; i++) {
      if (bins[s][i].IsUsed()) continue;
      Int_t idx[32]; UInt_t msk[32]; Int_t npeaks=0;
      FindPeaks(i, fNzSDD, bins[s], idx, msk, npeaks);

      if (npeaks>30) continue;

      Int_t k,l;
      for (k=0; k<npeaks-1; k++){//mark adjacent peaks
        if (idx[k] < 0) continue; //this peak is already removed
        for (l=k+1; l<npeaks; l++) {
           if (idx[l] < 0) continue; //this peak is already removed
           Int_t ki=idx[k]/fNzSDD, kj=idx[k] - ki*fNzSDD;
           Int_t li=idx[l]/fNzSDD, lj=idx[l] - li*fNzSDD;
           Int_t di=TMath::Abs(ki - li);
           Int_t dj=TMath::Abs(kj - lj);
           if (di>1 || dj>1) continue;
           if (bins[s][idx[k]].GetQ() > bins[s][idx[l]].GetQ()) {
              msk[l]=msk[k];
              idx[l]*=-1;
           } else {
              msk[k]=msk[l];
              idx[k]*=-1;
              break;
           } 
        }
      }

      for (k=0; k<npeaks; k++) {
        MarkPeak(TMath::Abs(idx[k]), fNzSDD, bins[s], msk[k]);
      }
        
      for (k=0; k<npeaks; k++) {
         if (idx[k] < 0) continue; //removed peak
         AliITSclusterV2 c;
         MakeCluster(idx[k], fNzSDD, bins[s], msk[k], c);

         //if (c.GetQ() < 200) continue; //noise cluster

	 /*
         Float_t s2 = c.GetSigmaY2()/c.GetQ() - c.GetY()*c.GetY();
	 Float_t w=par->GetPadPitchWidth(sec);
         c.SetSigmaY2((s2 + 1./12.)*w*w);
         if (s2 != 0.) {
	   c.SetSigmaY2(c.GetSigmaY2()*0.108);
	   if (sec<par->GetNInnerSector()) c.SetSigmaY2(c.GetSigmaY2()*2.07);
         }

         s2 = c.GetSigmaZ2()/c.GetQ() - c.GetZ()*c.GetZ();
         w=par->GetZWidth();
         c.SetSigmaZ2((s2 + 1./12.)*w*w);
         if (s2 != 0.) {
	   c.SetSigmaZ2(c.GetSigmaZ2()*0.169);
	   if (sec<par->GetNInnerSector()) c.SetSigmaZ2(c.GetSigmaZ2()*1.77);
         }
	 */

         c.SetSigmaY2(0.0030*0.0030);
         c.SetSigmaZ2(0.0020*0.0020);
         c.SetDetectorIndex(fNdet[fI]);

         Float_t y=c.GetY(),z=c.GetZ(), q=c.GetQ();
         y/=q; z/=q;

         y=(y-0.5)*fYpitchSDD;
         y-=fHwSDD;
         y-=fYoffSDD;  //delay ?
         if (s) y=-y;

         z=(z-0.5)*fZpitchSDD;
         z-=fHlSDD;

         y=-y-fYshift[fI];
         z= z+fZshift[fI];
         c.SetY(y);
         c.SetZ(z);

         c.SetQ(q/20.);  //to be consistent with the SSD charges

         AliBin *b=&bins[s][idx[k]];
         d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
         Int_t l0=(d->GetTracks())[0];
         if (l0<0) {
	   b=&bins[s][idx[k]-1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]+1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
	   }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]-fNzSDD];
          if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]+fNzSDD];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }

         if (l0<0) {
	   b=&bins[s][idx[k]+fNzSDD+1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]+fNzSDD-1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]-fNzSDD+1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }
         if (l0<0) {
	   b=&bins[s][idx[k]-fNzSDD-1];
           if (b->GetQ()>0) {
             d=(AliITSdigitSDD*)digits->UncheckedAt(b->GetIndex());
             l0=(d->GetTracks())[0];
           }
         }

         {
	   Int_t lab[3];
           lab[0]=(d->GetTracks())[0];
           lab[1]=(d->GetTracks())[1];
           lab[2]=(d->GetTracks())[2];
           //CheckLabels(lab);
           c.SetLabel(lab[0],0);
           c.SetLabel(lab[1],1);
           c.SetLabel(lab[2],2);
         }

         new (cl[ncl]) AliITSclusterV2(c); ncl++;
      }
    }

  delete[] bins[0];
  delete[] bins[1];
}

void AliITSclustererV2::
FindClustersSSD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SSD cluster finder
  //------------------------------------------------------------
  Int_t smax=digits->GetEntriesFast();
  if (smax==0) return;

  const Int_t MAX=1000;
  Int_t np=0, nn=0; 
  Ali1Dcluster pos[MAX], neg[MAX];
  Float_t y=0., q=0., qmax=0.; 
  Int_t lab[4]={-2,-2,-2,-2};

  TClonesArray &cl=*clusters;

  AliITSdigitSSD *d=(AliITSdigitSSD*)digits->UncheckedAt(0);
  q += d->GetSignal();
  y += d->GetCoord2()*d->GetSignal();
  qmax=d->GetSignal();
  lab[0]=d->GetTrack(0); lab[1]=d->GetTrack(1); lab[2]=d->GetTrack(2);
  Int_t curr=d->GetCoord2();
  Int_t flag=d->GetCoord1();
  Int_t *n=&nn;
  Ali1Dcluster *c=neg;
  Int_t nd=0;
  for (Int_t s=1; s<smax; s++) {
      d=(AliITSdigitSSD*)digits->UncheckedAt(s);
      Int_t strip=d->GetCoord2();
      if ((strip-curr) > 1 || flag!=d->GetCoord1()) {
         c[*n].SetY(y/q);
         c[*n].SetQ(q);
         c[*n].SetNd(nd);
         c[*n].SetLabels(lab);
         //Split suspiciously big cluster
         if (nd>3) {
            c[*n].SetY(y/q-0.5*nd);
            c[*n].SetQ(0.5*q);
            (*n)++;
            if (*n==MAX) {
              cerr<<
              "AliITSclustererV2::FindClustersSSD: Too many 1D clusters !\n";
              return;
            }
            c[*n].SetY(y/q+0.5*nd);
            c[*n].SetQ(0.5*q);
            c[*n].SetNd(nd);
            c[*n].SetLabels(lab);
         }
         (*n)++;
         if (*n==MAX) {
          cerr<<"AliITSclustererV2::FindClustersSSD: Too many 1D clusters !\n";
          return;
         }
         y=q=qmax=0.;
         nd=0;
         lab[0]=lab[1]=lab[2]=-2;
         if (flag!=d->GetCoord1()) { n=&np; c=pos; }
      }
      flag=d->GetCoord1();
      q += d->GetSignal();
      y += d->GetCoord2()*d->GetSignal();
      nd++;
      if (d->GetSignal()>qmax) {
         qmax=d->GetSignal();
         lab[0]=d->GetTrack(0); lab[1]=d->GetTrack(1); lab[2]=d->GetTrack(2);
      }
      curr=strip;
  }
  c[*n].SetY(y/q);
  c[*n].SetQ(q);
  c[*n].SetNd(nd);
  c[*n].SetLabels(lab);
  //Split suspiciously big cluster
  if (nd>3) {
     c[*n].SetY(y/q-0.5*nd);
     c[*n].SetQ(0.5*q);
     (*n)++;
     if (*n==MAX) {
        cerr<<"AliITSclustererV2::FindClustersSSD: Too many 1D clusters !\n";
        return;
     }
     c[*n].SetY(y/q+0.5*nd);
     c[*n].SetQ(0.5*q);
     c[*n].SetNd(nd);
     c[*n].SetLabels(lab);
  }
  (*n)++;
  if (*n==MAX) {
     cerr<<"AliITSclustererV2::FindClustersSSD: Too many 1D clusters !\n";
     return;
  }

  Float_t tanp=fTanP, tann=fTanN;
  if (fI>fLastSSD1) {tann=fTanP; tanp=fTanN;}

  Int_t idet=fNdet[fI];
  Int_t ncl=0;
  for (Int_t i=0; i<np; i++) {
    //Float_t dq_min=1.e+33;
    Float_t ybest=1000,zbest=1000,qbest=0;
    Float_t yp=pos[i].GetY()*fYpitchSSD; 
    for (Int_t j=0; j<nn; j++) {
      //if (pos[i].fTracks[0] != neg[j].fTracks[0]) continue;
      Float_t yn=neg[j].GetY()*fYpitchSSD;
      Float_t zt=(2*fHlSSD*tanp + yp - yn)/(tann+tanp);
      Float_t yt=yn + tann*zt;
      zt-=fHlSSD; yt-=fHwSSD;
      if (TMath::Abs(yt)<fHwSSD+0.01)
      if (TMath::Abs(zt)<fHlSSD+0.01) {
      //if (TMath::Abs(pos[i].GetQ()-neg[j].GetQ())<dq_min) {
	//dq_min=TMath::Abs(pos[i].GetQ()-neg[j].GetQ());
        ybest=yt; zbest=zt; 
        qbest=0.5*(pos[i].GetQ()+neg[j].GetQ());

        lab[0]=pos[i].GetLabel(0);
        lab[1]=pos[i].GetLabel(1);
        lab[2]=neg[i].GetLabel(0);
        lab[3]=(((i<<10) + j)<<10) + idet; // pos|neg|det
        Float_t lp[5];
        lp[0]=-ybest-fYshift[fI];
        lp[1]= zbest+fZshift[fI];
        lp[2]=0.0025*0.0025;  //SigmaY2
        lp[3]=0.110*0.110;  //SigmaZ2
        if (pos[i].GetNd()+neg[j].GetNd() > 4) {
           lp[2]*=9;
           lp[3]*=9;
        }
        lp[4]=qbest;        //Q

        //CheckLabels(lab);
        new (cl[ncl]) AliITSclusterV2(lab,lp); ncl++;
      }
    }
    /*
    if (ybest<100) {
       lab[3]=idet;
       Float_t lp[5];
       lp[0]=-ybest-fYshift[fI];
       lp[1]= zbest+fZshift[fI];
       lp[2]=0.002*0.002;  //SigmaY2
       lp[3]=0.080*0.080;  //SigmaZ2
       lp[4]=qbest;        //Q
       //
       new (cl[ncl]) AliITSclusterV2(lab,lp); ncl++;
    }
    */
  }
}

#else   //V1

#include "AliITSDetType.h"

#include "AliITSsegmentationSPD.h"
#include "AliITSClusterFinderSPD.h"

#include "AliITSresponseSDD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSClusterFinderSDD.h"

#include "AliITSsegmentationSSD.h"
#include "AliITSClusterFinderSSD.h"


void AliITSclustererV2::
FindClustersSPD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SPD cluster finding based on AliITSClusterFinderSPD
  //------------------------------------------------------------
  static AliITS *its=(AliITS*)gAlice->GetModule("ITS");
  static TClonesArray *points=its->RecPoints();
  static AliITSsegmentationSPD *seg=
         (AliITSsegmentationSPD *)its->DetType(0)->GetSegmentationModel();
  static AliITSClusterFinderSPD cf(seg, (TClonesArray*)digits, points);

  cf.FindRawClusters(fI);
  RecPoints2Clusters(points, fI, clusters);
  its->ResetRecPoints();

}

void AliITSclustererV2::
FindClustersSDD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SDD cluster finding based on AliITSClusterFinderSDD
  //------------------------------------------------------------
  static AliITS *its=(AliITS*)gAlice->GetModule("ITS");
  static TClonesArray *points=its->RecPoints();
  static AliITSresponseSDD *resp=
        (AliITSresponseSDD *)its->DetType(1)->GetResponseModel();
  static AliITSsegmentationSDD *seg=
         (AliITSsegmentationSDD *)its->DetType(1)->GetSegmentationModel();
  static AliITSClusterFinderSDD 
         cf(seg,resp,(TClonesArray*)digits,its->ClustersAddress(1));

  cf.FindRawClusters(fI);
  Int_t nc=points->GetEntriesFast();
  while (nc--) { //To be consistent with the SSD cluster charges
     AliITSRecPoint *p=(AliITSRecPoint*)points->UncheckedAt(nc);
     p->SetQ(p->GetQ/12.);
  }
  RecPoints2Clusters(points, fI, clusters);
  its->ResetClusters(1);
  its->ResetRecPoints();

}

void AliITSclustererV2::
FindClustersSSD(const TClonesArray *digits, TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SSD cluster finding based on AliITSClusterFinderSSD
  //------------------------------------------------------------
  static AliITS *its=(AliITS*)gAlice->GetModule("ITS");
  static TClonesArray *points=its->RecPoints();
  static AliITSsegmentationSSD *seg=
         (AliITSsegmentationSSD *)its->DetType(2)->GetSegmentationModel();
  static AliITSClusterFinderSSD cf(seg,(TClonesArray*)digits);

  cf.FindRawClusters(fI);
  RecPoints2Clusters(points, fI, clusters);
  its->ResetRecPoints();

}

#endif

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
Revision 1.7.4.2  2002/06/06 14:21:19  hristov
Merged with v3-08-02

Revision 1.7.4.1  2002/05/31 09:38:00  hristov
First set of changes done by Piotr

Revision 1.11  2002/10/23 07:17:34  alibrary
Introducing Riostream.h

Revision 1.10  2002/10/14 14:57:43  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.7.6.1  2002/06/10 15:26:12  hristov
Merged with v3-08-02

Revision 1.9  2002/05/09 06:57:09  kowal2
Protection against nonexisting input tree

Revision 1.8  2002/03/29 06:57:45  kowal2
Restored backward compatibility to use the hits from Dec. 2000 production.

Revision 1.7  2001/10/21 19:04:55  hristov
Several patches were done to adapt the barel reconstruction to the multi-event case. Some memory leaks were corrected. (Yu.Belikov)

Revision 1.6  2001/08/30 09:28:48  hristov
TTree names are explicitly set via SetName(name) and then Write() is called

Revision 1.5  2001/07/20 14:32:44  kowal2
Processing of many events possible now

Revision 1.4  2001/04/17 08:06:27  hristov
Possibility to define the magnetic field in the reconstruction (Yu.Belikov)

Revision 1.3  2000/10/05 16:14:01  kowal2
Forward declarations.

Revision 1.2  2000/06/30 12:07:50  kowal2
Updated from the TPC-PreRelease branch

Revision 1.1.2.1  2000/06/25 08:53:55  kowal2
Splitted from AliTPCtracking

*/

//-------------------------------------------------------
//          Implementation of the TPC clusterer
//
//   Origin: Jouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------

#include "AliTPCclusterer.h"
#include "AliTPCcluster.h"
#include <TObjArray.h>
#include <TFile.h>
#include "AliTPCClustersArray.h"
#include "AliTPCClustersRow.h"
#include "AliDigits.h"
#include "AliSimDigits.h"
#include "AliTPCParam.h"
#include <Riostream.h>
#include <TTree.h>
#include "AliRunLoader.h"
#include "AliLoader.h"

void AliTPCclusterer::FindPeaks(Int_t k,Int_t max,
AliBin *b,Int_t *idx,UInt_t *msk,Int_t& n) {
  //find local maxima
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

void AliTPCclusterer::MarkPeak(Int_t k, Int_t max, AliBin *bins, UInt_t m) {
  //mark this peak
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

void AliTPCclusterer::MakeCluster(Int_t k,Int_t max,AliBin *bins,UInt_t m,
AliTPCcluster &c) {
  //make cluster using digits of this peak
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

//_____________________________________________________________________________
void AliTPCclusterer::Digits2Clusters(const AliTPCParam *par, AliLoader *of, Int_t eventn)
{
  //-----------------------------------------------------------------
  // This is a simple cluster finder.
  //-----------------------------------------------------------------
  AliRunLoader* rl = (AliRunLoader*)of->GetEventFolder()->FindObject(AliRunLoader::fgkRunLoaderName);
  rl->GetEvent(eventn);
  TDirectory *savedir=gDirectory; 

  if (of->TreeR() == 0x0) of->MakeTree("R");
  
  
  if (of == 0x0) 
   {
     cerr<<"AliTPC::Digits2Clusters(): output file not open !\n";
     return;
   }

  const Int_t kMAXZ=par->GetMaxTBin()+2;

  
  TTree *t = (TTree *)of->TreeD();

  if (!t) {
    cerr<<"Input tree with digits not found"<<endl;
    return;
  }

  AliSimDigits digarr, *dummy=&digarr;
  t->GetBranch("Segment")->SetAddress(&dummy);
  Stat_t nentries = t->GetEntries();

  cout<<"Got "<<nentries<<" from TreeD"<<endl;

//  ((AliTPCParam*)par)->Write(par->GetTitle());
  
  AliTPCClustersArray carray;
  carray.Setup(par);
  carray.SetClusterType("AliTPCcluster");

  TTree* treeR = of->TreeR();
  carray.MakeTree(treeR);



  Int_t nclusters=0;

  for (Int_t n=0; n<nentries; n++) 
   {
   
    Int_t sec, row;
    t->GetEvent(n);

    if (!par->AdjustSectorRow(digarr.GetID(),sec,row)) {
       cerr<<"AliTPC warning: invalid segment ID ! "<<digarr.GetID()<<endl;
       continue;
    }

    AliTPCClustersRow *clrow=carray.CreateRow(sec,row);

    Float_t rx=par->GetPadRowRadii(sec,row);

    Int_t npads, sign;
    {
       const Int_t kNIS=par->GetNInnerSector(), kNOS=par->GetNOuterSector();
       if (sec < kNIS) {
          npads = par->GetNPadsLow(row);
          sign = (sec < kNIS/2) ? 1 : -1;
       } else {
          npads = par->GetNPadsUp(row);
          sign = ((sec-kNIS) < kNOS/2) ? 1 : -1;
       }
    }

    const Int_t kMAXBIN=kMAXZ*(npads+2);
    AliBin *bins=new AliBin[kMAXBIN];
    for (Int_t ii=0;ii<kMAXBIN;ii++) {
       bins[ii].SetQ(0); bins[ii].SetMask(0xFFFFFFFE);
    }

    digarr.First();
    do {
       Short_t dig=digarr.CurrentDigit();
       if (dig<=par->GetZeroSup()) continue;
       Int_t j=digarr.CurrentRow()+1, i=digarr.CurrentColumn()+1;
       bins[i*kMAXZ+j].SetQ(dig);
       bins[i*kMAXZ+j].SetMask(1);
    } while (digarr.Next());

    Int_t ncl=0;
    for (Int_t i=0; i<kMAXBIN; i++) {
      if ((bins[i].GetMask()&1) == 0) continue;
      Int_t idx[32]; UInt_t msk[32]; Int_t npeaks=0;
      FindPeaks(i, kMAXZ, bins, idx, msk, npeaks);

      if (npeaks>30) continue;

      Int_t k,l;
      for (k=0; k<npeaks-1; k++){//mark adjacent peaks
        if (idx[k] < 0) continue; //this peak is already removed
        for (l=k+1; l<npeaks; l++) {
           if (idx[l] < 0) continue; //this peak is already removed
           Int_t ki=idx[k]/kMAXZ, kj=idx[k] - ki*kMAXZ;
           Int_t li=idx[l]/kMAXZ, lj=idx[l] - li*kMAXZ;
           Int_t di=TMath::Abs(ki - li);
           Int_t dj=TMath::Abs(kj - lj);
           if (di>1 || dj>1) continue;
           if (bins[idx[k]].GetQ() > bins[idx[l]].GetQ()) {
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
        MarkPeak(TMath::Abs(idx[k]), kMAXZ, bins, msk[k]);
      }
        
      for (k=0; k<npeaks; k++) {
         if (idx[k] < 0) continue; //removed peak
         AliTPCcluster c;
         MakeCluster(idx[k], kMAXZ, bins, msk[k], c);
         if (c.GetQ() < 5) continue; //noise cluster
         c.SetY(c.GetY()/c.GetQ());
         c.SetZ(c.GetZ()/c.GetQ());

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

         c.SetY((c.GetY() - 0.5 - 0.5*npads)*par->GetPadPitchWidth(sec));
         c.SetZ(par->GetZWidth()*(c.GetZ()-1)); 
         c.SetZ(c.GetZ() - 3.*par->GetZSigma()); // PASA delay 
         c.SetZ(sign*(par->GetZLength() - c.GetZ()));

         if (rx<230./250.*TMath::Abs(c.GetZ())) continue;

         Int_t ki=idx[k]/kMAXZ, kj=idx[k] - ki*kMAXZ;
         c.SetLabel(digarr.GetTrackID(kj-1,ki-1,0),0);
         c.SetLabel(digarr.GetTrackID(kj-1,ki-1,1),1);
         c.SetLabel(digarr.GetTrackID(kj-1,ki-1,2),2);

         c.SetQ(bins[idx[k]].GetQ());

         if (ki==1 || ki==npads || kj==1 || kj==kMAXZ-2) {
           c.SetSigmaY2(c.GetSigmaY2()*25.);
           c.SetSigmaZ2(c.GetSigmaZ2()*4.);
         }
         clrow->InsertCluster(&c); ncl++;
      }
    }
    carray.StoreRow(sec,row);
    carray.ClearRow(sec,row);

    //cerr<<"sector, row, compressed digits, clusters: "
    //<<sec<<' '<<row<<' '<<digarr.GetSize()<<' '<<ncl<<"                  \r";

    nclusters+=ncl;

    delete[] bins;  
  }

  cerr<<"Number of found clusters : "<<nclusters<<"                        \n";

  of->WriteRecPoints("OVERWRITE");
  
  savedir->cd();

//  delete t;  //Thanks to Mariana Bondila
}


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

/* $Id$ */

//-------------------------------------------------------------------------
//               Implementation of the AliTracker class
//  that is the base for AliTPCtracker, AliITStrackerV2 and AliTRDtracker    
//        Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------

#include <TMath.h>

#include "AliTracker.h"
#include "AliCluster.h"
#include "AliKalmanTrack.h"
#include "AliRun.h"
#include "AliMagF.h"

extern AliRun* gAlice;

const AliMagF *AliTracker::fgkFieldMap=0;

ClassImp(AliTracker)

AliTracker::AliTracker():
  fEventN(0),
  fStoreBarrel(1),
  fX(0),
  fY(0),
  fZ(0),
  fSigmaX(0.005),
  fSigmaY(0.005),
  fSigmaZ(0.010)
{
  //--------------------------------------------------------------------
  // The default constructor.
  //--------------------------------------------------------------------
 AliMagF *field=gAlice->Field();
 if (field==0) Fatal("AliTracker()","Can't access the field map !");
 SetFieldMap(field);
}

void AliTracker::SetFieldMap(const AliMagF* map) {
  //--------------------------------------------------------------------
  //This passes the field map to the reconstruction.
  //--------------------------------------------------------------------
  if (map==0) ::Fatal("SetFieldMap","Can't access the field map !");
  AliKalmanTrack::SetConvConst(1000/0.299792458/map->SolenoidField());
  fgkFieldMap=map;
}

//__________________________________________________________________________
void AliTracker::CookLabel(AliKalmanTrack *t, Float_t wrong) const {
  //--------------------------------------------------------------------
  //This function "cooks" a track label. If label<0, this track is fake.
  //--------------------------------------------------------------------
  Int_t noc=t->GetNumberOfClusters();
  Int_t *lb=new Int_t[noc];
  Int_t *mx=new Int_t[noc];
  AliCluster **clusters=new AliCluster*[noc];

  Int_t i;
  for (i=0; i<noc; i++) {
     lb[i]=mx[i]=0;
     Int_t index=t->GetClusterIndex(i);
     clusters[i]=GetCluster(index);
  }

  Int_t lab=123456789;
  for (i=0; i<noc; i++) {
    AliCluster *c=clusters[i];
    lab=TMath::Abs(c->GetLabel(0));
    Int_t j;
    for (j=0; j<noc; j++) if (lb[j]==lab || mx[j]==0) break;
    lb[j]=lab;
    (mx[j])++;
  }

  Int_t max=0;
  for (i=0; i<noc; i++) if (mx[i]>max) {max=mx[i]; lab=lb[i];}
    
  for (i=0; i<noc; i++) {
    AliCluster *c=clusters[i];
    //if (TMath::Abs(c->GetLabel(1)) == lab ||
    //    TMath::Abs(c->GetLabel(2)) == lab ) max++;
    if (TMath::Abs(c->GetLabel(0)!=lab))
	if (TMath::Abs(c->GetLabel(1)) == lab ||
	    TMath::Abs(c->GetLabel(2)) == lab ) max++;
  }

  if ((1.- Float_t(max)/noc) > wrong) lab=-lab;
  t->SetFakeRatio((1.- Float_t(max)/noc));
  t->SetLabel(lab);

  delete[] lb;
  delete[] mx;
  delete[] clusters;
}

//____________________________________________________________________________
void AliTracker::UseClusters(const AliKalmanTrack *t, Int_t from) const {
  //------------------------------------------------------------------
  //This function marks clusters associated with the track.
  //------------------------------------------------------------------
  Int_t noc=t->GetNumberOfClusters();
  for (Int_t i=from; i<noc; i++) {
     Int_t index=t->GetClusterIndex(i);
     AliCluster *c=GetCluster(index); 
     c->Use();   
  }
}

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

//-------------------------------------------------------------------------
//               Implementation of the V0 vertexer class
//                  reads tracks writes out V0 vertices
//                      fills the ESD with the V0s       
//     Origin: Iouri Belikov, IReS, Strasbourg, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------
#include <TObjArray.h>
#include <TTree.h>

#include "AliESD.h"
#include "AliESDv0.h"
#include "AliESDtrack.h"
#include "AliV0vertexer.h"

ClassImp(AliV0vertexer)

Int_t AliV0vertexer::Tracks2V0vertices(AliESD *event) {
  //--------------------------------------------------------------------
  //This function reconstructs V0 vertices
  //--------------------------------------------------------------------

   Int_t nentr=event->GetNumberOfTracks();
   Double_t b=event->GetMagneticField();

   if (nentr<2) return 0; 

   TArrayI neg(nentr/2);
   TArrayI pos(nentr/2);

   Int_t nneg=0, npos=0, nvtx=0;

   Int_t i;
   for (i=0; i<nentr; i++) {
     AliESDtrack *esdTrack=event->GetTrack(i);
     UInt_t status=esdTrack->GetStatus();
     UInt_t flags=AliESDtrack::kITSin|AliESDtrack::kTPCin|
                  AliESDtrack::kTPCpid|AliESDtrack::kESDpid;

     if ((status&AliESDtrack::kITSrefit)==0)
        if (flags!=status) continue;

     Double_t d=esdTrack->GetD(fX,fY,b);
     if (TMath::Abs(d)<fDPmin) continue;
     if (TMath::Abs(d)>fRmax) continue;

     if (esdTrack->GetSign() < 0.) neg[nneg++]=i;
     else pos[npos++]=i;
   }   


   for (i=0; i<nneg; i++) {
      Int_t nidx=neg[i];
      AliESDtrack *ntrk=event->GetTrack(nidx);

      for (Int_t k=0; k<npos; k++) {
         Int_t pidx=pos[k];
	 AliESDtrack *ptrk=event->GetTrack(pidx);

         if (TMath::Abs(ntrk->GetD(fX,fY,b))<fDNmin)
	   if (TMath::Abs(ptrk->GetD(fX,fY,b))<fDNmin) continue;

         Double_t xn, xp, dca=ntrk->GetDCA(ptrk,b,xn,xp);
         if (dca > fDCAmax) continue;
         if ((xn+xp) > 2*fRmax) continue;
         if ((xn+xp) < 2*fRmin) continue;
   
         AliExternalTrackParam nt(*ntrk), pt(*ptrk);
         Bool_t corrected=kFALSE;
         if ((nt.GetX() > 3.) && (xn < 3.)) {
	   //correct for the beam pipe material
           corrected=kTRUE;
         }
         if ((pt.GetX() > 3.) && (xp < 3.)) {
	   //correct for the beam pipe material
           corrected=kTRUE;
         }
         if (corrected) {
	   dca=nt.GetDCA(&pt,b,xn,xp);
           if (dca > fDCAmax) continue;
           if ((xn+xp) > 2*fRmax) continue;
           if ((xn+xp) < 2*fRmin) continue;
	 }

         nt.PropagateTo(xn,b); pt.PropagateTo(xp,b);

         AliESDv0 vertex(nt,nidx,pt,pidx);
         if (vertex.GetChi2() > fChi2max) continue;
	 
         /*  Think of something better here ! 
         nt.PropagateToVertex(); if (TMath::Abs(nt.GetZ())<0.04) continue;
         pt.PropagateToVertex(); if (TMath::Abs(pt.GetZ())<0.04) continue;
	 */

         Double_t x,y,z; vertex.GetXYZ(x,y,z); 
         Double_t px,py,pz; vertex.GetPxPyPz(px,py,pz);
         Double_t p2=px*px+py*py+pz*pz;
         Double_t cost=((x-fX)*px + (y-fY)*py + (z-fZ)*pz)/
               TMath::Sqrt(p2*((x-fX)*(x-fX) + (y-fY)*(y-fY) + (z-fZ)*(z-fZ)));

        //if (cost < (5*fCPAmax-0.9-TMath::Sqrt(r2)*(fCPAmax-1))/4.1) continue;
         if (cost < fCPAmax) continue;
	 vertex.SetDcaDaughters(dca);
         //vertex.ChangeMassHypothesis(); //default is Lambda0 

         event->AddV0(&vertex);

         nvtx++;
      }
   }

   Info("Tracks2V0vertices","Number of reconstructed V0 vertices: %d",nvtx);

   return nvtx;
}















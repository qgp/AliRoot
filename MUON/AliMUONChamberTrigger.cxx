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

#include "AliMUONChamberTrigger.h"
#include "AliMUONSegmentationTrigger.h"
#include "AliMUONResponseTrigger.h"
#include "AliMUONResponseTriggerV1.h"
#include <TObjArray.h>
#include <TMath.h>
#include <Riostream.h>

ClassImp(AliMUONChamberTrigger)

//-------------------------------------------

AliMUONChamberTrigger::AliMUONChamberTrigger()
{
// Default constructor
}


AliMUONChamberTrigger::AliMUONChamberTrigger(Int_t id) : AliMUONChamber(id)
{
// Constructor using chamber id
}

//-------------------------------------------
void AliMUONChamberTrigger::DisIntegration(Float_t eloss, Float_t tof, 
					   Float_t xhit, Float_t yhit, Float_t zhit, 
					   Int_t& nnew,
					   Float_t newclust[6][500]) 
{
//    
//  Generates pad hits (simulated cluster) 
//  using the segmentation and the response model

  Int_t twentyNano;
  if (tof<75*TMath::Power(10,-9)) {
    twentyNano=1;
  } else {
    twentyNano=100;
  }

  Float_t qp;
  nnew=0;
  for (Int_t i=1; i<=fnsec; i++) {
    AliSegmentation * segmentation=
      (AliSegmentation*) (*fSegmentation)[i-1];
    
// Find the module & strip Id. which has fired
    Int_t ix,iy;
    
    segmentation->GetPadI(xhit,yhit,0,ix,iy);
    segmentation->SetPad(ix,iy);

// treatment of GEANT hits w/o corresponding strip (due to the fact that
// the 2 geometries are computed in a very slightly different way) 
    if (ix==0&&iy==0) {
      printf("AliMUONChamberTrigger hit w/o strip %f %f \n",xhit,yhit);
    } else {          
      // --- store signal information for this strip
      newclust[0][nnew]=1.;                       // total charge
      newclust[1][nnew]=ix;                       // ix-position of pad
      newclust[2][nnew]=iy;                       // iy-position of pad
      newclust[3][nnew]=twentyNano;               // time of flight
      newclust[4][nnew]=segmentation->ISector();  // sector id
      newclust[5][nnew]=(Float_t) i;              // counter
      nnew++;

// cluster-size if AliMUONResponseTriggerV1, nothing if AliMUONResponseTrigger
      if (((AliMUONResponseTrigger*) fResponse)->SetGenerCluster()) {
  
	// set hits
	segmentation->SetHit(xhit,yhit,zhit);
	// get the list of nearest neighbours
	Int_t nList, xList[10], yList[10];
	segmentation->Neighbours(ix,iy,&nList,xList,yList);
	
	qp = 0;
	for (Int_t j=0; j<nList; j++){       // loop over neighbours	  
	  if (xList[j]!=0) {                 // existing neighbour	    
	    if (j==0||j==5||qp!=0) {         // built-up cluster-size
	      
	      // neighbour real coordinates (just for checks here)
	      Float_t x,y,z;
	      segmentation->GetPadC(xList[j],yList[j],x,y,z);
	      // set pad (fx fy & fix fiy are the current pad coord. & Id.)
	      segmentation->SetPad(xList[j],yList[j]);	  
	      // get the chamber (i.e. current strip) response
	      qp=fResponse->IntXY(segmentation);	  
	      
	      if (qp > 0.5) {		
		// --- store signal information for neighbours 
		newclust[0][nnew]=qp;                      // total charge
		newclust[1][nnew]=segmentation->Ix();      // ix-pos. of pad
		newclust[2][nnew]=segmentation->Iy();      // iy-pos. of pad
		newclust[3][nnew]=twentyNano;              // time of flight
		newclust[4][nnew]=segmentation->ISector(); // sector id
		newclust[5][nnew]=(Float_t) i;             // counter
		nnew++;
	      } // qp > 0.5 
	    } // built-up cluster-size
	  } // existing neighbour
	} // loop over neighbours
      } // endif hit w/o strip
    } // loop over planes
  } // if AliMUONResponseTriggerV1
}











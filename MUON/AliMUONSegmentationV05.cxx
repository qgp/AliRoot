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

/////////////////////////////////////////////////////
//  Segmentation and Response classes version 05   //
/////////////////////////////////////////////////////


#include "AliMUONSegmentationV05.h"
#include "AliMUON.h"
#include "AliMUONChamber.h"
#include "AliRun.h"

#include <TMath.h>
//___________________________________________
ClassImp(AliMUONSegmentationV05)


void AliMUONSegmentationV05::Init(Int_t chamber)
{
//
//  Fill the arrays fCx (x-contour) and fNpxS (ix-contour) for each sector
//  These arrays help in converting from real to pad co-ordinates and
//  vice versa
//
//  Segmentation is defined by rectangular modules approximating
//  concentric circles as shown below
//

  //printf("\n Initialise Segmentation V05 \n");


//  PCB module size in cm
    const Float_t kDxPCB=40, kDyPCB=40;
//  PCB distribution (7 rows with 1+3 segmentation regions)
    const Int_t kpcb[7][4] = {{1, 2, 2, 2}, 
			      {0, 3, 2, 2}, 
			      {0, 2, 2, 2}, 
			      {0, 0, 3, 3}, 
			      {0, 0, 2, 3}, 
			      {0, 0, 0, 4}, 
			      {0, 0, 0, 3}};
    
    
//
//                             3 3 3 | 3 3 3
//                           3 3 3 3 | 3 3 3 3
//                         3 3 3 2 2 | 2 2 3 3 3
//                       3 3 3 2 2 2 | 2 2 2 3 3 3
//                       3 3 2 2 1 1 | 1 1 2 2 3 3      
//                     3 3 2 2 1 1 1 | 1 1 1 2 2 3 3
//                     3 3 2 2 1 1 0 | 0 1 1 2 2 3 3
//                    ------------------------------
//                     3 3 2 2 1 1 0 | 0 1 1 2 2 3 3
//                     3 3 2 2 1 1 1 | 1 1 1 2 2 3 3
//                       3 3 2 2 1 1 | 1 1 2 2 3 3      
//                       3 3 3 2 2 2 | 2 2 2 3 3 3                      
//                         3 3 3 2 2 | 2 2 3 3 3
//                           3 3 3 3 | 3 3 3 3
//                             3 3 3 | 3 3 3
//
// number of pad rows per PCB
//    
    Int_t nPyPCB=Int_t(kDyPCB/fDpy);
//
// maximum number of pad rows    
    fNpy=7*nPyPCB;
//
//  Calculate padsize along x
    (*fDpxD)[fNsec-1]=fDpx;
    if (fNsec > 1) {
	for (Int_t i=fNsec-2; i>=0; i--){
	    (*fDpxD)[i]=(*fDpxD)[fNsec-1]/(*fNDiv)[i];
	}
    }
//
// fill the arrays defining the pad segmentation boundaries
//
//  loop over pcb module rows
    Int_t iy=0;
    for (Int_t irow=0; irow<7; irow++) {
//  
//  loop over pads along the anode wires
	for (Int_t i=0; i<=nPyPCB; i++) {
//  iy counts the padrow
	    iy++;
//  Loop over sectors (isec=0 is the dead space surounding the beam pipe)
	    for (Int_t isec=0; isec<4; isec++) {
		if (isec==0) {
		    fNpxS[0][iy]=kpcb[irow][0]*Int_t(kDxPCB/(*fDpxD)[0]);
		    fCx[0][iy]=kpcb[irow][0]*kDxPCB;
		} else {
		    fNpxS[isec][iy]=fNpxS[isec-1][iy]
			+kpcb[irow][isec]*Int_t(kDxPCB/(*fDpxD)[isec]);

		    fCx[isec][iy]=fCx[isec-1][iy]
		    +kpcb[irow][isec]*kDxPCB;
		}
	    } // sectors
	} // pad raws in module
    } // PCB rows

    AliMUON *pMUON  = (AliMUON *) gAlice->GetModule("MUON");
    fChamber=&(pMUON->Chamber(chamber));
    fZ = fChamber->Z();
}

void AliMUONSegmentationV05::GiveTestPoints(Int_t &n, Float_t *x, Float_t *y) const
{
// Returns test point on the pad plane.
// Used during determination of the segmoid correction of the COG-method
    n=1;
    x[0]=(fCx[1][1]+fCx[0][1])/2/TMath::Sqrt(2.);
    y[0]=x[0];
}


















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

// --- MUON includes ---
#include "AliMUONChamber.h"
#include "AliMUONChamberGeometry.h"

// --- ROOT includes ---

#include "TRandom.h"
#include "TMath.h"

ClassImp(AliMUONChamber)	

    AliMUONChamber::AliMUONChamber()
{
// Default constructor
    fSegmentation = 0;
    fResponse=0;
    fnsec=1;
    fReconstruction=0;
    fGeometry = 0;
    
    fId=0;
    // to avoid mistakes if ChargeCorrelInit is not called
    fCurrentCorrel =1;
}

    AliMUONChamber::AliMUONChamber(Int_t id) 
{
// Construtor with chamber id 
    fSegmentation = new TObjArray(2);
    fSegmentation->AddAt(0,0);
    fSegmentation->AddAt(0,1);
    fResponse=0;
    fGeometry = new AliMUONChamberGeometry(fId);
    fnsec=1;
    fReconstruction=0;
    fId=id;
    // to avoid mistakes if ChargeCorrelInit is not called
    fCurrentCorrel =1;
}

AliMUONChamber::~AliMUONChamber() 
{
// Destructor
  if (fSegmentation) {
    fSegmentation->Delete();
    delete fSegmentation;
  }
}

AliMUONChamber::AliMUONChamber(const AliMUONChamber& rChamber):TObject(rChamber)
{
 // Dummy copy constructor
     ;
}


Bool_t  AliMUONChamber::IsSensId(Int_t volId) const 
{
// Returns true if the volume specified by volId is in the list
// of sesitive volumes for this chamber

  return fGeometry->IsSensitiveVolume(volId);
}  

void AliMUONChamber::Init()
{
// Initalisation ..
//
// ... for chamber segmentation
  //PH    if ((*fSegmentation)[0]) 
  //PH    ((AliSegmentation *) (*fSegmentation)[0])->Init(fId);
    if (fSegmentation->At(0)) 
    ((AliSegmentation *) fSegmentation->At(0))->Init(fId);

    if (fnsec==2) {
      //PH	if ((*fSegmentation)[1])
      //PH	((AliSegmentation *) (*fSegmentation)[1])->Init(fId);
	if (fSegmentation->At(1))
	((AliSegmentation *) fSegmentation->At(1))->Init(fId);
    }
}

Int_t   AliMUONChamber::SigGenCond(Float_t x, Float_t y, Float_t z)
{
// Ask segmentation if signal should be generated 
    if (fnsec==1) {
      //PH	return ((AliSegmentation*) (*fSegmentation)[0])
	return ((AliSegmentation*) fSegmentation->At(0))
	    ->SigGenCond(x, y, z) ;
    } else {
      //PH	return (((AliSegmentation*) (*fSegmentation)[0])
	return (((AliSegmentation*) fSegmentation->At(0))
		->SigGenCond(x, y, z)) ||
      //PH	    (((AliSegmentation*) (*fSegmentation)[1])
	    (((AliSegmentation*) fSegmentation->At(1))
	     ->SigGenCond(x, y, z)) ;
    }
}


void    AliMUONChamber::SigGenInit(Float_t x, Float_t y, Float_t z)
{
//
// Initialisation of segmentation for hit
//  
    if (fnsec==1) {
      //PH	((AliSegmentation*) (*fSegmentation)[0])->SigGenInit(x, y, z) ;
	((AliSegmentation*) fSegmentation->At(0))->SigGenInit(x, y, z) ;
    } else {
      //PH	((AliSegmentation*) (*fSegmentation)[0])->SigGenInit(x, y, z) ;
      //PH	((AliSegmentation*) (*fSegmentation)[1])->SigGenInit(x, y, z) ;
	((AliSegmentation*) fSegmentation->At(0))->SigGenInit(x, y, z) ;
	((AliSegmentation*) fSegmentation->At(1))->SigGenInit(x, y, z) ;
    }
}

void AliMUONChamber::ChargeCorrelationInit() {
// Initialisation of charge correlation for current hit
// the value is stored, and then used by Disintegration
if (fnsec==1) 
    fCurrentCorrel =1;
else 
    // exponential is here to avoid eventual problems in 0 
    // factor 2 because chargecorrel is q1/q2 and not q1/qtrue
    fCurrentCorrel = TMath::Exp(gRandom->Gaus(0,fResponse->ChargeCorrel()/2));
}

void AliMUONChamber::DisIntegration(Float_t eloss, Float_t /*tof*/, 
				    Float_t xhit, Float_t yhit, Float_t zhit,
				    Int_t& nnew,Float_t newclust[6][500]) 
{
//    
//  Generates pad hits (simulated cluster) 
//  using the segmentation and the response model 
    Float_t dx, dy;
    //
    // Width of the integration area
    //
    dx=fResponse->SigmaIntegration()*fResponse->ChargeSpreadX();
    dy=fResponse->SigmaIntegration()*fResponse->ChargeSpreadY();
    //
    // Get pulse height from energy loss
    Float_t qtot = fResponse->IntPH(eloss);
    //
    // Loop Over Pads
    
    Float_t qcheck=0, qp;
    nnew=0;
    
    // Cathode plane loop
    for (Int_t i=1; i<=fnsec; i++) {
	qcheck=0;
	Float_t qcath = qtot * (i==1? fCurrentCorrel : 1/fCurrentCorrel);
	AliSegmentation * segmentation=
      //PH	    (AliSegmentation *) (*fSegmentation)[i-1];
	    (AliSegmentation *) fSegmentation->At(i-1);
	for (segmentation->FirstPad(xhit, yhit, zhit, dx, dy); 
	     segmentation->MorePads(); 
	     segmentation->NextPad()) 
	{
	    qp=fResponse->IntXY(segmentation);
	    qp=TMath::Abs(qp);
//
//
	    if (qp > 1.e-4) {
		qcheck+=qp*qcath;
	    //
	    // --- store signal information
		newclust[0][nnew]=qcath;                     // total charge
		newclust[1][nnew]=segmentation->Ix();       // ix-position of pad
		newclust[2][nnew]=segmentation->Iy();       // iy-position of pad
		newclust[3][nnew]=qp * qcath;                // charge on pad
		newclust[4][nnew]=segmentation->ISector();  // sector id
		newclust[5][nnew]=(Float_t) i;              // counter
		nnew++;
//		if (i==2) printf("\n i, nnew, q %d %d %f", i, nnew, qp*qcath);
		
	    }
	} // Pad loop
	
    } // Cathode plane loop
}



void AliMUONChamber::InitGeo(Float_t /*zpos*/)
{
//    sensitive gas gap
      fdGas= 0.5;
//    3% radiation length of aluminum (X0=8.9 cm)      
      fdAlu= 3.0/100*8.9;
}


AliMUONChamber & AliMUONChamber::operator =(const AliMUONChamber& /*rhs*/)
{
// Dummy assignment operator
    return *this;
}

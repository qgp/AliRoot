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


// Generator for particles in a preset
// kinematic range (flat distribution)
// Note that for a given theta pt and p are not independent 
// Range for only one variable (pt or p) should be given.
// Comments and suggestions: andreas.morsch@cern.ch


#include "TPDGCode.h"

#include "AliConst.h"
#include "AliGenBox.h"
#include "AliRun.h"
#include "AliGenEventHeader.h"

ClassImp(AliGenBox)

//_____________________________________________________________________________
AliGenBox::AliGenBox()
    :AliGenerator(), 
     fIpart(0)
{
  //
  // Default constructor
  //
}

//_____________________________________________________________________________
AliGenBox::AliGenBox(Int_t npart)
    :AliGenerator(npart),
     fIpart(kProton)
{
  //
  // Standard constructor
  //
  fName  = "Box";
  fTitle = "Box particle generator";
}

//_____________________________________________________________________________

void AliGenBox::Generate()
{
  //
  // Generate one trigger
  //
  
    Float_t polar[3]= {0,0,0};
  //
    Float_t origin[3];
    Float_t p[3];
    Int_t i, j, nt;
    Double_t pmom, theta, phi, pt;
    //
    Float_t random[6];
  //
    for (j=0;j<3;j++) origin[j]=fOrigin[j];
    if(fVertexSmear==kPerEvent) {
	Vertex();
	for (j=0;j<3;j++) origin[j]=fVertex[j];
    }

    for(i=0;i<fNpart;i++) {
	Rndm(random,3);
	theta=fThetaMin+random[0]*(fThetaMax-fThetaMin);
	if(TestBit(kMomentumRange)) {
	    pmom=fPMin+random[1]*(fPMax-fPMin);
	    pt=pmom*TMath::Sin(theta);
	} else {

	    pt=fPtMin+random[1]*(fPtMax-fPtMin);
	    pmom=pt/TMath::Sin(theta);
	}
	phi=fPhiMin+random[2]*(fPhiMax-fPhiMin);
	p[0] = pt*TMath::Cos(phi);
	p[1] = pt*TMath::Sin(phi);
	p[2] = pmom*TMath::Cos(theta);

	if(fVertexSmear==kPerTrack) {
	    Rndm(random,6);
	    for (j=0;j<3;j++) {
		origin[j]=fOrigin[j]+fOsigma[j]*TMath::Cos(2*random[2*j]*TMath::Pi())*
		    TMath::Sqrt(-2*TMath::Log(random[2*j+1]));
	    }
	}
	PushTrack(fTrackIt,-1,fIpart,p,origin,polar,0,kPPrimary,nt);
    }

    AliGenEventHeader* header = new AliGenEventHeader("BOX");
    header->SetPrimaryVertex(fVertex);
    header->SetNProduced(fNpart);
    
 // Passes header either to the container or to gAlice
    if (fContainer) {
	fContainer->AddHeader(header);
    } else {
	gAlice->SetGenEventHeader(header);	
    }
}

//_____________________________________________________________________________

void AliGenBox::Init()
{
// Initialisation, check consistency of selected ranges
  if(TestBit(kPtRange)&&TestBit(kMomentumRange)) 
    Fatal("Init","You should not set the momentum range and the pt range!\n");
  if((!TestBit(kPtRange))&&(!TestBit(kMomentumRange))) 
    Fatal("Init","You should set either the momentum or the pt range!\n");
}


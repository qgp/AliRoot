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
//
// Utility class to make simple Glauber type calculations for collision geometries:
// Impact parameter, production points, reaction plane dependence
// The SimulateTrigger method can be used for simple MB and hard-process
// (binary scaling) trigger studies.
// Some basic quantities can be visualized directly.
// The default set-up for PbPb collisions can be read from a file calling Init(1).
//
// 
// Author: andreas.morsch@cern.ch

// from AliRoot
#include "AliFastGlauber.h"
// from root
#include <TH1F.h>
#include <TF1.h>
#include <TF2.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TFile.h>

ClassImp(AliFastGlauber)

TF1*    AliFastGlauber::fgWSb            = NULL;     
TF2*    AliFastGlauber::fgWSbz           = NULL;    
TF1*    AliFastGlauber::fgWSz            = NULL;     
TF1*    AliFastGlauber::fgWSta           = NULL;    
TF2*    AliFastGlauber::fgWStarfi        = NULL; 
TF2*    AliFastGlauber::fgWAlmond        = NULL; 
TF1*    AliFastGlauber::fgWStaa          = NULL;   
TF1*    AliFastGlauber::fgWSgeo          = NULL;   
TF1*    AliFastGlauber::fgWSbinary       = NULL;   
TF1*    AliFastGlauber::fgWSN            = NULL;   
TF1*    AliFastGlauber::fgWPathLength0   = NULL;   
TF1*    AliFastGlauber::fgWPathLength    = NULL;
TF1*    AliFastGlauber::fgWEnergyDensity = NULL;   
TF1*    AliFastGlauber::fgWIntRadius     = NULL;   
Float_t AliFastGlauber::fgBMax           = 0.;

AliFastGlauber::AliFastGlauber()
{
//  Default Constructor
//
//  Defaults for Pb
    SetWoodSaxonParameters(6.624, 0.549, 0.00, 7.69e-4);
    SetHardCrossSection();
    SetMaxImpact();
}

void AliFastGlauber::Init(Int_t mode)
{
// Initialisation
//
//  Wood-Saxon
//
    fgWSb = new TF1("WSb", WSb, 0, fgBMax, 4);
    fgWSb->SetParameter(0, fWSr0);
    fgWSb->SetParameter(1, fWSd);
    fgWSb->SetParameter(2, fWSw);
    fgWSb->SetParameter(3, fWSn);

    fgWSbz = new TF2("WSbz", WSbz, 0, fgBMax, 4);
    fgWSbz->SetParameter(0, fWSr0);
    fgWSbz->SetParameter(1, fWSd);
    fgWSbz->SetParameter(2, fWSw);
    fgWSbz->SetParameter(3, fWSn);

    fgWSz = new TF1("WSz", WSz, 0, fgBMax, 5);
    fgWSz->SetParameter(0, fWSr0);
    fgWSz->SetParameter(1, fWSd);
    fgWSz->SetParameter(2, fWSw);
    fgWSz->SetParameter(3, fWSn);

//
//  Thickness
//
    fgWSta = new TF1("WSta", WSta, 0., fgBMax, 0);
    
//
//  Overlap Kernel
//
    fgWStarfi = new TF2("WStarfi", WStarfi, 0., fgBMax, 0., TMath::Pi(), 1);
    fgWStarfi->SetParameter(0, 0.);     
    fgWStarfi->SetNpx(200);     
    fgWStarfi->SetNpy(20);     
//
//  Almond shaped interaction region
//
    fgWAlmond = new TF2("WAlmond", WAlmond, -fgBMax, fgBMax, -fgBMax, fgBMax, 1);
    fgWAlmond->SetParameter(0, 0.);     
    fgWAlmond->SetNpx(200);     
    fgWAlmond->SetNpy(200);    
//
//  Path Length as a function of Phi
//    
    fgWPathLength0 = new TF1("WPathLength0", WPathLength0, -TMath::Pi(), TMath::Pi(), 2);
    fgWPathLength0->SetParameter(0, 0.);
//  Pathlength definition     
    fgWPathLength0->SetParameter(1, 0.);     

    fgWPathLength = new TF1("WPathLength", WPathLength, -TMath::Pi(), TMath::Pi(), 3);
//  Impact Parameter
    fgWPathLength->SetParameter(0, 0.);    
//  Number of interactions used for average
    fgWPathLength->SetParameter(1, 1000.);    
//  Pathlength definition
    fgWPathLength->SetParameter(2, 0);    

    fgWIntRadius = new TF1("WIntRadius", WIntRadius, 0., fgBMax, 1);
    fgWIntRadius->SetParameter(0, 0.);    


//
//  Overlap
//
    if (! mode) {
	fgWStaa = new TF1("WStaa", WStaa, 0., fgBMax, 0);
	fgWStaa->SetNpx(100);
    } else {
	TFile* f = new TFile("$(ALICE_ROOT)/FASTSIM/data/glauberPbPb.root");
	fgWStaa = (TF1*) f->Get("WStaa");
    }
    
//
    fgWEnergyDensity = new TF1("WEnergyDensity", WEnergyDensity, 0., 2. * fWSr0, 1);
    fgWEnergyDensity->SetParameter(0, fWSr0 + 1.);
    
//
//  Geometrical Cross-Section
//
    fgWSgeo = new TF1("WSgeo", WSgeo, 0., fgBMax, 0);
    fgWSgeo->SetNpx(100);
//
//  Hard cross section (~ binary collisions)
//
    fgWSbinary = new TF1("WSbinary", WSbinary, 0., fgBMax, 1);
    fgWSbinary->SetParameter(0, fSigmaHard); // mb
    fgWSbinary->SetNpx(100);
//
// Hard collisions per event
//
    fgWSN = new TF1("WSN", WSN, 0., fgBMax, 1);
    fgWSN->SetNpx(100);
}

void AliFastGlauber::DrawWSb()
{
//
//  Draw Wood-Saxon Nuclear Density Function
//
    TCanvas *c1 = new TCanvas("c1","Wood Saxon",400,10,600,700);
    c1->cd();
    fgWSb->Draw();
}

void AliFastGlauber::DrawOverlap()
{
//
//  Draw Overlap Function
//
    TCanvas *c2 = new TCanvas("c2","Overlap",400,10,600,700);
    c2->cd();
    fgWStaa->Draw();
}

void AliFastGlauber::DrawThickness()
{
//
//  Draw Thickness Function
//
    TCanvas *c3 = new TCanvas("c3","Thickness",400,10,600,700);
    c3->cd();
    fgWSta->Draw();
}

void AliFastGlauber::DrawGeo()
{
//
//  Draw Geometrical Cross-Section
//
    TCanvas *c3 = new TCanvas("c3","Geometrical Cross-Section",400,10,600,700);
    c3->cd();
    fgWSgeo->Draw();
}

void AliFastGlauber::DrawBinary()
{
//
//  Draw Wood-Saxon Nuclear Density Function
//
    TCanvas *c4 = new TCanvas("c4","Binary Cross-Section",400,10,600,700);
    c4->cd();
    fgWSbinary->Draw();
}

void AliFastGlauber::DrawN()
{
//
//  Draw Binaries per event
//
    TCanvas *c5 = new TCanvas("c5","Binaries per event",400,10,600,700);
    c5->cd();
    fgWSN->Draw();
}

void AliFastGlauber::DrawKernel(Double_t b)
{
//
//  Draw Kernel
//
    TCanvas *c6 = new TCanvas("c6","Kernel",400,10,600,700);
    c6->cd();
    fgWStarfi->SetParameter(0, b);
    fgWStarfi->Draw();
}

void AliFastGlauber::DrawAlmond(Double_t b)
{
//
//  Draw Interaction Almond
//
    TCanvas *c7 = new TCanvas("c7","Almond",400,10,600,700);
    c7->cd();
    fgWAlmond->SetParameter(0, b);
    fgWAlmond->Draw();
}

void AliFastGlauber::DrawPathLength0(Double_t b, Int_t iopt)
{
//
//  Draw Path Length
//
    TCanvas *c8 = new TCanvas("c8","Path Length",400,10,600,700);
    c8->cd();
    fgWPathLength0->SetParameter(0, b);
    fgWPathLength0->SetParameter(1, Double_t(iopt));
    fgWPathLength0->SetMinimum(0.); 
    fgWPathLength0->SetMaximum(10.); 
    fgWPathLength0->Draw();
}

void AliFastGlauber::DrawPathLength(Double_t b , Int_t ni, Int_t iopt)
{
//
//  Draw Path Length
//
    TCanvas *c9 = new TCanvas("c9","Path Length",400,10,600,700);
    c9->cd();
    fgWAlmond->SetParameter(0, b);

    fgWPathLength->SetParameter(0, b);
    fgWPathLength->SetParameter(1, Double_t (ni));
    fgWPathLength->SetParameter(2, Double_t (iopt));
    fgWPathLength->SetMinimum(0.); 
    fgWPathLength->SetMaximum(10.); 
    fgWPathLength->Draw();
}

void AliFastGlauber::DrawIntRadius(Double_t b)
{
//
//  Draw Interaction Radius
//
    TCanvas *c10 = new TCanvas("c10","Interaction Radius",400,10,600,700);
    c10->cd();
    fgWIntRadius->SetParameter(0, b);
    fgWIntRadius->SetMinimum(0.);
    fgWIntRadius->Draw();
}

void AliFastGlauber::DrawEnergyDensity()
{
//
//  Draw energy density
//
    TCanvas *c11 = new TCanvas("c11","Energy Density",400, 10, 600, 700);
    c11->cd();
    fgWEnergyDensity->SetMinimum(0.);
    fgWEnergyDensity->Draw();
}

Double_t AliFastGlauber::WSb(Double_t* x, Double_t* par)
{
//
//  Woods-Saxon Parameterisation
//  as a function of radius
//
    Double_t xx  = x[0];
    Double_t r0  = par[0];
    Double_t d   = par[1];
    Double_t w   = par[2];
    Double_t n   = par[3];
    
    Double_t y  = n * (1.+w*(xx/r0)*(xx/r0))/(1.+TMath::Exp((xx-r0)/d));

    return y;
}

Double_t AliFastGlauber::WSbz(Double_t* x, Double_t* par)
{
//
//  Wood Saxon Parameterisation
//  as a function of z and  b
//
    Double_t bb  = x[0];
    Double_t zz  = x[1];
    Double_t r0  = par[0];
    Double_t d   = par[1];
    Double_t w   = par[2];
    Double_t n   = par[3];
    Double_t xx  = TMath::Sqrt(bb*bb+zz*zz);
    Double_t y  = n * (1.+w*(xx/r0)*(xx/r0))/(1.+TMath::Exp((xx-r0)/d));

    return y;
}

Double_t AliFastGlauber::WSz(Double_t* x, Double_t* par)
{
//
//  Wood Saxon Parameterisation
//  as a function of z for fixed b
//
    Double_t bb  = par[4];
    Double_t zz  = x[0];
    Double_t r0  = par[0];
    Double_t d   = par[1];
    Double_t w   = par[2];
    Double_t n   = par[3];
    Double_t xx  = TMath::Sqrt(bb*bb+zz*zz);
    Double_t y  = n * (1.+w*(xx/r0)*(xx/r0))/(1.+TMath::Exp((xx-r0)/d));

    return y;
}

Double_t AliFastGlauber::WSta(Double_t* x, Double_t* /*par*/)
{
//
//  Thickness function 
//
    Double_t b  = x[0];
    fgWSz->SetParameter(4, b);
    Double_t y  = 2. * fgWSz->Integral(0., fgBMax);
    return y;
}



Double_t AliFastGlauber::WStarfi(Double_t* x, Double_t* par)
{
//
//  Kernel for overlap function
//
    Double_t b    = par[0];
    Double_t r1   = x[0];
    Double_t phi  = x[1];
    Double_t r2   = TMath::Sqrt(r1 * r1 + b * b - 2. * r1 * b * TMath::Cos(phi)); 
    Double_t y    = r1 * fgWSta->Eval(r1) * fgWSta->Eval(r2);
    return y;
}


Double_t AliFastGlauber::WAlmond(Double_t* x, Double_t* par)
{
//
//  Almond shaped interaction region
//
    Double_t b    = par[0];
    Double_t xx   = x[0] + b/2.;
    Double_t yy   = x[1];
    Double_t r1   = TMath::Sqrt(xx * xx + yy * yy);
    Double_t phi  = TMath::ATan2(yy,xx);
    
    Double_t r2   = TMath::Sqrt(r1 * r1 + b * b - 2. * r1 * b * TMath::Cos(phi)); 
//
//  Interaction probability calculated as product of thicknesses
//
    Double_t y    = fgWSta->Eval(r1) * fgWSta->Eval(r2);
    return y;
}

Double_t AliFastGlauber::WIntRadius(Double_t* x, Double_t* par)
{
//
//  Average radius at which interaction takes place
//
//  Radius in the Almond
    Double_t r    = x[0];
//  Impact parameter
    Double_t b    = par[0];
    fgWAlmond->SetParameter(0, b);
//  Steps in phi
    Double_t dphi = 2. * TMath::Pi() / 100.;
//  Average over phi    
    Double_t phi  = 0.;
    Double_t y    = 0.;

    for (Int_t i = 0; i < 100; i++) {
	Double_t xx = r * TMath::Cos(phi);
	Double_t yy = r * TMath::Sin(phi);
	y   += fgWAlmond->Eval(xx,yy);
	phi += dphi;
    } // phi loop
// Result multiplied by Jacobian (2 pi r)     
    return (2. * TMath::Pi() * y * r / 100.);
}

Double_t AliFastGlauber::WPathLength0(Double_t* x, Double_t* par)
{
//
//  Path Length as a function of phi for interaction point fixed at (0,0)
//
//
//  Steps in r 
    const Int_t    kNp  = 100;
    const Double_t kDr  = fgBMax/Double_t(kNp);
//  Impact parameter    
    Double_t b      = par[0];
//  Path Length definition
    Int_t    iopt   = Int_t(par[1]);
    
//  Phi direction in Almond
    Double_t phi0   = x[0];
    Double_t r  = 0.;
    Double_t rw = 0.;
    Double_t w  = 0.;
//  Step along radial direction phi   
    for (Int_t i = 0; i < kNp; i++) {
//
//  Transform into target frame
//
	Double_t xx   = r * TMath::Cos(phi0) + b / 2.;
	Double_t yy   = r * TMath::Sin(phi0);
	Double_t phi  = TMath::ATan2(yy, xx);
	
	Double_t r1   = TMath::Sqrt(xx * xx + yy * yy);
// Radius in projectile frame
	Double_t r2   = TMath::Sqrt(r1 * r1 + b * b - 2. * r1 * b * TMath::Cos(phi)); 
	Double_t y    = fgWSta->Eval(r1) * fgWSta->Eval(r2);

	rw += y * r;
	w  += y;
	r  += kDr;
    } // radial steps
//
//  My length definition (is exact for hard disk)
    if (!iopt) {
	return (2. * rw / w);
    } else {
	return TMath::Sqrt(2. * rw * kDr / fgWSta->Eval(0.01) / fgWSta->Eval(0.01));
    }
}

Double_t AliFastGlauber::WPathLength(Double_t* x, Double_t* par)
{
//
//  Path Length as a function of phi 
//  Interaction point from random distribution
//
//
//  r-steps
// 
    const Int_t    kNp   = 100;
    const Double_t kDr  = fgBMax/Double_t(kNp);
//  Number of interactions
    const Int_t    kNpi  = Int_t (par[1]);

//
//  Impact parameter    
    Double_t b      = par[0];
//  Path Length definition 
    Int_t    iopt   = Int_t(par[2]);
//  Phi direction
    Double_t phi0   = x[0];

    printf("phi0 %f \n", phi0);
    
//  Path length 
    Double_t l = 0.;
    
    for (Int_t in = 0; in < kNpi; in ++) {
	Double_t rw = 0.;
	Double_t w  = 0.;
	
	// Interaction point
	Double_t x0, y0;
	fgWAlmond->GetRandom2(x0, y0);
// Initial radius
	Double_t r0  = TMath::Sqrt(x0 * x0 + y0 * y0);
	Int_t    nps = Int_t ((fgBMax - r0)/kDr) - 1;
	
	Double_t r  = 0.;
// Radial steps
	for (Int_t i = 0; (i < nps ); i++) {
	    
// Transform into target frame
	    Double_t xx   = x0 + r * TMath::Cos(phi0) + b / 2.;
	    Double_t yy   = y0 + r * TMath::Sin(phi0);
	    Double_t phi  = TMath::ATan2(yy, xx);
	    Double_t r1   = TMath::Sqrt(xx * xx + yy * yy);
// Radius in projectile frame
	    Double_t r2   = TMath::Sqrt(r1 * r1 + b * b - 2. * r1 * b * TMath::Cos(phi)); 
	    Double_t y    = fgWSta->Eval(r1) * fgWSta->Eval(r2);
	    
	    rw += y * r;
	    w  += y;
	    r  += kDr;
	} // steps
// Average over interactions
	if (!iopt) {
	    l += (2. * rw / w);
	} else {
	    l+= 2. * rw * kDr / fgWSta->Eval(0.01) / fgWSta->Eval(0.01);
	}
    } // interactions
    if (!iopt) 
	return (l / Double_t(kNpi));
    else 
	return (TMath::Sqrt(l / Double_t(kNpi)));
}

Double_t AliFastGlauber::WStaa(Double_t* x, Double_t* /*par*/)
{
//
//  Overlap function
//
    Double_t b    = x[0];
    fgWStarfi->SetParameter(0, b);
/*
    Double_t al[2];
    Double_t bl[2];
    al[0] = 0.;
    al[1] = 0.;
    bl[0] = 6.6;
    bl[1] = TMath::Pi();
    Double_t err;
    
    Double_t y =  2. * fgWStarfi->IntegralMultiple(2, al, bl, 0.001, err);
    printf("WStaa: %f %f %f\n", b, y, err);
*/
//
//  MC Integration
//
    Double_t y = 0;
    for (Int_t i = 0; i < 100000; i++)
    {
	Double_t phi = TMath::Pi() * gRandom->Rndm();
	Double_t b1  = fgBMax       * gRandom->Rndm();	
	y += fgWStarfi->Eval(b1, phi);
    }
    y *= 2. * 0.1 *  208. * 208. * TMath::Pi() * fgBMax / 100000.;
    return y;
}

Double_t AliFastGlauber::WSgeo(Double_t* x, Double_t* /*par*/)
{
//
//  Geometrical Cross-Section
//
    Double_t b    = x[0];
    Double_t taa  = fgWStaa->Eval(b);
    const Double_t kSigma = 55.6; // mbarn
    
    Double_t y    = 2. * TMath::Pi() * b * (1. - TMath::Exp(- kSigma * taa)); // fm
    return y;
}


Double_t AliFastGlauber::WSbinary(Double_t* x, Double_t* par)
{
//
//  Number of binary collisions
//
    Double_t b     = x[0];
    Double_t sigma = par[0];
    Double_t taa   = fgWStaa->Eval(b);
    
    Double_t y    = 2. * TMath::Pi() * b * sigma * taa; // fm
    return y;
}

Double_t AliFastGlauber::WSN(Double_t* x, Double_t* /*par*/)
{
//
//  Number of hard processes per event
//
    Double_t b     = x[0];
    Double_t y     = fgWSbinary->Eval(b)/fgWSgeo->Eval(b);
    return y;
}

Double_t AliFastGlauber::WEnergyDensity(Double_t* x, Double_t* par)
{
//
//  Initial energy density as a function of the impact parameter
//
    Double_t b     = x[0];
    Double_t rA    = par[0];
//
//  Attention: area of transverse reaction zone in hard-sphere approximation !     
    Double_t saa   = (TMath::Pi() - 2. * TMath::ASin(b/ 2./ rA)) * rA * rA 
	- b * TMath::Sqrt(rA * rA - b * b/ 4.);
    Double_t taa   = fgWStaa->Eval(b);
    
    return (taa/saa);
}

void AliFastGlauber::SimulateTrigger(Int_t n)
{
    //
    //  Simulates Trigger
    //
    TH1F* mbtH = new TH1F("mbtH", "MB Trigger b-Distribution",   100, 0., 20.);
    TH1F* hdtH = new TH1F("hdtH", "Hard Trigger b-Distribution", 100, 0., 20.);   
    TH1F* mbmH = new TH1F("mbmH", "MB Trigger Multiplicity Distribution",   100, 0., 8000.);
    TH1F* hdmH = new TH1F("hdmH", "Hard Trigger Multiplicity Distribution", 100, 0., 8000.);   

    mbtH->SetXTitle("b [fm]");
    hdtH->SetXTitle("b [fm]");    
    mbmH->SetXTitle("Multiplicity");
    hdmH->SetXTitle("Multiplicity");    

    TCanvas *c0 = new TCanvas("c0","Trigger Simulation",400,10,600,700);    
    c0->Divide(2,1);
    TCanvas *c1 = new TCanvas("c1","Trigger Simulation",400,10,600,700);    
    c1->Divide(1,2);

    //
    //
    Init(1);
    for (Int_t iev = 0; iev < n; iev++)
    {
	Float_t b, p, mult;
	GetRandom(b, p, mult);
	mbtH->Fill(b,1.);
	hdtH->Fill(b, p);
	mbmH->Fill(mult, 1.);
	hdmH->Fill(mult, p);

	c0->cd(1);
	mbtH->Draw();
	c0->cd(2);
	hdtH->Draw();	
	c0->Update();

	c1->cd(1);
	mbmH->Draw();
	c1->cd(2);
	hdmH->Draw();	
	c1->Update();
    }
}

void AliFastGlauber::GetRandom(Float_t& b, Float_t& p, Float_t& mult)
{
    //
    // Gives back a random impact parameter, hard trigger probability and multiplicity
    //
	b = fgWSgeo->GetRandom();
	Float_t mu = fgWSN->Eval(b);
	p = 1.-TMath::Exp(-mu);
	mult = 6000./fgWSN->Eval(1.) * mu;
}

void AliFastGlauber::GetRandom(Int_t& bin, Bool_t& hard)
{
    //
    // Gives back a random impact parameter bin, and hard trigger decission
    //
	Float_t b  = fgWSgeo->GetRandom();
	Float_t mu = fgWSN->Eval(b) * fSigmaHard;
	Float_t p  = 1.-TMath::Exp(-mu);
	if (b < 5.) {
	    bin = 1;
	} else if (b <  8.6) {
	    bin = 2;
	} else if (b < 11.2) {
	    bin = 3;
	} else if (b < 13.2) {
	    bin = 4;
	} else if (b < 15.0) {
	    bin = 5;
	} else {
	    bin = 6;
	}
	
	hard = kFALSE;
	
	Float_t r = gRandom->Rndm();
	
	if (r < p) hard = kTRUE;
}


Float_t  AliFastGlauber::GetRandomImpactParameter(Float_t bmin, Float_t bmax)
{
    //
    // Gives back a random impact parameter in the range bmin .. bmax
    //

    Float_t b = -1.;
    while(b < bmin || b > bmax)
	b = fgWSgeo->GetRandom();
    return b;
}

Double_t AliFastGlauber::CrossSection(Double_t b1, Double_t b2)
{
    //
    // Return cross-section integrated from b1 to b2 
    //
    
    return fgWSgeo->Integral(b1, b2)/100.;
}

Double_t AliFastGlauber::FractionOfHardCrossSection(Double_t b1, Double_t b2)
{
    //
    // Return raction of hard cross-section integrated from b1 to b2 
    //
    
    return fgWSbinary->Integral(b1, b2)/fgWSbinary->Integral(0., 100.);
}


Double_t AliFastGlauber::Binaries(Double_t b)
{
    //
    // Return number of binary collisions normalized to 1 at b=0
    //
    
    return fgWSN->Eval(b)/fgWSN->Eval(0.001);
}
